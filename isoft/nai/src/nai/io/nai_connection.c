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
/// @file       nai_connection.c
/// @brief      
/// @details
/// @date       2021-04-12
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#include "nai/io/nai_connection.h"
#include "nai/os/nai_socket.h"
#include "nai/runtime/nai_errno.h"
#include "nai/runtime/nai_log.h"



static nai_int_t nai_connection_update_timer(
    nai_connection_t* c, uint32_t now)
{
    nai_int_t r;
    nai_int_t n;
    nai_int_t first;
    nai_int_t t = 0, to;


    if (!c->timerset) {
        r = 0;
        goto _end;
    };

    /* find recent timer */
    first = 1;
    for (n = 0; n < 3; n ++) {
        if (!(c->timerset & (1<<n))) {
            continue;
        };
        if (first) {
            first = 0;
            t = c->timer[n];
        } else if ((nai_int_t)(t - c->timer[n]) < 0) {
            t = c->timer[n];
        };
    };

    assert(!first);
    to = t - now;
    if (to < 0) {
        to = 0;
    };

    r = nai_stream_set_timeout(&c->str, 0, to);
    if (r >= 0) {
        c->timerevt = 0;
    };

_end:
    return r;
};


static nai_int_t nai_connection_pop_timer(
    nai_connection_t* c, uint32_t now)
{
    nai_int_t r;
    nai_int_t n;
    nai_int_t t, ev;


    if (!c->timerset) {
        r = 0;
        goto _end;
    };

    /* find recent timer */
    for (n = 0; n < 3; n ++) {
        ev = (1<<n);
        if (!(c->timerset & ev)) {
            continue;
        };

        /* check timer */
        t = c->timer[n] - now;
        if (t > 0) {
            continue;
        };

        /* unset timer flag */
        c->timerset &= ~ ev;

        /* fix event code */
        if (n == 2) {
            ev = NAI_EV_TIMEOUT;
        };
        r = ev;
        goto _end;
    };

    r = 0;

_end:
    return r;
};


static nai_int_t nai_connection_timeout(nai_connection_t* c)
{
    uint32_t now;
    nai_sult_t rc;
    nai_conn_event_t e;
    nai_conn_event_f cb;


    /* process timeout */
    c->timerevt = 0;
    e.c = c;

    now = nai_tickcache_to_msec32();
    while (1) {
        e.events = nai_connection_pop_timer(c, now);
        if (e.events == 0) {
            break;
        };
        if (e.events & NAI_EV_READ) {
            cb = c->read;
        } else if (e.events & NAI_EV_WRITE) {
            cb = c->send;
        } else {
            cb = c->event;
        };
        if (cb) {
            rc = cb(&e);
            if (rc != NAI_DECLINED) {
                /* the connection has been closed */
                goto _end;
            };
        };
    };

    if (!c->timerevt && c->timerset) {
        rc = nai_connection_update_timer(c, now);
        if (rc < 0) {
            nai_log_alert(NAI_LOG_CORE, 
                nai_errno, "nai_connection_update_timer() failed");
            goto _end;
        };
    };

    rc = NAI_DECLINED;

_end:
    return rc;
};


static nai_int_t nai_connection_event(nai_iobase_t* s, nai_int_t events)
{
    nai_sult_t rc;
    nai_conn_event_t e;
    nai_conn_event_f cb;


    e.c = (nai_connection_t*)s;
    e.events = events;
    if (e.events & (NAI_EV_READ|NAI_EV_WRITE)) {
        if (e.timedout) {
            e.c->timedout = 1;
        };
        if (e.read) {
            cb = e.c->read;
            rc = cb(&e);
            if (rc != NAI_DECLINED) {
                goto _end;
            };
        };
        if (e.write) {
            cb = e.c->send;
            rc = cb(&e);
            if (rc != NAI_DECLINED) {
                goto _end;
            };
        };
        rc = NAI_DECLINED;
    } else if (e.events & NAI_EV_TIMEOUT) {
        rc = nai_connection_timeout(e.c);
    } else {
        cb = e.c->event;
        if (cb) {
            rc = cb(&e);
        } else {
            rc = NAI_DECLINED;
        };
    };

_end:
    return rc;
};


