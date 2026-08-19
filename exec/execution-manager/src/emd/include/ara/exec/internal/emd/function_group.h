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
/// @file       function_group.h
/// @brief      Function group class definition
/// @details
/// @date       2023-04-01
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/ExecutionManagement/Emd
/// @unit_name=FunctionGroup
/// @unit_description=Used to manage a group of processes.
/// @interface_level=unit
/// @endcode
///
/// ================================================================

#ifndef _ARA_EXEC_INTERAL_EMD_FUNCTION_GROUP_H_
#define _ARA_EXEC_INTERAL_EMD_FUNCTION_GROUP_H_

#include <ara/core/map.h>
#include <ara/core/string.h>
#include <ara/core/string_view.h>
#include <ara/core/vector.h>
#include <isoft/core/set.h>
#include <isoft/naicpp/evloop.h>

#include <cstdint>
#include <functional>
#include <memory>

#include "ara/exec/exec_error_domain.h"
#include "ara/exec/internal/config/sysconfig.h"
#include "ara/exec/internal/emd/finite_state_machine.h"
#include "ara/exec/internal/emd/process.h"

namespace ara {
namespace exec {
namespace internal {
namespace emd {

/// @brief Function group class definition
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_sr=SR_EM_00037~SR_EM_00058
/// @trace_id_ad=AD_EM_00123
/// @trace_id_dd=DD_EM_00433
/// @needwork = ad
/// @endcode
class FunctionGroup
{
public:
    /// @brief Function group state change callback function
    /// @param fg Pointer to the function group whose state has changed
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    using StateChangedAction = std::function< void(FunctionGroup const *fg) >;

    /// @brief Default constructor
    /// @param name Function group name
    /// @param states Function group state list
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_EM_00123
    /// @trace_id_dd=DD_EM_00434
    /// @needwork = dda
    /// @endcode
    FunctionGroup(ara::core::StringView const &name, ara::core::Vector< ara::core::String > const &states) noexcept;

    /// @brief Disable default constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_EM_00123
    /// @trace_id_dd=DD_EM_00435
    /// @needwork = dda
    /// @endcode
    FunctionGroup() = delete;

    /// @brief Clear the function group, restore to the initial state
    /// @return 0 success; <0 failure
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_EM_00123
    /// @trace_id_dd=DD_EM_00436
    /// @needwork = dda
    /// @endcode
    int32_t Clear() noexcept;

    /// @brief Build a dependency tree for processes within the function group
    /// @return 0 success; <0 failure
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_EM_00123
    /// @trace_id_dd=DD_EM_00437
    /// @needwork = dda
    /// @endcode
    int32_t GenDependTree() noexcept;

    /// @brief Add a process to the function group
    /// @param proc Process handle
    /// @return 0 success; <0 failure
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_EM_00123
    /// @trace_id_dd=DD_EM_00438
    /// @needwork = dda
    /// @endcode
    int32_t AddProcess(std::shared_ptr< Process > &proc) noexcept;

    /// @brief Traverse processes
    /// @param cb Callback function, called once for each process found
    /// @exception same as std::function
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_EM_00123
    /// @trace_id_dd=DD_EM_00439
    /// @needwork = dda
    /// @endcode
    void TraversalProcess(std::function< void(std::shared_ptr< Process >) > const &cb) const
    {
        if (cb == nullptr) {
            return;
        }
        for (auto const &it : processNameMap_) {
            cb(it.second);
        }
    }

    /// @brief Find a process pointer by process name
    /// @param name Process name
    /// @return nullptr failure; !nullptr found process
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_EM_00123
    /// @trace_id_dd=DD_EM_00440
    /// @needwork = dda
    /// @endcode
    std::shared_ptr< Process > FindProcessByName(ara::core::String const &name) noexcept
    {
        ara::core::Map< ara::core::String const, std::shared_ptr< Process > >::iterator const it{
            processNameMap_.find(name)};
        if (it != processNameMap_.end()) {
            return it->second;
        }

        return nullptr;
    }

    /// @brief Start state transition
    /// @param stateName Target state
    /// @return 0 success; <0 failure
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_EM_00123
    /// @trace_id_dd=DD_EM_00441
    /// @needwork = dda
    /// @endcode
    int32_t StartTransformState(ara::core::StringView const &stateName) noexcept;

    /// @brief Stop the ongoing function group state transition
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_EM_00123
    /// @trace_id_dd=DD_EM_00442
    /// @needwork = dda
    /// @endcode
    void StopTransformState() noexcept;

    /// @brief Determine whether it contains a certain function group state
    /// @param stateName Function group state name
    /// @return Yes/No
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_EM_00123
    /// @trace_id_dd=DD_EM_00443
    /// @needwork = dda
    /// @endcode
    bool HasState(ara::core::String const &stateName) const noexcept
    {
        ara::core::Map< ara::core::String const, ProcessList >::const_iterator const it{
            stateProcessMap_.find(stateName)};
        if (it != stateProcessMap_.end()) {
            return true;
        }
        return false;
    }

    /// @brief Determine whether the current function group is active
    /// @return Yes/No
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_EM_00123
    /// @trace_id_dd=DD_EM_00444
    /// @needwork = dda
    /// @endcode
    bool IsAlive() const noexcept { return currentState_.compare(config::GetFunctionGroupStateOff()) != 0; }

    /// @brief Get the function group state transition failure error code
    /// @return Execution error code
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_EM_00123
    /// @trace_id_dd=DD_EM_00445
    /// @needwork = dda
    /// @endcode
    ara::exec::ExecErrc GetTransitionError() const noexcept { return transErrorCode_; }

    /// @brief Get the execution error code of the function group state transition failure
    /// @return Execution error code
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_EM_00123
    /// @trace_id_dd=DD_EM_00446
    /// @needwork = dda
    /// @endcode
    uint32_t GetExecutionError() const noexcept { return executionErrorCode_; }

    /// @brief Get the function group name
    /// @return Function group name
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_EM_00123
    /// @trace_id_dd=DD_EM_00447
    /// @needwork = dda
    /// @endcode
    ara::core::StringView const GetName() const noexcept { return name_; }

    /// @brief Get the current function group state
    /// @return Current function group state
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_EM_00123
    /// @trace_id_dd=DD_EM_00448
    /// @needwork = dda
    /// @endcode
    ara::core::StringView const GetCurrentState() const noexcept { return currentState_; }

    /// @brief Get the state being transitioned
    /// @return empty, transition has ended, no state is being transitioned
    /// @return !empty, name of the state being transitioned
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_EM_00123
    /// @trace_id_dd=DD_EM_00449
    /// @needwork = dda
    /// @endcode
    ara::core::StringView const GetTransingState() const noexcept { return nextState_; }

    /// @brief Register a state change callback function, called when the function group state changes
    /// @param action Callback function
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_EM_00123
    /// @trace_id_dd=DD_EM_00450
    /// @needwork = dda
    /// @endcode
    void OnStateChanged(StateChangedAction const &action) noexcept { stateChangedAction_ = action; };

    /// @brief Print debug information
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    void DebugFG() const noexcept;

private:
    /// @brief Set the function group state transition error code
    /// @param ec Error code
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00123
    /// @trace_id_dd=DD_EM_00451
    /// @needwork = dda
    /// @endcode
    void _SetTransitionError(ara::exec::ExecErrc const &ec) noexcept { transErrorCode_ = ec; }

    /// @brief Set the execution error code for function group state transition failure, which comes from the configuration in the process execution manifest
    /// @param ec Error code
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00123
    /// @trace_id_dd=DD_EM_00452
    /// @needwork = dda
    /// @endcode
    void _SetExecutionError(uint32_t const ec) noexcept { executionErrorCode_ = ec; }

    /// @brief Set the current function group state
    /// @param state Function group state name
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00123
    /// @trace_id_dd=DD_EM_00453
    /// @needwork = dda
    /// @endcode
    void _SetCurrentState(ara::core::StringView const &state) noexcept;

    /// @brief Process termination event handler function, called when a process termination event is received
    /// @param proc Pointer to the terminated process
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00123
    /// @trace_id_dd=DD_EM_00454
    /// @needwork = dda
    /// @endcode
    void _ProcessTerminatedHandler(Process *const proc) noexcept;

    /// @brief Process start event handler function, called when a process start event is received
    /// @param proc Pointer to the terminated process
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00123
    /// @trace_id_dd=DD_EM_00455
    /// @needwork = dda
    /// @endcode
    void _ProcessRunningHandler(Process *const proc) const noexcept;

private:
    /// @brief Function group name
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00123
    /// @trace_id_dd=DD_EM_00456
    /// @needwork = dda
    /// @endcode
    ara::core::String name_;

    /// @brief The set of Process Pointer
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    using ProcessList = isoft::core::Set< std::shared_ptr< Process > >;

    /// @brief Mapping of function group states and processes (string corresponds to function group state name, ProcessList corresponds to list of Process pointers that depend on that state)
    /// @note Stable, once saved, will not change during runtime
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00123
    /// @trace_id_dd=DD_EM_00457
    /// @needwork = dda
    /// @endcode
    ara::core::Map< ara::core::String const, ProcessList > stateProcessMap_;

    /// @brief Process name mapping, convenient for finding processes by name (string corresponds to process name, Process corresponds to process pointer)
    /// @note  Stable, once saved, will not change during runtime
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00123
    /// @trace_id_dd=DD_EM_00458
    /// @needwork = dda
    /// @endcode
    ara::core::Map< ara::core::String const, std::shared_ptr< Process > > processNameMap_;

    /// @brief List of active processes, recording the currently running processes
    /// Variable, processes change during runtime
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00123
    /// @trace_id_dd=DD_EM_00459
    /// @needwork = dda
    /// @endcode
    isoft::core::Set< Process * > activedProcessList_;

    /// @brief Save the current function group state
    /// @note Each function group (including the function group MachineFG) has an Off state. If no other state is requested, execution management should use it as the default function group state
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sws=SWS_EM_01110
    /// @trace_id_ad=AD_EM_00123
    /// @trace_id_dd=DD_EM_00460
    /// @needwork = dda
    /// @endcode
    ara::core::String currentState_;

    /// @brief Next state, used to mark the state to be transitioned during the state transition process
    /// @note Variable, changes frequently during runtime. Also used as a transition flag; if the transition has ended, this string is cleared
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00123
    /// @trace_id_dd=DD_EM_00461
    /// @needwork = dda
    /// @endcode
    ara::core::String nextState_;

    /// @brief State transition state machine
    class StateTransferFSM;

    /// @brief State transition process finite state machine
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00123
    /// @trace_id_dd=DD_EM_00462
    /// @needwork = dda
    /// @endcode
    std::unique_ptr< StateTransferFSM > fsm_;  // PRQA S 2026 # Suppress incorrect comment format

    /// @brief State transition completion callback function
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00123
    /// @trace_id_dd=DD_EM_00463
    /// @needwork = dda
    /// @endcode
    StateChangedAction stateChangedAction_;

    /// @brief Save the function group state transition error code
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00123
    /// @trace_id_dd=DD_EM_00464
    /// @needwork = dda
    /// @endcode
    ara::exec::ExecErrc transErrorCode_;

    /// @brief Save the function group state transition error execution error code
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00123
    /// @trace_id_dd=DD_EM_00465
    /// @needwork = dda
    /// @endcode
    uint32_t executionErrorCode_;

};  ///< class FunctionGroup

/// @brief Function group state switching process finite state machine
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_sr=SR_EM_00042
/// @trace_id_ad=AD_EM_00123
/// @trace_id_dd=DD_EM_00466
/// @needwork = dd
/// @endcode
class FunctionGroup::StateTransferFSM final : public FiniteStateMachine
{
public:
    /// @brief Constructor
    /// @param name State machine name
    /// @param fg Function group handle
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_EM_00123
    /// @trace_id_dd=DD_EM_00469
    /// @needwork = dda
    /// @endcode
    StateTransferFSM(ara::core::String const &name, FunctionGroup *const fg) noexcept;

