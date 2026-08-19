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
/// @file       state_machine_manager.h
/// @brief      Define the state machine management class
/// @details
/// @date       2024-05-07
/// @author     lianglongxiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/StateManagement/StateMachineManagement
/// @unit_name=StateMachineManager
/// @interface_level=module
/// @unit_description=Define the state machine management class
/// @trace_id_sr=SR_SM_00003, SR_SM_01001, SR_SM_01002, SR_SM_01003, SR_SM_02001, SR_SM_02002, SR_SM_03002, SR_SM_04001, SR_SM_04002, SR_SM_04003, SR_SM_04004,
/// SR_SM_04005, SR_SM_04006, SR_SM_04007, SR_SM_04008, SR_SM_04009, SR_SM_05003, SR_SM_05004, SR_SM_05005, SR_SM_06001, SR_SM_06002, SR_SM_06003, SR_SM_06004,
/// SR_SM_08001, SR_SM_09001, SR_SM_09002, SR_SM_09003, SR_SM_09004, SR_SM_09005, SR_SM_09006, SR_SM_09007, SR_SM_09008, SR_SM_10001
/// @endcode
///
/// ================================================================

#ifndef STATE_MACHINE_MANAGER_H_
#define STATE_MACHINE_MANAGER_H_

#include <ara/core/map.h>
#include <ara/core/promise.h>
#include <ara/core/string.h>
#include <ara/core/vector.h>
#include <ara/log/logger.h>
#include <ara/sm/error_domain_sm.h>

#include "config/global_config.h"
#include "event.h"
#include "state_machine.h"
#ifdef ARA_WITH_PER
    #include "storage/storage.h"
#endif
/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
#define SHUTDOWN_PREVENTION_TIMEOUT_MS 600

namespace ara {
namespace sm {
namespace state_machine_management {

/// @brief State machine manager
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_00003, SR_SM_01001, SR_SM_01002, SR_SM_01003, SR_SM_02001, SR_SM_02002, SR_SM_03002, SR_SM_04001, SR_SM_04002, SR_SM_04003, SR_SM_04004,
/// SR_SM_04005, SR_SM_04006, SR_SM_04007, SR_SM_04008, SR_SM_04009, SR_SM_05003, SR_SM_05004, SR_SM_05005, SR_SM_06001, SR_SM_06002, SR_SM_06003, SR_SM_06004,
/// SR_SM_08001, SR_SM_09001, SR_SM_09002, SR_SM_09003, SR_SM_09004, SR_SM_09005, SR_SM_09006, SR_SM_09007, SR_SM_09008, SR_SM_10001
/// @trace_id_ad=AD_SM_08088
/// @trace_id_dd=DD_SM_08288
/// @needwork = ad
/// @endcode
class StateMachineManager
{
public:
    /// @brief Constructor function
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_00003, SR_SM_01001, SR_SM_01002, SR_SM_01003, SR_SM_02001, SR_SM_02002, SR_SM_03002, SR_SM_04001, SR_SM_04002, SR_SM_04003,
    /// SR_SM_04004, SR_SM_04005, SR_SM_04006, SR_SM_04007, SR_SM_04008, SR_SM_04009, SR_SM_05003, SR_SM_05004, SR_SM_05005, SR_SM_06001, SR_SM_06002,
    /// SR_SM_06003, SR_SM_06004, SR_SM_08001, SR_SM_09001, SR_SM_09002, SR_SM_09003, SR_SM_09004, SR_SM_09005, SR_SM_09006, SR_SM_09007, SR_SM_09008,
    /// SR_SM_10001
    /// @trace_id_ad=AD_SM_00281
    /// @trace_id_dd=DD_SM_00297
    /// @needwork = ad
    /// @endcode
    StateMachineManager() noexcept;

