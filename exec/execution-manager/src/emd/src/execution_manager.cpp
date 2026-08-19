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
/// @file       execution_manager.cpp
/// @brief      Execution management class definition
/// @details
/// @date       2023-04-01
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/ExecutionManagement/Emd
/// @unit_name=ExecutionManager
/// @unit_description=Used to manage the execution of processes in the execution management system.
/// @interface_level=module
/// @endcode
///
/// ================================================================

#include "ara/exec/internal/emd/execution_manager.h"

#include <ara/log/internal/initialization.h>

#include <array>
#include <memory>

#include "ara/exec/internal/emd/exec_trust_platform.h"
#include "ara/exec/internal/emd/ipc.h"
#include "ara/exec/internal/emd/log.h"
#include "isoft/ipc/ipc_base.h"
#include "isoft/osi/process/process.h"
#include "isoft/utils/error.h"
#include "nai/io/nai_event.h"
#include "nai/io/nai_signal.h"
#include "nai/os/nai_proc.h"

namespace ara {
namespace exec {
namespace internal {
namespace emd {

/// @brief Register the signal processing function for the child process after fork
/// @code{.isoft}
/// @interface_level=none
/// @trace_id_ad=AD_EM_00124
/// @trace_id_dd=DD_EM_00684
/// @needwork = dda
/// @endcode
void ExecutionManager::AtForkHandler() noexcept { std::ignore = signal(SIGTERM, SIG_DFL); }  // PRQA S ALL

/// @brief Signal handler function
/// @param signalHander Signal handle
/// @param events Event number
/// @return 0 success; <0 failure
/// @code{.isoft}
/// @interface_level=none
/// @trace_id_ad=AD_EM_00124
/// @trace_id_dd=DD_EM_00685
/// @needwork = dda
/// @endcode
int32_t ExecutionManager::SignalHandler(nai_signal_t *const signalHander, int32_t const events) noexcept
{
    // PRQA S ALL ++
    if ((static_cast< uint32_t >(events) & NAI_EV_MASK) != NAI_EV_SIGNAL) {
        return 0;
    }

    ExecutionManager *em{nullptr};
    int32_t const signum{static_cast< int32_t >(NAI_EV_SIGNAL_CODE(static_cast< uint32_t >(events)))};
    switch (signum) {
        case SIGCHLD: {
            em = nai_containof(signalHander, ExecutionManager, signalChild_);  // NOLINT
            int32_t r{-1};
            int32_t code{0};
            do {
                r = nai_proc_wait(nullptr, &code, 1);
                if (r < 0) {
                    continue;
                }

                // If the process has not exited (wait function returns due to continue/stop signals), then do nothing
                if (!NAI_IFSIGNALED(code) && !NAI_IFEXITED(code)) {
                    continue;
                }

                Process *const proc{em->_FindProcessByPid(static_cast< uint32_t >(r))};
                if (nullptr == proc) {
                    LOGE() << "Received SIGCHILD signal from " << r << ", but Can't find process";
                    continue;
                }

                /// Update the process PID to 0, indicating that the process has terminated
                em->procFinder_.SetPid(proc, 0U);
                if (NAI_IFSIGNALED(code)) {
                    LOGI() << proc->GetName() << "{" << proc->GetPid() << "} Terminated by signal {"
                           << NAI_EXITCODE(code) << "}.";
                } else if (NAI_IFEXITED(code)) {
                    LOGI() << proc->GetName() << "{" << proc->GetPid() << "} Terminated with exit code {"
                           << NAI_EXITCODE(code) << "}.";
                } else {
                    LOGI() << proc->GetName() << "{" << proc->GetPid() << "} Terminated abnormally with {"
                           << NAI_EXITCODE(code) << "}.";
                }

                /// Trigger the process termination event
                proc->SetTerminateCode(NAI_EXITCODE(code));
                proc->TriggerEvent(Process::Event::kTerminated);
            } while (r > 0);
            break;
        }
        case SIGINT: {
            em = nai_containof(signalHander, ExecutionManager, signalInterrupt_);  // NOLINT
            LOGI() << "Stop the emd Rudely.";
            std::ignore = em->Stop();
            break;
        }
        case SIGTERM: {
            em = nai_containof(signalHander, ExecutionManager, signalTerminate_);  // NOLINT
            ara::core::StringView const machineFgName{em->configManager_->GetMachineFunctionGroupFqn()};
            LOGI() << "the MachineFG will be Off";
            if (0 != em->_TransformFunctionGroupState(machineFgName, config::GetFunctionGroupStateOff())) {
                LOGE() << "_TransformFunctionGroupState(" << machineFgName << ", " << config::GetFunctionGroupStateOff()
                       << ")";
                LOGI() << "because we can't Transform MachineFG state to Off normaly, then Stop the Machine "
                          "Rudely.";
                std::ignore = em->Stop();
            }
            break;
        }
        default: {
            break;
        }
    }
    // PRQA S ALL --

    return 0;
}

/// @brief Singleton pattern to create an execution management object
/// @param araSysroot ara root path
/// @param araConfig ara configuration path
/// @param debugArg Debug parameters
/// @return Class object handle
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_EM_00124
/// @trace_id_dd=DD_EM_00640
/// @needwork = dda
/// @endcode
std::shared_ptr< ExecutionManager > ExecutionManager::CreateInstance(  // NOLINT
    ara::core::String const &araSysroot,
    ara::core::String const &araConfig,
    Argument const &debugArg) noexcept
{
    static std::shared_ptr< ExecutionManager > s_EmdInstance{nullptr};
    if (nullptr == s_EmdInstance) {
        // PRQA S 3801,5217 ++
        s_EmdInstance = std::shared_ptr< ExecutionManager >{new ExecutionManager(araSysroot, araConfig, debugArg)};
        // PRQA S 3801,5217 --
        if (0 != s_EmdInstance->_Init()) {
            LOGF() << "ExecutionManager::Init()";
            s_EmdInstance.reset();
        }
    }

    return s_EmdInstance;
}  // PRQA S 2707

/// @brief Execution management constructor
/// @param araSysroot The root path of the ara platform
/// @param araConfig ara configuration file name
/// @param debugArg Debug parameters
/// @code{.isoft}
/// @interface_level=none
/// @trace_id_ad=AD_EM_00124
/// @trace_id_dd=DD_EM_00645
/// @needwork = dda
/// @endcode
ExecutionManager::ExecutionManager(ara::core::String araSysroot,
                                   ara::core::String araConfig,
                                   Argument const &debugArg) noexcept
    : araSysroot_{std::move(araSysroot)}
    , araConfig_{std::move(araConfig)}
    , debugArg_{debugArg}
    , debugger_{}
    , configManager_{nullptr}
    , procFinder_{}
    , functionGroups_{}
    , resourceGroups_{}
    , gMainLoop_{nullptr}
    , spFsh_{nullptr}
    , isStopped_{false}
    , signalChild_{}
    , signalInterrupt_{}
    , signalTerminate_{}
    , smsServer_{}
    , emsServer_{}
    , umsServer_{}
{
}

/// @brief Start execution management
/// @return 0 success; <0 failure
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_EM_00124
/// @trace_id_dd=DD_EM_00642
/// @needwork = dda
/// @endcode
int32_t ExecutionManager::Run() noexcept
{
    int32_t ret{0};
    ara::core::StringView machineFgName;

    if (debugArg_.IsVerboseOn()) {
        LOGI() << "ExecutionManager Startup in VERBOSE mode.";
        _Debug();
    } else {
        LOGI() << "ExecutionManager Startup in NORMAL mode.";
    }

    /// Instead of goto
    while (true) {
        /// If the debug service is enabled, do not start the machine; start it in the debugger's callback function
        if (debugger_.IsServiceOn()) {
            LOGI() << "Waiting for recv trace info from IDE ...";
        } else {
            /// @code{.isoft}
            /// @trace_id_sws=SWS_EM_01023 EM should start the machine as soon as possible after startup
            /// @endcode
            machineFgName = configManager_->GetMachineFunctionGroupFqn();
            LOGI() << "bengin to Startup the MachineFG.";
            if (0 != _TransformFunctionGroupState(machineFgName, config::GetFunctionGroupStateStartup())) {
                /// @code{.isoft}
                /// @trace_id_sws=SWS_EM_02250 If the startup state is not configured for the function group MachineFG, execution management will stop the startup of the AUTOSAR adaptive platform
                /// @endcode
                LOGF() << "_TransformFunctionGroupState(" << machineFgName << ", "
                       << config::GetFunctionGroupStateStartup() << ")";
                ret = -1;
                break;
            }
        }

        if (isoft::kSuccess != gMainLoop_->Run(true)) {
            LOGF() << "gMainLoop_.Run(true)";
            ret = -1;
            break;
        }

        /// Instead of goto
        break;
    }  ///< while(true) {

    if (_PostRelease() != 0) {
        LOGE() << "Emd failed to do the post release !!!";
        ret = -1;
    }

    return ret;
}

/// @brief Stop execution management
/// @return 0 success; <0 failure
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_EM_00124
/// @trace_id_dd=DD_EM_00643
/// @needwork = dda
/// @endcode
int32_t ExecutionManager::Stop() noexcept
{
    if (!isStopped_) {
        LOGD() << "Stop Execuition Manager !!!";
        _Destroy();
        gMainLoop_->Stop();
        isStopped_ = true;
    }

    return 0;
}

/// @brief Create ems, ums, sms server
/// @return 0 success; <0 failure
/// @code{.isoft}
/// @interface_level=none
/// @trace_id_ad=AD_EM_00124
/// @trace_id_dd=DD_EM_00657
/// @needwork = dda
/// @endcode
int32_t ExecutionManager::_InitServer() noexcept
{
    // Open EMC
    if (0 != emsServer_.Open(gMainLoop_)) {
        LOGE() << "emsServer_.Open()";
        return -1;
    }
    emsServer_.OnReport([this](ems::Message::State const &state, uint32_t const pid) noexcept -> void {
        _ExecutionClientReportHandler(state, pid);
    });

    // Open SMC
    if (0 != smsServer_.Open(gMainLoop_)) {
        LOGE() << "smsServer_.Open()";
        return -1;
    }
    smsServer_.OnGetFunctionGroupState(
        [this](ara::core::StringView const &fgName, ara::core::StringView const &fgState) noexcept -> void {
            _StateClientGetFunctionGroupStateHandler(fgName, fgState);
        });
    smsServer_.OnSetFunctionGroupState(
        [this](ara::core::StringView const &fgName, ara::core::StringView const &fgState) noexcept -> void {
            _StateClientSetFunctionGroupStateHandler(fgName, fgState);
        });
    smsServer_.OnCancelFunctionGroupStateTransition(
        [this](ara::core::StringView const &fgName, ara::core::StringView const &fgState) noexcept -> void {
            _StateClientCancelFunctionGroupStateTransitionHandler(fgName, fgState);
        });

    // Open UMS
    if (0 != umsServer_.Open(gMainLoop_)) {
        LOGE() << "umsServer_.Open()";
        return -1;
    }
    umsServer_.OnUpdateUserSwcl([this](core::Vector< ums::SwclInfo > const &invalidSwcls,
                                       core::Vector< ums::SwclInfo > const &validSwcls) noexcept -> void {
        _UpdateClientUpdateUserSwclHandler(invalidSwcls, validSwcls);
    });
    umsServer_.OnGetUserSwclManifest([this]() noexcept -> void { _UpdateClientGetUserSwclManifestHandler(); });

    return 0;
}

/// @brief Initialize the debugger
/// @return 0 success; <0 failure
/// @code{.isoft}
/// @interface_level=none
/// @trace_id_ad=AD_EM_00124
/// @trace_id_dd=DD_EM_00654
/// @needwork = dda
/// @endcode
int32_t ExecutionManager::_InitDebugger() noexcept  // NOLINT
{
    /// If the debugger service is not enabled, return
    if (!debugArg_.IsServerOn()) {
        return 0;
    }

    rds::Debugger::GetFunctionGroupInfoCallBack const getFgCb{
        [this](ara::core::StringView const &fgName, rds::FunctionGroupInfo &info) -> bool {
            std::shared_ptr< FunctionGroup > fg{_FindFunctionGroupByName(fgName)};
            if (fg == nullptr) {
                return false;
            }
            info.SetName(fgName);
            info.SetState(fg->GetCurrentState());
            fg->TraversalProcess([&info](std::shared_ptr< Process > const &proc) noexcept -> void {
                if (proc == nullptr) {
                    return;
                }
                int32_t exitCode = 0;
                if (proc->GetProcessState() == Process::ProcessState::kTerminated) {
                    exitCode = proc->IsTerminateNormally() ? 0 : -1;
                }
                info.AddProcess(proc->GetFqn(), proc->GetProcessStateString(), proc->GetPid(), exitCode);
            });
            return true;
        }};

    rds::Debugger::GetProcessInfoCallBack const getProcCb{
        [this](ara::core::StringView const &fgName, rds::ProcessInfo &info) -> bool {
            std::size_t const pos{fgName.find_last_of("/")};
            if (pos == ara::core::StringView::npos) {
                return false;
            }

            ara::core::StringView const name{fgName.substr(pos + 1U)};
            std::shared_ptr< Process > proc{_FindProcessByName(name)};
            if (proc == nullptr) {
                return false;
            }
            info.SetName(fgName);
            info.SetPid(proc->GetPid());
            info.SetState(proc->GetProcessStateString());
            int32_t exitCode = 0;
            if (proc->GetProcessState() == Process::ProcessState::kTerminated) {
                exitCode = proc->IsTerminateNormally() ? 0 : -1;
            }
            info.SetExitCode(exitCode);
            return true;
        }};

    rds::Debugger::TraceRequestCallback const traceRequestCb{[this](
                                                                 ara::core::Vector< ara::core::String > const &procList)
                                                                 -> void {
        /// Set debugging process
        LOGI() << "Debugger::TraceRequestCallback() -> ProcessList:";
        for (auto const &procName : procList) {  // PRQA S 2961
            std::size_t const pos{procName.find_last_of("/")};
            if (pos == ara::core::StringView::npos) {
                continue;
            }

            ara::core::String const name{procName.substr(pos + 1U)};
            std::shared_ptr< Process > const proc{_FindProcessByName(name)};
            if (proc == nullptr) {
                continue;
            }
            proc->SetDebugTraced(true);
            LOGI() << proc->GetFqn();
        }

        /// Only start the machine when received for the first time
        if (config::GetFunctionGroupStateOff() == _GetCurrentMachineState()) {
            /// Start the machine state
            ara::core::StringView const machineFgName{configManager_->GetMachineFunctionGroupFqn()};
            LOGI() << "bengin to Startup the MachineFG.";
            if (0 != _TransformFunctionGroupState(machineFgName, config::GetFunctionGroupStateStartup())) {
                /// @code{.isoft}
                /// @trace_id_sws=SWS_EM_02250 If the startup state is not configured for the function group MachineFG, execution management will stop the startup of the AUTOSAR adaptive platform
                /// @endcode
                LOGF() << "_TransformFunctionGroupState(" << machineFgName << ", "
                       << config::GetFunctionGroupStateStartup() << ")";
            }
        }
    }};

    rds::Debugger::TraceCancelCallback const traceCancelCb{
        [this](ara::core::Vector< ara::core::String > const &procList) -> void {
            /// Cancel debugging process
            LOGI() << "Debugger::TraceCancelCallback() -> ProcessList:";
            for (auto const &procName : procList) {  // PRQA S 2961
                std::size_t const pos{procName.find_last_of("/")};
                if (pos == ara::core::StringView::npos) {
                    continue;
                }
                ara::core::String const name{procName.substr(pos + 1U)};
                std::shared_ptr< Process > const proc{_FindProcessByName(name)};
                if (proc == nullptr) {
                    continue;
                }
                proc->SetDebugTraced(false);
                LOGI() << proc->GetFqn();
            }
        }};

    debugger_.OnGetFunctionGroupInfo(getFgCb);
    debugger_.OnGetProcessInfo(getProcCb);
    debugger_.OnTraceRequest(traceRequestCb);
    debugger_.OnTraceCancel(traceCancelCb);

    if (0 != debugger_.Open(gMainLoop_, debugArg_.GetServerPort())) {
        LOGF() << "debugger.Open()";
        return -1;
    }

    return 0;
}

/// @brief Destroy the IPC communication environment
/// @return 0 success; <0 failure
/// @code{.isoft}
/// @interface_level=none
/// @trace_id_ad=AD_EM_00124
/// @trace_id_dd=DD_EM_00658
/// @needwork = dda
/// @endcode
int32_t ExecutionManager::_DestroyIpcEnv() const noexcept
{
    int32_t hasError{0};

    // Close EMC
    if (0 != emsServer_.Close()) {
        LOGE() << "emsServer_.Close()";
        hasError = -1;
    }

    // Close UMS
    if (0 != umsServer_.Close()) {
        LOGE() << "umsServer_.Close()";
        hasError = -1;
    }

    // Close SMC
    if (0 != smsServer_.Close()) {
        LOGE() << "smsServer_.Close()";
        hasError = -1;
    }

    /// Delete the IPC of execution management
    if (0 > DestroyIPC(config::GetExecutionManagerIpcName())) {
        LOGE() << "DestroyIPC(" << config::GetExecutionManagerIpcName() << ")";
        hasError = -1;
    }

    /// Clean up the IPC environment
    if (0 != isoft::ipc::IPCBase::Destroy()) {
        LOGE() << "IPCBase::Destroy()";
        hasError = -1;
    }

    return hasError;
}

/// @brief Create and initialize POSIX signals
/// @return 0 success; <0 failure
/// @code{.isoft}
/// @interface_level=none
/// @trace_id_ad=AD_EM_00124
/// @trace_id_dd=DD_EM_00659
/// @needwork = dda
/// @endcode
int32_t ExecutionManager::_InitPosixSignal() noexcept
{
    // PRQA S ALL ++
    /// After forking a child process, register the signal handling function for the child process
    std::ignore = pthread_atfork(nullptr, nullptr, &ExecutionManager::AtForkHandler);

    // Initialize the signal handler
    nai_signal_init(&signalChild_);
    nai_signal_init(&signalInterrupt_);
    nai_signal_init(&signalTerminate_);
    std::ignore = nai_signal_set_cb(&signalChild_, &SignalHandler);
    std::ignore = nai_signal_set_cb(&signalInterrupt_, &SignalHandler);
    std::ignore = nai_signal_set_cb(&signalTerminate_, &SignalHandler);
    // PRQA S ALL --
    if (0 > nai_signal_open(&signalChild_, gMainLoop_->GetRawEvLoop(), SIGCHLD)) {
        LOGE() << "nai_signal_open(SIGCHLD): " << isoft::utils::StrError();
        return -1;
    }

    if (0 > nai_signal_open(&signalInterrupt_, gMainLoop_->GetRawEvLoop(), SIGINT)) {
        LOGE() << "nai_signal_open(SIGINT): " << isoft::utils::StrError();
        return -1;
    }

    if (0 > nai_signal_open(&signalTerminate_, gMainLoop_->GetRawEvLoop(), SIGTERM)) {
        LOGE() << "nai_signal_open(SIGTERM): " << isoft::utils::StrError();
        return -1;
    }
    return 0;
}

/// @brief Destroy and release POSIX signals
/// @return 0 success; <0 failure
/// @code{.isoft}
/// @interface_level=none
/// @trace_id_ad=AD_EM_00124
/// @trace_id_dd=DD_EM_00660
/// @needwork = dda
/// @endcode
int32_t ExecutionManager::_DestroyPosixSignal() noexcept
{
    struct sigaction sa
    {
    };
    sa.sa_handler = SIG_IGN;  // NOLINT
    sa.sa_flags   = SA_NOCLDWAIT;
    std::ignore   = sigaction(SIGCHLD, &sa, nullptr);
    std::ignore   = sigaction(SIGTERM, &sa, nullptr);

    std::ignore = nai_signal_close(&signalChild_);
    std::ignore = nai_signal_close(&signalInterrupt_);
    std::ignore = nai_signal_close(&signalTerminate_);

    return 0;
}

/// @brief Build the system environment
/// @return 0 success; <0 failure
/// @code{.isoft}
/// @interface_level=none
/// @trace_id_ad=AD_EM_00124
/// @trace_id_dd=DD_EM_00653
/// @needwork = dda
/// @endcode
int32_t ExecutionManager::_SetupSysEnv() noexcept
{
    /// Set the fsh environment variable because the underlying ipcbase library needs to use fsh
    spFsh_ = isoft::ara_fsh::Platform::CreatePlatform(araSysroot_.c_str(), araConfig_.c_str());
    if (nullptr == spFsh_) {
        std::cerr << "isoft::ara_fsh::Platform::CreatePlatform(" << araSysroot_ << "," << araConfig_ << ") error !!!"
                  << std::endl;
        return -1;
    }

    std::vector< std::string > fshEnv;
    std::ignore = spFsh_->ExportEnvironment(fshEnv);
    for (auto const &env : fshEnv) {  // PRQA S 2961
        std::size_t const pos{env.find("=")};
        if (pos == std::string::npos) {
            continue;
        }
        ara::core::String const key{env.substr(0U, pos)};
        ara::core::String const value{env.substr(pos + 1U)};
        if (0 != setenv(key.c_str(), value.c_str(), 1)) {  // NOLINT
            std::cerr << "Error on setenv(" << key << "=" << value << ")" << std::endl;
            return -1;
        }
    }

    return 0;
}

/// @brief Create and initialize resource groups
/// @return 0 success; <0 failure
/// @code{.isoft}
/// @interface_level=none
/// @trace_id_ad=AD_EM_00124
/// @trace_id_dd=DD_EM_00655
/// @needwork = dda
/// @endcode
int32_t ExecutionManager::_InitResourceGroup() noexcept
{
    ara::core::Vector< config::ResourceGroup > const &rgConfs{
        configManager_->GetMachineManifest()->GetResourceGroups()};
    for (auto const &rgc : rgConfs) {  // PRQA S 2961
        ara::core::String const tmpRgName{rgc.GetName() + "_" + spFsh_->GetSysrootMd5Str().c_str()};
        std::shared_ptr< ResourceGroup > rg{
            std::make_shared< ResourceGroup >(tmpRgName.c_str(), rgc.GetCpuUsage(), rgc.GetMemUsage())};
        if (nullptr == rg) {
            LOGF() << "ResourceGroup {" << rgc.GetName() << "}";
            return -1;
        }

        int32_t const r{rg->Open()};
        if (0 != r) {
            LOGF() << "rg {" << rgc.GetName() << "}.Open(): " << r;
            return -1;
        }

        std::ignore = resourceGroups_.emplace(std::make_pair(rgc.GetName(), rg));
    }

    return 0;
}

/// @brief Destroy and release resource groups
/// @return 0 success; <0 failure
/// @code{.isoft}
/// @interface_level=none
/// @trace_id_ad=AD_EM_00124
/// @trace_id_dd=DD_EM_00656
/// @needwork = dda
/// @endcode
int32_t ExecutionManager::_DestroyResourceGroup() noexcept
{
    int32_t err{0};
    for (auto &it : resourceGroups_) {
        std::shared_ptr< ResourceGroup > &rg{it.second};
        std::int32_t const r{rg->Close()};
        if (0 != r) {
            LOGE() << "rg {" << rg->GetName() << "}.Close(): " << r;
            err = -1;
        }
    }
    resourceGroups_.clear();
    return err;
}

/// @brief Create all process objects based on the execution manifest and assign them to the corresponding function groups
/// @param executionManifests Execution manifest
/// @return 0 success; <0 failure
/// @code{.isoft}
/// @interface_level=none
/// @trace_id_ad=AD_EM_00124
/// @trace_id_dd=DD_EM_00661
/// @needwork = dda
/// @endcode
int32_t ExecutionManager::_LoadProcesses(
    std::list< std::shared_ptr< config::ExecutionManifest > > const &executionManifests) noexcept
{
    /// List of function groups that need to be updated
    isoft::core::Set< std::shared_ptr< FunctionGroup > > fgList;

    /// Create processes based on ExecutionManifest and add them to the corresponding function groups
    for (auto const &manifest : executionManifests) {
        if (manifest->HasConfigError()) {
            LOGE() << "_LoadProcesses(): Process {" << manifest->GetProcName() << "} has config error !!!";
            return -1;
        }

        if (manifest->GetStateDependentConfigs().empty()) {
            LOGW() << "_LoadProcesses(): No StateDependentConfig found for process {" << manifest->GetProcName() << "}";
            continue;
        }

        std::shared_ptr< Process > proc{std::make_shared< Process >(manifest, gMainLoop_, spFsh_)};
        if (nullptr == proc) {
            LOGF() << "Process(" << manifest->GetProcName() << ")";
            return -1;
        }

        std::shared_ptr< FunctionGroup > fgp{_FindFunctionGroupByName(manifest->GetFunctionGroupName())};
        if (fgp == nullptr) {
            LOGF() << "LoadProcess(): Can't find the function group {" << manifest->GetFunctionGroupName() << "}";
            return -1;
        }
        /// Insert into the list of function groups that need to be updated
        std::ignore = fgList.emplace(fgp);

        /// Add the process to the function group
        if (0 != fgp->AddProcess(proc)) {
            LOGF() << "AddProcess(" << proc->GetName() << ")";
            return -1;
        }

        /// Add the process to the process finder
        procFinder_.AddProcess(ara::core::String{proc->GetFqn()}, proc.get());

        /// Register the startup callback function
        proc->OnStartup([this](Process *const p) noexcept -> void { _ProcessStartupedHandler(p); });

        /// Register the process state change callback function
        proc->OnStateChanged([this](Process const *const p) -> void {
            /// Send debug tracing ready notification
            if (debugger_.IsServiceOn()) {
                rds::ProcessInfo info;
                info.SetName(p->GetFqn());
                info.SetPid(p->GetPid());
                info.SetState(p->GetProcessStateString());
                int32_t exitCode = 0;
                if (p->GetProcessState() == Process::ProcessState::kTerminated) {
                    exitCode = p->IsTerminateNormally() ? 0 : -1;
                }
                info.SetExitCode(exitCode);
                LOGD() << "Notify the Information of Process:"
                       << "Name(" << info.GetName() << "),"
                       << "Pid(" << info.GetPid() << "),"
                       << "State(" << info.GetState() << "),"
                       << "ExitCode(" << info.GetExitCode() << ")";
                debugger_.NotifyProcessStateChanged(info);
            }
        });
    }  ///< for

    /// Scan the process list and establish mappings based on dependencies. Establishing the process dependency mapping in advance can improve efficiency at runtime
    /// Based on function groups, because processes cannot cross function groups
    for (auto const &fg : fgList) {
        if (0 != fg->GenDependTree()) {
            LOGF() << "fg {" << fg->GetName() << "}->GenDependTree()";
            return -1;
        }
    }

    return 0;
}

/// @brief Create function group instances based on function group information
/// @return 0 success; <0 failure
/// @code{.isoft}
/// @interface_level=none
/// @trace_id_ad=AD_EM_00124
/// @trace_id_dd=DD_EM_00662
/// @needwork = dda
/// @endcode
int32_t ExecutionManager::_CreateFunctionGroup() noexcept
{
    /// Create function groups based on MachineManifest
    for (auto const &fg : configManager_->GetFunctionGroups()) {  // PRQA S 2961
        std::shared_ptr< FunctionGroup > spFg{std::make_shared< FunctionGroup >(fg.GetName(), fg.GetStates())};
        if (spFg == nullptr) {
            LOGF() << "LoadProcesses::FunctionGroup(" << fg.GetName() << ")";
            return -1;
        }
        spFg->OnStateChanged(
            [this](FunctionGroup const *const pFg) noexcept -> void { _FunctionGroupStateChangedHandler(pFg); });
        std::ignore = functionGroups_.emplace(std::make_pair(fg.GetName(), spFg));
    }

    return 0;
}

/// @brief Load platform configuration and create platform-related processes
/// @return 0 success; <0 failure
/// @code{.isoft}
/// @interface_level=none
/// @trace_id_ad=AD_EM_00124
/// @trace_id_dd=DD_EM_00663
/// @needwork = dda
/// @endcode
int32_t ExecutionManager::_LoadPlatformProcess() noexcept
{
    /// Load platform processes
    for (auto const &swcl : configManager_->GetPlatformSwclManifest()->GetSoftwareClusters()) {  // PRQA S 2961
        if (swcl.HasConfigError()) {
            return -1;
        }

        if (0 != _LoadProcesses(configManager_->GetExecutionManifestList(swcl.GetName()))) {
            return -1;
        }
    }

    return 0;
}

/// @brief Load user configuration and create user-related processes
/// @return 0 success; <0 failure
/// @code{.isoft}
/// @interface_level=none
/// @trace_id_ad=AD_EM_00124
/// @trace_id_dd=DD_EM_00664
/// @needwork = dda
/// @endcode
int32_t ExecutionManager::_LoadUserProcess() noexcept
{
    /// Load user process execution manifests
    std::shared_ptr< config::SoftwareClusterManifest > userSwclManifest{configManager_->GetUserSwclManifest()};
    if (nullptr == userSwclManifest) {
        LOGE() << "No application swcl manifest !!!";
        return -1;
    }

    int32_t ret{0};
    for (auto const &swcl : userSwclManifest->GetSoftwareClusters()) {  // PRQA S 2961
        if (swcl.HasConfigError()) {
            LOGW() << "Swcl (" << swcl.GetName() << ") is wrongly configured, not load its process !!!";
            ret = -1;
            continue;
        }

        if (0 != _LoadProcesses(configManager_->GetExecutionManifestList(swcl.GetName()))) {
            LOGW() << "Load the process of swcl (" << swcl.GetName() << ") failed !!!";
            ret = -1;
            continue;
        }
    }

    return ret;
}

/// @brief Initialize function groups and processes
/// @return 0 success; <0 failure
/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
int32_t ExecutionManager::_InitFgAndProcess() noexcept
{
    /// Load the system configuration file
    configManager_ = ConfigManager::CreateInstance();
    if (nullptr == configManager_) {
        LOGF() << "ConfigManager::CreateInstance()";
        return -1;
    }

    if (0 != _CreateFunctionGroup()) {
        LOGF() << "_CreateFunctionGroup() failed !!!";
        return -1;
    }

    /// Load platform processes
    if (0 != _LoadPlatformProcess()) {
        LOGF() << "_LoadPlatformProcess() failed !!!";
        return -1;
    }

    /// Load user processes
    if (0 != _LoadUserProcess()) {
        LOGW() << "_LoadUserProcess() failed !!!";
    }

    return 0;
}

/// @brief Initialization operations. Including initializing cgroup/filesystem, reading machine_manifest/loading executable program configuration, etc.
/// @return 0 success; <0 failure
/// @code{.isoft}
/// @interface_level=none
/// @trace_id_ad=AD_EM_00124
/// @trace_id_dd=DD_EM_00649
/// @needwork = dda
/// @endcode
int32_t ExecutionManager::_Init() noexcept  // NOLINT
{
    if (0 != _SetupSysEnv()) {
        std::cerr << "_SetupSysEnv() failed !!!" << std::endl;
        return -1;
    }

    if (0 != InitIPC()) {
        std::cerr << "InitIPC() failed !!!" << std::endl;
        return -1;
    }

    if (0 > CreateIPC(config::GetExecutionManagerIpcName())) {
        std::ostringstream oss;
        oss << "CreateIPC(" << config::GetExecutionManagerIpcName() << "): ";
        perror(oss.str().c_str());
        return -1;
    }

    gMainLoop_ = isoft::naicpp::GlobalGeneralEvLoop::Get();
    if (nullptr == gMainLoop_) {
        LOGF() << "nullptr == gMainLoop_";
        return -1;
    }

    /// Initialize logging
    ara::core::Result< void > const r{ara::log::internal::Initialize()};
    if (true != r.HasValue()) {
        std::cerr << "ara::log::internal::Initialize()" << std::endl;
        return -1;
    }

    _ShowSdkReleaseInfo();
    ExecTrustPlatform::Initialize();

    /// Initialize Posix signals
    if (0 != _InitPosixSignal()) {
        LOGF() << "_InitPosixSignal()";
        return -1;
    }

    /// Initialize the IPC environment
    if (0 != _InitServer()) {
        LOGF() << "_InitServer()";
        return -1;
    }

    if (0 != _InitFgAndProcess()) {
        LOGF() << "_InitFgAndProcess()";
        return -1;
    }

    /// Initialize the process finder
    if (0 != procFinder_.Create()) {
        LOGF() << "procFinder_.Create()";
        return -1;
    }

    if (!debugArg_.IsResourceGroupDisabled()) {
        /// Initialize resource groups
        if (0 != _InitResourceGroup()) {
            LOGF() << "_InitResourceGroup() error !!!";
            return -1;
        }
        LOGI() << "_InitResourceGroup() successfully !!!";
    }

    if (0 != _InitDebugger()) {
        LOGF() << "_InitDebugger() error !!!";
        return -1;
    }

    return 0;
}

/// @brief Terminate all running processes
/// @code{.isoft}
/// @interface_level=none
/// @trace_id_ad=AD_EM_00124
/// @trace_id_dd=DD_EM_00650
/// @needwork = dda
/// @endcode
void ExecutionManager::_TerminateAllRunningProcess() noexcept
{
    ara::core::Vector< Process * > killedProcs;
    for (auto &fg : functionGroups_) {
        fg.second->TraversalProcess([&killedProcs](std::shared_ptr< Process > const &proc) {
            bool const nonZeroPid{0 == proc->GetPid()};
            if ((nullptr == proc) || nonZeroPid) {
                return;
            }
            if (0 != proc->Kill(Process::TerminateCause::kRequest)) {
                LOGE() << "Destroy::Kill(" << proc->GetName() << ") error !!!";
            } else {
                killedProcs.emplace_back(proc.get());
            }
        });
    }

    for (auto &proc : killedProcs) {  // PRQA S 2961
        nai_proc_t h{proc->GetHandle()};
        std::ignore = nai_proc_wait(&h, nullptr, 0);
        LOGD() << "Process {" << proc->GetName() << "} is killed by force !!!";
        if (h.pid == 0) {
        }
    }

    procFinder_.ClearProcess();
    /// Close and destroy the process finder
    if (0 != procFinder_.Destroy()) {
        LOGW() << "procFinder_.Destroy()";
    }
}

/// @brief Resource destroy operation. The inverse operation of Init()
/// @code{.isoft}
/// @interface_level=none
/// @trace_id_ad=AD_EM_00124
/// @trace_id_dd=DD_EM_00651
/// @needwork = dda
/// @endcode
void ExecutionManager::_Destroy() noexcept
{
    /// Close the debugger service
    if (debugArg_.IsServerOn()) {
        std::ignore = debugger_.Close();
    }

    ExecTrustPlatform::DeInitialize();

    /// Close all processes
    _TerminateAllRunningProcess();

    LOGD() << "Release the resource of emd !!!";
}

/// @brief Post-execution cleanup operation
/// @return 0 success; <0 failure
/// @code{.isoft}
/// @interface_level=none
/// @trace_id_ad=AD_EM_00124
/// @trace_id_dd=DD_EM_00652
/// @needwork = dda
/// @endcode
int32_t ExecutionManager::_PostRelease() noexcept
{
    std::int32_t ret{0};

    if (0 != _DestroyIpcEnv()) {
        LOGE() << "_DestroyIpcEnv()";
        ret = -1;
    }

    std::ignore = _DestroyPosixSignal();

    if (0 != _DestroyResourceGroup()) {
        LOGE() << "_DestroyResourceGroup()";
        ret = -1;
    }

    LOGI() << "ExecutionManager is Terminated !!!";
    return ret;
}

/// @brief Switch function group state
/// @param fgName Function group name
/// @param stateName State name
/// @return 0 success; <0 failure
/// @code{.isoft}
/// @interface_level=none
/// @trace_id_ad=AD_EM_00124
/// @trace_id_dd=DD_EM_00666
/// @needwork = dda
/// @endcode
int32_t ExecutionManager::_TransformFunctionGroupState(ara::core::StringView const &fgName,
                                                       ara::core::StringView const &stateName) const noexcept
{
    LOGI() << "_TransformFunctionGroupState(" << fgName << "," << stateName << ") Begain";
    std::shared_ptr< FunctionGroup > fg{_FindFunctionGroupByName(fgName)};
    if (nullptr == fg) {
        LOGE() << "nullptr == _FindFunctionGroupByName(" << fgName << ")";
        return -1;
    }

    if (true != fg->HasState(ara::core::String{stateName})) {
        LOGE() << "true != fg {" << fgName << "}->HasState(" << stateName << ")";
        return -1;
    }

    return fg->StartTransformState(stateName);
}

/// @brief Get the current state of the function group
/// @param fgName Reference to function group name
/// @return Empty string failure; non-empty string success
/// @code{.isoft}
/// @interface_level=none
/// @trace_id_ad=AD_EM_00124
/// @trace_id_dd=DD_EM_00667
/// @needwork = dda
/// @endcode
ara::core::StringView const ExecutionManager::_GetCurrentFunctionGroupState(
    ara::core::StringView const &fgName) const noexcept
{
    std::shared_ptr< FunctionGroup > fgp{_FindFunctionGroupByName(fgName)};
    if (nullptr == fgp) {
        return "";
    }
    return fgp->GetCurrentState();
}

/// @brief Process creation callback function, called after the process is successfully created, used to perform some operations before exec
/// @param proc Process handle
/// @code{.isoft}
/// @interface_level=none
/// @trace_id_ad=AD_EM_00124
/// @trace_id_dd=DD_EM_00673
/// @needwork = dda
/// @endcode
void ExecutionManager::_ProcessStartupedHandler(Process *const proc) noexcept
{
    if (nullptr == proc) {
        return;
    }

    /// @code{.isoft}
    /// @trace_id_sws=SWS_EM_02400 EM should associate the PID with the process for IAM
    /// @endcode
    procFinder_.SetPid(proc, static_cast< uint32_t >(proc->GetPid()));

    if (true != debugArg_.IsResourceGroupDisabled()) {
        /// @code{.isoft}
        /// @trace_id_sws=SWS_EM_02106 Execution management will configure the process according to its ResourceGroup membership relationship
        /// @endcode
        ara::core::StringView const rgName{proc->GetCurrentResourceGroupName()};
        std::shared_ptr< ResourceGroup > const rg{_FindResourceGroupByName(ara::core::String{rgName})};
        if (nullptr == rg) {
            LOGE() << "NULL == FindResourceGroupByName(" << rgName << ")";
            std::ignore = gMainLoop_->Exec(
                [proc]() -> void { std::ignore = proc->Kill(Process::TerminateCause::kInitFailure); });
            return;
        }
        if (0 != rg->AttachTask(proc->GetPid())) {
            LOGE() << "rg {" << rgName << "}->AttachTask(" << proc->GetPid() << ")";
            std::ignore = gMainLoop_->Exec(
                [proc]() -> void { std::ignore = proc->Kill(Process::TerminateCause::kInitFailure); });
            return;
        }
    }

    /// Send debug tracing ready notification
    if (proc->IsDebugTraced()) {
        rds::ProcessInfo info;
        info.SetName(proc->GetFqn());
        info.SetPid(proc->GetPid());
        info.SetState(proc->GetProcessStateString());
        int32_t exitCode = 0;
        if (proc->GetProcessState() == Process::ProcessState::kTerminated) {
            exitCode = proc->IsTerminateNormally() ? 0 : -1;
        }
        info.SetExitCode(exitCode);
        debugger_.NotifyProcessTraceReady(info);
    }
}

/// @brief Function group state switch callback function, executed whenever the function group state changes
/// @param fg Function group handle
/// @code{.isoft}
/// @interface_level=none
/// @trace_id_ad=AD_EM_00124
/// @trace_id_dd=DD_EM_00674
/// @needwork = dda
/// @endcode
void ExecutionManager::_FunctionGroupStateChangedHandler(FunctionGroup const *const fg) noexcept
{
    if (nullptr == fg) {
        return;
    }

    /// Notify the debugger host
    if (debugger_.IsServiceOn()) {
        rds::FunctionGroupInfo info;
        info.SetName(fg->GetName());
        info.SetState(fg->GetCurrentState());
        fg->TraversalProcess([&info](std::shared_ptr< Process > const &proc) noexcept -> void {
            if (proc == nullptr) {
                return;
            }
            int32_t exitCode = 0;
            if (proc->GetProcessState() == Process::ProcessState::kTerminated) {
                exitCode = proc->IsTerminateNormally() ? 0 : -1;
            }
            info.AddProcess(proc->GetFqn(), proc->GetProcessStateString(), proc->GetPid(), exitCode);
        });
        LOGD() << "Notify FunctionGroup Info {" << fg->GetName() << "," << fg->GetCurrentState() << "}";
        debugger_.NotifyFunctionGroupStateChanged(info);
    }

    ara::core::StringView const machineFgName{configManager_->GetMachineFunctionGroupFqn()};
    if (fg->GetName() == machineFgName) {
        if (config::GetFunctionGroupStateOff() == fg->GetCurrentState()) {
            LOGI() << "the MachineFG state is turn Off, now do exit.";
            std::ignore = Stop();
            return;
        }

        if (config::GetFunctionGroupStateUndefined() == fg->GetCurrentState()) {
            bool isStateManageRunning{false};
            fg->TraversalProcess([&isStateManageRunning](std::shared_ptr< Process > const &proc) noexcept -> void {
                /// If state management has not started, it is impossible to resume machineFG, so MachineFG needs to be switched to the Off state, and EMD exits
                if (proc->IsStateManager()) {
                    if (proc->IsRunning()) {
                        isStateManageRunning = true;
                    }
                }
            });

            if (isStateManageRunning) {
                LOGW() << "MachineFG fell into Undefined state, SMD will deal with it further !!!";
            } else {
                LOGE() << "MachineFG fell into Undefined state, and SMD is not running."
                       << "There is no way to recovery, so shutdown the machine !!!";
                std::ignore = _TransformFunctionGroupState(machineFgName, config::GetFunctionGroupStateOff());
                return;
            }
        }
    }

    /// @code{.isoft}
    /// @trace_id_sws=SWS_EM_01067 After the function group transition is completed, the function group state should be set and notify SM
    /// @trace_id_sws=SWS_EM_02241 After completing the initial (self-starting) machine state transition to the startup state, execution management should notify state management that the startup state of the machine state has been reached
    /// @endcode
    ara::exec::ExecErrc const te{fg->GetTransitionError()};
    uint32_t const ee{fg->GetExecutionError()};

    if (0 != smsServer_.ResponseSetFunctionGroupState(fg->GetName(), te, ee)) {
        LOGW() << "smsServer_.ResponseSetFunctionGroupState(" << fg->GetName() << "): No SetFunctionGroup Request !!!";
    }

    if (config::GetFunctionGroupStateUndefined() == fg->GetCurrentState()) {
        LOGE() << "FunctionGroup {" << fg->GetName() << "} fell into Undefined state";
        if (0 != smsServer_.NotifyUndefineFunctionGroupState(fg->GetName(), te, ee)) {
            LOGE() << "smsServer_.NotifyUndefineFunctionGroupState(" << fg->GetName() << "): SMD is not running !!!";
        }
    } else {
        LOGI() << "FunctionGroup {" << fg->GetName() << "} change to" << fg->GetCurrentState() << "state";
    }
}

/// @brief  Execution client state report handler function, asynchronously called when a report is received
/// @param state Received state
/// @param pid From which PID
/// @code{.isoft}
/// @interface_level=none
/// @trace_id_ad=AD_EM_00124
/// @trace_id_dd=DD_EM_00675
/// @needwork = dda
/// @endcode
void ExecutionManager::_ExecutionClientReportHandler(ems::Message::State const state, uint32_t const pid) noexcept
{
    Process *const proc{_FindProcessByPid(pid)};
    if (nullptr == proc) {
        LOGE() << "Receive Reporting, but Process(" << pid << ") is not started by EMD !!!";
        return;
    }

    // Received start completion event
    if (ems::Message::State::kRunning != state) {
        LOGE() << proc->GetName() << "{" << proc->GetPid() << "} reports wrong state(" << static_cast< uint16_t >(state)
               << ").";
        return;
    }

    LOGI() << proc->GetName() << "{" << proc->GetPid() << "} Report Running";
    /// @code{.isoft}
    /// @trace_id_sws=SWS_EM_02243 Report running, but the current state is not the initialization state, should return kInvalidTransition
    /// @endcode
    if (Process::ExecutionState::kInitializing != proc->GetExecutionState()) {
        LOGE() << proc->GetName() << "{" << proc->GetPid() << "} Report Running, but currrent state is not Starting.";
        std::ignore = proc->Kill(Process::TerminateCause::kInvalidTransition);
    } else {
        proc->TriggerEvent(Process::Event::kRunning);
    }

    return;
}

/// @brief  State client state handler function, asynchronously called when a GetFunctionState event is received
/// @param fgName Obtained function group name
/// @param fgState Function group state
/// @code{.isoft}
/// @interface_level=none
/// @trace_id_ad=AD_EM_00124
/// @trace_id_dd=DD_EM_00676
/// @needwork = dda
/// @endcode
void ExecutionManager::_StateClientGetFunctionGroupStateHandler(ara::core::StringView const &fgName,
                                                                ara::core::StringView const &fgState) noexcept
{
    std::ignore = (fgState);

    ara::core::StringView state;
    std::shared_ptr< FunctionGroup > fg;
    ara::exec::ExecErrc te;
    uint32_t ee{0U};
    /// If you want to get the MachineFG state, then get the machine function group state.
    /// Special short name handling for the machine function group
    if (config::GetMachineFunctionGroupName() == fgName) {
        fg = _FindFunctionGroupByName(configManager_->GetMachineFunctionGroupFqn());
    } else {
        fg = _FindFunctionGroupByName(fgName);
    }
    if (nullptr == fg) {
        te = ara::exec::ExecErrc::kGeneralError;
        ee = 1U;
    } else {
        state = fg->GetCurrentState();
        te    = fg->GetTransitionError();
        ee    = fg->GetExecutionError();
    }

    LOGV() << "_StateClientGetFunctionGroupStateHandler(" << fgName << "), Result: " << state;

    if (0 != smsServer_.ResponseGetFunctionGroupState(fgName, state, te, ee)) {
        LOGE() << "smsServer_.ResponseGetFunctionGroupState(" << fgName << "," << state << ")";
    }
}

/// @brief  State client state transition cancel handler function, asynchronously called when a cancel event is received
/// @param fgName Function group name
/// @param fgState Function group state
/// @code{.isoft}
/// @interface_level=none
/// @trace_id_ad=AD_EM_00124
/// @trace_id_dd=DD_EM_00678
/// @needwork = dda
/// @endcode
void ExecutionManager::_StateClientCancelFunctionGroupStateTransitionHandler(
    ara::core::StringView const &fgName, ara::core::StringView const &fgState) noexcept
{
    std::ignore = fgState;

    ara::exec::ExecErrc te;
    uint32_t const ee{0U};
    std::shared_ptr< FunctionGroup > fg{_FindFunctionGroupByName(fgName)};
    if (nullptr == fg) {
        te = ara::exec::ExecErrc::kInvalidArguments;
    } else {
        LOGI() << "FunctionGroup {" << fgName << "} cancel the transition to" << fgState;
        fg->StopTransformState();
        te = ara::exec::ExecErrc::kCancelled;
    }
    if (0 != smsServer_.ResponseSetFunctionGroupState(fgName, te, ee)) {
        LOGE() << "smsServer_.ResponseSetFunctionGroupState(" << fgName << ")";
    }
}

/// @brief  State client state handler function, asynchronously called when a SetFunctionState event is received
/// @param fgName Function group name
/// @param fgState Function group state
/// @code{.isoft}
/// @interface_level=none
/// @trace_id_ad=AD_EM_00124
/// @trace_id_dd=DD_EM_00677
/// @needwork = dda
/// @endcode
void ExecutionManager::_StateClientSetFunctionGroupStateHandler(ara::core::StringView const &fgName,
                                                                ara::core::StringView const &fgState) noexcept
{
    LOGV() << "_StateClientSetFunctionGroupStateHandler(" << fgName << "," << fgState << ") called";

    /// Instead of GOTO
    ara::exec::ExecErrc te;
    while (true) {
        std::shared_ptr< FunctionGroup > const fg{_FindFunctionGroupByName(fgName)};
        if (fg == nullptr) {
            LOGW() << "_StateClientSetFunctionGroupStateHandler(" << fgName << "," << fgState
                   << "): invalid FunctionGroup";
            te = ara::exec::ExecErrc::kInvalidArguments;
            break;
        }

        if (!(fg->HasState(ara::core::String{fgState}))) {
            LOGW() << "_StateClientSetFunctionGroupStateHandler(" << fgName << "," << fgState << "): invalid State";
            te = ara::exec::ExecErrc::kInvalidArguments;
            break;
        }

        /// If it is already the state to be transitioned
        if (fg->GetCurrentState() == fgState) {
            te = ara::exec::ExecErrc::kAlreadyInState;
            LOGW() << "_StateClientSetFunctionGroupStateHandler(" << fgName << "," << fgState << "): AlreadyInState";
            break;
        }

        /// If the function group is currently undergoing a state transition, reject any state switching request. This applies when the machine state has not yet transitioned to the Startup state and a request to switch the machine function group is received
        /// For any other function group, the ongoing transition can be canceled, but not for the machine function group. Because a machine function group that has not entered the Startup state cannot meet normal requirements
        /// NOTE: When a function group receives a new state transition request during an ongoing transition, the underlying SMC handles it by first canceling the existing transition and then switching to the new state
        /// The cancellation operation is triggered by the StateClientCancellFunctionGroupStateTransitionHandler callback function
        ara::core::StringView const transingState{fg->GetTransingState()};
        /// If it is not empty, it indicates a transition is in progress, return an error immediately
        if (!transingState.empty()) {
            LOGW() << "_StateClientSetFunctionGroupStateHandler(" << fgName << "," << fgState << "): TransingState {"
                   << fg->GetCurrentState() << "}";
            te = ara::exec::ExecErrc::kGeneralError;
            break;
        }

        /// If the transition starts successfully, return directly; the result of the transition will be answered asynchronously to SM later
        if (0 == _TransformFunctionGroupState(fgName, fgState)) {
            return;
        }
        /// Transition start failed, enter error handling
        te = ara::exec::ExecErrc::kGeneralError;
        LOGE() << "_StateClientSetFunctionGroupStateHandler(" << fgName << "," << fgState
               << "): ERROR on _TransformFunctionGroupState()";

        /// Instead of GOTO
        break;
    }

    // Here it is because the function group does not exist or the state does not exist, resulting in an obvious error, then respond immediately
    std::uint32_t const ee{1U};  ///< At this point, it is not known which process caused the error, so use the default error code {1}
    if (0 != smsServer_.ResponseSetFunctionGroupState(fgName, te, ee)) {
        LOGE() << "smsServer_.ResponseSetFunctionGroupState(" << fgName << ")";
    }
}

/// @brief  Update client state event handler function, asynchronously called when a request to get the software cluster list name is received
/// @code{.isoft}
/// @interface_level=none
/// @trace_id_ad=AD_EM_00124
/// @trace_id_dd=DD_EM_00679
/// @needwork = dda
/// @endcode
void ExecutionManager::_UpdateClientGetUserSwclManifestHandler() noexcept
{
    LOGV() << "_UpdateClientGetUserSwclManifestHandler() called";

    ara::core::StringView const path{configManager_->GetUserSwclManifest()->GetManifestPath()};
    ara::core::StringView name;
    std::size_t const pos{(path.rfind("/"))};
    if (pos != ara::core::StringView::npos) {
        name = path.substr(pos + 1U);
    }

    if (0 != umsServer_.ResponseGetUserSwclManifest(name)) {
        LOGE() << "smsServer_.ResponseGetUserSwclManifest(" << name << ")";
        return;
    }

    return;
}

/// @brief  Update client state event handler function, asynchronously called when a request to reparse the software cluster list is received
/// @param invalidSwcls Invalid software cluster list
/// @param validSwcls   Effective software cluster list
/// @code{.isoft}
/// @interface_level=none
/// @trace_id_ad=AD_EM_00124
/// @trace_id_dd=DD_EM_00680
/// @needwork = dda
/// @endcode
void ExecutionManager::_UpdateClientUpdateUserSwclHandler(core::Vector< ums::SwclInfo > const &invalidSwcls,
                                                          core::Vector< ums::SwclInfo > const &validSwcls) noexcept
{
    LOGV() << "UCM request update user swcls !!!";

    bool isSuccess{true};
    while (true) {
        std::shared_ptr< ConfigManager > const spCfgManager{_UpdateClientReloadUserSwcls(invalidSwcls, validSwcls)};
        if (spCfgManager == nullptr) {
            isSuccess = false;
            break;
        }

        if (_UpdateClientReloadUserProcess(spCfgManager) < 0) {
            isSuccess = false;
            break;
        }

        /// Update the process finder
        if (0 != procFinder_.Update< Process >()) {
            LOGE() << "procFinder_.Update() error !!!";
            isSuccess = false;
            break;
        }
        break;
    }

    if (0 != umsServer_.ResponseUpdateUserSwcls(isSuccess)) {
        LOGE() << "umsServer_.ResponseUpdateUserSwcls(" << isSuccess << ")";
        return;
    }
    return;
}

/// @brief  Update client reload user software cluster information
/// @param invalidSwcls Invalid software cluster list
/// @param validSwcls   Effective software cluster list
/// @return 0 success; <0 failure
/// @code{.isoft}
/// @interface_level=none
/// @trace_id_ad=AD_EM_00124
/// @trace_id_dd=DD_EM_00681
/// @needwork = dda
/// @endcode
std::shared_ptr< ConfigManager > ExecutionManager::_UpdateClientReloadUserSwcls(
    core::Vector< ums::SwclInfo > const &invalidSwcls, core::Vector< ums::SwclInfo > const &validSwcls) noexcept
{
    std::shared_ptr< ConfigManager > spConfigManager{std::make_shared< ConfigManager >()};
    if (spConfigManager == nullptr) {
        LOGE() << "_UpdateClientReloadUserSwcls(): Create Config Manager failed !!!";
        return {nullptr};
    }

    if (0 != spConfigManager->LoadMachineManifest()) {
        LOGE() << "_UpdateClientReloadUserSwcls(): Load MachineManifest failed !!!";
        return {nullptr};
    }

    if (0 != spConfigManager->LoadUserSwclList()) {
        LOGE() << "_UpdateClientReloadUserSwcls(): Load User Swcl List failed !!!";
        return {nullptr};
    }

    for (auto const &swclInfo : validSwcls) {  // PRQA S 2961
        if (spConfigManager->LoadSpecifiedUserSwcl(swclInfo.swclName) < 0) {
            LOGE() << "_UpdateClientReloadUserSwcls(): Load User Swcl {" << swclInfo.swclName << "} failed !!!";
            return {nullptr};
        }
    }

    std::shared_ptr< config::SoftwareClusterManifest > swclManifest{configManager_->GetUserSwclManifest()};
    for (auto const &invalidSwcl : invalidSwcls) {  // PRQA S 2961
        config::SoftwareCluster const *const swcl{swclManifest->FindSwclbyName(invalidSwcl.swclName)};
        if (swcl == nullptr) {
            LOGE() << "_UpdateClientReloadUserSwcls(): No specific swcl {" << invalidSwcl.swclName << "} found !!!";
            return {nullptr};
        }

        if (_UpdateClientClearSoftwareCluster(*swcl) < 0) {
            LOGE() << "_UpdateClientReloadUserSwcls(): Clear swcl {" << invalidSwcl.swclName << "} failed !!!";
            return {nullptr};
        }

        swclManifest->DeleteSwclbyName(invalidSwcl.swclName);
    }

    return spConfigManager;
}

/// @brief Reload user processes from the effective software manifest
/// @param spConfigManager Newly effective configuration management instance
/// @return 0 success; <0 failure
/// @code{.isoft}
/// @interface_level=none
/// @trace_id_ad=AD_EM_00124
/// @trace_id_dd=DD_EM_00682
/// @needwork = dda
/// @endcode
int32_t ExecutionManager::_UpdateClientReloadUserProcess(
    std::shared_ptr< ConfigManager > const &spConfigManager) noexcept
{
    /// spConfigManager only loads the function groups contained in validSwcl, so add them directly
    configManager_->AddFunctionGroups(spConfigManager->GetFunctionGroups());
    for (auto const &fg : spConfigManager->GetFunctionGroups()) {  // PRQA S 2961
        std::shared_ptr< FunctionGroup > spFg{std::make_shared< FunctionGroup >(fg.GetName(), fg.GetStates())};
        if (spFg == nullptr) {
            LOGE() << "_UpdateClientReloadUserProcess(): Create FunctionGroup(" << fg.GetName() << ") failed !!!";
            return -1;
        }
        spFg->OnStateChanged(
            [this](FunctionGroup const *const pFg) noexcept -> void { _FunctionGroupStateChangedHandler(pFg); });
        std::ignore = functionGroups_.emplace(std::make_pair(fg.GetName(), spFg));
    }

    std::shared_ptr< config::SoftwareClusterManifest > swclManifest{configManager_->GetUserSwclManifest()};
    for (auto const &swcl : spConfigManager->GetUserSwclManifest()->GetSoftwareClusters()) {  // PRQA S 2961
        /// Only the effective software cluster loads the execution manifest, so only reload the processes contained in the effective software cluster
        std::list< std::shared_ptr< config::ExecutionManifest > > const &execManifestList{
            spConfigManager->GetExecutionManifestList(swcl.GetName())};
        if (!execManifestList.empty()) {
            swclManifest->AddSoftwareCluster(swcl);
            if (0 != _LoadProcesses(execManifestList)) {
                LOGE() << "_UpdateClientReloadUserProcess(): Load the processes of swcl (" << swcl.GetName()
                       << ") failed !!!";
                return -1;
            }
            configManager_->AddExecutionManifestList(swcl.GetName(), execManifestList);
        }
    }

    if (configManager_->UpdateExecutableInfo(swclManifest) != 0) {
        LOGE() << "_UpdateClientReloadUserProcess(): Update Executable Info for user swcls failed !!!";
        return -1;
    }

    swclManifest->SetManifestPath(spConfigManager->GetUserSwclManifest()->GetManifestPath());

    return 0;
}

/// @brief Clear the software cluster
/// @param swcl Software cluster
/// @return 0 success; <0 failure
/// @code{.isoft}
/// @interface_level=none
/// @trace_id_ad=AD_EM_00124
/// @trace_id_dd=DD_EM_00665
/// @needwork = dda
/// @endcode
int32_t ExecutionManager::_UpdateClientClearSoftwareCluster(config::SoftwareCluster const &swcl) noexcept
{
    LOGV() << "_UpdateClientClearSoftwareCluster(): swclName =" << swcl.GetName() << ", swclVer =" << swcl.GetVersion();

    /// Find the function group
    isoft::core::Set< ara::core::StringView > fgNameList;
    for (auto const &execMf : configManager_->GetExecutionManifestList(swcl.GetName())) {
        std::ignore = fgNameList.emplace(execMf->GetFunctionGroupName());
    }

    /// Clear the function group instance and function group configuration information
    for (auto const &fgName : fgNameList) {
        /// Find the function group instance by function group name
        ara::core::Map< ara::core::String const, std::shared_ptr< FunctionGroup > >::iterator const fgIter{
            functionGroups_.find(fgName.data())};
        // PRQA S 4749 ++
        if (fgIter == functionGroups_.end()) {
            LOGW() << "nullptr == _FindFunctionGroupByName(" << fgName << ")";
            continue;
        }

        /// If the function group is not in the Off state, it cannot be cleared
        if (fgIter->second->IsAlive()) {
            LOGE() << "_UpdateClientClearSoftwareCluster(): FunctionGroup {" << fgName
                   << "} is Alive, Can't be cleared !!!";
            return -1;
        }

        LOGD() << "_UpdateClientClearSoftwareCluster(): Clear FunctionGroup {" << fgIter->second->GetName() << "}";

        /// Delete the function group instance
        std::ignore = fgIter->second->Clear();
        std::ignore = functionGroups_.erase(fgIter);
        // PRQA S 4749 --
        /// Delete the configuration information corresponding to the function group
        configManager_->DeleteFunctionGroups(fgName);
    }

    /// Only after the above operations are successful, can the invalid processes be deleted from the process finder
    for (auto const &execMf : configManager_->GetExecutionManifestList(swcl.GetName())) {
        procFinder_.DelProcess(execMf->GetProcFqn());
    }

    configManager_->DeleteExecutionManifest(swcl.GetName());

    return 0;
}

/// @brief Print SDK release information
/// @code{.isoft}
/// @interface_level=none
/// @trace_id_ad=AD_EM_00124
/// @trace_id_dd=DD_EM_00644
/// @needwork = dda
/// @endcode
void ExecutionManager::_ShowSdkReleaseInfo() const noexcept
{
    ara::core::String const relFile{araSysroot_ + "/release"};
    FILE *const fp{fopen(relFile.c_str(), "re")};
    if (nullptr == fp) {
        LOGW() << "Open SDK release file(" << relFile << ") failed - " << isoft::utils::StrError();
        return;
    }

    constexpr uint32_t kLineSize{512U};
    std::array< Char8_t, kLineSize > tmpLine{'\0'};
    while (true) {
        if (nullptr == fgets(tmpLine.data(), static_cast< int32_t >(kLineSize), fp)) {
            break;
        }
        /// Remove carriage return and line feed
        tmpLine[strnlen(tmpLine.data(), kLineSize) - 1U] = '\0';  // NOINT

        /// Do not print empty lines and the line containing "[ara]"
        bool const notEmptyLine{strnlen(tmpLine.data(), kLineSize) > 0U};
        bool const notAraLine{strncmp(tmpLine.data(), "[ara]", 5U) != 0};
        if (notEmptyLine && notAraLine) {
            LOGI() << tmpLine.data();
        }
    }
    std::ignore = fclose(fp);
}

/// @brief Print debug information
/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
void ExecutionManager::_Debug() const noexcept
{
#if ARA_EXEC_DEBUG
    LOGV() << "+++++ ExecutionManager::Debug +++++";
    LOGV() << "AraSysroot: " << araSysroot_;
    LOGV() << "AraConfig: " << araConfig_;
    LOGV() << "DefaultEnterTimeout: " << configManager_->GetMachineManifest()->GetDefaultEnterTimeout();
    LOGV() << "DefaultExitTimeout: " << configManager_->GetMachineManifest()->GetDefaultExitTimeout();
    ara::core::String cpus;
    // PRQA S 2961 ++
    for (uint8_t const p : configManager_->GetMachineManifest()->GetProcessors()) {
        cpus += std::to_string(static_cast< int32_t >(p)).c_str() + ara::core::String{" "};
    }
    LOGV() << "Processor: " << cpus;

    ara::core::String envs;
    for (auto const &ite : configManager_->GetMachineManifest()->GetEnvironments()) {
        envs += ite + "; ";
    }
    LOGV() << "Environments: " << envs;
    LOGV() << "ResourceGroups: " << configManager_->GetMachineManifest()->GetResourceGroups().size();
    int32_t i = 1;
    for (auto const &rg : configManager_->GetMachineManifest()->GetResourceGroups()) {
        ara::core::String rgInfoStr{rg.GetName() + ": CPU "};
        rgInfoStr += std::to_string(static_cast< int32_t >(rg.GetCpuUsage())).c_str();
        rgInfoStr += "%, MEM ";
        rgInfoStr += std::to_string(rg.GetMemUsage()).c_str();
        rgInfoStr += "Byte";
        LOGV() << "\t" << i << " " << rg.GetName() << ": " << rgInfoStr;
        i = i + 1;
    }

    LOGV() << "CoreSwcls: " << configManager_->GetPlatformSwclManifest()->GetManifestPath();
    for (auto const &swc : configManager_->GetPlatformSwclManifest()->GetSoftwareClusters()) {
        ara::core::String pList{swc.GetName() + "{" + swc.GetVersion() + "}: "};
        for (auto const &p : swc.GetProcessNameList()) {
            pList += p + " ";
        }
        LOGV() << pList;
    }

    LOGV() << "UserSwcls: " << configManager_->GetUserSwclManifest()->GetManifestPath();
    for (auto const &swc : configManager_->GetUserSwclManifest()->GetSoftwareClusters()) {
        ara::core::String pList{swc.GetName() + "{" + swc.GetVersion() + "}: "};
        for (auto const &p : swc.GetProcessNameList()) {
            pList += p + " ";
        }
        LOGV() << pList;
    }
    // PRQA S 2961 --
    LOGV() << "FunctionGroups: " << functionGroups_.size();
    for (auto const &itFg : functionGroups_) {
        std::shared_ptr< FunctionGroup > const &fg{itFg.second};
        fg->DebugFG();
    }

    LOGV() << "----- ExecutionManager::Debug -----";
#endif
}

}  // namespace emd
}  // namespace internal
}  // namespace exec
}  // namespace ara
