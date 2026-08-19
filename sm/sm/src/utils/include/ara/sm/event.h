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
/// @brief      Define events and related structures
/// @details
/// @date       2024-06-25
/// @author     lianglongxiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/StateManagement/Utils
/// @unit_name=Event
/// @interface_level=module
/// @unit_description=Event definition
/// @trace_id_sr=SR_SM_01001, SR_SM_01002, SR_SM_01003, SR_SM_02001, SR_SM_02002, SR_SM_03002, SR_SM_04001, SR_SM_04005, SR_SM_04006, SR_SM_04007, SR_SM_04008,
/// SR_SM_04009, SR_SM_05003, SR_SM_05004, SR_SM_06001, SR_SM_06004, SR_SM_07001, SR_SM_07002, SR_SM_08001, SR_SM_09001, SR_SM_09008, SR_SM_10001
/// @endcode
///
/// ================================================================

#ifndef EVENT_H_
#define EVENT_H_

#include <ara/core/promise.h>
#include <ara/core/string.h>
#include <ara/core/variant.h>
#include <ara/core/vector.h>
#include <ara/exec/execution_client.h>
#include <ara/exec/execution_error_event.h>
#include <bits/stdint-uintn.h>

#include <chrono>
#include <functional>

#include "define.h"

namespace ara {
namespace sm {
namespace common {

/// @brief Network state internal type
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_05003, SR_SM_05004
/// @trace_id_ad=AD_SM_08123
/// @trace_id_dd=DD_SM_08473
/// @needwork = ad
/// @endcode
enum class NetworkStateInternalType : uint32_t
{
    /// @brief Enable network
    kFullCom = 0U,
    /// @brief Disable network
    kNoCom = 1U,
    /// @brief Consistent with network management
    kBusSleep = 2U,
    /// @brief Consistent with network management
    kPrepareBusSleep = 3U,
    /// @brief Consistent with network management
    kRepeatMessageState = 4U,
    /// @brief Consistent with network management
    kNormalOperation = 5U,
    /// @brief Consistent with network management
    kReadySleep = 6U
};

/// @brief Diagnostic reset request internal type
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_03002
/// @trace_id_ad=AD_SM_08124
/// @trace_id_dd=DD_SM_08474
/// @needwork = ad
/// @endcode
enum class DiagResetRequestInternalType : uint32_t
{
    /// @brief softReset
    kSoftReset = 0U,
    /// @brief hardReset
    kHardReset = 1U,
    /// @brief keyOffOnReset
    kKeyOffOnReset = 2U,
    /// @brief kCustomReset
    kCustomReset = 3U
};

/// @brief UMC function group list
/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
using UcmFunctionGroupListInternal = core::Vector< core::String >;

/// @brief Platform Health Management global supervision state
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_08001
/// @trace_id_ad=AD_SM_08125
/// @trace_id_dd=DD_SM_08475
/// @needwork = ad
/// @endcode
enum class PhmGlobalSupervisionStatusInternal : uint32_t
{
    kDeactivated = 0U,  ///< Global supervision not activated
    kOK          = 2U,  ///< All local supervision states under the global state are kOK or kDeactivated, global supervision is kOK
    kFailed      = 3U,  ///< At least one local supervision state is kFailed, but none is kExpired
    kExpired
    = 4U,  ///< At least one local supervision is in kExpired state, but the number of supervision cycles after this local supervision is kExpired is within the tolerance range, then the global state is kExpired
    kStopped
    = 5U,  ///< At least one local supervision is in kExpired state, and the number of supervision cycles after this local supervision is kExpired reaches or exceeds the tolerance range, then the global state is kStopped
};

/// @brief Platform Health Management global supervision type
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_08001
/// @trace_id_ad=AD_SM_08126
/// @trace_id_dd=DD_SM_08476
/// @needwork = ad
/// @endcode
enum class PhmTypeOfSupervisionInternal : uint32_t
{
    kAliveSupervision    = 0U,  ///< Basic supervision of type AliveSupervision
    kDeadlineSupervision = 1U,  ///< Basic supervision of type DeadlineSupervision
    kLogicalSupervision  = 2U   ///< Basic supervision of type LogicalSupervision
};

/// @brief Event type
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_01001, SR_SM_01002, SR_SM_01003, SR_SM_02001, SR_SM_02002, SR_SM_03002, SR_SM_04001, SR_SM_04005, SR_SM_04006, SR_SM_04007, SR_SM_04008,
/// SR_SM_04009, SR_SM_05003, SR_SM_05004, SR_SM_06001, SR_SM_06004, SR_SM_07001, SR_SM_07002, SR_SM_08001, SR_SM_09001, SR_SM_09008, SR_SM_10001
/// @trace_id_ad=AD_SM_08127
/// @trace_id_dd=DD_SM_08477
/// @needwork = ad
/// @endcode
enum class EventType : uint16_t
{
    kInShellSetFGState = 0x00U,  // Command line request to change the current state of a function group, event data uses FGStateInfo
    kInShellGetFGState = 0x01U,  // Command line request to get the current state of a function group, event data uses ara::core::String
    kInShellSetSMState = 0x02U,  // Command line request to change the current state of a state machine, event data uses SMStateInfo
    kInShellGetSMState = 0x03U,  // Command line request to get the current state of a state machine, event data uses ara::core::String
    kInUCMResetMachine = 0x04U,  // UCM's ResetMachine request
    kInUCMStartUpdateSession = 0x05U,  // UCM's StartUpdateSession request
    kInUCMStopUpdateSession  = 0x06U,  // UCM's StopUpdateSession request
    kInUCMPrepareUpdate      = 0x07U,  // UCM's PrepareUpdate request, event data uses FGListInfo
    kInUCMVerifyUpdate       = 0x08U,  // UCM's VerifyUpdate request, event data uses FGListInfo
    kInUCMPrepareRollback    = 0x09U,  // UCM's PrepareRollback request, event data uses FGListInfo
    // EM's StateClient's undefinedStateCallback notification, event data uses ara::exec::ExecutionErrorEvent
    kInEMUndefinedStateCallback = 0x0AU,
    // PHM's PHMSupervisionRecoveryNotification notification, event data uses PHMSupervisionRecoveryNotificationInfo
    kInPHMSupervisionRecoveryNotification = 0x0BU,
    // PHM's PHMHealthChannelRecoveryNotification notification, event data uses PHMHealthChannelRecoveryNotificationInfo
    kInPHMHealthChannelRecoveryNotification = 0x0CU,
    kInNMNetworkStateChange = 0x0DU,  // NM's NetworkCurrentStateChange notification, event data uses NetworkStateInfo
    kInAASetFGState         = 0x0EU,  // Application request to change the current state of a function group, event data uses FGStateInfo
    kInAAGetFGState  = 0x0FU,  // Application request to get the current state of a function group, event data uses ara::core::String
    kInGetAllFGState = 0x10U,  // PHM and IDSM request to get all function group states
    // SMControlApplication request to change the current state of a state machine through the state transition table, event data uses SMStateRequestInfo
    kInSMCSetSMState = 0x11U,
    // SMControlApplication or AA request to get the current state machine state, event data uses ara::core::String
    kInGetSMState              = 0x12U,
    kInDiagEnableRapidShutdown = 0x13U,  // Diagnostic's EnableRapidShutdown request, event data uses EnableRapidShutdownInfo
    kInDiagRequestReset        = 0x14U,  // Diagnostic's RequestReset request, event data uses DiagRequestResetInfo
    kInDiagExecuteReset        = 0x15U,  // Diagnostic's ExecuteReset request, event data uses DiagExecuteResetInfo
    kInAASetEcuState           = 0x16U,  // Other applications request to set the ECU state, event data uses ara::core::String
    kInGetEcuState             = 0x17U,  // Other applications request to get the ECU state, event data uses ara::core::String
    kOutNMSetNetworkState = 0x18U,  // State management requests NM to set its NetworkState, event data uses NetworkStateInfo
    kOutEMSetFGState      = 0x19U,  // State management requests EM to change the state of a function group, event data uses FGStateInfo
    kOutNotifyFGStateChange  = 0x1AU,  // State management notifies update of function group state, event data uses FGStateInfo
    kOutNotifySMStateChange  = 0x1BU,  // State management notifies update of state machine state, event data uses SMStateInfo
    kOutNotifyEcuStateChange = 0x1CU,  // State management notifies update of ECU state, event data uses ara::core::String
    kOutReportExecutionState = 0x1DU,  // State management reports execution state to EM, event data uses ara::exec::ExecutionState
    kOutCheckInitialStateTransition = 0x1FU,  // State management queries EM for initial state transition result
    kInShellGetAllFGInfos = 0x20U,  // Command line request to get all function group information, event data uses ara::core::Vector<FunctionGroupsInfoSM>
    kUnknown = 0xFFU,
};

/// @brief a Struct for event process
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_01001, SR_SM_01002, SR_SM_01003, SR_SM_02001, SR_SM_02002, SR_SM_03002, SR_SM_04001, SR_SM_04005, SR_SM_04006, SR_SM_04007, SR_SM_04008,
/// SR_SM_04009, SR_SM_05003, SR_SM_05004, SR_SM_06001, SR_SM_06004, SR_SM_07001, SR_SM_07002, SR_SM_08001, SR_SM_09001, SR_SM_09008, SR_SM_10001
/// @trace_id_ad=AD_SM_08128
/// @trace_id_dd=DD_SM_08478
/// @needwork = ad
/// @endcode
struct Event
{
    /// @brief Event type
    EventType type{EventType::kUnknown};

    /// @brief event data, note: must be deleted after use
    void *data{nullptr};

    /// @brief Promise corresponding to the event request, result is a void promise
    core::Promise< void > requestPromise;

    /// @brief Promise corresponding to the event request, result is a core::String promise
    core::Promise< core::String > requestPromiseWithStr;

    /// @brief Promise corresponding to the event request, result is an ara::core::Future<void> promise, used for asynchronously setting function group state or network state
    core::Promise< core::Future< void > > requestPromiseWithFuture;
};

/// @brief Data for function group state event
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_01001, SR_SM_01002, SR_SM_01003, SR_SM_09001, SR_SM_09008, SR_SM_10001
/// @trace_id_ad=AD_SM_08129
/// @trace_id_dd=DD_SM_08479
/// @needwork = ad
/// @endcode
struct FGStateInfo
{
    /// @brief Function group name
    core::String fgFQN;

    /// @brief Function group state
    core::String fgState;
};

/// @brief Data for state machine state event
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr= SR_SM_09001, SR_SM_09008
/// @trace_id_ad=AD_SM_08130
/// @trace_id_dd=DD_SM_08480
/// @needwork = ad
/// @endcode
struct SMStateInfo
{
    /// @brief State machine name
    core::String smFQN;

    /// @brief State machine state
    core::String smState;
};

/// @brief Data for state machine request transition event
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_09001
/// @trace_id_ad=AD_SM_08131
/// @trace_id_dd=DD_SM_08481
/// @needwork = ad
/// @endcode
struct SMStateRequestInfo
{
    /// @brief State machine name
    core::String smFQN;

    /// @brief Transition request
    std::uint32_t transitionRequest;
};

/// @brief Data for function group list, used for interaction with UCM
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_04007, SR_SM_04008, SR_SM_04009,
/// @trace_id_ad=AD_SM_08132
/// @trace_id_dd=DD_SM_08482
/// @needwork = ad
/// @endcode
struct FGListInfo
{
    /// @brief List of function groups
    UcmFunctionGroupListInternal fgList;
};

/// @brief PHMSupervisionRecoveryNotification information
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_08001
/// @trace_id_ad=AD_SM_08133
/// @trace_id_dd=DD_SM_08483
/// @needwork = ad
/// @endcode
struct PHMSupervisionRecoveryNotificationInfo
{
    /// @brief Execution error
    ara::exec::ExecutionErrorEvent executionError;

    /// @brief Supervision type
    PhmTypeOfSupervisionInternal supervisionType;

    /// @brief Function handle for obtaining the global supervision state
    std::function< core::Result< PhmGlobalSupervisionStatusInternal >() > getGlobalSupervisionStatusHandler{nullptr};
};

/// @brief Health management channel error recovery notification information
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_08001
/// @trace_id_ad=AD_SM_08134
/// @trace_id_dd=DD_SM_08484
/// @needwork = ad
/// @endcode
struct PHMHealthChannelRecoveryNotificationInfo
{
    /// @brief Health status type
    common::PHMHealthStatusType healthStatusType;

    /// @brief Health state
    uint32_t healthStatus;
};

/// @brief Network state information
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_05003, SR_SM_05004
/// @trace_id_ad=AD_SM_08135
/// @trace_id_dd=DD_SM_08485
/// @needwork = ad
/// @endcode
struct NetworkStateInfo
{
    /// @brief Network name
    core::String nmHandleName;

    /// @brief Network state
    NetworkStateInternalType state;
};

/// @brief Diagnostic management reset request information
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr= SR_SM_03002
/// @trace_id_ad=AD_SM_08136
/// @trace_id_dd=DD_SM_08486
/// @needwork = ad
/// @endcode
struct DiagRequestResetInfo
{
    /// @brief Diagnostic management reset type
    DiagResetRequestInternalType resetType;
    /// @brief Diagnostic management address type
    core::String addressType;
    /// @brief Diagnostic management address
    int32_t address;
};

/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
struct FunctionGroupInfoSM
{
    ara::core::String fgFQN;
    ara::core::Vector< core::String > fgStates;
};

/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
struct TransitionItemSM
{
    uint32_t requestID;
    core::String currentState;
    core::String nextState;
};

/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
struct FunctionGroupsInfoSM
{
    bool isManagedBySm{false};
    ara::core::String smFQN;
    ara::core::Vector< core::String > smStates;
    ara::core::Vector< FunctionGroupInfoSM > fgInfos;
    ara::core::Vector< TransitionItemSM > transitionTable;
};

}  // namespace common
}  // namespace sm
}  // namespace ara

#endif  // EVENT_H_
