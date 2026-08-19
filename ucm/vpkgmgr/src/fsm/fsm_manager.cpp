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
/// @brief      FsmManager impl
/// @details
/// @date       2024-07-20
/// @author     hanzhibo
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/UCM Master/Fsm
/// @interface_level=unit
/// @trace_id_sr=SR_UCM_00035
/// @unit_name=FsmManager
/// @unit_description=FsmManager impl
/// @endcode
///
/// ================================================================

#include "fsm/fsm_manager.h"  //NOLINT

#include <ara/ucm/internal/extraction/filesystem.h>
#include <isoft/ara_fsh/filesystem_hierarchy.h>

#include <chrono>
#include <utility>

#include "ara/ucm/internal/extraction/tinyfs.h"
#include "consts.h"
#include "fsm/states/campaign_state_activating.h"
#include "fsm/states/campaign_state_canceling.h"
#include "fsm/states/campaign_state_corrupted_status_file.h"
#include "fsm/states/campaign_state_idle.h"
#include "fsm/states/campaign_state_processing.h"
#include "fsm/states/campaign_state_softpkgtransfer.h"
#include "fsm/states/campaign_state_sync.h"
#include "fsm/states/campaign_state_vehicle_checking.h"
#include "fsm/states/campaign_state_vpkgtransfer.h"
#include "fsm/storage/history_database.h"
#include "fsm/storage/persistence.h"
#include "fsm/sync_executor.h"
#include "fsm/task_executor/task_queue.h"
#include "package_management_service/find_ucm_subs.h"
#include "services/concentrator.h"
#include "utils/helper.h"
#include "utils/ucmm_log.h"
#include "utils/utils.h"
#include "vehicle_package_application.h"

namespace ara {
namespace ucm {
namespace vpkgmgr {

/// @brief _createState
/// @param status
/// @param cancelCause
/// @return CampaignStateBase::Ptr
/// @throws no
CampaignStateBase::Ptr FsmManager::_createState(pkgmgr::CampaignStateType const status,
                                                pkgmgr::UCMMasterResolutionType const cancelCause) const
{
    core::String padding;
    log_.LogDebug() << "FsmManager::_createState(), start with status:" << CampaignStateTypeToString(status).data()
                    << " cancelCause:"
                    << com::internal::format::Formatter< core::String, pkgmgr::UCMMasterResolutionType >::ToString(
                           padding, cancelCause)
                           .c_str();

    CampaignStateBase::Ptr state;
    switch (status) {
        case pkgmgr::CampaignStateType::kIdle: {
            state = std::make_unique< CampaignStateIdle >();
            break;
        }
        case pkgmgr::CampaignStateType::kSyncing: {
            state = std::make_unique< CampaignStateSync >();
            break;
        }
        case pkgmgr::CampaignStateType::kVehiclePackageTransferring: {
            state = std::make_unique< CampaignStateVpkgTransfer >();
            break;
        }
        case pkgmgr::CampaignStateType::kSoftwarePackage_Transferring: {
            state = std::make_unique< CampaignStateSoftpkgTransfer >();
            break;
        }
        case pkgmgr::CampaignStateType::kProcessing: {
            state = std::make_unique< CampaignStateProcessing >();
            break;
        }
        case pkgmgr::CampaignStateType::kActivating: {
            state = std::make_unique< CampaignStateActivating >();
            break;
        }
        case pkgmgr::CampaignStateType::kVehicleChecking: {
            state = std::make_unique< CampaignStateVehicleCheck >();
            break;
        }
        case pkgmgr::CampaignStateType::kCancelling: {
            state = std::make_unique< CampaignStateCanceling >(cancelCause);
            break;
        }
        case pkgmgr::CampaignStateType::kCorruptedStatusFile: {
            state = std::make_unique< CampaignStateCorruptedStatusFile >();
            break;
        }
        default: {
            state = nullptr;
            break;
        }
    }
    return state;
}

/// @brief Initialize
/// @param vpApplication
/// @return Returns 0: success, non-zero: error code
/// @throws no
int32_t FsmManager::Init(VehiclePackageManagementApplication* const vpApplication)
{
    log_.LogInfo() << "FsmManager::Init(), begin.";

    subs_ = FindUcmSubs::GetInstance();
    isoft::ara_fsh::Platform const platform;

    varDataDir_ = ara::core::String(platform.GetAraVarDir()) + "/ucmm/";
    log_.LogDebug() << "FsmManager::Init(), varDataDir_:" << varDataDir_.c_str();
    if (!pkgmgr::Filesystem::DoesDirectoryExist(varDataDir_)) {
        int32_t const r{nai_dir_create(varDataDir_.c_str(), 0755, 1)};
        if (r < 0) {
            int32_t const err{nai_errno};
            log_.LogDebug() << "FsmManager::Init(), nai_dir_create return r:" << r
                            << " and err:" << strerror(err)  // NOLINT: [strerror]function is not thread safe
                            << "for varDataDir_:" << varDataDir_;
            if (err == EEXIST) {
                log_.LogDebug() << "FsmManager::Init(), nai_errno: EEXIST, so we can skip it.";
            } else {
                log_.LogError() << "FsmManager::Init(), failed to create vpkgsDir_:" << vpkgsDir_.c_str();
                return 1;
            }
        }
    }

    this->vpkgsDir_ = varDataDir_ + "/vpkgs/";
    log_.LogDebug() << "FsmManager::Init(), vpkgsDir_:" << vpkgsDir_.c_str();

    if (!pkgmgr::Filesystem::DoesDirectoryExist(vpkgsDir_)) {
        int32_t const r{nai_dir_create(vpkgsDir_.c_str(), 0755, 1)};
        if (r < 0) {
            int32_t const err{nai_errno};
            log_.LogDebug() << "FsmManager::Init(), nai_dir_create return r:" << r
                            << " and err:" << strerror(err)  // NOLINT: [strerror]function is not thread safe
                            << "for vpkgsDir_:" << vpkgsDir_;
            if (err == EEXIST) {
                log_.LogDebug() << "FsmManager::Init(), nai_errno: EEXIST, so we can skip it.";
            } else {
                log_.LogError() << "FsmManager::Init(), failed to create vpkgsDir_:" << vpkgsDir_.c_str();
                return 1;
            }
        }
    }

    packageStorage_ = PackageStorage::Create(this->vpkgsDir_);  // Package transfer handling

    /// Initialize state
    this->state_ = std::make_unique< LockedPtrWrapper< CampaignStateBase > >(
        RetrieveState(), [this](CampaignStateBase const* const pre, CampaignStateBase const& s) {
            // Save intermediate information
            DoWhileSwitchingNewState(pre, s);

            UpdateCurrentStatus(s.GetCampaignState());
        });

    // Restore downloaded software packages - relatively fast
    packageStorage_->RetrievePackagesData(vpkgsDir_);

    pkgmgr::CampaignStateType const retrievedState{state_->Get()->GetCampaignState()};
    log_.LogDebug() << "FsmManager::Init(), get retrievedState:" << CampaignStateTypeToString(retrievedState).data();

    if (!vpApplication->InitServices(retrievedState)) {
        log_.LogError() << "FsmManager::Init(), failed to InitServices.";
        return 1;
    }

    /// Check if there is an incomplete upgrade
    /// JSON storage
    std::ignore = TaskQueue::GetInstance()->PushTask([this, retrievedState]() -> int32_t {
        ContinueLastUpdateCamaign(retrievedState);
        return 0;
    });

    log_.LogInfo() << "FsmManager::Init(), end.";
    return 0;
}

/// @brief DoWhileSwitchingNewState
/// @param pre
/// @param s
/// @throws no
void FsmManager::DoWhileSwitchingNewState(CampaignStateBase const* const pre, CampaignStateBase const& s)
{
    pkgmgr::CampaignStateType const state{s.GetCampaignState()};
    log_.LogDebug() << "FsmManager::DoWhileSwitchingNewState(), start with state:"
                    << CampaignStateTypeToString(state).data();

    // Previous state
    if (nullptr != pre) {
        preState_ = pre->GetCampaignState();
        log_.LogDebug() << "FsmManager::DoWhileSwitchingNewState(), get preState_:"
                        << CampaignStateTypeToString(preState_).data();
    }

    // If state is kVehiclePackageTransferring, update heartbeat time
    if (pkgmgr::CampaignStateType::kVehiclePackageTransferring == state) {
        lastHeatBeatTime_ = static_cast< std::uint64_t >(
            std::chrono::duration_cast< std::chrono::milliseconds >(std::chrono::system_clock::now().time_since_epoch())
                .count());
        bool const ret{HistoryDatabase::GetInstance()->GetNewCampaignKey(campaignHistoryKey_)};
        ///assert(ret);

        log_.LogDebug() << "PackageManagerImpl::DoWhileSwitchingNewState(), get lastHeatBeatTime_:" << lastHeatBeatTime_
                        << " ret:" << ret << " campaignHistoryKey_:" << campaignHistoryKey_.c_str();
    } else if (pkgmgr::CampaignStateType::kVehicleChecking == state) {
        beginFinish_ = false;
    } else if (pkgmgr::CampaignStateType::kIdle == state) {
        lastHeatBeatTime_   = 0U;
        campaignHistoryKey_ = "";
    } else {
        ;
    }

    _saveStatus(state, s.GetCancelCause());
    log_.LogDebug() << "PackageManagerImpl::DoWhileSwitchingNewState(), end.";
}

/// @brief Update heartbeat
/// @throws no
void FsmManager::UpdateHeartbeat()
{
    // Current state
    pkgmgr::CampaignStateType const state{state_->Get()->GetCampaignState()};
    log_.LogDebug() << "FsmManager::UpdateHeartbeat(), start with state:" << CampaignStateTypeToString(state).data();

    if ((pkgmgr::CampaignStateType::kIdle != state) && (pkgmgr::CampaignStateType::kSyncing != state)
        && (pkgmgr::CampaignStateType::kCorruptedStatusFile != state)) {
        lastHeatBeatTime_ = static_cast< std::uint64_t >(
            std::chrono::duration_cast< std::chrono::milliseconds >(std::chrono::system_clock::now().time_since_epoch())
                .count());
        log_.LogDebug() << "FsmManager::UpdateHeartbeat(), update lastHeatBeatTime_:" << lastHeatBeatTime_;
    }

    // Update persistent file
    _saveStatus(state, state_->Get()->GetCancelCause());
    log_.LogDebug() << "FsmManager::UpdateHeartbeat(), end.";
}

/// @brief _saveStatus
/// @param state
/// @param cancelCause
/// @throws no
void FsmManager::_saveStatus(pkgmgr::CampaignStateType const& state,
                             pkgmgr::UCMMasterResolutionType const cancelCause) const
{
    core::String padding;
    log_.LogDebug() << "FsmManager::_saveStatus(), start with state:" << CampaignStateTypeToString(state).data()
                    << "cancelCause:"
                    << com::internal::format::Formatter< core::String, pkgmgr::UCMMasterResolutionType >::ToString(
                           padding, cancelCause)
                           .c_str();

    // Start transaction
    Persistence::GetInstance()->BeginSaveTransaction();

    // Vehicle package id
    pkgmgr::TransferIdType* pvpkTransferId{nullptr};
    pkgmgr::TransferIdType vpkTransferId{};
    if (vpkgTransfer_) {
        vpkTransferId  = vpkgTransfer_->GetVPKTransferID();
        pvpkTransferId = &vpkTransferId;
    }

    // Mapping from software package FQN to transfer ID
    ara::core::Map< ara::core::String, pkgmgr::TransferIdType > swpkFQN2TransferIDMap;
    if (spkgTransferManager_) {
        swpkFQN2TransferIDMap = spkgTransferManager_->GetSwpkFQN2TransferIDMap();
    }

    // Save state
    Persistence::GetInstance()->SetCurrentState(kUcmmVersion, preState_, state, pvpkTransferId, beginFinish_,
                                                lastHeatBeatTime_, allowCampaign_, swpkFQN2TransferIDMap,
                                                campaignHistoryKey_, cancelCause);

    // Commit transaction
    Persistence::GetInstance()->CommitSaveTransaction();

    log_.LogDebug() << "PackageManagerImpl::_saveStatus(), end.";
}

/// @brief RetrieveState
/// @return CampaignStateBase unique_ptr
/// @throws no
std::unique_ptr< CampaignStateBase > FsmManager::RetrieveState()
{
    log_.LogDebug() << "FsmManager::RetrieveState, start.";

    // ucmm status file path
    ara::core::String const statusFilePath{GetStatusFilePath()};
    log_.LogDebug() << "FsmManager::RetrieveState(), statusFilePath:" << statusFilePath.c_str();

    ara::core::String const stMD5Str{helper::MD5ForFile(statusFilePath)};
    log_.LogDebug() << "FsmManager::RetrieveState, get stMD5Str:" << stMD5Str.c_str()
                    << " for statusFilePath:" << statusFilePath.c_str();

    // ucmm status md5 file path
    ara::core::String const stStatusMD5FilePath{GetStatusMD5FilePath()};
    log_.LogDebug() << "FsmManager::RetrieveState(), stStatusMD5FilePath:" << stStatusMD5FilePath.c_str();

    ara::core::String const stMD5StrFromMD5File{pkgmgr::tinyfs::ReadMD5FromFile(stStatusMD5FilePath)};
    log_.LogDebug() << "FsmManager::RetrieveState, get stMD5StrFromMD5File:" << stMD5StrFromMD5File.c_str()
                    << " from stStatusMD5FilePath:" << stStatusMD5FilePath.c_str();

    if (stMD5Str.empty() && stMD5StrFromMD5File.empty()) {  // status.json and status.md5 files not found, regenerate.
        log_.LogDebug() << "FsmManager::RetrieveState, both statusFilePath:" << statusFilePath.c_str()
                        << " and stStatusMD5FilePath:" << stStatusMD5FilePath.c_str()
                        << " don't exist, so we will generate them with CampaignStateIdle.";

        // Generate idle status status.json and status.md5 files
        ara::core::Map< ara::core::String, pkgmgr::TransferIdType > const swpkFQN2TransferIDMap;
        Persistence::GetInstance()->SetCurrentState(
            kUcmmVersion, preState_, pkgmgr::CampaignStateType::kIdle, nullptr, beginFinish_, lastHeatBeatTime_,
            allowCampaign_, swpkFQN2TransferIDMap, campaignHistoryKey_, pkgmgr::UCMMasterResolutionType::kSuccess);

        // Commit transaction
        Persistence::GetInstance()->CommitSaveTransaction();
        return _createState(pkgmgr::CampaignStateType::kIdle, pkgmgr::UCMMasterResolutionType::kSuccess);
    }
    if (stMD5Str.empty()) {  // status.json file not found
        log_.LogError() << "FsmManager::RetrieveState, can't get MD5 for statusFilePath:" << statusFilePath.c_str();
        return _createState(pkgmgr::CampaignStateType::kCorruptedStatusFile, pkgmgr::UCMMasterResolutionType::kSuccess);
    }
    if (stMD5StrFromMD5File.empty()) {  // status.md5 file not found
        log_.LogError() << "FsmManager::RetrieveState, can't read MD5 from stStatusMD5FilePath:"
                        << stStatusMD5FilePath.c_str();
        return _createState(pkgmgr::CampaignStateType::kCorruptedStatusFile, pkgmgr::UCMMasterResolutionType::kSuccess);
    }

    if (0 != stMD5Str.compare(stMD5StrFromMD5File)) {
        log_.LogError() << "FsmManager::RetrieveState, stMD5Str:" << stMD5Str.c_str()
                        << " is not equal with stMD5StrFromMD5File" << stMD5StrFromMD5File.c_str();
        return _createState(pkgmgr::CampaignStateType::kCorruptedStatusFile, pkgmgr::UCMMasterResolutionType::kSuccess);
    }

    core::String statusVersion;
    pkgmgr::CampaignStateType campaignState;
    std::unique_ptr< pkgmgr::TransferIdType > vpkTransferId{nullptr};
    pkgmgr::UCMMasterResolutionType cancelCause{pkgmgr::UCMMasterResolutionType::kSuccess};
    bool const ret{Persistence::GetInstance()->RetrieveState(
        statusVersion, preState_, campaignState, vpkTransferId, beginFinish_, lastHeatBeatTime_, allowCampaign_,
        retrievedSwpkFQN2TransferIDMap_, campaignHistoryKey_, cancelCause)};
    assert(ret);
    std::ignore = ret;

    // Create vpkgTransfer_
    if (vpkTransferId) {
        vpkgTransfer_ = VehiclePackageTransfer::Create(packageStorage_, blockSize_, *vpkTransferId);
    }

    return _createState(campaignState, cancelCause);
}

/// @brief Notify the driver of the current activity status of the ota_client
/// @param state
/// @throws no
void FsmManager::UpdateCurrentStatus(pkgmgr::CampaignStateType const& state)
{
    LOG_INFO << "change CampaignState=" << CampaignStateTypeToString(state).data();
    return Concentrator::GetInstance()->ReportState(state);
}

/// @brief Check if the last update is complete; if not, continue the last update activity
/// @param state
/// @throws no
void FsmManager::ContinueLastUpdateCamaign(pkgmgr::CampaignStateType const state)
{
    log_.LogDebug() << "FsmManager::ContinueLastUpdateCamaign, begin with state:"
                    << CampaignStateTypeToString(state).data();
    switch (state) {
        case pkgmgr::CampaignStateType::
            kCorruptedStatusFile: {  // When CampaignState is CorruptedStatusFile (file content does not match its md5):
            Abort();
            break;
        }
        case pkgmgr::CampaignStateType::
            kIdle: {  // When CampaignState is IDLE   // If there is no status file, also set to this state
            break;    //     Do nothing
        }
        case pkgmgr::CampaignStateType::kSyncing: {  // When CampaignState is SYNCING
            CleanUpAllUCMs();                        //     Execute CleanUpAllUCMs
                                                     //     After completion, switch to IDLE state
            break;
        }
        case pkgmgr::CampaignStateType::
            kVehiclePackageTransferring: {  // When CampaignState is VEHICLEPACKAGE_TRANSFERRING, cancel
            if (vpkgTransfer_) {  // If there is a vehicle package ID, check if the vehicle package download is complete. If complete, re-parse the vehicle package: VehiclePackageTransfer::ProcessVehiclePackage(triggerSoftpkgTransfer=false)
                if (vpkgTransfer_->IsVPKTransferred()) {
                    _reloadLastVehiclePackage();
                }
            }

            ara::core::Promise< void > promise;
            // FsmManager::TriggerCancel is more graceful than FsmManager::Abort, can perform some cleanup
            std::ignore = TriggerCancel(pkgmgr::UCMMasterResolutionType::kCampaignCancelled, std::move(promise));
            break;
        }
        case pkgmgr::CampaignStateType::kSoftwarePackage_Transferring:  // Cancel
        case pkgmgr::CampaignStateType::kProcessing: {                  // Cancel
            _reloadLastVehiclePackage();
            ara::core::Promise< void > promise;
            std::ignore = TriggerCancel(pkgmgr::UCMMasterResolutionType::kCampaignCancelled, std::move(promise));
            break;
        }
        case pkgmgr::CampaignStateType::kActivating: {  // When CampaignState is ACTIVATING, continue processing
            _reloadLastVehiclePackage();
            MarkActivatingStart();
            std::ignore = _executeActivateStep();
            break;
        }
        case pkgmgr::CampaignStateType::kVehicleChecking: {
            log_.LogDebug() << "FsmManager::ContinueLastUpdateCamaign, get beginFinish_:" << beginFinish_;
            if (!beginFinish_) {  // Finish not started, cancel
                _reloadLastVehiclePackage();
                ara::core::Promise< void > promise;
                std::ignore = TriggerCancel(pkgmgr::UCMMasterResolutionType::kCampaignCancelled, std::move(promise));
            } else {  // Finish already started, continue processing
                if (vpkgTransfer_) {
                    if (vpkgTransfer_->IsVPKTransferred()) {
                        if (vpkgTransfer_->IsVPKExist()) {
                            _reloadLastVehiclePackage();
                        }
                    }
                }
                MarkVehicleCheckFinishingStart();
                std::ignore = _executeVehicleCheck();
            }
            break;
        }
        case pkgmgr::CampaignStateType::kCancelling: {  // When CampaignState is CANCELLING, continue processing
            if (vpkgTransfer_) {  // If there is a vehicle package ID, check if the vehicle package download is complete. If complete, re-parse the vehicle package (if the vehicle package has been deleted, such as restart after power failure during TriggerIdle, just continue execution): VehiclePackageTransfer::ProcessVehiclePackage(triggerSoftpkgTransfer=false)
                if (vpkgTransfer_->IsVPKTransferred()) {
                    if (vpkgTransfer_->IsVPKExist()) {
                        _reloadLastVehiclePackage();
                    }
                }
            }
            MarkCancellingStart();
            std::shared_ptr< ara::core::Promise< void > > promise{std::make_shared< ara::core::Promise< void > >()};
            std::ignore = _executeCancel(state_->Get()->GetCancelCause(), preState_, std::move(promise));
            break;
        }
        default: {
            break;
        }
    }

    // Clean up temporary data
    retrievedSwpkFQN2TransferIDMap_.clear();
    log_.LogDebug() << "FsmManager::ContinueLastUpdateCamaign, end.";
    return;
}

/// @brief Reload the vehicle package from the last update
/// @throws no
void FsmManager::_reloadLastVehiclePackage()
{
    log_.LogDebug() << "FsmManager::_reloadLastVehiclePackage(), begin.";
    ara::core::Promise< void > promise;
    int32_t const ret{
        vpkgTransfer_->ProcessVehiclePackage(vpkgTransfer_->GetVPKTransferID(), std::move(promise), false)};
    assert(0 == ret);
    std::ignore = ret;

    // List of ucmIDs involved in the vehicle package
    AraList< ara::core::String > const ucmIDs{vpkgTransfer_->GetUcmIds()};
    log_.LogDebug() << "FsmManager::_reloadLastVehiclePackage(), get ucmIDs:"
                    << Utils::ConcatenateStrings(ucmIDs).c_str();

    // Need to wait for all ucms related to the vehicle package to be online
    while (!subs_->UcmIsAllExsit(ucmIDs)) {
        log_.LogDebug() << "FsmManager::_reloadLastVehiclePackage(), waiting all ucms discovered.";
        std::this_thread::sleep_for(std::chrono::milliseconds(kGlobalWaitTime));
    }
    log_.LogDebug() << "FsmManager::_reloadLastVehiclePackage(), end.";
}

/// @brief Mark if processing of the vehicle package has started
/// @throws no
void FsmManager::MarkProcessingVPStart() noexcept
{
    log_.LogDebug() << "FsmManager::MarkProcessingVPStart(), begin.";
    processingVP_ = true;
    log_.LogDebug() << "FsmManager::MarkProcessingVPStart(), end.";
}

/// @brief Mark the completion of VehiclePackageTransfer::ProcessVehiclePackage
/// @throws no
void FsmManager::MarkProcessingVPFinish() noexcept
{  // Mark the end of VehiclePackageTransfer::ProcessVehiclePackage
    log_.LogDebug() << "FsmManager::MarkProcessingVPFinish(), begin.";
    processingVP_ = false;
    processingVPCondition_.notify_all();  // Notify condition variable for completion of vehicle package processing
    log_.LogDebug() << "FsmManager::MarkProcessingVPFinish(), end.";
}

/// @brief Wait for VehiclePackageTransfer::ProcessVehiclePackage to finish / not yet executed
/// @throws no
void FsmManager::WaitProcessingVPFinish()
{
    log_.LogDebug() << "FsmManager::WaitProcessingVPFinish(), begin.";
    std::unique_lock< std::mutex > condLock{condMutex_};
    processingVPCondition_.wait(condLock, [this]() noexcept -> bool { return !processingVP_; });
    log_.LogDebug() << "FsmManager::WaitProcessingVPFinish(), end.";
}

/// @brief Mark if software package transfer has started
/// @throws no
void FsmManager::MarkTransferringStart() noexcept
{
    log_.LogDebug() << "FsmManager::MarkTransferringStart(), begin.";
    transferring_ = true;
    log_.LogDebug() << "FsmManager::MarkTransferringStart(), end.";
}

/// @brief Mark the end of software package transfer in SOFTWAREPACKAGE_TRANSFERRING state
/// @throws no
void FsmManager::MarkTransferringFinish() noexcept
{  // Mark the end of software package transfer in SOFTWAREPACKAGE_TRANSFERRING state, but the actual transfer of software packages to ucm may still be ongoing
    log_.LogDebug() << "FsmManager::MarkTransferringFinish(), begin.";
    transferring_ = false;
    transferringCondition_.notify_all();  // Notify condition variable for completion
    log_.LogDebug() << "FsmManager::MarkTransferringFinish(), end.";
}

/// @brief  Wait for the software package transfer in this state within FsmManager::ExecuteProcessStep to end:
/// @throws no
void FsmManager::WaitTransferringFinish()
{
    log_.LogDebug() << "FsmManager::WaitTransferringFinish(), begin.";
    std::unique_lock< std::mutex > condLock{condMutex_};
    transferringCondition_.wait(condLock, [this]() noexcept -> bool { return !transferring_; });
    log_.LogDebug() << "FsmManager::WaitTransferringFinish(), end.";
}

/// @brief Mark if software package processing has started
/// @throws no
void FsmManager::MarkProcessingStart() noexcept
{
    log_.LogDebug() << "FsmManager::MarkProcessingStart(), begin.";
    processing_ = true;
    log_.LogDebug() << "FsmManager::MarkProcessingStart(), end.";
}

/// @brief Mark the end of software package processing
/// @throws no
void FsmManager::MarkProcessingFinish() noexcept
{  // Mark the end of software package processing
    log_.LogDebug() << "FsmManager::MarkProcessingFinish(), begin.";
    processing_ = false;
    processingCondition_.notify_all();  // Notify condition variable for completion
    log_.LogDebug() << "FsmManager::MarkProcessingFinish(), end.";
}

/// @brief Wait for FsmManager::ExecuteProcessStep to end
/// @throws no
void FsmManager::WaitProcessingFinish()
{
    log_.LogDebug() << "FsmManager::WaitProcessingFinish(), begin.";
    std::unique_lock< std::mutex > condLock{condMutex_};
    processingCondition_.wait(condLock, [this]() noexcept -> bool { return !processing_; });
    log_.LogDebug() << "FsmManager::WaitProcessingFinish(), end.";
}

/// @brief Mark if Activate has started
/// @throws no
void FsmManager::MarkActivatingStart() noexcept
{
    log_.LogDebug() << "FsmManager::MarkActivatingStart(), begin.";
    activating_ = true;
    log_.LogDebug() << "FsmManager::MarkActivatingStart(), end.";
}

/// @brief Mark the completion of FsmManager::ExecuteActivateStep
/// @throws no
void FsmManager::MarkActivatingFinish() noexcept
{  // Mark the end of FsmManager::ExecuteActivateStep
    log_.LogDebug() << "FsmManager::MarkActivatingFinish(), begin.";
    activating_ = false;
    activatingCondition_.notify_all();  // Notify condition variable for activation completion
    log_.LogDebug() << "FsmManager::MarkActivatingFinish(), end.";
}

/// @brief Wait for FsmManager::ExecuteActivateStep to end
/// @throws no
void FsmManager::WaitActivatingFinish()
{
    log_.LogDebug() << "FsmManager::WaitActivatingFinish(), begin.";
    std::unique_lock< std::mutex > condLock{condMutex_};
    activatingCondition_.wait(condLock, [this]() noexcept -> bool {
        return !activating_;  // No need to wait for terminate_ to be true, because when it is true, activating_ will be false
    });
    log_.LogDebug() << "FsmManager::WaitActivatingFinish(), end.";
}

/// @brief Mark if VehicleCheck preparation has started
/// @throws no
void FsmManager::MarkVehicleCheckPreparingStart() noexcept
{
    log_.LogDebug() << "FsmManager::MarkVehicleCheckPreparingStart(), begin.";
    vehicleCheckPreparing_ = true;
    log_.LogDebug() << "FsmManager::MarkVehicleCheckPreparingStart(), end.";
}

/// @brief Mark the completion of VehicleCheck preparation
/// @throws no
void FsmManager::MarkVehicleCheckPreparingFinish() noexcept
{
    log_.LogDebug() << "FsmManager::MarkVehicleCheckPreparingFinish(), begin.";
    vehicleCheckPreparing_ = false;
    vehicleCheckPreparingCondition_
        .notify_all();  // Notify condition variable for completion of VehicleCheck preparation
    log_.LogDebug() << "FsmManager::MarkVehicleCheckPreparingFinish(), end.";
}

/// @brief Wait for the end of preparation in ExecuteVehicleCheck
/// @throws no
void FsmManager::WaitVehicleCheckPreparingFinish()
{
    log_.LogDebug() << "FsmManager::WaitVehicleCheckPreparingFinish(), begin.";
    std::unique_lock< std::mutex > condLock{condMutex_};
    vehicleCheckPreparingCondition_.wait(condLock, [this]() noexcept -> bool { return !vehicleCheckPreparing_; });
    log_.LogDebug() << "FsmManager::WaitVehicleCheckPreparingFinish(), end.";
}

/// @brief Mark if Finish has started
/// @throws no
void FsmManager::MarkVehicleCheckFinishingStart() noexcept
{
    log_.LogDebug() << "FsmManager::MarkVehicleCheckFinishingStart(), begin.";
    vehicleCheckFinishing_ = true;
    log_.LogDebug() << "FsmManager::MarkVehicleCheckFinishingStart(), end.";
}
/// @brief Mark the completion of Finish
/// @throws no
void FsmManager::MarkVehicleCheckFinishingFinish() noexcept
{
    log_.LogDebug() << "FsmManager::MarkVehicleCheckFinishingFinish(), begin.";
    vehicleCheckFinishing_ = false;
    vehicleCheckFinishingCondition_.notify_all();  // Notify condition variable for completion of VehicleCheck Finish
    log_.LogDebug() << "FsmManager::MarkVehicleCheckFinishingFinish(), end.";
}

/// @brief Wait for the end of Finish in ExecuteVehicleCheck
/// @throws no
void FsmManager::WaitVehicleCheckFinishingFinish()
{
    log_.LogDebug() << "FsmManager::WaitVehicleCheckFinishingFinish(), begin.";
    std::unique_lock< std::mutex > condLock{condMutex_};
    vehicleCheckFinishingCondition_.wait(condLock, [this]() noexcept -> bool { return !vehicleCheckFinishing_; });
    log_.LogDebug() << "FsmManager::WaitVehicleCheckFinishingFinish(), end.";
}

/// @brief Mark if cancellation has started
/// @throws no
void FsmManager::MarkCancellingStart() noexcept
{
    log_.LogDebug() << "FsmManager::MarkCancellingStart(), begin.";
    cancelExecuting_ = true;
    log_.LogDebug() << "FsmManager::MarkCancellingStart(), end.";
}

/// @brief Mark the completion of FsmManager::ExecuteCancel
/// @throws no
void FsmManager::MarkCancellingFinish() noexcept
{  // Mark the end of FsmManager::ExecuteCancel
    log_.LogDebug() << "FsmManager::MarkCancellingFinish(), begin.";
    cancelExecuting_ = false;
    cancellingCondition_.notify_all();  // Notify condition variable for cancellation completion
    log_.LogDebug() << "FsmManager::MarkCancellingFinish(), end.";
}

/// @brief Wait for FsmManager::ExecuteCancel to end
/// @throws no
void FsmManager::WaitCancellingFinish()
{
    log_.LogDebug() << "FsmManager::WaitCancellingFinish(), begin.";
    std::unique_lock< std::mutex > condLock{condMutex_};
    cancellingCondition_.wait(condLock, [this]() noexcept -> bool { return !cancelExecuting_; });
    log_.LogDebug() << "FsmManager::WaitCancellingFinish(), end.";
}

/// @brief Exit Campaign
/// @throws no
void FsmManager::Abort()
{  // Execute FsmManager::Abort in TaskQueue
    log_.LogDebug() << "FsmManager::Abort(), begin with hasAborted_" << hasAborted_;
    {
        std::lock_guard< std::mutex > const lock{abortMutex_};
        if (hasAborted_) {  // Already exited
            log_.LogDebug() << "FsmManager::Abort(), end because of hasAborted_:" << hasAborted_;
            return;
        }
        hasAborted_ = true;
    }

    StateAccessor const accessor{state_->Get()};  // Lock state transition
    accessor->OnAbort();

    if (spkgTransferManager_) {  // Delete campaign-related software packages and their status information
        spkgTransferManager_->ClearSWPKGs();
    }

    // If there is a vehicle package
    if (vpkgTransfer_) {
        vpkgTransfer_
            ->ClearVPK();  // Clear the extracted files of the vehicle package, clean up the vehicle package zip file and its status information.
    }

    vpkgTransfer_.reset(nullptr);
    spkgTransferManager_.reset(nullptr);
    log_.LogDebug() << "FsmManager::Abort(), try to reset to CampaignStateIdle.";
    std::unique_ptr< CampaignStateIdle > idleUPtr{std::make_unique< CampaignStateIdle >()};
    accessor.Reset(std::move(idleUPtr));
    log_.LogDebug() << "FsmManager::Abort(), end.";
}

/// @brief CleanUpAllUCMs
/// @throws no
void FsmManager::CleanUpAllUCMs() const
{
    log_.LogDebug() << "FsmManager::CleanUpAllUCMs(), begin.";
    bool success{true};

    // Get service proxy corresponding to ucmID
    for (auto const& pair : FindUcmSubs::GetInstance()->GetAllPmApp()) {
        PackageManagementAppPtr const& ucmSubApp{pair.second};
        log_.LogDebug() << "FsmManager::CleanUpAllUCMs(), try to AttemptStateRecovery for ucmId:"
                        << ucmSubApp->GetId().c_str();
        if (!ucmSubApp->AttemptStateRecovery()) {  // Retry strategy is inside
            log_.LogDebug() << "FsmManager::CleanUpAllUCMs(), haven't finished AttemptStateRecovery for ucmId:"
                            << ucmSubApp->GetId().c_str() << ", so try again later.";
            success = false;
        }
    }

    log_.LogDebug() << "FsmManager::CleanUpAllUCMs(), end with success:" << success;
}

/// @brief After vehicle package parsing, this is the software package information to be upgraded in this activity
/// @param spkgInfos List of software package information
/// @throws no
void FsmManager::ReSetSwPackageInfos(AraList< SwPackageInfoPtr > const& spkgInfos)
{
    if (nullptr != spkgTransferManager_) {
        LOG_ERROR << "spkgTransferManager_ is not nullpt, please check code logical!!!";
        return;
    }
    LOG_DEBUG << "create SoftwarePackageManager";
    spkgTransferManager_
        = SoftwarePackageManager::Create(packageStorage_, spkgInfos, blockSize_, retrievedSwpkFQN2TransferIDMap_);
}

/// @brief Get transferring software package information based on software package name fqn
/// @param name Software package name fqn
/// @return Returns smart pointer to software package information on success, null pointer on failure
/// @throws no
SwPackageInfoPtr const FsmManager::GetTransferdSpkgByName(ara::core::String const& name) const
{
    SwPackageInfoPtr swpkgPtr{nullptr};
    if (nullptr == spkgTransferManager_.get()) {
        LOG_ERROR << "can't come here, please check code logical!";
    } else {
        swpkgPtr = spkgTransferManager_->GetTransferdSpkgByName(name);
    }
    return swpkgPtr;
}

/// @brief Get software package information based on software package name fqn
/// @param spkgFqn Software package name fqn
/// @return Returns smart pointer to software package information on success, null pointer on failure
/// @throws no
SwPackageInfoPtr const FsmManager::GetSpkgByName(ara::core::String const& spkgFqn) const
{
    SwPackageInfoPtr swpkgPtr{nullptr};
    if (nullptr == spkgTransferManager_.get()) {
        LOG_ERROR << "can't come here, please check code logical!";
    } else {
        swpkgPtr = spkgTransferManager_->GetSpkgByName(spkgFqn);
    }
    return swpkgPtr;
}

////////////////////////////////////////////////////////
/// @brief alias VDAIApp
using VDAIApp = vdai::VehicleDriverApplication;
/// @brief alias VPMApp
using VPMApp = pkgmgr::VehiclePackageManagement;

/// @brief Get historical activity records within a specified time period
/// @param timestampGE
/// @param timestampLT
/// @return CampaignHistoryVectorType
/// @throws no
pkgmgr::CampaignHistoryVectorType FsmManager::GetCampaignHistory(std::uint64_t const& timestampGE,
                                                                 std::uint64_t const& timestampLT)
{
    LOG_INFO << "timestampGE" << timestampGE << "timestampLT" << timestampLT;

    ara::core::Vector< pkgmgr::CampaignHistoryType > campaignRes{
        HistoryDatabase::GetInstance()->GetCampaignHistory(timestampGE, timestampLT)};
    ara::core::Map< ara::core::String, PackageManagementAppPtr > const ucmSubApps{
        FindUcmSubs::GetInstance()->GetAllPmApp()};

    for (pkgmgr::CampaignHistoryType& it : campaignRes) {
        /// Iterate to find ucm
        for (auto const& pair : ucmSubApps) {
            PackageManagementAppPtr const& pmApp{pair.second};
            ara::core::String const id{pmApp->GetId()};

            pkgmgr::GetHistoryVectorType const historyVec{
                pmApp->GetHistory(it.campaignResult.campaignStartTime, it.campaignResult.campaignResolutionTime)};

            it.historyVector.push_back({id, {id, historyVec}});
            LOG_DEBUG << "get a CampaignHistory record:" << helper::CampaignHistoryTypeToString(it).data();
        }
    }
    return campaignRes;
}

/// @brief Get overall software package transfer progress (VUCM -> UCM)
/// @return Returns progress 0-100 or a specific error code
/// @throws no
ara::core::Future< VDAIApp::GetSwTransferProgressOutput > FsmManager::GetSwTransferProgress() const
{
    ara::core::Promise< VDAIApp::GetSwTransferProgressOutput > promise;
    ara::core::Future< VDAIApp::GetSwTransferProgressOutput > future{promise.get_future()};
    LOG_INFO << "call";
    LOG_DEBUG << "transfer progress=" << GetTransferProgress();
    promise.set_value(VDAIApp::GetSwTransferProgressOutput{GetTransferProgress()});
    return future;
}

/// @brief Get overall software package installation progress
/// @return Returns progress 0-100 or a specific error code
/// @throws no
ara::core::Future< VDAIApp::GetSwProcessProgressOutput > FsmManager::GetSwProcessProgress() const
{
    ara::core::Promise< VDAIApp::GetSwProcessProgressOutput > promise;
    ara::core::Future< VDAIApp::GetSwProcessProgressOutput > future{promise.get_future()};
    LOG_INFO << "call";
    LOG_DEBUG << "process progress=" << GetProcessProgress();
    promise.set_value(VDAIApp::GetSwProcessProgressOutput{GetProcessProgress()});
    return future;
}

/// @brief List of general information of software clusters existing in the adaptive platform
/// @return Returns a Future of the general software cluster information list or a specific error code
/// @throws no
ara::core::Future< VDAIApp::GetSwClusterDescriptionOutput > FsmManager::GetSwClusterDescription() const
{
    // SwClusterInfoVectorType
    ara::core::Promise< VDAIApp::GetSwClusterDescriptionOutput > promise;
    ara::core::Future< VDAIApp::GetSwClusterDescriptionOutput > future{promise.get_future()};

    log_.LogDebug() << "FsmManager::GetSwClusterDescription call";
    VDAIApp::GetSwClusterDescriptionOutput out;
    for (auto const& pair : FindUcmSubs::GetInstance()->GetAllPmApp()) {
        PackageManagementAppPtr const& pmApp{pair.second};
        pkgmgr::SwDescVectorType swDesc{pmApp->GetSwClusterDescription()};
        std::ignore = out.SoftwareClusterDescriptions.insert(std::move(out.SoftwareClusterDescriptions.end()),
                                                             swDesc.begin(), swDesc.end());
    }
    promise.set_value(out);
    return future;
}

/// @brief Get the list of software clusters in the kPresent state on the platform.
/// @return Returns a future of a software cluster list or a specific error code
/// @throws no
ara::core::Future< VPMApp::GetSwClusterInfoOutput > FsmManager::GetSwClusterInfo()
{
    log_.LogDebug() << "FsmManager::GetSwClusterInfo(), begin.";

    GetSwInfoCBType const getSwInfoCB{
        [this](ara::core::Promise< VPMApp::GetSwClusterInfoOutput >&& promise) -> int32_t {
            CleanUpAllUCMs();

            VPMApp::GetSwClusterInfoOutput out;
            for (auto const& pair : FindUcmSubs::GetInstance()->GetAllPmApp()) {
                PackageManagementAppPtr const& pmApp{pair.second};
                pkgmgr::SwClusterInfoVectorType swInfo{pmApp->GetSwClusterInfo()};
                std::ignore = out.SwInfo.insert(out.SwInfo.end(), swInfo.begin(), swInfo.end());
            }
            promise.set_value(out);

            std::ignore = TaskQueue::GetInstance()->PushTask(
                [this]() -> int32_t { return TriggerIdle(pkgmgr::CampaignStateType::kSyncing); });
            return 0;
        }};

    StateAccessor const accessor{state_->Get()};
    // Will call CampaignStateIdle::GetSwClusterInfo, switch to SYNCING state
    return accessor->GetSwClusterInfo(accessor, getSwInfoCB);
}

/// @brief List of general information of software packages that are part of the current active activity processed by VUCM
/// @return Returns a Future of the general software package information list or a specific error code
/// @throws no
ara::core::Future< VDAIApp::GetSwPackageDescriptionOutput > FsmManager::GetSwPackageDescription() const
{
    ///   ara::ucm::pkgmgr::SwNameType Name;
    ///   ara::ucm::pkgmgr::StrongRevisionLabelString Version;
    ///   String TypeApproval;  This attribute contains country-specific certification information
    ///   String License;
    ///   String ReleaseNotes;
    ///   std::uint64_t Size;
    ara::core::Promise< VDAIApp::GetSwPackageDescriptionOutput > promise;
    ara::core::Future< VDAIApp::GetSwPackageDescriptionOutput > future{promise.get_future()};
    LOG_INFO << "call";
    VDAIApp::GetSwPackageDescriptionOutput out{};
    if (nullptr != spkgTransferManager_.get()) {
        for (auto const& pair : spkgTransferManager_->GetAllSpkgInfos()) {
            SwPackageInfoPtr const& ptr{pair.second};
            out.Packages.push_back({ptr->shortName, ptr->version, ptr->typeApproval, ptr->license, ptr->releaseNotes,
                                    ptr->compressedSize});
        }
    }
    promise.set_value(out);
    return future;
}

/// @brief Get software package information that is part of the current active activity processed by VUCM
/// @return Returns a future of a software package information list or a specific error code
/// @throws no
ara::core::Future< VPMApp::GetSwPackagesOutput > FsmManager::GetSwPackages() const
{
    ///ara::ucm::pkgmgr::SwNameType Name;
    ///ara::ucm::pkgmgr::StrongRevisionLabelString Version;
    ///ara::ucm::pkgmgr::TransferIdType TransferID;
    ///std::uint64_t ConsecutiveBytesReceived;
    ///std::uint64_t ConsecutiveBlocksReceived;
    ///ara::ucm::pkgmgr::SwPackageStateType State;
    ara::core::Promise< VPMApp::GetSwPackagesOutput > promise;
    ara::core::Future< VPMApp::GetSwPackagesOutput > future{promise.get_future()};
    LOG_INFO << "call";
    VPMApp::GetSwPackagesOutput out{};
    if (nullptr != spkgTransferManager_.get()) {
        for (auto const& pair : spkgTransferManager_->GetAllSpkgInfos()) {
            SwPackageInfoPtr const& ptr{pair.second};
            if (ptr->consecutiveBytesReceived.load() > 0U) {
                out.Packages.push_back({ptr->shortName, ptr->version, ptr->transferId,
                                        ptr->consecutiveBytesReceived.load(), ptr->consecutiveBlocksReceived.load(),
                                        ptr->state});
            }
        }
    }
    promise.set_value(out);
    return future;
}

///////////////////////////////////////////////////////////////////////////////////
/// @brief Allow the update activity to proceed
/// @param promise promise
/// @return 0: execution successful, non-zero: error code
/// @throws no
int32_t FsmManager::AllowCampaign(ara::core::Promise< void >&& promise)
{
    log_.LogDebug() << "FsmManager::AllowCampaign(), begin.";
    {
        StateAccessor const accessor{state_->Get()};
        pkgmgr::CampaignStateType const currentState{accessor->GetCampaignState()};
        if (pkgmgr::CampaignStateType::kIdle != currentState) {
            LOG_WARN << "CampaignState=" << CampaignStateTypeToString(currentState).data()
                     << " call AllowCampaign isn't inappropriate";
        }
    }

    allowCampaign_ = true;

    /// Persist
    pkgmgr::CampaignStateType nowState{state_->Get()->GetCampaignState()};
    _saveStatus(nowState, state_->Get()->GetCancelCause());

    promise.set_value();
    log_.LogDebug() << "FsmManager::AllowCampaign(), end.";
    return 0;
}

/// @brief Cancel the current update activity
/// @param disableCampaign  true: Cancel the upgrade activity and reject subsequent upgrade activity requests until AllowCampaign is called. false:
/// Only cancel this upgrade activity, does not affect subsequent ones
/// @param promise
/// @return
/// @throws no
int32_t FsmManager::CancelCampaign(bool const& disableCampaign, ara::core::Promise< void >&& promise)
{
    log_.LogDebug() << "FsmManager::CancelCampaign(), begin with disableCampaign:" << disableCampaign;

    allowCampaign_ = !disableCampaign;

    /// Persist
    pkgmgr::CampaignStateType nowState{state_->Get()->GetCampaignState()};
    _saveStatus(nowState, state_->Get()->GetCancelCause());

    int32_t const ret{TriggerCancel(pkgmgr::UCMMasterResolutionType::kCampaignCancelled, std::move(promise))};
    log_.LogDebug() << "FsmManager::CancelCampaign(), end with ret:" << ret;
    return ret;
}

/// @brief Transfer vehicle package
/// @param size Size of the vehicle package compressed package, in bytes
/// @return   Returns a future of a vehicle package transfer information handle or a specific error code
/// @throws no
ara::core::Future< TransferVehiclePackageOutput > FsmManager::TransferVehiclePackage(std::uint64_t const& size)
{
    if (!allowCampaign_.load()) {
        log_.LogDebug() << "FsmManager::TransferVehiclePackage(), already disable campaign.";
        ara::core::Promise< TransferVehiclePackageOutput > promise;
        promise.SetError(pkgmgr::UCMErrorDomainErrc::kNewCampaignDisabled);
        return promise.get_future();
    }

    // Use callback method to avoid forward declaration
    VpkgCampaignCBType const vpkgCB{
        [this](std::uint64_t const& s, ara::core::Promise< TransferVehiclePackageOutput >&& promise) -> int32_t {
            LOG_INFO << "begin transfer vpkg";
            hasAborted_ = false;
            int32_t const ret{TriggerVpkgTransfer(s, std::move(promise))};
            if (0 == ret) {
                _saveStatus(pkgmgr::CampaignStateType::kVehiclePackageTransferring,
                            pkgmgr::UCMMasterResolutionType::kSuccess);
            }
            return ret;
        }};

    LOG_INFO << "call";
    StateAccessor const accessor{state_->Get()};
    // Will call CampaignStateIdle::TransferVehiclePackage, transition to VEHICLEPACKAGE_TRANSFERRING state
    return accessor->TransferVehiclePackage(accessor, size, vpkgCB);
}

/// @brief Transfer software package
/// @param stSoftwarePackageName Software package name, version
/// @return   Returns a software package transfer information or a specific error code
/// @throws no
ara::core::Future< TransferStartOutput > FsmManager::TransferStart(
    pkgmgr::SwNameType const& stSoftwarePackageName) const
{
    log_.LogDebug() << "FsmManager::TransferStart(), begin with stSoftwarePackageName:"
                    << stSoftwarePackageName.c_str();

    ara::core::Promise< TransferStartOutput > promise;
    ara::core::Future< TransferStartOutput > future{promise.get_future()};

    StateAccessor const accessor{state_->Get()};  // Lock state transition
    pkgmgr::CampaignStateType const curState{accessor->GetCampaignState()};
    log_.LogDebug() << "FsmManager::TransferStart(), get curState:" << CampaignStateTypeToString(curState).data();

    if ((curState == pkgmgr::CampaignStateType::kSoftwarePackage_Transferring)
        || (curState == pkgmgr::CampaignStateType::kProcessing)) {
        assert(spkgTransferManager_);
        int32_t const ret{spkgTransferManager_->TransferStart(stSoftwarePackageName, std::move(promise))};
        if (0 == ret) {
            _saveStatus(curState, pkgmgr::UCMMasterResolutionType::kSuccess);
        }
    } else {
        log_.LogError() << "FsmManager::TransferData(), can't TransferStart in curState:"
                        << CampaignStateTypeToString(curState).data();
        promise.SetError(pkgmgr::UCMErrorDomainErrc::kOperationNotPermitted);
    }

    log_.LogDebug() << "FsmManager::TransferStart(), end for stSoftwarePackageName:" << stSoftwarePackageName.c_str();
    return future;
}

/// @brief Transfer a block of software package or vehicle package data
/// @param id Transfer id
/// @param data Transfer data block
/// @param blockCounter Block counter value of the current block
/// @return
/// @throws no
ara::core::Future< void > FsmManager::TransferData(pkgmgr::TransferIdType const& id,
                                                   pkgmgr::ByteVectorType const& data,
                                                   std::uint64_t const& blockCounter) const
{
    log_.LogDebug() << "FsmManager::TransferData(), begin with id:" << pkgmgr::helper::ToHexString(id).c_str()
                    << "blockCounter:" << blockCounter;

    ara::core::Promise< void > promise;
    ara::core::Future< void > future{promise.get_future()};

    StateAccessor const accessor{state_->Get()};  // Lock state transition
    pkgmgr::CampaignStateType const curState{accessor->GetCampaignState()};
    log_.LogDebug() << "FsmManager::TransferData(), get curState:" << CampaignStateTypeToString(curState).data();

    if (pkgmgr::CampaignStateType::kVehiclePackageTransferring == curState) {
        assert(vpkgTransfer_);
        std::ignore = vpkgTransfer_->TransferData(id, data, blockCounter, std::move(promise));
    } else if ((pkgmgr::CampaignStateType::kSoftwarePackage_Transferring == curState)
               || (pkgmgr::CampaignStateType::kProcessing == curState)) {
        assert(spkgTransferManager_);
        std::ignore = spkgTransferManager_->TransferData(id, data, blockCounter, std::move(promise));
    } else {
        log_.LogError() << "FsmManager::TransferData(), can't transferData in curState:"
                        << CampaignStateTypeToString(curState).data();
        promise.SetError(pkgmgr::UCMErrorDomainErrc::kOperationNotPermitted);
    }

    log_.LogDebug() << "FsmManager::TransferData(), end for id:" << pkgmgr::helper::ToHexString(id).c_str()
                    << "blockCounter:" << blockCounter;
    return future;
}

/// @brief Transfer of software package or vehicle package (OTA->VUCM) to VUCM is complete.
/// @param id Transfer handle
/// @return Returns a void future or a specific error code
/// @throws no
/// return error: AuthenticationFailed
/// return error: IncompatiblePackageVersion
/// return error: InsufficientData
/// return error: InvalidPackageManifest
/// return error: InvalidTransferId
/// return error: MissingDependencies
/// return error: OldVersion
/// return error: OperationNotPermitted
/// return error: PackageInconsistent
ara::core::Future< void > FsmManager::TransferExit(pkgmgr::TransferIdType const& id) const
{
    log_.LogDebug() << "FsmManager::TransferExit(), begin with id:" << pkgmgr::helper::ToHexString(id).c_str();

    ara::core::Promise< void > promise;
    ara::core::Future< void > future{promise.get_future()};

    StateAccessor const accessor{state_->Get()};  // Lock state transition
    pkgmgr::CampaignStateType const curState{accessor->GetCampaignState()};
    log_.LogDebug() << "FsmManager::TransferExit(), get curState:" << CampaignStateTypeToString(curState).data();

    if (pkgmgr::CampaignStateType::kVehiclePackageTransferring == curState) {
        assert(vpkgTransfer_);
        std::ignore = vpkgTransfer_->TransferExit(id, std::move(promise));
    } else if ((pkgmgr::CampaignStateType::kSoftwarePackage_Transferring == curState)
               || (pkgmgr::CampaignStateType::kProcessing == curState)) {
        assert(spkgTransferManager_);
        std::ignore = spkgTransferManager_->TransferExit(id, std::move(promise));
    } else {
        log_.LogError() << "FsmManager::TransferExit(), can't TransferExit in curState:"
                        << CampaignStateTypeToString(curState).data();
        promise.SetError(pkgmgr::UCMErrorDomainErrc::kOperationNotPermitted);
    }

    log_.LogDebug() << "FsmManager::TransferExit(), end for id:" << pkgmgr::helper::ToHexString(id).c_str();
    return future;
}

/// @brief Delete the vehicle package or software package with the specified id
/// @param id Transfer id
/// @return Returns void, or a specific error code
/// @throws no
ara::core::Future< void > FsmManager::DeleteTransfer(pkgmgr::TransferIdType const& id)
{
    log_.LogDebug() << "FsmManager::DeleteTransfer(), begin with id:" << pkgmgr::helper::ToHexString(id).c_str();

    ara::core::Promise< void > promise;
    ara::core::Future< void > future{promise.get_future()};

    // Lock state transition, does it conflict with FsmManager::TriggerCancel? conflict
    StateAccessor const accessor{state_->Get()};
    pkgmgr::CampaignStateType const curState{accessor->GetCampaignState()};
    log_.LogDebug() << "FsmManager::DeleteTransfer(), get curState:" << CampaignStateTypeToString(curState).data();

    if (pkgmgr::CampaignStateType::kVehiclePackageTransferring == curState) {
        if (vpkgTransfer_->GetVPKTransferID() == id) {
            // Cancel Campaign
            std::shared_ptr< ara::core::Promise< void > > cancelPromise{
                std::make_shared< ara::core::Promise< void > >(std::move(promise))};
            TriggerCancelWithAccessor(accessor, pkgmgr::UCMMasterResolutionType::kCampaignCancelled,
                                      std::move(*(cancelPromise.get())));
        } else {
            log_.LogError() << "FsmManager::DeleteTransfer(), vpkgTransfer_ is null.";
            promise.SetError(pkgmgr::UCMErrorDomainErrc::kInvalidTransferId);
        }
    } else if ((pkgmgr::CampaignStateType::kSoftwarePackage_Transferring == curState)
               || (pkgmgr::CampaignStateType::kProcessing == curState)) {
        promise.SetError(pkgmgr::UCMErrorDomainErrc::kOperationNotPermitted);
    } else {
        if (spkgTransferManager_) {
            spkgTransferManager_->DeleteTransfer(id, std::move(promise));
        } else {
            log_.LogError() << "FsmManager::DeleteTransfer(), spkgTransferManager_ is null.";
            promise.SetError(pkgmgr::UCMErrorDomainErrc::kOperationNotPermitted);
        }
    }

    return future;
}

/// @brief Delete the software package with the specified id
/// @param id Transfer id
/// @return Returns void, or a specific error code
/// @throws no
bool FsmManager::CleanSWPackage(pkgmgr::TransferIdType const& id) const
{
    log_.LogDebug() << "FsmManager::CleanSWPackage(), begin with id:" << pkgmgr::helper::ToHexString(id).c_str();

    bool ret{false};
    if (spkgTransferManager_) {
        ara::core::Promise< void > promise;
        ara::core::Future< void > future{promise.get_future()};
        spkgTransferManager_->DeleteTransfer(id, std::move(promise));
        ret = future.GetResult().HasValue();
    }

    log_.LogDebug() << "FsmManager::CleanSWPackage(), end with ret:" << ret
                    << "for id:" << pkgmgr::helper::ToHexString(id).c_str();
    return ret;
}

/// @brief Trigger synchronization of software cluster information (kIdle->kSync->kIdle)
/// @param newSwpkgs Newly published software cluster information
/// @return Software cluster information to be upgraded or a specific error code
/// @throws no
ara::core::Future< SwPackageInventoryOutput > FsmManager::TriggerSync(
    pkgmgr::SwNameVersionVectorType const& newSwpkgs) const
{
    SyncSwInfoCBType const syncSwInfoCB{[](pkgmgr::SwNameVersionVectorType const& availableSoftwarePackages,
                                           ara::core::Promise< SwPackageInventoryOutput >&& promise) {
        std::shared_ptr< SyncExecutor > const syncExecutor{
            std::make_shared< SyncExecutor >(availableSoftwarePackages, std::move(promise))};
        TaskFuncType executor{[syncExecutor]() -> int32_t {
            LOG_INFO << "sync swinfo begin";
            std::ignore = syncExecutor->Executor();
            LOG_INFO << "sync swinfo end";
            return 0;
        }};
        std::ignore = TaskQueue::GetInstance()->PushTask(std::move(executor));
    }};

    // This function cannot be blocking, otherwise accessor will hold state_ for a long time, other threads will be blocked from accessing, or deadlock
    LOG_INFO << "call";
    StateAccessor const accessor{state_->Get()};
    // Call CampaignStateIdle::SwPackageInventory, will switch to SYNCING state
    return accessor->SwPackageInventory(accessor, newSwpkgs, syncSwInfoCB);
}

/// @brief Trigger vehicle package transfer
/// @param size Vehicle package size
/// @param promise
/// @return 0 on success, non-zero on failure
/// @throws no
int32_t FsmManager::TriggerVpkgTransfer(std::uint64_t const& size,
                                        ara::core::Promise< TransferVehiclePackageOutput >&& promise)
{
    LOG_INFO << "call";
    HistoryDatabase::GetInstance()->SaveCampaignStartTime(std::move(ara::core::StringView(campaignHistoryKey_)),
                                                          Utils::GetTime());
    // Vehicle package transfer handling
    vpkgTransfer_ = VehiclePackageTransfer::Create(packageStorage_, blockSize_, pkgmgr::TransferIdType{});
    return vpkgTransfer_->TransferVehiclePackage(size, std::move(promise));
}

/// @brief Trigger software package transfer
/// @return 0 on success, non-zero on failure
/// @throws no
int32_t FsmManager::TriggerSoftpkgTransfer()
{
    Concentrator* const concentrator{Concentrator::GetInstance()};
    /// Request driver approval
    if (false == concentrator->TransferApproval()) {
        log_.LogDebug() << "FsmManager::TriggerSoftpkgTransfer(), failed to TransferApproval.";
        MarkProcessingVPFinish();
        /// Not approved, end this upgrade
        return 1;
    }

    FsmManager::GetInstance()->MarkProcessingVPFinish();

    /// Prepare resources for software package transfer, such as notifying ota of the software package name to transfer
    SpkgCampaignCBType const spkgCB{[this]() {
        log_.LogDebug() << "FsmManager::TriggerSoftpkgTransfer(), try to taskQueue->PushTask";
        std::ignore = TaskQueue::GetInstance()->PushTask([this]() -> int32_t {
            std::ignore = this->_executeProcessStep();
            return 0;
        });
        return 0;
    }};
    LOG_INFO << "call";
    /// Normally current state is VehiclePackage_Transfering // Transition to SOFTWAREPACKAGE_TRANSFERRING state
    StateAccessor const accessor{state_->Get()};
    return accessor->DoSoftwareTransfer(accessor, spkgCB);
}

/// @brief Trigger software cluster installation status
/// @return
/// @throws no
bool FsmManager::TriggerProcess()
{
    log_.LogDebug() << "FsmManager::TriggerProcess(), begin.";

    if (false == Concentrator::GetInstance()->ProcessApproval()) {
        log_.LogDebug() << "FsmManager::TriggerProcess(), failed to ProcessApproval.";
        // MarkTransferringFinish(); // Actually still transferring
        /// Not approved
        return false;
    }
    MarkTransferringFinish();

    ProcessCBType const processCB{[this]() noexcept -> int32_t {
        MarkProcessingStart();
        return 0;
    }};

    /// Current state is kSoftwarePackage_Transferring, here only to switch state
    StateAccessor const accessor{state_->Get()};  // Lock state transition
    int32_t const ret{accessor->DoProcess(
        accessor, processCB)};  // Will call CampaignStateSoftpkgTransfer::DoProcess to switch to PROCESSING state.
    if (0 == ret) {
        log_.LogDebug() << "FsmManager::TriggerProcess(), end with success.";
        return true;
    }
    log_.LogDebug() << "FsmManager::TriggerProcess(), end with failure.";
    return false;
}

/// @brief Check if it can process
/// @return bool
/// @throws no
bool FsmManager::CanProcess()
{
    log_.LogDebug() << "FsmManager::CanProcess(), begin with hasTriggerredProcess_:" << hasTriggerredProcess_
                    << "triggerredProcessSuccess_:" << triggerredProcessSuccess_;

    std::lock_guard< std::mutex > const lock{hasTriggerredProcessMutex_};
    if (!hasTriggerredProcess_) {  // Processing not yet triggered
        if (TriggerProcess()) {    // Result
            log_.LogDebug() << "FsmManager::CanProcess(), succeeded to TriggerProcess.";
            triggerredProcessSuccess_ = true;
        } else {
            log_.LogDebug() << "FsmManager::CanProcess(), failed to TriggerProcess.";
            triggerredProcessSuccess_ = false;
        }

        hasTriggerredProcess_ = true;  // Processing triggered
    }

    log_.LogDebug() << "FsmManager::CanProcess(), end with triggerredProcessSuccess_:" << triggerredProcessSuccess_;
    return triggerredProcessSuccess_;
}

/// @brief Trigger activation (after software package transfer and installation steps in this activity are completed)
/// @return 0 on success, non-zero on failure
/// @throws no
int32_t FsmManager::TriggerActivate()
{
    ActivateCBType const activateCB{[this]() -> int32_t {
        MarkActivatingStart();
        std::ignore = TaskQueue::GetInstance()->PushTask([this]() -> int32_t { return this->_executeActivateStep(); });
        return 0;
    }};
    LOG_INFO << "call";
    StateAccessor const accessor{state_->Get()};
    // Execute CampaignStateProcessing::DoActivate, will switch to ACTIVATING state
    return accessor->DoActivate(accessor, activateCB);
}

/// @brief Trigger vehicle check (OEM specific)
/// @return 0 on success, non-zero on failure
/// @throws no
int32_t FsmManager::TriggerVehicleCheck()
{
    VehicleCheckCBType const vehicleCheckCB{[this]() -> int32_t {
        MarkVehicleCheckPreparingStart();
        std::ignore = TaskQueue::GetInstance()->PushTask([this]() -> int32_t { return this->_executeVehicleCheck(); });
        return 0;
    }};
    LOG_INFO << "call";
    StateAccessor const accessor{state_->Get()};
    /// Normally current state is activate
    std::ignore = accessor->DoVehicleCheck(
        accessor,
        vehicleCheckCB);  // Execute CampaignStateActivating::DoVehicleCheck, will switch to VEHICLE_CHECKING state
    return 0;
}

/// @brief Trigger the upgrade cancellation process. Can be initiated actively, or forced due to operation failure.
/// @note Note!!! There is a scenario: RolloutStep has just finished execution, but before switching state, cancel is triggered.
/// There is a possibility of successful processing->active transition, but immediately after, processing->cancel transition is done in campaign_state_processing.
/// Previously, LockedPtrWrapper was modified to be mutually exclusive during state transitions, but accessor is accessible by multiple threads, this problem exists, leading to unexpected logical anomalies.
/// Therefore, the state transition process must be single-threaded and queued, and the accessor thread-exclusive can solve the problem.
/// @param cancelCause Reason for cancellation
/// @param promise
/// @param delSwpks
/// @return 0 on success, non-zero on failure
/// @throws no
int32_t FsmManager::TriggerCancel(pkgmgr::UCMMasterResolutionType const cancelCause,
                                  ara::core::Promise< void >&& promise,
                                  bool const delSwpks)
{
    core::String padding;
    log_.LogDebug() << "FsmManager::TriggerCancel(), begin with cancelCause:"
                    << com::internal::format::Formatter< core::String, pkgmgr::UCMMasterResolutionType >::ToString(
                           padding, cancelCause)
                           .c_str()
                    << "delSwpks:" << delSwpks;

    CancelCBType const cancelCB{
        [this, delSwpks](std::underlying_type< pkgmgr::UCMMasterResolutionType >::type const cancelCauseResolutionInt,
                         ara::core::Promise< void >&& cancelPromise) -> int32_t {
            auto cancelCauseResolution = static_cast< pkgmgr::UCMMasterResolutionType >(cancelCauseResolutionInt);
            MarkCancellingStart();
            std::shared_ptr< ara::core::Promise< void > > cancPromise{
                std::make_shared< ara::core::Promise< void > >(std::move(cancelPromise))};
            std::ignore
                = TaskQueue::GetInstance()->PushTask([this, delSwpks, cancelCauseResolution, cancPromise]() -> int32_t {
                      return this->_executeCancel(cancelCauseResolution, preState_, std::move(cancPromise));
                  });
            return 0;
        }};

    StateAccessor const accessor{
        state_->Get()};  // Lock state transition    ---During Cancel, the state machine state cannot change
    std::ignore = accessor->OnCancel(accessor, cancelCause, cancelCB, std::move(promise));
    log_.LogDebug() << "FsmManager::TriggerCancel(), end.";
    return 0;
}

int32_t FsmManager::TriggerCancelWithAccessor(StateAccessor const& accessor,
                                              pkgmgr::UCMMasterResolutionType const cancelCause,
                                              ara::core::Promise< void >&& promise,
                                              bool const delSwpks)
{
    core::String padding;
    log_.LogDebug() << "FsmManager::TriggerCancelWithAccessor(), begin with cancelCause:"
                    << com::internal::format::Formatter< core::String, pkgmgr::UCMMasterResolutionType >::ToString(
                           padding, cancelCause)
                    << "delSwpks:" << delSwpks;

    CancelCBType cancelCB = [&](std::underlying_type< pkgmgr::UCMMasterResolutionType >::type cancelCauseResolutionInt,
                                ara::core::Promise< void >&& cancelPromise) -> int32_t {
        auto cancelCauseResolution = static_cast< pkgmgr::UCMMasterResolutionType >(cancelCauseResolutionInt);
        MarkCancellingStart();
        TaskQueue* taskQueue = TaskQueue::GetInstance();
        auto cancPromise     = std::make_shared< ara::core::Promise< void > >(std::move(cancelPromise));
        taskQueue->PushTask([&, delSwpks, cancelCauseResolution, cancPromise]() -> int32_t {
            return this->_executeCancel(cancelCauseResolution, preState_, std::move(cancPromise));
        });
        return 0;
    };

    accessor->OnCancel(accessor, cancelCause, cancelCB, std::move(promise));
    log_.LogDebug() << "FsmManager::TriggerCancelWithAccessor(), end.";
    return 0;
}

/// @brief Trigger activity cancellation
/// @param cancelCause
/// @param delSwpks
/// @return 0 on success, non-zero on failure
/// @throws no
void FsmManager::TriggerCancelInTQ(pkgmgr::UCMMasterResolutionType const cancelCause, bool const delSwpks) const
{
    core::String padding;
    log_.LogDebug() << "FsmManager::TriggerCancelInTQ(), begin with delSwpks:" << delSwpks << "cancelCause:"
                    << com::internal::format::Formatter< core::String, pkgmgr::UCMMasterResolutionType >::ToString(
                           padding, cancelCause)
                           .c_str();

    // Cancel Campaign
    std::ignore = TaskQueue::GetInstance()->PushTask([cancelCause, delSwpks]() -> int32_t {
        ara::core::Promise< void > promise;
        std::ignore = FsmManager::GetInstance()->TriggerCancel(cancelCause, std::move(promise), delSwpks);
        return 0;
    });
    log_.LogDebug() << "FsmManager::TriggerCancelInTQ(), end.";
}

/// @brief Trigger transition to kIdle state, perform related resource cleanup and reset
/// @param fromState
/// @param delSwpks
/// @return int32_t
/// @throws no
int32_t FsmManager::TriggerIdle(pkgmgr::CampaignStateType const& fromState, bool const delSwpks)
{
    log_.LogDebug() << "FsmManager::TriggerIdle(), begin with fromState:" << CampaignStateTypeToString(fromState).data()
                    << "delSwpks:" << delSwpks;

    if (pkgmgr::CampaignStateType::kSyncing != fromState) {
        HistoryDatabase* const hist{HistoryDatabase::GetInstance()};
        std::ignore
            = hist->SaveCampaignResolutionTime(std::move(ara::core::StringView(campaignHistoryKey_)), Utils::GetTime());

        // if (delSwpks) {
        //     if (nullptr != spkgTransferManager_) { // Delete campaign-related software packages and their status information
        //         spkgTransferManager_->ClearSWPKGs();
        //     }
        // }

        // TODO(Han Zhibo): In the future, implement streaming transfer and cleaning of software packages based on SoftwarePackageStoringEnum implementation, then delete this cleanup
        if (nullptr
            != spkgTransferManager_) {  // Delete campaign-related software packages and their status information
            spkgTransferManager_->ClearSWPKGs();
        }

        // If there is a vehicle package
        if (vpkgTransfer_) {
            vpkgTransfer_
                ->ClearVPK();  // Clear the extracted files of the vehicle package, clean up the vehicle package zip file and its status information.
        }

        vpkgTransfer_.reset(nullptr);
        spkgTransferManager_.reset(nullptr);

        Reset();
    }

    StateAccessor const accessor{state_->Get()};  // Lock state transition
    std::ignore = accessor->DoIdle(accessor);
    log_.LogDebug() << "FsmManager::TriggerIdle(), end.";
    return 0;
}

/// @brief Execute installation steps in a background thread
/// @return 0 on success, non-zero on failure
/// @throws no
int32_t FsmManager::_executeProcessStep()
{
    log_.LogDebug() << "FsmManager::_executeProcessStep(), begin.";

    /// First software package transfer, subsequent software package transfers are triggered in TransferStart TransferExit
    spkgTransferManager_->TransferNextSwpkg();

    /// Start executing steps transfer and process
    log_.LogDebug()
        << "FsmManager::_executeProcessStep(), try to Execute RolloutStep sequentially with rolloutSteps_.size():"
        << rolloutSteps_.size();

    // Reset processing trigger flag
    hasTriggerredProcess_     = false;
    triggerredProcessSuccess_ = false;
    bool res{true};
    for (RolloutStep::Ptr const& step : rolloutSteps_) {
        log_.LogDebug() << "FsmManager::_executeProcessStep(), try to step->Execute() for RolloutStep:"
                        << step->GetShortName().c_str();
        ara::core::Result< void > const result{step->Execute()};
        if (!result.HasValue()) {
            log_.LogError() << "FsmManager::_executeProcessStep(), failed to step->Execute() for RolloutStep:"
                            << step->GetShortName().c_str() << "with error:" << result.Error().Message().data()
                            << ",so will break.";
            res = false;
            break;
        }
        log_.LogDebug() << "FsmManager::_executeProcessStep(), succeeded to step->Execute() for RolloutStep:"
                        << step->GetShortName().c_str();
    }
    log_.LogDebug() << "FsmManager::_executeProcessStep(), rollout end with res:" << res;

    if (res) {
        /// Request driver approval for activation
        Concentrator* const concentrator{Concentrator::GetInstance()};
        if (!concentrator->ActivateApproval()) {
            log_.LogError() << "FsmManager::_executeProcessStep(), failed to ActivateApproval.";
            MarkTransferringFinish();
            MarkProcessingFinish();
        } else {
            MarkTransferringFinish();
            MarkProcessingFinish();
            std::ignore = TriggerActivate();
        }
    } else {
        MarkTransferringFinish();
        MarkProcessingFinish();
    }

    log_.LogDebug() << "FsmManager::_executeProcessStep(), end.";
    return 0;
}

/// @brief Execute activation steps in a background thread
/// @return 0 on success, non-zero on failure
/// @throws no
int32_t FsmManager::_executeActivateStep()
{
    log_.LogDebug() << "FsmManager::_executeActivateStep(), begin.";

    // Whether successful
    bool success{true};

    // Execute activation steps sequentially
    log_.LogDebug()
        << "FsmManager::_executeActivateStep(), try to Activate RolloutStep sequentially with rolloutSteps_.size():"
        << rolloutSteps_.size();
    for (RolloutStep::Ptr const& it : rolloutSteps_) {
        log_.LogDebug() << "FsmManager::_executeActivateStep(), try to it->Activate() for RolloutStep:"
                        << it->GetShortName().c_str();
        ara::core::Result< void > const result{it->Activate()};
        if (!result.HasValue()) {
            success = false;
            log_.LogDebug() << "FsmManager::_executeActivateStep(), haven't finished Activate for RolloutStep:"
                            << it->GetShortName().c_str();
            break;
        }
        log_.LogDebug() << "FsmManager::_executeActivateStep(), succeeded to it->Activate() for RolloutStep:"
                        << it->GetShortName().c_str();
    }

    MarkActivatingFinish();  // Mark the end of FsmManager::ExecuteActivateStep
    if (success) {
        log_.LogDebug() << "FsmManager::_executeActivateStep(), end with success.";
        std::ignore = TriggerVehicleCheck();
        return 0;
    }
    log_.LogDebug() << "FsmManager::_executeActivateStep(), end with failure.";
    return 1;
}

/// @brief Execute rollback steps in a background thread
/// @param cancelCause
/// @param fromState
/// @param promise
/// @param delSwpks
/// @return 0 on success, non-zero on failure
/// @throws no
int32_t FsmManager::_executeCancel(pkgmgr::UCMMasterResolutionType const cancelCause,
                                   pkgmgr::CampaignStateType const fromState,
                                   std::shared_ptr< ara::core::Promise< void > > const& promise,
                                   bool const delSwpks)
{
    core::String padding;
    log_.LogDebug() << "FsmManager::_executeCancel(), begin with cancelCause:"
                    << com::internal::format::Formatter< core::String, pkgmgr::UCMMasterResolutionType >::ToString(
                           padding, cancelCause)
                           .c_str()
                    << "fromState:" << CampaignStateTypeToString(fromState).data() << "delSwpks:" << delSwpks;

    bool success{true};
    if ((pkgmgr::CampaignStateType::kVehiclePackageTransferring == fromState)
        || (pkgmgr::CampaignStateType::kSoftwarePackage_Transferring == fromState)) {
        log_.LogDebug() << "FsmManager::_executeCancel(), need to do nothing for fromState:"
                        << CampaignStateTypeToString(fromState).data();
        success = true;
    } else if ((pkgmgr::CampaignStateType::kProcessing == fromState)
               || (pkgmgr::CampaignStateType::kActivating == fromState)
               || (pkgmgr::CampaignStateType::kVehicleChecking == fromState)) {
        // List of ucmIDs involved in the vehicle package
        AraList< ara::core::String > const ucmIDs{vpkgTransfer_->GetUcmIds()};
        log_.LogDebug() << "FsmManager::_executeCancel(), get ucmIDs:" << Utils::ConcatenateStrings(ucmIDs).c_str();

        // Whether can continue
        bool canContinue{true};

        // Get service proxy corresponding to ucmID
        FindUcmSubs* const subs{FindUcmSubs::GetInstance()};
        ara::core::Map< ara::core::String, PackageManagementAppPtr > ucmSubApps;
        while (true) {
            canContinue = CanWorkAsMaster();
            if (!canContinue) {
                break;
            }
            if (subs->GetVpkgPmApp(ucmIDs, ucmSubApps)) {
                break;
            }
            log_.LogDebug() << "FsmManager::_executeCancel(), waitting for all ucms.";
            std::this_thread::sleep_for(std::chrono::milliseconds(kGlobalWaitTime));
        }

        if (canContinue) {  // Can continue
            for (auto const& pair : ucmSubApps) {
                PackageManagementAppPtr const& ucmSubApp{pair.second};
                log_.LogDebug() << "FsmManager::_executeCancel(), try to AttemptStateRecovery for ucmId:"
                                << ucmSubApp->GetId().c_str();
                if (!ucmSubApp->AttemptStateRecovery()) {  // Retry strategy is inside
                    log_.LogDebug() << "FsmManager::_executeCancel(), haven't finished AttemptStateRecovery for ucmId:"
                                    << ucmSubApp->GetId().c_str() << ", so try again later.";
                    success = false;
                    break;
                }
            }
        } else {
            log_.LogError() << "FsmManager::_executeCancel(), can't continue.";
            success = false;
        }
    } else {
        log_.LogError() << "FsmManager::_executeCancel(), can't cancel for fromState:"
                        << CampaignStateTypeToString(fromState).data();
        success = false;
    }

    // Mark the end of FsmManager::ExecuteCancel
    MarkCancellingFinish();

    // If all above AttemptStateRecovery are completed
    if (success) {
        log_.LogInfo() << "FsmManager::_executeCancel(), succeed to do all AttemptStateRecovery.";

        // Should set UCMMasterResolutionType based on passed parameter, must save Resolution at the end of ExecuteCancel, otherwise after ucmm upgrade failure and rollback, there will be no Campaign history data.
        std::ignore = SaveResolution(cancelCause);  // Save Resolution as error
        std::ignore = TriggerIdle(pkgmgr::CampaignStateType::kCancelling, delSwpks);
        promise->set_value();
        return 0;
    }
    // Failure occurred
    log_.LogDebug() << "FsmManager::_executeCancel(), haven't finished all AttemptStateRecovery.";
    promise->SetError(pkgmgr::UCMErrorDomainErrc::kGeneralReject);
    return 1;
}

/// @brief Planned addition
/// @return bool
/// @throws no
bool FsmManager::CanWorkAsMaster()
{
    log_.LogDebug() << "FsmManager::CanWorkAsMaster(), begin with terminate_:" << terminate_
                    << "hasAborted_:" << hasAborted_ << "lastHeatBeatTime_:" << lastHeatBeatTime_;
    bool ret{true};

    // Whether the program is terminating
    if (terminate_.load()) {
        ret = false;
    } else if (lastHeatBeatTime_ > 0U) {  // Campaign has started
        std::int64_t const now{
            std::chrono::duration_cast< std::chrono::milliseconds >(std::chrono::system_clock::now().time_since_epoch())
                .count()};
        log_.LogDebug() << "FsmManager::CanWorkAsMaster(), get now:" << now;
        if ((static_cast< std::uint64_t >(now) - lastHeatBeatTime_) < kMaxDurationOfUnconnect) {
            ret = true;
        } else {
            std::lock_guard< std::mutex > const lock{abortMutex_};
            if (!hasAborted_) {  // Not yet exited
                std::ignore = TaskQueue::GetInstance()->PushTask([this]() -> int32_t {
                    this->Abort();
                    return 0;
                });
            }
            ret = false;
        }
    } else {
        ;
    }

    log_.LogDebug() << "FsmManager::CanWorkAsMaster(), end with ret:" << ret;
    return ret;
}

/// @brief Check if can continue moving forward
/// @return bool
/// @throws no
bool FsmManager::CanMoveForward()
{
    log_.LogDebug() << "FsmManager::CanMoveForward(), begin.";

    bool ret{false};

    if (!campaignCancelFlag_.load()) {
        if (CanWorkAsMaster()) {
            ret = true;
        }
    }

    log_.LogDebug() << "FsmManager::CanMoveForward(), end with ret:" << ret;
    return ret;
}

/// @brief Execute VehicleCheck (OEM specific) steps in a background thread
/// @return 0 on success, non-zero on failure
/// @throws no
int32_t FsmManager::_executeVehicleCheck()
{
    log_.LogDebug() << "FsmManager::_executeVehicleCheck(), begin.";

    // Whether can continue
    bool const canContinue{CanMoveForward()};
    if (canContinue) {
        // Upgrade verification check, OEM specific
    }

    // Mark the end of preparation for FsmManager::ExecuteVehicleCheck
    MarkVehicleCheckPreparingFinish();

    if (!canContinue) {  // Cannot continue || OEM's VehicleCheck failed
        log_.LogError() << "FsmManager::_executeVehicleCheck(), just return because of canContinue:" << canContinue;
        return 1;
    }

    {
        log_.LogDebug() << "FsmManager::_executeVehicleCheck(), try to state_->Get().";
        StateAccessor const accessor{state_->Get()};  // Lock state transition
        pkgmgr::CampaignStateType const campaignState{accessor->GetCampaignState()};
        log_.LogDebug() << "FsmManager::_executeVehicleCheck(), get campaignState:"
                        << CampaignStateTypeToString(campaignState).data();
        if (pkgmgr::CampaignStateType::kVehicleChecking == campaignState) {  // Still in VEHICLE_CHECKING state
            beginFinish_ = true;

            // Update persistent state
            _saveStatus(pkgmgr::CampaignStateType::kVehicleChecking, pkgmgr::UCMMasterResolutionType::kSuccess);

            MarkVehicleCheckFinishingStart();
        } else {
            log_.LogError()
                << "FsmManager::_executeVehicleCheck(), just return because of not in kVehicleChecking state.";
            return 1;
        }
    }

    // List of ucmIDs involved in the vehicle package
    AraList< ara::core::String > const ucmIDs{vpkgTransfer_->GetUcmIds()};
    log_.LogDebug() << "FsmManager::_executeVehicleCheck(), get ucmIDs:" << Utils::ConcatenateStrings(ucmIDs).c_str();

    // Get service proxy corresponding to ucmID
    FindUcmSubs* const subs{FindUcmSubs::GetInstance()};
    ara::core::Map< ara::core::String, PackageManagementAppPtr > ucmSubApps;
    while (true) {
        if (!CanWorkAsMaster()) {
            break;
        }
        if (subs->GetVpkgPmApp(ucmIDs, ucmSubApps)) {
            break;
        }
        log_.LogDebug() << "FsmManager::_executeVehicleCheck(), waitting for all ucms.";
        std::this_thread::sleep_for(std::chrono::milliseconds(kGlobalWaitTime));
    }

    // Call Finish
    bool success{true};
    log_.LogDebug() << "FsmManager::_executeVehicleCheck(), try to Finish sequentially with ucmSubApps.size():"
                    << ucmSubApps.size();
    for (auto const& pair : ucmSubApps) {
        PackageManagementAppPtr const& ucmSubApp{pair.second};
        log_.LogDebug() << "FsmManager::_executeVehicleCheck(), try to ucmSubApp->Finish() for ucmSubApp->GetId():"
                        << ucmSubApp->GetId().c_str();
        if (!ucmSubApp->Finish()) {  // Retry strategy is inside
            log_.LogError() << "FsmManager::_executeVehicleCheck(), failed to Finish for ucmSubApp->GetId():"
                            << ucmSubApp->GetId().c_str();
            success = false;
            break;
        }
        log_.LogDebug() << "FsmManager::_executeVehicleCheck(), succeeded to Finish for ucmSubApp->GetId():"
                        << ucmSubApp->GetId().c_str();
    }

    MarkVehicleCheckFinishingFinish();  // Mark the end of FsmManager::_executeVehicleCheck Finish

    // If all above Finish are completed
    if (success) {
        log_.LogDebug() << "FsmManager::_executeVehicleCheck(), succeed to do all finishes.";
        std::ignore = SaveResolution(pkgmgr::UCMMasterResolutionType::kSuccess);  // Save Resolution as success
        std::ignore = TriggerIdle(pkgmgr::CampaignStateType::kVehicleChecking);
        return 0;
    }
    // Failure occurred, cancel
    log_.LogError() << "FsmManager::_executeVehicleCheck(), failed to do all finishes.";
    return 1;
}

/// @brief Reset
/// @throws no
void FsmManager::Reset() noexcept
{
    log_.LogDebug() << "FsmManager::Reset(), begin.";
    // Persistence only occurs when interface is called or config is read on restart
    ///allowCampaign_ = true;

    transferTotal_ = 0U;
    transferredNum_.store(0U);

    processTotal_ = 0U;
    processedNum_.store(0U);
    log_.LogDebug() << "FsmManager::Reset(), end.";
}

/// @brief Mark Campaign cancellation
/// @throws no
void FsmManager::SetCampaignCancelFlag() noexcept
{
    log_.LogDebug() << "FsmManager::SetCampaignCancelFlag(), begin.";
    campaignCancelFlag_ = true;
    log_.LogDebug() << "FsmManager::SetCampaignCancelFlag(), end.";
}

/// @brief Reset Campaign cancellation mark
/// @throws no
void FsmManager::ResetCampaignCancelFlag() noexcept
{
    log_.LogDebug() << "FsmManager::ResetCampaignCancelFlag(), begin.";
    campaignCancelFlag_ = false;
    log_.LogDebug() << "FsmManager::ResetCampaignCancelFlag(), end.";
}

/// @brief  Terminate
/// @throws no
void FsmManager::Stop() noexcept
{
    log_.LogDebug() << "FsmManager::Stop(), begin.";
    terminate_ = true;
    log_.LogDebug() << "FsmManager::Stop(), end.";
}

/// @brief GetTransferProgress
/// @return uint8_t
/// @throws no
uint8_t FsmManager::GetTransferProgress() const noexcept
{
    if (0U == transferTotal_) {
        return 0U;
    }
    double const progressBase{100.0F};
    double const progress{(static_cast< double >(transferredNum_.load())) / (static_cast< double >(transferTotal_))};

    log_.LogDebug() << "FsmManager::GetTransferProgress(), got transferredNum_:" << transferredNum_
                    << "transferTotal_:" << transferTotal_;
    return static_cast< uint8_t >(progress * progressBase);
}

/// @brief GetProcessProgress
/// @return uint8_t
/// @throws no
uint8_t FsmManager::GetProcessProgress() const noexcept
{
    if (0U == processTotal_) {
        return 0U;
    }
    double const progressBase{100.0F};
    double const progress{(static_cast< double >(processedNum_.load())) / (static_cast< double >(processTotal_))};

    log_.LogDebug() << "FsmManager::GetProcessProgress(), got processedNum_:" << processedNum_
                    << "processTotal_:" << processTotal_;
    return static_cast< uint8_t >(progress * progressBase);
}

/// @brief Update execution steps
/// @param rolloutSteps
/// @throws no
void FsmManager::UpdateRolloutSteps(ara::core::Vector< RolloutStep::Ptr >&& rolloutSteps) noexcept
{
    rolloutSteps_ = std::move(rolloutSteps);
}

/// @brief GetVarDataDir
/// @return String
/// @throws no
ara::core::String FsmManager::GetVarDataDir() const { return varDataDir_; }

/// @brief GetStatusFilePath
/// @return String
/// @throws no
ara::core::String FsmManager::GetStatusFilePath() const { return GetVarDataDir() + "/" + kStatusFileName; }

/// @brief GetStatusMD5FilePath
/// @return String
/// @throws no
ara::core::String FsmManager::GetStatusMD5FilePath() const { return GetVarDataDir() + "/" + kStatusMD5FileName; }

/// @brief Save Campaign repository
/// @param repository
/// @return bool
/// @throws no
bool FsmManager::SaveRepository(ara::core::String const& repository) const
{
    log_.LogDebug() << "FsmManager::SaveRepository(), begin with repository:" << repository.c_str();

    std::ignore = HistoryDatabase::GetInstance()->SaveRepository(std::move(ara::core::StringView(campaignHistoryKey_)),
                                                                 repository);

    log_.LogDebug() << "FsmManager::SaveRepository(), end.";
    return true;
}

/// @brief Save Campaign driverNotified
/// @param driverNotified
/// @return bool
/// @throws no
bool FsmManager::SaveDriverNotified(bool const driverNotified) const
{
    log_.LogDebug() << "FsmManager::SaveDriverNotified(), begin with driverNotified:" << driverNotified;

    std::ignore = HistoryDatabase::GetInstance()->SaveDriverNotified(
        std::move(ara::core::StringView(campaignHistoryKey_)), driverNotified);

    log_.LogDebug() << "FsmManager::SaveDriverNotified(), end.";
    return true;
}

/// @brief Save Campaign Resolution
/// @param resolution
/// @return bool
/// @throws no
bool FsmManager::SaveResolution(pkgmgr::UCMMasterResolutionType const resolution) const
{
    core::String padding;
    log_.LogDebug() << "FsmManager::SaveResolution(), begin with resolution:"
                    << com::internal::format::Formatter< core::String, pkgmgr::UCMMasterResolutionType >::ToString(
                           padding, resolution)
                           .c_str();

    std::ignore = HistoryDatabase::GetInstance()->SaveResolution(std::move(ara::core::StringView(campaignHistoryKey_)),
                                                                 resolution);

    log_.LogDebug() << "FsmManager::SaveResolution(), end.";
    return true;
}

/// @brief Record execution errors during the upgrade activity
/// @param ucmStepError
/// @return bool
/// @throws no
bool FsmManager::SaveUcmStepError(pkgmgr::UCMStepErrorType const& ucmStepError) const
{
    log_.LogDebug() << "FsmManager::SaveUcmStepError(), begin with ucmStepError:"
                    << helper::UCMStepErrorTypeToString(ucmStepError).c_str();

    std::ignore = HistoryDatabase::GetInstance()->SaveUcmStepError(
        std::move(ara::core::StringView(campaignHistoryKey_)), ucmStepError);

    log_.LogDebug() << "FsmManager::SaveUcmStepError(), end.";
    return true;
}

}  // namespace vpkgmgr
}  // namespace ucm
}  // namespace ara
