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
/// @file       nsi_service_client.c
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
#include "nai/os/nai_socket.h"
#include "nai/runtime/nai_errno.h"
#include "nai/runtime/nai_util.h"



static int nsi_service_client_open(nsi_service_t* s);
static int nsi_service_client_close(nsi_service_t* s);
static int nsi_service_client_offer(nsi_service_t* s, nsi_client_t* c);
static int nsi_service_client_stop(nsi_service_t* s, nsi_client_t* c);
static int nsi_service_client_send(nsi_service_t* s, nsi_message_t* m);
static int nsi_service_client_request(nsi_service_t* s, 
    int op, nsi_client_t* c, nsi_version_t major, nsi_versmin_t minor);
static int nsi_service_client_event(nsi_service_t* s, 
    int op, nsi_client_t* c, nsi_eid_t eid, const nsi_event_info_t* info);
static int nsi_service_client_subscribe(nsi_service_t* s, 
    int op, nsi_client_t* c, 
    nsi_gid_t gid, nsi_version_t major, 
    nsi_eid_t eid, uint32_t ttl);
static int nsi_service_client_available(nsi_service_t* s, int avail);
static int nsi_service_client_linkstate(nsi_service_t* s, int on);
static int nsi_service_client_connected(nsi_service_t* s, int idx);
static int nsi_service_client_disconnected(nsi_service_t* s, int idx);



nsi_service_ops_t nsi_service_client_ops = {
    "client", 
    nsi_service_client_open, 
    nsi_service_client_close, 
    nsi_service_client_offer, 
    nsi_service_client_stop, 
    nsi_service_client_send, 
    nsi_service_client_request, 
    nsi_service_client_event, 
    nsi_service_client_subscribe, 
    nsi_service_client_available, 
    nsi_service_client_linkstate, 
    nsi_service_client_connected, 
    nsi_service_client_disconnected, 
};


typedef struct nsi_service_client_s {
    nsi_endpoint_name_t names[2];
    char buf[2][64];
} nsi_service_client_t;


static int nsi_service_client_saveaddr(nsi_service_t* s)
{
    int r;
    int n;
    int changed;
    nsi_service_conn_t* c;
    nsi_service_client_t* ctx;
    nsi_service_client_t data;


    changed = 0;
    ctx = (nsi_service_client_t*)s->ctx;

    for (n = 0; n < (int)nai_countof(s->ep); n ++) {
        c = &s->ep[n];
        if (c->ep == 0) {
            data.names[n].len = 0;
            data.names[n].addr = 0;
        } else {
            data.names[n].len = sizeof(data.buf[n]);
            data.names[n].addr = (nai_sockaddr_t*)data.buf[n];
            r = nsi_endpoint_get_name(c->ep, c->conn, &data.names[n]);
            if (r < 0) {
                nai_log_alert(NSI_LOG_CORE, nai_errno, 
                    "get the bind name(%d) of service(%d, %d) failed", 
                    n, s->serv, s->inst);
                goto _end;
            };
        };

        if (data.names[n].addr && ctx->names[n].addr) {
            r = nsi_endpoint_name_equal(&ctx->names[n], &data.names[n]);
            if (r == 0) {
                changed = 1;
            };
        } else if (data.names[n].addr != ctx->names[n].addr) {
            changed = 1;
        };
    };

    if (changed) {
        for (n = 0; n < (int)nai_countof(s->ep); n ++) {
            ctx->names[n].len = data.names[n].len;
            if (data.names[n].addr == 0) {
                ctx->names[n].addr = 0;
            } else {
                ctx->names[n].addr = (nai_sockaddr_t*)ctx->buf[n];
                nai_memcpy(ctx->names[n].addr, 
                    data.names[n].addr, data.names[n].len);
            };
        };

        nai_log_info(NSI_LOG_CORE, 0, 
            "the bind name of service(%d, %d) is changed", 
            s->serv, s->inst);
    };

    r = changed;

_end:
    return r;
};


static int nsi_service_client_changed(nsi_service_t* s)
{
    int r;
    uint64_t now;
    nai_rbnode_t* node;
    nai_list_entry_t* ent;
    nsi_event_group_t* g;
    nsi_subscription_t* u;
    nsi_client_t* c;


    nai_log_info(NSI_LOG_CORE, 0, 
        "service(%d, %d)'s address is changed, unsubscribe all subscription", 
        s->serv, s->inst);

    /* begin batch */
    r = nsi_routing_batch_begin(s->rt);
    if (r < 0) {
        nai_log_alert(NSI_LOG_CORE, 
            nai_errno, "start batch failed");
        goto _end;
    };

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

            /* unsubscribe */
            r = s->ops->subscribe(s, 
                NSI_SERVICE_OP_REMOVE, c, g->gid, u->major, 
                NSI_EVENT_ANY, 0);
            if (r < 0) {
                nai_log_error(NSI_LOG_CORE, nai_errno, 
                    "service(%d, %d) failed to unsubscribe group", 
                    s->serv, s->inst);
            };
        };
    };

    nsi_routing_batch_end(s->rt);
    r = 0;

_end:
    return r;
};


static int nsi_service_client_connect(
    nsi_service_t* s, const nsi_service_info_t* si)
{
    int r;
    int n;
    int ec;
    int onlined;
    int changed;
    int connected;
    nsi_endpoint_t* ep[2];
    char buf[140];


    changed = 0;
    onlined = s->stat == NSI_SERVICE_ONLINED;

    assert(!si->local);

    /* begin batch */
    r = nsi_routing_batch_begin(s->rt);
    if (r < 0) {
        nai_log_alert(NSI_LOG_CORE, 
            nai_errno, "start batch failed");
        goto _end;
    };


    /* compare versions */
    if (si->major != s->major || 
        si->minor != s->minor) {
        changed = 1;
    };

    /* open endpoints
     * PRS_SOMEIPSD_00362
     */
    for (n = 0; n < (int)nai_countof(ep); n ++) {
        ep[n] = 0;
    };
    for (n = 0; n < (int)nai_countof(ep); n ++) {
        if (si->names[n].addr == 0) {
            continue;
        };

        /* PRS_SOMEIPSD_00380
         * PRS_SOMEIPSD_00320
         */
        ep[n] = nsi_routing_create_client_by_info(s->rt, si, n, NSI_EFLAG_WAIT);
        if (ep[n] == 0) {
            buf[0] = 0;
            nai_sockaddr_ntop(
                si->names[n].addr, si->names[n].len, 
                buf, sizeof(buf), NAI_ADDR_PORT|NAI_ADDR_ZANY);

            nai_log_error(NSI_LOG_CORE, nai_errno, 
                "the service(%d, %d) open client endpoint(%s) failed", 
                s->serv, s->inst, buf);
            goto _fail;
        };
        if (ep[n] != s->ep[n].ep) {
            changed = 1;
        };

        nai_log_info(NSI_LOG_CORE, 0,
            "the service(%d, %d) opened client endpoint(%s)", 
            s->serv, s->inst, nai_str(&ep[n]->host));
    };

    /* bind endpoints */
    connected = 1;
    for (n = 0; n < (int)nai_countof(ep); n ++) {
        r = nsi_service_bind_endpoint(s, n, ep[n], 0);
        if (r < 0) {
            nai_log_error(NSI_LOG_CORE, nai_errno, 
                "the service(%d, %d) bind client endpoint(%d) failed", 
                s->serv, s->inst, n);
            goto _fail;
        };

        if (ep[n] != 0) {
            ep[n] = 0;
            if (s->ep[n].stat == 0) {
                connected = 0;
            };
        };
    };


    /* update new stat */
    if (connected) {
        r = nsi_service_client_saveaddr(s);
        if (r < 0) {
            nai_log_alert(NSI_LOG_CORE, nai_errno, 
                "the service(%d, %d) get bind address failed", 
                s->serv, s->inst);

            goto _fail;
        };
        if (r > 0) {
            changed = 1;
        };

        s->stat = NSI_SERVICE_ONLINED;
    } else {
        s->stat = NSI_SERVICE_CONNECTING;
    };
    s->major = si->major;
    s->minor = si->minor;


    /* notify */
    if (onlined == 0) {
        changed = 0;
    };
    if (changed) {
        r = nsi_service_client_changed(s);
        if (r < 0) {
            nai_log_alert(NSI_LOG_CORE, nai_errno, 
                "the service(%d, %d) unsubscribe failed", 
                s->serv, s->inst);
        };
    };
    switch (s->stat) {
    case NSI_SERVICE_ONLINED:
        r = nsi_service_available(s, 
            changed ? NSI_AVAIL_CHANGED : NSI_AVAIL_ONLINED);
        break;

    default:
#if 0
        if (changed) {
            /* reconnecting*/
            r = nsi_service_available(s, NSI_AVAIL_OFFLINED);
        } else {
            r = 0;
        };
#else
        r = 0;
#endif
        break;
    };
    if (r < 0) {
        nai_log_error(NSI_LOG_CORE, 
            nai_errno, "failure to notify service available");
    };


    nsi_routing_batch_end(s->rt);
    r = 0;


_end:
    nai_log_info(NSI_LOG_CORE, 0,
            "the service(%d, %d) connect endpoint(%s) with r:%d, stat %d",
            s->serv, s->inst,
            ep[0]? nai_str(&ep[0]->host) : (ep[1]? nai_str(&ep[1]->host) : "NA"),
            r, s->stat);
    return r;


_fail:
    ec = nai_errno;

    nai_log_alert(NSI_LOG_CORE, ec, 
        "the service(%d, %d) connect failed", 
        s->serv, s->inst);

    for (n = 0; n < (int)nai_countof(ep); n ++) {
        if (ep[0] != 0) {
            nsi_routing_release_endpoint(ep[0]);
        };
    };


    /* notify offlined */
    s->stat = NSI_SERVICE_OPENED;
    if (onlined) {
        nsi_service_available(s, NSI_AVAIL_OFFLINED);
    };

    /* disconnect */
    nsi_service_unbind_endpoints(s);
    nsi_routing_batch_end(s->rt);
    nai_errno = ec;
    r = -1;
    goto _end;
};


static int nsi_service_client_disconnect(nsi_service_t* s, int keep)
{
    int r;
    int onlined;


    /* begin batch */
    r = nsi_routing_batch_begin(s->rt);
    if (r < 0) {
        nai_log_alert(NSI_LOG_CORE, 
            nai_errno, "start batch failed");
        goto _end;
    };

    /* notify offlined */
    onlined = s->stat == NSI_SERVICE_ONLINED;
    s->stat = keep ? NSI_SERVICE_DISCOVERY : NSI_SERVICE_OPENED;
    if (onlined) {
        nsi_service_available(s, NSI_AVAIL_OFFLINED);
    };

    /* disconnect */
    nsi_service_unbind_endpoints(s);
    nsi_routing_batch_end(s->rt);
    r = 0;

_end:
    return r;
};


static int nsi_service_client_open(nsi_service_t* s)
{
    int r;
    int n;
    nsi_service_ref_t* l;
    nsi_service_client_t* ctx;


    ctx = (nsi_service_client_t*)nsi_routing_alloc(s->rt, sizeof(*ctx));
    if (ctx == 0) {
        nai_log_alert(NSI_LOG_CORE, nai_errno, 
            "allocate context of service client failed");
        r = -1;
        goto _end;
    };

    for (n = 0; n < (int)nai_countof(ctx->names); n ++) {
        ctx->names[n].len = 0;
        ctx->names[n].addr = 0;
    };

    s->ctx = ctx;
    s->stat = NSI_SERVICE_OPENED;

    if (!nai_list_is_empty(&s->refs)) {
        /* for reopen */
        l = nai_containof(s->refs.next, nsi_service_ref_t, ents);
        r = s->ops->request(s, 
            NSI_SERVICE_OP_ADD, l->client, l->major, l->minor);
        if (r < 0) {
            nai_log_error(NSI_LOG_CORE, nai_errno, 
                "request service failed on reopening.");
        };
    };

    r = 0;

_end:
    return r;
};


static int nsi_service_client_close(nsi_service_t* s)
{
    int r;

    /* for nsi_service_reopen */
    if (nsi_service_is_started(s)) {
        if (nsi_service_is_avail(s)) {
            nsi_service_available(s, NSI_AVAIL_OFFLINED);
        };
    };

    if (s->ctx) {
        nsi_routing_free(s->rt, s->ctx);
        s->ctx = 0;
    };

    if (nsi_service_is_opened(s)) {
        s->ops = 0;
        s->stat = NSI_SERVICE_CLOSED;
        nsi_service_unbind_endpoints(s);
    };


    r = 0;

    return r;
};


