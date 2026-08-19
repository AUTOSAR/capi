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
/// @file       debounce_interface.h
/// @brief      This file provides debounce_interface functionality
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
/// @trace_id_sr=bfc788d1-ad11-4b84-b950-538ece9d76eb
/// @unit_name=DebounceInterface
/// @unit_description=This file provides debounce_interface functionality
/// @endcode
///
/// ================================================================

#ifndef __ISOFT_UDS_FAULT_DEBOUNCE_INTERFACE_H__
#define __ISOFT_UDS_FAULT_DEBOUNCE_INTERFACE_H__

#include <atomic>
#include <mutex>

#include "fault_management/public.h"
#include "isoft/uds/fault_management/fault_management.h"
#include "isoft/uds/fault_management/monitor_interface.h"

namespace isoft {
namespace uds {
namespace server {

class Event;

/// @brief DebounceInterface class
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02453
/// @needwork = dd
/// @endcode
class DebounceInterface
{
public:
    /// @param[in] other DebounceInterface reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00776
    /// @needwork = dda
    /// @endcode
    DebounceInterface(DebounceInterface const& other) = delete;

    /// @param[in] other DebounceInterface rvalue reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00777
    /// @needwork = dda
    /// @endcode
    DebounceInterface(DebounceInterface&& other) = delete;

    /// @param[in] right DebounceInterface reference
    /// @return DebounceInterface reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00778
    /// @needwork = dda
    /// @endcode
    DebounceInterface& operator=(DebounceInterface const& right) = delete;

    /// @param[in] right DebounceInterface rvalue reference
    /// @return DebounceInterface reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00779
    /// @needwork = dda
    /// @endcode
    DebounceInterface& operator=(DebounceInterface&& right) = delete;

public:
    /// @brief Constructor
    /// @param[in] event The Event bound to this algorithm
    /// @param[in] config The configuration parameters related to this algorithm, pre-parsed
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01488
    /// @needwork = dd
    /// @endcode
    explicit DebounceInterface(std::shared_ptr< Event > const& event, DebounceAlgorithmConfig& config) noexcept;

    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01489
    /// @needwork = dd
    /// @endcode
    virtual ~DebounceInterface() = default;

    /// @brief Event receives MonitorAction from Monitor, which is called here to trigger Debouncing related state processing
    /// @param[in] action The MonitorAction parameter passed by Event
    /// @code{.isoft}
    /// @throw unknown
    /// @needwork = dd
    /// @endcode
    virtual void IndicateReportedStatus(MonitorAction action) = 0;

    /// @brief Get current FDC
    /// @return FDC
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01490
    /// @needwork = dd
    /// @endcode
    virtual std::int8_t GetFaultDetectionCounter() noexcept;

    /// @brief Reset FDC
    /// @code{.isoft}
    /// @throw unknown
    /// @needwork = dd
    /// @endcode
    virtual void Reset() = 0;

    /// @brief Freeze or unfreeze debounce interface
    /// @param[in] b Whether to Freeze
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01491
    /// @needwork = dd
    /// @endcode
    virtual void Freeze(bool b) noexcept = 0;

    /// @brief Get the type of debounce interface
    /// @return Debouncing type
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01492
    /// @needwork = dd
    /// @endcode
    virtual DebounceTypeEnum GetType() const noexcept = 0;

    /// @brief Get the current state of debounce interface
    /// @return DebouncingState
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01493
    /// @needwork = dd
    /// @endcode
    virtual DebouncingState GetDouncingState() noexcept;

    /// @brief Check whether the storage threshold of FDC is met, the trigger mode of MemoryEntry DiagnosticMemoryEntryStorageTriggerEnum::kFdcThreshold will cooperate with this function to determine whether storage is satisfied
    /// @return Returns true if satisfied, otherwise false
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01494
    /// @needwork = dd
    /// @endcode
    virtual bool IsFilledFdcThresholdStorage() noexcept { return false; }

    /// @brief Set CounterBased parameters of debounce interface
    /// @param[in] counter CounterBased parameter
    /// @code{.isoft}
    /// @throw unknown
    /// @needwork = dd
    /// @endcode
    virtual void SetDebounceCounterParam(CounterBased const& counter) noexcept { std::ignore = counter; }

    /// @brief Set TimerBased parameters of debounce interface
    /// @param[in] timer TimerBased parameter
    /// @code{.isoft}
    /// @throw unknown
    /// @needwork = dd
    /// @endcode
    virtual void SetDebounceTimerParam(TimeBased const& timer) noexcept { std::ignore = timer; }

