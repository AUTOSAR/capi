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
/// @file       npc_types.h
/// @brief
/// @details
/// @date       2025-12-29
/// @author     Zan Shigang
/// @version    1.2.0
///
/// ================================================================

#ifndef _TYPES_H_NPC
#define _TYPES_H_NPC

#pragma once

#include "nai/runtime/nai_types.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    typedef uint16_t npc_serv_t;
    typedef uint16_t npc_inst_t;
    typedef uint32_t npc_servinst_t;
    typedef uint16_t npc_method_t;
    typedef uint16_t npc_eid_t;
    typedef uint16_t npc_gid_t;
    typedef uint32_t npc_cid_t;
    typedef uint32_t npc_connid_t;
    typedef uint8_t npc_version_t;
    typedef uint32_t npc_versmin_t;
    typedef uint16_t npc_session_t;

    typedef union npc_servkey_s
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
        npc_servinst_t servinst;

    } npc_servkey_t;

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
