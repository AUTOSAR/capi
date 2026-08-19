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
/// @brief      This file provides the implementation of Monitor.
/// @details
/// @date       2022-03-28
/// @author     gaohuiming
/// @version    1.2.0
///
/// ================================================================

#include "ara/diag/monitor.h"

#include "ara/diag/diag_error_domain.h"
#include "gen_code/monitor/clientAgent/monitor_agent.h"
#include "resolve.h"
#include "utility.h"

namespace ara {
namespace diag {

/// @brief Monitor constructor for Monitors with Monitor-internal debouncing
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
/// @param[in] specifier InstanceSpecifier to an PortPrototype of an
/// DiagnosticMonitorInterface
/// @param[in] initMonitor Possibility to register an InitMonitor callback
/// @param[in] getFaultDetectionCounter Possibility to register a function to
/// get the current FDC for this event.
/// @throws on overflow
///
/// @traceid{SWS_DM_00548}@tracestatus{draft}
Monitor::Monitor(ara::core::InstanceSpecifier const &specifier,
                 std::function< void(InitMonitorReason) > initMonitor,         /// NOLINT
                 std::function< std::int8_t(void) > getFaultDetectionCounter)  /// NOLINT
{
    ara::core::Result< internal::InstanceInfo > const retrieveResult{internal::Resolve(specifier)};
    if (!retrieveResult.HasValue()) {
        internal::LogError() << "Monitor::Monitor -> not found specifier=" << specifier.ToString();
        return;
    }
    proxy_ = std::make_shared< isoft::dm::dic::MonitorAgent >(retrieveResult.Value().instanceId,
                                                              retrieveResult.Value().serviceInstanceId);
    std::function< void() > getFdcCb = [this, getFaultDetectionCounter]() {
        int8_t const fdc              = getFaultDetectionCounter();
        ara::core::Result< void > res = proxy_->SetFaultDetectionCounter(fdc);
        if (!res.HasValue()) {
            internal::LogError() << "Monitor::Monitor|SetFaultDetectionCounter fail, errorno =" << res.Error();
        } else {
            internal::LogDebug() << "Monitor::Monitor|SetFaultDetectionCounter success fdc =" << fdc;
        }
    };
    proxy_->RegisterOnServiceReady([this, initMonitor, getFdcCb](bool ready) {
        internal::LogDebug() << "Monitor::Monitor|1 service is ready, callback is called, ready =" << ready
                             << "isReady =" << isReady_;
        if (!ready) {
            return;
        }

        if (isReady_) {
            return;
        }

        proxy_->RegisterInitMonitorNotify(initMonitor);

        proxy_->RegisterFaultDetectionCounterNotify(getFdcCb);

        isReady_ = true;
        internal::LogDebug() << "Monitor::Monitor|1 service is ready, callback is "
                                "called, finish init";
    });

    internal::LogDebug() << "Monitor::Monitor|init "
                            "finished(getFaultDetectionCounter), instanceId ="
                         << retrieveResult.Value().instanceId
                         << "serviceInstanceId =" << retrieveResult.Value().serviceInstanceId;
}

/// @brief Monitor constructor for Monitors with counter-based debouncing
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
/// @param[in] specifier InstanceSpecifier to an PortPrototype of an
/// DiagnosticMonitorInterface
/// @param[in] initMonitor Possibility to register an InitMonitor callback
/// @param[in] defaultValues Default values for CounterBased debouncing
/// @throws on overflow
///
/// @traceid{SWS_DM_00549}@tracestatus{draft}
Monitor::Monitor(ara::core::InstanceSpecifier const &specifier,
                 std::function< void(InitMonitorReason) > initMonitor,  /// NOLINT
                 CounterBased defaultValues)
{
    ara::core::Result< internal::InstanceInfo > const retrieveResult{internal::Resolve(specifier)};
    if (!retrieveResult.HasValue()) {
        internal::LogError() << "Monitor::Monitor|not found specifier =" << specifier.ToString();
        return;
    }
    proxy_ = std::make_shared< isoft::dm::dic::MonitorAgent >(retrieveResult.Value().instanceId,
                                                              retrieveResult.Value().serviceInstanceId);
    proxy_->RegisterOnServiceReady([this, initMonitor, defaultValues, specifier](bool ready) {
        internal::LogDebug() << "Monitor::Monitor|2 service is ready, callback is called, ready =" << ready
                             << "isReady =" << isReady_;
        if (!ready) {
            return;
        }

        if (isReady_) {
            return;
        }

        proxy_->RegisterInitMonitorNotify(initMonitor);

        decltype(auto) result = proxy_->SetCounterBased(defaultValues);
        if (!result.HasValue()) {
            internal::LogError() << "Monitor::Monitor|SetCounterBased failed. specifier ="
                                 << std::move(specifier.ToString()) << ", err =" << result.Error();
            return;
        }

        isReady_ = true;
        internal::LogDebug() << "Monitor::Monitor|2 service is ready, callback is "
                                "called, finish init";
    });

    internal::LogDebug() << "Monitor::Monitor|init finished(CounterBased), instanceId ="
                         << retrieveResult.Value().instanceId
                         << "serviceInstanceId =" << retrieveResult.Value().serviceInstanceId;
}

/// @brief Monitor constructor for Monitors with time-based debouncing
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
/// @param[in] specifier InstanceSpecifier to an PortPrototype of an
/// DiagnosticMonitorInterface
/// @param[in] initMonitor Possibility to register an InitMonitor callback
/// @param[in] defaultValues Default values for TimeBased debouncing
/// @throws on overflow
///
/// @traceid{SWS_DM_00550}@tracestatus{draft}
Monitor::Monitor(ara::core::InstanceSpecifier const &specifier,
                 std::function< void(InitMonitorReason) > initMonitor,  /// NOLINT
                 TimeBased defaultValues)
{
    ara::core::Result< internal::InstanceInfo > const retrieveResult{internal::Resolve(specifier)};
    if (!retrieveResult.HasValue()) {
        internal::LogError() << "Monitor::Monitor -> not found specifier=" << specifier.ToString();
        return;
    }
    proxy_ = std::make_shared< isoft::dm::dic::MonitorAgent >(retrieveResult.Value().instanceId,
                                                              retrieveResult.Value().serviceInstanceId);
    proxy_->RegisterOnServiceReady([this, initMonitor, defaultValues, specifier](bool ready) {
        internal::LogDebug() << "Monitor::Monitor|3 service is ready, callback is called, ready =" << ready
                             << "isReady =" << isReady_;
        if (!ready) {
            return;
        }

        if (isReady_) {
            return;
        }

        proxy_->RegisterInitMonitorNotify(initMonitor);

        decltype(auto) result = proxy_->SetTimeBased(defaultValues);
        if (!result.HasValue()) {
            internal::LogError() << "Monitor::Monitor|SetTimeBased failed. specifier="
                                 << std::move(specifier.ToString()) << ", err =" << result.Error();
            return;
        }

        isReady_ = true;
        internal::LogDebug() << "Monitor::Monitor|3 service is ready, callback is "
                                "called, finish init";
    });

    internal::LogDebug() << "Monitor::Monitor|init finished(TimeBased), instanceId ="
                         << retrieveResult.Value().instanceId
                         << "serviceInstanceId =" << retrieveResult.Value().serviceInstanceId;
}

/// @brief Function to report the status information being relevant for error
/// monitoring paths.
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
/// @param[in] action Contains either the last (un-)qualified test result of the
/// diagnostic monitor or commands to control the debouncing or to force a
/// prestorage.
/// @return a Result with either void or an error
/// error code: kNotOffered, kGenericError, kInvalidArgument, kReportIgnored,
/// kDebouncingConfigurationInconsistent, kConfigurationMismatch
/// @throws on overflow
///
/// @traceid{SWS_DM_00543}@tracestatus{draft}
ara::core::Result< void > Monitor::ReportMonitorAction(MonitorAction action)
{
    if (proxy_.get() == nullptr) {
        internal::LogError() << "Monitor::ReportMonitorAction|proxy is null.";
        return ara::core::Result< void >::FromError(ara::diag::DiagErrc::kFailed);
    }
    return proxy_->SetMonitorAction(action);
}

}  // namespace diag
}  // namespace ara