    /// @brief Destructor function
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_00003, SR_SM_01001, SR_SM_01002, SR_SM_01003, SR_SM_02001, SR_SM_02002, SR_SM_03002, SR_SM_04001, SR_SM_04002, SR_SM_04003,
    /// SR_SM_04004, SR_SM_04005, SR_SM_04006, SR_SM_04007, SR_SM_04008, SR_SM_04009, SR_SM_05003, SR_SM_05004, SR_SM_05005, SR_SM_06001, SR_SM_06002,
    /// SR_SM_06003, SR_SM_06004, SR_SM_08001, SR_SM_09001, SR_SM_09002, SR_SM_09003, SR_SM_09004, SR_SM_09005, SR_SM_09006, SR_SM_09007, SR_SM_09008,
    /// SR_SM_10001
    /// @trace_id_ad=AD_SM_00282
    /// @trace_id_dd=DD_SM_00298
    /// @needwork = ad
    /// @endcode
    ~StateMachineManager() noexcept;
    /// @brief deleted copy constructor function
    /// @param other The StateMachineManager instance to be copyed
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_00003, SR_SM_01001, SR_SM_01002, SR_SM_01003, SR_SM_02001, SR_SM_02002, SR_SM_03002, SR_SM_04001, SR_SM_04002, SR_SM_04003,
    /// SR_SM_04004, SR_SM_04005, SR_SM_04006, SR_SM_04007, SR_SM_04008, SR_SM_04009, SR_SM_05003, SR_SM_05004, SR_SM_05005, SR_SM_06001, SR_SM_06002,
    /// SR_SM_06003, SR_SM_06004, SR_SM_08001, SR_SM_09001, SR_SM_09002, SR_SM_09003, SR_SM_09004, SR_SM_09005, SR_SM_09006, SR_SM_09007, SR_SM_09008,
    /// SR_SM_10001
    /// @trace_id_ad=AD_SM_00283
    /// @trace_id_dd=DD_SM_00299
    /// @needwork = ad
    /// @endcode
    StateMachineManager(StateMachineManager const &other) = delete;

    /// @brief deleted copy assignment function
    /// @param other The StateMachineManager instance to be copyed
    /// @return the assigned StateMachineManager instance itself
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_00003, SR_SM_01001, SR_SM_01002, SR_SM_01003, SR_SM_02001, SR_SM_02002, SR_SM_03002, SR_SM_04001, SR_SM_04002, SR_SM_04003,
    /// SR_SM_04004, SR_SM_04005, SR_SM_04006, SR_SM_04007, SR_SM_04008, SR_SM_04009, SR_SM_05003, SR_SM_05004, SR_SM_05005, SR_SM_06001, SR_SM_06002,
    /// SR_SM_06003, SR_SM_06004, SR_SM_08001, SR_SM_09001, SR_SM_09002, SR_SM_09003, SR_SM_09004, SR_SM_09005, SR_SM_09006, SR_SM_09007, SR_SM_09008,
    /// SR_SM_10001
    /// @trace_id_ad=AD_SM_00284
    /// @trace_id_dd=DD_SM_00300
    /// @needwork = ad
    /// @endcode
    StateMachineManager &operator=(StateMachineManager const &other) = delete;

    /// @brief Move constructor function
    /// @param other The StateMachineManager instance to be moved
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_00003, SR_SM_01001, SR_SM_01002, SR_SM_01003, SR_SM_02001, SR_SM_02002, SR_SM_03002, SR_SM_04001, SR_SM_04002, SR_SM_04003,
    /// SR_SM_04004, SR_SM_04005, SR_SM_04006, SR_SM_04007, SR_SM_04008, SR_SM_04009, SR_SM_05003, SR_SM_05004, SR_SM_05005, SR_SM_06001, SR_SM_06002,
    /// SR_SM_06003, SR_SM_06004, SR_SM_08001, SR_SM_09001, SR_SM_09002, SR_SM_09003, SR_SM_09004, SR_SM_09005, SR_SM_09006, SR_SM_09007, SR_SM_09008,
    /// SR_SM_10001
    /// @trace_id_ad=AD_SM_00285
    /// @trace_id_dd=DD_SM_00301
    /// @needwork = ad
    /// @endcode
    StateMachineManager(StateMachineManager &&other) = default;

    /// @brief Move assignment function
    /// @param other The StateMachineManager instance to be moved
    /// @return the assigned StateMachineManager instance itself
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_00003, SR_SM_01001, SR_SM_01002, SR_SM_01003, SR_SM_02001, SR_SM_02002, SR_SM_03002, SR_SM_04001, SR_SM_04002, SR_SM_04003,
    /// SR_SM_04004, SR_SM_04005, SR_SM_04006, SR_SM_04007, SR_SM_04008, SR_SM_04009, SR_SM_05003, SR_SM_05004, SR_SM_05005, SR_SM_06001, SR_SM_06002,
    /// SR_SM_06003, SR_SM_06004, SR_SM_08001, SR_SM_09001, SR_SM_09002, SR_SM_09003, SR_SM_09004, SR_SM_09005, SR_SM_09006, SR_SM_09007, SR_SM_09008,
    /// SR_SM_10001
    /// @trace_id_ad=AD_SM_00286
    /// @trace_id_dd=DD_SM_00302
    /// @needwork = ad
    /// @endcode
    StateMachineManager &operator=(StateMachineManager &&other) = delete;

    /// @brief Register the callback function for appending events
    /// @param appendEventHandler Callback function for appending events
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_01001, SR_SM_01003, SR_SM_02002, SR_SM_05004, SR_SM_06001, SR_SM_09008
    /// @trace_id_ad=AD_SM_00287
    /// @trace_id_dd=DD_SM_00303
    /// @needwork = ad
    /// @endcode
    void RegisterAppendEventHandler(std::function< void(common::Event &&) > const &appendEventHandler) noexcept;

    /// @brief Register the callback function for obtaining execution errors
    /// @param getExecutionErrorHandler Callback function for obtaining execution errors
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_06004
    /// @trace_id_ad=AD_SM_00288
    /// @trace_id_dd=DD_SM_00304
    /// @needwork = ad
    /// @endcode
    void RegisterGetExecutionErrorHandler(
        std::function< core::Result< exec::ExecutionErrorEvent >(core::String const &) > const
            &getExecutionErrorHandler) noexcept;

    /// @brief Initialize
    /// @return true - success
    /// @return false - failure
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_00003, SR_SM_01001, SR_SM_01002, SR_SM_01003, SR_SM_02001, SR_SM_02002, SR_SM_03002, SR_SM_04001, SR_SM_04002, SR_SM_04003,
    /// SR_SM_04004, SR_SM_04005, SR_SM_04006, SR_SM_04007, SR_SM_04008, SR_SM_04009, SR_SM_05003, SR_SM_05004, SR_SM_05005, SR_SM_06001, SR_SM_06002,
    /// SR_SM_06003, SR_SM_06004, SR_SM_08001, SR_SM_09001, SR_SM_09002, SR_SM_09003, SR_SM_09004, SR_SM_09005, SR_SM_09006, SR_SM_09007, SR_SM_09008,
    /// SR_SM_10001
    /// @trace_id_ad=AD_SM_00289
    /// @trace_id_dd=DD_SM_00305
    /// @needwork = ad
    /// @endcode
    bool Init() noexcept;

    /// @brief Start state management
    /// @return true - success
    /// @return false - failure
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_00003, SR_SM_01001, SR_SM_01002, SR_SM_01003, SR_SM_02001, SR_SM_02002, SR_SM_03002, SR_SM_04001, SR_SM_04002, SR_SM_04003,
    /// SR_SM_04004, SR_SM_04005, SR_SM_04006, SR_SM_04007, SR_SM_04008, SR_SM_04009, SR_SM_05003, SR_SM_05004, SR_SM_05005, SR_SM_06001, SR_SM_06002,
    /// SR_SM_06003, SR_SM_06004, SR_SM_08001, SR_SM_09001, SR_SM_09002, SR_SM_09003, SR_SM_09004, SR_SM_09005, SR_SM_09006, SR_SM_09007, SR_SM_09008,
    /// SR_SM_10001
    /// @trace_id_ad=AD_SM_00290
    /// @trace_id_dd=DD_SM_00306
    /// @needwork = ad
    /// @endcode
    bool Start() noexcept;

    /// @brief Respond to network state changes
    /// @param nmHandleName Network name
    /// @param state Network state
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_05004
    /// @trace_id_ad=AD_SM_00292
    /// @trace_id_dd=DD_SM_00308
    /// @needwork = ad
    /// @endcode
    void NotifyNetworkStateChanged(core::String const &nmHandleName,
                                   common::NetworkStateInternalType const &state) noexcept;

    /// @brief Switch function group state
    /// Switching function group state externally is not supported in the following two cases:
    /// 1. The function group is managed by a state machine with a state transition table. In this case, the external should switch the function group by switching the state machine.
    /// 2
    /// The function group is managed by a state machine without a state transition table, but this state machine also manages at least 2 function groups including this one. In this case, the user is prompted to create a state transition table and switch the function group by switching the state machine externally.
    /// @param fgFQN Function group name
    /// @param fgState Function group state
    /// @param source Source of the function group transition request
    /// @param promise Used to asynchronously set the execution result
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_00003, SR_SM_01002, SR_SM_01003, SR_SM_10001
    /// @trace_id_ad=AD_SM_00294
    /// @trace_id_dd=DD_SM_00310
    /// @needwork = ad
    /// @endcode
    void ChangeFGState(core::String const &fgFQN,
                       core::String const &fgState,
                       FGStateChangeSource const source,
                       core::Promise< void > &&promise,
                       uint64_t const &nmHandleId = 0) noexcept;

    /// @brief Switch function group state
    /// Switching function group state externally is not supported in the following two cases:
    /// 1. The function group is managed by a state machine with a state transition table. In this case, the external should switch the function group by switching the state machine.
    /// 2
    /// The function group is managed by a state machine without a state transition table, but this state machine also manages at least 2 function groups including this one. In this case, the user is prompted to create a state transition table and switch the function group by switching the state machine externally.
    /// @param fgFQN Function group name
    /// @param fgState Function group state
    /// @param source Source of the function group transition request
    /// @param promise Used to asynchronously set the execution result
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_00003, SR_SM_01002, SR_SM_01003, SR_SM_10001
    /// @trace_id_ad=AD_SM_08140
    /// @trace_id_dd=DD_SM_08500
    /// @needwork = ad
    /// @endcode
    void ChangeFGState(core::String const &fgFQN,
                       core::String const &fgState,
                       FGStateChangeSource const source,
                       core::Promise< core::String > &&promise) noexcept;

    /// @brief Execute state machine request transition
    /// @param smFQN State machine name
    /// @param transitionRequest Request sequence number, corresponding to the first column of the state transition table
    /// @param promise Used to asynchronously set the execution result
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_09001
    /// @trace_id_ad=AD_SM_00295
    /// @trace_id_dd=DD_SM_00311
    /// @needwork = ad
    /// @endcode
    void RequestState(core::String const &smFQN,
                      TransitionRequestType const transitionRequest,
                      core::Promise< void > &&promise) noexcept;

    /// @brief Execute error recovery
    /// @param executionErrorEvent Execution error event
    /// @param promise Used to asynchronously set the execution result
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_06004, SR_SM_08001, SR_SM_09002
    /// @trace_id_ad=AD_SM_00296
    /// @trace_id_dd=DD_SM_00312
    /// @needwork = ad
    /// @endcode
    void ErrorRecovery(exec::ExecutionErrorEvent const &executionErrorEvent, core::Promise< void > &&promise) noexcept;

    /// @brief Execute UCM reset machine request
    /// @param promise Used to asynchronously set the execution result
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_04005
    /// @trace_id_ad=AD_SM_00297
    /// @trace_id_dd=DD_SM_00313
    /// @needwork = ad
    /// @endcode
    void ResetMachine(core::Promise< void > &&promise, FGStateChangeSource source) noexcept;
#ifdef ARA_WITH_UCM
    /// @brief Execute UCM start update request
    /// @param promise Used to asynchronously set the execution result
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_04001
    /// @trace_id_ad=AD_SM_00298
    /// @trace_id_dd=DD_SM_00314
    /// @needwork = ad
    /// @endcode
    void StartUpdateSession(core::Promise< void > &&promise) noexcept;

    /// @brief Execute UCM start update request
    /// @param fgList List of function groups to be updated
    /// @param promise Used to asynchronously set the execution result
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_04004, SR_SM_04007
    /// @trace_id_ad=AD_SM_00299
    /// @trace_id_dd=DD_SM_00315
    /// @needwork = ad
    /// @endcode
    void PrepareUpdate(common::UcmFunctionGroupListInternal const &fgList, core::Promise< void > &&promise) noexcept;

    /// @brief Execute UCM start verification request
    /// @param fgList List of function groups to start verification
    /// @param promise Used to asynchronously set the execution result
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_04008
    /// @trace_id_ad=AD_SM_00300
    /// @trace_id_dd=DD_SM_00316
    /// @needwork = ad
    /// @endcode
    void VerifyUpdate(common::UcmFunctionGroupListInternal const &fgList, core::Promise< void > &&promise) noexcept;

    /// @brief Execute UCM prepare rollback request
    /// @param fgList List of function groups to prepare rollback
    /// @param promise Used to asynchronously set the execution result
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_04009
    /// @trace_id_ad=AD_SM_00301
    /// @trace_id_dd=DD_SM_00317
    /// @needwork = ad
    /// @endcode
    void PrepareRollback(common::UcmFunctionGroupListInternal const &fgList, core::Promise< void > &&promise) noexcept;

    /// @brief Execute UCM end update request
    /// @param promise Used to asynchronously set the execution result
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_04004, SR_SM_04006
    /// @trace_id_ad=AD_SM_00302
    /// @trace_id_dd=DD_SM_00318
    /// @needwork = ad
    /// @endcode
    void StopUpdateSession(core::Promise< void > &&promise) noexcept;
#endif
    /// @brief Get the current states of all function groups
    /// @param promise Used to asynchronously set the execution result, the String format is: FG1.state;FG2.state
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_02001
    /// @trace_id_ad=AD_SM_00303
    /// @trace_id_dd=DD_SM_00319
    /// @needwork = ad
    /// @endcode
    void GetAllFGStates(core::Promise< core::String > &&promise) noexcept;

    /// @brief Get the current state of the specified function group
    /// @param fgFQN Function group name
    /// @param promise Used to asynchronously set the execution result
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_01001, SR_SM_01003, SR_SM_10001
    /// @trace_id_ad=AD_SM_00304
    /// @trace_id_dd=DD_SM_00320
    /// @needwork = ad
    /// @endcode
    void GetFGState(core::String const &fgFQN, core::Promise< core::String > &&promise) noexcept;

    /// @brief Get the current state of the specified state machine
    /// @param smFQN State machine full name
    /// @param promise Used to asynchronously set the execution result
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_10001
    /// @trace_id_ad=AD_SM_00305
    /// @trace_id_dd=DD_SM_00321
    /// @needwork = ad
    /// @endcode
    void GetSMState(core::String const &smFQN, core::Promise< core::String > &&promise) noexcept;

    /// @brief Check whether shutdown is allowed
    /// @param nextState The state machine state to transition to
    /// @param source Source of the transition request
    /// @return Whether shutdown is allowed
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_04003
    /// @trace_id_ad=AD_SM_00306
    /// @trace_id_dd=DD_SM_00322
    /// @needwork = ad
    /// @endcode
    core::Result< void > CheckCanShutdown(core::StringView const nextState,
                                          FGStateChangeSource const source) const noexcept;

    /// @brief Set the global configuration instance
    /// @param globalConfigInstance Global configuration instance
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_00003, SR_SM_01001, SR_SM_01002, SR_SM_01003, SR_SM_02001, SR_SM_02002, SR_SM_03002, SR_SM_04001, SR_SM_04002, SR_SM_04003,
    /// SR_SM_04004, SR_SM_04005, SR_SM_04006, SR_SM_04007, SR_SM_04008, SR_SM_04009, SR_SM_05003, SR_SM_05004, SR_SM_05005, SR_SM_06001, SR_SM_06002,
    /// SR_SM_06003, SR_SM_06004, SR_SM_08001, SR_SM_09001, SR_SM_09002, SR_SM_09003, SR_SM_09004, SR_SM_09005, SR_SM_09006, SR_SM_09007, SR_SM_09008,
    /// SR_SM_10001
    /// @trace_id_ad=AD_SM_00308
    /// @trace_id_dd=DD_SM_00324
    /// @needwork = ad
    /// @endcode
    void SetGlobalConfigInstance(std::shared_ptr< config::GlobalConfig > const &globalConfigInstance) noexcept;

    /// @brief Start the state machine
    /// @param smFQN State machine full name
    /// @param smState State machine state
    /// @return Future object
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_SR_SM_01002, SR_SM_03002, SR_SM_04007, SR_SM_04008, SR_SM_04009, SR_SM_05003, SR_SM_05004, SR_SM_10001
    /// @trace_id_ad=AD_SM_00309
    /// @trace_id_dd=DD_SM_00325
    /// @needwork = no
    /// @endcode
    core::Future< void > StartStateMachine(core::String const &smFQN, core::String const &smState) noexcept;

    /// @brief Stop the state machine
    /// @param smFQN State machine full name
    /// @return Future object
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_SR_SM_01002, SR_SM_03002, SR_SM_04007, SR_SM_04008, SR_SM_04009, SR_SM_05003, SR_SM_05004, SR_SM_10001
    /// @trace_id_ad=AD_SM_00310
    /// @trace_id_dd=DD_SM_00326
    /// @needwork = no
    /// @endcode
    core::Future< void > StopStateMachine(core::String const &smFQN) noexcept;

#ifdef ARA_WITH_DIAG
    /// @brief Handle diagnostic request reset
    /// @param resetType Reset type
    /// @param targetAddressTypeStr Target address type
    /// @param targetAddress Target address
    /// @param promise Used to asynchronously set the execution result
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_03002
    /// @trace_id_ad=AD_SM_00311
    /// @trace_id_dd=DD_SM_00327
    /// @needwork = ad
    /// @endcode
    void HandleDiagRequestReset(common::DiagResetRequestInternalType const resetType,
                                core::String const &targetAddressTypeStr,
                                int32_t const targetAddress,
                                core::Promise< void > &&promise) noexcept;
#endif
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    void GetAllFunctionGroupsInfo(core::Promise< core::String > &&promise);

private:
    /// @brief Check whether a function group is in a deactivated state
    /// @param fgFQN Function group full name
    /// @return true - in a deactivated state
    /// @return false - in an activated state
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_00003
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_00323
    /// @needwork = dda
    /// @endcode
    bool _checkFGIsDeactivated(core::String const &fgFQN) const noexcept;

    /// @brief Asynchronously switch a group of function groups to the same state
    /// @param fgList List of function groups
    /// @param fgState Function group state
    /// @param source Source of the transition request
    /// @param err Error code
    /// @param promise Carries the switching result
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_04007, SR_SM_04008, SR_SM_04009
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08289
    /// @needwork = dda
    /// @endcode
    void _asyncChangeFunctionGroups(common::UcmFunctionGroupListInternal const &fgList,
                                    core::String const &fgState,
                                    FGStateChangeSource const source,
                                    SMErrc const err,
                                    core::Promise< void > &&promise) noexcept;

