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
namespace dic {

MonitorAgent::MonitorAgent(uint16_t const& instanceId, uint32_t const& serviceInstanceId)
    : proxy_{std::make_unique< MonitorProxy >(instanceId, serviceInstanceId)}
{
}

/// @brief Register callback function for AA side that monitor needs to be reinitialized
/// @param notifier Callback function to be registered
/// @return
void MonitorAgent::RegisterInitMonitorNotify(
    std::function< void(ara::diag::InitMonitorReason reasonType) > const& notifier)
{
    proxy_->RegisterInitMonitorNotify(notifier);
}

/// @brief Register callback function for AA side of FDC change notifycation
/// @param notifier Callback function to be registered
/// @return
void MonitorAgent::RegisterFaultDetectionCounterNotify(std::function< void() > const& notifier)
{
    proxy_->RegisterFaultDetectionCounterNotify(notifier);
}

/// @brief AA report an event
/// @param ac MonitorAction type to be report
/// @return Return void for successful
ara::core::Result< void > MonitorAgent::SetMonitorAction(ara::diag::MonitorAction const ac)
{
    return proxy_->SetMonitorAction(ac);
}

/// @brief Set fault detection counter(FDC)
/// @param fdc fdc value
/// @return Return void for successful
ara::core::Result< void > MonitorAgent::SetFaultDetectionCounter(std::int8_t const fdc)
{
    return proxy_->SetFaultDetectionCounter(fdc);
}

/// @brief Set counter-based debounce parameters
/// @param counter Counter value to be set
/// @return Return void for successful
ara::core::Result< void > MonitorAgent::SetCounterBased(ara::diag::Monitor::CounterBased const& counter)
{
    return proxy_->SetCounterBased(counter);
}

/// @brief Set timer-based debounce parameters
/// @param time Time value to be set
/// @return Return void for successful setting
ara::core::Result< void > MonitorAgent::SetTimeBased(ara::diag::Monitor::TimeBased const& time)
{
    return proxy_->SetTimeBased(time);
}

/// @brief Register callback for service ready
/// @param cb Callback function to be registered
void MonitorAgent::RegisterOnServiceReady(std::function< void(bool) > const& cb) { proxy_->RegisterOnServiceReady(cb); }

}  // namespace dic
}  // namespace dm
}  // namespace isoft