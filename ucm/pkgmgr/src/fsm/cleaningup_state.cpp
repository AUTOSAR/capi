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
/// @file       cleaningup_state.cpp
/// @brief      CleaningUp state implementation
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
/// @unit_name=CleaningUpState
/// @unit_description=Handles the cleanup state of the package manager.
/// @endcode
///
/// ================================================================

#include "cleaningup_state.h"

#include "common/log.h"
#include "fsm_manager.h"

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @brief Creates a new instance of CleaningUp state and saves the current state
/// @param finalAction commit or revert, defines if all actions should be reverted in CLEANING_UP state
/// @throws no
CleaningUpState::CleaningUpState(FinalActionType const finalAction) noexcept
    : PackageManagerState{}, finalAction_{finalAction}
{
}

/// @brief OnFailure
/// @return result
/// @throws no
AraResultVoid CleaningUpState::OnFailure() noexcept
{
    LOGD << "call...";
    return {};
}

/// @brief Done
/// @throws no
void CleaningUpState::Done() noexcept
{
    LOGD << "call...";
    owner_->SwitchWithoutLock(PackageManagerStatusType::kIdle);
}

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara
