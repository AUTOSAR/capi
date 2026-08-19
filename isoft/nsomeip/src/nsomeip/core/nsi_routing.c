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
/// @file       nsi_routing.c
/// @brief
/// @details
/// @date       2021-05-16
/// @author     xn
/// @version    1.2.0
///
/// ================================================================

#include "nsi_routing_impl.h"
#include "nsi_client.h"
#include "nsi_service.h"
#include "nsi_discovery.h"
#include "nsi_log.h"
#include "nsomeip/core/nsi_app.h"
#include "nai/runtime/nai_errno.h"
#include "nai/runtime/nai_util.h"



//////////////////////////////////////////////////////////////////////////////
//


extern int nsi_conf_deserialize(nsi_conf_t* p, nsi_message_io_t* m);
extern int nsi_conf_add_preset_service(nsi_conf_t* p, nsi_discovery_t* sd);
extern int nsi_conf_attach_client(nsi_conf_t* p, 
    nsi_conf_t* c, nsi_cid_t cid, nsi_discovery_t* sd);
extern int nsi_conf_detach_client(nsi_conf_t* p, 
    nsi_conf_t* c, nsi_discovery_t* sd);



//////////////////////////////////////////////////////////////////////////////
//


typedef struct nsi_routing_exec_s {
    nai_list_entry_t ent;
    nai_cond_t* cond;
    nsi_routing_exec_f cb;
    void* ctx;
    int stat;
    int errcode;
} nsi_routing_exec_t;


typedef struct nsi_routing_args_s {
    nai_evloop_t* loop;
    int type;
} nsi_routing_args_t;


//////////////////////////////////////////////////////////////////////////////
// 



static int nsi_routing_open_stub(nsi_routing_t* p, void* ctx)
{
    int r;
    int ec;
    nsi_routing_args_t* a;

    a = (nsi_routing_args_t*)ctx;
    r = nsi_routing_open(p, a->loop, a->type);
    if (r < 0) {
        ec = nai_errno;
        nai_evnode_close(&p->ev);
        p->loop = 0;
        nai_errno = ec;
    };

    return r;
};


static int nsi_routing_close_stub(nsi_routing_t* p, void* ctx)
{
    int r;
    nai_rbnode_t* n;
    nai_list_entry_t* e;
    nsi_client_t* c;
    nsi_service_t* s;
    nsi_message_t* m;


    (void)ctx;

    r = nai_evnode_close(&p->ev);
    if (r < 0) {
        goto _end;
    };

    r = nsi_routing_batch_begin(p);
    if (r < 0) {
        goto _end;
    };

    /* start closing */
    p->stat = NSI_RSTAT_CLOSING;


    /* free services
     * PRS_SOMEIPSD_00432
     */
    n = nai_rbtree_begin(&p->servs);
    for ( ; n != nai_rbtree_end(&p->servs); ) {
        s = nai_containof(n, nsi_service_t, entr);
        n = nai_rbtree_next(n);
        nsi_service_close(s);
    };

    /* close discovery */
    if (p->sd) {
        nsi_discovery_close(p->sd);
        nsi_routing_free(p, p->sd);
        p->sd = 0;
    };

    /* close specical client */
    if (p->cl) {
        nsi_client_close(p->cl);
        p->cl = 0;
    };

    /* free clients */
    n = nai_rbtree_begin(&p->clients);
    for ( ; n != nai_rbtree_end(&p->clients); ) {
        c = nai_containof(n, nsi_client_t, entr);
        n = nai_rbtree_next(n);
        nsi_client_close(c);
    };

    /* close ipc endpoint */
    if (p->ipc) {
        nsi_routing_release_endpoint(p->ipc);
        p->ipc = 0;
    };

    /* free messages */
    nai_list_add_tail(&p->mlist, &p->posted);
    e = p->mlist.next;
    for ( ; e != &p->mlist; ) {
        m = (nsi_message_t*)e;
        e = e->next;
        nai_list_entry_remove(&m->ent);
        nsi_message_close(m);
    };

    /* close network */
    if (p->net) {
        nsi_network_close(p->net);
        nai_free(p->net);
        p->net = 0;
    };

_end:
    return r;
};


static int nsi_routing_set_link_state_stub(nsi_routing_t* p, void* ctx)
{
    int r;
    int on;
    nai_rbnode_t* n;
    nsi_service_t* s;


    if (p->stat != NSI_RSTAT_OPENED) {
        nai_errno = EPERM;
        r = -1;
        goto _end;
    };
    if (p->type != NSI_RTYPE_ROUTE) {
        r = 0;
        goto _end;
    };

    on = !!*(int*)ctx;
    if (p->link != on) {
        /* resume discovery first */
        if (on && p->sd) {
            r = nsi_discovery_resume(p->sd);
            if (r < 0) {
                goto _end;
            };
        };

        /* tell all services */
        n = nai_rbtree_begin(&p->servs);
        for ( ; n != nai_rbtree_end(&p->servs); ) {
            s = nai_containof(n, nsi_service_t, entr);
            n = nai_rbtree_next(n);
            r = nsi_service_handle_linkstate(s, on);
            if (r < 0) {
                if (!on || nai_errno != EADDRINUSE) {
                    goto _end;
                };

                /* fixme: bind failed */
            };
        };

        /* hangup discovery at last */
        if (!on && p->sd) {
            r = nsi_discovery_hangup(p->sd);
            if (r < 0) {
                goto _end;
            };
        };

        p->link = on;
    };
    r = 0;

_end:
    return r;
};


