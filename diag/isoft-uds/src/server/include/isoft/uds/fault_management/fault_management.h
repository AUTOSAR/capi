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
/// @file       fault_management.h
/// @brief      This file provides fault_management functionality
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
/// @trace_id_sr=4a8591e3-7296-43fb-aae5-6aebec23090f
/// @unit_name=FaultManagement
/// @unit_description=This file provides fault_management functionality
/// @endcode
///
/// ================================================================

#ifndef ISOFT_UDS_SERVER_CONFIG_DIAGNOSTICREADDTCINFORMATION_H_
#define ISOFT_UDS_SERVER_CONFIG_DIAGNOSTICREADDTCINFORMATION_H_
#include <map>
#include <memory>
#include <set>
#include <string>

#include "condition_interface.h"
#include "dtc_information_interface.h"
#include "event_interface.h"
#include "indicator_interface.h"
#include "isoft/uds/authentication_management/authentication_state.h"
#include "isoft/uds/data_management/environmental_condition_model.h"
#include "isoft/uds/fault_management/monitor_interface.h"
#include "isoft/uds/persistence_interface.h"
#include "isoft/uds/security_access_management/security_level.h"
#include "isoft/uds/session_management/session_model.h"
#include "isoft/uds/type_int.h"
#include "monitor_interface.h"
#include "operation_cycle_interface.h"

namespace isoft {
namespace uds {
namespace server {

/// @brief Primary target ID
/// @code{.isoft}
/// @note None
/// @interface_level=unit
/// @needwork=ad
/// @endcode
static std::uint32_t const kPrimaryDestinationId{kInt32_256U};

/// @brief All DTC group IDs
/// @code{.isoft}
/// @note None
/// @interface_level=unit
/// @needwork=ad
/// @endcode
static uint32_t const kGroupOfAllDTCs{0xFFFFFFU};

/// @brief Read DTC information subfunction
/// @code{.isoft}
/// @note None
/// @interface_level=unit
/// @needwork=ad
/// @endcode
enum class ReadDTCInformationSubfunction : std::uint8_t
{
    kReportNumberOfDtcByStatusMask                   = 0x01U,
    kReportDtcByStatusMask                           = 0x02U,
    kReportDtcSnapshotIdentification                 = 0x03U,
    kReportDtcSnapshotRecordByDtcNumber              = 0x04U,
    kReportDtcExtDataRecordByDtcNumber               = 0x06U,
    kReportNumberOfDtcBySeverityMaskRecord           = 0x07U,
    kReportSupportedDtcs                             = 0x0AU,
    kReportDtcFaultDetectionCounter                  = 0x14U,
    kReportUserDefMemoryDtcByStatusMask              = 0x17U,
    kReportUserDefMemoryDtcSnapshotRecordByDtcNumber = 0x18U,
    kReportUserDefMemoryDtcExtDataRecordByDtcNumber  = 0x19U,
};

/// @brief Read DTC information instance
/// @code{.isoft}
/// @note None
/// @interface_level=unit
/// @needwork=ad
/// @endcode
struct ReadDTCInformationInstance
{
public:
    /// @brief Subfunction number
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01163
    /// @trace_id_dd=DD_UDS_02277
    /// @needwork = ad
    /// @endcode
    ReadDTCInformationSubfunction subfunctionNumber{ReadDTCInformationSubfunction::kReportNumberOfDtcByStatusMask};
    /// @brief Access permission
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01164
    /// @trace_id_dd=DD_UDS_02278
    /// @needwork = ad
    /// @endcode
    EnableAuth accessPermissionAuth{};
    /// @brief Access permission session ID
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01165
    /// @trace_id_dd=DD_UDS_02279
    /// @needwork = ad
    /// @endcode
    std::set< SessionId > accessPermissionSession{};
    /// @brief Access permission security level
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01166
    /// @trace_id_dd=DD_UDS_02280
    /// @needwork = ad
    /// @endcode
    std::set< SecurityLevelId > accessPermissionSecurityLevel{};
    /// @brief Comparison operator
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01167
    /// @trace_id_dd=DD_UDS_02281
    /// @needwork = ad
    /// @endcode
    bool operator<(const ReadDTCInformationInstance &other) const
    {
        return subfunctionNumber < other.subfunctionNumber;
    }
};

/// @brief Read DTC information instance configuration
/// @code{.isoft}
/// @note None
/// @interface_level=unit
/// @needwork=ad
/// @endcode
struct ReadDTCInformationInstanceConfig : public ServiceConfigBase
{
    /// @brief Read DTC information instance
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01168
    /// @trace_id_dd=DD_UDS_02282
    /// @needwork = ad
    /// @endcode
    std::set< ReadDTCInformationInstance > instances;
};

/// @brief Control DTC setting subfunction
/// @code{.isoft}
/// @note None
/// @interface_level=unit
/// @needwork=ad
/// @endcode
enum class ControlDTCSettingSubfunction : std::uint8_t
{
    kOn  = 0x01U,
    kOff = 0x02U,
};

/// @brief Control DTC setting instance
/// @code{.isoft}
/// @note None
/// @interface_level=unit
/// @needwork=ad
/// @endcode
struct ControlDTCSettingInstance
{
public:
    /// @brief Subfunction number
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01169
    /// @trace_id_dd=DD_UDS_02283
    /// @needwork = ad
    /// @endcode
    ControlDTCSettingSubfunction subfunctionNumber{ControlDTCSettingSubfunction::kOn};
    /// @brief Access permission
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01170
    /// @trace_id_dd=DD_UDS_02284
    /// @needwork = ad
    /// @endcode
    EnableAuth accessPermissionAuth{};
    /// @brief Access permission session ID
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01171
    /// @trace_id_dd=DD_UDS_02285
    /// @needwork = ad
    /// @endcode
    std::set< SessionId > accessPermissionSession{};
    /// @brief Access permission security level
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01172
    /// @trace_id_dd=DD_UDS_02286
    /// @needwork = ad
    /// @endcode
    std::set< SecurityLevelId > accessPermissionSecurityLevel{};
    /// @brief Comparison operator
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01173
    /// @trace_id_dd=DD_UDS_02287
    /// @needwork = ad
    /// @endcode
    bool operator<(const ControlDTCSettingInstance &other) const { return subfunctionNumber < other.subfunctionNumber; }
};

/// @brief Control DTC setting instance configuration
/// @code{.isoft}
/// @note None
/// @interface_level=unit
/// @needwork=ad
/// @endcode
struct ControlDTCSettingInstanceConfig : public ServiceConfigBase
{
    /// @brief Control DTC setting instance
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01174
    /// @trace_id_dd=DD_UDS_02288
    /// @needwork = ad
    /// @endcode
    std::set< ControlDTCSettingInstance > instances;
};

/// @brief Clear diagnostic information instance configuration
/// @code{.isoft}
/// @note None
/// @interface_level=unit
/// @needwork=ad
/// @endcode
struct ClearDiagnosticInformationInstanceConfig : public ServiceConfigBase
{
public:
    /// @brief Access permission
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01175
    /// @trace_id_dd=DD_UDS_02289
    /// @needwork = ad
    /// @endcode
    EnableAuth accessPermissionAuth{};
    /// @brief Access permission session ID
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01176
    /// @trace_id_dd=DD_UDS_02290
    /// @needwork = ad
    /// @endcode
    std::set< SessionId > accessPermissionSession{};
    /// @brief Access permission security level
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01177
    /// @trace_id_dd=DD_UDS_02291
    /// @needwork = ad
    /// @endcode
    std::set< SecurityLevelId > accessPermissionSecurityLevel{};
    /// @brief Access permission environmental condition
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01178
    /// @trace_id_dd=DD_UDS_02292
    /// @needwork = ad
    /// @endcode
    EnvironmentalConditionId accessPermissionEnvCondition{0};
};

/// @brief Diagnostic clear DTC limitation
/// @code{.isoft}
/// @note None
/// @interface_level=unit
/// @needwork=ad
/// @endcode
enum class DiagnosticClearDtcLimitationEnum
{
    kAllSupportedDtcs = 0,  // DEM_ClearDtc API accepts all supported DTC values.
    kClearAllDtcs     = 1,  // DEM_ClearDtc API accepts ClearAllDTCs only.
};

/// @brief Diagnostic occurrence count processing
/// @code{.isoft}
/// @note None
/// @interface_level=unit
/// @needwork=ad
/// @endcode
enum class DiagnosticOccurrenceCounterProcessingEnum
{
    kConfirmedDtcBit = 0,  // The occurrence counter is triggered by the TestFailed bit if the fault confirmation was
                           // successful (ConfirmedDTC bit is set).
    kTestFailedBit = 1,    // The occurrence counter is only triggered by the TestFailed bit (and the fault confirmation
                           // is not considered).
};

/// @brief Diagnostic status bit handling TestFailedSinceLastClear
/// @code{.isoft}
/// @note None
/// @interface_level=unit
/// @needwork=ad
/// @endcode
enum class DiagnosticStatusBitHandlingTestFailedSinceLastClearEnum
{
    kStatusBitAgingAndDisplacement = 0,
    kStatusBitNormal               = 1,
};

/// @brief Diagnostic common attributes
/// @code{.isoft}
/// @note None
/// @interface_level=unit
/// @needwork=ad
/// @endcode
struct CommonProps final
{
public:
    /// @name agingRequiresTestedCycle
    /// Whether aging cycle counter should be processed in every aging cycle
    /// true : Only aging cycles of TestFailed will consider aging cycle counter processing
    /// false : Every aging cycle should consider aging cycle counter processing
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01179
    /// @trace_id_dd=DD_UDS_02293
    /// @needwork = ad
    /// @endcode
    bool agingRequiresTestedCycle{false};

