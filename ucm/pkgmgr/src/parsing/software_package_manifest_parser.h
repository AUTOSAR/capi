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
/// @file       software_package_manifest_parser.h
/// @brief      software package manifest parser
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
/// @unit_name=SoftwarePackageManifestParser
/// @unit_description=software package manifest parser
/// @endcode
///
/// ================================================================

#ifndef ARA_UCM_PKGMGR_PARSING_SOFTWARE_PACKAGE_MANIFEST_PARSER_H_
#define ARA_UCM_PKGMGR_PARSING_SOFTWARE_PACKAGE_MANIFEST_PARSER_H_

#include "ara/ucm/pkgmgr/impl_type_actiontype.h"
#include "manifest_parser.h"
#include "software_package_manifest.h"

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @brief Parser for a software package manifest
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10261
/// @trace_id_dd=DD_UCM_10463
/// @needwork = ad
/// @endcode
class SoftwarePackageManifestParser : public ManifestParser< SoftwarePackageManifest >
{
public:
    /// @brief constructor
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10464
    /// @needwork = dda
    /// @endcode
    SoftwarePackageManifestParser() = default;
    /// @brief destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10465
    /// @needwork = dda
    /// @endcode
    ~SoftwarePackageManifestParser() override = default;

    /// @brief delete copy construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10466
    /// @needwork = dda
    /// @endcode
    SoftwarePackageManifestParser(SoftwarePackageManifestParser const& other) = delete;
    /// @brief delete copy asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10467
    /// @needwork = dda
    /// @endcode
    SoftwarePackageManifestParser& operator=(SoftwarePackageManifestParser const& other) = delete;
    /// @brief delete move construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10468
    /// @needwork = dda
    /// @endcode
    SoftwarePackageManifestParser(SoftwarePackageManifestParser&& other) = delete;
    /// @brief delete move asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10469
    /// @needwork = dda
    /// @endcode
    SoftwarePackageManifestParser& operator=(SoftwarePackageManifestParser&& other) = delete;

    /// @brief Parses a manifest
    /// @param manifest The manifest json
    /// @returns The parsed manifest object representing the json content
    /// @throws no
    /// @code{.isoft}
    /// <inheritdoc>
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10470
    /// @needwork = dda
    /// @endcode
    std::unique_ptr< SoftwarePackageManifest > Parse(RManifestUPtr const& manifest) override;

private:
    /// @brief Helper method to parse the actionType attribute
    /// @param manifest The manifest json
    /// @returns ActionType
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10471
    /// @needwork = dda
    /// @endcode
    static AraResult< ActionType > ParseActionType(RManifestUPtr const& manifest);

    /// @brief Helper method to parse the activationAction attribute
    /// @param manifest The manifest json
    /// @returns ActivateOptionType
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10472
    /// @needwork = dda
    /// @endcode
    static AraResult< ActivateOptionType > ParseactivationAction(RManifestUPtr const& manifest);
};

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // ARA_UCM_PKGMGR_PARSING_SOFTWARE_PACKAGE_MANIFEST_PARSER_H_
