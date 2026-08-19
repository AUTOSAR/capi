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
/// @file       nai_agent_http.c
/// @brief      
/// @details
/// @date       2022-06-13
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#include "nai_agent_io.h"
#include "nai_sample_http.h"
#include "nai/runtime/nai_log.h"



#define NAI_HTTP_SEND           0
#define NAI_HTTP_PARSE_STATUS   1
#define NAI_HTTP_PARSE_HEADER   2
#define NAI_HTTP_CONNECTED      3



typedef struct nai_agent_http_s {
    nai_agent_nconf_t* ac;
    nai_iobase_cb_f cb;
    nai_sample_request_t req;
    uint32_t stat;
    uint32_t len;
} nai_agent_http_t;


#define nai_copymem(d, s, l) (                          \
    nai_memcpy((d), (s), (l)), (l))                     \

#define nai_copystr(d, s) (                             \
    nai_copymem((d), nai_str(s), nai_str_len(s)))       \

#define nai_copyconst(d, s) (                           \
    nai_copymem((d), (s), sizeof(s) - 1))               \

#define nai_copychar(d, c) (                            \
    *(d) = (c), 1)                                      \


static const nai_str_t http_method = nai_strconst("CONNECT ");
static const nai_str_t http_version = nai_strconst(" HTTP/1.0\r\n");
static const nai_str_t http_host = nai_strconst("Host: ");
static const nai_str_t http_conn = nai_strconst("Connection: close\r\n\r\n");


static nai_int_t nai_agent_http_handle(nai_iobase_t* p, nai_int_t events)
{
    intptr_t r;
    nai_int_t ec;
    nai_int_t len;
    nai_int_t signum;
    nai_iobase_t* c;
    nai_iobase_cb_f cb;
    nai_stream_ops_t* ops;
    nai_agent_buf_t* b;
    nai_agent_http_t* s;
    nai_agent_context_t* cc;


    c = (nai_iobase_t*)p;
    cc = (nai_agent_context_t*)c->agent;
    assert(cc != 0);

    /* handle error signal */
    if (events & NAI_EV_NOTIFY) {
        signum = nai_ev_notify_code(events);
        if (signum == NAI_IOBASE_SIGNAL) {
            /* special notification code means connection failed, 
             * make an error event to user.
             */
            if (cc && cc->error && c->cb) {
                ec = cc->error;
                cc->error = 0;
                events = nai_ev_error_from(ec);
            };
        };
    };

    s = (nai_agent_http_t*)cc->ctx;
    if (s == 0) {
        cb = c->cb;
        r = 0;
        goto _event;
    };

    if (!(events & (NAI_EV_READ|NAI_EV_WRITE))) {
        cb = s->cb;
        r = 0;
        goto _event;
    };

    if (events & NAI_EV_TIMEOUT) {
        ec = ETIMEDOUT;
        nai_log_error(NAI_LOG_CORE, 
            ec, "connect is timedout, event %d", events);
        events = nai_ev_error_from(ec);
        goto _fail;
    };

    ops = (nai_stream_ops_t*)cc->ops;
    while (1) {
        switch (s->stat) {
        case NAI_HTTP_SEND:
            r = ops->write(p, s->req.cur, s->req.cend - s->req.cur);
            if (r < 0) {
                ec = nai_errno;
                if (ec == NAI_EAGAIN || ec == NAI_EINPROGRESS) {
                    goto _end;
                };

                nai_log_crit(NAI_LOG_CORE, ec, "write protocol failed");
                events = nai_ev_error_from(ec);
                goto _fail;
            };

            s->req.cur += r;
            if (s->req.cur < s->req.cend) {
                continue;
            };

            s->stat = NAI_HTTP_PARSE_STATUS;

            /* fallthrough */

        case NAI_HTTP_PARSE_STATUS:
        case NAI_HTTP_PARSE_HEADER:

            if (s->req.cur != s->req.cend || 
                s->req.cur != s->req.start) {
                if (s->req.cend > s->req.cur) {
                    nai_memmove(s->req.start, 
                        s->req.cur, s->req.cend - s->req.cur);
                };

                s->req.cend -= (s->req.cur - s->req.start);
                s->req.cur = s->req.start;
            };

            r = ops->read(p, s->req.cend, 
                s->len - (s->req.cend - s->req.start));
            if (r < 0) {
                ec = nai_errno;
                if (ec == NAI_EAGAIN || ec == NAI_EINPROGRESS) {
                    goto _end;
                };

                nai_log_crit(NAI_LOG_CORE, ec, "write protocol failed");
                events = nai_ev_error_from(ec);
                goto _fail;
            };

            s->req.cend += r;

            switch (s->stat) {
            case NAI_HTTP_PARSE_STATUS:
                r = nai_sample_parse_status_line(&s->req);
                if (r < 0) {
                    ec = nai_sult_to_errno(r);
                    if (ec == NAI_EAGAIN) {
                        continue;
                    };

                    events = nai_ev_error_from(ec);
                    goto _fail;
                };

                if (s->req.status != 200) {
                    ec = ECONNREFUSED;
                    events = nai_ev_error_from(ec);
                    goto _fail;
                };

                s->stat = NAI_HTTP_PARSE_HEADER;
                s->req.parse.state = 0;

                /* fallthrough */

            case NAI_HTTP_PARSE_HEADER:
                r = nai_sample_parse_header_skip(&s->req);
                if (r < 0) {
                    ec = nai_sult_to_errno(r);
                    if (ec == NAI_EAGAIN) {
                        continue;
                    };

                    events = nai_ev_error_from(ec);
                    goto _fail;
                };

                len = (nai_int_t)(s->req.cend - s->req.cur);

                /* connected */
                if (len <= 0) {
                    goto _connected;
                };

                /* after receiving the data of the user connection, 
                 * a pseudo-connection needs to be constructed to 
                 * return the data to the user.
                 */
                c->cb = s->cb;
                cb = s->cb;
                cc->flags = 0;

                /* construct extra buffer */
                b = (nai_agent_buf_t*)s;
#if (NAI_HAVE_SSL)
                b->ssl = 0;
#endif
                nai_str_setm(&b->mem, s->req.cur, len);

                /* construct pseudo connection for extra buffer
                 */
                r = nai_agent_pseudo_open(c, 
                    nai_iobase_get_loop(c), 
                    NAI_AGENT_WARP_BUF, NAI_IO_TYPE_STREAM);
                if (r < 0) {
                    ec = nai_errno;
                    nai_log_error(NAI_LOG_CORE, 
                        ec, "open pseudo connection failed");

                    s->ac = 0;
                    s->cb = cb;
                    events = nai_ev_error_from(ec);
                    goto _fail;
                };

                events = NAI_EV_READ | NAI_EV_WRITE;
                goto _event;

            default:
                assert(0);
                break;
            };

            /* fallthrough */

        default:
            assert(0);
            events = nai_ev_error_from(ECONNREFUSED);
            goto _fail;
        };
    };

_event:
    if (!nai_iobase_is_blocking(c)) {
        if (cb) {
            cb(p, events);
        };
    } else if (r < 0) {
        nai_errno = nai_ev_error_code(events);
    };

_end:
    return (nai_int_t)r;


_fail:
    r = -1;

_connected:
    nai_pool_free_last(cc->pool, s, 
        sizeof(nai_agent_http_t) + s->len);

    cc->ctx = 0;
    c->cb = s->cb;
    c->st.ops = cc->ops;
    nai_agent_context_free(c);

    cb = c->cb;
    goto _event;
};


