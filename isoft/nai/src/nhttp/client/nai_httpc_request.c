// Disclaimer
//
// This work (specification and/or software implementation) and the material
// contained in it, as released by AUTOSAR, is for the purpose of information
// only. AUTOSAR and the companies that have contributed to it shall not be
// liable for any use of the work.
//
// The material contained in this work is protected by copyright and other
// types of intellectual property rights. The commercial exploitation of the
// material contained in this work requires a license to such intellectual
// property rights.
//
// This work may be utilized or reproduced without any modification, in any
// form or by any means, for informational purposes only. For any other
// purpose, no part of the work may be utilized or reproduced, in any form
// or by any means, without permission in writing from the publisher.
//
// The work has been developed for automotive applications only. It has
// neither been developed, nor tested for non-automotive applications.
//
// The word AUTOSAR and the AUTOSAR logo are registered trademarks.
// --------------------------------------------------------------------------

/// ================================================================
///
/// File description:
/// ----------------
/// @file       nai_httpc_request.c
/// @brief      
/// @details
/// @date       2021-08-19
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#include "nai/runtime/nai_errno.h"
#include "nai/runtime/nai_log.h"
#include "nai/service/nai_main_event.h"
#include "nai_httpc_core.h"



#define NAI_HTTPC_SIGNAL            3


#define NAI_HTTPC_STAT_NONE         0
#define NAI_HTTPC_STAT_CONNECT      1
#define NAI_HTTPC_STAT_REQUEST      2
#define NAI_HTTPC_STAT_FINALIZE     3
#define NAI_HTTPC_STAT_DONE         4



static nai_int_t nai_httpc_request_event(nai_evnode_t* e, nai_int_t events);
static nai_sult_t nai_httpc_prepare_method(nai_httpc_request_t* r);
static nai_sult_t nai_httpc_prepare_uri(nai_httpc_request_t* r);
static nai_sult_t nai_httpc_prepare_headers_out(nai_httpc_request_t* r);
static nai_sult_t nai_httpc_prepare_request(nai_httpc_request_t* r);
static nai_sult_t nai_httpc_request_flush(nai_httpc_request_t* r);
static nai_sult_t nai_httpc_request_discard(nai_httpc_request_t* r);
static nai_sult_t nai_httpc_request_connect(nai_httpc_request_t* r);
static nai_sult_t nai_httpc_request_run(nai_httpc_request_t* r);
static nai_sult_t nai_httpc_request_wait(nai_httpc_request_t* r);
static nai_sult_t nai_httpc_request_finish(nai_httpc_request_t* r);
static nai_sult_t nai_httpc_request_finishing(nai_httpc_request_t* r);



//////////////////////////////////////////////////////////////////////////////
// httpc request


#define nai_httpc_headers_count(t, first, last) ((      \
    nai_offsetof(t, last) - nai_offsetof(t, first)) /   \
    sizeof(void*) + 1)                                  \


#define nai_httpc_headers_in_count()                    \
    nai_httpc_headers_count(nai_httpc_headers_in_t,     \
    content_length, etag)                               \


#define nai_httpc_headers_out_count()                   \
    nai_httpc_headers_count(nai_httpc_headers_out_t,    \
    content_length, keep_alive)                         \



nai_sult_t nai_httpc_init(nai_httpc_request_t* r)
{
    nai_sult_t rc;


    /* init event node */
    nai_evnode_init(&r->ev);

    /* init pool */
    r->pool = 0;
    r->bufpool = 0;

    /* init input headers */
    r->headers_in.flags = 0;
    r->headers_in.version = 0;
    r->headers_in.keep_alive_timeo = -1;
    r->headers_in.content_length_n = -1;
    nai_pzero((void**)&r->headers_in.ent, nai_httpc_headers_in_count());
    nai_list_init(&r->headers_in.list);
    nai_array_init(&r->headers_in.cookies, 
        sizeof(nai_httpc_header_t*), r->pool);

    /* init output headers */
    r->headers_out.flags = 0;
    r->headers_out.keep_alive_n = -1;
    r->headers_out.content_length_n = -1;
    nai_pzero((void**)&r->headers_out.ent, nai_httpc_headers_out_count());
    nai_list_init(&r->headers_out.list);

    /* init request */
    r->method = NAI_HTTP_UNKNOWN;
    r->version = 0;
    nai_str_setn(&r->method_name);
    nai_str_setn(&r->protocol);
    nai_str_setn(&r->unparsed_uri);
    nai_str_setn(&r->uri);
    nai_str_setn(&r->args);
    nai_str_setn(&r->exten);
    nai_uri_init(&r->parsed_uri);

    /* init response */
    r->status = 0;
    nai_str_setn(&r->status_text);
    nai_buflist_init(&r->content, r->bufpool);

    /* read and send handle */
    r->read = 0;
    r->send = 0;
    r->event = 0;

    /* init status */
    r->stage = 0;
    r->flags = 0;
    r->keepalive = 1;
    r->lingering_close = 1;
    r->redirects = 15;

    /* init local and conf */
    r->local = 0;
    nai_pzero((void**)r->clocal, nai_countof(r->clocal));

    /* init regex and values */
    r->values = 0;
#if (NAI_HAVE_REGEX)
    r->ncapture = 0;
    r->captures = 0;
    r->match_data = 0;
#endif

    /* init times */
    r->msec = 1000;
    r->start_msec = 0;

    /* init connection */
    r->connection = 0;
    r->proto = 0;
    r->client = 0;
    rc = 0;

    return rc;
};


nai_sult_t nai_httpc_open(
    nai_httpc_request_t* r, 
    nai_httpc_client_t* c, nai_int_t flags, nai_pool_t* pool)
{
    nai_sult_t rc;
    nai_pool_t temp;


    if (c == 0 || 
        c->agent == 0) {
        rc = nai_errno_to_sult(EINVAL);
        goto _end;
    };

    if (r->client != 0) {
        rc = nai_errno_to_sult(EPERM);
        goto _end;
    };

    /* create request pool */
    r->pool = pool;
    if (r->pool == 0) {
        nai_pool_init(&temp, 0);

        r->pool = (nai_pool_t*)nai_palloc(&temp, sizeof(*pool));
        if (r->pool == 0) {
            rc = nai_sult_from_errno();
            nai_pool_close(&temp);
            goto _end;
        };

        r->pool_own = 1;
        nai_pool_init(r->pool, 0);
        nai_pool_exchange(r->pool, &temp);
    };

    /* alloc space to inplace buffer pool */
    r->bufpool = (nai_bufpool_t*)nai_palloc(r->pool, sizeof(*r->bufpool));
    if (r->bufpool == 0) {
        rc = nai_sult_from_errno();
        nai_log_alert(NAI_LOG_HTTPC, 
            nai_sult_to_errno(rc), "alloc request buffer pool failed");
        goto _fail;
    };

    /* create buffer pool */
    rc = nai_bufpool_from(r->bufpool, r->pool, 0);
    if (rc < 0) {
        rc = nai_sult_from_errno();
        nai_log_alert(NAI_LOG_HTTPC, 
            nai_sult_to_errno(rc), "create request buffer pool failed");
        goto _fail;
    };

    nai_buflist_init(&r->content, r->bufpool);
    nai_array_init(&r->headers_in.cookies, 
        sizeof(nai_httpc_header_t*), r->pool);

    if (flags & NAI_HTTPC_COPY_HEADERS) {
        r->copy_headers = 1;
    };

    r->client = c;
    r->conf = c->agent->conf;

_end:
    return rc;

_fail:
    if (r->pool_own) {
        nai_pool_close(r->pool);
        r->pool_own = 0;
    };
    r->pool = 0;
    r->bufpool = 0;
    goto _end;
};


nai_sult_t nai_httpc_close(nai_httpc_request_t* r)
{
    nai_sult_t rc;


    if (r->stat != NAI_HTTPC_STAT_NONE) {
        rc = nai_errno_to_sult(EPERM);
        goto _end;
    };

    if (r->pool_own) {
        nai_pool_close(r->pool);
        r->pool_own = 0;
    };

    r->pool = 0;
    r->bufpool = 0;

    rc = nai_httpc_init(r);

_end:
    return rc;
};


nai_sult_t nai_httpc_set_method(
    nai_httpc_request_t* r, nai_int_t method)
{
    nai_sult_t rc;


    r->method = method;
    nai_str_setn(&r->method_name);
    rc = 0;

    return rc;
};


nai_sult_t nai_httpc_set_method_name(
    nai_httpc_request_t* r, const nai_mem_t* method, nai_int_t flags)
{
    nai_sultp_t rc;
    nai_mem_t mem;


    if (flags & NAI_HTTP_DUP) {
        rc = nai_str_dup((nai_str_t*)&mem, 
            nai_str(method), nai_str_len(method), r->pool);
        if (rc < 0) {
            rc = nai_sult_from_errno();
            goto _end;
        };

        r->method_name = mem;
    } else {
        r->method_name = *method;
    };

    r->method = NAI_HTTP_UNKNOWN;
    rc = 0;

_end:
    return (nai_sult_t)rc;
};


nai_sult_t nai_httpc_set_uri(
    nai_httpc_request_t* r, const nai_mem_t* uri, nai_int_t flags)
{
    nai_sultp_t rc;
    nai_mem_t mem;


    if (flags & NAI_HTTP_DUP) {
        rc = nai_str_dup((nai_str_t*)&mem, 
            nai_str(uri), nai_str_len(uri), r->pool);
        if (rc < 0) {
            rc = nai_sult_from_errno();
            goto _end;
        };

        r->unparsed_uri = mem;
    } else {
        r->unparsed_uri = *uri;
    };

    if (r->valid_parsed_uri) {
        nai_uri_init(&r->parsed_uri);
        r->valid_parsed_uri = 0;
    };

    rc = 0;

_end:
    return (nai_sult_t)rc;
};


nai_sult_t nai_httpc_set_version(nai_httpc_request_t* r, nai_int_t version)
{
    nai_sult_t rc;


    r->version = version;
    rc = 0;

    return rc;
};


nai_sult_t nai_httpc_set_sigdone(
    nai_httpc_request_t* r, nai_int_t sigid, int32_t msec)
{
    nai_sult_t rc;


    if (msec < -1) {
        rc = nai_errno_to_sult(EINVAL);
        goto _end;
    };
    if (sigid < 0 || sigid >= NAI_HTTPC_SIGNAL) {
        rc = nai_errno_to_sult(EINVAL);
        goto _end;
    };

    r->msec = msec;
    r->sigdone = sigid;
    rc = 0;

_end:
    return rc;
};


nai_sult_t nai_httpc_post(nai_httpc_request_t* r, nai_int_t sigid)
{
    nai_sult_t rc;


    if (sigid < 0 || sigid >= NAI_HTTPC_SIGNAL) {
        rc = nai_errno_to_sult(EINVAL);
        goto _end;
    };

    rc = nai_evnode_post(&r->ev, sigid);
    if (rc < 0) {
        rc = nai_sult_from_errno();
    };

_end:
    return rc;
};


nai_sult_t nai_httpc_set_timeout(
    nai_httpc_request_t* r, nai_int_t op, int32_t msec)
{
    nai_sult_t rc;


    rc = nai_evnode_set_timeout(&r->ev, op, msec);
    if (rc < 0) {
        rc = nai_sult_from_errno();
    };

    return rc;
};


