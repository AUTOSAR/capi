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
/// @file       campaign_state_base.cpp
/// @brief      CampaignStateBase impl
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
/// @unit_name=CampaignStateBase
/// @unit_description=CampaignStateBase impl
/// @endcode
///
/// ================================================================

#include "fsm/states/campaign_state_base.h"

#include <ara/core/string_view.h>

#include "ara/ucm/pkgmgr/error_domain_ucmerrordomain.h"
#include "fsm/fsm_manager.h"
#include "utils/ucmm_log.h"

namespace ara {
namespace ucm {
namespace vpkgmgr {

using pkgmgr::UCMErrorDomainErrc;

/// @brief CampaignStateTypeToString
/// @param type
/// @return type stringview
/// @throws no
ara::core::StringView CampaignStateTypeToString(pkgmgr::CampaignStateType const type)
{
    ara::core::StringView str;
    switch (type) {
        case pkgmgr::CampaignStateType::kIdle: {
            str = ara::core::StringView("kIdle");
            break;
        }
        case pkgmgr::CampaignStateType::kSyncing: {
            str = ara::core::StringView("kSyncing");
            break;
        }
        case pkgmgr::CampaignStateType::kVehiclePackageTransferring: {
            str = ara::core::StringView("kVehiclePackageTransferring");
            break;
        }
        case pkgmgr::CampaignStateType::kSoftwarePackage_Transferring: {
            str = ara::core::StringView("kSoftwarePackage_Transferring");
            break;
        }
        case pkgmgr::CampaignStateType::kProcessing: {
            str = ara::core::StringView("kProcessing");
            break;
        }
        case pkgmgr::CampaignStateType::kActivating: {
            str = ara::core::StringView("kActivating");
            break;
        }
        case pkgmgr::CampaignStateType::kVehicleChecking: {
            str = ara::core::StringView("kVehicleChecking");
            break;
        }
        case pkgmgr::CampaignStateType::kCancelling: {
            str = ara::core::StringView("kCancelling");
            break;
        }
        default: {
            str = ara::core::StringView("unknown");
            break;
        }
    }
    return str;
}

/// @brief TransferStateTypeToString
/// @param type
/// @return type stringview
/// @throws no
ara::core::StringView TransferStateTypeToString(pkgmgr::TransferStateType const type)
{
    ara::core::StringView str;
    switch (type) {
        case pkgmgr::TransferStateType::kIdle: {
            str = ara::core::StringView("kIdle");
            break;
        }
        case pkgmgr::TransferStateType::kSyncing: {
            str = ara::core::StringView("kSyncing");
            break;
        }
        case pkgmgr::TransferStateType::kTransferring: {
            str = ara::core::StringView("kTransferring");
            break;
        }
        case pkgmgr::TransferStateType::kUpdating: {
            str = ara::core::StringView("kUpdating");
            break;
        }
        case pkgmgr::TransferStateType::kCancelling: {
            str = ara::core::StringView("kCancelling");
            break;
        }
        default: {
            str = ara::core::StringView("Unknown");
            break;
        }
    }
    return str;
}

/// @brief Get UCM subordinate software cluster information
/// @param accessor Activity state accessor
/// @param getSwInfoCB Callback to get UCM software cluster information
/// @return Returns software cluster information or a specific error code
/// @throws no
ara::core::Future< GetSwClusterInfoOutput > CampaignStateBase::GetSwClusterInfo(StateAccessor const& accessor,
                                                                                GetSwInfoCBType const& getSwInfoCB)
{
    std::ignore = accessor;
    std::ignore = getSwInfoCB;
    LOG_ERROR << "the method is not execute, current state=" << CampaignStateTypeToString(state_).data();
    ara::core::Promise< GetSwClusterInfoOutput > promise{};
    promise.SetError(UCMErrorDomainErrc::kOperationNotPermitted);
    return promise.get_future();
}

/// @brief Synchronously calculate which software clusters need to be updated
/// @param accessor Activity state accessor
/// @param availableSoftwarePackages List of newly released software cluster information
/// @param syncSwInfoCB Software cluster information synchronization callback
/// @return Returns list of software clusters that need to be upgraded
/// @throws no
ara::core::Future< SwPackageInventoryOutput > CampaignStateBase::SwPackageInventory(
    StateAccessor const& accessor,
    pkgmgr::SwNameVersionVectorType const& availableSoftwarePackages,
    SyncSwInfoCBType const& syncSwInfoCB)
{
    std::ignore = accessor;
    std::ignore = availableSoftwarePackages;
    std::ignore = syncSwInfoCB;
    LOG_ERROR << "the method is not execute, current state=" << CampaignStateTypeToString(state_).data();
    ara::core::Promise< pkgmgr::VehiclePackageManagement::SwPackageInventoryOutput > promise{};
    promise.SetError(UCMErrorDomainErrc::kOperationNotPermitted);
    return promise.get_future();
}

/// @brief Transfer vehicle package
/// @param accessor Activity state accessor
/// @param size Vehicle package size, in bytes
/// @param vpkgCB Callback to start vehicle package transfer processing
/// @return Returns vehicle package transfer id and block size, or a specific error code
/// @throws no
ara::core::Future< TransferVehiclePackageOutput > CampaignStateBase::TransferVehiclePackage(
    StateAccessor const& accessor, std::uint64_t const& size, VpkgCampaignCBType const& vpkgCB)
{
    std::ignore = accessor;
    std::ignore = size;
    std::ignore = vpkgCB;
    LOG_ERROR << "the method is not execute, current state=" << CampaignStateTypeToString(state_).data();
    ara::core::Promise< TransferVehiclePackageOutput > promise{};
    promise.SetError(UCMErrorDomainErrc::kOperationNotPermitted);
    return promise.get_future();
}

/// @brief Transition to kIdle state and perform related processing
/// @param accessor Activity state accessor
/// @return 0: execution successful, non-zero: execution failed
/// @throws no
int32_t CampaignStateBase::DoIdle(StateAccessor const& accessor)
{
    std::ignore = accessor;
    log_.LogDebug() << "CampaignStateBase::DoIdle(), begin.";
    log_.LogDebug() << "CampaignStateBase::DoIdle(), end.";
    return 1;
}

/// @brief  Activity cancelled or execution failed, transition to kCanceling state and perform related processing
/// @param accessor Activity state accessor
/// @param cancelCause Reason for cancellation
/// @param cancelCB
/// @param promise
/// @return 0: execution successful, non-zero: execution failed
/// @throws no
int32_t CampaignStateBase::OnCancel(StateAccessor const& accessor,
                                    pkgmgr::UCMMasterResolutionType cancelCause,
                                    CancelCBType const& cancelCB,
                                    ara::core::Promise< void >&& promise)
{
    std::ignore = accessor;
    std::ignore = cancelCB;
    log_.LogDebug() << "CampaignStateBase::OnCancel(), begin with cancelCause:"
                    << com::internal::format::Formatter< core::String, pkgmgr::UCMMasterResolutionType >::ToString(
                           padding_, cancelCause)
                           .c_str();
    promise.SetError(UCMErrorDomainErrc::kOperationNotPermitted);
    log_.LogDebug() << "CampaignStateBase::OnCancel(), end with Error:kOperationNotPermitted";
    return 1;
}

/// @brief  Activity exit
/// @throws no
void CampaignStateBase::OnAbort() noexcept
{
    log_.LogDebug() << "CampaignStateBase::OnAbort(), begin.";
    log_.LogDebug() << "CampaignStateBase::OnAbort(), end.";
}

/// @brief Transition to kSoftwarePackage_Transferring state and perform related processing
/// @param accessor Activity state accessor
/// @param spkgCB
/// @return 0: execution successful, non-zero: execution failed
/// @throws no
int32_t CampaignStateBase::DoSoftwareTransfer(StateAccessor const& accessor, SpkgCampaignCBType const& spkgCB)
{
    std::ignore = accessor;
    std::ignore = spkgCB;
    LOG_ERROR << "the method is not execute, current state=" << CampaignStateTypeToString(state_).data();
    return 1;
}

/// @brief Transition to kProcessing state, start coordinating ucm to install software packages (software package transfer exists in this process)
/// @param accessor Activity state accessor
/// @param processCB
/// @return 0: execution successful, non-zero: execution failed
/// @throws no
int32_t CampaignStateBase::DoProcess(StateAccessor const& accessor, ProcessCBType const& processCB)
{
    std::ignore = accessor;
    std::ignore = processCB;
    LOG_ERROR << "the method is not execute, current state=" << CampaignStateTypeToString(state_).data();
    return 1;
}

/// @brief Transition to kActivate state, start activation-related steps
/// @param accessor Activity state accessor
/// @param activateCB
/// @return 0: execution successful, non-zero: execution failed
/// @throws no
int32_t CampaignStateBase::DoActivate(StateAccessor const& accessor, ActivateCBType const& activateCB)
{
    std::ignore = accessor;
    std::ignore = activateCB;
    LOG_ERROR << "the method is not execute, current state=" << CampaignStateTypeToString(state_).data();
    return 1;
}

/// @brief Transition to kVehicleCheck state, start VehicleCheck (OEM specific) related work
/// @param accessor Activity state accessor
/// @param vehicleCheckCB
/// @return 0: execution successful, non-zero: execution failed
/// @throws no
int32_t CampaignStateBase::DoVehicleCheck(StateAccessor const& accessor, VehicleCheckCBType const& vehicleCheckCB)
{
    std::ignore = accessor;
    std::ignore = vehicleCheckCB;
    LOG_ERROR << "the method is not execute, current state=" << CampaignStateTypeToString(state_).data();
    return 1;
}

}  // namespace vpkgmgr
}  // namespace ucm
}  // namespace ara
