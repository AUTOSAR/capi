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
/// @file       cpi_service.c
/// @brief
/// @details
/// @date       2022-11-11
/// @author     Zan Shigang
/// @version    1.2.0
///
/// ================================================================

#include "nai/runtime/nai_errno.h"
#include "nai/runtime/nai_log.h"
#include "nai/os/nai_shmem.h"
#include "npc/cpi/cpi_mm.h"
#include "npc/cpi/cpi_service.h"
#include "npc/cpi/cpi_event.h"
#include "npc/cpi/cpi_error.h"
#include "npc/cpi/cpi_const.h"
#include "npc/cpi/cpi_frame.h"


struct cpi_sub_auto_s {
    cpi_service_t      *svc;
    cpi_endpoint_t     *ep;
};

struct cpi_notification_collect_s {
    cpi_service_t      *svc;
    cpi_service_peer_t *head;
    cpi_service_peer_t *tail;
};

static int cpi_service_timer_cb(nai_evnode_t *node, int events);
static int cpi_service_group_add(cpi_service_t *svc, cpi_event_info_t *info);
static int __cpi_service_event_add(cpi_service_t *svc, cpi_u16_t eid, cpi_event_info_t *info);
static int cpi_service_evgrp_map(cpi_service_t *svc, cpi_u16_t eid, cpi_event_info_t *info);
static int cpi_service_auto_subscribe_scan(cpi_rbtree_node_t *rn, void *rn_data, void *udata);
static int cpi_service_shm_post(void *ctx, cpi_u16_t peer, cpi_u32_t event);
static int cpi_service_shm_test(void *ctx, cpi_u16_t eid, cpi_u16_t uid);
static int cpi_service_shm_read(void *ctx, npc_port_t *p);
static int cpi_service_shm_quit(void *ctx, npc_port_t *p);
static cpi_service_peer_t *cpi_service_peer_new(cpi_service_t *svc, int appid);
static void cpi_service_peer_free(cpi_service_peer_t *sp);
static int cpi_service_peer_cmp(const cpi_service_peer_t *sp1, const cpi_service_peer_t *sp2);
static int cpi_service_notification_send(cpi_service_t *svc, cpi_message_t *m);
CPI_CHAIN_FUNC_DECLARE(cpi_service_peer, cpi_service_peer_t, static inline void,);
CPI_CHAIN_FUNC_DEFINE(cpi_service_peer, cpi_service_peer_t, static inline void, prev, next);
static int cpi_service_notification_peer_collect(cpi_message_t *m, int appid, void *data);
static int cpi_service_notification_send_check(cpi_service_t *svc, cpi_message_t *m);
static int cpi_service_notification_peer_check(cpi_message_t *m, int appid, void *data);
static int cpi_service_reregister_service_handler(cpi_rbtree_node_t *rn, void *rn_data, void *udata);
static int cpi_service_reoffer_service(cpi_service_t *svc);
static int cpi_service_refind_service(cpi_service_t *svc);

static npc_comif_ops_t comif_ops = {
    "service",
    cpi_service_shm_post,
    cpi_service_shm_test,
    cpi_service_shm_read,
    cpi_service_shm_quit,
};

