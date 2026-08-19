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
/// @file       process.cpp
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
/// @interface_level=module
/// @endcode
///
/// ================================================================

#include "isoft/osi/process/process.h"

#include <ara/core/vector.h>
#include <nai/io/nai_event.h>
#include <nai/os/nai_proc.h>

#include <memory>
#include <tuple>

#include "ara/exec/internal/emd/exec_trust_platform.h"
#include "ara/exec/internal/emd/log.h"
#include "ara/exec/internal/emd/process.h"
#include "isoft/osi/security/seccomp.h"
#include "isoft/utils/error.h"
#include "isoft/utils/process.h"

namespace ara {
namespace exec {
namespace internal {
namespace emd {

/// @brief Redefine char
using Char8_t = char;

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
Process::Process(std::shared_ptr< config::ExecutionManifest const > manifest,
                 std::shared_ptr< isoft::naicpp::EvLoop > mainLoop,
                 std::shared_ptr< isoft::ara_fsh::Platform > fsh) noexcept
    : debugTraced_{false}
    , manifest_{std::move(manifest)}
    , gMainLoop_{std::move(mainLoop)}
    , spFsh_{std::move(fsh)}
    , handle_{}
    , pid_{0}
    , workDir_{""}
    , currentFunctionGroupState_{}
    , currentStartupConfig_{nullptr}
    , processState_{ProcessState::kIdle}
    , executionState_{ExecutionState::kInitializing}
    , terminateCause_{TerminateCause::kUnknown}
    , terminateCode_{0}
    , restartCounter_{0U}
    , cpuSet_{}
    , attribute_{}  // PRQA S 2428
    , executionDependMap_{}
    , startupAction_{nullptr}
    , terminatedAction_{nullptr}
    , runnningAction_{nullptr}
    , stateChangedAction_{nullptr}
    , exitTimer_{nullptr}
    , enterTimer_{nullptr}
{
    /// Create and start the timer, -1 means not enabled
    std::ignore = gMainLoop_->MakeTimer(enterTimer_, -1, [this]() -> void {
        // If still in the Starting state, it means timeout
        if (ProcessState::kStarting == this->GetProcessState()) {
            LOGE() << "Process " << GetName() << "{" << GetPid() << "} Enter Timeout !";
            /// @code{.isoft}
            /// @trace_id_sws=SWS_EM_002260 Kill the process with KILL signal on timeout, and attempt to restart. (Restart operation is in FunctionGroup)
            /// @endcode
            std::ignore = this->Kill(TerminateCause::kEnterTimeout);
        }
    });

    /// Create the termination timer, -1 means not enabled
    std::ignore = gMainLoop_->MakeTimer(exitTimer_, -1, [this]() -> void {
        // If still in the Terminating state, it means timeout
        if (ProcessState::kTerminating == this->GetProcessState()) {
            LOGE() << "Process " << GetName() << "{" << GetPid() << "} Exit Timeout !";
            /// @code{.isoft}
            /// @trace_id_sws=SWS_EM_02311 Termination timeout handling flow: should follow SWS_EM_002255 first, then SWS_EM_002258
            /// @trace_id_sws=SWS_EM_02255 Kill the process on timeout
            /// @endcode
            std::ignore = this->Kill(TerminateCause::kExitTimeout);
        }
    });

    /// Establish an empty dependency mapping table
    ara::core::Vector< ara::core::String > fgsNameList;
    GetFunctionGroupStateNames(fgsNameList);
    for (auto const &stateName : fgsNameList) {  // PRQA S 2961
        std::ignore
            = executionDependMap_.emplace(std::make_pair(stateName, isoft::core::Set< ExecutionDependencyPair >()));
    }

    nai_cpuset_zero(&cpuSet_);                           // PRQA S ALL
    for (uint16_t const cpu : manifest_->GetCpuSet()) {  // PRQA S 2961
        nai_cpuset_set(cpu, &cpuSet_);                   // PRQA MS "nai_cpuset_set" ALL    // NOLINT
    }

    /// Initialize process attributes
    std::ignore = nai_proc_attr_init(&attribute_);

    /// @code{.isoft}
    /// @trace_id_sws=SWS_EM_02104 CPU affinity
    /// @endcode
    /// Only set CPU affinity if a CPU set is configured
    if (!manifest_->GetCpuSet().empty()) {
        std::ignore = nai_proc_attr_set_affinity(&attribute_, sizeof(cpuSet_), &cpuSet_);
    }

    /// Set the current working directory of the process {chdir(getcwd())}
    workDir_ = spFsh_->GetSwclRootDir(_GetSwclName().data(), _GetSwclVersion().data());
    if (!workDir_.empty()) {
        std::ignore = nai_proc_attr_set_cwd(&attribute_, workDir_.c_str());
    }
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
int32_t Process::AttachExecutionDependProcess(ara::core::String const &fgState,
                                              std::shared_ptr< Process > const &proc) noexcept
{
    if (nullptr == proc) {
        return -1;
    }

    ara::core::Map< ara::core::String const, isoft::core::Set< ExecutionDependencyPair > >::iterator const it{
        executionDependMap_.find(fgState)};
    if (executionDependMap_.end() == it) {
        return -1;
    }

    isoft::core::Set< ExecutionDependencyPair > &execDepPairList{it->second};
    config::ExecutionDependency const *const execDep{
        manifest_->FindExecutionDepend(fgState, ara::core::String{proc->GetName()})};
    if (nullptr == execDep) {
        return -1;
    }

    std::ignore = execDepPairList.emplace(ExecutionDependencyPair{execDep, proc});

    return 0;
}

/// @brief Check whether the execution dependencies of the process are all satisfied
/// @param fgStateName Function group state. Talking about process dependencies without a function group state is meaningless, because processes cannot establish dependencies across states
/// @return true, dependency satisfied; false, dependency not satisfied
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_EM_00122
/// @trace_id_dd=DD_EM_00537
/// @needwork = dda
/// @endcode
bool Process::IsExecutionDependsOk(ara::core::String const &fgStateName) const noexcept
{
    ara::core::Map< ara::core::String const, isoft::core::Set< ExecutionDependencyPair > >::const_iterator const it{
        executionDependMap_.find(fgStateName)};
    if (it == executionDependMap_.end()) {
        LOGE() << GetName() << "{" << GetPid() << "} is not in the function group state {" << fgStateName << "}";
        return false;
    }

    isoft::core::Set< ExecutionDependencyPair > const &execPairs{it->second};
    bool isOk{true};
    for (auto const &ecp : execPairs) {
        // Verify that the dependent process name and object are consistent
        if (ecp.depInfo->GetProcessName() != ecp.proc->GetName()) {
            LOGE() << GetName() << "{" << GetPid() << "} depends on a invalid process {"
                   << ecp.depInfo->GetProcessName() << "vs" << ecp.proc->GetName() << "}";
            isOk = false;
        }

        switch (ecp.depInfo->GetProcessState()) {
            case config::ExecutionDependencyState::kRunning: {
                if (ProcessState::kRunning != ecp.proc->GetProcessState()) {
                    isOk = false;
                }
                break;
            }
            case config::ExecutionDependencyState::kTerminated: {
                if (ProcessState::kTerminated != ecp.proc->GetProcessState()) {
                    isOk = false;
                }
                break;
            }
            default: {
                LOGE() << "Unknown process state {"
                       << static_cast< int16_t >(config::ExecutionDependencyState::kTerminated) << "}";
                isOk = false;
                break;
            }
        }
    }

    return isOk;
}

/// @brief Get the list of run-dependent processes in the specified function group state
/// @param fgStateName Function group state
/// @param procList Process list
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_EM_00122
/// @trace_id_dd=DD_EM_00559
/// @needwork = dda
/// @endcode
void Process::GetRunningDependProcessList(ara::core::String const &fgStateName,
                                          ara::core::Vector< Process * > &procList) const noexcept
{
    ara::core::Map< ara::core::String const, isoft::core::Set< ExecutionDependencyPair > >::const_iterator const it{
        executionDependMap_.find(fgStateName)};
    if (it == executionDependMap_.end()) {
        LOGE() << GetName() << "{" << GetPid() << "} is not in the function group state {" << fgStateName << "}";
        return;
    }

    isoft::core::Set< ExecutionDependencyPair > execPairs{it->second};
    for (auto const &ecp : execPairs) {
        // Verify that the dependent process name and object are consistent
        if (ecp.depInfo->GetProcessName() != ecp.proc->GetName()) {
            LOGE() << GetName() << "{" << GetPid() << "} depends on a invalid process {"
                   << ecp.depInfo->GetProcessName() << "vs" << ecp.proc->GetName() << "}";
            return;
        }

        if (config::ExecutionDependencyState::kRunning == ecp.depInfo->GetProcessState()) {
            procList.emplace_back(ecp.proc.get());
        }
    }
}

/// @brief Check whether the process is active
/// @return Yes/No
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_EM_00122
/// @trace_id_dd=DD_EM_00538
/// @needwork = dda
/// @endcode
bool Process::IsActived() const noexcept
{
    if ((GetProcessState() != ProcessState::kIdle) && (GetProcessState() != ProcessState::kTerminated)) {
        return true;
    }

    return false;
}

/// @brief Verify the executable program
/// @return 0 verification successful; <0 verification failed
/// @code{.isoft}
/// @interface_level=none
/// @trace_id_ad=AD_EM_00122
/// @trace_id_dd=DD_EM_00566
/// @needwork = dda
/// @endcode
int32_t Process::_VerifyExecutable() const noexcept
{
    if (0 != access(_GetExecutablePath().c_str(), F_OK | X_OK)) {
        LOGE() << "_VerifyExecutable(): invalid executable path {" << _GetExecutablePath() << "}";
        return -1;
    }

    /// @code{.isoft}
    /// @trace_id_sws=SWS_EM_02309 In strict mode, if the authenticity or integrity check of the executable file fails
    /// @endcode
    if (0 != ExecTrustPlatform::VerifyProcExecObj(_GetSwclName().data(), _GetSwclVersion().data(), GetName().data())) {
        LOGE() << "Verify executable object failed (ProcName = " << GetName() << ") !!!";
        return -1;
    }

    return 0;
}

/// @brief Restart the process, if the number of restarts exceeds the limit, return failure
/// @return 0 success; <0 failure
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_EM_00122
/// @trace_id_dd=DD_EM_00539
/// @needwork = dda
/// @endcode
int32_t Process::Restartup() noexcept
{
    if (restartCounter_ >= manifest_->GetNumberOfRestartAttempts()) {
        return -1;
    }
    restartCounter_++;
    LOGI() << "Process {" << GetName() << ":" << GetPid() << "} Attempt to Restartup {" << restartCounter_ << "}";
    return Startup(currentFunctionGroupState_);
}

/// @brief Start the process with the configuration in the specified function group state
/// @param fgState Function group state
/// @return 0 success; <0 failure
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_EM_00122
/// @trace_id_dd=DD_EM_00540
/// @needwork = dda
/// @endcode
int32_t Process::Startup(ara::core::String const &fgState) noexcept
{
    _Debug();
    /// Activity detection, if the current process is active, prohibit starting
    if (IsActived()) {
        LOGE() << "Process" << GetName() << "{" << GetPid() << "} is Actived";
        return -1;
    }

    /// Check the executable file
    if (0 != _VerifyExecutable()) {
        LOGE() << "_VerifyExecutable(): " << GetName() << "{" << GetPid() << "}.";
        return -1;
    }

    /// Get the startup configuration
    config::StateDependentConfig const *const pStartupConfig{manifest_->FindStartupConfig(fgState)};
    if (nullptr == pStartupConfig) {
        LOGE() << "FindStartupConfig(" << fgState << "): " << GetName() << "{" << GetPid()
               << "} Can`t findout the specified startup config.";
        return -1;
    }

#if NAI_HAVE_FORK  ///< If the current platform supports the fork() system call, use nai_fork mode
    /// Make the child process sleep just after creation so that the parent process can record its pid
    /// This operation is to avoid the child process terminating right after running, and the parent process not having time to record the pid before receiving the process termination event, thus not being able to obtain process information
    pid_ = nai_proc_fork(&handle_, 1);
    if (pid_ < 0) {
        LOGE() << "nai_fork() failed !!!";
        return -1;
    }
    if (pid_ == 0) {
        return _RunExec(pStartupConfig);
    }
#else   /// If fork() is not supported, use spawn mode. In this mode, seccomp is invalid
    /// Make the child process sleep just after creation so that the parent process can record its pid
    /// This operation is to avoid the child process terminating right after running, and the parent process not having time to record the pid before receiving the process termination event, thus not being able to obtain process information
    if (0 > nai_proc_attr_set_suspend(&attr, 1)) {
        LOGE() << "nai_proc_attr_set_suspend(): " << isoft::utils::StrError();
        return -1;
    }

    /// Create a system process and execute the new image
    retCode = nai_proc_spawn(&handle_, _GetExecutablePath().c_str(), &attr, arguments.data(), environments.data());
    if (0 > retCode) {
        LOGE() << "nai_proc_spawn(" << _GetExecutablePath() << ")";
        return -1;
    }
#endif  ///< NAI_HAVE_FORK
    /// @note After nai_proc_exec succeeds, the child process will go to execute the new executable file, and the subsequent code will not be executed
    ///		  Therefore, the following code segment is executed by the parent process (execution management) continues to execute
    pid_                       = nai_proc_get_pid(&handle_);
    currentFunctionGroupState_ = fgState;

    return _UpdateRunningState(pStartupConfig);
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
void Process::_SetExecEnvVar(config::StateDependentConfig const *const pStateDepConfig,
                             ara::core::Vector< ara::core::String > &environments) noexcept
{
    std::ignore = environments;
    if (pStateDepConfig == nullptr) {
        return;
    }
    /// @code{.isoft}
    /// @trace_id_sws=SWS_EM_02247 Execution management should prepare environment variables according to the machine configuration and pass them during process startup
    /// @trace_id_sws=SWS_EM_02246
    /// Execution management should prepare environment variables according to the process.stateDependentStartupConfig.startupConfig configuration and pass them during process startup
    /// @trace_id_sws=SWS_EM_02248
    /// Whenever the same environment variable is configured in both the execution manifest and the machine manifest, execution management should use the environment variable value from the execution manifest
    /// From the machine manifest, if it already exists, discard and do not insert
    /// @endcode
    environments = pStateDepConfig->GetEnvironmentVariables();

    ara::core::Vector< ara::core::String > const &sysEnv{manifest_->GetSysEnvironments()};
    std::ignore = environments.insert(environments.cend(), sysEnv.begin(), sysEnv.end());  // PRQA S 2961
}

/// @brief Set the execution attributes for the process
/// @param pStateDepConfig State dependency configuration
/// @return <0 failure; 0 success
/// @code{.isoft}
/// @interface_level=none
/// @trace_id_ad=AD_EM_00122
/// @trace_id_dd=DD_EM_00575
/// @needwork = dda
/// @endcode
int32_t Process::_SetExecAttribute(config::StateDependentConfig const *const pStateDepConfig) noexcept
{
    /// @code{.isoft}
    /// @trace_id_sws=SWS_EM_01014 Process scheduling policy
    /// @trace_id_sws=SWS_EM_01041 Provide FIFO process scheduling policy
    /// @trace_id_sws=SWS_EM_01042 Provide RR process scheduling policy
    /// @trace_id_sws=SWS_EM_01043 Provide OTHER process scheduling policy
    /// @endcode
    int32_t naiSched{0};
    switch (pStateDepConfig->GetSchedulingPolicy()) {
        case config::SchedulingPolicy::kFIFO: {
            naiSched = NAI_SCHED_FIFO;
            break;
        }
        case config::SchedulingPolicy::kRoundRobin: {
            naiSched = NAI_SCHED_RR;
            break;
        }
        case config::SchedulingPolicy::kOther: {
            naiSched = NAI_SCHED_NORMAL;
            break;
        }
        default: {
            break;
        }
    }

    /// @code{.isoft}
    /// @trace_id_sws=SWS_EM_01015 Process priority
    /// @endcode
    /// NAI only provides 5 levels by default. If you need to use system native levels, you need to give a negative value to nai_proc_attr_set_sched priority
    /// Also, SWS stipulates that higher numerical values represent higher priority
    int32_t const naiPriority{ToNaiPriority(pStateDepConfig->GetSchedulingPriority())};

    if (nai_proc_attr_set_sched(&attribute_, naiSched, naiPriority) < 0) {
        LOGE() << "nai_proc_attr_set_sched(" << static_cast< int32_t >(naiSched) << ","
               << static_cast< int32_t >(naiPriority) << ")";
        return -1;
    }

    return 0;
}

/// @brief Set the execution parameters for the process
/// @param pStateDepConfig State dependency configuration
/// @param arguments Process startup parameters
/// @code{.isoft}
/// @interface_level=none
/// @trace_id_ad=AD_EM_00122
/// @trace_id_dd=DD_EM_00576
/// @needwork = dda
/// @endcode
void Process::_SetExecArguments(config::StateDependentConfig const *const pStateDepConfig,
                                std::vector< ara::core::String > &arguments) const noexcept
{
    /// @code{.isoft}
    /// @trace_id_sws=SWS_EM_01012 When starting a process, the parameters configured in StartupConfig should be passed
    /// @trace_id_sws=SWS_EM_01072 Process parameter 0 should be the executable program name
    /// @trace_id_sws=SWS_EM_01078 Parameters in startupConfig should start from parameter 1
    /// @endcode
    arguments.emplace_back(_GetExecutableName());
    for (auto const &arg : pStateDepConfig->GetProcessArguments()) {  // PRQA S 2961
        arguments.emplace_back(arg);
    }
}

/// @brief After forking the process, update the process state
/// @param pStateDepConfig State dependency configuration
/// @return 0 success; <0 failure
/// @code{.isoft}
/// @interface_level=none
/// @trace_id_ad=AD_EM_00122
/// @trace_id_dd=DD_EM_00577
/// @needwork = dda
/// @endcode
int32_t Process::_UpdateRunningState(config::StateDependentConfig const *const pStateDepConfig) noexcept
{
    currentStartupConfig_ = pStateDepConfig;
    SetTerminateCode(0);  ///< Clear the previous exit code
    _SetTerminateCause(TerminateCause::kUnknown);

    /// Set the execution state to Starting
    /// @code{.isoft}
    /// @trace_id_sws=SWS_EM_01003 After creating and initializing resources, enter the Starting state
    /// @endcode
    SetProcessState(ProcessState::kStarting);

    /// @code{.isoft}
    /// @trace_id_sws=SWS_EM_01402 Non-reporting process state handling: if it is a reporting process, wait for subsequent reporting of Running via the execution client.
    /// @trace_id_sws=SWS_EM_02253 Execution management should monitor the time required for process startup (the time between when execution management requests the operating system to create the process and when the process successfully reports the running state)
    /// @endcode
    if (manifest_->IsReportingProcess()) {
        _StartEnterTimer(pStateDepConfig->GetEnterTimeout());
    } else {  ///< If it is a non-reporting process, directly set to Running state
        /// Run in the executor to prevent nested deadlocks
        std::ignore = gMainLoop_->Exec([this]() noexcept -> void { SetProcessState(ProcessState::kRunning); });
    }

    /// If the child process is not being debugged, wake up the child process
    if (!debugTraced_) {
        if (0 > nai_proc_kill(&handle_, SIGCONT)) {
            LOGE() << "nai_proc_kill(SIGCONT): " << isoft::utils::StrError();
            /// The child process cannot continue to run, need to kill the process
            return Kill(TerminateCause::kInitFailure);
        }
    } else {
        /// Debugging state, the IDE will wake up the child process
        LOGD() << "Waiting for debugging process {" << GetName() << "," << handle_.pid << "} !!! ";
    }

    return 0;
}

/// @brief Execute the executable file corresponding to the process
/// @param pStateDepConfig State dependency configuration
/// @return 0 success; <0 failure
/// @code{.isoft}
/// @interface_level=none
/// @trace_id_ad=AD_EM_00122
/// @trace_id_dd=DD_EM_00578
/// @needwork = dda
/// @endcode
int32_t Process::_RunExec(config::StateDependentConfig const *const pStateDepConfig) noexcept
{
    /// This operation must be called in the child process, i.e., after fork()
    /// When the parent process (execution management) dies, kill itself
    if (0 > isoft::osi::proc::ExitFollowupFather()) {
        LOGW() << "ExitFollowupFather():" << isoft::utils::StrError();
    }

    /// NOTE: To support UCM updating OS by calling sh scripts, allow UCM processes to create subprocesses
    /// FIXME: This is a temporary solution, which brings great security risks: UCM's subprocesses will escape the control of EMD, potentially consuming resources for a long time or excessively
    if (!manifest_->IsUpdateManager()) {
        /// Prohibit the child process from creating further subprocesses
        if (0 > isoft::osi::security::seccomp::DisableSysCallFork()) {
            LOGW() << "DisableSysCallFork():" << isoft::utils::StrError();
        }
    }

    uint32_t const heapBytes{pStateDepConfig->GetResouceConsumption().GetHeapUsage()};
    uint32_t const memBytes{pStateDepConfig->GetResouceConsumption().GetSystemMemoryUsage()};
    if (isoft::osi::proc::LimitProcResourceUse(heapBytes, memBytes) != 0) {
        LOGE() << "Limit the process heap/memory usage failed !!!";
        return -1;
    }

    /// Set the execution attributes for the process
    if (_SetExecAttribute(pStateDepConfig) < 0) {
        return -1;
    }
    /// Set the environment variables for the process
    ara::core::Vector< ara::core::String > environments;
    _SetExecEnvVar(pStateDepConfig, environments);

    /// Set the startup parameters for the process
    std::vector< ara::core::String > arguments;
    _SetExecArguments(pStateDepConfig, arguments);

    ara::core::Vector< Char8_t const * > envs;
    for (auto const &env : environments) {  // PRQA S 2961
        envs.emplace_back(env.c_str());
    }
    envs.emplace_back(nullptr);

    ara::core::Vector< Char8_t const * > args;
    for (auto const &arg : arguments) {  // PRQA S 2961
        args.emplace_back(arg.c_str());
    }
    args.emplace_back(nullptr);

    if (nai_exec(_GetExecutablePath().c_str(), &attribute_, args.data(), envs.data()) < 0) {
        LOGE() << "nai_exec(" << _GetExecutablePath() << "):" << isoft::utils::StrError();
        return -1;
    }

    return 0;
}

/// @brief Send a termination request to the process
/// @return 0 success; <0 failure
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_EM_00122
/// @trace_id_dd=DD_EM_00543
/// @needwork = dda
/// @endcode
int32_t Process::Terminate() noexcept
{
    if (!IsActived()) {
        LOGW() << "Process {" << GetName() << "," << GetPid() << "} is already terminated.";
        return 0;
    }

    /// Send the termination request
    /// @code{.isoft}
    /// @trace_id_sws=SWS_EM_01055 Send SIGTERM to initiate process termination
    /// @endcode
    if (nai_proc_kill(&handle_, SIGTERM) < 0) {
        if (isoft::utils::GetErrNo() == ESRCH) {
            SetProcessState(ProcessState::kTerminated);
            LOGW() << "Process {" << GetName() << "," << GetPid() << "} is already terminated.";
            return 0;
        }

        LOGE() << "Send SIGTERM to Process " << GetName() << " {" << GetPid() << "} error:" << isoft::utils::StrError();
        return -1;
    }

    /// @code{.isoft}
    /// @trace_id_sws=SWS_EM_01404 After sending a termination request, execution management should set the Terminating process state
    /// @endcode
    SetProcessState(ProcessState::kTerminating);
    /// Set the termination reason
    _SetTerminateCause(TerminateCause::kRequest);

    /// If the process is being debugged and is at a breakpoint, then do not start the timeout detection timer
    /// @code{.isoft}
    /// @trace_id_sws=SWS_EM_01065 Monitor termination timeout when the process terminates
    /// @endcode
    if (isoft::utils::process::IsProcessTraced(pid_)) {
        LOGD() << "Process {" << GetName() << "} stops at a break point, do not start the ExitTimer !!!";
    } else {
        _StartExitTimer(currentStartupConfig_->GetExitTimeout());
    }

    return 0;
}

/// @brief Directly kill the process
/// @param termCause Termination state (reason)
/// @return 0 success; <0 failure
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_EM_00122
/// @trace_id_dd=DD_EM_00544
/// @needwork = dda
/// @endcode
int32_t Process::Kill(TerminateCause const termCause) noexcept
{
    if (!IsActived()) {
        LOGV() << "Can`t kill process " << GetName() << "{" << GetPid() << "}"
               << ", because it`s InActived.";
        return 0;
    }

    LOGI() << "Force to kill process" << GetName() << "{" << GetPid()
           << "} with TerminateCause:" << GetTerminateCauseString(termCause);

    /// Send the kill signal
    if (nai_proc_kill(&handle_, SIGKILL) < 0) {
        if (isoft::utils::GetErrNo() == ESRCH) {
            LOGW() << "Process {" << GetName() << "," << GetPid() << "} is already terminated.";
            return 0;
        }
        LOGE() << "Send SIGKILL to Process " << GetName() << " {" << GetPid() << "} error:" << isoft::utils::StrError();
        return -1;
    }

    /// Set the process state and exit reason
    SetProcessState(ProcessState::kTerminating);
    _SetTerminateCause(termCause);

    return 0;
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
void Process::TriggerEvent(Event const &e) noexcept
{
#if 0
    /// Call the callback function in the executor to prevent recursive nesting of callbacks
    gMainLoop_->Exec([e, this]() -> void {
        switch (e) {
            case Event::kTerminated: {
                /// TODO: Cleanup failure, should report an error and make the process enter an error state, and call the callback function
                _Clear();
                /// @code{.isoft}
                /// @trace_id_sws=SWS_EM_01006 After the process terminates, the Terminated state should be set
                /// @endcode
                SetProcessState(ProcessState::kTerminated);
                break;
            }
            case Event::kRunning: {
                /// Clear the restart counter after a successful start
                restartCounter_ = 0;
                /// @code{.isoft}
                /// @trace_id_sws=SWS_EM_01004 After receiving the report, execution management sets the Running state
                /// @endcode
                SetProcessState(ProcessState::kRunning);
                break;
            }
        }
    });
#else
    switch (e) {
        case Event::kTerminated: {
            LOGD() << "Process " << GetName() << "{" << GetPid() << "} TriggerEvent { Terminated }";
            /// TODO: Cleanup failure, should report an error and make the process enter an error state, and call the callback function
            std::ignore = _Clear();
            _StopExitTimer();
            /// @code{.isoft}
            /// @trace_id_sws=SWS_EM_01006 After the process terminates, the Terminated state should be set
            /// @endcode
            SetProcessState(ProcessState::kTerminated);
            break;
        }
        case Event::kRunning: {
            LOGD() << "Process " << GetName() << "{" << GetPid() << "} TriggerEvent { Running }";
            /// Clear the restart counter after a successful start
            restartCounter_ = 0U;
            _StopEnterTimer();
            /// @code{.isoft}
            /// @trace_id_sws=SWS_EM_01004 After receiving the report, execution management sets the Running state
            /// @endcode
            SetProcessState(ProcessState::kRunning);
            break;
        }
    }
#endif
}

/// @brief Set the process state
/// @param state Process state
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_EM_00122
/// @trace_id_dd=DD_EM_00529
/// @needwork = dda
/// @endcode
void Process::SetProcessState(ProcessState const &state) noexcept
{
    processState_ = state;
    LOGI() << "Process" << GetName() << "{" << GetPid() << "} Enter" << GetProcessStateString() << "State.";

    if (this->stateChangedAction_) {
        this->stateChangedAction_(this);
    }

    switch (this->processState_) {
        case ProcessState::kRunning: {
            if (this->runnningAction_) {
                this->runnningAction_(this);
            }
            break;
        }
        case ProcessState::kTerminated: {
            if (this->terminatedAction_) {
                this->terminatedAction_(this);
            }
            break;
        }
        case ProcessState::kStarting: {
            if (this->startupAction_) {
                this->startupAction_(this);
            }
            break;
        }
        default: {
            break;
        }
    }
}

/// @brief Clean up all resources of the process object
/// @return 0 success; <0 failure
/// @code{.isoft}
/// @interface_level=none
/// @trace_id_ad=AD_EM_00122
/// @trace_id_dd=DD_EM_00579
/// @needwork = dda
/// @endcode
int32_t Process::_Clear() noexcept
{
    currentStartupConfig_ = nullptr;
    /// No need and cannot clear currentFunctionGroupState_, restart requires it
    /// currentFunctionGroupState_ = "";

    pid_ = 0;
    return 0;
}

/// @brief Convert scheduling priority to NAI priority
/// @param prio Process priority
/// @return Process priority in NAI format
/// @code{.isoft}
/// @interface_level=none
/// @trace_id_ad=AD_EM_00122
/// @trace_id_dd=DD_EM_00580
/// @needwork = dda
/// @endcode
int32_t Process::ToNaiPriority(uint32_t const prio) noexcept
{
    /// The priority range for real-time processes recognized by nai is 0-98, corresponding to Linux 1~99
    /// The priority range for non-real-time processes recognized by nai is 0-39, corresponding to Linux 19~-20
    return -(static_cast< int32_t >(prio)) + 1;
}

/// @brief Debug print information
void Process::_Debug() const noexcept
{
#if ARA_EXEC_DEBUG
    std::cout << "+++++ Process::Debug +++++\n";
    manifest_->Debug();
    std::cout << "Pid:\t" << GetPid() << std::endl;
    std::cout << "DebugTraced: " << debugTraced_ << std::endl;
    std::cout << "----- Process::Debug -----\n";

#endif  ///< ARA_EXEC_DEBUG
}

}  // namespace emd
}  // namespace internal
}  // namespace exec
}  // namespace ara
