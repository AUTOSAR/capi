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
/// @file       manifest_parser.h
/// @brief      common manifest parser
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
/// @unit_name=ManifestParser
/// @unit_description=Common interface for manifest parsers
/// @endcode
///
/// ================================================================

#ifndef ARA_UCM_PKGMGR_PARSING_MANIFEST_PARSER_H_
#define ARA_UCM_PKGMGR_PARSING_MANIFEST_PARSER_H_

#include "common/alias.h"
#include "common/log.h"
#include "common/rjson_manifest.h"
#include "util/noncopy.h"

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @brief Common interface for manifest parsers.
///
/// This class implements common behavior that is true for all parsers.
///
/// @code{.isoft}
/// @tparam T The class of the parsed manifest, i.e. SoftwareClusterManifest or SoftwarePackageManifest
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10258
/// @trace_id_dd=DD_UCM_10448
/// @needwork = ad
/// @endcode
template < class T >
class ManifestParser : public NonCopyNonMove
{
public:
    /// @brief constructor
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10449
    /// @needwork = dda
    /// @endcode
    ManifestParser() = default;
    /// @brief destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10450
    /// @needwork = dda
    /// @endcode
    virtual ~ManifestParser() = default;

    /// @brief delete copy construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10451
    /// @needwork = dda
    /// @endcode
    ManifestParser(ManifestParser const& other) = delete;
    /// @brief delete copy asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10452
    /// @needwork = dda
    /// @endcode
    ManifestParser& operator=(ManifestParser const& other) = delete;
    /// @brief delete move construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10453
    /// @needwork = dda
    /// @endcode
    ManifestParser(ManifestParser&& other) = delete;
    /// @brief delete move asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10454
    /// @needwork = dda
    /// @endcode
    ManifestParser& operator=(ManifestParser&& other) = delete;

    /// @brief Parses a manifest
    ///
    /// @param manifest The manifest json
    ///
    /// @returns The parsed manifest object representing the json content
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10455
    /// @needwork = dda
    /// @endcode
    virtual std::unique_ptr< T > Parse(RManifestUPtr const& manifest) = 0;

    /// @brief Parses a manifest given by the absolute file path.
    ///
    /// @param manifestPath The absolute path to a manifest json
    ///
    /// @returns The parsed software cluster manifest object representing the json content
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10456
    /// @needwork = dda
    /// @endcode
    virtual std::unique_ptr< T > ParseFromFile(AraString const& manifestPath)
    {
        std::unique_ptr< T > ret{nullptr};
        RManifestOpenAndCheck(doc, manifestPath, ret);
        ret = std::move(Parse(doc));
        return ret;
    }
};

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // ARA_UCM_PKGMGR_PARSING_MANIFEST_PARSER_H_
