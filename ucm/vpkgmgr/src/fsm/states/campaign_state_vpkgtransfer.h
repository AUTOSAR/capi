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
/// @file       campaign_state_vpkgtransfer.h
/// @brief      CampaignStateVpkgTransfer impl
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
/// @unit_name=CampaignStateVpkgTransfer
/// @unit_description=CampaignStateVpkgTransfer impl
/// @endcode
///
/// ================================================================

#ifndef _FSM_CAMPAIGN_STATE_VPKG_TRANSFER_H__
#define _FSM_CAMPAIGN_STATE_VPKG_TRANSFER_H__

#include "campaign_state_base.h"

namespace ara {
namespace ucm {
namespace vpkgmgr {

/// @brief CampaignStateVpkgTransfer
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_Master_00228
/// @trace_id_dd=DD_UCM_Master_00541
/// @needwork = ad
/// @endcode
class CampaignStateVpkgTransfer final : public CampaignStateBase
{
public:
    /// @brief constructor
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00229
    /// @trace_id_dd=DD_UCM_Master_00542
    /// @needwork = ad
    /// @endcode
    CampaignStateVpkgTransfer() : CampaignStateBase{pkgmgr::CampaignStateType::kVehiclePackageTransferring} {}
    /// @brief destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00230
    /// @trace_id_dd=DD_UCM_Master_00543
    /// @needwork = ad
    /// @endcode
    ~CampaignStateVpkgTransfer() final = default;

    /// @brief delete copy construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00231
    /// @trace_id_dd=DD_UCM_Master_00544
    /// @needwork = ad
    /// @endcode
    CampaignStateVpkgTransfer(CampaignStateVpkgTransfer const& other) = delete;
    /// @brief delete copy asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00232
    /// @trace_id_dd=DD_UCM_Master_00545
    /// @needwork = ad
    /// @endcode
    CampaignStateVpkgTransfer& operator=(CampaignStateVpkgTransfer const& other) = delete;
    /// @brief delete move construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00233
    /// @trace_id_dd=DD_UCM_Master_00546
    /// @needwork = ad
    /// @endcode
    CampaignStateVpkgTransfer(CampaignStateVpkgTransfer&& other) = delete;
    /// @brief delete move asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00234
    /// @trace_id_dd=DD_UCM_Master_00547
    /// @needwork = ad
    /// @endcode
    CampaignStateVpkgTransfer& operator=(CampaignStateVpkgTransfer&& other) = delete;

    /// @brief DoSoftwareTransfer
    /// @param accessor
    /// @param spkgCB
    /// @return int
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00235
    /// @trace_id_dd=DD_UCM_Master_00548
    /// @needwork = ad
    /// @endcode
    int32_t DoSoftwareTransfer(StateAccessor const& accessor, SpkgCampaignCBType const& spkgCB) final;

    /// @brief Transition to kIdle state and perform related processing
    /// @param accessor Activity state accessor
    /// @return 0: execution successful, non-zero: execution failed
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00253
    /// @trace_id_dd=DD_UCM_Master_00566
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
    /// @trace_id_ad=AD_UCM_Master_00236
    /// @trace_id_dd=DD_UCM_Master_00549
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
    /// @trace_id_ad=AD_UCM_Master_00237
    /// @trace_id_dd=DD_UCM_Master_00550
    /// @needwork = ad
    /// @endcode
    void OnAbort() noexcept final;
};

}  // namespace vpkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // _FSM_CAMPAIGN_STATE_VPKG_TRANSFER_H__