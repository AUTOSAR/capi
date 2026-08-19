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
/// @file       monitor.h
/// @brief      This file provides the definitions of Monitor and related types.
/// @details
/// @date       2021-11-23
/// @author     jiawei
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_DIAG_MONITOR_H_
#define ARA_DIAG_MONITOR_H_

#include <ara/core/instance_specifier.h>
#include <ara/core/result.h>

#include <cstdint>
#include <functional>
#include <memory>

namespace isoft {
namespace dm {
namespace dic {
class MonitorAgent;
}  // namespace dic
}  // namespace dm
}  // namespace isoft

namespace ara {
namespace diag {

/// @brief Represents the status information reported to AAs why the monitor may be re-initalized
///
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
///
/// @traceid{SWS_DM_00540}@tracestatus{draft}
enum class InitMonitorReason : std::uint32_t
{
    kClear     = 0x00,  ///<  Event was cleared and all internal values and states are reset.
    kRestart   = 0x01,  ///<  Operation cycle of the event was (re-)started
    kReenabled = 0x02,  ///<  Enable conditions are fulfilled and control DTC setting is set to on
    kDisabled
    = 0x03  ///<  Enable conditions no longer fulfilled, or Control DTC setting is set to off(version:R2111 Autosar)
};

/// @brief Represents the status information reported by AAs being relevant for error monitoring
///
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
///
/// @traceid{SWS_DM_00541}@tracestatus{draft}
enum class MonitorAction : std::uint32_t
{
    kPassed              = 0x00,  ///<  Monitor reports qualified test result passed.
    kFailed              = 0x01,  ///<  Monitor reports qualified test result failed
    kPrepassed           = 0x02,  ///<  Monitor reports unqualified test result pre-passed.
    kPrefailed           = 0x03,  ///<  Monitor reports unqualified test result pre-failed.
    kFdcThresholdReached = 0x04,  ///<  Monitor triggers the storage of ExtendedDataRecords and Freeze Frames (if the
                                  ///<  triggering condition is connected to this threshold).
    kResetTestFailed  = 0x05,     ///<  Reset TestFailed Bit without any other side effects like readiness
    kFreezeDebouncing = 0x06,     ///<  Freeze the internal debounce counter/timer.
    kResetDebouncing  = 0x07      ///<  Reset the internal debounce counter/timer.
};

/// @brief Class to implement operations on diagnostic Monitor interface.
///
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
///
/// @traceid{SWS_DM_00542}@tracestatus{draft}
class Monitor final
{
public:
    /// @brief Represents the parameters for counter-based debouncing
    ///
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    ///
    /// @traceid{SWS_DM_00538}@tracestatus{draft}
    struct CounterBased
    {
        /// @brief Threshold until qualified failed
        ///
        /// @traceid{SWS_DM_00621}@tracestatus{draft}
        std::int16_t failedThreshold;
        /// @brief Threshold until qualified passed
        ///
        /// @traceid{SWS_DM_00622}@tracestatus{draft}
        std::int16_t passedThreshold;
        /// @brief Stepsize per pre-failed report
        ///
        /// @traceid{SWS_DM_00623}@tracestatus{draft}
        std::uint16_t failedStepsize;
        /// @brief Stepsize per pre-passed report
        ///
        /// @traceid{SWS_DM_00624}@tracestatus{draft}
        std::uint16_t passedStepsize;
        /// @brief failed to jump value
        ///
        /// @traceid{SWS_DM_00625}@tracestatus{draft}
        std::int16_t failedJumpValue;
        /// @brief passed to jump value
        ///
        /// @traceid{SWS_DM_00626}@tracestatus{draft}
        std::int16_t passedJumpValue;
        /// @brief is jump supported
        ///
        /// @traceid{SWS_DM_00627}@tracestatus{draft}
        bool useJumpToFailed;
        /// @brief is jump supported
        ///
        /// @traceid{SWS_DM_00628}@tracestatus{draft}
        bool useJumpToPassed;
    };

    /// @brief Represents the parameters for time-based debouncing
    ///
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    ///
    /// @traceid{SWS_DM_00539}@tracestatus{draft}
    struct TimeBased
    {
        /// @brief time until failed in (ms)
        ///
        /// @traceid{SWS_DM_00629}@tracestatus{draft}
        std::uint32_t failedMs;
        /// @brief time until passed in (ms)
        ///
        /// @traceid{SWS_DM_00630}@tracestatus{draft}
        std::uint32_t passedMs;
    };

    /// @brief Monitor constructor for Monitors with Monitor-internal debouncing
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[in] specifier InstanceSpecifier to an PortPrototype of an DiagnosticMonitorInterface
    /// @param[in] initMonitor Possibility to register an InitMonitor callback
    /// @param[in] getFaultDetectionCounter Possibility to register a function to get the current FDC for this event.
    /// @throws on overflow
    ///
    /// @traceid{SWS_DM_00548}@tracestatus{draft}
    Monitor(ara::core::InstanceSpecifier const& specifier,
            std::function< void(InitMonitorReason) > initMonitor,
            std::function< std::int8_t(void) > getFaultDetectionCounter);

    /// @brief Monitor constructor for Monitors with counter-based debouncing
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[in] specifier InstanceSpecifier to an PortPrototype of an DiagnosticMonitorInterface
    /// @param[in] initMonitor Possibility to register an InitMonitor callback
    /// @param[in] defaultValues Default values for CounterBased debouncing
    /// @throws on overflow
    ///
    /// @traceid{SWS_DM_00549}@tracestatus{draft}
    Monitor(ara::core::InstanceSpecifier const& specifier,
            std::function< void(InitMonitorReason) > initMonitor,
            CounterBased defaultValues);

    /// @brief Monitor constructor for Monitors with time-based debouncing
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[in] specifier InstanceSpecifier to an PortPrototype of an DiagnosticMonitorInterface
    /// @param[in] initMonitor Possibility to register an InitMonitor callback
    /// @param[in] defaultValues Default values for TimeBased debouncing
    /// @throws on overflow
    ///
    /// @traceid{SWS_DM_00550}@tracestatus{draft}
    Monitor(ara::core::InstanceSpecifier const& specifier,
            std::function< void(InitMonitorReason) > initMonitor,
            TimeBased defaultValues);

    /// @brief Function to report the status information being relevant for error monitoring paths.
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[in] action Contains either the last (un-)qualified test result of the diagnostic monitor or commands to
    /// control the debouncing or to force a prestorage.
    /// @return a Result with either void or an error
    /// error code: kNotOffered, kGenericError, kInvalidArgument, kReportIgnored,
    /// kDebouncingConfigurationInconsistent, kConfigurationMismatch
    /// @throws on overflow
    ///
    /// @traceid{SWS_DM_00543}@tracestatus{draft}
    ara::core::Result< void > ReportMonitorAction(MonitorAction action);

private:
    std::shared_ptr< isoft::dm::dic::MonitorAgent >
        /// @name proxy_
        proxy_;

    bool isReady_{false};
};
}  // namespace diag
}  // namespace ara

#endif  // ARA_DIAG_MONITOR_H_