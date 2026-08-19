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
/// @file       icc_routing.c
/// @brief
/// @details
/// @date       2026-02-13
/// @author     jiusen.cui
/// @version    1.2.0
///
/// ================================================================

#include "icc/core/icc_app.h"
#include "icc_client.h"
#include "icc_log.h"
#include "icc_routing_impl.h"
#include "icc_service.h"
#include "nai/runtime/nai_errno.h"
#include "nai/runtime/nai_util.h"

typedef struct icc_routing_exec_s
{
    nai_list_entry_t ent;
    nai_cond_t* cond;
    icc_routing_exec_f cb;
    void* ctx;
    int stat;
    int errcode;
} icc_routing_exec_t;

typedef struct icc_routing_args_s
{
    nai_evloop_t* loop;
    int type;
} icc_routing_args_t;

//////////////////////////////////////////////////////////////////////////////
//

static int icc_routing_open_stub(icc_routing_t* p, void* ctx)
{
    int r;
    int ec;
    icc_routing_args_t* a;

    a = (icc_routing_args_t*)ctx;
    r = icc_routing_open(p, a->loop, a->type);
    if (r < 0) {
        ec = nai_errno;
        nai_evnode_close(&p->ev);
        p->loop   = 0;
        nai_errno = ec;
    };

    return r;
};

static int icc_routing_close_stub(icc_routing_t* p, void* ctx)
{
    int r;
    nai_rbnode_t* n;
    nai_list_entry_t* e;
    icc_service_t* s;
    icc_message_t* m;

    (void)ctx;

    r = nai_evnode_close(&p->ev);
    if (r < 0) {
        goto _end;
    };

    /* start closing */
    p->stat = ICC_RSTAT_CLOSING;

    /* free services
     * PRS_SOMEIPSD_00432
     */
    n = nai_rbtree_begin(&p->servs);
    for (; n != nai_rbtree_end(&p->servs);) {
        s = nai_containof(n, icc_service_t, entr);
        n = nai_rbtree_next(n);
        icc_service_close(s);
    };

    /* free messages */
    nai_list_add_tail(&p->mlist, &p->posted);
    e = p->mlist.next;
    for (; e != &p->mlist;) {
        m = (icc_message_t*)e;
        e = e->next;
        nai_list_entry_remove(&m->ent);
        icc_message_close(m);
    };

    /* close network */
    if (p->net) {
        icc_network_close(p->net);
        nai_free(p->net);
        p->net = 0;
    };

_end:
    return r;
};

static int icc_routing_exec_queue(icc_routing_t* p)
{
    int r;
    int empty = 0;
    icc_message_t* m;
    icc_routing_exec_t* h;
    nai_list_entry_t list;

    nai_mutex_lock(&p->lock);

    for (;;) {
        empty = 1;

        if (!nai_list_is_empty(&p->mlist)) {
            nai_list_init(&list);
            nai_list_add_tail(&list, &p->mlist);

            /* unlock for execute */
            nai_mutex_unlock(&p->lock);

            empty = 0;

            while (!nai_list_is_empty(&list)) {
                m = (icc_message_t*)list.next;
                nai_list_entry_remove(&m->ent);

                r = icc_routing_send_intern(p, m);
                if (r < 0) {
                    icc_message_close(m);
                };
            };

            /* unlock for execute */
            nai_mutex_lock(&p->lock);
        };

        /* start execute */
        while (!nai_list_is_empty(&p->elist)) {
            /* get an execution */
            h = (icc_routing_exec_t*)p->elist.next;
            nai_list_entry_remove(&h->ent);

            /* mark will execution */
            h->stat = 1;

            /* unlock for execute */
            nai_mutex_unlock(&p->lock);

            empty = 0;

            /* execute it */
            r = h->cb(p, h->ctx);
            if (r >= 0) {
                h->errcode = 0;
            } else {
                h->errcode = nai_errno;
            };

            /* notify compeleted */
            h->stat = 2;
            nai_cond_signal(h->cond);

            /* lock for next */
            nai_mutex_lock(&p->lock);
        };

        if (empty) {
            break;
        };
    };

    nai_mutex_unlock(&p->lock);
    r = 0;

    return r;
};

static int icc_routing_event(nai_evnode_t* e, int events)
{
    int r;
    icc_routing_t* p;

    p = nai_containof(e, icc_routing_t, ev);
    if (events & NAI_EV_NOTIFY) {
        switch (nai_ev_notify_code(events)) {
            case 0: /* async execution */
                icc_routing_exec_queue(p);
                break;
            default:
                break;
        };
    };

    r = 0;

    return r;
};

//////////////////////////////////////////////////////////////////////////////
//

extern icc_endpoint_cb_t icc_routing_endpoint_cb;

int icc_routing_init(icc_routing_t* p)
{
    int r;

    p->stat = ICC_RSTAT_CLOSED;
    p->net  = 0;
    p->conf = 0;
    nai_spin_init(&p->lock);
    nai_evnode_init(&p->ev);
    nai_rbtree_init(&p->servs);
    nai_rbtree_init(&p->clients);
    nai_list_init(&p->flist);
    nai_list_init(&p->elist);
    nai_list_init(&p->mlist);
    nai_list_init(&p->posted);
    r = 0;

    return r;
};

int icc_routing_set_conf(icc_routing_t* p, icc_conf_t* c)
{
    int r;

    if (p->stat != ICC_RSTAT_CLOSED) {
        nai_errno = EPERM;
        r         = -1;
        goto _end;
    };

    p->conf = c;
    r       = 0;

_end:
    return r;
};

int icc_routing_open(icc_routing_t* p, nai_evloop_t* loop, int type)
{
    int r;
    int ec;
    icc_routing_args_t a;

    (void)type;

    if (loop == 0) {
        nai_log_error(ICC_LOG_CORE, EINVAL, "must offer a valid evloop");
        nai_errno = EINVAL;
        r         = -1;
        goto _end;
    };

    if (p->stat > ICC_RSTAT_OPENING) {
        nai_log_error(ICC_LOG_CORE, EPERM, "the routing is already opened");
        nai_errno = EPERM;
        r         = -1;
        goto _end;
    };

    if (p->stat == ICC_RSTAT_CLOSED) {
        r = nai_mutex_open(&p->lock, 0);
        if (r < 0) {
            nai_log_alert(ICC_LOG_CORE, nai_errno, "create mutex failed");
            goto _end;
        };
    };

    if (nai_evloop_in_dispatch(loop) == 0) {
        nai_evnode_set_cb(&p->ev, icc_routing_event);
        r = nai_evnode_open(&p->ev, loop);
        if (r < 0) {
            nai_log_error(ICC_LOG_CORE, nai_errno, "open evnode failed");
            goto _fail;
        };

        p->stat = ICC_RSTAT_OPENING;
        p->loop = loop;
        a.loop  = loop;
        a.type  = type;
        r       = icc_routing_exec(p, icc_routing_open_stub, &a);
        if (r < 0) {
            ec = nai_errno;
            icc_routing_close(p);
            nai_errno = ec;
        };
        goto _end;
    };

    p->net = (icc_network_t*)nai_malloc(sizeof(*p->net));
    if (p->net == 0) {
        nai_log_alert(ICC_LOG_CORE, nai_errno, "allocate network failed");
        r = -1;
        goto _fail;
    };

    icc_network_init(p->net);
    icc_network_set_cb(p->net, &icc_routing_endpoint_cb);
    icc_network_set_routing(p->net, p);

    r = icc_network_open(p->net, loop);
    if (r < 0) {
        nai_log_error(ICC_LOG_CORE, nai_errno, "open network failed");
        goto _fail;
    };

    /* open evnode lastest */
    if (p->stat == ICC_RSTAT_CLOSED) {
        nai_evnode_set_cb(&p->ev, icc_routing_event);
        r = nai_evnode_open(&p->ev, loop);
        if (r < 0) {
            nai_log_error(ICC_LOG_CORE, nai_errno, "open evnode failed");
            goto _fail;
        };
    };

    p->stat = ICC_RSTAT_OPENED;
    p->loop = loop;
    r       = 0;

_end:
    return r;

_fail:
    ec = nai_errno;
    if (p->net) {
        icc_network_close(p->net);
        nai_free(p->net);
        p->net = 0;
    };
    if (p->stat == ICC_RSTAT_CLOSED) {
        nai_mutex_close(&p->lock);
    };
    nai_errno = ec;
    r         = -1;
    goto _end;
};

