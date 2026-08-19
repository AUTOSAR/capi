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
/// @file       icc_endpoint_msgbox.c
/// @brief
/// @details
/// @date       2026-02-13
/// @author     jiusen.cui
/// @version    1.2.0
///
/// ================================================================

#include "icc/core/icc_log.h"
#include "icc_network.h"
#include "nai/io/nai_io.h"
#include "nai/runtime/nai_errno.h"
#include "nai/runtime/nai_util.h"

int icc_msgbox_endpoint_open(icc_endpoint_t* e)
{
    (void)e;
    return 0;
};

int icc_msgbox_endpoint_send(icc_endpoint_t* e, icc_message_t* m, uint32_t uid, int flags)
{
    (void)e;
    (void)m;
    (void)uid;
    (void)flags;
    return 0;
};

int icc_msgbox_endpoint_flush(icc_endpoint_t* e)
{
    (void)e;
    return 0;
};

int icc_msgbox_endpoint_close(icc_endpoint_t* e)
{
    (void)e;
    return 0;
};

icc_endpoint_ops_t icc_endpoint_msgbox_ops
    = {icc_msgbox_endpoint_open, icc_msgbox_endpoint_send, icc_msgbox_endpoint_flush, icc_msgbox_endpoint_close};
