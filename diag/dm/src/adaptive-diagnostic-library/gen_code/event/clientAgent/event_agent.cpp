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
/// @file       event_agent.cpp
/// @brief
/// @details
/// @date       2024-12-12
/// @author     zhanglipeng
/// @version    1.2.0
///
/// ================================================================

#include "event_agent.h"

namespace isoft {
namespace dm {
namespace dic {

EventAgent::EventAgent(uint16_t const& instanceId, uint32_t const& serviceInstanceId)
    : proxy_{std::make_unique< EventProxy >(instanceId, serviceInstanceId)}
{
}

/// @brief Register callback function for AA side of event status change notification
/// @param callBack Callback function to be registered
/// @return
void EventAgent::RegisterEventStatusNotifier(std::function< void(ara::diag::Event::EventStatusByte) > const& callBack)
{
    proxy_->RegisterEventStatusNotifier(callBack);
}

/// @brief Get event status
/// @param
/// @return Current event status byte
ara::core::Result< ara::diag::Event::EventStatusByte > EventAgent::GetEventStatus() { return proxy_->GetEventStatus(); }

/// @brief Get latched WIR status
/// @param
/// @return Latched WIR status
ara::core::Result< bool > EventAgent::GetLatchedWIRStatus() { return proxy_->GetLatchedWIRStatus(); }

/// @brief Set latched WIR status
/// @param status WIR status to be set
/// @return Return void for successful
ara::core::Result< void > EventAgent::SetLatchedWIRStatus(bool status) { return proxy_->SetLatchedWIRStatus(status); }

/// @brief Get DTC number by dtcFormat
/// @param dtcFormat dtc format type
/// @return dtcNumber
ara::core::Result< std::uint32_t > EventAgent::GetDTCNumber(ara::diag::DTCFormatType dtcFormat)
{
    return proxy_->GetDTCNumber(dtcFormat);
}

/// @brief Get debounce status
/// @param
/// @return Debouncing state
ara::core::Result< ara::diag::Event::DebouncingState > EventAgent::GetDebouncingStatus()
{
    return proxy_->GetDebouncingStatus();
}

/// @brief Get Fault Detection Counter (FDC)
/// @param
/// @return Current fault detection counter value
ara::core::Result< std::int8_t > EventAgent::GetFaultDetectionCounter() { return proxy_->GetFaultDetectionCounter(); }

/// @brief Get test complete status of event
/// @param
/// @return Test complete status
ara::core::Result< bool > EventAgent::GetTestComplete() { return proxy_->GetTestComplete(); }

/// @brief Determine whether the service is ready
/// @return true: Ready
bool EventAgent::IsServiceReady() { return proxy_->IsServiceReady(); }

}  // namespace dic
}  // namespace dm
}  // namespace isoft