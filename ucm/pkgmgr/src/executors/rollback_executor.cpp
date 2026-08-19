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
/// @file       rollback_executor.cpp
/// @brief      RollbackExecutor implementation
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
/// @unit_name=RollbackExecutor
/// @unit_description=RollbackExecutor implementation
/// @endcode
///
/// ================================================================

#include "rollback_executor.h"

#include "common/assert.h"
#include "common/errc.h"
#include "common/log.h"
#include "fsm/fsm_manager.h"
#include "util/future_ext.h"

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @brief Perform the rollback
/// @return future with result
AraFutureVoid RollbackExecutor::RunInThread() noexcept
{
    LOGD << "call...";
    AraString const func{__func__};

    ASYNC_CALL_VOID_BEGIN_WITH_FUNC(func)
    {
        AraResultVoid const ret{_rollback()};
        if (ret.HasValue()) {
            LOGD << func.c_str() << " rollback done, call Done()";
            owner_->Done();
        }

        return ret;
    }
    ASYNC_CALL_END();
}

/// @brief rollback in local
/// @throws no
/// @return result
AraResultVoid RollbackExecutor::RunInLocal() const noexcept
{
    AraResultVoid const ret{_rollback()};
    if (ret.HasValue()) {
        LOGD << "rollback done, call Done()";
        owner_->DoneWithoutLock();
        return {};
    }

    ReturnVoidErrcWithLongLog("rollback failed", GetAraResultErrc(ret));
}

/// @brief _rollback
/// @throws no
/// @return result
AraResultVoid RollbackExecutor::_rollback() const noexcept
{
    LOGD << "call...";
    AraResultVoid ret;

    // UCM shall compute the order of the State Management UpdateRequest Service Interface PrepareUpdate, VerifyUpdate
    // and PrepareRollback method calls from the dependency model included in the Software Cluster manifests.
    LOGD << "call SwclFGChange";
    ret = owner_->mHelpExecutor.SwclFGChange(HelpExecutor::SwclUpdateType::kSwclRollback);
    if (!ret.HasValue()) {
        ReturnVoidErrcWithLongLog("SwclFGChange failed", GetAraResultErrc(ret));
    }

    // 2. If the update involves platform_core, delete the new version boot_option_ver(X+1).cfg and its md5 file;
    bool const platformCoreUpdated{
        FileSystemSWCLManager::IsPlatformCoreUpdated()};  // Update involves platform software set
    bool const nonPlatformCoreUpdated{
        FileSystemSWCLManager::IsNonPlatformCoreUpdated()};       // Update involves non-platform software sets
    bool const kOSUpdated{FileSystemSWCLManager::IsOSUpdated()};  // Update involves OS software set
    LOGD << "platformCoreUpdated:" << platformCoreUpdated << " nonPlatformCoreUpdated:" << nonPlatformCoreUpdated
         << " OSUpdated:" << kOSUpdated;
    AssertWithLog(platformCoreUpdated || nonPlatformCoreUpdated);

    if (platformCoreUpdated) {
        owner_->mABPartition.BootOptionFileRollbackToA();
    }

    // 3. Link the original application software set list to the appropriate platform_core software set (i.e., the core_version in boot_option_verX.cfg)
    //     Determine the target platform_core software set version
    //         If the status of platform_core is kPresent, then Version is the target platform_core software set version;   --- Will not be kAdded/kRemoved
    //         If the status of platform_core is kUpdated, then Version is the target platform_core software set version;.
    //     Clean the etc directory of the target platform_core software set version and create a new run_time_application_swcl_list.cfg
    //         Its process_list is application_swcl_list_verY.json, generate the corresponding md5;
    //         Generate the md5 of run_time_application_swcl_list.cfg
    owner_->mABPartition.SwclListFileRollbackToA(nonPlatformCoreUpdated);

    // 4. If the update involves OS, call updateOS active version to activate the old version of the OS;
    if (kOSUpdated) {
        ret = HelpExecutor::OSUpdateOperate(HelpExecutor::OSUpdateType::kOSRollback);
        AssertWithLog(ret.HasValue());
    }

    // // Link the original /ara/swcls/application_swcl_list_verX.json to /ara/core/1.0.0/etc/run_time_application_swcl_list.cfg
    ///AraString lastSWCLListFileName = GetSWCLListFileName(lastUsedSWCLListID_);
    ///ret = LinkSWCLList("1.0.0", lastSWCLListFileName);
    ///AssertWithLog(ret == true);.

    //       5. ResetMachine()/ReparseManifests();
    //            5.1 If a restart is involved
    //                5.1.1 If conversion from a new UCM version to an old UCM version is involved
    //                      Convert the current new version content in ucm_status.json to the content corresponding to the old version (still the current state RollingBack), and write it to ucm_status.json;.
    //                5.1.2 Generate ucm_status.json and its corresponding ucm_status.md5, so that it can enter the RollingBack state to continue execution after restart
    //                5.1.3 Notify SM: restart the OS and wait for the restart
    //                5.1.4 After the restart is complete, it will enter the RollingBack state to continue execution;
    //            5.2 If a restart is not involved
    //                 1. Notify EM: ReparseManifests() to parse (reload) the original old software set list application_swcl_list_verY.json;
    //       6. Switch the UCM state machine to RolledBack;
    // ResetMachine() or ReparseManifests()
    ret = owner_->mHelpExecutor.RebootOrReparse(HelpExecutor::SwclUpdateType::kSwclRollback);
    return ret;
}

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara
