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
/// @file       verifying_state.cpp
/// @brief      Verifying state implementation
/// @details
/// @date       2024-08-15
/// @author     cuiyinli
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/UCM/FsmManager
/// @interface_level=unit
/// @trace_id_sr=SR_UCM_00001
/// @unit_name=VerifyingState
/// @unit_description=Verifying state implementation
/// @endcode
///
/// ================================================================

#include "verifying_state.h"

#include "common/errc.h"
#include "common/log.h"
#include "fsm_manager.h"

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @brief Rollback the system to the state before the packages were processed.
/// @return The result of the rollback operation
/// @throws no
AraFutureVoid VerifyingState::Rollback()
{
    LOGD << "call...";
    FsmManager* const owner{owner_};
    owner->SwitchWithoutLock(PackageManagerStatusType::kRollingBack);
    return owner->mRollbackExecutor.RunInThread();
}

/// @brief OnSuccess
/// @throws no
/// @return result
AraResultVoid VerifyingState::OnSuccess() noexcept
{
    LOGD << "call...";
    owner_->SwitchWithoutLock(PackageManagerStatusType::kActivated);
    // Currently, the Activated state has no work content
    return {};
}

/// @brief OnFailure
/// @throws no
/// @return result
AraResultVoid VerifyingState::OnFailure() noexcept
{
    LOGD << "call...";
    FsmManager* const owner{owner_};
    owner->SwitchWithoutLock(PackageManagerStatusType::kRollingBack);
    return owner->mRollbackExecutor.RunInLocal();
}

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara
