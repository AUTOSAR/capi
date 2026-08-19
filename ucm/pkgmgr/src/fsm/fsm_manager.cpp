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
/// @file       fsm_manager.cpp
/// @brief      FSM Manager implementation
/// @details
/// @date       2024-08-15
/// @author     cuiyinli
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/UCM/FsmManager
/// @interface_level=module
/// @trace_id_sr=SR_UCM_00001
/// @unit_name=FsmManager
/// @unit_description=FSM Manager implementation
/// @endcode
///
/// ================================================================

#include "fsm_manager.h"

#include <type_traits>

#include "activated_state.h"
#include "activating_state.h"
#include "cleaningup_state.h"
#include "common/assert.h"
#include "common/const.h"
#include "common/log.h"
#include "common/path.h"
#include "common/rjson_manifest.h"
#include "common/strtype.h"
#include "data/software_package_manager.h"
#include "exception_cleaningup_state.h"
#include "exception_rollingback_state.h"
#include "idle_state.h"
#include "processing_state.h"
#include "ready_state.h"
#include "rolledback_state.h"
#include "rollingback_state.h"
#include "storage/filesystem_swcl_manager.h"
#include "ucm_status_storage.h"
#include "verifying_state.h"

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @brief PackageManagerState Accessor
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
using PMStateAccessor = LockedPtrWrapper< PackageManagerState >::Accessor;

/// @brief fsm manager
/// @param updateServiceInterfaceField
/// @param swpkgManagerA
/// @param swclManagerA
/// @param smService
FsmManager::FsmManager(std::function< void(std::underlying_type< PackageManagerStatusType >::type const& newStatus) >
                           updateServiceInterfaceField,
                       SoftwarePackageManager& swpkgManagerA,
                       FileSystemSWCLManager& swclManagerA,
                       std::unique_ptr< UpdateRequestService > smService) noexcept
    : updateServiceInterfaceField_{std::move(updateServiceInterfaceField)}
    , swpkgManager{swpkgManagerA}
    , swclManager{swclManagerA}
    , pSM{std::move(smService)}
{
    mHelpExecutor.SetOwner(this);
    mProcessExecutor.SetOwner(this);
    mActivateExecutor.SetOwner(this);
    mVerifyExecutor.SetOwner(this);
    mCleanupExecutor.SetOwner(this);
    mRollbackExecutor.SetOwner(this);
    mExceptionRollbackExecutor.SetOwner(this);
    mExceptionCleanupExecutor.SetOwner(this);

    _initState();
}

/// @brief Activate the processed components.
/// @throws no
/// @return future with result
AraFutureVoid FsmManager::Activate() const noexcept
{
    LOGD << "call...";
    PMStateAccessor const accessor{state_->Get()};
    return accessor->Activate();
}

/// @brief Finish the processing for the current set of
/// processed Software Packages.
/// @throws no
/// @return future with result
AraFutureVoid FsmManager::Finish() const noexcept
{
    LOGD << "call...";
    PMStateAccessor const accessor{state_->Get()};
    return accessor->Finish();
}

/// @brief Process transferred Software Package.
/// @param id TransferId
/// @throws no
/// @return future with result
AraFutureVoid FsmManager::ProcessSwPackage(TransferIdType const& id) const noexcept
{
    LOGD << "call..." << strtype::TransferIdTypeToHexStr(id).c_str();
    PMStateAccessor const accessor{state_->Get()};
    return accessor->ProcessSwPackage(id);
}

/// @brief Abort an ongoing processing of a Software Package.
/// @param id TransferId
/// @throws no
/// @return future with result
AraFutureVoid FsmManager::Cancel(TransferIdType const& id) const noexcept
{
    LOGD << "call..." << strtype::TransferIdTypeToHexStr(id).c_str();
    PMStateAccessor const accessor{state_->Get()};
    return accessor->Cancel(id);
}

/// @brief Revert the changes done by processing (ProcessSwPackage) of one
/// or several software packages.
/// @throws no
/// @return future with result
AraFutureVoid FsmManager::RevertProcessedSwPackages() const noexcept
{
    LOGD << "call...";
    PMStateAccessor const accessor{state_->Get()};
    return accessor->RevertProcessedSwPackages();
}

/// @brief Rollback the system to the state
/// before the packages were processed.
/// @throws no
/// @return future with result
AraFutureVoid FsmManager::Rollback() const noexcept
{
    LOGD << "call...";
    PMStateAccessor const accessor{state_->Get()};
    return accessor->Rollback();
}

/// @brief Verify all the software clusters were updated.
/// @throws no
/// @return future with result
void FsmManager::Verify() const noexcept
{
    LOGD << "call...";
    PMStateAccessor const accessor{state_->Get()};
    std::ignore = accessor;
}

// internal messages
/// @brief Done
/// @throws no
void FsmManager::Done() const noexcept { state_->Get()->Done(); }
/// @brief DoneWithoutLock
/// @throws no
void FsmManager::DoneWithoutLock() const noexcept { state_->GetWithoutLock()->Done(); }
/// @brief OnProcessSwPackageSucceeded
/// @throws no
void FsmManager::OnProcessSwPackageSucceeded() const noexcept { state_->Get()->OnProcessSwPackageSucceeded(); }
/// @brief OnProcessSwPackageFailed
/// @throws no
void FsmManager::OnProcessSwPackageFailed() const noexcept { state_->Get()->OnProcessSwPackageFailed(); }
/// @brief OnSuccess
/// @throws no
/// @return result
AraResultVoid FsmManager::OnSuccess() const noexcept { return state_->Get()->OnSuccess(); }
/// @brief OnSuccessWithoutLock
/// @throws no
/// @return result
AraResultVoid FsmManager::OnSuccessWithoutLock() const noexcept { return state_->GetWithoutLock()->OnSuccess(); }
/// @brief OnFailure
/// @throws no
/// @return result
AraResultVoid FsmManager::OnFailure() const noexcept { return state_->Get()->OnFailure(); }
/// @brief OnFailureWithoutLock
/// @throws no
/// @return result
AraResultVoid FsmManager::OnFailureWithoutLock() const noexcept { return state_->GetWithoutLock()->OnFailure(); }

/// @brief ContinueIfNecessary
/// @throws no
void FsmManager::ContinueIfNecessary() noexcept
{
    LOGD << "call...";

    PackageManagerStatusType const status{state_->Get()->GetStatus()};  // release the lock ASAP
    switch (status) {
        case PackageManagerStatusType::kVerifying: {
            // continue the activation+verification cycle of the FSM
            // TODO: recover the actions from persistence here
            mVerifyExecutor.RunInThreadAfterReboot();
            break;
        }
        case PackageManagerStatusType::kRollingBack: {
            Done();
            break;
        }
        case PackageManagerStatusType::kCleaningUp: {
            std::ignore = mCleanupExecutor.RunInThread();
            break;
        }
        case PackageManagerStatusType::kExceptionRollingBack: {
            std::ignore = mExceptionRollbackExecutor.RunInThread();
            break;
        }
        case PackageManagerStatusType::kExceptionCleaningUp: {
            std::ignore = mExceptionCleanupExecutor.RunInThread();
            break;
        }
        default: {
            LOGD << "nothing to do";
            break;
        }
    }

    LOGD << "end with status:" << strtype::PackageManagerStatusTypeToStr(status).c_str();
}

/// @brief Switch
/// @param status PackageManagerStatusType
/// @param cleanupAction FinalActionType
/// @throws no
void FsmManager::Switch(PackageManagerStatusType const status, FinalActionType const cleanupAction) const noexcept
{
    state_->Get().Reset(CreateState(status, cleanupAction));
}

/// @brief SwitchWithoutLock
/// @param status PackageManagerStatusType
/// @param cleanupAction FinalActionType
/// @throws no
void FsmManager::SwitchWithoutLock(PackageManagerStatusType const status,
                                   FinalActionType const cleanupAction) const noexcept
{
    state_->GetWithoutLock().Reset(CreateState(status, cleanupAction));
}

/// @brief GetFinalActionType
/// @throws no
/// @return FinalActionType
FinalActionType FsmManager::GetFinalActionType() const noexcept { return state_->Get()->GetFinalActionType(); }

/// @brief _initState
/// @throws no
void FsmManager::_initState() noexcept
{
    state_ = std::make_unique< LockedPtrWrapper< PackageManagerState > >(
        _recoverState(), [this](PackageManagerState const& st) {
            _onSwitchNewState(st);
            updateServiceInterfaceField_(
                static_cast< std::underlying_type< PackageManagerStatusType >::type >(st.GetStatus()));
        });

    // "Recovering to VERIFYING will invalidate ucm_status.json"
    if (PackageManagerStatusType::kVerifying == state_->Get()->GetStatus()) {
        UcmStatusStorage::BeginSaveTransaction();  // Note: do not commit the transaction
    }
}

/// @brief _recoverState
/// @throws no
/// @return PackageManagerState
std::unique_ptr< PackageManagerState > FsmManager::_recoverState() noexcept
{
    LOGD << "call...";

    std::unique_ptr< PackageManagerState > stateUPtr{nullptr};

    // Search the araDir_ directory for all boot_option_verX.cfg files, use the smallest X as lastUsedBootOptionFileID_
    // Search the appLayerSWCLDir directory for the IDs of all application_swcl_list_verY.json files, use the smallest Y as lastUsedSWCLListID_
    mABPartition.RecoverLastUsedId();

    // Recover data to packagesData_
    swpkgManager.RecoverPackagesData();

    // 1. Find a valid file named ucm_status.json under /ara/var/ucm/, i.e., satisfy;
    //        The MD5 calculated from the content of ucm_status.json matches the content of ucm_status.md5
    // 2. If no valid ucm_status.json is found in step 1, enter the rollback cleanup state: ROLLING-BACK-AND-CLEANING_UP, otherwise continue
    switch (UcmStatusStorage::CheckConsistency()) {
        case UcmStatusStorage::CheckResultType::
            kUCMStatusAllFileEmpty: {  // ucm_status.json and ucm_status.md5 not found, regenerate
            LOGD << "both ucmStatusFilePath and ucmStatusMD5FilePath don't exist,"
                 << " so we will generate them with IdleState";
            UcmStatusStorage::SaveCurrentStatus(IdleState(),
                                                swclManager.GetActionExecutionInfo());  ///////////////mytodo666//
            UcmStatusStorage::CommitSaveTransaction();
        } break;  /////////////////////do not return//////Compare with old code///////mytodo666//
        case UcmStatusStorage::CheckResultType::kUCMStatusFileEmpty:     // ucm_status.json not found
        case UcmStatusStorage::CheckResultType::kUCMStatusMD5FileEmpty:  // ucm_status.md5 not found
        case UcmStatusStorage::CheckResultType::
            kUCMStatusMD5NotConsistent: {  // ucm_status.json and ucm_status.md5 are inconsistent
            LOGD << "kUCMStatusFileEmpty or kUCMStatusMD5FileEmpty or kUCMStatusMD5NotConsistent,"
                 << " go to ExceptionRollingBackState";
            stateUPtr = CreateState(PackageManagerStatusType::kExceptionRollingBack,
                                    ExceptionRollingBackState().GetFinalActionType());
        } break;
        default: {
        } break;
    }

    if (stateUPtr != nullptr) {
        return stateUPtr;
    }

    // 3. Open ucm_status.json
    //       Only the current version has content, parse according to its content; --- No upgrade/downgrade rollback
    //       The current version has no corresponding content (has old UCM version content): --- Upgrade
    //          Convert the old version content to the content corresponding to the new version
    //       Read ProcessedSoftwarePackages and recover actions according to PackageManagerImpl::ProcessSoftwarePackage;
    //       And recover actionResolution and actionTimeStamp to the corresponding action
    //         ReversibleAction adds TransferIdType softwarePackageID_ so that it can be obtained when saving status information
    //         When entering the PackageManagerIdleState, ProcessedSoftwarePackages will be cleared (because FileSystemSWCLManager will be cleared)

    // Read the Version field, assert it is the current version fixedUCMVersion:1.0.0
    RManifestOpenAndCheck(doc, GetPathSettings().uCMStatusPath, nullptr);

    // Version information in the status file
    // "Version": "1.0.0",.
    AraString const ucmVersionStr{RManifestDocGetSS(doc, kUCMVersion)};
    LOGI << "got ucmVersionStr:" << ucmVersionStr.c_str() << ", kFixedUCMVersion:" << kFixedUCMVersion;

    Version const ucmVer{ucmVersionStr};
    Version const fixedUCMVer{kFixedUCMVersion};
    if (ucmVer > fixedUCMVer) {  // The UCM version in the status file is greater than the current UCM version
        LOGF << "ucmVersion > fixedUCMVersion, just exit";
        // TODO: Change to return an appropriate value to exit normally
        std::exit(-1);  // NOLINT : [std::exit]function is not thread safe, QAC also does not recommend using exit
    }

    // Recover the actions_ in SWCLManager_ from ProcessedSoftwarePackages in ucm_status.json
    // "ProcessedSoftwarePackages":
    // [
    //     {
    //         "TransferId":"2",.
    //         "actionResolution":"kSuccessfull",                --- Is it really unnecessary to recover these to memory? Or is recovery needed?
    //         "actionTimeStamp":"2021/09/17 18:32:55"
    //     },
    //     {
    //         "TransferId":"3",.
    //         "actionResolution":"kSuccessfull",.
    //         "actionTimeStamp":"2021/09/17 18:32:56"
    //     }
    // ]
    AraVector< ActionExecutionInfoType > actionExecutionInfoVec;
    RManifestLoadAndCheck(doc, kUCMProcessedSoftwarePackages, actionExecutionInfoVec, nullptr);
    LOGD << "got actionExecutionInfoVec.size:" << actionExecutionInfoVec.size();
    AraList< ActionExecutionInfoType > const actionExecutionInfoList{actionExecutionInfoVec.begin(),
                                                                     actionExecutionInfoVec.end()};

    // Read ProcessedSoftwarePackages and recover actions according to PackageManagerImpl::ProcessSoftwarePackage;
    // And recover actionResolution and actionTimeStamp to the corresponding action
    swclManager.RecoverProcessedSwpkgAction(actionExecutionInfoList);

    // Generate PackageManagerState from CurrentStatus and FinalAction in ucm_status.json;
    // PackageManager constructor will use impl_->RetrieveState() to set state_
    // PackageManager constructor will call UpdateCurrentStatus(state_.Get()->GetStatus());.
    // PackageManager::PackageManager(std::unique_ptr<PackageManagerImpl>&& impl, com::InstanceIdentifier instance_id)
    //     : Skeleton(instance_id, ara::com::MethodCallProcessingMode::kEvent),.
    //       impl_{std::move(impl)}.  // This lambda expression implements: Update fields::CurrentStatus CurrentStatus when Accessor::Reset is called;
    //       ,
    //       state_{impl_->RetrieveState(), [this](const PackageManagerState& s) { impl_->SaveCurrentState(s);.
    //       UpdateCurrentStatus(s.GetStatus()); }} {.
    //     // Update fields::CurrentStatus CurrentStatus upon first time;
    //     UpdateCurrentStatus(state_.Get()->GetStatus());.
    // }
    // Afterwards, PackageManagementApplication::Run will call static_cast<PackageManager*>(service_.get())->ContinueIfNeeded();
    // This is consistent with the normal transition flow: [this](const PackageManagerState& s) { impl_->SaveCurrentState(s);
    // UpdateCurrentStatus(s.GetStatus()); }; then call something like StartActivation.

    // "CurrentStatus":0,.
    PackageManagerStatusType currentStatus{};
    *reinterpret_cast< uint8_t* >(&(currentStatus))
        = static_cast< uint8_t >(RManifestDocGetI(doc, kUCMCurrentStatus, 0));

    // "FinalAction":0,.    //.
    // Save only when entering the CLEANING_UP state from PROCESSING/READY/ACTIVATED/ROLLED-BACK, and read when recovering to the CLEANING_UP state
    // (For consistency, other states can save/read empty).
    FinalActionType finalActionType{};
    *reinterpret_cast< uint8_t* >(&(finalActionType))
        = static_cast< uint8_t >(RManifestDocGetI(doc, kUCMFinalAction, 0));
    LOGD << "got currentStatus:" << strtype::PackageManagerStatusTypeToStr(currentStatus).c_str()
         << " finalActionType:" << strtype::FinalActionTypeToStr(finalActionType).c_str();

    AssertWithLog((PackageManagerStatusType::kProcessing != currentStatus)
                  && (PackageManagerStatusType::kExceptionRollingBack != currentStatus));

    return CreateState(currentStatus, finalActionType);
}

