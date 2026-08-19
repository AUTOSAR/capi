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
/// @file       vehicle_package_manager_service.cpp
/// @brief      Service interface provided by VUCM to the OTA client adaptive application
/// @details
/// @date       2024-07-20
/// @author     hanzhibo
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/UCM Master/Services
/// @interface_level=module
/// @trace_id_sr=SR_UCM_00035
/// @unit_name=VehiclePackageManagerService
/// @unit_description=Service interface provided by VUCM to the OTA client adaptive application
/// @endcode
///
/// ================================================================

#include "services/vehicle_package_management/vehicle_package_manager_service.h"

#include <ara/core/promise.h>
#include <ara/core/string.h>
#include <ara/core/vector.h>
#include <ara/ucm/pkgmgr/impl_type_campaignstatetype.h>

#include <algorithm>

#include "fsm/fsm_manager.h"
#include "fsm/parsing/vehicle_package_parser.h"
#include "fsm/states/campaign_state_base.h"
#include "utils/ucmm_log.h"
#include "utils/utils.h"

namespace ara {
namespace ucm {
namespace vpkgmgr {

using pkgmgr::ByteVectorType;
using pkgmgr::SwNameType;
using pkgmgr::SwNameVersionVectorType;
using pkgmgr::TransferIdType;
using pkgmgr::TransferStateType;
/// @brief alias SkeletonApp
using SkeletonApp = pkgmgr::skeleton::VehiclePackageManagementSkeleton;

/// @brief constructor
/// @param instanceSpec
/// @param mode
/// @throws no
VehiclePackageManagerService::VehiclePackageManagerService(ara::core::InstanceSpecifier instanceSpec,
                                                           ara::com::MethodCallProcessingMode const mode)
    : SkeletonApp{std::move(instanceSpec), mode}, state_{pkgmgr::TransferStateType::kIdle}
{
}

/// @brief constructor
/// @param instanceId
/// @param mode
/// @throws no
VehiclePackageManagerService::VehiclePackageManagerService(ara::com::InstanceIdentifier instanceId,
                                                           ara::com::MethodCallProcessingMode const mode)
    : SkeletonApp{std::move(instanceId), mode}, state_{pkgmgr::TransferStateType::kIdle}
{
}

/// @brief FieldInit
/// @param state
/// @return bool
/// @throws no
bool VehiclePackageManagerService::FieldInit(pkgmgr::TransferStateType const& state)
{
    LOG_DEBUG << "start with state:" << TransferStateTypeToString(state).data();

    ara::core::Result< void > voidRes{TransferState.Update(state)};
    if (!voidRes.HasValue()) {
        LOG_ERROR << "VehiclePackageManagerService TransferState Init value failed errmsg="
                  << voidRes.Error().Message().data();
        return false;
    }

    state_      = state;
    std::ignore = TransferState.RegisterGetHandler([this]() noexcept {
        FsmManager::GetInstance()->UpdateHeartbeat();
        return _getNotifier();
    });

    // Safely initialize to true
    pkgmgr::SwNameVersionType const swName{"NONE", "0.0.0"};
    voidRes = RequestedPackage.Update(swName);
    if (!voidRes.HasValue()) {
        LOG_ERROR << "VehiclePackageManagerService RequestedPackage Init value failed errmsg="
                  << voidRes.Error().Message().data();
        return false;
    }

    // Safely initialize to true
    voidRes = SafetyState.Update(true);
    if (!voidRes.HasValue()) {
        LOG_ERROR << "VehiclePackageManagerService SafetyState Init value failed errmsg="
                  << voidRes.Error().Message().data();
        return false;
    }

    LOG_INFO << "end";
    return true;
}

/// @brief _getNotifier
/// @return future
/// @throws no
ara::core::Future< pkgmgr::TransferStateType > VehiclePackageManagerService::_getNotifier() const noexcept
{
    LOG_DEBUG << "begin with state_:" << TransferStateTypeToString(state_).data();

    ara::core::Promise< pkgmgr::TransferStateType > promise;
    ara::core::Future< pkgmgr::TransferStateType > future{promise.get_future()};
    promise.set_value(state_);

    LOG_DEBUG << "end";
    return future;
}

/// @brief UpdateState
/// @param state
/// @throws no
void VehiclePackageManagerService::UpdateState(pkgmgr::TransferStateType const state)
{
    LOG_DEBUG << "begin with state:" << TransferStateTypeToString(state).data();
    state_ = state;
    LOG_DEBUG << "end";
}

///////////////////////////// method //////////////////////////////
// Allow new upgrade activity to start
/// @brief Let a new campaign begin
/// @return Returns a void future or a specific error code (please refer to ara::core::Future usage instructions)
/// @throws no
ara::core::Future< void > VehiclePackageManagerService::AllowCampaign()
{
    // Allow new update activity to be established
    ara::core::Future< void > future{};
    {
        ara::core::Promise< void > promise;
        future = promise.get_future();
        LOG_INFO << "begin";
        std::ignore = FsmManager::GetInstance()->AllowCampaign(std::move(promise));
    }

    LOG_INFO << "end";
    return future;
}

// CancelFailed
// InvalidTransferId
// OperationNotPermitted
/// @brief This method aborts the ongoing vehicle package processing activity.
/// @param disableCampaign Whether to disable new update activities. false: only disable this update activity. true: also disable subsequent update activities
/// @return Returns a void future or a specific error code (please refer to ara::core::Future usage instructions)
/// @throws no
ara::core::Future< void > VehiclePackageManagerService::CancelCampaign(bool const& disableCampaign)
{
    LOG_DEBUG << "start with disableCampaign:" << disableCampaign;
    ara::core::Future< void > future{};
    {
        ara::core::Promise< void > promise;
        future      = promise.get_future();
        std::ignore = FsmManager::GetInstance()->CancelCampaign(disableCampaign, std::move(promise));
    }

    LOG_DEBUG << "end";
    return future;
}

/// @brief Retrieve all operations executed by the UCM host
/// @param timestampGE Earliest timestamp (inclusive)
/// @param timestampLT Latest timestamp (exclusive)
/// @return Returns a future of all operation records executed by the UCM host or a specific error code (please refer to ara::core::Future usage instructions)
/// @throws no
ara::core::Future< SkeletonApp::GetCampaignHistoryOutput > VehiclePackageManagerService::GetCampaignHistory(
    std::uint64_t const& timestampGE, std::uint64_t const& timestampLT)
{
    LOG_INFO << "begin";
    ara::core::Promise< SkeletonApp::GetCampaignHistoryOutput > promise;
    ara::core::Future< SkeletonApp::GetCampaignHistoryOutput > future{promise.get_future()};

    SkeletonApp::GetCampaignHistoryOutput out;
    out.CampaignHistory = FsmManager::GetCampaignHistory(timestampGE, timestampLT);
    promise.set_value(out);

    return future;
}

/// @brief Get the list of software clusters in the kPresent state on the platform.
/// @return Returns a future of a software cluster list or a specific error code (please refer to ara::core::Future usage instructions)
/// @throws no
ara::core::Future< SkeletonApp::GetSwClusterInfoOutput > VehiclePackageManagerService::GetSwClusterInfo()
{
    LOG_INFO << "begin";
    return FsmManager::GetInstance()->GetSwClusterInfo();
}

/// @brief Get software package information that is part of the current active activity processed by VUCM
/// @return Returns a future of a software package information list or a specific error code (please refer to ara::core::Future usage instructions)
/// @throws no
ara::core::Future< SkeletonApp::GetSwPackagesOutput > VehiclePackageManagerService::GetSwPackages()
{
    LOG_INFO << "begin";
    return FsmManager::GetInstance()->GetSwPackages();
}

/// @brief Compares the provided list of available software clusters from the vehicle backend with the internal information of software clusters currently in the vehicle, and returns the list of software clusters selected for update
/// @param availableSoftwarePackages List of available software packages from the backend
/// @return Returns a future of the list of available update software packages from the backend or a specific error code (please refer to ara::core::Future usage instructions)
/// @throws no
ara::core::Future< SkeletonApp::SwPackageInventoryOutput > VehiclePackageManagerService::SwPackageInventory(
    pkgmgr::SwNameVersionVectorType const& availableSoftwarePackages)
{
    LOG_INFO << "begin";
    return FsmManager::GetInstance()->TriggerSync(availableSoftwarePackages);
}

/// @brief Start transporting the vehicle package. The size of the vehicle package to be transferred to VUCM must be provided. VUCM will generate a transfer ID for subsequent calls
/// TransferData, TransferXit, ProcessSwPackage and DeleteTransfer.
/// @param size The size of the vehicle package to be transferred (bytes).
/// @return   Returns a future of a vehicle package transfer information handle or a specific error code (please refer to ara::core::Future usage instructions)
/// @throws no
ara::core::Future< SkeletonApp::TransferVehiclePackageOutput > VehiclePackageManagerService::TransferVehiclePackage(
    std::uint64_t const& size)
{
    LOG_INFO << "begin";
    FsmManager* const fsm{FsmManager::GetInstance()};
    return fsm->TransferVehiclePackage(size);
}

/// @brief Start the transfer of a software package. The name of the software package to be transferred to VUCM must be provided. VUCM
/// will generate a Transfer ID for subsequent calls to TransferData, TransferXit, and DeleteTransfer.
/// The size of the software package for transfer to UCM subordinates is provided in the vehicle package and its contained software package manifest.
/// @param softwarePackageName The short name of the software package to be transferred.
/// @return   Returns a future of a software package transfer information handle or a specific error code (please refer to ara::core::Future usage instructions)
/// @throws no
ara::core::Future< SkeletonApp::TransferStartOutput > VehiclePackageManagerService::TransferStart(
    SwNameType const& softwarePackageName)
{
    LOG_INFO << "begin";
    FsmManager* const fsm{FsmManager::GetInstance()};
    return fsm->TransferStart(softwarePackageName);
}

/// @brief Block transfer interface for software package or vehicle package to VUCM
/// @param id Transfer handle
/// @param data Data block of the software package or vehicle package
/// @param blockCounter Block counter value of the current block
/// @return ara::core::Future<void> Returns a future or a specific error code (please refer to ara::core::Future usage instructions)
/// @throws no
ara::core::Future< void > VehiclePackageManagerService::TransferData(TransferIdType const& id,
                                                                     ByteVectorType const& data,
                                                                     std::uint64_t const& blockCounter)
{
    LOG_INFO << "begin";
    FsmManager* const fsm{FsmManager::GetInstance()};
    return fsm->TransferData(id, data, blockCounter);
}

/// @brief Transfer of software package or vehicle package to VUCM is complete.
/// @param id Transfer handle
/// @return Returns a void future or a specific error code (please refer to ara::core::Future usage instructions)
/// @throws no
ara::core::Future< void > VehiclePackageManagerService::TransferExit(TransferIdType const& id)
{
    LOG_INFO << "begin, id=" << pkgmgr::helper::ToHexString(id).c_str();
    FsmManager* const fsm{FsmManager::GetInstance()};
    return fsm->TransferExit(id);
}

/// @brief Delete the transferred software package or vehicle package
/// @param id Transfer handle id of the currently running request.
/// @return Returns a void future or a specific error code (please refer to ara::core::Future usage instructions)
/// @throws no
/// @uptrace={SWS_UCM_01010}
///
ara::core::Future< void > VehiclePackageManagerService::DeleteTransfer(TransferIdType const& id)
{
    LOG_INFO << "begin";
    FsmManager* const fsm{FsmManager::GetInstance()};
    return fsm->DeleteTransfer(id);
}

}  // namespace vpkgmgr
}  // namespace ucm
}  // namespace ara