static int nsi_service_client_offer(nsi_service_t* s, nsi_client_t* c)
{
    (void)s;
    (void)c;

    nai_errno = EPERM;
    return -1;
};


static int nsi_service_client_stop(nsi_service_t* s, nsi_client_t* c)
{
    (void)s;
    (void)c;

    nai_errno = EPERM;
    return -1;
};


static int nsi_service_client_send(nsi_service_t* s, nsi_message_t* m)
{
    int r;
    nsi_service_conn_t* e;


    switch (m->hdr.type) {
    case NSI_MT_REQUEST:
    case NSI_MT_REQUEST_NO_RETURN:

        /* do check before request */
        r = nsi_routing_grant_request(s->rt, m);
        if (r < 0) {
            nai_log_debug(NSI_LOG_CORE, nai_errno, 
                "the request(%d) of service(%d, %d) "
                "from the client(0x%x) is denied", 
                m->hdr.method, m->hdr.serv, m->inst, m->hdr.client);
            goto _end;
        };

        e = &s->ep[m->reliable];
        if (e->ep == 0) {
            e = &s->ep[!m->reliable];
        };

        m->conn = e->conn;
        r = nsi_endpoint_send(e->ep, m, 0, NSI_SEND_POSTED);
        break;

    case NSI_MT_RESPONSE:
    case NSI_MT_ERROR:
        nai_log_warn(NSI_LOG_CORE, EINVAL, 
            "the response should not be sent to server");
        nai_errno = EINVAL;
        r = -1;
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

_end:
    return r;
};


static int nsi_service_client_do_request(nsi_service_t* s, 
    nsi_client_t* c, nsi_version_t major, nsi_versmin_t minor)
{
    int r;
    nsi_discovery_t* sd;
    nsi_service_info_t si;


    /* do verify before request */
    r = nsi_routing_grant_find(
        s->rt, c, s->serv, s->inst, major, minor);
    if (r < 0) {
        nai_log_debug(NSI_LOG_CORE, nai_errno, 
            "the client(0x%x) request service(%d, %d) is denied", 
            c->cid, s->serv, s->inst);
        goto _end;
    };

    /* find */
    sd = s->rt->sd;
    if (sd != 0) {
        r = nsi_discovery_request_service(
            sd, c, s->serv, s->inst, major, minor);
        if (r < 0) {
            nai_log_error(NSI_LOG_CORE, nai_errno, 
                "start to find service(%d, %d) failed", 
                s->serv, s->inst);
            goto _end;
        };
    };


    if (nsi_service_is_started(s)) {
        if (nsi_service_is_avail(s) == 0) {
            /* waiting discovery */
            r = 0;
            goto _end;
        };

        r = nsi_service_available_to_client(
            s, NSI_AVAIL_ONLINED, c, major, minor);
        if (r < 0) {
            nai_log_error(NSI_LOG_CORE, nai_errno, 
                "failure to notify client(0x%x) that "
                "service(%d, %d) is available", c->cid, s->serv, s->inst);
        };
        goto _end;
    };


    /* find */
    if (sd != 0) {
        s->stat = NSI_SERVICE_DISCOVERY;
    };

    /* lookup service info */
    r = nsi_routing_get_service_info(s->rt, s->serv, s->inst, &si, 0);
    if (r < 0) {
        nai_log_debug(NSI_LOG_CORE, nai_errno, 
            "try get information of service(%d, %d) failed", 
            s->serv, s->inst);

        r = 0;
        goto _end;
    };


    /* start connect */
    r = nsi_service_client_connect(s, &si);


_end:
    return r;
};


static int nsi_service_client_do_release(nsi_service_t* s, 
    nsi_client_t* c, nsi_version_t major, nsi_versmin_t minor, int close)
{
    int r;
    nsi_discovery_t* sd;

    sd = s->rt->sd;
    if (sd != 0) {
        r = nsi_discovery_release_service(
            sd, c, s->serv, s->inst, major, minor);
        if (r < 0) {
            nai_log_error(NSI_LOG_CORE, nai_errno, 
                "stop find service(%d, %d) failed", 
                s->serv, s->inst);

            goto _end;
        };
    };

    if (nsi_service_is_started(s) && close) {
        nsi_service_client_disconnect(s, 0);
    };

    r = 0;

_end:
    return r;
};


static int nsi_service_client_request(nsi_service_t* s, 
    int op, nsi_client_t* c, nsi_version_t major, nsi_versmin_t minor)
{
    int r;


    switch (op) {
    case NSI_SERVICE_OP_ADD:
        r = nsi_service_client_do_request(s, c, major, minor);
        break;

    case NSI_SERVICE_OP_REMOVE:
        r = nsi_service_client_do_release(s, c, major, minor, 0);
        break;

    case NSI_SERVICE_OP_REMOVE_LAST:
        r = nsi_service_client_do_release(s, c, major, minor, 1);
        break;

    default:
        r = 0;
        break;
    };

    return r;
};


static int nsi_service_client_event(nsi_service_t* s, 
    int op, nsi_client_t* c, nsi_eid_t eid, const nsi_event_info_t* info)
{
    (void)s;
    (void)op;
    (void)c;
    (void)eid;
    (void)info;

    /* nothing todo */
    return 0;
};


static int nsi_service_client_subscribe(nsi_service_t* s, 
    int op, nsi_client_t* c, 
    nsi_gid_t gid, nsi_version_t major, 
    nsi_eid_t eid, uint32_t ttl)
{
    int r;
    int ec;
    nsi_routing_t* rt;
    nsi_discovery_t* sd;
    nsi_service_client_t* ctx;
    nsi_group_info_t gi;


    (void)eid;

    rt = s->rt;
    sd = rt->sd;
    if (sd == 0) {
        nai_errno = ENOENT;
        r = -1;
        goto _end;
    };

    /* send sd subscribe */
    switch (op) {
    case NSI_SERVICE_OP_ADD:
        /* do check before subscribe */
        r = nsi_routing_grant_subscribe(rt, c, s->serv, s->inst, gid, eid);
        if (r < 0) {
            ec = nai_errno;
            nai_log_debug(NSI_LOG_CORE, ec, 
                "the client(0x%x) subscribe "
                "event group(%d) of service(%d, %d) is denied", 
                c->cid, gid, s->serv, s->inst);
            goto _nack;
        };

        if (!nsi_routing_is_preset_service(rt, s->serv, s->inst)) {
            /* PRS_SOMEIPSD_00807
             * PRS_SOMEIPSD_00321
             * PRS_SOMEIPSD_00443
             */
            ctx = (nsi_service_client_t*)s->ctx;
            r = nsi_discovery_subscribe(sd, 
                c, s->serv, s->inst, gid, major, ctx->names);
        } else {
            /* a preset service */
            r = nsi_routing_get_group_info(
                rt, s->serv, s->inst, gid, &gi);
            if (r < 0) {
                ec = nai_errno;
                goto _nack;
            };

            r = nsi_service_subscribe_ack(
                s, c, NSI_E_OK, gid, s->major, eid, ttl, &gi.mcast);
        };
        break;
    case NSI_SERVICE_OP_REMOVE:
        /* do nothing, 
         * the client's subscription was not completely removed. 
         * it behaves differently than the request.
         */
        r = 0;
        break;
    case NSI_SERVICE_OP_REMOVE_LAST:
        /* the client's subscription was completely removed, 
         * do send unsubscribe.
         */
        if (!nsi_routing_is_preset_service(rt, s->serv, s->inst)) {
            r = nsi_discovery_unsubscribe(sd, 
                c, s->serv, s->inst, gid, major);
        } else {
            r = 0;
        };
        break;
    default:
        assert(0);
        nai_errno = EINVAL;
        r = -1;
        break;
    };

_end:
    return r;

_nack:
    nsi_service_subscribe_ack(
        s, c, NSI_E_NOT_OK, gid, major, eid, 0, 0);

    nai_errno = ec;
    goto _end;
};


static int nsi_service_client_available(nsi_service_t* s, int avail)
{
    int r;
    nsi_service_info_t si;


    nai_log_info(NSI_LOG_CORE, 0, 
        "the service(%d, %d) available changed, %d", 
        s->serv, s->inst, avail);

    /* do switch */
    switch (avail) {
    case NSI_AVAIL_CHANGED:
    case NSI_AVAIL_ONLINED:
        r = nsi_routing_get_service_info(s->rt, s->serv, s->inst, &si, 0);
        if (r < 0) {
            nai_log_error(NSI_LOG_CORE, nai_errno, 
                "get information of service(%d, %d) failed", 
                s->serv, s->inst);
            goto _end;
        };

        r = nsi_service_client_connect(s, &si);
        if (r < 0) {
            nai_log_error(NSI_LOG_CORE, 
                nai_errno, "connect to service(%d, %d) failed", 
                si.serv, si.inst);
        };
        break;

    case NSI_AVAIL_OFFLINED:
        r = nsi_service_client_disconnect(s, 1);
        if (r < 0) {
            nai_log_error(NSI_LOG_CORE, 
                nai_errno, "service disconnect failed");
        };
        break;
    default:
        assert(0);
        break;
    };

    r = 0;

_end:
    return r;
};


static int nsi_service_client_linkstate(nsi_service_t* s, int on)
{
    int r;

    (void)s;
    (void)on;

    /* do nothing */
    r = 0;

    return r;
};


static int nsi_service_client_connected(nsi_service_t* s, int idx)
{
    int r;
    int changed;


    if (s->ep[idx].stat) {
        r = 0;
        goto _end;
    };

    nai_log_info(NSI_LOG_CORE, 0, 
        "%s endpoint(%s) of the service(%d, %d) is connected", 
        idx ? "stream" : "dgram", 
        nai_str(&s->ep[idx].ep->host), s->serv, s->inst);

    /* are all endpoints connected */
    s->ep[idx].stat = 1;
    if (s->ep[!idx].ep && 
        s->ep[!idx].stat == 0) {
        nai_log_info(NSI_LOG_CORE, 0, 
            "not all endpoints of the service(%d, %d) are connected", 
            s->serv, s->inst);

        r = 0;
        goto _end;
    };

    /* get new address */
    r = nsi_service_client_saveaddr(s);
    if (r < 0) {
        nai_log_alert(NSI_LOG_CORE, nai_errno, 
            "the service(%d, %d) get bind address failed", 
            s->serv, s->inst);

        goto _end;
    };

    /* PRS_SOMEIPSD_00461 */
    changed = r;
    switch (s->stat) {
    case NSI_SERVICE_DISCOVERY:
    case NSI_SERVICE_CONNECTING:
        s->stat = NSI_SERVICE_ONLINED;
        r = nsi_service_available(s, NSI_AVAIL_ONLINED);
        break;

    case NSI_SERVICE_ONLINED:
        if (changed == 1) {
            r = nsi_service_client_changed(s);
            if (r < 0) {
                nai_log_alert(NSI_LOG_CORE, nai_errno, 
                    "the service(%d, %d) unsubscribe failed", 
                    s->serv, s->inst);
            };

            r = nsi_service_available(s, NSI_AVAIL_CHANGED);
            break;
        };

        /* fallthrough */

    default:
        r = 0;
        break;
    };
    if (r < 0) {
        nai_log_error(NSI_LOG_CORE, nai_errno, 
            "failed to notify clients that service is availabled");
    };

    r = 0;


_end:
    return r;
};


static int nsi_service_client_disconnected(nsi_service_t* s, int idx)
{
    int r;
    nsi_discovery_t* sd;


    if (idx != 1) {
        r = 0;
        goto _end;
    };

    nai_log_info(NSI_LOG_CORE, 0, 
        "the endpoint(%s) of the service(%d, %d) is disconnected", 
        nai_str(&s->ep[1].ep->host), s->serv, s->inst);

    /* tcp disconnected, the server has removed subscription, 
     * tell discovery remove subscription.
     * PRS_SOMEIPSD_00527
     */
    sd = s->rt->sd;
    if (sd != 0) {
        r = nsi_discovery_subscribe_cancel(sd, s->serv, s->inst);
        if (r < 0) {
            nai_log_error(NSI_LOG_CORE, nai_errno, 
                "cancel subscription of service(%d, %d) failed", 
                s->serv, s->inst);

            /* fixme: error ignored */
        };
    };

    assert(nsi_service_is_avail(s));
    s->stat = NSI_SERVICE_CONNECTING;
    s->ep[1].stat = 0;
    r = 0;


_end:
    return r;
};