int icc_routing_close(icc_routing_t* p)
{
    int r;
    nai_list_entry_t* e;
    icc_routing_exec_t* x;

    if (p->stat != ICC_RSTAT_OPENING && p->stat != ICC_RSTAT_OPENED) {
        nai_errno = EPERM;
        r         = -1;
        goto _end;
    };

    if (p->loop) {
        r = icc_routing_exec(p, icc_routing_close_stub, 0);
    } else {
        r = icc_routing_close_stub(p, 0);
    };
    if (r < 0) {
        goto _end;
    };

    /* free execution */
    nai_list_add_tail(&p->flist, &p->elist);
    e = p->flist.next;
    for (; e != &p->flist;) {
        x = (icc_routing_exec_t*)e;
        e = e->next;
        nai_list_entry_remove(&x->ent);
        nai_free(x);
    };

    nai_mutex_close(&p->lock);
    p->loop = 0;
    p->stat = ICC_RSTAT_CLOSED;
    r       = 0;

_end:
    return r;
};

int icc_routing_exec(icc_routing_t* p, icc_routing_exec_f cb, void* ctx)
{
    int r;
    int ec;
    int notify;
    nai_list_entry_t* e;
    icc_routing_exec_t* h;

    if (nai_evloop_in_dispatch(p->loop)) {
        r = cb(p, ctx);
        goto _end;
    };

    /* lock */
    nai_mutex_lock(&p->lock);

    e = p->flist.next;
    if (e != &p->flist) {
        h = (icc_routing_exec_t*)e;
        nai_list_entry_remove(&h->ent);
    } else {
        h = (icc_routing_exec_t*)nai_malloc(sizeof(*h));
        if (h == 0) {
            nai_log_alert(ICC_LOG_CORE, nai_errno, "failed to allocate memory for execute");
            goto _fail;
        };
    };

    /* get condition variable */
    h->cond = nai_thread_local_cond();
    if (h->cond == 0) {
        nai_list_insert_head(&p->flist, &h->ent);
        nai_log_alert(ICC_LOG_CORE, nai_errno, "failed to get thread local condition variable");

        h = 0;
        goto _fail;
    };

    /* check list */
    notify = 0;
    if (nai_list_is_empty(&p->elist) && nai_list_is_empty(&p->mlist)) {
        notify = 1;
    };

    /* mark stat is waiting */
    h->stat = 0;
    h->cb   = cb;
    h->ctx  = ctx;
    nai_list_insert_tail(&p->elist, &h->ent);

    /* notify and wait */
    if (notify) {
        r = nai_evnode_post(&p->ev, 0);
        if (r >= 0) {
            r = nai_cond_wait(h->cond, &p->lock);
        };
    } else {
        r = nai_cond_wait(h->cond, &p->lock);
    };

    /* check result */
    if (r < 0) {
        if (h->stat == 0) {
            h->cond = 0;
            nai_list_entry_remove(&h->ent);
            nai_list_insert_tail(&p->flist, &h->ent);
        };

        ec = nai_errno;
    } else {
        ec      = h->errcode;
        h->cond = 0;
        nai_list_insert_tail(&p->flist, &h->ent);
    };

    /* unlock */
    nai_mutex_unlock(&p->lock);

    if (ec != 0) {
        nai_errno = ec;
        r         = -1;
    };

_end:
    return r;

_fail:
    ec = nai_errno;
    nai_mutex_unlock(&p->lock);

    if (h != 0) {
        nai_free(h);
    };
    nai_errno = ec;
    r         = -1;
    goto _end;
};

