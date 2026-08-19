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
/// @file       event.cpp
/// @brief      This file provides the implementation of Event and related types.
/// @details
/// @date       2024-12-12
/// @author     zhanglipeng
/// @version    1.2.0
///
/// ================================================================

#include "event.h"

namespace ara {
namespace diag {
namespace dmd {

Event::Event(uint32_t const& serviceInstanceId) noexcept
    : eventAgent_{std::make_unique< isoft::dm::dis::EventAgent >(serviceInstanceId)}
{
}

/// @brief Notify AA side of event status changes
/// @param status Event status
/// @return Returns 0 for successful call
std::int32_t Event::NotifyEventStatus(uint8_t const status)
{
    eventAgent_->NotifyEventStatus(status);
    return 0;
}

/// @brief Register callback for AA side to get event status
/// @param callBack Callback function
/// @return Returns 0 for successful registration
std::int32_t Event::RegisterGetEventStatus(isoft::uds::server::GetEventStatusCallBack const& callBack)
{
    if (eventAgent_.get() == nullptr) {
        return -1;
    }

    getEventStatusCallBack_ = callBack;
    eventAgent_->RegisterGetEventStatus([this]() -> uint8_t { return getEventStatusCallBack_(); });
    return 0;
}

/// @brief Register callback for AA side to get WIR status
/// @param callBack Callback function
/// @return Returns 0 for successful registration
std::int32_t Event::RegisterGetLatchedWIRStatus(isoft::uds::server::GetLatchedWIRStatusCallBack const& callBack)
{
    if (eventAgent_.get() == nullptr) {
        return -1;
    }

    getLatchedWIRStatusCallBack_ = callBack;
    eventAgent_->RegisterGetLatchedWIRStatus([this]() -> bool { return getLatchedWIRStatusCallBack_(); });
    return 0;
}

/// @brief Register callback for AA side to set WIR status
/// @param callBack Callback function
/// @return Returns 0 for successful registration
std::int32_t Event::RegisterSetLatchedWIRStatus(isoft::uds::server::SetLatchedWIRStatusCallBack const& callBack)
{
    if (eventAgent_.get() == nullptr) {
        return -1;
    }

    setLatchedWIRStatusCallBack_ = callBack;
    eventAgent_->RegisterSetLatchedWIRStatus(
        [this](bool status) -> int32_t { return setLatchedWIRStatusCallBack_(status); });
    return 0;
}

/// @brief Register callback for AA side to get DTC number
/// @param callBack Callback function
/// @return Returns 0 for successful registration
std::int32_t Event::RegisterGetDTCNumber(isoft::uds::server::GetDTCNumberCallBack const& callBack)
{
    if (eventAgent_.get() == nullptr) {
        return -1;
    }

    getDTCNumberCallBack_ = callBack;
    eventAgent_->RegisterGetDTCNumber([this](uint8_t dtcFormat, uint32_t& dtcNumber) -> int32_t {
        return getDTCNumberCallBack_(dtcFormat, dtcNumber);
    });
    return 0;
}

/// @brief Register callback for AA side to get debounce status
/// @param callBack Callback function
/// @return Returns 0 for successful registration
std::int32_t Event::RegisterGetDebouncingStatus(isoft::uds::server::GetDebouncingStatusCallBack const& callBack)
{
    if (eventAgent_.get() == nullptr) {
        return -1;
    }

    getDebouncingStatusCallBack_ = callBack;
    eventAgent_->RegisterGetDebouncingStatus(
        [this]() -> uint8_t { return static_cast< uint8_t >(getDebouncingStatusCallBack_()); });
    return 0;
}

/// @brief Register callback for AA side to get FDC
/// @param callBack Callback function
/// @return Returns 0 for successful registration
std::int32_t Event::RegisterGetFaultDetectionCounter(
    isoft::uds::server::GetFaultDetectionCounterCallBack const& callBack)
{
    if (eventAgent_.get() == nullptr) {
        return -1;
    }

    getFaultDetectionCounterCallBack_ = callBack;
    eventAgent_->RegisterGetFaultDetectionCounter([this]() -> int8_t { return getFaultDetectionCounterCallBack_(); });
    return 0;
}

/// @brief Register callback for AA side to get the test status of the event
/// @param callBack Callback function
/// @return Returns 0 for successful registration
std::int32_t Event::RegisterGetTestComplete(isoft::uds::server::GetTestCompleteCallBack const& callBack)
{
    if (eventAgent_.get() == nullptr) {
        return -1;
    }

    getTestCompleteCallBack_ = callBack;
    eventAgent_->RegisterGetTestComplete([this]() -> bool { return getTestCompleteCallBack_(); });
    return 0;
}

}  // namespace dmd
}  // namespace diag
}  // namespace ara