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
/// @file       icc_service.c
/// @brief
/// @details
/// @date       2025-11-27
/// @author     Zan Shigang
/// @version    1.2.0
///
/// ================================================================

#include "icc_service.h"

#include "icc/core/icc_app.h"
#include "icc/net/icc_message.h"
#include "icc_client.h"
#include "icc_log.h"
#include "icc_routing_impl.h"
#include "nai/runtime/nai_errno.h"
#include "nai/runtime/nai_util.h"

int icc_service_send(icc_service_t* s, icc_message_t* m)
{
    int r;

    r = icc_endpoint_send(s->ep, m, 0, 0);
    if (r < 0) {
        nai_log_error(ICC_LOG_CORE, nai_errno, "send message(%d, %d) failed", s->serv, s->inst);
    };

    return r;
};

int icc_service_offer(icc_service_t* s, icc_client_t* c, icc_version_t major, icc_versmin_t minor)
{
    int r;

    (void)major;
    (void)minor;

    r = 0;

    c->offer  = 1;
    s->client = c;

    nai_log_info(ICC_LOG_CORE, 0, "icc service offer serv:inst:major:minor (%d:%d:%d:%d)", s->serv, s->inst, major,
                 minor);

    icc_client_handle_available(c, 1, s->serv, s->inst, major, minor, 0);

    return r;
};

int icc_service_stop(icc_service_t* s, icc_client_t* c, icc_version_t major, icc_versmin_t minor)
{
    int r;

    (void)major;
    (void)minor;
    (void)c;

    r         = 0;
    s->client = 0;

    return r;
};

int icc_service_request(icc_service_t* s, icc_client_t* c, icc_version_t major, icc_versmin_t minor)
{
    int r;

    r         = 0;
    c->offer  = 0;
    s->client = c;

    nai_log_info(ICC_LOG_CORE, 0, "icc service request serv:inst:major,minor (%d:%d:%d:%d)", s->serv, s->inst, major,
                 minor);

    icc_client_handle_available(c, 1, s->serv, s->inst, major, minor, 0);

    return r;
};

int icc_service_release(icc_service_t* s, icc_client_t* c, icc_version_t major, icc_versmin_t minor)
{
    int r;

    (void)major;
    (void)minor;
    (void)c;

    r         = 0;
    s->client = 0;

    return r;
};

int icc_service_register(icc_service_t* s, icc_client_t* c, icc_eid_t eid, const icc_event_info_t* info, int offer)
{
    int r;

    (void)c;
    (void)s;
    (void)eid;
    (void)info;
    (void)offer;

    r = 0;

    return r;
};

int icc_service_unregister(icc_service_t* s, icc_client_t* c, icc_eid_t eid)
{
    int r;

    (void)s;
    (void)c;
    (void)eid;

    r = 0;

    return r;
};

int icc_service_subscribe(icc_service_t* s, icc_client_t* c, icc_gid_t group, icc_version_t major, icc_eid_t event)
{
    int r;

    r = 0;
    nai_log_info(ICC_LOG_CORE, 0, "icc service subscribe serv:inst:group:major (%d:%d:%d:%d)", s->serv, s->inst, group,
                 major);

    s->client->meth = event;

    icc_client_handle_subscribe_ack(c, 0, s->serv, s->inst, group, major, event, -1, 0, 0);

    return r;
};

int icc_service_unsubscribe(icc_service_t* s, icc_client_t* c, icc_gid_t group, icc_version_t major, icc_eid_t event)
{
    int r;

    r = 0;
    icc_client_handle_subscribe_ack(c, 1, s->serv, s->inst, group, major, event, -1, 0, 0);

    return r;
};

int icc_service_subscribe_ack(
    icc_service_t* s, icc_client_t* c, int sult, icc_gid_t group, icc_version_t major, icc_eid_t event)
{
    int r;

    (void)s;
    (void)c;
    (void)sult;
    (void)group;
    (void)major;
    (void)event;

    r = 0;

    return r;
};