nai_sult_t nai_httpc_request(nai_httpc_request_t* r)
{
    nai_int_t inloop;
    nai_sult_t rc;
    nai_httpc_t* h;
    nai_httpc_agent_t* a;
    nai_main_event_t* e;
    nai_evloop_t* l;


    if (r->client == 0) {
        rc = nai_errno_to_sult(EINVAL);
        goto _end;
    };

    /* check request */
    if (r->stat != NAI_HTTPC_STAT_NONE) {
        rc = nai_errno_to_sult(EPERM);
        goto _end;
    };

    /* prepare request */
    rc = nai_httpc_prepare_request(r);
    if (rc < 0) {
        goto _end;
    };

    /* allocate local of request */
    a = r->client->agent;
    h = nai_httpc_get_service(a);
    if (r->local == 0) {
        rc = nai_httpc_local_alloc(h, r->pool, &r->local);
        if (rc < 0) {
            goto _end;
        };
    } else {
        nai_pzero(r->local->data + 1, h->max_modules);
    };


    /* mark request start */
    r->stat = NAI_HTTPC_STAT_CONNECT;
    r->start_msec = nai_tickcache_to_msec();


    /* get the event loop */
    e = nai_main_event_get(h->main);
    l = e->loop;

    /* add into the event loop in non-blocking mode */
    inloop = nai_evloop_in_dispatch(l);

    /* if not in the event loop, we should send a signal to the event loop */
    if (inloop == 0) {
        r->sigop = NAI_HTTPC_STAT_CONNECT;
        rc = nai_evnode_post(&r->ev, NAI_HTTPC_SIGNAL);
        if (rc < 0) {
            goto _fail;
        };
    };

    if (!nai_evnode_is_opened(&r->ev)) {
        /* set event callback */
        nai_evnode_set_cb(&r->ev, nai_httpc_request_event);

        /* open event node */
        rc = nai_evnode_open(&r->ev, l);
        if (rc < 0) {
            rc = nai_sult_from_errno();
            goto _fail;
        };
    };

    /* if not in the event loop, wait signal callback */
    if (inloop == 0) {
        rc = NAI_E_INPROGRESS;
        goto _end;
    };


    /* run request */
    rc = nai_httpc_request_run(r);
    if (rc < 0) {
        goto _fail;
    };
    if (rc > NAI_DONE) {
        goto _fail;
    };


_end:
    return rc;

_fail:
    r->stat = NAI_HTTPC_STAT_NONE;
    nai_evnode_close(&r->ev);
    goto _end;
};


nai_sult_t nai_httpc_finalize(nai_httpc_request_t* r)
{
    nai_int_t inloop;
    nai_sult_t rc;
    nai_evloop_t* l;


    switch (r->stat) {
    case NAI_HTTPC_STAT_NONE:
        rc = 0;
        goto _end;
    case NAI_HTTPC_STAT_FINALIZE:
        rc = nai_errno_to_sult(EAGAIN);
        goto _end;
    case NAI_HTTPC_STAT_DONE:
        rc = 0;
        goto _done;
    default:
        break;
    };


    l = nai_evnode_get_loop(&r->ev);
    if (l == 0) {
        rc = nai_errno_to_sult(EPERM);
        goto _end;
    };

    /* check whether it needs 
     * to be delivered to the event loop thread for execution */
    inloop = nai_evloop_in_dispatch(l);
    if (inloop == 0) {

        r->sigop = NAI_HTTPC_STAT_FINALIZE;
        rc = nai_evnode_post(&r->ev, NAI_HTTPC_SIGNAL);
        if (rc < 0) {
            rc = nai_sult_from_errno();
        } else {
            rc = NAI_E_INPROGRESS;
        };

        goto _end;
    };

    /* wait for finish */
    rc = nai_httpc_request_wait(r);

_done:
    if (rc == 0) {
        rc = nai_evnode_close(&r->ev);
        if (rc < 0) {
            rc = nai_sult_from_errno();
        } else {
            r->stat = NAI_HTTPC_STAT_NONE;
        };
    };

_end:
    return rc;
};


nai_sultp_t nai_httpc_read(
    nai_httpc_request_t* r, nai_buflist_t* l, size_t limit)
{
    nai_sultp_t rc;
    nai_httpc_t* h;


    if (r->proto == 0) {
        rc = NAI_E_AGAIN;
        goto _end;
    };

    h = nai_httpc_get_service(r);
    rc = nai_chain_this(&h->proto_in, r, l, limit);

_end:
    return rc;
};


nai_sultp_t nai_httpc_write(
    nai_httpc_request_t* r, nai_buflist_t* l, size_t limit)
{
    nai_sultp_t rc;
    nai_httpc_t* h;
    nai_httpc_protocol_t* p;


    if (l == 0) {
        if (r->proto == 0) {
            rc = NAI_E_AGAIN;
            goto _end;
        };
    } else {
        if (r->endofoutput) {
            rc = nai_errno_to_sult(NAI_ESHUTDOWN);
            goto _end;
        };
        if (r->proto == 0) {
            rc = nai_buflist_move(&r->content, l, limit, 0, 0);
            if (rc < 0) {
                rc = nai_sult_from_errno();
            } else {
                rc = NAI_E_AGAIN;
            };
            goto _end;
        };
    };

    h = nai_httpc_get_service(r);
    rc = nai_chain_this(&h->proto_out, r, l, limit);
    if (rc == 0) {
        p = r->proto;
        if (p->sending == 0) {
            r->endofsend = 1;
            r->endofoutput = 1;
        };
    };

_end:
    return rc;
};


nai_sult_t nai_httpc_write_end(nai_httpc_request_t* r)
{
    nai_sultp_t rc;
    nai_buf_t* b;
    nai_httpc_t* h;
    nai_httpc_protocol_t* p;


    if (r->endofoutput) {
        rc = nai_errno_to_sult(NAI_ESHUTDOWN);
        goto _end;
    };

    if (r->client != 0) {
        rc = nai_errno_to_sult(EPERM);
        goto _end;
    };

    b = nai_buf_from_rmemory(r->bufpool, "", 0, 0);
    if (b == 0) {
        rc = nai_sult_from_errno();
        goto _end;
    };

    b->eos = 1;
    nai_buflist_insert_tail(&r->content, b);
    r->send = nai_httpc_request_flush;
    r->endofoutput = 1;

    if (r->proto == 0) {
        rc = 0;
        goto _end;
    };

    h = nai_httpc_get_service(r);
    rc = nai_chain_this(&h->proto_out, r, 0, -1);
    if (rc == 0) {
        p = r->proto;
        if (p->sending == 0) {
            r->send = 0;
            r->endofsend = 1;
        };
    };
    if (rc == NAI_E_AGAIN || rc == NAI_E_INPROGRESS) {
        rc = 0;
    };

_end:
    return (nai_sult_t)rc;
};


//////////////////////////////////////////////////////////////////////////////
// httpc request handles


static nai_sult_t nai_httpc_request_finishing(nai_httpc_request_t* r)
{
    nai_int_t events;
    nai_sult_t rc;
    nai_httpc_connection_t* c;
    nai_httpc_request_event_f handle;


    c = r->connection;
    if (c) {
        rc = nai_httpc_request_finish(r);
        if (rc < 0) {
            goto _end;
        };
    };

    handle = r->event;
    if (handle) {
        events = nai_ev_notify_from(r->sigdone);
        rc = handle(r, events);
    } else {
        rc = 0;
    };

_end:
    return rc;
};


static nai_sult_t nai_httpc_request_error(
    nai_httpc_request_t* r, nai_sult_t sult)
{
    nai_int_t ec;
    nai_int_t events;
    nai_sultp_t rc;
    nai_httpc_request_event_f handle;


    r->keepalive = 0;

    if (r->stat == NAI_HTTPC_STAT_FINALIZE) {
        rc = nai_httpc_request_finishing(r);
        goto _end;
    };

    handle = r->event;
    if (handle) {
        ec = nai_sult_to_errno(sult);
        events = nai_ev_error_from(ec);
        rc = handle(r, events);
    } else {
        rc = 0;
    };

_end:
    return (nai_sult_t)rc;
};


static nai_sult_t nai_httpc_request_flush(nai_httpc_request_t* r)
{
    nai_sultp_t rc;
    nai_httpc_t* h;
    nai_httpc_protocol_t* p;


    p = r->proto;
    if (p->sendtimeo) {
        rc = nai_errno_to_sult(ETIMEDOUT);
        goto _fail;
    };

    /* write all data */
    h = nai_httpc_get_service(r);
    while (1) {
        rc = nai_chain_this(&h->proto_out, r, 0, -1);
        if (rc < 0) {
            if (rc == NAI_E_AGAIN || rc == NAI_E_INPROGRESS) {
                break;
            };

            goto _fail;
        };
        if (rc == 0) {
            r->send = 0;
            r->endofsend = 1;
            if (r->stat == NAI_HTTPC_STAT_FINALIZE && 
                r->endofread) {
                nai_httpc_request_finishing(r);
            };
            break;
        };
    };

_end:
    return (nai_sult_t)rc;

_fail:
    rc = nai_httpc_request_error(r, (nai_sult_t)rc);
    goto _end;
};


static nai_sult_t nai_httpc_request_discard(nai_httpc_request_t* r)
{
    nai_sultp_t rc;
    nai_buflist_t tmp;
    nai_httpc_protocol_t* p;


    p = r->proto;
    if (p->readtimeo) {
        rc = nai_errno_to_sult(ETIMEDOUT);
        goto _fail;
    };

    nai_buflist_init(&tmp, r->bufpool);

    /* write all data */
    while (1) {
        rc = nai_httpc_read(r, &tmp, 8*1024);
        if (rc < 0) {
            if (rc == NAI_E_AGAIN || rc == NAI_E_INPROGRESS) {
                break;
            };

            goto _fail;
        };

        nai_buflist_close(&tmp);

        if (rc == 0) {
            r->read = 0;
            r->endofread = 1;
            if (r->stat == NAI_HTTPC_STAT_FINALIZE && 
                r->endofsend) {
                nai_httpc_request_finishing(r);
            };
            break;
        };
    };

_end:
    return (nai_sult_t)rc;

_fail:
    rc = nai_httpc_request_error(r, (nai_sult_t)rc);
    goto _end;
};


static nai_sult_t nai_httpc_request_connect(nai_httpc_request_t* r)
{
    nai_int_t found;
    nai_int_t ssl;
    nai_int_t port;
    nai_int_t max_requests;
    nai_int_t max_inuse;
    uint32_t ver;
    nai_sult_t rc;
    nai_mem_t host;
    nai_mem_t portname;
    nai_list_entry_t* e;
    nai_httpc_agent_t* a;
    nai_httpc_server_t* s;
    nai_httpc_server_node_t* n;
    nai_httpc_location_t* l;
    nai_httpc_protocol_t* p;
    nai_httpc_connection_t* c;


    /* get host and port */
    ssl = r->ssl;
    host = r->parsed_uri.hostname;
    if (nai_str_len(&r->parsed_uri.portname) <= 0) {
        port = ssl ? 443 : 80;
    } else {
        portname = r->parsed_uri.portname;
        rc = nai_atoi(&port, nai_str(&portname), nai_str_len(&portname), 0);
        if (rc < 0) {
            rc = nai_sult_from_errno();
            nai_log_error(NAI_LOG_HTTPC, 
                nai_sult_to_errno(rc), 
                "request with an invalid portname %.*s", 
                nai_str_len(&portname), nai_str(&portname));

            goto _end;
        };
    };

    /* get agent */
    a = nai_httpc_get_agent(r);

    /* get server node */
    rc = nai_httpc_server_node(a, &host, port, &n);
    if (rc < 0) {
        goto _end;
    };

    /* set version */
    if (r->version == 0) {
        if (n->version) {
            r->version = n->version;
        } else {
            r->version = NAI_HTTP_11;
        };
    };

    /* set max request */
    s = nai_httpc_get_server(r);
    ver = r->major;
    if (ver < 1) {
        max_requests = 1;
    } else {
        max_requests = s->max_requests;
    };

    if (ver <= 1) {
        max_inuse = 1;
    } else {
        max_inuse = 128;
    };

    found = 0;
    e = n->list.next;
    for ( ; e != &n->list; e = e->next) {
        c = nai_containof(e, nai_httpc_connection_t, ents);

        p = c->proto;
        if (p == 0) {
            continue;
        };
        if (p->establish == 0) {
            break;
        };

#if (NAI_HAVE_SSL)
        if (ssl != (c->ssl != 0)) {
            continue;
        };
#endif
        if (p->version != ver) {
            continue;
        };

        if (p->inuse >= (uint32_t)max_inuse) {
            continue;
        };

        if (c->requests < max_requests) {
            found = 1;
            break;
        };
    };

    /* if not found, then create a new connection */
    if (found == 0) {
        rc = nai_httpc_connection_create(a, s, &c);
        if (rc < 0) {
            goto _end;
        };

        c->server = n;
        nai_list_insert_tail(&n->list, &c->ents);
    };

    /* auto finish output if no content */
    if (r->headers_out.content_length_n == 0) {
        r->endofoutput = 1;
        r->send = nai_httpc_request_flush;
    };

    /* mark start request */
    r->stat = NAI_HTTPC_STAT_REQUEST;

    /* associate request and connection */
    rc = nai_httpc_handle_start(c, r);
    if (rc < 0) {
        if (found == 0) {
            goto _fail;
        } else {
            goto _end;
        };
    };

    /* do connect */
    if (found == 0) {
        l = nai_httpc_get_location(r);
        rc = nai_stream_set_opt(
            &c->c.str, NAI_IO_SENDTIMEO, (size_t)l->connect_timeo);
        if (rc < 0) {
            rc = nai_sult_from_errno();
            r->connection = 0;
            r->proto = 0;
            goto _fail;
        };

        rc = nai_stream_set_wait(&c->c.str, NAI_EV_READ|NAI_EV_WRITE);
        if (rc < 0) {
            rc = nai_sult_from_errno();
            r->connection = 0;
            r->proto = 0;
            goto _fail;
        };

        rc = nai_stream_connect_host(
            &c->c.str, &r->client->attr, &n->host, n->port);
        if (rc < 0) {
            rc = nai_sult_from_errno();
            if (rc != NAI_E_AGAIN && rc != NAI_E_INPROGRESS) {
                r->connection = 0;
                r->proto = 0;
                goto _fail;
            };

            goto _end;
        };
    };

    /* handling */
    rc = nai_httpc_handle(r);

_end:
    return rc;

_fail:
    nai_httpc_connection_close(c);
    goto _end;
};


static nai_sult_t nai_httpc_request_run(nai_httpc_request_t* r)
{
    nai_sult_t rc;
    nai_httpc_server_t* server;


    /* lookup server */
    rc = nai_httpc_server_find(r, &r->parsed_uri.hostname, &server);
    if (rc >= 0) {
        r->conf = server->conf;
    };

    /* run core request stages */
    r->stage = 0;
    rc = nai_httpc_core_run_stage(r);
    switch (rc) {
    case NAI_OK:
    case NAI_DECLINED:
        break;

    case NAI_EAGAIN:
    case NAI_EINPROGRESS:
        rc = NAI_HTTPC_INTERNAL_ERROR;
        goto _end;

    default:
        goto _end;
    };

    /* connect request */
    rc = nai_httpc_request_connect(r);

_end:
    return rc;
};


static nai_sult_t nai_httpc_request_finish(nai_httpc_request_t* r)
{
    nai_sult_t rc;
    nai_httpc_t* h;


    /* run done stages */
    rc = nai_httpc_core_run_stage(r);
    if (rc < 0 || rc > NAI_DECLINED) {
        if (rc > NAI_DECLINED) {
            rc = nai_sult_from_http_status(rc);
        };
        if (rc == NAI_E_AGAIN || rc == NAI_E_INPROGRESS) {
            r->read = nai_httpc_request_finishing;
            r->send = nai_httpc_request_finishing;
            goto _end;
        };

        h = nai_httpc_get_service(r);
        r->stage = h->stages[r->stage].next;

        nai_log_error(NAI_LOG_HTTPC, 
            nai_sult_to_errno(rc), 
            "run done stages failed on finish");
    };

    if (r->keepalive) {
        rc = nai_httpc_handle_keepalive(r);
    } else {
        rc = nai_httpc_handle_close(r);
    };
    if (rc < 0) {
        if (rc == NAI_E_AGAIN || rc == NAI_E_INPROGRESS) {
            r->read = nai_httpc_request_finishing;
            r->send = nai_httpc_request_finishing;
            goto _end;
        };

        assert(0);

        nai_log_alert(NAI_LOG_HTTPC, 
            nai_sult_to_errno(rc), 
            "%s() failed", r->keepalive ? 
            "nai_httpc_handle_keepalive" : "nai_httpc_handle_close");
        goto _end;
    };

    r->stat = NAI_HTTPC_STAT_DONE;
    r->endofread = 0;
    r->endofsend = 0;
    r->endofoutput = 0;
    nai_buflist_close(&r->content);
    rc = 0;

_end:
    return rc;
};


static nai_sult_t nai_httpc_request_wait(nai_httpc_request_t* r)
{
    nai_sultp_t rc;
    nai_httpc_t* h;
    nai_buflist_t tmp;


    /* check stat */
    if (r->stat == NAI_HTTPC_STAT_CONNECT) {
        r->keepalive = 0;
        goto _close;
    };

    /* set stage to done */
    h = nai_httpc_get_service(r);
    r->stage = h->groups[NAI_HTTPC_STAGE_DONE].start_index;

    /* flush */
    if (!r->endofoutput) {
        rc = nai_httpc_write_end(r);
        if (rc < 0 && (
            rc != NAI_E_AGAIN && rc != NAI_E_INPROGRESS)) {
            r->keepalive = 0;
            goto _close;
        };
    } else if (!r->endofsend) {
        r->send = nai_httpc_request_flush;
    } else {
        /* nothing */
        ;
    };

    /* discard */
    if (!r->endofread) {

        nai_buflist_init(&tmp, r->bufpool);

        while (1) {
            rc = nai_httpc_read(r, &tmp, 8*1024);

            if (rc == 0) {
                r->endofread = 1;
                break;
            };
            if (rc < 0) {
                if (rc != NAI_E_AGAIN && rc != NAI_E_INPROGRESS) {
                    r->keepalive = 0;
                    goto _close;
                };

                r->read = nai_httpc_request_discard;
                break;
            };

            nai_buflist_close(&tmp);
        };
    };

    if (!r->endofsend || !r->endofread) {
        if (r->msec) {
            rc = nai_httpc_set_timeout(r, 0, r->msec);
            if (rc < 0) {
                goto _close;
            };

            r->stat = NAI_HTTPC_STAT_FINALIZE;
            rc = NAI_E_INPROGRESS;
            goto _end;
        };

        r->keepalive = 0;
    };

_close:
    r->stat = NAI_HTTPC_STAT_FINALIZE;
    rc = nai_httpc_request_finish(r);

_end:
    return (nai_sult_t)rc;
};



