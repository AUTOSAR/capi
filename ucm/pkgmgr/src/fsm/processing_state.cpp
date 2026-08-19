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
/// @file       processing_state.cpp
/// @brief      Implementation of Processing state class
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
/// @unit_name=ProcessingState
/// @unit_description=Implementation of Processing state class
/// @endcode
///
/// ================================================================

#include "processing_state.h"

#include "common/errc.h"
#include "common/log.h"
#include "common/strtype.h"
#include "fsm/fsm_manager.h"
#include "util/future_ext.h"

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @brief ProcessSwPackage
/// @param id TransferIdType
/// @return future with result
/// @throws no
AraFutureVoid ProcessingState::ProcessSwPackage(TransferIdType const& id) noexcept
{
    LOGD << "call...";
    AraString const func{__func__};

    std::ignore = id;
    return SyncCallVoid([&]() { ReturnVoidErrcEnumWithLongLog(func, kServiceBusy); });
}

/// @brief Cancel
/// @param id TransferIdType
/// @return future with result
/// @throws no
AraFutureVoid ProcessingState::Cancel(TransferIdType const& id) noexcept
{
    LOGD << "call...";
    AraString const func{__func__};

    ASYNC_CALL_VOID_BEGIN_WITH_FUNC_ID(func, id) { return owner_->mProcessExecutor.Cancel(id); }
    ASYNC_CALL_END();
}

/// @brief RevertProcessedSwPackages
/// @return future with result
/// @throws no
AraFutureVoid ProcessingState::RevertProcessedSwPackages() noexcept
{
    LOGD << "call...";

    ASYNC_CALL_VOID_BEGIN()
    {
        AraResultVoid const ret{owner_->mProcessExecutor.CancelForRevert()};
        if (!ret.HasValue()) {
            return ret;
        }

        FsmManager* const owner{owner_};
        owner->Switch(PackageManagerStatusType::kCleaningUp, FinalActionType::kRevert);
        return owner->mCleanupExecutor.RunInLocal();
    }
    ASYNC_CALL_END();
}

/// @brief OnProcessSwPackageSucceeded
/// @throws no
void ProcessingState::OnProcessSwPackageSucceeded() noexcept
{
    LOGD << "call...";
    owner_->SwitchWithoutLock(PackageManagerStatusType::kReady);
}

/// @brief OnProcessSwPackageFailed
/// @throws no
void ProcessingState::OnProcessSwPackageFailed() noexcept
{
    LOGD << "call...";

    AraResult< GetSwClusterChangeInfoOutput > const ret{owner_->swclManager.GetSwClusterChangeInfo().GetResult()};
    std::size_t const sz{ret.HasValue() ? ret.Value().SwInfo.size() : 0U};

    PackageManagerStatusType const st{(0U == sz) ? PackageManagerStatusType::kIdle : PackageManagerStatusType::kReady};
    owner_->SwitchWithoutLock(st);
}

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara
