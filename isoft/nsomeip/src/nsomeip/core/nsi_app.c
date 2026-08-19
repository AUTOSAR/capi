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
/// @file       nsi_app.c
/// @brief
/// @details
/// @date       2021-05-16
/// @author     xn
/// @version    1.2.0
///
/// ================================================================

#include "nsomeip/core/nsi_app.h"
#include "nsomeip/net/nsi_message.h"
#include "nsomeip/net/nsi_network.h"
#include "nsi_routing_impl.h"
#include "nai/runtime/nai_errno.h"



typedef struct nsi_app_service_args_s {
    nsi_app_t* app;
    nsi_serv_t serv;
    nsi_inst_t inst;
    nsi_version_t major;
    nsi_versmin_t minor;
} nsi_app_service_args_t;


typedef struct nsi_app_event_args_s {
    nsi_app_t* app;
    nsi_serv_t serv;
    nsi_inst_t inst;
    nsi_eid_t event;
    nsi_event_info_t* info;
} nsi_app_event_args_t;


typedef struct nsi_app_subscribe_args_s {
    nsi_app_t* app;
    nsi_serv_t serv;
    nsi_inst_t inst;
    nsi_gid_t group;
    nsi_version_t major;
    nsi_eid_t event;
} nsi_app_subscribe_args_t;


typedef struct nsi_app_subscribe_ack_args_s {
    nsi_app_t* app;
    int sult;
    nsi_cid_t cid;
    nsi_serv_t serv;
    nsi_inst_t inst;
    nsi_gid_t group;
    nsi_version_t major;
    nsi_eid_t event;
} nsi_app_subscribe_ack_args_t;


typedef struct nsi_app_execute_args_s {
    nsi_app_t* app;
    nsi_app_execute_f cb;
    void* ud;
} nsi_app_execute_args_t;



static int nsi_app_open_stub(nsi_routing_t* p, void* ctx)
{
    int r;
    nsi_app_t* a;

    a = (nsi_app_t*)ctx;
    r = nsi_routing_add_app(p, a);

    return r;
};


static int nsi_app_close_stub(nsi_routing_t* p, void* ctx)
{
    int r;
    nsi_app_t* a;

    a = (nsi_app_t*)ctx;
    r = nsi_routing_remove_app(p, a);

    return r;
};


static int nsi_app_offer_service_stub(nsi_routing_t* p, void* ctx)
{
    int r;
    nsi_app_service_args_t* a;

    a = (nsi_app_service_args_t*)ctx;
    r = nsi_routing_offer_service(
        p, a->app->client, a->serv, a->inst, a->major, a->minor);

    return r;
};


static int nsi_app_stop_service_stub(nsi_routing_t* p, void* ctx)
{
    int r;
    nsi_app_service_args_t* a;

    a = (nsi_app_service_args_t*)ctx;
    r = nsi_routing_stop_service(
        p, a->app->client, a->serv, a->inst, a->major, a->minor);

    return r;
};


static int nsi_app_offer_event_stub(nsi_routing_t* p, void* ctx)
{
    int r;
    nsi_app_event_args_t* a;

    a = (nsi_app_event_args_t*)ctx;
    r = nsi_routing_register_event(
        p, a->app->client, a->serv, a->inst, a->event, a->info, 1);

    return r;
};


static int nsi_app_stop_event_stub(nsi_routing_t* p, void* ctx)
{
    int r;
    nsi_app_event_args_t* a;

    a = (nsi_app_event_args_t*)ctx;
    r = nsi_routing_unregister_event(
        p, a->app->client, a->serv, a->inst, a->event);

    return r;
};


static int nsi_app_request_service_stub(nsi_routing_t* p, void* ctx)
{
    int r;
    nsi_app_service_args_t* a;

    a = (nsi_app_service_args_t*)ctx;
    r = nsi_routing_request_service(
        p, a->app->client, a->serv, a->inst, a->major, a->minor);

    return r;
};


static int nsi_app_release_service_stub(nsi_routing_t* p, void* ctx)
{
    int r;
    nsi_app_service_args_t* a;

    a = (nsi_app_service_args_t*)ctx;
    r = nsi_routing_release_service(
        p, a->app->client, a->serv, a->inst, a->major, a->minor);

    return r;
};


static int nsi_app_request_event_stub(nsi_routing_t* p, void* ctx)
{
    int r;
    nsi_app_event_args_t* a;

    a = (nsi_app_event_args_t*)ctx;
    r = nsi_routing_register_event(
        p, a->app->client, a->serv, a->inst, a->event, a->info, 0);

    return r;
};


static int nsi_app_release_event_stub(nsi_routing_t* p, void* ctx)
{
    int r;
    nsi_app_event_args_t* a;

    a = (nsi_app_event_args_t*)ctx;
    r = nsi_routing_unregister_event(
        p, a->app->client, a->serv, a->inst, a->event);

    return r;
};


static int nsi_app_subscribe_stub(nsi_routing_t* p, void* ctx)
{
    int r;
    nsi_app_subscribe_args_t* a;

    a = (nsi_app_subscribe_args_t*)ctx;
    r = nsi_routing_subscribe(
        p, a->app->client, a->serv, a->inst, 
        a->group, a->major, a->event, -1, 0, 0);

    return r;
};


static int nsi_app_unsubscribe_stub(nsi_routing_t* p, void* ctx)
{
    int r;
    nsi_app_subscribe_args_t* a;

    a = (nsi_app_subscribe_args_t*)ctx;
    r = nsi_routing_unsubscribe(
        p, a->app->client, a->serv, a->inst, a->group, a->major, a->event);

    return r;
};


static int nsi_app_subscribe_ack_stub(nsi_routing_t* p, void* ctx)
{
    int r;
    nsi_app_subscribe_ack_args_t* a;

    a = (nsi_app_subscribe_ack_args_t*)ctx;
    r = nsi_routing_subscribe_ack(
        p, a->app->client, a->sult, 
        a->cid, a->serv, a->inst, a->group, a->event);

    return r;
};


static int nsi_app_execute_stub(nsi_routing_t* p, void* ctx)
{
    int r;
    nsi_app_execute_args_t* a;


    (void)p;

    a = (nsi_app_execute_args_t*)ctx;
    a->cb(a->app, a->ud);
    r = 0;

    return r;
};



int nsi_app_init(nsi_app_t* p)
{
    int r;


    p->uid = 0;
    p->cid = 0;
    p->client = 0;
    p->rt = 0;
    p->ud = 0;
    p->sentcb = 0;
    p->msgcb = 0;
    p->subcb = 0;
    p->ackcb = 0;
    p->availcb = 0;
    p->offercb = 0;
    p->findcb = 0;
    r = 0;

    return r;
};


int nsi_app_open(nsi_app_t* p, nsi_routing_t* rt, nsi_cid_t cid)
{
    int r;

    if (p == 0) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };
    if (p->rt) {
        nai_errno = EPERM;
        r = -1;
        goto _end;
    };

    p->cid = cid;
    r = nsi_routing_exec(rt, nsi_app_open_stub, p);

_end:
    return r;
};


int nsi_app_open_name(nsi_app_t* p, nsi_routing_t* rt, const char* name)
{
    int r;
    nsi_cid_t cid;
    nai_str_t str;


    if (rt == 0 || 
        rt->conf == 0) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    nai_str_sets(&str, name);
    r = nsi_conf_get_client_id(rt->conf, &str, &cid);
    if (r < 0) {
        goto _end;
    };

    r = nsi_app_open(p, rt, cid);

_end:
    return r;
};


int nsi_app_close(nsi_app_t* p)
{
    int r;


    if (p == 0) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };
    if (p->rt == 0) {
        r = 0;
        goto _end;
    };

    r = nsi_routing_exec(p->rt, nsi_app_close_stub, p);

_end:
    return r;
};


int nsi_app_send(nsi_app_t* p, nsi_message_t* m)
{
    int r;

    r = nsi_routing_send(p->rt, m);

    return r;
};


