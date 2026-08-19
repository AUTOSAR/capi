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
/// @file       nsi_proxy.c
/// @brief
/// @details
/// @date       2021-05-24
/// @author     xn
/// @version    1.2.0
///
/// ================================================================

#include "nsi_proxy.h"
#include "nsi_service.h"
#include "nsi_client.h"
#include "nsi_discovery.h"
#include "nsi_routing_impl.h"
#include "nsi_log.h"
#include "nsomeip/net/nsi_network.h"
#include "nai/runtime/nai_errno.h"
#include "nai/runtime/nai_util.h"



#define NSI_PROXY_CMD_UNKNOWN       0
#define NSI_PROXY_CMD_CONNECT       0
#define NSI_PROXY_CMD_SERVICE       1
#define NSI_PROXY_CMD_AVAILABLE     2
#define NSI_PROXY_CMD_EVENT         3
#define NSI_PROXY_CMD_SUBSCRIBE     4
#define NSI_PROXY_CMD_SUBSCRIBE_ACK 5
#define NSI_PROXY_CMD_ERROR         6


static const char* nsi_proxy_cmdnames[] = {
    "connect", 
    "service", 
    "available", 
    "event", 
    "subscribe", 
    "ack", 
    "error"
};

static const char* nsi_proxy_opnames[] = {
    "add", 
    "offer", 
    "remove", 
    "remove"
};


typedef struct nsi_proxy_service_s {
    uint8_t cmd;
    uint8_t op;
    nsi_cid_t cid;
    nsi_cid_t rid;
    nsi_serv_t serv;
    nsi_inst_t inst;
    nsi_version_t major;
    nsi_versmin_t minor;
    nsi_cred_t cred;
} nsi_proxy_service_t;


typedef struct nsi_proxy_event_s {
    uint8_t cmd;
    uint8_t op;
    nsi_cid_t cid;
    nsi_cid_t rid;
    nsi_serv_t serv;
    nsi_inst_t inst;
    nsi_eid_t eid;
    uint8_t type;
    uint8_t reliability;
    nsi_cred_t cred;
    int ngroup;
    /* groups */
    /* nsi_gid_t groups[]; */
} nsi_proxy_event_t;


typedef struct nsi_proxy_subscribe_s {
    uint8_t cmd;
    union {
        uint8_t op;
        uint8_t sult;
    };
    nsi_cid_t cid;
    nsi_cid_t rid;
    nsi_serv_t serv;
    nsi_inst_t inst;
    nsi_gid_t gid;
    nsi_version_t major;
    nsi_eid_t eid;
    uint32_t ttl;
    nsi_cred_t cred;
} nsi_proxy_subscribe_t;


typedef struct nsi_proxy_connect_s {
    uint8_t cmd;
} nsi_proxy_connect_t;


typedef struct nsi_proxy_error_s {
    uint8_t cmd;
    uint8_t op;
    nsi_cid_t cid;
    nsi_serv_t serv;
    nsi_inst_t inst;
    nsi_version_t major;
    nsi_versmin_t minor;
    nsi_gid_t gid;
    nsi_eid_t eid;
    int error;
    uint8_t failcmd;
    uint8_t failop;
} nsi_proxy_error_t;


extern int nsi_conf_serialize(nsi_conf_t* p, nsi_message_io_t* m);


static int nsi_proxy_verify_cred(nsi_cred_t* cred)
{
    int r;


    switch (cred->type) {
    case NSI_CRED_LOCAL:
        r = cred->len == 0;
        break;
    case NSI_CRED_PID:
        r = cred->len == sizeof(cred->pid);
        break;
    case NSI_CRED_ADDRESS:
        switch (cred->addr.in.sa_family) {
        case AF_INET:
            r = cred->len == sizeof(cred->addr.in4);
            break;
#if defined(NAI_HAVE_SOCKADDR_IN6)
        case AF_INET6:
            r = cred->len == sizeof(cred->addr.in6);
            break;
#endif
        default:
            r = 0;
            break;
        };
        break;
    case NSI_CRED_CERT:
        r = 1;
        break;
    default:
        r = 0;
        break;
    };

    return r;
};


static int nsi_proxy_sendv(
    nsi_endpoint_conn_t* c, const nai_bufvec_t* v, int count)
{
    int r;
    int n;
    int ec;
    int len;
    uint8_t* ptr;
    nai_buf_t* b;
    nsi_message_t* m;
    nsi_routing_t* p;
    nsi_routing_endpoint_t* e;


    len = 0;
    for (n = 0; n < count; n ++) {
        len += (int)v[n].len;
    };

    e = nsi_routing_get_endpoint_ctx(c->ep);
    assert(e != 0);

    p = e->rt;
    m = nsi_network_create_message(p->net, NSI_USAGE_READ);
    if (m == 0) {
        nai_log_alert(NSI_LOG_CORE, 
            nai_errno, "create proxy message failed");
        r = -1;
        goto _end;
    };

    m->inst = 0;
    m->cookie = 0;
    m->conn = c->conn;
    m->hdr.serv = NSI_PROXY_SERV;
    m->hdr.method = NSI_PROXY_METHOD;
    m->hdr.len = NSI_MSGEXT_SIZE + len;
    m->hdr.protocol = NSI_PROTOCOL_VERSION;
    m->hdr.interface = NSI_DEFAULT_MAJOR;
    m->hdr.client = 0;
    m->hdr.session = 0;
    m->hdr.type = NSI_MT_NOTIFICATION;
    m->hdr.code = 0;


    b = nai_buf_alloc(m->payload.pool, len);
    if (b == 0) {
        nai_log_alert(NSI_LOG_CORE, 
            nai_errno, "create proxy message buffer failed");
        goto _fail;
    };

    ptr = nai_buf_ptr(b);
    for (n = 0; n < count; n ++) {
        nai_memcpy(ptr, v[n].buf, v[n].len);
        ptr += v[n].len;
    };
    nai_buf_wcommit(b, len);
    nai_buflist_insert_tail(&m->payload, b);


    r = nsi_endpoint_send(
        c->ep, m, 0, p->batch ? NSI_SEND_POSTED : 0);
    if (r < 0) {
        nai_log_alert(NSI_LOG_CORE, 
            nai_errno, "send proxy message failed");
        goto _fail;
    };


_end:
    return r;


_fail:
    ec = nai_errno;
    nsi_message_close(m);
    nai_errno = ec;
    r = -1;
    goto _end;
};


static int nsi_proxy_sendm(nsi_endpoint_conn_t* c, const void* buf, int len)
{
    int r;
    nai_bufvec_t v;

    v.buf = (uint8_t*)buf;
    v.len = len;
    r = nsi_proxy_sendv(c, &v, 1);

    return r;
};


int nsi_proxy_send(nsi_endpoint_conn_t* c, 
    nsi_message_t* m, uint32_t uid, int flags)
{
    intptr_t r;
    int ec;
    nsi_routing_t* p;
    nsi_routing_endpoint_t* e;
    nsi_message_t* d;
    nsi_message_io_t io;


    /* checking before send */
    if (uid) {
        r = nsi_endpoint_set_opt(c->ep, NSI_EOPT_QUERY_UCODE, c->conn);
        if (r < 0) {
            goto _end;
        };

        /* has been sent */
        if (r == (int)uid) {
            if (!(flags & NSI_SEND_MSGDUP)) {
                nsi_message_close(m);
            };

            r = 0;
            goto _end;
        };
    };

    /* dup message before modify */
    if (flags & NSI_SEND_MSGDUP) {
        d = nsi_message_dup(m, 0);
        if (d == 0) {
            nai_log_alert(NSI_LOG_CORE, nai_errno,
                "the proxy failed to dupilcate message");
            r = -1;
            goto _end;
        };

        m = d;
        flags &= ~NSI_SEND_MSGDUP;
    } else {
        d = 0;
    };

    if (!nsi_is_proxy_message(m)) {
        /* we need add 'inst' to the end of message */
        r = nsi_message_write_start(&io, m);
        if (r < 0) {
            nai_log_error(NSI_LOG_CORE, nai_errno, 
                "start write instance to the end of proxy message failed");
            goto _fail;
        };

        r = nsi_message_reserve(&io, 64);
        if (r < 0) {
            nai_log_error(NSI_LOG_CORE, nai_errno, 
                "reserve space to the end of proxy message failed");
            goto _fail;
        };

        r = nsi_message_put_u2(&io, m->inst);
        if (r < 0) {
            nai_log_error(NSI_LOG_CORE, nai_errno, 
                "write instance to the end of proxy message failed");
            goto _fail;
        };

        r = nsi_message_put_u2(&io, m->flags);
        if (r < 0) {
            nai_log_error(NSI_LOG_CORE, nai_errno, 
                "write flags to the end of proxy message failed");
            goto _fail;
        };

        r = nsi_message_put_u4(&io, m->conn);
        if (r < 0) {
            nai_log_error(NSI_LOG_CORE, nai_errno, 
                "write connection id to the end of proxy message failed");
            goto _fail;
        };

        r = nsi_message_put_u1(&io, m->cred.type);
        if (r < 0) {
            nai_log_error(NSI_LOG_CORE, nai_errno, 
                "write cred.type to the end of proxy message failed");
            goto _fail;
        };

        r = nsi_message_write(&io, m->cred.data, m->cred.len);
        if (r < 0) {
            nai_log_error(NSI_LOG_CORE, nai_errno, 
                "write cred.data to the end of proxy message failed");
            goto _fail;
        };

        r = nsi_message_put_u1(&io, m->cred.len);
        if (r < 0) {
            nai_log_error(NSI_LOG_CORE, nai_errno, 
                "write cred.len to the end of proxy message failed");
            goto _fail;
        };

        r = nsi_message_write_end(&io);
        if (r < 0) {
            nai_log_error(NSI_LOG_CORE, nai_errno, 
                "finish write cred to the end of proxy message failed");
            goto _fail;
        };
    };

    m->conn = c->conn;

    e = nsi_routing_get_endpoint_ctx(c->ep);
    assert(e != 0);

    p = e->rt;
    r = nsi_endpoint_send(
        c->ep, m, uid, flags | (p->batch ? NSI_SEND_POSTED : 0));
    if (r < 0) {
        goto _fail;
    };

_end:
    return (int)r;

_fail:
    if (d != 0) {
        ec = nai_errno;
        nsi_message_close(d);
        nai_errno = ec;
    };
    goto _end;
};


int nsi_proxy_connect(nsi_endpoint_conn_t* c)
{
    int r;
    int ec;
    nsi_routing_t* p;
    nsi_routing_endpoint_t* e;
    nsi_message_t* m;
    nsi_message_io_t io;
    nsi_proxy_connect_t s;


    e = nsi_routing_get_endpoint_ctx(c->ep);
    assert(e != 0);

    p = e->rt;
    m = nsi_network_create_message(p->net, NSI_USAGE_SEND);
    if (m == 0) {
        nai_log_alert(NSI_LOG_CORE, 
            nai_errno, "create proxy message failed");
        r = -1;
        goto _end;
    };

    m->inst = 0;
    m->cookie = 0;
    m->conn = c->conn;
    m->hdr.serv = NSI_PROXY_SERV;
    m->hdr.method = NSI_PROXY_METHOD;
    m->hdr.len = NSI_MSGEXT_SIZE;
    m->hdr.protocol = NSI_PROTOCOL_VERSION;
    m->hdr.interface = NSI_DEFAULT_MAJOR;
    m->hdr.client = 0;
    m->hdr.session = 0;
    m->hdr.type = NSI_MT_REQUEST;
    m->hdr.code = 0;


    /* we need add 'inst' to the end of message */
    r = nsi_message_write_start(&io, m);
    if (r < 0) {
        nai_log_error(NSI_LOG_CORE, nai_errno, 
            "start write connect message failed");
        goto _fail;
    };

    s.cmd = NSI_PROXY_CMD_CONNECT;
    r = nsi_message_put_u1(&io, s.cmd);
    if (r < 0) {
        nai_log_error(NSI_LOG_CORE, nai_errno, 
            "write command of connect message failed");
        goto _fail;
    };

    r = nsi_conf_serialize(p->conf, &io);
    if (r < 0) {
        nai_log_error(NSI_LOG_CORE, nai_errno, 
            "write configuration of connect message failed");
        goto _fail;
    };

    r = nsi_message_write_end(&io);
    if (r < 0) {
        nai_log_error(NSI_LOG_CORE, nai_errno, 
            "finish write connect message failed");
        goto _fail;
    };

    r = nsi_proxy_send(c, m, 0, 0);
    if (r < 0) {
        goto _fail;
    };

_end:
    return r;

_fail:
    ec = nai_errno;
    nsi_message_close(m);
    nai_errno = ec;
    goto _end;
};


int nsi_proxy_available(nsi_endpoint_conn_t* c, 
    int op, nsi_client_t* d, 
    nsi_serv_t serv, nsi_inst_t inst, 
    nsi_version_t major, nsi_versmin_t minor)
{
    int r;
    nsi_proxy_service_t s = { 0 };


    s.cmd = NSI_PROXY_CMD_AVAILABLE;
    s.op = op;
    s.cid = d->cid;
    s.rid = d->cid;
    s.serv = serv;
    s.inst = inst;
    s.major = major;
    s.minor = minor;

    nsi_cred_init(&s.cred);

    r = nsi_proxy_sendm(c, &s, sizeof(s));

    return r;
};


int nsi_proxy_offer(nsi_endpoint_conn_t* c, 
    int op, nsi_client_t* q, 
    nsi_serv_t serv, nsi_inst_t inst, 
    nsi_version_t major, nsi_versmin_t minor)
{
    int r;
    nsi_proxy_service_t s = { 0 };


    s.cmd = NSI_PROXY_CMD_SERVICE;
    s.op = op;
    s.cid = q->cid;
    s.rid = q->cid;
    s.serv = serv;
    s.inst = inst;
    s.major = major;
    s.minor = minor;

    nsi_cred_copy(&s.cred, &q->cred);

    r = nsi_proxy_sendm(c, &s, sizeof(s));

    return r;
};


