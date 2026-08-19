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
/// @file       cpi_frame.c
/// @brief
/// @details
/// @date       2022-11-11
/// @author     Zan Shigang
/// @version    1.2.0
///
/// ================================================================

#include "npc/cpi/cpi_frame.h"
#include "npc/cpi/cpi_app.h"
#include "npc/cpi/cpi_endpoint.h"
#include "npc/cpi/cpi_error.h"
#include "npc/cpi/cpi_service.h"
#include "npc/cpi/cpi_buf.h"
#include "npc/cpi/cpi_const.h"

static int cpi_frame_op_peer_id_handler(cpi_endpoint_t *ep, cpi_frame_t *f);
static int cpi_frame_op_offer_service_handler(cpi_endpoint_t *ep, cpi_frame_t *f);
static int cpi_frame_op_offer_service_ack_handler(cpi_endpoint_t *ep, cpi_frame_t *f);
static int cpi_frame_op_offer_service_nack_handler(cpi_endpoint_t *ep, cpi_frame_t *f);
static int cpi_frame_op_stop_service_handler(cpi_endpoint_t *ep, cpi_frame_t *f);
static int cpi_frame_op_stop_service_ack_handler(cpi_endpoint_t *ep, cpi_frame_t *f);
static int cpi_frame_op_stop_service_nack_handler(cpi_endpoint_t *ep, cpi_frame_t *f);
static int cpi_frame_op_find_service_handler(cpi_endpoint_t *ep, cpi_frame_t *f);
static int cpi_frame_op_find_service_ack_handler(cpi_endpoint_t *ep, cpi_frame_t *f);
static int cpi_frame_op_find_request_port_handler(cpi_endpoint_t *ep, cpi_frame_t *f);
static int cpi_frame_op_find_request_port_ack_handler(cpi_endpoint_t *ep, cpi_frame_t *f);
static int cpi_frame_op_find_service_send_frame(cpi_rbtree_node_t *node, void *rn_data, void *udata);
static int cpi_frame_op_stop_find_service_handler(cpi_endpoint_t *ep, cpi_frame_t *f);
static int cpi_frame_op_stop_find_service_ack_handler(cpi_endpoint_t *ep, cpi_frame_t *f);
static int cpi_frame_op_stop_find_service_notice_handler(cpi_endpoint_t *ep, cpi_frame_t *f);
static int cpi_frame_op_subscribe_handler(cpi_endpoint_t *ep, cpi_frame_t *f);
static int cpi_frame_op_subscribe_ack_handler(cpi_endpoint_t *ep, cpi_frame_t *f);
static int cpi_frame_op_unsubscribe_handler(cpi_endpoint_t *ep, cpi_frame_t *f);
static int cpi_frame_notice_service_clean(cpi_service_t *svc, cpi_frame_t *f);
static int cpi_frame_op_send_handler(cpi_endpoint_t *ep, cpi_frame_t *f);

struct cpi_frame_scan_s {
    cpi_frame_t    *f;
    cpi_endpoint_t *ep;
};

static cpi_frame_op_cb_t cbs[] = {
    cpi_frame_op_peer_id_handler,
    cpi_frame_op_offer_service_handler,
    cpi_frame_op_offer_service_ack_handler,
    cpi_frame_op_offer_service_nack_handler,
    cpi_frame_op_stop_service_handler,
    cpi_frame_op_stop_service_ack_handler,
    cpi_frame_op_stop_service_nack_handler,
    cpi_frame_op_find_service_handler,
    cpi_frame_op_find_service_ack_handler,
    cpi_frame_op_find_request_port_handler,
    cpi_frame_op_find_request_port_ack_handler,
    cpi_frame_op_stop_find_service_handler,
    cpi_frame_op_stop_find_service_ack_handler,
    cpi_frame_op_stop_find_service_notice_handler,
    cpi_frame_op_subscribe_handler,
    cpi_frame_op_subscribe_ack_handler,
    cpi_frame_op_unsubscribe_handler,
    cpi_frame_op_send_handler,
    NULL,
};

int cpi_frame_callback(cpi_endpoint_t *ep, cpi_frame_t *f)
{
    log("---LOG--- [%d]\n", f->op);
    int r = RET(CPI_ESUCC);

    if (f->op >= cpi_frame_op_max || f->op < cpi_frame_op_peer_id)
        r = RET(CPI_EINVA);
    else if (cbs[f->op] != NULL)
        r = cbs[f->op](ep, f);
    return r;
}

static int cpi_frame_op_peer_id_handler(cpi_endpoint_t *ep, cpi_frame_t *f)
{
    log("---LOG---\n");
    int r = RET(CPI_ESUCC);
    cpi_app_t *app = (cpi_app_t *)cpi_endpoint_get_owner(ep);
    cpi_endpoint_t *e;

    e = cpi_endpoint_search_by_id(app->endpoints_id, f->src.appid);
    if (e != NULL) {
        goto out;
    }

    cpi_endpoint_set_peerid(ep, f->src.appid);

    r = cpi_endpoint_add(app->endpoints_id, ep);

out:
    return r;
}

static int cpi_frame_op_offer_service_handler(cpi_endpoint_t *ep, cpi_frame_t *f)
{
    log("---LOG---\n");
    int r = RET(CPI_ESUCC);
    cpi_service_t *svc;
    cpi_frame_t fr;
    cpi_buf_t *b;
    cpi_u8ptr_t pdata;
    cpi_u32_t len;
    cpi_app_t *app = (cpi_app_t *)cpi_endpoint_get_owner(ep);
    cpi_rbtree_node_t *rn;

    cpi_frame_init(&fr);

    fr.dest = f->src;

    if (cpi_app_iam_offer_cb_get(app) != NULL) {
        r = cpi_app_iam_offer_cb_get(app)(app, cpi_app_get_data(app), f->src.svc, f->src.inst, f->src.major, f->src.minor, cpi_endpoint_get_cred(ep));
        if (r != 0) {
            r = RET(CPI_EPERM);
            fr.op = cpi_frame_op_offer_service_nack;
            goto out;
        }
    }

    if (f->src.appid != cpi_endpoint_get_peerid(ep) || !cpi_app_is_discovery(app)) {
        fr.op = cpi_frame_op_offer_service_nack;
        goto out;
    }

    svc = cpi_service_search(app->services, f->src.svc, f->src.inst);
    if (svc == NULL) {
        if ((svc = cpi_service_new(app,
                                   cpi_app_evloop_get(app),
                                   cpi_endpoint_get_peerid(ep),
                                   f->src.inst,
                                   f->src.svc,
                                   f->src.major,
                                   f->src.minor,
                                   svc_holder,
                                   f->src.shm_path,
                                   f->src.seg_size,
                                   f->src.seg_count,
                                   0)) == NULL)
        {
            fr.op = cpi_frame_op_offer_service_nack;
            goto out;
        }

        if ((rn = cpi_rbtree_node_new(app->services, svc)) == NULL) {
            cpi_service_free(svc);
            fr.op = cpi_frame_op_offer_service_nack;
            goto out;
        }
        cpi_rbtree_insert(rn);
    }

    cpi_service_set_addr(svc, f->src.addr);

    if (nai_evnode_set_timeout(cpi_service_get_node(svc), NAI_TIMEOP_SET, 100) < 0) {
        fr.op = cpi_frame_op_offer_service_nack;
        goto out;
    }

    fr.op = cpi_frame_op_offer_service_ack;

out:
    if ((b = cpi_buf_new()) == NULL) {
        r = RET(CPI_ENMEM);
        goto err;
    }
    pdata = cpi_buf_get_data(b);
    len = cpi_buf_get_avail_size(b);
    if (sizeof(fr) > len) {
        r = RET(CPI_EBLEN);
        cpi_buf_free(b);
        goto err;
    }
    memcpy(pdata, &fr, sizeof(fr));
    cpi_buf_inc_len(b, sizeof(fr));
    cpi_endpoint_buf_append(ep, b, CPI_ENDPOINT_SEND);

    r = ep->ops.send(ep);

err:
    return r;
}

static int cpi_frame_op_offer_service_ack_handler(cpi_endpoint_t *ep, cpi_frame_t *f)
{
    log("---LOG---\n");
    cpi_exec_t *e = f->dest.e;
    cpi_service_t *svc;
    cpi_app_t *app;

    if (e == NULL)
        goto out;

    app = cpi_exec_get_app(e);

    svc = cpi_service_search(app->services, f->dest.svc, f->dest.inst);
    if (svc == NULL) {
        e->error = CPI_ENSVC;
    } else {
        cpi_service_online(svc);
        e->error = CPI_ESUCC;
    }
    cpi_exec_no_transfer(e, ep);
    cpi_exec_feedback(e);

out:

    return RET(CPI_ESUCC);
}

static int cpi_frame_op_offer_service_nack_handler(cpi_endpoint_t *ep, cpi_frame_t *f)
{
    log("---LOG---\n");
    cpi_exec_t *e = f->dest.e;

    if (e != NULL) {
        e->error = CPI_ESOPR;
        cpi_exec_no_transfer(e, ep);
        cpi_exec_feedback(e);
    }

    return RET(CPI_ESUCC);
}

static int cpi_frame_op_stop_service_handler(cpi_endpoint_t *ep, cpi_frame_t *f)
{
    log("---LOG---\n");
    int r = RET(CPI_ESUCC);
    cpi_frame_t fr;
    cpi_buf_t *b;
    cpi_u8ptr_t pdata;
    cpi_u32_t len;
    cpi_app_t *app = (cpi_app_t *)cpi_endpoint_get_owner(ep);

    cpi_frame_init(&fr);

    fr.dest = f->src;

    if (f->src.appid != cpi_endpoint_get_peerid(ep) || !cpi_app_is_discovery(app)) {
        fr.op = cpi_frame_op_stop_service_nack;
        goto out;
    }

    if (cpi_service_remove(app->services, f->src.svc, f->src.inst) != CPI_ESUCC) {
        fr.op = cpi_frame_op_stop_service_nack;
        goto out;
    }

    fr.op = cpi_frame_op_stop_service_ack;

out:
    if ((b = cpi_buf_new()) == NULL) {
        r = RET(CPI_ENMEM);
        goto err;
    }
    pdata = cpi_buf_get_data(b);
    len = cpi_buf_get_avail_size(b);
    if (sizeof(fr) > len) {
        r = RET(CPI_EBLEN);
        cpi_buf_free(b);
        goto err;
    }
    memcpy(pdata, &fr, sizeof(fr));
    cpi_buf_inc_len(b, sizeof(fr));
    cpi_endpoint_buf_append(ep, b, CPI_ENDPOINT_SEND);

    r = ep->ops.send(ep);

err:
    return r;
}

static int cpi_frame_op_stop_service_ack_handler(cpi_endpoint_t *ep, cpi_frame_t *f)
{
    log("---LOG---\n");
    cpi_exec_t *e = f->dest.e;
    cpi_service_t *svc;
    cpi_app_t *app = cpi_exec_get_app(e);

    svc = cpi_service_search(app->services, f->dest.svc, f->dest.inst);
    if (svc == NULL) {
        e->error = CPI_ENSVC;
    } else {
        cpi_service_offline(svc);
        e->error = CPI_ESUCC;
    }
    cpi_exec_no_transfer(e, ep);
    cpi_exec_feedback(e);

    return RET(CPI_ESUCC);
}

static int cpi_frame_op_stop_service_nack_handler(cpi_endpoint_t *ep, cpi_frame_t *f)
{
    log("---LOG---\n");
    cpi_exec_t *e = f->dest.e;

    e->error = CPI_ESOPR;
    cpi_exec_no_transfer(e, ep);
    cpi_exec_feedback(e);

    return RET(CPI_ESUCC);
}

static int cpi_frame_op_find_service_handler(cpi_endpoint_t *ep, cpi_frame_t *f)
{
    log("---LOG---\n");
    int r = RET(CPI_ESUCC);
    struct cpi_frame_scan_s fs;
    cpi_app_t *app = (cpi_app_t *)cpi_endpoint_get_owner(ep);

    if (f->src.appid != ep->peerid || !cpi_app_is_discovery(app)) {
        r = RET(CPI_EPERM);
        goto out;
    }

    if (cpi_app_iam_find_cb_get(app) != NULL) {
        r = cpi_app_iam_find_cb_get(app)(app, cpi_app_get_data(app), f->dest.svc, f->dest.inst, f->dest.major, f->dest.minor, cpi_endpoint_get_cred(ep));
        if (r != 0) {
            r = RET(CPI_EPERM);
            goto out;
        }
    }

    r = cpi_app_find_svc_register(app, f->src.appid, f->dest.svc, f->dest.inst, f->dest.major, f->dest.minor);
    if (CODE(r) != CPI_ESUCC)
        goto out;


    fs.f = f;
    fs.ep = ep;
    r = cpi_rbtree_scan_all(app->services, cpi_frame_op_find_service_send_frame, &fs);
    if (CODE(r) != CPI_ESUCC)
        goto out;

    r = ep->ops.send(ep);

out:
    return r;
}