    /// @brief Set FDC of debounce interface
    /// @param[in] fdc FDC
    /// @param[in] notify Whether to trigger FDC change callback notification logic
    /// @code{.isoft}
    /// @throw unknown
    /// @needwork = dd
    /// @endcode
    void SetFDC(int8_t const fdc, bool const notify = true);

    /// @return Returns true if storage is needed, false if not
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00780
    /// @needwork = dda
    /// @endcode
    bool IsStorageCounter() const noexcept { return debounceCounterStorage_; }

    /// @brief Set FDC change callback function of debounce interface
    /// @param[in] notifier Callback function
    /// @code{.isoft}
    /// @throw unknown
    /// @needwork = dd
    /// @endcode
    void SetNotifierFDCChanged(std::function< void(std::int8_t fdc) > const& notifier)
    {
        fdcChangeNotifier_ = notifier;
    }

    /// @brief Get the Behavior of debounce interface
    /// @return DiagnosticDebounceBehaviorEnum
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01495
    /// @needwork = dd
    /// @endcode
    DiagnosticDebounceBehaviorEnum GetDebounceBehavior() const noexcept { return debounceBehavior_; }

protected:
    /// @brief Notify debounce interface of new MonitorAction
    /// @param[in] action The MonitorAction parameter passed by Event
    /// @code{.isoft}
    /// @throw unknown
    /// @needwork = dd
    /// @endcode
    void _NotifyEvent(MonitorAction const actionType);

    /// @brief Trigger FDC change callback notification logic
    /// @code{.isoft}
    /// @throw unknown
    /// @needwork = dd
    /// @endcode
    void _NotifierFDCChanged();

    /// @brief Trigger Monitor's FDC change event
    /// @code{.isoft}
    /// @throw unknown
    /// @needwork = dd
    /// @endcode
    void _FireEventGetFDCFromMonitor();

    /// @brief Check whether it is in Freeze state
    /// @return Returns true if in Freeze state, false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01496
    /// @needwork = dd
    /// @endcode
    bool _IsFreeze() const noexcept { return freeze_.load(); }

    /// @brief Set Freeze state
    /// @param[in] b Whether to Freeze
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01497
    /// @needwork = dd
    /// @endcode
    void _SetFreeze(bool const b) noexcept { freeze_ = b; }

    /// @brief Get FDC of debounce interface
    /// @return Internally recorded FDC
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01498
    /// @needwork = dd
    /// @endcode
    std::int32_t _GetFDC() noexcept { return debounceCounter_.load(); }

    /// @brief Set FDC of debounce interface
    /// @param[in] fdc FDC
    /// @param[in] notify Whether to trigger FDC change callback logic
    /// @code{.isoft}
    /// @throw unknown
    /// @needwork = dd
    /// @endcode
    void _SetFDC(std::int32_t const fdc, bool const notify);

private:
    /// @name mutex_
    /// Used to protect data security when reading and writing this algorithm data in multiple threads
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01499
    /// @needwork = dd
    /// @endcode
    std::recursive_mutex mutex_;

    /// @name event_
    /// The Event bound to this algorithm
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01500
    /// @needwork = dd
    /// @endcode
    std::weak_ptr< Event > event_;

    /// @name debounceCounterStorage_
    /// Whether to persistently store FDC
    /// true: persistent storage
    /// false: do not persistent storage
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01501
    /// @needwork = dd
    /// @endcode
    bool debounceCounterStorage_{false};

    /// @name freeze_
    /// Freeze state
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01502
    /// @needwork = dd
    /// @endcode
    std::atomic< bool > freeze_{false};

    /// @name debounceBehavior_
    /// If the Enable Condition of the related Event is not satisfied or ControlDTCSetting is set to disabled, this attribute defines the behavior of the algorithm
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01503
    /// @needwork = dd
    /// @endcode
    DiagnosticDebounceBehaviorEnum debounceBehavior_{DiagnosticDebounceBehaviorEnum::kFreeze};

    /// @name debounceCounter_
    /// debounce count
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01504
    /// @needwork = dd
    /// @endcode
    std::atomic< std::int32_t > debounceCounter_{0};

    /// @name fdcChangeNotifier_
    /// FDC change callback function
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01505
    /// @needwork = dd
    /// @endcode
    std::function< void(std::int8_t fdc) > fdcChangeNotifier_;
};
}  // namespace server
}  // namespace uds
}  // namespace isoft

#endif  // __DEBOUNCE_INTERFACE_H__