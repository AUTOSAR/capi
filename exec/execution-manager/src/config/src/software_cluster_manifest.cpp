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
/// @file       software_cluster_manifest.cpp
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

#include "ara/exec/internal/config/software_cluster_manifest.h"

#include <isoft/manifestreader/manifest_reader.h>

#include "ara/exec/internal/config/log.h"
#include "isoft/core/set.h"

namespace ara {
namespace exec {
namespace internal {
namespace config {

/// @brief Load software cluster information
/// @param node Single software cluster node in the software cluster manifest
/// @return 0 success; !0 failure
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_EM_00109
/// @trace_id_dd=DD_EM_00009
/// @needwork = dda
/// @endcode
int32_t SoftwareCluster::ManifestLoader(isoft::manifestreader::ManifestNode const &node) noexcept
{
    int32_t ret{0};

    /// Software cluster name
    if (isoft::kSuccess != node.Load(kName, name_)) {
        LOGE() << "SoftwareCluster: lack of {" << kName << "} !!!";
        ret = -1;
    }

    /// Software cluster fully qualified name
    if (isoft::kSuccess != node.Load(kFullQualifiedName, fqn_)) {
        LOGE() << "SoftwareCluster: lack of {" << kFullQualifiedName << "} !!!";
        ret = -1;
    }

    /// Software cluster type
    if (isoft::kSuccess != node.Load(kCategory, category_)) {
        LOGE() << "SoftwareCluster: lack of {" << kCategory << "} !!!";
        ret = -1;
    }

    /// Software cluster version number
    if (isoft::kSuccess != node.Load(kVersion, version_)) {
        LOGE() << "SoftwareCluster: lack of {" << kVersion << "} !!!";
        ret = -1;
    }

    /// List of dependent software clusters
    std::ignore = node.Load(kDependOn, dependOnSwclList_);
    /// List of executable program FQNs contained in the software cluster
    std::ignore = node.Load(kExecutableList, executableFqnList_);
    /// List of process names contained in the software cluster, should contain at least one process
    std::ignore = node.Load(kProcessList, processNameList_);

    if (0 == ret) {
        hasConfigError_ = false;
    }

    return 0;
}

/// @brief Whether the specified FQN is in the Executable List of the software cluster
/// @param execFqn Executable program FQN
/// @return true is in the Executable List of the software cluster; false is not in the Executable List of the software cluster
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_EM_00109
/// @trace_id_dd=DD_EM_00008
/// @needwork = dda
/// @endcode
bool SoftwareCluster::IsInExecutableFqnList(ara::core::String const &execFqn) const noexcept
{
    for (auto const &fqn : executableFqnList_) {  // PRQA S 2961
        if (fqn == execFqn) {
            return true;
        }
    }

    return false;
}

/// @brief Whether the software cluster configuration is incorrect
/// @return true configuration error; false configuration correct
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_EM_00109
/// @trace_id_dd=DD_EM_00011
/// @needwork = dda
/// @endcode
bool SoftwareCluster::HasConfigError() const noexcept
{
    if (hasConfigError_) {
        LOGE() << "SoftwareCluster: lack of some key fields !!!";
        return true;
    }

    if (name_.empty()) {
        LOGE() << "SoftwareCluster: name {" << name_.c_str() << "} should not be empty !!!";
        return true;
    }

    if (fqn_.empty()) {
        LOGE() << "SoftwareCluster: fqn {" << fqn_.c_str() << "} should not be empty !!!";
        return true;
    }

    if (category_.empty()) {
        LOGE() << "SoftwareCluster:  category {" << category_.c_str() << "} should not be empty !!!";
        return true;
    }

    if (version_.empty()) {
        LOGE() << "SoftwareCluster: version {" << version_.c_str() << "} should not be empty !!!";
        return true;
    }

#if 0  ///< TODO: Confirm whether the exec list and depend list of the OS software cluster can be empty at the same time
    if (executableFqnList_.size() == 0 && dependOnSwclList_.size() == 0) {
        LOGE() << "SoftwareCluster {" << name_ << "}: the field {" << kExecutableList << "} and {"
                << kDependOn << "} can not be lacked simultaneously !!!";
        return true;
    }

    bool isEmpty = true;
    for (auto const & execFqn: executableFqnList_) {
        if (!execFqn.empty()) {
            isEmpty = false;
            break;
        };
    }

    if (isEmpty) {
        for (auto const & swclName: dependOnSwclList_) {
            if (!swclName.empty()) {
                isEmpty = false;
                break;
            }
        }
    }

    if (isEmpty) {
        LOGE() << "SoftwareCluster: the field {" << kExecutableList << "} and {"
                << kDependOn << "} can not be empty simultaneously !!!";
        return true;
    }
#endif

    return false;
}

/// @brief Print debug information
/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
void SoftwareCluster::Debug() const noexcept
{
#if ARA_EXEC_DEBUG
    std::cout << "+++++ SoftwareCluster::Debug +++++\n";
    std::cout << "Name: " << GetName() << std::endl;
    std::cout << "FQN: " << GetFqn() << std::endl;
    std::cout << "Category: " << category_ << std::endl;
    std::cout << "Version: " << GetVersion() << std::endl;
    std::cout << "Executables: " << std::endl;
    // PRQA S 2961 ++
    for (auto const &e : executableFqnList_) {
        std::cout << "\t" << e << std::endl;
    }
    std::cout << "Processes: " << std::endl;
    for (auto const &p : GetProcessNameList()) {
        std::cout << "\t" << p << std::endl;
    }
    // PRQA S 2961 --
    std::cout << "----- SoftwareCluster::Debug -----\n";
#endif  ///< ARA_EXEC_DEBUG
}

/// @brief Load the manifest file
/// @param swclManifestPath Manifest json file path
/// @return 0 success; <0 failure
/// @code{.isoft}
/// @interface_level=none
/// @trace_id_ad=AD_EM_00109
/// @trace_id_dd=DD_EM_00034
/// @needwork = dda
/// @endcode
int32_t SoftwareClusterManifest::_Load(ara::core::String const &swclManifestPath) noexcept
{
    ara::core::Result< std::unique_ptr< isoft::manifestreader::Manifest > > manifestRes{
        isoft::manifestreader::OpenManifest(swclManifestPath)};
    if (!manifestRes.HasValue()) {
        LOGE() << "SoftwareClusterManifest::Load: Open manifest (" << swclManifestPath << ") failed !!!";
        return -1;
    }

    std::unique_ptr< isoft::manifestreader::Manifest > const manifest{std::move(manifestRes).Value()};
    std::ignore = manifest->Load(kSwcls, softwareClusters_);

    manifestPath_ = swclManifestPath;

    return 0;
}

/// @brief Whether the software cluster manifest configuration is incorrect
/// @return true configuration error; false configuration correct
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_EM_00109
/// @trace_id_dd=DD_EM_00033
/// @needwork = dda
/// @endcode
bool SoftwareClusterManifest::HasConfigError() const noexcept
{
    for (auto const &swcl : softwareClusters_) {  // PRQA S 2961
        if (swcl.HasConfigError()) {
            return true;
        }
    }

    return false;
}

/// @brief Print debug information
/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
void SoftwareClusterManifest::Debug() const noexcept
{
#if ARA_EXEC_DEBUG
    std::cout << "+++++ SoftwareClusterManifest::Debug +++++\n";
    std::cout << "ManifestPath:\t" << manifestPath_ << std::endl;
    for (auto const &swcl : GetSoftwareClusters()) {  // PRQA S 2961
        swcl.Debug();
    }
    std::cout << "----- SoftwareClusterManifest::Debug -----\n";
#endif  ///< ARA_EXEC_DEBUG
}

/// @brief Delete the specified software cluster by name
/// @param name Software cluster name
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_EM_00109
/// @trace_id_dd=DD_EM_00030
/// @needwork = dda
/// @endcode
void SoftwareClusterManifest::DeleteSwclbyName(ara::core::String const &name) noexcept
{
    std::ignore = std::remove_if(softwareClusters_.begin(), softwareClusters_.end(),
                                 [name](SoftwareCluster const &swcl) noexcept { return swcl.GetName() == name; });
}

/// @brief Find a software cluster by name
/// @param name Software cluster name
/// @return !nullptr software cluster pointer; nullptr not found
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_EM_00109
/// @trace_id_dd=DD_EM_00031
/// @needwork = dda
/// @endcode
SoftwareCluster const *SoftwareClusterManifest::FindSwclbyName(ara::core::String const &name) const noexcept
{
    SoftwareCluster const *ret{nullptr};

    for (auto const &swcl : softwareClusters_) {  // PRQA S 2961
        if (name == swcl.GetName()) {
            ret = &swcl;
            break;
        }
    }
    return ret;
}

/// @brief Find a software cluster by FQN
/// @param swclFqn Software cluster FQN
/// @return !nullptr software cluster pointer; nullptr not found
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_EM_00109
/// @trace_id_dd=DD_EM_00032
/// @needwork = dda
/// @endcode
SoftwareCluster const *SoftwareClusterManifest::FindSwclbyFqn(ara::core::String const &swclFqn) const noexcept
{
    SoftwareCluster const *ret{nullptr};
    for (auto const &swcl : softwareClusters_) {  // PRQA S 2961
        if (swclFqn == swcl.GetFqn()) {
            ret = &swcl;
            break;
        }
    }
    return ret;
}

}  // namespace config
}  // namespace internal
}  // namespace exec
}  // namespace ara