static int cpi_frame_op_find_service_send_frame(cpi_rbtree_node_t *node, void *rn_data, void *udata)
{
    log("---LOG---\n");
    int r = RET(CPI_ESUCC);
    cpi_frame_t fr;
    cpi_u8ptr_t pdata;
    cpi_buf_t *b;
    cpi_u32_t len;
    cpi_service_t *svc = (cpi_service_t *)rn_data;
    struct cpi_frame_scan_s *fs = (struct cpi_frame_scan_s *)udata;
    cpi_frame_t *f = fs->f;

    (void)node;

    cpi_frame_init(&fr);

    if ((f->dest.svc != CPI_SERVICE_ANY && f->dest.svc != cpi_service_get_serviceid(svc)) ||
        (f->dest.inst != CPI_INSTANCE_ANY && f->dest.inst != cpi_service_get_instanceid(svc)) ||
        (f->dest.major != CPI_MAJOR_ANY && f->dest.major != cpi_service_get_major(svc)) ||
        (f->dest.minor != CPI_MINOR_ANY && f->dest.minor != cpi_service_get_minor(svc)))
    {
        goto out;
    }

    fr.op = cpi_frame_op_find_service_ack;
    fr.src.code = cpi_service_get_status(svc) == svc_online? CPI_AVAIL_ONLINED: CPI_AVAIL_OFFLINED;
    fr.src.svc = cpi_service_get_serviceid(svc);
    fr.src.inst = cpi_service_get_instanceid(svc);
    fr.src.major = cpi_service_get_major(svc);
    fr.src.minor = cpi_service_get_minor(svc);
    memcpy(fr.src.shm_path, cpi_service_get_shm_path(svc), CPI_SHM_PATH_LEN);
    fr.src.seg_size = cpi_service_get_seg_size(svc);
    fr.src.seg_count = cpi_service_get_seg_count(svc);
    fr.dest = f->src;
    fr.dest.inst = f->dest.inst;
    fr.src.addr = cpi_service_get_addr(svc);
    fr.src.appid = cpi_service_get_appid(svc);

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
    cpi_endpoint_buf_append(fs->ep, b, CPI_ENDPOINT_SEND);

out:
    return r;
}

