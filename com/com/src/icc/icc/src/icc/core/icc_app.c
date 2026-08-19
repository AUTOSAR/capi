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
/// @file       icc_app.c
/// @brief
/// @details
/// @date       2026-02-13
/// @author     jiusen.cui
/// @version    1.2.0
///
/// ================================================================

#include "icc/core/icc_app.h"

#include "icc/net/icc_message.h"
#include "icc/net/icc_network.h"
#include "icc_routing_impl.h"
#include "nai/runtime/nai_errno.h"

typedef struct icc_app_service_args_s
{
    icc_app_t* app;
    icc_serv_t serv;
    icc_inst_t inst;
    icc_version_t major;
    icc_versmin_t minor;
} icc_app_service_args_t;

typedef struct icc_app_event_args_s
{
    icc_app_t* app;
    icc_serv_t serv;
    icc_inst_t inst;
    icc_eid_t event;
    icc_event_info_t* info;
} icc_app_event_args_t;

typedef struct icc_app_subscribe_args_s
{
    icc_app_t* app;
    icc_serv_t serv;
    icc_inst_t inst;
    icc_gid_t group;
    icc_version_t major;
    icc_eid_t event;
} icc_app_subscribe_args_t;

typedef struct icc_app_subscribe_ack_args_s
{
    icc_app_t* app;
    int sult;
    icc_cid_t cid;
    icc_serv_t serv;
    icc_inst_t inst;
    icc_gid_t group;
    icc_version_t major;
    icc_eid_t event;
} icc_app_subscribe_ack_args_t;

typedef struct icc_app_execute_args_s
{
    icc_app_t* app;
    icc_app_execute_f cb;
    void* ud;
} icc_app_execute_args_t;

static int icc_app_open_stub(icc_routing_t* p, void* ctx)
{
    int r;
    icc_app_t* a;

    a = (icc_app_t*)ctx;
    r = icc_routing_add_app(p, a);

    return r;
};

static int icc_app_close_stub(icc_routing_t* p, void* ctx)
{
    int r;
    icc_app_t* a;

    a = (icc_app_t*)ctx;
    r = icc_routing_remove_app(p, a);

    return r;
};

static int icc_app_offer_service_stub(icc_routing_t* p, void* ctx)
{
    int r;
    icc_app_service_args_t* a;

    a = (icc_app_service_args_t*)ctx;
    r = icc_routing_offer_service(p, a->app->client, a->serv, a->inst, a->major, a->minor);

    return r;
};

static int icc_app_stop_service_stub(icc_routing_t* p, void* ctx)
{
    int r;
    icc_app_service_args_t* a;

    a = (icc_app_service_args_t*)ctx;
    r = icc_routing_stop_service(p, a->app->client, a->serv, a->inst, a->major, a->minor);

    return r;
};

static int icc_app_offer_event_stub(icc_routing_t* p, void* ctx)
{
    int r;
    icc_app_event_args_t* a;

    a = (icc_app_event_args_t*)ctx;
    r = icc_routing_register_event(p, a->app->client, a->serv, a->inst, a->event, a->info, 1);

    return r;
};

static int icc_app_stop_event_stub(icc_routing_t* p, void* ctx)
{
    int r;
    icc_app_event_args_t* a;

    a = (icc_app_event_args_t*)ctx;
    r = icc_routing_unregister_event(p, a->app->client, a->serv, a->inst, a->event);

    return r;
};

static int icc_app_request_service_stub(icc_routing_t* p, void* ctx)
{
    int r;
    icc_app_service_args_t* a;

    a = (icc_app_service_args_t*)ctx;
    r = icc_routing_request_service(p, a->app->client, a->serv, a->inst, a->major, a->minor);

    return r;
};

static int icc_app_release_service_stub(icc_routing_t* p, void* ctx)
{
    int r;
    icc_app_service_args_t* a;

    a = (icc_app_service_args_t*)ctx;
    r = icc_routing_release_service(p, a->app->client, a->serv, a->inst, a->major, a->minor);

    return r;
};

static int icc_app_request_event_stub(icc_routing_t* p, void* ctx)
{
    int r;
    icc_app_event_args_t* a;

    a = (icc_app_event_args_t*)ctx;
    r = icc_routing_register_event(p, a->app->client, a->serv, a->inst, a->event, a->info, 0);

    return r;
};

static int icc_app_release_event_stub(icc_routing_t* p, void* ctx)
{
    int r;
    icc_app_event_args_t* a;

    a = (icc_app_event_args_t*)ctx;
    r = icc_routing_unregister_event(p, a->app->client, a->serv, a->inst, a->event);

    return r;
};

static int icc_app_subscribe_stub(icc_routing_t* p, void* ctx)
{
    int r;
    icc_app_subscribe_args_t* a;

    a = (icc_app_subscribe_args_t*)ctx;
    r = icc_routing_subscribe(p, a->app->client, a->serv, a->inst, a->group, a->major, a->event);

    return r;
};

static int icc_app_unsubscribe_stub(icc_routing_t* p, void* ctx)
{
    int r;
    icc_app_subscribe_args_t* a;

    a = (icc_app_subscribe_args_t*)ctx;
    r = icc_routing_unsubscribe(p, a->app->client, a->serv, a->inst, a->group, a->major, a->event);

    return r;
};

static int icc_app_subscribe_ack_stub(icc_routing_t* p, void* ctx)
{
    int r;
    icc_app_subscribe_ack_args_t* a;

    a = (icc_app_subscribe_ack_args_t*)ctx;
    r = icc_routing_subscribe_ack(p, a->app->client, a->sult, a->cid, a->serv, a->inst, a->group, a->event);

    return r;
};

static int icc_app_execute_stub(icc_routing_t* p, void* ctx)
{
    int r;
    icc_app_execute_args_t* a;

    (void)p;

    a = (icc_app_execute_args_t*)ctx;
    a->cb(a->app, a->ud);
    r = 0;

    return r;
};

int icc_app_init(icc_app_t* p)
{
    int r;

    p->uid     = 0;
    p->cid     = 0;
    p->client  = 0;
    p->rt      = 0;
    p->ud      = 0;
    p->sentcb  = 0;
    p->msgcb   = 0;
    p->subcb   = 0;
    p->ackcb   = 0;
    p->availcb = 0;
    p->offercb = 0;
    p->findcb  = 0;
    r          = 0;

    return r;
};

int icc_app_open(icc_app_t* p, icc_routing_t* rt, icc_cid_t cid)
{
    int r;

    if (p == 0) {
        nai_errno = EINVAL;
        r         = -1;
        goto _end;
    };
    if (p->rt) {
        nai_errno = EPERM;
        r         = -1;
        goto _end;
    };

    p->cid = cid;
    r      = icc_routing_exec(rt, icc_app_open_stub, p);

_end:
    return r;
};

int icc_app_open_name(icc_app_t* p, icc_routing_t* rt, const char* name)
{
    int r;
    icc_cid_t cid;
    nai_str_t str;

    if (rt == 0 || rt->conf == 0) {
        nai_errno = EINVAL;
        r         = -1;
        goto _end;
    };

    nai_str_sets(&str, name);
    r = icc_conf_get_client_id(rt->conf, &str, &cid);
    if (r < 0) {
        goto _end;
    };

    r = icc_app_open(p, rt, cid);

_end:
    return r;
};

int icc_app_close(icc_app_t* p)
{
    int r;

    if (p == 0) {
        nai_errno = EINVAL;
        r         = -1;
        goto _end;
    };
    if (p->rt == 0) {
        r = 0;
        goto _end;
    };

    r = icc_routing_exec(p->rt, icc_app_close_stub, p);

_end:
    return r;
};

int icc_app_send(icc_app_t* p, icc_message_t* m)
{
    int r;

    r = icc_routing_send(p->rt, m);

    return r;
};

int icc_app_execute(icc_app_t* p, icc_app_execute_f cb, void* ud)
{
    icc_routing_t* q;
    icc_app_execute_args_t args = {p, cb, ud};

    q = p->rt;
    return icc_routing_exec(q, icc_app_execute_stub, &args);
};

int icc_app_offer_service(icc_app_t* p, icc_serv_t serv, icc_inst_t inst, icc_version_t major, icc_versmin_t minor)
{
    icc_routing_t* q;
    icc_app_service_args_t args = {p, serv, inst, major, minor};

    q = p->rt;
    return icc_routing_exec(q, icc_app_offer_service_stub, &args);
};

int icc_app_stop_service(icc_app_t* p, icc_serv_t serv, icc_inst_t inst, icc_version_t major, icc_versmin_t minor)
{
    icc_routing_t* q;
    icc_app_service_args_t args = {p, serv, inst, major, minor};

    q = p->rt;
    return icc_routing_exec(q, icc_app_stop_service_stub, &args);
};

int icc_app_offer_event(icc_app_t* p, icc_serv_t serv, icc_inst_t inst, icc_eid_t event, const icc_event_info_t* info)
{
    icc_routing_t* q;
    icc_app_event_args_t args = {p, serv, inst, event, (icc_event_info_t*)info};

    q = p->rt;
    return icc_routing_exec(q, icc_app_offer_event_stub, &args);
};

int icc_app_stop_event(icc_app_t* p, icc_serv_t serv, icc_inst_t inst, icc_eid_t event)
{
    icc_routing_t* q;
    icc_app_event_args_t args = {p, serv, inst, event, 0};

    q = p->rt;
    return icc_routing_exec(q, icc_app_stop_event_stub, &args);
};

int icc_app_request_service(icc_app_t* p, icc_serv_t serv, icc_inst_t inst, icc_version_t major, icc_versmin_t minor)
{
    icc_routing_t* q;
    icc_app_service_args_t args = {p, serv, inst, major, minor};

    q = p->rt;
    return icc_routing_exec(q, icc_app_request_service_stub, &args);
};

int icc_app_release_service(icc_app_t* p, icc_serv_t serv, icc_inst_t inst, icc_version_t major, icc_versmin_t minor)
{
    icc_routing_t* q;
    icc_app_service_args_t args = {p, serv, inst, major, minor};

    q = p->rt;
    return icc_routing_exec(q, icc_app_release_service_stub, &args);
};

int icc_app_request_event(icc_app_t* p, icc_serv_t serv, icc_inst_t inst, icc_eid_t event, const icc_event_info_t* info)
{
    icc_routing_t* q;
    icc_app_event_args_t args = {p, serv, inst, event, (icc_event_info_t*)info};

    q = p->rt;
    return icc_routing_exec(q, icc_app_request_event_stub, &args);
};

int icc_app_release_event(icc_app_t* p, icc_serv_t serv, icc_inst_t inst, icc_eid_t event)
{
    icc_routing_t* q;
    icc_app_event_args_t args = {p, serv, inst, event};

    q = p->rt;
    return icc_routing_exec(q, icc_app_release_event_stub, &args);
};

int icc_app_subscribe(
    icc_app_t* p, icc_serv_t serv, icc_inst_t inst, icc_gid_t group, icc_version_t major, icc_eid_t event)
{
    icc_routing_t* q;
    icc_app_subscribe_args_t args = {p, serv, inst, group, major, event};

    q = p->rt;
    return icc_routing_exec(q, icc_app_subscribe_stub, &args);
};

int icc_app_unsubscribe(
    icc_app_t* p, icc_serv_t serv, icc_inst_t inst, icc_gid_t group, icc_version_t major, icc_eid_t event)
{
    icc_routing_t* q;
    icc_app_subscribe_args_t args = {p, serv, inst, group, major, event};

    q = p->rt;
    return icc_routing_exec(q, icc_app_unsubscribe_stub, &args);
};

int icc_app_subscribe_ack(
    icc_app_t* p, int sult, icc_cid_t cid, icc_serv_t serv, icc_inst_t inst, icc_gid_t group, icc_eid_t event)
{
    icc_routing_t* q;
    icc_app_subscribe_ack_args_t args = {p, sult, cid, serv, inst, group, 0, event};

    q = p->rt;
    return icc_routing_exec(q, icc_app_subscribe_ack_stub, &args);
};

icc_message_t* icc_app_create_message(icc_app_t* p)
{
    icc_routing_t* q;
    icc_message_t* m;

    q = p->rt;
    m = icc_network_create_message(q->net, ICC_USAGE_SEND);

    return m;
};

icc_message_t* icc_app_create_request(icc_app_t* p, icc_serv_t serv, icc_inst_t inst, icc_method_t meth)
{
    icc_routing_t* q;
    icc_message_t* m;

    (void)serv;
    (void)inst;
    (void)meth;

    q           = p->rt;
    m           = icc_network_create_message(q->net, ICC_USAGE_SEND);
    m->hdr.serv = serv;
    m->inst     = inst;
    m->hdr.len  = 0;

    return m;
};

icc_message_t* icc_app_create_response(icc_app_t* p, icc_message_t* r, uint8_t sult)
{
    icc_routing_t* q;
    icc_message_t* m;

    (void)r;
    (void)sult;

    q = p->rt;
    m = icc_network_create_message(q->net, ICC_USAGE_SEND);

    return m;
};

icc_message_t* icc_app_create_notification(icc_app_t* p, icc_serv_t serv, icc_inst_t inst, icc_eid_t event)
{
    icc_routing_t* q;
    icc_message_t* m;

    (void)serv;
    (void)inst;
    (void)event;

    q           = p->rt;
    m           = icc_network_create_message(q->net, ICC_USAGE_SEND);
    m->hdr.serv = serv;
    m->inst     = inst;
    m->hdr.len  = 0;

    return m;
};
