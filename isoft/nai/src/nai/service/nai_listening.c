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
/// @file       nai_listening.c
/// @brief      
/// @details
/// @date       2023-09-19
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#include "nai/service/nai_listening.h"
#include "nai/runtime/nai_util.h"
#include "nai/runtime/nai_errno.h"
#include "nai/runtime/nai_log.h"
#include "nai/runtime/nai_pool.h"
#include <string.h>  /* for memcmp */



static nai_int_t nai_listening_handle(nai_server_t* l, nai_int_t events)
{
    nai_int_t r;
    nai_int_t ec;
    nai_fd_t f = NAI_FD_INVALID;
    nai_listening_t* s = (nai_listening_t*)l;
    nai_sockname_t name;
    nai_socknbuf_t nbuf;


    if (!(events & NAI_EV_READ)) {
        r = 0;
        goto _end;
    }

    for (;;) {
        nbuf.len = sizeof(nbuf.storage);
        f = nai_server_accept(&s->l, &nbuf.addr, &nbuf.len);
        if (f == NAI_FD_INVALID) {
            ec = nai_errno;
            if (ec != NAI_EAGAIN && ec != NAI_EINPROGRESS) {
                nai_log_alert(NAI_LOG_CORE, ec, "accept failed");
                r = -1;
                goto _end;
            };
            break;
        };

        nai_sockname_setbuf(&name, &nbuf);
        s->cb(s, f, &name);
    };

    r = 0;

_end:
    return r;
};


nai_listening_t* nai_listening_remap(
    nai_listening_t* s, nai_fd_t f, nai_socknbuf_t* sock)
{
    nai_int_t r;
    nai_int_t cache = 0;
    nai_int_t addr_off;
    nai_int_t addr_len;
    nai_listening_t* c;
    nai_socknbuf_t nbuf;


    if (sock) {
        /* try get name from cache */
        r = nai_server_cache(&s->l, 
            NAI_IO_CACHE_SOCKNAME, &sock->addr, sizeof(sock->storage));
        if (r > 0) {
            cache = 1;
            sock->len = r;
        };
    };

    if (!s->any) {
        if (sock && !cache) {
            nai_socknbuf_copy(sock, &s->name);
        };
        c = s;
    } else if (!s->sibling) {
        c = s;
        if (sock && !cache) {
            sock->len = 0;
        };
    } else {
        if (!cache) {
            if (sock == 0) {
                sock = &nbuf;
            };

            sock->len = sizeof(sock->storage);
            r = nai_sock_get_sockname(f, &sock->addr, &sock->len);
            if (r < 0) {
                nai_log_alert(NAI_LOG_CORE, nai_errno, "getsockname failed");
                c = 0;
                goto _fail;
            };
        };

        addr_off = s->addr_off;
        addr_len = s->addr_len;
        c = s->sibling;
        for ( ; c; c = c->next) {
            if (nai_memcmp(
                (uint8_t*)&sock->addr + addr_off, 
                (uint8_t*)&c->name.addr + addr_off, addr_len) != 0) {
                continue;
            };
            break;
        };
        if (c == 0) {
            c = s;
        };
    };

_fail:
    return c;
};