/// @brief CreateState
/// @param status PackageManagerStatusType
/// @param finalActionIfCleaningUp FinalActionType
/// @throws no
/// @return PackageManagerState
std::unique_ptr< PackageManagerState > FsmManager::CreateState(PackageManagerStatusType const status,
                                                               FinalActionType finalActionIfCleaningUp) noexcept
{
    LOGD << "call... status:" << strtype::PackageManagerStatusTypeToStr(status).c_str()
         << " finalActionType:" << strtype::FinalActionTypeToStr(finalActionIfCleaningUp).c_str();

    std::unique_ptr< PackageManagerState > stateUPtr{nullptr};

    switch (status) {
        case PackageManagerStatusType::kIdle: {
            stateUPtr = std::make_unique< IdleState >();
            break;
        }
        case PackageManagerStatusType::kReady: {
            stateUPtr = std::make_unique< ReadyState >();
            break;
        }
        case PackageManagerStatusType::kProcessing: {
            stateUPtr = std::make_unique< ProcessingState >();
            break;
        }
        case PackageManagerStatusType::kActivating: {
            stateUPtr = std::make_unique< ActivatingState >();
            break;
        }
        case PackageManagerStatusType::kActivated: {
            stateUPtr = std::make_unique< ActivatedState >();
            break;
        }
        case PackageManagerStatusType::kRollingBack: {
            stateUPtr = std::make_unique< RollingBackState >();
            break;
        }
        case PackageManagerStatusType::kRolledBack: {
            stateUPtr = std::make_unique< RolledBackState >();
            break;
        }
        case PackageManagerStatusType::kCleaningUp: {
            stateUPtr   = std::make_unique< CleaningUpState >(finalActionIfCleaningUp);
            std::ignore = finalActionIfCleaningUp;
            break;
        }
        case PackageManagerStatusType::kVerifying: {
            stateUPtr = std::make_unique< VerifyingState >();
            break;
        }
        case PackageManagerStatusType::kExceptionRollingBack: {
            stateUPtr = std::make_unique< ExceptionRollingBackState >();
            break;
        }
        case PackageManagerStatusType::kExceptionCleaningUp: {
            stateUPtr = std::make_unique< ExceptionCleaningUpState >();
            break;
        }
        default: {
            stateUPtr = std::make_unique< IdleState >();
            break;
        }
    }

    return stateUPtr;
}

/// @brief _onSwitchNewState
/// @param st PackageManagerState
/// @throws no
void FsmManager::_onSwitchNewState(PackageManagerState const& st) noexcept
{
    st.SetOwner(this);

    PackageManagerStatusType const status{st.GetStatus()};
    LOGD << "start with state:" << strtype::PackageManagerStatusTypeToStr(status).c_str();

    // When UCM enters any state according to the state machine:
    //     //
    //     Therefore, when ucm_status.json is valid, the stored CurrentStatus indicates that the work of this phase is complete and a restart can be performed (the restart will still be in this phase, but the previous work will be skipped).
    //     (With the current UCM version) First modify/overwrite ucm_status.json and delete the corresponding md5 file;    --- Must not delete and rewrite
    //     Execute the corresponding operation; (there may be no operation).
    //     Generate ucm_status.json and its corresponding ucm_status.md5;
    //       --- Can save in IDLE, READY, ACTIVATED, ROLLED-BACK (these 4 states have no work to do and wait for user calls). (Although READY saves, an unexpected power off during PROCESSING state will lose previously processed software packages).
    //       --- When recovering to these states, ucm_status.json should not be rewritten
    //       --- Must save before restarting in VERIFYING/ROLLING-BACK (only need to commit). RollingBack involves conversion from new UCM version to old UCM version
    //       Also save at this time    --- Recovering to VERIFYING will invalidate ucm_status.json; recovering to ROLLING-BACK will not invalidate ucm_status.json
    //       --- Save also when entering CLEANING_UP (to avoid going all the way to kExceptionRollingBack), and save FinalAction at the same time, so it can be redone after restart? en
    //       --- When recovering to this state, ucm_status.json should not be rewritten
    //       --- Rollback cleanup state: save as kExceptionCleaningUp before restarting in kExceptionRollingBack (requires Begin+Set+Commit).
    //       --- When recovering to kExceptionCleaningUp, ucm_status.json will not be invalidated
    //                 //--- Rollback cleanup state: save before restarting in ROLLING-BACK-AND-CLEANING_UP (requires Begin+Set+Commit).
    //                 --- When recovering to ROLLING-BACK-AND-CLEANING_UP, ucm_status.json will not be invalidated
    //          Is saving as kExceptionCleaningUp before restarting in kExceptionRollingBack different from "save before restarting in VERIFYING/ROLLING-BACK (only need to commit)" above? Because:
    //          When in the kExceptionRollingBack state, some other things need to be done. The work done earlier in the VERIFYING/ROLLING-BACK phase will not be redone upon recovery.

    // Start transaction
    UcmStatusStorage::BeginSaveTransaction();

    // Save state
    UcmStatusStorage::SaveCurrentStatus(st, swclManager.GetActionExecutionInfo());

    // Commit transaction
    if ((PackageManagerStatusType::kIdle == status) || (PackageManagerStatusType::kReady == status)
        || (PackageManagerStatusType::kActivated == status) || (PackageManagerStatusType::kRolledBack == status)
        || (PackageManagerStatusType::kCleaningUp == status)
        || (PackageManagerStatusType::kExceptionCleaningUp == status)) {
        UcmStatusStorage::CommitSaveTransaction();
    }
}

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara
