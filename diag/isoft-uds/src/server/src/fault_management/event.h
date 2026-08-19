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
/// @file       event.h
/// @brief      This file provides event functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @module_path=fault_management
/// @interface_level=
/// @trace_id_sr=07e8882d-2602-407f-87d2-9b72ead21809
/// @unit_name=event
/// @unit_description=This file provides event functionality
///
/// ================================================================

#ifndef __ISOFT_UDS_FAULT_EVENT_H__
#define __ISOFT_UDS_FAULT_EVENT_H__

#include <vector>

#include "fault_management/status_handler.h"
#include "isoft/uds/fault_management/event_interface.h"
#include "isoft/uds/fault_management/fault_management.h"

namespace isoft {
namespace uds {
namespace server {

/// @brief DTC class declaration
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02464
/// @needwork = dd
/// @endcode
class Dtc;

/// @brief Monitor class declaration
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02465
/// @needwork = dd
/// @endcode
class Monitor;

/// @brief FaultManagementRuntimeContext class declaration
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02466
/// @needwork = dd
/// @endcode
class FaultManagementRuntimeContext;

/// @brief Indicator class declaration
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02467
/// @needwork = dd
/// @endcode
class Indicator;

/// @brief ConditionGroup class declaration
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02468
/// @needwork = dd
/// @endcode
class ConditionGroup;

/// @brief OperationCycle class declaration
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02469
/// @needwork = dd
/// @endcode
class OperationCycle;

/// @brief MemoryDestination class declaration
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02470
/// @needwork = dd
/// @endcode
class MemoryDestination;

/// @brief DebounceInterface class declaration
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02471
/// @needwork = dd
/// @endcode
class DebounceInterface;

/// @brief Event class declaration
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02472
/// @needwork = dd
/// @endcode
class Event final : public std::enable_shared_from_this< Event >
{
public:
    /// @brief Event copy constructor
    /// @param[in] other Event reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00785
    /// @needwork = dda
    /// @endcode
    Event(Event const& other) = delete;
    /// @brief Event move constructor
    /// @param[in] other Event rvalue reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00786
    /// @needwork = dda
    /// @endcode
    Event(Event&& other) = delete;
    /// @brief Copy assignment
    /// @param[in] right Event reference
    /// @return Event reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00787
    /// @needwork = dda
    /// @endcode
    Event& operator=(Event const& right) = delete;
    /// @brief Move assignment
    /// @param[in] right Event rvalue reference
    /// @return Event reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00788
    /// @needwork = dda
    /// @endcode
    Event& operator=(Event&& right) = delete;

public:
    /// @brief DTC format type
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01554
    /// @needwork = dd
    /// @endcode
    enum class DTCFormatType : std::uint8_t
    {
        kDTCFormatOBD   = 0,  ///< SAE_J2012-DA_DTCFormat_00 as defined in ISO 15031-6 specification.
        kDTCFormatUDS   = 1,  ///< ISO_14229-1_DTCFormat     as defined in ISO 14229-1 specification.
        kDTCFormatJ1939 = 2   ///< SAE_J1939-73_DTCFormat    as defined in SAE J1939-73.
    };

    /// @brief Delete DemEvent default constructor
    Event() = delete;

    /// @brief Event constructor with parameters
    /// @param[in] context dem runtime context, convenient for obtaining various data
    /// @param[in] id event marker
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01555
    /// @needwork = dd
    /// @endcode
    explicit Event(FaultManagementRuntimeContext& context) noexcept;

    /// @brief Event destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01556
    /// @needwork = dd
    /// @endcode
    ~Event() = default;

    /// @brief Parse parameters from configuration
    /// @param[in] node Configuration node
    /// @param[in] debounceAlgorithms All debounce algorithms, the algorithm bound to this event should be found here
    /// @param[in] indicators All indicators, the indicator bound to this event should be found here
    /// @param[in] conditionGroups All condition groups, the condition group bound to this event should be found here
    /// @param[in] operationCycles All operation cycles, the operation cycle bound to this event should be found here
    /// @param[in] dtcs All dtcs, the dtc bound to this event should be found here
    /// @param[in] memDestinations Persistent FDC finds destination through bound dtc
    /// @return Returns true on success, false on failure
    /// @code{.isoft}
    /// @throw unknown
    /// @needwork = dd
    /// @endcode
    bool Init(EventConfig& config,
              std::map< uint32_t, DebounceAlgorithmConfig >& debounceAlgorithms,
              std::map< uint32_t, std::shared_ptr< Indicator > >& indicators,
              std::map< uint32_t, std::shared_ptr< ConditionGroup > >& conditionGroups,
              std::map< uint32_t, std::shared_ptr< OperationCycle > >& operationCycles,
              std::map< uint32_t, std::shared_ptr< Dtc > >& dtcs,
              std::map< uint32_t, std::shared_ptr< MemoryDestination > >& memDestinations);

    /// @brief Get dtc value
    /// @return dtc value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01557
    /// @needwork = dd
    /// @endcode
    std::uint32_t GetDTCValue() noexcept;

    /// @brief Get debounce type
    /// @return debounce type
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01558
    /// @needwork = dd
    /// @endcode
    DebounceTypeEnum GetDebounceType() noexcept;

    /// @brief Get whether the bound debounce count needs to be persistently stored
    /// @return Returns true if stored, otherwise false
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01559
    /// @needwork = dd
    /// @endcode
    bool IsStorageCounter() noexcept;

    /// @brief Get bound debouncing state
    /// @return Returns bound debouncing state
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01560
    /// @needwork = dd
    /// @endcode
    DebouncingState GetDebouncingStatus() noexcept;

    /// @brief Get current FDC
    /// @return Current FDC
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01561
    /// @needwork = dd
    /// @endcode
    int8_t GetFaultDetectionCounter() noexcept;

    /// @brief Check whether this event detection is completed
    /// @return true or false
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01562
    /// @needwork = dd
    /// @endcode
    bool GetTestComplete() noexcept;

    /// @brief Get the monitor bound to this event
    /// @return Bound monitor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01563
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< Monitor > GetMonitor() const noexcept { return monitor_; }

    /// @brief Get configured attribute This parameter is used as a dependency value for dtc status bit processing when dtc clear behavior occurs
    /// @return DiagnosticClearEventAllowedBehaviorEnum
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01564
    /// @needwork = dd
    /// @endcode
    DiagnosticClearEventAllowedBehaviorEnum GetClearAllowedBehavior() const noexcept
    {
        return config_.clearEventAllowedBehavior;
    }

    /// @brief Check whether debounce FDC has reached the configured storage threshold
    /// @return Returns true if reached, otherwise false
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01565
    /// @needwork = dd
    /// @endcode
    bool IsFilledFdcThresholdStorage() noexcept;

    /// @brief [SWS_DM_01028]. When the bound dtc executes clear operation, it will call back here to clear the state of this event and related debouncing processing
    /// @param[in] isForbidden Whether the current clear of the bound dtc is restricted
    /// @param[in] isFromUds Whether the clear operation is initiated from UDS
    /// @code{.isoft}
    /// @throw unknown
    /// @needwork = dd
    /// @endcode
    void DtcClearedCallback(bool const isForbidden, bool const isFromUds);

    /// @brief Initiated from the bound custom debouncing, obtain user-filtered FDC from monitor
    /// @code{.isoft}
    /// @throw unknown
    /// @needwork = dd
    /// @endcode
    void GetFaultDetectionCounterFromMonitor();

    /// @brief Set state after debouncing
    /// @param[in] actionType State after debouncing
    /// @code{.isoft}
    /// @throw unknown
    /// @needwork = dd
    /// @endcode
    void SetDebouncedStatus(MonitorAction const actionType);

    /// @brief When the healing cycle of the indicator ends, process related logic
    /// @param[in] indicator A certain indicator bound to this event
    /// @code{.isoft}
    /// @throw unknown
    /// @needwork = dd
    /// @endcode
    void ProcessEndOfHealingCycle(std::shared_ptr< Indicator > const& indicator);

    /// @brief Get event marker
    /// @return event marker
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01566
    /// @needwork = dd
    /// @endcode
    std::uint32_t GetID() const noexcept { return config_.associatedEventIdentification; }

    /// @brief Get failed operation cycle count
    /// @return Return count
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01567
    /// @needwork = dd
    /// @endcode
    std::uint8_t GetFailedCycles() noexcept;

    /// @brief Get operation cycle count after first failure
    /// @return Return count
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01568
    /// @needwork = dd
    /// @endcode
    std::uint8_t GetCyclesSinceFirstFailed() noexcept;

    /// @brief Get operation cycle count after last failure
    /// @return Return count
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01569
    /// @needwork = dd
    /// @endcode
    std::uint8_t GetCyclesSinceLastFailed() noexcept;

    /// @brief Get the maximum FDC in the current operation cycle
    /// @return Return FDC
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01570
    /// @needwork = dd
    /// @endcode
    std::int8_t GetMaxFDCDuringCurrentCycle() noexcept;

    /// @brief Get the maximum FDC counted since the last clear
    /// @return Return maximum FDC
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01571
    /// @needwork = dd
    /// @endcode
    std::int8_t GetMaxFDCSinceLastClear() noexcept;

    /// @brief Check whether the user has initiated the operation to lock the WIR status bit
    /// @return true or false
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01572
    /// @needwork = dd
    /// @endcode
    bool IsWIRLock() const noexcept { return indicatorLock_; }

    /// @brief confirmationThreshold
    /// Configured parameter, when the confirmed fault operation cycle count reaches this threshold, the confirm status bit of dtc will be set to 1, the fault operation
    /// cycle count can be called trip counter, this logic will be mentioned in document ISO 14229-1 figure D.4
    /// @return Configured fault confirmation threshold
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01573
    /// @needwork = dd
    /// @endcode
    std::uint32_t GetConfirmThreshod() const noexcept { return config_.confirmationThreshold; }

    /// @brief When the state of the bound operation cycle changes, it will be called here to process related logic
    /// @param[in] oldState Old state
    /// @param[in] newState New state
    /// @code{.isoft}
    /// @throw unknown
    /// @needwork = dd
    /// @endcode
    void ReportOperationCycleStateTransition(OperationCycleType const oldState, OperationCycleType const newState);

    /// @brief IsDtcSuppressionOn function
    /// @return Operation result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01574
    /// @needwork = dd
    /// @endcode
    bool IsDtcSuppressionOn() noexcept;

private:
    /// @brief The information received in the monitor is sent here for processing related logic, such as: judgment of operation cycle state,
    /// judgment of enable condition, debouncing and state processing, etc.
    /// @param[in] action The pending information sent by monitor
    /// @return result
    /// @code{.isoft}
    /// @throw unknown
    /// @needwork = dd
    /// @endcode
    int32_t _processMonitorActionEvent(MonitorAction const action);

    /// @brief Register the bound enable condition group
    /// @param[in] group The enable condition group to be bound
    /// @return Returns true on success, false on failure
    /// @code{.isoft}
    /// @throw unknown
    /// @needwork = dd
    /// @endcode
    bool _registerEnableConditionGroup(std::shared_ptr< ConditionGroup > const& group);

    /// @brief Register the bound operation cycle
    /// @param[in] opCycle The operation cycle to be bound
    /// @return Returns true on success, false on failure
    /// @code{.isoft}
    /// @throw unknown
    /// @needwork = dd
    /// @endcode
    bool _registerOperationCycle(std::shared_ptr< OperationCycle > const& opCycle);

    /// @brief Register the bound dtc
    /// @param[in] dtc The dtc to be bound
    /// @return Returns true on success, false on failure
    /// @code{.isoft}
    /// @throw unknown
    /// @needwork = dd
    /// @endcode
    bool _registerDtc(std::shared_ptr< Dtc > const& dtc);

