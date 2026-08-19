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
/// @file       nsi_discovery_handle.c
/// @brief
/// @details
/// @date       2021-06-08
/// @author     xn
/// @version    1.2.0
///
/// ================================================================

#include "nsi_discovery.h"
#include "nsi_service.h"
#include "nsi_client.h"
#include "nsi_proxy.h"
#include "nsi_routing_impl.h"
#include "nsi_log.h"
#include "nai/os/nai_socket.h"
#include "nai/runtime/nai_util.h"



#define NSI_SD_PROTOCOL         0x01 /* PRS_SOMEIPSD_00161 */
#define NSI_SD_INTERFACE        0x01 /* PRS_SOMEIPSD_00162 */


#define NSI_SD_REBOOT           0x80 /* PRS_SOMEIPSD_00254 */
#define NSI_SD_UNICAST          0x40 /* PRS_SOMEIPSD_00259 */
#define NSI_SD_INITDATA         0x20 /* PRS_SOMEIPSD_00700 */


#define NSI_SD_OPT_CONF         0x01 /* len:1, key=value */
#define NSI_SD_OPT_LOAD         0x02 /* prio:2, weight:2 */
#define NSI_SD_OPT_IN4U         0x04 /* unicast   addr: 4, r:1, t:1, port:2
                                        PRS_SOMEIPSD_00305
                                        */
#define NSI_SD_OPT_IN4M         0x14 /* multicast addr: 4, r:1, t:1, port:2
                                        PRS_SOMEIPSD_00324
                                        */
#define NSI_SD_OPT_IN4D         0x24 /* discovery addr: 4, r:1, t:1, port:2
                                        PRS_SOMEIPSD_00550
                                        */
#define NSI_SD_OPT_IN6U         0x06 /* unicast   addr:16, r:1, t:1, port:2
                                        PRS_SOMEIPSD_00314
                                        */
#define NSI_SD_OPT_IN6M         0x16 /* multicast addr:16, r:1, t:1, port:2
                                        PRS_SOMEIPSD_00331
                                        */
#define NSI_SD_OPT_IN6D         0x26 /* discovery addr:16, r:1, t:1, port:2
                                        PRS_SOMEIPSD_00557
                                        */
#define NSI_SD_OPT_SELECT       0x20 /* clients array of nsi_cid_t */


#define NSI_SD_OPT_UNICAST      0x00
#define NSI_SD_OPT_MCAST        0x10
#define NSI_SD_OPT_DISCOVERY    0x20


#define NSI_SD_PROTO_TCP        0x06
#define NSI_SD_PROTO_UDP        0x11


/* PRS_SOMEIPSD_00267 */
#define NSI_SD_CMD_FIND         0x00 /* find */
#define NSI_SD_CMD_OFFER        0x01 /* offer */
#define NSI_SD_CMD_SUBSCRIBE    0x06 /* subscribe */
#define NSI_SD_CMD_ACK          0x07 /* subscribe ack */


#define NSI_SD_TTL_INFINITE     0xffffff


typedef struct nsi_sd_msghdr_s {
    union {
        struct {
            uint32_t reserved:24;   /* PRS_SOMEIPSD_00261 */
            uint32_t flags:8;       /* PRS_SOMEIPSD_00253 */
        };
        uint32_t u;
    };
} nsi_sd_msghdr_t;


/* PRS_SOMEIPSD_00268
 * PRS_SOMEIPSD_00269
 * PRS_SOMEIPSD_00270
 */
typedef struct nsi_sd_entry_s {

    uint8_t type;
    uint8_t opt[2];
    union {
        struct {
            uint8_t n1:4;
            uint8_t n0:4;
        };
        uint8_t n;
    } optc;

    union {
        struct {
            uint32_t inst:16;
            uint32_t serv:16;
        };
        uint32_t u1;
    };

    union {
        struct {
            uint32_t ttl:24;
            uint32_t major:8;
        };
        uint32_t u2;
    };

    union {
        /* PRS_SOMEIPSD_00270
         * PRS_SOMEIPSD_00389
         */
        struct {
            uint32_t gid:16;
            uint32_t counter:4;
            uint32_t reserved2:3;
            uint32_t init:1;
            uint32_t reserved1:8;
        };
        /* PRS_SOMEIPSD_00269 */
        uint32_t minor;
        uint32_t u3;
    };

} nsi_sd_entry_t;


typedef struct nsi_sd_option_s {
    uint8_t type;
    uint8_t discardable:1;
    uint8_t proto:1;
    uint8_t error:1;
    uint16_t len;
    void* data;
} nsi_sd_option_t;


/* PRS_SOMEIPSD_00273 */
typedef struct nsi_sd_opthdr_s {
    union {
        struct {
            uint32_t discardable:1;
            uint32_t reserved:7;
            uint32_t type:8;
            uint32_t len:16;        /**< exclude len and type */
        };
        uint32_t u;
    };
} nsi_sd_opthdr_t;


/* PRS_SOMEIPSD_00542
 * PRS_SOMEIPSD_00544
 */
typedef struct nsi_sd_load_s {
    union {
        struct {
            uint32_t weight:16;
            uint32_t prio:16;
        };
        uint32_t u;
    };
} nsi_sd_load_t;


/* PRS_SOMEIPSD_00307
 * PRS_SOMEIPSD_00326
 * PRS_SOMEIPSD_00333
 * PRS_SOMEIPSD_00552
 * PRS_SOMEIPSD_00559
 */
typedef struct nsi_sd_address_s {
    /* in_addr or in6_addr */
    /* already is network byte order */
    uint8_t reserved;
    uint8_t proto;
    uint16_t port;
} nsi_sd_address_t;



static int nsi_discovery_handle_offline(
    nsi_discovery_t* p, nsi_sd_service_t* s)
{
    int r;


    s->ttl = 0;
    if (s->own != 0) {
        s->own->offers --;
        s->own = 0;
    };

    nsi_discovery_remove_timer(p, &s->time);

    r = nsi_discovery_notify_available(p, s, NSI_AVAIL_OFFLINED);

    return r;
};


static int nsi_discovery_handle_expired(
    nsi_discovery_t* p, nai_list_entry_t* list)
{
    int r;
    nai_list_entry_t* e;
    nsi_sd_service_t* s;
    nsi_sd_subscribe_t* u;
    nsi_sd_timestat_t* t;


    r = nsi_routing_batch_begin(p->rt);
    if (r < 0) {
        nai_log_alert(NSI_LOG_CORE, 
            nai_errno, "start batch failed");

        e = list->next;
        for ( ; e != list; ) {
            t = nai_containof(e, nsi_sd_timestat_t, ent);
            e = e->next;
            nai_list_entry_remove(&t->ent);
            nai_list_init(&t->ent);
        };

    } else {

        /* process subscribes first, 
         * because subscribe may be modified in the callback of available. */
        e = list->next;
        for ( ; e != list; ) {
            t = nai_containof(e, nsi_sd_timestat_t, ent);
            e = e->next;
            if (t->type != NSI_SD_TYPE_SUBSCRIBE) {
                continue;
            };

            u = nai_containof(t, nsi_sd_subscribe_t, time);
            nai_log_info(NSI_LOG_CORE, 0, 
                "the service(%d, %d)'s subscribe group(%d) is expired", 
                u->serv, u->inst, u->gid);

            u->ack = 0;
            nai_list_entry_remove(&t->ent);
            nai_list_init(&t->ent);
            nsi_discovery_notify_ack(p, u, 0);
        };

        /* process service available */
        e = list->next;
        for ( ; e != list; ) {
            t = nai_containof(e, nsi_sd_timestat_t, ent);
            e = e->next;

            nai_list_entry_remove(&t->ent);
            nai_list_init(&t->ent);
            switch (t->type) {
            case NSI_SD_TYPE_SERVICE:
                s = nai_containof(t, nsi_sd_service_t, time);
                if (s->ttl) {
                    nai_log_info(NSI_LOG_CORE, 0, 
                        "the service(%d, %d)'s is expired", 
                        s->serv, s->inst);

                    nsi_discovery_handle_offline(p, s);
                };
                break;
            default:
                continue;
            };
        };

        nsi_routing_batch_end(p->rt);
        r = 0;
    };

    return r;
};


static int nsi_discovery_handle_next(
    nsi_discovery_t* p, nai_list_entry_t* list)
{
    int r;
    uint64_t msec;
    nai_list_entry_t* e;
    nsi_sd_timestat_t* t;
    nsi_sd_service_t* s;
    nsi_sd_subscribe_t* u;


    e = list->next;
    for ( ; e != list; ) {
        t = nai_containof(e, nsi_sd_timestat_t, ent);
        e = e->next;

        nai_list_entry_remove(&t->ent);
        switch (t->type) {
        case NSI_SD_TYPE_OFFER:
        case NSI_SD_TYPE_FIND:
            /* PRS_SOMEIPSD_00395
             * PRS_SOMEIPSD_00416
             * PRS_SOMEIPSD_00434
             * PRS_SOMEIPSD_00435
             */
            switch (t->stat) {
            case NSI_SD_STAT_INIT:
                /* PRS_SOMEIPSD_00404 */
                t->repeat = 1; /* had been sent */
                t->stat = NSI_SD_STAT_REPEAT;

                /* fallthrough */

            case NSI_SD_STAT_REPEAT:
                /* PRS_SOMEIPSD_00405
                 * PRS_SOMEIPSD_00406
                 * PRS_SOMEIPSD_00407
                 * PRS_SOMEIPSD_00408
                 */
                if (t->repeat >= p->repetitions_max) {
                    t->stat = NSI_SD_STAT_MAIN;
                } else {
                    msec = p->repetitions_delay << t->repeat;
                    t->repeat ++;
                    break;
                };

                /* fallthrough */

            case NSI_SD_STAT_MAIN:
                /* PRS_SOMEIPSD_00410 */
                if (t->type == NSI_SD_TYPE_FIND) {
                    /* make the stat of entry to removed
                     * PRS_SOMEIPSD_00415
                     */
                    nai_list_init(&t->ent);
                    continue;
                };

                /* PRS_SOMEIPSD_00411
                 * PRS_SOMEIPSD_00412
                 * PRS_SOMEIPSD_00413
                 */
                s = nai_containof(t, nsi_sd_service_t, time);
                if (t->type == NSI_SD_TYPE_OFFER && t->flags) {
                    /* cleanup for  PRS_SOMEIPSD_00417 */
                    nsi_discovery_free(p, s);
                    continue;
                }
                msec = s->cyclic_delay;
                break;

            default:
                /* unknown stat */
                assert(0);
                nai_list_init(&t->ent);
                continue;
            };
            break;

        case NSI_SD_TYPE_SUBSCRIBE:
            /* PRS_SOMEIPSD_00808 */
            u = nai_containof(t, nsi_sd_subscribe_t, time);
            switch (t->stat) {
            case NSI_SD_STAT_WAITING:
                t->stat = NSI_SD_STAT_PENDING;
                t->repeat = 1; /* subscribe had been sent */
                msec = u->subscribe_timeo;
                if (msec > (uint64_t)u->ttl * 1000) {
                    msec = (uint64_t)u->ttl * 1000;
                };
                break;

            case NSI_SD_STAT_ACKED:
                assert(u->subscribe_renew);
                t->stat = NSI_SD_STAT_ACKED_RENEW;
                t->repeat = 1; /* resubscribe had been sent */
                msec = u->subscribe_timeo;
                if (msec > (uint64_t)u->ack * 1000) {
                    msec = (uint64_t)u->ack * 1000;
                };
                break;

            case NSI_SD_STAT_PENDING:
                assert(u->subscribe_renew);
                t->repeat ++; /* resubscribe had been sent */
                msec = u->subscribe_timeo;
                if (msec > (uint64_t)u->ttl * 1000) {
                    msec = (uint64_t)u->ttl * 1000;
                };
                break;

            case NSI_SD_STAT_ACKED_RENEW:
                /* PRS_SOMEIPSD_00449 */
                assert(u->subscribe_renew);
                t->repeat ++; /* resubscribe had been sent */
                msec = u->subscribe_timeo;
                if (msec > (uint64_t)u->ack * 1000) {
                    msec = (uint64_t)u->ack * 1000;
                };

                /* PRS_SOMEIPSD_00704 */
                u = nai_containof(t, nsi_sd_subscribe_t, time);
                u->init = 0;
                break;

            case NSI_SD_STAT_REMOVE:
                /* unsubscribe had been sent */
                u = nai_containof(t, nsi_sd_subscribe_t, time);
                nai_list_entry_remove(&u->ent);
                nsi_discovery_free_subscribe(p, u);
                continue;

            default:
                /* unknown stat */
                assert(0);
                nai_list_init(&t->ent);
                continue;
            };

            break;

        case NSI_SD_TYPE_ACK:
            u = nai_containof(t, nsi_sd_subscribe_t, time);
            nai_list_entry_remove(&u->ent);
            nsi_discovery_free_subscribe(p, u);
            continue;

        default:
            nai_list_init(&t->ent);
            continue;
        };

        nsi_discovery_set_timer(p, t, msec);
    };

    r = 0;

    return r;
};


#if 0

static int nsi_discovery_cleanup_clients(nsi_discovery_t* p)
{
    int r;
    nai_rbnode_t* n;
    nsi_client_t* c;
    nsi_routing_endpoint_t* e;


    e = nsi_routing_get_endpoint_ctx(p->ep);
    if (e == 0) {
        r = 0;
        goto _end;
    };

    n = nai_rbtree_begin(&e->clients);
    for ( ; n != nai_rbtree_end(&e->clients); ) {
        c = nai_containof(n, nsi_client_t, ep.ente);
        n = nai_rbtree_next(n);

        if (c->remote == 0) {
            continue;
        };
        if (c->offers > 0) {
            continue;
        };

        if (nai_list_is_empty(&c->subs) && 
            nai_list_is_empty(&c->servs) && nai_list_is_empty(&c->events) && 
            nai_list_is_empty(&c->reqs)) {
            nai_log_debug(NSI_LOG_CORE, 0, 
                "discovery remove inactive client(0x%x)", c->cid);
            nsi_client_close(c);
        };
    };

    r = 0;

_end:
    return r;
};

#endif

/* this only a entry point for endpoint reset,
   should allocate a timer to check network connectivity,
   and then to perform the reconfig steps */
static void nsi_discovery_reconfig_ep(nsi_discovery_t* p)
{
    uint64_t now = 0;
    static uint64_t timestamp = 0;

    if (p->type != NSI_RTYPE_ROUTE) {
        return;
    }
    if (!p->ep || !p->mp) {
        return;
    }

    now = nai_tickcache_to_msec();
    if (now - timestamp <= 1500) {
        return;
    }

    timestamp = now;

    /* should try to reconfig the socket only when servcie_discovery is down */
    nsi_network_reconfig_eps(p->ep->net,
            NSI_EFLAG_SERVER,
            &p->ep->name);
}

int nsi_discovery_handle_timeout(nsi_discovery_t* p)
{
    int r;
    int n;
    uint64_t now;
    nai_list_entry_t send;
    nai_list_entry_t list;
    nai_list_entry_t* e;
    nai_rbnode_t* node;
    nsi_sd_timestat_t* t;
    nsi_sd_subscribe_t* u;


    if (p->link == 0) {
        r = 0;
        goto _end;
    };

    /* unset time */
    p->timing = 1;
    p->timenext = -1;


    /* initialize list and current time */
    nai_list_init(&send);
    nai_list_init(&list);
    now = nai_tickcache_to_msec();


    /* get from initial list */
    for (n = 0; n < (int)nai_countof(p->timedebo); n ++) {
        if (p->timedebo[n] > now + 5) {
            continue;
        };

        /* PRS_SOMEIPSD_00399 */
        p->timedebo[n] = now + p->debounce[n];

        e = p->initq.next;
        for ( ; e != &p->initq; ) {
            t = nai_containof(e, nsi_sd_timestat_t, ent);
            e = e->next;
            if (t->type != n) {
                continue;
            };

            t->queue = 0;
            nai_list_entry_remove(&t->ent);
            nai_list_insert_tail(&send, &t->ent);
        };
    };

    /* get from time queue */
    node = nai_rbtree_begin(&p->timeq);
    for ( ; node != nai_rbtree_end(&p->timeq); ) {
        t = nai_containof(node, nsi_sd_timestat_t, node);
        if (t->expire > now + 5) {
            p->timenext = t->expire;
            break;
        };

        node = nai_rbtree_next(node);

        t->queue = 0;
        nai_rbtree_erase(&p->timeq, &t->node);
        switch (t->type) {
        case NSI_SD_TYPE_SERVICE:
            /* service expired, remove it */
            nai_list_insert_tail(&list, &t->ent);
            break;

        case NSI_SD_TYPE_SUBSCRIBE:
            switch (t->stat) {
            case NSI_SD_STAT_PENDING:
            case NSI_SD_STAT_ACKED:
            case NSI_SD_STAT_ACKED_RENEW:
                /* maximum number of retries reached ? 
                 * PRS_SOMEIPSD_00808
                 * PRS_SOMEIPSD_00809
                 */
                u = nai_containof(t, nsi_sd_subscribe_t, time);
                if (t->repeat >= u->subscribe_renew && 
                    u->service->ttl != (uint32_t)-1) {
                    nai_list_insert_tail(&list, &t->ent);
                    break;
                };

                /* fallthrough */

            default:
                /* waiting or remove: always send */
                nai_list_insert_tail(&send, &t->ent);
                break;
            };
            break;

        default:
            /* offer or find: always send */
            nai_list_insert_tail(&send, &t->ent);
            break;
        };
    };


    /* update next time */
    for (n = 0; n < (int)nai_countof(p->timedebo); n ++) {
        if (p->timenext > p->timedebo[n]) {
            p->timenext = p->timedebo[n];
        };
    };

    /* we cannot remove clients, 
     * otherwise will cause the remote to mistake us for restarting
     */
#if 0
    /* cleanup old clients */
    if (p->timeclean < now) {
        p->timeclean = now + 30 * 1000;
        nsi_discovery_cleanup_clients(p);
    };
#endif


    /* send entries */
    if (!nai_list_is_empty(&send)) {
        if (p->ep) {
            nsi_discovery_send_dispatch(p, &send);
        };
        nsi_discovery_handle_next(p, &send);
    };

    /* process entries */
    if (!nai_list_is_empty(&list)) {
        nsi_discovery_handle_expired(p, &list);
    };

    /* reconfig udp server endpoint periodically */
    nsi_discovery_reconfig_ep(p);

    /* set new time */
    nai_evnode_set_timeout(
        &p->rt->ev, NAI_TIMEOP_SET, (uint32_t)(p->timenext - now));
    p->timing = 0;
    r = 0;

_end:
    return r;
};


int nsi_discovery_handle_connected(nsi_discovery_t* p)
{
    int r;
    nsi_endpoint_conn_t conn;


    if (p->type != NSI_RTYPE_PROXY) {
        r = 0;
        goto _end;
    };

    nai_log_debug(NSI_LOG_CORE, 0, 
        "discovery send connect on proxy connected");

    r = nsi_discovery_resume(p);
    if (r < 0) {
        nai_log_error(NSI_LOG_CORE, 
            nai_errno, "discovery resume failed");
        goto _end;
    };

    conn.ep = p->ep;
    conn.conn = 0;
    r = nsi_proxy_connect(&conn);
    if (r < 0) {
        nai_log_error(NSI_LOG_CORE, 
            nai_errno, "send proxy connect failed");

        /* fixme: error ignored */
    };

_end:
    return r;
};


int nsi_discovery_handle_disconnected(nsi_discovery_t* p)
{
    int r;


    if (p->type != NSI_RTYPE_PROXY) {
        r = 0;
        goto _end;
    };

    nai_log_debug(NSI_LOG_CORE, 0, 
        "discovery remove service info on proxy disconnected");

    r = nsi_discovery_hangup(p);
    if (r < 0) {
        nai_log_error(NSI_LOG_CORE, 
            nai_errno, "discovery hangup failed");
        goto _end;
    };

    r = 0;

_end:
    return r;
};


static int nsi_discovery_handle_offer(
    nsi_discovery_t* p, nsi_sd_entry_t* e, 
    nsi_client_t* c, 
    const nsi_endpoint_name_t* names, nai_list_entry_t* out)
{
    int r;
    int n;
    int local;
    nsi_service_info_t si;
    char buf[140];


    (void)out;

    nai_log_info(NSI_LOG_CORE, 0, 
        "discovery handle offer service(%d, %d) request with version(%d, %d)", 
        e->serv, e->inst, e->major, e->minor);

    local = nsi_routing_is_local_service(p->rt, e->serv, e->inst);
    if (local) {
        nai_log_debug(NSI_LOG_CORE, 0, 
            "discovery received service(%d, %d) conflicting with local.", 
            e->serv, e->inst);
        r = 0;
        goto _end;
    };

    /* PRS_SOMEIPSD_00310
     * PRS_SOMEIPSD_00319
     * PRS_SOMEIPSD_00357
     * PRS_SOMEIPSD_00583
     */
    if (names[0].addr == 0 && names[1].addr == 0) {
        nai_log_debug(NSI_LOG_CORE, 0, 
            "discovery received service(%d, %d) without valid addresses.", 
            e->serv, e->inst);
        r = 0;
        goto _end;
    };

    /* check addresses
     * PRS_SOMEIPSD_00656
     */
    for (n = 0; n < 2; n ++) {
        if (names[n].addr == 0) {
            continue;
        };
        if (!nsi_routing_is_subnet(p->rt, &names[n])) {
            if (nai_log_is_enabled_debug(&nsi_log_core)) {
                buf[0] = 0;
                nai_sockaddr_ntop(names[n].addr, 
                    names[n].len, buf, sizeof(buf), 0);
                nai_log_debug(NSI_LOG_CORE, 0, 
                    "discovery reject service(%d, %d) with banned "
                    "address(%s).", e->serv, e->inst, buf);
            };

            r = 0;
            goto _end;
        };
    };


    /* add info */
    si.local = 0;
    si.ttl = e->ttl == NSI_SD_TTL_INFINITE ? -1 : e->ttl;
    si.cyclic_delay = 0;
    si.owner = 0;
    si.serv = e->serv;
    si.inst = e->inst;
    si.major = e->major;
    si.minor = e->minor;
    si.names[0] = names[0];
    si.names[1] = names[1];

    r = nsi_discovery_set_service_info(p, &si, c);

_end:
    return r;
};

