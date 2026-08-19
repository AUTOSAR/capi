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
/// @file       activated_state.cpp
/// @brief      Activated state implementation
/// @details
/// @date       2024-01-01
/// @author     cuiyinli
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/UCM/FsmManager
/// @interface_level=unit
/// @trace_id_sr=SR_UCM_00001
/// @unit_name=ActivatedState
/// @unit_description=Handles the activated state of the package manager.
/// @endcode
///
/// ================================================================

#include "activated_state.h"

#include "common/log.h"
#include "fsm_manager.h"

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @brief Finish
/// @return future with result
/// @throws no
AraFutureVoid ActivatedState::Finish()
{
    LOGD << "call...";
    FsmManager* const owner{owner_};
    owner->SwitchWithoutLock(PackageManagerStatusType::kCleaningUp);
    return owner->mCleanupExecutor.RunInThread();
}

/// @brief Rollback
/// @return future with result
/// @throws no
AraFutureVoid ActivatedState::Rollback()
{
    LOGD << "call...";
    FsmManager* const owner{owner_};
    owner->SwitchWithoutLock(PackageManagerStatusType::kRollingBack);

    // now even if the rollback is executed in background and the promise is removed from the container,
    // it is still guaranteed to be held by promisePtr variable
    return owner->mRollbackExecutor.RunInThread();
}

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara
