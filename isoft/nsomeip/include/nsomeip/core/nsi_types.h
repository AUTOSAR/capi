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
/// @file       nsi_types.h
/// @brief
/// @details
/// @date       2021-05-11
/// @author     xn
/// @version    1.2.0
///
/// ================================================================

#ifndef _TYPES_H_NSI
#define _TYPES_H_NSI

#pragma once

#include "nai/runtime/nai_types.h"
#include "nsomeip/nsi_config.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    typedef uint16_t nsi_serv_t;
    typedef uint16_t nsi_inst_t;
    typedef uint32_t nsi_servinst_t;
    typedef uint16_t nsi_method_t;
    typedef uint16_t nsi_eid_t;
    typedef uint16_t nsi_gid_t;
    typedef uint32_t nsi_cid_t;
    typedef uint32_t nsi_connid_t;
    typedef uint8_t nsi_version_t;
    typedef uint32_t nsi_versmin_t;
    typedef uint16_t nsi_session_t;

    typedef union nsi_servkey_s
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
        nsi_servinst_t servinst;

    } nsi_servkey_t;

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
