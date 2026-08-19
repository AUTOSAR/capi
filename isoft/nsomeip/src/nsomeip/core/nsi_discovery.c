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
/// @file       nsi_discovery.c
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
#include "nsomeip/net/nsi_network.h"
#include "nai/runtime/nai_util.h"



typedef struct nsi_sd_servent_s {
    nai_rbnode_t ent;
    nsi_serv_t serv;
    nai_list_entry_t list;
} nsi_sd_servent_t;



static int nsi_discovery_notify_exist(
    nsi_discovery_t* p, 
    nsi_cid_t cid, nsi_serv_t serv, nsi_inst_t inst, 
    nsi_version_t major, nsi_versmin_t minor);


static nai_rbnode_t** nsi_discovery_find_servent(
    nai_rbtree_t* t, nsi_serv_t serv, nai_rbnode_t** pparent)
{
    nai_rbnode_t** n = &nai_rbtree_root(t);
    nai_rbnode_t* parent = nai_rbtree_end(t);
    nsi_sd_servent_t* e;


    while (*n) {
        parent = *n;
        e = nai_containof(parent, nsi_sd_servent_t, ent);
        if (e->serv == serv) {
            break;
        } else if (e->serv >= serv) {
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


static nsi_sd_servent_t* nsi_discovery_get_servent(
    nsi_discovery_t* p, nsi_serv_t serv, int na)
{
    nai_rbnode_t** n;
    nai_rbnode_t* parent;
    nsi_sd_servent_t* r;


    n = nsi_discovery_find_servent(&p->finds, serv, &parent);
    if (n[0] != 0) {
        r = nai_containof(n[0], nsi_sd_servent_t, ent);
        goto _end;
    };

    if (na) {
        nai_errno = ENOENT;
        r = 0;
        goto _end;
    };


    r = nsi_discovery_alloc(p, sizeof(*r));
    if (r == 0) {
        nai_log_alert(NSI_LOG_CORE, nai_errno, 
            "the discovery failed to allocate memory of servent(%d)", serv);
        goto _end;
    };


    r->serv = serv;
    nai_list_init(&r->list);
    nai_rbtree_link(&p->finds, &r->ent, parent, n);
    nai_rbtree_color(&p->finds, &r->ent);


_end:
    return r;
};


static nsi_sd_request_t* nsi_discovery_get_request(
    nsi_discovery_t* p, nsi_sd_servent_t* s, 
    nsi_inst_t inst, nsi_version_t major, nsi_versmin_t minor)
{
    nai_list_entry_t* e;
    nsi_sd_request_t* r;


    (void)p;

    e = s->list.next;
    for ( ; e != &s->list; e = e->next) {
        r = (nsi_sd_request_t*)e;
        if (r->inst == inst && 
            r->major == major && r->minor == minor) {
            goto _end;
        };
    };

    nai_errno = ENOENT;
    r = 0;

_end:
    return r;
};


static nai_rbnode_t** nsi_discovery_find_service(
    nai_rbtree_t* t, nsi_serv_t serv, nsi_inst_t inst, 
    nai_rbnode_t** pparent)
{
    nai_rbnode_t** n = &nai_rbtree_root(t);
    nai_rbnode_t* parent = nai_rbtree_end(t);
    nsi_sd_service_t* e;
    nsi_servkey_t v;


    v.serv = serv;
    v.inst = inst;

    while (*n) {
        parent = *n;
        e = nai_containof(parent, nsi_sd_service_t, ent);
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


nsi_sd_service_t* nsi_discovery_get_service(
    nsi_discovery_t* p, nai_rbtree_t* t, nsi_serv_t serv, nsi_inst_t inst)
{
    nai_rbnode_t** n;
    nsi_sd_service_t* r;


    (void)p;

    n = nsi_discovery_find_service(t, serv, inst, 0);
    if (n[0] == 0) {
        nai_errno = ENOENT;
        r = 0;
        goto _end;
    };

    r = nai_containof(n[0], nsi_sd_service_t, ent);

_end:
    return r;
};


nsi_sd_subscribe_t* nsi_discovery_get_subscribe(
    nsi_discovery_t* p, nsi_sd_service_t* q, nsi_gid_t gid)
{
    nai_list_entry_t* e;
    nsi_sd_subscribe_t* r;


    (void)p;

    e = q->subs.next;
    for ( ; e != &q->subs; e = e->next) {
        r = (nsi_sd_subscribe_t*)e;
        if (r->gid == gid) {
            goto _end;
        };
    };

    nai_errno = ENOENT;
    r = 0;

_end:
    return r;
};


int nsi_discovery_free_servent(
    nsi_discovery_t* p, nsi_sd_servent_t* s)
{
    int r;
    nai_list_entry_t* e;
    nai_list_entry_t* n;
    nsi_sd_request_t* q;
    nsi_sd_request_ref_t* f;


    /* remove all request */
    e = s->list.next;
    for ( ; e != &s->list; ) {
        q = nai_containof(e, nsi_sd_request_t, ent);
        e = e->next;
        nai_list_entry_remove(&q->ent);

        /* remove all client requests */
        n = q->clients.next;
        for ( ; n != &q->clients; ) {
            f = nai_containof(n, nsi_sd_request_ref_t, entq);
            n = n->next;
            nai_list_entry_remove(&f->entq);
            nai_list_entry_remove(&f->entc);
            nsi_discovery_free(p, f);
        };

        nsi_discovery_free(p, q);
    };

    r = 0;

    return r;
};


int nsi_discovery_free_service(
    nsi_discovery_t* p, nsi_sd_service_t* s)
{
    int r;
    nai_list_entry_t* e;
    nsi_sd_subscribe_t* u;


    /* remove all subscribe */
    e = s->subs.next;
    for ( ; e != &s->subs; ) {
        u = nai_containof(e, nsi_sd_subscribe_t, ent);
        e = e->next;
        nai_list_entry_remove(&u->ent);
        nsi_discovery_free_subscribe(p, u);
    };

    /* remove timer */
    nsi_discovery_remove_timer(p, &s->time);

    /* free buf */
    if (s->buf.ptr) {
        nsi_discovery_free(p, s->buf.ptr);
        nai_str_setn(&s->buf);
    };

    /* free service */
    nsi_discovery_free(p, s);
    r = 0;

    return r;
};


int nsi_discovery_free_subscribe(
    nsi_discovery_t* p, nsi_sd_subscribe_t* s)
{
    int r;

    /* remove timer */
    nsi_discovery_remove_timer(p, &s->time);

    /* free array */
    nai_array_close(&s->clients);

    /* free mcast address */
    if (s->mcast.addr) {
        nsi_discovery_free(p, s->mcast.addr);
    };

    /* free subscribe */
    nsi_discovery_free(p, s);
    r = 0;

    return r;
};


static int nsi_discovery_bind_endpoints(nsi_discovery_t* p)
{
    int r;
    int ec;
    nsi_routing_t* rt;
    nsi_endpoint_t* e;
    nsi_endpoint_t* m = 0;
    nsi_endpoint_name_t name;
    nsi_conf_discovery_t* conf;


    rt = p->rt;
    conf = &rt->conf->sd;

    if (conf->enable) {
        /* PRS_SOMEIPSD_00220 */
        e = nsi_routing_create_server(rt, &conf->ucast, 0, 0, 0);
        if (e == 0) {
            nai_log_error(NSI_LOG_CORE, 
                nai_errno, "the discovery open server endpoint failed");
            r = -1;
            goto _end;
        };

        if (conf->mcast.addr) {
            nai_log_info(NSI_LOG_CORE, 0, "bind multicast");

            /* PRS_SOMEIPSD_00600
             * PRS_SOMEIPSD_00603
             */
            p->mcast = nsi_endpoint_set_opt(e, 
                NSI_EOPT_BIND_NAME, (intptr_t)&conf->mcast);
            if (p->mcast == (nsi_connid_t)-1) {
                p->mcast = 0;

                ec = nai_errno;
                nai_log_error(NSI_LOG_CORE, ec, 
                    "the discovery bind multicast send address failed");
                r = -1;
                goto _fail;
            };

            if (conf->enable == 1) {
                name = conf->mcast;

#if 0
                /* cannot bind local address on linux */
                /* get port of multicast */
                si = nai_sockaddr_info(name.addr->sa_family);
                port = nai_sockaddr_get_port(si, name.addr);

                /* make local address */
                name = rt->conf->host;
                nbuf.len = name.len;
                nai_memcpy(&nbuf.addr, name.addr, name.len);
                nai_sockname_setbuf(&name, &nbuf);
                si = nai_sockaddr_info(name.addr->sa_family);
                nai_sockaddr_set_port(si, &nbuf.addr, port);
#endif

                m = nsi_routing_create_server(rt, &name, 0, 0, 0);
            } else {
                /* enable == 2 for local debug */
                m = nsi_routing_create_server(rt, &conf->ucast, 0, 0, 0);
            };
            if (m == 0) {
                ec = nai_errno;
                nai_log_error(NSI_LOG_CORE, 
                    ec, "the discovery open multicast endpoint failed");
                r = -1;
                goto _fail;
            };

            p->mconn = nsi_endpoint_set_opt(m, 
                conf->enable == 1 ? NSI_EOPT_BIND_MCAST : NSI_EOPT_BIND_NAME, 
                (intptr_t)&conf->mcast);
            if (p->mconn == (nsi_connid_t)-1) {
                p->mconn = 0;

                ec = nai_errno;
                nai_log_error(NSI_LOG_CORE, 
                    ec, "the discovery bind multicast address failed");
                r = -1;
                goto _fail;
            };
        };

        p->ep = e;
        p->mp = m;
    };

    r = 0;


_end:
    return r;

_fail:
    if (m != 0) {
        nsi_routing_release_endpoint(m);
    };
    nsi_routing_release_endpoint(e);
    nai_errno = ec;
    goto _end;
};


static int nsi_discovery_unbind_endpoints(nsi_discovery_t* p)
{
    int r;


    if (p->ep) {
        if (p->mcast) {
            nsi_endpoint_set_opt(p->ep, NSI_EOPT_UNBIND_NAME, p->mcast);
            p->mcast = 0;
        };

        nsi_routing_release_endpoint(p->ep);
        p->ep = 0;
    };
    if (p->mp) {
        if (p->mconn) {
            nsi_endpoint_set_opt(p->mp, NSI_EOPT_UNBIND_NAME, p->mconn);
            p->mconn = 0;
        };

        nsi_routing_release_endpoint(p->mp);
        p->mp = 0;
    };

    r = 0;

    return r;
};


int nsi_discovery_init(nsi_discovery_t* p)
{
    int r;


    p->type = 0;
    p->link = 0;
    p->rt = 0;
    p->ep = 0;
    p->mp = 0;
    p->mcast = 0;
    p->mconn = 0;
    p->session = 0;
    p->reboot = 1; /* PRS_SOMEIPSD_00255 */
    p->list = 0;
    p->out = 0;
    p->timing = 0;
    p->timenext = 0;
    p->timeclean = 0;
    p->timedebo[0] = 0;
    p->timedebo[1] = 0;
    p->mss = 1500 - 60; /* mss for ipv6 */
    p->ttl = -1;
    p->init_delay_min = 500;
    p->init_delay_max = 3000;
    p->request_response_delay_min = 5;
    p->request_response_delay_max = 10;
    p->offer_multicast_optim = 0;
    p->repetitions_delay = 100;
    p->repetitions_max = 3;
    p->offer_cyclic_delay = 500;
    p->offer_debounce = 500;
    p->find_debounce = 500;
    p->subscribe_ttl = -1;
    p->subscribe_timeo = 5000;
    p->subscribe_renew = 5;
    p->use_sd_option = 1;
    nai_list_init(&p->initq);
    nai_pool_init(&p->pool, 4096-64);
    nai_sockname_init(&p->name);
    nai_rbtree_init(&p->servs);
    nai_rbtree_init(&p->offer);
    nai_rbtree_init(&p->finds);
    nai_rbtree_init(&p->timeq);
    r = 0;

    return r;
};


int nsi_discovery_open(nsi_discovery_t* p, nsi_routing_t* rt)
{
    int r;
    nsi_endpoint_t* e;
    nsi_endpoint_name_t name;
    nsi_conf_discovery_t* conf;

#if 0
    int port;
    nai_socknbuf_t nbuf;
    const nai_sockaddr_info_t* si;
#endif


    nai_log_info(NSI_LOG_CORE, 0, 
        "the discovery will start with %s mode", 
        rt->type == NSI_RTYPE_ROUTE ? "route" : "proxy");

    if (rt->type == NSI_RTYPE_PROXY) {
        r = nsi_routing_get_localname(rt, &name);
        if (r < 0) {
            nai_log_error(NSI_LOG_CORE, 
                nai_errno, "the discovery get localname failed");
            goto _end;
        };

        e = nsi_routing_create_client(rt, &name, 1, NSI_EFLAG_WAIT, 0);
        if (e == 0) {
            nai_log_error(NSI_LOG_CORE, 
                nai_errno, "the discovery open proxy endpoint failed");
            r = -1;
            goto _end;
        };

        p->ep = e;

        /* use the memroy of configruation */
        p->name = name;

    } else if (rt->conf == 0) {

        nai_errno = EINVAL;
        r = -1;
        goto _end;

    } else if (rt->conf->sd.enable == 0) {

        /* without discovery configure*/

    } else {

        conf = &rt->conf->sd;

        /* use the memroy of configruation */
        p->name = conf->ucast;

        /* set configuration
         * PRS_SOMEIPSD_00400
         */
        p->mss = rt->conf->net.mss;
        p->use_sd_option = rt->conf->cp.use_sd_option;
        p->ttl = conf->ttl;
        p->init_delay_min = conf->init_delay_min;
        p->init_delay_max = conf->init_delay_max;
        p->request_response_delay_min = conf->request_response_delay_min;
        p->request_response_delay_max = conf->request_response_delay_max;
        p->offer_multicast_optim = conf->offer_multicast_optim;
        p->repetitions_delay = conf->repetitions_delay;
        p->repetitions_max = conf->repetitions_max;
        p->offer_cyclic_delay = conf->offer_cyclic_delay;
        p->offer_debounce = conf->offer_debounce;
        p->find_debounce = conf->find_debounce;
        p->subscribe_ttl = conf->subscribe_ttl;
        p->subscribe_timeo = conf->subscribe_timeo;
        p->subscribe_renew = conf->subscribe_renew;
        p->version = conf->version;
    };

    /* set routing */
    p->rt = rt;
    p->type = rt->type;

    /* resume */
    if (rt->link) {
        r = nsi_discovery_resume(p);
        if (r < 0) {
            goto _end;
        };
    };

    r = 0;

_end:
    return r;
};


int nsi_discovery_close(nsi_discovery_t* p)
{
    int r;
    nai_rbnode_t* e;
    nsi_sd_servent_t* f;
    nsi_sd_service_t* s;


    /* free all servents */
    e = nai_rbtree_begin(&p->finds);
    for ( ; e != nai_rbtree_end(&p->finds); ) {
        f = nai_containof(e, nsi_sd_servent_t, ent);
        e = nai_rbtree_next(e);
        nai_rbtree_erase(&p->finds, &f->ent);
        nsi_discovery_free_servent(p, f);
    };

    /* free all offer */
    e = nai_rbtree_begin(&p->offer);
    for ( ; e != nai_rbtree_end(&p->offer); ) {
        s = nai_containof(e, nsi_sd_service_t, ent);
        e = nai_rbtree_next(e);
        nai_rbtree_erase(&p->offer, &s->ent);
        nsi_discovery_free_service(p, s);
    };

    /* output unsubscribes before free subscribe
     * PRS_SOMEIPSD_00432
     */
    if (p->type == NSI_RTYPE_ROUTE && p->ep) {
        nsi_discovery_handle_timeout(p);
        nsi_endpoint_flush(p->ep);
    };

    /* free all services */
    e = nai_rbtree_begin(&p->servs);
    for ( ; e != nai_rbtree_end(&p->servs); ) {
        s = nai_containof(e, nsi_sd_service_t, ent);
        e = nai_rbtree_next(e);
        nai_rbtree_erase(&p->servs, &s->ent);
        nsi_discovery_free_service(p, s);
    };

    /* unbind endpoints */
    nsi_discovery_unbind_endpoints(p);


    /* free */
    nai_pool_close(&p->pool);
    nsi_discovery_init(p);
    r = 0;

    return r;
};


int nsi_discovery_resume(nsi_discovery_t* p)
{
    int r;
    uint32_t msec;
    uint64_t now;


    if (p->link) {
        r = 0;
        goto _end;
    };

    nai_log_info(NSI_LOG_CORE, 0, "discovery resume.");

    /* PRS_SOMEIPSD_00397
     * PRS_SOMEIPSD_00133
     * PRS_SOMEIPSD_00399
     * PRS_SOMEIPSD_00400
     * PRS_SOMEIPSD_00401
     * PRS_SOMEIPSD_00804
     */
    if (p->type == NSI_RTYPE_ROUTE) {
        r = nsi_discovery_bind_endpoints(p);
        if (r < 0) {
            nai_log_error(NSI_LOG_CORE, nai_errno, 
                "the discovery bind endpoints failed");
            goto _end;
        };

        /* set initialize timer */
        msec = p->init_delay_min;
        msec += (int)((float)(
            p->init_delay_max - p->init_delay_min) * nai_random());
        nai_evnode_set_timeout(&p->rt->ev, NAI_TIMEOP_SET, msec);
        now = nai_tickcache_to_msec();
        p->timenext = now + msec;
        p->timedebo[0] = now + msec;
        p->timedebo[1] = now + msec;

        nai_log_info(NSI_LOG_CORE, 0, 
            "the discovery is enabled with a random initial time(%d)", msec);
    };

    p->link = 1;
    r = 0;

_end:
    return r;
};


int nsi_discovery_hangup(nsi_discovery_t* p)
{
    int r;
    nai_rbnode_t* e;
    nsi_client_t* c;
    nsi_sd_service_t* s;
    nsi_sd_timestat_t* t;


    if (p->link == 0) {
        r = 0;
        goto _end;
    };

    nai_log_info(NSI_LOG_CORE, 0, 
        "discovery hangup, remove service info.");

    r = nsi_routing_batch_begin(p->rt);
    if (r < 0) {
        nai_log_alert(NSI_LOG_CORE, 
            nai_errno, "start batch failed");
        goto _end;
    };

    e = nai_rbtree_begin(&p->servs);
    for ( ; e != nai_rbtree_end(&p->servs); ) {
        s = nai_containof(e, nsi_sd_service_t, ent);
        e = nai_rbtree_next(e);

        /* PRS_SOMEIPSD_00450 */
        if (s->ttl && s->own && s->own->remote) {
            s->ttl = 0;
            nsi_discovery_notify_available(p, s, NSI_AVAIL_OFFLINED);
            nai_rbtree_erase(&p->servs, &s->ent);
            nsi_discovery_free_service(p, s);
        };
    };

    /* PRS_SOMEIPSD_00397
     * PRS_SOMEIPSD_00133
     * PRS_SOMEIPSD_00751
     * PRS_SOMEIPSD_00752
     */
    if (p->type == NSI_RTYPE_ROUTE) {

        /* kill timer */
        nai_evnode_set_timeout(&p->rt->ev, NAI_TIMEOP_SET, -1);

        /* reset to init phrase */
        e = nai_rbtree_begin(&p->timeq);
        for ( ; e != nai_rbtree_end(&p->timeq); ) {
            t = nai_containof(e, nsi_sd_timestat_t, node);
            e = nai_rbtree_next(e);

            switch (t->type) {
            case NSI_SD_TYPE_OFFER:
            case NSI_SD_TYPE_FIND:
                nsi_discovery_remove_timer(p, t);
                nsi_discovery_add_timer(p, t, t->type, NSI_SD_STAT_INIT, 0);
                break;
            default:
                assert(0);
                break;
            };
        };

        /* PRS_SOMEIPSD_00450
         * PRS_SOMEIPSD_00457
         */
        /* reset session */
        p->session = 0;
        p->reboot = 1;

        /* reset session of clients */
        e = nai_rbtree_begin(&p->rt->clients);
        for ( ; e != nai_rbtree_end(&p->rt->clients); ) {
            c = nai_containof(e, nsi_client_t, entr);
            e = nai_rbtree_next(e);
            c->ep.session = 0;
            c->reboot = 1;
        };

        nsi_discovery_unbind_endpoints(p);
    };

    p->link = 0;
    r = 0;

    nsi_routing_batch_end(p->rt);


_end:
    return r;
};


int nsi_discovery_offer_service(
    nsi_discovery_t* p, 
    nsi_client_t* c, nsi_serv_t serv, nsi_inst_t inst, 
    const nsi_service_info_t* info)
{
    int r;
    nai_rbnode_t** n;
    nai_rbnode_t* parent;
    nsi_sd_service_t* s;


    /* do check before offer */
    r = nsi_routing_grant_offer(
        p->rt, c, serv, inst, info->major, info->minor);
    if (r < 0) {
        nai_log_debug(NSI_LOG_CORE, nai_errno, 
            "the client(0x%x) offer service(%d, %d) is denied", 
            c->cid, serv, inst);
        goto _end;
    };

    /* PRS_SOMEIPSD_00355
     * PRS_SOMEIPSD_00356
     */

    n = nsi_discovery_find_service(&p->offer, serv, inst, &parent);
    if (n[0] != 0) {
        nai_errno = EEXIST;
        r = -1;
        goto _end;
    };

    s = nsi_discovery_alloc(p, sizeof(*s));
    if (s == 0) {
        nai_log_alert(NSI_LOG_CORE, nai_errno, 
            "the discovery failed to allocate memory of service(%d, %d)", 
            serv, inst);
        r = -1;
        goto _end;
    };

    s->serv = serv;
    s->inst = inst;
    s->major = info->major;
    s->minor = info->minor;
    s->ttl = info->ttl ? info->ttl : p->ttl;
    s->cyclic_delay = 
        info->cyclic_delay ? info->cyclic_delay : p->offer_cyclic_delay;
    s->own = 0;
    s->time.flags = 0;
    s->names[0] = info->names[0]; /* here, caller don't free memory */
    s->names[1] = info->names[1]; /* to ensure that stopping the service 
                                   * before the configuration is modified 
                                   * will not cause problems.
                                   */
    nai_str_setn(&s->buf);
    nai_list_init(&s->subs);
    nai_rbtree_link(&p->offer, &s->ent, parent, n);
    nai_rbtree_color(&p->offer, &s->ent);

    /* add timer */
    if (p->type == NSI_RTYPE_ROUTE) {
        nsi_discovery_add_timer(p, 
            &s->time, NSI_SD_TYPE_OFFER, NSI_SD_STAT_INIT, 0);
    } else {
        nsi_discovery_add_timer(p, 
            &s->time, NSI_SD_TYPE_OFFER, NSI_SD_STAT_MAIN, -1);
    };

    /* notify */
    nsi_discovery_notify_available(p, s, NSI_AVAIL_ONLINED);
    r = 0;

_end:
    return r;
};


int nsi_discovery_stop_service(
    nsi_discovery_t* p, 
    nsi_client_t* c, nsi_serv_t serv, nsi_inst_t inst)
{
    int r;
    nsi_sd_service_t* s;
    nai_list_entry_t send;


    (void)c;

    s = nsi_discovery_get_service(p, &p->offer, serv, inst);
    if (s == 0) {
        nai_errno = ENOENT;
        r = -1;
        goto _end;
    };

    if (p->type == NSI_RTYPE_ROUTE && 
        s->time.stat != NSI_SD_STAT_INIT) {
        /* send stop offer now
         * PRS_SOMEIPSD_00356
         * PRS_SOMEIPSD_00364
         * PRS_SOMEIPSD_00427
         */
        s->ttl = 0;
        nsi_discovery_remove_timer(p, &s->time);
        if (p->ep) {
            nai_list_init(&send);
            nai_list_insert_tail(&send, &s->time.ent);
            nsi_discovery_send(p, &send, 0);
        };
    };

    /* */
    nai_rbtree_erase(&p->offer, &s->ent);
    nsi_discovery_notify_available(p, s, NSI_AVAIL_OFFLINED);
    nsi_discovery_free_service(p, s);
    r = 0;

_end:
    return r;
};


int nsi_discovery_request_service(
    nsi_discovery_t* p, 
    nsi_client_t* c, nsi_serv_t serv, nsi_inst_t inst, 
    nsi_version_t major, nsi_versmin_t minor)
{
    int r;
    int ec;
    int init;
    int exist;
    nai_rbnode_t** n;
    nai_list_entry_t* e;
    nsi_sd_servent_t* s;
    nsi_sd_request_t* q;
    nsi_sd_request_ref_t* f;
    nsi_endpoint_conn_t conn;


    /* do check before find */
    if (c != 0) {
        r = nsi_routing_grant_find(
            p->rt, c, serv, inst, major, minor);
        if (r < 0) {
            nai_log_debug(NSI_LOG_CORE, nai_errno, 
                "the client(0x%x) request service(%d, %d) is denied", 
                c->cid, serv, inst);
            goto _end;
        };
    };


    s = nsi_discovery_get_servent(p, serv, 0);
    if (s == 0) {
        r = -1;
        goto _end;
    };

    /* PRS_SOMEIPSD_00351 */
    init = 0;
    q = nsi_discovery_get_request(p, s, inst, major, minor);
    if (q == 0) {
        q = (nsi_sd_request_t*)nsi_discovery_alloc(p, sizeof(*q));
        if (q == 0) {
            nai_log_alert(NSI_LOG_CORE, nai_errno, 
                "the discovery failed to allocate memory of request(%d, %d)", 
                serv, inst);
            r = -1;
            goto _end;
        };

        init = 1;
        q->serv = serv;
        q->inst = inst;
        q->major = major;
        q->minor = minor;
        q->request = 0;
        q->time.flags = 0;
        q->time.type = NSI_SD_TYPE_FIND;
        q->time.stat = NSI_SD_STAT_MAIN;
        q->time.queue = 0;
        nai_list_init(&q->clients);
        nai_list_init(&q->time.ent);
        nai_list_insert_tail(&s->list, &q->ent);
    };

    if (c == 0) {
        if (q->request) {
            r = 0;
            goto _end;
        };

        q->request = 1;
    } else {

        e = q->clients.next;
        for ( ; e != &q->clients; e = e->next) {
            f = nai_containof(e, nsi_sd_request_ref_t, entq);
            if (f->client == c) {
                r = 0;
                goto _end;
            };
        };

        f = (nsi_sd_request_ref_t*)nsi_discovery_alloc(p, sizeof(*f));
        if (f == 0) {
            nai_log_alert(NSI_LOG_CORE, nai_errno, 
                "the discovery failed to add request(%d, %d) client(0x%x)", 
                serv, inst, c->cid);

            if (init) {
                ec = nai_errno;
                nsi_discovery_free(p, q);
                nai_errno = ec;
            };

            r = -1;
            goto _end;
        };

        /* add client */
        f->client = c;
        f->request = q;
        nai_list_insert_tail(&q->clients, &f->entq);
        nai_list_insert_tail(&c->reqs, &f->entc);
    };

    if (serv == NSI_SERVICE_ANY) {
        ; /* nothing to do */

    } else if (p->type == NSI_RTYPE_PROXY) {
        if (c != 0) {
            conn.ep = p->ep;
            conn.conn = 0;
            r = nsi_proxy_request(&conn, 
                NSI_SERVICE_OP_ADD, 
                0, c, serv, inst, major, minor);
            if (r < 0) {
                nai_log_error(NSI_LOG_CORE, nai_errno, 
                    "the discovery send request(%d, %d) to proxy failed", 
                    serv, inst);

                /* ignore */
            };
        };

    } else if (init) {

        /* start timer */
        exist = 0;
        if (inst != NSI_INSTANCE_ANY) {
            n = nsi_discovery_find_service(&p->servs, serv, inst, 0);
            if (n[0] != 0) {
                /* already exists
                 * PRS_SOMEIPSD_00350
                 */
                exist = 1;
            };
        };

        if (exist) {
            nsi_discovery_add_timer(p, 
                &q->time, NSI_SD_TYPE_FIND, NSI_SD_STAT_MAIN, -1);
        } else {
            nsi_discovery_add_timer(p, 
                &q->time, NSI_SD_TYPE_FIND, NSI_SD_STAT_INIT, 0);
        };

    };

    /* do notify */
    if (c != 0) {
        nsi_discovery_notify_exist(p, c->cid, serv, inst, major, minor);
    };

    r = 0;

_end:
    return r;
};


int nsi_discovery_release_service(
    nsi_discovery_t* p, 
    nsi_client_t* c, nsi_serv_t serv, nsi_inst_t inst, 
    nsi_version_t major, nsi_versmin_t minor)
{
    int r;
    nai_list_entry_t* e;
    nsi_sd_servent_t* s;
    nsi_sd_request_t* q;
    nsi_sd_request_ref_t* f;
    nsi_endpoint_conn_t conn;


    s = nsi_discovery_get_servent(p, serv, 1);
    if (s == 0) {
        r = 0;
        goto _end;
    };

    q = nsi_discovery_get_request(p, s, inst, major, minor);
    if (q == 0) {
        r = 0;
        goto _end;
    };

    if (c == 0) {
        if (q->request == 0) {
            r = 0;
            goto _end;
        };

        q->request = 0;

    } else {

        e = q->clients.next;
        for ( ; e != &q->clients; e = e->next) {
            f = nai_containof(e, nsi_sd_request_ref_t, entq);
            if (f->client == c) {
                break;
            };
        };
        if (e == &q->clients) {
            r = 0;
            goto _end;
        };

        nai_list_entry_remove(&f->entq);
        nai_list_entry_remove(&f->entc);
        nsi_discovery_free(p, f);


        if (p->type == NSI_RTYPE_PROXY && serv != NSI_SERVICE_ANY) {
            conn.ep = p->ep;
            conn.conn = 0;
            r = nsi_proxy_request(&conn, 
                NSI_SERVICE_OP_REMOVE, 
                0, c, serv, inst, major, minor);
            if (r < 0) {
                nai_log_error(NSI_LOG_CORE, nai_errno, 
                    "the discovery send release(%d, %d) to proxy failed", 
                    serv, inst);

                ;/* ignore */
            };
        };
    };

    if (q->request == 0 && nai_list_is_empty(&q->clients)) {
        /* remove it */
        nai_list_entry_remove(&q->ent);
        nsi_discovery_remove_timer(p, &q->time);
        nsi_discovery_free(p, q);
    };

    r = 0;

_end:
    return r;
};


int nsi_discovery_subscribe(
    nsi_discovery_t* p, 
    nsi_client_t* c, nsi_serv_t serv, nsi_inst_t inst, 
    nsi_gid_t gid, nsi_version_t major, 
    const nsi_endpoint_name_t names[2])
{
    int r;
    int n;
    int ec;
    int len;
    char* buf;
    nsi_sd_service_t* s;
    nsi_sd_subscribe_t* u;
    nsi_cid_t cid;
    nsi_cid_t* q;
    nsi_group_info_t gi;


    (void)major;

    if (p->type != NSI_RTYPE_ROUTE) {
        nai_errno = EPERM;
        r = -1;
        goto _end;
    };


    cid = c->cid;

    s = nsi_discovery_get_service(p, &p->servs, serv, inst);
    if (s == 0) {
        nai_errno = ENOENT;
        r = -1;
        goto _end;
    };

    if (s->ttl == 0) {
        nai_errno = ENOENT;
        r = -1;
        goto _end;
    };


    u = nsi_discovery_get_subscribe(p, s, gid);
    if (u && 
        u->time.stat == NSI_SD_STAT_REMOVE) {

        /* the subscription will be unsubscribe and removed, 
         * force immediate processing
         */
        nsi_discovery_handle_timeout(p);

        /* get again, the subscription should be non-exists */
        u = nsi_discovery_get_subscribe(p, s, gid);
        assert(u == 0);
    };

    if (u == 0) {
        len = 0;
        if (names) {
            len = names[0].len + names[1].len;
        };

        u = (nsi_sd_subscribe_t*)nsi_discovery_alloc(p, sizeof(*u) + len);
        if (u == 0) {
            nai_log_alert(NSI_LOG_CORE, nai_errno, 
                "the discovery failed to "
                "allocate memory of subscribe(%d, %d, %d)", 
                serv, inst, gid);
            r = -1;
            goto _end;
        };

        r = nsi_routing_get_group_info(p->rt, serv, inst, gid, &gi);
        if (r < 0) {
            gi.ttl = 0;
            gi.subscribe_timeo = -1;
            gi.subscribe_renew = -1;
        };

        if (gi.ttl == 0) {
            gi.ttl = p->subscribe_ttl;
        };
        if (gi.subscribe_timeo == (uint32_t)-1) {
            gi.subscribe_timeo = p->subscribe_timeo;
        };
        if (gi.subscribe_renew == (uint32_t)-1) {
            gi.subscribe_renew = p->subscribe_renew;
        };

        /* PRS_SOMEIPSD_00703 */
        nai_array_init(&u->clients, sizeof(*q), 0);
        u->service = s;
        u->serv = serv;
        u->inst = inst;
        u->cid = cid;
        u->gid = gid;
        u->major = s->major;
        u->init = 0; /* control send stop before next request */
        u->ttl = gi.ttl;
        u->subscribe_timeo = gi.subscribe_timeo;
        u->subscribe_renew = gi.subscribe_renew;
        u->ack = 0;
        u->mcast.addr = 0;
        u->mcast.len = 0;

        /* copy names */
        buf = (char*)(u + 1);
        for (n = 0; n < 2; n ++) {
            if (names && names[n].addr) {
                u->names[n].addr = (nai_sockaddr_t*)buf;
                u->names[n].len = names[n].len;
                nai_memcpy(u->names[n].addr, names[n].addr, names[n].len);
                buf += names[n].len;
            } else {
                u->names[n].addr = 0;
                u->names[n].len = 0;
            };
        };

        u->time.flags = 0;
        nsi_discovery_add_timer(p, &u->time, 
            NSI_SD_TYPE_SUBSCRIBE, NSI_SD_STAT_WAITING, -1);

        q = nai_array_push(&u->clients);
        if (q == 0) {
            nai_log_alert(NSI_LOG_CORE, nai_errno, 
                "the discovery failed to"
                "add subscription(%d, %d, %d) client(0x%x)", 
                serv, inst, gid, cid);
            goto _fail;
        };

        q[0] = cid;


        nai_list_insert_tail(&s->subs, &u->ent);
        nsi_discovery_set_timer(p, &u->time, 0);

    } else {

        /* find */
        q = (nsi_cid_t*)u->clients.elts;
        for (n = 0; n < (intptr_t)u->clients.count; n ++) {
            if (q[n] == cid) {
                break;
            };
        };

        /* add client */
        if (n >= (intptr_t)u->clients.count) {
            q = nai_array_push(&u->clients);
            if (c == 0) {
                nai_log_alert(NSI_LOG_CORE, nai_errno, 
                    "the discovery failed to"
                    "add subscription(%d, %d, %d) client(0x%x)", 
                    serv, inst, gid, cid);
                r = -1;
                goto _end;
            };

            q[0] = cid;
        };


        /* update stat */
        switch (u->time.stat) {
        case NSI_SD_STAT_ACKED:
        case NSI_SD_STAT_ACKED_RENEW:
            /* do notify */
            if (u->ack) {
                nsi_discovery_notify_ack(p, u, cid);
                break;
            };

            /* fallthrough */

        case NSI_SD_STAT_REMOVE:
            u->init = 0;
            u->major = s->major;
            u->time.stat = NSI_SD_STAT_WAITING;
            nsi_discovery_set_timer(p, &u->time, 0);
            break;

        default:
            break;
        };
    };

    r = 0;

_end:
    return r;

_fail:
    ec = nai_errno;
    nsi_discovery_free_subscribe(p, u);
    nai_errno = ec;
    r = -1;
    goto _end;
};


int nsi_discovery_unsubscribe(
    nsi_discovery_t* p, 
    nsi_client_t* c, nsi_serv_t serv, nsi_inst_t inst, 
    nsi_gid_t gid, nsi_version_t major)
{
    int r;
    int n;
    nsi_sd_service_t* s;
    nsi_sd_subscribe_t* u;
    nsi_cid_t cid;
    nsi_cid_t* q;


    (void)major;

    if (p->type != NSI_RTYPE_ROUTE) {
        nai_errno = EPERM;
        r = -1;
        goto _end;
    };

    cid = c->cid;

    s = nsi_discovery_get_service(p, &p->servs, serv, inst);
    if (s == 0) {
        nai_errno = ENOENT;
        r = -1;
        goto _end;
    };


    u = nsi_discovery_get_subscribe(p, s, gid);
    if (u == 0) {
        nai_errno = ENOENT;
        r = -1;
        goto _end;
    };


    /* find */
    q = (nsi_cid_t*)u->clients.elts;
    for (n = 0; n < (intptr_t)u->clients.count; n ++) {
        if (q[n] == cid) {
            break;
        };
    };
    if (n >= (intptr_t)u->clients.count) {
        nai_errno = ENOENT;
        r = -1;
        goto _end;
    };

    /* remove the client */
    q[n] = q[u->clients.count - 1];
    u->clients.count --;
    if (u->clients.count > 0) {
        r = 0;
        goto _end;
    };


    /* update stat */
    switch (u->time.stat) {
    case NSI_SD_STAT_WAITING:
        nai_list_entry_remove(&u->ent);
        nsi_discovery_free_subscribe(p, u);
        break;
    case NSI_SD_STAT_PENDING:
        u->time.stat = NSI_SD_STAT_REMOVE;
        nsi_discovery_set_timer(p, &u->time, 0);
        break;
    case NSI_SD_STAT_ACKED:
    case NSI_SD_STAT_ACKED_RENEW:
        if (u->ack) {
            u->time.stat = NSI_SD_STAT_REMOVE;
            nsi_discovery_set_timer(p, &u->time, 0);
        } else {
            nai_list_entry_remove(&u->ent);
            nsi_discovery_free_subscribe(p, u);
        };
        break;
    default:
        break;
    };

    r = 0;

_end:
    return r;
};


int nsi_discovery_subscribe_ack(
    nsi_discovery_t* p, int sult, 
    nsi_cid_t cid, nsi_serv_t serv, nsi_inst_t inst, 
    nsi_gid_t gid, nsi_version_t major, uint32_t ttl, int init)
{
    int r;
    nsi_client_t* c;
    nsi_group_info_t gi;
    nsi_sd_subscribe_t sub;
    nsi_sd_subscribe_t* u;
    nai_list_entry_t list;


    assert(sult == 0 || ttl == 0);


    if (p->type != NSI_RTYPE_ROUTE) {
        nai_errno = EPERM;
        r = -1;
        goto _end;
    };


    c = nsi_client_find_in_routing(p->rt, cid);
    if (c == 0) {
        r = 0;
        goto _end;
    };

    if (c->remote == 0) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    if (p->out != c) {
        u = &sub;
    } else {
        u = (nsi_sd_subscribe_t*)nai_palloc(&p->pool, sizeof(*u));
        if (u == 0) {
            nai_log_alert(NSI_LOG_CORE, nai_errno, 
                "the discovery failed to "
                "allocate memory of subscription(%d, %d, %d) ack", 
                serv, inst, gid);
            r = -1;
            goto _end;
        };
    };

    /* fill ack */
    u->cid = 0;
    u->serv = serv;
    u->inst = inst;
    u->gid = gid;
    u->major = major;
    u->init = !!init;
    u->ttl = ttl;
    u->ack = ttl;
    nai_sockname_init(&u->mcast);
    nai_sockname_init(&u->names[0]);
    nai_sockname_init(&u->names[1]);
    nai_array_init(&u->clients, sizeof(nsi_cid_t), 0);

    /* get the multicast address of group */
    r = nsi_routing_get_group_info(p->rt, serv, inst, gid, &gi);
    if (r >= 0) {
        /* here, the memory 
         * is only used in nsi_discovery_handle_message, 
         * does not cause wild pointer access.
         */
        u->names[0] = gi.mcast;
    };

    /* send ack */
    u->time.flags = 0;
    u->time.queue = 0;
    u->time.type = NSI_SD_TYPE_ACK;
    u->time.stat = NSI_SD_STAT_WAITING;
    if (p->out == c) {
        nai_list_insert_tail(p->list, &u->time.ent);
    } else {
        nai_list_init(&list);
        nai_list_insert_tail(&list, &u->time.ent);
        if (p->ep) {
            r = nsi_discovery_send(p, &list, c);
            if (r < 0) {
                nai_log_error(NAI_LOG_CORE, nai_errno, 
                    "the discovery failed to "
                    "send subscription(%d, %d, %d) ack(%d) to client(0x%x)", 
                    serv, inst, gid, sult, cid);
            };
        };
    };

    r = 0;

_end:
    return r;
};


int nsi_discovery_subscribe_cancel(
    nsi_discovery_t* p, nsi_serv_t serv, nsi_inst_t inst)
{
    int r;
    nai_list_entry_t* e;
    nsi_sd_service_t* s;
    nsi_sd_subscribe_t* u;


    nai_log_debug(NSI_LOG_CORE, 0, 
        "cancel subscriptions of service(%d, %d).", 
        serv, inst);

    if (p->type != NSI_RTYPE_ROUTE) {
        nai_errno = EPERM;
        r = -1;
        goto _end;
    };


    s = nsi_discovery_get_service(p, &p->servs, serv, inst);
    if (s == 0) {
        nai_errno = ENOENT;
        r = -1;
        goto _end;
    };


    e = s->subs.next;
    for ( ; e != &s->subs; ) {
        u = (nsi_sd_subscribe_t*)e;
        e = e->next;

        nai_list_entry_remove(&u->ent);
        nsi_discovery_free_subscribe(p, u);
    };

    r = 0;

_end:
    return r;
};



int nsi_discovery_get_service_info(nsi_discovery_t* p, 
    nsi_serv_t serv, nsi_inst_t inst, nsi_service_info_t* info)
{
    int r;
    nsi_sd_service_t* s;


    s = nsi_discovery_get_service(p, &p->servs, serv, inst);
    if (s == 0) {
        nai_errno = ENOENT;
        r = -1;
        goto _end;
    };

    if (s->ttl == 0) {
        nai_errno = ENOENT;
        r = -1;
        goto _end;
    };

    if (info) {
        info->local = s->local;
        info->owner = 0;
        info->ttl = s->ttl;
        info->cyclic_delay = s->cyclic_delay;
        info->serv = serv;
        info->inst = inst;
        info->major = s->major;
        info->minor = s->minor;
        info->names[0] = s->names[0];
        info->names[1] = s->names[1];
        info->tls.tls_version = 0;
        nai_str_setn(&info->tls.tls_peer_ca);
        nai_str_setn(&info->tls.tls_local_cert);
        nai_str_setn(&info->tls.tls_local_privkey);
    };
    r = 0;

_end:
    return r;
};


int nsi_discovery_set_service_info(nsi_discovery_t* p, 
    const nsi_service_info_t* si, nsi_client_t* c)
{
    int r;
    int i;
    int changed;
    char* buf;
    uint32_t len;
    uint32_t ttl;
    uint64_t msec;
    nai_rbnode_t** n;
    nai_rbnode_t* parent;
    nsi_serv_t serv;
    nsi_inst_t inst;
    nsi_sd_service_t* s;
    const nsi_endpoint_name_t* names;


    ttl = si->ttl;
    serv = si->serv;
    inst = si->inst;
    nai_log_debug(NSI_LOG_CORE, 0, 
        "the discovery add service(%d, %d) info: major %d, minor %d, ttl %d", 
        serv, inst, si->major, si->minor, ttl);


    n = nsi_discovery_find_service(&p->servs, serv, inst, &parent);
    if (n[0] != 0) {
        s = nai_containof(n[0], nsi_sd_service_t, ent);
    } else {
        /* First stop offer service may impact some find service */
#if 0
        if (ttl == 0) {
            r = 0;
            goto _end;
        };
#endif
        /* PRS_SOMEIPSD_00826 */
        if (inst == NSI_INSTANCE_ANY) {
            nai_log_warn(NSI_LOG_CORE, 0, 
                "the discovered remote service(%d, %d) "
                "with an invalid instance id", 
                serv, inst);
            r = 0;
            goto _end;
        };

        if (p->type == NSI_RTYPE_ROUTE) {
            r = nsi_routing_is_local_service(p->rt, serv, inst);
            if (r > 0) {
                nai_log_warn(NSI_LOG_CORE, 0, 
                    "the discovered remote service(%d, %d) "
                    "conflicts with the local service", 
                    serv, inst);
                r = 0;
                goto _end;
            };
        };

        s = (nsi_sd_service_t*)nsi_discovery_alloc(p, sizeof(*s));
        if (s == 0) {
            nai_log_alert(NSI_LOG_CORE, nai_errno, 
                "the discovery faield to allocate memory of service(%d, %d)", 
                serv, inst);
            r = -1;
            goto _end;
        };

        s->serv = serv;
        s->inst = inst;
        s->major = NSI_MAJOR_ANY;
        s->minor = NSI_MINOR_ANY;
        s->local = 0;
        s->ttl = 0;
        s->cyclic_delay = 0;
        s->own = 0;
        s->last_offering = 0;
        nai_str_setn(&s->buf);
        nai_list_init(&s->subs);
        nai_sockname_init(&s->names[0]);
        nai_sockname_init(&s->names[1]);
        s->time.flags = 0;
        nsi_discovery_add_timer(p, 
            &s->time, NSI_SD_TYPE_SERVICE, NSI_SD_STAT_MAIN, -1);

        nai_rbtree_link(&p->servs, &s->ent, parent, n);
        nai_rbtree_color(&p->servs, &s->ent);
    };


    changed = 0;

    if (ttl == 0) {
        c = 0;

        /* PRS_SOMEIPSD_00827 */
        if ((s->ttl !=0) && (si->major != s->major || 
            si->minor != s->minor)) {
            nai_log_warn(NSI_LOG_CORE, 0, 
                "the versions(%d, %d) of stoped service(%d, %d) "
                "conflicts with the initial values(%d, %d)", 
                si->major, si->minor, 
                serv, inst, s->major, s->minor);

            r = 0;
            goto _end;
        };

    } else {

        /* PRS_SOMEIPSD_00826 */
        if (si->major == NSI_MAJOR_ANY || 
            si->minor == NSI_MINOR_ANY) {
            nai_log_warn(NSI_LOG_CORE, 0, 
                "the discovered remote service(%d, %d) "
                "with an invalid version(%d, %d)", 
                serv, inst, si->major, si->minor);
            r = 0;
            goto _end;
        };

        /* update address */
        len = 0;
        names = si->names;
        for (i = 0; i < 2; i ++) {
            if (names[i].addr) {
                len += names[i].len;
                if (s->names[i].addr && 
                    nsi_endpoint_name_equal(&names[i], &s->names[i])) {
                    continue;
                };
            } else {
                if (s->names[i].addr == 0) {
                    continue;
                };
            };
            changed = 1;
        };
        if (changed) {
            if (s->buf.len >= len) {
                buf = s->buf.ptr;
            } else {
                buf = (char*)nsi_discovery_alloc(p, len);
                if (buf == 0) {
                    nai_log_alert(NSI_LOG_CORE, nai_errno, 
                        "the discovery faield to "
                        "allocate memory of service(%d, %d) names", 
                        serv, inst);
                    r = -1;
                    goto _end;
                };
            };
            len = 0;
            for (i = 0; i < 2; i ++) {
                if (names[i].addr) {
                    nai_memcpy(buf + len, names[i].addr, names[i].len);
                    s->names[i].addr = (nai_sockaddr_t*)(buf+len);
                    s->names[i].len = names[i].len;
                    len += names[i].len;
                } else {
                    s->names[i].addr = 0;
                    s->names[i].len = 0;
                };
            };
            if (s->buf.ptr != buf) {
                if (s->buf.ptr) {
                    nsi_discovery_free(p, s->buf.ptr);
                };
                s->buf.ptr = buf;
                s->buf.len = len;
            };
        };

        /* update version */
        if (s->major != si->major) {
            s->major = si->major;
            changed = 1;
        };
        if (s->minor != si->minor) {
            s->minor = si->minor;
            changed = 1;
        };
    };

    /* update own */
    if (s->own != c) {
        if (s->own) {
            s->own->offers --;
        };
        if (c != 0) {
            c->offers ++;
        };
        s->own = c;
    };

    /* update ttl */
    if (ttl == 0) {
        /* PRS_SOMEIPSD_00363
         * PRS_SOMEIPSD_00364
         */

        /* become offlined */    
        r = NSI_AVAIL_OFFLINED;
        if (s->ttl != 0) {
            s->ttl = ttl;
            nsi_discovery_remove_timer(p, &s->time);
        };
        
    } else {
        if (s->ttl == 0) {
            r = NSI_AVAIL_ONLINED; /* become onlined */
        } else {
            r = changed ? NSI_AVAIL_CHANGED : 3;
        };
        s->ttl = ttl;

        if (ttl == (uint32_t)-1) {
            msec = (uint64_t)-1;
        } else {
            msec = (uint64_t)ttl* 1000;
        };
        nsi_discovery_set_timer(p, &s->time, msec);
    };

    s->local = si->local;

    /* do notify */
    switch (r) {
    case NSI_AVAIL_OFFLINED: /* become offlined */
    case NSI_AVAIL_ONLINED: /* become online */
    case NSI_AVAIL_CHANGED: /* changed */
        nai_log_info(NSI_LOG_CORE, 0, 
            "the remote service(%d, %d) is changed and available(%d)", 
            serv, inst, r);

        r = nsi_discovery_notify_available(p, s, r);
        break;
    case 3: /* no changed */
        nai_log_debug(NSI_LOG_CORE, 0, 
            "the remote service(%d, %d) without change", serv, inst);
        break;
    default:
        assert(0);
        break;
    };

    r = 0;

_end:
    return r;
};


int nsi_discovery_add_timer(
    nsi_discovery_t* p, 
    nsi_sd_timestat_t* t, short type, short stat, uint64_t msec)
{
    int r;


    t->type = type;
    t->stat = stat;
    if (stat == NSI_SD_STAT_INIT) {
        t->queue = 0;
        t->expire = 0;
        nai_list_insert_tail(&p->initq, &t->ent);
        r = 0;
    } else {
        t->queue = 0;
        nai_list_init(&t->ent);
        r = nsi_discovery_set_timer(p, t, msec);
    };

    return r;
};


int nsi_discovery_set_timer(
    nsi_discovery_t* p, nsi_sd_timestat_t* t, uint64_t msec)
{
    int r;
    uint64_t expire;
    nai_rbnode_t** n;
    nai_rbnode_t* parent;
    nsi_sd_timestat_t* e;


    /* remove first, but not for transient nsi_sd_timestat_t */
    if (!t->flags) {
        if (t->queue == 0) {
            nai_list_entry_remove(&t->ent);
        } else {
            nai_rbtree_erase(&p->timeq, &t->node);
        };
    }

    /* add */
    switch (msec) {
    case -1:
        t->queue = 0;
        t->expire = -1;
        nai_list_init(&t->ent);
        break;
    default:
        expire = nai_tickcache_to_msec() + msec;
        n = &nai_rbtree_root(&p->timeq);
        parent = nai_rbtree_end(&p->timeq);

        while (*n) {
            parent = *n;
            e = (nsi_sd_timestat_t*)parent;
            if (expire < e->expire) {
                n = &parent->rb_left;
            } else {
                n = &parent->rb_right;
            };
        };

        t->queue = 1;
        t->expire = expire;
        nai_rbtree_link(&p->timeq, &t->node, parent, n);
        nai_rbtree_color(&p->timeq, &t->node);

        if (p->timenext > expire) {
            p->timenext = expire;
            if (!p->timing) {
                nai_evnode_set_timeout(
                    &p->rt->ev, NAI_TIMEOP_SET, (int)msec);
            };
        };
        break;
    };

    r = 0;

    return r;
};


int nsi_discovery_remove_timer(
    nsi_discovery_t* p, nsi_sd_timestat_t* t)
{
    int r;

    if (t->queue == 0) {
        nai_list_entry_remove(&t->ent);
    } else {
        nai_rbtree_erase(&p->timeq, &t->node);
    };

    nai_list_init(&t->ent);
    t->queue = 0;
    r = 0;

    return r;
};


int nsi_discovery_notify_ack(
    nsi_discovery_t* p, nsi_sd_subscribe_t* u, nsi_cid_t cid)
{
    int r;
    int n;
    int sult;
    nsi_cid_t* a;
    nsi_client_t* c;
    nsi_service_t* s;


    s = nsi_service_open(p->rt, u->serv, u->inst, 1);
    if (s == 0) {
        r = 0;
        goto _end;
    };

    /* PRS_SOMEIPSD_00390 */
    sult = u->ack ? NSI_E_OK : NSI_E_NOT_OK;
    if (cid != 0) {
        /* notify one */
        c = nsi_client_find_in_routing(p->rt, cid);
        if (c != 0) {
            r = nsi_service_subscribe_ack(s, 
                c, sult, u->gid, u->major, NSI_EVENT_ANY, u->ack, &u->mcast);
            if (r < 0) {
                nai_log_error(NAI_LOG_CORE, nai_errno, 
                    "the discovery failed to "
                    "forward subscription(%d, %d, %d) ack(%d) to client(0x%x)", 
                    u->serv, u->inst, u->gid, sult, cid);
                goto _end;
            };
        };

        r = 0;

    } else {
        r = nsi_routing_batch_begin(p->rt);
        if (r < 0) {
            nai_log_alert(NSI_LOG_CORE, 
                nai_errno, "start batch failed");
            goto _end;
        };

        /* notify each client */
        a = (nsi_cid_t*)u->clients.elts;
        for (n = 0; n < (intptr_t)u->clients.count; n ++) {
            c = nsi_client_find_in_routing(p->rt, a[n]);
            if (c == 0) {
                continue;
            };

            r = nsi_service_subscribe_ack(s, 
                c, sult, u->gid, u->major, NSI_EVENT_ANY, u->ack, &u->mcast);
            if (r < 0) {
                nai_log_error(NAI_LOG_CORE, nai_errno, 
                    "the discovery failed to "
                    "forward subscription(%d, %d, %d) ack(%d) to client(0x%x)", 
                    u->serv, u->inst, u->gid, sult, c->cid);

                /* fixme: error ignored */
            };
        };


        if (u->ack == 0) {
            nai_list_entry_remove(&u->ent);
            nsi_discovery_free_subscribe(p, u);
        };

        nsi_routing_batch_end(p->rt);
        r = 0;
    };


_end:
    return r;
};


int nsi_discovery_notify_available(
    nsi_discovery_t* p, nsi_sd_service_t* se, int avail)
{
    int r;
    int m;
    int ec;
    int stat;
    nai_list_entry_t* ent;
    nai_list_entry_t* nod;
    nsi_client_t* c;
    nsi_service_t* s;
    nsi_sd_servent_t* e;
    nsi_sd_request_t* q;
    nsi_sd_request_ref_t* f;
    nsi_sd_subscribe_t* u;


    nai_log_info(NSI_LOG_CORE, 0, 
        "the discovery notify service(%d, %d) is available(%d)", 
        se->serv, se->inst, avail);


    r = nsi_routing_batch_begin(p->rt);
    if (r < 0) {
        nai_log_alert(NSI_LOG_CORE, 
            nai_errno, "start batch failed");
        goto _end;
    };


    if (avail != NSI_AVAIL_ONLINED) {
        ent = se->subs.next;
        for ( ; ent != &se->subs; ) {
            u = nai_containof(ent, nsi_sd_subscribe_t, ent);
            ent = ent->next;

            /* service is changed or offlined, 
             * notify nack to clients and remove subscription.
             */
            stat = u->time.stat;
            switch (stat) {
            case NSI_SD_STAT_ACKED:
            case NSI_SD_STAT_ACKED_RENEW:
                /* acked, notify and remove it */
                if (u->ack) {
                    u->ack = 0;
                    r = nsi_discovery_notify_ack(p, u, 0);
                    if (r < 0) {
                        nai_log_error(NSI_LOG_CORE, nai_errno, 
                            "the discovery failed to nack to clients");
                    };
                    break;
                };

                /* fallthrough */

            default:
                /* not acked, remove it */
                nai_list_entry_remove(&u->ent);
                nsi_discovery_free_subscribe(p, u);
                break;
            };
        };
    };

    /* notify serivce */
    s = nsi_service_find_in_routing(p->rt, se->serv, se->inst);
    if (s != 0) {
        r = nsi_service_handle_available(s, avail);
        if (r < 0) {
            nai_log_error(NSI_LOG_CORE, nai_errno, 
                "the service(%d, %d) failed to handle available(%d) notify", 
                se->serv, se->inst, avail);
            goto _fail;
        };
    };


    /* notify clients */
    for (m = 0; m < 2; m ++) {
        e = nsi_discovery_get_servent(p, m ? se->serv : NSI_SERVICE_ANY, 1);
        if (e == 0) {
            r = 0;
            continue;
        };

        ent = e->list.next;
        for ( ; ent != &e->list; ) {
            q = nai_containof(ent, nsi_sd_request_t, ent);
            ent = ent->next;

            /* PRS_SOMEIPSD_00351 */
            if (q->inst != NSI_INSTANCE_ANY) {
                if (q->inst != se->inst) {
                    continue;
                };

                /* PRS_SOMEIPSD_00350
                 * PRS_SOMEIPSD_00408
                 * PRS_SOMEIPSD_00430
                 */
                if (q->time.stat != NSI_SD_STAT_MAIN) {
                    q->time.stat = NSI_SD_STAT_MAIN;
                    nsi_discovery_remove_timer(p, &q->time);
                };
            };

            if (q->major != NSI_MAJOR_ANY && 
                q->major != se->major) {
                continue;
            };
            if (q->minor != NSI_MINOR_ANY && 
                q->minor != se->minor) {
                continue;
            };


            nod = q->clients.next;
            for ( ; nod != &q->clients; ) {
                f = nai_containof(nod, nsi_sd_request_ref_t, entq);
                nod = nod->next;

                c = f->client;
                r = nsi_client_handle_available(
                    c, avail, se->serv, se->inst, se->major, se->minor, 0);
                if (r < 0) {
                    nai_log_error(NSI_LOG_CORE, nai_errno, 
                        "the client(0x%x) failed to "
                        "handle service(%d, %d) available(%d) notify", 
                        c->cid, se->serv, se->inst, avail);
                    goto _fail;
                };
            };
        };
    };

    r = 0;

_fail:
    if (r < 0) {
        ec = nai_errno;
    };
    nsi_routing_batch_end(p->rt);
    if (r < 0) {
        nai_errno = ec;
    };

_end:
    return r;
};


static int nsi_discovery_notify_exist(
    nsi_discovery_t* p, 
    nsi_cid_t cid, nsi_serv_t serv, nsi_inst_t inst, 
    nsi_version_t major, nsi_versmin_t minor)
{
    int r;
    int ec;
    nai_rbnode_t* n;
    nsi_client_t* c;
    nsi_sd_service_t* s;


    c = nsi_client_find_in_routing(p->rt, cid);
    if (c == 0) {
        r = -1;
        goto _end;
    };

    r = nsi_routing_batch_begin(p->rt);
    if (r < 0) {
        nai_log_alert(NSI_LOG_CORE, 
            nai_errno, "start batch failed");
        goto _end;
    };

    /* for each offer */
    n = nai_rbtree_begin(&p->offer);
    for ( ; n != nai_rbtree_end(&p->offer); ) {
        s = nai_containof(n, nsi_sd_service_t, ent);
        n = nai_rbtree_next(n);

        if ((serv != NSI_SERVICE_ANY && serv != s->serv) || 
            (inst != NSI_INSTANCE_ANY && inst != s->inst) || 
            (major != NSI_MAJOR_ANY && major != s->major) || 
            (minor != NSI_MINOR_ANY && minor != s->minor)) {
            continue;
        };

        r = nsi_client_handle_available(
            c, NSI_AVAIL_ONLINED, s->serv, s->inst, s->major, s->minor, 0);
        if (r < 0) {
            nai_log_error(NSI_LOG_CORE, nai_errno, 
                "the client(0x%x) failed to "
                "handle local service(%d, %d) available(%d) notify", 
                c->cid, s->serv, s->inst, NSI_AVAIL_ONLINED);
            goto _fail;
        };
    };

    /* for each service */
    n = nai_rbtree_begin(&p->servs);
    for ( ; n != nai_rbtree_end(&p->servs); ) {
        s = nai_containof(n, nsi_sd_service_t, ent);
        n = nai_rbtree_next(n);

        if (s->ttl == 0) {
            continue;
        };
        if ((serv != NSI_SERVICE_ANY && serv != s->serv) || 
            (inst != NSI_INSTANCE_ANY && inst != s->inst) || 
            (major != NSI_MAJOR_ANY && major != s->major) || 
            (minor != NSI_MINOR_ANY && minor != s->minor)) {
            continue;
        };

        r = nsi_client_handle_available(
            c, NSI_AVAIL_ONLINED, s->serv, s->inst, s->major, s->minor, 0);
        if (r < 0) {
            nai_log_error(NSI_LOG_CORE, nai_errno, 
                "the client(0x%x) failed to "
                "handle remote service(%d, %d) available(%d) notify", 
                c->cid, s->serv, s->inst, NSI_AVAIL_ONLINED);
            goto _fail;
        };
    };

_fail:
    if (r < 0) {
        ec = nai_errno;
    };

    nsi_routing_batch_end(p->rt);

    if (r < 0) {
        nai_errno = ec;
    };

_end:
    return r;
};


