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
/// @file       help_executor.cpp
/// @brief      HelpExecutor implementation
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
/// @unit_name=HelpExecutor
/// @unit_description=HelpExecutor implementation
/// @endcode
///
/// ================================================================

#include "help_executor.h"

#include "ara/ucm/internal/extraction/tinyfs.h"
#include "ara/ucm/internal/extraction/tinyfsys.h"
#include "common/assert.h"
#include "common/errc.h"
#include "common/log.h"
#include "common/path.h"
#include "em/em_client.h"
#include "fsm/fsm_manager.h"
#include "fsm/ucm_status_storage.h"
#include "parsing/software_cluster.h"
#include "storage/os_update_operator.h"
#include "util/future_ext.h"

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @brief RebootOrReparse
/// @param type
/// @return result
/// @throws no
AraResultVoid HelpExecutor::RebootOrReparse(HelpExecutor::SwclUpdateType const type) const noexcept
{
    LOGD << "call...";

    UCMErrorDomainErrc errc{UCMErrorDomainErrc::kVerificationFailed};
    bool success{true};

    switch (type) {
        case HelpExecutor::SwclUpdateType::kSwclActivate: {
            break;
        }
        case HelpExecutor::SwclUpdateType::kSwclVerify: {
            errc = UCMErrorDomainErrc::kVerificationFailed;
            break;
        }
        case HelpExecutor::SwclUpdateType::kSwclRollback: {
            errc = UCMErrorDomainErrc::kNotAbleToRollback;
            break;
        }
        default: {
            break;
        }
    }

    // 4. ResetMachine()/ReparseManifests();
    ActivateOptionType const whatToDo{owner_->swclManager.GetStrongestActivationOption()};
    LOGD << "got restart option whatToDo:" << strtype::ActivateOptionTypeToStr(whatToDo).c_str();

    switch (whatToDo) {
        case ActivateOptionType::kReboot:  // serialize all actions and system restart
            // 4.1 If a restart is involved
            //     4.1.1 Generate ucm_status.json and its corresponding ucm_status.md5, so that it can enter the Verifying state to continue execution after restart
            //     4.1.2 Notify SM: restart the OS and wait for the restart
            //     4.1.3 After the restart is complete, enter the Verifying state to continue execution;
            //     --- After state recovery, ucm_status.md5 will be deleted, making ucm_status.json invalid.
            {
                // 4.1.1 Generate ucm_status.json and its corresponding ucm_status.md5, so that it can enter the Verifying state to continue execution after restart
                // Commit the transaction
                LOGD << "call CommitSaveTransaction";
                UcmStatusStorage::CommitSaveTransaction();

                // ResetMachine() during Verifying should keep retrying on failure, otherwise even rollback would require calling ResetMachine()? No
                // Notify SM: ResetMachine()
                LOGD << "call ResetMachine";
                AraResultVoid const ret{owner_->pSM->ResetMachine().GetResult()};
                if (ret.HasValue()) {
                    // TODO: After future changes to the underlying library, remove this setting
                    // Tell the client a restart is in progress? The client can also get the current status via CurrentStatus
                    LOGE
                        << "ResetMachine succeeded";  ////As long as it restarts, this error will be returned. So based on this error, we return and do not execute further.////
                    errc = UCMErrorDomainErrc::kResettingMachine;
                } else {
                    LOGE << "ResetMachine failed, errc:" << ret.Error().Message().data();
                }
                success = false;

                break;
            }
        case ActivateOptionType::kRestartApplication:  // SWCL restart: UCM continues
            // 4.2 If a restart is not involved   ---The application software set list must change
            //     Call EM's GetUsedAppSWCLListVersion to get the UsedAppSWCLList;
            //     Does UsedAppSWCLList match application_swcl_list_ver(Y+1).json? If yes, continue; otherwise, switch the UCM state machine to RollingBack;.
            {
                AraList< SwClusterInfoType > disableSwcls;
                AraList< SwClusterInfoType > enableSwcls;
                _getEnableSwcls(disableSwcls, enableSwcls, type);

                // Execution management could now be notified to use the updated processes list to start newly installed
                // processes and/or restart updated ones.
                LOGD << "ask EM to ReparseManifests";
                success = ExecClient::ReparseProcessList(disableSwcls, enableSwcls);
                if (!success) {  // If it fails, also set the ucm_status status to RollingBack_Prepare;     --- Skip the Verifying state here
                    LOGE << "ReparseProcessList failed";
                }

                break;
            }
        case ActivateOptionType::kWaitForReboot:  // Either we recover from restart, or there was nothing to do.
        {
            LOGD << "nothing to do";
            break;
        }
        default: {
            break;
        }
    }

    if (!success) {
        return AraResultVoid::FromError(UCMErrorDomainErrc{errc});
    }

    return {};
}

/// @brief OSUpdateOperate
/// @param type
/// @return result
/// @throws no
AraResultVoid HelpExecutor::OSUpdateOperate(HelpExecutor::OSUpdateType const type) noexcept
{
    LOGD << "OSUpdateType:" << strtype::HelpExecutorOSUpdateTypeToStr(type).c_str();

    SwClusterStateType osSwclState;
    AraString curOSVersion;
    AraString newOSVersion;
    std::ignore = FileSystemSWCLManager::GetSwclStatus(kOS_SWCL_NAME, osSwclState, curOSVersion, newOSVersion);

    // Target directory for OS software set update
    AraString const kOSUpdateDestDir{GetPath().GetSwclInstallDir(kOS_SWCL_NAME, newOSVersion)};
    OSUpdateOperator const kOSOperator{kOSUpdateDestDir};
    LOGD << "got OSUpdateDestDir:" << kOSUpdateDestDir.c_str();

    AraResultVoid ret;
    switch (type) {
        case HelpExecutor::OSUpdateType::kOSActivate: {
            ret = kOSOperator.ActiveOSPatition(newOSVersion);
            break;
        }
        case HelpExecutor::OSUpdateType::kOSVerify: {
            ret = kOSOperator.VerifyOSPatition(newOSVersion);
            break;
        }
        case HelpExecutor::OSUpdateType::kOSRollback: {
            ret = kOSOperator.ActiveOSPatition(curOSVersion);
            break;
        }
        case HelpExecutor::OSUpdateType::kOSExceptionRollback: {
            if (osSwclState == SwClusterStateType::kUpdated) {
                ret = kOSOperator.ActiveOSPatition(curOSVersion);
                if (ret) {
                    ret = kOSOperator.InvalidOSPatition(newOSVersion);
                }
            }
            break;
        }
        default: {
            break;
        }
    }

    return ret;
}

/// @brief SwclFGRemoveMachineFG
/// @param fgList
/// @throws no
void HelpExecutor::SwclFGRemoveMachineFG(AraVectorString& fgList) noexcept
{
    AraVectorString tmp;
    for (AraString const& it : fgList) {
        if (strutil::GetLastComponent(it)
            != kMachineFunctionGroupName) {  // Get the last part of the function group name
            tmp.push_back(it);
        }
    }
    fgList = tmp;
}

/// @brief DependencyLessThan
/// @param swcl1
/// @param swcl2
/// @return bool
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10389
/// @trace_id_dd=DD_UCM_10913
/// @needwork = dda
/// @endcode
inline static bool DependencyLessThan(SoftwareCluster const* const swcl1, SoftwareCluster const* const swcl2)
{
    SoftwareClusterDependencyFormula formula1{swcl1->GetSwclManifest().dependsOnFormula};
    return (formula1.DependedOn(swcl2->GetSwclManifest().fQN));
}
/// @brief DependencyBiggerThan
/// @param swcl1
/// @param swcl2
/// @return bool
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10389
/// @trace_id_dd=DD_UCM_10913
/// @needwork = dda
/// @endcode
inline static bool DependencyBiggerThan(SoftwareCluster const* const swcl1, SoftwareCluster const* const swcl2)
{
    SoftwareClusterDependencyFormula formula2{swcl2->GetSwclManifest().dependsOnFormula};
    return formula2.DependedOn(swcl1->GetSwclManifest().fQN);
}

/// @brief SwclFGChange
/// @param type
/// @return result
/// @throws no
AraResultVoid HelpExecutor::SwclFGChange(HelpExecutor::SwclUpdateType const type) const noexcept
{
    LOGD << "call... SwclUpdateType:" << strtype::HelpExecutorSwclUpdateTypeToStr(type).c_str();

    UCMErrorDomainErrc errc;
    bool success{true};

    // UCM shall compute the order of the State Management UpdateRequest Service Interface PrepareUpdate, VerifyUpdate
    // and PrepareRollback method calls from the dependency model included in the Software Cluster manifests.
    LOGD << "get swcls";
    AraList< SoftwareCluster > swcls;
    switch (type) {
        case HelpExecutor::SwclUpdateType::kSwclActivate: {
            swcls = FileSystemSWCLManager::GetSWCLsToPrepareUpdate();
            break;
        }
        case HelpExecutor::SwclUpdateType::kSwclVerify:
        case HelpExecutor::SwclUpdateType::kSwclRollback: {
            swcls = FileSystemSWCLManager::GetSWCLsToActivate();
            break;
        }
        default: {
            LOGE << "invalid SwclUpdateType";  // never go to here
            break;
        }
    }

    LOGD << "sort swcls";
    AraList< SoftwareCluster* > swclsSorted;
    for (SoftwareCluster& it : swcls) {
        swclsSorted.push_back(&it);
    }

    // AraVector<SoftwareCluster*>::iterator ssbegin;.
    // AraVector<SoftwareCluster*>::iterator ssend;.
    // ssbegin = swclsSorted.begin();.
    // ssend = swclsSorted.end();.

    switch (type) {
        case HelpExecutor::SwclUpdateType::kSwclActivate:
        case HelpExecutor::SwclUpdateType::kSwclRollback: {
            // std::sort(ssbegin, ssend, &DependencyLessThan);.
            swclsSorted.sort(&DependencyLessThan);
            break;
        }
        case HelpExecutor::SwclUpdateType::kSwclVerify: {
            // std::sort(ssbegin, ssend, &DependencyBiggerThan);.
            swclsSorted.sort(&DependencyBiggerThan);
            break;
        }
        default: {
            break;
        }
    }

    // As following step Update and Config Management uses the VerifyUpdate to request State Management to perform a
    // verification of the update. Therefore State Management will at least set all the Function Groups, given as
    // parameter, to Verify state. loop for each SoftwareCluster  VerifyUpdate(vector<FunctionGroup>)
    for (SoftwareCluster* const swclPtr : swclsSorted) {
        SoftwareCluster const& swcl{*swclPtr};

        // Call PrepareUpdate, VerifyUpdate, and PrepareRollBack respectively based on actionType being remove, update, install
        //      When a Software Cluster is removed Update and Config Management the VerifyUpdate and PrepareRollback
        //      will never be called by Update and Config Management. Contrary to that the PrepareUpdate will never be
        //      called, when a new Software Cluster is installed into the Machine.

        LOGD << "call _swclFGMatch for swcl:" << swcl.GetSwclManifest().shortName.c_str()
             << " Version:" << swcl.GetSwclManifest().version.ToString().c_str();

        if (_swclFGMatch(swcl, type)) {
            LOGD << "get fgList and call SwclFGRemoveMachineFG";
            AraVectorString fgList{swcl.GetSwclManifest().claimedFunctionGroups};

            // Do not call PrepareUpdate or PrepareRollback for MachineFG
            // State Management should be configured to run in every Machine State (this includes Startup, Shutdown and
            // Restart) other than Off.
            if ((HelpExecutor::SwclUpdateType::kSwclActivate == type)
                || (HelpExecutor::SwclUpdateType::kSwclRollback == type)) {
                SwclFGRemoveMachineFG(fgList);
            }

            // The state kRolling-Back shall be set if any of the State Management UpdateRequest Service Interface
            // VerifyUpdate method calls returns the result kVerifyFailed If any one of the State Management
            // UpdateRequest Service Interface VerifyUpdate returns error kRejected too many times or for too long
            // (implementation specific thresholds), UCM shall transition to kRolling-Back state. Waiting deadline
            LOGD << "call pSM";
            switch (type) {
                case HelpExecutor::SwclUpdateType::kSwclActivate: {
                    if (!owner_->pSM->RetryPrepareUpdate(fgList)) {
                        // TODO:
                        // If calling StopUpdateSession() fails, how to know and handle??? Its FireAndForget is true, so it cannot have a return value???
                        owner_->pSM->StopUpdateSession();
                        LOGE << "PrepareUpdate failed, and called StopUpdateSession";
                        errc    = UCMErrorDomainErrc::kPreActivationFailed;
                        success = false;
                    }
                    break;
                }
                case HelpExecutor::SwclUpdateType::kSwclVerify: {
                    if (!owner_->pSM->RetryVerifyUpdate(fgList)) {
                        LOGE << "VerifyUpdate failed";
                        errc    = UCMErrorDomainErrc::kVerificationFailed;
                        success = false;
                    }
                    break;
                }
                case HelpExecutor::SwclUpdateType::kSwclRollback: {
                    if (!owner_->pSM->RetryPrepareRollback(fgList)) {
                        LOGE << "PrepareRollback failed";
                        errc    = UCMErrorDomainErrc::kNotAbleToRollback;
                        success = false;
                    }
                    break;
                }
                default: {
                    break;
                }
            }
        }

        if (!success) {
            break;
        }
    }

    if (!success) {
        return AraResultVoid::FromError(UCMErrorDomainErrc(errc));
    }

    LOGD << "end with success";
    return {};
}

/// @brief GetAllFG
/// @return vector of string
/// @throws no
AraVectorString HelpExecutor::GetAllFG() noexcept
{
    AraVectorString out{};
    std::int32_t ret{0};

    // Obtain function group information from each software set
    // Get all software set directories
    AraVectorString allSwclDirs = FileSystemSWCLManager::GetAllSwclDirs();
    for (auto const& stSwclDir : allSwclDirs) {
        LOGD << "get SwclDir:" << stSwclDir;
        AraVectorString swclVersionDirs{tinyfsys::GetSubdirectories(stSwclDir)};

        for (auto const& swclVersionDir : swclVersionDirs) {
            LOGD << "get a swclVersionDir:" << swclVersionDir;
            AraString swclFGsFilePath{swclVersionDir + kPathSeparator + kETC_DIR_NAME + kPathSeparator
                                      + kFunctionGroupsFileName};

            // Function group file path
            LOGD << "get a swclFGsFilePath:" << swclFGsFilePath;
            if (tinyfsys::DoesFileExist(swclFGsFilePath)) {
                // get all FunctionGroups from MachineManifest
                RManifestOpenAndCheck(doc, swclFGsFilePath, out);

                ret = doc->IterateArray(std::move(AraStringView(kFunctionGroups)),
                                        [&out](std::size_t, RManifestNode const& node) {
                                            AraString stFGName{RManifestNodeGetSS(node, "name")};
                                            LOGD << "get a FGName:" << stFGName;
                                            out.push_back(stFGName);
                                        });
                RManifestLoadResultStrictCheck(kFunctionGroups, ret, out);
            }
        }
    }
    return out;
}

/// @brief _getEnableSwcls
/// @param disableSwcls
/// @param enableSwcls
/// @param type
/// @throws no
void HelpExecutor::_getEnableSwcls(AraList< SwClusterInfoType >& disableSwcls,
                                   AraList< SwClusterInfoType >& enableSwcls,
                                   HelpExecutor::SwclUpdateType const type) const noexcept
{
    // if verify, use curSWCLVersion as disable version, use swclVersion_ as enable version
    // if rollback, use curSWCLVersion as enable version, use swclVersion_ as disable version
    AraList< ActionExecutionInfoType > const actions{owner_->swclManager.GetActionExecutionInfo()};

    for (ActionExecutionInfoType const& it : actions) {
        switch (it.actionType) {
            case ActionType::kInstall: {
                if (HelpExecutor::SwclUpdateType::kSwclRollback == type) {
                    disableSwcls.emplace_back(
                        SwClusterInfoType{it.swclName, it.swclVersion, SwClusterStateType::kAdded});
                } else {  // enable added swcl when activate/verify
                    enableSwcls.emplace_back(
                        SwClusterInfoType{it.swclName, it.swclVersion, SwClusterStateType::kAdded});
                }
                break;
            }
            case ActionType::kRemove: {
                if (HelpExecutor::SwclUpdateType::kSwclRollback == type) {
                    enableSwcls.emplace_back(
                        SwClusterInfoType{it.swclName, it.curSWCLVersion, SwClusterStateType::kRemoved});
                } else {  // disable removed swcl when activate/verify
                    disableSwcls.emplace_back(
                        SwClusterInfoType{it.swclName, it.curSWCLVersion, SwClusterStateType::kRemoved});
                }
                break;
            }
            case ActionType::kUpdate: {
                if (HelpExecutor::SwclUpdateType::kSwclRollback == type) {
                    enableSwcls.emplace_back(
                        SwClusterInfoType{it.swclName, it.curSWCLVersion, SwClusterStateType::kUpdated});
                    disableSwcls.emplace_back(
                        SwClusterInfoType{it.swclName, it.swclVersion, SwClusterStateType::kUpdated});
                } else {  // disable curSWCLVersion and enable swclVersion_ for updated swcl when activate/verify
                    disableSwcls.emplace_back(
                        SwClusterInfoType{it.swclName, it.curSWCLVersion, SwClusterStateType::kUpdated});
                    enableSwcls.emplace_back(
                        SwClusterInfoType{it.swclName, it.swclVersion, SwClusterStateType::kUpdated});
                }
                break;
            }
            default: {
                AssertWithLog(false);  //never goto here
                // break;.
            }
        }
    }
}

/// @brief _swclFGMatch
/// @param swcl
/// @param type
/// @return bool
/// @throws no
bool HelpExecutor::_swclFGMatch(SoftwareCluster const& swcl, HelpExecutor::SwclUpdateType const type) const noexcept
{
    ActivateOptionType const activationOption{owner_->swclManager.GetActivationOption4Swcl(swcl)};
    bool const optionMatchRet{ActivateOptionType::kWaitForReboot != activationOption};
    if (!optionMatchRet) {
        LOGD << "swcl ShortName:" << swcl.GetSwclManifest().shortName.c_str()
             << " Version:" << swcl.GetSwclManifest().version.ToString().c_str()
             << " activationOption is kWaitForReboot, matchRet is false";
        return false;
    }

    SwClusterStateType const swclState{swcl.GetSwclState()};
    bool stateMatchRet{(SwClusterStateType::kAdded == swclState) || (SwClusterStateType::kUpdated == swclState)};
    if (HelpExecutor::SwclUpdateType::kSwclActivate == type) {
        stateMatchRet = stateMatchRet || (SwClusterStateType::kRemoved == swclState);
    }

    bool const matchRet{optionMatchRet && stateMatchRet};

    LOGD << "swcl, ShortName:" << swcl.GetSwclManifest().shortName.c_str()
         << " Version:" << swcl.GetSwclManifest().version.ToString().c_str()
         << " SwclState:" << FileSystemSWCLManager::SwclStateToStr(swclState).c_str()
         << " activationOption:" << strtype::ActivateOptionTypeToStr(activationOption).c_str()
         << " matchRet:" << matchRet;
    return matchRet;
}

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara
