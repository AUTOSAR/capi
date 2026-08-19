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
/// @file       config_manager.h
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
/// @interface_level=unit
/// @endcode
///
/// ================================================================

#ifndef ARA_EXEC_INTERNAL_CONFIG_MANAGER_H_
#define ARA_EXEC_INTERNAL_CONFIG_MANAGER_H_

#include <algorithm>
#include <list>

#include "ara/exec/internal/config/execution_manifest.h"
#include "ara/exec/internal/config/function_group_set.h"
#include "ara/exec/internal/config/machine_manifest.h"
#include "ara/exec/internal/config/software_cluster_manifest.h"
#include "isoft/ara_fsh/platform.h"
#include "isoft/osi/cpu/cpu_info.h"

namespace ara {
namespace exec {
namespace internal {
namespace emd {

/// @brief Configuration management class, used to manage all configurations used by EM (machine manifest, software cluster manifest, function group configuration, execution manifest, etc.)
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_EM_00066
/// @trace_id_ad=AD_EM_00120
/// @trace_id_dd=DD_EM_00605
/// @needwork = ad
/// @endcode
class ConfigManager
{
public:
    /// @brief Constructor
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00120
    /// @trace_id_dd=DD_EM_00606
    /// @needwork = dda
    /// @endcode
    ConfigManager() noexcept;

    /// @brief Default destructor
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00120
    /// @trace_id_dd=DD_EM_00607
    /// @needwork = dda
    /// @endcode
    ~ConfigManager() noexcept = default;

    /// @brief Disable move constructor
    /// @param other the other ConfigManager
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    ConfigManager(ConfigManager &&other) noexcept = delete;

    /// @brief Disable copy constructor
    /// @param other the other ConfigManager
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    ConfigManager(ConfigManager const &other) noexcept = delete;

    /// @brief Disable move assignment
    /// @param other the other ConfigManager
    /// @return New configuration management instance
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    ConfigManager &operator=(ConfigManager &&other) noexcept = delete;

    /// @brief Disable copy assignment
    /// @param other the other ConfigManager
    /// @return New configuration management instance
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    ConfigManager &operator=(ConfigManager const &other) noexcept = delete;

    /// @brief Create a configuration management class instance
    /// @return Configuration management class instance
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00120
    /// @trace_id_dd=DD_EM_00608
    /// @needwork = dda
    /// @endcode
    static std::shared_ptr< ConfigManager > CreateInstance() noexcept;  // PRQA S 2024

    /// @brief Load the machine manifest file
    /// @return 0 success; <0 failure
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00120
    /// @trace_id_dd=DD_EM_00609
    /// @needwork = dda
    /// @endcode
    int32_t LoadMachineManifest() noexcept;

    /// @brief Get the machine manifest handle
    /// @return Machine manifest handle
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00120
    /// @trace_id_dd=DD_EM_00610
    /// @needwork = dda
    /// @endcode
    std::shared_ptr< config::MachineManifest const > GetMachineManifest() const noexcept  // PRQA S 2024
    {
        return machineManifest_;
    }

    /// @brief Get the platform software cluster manifest handle
    /// @return Platform software cluster handle
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00120
    /// @trace_id_dd=DD_EM_00611
    /// @needwork = dda
    /// @endcode
    std::shared_ptr< config::SoftwareClusterManifest > GetPlatformSwclManifest() const noexcept
    {
        return spPlatformSwclManifest_;
    }

    /// @brief Get the user software cluster manifest handle
    /// @return Application software cluster handle
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00120
    /// @trace_id_dd=DD_EM_00612
    /// @needwork = dda
    /// @endcode
    std::shared_ptr< config::SoftwareClusterManifest > GetUserSwclManifest() const noexcept
    {
        return spUserSwclManifest_;
    }

    /// @brief Get function group information
    /// @return Function group set
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00120
    /// @trace_id_dd=DD_EM_00613
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< config::FunctionGroup > const &GetFunctionGroups() const noexcept { return functionGroups_; }

    /// @brief Get the FQN of the machine function group
    /// @return FQN or empty
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00120
    /// @trace_id_dd=DD_EM_00614
    /// @needwork = dda
    /// @endcode
    ara::core::StringView GetMachineFunctionGroupFqn() const noexcept;

    /// @brief Load the user software cluster list
    /// @return 0 success; <0 failure
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00120
    /// @trace_id_dd=DD_EM_00615
    /// @needwork = dda
    /// @endcode
    int32_t LoadUserSwclList() noexcept;

    /// @brief Load the specified user software cluster information (including function groups and execution manifest)
    /// @param swclName Software cluster name
    /// @return 0 success; <0 failure
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00120
    /// @trace_id_dd=DD_EM_00616
    /// @needwork = dda
    /// @endcode
    int32_t LoadSpecifiedUserSwcl(ara::core::String const &swclName) noexcept;

    /// @brief Delete the specified function group information
    /// @param fgName List of function group names to delete
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00120
    /// @trace_id_dd=DD_EM_00617
    /// @needwork = dda
    /// @endcode
    void DeleteFunctionGroups(ara::core::StringView const &fgName) noexcept
    {
        std::ignore
            = std::remove_if(functionGroups_.begin(), functionGroups_.end(),
                             [&fgName](config::FunctionGroup const &fg) noexcept { return fg.GetName() == fgName; });
    }

    /// @brief Add the specified function group information
    /// @param fgs Function group set to add
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00120
    /// @trace_id_dd=DD_EM_00618
    /// @needwork = dda
    /// @endcode
    void AddFunctionGroups(ara::core::Vector< config::FunctionGroup > const &fgs) noexcept
    {
        if (!fgs.empty()) {
            std::ignore = functionGroups_.insert(functionGroups_.cend(), fgs.cbegin(), fgs.cend());  // PRQA S 2961
        }
    }

    /// @brief Add the successfully loaded execution manifest to the manifest list
    /// @param swclName Software cluster name
    /// @param execManifest Execution manifest
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00120
    /// @trace_id_dd=DD_EM_00619
    /// @needwork = dda
    /// @endcode
    void AddExecutionManifest(ara::core::String const &swclName,
                              std::shared_ptr< config::ExecutionManifest > const &execManifest) noexcept
    {
        swclExecManifestMap_[swclName].emplace_back(execManifest);
    }

    /// @brief Add the successfully loaded execution manifest to the manifest list
    /// @param swclName Software cluster name
    /// @param execManifestList List of execution manifests
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00120
    /// @trace_id_dd=DD_EM_00620
    /// @needwork = dda
    /// @endcode
    void AddExecutionManifestList(
        ara::core::String const &swclName,
        std::list< std::shared_ptr< config::ExecutionManifest > > const &execManifestList) noexcept
    {
        if (!execManifestList.empty()) {
            std::list< std::shared_ptr< config::ExecutionManifest > >::const_iterator const end{
                swclExecManifestMap_[swclName].cend()};
            std::ignore = swclExecManifestMap_[swclName].insert(end, execManifestList.begin(), execManifestList.end());
        }
    }

    /// @brief Delete the execution manifest of the specified software cluster
    /// @param swclName Execution manifest
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00120
    /// @trace_id_dd=DD_EM_00621
    /// @needwork = dda
    /// @endcode
    void DeleteExecutionManifest(ara::core::String const &swclName) noexcept
    {
        std::ignore = swclExecManifestMap_.erase(swclName);
    }

    /// @brief Get all execution manifests under the specified software cluster
    /// @param swclName Software cluster name
    /// @return All execution manifests under the specified software cluster
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00120
    /// @trace_id_dd=DD_EM_00622
    /// @needwork = dda
    /// @endcode
    std::list< std::shared_ptr< config::ExecutionManifest > > const &GetExecutionManifestList(
        ara::core::String const &swclName) noexcept
    {
        return swclExecManifestMap_[swclName];
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
    int32_t UpdateExecutableInfo(std::shared_ptr< config::SoftwareClusterManifest > const &swclManifest) noexcept;

private:
    /// @brief Load all configuration files
    /// @return 0 success; <0 failure
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00120
    /// @trace_id_dd=DD_EM_00625
    /// @needwork = dda
    /// @endcode
    int32_t _LoadConfig() noexcept;

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
    std::shared_ptr< config::SoftwareClusterManifest > _LoadSwclManifest(ara::core::String const &swclManifestPath,
                                                                         bool const exitOnError) noexcept;

    /// @brief Load the function group set under the software cluster
    /// @param swcl Software cluster instance
    /// @return 0 success; <0 failure
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00120
    /// @trace_id_dd=DD_EM_00627
    /// @needwork = dda
    /// @endcode
    int32_t _LoadFunctionGroupSet(config::SoftwareCluster const &swcl) noexcept;

    /// @brief Load all execution manifest files under the software cluster
    /// @param swcl Software cluster instance
    /// @return 0 success; <0 failure
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00120
    /// @trace_id_dd=DD_EM_00628
    /// @needwork = dda
    /// @endcode
    int32_t _LoadExecutionManifest(config::SoftwareCluster const &swcl) noexcept;

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
    std::shared_ptr< config::ExecutionManifest > _LoadExecutionManifest(ara::core::String const &swclName,
                                                                        ara::core::String const &swclVersion,
                                                                        ara::core::String const &procName) noexcept;

    /// @brief Check whether the execution manifest configuration is legal
    /// @param execManifest Execution manifest instance
    /// @return true legal; false illegal
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00120
    /// @trace_id_dd=DD_EM_00630
    /// @needwork = dda
    /// @endcode
    bool _IsResourceGroupValid(std::shared_ptr< config::ExecutionManifest > const &execManifest) noexcept;

    /// @brief Update the execution manifest based on the machine manifest and software cluster manifest
    /// @param execManifest Execution manifest instance
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00120
    /// @trace_id_dd=DD_EM_00631
    /// @needwork = dda
    /// @endcode
    void _UpdateExecutionManifest(std::shared_ptr< config::ExecutionManifest > const &execManifest) noexcept;

private:
    /// @brief Minimum memory usage required
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    static constexpr uint32_t const kMinMemoryUsage{100000000U};

    /// @brief FSH platform information
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00120
    /// @trace_id_dd=DD_EM_00632
    /// @needwork = dda
    /// @endcode
    std::shared_ptr< isoft::ara_fsh::Platform > fsh_{nullptr};

    /// @brief CPU information
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00120
    /// @trace_id_dd=DD_EM_00633
    /// @needwork = dda
    /// @endcode
    std::shared_ptr< isoft::osi::cpu::CpuInfo > cpuInfo_{nullptr};

    /// @brief Machine manifest
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00120
    /// @trace_id_dd=DD_EM_00634
    /// @needwork = dda
    /// @endcode
    std::shared_ptr< config::MachineManifest > machineManifest_{nullptr};

    /// @brief Function group information (including platform function groups and user function groups)
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00120
    /// @trace_id_dd=DD_EM_00635
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< config::FunctionGroup > functionGroups_{};

    /// @brief Platform software cluster
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00120
    /// @trace_id_dd=DD_EM_00636
    /// @needwork = dda
    /// @endcode
    std::shared_ptr< config::SoftwareClusterManifest > spPlatformSwclManifest_{nullptr};

    /// @brief Application software cluster
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00120
    /// @trace_id_dd=DD_EM_00637
    /// @needwork = dda
    /// @endcode
    std::shared_ptr< config::SoftwareClusterManifest > spUserSwclManifest_{nullptr};

    /// @brief Software cluster execution manifest Map
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00120
    /// @trace_id_dd=DD_EM_00638
    /// @needwork = dda
    /// @endcode
    ara::core::Map< ara::core::String, std::list< std::shared_ptr< config::ExecutionManifest > > >
        swclExecManifestMap_{};
};

}  // namespace emd
}  // namespace internal
}  // namespace exec
}  // namespace ara

#endif  ///< ARA_EXEC_INTERNAL_CONFIG_MANAGER_H_
