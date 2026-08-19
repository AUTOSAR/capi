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
/// @file       campaign_state_idle.cpp
/// @brief      CampaignStateIdle impl
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
/// @unit_name=CampaignStateIdle
/// @unit_description=CampaignStateIdle impl
/// @endcode
///
/// ================================================================

#include "fsm/states/campaign_state_idle.h"

#include "fsm/states/campaign_state_sync.h"
#include "fsm/states/campaign_state_vpkgtransfer.h"
#include "utils/ucmm_log.h"

namespace ara {
namespace ucm {
namespace vpkgmgr {

/// @brief GetSwClusterInfo
/// @param accessor
/// @param getSwInfoCB
/// @return future
/// @throws no
ara::core::Future< GetSwClusterInfoOutput > CampaignStateIdle::GetSwClusterInfo(StateAccessor const& accessor,
                                                                                GetSwInfoCBType const& getSwInfoCB)
{
    std::unique_ptr< CampaignStateBase > stateUPtr{std::make_unique< CampaignStateSync >()};
    accessor.Reset(std::move(stateUPtr));

    ara::core::Future< GetSwClusterInfoOutput > future{};
    {
        ara::core::Promise< GetSwClusterInfoOutput > promise{};
        future      = promise.get_future();
        std::ignore = getSwInfoCB(std::move(promise));
    }

    return future;
}

/// @brief SwPackageInventory
/// @param accessor
/// @param availableSoftwarePackages
/// @param syncSwInfoCB
/// @return future
/// @throws no
ara::core::Future< SwPackageInventoryOutput > CampaignStateIdle::SwPackageInventory(
    StateAccessor const& accessor,
    pkgmgr::SwNameVersionVectorType const& availableSoftwarePackages,
    SyncSwInfoCBType const& syncSwInfoCB)
{
    std::unique_ptr< CampaignStateBase > stateUPtr{std::make_unique< CampaignStateSync >()};
    accessor.Reset(std::move(stateUPtr));

    ara::core::Future< pkgmgr::VehiclePackageManagement::SwPackageInventoryOutput > future{};
    {
        ara::core::Promise< pkgmgr::VehiclePackageManagement::SwPackageInventoryOutput > promise{};
        future = promise.get_future();
        if (syncSwInfoCB) {
            syncSwInfoCB(availableSoftwarePackages, std::move(promise));
        }
    }

    return future;
}

/// @brief TransferVehiclePackage
/// @param accessor
/// @param size
/// @param vpkgCB
/// @return future
/// @throws no
ara::core::Future< TransferVehiclePackageOutput > CampaignStateIdle::TransferVehiclePackage(
    StateAccessor const& accessor, std::uint64_t const& size, VpkgCampaignCBType const& vpkgCB)
{
    std::unique_ptr< CampaignStateBase > stateUPtr{std::make_unique< CampaignStateVpkgTransfer >()};
    accessor.Reset(std::move(stateUPtr));  // Will transition to VEHICLEPACKAGE_TRANSFERRING state

    ara::core::Future< pkgmgr::VehiclePackageManagement::TransferVehiclePackageOutput > future{};
    {
        ara::core::Promise< pkgmgr::VehiclePackageManagement::TransferVehiclePackageOutput > promise{};
        future = promise.get_future();
        if (vpkgCB) {
            std::ignore = vpkgCB(size, std::move(promise));
        }
    }

    return future;
}

}  // namespace vpkgmgr
}  // namespace ucm
}  // namespace ara
