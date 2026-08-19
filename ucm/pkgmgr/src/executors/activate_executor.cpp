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
/// @file       activate_executor.cpp
/// @brief      Activate executor implementation
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
/// @unit_name=ActivateExecutor
/// @unit_description=Activate executor implementation
/// @endcode
///
/// ================================================================

#include "activate_executor.h"

#include "common/errc.h"
#include "common/log.h"
#include "fsm/fsm_manager.h"
#include "util/future_ext.h"

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @brief Perform the activation
/// @return the future with result
AraFutureVoid ActivateExecutor::RunInThread() noexcept
{
    LOGD << "call...";

    ASYNC_CALL_VOID_BEGIN()
    {
        AraResultVoid const ret{Activate()};
        if (!ret.HasValue()) {
            std::ignore = owner_->OnFailure();
            return ret;
        }
        return owner_->OnSuccess();
    }
    ASYNC_CALL_END();
}

/// @brief Perform the activation
/// @return result
AraResultVoid ActivateExecutor::Activate() const noexcept
{
    LOGD << "call...";

    // Are dependencies satisfied?
    if (!_dependencyCheck()) {
        ReturnVoidErrcEnumWithLongLog("check dependency failed", kMissingDependencies);
    }

    // Notify SM: StartUpdateSession()
    LOGD << "call sm::StartUpdateSession";
    AraResultVoid ret{owner_->pSM->StartUpdateSession().GetResult()};
    if (!ret.HasValue()) {
        ReturnVoidErrcEnumWithLongLog("StartUpdateSession failed:" << ret.Error() << ret.Error().Message().data(),
                                      kUpdateSessionRejected);
    }

    // UCM shall compute the order of the State Management UpdateRequest Service Interface PrepareUpdate, VerifyUpdate
    // and PrepareRollback method calls from the dependency model included in the Software Cluster manifests.
    LOGD << "call SwclFGChange";
    ret = owner_->mHelpExecutor.SwclFGChange(HelpExecutor::SwclUpdateType::kSwclActivate);
    if (!ret.HasValue()) {
        ReturnVoidErrcWithLongLog("SwclFGChange failed", GetAraResultErrc(ret));
    }

    // Software clusters not of PLATFORM_CORE category: after PrepareUpdate in Activating state (related applications have been closed); MergeVar
    LOGD << "call MergeVars";
    ret = owner_->swclManager.MergeVars();
    if (!ret.HasValue()) {
        // Should not call RevertChanges here, because if Activate fails, it will go back to Ready, and then Activate can be called again
        /// AraResultVoid revertResult = SWCLManager_->RevertChanges();
        // TODO:
        // If calling StopUpdateSession() fails, how to know and handle??? Its FireAndForget is true, so it cannot have a return value???
        owner_->pSM->StopUpdateSession();
        ReturnVoidErrcWithLongLog("MergeVars failed and called StopUpdateSession", GetAraResultErrc(ret));
    }

    LOGD << "end with success";
    return {};
}

/// @brief check dependency
/// @throws no
/// @return bool
bool ActivateExecutor::_dependencyCheck() const
{
    LOGD << "call...";
    bool ret{true};
    AraMap< AraString, Version > mapSwcl2UpdatedVersion;

    // Get all available SWCLs --- GetSWCLsForActivation now indeed has this meaning (although not fully implemented yet)
    for (SoftwareCluster const& swcl : FileSystemSWCLManager::GetSWCLsForActivation()) {
        AraString const swclFQN{swcl.GetSwclManifest().fQN};
        Version const swclUpdatedVersion{swcl.GetSwclManifest().version};
        mapSwcl2UpdatedVersion[swclFQN] = swclUpdatedVersion;
    }

    // UCM has to be able to update several SoftwareClusters for an update campaign.
    // However, these SoftwareClusters could have dependencies not satisfied if updates
    // are processed and activated one by one. Therefore, UCM splits the activation action
    // from the general package processing.

    // [SWS_UCM_00026] Dependency Check dAt activation (i.e. after Activate method
    // is called), UCM shall perform a dependency check to ensure that all the Software
    // Packages having dependencies toward each other have been processed successfully,
    // otherwise return ApplicationError MissingDependencies.c(RS_UCM_00007)

    // SWCLManager_->GetSWCLsForActivation here should return: software to be installed/updated
    // cluster (should not include those with kPresent status)? no, should include because: Deleted ones may affect kPresent ones. Build the list of dependencies from
    // the manifest data
    for (SoftwareCluster const& swcl : FileSystemSWCLManager::GetSWCLsForActivation()) {
        AraString const swclFQN{swcl.GetSwclManifest().fQN};

        // Dependency check
        LOGD << "call dependsOnFormula's Evaluate for swclFQN:" << swclFQN.c_str();
        SoftwareClusterDependencyFormula formula{swcl.GetSwclManifest().dependsOnFormula};
        bool res{formula.Evaluate(mapSwcl2UpdatedVersion)};
        if (!res) {
            ret = false;
            break;
        }

        // Conflict check
        LOGD << "call conflictsToFormula's Evaluate for swclFQN:" << swclFQN.c_str();
        formula = swcl.GetSwclManifest().conflictsTo;
        res     = formula.Evaluate(mapSwcl2UpdatedVersion);
        if (res) {
            ret = false;
            break;
        }
    }

    LOGD << "end with ret:" << ret;
    return ret;
}

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara
