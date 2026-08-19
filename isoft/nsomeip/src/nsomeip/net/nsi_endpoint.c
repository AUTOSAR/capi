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
/// @file       nsi_endpoint.c
/// @brief
/// @details
/// @date       2021-05-11
/// @author     xn
/// @version    1.2.0
///
/// ================================================================

#include "nsomeip/core/nsi_log.h"
#include "nsi_network.h"
#include "nsi_endpoint.h"
#include "nsomeip/core/nsi_routing.h"
#include "nai/runtime/nai_errno.h"
#include "nai/runtime/nai_util.h"
#include "nai/io/nai_ssl.h"



//////////////////////////////////////////////////////////////////////////////
// endpoint


#define nsi_endpoint_typename(b)                        \
    (b) ? "server" : "client"                           \


extern nsi_endpoint_ops_t nsi_dgram_client_ops;
extern nsi_endpoint_ops_t nsi_dgram_server_ops;
extern nsi_endpoint_ops_t nsi_stream_client_ops;
extern nsi_endpoint_ops_t nsi_stream_server_ops;
extern nsi_endpoint_ops_t nsi_npc_client_ops;
extern nsi_endpoint_ops_t nsi_npc_server_ops;
static nsi_endpoint_ops_t* nsi_endpoint_ops[] = {
    &nsi_dgram_client_ops, 
    &nsi_dgram_server_ops, 
    &nsi_stream_client_ops, 
    &nsi_stream_server_ops, 
    &nsi_npc_client_ops, 
    &nsi_npc_server_ops, 
};

#if defined(NAI_HAVE_SSL)
int nsi_endpoint_sslctx_open(nsi_endpoint_t* e)
{
    int r = 0;
    nai_ssl_ctx_t *ctx = &e->ssl_ctx;

    nai_ssl_ctx_init(ctx);

    if (!e->tls) {
        return 0;
    }
    if (e->server) {
        if (!nai_str_len(&e->tls_info->tls_local_cert) || !nai_str_len(&e->tls_info->tls_local_privkey)) {
            nai_log_alert(NSI_LOG_CORE, EINVAL, "missing cert or key for server");
            r = -1;
            goto _end;
        }
    }

    if (e->tls_info->tls_version == 12) {
        r = nai_ssl_ctx_open(ctx, NAI_SSL_TLSv1_2);
    } else if (e->tls_info->tls_version == 13) {
        r = nai_ssl_ctx_open(ctx, NAI_SSL_TLSv1_3);
    } else {
        r = nai_ssl_ctx_open(ctx, NAI_SSL_TLS);
    }
    if (r < 0) {
        nai_log_alert(NSI_LOG_CORE, nai_errno, "nai_ssl_ctx_open failed");
        goto _end;
    }

    if (e->tls_info->tls_version == 12) {
        r = nai_ssl_ctx_ciphers(ctx, "ECDHE-RSA-AES128-GCM-SHA256");
        if (r < 0) {
            nai_log_alert(NSI_LOG_CORE, nai_errno, "nai_ssl_ctx_ciphers failed");
            goto _end;
        }
    }

    if (nai_str_len(&e->tls_info->tls_local_cert) && nai_str_len(&e->tls_info->tls_local_privkey)) {
        r = nai_ssl_ctx_certificate(ctx, &e->tls_info->tls_local_cert, &e->tls_info->tls_local_privkey, 0, 0);
        if (r < 0) {
            nai_log_alert(NSI_LOG_CORE, nai_errno, "nai_ssl load local certificate failed");
            goto _end;
        }
    }

    if (nai_str_len(&e->tls_info->tls_peer_ca)) {
        if (e->server) {
            r = nai_ssl_ctx_client_certificate(ctx, &e->tls_info->tls_peer_ca);
        } else {
            r = nai_ssl_ctx_trusted_certificate(ctx, &e->tls_info->tls_peer_ca);
        }
        if (r < 0) {
            nai_log_alert(NSI_LOG_CORE, nai_errno, "nai_ssl load peer certificate failed");
            goto _end;
        }
    }

    r = nai_ssl_ctx_verify_depth(ctx, 1);
    if (r < 0) {
        nai_log_alert(NSI_LOG_CORE, nai_errno, "nai_ssl_ctx_verify_depth failed");
        goto _end;
    } else {
        nai_log_info(NSI_LOG_CORE, 0, "ssl_ctx loaded for %s endpoint with address %s",
              nsi_endpoint_typename(e->server), nai_str(&e->host));
    }

    if (e->server && e->tls_info->tls_verify_client) {
        r =  nai_ssl_ctx_set_verify(ctx, NAI_SSL_VERIFY_MODE_PEER 
            | NAI_SSL_VERIFY_MODE_FAIL_IF_NO_PEER_CERT);
        if (r < 0) {
            nai_log_alert(NSI_LOG_CORE, nai_errno, "nai_ssl_ctx_set_verify failed");
        }
    }

_end:
    return r;
}

void nsi_endpoint_sslctx_close(nsi_endpoint_t* e) {
    int r = 0;

    if (!e->tls) {
        return;
    }

    r = nai_ssl_ctx_close(&e->ssl_ctx);
    if (r < 0) {
        nai_log_info(NSI_LOG_CORE, nai_errno, "nai_ssl_ctx_close failed");
    }
}
#else
int nsi_endpoint_sslctx_open(nsi_endpoint_t* e) { (void)e; return 0; }
void nsi_endpoint_sslctx_close(nsi_endpoint_t* e) { (void)e; }
#endif

nsi_endpoint_t* nsi_endpoint_create(
    nsi_network_t* p, const nsi_endpoint_name_t* name, 
    nsi_serv_t serv, nsi_inst_t inst, int flags, nsi_tls_info_t* tls)
{
    intptr_t r;
    int ec;
    int idx;
    int local;
    int server;
    int reliable;
    int npc;
    nsi_endpoint_t* e;
    char host[128];


    local = !!(flags & NSI_EFLAG_LOCAL);
    server = !!(flags & NSI_EFLAG_SERVER);
    reliable = !!(flags & NSI_EFLAG_RELIABLE);
    npc = !!(flags & NSI_EFLAG_NPC);

    r = nai_sockaddr_ntop(name->addr, name->len, 
        host, sizeof(host), NAI_ADDR_PORT|NAI_ADDR_ZANY);
    if (r < 0) {
        nai_log_error(NSI_LOG_CORE, nai_errno, 
            "cannot create %s endpoint with an invalid address", 
            nsi_endpoint_typename(server));
        e = 0;
        goto _end;
    };

    nai_log_info(NSI_LOG_CORE, 
        0, "create %s(%d) endpoint with address %s", 
        server ? "server" : "client", reliable, host);

    e = (nsi_endpoint_t*)nsi_network_alloc(p, sizeof(*e) + name->len + r + 1);
    if (e == 0) {
        nai_log_error(NSI_LOG_CORE, 
            nai_errno, "allocate %s endpoint failed", 
            nsi_endpoint_typename(server));
        goto _end;
    };

    idx = reliable * 2 + server;

    if (npc && reliable) {
        idx = idx + 2;
    };

    e->flags = 0;
    e->waiting = !!(flags & NSI_EFLAG_WAIT);
    e->local = local;
    e->server = server;
    e->reliable = reliable;
#if defined(NAI_HAVE_SSL)
    e->tls = !!(reliable && !local && tls && tls->tls_version);
#else
    e->tls = 0;
#endif
    e->serv = serv;
    e->inst = inst;
    e->net = p;
    e->cb = p->cb;
    e->ud = 0;
    e->ops = nsi_endpoint_ops[idx];
    e->ctx = 0;
    e->pexit = 0;
    e->name.len = name->len;
    e->name.addr = (nai_sockaddr_t*)(e + 1);
    e->host.len = (int)r;
    e->host.ptr = (char*)(e + 1) + name->len;
    e->tls_info = tls;
    nai_memcpy(e->name.addr, name->addr, name->len);
    nai_memcpy(e->host.ptr, host, r + 1);

    /* open endpoint */
    r = nsi_endpoint_sslctx_open(e);
    if (r >= 0) {
    r = e->ops->open(e);
    }
    if (r < 0) {
        ec = nai_errno;
        nai_log_error(NSI_LOG_CORE, ec, 
            "failed to create %s endpoint %s", 
            nsi_endpoint_typename(server), host);
        nsi_endpoint_sslctx_close(e);
        nsi_network_free(p, e);
        nai_errno = ec;
        e = 0;
        goto _end;
    };


_end:
    return e;
};


nsi_endpoint_t* nsi_endpoint_create_custom(
    nsi_network_t* p, const nsi_endpoint_name_t* name, int flags, 
    nsi_endpoint_ops_t* ops, void* ctx)
{
    intptr_t r;
    int ec;
    int local;
    int server;
    int reliable;
    nsi_endpoint_t* e;
    char host[128];


    local = !!(flags & NSI_EFLAG_LOCAL);
    server = !!(flags & NSI_EFLAG_SERVER);
    reliable = !!(flags & NSI_EFLAG_RELIABLE);

    r = nai_sockaddr_ntop(name->addr, name->len, 
        host, sizeof(host), NAI_ADDR_PORT|NAI_ADDR_ZANY);
    if (r < 0) {
        nai_log_error(NSI_LOG_CORE, nai_errno, 
            "cannot create custom endpoint with an invalid address");
        e = 0;
        goto _end;
    };

    nai_log_info(NSI_LOG_CORE, 0, 
        "create custom endpoint with address %s", host);

    e = (nsi_endpoint_t*)nsi_network_alloc(p, sizeof(*e) + name->len + r + 1);
    if (e == 0) {
        nai_log_error(NSI_LOG_CORE, 
            nai_errno, "allocate custom endpoint failed");
        goto _end;
    };

    e->flags = 0;
    e->local = local;
    e->server = server;
    e->reliable = reliable;
    e->tls = 0;
    e->serv = NSI_SERVICE_ANY;
    e->inst = NSI_INSTANCE_ANY;
    e->net = p;
    e->cb = p->cb;
    e->ud = 0;
    e->ops = ops;
    e->ctx = ctx;
    e->pexit = 0;
    e->name.len = name->len;
    e->name.addr = (nai_sockaddr_t*)(e + 1);
    e->host.len = (int)r;
    e->host.ptr = (char*)(e + 1) + name->len;
    nai_memcpy(e->name.addr, name->addr, name->len);
    nai_memcpy(e->host.ptr, host, r + 1);

    /* open endpoint */
    r = e->ops->open(e);
    if (r < 0) {
        ec = nai_errno;
        nai_log_error(NSI_LOG_CORE, ec, 
            "failed to create custom endpoint %s", host);
        nsi_network_free(p, e);
        nai_errno = ec;
        e = 0;
        goto _end;
    };


_end:
    return e;
};


int nsi_endpoint_map_conn(
    nsi_endpoint_t* e, nsi_endpoint_t* s, nsi_connid_t cid)
{
    int r;

    assert(e != 0);
    assert(e->ops != 0);

    r = e->ops->mapconn(e, s, cid);

    return r;
};


int nsi_endpoint_get_name(
    nsi_endpoint_t* e, nsi_connid_t cid, nsi_endpoint_name_t* name)
{
    int r;

    assert(e != 0);
    assert(e->ops != 0);

    r = e->ops->getname(e, cid, name);

    return r;
};


int nsi_endpoint_get_opt(nsi_endpoint_t* e, int opt, intptr_t* value)
{
    int r;

    assert(e != 0);
    assert(e->ops != 0);

    r = e->ops->getopt(e, opt, value);

    return r;
};


int nsi_endpoint_set_opt(nsi_endpoint_t* e, int opt, intptr_t value)
{
    int r;

    assert(e != 0);
    assert(e->ops != 0);

    r = e->ops->setopt(e, opt, value);

    return r;
};


int nsi_endpoint_send(
    nsi_endpoint_t* e, nsi_message_t* m, uint32_t uid, int flags)
{
    int r;

    assert(m != 0);
    assert(e != 0);
    assert(e->ops != 0);

#if defined(NSI_NET_FILTER)
    if (e->net->rt->closeoutgoing && !e->local) {
        r = -1;
        nai_errno = EPERM;
        nai_log_debug(NSI_LOG_CORE, 0, "sending message is disabled");
        return r;
    };
#endif

    r = e->ops->send(e, m, uid, flags);

    return r;
};


int nsi_endpoint_flush(nsi_endpoint_t* e)
{
    int r;

    assert(e != 0);
    assert(e->ops != 0);

    r = e->ops->flush(e);

    return r;
};


int nsi_endpoint_close(nsi_endpoint_t* e)
{
    int r;
    int idx;
    nsi_network_t* p;


    if (e->closing) {
        r = 0;
        goto _end;
    };

    nai_log_info(NSI_LOG_CORE, 
        0, "close %s(%d) endpoint(%s)", 
        e->server ? "server" : "client", e->reliable, nai_str(&e->host));

    /* mark closing */
    e->closing = 1;

    /* close */
    r = e->ops->close(e);
    if (r < 0) {
        nai_log_error(NSI_LOG_CORE, nai_errno, 
            "failed to close %s endpoint %s", 
            nsi_endpoint_typename(e->server), nai_str(&e->host));
        e->closing = 0;
        goto _end;
    };

    /* remove from map */
    p = e->net;
    if (p != 0) {
        idx = e->reliable + e->server * 2;
        nai_rbtree_erase(&p->eps[idx], &e->ent);
        e->net = 0;
    };

    /* mark closed */
    if (e->pexit) {
        e->pexit[0] = 1;
    };

    /* free */
    nsi_endpoint_sslctx_close(e);
    nsi_network_free(p, e);
    r = 0;

_end:
    return r;
};


int nsi_endpoint_name_equal(
    const nsi_endpoint_name_t* a1, 
    const nsi_endpoint_name_t* a2)
{
    int r;
    nai_sockaddr_t* sa1;
    nai_sockaddr_t* sa2;


    sa1 = a1->addr;
    sa2 = a2->addr;
    if (sa1->sa_family != sa2->sa_family) {
        r = 0;
        goto _end;
    };

    switch (sa1->sa_family) {
    case AF_INET:
        r = ((nai_sockaddr_in4_t*)sa1)->sin_port - 
            ((nai_sockaddr_in4_t*)sa2)->sin_port;
        if (r != 0) {
            r = 0;
            break;
        };

        r = !nai_memcmp(
            &((nai_sockaddr_in4_t*)sa1)->sin_addr, 
            &((nai_sockaddr_in4_t*)sa2)->sin_addr, 
            sizeof(nai_addr_in4_t));

        break;
#if defined(NAI_HAVE_SOCKADDR_IN6)
    case AF_INET6:
        r = ((nai_sockaddr_in6_t*)sa1)->sin6_port - 
            ((nai_sockaddr_in6_t*)sa2)->sin6_port;
        if (r != 0) {
            r = 0;
            break;
        };

        r = ((nai_sockaddr_in6_t*)sa1)->sin6_scope_id - 
            ((nai_sockaddr_in6_t*)sa2)->sin6_scope_id;
        if (r != 0) {
            r = 0;
            break;
        };

        r = !nai_memcmp(
            &((nai_sockaddr_in6_t*)sa1)->sin6_addr, 
            &((nai_sockaddr_in6_t*)sa2)->sin6_addr, 
            sizeof(nai_addr_in6_t));
        break;
#endif
    case 0: /* no address */
    default:
        r = 0;
        goto _end;
    };

_end:
    return r;
};



nai_buf_t* nsi_network_create_buf(nsi_network_t* n);


int nsi_network_release_buf(nsi_network_t* n, nai_buf_t* b);


int nsi_endpoint_realloc_buf(nsi_endpoint_t* p, nai_buf_t** pbuf)
{
    int r;
    int ec;
    nai_buf_t* b;
    nai_buf_t* c;
    nsi_network_t* n;


    n = p->net;
    b = pbuf[0];
    if (b != 0) {
        /* if no extra reference, we direct reset buffer */
        if (b->refcount <= 1) {
            nai_memmove(b->ref.obj, b->start, b->size);
            b->total += (b->start - (uint8_t*)b->ref.obj);
            b->start = (uint8_t*)b->ref.obj;
            r = 0;
            goto _end;
        };
    };

    c = nsi_network_create_buf(n);
    if (c == 0) {
        nai_log_alert(NSI_LOG_CORE, nai_errno, 
            "failed to allocate buffer for endpoint %s", nai_str(&p->host));
        r = -1;
        goto _end;
    };

    if (b != 0) {
        nai_memcpy(c->start, b->start, b->size);
        nai_buf_wcommit(c, b->size);

        r = nsi_network_release_buf(n, b);
        if (r < 0) {
            ec = nai_errno;
            nsi_network_release_buf(n, c);
            nai_errno = ec;
            goto _end;
        };
    };

    pbuf[0] = c;
    r = 0;

_end:
    return r;
};


int nsi_endpoint_release_buf(nsi_endpoint_t* p, nai_buf_t* buf)
{
    int r;


    r = nsi_network_release_buf(p->net, buf);


    return r;
};



//////////////////////////////////////////////////////////////////////////////
// endpoint callbacks


int nsi_endpoint_handle_sent(
    nsi_endpoint_t* p, nai_list_entry_t* msgs, size_t* qsize, int errcode)
{
    int r;
    int* pexit;
    int* pexitbk;
    int exit = 0;
    nsi_message_t* m;


    if (p == 0 || p->cb == 0) {
        r = NAI_DECLINED;
        goto _close;
    };

    pexitbk = p->pexit;
    if (!pexitbk) {
        p->pexit = &exit;
    };
    pexit = p->pexit;

    while (!nai_list_is_empty(msgs)) {
        m = (nsi_message_t*)msgs->next;
        qsize[0] -= m->hdr.len;
        nai_list_entry_remove(&m->ent);
        r = p->cb->sent(p, m, errcode);
        nsi_message_close(m);

        if (*pexit) {
            r = 0;
            goto _close;
        };
    };

    (void)r;

    p->pexit = pexitbk;
    r = NAI_DECLINED;

_close:
    while (!nai_list_is_empty(msgs)) {
        m = (nsi_message_t*)msgs->next;
        qsize[0] -= m->hdr.len;
        nai_list_entry_remove(&m->ent);
        nsi_message_close(m);
    };

    return r;
};


int nsi_endpoint_handle_incoming(nsi_endpoint_t* p, nsi_message_t* m)
{
    int r;
    int* pexit;
    int* pexitbk;
    int exit = 0;

#if defined(NSI_NET_FILTER)
    if (p->net->rt->closeincoming && !p->local) {
        nai_log_debug(NSI_LOG_CORE, 0, "handling incoming message is disabled");
        goto _close;
    };
#endif

    if (p->cb == 0) {
        goto _close;
    };

    pexitbk = p->pexit;
    if (!pexitbk) {
        p->pexit = &exit;
    };
    pexit = p->pexit;

    r = p->cb->message(p, m);

    if (*pexit) {
        r = 0;
        goto _end;
    };

    p->pexit = pexitbk;
    m = 0;

_close:
    (void)r;

    r = NAI_DECLINED;
    if (m) {
        nsi_message_close(m);
    };

_end:
    return r;
};


int nsi_endpoint_handle_connect(nsi_endpoint_t* p, uint32_t cid)
{
    int r;
    int* pexit;
    int* pexitbk;
    int exit = 0;


    nai_log_info(NSI_LOG_CORE, 0, 
        "%s endpoint %s connected, connid %d", 
        nsi_endpoint_typename(p->server), nai_str(&p->host), cid);

    if (p->cb == 0) {
        r = NAI_DECLINED;
        goto _end;
    };

    pexitbk = p->pexit;
    if (!pexitbk) {
        p->pexit = &exit;
    };
    pexit = p->pexit;

    r = p->cb->connected(p, cid);

    if (*pexit) {
        r = 0;
        goto _end;
    };

    (void)r;

    p->pexit = pexitbk;
    r = NAI_DECLINED;

_end:
    return r;
};


int nsi_endpoint_handle_except(
    nsi_endpoint_t* p, uint32_t cid, int errcode, int alert)
{
    int r;
    int* pexit;
    int* pexitbk;
    int exit = 0;


    if (alert) {
        nai_log_alert(NSI_LOG_CORE, errcode, 
            "%s endpoint(%s/%d) error occurred", 
            nsi_endpoint_typename(p->server), nai_str(&p->host), cid);
    } else {
        nai_log_info(NSI_LOG_CORE, errcode, 
            "%s endpoint(%s/%d) disconnected", 
            nsi_endpoint_typename(p->server), nai_str(&p->host), cid);
    };

    if (p->cb == 0) {
        r = NAI_DECLINED;
        goto _end;
    };

    pexitbk = p->pexit;
    if (!pexitbk) {
        p->pexit = &exit;
    };
    pexit = p->pexit;

    if (alert) {
        r = p->cb->alert(p, cid, errcode);
    } else {
        r = p->cb->error(p, cid, errcode);
    };

    if (*pexit) {
        r = 0;
        goto _end;
    };

    (void)r;

    p->pexit = pexitbk;
    r = NAI_DECLINED;

_end:
    return r;
};


int nsi_endpoint_is_signal(nsi_endpoint_t* p, 
    nsi_serv_t serv, nsi_inst_t inst, nsi_eid_t eid)
{
    int r;


    if (p->local) {
        r = 0;
        goto _end;
    };

    if (p->cb == 0) {
        r = 0;
        goto _end;
    };

    if (p->cb->is_signal == 0) {
        r = 0;
        goto _end;
    };

    r = p->cb->is_signal(p, serv, inst, eid);

_end:
    return r;
};


int nsi_endpoint_is_subnet(nsi_endpoint_t* p, const nsi_endpoint_name_t* n)
{
    int r;


    if (p->cb == 0) {
        r = 1;
        goto _end;
    };

    if (p->cb->is_subnet == 0) {
        r = 1;
        goto _end;
    };

    r = p->cb->is_subnet(p, n);

_end:
    return r;
};


int nsi_endpoint_get_bind(nsi_endpoint_t* p, 
    int reliable, const nsi_endpoint_name_t* n, nsi_range_t** pptr)
{
    int r;


    if (p->cb == 0) {
        nai_errno = ENOENT;
        r = -1;
        goto _end;
    };

    if (p->cb->get_bind == 0) {
        nai_errno = ENOENT;
        r = -1;
        goto _end;
    };

    r = p->cb->get_bind(p, reliable, n, pptr);

_end:
    return r;
};


int nsi_endpoint_get_interface(nsi_endpoint_t* p, nsi_endpoint_name_t* n)
{
    int r;


    if (p->cb == 0) {
        nai_errno = ENOENT;
        r = -1;
        goto _end;
    };

    if (p->cb->get_inferface == 0) {
        nai_errno = ENOENT;
        r = -1;
        goto _end;
    };

    r = p->cb->get_inferface(p, n);

_end:
    return r;
};



//////////////////////////////////////////////////////////////////////////////
// endpoint find

nai_rbnode_t** nsi_endpoint_pair_find(nai_rbtree_t* t,
    const nai_sockaddr_t* name, int namelen,
    const nai_sockaddr_t* r_name,
    nai_rbnode_t** pparent)
{
    int r;
    nai_rbnode_t** n = &nai_rbtree_root(t);
    nai_rbnode_t* parent = nai_rbtree_end(t);
    nsi_endpoint_node_t* e;
    nsi_iobase_t *c;
    nai_sockaddr_t* sa;     /* local address */
    nai_sockaddr_t* r_sa;   /* remote address */


    switch (name->sa_family) {
    case AF_INET:           /* only support Ipv4*/
        while (*n) {
            parent = *n;
            e = nai_containof(parent, nsi_endpoint_node_t, ent);
            c = nai_containof(parent, nsi_iobase_t, node);
            sa = e->name.addr;
            if (sa->sa_family != AF_INET) {
                if (sa->sa_family >= AF_INET) {
                    n = &parent->rb_left;
                } else {
                    n = &parent->rb_right;
                };
                continue;
            };

            r = ((nai_sockaddr_in4_t*)sa)->sin_port -
                ((nai_sockaddr_in4_t*)name)->sin_port;
            if (r != 0) {
                if (r > 0) {
                    n = &parent->rb_left;
                } else {
                    n = &parent->rb_right;
                };
                continue;
            };

            r = nai_memcmp(
                &((nai_sockaddr_in4_t*)sa)->sin_addr,
                &((nai_sockaddr_in4_t*)name)->sin_addr,
                sizeof(nai_addr_in4_t));
            if (r != 0) {
                if (r > 0) {
                    n = &parent->rb_left;
                } else {
                    n = &parent->rb_right;
                };
            }

            r_sa = c->ep->name.addr;
            r = ((nai_sockaddr_in4_t*)r_sa)->sin_port -
                ((nai_sockaddr_in4_t*)r_name)->sin_port;
            if (r != 0) {
                if (r > 0) {
                    n = &parent->rb_left;
                } else {
                    n = &parent->rb_right;
                };
                continue;
            };

            r = nai_memcmp(
                &((nai_sockaddr_in4_t*)r_sa)->sin_addr,
                &((nai_sockaddr_in4_t*)r_name)->sin_addr,
                sizeof(nai_addr_in4_t));
            if (r == 0) {
                break;
            } else if (r > 0) {
                n = &parent->rb_left;
            } else {
                n = &parent->rb_right;
            };
        };
        break;
#if defined(NAI_HAVE_SOCKADDR_UN)
    case AF_UNIX:
        while (*n) {
            parent = *n;
            e = nai_containof(parent, nsi_endpoint_node_t, ent);
            sa = e->name.addr;
            if (sa->sa_family != AF_UNIX) {
                if (sa->sa_family >= AF_UNIX) {
                    n = &parent->rb_left;
                } else {
                    n = &parent->rb_right;
                };
                continue;
            };

            r = nai_offsetof(nai_sockaddr_un_t, sun_path);
            r = nai_strn2cmp(
                ((nai_sockaddr_un_t*)sa)->sun_path, e->name.len - r,
                ((nai_sockaddr_un_t*)name)->sun_path, namelen - r);
            if (r == 0) {
                break;
            } else if (r > 0) {
                n = &parent->rb_left;
            } else {
                n = &parent->rb_right;
            };
        };
        break;
#endif

    default:
        assert(0);
        break;
    };

    if (pparent) {
        pparent[0] = parent;
    };

    return n;
};


