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
/// @file       nsi_routing_endpoint.c
/// @brief
/// @details
/// @date       2021-05-16
/// @author     xn
/// @version    1.2.0
///
/// ================================================================

#include "nsi_client.h"
#include "nsi_service.h"
#include "nsi_proxy.h"
#include "nsi_discovery.h"
#include "nsi_routing_impl.h"
#include "nsi_log.h"
#include "nsomeip/core/nsi_app.h"
#include "nai/runtime/nai_errno.h"
#include "nai/runtime/nai_util.h"



static int nsi_endpoint_inner_open(nsi_endpoint_t* ep)
{
    (void)ep;

    return 0;
};


static int nsi_endpoint_inner_map_conn(
    nsi_endpoint_t* ep, nsi_endpoint_t* s, nsi_connid_t cid)
{
    (void)ep;
    (void)s;
    (void)cid;

    nai_errno = ENOTSUP;
    return -1;
};


static int nsi_endpoint_inner_get_name(
    nsi_endpoint_t* ep, nsi_connid_t cid, nsi_endpoint_name_t* name)
{
    int r;
    nsi_endpoint_name_t addr;


    (void)cid;

    if (name) {
        addr = ep->name;
        if (name->addr && name->len > addr.len) {
            nai_memcpy(name->addr, addr.addr, addr.len);
        };
        name->len = addr.len;
    };

    r = 0;

    return r;
};


static int nsi_endpoint_inner_get_opt(
    nsi_endpoint_t* ep, int opt, intptr_t* value)
{
    (void)ep;
    (void)opt;
    (void)value;

    nai_errno = ENOTSUP;
    return -1;
};


static int nsi_endpoint_inner_set_opt(
    nsi_endpoint_t* ep, int opt, intptr_t value)
{
    int r;
    nai_rbnode_t* ent;
    nsi_routing_endpoint_t* e;
    nsi_client_t* c;
    nsi_app_t* a;


    (void)value;

    switch (opt) {
    case NSI_EOPT_RESET_UCODE:
        break;
    default:
        nai_errno = ENOTSUP;
        r = -1;
        goto _end;
    };


    e = nsi_routing_get_endpoint_ctx(ep);

    /* remove clients */
    ent = nai_rbtree_begin(&e->clients);
    for ( ; ent != nai_rbtree_end(&e->clients); ) {
        c = nai_containof(ent, nsi_client_t, ep.ente);
        ent = nai_rbtree_next(ent);

        a = c->app;
        if (a != 0) {
            a->uid = 0;
        };
    };

    r = 0;

_end:
    return r;
};


static int nsi_endpoint_inner_send(
    nsi_endpoint_t* ep, nsi_message_t* m, uint32_t uid, int flags)
{
    int r;
    nsi_client_t* c;


    c = nsi_client_find_in_endpoint(ep, m->conn);
    if (c == 0) {
        nai_log_error(NSI_LOG_CORE, nai_errno, 
            "cannot send to non-existent client(0x%x)", m->conn);
        r = -1;
        goto _end;
    };


    assert(c->app != 0);
    r = nsi_client_send(c, m, uid, flags);


_end:
    return r;
};


static int nsi_endpoint_inner_flush(nsi_endpoint_t* ep)
{
    (void)ep;

    return 0;
};


static int nsi_endpoint_inner_close(nsi_endpoint_t* ep)
{
    (void)ep;

    return 0;
};


static nsi_endpoint_ops_t nsi_endpoint_inner_ops = {
    nsi_endpoint_inner_open, 
    nsi_endpoint_inner_map_conn, 
    nsi_endpoint_inner_get_name, 
    nsi_endpoint_inner_get_opt, 
    nsi_endpoint_inner_set_opt, 
    nsi_endpoint_inner_send, 
    nsi_endpoint_inner_flush, 
    nsi_endpoint_inner_close
};