static nai_listening_t* nai_listening_map_bind_impl(
    nai_listening_map_t* p, const nai_sockaddr_t* name, nai_int_t namelen, 
    void* key, nai_listening_handle_f cb)
{
    nai_int_t conflict;
    nai_int_t any = 0;
    nai_int_t family;
    nai_int_t addr_off;
    nai_int_t addr_len;
    uint16_t port;
    nai_listening_t* s;
    nai_listening_t* n;
    nai_listening_t* child;
    nai_listening_t* parent;
    nai_sockaddr_in4_t* in4;
    const nai_sockaddr_info_t* af;

#if (NAI_HAVE_SOCKADDR_IN6)
    struct in6_addr zero6;
    nai_sockaddr_in6_t* in6;
#endif


    family = name->sa_family;
    switch (family) {
    case AF_INET:
        in4 = (nai_sockaddr_in4_t*)name;
        if (in4->sin_addr.s_addr == 0) {
            any = 1;
        };
        break;
#if (NAI_HAVE_SOCKADDR_IN6)
    case AF_INET6:
        in6 = (nai_sockaddr_in6_t*)name;
        nai_memset(&zero6, 0, sizeof(zero6));
        if (nai_memcmp(&in6->sin6_addr, &zero6, sizeof(zero6)) == 0) {
            any = 1;
        };
        break;
#endif
    default:
        break;
    };

    /* */
    af = nai_sockaddr_info(family);
    if (af) {
        addr_off = af->addr_off;
        addr_len = af->addr_len;
        port = nai_sockaddr_get_port(af, name);
    } else {
        addr_off = 0;
        addr_len = namelen;
        port = 0;
    };


    parent = 0;
    child = 0;
    conflict = 0;

    /* scan list, find address confict */
    s = p->list;
    for ( ; s; s = s->next) {
        if (family != s->name.addr.sa_family) {
            continue;
        };

        if (family != AF_INET && family != AF_INET6) {
            if (nai_sockaddr_compare(
                name, namelen, &s->name.addr, s->name.len) == 0) {
                goto _exist;
            };
            continue;
        };

        /* compare port */
        if (port == 0 || port == s->port) {
            continue;
        };
        /* port conflict */
        if (any) {
            if (s->any) {
                goto _exist;
            } else if (!s->top) {
                /* conflict with s' parent, skip */
                conflict = 1;
            } else {
                s->sibling = child;
                child = s;
            };
        } else {
            if (s->any) {
                parent = s;
            } else {
                /* compare address */
                if (nai_memcmp(
                    (uint8_t*)name + addr_off, 
                    (uint8_t*)&s->name.addr + addr_off, addr_len) != 0) {
                    continue;
                };
                goto _exist;
            };
        };
    };

    if (conflict) {
        /* error: shuoldn't enter here */
        nai_log_error(NAI_LOG_CORE, 0, 
            "address conflict, buf the listening is not found");

        nai_errno = EADDRINUSE;
        n = 0;
        goto _fail;
    };
    if (key == 0) {
        /* find operation */
        nai_errno = ENOENT;
        n = 0;
        goto _fail;
    };

    /* alloc a node */
    n = p->pool ? nai_palloc(p->pool, sizeof(*n)) : nai_malloc(sizeof(*n));
    if (n == 0) {
        nai_log_alert(NAI_LOG_CORE, 
            nai_errno, "can't allocate nai_listening_t");

        goto _fail;
    };

    /* initialize node */
    nai_server_init(&n->l);
    nai_memcpy(&n->name.addr, name, namelen);
    n->addr_info = af;
    n->addr_off = addr_off;
    n->addr_len = addr_len;
    n->port = port;
    n->backlog = 0;
    n->sendbuf = 0;
    n->recvbuf = 0;
    n->flags = 0;
    n->any = any;
    n->key = key;
    n->next = p->list;
    p->list = n;
    p->count ++;

    /* link node */
    if (parent) {
        n->sibling = parent->sibling;
        parent->sibling = n;
    } else {
        n->top = 1;
        n->sibling = child;
        for ( ; child; child = child->sibling) {
            child->top = 0;
        };
    };

_set:
    n->cb = cb;

_fail:
    return n;

_exist:
    if (key == 0) {
        return s;
    };
    if (key == s->key) {
        /* same key, reset */
        n = s;
        goto _set;
    };

    nai_log_warn(NAI_LOG_CORE, 0, "found address conflict");

    /* address conflict */
    nai_errno = EADDRINUSE;
    n = 0;
    goto _fail;
};


nai_int_t nai_listening_map_init(nai_listening_map_t* p, nai_pool_t* pool)
{
    p->pool = pool;
    p->list = 0;
    p->count = 0;
    p->start = 0;
    return 0;
};


