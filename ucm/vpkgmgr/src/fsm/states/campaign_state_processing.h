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
/// @file       campaign_state_processing.h
/// @brief      CampaignStateProcessing definition
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
/// @unit_name=CampaignStateProcessing
/// @unit_description=CampaignStateProcessing definition
/// @endcode
///
/// ================================================================

#ifndef VPKGMGR_SRC_FSM_CAMPAIGN_STATE_PROCESSING_H_
#define VPKGMGR_SRC_FSM_CAMPAIGN_STATE_PROCESSING_H_

#include "campaign_state_base.h"

namespace ara {
namespace ucm {
namespace vpkgmgr {

/// @brief CampaignStateProcessing
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_Master_00260
/// @trace_id_dd=DD_UCM_Master_00576
/// @needwork = ad
/// @endcode
class CampaignStateProcessing final : public CampaignStateBase
{
public:
    /// @brief constructor
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00261
    /// @trace_id_dd=DD_UCM_Master_00577
    /// @needwork = ad
    /// @endcode
    CampaignStateProcessing() : CampaignStateBase{pkgmgr::CampaignStateType::kProcessing} {}
    /// @brief destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00262
    /// @trace_id_dd=DD_UCM_Master_00578
    /// @needwork = ad
    /// @endcode
    ~CampaignStateProcessing() final = default;

    /// @brief delete copy construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00263
    /// @trace_id_dd=DD_UCM_Master_00579
    /// @needwork = ad
    /// @endcode
    CampaignStateProcessing(CampaignStateProcessing const& other) = delete;
    /// @brief delete copy asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00264
    /// @trace_id_dd=DD_UCM_Master_00580
    /// @needwork = ad
    /// @endcode
    CampaignStateProcessing& operator=(CampaignStateProcessing const& other) = delete;
    /// @brief delete move construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00265
    /// @trace_id_dd=DD_UCM_Master_00581
    /// @needwork = ad
    /// @endcode
    CampaignStateProcessing(CampaignStateProcessing&& other) = delete;
    /// @brief delete move asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00266
    /// @trace_id_dd=DD_UCM_Master_00582
    /// @needwork = ad
    /// @endcode
    CampaignStateProcessing& operator=(CampaignStateProcessing&& other) = delete;

    /// @brief DoSoftwareTransfer
    /// @param accessor
    /// @param spkgCB
    /// @return int
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00267
    /// @trace_id_dd=DD_UCM_Master_00583
    /// @needwork = ad
    /// @endcode
    int32_t DoSoftwareTransfer(StateAccessor const& accessor, SpkgCampaignCBType const& spkgCB) final;
    /// @brief DoActivate
    /// @param accessor
    /// @param activateCB
    /// @return int
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00268
    /// @trace_id_dd=DD_UCM_Master_00584
    /// @needwork = ad
    /// @endcode
    int32_t DoActivate(StateAccessor const& accessor, ActivateCBType const& activateCB) final;
    /// @brief OnCancel
    /// @param accessor
    /// @param cancelCause
    /// @param cancelCB
    /// @param promise
    /// @return int
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00269
    /// @trace_id_dd=DD_UCM_Master_00585
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
    /// @trace_id_ad=AD_UCM_Master_00270
    /// @trace_id_dd=DD_UCM_Master_00586
    /// @needwork = ad
    /// @endcode
    void OnAbort() noexcept final;
};

}  // namespace vpkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // VPKGMGR_SRC_FSM_CAMPAIGN_STATE_PROCESSING_H_