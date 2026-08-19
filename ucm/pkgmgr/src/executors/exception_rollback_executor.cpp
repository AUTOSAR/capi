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
/// @file       exception_rollback_executor.cpp
/// @brief      ExceptionRollbackExecutor implementation
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
/// @unit_name=ExceptionRollbackExecutor
/// @unit_description=ExceptionRollbackExecutor implementation
/// @endcode
///
/// ================================================================

#include "exception_rollback_executor.h"

#include "common/errc.h"
#include "common/log.h"
#include "common/path.h"
#include "fsm/fsm_manager.h"
#include "help_executor.h"
#include "util/future_ext.h"
#include "util/string_ext.h"

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @brief Clean up the temporary files
/// @return future with result
AraFutureVoid ExceptionRollbackExecutor::RunInThread() const noexcept
{
    LOGD << "call...";
    AraString const func{__func__};

    ASYNC_CALL_VOID_BEGIN_WITH_FUNC(func)
    {
        AraResultVoid ret{_doExceptionRollingBack()};
        if (!ret.HasValue()) {
            LOGE << func.c_str() << "_doExceptionRollingBack failed, call OnFailure()";
            std::ignore = owner_->OnFailure();
        } else {
            /// 6.
            // Generate ucm_status.json and its corresponding ucm_status.md5 (status is kExceptionCleaningUp), so that it can enter the kExceptionCleaningUp state to continue execution after restart
            LOGD << func.c_str() << "_doExceptionRollingBack done, call Done()";
            owner_->Done();

            /// 7. Notify SM: restart the OS and wait for the restart
            // Should ResetMachine() keep retrying on failure? No, it should be like 7.1.7 Status
            // Reporting: Could consider a WAITING_ROLLBACK state before ROLLING-BACK (where only Rollback can be called); kExceptionCleaningUp can call restart
            // Notify SM: ResetMachine()
            LOGD << "call ResetMachine";
            ret = owner_->pSM->ResetMachine().GetResult();
            if (ret) {
                // TODO: After future changes to the underlying library, remove this setting
                // Tell the client a restart is in progress? The client can also get the current status via CurrentStatus
                ReturnVoidErrcEnumWithLongLog("ResetMachine succeeded", kResettingMachine);
            }

            ReturnVoidErrcEnumWithLongLog("ResetMachine failed:" << ret.Error() << ret.Error().Message().data(),
                                          kGeneralReject);
        }

        return ret;
    }
    ASYNC_CALL_END();
}

/// @brief Clean up the temporary files
/// @throws no
/// @return result
AraResultVoid ExceptionRollbackExecutor::_doExceptionRollingBack() const noexcept
{
    LOGD << "call...";

    /// 1. Call StartUpdateSession()
    /// 2. Find all function groups in the machine manifest and call: PrepareRollBack(vector<FunctionGroup>)
    /// 3. Delete the new version boot_option_ver(X+1).cfg and its md5 file;
    /// 4. Link the original application software set list to the appropriate platform_core software set (i.e., the core_version in boot_option_verX.cfg)
    ///     Clean the etc directory of the target platform_core software set version and create a new run_time_application_swcl_list.cfg
    ///         Its process_list is application_swcl_list_verY.json, generate the corresponding md5
    ///         Generate the md5 of run_time_application_swcl_list.cfg
    /// 5. Delete the new version software set list application_swcl_list_ver(Y+1).json
    /// 6.
    /// Generate ucm_status.json and its corresponding ucm_status.md5 (status is kExceptionCleaningUp), so that it can enter the kExceptionCleaningUp state to continue execution after restart
    /// 7. Notify SM: restart the OS and wait for the restart

    /// 1. Call StartUpdateSession()
    LOGD << "call StartUpdateSession";
    AraResultVoid ret{owner_->pSM->StartUpdateSession().GetResult()};
    if (!ret.HasValue()) {
        LOGW << "Failed to StartUpdateSession with error:" << ret.Error() << ret.Error().Message().data()
             << ", but we can skip it.";

        // TODO: Change to only ignore kNotAllowedMultipleUpdateSessions in the next version
        // ReturnVoidErrcEnumWithLongLog("StartUpdateSession failed:" << ret.Error() << ret.Error().Message().data(),
        //                               kUpdateSessionRejected);
    }

    /// 2. Find all function groups in the machine manifest and call: PrepareRollBack(vector<FunctionGroup>)
    AraVectorString fgList{owner_->mHelpExecutor.GetAllFG()};
    HelpExecutor::SwclFGRemoveMachineFG(fgList);  // Remove machine function group
    LOGD << "call PrepareRollback with fgList.size:" << fgList.size()
         << " fgList:" << strutil::JoinVector(fgList).c_str();

    bool const success{owner_->pSM->RetryPrepareRollback(fgList)};
    if (!success) {
        ReturnVoidErrcEnumWithLongLog("PrepareRollback failed", kNotAbleToRollback);
    }

    /// 3. Delete the new version boot_option_ver(X+1).cfg and its md5 file;
    owner_->mABPartition.BootOptionFileExceptionRollbackToA();

    /// 4. Link the original application software set list to the appropriate platform_core software set (i.e., the core_version in boot_option_verX.cfg)
    ///     Clean the etc directory of the target platform_core software set version and create a new run_time_application_swcl_list.cfg
    ///         Its process_list is application_swcl_list_verY.json, generate the corresponding md5
    ///         Generate the md5 of run_time_application_swcl_list.cfg GenBootConfig
    /// 5. Delete the new version software set list application_swcl_list_ver(Y+1).json
    owner_->mABPartition.SwclListFileExceptionRollbackToA();

    // Therefore for the OS software set:
    //     Use GetSoftwareClusterStatus to get the current status and old/new versions
    //     If retrieval fails, it means it's still in the process state, no need to handle.
    //     If the current status is kUpdated:
    //         Use updateOS in the new version software set to activate the old version
    //         Use updateOS in the new version software set to invalidate the new version
    // OS software set directory
    AraString const kOSSwclDir{GetPath().GetSwclInstallDir(kOS_SWCL_NAME)};
    LOGD << "got OSSwclDir:" << kOSSwclDir.c_str();
    if (!kOSSwclDir.empty()) {
        ret = HelpExecutor::OSUpdateOperate(HelpExecutor::OSUpdateType::kOSExceptionRollback);
        if (!ret.HasValue()) {
            LOGE << "kOSExceptionRollback failed, todo...";
        }
    }

    return {};
}

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara
