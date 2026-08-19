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
/// @file       nsi_service_server.c
/// @brief
/// @details
/// @date       2021-05-20
/// @author     xn
/// @version    1.2.0
///
/// ================================================================

#include "nsi_service.h"
#include "nsi_client.h"
#include "nsi_discovery.h"
#include "nsi_routing_impl.h"
#include "nsi_log.h"
#include "nai/runtime/nai_errno.h"
#include "nai/runtime/nai_util.h"



static int nsi_service_server_open(nsi_service_t* s);
static int nsi_service_server_close(nsi_service_t* s);
static int nsi_service_server_offer(nsi_service_t* s, nsi_client_t* c);
static int nsi_service_server_stop(nsi_service_t* s, nsi_client_t* c);
static int nsi_service_server_send(nsi_service_t* s, nsi_message_t* m);
static int nsi_service_server_request(nsi_service_t* s, 
    int op, nsi_client_t* c, nsi_version_t major, nsi_versmin_t minor);
static int nsi_service_server_event(nsi_service_t* s, 
    int op, nsi_client_t* c, nsi_eid_t eid, const nsi_event_info_t* info);
static int nsi_service_server_subscribe(nsi_service_t* s, 
    int op, nsi_client_t* c, nsi_gid_t gid, 
    nsi_version_t major, nsi_eid_t eid, uint32_t ttl);
static int nsi_service_server_available(nsi_service_t* s, int avail);
static int nsi_service_server_linkstate(nsi_service_t* s, int on);
static int nsi_service_server_connected(nsi_service_t* s, int idx);
static int nsi_service_server_disconnected(nsi_service_t* s, int idx);



nsi_service_ops_t nsi_service_server_ops = {
    "server", 
    nsi_service_server_open, 
    nsi_service_server_close, 
    nsi_service_server_offer, 
    nsi_service_server_stop, 
    nsi_service_server_send, 
    nsi_service_server_request, 
    nsi_service_server_event, 
    nsi_service_server_subscribe, 
    nsi_service_server_available, 
    nsi_service_server_linkstate, 
    nsi_service_server_connected, 
    nsi_service_server_disconnected, 
};


extern int nsi_service_add_all_preset(nsi_service_t* s);
extern int nsi_service_remove_all_preset(nsi_service_t* s);


static int nsi_service_server_bind(nsi_service_t* s, 
    const nsi_service_info_t* si, nsi_client_t* c)
{
    int r;
    int n;
    int ec;
    nsi_endpoint_t* e;
    nsi_discovery_t* sd;


    /* open endpoints
     * PRS_SOMEIPSD_00362
     */
    e = 0;
    for (n = 0; n < (int)nai_countof(s->ep); n ++) {
        if (si->names[n].addr == 0) {
            continue;
        };

        e = nsi_routing_create_server_by_info(s->rt, si, n, 0);
        if (e == 0) {
            ec = nai_errno;
            nai_log_error(NSI_LOG_CORE, ec, 
                "the service(%d, %d) open server endpoint(%d) failed", 
                s->serv, s->inst, n);

            r = -1;
            goto _fail;
        };

        r = nsi_service_bind_endpoint(s, n, e, 0);
        if (r < 0) {
            ec = nai_errno;
            nai_log_error(NSI_LOG_CORE, ec, 
                "the service(%d, %d) bind server endpoint(%d) failed", 
                s->serv, s->inst, n);
            goto _fail;
        };

        e = 0;
    };

    /* add all preset subscriptions */
    r = nsi_service_add_all_preset(s);
    if (r < 0) {
        ec = nai_errno;
        nai_log_error(NSI_LOG_CORE, ec, 
            "the service(%d, %d) add preset subscriptions failed", 
            s->serv, s->inst);
        goto _fail;
    };

    /* send offer service */
    sd = s->rt->sd;
    if (sd != 0) {
        r = nsi_discovery_offer_service(sd, c, s->serv, s->inst, si);
        if (r < 0) {
            ec = nai_errno;
            nai_log_error(NSI_LOG_CORE, ec, 
                "start to offer service(%d, %d) failed", 
                s->serv, s->inst);
            goto _fail;
        };
    };

    r = 0;

_end:
    return r;

_fail:
    if (e) {
        nsi_routing_release_endpoint(e);
    };

    nsi_service_remove_all_preset(s);
    nsi_service_unbind_endpoints(s);
    nai_errno = ec;
    goto _end;
};


static int nsi_service_server_unbind(nsi_service_t* s, nsi_client_t* c)
{
    int r;
    nsi_discovery_t* sd;


    sd = s->rt->sd;
    if (sd) {
        r = nsi_discovery_stop_service(sd, c, s->serv, s->inst);
        if (r < 0) {
            assert(0);
            nai_log_error(NSI_LOG_CORE, nai_errno, 
                "stop offer service(%d, %d) failed", 
                s->serv, s->inst);
            goto _end;
        };
    };

    nsi_service_remove_all_preset(s);
    nsi_service_unbind_endpoints(s);
    r = 0;

_end:
    return r;
};


static int nsi_service_server_open(nsi_service_t* s)
{
    s->stat = NSI_SERVICE_OPENED;
    return 0;
};


static int nsi_service_server_close(nsi_service_t* s)
{
    int r;
    nsi_discovery_t* sd;


    /* for nsi_service_reopen */
    if (nsi_service_is_started(s)) {
        sd = s->rt->sd;
        if (sd != 0) {
            r = nsi_discovery_stop_service(sd, 0, s->serv, s->inst);
            if (r < 0) {
                nai_log_error(NSI_LOG_CORE, nai_errno, 
                    "stop offer service(%d, %d) failed", 
                    s->serv, s->inst);

                goto _end;
            };
        };
    };

    if (nsi_service_is_opened(s)) {
        s->ops = 0;
        s->stat = NSI_SERVICE_CLOSED;
        nsi_service_unbind_endpoints(s);
    };

    r = 0;

_end:
    return r;
};


static int nsi_service_server_offer(nsi_service_t* s, nsi_client_t* c)
{
    int r;
    int ec;
    nsi_client_t* o;
    nsi_service_info_t si;


    r = nsi_routing_get_service_info(
        s->rt, s->serv, s->inst, &si, 1);
    if (r < 0) {
        nai_log_error(NSI_LOG_CORE, nai_errno, 
            "get information of service(%d, %d) failed", 
            s->serv, s->inst);
        goto _end;
    };

    /* check service owner */
    if (si.owner && si.owner != c->cid) {
        o = nsi_client_find_in_routing(s->rt, si.owner);
        assert(o != 0);
        if (o == 0 || 
            o->ep.ep != c->ep.ep || 
            o->ep.conn != c->ep.conn) {
            ec = EPERM;
            nai_log_error(NSI_LOG_CORE, ec, 
                "the service(%d, %d) is not belong client(0x%x)", 
                s->serv, s->inst, c->cid);
            nai_errno = ec;
            r = -1;
            goto _end;
        };
    };


    r = nsi_routing_batch_begin(s->rt);
    if (r < 0) {
        nai_log_alert(NSI_LOG_CORE, 
            nai_errno, "start batch failed");
        goto _end;
    };

    if (s->rt->link) {
        r = nsi_service_server_bind(s, &si, c);
        if (r < 0) {
            ec = nai_errno;
            nai_log_error(NSI_LOG_CORE, ec, 
                "service(%d, %d) bind addresses failed", 
                s->serv, s->inst);
            goto _fail;
        };
    };

    /* switch to onlined */
    s->major = si.major;
    s->minor = si.minor;
    s->stat = NSI_SERVICE_ONLINED;

    /* do notify and sync */
    nsi_service_available(s, NSI_AVAIL_ONLINED);
    nsi_routing_batch_end(s->rt);
    r = 0;


_end:
    return r;

_fail:
    nsi_routing_batch_end(s->rt);
    nai_errno = ec;
    r = -1;
    goto _end;
};


static int nsi_service_server_stop(nsi_service_t* s, nsi_client_t* c)
{
    int r;
    int ec;


    if (!nsi_service_is_started(s)) {
        r = 0;
        goto _end;
    };

    r = nsi_routing_batch_begin(s->rt);
    if (r < 0) {
        nai_log_alert(NSI_LOG_CORE, 
            nai_errno, "start batch failed");
        goto _end;
    };

    s->stat = NSI_SERVICE_OPENED;
    r = nsi_service_available(s, NSI_AVAIL_OFFLINED);
    if (r < 0) {
        ec = nai_errno;
        nai_log_error(NSI_LOG_CORE, ec, 
            "failed to notify clients that service(%d, %d) is offlined", 
            s->serv, s->inst);
        goto _fail;
    };

    r = nsi_service_server_unbind(s, c);
    if (r < 0) {
        ec = nai_errno;
        nai_log_error(NSI_LOG_CORE, ec, 
            "service(%d, %d) unbind addresses failed", 
            s->serv, s->inst);
        goto _fail;
    };

    nsi_routing_batch_end(s->rt);
    r = 0;

_end:
    return r;

_fail:
    nsi_routing_batch_end(s->rt);
    nai_errno = ec;
    r = -1;
    goto _end;
};


static int nsi_service_server_send(nsi_service_t* s, nsi_message_t* m)
{
    int r;
    nsi_client_t* c;
    nsi_service_conn_t* e;


    switch (m->hdr.type) {
    case NSI_MT_REQUEST:
    case NSI_MT_REQUEST_NO_RETURN:
        c = s->own->client;
        r = nsi_client_send(c, m, 0, 0);
        break;

    case NSI_MT_RESPONSE:
    case NSI_MT_ERROR:
        /* PRS_SOMEIPSD_00361 */
        e = &s->ep[m->reliable];
        if (e->ep == 0) {
            e = &s->ep[!m->reliable];
        };
        if (e->ep == 0) {
            nai_errno = ENOTCONN;
            r = -1;
            break;
        };

        r = nsi_endpoint_send(e->ep, m, 0, NSI_SEND_POSTED);
        break;

    case NSI_MT_NOTIFICATION:
        nai_log_debug(NSI_LOG_CORE, EINVAL, 
            "the notification should not be routed here");
        nai_errno = EINVAL;
        r = -1;
        break;

    default:
        nai_log_debug(NSI_LOG_CORE, EINVAL, 
            "the unknown type of message should not be routed here");
        nai_errno = EINVAL;
        r = -1;
        break;
    };

    return r;
};


static int nsi_service_server_request(nsi_service_t* s, 
    int op, nsi_client_t* c, nsi_version_t major, nsi_versmin_t minor)
{
    int r;
    int ec;
    nsi_service_ref_t* o;


    if (op == NSI_SERVICE_OP_ADD) {
        r = nsi_routing_grant_find(
            s->rt, c, s->serv, s->inst, major, minor);
        if (r < 0) {
            nai_log_debug(NSI_LOG_CORE, nai_errno, 
                "the client(0x%x) request service(%d, %d) is denied", 
                c->cid, s->serv, s->inst);
            goto _end;
        };
    };


    o = s->own;
    if (o == 0) {
        r = 0;
        goto _end;
    };

    /* begin batch */
    r = nsi_routing_batch_begin(s->rt);
    if (r < 0) {
        nai_log_alert(NSI_LOG_CORE, 
            nai_errno, "start batch failed");
        goto _end;
    };


    if (o->client->app) {
        if (nsi_service_is_onlined(s) && op == NSI_SERVICE_OP_ADD) {
            r = nsi_service_available_to_client(
                s, NSI_AVAIL_ONLINED, c, major, minor);
            if (r < 0) {
                goto _fail;
            };
        };

        r = 0;
    } else {
        r = nsi_client_handle_request(
            o->client, op, c, s->serv, s->inst, major, minor, 0);
    };


_fail:
    if (r < 0) {
        ec = nai_errno;
    };
    nsi_routing_batch_end(s->rt);
    if (r < 0) {
        nai_errno = ec;
    };

_end:
    return r;
};


static int nsi_service_server_event(nsi_service_t* s, 
    int op, nsi_client_t* c, nsi_eid_t eid, const nsi_event_info_t* info)
{
    int r;
    nsi_service_ref_t* o;


    o = s->own;
    if (o == 0) {
        r = 0;
        goto _end;
    };

    if (op == NSI_SERVICE_OP_OFFER || 
        op == NSI_SERVICE_OP_REMOVE_OFFER) {
        r = 0;
        goto _end;
    };

    r = nsi_client_handle_event(
        o->client, op, c, s->serv, s->inst, eid, info, 0);

_end:
    return r;
};


static int nsi_service_server_subscribe(nsi_service_t* s, 
    int op, nsi_client_t* c, 
    nsi_gid_t gid, nsi_version_t major, 
    nsi_eid_t eid, uint32_t ttl)
{
    int r;
    nsi_service_ref_t* o;


    o = s->own;
    if (o == 0) {
        r = 0;
        goto _end;
    };

    r = nsi_client_handle_subscribe(
        o->client, op, c, s->serv, s->inst, gid, major, eid, ttl, 0);

_end:
    return r;
};


static int nsi_service_server_available(nsi_service_t* s, int avail)
{
    int r;

    (void)s;
    (void)avail;

    /* do nothing */
    r = 0;

    return r;
};


static int nsi_service_server_linkstate(nsi_service_t* s, int on)
{
    int r;
    nai_rbnode_t* n;
    nai_list_entry_t* e;
    nsi_client_t* c;
    nsi_event_group_t* g;
    nsi_subscription_t* u;
    nsi_service_info_t si;


    if (s->own == 0) {
        r = 0;
        goto _end;
    };

    if (on) {
        r = nsi_routing_get_service_info(
            s->rt, s->serv, s->inst, &si, 1);
        if (r < 0) {
            nai_log_error(NSI_LOG_CORE, nai_errno, 
                "get information of service(%d, %d) failed", 
                s->serv, s->inst);
            goto _end;
        };

        r = nsi_service_server_bind(s, &si, s->own->client);
    } else {
        n = nai_rbtree_begin(&s->groups);
        for ( ; n != nai_rbtree_end(&s->groups); ) {
            g = nai_containof(n, nsi_event_group_t, ent);
            n = nai_rbtree_next(n);

            e = g->subs.next;
            for ( ; e != &g->subs; ) {
                u = nai_containof(e, nsi_subscription_t, entg);
                e = e->next;
                c = u->client;
                if (c->remote) {
                    r = nsi_service_unsubscribe(
                        s, u->client, g->gid, u->major, NSI_EVENT_ANY);
                    if (r < 0) {
                        nai_log_error(NSI_LOG_CORE, 
                            nai_errno, "remove a subscription failed");

                        /* fixed me: ignored error */
                    };
                };
            };
        };

        r = nsi_service_server_unbind(s, s->own->client);
    };
    if (r < 0) {
        nai_log_error(NSI_LOG_CORE, nai_errno, 
            "the service(%d, %d) switch to link state(%d) failed", 
            s->serv, s->inst, on);

        /* fixme: bind failed */
    };


_end:
    return r;
};


static int nsi_service_server_connected(nsi_service_t* s, int idx)
{
    int r;


    if (idx != 1) {
        r = 0;
        goto _end;
    };

    if (s->ep[1].stat == 0) {
        r = 0;
        goto _end;
    };

    s->ep[1].stat = 1;
    r = 0;


_end:
    return r;
};


static int nsi_service_server_disconnected(nsi_service_t* s, int idx)
{
    int r;


    if (idx != 1) {
        r = 0;
        goto _end;
    };

    s->ep[1].stat = 0;
    r = 0;


_end:
    return r;
};


