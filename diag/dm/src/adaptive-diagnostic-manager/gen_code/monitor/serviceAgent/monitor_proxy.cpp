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
/// @date       2024-12-12
/// @author     zhanglipeng
/// @version    1.2.0
///
/// ================================================================

#include "monitor_proxy.h"

#include <vector>

#include "netProxy/net_proxy.h"
#include "netProxy/rpc_define.h"
#include "serialization/serialization.h"

namespace isoft {
namespace dm {
namespace dis {

constexpr uint8_t kFuncIDNotifyInitMonitor{1U};
constexpr uint8_t kFuncIDNotifyFaultDetectionCounter{2U};
constexpr uint8_t kFuncIDSetMonitorAction{3U};
constexpr uint8_t kFuncIDSetFaultDetectionCounter{4U};
constexpr uint8_t kFuncIDSetCounterBased{5U};
constexpr uint8_t kFuncIDSetTimeBased{6U};

MonitorProxy::MonitorProxy(uint32_t const &serviceInstanceId) : BussinessServiceProxy{serviceInstanceId}
{
    NetProxy::GetInstance().GetNetAccess()->RegisterService(
        serviceInstanceId, [this](CallRequest &&callData) { CallHandle(std::move(callData)); });
}

void MonitorProxy::CallHandle(CallRequest &&callData)
{
    CallRequest cr{std::move(callData)};

    switch (cr.funcData.funcId) {
        case kFuncIDNotifyInitMonitor:
        case kFuncIDNotifyFaultDetectionCounter: {
            Subscribe(std::move(cr));
            break;
        }
        case kFuncIDSetMonitorAction: {
            _setMonitorAction(std::move(cr));
            break;
        }
        case kFuncIDSetFaultDetectionCounter: {
            _setFaultDetectionCounter(std::move(cr));
            break;
        }
        case kFuncIDSetCounterBased: {
            _setCounterBased(std::move(cr));
            break;
        }
        case kFuncIDSetTimeBased: {
            _setTimeBased(std::move(cr));
            break;
        }
        default: {
        } break;
    }
}

/// @brief Notify AA side that the monitor needs to be reinitialized
/// @param reasonType Initialization reason
/// @return
void MonitorProxy::NotifyInitMonitor(ara::diag::InitMonitorReason const reasonType)
{
    std::vector< uint8_t > payLoad;
    uint32_t const reason = static_cast< uint32_t >(reasonType);
    if (serialize::Serialize(payLoad, reason) < 0) {
        return;
    }
    Notify(kFuncIDNotifyInitMonitor, std::move(payLoad));
}

/// @brief Notify AA side of FDC change
/// @param iFdc fdc
/// @return
void MonitorProxy::NotifyFaultDetectionCounter()
{
    std::vector< uint8_t > payLoad;
    Notify(kFuncIDNotifyFaultDetectionCounter, std::move(payLoad));
}

/// @brief Register callback for AA side to report events
/// @param callBack Callback function to be registered
/// @return
void MonitorProxy::RegisterSetMonitorAction(std::function< int32_t(ara::diag::MonitorAction const) > const &callBack)
{
    monitorActionCb_ = callBack;
}

/// @brief Register callback for AA side to set FDC
/// @param callBack Callback function to be registered
/// @return
void MonitorProxy::RegisterSetFaultDetectionCounter(std::function< void(std::int8_t const) > const &callBack)
{
    setFDCCb_ = callBack;
}

/// @brief Register callback for AA side to set counter-based debounce parameters
/// @param callBack Callback function to be registered
/// @return
void MonitorProxy::RegisterSetCounterBased(std::function< void(CounterBased const) > const &callBack)
{
    setCounterCb_ = callBack;
}

/// @brief Register callback for AA side to set timer-based debounce parameters
/// @param callBack Callback function to be registered
/// @return
void MonitorProxy::RegisterSetTimeBased(std::function< void(TimeBased const) > const &callBack)
{
    setTimerCb_ = callBack;
}

void MonitorProxy::_setMonitorAction(CallRequest &&callData)
{
    uint32_t action{};
    RetData retData;
    std::vector< uint8_t > buf;
    CallRequest const cr{std::move(callData)};
    if (serialize::Deserialize(cr.funcData.data, action) > 0) {
        retData.errorCode = monitorActionCb_(static_cast< ara::diag::MonitorAction >(action));
    } else {
        retData.errorCode = static_cast< int32_t >(InnerErrorCode::kDataDeserialize);
    }

    NetProxy::GetInstance().GetNetAccess()->ReturnResult(cr, std::move(retData));
}

void MonitorProxy::_setFaultDetectionCounter(CallRequest &&callData)
{
    if (setFDCCb_ == nullptr) {
        return;
    }
    int8_t fdc{};
    RetData retData;
    std::vector< uint8_t > buf;
    CallRequest const cr{std::move(callData)};
    if (serialize::Deserialize(cr.funcData.data, fdc) > 0) {
        setFDCCb_(fdc);
    } else {
        retData.errorCode = static_cast< int32_t >(InnerErrorCode::kDataDeserialize);
    }
    NetProxy::GetInstance().GetNetAccess()->ReturnResult(cr, std::move(retData));
}

void MonitorProxy::_setCounterBased(CallRequest &&callData)
{
    if (setCounterCb_ == nullptr) {
        return;
    }
    CounterBased counter{};
    RetData retData;
    std::vector< uint8_t > buf;
    CallRequest const cr{std::move(callData)};
    if (serialize::Deserialize(cr.funcData.data, counter) > 0) {
        setCounterCb_(counter);
    } else {
        retData.errorCode = static_cast< int32_t >(InnerErrorCode::kDataDeserialize);
    }
    NetProxy::GetInstance().GetNetAccess()->ReturnResult(cr, std::move(retData));
}

void MonitorProxy::_setTimeBased(CallRequest &&callData)
{
    if (setTimerCb_ == nullptr) {
        return;
    }
    TimeBased time{};
    RetData retData;
    std::vector< uint8_t > buf;
    CallRequest const cr{std::move(callData)};
    if (serialize::Deserialize(cr.funcData.data, time) > 0) {
        setTimerCb_(time);
    } else {
        retData.errorCode = static_cast< int32_t >(InnerErrorCode::kDataDeserialize);
    }
    NetProxy::GetInstance().GetNetAccess()->ReturnResult(cr, std::move(retData));
}

}  // namespace dis
}  // namespace dm
}  // namespace isoft