//////////////////////////////////////////////////////////////////////////////
// httpc request event


static nai_int_t nai_httpc_request_event(nai_evnode_t* e, nai_int_t events)
{
    nai_int_t signum;
    nai_sult_t rc;
    nai_httpc_request_t* r;
    nai_httpc_handle_f readfn;
    nai_httpc_request_event_f handle;


    r = nai_containof(e, nai_httpc_request_t, ev);
    switch (events & NAI_EV_MASK) {
    case NAI_EV_TIMEOUT:
        if (r->stat != NAI_HTTPC_STAT_FINALIZE) {
            goto _handle;
        };

        r->keepalive = 0;
        nai_httpc_request_finishing(r);
        break;

    case NAI_EV_NOTIFY:
        signum = nai_ev_notify_code(events);
        if (signum != NAI_HTTPC_SIGNAL) {
            goto _handle;
        };

        switch (r->sigop) {
        case NAI_HTTPC_STAT_CONNECT:
            if (r->stat != NAI_HTTPC_STAT_CONNECT) {
                goto _end;
            };

            rc = nai_httpc_request_run(r);
            switch (rc) {
            case NAI_OK:
            case NAI_E_AGAIN:
            case NAI_E_INPROGRESS:
                goto _end;

            case NAI_DONE:
                readfn = r->read;
                if (readfn) {
                    readfn(r);
                };
                goto _end;

            case NAI_DECLINED:
                assert(0);

            default:
                r->stat = NAI_HTTPC_STAT_DONE;
                break;
            };

            break;

        case NAI_HTTPC_STAT_FINALIZE:
            if (r->stat != NAI_HTTPC_STAT_CONNECT && 
                r->stat != NAI_HTTPC_STAT_REQUEST) {
                goto _end;
            };

            rc = nai_httpc_request_wait(r);
            switch (rc) {
            case NAI_OK:
                events = nai_ev_notify_from(r->sigdone);
                goto _handle;

            case NAI_E_AGAIN:
            case NAI_E_INPROGRESS:
                goto _end;

            default:
                /* wait shouldn't return these values */
                assert(0);
                break;
            };

            break;

        default:
            /* unknown signal op */
            goto _end;
        };

        if (rc > 0) {
            rc = nai_errno_from_http_status(rc);
        } else {
            rc = nai_sult_to_errno(rc);
        };

        events = nai_ev_error_code(rc);

        /* fallthrough */

    default:

_handle:
        handle = r->event;
        if (handle) {
            handle(r, events);
        };
        break;
    };

_end:
    return 0;
};


//////////////////////////////////////////////////////////////////////////////
// httpc prepare request


#if !(NAI_HAVE_BIG_ENDIAN) && !(NAI_HAVE_ALIGNED)


#define nai_str3cmp(m, c0, c1, c2)                                      \
    m[0] == c0 && m[1] == c1 && m[2] == c2                              \

#define nai_str3Ocmp(m, c0, c1, c2, c3)                                 \
    *(uint32_t *) m == ((c3 << 24) | (c2 << 16) | (c1 << 8) | c0)       \

#define nai_str4cmp(m, c0, c1, c2, c3)                                  \
    *(uint32_t *) m == ((c3 << 24) | (c2 << 16) | (c1 << 8) | c0)       \

#define nai_str5cmp(m, c0, c1, c2, c3, c4)                              \
    *(uint32_t *) m == ((c3 << 24) | (c2 << 16) | (c1 << 8) | c0)       \
    && m[4] == c4                                                       \

#define nai_str6cmp(m, c0, c1, c2, c3, c4, c5)                          \
    *(uint32_t *) m == ((c3 << 24) | (c2 << 16) | (c1 << 8) | c0) &&    \
    (((uint32_t *) m)[1] & 0xffff) == ((c5 << 8) | c4)                  \

#define nai_str7cmp(m, c0, c1, c2, c3, c4, c5, c6)                      \
    *(uint32_t *) m == ((c3 << 24) | (c2 << 16) | (c1 << 8) | c0) &&    \
    m[4] == c4 && m[5] == c5 && m[6] == c6                              \

#define nai_str8cmp(m, c0, c1, c2, c3, c4, c5, c6, c7)                  \
    *(uint32_t *) m == ((c3 << 24) | (c2 << 16) | (c1 << 8) | c0) &&    \
    ((uint32_t *) m)[1] == ((c7 << 24) | (c6 << 16) | (c5 << 8) | c4)   \

#define nai_str9cmp(m, c0, c1, c2, c3, c4, c5, c6, c7, c8)              \
    *(uint32_t *) m == ((c3 << 24) | (c2 << 16) | (c1 << 8) | c0) &&    \
    ((uint32_t *) m)[1] == ((c7 << 24) | (c6 << 16) | (c5 << 8) | c4)   \
    && m[8] == c8                                                       \


#else


#define nai_str3cmp(m, c0, c1, c2)                                      \
    m[0] == c0 && m[1] == c1 && m[2] == c2                              \

#define nai_str3Ocmp(m, c0, c1, c2, c3)                                 \
    m[0] == c0 && m[2] == c2 && m[3] == c3                              \

#define nai_str4cmp(m, c0, c1, c2, c3)                                  \
    m[0] == c0 && m[1] == c1 && m[2] == c2 && m[3] == c3                \