cpi_service_t *cpi_service_new(void *app,
                               nai_evloop_t *evloop,
                               int appid,
                               cpi_u16_t instance_id,
                               cpi_u16_t service_id,
                               cpi_u32_t major,
                               cpi_u32_t minor,
                               cpi_service_role_t role,
                               const char *shm_path,
                               cpi_u32_t seg_size,
                               cpi_u32_t seg_count,
                               cpi_u16_t msg_queue_len)
{
    cpi_service_t *s;
    npc_shmz_attr_t shm_attr;
    struct cpi_rbtree_attr rbattr;

    if ((s = (cpi_service_t *)cpi_malloc(sizeof(cpi_service_t))) == NULL) {
        goto out;
    }

    s->app = app;
    s->appid = appid;
    memset(&s->svc_addr, 0, sizeof(s->svc_addr));
    s->instance_id = instance_id;
    s->service_id = service_id;
    s->major = major;
    s->minor = minor;
    s->role = role;
    s->status = svc_offline;
    s->groups = s->events = s->subscriptions = s->peers = NULL;
    s->seg_size = seg_size;
    s->seg_count = seg_count;
    memcpy(s->shm_path, shm_path, CPI_SHM_PATH_LEN);
    s->shm_comif = NULL;
    s->msg_queue_len = msg_queue_len;
    s->read = s->write = NULL;
    s->read_addr = s->write_addr = 0;
    s->quit = 0;

    nai_evnode_init(&s->node);
    nai_evnode_set_cb(&s->node, cpi_service_timer_cb);
    if (nai_evnode_open(&s->node, evloop) < 0) {
        cpi_free(s);
        s = NULL;
        goto out;
    }

    rbattr.pool = NULL;
    rbattr.pool_alloc = NULL;
    rbattr.pool_free = NULL;
    rbattr.cache = 0;

    rbattr.cmp = (rbtree_cmp)cpi_event_group_cmp;
    rbattr.data_free = (rbtree_free_data)cpi_event_group_free;
    if ((s->groups = cpi_rbtree_init(&rbattr)) == NULL) {
        cpi_service_free(s);
        s = NULL;
        goto out;
    }

    rbattr.cmp = (rbtree_cmp)cpi_event_cmp;
    rbattr.data_free = (rbtree_free_data)cpi_event_free;
    if ((s->events = cpi_rbtree_init(&rbattr)) == NULL) {
        cpi_service_free(s);
        s = NULL;
        goto out;
    }

    rbattr.cmp = (rbtree_cmp)cpi_subscription_cmp;
    rbattr.data_free = (rbtree_free_data)cpi_subscription_free;
    if ((s->subscriptions = cpi_rbtree_init(&rbattr)) == NULL) {
        cpi_service_free(s);
        s = NULL;
        goto out;
    }

    rbattr.cmp = (rbtree_cmp)cpi_service_peer_cmp;
    rbattr.data_free = (rbtree_free_data)cpi_service_peer_free;
    if ((s->peers = cpi_rbtree_init(&rbattr)) == NULL) {
        cpi_service_free(s);
        s = NULL;
        goto out;
    }

    shm_attr.serv = service_id;
    shm_attr.inst = instance_id;
    shm_attr.self = cpi_app_get_id((cpi_app_t *)app);
    shm_attr.seg_size = seg_size;
    shm_attr.seg_count = seg_count;
    if (role == svc_provider) {
        nai_shm_unlink(shm_path);
        s->shm_comif = npc_comif_create(cpi_app_get_shm_comm((cpi_app_t *)app),
                                        &comif_ops,
                                        s,
                                        shm_path,
                                        &shm_attr,
                                        NAI_O_CREAT|NAI_O_RDWR,
                                        0644);
    } else if (role == svc_consumer) {
        s->shm_comif = npc_comif_create(cpi_app_get_shm_comm((cpi_app_t *)app),
                                        &comif_ops,
                                        s,
                                        shm_path,
                                        &shm_attr,
                                        NAI_O_RDWR);
    }
    if (role != svc_holder && s->shm_comif == NULL) {
        cpi_service_free(s);
        s = NULL;
        goto out;
    }

out:
    return s;
}

int cpi_service_consumer_comif_create(cpi_service_t *svc)
{
    int r = RET(CPI_ESUCC);
    npc_shmz_attr_t shm_attr;
    cpi_app_t *app = cpi_service_get_app(svc);

    if (svc->shm_comif != NULL)
        goto out;

    shm_attr.serv = cpi_service_get_serviceid(svc);
    shm_attr.inst = cpi_service_get_instanceid(svc);
    shm_attr.self = cpi_app_get_id(app);
    shm_attr.seg_size = cpi_service_get_seg_size(svc);
    shm_attr.seg_count = cpi_service_get_seg_count(svc);
    svc->shm_comif = npc_comif_create(cpi_app_get_shm_comm(cpi_service_get_app(svc)),
                                      &comif_ops,
                                      svc,
                                      cpi_service_get_shm_path(svc),
                                      &shm_attr,
                                      NAI_O_RDWR);
    if (svc->shm_comif == NULL) {
        r = RET(CPI_ECMIF);
        goto out;
    }

out:
    return r;
}

void cpi_service_free(cpi_service_t *s)
{
    log("---LOG---\n");
    if (s != NULL && !s->quit) {
        s->quit = 1;
        nai_evnode_close(&s->node);
        if (s->groups != NULL)
            cpi_rbtree_destroy(s->groups);
        if (s->events != NULL)
            cpi_rbtree_destroy(s->events);
        if (s->subscriptions != NULL)
            cpi_rbtree_destroy(s->subscriptions);
        if (s->peers != NULL)
            cpi_rbtree_destroy(s->peers);

        if (s->app != NULL && s->read != NULL) {
            log("---LOG--- shm_read\n");
            cpi_service_shm_read(s, s->read);
        }
        if (s->read != NULL)
            npc_port_close(s->read);
        if (s->write != NULL)
            npc_port_close(s->write);
        if (s->shm_comif != NULL)
            npc_comif_close(s->shm_comif);
        cpi_free(s);
    }
}

void cpi_service_consumer_port_free(cpi_service_t *s)
{
    if (s != NULL) {
        if (s->read != NULL)
            npc_port_close(s->read);
        if (s->write != NULL)
            npc_port_close(s->write);
        s->read = s->write = NULL;
        s->read_addr = s->write_addr = 0;
    }
}

int cpi_service_cmp(const cpi_service_t *s1, const cpi_service_t *s2)
{
    int r = 0;

    if (s1->service_id != CPI_SERVICE_ANY && s2->service_id != CPI_SERVICE_ANY) {
        if (s1->service_id > s2->service_id)
            r = 1;
        else if (s1->service_id < s2->service_id)
            r = -1;
    }
    if (s1->instance_id != CPI_INSTANCE_ANY && s2->instance_id != CPI_INSTANCE_ANY) {
        if (s1->instance_id > s2->instance_id)
            r = 1;
        else if (s1->instance_id < s2->instance_id)
            r = -1;
    }
    return r;
}

cpi_service_t *cpi_service_search(cpi_rbtree_t *services, cpi_u16_t svc, cpi_u16_t inst)
{
    cpi_rbtree_node_t *rn;
    cpi_service_t s, *ps;

    s.service_id = svc;
    s.instance_id = inst;
    rn = cpi_rbtree_search(services, &s);
    if (cpi_rbtree_null(rn)) {
        ps = NULL;
    } else {
        ps = (cpi_service_t *)(rn->data);
    }

    return ps;
}

int cpi_service_remove(cpi_rbtree_t *services, cpi_u16_t svc, cpi_u16_t inst)
{
    int r = RET(CPI_ESUCC);
    cpi_rbtree_node_t *rn;
    cpi_service_t s;

    s.service_id = svc;
    s.instance_id = inst;
    rn = cpi_rbtree_search(services, &s);
    if (cpi_rbtree_null(rn)) {
        r = RET(CPI_ENSVC);
    } else {
        cpi_rbtree_delete(rn);
        cpi_rbtree_node_free(rn);
    }

    return r;
}

static int cpi_service_timer_cb(nai_evnode_t *node, int events)
{
    cpi_service_t *svc = nai_containof(node, cpi_service_t, node);

    (void)events;

    cpi_service_online(svc);
    return cpi_app_find_svc_notice(svc->app,
                                   svc->appid,
                                   svc->service_id,
                                   svc->instance_id,
                                   svc->major,
                                   svc->minor,
                                   svc->status,
                                   &svc->svc_addr,
                                   svc->shm_path,
                                   svc->seg_size,
                                   svc->seg_count);
}

int cpi_service_event_add(cpi_service_t *svc, cpi_u16_t eid, cpi_event_info_t *info)
{
    int r = RET(CPI_ESUCC);

    r = cpi_service_group_add(svc, info);
    if (CODE(r) != CPI_ESUCC) {
        goto out;
    }

    r = __cpi_service_event_add(svc, eid, info);
    if (CODE(r) != CPI_ESUCC) {
        goto out;
    }

    r = cpi_service_evgrp_map(svc, eid, info);

out:
    return r;
}

static int cpi_service_group_add(cpi_service_t *svc, cpi_event_info_t *info)
{
    int r = RET(CPI_ESUCC);
    int i;
    cpi_s32_t gid;
    cpi_event_group_t *grp;

    for (i = 0; i < info->ngroup; ++i) {
        gid = info->groups[i];

        if ((grp = cpi_event_group_new(gid)) == NULL) {
            r = RET(CPI_ENMEM);
            goto out;
        }
        r = cpi_event_group_add(svc->groups, grp);
        if (CODE(r) != CPI_ESUCC) {
            cpi_event_group_free(grp);
            goto out;
        }
    }

out:
    return r;
}

static int __cpi_service_event_add(cpi_service_t *svc, cpi_u16_t eid, cpi_event_info_t *info)
{
    int r = RET(CPI_ESUCC);
    cpi_event_type_t type;
    cpi_event_t *e;

    type = info->type == CPI_ET_FIELD? cpi_event_field: cpi_event_ev;

    if ((e = cpi_event_new(eid, type)) == NULL) {
        r = RET(CPI_ENMEM);
        goto out;
    }
    r = cpi_event_add(svc->events, e);
    if (CODE(r) != CPI_ESUCC) {
        cpi_event_free(e);
        goto out;
    }

out:
    return r;
}

static int cpi_service_evgrp_map(cpi_service_t *svc, cpi_u16_t eid, cpi_event_info_t *info)
{
    int r = RET(CPI_ESUCC);
    cpi_s32_t i, gid;

    for (i = 0; i < info->ngroup; ++i) {
        gid = info->groups[i];

        r = cpi_event_group_add_event(svc->groups, svc->events, gid, eid);
        if (CODE(r) != CPI_ESUCC)
            goto out;
    }

out:
    return r;
}


int cpi_service_event_remove(cpi_service_t *svc, cpi_u16_t eid)
{
    int r = RET(CPI_ESUCC);
    cpi_event_t *ev;

    ev = cpi_event_search(svc->events, eid);
    if (ev == NULL) {
        goto out;
    }

    cpi_subscription_remove_event(ev->subscribers, svc->subscriptions, ev);
    cpi_event_group_remove_event(svc->groups, ev);
    cpi_event_remove(svc->events, eid);

out:
    return r;
}

int cpi_service_subscription_set(cpi_service_t *svc, int appid, cpi_u16_t gid, cpi_u16_t eid)
{
    int r = RET(CPI_ESUCC);
    cpi_subscription_t *sub;
    cpi_event_t *e;

    sub = cpi_subscription_search(svc->subscriptions, appid, gid, eid);
    if (sub != NULL) {
        goto out;
    }

    e = cpi_event_search(svc->events, eid);
    if (e == NULL) {
        r = RET(CPI_ENEVT);
        goto out;
    }

    sub = cpi_subscription_new(appid, gid, eid);
    if (sub == NULL) {
        r = RET(CPI_ENMEM);
        goto out;
    }
    r = cpi_subscription_add(svc->subscriptions, e->subscribers, sub);
    if (CODE(r) != CPI_ESUCC) {
        cpi_subscription_free(sub);
        goto out;
    }

out:
    return r;
}

int cpi_service_subscription_remove(cpi_service_t *svc, int appid, cpi_u16_t gid, cpi_u16_t eid)
{
    int r = RET(CPI_ESUCC);
    cpi_subscription_t *s;
    cpi_event_t *e;

    s = cpi_subscription_search(svc->subscriptions, appid, gid, eid);
    if (s == NULL) {
        r = RET(CPI_ENSUB);
        goto out;
    }

    e = cpi_event_search(svc->events, eid);
    if (e == NULL) {
        r = RET(CPI_ENEVT);
        goto out;
    }

    cpi_subscription_remove(svc->subscriptions, e->subscribers, s);

out:
    return r;
}

int cpi_service_auto_subscribe(cpi_service_t *svc, cpi_endpoint_t *ep)
{
    int r = RET(CPI_ESUCC);
    struct cpi_sub_auto_s sa;

    sa.svc = svc;
    sa.ep = ep;

    r = cpi_rbtree_scan_all(svc->subscriptions, cpi_service_auto_subscribe_scan, &sa);
    if (CODE(r) != CPI_ESUCC)
        goto out;

    r = ep->ops.send(ep);

out:
    return r;
}

static int cpi_service_auto_subscribe_scan(cpi_rbtree_node_t *rn, void *rn_data, void *udata)
{
    int r = RET(CPI_ESUCC);
    cpi_buf_t *b;
    cpi_frame_t f;
    cpi_u8ptr_t pdata;
    cpi_u32_t len;
    struct cpi_sub_auto_s *sa = (struct cpi_sub_auto_s *)udata;
    cpi_subscription_t *sub = (cpi_subscription_t *)rn_data;

    (void)rn;

    cpi_frame_init(&f);

    f.dest.inst = cpi_service_get_instanceid(sa->svc);
    f.dest.svc = cpi_service_get_serviceid(sa->svc);
    f.dest.major = cpi_service_get_major(sa->svc);
    f.dest.minor = cpi_service_get_minor(sa->svc);
    f.dest.gid = cpi_subscription_get_gid(sub);
    f.dest.eid = cpi_subscription_get_eid(sub);
    f.src.appid = cpi_app_get_id(cpi_service_get_app(sa->svc));
    f.op = cpi_frame_op_subscribe;

    b = cpi_buf_new();
    if (b == NULL) {
        r = RET(CPI_ENMEM);
        goto out;
    }

    pdata = cpi_buf_get_data(b);
    len = cpi_buf_get_avail_size(b);
    if (sizeof(f) > len) {
        r = RET(CPI_EBLEN);
        cpi_buf_free(b);
        goto out;
    }
    memcpy(pdata, &f, sizeof(f));
    cpi_buf_inc_len(b, sizeof(f));

    cpi_endpoint_buf_append(sa->ep, b, CPI_ENDPOINT_SEND);

out:
    return r;
}

static int cpi_service_shm_post(void *ctx, cpi_u16_t peer, cpi_u32_t event)
{
    log("---LOG---\n");
    int r = RET(CPI_ESUCC);
    cpi_frame_t f;
    cpi_buf_t *b;
    cpi_u8ptr_t pdata;
    cpi_u32_t len;
    cpi_service_t *s = (cpi_service_t *)ctx;
    cpi_app_t *app = cpi_service_get_app(s);
    cpi_endpoint_t *ep;

    cpi_frame_init(&f);

    if ((ep = cpi_endpoint_search_by_id(app->endpoints_id, peer)) == NULL) {
        r = RET(CPI_ENENP);
        goto out;
    }

    f.op = cpi_frame_op_send;
    f.dest.appid = peer;
    f.dest.event = event;
    f.src.appid = cpi_app_get_id(app);
    f.src.inst = s->instance_id;
    f.src.svc = s->service_id;
    f.src.major = s->major;
    f.src.minor = s->minor;

    b = cpi_buf_new();
    if (b == NULL) {
        r = RET(CPI_ENMEM);
        goto out;
    }

    pdata = cpi_buf_get_data(b);
    len = cpi_buf_get_avail_size(b);
    if (sizeof(f) > len) {
        r = RET(CPI_EBLEN);
        cpi_buf_free(b);
        goto out;
    }
    memcpy(pdata, &f, sizeof(f));
    cpi_buf_inc_len(b, sizeof(f));

    cpi_endpoint_buf_append(ep, b, CPI_ENDPOINT_SEND);
    r = ep->ops.send(ep);

out:
    return r;
}

static int cpi_service_shm_test(void *ctx, cpi_u16_t eid, cpi_u16_t uid)
{
    (void)ctx;
    (void)eid;
    (void)uid;
    return 0; /* replication will be solved before sending */
}

static int cpi_service_shm_read(void *ctx, npc_port_t *p)
{
    log("---LOG--- %p\n", p);
    int r = RET(CPI_ESUCC), rc;
    cpi_message_t *m;
    cpi_service_t *s = (cpi_service_t *)ctx;
    cpi_app_t *app = cpi_service_get_app(s);

    while (1) {
        m = npc_port_read(p);
        if (m == NULL) {
            if (nai_errno != EAGAIN) {
                r = RET(CPI_EOPPT);
            }
            goto out;
        }
        m->inst = cpi_service_get_instanceid(s);
        log("---LOG--- m:%p serv:%u inst:%u method:%u type:%u session:%u\n", m, m->hdr.serv, m->inst, m->hdr.method, m->hdr.type, m->hdr.session);
        if (!cpi_app_is_quit(app)) {
            if (cpi_app_iam_message_cb_get(app) != NULL) {
                rc = cpi_app_iam_message_cb_get(app)(app,  cpi_app_get_data(app), m);
                if (rc != 0) {
                    r = RET(CPI_EPERM);
                    goto out;
                }
            }
            if (cpi_app_get_msg_cb(app) != NULL) {
                if ((rc = cpi_app_get_msg_cb(app)(app, cpi_app_get_data(app), m)) < 0) {
                    //nai_log_notice(NAI_LOG_CORE, 0, "message callback failed. %s", cpi_error(rc, msg, sizeof(msg)));
                }
            }
        } else {
            npc_message_close(m);
        }
    }

out:
    return r;
}

static int cpi_service_shm_quit(void *ctx, npc_port_t *p)
{
    int r = RET(CPI_ESUCC), appid;
    cpi_frame_t f;
    cpi_buf_t *b;
    cpi_u8ptr_t pdata;
    cpi_u32_t len;
    cpi_service_t *s = (cpi_service_t *)ctx;
    cpi_app_t *app = cpi_service_get_app(s);
    cpi_rbtree_node_t *rn;
    cpi_endpoint_t *ep;

    cpi_frame_init(&f);

    if (p != NULL) {
        /* never be here */
        nai_log_notice(NAI_LOG_CORE, 0, "Port state won't be modified explicitly");
    } else if (s->role == svc_consumer) {
        f.dest.inst = cpi_service_get_instanceid(s);
        f.dest.svc = cpi_service_get_serviceid(s);
        f.dest.major = cpi_service_get_major(s);
        f.dest.minor = cpi_service_get_minor(s);
        f.src.appid = cpi_app_get_id(app);
        f.op = cpi_frame_op_stop_find_service;

        b = cpi_buf_new();
        if (b == NULL) {
            r = RET(CPI_ENMEM);
            goto out;
        }

        pdata = cpi_buf_get_data(b);
        len = cpi_buf_get_avail_size(b);
        if (sizeof(f) > len) {
            r = RET(CPI_EBLEN);
            cpi_buf_free(b);
            goto out;
        }
        memcpy(pdata, &f, sizeof(f));
        cpi_buf_inc_len(b, sizeof(f));

        cpi_endpoint_buf_append(app->discovery, b, CPI_ENDPOINT_SEND);
        r = app->discovery->ops.send(app->discovery);
        if (CODE(r) != CPI_ESUCC)
            goto out;

        appid = cpi_service_get_appid(s);

        if (!s->quit) {
            rn = cpi_rbtree_search(app->services, s);
            if (!cpi_rbtree_null(rn)) {
                cpi_rbtree_delete(rn);
                cpi_rbtree_node_free(rn);
            } else {
                cpi_service_free(s);
            }
        }

        ep = cpi_endpoint_search_by_id(app->endpoints_id, appid);
        if (ep != NULL) {
            if (cpi_endpoint_ref_dec(ep) <= 1) {
                cpi_endpoint_set_positive(ep);
                ep->ops.close(ep);
            }
        }
    }

out:
    return r;
}

int cpi_service_remove_and_disconnect(cpi_app_t *app, cpi_u16_t svc, cpi_u16_t inst, cpi_u32_t major, cpi_u32_t minor)
{
    int appid, r = RET(CPI_ESUCC);
    cpi_service_t s, *psvc;
    cpi_rbtree_node_t *rn;
    cpi_endpoint_t *ep;

    cpi_service_set_ids(&s, 0, inst, svc, major, minor);
    rn = cpi_rbtree_search(app->services, &s);
    if (cpi_rbtree_null(rn)) {
        r = RET(CPI_ENSVC);
        goto out;
    }
    psvc = (cpi_service_t *)(rn->data);

    if (psvc->shm_comif != NULL && npc_comif_state(psvc->shm_comif, NPC_STATE_QUIT) < 0) {
        if (nai_errno == NAI_EAGAIN) {
            r = RET(CPI_EBUSY);
        } else {
            r = RET(CPI_ECMIF);
        }
        goto out;
    }

    appid = cpi_service_get_appid(psvc);
    if (appid == app->id) {
        r = RET(CPI_ESOPR);
        goto out;
    }
    cpi_rbtree_delete(rn);
    cpi_rbtree_node_free(rn);

    ep = cpi_endpoint_search_by_id(app->endpoints_id, appid);
    if (ep != NULL) {
        if (cpi_endpoint_ref_dec(ep) <= 1) {
            cpi_endpoint_set_positive(ep);
            ep->ops.close(ep);
        }
    }

out:
    return r;
}


/*
 * service_peer
 */
static cpi_service_peer_t *cpi_service_peer_new(cpi_service_t *svc, int appid)
{
    cpi_service_peer_t *sp;

    if ((sp = (cpi_service_peer_t *)cpi_malloc(sizeof(cpi_service_peer_t))) == NULL) {
        goto out;
    }
    sp->appid = appid;
    sp->in_chain = 0;
    if ((sp->read = npc_port_create(svc->shm_comif, 1, svc->msg_queue_len, NPC_PORT_READ, appid)) == NULL) {
        cpi_free(sp);
        sp = NULL;
        goto out;
    }
    if ((sp->write = npc_port_create(svc->shm_comif, 1, svc->msg_queue_len, NPC_PORT_SEND)) == NULL) {
        npc_port_close(sp->read);
        cpi_free(sp);
        sp = NULL;
        goto out;
    }

    sp->prev = sp->next = NULL;

out:
    return sp;
}

static void cpi_service_peer_free(cpi_service_peer_t *sp)
{
    if (sp != NULL) {
        if (sp->read != NULL)
            npc_port_close(sp->read);
        if (sp->write != NULL)
            npc_port_close(sp->write);
        cpi_free(sp);
    }
}

static int cpi_service_peer_cmp(const cpi_service_peer_t *sp1, const cpi_service_peer_t *sp2)
{
    return sp1->appid - sp2->appid;
}

int cpi_service_peer_add(cpi_service_t *svc, int appid)
{
    int r = RET(CPI_ESUCC);
    cpi_rbtree_node_t *rn;
    cpi_service_peer_t *sp;

    sp = cpi_service_peer_search(svc, appid);
    if (sp == NULL) {
        sp = cpi_service_peer_new(svc, appid);
        if (sp == NULL) {
            r = RET(CPI_ENMEM);
            goto out;
        }
        rn = cpi_rbtree_node_new(svc->peers, sp);
        if (rn == NULL) {
            cpi_service_peer_free(sp);
            r = RET(CPI_ENMEM);
            goto out;
        }
        cpi_rbtree_insert(rn);

        if (npc_port_add(sp->write, appid) < 0) {
            cpi_rbtree_delete(rn);
            cpi_rbtree_node_free(rn);
            r = RET(CPI_EOPPT);
            goto out;
        }
    }

out:
    return r;
}

cpi_service_peer_t *cpi_service_peer_search(cpi_service_t *svc, int appid)
{
    cpi_rbtree_node_t *rn;
    cpi_service_peer_t sp, *psp;

    sp.appid = appid;
    rn = cpi_rbtree_search(svc->peers, &sp);
    if (cpi_rbtree_null(rn)) {
        psp = NULL;
    } else {
        psp = (cpi_service_peer_t *)(rn->data);
    }

    return psp;
}

void cpi_service_peer_remove(cpi_service_t *svc, int appid)
{
    cpi_rbtree_node_t *rn;
    cpi_service_peer_t sp, *psp;

    sp.appid = appid;
    rn = cpi_rbtree_search(svc->peers, &sp);
    if (!cpi_rbtree_null(rn)) {
        psp = (cpi_service_peer_t *)(rn->data);
        if (psp->write != NULL)
            npc_port_remove(psp->write, appid);
        cpi_rbtree_delete(rn);
        cpi_rbtree_node_free(rn);
    }
}

int cpi_service_request_port(cpi_service_t *svc, cpi_endpoint_t *ep)
{
    log("---LOG---\n");
    int r = RET(CPI_ESUCC);
    cpi_frame_t f;
    cpi_buf_t *b;
    cpi_u32_t len;
    cpi_u8ptr_t pdata;

    cpi_frame_init(&f);

    f.op = cpi_frame_op_find_request_port;
    f.src.appid = cpi_app_get_id(svc->app);
    f.dest.svc = cpi_service_get_serviceid(svc);
    f.dest.inst = cpi_service_get_instanceid(svc);
    f.dest.major = cpi_service_get_major(svc);
    f.dest.minor = cpi_service_get_minor(svc);

    if ((b = cpi_buf_new()) == NULL) {
        r = RET(CPI_ENMEM);
        goto out;
    }
    pdata = cpi_buf_get_data(b);
    len = cpi_buf_get_avail_size(b);
    if (sizeof(f) > len) {
        r = RET(CPI_EBLEN);
        cpi_buf_free(b);
        goto out;
    }
    memcpy(pdata, &f, sizeof(f));
    cpi_buf_inc_len(b, sizeof(f));
    cpi_endpoint_buf_append(ep, b, CPI_ENDPOINT_SEND);

out:
    return r;
}

int cpi_service_port_attach(cpi_service_t *svc, npc_mref_t wr, npc_mref_t rd)
{
    int r = RET(CPI_ESUCC);

    svc->read = npc_port_attach(svc->shm_comif, rd, NPC_PORT_READ);
    if (svc->read == NULL) {
        r = RET(CPI_EOPPT);
        goto out;
    }

    svc->write = npc_port_attach(svc->shm_comif, wr, NPC_PORT_SEND);
    if (svc->write == NULL) {
        npc_port_close(svc->read);
        r = RET(CPI_EOPPT);
        goto out;
    }

    svc->read_addr = rd;
    svc->write_addr = wr;

out:
    return r;
}

int cpi_service_send(cpi_service_t *svc, cpi_message_t *m)
{
    int r = RET(CPI_ESUCC);
    npc_port_t *port;
    cpi_service_peer_t *sp;
    cpi_app_t *app = cpi_service_get_app(svc);

    m->hdr.interface = cpi_service_get_major(svc);
    switch (cpi_service_get_role(svc)) {
        case svc_provider:
            if (m->hdr.type == NPC_MT_REQUEST) {
                r = RET(CPI_EPERM);
                goto out;
            }
            if (m->hdr.type == NPC_MT_NOTIFICATION) {
                r = cpi_service_notification_send(svc, m);
                goto out;
            }
            sp = cpi_service_peer_search(svc, m->hdr.client);
            if (sp == NULL) {
                r = RET(CPI_ENSCR);
                goto out;
            }
            port = cpi_service_get_port(sp, write);
            break;
        case svc_consumer:
            if (m->hdr.type != NPC_MT_REQUEST && m->hdr.type != NPC_MT_REQUEST_NO_RETURN) {
                r = RET(CPI_EPERM);
                goto out;
            }
            port = cpi_service_get_port(svc, write);
            break;
        default:
            r = RET(CPI_EPERM);
            goto out;
    }

    if (port == NULL) {
        r = RET(CPI_EPORT);
        goto out;
    }

    if (npc_port_send(port, m) < 0) {
        r = RET(CPI_EOPPT);
        goto out;
    }

out:
    if (cpi_app_get_sent_cb(app) != NULL)
        cpi_app_get_sent_cb(app)(app, cpi_app_get_data(app), m, r);
    if (CODE(r) != CPI_ESUCC)
        npc_message_close(m);

    return r;
}


static int cpi_service_notification_send(cpi_service_t *svc, cpi_message_t *m)
{
    int r = RET(CPI_ESUCC);
    cpi_service_peer_t *sp;
    struct cpi_notification_collect_s nc;
    cpi_message_t *msg;

    nc.svc = svc;
    nc.head = nc.tail = NULL;
    r = cpi_subscription_collect_peers(svc->subscriptions, cpi_service_notification_peer_collect, m, &nc);
    if (CODE(r) != CPI_ESUCC)
        goto out;
    if (nc.head == NULL) {
        r = RET(CPI_ENSCR);
    } else {
        while ((sp = nc.head) != NULL) {
            cpi_service_peer_chain_del(&nc.head, &nc.tail, sp);
            sp->in_chain = 0;
            if (CODE(r) == CPI_ESUCC) {
                msg = nc.head == NULL? m: npc_message_dup(m, 0);
                msg->hdr = m->hdr;
                if (msg == NULL) {
                    r = RET(CPI_ENMEM);
                    continue;
                }
                if (npc_port_send(sp->write, msg) < 0) {
                    r = RET(CPI_EOPPT);
                    if (msg != m)
                        npc_message_close(msg);
                }
            }
        }
    }

out:
    return r;
}

static int cpi_service_notification_peer_collect(cpi_message_t *m, int appid, void *data)
{
    int r = RET(CPI_ESUCC);
    struct cpi_notification_collect_s *nc = (struct cpi_notification_collect_s *)data;
    cpi_service_peer_t *sp;

    (void)m;

    sp = cpi_service_peer_search(nc->svc, appid);
    if (sp != NULL && !sp->in_chain && sp->write != NULL) {
        cpi_service_peer_chain_add(&nc->head, &nc->tail, sp);
        sp->in_chain = 1;
    }
    
    return r;
}

int cpi_service_send_check(cpi_service_t *svc, cpi_message_t *m)
{
    int r = RET(CPI_ESUCC);
    npc_port_t *port;
    cpi_service_peer_t *sp;

    switch (cpi_service_get_role(svc)) {
        case svc_provider:
            if (m->hdr.type == NPC_MT_REQUEST) {
                r = RET(CPI_EPERM);
                goto out;
            }
            if (m->hdr.type == NPC_MT_NOTIFICATION) {
                r = cpi_service_notification_send_check(svc, m);
                goto out;
            }
            sp = cpi_service_peer_search(svc, m->hdr.client);
            if (sp == NULL) {
                r = RET(CPI_ENSCR);
                goto out;
            }
            port = cpi_service_get_port(sp, write);
            break;
        case svc_consumer:
            if (m->hdr.type != NPC_MT_REQUEST && m->hdr.type != NPC_MT_REQUEST_NO_RETURN) {
                r = RET(CPI_EPERM);
                goto out;
            }
            port = cpi_service_get_port(svc, write);
            break;
        default:
            r = RET(CPI_EPERM);
            goto out;
    }

    if (port == NULL) {
        r = RET(CPI_EPORT);
        goto out;
    } else {
        npc_port_handle_remove(port);
        r = npc_port_is_full(port);
        if (r < 0) {
            r = RET(CPI_EOPPT);
        } else if (r > 0) {
            r = RET(CPI_EAGAN);
        } else {
            r = RET(CPI_ESUCC);
        }
    }

out:
    return r;
}

