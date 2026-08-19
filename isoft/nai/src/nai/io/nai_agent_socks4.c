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
/// @file       nai_agent_socks4.c
/// @brief      
/// @details
/// @date       2021-04-19
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#include "nai_agent_io.h"
#include "nai/runtime/nai_log.h"


/*
 * socks4 protocol:
 * https://www.openssh.com/txt/socks4.protocol
 * https://www.openssh.com/txt/socks4a.protocol
 */


#define NAI_SOCKS4_CONNECT           0x01
#define NAI_SOCKS4_BIND              0x02


#define NAI_SOCKS4_SULT_OK           90
#define NAI_SOCKS5_SULT_PREM         91
#define NAI_SOCKS5_SULT_CONNECT      92
#define NAI_SOCKS5_SULT_USER         93


#pragma pack(push, 1)


typedef struct nai_socks4_connect_s {
    uint8_t ver;
    uint8_t cmd;
    uint16_t port;
    uint32_t addr;
    /* uint8_t user[] */
    /* uint8_t domain[] */
} nai_socks4_connectq_t, nai_socks4_connectr_t;


#pragma pack(pop)


typedef struct nai_agent_socks4_s {
    nai_agent_nconf_t* ac;
    nai_iobase_cb_f cb;
    nai_agent_dest_t dest;
    uint16_t total;
    uint16_t limit;
    uint16_t len;
    uint16_t off;
    struct {
        uint8_t stat:5;
        uint8_t io:2;
        uint8_t alpha:1;
    };
    uint8_t mem[1];
} nai_agent_socks4_t;


extern nai_agent_ops_t nai_agent_socks4a;


static nai_int_t nai_agent_socks4_do_connect(nai_agent_socks4_t* s)
{
    nai_int_t r;
    nai_int_t off;
    nai_agent_nconf_t* ac;


    ac = s->ac;

    s->io = NAI_EV_WRITE;
    s->stat = 0;
    s->mem[0] = 4;
    s->mem[1] = NAI_SOCKS4_CONNECT;
    s->mem[2] = (uint8_t)(s->dest.port >> 8);
    s->mem[3] = (uint8_t)(s->dest.port >> 0);

    switch (s->dest.type) {
    case NAI_AGENT_ADDR_IN:
        nai_memcpy(s->mem+4, nai_str(&s->dest.addr), sizeof(nai_addr_in4_t));
        off = 4 + sizeof(nai_addr_in4_t);
        break;
    case NAI_AGENT_ADDR_DOMAIN:
        s->mem[4] = 0;
        s->mem[5] = 0;
        s->mem[6] = 0;
        s->mem[7] = 1;
        off = 4 + sizeof(nai_addr_in4_t);
        break;
    default:
        nai_errno = EAFNOSUPPORT;
        r = -1;
        goto _end;
    };

    /* put user */
    nai_memcpy(s->mem+off, nai_str(&ac->user), nai_str_len(&ac->user));
    off += nai_str_len(&ac->user);
    s->mem[off] = 0;
    off += 1;

    /* put domain */
    if (s->alpha) {
        if (s->dest.type == NAI_AGENT_ADDR_DOMAIN) {
            nai_memcpy(s->mem+off, 
                nai_str(&s->dest.addr), nai_str_len(&s->dest.addr));
            off += nai_str_len(&s->dest.addr);
        };
        s->mem[off] = 0;
        off += 1;
    };

    s->off = 0;
    s->len = off;
    r = 0;

_end:
    return r;
};


static nai_int_t nai_agent_socks4_to_errno(nai_int_t code)
{
    nai_int_t e;


    switch (code) {
    case NAI_SOCKS5_SULT_CONNECT:
        e = ECONNREFUSED;
        break;
    case NAI_SOCKS5_SULT_PREM:
        e = ECONNREFUSED;
        break;
    default:
        e = ECONNREFUSED;
        break;
    };

    return e;
};


