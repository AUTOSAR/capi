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
/// @file       campaign_state_sync.cpp
/// @brief      CampaignStateSync implementation
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
/// @unit_name=CampaignStateSync
/// @unit_description=CampaignStateSync implementation
/// @endcode
///
/// ================================================================

#include "fsm/states/campaign_state_sync.h"

#include <ara/core/string.h>
#include <ara/core/vector.h>

#include <algorithm>

#include "fsm/states/campaign_state_idle.h"
#include "utils/version.h"

namespace ara {
namespace ucm {
namespace vpkgmgr {

/// @brief DoIdle
/// @param accessor
/// @return int
/// @throws no
int32_t CampaignStateSync::DoIdle(StateAccessor const& accessor)
{
    log_.LogDebug() << "CampaignStateSync::DoIdle(), begin.";

    std::unique_ptr< CampaignStateBase > stateUPtr{std::make_unique< CampaignStateIdle >()};
    accessor.Reset(std::move(stateUPtr));

    LOG_DEBUG << "CampaignStateSync::DoIdle(), end.";
    return 0;
}

}  // namespace vpkgmgr
}  // namespace ucm
}  // namespace ara