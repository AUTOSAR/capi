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
/// @file       cpi_app_ep_cb.c
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
#include "nai/io/nai_io.h"
#include "nai/runtime/nai_errno.h"
#include "npc/cpi/cpi_app.h"
#include "npc/cpi/cpi_string.h"
#include "npc/cpi/cpi_error.h"
#include "npc/cpi/cpi_conf.h"
#include "npc/cpi/cpi_buf.h"
#include "npc/cpi/cpi_frame.h"
#include "npc/cpi/cpi_service.h"

static int cpi_ep_get_handle(const cpi_endpoint_t *ep);
static int cpi_listen_ep_open(cpi_endpoint_t *ep);
static int cpi_listen_ep_recv(cpi_endpoint_t *ep);
static int cpi_ep_close(cpi_endpoint_t *ep);
static int cpi_listen_ep_listen(cpi_endpoint_t *ep);
static int cpi_connect_ep_connect(cpi_endpoint_t *ep);
static int cpi_ep_send(cpi_endpoint_t *ep);
static int cpi_ep_recv(cpi_endpoint_t *ep);
static int cpi_ep_error(cpi_endpoint_t *ep);
static int cpi_app_accept_ep_cb(cpi_endpoint_t *ep, cpi_endpoint_cb_event_t event);
static int cpi_app_ep_service_offline(cpi_rbtree_node_t *node, void *rn_data, void *udata);
static int cpi_app_ep_service_disconnect(cpi_rbtree_node_t *node, void *rn_data, void *udata);
static int cpi_app_ep_service_port_clean(cpi_rbtree_node_t *node, void *rn_data, void *udata);

static cpi_endpoint_net_ops_t accept_ep_ops = {
    cpi_ep_get_handle,
    NULL,
    cpi_ep_close,
    NULL,
    NULL,
    cpi_ep_send,
    cpi_ep_recv,
    cpi_ep_error,
};
static cpi_endpoint_net_ops_t listen_ep_ops = {
    cpi_ep_get_handle,
    cpi_listen_ep_open,
    cpi_ep_close,
    cpi_listen_ep_listen,
    NULL,
    NULL,
    cpi_listen_ep_recv,
    cpi_ep_error,
};
static cpi_endpoint_net_ops_t connect_ep_ops = {
    cpi_ep_get_handle,
    NULL,
    cpi_ep_close,
    NULL,
    cpi_connect_ep_connect,
    cpi_ep_send,
    cpi_ep_recv,
    cpi_ep_error,
};
static cpi_string_t cpi_app_conf_host = cpi_string("host");
static cpi_string_t cpi_app_conf_svc_net_type = cpi_string("svc_net_type");
static cpi_string_t cpi_app_conf_svc_access_handle = cpi_string("svc_access_handle");


cpi_endpoint_net_ops_t *cpi_app_ep_get_listen_ops(void)
{
    return &listen_ep_ops;
}

cpi_endpoint_net_ops_t *cpi_app_ep_get_connect_ops(void)
{
    return &connect_ep_ops;
}

static int cpi_ep_get_handle(const cpi_endpoint_t *ep)
{
    return nai_iobase_get_fd(&ep->iobase);
}

static int cpi_ep_send(cpi_endpoint_t *ep)
{
    int r = RET(CPI_ESUCC), n, size, sent = 0;
    cpi_buf_t *b;
    cpi_rbtree_node_t *rn;
    cpi_app_t *app = (cpi_app_t *)cpi_endpoint_get_owner(ep);

    if (cpi_endpoint_get_status(ep) == cpi_ep_connect) {
        if (app->discovery != ep) {
            rn = cpi_rbtree_node_new(app->endpoints, ep);
            if (rn == NULL) {
                r = RET(CPI_ENMEM);
                goto out;
            }
            cpi_rbtree_insert(rn);
        }
        cpi_endpoint_set_status(ep, cpi_ep_connect_established);
        if (cpi_endpoint_get_cb(ep) != NULL)
            cpi_endpoint_get_cb(ep)(ep, cpi_ep_ev_establish);
        r = cpi_app_ep_connect_peerid_send(ep);
        if (CODE(r) != CPI_ESUCC) {
            goto out;
        }
    }
    if (cpi_endpoint_get_status(ep) != cpi_ep_connect_established &&
        cpi_endpoint_get_status(ep) != cpi_ep_accept_established)
    {
        r = RET(CPI_ECONS);
        goto out;
    }

    nai_iobase_set_mode(cpi_endpoint_get_iobase(ep), NAI_IO_READWRITE);

again:
    if (ep->sending != NULL) {
        b = ep->sending;
    } else {
        b = ep->sending = cpi_endpoint_snd_pop(ep);
    }

    if (b != NULL) {
        size = cpi_buf_get_left_size(b);
        n = nai_iobase_write(cpi_endpoint_get_iobase(ep), cpi_buf_get_pos(b), size);
        if (n >= 0) {
            sent = 1;
            if (n == size) {
                if (ep->sending != NULL) {
                    cpi_buf_free(b);
                    b = ep->sending = NULL;
                }
            } else {
                cpi_buf_inc_pos(b, n);
                cpi_buf_dec_len(b, n);
            }
            goto again;
        } else {
            if (nai_errno == NAI_EAGAIN || nai_errno == NAI_EINPROGRESS) {
                nai_iobase_set_mode(cpi_endpoint_get_iobase(ep), NAI_IO_READWRITE);
            } else {
                r = RET(CPI_ESEND);
                goto out;
            }
        }
    }

    if (b == NULL) {
        nai_iobase_set_mode(cpi_endpoint_get_iobase(ep), NAI_IO_READ);
    }
    if (sent && cpi_endpoint_get_cb(ep) != NULL)
        cpi_endpoint_get_cb(ep)(ep, cpi_ep_ev_send);
out:
    if (CODE(r) != CPI_ESUCC)
        cpi_endpoint_set_positive(ep);
    return r;
}

int cpi_app_ep_connect_peerid_send(cpi_endpoint_t *ep)
{
    int r = RET(CPI_ESUCC);
    cpi_buf_t *b;
    cpi_frame_t f;
    cpi_u32_t len;
    cpi_u8ptr_t pdata;

    if (cpi_endpoint_get_handshake(ep))
        goto out;

    cpi_frame_init(&f);

    f.src.appid = ((cpi_app_t *)cpi_endpoint_get_owner(ep))->id;
    f.op = cpi_frame_op_peer_id;

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

    cpi_endpoint_set_handshake(ep);
    cpi_endpoint_buf_append(ep, b, CPI_ENDPOINT_SEND);

out:
    return r;
}

static int cpi_ep_recv(cpi_endpoint_t *ep)
{
    int r = RET(CPI_ESUCC), n;
    cpi_buf_t *b;

    if (cpi_endpoint_get_status(ep) != cpi_ep_connect_established &&
        cpi_endpoint_get_status(ep) != cpi_ep_accept_established)
    {
        r = RET(CPI_ECONS);
        cpi_endpoint_set_positive(ep);
        goto out;
    }

    while (1) {
        b = cpi_buf_new();
        if (b == NULL) {
            r = RET(CPI_ENMEM);
            cpi_endpoint_set_positive(ep);
            goto out;
        }
        n = nai_iobase_read(cpi_endpoint_get_iobase(ep), cpi_buf_get_data(b), CPI_BUF_SIZE);
        if (n < 0) {
            cpi_buf_free(b);
            if (nai_errno == NAI_EAGAIN || nai_errno == NAI_EINPROGRESS) {
                break;
            }
            r = RET(CPI_ERECV);
            cpi_endpoint_set_negative(ep);
            goto out;
        } else if (n == 0) {
            cpi_buf_free(b);
            break;
        } else {
            cpi_buf_inc_len(b, n);
            cpi_endpoint_buf_append(ep, b, CPI_ENDPOINT_RECV);
        }
    }

    if (cpi_endpoint_get_cb(ep) != NULL) {
        if (CODE(r = cpi_endpoint_get_cb(ep)(ep, cpi_ep_ev_recv)) != CPI_ESUCC) {
            cpi_endpoint_set_positive(ep);
            goto out;
        }
    }
    if (n == 0) {
        cpi_endpoint_set_negative(ep);
        r = RET(CPI_ECLOS);
    }

out:
    return r;
}

