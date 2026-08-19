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
/// @file       nai_agent_socks5.c
/// @brief      
/// @details
/// @date       2021-04-18
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#include "nai_agent_io.h"
#include "nai/io/nai_dns.h"
#include "nai/os/nai_socket.h"
#include "nai/runtime/nai_uri.h"
#include "nai/runtime/nai_log.h"


/*
 * socks5 protocol:
 * https://tools.ietf.org/html/rfc1928
 */


#define NAI_SOCKS5_METHOD_NONE       0x00
#define NAI_SOCKS5_METHOD_PASSWORD   0x02


#define NAI_SOCKS5_CONNECT           0x01
#define NAI_SOCKS5_BIND              0x02
#define NAI_SOCKS5_UDP               0x03


#define NAI_SOCKS5_SULT_OK           0x00
#define NAI_SOCKS5_SULT_CONNECT      0x01
#define NAI_SOCKS5_SULT_PREM         0x02
#define NAI_SOCKS5_SULT_NETWORK      0x03
#define NAI_SOCKS5_SULT_HOST         0x04
#define NAI_SOCKS5_SULT_REFUSE       0x05
#define NAI_SOCKS5_SULT_TIMEDOUT     0x06
#define NAI_SOCKS5_SULT_NOCMD        0x07
#define NAI_SOCKS5_SULT_AFNOTSUPPORT 0x08


#pragma pack(push, 1)


typedef struct nai_socks5_methodq_s {
    uint8_t ver;
    uint8_t count;
    uint8_t methods[1];
} nai_socks5_methodq_t;


typedef struct nai_socks5_mehtodr_s {
    uint8_t ver;
    uint8_t method;
} nai_socks5_methodr_t;


typedef struct nai_socks5_authq_s {
    uint8_t ver;
    /* uint8_t len + uint8_t user[] */
    /* uint8_t len + uint8_t password[] */
} nai_socks5_authq_t;


typedef struct nai_socks5_authr_s {
    uint8_t ver;
    uint8_t state;
} nai_socks5_authr_t;


typedef struct nai_socks5_connect_s {
    uint8_t ver;
    uint8_t cmd;
    uint8_t rsv;
    uint8_t type;
    /* addr_in | addr_in6 | uint8_t len + uint8_t domain[] */
    /* uint16_t port */
} nai_socks5_connectq_t, nai_socks5_connectr_t;


#pragma pack(pop)


typedef struct nai_agent_socks5_s {
    nai_agent_nconf_t* ac;
    nai_iobase_cb_f cb;
    nai_agent_dest_t dest;
    uint16_t total;
    uint16_t len;
    uint16_t off;
    struct {
        uint8_t stat:6;
        uint8_t io:2;
    };
    uint8_t mem[1];
} nai_agent_socks5_t;



static nai_int_t nai_agent_socks5_do_auth(nai_agent_socks5_t* s)
{
    nai_int_t r;
    nai_int_t off;
    nai_int_t len;
    char* str;
    nai_agent_nconf_t* ac;


    ac = s->ac;
    s->io = NAI_EV_WRITE;
    s->stat = 1;
    s->mem[0] = 5;

    off = 1;
    str = nai_str(&ac->user);
    len = (uint8_t)nai_str_len(&ac->user);
    s->mem[off] = (uint8_t)len;
    nai_memcpy(s->mem + off + 1, str, len);

    off += 1 + len;
    str = nai_str(&ac->password);
    len = (uint8_t)nai_str_len(&ac->password);
    s->mem[off] = (uint8_t)len;
    nai_memcpy(s->mem + off + 1, str, len);

    s->off = 0;
    s->len = off + len + 1;
    r = 0;


    return r;
};


