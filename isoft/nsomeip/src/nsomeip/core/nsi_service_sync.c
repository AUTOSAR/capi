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
/// @file       nsi_service_sync.c
/// @brief
/// @details
/// @date       2021-05-20
/// @author     xn
/// @version    1.2.0
///
/// ================================================================

#include "nsi_proxy.h"
#include "nsi_service.h"
#include "nsi_client.h"
#include "nsi_routing_impl.h"
#include "nsi_log.h"
#include "nsomeip/net/nsi_message.h"
#include "nai/runtime/nai_array.h"
#include "nai/runtime/nai_errno.h"
#include "nai/runtime/nai_util.h"



static int nsi_service_sync_requests(nsi_service_t* s)
{
    int r;
    int ec;
    nsi_service_ref_t* f;
    nai_list_entry_t* entl;


    entl = s->refs.next;
    for ( ; entl != &s->refs; ) {
        f = nai_containof(entl, nsi_service_ref_t, ents);
        entl = entl->next;

        /* call implemention */
        if (nsi_service_is_opened(s) && !f->offer) {
            r = s->ops->request(
                s, NSI_SERVICE_OP_ADD, f->client, f->major, f->minor);
            if (r < 0) {
                ec = nai_errno;
                if (ec != EPERM) {
                    nai_log_error(NSI_LOG_CORE, nai_errno, 
                        "service(%d, %d) sync request of client(0x%x) failed", 
                        s->serv, s->inst, f->client->cid);
                    goto _end;
                };
            };
        };
    };

    r = 0;

_end:
    return r;
};


static int nsi_service_sync_events(nsi_service_t* s)
{
    int r;
    int ec;
    int offer;
    int fieldsync;
    nai_rbnode_t* node;
    nai_list_entry_t* ent;
    nsi_gid_t* g;
    nsi_event_t* e;
    nsi_event_ref_t* f;
    nsi_event_link_t* l;
    nsi_event_info_t info;
    nai_array_t v;


    nai_array_init(&v, sizeof(nsi_gid_t), 0);

    fieldsync = 0;
    if (s->proxy && nsi_service_is_offered(s) && s->ep[1].stat) {
        fieldsync = 1;
    };

    /* for each events */
    node = nai_rbtree_begin(&s->events);
    for ( ; node != nai_rbtree_end(&s->events); ) {
        e = nai_containof(node, nsi_event_t, ent);
        node = nai_rbtree_next(node);

        /* clear array */
        v.count = 0;

        /* find all eventgroup */
        ent = e->list.next;
        for ( ; ent != &e->list; ) {
            l = nai_containof(ent, nsi_event_link_t, ente);
            ent = ent->next;

            g = nai_array_push(&v);
            if (g == 0) {
                ec = nai_errno;
                nai_log_alert(NAI_LOG_CORE, ec, 
                    "failed to push group when sync event group");
                r = -1;
                goto _fail;
            };

            g[0] = l->group->gid;
        };

        info.type = e->type;
        info.reliability = e->reliability;
        info.groups = (nsi_gid_t*)v.elts;
        info.ngroup = (int)v.count;


        /* sync event */
        ent = e->refs.next;
        for ( ; ent != &e->refs; ) {
            f = nai_containof(ent, nsi_event_ref_t, ente);
            ent = ent->next;

            /* call implemention */
            if (nsi_service_is_opened(s)) {
                if (s->own) {
                    offer = f->client == s->own->client;
                } else {
                    offer = 0;
                };
                r = s->ops->event(s, 
                    offer ? NSI_SERVICE_OP_OFFER : NSI_SERVICE_OP_ADD, 
                    f->client, e->eid, &info);
                if (r < 0) {
                    ec = nai_errno;
                    nai_log_error(NSI_LOG_CORE, ec, 
                        "service(%d, %d) sync event of client(0x%x) failed", 
                        s->serv, s->inst, f->client->cid);
                    goto _fail;
                };
            };
        };

        if (!fieldsync) {
            continue;
        };

        /* sync field msg */
        if ((e->type == NSI_ET_FIELD) && (e->sent_to_routing == 0) && e->msg){
            r = nsi_proxy_send(&s->ep[1].c, e->msg, 0, NSI_SEND_POSTED | NSI_SEND_MSGDUP);
            if (r >= 0) {
                e->sent_to_routing = 1;
            };
        };
    };
    nai_array_close(&v);
    r = 0;

_end:
    return r;

_fail:
    nai_array_close(&v);
    nai_errno = ec;
    goto _end;
};


