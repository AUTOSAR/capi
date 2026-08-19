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
/// @file       campaign_state_idle.h
/// @brief      CampaignStateIdle
/// @details
/// @date       2024-07-20
/// @author     hanzhibo
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// export_level=/
/// @module_path=/UCM Master/Fsm
/// @interface_level=unit
/// @trace_id_sr=SR_UCM_00035
/// @unit_name=CampaignStateIdle
/// @unit_description=CampaignStateIdle impl
/// @endcode
///
/// ================================================================

#ifndef _FSM_CAMPAIGN_STATE_IDLE_H__
#define _FSM_CAMPAIGN_STATE_IDLE_H__

#include "fsm/states/campaign_state_base.h"

namespace ara {
namespace ucm {
namespace vpkgmgr {

/// @brief CampaignStateIdle
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_Master_00200
/// @trace_id_dd=DD_UCM_Master_00512
/// @needwork = ad
/// @endcode
class CampaignStateIdle final : public CampaignStateBase
{
public:
    /// @brief constructor
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00201
    /// @trace_id_dd=DD_UCM_Master_00513
    /// @needwork = ad
    /// @endcode
    CampaignStateIdle() : CampaignStateBase{pkgmgr::CampaignStateType::kIdle} {}
    /// @brief destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00202
    /// @trace_id_dd=DD_UCM_Master_00514
    /// @needwork = ad
    /// @endcode
    ~CampaignStateIdle() final = default;

    /// @brief delete copy construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00203
    /// @trace_id_dd=DD_UCM_Master_00515
    /// @needwork = ad
    /// @endcode
    CampaignStateIdle(CampaignStateIdle const& other) = delete;
    /// @brief delete copy asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00204
    /// @trace_id_dd=DD_UCM_Master_00516
    /// @needwork = ad
    /// @endcode
    CampaignStateIdle& operator=(CampaignStateIdle const& other) = delete;
    /// @brief delete move construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00205
    /// @trace_id_dd=DD_UCM_Master_00517
    /// @needwork = ad
    /// @endcode
    CampaignStateIdle(CampaignStateIdle&& other) = delete;
    /// @brief delete move asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00206
    /// @trace_id_dd=DD_UCM_Master_00518
    /// @needwork = ad
    /// @endcode
    CampaignStateIdle& operator=(CampaignStateIdle&& other) = delete;

    /// @brief GetSwClusterInfo
    /// @param accessor
    /// @param getSwInfoCB
    /// @return future
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00207
    /// @trace_id_dd=DD_UCM_Master_00519
    /// @needwork = ad
    /// @endcode
    ara::core::Future< GetSwClusterInfoOutput > GetSwClusterInfo(StateAccessor const& accessor,
                                                                 GetSwInfoCBType const& getSwInfoCB) final;

    /// @brief SwPackageInventory
    /// @param accessor
    /// @param availableSoftwarePackages
    /// @param syncSwInfoCB
    /// @return future
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00208
    /// @trace_id_dd=DD_UCM_Master_00520
    /// @needwork = ad
    /// @endcode
    ara::core::Future< SwPackageInventoryOutput > SwPackageInventory(
        StateAccessor const& accessor,
        pkgmgr::SwNameVersionVectorType const& availableSoftwarePackages,
        SyncSwInfoCBType const& syncSwInfoCB) final;

    /// @brief TransferVehiclePackage
    /// @param accessor
    /// @param size
    /// @param vpkgCB
    /// @return future
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00209
    /// @trace_id_dd=DD_UCM_Master_00521
    /// @needwork = ad
    /// @endcode
    ara::core::Future< TransferVehiclePackageOutput > TransferVehiclePackage(StateAccessor const& accessor,
                                                                             std::uint64_t const& size,
                                                                             VpkgCampaignCBType const& vpkgCB) final;
};

}  // namespace vpkgmgr
}  // namespace ucm
}  // namespace ara

#endif  //_FSM_CAMPAIGN_STATE_IDLE_H__