#define nai_str5cmp(m, c0, c1, c2, c3, c4)                              \
    m[0] == c0 && m[1] == c1 && m[2] == c2 && m[3] == c3 && m[4] == c4  \

#define nai_str6cmp(m, c0, c1, c2, c3, c4, c5)                          \
    m[0] == c0 && m[1] == c1 && m[2] == c2 && m[3] == c3 &&             \
    m[4] == c4 && m[5] == c5                                            \

#define nai_str7cmp(m, c0, c1, c2, c3, c4, c5, c6)                      \
    m[0] == c0 && m[1] == c1 && m[2] == c2 && m[3] == c3 &&             \
    m[4] == c4 && m[5] == c5 && m[6] == c6                              \

#define nai_str8cmp(m, c0, c1, c2, c3, c4, c5, c6, c7)                  \
    m[0] == c0 && m[1] == c1 && m[2] == c2 && m[3] == c3 &&             \
    m[4] == c4 && m[5] == c5 && m[6] == c6 && m[7] == c7                \

#define nai_str9cmp(m, c0, c1, c2, c3, c4, c5, c6, c7, c8)              \
    m[0] == c0 && m[1] == c1 && m[2] == c2 && m[3] == c3 &&             \
    m[4] == c4 && m[5] == c5 && m[6] == c6 && m[7] == c7 && m[8] == c8  \


#endif



typedef struct nai_httpc_values_s {
    nai_str_t https;
    nai_str_t close;
    nai_str_t chunked;
    nai_str_t keepalive;
} nai_httpc_values_t;


static nai_httpc_values_t nai_httpc_values = {
    .https = nai_strconst("https"), 
    .close = nai_strconst("close"), 
    .chunked = nai_strconst("chunked"), 
    .keepalive = nai_strconst("keep-alive"), 
};


static const nai_mem_t nai_httpc_methods[] = {
    nai_strconst("GET"), 
    nai_strconst("HEAD"), 
    nai_strconst("POST"), 
    nai_strconst("PUT"), 
    nai_strconst("DELETE"), 
    nai_strconst("MKCOL"), 
    nai_strconst("COPY"), 
    nai_strconst("MOVE"), 
    nai_strconst("OPTIONS"), 
    nai_strconst("PROPFIND"), 
    nai_strconst("PROPPATCH"), 
    nai_strconst("LOCK"), 
    nai_strconst("UNLOCK"), 
    nai_strconst("PATCH"), 
    nai_strconst("TRACE"), 
};


static nai_sult_t nai_httpc_prepare_request(nai_httpc_request_t* r)
{
    nai_sult_t rc;


    rc = nai_httpc_prepare_method(r);
    if (rc < 0) {
        goto _end;
    };

    rc = nai_httpc_prepare_uri(r);
    if (rc < 0) {
        goto _end;
    };

    rc = nai_httpc_prepare_headers_out(r);
    if (rc < 0) {
        goto _end;
    };

_end:
    return rc;
};


static nai_sult_t nai_httpc_prepare_method(nai_httpc_request_t* r)
{
    nai_sult_t rc;
    const char* m;


    if (r->method != NAI_HTTP_UNKNOWN) {
        if (nai_str_len(&r->method_name) > 0) {
            rc = 0;
            goto _end;
        };

        if (r->method >= nai_countof(nai_httpc_methods)) {
            nai_str_setc(&r->method_name, "GET");
            rc = 0;
            goto _end;
        };

        r->method_name = nai_httpc_methods[r->method-1];

    } else if (nai_str_len(&r->method_name) > 0) {

        if (r->method != NAI_HTTP_UNKNOWN) {
            rc = 0;
            goto _end;
        };

        m = nai_str(&r->method_name);
        switch (nai_str_len(&r->method_name)) {
        case 3:
            if (nai_str3cmp(m, 'G', 'E', 'T')) {
                r->method = NAI_HTTP_GET;
                break;
            }
            if (nai_str3cmp(m, 'P', 'U', 'T')) {
                r->method = NAI_HTTP_PUT;
                break;
            }
            break;

        case 4:
            if (m[1] == 'O') {
                if (nai_str3Ocmp(m, 'P', 'O', 'S', 'T')) {
                    r->method = NAI_HTTP_POST;
                    break;
                }
                if (nai_str3Ocmp(m, 'C', 'O', 'P', 'Y')) {
                    r->method = NAI_HTTP_COPY;
                    break;
                }
                if (nai_str3Ocmp(m, 'M', 'O', 'V', 'E')) {
                    r->method = NAI_HTTP_MOVE;
                    break;
                }
                if (nai_str3Ocmp(m, 'L', 'O', 'C', 'K')) {
                    r->method = NAI_HTTP_LOCK;
                    break;
                }

            } else {
                if (nai_str4cmp(m, 'H', 'E', 'A', 'D')) {
                    r->method = NAI_HTTP_HEAD;
                    break;
                }
            }
            break;

        case 5:
            if (nai_str5cmp(m, 'M', 'K', 'C', 'O', 'L')) {
                r->method = NAI_HTTP_MKCOL;
                break;
            }
            if (nai_str5cmp(m, 'P', 'A', 'T', 'C', 'H')) {
                r->method = NAI_HTTP_PATCH;
                break;
            }
            if (nai_str5cmp(m, 'T', 'R', 'A', 'C', 'E')) {
                r->method = NAI_HTTP_TRACE;
                break;
            }
            break;

        case 6:
            if (nai_str6cmp(m, 'D', 'E', 'L', 'E', 'T', 'E')) {
                r->method = NAI_HTTP_DELETE;
                break;
            }
            if (nai_str6cmp(m, 'U', 'N', 'L', 'O', 'C', 'K')) {
                r->method = NAI_HTTP_UNLOCK;
                break;
            }
            break;

        case 7:
            if (nai_str7cmp(m, 
                'O', 'P', 'T', 'I', 'O', 'N', 'S')) {
                r->method = NAI_HTTP_OPTIONS;
            }
            break;

        case 8:
            if (nai_str8cmp(m, 
                'P', 'R', 'O', 'P', 'F', 'I', 'N', 'D')) {
                r->method = NAI_HTTP_PROPFIND;
            }
            break;

        case 9:
            if (nai_str9cmp(m,
                'P', 'R', 'O', 'P', 'P', 'A', 'T', 'C', 'H')) {
                r->method = NAI_HTTP_PROPPATCH;
            }
            break;

        default:
            break;
        };

    } else {
        r->method = NAI_HTTP_GET;
        nai_str_setc(&r->method_name, "GET");
    };

    rc = 0;

_end:
    return rc;
};