static nai_int_t nai_agent_socks5_do_connect(nai_agent_socks5_t* s)
{
    nai_int_t r;
    nai_int_t off;
    uint8_t cmd;


    if (s->dest.proto == NAI_IO_TYPE_STREAM) {
        cmd = NAI_SOCKS5_CONNECT;
    } else {
        cmd = NAI_SOCKS5_UDP;
    };

    s->io = NAI_EV_WRITE;
    s->stat = 2;
    s->mem[0] = 5;
    s->mem[1] = cmd;
    s->mem[2] = 0;

    switch (s->dest.type) {
    case NAI_AGENT_ADDR_IN:
        s->mem[3] = 0x1;
        nai_memcpy(s->mem+4, nai_str(&s->dest.addr), sizeof(nai_addr_in4_t));
        off = 4 + nai_str_len(&s->dest.addr);
        break;
#if (NAI_HAVE_SOCKADDR_IN6)
    case NAI_AGENT_ADDR_IN6:
        s->mem[3] = 0x4;
        nai_memcpy(s->mem+4, nai_str(&s->dest.addr), sizeof(nai_addr_in6_t));
        off = 4 + nai_str_len(&s->dest.addr);
        break;
#endif
    case NAI_AGENT_ADDR_DOMAIN:
        s->mem[3] = 0x3;
        s->mem[4] = (uint8_t)nai_str_len(&s->dest.addr);
        nai_memcpy(s->mem+5, 
            nai_str(&s->dest.addr), nai_str_len(&s->dest.addr));
        off = 5 + nai_str_len(&s->dest.addr);
        break;
    default:
        nai_errno = EAFNOSUPPORT;
        r = -1;
        goto _end;
    };

    s->mem[off+0] = (uint8_t)(s->dest.port >> 8);
    s->mem[off+1] = (uint8_t)(s->dest.port >> 0);
    s->off = 0;
    s->len = off + 2;
    r = 0;


_end:
    return r;
};


static nai_int_t nai_agent_socks5_to_errno(nai_int_t code)
{
    nai_int_t e;

    switch (code) {
    case NAI_SOCKS5_SULT_CONNECT:
        e = ECONNREFUSED;
        break;
    case NAI_SOCKS5_SULT_PREM:
        e = ECONNREFUSED;
        break;
    case NAI_SOCKS5_SULT_NETWORK:
        e = ECONNABORTED;
        break;
    case NAI_SOCKS5_SULT_HOST:
        e = EHOSTUNREACH;
        break;
    case NAI_SOCKS5_SULT_REFUSE:
        e = ECONNREFUSED;
        break;
    case NAI_SOCKS5_SULT_TIMEDOUT:
        e = ETIMEDOUT;
        break;
    case NAI_SOCKS5_SULT_NOCMD:
        e = ECONNREFUSED;
        break;
    case NAI_SOCKS5_SULT_AFNOTSUPPORT:
        e = EAFNOSUPPORT;
        break;
    default:
        e = ECONNREFUSED;
        break;
    };

    return e;
};


static nai_int_t nai_agent_socks5_handle(
    nai_agent_socks5_t* s, nai_iobase_t* p, 
    nai_iobase_t* c, nai_int_t events)
{
    intptr_t r;
    nai_int_t ec;
    nai_int_t len;
    nai_int_t port;
    nai_int_t type;
    void** ptr;
    nai_evloop_t* loop;
    nai_iobase_cb_f cb;
    nai_stream_ops_t* ops;
    nai_agent_context_t* cc;
    nai_agent_buf_t* b;
    nai_agent_nconf_t* ac;
    nai_socknbuf_in_t nbuf;


    if (!(events & (NAI_EV_READ|NAI_EV_WRITE))) {
        cb = s->cb;
        r = 0;
        goto _event;
    };

    cc = (nai_agent_context_t*)c->agent;
    if (events & NAI_EV_TIMEOUT) {
        ec = ETIMEDOUT;
        nai_log_error(NAI_LOG_CORE, 
            ec, "connect is timedout, event %d", events);
        events = nai_ev_error_from(ec);
        goto _fail;
    };

    ac = s->ac;
    if (s->io == 0) {
        s->mem[0] = 5;
        if (nai_str_len(&ac->user)) {
            s->mem[1] = 2;
            s->mem[2] = NAI_SOCKS5_METHOD_NONE;
            s->mem[3] = NAI_SOCKS5_METHOD_PASSWORD;
            s->len = 4;
        } else {
            s->mem[1] = 1;
            s->mem[2] = NAI_SOCKS5_METHOD_NONE;
            s->len = 3;
        };
        s->io = NAI_EV_WRITE;
    };

    if (s->dest.proto == NAI_IO_TYPE_DGRAM) {
        ops = (nai_stream_ops_t*)p->st.ops;
    } else {
        ops = (nai_stream_ops_t*)cc->ops;
    };

    for (;;) {
        switch (s->io & nai_stream_get_event(p)) {
        case NAI_EV_READ:
            r = ops->read(p, s->mem + s->len, s->total - s->len);
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
            case 1: /* select method */
                if (s->len < sizeof(nai_socks5_methodr_t)) {
                    continue;
                };
                if (s->mem[0] != 5) {
                    ec = ECONNREFUSED;
                    nai_log_warn(NAI_LOG_CORE, ec, 
                        "incorrect version(%d) returned by the server", 
                        s->mem[0]);

                    events = nai_ev_error_from(ec);
                    goto _fail;
                };

                switch (s->mem[1]) {
                case NAI_SOCKS5_METHOD_NONE:
                    r = nai_agent_socks5_do_connect(s);
                    if (r < 0) {
                        ec = nai_errno;
                        nai_log_error(NAI_LOG_CORE, 
                            ec, "prepare request to failed");
                        events = nai_ev_error_from(ec);
                        goto _fail;
                    };
                    break;
                case NAI_SOCKS5_METHOD_PASSWORD:
                    r = nai_agent_socks5_do_auth(s);
                    if (r < 0) {
                        ec = nai_errno;
                        nai_log_error(NAI_LOG_CORE, 
                            ec, "prepare authentication failed");
                        events = nai_ev_error_from(ec);
                        goto _fail;
                    };
                    break;
                default:
                    ec = ECONNREFUSED;
                    nai_log_warn(NAI_LOG_CORE, ec, 
                        "unknown auth method(%d) returned by the server", 
                        s->mem[1]);

                    events = nai_ev_error_from(ec);
                    goto _fail;
                };
                break;

            case 2: /* auth */
                if (s->len < sizeof(nai_socks5_authr_t)) {
                    continue;
                };
                if (s->mem[0] != 5) {
                    ec = ECONNREFUSED;
                    nai_log_warn(NAI_LOG_CORE, ec, 
                        "incorrect version(%d) returned by the server", 
                        s->mem[0]);

                    events = nai_ev_error_from(ec);
                    goto _fail;
                };
                if (s->mem[1] != 0) {
                    ec = ECONNREFUSED;
                    nai_log_warn(NAI_LOG_CORE, ec, "authentication failed");
                    events = nai_ev_error_from(ec);
                    goto _fail;
                };

                r = nai_agent_socks5_do_connect(s);
                if (r < 0) {
                    ec = nai_errno;
                    nai_log_error(NAI_LOG_CORE, 
                        ec, "prepare to request failed");
                    events = nai_ev_error_from(ec);
                    goto _fail;
                };
                break;

            case 3: /* connect */
                if (s->len < sizeof(uint8_t)*2) {
                    continue;
                };
                if (s->mem[0] != 5) {
                    ec = ECONNREFUSED;
                    nai_log_warn(NAI_LOG_CORE, ec, 
                        "incorrect version(%d) returned by the server", 
                        s->mem[0]);

                    events = nai_ev_error_from(ec);
                    goto _fail;
                };
                if (s->mem[1] != NAI_SOCKS5_SULT_OK) {
                    ec = nai_agent_socks5_to_errno(s->mem[1]);
                    nai_log_warn(NAI_LOG_CORE, ec, 
                        "the server rejected request with error %d", 
                        s->mem[1]);

                    events = nai_ev_error_from(ec);
                    goto _fail;
                };


                if (s->len < sizeof(nai_socks5_connectr_t)) {
                    continue;
                };
                switch (s->mem[3]) {
                case 0x1: /* ipv4 */
                    len = 6 + sizeof(nai_addr_in4_t);
                    if (s->len < len) {
                        continue;
                    };
                    break;
#if (NAI_HAVE_SOCKADDR_IN6)
                case 0x4: /* ipv6 */
                    len = 6 + sizeof(nai_addr_in6_t);
                    if (s->len < len) {
                        continue;
                    };
                    break;
#endif
                case 0x3: /* domain */
                    if (s->len < 5) {
                        continue;
                    };
                    len = 7 + s->mem[4];
                    if (s->len < len) {
                        continue;
                    };
                    break;

                default:
                    ec = EAFNOSUPPORT;
                    nai_log_warn(NAI_LOG_CORE, ec, 
                        "unknown address type(%d) returned by the server", 
                        s->mem[3]);

                    events = nai_ev_error_from(ec);
                    goto _fail;
                };

                /* connected */
                loop = ac->agent->loop;
                if (s->dest.proto == NAI_IO_TYPE_DGRAM) {
                    if (s->mem[3] == 0x3) {
                        ec = EAFNOSUPPORT;
                        nai_log_error(NAI_LOG_CORE, ec, 
                            "udp unsupported connect to a domain(%d)", 
                            s->mem[3]);

                        events = nai_ev_error_from(ec);
                        goto _fail;
                    };

                    port = (uint8_t)s->mem[len-1];
                    port |= (uint16_t)s->mem[len-2] << 8;
                    type = s->mem[3] == 0x1 ? AF_INET : AF_INET6;

                    r = nai_agent_make_sockaddr(&nbuf, type, s->mem + 4, port);
                    if (r < 0) {
                        ec = nai_errno;
                        nai_log_error(NAI_LOG_CORE, 
                            ec, "make socket address failed");
                        events = nai_ev_error_from(ec);
                        goto _fail;
                    };

                    c->st.ops = 0;
                    r = nai_dgram_connect(
                        c, loop, &nbuf.addr, nbuf.len);
                    if (r < 0) {
                        ec = nai_errno;
                        nai_log_error(NAI_LOG_CORE, 
                            ec, "failed to start udp connection");
                        events = nai_ev_error_from(ec);
                        goto _fail;
                    };

                } else if (s->len > len) {
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
                    nai_str_setm(&b->mem, s->mem + len, s->len - len);

                    /* construct pseudo connection for extra buffer
                     */
                    r = nai_agent_pseudo_open(
                        c, loop, NAI_AGENT_WARP_BUF, NAI_IO_TYPE_STREAM);
                    if (r < 0) {
                        ec = nai_errno;
                        nai_log_error(NAI_LOG_CORE, 
                            ec, "open pseudo connection failed");

                        s->ac = 0;
                        s->cb = cb;
                        s->dest.proto = NAI_IO_TYPE_STREAM;
                        events = nai_ev_error_from(ec);
                        goto _fail;
                    };

                    events = NAI_EV_READ | NAI_EV_WRITE;
                    goto _event;
                };

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
        nai_offsetof(nai_agent_socks5_t, mem) + s->total);

    if (s->dest.proto == NAI_IO_TYPE_STREAM) {
        cc->ctx = 0;
        c->cb = s->cb;
        c->st.ops = cc->ops;
        nai_agent_context_free(c);

    } else {
        ptr = (void**)(p + 1);
        ptr[1] = 0;
    };

    cb = c->cb;
    goto _event;
};



static nai_int_t nai_agent_socks5_handle_tcp(nai_iobase_t* p, nai_int_t events)
{
    intptr_t r;
    nai_int_t ec;
    nai_int_t signum;
    nai_iobase_t* c;
    nai_agent_socks5_t* s;
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

    s = (nai_agent_socks5_t*)cc->ctx;
    if (s == 0) {
        if (c->cb) {
            c->cb(p, events);
        };
        r = 0;
    } else {
        r = nai_agent_socks5_handle(s, p, c, events);
    };

    return (nai_int_t)r;
};


static nai_int_t nai_agent_socks5_handle_udp(nai_iobase_t* p, nai_int_t events)
{
    intptr_t r;
    nai_int_t ec;
    void** ptr;
    nai_iobase_t* c;
    nai_agent_socks5_t* s;
    nai_iobase_cb_f cb;
    static uint8_t buf[128];


    ptr = (void**)(p + 1);
    c = (nai_iobase_t*)ptr[0];
    s = (nai_agent_socks5_t*)ptr[1];
    if (s) {
        r = nai_agent_socks5_handle(s, p, c, events);
        goto _end;
    };

    if (events & NAI_EV_READ) {
        while (1) {
            r = nai_stream_read(p, buf, sizeof(buf));
            if (r < 0) {
                ec = nai_errno;
                if (ec == NAI_EAGAIN || ec == NAI_EINPROGRESS) {
                    break;
                };

                nai_log_warn(NAI_LOG_CORE, ec, "connection is disconnected");
                events = nai_ev_error_from(ec);
                break;
            };
            if (r == 0) {
                events = nai_ev_error_from(ECONNREFUSED);
                break;
            };
        };
    };
    if (events & NAI_EV_ERROR) {
        if (nai_iobase_is_opened(c)) {
            cb = c->cb;
            if (cb) {
                cb(p, events);
            };
        };
    };

    r = 0;

_end:
    return (nai_int_t)r;
};


static void nai_agent_socks5_close_backend(void* p)
{
    nai_int_t r;
    nai_iobase_t* s;

    s = (nai_iobase_t*)p;
    r = nai_iobase_close(s);
    if (r < 0) {
        assert(0);
    };

    return;
};


static nai_int_t nai_agent_socks5_connect(
    nai_agent_attr_t* a, nai_iobase_t* c, 
    nai_agent_dest_t* dest)
{
    nai_int_t r;
    nai_int_t ec;
    nai_int_t mt;
    nai_int_t len;
    nai_int_t proto;
    void** ptr;
    nai_evloop_t* loop;
    nai_iobase_t* p;
    nai_agent_socks5_t* s;
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
     * */
    proto = dest->proto;
    r = nai_agent_pseudo_open(
        c, loop, NAI_AGENT_WARP_CONNECT, dest->proto);
    if (r < 0) {
        nai_log_error(NAI_LOG_CORE, 
            nai_errno, "open pseudo connection failed");
        goto _end;
    };

    mt = r;
    cc = (nai_agent_context_t*)c->agent;
    cc->ctx = 0;


    /* prepare connection */
    if (proto == NAI_IO_TYPE_STREAM) {
        p = c;
    } else {
        p = (nai_iobase_t*)nai_palloc(
            cc->pool, sizeof(*p) + sizeof(*ptr) * 2);
        if (p == 0) {
            nai_log_alert(NAI_LOG_CORE, 
                nai_errno, "allocate backend connection failed");
            s = 0;
            r = -1;
            goto _fail;
        };

        nai_stream_init(p);

        /* add into pool cleanup 
         * will be closed on destory connection.
         */
        r = nai_pool_add_cleanup(cc->pool, nai_agent_socks5_close_backend, p);
        if (r < 0) {
            nai_log_alert(NAI_LOG_CORE, 
                nai_errno, "add cleanup record failed");
            s = 0;
            goto _fail;
        };
    };

    /* alloc protocol context */
    len = 3 + nai_str_len(&ac->user) + nai_str_len(&ac->password);
    if (len < 6 + 256) {
        len = 6 + 256;
    };

    s = (nai_agent_socks5_t*)nai_palloc(
        cc->pool, nai_offsetof(nai_agent_socks5_t, mem) + len);
    if (s == 0) {
        nai_log_alert(NAI_LOG_CORE, 
            nai_errno, "allocate socks5 context failed");
        r = -1;
        goto _fail;
    };

    s->stat = 0;
    s->io = 0;
    s->ac = ac;
    s->cb = c->cb;
    s->dest = *dest;
    s->total = len;
    s->len = s->off = 0;


    if (proto == NAI_IO_TYPE_STREAM) {
        /* set context and callback */
        cc->ctx = s;
        c->st.ops = 0;

        /* connect */
        nai_stream_set_cb(p, nai_agent_socks5_handle_tcp);
        r = nai_stream_connect(p, loop, name->addr, name->len);
        if (r < 0) {
            nai_log_error(NAI_LOG_CORE, 
                nai_errno, "start connect node '%s' failed", 
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
            nai_log_error(NAI_LOG_CORE, 
                nai_errno, "open pseudo connection failed");
            goto _fail;
        };

    } else {
        /* set context */
        ptr = (void**)(p + 1);
        ptr[0] = c;
        ptr[1] = s;

        /* connect */
        nai_stream_set_cb(p, nai_agent_socks5_handle_udp);
        r = nai_stream_connect(p, loop, name->addr, name->len);
        if (r < 0) {
            nai_log_error(NAI_LOG_CORE, 
                nai_errno, "start connect node '%s' failed", 
                nai_str(&n->name));
            goto _fail;
        };
    };

    if (loop == 0) {
        p->cb(p, NAI_EV_READ|NAI_EV_WRITE);
        r = 0;
    };


_end:
    return r;

_fail:
    ec = nai_errno;

    if (s) {
        c->cb = s->cb;
        nai_pool_free_last(cc->pool, s, 
            nai_offsetof(nai_agent_socks5_t, mem) + len);
    };

    if (mt) {
        /* we can't close the event node in other thread, 
         * save error code and post event to the event loop thread.
         */
        nai_agent_failed(c, ec);
        r = 1;
    } else {

        /* close iobase */
        nai_iobase_close(c);
        nai_errno = ec;
    };

    goto _end;
};


static nai_int_t nai_agent_socks5_open(
    nai_agent_node_t* n, nai_agent_t* p, const nai_uri_t* attr)
{
    nai_int_t r;
    nai_agent_nconf_t* ac;


    r = nai_agent_node_open(n, p, attr, 0);
    if (r < 0) {
        goto _end;
    };

    ac = (nai_agent_nconf_t*)n->ctx;
    ac->connect = nai_agent_socks5_connect;
    if (ac->port == 0) {
        ac->port = 1080;
    };

    r = 0;

_end:
    return r;
};


nai_agent_ops_t nai_agent_socks5 = {
    "socks5", 
    (1 << NAI_IO_TYPE_STREAM)|(1 << NAI_IO_TYPE_DGRAM), 
    0, 
    nai_agent_socks5_open, 
    nai_agent_node_close, 
    nai_agent_node_connect
};