static int cpi_frame_op_find_service_ack_handler(cpi_endpoint_t *ep, cpi_frame_t *f)
{
    log("---LOG---\n");
    int r = RET(CPI_ESUCC);
    cpi_app_t *app = cpi_endpoint_get_owner(ep);
    cpi_service_t *svc;
    cpi_rbtree_node_t *rn;
    cpi_endpoint_t *pep;

    svc = cpi_service_search(app->services, f->src.svc, f->src.inst);
    if (svc == NULL) {
        if ((svc = cpi_service_new(app,
                                   cpi_app_evloop_get(app),
                                   f->src.appid,
                                   f->src.inst,
                                   f->src.svc,
                                   f->src.major,
                                   f->src.minor,
                                   svc_consumer,
                                   f->src.shm_path,
                                   f->src.seg_size,
                                   f->src.seg_count,
                                   0)) == NULL)
        {
            r = RET(CPI_ENMEM);
            goto out;
        }
        cpi_service_set_status(svc, f->src.code);
        cpi_service_set_addr(svc, f->src.addr);

        if ((rn = cpi_rbtree_node_new(app->services, svc)) == NULL) {
            r = RET(CPI_ENMEM);
            cpi_service_free(svc);
            goto out;
        }
        cpi_rbtree_insert(rn);
    } else {
        if (cpi_service_get_role(svc) != svc_provider) {
            if (f->src.code == CPI_AVAIL_ONLINED && cpi_service_consumer_comif_create(svc) < 0) {
                r = RET(CPI_ECMIF);
                goto out;
            }
            cpi_service_set_ids(svc, f->src.appid, f->src.inst, f->src.svc, f->src.major, f->src.minor);
            cpi_service_set_status(svc, f->src.code);
            cpi_service_set_addr(svc, f->src.addr);
        }
    }

    if (f->src.code == CPI_AVAIL_ONLINED && cpi_service_get_role(svc) == svc_consumer) {
        npc_comif_state(cpi_service_get_comif(svc), NPC_STATE_OPEN);
        if ((pep = cpi_endpoint_search_by_id(app->endpoints_id, cpi_service_get_appid(svc))) == NULL) {
            pep = cpi_app_connect_service(svc);
            if (pep == NULL) {
                r = RET(CPI_ECONN);
                goto out;
            }
        }
        r = cpi_service_request_port(svc, pep);
        if (CODE(r) != CPI_ESUCC)
            goto out;
        r = cpi_service_auto_subscribe(svc, pep);
        if (CODE(r) != CPI_ESUCC)
            goto out;
    }

    if (f->dest.inst == CPI_INSTANCE_ANY) {
        if (cpi_app_get_avail_cb(app) != NULL) {
            /* the cb is commonly for passing service status down to app, and does not expect feedback from app */
            cpi_app_get_avail_cb(app)(app, cpi_app_get_data(app), f->src.code, f->src.svc, f->src.inst, f->src.major, f->src.minor);
            r = 0;
        }
    }

out:
    if (f->dest.e != NULL) {
        f->dest.e->error = r;
        cpi_exec_no_transfer(f->dest.e, ep);
        cpi_exec_feedback(f->dest.e);
    }

    return r;
}

static int cpi_frame_op_find_request_port_handler(cpi_endpoint_t *ep, cpi_frame_t *f)
{
    log("---LOG---\n");
    int r = RET(CPI_ESUCC);
    cpi_app_t *app = cpi_endpoint_get_owner(ep);
    cpi_frame_t fr;
    cpi_buf_t *b;
    cpi_u32_t len;
    cpi_u8ptr_t pdata;
    cpi_service_t *svc;
    cpi_service_peer_t *sp;

    cpi_frame_init(&fr);

    if (cpi_app_is_discovery(app)) {
        r = RET(CPI_EPERM);
        goto out;
    }

    svc = cpi_service_search(app->services, f->dest.svc, f->dest.inst);
    if (svc == NULL) {
        r = RET(CPI_ENSVC);
        goto out;
    }
    if (cpi_service_get_role(svc) != svc_provider) {
        r = RET(CPI_EPERM);
        goto out;
    }

    if ((b = cpi_buf_new()) == NULL) {
        r = RET(CPI_ENMEM);
        goto out;
    }
    pdata = cpi_buf_get_data(b);
    len = cpi_buf_get_avail_size(b);
    if (sizeof(fr) > len) {
        cpi_buf_free(b);
        r = RET(CPI_EBLEN);
        goto out;
    }

    r = cpi_service_peer_add(svc, f->src.appid);
    if (CODE(r) != CPI_ESUCC) {
        cpi_buf_free(b);
        goto out;
    }

    sp = cpi_service_peer_search(svc, f->src.appid);

    fr.op = cpi_frame_op_find_request_port_ack;
    fr.src.appid = cpi_app_get_id(app);
    fr.src.svc = cpi_service_get_serviceid(svc);
    fr.src.inst = cpi_service_get_instanceid(svc);
    fr.src.major = cpi_service_get_major(svc);
    fr.src.minor = cpi_service_get_minor(svc);
    fr.src.read = cpi_service_get_port_ref(sp, read);
    fr.src.write = cpi_service_get_port_ref(sp, write);
    fr.dest = f->src;

    memcpy(pdata, &fr, sizeof(fr));
    cpi_buf_inc_len(b, sizeof(fr));
    cpi_endpoint_buf_append(ep, b, CPI_ENDPOINT_SEND);
    r = ep->ops.send(ep);
    if (CODE(r) != CPI_ESUCC) {
        goto out;
    }

out:
    return r;
}

