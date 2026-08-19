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
/// @file       campaign_history.cpp
/// @brief
/// @details
/// @date       2023-11-14
/// @author     zhaoyunfei
/// @version    1.2.0
///
/// ================================================================

#include <ara/ucm/pkgmgr/vehiclepackagemanagement_common.h>
namespace ara {
namespace ucm {
namespace vpkgmgr {

ara::core::StringView ActionToString(const pkgmgr::SwClusterStateType& action)
{
    switch (action) {
        case pkgmgr::SwClusterStateType::kPresent:
            return ara::core::StringView("present");
        case pkgmgr::SwClusterStateType::kAdded:
            return ara::core::StringView("install");
        case pkgmgr::SwClusterStateType::kUpdated:
            return ara::core::StringView("update");
        case pkgmgr::SwClusterStateType::kRemoved:
            return ara::core::StringView("remove");
        default:
            break;
    }
    return ara::core::StringView("unkown");
}

// CampaignHistory::CampaignHistory(/* args */) {}
// CampaignHistory::~CampaignHistory() {}

}  // namespace vpkgmgr
}  // namespace ucm
}  // namespace ara