static int nsi_routing_exec_queue(nsi_routing_t* p)
{
    int r;
    int ec;
    int empty = 0;
    nsi_client_t* c;
    nsi_message_t* m;
    nsi_routing_exec_t* h;
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
                m = (nsi_message_t*)list.next;
                nai_list_entry_remove(&m->ent);

                r = nsi_routing_send_intern(p, m);
                if (r < 0) {
                    if (m->hdr.type == NSI_MT_REQUEST || 
                        m->hdr.type == NSI_MT_REQUEST_NO_RETURN) {
                        ec = nai_errno;
                        c = nsi_client_find_in_routing(p, m->hdr.client);
                        if (c != 0) {
                            nsi_client_handle_sent(c, m, ec);
                        };
                    };

                    nsi_message_close(m);
                };
            };

            /* unlock for execute */
            nai_mutex_lock(&p->lock);

        };


        /* start execute */
        while (!nai_list_is_empty(&p->elist)) {

            /* get an execution */
            h = (nsi_routing_exec_t*)p->elist.next;
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


static int nsi_routing_event(nai_evnode_t* e, int events)
{
    int r;
    nsi_routing_t* p;


    p = nai_containof(e, nsi_routing_t, ev);
    if (events & NAI_EV_NOTIFY) {
        switch (nai_ev_notify_code(events)) {
        case 0: /* async execution */
            nsi_routing_exec_queue(p);
            break;
        default:
            break;
        };
    } else if (events & NAI_EV_TIMEOUT) {
        if (p->sd) {
            nsi_discovery_handle_timeout(p->sd);
        };
    };

    r = 0;

    return r;
};


//////////////////////////////////////////////////////////////////////////////
// 


extern nsi_endpoint_cb_t nsi_routing_endpoint_cb;


int nsi_routing_init(nsi_routing_t* p)
{
    int r;


    p->stat = NSI_RSTAT_CLOSED;
    p->type = 0;
    p->link = 1;
    p->batch = 0;
    p->ipc = 0;
    p->net = 0;
    p->sd = 0;
    p->cl = 0;
    p->ch = 0;
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
#if defined(NSI_NET_FILTER)
    p->closeincoming = 0;
    p->closeoutgoing = 0;
#endif

    return r;
};


int nsi_routing_set_conf(nsi_routing_t* p, nsi_conf_t* c)
{
    int r;


    if (p->stat != NSI_RSTAT_CLOSED) {
        nai_errno = EPERM;
        r = -1;
        goto _end;
    };

    p->conf = c;
    r = 0;

_end:
    return r;
};


int nsi_routing_set_link_state(nsi_routing_t* p, int on)
{
    int r;


    if (p->stat != NSI_RSTAT_OPENED) {
        p->link = !!on;
        r = 0;
        goto _end;
    };

    r = nsi_routing_exec(p, nsi_routing_set_link_state_stub, &on);

_end:
    return r;
};


int nsi_routing_open(nsi_routing_t* p, nai_evloop_t* loop, int type)
{
    int r;
    int n;
    int ec;
    int flag;
    nsi_app_t* app;
    nsi_routing_args_t a;


    switch (type) {
    case NSI_RTYPE_AUTO:
    case NSI_RTYPE_ROUTE:
    case NSI_RTYPE_PROXY:
        break;
    default:
        nai_log_error(NSI_LOG_CORE, 
            EINVAL, "invalid routing type %d", type);
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    if (loop == 0) {
        nai_log_error(NSI_LOG_CORE, 
            EINVAL, "must offer a valid evloop");
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    if (p->stat > NSI_RSTAT_OPENING) {
        nai_log_error(NSI_LOG_CORE, 
            EPERM, "the routing is already opened");
        nai_errno = EPERM;
        r = -1;
        goto _end;
    };

    if (p->stat == NSI_RSTAT_CLOSED) {
        r = nai_mutex_open(&p->lock, 0);
        if (r < 0) {
            nai_log_alert(NSI_LOG_CORE, 
                nai_errno, "create mutex failed");
            goto _end;
        };
    };

    if (nai_evloop_in_dispatch(loop) == 0) {
        nai_evnode_set_cb(&p->ev, nsi_routing_event);
        r = nai_evnode_open(&p->ev, loop);
        if (r < 0) {
            nai_log_error(NSI_LOG_CORE, 
                nai_errno, "open evnode failed");
            goto _fail;
        };

        p->stat = NSI_RSTAT_OPENING;
        p->loop = loop;
        a.loop = loop;
        a.type = type;
        r = nsi_routing_exec(p, nsi_routing_open_stub, &a);
        if (r < 0) {
            ec = nai_errno;
            nsi_routing_close(p);
            nai_errno = ec;
        };
        goto _end;
    };


    p->net = (nsi_network_t*)nai_malloc(sizeof(*p->net));
    if (p->net == 0) {
        nai_log_alert(NSI_LOG_CORE, 
            nai_errno, "allocate network failed");
        r = -1;
        goto _fail;
    };

    nsi_network_init(p->net);
    nsi_network_set_cb(p->net, &nsi_routing_endpoint_cb);
    nsi_network_set_routing(p->net, p);
    if (p->conf != 0 && p->conf->net.seted) {
        nsi_network_set_conf(p->net, &p->conf->net);
    };

    r = nsi_network_open(p->net, loop);
    if (r < 0) {
        nai_log_error(NSI_LOG_CORE, 
            nai_errno, "open network failed");
        goto _fail;
    };


    if (p->conf == 0) {
        p->type = type;
        if (p->type == NSI_RTYPE_AUTO) {
            p->type = NSI_RTYPE_ROUTE;
        };
    } else {
        p->type = type;
        if (p->type == NSI_RTYPE_AUTO) {
            p->type = p->conf->mode;
        };

        flag = 0;
        if (p->net->use_npc) {
            flag = NSI_EFLAG_NPC;
        };
        /* open ipc endpoint */
        for (n = 0; n < 2; n ++) {
#if !defined(_WIN32)
            if (p->type == NSI_RTYPE_ROUTE) {
#else
            if (p->type != NSI_RTYPE_PROXY) {
#endif
                p->ipc = nsi_routing_create_server(p, &p->conf->local, 1, flag, 0);
            } else {
                p->ipc = nsi_routing_create_client(
                    p, &p->conf->local, 1, 
                    p->type == NSI_RTYPE_PROXY ? NSI_EFLAG_WAIT | flag : flag, 0);
            };
            if (p->ipc != 0) {
                break;
            };
            if (p->type == NSI_RTYPE_AUTO) {
#if !defined(_WIN32)
                p->type = NSI_RTYPE_ROUTE;
#else
                p->type = NSI_RTYPE_PROXY;
#endif
                nai_log_error(NSI_LOG_CORE, nai_errno, 
                    "open local client endpoint failed, try to create server");
            } else {
                nai_log_error(NSI_LOG_CORE, 
                    nai_errno, "open local %s endpoint failed", 
                    p->type == NSI_RTYPE_ROUTE ? "server" : "client");
                goto _fail;
            };
        };
        if (p->type == NSI_RTYPE_AUTO) {
#if !defined(_WIN32)
            p->type = NSI_RTYPE_PROXY;
#else
            p->type = NSI_RTYPE_ROUTE;
#endif
        };

        /* open discovery */
        if (p->type == NSI_RTYPE_PROXY || 
            p->conf) {

            p->sd = (nsi_discovery_t*)nsi_routing_alloc(p, sizeof(*p->sd));
            if (p->sd == 0) {
                nai_log_alert(NSI_LOG_CORE, nai_errno, 
                    "failed to allocate memory for service discovery");
                r = -1;
                goto _fail;
            };

            nsi_discovery_init(p->sd);
            r = nsi_discovery_open(p->sd, p);
            if (r < 0) {
                nai_log_error(NSI_LOG_CORE, 
                    nai_errno, "open service discovery failed");
                r = -1;
                goto _fail;
            };

            r = nsi_conf_add_preset_service(p->conf, p->sd);
            if (r < 0) {
                nai_log_error(NSI_LOG_CORE, 
                    nai_errno, "add preset services to discovery failed");
                r = -1;
                goto _fail;
            };
        };
    };


    /* open specical client */
    if (p->type == NSI_RTYPE_ROUTE) {
        app = (nsi_app_t*)nsi_routing_alloc(p, sizeof(*app));
        if (app == 0) {
            nai_log_alert(NSI_LOG_CORE, nai_errno, 
                "failed to allocate memory for inner app");
            r = -1;
            goto _end;
        };

        nsi_app_init(app);
        app->cid = NSI_CLIENT_ID_TEMP;

        r = nsi_routing_add_app(p, app);
        if (r < 0) {
            nai_log_error(NSI_LOG_CORE, 
                nai_errno, "add inner app failed");
            goto _fail;
        };

        p->cl = app->client;
    };


    /* open evnode lastest */
    if (p->stat == NSI_RSTAT_CLOSED) {
        nai_evnode_set_cb(&p->ev, nsi_routing_event);
        r = nai_evnode_open(&p->ev, loop);
        if (r < 0) {
            nai_log_error(NSI_LOG_CORE, 
                nai_errno, "open evnode failed");
            goto _fail;
        };
    };


    p->stat = NSI_RSTAT_OPENED;
    p->loop = loop;
    r = 0;


_end:
    return r;

_fail:
    ec = nai_errno;
    if (p->cl) {
        nsi_client_close(p->cl);
        p->cl = 0;
    };
    if (p->sd) {
        nsi_discovery_close(p->sd);
        nsi_routing_free(p, p->sd);
        p->sd = 0;
    };
    if (p->ipc) {
        nsi_routing_release_endpoint(p->ipc);
        p->ipc = 0;
    };
    if (p->net) {
        nsi_network_close(p->net);
        nai_free(p->net);
        p->net = 0;
    };
    if (p->stat == NSI_RSTAT_CLOSED) {
        nai_mutex_close(&p->lock);
    };
    nai_errno = ec;
    r = -1;
    goto _end;
};


int nsi_routing_close(nsi_routing_t* p)
{
    int r;
    nai_list_entry_t* e;
    nsi_routing_exec_t* x;


    if (p->stat != NSI_RSTAT_OPENING && 
        p->stat != NSI_RSTAT_OPENED) {
        nai_errno = EPERM;
        r = -1;
        goto _end;
    };


    if (p->loop) {
        r = nsi_routing_exec(p, nsi_routing_close_stub, 0);
    } else {
        r = nsi_routing_close_stub(p, 0);
    };
    if (r < 0) {
        goto _end;
    };

    /* free execution */
    nai_list_add_tail(&p->flist, &p->elist);
    e = p->flist.next;
    for ( ; e != &p->flist; ) {
        x = (nsi_routing_exec_t*)e;
        e = e->next;
        nai_list_entry_remove(&x->ent);
        nai_free(x);
    };

    nai_mutex_close(&p->lock);
    p->batch = 0;
    p->loop = 0;
    p->stat = NSI_RSTAT_CLOSED;
    r = 0;

_end:
    return r;
};

#if defined(NSI_NET_FILTER)
int nsi_routing_set_net_filter(nsi_routing_t* p, int direction, int close)
{
    if (direction == 0) {
        p->closeoutgoing = !!close;
    } else if (direction == 1) {
        p->closeincoming = !!close;
    };
    nai_log_debug(NSI_LOG_CORE, 0 ,
        "set routing net filter value to in/out %u/%u",
        p->closeincoming, p->closeoutgoing);
    return 0;
};
#endif


int nsi_routing_exec(nsi_routing_t* p, nsi_routing_exec_f cb, void* ctx)
{
    int r;
    int ec;
    int notify;
    nai_list_entry_t* e;
    nsi_routing_exec_t* h;


    if (nai_evloop_in_dispatch(p->loop)) {
        r = cb(p, ctx);
        goto _end;
    };

    /* lock */
    nai_mutex_lock(&p->lock);


    e = p->flist.next;
    if (e != &p->flist) {
        h = (nsi_routing_exec_t*)e;
        nai_list_entry_remove(&h->ent);
    } else {

        h = (nsi_routing_exec_t*)nai_malloc(sizeof(*h));
        if (h == 0) {
            nai_log_alert(NSI_LOG_CORE, nai_errno, 
                "failed to allocate memory for execute");
            goto _fail;
        };
    };

    /* get condition variable */
    h->cond = nai_thread_local_cond();
    if (h->cond == 0) {
        nai_list_insert_head(&p->flist, &h->ent);
        nai_log_alert(NSI_LOG_CORE, nai_errno, 
            "failed to get thread local condition variable");

        h = 0;
        goto _fail;
    };

    /* check list */
    notify = 0;
    if (nai_list_is_empty(&p->elist) && 
        nai_list_is_empty(&p->mlist)) {
        notify = 1;
    };

    /* mark stat is waiting */
    h->stat = 0;
    h->cb = cb;
    h->ctx = ctx;
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
        ec = h->errcode;
        h->cond = 0;
        nai_list_insert_tail(&p->flist, &h->ent);
    };

    /* unlock */
    nai_mutex_unlock(&p->lock);


    if (ec != 0) {
        nai_errno = ec;
        r = -1;
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
    r = -1;
    goto _end;
};


int nsi_routing_send_intern(
    nsi_routing_t* p, nsi_message_t* m)
{
    int r;
    nsi_client_t* c;
    nsi_service_t* s;


    switch (m->hdr.type) {
    case NSI_MT_RESPONSE:
    case NSI_MT_ERROR:
        c = nsi_client_find_in_routing(p, m->hdr.client);
        if (c != 0) {
            r = nsi_client_send(c, m, 0, 0);
            if (r < 0) {
                nai_log_error(NSI_LOG_CORE, nai_errno, 
                    "send message(%d) to client(0x%x) failed", 
                    m->hdr.type, m->hdr.client);
            };
            break;
        };

        /* fallthrough */

    default:
        s = nsi_service_open(p, m->hdr.serv, m->inst, 1);
        if (s == 0) {
            nai_log_error(NSI_LOG_CORE, nai_errno, 
                "cannot send message(%d) with unknown service(%d, %d)", 
                m->hdr.type, m->hdr.serv, m->inst);
            r = -1;
            goto _end;
        };

        r = nsi_service_send(s, m);
        if (r < 0) {
            nai_log_error(NSI_LOG_CORE, nai_errno, 
                "send message(%d) to service(%d, %d) failed", 
                m->hdr.type, m->hdr.serv, m->inst);
        };
        break;
    };


_end:
    return r;
};


int nsi_routing_send(nsi_routing_t* p, nsi_message_t* m)
{
    int r;
    int signal;


    if (nai_evloop_in_dispatch(p->loop)) {
        r = nsi_routing_send_intern(p, m);
        goto _end;
    };

    /* lock */
    nai_mutex_lock(&p->lock);


    /* check list */
    signal = 0;
    if (nai_list_is_empty(&p->elist) && 
        nai_list_is_empty(&p->mlist)) {
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


int nsi_routing_add_app(nsi_routing_t* p, nsi_app_t* app)
{
    int r;
    int ec;
    nsi_client_t* c;
    nsi_endpoint_t* e;


    if (p->type != NSI_RTYPE_ROUTE && 
        app->cid == NSI_CLIENT_ID_SYSTEM) {
        nai_log_error(NSI_LOG_CORE, 
            EINVAL, "the app without a valid client id");
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    e = nsi_routing_create_inner(p);
    if (e == 0) {
        nai_log_alert(NSI_LOG_CORE, 
            nai_errno, "failed to create inner endpoint");
        r = -1;
        goto _end;
    };

    c = nsi_client_create(p, app->cid, e, app->cid, 0);
    if (c == 0) {
        nai_log_alert(NSI_LOG_CORE, 
            nai_errno, "failed to create inner client");
        goto _fail;
    };

    /* release */
    nsi_routing_release_endpoint(e);

    /* link app */
    c->app = app;

    /* construct app */
    app->rt = p;
    app->client = c;

    /* the checking client */
    if (app->cid == NSI_CLIENT_ID_SYSTEM) {
        p->ch = c;
    };

    r = 0;


_end:
    return r;

_fail:
    ec = nai_errno;
    nsi_routing_release_endpoint(e);
    nai_errno = ec;
    r = -1;
    goto _end;
};


int nsi_routing_remove_app(nsi_routing_t* p, nsi_app_t* app)
{
    int r;
    nsi_client_t* c;


    (void)p;

    c = app->client;
    if (c == 0) {
        r = 0;
        goto _end;
    };

    r = nsi_client_close(c);
    if (r < 0) {
        nai_log_error(NSI_LOG_CORE, 
            nai_errno, "failed to close inner client");
        goto _end;
    };

    if (p->ch == c) {
        p->ch = 0;
    };

    app->rt = 0;
    app->client = 0;
    r = 0;

_end:
    return r;
};


int nsi_routing_offer_service(nsi_routing_t* p, 
    nsi_client_t* c, 
    nsi_serv_t serv, nsi_inst_t inst, 
    nsi_version_t major, nsi_versmin_t minor)
{
    int r;
    nsi_service_t* s;


    s = nsi_service_open(p, serv, inst, 0);
    if (s == 0) {
        nai_log_error(NSI_LOG_CORE, 
            nai_errno, "failed to open service(%d, %d)", serv, inst);
        r = -1;
        goto _end;
    };


    r = nsi_service_offer(s, c, major, minor);
    if (r < 0) {
        goto _end;
    };


_end:
    return r;
};


int nsi_routing_stop_service(nsi_routing_t* p, 
    nsi_client_t* c, 
    nsi_serv_t serv, nsi_inst_t inst, 
    nsi_version_t major, nsi_versmin_t minor)
{
    int r;
    nsi_service_t* s;


    s = nsi_service_open(p, serv, inst, 1);
    if (s == 0) {
        nai_log_error(NSI_LOG_CORE, 
            nai_errno, "failed to open service(%d, %d)", serv, inst);
        r = -1;
        goto _end;
    };


    r = nsi_service_stop(s, c, major, minor);


_end:
    return r;
};


int nsi_routing_request_service(nsi_routing_t* p, 
    nsi_client_t* c, 
    nsi_serv_t serv, nsi_inst_t inst, 
    nsi_version_t major, nsi_versmin_t minor)
{
    int r;
    nsi_service_t* s;


    if (inst == NSI_INSTANCE_ANY) {
        if (p->sd == 0) {
            nai_log_error(NSI_LOG_CORE, ENOTSUP, 
                "cannot find service without service discovery");
            nai_errno = ENOTSUP;
            r = -1;
        } else {
            r = nsi_discovery_request_service(
                p->sd, c, serv, inst , major, minor);
        };
        goto _end;
    };

    s = nsi_service_open(p, serv, inst, 0);
    if (s == 0) {
        nai_log_error(NSI_LOG_CORE, 
            nai_errno, "failed to open service(%d, %d)", serv, inst);
        r = -1;
        goto _end;
    };


    r = nsi_service_request(s, c, major, minor);


_end:
    return r;
};


int nsi_routing_release_service(nsi_routing_t* p,
    nsi_client_t* c, 
    nsi_serv_t serv, nsi_inst_t inst, 
    nsi_version_t major, nsi_versmin_t minor)
{
    int r;
    nsi_service_t* s;


    if (inst == NSI_INSTANCE_ANY) {
        if (p->sd == 0) {
            nai_log_error(NSI_LOG_CORE, ENOTSUP, 
                "cannot stop find service without service discovery");
            nai_errno = ENOTSUP;
            r = -1;
        } else {
            r = nsi_discovery_release_service(
                p->sd, c, serv, inst , major, minor);
        };
        goto _end;
    };

    s = nsi_service_open(p, serv, inst, 1);
    if (s == 0) {
        nai_log_error(NSI_LOG_CORE, 
            nai_errno, "failed to open service(%d, %d)", serv, inst);
        r = -1;
        goto _end;
    };


    r = nsi_service_release(s, c, major, minor);

_end:
    return r;
};


int nsi_routing_register_event(nsi_routing_t* p, 
    nsi_client_t* c, 
    nsi_serv_t serv, nsi_inst_t inst, 
    nsi_eid_t event, const nsi_event_info_t* info, int offer)
{
    int r;
    nsi_service_t* s;


    s = nsi_service_open(p, serv, inst, 0);
    if (s == 0) {
        nai_log_error(NSI_LOG_CORE, 
            nai_errno, "failed to open service(%d, %d)", serv, inst);
        r = -1;
        goto _end;
    };


    r = nsi_service_register(s, c, event, info, offer);

_end:
    return r;
};


int nsi_routing_unregister_event(nsi_routing_t* p, 
    nsi_client_t* c, 
    nsi_serv_t serv, nsi_inst_t inst, 
    nsi_eid_t event)
{
    int r;
    nsi_service_t* s;


    s = nsi_service_open(p, serv, inst, 1);
    if (s == 0) {
        nai_log_error(NSI_LOG_CORE, 
            nai_errno, "failed to open service(%d, %d)", serv, inst);
        r = -1;
        goto _end;
    };


    r = nsi_service_unregister(s, c, event);

_end:
    return r;
};


int nsi_routing_subscribe(nsi_routing_t* p, 
    nsi_client_t* c, 
    nsi_serv_t serv, nsi_inst_t inst, 
    nsi_gid_t group, nsi_version_t major, nsi_eid_t event, 
    uint32_t ttl, uint32_t init, 
    const nsi_endpoint_name_t* names)
{
    int r;
    nsi_service_t* s;


    s = nsi_service_open(p, serv, inst, 0);
    if (s == 0) {
        nai_log_error(NSI_LOG_CORE, 
            nai_errno, "failed to open service(%d, %d)", serv, inst);
        r = -1;
        goto _end;
    };


    r = nsi_service_subscribe(s, c, 
        group, major, event, ttl, init, names);

_end:
    return r;
};


int nsi_routing_unsubscribe(nsi_routing_t* p, 
    nsi_client_t* c, 
    nsi_serv_t serv, nsi_inst_t inst, 
    nsi_gid_t group, nsi_version_t major, nsi_eid_t event)
{
    int r;
    nsi_service_t* s;


    s = nsi_service_open(p, serv, inst, 1);
    if (s == 0) {
        nai_log_error(NSI_LOG_CORE, 
            nai_errno, "failed to open service(%d, %d)", serv, inst);
        r = -1;
        goto _end;
    };


    r = nsi_service_unsubscribe(s, c, group, major, event);

_end:
    return r;
};


int nsi_routing_subscribe_ack(nsi_routing_t* p, 
    nsi_client_t* c, int sult, nsi_cid_t rid, 
    nsi_serv_t serv, nsi_inst_t inst, 
    nsi_gid_t group, nsi_eid_t event)
{
    int r;
    nsi_client_t* q;
    nsi_service_t* s;


    s = nsi_service_open(p, serv, inst, 1);
    if (s == 0) {
        nai_log_error(NSI_LOG_CORE, 
            nai_errno, "failed to open service(%d, %d)", serv, inst);
        r = -1;
        goto _end;
    };

    q = nsi_client_find_in_routing(p, rid);
    if (q == 0) {
        nai_log_error(NSI_LOG_CORE, 
            nai_errno, "cannot ack to non-existent client(0x%x)", rid);
        r = -1;
        goto _end;
    };

    if (s->own == 0 || 
        s->own->client != c) {
        nai_log_error(NSI_LOG_CORE, EPERM, 
            "client(0x%x) is not a service(%d, %d) provider "
            "and has no right to ack", 
            c->cid, serv, inst);
        nai_errno = EPERM;
        r = -1;
        goto _end;
    };

    r = nsi_service_subscribe_ack(s, q, sult, group, s->major, event, 0, 0);

_end:
    return r;
};




//////////////////////////////////////////////////////////////////////////////
// help


int nsi_routing_get_localname(
    nsi_routing_t* p, nsi_endpoint_name_t* name)
{
    int r;

    if (p->conf) {
        name[0] = p->conf->local;
        r = 0;
    } else {
        nai_errno = ENOENT;
        r = -1;
    };

    return r;
};


int nsi_routing_is_local_service(
    nsi_routing_t* p, nsi_serv_t serv, nsi_inst_t inst)
{
    int r;

    if (p->conf) {
        r = nsi_conf_is_local_service(p->conf, serv, inst);
    } else {
        r = 0;
    };

    return r;
};


int nsi_routing_is_preset_service(
    nsi_routing_t* p, nsi_serv_t serv, nsi_inst_t inst)
{
    int r;

    if (p->conf) {
        r = nsi_conf_is_preset_service(p->conf, serv, inst);
    } else {
        r = 0;
    };

    return r;
};


int nsi_routing_is_subnet(
    nsi_routing_t* p, const nsi_endpoint_name_t* name)
{
    int r;
    nsi_conf_t* c;


    c = p->conf;

    if (c == 0) {
        r = 1;
        goto _end;
    };

    r = nsi_conf_is_subnet(c, name);

_end:
    return r;
};


int nsi_routing_is_mcast(
    nsi_routing_t* p, const nsi_endpoint_name_t* name)
{
    int r;
    nsi_conf_t* c;


    c = p->conf;

    if (c == 0) {
        r = 1;
        goto _end;
    };

    r = nsi_conf_is_mcast(c, name);

_end:
    return r;
};


int nsi_routing_get_service_info(
    nsi_routing_t* p, nsi_serv_t serv, nsi_inst_t inst, 
    nsi_service_info_t* info, int localonly)
{
    int r;


    /* PRS_SOMEIPSD_00476 */
    if (p->sd && !localonly) {
        r = nsi_discovery_get_service_info(p->sd, serv, inst, info);
        if (r >= 0) {
            /* get local tls info */
            if (p->conf) {
                nsi_conf_get_tls_info(p->conf, serv, inst, &info->tls);
            };
            goto _end;
        };
    };

    if (p->conf) {
        r = nsi_conf_get_service(p->conf, serv, inst, info);
        if (r >= 0) {
            if (!localonly || info->local) {
                goto _end;
            };
        };
    };

    nai_errno = ENOENT;
    r = -1;

_end:
    return r;
};


int nsi_routing_get_event_info(
    nsi_routing_t* p, nsi_serv_t serv, nsi_inst_t inst, nsi_eid_t eid, 
    nsi_event_info_t* info)
{
    int r;


    if (p->conf) {
        r = nsi_conf_get_event(p->conf, serv, inst, eid, info);
        if (r >= 0) {
            goto _end;
        };
    };

    nai_errno = ENOENT;
    r = -1;

_end:
    return r;
};


int nsi_routing_get_group_info(
    nsi_routing_t* p, nsi_serv_t serv, nsi_inst_t inst, nsi_gid_t gid, 
    nsi_group_info_t* info)
{
    int r;


    if (p->conf) {
        r = nsi_conf_get_group(p->conf, serv, inst, gid, info);
        if (r >= 0) {
            goto _end;
        };
    };

    nai_errno = ENOENT;
    r = -1;

_end:
    return r;
};


int nsi_routing_get_bind(nsi_routing_t* p, 
    int reliable, const nsi_endpoint_name_t* name, nsi_range_t** pptr)
{
    int r;


    if (p->conf) {
        r = nsi_conf_get_bind(p->conf, reliable, name, pptr);
        if (r >= 0) {
            goto _end;
        };
    };

    nai_errno = ENOENT;
    r = -1;

_end:
    return r;
};



//////////////////////////////////////////////////////////////////////////////
// routing verify


int nsi_routing_grant_offer(
    nsi_routing_t* p, nsi_client_t* c, 
    nsi_serv_t serv, nsi_inst_t inst, 
    nsi_version_t major, nsi_versmin_t minor)
{
    int r;
    nsi_app_t* a;


    if (p->ch) {
        a = p->ch->app;
        if (a->offercb) {
            r = a->offercb(a, a->ud, serv, inst, major, minor, &c->cred);
            if (r != 0) {
                nai_errno = EPERM;
                nai_log_info(NSI_LOG_CORE, EPERM, 
                    "the client(0x%x) offer service(%d, %d) is denied", 
                    c->cid, serv, inst);
                r = -1;
                goto _end;
            };
        };
    };

    r = 0;

_end:
    return r;
};


int nsi_routing_grant_find(
    nsi_routing_t* p, nsi_client_t* c, 
    nsi_serv_t serv, nsi_inst_t inst, 
    nsi_version_t major, nsi_versmin_t minor)
{
    int r;
    nsi_app_t* a;


    if (p->ch) {
        a = p->ch->app;
        if (a->findcb) {
            r = a->findcb(a, a->ud, serv, inst, major, minor, &c->cred);
            if (r != 0) {
                nai_errno = EPERM;
                nai_log_info(NSI_LOG_CORE, EPERM, 
                    "the client(0x%x) request service(%d, %d) is denied", 
                    c->cid, serv, inst);
                r = -1;
                goto _end;
            };
        };
    };

    r = 0;

_end:
    return r;
};


int nsi_routing_grant_notify(
    nsi_routing_t* p, nsi_client_t* c, int avail, 
    nsi_serv_t serv, nsi_inst_t inst, 
    nsi_version_t major, nsi_versmin_t minor)
{
    int r;
    nsi_app_t* a;


    if (p->ch) {
        a = p->ch->app;
        if (a->availcb) {
            r = a->availcb(a, 
                a->ud, avail, serv, inst, major, minor, &c->cred);
            if (r != 0) {
                nai_errno = EPERM;
                nai_log_info(NSI_LOG_CORE, EPERM, 
                    "send notification of service(%d, %d) to "
                    "the client(0x%x) is denied", 
                    serv, inst, c->cid);
                r = -1;
                goto _end;
            };
        };
    };

    r = 0;

_end:
    return r;
};


int nsi_routing_grant_request(
    nsi_routing_t* p, nsi_message_t* m)
{
    int r;
    nsi_app_t* a;


    if (p->ch) {
        /* do checking */
        a = p->ch->app;
        if (a->msgcb) {
            r = a->msgcb(a, a->ud, m);
            if (r != 0) {
                nai_errno = EPERM;
                nai_log_info(NSI_LOG_CORE, EPERM, 
                    "the request of service(%d, %d) "
                    "from the client(0x%x) is denied", 
                    m->hdr.serv, m->inst, m->hdr.client);
                r = -1;
                goto _end;
            };
        };
    };

    r = 0;

_end:
    return r;
};


int nsi_routing_grant_subscribe(
    nsi_routing_t* p, nsi_client_t* c, 
    nsi_serv_t serv, nsi_inst_t inst, nsi_gid_t gid, nsi_eid_t eid)
{
    int r;
    nsi_app_t* a;

    /* do checking */
    if (p->ch) {
        a = p->ch->app;
        if (a->subcb) {
            r = a->subcb(
                a, a->ud, c->cid, serv, inst, gid, eid, &c->cred);
            if (r != 0) {
                nai_errno = EPERM;
                nai_log_info(NSI_LOG_CORE, EPERM, 
                    "the client(0x%x) subscribe "
                    "event group(%d) of service(%d, %d) is denied", 
                    c->cid, gid, serv, inst);
                r = -1;
                goto _end;
            };
        };
    };

    r = 0;

_end:
    return r;
};


//////////////////////////////////////////////////////////////////////////////
// routing configuration


static int nsi_routing_update_services(nsi_routing_t* p)
{
    int r;
    nai_rbnode_t* n;
    nsi_service_t* s;


    if (p->type == NSI_RTYPE_PROXY) {
        r = 0;
        goto _end;
    };

    r = nsi_routing_batch_begin(p);
    if (r < 0) {
        nai_log_alert(NSI_LOG_CORE, 
            nai_errno, "start batch failed");
        goto _end;
    };

    n = nai_rbtree_begin(&p->servs);
    for ( ; n != nai_rbtree_end(&p->servs); ) {
        s = nai_containof(n, nsi_service_t, entr);
        n = nai_rbtree_next(n);

        r = nsi_service_reopen(s);
        if (r < 0) {
            nai_log_error(NSI_LOG_CORE, nai_errno, 
                "failed to update service(%d, %d) which is changed.", 
                s->serv, s->inst);

            /* force close */
            nsi_service_close(s);
        };
    };

    nsi_routing_batch_end(p);
    r = 0;

_end:
    return r;
};


int nsi_routing_attach_client(
    nsi_routing_t* p, nsi_client_t* c)
{
    int r;
    int ec;


    if (p->conf == 0 || c->conf == 0) {
        r = 0;
        goto _end;
    };

    r = nsi_routing_batch_begin(p);
    if (r < 0) {
        nai_log_alert(NSI_LOG_CORE, 
            nai_errno, "start batch failed");
        goto _end;
    };

    r = nsi_conf_attach_client(p->conf, c->conf, c->cid, p->sd);
    if (r < 0) {
        goto _fail;
    };

    r = nsi_routing_update_services(p);
    if (r < 0) {
        nai_log_error(NSI_LOG_CORE, 
            nai_errno, "update service failed when attach client(0x%x).", 
            c->cid);

        /* fixme: error ignored */
        r = 0;
    };

_fail:
    if (r < 0) {
        ec = nai_errno;
    };
    nsi_routing_batch_end(p);
    if (r < 0) {
        nai_errno = ec;
    };

_end:
    return r;
};


int nsi_routing_detach_client(
    nsi_routing_t* p, nsi_client_t* c)
{
    int r;
    int ec;


    if (p->conf == 0 || c->conf == 0) {
        r = 0;
        goto _end;
    };

    r = nsi_routing_batch_begin(p);
    if (r < 0) {
        nai_log_alert(NSI_LOG_CORE, 
            nai_errno, "start batch failed");
        goto _end;
    };

    r = nsi_conf_detach_client(p->conf, c->conf, p->sd);
    if (r < 0) {
        /* nsi_conf_detach_client never fails */
        assert(0);
        goto _fail;
    };

    r = nsi_routing_update_services(p);
    if (r < 0) {
        nai_log_error(NSI_LOG_CORE, 
            nai_errno, "update service failed when detach client(0x%x).", 
            c->cid);

        /* fixme: error ignored */
        r = 0;
    };

_fail:
    if (r < 0) {
        ec = nai_errno;
    };
    nsi_routing_batch_end(p);
    if (r < 0) {
        nai_errno = ec;
    };

_end:
    return r;
};


int nsi_routing_proxy_connect(
    nsi_routing_t* p, nsi_client_t* c, nsi_message_io_t* io)
{
    int r;
    int ec;
    nsi_conf_t* cf;


    if (p->conf == 0) {
        /* without configuration, nothing todo */
        r = 0;
        goto _end;
    };

    cf = (nsi_conf_t*)nsi_routing_alloc(p, sizeof(*cf));
    if (cf == 0) {
        r = -1;
        goto _end;
    };

    nsi_conf_init(cf);

    r = nsi_conf_deserialize(cf, io);
    if (r < 0) {
        goto _fail;
    };

    r = nsi_conf_attach_client(p->conf, cf, c->cid, p->sd);
    if (r < 0) {
        goto _fail;
    };

    c->conf = cf;

    r = nsi_routing_update_services(p);
    if (r < 0) {
        nai_log_error(NSI_LOG_CORE, 
            nai_errno, "update service failed when attach client(0x%x).", 
            c->cid);

        /* fixme: error ignored */
        r = 0;
    };


_end:
    return r;

_fail:
    if (cf != 0) {
        ec = nai_errno;
        nsi_conf_close(cf);
        nsi_routing_free(p, cf);
        nai_errno = ec;
    };
    goto _end;
};


