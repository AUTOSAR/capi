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
/// @file       health_channel_supervision.cpp
/// @brief
/// @details
/// @date       2024-06-20
/// @author     wangyanlong
/// @version    1.2.0
///
/// ================================================================

#include "ara/phm/internal/hcmanager/health_channel_supervision.h"

#include "ara/phm/internal/phm_log.h"

namespace ara {
namespace phm {
namespace internal {
/// @brief Creation of a HealthChannelSupervision
/// @param conf conf of health channel supervision.
/// @param healthChannelEntity health channel supervised by this health channel supervision.
HealthChannelSupervision::HealthChannelSupervision(HealthChannelSupervisionConf const& conf,
                                                   std::shared_ptr< HealthChannelEntity > healthChannelEntity) noexcept
    : kName{conf.shortName}
    , kHcInterfaceIdentifier{conf.hcInterfaceIdentifier}
    , kMetaModelIdentifier{conf.metaModelIdentifier}
    , kRecoveryNotificationTimeout{SECOND_TO_MS(conf.recoveryNotification.recoveryNotificationTimeout)}
    , kRecoveryNotificationRetry{conf.recoveryNotification.recoveryNotificationRetry}
    , healthChannelEntity_{std::move(healthChannelEntity)}
    , offered_{false}

{
    LOG_INFO << "create a HealthChannelSupervision, name " << kName.c_str() << ", identifier "
             << kHcInterfaceIdentifier.c_str() << ", metaModelIdentifier " << kMetaModelIdentifier.c_str()
             << ", recovery notification timeout " << kRecoveryNotificationTimeout << ", recovery notification retry "
             << kRecoveryNotificationRetry;
    offered_ = true;
    if (healthChannelEntity_ == nullptr) {
        LOG_FATAL << "healthChannelEntity_ is null";
        std::terminate();
    }
}

/// @brief Returns flag about whether health status need recover
/// @param status id of health status
/// @return true, health status need recover；false, health status does not need recover.
bool HealthChannelSupervision::IsHealthStatusNeedRecover(uint32_t const& status) noexcept
{
    std::shared_ptr< HealthChannelStatus > statusObj{healthChannelEntity_->GetHealthChannelStatus(status)};
    if (statusObj) {
        return statusObj->GetTrigger();
    }
    LOG_WARN << "status " << status << " obj is null";
    return false;
}

/// TODO This needs to be modified later because the meaning is unclear
/// @brief Returns the meta mode identifier.
/// @return the meta mode identifier.

ara::core::String HealthChannelSupervision::GetMetaModelIdentifier() const noexcept { return kMetaModelIdentifier; }

/// @brief Returns recover timeout.
/// @return recover timeout.
int32_t HealthChannelSupervision::GetRecoveryNotificationTimeout() const noexcept
{
    return kRecoveryNotificationTimeout;
}

/// @brief Returns max retry.
/// @return max retry.
uint32_t HealthChannelSupervision::GetRecoveryNotificationRetry() const noexcept { return kRecoveryNotificationRetry; }

/// @brief Returns the name of health channel supervision.
/// @return the name of health channel supervision.
ara::core::String HealthChannelSupervision::GetName() const noexcept { return kName; }

/// @brief Returns the instance specifier of health channel.
/// @return the instance specifier of health channel.
ara::core::String HealthChannelSupervision::GetHcInterfaceIdentifier() const noexcept { return kHcInterfaceIdentifier; }

/// @brief Returns the flag of whether this supervision is offereed.
/// @return the flag of whether this supervision is offereed.
bool HealthChannelSupervision::IsOffered() const noexcept { return offered_; }

/// @brief Set the flag of whether this supervision is offereed.
/// @param offer true, offered; false not offered.
void HealthChannelSupervision::SetOffer(bool const offer) noexcept
{
    LOG_INFO << kName.c_str() << " offer status " << offered_ << " --> " << offer;
    offered_ = offer;
}

}  // namespace internal
}  // namespace phm
}  // namespace ara