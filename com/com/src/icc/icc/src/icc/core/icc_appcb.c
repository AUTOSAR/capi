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
/// @file       icc_appcb.c
/// @brief
/// @details
/// @date       2025-11-27
/// @author     Zan Shigang
/// @version    1.2.0
///
/// ================================================================

#include "icc/core/icc_app.h"
#include "icc/core/icc_conf.h"
#include "icc/net/icc_message.h"
#include "icc_log.h"
#include "nai/os/nai_socket.h"
#include "nai/runtime/nai_errno.h"
#include "nai/runtime/nai_util.h"

typedef struct icc_appcb_meth_s
{
    nai_rbnode_t ent;
    icc_method_t meth;
    icc_app_sent_f fsent;
    icc_app_message_f fmsg;
    void* usent;
    void* umsg;
} icc_appcb_meth_t;

typedef struct icc_appcb_serv_s
{
    nai_rbnode_t ent;

    /* service id */
    union
    {
        /**
         * keep serv is high word, 
         * so that the same serv is continuously in the tree.
         */
        struct
        {
            uint32_t inst : 16;
            uint32_t serv : 16;
        };
        icc_servinst_t servinst;
    };

    /* stat */
    uint8_t available;
    icc_version_t major;
    icc_versmin_t minor;

    /* callback */
    void* ud;
    icc_app_sent_f sent;
    icc_app_message_f msg;
    icc_app_available_f avail;
    icc_app_subscribe_f sub;
    icc_app_subscribe_ack_f ack;

    /* map of methods and events */
    nai_rbtree_t meths;

    icc_method_t method;

} icc_appcb_serv_t;

static nai_rbnode_t** icc_appcb_find_method(icc_appcb_serv_t* p, icc_method_t meth, nai_rbnode_t** pparent)
{
    nai_rbnode_t** n     = &nai_rbtree_root(&p->meths);
    nai_rbnode_t* parent = nai_rbtree_end(&p->meths);
    icc_appcb_meth_t* e;

    while (*n) {
        parent = *n;
        e      = (icc_appcb_meth_t*)parent;
        if (e->meth == meth) {
            break;
        } else if (e->meth >= meth) {
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

static nai_rbnode_t** icc_appcb_find_service(icc_appcb_t* p, icc_serv_t serv, icc_inst_t inst, nai_rbnode_t** pparent)
{
    nai_rbnode_t** n     = &nai_rbtree_root(&p->servs);
    nai_rbnode_t* parent = nai_rbtree_end(&p->servs);
    icc_appcb_serv_t* e;
    icc_servkey_t v;

    v.serv = serv;
    v.inst = inst;

    while (*n) {
        parent = *n;
        e      = (icc_appcb_serv_t*)parent;
        if (e->servinst == v.servinst) {
            break;
        } else if (e->servinst >= v.servinst) {
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

static nai_rbnode_t* icc_appcb_lbound_service(icc_appcb_t* p, icc_serv_t serv, icc_inst_t inst)
{
    nai_rbnode_t** n = &nai_rbtree_root(&p->servs);
    nai_rbnode_t* r  = nai_rbtree_end(&p->servs);
    nai_rbnode_t* parent;
    icc_appcb_serv_t* e;
    icc_servkey_t v;

    v.serv = serv;
    v.inst = inst;

    while (*n) {
        parent = *n;
        e      = (icc_appcb_serv_t*)parent;
        if (e->servinst >= v.servinst) {
            r = parent;
            n = &parent->rb_left;
        } else {
            n = &parent->rb_right;
        };
    };

    return r;
};

static icc_appcb_meth_t* icc_appcb_get_method(icc_appcb_serv_t* p, icc_method_t meth, int na)
{
    icc_appcb_meth_t* r;
    nai_rbnode_t** n;
    nai_rbnode_t* parent;

    n = icc_appcb_find_method(p, meth, &parent);
    if (n[0] != 0) {
        r = nai_containof(n[0], icc_appcb_meth_t, ent);
        goto _end;
    };

    if (na) {
        nai_errno = ENOENT;
        r         = 0;
        goto _end;
    };

    r = (icc_appcb_meth_t*)nai_malloc(sizeof(*r));
    if (r == 0) {
        nai_log_alert(ICC_LOG_CORE, nai_errno, "the appcb failed to allocate memory of method");
        goto _end;
    };

    r->meth  = meth;
    r->fmsg  = 0;
    r->umsg  = 0;
    r->fsent = 0;
    r->usent = 0;
    nai_rbtree_link(&p->meths, &r->ent, parent, n);
    nai_rbtree_color(&p->meths, &r->ent);

_end:
    return r;
};

static icc_appcb_serv_t* icc_appcb_get_service(icc_appcb_t* p, icc_serv_t serv, icc_inst_t inst, int na)
{
    icc_appcb_serv_t* r;
    nai_rbnode_t** n;
    nai_rbnode_t* parent;

    n = icc_appcb_find_service(p, serv, inst, &parent);
    if (n[0] != 0) {
        r = nai_containof(n[0], icc_appcb_serv_t, ent);
        goto _end;
    };

    if (na) {
        nai_errno = ENOENT;
        r         = 0;
        goto _end;
    };

    r = (icc_appcb_serv_t*)nai_malloc(sizeof(*r));
    if (r == 0) {
        nai_log_alert(ICC_LOG_CORE, nai_errno, "the appcb failed to allocate memory of service");
        goto _end;
    };

    r->serv      = serv;
    r->inst      = inst;
    r->available = 0;
    r->major     = 0;
    r->minor     = 0;
    r->sub       = 0;
    r->ack       = 0;
    r->avail     = 0;
    r->msg       = 0;
    r->sent      = 0;
    r->ud        = 0;
    nai_rbtree_init(&r->meths);
    nai_rbtree_link(&p->servs, &r->ent, parent, n);
    nai_rbtree_color(&p->servs, &r->ent);

_end:
    return r;
};

static int icc_appcb_handle_sent(icc_app_t* a, void* ud, icc_message_t* m, int errcode)
{
    int r;
    icc_app_sent_f cb = 0;
    icc_appcb_t* p    = (icc_appcb_t*)ud;
    icc_appcb_serv_t* s;
    icc_appcb_meth_t* d;

    nai_spin_lock(&p->lock);

    s = icc_appcb_get_service(p, m->hdr.serv, m->inst, 1);
    if (s != 0) {
        d = icc_appcb_get_method(s, m->hdr.method, 1);
        if (d != 0) {
            cb = d->fsent;
            ud = d->usent;
        } else {
            cb = s->sent;
            ud = s->ud;
        };
    };

    nai_spin_unlock(&p->lock);

    if (cb) {
        r = cb(a, ud, m, errcode);
    } else {
        r = 0;
    };

    return r;
};

static int icc_appcb_handle_message(icc_app_t* a, void* ud, icc_message_t* m)
{
    int r;
    icc_app_message_f cb = 0;
    icc_appcb_t* p       = (icc_appcb_t*)ud;
    icc_appcb_serv_t* s;
    icc_appcb_meth_t* d;

    nai_spin_lock(&p->lock);

    s = icc_appcb_get_service(p, m->hdr.serv, m->inst, 1);
    if (s != 0) {
        if (m->hdr.type == ICC_MT_REQUEST) {
            m->hdr.method = s->method;
        };
        d = icc_appcb_get_method(s, m->hdr.method, 1);
        if (d != 0) {
            cb = d->fmsg;
            ud = d->umsg;
        } else {
            cb = s->msg;
            ud = s->ud;
        };
    };

    nai_spin_unlock(&p->lock);

    if (cb) {
        r = cb(a, ud, m);
    } else {
        r = icc_message_close(m);
    };

    return r;
};

static int icc_appcb_handle_available(icc_app_t* a,
                                      void* ud,
                                      int avail,
                                      icc_serv_t serv,
                                      icc_inst_t inst,
                                      icc_version_t major,
                                      icc_versmin_t minor,
                                      const icc_cred_t* cred)
{
    int r;
    icc_app_available_f cb = 0;
    icc_appcb_t* p         = (icc_appcb_t*)ud;
    icc_appcb_serv_t* s;

    (void)cred;

    nai_spin_lock(&p->lock);

    s = icc_appcb_get_service(p, serv, inst, 0);
    if (s != 0) {
        s->available = (uint8_t)avail;
        if (avail) {
            s->major = major;
            s->minor = minor;
        };

        cb = s->avail;
        ud = s->ud;
        if (cb == 0) {
            s = icc_appcb_get_service(p, serv, ICC_INSTANCE_ANY, 1);
            if (s != 0) {
                cb = s->avail;
                ud = s->ud;
            };
        };
    };

    nai_spin_unlock(&p->lock);

    if (cb) {
        r = cb(a, ud, avail, serv, inst, major, minor, 0);
    } else {
        r = 0;
    };

    return r;
};

static int icc_appcb_handle_subscribe(icc_app_t* a,
                                      void* ud,
                                      icc_cid_t cid,
                                      icc_serv_t serv,
                                      icc_inst_t inst,
                                      icc_gid_t gid,
                                      icc_eid_t eid,
                                      const icc_cred_t* cred)
{
    int r;
    (void)a;
    (void)ud;
    (void)cid;
    (void)serv;
    (void)inst;
    (void)gid;
    (void)eid;
    (void)cred;

    r = 0;

    return r;
};

static int icc_appcb_handle_subscribe_ack(
    icc_app_t* a, void* ud, int rcode, icc_serv_t serv, icc_inst_t inst, icc_gid_t gid, icc_eid_t eid)
{
    int r;
    icc_app_subscribe_ack_f cb = 0;
    icc_appcb_t* p             = (icc_appcb_t*)ud;
    icc_appcb_serv_t* s;

    nai_spin_lock(&p->lock);

    s = icc_appcb_get_service(p, serv, inst, 1);
    if (s != 0) {
        cb = s->ack;
        ud = s->ud;
    };

    nai_spin_unlock(&p->lock);

    if (cb) {
        r = cb(a, ud, rcode, serv, inst, gid, eid);
    } else {
        r = 0;
    };

    return r;
};

int icc_appcb_init(icc_appcb_t* p)
{
    int r;

    p->app = 0;
    nai_spin_init(&p->lock);
    nai_rbtree_init(&p->servs);
    r = 0;

    return r;
};

int icc_appcb_open(icc_appcb_t* p, icc_app_t* a)
{
    int r;

    r = nai_spin_open(&p->lock, 0);
    if (r < 0) {
        goto _end;
    };

    p->app = a;
    icc_app_set_ud(a, p);
    icc_app_set_cb_available(a, icc_appcb_handle_available);
    icc_app_set_cb_sent(a, icc_appcb_handle_sent);
    icc_app_set_cb_message(a, icc_appcb_handle_message);
    icc_app_set_cb_subscribe(a, icc_appcb_handle_subscribe);
    icc_app_set_cb_subscribe_ack(a, icc_appcb_handle_subscribe_ack);

_end:
    return r;
};

int icc_appcb_close(icc_appcb_t* p)
{
    int r;
    nai_rbnode_t* e1;
    nai_rbnode_t* e2;
    icc_appcb_serv_t* s;
    icc_appcb_meth_t* m;
    icc_app_t* a;

    /* remove monitor */
    if (p->app) {
        a = p->app;
        icc_app_set_cb_available(a, 0);
        icc_app_set_cb_sent(a, 0);
        icc_app_set_cb_message(a, 0);
        icc_app_set_cb_subscribe(a, 0);
        icc_app_set_cb_subscribe_ack(a, 0);
        icc_app_set_ud(a, 0);
        p->app = 0;
    };

    (void)r;

    r = nai_spin_lock(&p->lock);
    if (r < 0) {
        goto _end;
    };

    /* free each service */
    e1 = nai_rbtree_begin(&p->servs);
    for (; e1 != nai_rbtree_end(&p->servs);) {
        s  = nai_containof(e1, icc_appcb_serv_t, ent);
        e1 = nai_rbtree_next(e1);

        /* free each method */
        e2 = nai_rbtree_begin(&s->meths);
        for (; e2 != nai_rbtree_end(&s->meths);) {
            m  = nai_containof(e2, icc_appcb_meth_t, ent);
            e2 = nai_rbtree_next(e2);

            nai_rbtree_erase(&s->meths, &m->ent);
            nai_free(m);
        };

        nai_rbtree_erase(&p->servs, &s->ent);
        nai_free(s);
    };

    /* clear appcb */
    nai_spin_unlock(&p->lock);
    nai_spin_close(&p->lock);
    r = 0;

_end:
    return r;
};

void* icc_appcb_get_ud(icc_appcb_t* p, icc_serv_t serv, icc_inst_t inst)
{
    int ec;
    void* r;
    icc_appcb_serv_t* s;

    nai_spin_lock(&p->lock);

    s = icc_appcb_get_service(p, serv, inst, 1);
    if (s == 0) {
        goto _fail;
    };

    r = s->ud;

    nai_spin_unlock(&p->lock);

_end:
    return r;

_fail:
    ec = nai_errno;
    nai_spin_unlock(&p->lock);
    nai_errno = ec;
    r         = 0;
    goto _end;
};

int icc_appcb_set_ud(icc_appcb_t* p, icc_serv_t serv, icc_inst_t inst, void* ud)
{
    int r;
    int ec;
    icc_appcb_serv_t* s;

    nai_spin_lock(&p->lock);

    s = icc_appcb_get_service(p, serv, inst, 0);
    if (s == 0) {
        goto _fail;
    };

    s->ud = ud;

    nai_spin_unlock(&p->lock);
    r = 0;

_end:
    return r;

_fail:
    ec = nai_errno;
    nai_spin_unlock(&p->lock);
    nai_errno = ec;
    r         = -1;
    goto _end;
};

int icc_appcb_set_available(icc_appcb_t* p, icc_serv_t serv, icc_inst_t inst, icc_app_available_f cb)
{
    int r;
    int ec;
    icc_appcb_serv_t* s;

    nai_spin_lock(&p->lock);

    s = icc_appcb_get_service(p, serv, inst, cb == 0);
    if (s == 0) {
        goto _fail;
    };

    s->avail = cb;

_skip:
    nai_spin_unlock(&p->lock);
    r = 0;

_end:
    return r;

_fail:
    (void)r;

    if (cb == 0) {
        goto _skip;
    };

    ec = nai_errno;
    nai_spin_unlock(&p->lock);
    nai_errno = ec;
    r         = -1;
    goto _end;
};

int icc_appcb_set_subscribe(icc_appcb_t* p, icc_serv_t serv, icc_inst_t inst, icc_app_subscribe_f cb)
{
    int r;
    int ec;
    icc_appcb_serv_t* s;

    nai_spin_lock(&p->lock);

    s = icc_appcb_get_service(p, serv, inst, cb == 0);
    if (s == 0) {
        goto _fail;
    };

    s->sub = cb;

_skip:
    nai_spin_unlock(&p->lock);
    r = 0;

_end:
    return r;

_fail:
    (void)r;

    if (cb == 0) {
        goto _skip;
    };

    ec = nai_errno;
    nai_spin_unlock(&p->lock);
    nai_errno = ec;
    r         = -1;
    goto _end;
};

int icc_appcb_set_subscribe_ack(icc_appcb_t* p, icc_serv_t serv, icc_inst_t inst, icc_app_subscribe_ack_f cb)
{
    int r;
    int ec;
    icc_appcb_serv_t* s;

    nai_spin_lock(&p->lock);

    s = icc_appcb_get_service(p, serv, inst, cb == 0);
    if (s == 0) {
        goto _fail;
    };

    s->ack = cb;

_skip:
    nai_spin_unlock(&p->lock);
    r = 0;

_end:
    return r;

_fail:
    (void)r;

    if (cb == 0) {
        goto _skip;
    };

    ec = nai_errno;
    nai_spin_unlock(&p->lock);
    nai_errno = ec;
    r         = -1;
    goto _end;
};

int icc_appcb_set_sent(icc_appcb_t* p, icc_serv_t serv, icc_inst_t inst, icc_method_t meth, icc_app_sent_f cb, void* ud)
{
    int r;
    int ec;
    icc_appcb_serv_t* s;
    icc_appcb_meth_t* m;

    nai_spin_lock(&p->lock);

    s = icc_appcb_get_service(p, serv, inst, cb == 0);
    if (s == 0) {
        goto _fail;
    };

    if (meth == ICC_METHOD_ANY) {
        s->sent = cb;
    } else {
        m = icc_appcb_get_method(s, meth, cb == 0);
        if (m == 0) {
            goto _fail;
        };

        m->fsent = cb;
        m->usent = ud;
    };

_skip:
    nai_spin_unlock(&p->lock);
    r = 0;

_end:
    return r;

_fail:
    (void)r;

    if (cb == 0) {
        goto _skip;
    };

    ec = nai_errno;
    nai_spin_unlock(&p->lock);
    nai_errno = ec;
    r         = -1;
    goto _end;
};

int icc_appcb_set_message(
    icc_appcb_t* p, icc_serv_t serv, icc_inst_t inst, icc_method_t meth, icc_app_message_f cb, void* ud)
{
    int r;
    int ec;
    icc_appcb_serv_t* s;
    icc_appcb_meth_t* m;

    nai_spin_lock(&p->lock);

    s = icc_appcb_get_service(p, serv, inst, cb == 0);
    if (s == 0) {
        r = -1;
        goto _fail;
    };

    s->msg    = cb;
    s->method = meth;

    m = icc_appcb_get_method(s, meth, cb == 0);
    if (m == 0) {
        goto _fail;
    };

    m->fmsg = cb;
    m->umsg = ud;

_skip:
    nai_spin_unlock(&p->lock);
    r = 0;

_end:
    return r;

_fail:
    (void)r;

    if (cb == 0) {
        goto _skip;
    };

    ec = nai_errno;
    nai_spin_unlock(&p->lock);
    nai_errno = ec;
    r         = -1;
    goto _end;
};

int icc_appcb_is_available(icc_appcb_t* p, icc_serv_t serv, icc_inst_t inst)
{
    int r;
    icc_appcb_serv_t* s;

    nai_spin_lock(&p->lock);

    s = icc_appcb_get_service(p, serv, inst, 1);
    if (s != 0) {
        r = s->available;
    } else {
        r = 0;
    };

    nai_spin_unlock(&p->lock);

    return r;
};

int icc_appcb_get_availables(icc_appcb_t* p, icc_serv_t serv, icc_inst_t inst, nai_array_t* out)
{
    int r;
    int ec;
    nai_rbnode_t* n;
    icc_appcb_serv_t* s;
    icc_service_info_t* si;

    r = 0;
    nai_spin_lock(&p->lock);

    n = icc_appcb_lbound_service(p, serv, 0);
    for (; n != nai_rbtree_end(&p->servs);) {
        s = (icc_appcb_serv_t*)n;
        n = nai_rbtree_next(n);

        if (s->serv != serv) {
            break;
        };
        if (s->available == ICC_AVAIL_OFFLINED) {
            continue;
        };
        if (inst != ICC_INSTANCE_ANY && inst != s->inst) {
            continue;
        };

        si = (icc_service_info_t*)nai_array_push(out);
        if (si == 0) {
            ec = nai_errno;
            nai_log_error(ICC_LOG_CORE, ec, "failed to push service info");
            r = -1;
            goto _end;
        };

        si->serv  = serv;
        si->inst  = s->inst;
        si->major = s->major;
        si->minor = s->minor;
        r++;
    };

_end:
    nai_spin_unlock(&p->lock);

    if (r < 0) {
        nai_errno = ec;
    };
    return r;
};