int nsi_proxy_request(nsi_endpoint_conn_t* c, 
    int op, nsi_client_t* d, nsi_client_t* q, 
    nsi_serv_t serv, nsi_inst_t inst, 
    nsi_version_t major, nsi_versmin_t minor)
{
    int r;
    nsi_proxy_service_t s = { 0 };


    s.cmd = NSI_PROXY_CMD_SERVICE;
    s.op = op;
    s.cid = d ? d->cid : 0;
    s.rid = q->cid;
    s.serv = serv;
    s.inst = inst;
    s.major = major;
    s.minor = minor;

    nsi_cred_copy(&s.cred, &q->cred);

    r = nsi_proxy_sendm(c, &s, sizeof(s));

    return r;
};


int nsi_proxy_event(nsi_endpoint_conn_t* c, 
    int op, nsi_client_t* d, nsi_client_t* q, 
    nsi_serv_t serv, nsi_inst_t inst, nsi_eid_t eid, 
    const nsi_event_info_t* info)
{
    int r;
    nsi_proxy_event_t s = { 0 };
    nai_bufvec_t v[2];


    s.cmd = NSI_PROXY_CMD_EVENT;
    s.op = op;
    s.cid = d ? d->cid : 0;
    s.rid = q->cid;
    s.serv = serv;
    s.inst = inst;
    s.eid = eid;

    if (info) {
        s.type = info->type;
        s.reliability = info->reliability;
    } else {
        s.type = NSI_ET_UNKNOWN;
        s.reliability = NSI_RT_UNKNOWN;
    };

    nsi_cred_copy(&s.cred, &q->cred);

    switch (op) {
    case NSI_SERVICE_OP_ADD:
    case NSI_SERVICE_OP_OFFER:
        s.ngroup = info->ngroup;
        v[0].buf = (uint8_t*)&s;
        v[0].len = sizeof(s);
        v[1].buf = (uint8_t*)info->groups;
        v[1].len = info->ngroup * sizeof(nsi_gid_t);
        r = nsi_proxy_sendv(c, v, 2);
        break;
    default:
        s.ngroup = 0;
        r = nsi_proxy_sendm(c, &s, sizeof(s));
        break;
    };

    return r;
};


int nsi_proxy_subscribe(nsi_endpoint_conn_t* c, 
    int op, nsi_client_t* d, nsi_client_t* q, 
    nsi_serv_t serv, nsi_inst_t inst, 
    nsi_gid_t gid, nsi_version_t major, nsi_eid_t eid, 
    uint32_t ttl)
{
    int r;
    nsi_proxy_subscribe_t s = { 0 };


    s.cmd = NSI_PROXY_CMD_SUBSCRIBE;
    s.op = op;
    s.cid = d ? d->cid : 0;
    s.rid = q->cid;
    s.serv = serv;
    s.inst = inst;
    s.gid = gid;
    s.major = major;
    s.eid = eid;
    s.ttl = ttl;

    nsi_cred_copy(&s.cred, &q->cred);

    r = nsi_proxy_sendm(c, &s, sizeof(s));

    return r;
};


int nsi_proxy_subscribe_ack(nsi_endpoint_conn_t* c, 
    int sult, nsi_client_t* d, 
    nsi_serv_t serv, nsi_inst_t inst, 
    nsi_gid_t gid, nsi_version_t major, nsi_eid_t eid, 
    uint32_t ttl)
{
    int r;
    nsi_proxy_subscribe_t s = { 0 };


    s.cmd = NSI_PROXY_CMD_SUBSCRIBE_ACK;
    s.sult = sult;
    s.cid = d->cid;
    s.rid = d->cid;
    s.serv = serv;
    s.inst = inst;
    s.gid = gid;
    s.major = major;
    s.eid = eid;
    s.ttl = ttl;

    nsi_cred_init(&s.cred);

    r = nsi_proxy_sendm(c, &s, sizeof(s));

    return r;
};


int nsi_proxy_error(nsi_endpoint_conn_t* c, 
    int cmd, int op, int error, nsi_cid_t cid, 
    nsi_serv_t serv, nsi_inst_t inst, 
    nsi_version_t major, nsi_versmin_t minor, 
    nsi_gid_t gid, nsi_eid_t eid)
{
    int r;
    nsi_proxy_error_t s;


    s.cmd = NSI_PROXY_CMD_ERROR;
    s.op = 0;
    s.cid = cid;
    s.serv = serv;
    s.inst = inst;
    s.major = major;
    s.minor = minor;
    s.gid = gid;
    s.eid = eid;
    s.error = error;
    s.failcmd = cmd;
    s.failop = op;

    r = nsi_proxy_sendm(c, &s, sizeof(s));

    return r;
};