int nsi_app_execute(nsi_app_t* p, nsi_app_execute_f cb, void* ud)
{
    nsi_routing_t* q;
    nsi_app_execute_args_t args = {
        p, cb, ud
    };

    q = p->rt;
    return nsi_routing_exec(q, nsi_app_execute_stub, &args);
};


int nsi_app_offer_service(nsi_app_t* p, 
    nsi_serv_t serv, nsi_inst_t inst, 
    nsi_version_t major, nsi_versmin_t minor)
{
    nsi_routing_t* q;
    nsi_app_service_args_t args = {
        p, serv, inst, major, minor
    };

    q = p->rt;
    return nsi_routing_exec(q, nsi_app_offer_service_stub, &args);
};


int nsi_app_stop_service(nsi_app_t* p, 
    nsi_serv_t serv, nsi_inst_t inst, 
    nsi_version_t major, nsi_versmin_t minor)
{
    nsi_routing_t* q;
    nsi_app_service_args_t args = {
        p, serv, inst, major, minor
    };

    q = p->rt;
    return nsi_routing_exec(q, nsi_app_stop_service_stub, &args);
};


int nsi_app_offer_event(nsi_app_t* p, 
    nsi_serv_t serv, nsi_inst_t inst, 
    nsi_eid_t event, const nsi_event_info_t* info)
{
    nsi_routing_t* q;
    nsi_app_event_args_t args = {
        p, serv, inst, event, (nsi_event_info_t*)info
    };

    q = p->rt;
    return nsi_routing_exec(q, nsi_app_offer_event_stub, &args);
};


int nsi_app_stop_event(nsi_app_t* p, 
    nsi_serv_t serv, nsi_inst_t inst, nsi_eid_t event)
{
    nsi_routing_t* q;
    nsi_app_event_args_t args = {
        p, serv, inst, event, 0
    };

    q = p->rt;
    return nsi_routing_exec(q, nsi_app_stop_event_stub, &args);
};


int nsi_app_request_service(nsi_app_t* p, 
    nsi_serv_t serv, nsi_inst_t inst, 
    nsi_version_t major, nsi_versmin_t minor)
{
    nsi_routing_t* q;
    nsi_app_service_args_t args = {
        p, serv, inst, major, minor
    };

    q = p->rt;
    return nsi_routing_exec(q, nsi_app_request_service_stub, &args);
};


int nsi_app_release_service(nsi_app_t* p, 
    nsi_serv_t serv, nsi_inst_t inst, 
    nsi_version_t major, nsi_versmin_t minor)
{
    nsi_routing_t* q;
    nsi_app_service_args_t args = {
        p, serv, inst, major, minor
    };

    q = p->rt;
    return nsi_routing_exec(q, nsi_app_release_service_stub, &args);
};


int nsi_app_request_event(nsi_app_t* p, 
    nsi_serv_t serv, nsi_inst_t inst, 
    nsi_eid_t event, const nsi_event_info_t* info)
{
    nsi_routing_t* q;
    nsi_app_event_args_t args = {
        p, serv, inst, event, (nsi_event_info_t*)info
    };

    q = p->rt;
    return nsi_routing_exec(q, nsi_app_request_event_stub, &args);
};


int nsi_app_release_event(nsi_app_t* p, 
    nsi_serv_t serv, nsi_inst_t inst, nsi_eid_t event)
{
    nsi_routing_t* q;
    nsi_app_event_args_t args = {
        p, serv, inst, event
    };

    q = p->rt;
    return nsi_routing_exec(q, nsi_app_release_event_stub, &args);
};


int nsi_app_subscribe(nsi_app_t* p, 
    nsi_serv_t serv, nsi_inst_t inst, 
    nsi_gid_t group, nsi_version_t major, nsi_eid_t event)
{
    nsi_routing_t* q;
    nsi_app_subscribe_args_t args = {
        p, serv, inst, group, major, event
    };

    q = p->rt;
    return nsi_routing_exec(q, nsi_app_subscribe_stub, &args);
};


