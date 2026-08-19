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
/// @file       process.h
/// @brief      Process class definition
/// @details
/// @date       2023-04-01
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/ExecutionManagement/Emd
/// @unit_name=Process
/// @unit_description=Used to manage the execution of processes in the execution management system.
/// @interface_level=unit
/// @endcode
///
/// ================================================================

#ifndef _ARA_EXEC_INTERNAL_PROCESS_H_
#define _ARA_EXEC_INTERNAL_PROCESS_H_

#include <ara/core/map.h>
#include <ara/core/span.h>
#include <ara/core/string.h>
#include <isoft/ara_fsh/filesystem_hierarchy.h>
#include <isoft/core/set.h>
#include <isoft/naicpp/evloop.h>
#include <nai/io/nai_event.h>
#include <nai/io/nai_signal.h>
#include <nai/os/nai_proc.h>

#include <cstdint>
#include <functional>
#include <memory>

#include "ara/exec/internal/config/execution_manifest.h"
#include "isoft/utils/mix.h"

namespace ara {
namespace exec {
namespace internal {
namespace emd {

/// @brief Process class, describing all information of a modeled process
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_sr=SR_EM_00001~SR_EM_00030
/// @trace_id_ad=AD_EM_00122
/// @trace_id_dd=DD_EM_00516
/// @needwork = ad
/// @endcode
class Process
{
public:
    /// @brief Used to describe the process execution state, used for interaction with the execution client
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_EM_00004
    /// @trace_id_ad=AD_EM_00122
    /// @trace_id_dd=DD_EM_00517
    /// @needwork = dda
    /// @endcode
    enum class ExecutionState : uint8_t
    {
        /// @code{.isoft}
        /// @interface_level=unit
        /// @trace_id_ad=AD_EM_00122
        /// @trace_id_dd=DD_EM_00837
        /// @needwork = dda
        /// @endcode
        kInitializing = 0,  ///< Initial state, before the reporting process reports kRunning
        /// @code{.isoft}
        /// @interface_level=unit
        /// @trace_id_ad=AD_EM_00122
        /// @trace_id_dd=DD_EM_00838
        /// @needwork = dda
        /// @endcode
        kRunning,  ///< After the reporting process reports kRunning
        /// @code{.isoft}
        /// @interface_level=unit
        /// @trace_id_ad=AD_EM_00122
        /// @trace_id_dd=DD_EM_00839
        /// @needwork = dda
        /// @endcode
        kTerminating,  ///< The self-terminating process actively reports kTerminating
    };

    /// @brief Used to describe the internal lifecycle of the process
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_EM_00004
    /// @trace_id_ad=AD_EM_00122
    /// @trace_id_dd=DD_EM_00518
    /// @needwork = dda
    /// @endcode
    enum class ProcessState : uint8_t
    {
        /// @code{.isoft}
        /// @interface_level=unit
        /// @trace_id_ad=AD_EM_00122
        /// @trace_id_dd=DD_EM_00825
        /// @needwork = dda
        /// @endcode
        kIdle = 0,  ///< Process has not been created and its resources are not allocated.

        /// @code{.isoft}
        /// @interface_level=unit
        /// @trace_id_ad=AD_EM_00122
        /// @trace_id_dd=DD_EM_00826
        /// @needwork = dda
        /// @endcode
        kStarting,  ///< Process has been created and its resources are allocated.

        /// @code{.isoft}
        /// @interface_level=unit
        /// @trace_id_ad=AD_EM_00122
        /// @trace_id_dd=DD_EM_00827
        /// @needwork = dda
        /// @endcode
        kRunning,  ///< Process has been scheduled and reported kRunning to EM.

        /// @code{.isoft}
        /// @interface_level=unit
        /// @trace_id_ad=AD_EM_00122
        /// @trace_id_dd=DD_EM_00828
        /// @needwork = dda
        /// @endcode
        kTerminating,  ///< Process has decided to self-terminate, or EM sent SIGTERM signal to it.

        /// @code{.isoft}
        /// @interface_level=unit
        /// @trace_id_ad=AD_EM_00122
        /// @trace_id_dd=DD_EM_00829
        /// @needwork = dda
        /// @endcode
        kTerminated  ///< Process has terminated and its resources have been freed.
    };

    /// @brief Process end state, used to describe whether the process ended normally or was killed
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_EM_00011
    /// @trace_id_ad=AD_EM_00122
    /// @trace_id_dd=DD_EM_00519
    /// @needwork = dda
    /// @endcode
    enum class TerminateCause : uint8_t
    {
        /// @code{.isoft}
        /// @interface_level=unit
        /// @trace_id_ad=AD_EM_00122
        /// @trace_id_dd=DD_EM_00830
        /// @needwork = dda
        /// @endcode
        kSelf = 0,  ///< Self-terminating process, ends on its own

        /// @code{.isoft}
        /// @interface_level=unit
        /// @trace_id_ad=AD_EM_00122
        /// @trace_id_dd=DD_EM_00831
        /// @needwork = dda
        /// @endcode
        kRequest,  ///< Requested to end due to switching function groups, etc.

        /// @code{.isoft}
        /// @interface_level=unit
        /// @trace_id_ad=AD_EM_00122
        /// @trace_id_dd=DD_EM_00832
        /// @needwork = dda
        /// @endcode
        kInitFailure,  ///< Startup failure

        /// @code{.isoft}
        /// @interface_level=unit
        /// @trace_id_ad=AD_EM_00122
        /// @trace_id_dd=DD_EM_00833
        /// @needwork = dda
        /// @endcode
        kEnterTimeout,  ///< Startup timeout

        /// @code{.isoft}
        /// @interface_level=unit
        /// @trace_id_ad=AD_EM_00122
        /// @trace_id_dd=DD_EM_00834
        /// @needwork = dda
        /// @endcode
        kExitTimeout,  ///< Exit timeout

        /// @code{.isoft}
        /// @interface_level=unit
        /// @trace_id_ad=AD_EM_00122
        /// @trace_id_dd=DD_EM_00835
        /// @needwork = dda
        /// @endcode
        kInvalidTransition,  ///< Invalid state transition

        /// @code{.isoft}
        /// @interface_level=unit
        /// @trace_id_ad=AD_EM_00122
        /// @trace_id_dd=DD_EM_00836
        /// @needwork = dda
        /// @endcode
        kUnknown,  ///< Exit for unknown reasons, need to determine based on return value, etc.
    };

    /// @brief Process events, used to trigger process actions
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_EM_00015
    /// @trace_id_ad=AD_EM_00122
    /// @trace_id_dd=DD_EM_00520
    /// @needwork = dda
    /// @endcode
    enum class Event : uint8_t
    {
        /// @brief Process termination event
        /// @code{.isoft}
        /// @interface_level=unit
        /// @trace_id_ad=AD_EM_00122
        /// @trace_id_dd=DD_EM_00823
        /// @needwork = dda
        /// @endcode
        kTerminated,

        /// @brief Process running event
        /// @code{.isoft}
        /// @interface_level=unit
        /// @trace_id_ad=AD_EM_00122
        /// @trace_id_dd=DD_EM_00824
        /// @needwork = dda
        /// @endcode
        kRunning
    };

    /// @brief Process state change callback function
    /// @param proc Pointer to the process whose state has changed
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    using StateChangedActionConst = std::function< void(Process const *proc) >;

    /// @brief Process state change callback function
    /// @param proc Pointer to the process whose state has changed
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    using StateChangedAction = std::function< void(Process *proc) >;

    /// @brief Disable default constructor
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    Process() noexcept = delete;

    /// @brief Constructor
    /// @param manifest Execution manifest associated with the current process
    /// @param mainLoop Main event loop handle
    /// @param fsh fsh instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_EM_00122
    /// @trace_id_dd=DD_EM_00521
    /// @needwork = dda
    /// @endcode
    Process(std::shared_ptr< config::ExecutionManifest const > manifest,
            std::shared_ptr< isoft::naicpp::EvLoop > mainLoop,
            std::shared_ptr< isoft::ara_fsh::Platform > fsh) noexcept;

    /// @brief Default destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_EM_00122
    /// @trace_id_dd=DD_EM_00522
    /// @needwork = dda
    /// @endcode
    ~Process() = default;

    /// @brief Disable move constructor
    /// @param other the other Process
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    Process(Process &&other) noexcept = delete;

    /// @brief Disable copy constructor
    /// @param other the other Process
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    Process(Process const &other) noexcept = delete;

    /// @brief Disable move assignment
    /// @param other the other Process
    /// @return new Process
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    Process &operator=(Process &&other) noexcept = delete;

    /// @brief Disable copy assignment
    /// @param other the other Process
    /// @return new Process
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    Process &operator=(Process const &other) noexcept = delete;

    /// @brief Get the process name
    /// @return Process name
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_EM_00122
    /// @trace_id_dd=DD_EM_00523
    /// @needwork = dda
    /// @endcode
    ara::core::StringView const GetName() const noexcept
    {
        if (manifest_ != nullptr) {
            return manifest_->GetProcName();
        }

        return "";
    }

    /// @brief Get the process FQN
    /// @return Process FQN
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_EM_00122
    /// @trace_id_dd=DD_EM_00524
    /// @needwork = dda
    /// @endcode
    ara::core::StringView const GetFqn() const noexcept
    {
        if (manifest_ != nullptr) {
            return manifest_->GetProcFqn();
        }

        return "";
    }

    /// @brief Get the process pid
    /// @return Process PID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_EM_00122
    /// @trace_id_dd=DD_EM_00525
    /// @needwork = dda
    /// @endcode
    int32_t GetPid() const noexcept { return pid_; }

    /// @brief Get the process handle
    /// @return Process handle
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_EM_00122
    /// @trace_id_dd=DD_EM_00526
    /// @needwork = dda
    /// @endcode
    nai_proc_t GetHandle() const noexcept { return handle_; }

    /// @brief Get the current execution state
    /// @return Process execution state
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_EM_00122
    /// @trace_id_dd=DD_EM_00527
    /// @needwork = dda
    /// @endcode
    ExecutionState GetExecutionState() const noexcept { return executionState_; }

    /// @brief Get the current process state
    /// @return Process state
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_EM_00122
    /// @trace_id_dd=DD_EM_00528
    /// @needwork = dda
    /// @endcode
    ProcessState GetProcessState() const noexcept { return processState_; }

    /// @brief Set the process state
    /// @param state Process state
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_EM_00122
    /// @trace_id_dd=DD_EM_00529
    /// @needwork = dda
    /// @endcode
    void SetProcessState(ProcessState const &state) noexcept;

    /// @brief Check if the process is in the running state
    /// @return Yes/No
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_EM_00122
    /// @trace_id_dd=DD_EM_00530
    /// @needwork = dda
    /// @endcode
    bool IsRunning() const noexcept { return (GetProcessState() == ProcessState::kRunning); }

    /// @brief Get the process state string
    /// @return Process state string
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_EM_00122
    /// @trace_id_dd=DD_EM_00531
    /// @needwork = dda
    /// @endcode
    ara::core::StringView const GetProcessStateString() const noexcept
    {
        ara::core::StringView procStateString;
        switch (GetProcessState()) {
            case Process::ProcessState::kIdle: {
                procStateString = "Idle";
                break;
            }

            case Process::ProcessState::kRunning: {
                procStateString = "Running";
                break;
            }

            case Process::ProcessState::kTerminated: {
                procStateString = "Terminated";
                break;
            }

            case Process::ProcessState::kStarting: {
                procStateString = "Starting";
                break;
            }

            case Process::ProcessState::kTerminating: {
                procStateString = "Terminating";
                break;
            }

            default: {
                procStateString = "Unknown";
                break;
            }
        }

        return procStateString;
    }

    /// @brief Get the process exit state
    /// @return Process exit state
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_EM_00122
    /// @trace_id_dd=DD_EM_00532
    /// @needwork = dda
    /// @endcode
    TerminateCause GetTerminateCause() const noexcept { return terminateCause_; }

    /// @brief Get the process exit state string
    /// @return Process exit state string
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_EM_00122
    /// @trace_id_dd=DD_EM_00533
    /// @needwork = dda
    /// @endcode
    ara::core::StringView GetTerminateCauseString() const noexcept
    {
        return GetTerminateCauseString(GetTerminateCause());
    }

    /// @brief Check if it is a normal termination
    /// @return Yes/No
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_EM_00122
    /// @trace_id_dd=DD_EM_00534
    /// @needwork = dda
    /// @endcode
    bool IsTerminateNormally() const noexcept
    {
        /// If the exit code is not 0, it is always considered abnormal
        if (GetTerminateCode() != 0) {
            return false;
        }

        /// Only processes actively terminated by EM or self-terminating processes with an exit code of 0 are considered normal termination.
        bool const isReqTerm{GetTerminateCause() == TerminateCause::kRequest};
        bool const selfTerm{IsSelfTerminate()};
        bool const isSelfTerm{(GetTerminateCause() == TerminateCause::kUnknown) && selfTerm};
        if (isReqTerm || isSelfTerm) {
            return true;
        }

        return false;
    }

    /// @brief Check if it is a self-terminating process
    /// @return Yes/No
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_EM_00122
    /// @trace_id_dd=DD_EM_00535
    /// @needwork = dda
    /// @endcode
    bool IsSelfTerminate() const noexcept
    {
        config::StateDependentConfig const *const sc{GetStartupConfig(currentFunctionGroupState_)};
        if (nullptr == sc) {
            return false;
        }
        return sc->IsSelfTerminate();
    }

    /// @brief Associate execution-dependent processes
    /// @param fgState Function group state name
    /// @param proc Process object pointer
    /// @return 0 success; <0 failure
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_EM_00122
    /// @trace_id_dd=DD_EM_00536
    /// @needwork = dda
    /// @endcode
    int32_t AttachExecutionDependProcess(ara::core::String const &fgState,
                                         std::shared_ptr< Process > const &proc) noexcept;

    /// @brief Check whether the execution dependencies of the process are all satisfied
    /// @param fgStateName Function group state. Talking about process dependencies without a function group state is meaningless, because processes cannot establish dependencies across states
    /// @return true, dependency satisfied; false, dependency not satisfied
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_EM_00122
    /// @trace_id_dd=DD_EM_00537
    /// @needwork = dda
    /// @endcode
    bool IsExecutionDependsOk(ara::core::String const &fgStateName) const noexcept;

    /// @brief Check whether the process is active
    /// @return Yes/No
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_EM_00122
    /// @trace_id_dd=DD_EM_00538
    /// @needwork = dda
    /// @endcode
    bool IsActived() const noexcept;

    /// @brief Restart the process, if the number of restarts exceeds the limit, return failure
    /// @return 0 success; <0 failure
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_EM_00122
    /// @trace_id_dd=DD_EM_00539
    /// @needwork = dda
    /// @endcode
    int32_t Restartup() noexcept;

    /// @brief Start the process with the configuration in the specified function group state
    /// @param fgState Function group state
    /// @return 0 success; <0 failure
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_EM_00122
    /// @trace_id_dd=DD_EM_00540
    /// @needwork = dda
    /// @endcode
    int32_t Startup(ara::core::String const &fgState) noexcept;

    /// @brief Set debug mode
    /// @param debugTraced Whether in debug traced mode; in this mode, after the process starts, it will be paused and timeouts are ignored
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_EM_00122
    /// @trace_id_dd=DD_EM_00541
    /// @needwork = dda
    /// @endcode
    void SetDebugTraced(bool const debugTraced) noexcept { debugTraced_ = debugTraced; }

    /// @brief Whether the process is set to trace mode by the IDE
    /// @return true in trace mode; false not in trace mode
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_EM_00122
    /// @trace_id_dd=DD_EM_00542
    /// @needwork = dda
    /// @endcode
    bool IsDebugTraced() const noexcept { return debugTraced_; }

    /// @brief Send a termination request to the process
    /// @return 0 success; <0 failure
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_EM_00122
    /// @trace_id_dd=DD_EM_00543
    /// @needwork = dda
    /// @endcode
    int32_t Terminate() noexcept;

    /// @brief Directly kill the process
    /// @param termCause Termination state (reason)
    /// @return 0 success; <0 failure
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_EM_00122
    /// @trace_id_dd=DD_EM_00544
    /// @needwork = dda
    /// @endcode
    int32_t Kill(TerminateCause const termCause) noexcept;

    /// @brief Set the process termination code, from the waitpid function
    /// @param code Exit code
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_EM_00122
    /// @trace_id_dd=DD_EM_00545
    /// @needwork = dda
    /// @endcode
    void SetTerminateCode(int32_t const code) noexcept { terminateCode_ = code; }

    /// @brief Get the process termination code, from the waitpid function
    /// @return Exit code
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_EM_00122
    /// @trace_id_dd=DD_EM_00546
    /// @needwork = dda
    /// @endcode
    int32_t GetTerminateCode() const noexcept { return terminateCode_; }

    /// @brief Get the execution error code of the process in the specified function group state
    /// @param fgState Function group state
    /// @return Execution error code
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_EM_00122
    /// @trace_id_dd=DD_EM_00547
    /// @needwork = dda
    /// @endcode
    uint32_t GetExecutionError(ara::core::String const &fgState) const noexcept
    {
        return manifest_->GetExecutionError(fgState);
    }

    /// @brief Event trigger function
    /// @note  Some processes (real system processes) need execution management to notify the Process class of events, because the SIGCHLD callback function and the execution client's callback function are both maintained by the ExecutionManager class, so ExecutionManager objects need to notify Process objects
    ///         1. Process termination event, after the process terminates, it will send a SIGCHLD signal to execution management;
    ///         2. Process start event, the process will report a Running message to execution management
    /// @param e Event
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_EM_00122
    /// @trace_id_dd=DD_EM_00548
    /// @needwork = dda
    /// @endcode
    void TriggerEvent(Event const &e) noexcept;

    /// @brief Register a process startup callback function, called when the process enters the Startup state
    /// @param action Callback function
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_EM_00122
    /// @trace_id_dd=DD_EM_00549
    /// @needwork = dda
    /// @endcode
    void OnStartup(StateChangedAction const &action) noexcept { startupAction_ = action; }

    /// @brief Register a process termination callback function, called when the process enters the Terminated state
    /// @param action Callback function
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_EM_00122
    /// @trace_id_dd=DD_EM_00550
    /// @needwork = dda
    /// @endcode
    void OnTerminated(StateChangedAction const &action) noexcept { terminatedAction_ = action; }

    /// @brief Register a process running callback function, called when the process enters the Running state
    /// @param action Callback function
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_EM_00122
    /// @trace_id_dd=DD_EM_00551
    /// @needwork = dda
    /// @endcode
    void OnRunning(StateChangedAction const &action) noexcept { runnningAction_ = action; }

    /// @brief Register a process state change function, called when the process state changes
    /// @param action Callback function
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_EM_00122
    /// @trace_id_dd=DD_EM_00552
    /// @needwork = dda
    /// @endcode
    void OnStateChanged(StateChangedActionConst const &action) noexcept { stateChangedAction_ = action; }

    /// @brief Get the list of function group state names that the process depends on
    /// @param nameList Save the obtained list of state names
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_EM_00122
    /// @trace_id_dd=DD_EM_00553
    /// @needwork = dda
    /// @endcode
    void GetFunctionGroupStateNames(ara::core::Vector< ara::core::String > &nameList) const noexcept
    {
        if (nullptr != manifest_) {
            manifest_->GetFunctionGroupStateNames(nameList);
        }
    }

    /// @brief Get the list of process names that the process execution depends on
    /// @param fgState Specified function group name
    /// @param nameList Save the obtained list of process names
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_EM_00122
    /// @trace_id_dd=DD_EM_00554
    /// @needwork = dda
    /// @endcode
    void GetExecutionDependNames(ara::core::String const &fgState,
                                 ara::core::Vector< ara::core::String > &nameList) const noexcept
    {
        if (nullptr != manifest_) {
            manifest_->GetExecutionDependNames(fgState, nameList);
        }
    }

    /// @brief Get the function group state in which the current process is running
    /// @return Function group state name
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_EM_00122
    /// @trace_id_dd=DD_EM_00555
    /// @needwork = dda
    /// @endcode
    ara::core::StringView GetCurrentFunctionGroupState() const noexcept { return currentFunctionGroupState_; }

    /// @brief Get the resource group name in which the current process is running
    /// @return Resource group name
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_EM_00122
    /// @trace_id_dd=DD_EM_00556
    /// @needwork = dda
    /// @endcode
    ara::core::StringView const GetCurrentResourceGroupName() const noexcept
    {
        if (nullptr == currentStartupConfig_) {
            return "";
        }
        return currentStartupConfig_->GetResourceGroupName();
    }

    /// @brief Get the startup configuration in the specified function group state
    /// @param fgState Specified function group name
    /// @return Startup configuration pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_EM_00122
    /// @trace_id_dd=DD_EM_00557
    /// @needwork = dda
    /// @endcode
    config::StateDependentConfig const *GetStartupConfig(ara::core::String const &fgState) const noexcept
    {
        return manifest_->FindStartupConfig(ara::core::String{fgState});
    }

    /// @brief Get the startup configuration used by the current process
    /// @return Startup configuration pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_EM_00122
    /// @trace_id_dd=DD_EM_00558
    /// @needwork = dda
    /// @endcode
    config::StateDependentConfig const *GetCurrentStartupConfig() const noexcept { return currentStartupConfig_; }

    /// @brief Get the list of run-dependent processes in the specified function group state
    /// @param fgStateName Function group state
    /// @param procList Process list
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_EM_00122
    /// @trace_id_dd=DD_EM_00559
    /// @needwork = dda
    /// @endcode
    void GetRunningDependProcessList(ara::core::String const &fgStateName,
                                     ara::core::Vector< Process * > &procList) const noexcept;

    /// @brief Determine whether it is a state management process
    /// @return true is state management process; false is not state management process
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_EM_00122
    /// @trace_id_dd=DD_EM_00560
    /// @needwork = dda
    /// @endcode
    bool IsStateManager() const noexcept
    {
        if (manifest_ != nullptr) {
            return manifest_->IsStateManager();
        }
        return false;
    }

private:
    /// @brief Get the software cluster name
    /// @return Name of the software cluster the process belongs to
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00122
    /// @trace_id_dd=DD_EM_00561
    /// @needwork = dda
    /// @endcode
    ara::core::StringView const _GetSwclName() const noexcept
    {
        if (manifest_ != nullptr) {
            return manifest_->GetSwclName();
        }
        return "";
    }

    /// @brief Get the software cluster version
    /// @return Version of the software cluster the process belongs to
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00122
    /// @trace_id_dd=DD_EM_00562
    /// @needwork = dda
    /// @endcode
    ara::core::StringView const _GetSwclVersion() const noexcept
    {
        if (manifest_ != nullptr) {
            return manifest_->GetSwclVersion();
        }
        return "";
    }

    /// @brief Debug print information
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    void _Debug() const noexcept;

    /// @brief Get the process exit state string
    /// @param terminateState Process exit state
    /// @return Process exit state string
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00122
    /// @trace_id_dd=DD_EM_00563
    /// @needwork = dda
    /// @endcode
    static ara::core::StringView GetTerminateCauseString(TerminateCause const terminateState) noexcept
    {
        ara::core::StringView termStateString;
        switch (terminateState) {
            case TerminateCause::kSelf: {
                termStateString = "Self-Terminated";
                break;
            }
            case TerminateCause::kRequest: {
                termStateString = "Request";
                break;
            }
            case TerminateCause::kInitFailure: {
                termStateString = "kInitFailure";
                break;
            }
            case TerminateCause::kEnterTimeout: {
                termStateString = "EnterTimeout";
                break;
            }
            case TerminateCause::kExitTimeout: {
                termStateString = "ExitTimeout";
                break;
            }
            case TerminateCause::kInvalidTransition: {
                termStateString = "InvalidTransition";
                break;
            }
            case TerminateCause::kUnknown: {
                termStateString = "Unknown";
                break;
            }

            default: {
                termStateString = "Unsupported Terminate State";
                break;
            }
        }

        return termStateString;
    }

    /// @brief Get the process executable program path
    /// @return Executable program path
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00122
    /// @trace_id_dd=DD_EM_00565
    /// @needwork = dda
    /// @endcode
    ara::core::String _GetExecutablePath() const noexcept
    {
        if (manifest_ != nullptr) {
            return manifest_->GetExecutablePath();
        }

        return "";
    }

    /// @brief Verify the executable program
    /// @return 0 verification successful; <0 verification failed
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00122
    /// @trace_id_dd=DD_EM_00566
    /// @needwork = dda
    /// @endcode
    int32_t _VerifyExecutable() const noexcept;

    /// @brief Set the process exit state
    /// @param state Termination state
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00122
    /// @trace_id_dd=DD_EM_00567
    /// @needwork = dda
    /// @endcode
    void _SetTerminateCause(TerminateCause const &state) noexcept { terminateCause_ = state; }

    /// @brief Start the startup timeout detection timer
    /// @param timeout Timeout, in seconds
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00122
    /// @trace_id_dd=DD_EM_00568
    /// @needwork = dda
    /// @endcode
    void _StartEnterTimer(double const timeout) noexcept
    {
        if (!IsDebugTraced()) {
            // The timer is in milliseconds
            int32_t const tmMs{isoft::utils::TimeS2MS(timeout)};
            std::ignore = enterTimer_->UpdateTime(tmMs);
        }
    }

    /// @brief Stop the startup timeout detection timer
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00122
    /// @trace_id_dd=DD_EM_00569
    /// @needwork = dda
    /// @endcode
    void _StopEnterTimer() noexcept
    {
        if (!IsDebugTraced()) {
            std::ignore = enterTimer_->UpdateTime(-1);
        }
    }

    /// @brief Start the termination timeout detection timer
    /// @param timeout Timeout, in seconds
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00122
    /// @trace_id_dd=DD_EM_00570
    /// @needwork = dda
    /// @endcode
    void _StartExitTimer(double const timeout) noexcept
    {
        // The timer is in milliseconds
        int32_t const tmMs{isoft::utils::TimeS2MS(timeout)};
        std::ignore = exitTimer_->UpdateTime(tmMs);
    }

    /// @brief Stop the termination timeout detection timer
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00122
    /// @trace_id_dd=DD_EM_00571
    /// @needwork = dda
    /// @endcode
    void _StopExitTimer() noexcept { std::ignore = exitTimer_->UpdateTime(-1); }

    /// @brief Get the executable program name
    /// @return Executable name, or empty string
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00122
    /// @trace_id_dd=DD_EM_00572
    /// @needwork = dda
    /// @endcode
    ara::core::StringView const _GetExecutableName() const noexcept
    {
        if (manifest_ != nullptr) {
            return manifest_->GetExecutableName();
        }
        return "";
    }

