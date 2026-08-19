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
/// @file       campaign_state_vehicle_checking.h
/// @brief      CampaignStateVehicleCheck definition
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
/// @unit_name=CampaignStateVehicleCheck
/// @unit_description=CampaignStateVehicleCheck definition
/// @endcode
///
/// ================================================================

#ifndef _FSM_CAMPAIGN_STATE_VEHICLE_CHECKING_H__
#define _FSM_CAMPAIGN_STATE_VEHICLE_CHECKING_H__
#include "campaign_state_base.h"

namespace ara {
namespace ucm {
namespace vpkgmgr {

/// @brief CampaignStateVehicleCheck
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_Master_00180
/// @trace_id_dd=DD_UCM_Master_00492
/// @needwork = ad
/// @endcode
class CampaignStateVehicleCheck final : public CampaignStateBase
{
public:
    /// @brief constructor
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00181
    /// @trace_id_dd=DD_UCM_Master_00493
    /// @needwork = ad
    /// @endcode
    CampaignStateVehicleCheck() : CampaignStateBase{pkgmgr::CampaignStateType::kVehicleChecking} {}
    /// @brief destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00182
    /// @trace_id_dd=DD_UCM_Master_00494
    /// @needwork = ad
    /// @endcode
    ~CampaignStateVehicleCheck() final = default;

    /// @brief delete copy construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00183
    /// @trace_id_dd=DD_UCM_Master_00495
    /// @needwork = ad
    /// @endcode
    CampaignStateVehicleCheck(CampaignStateVehicleCheck const& other) = delete;
    /// @brief delete copy asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00184
    /// @trace_id_dd=DD_UCM_Master_00496
    /// @needwork = ad
    /// @endcode
    CampaignStateVehicleCheck& operator=(CampaignStateVehicleCheck const& other) = delete;
    /// @brief delete move construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00185
    /// @trace_id_dd=DD_UCM_Master_00497
    /// @needwork = ad
    /// @endcode
    CampaignStateVehicleCheck(CampaignStateVehicleCheck&& other) = delete;
    /// @brief delete move asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00186
    /// @trace_id_dd=DD_UCM_Master_00498
    /// @needwork = ad
    /// @endcode
    CampaignStateVehicleCheck& operator=(CampaignStateVehicleCheck&& other) = delete;

    /// @brief DoIdle
    /// @param accessor
    /// @return int
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00187
    /// @trace_id_dd=DD_UCM_Master_00499
    /// @needwork = ad
    /// @endcode
    int32_t DoIdle(StateAccessor const& accessor) final;
    /// @brief OnCancel
    /// @param accessor
    /// @param cancelCause
    /// @param cancelCB
    /// @param promise
    /// @return int
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00188
    /// @trace_id_dd=DD_UCM_Master_00500
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
    /// @trace_id_ad=AD_UCM_Master_00189
    /// @trace_id_dd=DD_UCM_Master_00501
    /// @needwork = ad
    /// @endcode
    void OnAbort() noexcept final;
};

}  // namespace vpkgmgr
}  // namespace ucm
}  // namespace ara

#endif  //_FSM_CAMPAIGN_STATE_VEHICLE_CHECKING_H__