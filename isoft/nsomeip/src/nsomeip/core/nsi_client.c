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
/// @file       nsi_client.c
/// @brief
/// @details
/// @date       2021-05-16
/// @author     xn
/// @version    1.2.0
///
/// ================================================================

#include "nsi_client.h"
#include "nsi_service.h"
#include "nsi_discovery.h"
#include "nsi_proxy.h"
#include "nsi_routing_impl.h"
#include "nsi_log.h"
#include "nsomeip/core/nsi_app.h"
#include "nai/runtime/nai_errno.h"
#include "nai/runtime/nai_util.h"



static nai_rbnode_t** nsi_client_find_impl(
    nsi_routing_t* p, nsi_cid_t cid, nai_rbnode_t** pparent)
{
    nai_rbnode_t** n = &nai_rbtree_root(&p->clients);
    nai_rbnode_t* parent = nai_rbtree_end(&p->clients);
    nsi_client_t* e;


    while (*n) {
        parent = *n;
        e = nai_containof(parent, nsi_client_t, entr);
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


static nai_rbnode_t** nsi_client_find_impl2(
    nsi_routing_endpoint_t* p, nsi_cid_t cid, nai_rbnode_t** pparent)
{
    nai_rbnode_t** n = &nai_rbtree_root(&p->clients);
    nai_rbnode_t* parent = nai_rbtree_end(&p->clients);
    nsi_client_t* e;


    while (*n) {
        parent = *n;
        e = nai_containof(parent, nsi_client_t, ep.ente);
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


static int nsi_client_remove_services(nsi_client_t* c)
{
    int r;
    nai_list_entry_t* ent;
    nsi_service_t* s;
    nsi_service_ref_t* f;


    ent = c->servs.next;
    for ( ; ent != &c->servs; ) {
        f = nai_containof(ent, nsi_service_ref_t, entc);
        ent = ent->next;

        s = f->service;
        if (f->offer) {
            r = nsi_service_stop(s, c, f->major, f->minor);
        } else {
            r = nsi_service_release(s, c, f->major, f->minor);
        };
        if (r < 0) {
            nai_log_error(NSI_LOG_CORE, nai_errno, 
                "the client(0x%x) release service failed", c->cid);
            goto _end;
        };
    };

    r = 0;

_end:
    return r;
};


static int nsi_client_remove_events(nsi_client_t* c)
{
    int r;
    nai_list_entry_t* ent;
    nsi_event_ref_t* f;
    nsi_service_t* s;


    ent = c->events.next;
    for ( ; ent != &c->events; ) {
        f = nai_containof(ent, nsi_event_ref_t, entc);
        ent = ent->next;

        s = f->event->serv;
        r = nsi_service_unregister(s, c, f->event->eid);
        if (r < 0) {
            nai_log_error(NSI_LOG_CORE, nai_errno, 
                "the client(0x%x) release event failed", c->cid);
            goto _end;
        };
    };

    r = 0;

_end:
    return r;
};


static int nsi_client_remove_subscriptions(nsi_client_t* c)
{
    int r;
    nai_list_entry_t* ent;
    nsi_service_t* s;
    nsi_subscription_t* u;


    ent = c->subs.next;
    for ( ; ent != &c->subs; ) {
        u = nai_containof(ent, nsi_subscription_t, entc);
        ent = ent->next;

        s = u->serv;
        r = nsi_service_unsubscribe(s, 
            c, u->group->gid, u->major, NSI_EVENT_ANY);
        if (r < 0) {
            nai_log_error(NSI_LOG_CORE, nai_errno, 
                "the client(0x%x) release subscription failed", c->cid);
            goto _end;
        };
    };

    r = 0;

_end:
    return r;
};


static int nsi_client_remove_requests(nsi_client_t* c)
{
    int r;
    nai_list_entry_t* ent;
    nsi_discovery_t* sd;
    nsi_sd_request_t* q;
    nsi_sd_request_ref_t* f;


    sd = c->rt->sd;
    ent = c->reqs.next;
    for ( ; ent != &c->reqs; ) {
        f = nai_containof(ent, nsi_sd_request_ref_t, entc);
        ent = ent->next;

        q = f->request;
        r = nsi_discovery_release_service(sd, 
            c, q->serv, q->inst, q->major, q->minor);
        if (r < 0) {
            nai_log_error(NSI_LOG_CORE, nai_errno, 
                "the client(0x%x) release request failed", c->cid);
            goto _end;
        };
    };

    r = 0;

_end:
    return r;
};



nsi_client_t* nsi_client_find_in_routing(nsi_routing_t* p, nsi_cid_t cid)
{
    nai_rbnode_t** n;
    nai_rbnode_t* parent;
    nsi_client_t* r;


    n = nsi_client_find_impl(p, cid, &parent);
    if (n[0] == 0) {
        nai_errno = ENOENT;
        r = 0;
        goto _end;
    };

    r = nai_containof(n[0], nsi_client_t, entr);

_end:
    return r;
};


nsi_client_t* nsi_client_find_in_endpoint(nsi_endpoint_t* ep, nsi_cid_t cid)
{
    nsi_client_t* r;
    nai_rbnode_t** n;
    nai_rbnode_t* parent;
    nsi_routing_endpoint_t* e;


    e = nsi_routing_get_endpoint_ctx(ep);
    if (e == 0) {
        r = 0;
        goto _end;
    };

    n = nsi_client_find_impl2(e, cid, &parent);
    if (n[0] == 0) {
        nai_errno = ENOENT;
        r = 0;
        goto _end;
    };

    r = nai_containof(n[0], nsi_client_t, ep.ente);

_end:
    return r;
};


nsi_client_t* nsi_client_create(nsi_routing_t* p, 
    nsi_cid_t cid, nsi_endpoint_t* ep, nsi_connid_t conn, 
    const nsi_cred_t* cred)
{
    int r;
    int ec;
    nsi_client_t* c;
    nai_rbnode_t** n1;
    nai_rbnode_t** n2;
    nai_rbnode_t* parent1;
    nai_rbnode_t* parent2;
    nsi_routing_endpoint_t* e;


    n1 = nsi_client_find_impl(p, cid, &parent1);
    if (n1[0] != 0) {
        c = nai_containof(n1[0], nsi_client_t, entr);
        if (c->ep.ep != ep || c->ep.conn != conn) {
            nai_log_warn(NSI_LOG_CORE, nai_errno, 
                "cannot create client(0x%x) with different endpoint: "
                "exists(%s, %d), new(%s, %d)", cid, 
                nai_str(&c->ep.ep->host), c->ep.conn, 
                nai_str(&ep->host), conn);

            nai_errno = EEXIST;
            c = 0;
        };
        goto _end;
    };


    if (ep == 0) {
        e = 0;
    } else {
        e = nsi_routing_get_endpoint_ctx(ep);
        if (e == 0) {
            assert(0);
            c = 0;
            goto _end;
        };

        n2 = nsi_client_find_impl2(e, cid, &parent2);
        if (n2[0] != 0) {
            nai_errno = EEXIST;
            c = 0;
            goto _end;
        };
    };


    c = (nsi_client_t*)nsi_routing_alloc(e->rt, sizeof(*c));
    if (c == 0) {
        nai_log_alert(NSI_LOG_CORE, nai_errno, 
            "failed to allocate memory of client(0x%x)", cid);
        goto _end;
    };

    c->rt = p;
    c->conf = 0;
    c->app = 0;
    c->cid = cid;
    c->flags = 0;
    c->reboot = 1;
    c->reboot_recv = 0x3;
    c->offers = 0;
    c->ep.ep = ep;
    c->ep.conn = conn;
    c->ep.sinternal = 0;
    c->ep.session = 0;
    c->ep.srecv[0] = c->ep.srecv[1] = 0;
    nai_list_init(&c->servs);
    nai_list_init(&c->events);
    nai_list_init(&c->subs);
    nai_list_init(&c->reqs);

    if (cred) {
        nsi_cred_copy(&c->cred, cred);
    } else {
        nsi_cred_init(&c->cred);
    };

    if (cid & NSI_CLIENT_ID_REMOTE) {
        c->remote_cli = 1;
    };
    if (cid >= NSI_CLIENT_ID_TEMP && cid < NSI_CLIENT_ID_REMOTE) {
        c->temp = 1;
    }
    if (ep == p->ipc) {
        c->proxy = 1;
    } else if (c->remote_cli) {
        c->remote = 1;
        r = nsi_endpoint_set_opt(ep, NSI_EOPT_BIND_CONN, conn);
        if (r < 0) {
            nai_log_error(NSI_LOG_CORE, nai_errno, 
                "failed to bind connection for remote client(0x%x)", cid);
            goto _fail;
        };
        if (conn != (cid & ~NSI_CLIENT_ID_REMOTE)) {
            /* for sd-ipv4 client which has two address, 
             * one is itself for id, one is proxy for answering, 
             * bind itself for keeping cid.
             */
            r = nsi_endpoint_set_opt(ep, 
                NSI_EOPT_BIND_CONN, (cid & ~NSI_CLIENT_ID_REMOTE));
            if (r < 0) {
                nai_log_error(NSI_LOG_CORE, nai_errno, 
                    "failed to bind connection for remote client(0x%x)", cid);

                ec = nai_errno;
                nsi_endpoint_set_opt(ep, NSI_EOPT_UNBIND_NAME, conn);
                nai_errno = ec;
                goto _fail;
            };
        };
    } else {
        c->local = 1;
    };

    /* attach routing */
    nai_rbtree_link(&p->clients, &c->entr, parent1, n1);
    nai_rbtree_color(&p->clients, &c->entr);

    /* attach endpoint */
    if (ep != 0) {
        nai_rbtree_link(&e->clients, &c->ep.ente, parent2, n2);
        nai_rbtree_color(&e->clients, &c->ep.ente);
        e->refs ++;
    };


_end:
    return c;

_fail:
    ec = nai_errno;
    nsi_routing_free(e->rt, c);
    nai_errno = ec;
    c = 0;
    goto _end;
};


nsi_client_t* nsi_client_create_anon(
    nsi_routing_t* p, nsi_endpoint_t* ep, nsi_connid_t conn, 
    const nsi_cred_t* cred)
{
    nsi_client_t* c;
    nsi_cid_t cid;
    nsi_cid_t max;
    nai_rbnode_t** n;


    cid = NSI_CLIENT_ID_ANON;
    max = NSI_CLIENT_ID_TEMP;
    for ( ; cid < max; cid ++) {
        n = nsi_client_find_impl(p, cid, 0);
        if (n[0] == 0) {
            break;
        };
    };
    if (cid >= max) {
        nai_errno = EBUSY;
        c = 0;
        goto _end;
    };

    c = nsi_client_create(p, cid, ep, conn, cred);

_end:
    return c;
};


nsi_client_t* nsi_client_create_temp(nsi_routing_t* p)
{
    nsi_client_t* c;
    nsi_cid_t cid;
    nsi_cid_t max;
    nai_rbnode_t** n;
    nsi_endpoint_t* ep;


    cid = NSI_CLIENT_ID_TEMP;
    max = NSI_CLIENT_ID_REMOTE;
    for ( ; cid < max; cid ++) {
        n = nsi_client_find_impl(p, cid, 0);
        if (n[0] == 0) {
            break;
        };
    };
    if (cid >= max) {
        nai_errno = EBUSY;
        c = 0;
        goto _end;
    };

    /* use inner endpoint */
    assert(p->cl);
    ep = p->cl->ep.ep;

    /* create client */
    c = nsi_client_create(p, cid, ep, cid, 0);
    if (c != 0) {
        /* use an app to disable all callback */
        c->app = p->cl->app;
    };


_end:
    return c;
};


static int nsi_client_close_impl(nsi_client_t* c, int reset)
{
    int r;
    int ec;
    nsi_endpoint_t* ep;
    nsi_routing_t* rt;
    nsi_routing_endpoint_t* e;


    rt = c->rt;
    r = nsi_routing_batch_begin(rt);
    if (r < 0) {
        nai_log_alert(NSI_LOG_CORE, 
            nai_errno, "start batch failed");
        goto _end;
    };


    r = nsi_client_remove_requests(c);
    if (r < 0) {
        goto _stop;
    };

    r = nsi_client_remove_subscriptions(c);
    if (r < 0) {
        goto _stop;
    };

    r = nsi_client_remove_events(c);
    if (r < 0) {
        goto _stop;
    };

    r = nsi_client_remove_services(c);
    if (r < 0) {
        goto _stop;
    };


    if (reset) {
        r = 0;
        goto _stop;
    };

    if (c->conf) {
        r = nsi_routing_detach_client(rt, c);
        if (r < 0) {
            assert(0);
        };

        nsi_conf_close(c->conf);
        nsi_routing_free(rt, c->conf);
        c->conf = 0;
    };

    /* detach from routing */
    nai_rbtree_erase(&rt->clients, &c->entr);


    /* detach from endpoint */
    ep = c->ep.ep;
    if (ep != 0) {
        if (c->remote && c->remote_cli) {
            r = nsi_endpoint_set_opt(ep, NSI_EOPT_UNBIND_NAME, c->ep.conn);
            if (r < 0) {
                nai_log_error(NSI_LOG_CORE, nai_errno, 
                    "failed to unbind connection for remote client(0x%x)", 
                    c->cid);
            };

            /* for sd-ipv4 client which has two address, 
             * one is itself for id, one is proxy for answering, 
             * bind itself for keeping cid.
             * unbind it to free.
             */
            if (c->ep.conn != (c->cid & ~NSI_CLIENT_ID_REMOTE)) {
                r = nsi_endpoint_set_opt(ep, 
                    NSI_EOPT_UNBIND_NAME, c->cid & ~NSI_CLIENT_ID_REMOTE);
                if (r < 0) {
                    nai_log_error(NSI_LOG_CORE, nai_errno, 
                        "failed to unbind connection for remote client(0x%x)", 
                        c->cid);
                };
            };
        };

        e = nsi_routing_get_endpoint_ctx(ep);
        assert(e != 0);

        c->ep.ep = 0;
        nai_rbtree_erase(&e->clients, &c->ep.ente);
        nsi_routing_release_endpoint(ep);
    };

    /* free client */
    nsi_routing_free(rt, c);
    r = 0;


_stop:
    if (r < 0) {
        ec = nai_errno;
    };

    nsi_routing_batch_end(rt);

    if (r < 0) {
        nai_errno = ec;
    };

_end:
    return r;
};


int nsi_client_reset(nsi_client_t* c)
{
    return nsi_client_close_impl(c, 1);
};


int nsi_client_close(nsi_client_t* c)
{
    return nsi_client_close_impl(c, 0);
};


int nsi_client_try_close(nsi_client_t* c)
{
    int r;


    if (c->temp && 
        nai_list_is_empty(&c->servs) && 
        nai_list_is_empty(&c->events) && 
        nai_list_is_empty(&c->subs) && 
        nai_list_is_empty(&c->reqs)) {
        r = nsi_client_close(c);
    } else {
        r = 0;
    };

    return r;
};


int nsi_client_send(nsi_client_t* c, nsi_message_t* m, uint32_t uid, int flags)
{
    int r;
    int ec;
    nsi_app_t* a;
    nsi_client_t* s;
    nsi_message_t* d;
    nsi_routing_t* rt;


    rt = c->rt;
    if (rt->stat != NSI_RSTAT_OPENED) {
        goto _skip;
    };

    a = c->app;
    if (a != 0) {
        if (uid != 0) {
            if (a->uid == uid) {
                ec = 0;
                goto _sent;
            };

            a->uid = uid;
        };
        if (a->msgcb != 0) {
            d = m;
            if (flags & NSI_SEND_MSGDUP) {
                d = nsi_message_dup(m, 1);
                if (d == 0) {
                    ec = nai_errno;
                    nai_log_alert(NSI_LOG_CORE, ec, 
                        "the client(0x%x) failed to dupilcate message", 
                        c->cid);
                    goto _sent;
                };

                flags &= ~NSI_SEND_MSGDUP;
            };

            if (flags & NSI_SEND_POSTED) {
                /* posted */
                assert(rt->batch);
                d->cookie = c->cid;
                nai_list_insert_tail(&c->rt->posted, &d->ent);
                r = 0;
                goto _end;
            };

            if (m->hdr.type == NSI_MT_REQUEST || 
                m->hdr.type == NSI_MT_REQUEST_NO_RETURN) {
                /* do check before request */
                r = nsi_routing_grant_request(rt, d);
                if (r < 0) {
                    ec = nai_errno;
                    nai_log_debug(NSI_LOG_CORE, ec, 
                        "the request(%d) of service(%d, %d) "
                        "from the client(0x%x) is denied", 
                        m->hdr.method, m->hdr.serv, m->inst, m->hdr.client);
                    goto _sent;
                };
            };

            if (d == m) {
                /* add ref */
                nsi_message_ref(m);
            };

            /* app message callback */
            a->msgcb(a, a->ud, d);
            r = 0;
        };

        ec = 0;

    } else if (c->ep.ep != 0) {

        if (!(flags & NSI_SEND_POSTED) && (
            m->hdr.type == NSI_MT_REQUEST || 
            m->hdr.type == NSI_MT_REQUEST_NO_RETURN)) {
            /* do check before request */
            r = nsi_routing_grant_request(rt, m);
            if (r < 0) {
                ec = nai_errno;
                nai_log_debug(NSI_LOG_CORE, ec, 
                    "the request(%d) of service(%d, %d) "
                    "from the client(0x%x) is denied", 
                    m->hdr.method, m->hdr.serv, m->inst, m->hdr.client);
                goto _sent;
            };
        };

        flags |= NSI_SEND_POSTED;
        if (c->ep.ep == rt->ipc) {
            r = nsi_proxy_send(&c->ep.c, m, uid, flags);
        } else {
            m->conn = c->ep.conn;
            r = nsi_endpoint_send(c->ep.ep, m, uid, flags);
        };
        if (r >= 0) {
            goto _end;
        };

        ec = nai_errno;
        nai_log_alert(NSI_LOG_CORE, ec, 
            "the client(0x%x) send message failed", c->cid);

    } else {
        ec = 0;
    };


_sent:
    if (m->hdr.type == NSI_MT_REQUEST || 
        m->hdr.type == NSI_MT_REQUEST_NO_RETURN) {
        /* find the client who sent the message */
        s = nsi_client_find_in_routing(rt, m->hdr.client);
        if (s != 0) {
            nsi_client_handle_sent(s, m, ec);
        };
    };

_skip:
    (void)r;

    if (!(flags & NSI_SEND_MSGDUP)) {
        nsi_message_close(m);
    };
    r = 0;


_end:
    return r;
};


int nsi_client_handle_sent(nsi_client_t* c, nsi_message_t* m, int errcode)
{
    int r;
    nsi_app_t* a;
    nsi_routing_t* rt;


    rt = c->rt;
    if (rt->stat != NSI_RSTAT_OPENED) {
        r = 0;
        goto _end;
    };

    a = c->app;
    if (a == 0 || 
        a->sentcb == 0) {
        r = 0;
        goto _end;
    };

    a->sentcb(a, a->ud, m, errcode);
    r = 0;

_end:
    return r;
};


int nsi_client_handle_available(nsi_client_t* c, 
    int avail, nsi_serv_t serv, nsi_inst_t inst, 
    nsi_version_t major, nsi_versmin_t minor, int nopost)
{
    int r;
    int post;
    nsi_app_t* a;
    nsi_routing_t* rt;


    nai_log_debug(NSI_LOG_CORE, 0, 
        "service(%d, %d) notify client(0x%x) available(%d)", 
        serv, inst, c->cid, avail);

    rt = c->rt;
    if (rt->stat != NSI_RSTAT_OPENED) {
        r = 0;
        goto _end;
    };

    post = rt->batch;
    if ((avail != NSI_AVAIL_OFFLINED) && 
        (post == 0 || nopost)) {
        /* do check before notify */
        r = nsi_routing_grant_notify(
            rt, c, avail, serv, inst, major, minor);
        if (r < 0) {
            nai_log_debug(NSI_LOG_CORE, nai_errno, 
                "send notification of service(%d, %d) to "
                "the client(0x%x) is denied", 
                serv, inst, c->cid);
            r = 0;
            goto _end;
        };
    };

    a = c->app;
    if (a != 0) {
        if (a->availcb == 0) {
            r = 0;
            goto _end;
        };

        /* don't callback if in batch-mode */
        if (post == 0 || nopost) {
            a->availcb(a, a->ud, avail, serv, inst, major, minor, &c->cred);
            r = 0;
            goto _end;
        };
    };


    if (c->ep.ep && c->remote == 0) {
        r = nsi_proxy_available(&c->ep.c, 
            avail, c, serv, inst, major, minor);
    } else {
        r = 0;
    };


_end:
    return r;
};


int nsi_client_handle_request(nsi_client_t* c, 
    int op, nsi_client_t* q, nsi_serv_t serv, nsi_inst_t inst, 
    nsi_version_t major, nsi_versmin_t minor, int nopost)
{
    int r;
    nsi_app_t* a;
    nsi_routing_t* rt;


    (void)nopost;

    rt = c->rt;
    if (rt->stat != NSI_RSTAT_OPENED) {
        r = 0;
        goto _end;
    };

    a = c->app;
    if (a != 0) {
        r = 0;
        goto _end;
    };

    /* we send message if c and d from different connection */
    if (c->ep.ep && c->remote == 0 && 
        !nsi_endpoint_conn_equal(&c->ep, &q->ep)) {
        r = nsi_proxy_request(&c->ep.c, 
            op, c, q, serv, inst, major, minor);
    } else {
        r = 0;
    };


_end:
    return r;
};



int nsi_client_handle_event(nsi_client_t* c, 
    int op, nsi_client_t* q, nsi_serv_t serv, nsi_inst_t inst, 
    nsi_eid_t eid, const nsi_event_info_t* info, int nopost)
{
    int r;
    nsi_app_t* a;
    nsi_routing_t* rt;


    (void)nopost;

    rt = c->rt;
    if (rt->stat != NSI_RSTAT_OPENED) {
        r = 0;
        goto _end;
    };

    a = c->app;
    if (a != 0) {
        r = 0;
        goto _end;
    };

    /* we send message if c and d from different connection */
    if (c->ep.ep && c->remote == 0 && 
        !nsi_endpoint_conn_equal(&c->ep, &q->ep)) {
        r = nsi_proxy_event(&c->ep.c, 
            op, c, q, serv, inst, eid, info);
    } else {
        r = 0;
    };


_end:
    return r;
};


int nsi_client_handle_subscribe(nsi_client_t* c, 
    int op, nsi_client_t* q, nsi_serv_t serv, nsi_inst_t inst, 
    nsi_gid_t gid, nsi_version_t major, nsi_eid_t eid, 
    uint32_t ttl, int nopost)
{
    int r;
    int post;
    int sult;
    nsi_app_t* a;
    nsi_service_t* s;
    nsi_routing_t* rt;


    rt = c->rt;
    if (rt->stat != NSI_RSTAT_OPENED) {
        r = 0;
        goto _end;
    };

    post = rt->batch;
    if ((op == NSI_SERVICE_OP_ADD) && 
        (post == 0 || nopost)) {
        /* do check before subscribe */
        r = nsi_routing_grant_subscribe(rt, q, serv, inst, gid, eid);
        if (r < 0) {
            nai_log_debug(NSI_LOG_CORE, nai_errno, 
                "the client(0x%x) subscribe "
                "event group(%d) of service(%d, %d) is denied", 
                c->cid, gid, serv, inst);

            sult = NSI_E_NOT_OK;
            goto _ack;
        };
    };

    a = c->app;
    if (a != 0) {
        if (op != NSI_SERVICE_OP_ADD) {
            r = 0;
            goto _end;
        };

        /* don't callback if in batch-mode */
        if (post == 0 || nopost) {
            if (a->subcb != 0) {
                a->subcb(a, a->ud, q->cid, serv, inst, gid, eid, &q->cred);
                r = 0;
                goto _end;
            } else {
                sult = 0;
                goto _ack;
            };
        };
    };


    /* we send message if c and d from different connection */
    if (c->ep.ep && c->remote == 0 && 
        !nsi_endpoint_conn_equal(&c->ep, &q->ep)) {
        r = nsi_proxy_subscribe(&c->ep.c, 
            op, c, q, serv, inst, gid, major, eid, ttl);
    } else {
        r = 0;
    };


_end:
    return r;

_ack:
    s = nsi_service_find_in_routing(c->rt, serv, inst);
    if (s == 0) {
        r = 0;
        goto _end;
    };

    r = nsi_service_subscribe_ack(
        s, q, sult, gid, major, eid, ttl, 0);
    goto _end;
};


int nsi_client_handle_subscribe_ack(nsi_client_t* c, 
    int sult, nsi_serv_t serv, nsi_inst_t inst, 
    nsi_gid_t gid, nsi_version_t major, nsi_eid_t eid, uint32_t ttl, 
    int init, int nopost)
{
    int r;
    int post;
    nsi_app_t* a;
    nsi_discovery_t* sd;
    nsi_routing_t* rt;


    assert(sult == 0 || ttl == 0);

    rt = c->rt;
    if (rt->stat != NSI_RSTAT_OPENED) {
        r = 0;
        goto _end;
    };

    a = c->app;
    if (a != 0) {
        if (a->ackcb == 0) {
            r = 0;
            goto _end;
        };

        /* don't callback if in batch-mode */
        post = rt->batch;
        if (post == 0 || nopost) {
            a->ackcb(a, a->ud, sult, serv, inst, gid, eid);
            r = 0;
            goto _end;
        };
    };


    if (c->ep.ep && c->remote == 0) {
        r = nsi_proxy_subscribe_ack(&c->ep.c, 
            sult, c, serv, inst, gid, major, eid, ttl);
    } else {
        /* a remote client from discovery */
        sd = c->rt->sd;
        if (sd != 0) {
            r = nsi_discovery_subscribe_ack(
                sd, sult, c->cid, serv, inst, gid, major, ttl, init);
        } else {
            r = 0;
        };
    };


_end:
    return r;
};


