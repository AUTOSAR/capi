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
/// @file       event_proxy.cpp
/// @brief
/// @details
/// @date       2024-12-17
/// @author     zhanglipeng
/// @version    1.2.0
///
/// ================================================================

#include "event_proxy.h"

#include "ara/diag/diag_error_domain.h"
#include "log/log.h"
#include "serialization/serialization.h"

namespace isoft {
namespace dm {
namespace dic {

constexpr uint8_t kFuncIDNotifyEventStatus{1U};
constexpr uint8_t kFuncIDGetEventStatus{2U};
constexpr uint8_t kFuncIDGetLatchedWIRStatus{3U};
constexpr uint8_t kFuncIDSetLatchedWIRStatus{4U};
constexpr uint8_t kFuncIDGetDTCNumber{5U};
constexpr uint8_t kFuncIDGetDebouncingStatus{6U};
constexpr uint8_t kFuncIDGetFaultDetectionCounter{7U};
constexpr uint8_t kFuncIDGetTestComplete{8U};

using ara::diag::common::LogError;

EventProxy::EventProxy(uint16_t const& instanceId, uint32_t const& serviceInstanceId)
    : BussinessClientProxy{serviceInstanceId, instanceId}
{
    RegisterNotificationCallBack([this](FuncData const& funcData) { _notify(funcData); });
}

/// @brief Register callback function for AA side of event status change notification
/// @param callBack Callback function to be registered
/// @return
void EventProxy::RegisterEventStatusNotifier(std::function< void(ara::diag::Event::EventStatusByte) > const& callBack)
{
    static uint8_t s_CallId{0U};

    ara::core::Result< void > const result{RegisterCallBack(kFuncIDNotifyEventStatus, s_CallId)};
    if (!result.HasValue()) {
        LogError() << "EventProxy::RegisterEventStatusNotifier|register error";
        return;
    }

    eventStatusNotifierCb_ = callBack;
}

/// @brief Get event status
/// @param
/// @return Current event status byte
ara::core::Result< ara::diag::Event::EventStatusByte > EventProxy::GetEventStatus()
{
    static uint8_t s_CallId{0U};
    ara::core::Result< uint8_t > const result{SyncCallFunc< uint8_t >(kFuncIDGetEventStatus, s_CallId)};
    if (result.HasValue()) {
        uint8_t const ret = result.Value();
        ara::diag::Event::EventStatusByte retCopy;
        retCopy.eventStatusByte_ = ret;  /// NOLINT
        return ara::core::Result< ara::diag::Event::EventStatusByte >::FromValue(retCopy);
    }
    return ara::core::Result< ara::diag::Event::EventStatusByte >::FromError(result.Error());
}

/// @brief Get latched WIR status
/// @param
/// @return Latched WIR status
ara::core::Result< bool > EventProxy::GetLatchedWIRStatus()
{
    static uint8_t s_CallId{0U};
    ara::core::Result< bool > const result{SyncCallFunc< bool >(kFuncIDGetLatchedWIRStatus, s_CallId)};
    if (result.HasValue()) {
        bool const ret = result.Value();
        return ara::core::Result< bool >::FromValue(ret);
    }
    return ara::core::Result< bool >::FromError(result.Error());
}

/// @brief Set latched WIR status
/// @param status WIR status to be set
/// @return Return void for successful
ara::core::Result< void > EventProxy::SetLatchedWIRStatus(bool status)
{
    static uint8_t s_CallId{0U};
    ara::core::Result< void > const result{SyncCallFunc< void >(kFuncIDSetLatchedWIRStatus, s_CallId, status)};
    if (result.HasValue()) {
        return ara::core::Result< void >::FromValue();
    }
    return ara::core::Result< void >::FromError(result.Error());
}

/// @brief Get DTC number by dtcFormat
/// @param dtcFormat dtc format type
/// @return dtcNumber
ara::core::Result< std::uint32_t > EventProxy::GetDTCNumber(ara::diag::DTCFormatType dtcFormat)
{
    static uint8_t s_CallId{0U};
    ara::core::Result< uint32_t > const result{
        SyncCallFunc< std::uint32_t >(kFuncIDGetDTCNumber, s_CallId, static_cast< uint8_t >(dtcFormat))};
    if (result.HasValue()) {
        std::uint32_t const ret = result.Value();
        return ara::core::Result< std::uint32_t >::FromValue(ret);
    }
    return ara::core::Result< std::uint32_t >::FromError(result.Error());
}

/// @brief Get debounce status
/// @param
/// @return Debouncing state
ara::core::Result< ara::diag::Event::DebouncingState > EventProxy::GetDebouncingStatus()
{
    static uint8_t s_CallId{0U};
    ara::core::Result< uint8_t > const result{SyncCallFunc< std::uint8_t >(kFuncIDGetDebouncingStatus, s_CallId)};
    if (result.HasValue()) {
        ara::diag::Event::DebouncingState const ret = static_cast< ara::diag::Event::DebouncingState >(result.Value());
        return ara::core::Result< ara::diag::Event::DebouncingState >::FromValue(ret);
    }
    return ara::core::Result< ara::diag::Event::DebouncingState >::FromError(result.Error());
}

/// @brief Get Fault Detection Counter (FDC)
/// @param
/// @return Current fault detection counter value
ara::core::Result< std::int8_t > EventProxy::GetFaultDetectionCounter()
{
    static uint8_t s_CallId{0U};
    ara::core::Result< std::int8_t > const result{
        SyncCallFunc< std::int8_t >(kFuncIDGetFaultDetectionCounter, s_CallId)};
    if (result.HasValue()) {
        std::int8_t const ret = result.Value();
        return ara::core::Result< std::int8_t >::FromValue(ret);
    }
    return ara::core::Result< std::int8_t >::FromError(result.Error());
}

/// @brief Get test complete status of event
/// @param
/// @return Test complete status
ara::core::Result< bool > EventProxy::GetTestComplete()
{
    static uint8_t s_CallId{0U};
    ara::core::Result< bool > const result{SyncCallFunc< bool >(kFuncIDGetTestComplete, s_CallId)};
    if (result.HasValue()) {
        bool const ret = result.Value();
        return ara::core::Result< bool >::FromValue(ret);
    }
    return ara::core::Result< bool >::FromError(result.Error());
}

void EventProxy::_notify(FuncData const& funcData) const
{
    switch (static_cast< int >(funcData.funcId)) {
        case kFuncIDNotifyEventStatus: {
            if (nullptr != eventStatusNotifierCb_) {
                std::uint8_t status{};
                if (serialize::Deserialize(funcData.data, status) > 0) {
                    ara::diag::Event::EventStatusByte eventStatus;
                    eventStatus.eventStatusByte_ = status;
                    eventStatusNotifierCb_(eventStatus);
                }
            } else {
                LogError() << "EventProxy::_notify|eventStatusNotifierCb_ is nullptr";
            }
        } break;
        default: {
        } break;
    }
}

}  // namespace dic
}  // namespace dm
}  // namespace isoft