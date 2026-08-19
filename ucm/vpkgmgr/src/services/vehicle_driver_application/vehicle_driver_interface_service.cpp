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
/// @file       vehicle_driver_interface_service.cpp
/// @brief      All service interfaces provided by the UCM host to the vehicle driver adaptive application.
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
/// @unit_name=VehicleDriverInterfaceService
/// @unit_description=All service interfaces provided by the UCM host to the vehicle driver adaptive application.
/// @endcode
///
/// ================================================================

#include "services/vehicle_driver_application/vehicle_driver_interface_service.h"

#include "consts.h"
#include "fsm/fsm_manager.h"
#include "utils/ucmm_log.h"

namespace ara {
namespace ucm {
namespace vpkgmgr {

/// @brief alias SkeletonApp
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
using SkeletonApp = vdai::skeleton::VehicleDriverApplicationSkeleton;

/// @brief constructor
/// @param instanceSpec
/// @param mode
/// @throws no
VehicleDriverInterfaceService::VehicleDriverInterfaceService(ara::core::InstanceSpecifier instanceSpec,
                                                             ara::com::MethodCallProcessingMode const mode)
    : SkeletonApp{std::move(instanceSpec), mode}
    , actionCB_{[](bool const&, pkgmgr::SafetyPolicyType const&) noexcept { LOG_WARN << "call default callback;"; }}
{
}

/// @brief constructor
/// @param instanceId
/// @param mode
/// @throws no
VehicleDriverInterfaceService::VehicleDriverInterfaceService(ara::com::InstanceIdentifier instanceId,
                                                             ara::com::MethodCallProcessingMode const mode)
    : SkeletonApp{std::move(instanceId), mode}
    , actionCB_{[](bool const&, pkgmgr::SafetyPolicyType const&) noexcept { LOG_WARN << "call default callback;"; }}
{
}

/// @brief FieldInit
/// @param state
/// @return bool
/// @throws no
bool VehicleDriverInterfaceService::FieldInit(pkgmgr::CampaignStateType const& state)
{
    LOG_DEBUG << "start with state:" << CampaignStateTypeToString(state).data();

    ara::core::Result< void > voidRes{ApprovalRequired.Update(false)};
    if (!voidRes.HasValue()) {
        LOG_ERROR << "VehicleDriverInterfaceService ApprovalRequired Init value failed errmsg="
                  << voidRes.Error().Message().data();
        return false;
    }
    voidRes = CampaignState.Update(state);
    if (!voidRes.HasValue()) {
        LOG_ERROR << "VehicleDriverInterfaceService CampaignState Init value failed errmsg="
                  << voidRes.Error().Message().data();
        return false;
    }

    voidRes = SafetyPolicy.Update(kNone);
    if (!voidRes.HasValue()) {
        LOG_ERROR << "VehicleDriverInterfaceService SafetyPolicy Init value failed errmsg="
                  << voidRes.Error().Message().data();
        return false;
    }

    voidRes = SafetyState.Update(false);
    if (!voidRes.HasValue()) {
        LOG_ERROR << "VehicleDriverInterfaceService SafetyState Init value failed errmsg="
                  << voidRes.Error().Message().data();
        return false;
    }
    return true;
}

/// @brief SetApprovalCB
/// @param cb
/// @throws no
void VehicleDriverInterfaceService::SetApprovalCB(CBType const& cb) { actionCB_ = cb; }

/// @brief This method aborts the ongoing vehicle package processing activity.
/// @param disableCampaign
/// Whether to disable new update activities. false: only disable this update activity.
/// true: also disable subsequent update activities
/// @return Returns a void future, or a future of a specific error code (please refer to ara::core::Future usage instructions)
/// @throws no
ara::core::Future< void > VehicleDriverInterfaceService::CancelCampaign(bool const& disableCampaign)
{
    ara::core::Future< void > future{};
    {
        ara::core::Promise< void > promise;
        future = promise.get_future();
        LOG_INFO << "begin";
        std::ignore = FsmManager::GetInstance()->CancelCampaign(disableCampaign, std::move(promise));
    }

    LOG_INFO << "end";
    return future;
}

/// @brief Allow a new upgrade/update activity to start
/// @return Returns a void future, or a future of a specific error code (please refer to ara::core::Future usage instructions)
/// @throws no
ara::core::Future< void > VehicleDriverInterfaceService::AllowCampaign()
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

/// @brief
/// @param approval
/// @param safetyPolicy Notifies the UCM Master of all unsupported safety conditions
/// @return
/// @throws no
ara::core::Future< void > VehicleDriverInterfaceService::DriverApproval(bool const& approval,
                                                                        pkgmgr::SafetyPolicyType const& safetyPolicy)
{
    LOG_INFO << "approval=" << BoolToStr(approval).data() << " safetyPolicy=" << safetyPolicy.c_str();

    ara::core::Promise< void > promise;
    ara::core::Future< void > future{promise.get_future()};

    actionCB_(approval, safetyPolicy);

    promise.set_value();
    return future;
}

/// @brief Retrieve all operations executed by the UCM host
/// @param timestampGE Earliest timestamp (inclusive)
/// @param timestampLT Latest timestamp (exclusive)
/// @return Returns a future of all operation records executed by the UCM host (please refer to ara::core::Future usage instructions) or a specific error code
/// @throws no
ara::core::Future< SkeletonApp::GetCampaignHistoryOutput > VehicleDriverInterfaceService::GetCampaignHistory(
    std::uint64_t const& timestampGE, std::uint64_t const& timestampLT)
{
    ara::core::Promise< SkeletonApp::GetCampaignHistoryOutput > promise;
    ara::core::Future< SkeletonApp::GetCampaignHistoryOutput > future{promise.get_future()};

    SkeletonApp::GetCampaignHistoryOutput out;
    out.history = FsmManager::GetCampaignHistory(timestampGE, timestampLT);
    promise.set_value(out);
    return future;
}

/// @brief List of general information of software clusters existing in the adaptive platform
/// @return Returns a Future of the general software cluster information list or a specific error code (please refer to ara::core::Future usage instructions)
/// @throws no
ara::core::Future< SkeletonApp::GetSwClusterDescriptionOutput > VehicleDriverInterfaceService::GetSwClusterDescription()
{
    return FsmManager::GetInstance()->GetSwClusterDescription();
}

/// @brief List of general information of software packages that are part of the current active activity processed by VUCM
/// @return Returns a Future of the general software package information list or a specific error code (please refer to ara::core::Future usage instructions)
/// @throws no
ara::core::Future< SkeletonApp::GetSwPackageDescriptionOutput > VehicleDriverInterfaceService::GetSwPackageDescription()
{
    LOG_INFO << "begin";

    return FsmManager::GetInstance()->GetSwPackageDescription();
}

/// @brief Get the current package processing progress (0-100%).
/// @return Returns a Future of the current package upgrade/installation progress or a specific error code (please refer to ara::core::Future usage instructions)
/// Current package processing progress (0%-100%). 0x00…0x64, 0xFF indicates no information available
/// @throws no
ara::core::Future< SkeletonApp::GetSwProcessProgressOutput > VehicleDriverInterfaceService::GetSwProcessProgress()
{
    LOG_INFO << "begin";
    return FsmManager::GetInstance()->GetSwProcessProgress();
}

/// @brief Get the current package transfer progress (0-100%).
/// @return Returns a Future of the current package transfer progress or a specific error code (please refer to ara::core::Future usage instructions)
/// @throws no
ara::core::Future< SkeletonApp::GetSwTransferProgressOutput > VehicleDriverInterfaceService::GetSwTransferProgress()
{
    LOG_INFO << "begin";

    return FsmManager::GetInstance()->GetSwTransferProgress();
}

}  // namespace vpkgmgr
}  // namespace ucm
}  // namespace ara