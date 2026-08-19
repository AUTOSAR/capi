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
/// @file       cleanup_executor.cpp
/// @brief      Cleanup executor implementation
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
/// @unit_name=CleanupExecutor
/// @unit_description=Cleanup executor implementation
/// @endcode
///
/// ================================================================

#include "cleanup_executor.h"

#include "common/errc.h"
#include "common/log.h"
#include "common/path.h"
#include "common/strtype.h"
#include "fsm/fsm_manager.h"
#include "storage/ucm_database.h"
#include "util/future_ext.h"

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @brief Clean up the temporary files
///
/// TODO: Implementation is missing.
/// @uptrace={SWS_UCM_00158}
/// @return future with result
AraFutureVoid CleanupExecutor::RunInThread() const noexcept
{
    LOGD << "call...";

    ASYNC_CALL_VOID_BEGIN() { return RunInLocal(); }
    ASYNC_CALL_END();
}

/// @brief Clean up the temporary files
/// @return result
AraResultVoid CleanupExecutor::RunInLocal() const noexcept
{
    LOGD << "call...";
    AraResultVoid const ret{_cleanup()};
    if (!ret.HasValue()) {
        std::ignore = owner_->OnFailure();
    } else {
        owner_->Done();
    }

    return ret;
}

/// @brief Clean up the temporary files
/// @return result
/// @throws no
AraResultVoid CleanupExecutor::_cleanup() const noexcept
{
    AraList< GetHistoryType > actionsInfo{owner_->swclManager.GetReversibleActionsInfo()};
    FinalActionType const finalAction{owner_->GetFinalActionType()};
    LOGD << "start with finalAction:" << strtype::FinalActionTypeToStr(finalAction).c_str();

    if (finalAction == FinalActionType::kRevert) {
        LOGD << "call RevertChanges";

        AraResultVoid const ret{owner_->swclManager.RevertChanges()};
        if (!ret.HasValue()) {
            ReturnVoidErrcWithLongLog("RevertChanges failed", GetAraResultErrc(ret));
        }

        Save2History(actionsInfo, false);
        return {};
    }

    bool const platformCoreUpdated{FileSystemSWCLManager::IsPlatformCoreUpdated()};
    bool const nonPlatformCoreUpdated{FileSystemSWCLManager::IsNonPlatformCoreUpdated()};
    LOGD << "platformCoreUpdated:" << platformCoreUpdated << "nonPlatformCoreUpdated:" << nonPlatformCoreUpdated;
    ///AssertWithLog(platformCoreUpdated || nonPlatformCoreUpdated);.

    // If the code runs to SWCLManager_->CommitChanges() and then restarts, this situation may occur
    if (platformCoreUpdated) {  // If the update involves platform_core (executed before SWCLManager_->CommitChanges();)
        owner_->mABPartition.BootOptionFileCleanupA();
    }

    if (nonPlatformCoreUpdated) {  // If the update involves non-platform software sets
        owner_->mABPartition.SwclListFileCleanupA();
    }

    LOGD << "call CommitChanges";
    AraResultVoid const ret{owner_->swclManager.CommitChanges()};
    if (!ret.HasValue()) {
        ReturnVoidErrcEnumWithLongLog("CommitChanges failed", kOperationNotPermitted);
    }

    LOGD << "call ResetSWCLChangeInfo";
    owner_->swclManager.ResetSWCLChangeInfo();

    // Notify SM: StopUpdateSession()
    LOGD << "call StopUpdateSession";
    // TODO:
    // If calling StopUpdateSession() fails, how to know and handle??? Its FireAndForget is true, so it cannot have a return value???
    owner_->pSM->StopUpdateSession();

    Save2History(actionsInfo, true);
    return {};
}

/// @brief save actions
/// @param actionsInfo
/// @param success
/// @throws no
void CleanupExecutor::Save2History(AraList< GetHistoryType >& actionsInfo, bool const success) noexcept
{
    LOGD << "call with success:" << success;

    /// TODO: move the below lines to the method "PackageManagerCleaningUpState::Done()" once R20/11 branch release has
    /// been created rationale: to be compliant with the R21/11 version of the requirement [SWS_UCM_00115]
    ///
    /// copy of the actions realized by UCM during this update sequence

    // Set to Failed if verification fails
    if (!success) {
        for (GetHistoryType& it : actionsInfo) {
            it.Resolution = ResultType::kFailed;
        }
    }

    /// write those actions and related information into the UCM History file
    std::ignore = UcmDatabase(GetPathSettings().historyPath).SaveLastSnapshot(actionsInfo);
}

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara
