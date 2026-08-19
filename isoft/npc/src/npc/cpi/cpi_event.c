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
/// @file       cpi_event.c
/// @brief
/// @details
/// @date       2022-11-11
/// @author     Zan Shigang
/// @version    1.2.0
///
/// ================================================================

#include "npc/cpi/cpi_mm.h"
#include "npc/cpi/cpi_error.h"
#include "npc/cpi/cpi_event.h"

struct cpi_event_grp_remove_s {
    cpi_rbtree_t *t;
    cpi_event_t  *ev;
};

struct cpi_sub_remove_s {
    cpi_rbtree_t *t;
    cpi_event_t *ev;
};

struct cpi_sub_collect_s {
    cpi_subscription_iterate_handler  handler;
    cpi_message_t                    *m;
    void                             *udata;
};

static int cpi_event_group_remove_event_scan(cpi_rbtree_node_t *node, void *rn_data, void *udata);
static int cpi_subscription_remove_event_scan(cpi_rbtree_node_t *node, void *rn_data, void *udata);
static int cpi_subscription_collect_peers_scan(cpi_rbtree_node_t *node, void *rn_data, void *udata);

/*
 * event group
 */
cpi_event_group_t *cpi_event_group_new(cpi_u16_t id)
{
    struct cpi_rbtree_attr rbattr;
    cpi_event_group_t *eg = (cpi_event_group_t *)cpi_malloc(sizeof(cpi_event_group_t));
    if (eg == NULL)
        goto out;

    eg->id = id;

    rbattr.pool = NULL;
    rbattr.pool_alloc = NULL;
    rbattr.pool_free = NULL;
    rbattr.cmp = (rbtree_cmp)cpi_event_cmp;
    rbattr.data_free = NULL;
    rbattr.cache = 0;
    eg->events = cpi_rbtree_init(&rbattr);
    if (eg->events == NULL) {
        cpi_free(eg);
        eg = NULL;
        goto out;
    }

out:
    return eg;
}

void cpi_event_group_free(cpi_event_group_t *eg)
{
    if (eg != NULL) {
        if (eg->events != NULL)
            cpi_rbtree_destroy(eg->events);
        cpi_free(eg);
    }
}

int cpi_event_group_cmp(const cpi_event_group_t *eg1, const cpi_event_group_t *eg2)
{
    int r = 0;
    if (eg1->id > eg2->id)
        r = 1;
    else if (eg1->id < eg2->id)
        r = -1;
    return r;
}

int cpi_event_group_add_event(cpi_rbtree_t *grps, cpi_rbtree_t *evs, cpi_s32_t gid, cpi_s32_t eid)
{
    int r = RET(CPI_ESUCC);
    cpi_rbtree_node_t *rn, *rn2;
    cpi_event_group_t g, *grp;
    cpi_event_t e, *ev, *tmp;

    g.id = gid;
    rn = cpi_rbtree_search(grps, &g);
    if (cpi_rbtree_null(rn)) {
        r = RET(CPI_EGNEX);
        goto out;
    }
    grp = (cpi_event_group_t *)(rn->data);

    e.id = eid;
    rn = cpi_rbtree_search(evs, &e);
    if (cpi_rbtree_null(rn)) {
        r = RET(CPI_EENEX);
        goto out;
    }
    ev = (cpi_event_t *)(rn->data);

    if ((tmp = cpi_event_group_event_search(grp, eid)) != NULL) {
        goto out;
    }

    if ((rn = cpi_rbtree_node_new(ev->groups, grp)) == NULL) {
        r = RET(CPI_ENMEM);
        goto out;
    }

    if ((rn2 = cpi_rbtree_node_new(grp->events, ev)) == NULL) {
        cpi_rbtree_node_free(rn);
        r = RET(CPI_ENMEM);
        goto out;
    }

    cpi_rbtree_insert(rn);
    cpi_rbtree_insert(rn2);

out:
    return r;
}

cpi_event_t *cpi_event_group_event_search(cpi_event_group_t *group, cpi_u16_t event_id)
{
    cpi_event_t e, *r;
    cpi_rbtree_node_t *rn;

    e.id = event_id;
    rn = cpi_rbtree_search(group->events, &e);
    if (cpi_rbtree_null(rn)) {
        r = NULL;
    } else {
        r = (cpi_event_t *)(rn->data);
    }
    return r;
}

int cpi_event_group_add(cpi_rbtree_t *t, cpi_event_group_t *grp)
{
    int r = RET(CPI_ESUCC);
    cpi_rbtree_node_t *rn;

    rn = cpi_rbtree_search(t, grp);
    if (!cpi_rbtree_null(rn)) {
        r = RET(CPI_EGEXI);
        goto out;
    }

    if ((rn = cpi_rbtree_node_new(t, grp)) == NULL) {
        r = RET(CPI_ENMEM);
        goto out;
    }
    cpi_rbtree_insert(rn);

out:
    return r;
}

void cpi_event_group_remove_event(cpi_rbtree_t *grps, cpi_event_t *ev)
{
    struct cpi_event_grp_remove_s egr;

    egr.t = grps;
    egr.ev = ev;
    cpi_rbtree_scan_all(grps, cpi_event_group_remove_event_scan, &egr);
}

static int cpi_event_group_remove_event_scan(cpi_rbtree_node_t *node, void *rn_data, void *udata)
{
    struct cpi_event_grp_remove_s *egr = (struct cpi_event_grp_remove_s *)udata;
    cpi_rbtree_node_t *rn;
    cpi_event_group_t *grp = (cpi_event_group_t *)rn_data;

    rn = cpi_rbtree_search(grp->events, egr->ev);
    if (!cpi_rbtree_null(rn)) {
        cpi_rbtree_delete(rn);
        cpi_rbtree_node_free(rn);
    }
    if (!cpi_rbtree_get_nr_node(grp->events)) {
        cpi_rbtree_delete(node);
        cpi_rbtree_node_free(node);
    }

    return RET(CPI_ESUCC);
}

/*
 * event
 */
cpi_event_t *cpi_event_new(cpi_u16_t id, cpi_event_type_t type)
{
    struct cpi_rbtree_attr rbattr;
    cpi_event_t *e = (cpi_event_t *)cpi_malloc(sizeof(cpi_event_t));
    if (e == NULL)
        goto out;

    e->id = id;
    e->type = type;

    rbattr.pool = NULL;
    rbattr.pool_alloc = NULL;
    rbattr.pool_free = NULL;
    rbattr.cache = 0;
    rbattr.data_free = NULL;
    rbattr.cmp = (rbtree_cmp)cpi_event_group_cmp;
    e->groups = cpi_rbtree_init(&rbattr);
    if (e->groups == NULL) {
        cpi_free(e);
        e = NULL;
        goto out;
    }

    rbattr.cmp = (rbtree_cmp)cpi_subscription_cmp;
    e->subscribers = cpi_rbtree_init(&rbattr);
    if (e->subscribers == NULL) {
        cpi_rbtree_destroy(e->groups);
        cpi_free(e);
        e = NULL;
        goto out;
    }

out:
    return e;
}

void cpi_event_free(cpi_event_t *e)
{
    if (e != NULL) {
        if (e->groups != NULL)
            cpi_rbtree_destroy(e->groups);
        if (e->subscribers != NULL)
            cpi_rbtree_destroy(e->subscribers);
        cpi_free(e);
    }
}

int cpi_event_cmp(const cpi_event_t *e1, const cpi_event_t *e2)
{
    int r = 0;
    if (e1->id > e2->id)
        r = 1;
    else if (e1->id < e2->id)
        r = -1;
    return r;
}

cpi_event_t *cpi_event_search(cpi_rbtree_t *t, cpi_u16_t eid)
{
    cpi_event_t e, *ev;
    cpi_rbtree_node_t *rn;

    e.id = eid;
    rn = cpi_rbtree_search(t, &e);
    if (cpi_rbtree_null(rn)) {
        ev = NULL;
    } else {
        ev = (cpi_event_t *)(rn->data);
    }

    return ev;
}

int cpi_event_add(cpi_rbtree_t *t, cpi_event_t *e)
{
    int r = RET(CPI_ESUCC);
    cpi_rbtree_node_t *rn;

    rn = cpi_rbtree_search(t, e);
    if (!cpi_rbtree_null(rn)) {
        r = RET(CPI_ESEXI);
        goto out;
    }

    if ((rn = cpi_rbtree_node_new(t, e)) == NULL) {
        r = RET(CPI_ENMEM);
        goto out;
    }
    cpi_rbtree_insert(rn);

out:
    return r;
}

void cpi_event_remove(cpi_rbtree_t *t, cpi_u16_t eid)
{
    cpi_rbtree_node_t *rn;
    cpi_event_t e;

    e.id = eid;
    rn = cpi_rbtree_search(t, &e);
    if (cpi_rbtree_null(rn)) {
        return;
    }

    cpi_rbtree_delete(rn);
    cpi_rbtree_node_free(rn);
}

/*
 * subscription
 */
