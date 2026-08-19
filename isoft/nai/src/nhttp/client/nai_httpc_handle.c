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
/// @file       nai_httpc_handle.c
/// @brief      
/// @details
/// @date       2021-08-19
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#include "nai/runtime/nai_errno.h"
#include "nai/runtime/nai_log.h"
#include "nai_httpc_core.h"



static nai_sult_t nai_httpc_prepare_headers_in(nai_httpc_request_t* r);
static nai_sult_t nai_httpc_handle_empty(nai_httpc_event_t* e);
static nai_sult_t nai_httpc_handle_connect(nai_httpc_event_t* e);
static nai_sult_t nai_httpc_handle_status_line(nai_httpc_event_t* e);
static nai_sult_t nai_httpc_handle_headers(nai_httpc_event_t* e);
static nai_sult_t nai_httpc_handle_response_start(nai_httpc_event_t* e);
static nai_sult_t nai_httpc_handle_response(nai_httpc_event_t* e);
static nai_sult_t nai_httpc_handle_read(nai_httpc_event_t* e);
static nai_sult_t nai_httpc_handle_send(nai_httpc_event_t* e);
static nai_sult_t nai_httpc_handle_lingering(nai_httpc_event_t* e);
static nai_sult_t nai_httpc_handle_event(nai_httpc_event_t* e);


static nai_sult_t nai_httpc_handle_error(
    nai_httpc_connection_t* c, nai_sult_t sult);



typedef struct nai_httpc_values_s {
    nai_str_t close;
    nai_str_t chunked;
    nai_str_t keepalive;
} nai_httpc_values_t;


static nai_httpc_values_t nai_httpc_values = {
    .close = nai_strconst("close"), 
    .chunked = nai_strconst("chunked"), 
    .keepalive = nai_strconst("keep-alive"), 
};


static nai_sult_t nai_httpc_prepare_headers_in(nai_httpc_request_t* r)
{
    nai_sult_t rc;
    nai_str_t val;
    nai_httpc_header_t* hdr;
    nai_httpc_protocol_t* p;


    /* process connection */
    hdr = r->headers_in.connection;
    if (hdr) {
        val = hdr->value;
        if (nai_str_casecmp(&val, &nai_httpc_values.close) == 0) {
            r->headers_in.connection_type = NAI_HTTP_CLOSE;
            r->keepalive = 0;
        } else if (nai_str_casecmp(&val, &nai_httpc_values.keepalive) == 0) {
            r->headers_in.connection_type = NAI_HTTP_KEEPALIVE;
        } else {
            nai_log_info(NAI_LOG_HTTPC, EINVAL, 
                "response has an invalid value of Connection: %.*s", 
                nai_str_len(&hdr->value), nai_str(&hdr->value));
        };
    };

    hdr = r->headers_in.keep_alive;
    if (hdr) {
        val = hdr->value;
        if (nai_strncmp(nai_str(&val), "timeout=", 8) == 0) {
            nai_atoui(&r->headers_in.keep_alive_timeo, 
                nai_str(&val)+8, nai_str_len(&val)-8, 0);
        };
    };

    /* process content-length */
    hdr = r->headers_in.content_length;
    if (hdr == 0) {
        r->headers_in.content_length_n = -1;
    } else {
        rc = nai_atoul(
            &r->headers_in.content_length_n, 
            nai_str(&hdr->value), nai_str_len(&hdr->value), 0);
        if (rc < 0) {
            nai_log_info(NAI_LOG_HTTPC, nai_errno, 
                "response has an invalid value of Content-Length: %.*s", 
                nai_str_len(&hdr->value), nai_str(&hdr->value));
            rc = NAI_HTTP_BAD_REQUEST;
            goto _end;
        };
    };

    /* process transfer-encoding */
    hdr = r->headers_in.transfer_encoding;
    if (hdr) {
        if (nai_str_caseeq(&hdr->value, &nai_httpc_values.chunked)) {
            r->headers_in.content_length_n = -1;
            r->headers_in.chunked = 1;
        } else {
            nai_log_info(NAI_LOG_HTTPC, EINVAL, 
                "response has an invalid value of Transfer-Encoding: %.*s", 
                nai_str_len(&hdr->value), nai_str(&hdr->value));
            rc = NAI_HTTP_NOT_IMPLEMENTED;
            goto _end;
        };
    };

#if 0
    /* process keep-alive */
    if (r->headers_in.connection_type == NAI_HTTP_KEEPALIVE && 
        r->headers_in.keep_alive) {
        hdr = r->headers_in.keep_alive;
        nai_atoui(&r->headers_in.keep_alive_n, 
            nai_str(&hdr->value), nai_str_len(&hdr->value), 0);
    };
#endif

    if (r->headers_in.content_length_n != (uint64_t)-1) {
        p = r->proto;
        p->read_left = r->headers_in.content_length_n;
    } else if (r->headers_in.chunked == 0) {
        r->keepalive = 0;
    };

    rc = 0;

_end:
    return rc;
};



static nai_sult_t nai_httpc_read_buffer(nai_httpc_connection_t* c)
{
    nai_sultp_t rc;
    nai_buflist_t tmp;
    nai_httpc_t* h = nai_httpc_get_service(c);
    nai_httpc_proto_v1_t* p;


    p = (nai_httpc_proto_v1_t*)c->proto;
    nai_buflist_init(&tmp, 0);
    nai_buflist_insert_tail(&tmp, p->head);

    /* read from next of trans_in  */
    rc = nai_chain_next((nai_chain_t*)&h->trans_in, c, &tmp, -1);

    nai_buf_entry_remove(p->head);
    return (nai_sult_t)rc;
};


static nai_sult_t nai_httpc_alloc_header_buffer(nai_httpc_request_t* r)
{
    nai_sult_t rc;
    nai_httpc_server_t* s;
    nai_httpc_proto_v1_t* p;
    nai_httpc_connection_t* c;
    size_t size;


    c = r->connection;
    p = (nai_httpc_proto_v1_t*)r->proto;
    if (p->head && p->head != p->hsmall) {
        nai_buflist_insert_tail(&p->head_list, p->head);
        p->head_bufs ++;
        p->head = 0;
    };

    s = nai_httpc_get_server(c);
    if (p->head_bufs >= (intptr_t)s->max_header_buffers.num) {
        rc = NAI_HTTPC_HEADER_TOO_LARGE;
        goto _end;
    };

    size = nai_str_len(&p->parse.cur);
    if (size >= s->max_header_buffers.size) {
        rc = NAI_HTTPC_HEADER_TOO_LARGE;
        goto _end;
    };

    /* lookup exist */
    p->head = (nai_buf_t*)p->head_list.ent.next;
    if (p->head && 
        nai_buf_space(p->head) >= s->max_header_buffers.size) {
        nai_buf_entry_remove(p->head);
    } else {
        /* no buffer, allocate a new buffer */
        p->head = nai_buf_alloc(&c->c.bufpool, s->max_header_buffers.size);
        if (p->head == 0) {
            rc = nai_sult_from_errno();
            goto _end;
        };
    };

    if (nai_str(&p->parse.cur)) {
        size = nai_str_len(&p->parse.cur);
        nai_memcpy(nai_buf_ptr(p->head), nai_str(&p->parse.cur), size);
        p->parse.cur.ptr = (char*)nai_buf_ptr(p->head);
    };

    rc = 0;

_end:
    return rc;
};




static nai_sult_t nai_httpc_handle_empty(nai_httpc_event_t* e)
{
    nai_log_debug(NAI_LOG_HTTPC, 
        0, "empty handle with event 0x%x", e->events);
    return NAI_DECLINED;
};


#if (NAI_HAVE_SSL)


extern nai_sult_t nai_httpc_ssl_create(
    nai_httpc_connection_t* c, nai_buf_t* head);
extern nai_sult_t nai_httpc_ssl_verify_cert(
    nai_httpc_connection_t* c);


static nai_sult_t nai_httpc_handle_handshake(nai_httpc_event_t* e)
{
    nai_int_t events;
    nai_sult_t rc = 0;
    nai_httpc_connection_t* c;


    c = (nai_httpc_connection_t*)e->c;
    if (e->timedout) {
        rc = nai_errno_to_sult(ETIMEDOUT);
        nai_log_crit(NAI_LOG_HTTPC, 
            nai_sult_to_errno(rc), "connection timed out");
        goto _fail;
    };


    rc = nai_ssl_handshake(c->ssl);
    if (rc < 0) {
        rc = nai_sult_from_errno();
        if (rc != NAI_E_AGAIN) {
            nai_log_error(NAI_LOG_HTTPC, 
                nai_sult_to_errno(rc), "nai_ssl_handshake() failed");
            goto _fail;
        };

        rc = NAI_DECLINED;
        goto _end;

    };

    rc = nai_httpc_ssl_verify_cert(c);
    if (rc < 0 || rc > NAI_DECLINED) {
        /* error log in ssl verify */
        nai_log_info(NAI_LOG_HTTPC, 
            nai_sult_to_errno(rc), "nai_httpc_ssl_verify_cert() failed");
        goto _fail;
    };

    c->c.read = nai_httpc_handle_status_line;
    c->c.send = nai_httpc_handle_send;

    /* handshaked */
    events = nai_stream_get_event(&c->c.str);
    rc = c->c.str.cb(&c->c.str, events);


_end:
    return rc;

_fail:
    rc = nai_httpc_handle_error(c, rc);
    goto _end;
};


#endif


