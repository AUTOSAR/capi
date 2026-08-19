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
/// @file       monitor.cpp
/// @brief      This file provides the implementation of Monitor and related types.
/// @details
/// @date       2024-12-12
/// @author     zhanglipeng
/// @version    1.2.0
///
/// ================================================================

#include "monitor.h"

namespace ara {
namespace diag {
namespace dmd {

Monitor::Monitor(uint32_t const& serviceInstanceId) noexcept
    : monitorAgent_{std::make_unique< isoft::dm::dis::MonitorAgent >(serviceInstanceId)}
{
}

/// @brief Notify AA side that monitor needs to be reinitialized
/// @param reasonType Initialization reason
/// @return Returns 0 for successful call
std::int32_t Monitor::NotifyInitMonitor(isoft::uds::server::InitMonitorReason const reasonType)
{
    if (monitorAgent_.get() == nullptr) {
        return -1;
    }

    monitorAgent_->NotifyInitMonitor(static_cast< ara::diag::InitMonitorReason >(reasonType));
    return 0;
}

/// @brief Notify AA side of FDC changes
/// @param iFdc fdc
/// @return Returns 0 for successful call
std::int32_t Monitor::NotifyFaultDetectionCounter()
{
    if (monitorAgent_.get() == nullptr) {
        return -1;
    }

    monitorAgent_->NotifyFaultDetectionCounter();
    return 0;
}

/// @brief Register callback for AA side to report events
/// @param callBack Callback function
/// @return Returns 0 for successful registration
std::int32_t Monitor::RegisterSetMonitorAction(isoft::uds::server::SetMonitorActionCallBack const& callBack)
{
    if (monitorAgent_.get() == nullptr) {
        return -1;
    }

    monitorActionCb_ = callBack;
    monitorAgent_->RegisterSetMonitorAction([this](ara::diag::MonitorAction action) -> int32_t {
        return monitorActionCb_(static_cast< isoft::uds::server::MonitorAction >(action));
    });
    return 0;
}

/// @brief Register callback for AA side to set FDC
/// @param callBack Callback function
/// @return Returns 0 for successful registration
std::int32_t Monitor::RegisterSetFaultDetectionCounter(
    isoft::uds::server::SetFaultDetectionCounterCallBack const& callBack)
{
    if (monitorAgent_.get() == nullptr) {
        return -1;
    }

    fdcCb_ = callBack;
    monitorAgent_->RegisterSetFaultDetectionCounter([this](int8_t fdc) { fdcCb_(fdc); });
    return 0;
}

/// @brief Register callback for AA side to set counter-based debounce parameters
/// @param callBack Callback function
/// @return Returns 0 for successful registration
std::int32_t Monitor::RegisterSetCounterBased(isoft::uds::server::SetCounterBasedCallBack const& callBack)
{
    if (monitorAgent_.get() == nullptr) {
        return -1;
    }

    counterCb_ = callBack;
    monitorAgent_->RegisterSetCounterBased([this](isoft::dm::CounterBased counter) {
        isoft::uds::server::CounterBased cn;
        cn.counterDecrementStepSize = counter.passedStepsize;
        cn.counterFailedThreshold   = counter.failedThreshold;
        cn.counterIncrementStepSize = counter.failedStepsize;
        cn.counterJumpDown          = counter.useJumpToPassed;
        cn.counterJumpDownValue     = counter.passedJumpValue;
        cn.counterJumpUp            = counter.useJumpToFailed;
        cn.counterJumpUpValue       = counter.failedJumpValue;
        cn.counterPassedThreshold   = counter.passedThreshold;
        counterCb_(cn);
    });
    return 0;
}

/// @brief Register callback for AA side to set timer-based debounce parameters
/// @param callBack Callback function
/// @return Returns 0 for successful registration
std::int32_t Monitor::RegisterSetTimeBased(isoft::uds::server::SetTimeBasedCallBack const& callBack)
{
    if (monitorAgent_.get() == nullptr) {
        return -1;
    }

    timeCb_ = callBack;
    monitorAgent_->RegisterSetTimeBased([this](isoft::dm::TimeBased time) {
        isoft::uds::server::TimeBased tm;
        tm.timeFailedThreshold = time.failedMs;
        tm.timePassedThreshold = time.passedMs;
        timeCb_(tm);
    });
    return 0;
}

}  // namespace dmd
}  // namespace diag
}  // namespace ara