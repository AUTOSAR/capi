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
/// @file       nsi_service_proxy.c
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
#include "nsi_proxy.h"
#include "nsi_routing_impl.h"
#include "nsi_log.h"
#include "nai/runtime/nai_errno.h"
#include "nai/runtime/nai_util.h"



static int nsi_service_proxy_open(nsi_service_t* s);
static int nsi_service_proxy_close(nsi_service_t* s);
static int nsi_service_proxy_offer(nsi_service_t* s, nsi_client_t* c);
static int nsi_service_proxy_stop(nsi_service_t* s, nsi_client_t* c);
static int nsi_service_proxy_no_offer(nsi_service_t* s, nsi_client_t* c);
static int nsi_service_proxy_no_stop(nsi_service_t* s, nsi_client_t* c);
static int nsi_service_proxy_send(nsi_service_t* s, nsi_message_t* m);
static int nsi_service_proxy_request(nsi_service_t* s, 
    int op, nsi_client_t* c, nsi_version_t major, nsi_versmin_t minor);
static int nsi_service_proxy_event(nsi_service_t* s, 
    int op, nsi_client_t* c, nsi_eid_t eid, const nsi_event_info_t* info);
static int nsi_service_proxy_subscribe(nsi_service_t* s, 
    int op, nsi_client_t* c, 
    nsi_gid_t gid, nsi_version_t major, 
    nsi_eid_t eid, uint32_t ttl);
static int nsi_service_proxy_available(nsi_service_t* s, int avail);
static int nsi_service_proxy_linkstate(nsi_service_t* s, int on);
static int nsi_service_proxy_connected(nsi_service_t* s, int idx);
static int nsi_service_proxy_disconnected(nsi_service_t* s, int idx);



nsi_service_ops_t nsi_service_sproxy_ops = {
    "server-proxy", 
    nsi_service_proxy_open, 
    nsi_service_proxy_close, 
    nsi_service_proxy_offer, 
    nsi_service_proxy_stop, 
    nsi_service_proxy_send, 
    nsi_service_proxy_request, 
    nsi_service_proxy_event, 
    nsi_service_proxy_subscribe, 
    nsi_service_proxy_available, 
    nsi_service_proxy_linkstate, 
    nsi_service_proxy_connected, 
    nsi_service_proxy_disconnected, 
};


nsi_service_ops_t nsi_service_cproxy_ops = {
    "client-proxy", 
    nsi_service_proxy_open, 
    nsi_service_proxy_close, 
    nsi_service_proxy_no_offer, 
    nsi_service_proxy_no_stop, 
    nsi_service_proxy_send, 
    nsi_service_proxy_request, 
    nsi_service_proxy_event, 
    nsi_service_proxy_subscribe, 
    nsi_service_proxy_available, 
    nsi_service_proxy_linkstate, 
    nsi_service_proxy_connected, 
    nsi_service_proxy_disconnected, 
};



static int nsi_service_proxy_open(nsi_service_t* s)
{
    int r;
    int ec;
    nsi_endpoint_t* e;
    nsi_endpoint_name_t name;


    r = nsi_routing_get_localname(s->rt, &name);
    if (r < 0) {
        nai_log_error(NSI_LOG_CORE, 
            nai_errno, "the proxy get localname failed");
        goto _end;
    };


    e = nsi_routing_create_client(s->rt, &name, 1, NSI_EFLAG_WAIT, 0);
    if (e == 0) {
        nai_log_error(NSI_LOG_CORE, nai_errno, 
            "the proxy service(%d, %d) open endpoint failed", 
            s->serv, s->inst);
        r = -1;
        goto _end;
    };

    r = nsi_service_bind_endpoint(s, 1, e, 0);
    if (r < 0) {
        nai_log_error(NSI_LOG_CORE, nai_errno, 
            "the service(%d, %d) open proxy endpoint(%d) failed", 
            s->serv, s->inst, 1);
        goto _fail;
    };

    s->stat = NSI_SERVICE_OPENED;

_end:
    nai_log_info(NSI_LOG_CORE, 0,
            "the service(%d, %d) open endpoint(%s) with r:%d, stat %d",
            s->serv, s->inst, e? nai_str(&e->host) : "invalid", r, s->stat);
    return r;

_fail:
    if (e) {
        ec = nai_errno;
        nsi_routing_release_endpoint(e);
        nai_errno = ec;
    };
    goto _end;
};


static int nsi_service_proxy_close(nsi_service_t* s)
{
    int r;
    nsi_discovery_t* sd;


    /* for nsi_service_reopen */
    if (nsi_service_is_started(s)) {
        sd = s->rt->sd;
        if (sd != 0 && s->own) {
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


static int nsi_service_proxy_offer(nsi_service_t* s, nsi_client_t* c)
{
    int r;
    nsi_discovery_t* sd;
    nsi_service_conn_t* e;
    nsi_service_info_t si;


    r = nsi_routing_get_service_info(
        s->rt, s->serv, s->inst, &si, 1);
    if (r < 0) {
        nai_log_error(NSI_LOG_CORE, nai_errno, 
            "get information of service(%d, %d) failed", 
            s->serv, s->inst);
        goto _end;
    };


    /* do verify before offer */
    r = nsi_routing_grant_offer(
        s->rt, c, s->serv, s->inst, si.major, si.minor);
    if (r < 0) {
        nai_log_debug(NSI_LOG_CORE, nai_errno, 
            "the client(0x%x) offer service(%d, %d) is denied", 
            c->cid, s->serv, s->inst);
        goto _end;
    };


    r = nsi_routing_batch_begin(s->rt);
    if (r < 0) {
        nai_log_alert(NSI_LOG_CORE, 
            nai_errno, "start batch failed");
        goto _end;
    };

    e = &s->ep[1];
    if (e->stat) {
        r = nsi_proxy_offer(&e->c, 
            NSI_SERVICE_OP_OFFER, 
            c, s->serv, s->inst, si.major, si.minor);
        if (r < 0) {
            nai_log_error(NSI_LOG_CORE, nai_errno, 
                "the proxy service(%d, %d) send offer command failed", 
                s->serv, s->inst);

            /* ignore */
        };
    };

    /* start */
    s->major = si.major;
    s->minor = si.minor;
    s->stat = NSI_SERVICE_ONLINED;
    sd = s->rt->sd;
    if (sd != 0) {
        si.local = 1;
        nai_sockname_init(&si.names[0]);
        nai_sockname_init(&si.names[1]);
        nsi_discovery_offer_service(sd, c, s->serv, s->inst, &si);
    };

    /* do notify and sync */
    nsi_service_available(s, NSI_AVAIL_ONLINED);
    nsi_routing_batch_end(s->rt);
    r = 0;

_end:
    return r;
};


static int nsi_service_proxy_stop(nsi_service_t* s, nsi_client_t* c)
{
    int r;
    nsi_discovery_t* sd;
    nsi_service_conn_t* e;


    r = nsi_routing_batch_begin(s->rt);
    if (r < 0) {
        nai_log_alert(NSI_LOG_CORE, 
            nai_errno, "start batch failed");
        goto _end;
    };

    e = &s->ep[1];
    if (e->stat) {
        r = nsi_proxy_offer(&e->c, 
            NSI_SERVICE_OP_REMOVE, 
            c, s->serv, s->inst, s->major, s->minor);
        if (r < 0) {
            nai_log_error(NSI_LOG_CORE, nai_errno, 
                "the proxy service(%d, %d) send stop command failed", 
                s->serv, s->inst);

            /* ignore */
        };
    };

    /* stop */
    s->stat = NSI_SERVICE_OPENED;
    sd = s->rt->sd;
    if (sd != 0) {
        nsi_discovery_stop_service(sd, c, s->serv, s->inst);
    };

    /* do nack and notify */
    nsi_service_available(s, NSI_AVAIL_OFFLINED);
    nsi_routing_batch_end(s->rt);
    r = 0;

_end:
    return r;
};


static int nsi_service_proxy_no_offer(nsi_service_t* s, nsi_client_t* c)
{
    (void)s;
    (void)c;

    nai_errno = EPERM;
    return -1;
};


static int nsi_service_proxy_no_stop(nsi_service_t* s, nsi_client_t* c)
{
    (void)s;
    (void)c;

    nai_errno = EPERM;
    return -1;
};


static int nsi_service_proxy_send(nsi_service_t* s, nsi_message_t* m)
{
    int r;
    nsi_client_t* c;
    nsi_service_conn_t* e;


    switch (m->hdr.type) {
    case NSI_MT_REQUEST:
    case NSI_MT_REQUEST_NO_RETURN:
        if (s->own) {
            c = s->own->client;
            r = nsi_client_send(c, m, 0, 0);
            break;
        };

        /* do check before request */
        r = nsi_routing_grant_request(s->rt, m);
        if (r < 0) {
            nai_log_debug(NSI_LOG_CORE, nai_errno, 
                "the request(%d) of service(%d, %d) "
                "from the client(0x%x) is denied", 
                m->hdr.method, m->hdr.serv, m->inst, m->hdr.client);
            goto _end;
        };

        /* fallthrough */

    case NSI_MT_RESPONSE:
    case NSI_MT_ERROR:
        e = &s->ep[1];
        r = nsi_proxy_send(&e->c, m, 0, NSI_SEND_POSTED);
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


static int nsi_service_proxy_request(nsi_service_t* s, 
    int op, nsi_client_t* c, nsi_version_t major, nsi_versmin_t minor)
{
    int r;
    int ec;
    nsi_service_conn_t* e;
    nsi_service_info_t si;
    nsi_discovery_t* sd;


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


    /* begin batch */
    r = nsi_routing_batch_begin(s->rt);
    if (r < 0) {
        nai_log_alert(NSI_LOG_CORE, 
            nai_errno, "start batch failed");
        goto _end;
    };


    e = &s->ep[1];
    if (s->local) {
        if (nsi_service_is_avail(s) && op == NSI_SERVICE_OP_ADD) {
            r = nsi_service_available_to_client(
                s, NSI_AVAIL_ONLINED, c, major, minor);
            if (r < 0) {
                goto _fail;
            };
        };

        r = 0;
    } else if (e->stat == 0 || 
        nsi_endpoint_conn_equal(&c->ep, e)) { /* prevent circulation loop */
        r = 0;
    } else {
        switch (op) {
        case NSI_SERVICE_OP_ADD:
            if (!nsi_service_is_avail(s)) {
                /* sd is always available in proxy mode */
                sd = s->rt->sd;
                assert(sd != 0);

                /* check the service is available */
                r = nsi_discovery_get_service_info(sd, s->serv, s->inst, &si);
                if (r >= 0) {
                    /* service is onlined */
                    s->stat = NSI_SERVICE_ONLINED;
                    /* update the versions of service */
                    s->major = si.major;
                    s->minor = si.minor;
                } else {
                    ec = nai_errno;
                    if (ec != ENOENT) {
                        nai_log_error(NSI_LOG_CORE, ec, 
                            "get service(%d, %d) info failed", 
                            s->serv, s->inst);
                        goto _fail;
                    };

                    /* service is offlined */
                    s->stat = NSI_SERVICE_DISCOVERY;
                };
            };
            break;
        case NSI_SERVICE_OP_REMOVE_LAST:
            if (!nsi_service_is_closed(s)) {
                s->stat = NSI_SERVICE_OPENED;
            };
            break;
        default:
            break;
        };
        r = nsi_proxy_request(&e->c, 
            op, 0, c, s->serv, s->inst, major, minor);
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


static int nsi_service_proxy_event(nsi_service_t* s, 
    int op, nsi_client_t* c, nsi_eid_t eid, const nsi_event_info_t* info)
{
    int r;
    nsi_service_conn_t* e;


    e = &s->ep[1];
    if (s->local && (
        op != NSI_SERVICE_OP_OFFER && 
        op != NSI_SERVICE_OP_REMOVE_OFFER)) {
        r = 0;
    } else if (e->stat == 0 || 
        nsi_endpoint_conn_equal(&c->ep, e)) { /* prevent circulation loop */
        r = 0;
    } else {
        r = nsi_proxy_event(&e->c, 
            op, 0, c, s->serv, s->inst, eid, info);
    };

    return r;
};


static int nsi_service_proxy_subscribe(nsi_service_t* s, 
    int op, nsi_client_t* c, 
    nsi_gid_t gid, nsi_version_t major, 
    nsi_eid_t eid, uint32_t ttl)
{
    int r;
    int ec;
    nsi_service_conn_t* e;
    nsi_service_ref_t* o;


    e = &s->ep[1];
    if (s->local) {
        o = s->own;
        if (o) {
            r = nsi_client_handle_subscribe(o->client, 
                op, c, s->serv, s->inst, gid, major, eid, ttl, 0);
        } else {
            r = 0;
        };
    } else if (e->stat == 0 || 
        nsi_endpoint_conn_equal(&c->ep, e)) { /* prevent circulation loop */
        r = 0;
    } else {
        /* do check before subscribe */
        r = nsi_routing_grant_subscribe(s->rt, c, s->serv, s->inst, gid, eid);
        if (r < 0) {
            ec = nai_errno;
            nai_log_debug(NSI_LOG_CORE, ec, 
                "the client(0x%x) subscribe "
                "event group(%d) of service(%d, %d) is denied", 
                c->cid, gid, s->serv, s->inst);
            goto _nack;
        };

        r = nsi_proxy_subscribe(&e->c, 
            op, 0, c, s->serv, s->inst, gid, major, eid, ttl);
    };

_end:
    return r;

_nack:
    nsi_service_subscribe_ack(
        s, c, NSI_E_NOT_OK, gid, major, eid, 0, 0);

    nai_errno = ec;
    goto _end;
};


static int nsi_service_proxy_available(nsi_service_t* s, int avail)
{
    int r;


    if (s->local && s->own) {
        r = 0;
        goto _end;
    };
    if (nsi_service_is_closed(s)) {
        r = 0;
        goto _end;
    };

    nai_log_info(NSI_LOG_CORE, 0, 
        "the service(%d, %d) available changed, %d", 
        s->serv, s->inst, avail);

    /* note: major and minor is modifited in nsi_proxy_handle_message */
    switch (avail) {
    case NSI_AVAIL_ONLINED:
    case NSI_AVAIL_CHANGED:
        s->stat = NSI_SERVICE_ONLINED;
        break;
    case NSI_AVAIL_OFFLINED:
        if (nsi_service_is_started(s)) {
            s->stat = NSI_SERVICE_DISCOVERY;
        };
        break;
    default:
        assert(0);
        break;
    };

    /* the 'available' is notified by host, 
     * the proxy do nothing here.
     */
    r = 0;

_end:
    return r;
};


static int nsi_service_proxy_linkstate(nsi_service_t* s, int on)
{
    int r;

    (void)s;
    (void)on;

    /* do nothing */
    r = 0;

    return r;
};


static int nsi_service_proxy_connected(nsi_service_t* s, int idx)
{
    int r;
    nsi_service_conn_t* e;
    nsi_client_t* c;


    if (idx != 1) {
        r = 0;
        goto _end;
    };

    if (s->ep[1].stat) {
        r = 0;
        goto _end;
    };

    nai_log_info(NSI_LOG_CORE, 0, 
        "the proxy service(%d, %d) is connected", 
        s->serv, s->inst);

    e = &s->ep[1];
    e->stat = 1;
    if (s->own && nsi_service_is_onlined(s)) {
        c = s->own->client;

        /* do verify before offer */
        r = nsi_routing_grant_offer(
            s->rt, c, s->serv, s->inst, s->major, s->minor);
        if (r < 0) {
            nai_log_debug(NSI_LOG_CORE, nai_errno, 
                "the client(0x%x) offer service(%d, %d) is denied", 
                c->cid, s->serv, s->inst);
            goto _end;
        };

        r = nsi_proxy_offer(&e->c, 
            NSI_SERVICE_OP_OFFER, 
            c, s->serv, s->inst, s->major, s->minor);
        if (r < 0) {
            goto _end;
        };
    };

    /* send all requests */
    r = nsi_service_sync(s);
    if (r < 0) {
        nai_log_error(NSI_LOG_CORE, nai_errno, 
            "failed to synchronize service information");
    };

_end:
    return r;
};


static int nsi_service_proxy_disconnected(nsi_service_t* s, int idx)
{
    int r;


    if (idx != 1) {
        r = 0;
        goto _end;
    };

    nai_log_info(NSI_LOG_CORE, 0, 
        "the proxy service(%d, %d) is disconnected", 
        s->serv, s->inst);

    s->ep[1].stat = 0;
    if (s->local && s->own) {
        r = 0;
    } else {
        s->stat = NSI_SERVICE_OPENED;

        /* do nack and notify */
        r = nsi_service_available(s, NSI_AVAIL_OFFLINED);
        if (r < 0) {
            nai_log_error(NSI_LOG_CORE, nai_errno, 
                "failed to notify clients that service is offlined");
        };
    };

_end:
    return r;
};


