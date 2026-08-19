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
/// @file       event_report.cpp
/// @brief      =
/// @details
/// @date       2022-12-27
/// @author     zhaoyunfei
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/idsm/lib client
/// @interface_level=module
/// @trace_id_sr=SRS_IDSM_0005
/// @unit_name=EventReporter
/// @unit_description=
/// @endcode
///
/// ================================================================

#include "ara/idsm/event_report.h"

#include <ara/core/span.h>

#include "idsc_client.h"
#include "idsc_log.h"

namespace ara {
namespace idsm {

/// @brief  [SWS_AIDSM_10301] Construct a new Event Reporter object. Called by the sensor for each event type using
/// the
/// instance specified of the event type.
/// @param eventType
EventReporter::EventReporter(ara::core::InstanceSpecifier eventType) noexcept : instanceId_{std::move(eventType)} {}

/// @brief [SWS_AIDSM_10302] Create a new security event at the IdsM. .
/// @param  counter
void EventReporter::ReportEvent(CountType const counter) const noexcept
{
    IdsmClient *const client{IdsmClient::GetInstance()};
    if (nullptr == client) {
        IDSC_LOG_ERROR << "IdsmClient::GetInstance is nullptr!";
        return;
    }
    std::ignore = client->Report(instanceId_, counter);
}

/// @brief [SWS_AIDSM_10303] Create a new security event with a sensor-provided timestamp at the IdsM. .
/// @param  timestamp
/// @param counter
void EventReporter::ReportEvent(TimestampType const timestamp, CountType const counter) const noexcept
{
    IdsmClient *const client{IdsmClient::GetInstance()};
    if (nullptr == client) {
        IDSC_LOG_ERROR << "IdsmClient::GetInstance is nullptr!";
        return;
    }
    TimestampType const tempTimeStamp{timestamp & 0x3fffffffffffffffU};
    std::ignore = client->Report(instanceId_, tempTimeStamp, counter);
}

/// @brief [SWS_AIDSM_10304]  Create a new security event with sensor-provided context data at the IdsM. .
/// @param contextData counter
/// @param counter
void EventReporter::ReportEvent(ContextDataType const &contextData, CountType const counter) const noexcept
{
    IdsmClient *const client{IdsmClient::GetInstance()};
    if (nullptr == client) {
        IDSC_LOG_ERROR << "IdsmClient::GetInstance is nullptr!";
        return;
    }
    std::ignore = client->Report(instanceId_, contextData, counter);
}

/// @brief [SWS_AIDSM_10305] Create a new security event with sensor-provided context data and with a
/// sensor-provided
//// timestamp at the IdsM.
/// @param contextData
/// @param timestamp
/// @param counter
void EventReporter::ReportEvent(ContextDataType const &contextData,
                                TimestampType const timestamp,
                                CountType const counter) const noexcept
{
    IdsmClient *const client{IdsmClient::GetInstance()};
    if (nullptr == client) {
        IDSC_LOG_ERROR << "IdsmClient::GetInstance is nullptr!";
        return;
    }
    TimestampType const tempTimeStamp{timestamp & 0x3fffffffffffffffU};
    std::ignore = client->Report(instanceId_, contextData, tempTimeStamp, counter);
}

}  // namespace idsm
}  // namespace ara