    /// @brief Register the bound monitor
    /// @param[in] monitor The monitor to be bound
    /// @return Returns true on success, false on failure
    /// @code{.isoft}
    /// @throw unknown
    /// @needwork = dd
    /// @endcode
    bool _registerDiagnosticMonitor(std::shared_ptr< Monitor > const& monitor);

    /// @name debounce_
    /// The debouncing bound to this event
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01575
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< DebounceInterface > debounce_{nullptr};

    /// @name isOperationCycleStarted_
    /// Whether the bound operation cycle is in start state
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01576
    /// @needwork = dd
    /// @endcode
    bool isOperationCycleStarted_{false};

    /// @name monitor_
    /// Bound monitor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01577
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< Monitor > monitor_{};

    /// @name eventStatusHandler_
    /// Event state processing wrapper class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01578
    /// @needwork = dd
    /// @endcode
    EventStatusHandler eventStatusHandler_{};

    /// @name dtc_
    /// Bound dtc
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01579
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< Dtc > dtc_{};

    /// @name connectedIndicators_
    /// Associated set of indicators
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01580
    /// @needwork = dd
    /// @endcode
    std::vector< std::shared_ptr< Indicator > > connectedIndicators_{};

    /// @name enableConditionGroup_
    /// Bound enable condition group
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01581
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< ConditionGroup > enableConditionGroup_{};

    /// @name context_
    /// Save dem runtime context, many logics in event need to query data from here
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01582
    /// @needwork = dd
    /// @endcode
    FaultManagementRuntimeContext& context_;

    /// @name indicatorLock_
    /// Record whether the WIR status bit is locked by AA initiation
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01583
    /// @needwork = dd
    /// @endcode
    bool indicatorLock_{false};

    /// @name isFailedFirst_
    /// Whether the fault has occurred for the first time, this value will be detected when the event state changes
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01584
    /// @needwork = dd
    /// @endcode
    bool isFailedFirst_{false};

    /// @name isFailedLast_
    /// Whether the fault has occurred recently
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01585
    /// @needwork = dd
    /// @endcode
    bool isFailedLast_{false};

    /// @name failedCycleCounter_
    /// Fault cycle count, used as EDR internal DataElement data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01586
    /// @needwork = dd
    /// @endcode
    std::atomic_uint8_t failedCycleCounter_{0U};

    /// @name cycleCounterSinceFirstFailed_
    /// Cycle count after first fault, used as EDR internal DataElement data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01587
    /// @needwork = dd
    /// @endcode
    std::atomic_uint8_t cycleCounterSinceFirstFailed_{0U};

    /// @name cycleCounterSinceLastFailed_
    /// Cycle count after most recent fault, used as EDR internal DataElement data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01588
    /// @needwork = dd
    /// @endcode
    std::atomic_uint8_t cycleCounterSinceLastFailed_{0U};

    /// @name maxFdcDuringCurCycle_
    /// Maximum FDC in the current operation cycle, used as EDR internal DataElement data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01589
    /// @needwork = dd
    /// @endcode
    std::atomic_int8_t maxFdcDuringCurCycle_{0};

    /// @name maxFdcSinceLastClear_
    /// Maximum FDC since last clear, used as EDR internal DataElement data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01590
    /// @needwork = dd
    /// @endcode
    std::atomic_int8_t maxFdcSinceLastClear_{0};

    /// @name bFreezeUpdateDtc_
    /// Whether to freeze monitor's update of DTC, 0x85 service
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01591
    /// @needwork = dd
    /// @endcode
    bool bFreezeUpdateDtc_{false};

    /// @brief Configuration
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01592
    /// @needwork = dd
    /// @endcode
    EventConfig config_{};
};
}  // namespace server
}  // namespace uds
}  // namespace isoft

#endif  // __ISOFT_UDS_FAULT_EVENT_H__