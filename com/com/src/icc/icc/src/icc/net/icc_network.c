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
/// @file       icc_network.c
/// @brief
/// @details
/// @date       2026-02-13
/// @author     jiusen.cui
/// @version    1.2.0
///
/// ================================================================

#include "icc_network.h"

#include "icc/core/icc_log.h"
#include "icc/core/icc_routing_impl.h"
#include "nai/os/nai_socket.h"
#include "nai/os/nai_stat.h"
#include "nai/runtime/nai_errno.h"
#include "nai/runtime/nai_util.h"

//////////////////////////////////////////////////////////////////////////////
// network

static int icc_network_close_locks(icc_network_t* p)
{
    int r;
    int n;

    for (n = 0; n < (int)nai_countof(p->locks); n++) {
        r = nai_spin_close(&p->locks[n]);
        if (r < 0) {
            nai_log_alert(ICC_LOG_CORE, nai_errno, "failed to close spin lock");
            goto _end;
        };
    };

    r = 0;

_end:
    return r;
};

static int icc_network_close_endpoints(icc_network_t* p)
{
    int r;
    int n;
    nai_rbtree_t* t;
    nai_rbnode_t* e;
    icc_endpoint_t* c;

    for (n = 0; n < (int)nai_countof(p->eps); n++) {
        t = &p->eps[n];
        e = nai_rbtree_first(t);
        for (; e != nai_rbtree_end(t);) {
            c = nai_containof(e, icc_endpoint_t, ent);
            e = nai_rbtree_next(e);

            r = icc_endpoint_close(c);
            if (r < 0) {
                nai_log_alert(ICC_LOG_CORE, nai_errno, "failed to close endpoint %d:%s", c->icc_type,
                              nai_str(&c->icc_identifier));
                goto _end;
            };
        };
    };

    r = 0;

_end:
    return r;
};

static nai_rbnode_t** icc_network_find_impl(nai_rbtree_t* t,
                                            uint16_t icc_type,
                                            const nai_str_t* icc_identifier,
                                            icc_serv_t serv,
                                            icc_inst_t inst,
                                            nai_rbnode_t** pparent)
{
    int r;
    nai_rbnode_t** n     = &nai_rbtree_root(t);
    nai_rbnode_t* parent = nai_rbtree_end(t);
    icc_endpoint_t* e;
    icc_servkey_t v;

    (void)icc_type;

    v.serv = serv;
    v.inst = inst;

    while (*n) {
        parent = *n;
        e      = nai_containof(parent, icc_endpoint_t, ent);
        r      = nai_strcmp(nai_str(&e->icc_identifier), nai_str(icc_identifier));
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

static int icc_network_to_index(uint16_t icc_type)
{
    int r;

    r = icc_type;

    return r;
};

icc_endpoint_t* icc_network_create_endpoint(
    icc_network_t* p, uint16_t icc_type, const nai_str_t* icc_identifier, icc_serv_t serv, icc_inst_t inst)
{
    int i;
    nai_rbnode_t** n;
    nai_rbnode_t* parent;
    icc_endpoint_t* e;

    i = icc_network_to_index(icc_type);
    n = icc_network_find_impl(&p->eps[i], icc_type, icc_identifier, serv, inst, &parent);
    e = (icc_endpoint_t*)*n;
    if (e != 0) {
        nai_errno = EEXIST;
        goto _end;
    };

    e = icc_endpoint_create(p, icc_type, icc_identifier, serv, inst);
    if (e == 0) {
        goto _end;
    };

    nai_rbtree_link(&p->eps[i], &e->ent, parent, n);
    nai_rbtree_color(&p->eps[i], &e->ent);
    nai_log_info(ICC_LOG_CORE, 0, "create endpoint %d:%s", icc_type, nai_str(icc_identifier));

_end:
    return e;
};

int icc_network_init(icc_network_t* p)
{
    int r;
    int n;

    p->loop   = 0;
    p->cb     = 0;
    p->rt     = 0;
    p->bsize  = 64 * 1024 - 64;
    p->bcount = 0;
    p->bwmark = 256;

    nai_list_init(&p->bufs);
    nai_bufpool_init(&p->pool);

    for (n = 0; n < (int)nai_countof(p->eps); n++) {
        nai_rbtree_init(&p->eps[n]);
    };
    for (n = 0; n < (int)nai_countof(p->locks); n++) {
        nai_spin_init(&p->locks[n]);
    };
    for (n = 0; n < (int)nai_countof(p->msgpool); n++) {
        icc_msgpool_init(&p->msgpool[n], n ? 4096 - 64 : 512, 0);
    };

    r = 0;

    return r;
};

int icc_network_open(icc_network_t* p, nai_evloop_t* loop)
{
    int r;
    int n;
    int e;
    nai_spin_t* lock;

    if (loop == 0) {
        nai_log_error(ICC_LOG_CORE, EINVAL, "must offer a valid evloop");
        nai_errno = EINVAL;
        r         = -1;
        goto _end;
    };
    if (p->loop) {
        nai_log_error(ICC_LOG_CORE, EPERM, "the network is already opened");
        nai_errno = EPERM;
        r         = -1;
        goto _end;
    };

    for (n = 0; n < (int)nai_countof(p->locks); n++) {
        r = nai_spin_open(&p->locks[n], 0);
        if (r < 0) {
            nai_log_alert(ICC_LOG_CORE, nai_errno, "failed to create spin lock");
            goto _fail;
        };
    };

    lock = &p->locks[ICC_NETWORK_MSG_LOCK];
    for (n = 0; n < (int)nai_countof(p->msgpool); n++) {
        p->msgpool[n].lock = lock;
    };

    r = nai_bufpool_open(&p->pool, 4096 - 64, 0);
    if (r < 0) {
        nai_log_alert(ICC_LOG_CORE, nai_errno, "failed to allocate buffer pool");
        goto _fail;
    };

    /* 64 is the size of allocator metadata */
    p->bsize  = (uint32_t)(64 * 1024 - 64);
    p->bwmark = 64 * 1024 * 1024;
    p->loop   = loop;
    r         = 0;

_end:
    return r;

_fail:
    e = nai_errno;
    r = icc_network_close_locks(p);
    assert(r >= 0);
    nai_errno = e;

    (void)r;

    r = -1;
    goto _end;
};

int icc_network_close(icc_network_t* p)
{
    int r;
    int n;

    r = icc_network_close_endpoints(p);
    if (r < 0) {
        nai_log_error(ICC_LOG_CORE, nai_errno, "failed to close endpoints when shutdown network");
        goto _end;
    };

    r = icc_network_close_locks(p);
    if (r < 0) {
        nai_log_error(ICC_LOG_CORE, nai_errno, "failed to close locks when shutdown network");
        goto _end;
    };

    /* free all message cache */
    for (n = 0; n < (int)nai_countof(p->msgpool); n++) {
        p->msgpool[n].lock = 0;
        r                  = icc_msgpool_close(&p->msgpool[n]);
        if (r < 0) {
            nai_log_error(ICC_LOG_CORE, nai_errno, "failed to close message pool(%d) when shutdown network", n);
            goto _end;
        };
    };

    /* free all buffer cache */
    nai_list_init(&p->bufs);
    r = nai_bufpool_close(&p->pool);
    if (r < 0) {
        nai_log_error(ICC_LOG_CORE, nai_errno, "failed to close buffer pool when shutdown network");
        goto _end;
    };

    p->loop = 0;
    r       = 0;

_end:
    return r;
};

icc_endpoint_t* icc_network_find(
    icc_network_t* p, uint16_t icc_type, const nai_str_t* icc_identifier, icc_serv_t serv, icc_inst_t inst)
{
    int i;
    nai_rbnode_t** n;
    icc_endpoint_t* e;

    i = icc_network_to_index(icc_type);
    n = icc_network_find_impl(&p->eps[i], icc_type, icc_identifier, serv, inst, 0);

    e = (icc_endpoint_t*)*n;
    if (e == 0) {
        nai_errno = ENOENT;
        goto _end;
    };

_end:
    return e;
};

icc_message_t* icc_network_create_message(icc_network_t* p, int usage)
{
    return icc_message_create(&p->msgpool[!!usage]);
};

//////////////////////////////////////////////////////////////////////////////
// buf and memory

nai_buf_t* icc_network_create_buf(icc_network_t* n)
{
    nai_buf_t* b;
    nai_list_entry_t* e;

    e = n->bufs.next;
    for (; e != &n->bufs; e = e->next) {
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
        nai_log_alert(ICC_LOG_CORE, nai_errno, "failed to allocate buffer");
        goto _end;
    };

    /* mark threading */
    nai_buf_set_threading(b);

    /* setup memory watermark */
    n->bcount++;
    if (n->bcount > 256) {
        n->bwmark = 192;
    };

_end:
    return b;
};

int icc_network_release_buf(icc_network_t* n, nai_buf_t* b)
{
    int r;

    if (n->bcount < n->bwmark) {
        nai_list_insert_tail(&n->bufs, &b->ent);
    } else {
        nai_buf_close(b);
        n->bcount--;
        if (n->bwmark >= n->bcount) {
            n->bwmark = 256;
        };
    };

    r = 0;

    return r;
};
