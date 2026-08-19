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
/// @file       icc_client.c
/// @brief
/// @details
/// @date       2025-11-27
/// @author     Zan Shigang
/// @version    1.2.0
///
/// ================================================================

#include "icc_client.h"

#include "icc/core/icc_app.h"
#include "icc_log.h"
#include "icc_routing_impl.h"
#include "icc_service.h"
#include "nai/runtime/nai_errno.h"
#include "nai/runtime/nai_util.h"

static nai_rbnode_t** icc_client_find_impl(icc_routing_t* p, icc_cid_t cid, nai_rbnode_t** pparent)
{
    nai_rbnode_t** n     = &nai_rbtree_root(&p->clients);
    nai_rbnode_t* parent = nai_rbtree_end(&p->clients);
    icc_client_t* e;

    while (*n) {
        parent = *n;
        e      = nai_containof(parent, icc_client_t, entr);
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

icc_client_t* icc_client_find_in_routing(icc_routing_t* p, icc_cid_t cid)
{
    nai_rbnode_t** n;
    nai_rbnode_t* parent;
    icc_client_t* r;

    n = icc_client_find_impl(p, cid, &parent);
    if (n[0] == 0) {
        nai_errno = ENOENT;
        r         = 0;
        goto _end;
    };

    r = nai_containof(n[0], icc_client_t, entr);

_end:
    return r;
};

icc_client_t* icc_client_create(
    icc_routing_t* p, icc_cid_t cid, icc_endpoint_t* ep, icc_connid_t conn, const icc_cred_t* cred)
{
    icc_client_t* c;
    nai_rbnode_t** n1;
    nai_rbnode_t* parent1;

    (void)ep;
    (void)conn;
    (void)cred;

    n1 = icc_client_find_impl(p, cid, &parent1);
    if (n1[0] != 0) {
        nai_log_warn(ICC_LOG_CORE, nai_errno, "cannot create exist client(0x%x) ", cid);

        nai_errno = EEXIST;
        c         = 0;
        goto _end;
    };

    c = (icc_client_t*)icc_routing_alloc(p, sizeof(*c));
    if (c == 0) {
        nai_log_alert(ICC_LOG_CORE, nai_errno, "failed to allocate memory of client(0x%x)", cid);
        goto _end;
    };

    c->rt  = p;
    c->cid = cid;
    c->app = 0;

    /* attach routing */
    nai_rbtree_link(&p->clients, &c->entr, parent1, n1);
    nai_rbtree_color(&p->clients, &c->entr);

_end:
    return c;
};

int icc_client_close(icc_client_t* c)
{
    int r;

    /* detach from routing */
    nai_rbtree_erase(&c->rt->clients, &c->entr);

    /* free client */
    icc_routing_free(c->rt, c);
    r = 0;

    return r;
};

int icc_client_send(icc_client_t* c, icc_message_t* m, uint32_t uid, int flags)
{
    int r;
    icc_app_t* a;
    icc_routing_t* rt;

    (void)uid;
    (void)flags;

    rt = c->rt;
    if (rt->stat != ICC_RSTAT_OPENED) {
        goto _skip;
    };

    a = c->app;
    if (a != 0) {
        if (a->msgcb != 0) {
            /* app message callback */
            a->msgcb(a, a->ud, m);
            r = 0;
            goto _end;
        };
    };

_skip:

    icc_message_close(m);
    r = 0;

_end:
    return r;
};

int icc_client_handle_sent(icc_client_t* c, icc_message_t* m, int errcode)
{
    int r;
    icc_app_t* a;
    icc_routing_t* rt;

    rt = c->rt;
    if (rt->stat != ICC_RSTAT_OPENED) {
        r = 0;
        goto _end;
    };

    a = c->app;
    if (a == 0 || a->sentcb == 0) {
        r = 0;
        goto _end;
    };

    a->sentcb(a, a->ud, m, errcode);
    r = 0;

_end:
    return r;
};

int icc_client_handle_available(
    icc_client_t* c, int avail, icc_serv_t serv, icc_inst_t inst, icc_version_t major, icc_versmin_t minor, int nopost)
{
    int r;
    icc_app_t* a;
    icc_routing_t* rt;

    (void)nopost;

    nai_log_info(ICC_LOG_CORE, 0, "service(%d, %d) notify client(0x%x) available(%d)", serv, inst, c->cid, avail);

    rt = c->rt;
    if (rt->stat != ICC_RSTAT_OPENED) {
        r = 0;
        goto _end;
    };

    a = c->app;
    if (a != 0) {
        if (a->availcb == 0) {
            r = 0;
            goto _end;
        };

        a->availcb(a, a->ud, avail, serv, inst, major, minor, 0);
        r = 0;
    };

_end:
    return r;
};

int icc_client_handle_subscribe_ack(icc_client_t* c,
                                    int sult,
                                    icc_serv_t serv,
                                    icc_inst_t inst,
                                    icc_gid_t gid,
                                    icc_version_t major,
                                    icc_eid_t eid,
                                    uint32_t ttl,
                                    int init,
                                    int nopost)
{
    int r;
    icc_app_t* a;
    icc_routing_t* rt;

    (void)major;
    (void)ttl;
    (void)init;
    (void)nopost;

    assert(sult == 0 || ttl == 0);

    rt = c->rt;
    if (rt->stat != ICC_RSTAT_OPENED) {
        r = 0;
        goto _end;
    };

    a = c->app;
    if (a != 0) {
        if (a->ackcb == 0) {
            r = 0;
            goto _end;
        };

        nai_log_info(ICC_LOG_CORE, 0, "icc handle subscribe ack serv:inst:gid:eid (%d:%d:%d:%d)", serv, inst, gid, eid);

        a->ackcb(a, a->ud, sult, serv, inst, gid, eid);
        r = 0;
    };

_end:
    return r;
};
