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
/// @file       nsi_service_open.c
/// @brief
/// @details
/// @date       2021-05-20
/// @author     xn
/// @version    1.2.0
///
/// ================================================================

#include "nsi_service.h"
#include "nsi_client.h"
#include "nsi_routing_impl.h"
#include "nsi_log.h"
#include "nsomeip/net/nsi_message.h"
#include "nai/runtime/nai_errno.h"
#include "nai/runtime/nai_util.h"



static nai_rbnode_t** nsi_service_find_impl(
    nsi_routing_t* p, nsi_serv_t serv, nsi_inst_t inst, 
    nai_rbnode_t** pparent)
{
    nai_rbnode_t** n = &nai_rbtree_root(&p->servs);
    nai_rbnode_t* parent = nai_rbtree_end(&p->servs);
    nsi_service_t* e;
    nsi_servkey_t v;


    v.serv = serv;
    v.inst = inst;

    while (*n) {
        parent = *n;
        e = (nsi_service_t*)parent;
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


static nai_rbnode_t** nsi_service_find_impl2(
    nsi_routing_endpoint_t* p, nsi_serv_t serv, nsi_inst_t inst, 
    nai_rbnode_t** pparent)
{
    nai_rbnode_t** n = &nai_rbtree_root(&p->servs);
    nai_rbnode_t* parent = nai_rbtree_end(&p->servs);
    nsi_service_t* e;
    nsi_service_conn_t* c;
    nsi_servkey_t v;


    if (inst) {
        /* for proxy, 
         * multi instances of same service 
         * must only be bound to the proxy endpoint.
         */
        v.serv = serv;
        v.inst = inst;

        while (*n) {
            parent = *n;
            c = nai_containof(parent, nsi_service_conn_t, ente);
            e = nai_containof(c, nsi_service_t, ep[c->idx]);
            if (e->servinst == v.servinst) {
                break;
            } else if (e->servinst >= v.servinst) {
                n = &parent->rb_left;
            } else {
                n = &parent->rb_right;
            };
        };
    } else {
        /* for others, 
         * only one instance can be bound to the non-proxy endpoint.
         */
        while (*n) {
            parent = *n;
            c = nai_containof(parent, nsi_service_conn_t, ente);
            e = nai_containof(c, nsi_service_t, ep[c->idx]);
            if (e->serv == serv) {
                break;
            } else if (e->serv >= serv) {
                n = &parent->rb_left;
            } else {
                n = &parent->rb_right;
            };
        };
    };

    if (pparent) {
        pparent[0] = parent;
    };

    return n;
};


static nai_rbnode_t** nsi_service_find_mcast(
    nsi_routing_endpoint_t* p, nsi_serv_t serv, nai_rbnode_t** pparent)
{
    nai_rbnode_t** n = &nai_rbtree_root(&p->mcasts);
    nai_rbnode_t* parent = nai_rbtree_end(&p->mcasts);
    nsi_mcast_conn_t* c;


    while (*n) {
        parent = *n;
        c = nai_containof(parent, nsi_mcast_conn_t, ente);
        if (c->serv == serv) {
            break;
        } else if (c->serv >= serv) {
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


extern nsi_service_ops_t nsi_service_client_ops;
extern nsi_service_ops_t nsi_service_server_ops;
extern nsi_service_ops_t nsi_service_cproxy_ops;
extern nsi_service_ops_t nsi_service_sproxy_ops;


static int nsi_service_init(nsi_service_t* s, 
    nsi_routing_t* p, nsi_serv_t serv, nsi_inst_t inst)
{
    int r;
    int n;


    s->rt = p;
    s->flags = 0;
    s->serv = serv;
    s->inst = inst;
    s->major = NSI_MAJOR_ANY;
    s->minor = NSI_MINOR_ANY;
    s->ops = 0;
    s->ctx = 0;
    s->own = 0;
    nai_list_init(&s->refs);
    nai_rbtree_init(&s->events);
    nai_rbtree_init(&s->groups);

    for (n = 0; n < (int)nai_countof(s->ep); n ++) {
        s->ep[n].idx = n;
        s->ep[n].ep  = 0;
        s->ep[n].conn = 0;
        s->ep[n].stat = 0;
    };

    r = 0;

    return r;
};


static int nsi_service_open_impl(nsi_service_t* s)
{
    int r;
    nsi_service_info_t si;


    if (nsi_service_is_opened(s)) {
        r = 0;
        goto _end;
    };


    /* query service info */
    r = nsi_routing_get_service_info(s->rt, s->serv, s->inst, &si, 1);
    if (r < 0) {
        s->local = nsi_routing_is_local_service(s->rt, s->serv, s->inst);
    } else {
        s->local = si.local != 0;
    };


    /* select operation */
    s->proxy = s->rt->type == NSI_RTYPE_PROXY;
    if (s->proxy) {
        if (s->local == 1) {
            s->ops = &nsi_service_sproxy_ops;
        } else {
            s->ops = &nsi_service_cproxy_ops;
        };
    } else {
        if (s->local == 1) {
            s->ops = &nsi_service_server_ops;
        } else {
            s->ops = &nsi_service_client_ops;
        };
    };

    s->tls = &si.tls;

    /* open */
    r = s->ops->open(s);
    if (r < 0) {
        s->ops = 0;
        nai_log_error(NSI_LOG_CORE, nai_errno, 
            "open service(%d, %d) failed", s->serv, s->inst);
        goto _end;
    };


_end:
    return r;
};


static int nsi_service_free_events(nsi_service_t* s)
{
    int r;
    nai_rbnode_t* node;
    nai_list_entry_t* ent;
    nsi_event_t* e;
    nsi_event_ref_t* f;
    nsi_event_link_t* l;


    /* for each events */
    node = nai_rbtree_begin(&s->events);
    for ( ; node != nai_rbtree_end(&s->events); ) {
        e = nai_containof(node, nsi_event_t, ent);
        node = nai_rbtree_next(node);


        /* remove and free all event link */
        ent = e->list.next;
        for ( ; ent != &e->list; ) {
            l = nai_containof(ent, nsi_event_link_t, ente);
            ent = ent->next;
            nai_list_entry_remove(&l->ente);
            nai_list_entry_remove(&l->entg);
            nsi_routing_free(s->rt, l);
        };


        /* remove and free all event ref */
        ent = e->refs.next;
        for ( ; ent != &e->refs; ) {
            f = nai_containof(ent, nsi_event_ref_t, ente);
            ent = ent->next;
            nai_list_entry_remove(&f->entc);
            nai_list_entry_remove(&f->ente);
            nsi_routing_free(s->rt, f);

            /* call implemention */
            if (nsi_service_is_opened(s)) {
                r = s->ops->event(s, 
                    NSI_SERVICE_OP_REMOVE, f->client, e->eid, 0);
                if (r < 0) {
                    nai_log_error(NSI_LOG_CORE, nai_errno, 
                        "service(%d, %d) "
                        "failed to remove event of client(0x%x) when closing", 
                        s->serv, s->inst, f->client->cid);
                };
            };
        };

        /* free event */
        nai_rbtree_erase(&s->events, &e->ent);
        nsi_service_free_event(s, e);
    };

    r = 0;


    return r;
};


static int nsi_service_free_groups(nsi_service_t* s)
{
    int r;
    nai_rbnode_t* node;
    nai_list_entry_t* ent;
    nsi_event_group_t* g;
    nsi_event_link_t* l;
    nsi_subscription_t* u;


    /* for each events */
    node = nai_rbtree_begin(&s->groups);
    for ( ; node != nai_rbtree_end(&s->groups); ) {
        g = nai_containof(node, nsi_event_group_t, ent);
        node = nai_rbtree_next(node);


        /* remove and free all event link */
        ent = g->list.next;
        for ( ; ent != &g->list; ) {
            l = nai_containof(ent, nsi_event_link_t, entg);
            ent = ent->next;
            nai_list_entry_remove(&l->ente);
            nai_list_entry_remove(&l->entg);
            nsi_routing_free(s->rt, l);
        };


        /* remove and free all subscription */
        ent = g->subs.next;
        for ( ; ent != &g->subs; ) {
            u = nai_containof(ent, nsi_subscription_t, entg);
            ent = ent->next;

            /* call implemention */
            if (nsi_service_is_onlined(s) || 
                nsi_service_is_proxy(s)) {
                r = s->ops->subscribe(s, 
                    NSI_SERVICE_OP_REMOVE_LAST, 
                    u->client, g->gid, u->major, NSI_EVENT_ANY, 0);
                if (r < 0) {
                    nai_log_error(NSI_LOG_CORE, nai_errno, 
                        "service(%d, %d) "
                        "failed to unsubscribe group when closing", 
                        s->serv, s->inst);
                };
            };
 
            /* clear */
            nai_list_entry_remove(&u->entc);
            nai_list_entry_remove(&u->entg);
            nsi_service_free_subscription(s, u);
        };

        /* free eventgroup */
        nai_rbtree_erase(&s->groups, &g->ent);
        nsi_service_free_group(s, g);
    };

    r = 0;


    return r;
};


static int nsi_service_free_clients(nsi_service_t* s)
{
    int r;
    int last;
    nai_list_entry_t* ent;
    nsi_service_ref_t* f;


    ent = s->refs.next;
    for ( ; ent != &s->refs; ) {
        f = nai_containof(ent, nsi_service_ref_t, ents);
        ent = ent->next;

        /* call implemention */
        if (nsi_service_is_opened(s)) {
            if (!f->offer) {
                last = ent == &s->refs;
                r = s->ops->request(s, 
                    last ? NSI_SERVICE_OP_REMOVE_LAST : NSI_SERVICE_OP_REMOVE, 
                    f->client, f->major, f->minor);
                if (r < 0) {
                    nai_log_error(NSI_LOG_CORE, nai_errno, 
                        "service(%d, %d) failed to "
                        "release requesting of client(0x%x) when closing", 
                        s->serv, s->inst, f->client->cid);
                };
            } else {
                assert(s->own == f);
                s->own = 0;
                r = s->ops->stop(s, f->client);
                if (r < 0) {
                    nai_log_error(NSI_LOG_CORE, nai_errno, 
                        "service(%d, %d) failed to "
                        "stop offer of client(0x%x) when closing", 
                        s->serv, s->inst, f->client->cid);
                };
            };
        };

        /* free service ref */
        nai_list_entry_remove(&f->ents);
        nai_list_entry_remove(&f->entc);
        nsi_routing_free(s->rt, f);
    };

    r = 0;


    return r;
};


static int nsi_service_insert_mcast(
    nsi_endpoint_t* ep, nsi_event_group_t* g)
{
    int r;
    nsi_routing_endpoint_t* e = nsi_routing_get_endpoint_ctx(ep);
    nai_rbnode_t** n = &nai_rbtree_root(&e->mcasts);
    nai_rbnode_t* parent = nai_rbtree_end(&e->mcasts);
    nsi_serv_t serv = g->ep.serv;
    nsi_mcast_conn_t* c;
    nsi_event_group_t* t;


    while (*n) {
        parent = *n;
        c = nai_containof(parent, nsi_mcast_conn_t, ente);
        if (c->serv == serv) {
            t = nai_containof(c, nsi_event_group_t, ep);
            if (t->serv != g->serv) {
                nai_errno = EEXIST;
                r = -1;
                goto _end;
            };
            n = &parent->rb_left;
        } else if (c->serv >= serv) {
            n = &parent->rb_left;
        } else {
            n = &parent->rb_right;
        };
    };

    nai_rbtree_link(&e->mcasts, &g->ep.ente, parent, n);
    nai_rbtree_color(&e->mcasts, &g->ep.ente);
    r = 0;

_end:
    return r;
};


static int nsi_service_bind_send_mcast(nsi_service_t* s)
{
    int r;
    nai_rbnode_t* n;
    nsi_event_group_t* g;
    nsi_group_info_t gi;


    /* bind groups */
    n = nai_rbtree_begin(&s->groups);
    for ( ; n != nai_rbtree_end(&s->groups); ) {
        g = nai_containof(n, nsi_event_group_t, ent);
        n = nai_rbtree_next(n);

        if (g->mcast) {
            continue;
        };

        r = nsi_routing_get_group_info(
            s->rt, s->serv, s->inst, g->gid, &gi);
        if (r < 0) {
            gi.mcast.addr = 0;
            gi.mcast.len = 0;
        };
        if (!gi.mcast.addr) {
            nai_log_debug(NSI_LOG_CORE, r < 0 ? nai_errno : 0, 
                "group(%d) of service(%d, %d) without multicast address", 
                g->gid, s->serv, s->inst);
        };

        /* PRS_SOMEIPSD_00329
         * PRS_SOMEIPSD_00336
         */
        r = nsi_service_bind_name(
            s->ep[0].ep, NSI_EOPT_BIND_NAME, &gi.mcast, &g->mcast);
        if (r < 0) {
            nai_log_error(NSI_LOG_CORE, nai_errno, 
                "service(%d, %d) bind "
                "multicast address of group(%d) failed", 
                s->serv, s->inst, g->gid);
            goto _end;
        };
    };

    r = 0;

_end:
    return r;
};


static int nsi_service_unbind_send_mcast(nsi_service_t* s)
{
    int r;
    nai_rbnode_t* n;
    nsi_event_group_t* g;


    /* unbind groups multicast */
    n = nai_rbtree_begin(&s->groups);
    for ( ; n != nai_rbtree_end(&s->groups); ) {
        g = nai_containof(n, nsi_event_group_t, ent);
        n = nai_rbtree_next(n);

        if (g->mcast) {
            r = nsi_endpoint_set_opt(
                s->ep[0].ep, NSI_EOPT_UNBIND_NAME, g->mcast);
            if (r < 0) {
                nai_log_error(NSI_LOG_CORE, nai_errno, 
                    "service(%d, %d) "
                    "unbind multicast address of group(%d) failed", 
                    s->serv, s->inst, g->gid);
            };

            g->mcast = 0;
        };
    };

    r = 0;

    return r;
};



nsi_service_t* nsi_service_find_in_routing(
    nsi_routing_t* p, nsi_serv_t serv, nsi_inst_t inst)
{
    nai_rbnode_t** n;
    nsi_service_t* r;


    n = nsi_service_find_impl(p, serv, inst, 0);
    if (n[0] == 0) {
        nai_errno = ENOENT;
        r = 0;
        goto _end;
    };

    r = nai_containof(n[0], nsi_service_t, entr);

_end:
    return r;
};


nsi_service_t* nsi_service_find_in_endpoint(
    nsi_endpoint_t* ep, nsi_serv_t serv, nsi_inst_t inst)
{
    nsi_service_t* r;
    nai_rbnode_t** n;
    nsi_routing_endpoint_t* e;
    nsi_service_conn_t* c;
    nsi_event_group_t* g;


    e = nsi_routing_get_endpoint_ctx(ep);
    if (e == 0) {
        nai_errno = ENOENT;
        r = 0;
        goto _end;
    };

    if (inst == 0 && ep == e->rt->ipc) {
        /* instance id should be provided, 
         * multi instances of same service 
         * must only be bound to the proxy endpoint.
         */
        assert(0);
        nai_errno = ENOENT;
        r = 0;
        goto _end;
    };

    n = nsi_service_find_impl2(e, serv, inst, 0);
    if (n[0] != 0) {

        c = nai_containof(n[0], nsi_service_conn_t, ente);
        r = nai_containof(c, nsi_service_t, ep[c->idx]);

    } else {

        if (ep->reliable) {
            nai_errno = ENOENT;
            r = 0;
            goto _end;
        };

        if (inst != 0) {
            /* only one instance can be bound to the non-proxy endpoint.
             * so valid instance id lookup is not supported.
             */
            assert(0);
            nai_errno = ENOENT;
            r = 0;
            goto _end;
        };

        n = nsi_service_find_mcast(e, serv, 0);
        if (n[0] == 0) {
            nai_errno = ENOENT;
            r = 0;
            goto _end;
        };

        g = nai_containof(n[0], nsi_event_group_t, ep.ente);
        r = g->serv;
    };

_end:
    return r;
};


nsi_service_t* nsi_service_open(
    nsi_routing_t* p, nsi_serv_t serv, nsi_inst_t inst, int na)
{
    int r;
    int ec;
    nai_rbnode_t** n;
    nai_rbnode_t* parent;
    nsi_service_t* s;


    n = nsi_service_find_impl(p, serv, inst, &parent);
    s = (nsi_service_t*)n[0];
    if (s != 0) {
        goto _end;
    };

    if (na) {
        nai_errno = ENOENT;
        goto _end;
    };


    s = (nsi_service_t*)nsi_routing_alloc(p, sizeof(*s));
    if (s == 0) {
        nai_log_alert(NSI_LOG_CORE, 
            nai_errno, "allocate a service failed");
        goto _end;
    };

    nsi_service_init(s, p, serv, inst);
    r = nsi_service_open_impl(s);
    if (r < 0) {
        ec = nai_errno;
        nai_log_error(NSI_LOG_CORE, ec, "open service failed");
        nsi_routing_free(p, s);
        nai_errno = ec;
        s = 0;
        goto _end;
    };

    nai_rbtree_link(&p->servs, &s->entr, parent, n);
    nai_rbtree_color(&p->servs, &s->entr);


_end:
    return s;
};


int nsi_service_reopen(nsi_service_t* s)
{
    int r;
    int ec;
    uint16_t local;
    nsi_routing_t* p;
    nsi_service_info_t si;


    if (nsi_service_is_proxy(s)) {
        /* ignore proxy service */
        r = 0;
        goto _end;
    };

    nai_log_debug(NSI_LOG_CORE, 0, 
        "reopen service(%d, %d).", s->serv, s->inst);

    r = nsi_routing_get_service_info(s->rt, s->serv, s->inst, &si, 1);
    if (r < 0) {
        local = nsi_routing_is_local_service(s->rt, s->serv, s->inst);
    } else {
        local = si.local != 0;
    };

    if (s->local == local) {
        /* no changed */
        nai_log_debug(NSI_LOG_CORE, 0, 
            "%s service(%d, %d) isn't changed, do nothing", 
            local ? "local" : "remote", s->serv, s->inst);
        r = 0;
        goto _end;
    };

    /* the local service should be had no owner */
    if (s->local && s->own) {
        assert(0);
        nai_errno = EPERM;
        r = -1;
        goto _end;
    };

    nai_log_debug(NSI_LOG_CORE, 0, 
        "%s service(%d, %d) is changed, reopen it.", 
        local ? "local" : "remote", s->serv, s->inst);

    p = s->rt;
    r = nsi_routing_batch_begin(p);
    if (r < 0) {
        nai_log_alert(NSI_LOG_CORE, 
            nai_errno, "start batch failed");
        goto _end;
    };

    /* close the service */
    r = s->ops->close(s);
    if (r < 0) {
        nai_log_alert(NSI_LOG_CORE, 
            nai_errno, "close service failed");
        goto _fail;
    };

    /* change the implemention */
    s->local = local;
    s->ops = local ? &nsi_service_server_ops : &nsi_service_client_ops;

    /* open the service */
    r = s->ops->open(s);
    if (r < 0) {
        nai_log_alert(NSI_LOG_CORE, 
            nai_errno, "open service failed");
        goto _fail;
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


int nsi_service_close(nsi_service_t* s)
{
    int r;
    int ec;
    nsi_routing_t* p;


    p = s->rt;
    r = nsi_routing_batch_begin(p);
    if (r < 0) {
        nai_log_alert(NSI_LOG_CORE, 
            nai_errno, "start batch failed");
        goto _end;
    };


    /* PRS_SOMEIPSD_00432 */

    /* close events */
    nsi_service_free_events(s);

    /* close groups */
    nsi_service_free_groups(s);

    /* close clients */
    nsi_service_free_clients(s);


    /* do close */
    switch (s->stat) {
    case NSI_SERVICE_CLOSED:
        r = 0;
        break;
    default:
        r = s->ops->close(s);
        if (r < 0) {
            nai_log_error(NSI_LOG_CORE, 
                nai_errno, "close service failed");
            goto _fail;
        };
        break;
    };

    nai_rbtree_erase(&p->servs, &s->entr);
    nsi_routing_free(p, s);
    r = 0;

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


int nsi_service_bind_name(nsi_endpoint_t* ep, 
    int op, const nsi_endpoint_name_t* name, nsi_connid_t* cid)
{
    int r;
    nsi_connid_t oldone;
    nsi_connid_t newone;


    if (ep == 0) {
        cid[0] = 0;
        r = 0;
        goto _end;
    };

    newone = 0;
    if (name != 0 && name->addr != 0) {
        newone = nsi_endpoint_set_opt(ep, op, (intptr_t)name);
        if (newone == (nsi_connid_t)-1) {
            newone = 0;
            nai_log_error(NSI_LOG_CORE, 
                nai_errno, "bind new address failed");
        };
    };

    oldone = cid[0];
    if (oldone) {
        r = nsi_endpoint_set_opt(
            ep, NSI_EOPT_UNBIND_NAME, oldone);
        if (r < 0) {
            nai_log_error(NSI_LOG_CORE, 
                nai_errno, "unbind old address failed");
        };
    };

    cid[0] = newone;
    r = 0;

_end:
    return r;
};


int nsi_service_bind_mcast(nsi_service_t* s, 
    nsi_event_group_t* g, const nsi_endpoint_name_t* mcast)
{
    int r;
    int ec;
    nsi_routing_t* rt;
    nsi_routing_endpoint_t* e;
    nsi_mcast_conn_t* c;
    nsi_endpoint_t* ep;
    nsi_connid_t conn;
    nsi_endpoint_name_t name;

#if 0
    int port;
    nai_socknbuf_in_t nbuf;
    const nai_sockaddr_info_t* si;
#endif


    nai_log_debug(NSI_LOG_CORE, 0, 
        "service(%d, %d) %s "
        "multicast endpoint of subscribe(%d)", 
        s->serv, s->inst, mcast ? "bind" : "unbind", g->gid);

    if (mcast == 0 || mcast->addr == 0) {
        ep = 0;
        conn = 0;
    } else {
        rt = s->rt;
        name = *mcast;

#if 0
        /* cannot bind local address on linux */
        /* build local address with port of multicast */
        if (rt->conf) {
            /* get port of multicast */
            si = nai_sockaddr_info(name.addr->sa_family);
            port = nai_sockaddr_get_port(si, &name);

            /* make local address */
            name = rt->conf->host;
            nbuf.len = name.len;
            nai_memcpy(&nbuf.addr, name.addr, name.len);
            nai_sockname_setbuf(&name, &nbuf);
            si = nai_sockaddr_info(name.addr->sa_family);
            nai_sockaddr_set_port(si, &nbuf.addr, port);
        };
#endif

        /* create endpoint */
        ep = nsi_routing_create_server(rt, &name, 0, 0, s->tls);
        if (ep == 0) {
            nai_log_error(NSI_LOG_CORE, nai_errno, 
                "service(%d, %d) open "
                "multicast endpoint of subscribe(%d) failed", 
                s->serv, s->inst, g->gid);
            r = -1;
            goto _end;
        };

        /* join multicast */
        conn = 0;
        r = nsi_service_bind_name(ep, NSI_EOPT_BIND_MCAST, mcast, &conn);
        if (r < 0) {
            nai_log_error(NSI_LOG_CORE, nai_errno, 
                "service(%d, %d) bind "
                "multicast address of subscribe(%d) failed", 
                s->serv, s->inst, g->gid);
            goto _fail;
        };
    };

    c = &g->ep;
    if (c->ep == ep) {
        if (c->ep != 0) {
            if (c->conn != conn) {
                r = nsi_service_bind_name(c->ep, 0, 0, &c->conn);
                if (r < 0) {
                    nai_log_error(NSI_LOG_CORE, nai_errno, 
                        "service(%d, %d) unbind "
                        "multicast address of subscribe(%d) failed", 
                        s->serv, s->inst, g->gid);
                };

                c->conn = conn;
            };

            nsi_routing_release_endpoint(c->ep);
        };
    } else {
        if (c->ep != 0) {
            if (c->conn != 0) {
                r = nsi_service_bind_name(c->ep, 0, 0, &c->conn);
                if (r < 0) {
                    nai_log_error(NSI_LOG_CORE, nai_errno, 
                        "service(%d, %d) unbind "
                        "multicast address of subscribe(%d) failed", 
                        s->serv, s->inst, g->gid);
                };
            };

            e = nsi_routing_get_endpoint_ctx(c->ep);
            assert(e != 0);

            nai_rbtree_erase(&e->mcasts, &c->ente);
            nsi_routing_release_endpoint(c->ep);
            c->ep = 0;
        };

        if (ep != 0) {
            r = nsi_service_insert_mcast(ep, g);
            if (r < 0) {
                nai_errno = EEXIST;
                nai_log_alert(NSI_LOG_CORE, EEXIST, 
                    "an other instance of service(%d) "
                    "already bind the multicast endpoint", s->serv);
                r = -1;
                goto _end;
            };

            c->ep = ep;
            c->conn = conn;
        };
    };

    r = 0;

_end:
    return r;

_fail:
    ec = nai_errno;
    if (conn != 0) {
        nsi_endpoint_set_opt(ep, NSI_EOPT_UNBIND_NAME, conn);
    };
    nsi_routing_release_endpoint(ep);
    nai_errno = ec;
    goto _end;
};


int nsi_service_bind_endpoint(
    nsi_service_t* s, int idx, nsi_endpoint_t* ep, nsi_connid_t cid)
{
    int r;
    nai_rbnode_t** n;
    nai_rbnode_t* parent;
    nsi_routing_endpoint_t* e;
    nsi_endpoint_t* cp;
    nsi_service_conn_t* c;


    c = &s->ep[idx];

    cp = c->ep;
    if (cp == ep) {
        if (ep != 0) {
            nsi_routing_release_endpoint(ep);
        };
        r = 0;
        goto _end;
    };

    if (cp != 0) {
        nai_log_debug(NSI_LOG_CORE, 0, 
            "service(%d, %d) unbind endpoint(%d)", s->serv, s->inst, idx);

        if (idx == 0) {
            nsi_service_unbind_send_mcast(s);
        };

        e = nsi_routing_get_endpoint_ctx(cp);
        assert(e != 0);

        nai_rbtree_erase(&e->servs, &c->ente);
        c->ep = 0;
        c->conn = 0;
        c->stat = 0;

        nsi_routing_release_endpoint(cp);
    };

    if (ep) {
        nai_log_debug(NSI_LOG_CORE, 0, 
            "service(%d, %d) bind a new endpoint(%d)", s->serv, s->inst, idx);
        e = nsi_routing_get_endpoint_ctx(ep);
        assert(e != 0);


        /* PRS_SOMEIPSD_00238
         * PRS_SOMEIPSD_00239
         * PRS_SOMEIPSD_00481
         * PRS_SOMEIPSD_00806
         */
        n = nsi_service_find_impl2(
            e, s->serv, s->proxy ? s->inst : 0, &parent);
        if (n[0] != 0) {
            nai_errno = EEXIST;
            nai_log_alert(NSI_LOG_CORE, EEXIST, 
                "service(%d, %d) already bind the endpoint", 
                s->serv, s->inst);
            r = -1;
            goto _end;
        };

        c->ep = ep;
        if (idx == 0) {
            r = nsi_service_bind_send_mcast(s);
            if (r < 0) {
                c->ep = 0;
                nai_log_alert(NSI_LOG_CORE, nai_errno, 
                    "service(%d, %d) bind multicast address of groups failed", 
                    s->serv, s->inst);
                r = -1;
                goto _end;
            };
        };

        c->conn = cid;
        c->stat = nsi_endpoint_set_opt(
            ep, NSI_EOPT_CHECK_CSTAT, cid) == 1;
        nai_rbtree_link(&e->servs, &c->ente, parent, n);
        nai_rbtree_color(&e->servs, &c->ente);
    };

    r = 0;

_end:
    return r;
};


int nsi_service_unbind_endpoints(nsi_service_t* s)
{
    int r;
    int n;
    nsi_endpoint_t* ep[2];
    nsi_service_conn_t* c;
    nsi_routing_endpoint_t* e;


    /* we unneed unbind connecions of subscription, 
     * them already were free in nsi_service_available(NSI_AVAIL_OFFLINED)
     */

    /* unbind mcast */
    if (s->ep[0].ep != 0) {
        nsi_service_unbind_send_mcast(s);
    };

    /* unlink */
    for (n = 0; n < (int)nai_countof(s->ep); n ++) {
        c = &s->ep[n];
        ep[n] = c->ep;
        if (ep[n] == 0) {
            continue;
        };

        e = nsi_routing_get_endpoint_ctx(ep[n]);
        assert(e != 0);

        nai_rbtree_erase(&e->servs, &c->ente);
        c->ep = 0;
        c->conn = 0;
    };

    /* release */
    for (n = 0; n < (int)nai_countof(s->ep); n ++) {
        if (ep[n] == 0) {
            continue;
        };

        nsi_routing_release_endpoint(ep[n]);
    };

    r = 0;


    return r;
};


int nsi_service_free_event(nsi_service_t* s, nsi_event_t* e)
{
    int r;


    if (e->msg) {
        nsi_message_close(e->msg);
        e->msg = 0;
    };

    /* free */
    nsi_routing_free(s->rt, e);
    r = 0;

    return r;
};


int nsi_service_free_group(nsi_service_t* s, nsi_event_group_t* g)
{
    int r;


    if (g->mcast) {
        r = nsi_endpoint_set_opt(
            s->ep[0].ep, NSI_EOPT_UNBIND_NAME, g->mcast);
        if (r < 0) {
            nai_log_error(NSI_LOG_CORE, nai_errno, 
                "service(%d, %d) unbind "
                "multicast address of group(%d) failed", 
                s->serv, s->inst, g->gid);
        };

        g->mcast = 0;
    };

    /* remove mcast name */
    nsi_service_bind_mcast(s, g, 0);

    /* free */
    nsi_routing_free(s->rt, g);
    r = 0;

    return r;
};


int nsi_service_free_subscription(
    nsi_service_t* s, nsi_subscription_t* u)
{
    int r;
    int n;
    nsi_event_group_t* g;


    /* remove */
    if (u->events) {
        nsi_routing_free(s->rt, u->events);
        u->events = 0;
        u->count = 0;
        u->total = 0;
    };

    /* remove bind name */
    for (n = 0; n < (int)nai_countof(u->conn); n ++) {
        if (u->conn[n] == 0) {
            continue;
        };

        if (s->ep[n].ep) {
            r = nsi_endpoint_set_opt(
                s->ep[n].ep, NSI_EOPT_UNBIND_NAME, u->conn[n]);
            if (r < 0) {
                nai_log_error(NSI_LOG_CORE, nai_errno, 
                    "service(%d, %d) unbind subscribe address(%d) failed", 
                    s->serv, s->inst, n);
            };
        };
        u->conn[n] = 0;
    };


    g = u->group;
    if (u->ar) { /* sub an active remote client */
        g->remote --;
    };
    if (u->ac) { /* sub an active client */
        g->count --;
        if (g->count <= 0) {
            r = nsi_service_bind_mcast(s, g, 0);
            if (r < 0) {
                nai_log_error(NSI_LOG_CORE, nai_errno, 
                    "unbind multicast address "
                    "from subscribe group(%d) in service(%d, %d)", 
                    g->gid, s->serv, s->inst);
            };
        };
    };

    /* free */
    nsi_routing_free(s->rt, u);
    r = 0;

    return r;
};