static int nsi_routing_connected(nsi_endpoint_t* ep, nsi_connid_t cid)
{
    int r;
    nai_rbnode_t* ent;
    nsi_service_t* s;
    nsi_service_conn_t* sc;
    nsi_routing_t* p;
    nsi_routing_endpoint_t* e;


    nai_log_debug(NSI_LOG_CORE, 0, 
        "the endpoint(%s/%d) is connected", nai_str(&ep->host), cid);

    e = nsi_routing_get_endpoint_ctx(ep);
    if (e == 0) {
        r = 0;
        goto _end;
    };

    e->refs ++;
    p = e->rt;
    r = nsi_routing_batch_begin(p);
    if (r < 0) {
        nai_log_alert(NSI_LOG_CORE, 
            nai_errno, "start batch failed");
        goto _end;
    };


    /* send connect on proxy mode */
    if (p->type == NSI_RTYPE_PROXY && 
        p->ipc == ep && p->sd != 0) {
        nsi_discovery_handle_connected(p->sd);
    };


    ent = nai_rbtree_begin(&e->servs);
    for ( ; ent != nai_rbtree_end(&e->servs); ) {
        sc = nai_containof(ent, nsi_service_conn_t, ente);
        ent = nai_rbtree_next(ent);

        if (sc->conn != cid) {
            continue;
        };

        s = nai_containof(sc, nsi_service_t, ep[sc->idx]);
        r = nsi_service_handle_connected(s, ep, cid);
        if (r < 0) {
            nai_log_error(NSI_LOG_CORE, 
                nai_errno, "service handle connect event failed");
        };
    };


    nsi_routing_batch_end(p);
    nsi_routing_release_endpoint(ep);
    r = 0;

_end:
    return r;
};


static int nsi_routing_disconnected(nsi_endpoint_t* ep, nsi_connid_t cid)
{
    int r;
    nai_rbnode_t* ent;
    nsi_client_t* c;
    nsi_service_t* s;
    nsi_service_conn_t* sc;
    nsi_routing_t* p;
    nsi_routing_endpoint_t* e;


    nai_log_debug(NSI_LOG_CORE, 0, 
        "the endpoint(%s/%d) is disconnected", nai_str(&ep->host), cid);

    e = nsi_routing_get_endpoint_ctx(ep);
    if (e == 0) {
        r = 0;
        goto _end;
    };

    e->refs ++;
    p = e->rt;
    r = nsi_routing_batch_begin(p);
    if (r < 0) {
        nai_log_alert(NSI_LOG_CORE, 
            nai_errno, "start batch failed");
        goto _end;
    };


    /* remove clients */
    ent = nai_rbtree_begin(&e->clients);
    for ( ; ent != nai_rbtree_end(&e->clients); ) {
        c = nai_containof(ent, nsi_client_t, ep.ente);
        ent = nai_rbtree_next(ent);

        if (c->ep.conn != cid) {
            continue;
        };

        nsi_client_close(c);
    };

    /* notify service */
    ent = nai_rbtree_begin(&e->servs);
    for ( ; ent != nai_rbtree_end(&e->servs); ) {
        sc = nai_containof(ent, nsi_service_conn_t, ente);
        ent = nai_rbtree_next(ent);

#if 0
        /* disable check for notify tcp disconnected to subscription */
        if (sc->conn != cid) {
            continue;
        };
#endif

        s = nai_containof(sc, nsi_service_t, ep[sc->idx]);
        r = nsi_service_handle_disconnected(s, ep, cid);
        if (r < 0) {
            nai_log_error(NSI_LOG_CORE, 
                nai_errno, "service handle disconnect event failed");
        };
    };

    if (p->type == NSI_RTYPE_PROXY && 
        p->ipc == ep && p->sd != 0) {
        nsi_discovery_handle_disconnected(p->sd);
    };

    nsi_routing_batch_end(p);
    nsi_routing_release_endpoint(ep);
    r = 0;

_end:
    return r;
};



static int nsi_routing_denied_message(nsi_message_t* m)
{
    const char* type;
    const char* cred;
    char from[128];


    if (nai_log_is_enabled_warn(&nsi_log_core)) {
        switch (m->cred.type) {
        case NSI_CRED_ADDRESS:
            cred = "host";
            from[0] = 0;
            nai_sockaddr_ntop(&m->cred.addr.in, 
                m->cred.len, from, sizeof(from), 0);
            break;
        case NSI_CRED_PID:
            cred = "local";
            nai_sprintf(from, "%d", m->cred.pid);
            break;
        case NSI_CRED_LOCAL:
            cred = "local";
            nai_strcpy(from, "self");
            break;
        default:
            cred = "unknown";
            from[0] = 0;
            break;
        };

        switch (m->hdr.type) {
        case NSI_MT_REQUEST:
        case NSI_MT_REQUEST_NO_RETURN:
            type = "request ";
            break;
        case NSI_MT_RESPONSE:
        case NSI_MT_ERROR:
            type = "response ";
            break;
        case NSI_MT_NOTIFICATION:
            type = "notification ";
            break;
        default:
            type = "";
            break;
        };

        if (m->hdr.client == 0) {
            nai_log_warn(NSI_LOG_CORE, EINVAL, 
                "the %smessage(%d) from %s(%s) is denied", 
                type, m->hdr.method, cred, from);
        } else {
            nai_log_warn(NSI_LOG_CORE, EINVAL, 
                "the %smessage(%d) of client(0x%x) from %s(%s) is denied", 
                type, m->hdr.method, m->hdr.client, cred, from);
        };
    };

    return 0;
};