static int cpi_ep_error(cpi_endpoint_t *ep)
{
    (void)ep;
    cpi_endpoint_set_negative(ep);
    return CPI_ECONN;
}

static int cpi_ep_close(cpi_endpoint_t *ep)
{
    cpi_app_t *app = (cpi_app_t *)cpi_endpoint_get_owner(ep);

    if (cpi_endpoint_get_cb(ep) != NULL)
        cpi_endpoint_get_cb(ep)(ep, cpi_ep_ev_destroy);

    cpi_endpoint_remove(app->endpoints_id, ep);

    if (cpi_endpoint_get_status(ep) != cpi_ep_connect_established &&
        cpi_endpoint_get_status(ep) != cpi_ep_accept_established &&
        cpi_endpoint_get_status(ep) != cpi_ep_listen)
    {
        cpi_endpoint_free(ep);
    } else {
        if (app->discovery == ep) {
            cpi_endpoint_free(ep);
            app->discovery = NULL;
            nai_evnode_set_timeout(cpi_app_get_dis_evnode(app), NAI_TIMEOP_SET, 100);
        } else {
            cpi_endpoint_remove(app->endpoints, ep);
        }
    }
    return RET(CPI_ESUCC);
}

/*
 * listen
 */
static int cpi_listen_ep_open(cpi_endpoint_t *ep)
{
    int n, r = RET(CPI_ESUCC);
    cpi_app_t *app = (cpi_app_t *)cpi_endpoint_get_owner(ep);
    cpi_conf_item_t *ci;
    cpi_string_t *host, *svc_net_type, *svc_access_handle;
    cpi_string_t type_unix = cpi_string("unixsock");
    cpi_string_t type_tcp = cpi_string("tcp");
    char access_path[1024] = {0};

    ci = cpi_conf_search(app->conf, &cpi_app_conf_host);
    if (ci == NULL || cpi_conf_get_type(ci) != cpi_conf_type_string) {
        r = RET(CPI_ECONF);
        goto out;
    }
    host = cpi_conf_get_string_val(ci);

    ci = cpi_conf_search(app->conf, &cpi_app_conf_svc_net_type);
    if (ci == NULL || cpi_conf_get_type(ci) != cpi_conf_type_string) {
        r = RET(CPI_ECONF);
        goto out;
    }
    svc_net_type = cpi_conf_get_string_val(ci);
    if (!svc_net_type->len) {
        r = RET(CPI_ECONF);
        goto out;
    }

    ci = cpi_conf_search(app->conf, &cpi_app_conf_svc_access_handle);
    if (ci == NULL || cpi_conf_get_type(ci) != cpi_conf_type_string) {
        r = RET(CPI_ECONF);
        goto out;
    }
    svc_access_handle = cpi_conf_get_string_val(ci);
    if (!svc_access_handle->len) {
        r = RET(CPI_ECONF);
        goto out;
    }

    if (!cpi_string_strcmp(svc_net_type, &type_unix)) {
        n = snprintf(access_path, sizeof(access_path)-1, "unix:%s/%s", host->data, svc_access_handle->data);
        n = -1;
        remove(access_path+5);
        cpi_endpoint_set_nettype(&app->svc_addr, CPI_ENDPOINT_ADDR_UNIX);
    } else if (!cpi_string_strcmp(svc_net_type, &type_tcp)) {
        n = snprintf(access_path, sizeof(access_path)-1, "%s:%s", host->data, svc_access_handle->data);
        cpi_endpoint_set_nettype(&app->svc_addr, CPI_ENDPOINT_ADDR_NET);
    } else {
        r = RET(CPI_ECONF);
        goto out;
    }
    cpi_endpoint_addr_pton(access_path, n, &app->svc_addr);
    ep->addr = app->svc_addr;

out:
    return r;
}

static int cpi_listen_ep_listen(cpi_endpoint_t *ep)
{
    int r = RET(CPI_ESUCC);
    cpi_app_t *app = (cpi_app_t *)cpi_endpoint_get_owner(ep);
    int rc;

    nai_server_set_opt(&ep->iobase, NAI_IO_REUSEADDR, 1);
    /* the option SO_REUSEPORT is invalid for unix domain socket, disable it */
    //nai_server_set_opt(&ep->iobase, NAI_IO_REUSEPORT, 1);
    rc = nai_server_bind(&ep->iobase, app->evloop, &(app->svc_addr.net_addr), app->svc_addr.addr_len);
    if (rc < 0) {
        r = RET(CPI_EBIND);
    } else {
        cpi_endpoint_set_status(ep, cpi_ep_listen);
    }
    return r;
}

static int cpi_listen_ep_recv(cpi_endpoint_t *ep)
{
    nai_fd_t f;
    cpi_endpoint_t *new_ep;
    cpi_rbtree_node_t *rn;
    cpi_app_t *app = (cpi_app_t *)cpi_endpoint_get_owner(ep);
    nai_sockaddr_t addr;
    int addrlen;

    while (1) {
        memset(&addr, 0, sizeof(addr));
        addrlen = sizeof(addr);

        f = nai_server_accept(cpi_endpoint_get_iobase(ep), &addr, &addrlen);
        if (f == NAI_FD_INVALID) {
            if (nai_errno == EAGAIN) {
                break;
            } else {
                goto out;
            }
        }

        if ((new_ep = cpi_endpoint_new(cpi_app_accept_ep_cb, &accept_ep_ops, app, (nai_iobase_cb_f)cpi_app_ep_iocb, NULL)) == NULL) {
            nai_sock_close(f);
            goto out;
        }

        if (nai_iobase_set_fd(cpi_endpoint_get_iobase(new_ep), f, NAI_FD_TYPE_SOCK) < 0) {
            cpi_endpoint_free(new_ep);
            goto out;
        }
        nai_stream_set_fdown(cpi_endpoint_get_iobase(new_ep), 1);

        if (nai_stream_open(cpi_endpoint_get_iobase(new_ep), app->evloop) < 0) {
            cpi_endpoint_free(new_ep);
            goto out;
        }

        if ((rn = cpi_rbtree_node_new(app->endpoints, new_ep)) == NULL) {
            cpi_endpoint_free(new_ep);
            goto out;
        }
        cpi_rbtree_insert(rn);

        cpi_endpoint_set_status(new_ep, cpi_ep_accept_established);

        nai_iobase_set_mode(cpi_endpoint_get_iobase(new_ep), NAI_IO_READ);

        if (cpi_endpoint_get_cb(new_ep) != NULL)
            cpi_endpoint_get_cb(new_ep)(new_ep, cpi_ep_ev_establish);

        if (cpi_app_ep_connect_peerid_send(new_ep) != CPI_ESUCC) {
            cpi_rbtree_delete(rn);
            cpi_rbtree_node_free(rn);
        }
        if (new_ep->ops.send(new_ep) != CPI_ESUCC) {
            cpi_rbtree_delete(rn);
            cpi_rbtree_node_free(rn);
        }
    }
out:
    return RET(CPI_ESUCC);
}

/*
 * connect
 */
static int cpi_connect_ep_connect(cpi_endpoint_t *ep)
{
    int rc, r = RET(CPI_ESUCC);
    cpi_app_t *app = (cpi_app_t *)cpi_endpoint_get_owner(ep);
    cpi_endpoint_addr_t addr;

    addr = cpi_endpoint_get_addr(ep);

    rc = nai_stream_connect(&ep->iobase, app->evloop, &addr.net_addr, addr.addr_len);
    if (rc < 0) {
        r = RET(CPI_ECONN);
        goto out;
    }
    cpi_endpoint_set_status(ep, cpi_ep_connect);

out:
    return r;
}

/*
 * endpoint cb
 */
int cpi_app_connect_ep_cb(cpi_endpoint_t *ep, cpi_endpoint_cb_event_t event)
{
    int r = RET(CPI_ESUCC);
    cpi_frame_t f;
    cpi_app_t *app = (cpi_app_t *)cpi_endpoint_get_owner(ep);

    cpi_frame_init(&f);

    if (event == cpi_ep_ev_recv) {
        while (CODE(cpi_endpoint_get_recv_buf(ep, &f, sizeof(f))) == CPI_ESUCC) {
            r = cpi_frame_callback(ep, &f);
            if (CODE(r) != CPI_ESUCC)
                goto out;
        }
    } else if (event == cpi_ep_ev_destroy && !cpi_app_is_discovery(app)) {
        cpi_rbtree_scan_all(app->services, cpi_app_ep_service_port_clean, &cpi_endpoint_get_peerid(ep));
    } else if (event == cpi_ep_ev_establish) {
        if (!cpi_endpoint_isset_cred(ep)) {
            cpi_endpoint_cred_get(ep);
        }
    } else {
        /* ignore rest events */
    }

out:
    return r;
}

static int cpi_app_accept_ep_cb(cpi_endpoint_t *ep, cpi_endpoint_cb_event_t event)
{
    int r = RET(CPI_ESUCC);
    cpi_frame_t f;
    cpi_app_t *app = (cpi_app_t *)cpi_endpoint_get_owner(ep);

    cpi_frame_init(&f);

    if (event == cpi_ep_ev_recv) {
        while (cpi_endpoint_get_recv_buf(ep, &f, sizeof(f)) == CPI_ESUCC) {
            r = cpi_frame_callback(ep, &f);
            if (CODE(r) != CPI_ESUCC)
                goto out;
        }
    } else if (event == cpi_ep_ev_destroy) {
        if (cpi_app_is_discovery(app)) {
            cpi_rbtree_scan_all(app->services, cpi_app_ep_service_offline, &cpi_endpoint_get_peerid(ep));
        } else {
            cpi_rbtree_scan_all(app->services, cpi_app_ep_service_disconnect, ep);
        }
    } else if (event == cpi_ep_ev_establish) {
        if (!cpi_endpoint_isset_cred(ep)) {
            cpi_endpoint_cred_get(ep);
        }
    } else {
        /* ignore rest events */
    }

out:
    return r;
}

static int cpi_app_ep_service_offline(cpi_rbtree_node_t *node, void *rn_data, void *udata)
{
    int r = RET(CPI_ESUCC);
    cpi_service_t *svc = (cpi_service_t *)rn_data;
    cpi_s32_t appid = *(cpi_s32_t *)udata;
    cpi_endpoint_addr_t addr;

    (void)node;

    if (cpi_service_get_appid(svc) != appid)
        goto out;

    cpi_service_offline(svc);

    addr = cpi_service_get_addr(svc);
    r = cpi_app_find_svc_notice(cpi_service_get_app(svc),
                                cpi_service_get_appid(svc),
                                cpi_service_get_serviceid(svc),
                                cpi_service_get_instanceid(svc),
                                cpi_service_get_major(svc),
                                cpi_service_get_minor(svc),
                                cpi_service_get_status(svc),
                                &addr,
                                cpi_service_get_shm_path(svc),
                                cpi_service_get_seg_size(svc),
                                cpi_service_get_seg_count(svc));

out:
    return r;
}

static int cpi_app_ep_service_disconnect(cpi_rbtree_node_t *node, void *rn_data, void *udata)
{
    int r = RET(CPI_ESUCC);
    cpi_service_t *svc = (cpi_service_t *)rn_data;
    cpi_service_peer_t *sp;
    cpi_endpoint_t *ep = (cpi_endpoint_t *)udata;
    cpi_s32_t appid = cpi_endpoint_get_peerid(ep);

    (void)node;

    sp = cpi_service_peer_search(svc, appid);
    if (sp != NULL) {
        cpi_service_peer_remove(svc, appid);
        if (!cpi_endpoint_is_positive(ep))
            npc_comif_clean(cpi_service_get_comif(svc), appid);
    }

    return r;
}

static int cpi_app_ep_service_port_clean(cpi_rbtree_node_t *node, void *rn_data, void *udata)
{
    int r = RET(CPI_ESUCC);
    cpi_service_t *svc = (cpi_service_t *)rn_data;
    cpi_s32_t appid = *(cpi_s32_t *)udata;

    (void)node;

    if (cpi_service_get_appid(svc) == appid) {
        cpi_service_consumer_port_free(svc);
        npc_comif_close(cpi_service_get_comif(svc));
        cpi_service_set_comif(svc, NULL);
    }

    return r;
}

