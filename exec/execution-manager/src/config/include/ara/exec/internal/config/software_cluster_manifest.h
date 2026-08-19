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
/// @file       software_cluster_manifest.h
/// @brief      ara configuration series software cluster manifest class
/// @details
/// @date       2023-03-06
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/ExecutionManagement/Config
/// @unit_name=SoftwareClusterManifest
/// @unit_description=Used to read and save the software cluster manifest configuration.
/// @interface_level=module
/// @endcode
///
/// ================================================================

#ifndef ARA_EXEC_INTERNAL_CONFIG_SOFTWARE_CLUSTER_MANIFEST_H_
#define ARA_EXEC_INTERNAL_CONFIG_SOFTWARE_CLUSTER_MANIFEST_H_

#include <ara/core/string.h>
#include <ara/core/vector.h>
#include <isoft/manifestreader/manifest_node.h>

#include <cstdint>
#include <memory>

namespace ara {
namespace exec {
namespace internal {
namespace config {

/// @brief Software cluster configuration
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_EM_20001
/// @trace_id_ad=AD_EM_00109
/// @trace_id_dd=DD_EM_00001
/// @needwork = dd
/// @endcode
class SoftwareCluster  // PRQA S 5215
{
public:
    /// @brief Default constructor
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00109
    /// @trace_id_dd=DD_EM_00842
    /// @needwork = dda
    /// @endcode
    SoftwareCluster() = default;

    /// @brief Copy constructor
    /// @param other Other software cluster
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00109
    /// @trace_id_dd=DD_EM_00843
    /// @needwork = dda
    /// @endcode
    SoftwareCluster(SoftwareCluster const& other) = default;

    /// @brief Disable move constructor
    /// @param other Other software cluster
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    SoftwareCluster(SoftwareCluster&& other) noexcept = delete;

    /// @brief Copy assignment operator
    /// @param other Other software cluster
    /// @return New software cluster
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00109
    /// @trace_id_dd=DD_EM_00844
    /// @needwork = dda
    /// @endcode
    SoftwareCluster& operator=(SoftwareCluster const& other) = default;

    /// @brief Move assignment operator
    /// @param other Other software cluster
    /// @return New software cluster
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    SoftwareCluster& operator=(SoftwareCluster&& other) noexcept = default;

    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00109
    /// @trace_id_dd=DD_EM_00002
    /// @needwork = dda
    /// @endcode
    ~SoftwareCluster() noexcept { Clear(); }

    /// @brief Get the software cluster name
    /// @return Software cluster name
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00109
    /// @trace_id_dd=DD_EM_00003
    /// @needwork = dda
    /// @endcode
    ara::core::String const& GetName() const noexcept { return name_; }

    /// @brief Get the software cluster FQN
    /// @return Software cluster FQN
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00109
    /// @trace_id_dd=DD_EM_00004
    /// @needwork = dda
    /// @endcode
    ara::core::String const& GetFqn() const noexcept { return fqn_; }

    /// @brief Get the software cluster version number
    /// @return Software cluster version number
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00109
    /// @trace_id_dd=DD_EM_00005
    /// @needwork = dda
    /// @endcode
    ara::core::String const& GetVersion() const noexcept { return version_; }

    /// @brief Get the list of software clusters that this software cluster depends on
    /// @return List of software clusters that this software cluster depends on
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00109
    /// @trace_id_dd=DD_EM_00006
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< ara::core::String > const& GetDependOnSwclList() const noexcept { return dependOnSwclList_; }

    /// @brief Get the list of process names contained in the software cluster
    /// @return List of process names contained in the software cluster
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00109
    /// @trace_id_dd=DD_EM_00007
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< ara::core::String > const& GetProcessNameList() const noexcept { return processNameList_; }

    /// @brief Whether the specified FQN is in the Executable List of the software cluster
    /// @param execFqn Executable program FQN
    /// @return true is in the Executable List of the software cluster; false is not in the Executable List of the software cluster
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00109
    /// @trace_id_dd=DD_EM_00008
    /// @needwork = dda
    /// @endcode
    bool IsInExecutableFqnList(ara::core::String const& execFqn) const noexcept;

    /// @brief Load software cluster information
    /// @param node Single software cluster node in the software cluster manifest
    /// @return 0 success; !0 failure
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00109
    /// @trace_id_dd=DD_EM_00009
    /// @needwork = dda
    /// @endcode
    int32_t ManifestLoader(isoft::manifestreader::ManifestNode const& node) noexcept;

    /// @brief Clear the contents of the software cluster manifest
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00109
    /// @trace_id_dd=DD_EM_00010
    /// @needwork = dda
    /// @endcode
    void Clear() noexcept
    {
        processNameList_.clear();
        executableFqnList_.clear();
        dependOnSwclList_.clear();
    }

    /// @brief Whether the software cluster configuration is incorrect
    /// @return true configuration error; false configuration correct
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00109
    /// @trace_id_dd=DD_EM_00011
    /// @needwork = dda
    /// @endcode
    bool HasConfigError() const noexcept;

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
    static constexpr char const* const kName{"swcl_name"};

    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    static constexpr char const* const kFullQualifiedName{"swcl_fqn"};

    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    static constexpr char const* const kCategory{"category"};
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    static constexpr char const* const kDependOn{"dependsOn"};
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    static constexpr char const* const kVersion{"swcl_version"};
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    static constexpr char const* const kProcessList{"process_list"};
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    static constexpr char const* const kExecutableList{"executable_list"};

    // PRQA S 2428,4151 -- # Use char* to define json keys to be compatible with manifestreader

    /// @brief Whether the configuration is incorrect
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00109
    /// @trace_id_dd=DD_EM_00014
    /// @needwork = dda
    /// @endcode
    bool hasConfigError_{true};

    /// @brief Software cluster name
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00109
    /// @trace_id_dd=DD_EM_00015
    /// @needwork = dda
    /// @endcode
    ara::core::String name_;
    /// @brief Software cluster fully qualified name
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00109
    /// @trace_id_dd=DD_EM_00016
    /// @needwork = dda
    /// @endcode
    ara::core::String fqn_;
    /// @brief Software cluster type
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00109
    /// @trace_id_dd=DD_EM_00017
    /// @needwork = dda
    /// @endcode
    ara::core::String category_;
    /// @brief Software cluster version number
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00109
    /// @trace_id_dd=DD_EM_00018
    /// @needwork = dda
    /// @endcode
    ara::core::String version_;
    /// @brief Dependent software cluster FQN
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00109
    /// @trace_id_dd=DD_EM_00019
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< ara::core::String > dependOnSwclList_;
    /// @brief List of process names contained in the software cluster
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00109
    /// @trace_id_dd=DD_EM_00020
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< ara::core::String > processNameList_;
    /// @brief List of executable program fully qualified names contained in the software cluster
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00109
    /// @trace_id_dd=DD_EM_00021
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< ara::core::String > executableFqnList_;
};

/// @brief Read and hold the software cluster configuration
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_EM_20001
/// @trace_id_ad=AD_EM_00109
/// @trace_id_dd=DD_EM_00022
/// @needwork = ad
/// @endcode
class SoftwareClusterManifest
{
public:
    /// @brief Constructor
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00109
    /// @trace_id_dd=DD_EM_00023
    /// @needwork = dda
    /// @endcode
    SoftwareClusterManifest() = default;

    /// @brief Copy constructor
    /// @param other Another software cluster manifest instance
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00109
    /// @trace_id_dd=DD_EM_00845
    /// @needwork = dda
    /// @endcode
    SoftwareClusterManifest(SoftwareClusterManifest const& other) = default;

    /// @brief Move constructor
    /// @param other Other software cluster
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    SoftwareClusterManifest(SoftwareClusterManifest&& other) noexcept = delete;

    /// @brief Copy assignment operator
    /// @param other Another software cluster manifest instance
    /// @return New software cluster manifest instance
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00109
    /// @trace_id_dd=DD_EM_00846
    /// @needwork = dda
    /// @endcode
    SoftwareClusterManifest& operator=(SoftwareClusterManifest const& other) = default;

    /// @brief Move assignment operator
    /// @param other Another software cluster manifest instance
    /// @return New software cluster manifest instance
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    SoftwareClusterManifest& operator=(SoftwareClusterManifest&& other) noexcept = delete;

    /// @brief Default destructor
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00109
    /// @trace_id_dd=DD_EM_00024
    /// @needwork = dda
    /// @endcode
    ~SoftwareClusterManifest() noexcept = default;

    /// @brief Create a machine manifest object
    /// @param manifestPath Software cluster manifest path
    /// @return Machine manifest handle
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00109
    /// @trace_id_dd=DD_EM_00025
    /// @needwork = dda
    /// @endcode
    static std::shared_ptr< SoftwareClusterManifest > CreateInstance(  // PRQA S 2024
        ara::core::String const& manifestPath) noexcept
    {
        std::shared_ptr< SoftwareClusterManifest > mf{std::make_shared< SoftwareClusterManifest >()};
        if (nullptr == mf) {
            return mf;
        }

        if (mf->_Load(manifestPath) < 0) {
            mf.reset();
        }

        return mf;
    }

    /// @brief Get the software cluster manifest file name; if the manifest fails to load, the file name will be empty, so it can be used as an indicator of successful loading
    /// @return Software cluster manifest file name
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00109
    /// @trace_id_dd=DD_EM_00026
    /// @needwork = dda
    /// @endcode
    ara::core::String GetManifestPath() const noexcept
    {
        if (HasConfigError()) {
            return "";
        }
        return manifestPath_;
    }

    /// @brief Set the software cluster manifest file name
    /// @param swclManifest Software cluster manifest file name
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00109
    /// @trace_id_dd=DD_EM_00027
    /// @needwork = dda
    /// @endcode
    void SetManifestPath(ara::core::String const& swclManifest) noexcept { manifestPath_ = swclManifest; }

    /// @brief Add a software cluster to the software cluster list
    /// @param swcl Software cluster to add
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00109
    /// @trace_id_dd=DD_EM_00028
    /// @needwork = dda
    /// @endcode
    void AddSoftwareCluster(SoftwareCluster const& swcl) noexcept { return softwareClusters_.emplace_back(swcl); }

    /// @brief Get the list of software clusters
    /// @return List of software clusters
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00109
    /// @trace_id_dd=DD_EM_00029
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< SoftwareCluster > const& GetSoftwareClusters() const noexcept { return softwareClusters_; }

    /// @brief Delete the specified software cluster by name
    /// @param name Software cluster name
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00109
    /// @trace_id_dd=DD_EM_00030
    /// @needwork = dda
    /// @endcode
    void DeleteSwclbyName(ara::core::String const& name) noexcept;

    /// @brief Find a software cluster by name
    /// @param name Software cluster name
    /// @return !nullptr software cluster pointer; nullptr not found
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00109
    /// @trace_id_dd=DD_EM_00031
    /// @needwork = dda
    /// @endcode
    SoftwareCluster const* FindSwclbyName(ara::core::String const& name) const noexcept;

    /// @brief Find a software cluster by FQN
    /// @param swclFqn Software cluster FQN
    /// @return !nullptr software cluster pointer; nullptr not found
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00109
    /// @trace_id_dd=DD_EM_00032
    /// @needwork = dda
    /// @endcode
    SoftwareCluster const* FindSwclbyFqn(ara::core::String const& swclFqn) const noexcept;

    /// @brief Whether the software cluster manifest configuration is incorrect
    /// @return true configuration error; false configuration correct
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00109
    /// @trace_id_dd=DD_EM_00033
    /// @needwork = dda
    /// @endcode
    bool HasConfigError() const noexcept;

    /// @brief Print debug information
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    void Debug() const noexcept;

private:
    /// @brief Load the manifest file
    /// @param swclManifestPath Manifest json file path
    /// @return 0 success; <0 failure
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00109
    /// @trace_id_dd=DD_EM_00034
    /// @needwork = dda
    /// @endcode
    int32_t _Load(ara::core::String const& swclManifestPath) noexcept;

private:
    // PRQA S 2428,4151 ++ # Use char* to define json keys to be compatible with manifestreader

    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    static constexpr char const* const kSwcls{"swcls"};

    // PRQA S 2428,4151 -- # Use char* to define json keys to be compatible with manifestreader

    /// @brief Path to the software cluster manifest file
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00109
    /// @trace_id_dd=DD_EM_00035
    /// @needwork = dda
    /// @endcode
    ara::core::String manifestPath_{""};

    /// @brief List of software clusters
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00109
    /// @trace_id_dd=DD_EM_00036
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< SoftwareCluster > softwareClusters_;
};  ///< class SoftwareClusterManifest

}  // namespace config
}  // namespace internal
}  // namespace exec
}  // namespace ara

#endif  ///< ARA_EXEC_INTERNAL_CONFIG_SOFTWARE_CLUSTER_MANIFEST_H_