int nsi_proxy_handle_message(
    nsi_routing_t* p, nsi_message_t* m, nsi_endpoint_t* ep)
{
    intptr_t r;
    int ec;
    int len;
    nai_buf_t* b;
    nsi_message_io_t io;
    nsi_event_info_t ei;
    nsi_cred_t* cred;
    nsi_client_t* c;
    nsi_client_t* s;
    nsi_service_t* se;
    nsi_service_info_t si;
    nsi_version_t major;
    nsi_versmin_t minor;
    nsi_endpoint_conn_t conn;
    union {
        struct {
            uint8_t cmd;
            uint8_t op;
            nsi_cid_t cid;
            nsi_cid_t rid;
        };
        nsi_proxy_connect_t con;
        nsi_proxy_error_t err;
        nsi_proxy_event_t evt;
        nsi_proxy_service_t ser;
        nsi_proxy_subscribe_t sub;
    } h;


    len = m->hdr.len;
    len -= NSI_MSGEXT_SIZE;
    if (len < (int)sizeof(uint8_t)) {
        nai_log_warn(NSI_LOG_CORE, 
            EINVAL, "invalid proxy message is too small");
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };


    r = nsi_message_read_start(&io, m, 0);
    if (r < 0) {
        nai_log_alert(NSI_LOG_CORE, 
            nai_errno, "failed to read proxy message");
        goto _end;
    };


    r = nsi_message_get_u1(&io, &h.cmd);
    if (r < 0) {
        nai_log_alert(NSI_LOG_CORE, 
            nai_errno, "failed to read proxy command");
        goto _end;
    };

    switch (h.cmd) {
    case NSI_PROXY_CMD_CONNECT:
        break;
    case NSI_PROXY_CMD_SERVICE:
    case NSI_PROXY_CMD_AVAILABLE:
        if (len < (int)sizeof(h.ser)) {
            nai_log_warn(NSI_LOG_CORE, EINVAL, 
                "invalid proxy service message(%d) is too small", h.cmd);
            nai_errno = EINVAL;
            r = -1;
            goto _end;
        };

        r = nsi_message_read_least(
            &io, &h.ser.op, sizeof(h.ser) - sizeof(h.cmd));
        if (r < 0) {
            nai_log_alert(NSI_LOG_CORE, nai_errno, 
                "failed to read proxy service message(%d)", h.cmd);
            goto _end;
        };
        break;

    case NSI_PROXY_CMD_EVENT:
        if (len < (int)sizeof(h.evt)) {
            nai_log_warn(NSI_LOG_CORE, EINVAL, 
                "invalid proxy event message(%d) is too small", h.cmd);
            nai_errno = EINVAL;
            r = -1;
            goto _end;
        };

        r = nsi_message_read_least(
            &io, &h.evt.op, sizeof(h.evt) - sizeof(h.cmd));
        if (r < 0) {
            nai_log_alert(NSI_LOG_CORE, nai_errno, 
                "failed to read proxy event message(%d)", h.cmd);
            goto _end;
        };

        if (len < (int)(sizeof(h.evt) + h.evt.ngroup * sizeof(nsi_gid_t))) {
            nai_log_warn(NSI_LOG_CORE, EINVAL, 
                "proxy message(%d) without valid groups", h.cmd);
            nai_errno = EINVAL;
            r = -1;
            goto _end;
        };

        b = nai_buf_alloc(m->payload.pool, h.evt.ngroup * sizeof(nsi_gid_t));
        if (b == 0) {
            nai_log_alert(NSI_LOG_CORE, 
                nai_errno, "failed to allocate buffer to store groups");
            r = -1;
            goto _end;
        };

        nsi_message_read(&io, nai_buf_ptr(b), nai_buf_total(b));
        nai_buf_wcommit(b, nai_buf_total(b));
        nai_buflist_insert_tail(&m->payload, b);

        ei.type = h.evt.type;
        ei.reliability = h.evt.reliability;
        ei.ngroup = h.evt.ngroup;
        ei.groups = (nsi_gid_t*)nai_buf_ptr(b);
        break;

    case NSI_PROXY_CMD_SUBSCRIBE:
    case NSI_PROXY_CMD_SUBSCRIBE_ACK:
        if (len < (int)sizeof(h.sub)) {
            nai_log_warn(NSI_LOG_CORE, EINVAL, 
                "invalid proxy subscribe message(%d) is too small", h.cmd);
            nai_errno = EINVAL;
            r = -1;
            goto _end;
        };

        r = nsi_message_read_least(
            &io, &h.sub.op, sizeof(h.sub) - sizeof(h.cmd));
        if (r < 0) {
            nai_log_alert(NSI_LOG_CORE, nai_errno, 
                "failed to read proxy subscribe message(%d)", h.cmd);
            goto _end;
        };
        break;

    case NSI_PROXY_CMD_ERROR:
        if (len < (int)sizeof(h.err)) {
            nai_log_warn(NSI_LOG_CORE, EINVAL, 
                "invalid proxy error message(%d) is too small", h.cmd);
            nai_errno = EINVAL;
            r = -1;
            goto _end;
        };

        r = nsi_message_read_least(
            &io, &h.err.op, sizeof(h.err) - sizeof(h.cmd));
        if (r < 0) {
            nai_log_alert(NSI_LOG_CORE, nai_errno, 
                "failed to read proxy error message(%d)", h.cmd);
            goto _end;
        };
        break;

    default:
        nai_log_warn(NSI_LOG_CORE, EINVAL, 
            "unknown proxy error message(%d)", h.cmd);
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };


    if (ep == 0) {
        /* for local commands, 
         * which generated by the btach of routing. 
         */

        c = nsi_client_find_in_routing(p, h.cid);
        if (c == 0) {
            nai_log_error(NSI_LOG_CORE, nai_errno, 
                "cannot find client(0x%x) of proxy message(%d)", h.cid, h.cmd);
            r = -1;
            goto _fail;
        };

        switch (h.cmd) {
        case NSI_PROXY_CMD_EVENT:
        case NSI_PROXY_CMD_SERVICE:
        case NSI_PROXY_CMD_SUBSCRIBE:
            s = nsi_client_find_in_routing(p, h.rid);
            if (s == 0) {
                nai_log_error(NSI_LOG_CORE, nai_errno, 
                    "cannot find request client(0x%x) of proxy message(%d)", 
                    h.rid, h.cmd);
                r = -1;
                goto _fail;
            };
            break;
        default:
            s = 0;
            break;
        };

        switch (h.cmd) {
        case NSI_PROXY_CMD_SERVICE:
            nsi_client_handle_request(c, h.op, s, 
                h.ser.serv, h.ser.inst, h.ser.major, h.ser.minor, 1);
            break;
        case NSI_PROXY_CMD_AVAILABLE:
            nsi_client_handle_available(c, h.op, 
                h.ser.serv, h.ser.inst, h.ser.major, h.ser.minor, 1);
            break;
        case NSI_PROXY_CMD_EVENT:
            nsi_client_handle_event(c, h.op, s, 
                h.evt.serv, h.evt.inst, h.evt.eid, &ei, 1);
            break;
        case NSI_PROXY_CMD_SUBSCRIBE:
            nsi_client_handle_subscribe(c, h.op, s, 
                h.sub.serv, h.sub.inst, 
                h.sub.gid, h.sub.major, h.sub.eid, h.sub.ttl, 1);
            break;
        case NSI_PROXY_CMD_SUBSCRIBE_ACK:
            nsi_client_handle_subscribe_ack(c, h.sub.sult, 
                h.sub.serv, h.sub.inst, 
                h.sub.gid, h.sub.major, h.sub.eid, h.sub.ttl, 0, 1);
            break;
        default:
            nai_errno = EINVAL;
            r = -1;
            goto _fail;
        };

    } else {
        /* for remote commands */

        if (h.cmd == NSI_PROXY_CMD_CONNECT) {
            if (p->type != NSI_RTYPE_ROUTE || p->sd == 0) {
                nai_log_warn(NSI_LOG_CORE, nai_errno, 
                    "not in normal mode, igrone proxy connect message");
                r = -1;
                goto _fail;
            };

            c = nsi_client_create_anon(p, ep, m->conn, &m->cred);
            if (c == 0) {
                nai_log_error(NSI_LOG_CORE, nai_errno, 
                    "failed to create anon client "
                    "when handle proxy connect message");
                r = -1;
                goto _fail;
            };

            r = nsi_routing_proxy_connect(p, c, &io);
            if (r < 0) {
                ec = nai_errno;
                nai_log_error(NSI_LOG_CORE, ec, 
                    "failed to attach configuration of proxy client.");

                conn.ep = ep;
                conn.conn = m->conn;
                nsi_proxy_error(&conn, h.cmd, 0, ec, 
                    0, 0, 0, 
                    0, 0, 
                    0, 0);
                r = -1;
                goto _fail;
            };

        } else if (h.rid >= NSI_CLIENT_ID_ANON && 
            h.rid < NSI_CLIENT_ID_TEMP) {

            if (p->type != NSI_RTYPE_PROXY) {
                nai_log_warn(NSI_LOG_CORE, nai_errno, 
                    "not in proxy mode, igrone unknown client(0x%x) message", 
                    h.rid);
                r = -1;
                goto _fail;
            };
            if (h.cmd != NSI_PROXY_CMD_AVAILABLE) {
                nai_log_warn(NSI_LOG_CORE, nai_errno, 
                    "not an available message, "
                    "igrone unknown client(0x%x) message", h.rid);
                r = -1;
                goto _fail;
            };

            c = 0;

        } else {

            c = nsi_client_find_in_routing(p, h.rid);
            if (c == 0 && h.cmd != NSI_PROXY_CMD_ERROR) {
                switch (h.cmd) {
                case NSI_PROXY_CMD_SERVICE:
                    cred = &h.ser.cred;
                    break;
                case NSI_PROXY_CMD_EVENT:
                    cred = &h.evt.cred;
                    break;
                case NSI_PROXY_CMD_SUBSCRIBE:
                    cred = &h.sub.cred;
                    break;
                default:
                    /* client is closed */
                    goto _fail;
                };

                if (p->type == NSI_RTYPE_ROUTE) {
                    cred = &m->cred;
                } else if (cred->type == NSI_CRED_LOCAL) {
                    cred = &m->cred;
                } else {
                    r = nsi_proxy_verify_cred(cred);
                    if (r == 0) {
                        nai_errno = EINVAL;
                        nai_log_error(NSI_LOG_CORE, nai_errno, 
                            "the proxy message(%d) from client(0x%x) "
                            "with an invalid cred", h.cmd, h.rid);
                        r = -1;
                        goto _fail;
                    };
                };

                c = nsi_client_create(p, h.rid, ep, m->conn, cred);
                if (c == 0) {
                    nai_log_error(NSI_LOG_CORE, nai_errno, 
                        "failed to create request client(0x%x) "
                        "when handle proxy message(%d)", h.rid, h.cmd);
                    r = -1;
                    goto _fail;
                };
            };
        };

        switch (h.cmd) {
        case NSI_PROXY_CMD_CONNECT:
            nsi_discovery_request_service(
                p->sd, c, NSI_SERVICE_ANY, NSI_INSTANCE_ANY, 
                NSI_MAJOR_ANY, NSI_MINOR_ANY);

            /* ignore error */
            r = 0;
            break;
        case NSI_PROXY_CMD_SERVICE:
            nai_log_debug(NSI_LOG_CORE, 0, 
                "the proxy get a service(%d, %d) request(%d) "
                "from client(0x%x)", 
                h.ser.serv, h.ser.inst, h.op, c ? c->cid : 0);

            switch (h.op) {
            case NSI_SERVICE_OP_ADD:
                r = nsi_routing_request_service(
                    p, c, h.ser.serv, h.ser.inst, h.ser.major, h.ser.minor);
                break;
            case NSI_SERVICE_OP_OFFER:
                r = nsi_routing_offer_service(
                    p, c, h.ser.serv, h.ser.inst, h.ser.major, h.ser.minor);
                break;
            case NSI_SERVICE_OP_REMOVE:
            case NSI_SERVICE_OP_REMOVE_LAST:
                r = nsi_routing_release_service(
                    p, c, h.ser.serv, h.ser.inst, h.ser.major, h.ser.minor);
                break;
            default:
                nai_errno = EINVAL;
                r = -1;
                goto _fail;
            };

            if (r < 0) {
                ec = nai_errno;
                nai_log_error(NSI_LOG_CORE, ec, 
                    "proxy client(0x%x) failed to access(%d) service(%d, %d)", 
                    h.rid, h.op, h.ser.serv, h.ser.inst);

                /* reply error */
                conn.ep = ep;
                conn.conn = m->conn;
                nsi_proxy_error(&conn, h.cmd, h.op, ec, 
                    h.rid, h.ser.serv, h.ser.inst, 
                    h.ser.major, h.ser.minor, 
                    0, 0);
            };
            break;

        case NSI_PROXY_CMD_EVENT:
            switch (h.op) {
            case NSI_SERVICE_OP_ADD:
                r = nsi_routing_register_event(
                    p, c, h.evt.serv, h.evt.inst, h.evt.eid, &ei, 0);
                break;
            case NSI_SERVICE_OP_OFFER:
                r = nsi_routing_register_event(
                    p, c, h.evt.serv, h.evt.inst, h.evt.eid, &ei, 1);
                break;
            case NSI_SERVICE_OP_REMOVE:
            case NSI_SERVICE_OP_REMOVE_LAST:
                r = nsi_routing_unregister_event(
                    p, c, h.evt.serv, h.evt.inst, h.evt.eid);
                break;
            default:
                nai_errno = EINVAL;
                r = -1;
                break;
            };

            if (r < 0) {
                ec = nai_errno;
                nai_log_error(NSI_LOG_CORE, ec, 
                    "proxy client(0x%x) "
                    "failed to access(%d) event(%d) of service(%d, %d)", 
                    h.rid, h.op, h.evt.eid, h.evt.serv, h.evt.inst);

                /* reply error */
                conn.ep = ep;
                conn.conn = m->conn;
                nsi_proxy_error(&conn, h.cmd, h.op, ec, 
                    h.rid, h.evt.serv, h.evt.inst, 
                    0, 0, 
                    0, h.evt.eid);
            };
            break;

        case NSI_PROXY_CMD_AVAILABLE:

#if 0
            /* the routing should allow forward available message */
            if (p->type != NSI_RTYPE_PROXY) {
                break;
            };
#endif

            nai_log_debug(NSI_LOG_CORE, 0, 
                "the proxy get a available(%d) to client(0x%x)", 
                h.op, c ? c->cid : 0);

            se = nsi_service_find_in_routing(p, h.ser.serv, h.ser.inst);
            if (se != 0 && 
                se->own == 0 && !nsi_service_is_closed(se)) {

                switch (h.op) {
                case NSI_AVAIL_CHANGED:
                case NSI_AVAIL_ONLINED:
                    major = se->major;
                    minor = se->minor;
                    if ((major == NSI_MAJOR_ANY || major == h.ser.major) && 
                        (minor == NSI_MINOR_ANY || minor == h.ser.minor)) {
                        h.op = NSI_AVAIL_ONLINED;
                    } else {
                        h.op = NSI_AVAIL_CHANGED;
                    };

                    se->major = h.ser.major;
                    se->minor = h.ser.minor;
                    break;
                default:
                    break;
                };
                /* proxy save stat only */
                r = nsi_service_handle_available(se, h.op);
            };

            if (p->sd != 0 && 
                !nsi_routing_is_local_service(p, h.ser.serv, h.ser.inst)) {
                si.local = 0;
                si.owner = 0;
                si.ttl = h.op == NSI_AVAIL_OFFLINED ? 0 : -1;
                si.cyclic_delay = 0;
                si.serv = h.ser.serv;
                si.inst = h.ser.inst;
                si.major = h.ser.major;
                si.minor = h.ser.minor;
                nai_sockname_init(&si.names[0]);
                nai_sockname_init(&si.names[1]);
                nsi_discovery_set_service_info(p->sd, &si, 0);
            };

            if (c != 0) {
                r = nsi_client_handle_available(c, 
                    h.op, h.ser.serv, h.ser.inst, h.ser.major, h.ser.minor, 0);
            };
            break;

        case NSI_PROXY_CMD_SUBSCRIBE:
            switch (h.op) {
            case NSI_SERVICE_OP_ADD:
            case NSI_SERVICE_OP_OFFER:
                r = nsi_routing_subscribe(
                    p, c, h.sub.serv, h.sub.inst, 
                    h.sub.gid, h.sub.major, h.sub.eid, 
                    h.sub.ttl, 0, 0);
                break;
            case NSI_SERVICE_OP_REMOVE:
            case NSI_SERVICE_OP_REMOVE_LAST:
                r = nsi_routing_unsubscribe(
                    p, c, h.sub.serv, h.sub.inst, 
                    h.sub.gid, h.sub.major, h.sub.eid);
                break;
            default:
                nai_errno = EINVAL;
                r = -1;
                goto _fail;
            };
            if (r < 0) {
                nai_log_error(NSI_LOG_CORE, nai_errno, 
                    "proxy client(0x%x) "
                    "failed to access(%d) group(%d) of service(%d, %d)", 
                    h.rid, h.op, h.sub.gid, h.sub.serv, h.sub.inst);

                /* reply nack */
                conn.ep = ep;
                conn.conn = m->conn;
                nsi_proxy_subscribe_ack(&conn, NSI_E_NOT_OK, 
                    c, h.sub.serv, h.sub.inst, 
                    h.sub.gid, h.sub.major, h.sub.eid, 0);
            };

            /* try to close a temprary client */
            nsi_client_try_close(c);
            break;

        case NSI_PROXY_CMD_SUBSCRIBE_ACK:
            /* lookup service */
            se = nsi_service_find_in_routing(p, h.sub.serv, h.sub.inst);
            if (se == 0) {
                nai_log_error(NSI_LOG_CORE, nai_errno, 
                    "cannot find service(%d, %d) to ack by proxy client(0x%x)", 
                    h.sub.serv, h.sub.inst, h.rid);
                r = -1;
                goto _fail;
            };

            r = nsi_service_subscribe_ack(se, c, 
                h.sub.sult, h.sub.gid, h.sub.major, h.sub.eid, h.sub.ttl, 0);
            break;


        case NSI_PROXY_CMD_ERROR:
            nai_log_error(NSI_LOG_CORE, h.err.error, 
                "proxy client(0x%x) get a error: %d, "
                "cmd '%s', op '%s', serv %d, inst %d, major %d, minor %d, "
                "gid: %d, eid: %d", 
                h.err.cid, h.err.error, 
                nsi_proxy_cmdnames[h.err.failcmd], 
                nsi_proxy_opnames[h.err.failop], 
                h.err.serv, h.err.inst, h.err.major, h.err.minor, 
                h.err.gid, h.err.eid);

            if (h.err.failcmd == NSI_PROXY_CMD_CONNECT) {
                nai_log_error(NSI_LOG_CORE, 
                    h.err.error, "proxy connect failed");
                assert(0);
            };
            r = 0;
            break;

        default:
            nai_errno = EINVAL;
            r = -1;
            goto _fail;
        };
    };

_fail:
    (void)r;

    nsi_message_close(m);
    r = 0;

_end:
    return (int)r;
};

