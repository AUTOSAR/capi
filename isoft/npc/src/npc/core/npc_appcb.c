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
/// @file       npc_appcb.c
/// @brief
/// @details
/// @date       2022-09-08
/// @author     xn
/// @version    1.2.0
///
/// ================================================================

#include "npc/core/npc_app.h"
#include "npc/core/npc_message.h"
#include "npc_log.h"
#include "nai/os/nai_socket.h"
#include "nai/runtime/nai_array.h"
#include "nai/runtime/nai_errno.h"
#include "nai/runtime/nai_util.h"



typedef struct npc_appcb_meth_s {
    nai_rbnode_t ent;
    npc_method_t meth;
    npc_app_sent_f fsent;
    npc_app_message_f fmsg;
    void* usent;
    void* umsg;
} npc_appcb_meth_t;


typedef struct npc_appcb_serv_s {
    nai_rbnode_t ent;

    /* service id */
    union {
        /**
         * keep serv is high word, 
         * so that the same serv is continuously in the tree.
         */
        struct {
            uint32_t inst:16;
            uint32_t serv:16;
        };
        npc_servinst_t servinst;
    };

    /* stat */
    uint8_t available;
    npc_version_t major;
    npc_versmin_t minor;

    /* callback */
    void* ud;
    npc_app_sent_f sent;
    npc_app_message_f msg;
    npc_app_available_f avail;
    npc_app_subscribe_f sub;
    npc_app_subscribe_ack_f ack;


    /* map of methods and events */
    nai_rbtree_t meths;

} npc_appcb_serv_t;



static nai_rbnode_t** npc_appcb_find_method(
    npc_appcb_serv_t* p, npc_method_t meth, nai_rbnode_t** pparent)
{
    nai_rbnode_t** n = &nai_rbtree_root(&p->meths);
    nai_rbnode_t* parent = nai_rbtree_end(&p->meths);
    npc_appcb_meth_t* e;

    while (*n) {
        parent = *n;
        e = (npc_appcb_meth_t*)parent;
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


static nai_rbnode_t** npc_appcb_find_service(
    npc_appcb_t* p, npc_serv_t serv, npc_inst_t inst, nai_rbnode_t** pparent)
{
    nai_rbnode_t** n = &nai_rbtree_root(&p->servs);
    nai_rbnode_t* parent = nai_rbtree_end(&p->servs);
    npc_appcb_serv_t* e;
    npc_servkey_t v;


    v.serv = serv;
    v.inst = inst;

    while (*n) {
        parent = *n;
        e = (npc_appcb_serv_t*)parent;
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


static nai_rbnode_t* npc_appcb_lbound_service(
    npc_appcb_t* p, npc_serv_t serv, npc_inst_t inst)
{
    nai_rbnode_t** n = &nai_rbtree_root(&p->servs);
    nai_rbnode_t* r = nai_rbtree_end(&p->servs);
    nai_rbnode_t* parent;
    npc_appcb_serv_t* e;
    npc_servkey_t v;


    v.serv = serv;
    v.inst = inst;

    while (*n) {
        parent = *n;
        e = (npc_appcb_serv_t*)parent;
        if (e->servinst >= v.servinst) {
            r = parent;
            n = &parent->rb_left;
        } else {
            n = &parent->rb_right;
        };
    };

    return r;
};


static npc_appcb_meth_t* npc_appcb_get_method(
    npc_appcb_serv_t* p, npc_method_t meth, int na)
{
    npc_appcb_meth_t* r;
    nai_rbnode_t** n;
    nai_rbnode_t* parent;


    n = npc_appcb_find_method(p, meth, &parent);
    if (n[0] != 0) {
        r = nai_containof(n[0], npc_appcb_meth_t, ent);
        goto _end;
    };

    if (na) {
        nai_errno = ENOENT;
        r = 0;
        goto _end;
    };

    r = (npc_appcb_meth_t*)nai_malloc(sizeof(*r));
    if (r == 0) {
        nai_log_alert(NPC_LOG_CORE, nai_errno, 
            "the appcb failed to allocate memory of method");
        goto _end;
    };

    r->meth = meth;
    r->fmsg = 0;
    r->umsg = 0;
    r->fsent = 0;
    r->usent = 0;
    nai_rbtree_link(&p->meths, &r->ent, parent, n);
    nai_rbtree_color(&p->meths, &r->ent);


_end:
    return r;
};


static npc_appcb_serv_t* npc_appcb_get_service(
    npc_appcb_t* p, npc_serv_t serv, npc_inst_t inst, int na)
{
    npc_appcb_serv_t* r;
    nai_rbnode_t** n;
    nai_rbnode_t* parent;


    n = npc_appcb_find_service(p, serv, inst, &parent);
    if (n[0] != 0) {
        r = nai_containof(n[0], npc_appcb_serv_t, ent);
        goto _end;
    };

    if (na) {
        nai_errno = ENOENT;
        r = 0;
        goto _end;
    };

    r = (npc_appcb_serv_t*)nai_malloc(sizeof(*r));
    if (r == 0) {
        nai_log_alert(NPC_LOG_CORE, nai_errno, 
            "the appcb failed to allocate memory of service");
        goto _end;
    };

    r->serv = serv;
    r->inst = inst;
    r->available = 0;
    r->major = 0;
    r->minor = 0;
    r->sub = 0;
    r->ack = 0;
    r->avail = 0;
    r->msg = 0;
    r->sent = 0;
    r->ud = 0;
    nai_rbtree_init(&r->meths);
    nai_rbtree_link(&p->servs, &r->ent, parent, n);
    nai_rbtree_color(&p->servs, &r->ent);


_end:
    return r;
};


static int npc_appcb_handle_sent(npc_app_t* a, 
    void* ud, npc_message_t* m, int errcode)
{
    int r;
    npc_app_sent_f cb = 0;
    npc_appcb_t* p = (npc_appcb_t*)ud;
    npc_appcb_serv_t* s;
    npc_appcb_meth_t* d;


    nai_spin_lock(&p->lock);


    s = npc_appcb_get_service(p, m->hdr.serv, m->inst, 1);
    if (s != 0) {
        d = npc_appcb_get_method(s, m->hdr.method, 1);
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


static int npc_appcb_handle_message(npc_app_t* a, 
    void* ud, npc_message_t* m)
{
    int r;
    npc_app_message_f cb = 0;
    npc_appcb_t* p = (npc_appcb_t*)ud;
    npc_appcb_serv_t* s;
    npc_appcb_meth_t* d;


    nai_spin_lock(&p->lock);


    s = npc_appcb_get_service(p, m->hdr.serv, m->inst, 1);
    if (s != 0) {
        d = npc_appcb_get_method(s, m->hdr.method, 1);
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
        r = npc_message_close(m);
    };

    return r;
};


static int npc_appcb_handle_available(npc_app_t* a, 
    void* ud, int avail, npc_serv_t serv, npc_inst_t inst, 
    npc_version_t major, npc_versmin_t minor)
{
    int r;
    npc_app_available_f cb = 0;
    npc_appcb_t* p = (npc_appcb_t*)ud;
    npc_appcb_serv_t* s;


    nai_spin_lock(&p->lock);


    s = npc_appcb_get_service(p, serv, inst, 0);
    if (s != 0) {
        s->available = (uint8_t)avail;
        if (avail) {
            s->major = major;
            s->minor = minor;
        };

        cb = s->avail;
        ud = s->ud;
        if (cb == 0) {
            s = npc_appcb_get_service(p, serv, NPC_INSTANCE_ANY, 1);
            if (s != 0) {
                cb = s->avail;
                ud = s->ud;
            };
        };
    };

    nai_spin_unlock(&p->lock);


    if (cb) {
        r = cb(a, ud, avail, serv, inst, major, minor);
    } else {
        r = 0;
    };

    return r;
};


static int npc_appcb_handle_subscribe(npc_app_t* a, 
    void* ud, npc_cid_t cid, npc_serv_t serv, npc_inst_t inst, 
    npc_gid_t gid, npc_eid_t eid, const npc_cred_t *cred)
{
    int r;
    npc_app_subscribe_f cb = 0;
    npc_appcb_t* p = (npc_appcb_t*)ud;
    npc_appcb_serv_t* s;

    nai_spin_lock(&p->lock);


    s = npc_appcb_get_service(p, serv, inst, 1);
    if (s != 0) {
        cb = s->sub;
        ud = s->ud;
    };

    nai_spin_unlock(&p->lock);


    if (cb) {
        r = cb(a, ud, cid, serv, inst, gid, eid, cred);
    } else {
        r = cpi_app_subscribe_ack(a, 0, cid, serv, inst, gid, eid);
    };

    return r;
};


static int npc_appcb_handle_subscribe_ack(npc_app_t* a, 
    void* ud, int rcode, npc_serv_t serv, npc_inst_t inst, 
    npc_gid_t gid, npc_eid_t eid)
{
    int r;
    npc_app_subscribe_ack_f cb = 0;
    npc_appcb_t* p = (npc_appcb_t*)ud;
    npc_appcb_serv_t* s;


    nai_spin_lock(&p->lock);


    s = npc_appcb_get_service(p, serv, inst, 1);
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



int npc_appcb_init(npc_appcb_t* p)
{
    int r;

    p->app = 0;
    nai_spin_init(&p->lock);
    nai_rbtree_init(&p->servs);
    r = 0;

    return r;
};


int npc_appcb_open(npc_appcb_t* p, npc_app_t* a)
{
    int r;
    int ec;


    r = nai_spin_open(&p->lock, 0);
    if (r < 0) {
        goto _end;
    };

    p->app = a;
    npc_app_set_ud(a, p);
    npc_app_set_cb_available(a, npc_appcb_handle_available);
    npc_app_set_cb_sent(a, npc_appcb_handle_sent);
    npc_app_set_cb_message(a, npc_appcb_handle_message);
    npc_app_set_cb_subscribe(a, (cpi_app_sub_cb_t)npc_appcb_handle_subscribe);
    npc_app_set_cb_subscribe_ack(a, npc_appcb_handle_subscribe_ack);

    /* monitor all service */
    r = cpi_app_request_service(a, 
        NPC_SERVICE_ANY, NPC_INSTANCE_ANY, NPC_MAJOR_ANY, NPC_MINOR_ANY);
    if (r < 0) {
        ec = nai_errno;
        npc_appcb_close(p);
        nai_errno = ec;
    };

_end:
    return r;
};


int npc_appcb_close(npc_appcb_t* p)
{
    int r;
    nai_rbnode_t* e1;
    nai_rbnode_t* e2;
    npc_appcb_serv_t* s;
    npc_appcb_meth_t* m;
    npc_app_t* a;


    /* remove monitor */
    if (p->app) {
        r = cpi_app_release_service(p->app, 
            NPC_SERVICE_ANY, NPC_INSTANCE_ANY, NPC_MAJOR_ANY, NPC_MINOR_ANY);

        a = p->app;
        npc_app_set_cb_available(a, 0);
        npc_app_set_cb_sent(a, 0);
        npc_app_set_cb_message(a, 0);
        npc_app_set_cb_subscribe(a, 0);
        npc_app_set_cb_subscribe_ack(a, 0);
        npc_app_set_ud(a, 0);
        p->app = 0;
    };

    (void)r;

    r = nai_spin_lock(&p->lock);
    if (r < 0) {
        goto _end;
    };

    /* free each service */
    e1 = nai_rbtree_begin(&p->servs);
    for ( ; e1 != nai_rbtree_end(&p->servs); ) {
        s = nai_containof(e1, npc_appcb_serv_t, ent);
        e1 = nai_rbtree_next(e1);

        /* free each method */
        e2 = nai_rbtree_begin(&s->meths);
        for ( ; e2 != nai_rbtree_end(&s->meths); ) {
            m = nai_containof(e2, npc_appcb_meth_t, ent);
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


void* npc_appcb_get_ud(npc_appcb_t* p, npc_serv_t serv, npc_inst_t inst)
{
    int ec;
    void* r;
    npc_appcb_serv_t* s;


    nai_spin_lock(&p->lock);


    s = npc_appcb_get_service(p, serv, inst, 1);
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
    r = 0;
    goto _end;
};


int npc_appcb_set_ud(npc_appcb_t* p, 
    npc_serv_t serv, npc_inst_t inst, void* ud)
{
    int r;
    int ec;
    npc_appcb_serv_t* s;


    nai_spin_lock(&p->lock);


    s = npc_appcb_get_service(p, serv, inst, 0);
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
    r = -1;
    goto _end;
};


int npc_appcb_set_available(npc_appcb_t* p, 
    npc_serv_t serv, npc_inst_t inst, npc_app_available_f cb)
{
    int r;
    int ec;
    npc_appcb_serv_t* s;


    nai_spin_lock(&p->lock);


    s = npc_appcb_get_service(p, serv, inst, cb == 0);
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
    r = -1;
    goto _end;
};


int npc_appcb_set_subscribe(npc_appcb_t* p, 
    npc_serv_t serv, npc_inst_t inst, npc_app_subscribe_f cb)
{
    int r;
    int ec;
    npc_appcb_serv_t* s;


    nai_spin_lock(&p->lock);


    s = npc_appcb_get_service(p, serv, inst, cb == 0);
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
    r = -1;
    goto _end;
};


int npc_appcb_set_subscribe_ack(npc_appcb_t* p, 
    npc_serv_t serv, npc_inst_t inst, npc_app_subscribe_ack_f cb)
{
    int r;
    int ec;
    npc_appcb_serv_t* s;


    nai_spin_lock(&p->lock);


    s = npc_appcb_get_service(p, serv, inst, cb == 0);
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
    r = -1;
    goto _end;
};


int npc_appcb_set_sent(npc_appcb_t* p, 
    npc_serv_t serv, npc_inst_t inst, npc_method_t meth, 
    npc_app_sent_f cb, void* ud)
{
    int r;
    int ec;
    npc_appcb_serv_t* s;
    npc_appcb_meth_t* m;


    nai_spin_lock(&p->lock);


    s = npc_appcb_get_service(p, serv, inst, cb == 0);
    if (s == 0) {
        goto _fail;
    };

    if (meth == NPC_METHOD_ANY) {
        s->sent = cb;
    } else {
        m = npc_appcb_get_method(s, meth, cb == 0);
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
    r = -1;
    goto _end;
};


int npc_appcb_set_message(npc_appcb_t* p, 
    npc_serv_t serv, npc_inst_t inst, npc_method_t meth, 
    npc_app_message_f cb, void* ud)
{
    int r;
    int ec;
    npc_appcb_serv_t* s;
    npc_appcb_meth_t* m;


    nai_spin_lock(&p->lock);


    s = npc_appcb_get_service(p, serv, inst, cb == 0);
    if (s == 0) {
        r = -1;
        goto _fail;
    };

    if (meth == NPC_METHOD_ANY) {
        s->msg = cb;
    } else {
        m = npc_appcb_get_method(s, meth, cb == 0);
        if (m == 0) {
            goto _fail;
        };

        m->fmsg = cb;
        m->umsg = ud;
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
    r = -1;
    goto _end;
};


int npc_appcb_is_available(npc_appcb_t* p, npc_serv_t serv, npc_inst_t inst)
{
    int r;
    npc_appcb_serv_t* s;


    nai_spin_lock(&p->lock);


    s = npc_appcb_get_service(p, serv, inst, 1);
    if (s) {
        r = s->available;
    } else {
        r = 0;
    };

    nai_spin_unlock(&p->lock);


    return r;
};



int npc_appcb_get_availables(npc_appcb_t* p, 
    npc_serv_t serv, npc_inst_t inst, nai_array_t* out)
{
    int r;
    int ec;
    nai_rbnode_t* n;
    npc_appcb_serv_t* s;
    npc_service_info_t* si;


    r = 0;
    nai_spin_lock(&p->lock);


    n = npc_appcb_lbound_service(p, serv, 0);
    for ( ; n != nai_rbtree_end(&p->servs); ) {
        s = (npc_appcb_serv_t*)n;
        n = nai_rbtree_next(n);

        if (s->serv != serv) {
            break;
        };
        if (s->available == NPC_AVAIL_OFFLINED) {
            continue;
        };
        if (inst != NPC_INSTANCE_ANY && inst != s->inst) {
            continue;
        };

        si = (npc_service_info_t*)nai_array_push(out);
        if (si == 0) {
            ec = nai_errno;
            nai_log_error(NPC_LOG_CORE, ec, "failed to push service info");
            r = -1;
            goto _end;
        };

        si->serv = serv;
        si->inst = s->inst;
        si->major = s->major;
        si->minor = s->minor;
        r ++;
    };

_end:
    nai_spin_unlock(&p->lock);

    if (r < 0) {
        nai_errno = ec;
    };
    return r;
};