static void nsi_discovery_delay_offer(
    nsi_discovery_t* p, nsi_sd_service_t* d, nsi_endpoint_t *ep, nsi_client_t* c)
{
    uint64_t now = nai_tickcache_to_msec();
    uint32_t delta = p->request_response_delay_max - p->request_response_delay_min;

    /* offer for the find request is transient */
    d->time.flags = 1;
    d->time.queue = 0;

    /* non-NULL nsi_client means the offer is target to unicast */
    if (!p->offer_multicast_optim) {
        /* PRS_SOMEIPSD_00422 */
        d->own = c;
    } else {
        /* PRS_SOMEIPSD_00423 */
        d->own = (now - d->last_offering > (p->offer_cyclic_delay/2))?  0 : c;
    }

    /* orig from unicast, and dest for unicast */
    if (ep == p->ep && d->own) {
        /* PRS_SOMEIPSD_00419 */
        nsi_discovery_set_timer(p, &d->time, 0);
    } else {
        /* PRS_SOMEIPSD_00417
        * PRS_SOMEIPSD_00419
        * PRS_SOMEIPSD_00420
        */
        delta = nai_rand32() % delta;
        nsi_discovery_set_timer(p, &d->time, p->request_response_delay_min + delta);
    }
}


static int nsi_discovery_handle_find(
    nsi_discovery_t* p, nsi_sd_entry_t* e, 
    nsi_endpoint_t *ep, nsi_client_t* c, nai_list_entry_t* out)
{
    int r;
    nai_rbnode_t* n;
    nsi_sd_service_t* s;
    nsi_sd_service_t* d;


    (void)out;
    nai_log_info(NSI_LOG_CORE, 0, 
        "discovery handle find service(%d, %d) request with version(%d, %d)", 
        e->serv, e->inst, e->major, e->minor);


    /* do check before find */
    r = nsi_routing_grant_find(
        p->rt, c, e->serv, e->inst, e->major, e->minor);
    if (r < 0) {
        nai_log_debug(NSI_LOG_CORE, nai_errno, 
            "the client(0x%x) request service(%d, %d) is denied", 
            c->cid, e->serv, e->inst);
        r = 0;
        goto _end;
    };


    if (e->inst != NSI_INSTANCE_ANY) {
        /* PRS_SOMEIPSD_00825
         * PRS_SOMEIPSD_00126
         * PRS_SOMEIPSD_00127
         */
        s = nsi_discovery_get_service(p, &p->offer, e->serv, e->inst);
        if (s == 0) {
            r = 0;
            goto _end;
        };

        /* does not need to answer in the repetition phase */
        if (s->time.stat < NSI_SD_STAT_MAIN) {
            r = 0;
            goto _end;
        };

        /* PRS_SOMEIPSD_00825
         * PRS_SOMEIPSD_00128
         */
        if (e->major != NSI_MAJOR_ANY && e->major != s->major) {
            r = 0;
            goto _end;
        };
        if (e->minor != NSI_MINOR_ANY && e->minor != s->minor) {
            r = 0;
            goto _end;
        };

        d = (nsi_sd_service_t*)nsi_discovery_alloc(p, sizeof(*d));
        if (d == 0) {
            nai_log_error(NSI_LOG_CORE, nai_errno, 
                "discovery failed to "
                "allocate memory to reply find service(%d, %d)", 
                e->serv, e->inst);
            r = -1;
            goto _end;
        };

        d[0] = s[0];
        d->time.flags = 0;
        nsi_discovery_delay_offer(p, d, ep, c);

    } else {
        n = nai_rbtree_begin(&p->offer);
        for ( ; n != nai_rbtree_end(&p->offer); ) {
            s = nai_containof(n, nsi_sd_service_t, ent);
            n = nai_rbtree_next(n);

            /* does not need to answer in the repetition phase */
            if (s->time.stat < NSI_SD_STAT_MAIN) {
                continue;
            };
            if (e->serv != s->serv) {
                continue;
            };
            if (e->major != NSI_MAJOR_ANY && e->major != s->major) {
                continue;
            };
            if (e->minor != NSI_MINOR_ANY && e->minor != s->minor) {
                continue;
            };

            d = (nsi_sd_service_t*)nsi_discovery_alloc(p, sizeof(*d));
            if (d == 0) {
                nai_log_error(NSI_LOG_CORE, nai_errno, 
                    "discovery failed to "
                    "allocate memory to reply find service(%d, %d)", 
                    e->serv, e->inst);
                r = -1;
                goto _end;
            };

            d[0] = s[0];
            d->time.flags = 0;
            nsi_discovery_delay_offer(p, d, ep, c);
        };
    };

    r = 0;

_end:
    return r;
};


static int nsi_discovery_handle_subscribe(
    nsi_discovery_t* p, nsi_sd_entry_t* e, 
    nsi_client_t* c, 
    const nsi_endpoint_name_t* names, nai_list_entry_t* out, int nack)
{
    int r;
    int n;
    nsi_sd_service_t* s;
    nsi_sd_subscribe_t* u;
    const nai_sockaddr_info_t* si;
    char buf[140];


    nai_log_debug(NSI_LOG_CORE, 0, 
        "discovery handle subscribe(%d, %d, %d) request with version(%d)", 
        e->serv, e->inst, e->gid, e->major);

    if (nack == 1) {
        goto _nack;
    };

    /* PRS_SOMEIPSD_00126
     * PRS_SOMEIPSD_00127
     */
    s = nsi_discovery_get_service(p, &p->offer, e->serv, e->inst);
    if (s == 0) {
        goto _nack;
    };

    if (s->time.stat == NSI_SD_STAT_INIT) {
        r = 0; /* initliaze phare, ignore */
        goto _end;
    };

    /* PRS_SOMEIPSD_00128 */
    if (s->major != e->major) {
        goto _nack;
    };

    /* PRS_SOMEIPSD_00389 */
    if (e->ttl == 0) {
        r = nsi_routing_unsubscribe(p->rt, 
            c, e->serv, e->inst, e->gid, e->major, NSI_EVENT_ANY);
    } else {
        /* checking addresses */
        if (names[0].addr && names[1].addr) {
            /* PRS_SOMEIPSD_00387 */
            if (names[0].addr->sa_family != names[1].addr->sa_family) {
                goto _nack;
            };
        } else if (names[0].addr || names[1].addr) {
            ;
        } else {
            /* PRS_SOMEIPSD_00583 */
            goto _nack;
        };

        /* checking addresses
         * PRS_SOMEIPSD_00656
         */
        for (n = 0; n < 2; n ++) {
            if (names[n].addr == 0) {
                continue;
            };

            si = nai_sockaddr_info(names[n].addr->sa_family);
            if (nai_sockaddr_get_port(si,names[n].addr) == 0) {
                r = 0;
                goto _nack;
            };

            if (!nsi_routing_is_subnet(p->rt, &names[n])) {
                if (nai_log_is_enabled_debug(&nsi_log_core)) {
                    buf[0] = 0;
                    nai_sockaddr_ntop(names[n].addr, 
                        names[n].len, buf, sizeof(buf), 0);
                    nai_log_debug(NSI_LOG_CORE, 0, 
                        "discovery reject subscribe(%d, %d, %d) with banned "
                        "address(%s).", e->serv, e->inst, e->major, buf);
                };

                r = 0;
                goto _nack;
            };
        };

        /* PRS_SOMEIPSD_00360 */
        r = nsi_routing_subscribe(p->rt, 
            c, e->serv, e->inst, e->gid, e->major, NSI_EVENT_ANY, 
            e->ttl == NSI_SD_TTL_INFINITE ? -1 : e->ttl, e->init, names);
        if (r < 0) {
            goto _nack;
        };
    };

    r = 0;

_end:
    return r;

_nack:
    if (e->ttl == 0) {
        /* do nothing */
        r = 0;
        goto _end;
    };

    /* PRS_SOMEIPSD_00462 */

    nai_log_debug(NSI_LOG_CORE, 0, 
        "discovery nack subscribe(%d, %d, %d) request with version(%d)", 
        e->serv, e->inst, e->gid, e->major);

    u = (nsi_sd_subscribe_t*)nai_palloc(&p->pool, sizeof(*u));
    if (u == 0) {
        nai_log_alert(NSI_LOG_CORE, nai_errno, 
            "the discovery failed to "
            "allocate memory and cannot reply subscribe(%d, %d, %d) nack", 
            e->serv, e->inst, e->gid);
        r = -1;
        goto _end;
    };

    u->serv = e->serv;
    u->inst = e->inst;
    u->gid = e->gid;
    u->major = e->major;
    u->ttl = u->ack = 0;
    nai_sockname_init(&u->mcast);
    nai_sockname_init(&u->names[0]);
    nai_sockname_init(&u->names[1]);
    nai_array_init(&u->clients, sizeof(nsi_cid_t), 0);

    u->time.flags = 0;
    u->time.type = NSI_SD_TYPE_ACK;
    u->time.stat = NSI_SD_STAT_WAITING;
    u->time.queue = 0;
    nai_list_init(&u->time.ent);
    nai_list_insert_tail(out, &u->time.ent);
    r = 0;
    goto _end;
};


