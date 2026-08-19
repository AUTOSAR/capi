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
/// @file       initialization_ara.h
/// @brief      This file describes the global initialization and shutdown functions that initialize resp. deinitialize data structures and threads of the AUTOSAR Runtime for Adaptive Applications.
/// @details
/// @date       2022-06-24
/// @author     hanzhibo
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_CORE_INITIALIZATION_ARA_H_
#define ARA_CORE_INITIALIZATION_ARA_H_

#include <isoft/naicpp/global_evloop.h>

#include "ara/core/internal/initialization.h"
#ifdef HAS_ARA_LOG
    #include "ara/log/internal/initialization.h"
#endif

#ifdef HAS_ARA_DIAG
    #include "ara/diag/internal/runtime.h"
#endif

#ifdef HAS_ARA_COM
    #include "ara/com/internal/runtime.h"
#endif

#ifdef HAS_ARA_CRYPTO
    #include "ara/crypto/internal/initialize.h"
#endif

#ifdef HAS_ARA_PER
    #include "ara/per/internal/initialize.h"
#endif

#ifdef HAS_ARA_IDSM_CLIENT
    #include "ara/idsm/internal/initialization.h"
#endif

#ifdef HAS_ARA_FW_CLIENT
    #include "ara/fw/internal/initialization.h"
#endif

#if defined(HAS_ARA_EXEC_EXECUTION_CLIENT) || defined(HAS_ARA_EXEC_FIND_PROCESS_CLIENT)                                \
    || defined(HAS_ARA_EXEC_STATE_CLIENT) || defined(HAS_ARA_EXEC_UPDATE_CLIENT) || defined(HAS_ARA_PHM_WATCHDOG)      \
    || defined(HAS_ARA_PHM_RECOVERY_ACTION) || defined(HAS_ARA_PHM_CLIENT) || defined(HAS_ARA_TSYNC_TSCP)              \
    || defined(HAS_ARA_CRYPTO) || defined(HAS_ARA_IDSM_CLIENT) || defined(HAS_ARA_FW_CLIENT) || defined(HAS_ARA_LOG)   \
    || defined(HAS_NSOMEIP_BINDING) || defined(HAS_NPC_BINDING)
    #include "isoft/core/ipc/ipc.h"
#endif

namespace ara {
namespace core {
namespace internal {

// Initialize wrapper function

}  // namespace internal
}  // namespace core
}  // namespace ara

#endif  // ARA_CORE_INITIALIZATION_ARA_H_