static nai_int_t nai_agent_socks4_handle(nai_iobase_t* p, nai_int_t events)
{
    intptr_t r;
    nai_int_t ec;
    nai_int_t len;
    nai_int_t signum;
    nai_iobase_t* c;
    nai_iobase_cb_f cb;
    nai_stream_ops_t* ops;
    nai_agent_socks4_t* s;
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

    s = (nai_agent_socks4_t*)cc->ctx;
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

    if (s->io == 0) {
        r = nai_agent_socks4_do_connect(s);
        if (r < 0) {
            ec = nai_errno;
            nai_log_error(NAI_LOG_CORE, ec, "prepare to request failed");
            events = nai_ev_error_from(ec);
            goto _fail;
        };
    };


    ops = (nai_stream_ops_t*)cc->ops;
    for (;;) {
        switch (s->io & nai_stream_get_event(p)) {
        case NAI_EV_READ:
            r = ops->read(p, s->mem + s->len, s->limit - s->len);
            if (r < 0) {
                ec = nai_errno;
                if (ec == NAI_EAGAIN || ec == NAI_EINPROGRESS) {
                    goto _end;
                };

                nai_log_crit(NAI_LOG_CORE, ec, "read protocol failed");
                events = nai_ev_error_from(ec);
                goto _fail;
            };
            if (r == 0) {
                ec = ECONNREFUSED;
                nai_log_warn(NAI_LOG_CORE, ec, 
                    "the connection is closed by the server");

                events = nai_ev_error_from(ec);
                goto _fail;
            };

            s->len += (nai_int_t)r;

            switch (s->stat) {
            case 1: /* connect */
                if (s->len < sizeof(uint8_t)*2) {
                    continue;
                };
                if (s->mem[0] != 4) {
                    ec = ECONNREFUSED;
                    nai_log_warn(NAI_LOG_CORE, ec, 
                        "the version(%d) returned by the server is incorrect", 
                        s->mem[0]);

                    events = nai_ev_error_from(ec);
                    goto _fail;
                };
                if (s->mem[1] != NAI_SOCKS4_SULT_OK) {
                    ec = nai_agent_socks4_to_errno(s->mem[1]);
                    nai_log_warn(NAI_LOG_CORE, ec, 
                        "the server rejected request with error %d", 
                        s->mem[1]);

                    events = nai_ev_error_from(ec);
                    goto _fail;
                };


                len = sizeof(nai_socks4_connectr_t);
                if (s->len < len) {
                    continue;
                };

                /* connected */
                events = NAI_EV_READ | NAI_EV_WRITE;
                r = 0;
                goto _connected;

            default:
                assert(0);
                events = nai_ev_error_from(ECONNREFUSED);
                goto _fail;
            };
            break;

        case NAI_EV_WRITE:
            r = ops->write(p, s->mem + s->off, s->len - s->off);
            if (r < 0) {
                ec = nai_errno;
                if (ec == NAI_EAGAIN || ec == NAI_EINPROGRESS) {
                    goto _end;
                };

                nai_log_crit(NAI_LOG_CORE, ec, "write protocol failed");
                events = nai_ev_error_from(ec);
                goto _fail;
            };

            s->off += (nai_int_t)r;
            if (s->off >= s->len) {
                s->limit = sizeof(nai_socks4_connectr_t);
                s->io = NAI_EV_READ;
                s->stat ++;
                s->len = 0;
                s->off = 0;
            };
            break;

        default:
            r = 0;
            goto _end;
        };
    };

#if 0
    cb = s->cb;
#endif

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
        nai_offsetof(nai_agent_socks4_t, mem) + s->total);

    cc->ctx = 0;
    c->cb = s->cb;
    c->st.ops = cc->ops;
    nai_agent_context_free(c);

    cb = c->cb;
    goto _event;
};



static nai_int_t nai_agent_socks4_connect(
    nai_agent_attr_t* a, nai_iobase_t* c, 
    nai_agent_dest_t* dest)
{
    nai_int_t r;
    nai_int_t ec;
    nai_int_t mt;
    nai_int_t len;
    nai_int_t proto;
    nai_evloop_t* loop;
    nai_agent_socks4_t* s;
    nai_agent_node_t* n;
    nai_agent_nconf_t* ac;
    nai_agent_context_t* cc;
    nai_sockname_t* name;


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


    /* alloc protocol context */
    len = sizeof(nai_socks4_connectq_t) + 2 + nai_str_len(&ac->user);
    if (dest->type == NAI_AGENT_ADDR_DOMAIN) {
        len += nai_str_len(&dest->addr);
    };

    s = (nai_agent_socks4_t*)nai_palloc(
        cc->pool, nai_offsetof(nai_agent_socks4_t, mem) + len);
    if (s == 0) {
        ec = nai_errno;
        nai_log_alert(NAI_LOG_CORE, 
            ec, "allocate socks4 context failed");
        goto _fail;
    };

    s->stat = 0;
    s->io = 0;
    s->ac = ac;
    s->cb = c->cb;
    s->dest = *dest;
    s->total = len;
    s->len = s->off = 0;
    s->alpha = n->ops == &nai_agent_socks4a;


    /* set context and callback */
    cc->ctx = s;
    c->st.ops = 0;
    c->cb = nai_agent_socks4_handle;

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
            nai_offsetof(nai_agent_socks4_t, mem) + len);
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


static nai_int_t nai_agent_socks4_open(
    nai_agent_node_t* n, nai_agent_t* p, const nai_uri_t* attr)
{
    nai_int_t r;
    nai_agent_nconf_t* ac;


    r = nai_agent_node_open(n, p, attr, 0);
    if (r < 0) {
        goto _end;
    };

    ac = (nai_agent_nconf_t*)n->ctx;
    ac->connect = nai_agent_socks4_connect;
    if (ac->port == 0) {
        ac->port = 1080;
    };

    r = 0;

_end:
    return r;
};


nai_agent_ops_t nai_agent_socks4 = {
    "socks4", 
    (1 << NAI_IO_TYPE_STREAM), 
    NAI_AGENT_REQUIRE_SOCKADDR, 
    nai_agent_socks4_open, 
    nai_agent_node_close, 
    nai_agent_node_connect
};

nai_agent_ops_t nai_agent_socks4a = {
    "socks4a", 
    (1 << NAI_IO_TYPE_STREAM), 
    0, 
    nai_agent_socks4_open, 
    nai_agent_node_close, 
    nai_agent_node_connect
};