static int nsi_discovery_handle_ack(
    nsi_discovery_t* p, nsi_sd_entry_t* e, 
    nsi_client_t* c, 
    const nsi_endpoint_name_t* mcast, nai_list_entry_t* out)
{
    int r;
    int changed;
    uint64_t msec;
    nsi_sd_service_t* d;
    nsi_sd_subscribe_t* u;
    nsi_endpoint_name_t name;
    char buf[140];


    (void)out;

    nai_log_info(NSI_LOG_CORE, 0, 
        "discovery handle subscription(%d, %d, %d) ack with ttl(0x%x)", 
        e->serv, e->inst, e->gid, e->ttl);


    /* check multicat address */
    if (mcast && mcast->addr) {
        r = nsi_routing_is_mcast(p->rt, mcast);
        if (!r) {
            if (nai_log_is_enabled_debug(&nsi_log_core)) {
                buf[0] = 0;
                nai_sockaddr_ntop(mcast->addr, 
                    mcast->len, buf, sizeof(buf), 0);
                nai_log_debug(NSI_LOG_CORE, 0, 
                    "discovery cannot handle subscription(%d, %d, %d) ack "
                    "with invalid multicast address(%s)", 
                    e->serv, e->inst, e->gid, buf);
            };
            goto _end;
        };
    };

    /* find service
     * PRS_SOMEIPSD_00126
     * PRS_SOMEIPSD_00127
     */
    d = nsi_discovery_get_service(p, &p->servs, e->serv, e->inst);
    if (d == 0) {
        r = 0;
        goto _end;
    };

    /* find subscribe
     * PRS_SOMEIPSD_00129
     */
    u = nsi_discovery_get_subscribe(p, d, e->gid);
    if (u == 0) {
        r = 0;
        goto _end;
    };

    if (u->time.stat == NSI_SD_STAT_REMOVE) {
        r = 0;
        goto _end;
    };

    /* PRS_SOMEIPSD_00128 */
    if (u->major != e->major) {
        r = 0;
        goto _end;
    };

    /* update mcast address */
    if (mcast->addr == 0) {
        changed = u->mcast.addr != 0;
    } else if (u->mcast.addr == 0) {
        changed = 1;
    } else {
        changed = !nsi_endpoint_name_equal(mcast, &u->mcast);
    };
    if (changed) {
        if (mcast->addr == 0) {
            name.addr = 0;
            name.len = 0;
        } else {
            name.len = mcast->len;
            name.addr = (nai_sockaddr_t*)nsi_discovery_alloc(p, name.len);
            if (name.addr == 0) {
                nai_log_alert(NSI_LOG_CORE, nai_errno, 
                    "discovery failed to allocate memory and unable to "
                    "process subscription(%d, %d, %d) ack with ttl(0x%x)", 
                    e->serv, e->inst, e->gid, e->ttl);

                r = -1;
                goto _end;
            };

            nai_memcpy(name.addr, mcast->addr, name.len);
        };

        if (u->mcast.addr) {
            nsi_discovery_free(p, u->mcast.addr);
        };

        u->mcast = name;
    };

    /* update stat and ttl */
    u->time.stat = NSI_SD_STAT_ACKED;
    u->time.repeat = 0;
    if (e->ttl == 0) {
        u->ack = 0;
        nsi_discovery_remove_timer(p, &u->time);
    } else {
        /* mark received for
         * if not expired, then the client shall not request Initial Events.
         * PRS_SOMEIPSD_00704
         * PRS_SOMEIPSD_00703
         */
        if (c->initial_expl) {
            u->init |= e->init; 
        } else {
            u->init = 1; /* no explicit control, always set 1 */
        };
        if (e->ttl == NSI_SD_TTL_INFINITE) {
            msec = u->ack = -1;
        } else if (u->subscribe_renew == 0) {
            msec = u->ack = e->ttl;
            msec *= 1000;
        } else {
            msec = u->ack = e->ttl;
            msec *= 1000;
            if (msec <= u->subscribe_timeo) {
                msec = 0;
            } else {
                msec -= u->subscribe_timeo;
            };;
        };
        nsi_discovery_set_timer(p, &u->time, msec);
    };

    /* notify */
    r = nsi_discovery_notify_ack(p, u, 0);

_end:
    return r;
};


static int nsi_discovery_handle_reboot(
    nsi_discovery_t* p, nsi_client_t* c, int flags, 
    nsi_session_t sess, int unicast)
{
    int r;
    int n;
    int reboot;
    int reboot_last;
    nai_rbnode_t* e;
    nsi_sd_service_t* s;


    n = 0;
    if (unicast) {
        n = 1;
    };

    reboot = 0;
    if (flags & NSI_SD_REBOOT) {
        reboot = 1;
    };

    /* PRS_SOMEIPSD_00160
     * PRS_SOMEIPSD_00631
     * PRS_SOMEIPSD_00258
     * PRS_SOMEIPSD_00503
     */
    reboot_last = (c->reboot_recv & (1 << n));
    if (reboot && !reboot_last) {
        r = 1;
    } else if (reboot && reboot_last && c->ep.srecv[n] >= sess) {
        r = 1;
    } else {
        r = 0;
    };

    if (r == 0) {
        goto _ok;
    };

    nai_log_info(NSI_LOG_CORE, 0, 
        "the discovery detected that the remote client(0x%x) has restarted, "
        "reboot %d, reboot_last %d, session %d, session_last %d", 
        c->cid, reboot, reboot_last, sess, c->ep.srecv[n]);

    /* remove all client's subscriptions */
    r = nsi_client_reset(c);
    if (r < 0) {
        nai_log_error(NSI_LOG_CORE, nai_errno, 
            "the discovery failed to reset the client(0x%x) who is reboot", 
            c->cid);
        goto _end;
    };

    /* the client is rebooted, reset stat */
    c->ep.srecv[0] = c->ep.srecv[1] = 0;
    c->reboot_recv = 0x3;

    /* remove services which provided by the client */
    e = nai_rbtree_begin(&p->servs);
    for ( ; e != nai_rbtree_end(&p->servs); ) {
        s = nai_containof(e, nsi_sd_service_t, ent);
        e = nai_rbtree_next(e);

        if (s->own != c) {
            continue;
        };
        if (s->ttl == 0) {
            continue;
        };

        nsi_discovery_handle_offline(p, s);
    };

    r = 1;

_ok:
    c->ep.sinternal ++;
    if (c->ep.sinternal == 0) {
        c->ep.sinternal = 1;
    };
    c->ep.srecv[n] = sess;
    c->reboot_recv &= ~(1 << n);
    c->reboot_recv |= (reboot << n);

_end:
    return r;
};


#if defined(NAI_HAVE_SOCKADDR_IN6)

static int nsi_discovery_fix_zone(
    nsi_discovery_t* p, nai_sockaddr_in6_t* name)
{
    int r;
    uint32_t scope_id;
    nai_sockaddr_in6_t* in6;
    nsi_conf_t* conf;


    /* get scope_id from 'conf.host' 
     * and set scope_id to the address for ipv6
     */
    scope_id = 0;
    conf = p->rt->conf;
    if (conf != 0 && 
        conf->host.addr->sa_family == AF_INET6) {
        in6 = (nai_sockaddr_in6_t*)conf->host.addr;
        scope_id = in6->sin6_scope_id;
        name->sin6_scope_id = scope_id;
    };

    r = 0;

    return r;
};

#endif


static int nsi_discovery_read_option(
    nsi_discovery_t* p, nsi_message_io_t* io, nsi_sd_option_t* opt)
{
    int r;
    nai_pool_t* pool;
    nsi_sd_opthdr_t ost;
    nsi_sd_address_t trl;
    nsi_sd_load_t* ld;
    nai_sockaddr_in4_t* sa4;
    nai_sockaddr_in6_t* sa6;


    opt->error = 0;
    r = nsi_message_get_u4_be(io, (uint32_t*)&ost.u);
    if (r < 0) {
        nai_log_warn(NSI_LOG_CORE, nai_errno, 
            "the discovery failed to read message option");
        goto _end;
    };


    /* PRS_SOMEIPSD_00274
     * PRS_SOMEIPSD_00497
     * PRS_SOMEIPSD_00498
     */
    pool = &p->pool;
    opt->type = ost.type;
    opt->discardable = ost.discardable;
    switch (ost.type) {
    case NSI_SD_OPT_LOAD:
        if (ost.len != 5) {
            nai_log_debug(NSI_LOG_CORE, EINVAL, 
                "the discovery handle "
                "option 'load' with invalid length(%d)", ost.len);

            opt->error = 1;
            break;
        };

        ld = (nsi_sd_load_t*)nai_palloc(pool, sizeof(*ld));
        if (ld == 0) {
            nai_log_alert(NSI_LOG_CORE, nai_errno, 
                "the discovery failed to allocate option 'load'");
            r = -1;
            goto _end;
        };

        r = nsi_message_get_u4_be(io, &ld->u);
        if (r < 0) {
            nai_log_debug(NSI_LOG_CORE, nai_errno, 
                "the discovery failed to read message option 'load'");
            goto _end;
        };

        opt->proto = 0;
        opt->len = sizeof(*ld);
        opt->data = ld;
        break;

    case NSI_SD_OPT_IN4U:
    case NSI_SD_OPT_IN4M:
    case NSI_SD_OPT_IN4D:
        if (ost.len != 9) {
            nai_log_debug(NSI_LOG_CORE, EINVAL, 
                "the discovery handle "
                "option 'ipv4' with invalid length(%d)", ost.len);
            opt->error = 1;
            break;
        };

        sa4 = (nai_sockaddr_in4_t*)nai_palloc(pool, sizeof(*sa4));
        if (sa4 == 0) {
            nai_log_alert(NSI_LOG_CORE, nai_errno, 
                "the discovery failed to allocate option 'ipv4'");
            r = -1;
            goto _end;
        };

        /* PRS_SOMEIPSD_00306
         * PRS_SOMEIPSD_00307
         * PRS_SOMEIPSD_00325
         * PRS_SOMEIPSD_00326
         * PRS_SOMEIPSD_00552
         * PRS_SOMEIPSD_00559
         */
        r = (int)nsi_message_read_least(
            io, &sa4->sin_addr, sizeof(sa4->sin_addr));
        if (r < 0) {
            nai_log_debug(NSI_LOG_CORE, nai_errno, 
                "the discovery failed to read message option 'ipv4'");
            goto _end;
        };

        r = (int)nsi_message_read_least(io, &trl, sizeof(trl));
        if (r < 0) {
            nai_log_debug(NSI_LOG_CORE, nai_errno, 
                "the discovery failed to read message option 'ipv4'");
            goto _end;
        };


#if defined(NAI_HAVE_SOCKADDR_LEN)
        sa4->sin_len = sizeof(*sa4);
#endif
        sa4->sin_family = AF_INET;
        sa4->sin_port = trl.port;
        nai_memset(sa4->sin_zero, 0, sizeof(sa4->sin_zero));


        /* set address option */
        opt->len = sizeof(*sa4);
        opt->data = sa4;
        switch (trl.proto) {
        case NSI_SD_PROTO_TCP:
            opt->proto = 1;
            break;
        case NSI_SD_PROTO_UDP:
            opt->proto = 0;
            break;
        default:
            nai_log_debug(NSI_LOG_CORE, EINVAL, 
                "the discovery handle "
                "option 'ipv4' with invalid proto(%d)", trl.proto);
            opt->error = 1;
            break;
        };
        break;

#if defined(NAI_HAVE_SOCKADDR_IN6)
    case NSI_SD_OPT_IN6U:
    case NSI_SD_OPT_IN6M:
    case NSI_SD_OPT_IN6D:
        if (ost.len != 21) {
            nai_log_debug(NSI_LOG_CORE, EINVAL, 
                "the discovery handle "
                "option 'ipv6' with invalid length(%d)", ost.len);
            opt->error = 1;
            break;
        };

        sa6 = (nai_sockaddr_in6_t*)nai_palloc(pool, sizeof(*sa6));
        if (sa6 == 0) {
            nai_log_alert(NSI_LOG_CORE, nai_errno, 
                "the discovery failed to allocate option 'ipv6'");
            r = -1;
            goto _end;
        };

        /* PRS_SOMEIPSD_00315
         * PRS_SOMEIPSD_00332
         * PRS_SOMEIPSD_00333
         */
        r = (int)nsi_message_read_least(io, 
            &sa6->sin6_addr, sizeof(sa6->sin6_addr));
        if (r < 0) {
            nai_log_debug(NSI_LOG_CORE, nai_errno, 
                "the discovery failed to read message option 'ipv6'");
            goto _end;
        };

        r = (int)nsi_message_read_least(io, &trl, sizeof(trl));
        if (r < 0) {
            nai_log_debug(NSI_LOG_CORE, nai_errno, 
                "the discovery failed to read message option 'ipv6'");
            goto _end;
        };

#if defined(NAI_HAVE_SOCKADDR_LEN)
        sa6->sin6_len = sizeof(*sa6);
#endif
        sa6->sin6_family = AF_INET6;
        sa6->sin6_port = trl.port;
        sa6->sin6_flowinfo = 0;
        sa6->sin6_scope_id = 0;


        /* fix scope_id of ipv6 address */
        nsi_discovery_fix_zone(p, sa6);


        /* set address option */
        opt->len = sizeof(*sa6);
        opt->data = sa6;
        switch (trl.proto) {
        case NSI_SD_PROTO_TCP:
            opt->proto = 1;
            break;
        case NSI_SD_PROTO_UDP:
            opt->proto = 0;
            break;
        default:
            nai_log_debug(NSI_LOG_CORE, EINVAL, 
                "the discovery handle "
                "option 'ipv6' with invalid proto(%d)", trl.proto);
            opt->error = 1;
            break;
        };
        break;
#endif

    case NSI_SD_OPT_CONF:
    case NSI_SD_OPT_SELECT:
        /* PRS_SOMEIPSD_00278
         * PRS_SOMEIPSD_00289
         */
        opt->proto = 0;
        opt->len = ost.len - 1;
        opt->data = nai_palloc(pool, opt->len);
        if (opt->data == 0) {
            nai_log_alert(NSI_LOG_CORE, nai_errno, 
                "the discovery failed to allocate option data");

            nai_errno = EINVAL;
            r = -1;
            goto _end;
        };
        r = (int)nsi_message_read_least(io, opt->data, opt->len);
        if (r < 0) {
            nai_log_debug(NSI_LOG_CORE, nai_errno, 
                "the discovery failed to read message option");
            goto _end;
        };
        break;

    default:
        nai_log_debug(NSI_LOG_CORE, nai_errno, 
                "the discovery handle unknown option type");
        opt->error = 1;
        break;
    };

    r = ost.len + 3;

_end:
    return (int)r;
};


static nsi_client_t* nsi_discovery_get_real_client(
    nsi_discovery_t* p, nsi_client_t* c, nsi_sd_option_t* opt, int opts)
{
    int r;
    int n;
    nsi_client_t* d;
    nsi_connid_t conn;
    nsi_endpoint_name_t name;


    conn = c->ep.conn;
    nai_sockname_init(&name);

    /* lookup option 'sd-endpoint' and use it to reply */
    for (n = 0; n < opts; n ++) {
        
        /* skip error opt*/
        if (opt[n].error == 1) {
            continue;
        };

        switch (opt[n].type) {
        case NSI_SD_OPT_IN4D:
#if defined(NAI_HAVE_SOCKADDR_IN6)
        case NSI_SD_OPT_IN6D:
#endif
            if (name.addr) {
                /* PRS_SOMEIPSD_00547
                 * PRS_SOMEIPSD_00554
                 */
                nai_log_debug(NSI_LOG_CORE, 0, 
                    "too many sd-endpoint in sd-message");
                continue;
            };
            if (n != 0) {
                /* PRS_SOMEIPSD_00651
                 * PRS_SOMEIPSD_00654
                 */
                nai_log_debug(NSI_LOG_CORE, 0, 
                    "sd-endpoint shall be first option in sd-message");
                continue;
            };
            /* tcp: flag=1, udp: flag=0 */
            if (opt[n].proto) {
                nai_log_debug(NSI_LOG_CORE, EINVAL, 
                    "the discovery cannot handle a tcp sd-endpoint");
                nai_errno = EINVAL;
                d = 0;
                goto _end;
            };

            name.len = opt[n].len;
            name.addr = (nai_sockaddr_t*)opt[n].data;
            break;
        default:
            break;
        };
    };
    if (name.addr == 0) {
        d = c;
    } else {
        /* check addreses, is not subnet or invalid family ? */
        if (!nsi_routing_is_subnet(p->rt, &name)) {
            /* PRS_SOMEIPSD_00656 */
            nai_log_debug(NSI_LOG_CORE, 0, 
                "the sd-endpoint is an invalid address");
            d = 0;
            goto _end;
        } else if (p->ep->name.addr->sa_family != name.addr->sa_family) {
            /* PRS_SOMEIPSD_00650
             * PRS_SOMEIPSD_00710
             * PRS_SOMEIPSD_00551
             * PRS_SOMEIPSD_00558
             */
            nai_log_debug(NSI_LOG_CORE, 0, 
                "the sd-endpoint with invalid address family");
            d = 0;
            goto _end;
        } else {
            /* ok */
            ;
        };

        /* get connection id */
        r = nsi_endpoint_set_opt(p->ep, 
            NSI_EOPT_BIND_TEMP, (intptr_t)&name);
        if (r < 0) {
            nai_log_error(NSI_LOG_CORE, nai_errno, 
                "the discovery failed to bind address of sd-endpoint");
            d = 0;
            goto _end;
        };

        d = c;
        if (conn != (nsi_connid_t)r) {

            if (name.addr->sa_family == AF_INET) {
                /* here, client id is different with connection id, 
                 * also see nsi_client_create
                 * PRS_SOMEIPSD_00549
                 */
            } else {
                /* PRS_SOMEIPSD_00556 */
                conn = r;
            };

            /* create reply client for answering
             * PRS_SOMEIPSD_00549
             * PRS_SOMEIPSD_00556
             */
            d = nsi_client_create(
                p->rt, r | NSI_CLIENT_ID_REMOTE, p->ep, conn, &c->cred);
            if (d == 0) {
                nai_log_alert(NSI_LOG_CORE, nai_errno, 
                    "the discovery failed to create reply client(0x%x)", 
                    conn | NSI_CLIENT_ID_REMOTE);
                goto _end;
            };
        };
    };

_end:
    return d;
};


int nsi_discovery_handle_message(
    nsi_discovery_t* p, nsi_message_t* m, nsi_endpoint_t* ep)
{
    int r;
    int ec;
    int n, i, l, count;
    int ents;
    int entlen;
    int opts;
    int optlen;
    int optcnt = 0;
    int batch = 0;
    int fails;
    size_t len;
    size_t off;
    int entvalid = 1;
    nai_pool_t* pool;
    nai_list_entry_t send;
    nsi_client_t* c;
    nsi_connid_t conn;
    nsi_message_io_t io;
    nsi_sd_msghdr_t hdr;
    nsi_sd_entry_t ent;
    nsi_sd_entry_t stopsubcache;
    nsi_sd_option_t* opt;
    nsi_endpoint_name_t names[2];


    if (p->link == 0) {
        nai_errno = ENOTCONN;
        r = -1;
        goto _end;
    };

    /* check head
     * PRS_SOMEIPSD_00125
     */
    len = m->hdr.len;
    len -= NSI_MSGEXT_SIZE;
    if (len < sizeof(hdr) + sizeof(uint32_t) * 2) {
        nai_log_debug(NSI_LOG_CORE, EINVAL, 
            "the discovery cannot "
            "handle message with invalid length(%d)", (int)len);
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    r = nsi_message_read_start(&io, m, 0);
    if (r < 0) {
        nai_log_error(NSI_LOG_CORE, nai_errno, 
            "the discovery failed to start read message");
        goto _end;
    };

    r = nsi_message_get_u4_be(&io, &hdr.u);
    if (r < 0) {
        nai_log_warn(NSI_LOG_CORE, nai_errno, 
            "the discovery failed to read message header");
        goto _end;
    };

    /* read the count of entry
     * PRS_SOMEIPSD_00251
     * PRS_SOMEIPSD_00262
     * PRS_SOMEIPSD_00265
     */
    r = nsi_message_get_u4_be(&io, (uint32_t*)&entlen);
    if (r < 0) {
        nai_log_warn(NSI_LOG_CORE, nai_errno, 
            "the discovery failed to read the bytes of entries");
        goto _end;
    };

    /* PRS_SOMEIPSD_00803 */
    if (entlen < 0) {
        nai_log_debug(NSI_LOG_CORE, EINVAL, 
            "the discovery cannot "
            "handle message with invalid entries size %d.", entlen);
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    nsi_message_read_end(&io);


    off = sizeof(hdr) + sizeof(uint32_t) + entlen;
    if (len < off + sizeof(uint32_t)) {
        nai_log_debug(NSI_LOG_CORE, EINVAL, 
            "the discovery try to "
            "handle message with invalid length(%d), "
            "total %d bytes of entries", 
            (int)len, entlen);
        entlen = len - sizeof(hdr) - sizeof(uint32_t);
        entvalid = 0;
    };

    ents = entlen / sizeof(ent);
    if (entlen != (int)(ents * sizeof(ent))) {
        nai_log_debug(NSI_LOG_CORE, EINVAL, 
            "the discovery try to "
            "handle message with invalid entires size %d.", entlen);
        entvalid = 0;
    };

    if (entvalid) {

        r = nsi_message_read_start(&io, m, off);
        if (r < 0) {
            nai_log_warn(NSI_LOG_CORE, nai_errno, 
                "the discovery failed to "
                "seek to offset(%d) of options", (int)off);
            goto _end;
        };

        /* read the count of option
        * PRS_SOMEIPSD_00251
        * PRS_SOMEIPSD_00264
        * PRS_SOMEIPSD_00265
        */
        r = nsi_message_get_u4_be(&io, (uint32_t*)&optlen);
        if (r < 0) {
            nai_log_warn(NSI_LOG_CORE, nai_errno, 
                "the discovery failed to read the bytes of options");
            goto _end;
        };

        if (optlen < 0) {
            nai_log_debug(NSI_LOG_CORE, EINVAL, 
                "the discovery cannot "
                "handle message with invalid options size %d.", optlen);
            nai_errno = EINVAL;
            r = -1;
            goto _end;
        };

        if ( (optlen + off + sizeof(uint32_t)) > len) {
            nai_log_debug(NSI_LOG_CORE, nai_errno, 
                "the discovery try to"
                "handle message with invalid options size %d.", optlen);
            optlen = len - off - sizeof(uint32_t);
        };

    } else {
        /* let fail later*/
        optlen = 1;
    };

    pool = &p->pool;
    nai_pool_reset(pool);
    optcnt = optlen/8;
    opt = (nsi_sd_option_t*)nai_palloc(pool, sizeof(*opt) * optcnt);
    if (opt == 0) {
        nai_log_alert(NSI_LOG_CORE, nai_errno, 
            "the discovery failed to allocate message options");
        r = -1;
        goto _end;
    };

    /* read options */
    for (opts = 0; (optlen > 4) && (opts < optcnt); ) {
        r = nsi_discovery_read_option(p, &io, opt + opts);
        if (r < 0) {
            nai_log_debug(NSI_LOG_CORE, nai_errno, 
                "the discovery failed to read option(%d).", opts);
            break;
        };

        optlen -= r;
        opts ++;
    };


    nsi_message_read_end(&io);


    nai_log_debug(NSI_LOG_CORE, 0, 
        "discovery handle message with %d enties and %d options", ents, opts);


    /* PRS_SOMEIPSD_00262 */
    r = nsi_message_read_start(&io, m, sizeof(hdr) + sizeof(uint32_t));
    if (r < 0) {
        nai_log_alert(NSI_LOG_CORE, nai_errno, 
            "the discovery failed to seek to offset(%d) of entries", 
            (int)(sizeof(hdr) + sizeof(uint32_t)));
        goto _end;
    };


    if (ep == p->ep) {
        conn = m->conn;
    } else if (ep == p->mp) {
        r = nsi_endpoint_map_conn(p->ep, ep, m->conn);
        if (r < 0) {
            nai_log_error(NSI_LOG_CORE, nai_errno, 
                "the discovery failed to map connection from multicast");
            goto _end;
        };

        conn = (nsi_connid_t)r;
    } else {
        /* the endpoint isn't belong discovery */
        nai_log_debug(NSI_LOG_CORE, EINVAL, 
            "the message from an invalid endpoint which not belong discovery");
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };
    c = nsi_client_create(
        p->rt, conn | NSI_CLIENT_ID_REMOTE, p->ep, conn, &m->cred);
    if (c == 0) {
        nai_log_alert(NSI_LOG_CORE, nai_errno, 
            "the discovery failed to create remote client(0x%x)", 
            conn | NSI_CLIENT_ID_REMOTE);
        r = -1;
        goto _end;
    };

    /* lookup option 'sd-endpoint' and use it to reply */
    c = nsi_discovery_get_real_client(p, c, opt, opts);
    if (c == 0) {
        r = -1;
        goto _end;
    };


    /* PRS_SOMEIPSD_00702 */
    /* save unicast flags */
    c->accept_ucast = !!(hdr.flags & NSI_SD_UNICAST);
    /* save explicit initial data control flag , this value should be 0 for 2311 version. */
    c->initial_expl = !!(hdr.flags & NSI_SD_INITDATA);

    /* check reboot */
    r = nsi_discovery_handle_reboot(
        p, c, hdr.flags, m->hdr.session, ep == p->ep);
    if (r < 0) {
        goto _end;
    };


    /* begin response */
    r = nsi_routing_batch_begin(p->rt);
    if (r < 0) {
        nai_log_alert(NSI_LOG_CORE, 
            nai_errno, "start batch failed");
        goto _end;
    };

    batch = 1;
    p->out = c;
    p->list = &send;
    nai_list_init(&send);

    stopsubcache.type = 0xff;

    /* read entry and process
     * PRS_SOMEIPSD_00266
     * PRS_SOMEIPSD_00496
     */
    for (n = 0; n < ents; n ++) {
        r = (int)nsi_message_read_least(&io, &ent.type, sizeof(uint32_t));
        if (r < 0) {
            nai_log_error(NSI_LOG_CORE, nai_errno, 
                "the discovery failed to read entry %d", n);
            goto _end;
        };

        r = nsi_message_get_u4_be(&io, &ent.u1);
        if (r < 0) {
            nai_log_error(NSI_LOG_CORE, nai_errno, 
                "the discovery failed to read int 1 of entry %d", n);
            goto _end;
        };

        r = nsi_message_get_u4_be(&io, &ent.u2);
        if (r < 0) {
            nai_log_error(NSI_LOG_CORE, nai_errno, 
                "the discovery failed to read int 2 of entry %d", n);
            goto _end;
        };

        r = nsi_message_get_u4_be(&io, &ent.u3);
        if (r < 0) {
            nai_log_error(NSI_LOG_CORE, nai_errno, 
                "the discovery failed to read int 3 of entry %d", n);
            goto _end;
        };

        /* PRS_SOMEIPSD_00341 */
        if (ent.optc.n1 != 0 && ent.optc.n0 == 0) {
            nai_log_debug(NSI_LOG_CORE, EINVAL, 
                "the discovery cannot handle entry %d "
                "with empty first option", n);
        };

        /* process option */
        fails = 0;
        for (l = 0; l < (int)nai_countof(names); l ++) {
            nai_sockname_init(&names[l]);
        };
        for (l = 0; l < 2; l ++) {
            i = ent.opt[l];
            count = i;
            count += l ? ent.optc.n1 : ent.optc.n0;
            if (i > opts || count > opts) {
                nai_log_debug(NSI_LOG_CORE, EINVAL, 
                    "the discovery cannot handle entry %d "
                    "with option index(%d, %d) is out of range", n, i, count);

                /* PRS_SOMEIPSD_00130 */
                fails = 1;
            };
            if (fails) {
                break;
            };

            /* PRS_SOMEIPSD_00343 */
            if (i && i >= count) {
                nai_log_debug(NSI_LOG_CORE, EINVAL, 
                    "the discovery handle entry %d "
                    "with option index(%d, %d) is invalid.", n, i, count);
            };


            /* PRS_SOMEIPSD_00342
             * PRS_SOMEIPSD_00231
             */
            for ( ; i < count; i ++) {

                /* fail when any error opt involved */
                if (opt[i].error == 1) {
                    fails = 1 ;
                    break;
                };

                switch (opt[i].type) {
                case NSI_SD_OPT_CONF:
                case NSI_SD_OPT_LOAD:
                    break;

                case NSI_SD_OPT_IN4U:
#if defined(NAI_HAVE_SOCKADDR_IN6)
                case NSI_SD_OPT_IN6U:
#endif
                    /* tcp: proto=1, udp: proto=0 */
                    /* PRS_SOMEIPSD_00528
                     * PRS_SOMEIPSD_00529
                     * PRS_SOMEIPSD_00358
                     * PRS_SOMEIPSD_00359
                     */
                    if (ent.type != NSI_SD_CMD_ACK && 
                        ent.type != NSI_SD_CMD_FIND) {
                        if (names[opt[i].proto].addr) {
                            /* repeat */
                            nai_log_debug(NSI_LOG_CORE, EINVAL, 
                                "the discovery get repeat address of " 
                                "proto(%d) from same entry %d ", 
                                opt[i].proto, n);

                            fails = 1;
                            continue;
                        };
                        names[opt[i].proto].len = opt[i].len;
                        names[opt[i].proto].addr = (nai_sockaddr_t*)opt[i].data;
                    } else if (ent.type == NSI_SD_CMD_FIND) {
                        /* skip opt for NSI_SD_CMD_FIND */
                        continue;
                    } else {
                        /* PRS_SOMEIPSD_00130 */
                        if (!opt[i].discardable) {
                            fails = 1;
                        };
                    };
                    break;

                case NSI_SD_OPT_IN4M:
#if defined(NAI_HAVE_SOCKADDR_IN6)
                case NSI_SD_OPT_IN6M:
#endif
                    /* tcp: proto=1, udp: proto=0 */
                    /* PRS_SOMEIPSD_00489 */
                    if (opt[i].proto) {
                        nai_log_debug(NSI_LOG_CORE, EINVAL, 
                            "the discovery cannot handle entry %d "
                            "with invalid multicast proto(%d)", 
                            n, opt[i].proto);
                        nai_errno = EINVAL;
                        r = -1;
                        goto _end;
                    };

                    /* PRS_SOMEIPSD_00323
                     * PRS_SOMEIPSD_00545
                     * PRS_SOMEIPSD_00528
                     * PRS_SOMEIPSD_00529
                     * PRS_SOMEIPSD_00488
                     */
                    if (ent.type == NSI_SD_CMD_ACK) {
                        if (names[0].addr) {
                            /* repeat */
                            nai_log_debug(NSI_LOG_CORE, EINVAL, 
                                "the discovery get repeat address of " 
                                "proto(%d) from same entry %d ", 
                                0, n);

                            fails = 1;
                            continue;
                        };
                        names[0].len = opt[i].len;
                        names[0].addr = (nai_sockaddr_t*)opt[i].data;
                    } else if (ent.type == NSI_SD_CMD_FIND) {
                        /* skip opt for NSI_SD_CMD_FIND */
                        continue;
                    } else {
                        /* PRS_SOMEIPSD_00130 */
                        if (!opt[i].discardable) {
                            fails = 1;
                        };
                    };
                    break;

                case NSI_SD_OPT_IN4D:
#if defined(NAI_HAVE_SOCKADDR_IN6)
                case NSI_SD_OPT_IN6D:
#endif
                    /* PRS_SOMEIPSD_00548
                     * PRS_SOMEIPSD_00555
                     */

                default:
                    /* PRS_SOMEIPSD_00130 */
                    if (!opt[i].discardable) {
                        fails = 1;
                    };

                    /* PRS_SOMEIPSD_00530 */
                    break;
                };
            };
        };

        if (stopsubcache.type != 0xff) {
            if (ent.type == stopsubcache.type && p->version == 2311) {
                if (ent.ttl != 0 && ent.major == stopsubcache.major) {
                    if (ent.u1 == stopsubcache.u1 && ent.u3 == stopsubcache.u3
                        && ent.optc.n == stopsubcache.optc.n && ent.opt[0] == stopsubcache.opt[0]
                        && ent.opt[1] == stopsubcache.opt[1]) {
                        ent.init = 1;
                    };
                };
            }; 

            r = nsi_discovery_handle_subscribe(p, &stopsubcache, c, names, &send, 0);
            if (r < 0) {
                goto _fail;
            };

            stopsubcache.type = 0xff;
        };

        if (fails == 0) {
            /* process it
             * PRS_SOMEIPSD_00263
             * PRS_SOMEIPSD_00385
             */
            switch (ent.type) {
            case NSI_SD_CMD_OFFER:
                r = nsi_discovery_handle_offer(p, &ent, c, names, &send);
                break;
            case NSI_SD_CMD_FIND:
                /* PRS_SOMEIPSD_00422 */
                r = nsi_discovery_handle_find(p, &ent, ep, c, &send);
                break;
            case NSI_SD_CMD_SUBSCRIBE:
                /* PRS_SOMEIPSD_00122 */
                if (ent.ttl == 0 && ((n + 1) < ents)) {
                    stopsubcache = ent;
                    break;
                };
                r = nsi_discovery_handle_subscribe(p, &ent, c, names, &send, 0);
                break;
            case NSI_SD_CMD_ACK:
                r = nsi_discovery_handle_ack(p, &ent, c, names, &send);
                break;
            default:
                nai_log_debug(NSI_LOG_CORE, EINVAL, 
                    "the discovery cannot handle unknown type %d of entry %d", 
                    ent.type, n);
                nai_errno = EINVAL;
                r = -1;
                break;
            };
        } else {
            switch (ent.type) {
            case NSI_SD_CMD_ACK:
                /* PRS_SOMEIPSD_00235 */
                ent.ttl = 0;
                r = nsi_discovery_handle_ack(p, &ent, c, names, &send);
                break;

            case NSI_SD_CMD_SUBSCRIBE:
                /* PRS_SOMEIPSD_00234 */
                if (ent.ttl != 0) {
                    /* let it failed */
                    r = nsi_discovery_handle_subscribe(
                        p, &ent, c, names, &send, 1);
                    break;
                };

                /* fallthrough */

            default:
                /* PRS_SOMEIPSD_00232
                 * PRS_SOMEIPSD_00233
                 */
                r = 0;
                break;
            };
        };
        if (r < 0) {
            goto _fail;
        };
    };

    /* PRS_SOMEIPSD_00419 */
    if (!nai_list_is_empty(&send)) {
        r = nsi_discovery_send(p, &send, c);
        if (r < 0) {
            nai_log_error(NSI_LOG_CORE, 
                nai_errno, "the discovery send reply message failed");
        };
    };

_fail:
    nsi_message_close(m);
    r = 0;

_end:
    if (batch) {
        p->out = 0;
        p->list = 0;

        if (r < 0) {
            ec = nai_errno;
        };
        nsi_routing_batch_end(p->rt);
        if (r < 0) {
            nai_errno = ec;
        };
    };
    return (int)r;
};


static int nsi_discovery_put_addr(
    char* buf, const nsi_endpoint_name_t* name, int tcp, int type)
{
    int r;
    int addrlen;
    void* addr;
    nsi_sd_opthdr_t hdr;
    nsi_sd_address_t trl;
    nai_sockaddr_t* sa;


    hdr.discardable = 0;
    hdr.reserved = 0;
    hdr.type = type;

    trl.reserved = 0;
    if (tcp) {
        trl.proto = NSI_SD_PROTO_TCP;
    } else {
        trl.proto = NSI_SD_PROTO_UDP;
    };

    /* PRS_SOMEIPSD_00274
     * PRS_SOMEIPSD_00497
     * PRS_SOMEIPSD_00498
     */
    sa = name->addr;
    switch (sa->sa_family) {
    case AF_INET:
        /* PRS_SOMEIPSD_00306
         * PRS_SOMEIPSD_00307
         * PRS_SOMEIPSD_00325
         * PRS_SOMEIPSD_00326
         * PRS_SOMEIPSD_00552
         * PRS_SOMEIPSD_00559
         */
        addr = &((nai_sockaddr_in4_t*)sa)->sin_addr;
        addrlen = sizeof(nai_addr_in4_t);
        hdr.type |= NSI_SD_OPT_IN4U;
        hdr.len = addrlen + sizeof(trl) + 1;
        trl.port = ((nai_sockaddr_in4_t*)sa)->sin_port;
        break;

#if defined(NAI_HAVE_SOCKADDR_IN6)
    case AF_INET6:
        /* PRS_SOMEIPSD_00315
         * PRS_SOMEIPSD_00332
         * PRS_SOMEIPSD_00333
         */
        addr = &((nai_sockaddr_in6_t*)sa)->sin6_addr;
        addrlen = sizeof(nai_addr_in6_t);
        hdr.type |= NSI_SD_OPT_IN6U;
        hdr.len = addrlen + sizeof(trl) + 1;
        trl.port = ((nai_sockaddr_in6_t*)sa)->sin6_port;
        break;
#endif

    default:
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    if (buf) {
        hdr.u = nai_htonl(hdr.u);
        nai_memcpy(buf, &hdr, sizeof(hdr));
        buf += sizeof(hdr);
        nai_memcpy(buf, addr, addrlen);
        buf += addrlen;
        nai_memcpy(buf, &trl, sizeof(trl));
        buf += sizeof(trl);
    };

    r = sizeof(hdr) + sizeof(trl) + addrlen;

_end:
    return r;
};


#if 0
static int nsi_discovery_put_clients(
    char* buf, const nsi_cid_t* a, int count)
{
    int r;
    int n;
    uint16_t v;
    nsi_sd_opthdr_t hdr;


    if (buf) {
        hdr.len = (short)count * sizeof(v) + 1;
        hdr.type = NSI_SD_OPT_SELECT;
        hdr.discardable = 0;
        hdr.reserved = 0;
        hdr.u = nai_htonl(hdr.u);

        nai_memcpy(buf, &hdr, sizeof(hdr));
        buf += sizeof(hdr);

        for (n = 0; n < count; n ++) {
            v = nai_htons((uint16_t)a[n]);
            nai_memcpy(buf, &v, sizeof(v));
            buf += sizeof(v);
        };
    };

    r = sizeof(hdr) + (int)count * sizeof(v);


    return r;
};
#endif


int nsi_discovery_send(
    nsi_discovery_t* p, nai_list_entry_t* list, nsi_client_t* c)
{
    int r;
    int n;
    int ec;
    int sub;
    int stop;
    int ents;
    int opts;
    int ecount;
    int ocount;
    size_t size;
    size_t len;
    size_t max = p->mss - sizeof(nsi_msghdr_t);
    char *buf, *ent, *opt;
    nai_list_entry_t* e;
    nai_list_entry_t* last;
    nai_list_entry_t* tmp;
    nsi_sd_timestat_t* t;
    nsi_sd_request_t* q;
    nsi_sd_service_t* s;
    nsi_sd_subscribe_t* u;
    nsi_endpoint_t* ep;
    nsi_message_t* m;
    nai_buf_t* b;
    nsi_sd_msghdr_t hdr;
    nsi_sd_entry_t ed;


    /* PRS_SOMEIPSD_00266
     * PRS_SOMEIPSD_00800
     * PRS_SOMEIPSD_00496
     */
    last = list->next;
    while (last != list) {
        /* PRS_SOMEIPSD_00262 */
        size = sizeof(nsi_sd_msghdr_t);
        size += sizeof(uint32_t) * 2;
        ents = 0;
        opts = 0;

        if (p->use_sd_option) {
            /* PRS_SOMEIPSD_00547
             * PRS_SOMEIPSD_00650
             * PRS_SOMEIPSD_00651
             * PRS_SOMEIPSD_00548
             * PRS_SOMEIPSD_00551
             * PRS_SOMEIPSD_00654
             * PRS_SOMEIPSD_00555
             * PRS_SOMEIPSD_00558
             */
            opts ++;
            size += nsi_discovery_put_addr(
                0, &p->name, 0, NSI_SD_OPT_DISCOVERY);
        };

        e = last;
        for ( ; e != list; e = e->next) {
            t = nai_containof(e, nsi_sd_timestat_t, ent);
            len = sizeof(nsi_sd_entry_t);
            ecount = 1;
            ocount = 0;
            switch (t->type) {
            case NSI_SD_TYPE_OFFER:
                /* PRS_SOMEIPSD_00358
                 * PRS_SOMEIPSD_00359
                 */
                s = nai_containof(t, nsi_sd_service_t, time);
                for (n = 0; n < 2; n ++) {
                    if (s->names[n].addr == 0) {
                        continue;
                    };
                    len += nsi_discovery_put_addr(
                        0, s->names+n, n, NSI_SD_OPT_UNICAST);
                    ocount ++;
                };
                break;
            case NSI_SD_TYPE_FIND:
                /* PRS_SOMEIPSD_00528 */
                break;
            case NSI_SD_TYPE_SUBSCRIBE:
            case NSI_SD_TYPE_ACK:
                /* PRS_SOMEIPSD_00472 */
                u = nai_containof(t, nsi_sd_subscribe_t, time);
                if (c == 0) {
                    nai_log_warn(NSI_LOG_CORE, 0, 
                        "we can't send a subscirbe/ack of service(%d, %d) "
                        "without a peer", 
                        u->serv, u->inst);
                    continue;
                };

                stop = 0;
                sub = t->type != NSI_SD_TYPE_ACK;
                if (sub) {
                    switch (t->stat) {
                    case NSI_SD_STAT_REMOVE:
                        stop = 1;
                        break;
                    case NSI_SD_STAT_WAITING:
                        if (p->version == 2311) {
                            len += sizeof(nsi_sd_entry_t);
                            ecount ++;
                        };
                        break;
                    case NSI_SD_STAT_PENDING:
                    case NSI_SD_STAT_ACKED_RENEW:
                        /* repeat >= 1 means lost ack, we send stop in case:
                         * 1. explicit initial data of server is 0
                         * 2. last initial data of ack is 0
                         * PRS_SOMEIPSD_00463
                         */
                        if (p->version == 2311) {
                            if (t->repeat >= 1) {
                                len += sizeof(nsi_sd_entry_t);
                                ecount ++;
                            };
                        } else {
                            if (t->repeat >= 1 && (
                                u->init == 0 || c->initial_expl == 0)) {
                                len += sizeof(nsi_sd_entry_t);
                                ecount ++;
                            };
                        };
                        break;
                    default:
                        break;
                    };
                };

                /* put address options */
                /* PRS_SOMEIPSD_00323
                 * PRS_SOMEIPSD_00329
                 * PRS_SOMEIPSD_00545
                 * PRS_SOMEIPSD_00336
                 * PRS_SOMEIPSD_00488
                 * PRS_SOMEIPSD_00489
                 */
                for (n = 0; n < 1 + sub; n ++) {
                    if (u->names[n].addr == 0) {
                        continue;
                    };
                    len += nsi_discovery_put_addr(0, u->names+n, 
                        n, sub ? NSI_SD_OPT_UNICAST : NSI_SD_OPT_MCAST);
                    ocount ++;
                };
#if 0
                if (u->cid != 0) {
                    len += nsi_discovery_put_clients(0, &u->cid, 1);
                    count ++;
                };
#endif
                break;
            default:
                break;
            };

            if (size + len > max) {
                break;
            };

            size += len;
            ents += ecount;
            opts += ocount;
        };

        m = nsi_network_create_message(p->rt->net, NSI_USAGE_SEND);
        if (m == 0) {
            nai_log_alert(NSI_LOG_CORE, nai_errno, 
                "the discovery failed to create message");
            goto _fail;
        };

        b = nai_buf_alloc(m->payload.pool, size);
        if (b == 0) {
            nai_log_alert(NSI_LOG_CORE, nai_errno, 
                "the discovery failed to create buffer with size %d", 
                (int)size);
            goto _fail;
        };

        buf = (char*)nai_buf_ptr(b);

        /* fill header
         * PRS_SOMEIPSD_00156
         * PRS_SOMEIPSD_00157
         * PRS_SOMEIPSD_00158
         * PRS_SOMEIPSD_00159
         * PRS_SOMEIPSD_00160
         * PRS_SOMEIPSD_00255
         * PRS_SOMEIPSD_00256
         * PRS_SOMEIPSD_00631
         */
        if (c && c->accept_ucast && 
            p->ep != p->mp) { /* for debug discovery */
            c->ep.session ++;
            if (c->ep.session == 0) {
                c->ep.session = 1;
                c->reboot = 0;
            };

            m->hdr.session = c->ep.session;
            m->conn = c->ep.conn;
            ep = c->ep.ep;

            hdr.flags = c->reboot ? NSI_SD_REBOOT : 0;
        } else {
            p->session ++;
            if (p->session == 0) {
                p->session = 1;
                p->reboot = 0;
            };

            /* PRS_SOMEIPSD_00600 */
            m->hdr.session = p->session;
            m->conn = p->mcast;
            ep = p->ep;

            hdr.flags = p->reboot ? NSI_SD_REBOOT : 0;
        };

        /* PRS_SOMEIPSD_00540
         * PRS_SOMEIPSD_00701
         * PRS_SOMEIPSD_00702
         */
        if (p->version == 2311) {
            hdr.flags |= NSI_SD_UNICAST;
        } else {
            hdr.flags |= NSI_SD_UNICAST|NSI_SD_INITDATA;
        };
        hdr.reserved = 0;

        /* put header */
        hdr.u = nai_htonl(hdr.u);
        nai_memcpy(buf, &hdr, sizeof(hdr));

        /* put entry count
         * PRS_SOMEIPSD_00251
         * PRS_SOMEIPSD_00262
         * PRS_SOMEIPSD_00265
         */
        ent = buf + sizeof(hdr);
        *((uint32_t*)ent) = nai_htonl(ents * sizeof(nsi_sd_entry_t));
        ent += sizeof(uint32_t);

        /* put option count
         * PRS_SOMEIPSD_00251
         * PRS_SOMEIPSD_00264
         * PRS_SOMEIPSD_00265
         */
        opt = ent + ents * sizeof(nsi_sd_entry_t);
        *((uint32_t*)opt) = nai_htonl(
            (uint32_t)(size - (opt - buf + sizeof(uint32_t))));
        opt += sizeof(uint32_t);

#if 1
        nai_log_debug(NSI_LOG_CORE, 0, 
            "send a discovery message with entries %d and options %d", 
            ents, opts);
#endif
        ents = 0;
        opts = 0;

        if (p->use_sd_option) {
            /* PRS_SOMEIPSD_00547
             * PRS_SOMEIPSD_00650
             * PRS_SOMEIPSD_00651
             * PRS_SOMEIPSD_00548
             * PRS_SOMEIPSD_00551
             * PRS_SOMEIPSD_00654
             * PRS_SOMEIPSD_00555
             * PRS_SOMEIPSD_00558
             */
            opts ++;
            opt += nsi_discovery_put_addr(
                opt, &p->name, 0, NSI_SD_OPT_DISCOVERY);
        };

        /* swap */
        tmp = e;
        e = last; last = tmp;

        /* PRS_SOMEIPSD_00266
         * PRS_SOMEIPSD_00341
         */
        for ( ; e != last; e = e->next) {
            t = nai_containof(e, nsi_sd_timestat_t, ent);
            ecount = 1;
            ocount = 0;
            switch (t->type) {
            case NSI_SD_TYPE_OFFER:
                s = nai_containof(t, nsi_sd_service_t, time);
                s->last_offering =  nai_tickcache_to_msec();
                ed.type = NSI_SD_CMD_OFFER;
                ed.serv = s->serv;
                ed.inst = s->inst;
                ed.major = s->major;
                ed.minor = s->minor;
                ed.ttl = s->ttl;
                ed.opt[0] = opts;
                ed.opt[1] = 0;
                ed.optc.n0 = ed.optc.n1 = 0;
                for (n = 0; n < 2; n ++) {
                    if (s->names[n].addr == 0) {
                        continue;
                    };
                    opt += nsi_discovery_put_addr(
                        opt, s->names+n, n, NSI_SD_OPT_UNICAST);
                    ocount ++;
                    ed.optc.n0 ++;
                };
                break;
            case NSI_SD_TYPE_FIND:
                q = nai_containof(t, nsi_sd_request_t, time);
                ed.type = NSI_SD_CMD_FIND;
                ed.serv = q->serv;
                ed.inst = q->inst;
                ed.major = q->major;
                ed.minor = q->minor;
                ed.ttl = -1;
                ed.opt[0] = 0;
                ed.opt[1] = 0;
                ed.optc.n0 = ed.optc.n1 = 0;
                break;
            case NSI_SD_TYPE_SUBSCRIBE:
            case NSI_SD_TYPE_ACK:
                u = nai_containof(t, nsi_sd_subscribe_t, time);
                stop = 0;
                sub = t->type != NSI_SD_TYPE_ACK;
                if (sub) {
                    switch (t->stat) {
                    case NSI_SD_STAT_REMOVE:
                        stop = 1;
                        break;
                    case NSI_SD_STAT_WAITING:
                        if (p->version == 2311) {
                            ecount ++;
                        };
                        break;
                    case NSI_SD_STAT_PENDING:
                    case NSI_SD_STAT_ACKED_RENEW:
                        /* repeat >= 1 means lost ack, we send stop in case:
                         * 1. explicit initial data of server is 0
                         * 2. last initial data of ack is 0
                         * PRS_SOMEIPSD_00463
                         */
                        if (p->version == 2311) {
                            if (t->repeat >= 1) {
                                ecount ++;
                            };
                        } else {
                            if (t->repeat >= 1 && (
                                u->init == 0 || c->initial_expl == 0)) {
                                ecount ++;
                            };
                        };
                        break;
                    default:
                        break;
                    };
                };

                /* PRS_SOMEIPSD_00385
                 * PRS_SOMEIPSD_00386
                 * PRS_SOMEIPSD_00391
                 * PRS_SOMEIPSD_00394
                 */
                ed.type = !sub ? NSI_SD_CMD_ACK : NSI_SD_CMD_SUBSCRIBE;
                ed.serv = u->serv;
                ed.inst = u->inst;
                ed.major = u->major;
                ed.gid = u->gid;
                ed.counter = 0; /* not used */
                ed.reserved1 = 0;
                ed.reserved2 = 0;
                ed.ttl = 0;
                ed.opt[0] = opts;
                ed.opt[1] = 0;
                ed.optc.n0 = ed.optc.n1 = 0;

                if (p->version == 2311) {
                    ed.init = 0;
                } else {
                    if (sub) {
                    /* PRS_SOMEIPSD_00703 */
                        ed.init = !u->init;
                    } else {
                        ed.init = u->init;
                    };
                };

                /* put address options */
                /* PRS_SOMEIPSD_00323
                 * PRS_SOMEIPSD_00545
                 * PRS_SOMEIPSD_00488
                 * PRS_SOMEIPSD_00489
                 */
                for (n = 0; n < 1 + sub; n ++) {
                    if (u->names[n].addr == 0) {
                        continue;
                    };
                    opt += nsi_discovery_put_addr(opt, u->names+n, 
                        n, sub ? NSI_SD_OPT_UNICAST : NSI_SD_OPT_MCAST);
                    ocount ++;
                    ed.optc.n0 ++;
                };

                /* put stop before subscribe */
                if (ecount > 1) {
                    ed.u1 = nai_htonl(ed.u1);
                    ed.u2 = nai_htonl(ed.u2);
                    ed.u3 = nai_htonl(ed.u3);
                    nai_memcpy(ent, &ed, sizeof(ed));
                    ent += sizeof(ed);
                    ed.u1 = nai_ntohl(ed.u1);
                    ed.u2 = nai_ntohl(ed.u2);
                    ed.u3 = nai_ntohl(ed.u3);
                };

                /* PRS_SOMEIPSD_00452 */
                ed.ttl = stop ? 0 : u->ttl;
#if 0
                if (u->cid != 0) {
                    opt += nsi_discovery_put_clients(opt, &u->cid, 1);
                    count ++;
                    ed.optc.n0 ++;
                };
#endif
                break;
            default:
                continue;
            };

            /* PRS_SOMEIPSD_00343 */
            if (ed.optc.n0 == 0) {
                ed.opt[0] = 0;
            };

            ed.u1 = nai_htonl(ed.u1);
            ed.u2 = nai_htonl(ed.u2);
            ed.u3 = nai_htonl(ed.u3);
            nai_memcpy(ent, &ed, sizeof(ed));
            ent += sizeof(ed);
            ents += ecount;
            opts += ocount;
        };

        /* PRS_SOMEIPSD_00250
         * PRS_SOMEIPSD_00154
         * PRS_SOMEIPSD_00163
         * PRS_SOMEIPSD_00164
         */
        m->hdr.serv = NSI_DISCOVERY_SERV;
        m->hdr.method = NSI_DISCOVERY_METHOD;
        m->hdr.len = NSI_MSGEXT_SIZE + (uint32_t)size;
        m->hdr.client = 0;
        m->hdr.protocol = NSI_SD_PROTOCOL;
        m->hdr.interface = NSI_SD_INTERFACE;
        m->hdr.type = NSI_MT_NOTIFICATION;
        m->hdr.code = NSI_E_OK;

        nai_buf_wcommit(b, size);
        nai_buflist_insert_tail(&m->payload, b);

        /* PRS_SOMEIPSD_00252 */
        r = nsi_endpoint_send(
            ep, m, 0, p->rt->batch ? NSI_SEND_POSTED : 0);
        if (r < 0) {
            nai_log_error(NSI_LOG_CORE, 
                nai_errno, "the discovery send message bypass %s failed", 
                ep == p->ep ? "unicast" : "multicast");
            goto _fail;
        };
    };

    r = 0;

_end:
    return (int)r;

_fail:
    if (m != 0) {
        ec = nai_errno;
        nsi_message_close(m);
        nai_errno = ec;
    };

    r = -1;
    goto _end;
};


int nsi_discovery_send_dispatch(nsi_discovery_t* p, nai_list_t* list)
{
    int r;
    nsi_client_t* c;
    nai_list_t send;
    nai_list_t sent;
    nai_list_entry_t* e;
    nsi_sd_timestat_t* t;
    nsi_sd_service_t* s;
    nsi_sd_subscribe_t* u;


    nai_list_init(&sent);

    for (;;) {
        /* PRS_SOMEIPSD_00266 */
        nai_list_init(&send);

        c = 0;
        e = list->next;
        for ( ; e != list; ) {
            t = nai_containof(e, nsi_sd_timestat_t, ent);
            e = e->next;
            switch (t->type) {
            case NSI_SD_TYPE_OFFER:
                if (!t->flags) {
                    /* same as interval offer, treat it like c == NULL */
                    break;
                }
                s = nai_containof(t, nsi_sd_service_t, time);
                if (!s->own) {
                    break;
                }
                if (c == 0) {
                    c = s->own;
                };
                if (c != s->own) {
                    break;
                };

                nai_list_entry_remove(&t->ent);
                nai_list_insert_tail(&send, &t->ent);
                break;
            case NSI_SD_TYPE_SUBSCRIBE:
                u = nai_containof(t, nsi_sd_subscribe_t, time);
                s = u->service;
                if (!s->own) {
                    break;
                };

                if (c == 0) {
                    c = s->own;
                };
                if (c != s->own) {
                    break;
                };

                nai_list_entry_remove(&u->time.ent);
                nai_list_insert_tail(&send, &u->time.ent);
                break;

            default:
                break;
            };
        };

        if (nai_list_is_empty(&send)) {
            break;
        };

        r = nsi_discovery_send(p, &send, c);
        nai_list_add_tail(&sent, &send);
        if (r < 0) {
            goto _end;
        };
    };

    if (!nai_list_is_empty(list)) {
        r = nsi_discovery_send(p, list, 0);
        if (r < 0) {
            goto _end;
        };
    };

    r = 0;

_end:
    nai_list_add_head(list, &sent);
    return r;
};


