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
/// @file       trigger_vehicle_announcement_proxy.cpp
/// @brief
/// @details
/// @date       2024-12-27
/// @author     zhanglipeng
/// @version    1.2.0
///
/// ================================================================

#include "trigger_vehicle_announcement_proxy.h"

#include "ara/diag/diag_error_domain.h"
#include "serialization/serialization.h"

namespace isoft {
namespace dm {
namespace dic {

constexpr uint8_t kFuncIDTriggerVehicleAnnouncement{1U};

TriggerVehicleAnnouncementProxy::TriggerVehicleAnnouncementProxy(uint16_t const& instanceId,
                                                                 uint32_t const& serviceInstanceId)
    : BussinessClientProxy{serviceInstanceId, instanceId}
{
}

/// @brief Register callback function for AA side to set the status of operation cycle
/// @param Callback function to be registered
/// @return Return void for successful registration
ara::core::Result< void > TriggerVehicleAnnouncementProxy::TriggerVehicleAnnouncement(std::uint8_t networkInterfaceId)
{
    static uint8_t s_CallId{0U};
    ara::core::Result< void > const result{
        SyncCallFunc< void >(kFuncIDTriggerVehicleAnnouncement, s_CallId, networkInterfaceId)};
    if (result.HasValue()) {
        return ara::core::Result< void >::FromValue();
    }
    return ara::core::Result< void >::FromError(result.Error());
}

}  // namespace dic
}  // namespace dm
}  // namespace isoft