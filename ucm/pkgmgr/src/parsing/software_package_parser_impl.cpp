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
/// @file       software_package_parser_impl.cpp
/// @brief      software package parser implementation
/// @details
/// @date       2024-08-15
/// @author     cuiyinli
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/UCM/SoftwarePackageManager
/// @interface_level=unit
/// @trace_id_sr=SR_UCM_00001
/// @unit_name=SoftwarePackageParserImpl
/// @unit_description=software package parser implementation
/// @endcode
///
/// ================================================================

#include "software_package_parser_impl.h"

#include "software_cluster_manifest.h"
#include "software_package_manifest.h"

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @brief Parse the extracted software package
///
/// @param extractionDir Full file path to the extraction directory
///
/// @returns The parsed SoftwarePackage
/// @throws no
std::unique_ptr< SoftwarePackage > SoftwarePackageParserImpl::Parse(AraString const& extractionDir)
{
    std::unique_ptr< SoftwarePackage > swpkg{nullptr};
    std::unique_ptr< SoftwarePackageManifest > swpkgMnf{nullptr};
    std::unique_ptr< SoftwareClusterManifest > swclMnf{nullptr};
    AraString swpkgMnfPath;
    AraString swclMnfPath;

    swpkgMnfPath = extractionDir + "/swpkg_manifest.json";
    swclMnfPath  = extractionDir + "/swcl_manifest.json";

    /// Parse to get SoftwarePackageManifest
    swpkgMnf = swpkgManifestParser_.ParseFromFile(swpkgMnfPath);
    if (swpkgMnf == nullptr) {
        return swpkg;
    }

    /// Parse to get SoftwareClusterManifest
    swclMnf = swclManifestParser_.ParseFromFile(swclMnfPath);
    if (swclMnf == nullptr) {
        return swpkg;
    }

    /// Construct SoftwareCluster (copy *swclManifest)
    SoftwareCluster swcl{*swclMnf, swclMnfPath};

    /// Construct SoftwarePackage (copy *swpkgManifest and swCluster)
    swpkg = std::make_unique< SoftwarePackage >(*swpkgMnf, extractionDir, swcl);
    return swpkg;
}

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara
