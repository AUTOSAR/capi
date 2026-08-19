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
/// @file       campaign_state_sync.h
/// @brief      CampaignStateSync definition
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
/// @unit_name=CampaignStateSync
/// @unit_description=CampaignStateSync definition
/// @endcode
///
/// ================================================================

#ifndef _FSM_CAMPAIGN_STATE_SYNC_H__
#define _FSM_CAMPAIGN_STATE_SYNC_H__

#include <cstdint>
#include <functional>
#include <memory>

#include "campaign_state_base.h"

namespace ara {
namespace ucm {
namespace vpkgmgr {

/// @brief CampaignStateSync
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_Master_00220
/// @trace_id_dd=DD_UCM_Master_00533
/// @needwork = ad
/// @endcode
class CampaignStateSync final : public CampaignStateBase
{
public:
    /// @brief constructor
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00221
    /// @trace_id_dd=DD_UCM_Master_00534
    /// @needwork = ad
    /// @endcode
    CampaignStateSync() : CampaignStateBase{pkgmgr::CampaignStateType::kSyncing} {}
    /// @brief destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00222
    /// @trace_id_dd=DD_UCM_Master_00535
    /// @needwork = ad
    /// @endcode
    ~CampaignStateSync() final = default;

    /// @brief delete copy construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00223
    /// @trace_id_dd=DD_UCM_Master_00536
    /// @needwork = ad
    /// @endcode
    CampaignStateSync(CampaignStateSync const& other) = delete;
    /// @brief delete copy asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00224
    /// @trace_id_dd=DD_UCM_Master_00537
    /// @needwork = ad
    /// @endcode
    CampaignStateSync& operator=(CampaignStateSync const& other) = delete;
    /// @brief delete move construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00225
    /// @trace_id_dd=DD_UCM_Master_00538
    /// @needwork = ad
    /// @endcode
    CampaignStateSync(CampaignStateSync&& other) = delete;
    /// @brief delete move asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00226
    /// @trace_id_dd=DD_UCM_Master_00539
    /// @needwork = ad
    /// @endcode
    CampaignStateSync& operator=(CampaignStateSync&& other) = delete;

    /// @brief DoIdle
    /// @param accessor
    /// @return int
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00227
    /// @trace_id_dd=DD_UCM_Master_00540
    /// @needwork = ad
    /// @endcode
    int32_t DoIdle(StateAccessor const& accessor) final;

private:
    ///pkgmgr::SwNameVersionVectorType availableSoftwarePackages_;.
    ///ara::core::Promise<VpmSkeleton::SwPackageInventoryOutput> promise_;.
};

}  // namespace vpkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // _FSM_CAMPAIGN_STATE_SYNC_H__