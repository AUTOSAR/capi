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
/// @file       health_channel_action_impl.cpp
/// @brief      Inherit PHM's HealthChannelAction, add appendEventHandler member to facilitate event publishing
/// @details
/// @date       2024-06-06
/// @author     lianglongxiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/StateManagement/PHMComm
/// @interface_level=uint
/// @trace_id_sr=SR_SM_08001
/// @unit_name=HealthChannelActionImpl
/// @unit_description=Inherit PHM's HealthChannelAction, add appendEventHandler member to facilitate event publishing
/// @endcode
///
/// ================================================================

#include "health_channel_action_impl.h"

#include <ara/phm/health_channels/hc_testunit.h>  // NOLINT
#include <ara/phm/health_channels/tyre_pressure.h>
#include <ara/phm/health_channels/vol.h>
#include <ara/sm/error_domain_sm.h>

#include "helper.h"

namespace ara {
namespace sm {
namespace phm_comm {

/// @brief Register the callback function for appending events
/// @tparam EnumT Enum template parameter
/// @param appendEventHandler Callback function for appending events
template < typename EnumT >
inline void HealthChannelActionImpl< EnumT >::RegisterAppendEventHandler(
    std::function< void(common::Event &&) > const &appendEventHandler) noexcept
{
    log_.LogInfo() << "HealthChannelActionImpl<EnumT>::RegisterAppendEventHandler()";
    appendEventHandler_ = appendEventHandler;
}

/// @brief Forward RecoveryNotification request
/// @tparam EnumT Enum template parameter
/// @param[in] type, HealthStatus type of PHM
/// @param[in] healthStatusId, The identifier representing the Health Status.
/// @param[in] promise  Request promise
template < typename EnumT >
inline void HealthChannelActionImpl< EnumT >::DeliverRecoveryNotificationRequest(
    common::PHMHealthStatusType const type, EnumT const healthStatusId, core::Promise< void > &&promise) const noexcept
{
    log_.LogInfo() << "HealthChannelActionImpl<EnumT>::DeliverRecoveryNotificationRequest(), type:"
                   << common::PHMHealthStatusTypeToString(type).data()
                   << "healthStatusId:" << static_cast< phm::HealthStatus >(healthStatusId);

    if (appendEventHandler_) {
        // PHM's HealthChannelAction's PHMHealthChannelRecoveryNotification request
        common::Event event;
        event.type = common::EventType::kInPHMHealthChannelRecoveryNotification;

        // PHMHealthChannelRecoveryNotification information
        common::PHMHealthChannelRecoveryNotificationInfo *const data{
            new common::PHMHealthChannelRecoveryNotificationInfo()};
        data->healthStatusType = type;
        data->healthStatus     = static_cast< phm::HealthStatus >(healthStatusId);
        event.data             = data;
        event.requestPromise   = std::move(promise);

        // Add to event queue
        appendEventHandler_(std::move(event));
    } else {
        log_.LogWarn()
            << "HealthChannelActionImpl<EnumT>::DeliverRecoveryNotificationRequest() appendEventHandler_ is nullptr";
        promise.SetError(SMErrc::kRejected);
    }
}

template void HealthChannelActionImpl< phm::health_channels::vol::HealthStatuses >::RegisterAppendEventHandler(
    std::function< void(common::Event &&) > const &) noexcept;
template void
HealthChannelActionImpl< phm::health_channels::tyre_pressure::HealthStatuses >::RegisterAppendEventHandler(
    std::function< void(common::Event &&) > const &) noexcept;
template void HealthChannelActionImpl< phm::health_channels::hc_testunit::HealthStatuses >::RegisterAppendEventHandler(
    std::function< void(common::Event &&) > const &) noexcept;

template void HealthChannelActionImpl< phm::health_channels::vol::HealthStatuses >::DeliverRecoveryNotificationRequest(
    common::PHMHealthStatusType type,
    phm::health_channels::vol::HealthStatuses healthStatusId,
    core::Promise< void > &&promise) const noexcept;
template void
HealthChannelActionImpl< phm::health_channels::tyre_pressure::HealthStatuses >::DeliverRecoveryNotificationRequest(
    common::PHMHealthStatusType type,
    phm::health_channels::tyre_pressure::HealthStatuses healthStatusId,
    core::Promise< void > &&promise) const noexcept;
template void
HealthChannelActionImpl< phm::health_channels::hc_testunit::HealthStatuses >::DeliverRecoveryNotificationRequest(
    common::PHMHealthStatusType type,
    phm::health_channels::hc_testunit::HealthStatuses healthStatusId,
    core::Promise< void > &&promise) const noexcept;

}  // namespace phm_comm
}  // namespace sm
}  // namespace ara