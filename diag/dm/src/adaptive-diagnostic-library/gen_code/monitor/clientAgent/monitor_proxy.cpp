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
/// @file       monitor_proxy.cpp
/// @brief
/// @details
/// @date       2024-12-06
/// @author     xuhua
/// @version    1.2.0
///
/// ================================================================

#include "monitor_proxy.h"

#include "ara/diag/diag_error_domain.h"
#include "log/log.h"
#include "serialization/serialization.h"
#include "thread_pool/thread_pool.h"

namespace isoft {
namespace dm {
namespace dic {

constexpr uint8_t kFuncIDNotifyInitMonitor{1U};
constexpr uint8_t kFuncIDNotifyFaultDetectionCounter{2U};
constexpr uint8_t kFuncIDSetMonitorAction{3U};
constexpr uint8_t kFuncIDSetFaultDetectionCounter{4U};
constexpr uint8_t kFuncIDSetCounterBased{5U};
constexpr uint8_t kFuncIDSetTimeBased{6U};

using ara::diag::common::LogError;
using ara::diag::common::LogWarn;

MonitorProxy::MonitorProxy(uint16_t const& instanceId, uint32_t const& serviceInstanceId)
    : BussinessClientProxy{serviceInstanceId, instanceId}
{
    RegisterNotificationCallBack([this](FuncData const& funcData) { _notify(funcData); });
}

/// @brief Register callback function for AA side that monitor needs to be reinitialized
/// @param notifier Callback function to be registered
/// @return
void MonitorProxy::RegisterInitMonitorNotify(
    std::function< void(ara::diag::InitMonitorReason reasonType) > const& notifier)
{
    static uint8_t s_CallId{0U};

    ara::core::Result< void > result{RegisterCallBack(kFuncIDNotifyInitMonitor, s_CallId)};
    if (!result.HasValue()) {
        LogError() << "MonitorProxy::RegisterInitMonitorNotify|register error =" << result.Error().Value();
        return;
    }

    initMonitorCb_ = notifier;
}

/// @brief Register callback function for AA side of FDC change notifycation
/// @param notifier Callback function to be registered
/// @return
void MonitorProxy::RegisterFaultDetectionCounterNotify(std::function< void() > const& notifier)
{
    static uint8_t s_CallId{0U};

    ara::core::Result< void > result{RegisterCallBack(kFuncIDNotifyFaultDetectionCounter, s_CallId)};
    if (!result.HasValue()) {
        LogError() << "MonitorProxy::RegisterFaultDetectionCounterNotify|register error =" << result.Error().Value();
        return;
    }

    fdcNotiferCb_ = notifier;
}

/// @brief AA report an event
/// @param ac MonitorAction type to be report
/// @return Return void for successful
ara::core::Result< void > MonitorProxy::SetMonitorAction(ara::diag::MonitorAction const ac)
{
    static uint8_t s_CallId{0U};
    ara::core::Result< void > const result{
        SyncCallFunc< void >(kFuncIDSetMonitorAction, s_CallId, static_cast< uint32_t >(ac))};
    if (result.HasValue()) {
        return ara::core::Result< void >::FromValue();
    }
    return ara::core::Result< void >::FromError(result.Error());
}

/// @brief Set fault detection counter(FDC)
/// @param fdc fdc value
/// @return Return void for successful
ara::core::Result< void > MonitorProxy::SetFaultDetectionCounter(std::int8_t const fdc)
{
    static uint8_t s_CallId{0U};
    ara::core::Result< void > const result{SyncCallFunc< void >(kFuncIDSetFaultDetectionCounter, s_CallId, fdc)};
    if (result.HasValue()) {
        return ara::core::Result< void >::FromValue();
    }
    return ara::core::Result< void >::FromError(result.Error());
}

/// @brief Set counter-based debounce parameters
/// @param counter Counter value to be set
/// @return Return void for successful
ara::core::Result< void > MonitorProxy::SetCounterBased(ara::diag::Monitor::CounterBased const& counter)
{
    static uint8_t s_CallId{0U};
    ara::core::Result< void > const result{
        SyncCallFunc< void >(kFuncIDSetCounterBased, s_CallId, counter.failedThreshold, counter.passedThreshold,
                             counter.failedStepsize, counter.passedStepsize, counter.failedJumpValue,
                             counter.passedJumpValue, counter.useJumpToFailed, counter.useJumpToPassed)};
    if (result.HasValue()) {
        return ara::core::Result< void >::FromValue();
    }
    return ara::core::Result< void >::FromError(result.Error());
}

/// @brief Set timer-based debounce parameters
/// @param time Time value to be set
/// @return Return void for successful setting
ara::core::Result< void > MonitorProxy::SetTimeBased(ara::diag::Monitor::TimeBased const& time)
{
    static uint8_t s_CallId{0U};
    ara::core::Result< void > const result{
        SyncCallFunc< void >(kFuncIDSetTimeBased, s_CallId, time.failedMs, time.passedMs)};
    if (result.HasValue()) {
        return ara::core::Result< void >::FromValue();
    }
    return ara::core::Result< void >::FromError(result.Error());
}

void MonitorProxy::_notify(FuncData const& funcData) const
{
    switch (funcData.funcId) {
        case kFuncIDNotifyInitMonitor: {
            if (nullptr != initMonitorCb_) {
                std::uint32_t reason{};
                if (serialize::Deserialize(funcData.data, reason) > 0) {
                    initMonitorCb_(static_cast< ara::diag::InitMonitorReason >(reason));
                }
            } else {
                LogWarn() << "MonitorProxy::_notify|initMonitorCb_ is nullptr";
            }
            break;
        }
        case kFuncIDNotifyFaultDetectionCounter: {
            if (nullptr != fdcNotiferCb_) {
                fdcNotiferCb_();
            } else {
                LogWarn() << "MonitorProxy::_notify|fdcNotiferCb_ is nullptr";
            }
            break;
        }
        default: {
        } break;
    }
}

}  // namespace dic
}  // namespace dm
}  // namespace isoft