    /// @name clearDtcLimitation
    /// Defines some limitations for clearing dtc
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01180
    /// @trace_id_dd=DD_UDS_02294
    /// @needwork = ad
    /// @endcode
    DiagnosticClearDtcLimitationEnum clearDtcLimitation{DiagnosticClearDtcLimitationEnum::kClearAllDtcs};

    /// @name occurrenceCounterProcessing
    /// This attribute defines different handling methods for fault confirmation of occurrence counter
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01181
    /// @trace_id_dd=DD_UDS_02295
    /// @needwork = ad
    /// @endcode
    DiagnosticOccurrenceCounterProcessingEnum occurrenceCounterProcessing{
        DiagnosticOccurrenceCounterProcessingEnum::kTestFailedBit};

    /// @name statusBitStorageTestFailed
    /// This attribute defines whether to persistently store the "TestFailed" status bit
    /// true : Store
    /// false : Do not store
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01182
    /// @trace_id_dd=DD_UDS_02296
    /// @needwork = ad
    /// @endcode
    bool statusBitStorageTestFailed{false};

    /// @name resetConfirmedBitOnOverflow
    /// This attribute defines whether to reset the "Confirm" status bit when Event Memory Entry is displaced
    /// true : Reset
    /// false : Do not reset
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01183
    /// @trace_id_dd=DD_UDS_02297
    /// @needwork = ad
    /// @endcode
    bool resetConfirmedBitOnOverflow{false};

    /// @name statusBitHandlingTestFailedSinceLastClear
    /// This attribute defines whether aging and displacement should handle the "TestFailedSinceLastClear" status bit
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01184
    /// @trace_id_dd=DD_UDS_02298
    /// @needwork = ad
    /// @endcode
    DiagnosticStatusBitHandlingTestFailedSinceLastClearEnum statusBitHandlingTestFailedSinceLastClear{
        DiagnosticStatusBitHandlingTestFailedSinceLastClearEnum::kStatusBitNormal};
};

/// @brief Diagnostic debounce type type
/// @code{.isoft}
/// @note None
/// @interface_level=unit
/// @needwork=ad
/// @endcode
enum class DebounceTypeEnum : uint8_t
{
    kCounterBase = 0,
    kTimeBase,
    kUserDefined,
    kUnknown
};

/// @brief Diagnostic debounce behavior
/// @code{.isoft}
/// @note None
/// @interface_level=unit
/// @needwork=ad
/// @endcode
enum class DiagnosticDebounceBehaviorEnum : uint8_t
{
    kFreeze
    = 0,  // The event debounce counter will be frozen with the current value and will not change while a related
          // enable condition is not fulfilled or ControlDTCSetting of the related event is disabled. After all
          // related enable conditions are fulfilled and ControlDTCSetting of the related event is enabled again, the
          /// @brief event function
          /// @return Return value
          // event qualification will continue with the next report of the event (i.e. SetEventStatus).
    kReset = 1,  // The event debounce counter will be reset to initial value if a related enable condition is not
                 // fulfilled or ControlDTCSetting of the related event is disabled. The qualification of the event will
                 // be restarted with the next valid event report.
};

/// @brief Diagnostic debounce CounterBased parameters
/// @code{.isoft}
/// @note None
/// @interface_level=unit
/// @needwork=ad
/// @endcode
struct CounterBasedParam final
{
public:
    /// @name counterBasedFdcThresholdStorageValue
    /// This threshold is used to allocate Event Memory Entry and capture Freeze Frame
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01185
    /// @trace_id_dd=DD_UDS_02299
    /// @needwork = ad
    /// @endcode
    std::int32_t counterBasedFdcThresholdStorageValue{0};

    CounterBased counter{};
};

/// @brief Diagnostic debounce TimeBased parameters
/// @code{.isoft}
/// @note None
/// @interface_level=unit
/// @needwork=ad
/// @endcode
struct TimeBasedParam final
{
public:
    /// @name timeBasedFdcThresholdStorageValue
    /// This threshold is used to allocate Event Memory Entry and capture Freeze Frame
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01186
    /// @trace_id_dd=DD_UDS_02300
    /// @needwork = ad
    /// @endcode
    uint32_t timeBasedFdcThresholdStorageValue{0U};  // second

    TimeBased timer{};
};

/// @brief Diagnostic debounce algorithm configuration
/// @code{.isoft}
/// @note None
/// @interface_level=unit
/// @needwork=ad
/// @endcode
struct DebounceAlgorithmConfig
{
    /// @name id
    /// Algorithm id
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01187
    /// @trace_id_dd=DD_UDS_02301
    /// @needwork = ad
    /// @endcode
    uint32_t algorithmId{0};

    /// @name type
    /// Algorithm type, if type is kCounterBase, read counterBased_ parameter, if type is kTimeBase, read timeBased_ parameter
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01188
    /// @trace_id_dd=DD_UDS_02302
    /// @needwork = ad
    /// @endcode
    DebounceTypeEnum type{DebounceTypeEnum::kUnknown};

    /// @name debounceBehavior
    /// If the Enable Condition of the related Event is not satisfied or ControlDTCSetting is disabled, this attribute defines the behavior of the algorithm
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01189
    /// @trace_id_dd=DD_UDS_02303
    /// @needwork = ad
    /// @endcode
    DiagnosticDebounceBehaviorEnum debounceBehavior{DiagnosticDebounceBehaviorEnum::kFreeze};

    /// @name debounceCounterStorage
    /// Whether to persistently store FDC
    /// true: Persistent storage
    /// false: Do not persistent storage
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01190
    /// @trace_id_dd=DD_UDS_02304
    /// @needwork = ad
    /// @endcode
    bool debounceCounterStorage{false};

    /// @name counterBased
    /// Configuration parameters for CounterBased debouncing algorithm
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01191
    /// @trace_id_dd=DD_UDS_02305
    /// @needwork = ad
    /// @endcode
    std::shared_ptr< CounterBasedParam > counterBased{nullptr};

    /// @name timeBased
    /// Configuration parameters for TimeBased debouncing algorithm
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01192
    /// @trace_id_dd=DD_UDS_02306
    /// @needwork = ad
    /// @endcode
    std::shared_ptr< TimeBasedParam > timeBased{nullptr};
};

/// @brief Diagnostic debounce behavior
/// @code{.isoft}
/// @note None
/// @interface_level=unit
/// @needwork=ad
/// @endcode
enum class DiagnosticClearEventAllowedBehaviorEnum : uint8_t
{
    kNoStatusByteChange        = 0,  // The event status byte keeps unchanged.
    kOnlyThisCycleAndReadiness = 1,  // The OperationCycle and readiness bits of the event status byte are reset.
};

/// @brief Diagnostic debounce behavior
/// @code{.isoft}
/// @note None
/// @interface_level=unit
/// @needwork=ad
/// @endcode
enum class DiagnosticConnectedIndicatorBehaviorEnum : uint8_t
{
    kBlinkMode               = 0,  // The indicator blinks when the event has status FAILED.
    kBlinkOrContinuousOnMode = 1,  // The indicator is active and blinks when the event has status FAILED.
    kContinuousOnMode        = 2,  // The indicator is active when the event has status FAILED.
    kFastFlashingMode        = 3,  // Flash Indicator Lamp should be set to "Fast Flash".
    kSlowFlashingMode        = 4,  // Flash Indicator Lamp should be set to "Slow Flash".
};

/// @brief Diagnostic debounce configuration
/// @code{.isoft}
/// @note None
/// @interface_level=unit
/// @needwork=ad
/// @endcode
struct ConnectedIndicatorConfig
{
public:
    uint32_t indicatorId{UINT32_MAX};
    uint32_t healingCycle{UINT32_MAX};
    DiagnosticConnectedIndicatorBehaviorEnum behavior{DiagnosticConnectedIndicatorBehaviorEnum::kBlinkMode};
    uint32_t healingCycleCounterThreshold{0U};
};

/// @brief Diagnostic debounce event configuration
/// @code{.isoft}
/// @note None
/// @interface_level=unit
/// @needwork=ad
/// @endcode
struct EventConfig
{
public:
    /// @name associatedEventIdentification
    /// Associated event identifier
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01193
    /// @trace_id_dd=DD_UDS_02307
    /// @needwork = ad
    /// @endcode
    uint32_t associatedEventIdentification{UINT32_MAX};

    /// @name clearEventAllowedBehavior
    /// Configured parameter, when dtc clear behavior occurs, this parameter is used as a dependency value for dtc status bit processing
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01194
    /// @trace_id_dd=DD_UDS_02308
    /// @needwork = ad
    /// @endcode
    DiagnosticClearEventAllowedBehaviorEnum clearEventAllowedBehavior{
        DiagnosticClearEventAllowedBehaviorEnum::kNoStatusByteChange};

    /// @name cycle count can be called trip counter, this logic will be mentioned in document ISO 14229-1 figure D.4
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01195
    /// @trace_id_dd=DD_UDS_02309
    /// @needwork = ad
    /// @endcode
    uint32_t confirmationThreshold{1U};

    /// @name Configured parameter, mainly affects the status change of event and dtc. When this parameter is true, when PASSED is received, the Failed
    /// status bit in the current operation cycle will be reset; when the parameter is false, the reported PASSED will be ignored, will not cause Failed reset logic;
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01196
    /// @trace_id_dd=DD_UDS_02310
    /// @needwork = ad
    /// @endcode
    bool recoverableInSameOperationCycle{false};

    /// @name debounceAlgorithm
    /// Debouncing algorithm ID, used to associate the specific debouncing algorithm configuration (DebounceAlgorithmConfig)
    /// This parameter defines the debouncing strategy used by the event, supporting CounterBased, TimeBased and other algorithm types
    /// Refer to AUTOSAR DEM (Diagnostic Event Manager) specification for Debounce Algorithm configuration:
    /// - Used to suppress signal jitter and avoid false alarms caused by noise
    /// - When FDC (Fault Detection Counter) reaches the threshold, the event status will change
    /// - Value UINT32_MAX indicates that no debouncing algorithm is configured
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01197
    /// @trace_id_dd=DD_UDS_02311
    /// @needwork = ad
    /// @endcode
    uint32_t debounceAlgorithm{UINT32_MAX};

    /// @name connectedIndicator
    /// Associated indicator configuration list, used to define the indicator behavior associated when the event is triggered
    /// Each configuration item contains parameters such as indicator ID, healing cycle, behavior mode, etc.
    /// Supports multiple indicator behavior modes: flashing mode, continuous on mode, fast flashing mode, slow flashing mode, etc.
    /// Associated indicator configuration
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01198
    /// @trace_id_dd=DD_UDS_02312
    /// @needwork = ad
    /// @endcode
    std::vector< ConnectedIndicatorConfig > connectedIndicator{};

    /// @name enableConditionGroup
    /// Enable condition group ID, used to associate a set of enable conditions
    /// This parameter defines a set of enable conditions required for the event to take effect. The event can only be processed when all conditions in the group are satisfied.
    /// Refer to AUTOSAR DEM specification for Enable Condition configuration:
    /// - Used to control the enable state of the event, preventing false fault reporting under specific conditions
    /// - For example: vehicle speed condition, engine speed condition, etc., fault reporting is only allowed when preset conditions are met
    /// - Value UINT32_MAX indicates that no enable condition group is configured, the event is always in enable state
    /// Typical application scenarios:
    /// - Do not detect certain sensor faults when the engine is not started
    /// - Do not detect wheel speed sensor faults when the vehicle speed is below the threshold
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01199
    /// @trace_id_dd=DD_UDS_02313
    /// @needwork = ad
    /// @endcode
    uint32_t enableConditionGroup{UINT32_MAX};

    /// @name operationCycle
    /// Operation cycle ID, used to associate a specific operation cycle configuration (OperationCycleConfig)
    /// This parameter defines the operation cycle when the event takes effect, supporting Ignition, ObdDrivingCycle, Other and other operation cycle types
    /// Refer to AUTOSAR DEM specification for Operation Cycle configuration:
    /// - Used to define the operation cycle when the event takes effect, affecting the timing of event triggering
    /// - Value UINT32_MAX indicates that no operation cycle is configured
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01200
    /// @trace_id_dd=DD_UDS_02314
    /// @needwork = ad
    /// @endcode
    uint32_t operationCycle{UINT32_MAX};

    /// @name troubleCodeUds
    /// Trouble code UDS configuration, used to define the trouble code UDS behavior when the event is triggered
    /// Refer to AUTOSAR DEM specification for Trouble Code UDS configuration:
    /// - Used to define the trouble code UDS behavior when the event is triggered, including sending, storing, clearing of trouble codes, etc.
    /// - Value UINT32_MAX indicates that no trouble code UDS is configured
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01201
    /// @trace_id_dd=DD_UDS_02315
    /// @needwork = ad
    /// @endcode
    uint32_t troubleCodeUds{UINT32_MAX};

    /// @name eventInterface
    /// Event interface pointer, used to call event-related methods
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01202
    /// @trace_id_dd=DD_UDS_02316
    /// @needwork = ad
    /// @endcode
    std::shared_ptr< EventInterface > eventInterface{nullptr};

    /// @name monitorInterface
    /// Monitor interface pointer, used to call monitor-related methods
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01203
    /// @trace_id_dd=DD_UDS_02317
    /// @needwork = ad
    /// @endcode
    std::shared_ptr< MonitorInterface > monitorInterface{nullptr};
};

/// @brief Enable condition configuration
/// @code{.isoft}
/// @endcode
/// @needwork = ad
struct ConditionConfig
{
public:
    /// @name enableConditionId
    /// @brief Enable condition ID
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01204
    /// @trace_id_dd=DD_UDS_02318
    /// @needwork = ad
    /// @endcode
    uint32_t enableConditionId{UINT32_MAX};

    /// @name state
    /// @brief Enable condition status
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01205
    /// @trace_id_dd=DD_UDS_02319
    /// @needwork = ad
    /// @endcode
    bool state{false};

    /// @name conditionInterface
    /// @brief Enable condition interface pointer, used to call enable condition related methods
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01206
    /// @trace_id_dd=DD_UDS_02320
    /// @needwork = ad
    /// @endcode
    std::shared_ptr< ConditionInterface > conditionInterface{nullptr};
};

/// @brief Operation cycle type enumeration
/// @code{.isoft}
/// @endcode
/// @needwork = ad
enum class DiagnosticOperationCycleTypeEnum : uint8_t
{
    kIgnition        = 0,  // Ignition ON / OFF cycle
    kObdDrivingCycle = 1,  // OBD Driving cycle
    kOther           = 2,  // further operation cycle
    kPower           = 3,  // Power ON / OFF cycle
    kTime            = 4,  // Time based operation cycle
    kWarmup          = 5,  // OBD Warm up cycle
};

/// @brief Operation cycle configuration
/// @code{.isoft}
/// @endcode
/// @needwork = ad
struct OperationCycleConfig
{
public:
    /// @name operationCycleId
    /// @brief Operation cycle ID
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01207
    /// @trace_id_dd=DD_UDS_02321
    /// @needwork = ad
    /// @endcode
    uint32_t operationCycleId{UINT32_MAX};

    /// @name autoStart
    /// @brief Automatically start operation cycle
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @needwork=ad
    /// true : Automatically start operation cycle
    /// false: Manually start operation cycle
    /// @endcode
    bool autoStart{false};

    /// @name autoEnd
    /// @brief Automatically end operation cycle
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @needwork=ad
    /// true : Automatically end operation cycle
    /// false: Manually end operation cycle
    /// @endcode
    bool autoEnd{false};
    /// @name cycleStatusStorage
    /// @brief Operation cycle status storage
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01208
    /// @trace_id_dd=DD_UDS_02322
    /// @needwork = ad
    /// @endcode
    bool cycleStatusStorage{false};
    /// @name type
    /// @brief Operation cycle type
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01209
    /// @trace_id_dd=DD_UDS_02323
    /// @needwork = ad
    /// @endcode
    DiagnosticOperationCycleTypeEnum type{DiagnosticOperationCycleTypeEnum::kOther};
    /// @name operationInterface
    /// @brief Operation cycle interface pointer, used to call operation cycle related methods
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01210
    /// @trace_id_dd=DD_UDS_02324
    /// @needwork = ad
    /// @endcode
    std::shared_ptr< OperationCycleInterface > operationInterface{nullptr};
};

/// @brief Indicator type enumeration
/// @code{.isoft}
/// @endcode
/// @needwork = ad
enum class DiagnosticIndicatorTypeEnum : uint8_t
{
    kAmberWarning = 0,  // Amber Warning Lamp
    kMalfunction  = 1,  // Malfunction Indicator Lamp
    kProtectLamp  = 2,  // Protect Lamp
    kRedStopLamp  = 3,  // Red Stop Lamp
    kWarning      = 4,  // Warning
};

/// @brief Indicator configuration
/// @code{.isoft}
/// @endcode
/// @needwork = ad
struct IndicatorConfig
{
public:
    /// @name indicatorId
    /// @brief Indicator ID
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01211
    /// @trace_id_dd=DD_UDS_02325
    /// @needwork = ad
    /// @endcode
    uint32_t indicatorId{UINT32_MAX};
    /// @name type
    /// @brief Indicator type
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01212
    /// @trace_id_dd=DD_UDS_02326
    /// @needwork = ad
    /// @endcode
    DiagnosticIndicatorTypeEnum type{DiagnosticIndicatorTypeEnum::kWarning};
    /// @name indicatorInterface
    /// @brief Indicator interface pointer, used to call indicator related methods
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01213
    /// @trace_id_dd=DD_UDS_02327
    /// @needwork = ad
    /// @endcode
    std::shared_ptr< IndicatorInterface > indicatorInterface{nullptr};
};

/// @brief Severity enumeration
/// @code{.isoft}
/// @endcode
/// @needwork = ad
enum class DiagnosticUdsSeverityEnum : uint8_t
{
    kCheckAtNextHalt = 0,  // Check at next halt.
    kImmediately     = 1,  // Check immediately.
    kMaintenanceOnly = 2,  // Maintenance required.
    kNoSeverity      = 3,  // No severity information available.
};

/// @brief DTC configuration
/// @code{.isoft}
/// @endcode
/// @needwork = ad
struct DtcConfig
{
public:
    /// @name dtcValue
    /// @brief DTC value
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01214
    /// @trace_id_dd=DD_UDS_02328
    /// @needwork = ad
    /// @endcode
    uint32_t dtcValue{UINT32_MAX};
    /// @name severity
    /// @brief Severity
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01215
    /// @trace_id_dd=DD_UDS_02329
    /// @needwork = ad
    /// @endcode
    DiagnosticUdsSeverityEnum severity{DiagnosticUdsSeverityEnum::kNoSeverity};
    /// @name clearConditionGroupId
    /// @brief Clear condition group ID
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01216
    /// @trace_id_dd=DD_UDS_02330
    /// @needwork = ad
    /// @endcode
    uint32_t clearConditionGroupId{UINT32_MAX};
    /// @name dtcPropsId
    /// @brief DTC attribute ID
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01217
    /// @trace_id_dd=DD_UDS_02331
    /// @needwork = ad
    /// @endcode
    uint32_t dtcPropsId{UINT32_MAX};
};

/// @brief TC attribute configuration
/// @code{.isoft}
/// @endcode
/// @needwork = ad
enum class DiagnosticSignificanceEnum : uint8_t
{
    kFault     = 0,  // Failure, which affects the component/ECU itself.
    kOccurence = 1,  // Issue, which indicates additional information concerning insufficient system behavior.
};

/// @brief DTC attribute configuration
/// @code{.isoft}
/// @endcode
/// @needwork = ad
struct DtcPropsConfig
{
public:
    /// @name dtcPropsId
    /// @brief DTC attribute ID
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01218
    /// @trace_id_dd=DD_UDS_02332
    /// @needwork = ad
    /// @endcode
    uint32_t dtcPropsID{UINT32_MAX};

    /// @name immediateNvDataStorage
    /// A switch, whether to persist storage entry immediately
    /// true : Persist storage immediately
    /// false: Do not persist storage immediately
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01219
    /// @trace_id_dd=DD_UDS_02333
    /// @needwork = ad
    /// @endcode
    bool immediateNvDataStorage{false};

    /// @name maxNumberFreezeFrameRecords
    /// The maximum number of freeze frame records an event is allowed to store
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01220
    /// @trace_id_dd=DD_UDS_02334
    /// @needwork = ad
    /// @endcode
    uint32_t maxNumberFreezeFrameRecords{0};

    /// @name priority
    /// Event priority, this value is needed when displacement strategy occurs, smaller value means higher priority
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01221
    /// @trace_id_dd=DD_UDS_02335
    /// @needwork = ad
    /// @endcode
    uint8_t priority{0};

    /// @name significance
    /// Indicates the specific meaning of the event, this type is used in EDR
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01222
    /// @trace_id_dd=DD_UDS_02336
    /// @needwork = ad
    /// @endcode
    DiagnosticSignificanceEnum significance{DiagnosticSignificanceEnum::kFault};

    /// @name snapshotRecordContent
    /// @brief Snapshot data record content
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01223
    /// @trace_id_dd=DD_UDS_02337
    /// @needwork = ad
    /// @endcode
    uint32_t snapshotRecordContent{UINT32_MAX};

    /// @name extendedDataRecord
    /// @brief Extended data record
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01224
    /// @trace_id_dd=DD_UDS_02338
    /// @needwork = ad
    /// @endcode
    std::set< uint32_t > extendedDataRecord{};

    /// @name freezeFrame
    /// @brief Freeze frame
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01225
    /// @trace_id_dd=DD_UDS_02339
    /// @needwork = ad
    /// @endcode
    std::set< uint32_t > freezeFrame{};

    /// @name memoryDestination
    /// @brief Memory destination
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01226
    /// @trace_id_dd=DD_UDS_02340
    /// @needwork = ad
    /// @endcode
    std::set< uint32_t > memoryDestination{};

    /// @name agingID
    /// @brief Aging ID
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01227
    /// @trace_id_dd=DD_UDS_02341
    /// @needwork = ad
    /// @endcode
    uint32_t agingID{UINT32_MAX};
};

/// @brief Aging configuration
/// @code{.isoft}
/// @endcode
/// @needwork = ad
struct AgingConfig
{
public:
    /// @name agingID
    /// @brief Aging ID
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01228
    /// @trace_id_dd=DD_UDS_02342
    /// @needwork = ad
    /// @endcode
    uint32_t agingID{UINT32_MAX};

    /// @name threshold
    /// @brief Aging threshold
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01229
    /// @trace_id_dd=DD_UDS_02343
    /// @needwork = ad
    /// @endcode
    uint32_t threshold{0U};

    /// @name agingCycle
    /// @brief Aging cycle
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01230
    /// @trace_id_dd=DD_UDS_02344
    /// @needwork = ad
    /// @endcode
    uint32_t agingCycle{UINT32_MAX};
};

/// @brief Diagnostic record trigger enumeration
/// @code{.isoft}
/// @endcode
/// @needwork = ad
enum class DiagnosticRecordTriggerEnum : uint8_t
{
    kConfirmed                    = 0,  // capture on "Confirmed"
    kFdcThreshold                 = 1,  // capture on "FDC Threshold"
    kPending                      = 2,  // capture on "Pending"
    kTestFailed                   = 3,  // capture on "Test Failed"
    kCustom                       = 4,  // implement custom capture
    kTestFailedThisOperationCycle = 5,  // Test Failed This Operation Cycle.
};

/// @brief Freeze frame configuration
/// @code{.isoft}
/// @endcode
/// @needwork = ad
struct FreezeFrameConfig
{
public:
    /// @name recordNumber
    /// @brief Freeze frame record number
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01231
    /// @trace_id_dd=DD_UDS_02345
    /// @needwork = ad
    /// @endcode
    uint32_t recordNumber{UINT32_MAX};

    /// @name trigger
    /// @brief Freeze frame trigger enumeration
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01232
    /// @trace_id_dd=DD_UDS_02346
    /// @needwork = ad
    /// @endcode
    DiagnosticRecordTriggerEnum trigger{DiagnosticRecordTriggerEnum::kConfirmed};

    /// @name update
    /// @brief Whether to update
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01233
    /// @trace_id_dd=DD_UDS_02347
    /// @needwork = ad
    /// @endcode
    bool update{false};

    /// @name customTrigger
    /// @brief Custom trigger
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01234
    /// @trace_id_dd=DD_UDS_02348
    /// @needwork = ad
    /// @endcode
    std::string customTrigger{};
};

/// @brief EDR data element classification enumeration
/// @code{.isoft}
/// @endcode
/// @needwork = ad
enum class EdrDataElementCategory : uint8_t
{
    kExternal                         = 0,
    kDemAgingctrDowncnt               = 1,
    kDemAgingctrUpcnt                 = 2,
    kDemCurrentFdc                    = 3,
    kDemCyclesSinceFirstFailed        = 4,
    kDemCyclesSinceLastFailed         = 5,
    kDemFailedCycles                  = 6,
    kDemMaxFdcDuringCurrentCycle      = 7,
    kDemMaxFdcSinceLastClear          = 8,
    kDemOccctr                        = 9,
    kDemOvflind                       = 10,
    kDemSignificance                  = 11,
    kDemPriority                      = 12,
    kDemEventAssociatedIdentification = 13,
};

/// @brief EDR data element configuration
/// @code{.isoft}
/// @endcode
/// @needwork = ad
struct EdrDataElementConfig
{
public:
    /// @name dataElementId
    /// @brief Data element ID
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01235
    /// @trace_id_dd=DD_UDS_02349
    /// @needwork = ad
    /// @endcode
    std::uint32_t dataElementId{};

    /// @name category
    /// @brief Data element classification
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01236
    /// @trace_id_dd=DD_UDS_02350
    /// @needwork = ad
    /// @endcode
    EdrDataElementCategory category{EdrDataElementCategory::kExternal};
};

/// @brief Extended data record configuration
/// @code{.isoft}
/// @endcode
/// @needwork = ad
struct ExtendedDataRecordConfig
{
public:
    /// @name recordNumber
    /// @brief Extended data record number
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01237
    /// @trace_id_dd=DD_UDS_02351
    /// @needwork = ad
    /// @endcode
    uint32_t recordNumber{UINT32_MAX};

    /// @name trigger
    /// @brief Extended data record trigger enumeration
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01238
    /// @trace_id_dd=DD_UDS_02352
    /// @needwork = ad
    /// @endcode
    DiagnosticRecordTriggerEnum trigger{DiagnosticRecordTriggerEnum::kConfirmed};

    /// @name update
    /// @brief Whether to update
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01239
    /// @trace_id_dd=DD_UDS_02353
    /// @needwork = ad
    /// @endcode
    bool update{false};

    /// @name customTrigger
    /// @brief Custom trigger
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01240
    /// @trace_id_dd=DD_UDS_02354
    /// @needwork = ad
    /// @endcode
    std::string customTrigger{};

    /// @name dataElementList
    /// @brief Data element list
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01241
    /// @trace_id_dd=DD_UDS_02355
    /// @needwork = ad
    /// @endcode
    std::vector< EdrDataElementConfig > dataElementList{};
};

/// @brief Event memory entry displacement strategy enumeration
/// @code{.isoft}
/// @endcode
/// @needwork = ad
enum class DiagnosticEventDisplacementStrategyEnum : std::uint8_t
{
    kFull = 0,     // Event memory entry displacement is enabled, by consideration of priority active/passive status,
                   // and occurrence.
    kNone    = 1,  // Event memory entry displacement is disabled.
    kPrioOcc = 2,  // Event memory entry displacement is enabled, by consideration of priority and occurrence (but
                   // without active/passive status).
};

/// @brief Event memory entry storage trigger enumeration
/// @code{.isoft}
/// @endcode
/// @needwork = ad
enum class DiagnosticMemoryEntryStorageTriggerEnum : std::uint8_t
{
    kConfirmed    = 0,  // Status information of UDS DTC status bit 3
    kFdcThreshold = 1,  // Threshold to allocate an event memory entry and to capture the Freeze Frame.
    kPending      = 2,  // Status information of UDS DTC status bit 2.
    kTestFailed   = 3,  // Status information of UDS DTC status bit 0.
};

/// @brief Freeze frame record number enumeration
/// @code{.isoft}
/// @endcode
/// @needwork = ad
enum class DiagnosticTypeOfFreezeFrameRecordNumerationEnum : uint8_t
{
    kCalculated = 0,  // Freeze frame records will be numbered consecutive starting by 1 in their chronological order.
    kConfigured
    = 1,  // Freeze frame records will be numbered based on the given configuration in their chronological order.
};

/// @brief Supported DTC format enumeration
/// @code{.isoft}
/// @endcode
/// @needwork = ad
enum class DiagnosticTypeOfDtcSupportedEnum : uint8_t
{
    kIso119924 = 0,   // ISO11992-4 DTC format
                      /// @brief format function
                      /// @param byte Parameter
                      /// @return Return value
    kIso142291 = 1,   // ISO14229-1 DTC format (3 byte format)
                      /// @brief format function
                      /// @param byte Parameter
                      /// @return Return value
    kIso150316  = 2,  // ISO15031-6 DTC format (2 byte format)
    kSaeJ193973 = 3,  // SAEJ1939-73 DTC format
    kSaeJ2012Da = 4,  // SAE_J2012-DA_DTCFormat_00 (3 byte format)
};

/// @brief Memory destination configuration
/// @code{.isoft}
/// @endcode
/// @needwork = ad
struct MemoryDestinationConfig
{
public:
    /// @name destinationID
    /// @brief Memory destination ID
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01242
    /// @trace_id_dd=DD_UDS_02356
    /// @needwork = ad
    /// @endcode
    uint32_t destinationID{kInt32_256U};

    /// @name dtcStatusAvailabilityMask
    /// @brief DTC status available mask
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01243
    /// @trace_id_dd=DD_UDS_02357
    /// @needwork = ad
    /// @endcode
    uint8_t dtcStatusAvailabilityMask{0U};

    /// @name displacementStrategy
    /// @brief Event memory entry displacement strategy enumeration
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01244
    /// @trace_id_dd=DD_UDS_02358
    /// @needwork = ad
    /// @endcode
    DiagnosticEventDisplacementStrategyEnum displacementStrategy{DiagnosticEventDisplacementStrategyEnum::kNone};

    /// @name maxNumberOfEventEntries
    /// @brief Maximum number of event memory entries
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01245
    /// @trace_id_dd=DD_UDS_02359
    /// @needwork = ad
    /// @endcode
    uint32_t maxNumberOfEventEntries{0U};

    /// @name memoryEntryStorageTrigger
    /// @brief Event memory entry storage trigger enumeration
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01246
    /// @trace_id_dd=DD_UDS_02360
    /// @needwork = ad
    /// @endcode
    DiagnosticMemoryEntryStorageTriggerEnum memoryEntryStorageTrigger{
        DiagnosticMemoryEntryStorageTriggerEnum::kTestFailed};

    /// @name typeOfFreezeFrameRecordNumeration
    /// @brief Freeze frame record number enumeration
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01247
    /// @trace_id_dd=DD_UDS_02361
    /// @needwork = ad
    /// @endcode
    DiagnosticTypeOfFreezeFrameRecordNumerationEnum typeOfFreezeFrameRecordNumeration{
        DiagnosticTypeOfFreezeFrameRecordNumerationEnum::kConfigured};

    /// @name dtcInformationInterface
    /// @brief DTC information interface
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01248
    /// @trace_id_dd=DD_UDS_02362
    /// @needwork = ad
    /// @endcode
    std::shared_ptr< DtcInformationInterface > dtcInformationInterface{nullptr};

    /// @name typeOfDtcSupported
    /// @brief Supported DTC format enumeration
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01249
    /// @trace_id_dd=DD_UDS_02363
    /// @needwork = ad
    /// @endcode
    DiagnosticTypeOfDtcSupportedEnum typeOfDtcSupported{DiagnosticTypeOfDtcSupportedEnum::kIso142291};
};

/// @brief Fault management configuration
/// @code{.isoft}
/// @endcode
/// @needwork = ad
struct FaultManagement
{
public:
    /// @name clusterName
    /// @brief Cluster name
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01250
    /// @trace_id_dd=DD_UDS_02364
    /// @needwork = ad
    /// @endcode
    std::string clusterName{};

    /// @name readDTCInformationInstanceConfig
    /// @brief Read DTC information instance configuration
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01251
    /// @trace_id_dd=DD_UDS_02365
    /// @needwork = ad
    /// @endcode
    ReadDTCInformationInstanceConfig readDTCInformationInstanceConfig;

    /// @name controlDTCSettingInstanceConfig
    /// @brief Control DTC setting instance configuration
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01252
    /// @trace_id_dd=DD_UDS_02366
    /// @needwork = ad
    /// @endcode
    ControlDTCSettingInstanceConfig controlDTCSettingInstanceConfig;

    /// @name clearDiagnosticInformationInstanceConfig
    /// @brief Clear diagnostic information instance configuration
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01253
    /// @trace_id_dd=DD_UDS_02367
    /// @needwork = ad
    /// @endcode
    ClearDiagnosticInformationInstanceConfig clearDiagnosticInformationInstanceConfig;

    /// @name commonProps
    /// @brief Common attributes
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01254
    /// @trace_id_dd=DD_UDS_02368
    /// @needwork = ad
    /// @endcode
    CommonProps commonProps{};

    /// @name memoryDestinationConfig
    /// @brief Memory destination configuration
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01255
    /// @trace_id_dd=DD_UDS_02369
    /// @needwork = ad
    /// @endcode
    std::map< uint32_t, MemoryDestinationConfig > memoryDestinationConfig{};

    /// @name eventConfig
    /// @brief Event configuration
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01256
    /// @trace_id_dd=DD_UDS_02370
    /// @needwork = ad
    /// @endcode
    std::map< uint32_t, EventConfig > eventConfig{};

    /// @name dtcConfig
    /// @brief DTC configuration
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01257
    /// @trace_id_dd=DD_UDS_02371
    /// @needwork = ad
    /// @endcode
    std::map< uint32_t, DtcConfig > dtcConfig{};

    /// @name dtcPropsConfig
    /// @brief DTC attribute configuration
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01258
    /// @trace_id_dd=DD_UDS_02372
    /// @needwork = ad
    /// @endcode
    std::map< uint32_t, DtcPropsConfig > dtcPropsConfig{};

    /// @name dtcGroup
    /// @brief DTC group
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01259
    /// @trace_id_dd=DD_UDS_02373
    /// @needwork = ad
    /// @endcode
    std::map< uint32_t, std::set< uint32_t > > dtcGroup{};

    /// @name enableConditionConfig
    /// @brief Enable condition configuration
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01260
    /// @trace_id_dd=DD_UDS_02374
    /// @needwork = ad
    /// @endcode
    std::map< uint32_t, ConditionConfig > enableConditionConfig{};

    /// @name enableConditionGroup
    /// @brief Enable condition group
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01261
    /// @trace_id_dd=DD_UDS_02375
    /// @needwork = ad
    /// @endcode
    std::map< uint32_t, std::set< uint32_t > > enableConditionGroup{};

    /// @name operationCycleConfig
    /// @brief Operation cycle configuration
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01262
    /// @trace_id_dd=DD_UDS_02376
    /// @needwork = ad
    /// @endcode
    std::map< uint32_t, OperationCycleConfig > operationCycleConfig{};

    /// @name indicatorConfig
    /// @brief Indicator configuration
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01263
    /// @trace_id_dd=DD_UDS_02377
    /// @needwork = ad
    /// @endcode
    std::map< uint32_t, IndicatorConfig > indicatorConfig{};

    /// @name agingConfig
    /// @brief Aging configuration
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01264
    /// @trace_id_dd=DD_UDS_02378
    /// @needwork = ad
    /// @endcode
    std::map< uint32_t, AgingConfig > agingConfig{};

    /// @name freezeFrameConfig
    /// @brief Freeze frame configuration
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01265
    /// @trace_id_dd=DD_UDS_02379
    /// @needwork = ad
    /// @endcode
    std::map< uint32_t, FreezeFrameConfig > freezeFrameConfig{};

    /// @name extendedDataRecordConfig
    /// @brief Extended data record configuration
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01266
    /// @trace_id_dd=DD_UDS_02380
    /// @needwork = ad
    /// @endcode
    std::map< uint32_t, ExtendedDataRecordConfig > extendedDataRecordConfig{};

    /// @name dataIdentifierSet
    /// @brief Data identifier set
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01267
    /// @trace_id_dd=DD_UDS_02381
    /// @needwork = ad
    /// @endcode
    std::map< uint32_t, std::set< uint16_t > > dataIdentifierSet{};

    /// @name clearConditionConfig
    /// @brief Clear condition configuration
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01268
    /// @trace_id_dd=DD_UDS_02382
    /// @needwork = ad
    /// @endcode
    std::map< uint32_t, ConditionConfig > clearConditionConfig{};

    /// @name clearConditionGroup
    /// @brief Clear condition group
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01269
    /// @trace_id_dd=DD_UDS_02383
    /// @needwork = ad
    /// @endcode
    std::map< uint32_t, std::set< uint32_t > > clearConditionGroup{};

    /// @name debounceAlgorithmConfig
    /// @brief Debounce algorithm configuration
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01270
    /// @trace_id_dd=DD_UDS_02384
    /// @needwork = ad
    /// @endcode
    std::map< uint32_t, DebounceAlgorithmConfig > debounceAlgorithmConfig{};
};

}  // namespace server
}  // namespace uds
}  // namespace isoft

#endif  /// ISOFT_UDS_SERVER_CONFIG_DIAGNOSTICREADDTCINFORMATION_H_