nai_connection_t* nai_connection_new(nai_pool_t* p, size_t size)
{
    nai_int_t r;
    nai_int_t ec;
    nai_pool_t* n;
    nai_pool_t pool;
    nai_connection_t* c;


    if (size == 0) {
        size = sizeof(*c);
    } else if (size < sizeof(*c)) {
        nai_errno = EINVAL;
        c = 0;
        goto _end;
    };

    if (p == 0) {
        r = nai_pool_init(&pool, 0);
    } else {
        r = nai_pool_from(&pool, p);
    };
    if (r < 0) {
        c = 0;
        goto _end;
    };

    n = (nai_pool_t*)nai_palloc(&pool, sizeof(pool));
    if (n == 0) {
        n = &pool;
        goto _fail;
    };

    nai_pool_init(n, 0);
    nai_pool_exchange(n, &pool);


    c = nai_palloc(n, size);
    if (c == 0) {
        goto _fail;
    };

    /* initialize connection */
    r = nai_connection_init(c, n);
    if (r < 0) {
        goto _fail;
    };


_end:
    return c;

_fail:
    ec = nai_errno;
    nai_pool_close(n);
    nai_errno = ec;
    c = 0;
    goto _end;
};


nai_int_t nai_connection_init(nai_connection_t* c, nai_pool_t* p)
{
    nai_int_t r;
    nai_int_t n;


    r = nai_bufpool_from(&c->bufpool, p, 0);
    if (r < 0) {
        goto _end;
    };


    nai_list_init(&c->ent);
    nai_iobase_init(&c->str);
    nai_iobase_set_cb(&c->str, nai_connection_event);
    c->str.st.connection = 1;
    c->pool = p;
    c->read = 0;
    c->send = 0;
    c->event = 0;
    c->flags = 0;


    for (n = 0; n < 2; n ++) {
        c->ports[n] = 0;
        nai_str_setn(&c->hosts[n]);
        nai_sockname_init(&c->addrs[n]);
    };


_end:
    return r;
};


nai_int_t nai_connection_tcp_nopush(nai_connection_t* c, nai_int_t on)
{
    nai_int_t r;
    uint32_t nopush;


    nopush = !!on;
    if (c->tcp_nopush == nopush) {
        r = 0;
        goto _end;
    };

    r = nai_stream_set_opt(&c->str, NAI_IO_NOPUSH, nopush);
    if (r < 0) {
        goto _end;
    };

    c->tcp_nopush = nopush;
#if !(__freebsd__) && !(__drawin__)
    c->tcp_nodelay = !nopush;
#endif

_end:
    return r;
};


nai_int_t nai_connection_tcp_nodelay(nai_connection_t* c, nai_int_t on)
{
    nai_int_t r;
    uint32_t nodelay;


    nodelay = !!on;
    if (c->tcp_nodelay == nodelay) {
        r = 0;
        goto _end;
    };

    r = nai_stream_set_opt(&c->str, NAI_IO_NODELAY, nodelay);
    if (r < 0) {
        goto _end;
    };

    c->tcp_nodelay = nodelay;

_end:
    return r;
};


