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
/// @file       vehicle_package_application.cpp
/// @brief      Completes program initialization and shutdown, creates other modules
/// @details
/// @date       2024-07-20
/// @author     hanzhibo
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/UCM Master/Main
/// @interface_level=unit
/// @trace_id_sr=SR_UCM_00035
/// @unit_name=VehiclePackageManagementApplication
/// @unit_description=Completes program initialization and shutdown, creates other modules
/// @endcode
///
/// ================================================================

#include "vehicle_package_application.h"

#include <isoft/ara_fsh/filesystem_hierarchy.h>
#include <isoft/naicpp/evloop.h>
#include <isoft/naicpp/global_evloop.h>

#include <chrono>
#include <memory>
#include <thread>
#include <utility>

#include "consts.h"
#include "fsm/fsm_manager.h"
#include "fsm/parsing/manifest_parse.h"
#include "fsm/storage/history_database.h"
#include "fsm/storage/persistence.h"
#include "fsm/task_executor/task_queue.h"
#include "services/concentrator.h"
#include "utils/ucmm_log.h"
#include "utils/utils.h"

namespace ara {
namespace ucm {
namespace vpkgmgr {

/// @brief _vehiclePackageManagerInit
/// @param state
/// @return bool
/// @throws no
bool VehiclePackageManagementApplication::_vehiclePackageManagerInit(pkgmgr::TransferStateType const& state)
{
    ///ara::com::InstanceIdentifier serviceInstanceId{"SOME/IP:909"};
    LOG_INFO << "begin";

    ara::core::InstanceSpecifier const serviceInstanceId{
        std::move(ara::core::StringView("ucmmd/ucmm_root/VehiclePackageManagementPPort"))};
    /// VehiclePackageManagerService interacts with OTAClient, this is the server side
    ara::core::Result< VehiclePackageManagerService > vpmRet{
        VehiclePackageManagerService::Create< VehiclePackageManagerService >(
            serviceInstanceId, ara::com::MethodCallProcessingMode::kEvent)};
    if (!vpmRet.HasValue()) {
        LOG_ERROR << "VehiclePackageManagerService create failed! errmsg=" << vpmRet.Error().Message().data();
        return false;
    }
    vpmService_ = std::make_shared< VehiclePackageManagerService >(std::move(vpmRet).Value());
    if (!vpmService_->FieldInit(state)) {
        LOG_ERROR << "VehiclePackageManagerService FieldInit fail!";
        return false;
    }
    ara::core::Result< void > const offerRet{vpmService_->OfferService()};
    if (!offerRet.HasValue()) {
        LOG_ERROR << "VehiclePackageManagerService OfferService failed! errmsg=" << offerRet.Error().Message().data();
        return false;
    }
    LOG_INFO << "VehiclePackageManagement service instance is created with id " << serviceInstanceId.ToString().data();
    LOG_INFO << "end";

    return true;
}

/// @brief _vehicleDriverInterfaceServiceInit
/// @param state
/// @return bool
/// @throws no
bool VehiclePackageManagementApplication::_vehicleDriverInterfaceServiceInit(pkgmgr::CampaignStateType const& state)
{
    // For Vehicle Driver interface, this program is the server side
    ///ara::com::InstanceIdentifier driverInstanceId{"SOME/IP:908"};
    LOG_INFO << "begin";
    ara::core::InstanceSpecifier const driverInstanceId{
        std::move(ara::core::StringView("ucmmd/ucmm_root/VehicleDriverApplicationPPort"))};
    ara::core::Result< VehicleDriverInterfaceService > vdifRet{
        VehicleDriverInterfaceService::Create< VehicleDriverInterfaceService >(
            driverInstanceId, ara::com::MethodCallProcessingMode::kEvent)};
    if (!vdifRet.HasValue()) {
        LOG_ERROR << "VehicleDriverInterfaceService create failed! errmsg=" << vdifRet.Error().Message().data();
        return false;
    }
    driverInterfaceService_ = std::make_shared< VehicleDriverInterfaceService >(std::move(vdifRet).Value());
    if (!driverInterfaceService_->FieldInit(state)) {
        LOG_ERROR << "VehicleDriverInterfaceService FieldInit fail";
        return false;
    }
    ara::core::Result< void > const offerRet{driverInterfaceService_->OfferService()};
    if (!offerRet.HasValue()) {
        LOG_ERROR << "VehicleDriverInterfaceService OfferService failed! errmsg=" << offerRet.Error().Message().data();
        return false;
    }
    LOG_INFO << "VehicleDriverInterfaceService instance is created with id " << driverInstanceId.ToString().data();
    LOG_INFO << "end";
    return true;
}

/// @brief _vehicleStateManagerServiceInit
/// @return bool
/// @throws no
bool VehiclePackageManagementApplication::_vehicleStateManagerServiceInit()
{
    // For Vehicle State Manager, this program is the server side
    ///ara::com::InstanceIdentifier stateManagerInstanceId{"SOME/IP:907"};
    LOG_INFO << "begin";
    ara::core::InstanceSpecifier const stateManagerInstanceId{
        std::move(ara::core::StringView("ucmmd/ucmm_root/VehicleStateManagerPPort"))};
    ara::core::Result< VehicleStateManagerService > vsmRet{
        VehicleStateManagerService::Create< VehicleStateManagerService >(stateManagerInstanceId,
                                                                         ara::com::MethodCallProcessingMode::kEvent)};
    if (!vsmRet.HasValue()) {
        LOG_ERROR << "VehicleStateManagerService create failed! errmsg=" << vsmRet.Error().Message().data();
        return false;
    }
    stateManagerService_ = std::make_shared< VehicleStateManagerService >(std::move(vsmRet).Value());
    if (!stateManagerService_->FieldInit()) {
        LOG_ERROR << "VehicleStateManagerService FieldInit fail";
        return false;
    }
    ara::core::Result< void > const offerRet{stateManagerService_->OfferService()};
    if (!offerRet.HasValue()) {
        LOG_ERROR << "VehicleStateManagerService OfferService failed! errmsg=" << offerRet.Error().Message().data();
        return false;
    }
    LOG_INFO << "VehicleStateManagerService instance is created with id " << stateManagerInstanceId.ToString().data();
    LOG_INFO << "end";
    return true;
}

/// @brief Service initialization, load configuration
/// @return true: success, false: failure
/// @throws no
bool VehiclePackageManagementApplication::Initialize()
{
    LOG_INFO << "begin";

    /// Unified creation via singleton
    std::ignore = HistoryDatabase::Create();
    if (!HistoryDatabase::GetInstance()->Init()) {
        log_.LogError()
            << "VehiclePackageManagementApplication::Initialize(), failed to HistoryDatabase::GetInstance()->Init.";
        return false;
    }

    std::ignore = Persistence::Create();
    std::ignore = TaskQueue::Create();
    FindUcmSubs* const subs{FindUcmSubs::Create()};
    std::ignore = Concentrator::Create();
    FsmManager* const fsm{FsmManager::Create()};

    // Machine manifest location
    isoft::ara_fsh::Platform const platform;
    ara::core::String const machineManifestPath{platform.GetMachineManifest()};
    ManifestParse manifestParse{};
    std::ignore = manifestParse.Parse(std::move(ara::core::StringView(machineManifestPath)));

    subs->SetApiWaitTime(manifestParse.GetApiWaitTime());
    /// For UCM subordinates, this program is the client; find all UCM subordinates
    if (!subs->FindService()) {
        return false;
    }

    if (0 != fsm->Init(this)) {
        return false;
    }

    LOG_INFO << "Application::Initialize() end";
    return true;
}

/// @brief InitServices
/// @param state
/// @return bool
/// @throws no
bool VehiclePackageManagementApplication::InitServices(pkgmgr::CampaignStateType const& state)
{
    log_.LogDebug() << "VehiclePackageManagementApplication::InitServices(), start with state:"
                    << CampaignStateTypeToString(state).data();

    /// Initialize service for interacting with OTAClient
    if (!_vehiclePackageManagerInit(_toTransferState(state))) {
        return false;
    }

    /// Initialize driver interface service
    if (!_vehicleDriverInterfaceServiceInit(state)) {
        return false;
    }

    /// Initialize vehicle state service
    if (!_vehicleStateManagerServiceInit()) {
        return false;
    }
    Concentrator::GetInstance()->LinkService(driverInterfaceService_, stateManagerService_, vpmService_);
    log_.LogDebug() << "VehiclePackageManagementApplication::InitServices(), end.";
    return true;
}

/// @brief A method to be executed at runtime.
/// Shall be reimplemented for the specific application
/// @throws no
void VehiclePackageManagementApplication::Run() const
{
    log_.LogDebug() << "VehiclePackageManagementApplication::Run(), begin.";
    // report that execution state is running state
    ReportRunningState();

    // run util receive close signal
    log_.LogDebug() << "VehiclePackageManagementApplication::Run(), TaskQueue AsyncRun.";
    TaskQueue::GetInstance()->AsyncRun();

    log_.LogDebug() << "VehiclePackageManagementApplication::Run(), GlobalGeneralEvLoop Run.";
    std::ignore = isoft::naicpp::GlobalGeneralEvLoop::Get()->Run(true);

    log_.LogDebug() << "VehiclePackageManagementApplication::Run(), end.";
}
/// @brief Stop
/// @throws no
void VehiclePackageManagementApplication::Stop() const noexcept
{
    log_.LogDebug() << "VehiclePackageManagementApplication::Stop, start.";

    // cancel
    FsmManager::GetInstance()->Stop();  // Set stop flag

    log_.LogDebug() << "VehiclePackageManagementApplication::Stop(), try to StopFindService.";
    FindUcmSubs::GetInstance()->StopFindService();

    log_.LogDebug() << "VehiclePackageManagementApplication::Stop(), try to Stop TaskQueue.";
    TaskQueue::GetInstance()->Stop();

    log_.LogDebug() << "VehiclePackageManagementApplication::Stop, end.";
}
/// @brief Uninitialize
/// @throws no
void VehiclePackageManagementApplication::Uninitialize()
{
    log_.LogDebug() << "VehiclePackageManagementApplication::Uninitialize(), begin.";

    log_.LogDebug() << "VehiclePackageManagementApplication::Uninitialize(), try to StopOfferService for vpmService_.";
    if (nullptr != vpmService_.get()) {
        LOG_INFO << "vpm stop offer";
        vpmService_->StopOfferService();
    }

    log_.LogDebug()
        << "VehiclePackageManagementApplication::Uninitialize(), try to StopOfferService for driverInterfaceService_.";
    if (nullptr != driverInterfaceService_.get()) {
        LOG_INFO << "vdi stop offer";
        driverInterfaceService_->StopOfferService();
    }

    log_.LogDebug()
        << "VehiclePackageManagementApplication::Uninitialize(), try to StopOfferService for stateManagerService_.";
    if (nullptr != stateManagerService_.get()) {
        LOG_INFO << "vsm stop offer";
        stateManagerService_->StopOfferService();
    }

    log_.LogDebug() << "VehiclePackageManagementApplication::Uninitialize(), try to Release FsmManager.";
    FsmManager::Release();

    log_.LogDebug() << "VehiclePackageManagementApplication::Uninitialize(), try to Release Concentrator.";
    Concentrator::Release();

    log_.LogDebug() << "VehiclePackageManagementApplication::Uninitialize(), try to Release FindUcmSubs.";
    FindUcmSubs::Release();

    log_.LogDebug() << "VehiclePackageManagementApplication::Uninitialize(), try to Release TaskQueue.";
    TaskQueue::Release();

    log_.LogDebug() << "VehiclePackageManagementApplication::Uninitialize(), try to Release Persistence.";
    Persistence::Release();

    log_.LogDebug() << "VehiclePackageManagementApplication::Uninitialize(), try to Release HistoryDatabase.";
    HistoryDatabase::Release();

    ///log_.LogDebug() << "VehiclePackageManagementApplication::Uninitialize(), try to stop GlobalGeneralEvLoop.";
    ///isoft::naicpp::GlobalGeneralEvLoop::Get()->Stop();

    log_.LogDebug() << "VehiclePackageManagementApplication::Uninitialize(), end.";
}

/// @brief Report to Execution Manager that the App reached its running state
/// @throws no
void VehiclePackageManagementApplication::ReportRunningState() noexcept
{
    ara::exec::ExecutionClient exeClient;
    if (!exeClient.ReportExecutionState(ara::exec::ExecutionState::kRunning).HasValue()) {
        LOG_ERROR << "report ExecutionState::kRunning failed";
        return;
    }
    LOG_INFO << "reported ExecutionState::kRunning successfully";
    return;
}

/// @brief _toTransferState
/// @param campaignState
/// @return TransferStateType
/// @throws no
pkgmgr::TransferStateType VehiclePackageManagementApplication::_toTransferState(
    pkgmgr::CampaignStateType const campaignState) const noexcept
{
    pkgmgr::TransferStateType transferState{pkgmgr::TransferStateType::kIdle};

    switch (campaignState) {
        case pkgmgr::CampaignStateType::kIdle: {
            transferState = pkgmgr::TransferStateType::kIdle;
            break;
        }
        case pkgmgr::CampaignStateType::kSyncing: {
            transferState = pkgmgr::TransferStateType::kSyncing;
            break;
        }
        case pkgmgr::CampaignStateType::kVehiclePackageTransferring:
        case pkgmgr::CampaignStateType::kSoftwarePackage_Transferring: {
            transferState = pkgmgr::TransferStateType::kTransferring;
            break;
        }
        case pkgmgr::CampaignStateType::kProcessing:
        case pkgmgr::CampaignStateType::kActivating:
        case pkgmgr::CampaignStateType::kVehicleChecking: {
            transferState = pkgmgr::TransferStateType::kUpdating;
            break;
        }
        case pkgmgr::CampaignStateType::kCancelling:
        case pkgmgr::CampaignStateType::kCorruptedStatusFile: {
            transferState = pkgmgr::TransferStateType::kCancelling;
            break;
        }
        default: {
            log_.LogError() << "VehiclePackageManagementApplication::_toTransferState(), unknown campaignState";
            break;
        }
    }

    return transferState;
}

}  // namespace vpkgmgr
}  // namespace ucm
}  // namespace ara
