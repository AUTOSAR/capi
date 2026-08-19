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
/// @file       icc_types.h
/// @brief
/// @details
/// @date       2025-11-27
/// @author     Zan Shigang
/// @version    1.2.0
///
/// ================================================================

#ifndef _TYPES_H_ICC
#define _TYPES_H_ICC

#pragma once

#include "icc/icc_config.h"
#include "nai/runtime/nai_types.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    typedef uint16_t icc_serv_t;
    typedef uint16_t icc_inst_t;
    typedef uint32_t icc_servinst_t;
    typedef uint16_t icc_method_t;
    typedef uint16_t icc_eid_t;
    typedef uint16_t icc_gid_t;
    typedef uint32_t icc_cid_t;
    typedef uint32_t icc_connid_t;
    typedef uint8_t icc_version_t;
    typedef uint32_t icc_versmin_t;
    typedef uint16_t icc_session_t;

    typedef union icc_servkey_s
    {
        /**
     * keep serv is high word, 
     * so that the same serv is continuously in the tree.
     */
        struct
        {
            uint32_t inst : 16;
            uint32_t serv : 16;
        };
        icc_servinst_t servinst;

    } icc_servkey_t;

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
