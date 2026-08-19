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
/// @file       nsi_network.c
/// @brief
/// @details
/// @date       2021-05-11
/// @author     xn
/// @version    1.2.0
///
/// ================================================================

#include "nsi_network.h"
#include "nsomeip/core/nsi_log.h"
#include "nsomeip/core/nsi_routing_impl.h"
#include "nai/os/nai_socket.h"
#include "nai/os/nai_stat.h"
#include "nai/runtime/nai_errno.h"
#include "nai/runtime/nai_util.h"


//////////////////////////////////////////////////////////////////////////////
// network


typedef struct nsi_network_route_s {
    nai_rbnode_t ent;
    nsi_endpoint_name_t name;
    nsi_endpoint_name_t interface;
} nsi_network_route_t;


static int nsi_network_close_locks(nsi_network_t* p)
{
    int r;
    int n;


    for (n = 0; n < (int)nai_countof(p->locks); n ++) {
        r = nai_spin_close(&p->locks[n]);
        if (r < 0) {
            nai_log_alert(NSI_LOG_CORE, 
                nai_errno, "failed to close spin lock");
            goto _end;
        };
    };

    r = 0;

_end:
    return r;
};


static int nsi_network_close_endpoints(nsi_network_t* p)
{
    int r;
    int n;
    nai_rbtree_t* t;
    nai_rbnode_t* e;
    nsi_endpoint_t* c;


    for (n = 0; n < (int)nai_countof(p->eps); n ++) {
        t = &p->eps[n];
        e = nai_rbtree_first(t);
        for ( ; e != nai_rbtree_end(t); ) {
            c = nai_containof(e, nsi_endpoint_t, ent);
            e = nai_rbtree_next(e);

            r = nsi_endpoint_close(c);
            if (r < 0) {
                nai_log_alert(NSI_LOG_CORE, 
                    nai_errno, "failed to close endpoint %*.s", 
                    nai_str_len(&c->host), nai_str(&c->host));
                goto _end;
            };
        };
    };

    r = 0;

_end:
    return r;
};


static nai_rbnode_t** nsi_network_find_impl(
    nai_rbtree_t* t, const nsi_endpoint_name_t* name, 
    nsi_serv_t serv, nsi_inst_t inst, nai_rbnode_t** pparent)
{
    int r;
    nai_rbnode_t** n = &nai_rbtree_root(t);
    nai_rbnode_t* parent = nai_rbtree_end(t);
    nsi_endpoint_t* e;
    nsi_servkey_t v;


    v.serv = serv;
    v.inst = inst;

    while (*n) {
        parent = *n;
        e = nai_containof(parent, nsi_endpoint_t, ent);
        r = nai_sockaddr_compare(
            e->name.addr, e->name.len, name->addr, name->len);
        if (r != 0) {
            if (r > 0) {
                n = &parent->rb_left;
            } else {
                n = &parent->rb_right;
            };
            continue;
        };

        r = e->servinst - v.servinst;
        if (r == 0) {
            break;
        } else if (r > 0) {
            n = &parent->rb_left;
        } else {
            n = &parent->rb_right;
        };
    };

    if (pparent) {
        pparent[0] = parent;
    };

    return n;
};


static int nsi_network_to_index(int reliable, int server)
{
    int r;


    r = (!!reliable) + (!!server) * 2;

    return r;
};

/* redo the setsockopt for all the matched endpoint. */
void nsi_network_reconfig_eps(nsi_network_t* p, int flags, nsi_endpoint_name_t* host)
{
    int i;
    int r;
    int server;
    int reliable;
    nai_rbnode_t* node;
    nsi_endpoint_t* ep;
    nai_iobase_t* c;
    nai_cidr_t mmask;
    char buf[140];

    nai_inet_ptoc("224.0.0.0/4", -1, &mmask);

    server = (flags & NSI_EFLAG_SERVER);
    reliable = (flags & NSI_EFLAG_RELIABLE);
    i = nsi_network_to_index(reliable, server);

    if (!server || reliable) {
        /* other types not support by now */
        return;
    }

    for(node = nai_rbtree_begin( &p->eps[i]);
        node != nai_rbtree_end(&p->eps[i]);
        node = nai_rbtree_next(node)) {
        ep = nai_containof(node, nsi_endpoint_t, ent);

        /* skip any other family beside IPv4 */
        if (ep->name.addr->sa_family != AF_INET) {
            continue;
        }

        nai_sockaddr_ntop(ep->name.addr, ep->name.len, buf, sizeof(buf), 1);
        nai_log_debug(NSI_LOG_CORE, 0, "reset or check option for endpoint(%s)", buf);

        c = (nai_iobase_t *)ep->ctx;
        r = nai_sockaddr_match(ep->name.addr, ep->name.len, &mmask);
        /* unicast address*/
        if (r == 0) {
            nai_dgram_set_opt(c, NAI_IO_MULTICAST_IF, (intptr_t) host->addr);
        } else { /* multicast address */
            nai_dgram_set_opt(c, NAI_IO_MULTICAST_IF, (intptr_t) host->addr);
            nai_dgram_set_opt(c, NAI_IO_MULTICAST_LOOP, 0);
            nsi_endpoint_set_opt(ep, NSI_EOPT_REBIND_MCAST, (intptr_t) &ep->name);
        }
    }
}

static nsi_endpoint_t* nsi_network_create_endpoint(
    nsi_network_t* p, const nsi_endpoint_name_t* name, 
    nsi_serv_t serv, nsi_inst_t inst, int flags, nsi_tls_info_t* tls)
{
    int i;
    int server;
    int reliable;
    nai_rbnode_t** n;
    nai_rbnode_t* parent;
    nsi_endpoint_t* e;


    server = (flags & NSI_EFLAG_SERVER);
    reliable = (flags & NSI_EFLAG_RELIABLE);
    i = nsi_network_to_index(reliable, server);
    n = nsi_network_find_impl(&p->eps[i], name, serv, inst, &parent);
    e = (nsi_endpoint_t*)*n;
    if (e != 0) {
        nai_errno = EEXIST;
        goto _end;
    };

    e = nsi_endpoint_create(p, name, serv, inst, flags, tls);
    if (e == 0) {
        goto _end;
    };

    nai_rbtree_link(&p->eps[i], &e->ent, parent, n);
    nai_rbtree_color(&p->eps[i], &e->ent);
    nai_log_info(NSI_LOG_CORE, 0, 
        "create %s endpoint at %s", 
        server ? "server" : "client", nai_str(&e->host));

_end:
    return e;
};

extern nsi_msgpool_ops_t nsi_local_pool_ops;

int nsi_network_init(nsi_network_t* p)
{
    int r;
    int n;


    p->loop = 0;
    p->cb = 0;
    p->rt = 0;
    p->ucode = 0;
    p->bsize = 64 * 1024 - 64;
    p->bcount = 0;
    p->bwmark = 256;
    p->mss = 1500 - 60; /* mss for ipv6 */
    p->recv_mem = 64 * 1024;
    p->recv_mem_hwm = 256;
    p->recv_mem_lwm = 192;
    p->recv_buf_udp = -1;
    p->recv_buf_tcp = -1;
    p->send_buf_udp = -1;
    p->send_buf_tcp = -1;
    p->recv_timeo = 30 * 1000;
    p->send_timeo = 30 * 1000;
    p->recv_segs = 0;
    p->send_segs = 0;
    p->queue_limit = 16 * 1024*1024;
    p->queue_mix_limit = 32 * 1024*1024;
    p->msg_limit = 32 * 1024*1024;
    p->tpm_limit = 32 * 1024*1024;
    p->tpm_max = 256;
    p->use_polling = 1;
    p->use_zerocopy = 1;
    p->use_npc = 0;
    p->npc_segcount = 1;
    p->npc_segsize = 10*1024*1024;
    p->npc_comm = 0;
    p->keepalive_enable = 0;
    p->keepalive_idle = 60;
    p->keepalive_interval = 5;
    p->keepalive_cnt = 5;
    nai_list_init(&p->bufs);
    nai_rbtree_init(&p->route);
    nai_bufpool_init(&p->pool);

    for (n = 0; n < (int)nai_countof(p->eps); n ++) {
        nai_rbtree_init(&p->eps[n]);
    };
    for (n = 0; n < (int)nai_countof(p->binds); n ++) {
        nai_rbtree_init(&p->binds[n]);
    };
    for (n = 0; n < (int)nai_countof(p->locks); n ++) {
        nai_spin_init(&p->locks[n]);
    };
    for (n = 0; n < (int)nai_countof(p->msgpool); n ++) {
        nsi_msgpool_init(&p->msgpool[n], n ? 4096-64 : 512, 0, n);
        p->msgpool[n].ops = 0;
        p->msgpool[n].ud = 0;
    };

    r = 0;


    return r;
};

extern nsi_msgpool_ops_t nsi_local_pool_ops;
extern nsi_msgpool_ops_t nsi_npc_pool_ops;

int nsi_network_open(nsi_network_t* p, nai_evloop_t* loop)
{
    int r;
    int n;
    int e;
    size_t segs;
    size_t size;
    nai_spin_t* lock;


    if (loop == 0) {
        nai_log_error(NSI_LOG_CORE, 
            EINVAL, "must offer a valid evloop");
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };
    if (p->loop) {
        nai_log_error(NSI_LOG_CORE, 
            EPERM, "the network is already opened");
        nai_errno = EPERM;
        r = -1;
        goto _end;
    };


    for (n = 0; n < (int)nai_countof(p->locks); n ++) {
        r = nai_spin_open(&p->locks[n], 0);
        if (r < 0) {
            nai_log_alert(NSI_LOG_CORE, 
                nai_errno, "failed to create spin lock");
            goto _fail;
        };
    };

    lock = &p->locks[NSI_NETWORK_MSG_LOCK];
    for (n = 0; n < (int)nai_countof(p->msgpool); n ++) {
        p->msgpool[n].lock = lock;
    };

    r = nai_bufpool_open(&p->pool, 4096-64, 0);
    if (r < 0) {
        nai_log_alert(NSI_LOG_CORE, 
            nai_errno, "failed to allocate buffer pool");
        goto _fail;
    };


#if (NAI_HAVE_UDP_SEGMENT)
    segs = p->recv_segs;
#else
    segs = 1;
#endif
    segs = segs * 2;
    if (segs < 2) {
        segs = 2;
    };

    /* allocate at least twice the size of mss */
    size = nai_align(p->recv_mem, 4096);
    if (size < p->mss * segs + 8192 + 64) {
        size = p->mss * segs + 8192 + 64;
        size = nai_align(size, 4096);
    };
    if (size < 64 * 1024) {
        size = 64 * 1024;
    };

    /* 64 is the size of allocator metadata */
    p->bsize = (uint32_t)(size - 64);
    p->bwmark = p->recv_mem_hwm;
    p->loop = loop;
    r = 0;

    for (n = 0; n < 2; n ++) {
        p->msgpool[n].ops = &nsi_local_pool_ops;
        p->msgpool[n].ud = 0;
    };

    if (p->use_npc) {
        p->npc_comm = (npc_comm_t*)nsi_routing_alloc(p->rt, sizeof(npc_comm_t));
        if (p->npc_comm == 0) {
            nai_log_alert(NSI_LOG_CORE, nai_errno, 
                "allocate npc comm structure failed");
            r = -1;
            goto _fail;
        };

        npc_comm_init(p->npc_comm);

        p->npc_comm->ud = p;

        r = npc_comm_open(p->npc_comm, loop);
        if (r < 0) {
            nai_log_alert(NSI_LOG_CORE, 
                nai_errno, "failed to open npc comm");
            goto _fail;
        };

        for (n = 0; n < 2; n ++) {
            p->msgpool[n].ops = &nsi_npc_pool_ops;
            p->msgpool[n].ud = p->npc_comm;
        };
        for (n = 2; n < (int)nai_countof(p->msgpool); n ++) {
            p->msgpool[n].ops = &nsi_local_pool_ops;
            p->msgpool[n].ud = 0;
        };
    };

_end:
    return r;

_fail:
    if (p->npc_comm) {
        nsi_routing_free(p->rt, p->npc_comm);
        p->npc_comm = 0;
    };
    e = nai_errno;
    r = nsi_network_close_locks(p);
    assert(r >= 0);
    nai_errno = e;

    (void)r;

    r = -1;
    goto _end;
};


int nsi_network_close(nsi_network_t* p)
{
    int r;
    int n;


    r = nsi_network_close_endpoints(p);
    if (r < 0) {
        nai_log_error(NSI_LOG_CORE, nai_errno, 
            "failed to close endpoints when shutdown network");
        goto _end;
    };

    r = nsi_network_close_locks(p);
    if (r < 0) {
        nai_log_error(NSI_LOG_CORE, nai_errno, 
            "failed to close locks when shutdown network");
        goto _end;
    };

    /* free all message cache */
    for (n = 0; n < (int)nai_countof(p->msgpool); n ++) {
        p->msgpool[n].lock = 0;
        r = nsi_msgpool_close(&p->msgpool[n]);
        if (r < 0) {
            nai_log_error(NSI_LOG_CORE, nai_errno, 
                "failed to close message pool(%d) when shutdown network", n);
            goto _end;
        };
    };

    if (p->use_npc) {
        npc_comm_close(p->npc_comm);
        if (p->npc_comm) {
            nsi_routing_free(p->rt, p->npc_comm);
            p->npc_comm = 0;
        };
    };

    /* free all buffer cache */
    nai_list_init(&p->bufs);
    nai_rbtree_init(&p->route);
    r = nai_bufpool_close(&p->pool);
    if (r < 0) {
        nai_log_error(NSI_LOG_CORE, nai_errno, 
            "failed to close buffer pool when shutdown network");
        goto _end;
    };

    p->loop = 0;
    p->ucode = 0;
    r = 0;

_end:
    return r;
};


nsi_endpoint_t* nsi_network_find(nsi_network_t* p, 
    const nsi_endpoint_name_t* name, 
    nsi_serv_t serv, nsi_inst_t inst, int reliable, int server)
{
    int i;
    nai_rbnode_t** n;
    nsi_endpoint_t* e;


    i = nsi_network_to_index(reliable, server);
    n = nsi_network_find_impl(&p->eps[i], name, serv, inst, 0);

    e = (nsi_endpoint_t*)*n;
    if (e == 0) {
        nai_errno = ENOENT;
        goto _end;
    };


_end:
    return e;
};


uint32_t nsi_network_unique_code(nsi_network_t* p)
{
    uint32_t r;
    int n;
    nai_rbtree_t* t;
    nai_rbnode_t* e;
    nsi_endpoint_t* c;


    r = ++ p->ucode;
    if (r != 0) {
        goto _end;
    };

    for (n = 0; n < (int)nai_countof(p->eps); n ++) {
        t = &p->eps[n];
        e = nai_rbtree_first(t);
        for ( ; e != nai_rbtree_end(t); ) {
            c = (nsi_endpoint_t*)e;
            e = nai_rbtree_next(e);
            nsi_endpoint_set_opt(c, NSI_EOPT_RESET_UCODE, 0);
        };
    };

    r = ++ p->ucode;


_end:
    return r;
};


