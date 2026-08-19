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
/// @file       config_manager.cpp
/// @brief      ara configuration management class
/// @details
/// @date       2023-03-06
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/ExecutionManagement/Emd
/// @unit_name=ConfigManager
/// @unit_description=Used to manage all configurations used by the Emd.
/// @interface_level=module
/// @endcode
///
/// ================================================================

#include "ara/exec/internal/emd/config_manager.h"

#include <isoft/ara_fsh/process.h>
#include <isoft/core/set.h>

#include "ara/exec/internal/config/sysconfig.h"
#include "ara/exec/internal/emd/exec_trust_platform.h"
#include "ara/exec/internal/emd/log.h"
#include "isoft/utils/string.h"

namespace ara {
namespace exec {
namespace internal {
namespace emd {

/// @brief Constructor
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_EM_00120
/// @trace_id_dd=DD_EM_00606
/// @needwork = dda
/// @endcode
ConfigManager::ConfigManager() noexcept
{
    fsh_ = isoft::ara_fsh::Platform::CreatePlatform();
    if (fsh_ == nullptr) {
        LOGE() << "ConfigManager(): Platform::CreatePlatform() failed !!!";
    }
}

/// @brief Create a configuration management class instance
/// @return Configuration management class instance
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_EM_00120
/// @trace_id_dd=DD_EM_00608
/// @needwork = dda
/// @endcode
std::shared_ptr< ConfigManager > ConfigManager::CreateInstance() noexcept
{
    std::shared_ptr< ConfigManager > configManager{std::make_shared< ConfigManager >()};
    if (configManager == nullptr) {
        return {nullptr};
    }

    if (0 != configManager->_LoadConfig()) {
        configManager.reset();
    }

    return configManager;
}

/// @brief Load all configuration files
/// @return 0 success; <0 failure
/// @code{.isoft}
/// @interface_level=none
/// @trace_id_ad=AD_EM_00120
/// @trace_id_dd=DD_EM_00625
/// @needwork = dda
/// @endcode
int32_t ConfigManager::_LoadConfig() noexcept
{
    if (fsh_ == nullptr) {
        LOGF() << "Load fsh failed !!!";
        return -1;
    }

    /// Load the machine manifest
    if (LoadMachineManifest() < 0) {
        LOGF() << "Load Machine Manifest (" << fsh_->GetMachineManifest() << ") failed !!!";
        return -1;
    }
    LOGD() << "Load Machine Manifest (" << fsh_->GetMachineManifest() << ") successfully !!!";

    /// Set the trusted platform boot mode
    ExecTrustPlatform::SetLaunchBehavior(machineManifest_->GetTrustPlatformLaunchBehavior());

    /// Load the platform software cluster manifest
    spPlatformSwclManifest_ = _LoadSwclManifest(ara::core::String{fsh_->GetPlatformSwclsManifest()}, true);
    if (spPlatformSwclManifest_ == nullptr) {
        LOGE() << "Load Platform Software Cluster Manifest (" << fsh_->GetPlatformSwclsManifest() << ") failed !!!";
        return -1;
    }
    LOGD() << "Load Platform Software Cluster Manifest (" << fsh_->GetPlatformSwclsManifest() << ") successfully !!!";

    spPlatformSwclManifest_->Debug();

    /// Load the user software cluster manifest
    spUserSwclManifest_ = _LoadSwclManifest(ara::core::String{fsh_->GetApplicationSwclsManifest()}, false);
    if (spUserSwclManifest_ == nullptr) {
        LOGW() << "Load User Software Cluster Manifest (" << fsh_->GetApplicationSwclsManifest() << ") failed !!!";
    } else {
        LOGD() << "Load user Software Cluster Manifest (" << fsh_->GetApplicationSwclsManifest()
               << ") successfully !!!";
    }

    return 0;
}

/// @brief Load the machine manifest file
/// @return 0 success; <0 failure
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_EM_00120
/// @trace_id_dd=DD_EM_00609
/// @needwork = dda
/// @endcode
int32_t ConfigManager::LoadMachineManifest() noexcept
{
    cpuInfo_ = isoft::osi::cpu::CpuInfo::CreateCpuInfo();
    if (nullptr == cpuInfo_) {
        LOGE() << "CpuInfo::CreateCpuInfo() failed !!!";
        return -1;
    }

    /// Verify the machine manifest
    int32_t const ret{
        ExecTrustPlatform::VerifyMachineManifest(fsh_->GetMachineManifest(), fsh_->GetPlatformSecurityDir())};
    if (ret != 0) {
        LOGF() << "Verify Machine Manifest failed !!!";
        return -1;
    }

    /// Create a machine manifest instance
    machineManifest_ = config::MachineManifest::CreateInstance(ara::core::String{fsh_->GetMachineManifest()});
    if (nullptr == machineManifest_) {
        LOGE() << "Load {" << fsh_->GetMachineManifest() << "} failed !!!";
        return -1;
    }

    machineManifest_->Debug();

    /// Verify whether the resource group configuration in the machine manifest is correct
    for (auto const &rg : machineManifest_->GetResourceGroups()) {  // PRQA S 2961
        if (rg.GetName().empty()) {
            LOGF() << "ResourceGroup name is empty !!!";
            return -1;
        }

        if (kMinMemoryUsage > rg.GetMemUsage()) {
            constexpr uint32_t const kOneMegabyte{1000000U};
            LOGF() << "ResourceGroup {" << rg.GetName() << "," << rg.GetMemUsage() << "}'s memUsage is less than"
                   << (kMinMemoryUsage / kOneMegabyte) << "M !!!";
            return -1;
        }
    }

    return 0;
}

/// @brief Load the software cluster manifest file
/// @param swclManifestPath Software cluster manifest file path
/// @param exitOnError Whether it is a platform software cluster
/// @return !nullptr software cluster manifest instance; nullptr failure
/// @code{.isoft}
/// @interface_level=none
/// @trace_id_ad=AD_EM_00120
/// @trace_id_dd=DD_EM_00626
/// @needwork = dda
/// @endcode
std::shared_ptr< config::SoftwareClusterManifest > ConfigManager::_LoadSwclManifest(
    ara::core::String const &swclManifestPath, bool const exitOnError) noexcept
{
    std::shared_ptr< config::SoftwareClusterManifest > spSwclManifest{
        config::SoftwareClusterManifest::CreateInstance(swclManifestPath)};
    if (nullptr == spSwclManifest) {
        LOGE() << "ConfigManager::LoadSwclManifest(): Load" << swclManifestPath << ") failed !!!";
        return {nullptr};
    }

    /// If the software cluster manifest configuration is incorrect
    if (spSwclManifest->HasConfigError()) {
        LOGW() << "ConfigManager::LoadSwclManifest(): SoftwareClusterManifest {" << swclManifestPath
               << "} has config error !!!";
        if (exitOnError) {
            spSwclManifest.reset();
            return {nullptr};
        }
    }

    for (auto const &swcl : spSwclManifest->GetSoftwareClusters()) {  // PRQA S 2961
        if (swcl.HasConfigError()) {
            continue;
        }
        if (_LoadFunctionGroupSet(swcl) < 0) {
            if (exitOnError) {
                LOGE() << "ConfigManager::LoadSwclManifest(): Load {" << swcl.GetName() << "," << swcl.GetVersion()
                       << "}'s function group set failed !!!";
                spSwclManifest.reset();
                return {nullptr};
            }
        }

        if (_LoadExecutionManifest(swcl) < 0) {
            if (exitOnError) {
                LOGE() << "ConfigManager::LoadSwclManifest(): Load {" << swcl.GetName()
                       << "}'s execution manifest failed !!!";
                spSwclManifest.reset();
                return {nullptr};
            }
        }
    }

    if (UpdateExecutableInfo(spSwclManifest) < 0) {
        /// If it is a platform software cluster, return failure
        if (exitOnError) {
            LOGE() << "Update platform executable info failed !!!";
            spSwclManifest.reset();
            return {nullptr};
        }
    }

    return spSwclManifest;
}

/// @brief Load the function group set under the software cluster
/// @param swcl Software cluster instance
/// @return 0 success; <0 failure
/// @code{.isoft}
/// @interface_level=none
/// @trace_id_ad=AD_EM_00120
/// @trace_id_dd=DD_EM_00627
/// @needwork = dda
/// @endcode
int32_t ConfigManager::_LoadFunctionGroupSet(config::SoftwareCluster const &swcl) noexcept
{
    ara::core::String const fgsPath{fsh_->GetSwclFuncGrpSet(swcl.GetName().c_str(), swcl.GetVersion().c_str())};
    if (0 != access(fgsPath.c_str(), F_OK | R_OK)) {
        LOGW() << "ConfigManager::LoadFunctionGroupSet(): No FunctionGroupSet was found for {" << swcl.GetName() << ","
               << swcl.GetVersion() << "} !!!";
        /// Some software clusters do not have a function group set manifest, this is not considered an error
        return 0;
    }

    std::shared_ptr< config::FunctionGroupSet > fgSet{config::FunctionGroupSet::CreateInstance(fgsPath)};
    if (fgSet == nullptr) {
        LOGE() << "ConfigManager::LoadFunctionGroupSet(): Load Function Group Set failed !!!";
        return -1;
    }

    int32_t ret{0};
    for (auto const &fg : fgSet->GetFunctionGroups()) {  // PRQA S 2961
        if (fg.HasConfigError()) {
            LOGE() << "ConfigManager::LoadFunctionGroupSet(): Function Group Set has config error !!!";
            ret = -1;
            continue;
        }

        functionGroups_.emplace_back(fg);
    }

    fgSet.reset();
    return ret;
}

/// @brief Load all execution manifest files under the software cluster
/// @param swcl Software cluster instance
/// @return 0 success; <0 failure
/// @code{.isoft}
/// @interface_level=none
/// @trace_id_ad=AD_EM_00120
/// @trace_id_dd=DD_EM_00628
/// @needwork = dda
/// @endcode
int32_t ConfigManager::_LoadExecutionManifest(config::SoftwareCluster const &swcl) noexcept
{
    int32_t ret{0};
    for (auto const &procName : swcl.GetProcessNameList()) {
        ara::core::String const procShortName{isoft::utils::GetShortName(procName.c_str())};
        LOGD() << "Load {" << swcl.GetName() << "}'s execution manifest (" << procShortName << ")";
        std::shared_ptr< config::ExecutionManifest > const execManifest{
            _LoadExecutionManifest(swcl.GetName(), swcl.GetVersion(), procShortName)};
        if (nullptr == execManifest) {
            ret = -1;
        } else {
            /// If the execution manifest loads successfully, add it to the software cluster's manifest list
            AddExecutionManifest(swcl.GetName(), execManifest);
        }
    }

    return ret;
}

/// @brief Load the execution manifest file
/// @param swclName Software cluster name
/// @param swclVersion Software cluster version
/// @param procName Process name
/// @return Loaded execution manifest instance
/// @code{.isoft}
/// @interface_level=none
/// @trace_id_ad=AD_EM_00120
/// @trace_id_dd=DD_EM_00629
/// @needwork = dda
/// @endcode
std::shared_ptr< config::ExecutionManifest > ConfigManager::_LoadExecutionManifest(
    ara::core::String const &swclName, ara::core::String const &swclVersion, ara::core::String const &procName) noexcept
{
    if (procName.empty()) {
        return {nullptr};
    }

    /// @code{.isoft}
    /// @trace_id_sws=SWS_EM_02254
    /// Misconfigured processes - assigned to multiple function groups. In the system configuration error event, execution management cannot start processes that reference multiple function group states
    /// @trace_id_sws=SWS_EM_02307 In strict mode, if the authenticity or integrity check of the execution manifest fails, the executable file must not be started
    /// @trace_id_sws=SWS_EM_02308 In strict mode, if the authenticity or integrity check of the service manifest fails, the executable file must not be started
    /// @endcode
    if (0 != ExecTrustPlatform::VerifyProcManifest(swclName.c_str(), swclVersion.c_str(), procName.c_str())) {
        LOGE() << "Verify Execution Manifest Failed !!!";
        return {nullptr};
    }

    std::shared_ptr< isoft::ara_fsh::Process > fshProc{
        isoft::ara_fsh::Process::CreateProcess(swclName.c_str(), swclVersion.c_str(), procName.c_str())};
    ara::core::String const execManifestPath{fshProc->GetExecutionManifest()};
    std::shared_ptr< config::ExecutionManifest > execManifest{
        config::ExecutionManifest::CreateInstance(execManifestPath)};
    if (nullptr == execManifest) {
        LOGE() << "execManifest->Load(" << execManifestPath << ") failed !!!";
        return {nullptr};
    }

    if (execManifest->HasConfigError()) {
        LOGE() << "Execution Manifest {" << execManifestPath << "} has config error !!!";
        execManifest.reset();
        return {nullptr};
    }

    if (!_IsResourceGroupValid(execManifest)) {
        LOGE() << "Execution Manifest {" << execManifestPath << "} has invalid resource group !!!";
        execManifest.reset();
        return {nullptr};
    }

    execManifest->SetSwclName(swclName);
    execManifest->SetSwclVersion(swclVersion);

    std::vector< std::string > envList;
    if (fshProc->ExportEnvironment(envList) == 0) {
        for (std::string const &env : envList) {           // PRQA S 2961
            execManifest->AddSysEnvironment(env.c_str());  // NOLINT
        }
    }

    /// Update the execution manifest information based on the global configuration of the machine manifest
    _UpdateExecutionManifest(execManifest);

    return execManifest;
}

/// @brief Update the executable file information of the execution manifest based on the software cluster manifest
/// @param swclManifest Software cluster manifest instance
/// @return 0 update successful; <0 update failed
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_EM_00120
/// @trace_id_dd=DD_EM_00623
/// @needwork = dda
/// @endcode
int32_t ConfigManager::UpdateExecutableInfo(
    std::shared_ptr< config::SoftwareClusterManifest > const &swclManifest) noexcept
{
    int32_t ret{0};
    ara::core::String execPath;

    for (auto const &swcl : swclManifest->GetSoftwareClusters()) {  // PRQA S 2961
        ara::core::Map< ara::core::String, std::list< std::shared_ptr< config::ExecutionManifest > > >::iterator const
            end{swclExecManifestMap_.end()};
        if (swclExecManifestMap_.find(swcl.GetName()) == end) {
            continue;
        }

        std::list< std::shared_ptr< config::ExecutionManifest > > &execManifestList{
            swclExecManifestMap_[swcl.GetName()]};
        for (auto &execManifest : execManifestList) {
            if (swcl.IsInExecutableFqnList(execManifest->GetExecutableFqn())) {
                execPath = fsh_->GetSwclBinDir(swcl.GetName(), swcl.GetVersion()) + execManifest->GetExecutableName();
                if (0 == access(execPath.c_str(), F_OK | X_OK)) {
                    std::ignore = execManifest->SetExecutablePath(execPath);
                    continue;
                }

                LOGW() << "No executable {" << execPath << "} found, continue to find it in dependent swcl !!!";
                execPath.clear();
            }

            /// If not in the executable file list, check dependent software clusters
            for (auto const &dependSwcl : swcl.GetDependOnSwclList()) {  // PRQA S 2961
                config::SoftwareCluster const *const tmpSwcl{swclManifest->FindSwclbyFqn(dependSwcl)};
                if (nullptr == tmpSwcl) {
                    LOGW() << "No depend swcl {" << dependSwcl << "} found !!!";
                    continue;
                }

                if (tmpSwcl->IsInExecutableFqnList(execManifest->GetExecutableFqn())) {
                    execPath = fsh_->GetSwclBinDir(tmpSwcl->GetName(), tmpSwcl->GetVersion())
                               + execManifest->GetExecutableName();
                    if (0 == access(execPath.c_str(), F_OK | X_OK)) {
                        std::ignore = execManifest->SetExecutablePath(execPath);
                        break;
                    }

                    LOGW() << "No executable {" << execPath << "} found in depend swcl {" << tmpSwcl->GetName()
                           << "} !!!";
                    execPath.clear();
                }
            }

            if (!execPath.empty()) {
                continue;
            }

            /// If the executable file is still not found, for fault tolerance, check if there is an executable file in the current software cluster
            execPath = fsh_->GetSwclBinDir(swcl.GetName(), swcl.GetVersion()) + execManifest->GetExecutableName();
            if (0 != access(execPath.c_str(), F_OK | X_OK)) {
                LOGE() << "No executable {" << execPath << "} found !!!";
                ret = -1;  //PRQA S 2983
                execPath.clear();
                continue;
            }

            std::ignore = execManifest->SetExecutablePath(execPath);
        }
    }

    return ret;
}

/// @brief Check whether the execution manifest configuration is legal
/// @param execManifest Execution manifest instance
/// @return true legal; false illegal
/// @code{.isoft}
/// @interface_level=none
/// @trace_id_ad=AD_EM_00120
/// @trace_id_dd=DD_EM_00630
/// @needwork = dda
/// @endcode
bool ConfigManager::_IsResourceGroupValid(std::shared_ptr< config::ExecutionManifest > const &execManifest) noexcept
{
    for (auto const &stateDepConf : execManifest->GetStateDependentConfigs()) {  // PRQA S 2961
        /// Check whether the resource group name in the execution manifest is consistent with the resource group name in the machine manifest
        bool isRgNameMatched{false};
        for (auto const &rg : machineManifest_->GetResourceGroups()) {  // PRQA S 2961
            if (stateDepConf.GetResourceGroupName().compare(rg.GetName()) == 0) {
                isRgNameMatched = true;
                break;
            }
        }

        /// If the resource group names are inconsistent, the configuration is incorrect
        if (!isRgNameMatched) {
            LOGE() << "Resource group name is not matched with machine manifest.";
            return false;
        }
    }

    return true;
}

/// @brief Update the execution manifest based on the machine manifest and software cluster manifest
/// @param execManifest Execution manifest instance
/// @code{.isoft}
/// @interface_level=none
/// @trace_id_ad=AD_EM_00120
/// @trace_id_dd=DD_EM_00631
/// @needwork = dda
/// @endcode
void ConfigManager::_UpdateExecutionManifest(std::shared_ptr< config::ExecutionManifest > const &execManifest) noexcept
{
    execManifest->AddSysEnvironment(machineManifest_->GetEnvironments());
    execManifest->UpdateProcessors(machineManifest_->GetProcessors(), cpuInfo_);
    execManifest->UpdateTimer(machineManifest_->GetDefaultEnterTimeout(), machineManifest_->GetDefaultExitTimeout());
}

/// @brief Get the FQN of the machine function group
/// @return FQN or empty
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_EM_00120
/// @trace_id_dd=DD_EM_00614
/// @needwork = dda
/// @endcode
ara::core::StringView ConfigManager::GetMachineFunctionGroupFqn() const noexcept
{
    for (auto const &fg : functionGroups_) {
        std::string const fgShortName{isoft::utils::GetShortName(fg.GetName().c_str())};
        if (fgShortName.compare(config::GetMachineFunctionGroupName()) == 0) {
            return fg.GetName();
        }
    }

    return "";
}

/// @brief Load the user software cluster list
/// @return 0 success; <0 failure
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_EM_00120
/// @trace_id_dd=DD_EM_00615
/// @needwork = dda
/// @endcode
int32_t ConfigManager::LoadUserSwclList() noexcept
{
    if (fsh_ == nullptr) {
        LOGE() << "LoadUserSwclList(): fsh_ == nullptr !!!";
        return -1;
    }

    /// Load the new user software cluster manifest
    ara::core::String const manifestPath{fsh_->GetApplicationSwclsManifest()};
    spUserSwclManifest_ = config::SoftwareClusterManifest::CreateInstance(manifestPath);
    bool const hasError{spUserSwclManifest_->HasConfigError()};
    if (spUserSwclManifest_ == nullptr || hasError) {
        LOGE() << "LoadUserSwclList(): Load new swcl list {" << manifestPath << "} failed !!!";
        return -1;
    }

    return 0;
}

/// @brief Load the specified user software cluster information (including function groups and execution manifest)
/// @param swclName Software cluster name
/// @return 0 success; <0 failure
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_EM_00120
/// @trace_id_dd=DD_EM_00616
/// @needwork = dda
/// @endcode
int32_t ConfigManager::LoadSpecifiedUserSwcl(ara::core::String const &swclName) noexcept
{
    bool findSwcl{false};
    for (auto const &swcl : spUserSwclManifest_->GetSoftwareClusters()) {  // PRQA S 2961
        if (swclName == swcl.GetName()) {
            findSwcl = true;
            if (_LoadFunctionGroupSet(swcl) < 0) {
                LOGE() << "ConfigManager::LoadSpecifiedUserSwcl(): Load {" << swcl.GetName() << "," << swcl.GetVersion()
                       << "}'s function group set failed !!!";
                return -1;
            }

            if (_LoadExecutionManifest(swcl) < 0) {
                LOGE() << "ConfigManager::LoadSpecifiedUserSwcl(): Load {" << swcl.GetName()
                       << "}'s execution manifest failed !!!";
                return -1;
            }
        }
    }

    if (!findSwcl) {
        LOGE() << "ConfigManager::LoadSpecifiedUserSwcl(): No specified swcl {" << swclName << "found !!!";
        return -1;
    }

    return 0;
}

}  // namespace emd
}  // namespace internal
}  // namespace exec
}  // namespace ara