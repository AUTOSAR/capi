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
/// @file       monitor_interface.h
/// @brief      This file provides monitor_interface functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/isoft/uds/fault_management
/// @interface_level=software
/// @trace_id_sr=cf5fa4ed-7cbb-454f-8cfa-ea94527e4adb
/// @unit_name=MonitorInterface
/// @unit_description=This file provides monitor_interface functionality
/// @endcode
///
/// ================================================================

#ifndef __ISOFT_UDS_FAULT_MONITOR_INTERFACE_H__
#define __ISOFT_UDS_FAULT_MONITOR_INTERFACE_H__

#include <cstdint>
#include <functional>
#include <memory>

namespace isoft {
namespace uds {
namespace server {

/// @brief Reason for initializing monitor
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01384
/// @trace_id_dd=DD_UDS_02676
/// @needwork = ad
/// @endcode
enum class InitMonitorReason : std::uint32_t
{
    kClear = 0x00,      ///<  Event was cleared and all internal values and states are reset.
    kRestart   = 0x01,  ///<  Operation cycle of the event was (re-)started
    kReenabled = 0x02,  ///<  Enable conditions are fulfilled and control DTC setting is set to on
    kDisabled  = 0x03  ///<  Enable conditions no longer fulfilled, or Control DTC setting is set to off(version:R2111 Autosar)
};

/// @brief Count-based monitor
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01385
/// @trace_id_dd=DD_UDS_02677
/// @needwork = ad
/// @endcode
class CounterBased
{
public:
    /// @name counterDecrementStepSize
    /// This value is used to decrement the internal Debounce count
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01145
    /// @trace_id_dd=DD_UDS_02259
    /// @needwork = ad
    /// @endcode
    std::uint16_t counterDecrementStepSize{0U};

    /// @name counterFailedThreshold
    /// When the internal Debounce count is greater than or equal to this threshold, it is considered "Failed" state
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01146
    /// @trace_id_dd=DD_UDS_02260
    /// @needwork = ad
    /// @endcode
    std::int16_t counterFailedThreshold{0};

    /// @name counterIncrementStepSize
    /// This value is used to increment the internal Debounce count
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01147
    /// @trace_id_dd=DD_UDS_02261
    /// @needwork = ad
    /// @endcode
    std::uint16_t counterIncrementStepSize{0U};

    /// @name counterPassedThreshold
    /// When the internal Debounce count is less than or equal to this threshold, it is considered "Passed" state
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01148
    /// @trace_id_dd=DD_UDS_02262
    /// @needwork = ad
    /// @endcode
    std::int16_t counterPassedThreshold{0};

    /// @name counterJumpDown
    /// Switch for whether to perform jump-down behavior
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01149
    /// @trace_id_dd=DD_UDS_02263
    /// @needwork = ad
    /// @endcode
    bool counterJumpDown{false};

    /// @name counterJumpDownValue
    /// When the direction of count change changes from increment to decrement, this value represents the initial value of the internal Debounce count
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01150
    /// @trace_id_dd=DD_UDS_02264
    /// @needwork = ad
    /// @endcode
    std::int16_t counterJumpDownValue{0};

    /// @name counterJumpUp
    /// Switch for whether to perform jump-up behavior
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01151
    /// @trace_id_dd=DD_UDS_02265
    /// @needwork = ad
    /// @endcode
    bool counterJumpUp{false};

    /// @name counterJumpUpValue
    /// When the direction of count change changes from decrement to increment, this value represents the initial value of the internal Debounce count
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01152
    /// @trace_id_dd=DD_UDS_02266
    /// @needwork = ad
    /// @endcode
    std::int16_t counterJumpUpValue{0};
};

/// @brief Time-based monitor
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01386
/// @trace_id_dd=DD_UDS_02678
/// @needwork = ad
/// @endcode
class TimeBased
{
public:
    /// @name timeFailedThreshold
    /// When the internal Debounce count is greater than or equal to this threshold, it is considered "Failed" state
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01153
    /// @trace_id_dd=DD_UDS_02267
    /// @needwork = ad
    /// @endcode
    std::uint32_t timeFailedThreshold{0U};  // second

    /// @name timePassedThreshold
    /// When the internal Debounce count is less than or equal to this threshold, it is considered "Passed" state
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01154
    /// @trace_id_dd=DD_UDS_02268
    /// @needwork = ad
    /// @endcode
    std::uint32_t timePassedThreshold{0U};  // second
};

/// @brief Monitor operation
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01387
/// @trace_id_dd=DD_UDS_02679
/// @needwork = ad
/// @endcode
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

/// @brief Set monitor operation callback
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01388
/// @trace_id_dd=DD_UDS_02680
/// @needwork = ad
/// @endcode
using SetMonitorActionCallBack = std::function< int32_t(MonitorAction) >;
/// @brief Set fault detection counter callback
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01389
/// @trace_id_dd=DD_UDS_02681
/// @needwork = ad
/// @endcode
using SetFaultDetectionCounterCallBack = std::function< void(std::int8_t) >;
/// @brief Set count-based monitor callback
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01390
/// @trace_id_dd=DD_UDS_02682
/// @needwork = ad
/// @endcode
using SetCounterBasedCallBack = std::function< void(CounterBased) >;
/// @brief Set time-based monitor callback
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01391
/// @trace_id_dd=DD_UDS_02683
/// @needwork = ad
/// @endcode
using SetTimeBasedCallBack = std::function< void(TimeBased) >;

/// @brief Monitor interface
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01392
/// @trace_id_dd=DD_UDS_02684
/// @needwork = ad
/// @endcode
class MonitorInterface
{
public:
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_01031
    /// @needwork = dda
    /// @endcode
    MonitorInterface(MonitorInterface const&) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_01032
    /// @needwork = dda
    /// @endcode
    MonitorInterface(MonitorInterface&&) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_01033
    /// @needwork = dda
    /// @endcode
    MonitorInterface& operator=(MonitorInterface const&) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_01034
    /// @needwork = dda
    /// @endcode
    MonitorInterface& operator=(MonitorInterface&&) = delete;
    /// @brief Constructor
    /// @return None
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01155
    /// @trace_id_dd=DD_UDS_02269
    /// @needwork = ad
    /// @endcode
    MonitorInterface() noexcept = default;
    /// @brief Destructor
    /// @return None
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01156
    /// @trace_id_dd=DD_UDS_02270
    /// @needwork = ad
    /// @endcode
    virtual ~MonitorInterface() noexcept = default;

public:
    /// @brief Notify initialization of monitor
    /// @param reasonType Initialization reason
    /// @return Returns 0 on successful call
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01157
    /// @trace_id_dd=DD_UDS_02271
    /// @needwork = ad
    /// @endcode
    virtual std::int32_t NotifyInitMonitor(InitMonitorReason const reasonType) = 0;
    /// @brief Notify fault detection counter
    /// @param iFdc fdc
    /// @return Returns 0 on successful call
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01158
    /// @trace_id_dd=DD_UDS_02272
    /// @needwork = ad
    /// @endcode
    virtual std::int32_t NotifyFaultDetectionCounter() = 0;

    /// @brief Register set monitor operation callback
    /// @param callBack Callback
    /// @return Returns 0 on successful registration
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01159
    /// @trace_id_dd=DD_UDS_02273
    /// @needwork = ad
    /// @endcode
    virtual std::int32_t RegisterSetMonitorAction(SetMonitorActionCallBack const& callBack) = 0;

    /// @brief Register set fault detection counter callback
    /// @param callBack Callback
    /// @return Returns 0 on successful registration
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01160
    /// @trace_id_dd=DD_UDS_02274
    /// @needwork = ad
    /// @endcode
    virtual std::int32_t RegisterSetFaultDetectionCounter(SetFaultDetectionCounterCallBack const& callBack) = 0;

    /// @brief Register set count-based monitor callback
    /// @param callBack Callback
    /// @return Returns 0 on successful registration
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01161
    /// @trace_id_dd=DD_UDS_02275
    /// @needwork = ad
    /// @endcode
    virtual std::int32_t RegisterSetCounterBased(SetCounterBasedCallBack const& callBack) = 0;

    /// @brief Register set time-based monitor callback
    /// @param callBack Callback
    /// @return Returns 0 on successful registration
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01162
    /// @trace_id_dd=DD_UDS_02276
    /// @needwork = ad
    /// @endcode
    virtual std::int32_t RegisterSetTimeBased(SetTimeBasedCallBack const& callBack) = 0;
};

}  // namespace server
}  // namespace uds
}  // namespace isoft

#endif  // DM_UDS_GENERIC_UDS_SERVICE_H_