nai_rbnode_t** nsi_endpoint_name_find(nai_rbtree_t* t, 
    const nai_sockaddr_t* name, int namelen, nai_rbnode_t** pparent)
{
    int r;
    nai_rbnode_t** n = &nai_rbtree_root(t);
    nai_rbnode_t* parent = nai_rbtree_end(t);
    nsi_endpoint_node_t* e;
    nai_sockaddr_t* sa;


    switch (name->sa_family) {
    case AF_INET:
        while (*n) {
            parent = *n;
            e = nai_containof(parent, nsi_endpoint_node_t, ent);
            sa = e->name.addr;
            if (sa->sa_family != AF_INET) {
                if (sa->sa_family >= AF_INET) {
                    n = &parent->rb_left;
                } else {
                    n = &parent->rb_right;
                };
                continue;
            };

            r = ((nai_sockaddr_in4_t*)sa)->sin_port - 
                ((nai_sockaddr_in4_t*)name)->sin_port;
            if (r != 0) {
                if (r > 0) {
                    n = &parent->rb_left;
                } else {
                    n = &parent->rb_right;
                };
                continue;
            };

            r = nai_memcmp(
                &((nai_sockaddr_in4_t*)sa)->sin_addr, 
                &((nai_sockaddr_in4_t*)name)->sin_addr, 
                sizeof(nai_addr_in4_t));
            if (r == 0) {
                break;
            } else if (r > 0) {
                n = &parent->rb_left;
            } else {
                n = &parent->rb_right;
            };
        };
        break;

#if defined(NAI_HAVE_SOCKADDR_IN6)
    case AF_INET6:
        while (*n) {
            parent = *n;
            e = nai_containof(parent, nsi_endpoint_node_t, ent);
            sa = e->name.addr;
            if (sa->sa_family != AF_INET6) {
                if (sa->sa_family >= AF_INET6) {
                    n = &parent->rb_left;
                } else {
                    n = &parent->rb_right;
                };
                continue;
            };

            r = ((nai_sockaddr_in6_t*)sa)->sin6_port - 
                ((nai_sockaddr_in6_t*)name)->sin6_port;
            if (r != 0) {
                if (r > 0) {
                    n = &parent->rb_left;
                } else {
                    n = &parent->rb_right;
                };
                continue;
            };

            r = ((nai_sockaddr_in6_t*)sa)->sin6_scope_id - 
                ((nai_sockaddr_in6_t*)name)->sin6_scope_id;
            if (r != 0) {
                if (r > 0) {
                    n = &parent->rb_left;
                } else {
                    n = &parent->rb_right;
                };
                continue;
            };

            r = nai_memcmp(
                &((nai_sockaddr_in6_t*)sa)->sin6_addr, 
                &((nai_sockaddr_in6_t*)name)->sin6_addr, 
                sizeof(nai_addr_in6_t));
            if (r == 0) {
                break;
            } else if (r > 0) {
                n = &parent->rb_left;
            } else {
                n = &parent->rb_right;
            };
        };
        break;
#endif

#if defined(NAI_HAVE_SOCKADDR_UN)
    case AF_UNIX:
        while (*n) {
            parent = *n;
            e = nai_containof(parent, nsi_endpoint_node_t, ent);
            sa = e->name.addr;
            if (sa->sa_family != AF_UNIX) {
                if (sa->sa_family >= AF_UNIX) {
                    n = &parent->rb_left;
                } else {
                    n = &parent->rb_right;
                };
                continue;
            };

            r = nai_offsetof(nai_sockaddr_un_t, sun_path);
            r = nai_strn2cmp(
                ((nai_sockaddr_un_t*)sa)->sun_path, e->name.len - r, 
                ((nai_sockaddr_un_t*)name)->sun_path, namelen - r);
            if (r == 0) {
                break;
            } else if (r > 0) {
                n = &parent->rb_left;
            } else {
                n = &parent->rb_right;
            };
        };
        break;
#endif

    default:
        assert(0);
        break;
    };

    if (pparent) {
        pparent[0] = parent;
    };

    return n;
};


nai_rbnode_t** nsi_endpoint_name_lbound(nai_rbtree_t* t, 
    const nai_sockaddr_t* name, int namelen, nai_rbnode_t** pparent)
{
    int r;
    nai_rbnode_t** n = &nai_rbtree_root(t);
    nai_rbnode_t* parent = nai_rbtree_end(t);
    nsi_endpoint_node_t* e;
    nai_sockaddr_t* sa;


    switch (name->sa_family) {
    case AF_INET:
        while (*n) {
            parent = *n;
            e = nai_containof(parent, nsi_endpoint_node_t, ent);
            sa = e->name.addr;
            if (sa->sa_family != AF_INET) {
                if (sa->sa_family >= AF_INET) {
                    n = &parent->rb_left;
                } else {
                    n = &parent->rb_right;
                };
                continue;
            };

            r = ((nai_sockaddr_in4_t*)sa)->sin_port - 
                ((nai_sockaddr_in4_t*)name)->sin_port;
            if (r != 0) {
                if (r > 0) {
                    n = &parent->rb_left;
                } else {
                    n = &parent->rb_right;
                };
                continue;
            };

            r = nai_memcmp(
                &((nai_sockaddr_in4_t*)sa)->sin_addr, 
                &((nai_sockaddr_in4_t*)name)->sin_addr, 
                sizeof(nai_addr_in4_t));
            if (r >= 0) {
                n = &parent->rb_left;
            } else {
                n = &parent->rb_right;
            };
        };
        break;

#if defined(NAI_HAVE_SOCKADDR_IN6)
    case AF_INET6:
        while (*n) {
            parent = *n;
            e = nai_containof(parent, nsi_endpoint_node_t, ent);
            sa = e->name.addr;
            if (sa->sa_family != AF_INET6) {
                if (sa->sa_family >= AF_INET6) {
                    n = &parent->rb_left;
                } else {
                    n = &parent->rb_right;
                };
                continue;
            };

            r = ((nai_sockaddr_in6_t*)sa)->sin6_port - 
                ((nai_sockaddr_in6_t*)name)->sin6_port;
            if (r != 0) {
                if (r > 0) {
                    n = &parent->rb_left;
                } else {
                    n = &parent->rb_right;
                };
                continue;
            };

            r = ((nai_sockaddr_in6_t*)sa)->sin6_scope_id - 
                ((nai_sockaddr_in6_t*)name)->sin6_scope_id;
            if (r != 0) {
                if (r > 0) {
                    n = &parent->rb_left;
                } else {
                    n = &parent->rb_right;
                };
                continue;
            };

            r = nai_memcmp(
                &((nai_sockaddr_in6_t*)sa)->sin6_addr, 
                &((nai_sockaddr_in6_t*)name)->sin6_addr, 
                sizeof(nai_addr_in6_t));
            if (r >= 0) {
                n = &parent->rb_left;
            } else {
                n = &parent->rb_right;
            };
        };
        break;
#endif

#if defined(NAI_HAVE_SOCKADDR_UN)
    case AF_UNIX:
        while (*n) {
            parent = *n;
            e = nai_containof(parent, nsi_endpoint_node_t, ent);
            sa = e->name.addr;
            if (sa->sa_family != AF_UNIX) {
                if (sa->sa_family >= AF_UNIX) {
                    n = &parent->rb_left;
                } else {
                    n = &parent->rb_right;
                };
                continue;
            };

            r = nai_offsetof(nai_sockaddr_un_t, sun_path);
            r = nai_strn2cmp(
                ((nai_sockaddr_un_t*)sa)->sun_path, e->name.len - r, 
                ((nai_sockaddr_un_t*)name)->sun_path, namelen - r);
            if (r >= 0) {
                n = &parent->rb_left;
            } else {
                n = &parent->rb_right;
            };
        };
        break;
#endif

    default:
        assert(0);
        break;
    };

    if (pparent) {
        pparent[0] = parent;
    };

    return n;
};


