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
/// @file       machine_manifest.h
/// @brief      ara configuration series machine manifest class
/// @details
/// @date       2023-03-06
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/ExecutionManagement/Config
/// @unit_name=MachineManifest
/// @unit_description=Used to read and save the machine manifest configuration.
/// @interface_level=module
/// @endcode
///
/// ================================================================

#ifndef ARA_EXEC_INTERNAL_CONFIG_MACHINE_MANIFEST_H_
#define ARA_EXEC_INTERNAL_CONFIG_MACHINE_MANIFEST_H_

#include <ara/core/map.h>
#include <ara/core/string.h>
#include <ara/core/vector.h>
#include <isoft/manifestreader/manifest_node.h>
#include <isoft/manifestreader/tps_enumeration.h>

#include <cstdint>

namespace ara {
namespace exec {
namespace internal {
namespace config {

/// @brief Resource group configuration
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_EM_20003
/// @trace_id_ad=AD_EM_00108
/// @trace_id_dd=DD_EM_00037
/// @needwork = dd
/// @endcode
class ResourceGroup  // PRQA S 5215
{
public:
    /// @brief Get the resource group name
    /// @return Resource group name
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00108
    /// @trace_id_dd=DD_EM_00038
    /// @needwork = dda
    /// @endcode
    ara::core::String const &GetName() const noexcept { return name_; }

    /// @brief CPU usage limit (1 ~ 100)
    /// @return A non-negative integer within 100 (inclusive)
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00108
    /// @trace_id_dd=DD_EM_00039
    /// @needwork = dda
    /// @endcode
    uint8_t GetCpuUsage() const noexcept { return cpuUsage_; }

    /// @brief Get the memory usage limit
    /// @return Memory usage limit in bytes
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00108
    /// @trace_id_dd=DD_EM_00040
    /// @needwork = dda
    /// @endcode
    uint32_t GetMemUsage() const noexcept { return memUsage_; }

    /// @brief Load resource group configuration
    /// @param node Resource group configuration node
    /// @return 0 success; <0 failure
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00108
    /// @trace_id_dd=DD_EM_00041
    /// @needwork = dda
    /// @endcode
    int32_t ManifestLoader(isoft::manifestreader::ManifestNode const &node) noexcept;

    /// @brief Print debug information
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    void Debug() const noexcept;

private:
    // PRQA S 2428,4151 ++ # Use char* to define json keys to be compatible with manifestreader

    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    static constexpr char const *const kName{"name"};

    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    static constexpr char const *const kCpuUsage{"cpuUsage"};

    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    static constexpr char const *const kMemUsage{"memUsage"};

    // PRQA S 2428,4151 -- # Use char* to define json keys to be compatible with manifestreader

    /// @brief Resource group name
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00108
    /// @trace_id_dd=DD_EM_00042
    /// @needwork = dda
    /// @endcode
    ara::core::String name_;

    /// @brief CPU usage limit, 1 ~ 100
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00108
    /// @trace_id_dd=DD_EM_00043
    /// @needwork = dda
    /// @endcode
    uint8_t cpuUsage_;

    /// @brief Memory usage limit, in bytes
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00108
    /// @trace_id_dd=DD_EM_00044
    /// @needwork = dda
    /// @endcode
    uint32_t memUsage_;
};

/// @brief Read and hold the configuration information of the machine manifest
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_EM_20002~SR_EM_20007
/// @trace_id_ad=AD_EM_00108
/// @trace_id_dd=DD_EM_00045
/// @needwork = ad
/// @endcode
class MachineManifest
{
public:
    /// @brief Default constructor
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00108
    /// @trace_id_dd=DD_EM_00046
    /// @needwork = dda
    /// @endcode
    MachineManifest() noexcept = default;

    /// @brief Default destructor
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00108
    /// @trace_id_dd=DD_EM_00047
    /// @needwork = dda
    /// @endcode
    ~MachineManifest() noexcept = default;

    /// @brief Default copy constructor
    /// @param other Another machine manifest instance
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    MachineManifest(MachineManifest const &other) = delete;

    /// @brief Default move constructor
    /// @param other Another machine manifest instance
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    MachineManifest(MachineManifest &&other) = delete;

    /// @brief Default copy assignment function
    /// @param other Another machine manifest instance
    /// @return New machine manifest instance
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    MachineManifest &operator=(MachineManifest const &other) = delete;

    /// @brief Default move assignment function
    /// @param other Another machine manifest instance
    /// @return New machine manifest instance
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    MachineManifest &operator=(MachineManifest &&other) = delete;

    /// @brief Define trusted platform boot mode
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    using TrustedPlatformLaunchBehaviorEnum = isoft::manifestreader::tps::TrustedPlatformExecutableLaunchBehaviorEnum;

    /// @brief Create a machine manifest object
    /// @param manifestPath Machine manifest path
    /// @return Machine manifest handle
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00108
    /// @trace_id_dd=DD_EM_00048
    /// @needwork = dda
    /// @endcode
    static std::shared_ptr< MachineManifest > CreateInstance(  // PRQA S 2024
        ara::core::String const &manifestPath) noexcept
    {
        std::shared_ptr< MachineManifest > mf{std::make_shared< MachineManifest >()};
        if (nullptr == mf) {
            return mf;
        }

        if (0 != mf->_Load(manifestPath)) {
            mf.reset();
        }
        return mf;
    }

    /// @brief Get the process default startup timeout
    /// @return Seconds
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00108
    /// @trace_id_dd=DD_EM_00049
    /// @needwork = dda
    /// @endcode
    double GetDefaultEnterTimeout() const noexcept { return defaultEnterTimeout_; }

    /// @brief Get the process default exit timeout
    /// @return Seconds
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00108
    /// @trace_id_dd=DD_EM_00050
    /// @needwork = dda
    /// @endcode
    double GetDefaultExitTimeout() const noexcept { return defaultExitTimeout_; }

    /// @brief Get the environment variables provided by the machine
    /// @return Environment variables provided by the machine
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00108
    /// @trace_id_dd=DD_EM_00051
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< ara::core::String > const &GetEnvironments() const noexcept { return environments_; }

    /// @brief Get the CPUID bound to the machine, you can specify the CPU core to run on for the current machine
    /// @return Set of CPUIDs bound to the machine
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00108
    /// @trace_id_dd=DD_EM_00052
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< uint8_t > const &GetProcessors() const noexcept { return processors_; }

    /// @brief Get the trusted platform boot mode
    /// @return Trusted platform boot mode
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00108
    /// @trace_id_dd=DD_EM_00053
    /// @needwork = dda
    /// @endcode
    TrustedPlatformLaunchBehaviorEnum const &GetTrustPlatformLaunchBehavior() const noexcept
    {
        return trustPlatformLaunchBehavior_;
    }

    /// @brief Get the resource group information configured by the machine
    /// @return Resource group information configured by the machine
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00108
    /// @trace_id_dd=DD_EM_00054
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< ResourceGroup > const &GetResourceGroups() const noexcept { return resourceGroups_; }

    /// @brief Print debug information
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    void Debug() const noexcept;

private:
    /// @brief Load the manifest file
    /// @param manifestPath Manifest json file path
    /// @return 0 success; <0 failure
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00108
    /// @trace_id_dd=DD_EM_00055
    /// @needwork = dda
    /// @endcode
    int32_t _Load(ara::core::String const &manifestPath) noexcept;

    /// @brief Add environment variable
    /// @param env Environment variable name (key=value)
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00108
    /// @trace_id_dd=DD_EM_00056
    /// @needwork = dda
    /// @endcode
    void _AddEnvironment(ara::core::String const &env) noexcept
    {
        ///FIXME: Detect duplicate keys and overwrite
        environments_.emplace_back(env);
    }

private:
    // PRQA S 2428,4151 ++ # Use char* to define json keys to be compatible with manifestreader

    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    static constexpr char const *const kDefaultEnterTimeout{"defaultApplicationTimeout.enterTimeout"};

    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    static constexpr char const *const kDefaultExitTimeout{"defaultApplicationTimeout.exitTimeout"};

    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    static constexpr char const *const kEnvironments{"environmentVariable"};

    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    static constexpr char const *const kProcessor{"processor"};

    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    static constexpr char const *const kTrustExecLaunchBehavior{"trustedPlatformExecutableLaunchBehavior"};

    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    static constexpr char const *const kResourceGroups{"resourceGroups"};

    // PRQA S 2428,4151 -- # Use char* to define json keys to be compatible with manifestreader

    /// @brief Process default startup timeout
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00108
    /// @trace_id_dd=DD_EM_00057
    /// @needwork = dda
    /// @endcode
    double defaultEnterTimeout_{0.0};

    /// @brief Process default termination timeout
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00108
    /// @trace_id_dd=DD_EM_00058
    /// @needwork = dda
    /// @endcode
    double defaultExitTimeout_{0.0};

    /// @brief Environment variables provided by the machine, will be shared by all processes
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00108
    /// @trace_id_dd=DD_EM_00059
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< ara::core::String > environments_;

    /// @brief CPUID bound to the machine, you can specify the CPU core to run on for the current machine
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00108
    /// @trace_id_dd=DD_EM_00060
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< uint8_t > processors_;

    /// @brief Trusted platform boot mode, determines whether to start the executable program when it fails verification
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00108
    /// @trace_id_dd=DD_EM_00061
    /// @needwork = dda
    /// @endcode
    TrustedPlatformLaunchBehaviorEnum trustPlatformLaunchBehavior_{TrustedPlatformLaunchBehaviorEnum::kMonitorMode};

    /// @brief Resource group information configured by the machine
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00108
    /// @trace_id_dd=DD_EM_00062
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< ResourceGroup > resourceGroups_;
};  ///< MachineManifest

}  // namespace config
}  // namespace internal
}  // namespace exec
}  // namespace ara

#endif  ///< ARA_EXEC_INTERNAL_CONFIG_MACHINE_MANIFEST_H_