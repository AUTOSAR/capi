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
/// @file       cpi_app.c
/// @brief
/// @details
/// @date       2022-11-11
/// @author     Zan Shigang
/// @version    1.2.0
///
/// ================================================================

#include "npc/cpi/cpi_types.h"
#include "nai/io/nai_buf.h"
#include "nai/os/nai_socket.h"
#include "nai/os/nai_system.h"
#include "nai/os/nai_system.h"
#include "npc/cpi/cpi_conf.h"
#include "npc/cpi/cpi_mm.h"
#include "npc/cpi/cpi_frame.h"
#include "npc/cpi/cpi_const.h"
#include "npc/cpi/cpi_app.h"
#include "npc/cpi/cpi_mm.h"
#include "npc/cpi/cpi_conf.h"
#include "npc/cpi/cpi_service.h"
#include "npc/cpi/cpi_rbtree.h"
#include "npc/cpi/cpi_endpoint.h"
#include "npc/cpi/cpi_error.h"
#include "npc/cpi/cpi_service.h"
#include <stdlib.h>
#include <arpa/inet.h>

struct cpi_app_find_svc_scan_s {
    cpi_app_t            *app;
    cpi_endpoint_addr_t  *addr;
    int                   appid;
    cpi_u16_t             svc;
    cpi_u16_t             inst;
    cpi_u32_t             major;
    cpi_u32_t             minor;
    int                   status;
    cpi_u32_t             seg_size;
    cpi_u32_t             seg_count;
    cpi_s8ptr_t           shm_path;
    cpi_app_find_svc_t   *head;
    cpi_app_find_svc_t   *tail;
};

CPI_CHAIN_FUNC_DECLARE(cpi_app_find_svc, cpi_app_find_svc_t, static inline void,);
CPI_CHAIN_FUNC_DEFINE(cpi_app_find_svc, cpi_app_find_svc_t, static inline void, prev, next);
CPI_CHAIN_FUNC_DECLARE(cpi_app_message_list, cpi_app_message_list_t, static inline void,);
CPI_CHAIN_FUNC_DEFINE(cpi_app_message_list, cpi_app_message_list_t, static inline void, prev, next);
static int __cpi_app_open(cpi_app_t *app, void **data);
static int __cpi_app_destroy(cpi_app_t *app, void **data);
static int cpi_app_init_service(cpi_app_t *app);
static int __cpi_app_offer_service(cpi_app_t *app, void **data);
static int __cpi_app_stop_service(cpi_app_t *app, void **data);
static int __cpi_app_request_service(cpi_app_t *app, void **data);
static int __cpi_app_release_service(cpi_app_t *app, void **data);
static int __cpi_app_offer_event(cpi_app_t *app, void **data);
static int __cpi_app_stop_event(cpi_app_t *app, void **data);
static int __cpi_app_request_event(cpi_app_t *app, void **data);
static int __cpi_app_release_event(cpi_app_t *app, void **data);
static int __cpi_app_subscribe(cpi_app_t *app, void **data);
static int __cpi_app_subscribe_ack(cpi_app_t *app, void **data);
static int __cpi_app_unsubscribe(cpi_app_t *app, void **data);
static cpi_app_find_svc_t *cpi_app_find_svc_new(int appid, cpi_u16_t svc, cpi_u16_t inst, cpi_u32_t major, cpi_u32_t minor);
static void cpi_app_find_svc_free(cpi_app_find_svc_t *fs);
static int cpi_app_find_svc_cmp(const cpi_app_find_svc_t *f1, const cpi_app_find_svc_t *f2);
static int cpi_app_find_svc_effect_scope(cpi_app_find_svc_t *f1, cpi_app_find_svc_t *f2);
static int cpi_app_find_svc_notice_send(cpi_app_find_svc_t *f, struct cpi_app_find_svc_scan_s *fss);
static int cpi_app_find_svc_collect(cpi_rbtree_node_t *node, void *rn_data, void *udata);
static int cpi_app_find_svc_unregister_scan(cpi_rbtree_node_t *rn, void *rn_data, void *udata);
static int __cpi_app_send(cpi_app_t *app, void **data);
static cpi_app_message_list_t *cpi_app_message_list_new(cpi_message_t *m);
static void cpi_app_message_list_free(cpi_app_message_list_t *ml);
static int cpi_app_send_cb(nai_evnode_t *node, int events);
static int cpi_app_dis_cb(nai_evnode_t *node, int events);

static cpi_string_t cpi_app_conf_host = cpi_string("host");
static cpi_string_t cpi_app_conf_dis_net_type = cpi_string("dis_net_type");
static cpi_string_t cpi_app_conf_dis_access_handle = cpi_string("dis_access_handle");

cpi_app_t *cpi_app_new(cpi_conf_t *conf)
{
    cpi_app_t *app;

    if ((app = (cpi_app_t *)cpi_malloc(sizeof(cpi_app_t))) == NULL) {
        return NULL;
    }
    if (cpi_app_init(app, conf) < 0) {
        cpi_free(app);
        return NULL;
    }
    return app;
}

int cpi_app_init(cpi_app_t *app, cpi_conf_t *conf)
{
    log("---LOG---\n");
    int r = RET(CPI_ESUCC);
    struct cpi_rbtree_attr rbattr;

    if (conf == NULL) {
        r = RET(CPI_ECONF);
        goto out;
    }

    app->id = -1;
    app->type = app_type_unknown;
    app->evloop = NULL;
    app->conf = conf;
    memset(&app->svc_addr, 0, sizeof(app->svc_addr));
    memset(&app->dis_addr, 0, sizeof(app->dis_addr));
    app->head = app->tail = NULL;
    app->quit_cb = NULL;
    app->subscribe_callback = NULL;
    app->subscribe_ack_callback = NULL;
    app->avail_callback = NULL;
    app->message_callback = NULL;
    app->sent_callback = NULL;
    app->iam_offer_callback = app->iam_find_callback = NULL;
    app->iam_subscribe_callback = NULL;
    app->iam_message_callback = NULL;
    app->send_timeout = 0;
    cpi_spin_init(&app->lock);
    nai_evnode_init(&app->node);
    nai_evnode_set_cb(&app->node, cpi_exec_cb);
    nai_evnode_init(&app->send_node);
    nai_evnode_set_cb(&app->send_node, cpi_app_send_cb);
    nai_evnode_init(&app->dis_node);
    nai_evnode_set_cb(&app->dis_node, cpi_app_dis_cb);

    rbattr.pool = NULL;
    rbattr.pool_alloc = NULL;
    rbattr.pool_free = NULL;
    rbattr.cache = 0;

    rbattr.cmp = (rbtree_cmp)cpi_endpoint_cmp;
    rbattr.data_free = (rbtree_free_data)cpi_endpoint_free;
    if ((app->endpoints = cpi_rbtree_init(&rbattr)) == NULL) {
        r = RET(CPI_ENMEM);
        goto out;
    }

    rbattr.cmp = (rbtree_cmp)cpi_service_cmp;
    rbattr.data_free = (rbtree_free_data)cpi_service_free;
    if ((app->services = cpi_rbtree_init(&rbattr)) == NULL) {
        r = RET(CPI_ENMEM);
        goto err1;
    }

    rbattr.cmp = (rbtree_cmp)cpi_endpoint_id_cmp;
    rbattr.data_free = NULL;
    if ((app->endpoints_id = cpi_rbtree_init(&rbattr)) == NULL) {
        r = RET(CPI_ENMEM);
        goto err2;
    }

    rbattr.cmp = (rbtree_cmp)cpi_app_find_svc_cmp;
    rbattr.data_free = (rbtree_free_data)cpi_app_find_svc_free;
    if ((app->find_services = cpi_rbtree_init(&rbattr)) == NULL) {
        r = RET(CPI_ENMEM);
        goto err3;
    }

    npc_comm_init(&app->shm_comm);

    app->data = NULL;
    app->discovery = NULL;
    app->msg_head = app->msg_tail = NULL;
    app->quit_flag = 0;

    goto out;

err3:
    cpi_rbtree_destroy(app->endpoints_id);
err2:
    cpi_rbtree_destroy(app->services);
err1:
    cpi_rbtree_destroy(app->endpoints);
out:
    return r;
}

int cpi_app_open(cpi_app_t *app)
{
    log("---LOG---\n");
    int r = RET(CPI_ESUCC);
    if (nai_evnode_open(&app->node, app->evloop) < 0) {
        r = RET(CPI_ENMEM);
        goto out;
    }

    r = cpi_exec(app, (cpi_exec_cb_t)__cpi_app_open, NULL, 0);

out:
    return r;
}

static int __cpi_app_open(cpi_app_t *app, void **data)
{
    log("---LOG---\n");
    int n, r = RET(CPI_ESUCC);
    cpi_endpoint_t *ep;
    cpi_rbtree_node_t *rn;
    cpi_exec_t *e = nai_containof(data, cpi_exec_t, data);
    cpi_conf_item_t *ci;
    cpi_string_t *host, *dis_net_type, *dis_access_handle;
    cpi_string_t type_unix = cpi_string("unixsock");
    cpi_string_t type_tcp = cpi_string("tcp");
    cpi_string_t id = cpi_string("appid");
    cpi_string_t mode = cpi_string("mode");
    cpi_string_t *mode_val;
    cpi_string_t mode_dis = cpi_string("discovery");
    cpi_string_t mode_app = cpi_string("application");
    cpi_string_t send_timeout = cpi_string("send_timeout");
    char access_path[1024] = {0};

    if (nai_evnode_open(&app->send_node, app->evloop) < 0) {
       r = RET(CPI_ENMEM);
       goto out;
    }

    if (nai_evnode_open(&app->dis_node, app->evloop) < 0) {
       r = RET(CPI_ENMEM);
       goto out;
    }

    if (npc_comm_open(&app->shm_comm, app->evloop) < 0) {
        r = RET(CPI_ECOMM);
        goto out;
    }

    ci = cpi_conf_search(app->conf, &id);
    if (ci == NULL || cpi_conf_get_type(ci) != cpi_conf_type_num) {
        r = RET(CPI_ECONF);
        goto out;
    }
    app->id = cpi_conf_get_int_val(ci);

    ci = cpi_conf_search(app->conf, &mode);
    if (ci == NULL || cpi_conf_get_type(ci) != cpi_conf_type_string) {
        r = RET(CPI_ECONF);
        goto out;
    }
    mode_val = cpi_conf_get_string_val(ci);
    if (!cpi_string_strcmp(mode_val, &mode_dis)) {
        app->type = app_type_dis;
    } else if (!cpi_string_strcmp(mode_val, &mode_app)) {
        app->type = app_type_app;

        ci = cpi_conf_search(app->conf, &send_timeout);
        if (ci == NULL || cpi_conf_get_type(ci) != cpi_conf_type_num) {
            app->send_timeout = 0;
        } else {
            app->send_timeout = cpi_conf_get_int_val(ci);
        }

        r = cpi_app_init_service(app);
        if (CODE(r) == CPI_ENENT) {
            r = RET(CPI_ESUCC);
            goto goon;
        }
        if (CODE(r) != CPI_ESUCC) {
            cpi_app_destroy(app);
            goto out;
        }
    } else {
        r = RET(CPI_ECONF);
        goto out;
    }

    if ((ep = cpi_endpoint_new(NULL, cpi_app_ep_get_listen_ops(), app, cpi_app_ep_iocb, &app->svc_addr)) == NULL) {
        r = RET(CPI_ENMEM);
        goto out;
    }
    if (ep->ops.open != NULL && (r = ep->ops.open(ep)) < 0) {
        cpi_endpoint_free(ep);
        goto out;
    }
    if (ep->ops.listen != NULL && (r = ep->ops.listen(ep)) < 0) {
        if (ep->ops.close != NULL) {
            cpi_endpoint_set_positive(ep);
            ep->ops.close(ep);
        }
        goto out;
    }

    if ((rn = cpi_rbtree_node_new(app->endpoints, ep)) == NULL) {
        if (ep->ops.close != NULL) {
            cpi_endpoint_set_positive(ep);
            ep->ops.close(ep);
        }
        r = RET(CPI_ENMEM);
        goto out;
    }
    cpi_rbtree_insert(rn);

goon:
    if (app->type == app_type_app) {
        ci = cpi_conf_search(app->conf, &cpi_app_conf_host);
        if (ci == NULL || cpi_conf_get_type(ci) != cpi_conf_type_string) {
            r = RET(CPI_ECONF);
            goto out;
        }
        host = cpi_conf_get_string_val(ci);

        ci = cpi_conf_search(app->conf, &cpi_app_conf_dis_net_type);
        if (ci == NULL || cpi_conf_get_type(ci) != cpi_conf_type_string) {
            r = RET(CPI_ECONF);
            goto out;
        }
        dis_net_type = cpi_conf_get_string_val(ci);
        if (!dis_net_type->len) {
            r = RET(CPI_ECONF);
            goto out;
        }

        ci = cpi_conf_search(app->conf, &cpi_app_conf_dis_access_handle);
        if (ci == NULL || cpi_conf_get_type(ci) != cpi_conf_type_string) {
            r = RET(CPI_ECONF);
            goto out;
        }
        dis_access_handle = cpi_conf_get_string_val(ci);
        if (!dis_access_handle->len) {
            r = RET(CPI_ECONF);
            goto out;
        }

        if (!cpi_string_strcmp(dis_net_type, &type_unix)) {
            n = snprintf(access_path, sizeof(access_path)-1, "unix:%s/%s", host->data, dis_access_handle->data);
            cpi_endpoint_set_nettype(&app->dis_addr, CPI_ENDPOINT_ADDR_UNIX);
            n = -1;
        } else if (!cpi_string_strcmp(dis_net_type, &type_tcp)) {
            n = snprintf(access_path, sizeof(access_path)-1, "%s:%s", host->data, dis_access_handle->data);
            cpi_endpoint_set_nettype(&app->dis_addr, CPI_ENDPOINT_ADDR_NET);
        } else {
            r = RET(CPI_ECONF);
            goto out;
        }
        cpi_endpoint_addr_pton(access_path, n, &app->dis_addr);

        if ((ep = cpi_endpoint_new(cpi_app_connect_ep_cb, cpi_app_ep_get_connect_ops(), app, cpi_app_ep_iocb, &app->dis_addr)) == NULL) {
            r = RET(CPI_ENMEM);
            goto out;
        }

        if (ep->ops.open != NULL && (r = ep->ops.open(ep)) < 0) {
            cpi_endpoint_free(ep);
            goto out;
        }
        if (ep->ops.connect != NULL && (r = ep->ops.connect(ep)) < 0) {
            if (ep->ops.close != NULL) {
                cpi_endpoint_set_positive(ep);
                ep->ops.close(ep);
            }
            goto out;
        }

        app->discovery = ep;
        if (!nai_evloop_in_dispatch(app->evloop))
            cpi_exec_transfer(e, ep);
    } else {
        cpi_exec_feedback(e);
    }

out:
    return r;
}

void cpi_app_free(cpi_app_t *app)
{
    cpi_app_destroy(app);
    cpi_free(app);
}

void cpi_app_destroy(cpi_app_t *app)
{
    log("---LOG---\n");
    if (app != NULL) {
        if (app->evloop == NULL || nai_evloop_in_dispatch(app->evloop)) {
            __cpi_app_destroy(app, NULL);
        } else {
            cpi_exec(app, (cpi_exec_cb_t)__cpi_app_destroy, NULL, 0);
        }
    }
}

static int __cpi_app_destroy(cpi_app_t *app, void **data)
{
    cpi_exec_t *e;
    cpi_app_message_list_t *ml;

    (void)data;

    app->quit_flag = 1;

    if (app->quit_cb != NULL)
        app->quit_cb(app);

    while ((ml = app->msg_head) != NULL) {
        cpi_app_message_list_chain_del(&app->msg_head, &app->msg_tail, ml);
        cpi_app_message_list_free(ml);
    }

    if (app->services != NULL)
        cpi_rbtree_destroy(app->services);

    if (app->find_services != NULL)
        cpi_rbtree_destroy(app->find_services);

    if (app->endpoints_id != NULL)
        cpi_rbtree_destroy(app->endpoints_id);

    if (app->endpoints != NULL)
        cpi_rbtree_destroy(app->endpoints);

    if (app->discovery != NULL)
        cpi_endpoint_free(app->discovery);

    npc_comm_close(&app->shm_comm);
    nai_evnode_close(&app->send_node);
    nai_evnode_close(&app->dis_node);

lp:
    cpi_spin_lock(&app->lock);
    e = app->head;
    if (e == NULL)
        goto out;

    cpi_spin_unlock(&app->lock);

    goto lp;

out:
    cpi_spin_unlock(&app->lock);
    nai_evnode_close(&app->node);
    cpi_spin_destroy(&app->lock);

    return RET(CPI_ESUCC);
}

static int cpi_app_init_service(cpi_app_t *app)
{
    int r = RET(CPI_ESUCC);
    cpi_string_t cpi_app_conf_nservice = cpi_string("nservice");
    cpi_string_t tmp, *s, host_key = cpi_string("host"), *host_val;
    cpi_s32_t nsvc, i, n;
    cpi_conf_item_t *ci;
    cpi_s8_t key[1024], shm_path[CPI_SHM_PATH_LEN] = {0};
    cpi_u32_t svcid, instid, major, minor, seg_size, seg_count, msg_queue_len;
    cpi_service_t *svc;
    cpi_rbtree_node_t *rn;

    ci = cpi_conf_search(app->conf, &host_key);
    if (ci == NULL) {
        r = RET(CPI_ENENT);
        goto out;
    }
    if (cpi_conf_get_type(ci) != cpi_conf_type_string) {
        r = RET(CPI_ECONF);
        goto out;
    }
    host_val = cpi_conf_get_string_val(ci);

    ci = cpi_conf_search(app->conf, &cpi_app_conf_nservice);
    if (ci == NULL) {
        r = RET(CPI_ENENT);
        goto out;
    }
    if (cpi_conf_get_type(ci) != cpi_conf_type_num) {
        r = RET(CPI_ECONF);
        goto out;
    }
    nsvc = cpi_conf_get_int_val(ci);

    if (!nsvc) {
        r = RET(CPI_ENENT);
        goto out;
    }

    for (i = 0; i < nsvc; ++i) {
        n = snprintf(key, sizeof(key)-1, "service.%d.service", i);
        key[n] = 0;
        cpi_string_nset(&tmp, key, n);
        ci = cpi_conf_search(app->conf, &tmp);
        if (ci == NULL || cpi_conf_get_type(ci) != cpi_conf_type_num) {
            r = RET(CPI_ECONF);
            goto out;
        }
        svcid = cpi_conf_get_int_val(ci);

        n = snprintf(key, sizeof(key)-1, "service.%d.instance", i);
        key[n] = 0;
        cpi_string_nset(&tmp, key, n);
        ci = cpi_conf_search(app->conf, &tmp);
        if (ci == NULL || cpi_conf_get_type(ci) != cpi_conf_type_num) {
            r = RET(CPI_ECONF);
            goto out;
        }
        instid = cpi_conf_get_int_val(ci);

        n = snprintf(key, sizeof(key)-1, "service.%d.major", i);
        key[n] = 0;
        cpi_string_nset(&tmp, key, n);
        ci = cpi_conf_search(app->conf, &tmp);
        if (ci == NULL || cpi_conf_get_type(ci) != cpi_conf_type_num) {
            r = RET(CPI_ECONF);
            goto out;
        }
        major = cpi_conf_get_int_val(ci);

        n = snprintf(key, sizeof(key)-1, "service.%d.minor", i);
        key[n] = 0;
        cpi_string_nset(&tmp, key, n);
        ci = cpi_conf_search(app->conf, &tmp);
        if (ci == NULL || cpi_conf_get_type(ci) != cpi_conf_type_num) {
            r = RET(CPI_ECONF);
            goto out;
        }
        minor = cpi_conf_get_int_val(ci);

        n = snprintf(key, sizeof(key)-1, "service.%d.shm_path", i);
        key[n] = 0;
        cpi_string_nset(&tmp, key, n);
        ci = cpi_conf_search(app->conf, &tmp);
        if (ci == NULL || cpi_conf_get_type(ci) != cpi_conf_type_string) {
            r = RET(CPI_ECONF);
            goto out;
        }
        s = cpi_conf_get_string_val(ci);
        if (!s->len) {
            r = RET(CPI_ECONF);
            goto out;
        }
        memcpy(key, s->data, CPI_SHM_PATH_LEN-1 > s->len? s->len: CPI_SHM_PATH_LEN-1);
        key[CPI_SHM_PATH_LEN-1 > s->len? s->len: CPI_SHM_PATH_LEN-1] = 0;
        n = snprintf(shm_path, sizeof(shm_path)-1, "%s/%s", host_val->data, key);
        shm_path[n] = 0;

        n = snprintf(key, sizeof(key)-1, "service.%d.shm_seg_size", i);
        key[n] = 0;
        cpi_string_nset(&tmp, key, n);
        ci = cpi_conf_search(app->conf, &tmp);
        if (ci == NULL || cpi_conf_get_type(ci) != cpi_conf_type_num) {
            r = RET(CPI_ECONF);
            goto out;
        }
        seg_size = cpi_conf_get_int_val(ci);

        n = snprintf(key, sizeof(key)-1, "service.%d.shm_seg_count", i);
        key[n] = 0;
        cpi_string_nset(&tmp, key, n);
        ci = cpi_conf_search(app->conf, &tmp);
        if (ci == NULL || cpi_conf_get_type(ci) != cpi_conf_type_num) {
            r = RET(CPI_ECONF);
            goto out;
        }
        seg_count = cpi_conf_get_int_val(ci);

        n = snprintf(key, sizeof(key)-1, "service.%d.msg_queue_len", i);
        key[n] = 0;
        cpi_string_nset(&tmp, key, n);
        ci = cpi_conf_search(app->conf, &tmp);
        if (ci == NULL || cpi_conf_get_type(ci) != cpi_conf_type_num) {
            r = RET(CPI_ECONF);
            goto out;
        }
        msg_queue_len = cpi_conf_get_int_val(ci);

        if ((svc = cpi_service_new(app,
                                   app->evloop,
                                   app->id,
                                   instid,
                                   svcid,
                                   major,
                                   minor,
                                   svc_provider,
                                   shm_path,
                                   seg_size,
                                   seg_count,
                                   msg_queue_len)) == NULL)
        {
            r = RET(CPI_ENMEM);
            goto out;
        }
        cpi_service_set_addr(svc, app->svc_addr);
        if ((rn = cpi_rbtree_node_new(app->services, svc)) == NULL) {
            cpi_service_free(svc);
            r = RET(CPI_ENMEM);
            goto out;
        }
        cpi_rbtree_insert(rn);
    }

out:
    return r;
}

int cpi_app_ep_iocb(nai_iobase_t *base, int events)
{
    cpi_endpoint_t *ep = nai_containof(base, cpi_endpoint_t, iobase);
    int r = RET(CPI_ESUCC);

    events &= NAI_EV_MASK;
    if (events & NAI_EV_ERROR) {
        if (ep->ops.error != NULL)
            r = ep->ops.error(ep);
    }
    if (CODE(r) != CPI_ESUCC) {
        goto fail;
    }
    if (events & NAI_EV_WRITE) {
        if (ep->ops.send != NULL)
            r = ep->ops.send(ep);
    }
    if (CODE(r) != CPI_ESUCC) {
        goto fail;
    }
    if (events & NAI_EV_READ) {
        if (ep->ops.recv != NULL)
            r = ep->ops.recv(ep);
    }
    if (CODE(r) == CPI_ESUCC) {
        goto out;
    }

fail:
    r = ep->ops.close(ep);

out:
    return r;
}


/*
 * offer service
 */
int cpi_app_offer_service(cpi_app_t* app, cpi_u16_t svc, cpi_u16_t inst, cpi_u32_t major, cpi_u32_t minor)
{
    log("---LOG---[%u][%u]\n", svc, inst);
    int r = RET(CPI_ESUCC);
    if (app->type == app_type_dis) {
        goto out;
    }
    cpi_app_exec_pack_t p = {
        app, app->id, inst, svc, major, minor
    };
    r = cpi_exec(app, (cpi_exec_cb_t)__cpi_app_offer_service, &p, 0);
out:
    return r;
}

static int __cpi_app_offer_service(cpi_app_t *app, void **data)
{
    int r = RET(CPI_ESUCC);
    cpi_buf_t *b;
    cpi_frame_t f;
    cpi_app_exec_pack_t *p = *((cpi_app_exec_pack_t **)data);
    cpi_u8ptr_t pdata;
    cpi_u32_t len;
    cpi_exec_t *e = nai_containof(data, cpi_exec_t, data);
    cpi_service_t *svc;

    svc = cpi_service_search(app->services, p->service_id, p->instance_id);
    if (svc == NULL) {
        r = RET(CPI_ENSVC);
        goto out;
    }

    cpi_frame_init(&f);

    f.src.inst = p->instance_id;
    f.src.svc = p->service_id;
    f.src.major = p->major;
    f.src.minor = p->minor;
    f.src.appid = app->id;
    f.src.e = e;
    f.src.addr = app->svc_addr;
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
    cpi_exec_transfer(e, app->discovery);
    memcpy(pdata, &f, sizeof(f));
    cpi_buf_inc_len(b, sizeof(f));

    cpi_endpoint_buf_append(app->discovery, b, CPI_ENDPOINT_SEND);
    r = app->discovery->ops.send(app->discovery);
    if (CODE(r) != CPI_ESUCC) {
        cpi_exec_no_transfer(e, app->discovery);
    }
out:
    return r;
}

/*
 * stop service
 */
int cpi_app_stop_service(cpi_app_t* app, cpi_u16_t svc, cpi_u16_t inst, cpi_u32_t major, cpi_u32_t minor)
{
    log("---LOG---\n");
    int r = RET(CPI_ESUCC);
    if (app->type == app_type_dis) {
        goto out;
    }
    cpi_app_exec_pack_t p = {
        app, app->id, inst, svc, major, minor
    };
    r = cpi_exec(app, (cpi_exec_cb_t)__cpi_app_stop_service, &p, 0);
out:
    return r;
}

static int __cpi_app_stop_service(cpi_app_t *app, void **data)
{
    int r = RET(CPI_ESUCC);
    cpi_buf_t *b;
    cpi_frame_t f;
    cpi_app_exec_pack_t *p = *((cpi_app_exec_pack_t **)data);
    cpi_u8ptr_t pdata;
    cpi_u32_t len;
    cpi_exec_t *e = nai_containof(data, cpi_exec_t, data);

    cpi_frame_init(&f);

    f.src.inst = p->instance_id;
    f.src.svc = p->service_id;
    f.src.major = p->major;
    f.src.minor = p->minor;
    f.src.appid = app->id;
    f.src.e = e;
    f.op = cpi_frame_op_stop_service;

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
    cpi_exec_transfer(e, app->discovery);
    memcpy(pdata, &f, sizeof(f));
    cpi_buf_inc_len(b, sizeof(f));

    cpi_endpoint_buf_append(app->discovery, b, CPI_ENDPOINT_SEND);
    r = app->discovery->ops.send(app->discovery);
    if (CODE(r) != CPI_ESUCC) {
        cpi_exec_no_transfer(e, app->discovery);
    }
out:
    return r;
}

/*
 * request service
 */
int cpi_app_request_service(cpi_app_t* app, cpi_u16_t svc, cpi_u16_t inst, cpi_u32_t major, cpi_u32_t minor)
{
    log("---LOG---[%u:%u:%u:%d]\n", svc, inst, major, minor);
    int r = RET(CPI_ESUCC);
    if (app->type == app_type_dis) {
        goto out;
    }
    cpi_app_exec_pack_t p = {
        app, app->id, inst, svc, major, minor
    };
    if (app->discovery == NULL) {
        r = RET(CPI_ENENP);
        goto out;
    }
    r = cpi_exec(app, (cpi_exec_cb_t)__cpi_app_request_service, &p, 0);

out:
    return r;
}

static int __cpi_app_request_service(cpi_app_t *app, void **data)
{
    int r = RET(CPI_ESUCC);
    cpi_buf_t *b;
    cpi_frame_t f;
    cpi_app_exec_pack_t *p = *((cpi_app_exec_pack_t **)data);
    cpi_u8ptr_t pdata;
    cpi_u32_t len;
    cpi_service_t *svc;
    cpi_endpoint_t *ep;
    cpi_exec_t *e = nai_containof(data, cpi_exec_t, data);

    cpi_frame_init(&f);

    if (p->service_id != CPI_SERVICE_ANY && p->instance_id != CPI_INSTANCE_ANY) {
        svc = cpi_service_search(app->services, p->service_id, p->instance_id);
        if (svc == NULL) {
            r = RET(CPI_ENSVC);
            goto out;
        }
        if (cpi_service_get_role(svc) != svc_consumer) {
            r = RET(CPI_ESOPR);
            goto out;
        }

        npc_comif_state(cpi_service_get_comif(svc), NPC_STATE_OPEN);
        ep = cpi_app_connect_service(svc);
        if (ep == NULL) {
            r = RET(CPI_ECONN);
            goto out;
        }
        r = cpi_service_auto_subscribe(svc, ep);
        if (CODE(r) != CPI_ESUCC)
            goto out;
        r = cpi_service_request_port(svc, ep);
        if (CODE(r) != CPI_ESUCC)
            goto out;
    }

    f.dest.inst = p->instance_id;
    f.dest.svc = p->service_id;
    f.dest.major = p->major;
    f.dest.minor = p->minor;
    f.src.appid = app->id;
    f.op = cpi_frame_op_find_service;
    if (p->instance_id != CPI_INSTANCE_ANY && !nai_evloop_in_dispatch(cpi_app_evloop_get(app)))
        f.src.e = e;

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

    if (p->instance_id != CPI_INSTANCE_ANY && !nai_evloop_in_dispatch(cpi_app_evloop_get(app)))
        cpi_exec_transfer(e, app->discovery);

    cpi_endpoint_buf_append(app->discovery, b, CPI_ENDPOINT_SEND);
    r = app->discovery->ops.send(app->discovery);
    if (p->instance_id != CPI_INSTANCE_ANY && !nai_evloop_in_dispatch(cpi_app_evloop_get(app)) && CODE(r) != CPI_ESUCC)
        cpi_exec_no_transfer(e, app->discovery);
out:
    return r;
}

cpi_endpoint_t *cpi_app_connect_service(void *service)
{
    int r;
    cpi_service_t *svc = (cpi_service_t *)service;
    cpi_app_t *app = cpi_service_get_app(svc);
    cpi_endpoint_t *ep;
    cpi_endpoint_addr_t addr;

    ep = cpi_endpoint_search_by_id(app->endpoints_id, cpi_service_get_appid(svc));
    if (ep != NULL) {
        cpi_endpoint_ref_inc(ep);
        goto out;
    }

    addr = cpi_service_get_addr(svc);

    if ((ep = cpi_endpoint_new(cpi_app_connect_ep_cb, cpi_app_ep_get_connect_ops(), app, cpi_app_ep_iocb, &addr)) == NULL) {
        goto out;
    }

    if (ep->ops.open != NULL && (r = ep->ops.open(ep)) < 0) {
        cpi_endpoint_free(ep);
        ep = NULL;
        goto out;
    }
    if (ep->ops.connect != NULL && (r = ep->ops.connect(ep)) < 0) {
        if (ep->ops.close != NULL) {
            cpi_endpoint_set_positive(ep);
            ep->ops.close(ep);
        }
        ep = NULL;
        goto out;
    }

    if (cpi_app_ep_connect_peerid_send(ep) != CPI_ESUCC) {
        if (ep->ops.close != NULL) {
            cpi_endpoint_set_positive(ep);
            ep->ops.close(ep);
        }
        ep = NULL;
        goto out;
    }

    if (ep->ops.send(ep) != CPI_ESUCC) {
        if (ep->ops.close != NULL) {
            cpi_endpoint_set_positive(ep);
            ep->ops.close(ep);
        }
        ep = NULL;
        goto out;
    }

    cpi_endpoint_set_peerid(ep, cpi_service_get_appid(svc));
    r = cpi_endpoint_add(app->endpoints_id, ep);
    if (CODE(r) != CPI_ESUCC) {
        if (ep->ops.close != NULL) {
            cpi_endpoint_set_positive(ep);
            ep->ops.close(ep);
        }
        ep = NULL;
        goto out;
    }

out:
    return ep;
}

/*
 * release service
 */
int cpi_app_release_service(cpi_app_t* app, cpi_u16_t svc, cpi_u16_t inst, cpi_u32_t major, cpi_u32_t minor)
{
    log("---LOG---%p %u %u %u %u\n", app->discovery, svc, inst, major, minor);
    int r = RET(CPI_ESUCC);
    if (app->type == app_type_dis) {
        goto out;
    }
    cpi_app_exec_pack_t p = {
        app, app->id, inst, svc, major, minor,
    };
    r = cpi_exec(app, (cpi_exec_cb_t)__cpi_app_release_service, &p, 0);

out:
    return r;
}

static int __cpi_app_release_service(cpi_app_t *app, void **data)
{
    int r = RET(CPI_ESUCC);
    cpi_buf_t *b;
    cpi_frame_t f;
    cpi_app_exec_pack_t *p = *((cpi_app_exec_pack_t **)data);
    cpi_u8ptr_t pdata;
    cpi_u32_t len;
    cpi_exec_t *e = nai_containof(data, cpi_exec_t, data);

    cpi_frame_init(&f);

    if (p->instance_id != CPI_INSTANCE_ANY) {
        r = cpi_service_remove_and_disconnect(app, p->service_id, p->instance_id, p->major, p->minor);
        if (CODE(r) != CPI_ESUCC) {
            if (CODE(r) == CPI_ENSVC || CODE(r) == CPI_EBUSY)
                r = RET(CPI_ESUCC);
            goto out;
        }
    }

    f.dest.inst = p->instance_id;
    f.dest.svc = p->service_id;
    f.dest.major = p->major;
    f.dest.minor = p->minor;
    f.src.appid = app->id;
    f.op = cpi_frame_op_stop_find_service;
    if (!nai_evloop_in_dispatch(cpi_app_evloop_get(app)))
        f.src.e = e;

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

    if (!nai_evloop_in_dispatch(cpi_app_evloop_get(app)))
        cpi_exec_transfer(e, app->discovery);

    cpi_endpoint_buf_append(app->discovery, b, CPI_ENDPOINT_SEND);
    r = app->discovery->ops.send(app->discovery);
    if (CODE(r) != CPI_ESUCC && !nai_evloop_in_dispatch(cpi_app_evloop_get(app)))
        cpi_exec_no_transfer(e, app->discovery);
out:
    return r;
}

/*
 * offer_event
 */
int cpi_app_offer_event(cpi_app_t *app, cpi_u16_t svc, cpi_u16_t inst, cpi_u16_t evid, cpi_event_info_t *info)
{
    log("---LOG---\n");
    int r = RET(CPI_ESUCC);
    if (app->type == app_type_dis) {
        goto out;
    }
    cpi_app_exec_pack_t p = {
        app, app->id, inst, svc, CPI_MAJOR_ANY, CPI_MINOR_ANY, evid, 0, 0, info
    };
    r = cpi_exec(app, (cpi_exec_cb_t)__cpi_app_offer_event, &p, 0);

out:
    return r;
}

static int __cpi_app_offer_event(cpi_app_t *app, void **data)
{
    int r = RET(CPI_ESUCC);
    cpi_app_exec_pack_t *p = *((cpi_app_exec_pack_t **)data);
    cpi_service_t *svc;

    svc = cpi_service_search(app->services, p->service_id, p->instance_id);
    if (svc == NULL) {
        r = RET(CPI_ENSVC);
        goto out;
    }

    r = cpi_service_event_add(svc, p->eid, p->info);

out:
    return r;
}


/*
 * stop_event
 */
int cpi_app_stop_event(cpi_app_t *app, cpi_u16_t svc, cpi_u16_t inst, cpi_u16_t evid)
{
    log("---LOG---\n");
    int r = RET(CPI_ESUCC);
    if (app->type == app_type_dis) {
        goto out;
    }
    cpi_app_exec_pack_t p = {
        app, app->id, inst, svc, CPI_MAJOR_ANY, CPI_MINOR_ANY, evid, 0, 0, NULL
    };
    r = cpi_exec(app, (cpi_exec_cb_t)__cpi_app_stop_event, &p, 0);

out:
    return r;
}

static int __cpi_app_stop_event(cpi_app_t *app, void **data)
{
    int r = RET(CPI_ESUCC);
    cpi_app_exec_pack_t *p = *((cpi_app_exec_pack_t **)data);
    cpi_service_t *svc;

    svc = cpi_service_search(app->services, p->service_id, p->instance_id);
    if (svc == NULL) {
        r = RET(CPI_ENSVC);
        goto out;
    }

    r = cpi_service_event_remove(svc, p->eid);

out:
    return r;
}

/*
 * request_event
 */
int cpi_app_request_event(cpi_app_t *app, cpi_u16_t svc, cpi_u16_t inst, cpi_u16_t eid, cpi_event_info_t *info)
{
    log("---LOG---\n");
    int r = RET(CPI_ESUCC);
    if (app->type == app_type_dis) {
        goto out;
    }
    cpi_app_exec_pack_t p = {
        app, app->id, inst, svc, CPI_MAJOR_ANY, CPI_MINOR_ANY, eid, 0, 0, info
    };
    r = cpi_exec(app, (cpi_exec_cb_t)__cpi_app_request_event, &p, 0);

out:
    return r;
}

static int __cpi_app_request_event(cpi_app_t *app, void **data)
{
    int r = RET(CPI_ESUCC);
    cpi_app_exec_pack_t *p = *((cpi_app_exec_pack_t **)data);
    cpi_service_t *svc;

    svc = cpi_service_search(app->services, p->service_id, p->instance_id);
    if (svc == NULL) {
        r = RET(CPI_ENSVC);
        goto out;
    }

    r = cpi_service_event_add(svc, p->eid, p->info);

out:
    return r;
}

/*
 * release_event
 */
int cpi_app_release_event(cpi_app_t *app, cpi_u16_t svc, cpi_u16_t inst, cpi_u16_t eid)
{
    log("---LOG---\n");
    int r = RET(CPI_ESUCC);
    if (app->type == app_type_dis) {
        goto out;
    }
    cpi_app_exec_pack_t p = {
        app, app->id, inst, svc, CPI_MAJOR_ANY, CPI_MINOR_ANY, eid, 0, 0, NULL
    };
    r = cpi_exec(app, (cpi_exec_cb_t)__cpi_app_release_event, &p, 0);

out:
    return r;
}

static int __cpi_app_release_event(cpi_app_t *app, void **data)
{
    int r = RET(CPI_ESUCC);
    cpi_app_exec_pack_t *p = *((cpi_app_exec_pack_t **)data);
    cpi_service_t *svc;

    svc = cpi_service_search(app->services, p->service_id, p->instance_id);
    if (svc == NULL) {
        r = RET(CPI_ENSVC);
        goto out;
    }

    r = cpi_service_event_remove(svc, p->eid);

out:
    return r;
}

/*
 * subscribe
 */
int cpi_app_subscribe(cpi_app_t *app, cpi_u16_t svc, cpi_u16_t inst, cpi_u16_t gid, cpi_u32_t major, cpi_u16_t eid)
{
    log("---LOG---%u %u %u %u\n", svc, inst, gid, eid);
    int r = RET(CPI_ESUCC);
    if (app->type == app_type_dis) {
        goto out;
    }
    cpi_app_exec_pack_t p = {
        app, app->id, inst, svc, major, CPI_MINOR_ANY, eid, gid, 0, NULL
    };
    r = cpi_exec(app, (cpi_exec_cb_t)__cpi_app_subscribe, &p, 0);

out:
    return r;
}

static int __cpi_app_subscribe(cpi_app_t *app, void **data)
{
    int r = RET(CPI_ESUCC);
    cpi_buf_t *b;
    cpi_frame_t f;
    cpi_app_exec_pack_t *p = *((cpi_app_exec_pack_t **)data);
    cpi_u8ptr_t pdata;
    cpi_u32_t len;
    cpi_endpoint_t *ep;
    cpi_service_t *svc;

    cpi_frame_init(&f);

    svc = cpi_service_search(app->services, p->service_id, p->instance_id);
    if (svc == NULL) {
        r = RET(CPI_ENSVC);
        goto out;
    }
    if (cpi_service_get_role(svc) != svc_consumer) {
        r = RET(CPI_ENSVC);
        goto out;
    }

    ep = cpi_endpoint_search_by_id(app->endpoints_id, cpi_service_get_appid(svc));
    if (ep == NULL) {
        r = RET(CPI_ENENP);
        goto out;
    }

    f.dest.inst = p->instance_id;
    f.dest.svc = p->service_id;
    f.dest.major = p->major;
    f.dest.minor = p->minor;
    f.dest.gid = p->gid;
    f.dest.eid = p->eid;
    f.src.appid = app->id;
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

    cpi_endpoint_buf_append(ep, b, CPI_ENDPOINT_SEND);
    r = ep->ops.send(ep);
out:
    return r;
}

/*
 * subscribe_ack
 */
int cpi_app_subscribe_ack(cpi_app_t *app, int code, int appid, cpi_u16_t svc, cpi_u16_t inst, cpi_u16_t gid, cpi_u16_t eid)
{
    log("---LOG---%d %d %u %u %u %u\n", code, appid, svc, inst, gid, eid);
    int r = RET(CPI_ESUCC);
    if (app->type == app_type_dis) {
        goto out;
    }
    cpi_app_exec_pack_t p = {
        app, appid, inst, svc, CPI_MAJOR_ANY, CPI_MINOR_ANY, eid, gid, code, NULL
    };
    r = cpi_exec(app, (cpi_exec_cb_t)__cpi_app_subscribe_ack, &p, 0);

out:
    return r;
}

