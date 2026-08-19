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
/// @file       software_cluster.h
/// @brief      software cluster
/// @details
/// @date       2024-08-15
/// @author     cuiyinli
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @export_level=/UCM/SoftwareClusterManager
/// @module_path=/UCM/SoftwareClusterManager
/// @interface_level=module
/// @trace_id_sr=SR_UCM_00001
/// @unit_name=SoftwareCluster
/// @unit_description=This class represents an adaptive platform Software Cluster
/// @endcode
///
/// ================================================================

#ifndef ARA_UCM_PKGMGR_PARSING_SOFTWARE_CLUSTER_H_
#define ARA_UCM_PKGMGR_PARSING_SOFTWARE_CLUSTER_H_

#include "common/alias.h"
#include "parsing_types.h"
#include "software_cluster_manifest.h"
#include "version.h"

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @brief This class represents an adaptive platform Software Cluster
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10254
/// @trace_id_dd=DD_UCM_10377
/// @needwork = ad
/// @endcode
class SoftwareCluster
{
public:
    /// @brief Initializes a new Software Cluster.
    ///
    /// @param swclManifest The Manifest of this SoftwareCluster
    /// @param manifestPath The absolte path to the manifest file
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10378
    /// @needwork = dda
    /// @endcode
    SoftwareCluster(SoftwareClusterManifest const& swclManifest, AraString const& manifestPath)
        : kSwclManifest{std::move(swclManifest)}
        , manifestPath_{std::move(manifestPath)}  // Call the copy constructor of SoftwareClusterManifest
        , dependencies_{}
    {
    }
    /// @brief destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10379
    /// @needwork = dda
    /// @endcode
    virtual ~SoftwareCluster() = default;

    /// @brief default copy construct
    /// @param other other class object
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10380
    /// @needwork = dda
    /// @endcode
    SoftwareCluster(SoftwareCluster const& other) = default;
    /// @brief default copy asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10381
    /// @needwork = dda
    /// @endcode
    SoftwareCluster& operator=(SoftwareCluster const& other) = delete;
    /// @brief default move construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10382
    /// @needwork = dda
    /// @endcode
    SoftwareCluster(SoftwareCluster&& other) = default;
    /// @brief default move asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10383
    /// @needwork = dda
    /// @endcode
    SoftwareCluster& operator=(SoftwareCluster&& other) = delete;

    /// @brief Set the state of Software Cluster
    /// @param swclState
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10384
    /// @needwork = dda
    /// @endcode
    virtual void SetSwclState(SwClusterStateType swclState) noexcept { swclState_ = swclState; }  // Not used for now
    /// @brief Returns the state of this Software Cluster.
    ///
    /// @returns Software Cluster state
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10385
    /// @needwork = dda
    /// @endcode
    virtual SwClusterStateType const& GetSwclState() const noexcept { return swclState_; }

    /// @brief Returns the manifest of this Software Cluster
    ///
    /// @returns the Software Cluster Manifest
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10386
    /// @needwork = dda
    /// @endcode
    virtual SoftwareClusterManifest const& GetSwclManifest() const noexcept { return kSwclManifest; }

    /// @brief Returns the absolute path to this Software Cluster's manifest file.
    ///
    /// @returns absolute path to manifest
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10387
    /// @needwork = dda
    /// @endcode
    AraString const& GetManifestPath() const noexcept { return manifestPath_; }

    /// @brief Returns the dependencies of this application.
    ///
    /// The dependencies are defined in the application manifest
    ///
    /// @returns List of swcl
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10388
    /// @needwork = dda
    /// @endcode
    virtual AraVector< SoftwareCluster > const& GetDependencies() const noexcept
    {
        return dependencies_;
    }  // Not used for now
       // GetMissingDependencies in PackageManagerImpl uses GetSoftwareClusterDependencies of SoftwareClusterManifest

private:
    /// @brief The state of Software Cluster
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10389
    /// @needwork = dda
    /// @endcode
    SwClusterStateType swclState_{SwClusterStateType::kPresent};
    /// @brief parsed Software Cluster manifest of this Software Cluster
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10390
    /// @needwork = dda
    /// @endcode
    SoftwareClusterManifest const kSwclManifest;
    /// @brief Absolute path to this Software Cluster's manifest file
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10391
    /// @needwork = dda
    /// @endcode
    ara::core::String manifestPath_{""};
    /// @brief List of dependencies for this Software Cluster.
    /// This list is filled when parsing the manifest file
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10392
    /// @needwork = dda
    /// @endcode
    AraVector< SoftwareCluster > dependencies_{};
};

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // ARA_UCM_PKGMGR_PARSING_SOFTWARE_CLUSTER_H_
