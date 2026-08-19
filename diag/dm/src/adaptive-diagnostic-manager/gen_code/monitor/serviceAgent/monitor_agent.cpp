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
/// @file       monitor_agent.cpp
/// @brief
/// @details
/// @date       2024-12-12
/// @author     zhanglipeng
/// @version    1.2.0
///
/// ================================================================

#include "monitor_agent.h"

namespace isoft {
namespace dm {
namespace dis {

MonitorAgent::MonitorAgent(uint32_t const& serviceInstanceId)
{
    proxy_ = std::make_unique< MonitorProxy >(serviceInstanceId);
}

/// @brief Notify AA side that the monitor needs to be reinitialized
/// @param reasonType Initialization reason
/// @return
void MonitorAgent::NotifyInitMonitor(ara::diag::InitMonitorReason const reasonType)
{
    if (proxy_.get() == nullptr) {
        return;
    }

    proxy_->NotifyInitMonitor(reasonType);
}

/// @brief Notify AA side of FDC change
/// @param iFdc fdc
/// @return
void MonitorAgent::NotifyFaultDetectionCounter()
{
    if (proxy_.get() == nullptr) {
        return;
    }

    proxy_->NotifyFaultDetectionCounter();
}

/// @brief Register callback for AA side to report events
/// @param callBack Callback function to be registered
/// @return
void MonitorAgent::RegisterSetMonitorAction(std::function< int32_t(ara::diag::MonitorAction const) > const& callBack)
{
    if (proxy_.get() == nullptr) {
        return;
    }

    proxy_->RegisterSetMonitorAction(callBack);
}

/// @brief Register callback for AA side to set FDC
/// @param callBack Callback function to be registered
/// @return
void MonitorAgent::RegisterSetFaultDetectionCounter(std::function< void(std::int8_t const) > const& callBack)
{
    if (proxy_.get() == nullptr) {
        return;
    }

    proxy_->RegisterSetFaultDetectionCounter(callBack);
}

/// @brief Register callback for AA side to set counter-based debounce parameters
/// @param callBack Callback function to be registered
/// @return
void MonitorAgent::RegisterSetCounterBased(std::function< void(CounterBased const) > const& callBack)
{
    if (proxy_.get() == nullptr) {
        return;
    }

    proxy_->RegisterSetCounterBased(callBack);
}

/// @brief Register callback for AA side to set timer-based debounce parameters
/// @param callBack Callback function to be registered
/// @return
void MonitorAgent::RegisterSetTimeBased(std::function< void(TimeBased const) > const& callBack)
{
    if (proxy_.get() == nullptr) {
        return;
    }

    proxy_->RegisterSetTimeBased(callBack);
}

}  // namespace dis
}  // namespace dm
}  // namespace isoft