static nai_int_t nai_agent_http_connect(
    nai_agent_attr_t* a, nai_iobase_t* c, 
    nai_agent_dest_t* dest)
{
    nai_int_t r;
    nai_int_t ec;
    nai_int_t mt;
    nai_int_t len;
    nai_int_t proto;
    nai_evloop_t* loop;
    nai_agent_http_t* s;
    nai_agent_node_t* n;
    nai_agent_nconf_t* ac;
    nai_agent_context_t* cc;
    nai_sockname_t* name;
    nai_str_t host;
    nai_str_t port;
    char* out;
    char buf1[96];
    char buf2[32];


    name = &dest->proxy;
    if (c->st.connection) {
        /* set connection address */
        r = nai_connection_set_addr(
            (nai_connection_t*)c, 
            NAI_CONN_PEERNAME, name->addr, name->len);
        if (r < 0) {
            nai_log_error(NAI_LOG_CORE, 
                nai_errno, "set connection peer address failed");
            goto _end;
        };
    };

    n = a->node;

    /* get context and event loop */
    ac = (nai_agent_nconf_t*)n->ctx;
    loop = ac->agent->loop;


    /* construct pseudo connection, 
     * to make the connection look like it is connecting
     */
    proto = dest->proto;
    r = nai_agent_pseudo_open(
        c, loop, NAI_AGENT_WARP_CONNECT, proto);
    if (r < 0) {
        nai_log_error(NAI_LOG_CORE, 
            nai_errno, "open pseudo connection failed");
        goto _end;
    };

    mt = r;
    cc = (nai_agent_context_t*)c->agent;
    cc->ctx = 0;


    s = 0;
    if (dest->type == NAI_AGENT_ADDR_DOMAIN) {
        host = dest->addr;
    } else {
        switch (dest->type) {
        case NAI_AGENT_ADDR_IN:
            r = nai_inet_ntop(
                AF_INET, dest->addr.ptr, buf1, sizeof(buf1)) ? 0 : -1;
            break;
#if (NAI_HAVE_SOCKADDR_IN6)
        case NAI_AGENT_ADDR_IN6:
            r = nai_inet_ntop(
                AF_INET6, dest->addr.ptr, buf1, sizeof(buf1)) ? 0 : -1;
            break;
#endif
        default:
            assert(0);
            nai_errno = EAFNOSUPPORT;
            r = -1;
            break;
        };
        if (r < 0) {
            ec = nai_errno;
            goto _fail;
        };

        nai_str_setm(&host, buf1, r);
    };

    r = nai_snprintf(buf2, sizeof(buf2), "%u", dest->port);
    nai_str_setm(&port, buf2, r);


    /* count request length */
    len = 0;
    len += nai_str_len(&http_method);
    len += nai_str_len(&host);
    len += 1;
    len += nai_str_len(&port);
    len += nai_str_len(&http_version);
    len += nai_str_len(&http_host);
    len += nai_str_len(&ac->host);
    len += 2;
    len += nai_str_len(&http_conn);

    /* alloc protocol context */
    s = (nai_agent_http_t*)nai_palloc(
        cc->pool, sizeof(nai_agent_http_t) + len);
    if (s == 0) {
        ec = nai_errno;
        nai_log_alert(NAI_LOG_CORE, 
            ec, "allocate http context failed");
        goto _fail;
    };

    s->ac = ac;
    s->cb = c->cb;
    s->stat = 0;
    s->len = len;
    s->req.parse.state = 0;
    s->req.start = (char*)(s + 1);
    s->req.cur = s->req.start;
    s->req.cend = s->req.cur + len;


    /* make request */
    out = s->req.start;
    out += nai_copystr(out, &http_method);
    out += nai_copystr(out, &host);
    out += nai_copychar(out, ' ');
    out += nai_copystr(out, &port);
    out += nai_copystr(out, &http_version);
    out += nai_copystr(out, &http_host);
    out += nai_copystr(out, &ac->host);
    out += nai_copyconst(out, "\r\n");
    out += nai_copystr(out, &http_conn);


    /* set context and callback */
    cc->ctx = s;
    c->st.ops = 0;
    c->cb = nai_agent_http_handle;

    /* connect */
    r = nai_stream_connect(c, loop, name->addr, name->len);
    if (r < 0) {
        ec = nai_errno;
        nai_log_error(NAI_LOG_CORE, 
            ec, "start connect node '%s' failed", 
            nai_str(&n->name));
        goto _fail;
    };

    /* open again */
    /* construct pseudo connection, 
     * to make the connection look like it is connecting
     */
    r = nai_agent_pseudo_open(
        c, loop, NAI_AGENT_WARP_CONNECT, proto);
    if (r < 0) {
        ec = nai_errno;
        nai_log_error(NAI_LOG_CORE, 
            ec, "open pseudo connection failed");
        goto _fail;
    };


    if (loop == 0) {
        c->cb(c, NAI_EV_READ|NAI_EV_WRITE);
        r = 0;
    };


_end:
    return r;

_fail:
    if (s) {
        c->cb = s->cb;
        nai_pool_free_last(cc->pool, s, 
            sizeof(nai_agent_http_t) + len);
    };

    if (mt) {
        /* we can't close the event node in other thread, 
         * save error code and post event to the event loop thread.
         */
        nai_agent_failed(c, ec);
        r = 1;

    } else {

        /* cleanup iobase */
        nai_iobase_close(c);
        nai_errno = ec;
        r = -1;
    };

    goto _end;
};


static nai_int_t nai_agent_http_open(
    nai_agent_node_t* n, nai_agent_t* p, const nai_uri_t* attr)
{
    nai_int_t r;
    nai_agent_nconf_t* ac;


    r = nai_agent_node_open(n, p, attr, 0);
    if (r < 0) {
        goto _end;
    };

    ac = (nai_agent_nconf_t*)n->ctx;
    ac->connect = nai_agent_http_connect;
    if (ac->port == 0) {
        ac->port = 80;
    };

    r = 0;

_end:
    return r;
};


nai_agent_ops_t nai_agent_http = {
    "http", 
    (1 << NAI_IO_TYPE_STREAM), 
    0, 
    nai_agent_http_open, 
    nai_agent_node_close, 
    nai_agent_node_connect
};

