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
/// @file       nsi_service.c
/// @brief
/// @details
/// @date       2021-05-16
/// @author     xn
/// @version    1.2.0
///
/// ================================================================

#include "nai/os/nai_proc.h"
#include "nsi_service.h"
#include "nsi_client.h"
#include "nsi_proxy.h"
#include "nsi_routing_impl.h"
#include "nsi_log.h"
#include "nsi_discovery.h"
#include "nsomeip/core/nsi_app.h"
#include "nsomeip/net/nsi_message.h"
#include "nai/runtime/nai_errno.h"
#include "nai/runtime/nai_util.h"



static nsi_service_ref_t* nsi_service_find_client(
    nsi_service_t* s, nsi_client_t* c)
{
    nai_list_entry_t* e;
    nsi_service_ref_t* r;


    e = s->refs.next;
    for ( ; e != &s->refs; e = e->next) {
        r = nai_containof(e, nsi_service_ref_t, ents);
        if (r->client == c) {
            goto _end;
        };
    };

    r = 0;

_end:
    return r;
};


static nsi_service_ref_t* nsi_service_find_request(
    nsi_service_t* s, 
    nsi_client_t* c, nsi_version_t major, nsi_versmin_t minor)
{
    nai_list_entry_t* e;
    nsi_service_ref_t* r;


    e = s->refs.next;
    for ( ; e != &s->refs; e = e->next) {
        r = nai_containof(e, nsi_service_ref_t, ents);
        if (r->client == c && 
            r->major == major && 
            r->minor == minor) {
            goto _end;
        };
    };

    r = 0;

_end:
    return r;
};


static nai_rbnode_t** nsi_service_find_event(
    nsi_service_t* s, nsi_eid_t eid, nai_rbnode_t** pparent)
{
    nai_rbnode_t** n = &nai_rbtree_root(&s->events);
    nai_rbnode_t* parent = nai_rbtree_end(&s->events);
    nsi_event_t* e;


    while (*n) {
        parent = *n;
        e = nai_containof(parent, nsi_event_t, ent);
        if (e->eid == eid) {
            break;
        } else if (e->eid >= eid) {
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


static nai_rbnode_t** nsi_service_find_eventgroup(
    nsi_service_t* s, nsi_eid_t eid, nai_rbnode_t** pparent)
{
    nai_rbnode_t** n = &nai_rbtree_root(&s->groups);
    nai_rbnode_t* parent = nai_rbtree_end(&s->groups);
    nsi_event_group_t* e;


    while (*n) {
        parent = *n;
        e = nai_containof(parent, nsi_event_group_t, ent);
        if (e->gid == eid) {
            break;
        } else if (e->gid >= eid) {
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


static int nsi_service_subevent_insert(
    nsi_service_t* s, nsi_subscription_t* u, 
    nsi_eid_t event, uint16_t stat)
{
    int r;
    int count;
    int alloc;
    nsi_subevent_t* mem;


    count = u->count;
    if (count >= u->total) {
        alloc = u->total * 2;
        if (alloc < 8) {
            alloc = 8;
        };

        mem = (nsi_subevent_t*)
            nsi_routing_alloc(s->rt, sizeof(*mem) * alloc);
        if (mem == 0) {
            nai_log_alert(NSI_LOG_CORE, nai_errno, 
                "failed to allocate subevent array when insert event");
            r = -1;
            goto _end;
        };

        if (u->events) {
            nai_memcpy(mem, u->events, sizeof(*mem) * count);
            nsi_routing_free(s->rt, u->events);
        };

        u->total = alloc;
        u->events = mem;
    };

    u->events[count].eid = event;
    u->events[count].stat = stat;
    u->count ++;
    r = 0;


_end:
    return r;
};


static int nsi_service_subevent_to_add(uint32_t stat)
{
    int r;

    switch (stat) {
    case NSI_SUBSCRIBE_NACK:
    case NSI_SUBSCRIBE_REMOVED:
        r = 1;
        break;
    default:
        r = 0;
        break;
    };

    return r;
};


static int nsi_service_subevent_to_just(uint32_t stat)
{
    int r;

    switch (stat) {
    case NSI_SUBSCRIBE_NACK:
    case NSI_SUBSCRIBE_PENDING:
        r = 1;
        break;
    default:
        r = 0;
        break;
    };

    return r;
};


static int nsi_service_subevent_to_ack(uint32_t stat)
{
    int r;

    switch (stat) {
    case NSI_SUBSCRIBE_JUST:
        r = 1;
        break;
    default:
        r = 0;
        break;
    };

    return r;
};


static int nsi_service_subevent_to_nack(uint32_t stat)
{
    int r;

    switch (stat) {
    case NSI_SUBSCRIBE_JUST:
    case NSI_SUBSCRIBE_ACK:
    case NSI_SUBSCRIBE_PENDING:
        r = 1;
        break;
    default:
        r = 0;
        break;
    };

    return r;
};


static int nsi_service_subevent_to_remove(uint32_t stat)
{
    int r;

    switch (stat) {
    case NSI_SUBSCRIBE_REMOVED:
        r = 0;
        break;
    default:
        r = 1;
        break;
    };

    return r;
};


static int nsi_service_subevent_stat(
    nsi_service_t* s, nsi_subscription_t* u, nsi_eid_t event)
{
    int r;
    int m;
    int count;


    (void)s;

    count = u->count;
    for (m = 0; m < count; m ++) {
        if (u->events[m].eid == event) {
            break;
        };
    };

    if (m < count) {
        /* in black list, skip */
        r = u->events[m].stat;
    } else {
        r = u->stat;
    };

    return r;
};


static int nsi_service_subevent_is(
    nsi_service_t* s, nsi_subscription_t* u, nsi_eid_t event, uint32_t stat)
{
    return nsi_service_subevent_stat(s, u, event) == (int)stat;
};


static int nsi_service_subevent_are(
    nsi_service_t* s, nsi_subscription_t* u, 
    nsi_eid_t event, uint32_t stat, int all)
{
    int r;
    nai_list_entry_t* e;
    nsi_client_t* c;
    nsi_subscription_t* b;


    r = nsi_service_subevent_is(s, u, event, stat);
    if (r == 0) {
        goto _end;
    };

    c = u->client;
    e = c->subs.next;
    for ( ; e != &c->subs; e = e->next) {
        b = nai_containof(e, nsi_subscription_t, entc);
        if (b == u) {
            continue;
        };
        if (!all && u->session != b->session) {
            continue;
        };

        r = nsi_service_subevent_is(s, b, event, stat);
        if (r == 0) {
            r = 0;
            goto _end;
        };
    };

    r = 1;

_end:
    return r;
};


static int nsi_service_subevent_update(
    nsi_service_t* s, nsi_subscription_t* u, nsi_eid_t event, 
    uint32_t stat)
{
    int r;
    int m;
    int count;
    nsi_subevent_t* e;
    int (*check)(uint32_t);


    /* the function of 'check' is used to 
     * determine whether the state needs to be changed.
     * */
    switch (stat) {
    case NSI_SUBSCRIBE_PENDING:
        check = nsi_service_subevent_to_add;
        break;
    case NSI_SUBSCRIBE_JUST:
        check = nsi_service_subevent_to_just;
        break;
    case NSI_SUBSCRIBE_ACK:
        check = nsi_service_subevent_to_ack;
        break;
    case NSI_SUBSCRIBE_NACK:
        check = nsi_service_subevent_to_nack;
        break;
    case NSI_SUBSCRIBE_REMOVED:
        check = nsi_service_subevent_to_remove;
        break;
    default:
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    if (event == NSI_EVENT_ANY) {

        r = 0;
        /* check default state */
        if (check(u->stat)) { /* need changed */
            u->stat = stat;
            r = 1;
        };

        /* check each difference event */
        for (m = 0; m < u->count; ) {
            e = &u->events[m];
            if (!check(e->stat)) { /* unneed changed */
                m ++;
                continue;
            };

            /* if the default state is same as the new state, 
             * remove it, 
             * otherwise modify it
             * */
            if (u->stat == stat) {
                u->count --;
                e[0] = u->events[u->count];
            } else {
                e[0].stat = stat;
                m ++;
            };

            r = 1;
        };

    } else {

        /* find the event that needs to be modified */
        count = u->count;
        for (m = 0; m < count; m ++) {
            e = &u->events[m];
            if (e->eid == event) {
                break;
            };
        };

        if (check(u->stat)) {
            /* the default state is not compatible with the new state, 
             * we try to modify the exists one or insert a new one
             * */
            if (m < count) {
                if (check(e->stat)) { /* need changed */
                    e->stat = stat;
                    r = 1;
                    goto _end;
                } else {
                    r = 0;
                    goto _end;
                };
            };

        } else {
            /* the default state is compatible with the new state, 
             * we try to remove or modify the exists one.
             * */
            if (m < count) {
                if (check(e->stat)) { /* need changed */
                    /* if the default state is same as the new state, 
                     * remove it, 
                     * otherwise modify it
                     * */
                    if (u->stat == stat) {
                        e[0] = u->events[count-1];
                        u->count --;
                    } else {
                        e[0].stat = stat;
                    };
                    r = 1;
                    goto _end;
                };
            };

            /* unneed change or non-exists */
            r = 0;
            goto _end;
        };

        /* not found, insert a new one */
        r = nsi_service_subevent_insert(s, u, event, stat);
        if (r < 0) {
            /* already logged */
            goto _end;
        };

        r = 1;
    };



_end:
    return r;
};


static int nsi_service_subevent_add(
    nsi_service_t* s, nsi_subscription_t* u, nsi_eid_t event)
{
    return nsi_service_subevent_update(
        s, u, event, NSI_SUBSCRIBE_PENDING);
};


static int nsi_service_subevent_just(
    nsi_service_t* s, nsi_subscription_t* u, nsi_eid_t event)
{
    return nsi_service_subevent_update(
        s, u, event, NSI_SUBSCRIBE_JUST);
};


static int nsi_service_subevent_ack(
    nsi_service_t* s, nsi_subscription_t* u, nsi_eid_t event)
{
    return nsi_service_subevent_update(
        s, u, event, NSI_SUBSCRIBE_ACK);
};


static int nsi_service_subevent_nack(
    nsi_service_t* s, nsi_subscription_t* u, nsi_eid_t event)
{
    return nsi_service_subevent_update(
        s, u, event, NSI_SUBSCRIBE_NACK);
};

static int nsi_service_subevent_remove(
    nsi_service_t* s, nsi_subscription_t* u, nsi_eid_t event)
{
    return nsi_service_subevent_update(
        s, u, event, NSI_SUBSCRIBE_REMOVED);
};


static int nsi_service_notify_one(
    nsi_service_t* s, nsi_subscription_t* u, 
    nsi_event_t* e, uint32_t uid, int flags)
{
    int r;
    int idx;
    uint16_t stat;
    nsi_connid_t cid;
    nsi_message_t* msg;


    /* get message */
    msg = e->msg;

    /* get event stat */
    stat = nsi_service_subevent_stat(s, u, msg->hdr.method);
    switch (stat) {
    case NSI_SUBSCRIBE_ACK:
    case NSI_SUBSCRIBE_JUST:
        break;
    default:
        /* return -2 and let caller to free memory if necessary */
        nai_errno = ENOENT;
        r = -2;
        goto _end;
    };

    /* using endpoint depends on contifuration of event */
    switch (e->reliability) {
    default:
        assert(0);
    case NSI_RT_UNKNOWN:
    case NSI_RT_BOTH:
    case NSI_RT_RELIABLE:
        idx = 1;
        break;
    case NSI_RT_UNRELIABLE:
        idx = 0;
        break;
    };

    cid = u->conn[idx];
    if (cid == 0) {
        idx = !idx;
        cid = u->conn[idx];
    };


    /* only check for present subscription, 
     * tcp disconnection is handled in nsi_service_disconnected.
     */
    if (!s->proxy && u->client->temp) {
        /* idx != 0 means it is a tcp connection */
        /* tcp needs to check whether the connection is established */
        if (idx != 0 && cid != 0) {
            r = nsi_endpoint_set_opt(
                s->ep[idx].ep, NSI_EOPT_CHECK_CSTAT, cid);
            if (r <= 0) {
                idx = !idx;
                cid = u->conn[idx];
            };
        };
    };

    if (cid == 0) {
        /* remote client is allowed without addresses, 
         * and we can't send message to the client, because the client is 
         * the discovery of the peer.
         * PRS_SOMEIPSD_00583
         */
        if (!u->client->remote) {
            r = nsi_client_send(u->client, msg, uid, flags);
        } else {
            if (flags & NSI_SEND_MSGDUP) {
                r = 0;
            } else {
                r = nsi_message_close(msg);
            };
        };
    } else {
        /* PRS_SOMEIPSD_00360
         * PRS_SOMEIPSD_00361
         * PRS_SOMEIPSD_00362
         * PRS_SOMEIPSD_00480
         * PRS_SOMEIPSD_00484
         */
        msg->conn = cid;
        flags |= NSI_SEND_POSTED;
        r = nsi_endpoint_send(s->ep[idx].ep, msg, uid, flags);
    };
    if (r >= 0) {
        if (s->proxy && u->client->proxy) {
            e->sent_to_routing = 1;
        };
    };


_end:
    return r;
};


static int nsi_service_notify(nsi_service_t* s, nsi_message_t* msg)
{
    int r;
    int flags;
    int mcast;
    int count;
    uint32_t uid;
    uint64_t now;
    nai_rbnode_t** n;
    nai_list_entry_t* ent;
    nai_list_entry_t* ents;
    nsi_client_t* c;
    nsi_event_t* e;
    nsi_event_link_t* l;
    nsi_event_group_t* g;
    nsi_subscription_t* u;


    n = nsi_service_find_event(s, msg->hdr.method, 0);
    if (n[0] == 0) {
        nai_errno = ENOENT;
        r = -1;
        goto _end;
    };


    /* do check the owner */
    e = nai_containof(n[0], nsi_event_t, ent);
    if (s->local) {
        if (e->own == 0 || 
            e->own->client->cid != msg->hdr.client) {
            nai_errno = EACCES;
            nai_log_debug(NSI_LOG_CORE, nai_errno, 
                "the event(%d) of service(%d, %d) "
                "is not owned by the client(0x%x)", 
                msg->hdr.method, msg->hdr.serv, msg->inst, msg->hdr.client);
            r = -1;
            goto _end;
        };
    };


    /* do check before send event */
    r = nsi_routing_grant_event(s->rt, msg);
    if (r < 0) {
        nai_log_debug(NSI_LOG_CORE, nai_errno, 
            "the event(%d) of service(%d, %d) "
            "from the client(0x%x) is denied", 
            msg->hdr.method, msg->hdr.serv, msg->inst, msg->hdr.client);
        goto _end;
    };


    /* clear the client id of events when the events will be sent to network */
    /* SWS_CM_10291 */
    if (s->local && !s->proxy) {
        msg->hdr.client = 0;
    };


    /* prevent repeated events */
    if (e->msg == 0) {
        if (!s->proxy) {
            /* routing make first event's unique id to zero */
            msg->unique_id = 0;
        };
    } else {
        if (!s->proxy) {
            /* routing make an unique id for events */
            msg->unique_id = e->msg->unique_id + 1;
            if (msg->unique_id == 0) {
                msg->unique_id ++;
            };
        } else {
            /* proxy client need to check repeated event, 
             * unique id of zero means the first event
             **/
            if (!s->own && 
                msg->unique_id == e->msg->unique_id) {
                nsi_message_close(msg);
                r = 0;
                goto _end;
            };
        };

        nsi_message_close(e->msg);
    };

    /* save event message */
    e->msg = msg;
    e->sent_to_routing = 0;


    /* begin queue */
    r = nsi_routing_batch_begin(s->rt);
    if (r < 0) {
        nai_log_alert(NSI_LOG_CORE, 
            nai_errno, "start batch failed");
        r = 0;
        goto _end;
    };


    count = 0;
    now = nai_tickcache_to_msec();
    ent = e->list.next;
    for ( ; ent != &e->list; ent = ent->next) {
        l = nai_containof(ent, nsi_event_link_t, ente);
        g = l->group;

        /* for each subscription: remove expried */
        ents = g->subs.next;
        for ( ; ents != &g->subs; ) {
            u = nai_containof(ents, nsi_subscription_t, entg);
            ents = ents->next;

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
        };

        /* try to multicast when remotes greater than threshold
         * PRS_SOMEIPSD_00134
         */
        if (g->mcast && e->own) {
            mcast = g->threshold <= g->remote;
            if (mcast) {
                /* PRS_SOMEIPSD_00329
                 * PRS_SOMEIPSD_00336
                 */
                count ++;
            };
        } else {
            mcast = 0;
        };

        count += g->count - (mcast ? g->remote : 0);
    };

    if (count > 0) {
        /* get unique code to prevent repeated sending, 
         * and disable all message callback
         * PRS_SOMEIPSD_00570
         */
        if (count > 1) {
            uid = nsi_routing_unique_code(s->rt);
        } else {
            uid = 0;
        };

        if (e->type == NSI_ET_FIELD) {
            count ++;
        };

        /* for each eventgroup */
        ent = e->list.next;
        for ( ; ent != &e->list; ent = ent->next) {
            l = nai_containof(ent, nsi_event_link_t, ente);
            g = l->group;

            /* try to multicast when remotes greater than threshold
             * PRS_SOMEIPSD_00134
             */
            if (g->mcast && e->own) {
                mcast = g->threshold <= g->remote;
                if (mcast) {
                    /* PRS_SOMEIPSD_00329
                     * PRS_SOMEIPSD_00336
                     */
                    count --;
                    if (count > 0) {
                        flags = NSI_SEND_POSTED | NSI_SEND_MSGDUP;
                    } else {
                        flags = NSI_SEND_POSTED;
                    };

                    msg->conn = g->mcast;
                    r = nsi_endpoint_send(s->ep[0].ep, msg, uid, flags);
                    if (r < 0) {
                        nai_log_error(NSI_LOG_CORE, nai_errno, 
                            "the service(%d, %d) failed to "
                            "multicast event message to group(%d)", 
                            s->serv, s->inst, g->gid);

                        if (count <= 0) {
                            nsi_message_close(msg);
                        };
                    };
                };
            } else {
                mcast = 0;
            };
            if (g->count - (mcast ? g->remote : 0) <= 0) {
                continue;
            };

            /* for each subscription */
            ents = g->subs.next;
            for ( ; ents != &g->subs; ) {
                u = nai_containof(ents, nsi_subscription_t, entg);
                ents = ents->next;

                c = u->client;
                if (u->ac == 0) {
                    continue;
                };
                if (mcast && c->remote) {
                    continue;
                };

                /* send to */
                count --;
                if (count > 0) {
                    flags = NSI_SEND_POSTED | NSI_SEND_MSGDUP;
                } else {
                    flags = NSI_SEND_POSTED;
                };
                r = nsi_service_notify_one(s, u, e, uid, flags);
                if (r < 0) {
                    if (-2 != r) {
                        nai_log_debug(NSI_LOG_CORE, nai_errno, 
                            "the service(%d, %d) failed to "
                            "send event message to client(0x%x)", 
                            s->serv, s->inst, c->cid);
                    };

                    if (count <= 0) {
                        nsi_message_close(msg);
                    };
                };
            };
        };
    } else {
        if (e->type != NSI_ET_FIELD) {
            nsi_message_close(msg);
        };
    };

    if (e->type != NSI_ET_FIELD) {
        e->msg = 0;
    } else {
        /* Sync field msg from proxy to router */
        if (s->proxy && nsi_service_is_offered(s) && s->ep[1].stat) {
            if (e->sent_to_routing == 0) {
                r = nsi_proxy_send(&s->ep[1].c, msg, 0, NSI_SEND_POSTED | NSI_SEND_MSGDUP);
                if (r >= 0) {
                    e->sent_to_routing = 1;
                };
            };
        };
    };

    nsi_routing_batch_end(s->rt);

    r = 0;

_end:
    return r;
};


static nsi_client_t* nsi_service_get_offer(nsi_service_t* s)
{
    nsi_event_t* e;
    nsi_client_t* r;
    nai_rbnode_t* n;


    if (s->own) {
        r = s->own->client;
        goto _end;
    };

    n = nai_rbtree_begin(&s->events);
    for ( ; n != nai_rbtree_end(&s->events); ) {
        e = nai_containof(n, nsi_event_t, ent);
        n = nai_rbtree_next(n);

        if (e->own) {
            r = e->own->client;
            goto _end;
        };
    };

    r = 0;

_end:
    return r;
};


static int nsi_service_add_preset(nsi_service_t* s, nsi_event_group_t* g)
{
    int r;
    int i;
    nsi_version_t major;
    nsi_client_t* c;
    nsi_group_info_t gi;
    nsi_service_info_t si;


    /* PRS_SOMEIPSD_00470 */

    r = nsi_routing_get_group_info(
        s->rt, s->serv, s->inst, g->gid, &gi);
    if (r < 0 || 
        gi.scount <= 0) {
        r = 0;
        goto _end;
    };

    major = s->major;
    if (major == NSI_MAJOR_ANY) {
        r = nsi_routing_get_service_info(s->rt, s->serv, s->inst, &si, 1);
        if (r < 0) {
            nai_log_error(NSI_LOG_CORE, EPERM, 
                "the service(%d, %d) is not a local service", 
                s->serv, s->inst);
            nai_errno = EPERM;
            r = -1;
            goto _end;
        };

        major = si.major;
    };

    /* add preset subscription */
    for (i = 0; i < gi.scount; i ++) {
        /* make clients */
        c = nsi_client_create_temp(s->rt);
        if (c == 0) {
            nai_log_alert(NSI_LOG_CORE, 
                nai_errno, "create a temparay client failed");
            r = -1;
            goto _end;
        };

        /* force set remote */
        c->remote = 1;
        c->local = 0;

        /* subscribe */
        r = nsi_service_subscribe(s, 
            c, g->gid, major, NSI_EVENT_ANY, -1, 0, gi.subs[i].names);
        if (r < 0) {
            nai_log_error(NSI_LOG_CORE, 
                nai_errno, "add a preset subscription failed");
            r = -1;
            goto _end;
        };
    };

    r = 0;

_end:
    return r;
};


static int nsi_service_remove_preset(nsi_service_t* s, nsi_event_group_t* g)
{
    int r;
    nai_list_entry_t* e;
    nsi_client_t* c;
    nsi_subscription_t* u;


    e = g->subs.next;
    for ( ; e != &g->subs; ) {
        u = nai_containof(e, nsi_subscription_t, entg);
        e = e->next;
        c = u->client;
        if (c->temp) {
            r = nsi_service_unsubscribe(
                s, u->client, g->gid, u->major, NSI_EVENT_ANY);
            if (r < 0) {
                nai_log_error(NSI_LOG_CORE, 
                    nai_errno, "remove a preset subscription failed");

                /* fixed me: ignored error */
            };

            /* try to close a temprary client */
            nsi_client_try_close(c);
        };
    };

    r = 0;

    return r;
};


int nsi_service_add_all_preset(nsi_service_t* s)
{
    int r;
    nai_rbnode_t* n;
    nsi_event_group_t* g;


    n = nai_rbtree_begin(&s->groups);
    for ( ; n != nai_rbtree_end(&s->groups); ) {
        g = nai_containof(n, nsi_event_group_t, ent);
        n = nai_rbtree_next(n);
        r = nsi_service_add_preset(s, g);
        if (r < 0) {
            goto _end;
        };
    };

    r = 0;

_end:
    return r;
};


int nsi_service_remove_all_preset(nsi_service_t* s)
{
    int r;
    nai_rbnode_t* n;
    nsi_event_group_t* g;


    n = nai_rbtree_begin(&s->groups);
    for ( ; n != nai_rbtree_end(&s->groups); ) {
        g = nai_containof(n, nsi_event_group_t, ent);
        n = nai_rbtree_next(n);
        r = nsi_service_remove_preset(s, g);
        if (r < 0) {
            /* fixed me: ignored error */
            ;
        };
    };

    r = 0;

    return r;
};


int nsi_service_send(nsi_service_t* s, nsi_message_t* m)
{
    int r;
    nsi_inst_t inst;


    /* fix instance of message */
    inst = m->inst;
    m->inst = s->inst;

    /* route message */
    switch (m->hdr.type) {
    case NSI_MT_REQUEST:
    case NSI_MT_REQUEST_NO_RETURN:
    case NSI_MT_RESPONSE:
    case NSI_MT_ERROR:
        /* allow send message before connected */
        if (nsi_service_is_avail(s) == 0) {
            nai_errno = ENOTCONN;
            r = -1;
        } else {
            r = s->ops->send(s, m);
        };
        break;

    case NSI_MT_NOTIFICATION:
        if (nsi_service_is_avail(s) == 0 && !s->local) {
            nai_log_error(NSI_LOG_CORE, EINVAL, 
                "service(%d, %d) cannot send the message with type(%d)", 
                s->serv, s->inst, m->hdr.type);
            nai_errno = EINVAL;
            r = -1;
        } else {
            r = nsi_service_notify(s, m);
        };
        break;

    default:
        nai_log_error(NSI_LOG_CORE, EINVAL, 
            "service(%d, %d) cannot send the message with unknown type(%d)", 
            s->serv, s->inst, m->hdr.type);
        nai_errno = EINVAL;
        r = -1;
        break;
    };

    /* restore inst of message on failed */
    if (r < 0) {
        m->inst = inst;
    };


    return r;
};


int nsi_service_offer(nsi_service_t* s, 
    nsi_client_t* c, nsi_version_t major, nsi_versmin_t minor)
{
    int r;
    int ec;
    int batch;
    nsi_client_t* own;
    nsi_service_ref_t* l;
    nsi_service_info_t si;


    nai_log_info(NSI_LOG_CORE, 0, 
        "client(0x%x) offer service(%d, %d) with version(%d, %d)", 
        c->cid, s->serv, s->inst, major, minor);

    if (!s->local) {
        nai_log_error(NSI_LOG_CORE, EPERM, 
            "the service(%d, %d) is not a local service", 
            s->serv, s->inst);

        nai_errno = EPERM;
        r = -1;
        goto _end;
    };

    own = nsi_service_get_offer(s);
    if (s->own || (own != 0 && own != c)) {
        if (own == c) {
            r = 0;
        } else {
            nai_log_error(NSI_LOG_CORE, EPERM, 
                "the service(%d, %d) is already offer by client(0x%x)", 
                s->serv, s->inst, own->cid);
            nai_errno = EPERM;
            r = -1;
        };
        goto _end;
    };

    if (nsi_service_is_avail(s)) {
        nai_log_error(NSI_LOG_CORE, EPERM, 
            "the service(%d, %d) is already offer by remote client", 
            s->serv, s->inst);
        r = -1;
        goto _end;
    };


    r = nsi_routing_get_service_info(s->rt, s->serv, s->inst, &si, 1);
    if (r < 0) {
        nai_log_error(NSI_LOG_CORE, EPERM, 
            "the service(%d, %d) is not a local service", 
            s->serv, s->inst);
        nai_errno = EPERM;
        r = -1;
        goto _end;
    };

    if (si.major != major || 
        si.minor != minor) {
        nai_log_error(NSI_LOG_CORE, EINVAL, 
            "client(0x%x) offer service(%d, %d) with wrong version(%d, %d) "
            "which mismatch version(%d, %d) in configuration", 
            c->cid, s->serv, s->inst, 
            major, minor, si.major, si.minor);
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };


    l = s->own;
    if (l == 0) {
        l = nsi_service_find_client(s, c);
        if (l != 0) {
            nai_log_error(NSI_LOG_CORE, EEXIST, 
                "client(0x%x) is already requested the service(%d, %d)", 
                c->cid, s->serv, s->inst);
            nai_errno = EEXIST;
            r = -1;
            goto _end;
        };


        l = (nsi_service_ref_t*)nsi_routing_alloc(s->rt, sizeof(*l));
        if (l == 0) {
            nai_log_alert(NSI_LOG_CORE, nai_errno, 
                "failed to allocate memory for offer service");
            r = -1;
            goto _end;
        };

        s->own = l;
        l->flags = 0;
        l->offer = 1;
        l->client = c;
        l->service = s;
        l->major = major;
        l->minor = minor;
        nai_list_insert_tail(&s->refs, &l->ents);
        nai_list_insert_tail(&c->servs, &l->entc);
    };


    /* begin batch */
    r = nsi_routing_batch_begin(s->rt);
    if (r < 0) {
        nai_log_alert(NSI_LOG_CORE, 
            nai_errno, "start batch failed");
        batch = 0;
        goto _fail;
    };
    batch = 1;

    r = s->ops->offer(s, c);
    if (r < 0) {
        nai_log_error(NSI_LOG_CORE, nai_errno, 
            "failed to send command of offer service(%d, %d) for client(0x%x)", 
            s->serv, s->inst, c->cid);
        if (!s->proxy) {
            if (nai_errno == EADDRINUSE ||
                nai_errno == EADDRNOTAVAIL ||
                nai_errno == ENETDOWN) {
                nai_log_crit(NSI_LOG_CORE, nai_errno, 
                    "meet unrecoverable reason for offering service(%d, %d) for client(0x%x) and raise SIGTERM", 
                    s->serv, s->inst, c->cid);
                raise(SIGTERM);
            };
        };
        goto _fail;
    };

    nsi_routing_batch_end(s->rt);


_end:
    return r;

_fail:
    ec = nai_errno;

    s->own = 0;
    nai_list_entry_remove(&l->ents);
    nai_list_entry_remove(&l->entc);
    nsi_routing_free(s->rt, l);

    if (batch) {
        nsi_routing_batch_end(s->rt);
    };

    nai_errno = ec;
    r = -1;
    goto _end;
};


int nsi_service_stop(nsi_service_t* s, 
    nsi_client_t* c, nsi_version_t major, nsi_versmin_t minor)
{
    int r;
    int ec;
    nsi_service_ref_t* l;


    nai_log_info(NSI_LOG_CORE, 0, 
        "client(0x%x) stop service(%d, %d)", 
        c->cid, s->serv, s->inst);

    l = s->own;
    if (l == 0) {
        nai_log_error(NSI_LOG_CORE, EPERM, 
            "service(%d, %d) without offer client", 
            s->serv, s->inst);
        nai_errno = EPERM;
        r = -1;
        goto _end;
    };
    if (l->client != c) {
        nai_log_error(NSI_LOG_CORE, EPERM, 
            "the service(%d, %d) is already offer by client(0x%x)", 
            s->serv, s->inst, l->client->cid);
        nai_errno = EPERM;
        r = -1;
        goto _end;
    };

    assert(nsi_service_is_started(s));


    /* check versions */
    if ((major != NSI_MAJOR_ANY && s->major != major) || 
        (minor != NSI_MINOR_ANY && s->minor != minor)) {
        nai_log_error(NSI_LOG_CORE, EINVAL, 
            "cannot stop service(%d, %d) with wrong version(%d, %d) "
            "which mismatch the version(%d, %d) of service", 
            major, minor, s->major, s->minor, s->serv, s->inst);
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };


    r = nsi_routing_batch_begin(s->rt);
    if (r < 0) {
        nai_log_alert(NSI_LOG_CORE, 
            nai_errno, "start batch failed");
        goto _end;
    };

    /* remove offer */
    l = s->own;
    s->own = 0;
    nai_list_entry_remove(&l->ents);
    nai_list_entry_remove(&l->entc);
    nsi_routing_free(s->rt, l);


    /* do stop service */
    r = s->ops->stop(s, c);
    if (r < 0) {
        ec = nai_errno;
        nai_log_error(NSI_LOG_CORE, ec, 
            "failed to send command of stop service(%d, %d) for client(0x%x)", 
            s->serv, s->inst, c->cid);
    };

    nsi_routing_batch_end(s->rt);

    if (r < 0) {
        nai_errno = ec;
    };

_end:
    return r;
};


int nsi_service_request(nsi_service_t* s, 
    nsi_client_t* c, nsi_version_t major, nsi_versmin_t minor)
{
    int r;
    int ec;
    int batch;
    nsi_service_ref_t* l;


    nai_log_info(NSI_LOG_CORE, 0, 
        "client(0x%x) request service(%d, %d) with version(%d, %d)", 
        c->cid, s->serv, s->inst, major, minor);


    l = nsi_service_find_request(s, c, major, minor);
    if (l != 0) {
        nai_log_info(NSI_LOG_CORE, EEXIST, 
            "client(0x%x) is already requested service(%d, %d)", 
            c->cid, s->serv, s->inst);
        r = 0;
        goto _end;
    };


    l = (nsi_service_ref_t*)nsi_routing_alloc(s->rt, sizeof(*l));
    if (l == 0) {
        nai_log_alert(NSI_LOG_CORE, nai_errno, 
            "failed to allocate memory for request service");
        r = -1;
        goto _end;
    };

    l->flags = 0;
    l->client = c;
    l->service = s;
    l->major = major;
    l->minor = minor;
    nai_list_insert_tail(&s->refs, &l->ents);
    nai_list_insert_tail(&c->servs, &l->entc);


    /* begin batch */
    r = nsi_routing_batch_begin(s->rt);
    if (r < 0) {
        nai_log_alert(NSI_LOG_CORE, 
            nai_errno, "start batch failed");
        batch = 0;
        goto _fail;
    };
    batch = 1;


    /* do request */
    r = s->ops->request(s, NSI_SERVICE_OP_ADD, c, major, minor);
    if (r < 0) {
        nai_log_error(NSI_LOG_CORE, nai_errno, 
            "failed to send command of "
            "request service(%d, %d) for client(0x%x)", 
            s->serv, s->inst, c->cid);
        goto _fail;
    };

    nsi_routing_batch_end(s->rt);


_end:
    return r;

_fail:
    ec = nai_errno;

    nai_list_entry_remove(&l->ents);
    nai_list_entry_remove(&l->entc);
    nsi_routing_free(s->rt, l);

    if (batch) {
        nsi_routing_batch_end(s->rt);
    };

    nai_errno = ec;
    r = -1;
    goto _end;
};


int nsi_service_release(nsi_service_t* s, 
    nsi_client_t* c, nsi_version_t major, nsi_versmin_t minor)
{
    int r;
    int last;
    nsi_service_ref_t* l;


    nai_log_info(NSI_LOG_CORE, 0, 
        "client(0x%x) release service(%d, %d) with version(%d, %d)", 
        c->cid, s->serv, s->inst, major, minor);

    l = nsi_service_find_request(s, c, major, minor);
    if (l == 0) {
        nai_log_info(NSI_LOG_CORE, ENOENT, 
            "client(0x%x) did not request service(%d, %d)", 
            c->cid, s->serv, s->inst);
        nai_errno = ENOENT;
        r = -1;
        goto _end;
    };


    if (l->offer) {
        r = nsi_service_stop(s, c, major, minor);
        goto _end;
    };


    major = l->major;
    minor = l->minor;
    nai_list_entry_remove(&l->ents);
    nai_list_entry_remove(&l->entc);
    nsi_routing_free(s->rt, l);


    /* do release */
    last = nai_list_is_empty(&s->refs);
    r = s->ops->request(s, 
        last ? NSI_SERVICE_OP_REMOVE_LAST : NSI_SERVICE_OP_REMOVE, 
        c, major, minor);
    if (r < 0) {
        nai_log_error(NSI_LOG_CORE, nai_errno, 
            "failed to send command of release service");
    };


_end:
    return r;
};

static void nsi_service_update_group_multicast(nsi_service_t *s, nsi_event_group_t *g)
{
     int r;
     nsi_group_info_t gi;

    if (g->threshold) {
        return;
    }

    /*  after nsi_service_reopen, group info needs update.
        especially when non-local service being changed to local service */

    r = nsi_routing_get_group_info(
        s->rt, s->serv, s->inst, g->gid, &gi);
    if (r >= 0 &&
        s->local && !s->proxy) {
        /* PRS_SOMEIPSD_00329
         * PRS_SOMEIPSD_00336
         */
        g->threshold = gi.threshold;
        nsi_service_bind_name(s->ep[0].ep,
            NSI_EOPT_BIND_NAME, &gi.mcast, &g->mcast);
    };

}

int nsi_service_register(nsi_service_t* s, 
    nsi_client_t* c, nsi_eid_t eid, const nsi_event_info_t* info, int offer)
{
    int r;
    int m;
    int ec;
    int owned;
    int init;
    nsi_gid_t gid;
    nai_rbnode_t** n;
    nai_rbnode_t* parent;
    nai_list_entry_t* ent;
    nai_list_entry_t* entf;
    nai_list_entry_t list;
    nsi_client_t* own;
    nsi_event_t* e;
    nsi_event_group_t* g;
    nsi_event_ref_t* f;
    nsi_event_link_t* l;
    nsi_event_info_t ei;

    nai_log_info(NSI_LOG_CORE, 0, 
        "client(0x%x) %s event(%d) to service(%d, %d)", 
        c->cid, offer ? "offer" : "request", eid, s->serv, s->inst);

    offer = !!offer;
    if (offer && !s->local) {
        nai_log_error(NSI_LOG_CORE, EPERM, 
            "the service(%d, %d) is not a local service", 
            s->serv, s->inst);

        nai_errno = EPERM;
        r = -1;
        goto _end;
    };

    own = nsi_service_get_offer(s);
    owned = own == c;
    if (offer != owned && own) {
        if (offer) {
            nai_log_error(NSI_LOG_CORE, EPERM, 
                "client(0x%x) is not the service(%d, %d) provider", 
                c->cid, s->serv, s->inst);
        } else {
            nai_log_error(NSI_LOG_CORE, EPERM, 
                "client(0x%x) is the service(%d, %d) provider", 
                c->cid, s->serv, s->inst);
        };

        nai_errno = EPERM;
        r = -1;
        goto _end;
    };

    /* find and create event */
    n = nsi_service_find_event(s, eid, &parent);
    if (n[0] != 0) {
        e = nai_containof(n[0], nsi_event_t, ent);

        /* lookup event ref */
        ent = e->refs.next;
        for ( ; ent != &e->refs; ent = ent->next) {
            f = nai_containof(ent, nsi_event_ref_t, ente);
            if (f->client == c) {
                break;
            };
        };
        if (ent != &e->refs) {
            if (f->offer != offer) {
                nai_log_error(NSI_LOG_CORE, EPERM, 
                    "client(0x%x) already to %s event to service(%d, %d)", 
                    c->cid, f->offer ? "offer" : "request", s->serv, s->inst);
                nai_errno = EPERM;
                r = -1;
                goto _end;
            };
        };

        entf = ent;
        init = 0;

    } else {
        e = (nsi_event_t*)nsi_routing_alloc(s->rt, sizeof(*e));
        if (e == 0) {
            nai_log_alert(NSI_LOG_CORE, 
                nai_errno, "failed to allocate event");
            r = -1;
            goto _end;
        };

        e->eid = eid;
        e->msg = 0;
        e->own = 0;
        e->serv = s;
        e->type = NSI_ET_UNKNOWN;
        e->reliability = NSI_RT_UNKNOWN;
        e->sent_to_routing = 0;
        nai_list_init(&e->list);
        nai_list_init(&e->refs);
        nai_rbtree_link(&s->events, &e->ent, parent, n);
        nai_rbtree_color(&s->events, &e->ent);

        entf = &e->refs;
        init = 1;
    };

    if (init || offer) {
        r = nsi_routing_get_event_info(
            s->rt, s->serv, s->inst, eid, &ei);
        if (r >= 0) {
            e->type = ei.type;
            e->reliability = ei.reliability;
        };
        if (e->type == NSI_ET_UNKNOWN) {
            e->type = info->type;
        };
        if (e->type == NSI_ET_UNKNOWN && offer) {
            e->type = NSI_ET_EVENT;
        };
        if (e->reliability == NSI_RT_UNKNOWN) {
            e->reliability = info->reliability;
        };
        if (e->reliability == NSI_RT_UNKNOWN && offer) {
            e->reliability = NSI_RT_BOTH;
        };
    };


    r = nsi_routing_batch_begin(s->rt);
    if (r < 0) {
        nai_log_alert(NSI_LOG_CORE, 
            nai_errno, "start batch failed");
        goto _fail;
    };


    /* alloc event link and group
     * we link them after all event link are allocated.
     */
    nai_list_init(&list);
    for (m = 0; m < info->ngroup; m ++) {

        gid = info->groups[m];

        /* find in exists list */
        ent = e->list.next;
        for ( ; ent != &e->list; ent = ent->next) {
            l = nai_containof(ent, nsi_event_link_t, ente);
            if (l->group->gid == gid) {
                nsi_service_update_group_multicast(s, l->group);
                break;
            };
        };
        if (ent != &e->list) {
            continue;
        };

        /* find in new list */
        ent = list.next;
        for ( ; ent != &list; ent = ent->next) {
            l = nai_containof(ent, nsi_event_link_t, ente);
            if (l->group->gid == gid) {
                nsi_service_update_group_multicast(s, l->group);
                break;
            };
        };
        if (ent != &list) {
            continue;
        };

        /* alloc event link */
        l = (nsi_event_link_t*)nsi_routing_alloc(s->rt, sizeof(*l));
        if (l == 0) {
            nai_log_alert(NSI_LOG_CORE, 
                nai_errno, "failed to allocate event link");
            goto _fail;
        };

        l->event = e;
        l->group = 0;
        nai_list_insert_tail(&list, &l->ente);


        /* find and create eventgroup */
        n = nsi_service_find_eventgroup(s, gid, &parent);
        if (n[0] != 0) {
            g = nai_containof(n[0], nsi_event_group_t, ent);
        } else {
            g = (nsi_event_group_t*)nsi_routing_alloc(s->rt, sizeof(*g));
            if (g == 0) {
                goto _fail;
            };

            /* init group */
            g->gid = gid;
            g->serv = s;
            g->count = 0;
            g->remote = 0;
            g->threshold = 0;
            g->mcast = 0;
            g->ep.ep = 0;
            g->ep.conn = 0;
            g->ep.serv = s->serv;
            nai_list_init(&g->list);
            nai_list_init(&g->subs);

            /* set group multicast */
            nsi_service_update_group_multicast(s, g);

            nai_rbtree_link(&s->groups, &g->ent, parent, n);
            nai_rbtree_color(&s->groups, &g->ent);

        };

        l->group = g;


        if (!nai_list_is_empty(&g->list)) {
            continue;
        };
        if (!(s->local && !s->proxy) || 
            !(s->own)) {
            continue;
        };

        /* add preset subscriptions */
        r = nsi_service_add_preset(s, g);
        if (r < 0) {
            nai_log_error(NSI_LOG_CORE, 
                nai_errno, "add preset subscriptions failed");
            goto _fail;
        };
    };


    /* no event ref exist, we alloc a new ref to link client with event */
    if (entf != &e->refs) {
        f = nai_containof(entf, nsi_event_ref_t, ente);
    } else {
        f = (nsi_event_ref_t*)nsi_routing_alloc(s->rt, sizeof(*c));
        if (f == 0) {
            nai_log_alert(NSI_LOG_CORE, 
                nai_errno, "failed to allocate event ref");
            goto _fail;
        };

        f->event = e;
        f->client = c;
        nai_list_insert_tail(&e->refs, &f->ente);
        nai_list_insert_tail(&c->events, &f->entc);
    };

    f->offer = offer;
    if (f->offer) {
        e->own = f;
    };


    /* link event with groups */
    ent = list.next;
    for ( ; ent != &list; ) {
        l = nai_containof(ent, nsi_event_link_t, ente);
        ent = ent->next;

        g = l->group;
        nai_list_insert_tail(&e->list, &l->ente);
        nai_list_insert_tail(&g->list, &l->entg);
    };


    /* do register */
    r = s->ops->event(s, 
        offer ? NSI_SERVICE_OP_OFFER : NSI_SERVICE_OP_ADD, 
        c, eid, info);
    if (r < 0) {
        nai_log_error(NSI_LOG_CORE, 
            nai_errno, "failed to send command of %s event", 
            offer ? "offer" : "request");

        /* ignore, we can resent later */
        r = 0;
    };


    nsi_routing_batch_end(s->rt);

_end:
    return r;


_fail:
    ec = nai_errno;

    /* free all event link and event group */
    ent = list.next;
    for ( ; ent != &list; ) {
        l = nai_containof(ent, nsi_event_link_t, ente);
        ent = ent->next;
        g = l->group;

        /* free link */
        nsi_routing_free(s->rt, l);

        /* is empty group */
        if (g && nai_list_is_empty(&g->list)) {
            /* free preset subscriptions */
            if (s->local && !s->proxy) {
                nsi_service_remove_preset(s, g);
            };

            /* free empty group */
            if (nai_list_is_empty(&g->subs)) {
                nai_rbtree_erase(&s->groups, &g->ent);
                nsi_service_free_group(s, g);
            };
        };
    };

    /* free empty event */
    if (nai_list_is_empty(&e->refs) && 
        nai_list_is_empty(&e->list)) {
        nai_rbtree_erase(&s->events, &e->ent);
        nsi_service_free_event(s, e);
    };

    nsi_routing_batch_end(s->rt);

    nai_errno = ec;
    r = -1;
    goto _end;
};


int nsi_service_unregister(nsi_service_t* s, 
    nsi_client_t* c, nsi_eid_t eid)
{
    int r;
    int owned;
    nai_list_entry_t* ent;
    nsi_event_t* e;
    nsi_event_group_t* g;
    nsi_event_ref_t* f;
    nsi_event_link_t* l;


    nai_log_info(NSI_LOG_CORE, 0, 
        "client(0x%x) release event from service(%d, %d)", 
        c->cid, s->serv, s->inst);

    /* lookup event ref */
    ent = c->events.next;
    for ( ; ent != &c->events; ent = ent->next) {
        f = nai_containof(ent, nsi_event_ref_t, entc);
        if (f->event->eid == eid && 
            f->event->serv == s) {
            break;
        };
    };
    if (ent == &c->events) {
        nai_log_info(NSI_LOG_CORE, ENOENT, 
            "client(0x%x) did not request event(%d) from service(%d, %d)", 
            c->cid, eid, s->serv, s->inst);
        r = 0;
        goto _end;
    };


    /* unlink owner */
    owned = 0;
    e = f->event;
    if (e->own == f) {
        e->own = 0;

        /* clear message */
        if (e->msg) {
            nsi_message_close(e->msg);
            e->msg = 0;
        };

        owned = 1;
    };


    /* free event ref */
    nai_list_entry_remove(&f->entc);
    nai_list_entry_remove(&f->ente);
    nsi_routing_free(s->rt, f);


    /* free empty event */
    if (nai_list_is_empty(&e->refs)) {

        /* enumerates event links */
        ent = e->list.next;
        for ( ; ent != &e->list; ) {
            l = nai_containof(ent, nsi_event_link_t, ente);
            ent = ent->next;
            g = l->group;


            /* free event link */
            nai_list_entry_remove(&l->ente);
            nai_list_entry_remove(&l->entg);
            nsi_routing_free(s->rt, l);


            /* is empty group */
            if (!nai_list_is_empty(&g->list) || 
                !nai_list_is_empty(&g->subs)) {
                continue;
            };


            /* free empty group */
            nai_rbtree_erase(&s->groups, &g->ent);
            nsi_service_free_group(s, g);
        };

        nai_rbtree_erase(&s->events, &e->ent);
        nsi_service_free_event(s, e);
    };

    /* do unregister */
    r = s->ops->event(s, 
        owned ? NSI_SERVICE_OP_REMOVE_OFFER : NSI_SERVICE_OP_REMOVE, 
        c, eid, 0);
    if (r < 0) {
        nai_log_error(NSI_LOG_CORE, 
            nai_errno, "failed to send command of release event");

        /* ignore, we can resent later */
        r = 0;
    };


_end:
    return r;
};


int nsi_service_subscribe(nsi_service_t* s, 
    nsi_client_t* c, 
    nsi_gid_t group, nsi_version_t major, nsi_eid_t event, 
    uint32_t ttl, uint32_t init, 
    const nsi_endpoint_name_t names[2])
{
    int r;
    int m;
    int ec;
    uint64_t now;
    nai_list_entry_t* ent;
    nai_rbnode_t** n;
    nai_rbnode_t* parent;
    nsi_event_group_t* g;
    nsi_subscription_t* u;


    /* PRS_SOMEIPSD_00453 */

    /* nai_log_info(NSI_LOG_CORE, 0,
     *     "client(0x%x) subscribe event group(%d) of service(%d, %d)",
     *     c->cid, group, s->serv, s->inst);
     */

    /* PRS_SOMEIPSD_00129 */
    n = nsi_service_find_eventgroup(s, group, &parent);
    if (n[0] == 0) {
        nai_log_error(NSI_LOG_CORE, ENOENT, 
            "can't find event group(%d) of service(%d, %d)", 
            group, s->serv, s->inst);
        nai_errno = ENOENT;
        r = -1;
        goto _end;
    };

    /* PRS_SOMEIPSD_00828
     * PRS_SOMEIPSD_00566
     */
    if (nsi_service_is_onlined(s) && 
        major != s->major) {
        nai_log_warn(NSI_LOG_CORE, EINVAL, 
            "client(0x%x) subscribe event group(%d) of service(%d, %d) "
            "with different version %d, previous is %d", 
            c->cid, group, s->serv, s->inst, major, s->major);
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    /* PRS_SOMEIPSD_00129 */
    g = nai_containof(n[0], nsi_event_group_t, ent);
    ent = g->subs.next;
    for ( ; ent != &g->subs; ent = ent->next) {
        u = nai_containof(ent, nsi_subscription_t, entg);
        if (u->client == c) {
            break;
        };
    };
    if (ent == &g->subs) {
       nai_log_info(NSI_LOG_CORE, 0,
           "client(0x%x) subscribe event group(%d) of service(%d, %d)",
           c->cid, group, s->serv, s->inst);

        u = (nsi_subscription_t*)nsi_routing_alloc(s->rt, sizeof(*u));
        if (u == 0) {
            nai_log_alert(NSI_LOG_CORE, 
                nai_errno, "failed to allocate subscription");
            r = -1;
            goto _end;
        };

        u->serv = s;
        u->group = g;
        u->client = c;
        u->conn[0] = 0;
        u->conn[1] = 0;
        u->expire = -1;
        u->major = major;
        u->session = 0;
        u->ac = u->ar = 0;
        u->stat = NSI_SUBSCRIBE_REMOVED;
        u->count = 0;
        u->total = 0;
        u->events = 0;
        nai_list_insert_tail(&c->subs, &u->entc);
        nai_list_insert_tail(&g->subs, &u->entg);

    } else {
       nai_log_debug(NSI_LOG_CORE, 0,
           "client(0x%x) subscribe event group(%d) of service(%d, %d)",
           c->cid, group, s->serv, s->inst);

        now = nai_tickcache_to_msec();
        if (u->expire <= now) {
            r = nsi_service_subevent_nack(s, u, NSI_EVENT_ANY);
            if (r < 0) {
                nai_log_error(NSI_LOG_CORE, nai_errno, 
                    "update subscribe group(%d) of service(%d, %d) failed", 
                    group, s->serv, s->inst);
                goto _end;
            };
        };

        if (u->major != major) {
            nai_log_error(NSI_LOG_CORE, EINVAL, 
                "client(0x%x) resubscribe event group(%d) of service(%d, %d) " 
                "with different version %d, previous is %d", 
                c->cid, group, s->serv, s->inst, major, u->major);
            nai_errno = EINVAL;
            r = -1;
            goto _end;
        };
    };

    /* save ttl */
    u->ttl = ttl;
    /* save initial data flag */
    u->init = !!init;
    /* save session id */
    u->session = c->ep.sinternal;


    if (s->own) {
        /* if the group without multicast and the remote without a valid 
         * address, the remote will not receives the events, 
         * so send nack to it.
         * PRS_SOMEIPSD_00810
         */
        if (g->mcast == 0 && c->remote && (
            names == 0 || (names[0].addr == 0 && names[1].addr == 0))) {
            nai_errno = EINVAL;
            r = -1;
            goto _fail;
        };

        /* bind name
         * PRS_SOMEIPSD_00360
         */
        for (m = 0; m < (int)nai_countof(u->conn); m ++) {
            if (names) {
                r = nsi_service_bind_name(
                    s->ep[m].ep, NSI_EOPT_BIND_NAME, names+m, u->conn+m);
            } else {
                r = nsi_service_bind_name(
                    s->ep[m].ep, NSI_EOPT_BIND_NAME, 0, u->conn+m);
            };
            if (r < 0) {
                nai_log_error(NSI_LOG_CORE, nai_errno, 
                    "failed to bind client(0x%x) address(%d) "
                    "to subscribe group(%d) of service(%d, %d)", 
                    c->cid, m, group, s->serv, s->inst);
                goto _fail;
            };

            if (c->temp) {
                /* preset subscription, do not check */
                continue;
            };
            if (m != 1 || u->conn[m] == 0) {
                continue;
            };

            /* check tcp connection
             * PRS_SOMEIPSD_00566
             * PRS_SOMEIPSD_00131
             * PRS_SOMEIPSD_00234
             */
            r = nsi_endpoint_set_opt(
                s->ep[m].ep, NSI_EOPT_CHECK_CSTAT, u->conn[m]);
            if (r <= 0) {
                if (r >= 0) {
                    nai_errno = ENOTCONN;
                    r = -1;
                };

                nai_log_error(NSI_LOG_CORE, r < 0 ? nai_errno : 0, 
                    "tcp connection of client(0x%x) is not connect "
                    "to subscribe group(%d) of service(%d, %d)", 
                    c->cid, group, s->serv, s->inst);

                goto _fail;
            };
        };
    };


    r = nsi_service_subevent_add(s, u, event);
    if (r < 0) {
        nai_log_error(NSI_LOG_CORE, nai_errno, 
            "failed to add event to subscribe group(%d) of service(%d, %d)", 
            group, s->serv, s->inst);
        goto _fail;
    };


    /* do subscribe
     * sending a subscription request must be done with a connection 
     * to the server.
     * PRS_SOMEIPSD_00807
     * PRS_SOMEIPSD_00321
     * PRS_SOMEIPSD_00486
     * PRS_SOMEIPSD_00415
     */
    if (nsi_service_is_onlined(s) || 
        nsi_service_is_proxy(s)) {
        r = s->ops->subscribe(
            s, NSI_SERVICE_OP_ADD, c, group, major, event, ttl);
        if (r < 0) {
            nai_log_error(NSI_LOG_CORE, 
                nai_errno, "failed to send command of subscribe");

            /* ignore, we can resent later */
            r = 0;
        };
    };


_end:
    return r;

_fail:
    if (c->remote_cli) {
        ec = nai_errno;
        nai_list_entry_remove(&u->entc);
        nai_list_entry_remove(&u->entg);
        nsi_service_free_subscription(s, u);
        nai_errno = ec;
    };
    goto _end;
};


int nsi_service_unsubscribe(nsi_service_t* s, 
    nsi_client_t* c, 
    nsi_gid_t group, nsi_version_t major, nsi_eid_t event)
{
    int r;
    int last;
    int batch;
    nai_list_entry_t* ent;
    nai_rbnode_t** n;
    nai_rbnode_t* parent;
    nsi_event_group_t* g;
    nsi_subscription_t* u;


    /* PRS_SOMEIPSD_00388
     * PRS_SOMEIPSD_00453
     */

    nai_log_info(NSI_LOG_CORE, 0, 
        "client(0x%x) unsubscribe event(%d) in group(%d) of service(%d, %d)", 
        c->cid, event, group, s->serv, s->inst);

    /* PRS_SOMEIPSD_00129 */
    n = nsi_service_find_eventgroup(s, group, &parent);
    if (n[0] == 0) {
        nai_log_error(NSI_LOG_CORE, ENOENT, 
            "can't find event group(%d) of service(%d, %d)", 
            group, s->serv, s->inst);
        nai_errno = ENOENT;
        r = -1;
        goto _end;
    };

    /* PRS_SOMEIPSD_00574 */
    g = nai_containof(n[0], nsi_event_group_t, ent);
    ent = g->subs.next;
    for ( ; ent != &g->subs; ent = ent->next) {
        u = nai_containof(ent, nsi_subscription_t, entg);
        if (u->client == c) {
            break;
        };
    };
    if (ent == &g->subs) {
        nai_log_warn(NSI_LOG_CORE, ENOENT, 
            "client(0x%x) did not subscribe group(%d) of service(%d, %d)", 
            c->cid, group, s->serv, s->inst);
        r = 0;
        goto _end;
    };

    /* PRS_SOMEIPSD_00828
     * PRS_SOMEIPSD_00574
     * PRS_SOMEIPSD_00128
     */
    if (major != u->major) {
        nai_log_warn(NSI_LOG_CORE, EINVAL, 
            "client(0x%x) unsubscribe event group(%d) of service(%d, %d) "
            "with different version %d, previous is %d", 
            c->cid, group, s->serv, s->inst, major, u->major);

        if (c->remote_cli == 0) {
            nai_errno = EINVAL;
            r = -1;
            goto _end;
        };
    };


    r = nsi_service_subevent_remove(s, u, event);
    if (r <= 0) {
        if (r < 0) {
            nai_log_error(NSI_LOG_CORE, nai_errno, 
                "failed to remove "
                "event(%d) from subscribe group(%d) of service(%d, %d)", 
                event, group, s->serv, s->inst);
        };
        goto _end;
    };


    last = 0;
    batch = 0;
    if (u->stat == NSI_SUBSCRIBE_REMOVED && 
        u->count <= 0) {

        r = nsi_routing_batch_begin(s->rt);
        if (r < 0) {
            nai_log_alert(NSI_LOG_CORE, 
                nai_errno, "start batch failed");
            goto _end;
        };

        batch = 1;

        nai_list_entry_remove(&u->entc);
        nai_list_entry_remove(&u->entg);
        nsi_service_free_subscription(s, u);
        last = 1;
    };

    /* do unsubscribe
     * sending a subscription request must be done with a connection 
     * to the server.
     */
    if (nsi_service_is_onlined(s) || 
        nsi_service_is_proxy(s)) {
        r = s->ops->subscribe(s, 
            last ? NSI_SERVICE_OP_REMOVE_LAST : NSI_SERVICE_OP_REMOVE, 
            c, group, major, event, 0);
        if (r < 0) {
            nai_log_error(NSI_LOG_CORE, 
                nai_errno, "failed to send command of unsubscribe");

            /* ignore, we can resent later */
            r = 0;
        };
    };

    if (batch) {
        nsi_routing_batch_end(s->rt);
    };

_end:
    return r;
};


int nsi_service_subscribe_ack(nsi_service_t* s, 
    nsi_client_t* c, int sult, 
    nsi_gid_t group, nsi_version_t major, nsi_eid_t event, 
    uint32_t ttl, const nsi_endpoint_name_t* mcast)
{
    int r;
    int init;
    int batch;
    int change;
    nai_list_entry_t* ent;
    nai_rbnode_t** n;
    nai_rbnode_t* parent;
    nsi_routing_t* rt;
    nsi_event_t* e;
    nsi_event_group_t* g;
    nsi_event_link_t* l;
    nsi_subscription_t* u;


    /* PRS_SOMEIPSD_00453
     * PRS_SOMEIPSD_00462
     */

    (void)major;

    /* nai_log_info(NSI_LOG_CORE, 0,
     *     "client(0x%x) get subscription ack(%d) "
     *     "about event(%d) in group(%d) of service(%d, %d)",
     *     c->cid, sult, event, group, s->serv, s->inst);
     */

    n = nsi_service_find_eventgroup(s, group, &parent);
    if (n[0] == 0) {
        nai_log_error(NSI_LOG_CORE, ENOENT, 
            "can't find event group(%d) of service(%d, %d)", 
            group, s->serv, s->inst);
        nai_errno = ENOENT;
        r = -1;
        goto _end;
    };


    /* PRS_SOMEIPSD_00829 */
    g = nai_containof(n[0], nsi_event_group_t, ent);
    ent = g->subs.next;
    for ( ; ent != &g->subs; ent = ent->next) {
        u = nai_containof(ent, nsi_subscription_t, entg);
        if (u->client == c) {
            break;
        };
    };

    if (u->expire == (uint64_t)(-1)) {
       nai_log_info(NSI_LOG_CORE, 0,
           "client(0x%x) get subscription ack(%d) "
           "about event(%d) in group(%d) of service(%d, %d)",
           c->cid, sult, event, group, s->serv, s->inst);
    } else {
       nai_log_debug(NSI_LOG_CORE, 0,
           "client(0x%x) get subscription ack(%d) "
           "about event(%d) in group(%d) of service(%d, %d)",
           c->cid, sult, event, group, s->serv, s->inst);
    };

    if (ent == &g->subs) {
        nai_log_warn(NSI_LOG_CORE, ENOENT, 
            "client(0x%x) did not subscribe group(%d) of service(%d, %d) "
            "and the ack(%d) will be ignore", 
            c->cid, group, s->serv, s->inst, sult);
        nai_errno = ENOENT;
        r = -1;
        goto _end;
    };

    /* PRS_SOMEIPSD_00829 */
    if (major != u->major) {
        nai_log_warn(NSI_LOG_CORE, EINVAL, 
            "the version %d of ack is different from the version %d of "
            "client(0x%x) who subscribe event group(%d) of service(%d, %d)", 
            major, u->major, c->cid, group, s->serv, s->inst);

        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };


    if (sult == NSI_E_OK) {
        /* bind multicast
         * PRS_SOMEIPSD_00490
         * PRS_SOMEIPSD_00392
         */
        r = nsi_service_bind_mcast(s, g, mcast);
        if (r < 0) {
            nai_log_error(NSI_LOG_CORE, nai_errno, 
                "failed to bind multicast address "
                "to subscribe group(%d) of service(%d, %d)", 
                group, s->serv, s->inst);

            /* fixme: error ignored */
        };

        r = nsi_service_subevent_just(s, u, event);
        if (r < 0) {
            nai_log_error(NSI_LOG_CORE, nai_errno, 
                "update subscription ack to group(%d) "
                "of service(%d, %d) failed", 
                group, s->serv, s->inst);
            goto _fail;
        };

        /* update expire time */
        if (ttl == 0) {
            ttl = u->ttl;
        }
        if (ttl == (uint32_t)-1) {
            u->expire = -1;
        } else {
            u->expire = nai_tickcache_to_msec() + (uint64_t)ttl * 1000;
        };

        /* increase remote subscription to group */
        if (c->remote && u->ar == 0) {
            g->remote ++;
            u->ar = 1;
        };
        /* increase subscription to group */
        if (u->ac == 0) {
            g->count ++;
            u->ac = 1;
        };

    } else {
        r = nsi_service_subevent_nack(s, u, event);
        if (r < 0) {
            nai_log_error(NSI_LOG_CORE, nai_errno, 
                "update subscribe nack to group(%d) of service(%d, %d) failed", 
                group, s->serv, s->inst);
            goto _fail;
        };
    };

    change = r;


_cb:
    init = 0;
    rt = s->rt;
    r = nsi_routing_batch_begin(rt);
    if (r < 0) {
        nai_log_alert(NSI_LOG_CORE, 
            nai_errno, "start batch failed");
        batch = 0;
        goto _skip;
    };

    batch = 1;

    /* send initial events
     * PRS_SOMEIPSD_00121
     * PRS_SOMEIPSD_00123
     */
    if (sult == NSI_E_OK) {

        /* send initial event in case:
         * 1. explicit initial data contorl is 0
         * 2. explicit initial data contorl is 1, initial data is 1
         * PRS_SOMEIPSD_00464
         * PRS_SOMEIPSD_00822
         * PRS_SOMEIPSD_00824
         * PRS_SOMEIPSD_00391
         */
        if (s->rt->sd && s->rt->sd->version == 2311) {
            if (u->init == 0) {
                goto _skip;
            };
        } else {
            if (c->initial_expl && u->init == 0) {
                /* no changed, don't send initial event
                * PRS_SOMEIPSD_00121
                */
                if (change == 0) {
                    goto _skip;
                };
            };
        };

        /* check has event message */
        ent = g->list.next;
        for ( ; ent != &g->list; ent = ent->next) {
            l = nai_containof(ent, nsi_event_link_t, entg);
            e = l->event;
            if (e->msg == 0) {
                continue;
            };

            /* only send initial data for field
             * PRS_SOMEIPSD_00464
             * PRS_SOMEIPSD_00465
             * PRS_SOMEIPSD_00120
             * PRS_SOMEIPSD_00122
             */
            if (e->type != NSI_ET_FIELD) {
                continue;
            };

            /* prevent event from being repeatedly sent to route */
            if (e->sent_to_routing && s->proxy && c->proxy) {
                continue;
            };

            /* event is sent ? */
            if (c->remote) {
                /* 1. send initial events for different sd-message
                 * 2. don't repeat send initial events for one sd-message
                 * PRS_SOMEIPSD_00571
                 * PRS_SOMEIPSD_00572
                 */
                if (!nsi_service_subevent_are(
                    s, u, e->eid, NSI_SUBSCRIBE_JUST, 0)) {
                    continue;
                };
            } else {
                /* local and proxy client */
                if (!nsi_service_subevent_are(
                    s, u, e->eid, NSI_SUBSCRIBE_JUST, 1)) {
                    continue;
                };
            };

            init = 1;
        };

    } else {

        /* PRS_SOMEIPSD_00394 */
        init = u->init;

        /* PRS_SOMEIPSD_00393 */
        if (c->remote_cli) {
            nai_list_entry_remove(&u->entc);
            nai_list_entry_remove(&u->entg);
            nsi_service_free_subscription(s, u);
        } else if (u->ac) {
            u->ac = 0;
            g->count --;

            if (g->count <= 0) {
                r = nsi_service_bind_mcast(s, g, 0);
                if (r < 0) {
                    nai_log_error(NSI_LOG_CORE, nai_errno, 
                        "unbind multicast address "
                        "from subscribe group(%d) of service(%d, %d)", 
                        group, s->serv, s->inst);

                    /* fixme: error ignored */
                };
            };
        };

        /* nack, set ttl = 0 */
        ttl = 0;
    };

_skip:
    r = nsi_client_handle_subscribe_ack(
        c, sult, s->serv, s->inst, group, s->major, event, ttl, init, 0);
    if (r < 0) {
        nai_log_error(NSI_LOG_CORE, 
            nai_errno, "failed to send command of subscription ack");
    } else if (!init) {
        /* nothing to do */
        ;
    } else {
        ent = g->list.next;
        for ( ; ent != &g->list; ent = ent->next) {
            l = nai_containof(ent, nsi_event_link_t, entg);
            e = l->event;
            if (e->msg == 0) {
                continue;
            };

            /* only send initial data for field
             * PRS_SOMEIPSD_00464
             * PRS_SOMEIPSD_00465
             * PRS_SOMEIPSD_00120
             * PRS_SOMEIPSD_00122
             */
            if (e->type != NSI_ET_FIELD) {
                continue;
            };

            /* prevent event from being repeatedly sent to route */
            if (e->sent_to_routing && s->proxy && c->proxy) {
                continue;
            };

            /* event is sent ? */
            if (c->remote) {
                /* 1. send initial events for different sd-message
                 * 2. don't repeat send initial events for one sd-message
                 * PRS_SOMEIPSD_00571
                 * PRS_SOMEIPSD_00572
                 */
                if (!nsi_service_subevent_are(
                    s, u, e->eid, NSI_SUBSCRIBE_JUST, 0)) {
                    continue;
                };
            } else {
                /* local and proxy client */
                if (!nsi_service_subevent_are(
                    s, u, e->eid, NSI_SUBSCRIBE_JUST, 1)) {
                    continue;
                };
            };

            /* PRS_SOMEIPSD_00487 */
            r = nsi_service_notify_one(
                s, u, e, 0, NSI_SEND_POSTED|NSI_SEND_MSGDUP);
            if (r < 0) {
                nai_log_error(NSI_LOG_CORE, nai_errno, 
                    "send first event(%d) data to client(0x%x) failed", 
                    e->eid, c->cid);
                break;
            };
        };

        r = nsi_service_subevent_ack(s, u, NSI_EVENT_ANY);
        if (r < 0) {
            nai_log_error(NSI_LOG_CORE, nai_errno, 
                "update subscribe group(%d) of service(%d, %d) failed", 
                group, s->serv, s->inst);

            /* fixme: error ignored */
        };
    };

    if (batch) {
        nsi_routing_batch_end(rt);
    };

_end:
    return r;


_fail:
    sult = NSI_E_NOT_REACHABLE;
    goto _cb;
};


int nsi_service_handle_available(nsi_service_t* s, int avail)
{
    int r;


    r = s->ops->available(s, avail);

    return r;
};


int nsi_service_handle_linkstate(nsi_service_t* s, int on)
{
    int r;


    r = s->ops->linkstate(s, on);

    return r;
};


int nsi_service_handle_connected(
    nsi_service_t* s, nsi_endpoint_t* ep, nsi_connid_t conn)
{
    int r;
    int n;


    for (n = 0; n < (int)nai_countof(s->ep); n ++) {
        if (s->ep[n].ep == ep && s->ep[n].conn == conn) {
            s->ops->connected(s, n);
            break;
        };
    };

    r = 0;

    return r;
};


int nsi_service_handle_disconnected(
    nsi_service_t* s, nsi_endpoint_t* ep, nsi_connid_t conn)
{
    int r;
    int n;
    nai_rbnode_t* node;
    nai_list_entry_t* ent;
    nsi_client_t* c;
    nsi_routing_t* rt;
    nsi_event_group_t* g;
    nsi_subscription_t* u;


    rt = s->rt;
    r = nsi_routing_batch_begin(rt);
    if (r < 0) {
        nai_log_alert(NSI_LOG_CORE, 
            nai_errno, "start batch failed");
        goto _end;
    };

    n = 0;

    if (s->local && s->proxy == 0 &&    /* is local server */
        s->ep[1].ep == ep) {            /* is tcp */
        n = 1;

        /* for each groups */
        node = nai_rbtree_begin(&s->groups);
        for ( ; node != nai_rbtree_end(&s->groups); ) {
            g = nai_containof(node, nsi_event_group_t, ent);
            node = nai_rbtree_next(node);

            /* for each subscriptions */
            ent = g->subs.next;
            for ( ; ent != &g->subs; ) {
                u = nai_containof(ent, nsi_subscription_t, entg);
                ent = ent->next;

                if (u->conn[1] != conn) {
                    continue;
                };

                c = u->client;
                if (c->temp) {
                    /* preset subscription, do not remove */
                    continue;
                };

                /* the remote is disconnection, auto remove subscription */
                nai_list_entry_remove(&u->entg);
                nai_list_entry_remove(&u->entc);
                nsi_service_free_subscription(s, u);

                /* forward 'unsubscribe' to owner */
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
        };
    };

    for ( ; n < (int)nai_countof(s->ep); n ++) {
        if (s->ep[n].ep == ep && s->ep[n].conn == conn) {
            s->ops->disconnected(s, n);
            break;
        };
    };

    nsi_routing_batch_end(rt);
    r = 0;

_end:
    return r;
};


