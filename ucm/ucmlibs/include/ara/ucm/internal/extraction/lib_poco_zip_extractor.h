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
/// @file       lib_poco_zip_extractor.h
/// @brief      The LibPocoZipExtractor
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
/// @unit_name=LibPocoZipExtractor
/// @unit_description=LibPocoZipExtractor definition
/// @endcode
///
/// ================================================================

#ifndef ARA_UCM_PKGMGR_EXTRACTION_LIB_POCO_ZIP_EXTRACTOR_H_
#define ARA_UCM_PKGMGR_EXTRACTION_LIB_POCO_ZIP_EXTRACTOR_H_

#include "alias.h"
#include "filesystem.h"
#include "software_package_extractor.h"

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @brief Archive extraction
///
/// @code{.isoft}
/// @uptrace={SWS_UCM_00001, a665954b7a62b2349db109a9408efe084b8a7136}
/// @interface_level=module
/// @trace_id_ad=AD_UCM_00005
/// @trace_id_dd=DD_UCM_00060
/// @needwork = dd
/// @endcode
class LibPocoZipExtractor : public SoftwarePackageExtractor
{
public:
    /// @brief constructor
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_UCM_00005
    /// @trace_id_dd=DD_UCM_00061
    /// @needwork = dda
    /// @endcode
    LibPocoZipExtractor() = default;

    /// @brief Create a new instance of the extractor.
    ///
    /// @param fs The reference to the filesystem
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_UCM_00005
    /// @trace_id_dd=DD_UCM_00062
    /// @needwork = dda
    /// @endcode
    explicit LibPocoZipExtractor(Filesystem const& fs);

    /// @brief Extract the provided archive into the provided directory.
    /// @param archivePath The full file path to the archive
    /// @param extractionDir The full path to the target directory
    /// @return True if the archive was successfully extracted, else false
    /// @throws no
    /// @code{.isoft}
    /// @copybrief SoftwarePackageExtractor::Extract()
    ///
    /// The last folder of the @p extractionDirectory must not yet exist and
    /// will be created during extraction and deleted during cleanup.
    /// If extraction fails (i.e. false is returned) the extraction directory @p
    /// extractionDirectory is deleted.
    ///
    /// @copydetails SoftwarePackageExtractor::Extract()
    /// @interface_level=module
    /// @trace_id_ad=AD_UCM_00005
    /// @trace_id_dd=DD_UCM_00063
    /// @needwork = dda
    /// @endcode
    bool Extract(AraString const& archivePath, AraString const& extractionDir) override;

    /// @brief Cleanup of extracted files.
    /// This deletes any files or folders which have been created as part of the last extraction
    /// @throws no
    /// @code{.isoft}
    /// @copybrief SoftwarePackageExtractor::Cleanup()
    ///
    /// For this implementation the cleanup is simplified to just deleting the
    /// extractionDirectory
    /// @interface_level=module
    /// @trace_id_ad=AD_UCM_00005
    /// @trace_id_dd=DD_UCM_00064
    /// @needwork = dda
    /// @endcode
    void Cleanup() override;

private:
    /// @brief Callback function for successful decompression
    /// @param extractionDir The directory where files were extracted
    /// @return True if decompression was successful, else false
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00005
    /// @trace_id_dd=DD_UCM_00065
    /// @needwork = dda
    /// @endcode
    static bool OnDecompressOk(AraString const& extractionDir);

private:
    /// @brief Stores the last extraction directory for cleanup
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00005
    /// @trace_id_dd=DD_UCM_00066
    /// @needwork = dda
    /// @endcode
    AraString lastExtractionDir_;
};

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // ARA_UCM_PKGMGR_EXTRACTION_BUSYBOX_ARCHIVE_EXTRACTOR_H_