static int cpi_service_notification_send_check(cpi_service_t *svc, cpi_message_t *m)
{
    return cpi_subscription_collect_peers(svc->subscriptions, cpi_service_notification_peer_check, m, svc);
}

static int cpi_service_notification_peer_check(cpi_message_t *m, int appid, void *data)
{
    int r = RET(CPI_ESUCC);
    cpi_service_t *svc = (cpi_service_t *)data;
    cpi_service_peer_t *sp;

    (void)m;

    sp = cpi_service_peer_search(svc, appid);
    if (sp != NULL && sp->write == NULL) {
        r = RET(CPI_EPORT);
        goto out;
    }

out:
    return r;
}

void cpi_service_reregister_service(cpi_rbtree_t *services)
{
    cpi_rbtree_scan_all(services, cpi_service_reregister_service_handler, NULL);
}

static int cpi_service_reregister_service_handler(cpi_rbtree_node_t *rn, void *rn_data, void *udata)
{
    int r = RET(CPI_ESUCC);
    cpi_service_t *svc = (cpi_service_t *)rn_data;

    (void)rn;
    (void)udata;

    if (cpi_service_get_role(svc) == svc_provider) {
        cpi_service_reoffer_service(svc);
    } else if (cpi_service_get_role(svc) == svc_consumer) {
        cpi_service_refind_service(svc);
    } else {
        r = RET(CPI_EPERM);
    }

    return r;
}

static int cpi_service_reoffer_service(cpi_service_t *svc)
{
    int r = RET(CPI_ESUCC);
    cpi_buf_t *b;
    cpi_frame_t f;
    cpi_u8ptr_t pdata;
    cpi_u32_t len;
    cpi_app_t *app = cpi_service_get_app(svc);

    cpi_frame_init(&f);

    f.src.inst = cpi_service_get_instanceid(svc);
    f.src.svc = cpi_service_get_serviceid(svc);
    f.src.major = cpi_service_get_major(svc);
    f.src.minor = cpi_service_get_minor(svc);
    f.src.appid = cpi_service_get_appid(svc);
    f.src.addr = cpi_app_get_svc_addr(app);
    memcpy(f.src.shm_path, cpi_service_get_shm_path(svc), CPI_SHM_PATH_LEN);
    f.src.seg_size = cpi_service_get_seg_size(svc);
    f.src.seg_count = cpi_service_get_seg_count(svc);
    f.op = cpi_frame_op_offer_service;

    b = cpi_buf_new();
    if (b == NULL) {
        r = RET(CPI_ENMEM);
        goto out;
    }

    pdata = cpi_buf_get_data(b);
    len = cpi_buf_get_avail_size(b);
    if (sizeof(f) > len) {
        r = RET(CPI_EBLEN);
        cpi_buf_free(b);
        goto out;
    }
    memcpy(pdata, &f, sizeof(f));
    cpi_buf_inc_len(b, sizeof(f));

    cpi_endpoint_buf_append(app->discovery, b, CPI_ENDPOINT_SEND);
    r = app->discovery->ops.send(app->discovery);

out:
    return r;
}

static int cpi_service_refind_service(cpi_service_t *svc)
{
    int r = RET(CPI_ESUCC);
    cpi_buf_t *b;
    cpi_frame_t f;
    cpi_u8ptr_t pdata;
    cpi_u32_t len;
    cpi_app_t *app = cpi_service_get_app(svc);

    cpi_frame_init(&f);

    f.dest.inst = cpi_service_get_instanceid(svc);
    f.dest.svc = cpi_service_get_serviceid(svc);
    f.dest.major = cpi_service_get_major(svc);
    f.dest.minor = cpi_service_get_minor(svc);
    f.src.appid = app->id;
    f.op = cpi_frame_op_find_service;

    b = cpi_buf_new();
    if (b == NULL) {
        r = RET(CPI_ENMEM);
        goto out;
    }

    pdata = cpi_buf_get_data(b);
    len = cpi_buf_get_avail_size(b);
    if (sizeof(f) > len) {
        r = RET(CPI_EBLEN);
        cpi_buf_free(b);
        goto out;
    }
    memcpy(pdata, &f, sizeof(f));
    cpi_buf_inc_len(b, sizeof(f));

    cpi_endpoint_buf_append(app->discovery, b, CPI_ENDPOINT_SEND);
    r = app->discovery->ops.send(app->discovery);

out:
    return r;
}

