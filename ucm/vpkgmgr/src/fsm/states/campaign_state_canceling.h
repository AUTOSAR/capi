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
/// @file       campaign_state_canceling.h
/// @brief      CampaignStateCanceling
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
/// @unit_name=CampaignStateCanceling
/// @unit_description=CampaignStateCanceling impl
/// @endcode
///
/// ================================================================

#ifndef VPKGMGR_SRC_FSM_CAMPAIGN_STATE_CANCELING_H_
#define VPKGMGR_SRC_FSM_CAMPAIGN_STATE_CANCELING_H_

#include "campaign_state_base.h"

namespace ara {
namespace ucm {
namespace vpkgmgr {

/// @brief CampaignStateCanceling
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_Master_00210
/// @trace_id_dd=DD_UCM_Master_00522
/// @needwork = ad
/// @endcode
class CampaignStateCanceling final : public CampaignStateBase
{
public:
    /// @brief constructor
    /// @param cancelCause
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00211
    /// @trace_id_dd=DD_UCM_Master_00523
    /// @needwork = ad
    /// @endcode
    explicit CampaignStateCanceling(pkgmgr::UCMMasterResolutionType const cancelCause)
        : CampaignStateBase{pkgmgr::CampaignStateType::kCancelling}, cancelCause_{cancelCause}
    {
    }
    /// @brief destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00212
    /// @trace_id_dd=DD_UCM_Master_00524
    /// @needwork = ad
    /// @endcode
    ~CampaignStateCanceling() final = default;

    /// @brief delete copy construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00213
    /// @trace_id_dd=DD_UCM_Master_00525
    /// @needwork = ad
    /// @endcode
    CampaignStateCanceling(CampaignStateCanceling const& other) = delete;
    /// @brief delete copy asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00214
    /// @trace_id_dd=DD_UCM_Master_00526
    /// @needwork = ad
    /// @endcode
    CampaignStateCanceling& operator=(CampaignStateCanceling const& other) = delete;
    /// @brief delete move construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00215
    /// @trace_id_dd=DD_UCM_Master_00527
    /// @needwork = ad
    /// @endcode
    CampaignStateCanceling(CampaignStateCanceling&& other) = delete;
    /// @brief delete move asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00216
    /// @trace_id_dd=DD_UCM_Master_00528
    /// @needwork = ad
    /// @endcode
    CampaignStateCanceling& operator=(CampaignStateCanceling&& other) = delete;

    /// @brief DoIdle
    /// @param accessor
    /// @return 0 success, non-0 fail
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00217
    /// @trace_id_dd=DD_UCM_Master_00529
    /// @needwork = ad
    /// @endcode
    int32_t DoIdle(StateAccessor const& accessor) final;
    /// @brief OnAbort
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00218
    /// @trace_id_dd=DD_UCM_Master_00530
    /// @needwork = ad
    /// @endcode
    void OnAbort() noexcept final;

    /// @brief GetCancelCause
    /// @return UCMMasterResolutionType
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00219
    /// @trace_id_dd=DD_UCM_Master_00531
    /// @needwork = ad
    /// @endcode
    pkgmgr::UCMMasterResolutionType GetCancelCause() const noexcept final { return cancelCause_; }

private:
    /// @brief cancelCause_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00532
    /// @needwork = dda
    /// @endcode
    pkgmgr::UCMMasterResolutionType cancelCause_;
};

}  // namespace vpkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // VPKGMGR_SRC_FSM_CAMPAIGN_STATE_CANCELING_H_