nai_int_t nai_connection_get_name(
    nai_connection_t* c, 
    nai_int_t opt, nai_str_t* host, nai_int_t* port)
{
    intptr_t r;
    char* str;
    const nai_sockaddr_info_t* i;
    nai_sockname_t* p;
    nai_socknbuf_t nbuf;


    switch (opt) {
    case NAI_CONN_SOCKNAME:
    case NAI_CONN_PEERNAME:
        p = &c->addrs[opt];
        if (p->addr == 0) {
            nbuf.len = sizeof(nbuf.storage);
            if (opt == 0) {
                r = nai_sock_get_sockname(
                    nai_stream_get_fd(&c->str), &nbuf.addr, &nbuf.len);
            } else {
                r = nai_sock_get_peername(
                    nai_stream_get_fd(&c->str), &nbuf.addr, &nbuf.len);
            };
            if (r < 0) {
                nai_log_crit(NAI_LOG_CORE, 
                    nai_errno, "nai_sock_get_sockname failed");
                goto _end;
            };

            p->len = nbuf.len;
            p->addr = (nai_sockaddr_t*)nai_palloc(c->pool, nbuf.len);
            if (p->addr == 0) {
                nai_log_alert(NAI_LOG_CORE, 
                    nai_errno, "alloc buffer for address failed");
                r = -1;
                goto _end;
            };

            nai_memcpy(p->addr, &nbuf.addr, nbuf.len);
        };
        break;

    default:
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    if (nai_str_len(&c->hosts[opt]) <= 0) {
        r = nai_sockaddr_ntop(
            c->addrs[opt].addr, c->addrs[opt].len, 0, 0, 0);
        if (r < 0) {
            nai_log_crit(NAI_LOG_CORE, 
                nai_errno, "nai_sockaddr_ntop failed");
            goto _end;
        };

        str = nai_palloc(c->pool, r);
        if (str == 0) {
            nai_log_alert(NAI_LOG_CORE, 
                nai_errno, "alloc buffer for address failed");
            r = -1;
            goto _end;
        };

        r = nai_sockaddr_ntop(
            c->addrs[opt].addr, c->addrs[opt].len, str, r, 0);
        if (r < 0) {
            nai_log_crit(NAI_LOG_CORE, 
                nai_errno, "nai_sockaddr_ntop failed");
            goto _end;
        };

        /* get host */
        nai_str_setm(&c->hosts[opt], str, r-1);

        /* get port */
        i = nai_sockaddr_info(c->addrs[opt].addr->sa_family);
        if (i) {
            c->ports[opt] = nai_sockaddr_get_port(i, c->addrs[opt].addr);
        };
    };

    if (host) {
        host[0] = c->hosts[opt];
    };
    if (port) {
        port[0] = c->ports[opt];
    };

    r = 0;

_end:
    return (nai_int_t)r;
};


nai_int_t nai_connection_get_addr(
    nai_connection_t* c, 
    nai_int_t opt, nai_sockaddr_t* in, nai_int_t* inlen)
{
    nai_int_t r;
    nai_sockname_t* p;
    nai_socknbuf_t nbuf;


    switch (opt) {
    case NAI_CONN_SOCKNAME:
    case NAI_CONN_PEERNAME:
        p = &c->addrs[opt];
        if (p->addr == 0) {
            nbuf.len = sizeof(nbuf.storage);
            if (opt == 0) {
                r = nai_sock_get_sockname(
                    nai_stream_get_fd(&c->str), &nbuf.addr, &nbuf.len);
            } else {
                r = nai_sock_get_peername(
                    nai_stream_get_fd(&c->str), &nbuf.addr, &nbuf.len);
            };
            if (r < 0) {
                nai_log_crit(NAI_LOG_CORE, 
                    nai_errno, "nai_sock_get_sockname failed");
                goto _end;
            };

            p->len = nbuf.len;
            p->addr = (nai_sockaddr_t*)nai_palloc(c->pool, nbuf.len);
            if (p->addr == 0) {
                nai_log_alert(NAI_LOG_CORE, 
                    nai_errno, "alloc buffer for address failed");
                r = -1;
                goto _end;
            };

            nai_memcpy(p->addr, &nbuf.addr, nbuf.len);
        };
        break;

    default:
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    if (in) {
        if (p->len > inlen[0]) {
            r = ERANGE;
            goto _end;
        };

        nai_memcpy(in, p->addr, p->len);
        inlen[0] = p->len;
    };

    r = 0;

_end:
    return r;
};


nai_int_t nai_connection_set_name(
    nai_connection_t* c, 
    nai_int_t opt, const nai_str_t* host, nai_int_t port)
{
    intptr_t r;
    intptr_t len;
    char* str;


    switch (opt) {
    case NAI_CONN_SOCKNAME:
    case NAI_CONN_PEERNAME:
        str = nai_str(host);
        len = nai_str_len(host);
        if (nai_str_len(&c->hosts[opt]) >= nai_str_len(host)) {
            nai_memcpy(nai_str(&c->hosts[opt]), str, len + 1);
            nai_str_setl(&c->hosts[opt], len);
        } else {
            r = nai_str_dup(&c->hosts[opt], str, len, c->pool);
            if (r < 0) {
                goto _end;
            };
        };

        c->ports[opt] = port;
        break;

    default:
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };


    r = 0;

_end:
    return (nai_int_t)r;
};


nai_int_t nai_connection_set_addr(
    nai_connection_t* c, 
    nai_int_t opt, const nai_sockaddr_t* in, nai_int_t inlen)
{
    nai_int_t r;
    nai_sockname_t* p;
    nai_sockaddr_t* name;


    switch (opt) {
    case NAI_CONN_SOCKNAME:
    case NAI_CONN_PEERNAME:
        p = &c->addrs[opt];
        if (p->len < inlen) {
            name = (nai_sockaddr_t*)nai_palloc(c->pool, inlen);
            if (name == 0) {
                nai_log_alert(NAI_LOG_CORE, 
                    nai_errno, "alloc buffer for address failed");
                r = -1;
                goto _end;
            };

            p->addr = name;
        };

        nai_memcpy(p->addr, in, inlen);
        p->len = inlen;
        break;

    default:
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };


    r = 0;

_end:
    return r;
};


nai_int_t nai_connection_set_timeout(
    nai_connection_t* c, nai_int_t ops, int32_t msec)
{
    nai_int_t r;
    nai_int_t ev;
    nai_int_t e = 2;
    nai_int_t get;
    uint32_t now;


    if (msec < -1) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    get = 0;

    /* user custom timer */
    ev = 1<<e;
    if (msec != -1) {
        if (ops == NAI_TIMEOP_ADD) {
            now = c->timer[e];
        } else {
            get = 1;
            now = nai_tickcache_to_msec32();
            switch (ops) {
            case NAI_TIMEOP_SET:
            case NAI_TIMEOP_ADD:
                break;
            case NAI_TIMEOP_MIN:
                if ((c->timerset & ev) && 
                    (int32_t)(c->timer[e] - now) <= msec) {
                    /* do nothing */
                    r = 1;
                    goto _end;
                };
                break;
            case NAI_TIMEOP_MAX:
                if ((c->timerset & ev) && 
                    (int32_t)(c->timer[e] - now) >= msec) {
                    /* do nothing */
                    r = 1;
                    goto _end;
                };
                break;
            default:
                nai_errno = EINVAL;
                r = -1;
                goto _end;
            };
        };
        c->timerset |= ev;
        c->timer[e] = now + msec;
        if (c->timer[e] == 0) {
            c->timer[e] = 1;
        };
    } else {
        if (!(c->timerset & ev)) {
            r = 0;
            goto _end;
        };
        c->timerset &= ~ev;
        c->timer[e] = 0;
    };

    /* update timer */
    if (c->timerset) {
        if (get == 0) {
            now = nai_tickcache_to_msec32();
        };
        r = nai_connection_update_timer(c, now);
    } else {
        if (!c->timerevt) {
            r = 0;
            goto _end;
        };

        r = nai_stream_set_timeout(&c->str, 0, -1);
        if (r >= 0) {
            c->timerevt = 0;
        };
    };

_end:
    return r;
};


nai_int_t nai_connection_set_delay(
    nai_connection_t* c, nai_int_t tid, int32_t msec)
{
    nai_int_t r;
    nai_int_t ev;
    uint32_t now;


    if (msec < -1) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };
    if (tid < 0 || tid > NAI_CONN_DELAY_WRITE) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };


    ev = (1 << tid);
    if (msec != -1) {
        c->timerset |= ev;
    } else {
        if (!(c->timerset & ev)) {
            r = 0;
            goto _end;
        };
        c->timerset &= ~ev;
    };

    /* update timer */
    if (c->timerset) {
        now = nai_tickcache_to_msec32();
        if (msec != -1) {
            c->timer[tid] = now + msec;
        };
        r = nai_connection_update_timer(c, now);
    } else {
        if (!c->timerevt) {
            r = 0;
            goto _end;
        };

        r = nai_stream_set_timeout(&c->str, 0, -1);
        if (r >= 0) {
            c->timerevt = 0;
        };
    };

_end:
    return r;
};


nai_int_t nai_connection_close(nai_connection_t* c)
{
    nai_int_t r;
    nai_pool_t* pool;


    r = nai_iobase_close(&c->str);
    if (r < 0) {
        goto _end;
    };

    pool = c->pool;
    if (pool) {
        c->pool = 0;
        nai_pool_close(pool);
    };

    r = 0;

_end:
    return r;
};


