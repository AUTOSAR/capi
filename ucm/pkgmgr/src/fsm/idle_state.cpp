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
/// @file       idle_state.cpp
/// @brief      Idle state implementation
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
/// @unit_name=IdleState
/// @unit_description=Idle state implementation
/// @endcode
///
/// ================================================================

#include "idle_state.h"

#include "fsm_manager.h"

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @brief ProcessSwPackage
/// @param id TransferIdType
/// @return future with result
/// @throws no
AraFutureVoid IdleState::ProcessSwPackage(TransferIdType const& id) noexcept
{
    FsmManager* const owner{owner_};
    owner_->SwitchWithoutLock(PackageManagerStatusType::kProcessing);
    return owner->mProcessExecutor.RunInThread(id);
}

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara
