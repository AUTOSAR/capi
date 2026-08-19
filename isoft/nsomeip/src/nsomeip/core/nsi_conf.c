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
/// @file       nsi_conf.c
/// @brief
/// @details
/// @date       2021-05-31
/// @author     xn
/// @version    1.2.0
///
/// ================================================================

#include "nsomeip/core/nsi_conf.h"
#include "nsomeip/core/nsi_routing.h"
#include "nsomeip/core/nsi_routing_impl.h"
#include "nsomeip/core/nsi_discovery.h"
#include "nsomeip/core/nsi_const.h"
#include "nsomeip/core/nsi_log.h"
#include "nsomeip/net/nsi_message.h"
#include "nai/os/nai_socket.h"
#include "nai/runtime/nai_value.h"
#include "nai/runtime/nai_errno.h"



typedef struct nsi_subs_info_s nsi_conf_subs_t;
typedef struct nsi_conf_bind_s nsi_conf_bind_t;
typedef struct nsi_conf_local_s nsi_conf_local_t;
typedef struct nsi_conf_event_s nsi_conf_event_t;
typedef struct nsi_conf_group_s nsi_conf_group_t;
typedef struct nsi_conf_service_s nsi_conf_service_t;
typedef struct nsi_conf_app_s nsi_conf_app_t;


struct nsi_conf_bind_s {
    nai_rbnode_t ent;
    nsi_endpoint_name_t name;
    nai_list_entry_t node;
    int reliability;
    nai_array_t ports;
};


struct nsi_conf_local_s {
    nsi_range_t serv;
    nsi_range_t inst;
};


struct nsi_conf_event_s {
    nai_rbnode_t ent;
    nsi_eid_t eid;
    int type;
    int reliability;
};


struct nsi_conf_group_s {
    nai_rbnode_t ent;
    nsi_gid_t gid;
    uint32_t threshold;
    uint32_t ttl;               /**< the ttl of subscribe, in seconds */
    uint32_t subscribe_timeo;   /**< the timeout of subscribe, milli-seconds */
    uint32_t subscribe_renew;   /**< the max times of resubscribe */
    nsi_endpoint_name_t mcast;
    nai_array_t subs;
};


struct nsi_conf_service_s {
    nai_rbnode_t ent;

    nai_list_entry_t node;

    /* service id */
    union {
        /**
         * keep serv is high word, 
         * so that the same serv is continuously in the tree.
         */
        struct {
#if defined(NAI_HAVE_BIG_ENDIAN)
            nsi_serv_t serv;
            nsi_inst_t inst;
#else
            nsi_inst_t inst;
            nsi_serv_t serv;
#endif
        };
        nsi_servinst_t servinst;
    };


    uint32_t local:1;           /**< is a local service */
    uint32_t preset:1;          /**< is a preset service */
    uint32_t signal:1;          /**< with signal */
    int reliability;            /**< default value of reliability */
    nsi_cid_t owner;            /**< the owner of configure */
    nsi_version_t major;        /**< the major version of the service */
    nsi_versmin_t minor;        /**< the minor version of the service */
    uint32_t ttl;               /**< the ttl of the service */
    uint32_t cyclic_delay;      /**< the interval time of offer message */


    union {
        nsi_endpoint_name_t names[3];
        struct {
            nsi_endpoint_name_t ucast1;
            nsi_endpoint_name_t ucast2;
            nsi_endpoint_name_t mcast;
        };
    };

    nai_rbtree_t events;        /**< the map of events and signals */
    nai_rbtree_t groups;        /**< the map of groups */

    uint16_t tls_version;           /**< 12 for version 1.2 and 13 for version 1.3 */
    uint16_t tls_verify_client;     /**< verify client or not, 1 or 0 */
    nai_str_t tls_peer_ca;          /**< ca file path used to verify peer cert */
    nai_str_t tls_local_cert;       /**< local cert file path */
    nai_str_t tls_local_privkey;    /**< local private key file path */

};


struct nsi_conf_app_s {
    nai_rbnode_t ent;
    nai_str_t name;
    nsi_cid_t cid;
};


typedef struct nsi_conf_ctx_s nsi_conf_ctx_t;
typedef struct nsi_conf_arg_s nsi_conf_arg_t;
typedef struct nsi_conf_stack_s nsi_conf_stack_t;
typedef struct nsi_conf_optinfo_s nsi_conf_optinfo_t;
typedef struct nsi_conf_command_s nsi_conf_command_t;
typedef int (*nsi_conf_command_f)(
    nsi_conf_command_t* ctx, nsi_conf_arg_t* arg);


struct nsi_conf_optinfo_s {
    nai_str_t name;
    int value;
};


struct nsi_conf_command_s {
    const char* name;
    nsi_conf_command_f handle;
    int flags;
    size_t offset;
    void* ctx;
};


struct nsi_conf_arg_s {
    nsi_conf_ctx_t* ctx;
    nai_value_t v;
    char* data;
};


struct nsi_conf_stack_s {
    nsi_conf_stack_t* prev;
    nsi_conf_command_t* cmds;
    char* data;
};


struct nsi_conf_ctx_s {
    nai_pool_t pool;
    nsi_conf_t* conf;
    nsi_conf_stack_t* top;
};


#define NSI_CONF_BEGIN          0x01
#define NSI_CONF_END            0x02
#define NSI_CONF_ADDRONLY       0x0100


static int nsi_conf_setopt(nsi_conf_t* p, const char* name, nai_value_t* v);


static nsi_conf_command_t* 
    nsi_conf_find_command(
    nsi_conf_command_t* tbl, const char* name, size_t namelen)
{
    int n;
    nsi_conf_command_t* cmd;


    for (n = 0; ; n ++) {
        cmd = tbl + n;
        if (cmd->name == 0) {
            break;
        };
        if (nai_strncmp(cmd->name, name, namelen+1) == 0) {
            goto _end;
        };
    };

    cmd = 0;

_end:
    return cmd;
};