static nai_sult_t nai_httpc_handle_connect(nai_httpc_event_t* e)
{
    nai_int_t events;
    nai_sult_t rc;
    nai_httpc_server_t* s;
    nai_httpc_request_t* r;
    nai_httpc_connection_t* c;


    c = (nai_httpc_connection_t*)e->c;
    if (e->timedout) {
        rc = nai_errno_to_sult(ETIMEDOUT);
        nai_log_crit(NAI_LOG_HTTPC, 
            nai_sult_to_errno(rc), "connection timed out");
        goto _fail;
    };

    /* set option */
    r = c->proto->request;
    s = nai_httpc_get_server(r);
    if (s->read_so_bufsize) {
        rc = nai_stream_set_opt(
            &c->c.str, NAI_IO_RECVBUF, s->read_so_bufsize);
        if (rc < 0) {
            rc = nai_sult_from_errno();
            nai_log_crit(NAI_LOG_HTTPC, 
                nai_sult_to_errno(rc), "nai_stream_set_opt(recvbuf) failed");
            goto _fail;
        };
    };
    if (s->send_so_bufsize) {
        rc = nai_stream_set_opt(
            &c->c.str, NAI_IO_SENDBUF, s->send_so_bufsize);
        if (rc < 0) {
            rc = nai_sult_from_errno();
            nai_log_crit(NAI_LOG_HTTPC, 
                nai_sult_to_errno(rc), "nai_stream_set_opt(sendbuf) failed");
            goto _fail;
        };
    };

    c->c.read = nai_httpc_handle_status_line;
    c->c.send = nai_httpc_handle_send;

#if (NAI_HAVE_SSL)
    /* is ssl */
    if (r->ssl) {
        rc = nai_httpc_ssl_create(c, 0);
        if (rc < 0) {
            nai_log_alert(NAI_LOG_HTTPC, 
                nai_sult_to_errno(rc), "nai_httpc_ssl_create() failed");
            goto _fail;
        };

        c->c.read = nai_httpc_handle_handshake;
        c->c.send = nai_httpc_handle_handshake;
    };
#endif

    /* connected */
    events = nai_stream_get_event(&c->c.str);
    rc = c->c.str.cb(&c->c.str, events);

_end:
    return rc;

_fail:
    rc = nai_httpc_handle_error(c, rc);
    goto _end;
};


static nai_sult_t nai_httpc_handle_status_line(nai_httpc_event_t* e)
{
    nai_sult_t rc;
    nai_httpc_request_t* r;
    nai_httpc_proto_v1_t* p;
    nai_httpc_connection_t* c;


    c = (nai_httpc_connection_t*)e->c;
    p = (nai_httpc_proto_v1_t*)c->proto;
    r = p->request;

    if (e->timedout) {
        rc = nai_errno_to_sult(ETIMEDOUT);
        nai_log_crit(NAI_LOG_HTTPC, 
            nai_sult_to_errno(rc), "request timed out");
        goto _fail;
    };

    for (;;) {
        assert(p->head != 0);
        if (nai_buf_size(p->head) <= 0) {
            rc = nai_httpc_read_buffer(c);
            if (rc < 0) {
                if (rc != NAI_E_AGAIN && rc != NAI_E_INPROGRESS) {
                    goto _fail;
                };

                /* tell caller the connection is not closed */
                rc = NAI_DECLINED;
                goto _end;
            };
        };

        rc = nai_httpc_parse_status_line(r);
        if (rc != 0) {
            if (rc != NAI_E_AGAIN) {
                nai_log_info(NAI_LOG_HTTPC, rc > 0 ? 
                    nai_errno_from_http_status(rc) : 
                    nai_sult_to_errno(rc), 
                    "invalid request line, parse failed");
                goto _fail;
            };

            if (nai_buf_space(p->head) <= 0) {
                /* get a larger buffer */
                rc = nai_httpc_alloc_header_buffer(r);
                if (rc != 0) {
                    goto _fail;
                };
            };
            continue;
        };

        if (r->version < NAI_HTTP_10) {
            rc = nai_httpc_handle_response_start(e);
            goto _end;
        };


        /* success */
        p->parse.state = 0;
        c->c.read = nai_httpc_handle_headers;
        rc = c->c.read(e);
        goto _end;
    };

_fail:
    rc = nai_httpc_handle_error(c, rc);

_end:
    return rc;
};


