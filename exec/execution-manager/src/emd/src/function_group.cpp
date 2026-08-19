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
/// @file       function_group.cpp
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
/// @interface_level=module
/// @endcode
///
/// ================================================================

#include "ara/exec/internal/emd/function_group.h"

#include "ara/core/map.h"
#include "ara/exec/internal/emd/log.h"
#include "isoft/core/set.h"
#include "isoft/naicpp/evloop.h"

namespace ara {
namespace exec {
namespace internal {
namespace emd {

/// @brief Constructor
/// @param name State machine name
/// @param fg Function group handle
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_EM_00123
/// @trace_id_dd=DD_EM_00469
/// @needwork = dda
/// @endcode
FunctionGroup::StateTransferFSM::StateTransferFSM(ara::core::String const &name, FunctionGroup *const fg) noexcept
    : FiniteStateMachine{name}
    , fg_{fg}
    , startupProcessList_{}
    , skipStartingCondition_{false}
    , startupProcessListPreSize_{0}
    , terminateProcessList_{}
    , terminateProcessListPreSize_{0}
{
    /// Initialize the state machine
    /// Add events
    _AddEvent(Event::kProcessStarted, nullptr);
    _AddEvent(Event::kProcessTerminated, nullptr);

    /// Add states
    Action const terminatingAction{
        [this](FiniteStateMachine::State const &s, FiniteStateMachine::Event const &e) noexcept -> int32_t {
            return this->_TerminatingAction(s, e);
        }};
    Action const startingAction{
        [this](FiniteStateMachine::State const &s, FiniteStateMachine::Event const &e) noexcept -> int32_t {
            return this->_StartingAction(s, e);
        }};

    _AddState(State::kTerminating, terminatingAction, nullptr);
    _AddState(State::kTerminated, nullptr, nullptr);  ///< Abstract state, convenient for discussing the model, no practical significance
    _AddState(State::kStarting, startingAction, nullptr);
    _AddState(State::kStarted, nullptr, nullptr);  ///< Abstract state, representing final completion

    /// Add transition paths
    TransitionCondition const terminatingCondition{
        [this](FiniteStateMachine::State const &src, FiniteStateMachine::State const &dest) noexcept -> bool {
            return this->_TerminatingCondition(src, dest);
        }};
    TransitionCondition const terminatedCondition{
        [this](FiniteStateMachine::State const &src, FiniteStateMachine::State const &dest) noexcept -> bool {
            return this->_TerminatedCondition(src, dest);
        }};
    TransitionCondition const startingCondition{
        [this](FiniteStateMachine::State const &src, FiniteStateMachine::State const &dest) noexcept -> bool {
            return this->_StartingCondition(src, dest);
        }};
    TransitionCondition const startedCondition{
        [this](FiniteStateMachine::State const &src, FiniteStateMachine::State const &dest) noexcept -> bool {
            return this->_StartedCondition(src, dest);
        }};

    /// Need to enter the Terminating state multiple times, each time only terminating processes that can be terminated
    _AddTransitionRule(State::kTerminating, State::kTerminating, terminatingCondition);
    /// After all processes that need to be terminated are terminated, enter the Terminated state
    _AddTransitionRule(State::kTerminating, State::kTerminated, terminatedCondition);
    /// Abstract state, unconditionally enter Starting
    _AddTransitionRule(State::kTerminated, State::kStarting, nullptr);
    /// Need to enter the Starting state multiple times, each time only starting processes that can be started
    _AddTransitionRule(State::kStarting, State::kStarting, startingCondition);
    /// After all processes that need to be started are started, enter the Started state
    _AddTransitionRule(State::kStarting, State::kStarted, startedCondition);

    /// Register the state machine success callback function
    OnFinal([this](FiniteStateMachine::State const &, FiniteStateMachine::Event const &) noexcept -> int32_t {
        /// @code{.isoft}
        /// @trace_id_sws=SWS_EM_01067 After the function group transition is completed, the function group state should be set and notify SM
        /// @endcode
        fg_->_SetExecutionError(0U);
        fg_->_SetTransitionError(ara::exec::ExecErrc::kNoError);
        fg_->_SetCurrentState(fg_->nextState_);
        return 0;
    });

    /// Register the state machine failure callback function
    OnError([this](FiniteStateMachine::State const &, FiniteStateMachine::Event const &) -> int32_t {
        fg_->_SetTransitionError(ara::exec::ExecErrc::kFailed);
        fg_->_SetCurrentState(config::GetFunctionGroupStateUndefined());
        return 0;
    });
}

/// @brief Initialization function, establishes process start/termination lists, etc.
/// @return 0 success; <0 failure
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_EM_00123
/// @trace_id_dd=DD_EM_00472
/// @needwork = dda
/// @endcode
int32_t FunctionGroup::StateTransferFSM::Init() noexcept
{
    /// If another state is being switched, terminate the switch
    /// Mainly for EM to handle the SIGTERM signal. If MachineFG is switching to a state, terminate the switch,
    /// switch MachineFG to Off, otherwise there may be a non-empty startupProcessList_, causing Off state to still start new processes
    Destroy();

    /// Set the initial state
    SetInitialState(State::kTerminating);

    /// @code{.isoft}
    /// @trace_id_sws=SWS_EM_01033 When switching function groups, start processes according to configuration
    /// Establish the process termination list and startup list
    /// Principle: Check whether the StartupConfig of the process is consistent in the current function group state and the target function group state. If inconsistent, the process needs to be restarted (first terminate, then start)
    ///          If there is no StartupConfig under the standard function group state, then GetStartupConfig will return nullptr, and the process needs to be terminated
    /// @trace_id_sws=SWS_EM_01060 When switching function group states, inactive processes need to be terminated
    /// @trace_id_sws=SWS_EM_02251
    /// During state transitions, execution management should terminate all processes that reference the CurrentState in their execution manifest but reference a different StartupConfig for the requeststate, and whose process state is different from [Idle or Terminated]
    /// @endcode
    /// Check the active process list to find processes that need to be terminated
    for (auto const &proc : fg_->activedProcessList_) {
        config::StateDependentConfig const *const startupConfig0{proc->GetCurrentStartupConfig()};
        config::StateDependentConfig const *const startupConfig1{proc->GetStartupConfig(fg_->nextState_)};
        /// Check the process state, if it has already terminated, do not add it to the termination list
        if (!proc->IsActived()) {
            LOGD() << "Process {" << proc->GetName()
                   << "} is in InActived state, so we don't push it to TerminatedList.";
            continue;
        }
        /// If the two startup configurations are the same, it means the process does not need to be restarted, keep it unchanged
        if (startupConfig0 == startupConfig1) {
            LOGD() << "Process {" << proc->GetName() << "} has the same configuration for the state {"
                   << proc->GetCurrentFunctionGroupState() << "} and the state {" << fg_->nextState_ << "}.";
            continue;
        }
        /// Add processes that need to be restarted or terminated to the termination list
        std::ignore = terminateProcessList_.emplace(std::make_pair(proc, isoft::core::Set< Process * >{}));
    }
    /// Scan the termination list to generate a termination dependency list
    for (auto &it : terminateProcessList_) {
        Process *proc{it.first};
        if (nullptr == proc) {
            continue;
        }
        ara::core::Vector< Process * > runDepList;
        proc->GetRunningDependProcessList(ara::core::String(proc->GetCurrentFunctionGroupState()), runDepList);
        for (auto &p : runDepList) {  // PRQA S 2961
            ara::core::Map< Process *, isoft::core::Set< Process * > >::iterator const termProcIter{
                terminateProcessList_.find(p)};
            if (termProcIter == terminateProcessList_.end()) {
                LOGD() << "Process {" << proc->GetName() << "}'s dependent Process {" << p->GetName()
                       << "} is not in the terminate list.";
                continue;
            }
            isoft::core::Set< Process * > &ps{termProcIter->second};
            std::ignore = ps.emplace(proc);
        }
    }

    /// @code{.isoft}
    /// @trace_id_sws=SWS_EM_01000 The startup order of platform-level processes is determined by EM based on the configuration of the machine manifest and execution manifest
    /// @trace_id_sws=SWS_EM_01066 When switching function group states, all processes in the target state must be started
    /// @endcode
    for (auto const &proc : fg_->stateProcessMap_[fg_->nextState_]) {
        config::StateDependentConfig const *const startupConfig0{proc->GetCurrentStartupConfig()};
        config::StateDependentConfig const *const startupConfig1{proc->GetStartupConfig(fg_->nextState_)};
        /// FIXME: If a self-terminating process references two function group states with the same configuration, does it need to be started when switching function groups?
        /// If the two startup configurations are the same, it means the process does not need to be restarted, keep it unchanged
        if (startupConfig0 == startupConfig1) {
            continue;
        }

        /// If the process state is terminated, reset the process state to the initial Idle state to prevent processes that depend on the terminated state from being started directly
        if (Process::ProcessState::kTerminated == proc->GetProcessState()) {
            proc->SetProcessState(Process::ProcessState::kIdle);
        }

        /// Otherwise, the process needs to be restarted. Add it to the startup list
        if (startupConfig1 != nullptr) {
            std::ignore = startupProcessList_.emplace(proc.get());
        }
    }

    ara::core::String procListStr{"TerminateProcessList: "};
    for (auto &it : terminateProcessList_) {
        Process *const proc{it.first};
        if (nullptr == proc) {
            continue;
        }
        procListStr += ara::core::String{proc->GetName()} + " ";
    }
    LOGI() << procListStr;

    procListStr = "StartupProcessList: ";
    for (auto const &proc : startupProcessList_) {
        if (nullptr == proc) {
            continue;
        }
        procListStr += ara::core::String{proc->GetName()} + " ";
    }
    LOGI() << procListStr;

    startupProcessListPreSize_   = 0;
    terminateProcessListPreSize_ = 0;

    return 0;
}

/// @brief Destroy function, clears and restores internal state, objects, etc.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_EM_00123
/// @trace_id_dd=DD_EM_00473
/// @needwork = dda
/// @endcode
void FunctionGroup::StateTransferFSM::Destroy() noexcept
{
    Stop();
    startupProcessList_.clear();
    terminateProcessList_.clear();
    startupProcessListPreSize_   = 0;
    terminateProcessListPreSize_ = 0;
}

/// @brief Print debug information
/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
void FunctionGroup::StateTransferFSM::Debug() const noexcept
{
#if ARA_EXEC_DEBUG
    std::cout << "StartupProcessList: " << startupProcessList_.size() << std::endl;
    for (auto const &p : startupProcessList_) {
        std::cout << p->GetName() << " ";
    }
    std::cout << std::endl;
    std::cout << "TerminateProcessList: ";
    for (auto const &it : terminateProcessList_) {
        Process *const proc{it.first};
        std::cout << proc->GetName() << ": ";
        for (auto const &p : it.second) {
            std::cout << p->GetName() << " ";
        }
        std::cout << std::endl;
    }
#endif
}

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
int32_t FunctionGroup::StateTransferFSM::_TerminatingAction(FiniteStateMachine::State const &s,
                                                            FiniteStateMachine::Event const &e) noexcept
{
    std::ignore = s;
    std::ignore = e;

    /// @code{.isoft}
    /// @trace_id_sws=SWS_EM_01051 A process cannot terminate before the processes that depend on it terminate
    /// @endcode
    for (auto &it : terminateProcessList_) {
        Process *const proc{it.first};
        if (nullptr == proc) {
            LOGE() << "ERROR on StateTransferFSM::_TerminatingAction(): invalid Process { nullptr }";
            return -1;
        }

        /// The dependent list is not empty, indicating that there are processes that depend on itself still running, it cannot terminate itself
        if (!(it.second.empty())) {
            LOGD() << "Process {" << proc->GetName() << "," << proc->GetPid()
                   << "} have Depended process, so we don't terminate it.";
            continue;
        }

        /// No processes depend on itself, this process can be terminated

        /// If already terminating, do not resend the termination request
        if (Process::ProcessState::kTerminating == proc->GetProcessState()) {
            LOGD() << "Process {" << proc->GetName() << "," << proc->GetPid()
                   << "} is terminating, so we don't terminate it again.";
            continue;
        }

        LOGI() << "Terminate  Process {" << proc->GetName() << "," << proc->GetPid() << "}";
        if (0 != proc->Terminate()) {
            return -1;
        }
    }
    return 0;
}

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
int32_t FunctionGroup::StateTransferFSM::_StartingAction(FiniteStateMachine::State const &s,
                                                         FiniteStateMachine::Event const &e) noexcept
{
    std::ignore = s;
    std::ignore = e;
    for (auto const &proc : startupProcessList_) {
        if (nullptr == proc) {
            LOGE() << "ERROR on StateTransferFSM::_StartingAction(): invalid Process { nullptr }";
            return -1;
        }

        if (proc->IsActived()) {
            LOGD() << "Process {" << proc->GetName() << "," << proc->GetPid()
                   << "} is in Actived state, so we don't startup it.";
            continue;
        }

        /// @code{.isoft}
        /// @trace_id_sws=SWS_EM_01050 Check dependencies before starting a process
        /// @trace_id_sws=SWS_EM_02245 Execution management should ensure that execution dependency resolution is performed for processes configured for requeststate
        /// @endcode
        if (true != proc->IsExecutionDependsOk(fg_->nextState_)) {
            LOGD() << "Process {" << proc->GetName() << "," << proc->GetPid()
                   << "} ExeuctionDepends is NOT ok, so we don't startup it.";
            continue;
        }

        LOGI() << "Startup Process {" << proc->GetName() << "}";
        int32_t const ret{proc->Startup(fg_->nextState_)};
        if (0 != ret) {
            fg_->_SetExecutionError(proc->GetExecutionError(ara::core::String{fg_->nextState_}));
            LOGE() << "proc->Startup(): " << ret;
            return -1;
        }
        /// Add the process to the active list
        std::ignore = fg_->activedProcessList_.insert(proc);
    }
    return 0;
}

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
bool FunctionGroup::StateTransferFSM::_TerminatingCondition(FiniteStateMachine::State const &src,
                                                            FiniteStateMachine::State const &dest) noexcept
{
    std::ignore = src;
    std::ignore = dest;
    std::int32_t const curSize{static_cast< std::int32_t >(terminateProcessList_.size())};

    /// Only re-enter the Terminating state when the termination list changes, otherwise it will cause an infinite loop
    if (0 != curSize) {
        if (curSize != terminateProcessListPreSize_) {
            terminateProcessListPreSize_ = curSize;
            return true;
        }

        return false;
    }
    // When curSize is 0, reset the TerminateProcessListPreSize_ count
    terminateProcessListPreSize_ = 0;
    return false;
}

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
bool FunctionGroup::StateTransferFSM::_TerminatedCondition(FiniteStateMachine::State const &src,
                                                           FiniteStateMachine::State const &dest) const noexcept
{
    std::ignore = src;
    std::ignore = dest;
    return terminateProcessList_.empty();
}

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
bool FunctionGroup::StateTransferFSM::_StartingCondition(FiniteStateMachine::State const &src,
                                                         FiniteStateMachine::State const &dest) noexcept
{
    std::ignore = src;
    std::ignore = dest;
    std::int32_t const curSize{static_cast< std::int32_t >(startupProcessList_.size())};

    /// Only re-enter the Starting state when the startup list changes, otherwise it will cause an infinite loop
    if (0 != curSize) {
        /// When a self-terminating process ends, skip this condition check to see if there are processes that depend on
        /// the self-terminating process that can be started
        if (skipStartingCondition_) {
            skipStartingCondition_ = false;
            return true;
        }

        if (curSize != startupProcessListPreSize_) {
            startupProcessListPreSize_ = curSize;
            return true;
        }

        return false;
    }
    // When curSize is 0, reset the startupProcessListPreSize_ count
    startupProcessListPreSize_ = 0;
    return false;
}

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
bool FunctionGroup::StateTransferFSM::_StartedCondition(FiniteStateMachine::State const &src,
                                                        FiniteStateMachine::State const &dest) const noexcept
{
    std::ignore = src;
    std::ignore = dest;
    return startupProcessList_.empty();
}

/// @brief Default constructor
/// @param name Function group name
/// @param states Function group state list
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_EM_00123
/// @trace_id_dd=DD_EM_00434
/// @needwork = dda
/// @endcode
FunctionGroup::FunctionGroup(ara::core::StringView const &name,
                             ara::core::Vector< ara::core::String > const &states) noexcept
    : name_{name}
    , stateProcessMap_{}
    , processNameMap_{}
    , activedProcessList_{}
    , currentState_{config::GetFunctionGroupStateOff()}
    , nextState_{}
    , fsm_{std::make_unique< StateTransferFSM >(name_, this)}
    , stateChangedAction_{nullptr}
    , transErrorCode_{ara::exec::ExecErrc::kNoError}
    , executionErrorCode_{0U}
{
    /// Initialize the state and process mapping table
    for (auto const &s : states) {  // PRQA S 2961
        ProcessList ps;
        std::ignore = stateProcessMap_.emplace(make_pair(s, ps));
    }
}

/// @brief Clear the function group, restore to the initial state
/// @return 0 success; <0 failure
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_EM_00123
/// @trace_id_dd=DD_EM_00436
/// @needwork = dda
/// @endcode
int32_t FunctionGroup::Clear() noexcept
{
    if (IsAlive()) {
        return -1;
    }
    for (auto &map : stateProcessMap_) {
        map.second.clear();
    }
    processNameMap_.clear();
    activedProcessList_.clear();
    nextState_.clear();
    currentState_ = config::GetFunctionGroupStateOff();
    fsm_->Destroy();
    transErrorCode_     = ara::exec::ExecErrc::kNoError;
    executionErrorCode_ = 0U;
    return 0;
}

/// @brief Add a process to the function group
/// @param proc Process handle
/// @return 0 success; <0 failure
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_EM_00123
/// @trace_id_dd=DD_EM_00438
/// @needwork = dda
/// @endcode
int32_t FunctionGroup::AddProcess(std::shared_ptr< Process > &proc) noexcept
{
    if (nullptr == proc) {
        LOGE() << "AddProcess: proc is nullptr";
        return -1;
    }

    /// Add the process to the dependent function group states
    // Traverse the function group states that the process depends on
    ara::core::Vector< ara::core::String > fgStateNameList;
    proc->GetFunctionGroupStateNames(fgStateNameList);
    for (auto const &fgStateName : fgStateNameList) {  // PRQA S 2961
        // Find the specified function group state
        /// @code{.isoft}
        /// @trace_id_sws=SWS_EM_01109 In the case of a system configuration error, execution management cannot start a process that does not reference at least one state
        /// @endcode
        ara::core::Map< ara::core::String const, ProcessList >::iterator const it{stateProcessMap_.find(fgStateName)};
        if (it == stateProcessMap_.end()) {
            LOGE() << GetName() << " have no state: " << fgStateName;
            return -1;
        }
        // Add the process to the process list under the specified state
        ProcessList &procs{it->second};
        std::ignore = procs.emplace(proc);
    }

    /// Add the process to the mapping table
    std::ignore = processNameMap_.emplace(std::make_pair(proc->GetName(), proc));

    /// Register the process Running state callback function
    proc->OnRunning([this](Process *const p) noexcept -> void { this->_ProcessRunningHandler(p); });
    /// Register the process Terminated state callback function
    proc->OnTerminated([this](Process *const p) noexcept -> void { this->_ProcessTerminatedHandler(p); });

    return 0;
}

/// @brief Build a dependency tree for processes within the function group
/// @return 0 success; <0 failure
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_EM_00123
/// @trace_id_dd=DD_EM_00437
/// @needwork = dda
/// @endcode
int32_t FunctionGroup::GenDependTree() noexcept
{
    /// Generate a dependency tree based on function group state, because process dependencies cannot cross function group states
    // Traverse function group states
    for (auto &it : stateProcessMap_) {
        ara::core::String const &fgState{it.first};
        ProcessList &procs{it.second};
        // Traverse the process list
        for (auto const &proc : procs) {
            ara::core::Vector< ara::core::String > depNameList;
            proc->GetExecutionDependNames(fgState, depNameList);
            for (auto const &depName : depNameList) {  // PRQA S 2961
                LOGD() << "Process {" << proc->GetName() << "} depends on {" << depName << "}";
                std::shared_ptr< Process > const edProc{FindProcessByName(depName)};
                if (nullptr != edProc) {
                    std::ignore = proc->AttachExecutionDependProcess(fgState, edProc);
                } else {
                    /// @code{.isoft}
                    /// @trace_id_sws=SWS_EM_01001 Processes cannot depend across function groups
                    /// @endcode
                    LOGE() << GetName() << " have no process named: " << depName;
                    return -1;
                }
            }
        }
    }
    return 0;
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
int32_t FunctionGroup::StartTransformState(ara::core::StringView const &stateName) noexcept
{
    /// Save the next state to be switched to
    nextState_ = stateName;

    /// Initialize the state machine
    if (0 != fsm_->Init()) {
        LOGE() << "Transform function group (" << name_ << ") from" << currentState_ << "to" << stateName
               << "failed !!!";
        return -1;
    }

    /// Start the state machine
    fsm_->Start();

    return 0;
}

/// @brief Stop the ongoing function group state transition
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_EM_00123
/// @trace_id_dd=DD_EM_00442
/// @needwork = dda
/// @endcode
void FunctionGroup::StopTransformState() noexcept
{
    fsm_->Destroy();
    /// Set the function group to a temporary stop state to allow A->B->A function group switching
    currentState_ = "Stopped_State_Tmp";
    nextState_    = "";
}

/// @brief Process termination event handler function, called when a process termination event is received
/// @param proc Pointer to the terminated process
/// @code{.isoft}
/// @interface_level=none
/// @trace_id_ad=AD_EM_00123
/// @trace_id_dd=DD_EM_00454
/// @needwork = dda
/// @endcode
void FunctionGroup::_ProcessTerminatedHandler(Process *const proc) noexcept
{
    if (nullptr == proc) {
        LOGE() << "_ProcessTerminatedHandler(nullptr)";
        return;
    }
    /// Delete the process from the active list
    std::ignore = activedProcessList_.erase(proc);

    FiniteStateMachine::State const fsmState{fsm_->GetCurrentState()};
    /// If the function group enters the undefined state (the internal state machine is kInvalidState), abnormal exit of the process will not be handled further
    if (FiniteStateMachine::State::kInvalidState == fsmState) {
        LOGD() << "Process {" << proc->GetName() << "} terminated in Undefined State of" << GetName();
        return;
    }
    /// TODO: Need to handle the case where after entering the Undefined state, a residual process exits normally or abnormally, and there is a new state switching request
    /// and the process is in the startup list

    /// If the process terminated unexpectedly, error handling
    if (true != proc->IsTerminateNormally()) {
        LOGE() << "Process {" << proc->GetName() << "} terminated Unexpected because of {"
               << proc->GetTerminateCauseString() << "}";
        bool const isTerminating{FiniteStateMachine::State::kTerminating == fsmState};
        bool const isInTerminatedList{fsm_->IsInTerminatedProcessList(proc)};
        if (isTerminating && isInTerminatedList) {
            /// @code{.isoft}
            /// @trace_id_sws=SWS_EM_02258 Process termination timeout handling during state transition
            /// 1.Stop; 2.Log; 3.set Undefined; 4.report kFailed; 5
            /// @trace_id_sws=SWS_EM_02314 Process unexpected termination handling in Terminating state during state transition
            // 1. Stop; 2.Log; 3. SetUndefinedState; 4.report
            // kFailedUnexpectedTerminationOnExit; 5._SetExecutionErrorCode;
            /// @endcode
            fsm_->TriggerProcessTerminatedEvent(proc);
        } else {
            bool const isStarting{FiniteStateMachine::State::kStarting == fsmState};
            bool const isEnterTimeout{Process::TerminateCause::kEnterTimeout == proc->GetTerminateCause()};
            if (isStarting && isEnterTimeout) {
                /// @code{.isoft}
                /// @trace_id_sws=SWS_EM_02259 Process startup timeout handling during state transition
                /// 1.Stop; 2.Log; 3.set Undefined; 4.report kFailed; 5
                /// @trace_id_sws=SWS_EM_02313 Process unexpected termination handling in Starting state during state transition
                // 1. Stop; 2.Log; 3. SetUndefinedState; 4.report
                // kFailedUnexpectedTerminationOnEnter; 5._SetExecutionErrorCode;
                /// @trace_id_sws=SWS_EM_02280 Processes terminated due to startup timeout and then restarted should not satisfy any termination dependencies
                /// In fact, processes during restart will not enter the state machine's processing flow, thus will not trigger the state machine's termination event
                /// @trace_id_sws=SWS_EM_02312 The order of handling startup timeout: first execute SWS_EM_02310, then execute SWS_EM_02259
                /// @trace_id_sws=SWS_EM_02260 Processes terminated due to startup timeout should be attempted to restart
                /// @trace_id_sws=SWS_EM_02310 Processes terminated due to startup timeout and restarted, if restart exceeds the number of retries, kill the process
                /// @endcode
                /// If the restart is successful, return directly. Otherwise, enter the error handling flow
                if (0 == proc->Restartup()) {
                    /// After a successful restart, the process needs to be added to the active process list
                    std::ignore = activedProcessList_.insert(proc);
                    return;
                }

                _SetTransitionError(ara::exec::ExecErrc::kFailed);
            } else {
                _SetTransitionError(ara::exec::ExecErrc::kFailedUnexpectedTerminationOnEnter);
            }

            fsm_->Destroy();

            /// @code{.isoft}
            /// @trace_id_sws=SWS_EM_01309 Handling flow for unexpected process termination
            // 1.Log; 2.SetUndefinedState; 3._SetExecutionErrorCode;
            /// @endcode
            ara::core::StringView const transState{GetTransingState().empty() ? GetCurrentState() : GetTransingState()};
            _SetExecutionError(proc->GetExecutionError(ara::core::String(transState)));
            _SetCurrentState(config::GetFunctionGroupStateUndefined());
        }
    } else {
        fsm_->TriggerProcessTerminatedEvent(proc);
    }
}

/// @brief Process start event handler function, called when a process start event is received
/// @param proc Pointer to the terminated process
/// @code{.isoft}
/// @interface_level=none
/// @trace_id_ad=AD_EM_00123
/// @trace_id_dd=DD_EM_00455
/// @needwork = dda
/// @endcode
void FunctionGroup::_ProcessRunningHandler(Process *const proc) const noexcept
{
    fsm_->TriggerProcessStartedEvent(proc);
}

/// @brief Set the current function group state
/// @param state Function group state name
/// @code{.isoft}
/// @interface_level=none
/// @trace_id_ad=AD_EM_00123
/// @trace_id_dd=DD_EM_00453
/// @needwork = dda
/// @endcode
void FunctionGroup::_SetCurrentState(ara::core::StringView const &state) noexcept
{
    currentState_ = state;
    /// Clear the next state, marking the end of the transition
    nextState_ = "";
    if (nullptr != stateChangedAction_) {
        stateChangedAction_(this);
    }
}

/// @brief Print debug information
/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
void FunctionGroup::DebugFG() const noexcept
{
#if ARA_EXEC_DEBUG
    LOGD() << "+++++ FunctionGroup Debug +++++";
    LOGD() << "Name: " << GetName();
    LOGD() << "CurrentState: " << GetCurrentState();
    LOGD() << "ProcessMap: ";
    for (auto const &sp : stateProcessMap_) {
        ara::core::String procs{"\t" + sp.first + ":\t"};
        for (auto const &p : sp.second) {
            if (nullptr != p) {
                procs += p->GetName();
                procs += " ";
            }
        }
        LOGD() << procs;
    }
    LOGD() << "----- FunctionGroup Debug -----";
#endif  ///< ARA_EXEC_DEBUG
}

}  // namespace emd
}  // namespace internal
}  // namespace exec
}  // namespace ara