static int nsi_service_sync_subscribe(nsi_service_t* s)
{
    int r;
    int ec;
    int n;
    int op;
    int stat;
    uint32_t ttl;
    uint64_t now;
    nai_rbnode_t* node;
    nai_list_entry_t* ent;
    nsi_event_group_t* g;
    nsi_subscription_t* u;
    nsi_client_t* c;


    now = nai_tickcache_to_msec();

    /* for each groups */
    node = nai_rbtree_begin(&s->groups);
    for ( ; node != nai_rbtree_end(&s->groups); ) {
        g = nai_containof(node, nsi_event_group_t, ent);
        node = nai_rbtree_next(node);


        /* sync all subscription */
        ent = g->subs.next;
        for ( ; ent != &g->subs; ) {
            u = nai_containof(ent, nsi_subscription_t, entg);
            ent = ent->next;

            c = u->client;
            if (c->remote_cli) {
                if (u->expire <= now) {
                    /* already expired, remove it */
                    nai_list_entry_remove(&u->entg);
                    nai_list_entry_remove(&u->entc);
                    nsi_service_free_subscription(s, u);
                };

                /* remote client do not need to be automatically subscribed */
                continue;
            };

            /* PRS_SOMEIPSD_00828
             * PRS_SOMEIPSD_00566
             */
            if (nsi_service_is_onlined(s) && 
                u->major != s->major) {
                nai_log_debug(NSI_LOG_CORE, 0, 
                    "client(0x%x) subscribe event group(%d) "
                    "of service(%d, %d) with different version %d, "
                    "previous is %d, do nack", 
                    c->cid, g->gid, s->serv, s->inst, u->major, s->major);

                nsi_service_subscribe_ack(s, c, 
                    NSI_E_NOT_OK, g->gid, u->major, NSI_EVENT_ANY, 0, 0);
                continue;
            };

            if (u->expire == (uint64_t)-1) {
                ttl = u->ttl;
            } else {
                ttl = (uint32_t)((u->expire - now) / 1000);
            };

            /* sync basic */
            if (u->stat != NSI_SUBSCRIBE_REMOVED) {
                op = NSI_SERVICE_OP_ADD;
            } else {
                op = NSI_SERVICE_OP_REMOVE;
            };
            if (nsi_service_is_onlined(s) || 
                nsi_service_is_proxy(s)) {
                r = s->ops->subscribe(s, 
                    op, c, g->gid, u->major, NSI_EVENT_ANY, ttl);
                if (r < 0) {
                    ec = nai_errno;
                    if (ec != EPERM) {
                        nai_log_error(NSI_LOG_CORE, ec, 
                            "service(%d, %d) sync "
                            "subscribe of client(0x%x) failed", 
                            s->serv, s->inst, c->cid);
                        goto _end;
                    };
                };
            };

            /* sync extra */
            for (n = 0; n < u->count; n ++) {
                stat = u->events[n].stat;
                if (stat == NSI_SUBSCRIBE_REMOVED) {
                    op = NSI_SERVICE_OP_REMOVE;
                } else {
                    op = NSI_SERVICE_OP_ADD;
                };
                if (nsi_service_is_onlined(s) || 
                    nsi_service_is_proxy(s)) {
                    r = s->ops->subscribe(s, 
                        op, c, g->gid, u->major, u->events[n].eid, ttl);
                    if (r < 0) {
                        ec = nai_errno;
                        if (ec != EPERM) {
                            nai_log_error(NSI_LOG_CORE, ec, 
                                "service(%d, %d) "
                                "sync subscribe of client(0x%x) failed", 
                                s->serv, s->inst, c->cid);
                            goto _end;
                        };
                    };
                };
            };
        };
    };

    r = 0;


_end:
    return r;
};


static int nsi_service_sync_offline(nsi_service_t* s)
{
    int r;
    uint64_t now;
    nai_rbnode_t* node;
    nai_list_entry_t* ent;
    nsi_event_t* e;
    nsi_event_group_t* g;
    nsi_subscription_t* u;
    nsi_client_t* c;


    nai_log_info(NSI_LOG_CORE, 0, 
        "service(%d, %d) is offlined, "
        "do clear events and nack all subscription", 
        s->serv, s->inst);

    now = nai_tickcache_to_msec();


    /* for each events */
    node = nai_rbtree_begin(&s->events);
    for ( ; node != nai_rbtree_end(&s->events); ) {
        e = nai_containof(node, nsi_event_t, ent);
        node = nai_rbtree_next(node);

        /* clear send status of events*/
        e->sent_to_routing = 0;

        /* clear message of client events */
        if (e->msg && !s->local) {
            nsi_message_close(e->msg);
            e->msg = 0;
        };
    };

    /* for each groups */
    node = nai_rbtree_begin(&s->groups);
    for ( ; node != nai_rbtree_end(&s->groups); ) {
        g = nai_containof(node, nsi_event_group_t, ent);
        node = nai_rbtree_next(node);

        /* sync all subscription */
        ent = g->subs.next;
        for ( ; ent != &g->subs; ) {
            u = nai_containof(ent, nsi_subscription_t, entg);
            ent = ent->next;

            c = u->client;
            if (c->remote_cli) {
                if (u->expire <= now) {
                    /* already expired, remove it */
                    nai_list_entry_remove(&u->entg);
                    nai_list_entry_remove(&u->entc);
                    nsi_service_free_subscription(s, u);
                    continue;
                };
            };

            if (u->major != s->major && 
                u->major != NSI_MAJOR_ANY && 
                s->major != NSI_MAJOR_ANY) {
                nai_log_debug(NSI_LOG_CORE, 0, 
                    "client(0x%x) subscribe event group(%d) "
                    "of service(%d, %d) with different version %d, "
                    "previous is %d, ignore it", 
                    c->cid, g->gid, s->serv, s->inst, u->major, s->major);

                continue;
            };

            /* forword 'unsubscribe' to discovery for nsi_service_client
             * PRS_SOMEIPSD_00431
             */
            if (nsi_service_is_opened(s) && 
                !nsi_service_is_proxy(s) && s->local == 0) {
                r = s->ops->subscribe(s, 
                    NSI_SERVICE_OP_REMOVE_LAST, c, g->gid, u->major, 
                    NSI_EVENT_ANY, 0);
                if (r < 0) {
                    nai_log_error(NSI_LOG_CORE, nai_errno, 
                        "service(%d, %d) failed to unsubscribe group", 
                        s->serv, s->inst);

                    /* fixme: error ignored */
                };
            };

            /* tell client subscribe failed
             * PRS_SOMEIPSD_00428
             * PRS_SOMEIPSD_00429
             */
            r = nsi_service_subscribe_ack(
                s, c, NSI_E_NOT_OK, g->gid, u->major, NSI_EVENT_ANY, 0, 0);
            if (r < 0) {
                nai_log_error(NSI_LOG_CORE, nai_errno, 
                    "service(%d, %d) failed to ack the subscription", 
                    s->serv, s->inst);

                /* fixme: error ignored */
            };
        };
    };

    r = 0;

    return r;
};


static int nsi_service_sync_impl(nsi_service_t* s)
{
    int r;


    nai_log_info(NSI_LOG_CORE, 0, 
        "service(%d, %d) sync all requests", 
        s->serv, s->inst);


    /* sync events */
    r = nsi_service_sync_events(s);
    if (r < 0) {
        goto _end;
    };

    /* sync subscriptions */
    r = nsi_service_sync_subscribe(s);
    if (r < 0) {
        goto _end;
    };

    /* sync clients */
    r = nsi_service_sync_requests(s);
    if (r < 0) {
        goto _end;
    };

    r = 0;

_end:
    return r;
};


int nsi_service_sync(nsi_service_t* s)
{
    int r;
    int ec;


    /* begin queue */
    r = nsi_routing_batch_begin(s->rt);
    if (r < 0) {
        nai_log_alert(NSI_LOG_CORE, 
            nai_errno, "start batch failed");
        goto _end;
    };

    /* sync clients */
    r = nsi_service_sync_impl(s);
    if (r < 0) {
        nai_log_alert(NSI_LOG_CORE, 
            nai_errno, "failed to sync service(%d, %d)", 
            s->serv, s->inst);
        goto _fail;
    };

    /* process queue */
    nsi_routing_batch_end(s->rt);
    r = 0;

_end:
    return r;

_fail:
    ec = nai_errno;
    nsi_routing_batch_end(s->rt);
    nai_errno = ec;
    goto _end;
};


int nsi_service_available_to_client(nsi_service_t* s, 
    int avail, nsi_client_t* c, nsi_version_t major, nsi_versmin_t minor)
{
    int r;


    if ((major != NSI_MAJOR_ANY && major != s->major) || 
        (minor != NSI_MINOR_ANY && minor != s->minor)) {
        nai_log_debug(NSI_LOG_CORE, nai_errno, 
            "service(%d, %d) mismatch client(0x%x) required version, "
            "ignore available(%d) notify", 
            s->serv, s->inst, c->cid, avail);

        r = 0;
        goto _end;
    };

    r = nsi_client_handle_available(
        c, avail, s->serv, s->inst, s->major, s->minor, 0);

_end:
    return r;
};


int nsi_service_available(nsi_service_t* s, int avail)
{
    int r;
    nsi_service_ref_t* f;
    nai_list_entry_t* ent;


    nai_log_info(NSI_LOG_CORE, 0, 
        "service(%d, %d) is available(%d)", 
        s->serv, s->inst, avail);

    /* begin queue */
    r = nsi_routing_batch_begin(s->rt);
    if (r < 0) {
        nai_log_alert(NSI_LOG_CORE, 
            nai_errno, "start batch failed");
        goto _end;
    };

    /* before available notify */
    switch (avail) {
    case NSI_AVAIL_OFFLINED:
        /* do clear event and nack
         * PRS_SOMEIPSD_00428
         * PRS_SOMEIPSD_00429
         * PRS_SOMEIPSD_00431
         */
        nsi_service_sync_offline(s);
        break;
    default:
        break;
    };

    /* notify all request client */
    ent = s->refs.next;
    for ( ; ent != &s->refs; ) {
        f = nai_containof(ent, nsi_service_ref_t, ents);
        ent = ent->next;

        if (f->offer) {
            continue;
        };

        /* call implemention */
        r = nsi_service_available_to_client(
            s, avail, f->client, f->major, f->minor);
        if (r < 0) {
            nai_log_error(NSI_LOG_CORE, nai_errno, 
                "failure to notify client(0x%x) "
                "that service(%d, %d) status(%d)", 
                f->client->cid, s->serv, s->inst, avail);

            /* fixme: error ignored */
        };
    };

    /* after available notify */
    switch (avail) {
    case NSI_AVAIL_ONLINED:
        /* do sync
         * PRS_SOMEIPSD_00486
         * PRS_SOMEIPSD_00527
         * PRS_SOMEIPSD_00415
         * PRS_SOMEIPSD_00446
         */
        if (nsi_service_is_proxy(s) || 
            nsi_service_is_offered(s)) {
            r = nsi_service_sync_impl(s);
            if (r < 0) {
                nai_log_alert(NSI_LOG_CORE, 
                    nai_errno, "failed to sync service(%d, %d)", 
                    s->serv, s->inst);

                /* fixme: error ignored */
            };
            break;
        };

        /* fall to case 'changed' */
        /* fallthrough */

    case NSI_AVAIL_CHANGED:
        assert(!nsi_service_is_proxy(s));

        /* do subscribe all
         * PRS_SOMEIPSD_00486
         * PRS_SOMEIPSD_00527
         * PRS_SOMEIPSD_00415
         * PRS_SOMEIPSD_00446
         */
        if (nsi_service_is_onlined(s)) {
            r = nsi_service_sync_subscribe(s);
            if (r < 0) {
                nai_log_alert(NSI_LOG_CORE, 
                    nai_errno, "failed to sync service(%d, %d) subscribes", 
                    s->serv, s->inst);

                /* fixme: error ignored */
            };
        };
        break;
    default:
        break;
    };

    nsi_routing_batch_end(s->rt);
    r = 0;

_end:
    return r;
};