static nai_rbnode_t** nsi_conf_find_app(
    nsi_conf_t* p, const nai_str_t* name, nai_rbnode_t** pparent)
{
    int c;
    nai_rbnode_t** n = &nai_rbtree_root(&p->apps);
    nai_rbnode_t* parent = nai_rbtree_end(&p->apps);
    nsi_conf_app_t* e;

    while (*n) {
        parent = *n;
        e = (nsi_conf_app_t*)parent;
        c = nai_str_cmp(&e->name, name);
        if (c == 0) {
            break;
        } else if (c >= 0) {
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


static nai_rbnode_t** nsi_conf_find_bind(
    nsi_conf_t* p, int reliable, const nsi_endpoint_name_t* name, 
    nai_rbnode_t** pparent)
{
    return nsi_endpoint_name_find(
        &p->binds[reliable], name->addr, name->len, pparent);
};


static nai_rbnode_t** nsi_conf_find_service(
    nsi_conf_t* p, nsi_serv_t serv, nsi_inst_t inst, 
    nai_rbnode_t** pparent)
{
    nai_rbnode_t** n = &nai_rbtree_root(&p->servs);
    nai_rbnode_t* parent = nai_rbtree_end(&p->servs);
    nsi_conf_service_t* e;
    nsi_servkey_t v;


    v.serv = serv;
    v.inst = inst;

    while (*n) {
        parent = *n;
        e = (nsi_conf_service_t*)parent;
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


static nai_rbnode_t** nsi_conf_find_extra(
    nsi_conf_t* p, nsi_serv_t serv, nsi_inst_t inst, 
    nai_rbnode_t** pparent)
{
    nai_rbnode_t** n = &nai_rbtree_root(&p->extra);
    nai_rbnode_t* parent = nai_rbtree_end(&p->extra);
    nsi_conf_service_t* e;
    nsi_servkey_t v;


    v.serv = serv;
    v.inst = inst;

    while (*n) {
        parent = *n;
        e = (nsi_conf_service_t*)parent;
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


static nai_rbnode_t** nsi_conf_find_event(
    nsi_conf_service_t* s, nsi_eid_t eid, nai_rbnode_t** pparent)
{
    nai_rbnode_t** n = &nai_rbtree_root(&s->events);
    nai_rbnode_t* parent = nai_rbtree_end(&s->events);
    nsi_conf_event_t* e;


    while (*n) {
        parent = *n;
        e = nai_containof(parent, nsi_conf_event_t, ent);
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


static nai_rbnode_t** nsi_conf_find_group(
    nsi_conf_service_t* s, nsi_gid_t gid, nai_rbnode_t** pparent)
{
    nai_rbnode_t** n = &nai_rbtree_root(&s->groups);
    nai_rbnode_t* parent = nai_rbtree_end(&s->groups);
    nsi_conf_group_t* g;


    while (*n) {
        parent = *n;
        g = nai_containof(parent, nsi_conf_group_t, ent);
        if (g->gid == gid) {
            break;
        } else if (g->gid >= gid) {
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


static int nsi_conf_insert_bind(
    nsi_conf_t* p, nsi_conf_bind_t* b)
{
    int r;
    int reliable;
    nai_rbtree_t* binds;
    nai_rbnode_t** n;
    nai_rbnode_t* parent;


    reliable = b->reliability == NSI_RT_RELIABLE;
    binds = &p->binds[reliable];
    n = nsi_endpoint_name_lbound(
        binds, b->name.addr, b->name.len, &parent);


    nai_rbtree_link(binds, &b->ent, parent, n);
    nai_rbtree_color(binds, &b->ent);
    r = 0;

    return r;
};


static int nsi_conf_insert_extra(
    nsi_conf_t* p, nsi_conf_service_t* s)
{
    int r;
    nai_rbnode_t** n = &nai_rbtree_root(&p->extra);
    nai_rbnode_t* parent = nai_rbtree_end(&p->extra);
    nsi_conf_service_t* e;
    nsi_servkey_t v;


    v.servinst = s->servinst;

    while (*n) {
        parent = *n;
        e = (nsi_conf_service_t*)parent;
        if (e->servinst >= v.servinst) {
            n = &parent->rb_left;
        } else {
            n = &parent->rb_right;
        };
    };

    nai_rbtree_link(&p->extra, &s->ent, parent, n);
    nai_rbtree_color(&p->extra, &s->ent);
    r = 0;

    return r;
};


static int nsi_conf_handle_address(nsi_conf_t* p, nsi_endpoint_name_t* name)
{
    int r;
    int port;
    nai_sockaddr_t* addr;
    const nai_sockaddr_info_t* si;


    if (name->addr && name->len == 0) {
        /* is a port */
        port = (int)(intptr_t)name->addr;
        addr = (nai_sockaddr_t*)nai_palloc(&p->pool, p->host.len);
        if (addr == 0) {
            r = -1;
            goto _end;
        };

        si = nai_sockaddr_info(p->host.addr->sa_family);
        nai_memcpy(addr, p->host.addr, p->host.len);
        nai_sockaddr_set_port(si, addr, port);
        name->addr = addr;
        name->len = p->host.len;
    };

    r = 0;

_end:
    return r;
};


static int nsi_conf_check_multicast(
    const char* name, nsi_endpoint_name_t* mcast, 
    const nsi_endpoint_name_t* ucast, int sameport)
{
    int r;
    int port1;
    int port2;
    const nai_sockaddr_info_t* si;


    if (mcast->addr != 0 && ucast->addr != 0) {
        if (mcast->addr->sa_family != ucast->addr->sa_family) {
            nai_log_alert(NSI_LOG_CORE, EINVAL, 
                "%s requires the family of multicast address "
                "must be the same as the bound address", name);
            nai_errno = EINVAL;
            r = -1;
            goto _end;
        };

        si = nai_sockaddr_info(mcast->addr->sa_family);
        port1 = nai_sockaddr_get_port(si, mcast->addr);
        port2 = nai_sockaddr_get_port(si, ucast->addr);
        if (sameport) {
            if (port1 == 0) {
                nai_sockaddr_set_port(si, mcast->addr, port2);
            } else if (port2 == 0) {
                nai_log_alert(NSI_LOG_CORE, EINVAL, 
                    "%s requires the bound address with a valid port", name);
                nai_errno = EINVAL;
                r = -1;
                goto _end;
            } else if (port1 != port2) {
                nai_log_alert(NSI_LOG_CORE, EINVAL, 
                    "%s requires the port of multicast address "
                    "must be the same as the bound address", name);
                nai_errno = EINVAL;
                r = -1;
                goto _end;
            };
        } else {
            if (port1 == 0) {
                nai_sockaddr_set_port(si, mcast->addr, port2);
            };
#if 0
            if (port1 == 0) {
                nai_log_alert(NSI_LOG_CORE, EINVAL, 
                    "%s requires the multicast address with a valid port", 
                    name);
                nai_errno = EINVAL;
                r = -1;
                goto _end;
            };
#endif
        };
    };

    r = 0;

_end:
    return r;
};



int nsi_conf_init(nsi_conf_t* p)
{
    int r;


    p->mode = NSI_RTYPE_AUTO;
    p->ctx = 0;
    p->net.seted = 0;
    p->net.mss = -1;
    p->net.recv_mem = -1;
    p->net.recv_mem_hwm = -1;
    p->net.recv_mem_lwm = -1;
    p->net.recv_buf_udp = -1;
    p->net.recv_buf_tcp = -1;
    p->net.send_buf_udp = -1;
    p->net.send_buf_tcp = -1;
    p->net.recv_timeo = -1;
    p->net.send_timeo = -1;
    p->net.recv_segs = -1;
    p->net.send_segs = -1;
    p->net.queue_limit = -1;
    p->net.queue_mix_limit = -1;
    p->net.msg_limit = -1;
    p->net.tpm_limit = -1;
    p->net.tpm_max = -1;
    p->net.use_polling = -1;
    p->net.use_zerocopy = -1;
    p->net.use_npc = -1;
    p->net.npc_segcount = -1;
    p->net.npc_segsize = -1;
    p->net.keepalive_enable = -1;
    p->net.keepalive_idle = -1;
    p->net.keepalive_interval = -1;
    p->net.keepalive_cnt = -1;
    p->sd.enable = 0;
    p->sd.version = 2011;
    p->sd.init_delay_max = -1;
    p->sd.init_delay_min = -1;
    p->sd.request_response_delay_min = -1;
    p->sd.request_response_delay_max = -1;
    p->sd.offer_multicast_optim = -1;
    p->sd.repetitions_delay = -1;
    p->sd.repetitions_max = -1;
    p->sd.ttl = -1;
    p->sd.offer_cyclic_delay = -1;
    p->sd.offer_debounce = -1;
    p->sd.find_debounce = -1;
    p->sd.subscribe_ttl = -1;
    p->sd.subscribe_timeo = -1;
    p->sd.subscribe_renew = -1;
    p->cp.use_sd_option = -1;
    nai_str_setn(&p->netname);
    nai_str_setn(&p->net.npc_shm_path);
    nai_cidr_init(&p->umask);
    nai_cidr_init(&p->mmask);
    nai_sockname_init(&p->host);
    nai_sockname_init(&p->local);
    nai_sockname_init(&p->sd.ucast);
    nai_sockname_init(&p->sd.mcast);
    nai_pool_init(&p->pool, 1000);
    nai_rbtree_init(&p->apps);
    nai_rbtree_init(&p->binds[0]);
    nai_rbtree_init(&p->binds[1]);
    nai_rbtree_init(&p->servs);
    nai_rbtree_init(&p->extra);
    nai_list_init(&p->owns.servs);
    nai_list_init(&p->owns.binds);
    nai_array_init(&p->locals, sizeof(nsi_conf_local_t), &p->pool);
    r = 0;

    return r;
};


int nsi_conf_setopti(nsi_conf_t* p, const char* name, intptr_t v)
{
    int r;
    nai_value_t val;

    nai_value_init_intptr(&val, &v);
    r = nsi_conf_setopt(p, name, &val);

    return r;
};


int nsi_conf_setoptf(nsi_conf_t* p, const char* name, float v)
{
    int r;
    nai_value_t val;

    nai_value_init_float(&val, &v);
    r = nsi_conf_setopt(p, name, &val);

    return r;
};


int nsi_conf_setoptd(nsi_conf_t* p, const char* name, double v)
{
    int r;
    nai_value_t val;

    nai_value_init_double(&val, &v);
    r = nsi_conf_setopt(p, name, &val);

    return r;
};


int nsi_conf_setopts(nsi_conf_t* p, const char* name, const char* v)
{
    int r;
    nai_value_t val;


    nai_value_init_string(&val, v, nai_strlen(v));
    r = nsi_conf_setopt(p, name, &val);

    return r;
};


int nsi_conf_finish(nsi_conf_t* p)
{
    int r;
    int m;
    nai_rbnode_t* e;
    nai_rbnode_t* n;
    nsi_conf_ctx_t* ctx;
    nsi_conf_service_t* s;
    nsi_conf_group_t* g;
    nsi_conf_event_t* v;
    nai_socknbuf_t nbuf;
    char buf[140];


    if (p->ctx) {
        ctx = (nsi_conf_ctx_t*)p->ctx;
        if (ctx->top->data != (char*)p) {
            nai_log_error(NSI_LOG_CORE, 
                EINVAL, "configuration end not reached");
            nai_errno = EINVAL;
            r = -1;
            goto _end;
        };

        nai_pool_close(&ctx->pool);
        p->ctx = 0;
    };

    if (p->host.addr == 0) {
        nai_log_error(NSI_LOG_CORE, EINVAL, "missing host address");
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };
    if (p->umask.af != 0) {
        r = nai_sockaddr_match(p->host.addr, p->host.len, &p->umask);
        if (r == 0) {
            nai_log_error(NSI_LOG_CORE, 
                EINVAL, "host address missing match with umask");
            nai_errno = EINVAL;
            r = -1;
            goto _end;
        };
    };
    if (p->mmask.af != 0) {
        if (p->host.addr->sa_family != p->mmask.af) {
            nai_log_error(NSI_LOG_CORE, 
                EINVAL, "the family of mmask is different from host");
            nai_errno = EINVAL;
            r = -1;
            goto _end;
        };
    } else {
        switch (p->host.addr->sa_family) {
        case AF_INET:
            nai_inet_ptoc("224.0.0.0/4", -1, &p->mmask);
            break;
#if NAI_HAVE_SOCKADDR_IN6
        case AF_INET6:
            nai_inet_ptoc("ff00::/8", -1, &p->mmask);
            break;
#endif
        default:
            break;
        };
    };
    if (nai_str(&p->netname) == 0) {
        nai_str_setc(&p->netname, "nsomeip");
    };


    if (p->net.mss == (uint32_t)-1) {
        p->net.mss = 1500 - 60; /* mss for ipv6 */
    };

    if (p->cp.use_sd_option == (uint32_t)-1) {
        p->cp.use_sd_option = 1;
    };

    if (p->sd.enable) {
        r = nsi_conf_handle_address(p, &p->sd.ucast);
        if (r < 0) {
            goto _end;
        };

        /* check unreliability and mcast */
        r = nsi_conf_check_multicast(
            "service discovery", &p->sd.mcast, &p->sd.ucast, 0);
        if (r < 0) {
            goto _end;
        };
    };

    e = nai_rbtree_begin(&p->servs);
    for ( ; e != nai_rbtree_end(&p->servs); ) {
        s = (nsi_conf_service_t*)e;
        e = nai_rbtree_next(e);

        n = nai_rbtree_begin(&s->events);
        for ( ; n != nai_rbtree_end(&s->events); ) {
            v = (nsi_conf_event_t*)n;
            n = nai_rbtree_next(n);
            if (v->reliability == NSI_RT_UNKNOWN) {
                v->reliability = s->reliability;
            };
            if (v->type == NSI_ET_SIGNAL) {
                s->signal = 1;
            };
        };

        if (s->inst == NSI_INSTANCE_ANY || (
            s->local == 0 && s->preset == 0)) {
            continue;
        };

        /* PRS_SOMEIPSD_00362 */
        for (m = 0; m < 2; m ++) {
            r = nsi_conf_handle_address(p, &s->names[m]);
            if (r < 0) {
                goto _end;
            };
        };

        /* PRS_SOMEIPSD_00387 */
        if (s->names[0].addr && s->names[1].addr && 
            s->names[0].addr->sa_family != s->names[1].addr->sa_family) {
            nai_log_alert(NSI_LOG_CORE, EINVAL, 
                "command cannot add service which has addresses "
                "with different family");
            nai_errno = EINVAL;
            r = -1;
            goto _end;
        };

        /* check unreliability and mcast */
        r = nsi_conf_check_multicast(
            "service", &s->mcast, &s->names[0], 0);
        if (r < 0) {
            goto _end;
        };

        n = nai_rbtree_begin(&s->groups);
        for ( ; n != nai_rbtree_end(&s->groups); ) {
            g = (nsi_conf_group_t*)n;
            n = nai_rbtree_next(n);
            if (g->mcast.addr == 0) {
                g->mcast = s->mcast;
            } else {
                /* check unreliability and mcast */
                if (s->mcast.addr) {
                    r = nsi_conf_check_multicast(
                        "service group", &g->mcast, &s->mcast, 0);
                } else if (s->names[0].addr) {
                    r = nsi_conf_check_multicast(
                        "service group", &g->mcast, &s->names[0], 0);
                } else {
                    nai_log_alert(NSI_LOG_CORE, EINVAL, 
                        "command cannot add service which enable multicast "
                        "without unreliable address");
                    nai_errno = EINVAL;
                    r = -1;
                    goto _end;
                };
                if (r < 0) {
                    goto _end;
                };
            };
        };
    };


#if (NAI_HAVE_SOCKADDR_UN) && !defined(NSI_UNIX_SOCK_PREFIX)
#define NSI_UNIX_SOCK_PREFIX    "/tmp" 
#endif
#if (NAI_HAVE_SOCKADDR_UN) && !defined(NSI_UNIX_SOCK_SUFFIX)
#define NSI_UNIX_SOCK_SUFFIX    ".sock" 
#endif

#if (NAI_HAVE_SOCKADDR_UN)
    if (nai_str_len(&p->netname) > 0 && 
        nai_str_at(&p->netname, 0) == '/') {
        nai_snprintf(buf, sizeof(buf), 
            "unix:%s" NSI_UNIX_SOCK_SUFFIX, 
            nai_str(&p->netname));
    } else {
        nai_snprintf(buf, sizeof(buf), 
            "unix:" NSI_UNIX_SOCK_PREFIX "/%s" NSI_UNIX_SOCK_SUFFIX, 
            nai_str(&p->netname));
    };
#else
    nai_snprintf(buf, sizeof(buf), "127.0.0.1:%d", 34512);
#endif
    nbuf.len = sizeof(nbuf.storage);
    r = nai_sockaddr_pton(buf, -1, &nbuf.addr, &nbuf.len);
    if (r < 0) {
        nai_log_error(NSI_LOG_CORE, nai_errno, 
            "nai_sockaddr_pton parse local address failed");
        r = -1;
        goto _end;
    };

    p->local.addr = (nai_sockaddr_t*)nai_palloc(&p->pool, nbuf.len);
    if (p->local.addr == 0) {
        nai_log_error(NSI_LOG_CORE, nai_errno, 
            "allocate local address failed");
        r = -1;
        goto _end;
    };

    nai_memcpy(p->local.addr, &nbuf.addr, nbuf.len);
    p->local.len = nbuf.len;
    r = 0;

_end:
    return r;
};


int nsi_conf_close(nsi_conf_t* p)
{
    int r;
    nsi_conf_ctx_t* ctx;


    if (p->ctx) {
        ctx = (nsi_conf_ctx_t*)p->ctx;
        nai_pool_close(&ctx->pool);
        p->ctx = 0;
    };

    nai_pool_close(&p->pool);
    nsi_conf_init(p);
    r = 0;

    return r;
};


int nsi_conf_get_client_id(nsi_conf_t* p, 
    const nai_str_t* name, nsi_cid_t* cid)
{
    int r;
    nai_rbnode_t** n;
    nsi_conf_app_t* a;


    n = nsi_conf_find_app(p, name, 0);
    if (n[0] == 0) {
        nai_errno = ENOENT;
        r = -1;
        goto _end;
    };

    a = (nsi_conf_app_t*)n[0];
    if (cid) {
        cid[0] = a->cid;
    };

    r = 0;

_end:
    return r;
};


int nsi_conf_is_local_service(nsi_conf_t* p, 
    nsi_serv_t serv, nsi_inst_t inst)
{
    int r;
    int m;
    nai_rbnode_t** n;
    nsi_conf_local_t* l;
    nsi_conf_service_t* s;


    if (inst == NSI_INSTANCE_ANY) {
        r = 0;
        goto _end;
    };

    n = nsi_conf_find_service(p, serv, inst, 0);
    if (n[0] != 0) {
        s = nai_containof(n[0], nsi_conf_service_t, ent);
        r = s->local;
        goto _end;
    };

    m = 0;
    for ( ; m < (intptr_t)p->locals.count; m ++) {
        l = (nsi_conf_local_t*)p->locals.elts + m;
        if (l->serv.start <= serv && l->serv.end >= serv && 
            l->inst.start <= inst && l->inst.end >= inst) {
            r = 1;
            goto _end;
        };
    };

    r = 0;

_end:
    return r;
};


int nsi_conf_is_preset_service(nsi_conf_t* p, 
    nsi_serv_t serv, nsi_inst_t inst)
{
    int r;
    nai_rbnode_t** n;
    nsi_conf_service_t* s;


    if (inst == NSI_INSTANCE_ANY) {
        r = 0;
        goto _end;
    };

    n = nsi_conf_find_service(p, serv, inst, 0);
    if (n[0] != 0) {
        s = nai_containof(n[0], nsi_conf_service_t, ent);
        r = s->preset;
        goto _end;
    };

    r = 0;

_end:
    return r;
};


int nsi_conf_is_signal(nsi_conf_t* p, 
    nsi_serv_t serv, nsi_inst_t inst, nsi_eid_t eid)
{
    int r;
    nai_rbnode_t** n;
    nsi_conf_service_t* s;
    nsi_conf_event_t* e;


    n = nsi_conf_find_service(p, serv, inst, 0);
    if (n[0] == 0) {
        r = 0;
        goto _end;
    };

    s = nai_containof(n[0], nsi_conf_service_t, ent);
    if (!s->signal) {
        r = 0;
        goto _end;
    };

    n = nsi_conf_find_event(s, eid, 0);
    if (n[0] == 0) {
        r = 0;
        goto _end;
    };

    e = nai_containof(n[0], nsi_conf_event_t, ent);
    r = e->type == NSI_ET_SIGNAL;

_end:
    return r;
};


int nsi_conf_is_subnet(nsi_conf_t* p, const nsi_endpoint_name_t* name)
{
    int r;


    if (p->umask.af == 0) {
        r = 1;
        goto _end;
    };

    r = nai_sockaddr_match(name->addr, name->len, &p->umask);

_end:
    return r;
};


int nsi_conf_is_mcast(nsi_conf_t* p, const nsi_endpoint_name_t* name)
{
    int r;


    if (p->mmask.af == 0) {
        r = 1;
        goto _end;
    };

    r = nai_sockaddr_match(name->addr, name->len, &p->mmask);

_end:
    return r;
};


int nsi_conf_get_bind(nsi_conf_t* p, 
    int reliable, const nsi_endpoint_name_t* name, nsi_range_t** pptr)
{
    int r;
    nai_rbnode_t** n;
    nsi_conf_bind_t* b;


    n = nsi_conf_find_bind(p, !!reliable, name, 0);
    if (n[0] == 0) {
        nai_errno = ENOENT;
        r = -1;
        goto _end;
    };

    b = nai_containof(n[0], nsi_conf_bind_t, ent);
    if (pptr) {
        pptr[0] = (nsi_range_t*)b->ports.elts;
    };

    r = (int)b->ports.count;

_end:
    return r;
};

NSI_EXTERN
int nsi_conf_get_tls_info(nsi_conf_t*p,
    nsi_serv_t serv, nsi_inst_t inst, nsi_tls_info_t* tls)
{
    int r;
    nai_rbnode_t** n;
    nsi_conf_service_t* s;


    n = nsi_conf_find_service(p, serv, inst, 0);
    if (n[0] == 0) {
        nai_errno = ENOENT;
        r = -1;
        goto _end;
    };

    s = nai_containof(n[0], nsi_conf_service_t, ent);

    if (tls) {
        tls->tls_version = s->tls_version;
        tls->tls_verify_client = s->tls_verify_client;
        tls->tls_peer_ca = s->tls_peer_ca;
        tls->tls_local_cert = s->tls_local_cert;
        tls->tls_local_privkey = s->tls_local_privkey;
    };
    r = 0;

_end:
    return r;
};


int nsi_conf_get_service(nsi_conf_t* p, 
    nsi_serv_t serv, nsi_inst_t inst, nsi_service_info_t* info)
{
    int r;
    nai_rbnode_t** n;
    nsi_conf_service_t* s;


    n = nsi_conf_find_service(p, serv, inst, 0);
    if (n[0] == 0) {
        nai_errno = ENOENT;
        r = -1;
        goto _end;
    };

    s = nai_containof(n[0], nsi_conf_service_t, ent);
    if (s->local == 0 && s->preset == 0) {
        nai_errno = ENOENT;
        r = -1;
        goto _end;
    };

    if (info) {
        info->local = s->local;
        info->owner = s->owner;
        info->serv = serv;
        info->inst = inst;
        info->major = s->major;
        info->minor = s->minor;
        info->ttl = s->ttl;
        info->cyclic_delay = s->cyclic_delay;
        info->names[0] = s->names[0];
        info->names[1] = s->names[1];
        info->tls.tls_version = s->tls_version;
        info->tls.tls_verify_client = s->tls_verify_client;
        info->tls.tls_peer_ca = s->tls_peer_ca;
        info->tls.tls_local_cert = s->tls_local_cert;
        info->tls.tls_local_privkey = s->tls_local_privkey;
    };
    r = 0;

_end:
    return r;
};


int nsi_conf_get_event(nsi_conf_t* p, 
    nsi_serv_t serv, nsi_inst_t inst, nsi_eid_t eid, nsi_event_info_t* info)
{
    int r;
    nai_rbnode_t** n;
    nsi_conf_service_t* s;
    nsi_conf_event_t* e;


    n = nsi_conf_find_service(p, serv, inst, 0);
    if (n[0] == 0) {
        nai_errno = ENOENT;
        r = -1;
        goto _end;
    };

    s = nai_containof(n[0], nsi_conf_service_t, ent);
    n = nsi_conf_find_event(s, eid, 0);
    if (n[0] == 0) {
        nai_errno = ENOENT;
        r = -1;
        goto _end;
    };

    e = nai_containof(n[0], nsi_conf_event_t, ent);
    if (info) {
        info->type = e->type;
        info->reliability = e->reliability;
        info->ngroup = 0;
        info->groups = 0;
    };
    r = 0;

_end:
    return r;
};


int nsi_conf_get_group(nsi_conf_t* p, 
    nsi_serv_t serv, nsi_inst_t inst, nsi_gid_t gid, nsi_group_info_t* info)
{
    int r;
    nai_rbnode_t** n;
    nsi_conf_service_t* s;
    nsi_conf_group_t* g;


    n = nsi_conf_find_service(p, serv, inst, 0);
    if (n[0] == 0) {
        nai_errno = ENOENT;
        r = -1;
        goto _end;
    };

    s = nai_containof(n[0], nsi_conf_service_t, ent);
    n = nsi_conf_find_group(s, gid, 0);
    if (n[0] == 0) {
        nai_errno = ENOENT;
        r = -1;
        goto _end;
    };

    g = nai_containof(n[0], nsi_conf_group_t, ent);
    if (info) {
        info->mcast = g->mcast;
        info->threshold = g->threshold;
        info->ttl = g->ttl;
        info->subscribe_timeo = g->subscribe_timeo;
        info->subscribe_renew = g->subscribe_renew;
        info->scount = (int)g->subs.count;
        info->subs = (nsi_subs_info_t*)g->subs.elts;
    };
    r = 0;

_end:
    return r;
};



//////////////////////////////////////////////////////////////////////////////
// conf option commands

/**
 * 1. service info
 * 2. service local range
 * 3. event: type, reliable
 * 4. eventgroup: multi-cast
 * 5. client port
 * 6. max message size, reliable/unreliable/local
 * 7. service discovery
 */


static nsi_conf_optinfo_t nsi_option_onoff[] = {
    { nai_strconst("on"), 1 }, 
    { nai_strconst("off"),  0 },
    { nai_strnull(), 0 }
};

static nsi_conf_optinfo_t nsi_option_mode[] = {
    { nai_strconst("auto"), NSI_RTYPE_AUTO }, 
    { nai_strconst("route"),  NSI_RTYPE_ROUTE },
    { nai_strconst("proxy"),  NSI_RTYPE_PROXY },
    { nai_strnull(), 0 }
};

static nsi_conf_optinfo_t nsi_option_et[] = {
    { nai_strconst("event"), NSI_ET_EVENT }, 
    { nai_strconst("field"), NSI_ET_FIELD }, 
    { nai_strconst("unknown"), NSI_ET_UNKNOWN }, 
    { nai_strnull(), 0 }
};

static nsi_conf_optinfo_t nsi_option_rt[] = {
    { nai_strconst("reliable"), NSI_RT_RELIABLE }, 
    { nai_strconst("unreliable"),  NSI_RT_UNRELIABLE },
    { nai_strconst("both"),  NSI_RT_BOTH },
    { nai_strconst("unknown"),  NSI_RT_UNKNOWN },
    { nai_strnull(), 0 }
};


static nai_sult_t nsi_conf_set_int8(
    nsi_conf_command_t* c, nsi_conf_arg_t* arg)
{
    int v;
    uint8_t* d;
    nai_sult_t rc;


    rc = nai_value_geti(&arg->v, &v);
    if (rc < 0) {
        nai_log_error(NSI_LOG_CORE, 
            nai_sult_to_errno(rc), 
            "invalid argument of command '%s'", c->name);
        goto _end;
    };
    if (v < -128 || v > 255) {
        rc = nai_errno_to_sult(ERANGE);
        nai_log_error(NSI_LOG_CORE, 
            nai_sult_to_errno(rc), 
            "command '%s' argument is out of range", c->name);
        goto _end;
    };

    d = (uint8_t*)(arg->data + c->offset);
    d[0] = (uint8_t)v;
    rc = 0;

_end:
    return rc;
};


static nai_sult_t nsi_conf_set_int16(
    nsi_conf_command_t* c, nsi_conf_arg_t* arg)
{
    int v;
    uint16_t* d;
    nai_sult_t rc;


    rc = nai_value_geti(&arg->v, &v);
    if (rc < 0) {
        nai_log_error(NSI_LOG_CORE, 
            nai_sult_to_errno(rc), 
            "invalid argument of command '%s'", c->name);
        goto _end;
    };
    if (v < -32768 || v > 65535) {
        rc = nai_errno_to_sult(ERANGE);
        nai_log_error(NSI_LOG_CORE, 
            nai_sult_to_errno(rc), 
            "command '%s' argument is out of range", c->name);
        goto _end;
    };

    d = (uint16_t*)(arg->data + c->offset);
    d[0] = (uint16_t)v;
    rc = 0;

_end:
    return rc;
};


static nai_sult_t nsi_conf_set_int32(
    nsi_conf_command_t* c, nsi_conf_arg_t* arg)
{
    int v;
    uint32_t* d;
    nai_sult_t rc;


    rc = nai_value_geti(&arg->v, &v);
    if (rc < 0) {
        nai_log_error(NSI_LOG_CORE, 
            nai_sult_to_errno(rc), 
            "invalid argument of command '%s'", c->name);
        goto _end;
    };


    d = (uint32_t*)(arg->data + c->offset);
    d[0] = v;
    rc = 0;

_end:
    return rc;
};


static nai_sult_t nsi_conf_set_option(
    nsi_conf_command_t* c, nsi_conf_arg_t* arg)
{
    nai_sult_t rc;
    int m;
    int found;
    int optval = 0;
    nai_mem_t name;
    nsi_conf_optinfo_t* opts;


    opts = (nsi_conf_optinfo_t*)c->ctx;
    if (opts == 0) {
        opts = nsi_option_onoff;
    };


    rc = nai_value_getm(&arg->v, &name);
    if (rc < 0) {
        nai_log_error(NSI_LOG_CORE, 
            nai_sult_to_errno(rc), 
            "invalid argument of command '%s'", c->name);
        goto _end;
    };

    found = 0;
    for (m = 0; opts[m].name.ptr; m ++) {
        if (nai_str_caseeq(&name, &opts[m].name)) {
            optval |= opts[m].value;
            found = 1;
            break;
        };
    };
    if (!found) {
        nai_log_error(NSI_LOG_CORE, EINVAL, 
            "invalid option value '%s' of command '%s'", 
            nai_str(&name), c->name);
        rc = nai_errno_to_sult(EINVAL);
        goto _end;
    };

    *(int*)(arg->data + c->offset) = optval;
    rc = 0;

_end:
    return rc;
};


static nai_sult_t nsi_conf_set_string(
    nsi_conf_command_t* c, nsi_conf_arg_t* arg)
{
    nai_sult_t rc;
    nai_str_t* d;
    nsi_conf_t* conf;


    d = (nai_str_t*)(arg->data + c->offset);
    if (nai_str(d)) {
        rc = nai_errno_to_sult(EEXIST);
        nai_log_error(NSI_LOG_CORE, 
            nai_sult_to_errno(rc), 
            "duplicated of command '%s'", c->name);
        goto _end;
    };

    conf = arg->ctx->conf;
    rc = nai_value_dups(&arg->v, d, &conf->pool);

_end:
    return rc;
};


static nai_sult_t nsi_conf_set_address(
    nsi_conf_command_t* c, nsi_conf_arg_t* arg)
{
    int port;
    nai_sult_t rc;
    nai_mem_t str;
    nsi_conf_t* conf;
    nsi_endpoint_name_t* d;
    nai_socknbuf_t nbuf;


    d = (nsi_endpoint_name_t*)(arg->data + c->offset);
    if (d->addr) {
        rc = nai_errno_to_sult(EEXIST);
        nai_log_error(NSI_LOG_CORE, 
            nai_sult_to_errno(rc), 
            "duplicated of command '%s'", c->name);
        goto _end;
    };


    if (!(c->flags & NSI_CONF_ADDRONLY)) {
        rc = nai_value_geti(&arg->v, &port);
        if (rc >= 0) {
            if (port <= 0 || port > 65535) {
                rc = nai_errno_to_sult(ERANGE);
                nai_log_error(NSI_LOG_CORE, 
                    nai_sult_to_errno(rc), 
                    "invalid port of command '%s'", c->name);
                goto _end;
            };

            d->addr = (nai_sockaddr_t*)(intptr_t)port;
            rc = 0;
            goto _end;
        };
    };

    rc = nai_value_getm(&arg->v, &str);
    if (rc < 0) {
        nai_log_error(NSI_LOG_CORE, 
            nai_sult_to_errno(rc), 
            "invalid argument of command '%s'", c->name);
        goto _end;
    };

    nbuf.len = sizeof(nbuf.storage);
    rc = nai_sockaddr_pton(
        nai_str(&str), nai_str_len(&str), &nbuf.addr, &nbuf.len);
    if (rc < 0) {
        rc = (int)nai_sockaddr_list(nai_str(&str), 0, &nbuf, nbuf.len);
        if (rc < 0) {
            rc = nai_sult_from_errno();
            nai_log_error(NSI_LOG_CORE, 
                nai_sult_to_errno(rc), 
                "invalid address(%.*s) of command '%s'", 
                nai_str_len(&str), nai_str(&str), c->name);
            goto _end;
        };
    };

    conf = arg->ctx->conf;
    d->addr = (nai_sockaddr_t*)nai_palloc(&conf->pool, nbuf.len);
    if (d->addr == 0) {
        rc = nai_sult_from_errno();
        nai_log_alert(NSI_LOG_CORE, 
            nai_sult_to_errno(rc), 
            "command '%s' allocate memory failed", c->name);
        goto _end;
    };

    d->len = nbuf.len;
    nai_memcpy(d->addr, &nbuf.addr, nbuf.len);
    rc = 0;

_end:
    return rc;
};


static nai_sult_t nsi_conf_set_cidr(
    nsi_conf_command_t* c, nsi_conf_arg_t* arg)
{
    nai_sult_t rc;
    nai_mem_t str;
    nai_cidr_t* d;
    nai_cidr_t cbuf;


    d = (nai_cidr_t*)(arg->data + c->offset);
    if (d->af) {
        rc = nai_errno_to_sult(EEXIST);
        nai_log_error(NSI_LOG_CORE, 
            nai_sult_to_errno(rc), 
            "duplicated of command '%s'", c->name);
        goto _end;
    };


    rc = nai_value_getm(&arg->v, &str);
    if (rc < 0) {
        nai_log_error(NSI_LOG_CORE, 
            nai_sult_to_errno(rc), 
            "invalid argument of command '%s'", c->name);
        goto _end;
    };

    rc = nai_inet_ptoc(nai_str(&str), nai_str_len(&str), &cbuf);
    if (rc < 0) {
        rc = nai_sult_from_errno();
        nai_log_error(NSI_LOG_CORE, 
            nai_sult_to_errno(rc), 
            "invalid cidr(%.*s) of command '%s'", 
            nai_str_len(&str), nai_str(&str), c->name);
        goto _end;
    };

    d[0] = cbuf;

_end:
    return rc;
};


static nai_sult_t nsi_conf_command_begin(
    nsi_conf_command_t* c, nsi_conf_arg_t* arg);
static nai_sult_t nsi_conf_command_end(
    nsi_conf_command_t* c, nsi_conf_arg_t* arg);
static nai_sult_t nsi_conf_network(
    nsi_conf_command_t* c, nsi_conf_arg_t* arg);
static nai_sult_t nsi_conf_network_end(
    nsi_conf_command_t* c, nsi_conf_arg_t* arg);
static nai_sult_t nsi_conf_app(
    nsi_conf_command_t* c, nsi_conf_arg_t* arg);
static nai_sult_t nsi_conf_app_end(
    nsi_conf_command_t* c, nsi_conf_arg_t* arg);
static nai_sult_t nsi_conf_bind(
    nsi_conf_command_t* c, nsi_conf_arg_t* arg);
static nai_sult_t nsi_conf_bind_end(
    nsi_conf_command_t* c, nsi_conf_arg_t* arg);
static nai_sult_t nsi_conf_port(
    nsi_conf_command_t* c, nsi_conf_arg_t* arg);
static nai_sult_t nsi_conf_port_end(
    nsi_conf_command_t* c, nsi_conf_arg_t* arg);
static nai_sult_t nsi_conf_service(
    nsi_conf_command_t* c, nsi_conf_arg_t* arg);
static nai_sult_t nsi_conf_service_end(
    nsi_conf_command_t* c, nsi_conf_arg_t* arg);
static nai_sult_t nsi_conf_event(
    nsi_conf_command_t* c, nsi_conf_arg_t* arg);
static nai_sult_t nsi_conf_event_end(
    nsi_conf_command_t* c, nsi_conf_arg_t* arg);
static nai_sult_t nsi_conf_group(
    nsi_conf_command_t* c, nsi_conf_arg_t* arg);
static nai_sult_t nsi_conf_group_end(
    nsi_conf_command_t* c, nsi_conf_arg_t* arg);
static nai_sult_t nsi_conf_subs(
    nsi_conf_command_t* c, nsi_conf_arg_t* arg);
static nai_sult_t nsi_conf_subs_end(
    nsi_conf_command_t* c, nsi_conf_arg_t* arg);
static nai_sult_t nsi_conf_local(
    nsi_conf_command_t* c, nsi_conf_arg_t* arg);
static nai_sult_t nsi_conf_local_end(
    nsi_conf_command_t* c, nsi_conf_arg_t* arg);
static nai_sult_t nsi_conf_discovery(
    nsi_conf_command_t* c, nsi_conf_arg_t* arg);
static nai_sult_t nsi_conf_discovery_end(
    nsi_conf_command_t* c, nsi_conf_arg_t* arg);
static nai_sult_t nsi_conf_compatible(
    nsi_conf_command_t* c, nsi_conf_arg_t* arg);
static nai_sult_t nsi_conf_compatible_end(
    nsi_conf_command_t* c, nsi_conf_arg_t* arg);


static nsi_conf_command_t nsi_conf_begin = {
    "begin", nsi_conf_command_begin, 0, 0
};
static nsi_conf_command_t nsi_conf_end = {
    "end", nsi_conf_command_end, 0, 0
};
static nsi_conf_command_t nsi_conf_main[] = {
    { "netname", nsi_conf_set_string, 0, 
      nai_offsetof(nsi_conf_t, netname) }, 
    { "host", nsi_conf_set_address, NSI_CONF_ADDRONLY, 
      nai_offsetof(nsi_conf_t, host) }, 
    { "mode", nsi_conf_set_option, 0, 
      nai_offsetof(nsi_conf_t, mode), 
      nsi_option_mode }, 
    { "umask", nsi_conf_set_cidr, 0, 
      nai_offsetof(nsi_conf_t, umask) }, 
    { "mmask", nsi_conf_set_cidr, 0, 
      nai_offsetof(nsi_conf_t, mmask) }, 
    { "network", nsi_conf_network, NSI_CONF_BEGIN, 0 }, 
    { "application", nsi_conf_app, NSI_CONF_BEGIN, 0 }, 
    { "bind", nsi_conf_bind, NSI_CONF_BEGIN, 0 }, 
    { "service", nsi_conf_service, NSI_CONF_BEGIN, 0 }, 
    { "service_exist", nsi_conf_service, NSI_CONF_BEGIN, 0 }, 
    { "service_local", nsi_conf_local, NSI_CONF_BEGIN, 0 }, 
    { "service_discovery", nsi_conf_discovery, NSI_CONF_BEGIN, 0 }, 
    { "compatible", nsi_conf_compatible, NSI_CONF_BEGIN, 0 }, 
    { 0 }, 
};


static int nsi_conf_setopt(nsi_conf_t* p, const char* name, nai_value_t* v)
{
    int r;
    int ec;
    nsi_conf_arg_t arg;
    nsi_conf_ctx_t* ctx;
    nsi_conf_stack_t* top;
    nsi_conf_command_t* cmd;
    nai_pool_t pool;


    ctx = (nsi_conf_ctx_t*)p->ctx;
    if (ctx == 0) {
        nai_pool_init(&pool, 0);

        ctx = (nsi_conf_ctx_t*)nai_palloc(&pool, sizeof(*ctx));
        if (ctx == 0) {
            nai_log_alert(NSI_LOG_CORE, 
                nai_errno, "create memory pool failed");
            r = -1;
            goto _end;
        };

        top = (nsi_conf_stack_t*)nai_palloc(&pool, sizeof(*top));
        if (top == 0) {
            ec = nai_errno;
            nai_log_alert(NSI_LOG_CORE, 
                ec, "allocate memory failed");
            nai_pool_close(&pool);
            nai_errno = ec;
            r = -1;
            goto _end;
        };

        top->prev = 0;
        top->cmds = nsi_conf_main;
        top->data = (char*)p;
        ctx->conf = p;
        ctx->top = top;
        nai_pool_init(&ctx->pool, 0);
        nai_pool_exchange(&ctx->pool, &pool);
        p->ctx = ctx;
    };


    top = ctx->top;
    arg.ctx = ctx;
    arg.v = *v;
    arg.data = top->data;

    if (nai_strcmp(name, "begin") == 0) {
        cmd = &nsi_conf_begin;
    } else if (nai_strcmp(name, "end") == 0) {
        cmd = &nsi_conf_end;
    } else {
        cmd = nsi_conf_find_command(top->cmds, name, nai_strlen(name));
        if (cmd == 0) {
            nai_log_error(NSI_LOG_CORE, ENOENT, 
                "command '%s' is not exist", name);
            nai_errno = ENOENT;
            r = -1;
            goto _end;
        };
        if (cmd->flags & (NSI_CONF_BEGIN|NSI_CONF_END)) {
            nai_log_error(NSI_LOG_CORE, ENOENT, 
                "invalid command '%s'", name);
            nai_errno = ENOENT;
            r = -1;
            goto _end;
        };
    };

    r = cmd->handle(cmd, &arg);
    if (r < 0) {
        nai_errno = nai_sult_to_errno(r);
        r = -1;
    };


_end:
    return r;
};


static nai_sult_t nsi_conf_command_begin(
    nsi_conf_command_t* c, nsi_conf_arg_t* arg)
{
    nai_sult_t rc;
    nai_mem_t name;
    nsi_conf_ctx_t* ctx;
    nsi_conf_stack_t* top;
    nsi_conf_command_t* cmd;


    rc = nai_value_getm(&arg->v, &name);
    if (rc < 0) {
        nai_log_error(NSI_LOG_CORE, 
            nai_sult_to_errno(rc), 
            "invalid argument of command '%s'", c->name);
        goto _end;
    };


    ctx = arg->ctx;
    top = ctx->top;
    cmd = nsi_conf_find_command(
        top->cmds, nai_str(&name), nai_str_len(&name));
    if (cmd == 0) {
        rc = nai_errno_to_sult(ENOENT);
        nai_log_error(NSI_LOG_CORE, 
            nai_sult_to_errno(rc), "no valid begin command of segement.");
        goto _end;
    };
    if (!(cmd->flags & NSI_CONF_BEGIN)) {
        rc = nai_errno_to_sult(ENOENT);
        nai_log_error(NSI_LOG_CORE, nai_sult_to_errno(rc), 
            "no valid begin command of segement. without begin flags");
        goto _end;
    };


    top = (nsi_conf_stack_t*)nai_palloc(&ctx->pool, sizeof(*top));
    if (top == 0) {
        rc = nai_sult_from_errno();
        nai_log_alert(NSI_LOG_CORE, 
            nai_sult_to_errno(rc), 
            "command '%s' allocate memory failed", c->name);
        goto _end;
    };

    top->cmds = 0;
    top->data = 0;
    top->prev = ctx->top;
    ctx->top = top;


    rc = cmd->handle(cmd, arg);
    if (rc < 0) {
        ctx->top = top;
        goto _end;
    };

_end:
    return rc;
};


static nai_sult_t nsi_conf_command_end(
    nsi_conf_command_t* c, nsi_conf_arg_t* arg)
{
    nai_sult_t rc;
    nsi_conf_ctx_t* ctx;
    nsi_conf_stack_t* top;
    nsi_conf_command_t* cmd;


    (void)c;

    ctx = arg->ctx;
    top = ctx->top;
    if (top->prev == 0) {
        rc = nai_errno_to_sult(ENOENT);
        nai_log_error(NSI_LOG_CORE, 
            nai_sult_to_errno(rc), "no valid segement.");
        goto _end;
    };

    cmd = nsi_conf_find_command(top->cmds, "", 0);
    if (cmd == 0) {
        rc = nai_errno_to_sult(ENOENT);
        nai_log_error(NSI_LOG_CORE, 
            nai_sult_to_errno(rc), "no valid end command of segement.");
        goto _end;
    };
    if (!(cmd->flags & NSI_CONF_END)) {
        rc = nai_errno_to_sult(ENOENT);
        nai_log_error(NSI_LOG_CORE, 
            nai_sult_to_errno(rc), 
            "no valid end command of segement. without end flags");
        goto _end;
    };


    rc = cmd->handle(cmd, arg);
    if (rc < 0) {
        goto _end;
    };

    ctx->top = top->prev;
    rc = 0;


_end:
    return rc;
};


static nsi_conf_command_t nsi_conf_app_commands[] = {
    { "name", nsi_conf_set_string, 0, 
      nai_offsetof(nsi_conf_app_t, name) }, 
    { "client", nsi_conf_set_int16, 0, 
      nai_offsetof(nsi_conf_app_t, cid) }, 
    { "", nsi_conf_app_end, NSI_CONF_END, 0 }, 
    { 0 }, 
};


static nai_sult_t nsi_conf_app(nsi_conf_command_t* c, nsi_conf_arg_t* arg)
{
    nai_sult_t rc;
    nsi_conf_t* conf;
    nsi_conf_ctx_t* ctx;
    nsi_conf_stack_t* top;
    nsi_conf_app_t* a;


    ctx = arg->ctx;
    conf = ctx->conf;
    a = (nsi_conf_app_t*)nai_palloc(&conf->pool, sizeof(*a));
    if (a == 0) {
        rc = nai_sult_from_errno();
        nai_log_alert(NSI_LOG_CORE, 
            nai_sult_to_errno(rc), 
            "command '%s' allocate memory failed", c->name);
        goto _end;
    };

    nai_str_setn(&a->name);
    a->cid = 0;

    top = ctx->top;
    top->data = (char*)a;
    top->cmds = nsi_conf_app_commands;
    rc = 0;


_end:
    return rc;
};


static nai_sult_t nsi_conf_app_end(nsi_conf_command_t* c, nsi_conf_arg_t* arg)
{
    nai_sult_t rc;
    nai_rbnode_t** n;
    nai_rbnode_t* parent;
    nsi_conf_t* conf;
    nsi_conf_ctx_t* ctx;
    nsi_conf_app_t* a;


    (void)c;

    a = (nsi_conf_app_t*)arg->data;
    if (nai_str_len(&a->name) <= 0) {
        rc = nai_errno_to_sult(EINVAL);
        nai_log_alert(NSI_LOG_CORE, 
            nai_sult_to_errno(rc), 
            "command cannot add application without name");
        goto _end;
    }
    if (a->cid == (nsi_cid_t)-1) {
        rc = nai_errno_to_sult(EINVAL);
        nai_log_alert(NSI_LOG_CORE, 
            nai_sult_to_errno(rc), 
            "command cannot add application without client id");
        goto _end;
    };

    ctx = arg->ctx;
    conf = ctx->conf;
    n = nsi_conf_find_app(conf, &a->name, &parent);
    if (n[0] != 0) {
        rc = nai_errno_to_sult(EEXIST);
        nai_log_alert(NSI_LOG_CORE, 
            nai_sult_to_errno(rc), 
            "application(%s) already exists", nai_str(&a->name));
        goto _end;
    };

    nai_rbtree_link(&conf->apps, &a->ent, parent, n);
    nai_rbtree_color(&conf->apps, &a->ent);
    rc = 0;

_end:
    return rc;
};


static nsi_conf_command_t nsi_conf_bind_commands[] = {
    { "reliability", nsi_conf_set_option, 0, 
      nai_offsetof(nsi_conf_bind_t, reliability), 
      nsi_option_rt}, 
    { "address", nsi_conf_set_address, NSI_CONF_ADDRONLY, 
      nai_offsetof(nsi_conf_app_t, name) }, 
    { "port", nsi_conf_port, NSI_CONF_BEGIN, 0 }, 
    { "", nsi_conf_bind_end, NSI_CONF_END, 0 }, 
    { 0 }, 
};


static int nsi_conf_bind(nsi_conf_command_t* c, nsi_conf_arg_t* arg)
{
    nai_sult_t rc;
    nsi_conf_t* conf;
    nsi_conf_ctx_t* ctx;
    nsi_conf_stack_t* top;
    nsi_conf_bind_t* b;


    ctx = arg->ctx;
    conf = ctx->conf;
    b = (nsi_conf_bind_t*)nai_palloc(&conf->pool, sizeof(*b));
    if (b == 0) {
        rc = nai_sult_from_errno();
        nai_log_alert(NSI_LOG_CORE, 
            nai_sult_to_errno(rc), 
            "command '%s' allocate memory failed", c->name);
        goto _end;
    };

    b->reliability = NSI_RT_UNKNOWN;
    nai_sockname_init(&b->name);
    nai_array_init(&b->ports, sizeof(nsi_range_t), &conf->pool);

    top = ctx->top;
    top->data = (char*)b;
    top->cmds = nsi_conf_bind_commands;
    rc = 0;

_end:
    return rc;
};

static void nsi_conf_bind_port_sort(nai_array_t *port_ranges)
{
    nsi_range_t *ranges = (nsi_range_t*) port_ranges->elts;
    int cnt = (int)(port_ranges->count);
    nsi_range_t tmp;
    int i = 0;
    int j = 0;

    if (cnt <= 1) {
        return;
    }

    for(i = 0; i < cnt; i++) {
        for(j = 0; j < (cnt-i-1); j++) {
            if (ranges[j].start > ranges[j+1].start) {
                tmp = ranges[j];
                ranges[j] = ranges[j+1];
                ranges[j+1] = tmp;
            }
        }
    }
}

static int nsi_conf_bind_end(nsi_conf_command_t* c, nsi_conf_arg_t* arg)
{
    nai_sult_t rc;
    nai_rbnode_t** n;
    nai_rbnode_t* parent;
    nsi_conf_t* conf;
    nsi_conf_ctx_t* ctx;
    nsi_conf_bind_t* b;
    int reliable;
    char name[128];


    (void)c;

    b = (nsi_conf_bind_t*)arg->data;
    if (b->reliability != NSI_RT_RELIABLE && 
        b->reliability != NSI_RT_UNRELIABLE) {
        rc = nai_errno_to_sult(EINVAL);
        nai_log_alert(NSI_LOG_CORE, 
            nai_sult_to_errno(rc), 
            "command cannot add bind without a valid option 'reliability'");
        goto _end;
    };
    if (b->name.addr == 0) {
        rc = nai_errno_to_sult(EINVAL);
        nai_log_alert(NSI_LOG_CORE, 
            nai_sult_to_errno(rc), 
            "command cannot add bind "
            "without reliable or unreliable address");
        goto _end;
    };

    nsi_conf_bind_port_sort(&(b->ports));

    ctx = arg->ctx;
    conf = ctx->conf;
    reliable = b->reliability == NSI_RT_RELIABLE;
    n = nsi_conf_find_bind(conf, reliable, &b->name, &parent);
    if (n[0] != 0) {
        rc = nai_errno_to_sult(EEXIST);
        nai_sockaddr_ntop(b->name.addr, b->name.len, name, sizeof(name), 1);
        nai_log_alert(NSI_LOG_CORE, 
            nai_sult_to_errno(rc), 
            "bind(%d, %s) already exists", reliable, name);
        goto _end;
    };

    nai_rbtree_link(&conf->binds[reliable], &b->ent, parent, n);
    nai_rbtree_color(&conf->binds[reliable], &b->ent);
    nai_list_insert_tail(&conf->owns.binds, &b->node);
    rc = 0;


_end:
    return rc;
};


static nsi_conf_command_t nsi_conf_port_commands[] = {
    { "port_start", nsi_conf_set_int32, 0, 
      nai_offsetof(nsi_range_t, start), 
      nsi_option_rt}, 
    { "port_end", nsi_conf_set_int32, 0, 
      nai_offsetof(nsi_range_t, end) }, 
    { "", nsi_conf_port_end, NSI_CONF_END, 0 }, 
    { 0 }, 
};


static nai_sult_t nsi_conf_port(
    nsi_conf_command_t* c, nsi_conf_arg_t* arg)
{
    nai_sult_t rc;
    nsi_conf_ctx_t* ctx;
    nsi_conf_stack_t* top;
    nsi_conf_bind_t* b;
    nsi_range_t* p;


    ctx = arg->ctx;
    top = ctx->top->prev;
    b = (nsi_conf_bind_t*)top->data;
    p = (nsi_range_t*)nai_array_push(&b->ports);
    if (p == 0) {
        rc = nai_sult_from_errno();
        nai_log_alert(NSI_LOG_CORE, 
            nai_sult_to_errno(rc), 
            "command '%s' allocate memory failed", c->name);
        goto _end;
    };

    p->start = -1;
    p->end = -1;

    top = ctx->top;
    top->data = (char*)p;
    top->cmds = nsi_conf_port_commands;
    rc = 0;


_end:
    return rc;
};


static nai_sult_t nsi_conf_port_end(
    nsi_conf_command_t* c, nsi_conf_arg_t* arg)
{
    nai_sult_t rc;
    nsi_range_t* p;


    (void)c;

    p = (nsi_range_t*)arg->data;
    if (p->start == (uint32_t)-1) {
        p->start = 0;
    };
    if (p->end == (uint32_t)-1) {
        p->end = 65535;
    };
    if (p->start > p->end) {
        rc = nai_errno_to_sult(EINVAL);
        nai_log_alert(NSI_LOG_CORE, 
            nai_sult_to_errno(rc), 
            "the port_end should be greater than the port_start");
        goto _end;
    };

    rc = 0;

_end:
    return rc;
};


static nsi_conf_command_t nsi_conf_service_commands[] = {
    { "service", nsi_conf_set_int16, 0, 
      nai_offsetof(nsi_conf_service_t, serv) }, 
    { "instance", nsi_conf_set_int16, 0, 
      nai_offsetof(nsi_conf_service_t, inst) }, 
    { "reliability", nsi_conf_set_option, 0, 
      nai_offsetof(nsi_conf_service_t, reliability), 
      nsi_option_rt }, 
    { "major", nsi_conf_set_int8, 0, 
      nai_offsetof(nsi_conf_service_t, major) }, 
    { "minor", nsi_conf_set_int32, 0, 
      nai_offsetof(nsi_conf_service_t, minor) }, 
    { "ttl", nsi_conf_set_int32, 0, 
      nai_offsetof(nsi_conf_service_t, ttl) }, 
    { "cyclic_delay", nsi_conf_set_int32, 0, 
      nai_offsetof(nsi_conf_service_t, cyclic_delay) }, 
    { "reliable", nsi_conf_set_address, 0, 
      nai_offsetof(nsi_conf_service_t, names[1]) }, 
    { "unreliable", nsi_conf_set_address, 0, 
      nai_offsetof(nsi_conf_service_t, names[0]) }, 
    { "multicast", nsi_conf_set_address, NSI_CONF_ADDRONLY, 
      nai_offsetof(nsi_conf_service_t, names[2]) }, 
    { "event", nsi_conf_event, NSI_CONF_BEGIN, 0 }, 
    { "group", nsi_conf_group, NSI_CONF_BEGIN, 0 }, 
    { "signal", nsi_conf_event, NSI_CONF_BEGIN, 0 }, 
    { "tls_version", nsi_conf_set_int32, 0,
      nai_offsetof(nsi_conf_service_t, tls_version) },
    { "tls_verify_client", nsi_conf_set_int32, 0,
      nai_offsetof(nsi_conf_service_t, tls_verify_client) },
    { "tls_peer_ca", nsi_conf_set_string, 0,
      nai_offsetof(nsi_conf_service_t, tls_peer_ca) },
    { "tls_local_cert", nsi_conf_set_string, 0,
      nai_offsetof(nsi_conf_service_t, tls_local_cert) },
    { "tls_local_privkey", nsi_conf_set_string, 0,
      nai_offsetof(nsi_conf_service_t, tls_local_privkey) },
    { "", nsi_conf_service_end, NSI_CONF_END, 0 }, 
    { 0 }, 
};

static nsi_conf_command_t nsi_conf_service_exist_commands[] = {
    { "service", nsi_conf_set_int16, 0, 
      nai_offsetof(nsi_conf_service_t, serv) }, 
    { "instance", nsi_conf_set_int16, 0, 
      nai_offsetof(nsi_conf_service_t, inst) }, 
    { "reliability", nsi_conf_set_option, 0, 
      nai_offsetof(nsi_conf_service_t, reliability), 
      nsi_option_rt }, 
    { "major", nsi_conf_set_int8, 0, 
      nai_offsetof(nsi_conf_service_t, major) }, 
    { "minor", nsi_conf_set_int32, 0, 
      nai_offsetof(nsi_conf_service_t, minor) }, 
    { "ttl", nsi_conf_set_int32, 0, 
      nai_offsetof(nsi_conf_service_t, ttl) }, 
    { "cyclic_delay", nsi_conf_set_int32, 0, 
      nai_offsetof(nsi_conf_service_t, cyclic_delay) }, 
    { "reliable", nsi_conf_set_address, 0, 
      nai_offsetof(nsi_conf_service_t, names[1]) }, 
    { "unreliable", nsi_conf_set_address, 0, 
      nai_offsetof(nsi_conf_service_t, names[0]) }, 
    { "multicast", nsi_conf_set_address, NSI_CONF_ADDRONLY, 
      nai_offsetof(nsi_conf_service_t, names[2]) }, 
    { "event", nsi_conf_event, NSI_CONF_BEGIN, 0 }, 
    { "group", nsi_conf_group, NSI_CONF_BEGIN, 0 }, 
    { "signal", nsi_conf_event, NSI_CONF_BEGIN, 0 },
    { "tls_version", nsi_conf_set_int32, 0,
      nai_offsetof(nsi_conf_service_t, tls_version) },
    { "tls_verify_client", nsi_conf_set_int32, 0,
      nai_offsetof(nsi_conf_service_t, tls_verify_client) },
    { "tls_peer_ca", nsi_conf_set_string, 0,
      nai_offsetof(nsi_conf_service_t, tls_peer_ca) },
    { "tls_local_cert", nsi_conf_set_string, 0,
      nai_offsetof(nsi_conf_service_t, tls_local_cert) },
    { "tls_local_privkey", nsi_conf_set_string, 0,
      nai_offsetof(nsi_conf_service_t, tls_local_privkey) },
    { "", nsi_conf_service_end, NSI_CONF_END, 0 }, 
    { 0 }, 
};


static nai_sult_t nsi_conf_service(nsi_conf_command_t* c, nsi_conf_arg_t* arg)
{
    int n;
    int local;
    nai_sult_t rc;
    nsi_conf_t* conf;
    nsi_conf_ctx_t* ctx;
    nsi_conf_stack_t* top;
    nsi_conf_service_t* s;


    /* PRS_SOMEIPSD_00470 */

    ctx = arg->ctx;
    conf = ctx->conf;
    s = (nsi_conf_service_t*)nai_palloc(&conf->pool, sizeof(*s));
    if (s == 0) {
        rc = nai_sult_from_errno();
        nai_log_alert(NSI_LOG_CORE, 
            nai_sult_to_errno(rc), 
            "command '%s' allocate memory failed", c->name);
        goto _end;
    };

    local = nai_strcmp(c->name, "service") == 0;

    s->serv = -1;
    s->inst = -1;
    s->reliability = -1;
    s->major = -1;
    s->minor = -1;
    s->ttl = -1;
    s->cyclic_delay = -1;
    s->local = local;
    s->preset = 0;
    s->signal = 0;
    s->owner = 0;
    nai_rbtree_init(&s->events);
    nai_rbtree_init(&s->groups);

    for (n = 0; n < (int)nai_countof(s->names); n ++) {
        s->names[n].addr = 0;
        s->names[n].len = 0;
    };

    s->tls_version = 0;
    s->tls_verify_client = 0;
    nai_str_setn(&s->tls_peer_ca);
    nai_str_setn(&s->tls_local_cert);
    nai_str_setn(&s->tls_local_privkey);

    top = ctx->top;
    top->data = (char*)s;
    top->cmds = local ? 
        nsi_conf_service_commands : 
        nsi_conf_service_exist_commands;
    rc = 0;


_end:
    return rc;
};


static nai_sult_t nsi_conf_service_end(
    nsi_conf_command_t* c, nsi_conf_arg_t* arg)
{
    nai_sult_t rc;
    nai_rbnode_t** n;
    nai_rbnode_t* parent;
    nsi_conf_t* conf;
    nsi_conf_ctx_t* ctx;
    nsi_conf_service_t* s;


    (void)c;

    s = (nsi_conf_service_t*)arg->data;
    if (s->serv == (nsi_serv_t)-1) {
        rc = nai_errno_to_sult(EINVAL);
        nai_log_alert(NSI_LOG_CORE, 
            nai_sult_to_errno(rc), 
            "command cannot add service without service id");
        goto _end;
    };
    if (s->inst == (nsi_inst_t)-1) {
        if (s->local) {
            rc = nai_errno_to_sult(EINVAL);
            nai_log_alert(NSI_LOG_CORE, 
                nai_sult_to_errno(rc), 
                "command cannot add service without instance id");
            goto _end;
        };
    } else {
        if (s->major == (nsi_version_t)-1 || 
            s->minor == (nsi_versmin_t)-1) {
            rc = nai_errno_to_sult(EINVAL);
            nai_log_alert(NSI_LOG_CORE, 
                nai_sult_to_errno(rc), 
                "command cannot add service without major or minor version");
            goto _end;
        };

        /* PRS_SOMEIPSD_00310
         * PRS_SOMEIPSD_00319
         * PRS_SOMEIPSD_00357
         */
        if (s->names[0].addr == 0 && s->names[1].addr == 0) {
            if (s->local) {
                rc = nai_errno_to_sult(EINVAL);
                nai_log_alert(NSI_LOG_CORE, 
                    nai_sult_to_errno(rc), 
                    "command cannot add service "
                    "without reliable or unreliable address");
                goto _end;
            };
        } else {
            if (s->local == 0) {
                s->preset = 1;
            };
        };

        if (s->names[0].addr == 0 && s->names[2].addr != 0) {
            rc = nai_errno_to_sult(EINVAL);
            nai_log_alert(NSI_LOG_CORE, 
                nai_sult_to_errno(rc), 
                "command cannot add service which enable multicast "
                "without unreliable address");
            goto _end;
        };
    };
    if (s->reliability == -1) {
        s->reliability = NSI_RT_UNKNOWN;
    };

    /* use default value of discovery */
    if (s->ttl == (uint32_t)-1) {
        s->ttl = 0;
    } else if (s->ttl >= 0xffffff) {
        s->ttl = -1;
    } else {
        /* nothing */
        ;
    };
    /* use default value of discovery */
    if (s->cyclic_delay == (uint32_t)-1) {
        s->cyclic_delay = 0;
    };

    ctx = arg->ctx;
    conf = ctx->conf;
    n = nsi_conf_find_service(conf, s->serv, s->inst, &parent);
    if (n[0] != 0) {
        rc = nai_errno_to_sult(EEXIST);
        nai_log_alert(NSI_LOG_CORE, 
            nai_sult_to_errno(rc), 
            "service (%d, %d) already exists", s->serv, s->inst);
        goto _end;
    };

    nai_rbtree_link(&conf->servs, &s->ent, parent, n);
    nai_rbtree_color(&conf->servs, &s->ent);
    nai_list_insert_tail(&conf->owns.servs, &s->node);
    rc = 0;

_end:
    return rc;
};


static nsi_conf_command_t nsi_conf_event_commands[] = {
    { "event", nsi_conf_set_int16, 0, 
      nai_offsetof(nsi_conf_event_t, eid) }, 
    { "type", nsi_conf_set_option, 0, 
      nai_offsetof(nsi_conf_event_t, type), 
      nsi_option_et }, 
    { "reliability", nsi_conf_set_option, 0, 
      nai_offsetof(nsi_conf_event_t, reliability), 
      nsi_option_rt }, 
    { "", nsi_conf_event_end, NSI_CONF_END, 0 }, 
    { 0 }, 
};

static nsi_conf_command_t nsi_conf_signal_commands[] = {
    { "signal", nsi_conf_set_int16, 0, 
      nai_offsetof(nsi_conf_event_t, eid) }, 
    { "reliability", nsi_conf_set_option, 0, 
      nai_offsetof(nsi_conf_event_t, reliability), 
      nsi_option_rt }, 
    { "", nsi_conf_event_end, NSI_CONF_END, 0 }, 
    { 0 }, 
};


static nai_sult_t nsi_conf_event(
    nsi_conf_command_t* c, nsi_conf_arg_t* arg)
{
    nai_sult_t rc;
    nsi_conf_t* conf;
    nsi_conf_ctx_t* ctx;
    nsi_conf_stack_t* top;
    nsi_conf_event_t* e;


    ctx = arg->ctx;
    conf = ctx->conf;
    e = (nsi_conf_event_t*)nai_palloc(&conf->pool, sizeof(*e));
    if (e == 0) {
        rc = nai_sult_from_errno();
        nai_log_alert(NSI_LOG_CORE, 
            nai_sult_to_errno(rc), 
            "command '%s' allocate memory failed", c->name);
        goto _end;
    };

    e->eid = -1;
    e->type = nai_strcmp(c->name, "signal") == 0 ? NSI_ET_SIGNAL : -1;
    e->reliability = -1;

    top = ctx->top;
    top->data = (char*)e;
    top->cmds = e->type == -1 ? 
        nsi_conf_event_commands : 
        nsi_conf_signal_commands;

    rc = 0;


_end:
    return rc;
};


static nai_sult_t nsi_conf_event_end(
    nsi_conf_command_t* c, nsi_conf_arg_t* arg)
{
    nai_sult_t rc;
    nai_rbnode_t** n;
    nai_rbnode_t* parent;
    nsi_conf_ctx_t* ctx;
    nsi_conf_stack_t* top;
    nsi_conf_event_t* e;
    nsi_conf_service_t* s;


    (void)c;

    e = (nsi_conf_event_t*)arg->data;
    if (e->eid == (nsi_eid_t)-1) {
        rc = nai_errno_to_sult(EINVAL);
        nai_log_alert(NSI_LOG_CORE, 
            nai_sult_to_errno(rc), 
            "command cannot add event without event id");
        goto _end;
    };
    if (e->type == -1) {
        e->type = NSI_ET_UNKNOWN;
    };
    if (e->reliability == -1) {
        e->reliability = NSI_RT_UNKNOWN;
    };

    /* add to service */
    ctx = arg->ctx;
    top = ctx->top->prev;
    s = (nsi_conf_service_t*)top->data;
    n = nsi_conf_find_event(s, e->eid, &parent);
    if (n[0] != 0) {
        rc = nai_errno_to_sult(EEXIST);
        nai_log_alert(NSI_LOG_CORE, 
            nai_sult_to_errno(rc), 
            "event %d already exists", e->eid);
        goto _end;
    };

    nai_rbtree_link(&s->events, &e->ent, parent, n);
    nai_rbtree_color(&s->events, &e->ent);
    rc = 0;

_end:
    return rc;
};


static nsi_conf_command_t nsi_conf_group_commands[] = {
    { "group", nsi_conf_set_int16, 0, 
      nai_offsetof(nsi_conf_group_t, gid) }, 
    { "threshold", nsi_conf_set_int32, 0, 
      nai_offsetof(nsi_conf_group_t, threshold) }, 
    { "ttl", nsi_conf_set_int32, 0, 
      nai_offsetof(nsi_conf_group_t, ttl) }, 
    { "subscribe_timeo", nsi_conf_set_int32, 0, 
      nai_offsetof(nsi_conf_group_t, subscribe_timeo) }, 
    { "subscribe_renew", nsi_conf_set_int32, 0, 
      nai_offsetof(nsi_conf_group_t, subscribe_renew) }, 
    { "multicast", nsi_conf_set_address, NSI_CONF_ADDRONLY, 
      nai_offsetof(nsi_conf_group_t, mcast) }, 
    { "subs", nsi_conf_subs, NSI_CONF_BEGIN, 0 }, 
    { "", nsi_conf_group_end, NSI_CONF_END, 0 }, 
    { 0 }, 
};


static nai_sult_t nsi_conf_group(
    nsi_conf_command_t* c, nsi_conf_arg_t* arg)
{
    nai_sult_t rc;
    nsi_conf_t* conf;
    nsi_conf_ctx_t* ctx;
    nsi_conf_stack_t* top;
    nsi_conf_group_t* g;


    ctx = arg->ctx;
    conf = ctx->conf;
    g = (nsi_conf_group_t*)nai_palloc(&conf->pool, sizeof(*g));
    if (g == 0) {
        rc = nai_sult_from_errno();
        nai_log_alert(NSI_LOG_CORE, 
            nai_sult_to_errno(rc), 
            "command '%s' allocate memory failed", c->name);
        goto _end;
    };

    nai_array_init(&g->subs, sizeof(nsi_conf_subs_t), &conf->pool);
    g->gid = -1;
    g->threshold = -1;
    g->ttl = -1;
    g->subscribe_timeo = -1;
    g->subscribe_renew = -1;
    g->mcast.addr = 0;
    g->mcast.len = 0;

    top = ctx->top;
    top->data = (char*)g;
    top->cmds = nsi_conf_group_commands;
    rc = 0;


_end:
    return rc;
};


static nai_sult_t nsi_conf_group_end(
    nsi_conf_command_t* c, nsi_conf_arg_t* arg)
{
    nai_sult_t rc;
    nai_rbnode_t** n;
    nai_rbnode_t* parent;
    nsi_conf_ctx_t* ctx;
    nsi_conf_stack_t* top;
    nsi_conf_group_t* g;
    nsi_conf_service_t* s;


    (void)c;

    g = (nsi_conf_group_t*)arg->data;
    if (g->gid == (nsi_gid_t)-1) {
        rc = nai_errno_to_sult(EINVAL);
        nai_log_alert(NSI_LOG_CORE, 
            nai_sult_to_errno(rc), 
            "command cannot add group without group id");
        goto _end;
    };
    if ((int)g->threshold < -1) {
        rc = nai_errno_to_sult(EINVAL);
        nai_log_alert(NSI_LOG_CORE, 
            nai_sult_to_errno(rc), 
            "command cannot add group with threshold=%d", g->threshold);
        goto _end;
    } else if ((int)g->threshold <= 0) {
        g->threshold = NAI_INT32_T_MAX;
    } else {
        ;
    };
    if (g->ttl == (uint32_t)-1) {
        g->ttl = 0; /* use default */
    } else if (g->ttl >= 0xffffff) {
        g->ttl = -1;
    } else {
        /* nothing */
        ;
    };

    /* add to service */
    ctx = arg->ctx;
    top = ctx->top->prev;
    s = (nsi_conf_service_t*)top->data;
    n = nsi_conf_find_group(s, g->gid, &parent);
    if (n[0] != 0) {
        rc = nai_errno_to_sult(EEXIST);
        nai_log_alert(NSI_LOG_CORE, 
            nai_sult_to_errno(rc), 
            "group %d already exists", g->gid);
        goto _end;
    };

    nai_rbtree_link(&s->groups, &g->ent, parent, n);
    nai_rbtree_color(&s->groups, &g->ent);
    rc = 0;

_end:
    return rc;
};


static nsi_conf_command_t nsi_conf_subs_commands[] = {
    { "reliable", nsi_conf_set_address, NSI_CONF_ADDRONLY, 
      nai_offsetof(nsi_conf_subs_t, names[1]) }, 
    { "unreliable", nsi_conf_set_address, NSI_CONF_ADDRONLY, 
      nai_offsetof(nsi_conf_subs_t, names[0]) }, 
    { "", nsi_conf_subs_end, NSI_CONF_END, 0 }, 
    { 0 }, 
};


static nai_sult_t nsi_conf_subs(
    nsi_conf_command_t* c, nsi_conf_arg_t* arg)
{
    nai_sult_t rc;
    nsi_conf_ctx_t* ctx;
    nsi_conf_stack_t* top;
    nsi_conf_group_t* g;
    nsi_conf_subs_t* u;
    nsi_conf_service_t* s;


    ctx = arg->ctx;
    top = ctx->top;
    s = (nsi_conf_service_t*)top->prev->prev->data;
    if (!s->local) {
        rc = nai_errno_to_sult(ENOENT);
        nai_log_alert(NSI_LOG_CORE, 
            nai_sult_to_errno(rc), 
            "command '%s' is invalid on a remote service", c->name);
        goto _end;
    };

    g = (nsi_conf_group_t*)top->prev->data;
    u = (nsi_conf_subs_t*)nai_array_push(&g->subs);
    if (u == 0) {
        rc = nai_sult_from_errno();
        nai_log_alert(NSI_LOG_CORE, 
            nai_sult_to_errno(rc), 
            "command '%s' allocate memory failed", c->name);
        goto _end;
    };

    nai_sockname_init(&u->names[0]);
    nai_sockname_init(&u->names[1]);

    top->data = (char*)u;
    top->cmds = nsi_conf_subs_commands;
    rc = 0;

_end:
    return rc;
};


static nai_sult_t nsi_conf_subs_end(
    nsi_conf_command_t* c, nsi_conf_arg_t* arg)
{
    nai_sult_t rc;
    nsi_conf_subs_t* u;


    (void)c;

    u = (nsi_conf_subs_t*)arg->data;
    if (u->names[0].addr == 0 && u->names[1].addr == 0) {
        rc = nai_errno_to_sult(EINVAL);
        nai_log_alert(NSI_LOG_CORE, 
            nai_sult_to_errno(rc), 
            "command cannot add subscription "
            "without reliable and unreliable address");
        goto _end;
    };

    rc = 0;

_end:
    return rc;
};


static nsi_conf_command_t nsi_conf_local_commands[] = {
    { "serv_start", nsi_conf_set_int32, 0, 
      nai_offsetof(nsi_conf_local_t, serv.start) }, 
    { "serv_end", nsi_conf_set_int32, 0, 
      nai_offsetof(nsi_conf_local_t, serv.end) }, 
    { "inst_start", nsi_conf_set_int32, 0, 
      nai_offsetof(nsi_conf_local_t, inst.start) }, 
    { "inst_end", nsi_conf_set_int32, 0, 
      nai_offsetof(nsi_conf_local_t, inst.end) }, 
    { "", nsi_conf_local_end, NSI_CONF_END, 0 }, 
    { 0 }, 
};


static nai_sult_t nsi_conf_local(
    nsi_conf_command_t* c, nsi_conf_arg_t* arg)
{
    nai_sult_t rc;
    nsi_conf_t* conf;
    nsi_conf_ctx_t* ctx;
    nsi_conf_stack_t* top;
    nsi_conf_local_t* l;


    ctx = arg->ctx;
    conf = ctx->conf;
    l = (nsi_conf_local_t*)nai_array_push(&conf->locals);
    if (l == 0) {
        rc = nai_sult_from_errno();
        nai_log_alert(NSI_LOG_CORE, 
            nai_sult_to_errno(rc), 
            "command '%s' allocate memory failed", c->name);
        goto _end;
    };

    l->serv.start = -1;
    l->serv.end = -1;
    l->inst.start = -1;
    l->inst.start = -1;

    top = ctx->top;
    top->data = (char*)l;
    top->cmds = nsi_conf_local_commands;
    rc = 0;

_end:
    return rc;
};


static nai_sult_t nsi_conf_local_end(
    nsi_conf_command_t* c, nsi_conf_arg_t* arg)
{
    nai_sult_t rc;
    nsi_conf_local_t* l;


    (void)c;

    l = (nsi_conf_local_t*)arg->data;
    if (l->serv.start == (uint32_t)-1) {
        l->serv.start = 0;
    };
    if (l->serv.end == (uint32_t)-1) {
        l->serv.end = 65535;
    };
    if (l->inst.start == (uint32_t)-1) {
        l->inst.start = 0;
    };
    if (l->inst.end == (uint32_t)-1) {
        l->inst.end = 65535;
    };
    if (l->serv.start > l->serv.end) {
        rc = nai_errno_to_sult(EINVAL);
        nai_log_alert(NSI_LOG_CORE, 
            nai_sult_to_errno(rc), 
            "the serv_end should be greater than the serv_start");
        goto _end;
    };
    if (l->inst.start > l->inst.end) {
        rc = nai_errno_to_sult(EINVAL);
        nai_log_alert(NSI_LOG_CORE, 
            nai_sult_to_errno(rc), 
            "the inst_end should be greater than the inst_start");
        goto _end;
    };

    rc = 0;

_end:
    return rc;
};


static nsi_conf_command_t nsi_conf_discovery_commands[] = {
    { "unicast", nsi_conf_set_address, 0, 
      nai_offsetof(nsi_conf_discovery_t, ucast) }, 
    { "multicast", nsi_conf_set_address, NSI_CONF_ADDRONLY, 
      nai_offsetof(nsi_conf_discovery_t, mcast) }, 
    { "init_delay_min", nsi_conf_set_int32, 0, 
      nai_offsetof(nsi_conf_discovery_t, init_delay_min) }, 
    { "init_delay_max", nsi_conf_set_int32, 0, 
      nai_offsetof(nsi_conf_discovery_t, init_delay_max) }, 
    { "request_response_delay_min", nsi_conf_set_int32, 0,
      nai_offsetof(nsi_conf_discovery_t, request_response_delay_min) },
    { "request_response_delay_max", nsi_conf_set_int32, 0,
      nai_offsetof(nsi_conf_discovery_t, request_response_delay_max) },
    { "offer_multicast_optim", nsi_conf_set_int32, 0,
      nai_offsetof(nsi_conf_discovery_t, offer_multicast_optim) },
    { "repetitions_delay", nsi_conf_set_int32, 0, 
      nai_offsetof(nsi_conf_discovery_t, repetitions_delay) }, 
    { "repetitions_max", nsi_conf_set_int32, 0, 
      nai_offsetof(nsi_conf_discovery_t, repetitions_max) }, 
    { "ttl", nsi_conf_set_int32, 0, 
      nai_offsetof(nsi_conf_discovery_t, ttl) }, 
    { "offer_cyclic_delay", nsi_conf_set_int32, 0, 
      nai_offsetof(nsi_conf_discovery_t, offer_cyclic_delay) }, 
    { "offer_debounce", nsi_conf_set_int32, 0, 
      nai_offsetof(nsi_conf_discovery_t, offer_debounce) }, 
    { "find_debounce", nsi_conf_set_int32, 0, 
      nai_offsetof(nsi_conf_discovery_t, find_debounce) }, 
    { "subscribe_ttl", nsi_conf_set_int32, 0, 
      nai_offsetof(nsi_conf_discovery_t, subscribe_ttl) }, 
    { "subscribe_timeo", nsi_conf_set_int32, 0, 
      nai_offsetof(nsi_conf_discovery_t, subscribe_timeo) }, 
    { "subscribe_renew", nsi_conf_set_int32, 0, 
      nai_offsetof(nsi_conf_discovery_t, subscribe_renew) }, 
    { "debug", nsi_conf_set_int32, 0, 
      nai_offsetof(nsi_conf_discovery_t, enable) }, 
    { "version", nsi_conf_set_int32, 0, 
      nai_offsetof(nsi_conf_discovery_t, version) }, 
    { "", nsi_conf_discovery_end, NSI_CONF_END, 0 }, 
    { 0 }, 
};


static nai_sult_t nsi_conf_discovery(
    nsi_conf_command_t* c, nsi_conf_arg_t* arg)
{
    nai_sult_t rc;
    nsi_conf_t* conf;
    nsi_conf_ctx_t* ctx;
    nsi_conf_stack_t* top;


    ctx = arg->ctx;
    conf = ctx->conf;
    if (conf->sd.enable) {
        rc = nai_errno_to_sult(EEXIST);
        nai_log_error(NSI_LOG_CORE, 
            nai_sult_to_errno(rc), 
            "duplicated of command '%s'", c->name);
        goto _end;
    };

    top = ctx->top;
    top->data = (char*)&conf->sd;
    top->cmds = nsi_conf_discovery_commands;
    rc = 0;

_end:
    return rc;
};


static nai_sult_t nsi_conf_discovery_end(
    nsi_conf_command_t* c, nsi_conf_arg_t* arg)
{
    nai_sult_t rc;
    nsi_conf_discovery_t* d;


    (void)c;

    d = (nsi_conf_discovery_t*)arg->data;
    if (d->ucast.addr == 0) {
        rc = nai_errno_to_sult(EINVAL);
        nai_log_alert(NSI_LOG_CORE, 
            nai_sult_to_errno(rc), 
            "service discovery requires a valid bound address");
        goto _end;
    };

    if (d->init_delay_min == (uint32_t)-1) {
        d->init_delay_min = 0;
    };
    if (d->init_delay_max == (uint32_t)-1) {
        d->init_delay_max = 3000; /* 3000 ms */
    };
    if (d->request_response_delay_min == (uint32_t)-1) {
        d->request_response_delay_min = 5;
    };
    if (d->request_response_delay_max == (uint32_t)-1) {
        d->request_response_delay_max = 10;
    };
    if (d->offer_multicast_optim == (uint32_t)-1) {
        d->offer_multicast_optim = 0;
    };
    if (d->repetitions_delay == (uint32_t)-1) {
        d->repetitions_delay = 10; /* 10 ms */
    };
    if (d->repetitions_max == (uint32_t)-1) {
        d->repetitions_max = 3;
    };
    if (d->ttl == (uint32_t)-1 || d->ttl >= 0xffffff) {
        d->ttl = -1;
    };
    if (d->offer_cyclic_delay == (uint32_t)-1) {
        d->offer_cyclic_delay = 1000;
    };
    if (d->offer_debounce == (uint32_t)-1) {
        d->offer_debounce = 500;
    };
    if (d->find_debounce == (uint32_t)-1) {
        d->find_debounce = 500;
    };
    if (d->subscribe_ttl == (uint32_t)-1 || d->subscribe_ttl >= 0xffffff) {
        d->subscribe_ttl = -1;
    };
    if (d->subscribe_timeo == (uint32_t)-1) {
        d->subscribe_timeo = 5000;
    };
    if (d->subscribe_renew == (uint32_t)-1) {
        d->subscribe_renew = 5;
    };

    if (d->init_delay_max < d->init_delay_min) {
        rc = nai_errno_to_sult(EINVAL);
        nai_log_alert(NSI_LOG_CORE, 
            nai_sult_to_errno(rc), 
            "the maximum initial delay of service discovery "
            "should be greater than the minimum");
        goto _end;
    };

    if (d->request_response_delay_max < d->request_response_delay_min) {
        rc = nai_errno_to_sult(EINVAL);
        nai_log_alert(NSI_LOG_CORE,
            nai_sult_to_errno(rc),
            "the maximum initial value of request_response_delay "
            "should be greater than the minimum");
        goto _end;
    };

    if (d->enable == 0) {
        d->enable = 1;      /* normal mode */
    } else {
        d->enable = 2;      /* debug mode, use unicast instead multicast */
    };

    if (d->version != 2311) {
        d->version = 2011;
    };

    rc = 0;

_end:
    return rc;
};


static nsi_conf_command_t nsi_conf_network_commands[] = {
    { "mss", nsi_conf_set_int32, 0, 
      nai_offsetof(nsi_conf_network_t, mss) }, 
    { "recv_mem", nsi_conf_set_int32, 0, 
      nai_offsetof(nsi_conf_network_t, recv_mem) }, 
    { "recv_mem_hwm", nsi_conf_set_int32, 0, 
      nai_offsetof(nsi_conf_network_t, recv_mem_hwm) }, 
    { "recv_mem_lwm", nsi_conf_set_int32, 0, 
      nai_offsetof(nsi_conf_network_t, recv_mem_lwm) }, 
    { "recv_buf_udp", nsi_conf_set_int32, 0, 
      nai_offsetof(nsi_conf_network_t, recv_buf_udp) }, 
    { "recv_buf_tcp", nsi_conf_set_int32, 0, 
      nai_offsetof(nsi_conf_network_t, recv_buf_tcp) }, 
    { "send_buf_udp", nsi_conf_set_int32, 0, 
      nai_offsetof(nsi_conf_network_t, send_buf_udp) }, 
    { "send_buf_tcp", nsi_conf_set_int32, 0, 
      nai_offsetof(nsi_conf_network_t, send_buf_tcp) }, 
    { "recv_timeo", nsi_conf_set_int32, 0, 
      nai_offsetof(nsi_conf_network_t, recv_timeo) }, 
    { "send_timeo", nsi_conf_set_int32, 0, 
      nai_offsetof(nsi_conf_network_t, send_timeo) }, 
    { "recv_segs", nsi_conf_set_int32, 0, 
      nai_offsetof(nsi_conf_network_t, recv_segs) }, 
    { "send_segs", nsi_conf_set_int32, 0, 
      nai_offsetof(nsi_conf_network_t, send_segs) }, 
    { "queue_limit", nsi_conf_set_int32, 0, 
      nai_offsetof(nsi_conf_network_t, queue_limit) }, 
    { "queue_mix_limit", nsi_conf_set_int32, 0, 
      nai_offsetof(nsi_conf_network_t, queue_mix_limit) }, 
    { "msg_limit", nsi_conf_set_int32, 0, 
      nai_offsetof(nsi_conf_network_t, msg_limit) }, 
    { "tpm_limit", nsi_conf_set_int32, 0, 
      nai_offsetof(nsi_conf_network_t, tpm_limit) }, 
    { "tpm_max", nsi_conf_set_int32, 0, 
      nai_offsetof(nsi_conf_network_t, tpm_max) }, 
    { "use_polling", nsi_conf_set_int32, 0, 
      nai_offsetof(nsi_conf_network_t, use_polling) }, 
    { "use_zerocopy", nsi_conf_set_int32, 0, 
      nai_offsetof(nsi_conf_network_t, use_zerocopy) }, 
    { "use_npc", nsi_conf_set_int32, 0, 
      nai_offsetof(nsi_conf_network_t, use_npc) }, 
    { "npc_segcount", nsi_conf_set_int32, 0, 
      nai_offsetof(nsi_conf_network_t, npc_segcount) }, 
    { "npc_segsize", nsi_conf_set_int32, 0, 
      nai_offsetof(nsi_conf_network_t, npc_segsize) }, 
    { "npc_shm_path", nsi_conf_set_string, 0, 
      nai_offsetof(nsi_conf_network_t, npc_shm_path) }, 
    { "tcp_keepalive_enable", nsi_conf_set_int32, 0,
      nai_offsetof(nsi_conf_network_t, keepalive_enable) },
    { "tcp_keepalive_idle", nsi_conf_set_int32, 0,
      nai_offsetof(nsi_conf_network_t, keepalive_idle) },
    { "tcp_keepalive_interval", nsi_conf_set_int32, 0,
      nai_offsetof(nsi_conf_network_t, keepalive_interval) },
    { "tcp_keepalive_cnt", nsi_conf_set_int32, 0,
      nai_offsetof(nsi_conf_network_t, keepalive_cnt) },
    { "", nsi_conf_network_end, NSI_CONF_END, 0 }, 
    { 0 }, 
};


static nai_sult_t nsi_conf_network(
    nsi_conf_command_t* c, nsi_conf_arg_t* arg)
{
    nai_sult_t rc;
    nsi_conf_t* conf;
    nsi_conf_ctx_t* ctx;
    nsi_conf_stack_t* top;


    ctx = arg->ctx;
    conf = ctx->conf;
    if (conf->net.seted) {
        rc = nai_errno_to_sult(EEXIST);
        nai_log_error(NSI_LOG_CORE, 
            nai_sult_to_errno(rc), 
            "duplicated of command '%s'", c->name);
        goto _end;
    };

    top = ctx->top;
    top->data = (char*)&conf->net;
    top->cmds = nsi_conf_network_commands;
    rc = 0;

_end:
    return rc;
};


#define nsi_conf_fix_min(v, def, min, nil) {            \
    if ((v) == (uint32_t)-1) {                          \
        (v) = (def);                                    \
    } else if ((v) == 0) {                              \
        (v) = (nil);                                    \
    } else if ((v) < (min)) {                           \
        (v) = (min);                                    \
    } else {                                            \
        ;                                               \
    };                                                  \
}                                                       \


#define nsi_conf_fix_max(v, def, max, nil) {            \
    if ((v) == (uint32_t)-1) {                          \
        (v) = (def);                                    \
    } else if ((v) == 0) {                              \
        (v) = (nil);                                    \
    } else if ((v) > (max)) {                           \
        (v) = (max);                                    \
    } else {                                            \
        ;                                               \
    };                                                  \
}                                                       \


static nai_sult_t nsi_conf_network_end(
    nsi_conf_command_t* c, nsi_conf_arg_t* arg)
{
    uint32_t v;
    nai_sult_t rc;
    nsi_conf_network_t* n;


    (void)c;

    n = (nsi_conf_network_t*)arg->data;
    if (n->mss != (uint32_t)-1 && n->mss > 64*1024 - 60) {
        rc = nai_errno_to_sult(EINVAL);
        nai_log_alert(NSI_LOG_CORE, 
            nai_sult_to_errno(rc), 
            "mss(%d) too large, maxitum value of mss is %d", 
            n->mss, 64*1024 - 60);
        goto _end;
    };
    if (n->mss == 0) {
        rc = nai_errno_to_sult(EINVAL);
        nai_log_alert(NSI_LOG_CORE, 
            nai_sult_to_errno(rc), "mss can't be set to zero");
        goto _end;
    };

    nsi_conf_fix_min(n->queue_limit, 
        16*1024*1024, 4*1024, NAI_INT_T_MAX/2);
    nsi_conf_fix_min(n->queue_mix_limit, 
        32*1024*1024, 8*1024, NAI_INT_T_MAX/2);
    nsi_conf_fix_min(n->msg_limit, 
        32*1024*1024, 8*1024*1024, NAI_INT_T_MAX/2);
    nsi_conf_fix_min(n->tpm_limit, 
        32*1024*1024, 8*1024*1024, NAI_INT_T_MAX/2);
    nsi_conf_fix_min(n->tpm_max, 256, 4, NAI_INT_T_MAX/2);
    nsi_conf_fix_min(n->recv_mem_hwm, 256, 64, 64);
    nsi_conf_fix_min(n->recv_mem_lwm, 192, 64, 64);
    nsi_conf_fix_max(n->recv_segs, 0, 128, 0);
    nsi_conf_fix_max(n->send_segs, 0, 32, 0);

    if (n->recv_mem_hwm < n->recv_mem_lwm) {
        v = n->recv_mem_hwm;
        n->recv_mem_hwm = n->recv_mem_lwm;
        n->recv_mem_lwm = v;
    };
    if (n->recv_mem == (uint32_t)-1) {
        n->recv_mem = 64*1024;
    };
    if (n->use_polling == (uint32_t)-1) {
        n->use_polling = 1;
    };
    if (n->use_zerocopy == (uint32_t)-1) {
        n->use_zerocopy = 0;
    };
    if (n->use_npc == (uint32_t)-1) {
        n->use_npc = 0;
    };
    if (n->npc_segcount == (uint32_t)-1) {
        n->npc_segcount = 1;
    };
    if (n->npc_segsize == (uint32_t)-1) {
        n->npc_segsize = 10*1024*1024;
    };

    if (n->use_npc == 1 && nai_str_len(&n->npc_shm_path) == 0) {
        n->use_npc = 0;
    };
    if (n->keepalive_enable == (uint32_t)-1) {
        n->keepalive_enable = 0;
    } else if (n->keepalive_enable > 0) {
        n->keepalive_enable = 1;
    } else {
        n->keepalive_enable = 0;
    }
    if (n->keepalive_idle == (uint32_t)-1) {
        n->keepalive_idle = 60;
    };
    if (n->keepalive_interval == (uint32_t)-1) {
        n->keepalive_interval = 5;
    };
    if (n->keepalive_cnt == (uint32_t)-1) {
        n->keepalive_cnt = 5;
    };

    n->seted = 1;
    rc = 0;

_end:
    return rc;
};


static nsi_conf_command_t nsi_conf_compatible_commands[] = {
    { "use_sd_option", nsi_conf_set_int32, 0, 
      nai_offsetof(nsi_conf_compatible_t, use_sd_option) }, 
    { "", nsi_conf_compatible_end, NSI_CONF_END, 0 }, 
    { 0 }, 
};


static nai_sult_t nsi_conf_compatible(
    nsi_conf_command_t* c, nsi_conf_arg_t* arg)
{
    nai_sult_t rc;
    nsi_conf_t* conf;
    nsi_conf_ctx_t* ctx;
    nsi_conf_stack_t* top;


    (void)c;

    ctx = arg->ctx;
    conf = ctx->conf;

    top = ctx->top;
    top->data = (char*)&conf->cp;
    top->cmds = nsi_conf_compatible_commands;
    rc = 0;

    return rc;
};


static nai_sult_t nsi_conf_compatible_end(
    nsi_conf_command_t* c, nsi_conf_arg_t* arg)
{
    nai_sult_t rc;


    (void)c;
    (void)arg;

    rc = 0;

    return rc;
};


//////////////////////////////////////////////////////////////////////////////
// configuation serialize


static int nsi_conf_deserialize_u1(
    nsi_conf_t* p, nsi_message_io_t* m, const char* name)
{
    int r;
    uint8_t v;


    r = nsi_message_get_u1(m, &v);
    if (r < 0) {
        nai_log_error(NSI_LOG_CORE, nai_errno, 
            "failed to read variable '%s' of type u1.", name);
        goto _end;
    };

    r = nsi_conf_setopti(p, name, v);
    if (r < 0) {
        nai_log_error(NSI_LOG_CORE, nai_errno, 
            "failed to set option '%s' and value is %u", name, v);
        goto _end;
    };

_end:
    return r;
};

static int nsi_conf_deserialize_u2(
    nsi_conf_t* p, nsi_message_io_t* m, const char* name)
{
    int r;
    uint16_t v;


    r = nsi_message_get_u2(m, &v);
    if (r < 0) {
        nai_log_error(NSI_LOG_CORE, nai_errno, 
            "failed to read variable '%s' of type u2.", name);
        goto _end;
    };

    r = nsi_conf_setopti(p, name, v);
    if (r < 0) {
        nai_log_error(NSI_LOG_CORE, nai_errno, 
            "failed to set option '%s' and value is %u", name, v);
        goto _end;
    };

_end:
    return r;
};

static int nsi_conf_deserialize_u4(
    nsi_conf_t* p, nsi_message_io_t* m, const char* name)
{
    int r;
    uint32_t v;


    r = nsi_message_get_u4(m, &v);
    if (r < 0) {
        nai_log_error(NSI_LOG_CORE, nai_errno, 
            "failed to read variable '%s' of type u4.", name);
        goto _end;
    };

    r = nsi_conf_setopti(p, name, v);
    if (r < 0) {
        nai_log_error(NSI_LOG_CORE, nai_errno, 
            "failed to set option '%s' and value is %u", name, v);
        goto _end;
    };

_end:
    return r;
};

static int nsi_conf_deserialize_service(
    nsi_conf_t* p, nsi_message_io_t* m)
{
    int r;
    uint16_t id;
    uint16_t ii;
    uint8_t local;


    r = nsi_message_get_u2(m, &id);
    if (r < 0) {
        nai_log_error(NSI_LOG_CORE, 
            nai_errno, "failed to read service id.");
        goto _end;
    };

    if (id == (uint16_t)-1) {
        /* end mark */
        r = 0;
        goto _end;
    }

    r = nsi_message_get_u2(m, &ii);
    if (r < 0) {
        nai_log_error(NSI_LOG_CORE, 
            nai_errno, "failed to read instance id.");
        goto _end;
    };

    r = nsi_message_get_u1(m, &local);
    if (r < 0) {
        nai_log_error(NSI_LOG_CORE, 
            nai_errno, "failed to read option 'local'.");
        goto _end;
    };


    if (local) {
        nai_log_debug(NSI_LOG_CORE, 0, 
            "deserialize service(%d, %d).", id, ii);

        r = nsi_conf_setopts(p, "begin", "service");
    } else {
        nai_log_debug(NSI_LOG_CORE, 0, 
            "deserialize service_exist(%d).", id);

        r = nsi_conf_setopts(p, "begin", "service_exist");
    };
    if (r < 0) {
        nai_log_error(NSI_LOG_CORE, 
            nai_errno, "failed to begin %s.", 
            ii != NSI_INSTANCE_ANY ? "service" : "service_exist");
        goto _end;
    };


    r = nsi_conf_setopti(p, "service", id);
    if (r < 0) {
        nai_log_error(NSI_LOG_CORE, nai_errno, 
            "failed to set option 'service' and value is %u.", id);
        goto _end;
    };

    r = nsi_conf_setopti(p, "instance", ii);
    if (r < 0) {
        nai_log_error(NSI_LOG_CORE, nai_errno, 
            "failed to set option 'instance' and value is %u.", ii);
        goto _end;
    };

    r = 1;

_end:
    return r;
};

static int nsi_conf_deserialize_object(
    nsi_conf_t* p, nsi_message_io_t* m, const char* name, const char* name_id)
{
    int r;
    uint16_t id;


    r = nsi_message_get_u2(m, &id);
    if (r < 0) {
        nai_log_error(NSI_LOG_CORE, 
            nai_errno, "failed to read %s id.", name);
        goto _end;
    };

    if (id == (uint16_t)0) {
        /* end mark */
        r = 0;
        goto _end;
    };

    nai_log_debug(NSI_LOG_CORE, 0, 
        "deserialize %s with id %d.", name, id);

    r = nsi_conf_setopts(p, "begin", name);
    if (r < 0) {
        nai_log_error(NSI_LOG_CORE, 
            nai_errno, "failed to begin %s.", name);
        goto _end;
    };

    r = nsi_conf_setopti(p, name_id, id);
    if (r < 0) {
        nai_log_error(NSI_LOG_CORE, nai_errno, 
            "failed to set option '%s' and value is %u.", name_id, id);
        goto _end;
    };

    r = 1;

_end:
    return r;
};

static int nsi_conf_deserialize_object_noid(
    nsi_conf_t* p, nsi_message_io_t* m, const char* name)
{
    int r;
    uint16_t id;


    r = nsi_message_get_u2(m, &id);
    if (r < 0) {
        nai_log_error(NSI_LOG_CORE, 
            nai_errno, "failed to read %s id.", name);
        goto _end;
    };

    if (id == (uint16_t)0) {
        /* end mark */
        r = 0;
        goto _end;
    };

    nai_log_debug(NSI_LOG_CORE, 0, 
        "deserialize %s with id %d.", name, id);

    r = nsi_conf_setopts(p, "begin", name);
    if (r < 0) {
        nai_log_error(NSI_LOG_CORE, 
            nai_errno, "failed to begin %s.", name);
        goto _end;
    };

    r = 1;

_end:
    return r;
};

static int nsi_conf_deserialize_bind(
    nsi_conf_t* p, nsi_message_io_t* m)
{
    return nsi_conf_deserialize_object_noid(p, m, "bind");
};

static int nsi_conf_deserialize_port(
    nsi_conf_t* p, nsi_message_io_t* m)
{
    return nsi_conf_deserialize_object_noid(p, m, "port");
};

static int nsi_conf_deserialize_group(
    nsi_conf_t* p, nsi_message_io_t* m)
{
    return nsi_conf_deserialize_object(p, m, "group", "group");
};

static int nsi_conf_deserialize_event(
    nsi_conf_t* p, nsi_message_io_t* m)
{
    return nsi_conf_deserialize_object(p, m, "event", "event");
};

static int nsi_conf_deserialize_signal(
    nsi_conf_t* p, nsi_message_io_t* m)
{
    return nsi_conf_deserialize_object(p, m, "signal", "signal");
};

static int nsi_conf_deserialize_subs(
    nsi_conf_t* p, nsi_message_io_t* m)
{
    return nsi_conf_deserialize_object_noid(p, m, "subs");
};

static int nsi_conf_deserialize_type(
    nsi_conf_t* p, nsi_message_io_t* m, const char* name)
{
    int r;
    uint8_t v;
    const char* value;


    r = nsi_message_get_u1(m, &v);
    if (r < 0) {
        nai_log_error(NSI_LOG_CORE, 
            nai_errno, "failed to read event type.");
        goto _end;
    };

    switch (v) {
    case NSI_ET_UNKNOWN:
        value = "unknown";
        break;
    case NSI_ET_EVENT:
        value = "event";
        break;
    case NSI_ET_FIELD:
        value = "field";
        break;
    case NSI_ET_SIGNAL:
        value = "signal";
        break;
    default:
        nai_log_error(NSI_LOG_CORE, 
            EINVAL, "read an invalid event type %d.", v);
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };
    r = nsi_conf_setopts(p, name, value);
    if (r < 0) {
        nai_log_error(NSI_LOG_CORE, nai_errno, 
            "failed to set option '%s' and value is '%s'.", name, value);
        goto _end;
    };

_end:
    return r;
};

static int nsi_conf_deserialize_reliability(
    nsi_conf_t* p, nsi_message_io_t* m, const char* name)
{
    int r;
    uint8_t v;
    const char* value;


    r = nsi_message_get_u1(m, &v);
    if (r < 0) {
        nai_log_error(NSI_LOG_CORE, 
            nai_errno, "failed to read reliability type.");
        goto _end;
    };

    switch (v) {
    case NSI_RT_UNKNOWN:
        value = "unknown";
        break;
    case NSI_RT_RELIABLE:
        value = "reliable";
        break;
    case NSI_RT_UNRELIABLE:
        value = "unreliable";
        break;
    case NSI_RT_BOTH:
        value = "both";
        break;
    default:
        nai_log_error(NSI_LOG_CORE, 
            EINVAL, "read an invalid reliability type %d.", v);
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };
    r = nsi_conf_setopts(p, name, value);
    if (r < 0) {
        nai_log_error(NSI_LOG_CORE, nai_errno, 
            "failed to set option '%s' and value is '%s'.", name, value);
        goto _end;
    };

_end:
    return r;
};

static int nsi_conf_deserialize_address(
    nsi_conf_t* p, nsi_message_io_t* m, const char* name)
{
    intptr_t r;
    uint8_t len;
    nai_sockaddr_t* addr;
    char buf[256];
    char value[256];


    r = nsi_message_get_u1(m, &len);
    if (r < 0) {
        nai_log_error(NSI_LOG_CORE, 
            nai_errno, "failed to read address length.");
        goto _end;
    };

    if (len <= 0) {
        r = 0;
        goto _end;
    };

    r = nsi_message_read_least(m, buf, len);
    if (r < 0) {
        nai_log_error(NSI_LOG_CORE, 
            nai_errno, "failed to read %d bytes of address.", len);
        goto _end;
    };

    addr = (nai_sockaddr_t*)buf;
    r = nai_sockaddr_ntop(addr, len, 
        value, sizeof(value), NAI_ADDR_PORT|NAI_ADDR_ZINDEX);
    if (r < 0) {
        nai_log_error(NSI_LOG_CORE, 
            nai_errno, "output address string failed.");
        goto _end;
    };

    r = nsi_conf_setopts(p, name, value);
    if (r < 0) {
        nai_log_error(NSI_LOG_CORE, nai_errno, 
            "failed to set option '%s' and value is '%s'.", name, value);
        goto _end;
    };

_end:
    return (int)r;
};

static int nsi_conf_deserialize_tls(
    nsi_conf_t* p, nsi_message_io_t* m, const char* name)
{
    intptr_t r;
    uint8_t len;
    char buf[256];

    r = nsi_message_get_u1(m, &len);
    if (r < 0) {
        nai_log_error(NSI_LOG_CORE,
            nai_errno, "failed to read tls attribute length.");
        goto _end;
    };

    if (len <= 0) {
        r = 0;
        goto _end;
    };

    r = nsi_message_read_least(m, buf, len);
    if (r < 0) {
        nai_log_error(NSI_LOG_CORE,
            nai_errno, "failed to read %d bytes of tls attribute.", len);
        goto _end;
    };

    buf[len] = 0;

    r = nsi_conf_setopts(p, name, buf);
    if (r < 0) {
        nai_log_error(NSI_LOG_CORE, nai_errno,
            "failed to set option '%s' and value is '%s'.", name, buf);
        goto _end;
    };

_end:
    return (int)r;
};


int nsi_conf_serialize(nsi_conf_t* p, nsi_message_io_t* m)
{
    intptr_t r;
    int i, j;
    nai_rbnode_t* e;
    nai_rbnode_t* n;
    nsi_conf_service_t* s;
    nsi_conf_event_t* v;
    nsi_conf_group_t* g;
    nsi_conf_subs_t* u;
    nsi_conf_bind_t* b;
    nsi_range_t* q;


    nai_log_info(NSI_LOG_CORE, 0, 
        "serialize configuration ...");


    if (p == 0) {
        goto _last;
    };


    for (j = 0; j < (intptr_t)nai_countof(p->binds); j ++) {
        e = nai_rbtree_begin(&p->binds[j]);
        for ( ; e != nai_rbtree_end(&p->binds[j]); ) {
            b = (nsi_conf_bind_t*)e;
            e = nai_rbtree_next(e);


            /* put bind */
            r = nsi_message_put_u2(m, 1);
            if (r < 0) {
                nai_log_error(NSI_LOG_CORE, 
                    nai_errno, "failed to put option '%s'", "bind");
                goto _end;
            };

            r = nsi_message_put_u1(m, b->reliability);
            if (r < 0) {
                nai_log_error(NSI_LOG_CORE, 
                    nai_errno, "failed to put option '%s'", "reliability");
                goto _end;
            };

            r = nsi_message_put_u1(m, b->name.len);
            if (r < 0) {
                nai_log_error(NSI_LOG_CORE, 
                    nai_errno, "failed to put the length of address");
                goto _end;
            };

            r = nsi_message_write(m, b->name.addr, b->name.len);
            if (r < 0) {
                nai_log_error(NSI_LOG_CORE, 
                    nai_errno, "failed to put the address");
                goto _end;
            };


            for (i = 0; i < (intptr_t)b->ports.count; i ++) {
                q = (nsi_range_t*)b->ports.elts + i;

                r = nsi_message_put_u2(m, 1);
                if (r < 0) {
                    nai_log_error(NSI_LOG_CORE, 
                        nai_errno, "failed to put option '%s'", "port");
                    goto _end;
                };

                r = nsi_message_put_u2(m, q->start);
                if (r < 0) {
                    nai_log_error(NSI_LOG_CORE, 
                        nai_errno, "failed to put option '%s'", "port_start");
                    goto _end;
                };

                r = nsi_message_put_u2(m, q->end);
                if (r < 0) {
                    nai_log_error(NSI_LOG_CORE, 
                        nai_errno, "failed to put option '%s'", "port_end");
                    goto _end;
                };
            };

            /* the mark of last port */
            r = nsi_message_put_u2(m, 0);
            if (r < 0) {
                nai_log_error(NSI_LOG_CORE, 
                    nai_errno, "failed to put end mark of '%s'", "ports");
                goto _end;
            };
        };
    };

    /* the mark of last bind */
    r = nsi_message_put_u2(m, 0);
    if (r < 0) {
        nai_log_error(NSI_LOG_CORE, 
            nai_errno, "failed to put end mark of '%s'", "binds");
        goto _end;
    };


    e = nai_rbtree_begin(&p->servs);
    for ( ; e != nai_rbtree_end(&p->servs); ) {
        s = (nsi_conf_service_t*)e;
        e = nai_rbtree_next(e);


        /* put service */
        r = nsi_message_put_u2(m, s->serv);
        if (r < 0) {
            nai_log_error(NSI_LOG_CORE, 
                nai_errno, "failed to put option '%s'", "service");
            goto _end;
        };

        r = nsi_message_put_u2(m, s->inst);
        if (r < 0) {
            nai_log_error(NSI_LOG_CORE, 
                nai_errno, "failed to put option '%s'", "instance");
            goto _end;
        };

        r = nsi_message_put_u1(m, s->local);
        if (r < 0) {
            nai_log_error(NSI_LOG_CORE, 
                nai_errno, "failed to put option '%s'", "local");
            goto _end;
        };

        r = nsi_message_put_u1(m, s->reliability);
        if (r < 0) {
            nai_log_error(NSI_LOG_CORE, 
                nai_errno, "failed to put option '%s'", "reliability");
            goto _end;
        };

        r = nsi_message_put_u1(m, s->major);
        if (r < 0) {
            nai_log_error(NSI_LOG_CORE, 
                nai_errno, "failed to put option '%s'", "major");
            goto _end;
        };

        r = nsi_message_put_u4(m, s->minor);
        if (r < 0) {
            nai_log_error(NSI_LOG_CORE, 
                nai_errno, "failed to put option '%s'", "minor");
            goto _end;
        };

        r = nsi_message_put_u4(m, s->ttl);
        if (r < 0) {
            nai_log_error(NSI_LOG_CORE, 
                nai_errno, "failed to put option '%s'", "ttl");
            goto _end;
        };

        r = nsi_message_put_u4(m, s->cyclic_delay);
        if (r < 0) {
            nai_log_error(NSI_LOG_CORE, 
                nai_errno, "failed to put option '%s'", "cyclic_delay");
            goto _end;
        };

        for (i = 0; i < (int)nai_countof(s->names); i ++) {
            r = nsi_message_put_u1(m, s->names[i].len);
            if (r < 0) {
                nai_log_error(NSI_LOG_CORE, 
                    nai_errno, "failed to put the length of address(%d)", i);
                goto _end;
            };

            r = nsi_message_write(m, s->names[i].addr, s->names[i].len);
            if (r < 0) {
                nai_log_error(NSI_LOG_CORE, 
                    nai_errno, "failed to put the address(%d)", i);
                goto _end;
            };
        };


        /* put events */
        n = nai_rbtree_begin(&s->events);
        for ( ; n != nai_rbtree_end(&s->events); ) {
            v = (nsi_conf_event_t*)n;
            n = nai_rbtree_next(n);

            if (v->type == NSI_ET_SIGNAL) {
                continue;
            };

            r = nsi_message_put_u2(m, v->eid);
            if (r < 0) {
                nai_log_error(NSI_LOG_CORE, 
                    nai_errno, "failed to put option '%s'", "event");
                goto _end;
            };

            r = nsi_message_put_u1(m, v->type);
            if (r < 0) {
                nai_log_error(NSI_LOG_CORE, 
                    nai_errno, "failed to put option '%s'", "type");
                goto _end;
            };

            r = nsi_message_put_u1(m, v->reliability);
            if (r < 0) {
                nai_log_error(NSI_LOG_CORE, 
                    nai_errno, "failed to put option '%s'", "reliability");
                goto _end;
            };
        };

        /* the mark of last event */
        r = nsi_message_put_u2(m, NSI_EVENT_ANY);
        if (r < 0) {
            nai_log_error(NSI_LOG_CORE, 
                nai_errno, "failed to put end mark of '%s'", "events");
            goto _end;
        };


        /* put signals */
        n = nai_rbtree_begin(&s->events);
        for ( ; n != nai_rbtree_end(&s->events); ) {
            v = (nsi_conf_event_t*)n;
            n = nai_rbtree_next(n);

            if (v->type != NSI_ET_SIGNAL) {
                continue;
            };

            r = nsi_message_put_u2(m, v->eid);
            if (r < 0) {
                nai_log_error(NSI_LOG_CORE, 
                    nai_errno, "failed to put option '%s'", "signal");
                goto _end;
            };

            r = nsi_message_put_u1(m, v->reliability);
            if (r < 0) {
                nai_log_error(NSI_LOG_CORE, 
                    nai_errno, "failed to put option '%s'", "reliability");
                goto _end;
            };
        };

        /* the mark of last signal */
        r = nsi_message_put_u2(m, NSI_EVENT_ANY);
        if (r < 0) {
            nai_log_error(NSI_LOG_CORE, 
                nai_errno, "failed to put end mark of '%s'", "signals");
            goto _end;
        };


        /* put groups */
        n = nai_rbtree_begin(&s->groups);
        for ( ; n != nai_rbtree_end(&s->groups); ) {
            g = (nsi_conf_group_t*)n;
            n = nai_rbtree_next(n);

            r = nsi_message_put_u2(m, g->gid);
            if (r < 0) {
                nai_log_error(NSI_LOG_CORE,
                    nai_errno, "failed to put option '%s'", "group");
                goto _end;
            };

            r = nsi_message_put_u4(m, g->threshold);
            if (r < 0) {
                nai_log_error(NSI_LOG_CORE,
                    nai_errno, "failed to put option '%s'", "threshold");
                goto _end;
            };

            r = nsi_message_put_u4(m, g->ttl);
            if (r < 0) {
                nai_log_error(NSI_LOG_CORE,
                    nai_errno, "failed to put option '%s'", "ttl");
                goto _end;
            };

            r = nsi_message_put_u4(m, g->subscribe_timeo);
            if (r < 0) {
                nai_log_error(NSI_LOG_CORE,
                    nai_errno, "failed to put option '%s'", "subscribe_timeo");
                goto _end;
            };

            r = nsi_message_put_u4(m, g->subscribe_renew);
            if (r < 0) {
                nai_log_error(NSI_LOG_CORE,
                    nai_errno, "failed to put option '%s'", "subscribe_renew");
                goto _end;
            };

            r = nsi_message_put_u1(m, g->mcast.len);
            if (r < 0) {
                nai_log_error(NSI_LOG_CORE,
                    nai_errno, "failed to put the length of mcast");
                goto _end;
            };

            r = nsi_message_write(m, g->mcast.addr, g->mcast.len);
            if (r < 0) {
                nai_log_error(NSI_LOG_CORE,
                    nai_errno, "failed to put the mcast");
                goto _end;
            };

            for (i = 0; i < (intptr_t)g->subs.count; i ++) {
                u = (nsi_conf_subs_t*)g->subs.elts + i;

                r = nsi_message_put_u2(m, 1);
                if (r < 0) {
                    nai_log_error(NSI_LOG_CORE,
                        nai_errno, "failed to put option '%s'", "subs");
                    goto _end;
                };

                for (j = 0; j < (int)nai_countof(u->names); j ++) {
                    r = nsi_message_put_u1(m, u->names[j].len);
                    if (r < 0) {
                        nai_log_error(NSI_LOG_CORE, nai_errno, 
                            "failed to put the length of address(%d)", j);
                        goto _end;
                    };

                    r = nsi_message_write(m, u->names[j].addr, u->names[j].len);
                    if (r < 0) {
                        nai_log_error(NSI_LOG_CORE, nai_errno, 
                            "failed to put the address(%d)", j);
                        goto _end;
                    };
                };
            };

            /* the mark of last subs */
            r = nsi_message_put_u2(m, 0);
            if (r < 0) {
                nai_log_error(NSI_LOG_CORE, 
                    nai_errno, "failed to put end mark of '%s'", "subs");
                goto _end;
            };
        };

        /* the mark of last group */
        r = nsi_message_put_u2(m, (nsi_gid_t)0);
        if (r < 0) {
            nai_log_error(NSI_LOG_CORE, 
                nai_errno, "failed to put end mark of '%s'", "groups");
            goto _end;
        };

        /* put tls */
        r = nsi_message_put_u2(m, s->tls_version);
        if (r < 0) {
            nai_log_error(NSI_LOG_CORE,
                nai_errno, "failed to put tls version '%s'", "ttl");
            goto _end;
        };

        r = nsi_message_put_u2(m, s->tls_verify_client);
        if (r < 0) {
            nai_log_error(NSI_LOG_CORE,
                nai_errno, "failed to put tls verify client %d", s->tls_verify_client);
            goto _end;
        };

        r = nsi_message_put_u1(m, nai_str_len(&s->tls_peer_ca));
        if (r < 0) {
            nai_log_error(NSI_LOG_CORE,
                nai_errno, "failed to put the length of tls peer ca path");
            goto _end;
        };

        r = nsi_message_write(m, nai_str(&s->tls_peer_ca), nai_str_len(&s->tls_peer_ca));
        if (r < 0) {
            nai_log_error(NSI_LOG_CORE,
                nai_errno, "failed to put tls peer ca path [%s]", nai_str(&s->tls_peer_ca));
            goto _end;
        };

        r = nsi_message_put_u1(m, nai_str_len(&s->tls_local_cert));
        if (r < 0) {
            nai_log_error(NSI_LOG_CORE,
                nai_errno, "failed to put the length of tls local cert path");
            goto _end;
        };

        r = nsi_message_write(m, nai_str(&s->tls_local_cert), nai_str_len(&s->tls_local_cert));
        if (r < 0) {
            nai_log_error(NSI_LOG_CORE,
                nai_errno, "failed to put tls local cert path [%s]", nai_str(&s->tls_local_cert));
            goto _end;
        };

        r = nsi_message_put_u1(m, nai_str_len(&s->tls_local_privkey));
        if (r < 0) {
            nai_log_error(NSI_LOG_CORE,
                nai_errno, "failed to put the length of tls private key path");
            goto _end;
        };

        r = nsi_message_write(m, nai_str(&s->tls_local_privkey), nai_str_len(&s->tls_local_privkey));
        if (r < 0) {
            nai_log_error(NSI_LOG_CORE,
                nai_errno, "failed to put tls private key path [%s]", nai_str(&s->tls_local_privkey));
            goto _end;
        };

    };

_last:
    /* the mark of last service */
    r = nsi_message_put_u2(m, NSI_SERVICE_ANY);
    if (r < 0) {
        nai_log_error(NSI_LOG_CORE, 
            nai_errno, "failed to put end mark of '%s'", "services");
        goto _end;
    };

_end:
    return (int)r;
};


int nsi_conf_deserialize(nsi_conf_t* p, nsi_message_io_t* m)
{
    int r;
    nsi_conf_ctx_t* ctx;


    nai_log_info(NSI_LOG_CORE, 0, 
        "deserialize configuration ...");

    /* deserialize binds */
    for (;;) {
        r = nsi_conf_deserialize_bind(p, m);
        if (r < 0) {
            goto _fail;
        };
        if (r == 0) {
            break;
        };

        r = nsi_conf_deserialize_reliability(p, m, "reliability");
        if (r < 0) {
            goto _fail;
        };

        r = nsi_conf_deserialize_address(p, m, "address");
        if (r < 0) {
            goto _fail;
        };

        /* for ports */
        for (;;) {
            r = nsi_conf_deserialize_port(p, m);
            if (r < 0) {
                goto _fail;
            };
            if (r == 0) {
                break;
            };

            r = nsi_conf_deserialize_u2(p, m, "port_start");
            if (r < 0) {
                goto _fail;
            };

            r = nsi_conf_deserialize_u2(p, m, "port_end");
            if (r < 0) {
                goto _fail;
            };

            r = nsi_conf_setopts(p, "end", "port");
            if (r < 0) {
                nai_log_error(NSI_LOG_CORE, 
                    nai_errno, "failed to end option of '%s'", "port");
                goto _fail;
            };
        };

        r = nsi_conf_setopts(p, "end", "bind");
        if (r < 0) {
            nai_log_error(NSI_LOG_CORE, 
                nai_errno, "failed to end option of '%s'", "bind");
            goto _fail;
        };
    };

    /* deserialize services */
    for (;;) {
        r = nsi_conf_deserialize_service(p, m);
        if (r < 0) {
            goto _fail;
        };
        if (r == 0) {
            break;
        };

        r = nsi_conf_deserialize_reliability(p, m, "reliability");
        if (r < 0) {
            goto _fail;
        };

        r = nsi_conf_deserialize_u1(p, m, "major");
        if (r < 0) {
            goto _fail;
        };

        r = nsi_conf_deserialize_u4(p, m, "minor");
        if (r < 0) {
            goto _fail;
        };

        r = nsi_conf_deserialize_u4(p, m, "ttl");
        if (r < 0) {
            goto _fail;
        };

        r = nsi_conf_deserialize_u4(p, m, "cyclic_delay");
        if (r < 0) {
            goto _fail;
        };

        r = nsi_conf_deserialize_address(p, m, "unreliable");
        if (r < 0) {
            goto _fail;
        };

        r = nsi_conf_deserialize_address(p, m, "reliable");
        if (r < 0) {
            goto _fail;
        };

        r = nsi_conf_deserialize_address(p, m, "multicast");
        if (r < 0) {
            goto _fail;
        };

        /* for events */
        for (;;) {
            r = nsi_conf_deserialize_event(p, m);
            if (r < 0) {
                goto _fail;
            };
            if (r == 0) {
                break;
            };

            r = nsi_conf_deserialize_type(p, m, "type");
            if (r < 0) {
                goto _fail;
            };

            r = nsi_conf_deserialize_reliability(p, m, "reliability");
            if (r < 0) {
                goto _fail;
            };

            r = nsi_conf_setopts(p, "end", "event");
            if (r < 0) {
                nai_log_error(NSI_LOG_CORE, 
                    nai_errno, "failed to end option of '%s'", "event");
                goto _fail;
            };
        };


        /* for signals */
        for (;;) {
            r = nsi_conf_deserialize_signal(p, m);
            if (r < 0) {
                goto _fail;
            };
            if (r == 0) {
                break;
            };

            r = nsi_conf_deserialize_reliability(p, m, "reliability");
            if (r < 0) {
                goto _fail;
            };

            r = nsi_conf_setopts(p, "end", "signal");
            if (r < 0) {
                nai_log_error(NSI_LOG_CORE, 
                    nai_errno, "failed to end option of '%s'", "signal");
                goto _fail;
            };
        };


        /* for groups */
        for (;;) {
            r = nsi_conf_deserialize_group(p, m);
            if (r < 0) {
                goto _fail;
            };
            if (r == 0) {
                break;
            };

            r = nsi_conf_deserialize_u4(p, m, "threshold");
            if (r < 0) {
                goto _fail;
            };

            r = nsi_conf_deserialize_u4(p, m, "ttl");
            if (r < 0) {
                goto _fail;
            };

            r = nsi_conf_deserialize_u4(p, m, "subscribe_timeo");
            if (r < 0) {
                goto _fail;
            };

            r = nsi_conf_deserialize_u4(p, m, "subscribe_renew");
            if (r < 0) {
                goto _fail;
            };

            r = nsi_conf_deserialize_address(p, m, "multicast");
            if (r < 0) {
                goto _fail;
            };

            /* for subs */
            for (;;) {
                r = nsi_conf_deserialize_subs(p, m);
                if (r < 0) {
                    goto _fail;
                };
                if (r == 0) {
                    break;
                };

                r = nsi_conf_deserialize_address(p, m, "unreliable");
                if (r < 0) {
                    goto _fail;
                };

                r = nsi_conf_deserialize_address(p, m, "reliable");
                if (r < 0) {
                    goto _fail;
                };

                r = nsi_conf_setopts(p, "end", "subs");
                if (r < 0) {
                    nai_log_error(NSI_LOG_CORE, 
                        nai_errno, "failed to end option of '%s'", "subs");
                    goto _fail;
                };
            };

            r = nsi_conf_setopts(p, "end", "group");
            if (r < 0) {
                nai_log_error(NSI_LOG_CORE, 
                    nai_errno, "failed to end option of '%s'", "group");
                goto _fail;
            };
        };

        /* for tls */
        r = nsi_conf_deserialize_u2(p, m, "tls_version");
        if (r < 0) {
            goto _fail;
        };

        r = nsi_conf_deserialize_u2(p, m, "tls_verify_client");
        if (r < 0) {
            goto _fail;
        };

        r = nsi_conf_deserialize_tls(p, m, "tls_peer_ca");
        if (r < 0) {
            goto _fail;
        };

        r = nsi_conf_deserialize_tls(p, m, "tls_local_cert");
        if (r < 0) {
            goto _fail;
        };

        r = nsi_conf_deserialize_tls(p, m, "tls_local_privkey");
        if (r < 0) {
            goto _fail;
        };

        /* end service */
        r = nsi_conf_setopts(p, "end", "");
        if (r < 0) {
            nai_log_error(NSI_LOG_CORE, 
                nai_errno, "failed to end option of '%s'", "service");
            goto _fail;
        };
    };

    if (p->ctx) {
        ctx = (nsi_conf_ctx_t*)p->ctx;
        if (ctx->top->data != (char*)p) {
            nai_log_error(NSI_LOG_CORE, 
                EINVAL, "configuration end not reached");
            nai_errno = EINVAL;
            r = -1;
            goto _end;
        };

        nai_pool_close(&ctx->pool);
        p->ctx = 0;
    };

_end:
    return r;

_fail:
    nai_log_error(NSI_LOG_CORE, 
        nai_errno, "deserialize configuration failed");
    goto _end;
};


static int nsi_conf_apply_service_info(
    nsi_conf_service_t* s, uint32_t ttl, nsi_discovery_t* sd)
{
    int r;
    nsi_routing_t* rt;
    nsi_service_info_t si;


    rt = sd->rt;
    si.local = 0;
    si.owner = 0;
    si.ttl = ttl;
    si.cyclic_delay = 0;
    si.serv = s->serv;
    si.inst = s->inst;
    si.major = s->major;
    si.minor = s->minor;
    si.names[0] = s->names[0];
    si.names[1] = s->names[1];
    nsi_discovery_set_service_info(sd, &si, rt->cl);

    r = 0;

    return r;
};


int nsi_conf_add_preset_service(
    nsi_conf_t* p, nsi_discovery_t* sd)
{
    int r;
    nai_rbnode_t* e;
    nsi_routing_t* rt;
    nsi_conf_service_t* s;


    nai_log_info(NSI_LOG_CORE, 0, 
        "add preset services to discovery");


    rt = sd->rt;
    r = nsi_routing_batch_begin(rt);
    if (r < 0) {
        nai_log_alert(NSI_LOG_CORE, 
            nai_errno, "start batch failed");
        goto _end;
    };

    /* for each service */
    /* add preset service to discovery */
    e = nai_rbtree_begin(&p->servs);
    for ( ; e != nai_rbtree_end(&p->servs); ) {
        s = (nsi_conf_service_t*)e;
        e = nai_rbtree_next(e);

        /* add new service into discovery */
        if (s->preset && sd) {
            nsi_conf_apply_service_info(s, -1, sd);
        };
    };

    nsi_routing_batch_end(rt);
    r = 0;

_end:
    return r;
};


int nsi_conf_attach_client(
    nsi_conf_t* p, nsi_conf_t* c, nsi_cid_t cid, 
    nsi_discovery_t* sd)
{
    int r;
    int ec;
    int reliable;
    nai_rbnode_t** n;
    nai_rbnode_t* parent;
    nai_list_entry_t* e;
    nsi_routing_t* rt;
    nsi_conf_bind_t* b;
    nsi_conf_service_t* s;
    nsi_conf_service_t* d;


    nai_log_info(NSI_LOG_CORE, 0, 
        "attach configuration of client(0x%x)", cid);


    /* for each service */
    /* checking before merge configuration */
    e = c->owns.servs.next;
    for ( ; e != &c->owns.servs; e = e->next) {
        s = nai_containof(e, nsi_conf_service_t, node);

        if (!s->local) {
            continue;
        };

        /* find exists service */
        n = nsi_conf_find_service(p, s->serv, s->inst, 0);
        if (n[0] == 0) {
            continue;
        };

        d = nai_containof(n[0], nsi_conf_service_t, ent);
        if (d->local) {
            /* exists */
            ec = EEXIST;
            nai_log_error(NSI_LOG_CORE, ec, 
                "the configuration of service(%d, %d) is exists.", 
                s->serv, s->inst);

            nai_errno = ec;
            r = -1;
            goto _end;
        };
    };

    rt = sd ? sd->rt : 0;
    if (sd) {
        r = nsi_routing_batch_begin(rt);
        if (r < 0) {
            nai_log_alert(NSI_LOG_CORE, 
                nai_errno, "start batch failed");
            goto _end;
        };
    };

    /* for each bind */
    /* merge configuration */
    e = c->owns.binds.next;
    for ( ; e != &c->owns.binds; e = e->next) {
        b = nai_containof(e, nsi_conf_bind_t, node);
        reliable = b->reliability == NSI_RT_RELIABLE;
        nai_rbtree_erase(&c->binds[reliable], &b->ent);
        nsi_conf_insert_bind(p, b);
    };

    /* for each service */
    /* merge configuration */
    e = c->owns.servs.next;
    for ( ; e != &c->owns.servs; e = e->next) {
        s = nai_containof(e, nsi_conf_service_t, node);
        s->owner = cid;
        nai_rbtree_erase(&c->servs, &s->ent);

        n = nsi_conf_find_service(p, s->serv, s->inst, &parent);
        if (n[0]) {
            d = nai_containof(n[0], nsi_conf_service_t, ent);
            if (d->local || !s->local) {
                nsi_conf_insert_extra(p, s);
                continue;
            };

            /* remove from discovery */
            if (d->preset && sd) {
                nsi_conf_apply_service_info(d, 0, sd);
            };

            /* move the configuation of client to extra */
            nai_rbtree_erase(&p->servs, &d->ent);
            nsi_conf_insert_extra(p, d);
            n = nsi_conf_find_service(p, s->serv, s->inst, &parent);
        };

        nai_log_info(NSI_LOG_CORE, 0, 
            "attach service(%d, %d) configuration of client(0x%x).", 
            s->serv, s->inst, s->owner);

        nai_rbtree_link(&p->servs, &s->ent, parent, n);
        nai_rbtree_color(&p->servs, &s->ent);

        /* add new service into discovery */
        if (s->preset && sd) {
            nsi_conf_apply_service_info(s, -1, sd);
        };
    };

    if (sd) {
        nsi_routing_batch_end(rt);
    };
    r = 0;

_end:
    return r;
};


int nsi_conf_detach_client(
    nsi_conf_t* p, nsi_conf_t* c, 
    nsi_discovery_t* sd)
{
    int r;
    int serv;
    int reliable;
    nai_rbnode_t** n;
    nai_rbnode_t* parent;
    nai_list_entry_t* e;
    nsi_routing_t* rt;
    nsi_conf_bind_t* b;
    nsi_conf_service_t* s;
    nsi_conf_service_t* d;


    e = c->owns.servs.next;
    if (e != &c->owns.servs) {
        s = nai_containof(e, nsi_conf_service_t, node);

        nai_log_info(NSI_LOG_CORE, 0, 
            "detach configuration of client(0x%x)", s->owner);
    };

    rt = sd ? sd->rt : 0;
    if (sd) {
        r = nsi_routing_batch_begin(rt);
        if (r < 0) {
            nai_log_alert(NSI_LOG_CORE, 
                nai_errno, "start batch failed");
            goto _end;
        };
    };

    /* for each bind */
    /* move from main configuration to client configuration */
    e = c->owns.binds.next;
    for ( ; e != &c->owns.binds; e = e->next) {
        b = nai_containof(e, nsi_conf_bind_t, node);
        reliable = b->reliability == NSI_RT_RELIABLE;
        nai_rbtree_erase(&p->binds[reliable], &b->ent);
        nsi_conf_insert_bind(c, b);
    };

    /* for each service */
    /* move from main configuration to client configuration */
    e = c->owns.servs.next;
    for ( ; e != &c->owns.servs; e = e->next) {
        s = nai_containof(e, nsi_conf_service_t, node);

        /* is added ? */
        if (s->owner == 0) {
            continue;
        };

        /* find service */
        n = nsi_conf_find_service(p, s->serv, s->inst, 0);

        /* remove from main configuation */
        serv = n[0] == &s->ent;
        if (serv) {
            assert(n[0] == &s->ent);
            nai_rbtree_erase(&p->servs, &s->ent);
        } else {
            nai_rbtree_erase(&p->extra, &s->ent);
        };

        if (serv) {
            /* when an item is removed from the servs map, 
             * check if there are other duplicates in the extra map, 
             * and move the duplicate to replace the removed item.
             */
            n = nsi_conf_find_extra(p, s->serv, s->inst, 0);
            if (n[0]) {
                /* remove from extra map */
                d = nai_containof(n[0], nsi_conf_service_t, ent);
                nai_rbtree_erase(&p->extra, &d->ent);

                /* move to servs map */
                n = nsi_conf_find_service(p, d->serv, d->inst, &parent);
                nai_rbtree_link(&p->servs, &d->ent, parent, n);
                nai_rbtree_color(&p->servs, &d->ent);

                /* add new service into discovery */
                if (d->preset && sd) {
                    nsi_conf_apply_service_info(d, -1, sd);
                };
            } else {
                /* no service, remove from discovery */
                if (s->preset && sd) {
                    nsi_conf_apply_service_info(s, 0, sd);
                };
            };

            nai_log_info(NSI_LOG_CORE, 0, 
                "detach service(%d, %d) configuration of client(0x%x).", 
                s->serv, s->inst, s->owner);
        };

        /* move back to client configuation */
        n = nsi_conf_find_service(c, s->serv, s->inst, &parent);
        nai_rbtree_link(&c->servs, &s->ent, parent, n);
        nai_rbtree_color(&c->servs, &s->ent);
    };

    if (sd) {
        nsi_routing_batch_end(rt);
    };
    r = 0;

_end:
    return r;
};


