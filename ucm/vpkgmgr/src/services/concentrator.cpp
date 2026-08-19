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
/// @file       concentrator.cpp
/// @brief      Implementation for requesting OTA Client to transfer software packages and requesting driver approval.
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
/// @unit_name=Concentrator
/// @unit_description=Implementation for requesting OTA Client to transfer software packages and requesting driver approval.
/// @endcode
///
/// ================================================================

#include "services/concentrator.h"

#include "fsm/approvals/approval_activate.h"
#include "fsm/approvals/approval_process.h"
#include "fsm/approvals/approval_transfer.h"
#include "fsm/storage/persistence.h"

///#define APPROVAL_ACTIVATE "activate"
///#define APPROVAL_CACEL "cancel"
///#define APPROVAL_FINISH "finish"
///#define APPROVAL_PROCESS "process"
///#define APPROVAL_ROLLBACK "rollBack"
///#define APPROVAL_TRANSFER "transfer"

// TRANSFER
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
#define APPROVAL_ACTIVATE "ACTIVATE"
///#define APPROVAL_CACEL "CACEL"
///#define APPROVAL_FINISH "FINISH"
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
#define APPROVAL_PROCESS "PROCESS"
///#define APPROVAL_ROLLBACK "ROLLBACK"
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
#define APPROVAL_TRANSFER "TRANSFER"

namespace ara {
namespace ucm {
namespace vpkgmgr {

/// @brief LinkService
/// @param vdi
/// @param vsms
/// @param vpms
/// @throws no
void Concentrator::LinkService(VehicleDriverInterfaceService::Ptr const& vdi,
                               VehicleStateManagerService::Ptr const& vsms,
                               VehiclePackageManagerService::Ptr const& vpms) noexcept
{
    vdiService_  = vdi;
    vsmService_  = vsms;
    vpmsService_ = vpms;
}

/// @brief ReportState
/// @param campaignState
/// @throws no
void Concentrator::ReportState(pkgmgr::CampaignStateType const campaignState)
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
        case pkgmgr::CampaignStateType::kCancelling: {
            transferState = pkgmgr::TransferStateType::kCancelling;
            break;
        }
        case pkgmgr::CampaignStateType::kCorruptedStatusFile:
        default: {
            /// LOG_ERROR << "HandleStateChange happen a serious error campaignState_=" << static_cast<int8_t>(campaignState);
            break;
        }
    }

    std::ignore = vpmsService_->TransferState.Update(transferState);
    vpmsService_->UpdateState(transferState);
    std::ignore = vdiService_->CampaignState.Update(campaignState);
}

/// @brief NotificationSwpkg
/// @param swNameVersion
/// @throws no
void Concentrator::NotificationSwpkg(pkgmgr::SwNameVersionType const& swNameVersion) noexcept
{
    LOG_DEBUG << "begin with swNameVersion.Name:" << swNameVersion.Name.c_str()
              << "swNameVersion.Version:" << swNameVersion.Version.c_str();
    std::ignore = vpmsService_->RequestedPackage.Update(swNameVersion);
}

/// @brief UpdateDriverNotifications
/// @param driverNotifications
/// @throws no
void Concentrator::UpdateDriverNotifications(ara::core::Map< ara::core::String, bool > const& driverNotifications)
{
    driverNotifications_.clear();
    driverNotifications_ = driverNotifications;
}
/// @brief UpdateSafetyCondition
/// @param safetyCondition
/// @throws no
void Concentrator::UpdateSafetyCondition(pkgmgr::SafetyPolicyType const& safetyCondition)
{
    safetyCondition_ = safetyCondition;
}

/// @brief TransferApproval
/// @return bool
/// @throws no
bool Concentrator::TransferApproval()
{
    LOG_INFO << "requesting driver approval";
    ara::core::Map< ara::core::String, bool >::iterator const iter{driverNotifications_.find(APPROVAL_TRANSFER)};
    if (iter == driverNotifications_.end()) {
        LOG_DEBUG << "can't find driverNotification of " << APPROVAL_TRANSFER;
        return true;
    }

    bool const transferApproval{iter->second};
    ApprovalTransfer::Ptr approval{std::make_shared< ApprovalTransfer >(vdiService_, vsmService_, transferApproval)};
    return approval->Run(safetyCondition_);
}
/// @brief ProcessApproval
/// @return bool
/// @throws no
bool Concentrator::ProcessApproval()
{
    LOG_INFO << "requesting driver approval";
    ara::core::Map< ara::core::String, bool >::iterator const iter{driverNotifications_.find(APPROVAL_PROCESS)};
    if (iter == driverNotifications_.end()) {
        LOG_DEBUG << "can't find driverNotification of " << APPROVAL_PROCESS;
        return true;
    }

    bool const processApproval{iter->second};
    ApprovalProcess::Ptr approval{std::make_shared< ApprovalProcess >(vdiService_, vsmService_, processApproval)};
    return approval->Run(safetyCondition_);
}
/// @brief ActivateApproval
/// @return bool
/// @throws no
bool Concentrator::ActivateApproval()
{
    LOG_INFO << "requesting driver approval";
    ara::core::Map< ara::core::String, bool >::iterator const iter{driverNotifications_.find(APPROVAL_ACTIVATE)};
    if (iter == driverNotifications_.end()) {
        LOG_DEBUG << "can't find driverNotification of " << APPROVAL_ACTIVATE;
        return true;
    }

    bool const activateApproval{iter->second};
    ApprovalActivate::Ptr approval{std::make_shared< ApprovalActivate >(vdiService_, vsmService_, activateApproval)};
    return approval->Run(safetyCondition_);
}

}  // namespace vpkgmgr
}  // namespace ucm
}  // namespace ara