static int __cpi_app_subscribe_ack(cpi_app_t *app, void **data)
{
    int r = RET(CPI_ESUCC);
    cpi_buf_t *b;
    cpi_frame_t f;
    cpi_app_exec_pack_t *p = *((cpi_app_exec_pack_t **)data);
    cpi_u8ptr_t pdata;
    cpi_u32_t len;
    cpi_endpoint_t *ep;
    cpi_service_t *svc;

    cpi_frame_init(&f);

    svc = cpi_service_search(app->services, p->service_id, p->instance_id);
    if (svc == NULL) {
        r = RET(CPI_ENSVC);
        goto out;
    }
    if (cpi_service_get_role(svc) != svc_provider) {
        r = RET(CPI_ENSVC);
        goto out;
    }

    ep = cpi_endpoint_search_by_id(app->endpoints_id, p->appid);
    if (ep == NULL) {
        r = RET(CPI_ENENP);
        goto out;
    }

    f.src.inst = p->instance_id;
    f.src.svc = p->service_id;
    f.src.major = p->major;
    f.src.minor = p->minor;
    f.src.gid = p->gid;
    f.src.eid = p->eid;
    f.src.appid = app->id;
    f.src.code = p->code;
    f.dest.appid = p->appid;
    f.op = cpi_frame_op_subscribe_ack;

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

    r = cpi_service_subscription_set(svc, cpi_endpoint_get_peerid(ep), p->gid, p->eid);
    if (CODE(r) != CPI_ESUCC) {
        cpi_buf_free(cpi_endpoint_snd_remove(ep));
        goto out;
    }

    r = ep->ops.send(ep);
out:
    return r;
}

/*
 * unsubscribe
 */
int cpi_app_unsubscribe(cpi_app_t *app, cpi_u16_t svc, cpi_u16_t inst, cpi_u16_t gid, cpi_u32_t major, cpi_u16_t eid)
{
    log("---LOG---\n");
    int r = RET(CPI_ESUCC);
    if (app->type == app_type_dis) {
        goto out;
    }
    cpi_app_exec_pack_t p = {
        app, app->id, inst, svc, major, CPI_MINOR_ANY, eid, gid, 0, NULL
    };
    r = cpi_exec(app, (cpi_exec_cb_t)__cpi_app_unsubscribe, &p, 0);

out:
    return r;
}

static int __cpi_app_unsubscribe(cpi_app_t *app, void **data)
{
    int r = RET(CPI_ESUCC);
    cpi_buf_t *b;
    cpi_frame_t f;
    cpi_app_exec_pack_t *p = *((cpi_app_exec_pack_t **)data);
    cpi_u8ptr_t pdata;
    cpi_u32_t len;
    cpi_endpoint_t *ep;
    cpi_service_t *svc;

    cpi_frame_init(&f);

    svc = cpi_service_search(app->services, p->service_id, p->instance_id);
    if (svc == NULL) {
        r = RET(CPI_ENSVC);
        goto out;
    }
    if (cpi_service_get_role(svc) != svc_consumer) {
        r = RET(CPI_ENSVC);
        goto out;
    }

    ep = cpi_endpoint_search_by_id(app->endpoints_id, cpi_service_get_appid(svc));
    if (ep == NULL) {
        r = RET(CPI_ENENP);
        goto out;
    }

    f.dest.inst = p->instance_id;
    f.dest.svc = p->service_id;
    f.dest.major = p->major;
    f.dest.minor = p->minor;
    f.dest.gid = p->gid;
    f.dest.eid = p->eid;
    f.dest.appid = cpi_service_get_appid(svc);
    f.src.appid = p->appid;
    f.op = cpi_frame_op_unsubscribe;

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

    r = cpi_service_subscription_remove(svc, cpi_app_get_id(app), p->gid, p->eid);
    if (CODE(r) != CPI_ESUCC) {
        cpi_buf_free(cpi_endpoint_snd_remove(ep));
        goto out;
    }

    r = ep->ops.send(ep);
out:
    return r;
}

/*
 * cpi_app_find_svc_t
 */
static cpi_app_find_svc_t *cpi_app_find_svc_new(int appid, cpi_u16_t svc, cpi_u16_t inst, cpi_u32_t major, cpi_u32_t minor)
{
    cpi_app_find_svc_t *fs;

    if ((fs = (cpi_app_find_svc_t *)cpi_malloc(sizeof(cpi_app_find_svc_t))) == NULL) {
        goto out;
    }
    fs->appid = appid;
    fs->svc = svc;
    fs->inst = inst;
    fs->major = major;
    fs->minor = minor;
    fs->prev = fs->next = NULL;

out:
    return fs;
}

static void cpi_app_find_svc_free(cpi_app_find_svc_t *fs)
{
    if (fs != NULL)
        cpi_free(fs);
}

static int cpi_app_find_svc_cmp(const cpi_app_find_svc_t *f1, const cpi_app_find_svc_t *f2)
{
    int r = 0;

    if (f1->appid > f2->appid)
        r = 1;
    else if (f1->appid < f2->appid)
        r = -1;
    else if (f1->svc > f2->svc)
        r = 1;
    else if (f1->svc < f2->svc)
        r = -1;
    else if (f1->inst > f2->inst)
        r = 1;
    else if (f1->inst < f2->inst)
        r = -1;
    else if (f1->major > f2->major)
        r = 1;
    else if (f1->major < f2->major)
        r = -1;
    else if (f1->minor > f2->minor)
        r = 1;
    else if (f1->minor < f2->minor)
        r = -1;
    return r;
}

int cpi_app_find_svc_register(cpi_app_t *app, int appid, cpi_u16_t svc, cpi_u16_t inst, cpi_u32_t major, cpi_u32_t minor)
{
    int r = RET(CPI_ESUCC);
    cpi_rbtree_node_t *rn;
    cpi_app_find_svc_t f, *fs;

    f.appid = appid;
    f.svc = svc;
    f.inst = inst;
    f.major = major;
    f.minor = minor;
    rn = cpi_rbtree_search(app->find_services, &f);
    if (!cpi_rbtree_null(rn)) {
        goto out;
    }

    if ((fs = cpi_app_find_svc_new(appid, svc, inst, major, minor)) == NULL) {
        r = RET(CPI_ENMEM);
        goto out;
    }
    if ((rn = cpi_rbtree_node_new(app->find_services, fs)) == NULL) {
        cpi_app_find_svc_free(fs);
        r = RET(CPI_ENMEM);
        goto out;
    }
    cpi_rbtree_insert(rn);

out:
    return r;
}

void cpi_app_find_svc_unregister(cpi_app_t *app, int appid, cpi_u16_t svc, cpi_u16_t inst, cpi_u32_t major, cpi_u32_t minor)
{
    cpi_app_find_svc_t f;

    f.appid = appid;
    f.svc = svc;
    f.inst = inst;
    f.major = major;
    f.minor = minor;

    cpi_rbtree_scan_all(app->find_services, cpi_app_find_svc_unregister_scan, &f);
}

static int cpi_app_find_svc_unregister_scan(cpi_rbtree_node_t *rn, void *rn_data, void *udata)
{
    cpi_app_find_svc_t *in = (cpi_app_find_svc_t *)udata;
    cpi_app_find_svc_t *f = (cpi_app_find_svc_t *)rn_data;

    if (in->appid != f->appid ||
        (in->svc != CPI_SERVICE_ANY && in->svc != f->svc) ||
        (in->inst != CPI_INSTANCE_ANY && in->inst != f->inst) ||
        (in->major != CPI_MAJOR_ANY && in->major != f->major) ||
        (in->minor != CPI_MINOR_ANY && in->minor != f->minor))
    {
        goto out;
    }

    cpi_rbtree_delete(rn);
    cpi_rbtree_node_free(rn);

out:
    return 0;
}

int cpi_app_find_svc_notice(cpi_app_t *app,
                            int appid,
                            cpi_u16_t svc,
                            cpi_u16_t inst,
                            cpi_u32_t major,
                            cpi_u32_t minor,
                            int status,
                            cpi_endpoint_addr_t *addr,
                            cpi_s8ptr_t shm_path,
                            cpi_u32_t seg_size,
                            cpi_u32_t seg_count)
{
    log("---LOG---\n");
    int r = RET(CPI_ESUCC);
    struct cpi_app_find_svc_scan_s fss;
    cpi_app_find_svc_t *f;

    fss.app = app;
    fss.appid = appid;
    fss.svc = svc;
    fss.inst = inst;
    fss.major = major;
    fss.minor = minor;
    fss.status = status;
    fss.addr = addr;
    fss.head = fss.tail = NULL;
    fss.shm_path = shm_path;
    fss.seg_size = seg_size;
    fss.seg_count = seg_count;

    cpi_rbtree_scan_all(app->find_services, cpi_app_find_svc_collect, &fss);

    while ((f = fss.head) != NULL) {
        cpi_app_find_svc_chain_del(&fss.head, &fss.tail, f);
        if (CODE(r) == CPI_ESUCC)
            r = cpi_app_find_svc_notice_send(f, &fss);
    }

    return r;
}