static int nsi_routing_message(nsi_endpoint_t* ep, nsi_message_t* m)
{
    intptr_t r;
    int ext;
    uint8_t len;
    uint8_t type;
    uint16_t flags;
    nsi_connid_t conn;
    nsi_inst_t inst;
    nsi_client_t* c;
    nsi_service_t* s;
    nsi_routing_endpoint_t* e;
    nsi_routing_t* p;
    nsi_message_io_t io;
    nsi_message_t* emsg;


    e = nsi_routing_get_endpoint_ctx(ep);
    assert(e != 0);

    if (nsi_is_proxy_message(m)) {
        p = e->rt;
        r = nsi_proxy_handle_message(p, m, ep);
        if (r < 0) {
            nai_log_error(NSI_LOG_CORE, 
                nai_errno, "handle proxy message failed");
            goto _fail;
        };

        goto _end;
    };

    /* PRS_SOMEIPSD_00252 */
    if (nsi_is_discovery_message(m)) {
        p = e->rt;
        if (p->sd == 0) {
            nai_log_warn(NSI_LOG_CORE, 0, 
                "unable to handle message without service discovery");
            goto _fail;
        };

        r = nsi_discovery_handle_message(p->sd, m, ep);
        if (r < 0) {
            nai_log_error(NSI_LOG_CORE, 
                nai_errno, "handle discovery message failed");
            goto _fail;
        };

        goto _end;
    };

    p = e->rt;
    if (ep == p->ipc) {
        r = nsi_message_read_start(&io, 
            m, m->hdr.len - NSI_MSGEXT_SIZE - 1);
        if (r < 0) {
            nai_log_error(NSI_LOG_CORE, nai_errno, 
                "start read instance from proxy message failed");
            goto _fail;
        };

        r = nsi_message_get_u1(&io, &len);
        if (r < 0) {
            nai_log_error(NSI_LOG_CORE, nai_errno, 
                "read cret.len from proxy message failed");
            goto _fail;
        };
        if (len > sizeof(m->cred.data)) {
            nai_log_error(NSI_LOG_CORE, EINVAL, 
                "the cret.len(%d) from proxy message is too large", len);
            nai_errno = EINVAL;
            goto _fail;
        };

        ext = sizeof(inst) + sizeof(flags) + sizeof(conn) + 
            sizeof(uint8_t) * 2 + len;

        r = nsi_message_read_start(&io, 
            m, m->hdr.len - NSI_MSGEXT_SIZE - ext);
        if (r < 0) {
            nai_log_error(NSI_LOG_CORE, nai_errno, 
                "start read instance from proxy message failed");
            goto _fail;
        };

        r = nsi_message_get_u2(&io, &inst);
        if (r < 0) {
            nai_log_error(NSI_LOG_CORE, nai_errno, 
                "read instance from proxy message failed");
            goto _fail;
        };

        r = nsi_message_get_u2(&io, &flags);
        if (r < 0) {
            nai_log_error(NSI_LOG_CORE, nai_errno, 
                "read flags from proxy message failed");
            goto _fail;
        };

        r = nsi_message_get_u4(&io, &conn);
        if (r < 0) {
            nai_log_error(NSI_LOG_CORE, nai_errno, 
                "read connection id from proxy message failed");
            goto _fail;
        };

        if (p->type != NSI_RTYPE_ROUTE) {

            r = nsi_message_get_u1(&io, &type);
            if (r < 0) {
                nai_log_error(NSI_LOG_CORE, nai_errno, 
                    "read cred.type from proxy message failed");
                goto _fail;
            };

            if (type != NSI_CRED_LOCAL) {
                r = nsi_message_read_least(&io, m->cred.data, len);
                if (r < 0) {
                    nai_log_error(NSI_LOG_CORE, nai_errno, 
                        "read cred.data from proxy message failed");
                    goto _fail;
                };

                m->cred.len = len;
                m->cred.type = type;
            };
        };

        r = nsi_message_read_end(&io);
        if (r < 0) {
            nai_log_error(NSI_LOG_CORE, nai_errno, 
                "finish read extra data from proxy message failed");
            goto _fail;
        };

        r = nai_buflist_rcommit_last(&m->payload, ext);
        if (r < 0) {
            nai_log_error(NSI_LOG_CORE, nai_errno, 
                "remove extra data from proxy message failed");
            goto _fail;
        };

        m->conn = conn;
        m->flags = flags;
        m->sent = 0;
        m->last = 0;
        m->hdr.len -= ext;
        s = nsi_service_find_in_routing(p, m->hdr.serv, inst);
    } else {
        s = nsi_service_find_in_endpoint(ep, m->hdr.serv, 0);
    };
    if (s == 0) {
        nai_log_debug(NSI_LOG_CORE, nai_errno, 
            "the message(%d) is send to unknown service(%d)", 
            m->hdr.method, m->hdr.serv);
        
        /*send back NSI_E_UNKNOWN_SERVICE for NSI_MT_REQUEST*/
        if (m->hdr.type == NSI_MT_REQUEST) {
            emsg = nsi_network_create_message(p->net, NSI_USAGE_READ);
            if (emsg != 0) {
                emsg->reliable = m->reliable;
                emsg->conn = m->conn;
                emsg->inst = m->inst;
                emsg->hdr.serv = m->hdr.serv;
                emsg->hdr.method = m->hdr.method;
                emsg->hdr.len = NSI_MSGEXT_SIZE;
                emsg->hdr.client = m->hdr.client;
                emsg->hdr.session = m->hdr.session;
                emsg->hdr.protocol = m->hdr.protocol;
                emsg->hdr.interface = m->hdr.interface;
                emsg->hdr.type = NSI_MT_ERROR;
                emsg->hdr.code = NSI_E_UNKNOWN_SERVICE;
                if (nsi_endpoint_send(ep, emsg, 0, NSI_SEND_POSTED) < 0){
                    nsi_message_close(emsg);
                };
            };
        };
        goto _fail;
    };

    /* PRS_SOMEIPSD_00360
     * PRS_SOMEIPSD_00361
     * PRS_SOMEIPSD_00362
     */
    m->inst = s->inst;
    switch (m->hdr.type) {
    case NSI_MT_REQUEST:
    case NSI_MT_REQUEST_NO_RETURN:
        /* if the service is remote, only allow the messages from local */
        if (s->local == 0 && (
            s->proxy || m->cred.type == NSI_CRED_ADDRESS)) {
            nsi_routing_denied_message(m);
            goto _fail;
        };

        /* route to service */
        r = nsi_service_send(s, m);
        if (r < 0) {
            nai_log_error(NSI_LOG_CORE, 
                nai_errno, "send message(%d) to service(%d) failed", 
                m->hdr.method, m->hdr.serv);
            goto _fail;
        };
        break;

    case NSI_MT_RESPONSE:
    case NSI_MT_ERROR:
        /* if the service is local, only allow the messages from owner */
        if (s->local && (
            s->own == 0 || s->own->client->local || 
            m->cred.type == NSI_CRED_ADDRESS)) {
            nsi_routing_denied_message(m);
            goto _fail;
        };

        p = e->rt;
        c = 0;
        if (m->hdr.client != 0) {
            c = nsi_client_find_in_routing(p, m->hdr.client);
        };
        if (c != 0) {
            /* route to client */
            r = nsi_client_send(c, m, 0, 0);
            if (r < 0) {
                nai_log_error(NSI_LOG_CORE, 
                    nai_errno, "send message(%d) to client(0x%x) failed", 
                    m->hdr.method, m->hdr.client);
                goto _fail;
            };
        } else if (ep != p->ipc) {
            nai_log_warn(NSI_LOG_CORE, nai_errno, 
                "the message(%d) is send to non-existent client(0x%x)", 
                m->hdr.method, m->hdr.client);
            goto _fail;
        } else {
            /* route to serivce */
            r = nsi_service_send(s, m);
            if (r < 0) {
                nai_log_error(NSI_LOG_CORE, 
                    nai_errno, "send message(%d) to service(%d) failed", 
                    m->hdr.method, m->hdr.serv);
                goto _fail;
            };
        };
        break;

    case NSI_MT_NOTIFICATION:
        /* if the service is local, only allow the messages from owner */
        if (s->local && (
            s->own == 0 || s->own->client->local || 
            m->cred.type == NSI_CRED_ADDRESS)) {
            nsi_routing_denied_message(m);
            goto _fail;
        };

        /* route to service */
        r = nsi_service_send(s, m);
        if (r < 0) {
            nai_log_debug(NSI_LOG_CORE, 
                nai_errno, "send message(%d) to service(%d) failed", 
                m->hdr.method, m->hdr.serv);
            goto _fail;
        };
        break;

    default:
        goto _fail;
    };


_end:
    return (int)r;

_fail:
    nsi_message_close(m);
    r = 0;
    goto _end;
};


static int nsi_routing_handle_sent(
    nsi_routing_t* p, nsi_message_t* m, int errcode)
{
    int r;
    nsi_client_t* c;


    c = nsi_client_find_in_routing(p, m->hdr.client);
    if (c == 0) {
        r = 0;
        goto _end;
    };

    r = nsi_client_handle_sent(c, m, errcode);

_end:
    return r;
};


static int nsi_routing_sent(nsi_endpoint_t* ep, nsi_message_t* m, int errcode)
{
    int r;
    nsi_routing_t* rt;
    nsi_routing_endpoint_t* e;


    if (m->hdr.type != NSI_MT_REQUEST && 
        m->hdr.type != NSI_MT_REQUEST_NO_RETURN) {
        r = 0;
        goto _end;
    };


    e = nsi_routing_get_endpoint_ctx(ep);
    assert(e != 0);

    rt = e->rt;
    if (rt->batch) {
        m->sent = 1;
        m->cookie = errcode;
        nsi_message_ref(m);
        nai_list_insert_tail(&rt->posted, &m->ent);
        r = 0;
        goto _end;
    };

    r = nsi_routing_handle_sent(rt, m, errcode);


_end:
    return r;
};


static int nsi_routing_error(nsi_endpoint_t* ep, nsi_connid_t cid, int errcode)
{
    int r;

    nai_log_warn(NSI_LOG_CORE, errcode, 
        "connection(%d) is disconnected", cid);

    r = nsi_routing_disconnected(ep, cid);

    return r;
};


static int nsi_routing_alert(nsi_endpoint_t* ep, nsi_connid_t cid, int errcode)
{
    int r;

    (void)ep;
    (void)cid;

    nai_log_alert(NSI_LOG_CORE, errcode, 
        "a fatal error occurred in io processing");
    r = 0;

    return r;
};


static int nsi_routing_is_signal_ep(
    nsi_endpoint_t* ep, nsi_serv_t serv, nsi_inst_t inst, nsi_eid_t eid)
{
    int r;
    nsi_conf_t* c;
    nsi_routing_t* rt;
    nsi_routing_endpoint_t* e;
    nsi_service_t* s;
    nsi_event_info_t ei;


    e = nsi_routing_get_endpoint_ctx(ep);
    if (e == 0) {
        rt = ep->net->rt;
    } else {
        rt = e->rt;
    };

    if (ep == rt->ipc) {
        r = 0;
        goto _end;
    };

    c = rt->conf;
    if (c == 0) {
        r = 0;
        goto _end;
    };

    if (inst == NSI_INSTANCE_ANY) {
        s = nsi_service_find_in_endpoint(ep, serv, 0);
        if (s != 0) {
            inst = s->inst;
        };
    };
    if (inst != NSI_INSTANCE_ANY) {
        r = nsi_conf_get_event(c, serv, inst, eid, &ei);
        if (r >= 0) {
            r = ei.type == NSI_ET_SIGNAL;
            goto _end;
        };
    };

    r = nsi_conf_is_signal(c, serv, NSI_INSTANCE_ANY, eid);

_end:
    return r;
};


