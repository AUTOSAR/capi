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
/// @file       lib_poco_zip_extractor.cpp
/// @brief      The LibPocoZipExtractor
/// @details
/// @date       2024-10-26
/// @author     cuiyinli
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/UCM/UCMLib
/// @interface_level=module
/// @trace_id_sr=SR_UCM_00025
/// @unit_name=LibPocoZipExtractor
/// @unit_description=Provide PocoZip decompression functionality
/// @endcode
///
/// ================================================================

#include "ara/ucm/internal/extraction/lib_poco_zip_extractor.h"

#include <nai/os/nai_file.h>

#include "ara/ucm/internal/extraction/tinyfs.h"

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_00005
/// @trace_id_dd=DD_UCM_00065
/// @needwork = dd
/// @endcode
constexpr std::int32_t kMode0755{493};  // 0755

/// @brief SIsInVector
/// @param vec
/// @param item
/// @return bool
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_00005
/// @trace_id_dd=DD_UCM_00065
/// @needwork = dd
/// @endcode
inline static bool SIsInVector(AraVectorString const& vec, AraString const& item) noexcept
{
    for (AraString const& it : vec) {
        if (it == item) {
            return true;
        }
    }
    return false;
}

/// @brief Create a new instance of the extractor.
/// @param fs The reference to the filesystem
/// @throws no
LibPocoZipExtractor::LibPocoZipExtractor(Filesystem const& fs) : SoftwarePackageExtractor{}, lastExtractionDir_{}
{
    std::ignore = fs;
}

/// @brief Extract
/// @param archivePath
/// @param extractionDir
/// @return bool
/// @throws no
bool LibPocoZipExtractor::Extract(AraString const& archivePath, AraString const& extractionDir)
{
    lastExtractionDir_ = extractionDir;

    AraResultVoid const ret{tinyfsys::Unzip(archivePath, extractionDir)};
    if (!ret.HasValue()) {
        Cleanup();
        return false;
    }

    // set mode, do not check error
    return OnDecompressOk(extractionDir);
}

/// @brief Cleanup
/// @throws no
void LibPocoZipExtractor::Cleanup()
{
    if (!lastExtractionDir_.empty()) {
        if (tinyfsys::DoesDirectoryExist(lastExtractionDir_)) {
            std::ignore = tinyfsys::RemoveDirectory(lastExtractionDir_);
        }
    }
}

/// @brief OnDecompressOk
/// @param extractionDir
/// @return bool
/// @throws no
bool LibPocoZipExtractor::OnDecompressOk(AraString const& extractionDir)
{
    // add 0755 to files in extractionDir's sub dir named bin and sbin
    AraVectorString const specialDirs{AraString("bin"), AraString("sbin")};
    AraVectorString const specialFiles{AraString("updateOS")};
    AraString name;

    for (AraString const& subDir : tinyfsys::GetSubdirectories(extractionDir)) {
        name = tinyfs::Basename(subDir);
        if (SIsInVector(specialDirs, name)) {
            for (AraString const& subFile : tinyfsys::GetFiles(subDir)) {
                if (nai_path_set_mode(subFile.c_str(), kMode0755) < 0) {
                    return false;
                }
            }
        }
    }

    // add 0755 to files in extractionDir's sub file named updateOS
    for (AraString const& subFile : tinyfsys::GetFiles(extractionDir)) {
        name = tinyfs::Basename(subFile);
        if (SIsInVector(specialFiles, name)) {
            return nai_path_set_mode(subFile.c_str(), kMode0755)
                   >= 0;  /////////OS update needs testing////////mytodo666////
        }
    }

    return true;
}

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara
