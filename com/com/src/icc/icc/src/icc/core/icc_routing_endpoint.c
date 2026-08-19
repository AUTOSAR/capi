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
/// @file       icc_routing_endpoint.c
/// @brief
/// @details
/// @date       2025-11-27
/// @author     Zan Shigang
/// @version    1.2.0
///
/// ================================================================

#include "icc/core/icc_app.h"
#include "icc_client.h"
#include "icc_log.h"
#include "icc_routing_impl.h"
#include "icc_service.h"
#include "nai/runtime/nai_errno.h"
#include "nai/runtime/nai_util.h"

static int icc_routing_message(icc_endpoint_t* ep, icc_message_t* m)
{
    int r;
    icc_client_t* c;
    icc_service_t* s;

    s = icc_service_find_in_routing(ep->net->rt, m->hdr.serv, m->inst);
    if (s == 0) {
        nai_log_error(ICC_LOG_CORE, nai_errno, "cannot find service");
        goto _fail;
    };

    c = s->client;
    if (c == 0) {
        nai_log_error(ICC_LOG_CORE, nai_errno, "cannot find client for service");
        goto _fail;
    };

    if (!c->offer) {
        m->hdr.method = c->meth;
        m->hdr.type   = ICC_MT_NOTIFICATION;
    } else {
        m->hdr.type = ICC_MT_REQUEST;
    };

    r = icc_client_send(c, m, 0, 0);
    if (r < 0) {
        nai_log_error(ICC_LOG_CORE, nai_errno, "client send message fail");
        goto _fail;
    };

_end:
    return (int)r;

_fail:
    icc_message_close(m);
    r = 0;
    goto _end;
};

static int icc_routing_sent(icc_endpoint_t* p, icc_message_t* m, int errcode)
{
    int r;
    icc_client_t* c;
    icc_service_t* s;

    s = icc_service_find_in_routing(p->net->rt, m->hdr.serv, m->inst);
    if (s == 0) {
        nai_log_error(ICC_LOG_CORE, nai_errno, "cannot find service");
        r = -1;
        goto _end;
    };

    c = s->client;
    if (c == 0) {
        nai_log_error(ICC_LOG_CORE, nai_errno, "cannot find client for service");
        r = -1;
        goto _end;
    };

    r = icc_client_handle_sent(c, m, errcode);
    if (r < 0) {
        nai_log_error(ICC_LOG_CORE, nai_errno, "client handle sent message fail");
        r = 0;
    };

_end:
    return r;
};

icc_endpoint_cb_t icc_routing_endpoint_cb = {
    icc_routing_message,
    icc_routing_sent,
};