int nsi_app_unsubscribe(nsi_app_t* p, 
    nsi_serv_t serv, nsi_inst_t inst, 
    nsi_gid_t group, nsi_version_t major, nsi_eid_t event)
{
    nsi_routing_t* q;
    nsi_app_subscribe_args_t args = {
        p, serv, inst, group, major, event
    };

    q = p->rt;
    return nsi_routing_exec(q, nsi_app_unsubscribe_stub, &args);
};


int nsi_app_subscribe_ack(nsi_app_t* p, 
    int sult, nsi_cid_t cid, nsi_serv_t serv, nsi_inst_t inst, 
    nsi_gid_t group, nsi_eid_t event)
{
    nsi_routing_t* q;
    nsi_app_subscribe_ack_args_t args = {
        p, sult, cid, serv, inst, group, 0, event
    };

    q = p->rt;
    return nsi_routing_exec(q, nsi_app_subscribe_ack_stub, &args);
};


nsi_message_t* nsi_app_create_message(nsi_app_t* p)
{
    nsi_routing_t* q;
    nsi_message_t* m;

    q = p->rt;
    m = nsi_network_create_message(q->net, NSI_USAGE_SEND);

    return m;
};


nsi_message_t* nsi_app_create_request(
    nsi_app_t* p, nsi_serv_t serv, nsi_inst_t inst, nsi_method_t meth)
{
    nsi_routing_t* q;
    nsi_message_t* m;


    q = p->rt;
    m = nsi_network_create_message(q->net, NSI_USAGE_SEND);
    if (m != 0) {
        m->conn = 0;
        m->inst = inst;
        m->hdr.serv = serv;
        m->hdr.method = meth;
        m->hdr.len = NSI_MSGEXT_SIZE;
        m->hdr.client = (uint16_t)p->cid;
        m->hdr.session = 0;
        m->hdr.protocol = NSI_PROTOCOL_VERSION;
        m->hdr.interface = NSI_DEFAULT_MAJOR;
        m->hdr.type = NSI_MT_REQUEST;
        m->hdr.code = NSI_E_OK;
    };

    return m;
};


nsi_message_t* nsi_app_create_response(
    nsi_app_t* p, nsi_message_t* r, uint8_t sult)
{
    nsi_routing_t* q;
    nsi_message_t* m;


    q = p->rt;
    m = nsi_network_create_message(q->net, NSI_USAGE_SEND);
    if (m != 0) {
        m->reliable = r->reliable;
        m->conn = r->conn;
        m->inst = r->inst;
        m->hdr.serv = r->hdr.serv;
        m->hdr.method = r->hdr.method;
        m->hdr.len = NSI_MSGEXT_SIZE;
        m->hdr.client = r->hdr.client;
        m->hdr.session = r->hdr.session;
        m->hdr.protocol = r->hdr.protocol;
        m->hdr.interface = r->hdr.interface;
        m->hdr.type = sult == NSI_E_OK ? NSI_MT_RESPONSE : NSI_MT_ERROR;
        m->hdr.code = sult;
    };

    return m;
};


nsi_message_t* nsi_app_create_notification(
    nsi_app_t* p, nsi_serv_t serv, nsi_inst_t inst, nsi_eid_t event)
{
    nsi_routing_t* q;
    nsi_message_t* m;


    q = p->rt;
    m = nsi_network_create_message(q->net, NSI_USAGE_SEND);
    if (m != 0) {
        m->conn = 0;
        m->inst = inst;
        m->hdr.serv = serv;
        m->hdr.method = event;
        m->hdr.len = NSI_MSGEXT_SIZE;
        m->hdr.client = (uint16_t)p->cid;
        m->hdr.session = 0;
        m->hdr.protocol = NSI_PROTOCOL_VERSION;
        m->hdr.interface = NSI_DEFAULT_MAJOR;
        m->hdr.type = NSI_MT_NOTIFICATION;
        m->hdr.code = NSI_E_OK;
    };

    return m;
};


