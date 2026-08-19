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
/// @file       verify_executor.cpp
/// @brief      VerifyExecutor implementation
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
/// @unit_name=VerifyExecutor
/// @unit_description=VerifyExecutor implementation
/// @endcode
///
/// ================================================================

#include "verify_executor.h"

#include "common/assert.h"
#include "common/errc.h"
#include "common/log.h"
#include "fsm/fsm_manager.h"
#include "util/future_ext.h"

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @brief Perform the verification
/// @return result
AraResultVoid VerifyExecutor::RunInLocal() const noexcept
{
    AraResultVoid ret{_verify()};
    if (!ret.HasValue()) {
        if (UCMErrorDomainErrc::kResettingMachine == static_cast< UCMErrorDomainErrc >(ret.Error().Value())) {
            LOGE << "Verification succeeded, when ResetMachine succeeded";
        } else {
            LOGE << "Verification failed, call OnFailure";
            std::ignore = owner_->OnFailureWithoutLock();
        }

        return ret;
    }

    LOGD << "Verification succeeded, call OnSuccess";
    return owner_->OnSuccessWithoutLock();
}

/// @brief do something after restart
/// @throws no
/// @return result
void VerifyExecutor::RunInThreadAfterReboot() noexcept
{
    LOGD << "call...";
    std::ignore = ASyncCallVoid([this]() noexcept {
        AraResultVoid res{this->_verifyAfterRestart()};
        if (!res.HasValue()) {
            LOGE << "_verifyAfterRestart failed, call OnFailure";
            std::ignore = owner_->OnFailureWithoutLock();
        } else {
            LOGD << "_verifyAfterRestart succeeded, call OnSuccess";
            std::ignore = owner_->OnSuccessWithoutLock();
        }
        return res;
    });
}

/// @brief _verify
/// @throws no
/// @return result
AraResultVoid VerifyExecutor::_verify() const noexcept
{
    LOGD << "call...";
    AraResultVoid ret;
    ret = _verifyBeforeRestart();
    if (!ret.HasValue()) {
        return ret;
    }
    ret = _verifyOnRestart();
    if (!ret.HasValue()) {
        return ret;
    }
    ret = _verifyAfterRestart();
    return ret;
}

/// @brief Verify Before Restart
/// @throws no
/// @return result
AraResultVoid VerifyExecutor::_verifyBeforeRestart() const noexcept
{
    LOGD << "call...";

    /// Symlinks or A/B switch()
    // 1 Determine the application software set list
    bool const platformCoreUpdated{
        FileSystemSWCLManager::IsPlatformCoreUpdated()};  // Update involves platform software set
    bool const nonPlatformCoreUpdated{
        FileSystemSWCLManager::IsNonPlatformCoreUpdated()};       // Update involves non-platform software sets
    bool const kOSUpdated{FileSystemSWCLManager::IsOSUpdated()};  // Update involves OS software set
    LOGD << "platformCoreUpdated:" << platformCoreUpdated << " nonPlatformCoreUpdated:" << nonPlatformCoreUpdated
         << " OSUpdated:" << kOSUpdated;

    // All software sets to be activated
    AraList< SoftwareCluster > const activationSwcls{FileSystemSWCLManager::GetSWCLsForActivation()};

    // There must be at least one software set updated (platform software set or normal software set)
    AssertWithLog(platformCoreUpdated || nonPlatformCoreUpdated);
    owner_->mABPartition.SwclListFileLinkToB(activationSwcls, nonPlatformCoreUpdated);

    // 3 If the update involves platform_core, clean and create a new boot_option_ver(X+1).cfg (content is empty);
    //         Use the newVersion of platform_core as core_version.
    //         Clean and generate boot_option_ver(X+1).md5 based on boot_option_ver(X+1).cfg;
    if (platformCoreUpdated) {
        owner_->mABPartition.BootOptionFileLinkToB(activationSwcls);
    }

    // 5 If the update involves OS, call updateOS active new_version to activate the new version of the OS;
    if (kOSUpdated) {
        AraResultVoid const ret{HelpExecutor::OSUpdateOperate(HelpExecutor::OSUpdateType::kOSActivate)};
        AssertWithLog(ret.HasValue());  /////////Should handle the return result//mytodo////
    }

    return {};
}

/// @brief Verify On Restart
/// @throws no
/// @return result
AraResultVoid VerifyExecutor::_verifyOnRestart() const noexcept
{
    LOGD << "call...";
    // 4. ResetMachine()/ReparseManifests();
    return owner_->mHelpExecutor.RebootOrReparse(HelpExecutor::SwclUpdateType::kSwclVerify);
}

/// @brief Verify After Restart
/// @throws no
/// @return result
AraResultVoid VerifyExecutor::_verifyAfterRestart() const noexcept
{
    LOGD << "call...";
    AraResultVoid ret;

    // test
    ///std::this_thread::sleep_for(std::chrono::seconds(60));.
    // 4.1 If a restart is involved
    //     4.1.4 Determine if the current version is the new version
    //         4.1.4.1 If the update involves platform_core
    //                 Determine if platform_core is the new version based on the current process path: if it is the new version, continue; otherwise, it indicates a switch failure, go to 4.1.4.5
    //         4.1.4.2 Call EM's GetUsedAppSWCLListVersion to get the UsedAppSWCLList;
    //         --- If only using something like CurrentAppSWCLListCorrect, then: in cases like updating only a non-platform software set but run_time_application_swcl_list.cfg hasn't been updated yet, it is still necessary to parse run_time_application_swcl_list.cfg
    //         4.1.4.3 Does the update involve non-platform software sets?
    //                     If involved
    //                     Does UsedAppSWCLList match application_swcl_list_ver(Y+1).json? If yes, continue; otherwise, go to 4.1.4.5
    //                     If not involved
    //                     Does UsedAppSWCLList match application_swcl_list_verY.json? If yes, continue; otherwise, go to 4.1.4.5
    //         4.1.4.4
    //         If the upgrade of the persistent database is involved, call the persistent database's upgrade interface UpdatePersistency() to convert the information stored in the persistent database to the new version;
    //                     //Could consider:
    //                     //  Transfer is only possible in IDLE, PROCESSING, READY states; once Activate is called and enters the Activating state, transfer should be paused
    //                     //  During program startup recovery to the Verifying/any state, transfer cannot occur
    //                 Execute 5
    //         4.1.4.5 Switch the UCM state machine to RollingBack;
    ActivateOptionType const whatToDo{owner_->swclManager.GetStrongestActivationOption()};
    LOGI << "got restart option whatToDo:" << strtype::ActivateOptionTypeToStr(whatToDo).c_str();

    if (ActivateOptionType::kReboot == whatToDo) {
        bool const platformCoreUpdated{
            FileSystemSWCLManager::IsPlatformCoreUpdated()};  // Update involves platform software set
        bool const nonPlatformCoreUpdated{
            FileSystemSWCLManager::IsNonPlatformCoreUpdated()};  // Update involves non-platform software sets
        LOGD << "platformCoreUpdated:" << platformCoreUpdated << " nonPlatformCoreUpdated:" << nonPlatformCoreUpdated;
        AssertWithLog(platformCoreUpdated || nonPlatformCoreUpdated);

        if (platformCoreUpdated) {  // If the update involves platform_core
            // Determine if platform_core is the new version based on the current process path: if it is the new version, continue; otherwise, it indicates a switch failure, go to 4.1.4.5
            if (!ABPartition::CheckCoreVersionAfterReboot()) {
                ReturnVoidErrcEnumWithLongLog("CheckCoreVersionAfterReboot failed", kVerificationFailed);
            }
        }

        // Does the update involve non-platform software sets?
        if (nonPlatformCoreUpdated) {  // Involved
            // Does UsedAppSWCLListVersion match application_swcl_list_ver(Y+1).json? If yes, continue; otherwise, go to 4.1.4.5
            if (!owner_->mABPartition.CheckSwclListVersionAfterReboot()) {
                ReturnVoidErrcEnumWithLongLog("CheckSwclListVersionAfterReboot failed", kVerificationFailed);
            }
        } else {  // Not involved
            /// // Does UsedAppSWCLListVersion match application_swcl_list_verY.json? If yes, continue; otherwise, go to 4.1.4.5
            ///if (!owner_->mABPartition.CheckSwclListVersionWithoutUpdate()) {
            ///    LOGE << "CheckSwclListVersionWithoutUpdate failed, Errc::kVerificationFailed";
            ///    return AraResultVoid::FromError(UCMErrorDomainErrc::kVerificationFailed);
            ///}

            /// It could be that the application list becomes unusable due to a platform upgrade. When there is only a platform update, the update should not fail because the application list cannot start. Therefore, change this part to: continue.
        }
    }

    bool const kOSUpdated{FileSystemSWCLManager::IsOSUpdated()};  // Update involves OS software set
    LOGD << "OSUpdated:" << kOSUpdated;

    // If the update involves OS, find the target directory of the OS software set, then call updateOS verify new_version
    if (kOSUpdated) {
        ret = HelpExecutor::OSUpdateOperate(HelpExecutor::OSUpdateType::kOSVerify);
        if (!ret.HasValue()) {
            ReturnVoidErrcEnumWithLongLog("VerifyOSPatition failed", kVerificationFailed);
        }
    }

    // 5 VerifyUpdate(vector<FunctionGroup>)
    // 6 Switch the UCM state machine to RollingBack/Activated;

    // UCM shall compute the order of the State Management UpdateRequest Service Interface PrepareUpdate, VerifyUpdate
    // and PrepareRollback method calls from the dependency model included in the Software Cluster manifests.
    LOGD << "call SwclFGChange";
    ret = owner_->mHelpExecutor.SwclFGChange(HelpExecutor::SwclUpdateType::kSwclVerify);
    if (!ret.HasValue()) {
        ReturnVoidErrcWithLongLog("SwclFGChange failed", GetAraResultErrc(ret));
    }

    LOGD << "end with success";
    return ret;
}

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara
