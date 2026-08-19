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
/// @file       software_cluster_manifest_parser.h
/// @brief      software cluster manifest parser
/// @details
/// @date       2024-08-15
/// @author     cuiyinli
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @export_level=/UCM/SoftwarePackageManager
/// @module_path=/UCM/SoftwarePackageManager
/// @interface_level=unit
/// @trace_id_sr=SR_UCM_00001
/// @unit_name=SoftwareClusterManifestParser
/// @unit_description=Parser for the software cluster manifest
/// @endcode
///
/// ================================================================

#ifndef ARA_UCM_PKGMGR_PARSING_SOFTWARE_CLUSTER_MANIFEST_PARSER_H_
#define ARA_UCM_PKGMGR_PARSING_SOFTWARE_CLUSTER_MANIFEST_PARSER_H_

#include "manifest_parser.h"
#include "software_cluster_manifest.h"

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @brief Parser for the software cluster manifest
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10253
/// @trace_id_dd=DD_UCM_10369
/// @needwork = ad
/// @endcode
class SoftwareClusterManifestParser : public ManifestParser< SoftwareClusterManifest >
{
public:
    /// @brief constructor
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10370
    /// @needwork = dda
    /// @endcode
    SoftwareClusterManifestParser() = default;
    /// @brief destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10371
    /// @needwork = dda
    /// @endcode
    ~SoftwareClusterManifestParser() override = default;

    /// @brief delete copy construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10372
    /// @needwork = dda
    /// @endcode
    SoftwareClusterManifestParser(SoftwareClusterManifestParser const& other) = delete;
    /// @brief delete copy asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10373
    /// @needwork = dda
    /// @endcode
    SoftwareClusterManifestParser& operator=(SoftwareClusterManifestParser const& other) = delete;
    /// @brief delete move construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10374
    /// @needwork = dda
    /// @endcode
    SoftwareClusterManifestParser(SoftwareClusterManifestParser&& other) = delete;
    /// @brief delete move asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10375
    /// @needwork = dda
    /// @endcode
    SoftwareClusterManifestParser& operator=(SoftwareClusterManifestParser&& other) = delete;

    /// @brief Parses a manifest
    /// @param manifest The manifest json
    /// @returns The parsed manifest object representing the json content
    /// @throws no
    /// @code{.isoft}
    /// <inheritdoc>
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10376
    /// @needwork = dda
    /// @endcode
    std::unique_ptr< SoftwareClusterManifest > Parse(RManifestUPtr const& manifest) override;
};

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // ARA_UCM_PKGMGR_PARSING_SOFTWARE_CLUSTER_MANIFEST_PARSER_H_
