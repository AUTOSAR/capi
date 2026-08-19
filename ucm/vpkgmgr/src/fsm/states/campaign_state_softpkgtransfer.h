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
/// @file       campaign_state_softpkgtransfer.h
/// @brief      CampaignStateSoftpkgTransfer definition
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
/// @unit_name=CampaignStateSoftpkgTransfer
/// @unit_description=CampaignStateSoftpkgTransfer definition
/// @endcode
///
/// ================================================================

#ifndef VPKGMGR_SRC_FSM_CAMPAIGN_STATE_SOFTPKGTRANSFER_H_
#define VPKGMGR_SRC_FSM_CAMPAIGN_STATE_SOFTPKGTRANSFER_H_

#include "campaign_state_base.h"

namespace ara {
namespace ucm {
namespace vpkgmgr {

/// @brief CampaignStateSoftpkgTransfer
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_Master_00190
/// @trace_id_dd=DD_UCM_Master_00502
/// @needwork = ad
/// @endcode
class CampaignStateSoftpkgTransfer final : public CampaignStateBase
{
public:
    /// @brief constructor
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00191
    /// @trace_id_dd=DD_UCM_Master_00503
    /// @needwork = ad
    /// @endcode
    CampaignStateSoftpkgTransfer() : CampaignStateBase{pkgmgr::CampaignStateType::kSoftwarePackage_Transferring} {}
    /// @brief destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00192
    /// @trace_id_dd=DD_UCM_Master_00504
    /// @needwork = ad
    /// @endcode
    ~CampaignStateSoftpkgTransfer() final = default;

    /// @brief delete copy construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00193
    /// @trace_id_dd=DD_UCM_Master_00505
    /// @needwork = ad
    /// @endcode
    CampaignStateSoftpkgTransfer(CampaignStateSoftpkgTransfer const& other) = delete;
    /// @brief delete copy asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00194
    /// @trace_id_dd=DD_UCM_Master_00506
    /// @needwork = ad
    /// @endcode
    CampaignStateSoftpkgTransfer& operator=(CampaignStateSoftpkgTransfer const& other) = delete;
    /// @brief delete move construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00195
    /// @trace_id_dd=DD_UCM_Master_00507
    /// @needwork = ad
    /// @endcode
    CampaignStateSoftpkgTransfer(CampaignStateSoftpkgTransfer&& other) = delete;
    /// @brief delete move asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00196
    /// @trace_id_dd=DD_UCM_Master_00508
    /// @needwork = ad
    /// @endcode
    CampaignStateSoftpkgTransfer& operator=(CampaignStateSoftpkgTransfer&& other) = delete;

    /// @brief DoProcess
    /// @param accessor
    /// @param processCB
    /// @return int
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00197
    /// @trace_id_dd=DD_UCM_Master_00509
    /// @needwork = ad
    /// @endcode
    int32_t DoProcess(StateAccessor const& accessor, ProcessCBType const& processCB) final;
    /// @brief OnCancel
    /// @param accessor
    /// @param cancelCause
    /// @param cancelCB
    /// @param promise
    /// @return int
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00198
    /// @trace_id_dd=DD_UCM_Master_00510
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
    /// @trace_id_ad=AD_UCM_Master_00199
    /// @trace_id_dd=DD_UCM_Master_00511
    /// @needwork = ad
    /// @endcode
    void OnAbort() noexcept final;
};

}  // namespace vpkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // VPKGMGR_SRC_FSM_CAMPAIGN_STATE_SOFTPKGTRANSFER_H_