    /// @brief Set the environment variables for the process
    /// @param pStateDepConfig State dependency configuration
    /// @param environments Environment variable information
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00122
    /// @trace_id_dd=DD_EM_00574
    /// @needwork = dda
    /// @endcode
    void _SetExecEnvVar(config::StateDependentConfig const *const pStateDepConfig,
                        ara::core::Vector< ara::core::String > &environments) noexcept;

    /// @brief Set the execution attributes for the process
    /// @param pStateDepConfig State dependency configuration
    /// @return <0 failure; 0 success
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00122
    /// @trace_id_dd=DD_EM_00575
    /// @needwork = dda
    /// @endcode
    int32_t _SetExecAttribute(config::StateDependentConfig const *const pStateDepConfig) noexcept;

    /// @brief Set the execution parameters for the process
    /// @param pStateDepConfig State dependency configuration
    /// @param arguments Process startup parameters
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00122
    /// @trace_id_dd=DD_EM_00576
    /// @needwork = dda
    /// @endcode
    void _SetExecArguments(config::StateDependentConfig const *const pStateDepConfig,
                           std::vector< ara::core::String > &arguments) const noexcept;

    /// @brief After forking the process, update the process state
    /// @param pStateDepConfig State dependency configuration
    /// @return 0 success; <0 failure
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00122
    /// @trace_id_dd=DD_EM_00577
    /// @needwork = dda
    /// @endcode
    int32_t _UpdateRunningState(config::StateDependentConfig const *const pStateDepConfig) noexcept;

    /// @brief Execute the executable file corresponding to the process
    /// @param pStateDepConfig State dependency configuration
    /// @return 0 success; <0 failure
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00122
    /// @trace_id_dd=DD_EM_00578
    /// @needwork = dda
    /// @endcode
    int32_t _RunExec(config::StateDependentConfig const *const pStateDepConfig) noexcept;

    /// @brief Clean up all resources of the process object
    /// @return 0 success; <0 failure
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00122
    /// @trace_id_dd=DD_EM_00579
    /// @needwork = dda
    /// @endcode
    int32_t _Clear() noexcept;

    /// @brief Convert scheduling priority to NAI priority
    /// @param prio Process priority
    /// @return Process priority in NAI format
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00122
    /// @trace_id_dd=DD_EM_00580
    /// @needwork = dda
    /// @endcode
    static int32_t ToNaiPriority(uint32_t const prio) noexcept;

private:
    /// @brief Ignore the process's enter & exit timeout
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00122
    /// @trace_id_dd=DD_EM_00581
    /// @needwork = dda
    /// @endcode
    bool debugTraced_;

    /// @brief Execution manifest
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00122
    /// @trace_id_dd=DD_EM_00582
    /// @needwork = dda
    /// @endcode
    std::shared_ptr< config::ExecutionManifest const > manifest_;
    /// @brief Global event loop handle
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00122
    /// @trace_id_dd=DD_EM_00583
    /// @needwork = dda
    /// @endcode
    std::shared_ptr< isoft::naicpp::EvLoop > gMainLoop_;

    /// @brief fsh instance
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00122
    /// @trace_id_dd=DD_EM_00584
    /// @needwork = dda
    /// @endcode
    std::shared_ptr< isoft::ara_fsh::Platform > spFsh_;

    /// @brief nai process handle
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00122
    /// @trace_id_dd=DD_EM_00585
    /// @needwork = dda
    /// @endcode
    nai_proc_t handle_;
    /// @brief Process pid
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00122
    /// @trace_id_dd=DD_EM_00586
    /// @needwork = dda
    /// @endcode
    int32_t pid_;
    /// @brief Process working directory
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00122
    /// @trace_id_dd=DD_EM_00587
    /// @needwork = dda
    /// @endcode
    ara::core::String workDir_;

    /// @brief Save the function group state in which the current process is located
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00122
    /// @trace_id_dd=DD_EM_00588
    /// @needwork = dda
    /// @endcode
    ara::core::String currentFunctionGroupState_;

    /// @brief Save the startup configuration currently used by the process, only valid during activity, set to nullptr after the process terminates
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00122
    /// @trace_id_dd=DD_EM_00589
    /// @needwork = dda
    /// @endcode
    config::StateDependentConfig const *currentStartupConfig_;

    /// @brief Current execution state of the process
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sws=SWS_EM_01002 Initial state is Idle
    /// @trace_id_ad=AD_EM_00122
    /// @trace_id_dd=DD_EM_00590
    /// @needwork = dda
    /// @endcode
    ProcessState processState_;
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00122
    /// @trace_id_dd=DD_EM_00591
    /// @needwork = dda
    /// @endcode
    ExecutionState executionState_;

    /// @brief Process exit state
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00122
    /// @trace_id_dd=DD_EM_00592
    /// @needwork = dda
    /// @endcode
    TerminateCause terminateCause_;

    /// @brief Process exit return code
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00122
    /// @trace_id_dd=DD_EM_00593
    /// @needwork = dda
    /// @endcode
    int32_t terminateCode_;

    /// @brief Process restart counter, increments by 1 each restart, cleared on successful start
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00122
    /// @trace_id_dd=DD_EM_00594
    /// @needwork = dda
    /// @endcode
    uint16_t restartCounter_;

    /// @brief Execution dependency mapping, for quick detection of process dependencies
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_EM_00015
    /// @trace_id_ad=AD_EM_00122
    /// @trace_id_dd=DD_EM_00595
    /// @needwork = dda
    /// @endcode
    class ExecutionDependencyPair
    {
    public:
        /// @brief ExecutionDependency, dependency structure from the execution manifest
        /// @code{.isoft}
        /// @interface_level=none
        /// @trace_id_ad=AD_EM_00122
        /// @trace_id_dd=DD_EM_00783
        /// @needwork = dda
        /// @endcode
        config::ExecutionDependency const *depInfo;

        /// @brief Dependent process object
        /// @code{.isoft}
        /// @interface_level=none
        /// @trace_id_ad=AD_EM_00122
        /// @trace_id_dd=DD_EM_00784
        /// @needwork = dda
        /// @endcode
        std::shared_ptr< Process > proc;
    };

    /// @brief CPU set bound to the process
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00122
    /// @trace_id_dd=DD_EM_00596
    /// @needwork = dda
    /// @endcode
    nai_cpuset_t cpuSet_;

    /// @brief Process attributes
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00122
    /// @trace_id_dd=DD_EM_00597
    /// @needwork = dda
    /// @endcode
    nai_proc_attr_t attribute_;

    /// @brief Execution dependency mapping, for quick checking of dependency conditions
    /// String Function group state, because dependencies cannot cross function group states
    /// ExecutionDependencyPair, dependency pair
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00122
    /// @trace_id_dd=DD_EM_00598
    /// @needwork = dda
    /// @endcode
    ara::core::Map< ara::core::String const, isoft::core::Set< ExecutionDependencyPair > > executionDependMap_;

    /// @brief Process startup callback function
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00122
    /// @trace_id_dd=DD_EM_00599
    /// @needwork = dda
    /// @endcode
    StateChangedAction startupAction_;

    /// @brief Process terminated callback function
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00122
    /// @trace_id_dd=DD_EM_00600
    /// @needwork = dda
    /// @endcode
    StateChangedAction terminatedAction_;

    /// @brief Process running callback function
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00122
    /// @trace_id_dd=DD_EM_00601
    /// @needwork = dda
    /// @endcode
    StateChangedAction runnningAction_;

    /// @brief Process state change function
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00122
    /// @trace_id_dd=DD_EM_00602
    /// @needwork = dda
    /// @endcode
    StateChangedAction stateChangedAction_;

    /// @brief Process termination timeout detection timer
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00122
    /// @trace_id_dd=DD_EM_00603
    /// @needwork = dda
    /// @endcode
    isoft::naicpp::EvLoop::TimerPtr exitTimer_;

    /// @brief Process startup timeout detection timer
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00122
    /// @trace_id_dd=DD_EM_00604
    /// @needwork = dda
    /// @endcode
    isoft::naicpp::EvLoop::TimerPtr enterTimer_;

    /// @brief Execution dependency comparison function for process objects
    /// @param l Execution dependency
    /// @param r Execution dependency
    /// @return Comparison result
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00122
    /// @trace_id_dd=DD_EM_00785
    /// @needwork = dda
    /// @endcode
    friend bool operator<(Process::ExecutionDependencyPair const &l,
                          Process::ExecutionDependencyPair const &r) noexcept;

};  ///< class Process

/// @brief Execution dependency comparison function for process objects
/// @param l Execution dependency
/// @param r Execution dependency
/// @return Comparison result
/// @code{.isoft}
/// @interface_level=none
/// @trace_id_ad=AD_EM_00122
/// @trace_id_dd=DD_EM_00785
/// @needwork = dda
/// @endcode
inline bool operator<(Process::ExecutionDependencyPair const &l, Process::ExecutionDependencyPair const &r) noexcept
{
    if (l.depInfo != r.depInfo) {
        return l.depInfo < r.depInfo;
    }

    return l.proc < r.proc;
}
}  // namespace emd
}  // namespace internal
}  // namespace exec
}  // namespace ara

#endif  ///< _ARA_EXEC_INTERNAL_PROCESS_H_
