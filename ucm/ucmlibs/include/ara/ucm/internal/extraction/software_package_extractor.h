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
/// @file       software_package_extractor.h
/// @brief      The SoftwarePackageExtractor Interface for software package extraction.
/// @details
/// @date       2022-06-13
/// @author     hanzhibo
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @export_level=/UCM/UCMLib
/// @module_path=/UCM/UCMLib
/// @interface_level=module
/// @trace_id_sr=SR_UCM_00025
/// @unit_name=SoftwarePackageExtractor
/// @unit_description=SoftwarePackageExtractor definition provided for UCM
/// @endcode
///
/// ================================================================

#ifndef ARA_UCM_PKGMGR_EXTRACTION_SOFTWARE_PACKAGE_EXTRACTOR_H_
#define ARA_UCM_PKGMGR_EXTRACTION_SOFTWARE_PACKAGE_EXTRACTOR_H_

#include "alias.h"

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @brief Interface for software package extraction.
///
/// @code{.isoft}
/// @uptrace={SWS_UCM_00001, a665954b7a62b2349db109a9408efe084b8a7136}
/// @interface_level=module
/// @trace_id_ad=AD_UCM_00005
/// @trace_id_dd=DD_UCM_00108
/// @needwork = ad
/// @endcode
class SoftwarePackageExtractor
{
public:
    /// @brief constructor
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_UCM_00005
    /// @trace_id_dd=DD_UCM_00109
    /// @needwork = dda
    /// @endcode
    SoftwarePackageExtractor() = default;
    /// @brief destructor
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_UCM_00005
    /// @trace_id_dd=DD_UCM_00110
    /// @needwork = dda
    /// @endcode
    virtual ~SoftwarePackageExtractor() = default;

    /// @brief delete copy construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_UCM_00005
    /// @trace_id_dd=DD_UCM_00111
    /// @needwork = dda
    /// @endcode
    SoftwarePackageExtractor(SoftwarePackageExtractor const& other) = delete;
    /// @brief delete copy asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_UCM_00005
    /// @trace_id_dd=DD_UCM_00112
    /// @needwork = dda
    /// @endcode
    SoftwarePackageExtractor& operator=(SoftwarePackageExtractor const& other) = delete;
    /// @brief delete move construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_UCM_00005
    /// @trace_id_dd=DD_UCM_00113
    /// @needwork = dda
    /// @endcode
    SoftwarePackageExtractor(SoftwarePackageExtractor&& other) = delete;
    /// @brief delete move asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_UCM_00005
    /// @trace_id_dd=DD_UCM_00114
    /// @needwork = dda
    /// @endcode
    SoftwarePackageExtractor& operator=(SoftwarePackageExtractor&& other) = delete;

    /// @brief Extract provided archive into the provided directory.
    ///
    /// If the extraction was unsuccessful any partially extracted files/folders are deleted.
    ///
    /// @param archivePath The full file path to the archive
    /// @param extractionDir The full path to the target directory.
    ///
    /// @returns True if archive was successfully extracted, else false
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_UCM_00005
    /// @trace_id_dd=DD_UCM_00115
    /// @needwork = dda
    /// @endcode
    virtual bool Extract(AraString const& archivePath, AraString const& extractionDir) = 0;

    /// @brief Cleanup of extracted files.
    ///
    /// This deletes any files or folders which have been created as part of the last extraction
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_UCM_00005
    /// @trace_id_dd=DD_UCM_00116
    /// @needwork = dda
    /// @endcode
    virtual void Cleanup() = 0;
};

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // ARA_UCM_PKGMGR_EXTRACTION_SOFTWARE_PACKAGE_EXTRACTOR_H_