static int nsi_routing_is_subnet_ep(
    nsi_endpoint_t* ep, const nsi_endpoint_name_t* name)
{
    int r;
    nsi_conf_t* c;
    nsi_routing_t* rt;
    nsi_routing_endpoint_t* e;


    e = nsi_routing_get_endpoint_ctx(ep);
    if (e == 0) {
        rt = ep->net->rt;
    } else {
        rt = e->rt;
    };

    c = rt->conf;
    if (c == 0) {
        r = 1;
        goto _end;
    };

    r = nsi_conf_is_subnet(c, name);

_end:
    return r;
};


static int nsi_routing_get_bind(nsi_endpoint_t* ep, 
    int reliable, const nsi_endpoint_name_t* name, nsi_range_t** pptr)
{
    int r;
    nsi_conf_t* c;
    nsi_routing_t* rt;
    nsi_routing_endpoint_t* e;


    e = nsi_routing_get_endpoint_ctx(ep);
    if (e == 0) {
        rt = ep->net->rt;
    } else {
        rt = e->rt;
    };

    c = rt->conf;
    if (c == 0) {
        nai_errno = ENOENT;
        r = -1;
        goto _end;
    };

    r = nsi_conf_get_bind(c, reliable, name, pptr);

_end:
    return r;
};


static int nsi_routing_get_interface(
    nsi_endpoint_t* ep, nsi_endpoint_name_t* name)
{
    int r;
    nsi_conf_t* c;
    nsi_routing_t* rt;
    nsi_routing_endpoint_t* e;


    e = nsi_routing_get_endpoint_ctx(ep);
    if (e == 0) {
        rt = ep->net->rt;
    } else {
        rt = e->rt;
    };

    c = rt->conf;
    if (c == 0) {
        nai_errno = ENOENT;
        r = -1;
        goto _end;
    };

    name[0] = c->host;
    r = 0;

_end:
    return r;
};


nsi_endpoint_cb_t nsi_routing_endpoint_cb = {
    nsi_routing_connected, 
    nsi_routing_message, 
    nsi_routing_sent, 
    nsi_routing_error, 
    nsi_routing_alert, 
    nsi_routing_is_signal_ep, 
    nsi_routing_is_subnet_ep, 
    nsi_routing_get_bind, 
    nsi_routing_get_interface, 
};


static nai_sockaddr_in4_t inner_name = {
    .sin_family = AF_INET, 
};


static void nsi_routing_init_endpoint(
    nsi_routing_t* p, nsi_endpoint_t* e, nsi_routing_endpoint_t* r)
{
    nai_rbtree_init(&r->servs);
    nai_rbtree_init(&r->clients);
    nai_rbtree_init(&r->mcasts);
    r->refs = 1;
    r->ep = e;
    r->rt = p;
    e->cb = &nsi_routing_endpoint_cb;
    e->ud = r;

    return;
};


static nsi_endpoint_t* nsi_routing_create_server_impl(
    nsi_routing_t* p, const nsi_endpoint_name_t* name, 
    nsi_serv_t serv, nsi_inst_t inst, int reliable, int flags, nsi_tls_info_t* tls)
{
    int ec;
    nsi_endpoint_t* r;
    nsi_routing_endpoint_t* e;


    r = nsi_network_find(
        p->net, name, serv, inst, reliable, 1);
    if (r != 0) {
        assert(r->ud != 0);
        e = nsi_routing_get_endpoint_ctx(r);
        e->refs ++;
        goto _end;
    };

    e = (nsi_routing_endpoint_t*)nsi_routing_alloc(p, sizeof(*r));
    if (e == 0) {
        nai_log_alert(NSI_LOG_CORE, 
            nai_errno, "failed to allocate endpoint context");
        goto _end;
    };

    r = nsi_network_create_server(
        p->net, name, serv, inst, 
        (flags | reliable ? NSI_EFLAG_RELIABLE : 0), tls);
    if (r == 0) {
        ec = nai_errno;
        nai_log_error(NSI_LOG_CORE, ec, "failed to create server endpoint");
        nsi_routing_free(p, e);
        nai_errno = ec;
        goto _end;
    };

    nsi_routing_init_endpoint(p, r, e);


_end:
    return r;
};


static nsi_endpoint_t* nsi_routing_create_client_impl(
    nsi_routing_t* p, const nsi_endpoint_name_t* name, 
    nsi_serv_t serv, nsi_inst_t inst, int reliable, int flags, nsi_tls_info_t* tls)
{
    int ec;
    nsi_endpoint_t* r;
    nsi_routing_endpoint_t* e;


    r = nsi_network_find(
        p->net, name, serv, inst, reliable, 0);
    if (r != 0) {
        assert(r->ud != 0);
        e = nsi_routing_get_endpoint_ctx(r);
        e->refs ++;
        goto _end;
    };

    e = (nsi_routing_endpoint_t*)nsi_routing_alloc(p, sizeof(*r));
    if (e == 0) {
        nai_log_alert(NSI_LOG_CORE, 
            nai_errno, "failed to allocate endpoint context");
        goto _end;
    };

    r = nsi_network_create_client(
        p->net, name, serv, inst, 
        flags | (reliable ? NSI_EFLAG_RELIABLE : 0), tls);
    if (r == 0) {
        ec = nai_errno;
        nai_log_error(NSI_LOG_CORE, ec, "failed to create client endpoint");
        nsi_routing_free(p, e);
        nai_errno = ec;
        goto _end;
    };

    nsi_routing_init_endpoint(p, r, e);


_end:
    return r;
};


nsi_endpoint_t* nsi_routing_create_inner(nsi_routing_t* p)
{
    int ec;
    nsi_endpoint_t* r;
    nsi_endpoint_name_t name;
    nsi_routing_endpoint_t* e;


    name.addr = (nai_sockaddr_t*)&inner_name;
    name.len = sizeof(inner_name);
    r = nsi_network_find(
        p->net, &name, NSI_SERVICE_ANY, NSI_INSTANCE_ANY, 1, 1);
    if (r != 0) {
        assert(r->ud != 0);
        e = nsi_routing_get_endpoint_ctx(r);
        e->refs ++;
        goto _end;
    };

    e = (nsi_routing_endpoint_t*)nsi_routing_alloc(p, sizeof(*r));
    if (e == 0) {
        nai_log_alert(NSI_LOG_CORE, 
            nai_errno, "failed to allocate endpoint context");
        goto _end;
    };

    r = nsi_network_create_custom(
        p->net, &name, 1, &nsi_endpoint_inner_ops, p);
    if (r == 0) {
        ec = nai_errno;
        nai_log_alert(NSI_LOG_CORE, ec, "failed to create inner endpoint");
        nsi_routing_free(p, e);
        nai_errno = ec;
        goto _end;
    };

    nsi_routing_init_endpoint(p, r, e);


_end:
    return r;
};


nsi_endpoint_t* nsi_routing_create_server(
    nsi_routing_t* p, const nsi_endpoint_name_t* name, int reliable, 
    int flags, nsi_tls_info_t* tls)
{
    nsi_endpoint_t* r;


    r = nsi_routing_create_server_impl(p, 
        name, NSI_SERVICE_ANY, NSI_INSTANCE_ANY, 
        reliable, flags, tls);

    return r;
};


nsi_endpoint_t* nsi_routing_create_server_by_info(
    nsi_routing_t* p, const nsi_service_info_t* si, int reliable, 
    int flags)
{
    nsi_endpoint_t* e;


    assert(si->names[reliable].addr != 0);
    e = nsi_routing_create_server_impl(p, 
        &si->names[reliable], NSI_SERVICE_ANY, NSI_INSTANCE_ANY, 
        reliable, flags, (nsi_tls_info_t*)&si->tls);

    return e;
};


nsi_endpoint_t* nsi_routing_create_client(
    nsi_routing_t* p, const nsi_endpoint_name_t* name, int reliable, 
    int flags, nsi_tls_info_t* tls)
{
    nsi_endpoint_t* r;


    r = nsi_routing_create_client_impl(p, 
        name, NSI_SERVICE_ANY, NSI_INSTANCE_ANY, 
        reliable, flags, tls);

    return r;
};


nsi_endpoint_t* nsi_routing_create_client_by_info(
    nsi_routing_t* p, const nsi_service_info_t* si, int reliable, 
    int flags)
{
    nsi_endpoint_t* e;


    assert(si->names[reliable].addr != 0);
    e = nsi_routing_create_client_impl(p, 
        &si->names[reliable], NSI_SERVICE_ANY, NSI_INSTANCE_ANY, 
        reliable, flags, (nsi_tls_info_t*)&si->tls);

    return e;
};