static nai_sult_t nai_httpc_prepare_uri(nai_httpc_request_t* r)
{
    nai_sultp_t rc;
    nai_mem_t uri;
    nai_mem_t str;
    nai_httpc_header_t* hdr;
    size_t size;


    if (r->valid_parsed_uri == 0) {
        rc = nai_uri_parse(&r->parsed_uri, 
            nai_str(&r->unparsed_uri), nai_str_len(&r->unparsed_uri), 
            NAI_URI_STRICT);
        if (rc < 0) {
            goto _end;
        };

        r->valid_parsed_uri = 1;
    };


    /* check ssl */
    r->ssl = nai_str_caseeq(
        &r->parsed_uri.scheme, &nai_httpc_values.https);


    /* check host */
    if (nai_str_len(&r->parsed_uri.hostname) <= 0) {
        hdr = r->headers_out.host;
        if (hdr == 0) {
            nai_log_info(NAI_LOG_HTTPC, 
                EINVAL, "HTTP request without Host");
            rc = NAI_HTTP_BAD_REQUEST;
            goto _end;
        };

        rc = nai_uri_parse(&r->parsed_uri, 
            nai_str(&hdr->name), nai_str_len(&hdr->name), 
            NAI_URI_HOSTONLY|NAI_URI_STRICT);
        if (rc < 0) {
            goto _end;
        };
    };


    /* check path */
    if (r->parsed_uri.path_empty) {
        nai_str_setc(&r->uri, "/");
        r->valid_uri = 1;
        rc = 0;
        goto _end;
    };

    size  = nai_str_len(&r->parsed_uri.path);
    uri.ptr = nai_palloc(r->pool, size + 1);
    if (uri.ptr == 0) {
        rc = nai_sult_from_errno();
        goto _end;
    };

    str = r->parsed_uri.path;
    nai_str_setl(&uri, size);
    if (!r->parsed_uri.path_escape && 
        !r->parsed_uri.path_complex) {
        nai_memcpy(nai_str(&uri), nai_str(&str), size);
    } else {

        if (r->parsed_uri.path_escape) {
            rc = nai_uri_unescape(
                uri.ptr, size, nai_str(&str), nai_str_len(&str));
            if (rc < 0) {
                rc = nai_sult_from_errno();
                goto _end;
            };

            nai_str_setl(&uri, rc);
            str = uri;
        };

        if (r->parsed_uri.path_complex) {
            rc = nai_path_canonicalize(
                uri.ptr, size, nai_str(&str), nai_str_len(&str), '/');
            if (rc < 0) {
                rc = nai_sult_from_errno();
                goto _end;
            };

            nai_str_setl(&uri, rc);
        };
    };

    nai_str_at(&uri, nai_str_len(&uri)) = 0;
    nai_str_setm(&r->uri, nai_str(&uri), nai_str_len(&uri));
    r->args = r->parsed_uri.query;
    r->valid_uri = 1;

    rc = 0;


_end:
    return (nai_sult_t)rc;
};


static nai_sult_t nai_httpc_prepare_headers_out(nai_httpc_request_t* r)
{
    nai_sult_t rc;
    nai_str_t val;
    nai_httpc_header_t* hdr;


    /* process connection */
    hdr = r->headers_out.connection;
    if (hdr) {
        val = hdr->value;
        if (nai_str_casecmp(&val, &nai_httpc_values.close) == 0) {
            r->headers_out.connection_type = NAI_HTTP_CLOSE;
        } else if (nai_str_casecmp(&val, &nai_httpc_values.keepalive) == 0) {
            r->headers_out.connection_type = NAI_HTTP_KEEPALIVE;
        } else {
            nai_log_info(NAI_LOG_HTTPC, EINVAL, 
                "request has an invalid value of Connection: %.*s", 
                nai_str_len(&hdr->value), nai_str(&hdr->value));
        };
    };

    /* process content-length */
    hdr = r->headers_out.content_length;
    if (hdr == 0) {
        r->headers_out.content_length_n = -1;
    } else {
        rc = nai_atoul(
            &r->headers_out.content_length_n, 
            nai_str(&hdr->value), nai_str_len(&hdr->value), 0);
        if (rc < 0) {
            nai_log_info(NAI_LOG_HTTPC, nai_errno, 
                "request has a invalid value of Content-Length: %.*s", 
                nai_str_len(&hdr->value), nai_str(&hdr->value));
            rc = NAI_HTTP_BAD_REQUEST;
            goto _end;
        };
    };

    /* process transfer-encoding */
    hdr = r->headers_out.transfer_encoding;
    if (hdr) {
        if (nai_str_caseeq(&hdr->value, &nai_httpc_values.chunked)) {
            r->headers_in.content_length_n = -1;
            r->headers_in.chunked = 1;
        } else {
            nai_log_info(NAI_LOG_HTTPC, EINVAL, 
                "request has a invalid value of Transfer-Encoding: %.*s", 
                nai_str_len(&hdr->value), nai_str(&hdr->value));
            rc = NAI_HTTP_NOT_IMPLEMENTED;
            goto _end;
        };
    };

    /* process keep-alive */
    if (r->headers_out.connection_type == NAI_HTTP_KEEPALIVE && 
        r->headers_out.keep_alive) {
        hdr = r->headers_out.keep_alive;
        nai_atoui(&r->headers_out.keep_alive_n, 
            nai_str(&hdr->value), nai_str_len(&hdr->value), 0);
    };


    rc = 0;

_end:
    return rc;
};