static nai_sult_t nai_httpc_handle_headers(nai_httpc_event_t* e)
{
    nai_sult_t rc;
    nai_httpc_proto_v1_t* p;
    nai_httpc_request_t* r;
    nai_httpc_connection_t* c;


    c = (nai_httpc_connection_t*)e->c;
    p = (nai_httpc_proto_v1_t*)c->proto;
    r = p->request;

    if (e->timedout) {
        rc = nai_errno_to_sult(ETIMEDOUT);
        nai_log_crit(NAI_LOG_HTTPC, 
            nai_sult_to_errno(rc), "request timed out");
        goto _fail;
    };

    for (;;) {
        assert(p->head != 0);
        if (nai_buf_size(p->head) <= 0) {
            rc = nai_httpc_read_buffer(c);
            if (rc < 0) {
                if (rc != NAI_E_AGAIN && rc != NAI_E_INPROGRESS) {
                    goto _fail;
                };

                /* tell caller the connection is not closed */
                rc = NAI_DECLINED;
                goto _end;
            };
        };

        rc = nai_httpc_parse_headers(r);
        if (rc != 0) {
            if (rc != NAI_E_AGAIN) {
                nai_log_info(NAI_LOG_HTTPC, rc > 0 ? 
                    nai_errno_from_http_status(rc) : 
                    nai_sult_to_errno(rc), 
                    "invalid request header, parse failed");
                goto _fail;
            };

            if (nai_buf_space(p->head) <= 0) {
                /* get a larger buffer */
                rc = nai_httpc_alloc_header_buffer(r);
                if (rc != 0) {
                    goto _fail;
                };
            };
            continue;
        };

        rc = nai_httpc_prepare_headers_in(r);
        if (rc != 0) {
            goto _fail;
        };

        rc = nai_httpc_handle_response_start(e);
        goto _end;
    };

_fail:
    rc = nai_httpc_handle_error(c, rc);

_end:
    return rc;
};


static nai_sult_t nai_httpc_handle_response_start(nai_httpc_event_t* e)
{
    nai_sult_t rc;
    nai_httpc_t* h;
    nai_httpc_request_t* r;
    nai_httpc_protocol_t* p;
    nai_httpc_connection_t* c;


    /* setup connection handler */
    c = (nai_httpc_connection_t*)e->c;
    c->c.read = nai_httpc_handle_response;
    c->c.send = nai_httpc_handle_send;

    /* setup request handler */
    p = (nai_httpc_protocol_t*)c->proto;
    p->timeoset = 0;
    p->establish = 1;
    nai_list_entry_remove(&c->ents);
    nai_list_insert_head(&c->server->list, &c->ents);

    /* setup stage of request */
    h = nai_httpc_get_service(c);
    r = p->request;
    r->stage = h->groups[NAI_HTTPC_STAGE_RESPONSE].start_index;
    r->responsed = 1;

    /* next */
    rc = c->c.read(e);

    return rc;
};


static nai_sult_t nai_httpc_handle_response(nai_httpc_event_t* e)
{
    nai_sult_t rc;
    nai_httpc_request_t* r;
    nai_httpc_protocol_t* p;
    nai_httpc_connection_t* c;


    c = (nai_httpc_connection_t*)e->c;
    p = (nai_httpc_protocol_t*)c->proto;
    r = p->request;

    rc = nai_httpc_core_run_stage(r);
    switch (rc) {
    case NAI_E_AGAIN:
    case NAI_E_INPROGRESS:
        goto _end;

    case NAI_OK:
    case NAI_DONE:
    case NAI_DECLINED:
        c->c.read = nai_httpc_handle_read;
        rc = c->c.read(e);
        goto _end;

    default:
        break;
    };

    if (rc > 0) {
        rc = nai_sult_from_http_status(rc);
    };

    rc = nai_httpc_handle_error(c, rc);

_end:
    return rc;
};


static nai_sult_t nai_httpc_handle_read(nai_httpc_event_t* e)
{
    nai_sult_t rc;
    nai_httpc_protocol_t* p;
    nai_httpc_request_t* r;
    nai_httpc_connection_t* c;
    nai_httpc_handle_f readfn;


    c = (nai_httpc_connection_t*)e->c;
    p = (nai_httpc_protocol_t*)c->proto;
    p->readtimeo = e->timedout;
    p->sendtimeo = 0;
    p->readinprg = 0;

    r = p->request;
    readfn = r->read;
    if (readfn) {
        p->handling = 1;

        readfn(r);

        if (p->terminate) {
            rc = nai_httpc_connection_close(c);
            if (rc < 0) {
                nai_log_error(NAI_LOG_HTTPC, 
                    nai_sult_to_errno(rc), "close connection failed");
            };

            rc = 0;
            goto _end;
        };

        p->handling = 0;
    };

    rc = NAI_DECLINED;

_end:
    return rc;
};


static nai_sult_t nai_httpc_handle_send(nai_httpc_event_t* e)
{
    nai_sult_t rc;
    nai_httpc_protocol_t* p;
    nai_httpc_request_t* r;
    nai_httpc_connection_t* c;
    nai_httpc_handle_f sendfn;


    c = (nai_httpc_connection_t*)e->c;
    p = (nai_httpc_protocol_t*)c->proto;
    p->readtimeo = 0;
    p->sendtimeo = e->timedout;
    p->sendinprg = 0;


    r = p->request;
    sendfn = r->send;
    if (sendfn) {
        p->handling = 1;

        sendfn(r);

        if (p->terminate) {
            rc = nai_httpc_connection_close(c);
            if (rc < 0) {
                nai_log_error(NAI_LOG_HTTPC, 
                    nai_sult_to_errno(rc), "close connection failed");
            };

            rc = 0;
            goto _end;
        };

        p->handling = 0;
    };

    rc = NAI_DECLINED;

_end:
    return rc;
};