nai_rbnode_t** nsi_endpoint_bind_find_conn(
    nai_rbtree_t* t, uint32_t cid, nai_rbnode_t** pparent)
{
    nai_rbnode_t** n = &nai_rbtree_root(t);
    nai_rbnode_t* parent = nai_rbtree_end(t);
    nsi_endpoint_bind_t* e;


    while (*n) {
        parent = *n;
        e = nai_containof(parent, nsi_endpoint_bind_t, entc);
        if (e->cid == cid) {
            break;
        } else if (e->cid >= cid) {
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


nai_rbnode_t** nsi_endpoint_bind_find_name(nai_rbtree_t* t, 
    const nai_sockaddr_t* name, int namelen, nai_rbnode_t** pparent)
{
    return nsi_endpoint_name_find(t, name, namelen, pparent);
};



//////////////////////////////////////////////////////////////////////////////
// iobase


static int nsi_iobase_bind_impl(
    nsi_iobase_t* c, nsi_endpoint_name_t* name, int reliable)
{
    int r;
    int ec;
    int port;
    nai_fd_t s;
    nsi_endpoint_t* ep;
    nsi_endpoint_name_t* dest;
    const nai_sockaddr_info_t* si;


    ep = c->ep;

    s = nai_iobase_get_fd(&c->io);
    while (1) {
        r = nai_sock_bind(s, name->addr, name->len);
        if (r >= 0) {
            if ((!ep->reliable) && (!ep->server)) {
                r = nai_sock_connect(s, ep->name.addr, ep->name.len);
            }
        };
        if (r >= 0) {
            break;
        };

        ec = nai_errno;
        if (ec == EADDRINUSE) {
            break;
        };

        if (ec != EADDRNOTAVAIL) {
            break;
        };

        nai_log_error(NSI_LOG_CORE, ec, 
            "%s endpoint(%s/%d) "
            "bind address isn't available, remap again", 
            reliable ? "stream" : "dgram", 
            nai_str(&ep->host), 0);

        /* save port */
        si = nai_sockaddr_info(name->addr->sa_family);
        port = nai_sockaddr_get_port(si, name->addr);

        /* remap address */
        dest = &ep->name;
        r = nsi_network_map_route(ep->net, 
            dest->addr, dest->len, name->addr, &name->len, 1);
        if (r < 0) {
            nai_errno = EADDRNOTAVAIL;
            break;
        };

        /* restore port */
        nai_sockaddr_set_port(si, name->addr, port);

        /* if port not equal 0 and address is changed, 
         * break and let the caller re-checking the new address */
        if (port != 0) {
            nai_errno = EAGAIN;
            r = -1;
            break;
        };
    };

    return r;
};


static int nsi_iobase_bind_log(nsi_iobase_t* c, int reliable, int bind)
{
    int r;
    nsi_endpoint_t* ep;
    nsi_endpoint_name_t* name;
    char addr[128];


    addr[0] = 0;
    ep = c->ep;
    name = &c->name;

    nai_sockaddr_ntop(name->addr, 
        name->len, addr, sizeof(addr), 1);
    nai_log_debug(NSI_LOG_CORE, 0, 
        "%s endpoint(%s/%d) is %s at %s", 
        reliable ? "stream" : "dgram", 
        nai_str(&ep->host), 0, 
        bind ? "bind" : "unbind", 
        addr);


    r = 0;

    return r;
};


int nsi_iobase_bind(nsi_iobase_t* c, int reliable)
{
    int r;
    int ec;
    int i;
    int bind;
    int type;
    int port;
    int exist;
    int count;
    nsi_range_t* ports;
    nsi_endpoint_t* ep;
    nsi_endpoint_name_t* name;
    nai_fd_t sock;
    nai_rbtree_t* map;
    nai_rbnode_t** n;
    nai_rbnode_t* parent;
    const nai_sockaddr_info_t* si;


    ep = c->ep;
    if (ep->local) {
        r = 0;
        goto _end;
    };


    name = &c->name;
    si = nai_sockaddr_info(name->addr->sa_family);

    /* bind existing addresses first */
    exist = 0;
    if (nai_sockaddr_get_port(si, name->addr)) {
        exist = 1;
    };

    /* create new socket */
    type = reliable ? SOCK_STREAM : SOCK_DGRAM;
    sock = nai_sock_open(name->addr->sa_family, type, 0);
    if (sock == NAI_FD_INVALID) {
        ec = nai_errno;
        nai_log_alert(NSI_LOG_CORE, ec, 
            "%s endpoint(%s/%d) create socket failed", 
            reliable ? "stream" : "dgram", 
            nai_str(&ep->host), 0);

        r = -1;
        goto _alert;
    };


    /* close old and set new socket */
    r = nai_iobase_close(&c->io);
    if (r < 0) {
        ec = nai_errno;
        nai_log_alert(NSI_LOG_CORE, ec, 
            "%s endpoint(%s/%d) close failed", 
            reliable ? "stream" : "dgram", 
            nai_str(&ep->host), 0);

        assert(0);
        nai_sock_close(sock);
        nai_errno = ec;
        r = -1;
        goto _alert;
    };

    nai_iobase_set_mode(&c->io, NAI_IO_READWRITE);
    nai_iobase_set_fd(&c->io, sock, NAI_FD_TYPE_SOCK);
    nai_iobase_set_fdown(&c->io, 1);
    nai_iobase_set_opt(&c->io, NAI_IO_REUSEADDR, 1);

    if (ep->reliable && !ep->server) {
        nai_iobase_set_opt(&c->io, NAI_IO_LINGER, 0);
    }

    bind = 0;
    map = &ep->net->binds[!!reliable];
    if (exist) {

        /* log address opeartion 'unbind' */
        if (nai_log_is_enabled_info(&nsi_log_core)) {
            nsi_iobase_bind_log(c, reliable, 0);
        };

        /* remove */
        nai_rbtree_erase(map, &c->node);

        /* bind */
        r = nsi_iobase_bind_impl(c, name, reliable);
        if (r < 0) {
            ec = nai_errno;
            if (ec == EADDRNOTAVAIL) {
                goto _fail;
            };

        } else {

            /* find insert positioin of map */
            n = nsi_endpoint_pair_find(map, name->addr, name->len, ep->name.addr, &parent);

            /* already bind */
            bind = 1;
        };
    };

    if (bind == 0) {
        /* lookup bind ranges */
        r = nsi_endpoint_get_bind(ep, reliable, &ep->name, &ports);
        if (r <= 0) {
            /* lookup address only */
            port = nai_sockaddr_get_port(si, ep->name.addr);
            nai_sockaddr_set_port(si, ep->name.addr, 0);
            r = nsi_endpoint_get_bind(ep, reliable, &ep->name, &ports);
            nai_sockaddr_set_port(si, ep->name.addr, port);
        };
        if (r <= 0) {

            /* no specified ranges */
            while (1) {
                /* bind to any port */
                nai_sockaddr_set_port(si, name->addr, 0);

                /* bind */
                r = nsi_iobase_bind_impl(c, name, reliable);
                if (r >= 0) {
                    break;
                };

                ec = nai_errno;
                nai_log_error(NSI_LOG_CORE, ec, 
                    "%s endpoint(%s/%d) bind address failed", 
                    reliable ? "stream" : "dgram", 
                    nai_str(&ep->host), 0);

                goto _fail;
            };

            /* get real sockname */
            r = nai_sock_get_sockname(sock, name->addr, &name->len);
            if (r < 0) {
                ec = nai_errno;
                nai_log_error(NSI_LOG_CORE, ec, 
                    "%s endpoint(%s/%d) get sockname failed", 
                    reliable ? "stream" : "dgram", 
                    nai_str(&ep->host), 0);

                goto _fail;
            };

        } else {

            i = 0;
            count = r;
            port = ports[i].start;

            /* use specified ranges */
            while (1) {
                /* get a port from the specilied ranges */
                for ( ; i < count; i ++) {
                    for ( ; port <= (int)ports[i].end; port ++) {
                        nai_sockaddr_set_port(si, name->addr, port);

                        /* find exists */
                        n = nsi_endpoint_pair_find(
                            map, name->addr, name->len, ep->name.addr, &parent);
                        if (n[0] == 0) {
                            break;
                        };
                    };
                    if (port <= (int)ports[i].end) {
                        break;
                    } else {
                        if ((i+1) < count)
                            port = (int)ports[i+1].start;
                    };
                };
                if (i >= count) {
                    ec = EADDRINUSE;
                    nai_log_error(NSI_LOG_CORE, ec, 
                        "all specified address of "
                        "%s endpoint(%s/%d) are in use", 
                        reliable ? "stream" : "dgram", 
                        nai_str(&ep->host), 0);

                    r = -1;
                    goto _fail;
                };

                /* bind */
                r = nsi_iobase_bind_impl(c, name, reliable);
                if (r >= 0) {
                    /* ok */
                    break;
                };

                /* test error */
                ec = nai_errno;
                if (ec == EAGAIN) {
                    /* address is changed, re-checking */
                    continue;
                };
                if (ec != EADDRINUSE) {
                    goto _fail;
                };

                port ++;
            };
        };

        /* find insert positioin of map */
        n = nsi_endpoint_pair_find(map, name->addr, name->len, ep->name.addr, &parent);
    };

    /* log address opeartion 'bind' */
    if (nai_log_is_enabled_info(&nsi_log_core)) {
        nsi_iobase_bind_log(c, reliable, 1);
    };

    /* add into map */
    assert(n[0] == 0);
    nai_rbtree_link(map, &c->node, parent, n);
    nai_rbtree_color(map, &c->node);
    r = 0;

_end:
    return r;

_fail:
    nai_sockaddr_set_port(si, name->addr, 0)
    nai_stream_close(&c->io);
    nai_errno = ec;
    goto _end;

_alert:
    goto _end;
};


int nsi_iobase_close(nsi_iobase_t* c, int reliable)
{
    int r;
    nai_rbtree_t* map;
    nsi_endpoint_t* ep;
    nsi_endpoint_name_t* name;
    const nai_sockaddr_info_t* si;


    ep = c->ep;
    if (ep->local == 0 && ep->server == 0) {
        name = &c->name;
        si = nai_sockaddr_info(name->addr->sa_family);
        if (nai_sockaddr_get_port(si, name->addr)) {

            /* log address opeartion 'unbind' */
            if (nai_log_is_enabled_info(&nsi_log_core)) {
                nsi_iobase_bind_log(c, reliable, 0);
            };

            map = &ep->net->binds[!!reliable];
            nai_rbtree_erase(map, &c->node);
            nai_sockaddr_set_port(si, name->addr, 0);
        };
    };

    r = nai_iobase_close(&c->io);

    return r;
};