nai_listening_t* nai_listening_map_find(
    nai_listening_map_t* s, const nai_sockaddr_t* name, nai_int_t namelen)
{
    return nai_listening_map_bind_impl(s, name, namelen, 0, 0);
};


nai_listening_t* nai_listening_map_bind(
    nai_listening_map_t* s, const nai_sockaddr_t* name, nai_int_t namelen, 
    void* key, nai_listening_handle_f cb)
{
    if (key == 0) {
        nai_errno = EINVAL;
        return 0;
    };

    return nai_listening_map_bind_impl(s, name, namelen, key, cb);
};


nai_int_t nai_listening_map_start(nai_listening_map_t* p, nai_evloop_t* l)
{
    nai_int_t r;
    nai_fd_t f;
    nai_listening_t* s;
    nai_listening_t* n;
    nai_listening_t* h = 0;
    nai_listening_t** t = 0;


    /* sort out
     * move to head if top is 1
     * move to tail if top is 0
     **/
    s = p->list;
    for ( ; s; ) {
        n = s;
        s = s->next;
        if (t == 0) {
            t = &n->next;
        };
        if (n->top) {
            n->next = h;
            h = n;
        } else {
            n->next = 0;
            t[0] = n;
            t = &n->next;
        };
    };
    p->list = h;

    /* start listen */
    s = p->list;
    for ( ; s; s = s->next) {
        if (!s->top) {
            break;
        };

        f = nai_sock_open(s->name.addr.sa_family, SOCK_STREAM, 0);
        if (f == NAI_FD_INVALID) {
            nai_log_alert(NAI_LOG_CORE, nai_errno, 
                "can't create socket with family %d", s->name.addr.sa_family);

            r = -1;
            goto _end;
        };

        nai_server_set_cb(&s->l, nai_listening_handle);
        nai_server_set_fd(&s->l, f, NAI_FD_TYPE_SOCK);
        nai_server_set_fdown(&s->l, 1);
        if (s->backlog) {
            nai_server_set_backlog(&s->l, s->backlog);
        };
        if (s->reuse) {
            nai_server_set_opt(&s->l, NAI_IO_REUSEADDR, 1);
        };
        if (s->recvbuf != 0) {
            nai_server_set_opt(&s->l, NAI_IO_RECVBUF, s->recvbuf);
        };
        if (s->sendbuf != 0) {
            nai_server_set_opt(&s->l, NAI_IO_SENDBUF, s->sendbuf);
        };

        r = nai_server_bind(&s->l, l, &s->name.addr, s->name.len);
        if (r < 0) {
            char name[128];
            nai_log_crit(NAI_LOG_CORE, nai_errno, 
                "listen at %s failed", 
                nai_inet_ntop(s->name.addr.sa_family, 
                (uint8_t*)&s->name.addr + s->addr_off, name, sizeof(name)));

            goto _end;
        };
    };

    r = 0;

_end:
    return r;
};


nai_int_t nai_listening_map_stop(nai_listening_map_t* p)
{
    nai_int_t r;
    nai_listening_t* s;

    /* close listen */
    s = p->list;
    for ( ; s; s = s->next) {
        if (!s->top) {
            break;
        };

        r = nai_server_close(&s->l);
        if (r < 0) {
            char name[128];
            nai_log_crit(NAI_LOG_CORE, nai_errno, 
                "close listen %s failed", 
                nai_inet_ntop(s->name.addr.sa_family, 
                (uint8_t*)&s->name.addr + s->addr_off, name, sizeof(name)));

            goto _end;
        };
    };

    r = 0;

_end:
    return r;
};


nai_int_t nai_listening_map_close(nai_listening_map_t* p)
{
    nai_int_t r;
    nai_listening_t* s;
    nai_listening_t* n;


    r = nai_listening_map_stop(p);
    if (r < 0) {
        goto _end;
    };

    /* free listens */
    n = p->list;
    for ( ; n; n = n->next) {
        s = n;
        n = n->next;
        if (!p->pool) {
            nai_free(s);
        };
    };

    p->list = 0;
    r = 0;

_end:
    return r;
};