static nai_sult_t nai_httpc_handle_error(
    nai_httpc_connection_t* c, nai_sult_t sult)
{
    nai_int_t ec;
    nai_int_t events;
    nai_sult_t rc;
    nai_httpc_request_t* r;
    nai_httpc_protocol_t* p;
    nai_httpc_request_event_f handle;


    p = (nai_httpc_protocol_t*)c->proto;
    p->readtimeo = 0;
    p->sendtimeo = 0;
    c->c.error = 1;

    r = p->request;
    handle = r->event;
    if (handle) {
        p->handling = 1;

        ec = nai_sult_to_errno(sult);
        events = nai_ev_error_from(ec);
        handle(r, events);

        if (p->terminate) {
            rc = nai_httpc_connection_close(c);
            if (rc < 0) {
                nai_log_error(NAI_LOG_HTTPC, 
                    nai_sult_to_errno(rc), "close connection failed");
            };

            rc = 0;
            goto _end;
        };

        p->handling = 0;
    };

    rc = NAI_DECLINED;

_end:
    return rc;
};


static nai_sult_t nai_httpc_handle_lingering(nai_httpc_event_t* e)
{
    nai_sultp_t rc;
    nai_buf_t* b;
    nai_httpc_server_t* s;
    nai_httpc_proto_v1_t* p;
    nai_httpc_connection_t* c;
    uint64_t now;


    c = (nai_httpc_connection_t*)e->c;
    p = (nai_httpc_proto_v1_t*)c->proto;

    if (e->timedout) {
        rc = nai_errno_to_sult(NAI_ETIMEDOUT);
        goto _fail;
    };

    now = nai_tickcache_to_msec();
    if (now >= c->lingering_time) {
        rc = nai_errno_to_sult(NAI_ETIMEDOUT);
        goto _fail;
    };


    while (p->head == 0) {
        p->head = p->hsmall;
        if (p->head) {
            break;
        };

        s = nai_httpc_get_server(c);
        p->head = nai_buf_alloc(&c->c.bufpool, s->min_header_buffer);
        if (p->head == 0) {
            rc = nai_sult_from_errno();
            nai_log_alert(NAI_LOG_HTTPC, 
                nai_sult_to_errno(rc), 
                "alloc buffer failed on lingering close");
            goto _fail;
        };
        break;
    };

    b = p->head;
    b->total += b->start - (uint8_t*)b->ref.obj;
    b->start = (uint8_t*)b->ref.obj;
    for (;;) {

        rc = nai_stream_read(&c->c.str, nai_buf_ptr(b), nai_buf_space(b));
        if (rc < 0) {
            rc = nai_sult_from_errno();
            if (rc != NAI_E_AGAIN && rc != NAI_E_INPROGRESS) {
                goto _fail;
            };

            /* tell caller the connection is not closed */
            rc = NAI_DECLINED;
            goto _end;
        };
        if (rc == 0) {
            break;
        };
    };


_fail:
    (void)rc;

    rc = nai_httpc_handle_termiate(c, "lingering close");

_end:
    return (nai_sult_t)rc;
};


static nai_sult_t nai_httpc_handle_event(nai_httpc_event_t* e)
{
    nai_sult_t rc;
    nai_httpc_connection_t* c;


    c = (nai_httpc_connection_t*)e->c;

    if (e->timedout) {
        if (c->proto && 
            c->proto->request == 0) {
            nai_httpc_connection_close(c);
            rc = 0;
        } else {
            rc = NAI_DECLINED;
        };
        goto _end;
    };
    if (e->error) {
        if (c->proto && 
            c->proto->request == 0) {
            nai_httpc_connection_close(c);
            rc = 0;
        } else {
            rc = nai_httpc_handle_error(c, e->code);
        }
        goto _end;
    };

    rc = NAI_DECLINED;

_end:
    return rc;
};


nai_sult_t nai_httpc_handle_start(
    nai_httpc_connection_t* c, nai_httpc_request_t* r)
{
    nai_sult_t rc;
    nai_httpc_server_t* s;
    nai_httpc_proto_v1_t* p;
    nai_httpc_location_t* l;


    p = (nai_httpc_proto_v1_t*)c->proto;
    if (p != 0) {
        /* in keepalive */
        c->c.read = nai_httpc_handle_status_line;
        c->c.send = nai_httpc_handle_send;
        goto _run;
    };


    p = (nai_httpc_proto_v1_t*)nai_palloc(c->c.pool, sizeof(*p));
    if (p == 0) {
        rc = nai_sult_from_errno();
        goto _end;
    };

    /* this is http/1.x protocol */
    p->flags = 0;
    p->version = 1;

    /* init head buffer */
    p->head = 0;
    p->hsmall = 0;
    p->head_bufs = 0;
    nai_buflist_init(&p->head_list, &c->c.bufpool);

    /* init busy buffer */
    p->busy_bufs = 0;
    nai_buflist_init(&p->busy_list, &c->c.bufpool);

    /* init read buffer */
    p->read_body = 0;
    p->read_left = -1;
    nai_buflist_init(&p->read_list, &c->c.bufpool);

    /* init send buffer */
    p->send_body = 0;
    p->send_left = 0;
    nai_buflist_init(&p->send_list, &c->c.bufpool);


    /* init parse status */
    p->parse.state = 0;
    nai_str_setn(&p->parse.cur);
    nai_str_setn(&p->parse.stash);

    /* set to connection */
    c->proto = (nai_httpc_protocol_t*)p;


    /* allocate head buffer */
    if (p->head == 0) {
        p->head = p->hsmall;
        if (p->head == 0) {
            s = nai_httpc_get_server(c);
            p->head = nai_buf_alloc(&c->c.bufpool, s->min_header_buffer);
            if (p->head == 0) {
                rc = nai_sult_from_errno();
                nai_log_alert(NAI_LOG_HTTPC, 
                    nai_sult_to_errno(rc), "alloc header buffer failed");
                goto _end;
            };

            p->hsmall = p->head;
        };
    };


    c->c.read = nai_httpc_handle_empty;
    c->c.send = nai_httpc_handle_connect;
    c->c.event = nai_httpc_handle_event;

_run:
    r->connection = c;
    r->proto = (nai_httpc_protocol_t*)p;
    p->request = r;
    p->inuse ++;
    c->requests ++;

    /* handle keepalive */
    if (r->keepalive) {
        l = nai_httpc_get_location(r);
        if (c->requests >= l->keepalive_max) {
            r->keepalive = 0;
        };
    };

    rc = 0;

_end:
    return rc;
};


nai_sult_t nai_httpc_handle_termiate(
    nai_httpc_connection_t* c, const char* procname)
{
    nai_sult_t rc;
    nai_httpc_request_t* r;
    nai_httpc_proto_v1_t* p;


    p = (nai_httpc_proto_v1_t*)c->proto;
    if (p->request) {
        r = p->request;
        r->stat = 0;
        r->proto = 0;
        r->connection = 0;
        p->request = 0;
        p->inuse = 0;
    };

    /* close buffer lists, 
     * because the buffer in list well be invalid after close request */
    nai_buflist_close(&p->send_list);
    nai_buflist_close(&p->read_list);
    nai_buflist_close(&p->busy_list);

    /* reset handle */
    c->c.send = nai_httpc_handle_empty;
    c->c.read = nai_httpc_handle_empty;

    if (p->handling) {
        p->terminate = 1;

        if (c->server) {
            nai_list_entry_remove(&c->ents);
            c->server = 0;
        };

    } else {

        rc = nai_httpc_connection_close(c);
        if (rc < 0) {
            nai_log_error(NAI_LOG_HTTPC, 
                nai_sult_to_errno(rc), 
                "close connection failed on %s", procname);
        };
    };

    rc = 0;

    return rc;
};


nai_sult_t nai_httpc_handle(nai_httpc_request_t* r)
{
    nai_int_t events;
    nai_sult_t rc;
    nai_httpc_connection_t* c;


    /* process */
    c = r->connection;
    events = nai_stream_get_event(&c->c.str);
    if (events) {
        c->c.str.cb(&c->c.str, events);
    };

    rc = 0;

    return rc;
};