    /// @brief Save the update session state
    /// @param updateSessionActive Whether the update session is active
    /// @param updateSessionStartTime Update session start time
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_04004
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08290
    /// @needwork = dda
    /// @endcode
    void _saveUpdateSessionStatus(bool const updateSessionActive, std::int64_t const updateSessionStartTime) noexcept;

    /// @brief Save the list of function groups to be updated
    /// @param updatingFGSet Information of function groups to be updated
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_04004
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08291
    /// @needwork = dda
    /// @endcode
    void _saveUpdatingFGSet(std::set< core::String > const &updatingFGSet) noexcept;

    /// @brief Check whether the involved function group or state machine is in an update state
    /// @param requestFQN Full name of the function group or state machine
    /// @return true - updating
    /// @return false - not updating
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_04002, SR_SM_04003
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08292
    /// @needwork = dda
    /// @endcode
    bool _checkIsUpdating(core::String const &requestFQN) noexcept;

    /// @brief Get the function group FQN and state machine FQN based on the function group name
    /// @param fgName Function group name, it may be the function group FQN itself
    /// @param fgFQN Function group FQN
    /// @param smFQN State machine FQN
    /// @return true - retrieval successful; false - retrieval failed
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_00003, SR_SM_01002, SR_SM_01003, SR_SM_10001
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08293
    /// @needwork = dda
    /// @endcode
    bool _getFgAndSmFQN(core::String const &fgName, core::String &fgFQN, core::String &smFQN) noexcept;

    /// @brief Get the state machine FQN based on the state machine name
    /// @param smName State machine name, it may be the state machine FQN itself
    /// @return State machine FQN
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_09001, SR_SM_09008, SR_SM_10001
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08294
    /// @needwork = dda
    /// @endcode
    core::String _getSmFQN(core::String const &smName) noexcept;

    /// @brief Convert a timestamp in seconds to a string in the format YYYY-MM-DD HH:MM:SS
    /// @param timeStamp Function group name
    /// @return String in the format YYYY-MM-DD HH:MM:SS
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_04004
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08295
    /// @needwork = dda
    /// @endcode
    core::String _timeStampSecondToStr(std::int64_t const &timeStamp) const noexcept;

    /// @brief Get the current state of the specified function group
    /// @param fgFQN Function group name
    /// @return Current state of the specified function group
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_01001, SR_SM_01003, SR_SM_10001
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08296
    /// @needwork = dda
    /// @endcode
    core::String _getFGState(core::String const &fgFQN) noexcept;

    core::String getChangedNmHandle_(uint64_t const &nmHandleId) noexcept;

    /// @brief Function handle for publishing events to EventManager
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_01001, SR_SM_01003, SR_SM_02002, SR_SM_05004, SR_SM_06001, SR_SM_09008
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08297
    /// @needwork = dda
    /// @endcode
    std::function< void(common::Event &&) > appendEventHandler_{nullptr};

    /// @brief Function handle for obtaining execution errors
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_06004
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08298
    /// @needwork = dda
    /// @endcode
    std::function< core::Result< exec::ExecutionErrorEvent >(core::String const &) > getExecutionErrorHandler_{nullptr};

    /// @brief Global configuration instance, used to obtain function group information
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_00003, SR_SM_01001, SR_SM_01002, SR_SM_01003, SR_SM_02001, SR_SM_02002, SR_SM_03002, SR_SM_04001, SR_SM_04002, SR_SM_04003,
    /// SR_SM_04004, SR_SM_04005, SR_SM_04006, SR_SM_04007, SR_SM_04008, SR_SM_04009, SR_SM_05003, SR_SM_05004, SR_SM_05005, SR_SM_06001, SR_SM_06002,
    /// SR_SM_06003, SR_SM_06004, SR_SM_08001, SR_SM_09001, SR_SM_09002, SR_SM_09003, SR_SM_09004, SR_SM_09005, SR_SM_09006, SR_SM_09007, SR_SM_09008,
    /// SR_SM_10001
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08299
    /// @needwork = dda
    /// @endcode
    std::shared_ptr< config::GlobalConfig > globalConfigInstance_{nullptr};

    /// @brief Mapping of state machine FQN to state machine
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_00003, SR_SM_01001, SR_SM_01002, SR_SM_01003, SR_SM_02001, SR_SM_02002, SR_SM_03002, SR_SM_04001, SR_SM_04002, SR_SM_04003,
    /// SR_SM_04004, SR_SM_04005, SR_SM_04006, SR_SM_04007, SR_SM_04008, SR_SM_04009, SR_SM_05003, SR_SM_05004, SR_SM_05005, SR_SM_06001, SR_SM_06002,
    /// SR_SM_06003, SR_SM_06004, SR_SM_08001, SR_SM_09001, SR_SM_09002, SR_SM_09003, SR_SM_09004, SR_SM_09005, SR_SM_09006, SR_SM_09007, SR_SM_09008,
    /// SR_SM_10001
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08300
    /// @needwork = dda
    /// @endcode
    core::Map< core::String, std::shared_ptr< StateMachine > > sms_;

    core::Map< uint64_t, core::String > nmId2nmHandle_;

    std::mutex nmMutex_;

    /// @brief Master state machine
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_00003, SR_SM_01001, SR_SM_01002, SR_SM_01003, SR_SM_02001, SR_SM_02002, SR_SM_03002, SR_SM_04001, SR_SM_04002, SR_SM_04003,
    /// SR_SM_04004, SR_SM_04005, SR_SM_04006, SR_SM_04007, SR_SM_04008, SR_SM_04009, SR_SM_05003, SR_SM_05004, SR_SM_05005, SR_SM_06001, SR_SM_06002,
    /// SR_SM_06003, SR_SM_06004, SR_SM_08001, SR_SM_09001, SR_SM_09002, SR_SM_09003, SR_SM_09004, SR_SM_09005, SR_SM_09006, SR_SM_09007, SR_SM_09008,
    /// SR_SM_10001
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08301
    /// @needwork = dda
    /// @endcode
    std::shared_ptr< StateMachine > controllerSm_;

    /// @brief Mapping of function group FQN to state machine FQN, used to determine which state machine manages this function group
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_00003, SR_SM_01001, SR_SM_01002, SR_SM_01003, SR_SM_02001, SR_SM_02002, SR_SM_03002, SR_SM_04001, SR_SM_04002, SR_SM_04003,
    /// SR_SM_04004, SR_SM_04005, SR_SM_04006, SR_SM_04007, SR_SM_04008, SR_SM_04009, SR_SM_05003, SR_SM_05004, SR_SM_05005, SR_SM_06001, SR_SM_06002,
    /// SR_SM_06003, SR_SM_06004, SR_SM_08001, SR_SM_09001, SR_SM_09002, SR_SM_09003, SR_SM_09004, SR_SM_09005, SR_SM_09006, SR_SM_09007, SR_SM_09008,
    /// SR_SM_10001
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08302
    /// @needwork = dda
    /// @endcode
    core::Map< core::String, core::String > fg2sm_;

    /// @brief Whether it has started
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_00003, SR_SM_01001, SR_SM_01002, SR_SM_01003, SR_SM_02001, SR_SM_02002, SR_SM_03002, SR_SM_04001, SR_SM_04002, SR_SM_04003,
    /// SR_SM_04004, SR_SM_04005, SR_SM_04006, SR_SM_04007, SR_SM_04008, SR_SM_04009, SR_SM_05003, SR_SM_05004, SR_SM_05005, SR_SM_06001, SR_SM_06002,
    /// SR_SM_06003, SR_SM_06004, SR_SM_08001, SR_SM_09001, SR_SM_09002, SR_SM_09003, SR_SM_09004, SR_SM_09005, SR_SM_09006, SR_SM_09007, SR_SM_09008,
    /// SR_SM_10001
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08303
    /// @needwork = dda
    /// @endcode
    bool started_{false};

    /// @brief Mapping of FullCom network to function group state list, core::Map<Network, core::Map<Function group FQN, core::Vector<Function group state>>>
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_05003
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08304
    /// @needwork = dda
    /// @endcode
    core::Map< core::String, core::Map< core::String, core::String > > fullComNmHandle2FGStatesMap_;

    /// @brief Mapping of NoCom network to function group state list, core::Map<Network, core::Map<Function group FQN, core::Vector<Function group state>>>
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_05003
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08305
    /// @needwork = dda
    /// @endcode
    core::Map< core::String, core::Map< core::String, core::String > > noComNmHandle2FGStatesMap_;

    /// @brief Whether there is an active update currently
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_04002, SR_SM_04003, SR_SM_04007, SR_SM_04008, SR_SM_04009
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08306
    /// @needwork = dda
    /// @endcode
    bool updateSessionActive_{false};

    /// @brief Whether it is in the prepare update process
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_00003, SR_SM_01001, SR_SM_01002, SR_SM_01003, SR_SM_02001, SR_SM_02002, SR_SM_03002, SR_SM_04001, SR_SM_04002, SR_SM_04003,
    /// SR_SM_04004, SR_SM_04005, SR_SM_04006, SR_SM_04007, SR_SM_04008, SR_SM_04009, SR_SM_05003, SR_SM_05004, SR_SM_05005, SR_SM_06001, SR_SM_06002,
    /// SR_SM_06003, SR_SM_06004, SR_SM_08001, SR_SM_09001, SR_SM_09002, SR_SM_09003, SR_SM_09004, SR_SM_09005, SR_SM_09006, SR_SM_09007, SR_SM_09008,
    /// SR_SM_10001
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08307
    /// @needwork = no
    /// @endcode
    bool isInPrepareUpdate_{false};

    /// @brief Whether it is in the prepare rollback process
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_00003, SR_SM_01001, SR_SM_01002, SR_SM_01003, SR_SM_02001, SR_SM_02002, SR_SM_03002, SR_SM_04001, SR_SM_04002, SR_SM_04003,
    /// SR_SM_04004, SR_SM_04005, SR_SM_04006, SR_SM_04007, SR_SM_04008, SR_SM_04009, SR_SM_05003, SR_SM_05004, SR_SM_05005, SR_SM_06001, SR_SM_06002,
    /// SR_SM_06003, SR_SM_06004, SR_SM_08001, SR_SM_09001, SR_SM_09002, SR_SM_09003, SR_SM_09004, SR_SM_09005, SR_SM_09006, SR_SM_09007, SR_SM_09008,
    /// SR_SM_10001
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08308
    /// @needwork = no
    /// @endcode
    bool isInPrepareRollback_{false};

    /// @brief Whether it is in the verify update process
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_00003, SR_SM_01001, SR_SM_01002, SR_SM_01003, SR_SM_02001, SR_SM_02002, SR_SM_03002, SR_SM_04001, SR_SM_04002, SR_SM_04003,
    /// SR_SM_04004, SR_SM_04005, SR_SM_04006, SR_SM_04007, SR_SM_04008, SR_SM_04009, SR_SM_05003, SR_SM_05004, SR_SM_05005, SR_SM_06001, SR_SM_06002,
    /// SR_SM_06003, SR_SM_06004, SR_SM_08001, SR_SM_09001, SR_SM_09002, SR_SM_09003, SR_SM_09004, SR_SM_09005, SR_SM_09006, SR_SM_09007, SR_SM_09008,
    /// SR_SM_10001
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08309
    /// @needwork = no
    /// @endcode
    bool isInVerifyUpdate_{false};

    /// @brief Update start time (in seconds)
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_04002, SR_SM_04003, SR_SM_04004
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08310
    /// @needwork = dda
    /// @endcode
    std::int64_t updateSessionStartTime_{0};

    /// @brief Timeout for the update session to prevent shutdown (in seconds), default is 10 minutes
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_04003
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08311
    /// @needwork = dda
    /// @endcode
    std::int64_t shutdownPreventionTimeout_{SHUTDOWN_PREVENTION_TIMEOUT_MS};

    /// @brief Set of function groups being updated
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_04002, SR_SM_04003, SR_SM_04004
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08312
    /// @needwork = dda
    /// @endcode
    std::set< core::String > updatingFGSet_;

#ifdef ARA_WITH_PER
    /// @brief Update session state storage
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_04004
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08313
    /// @needwork = dda
    /// @endcode
    storage::Storage updateSessionStatusStorage_;
#endif

    /// @brief Information of all function groups
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_02001
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08314
    /// @needwork = dda
    /// @endcode
    core::Vector< config::FGInfo > fgInfos_;

    /// @brief Logger instance
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_00003, SR_SM_01001, SR_SM_01002, SR_SM_01003, SR_SM_02001, SR_SM_02002, SR_SM_03002, SR_SM_04001, SR_SM_04002, SR_SM_04003,
    /// SR_SM_04004, SR_SM_04005, SR_SM_04006, SR_SM_04007, SR_SM_04008, SR_SM_04009, SR_SM_05003, SR_SM_05004, SR_SM_05005, SR_SM_06001, SR_SM_06002,
    /// SR_SM_06003, SR_SM_06004, SR_SM_08001, SR_SM_09001, SR_SM_09002, SR_SM_09003, SR_SM_09004, SR_SM_09005, SR_SM_09006, SR_SM_09007, SR_SM_09008,
    /// SR_SM_10001
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08315
    /// @needwork = dda
    /// @endcode
    log::Logger &log_{log::CreateLogger((core::StringView{"#SMM"}), (core::StringView{"State Machine Management"}))};
};

}  // namespace state_machine_management
}  // namespace sm
}  // namespace ara

#endif