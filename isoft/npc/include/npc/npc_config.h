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
/// @file       npc_config.h
/// @brief
/// @details
/// @date       2022-07-09
/// @author     xn
/// @version    1.2.0
///
/// ================================================================

#ifndef _CONFIG_H_NPC
#define _CONFIG_H_NPC

#include "nai/nai_config.h"
#include "npc/config/npc_config.h"

#if defined(_WIN32)

    #if defined(NPC_DLL)
        #define NPC_EXTERN __declspec(dllexport)
    #elif !defined(NPC_STATIC)
        #define NPC_EXTERN __declspec(dllimport)
    #else
        #define NPC_EXTERN /* nothing */
    #endif

#elif __GNUC__ >= 4

    #if defined(NPC_DLL)
        #define NPC_EXTERN __attribute__((visibility("default")))
    #else
        #define NPC_EXTERN /* nothing */
    #endif

#else
    #define NPC_EXTERN /* nothing */
#endif

#define NPC_INLINE inline

#endif
