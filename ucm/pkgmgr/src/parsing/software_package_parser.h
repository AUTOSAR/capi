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
/// @file       software_package_parser.h
/// @brief      software package parser
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
/// @unit_name=SoftwarePackageParser
/// @unit_description=software package parser
/// @endcode
///
/// ================================================================

#ifndef ARA_UCM_PKGMGR_PARSING_SOFTWARE_PACKAGE_PARSER_H_
#define ARA_UCM_PKGMGR_PARSING_SOFTWARE_PACKAGE_PARSER_H_

#include <memory>

#include "common/alias.h"
#include "software_package.h"
#include "util/noncopy.h"

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @brief Parser for a software package.
///
/// This class encodes how a software package is represented on the filesystem
/// which is implementation specific (not specified).
///
/// @code{.isoft}
/// @uptrace={SWS_UCM_00001, a665954b7a62b2349db109a9408efe084b8a7136}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10262
/// @trace_id_dd=DD_UCM_10473
/// @needwork = ad
/// @endcode
class SoftwarePackageParser : NonCopyNonMove
{
public:
    /// @brief constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10474
    /// @needwork = dda
    /// @endcode
    SoftwarePackageParser() = default;
    /// @brief destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10475
    /// @needwork = dda
    /// @endcode
    virtual ~SoftwarePackageParser() = default;

    /// @brief delete copy construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10476
    /// @needwork = dda
    /// @endcode
    SoftwarePackageParser(SoftwarePackageParser const& other) = delete;
    /// @brief delete copy asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10477
    /// @needwork = dda
    /// @endcode
    SoftwarePackageParser& operator=(SoftwarePackageParser const& other) = delete;
    /// @brief delete move construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10478
    /// @needwork = dda
    /// @endcode
    SoftwarePackageParser(SoftwarePackageParser&& other) = delete;
    /// @brief delete move asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10479
    /// @needwork = dda
    /// @endcode
    SoftwarePackageParser& operator=(SoftwarePackageParser&& other) = delete;

    /// @brief Parse the extracted software package
    ///
    /// @param extractionDir Full file path to the extraction directory
    ///
    /// @returns The parsed SoftwarePackage
    /// @throws no
    ///
    /// @code{.isoft}
    /// @uptrace={SWS_UCM_00029, 734d4556fd4ea60d429916dc074a4057f12ab71f}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10480
    /// @needwork = dda
    /// @endcode
    virtual std::unique_ptr< SoftwarePackage > Parse(AraString const& extractionDir) = 0;
};

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // ARA_UCM_PKGMGR_PARSING_SOFTWARE_PACKAGE_PARSER_H_