int icc_routing_send_intern(icc_routing_t* p, icc_message_t* m)
{
    int r;
    icc_service_t* s;

    s = icc_service_open(p, m->hdr.serv, m->inst, 1);
    if (s == 0) {
        nai_log_error(ICC_LOG_CORE, nai_errno, "cannot send message(%d) with unknown service(%d, %d)", m->hdr.type,
                      m->hdr.serv, m->inst);
        r = -1;
        goto _end;
    };

    r = icc_service_send(s, m);
    if (r < 0) {
        nai_log_error(ICC_LOG_CORE, nai_errno, "send message(%d) to service(%d, %d) failed", m->hdr.type, m->hdr.serv,
                      m->inst);
    };

_end:
    return r;
};

int icc_routing_send(icc_routing_t* p, icc_message_t* m)
{
    int r;
    int signal;

    if (nai_evloop_in_dispatch(p->loop)) {
        r = icc_routing_send_intern(p, m);
        goto _end;
    };

    /* lock */
    nai_mutex_lock(&p->lock);

    /* check list */
    signal = 0;
    if (nai_list_is_empty(&p->elist) && nai_list_is_empty(&p->mlist)) {
        signal = 1;
    };

    nai_list_insert_tail(&p->mlist, &m->ent);

    if (signal) {
        nai_evnode_post(&p->ev, 0);
    };

    nai_mutex_unlock(&p->lock);

    r = 0;

_end:
    return r;
};

int icc_routing_add_app(icc_routing_t* p, icc_app_t* app)
{
    int r;
    icc_client_t* c;

    c = icc_client_create(p, app->cid, 0, 0, 0);
    if (c == 0) {
        nai_log_alert(ICC_LOG_CORE, nai_errno, "failed to create app client");
        r = -1;
        goto _end;
    };

    /* link app */
    c->app = app;

    /* construct app */
    app->rt     = p;
    app->client = c;

    r = 0;

_end:
    return r;
};

int icc_routing_remove_app(icc_routing_t* p, icc_app_t* app)
{
    int r;
    icc_client_t* c;

    (void)p;

    c = app->client;
    if (c == 0) {
        r = 0;
        goto _end;
    };

    r = icc_client_close(c);
    if (r < 0) {
        nai_log_error(ICC_LOG_CORE, nai_errno, "failed to close inner client");
        goto _end;
    };

    app->rt     = 0;
    app->client = 0;
    r           = 0;

_end:
    return r;
};

int icc_routing_offer_service(
    icc_routing_t* p, icc_client_t* c, icc_serv_t serv, icc_inst_t inst, icc_version_t major, icc_versmin_t minor)
{
    int r;
    icc_service_t* s;

    s = icc_service_open(p, serv, inst, 0);
    if (s == 0) {
        nai_log_error(ICC_LOG_CORE, nai_errno, "failed to open service(%d, %d)", serv, inst);
        r = -1;
        goto _end;
    };

    r = icc_service_offer(s, c, major, minor);

_end:
    return r;
};

int icc_routing_stop_service(
    icc_routing_t* p, icc_client_t* c, icc_serv_t serv, icc_inst_t inst, icc_version_t major, icc_versmin_t minor)
{
    int r;
    icc_service_t* s;

    s = icc_service_open(p, serv, inst, 1);
    if (s == 0) {
        nai_log_error(ICC_LOG_CORE, nai_errno, "failed to open service(%d, %d)", serv, inst);
        r = -1;
        goto _end;
    };

    r = icc_service_stop(s, c, major, minor);

_end:
    return r;
};

int icc_routing_request_service(
    icc_routing_t* p, icc_client_t* c, icc_serv_t serv, icc_inst_t inst, icc_version_t major, icc_versmin_t minor)
{
    int r;
    icc_service_t* s;

    s = icc_service_open(p, serv, inst, 0);
    if (s == 0) {
        nai_log_error(ICC_LOG_CORE, nai_errno, "failed to open service(%d, %d)", serv, inst);
        r = -1;
        goto _end;
    };

    r = icc_service_request(s, c, major, minor);

_end:
    return r;
};

int icc_routing_release_service(
    icc_routing_t* p, icc_client_t* c, icc_serv_t serv, icc_inst_t inst, icc_version_t major, icc_versmin_t minor)
{
    int r;
    icc_service_t* s;

    s = icc_service_open(p, serv, inst, 1);
    if (s == 0) {
        nai_log_error(ICC_LOG_CORE, nai_errno, "failed to open service(%d, %d)", serv, inst);
        r = -1;
        goto _end;
    };

    r = icc_service_release(s, c, major, minor);

_end:
    return r;
};

int icc_routing_register_event(icc_routing_t* p,
                               icc_client_t* c,
                               icc_serv_t serv,
                               icc_inst_t inst,
                               icc_eid_t event,
                               const icc_event_info_t* info,
                               int offer)
{
    int r;
    icc_service_t* s;

    s = icc_service_open(p, serv, inst, 0);
    if (s == 0) {
        nai_log_error(ICC_LOG_CORE, nai_errno, "failed to open service(%d, %d)", serv, inst);
        r = -1;
        goto _end;
    };

    r = icc_service_register(s, c, event, info, offer);

_end:
    return r;
};

int icc_routing_unregister_event(icc_routing_t* p, icc_client_t* c, icc_serv_t serv, icc_inst_t inst, icc_eid_t event)
{
    int r;
    icc_service_t* s;

    s = icc_service_open(p, serv, inst, 1);
    if (s == 0) {
        nai_log_error(ICC_LOG_CORE, nai_errno, "failed to open service(%d, %d)", serv, inst);
        r = -1;
        goto _end;
    };

    r = icc_service_unregister(s, c, event);

_end:
    return r;
};

int icc_routing_subscribe(icc_routing_t* p,
                          icc_client_t* c,
                          icc_serv_t serv,
                          icc_inst_t inst,
                          icc_gid_t group,
                          icc_version_t major,
                          icc_eid_t event)
{
    int r;
    icc_service_t* s;

    s = icc_service_open(p, serv, inst, 0);
    if (s == 0) {
        nai_log_error(ICC_LOG_CORE, nai_errno, "failed to open service(%d, %d)", serv, inst);
        r = -1;
        goto _end;
    };

    r = icc_service_subscribe(s, c, group, major, event);

_end:
    return r;
};

int icc_routing_unsubscribe(icc_routing_t* p,
                            icc_client_t* c,
                            icc_serv_t serv,
                            icc_inst_t inst,
                            icc_gid_t group,
                            icc_version_t major,
                            icc_eid_t event)
{
    int r;
    icc_service_t* s;

    s = icc_service_open(p, serv, inst, 1);
    if (s == 0) {
        nai_log_error(ICC_LOG_CORE, nai_errno, "failed to open service(%d, %d)", serv, inst);
        r = -1;
        goto _end;
    };

    r = icc_service_unsubscribe(s, c, group, major, event);

_end:
    return r;
};

int icc_routing_subscribe_ack(icc_routing_t* p,
                              icc_client_t* c,
                              int sult,
                              icc_cid_t rid,
                              icc_serv_t serv,
                              icc_inst_t inst,
                              icc_gid_t group,
                              icc_eid_t event)
{
    int r;
    icc_service_t* s;

    (void)rid;

    s = icc_service_open(p, serv, inst, 1);
    if (s == 0) {
        nai_log_error(ICC_LOG_CORE, nai_errno, "failed to open service(%d, %d)", serv, inst);
        r = -1;
        goto _end;
    };

    r = icc_service_subscribe_ack(s, c, sult, group, s->major, event);

_end:
    return r;
};