nai_sult_t nai_httpc_handle_keepalive(nai_httpc_request_t* r)
{
    size_t size;
    size_t write;
    uint64_t csize;
    uint32_t timeo;
    nai_sult_t rc;
    nai_buf_t* b;
    nai_buf_t* d;
    nai_list_entry_t* e;
    nai_httpc_connection_t* c;
    nai_httpc_proto_v1_t* p;
    nai_httpc_server_t* s;
    nai_httpc_location_t* l;


    p = (nai_httpc_proto_v1_t*)r->proto;
    c = r->connection;


    /* if read/send inprogress, then waiting */
    if (p->readinprg || p->sendinprg) {
        rc = NAI_E_INPROGRESS;
        goto _end;
    };

    if (c->c.closed) {
        r->keepalive = 0;
        rc = nai_httpc_handle_close(r);
        goto _end;
    };

    /* update keepalive */
    l = nai_httpc_get_location(r);
    if (c->requests >= l->keepalive_max) {
        r->keepalive = 0;
        rc = nai_httpc_handle_close(r);
        goto _end;
    };


    /* detach request */
    p->request = 0;
    r->proto = 0;
    r->connection = 0;

    /* clear head buffers */
    b = p->head;
    if (b && nai_buf_size(b) <= 0) {
        if (b != p->hsmall) {
            nai_buflist_insert_tail(&p->head_list, b);
        };
        b = 0;
        p->head = 0;
    };
    if (p->hsmall && p->hsmall != b) {
        d = p->hsmall;
        d->total += d->start - (uint8_t*)d->ref.obj;
        d->start = (uint8_t*)d->ref.obj;
        d->size = 0;
    };
    e = p->head_list.ent.next;
    for ( ; e != &p->head_list.ent; e = e->next) {
        d = (nai_buf_t*)e;
        d->total += d->start - (uint8_t*)d->ref.obj;
        d->start = (uint8_t*)d->ref.obj;
        d->size = 0;
    };
    if (b && b != p->hsmall) {
        p->head_bufs = 1;
    } else {
        p->head_bufs = 0;
        p->head = p->hsmall;
    };


    /* move the content in body buffers to header buffer */
    if (p->reading) {

        /* p->head != 0 */
        d = p->head;

        /* calculate the remaining data size */
        csize = nai_buflist_size(&p->read_list, 0, 0);

        /* get a larger area if there is not enough space */
        if (csize > nai_buf_space(d)) {
            s = nai_httpc_get_server(c);
            if (csize > s->max_header_buffers.size) {
                nai_log_alert(NAI_LOG_HTTPC, ERANGE, 
                    "the body processing reads too many the content "
                    "of the next request");
                rc = NAI_HTTPC_INTERNAL_ERROR;
                goto _fail;
            };

            if (d == p->hsmall) {
                e = p->head_list.ent.next;
                if (e != &p->head_list.ent) {
                    nai_list_entry_remove(e);
                } else {
                    e = (nai_list_entry_t*)nai_buf_alloc(
                        &c->c.bufpool, s->max_header_buffers.size);
                    if (e == 0) {
                        rc = nai_sult_from_errno();
                        nai_log_alert(NAI_LOG_HTTPC, nai_sult_to_errno(rc), 
                            "alloc header buffer failed");
                        goto _fail;
                    };
                };

                d = (nai_buf_t*)e;
                p->head = d;
                p->head_bufs = 1;
            };
        };
        d->total += d->start - (uint8_t*)d->ref.obj;
        d->start = (uint8_t*)d->ref.obj;
        d->size = 0;

        /* move memory areas that may overlap to avoid overwriting */
        write = 0;
        e = p->read_list.ent.next;
        for ( ; e != &p->read_list.ent; ) {
            b = (nai_buf_t*)e;
            e = e->next;

            size = nai_buf_size(b);
            if (size <= 0) {
                nai_buf_entry_remove(b);
                nai_buf_close(b);
                continue;
            };

            /* check whether the memory area overlaps */
            if (nai_buf_ptr(b) < nai_buf_ptr(d) || 
                nai_buf_ptr(b) >= nai_buf_ptr(d) + nai_buf_total(d)) {
                /* not overlapping memory area, skip */
                write += size;
                continue;
            };

            /* move data when the overlapping area 
             * is not the first one (write != 0) and 
             * the destination address is less than the source address */
            if (write != 0 && 
                nai_buf_ptr(d) + write < nai_buf_ptr(b)) {
                nai_memmove(nai_buf_ptr(d) + write, nai_buf_ptr(b), size);
                b->start = nai_buf_ptr(d) + write;
                b->total = size;
            };

            /* stop, because there is only one overlapped memory area */
            break;
        };

        /* merge the remaining buffers into head buffer */
        write = 0;
        e = p->read_list.ent.next;
        for ( ; e != &p->read_list.ent; ) {
            b = (nai_buf_t*)e;
            e = e->next;

            size = nai_buf_size(b);
            if (size <= 0) {
                nai_buf_entry_remove(b);
                nai_buf_close(b);
                continue;
            };

            /* if the overlapping area is the first and 
             * the remaining space is sufficient, 
             * directly modify the buffer address and size to avoid copying */
            if (write == 0 && 
                nai_buf_ptr(b) >= nai_buf_ptr(d) && 
                nai_buf_ptr(b) < nai_buf_ptr(d) + nai_buf_total(d) && (
                e == &p->read_list.ent || 
                nai_buf_space(b) >= (csize - size))) {
                d->total -= b->start - d->start;
                d->start = b->start;
            } else {
                if (nai_buf_space(d) < size) {
                    nai_log_alert(NAI_LOG_HTTPC, ERANGE, 
                        "the body processing reads too many the content "
                        "of the next request");
                    rc = NAI_HTTPC_INTERNAL_ERROR;
                    goto _fail;
                };

                if (nai_buf_ptr(d) + nai_buf_size(d) != nai_buf_ptr(b)) {
                    /* we use memcpy here, 
                     * because pointer of body always greater than head, 
                     * no copy overwrite
                     */
                    nai_memcpy(nai_buf_ptr(d) + 
                        nai_buf_size(d), nai_buf_ptr(b), size);
                };
            };

            nai_buf_wcommit(d, size);
            write += size;

            assert(b->refcount == 1);
            if (b->refcount != 1) {
                nai_log_warn(NAI_LOG_HTTPC, 0, 
                    "the boby buffer is in use, there may be a leak");
            };
            nai_buf_entry_remove(b);
            nai_buf_close(b);
        };

        e = p->busy_list.ent.next;
        for ( ; e != &p->busy_list.ent; ) {
            b = (nai_buf_t*)e;
            e = e->next;

            assert(b->refcount == 1);
            if (b->refcount != 1) {
                nai_log_warn(NAI_LOG_HTTPC, 0, 
                    "the busy buffer is in use, there may be a leak");
            };
            nai_buf_entry_remove(b);
            nai_buf_close(b);
        };
    };

    /* close send list, 
     * because the buffer in list well be invalid after close request */
    nai_buflist_close(&p->send_list);


    /* reset */
    p->header_sent = 0;
    p->reading = 0;
    p->sending = 0;
    p->read_body = 0;
    p->read_left = -1;
    p->parse.state = 0;
    p->inuse --;


    /* reset tcp options */
    if (c->c.tcp) {
        if (c->c.tcp_nopush == 1 && l->tcp_nopush) {
            rc = nai_httpc_connection_tcp_nopush(c, 0);
            if (rc < 0) {
                if (rc != nai_errno_to_sult(ENOTSUP)) {
                    nai_log_error(NAI_LOG_HTTPC, 
                        nai_sult_to_errno(rc), "set nopush option failed");
                    goto _fail;
                };
            };
        };
        if (c->c.tcp_nodelay == 0 && l->tcp_nodelay) {
            rc = nai_httpc_connection_tcp_nodelay(c, 1);
            if (rc < 0) {
                nai_log_error(NAI_LOG_HTTPC, 
                    nai_sult_to_errno(rc), "set nodelay option failed");
                goto _fail;
            };
        };
    };

    /* get keepalive timeout */
    timeo = r->headers_in.keep_alive_timeo;
    if (timeo > l->keepalive_timeo) {
        timeo = (uint32_t)l->keepalive_timeo;
    };

    /* start keepalive timer */
    rc = nai_connection_set_timeout(&c->c, 0, timeo);
    if (rc < 0) {
        goto _fail;
    };

    /* idle */
    c->c.read = nai_httpc_handle_empty;
    c->c.send = nai_httpc_handle_empty;
    rc = 0;

_end:
    return rc;

_fail:
    (void)rc;
    rc = nai_httpc_handle_termiate(c, "keepalive");
    goto _end;
};


