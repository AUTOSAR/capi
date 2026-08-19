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
/// @file       health_channel_entity.cpp
/// @brief
/// @details
/// @date       2024-06-20
/// @author     wangyanlong
/// @version    1.2.0
///
/// ================================================================

#include "ara/phm/internal/hcmanager/health_channel_entity.h"

#include "ara/phm/internal/phm_log.h"

namespace ara {
namespace phm {
namespace internal {
/// @brief Creation of a HealthChannelEntity.
/// @param name name of health channel.
/// @param healthChannelStatus objects of health channel status belongs to health channel.
HealthChannelEntity::HealthChannelEntity(
    ara::core::String name,
    ara::core::Vector< std::shared_ptr< HealthChannelStatus > > const& healthChannelStatus) noexcept
    : kName{std::move(name)}, healthStatus_{}
{
    LOG_INFO << "create HealthChannelEntity, name " << kName.c_str() << ", status count " << healthChannelStatus.size();
    for (std::shared_ptr< HealthChannelStatus > const& healthStatus : healthChannelStatus) {
        LOG_INFO << "add health status " << healthStatus->GetStatusId();
        healthStatus_[healthStatus->GetStatusId()] = healthStatus;
    }
}

/// @brief Returns HealthStatus of a health status id.
/// @param status id of health status.
/// @return Object of health status.
std::shared_ptr< HealthChannelStatus > HealthChannelEntity::GetHealthChannelStatus(HealthStatus const& status) noexcept
{
    LOG_INFO << "GetHealthChannelStatus, status " << status;
    if (healthStatus_.count(status) != 0U) {
        return healthStatus_[status];
    }
    LOG_ERROR << "status id " << status << " of health channel " << kName.c_str() << " not exist";
    return std::shared_ptr< HealthChannelStatus >{nullptr};
}

/// @brief Returns the name of health channel.
/// @return the name of health channel.
ara::core::String HealthChannelEntity::GetName() const noexcept { return kName; }

}  // namespace internal
}  // namespace phm
}  // namespace ara