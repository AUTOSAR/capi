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
/// @file       software_package_parser_impl.h
/// @brief      software package parser impl
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
/// @unit_description=software package parser impl
/// @endcode
///
/// ================================================================

#ifndef ARA_UCM_PKGMGR_PARSING_SOFTWARE_PACKAGE_PARSER_IMPL_H_
#define ARA_UCM_PKGMGR_PARSING_SOFTWARE_PACKAGE_PARSER_IMPL_H_

#include <memory>

#include "common/alias.h"
#include "software_cluster_manifest_parser.h"
#include "software_package.h"
#include "software_package_manifest_parser.h"
#include "software_package_parser.h"

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @code{.isoft}
/// @copydoc SoftwarePackageParser
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10262
/// @trace_id_dd=DD_UCM_10433
/// @needwork = dd
/// @endcode
class SoftwarePackageParserImpl : public SoftwarePackageParser
{
public:
    /// @brief Creates an instance of the SoftwarePackageParser.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10434
    /// @needwork = dda
    /// @endcode
    SoftwarePackageParserImpl() = default;
    /// @brief destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10435
    /// @needwork = dda
    /// @endcode
    ~SoftwarePackageParserImpl() override = default;

    /// @brief delete copy construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10436
    /// @needwork = dda
    /// @endcode
    SoftwarePackageParserImpl(SoftwarePackageParserImpl const& other) = delete;
    /// @brief delete copy asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10437
    /// @needwork = dda
    /// @endcode
    SoftwarePackageParserImpl& operator=(SoftwarePackageParserImpl const& other) = delete;
    /// @brief delete move construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10438
    /// @needwork = dda
    /// @endcode
    SoftwarePackageParserImpl(SoftwarePackageParserImpl&& other) = delete;
    /// @brief delete move asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10439
    /// @needwork = dda
    /// @endcode
    SoftwarePackageParserImpl& operator=(SoftwarePackageParserImpl&& other) = delete;

    /// @brief Parse the extracted software package
    ///
    /// @param extractionDir Full file path to the extraction directory
    ///
    /// @returns The parsed SoftwarePackage
    /// @throws no
    /// @code{.isoft}
    /// <inheritdoc>
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10440
    /// @needwork = dda
    /// @endcode
    std::unique_ptr< SoftwarePackage > Parse(AraString const& extractionDir) override;

private:
    /// @brief Parser for the software cluster manifest file
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10441
    /// @needwork = dda
    /// @endcode
    SoftwareClusterManifestParser swclManifestParser_;
    /// @brief Parser for the software package manifest file
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10442
    /// @needwork = dda
    /// @endcode
    SoftwarePackageManifestParser swpkgManifestParser_;
};

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // ARA_UCM_PKGMGR_PARSING_SOFTWARE_PACKAGE_PARSER_IMPL_H_