nai_sult_t nai_httpc_handle_close(nai_httpc_request_t* r)
{
    nai_sult_t rc;
    nai_httpc_event_t e;
    nai_httpc_connection_t* c;
    nai_httpc_proto_v1_t* p;
    nai_httpc_location_t* l;


    p = (nai_httpc_proto_v1_t*)r->proto;
    c = r->connection;

    /* if read/send inprogress, then waiting */
    if (p->readinprg || p->sendinprg) {
        rc = NAI_E_INPROGRESS;
        goto _end;
    };

    /* detach request */
    p->request = 0;
    r->proto = 0;
    r->connection = 0;


    /* already closed */
    if (c->c.closed) {
        rc = nai_httpc_handle_termiate(c, "close");
        goto _end;
    };


    /* get lingering time */
    l = nai_httpc_get_location(r);
    if (c->lingering_time == 0) {
        c->lingering_time = nai_tickcache_to_msec() + 
            l->lingering_timeomax;
    };

    /* shutdown write */
    rc = nai_stream_shutdown(&c->c.str, NAI_SOCK_WR);
    if (rc < 0) {
        rc = nai_sult_from_errno();
        nai_log_error(NAI_LOG_HTTPC, 
            nai_sult_to_errno(rc), "shudown(wr) connection failed");
        goto _fail;
    };

    /* set read timeout */
    rc = nai_stream_set_opt(
        &c->c.str, NAI_IO_RECVTIMEO, (intptr_t)l->lingering_timeo);
    if (rc < 0) {
        rc = nai_sult_from_errno();
        nai_log_error(NAI_LOG_HTTPC, 
            nai_sult_to_errno(rc), 
            "set the lingering timeout for the request failed");
        goto _fail;
    };

    /* close buffer lists, 
     * because the buffer in list well be invalid after close request */
    nai_buflist_close(&p->send_list);
    nai_buflist_close(&p->read_list);
    nai_buflist_close(&p->busy_list);


    /* set handles */
    c->c.read = nai_httpc_handle_lingering;
    c->c.send = nai_httpc_handle_empty;

    /* try execute now */
    e.events = nai_stream_get_event(&c->c.str);
    if (e.events & NAI_EV_READ) {
        e.c = &c->c;
        c->c.read(&e);
    };


    rc = 0;

_end:
    return rc;

_fail:
    rc = nai_httpc_handle_termiate(c, "close");
    goto _end;
};