int nsi_routing_release_endpoint(nsi_endpoint_t* ep)
{
    int r;
    nsi_routing_endpoint_t* e;


    e = nsi_routing_get_endpoint_ctx(ep);
    assert(e != 0);

    e->refs --;
    if (e->refs <= 0) {
        nsi_endpoint_close(ep);
        nsi_routing_free(e->rt, e);
    };

    r = 0;

    return r;
};


uint32_t nsi_routing_unique_code(nsi_routing_t* p)
{
    return nsi_network_unique_code(p->net);
};


#if defined(NSI_TRACK_BATCH)
int nsi_routing_batch_begin_d(nsi_routing_t* p, const char* func, int line)
#else
int nsi_routing_batch_begin(nsi_routing_t* p)
#endif
{
    int r;


    p->batch ++;
    if (p->batch == 1) {
#if defined(NSI_TRACK_BATCH)
        nai_log_debug(NSI_LOG_CORE, 
            0, "enable batch at %s:%d", func, line);
#endif
        nai_log_debug(NSI_LOG_CORE, 
            0, "enable batch, postpone all messages");
    };
    r = 0;

    return r;
};


#if defined(NSI_TRACK_BATCH)
int nsi_routing_batch_end_d(nsi_routing_t* p, const char* func, int line)
#else
int nsi_routing_batch_end(nsi_routing_t* p)
#endif
{
    int r;
    nsi_client_t* c;
    nsi_message_t* m;
    nai_list_entry_t* e;
    nai_list_entry_t list;


    if (p->batch <= 0) {
        nai_log_alert(NSI_LOG_CORE, 
            EPERM, "cannot end batch because the batch did not enable");
        nai_errno = EPERM;
        r = -1;
        goto _end;
    };

    if (p->batch > 1) {
        p->batch --;
        r = 0;
        goto _end;
    };

#if defined(NSI_TRACK_BATCH)
    nai_log_debug(NSI_LOG_CORE, 
        0, "disable batch at %s:%d", func, line);
#endif

    if (nai_list_is_empty(&p->posted)) {
        nai_log_debug(NSI_LOG_CORE, 0, "no postponed messages");
        p->batch --;
        r = 0;
        goto _end;
    };

_again:
    nai_log_debug(NSI_LOG_CORE, 0, "handle postponed messages");
    nai_list_init(&list);
    nai_list_add_tail(&list, &p->posted);

    while (!nai_list_is_empty(&list)) {
        e = list.next;
        m = (nsi_message_t*)e;
        nai_list_entry_remove(&m->ent);


        if (m->sent) {
            r = nsi_routing_handle_sent(p, m, m->cookie);
        } else if (nsi_is_proxy_message(m)) {
            r = nsi_proxy_handle_message(p, m, 0);
            if (r >= 0) {
                continue;
            };

            nai_log_error(NSI_LOG_CORE, nai_errno, 
                "failed to handle proxy message in batch");

        } else {
            c = nsi_client_find_in_routing(p, m->cookie);
            if (c != 0) {
                r = nsi_client_send(c, m, 0, 0);
                if (r >= 0) {
                    continue;
                };

                nai_log_error(NSI_LOG_CORE, nai_errno, 
                    "failed to send message(%d) to client(0x%x) in batch", 
                    m->hdr.method, m->cookie);
            };

            if (m->hdr.type == NSI_MT_REQUEST || 
                m->hdr.type == NSI_MT_REQUEST_NO_RETURN) {
                c = nsi_client_find_in_routing(p, m->hdr.client);
                if (c != 0) {
                    nsi_client_handle_sent(c, m, ECANCELED);
                };
            };
        };

        nsi_message_close(m);
    };

    if (p->batch != 1) {
        nai_log_alert(NSI_LOG_CORE, 
            EFAULT, "the count of batch is incorrect");
        nai_errno = EFAULT;
        r = -1;
        goto _end;
    };
    if (!nai_list_is_empty(&p->posted)) {
        goto _again;
    };

    (void)r;

    p->batch --;
    r = 0;


_end:
    return r;
};


