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
/// @file       campaign_state_corrupted_status_file.h
/// @brief      CampaignStateCorruptedStatusFile
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
/// @unit_name=CampaignStateCorruptedStatusFile
/// @unit_description=CampaignStateCorruptedStatusFile impl
/// @endcode
///
/// ================================================================

#ifndef _FSM_CAMPAIGN_STATE_CORRUPTED_STATUS_FILE_H__
#define _FSM_CAMPAIGN_STATE_CORRUPTED_STATUS_FILE_H__

#include "ara/ucm/pkgmgr/impl_type_campaignstatetype.h"
#include "fsm/states/campaign_state_base.h"

namespace ara {
namespace ucm {
namespace vpkgmgr {

/// @brief CampaignStateCorruptedStatusFile
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_Master_00173
/// @trace_id_dd=DD_UCM_Master_00485
/// @needwork = ad
/// @endcode
class CampaignStateCorruptedStatusFile final : public CampaignStateBase
{
public:
    /// @brief constructor
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00174
    /// @trace_id_dd=DD_UCM_Master_00486
    /// @needwork = ad
    /// @endcode
    CampaignStateCorruptedStatusFile() : CampaignStateBase{ara::ucm::pkgmgr::CampaignStateType::kCorruptedStatusFile} {}
    /// @brief destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00175
    /// @trace_id_dd=DD_UCM_Master_00487
    /// @needwork = ad
    /// @endcode
    ~CampaignStateCorruptedStatusFile() final = default;

    /// @brief delete copy construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00176
    /// @trace_id_dd=DD_UCM_Master_00488
    /// @needwork = ad
    /// @endcode
    CampaignStateCorruptedStatusFile(CampaignStateCorruptedStatusFile const& other) = delete;
    /// @brief delete copy asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00177
    /// @trace_id_dd=DD_UCM_Master_00489
    /// @needwork = ad
    /// @endcode
    CampaignStateCorruptedStatusFile& operator=(CampaignStateCorruptedStatusFile const& other) = delete;
    /// @brief delete move construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00178
    /// @trace_id_dd=DD_UCM_Master_00490
    /// @needwork = ad
    /// @endcode
    CampaignStateCorruptedStatusFile(CampaignStateCorruptedStatusFile&& other) = delete;
    /// @brief delete move asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00179
    /// @trace_id_dd=DD_UCM_Master_00491
    /// @needwork = ad
    /// @endcode
    CampaignStateCorruptedStatusFile& operator=(CampaignStateCorruptedStatusFile&& other) = delete;
};

}  // namespace vpkgmgr
}  // namespace ucm
}  // namespace ara

#endif  //_FSM_CAMPAIGN_STATE_CORRUPTED_STATUS_FILE_H__