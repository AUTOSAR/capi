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
/// @file       campaign_state_activating.h
/// @brief      CampaignStateActivating
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
/// @unit_name=CampaignStateActivating
/// @unit_description=CampaignStateActivating impl
/// @endcode
///
/// ================================================================

#ifndef VPKGMGR_SRC_FSM_CAMPAIGN_STATE_ACTIVATING_H_
#define VPKGMGR_SRC_FSM_CAMPAIGN_STATE_ACTIVATING_H_

#include "campaign_state_base.h"

namespace ara {
namespace ucm {
namespace vpkgmgr {

/// @brief CampaignStateActivating
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_Master_00163
/// @trace_id_dd=DD_UCM_Master_00475
/// @needwork = ad
/// @endcode
class CampaignStateActivating final : public CampaignStateBase
{
public:
    /// @brief constructor
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00164
    /// @trace_id_dd=DD_UCM_Master_00476
    /// @needwork = ad
    /// @endcode
    CampaignStateActivating() : CampaignStateBase{pkgmgr::CampaignStateType::kActivating} {}
    /// @brief destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00165
    /// @trace_id_dd=DD_UCM_Master_00477
    /// @needwork = ad
    /// @endcode
    ~CampaignStateActivating() final = default;

    /// @brief delete copy construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00166
    /// @trace_id_dd=DD_UCM_Master_00478
    /// @needwork = ad
    /// @endcode
    CampaignStateActivating(CampaignStateActivating const& other) = delete;
    /// @brief delete copy asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00167
    /// @trace_id_dd=DD_UCM_Master_00479
    /// @needwork = ad
    /// @endcode
    CampaignStateActivating& operator=(CampaignStateActivating const& other) = delete;
    /// @brief delete move construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00168
    /// @trace_id_dd=DD_UCM_Master_00480
    /// @needwork = ad
    /// @endcode
    CampaignStateActivating(CampaignStateActivating&& other) = delete;
    /// @brief delete move asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00169
    /// @trace_id_dd=DD_UCM_Master_00481
    /// @needwork = ad
    /// @endcode
    CampaignStateActivating& operator=(CampaignStateActivating&& other) = delete;

    /// @brief DoVehicleCheck
    /// @param accessor
    /// @param vehicleCheckCB
    /// @return int
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00170
    /// @trace_id_dd=DD_UCM_Master_00482
    /// @needwork = ad
    /// @endcode
    int32_t DoVehicleCheck(StateAccessor const& accessor, VehicleCheckCBType const& vehicleCheckCB) final;
    /// @brief OnCancel
    /// @param accessor
    /// @param cancelCause
    /// @param cancelCB
    /// @param promise
    /// @return int
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00171
    /// @trace_id_dd=DD_UCM_Master_00483
    /// @needwork = ad
    /// @endcode
    int32_t OnCancel(StateAccessor const& accessor,
                     pkgmgr::UCMMasterResolutionType cancelCause,
                     CancelCBType const& cancelCB,
                     ara::core::Promise< void >&& promise) final;
    /// @brief OnAbort
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00172
    /// @trace_id_dd=DD_UCM_Master_00484
    /// @needwork = ad
    /// @endcode
    void OnAbort() noexcept final;
};

}  // namespace vpkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // VPKGMGR_SRC_FSM_CAMPAIGN_STATE_ACTIVATING_H_