cpi_subscription_t *cpi_subscription_new(cpi_u32_t appid, cpi_u16_t gid, cpi_u16_t eid)
{
    cpi_subscription_t *s = (cpi_subscription_t *)cpi_malloc(sizeof(cpi_subscription_t));
    if (s == NULL)
        goto out;

    s->appid = appid;
    s->gid = gid;
    s->eid = eid;

out:
    return s;
}

void cpi_subscription_free(cpi_subscription_t *s)
{
    if (s != NULL) {
        cpi_free(s);
    }
}

int cpi_subscription_cmp(const cpi_subscription_t *s1, const cpi_subscription_t *s2)
{
    int r = 0;
    if (s1->gid > s2->gid)
        r = 1;
    else if (s1->gid < s2->gid)
        r = -1;
    else if (s1->eid > s2->eid)
        r = 1;
    else if (s1->eid < s2->eid)
        return -1;
    else if (s1->appid > s2->appid)
        r = 1;
    else if (s1->appid < s2->appid)
        r = -1;
    return r;
}

void cpi_subscription_remove_event(cpi_rbtree_t *subscribers, cpi_rbtree_t *subscriptions, cpi_event_t *ev)
{
    struct cpi_sub_remove_s sr;

    sr.t = subscriptions;
    sr.ev = ev;
    cpi_rbtree_scan_all(subscribers, cpi_subscription_remove_event_scan, &sr);
}

static int cpi_subscription_remove_event_scan(cpi_rbtree_node_t *node, void *rn_data, void *udata)
{
    cpi_rbtree_node_t *rn;
    struct cpi_sub_remove_s *sr = (struct cpi_sub_remove_s *)udata;
    cpi_subscription_t *sub = (cpi_subscription_t *)rn_data;

    (void)node;

    rn = cpi_rbtree_search(sr->t, sub);
    if (!cpi_rbtree_null(rn)) {
        cpi_rbtree_delete(rn);
        cpi_rbtree_node_free(rn);
    }

    return RET(CPI_ESUCC);
}

cpi_subscription_t *cpi_subscription_search(cpi_rbtree_t *t, int appid, cpi_u16_t gid, cpi_u16_t eid)
{
    cpi_subscription_t *sub, s;
    cpi_rbtree_node_t *rn;

    cpi_subscription_setid(&s, appid, gid, eid);
    rn = cpi_rbtree_search(t, &s);
    if (cpi_rbtree_null(rn)) {
        sub = NULL;
    } else {
        sub = (cpi_subscription_t *)(rn->data);
    }

    return sub;
}

int cpi_subscription_add(cpi_rbtree_t *subscriptions, cpi_rbtree_t *subscribers, cpi_subscription_t *sub)
{
    int r = RET(CPI_ESUCC);
    cpi_rbtree_node_t *rn, *rn2;

    rn = cpi_rbtree_node_new(subscribers, sub);
    if (rn == NULL) {
        r = RET(CPI_ENMEM);
        goto out;
    }
    rn2 = cpi_rbtree_node_new(subscriptions, sub);
    if (rn2 == NULL) {
        cpi_rbtree_node_free(rn);
        r = RET(CPI_ENMEM);
        goto out;
    }

    cpi_rbtree_insert(rn);
    cpi_rbtree_insert(rn2);

out:
    return r;
}

void cpi_subscription_remove(cpi_rbtree_t *subscriptions, cpi_rbtree_t *subscribers, cpi_subscription_t *sub)
{
    cpi_rbtree_node_t *rn;

    rn = cpi_rbtree_search(subscribers, sub);
    if (!cpi_rbtree_null(rn)) {
        cpi_rbtree_delete(rn);
        cpi_rbtree_node_free(rn);
    }

    rn = cpi_rbtree_search(subscriptions, sub);
    if (!cpi_rbtree_null(rn)) {
        cpi_rbtree_delete(rn);
        cpi_rbtree_node_free(rn);
    }
}

int cpi_subscription_collect_peers(cpi_rbtree_t *subs, cpi_subscription_iterate_handler handler, cpi_message_t *m, void *udata)
{
    struct cpi_sub_collect_s sc;

    sc.handler = handler;
    sc.m = m;
    sc.udata = udata;
    return cpi_rbtree_scan_all(subs, cpi_subscription_collect_peers_scan, &sc);
}

static int cpi_subscription_collect_peers_scan(cpi_rbtree_node_t *node, void *rn_data, void *udata)
{
    int r = RET(CPI_ESUCC);
    struct cpi_sub_collect_s *sc = (struct cpi_sub_collect_s *)udata;
    cpi_subscription_t *sub = (cpi_subscription_t *)rn_data;

    (void)node;

    if (sub->eid == sc->m->hdr.method && sc->handler != NULL) {
        r = sc->handler(sc->m, sub->appid, sc->udata);
    }

    return r;
}

