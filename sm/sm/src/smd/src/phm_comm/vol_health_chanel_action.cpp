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
/// @file       vol_health_chanel_action.cpp
/// @brief      HealthChannelAction for voltage health channel.
/// @details
/// @date       2024-06-25
/// @author     lianglongxiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/StateManagement/PHMComm
/// @interface_level=uint
/// @trace_id_sr=SR_SM_08001
/// @unit_name=VolHealthChannelAction
/// @unit_description=HealthChannelAction for voltage health channel.
/// @endcode
///
/// ================================================================

#include "phm_comm/vol_health_chanel_action.h"

#include "define.h"
#include "helper.h"

namespace ara {
namespace sm {
namespace phm_comm {

/// @brief _volHealthStatusesToString
/// @param status the Health Statuses
/// @return string
/// @code{.isoft}
/// @interface_level=none
/// @needwork = dd
/// @endcode
core::String const VolHealthChannelAction::_volHealthStatusesToString(VolHealthStatuses const status) const noexcept
{
    std::ignore = log_;
    core::String healthStatuses{};
    switch (status) {
        case VolHealthStatuses::kLow: {
            healthStatuses = "Low";
        } break;

        case VolHealthStatuses::kOk: {
            healthStatuses = "Ok";
        } break;

        case VolHealthStatuses::kHigh: {
            healthStatuses = "High";
        } break;

        default: {
            healthStatuses = "Unknown";
        } break;
    }
    return healthStatuses;
}

/// @brief RecoveryHandler to be invoked by PHM.
/// @param volHealthStatusId The identifier representing the Health Status.
void VolHealthChannelAction::RecoveryHandler(VolHealthStatuses volHealthStatusId) noexcept
{
    log_.LogInfo() << "VolHealthChannelAction::RecoveryHandler(), volHealthStatusId number:"
                   << static_cast< uint32_t >(volHealthStatusId)
                   << "volHealthStatusId string:" << _volHealthStatusesToString(volHealthStatusId).c_str();

    // Corresponding promise for the request
    core::Promise< void > promise;
    ara::core::Future< void, ara::core::ErrorCode > const future{promise.get_future()};
    DeliverRecoveryNotificationRequest(common::PHMHealthStatusType::kVol, volHealthStatusId, std::move(promise));

    std::ignore = future;
}

/// @brief Destructor function
/// @code{.isoft}
/// @interface_level=unit
/// @needwork = ad
/// @endcode
VolHealthChannelAction::~VolHealthChannelAction() noexcept  // NOLINT
{
    log_.LogInfo() << "VolHealthChannelAction::~VolHealthChannelAction()";
}

}  // namespace phm_comm
}  // namespace sm
}  // namespace ara