    /// @brief Disable default constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_EM_00123
    /// @trace_id_dd=DD_EM_00470
    /// @needwork = dda
    /// @endcode
    StateTransferFSM() = delete;

    /// @brief Default destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_EM_00123
    /// @trace_id_dd=DD_EM_00471
    /// @needwork = dda
    /// @endcode
    ~StateTransferFSM() final = default;

    /// @brief Disable move constructor
    /// @param other the other StateTransferFSM
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    StateTransferFSM(StateTransferFSM &&other) noexcept = delete;

    /// @brief Disable copy constructor
    /// @param other the other StateTransferFSM
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    StateTransferFSM(StateTransferFSM const &other) noexcept = delete;

    /// @brief Disable move assignment
    /// @param other the other StateTransferFSM
    /// @return new StateTransferFSM
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    StateTransferFSM &operator=(StateTransferFSM &&other) noexcept = delete;

    /// @brief Disable copy assignment
    /// @param other the other StateTransferFSM
    /// @return new StateTransferFSM
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    StateTransferFSM &operator=(StateTransferFSM const &other) noexcept = delete;

    /// @brief Initialization function, establishes process start/termination lists, etc.
    /// @return 0 success; <0 failure
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_EM_00123
    /// @trace_id_dd=DD_EM_00472
    /// @needwork = dda
    /// @endcode
    int32_t Init() noexcept;

    /// @brief Destroy function, clears and restores internal state, objects, etc.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_EM_00123
    /// @trace_id_dd=DD_EM_00473
    /// @needwork = dda
    /// @endcode
    void Destroy() noexcept;

    /// @brief Print debug information
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    void Debug() const noexcept;

    /// @brief Trigger a process termination event
    /// @param proc Process handle
    /// @exception std::runtime_error
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_EM_00123
    /// @trace_id_dd=DD_EM_00474
    /// @needwork = dda
    /// @endcode
    void TriggerProcessTerminatedEvent(Process *const proc)
    {
        /// Delete proc from the dependent list of all processes
        for (auto &it : terminateProcessList_) {
            isoft::core::Set< Process * > &ps{it.second};
            std::ignore = ps.erase(proc);
        }
        /// Remove proc itself
        ara::core::Map< Process *, isoft::core::Set< Process * > >::iterator const it{terminateProcessList_.find(proc)};
        if (it != terminateProcessList_.end()) {
            std::ignore = terminateProcessList_.erase(it);
            TriggerEvent(Event::kProcessTerminated);
        } else {
            if (proc->IsSelfTerminate()) {
                /// Processes that depend on a self-terminating process will not cause changes in the startup list
                /// During Starting condition checking, it will cause the condition to be unsatisfied, thus
                /// the process will not be started, so skip the condition check
                _SkipStartingCondition();
                TriggerEvent(Event::kProcessTerminated);
            }
        }
    }

    /// @brief Trigger a process start event
    /// @param proc Process handle
    /// @exception std::runtime_error
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_EM_00123
    /// @trace_id_dd=DD_EM_00475
    /// @needwork = dda
    /// @endcode
    void TriggerProcessStartedEvent(Process *const proc)
    {
        std::ignore = startupProcessList_.erase(proc);
        TriggerEvent(Event::kProcessStarted);
    }

    /// @brief Determine whether the process is in the termination list
    /// @param proc Process handle
    /// @return true/false
    /// @exception std::bad_alloc
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_EM_00123
    /// @trace_id_dd=DD_EM_00476
    /// @needwork = dda
    /// @endcode
    bool IsInTerminatedProcessList(Process *const proc)
    {
        ara::core::Map< Process *, isoft::core::Set< Process * > >::iterator const itFind{
            terminateProcessList_.find(proc)};
        return (itFind != terminateProcessList_.end());
    }

private:
    /// @brief Asynchronously terminate processes, the function scans once each time it is called, if there are processes that meet the conditions, terminate them
    /// @note  This function needs to be called multiple times in an asynchronous handler until the list of processes to be terminated is empty
    /// @param s State
    /// @param e Event
    /// @return 0 success; <0 failure
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00123
    /// @trace_id_dd=DD_EM_00477
    /// @needwork = dda
    /// @endcode
    int32_t _TerminatingAction(FiniteStateMachine::State const &s, FiniteStateMachine::Event const &e) noexcept;

