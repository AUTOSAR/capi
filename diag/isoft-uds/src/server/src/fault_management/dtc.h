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
/// @file       dtc.h
/// @brief      This file provides dtc functionality
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
/// @trace_id_sr=7b479965-8c5d-4c12-b97a-545fab6938e1
/// @unit_name=dtc
/// @unit_description=This file provides dtc functionality
/// @endcode
///
/// ================================================================

#ifndef __ISOFT_UDS_FAULT_DTC_H__
#define __ISOFT_UDS_FAULT_DTC_H__
#include <atomic>
#include <map>
#include <memory>
#include <mutex>

#include "fault_management/status_handler.h"
#include "isoft/uds/fault_management/fault_management.h"

namespace isoft {
namespace uds {
namespace server {

/// @brief Aging declaration
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02536
/// @needwork = dd
/// @endcode
class Aging;

/// @brief Extended data record declaration
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02537
/// @needwork = dd
/// @endcode
class ExtendedDataRecord;

/// @brief Condition group declaration
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02538
/// @needwork = dd
/// @endcode
class ConditionGroup;

/// @brief Event declaration
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02539
/// @needwork = dd
/// @endcode
class Event;

/// @brief Memory entry declaration
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02540
/// @needwork = dd
/// @endcode
class MemoryEntry;

/// @brief Memory destination declaration
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02541
/// @needwork = dd
/// @endcode
class MemoryDestination;

/// @brief Data identifier set declaration
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02542
/// @needwork = dd
/// @endcode
class DataIdentifierSet;

/// @brief Freeze frame declaration
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02543
/// @needwork = dd
/// @endcode
class FreezeFrame;

/// @brief Fault management runtime context declaration
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02544
/// @needwork = dd
/// @endcode
class FaultManagementRuntimeContext;

/// @brief DTC status declaration
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02545
/// @needwork = dd
/// @endcode
class DTCStatus;

/// @brief Snapshot data record type declaration
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02546
/// @needwork = dd
/// @endcode
class SnapshotDataRecordType;

/// @brief Memory entry freeze record declaration
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02547
/// @needwork = dd
/// @endcode
class MemoryEntryFreezeRecord;

/// @brief DTC attribute definition
/// @code{.isoft}
/// @interface_level=unit
/// @needwork = dd
/// @endcode
class DtcProps final
{
public:
    /// @name aging
    /// If dtc is configured with aging, this property is non-null and is used to bind the corresponding aging function
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01828
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< Aging > aging{nullptr};
    /// @name immediateNvDataStorage
    /// A switch, whether to persist storage entry immediately
    /// true : persist storage immediately
    /// false: do not persist storage immediately
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01829
    /// @needwork = dd
    /// @endcode
    bool immediateNvDataStorage{false};

    // std::shared_ptr<DataIdentifierSet> legislatedFreezeFrameContentWwhObd{nullptr};.

    /// @name maxNumberFreezeFrameRecords
    /// The maximum number of freeze frame records an event is allowed to store
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01830
    /// @needwork = dd
    /// @endcode
    uint32_t maxNumberFreezeFrameRecords{0U};
    /// @name priority
    /// Event priority, this value is needed when displacement strategy occurs, smaller value means higher priority
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01831
    /// @needwork = dd
    /// @endcode
    uint8_t priority{0};
    /// @name significance
    /// Indicates the specific meaning of the event, this type is used in EDR
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01832
    /// @needwork = dd
    /// @endcode
    DiagnosticSignificanceEnum significance{DiagnosticSignificanceEnum::kFault};
    /// @name snapshotRecordContent
    /// A set of dids, describing the data composition of freeze frame
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01833
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< DataIdentifierSet > snapshotRecordContent{nullptr};
    /// @name extendedDataRecords
    /// Multiple EDRs contained in dtc
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01834
    /// @needwork = dd
    /// @endcode
    std::vector< std::shared_ptr< ExtendedDataRecord > > extendedDataRecords{};
    /// @name freezeFrames
    /// Multiple FreezeFrames contained in dtc
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01835
    /// @needwork = dd
    /// @endcode
    std::vector< std::shared_ptr< FreezeFrame > > freezeFrames{};
    /// @name memoryDestinations
    /// A dtc can be stored in multiple destinations respectively
    /// [SWS_DM_00083].
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01836
    /// @needwork = dd
    /// @endcode
    std::vector< std::shared_ptr< MemoryDestination > > memoryDestinations{};

    /// @brief Default constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01837
    /// @needwork = dd
    /// @endcode
    DtcProps() = default;

    /// @brief Deep copy constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01838
    /// @needwork = dd
    /// @endcode
    DtcProps(DtcProps const& other) = default;

    /// @brief Move constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01839
    /// @needwork = dd
    /// @endcode
    DtcProps(DtcProps&& other) = default;

    /// @brief Deep copy assignment operator
    /// @param[in] right DtcProps reference
    /// @return DtcProps reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01840
    /// @needwork = dd
    /// @endcode
    DtcProps& operator=(DtcProps const& right) = default;

    /// @brief Move assignment operator
    /// @param[in] right DtcProps rvalue reference
    /// @return DtcProps reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01841
    /// @needwork = dd
    /// @endcode
    DtcProps& operator=(DtcProps&& right) = default;

    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01842
    /// @needwork = dd
    /// @endcode
    ~DtcProps() = default;

    /// @brief Parse parameters from configuration
    /// @param[in] config DtcPropsConfig reference
    /// @param[in] ffMap All FreezeFrames parsed by previous logic, configured FreezeFrames need to be found here
    /// @param[in] edrMap All EDRs parsed by previous logic, configured EDRs need to be found here
    /// @param[in] agings All aging parsed by previous logic, configured aging needs to be found here
    /// @param[in] didSets All did sets parsed by previous logic, configured did sets need to be found here
    /// @param[in] memDestinations All destinations parsed by previous logic, configured destinations need to be found here
    /// @return Returns true on success, false on failure
    /// @code{.isoft}
    /// @throw unknown
    /// @needwork = dd
    /// @endcode
    bool Init(DtcPropsConfig& config,
              std::map< uint32_t, std::shared_ptr< FreezeFrame > >& ffMap,
              std::map< uint32_t, std::shared_ptr< ExtendedDataRecord > >& edrMap,
              std::map< uint32_t, std::shared_ptr< Aging > >& agings,
              std::map< uint32_t, std::shared_ptr< DataIdentifierSet > >& didSets,
              std::map< uint32_t, std::shared_ptr< MemoryDestination > >& memDestinations);
};

/// @brief dtc wrapper class
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02548
/// @needwork = dd
/// @endcode
class Dtc final : public std::enable_shared_from_this< Dtc >
{
public:
    /// @param[in] other Dtc reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00806
    /// @needwork = dda
    /// @endcode
    Dtc(Dtc const& other) = delete;
    /// @param[in] other Dtc rvalue reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00807
    /// @needwork = dda
    /// @endcode
    Dtc(Dtc&& other) = delete;
    /// @param[in] right Dtc reference
    /// @return Dtc reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00808
    /// @needwork = dda
    /// @endcode
    Dtc& operator=(Dtc const& right) = delete;
    /// @param[in] right Dtc rvalue reference
    /// @return Dtc reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00809
    /// @needwork = dda
    /// @endcode
    Dtc& operator=(Dtc&& right) = delete;

public:
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00810
    /// @needwork = dda
    /// @endcode
    Dtc() = delete;

    /// @brief Constructor
    /// @param[in] context FaultManagementRuntimeContext reference
    /// @code{.isoft}
    /// @throw unknown
    /// @needwork = dd
    /// @endcode
    explicit Dtc(FaultManagementRuntimeContext const& context);

    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01843
    /// @needwork = dd
    /// @endcode
    ~Dtc() = default;

    /// @brief Parse parameters from configuration
    /// @param[in] config DtcConfig reference
    /// @param[in] props DtcProps map reference
    /// @param[in] clearConditionGroups ConditionGroup map reference
    /// @return true or false
    /// @code{.isoft}
    /// @throw unknown
    /// @needwork = dd
    /// @endcode
    bool Init(DtcConfig const& config,
              std::map< uint32_t, std::shared_ptr< DtcProps > > const& props,
              std::map< uint32_t, std::shared_ptr< ConditionGroup > > const& clearConditionGroups);

    /// @brief Return dtc marker
    /// @return dtc marker
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01844
    /// @needwork = dd
    /// @endcode
    std::uint32_t GetID() const noexcept { return id_; }

    /// @brief Return dtc value
    /// @return dtc value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01845
    /// @needwork = dd
    /// @endcode
    std::uint32_t GetValue() const noexcept { return dtcValue_; }

    /// @brief Bind event
    /// @param[in] event The bound event
    /// @code{.isoft}
    /// @throw unknown
    /// @needwork = dd
    /// @endcode
    void SetEvent(std::shared_ptr< Event > const& event);

    /// @brief ResetStatus function
    /// @code{.isoft}
    /// @throw unknown
    /// @needwork = dd
    /// @endcode
    void ResetStatus() noexcept;

    /// @brief Return current dtc status
    /// @return Current dtc status
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01846
    /// @needwork = dd
    /// @endcode
    DTCStatus const& GetStatus() const noexcept;

    /// @brief Set dtc status
    /// @param[in] actionType Current fault situation
    /// @param[in] confirmThreshold Fault confirmation threshold configured in event
    /// @param[in] indicatorLock Whether the indicator bound to event is locked by AA side
    /// @param[in] hasIndicator Whether event has an indicator bound
    /// @param[in] recoverableInSameOperationCycle DTC status modification requires this value, which is configured in the bound event, passed in here
    /// @code{.isoft}
    /// @throw unknown
    /// @needwork = dd
    /// @endcode
    void SetStatus(MonitorAction const actionType,
                   std::uint32_t const confirmThreshold,
                   bool const indicatorLock,
                   bool const hasIndicator,
                   bool const recoverableInSameOperationCycle) noexcept;

    /// @brief Set dtc status to WIR
    /// @param[in] b set to 1 if true, set to 0 if false
    /// @code{.isoft}
    /// @throw unknown
    /// @needwork = dd
    /// @endcode
    void SetStatusWIR(bool const b);

    /// @brief When the operation cycle state of the bound event changes to start, it will also notify here to modify dtc status
    /// @code{.isoft}
    /// @throw unknown
    /// @needwork = dd
    /// @endcode
    void OpCycleStartedStatusChange() noexcept;

    /// @brief When the operation cycle state of the bound event changes to end, it will also notify here to modify dtc status
    /// @code{.isoft}
    /// @throw unknown
    /// @needwork = dd
    /// @endcode
    void OpCycleStoppedStatusChange() noexcept;

    /// @brief Execute clear operation for this dtc, mainly to handle its own state and notify the bound event to execute the corresponding clear operation
    /// @param[in] isFromUds Whether the clear operation is initiated by UDS service, false means initiated by AA side
    /// @code{.isoft}
    /// @throw unknown
    /// @needwork = dd
    /// @endcode
    void Clear(bool const isFromUds);

    /// @brief Check whether the storage timing is triggered, entry, freeze frame, edr, all are checked here, storage is performed when timing is satisfied
    /// @param[in] actionType Current fault situation
    /// @code{.isoft}
    /// @throw unknown
    /// @needwork = dd
    /// @endcode
    void CheckAndHandleStorage(MonitorAction const actionType, bool const testFaildFrom0To1);

    /// @brief Get the attribute in the configuration, whether to persist storage immediately
    /// @return true or false
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01847
    /// @needwork = dd
    /// @endcode
    bool IsImmediateNvDataStorage() const noexcept;

    /// @brief Determine whether clear is restricted, mainly depends on the current state of the clear condition group
    /// @return true or false
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01848
    /// @needwork = dd
    /// @endcode
    bool IsForbiddenClear() const noexcept;

    /// @brief Get the attribute in the bound event configuration, this attribute mainly affects some status bits when executing forbidden clear operation
    /// @return Return the attribute in the bound event configuration
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01849
    /// @needwork = dd
    /// @endcode
    DiagnosticClearEventAllowedBehaviorEnum GetEventClearAllowedBehavior() noexcept;

    /// @brief Get current FDC
    /// @return Return current FDC
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01850
    /// @needwork = dd
    /// @endcode
    std::int8_t GetCurrentFDC() noexcept;

    /// @brief Get the maximum FDC counted since the last clear
    /// @return Return maximum FDC
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01851
    /// @needwork = dd
    /// @endcode
    std::int8_t GetMaxFDCSinceLastClear() const noexcept;

    /// @brief Get the maximum FDC in the current operation cycle
    /// @return Return FDC
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01852
    /// @needwork = dd
    /// @endcode
    std::int8_t GetMaxFDCDuringCurrentCycle() const noexcept;

    /// @brief Get failed operation cycle count
    /// @return Return count
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01853
    /// @needwork = dd
    /// @endcode
    std::uint8_t GetFailedCycles() const noexcept;

    /// @brief Get operation cycle count after first failure
    /// @return Return count
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01854
    /// @needwork = dd
    /// @endcode
    std::uint8_t GetCyclesSinceFirstFailed() const noexcept;

    /// @brief Get operation cycle count after last failure
    /// @return Return count
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01855
    /// @needwork = dd
    /// @endcode
    std::uint8_t GetCyclesSinceLastFailed() const noexcept;

    /// @brief Get configured priority
    /// @return Return priority
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01856
    /// @needwork = dd
    /// @endcode
    std::uint8_t GetPriority() const noexcept;

    /// @brief Get dtc severity level
    /// @return Severity level
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01857
    /// @needwork = dd
    /// @endcode
    std::uint8_t GetSeverity() const noexcept;

    /// @brief Check whether aging function is configured
    /// @return true or false
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01858
    /// @needwork = dd
    /// @endcode
    bool HasAging() const noexcept;

    /// @brief Get aging threshold
    /// @return Return threshold
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01859
    /// @needwork = dd
    /// @endcode
    std::uint32_t GetAgingThreshold() const noexcept;

    /// @brief Convert configuration parameter DiagnosticSignificanceEnum to a specific numerical value, used as EDR data
    /// @return Return converted data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01860
    /// @needwork = dd
    /// @endcode
    std::uint8_t GetSignificance() const noexcept;

    /// @brief Get bound event marker
    /// @return Return marker
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01861
    /// @needwork = dd
    /// @endcode
    std::uint32_t GetEventID() const noexcept;

    /// @brief Get all snapshot identifiers
    /// @return Snapshot identifiers
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01862
    /// @needwork = dd
    /// @endcode
    std::vector< std::uint8_t > GetFreezeFrameNumberTable() noexcept;

    /// @brief Get a specific edr of dtc
    /// @return edr
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01863
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< ExtendedDataRecord > GetExtendedDataRecord(uint8_t const edrNumber) noexcept;

    /// @brief Reset current DTC status to dtcStatus
    /// @param[in] dtcStatus DTC status
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01864
    /// @needwork = dd
    /// @endcode
    void ResetStatusTo(DTCStatus const& dtcStatus) noexcept;

    /// @brief Set DtcSuppression
    /// @param const Parameter
    /// @return Return code
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01865
    /// @needwork = dd
    /// @endcode
    std::int32_t SetDtcSuppression(DtcSuppressionType const type) noexcept;

    /// @brief Get DtcSuppression
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01866
    /// @needwork = dd
    /// @endcode
    DtcSuppressionType GetDtcSuppression() noexcept;

    /// @brief IsDtcSuppressionOn function
    /// @return Operation result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01867
    /// @needwork = dd
    /// @endcode
    bool IsDtcSuppressionOn() noexcept;

private:
    /// @brief Register the clear condition group bound to this dtc
    /// @param[in] conditionGroup The clear condition group bound to this dtc
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01868
    /// @needwork = dd
    /// @endcode
    void _registerClearCondtionGroup(std::shared_ptr< ConditionGroup > const& conditionGroup) noexcept;

    /// @brief Register the bound aging cycle
    /// @code{.isoft}
    /// @throw unknown
    /// @needwork = dd
    /// @endcode
    void _registerAgingCycle();

    /// @brief When the state of aging cycle changes to end, call here to handle aging counting and dtc related status
    /// @code{.isoft}
    /// @throw unknown
    /// @needwork = dd
    /// @endcode
    void _processEndOfAgingCycle();

    /// @brief Process data capture, update and allocation logic of freeze frame in the specified entry
    /// @param[in] entry The entry to be processed
    /// @param[in] actionType Current fault situation
    /// @param[in] dest Assigned to this destination when allocation conditions are met
    /// @param[in] ssrs Updated snapshot records
    /// @param[in] enableAlloc
    /// @brief Whether to allow data allocation, the judgment of whether the entry can be allocated is performed, passed in here, false means capture and allocation cannot be triggered
    /// @return Returns true if capture and allocation are successful, otherwise false
    /// @code{.isoft}
    /// @throw unknown
    /// @needwork = dd
    /// @endcode
    bool _checkStorageFreezeFrames(std::shared_ptr< MemoryEntry > const& entry,
                                   MonitorAction const actionType,
                                   std::shared_ptr< MemoryDestination > const& dest,
                                   std::vector< SnapshotDataRecordType >& ssrs,
                                   bool const enableAlloc,
                                   bool const testFaildFrom0To1);

    /// @brief Process data capture, update and allocation logic of EDR in the specified entry
    /// @param[in] entry The entry to be processed
    /// @param[in] actionType Current fault situation
    /// @param[in] dest Assigned to this destination when allocation conditions are met
    /// @param[in] enableAlloc
    /// @brief Whether to allow data allocation, the judgment of whether the entry can be allocated is performed, passed in here, false means capture and allocation cannot be triggered
    /// @return Returns true if capture and allocation are successful, otherwise false
    /// @code{.isoft}
    /// @throw unknown
    /// @needwork = dd
    /// @endcode
    bool _checkStorageExtendDatas(std::shared_ptr< MemoryEntry > const& entry,
                                  MonitorAction const actionType,
                                  std::shared_ptr< MemoryDestination > const& dest,
                                  bool const enableAlloc);

    /// @brief Check whether the freeze frame or EDR of the current dtc meets the conditions for capturing and allocating data
    /// @param[in] trigger Freeze frame or EDR trigger
    /// @param[in] actionType Current fault situation
    /// @return Returns true if conditions are met, otherwise false
    /// @code{.isoft}
    /// @throw unknown
    /// @needwork = dd
    /// @endcode
    bool _canTrigger(DiagnosticRecordTriggerEnum const trigger, MonitorAction const actionType) const;

    /// @brief
    /// When allocating data to destination, severity in the configuration needs to be further converted to a specific numerical value. For specific conversion details, refer to iso14229-1 D.3
    /// @return Converted numerical value
    /// @code{.isoft}
    /// @throw unknown
    /// @needwork = dd
    /// @endcode
    std::uint8_t _getSeverityBitValue() const noexcept;

    /// @brief Build stored freeze frame structure
    /// @param in Input structure
    /// @param out Output structure
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01869
    /// @needwork = dd
    /// @endcode
    static void MakeStroedFFStruct(SnapshotDataRecordType const& in, std::vector< MemoryEntryFreezeRecord >& out);

private:
    /// @name id_
    /// unique identifier
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01870
    /// @needwork = dd
    /// @endcode
    std::uint32_t id_{UINT32_MAX};
    /// @name dtcValue_
    /// value of this DTC
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01871
    /// @needwork = dd
    /// @endcode
    std::uint32_t dtcValue_{0U};

    /// @name severity_
    /// DTC severity according to ISO 14229-1.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01872
    /// @needwork = dd
    /// @endcode
    DiagnosticUdsSeverityEnum severity_{};
    /// @name clearConditionGroup_
    /// a ClearCondition Group
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01873
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< ConditionGroup > clearConditionGroup_{nullptr};
    /// @name props_
    /// propties of this DTC
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01874
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< DtcProps > props_{};
    /// @name dtcStatusHandler_
    /// status handler of DTC
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01875
    /// @needwork = dd
    /// @endcode
    DTCStatusHandler dtcStatusHandler_{};
    /// @name event_
    /// pointer of related event
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01876
    /// @needwork = dd
    /// @endcode
    std::weak_ptr< Event > event_{};
    /// @name context_
    /// context of runtime in dem
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01877
    /// @needwork = dd
    /// @endcode
    FaultManagementRuntimeContext const& context_;
    /// @name mutexEvent_
    /// for thread safe
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01878
    /// @needwork = dd
    /// @endcode
    std::mutex mutexEvent_{};
    /// @name tripCounter_
    /// counter of confirm status
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01879
    /// @needwork = dd
    /// @endcode
    uint32_t tripCounter_{0U};

    /// @name suppressionType_
    /// suppression type of this DTC
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01880
    /// @needwork = dd
    /// @endcode
    DtcSuppressionType suppressionType_{DtcSuppressionType::kDTCSuppressionOff};
};

}  // namespace server
}  // namespace uds
}  // namespace isoft

#endif  // __ISOFT_UDS_FAULT_DTC_H__