int nsi_network_map_route(
    nsi_network_t* p, const nai_sockaddr_t* name, int namelen, 
    nai_sockaddr_t* out, int* outlen, int nocache)
{
    int r;
    int ec;
    nai_fd_t s;
    nai_rbnode_t** n;
    nai_rbnode_t* parent;
    nai_socknbuf_t nbuf;
    nsi_network_route_t* m;
    nsi_endpoint_name_t dest;
    const nai_sockaddr_info_t* si;


    si = nai_sockaddr_info(name->sa_family);
    nbuf.len = namelen;
    nai_memcpy(&nbuf.addr, name, namelen);
    nai_sockaddr_set_port(si, &nbuf.addr, 0);

    dest.len = nbuf.len;
    dest.addr = &nbuf.addr;
    n = nsi_endpoint_name_find(&p->route, dest.addr, dest.len, &parent);
    if (n[0] == 0) {
        m = 0;
    } else {
        m = nai_containof(n[0], nsi_network_route_t, ent);
        s = NAI_FD_INVALID;
    };

    if (m == 0 || nocache) {

        nai_log_debug(NSI_LOG_CORE, 0, 
            "network route mapping local interface address");

        s = nai_sock_open(name->sa_family, SOCK_DGRAM, 0);
        if (s == NAI_FD_INVALID) {
            r = -1;
            goto _end;
        };

        r = nai_sock_connect(s, name, namelen);
        if (r < 0) {
            r = -1;
            goto _end;
        };

        nbuf.len = sizeof(nbuf.storage);
        r = nai_sock_get_sockname(s, &nbuf.addr, &nbuf.len);
        if (r < 0) {
            goto _end;
        };

        if (m == 0) {
            m = nsi_network_alloc(p, sizeof(*m) + namelen + nbuf.len);
            if (m == 0) {
                r = -1;
                goto _end;
            };

            m->name.len = namelen;
            m->name.addr = (nai_sockaddr_t*)(m + 1);
            m->interface.len = nbuf.len;
            m->interface.addr = (nai_sockaddr_t*)(
                (char*)m->name.addr + m->name.len);

            nai_memcpy(m->name.addr, name, namelen);
            nai_sockaddr_set_port(si, m->name.addr, 0);

            nai_rbtree_link(&p->route, &m->ent, parent, n);
            nai_rbtree_color(&p->route, &m->ent);
        };

        nai_memcpy(m->interface.addr, &nbuf.addr, nbuf.len);
        nai_sockaddr_set_port(si, m->interface.addr, 0);
    };

    if (out && outlen) {
        if (outlen[0] < m->interface.len) {
            outlen[0] = m->interface.len;
            nai_errno = ERANGE;
            r = -1;
            goto _end;
        };
        nai_memcpy(out, m->interface.addr, m->interface.len);
        outlen[0] = m->interface.len;
    };

    r = 0;

_end:
    if (s != NAI_FD_INVALID) {
        if (r < 0) {
            ec = nai_errno;
        };
        nai_sock_close(s);
        if (r < 0) {
            nai_errno = ec;
        };
    };
    return r;
};


nsi_endpoint_t* nsi_network_create_server(
    nsi_network_t* p, const nsi_endpoint_name_t* name, 
    nsi_serv_t serv, nsi_inst_t inst, int flags, nsi_tls_info_t* tls)
{
    nsi_endpoint_t* e;
#if defined(NAI_HAVE_SOCKADDR_UN)
    int r;
    nai_stat_t st;
    const char* path;
#endif


    switch (name->addr->sa_family) {
    case AF_INET:
#if defined(NAI_HAVE_SOCKADDR_IN6)
    case AF_INET6:
#endif
        flags &= ~NSI_EFLAG_LOCAL;
        break;
#if defined(NAI_HAVE_SOCKADDR_UN)
    case AF_UNIX:
        flags |= NSI_EFLAG_LOCAL;
        path = ((nai_sockaddr_un_t*)name->addr)->sun_path;
        r = nai_stat(path, &st, 0);
        if (r >= 0 && (st.st_mode & NAI_S_IFMT) != NAI_S_IFSOCK) {
            nai_log_error(NSI_LOG_CORE, 0, 
                "failed to create server endpoint "
                "on an inusing path %s", path);
            e = 0;
            goto _end;
        };

        r = nai_file_unlink(path);
        if (r < 0) {
            if (nai_errno != ENOENT) {
                nai_log_error(NSI_LOG_CORE, nai_errno, 
                    "failed to create server endpoint "
                    "on an inusing or invalid path %s", path);
                e = 0;
                goto _end;
            };
        };
        break;
#endif
    default:
        nai_log_error(NSI_LOG_CORE, EAFNOSUPPORT, 
            "failed to create server endpoint "
            "with unknown address family(%d)", name->addr->sa_family);
        nai_errno = EAFNOSUPPORT;
        e = 0;
        goto _end;
    };

    e = nsi_network_create_endpoint(p, 
        name, serv, inst, flags | NSI_EFLAG_SERVER, tls);

_end:
    return e;
};


nsi_endpoint_t* nsi_network_create_client(
    nsi_network_t* p, const nsi_endpoint_name_t* name, 
    nsi_serv_t serv, nsi_inst_t inst, int flags, nsi_tls_info_t* tls)
{
    nsi_endpoint_t* e;


    switch (name->addr->sa_family) {
    case AF_INET:
#if defined(NAI_HAVE_SOCKADDR_IN6)
    case AF_INET6:
#endif
        flags &= ~NSI_EFLAG_LOCAL;
        break;
#if defined(NAI_HAVE_SOCKADDR_UN)
    case AF_UNIX:
        flags |= NSI_EFLAG_LOCAL;
        break;
#endif
    default:
        nai_log_error(NSI_LOG_CORE, EAFNOSUPPORT, 
            "failed to create client endpoint "
            "with unknown address family(%d)", name->addr->sa_family);
        nai_errno = EAFNOSUPPORT;
        e = 0;
        goto _end;
    };

    e = nsi_network_create_endpoint(p, 
        name, serv, inst, flags & ~NSI_EFLAG_SERVER, tls);

_end:
    return e;
};


nsi_endpoint_t* nsi_network_create_custom(
    nsi_network_t* p, const nsi_endpoint_name_t* name, int flags, 
    nsi_endpoint_ops_t* ops, void* ctx)
{
    int i;
    int reliable;
    nai_rbnode_t** n;
    nai_rbnode_t* parent;
    nsi_endpoint_t* e;


    reliable = !!(flags & NSI_EFLAG_RELIABLE);
    i = nsi_network_to_index(reliable, 1);
    n = nsi_network_find_impl(&p->eps[i], 
        name, NSI_SERVICE_ANY, NSI_INSTANCE_ANY, &parent);
    e = (nsi_endpoint_t*)*n;
    if (e != 0) {
        nai_errno = EEXIST;
        goto _end;
    };


    flags |= NSI_EFLAG_SERVER;
    flags |= NSI_EFLAG_LOCAL;
    e = nsi_endpoint_create_custom(p, name, flags, ops, ctx);
    if (e == 0) {
        goto _end;
    };

    nai_rbtree_link(&p->eps[i], &e->ent, parent, n);
    nai_rbtree_color(&p->eps[i], &e->ent);
    nai_log_debug(NSI_LOG_CORE, 0, 
        "create custom endpoint at %s", nai_str(&e->host));


_end:
    return e;
};


nsi_message_t* nsi_network_create_message(nsi_network_t* p, int usage)
{
    nsi_message_t* m;
    m = nsi_message_create(&p->msgpool[!!usage]);
    if (m == 0 && p->use_npc) {
        m = nsi_message_create(&p->msgpool[!!usage + 2]);
    };
    return m;
};



//////////////////////////////////////////////////////////////////////////////
// buf and memory



nai_buf_t* nsi_network_create_buf(nsi_network_t* n)
{
    nai_buf_t* b;
    nai_list_entry_t* e;


    e = n->bufs.next;
    for ( ; e != &n->bufs; e = e->next) {
        b = (nai_buf_t*)e;
        if (b->refcount == 1) {
            nai_list_entry_remove(&b->ent);
            b->size = 0;
            b->total += (b->start - (uint8_t*)b->ref.obj);
            b->start = (uint8_t*)b->ref.obj;
            goto _end;
        };
    };

    b = nai_buf_alloc(&n->pool, n->bsize);
    if (b == 0) {
        nai_log_alert(NSI_LOG_CORE, 
            nai_errno, "failed to allocate buffer");
        goto _end;
    };

    /* mark threading */
    nai_buf_set_threading(b);

    /* setup memory watermark */
    n->bcount ++;
    if (n->bcount > n->recv_mem_hwm) {
        n->bwmark = n->recv_mem_lwm;
    };

_end:
    return b;
};


int nsi_network_release_buf(nsi_network_t* n, nai_buf_t* b)
{
    int r;


    if (n->bcount < n->bwmark) {
        nai_list_insert_tail(&n->bufs, &b->ent);
    } else {
        nai_buf_close(b);
        n->bcount --;
        if (n->bwmark >= n->bcount) {
            n->bwmark = n->recv_mem_hwm;
        };
    };

    r = 0;

    return r;
};