    /// @brief Asynchronously start processes, the function scans once each time it is called, if there are processes with satisfied dependencies, start them
    /// @note  This function needs to be called multiple times in an asynchronous handler until the list of processes to be started is empty
    /// @param s State
    /// @param e Event
    /// @return 0 success; <0 failure
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00123
    /// @trace_id_dd=DD_EM_00478
    /// @needwork = dda
    /// @endcode
    int32_t _StartingAction(FiniteStateMachine::State const &s, FiniteStateMachine::Event const &e) noexcept;

    /// @brief Condition judgment function for entering the Terminating state
    /// @param src Source state
    /// @param dest Target state
    /// @return Whether the condition is met
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00123
    /// @trace_id_dd=DD_EM_00479
    /// @needwork = dda
    /// @endcode
    bool _TerminatingCondition(FiniteStateMachine::State const &src, FiniteStateMachine::State const &dest) noexcept;

    /// @brief Condition judgment function for entering the Terminated state
    /// @param src Source state
    /// @param dest Target state
    /// @return Whether the condition is met
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00123
    /// @trace_id_dd=DD_EM_00480
    /// @needwork = dda
    /// @endcode
    bool _TerminatedCondition(FiniteStateMachine::State const &src,
                              FiniteStateMachine::State const &dest) const noexcept;

    /// @brief Condition judgment function for skipping the Starting state
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00123
    /// @trace_id_dd=DD_EM_00481
    /// @needwork = dda
    /// @endcode
    void _SkipStartingCondition() noexcept { skipStartingCondition_ = true; }

    /// @brief Condition judgment function for entering the Starting state
    /// @param src Source state
    /// @param dest Target state
    /// @return Whether the condition is met
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00123
    /// @trace_id_dd=DD_EM_00482
    /// @needwork = dda
    /// @endcode
    bool _StartingCondition(FiniteStateMachine::State const &src, FiniteStateMachine::State const &dest) noexcept;

    /// @brief Condition judgment function for entering the Started state
    /// @param src Source state
    /// @param dest Target state
    /// @return Whether the condition is met
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00123
    /// @trace_id_dd=DD_EM_00483
    /// @needwork = dda
    /// @endcode
    bool _StartedCondition(FiniteStateMachine::State const &src, FiniteStateMachine::State const &dest) const noexcept;

private:
    /// @brief Function group handle
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00123
    /// @trace_id_dd=DD_EM_00484
    /// @needwork = dda
    /// @endcode
    FunctionGroup *fg_;
    /// @brief Process startup list
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00123
    /// @trace_id_dd=DD_EM_00485
    /// @needwork = dda
    /// @endcode
    isoft::core::Set< Process * > startupProcessList_;
    /// @brief Skip the starting condition check to prevent processes that depend on a self-terminating process from being unable to start
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00123
    /// @trace_id_dd=DD_EM_00486
    /// @needwork = dda
    /// @endcode
    bool skipStartingCondition_;
    /// @brief Record the previous list length, used as an identifier
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00123
    /// @trace_id_dd=DD_EM_00487
    /// @needwork = dda
    /// @endcode
    int32_t startupProcessListPreSize_;

    /// @brief Process termination list (Process, process pointer to be terminated, Set<Process*>, list of processes that depend on itself)
    /// @note  If empty, it means no other processes depend on itself, can be terminated directly
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00123
    /// @trace_id_dd=DD_EM_00488
    /// @needwork = dda
    /// @endcode
    ara::core::Map< Process *, isoft::core::Set< Process * > > terminateProcessList_;
    /// @brief Record the previous termination list length, used as an identifier
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00123
    /// @trace_id_dd=DD_EM_00489
    /// @needwork = dda
    /// @endcode
    int32_t terminateProcessListPreSize_;
};  ///< class StateTransferFSM

}  // namespace emd
}  // namespace internal
}  // namespace exec
}  // namespace ara

#endif  ///< _ARA_EXEC_INTERAL_EMD_FUNCTION_GROUP_H_
