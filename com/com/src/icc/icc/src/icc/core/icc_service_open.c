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
/// @file       icc_service_open.c
/// @brief
/// @details
/// @date       2026-02-13
/// @author     jiusen.cui
/// @version    1.2.0
///
/// ================================================================

#include "icc/net/icc_message.h"
#include "icc_client.h"
#include "icc_log.h"
#include "icc_routing_impl.h"
#include "icc_service.h"
#include "nai/runtime/nai_errno.h"
#include "nai/runtime/nai_util.h"

static nai_rbnode_t** icc_service_find_impl(icc_routing_t* p, icc_serv_t serv, icc_inst_t inst, nai_rbnode_t** pparent)
{
    nai_rbnode_t** n     = &nai_rbtree_root(&p->servs);
    nai_rbnode_t* parent = nai_rbtree_end(&p->servs);
    icc_service_t* e;
    icc_servkey_t v;

    v.serv = serv;
    v.inst = inst;

    while (*n) {
        parent = *n;
        e      = (icc_service_t*)parent;
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

static int icc_service_init(icc_service_t* s, icc_routing_t* p, icc_serv_t serv, icc_inst_t inst)
{
    int r;

    s->rt     = p;
    s->serv   = serv;
    s->inst   = inst;
    s->ep     = 0;
    s->client = 0;

    r = 0;
    return r;
};

static int icc_service_open_impl(icc_service_t* s)
{
    int r;
    icc_service_info_t si;

    if (icc_service_is_opened(s)) {
        r = 0;
        goto _end;
    };

    /* query service info */
    r = icc_conf_get_service(s->rt->conf, s->serv, s->inst, &si);
    if (r < 0) {
        nai_log_error(ICC_LOG_CORE, nai_errno, "open service(%d, %d) failed", s->serv, s->inst);
        goto _end;
    };
    s->ep = icc_network_create_endpoint(s->rt->net, si.icc_type, &si.icc_identifier, s->serv, s->inst);
    if (!s->ep) {
        nai_log_error(ICC_LOG_CORE, nai_errno, "open service(%d, %d) failed", s->serv, s->inst);
        r = -1;
        goto _end;
    };

    s->stat = ICC_SERVICE_OPENED;

_end:
    return r;
};

icc_service_t* icc_service_find_in_routing(icc_routing_t* p, icc_serv_t serv, icc_inst_t inst)
{
    nai_rbnode_t** n;
    icc_service_t* r;

    n = icc_service_find_impl(p, serv, inst, 0);
    if (n[0] == 0) {
        nai_errno = ENOENT;
        r         = 0;
        goto _end;
    };

    r = nai_containof(n[0], icc_service_t, entr);

_end:
    return r;
};

icc_service_t* icc_service_open(icc_routing_t* p, icc_serv_t serv, icc_inst_t inst, int na)
{
    int r;
    int ec;
    nai_rbnode_t** n;
    nai_rbnode_t* parent;
    icc_service_t* s;

    n = icc_service_find_impl(p, serv, inst, &parent);
    s = (icc_service_t*)n[0];
    if (s != 0) {
        goto _end;
    };

    if (na) {
        nai_errno = ENOENT;
        goto _end;
    };

    s = (icc_service_t*)icc_routing_alloc(p, sizeof(*s));
    if (s == 0) {
        nai_log_alert(ICC_LOG_CORE, nai_errno, "allocate a service failed");
        goto _end;
    };

    icc_service_init(s, p, serv, inst);
    r = icc_service_open_impl(s);
    if (r < 0) {
        ec = nai_errno;
        nai_log_error(ICC_LOG_CORE, ec, "open service failed");
        icc_routing_free(p, s);
        nai_errno = ec;
        s         = 0;
        goto _end;
    };

    nai_rbtree_link(&p->servs, &s->entr, parent, n);
    nai_rbtree_color(&p->servs, &s->entr);

_end:
    return s;
};

int icc_service_close(icc_service_t* s)
{
    int r;
    icc_routing_t* p;

    p = s->rt;

    /* do close */
    switch (s->stat) {
        case ICC_SERVICE_CLOSED:
            r = 0;
            break;
        default:
            r = 0;
            break;
    };

    nai_rbtree_erase(&p->servs, &s->entr);
    icc_routing_free(p, s);
    r = 0;

    return r;
};