static int cpi_frame_op_find_request_port_ack_handler(cpi_endpoint_t *ep, cpi_frame_t *f)
{
    log("---LOG---\n");
    int r = RET(CPI_ESUCC);
    cpi_app_t *app = cpi_endpoint_get_owner(ep);
    cpi_service_t *svc;

    if (cpi_app_is_discovery(app)) {
        r = RET(CPI_EPERM);
        goto out;
    }

    svc = cpi_service_search(app->services, f->src.svc, f->src.inst);
    if (svc == NULL) {
        r = RET(CPI_ENSVC);
        goto out;
    }

    if (cpi_service_get_port(svc, read_addr) != f->src.write || cpi_service_get_port(svc, write_addr) != f->src.read) {
        r = cpi_service_port_attach(svc, f->src.read, f->src.write);
        if (CODE(r) != CPI_ESUCC) {
            goto out;
        }
    }

out:
    return r;
}

static int cpi_frame_op_stop_find_service_handler(cpi_endpoint_t *ep, cpi_frame_t *f)
{
    int r = RET(CPI_ESUCC);
    cpi_app_t *app = (cpi_app_t *)cpi_endpoint_get_owner(ep);
    cpi_frame_t fr;
    cpi_u8ptr_t pdata;
    cpi_buf_t *b;
    cpi_u32_t len;
    cpi_service_t *svc;
    log("---LOG---nr_node:%lu serv:%u inst:%u\n", app->services->nr_node, f->dest.svc, f->dest.inst);

    cpi_frame_init(&fr);

    if (f->src.appid != ep->peerid || !cpi_app_is_discovery(app)) {
        r = RET(CPI_EPERM);
        goto out;
    }

    svc = cpi_service_search(app->services, f->dest.svc, f->dest.inst);

    cpi_app_find_svc_unregister(app, f->src.appid, f->dest.svc, f->dest.inst, f->dest.major, f->dest.minor);

    fr.src = f->dest;
    fr.dest = f->src;
    fr.op = cpi_frame_op_stop_find_service_ack;
    fr.src.code = r;

    b = cpi_buf_new();
    if (b == NULL) {
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
    if (CODE(r) != CPI_ESUCC) {
        goto out;
    }

    if (f->dest.inst != CPI_INSTANCE_ANY && svc != NULL)
        r = cpi_frame_notice_service_clean(svc, f);

out:
    return r;
}

static int cpi_frame_notice_service_clean(cpi_service_t *svc, cpi_frame_t *f)
{
    int r = RET(CPI_ESUCC);
    cpi_frame_t fr;
    cpi_u8ptr_t pdata;
    cpi_buf_t *b;
    cpi_u32_t len;
    cpi_endpoint_t *ep;
    cpi_app_t *app = cpi_service_get_app(svc);

    ep = cpi_endpoint_search_by_id(app->endpoints_id, cpi_service_get_appid(svc));
    if (ep == NULL) {
        r = RET(CPI_ENENP);
        goto out;
    }

    cpi_frame_init(&fr);

    fr.src.appid = f->src.appid;
    fr.dest.svc = cpi_service_get_serviceid(svc);
    fr.dest.inst = cpi_service_get_instanceid(svc);
    fr.dest.major = cpi_service_get_major(svc);
    fr.dest.minor = cpi_service_get_minor(svc);
    fr.op = cpi_frame_op_stop_find_service_notice;

    b = cpi_buf_new();
    if (b == NULL) {
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

static int cpi_frame_op_stop_find_service_notice_handler(cpi_endpoint_t *ep, cpi_frame_t *f)
{
    log("---LOG---\n");
    int r = RET(CPI_ESUCC);
    cpi_service_t *svc;
    cpi_app_t *app = (cpi_app_t *)cpi_endpoint_get_owner(ep);

    svc = cpi_service_search(app->services, f->dest.svc, f->dest.inst);
    if (svc == NULL) {
        r = RET(CPI_ENSVC);
        goto out;
    }

    cpi_service_peer_remove(svc, f->src.appid);

out:
    return r;
}

static int cpi_frame_op_stop_find_service_ack_handler(cpi_endpoint_t *ep, cpi_frame_t *f)
{
    log("---LOG---\n");
    int r = f->src.code;
    cpi_exec_t *e = f->dest.e;

    if (e != NULL) {
        cpi_exec_no_transfer(e, ep);
        cpi_exec_feedback(e);
    }

    return r;
}

static int cpi_frame_op_subscribe_handler(cpi_endpoint_t *ep, cpi_frame_t *f)
{
    log("---LOG---\n");
    int rc, r = RET(CPI_ESUCC);
    cpi_app_t *app = cpi_endpoint_get_owner(ep);

    if (cpi_app_iam_subscribe_cb_get(app) != NULL) {
        rc = cpi_app_iam_subscribe_cb_get(app)(app, cpi_app_get_data(app), f->src.appid, f->dest.svc, f->dest.inst, f->dest.gid, f->dest.eid, cpi_endpoint_get_cred(ep));
        if (rc != 0) {
            r = RET(CPI_EPERM);
            goto out;
        }
    }

    if (cpi_app_get_sub_cb(app) != NULL) {
        r = cpi_app_get_sub_cb(app)(app, cpi_app_get_data(app), f->src.appid, f->dest.svc, f->dest.inst, f->dest.gid, f->dest.eid, cpi_endpoint_get_cred(ep));
        if (r != 0) {
            r = RET(CPI_ECALL);
            goto out;
        }
    }

out:
    return r;
}

static int cpi_frame_op_subscribe_ack_handler(cpi_endpoint_t *ep, cpi_frame_t *f)
{
    log("---LOG---\n");
    int r = RET(CPI_ESUCC);
    cpi_app_t *app = cpi_endpoint_get_owner(ep);
    cpi_service_t *svc;

    if (cpi_app_get_id(app) != f->dest.appid ||
        cpi_endpoint_get_peerid(ep) != f->src.appid)
    {
        r = RET(CPI_EPERM);
        goto out;
    }

    svc = cpi_service_search(app->services, f->src.svc, f->src.inst);
    if (svc == NULL) {
        r = RET(CPI_ENSVC);
        goto out;
    }

    r = cpi_service_subscription_set(svc, cpi_app_get_id(app), f->src.gid, f->src.eid);
    if (CODE(r) != CPI_ESUCC) {
        goto out;
    }

    if (cpi_app_get_suback_cb(app) != NULL) {
        r = cpi_app_get_suback_cb(app)(app, cpi_app_get_data(app), f->src.code, f->src.svc, f->src.inst, f->src.gid, f->src.eid);
        if (r != 0) {
            r = RET(CPI_ECALL);
            goto out;
        }
    }

out:
    return r;
}

static int cpi_frame_op_unsubscribe_handler(cpi_endpoint_t *ep, cpi_frame_t *f)
{
    log("---LOG---\n");
    int r = RET(CPI_ESUCC);
    cpi_app_t *app = cpi_endpoint_get_owner(ep);
    cpi_service_t *svc;

    if (cpi_app_get_id(app) != f->dest.appid ||
        cpi_endpoint_get_peerid(ep) != f->src.appid)
    {
        r = RET(CPI_EPERM);
        goto out;
    }

    svc = cpi_service_search(app->services, f->dest.svc, f->dest.inst);
    if (svc == NULL) {
        r = RET(CPI_ENSVC);
        goto out;
    }

    r = cpi_service_subscription_remove(svc, cpi_endpoint_get_peerid(ep), f->dest.gid, f->dest.eid);

out:
    return r;
}

static int cpi_frame_op_send_handler(cpi_endpoint_t *ep, cpi_frame_t *f)
{
    log("---LOG---\n");
    int r = RET(CPI_ESUCC);
    cpi_app_t *app = cpi_endpoint_get_owner(ep);
    cpi_service_t *svc;

    if (cpi_app_get_id(app) != f->dest.appid ||
        cpi_endpoint_get_peerid(ep) != f->src.appid)
    {
        r = RET(CPI_EPERM);
        goto out;
    }

    svc = cpi_service_search(app->services, f->src.svc, f->src.inst);
    if (svc == NULL) {
        r = RET(CPI_ENSVC);
        goto out;
    }

    log("---LOG--- svc->read:%p\n", svc->read);
    if (npc_comif_event(cpi_service_get_comif(svc), f->dest.event) < 0) {
        r = RET(CPI_ECMEV);
        goto out;
    }
out:
    return r;
}