static int cpi_app_find_svc_collect(cpi_rbtree_node_t *node, void *rn_data, void *udata)
{
    int r = RET(CPI_ESUCC);
    int ret, append_flag = 1;
    cpi_app_find_svc_t *f = (cpi_app_find_svc_t *)rn_data, *p, *q;
    struct cpi_app_find_svc_scan_s *fss = (struct cpi_app_find_svc_scan_s *)udata;

    (void)node;

    if (f->svc != fss->svc)
        goto out;

    if (f->inst != CPI_INSTANCE_ANY && f->inst != fss->inst)
        goto out;

    if (f->major != CPI_MAJOR_ANY && f->major != fss->major)
        goto out;

    if (f->minor != CPI_MINOR_ANY && f->minor != fss->minor)
        goto out;

    p = fss->head;
    while (p != NULL) {
        ret = cpi_app_find_svc_effect_scope(p, f);
        if (ret < 0) {
            q = p->next;
            cpi_app_find_svc_chain_del(&fss->head, &fss->tail, p);
            append_flag = 1;
            p = q;
            continue;
        } else if (ret > 0) {
            append_flag = 0;
        } else {
            append_flag = 1;
        }
        p = p->next;
    }
    if (append_flag)
        cpi_app_find_svc_chain_add(&fss->head, &fss->tail, f);

out:
    return r;
}

static int cpi_app_find_svc_effect_scope(cpi_app_find_svc_t *f1, cpi_app_find_svc_t *f2)
{
    int r = 0;

    if (f1->appid != f2->appid)
        goto out;

    if (f1->inst == f2->inst || f1->inst == CPI_INSTANCE_ANY)
        r = 1;
    else if (f2->inst == CPI_INSTANCE_ANY)
        r = -1;

    if (r)
        goto out;

    if (f1->major == f2->major || f1->major == CPI_MAJOR_ANY)
        r = 1;
    else if (f2->major == CPI_MAJOR_ANY)
        r = -1;

    if (r)
        goto out;

    if (f1->minor == f2->minor || f1->minor == CPI_MINOR_ANY)
        r = 1;
    else if (f2->minor == CPI_MINOR_ANY)
        r = -1;

out:
    return r;
}

static int cpi_app_find_svc_notice_send(cpi_app_find_svc_t *f, struct cpi_app_find_svc_scan_s *fss)
{
    int r = RET(CPI_ESUCC);
    cpi_endpoint_t *ep;
    cpi_buf_t *b;
    cpi_u32_t len;
    cpi_u8ptr_t pdata;
    cpi_frame_t fr;

    cpi_frame_init(&fr);

    ep = cpi_endpoint_search_by_id(fss->app->endpoints_id, f->appid);
    if (ep == NULL) {
        r = RET(CPI_ENENP);
        goto out;
    }

    fr.op = cpi_frame_op_find_service_ack;
    fr.src.code = fss->status == svc_online? CPI_AVAIL_ONLINED: CPI_AVAIL_OFFLINED;
    fr.src.svc = fss->svc;
    fr.src.inst = fss->inst;
    fr.src.major = fss->major;
    fr.src.minor = fss->minor;
    fr.dest.appid = cpi_endpoint_get_peerid(ep);
    fr.dest.inst = CPI_INSTANCE_ANY;
    fr.dest.e = NULL;
    fr.src.addr = *(fss->addr);
    fr.src.appid = fss->appid;
    memcpy(fr.src.shm_path, fss->shm_path, CPI_SHM_PATH_LEN);
    fr.src.seg_size = fss->seg_size;
    fr.src.seg_count = fss->seg_count;

    if ((b = cpi_buf_new()) == NULL) {
        r = RET(CPI_ENMEM);
        goto out;
    }
    pdata = cpi_buf_get_data(b);
    len = cpi_buf_get_avail_size(b);
    if (sizeof(fr) > len) {
        r = RET(CPI_EBLEN);
        cpi_buf_free(b);
        goto out;
    }
    memcpy(pdata, &fr, sizeof(fr));
    cpi_buf_inc_len(b, sizeof(fr));
    cpi_endpoint_buf_append(ep, b, CPI_ENDPOINT_SEND);

    r = ep->ops.send(ep);
out:
    return r;
}


/*
 * create_message
 */
cpi_message_t *cpi_app_create_message(cpi_app_t *app, cpi_u16_t svc, cpi_u16_t inst)
{
    cpi_message_t *m;

    m = npc_comm_create_message(&app->shm_comm, svc, inst, NPC_USAGE_SEND);

    return m;
}

cpi_message_t *cpi_app_create_request(cpi_app_t *app, cpi_u16_t svc, cpi_u16_t inst, cpi_method_t meth)
{
    cpi_message_t *m;

    m = cpi_app_create_message(app, svc, inst);
    if (m != NULL) {
        m->inst = inst;
        m->hdr.serv = svc;
        m->hdr.method = meth;
        m->hdr.len = sizeof(m->hdr);
        m->hdr.client = (cpi_u16_t)app->id;
        m->hdr.session = 0;
        m->hdr.protocol = NPC_PROTOCOL_VERSION;
        m->hdr.interface = NPC_DEFAULT_MAJOR;
        m->hdr.type = NPC_MT_REQUEST;
        m->hdr.code = NPC_E_OK;
    }

    return m;
}

cpi_message_t* cpi_app_create_response(cpi_app_t *app, cpi_message_t *r, cpi_u8_t sult)
{
    cpi_message_t *m;

    m = cpi_app_create_message(app, r->hdr.serv, r->inst);
    if (m != NULL) {
        m->reliable = r->reliable;
        m->inst = r->inst;
        m->hdr.serv = r->hdr.serv;
        m->hdr.method = r->hdr.method;
        m->hdr.len = sizeof(m->hdr);
        m->hdr.client = r->hdr.client;
        m->hdr.session = r->hdr.session;
        m->hdr.protocol = r->hdr.protocol;
        m->hdr.interface = r->hdr.interface;
        m->hdr.type = sult == NPC_E_OK ? NPC_MT_RESPONSE : NPC_MT_ERROR;
        m->hdr.code = sult;
    }

    return m;
}

cpi_message_t* cpi_app_create_notification(cpi_app_t *app, cpi_u16_t svc, cpi_u16_t inst, cpi_u16_t event)
{
    cpi_message_t *m;

    m = cpi_app_create_message(app, svc, inst);
    if (m != NULL) {
        m->inst = inst;
        m->hdr.serv = svc;
        m->hdr.method = event;
        m->hdr.len = sizeof(m->hdr);
        m->hdr.client = (cpi_u16_t)app->id;
        m->hdr.session = 0;
        m->hdr.protocol = NPC_PROTOCOL_VERSION;
        m->hdr.interface = NPC_DEFAULT_MAJOR;
        m->hdr.type = NPC_MT_NOTIFICATION;
        m->hdr.code = NPC_E_OK;
    }

    return m;
}

void cpi_app_init_notification(cpi_message_t *m, cpi_app_t *app, cpi_u16_t svc, cpi_u16_t inst, cpi_u16_t event)
{
    m->inst = inst;
    m->hdr.serv = svc;
    m->hdr.method = event;
    m->hdr.len = sizeof(m->hdr);
    m->hdr.client = (cpi_u16_t)app->id;
    m->hdr.session = 0;
    m->hdr.protocol = NPC_PROTOCOL_VERSION;
    m->hdr.interface = NPC_DEFAULT_MAJOR;
    m->hdr.unserialized = 1;
    m->hdr.type = NPC_MT_NOTIFICATION;
    m->hdr.code = NPC_E_OK;
}

/*
 * memory detection
 */
int cpi_app_ptr_in_shm(cpi_app_t *app, cpi_u16_t svc, cpi_u16_t inst, const void *ptr)
{
    int r = 0;
    cpi_service_t *s;
    npc_comif_t *cif;
    npc_mref_t ref;

    s = cpi_service_search(app->services, svc, inst);
    if (s == NULL) {
        goto out;
    }

    cif = cpi_service_get_comif(s);
    if (cif == NULL)
        goto out;

    ref = npc_shmz_2ref(&cif->shmz, ptr);
    if (ref)
        r = 1;

out:
    return r;
}

/*
 * send
 */
int cpi_app_send(cpi_app_t *app, cpi_message_t *m)
{
    int r = RET(CPI_ESUCC);
    log("---LOG--- m:%p serv:%u inst:%u method:%u type:%u\n", m, m->hdr.serv, m->inst, m->hdr.method, m->hdr.type);
    cpi_app_exec_pack_t *p;

    if (app->type == app_type_dis) {
        goto out;
    }

    if ((p = (cpi_app_exec_pack_t *)cpi_malloc(sizeof(cpi_app_exec_pack_t))) == NULL) {
        r = RET(CPI_ENMEM);
        goto out;
    }
    p->app = app;
    p->appid = app->id;
    p->m = m;

    r = cpi_exec(app, (cpi_exec_cb_t)__cpi_app_send, p, 1);
out:
    return r;
}

static int __cpi_app_send(cpi_app_t *app, void **data)
{
    int r = RET(CPI_ESUCC);
    cpi_app_message_list_t *ml;
    cpi_app_exec_pack_t *p = *((cpi_app_exec_pack_t **)data);
    log("---LOG--- %p\n", p->m);

    if ((ml = cpi_app_message_list_new(p->m)) == NULL) {
        r = RET(CPI_ENMEM);
        goto out;
    }
    cpi_app_message_list_chain_add(&app->msg_head, &app->msg_tail, ml);
    log("---LOG--- msg_head:%p msg_tail:%p m:%p serv:%u inst:%u method:%u type: %u session: %u\n", app->msg_head, app->msg_tail, p->m, p->m->hdr.serv, p->m->inst, p->m->hdr.method, p->m->hdr.type, p->m->hdr.session);
    cpi_app_send_cb(&app->send_node, 0);

out:
    cpi_free(p);
    return r;
}

static int cpi_app_send_cb(nai_evnode_t *node, int events)
{
    log("---LOG---\n");
    int r;
    cpi_app_t *app = nai_containof(node, cpi_app_t, send_node);
    cpi_service_t *svc;
    cpi_app_message_list_t *ml, *fr;
    cpi_message_t *m;
    cpi_u64_t ts;

    (void)events;

    ts = nai_time();

    ml = app->msg_head;
    while (ml != NULL) {
        m = ml->m;
    log("---LOG--- msg_head:%p msg_tail:%p m:%p serv:%u inst:%u method:%u type: %u session: %u\n", app->msg_head, app->msg_tail, m, m->hdr.serv, m->inst, m->hdr.method, m->hdr.type, m->hdr.session);
        svc = cpi_service_search(app->services, m->hdr.serv, m->inst);
        if (svc == NULL) {
            r = RET(CPI_ENSVC);
            goto err;
        }

        r = cpi_service_send_check(svc, m);
        if (CODE(r) == CPI_EPORT || CODE(r) == CPI_EAGAN) {
            if (!ml->ts || ts - ml->ts >= (cpi_u64_t)app->send_timeout) {
                if (++ml->retry >= CPI_APP_RETRY_MAX) {
                    goto err;
                }
                nai_evnode_set_timeout(&app->send_node, NAI_TIMEOP_SET, app->send_timeout);
                ml->ts = ts;
            }
            ml = ml->next;
            continue;
        } else if (CODE(r) != CPI_ESUCC) {
err:
            fr = ml;
            ml = ml->next;
            cpi_app_message_list_chain_del(&app->msg_head, &app->msg_tail, ml);
            if (app->sent_callback != NULL)
                app->sent_callback(app, app->data, m, r);
            cpi_app_message_list_free(fr);
            continue;
        }

        fr = ml;
        ml = ml->next;
        cpi_app_message_list_chain_del(&app->msg_head, &app->msg_tail, fr);
        fr->m = NULL;

        r = cpi_service_send(svc, m);

        cpi_app_message_list_free(fr);
    }

    return RET(CPI_ESUCC);
}

static int cpi_app_dis_cb(nai_evnode_t *node, int events)
{
    int r = RET(CPI_ESUCC);
    cpi_endpoint_t *ep;
    cpi_app_t *app = nai_containof(node, cpi_app_t, dis_node);

    (void)events;

    if ((ep = cpi_endpoint_new(cpi_app_connect_ep_cb, cpi_app_ep_get_connect_ops(), app, cpi_app_ep_iocb, &app->dis_addr)) == NULL) {
        r = RET(CPI_ENMEM);
        goto out;
    }
    if (ep->ops.open != NULL && (r = ep->ops.open(ep)) < 0) {
        cpi_endpoint_free(ep);
        nai_evnode_set_timeout(cpi_app_get_dis_evnode(app), NAI_TIMEOP_SET, 100);
        goto out;
    }
    if (ep->ops.connect != NULL && (r = ep->ops.connect(ep)) < 0) {
        if (ep->ops.close != NULL) {
            cpi_endpoint_set_positive(ep);
            ep->ops.close(ep);
        }
        nai_evnode_set_timeout(cpi_app_get_dis_evnode(app), NAI_TIMEOP_SET, 100);
        goto out;
    }
    if ((r = cpi_app_ep_connect_peerid_send(ep)) < 0) {
        if (ep->ops.close != NULL) {
            cpi_endpoint_set_positive(ep);
            ep->ops.close(ep);
        }
        ep = NULL;
        nai_evnode_set_timeout(cpi_app_get_dis_evnode(app), NAI_TIMEOP_SET, 100);
        goto out;
    }

    if ((r = ep->ops.send(ep)) < 0) {
        if (ep->ops.close != NULL) {
            cpi_endpoint_set_positive(ep);
            ep->ops.close(ep);
        }
        ep = NULL;
        nai_evnode_set_timeout(cpi_app_get_dis_evnode(app), NAI_TIMEOP_SET, 100);
        goto out;
    }

    app->discovery = ep;

    cpi_service_reregister_service(app->services);
out:
    return r;
}

/*
 * cpi_app_message_list_t
 */
static cpi_app_message_list_t *cpi_app_message_list_new(cpi_message_t *m)
{
    cpi_app_message_list_t *ml;

    if ((ml = (cpi_app_message_list_t *)cpi_malloc(sizeof(cpi_app_message_list_t))) == NULL) {
        ml = NULL;
        goto out;
    }

    ml->m = m;
    ml->ts = 0;
    ml->retry = 0;
    ml->prev = ml->next = NULL;

out:
    return ml;
}

static void cpi_app_message_list_free(cpi_app_message_list_t *ml)
{
    if (ml != NULL) {
        if (ml->m != NULL) {
            npc_message_close(ml->m);
        }
        cpi_free(ml);
    }
}

/*
 * only for cpi.h
 */
void cpi_app_set_ud(cpi_app_t *app, void *data)
{
    cpi_app_set_data(app, data);
}

void cpi_app_set_cb_available(cpi_app_t *app, cpi_app_avail_cb_t cb)
{
    cpi_app_set_avail_cb(app, cb);
}

void cpi_app_set_cb_message(cpi_app_t *app, cpi_app_msg_cb_t cb)
{
    cpi_app_set_msg_cb(app, cb);
}

void cpi_app_set_cb_sent(cpi_app_t *app, cpi_app_sent_cb_t cb)
{
    cpi_app_set_sent_cb(app, cb);
}

void cpi_app_set_cb_subscribe(cpi_app_t *app, cpi_app_sub_cb_t cb)
{
    cpi_app_set_sub_cb(app, cb);
}

void cpi_app_set_cb_subscribe_ack(cpi_app_t *app, cpi_app_suback_cb_t cb)
{
    cpi_app_set_suback_cb(app, cb);
}

void cpi_app_set_evloop(cpi_app_t *app, void *ev)
{
    cpi_app_evloop_set(app, ev);
}

void cpi_app_set_cb_iam_offer(cpi_app_t *app, cpi_app_service_cb_t cb)
{
    cpi_app_iam_offer_cb_set(app, cb);
}

void cpi_app_set_cb_iam_find(cpi_app_t *app, cpi_app_service_cb_t cb)
{
    cpi_app_iam_find_cb_set(app, cb);
}

void cpi_app_set_cb_iam_subscribe(cpi_app_t *app, cpi_app_sub_cb_t cb)
{
    cpi_app_iam_subscribe_cb_set(app, cb);
}

void cpi_app_set_cb_iam_message(cpi_app_t *app, cpi_app_msg_cb_t cb)
{
    cpi_app_iam_message_cb_set(app, cb);
}

/*
 * shm operations
 */
void *cpi_app_shm_alloc(cpi_app_t *app, cpi_u16_t svc, cpi_u16_t inst, size_t size)
{
    int r = RET(CPI_ESUCC);
    npc_message_t *m;
    cpi_u8ptr_t ptr = NULL;

    m = cpi_app_create_message(app, svc, inst);
    if (m == NULL) {
        r = RET(CPI_ENMEM);
        goto out;
    }

    ptr = npc_message_buf_alloc(m, size);
    if (ptr == NULL) {
        r = RET(CPI_ENMEM);
        goto out;
    }
out:
    (void)r;
    return ptr;
}

void cpi_app_shm_free(void *ptr, size_t val_size)
{
    npc_message_t *m;

    if (ptr != NULL) {
        m = npc_message_get_from_buf(ptr, val_size);
        if (m != NULL) {
            npc_message_close(m);
        }
    }
}
