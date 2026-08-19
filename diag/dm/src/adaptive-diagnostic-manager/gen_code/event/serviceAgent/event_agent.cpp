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
/// @date       2024-12-17
/// @author     zhanglipeng
/// @version    1.2.0
///
/// ================================================================

#include "event_agent.h"

namespace isoft {
namespace dm {
namespace dis {

EventAgent::EventAgent(uint32_t const& serviceInstanceId)
{
    proxy_ = std::make_unique< EventProxy >(serviceInstanceId);
}

/// @brief Notify AA of event status change
/// @param status Event status
/// @return
void EventAgent::NotifyEventStatus(uint8_t const status) { proxy_->NotifyEventStatus(status); }

/// @brief Register callback for AA side to get event status
/// @param callBack Callback function to be registered
/// @return
void EventAgent::RegisterGetEventStatus(std::function< std::uint8_t() > const& callBack)
{
    proxy_->RegisterGetEventStatus(callBack);
}

/// @brief Register callback for AA side to get WIR status
/// @param callBack Callback function to be registered
/// @return
void EventAgent::RegisterGetLatchedWIRStatus(std::function< bool() > const& callBack)
{
    proxy_->RegisterGetLatchedWIRStatus(callBack);
}

/// @brief Register callback for AA side to set WIR status
/// @param callBack Callback function to be registered
/// @return
void EventAgent::RegisterSetLatchedWIRStatus(std::function< int32_t(bool) > const& callBack)
{
    proxy_->RegisterSetLatchedWIRStatus(callBack);
}

/// @brief Register callback for AA side to get DTC number
/// @param callBack Callback function to be registered
/// @return
void EventAgent::RegisterGetDTCNumber(std::function< std::int32_t(std::uint8_t, std::uint32_t&) > const& callBack)
{
    proxy_->RegisterGetDTCNumber(callBack);
}

/// @brief Register callback for AA side to get debounce status
/// @param callBack Callback function to be registered
/// @return
void EventAgent::RegisterGetDebouncingStatus(std::function< uint8_t() > const& callBack)
{
    proxy_->RegisterGetDebouncingStatus(callBack);
}

/// @brief Register callback for AA side to get FDC
/// @param callBack Callback function to be registered
/// @return
void EventAgent::RegisterGetFaultDetectionCounter(std::function< std::int8_t() > const& callBack)
{
    proxy_->RegisterGetFaultDetectionCounter(callBack);
}

/// @brief Register callback for AA side to get event test status
/// @param callBack Callback function to be registered
/// @return
void EventAgent::RegisterGetTestComplete(std::function< bool() > const& callBack)
{
    proxy_->RegisterGetTestComplete(callBack);
}

}  // namespace dis
}  // namespace dm
}  // namespace isoft