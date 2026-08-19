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
/// @file       execution_manager.h
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
/// @interface_level=unit
/// @endcode
///
/// ================================================================

#ifndef _ARA_EXEC_INTERNAL_EXECUTION_MANAGER_H_
#define _ARA_EXEC_INTERNAL_EXECUTION_MANAGER_H_

#include <cstdint>
#include <memory>

#include "ara/core/map.h"
#include "ara/core/string.h"
#include "ara/core/vector.h"
#include "ara/exec/internal/emd/argument.h"
#include "ara/exec/internal/emd/config_manager.h"
#include "ara/exec/internal/emd/function_group.h"
#include "ara/exec/internal/emd/process.h"
#include "ara/exec/internal/ems/server.h"
#include "ara/exec/internal/fps/find_process.h"
#include "ara/exec/internal/rds/debugger.h"
#include "ara/exec/internal/sms/server.h"
#include "ara/exec/internal/ums/server.h"
#include "isoft/ara_fsh/filesystem_hierarchy.h"
#include "isoft/naicpp/global_evloop.h"
#include "isoft/osi/resource_group/resource_group.h"
#include "nai/io/nai_signal.h"

namespace ara {
namespace exec {
namespace internal {
namespace emd {

/// @brief Execution management class definition
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_sr=SR_EM_00001,SR_EM_00066
/// @trace_id_ad=AD_EM_00124
/// @trace_id_dd=DD_EM_00639
/// @needwork = ad
/// @endcode
class ExecutionManager final
{
public:
    /// @brief Disable copy constructor
    /// @param  other the other ExecutionManager
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00124
    /// @trace_id_dd=DD_EM_00646
    /// @needwork = dda
    /// @endcode
    ExecutionManager(ExecutionManager const &other) = delete;

    /// @brief Disable move constructor
    /// @param  other the other ExecutionManager
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    ExecutionManager(ExecutionManager &&other) = delete;

    /// @brief Disable assignment constructor
    /// @param  other the other ExecutionManager
    /// @return New execution management instance
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00124
    /// @trace_id_dd=DD_EM_00647
    /// @needwork = dda
    /// @endcode
    ExecutionManager &operator=(ExecutionManager const &other) = delete;

    /// @brief Disable move assignment function
    /// @param  other the other ExecutionManager
    /// @return New execution management instance
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    ExecutionManager &operator=(ExecutionManager &&other) = delete;

    /// @brief Disable use of default constructor
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00124
    /// @trace_id_dd=DD_EM_00648
    /// @needwork = dda
    /// @endcode
    ExecutionManager() = delete;

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
    static std::shared_ptr< ExecutionManager > CreateInstance(ara::core::String const &araSysroot,  // PRQA S 2024
                                                              ara::core::String const &araConfig,
                                                              Argument const &debugArg) noexcept;

    /// @brief Default destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_EM_00124
    /// @trace_id_dd=DD_EM_00641
    /// @needwork = dda
    /// @endcode
    ~ExecutionManager() noexcept = default;

    /// @brief Start execution management
    /// @return 0 success; <0 failure
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_EM_00124
    /// @trace_id_dd=DD_EM_00642
    /// @needwork = dda
    /// @endcode
    int32_t Run() noexcept;

    /// @brief Stop execution management
    /// @return 0 success; <0 failure
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_EM_00124
    /// @trace_id_dd=DD_EM_00643
    /// @needwork = dda
    /// @endcode
    int32_t Stop() noexcept;

private:
    /// @brief Resource group type alias
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    using ResourceGroup = isoft::osi::resource_group::ResourceGroup;

    /// @brief Print SDK release information
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00124
    /// @trace_id_dd=DD_EM_00644
    /// @needwork = dda
    /// @endcode
    void _ShowSdkReleaseInfo() const noexcept;

    /// @brief Print debug information
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    void _Debug() const noexcept;

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
    explicit ExecutionManager(ara::core::String araSysroot,
                              ara::core::String araConfig,
                              Argument const &debugArg) noexcept;

    /// @brief Initialization operations. Including initializing cgroup/filesystem, reading machine_manifest/loading executable program configuration, etc.
    /// @return 0 success; <0 failure
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00124
    /// @trace_id_dd=DD_EM_00649
    /// @needwork = dda
    /// @endcode
    int32_t _Init() noexcept;

    /// @brief Terminate all running processes
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00124
    /// @trace_id_dd=DD_EM_00650
    /// @needwork = dda
    /// @endcode
    void _TerminateAllRunningProcess() noexcept;

    /// @brief Resource destroy operation. The inverse operation of Init()
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00124
    /// @trace_id_dd=DD_EM_00651
    /// @needwork = dda
    /// @endcode
    void _Destroy() noexcept;

    /// @brief Post-execution cleanup operation
    /// @return 0 success; <0 failure
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00124
    /// @trace_id_dd=DD_EM_00652
    /// @needwork = dda
    /// @endcode
    int32_t _PostRelease() noexcept;

    /// @brief Build the system environment
    /// @return 0 success; <0 failure
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00124
    /// @trace_id_dd=DD_EM_00653
    /// @needwork = dda
    /// @endcode
    int32_t _SetupSysEnv() noexcept;

    /// @brief Initialize the debugger
    /// @return 0 success; <0 failure
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00124
    /// @trace_id_dd=DD_EM_00654
    /// @needwork = dda
    /// @endcode
    int32_t _InitDebugger() noexcept;

    /// @brief Create and initialize resource groups
    /// @return 0 success; <0 failure
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00124
    /// @trace_id_dd=DD_EM_00655
    /// @needwork = dda
    /// @endcode
    int32_t _InitResourceGroup() noexcept;

    /// @brief Destroy and release resource groups
    /// @return 0 success; <0 failure
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00124
    /// @trace_id_dd=DD_EM_00656
    /// @needwork = dda
    /// @endcode
    int32_t _DestroyResourceGroup() noexcept;

    /// @brief Create ems, ums, sms server
    /// @return 0 success; <0 failure
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00124
    /// @trace_id_dd=DD_EM_00657
    /// @needwork = dda
    /// @endcode
    int32_t _InitServer() noexcept;

    /// @brief Destroy the IPC communication environment
    /// @return 0 success; <0 failure
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00124
    /// @trace_id_dd=DD_EM_00658
    /// @needwork = dda
    /// @endcode
    int32_t _DestroyIpcEnv() const noexcept;

    /// @brief Create and initialize POSIX signals
    /// @return 0 success; <0 failure
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00124
    /// @trace_id_dd=DD_EM_00659
    /// @needwork = dda
    /// @endcode
    int32_t _InitPosixSignal() noexcept;

    /// @brief Destroy and release POSIX signals
    /// @return 0 success; <0 failure
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00124
    /// @trace_id_dd=DD_EM_00660
    /// @needwork = dda
    /// @endcode
    int32_t _DestroyPosixSignal() noexcept;

    /// @brief Initialize function groups and processes
    /// @return 0 success; <0 failure
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    int32_t _InitFgAndProcess() noexcept;

    /// @brief Create all process objects based on the execution manifest and assign them to the corresponding function groups
    /// @param executionManifests Execution manifest
    /// @return 0 success; <0 failure
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00124
    /// @trace_id_dd=DD_EM_00661
    /// @needwork = dda
    /// @endcode
    int32_t _LoadProcesses(
        std::list< std::shared_ptr< config::ExecutionManifest > > const &executionManifests) noexcept;

    /// @brief Create function group instances based on function group information
    /// @return 0 success; <0 failure
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00124
    /// @trace_id_dd=DD_EM_00662
    /// @needwork = dda
    /// @endcode
    int32_t _CreateFunctionGroup() noexcept;

    /// @brief Load platform configuration and create platform-related processes
    /// @return 0 success; <0 failure
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00124
    /// @trace_id_dd=DD_EM_00663
    /// @needwork = dda
    /// @endcode
    int32_t _LoadPlatformProcess() noexcept;

    /// @brief Load user configuration and create user-related processes
    /// @return 0 success; <0 failure
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00124
    /// @trace_id_dd=DD_EM_00664
    /// @needwork = dda
    /// @endcode
    int32_t _LoadUserProcess() noexcept;

    /// @brief Clear the software cluster
    /// @param swcl Software cluster
    /// @return 0 success; <0 failure
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00124
    /// @trace_id_dd=DD_EM_00665
    /// @needwork = dda
    /// @endcode
    int32_t _UpdateClientClearSoftwareCluster(config::SoftwareCluster const &swcl) noexcept;

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
    int32_t _TransformFunctionGroupState(ara::core::StringView const &fgName,
                                         ara::core::StringView const &stateName) const noexcept;

    /// @brief Get the current state of the function group
    /// @param fgName Reference to function group name
    /// @return Empty string failure; non-empty string success
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00124
    /// @trace_id_dd=DD_EM_00667
    /// @needwork = dda
    /// @endcode
    ara::core::StringView const _GetCurrentFunctionGroupState(ara::core::StringView const &fgName) const noexcept;

    /// @brief Get the current state of the machine function group
    /// @return Empty string failure; non-empty string success
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00124
    /// @trace_id_dd=DD_EM_00668
    /// @needwork = dda
    /// @endcode
    ara::core::StringView const _GetCurrentMachineState() const noexcept
    {
        return _GetCurrentFunctionGroupState(configManager_->GetMachineFunctionGroupFqn());
    }

    /// @brief  Find the corresponding Process object pointer based on PID
    /// @param pid Process PID
    /// @return nullptr process not found; !nullptr Process object pointer
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00124
    /// @trace_id_dd=DD_EM_00669
    /// @needwork = dda
    /// @endcode
    Process *_FindProcessByPid(uint32_t const pid) noexcept
    {
        Process *proc{nullptr};
        for (auto &fg : functionGroups_) {
            fg.second->TraversalProcess([&proc, pid](std::shared_ptr< Process > const &p) noexcept {
                if (nullptr != p) {
                    if (pid == static_cast< uint32_t >(p->GetPid())) {
                        proc = p.get();
                    }
                }
            });
        }

        return proc;
    }

    /// @brief Find a process instance by process name
    /// @param procName Process name
    /// @return nullptr failure; !nullptr process instance
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00124
    /// @trace_id_dd=DD_EM_00670
    /// @needwork = dda
    /// @endcode
    std::shared_ptr< Process > _FindProcessByName(ara::core::StringView const &procName) noexcept
    {
        for (auto const &it : functionGroups_) {
            std::shared_ptr< FunctionGroup > fg{it.second};
            if (fg) {
                std::shared_ptr< Process > proc{fg->FindProcessByName(ara::core::String{procName})};
                if (proc) {
                    return proc;
                }
            }
        }
        return {nullptr};
    }

    /// @brief  Find the corresponding FunctionGroup object pointer based on function group name
    /// @param fgName Function group name
    /// @return nullptr failure; !nullptr success
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00124
    /// @trace_id_dd=DD_EM_00671
    /// @needwork = dda
    /// @endcode
    std::shared_ptr< FunctionGroup > _FindFunctionGroupByName(  // PRQA S 2024
        ara::core::StringView const &fgName) const noexcept
    {
        ara::core::Map< ara::core::String const, std::shared_ptr< FunctionGroup > >::const_iterator const it{
            functionGroups_.find(ara::core::String{fgName})};
        if (it != functionGroups_.end()) {
            return it->second;
        }
        return {nullptr};
    }

    /// @brief  Find the corresponding ResourceGroup object pointer based on resource group name
    /// @param rgName Resource group name
    /// @return nullptr failure; ! nullptr resource group handle
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00124
    /// @trace_id_dd=DD_EM_00672
    /// @needwork = dda
    /// @endcode
    std::shared_ptr< ResourceGroup > _FindResourceGroupByName(ara::core::String const &rgName) noexcept  // PRQA S 2024
    {
        ara::core::Map< ara::core::String const, std::shared_ptr< ExecutionManager::ResourceGroup > >::iterator const
            it{resourceGroups_.find(rgName)};
        if (it != resourceGroups_.end()) {
            return it->second;
        }
        return {nullptr};
    }

    /// @brief Process creation callback function, called after the process is successfully created, used to perform some operations before exec
    /// @param proc Process handle
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00124
    /// @trace_id_dd=DD_EM_00673
    /// @needwork = dda
    /// @endcode
    void _ProcessStartupedHandler(Process *const proc) noexcept;

    /// @brief Function group state switch callback function, executed whenever the function group state changes
    /// @param fg Function group handle
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00124
    /// @trace_id_dd=DD_EM_00674
    /// @needwork = dda
    /// @endcode
    void _FunctionGroupStateChangedHandler(FunctionGroup const *const fg) noexcept;

    /// @brief  Execution client state report handler function, asynchronously called when a report is received
    /// @param state Received state
    /// @param pid From which PID
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00124
    /// @trace_id_dd=DD_EM_00675
    /// @needwork = dda
    /// @endcode
    void _ExecutionClientReportHandler(ems::Message::State const state, uint32_t const pid) noexcept;

    /// @brief  State client state handler function, asynchronously called when a GetFunctionState event is received
    /// @param fgName Obtained function group name
    /// @param fgState Function group state
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00124
    /// @trace_id_dd=DD_EM_00676
    /// @needwork = dda
    /// @endcode
    void _StateClientGetFunctionGroupStateHandler(ara::core::StringView const &fgName,
                                                  ara::core::StringView const &fgState) noexcept;

    /// @brief  State client state handler function, asynchronously called when a SetFunctionState event is received
    /// @param fgName Function group name
    /// @param fgState Function group state
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00124
    /// @trace_id_dd=DD_EM_00677
    /// @needwork = dda
    /// @endcode
    void _StateClientSetFunctionGroupStateHandler(ara::core::StringView const &fgName,
                                                  ara::core::StringView const &fgState) noexcept;

    /// @brief  State client state transition cancel handler function, asynchronously called when a cancel event is received
    /// @param fgName Function group name
    /// @param fgState Function group state
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00124
    /// @trace_id_dd=DD_EM_00678
    /// @needwork = dda
    /// @endcode
    void _StateClientCancelFunctionGroupStateTransitionHandler(ara::core::StringView const &fgName,
                                                               ara::core::StringView const &fgState) noexcept;

    /// @brief  Update client state event handler function, asynchronously called when a request to get the software cluster list name is received
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00124
    /// @trace_id_dd=DD_EM_00679
    /// @needwork = dda
    /// @endcode
    void _UpdateClientGetUserSwclManifestHandler() noexcept;

    /// @brief  Update client state event handler function, asynchronously called when a request to reparse the software cluster list is received
    /// @param invalidSwcls Invalid software cluster list
    /// @param validSwcls   Effective software cluster list
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00124
    /// @trace_id_dd=DD_EM_00680
    /// @needwork = dda
    /// @endcode
    void _UpdateClientUpdateUserSwclHandler(core::Vector< ums::SwclInfo > const &invalidSwcls,
                                            core::Vector< ums::SwclInfo > const &validSwcls) noexcept;

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
    std::shared_ptr< ConfigManager > _UpdateClientReloadUserSwcls(
        core::Vector< ums::SwclInfo > const &invalidSwcls, core::Vector< ums::SwclInfo > const &validSwcls) noexcept;

    /// @brief Reload user processes from the effective software manifest
    /// @param spConfigManager Newly effective configuration management instance
    /// @return 0 success; <0 failure
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00124
    /// @trace_id_dd=DD_EM_00682
    /// @needwork = dda
    /// @endcode
    int32_t _UpdateClientReloadUserProcess(std::shared_ptr< ConfigManager > const &spConfigManager) noexcept;

    /// @brief Get the debugger instance
    /// @return Debugger instance
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00124
    /// @trace_id_dd=DD_EM_00683
    /// @needwork = dda
    /// @endcode
    //rds::Debugger const &GetDebugger() const noexcept { return debugger_; }

    /// @brief Register the signal processing function for the child process after fork
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00124
    /// @trace_id_dd=DD_EM_00684
    /// @needwork = dda
    /// @endcode
    static void AtForkHandler() noexcept;

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
    static int32_t SignalHandler(nai_signal_t *const signalHander, int32_t const events) noexcept;

private:
    /// @brief ara platform root path
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00124
    /// @trace_id_dd=DD_EM_00686
    /// @needwork = dda
    /// @endcode
    ara::core::String araSysroot_;

    /// @brief Platform configuration file name
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00124
    /// @trace_id_dd=DD_EM_00687
    /// @needwork = dda
    /// @endcode
    ara::core::String araConfig_;

    /// @brief Debug parameters
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00124
    /// @trace_id_dd=DD_EM_00688
    /// @needwork = dda
    /// @endcode
    Argument debugArg_;

    /// @brief Debugger instance
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00124
    /// @trace_id_dd=DD_EM_00689
    /// @needwork = dda
    /// @endcode
    rds::Debugger debugger_;

    /// @brief Configuration manager handle
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00124
    /// @trace_id_dd=DD_EM_00690
    /// @needwork = dda
    /// @endcode
    std::shared_ptr< ConfigManager > configManager_;

    /// @brief Find a process by process name/pid/object address
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00124
    /// @trace_id_dd=DD_EM_00691
    /// @needwork = dda
    /// @endcode
    fps::FindProcess procFinder_;

    /// @brief Mapping of function groups and their names to quickly find the corresponding function group by name
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00124
    /// @trace_id_dd=DD_EM_00692
    /// @needwork = dda
    /// @endcode
    ara::core::Map< ara::core::String const, std::shared_ptr< FunctionGroup > > functionGroups_;

    /// @brief Mapping of resource groups and their names to quickly find the corresponding resource group by name
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00124
    /// @trace_id_dd=DD_EM_00693
    /// @needwork = dda
    /// @endcode
    ara::core::Map< ara::core::String const, std::shared_ptr< ResourceGroup > > resourceGroups_;

    /// @brief Event handle
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00124
    /// @trace_id_dd=DD_EM_00694
    /// @needwork = dda
    /// @endcode
    std::shared_ptr< isoft::naicpp::EvLoop > gMainLoop_;

    /// @brief fsh instance
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00124
    /// @trace_id_dd=DD_EM_00695
    /// @needwork = dda
    /// @endcode
    std::shared_ptr< isoft::ara_fsh::Platform > spFsh_;

    /// @brief Signal handle
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00124
    /// @trace_id_dd=DD_EM_00696
    /// @needwork = dda
    /// @endcode
    bool isStopped_;

    /// @brief Signal handle
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00124
    /// @trace_id_dd=DD_EM_00697
    /// @needwork = dda
    /// @endcode
    nai_signal_t signalChild_;
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00124
    /// @trace_id_dd=DD_EM_00698
    /// @needwork = dda
    /// @endcode
    nai_signal_t signalInterrupt_;
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00124
    /// @trace_id_dd=DD_EM_00699
    /// @needwork = dda
    /// @endcode
    nai_signal_t signalTerminate_;

    /// @brief State client service handle
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00124
    /// @trace_id_dd=DD_EM_00700
    /// @needwork = dda
    /// @endcode
    sms::Server smsServer_;
    /// @brief Execution client service handle
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00124
    /// @trace_id_dd=DD_EM_00701
    /// @needwork = dda
    /// @endcode
    ems::Server emsServer_;
    /// @brief Update client service handle
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00124
    /// @trace_id_dd=DD_EM_00702
    /// @needwork = dda
    /// @endcode
    ums::Server umsServer_;

};  ///< class ExecutionManager

}  // namespace emd
}  // namespace internal
}  // namespace exec
}  // namespace ara

#endif  ///< _ARA_EXEC_INTERNAL_EXECUTION_MANAGER_H_
