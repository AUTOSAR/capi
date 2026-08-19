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
/// @file       delta_update_action.cpp
/// @brief      delta update action implementation
/// @details
/// @date       2024-08-15
/// @author     cuiyinli
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/UCM/SoftwareClusterManager
/// @interface_level=unit
/// @trace_id_sr=SR_UCM_00001
/// @unit_name=DeltaUpdateAction
/// @unit_description=Action for delta update of a software package
/// @endcode
///
/// ================================================================

#include "delta_update_action.h"

#include "ara/ucm/internal/extraction/filesystem.h"
#include "ara/ucm/internal/extraction/tinyfsys.h"
#include "ara/ucm/pkgmgr/impl_type_resulttype.h"
#include "common/errc.h"
#include "common/log.h"
#include "common/path.h"
#include "common/strtype.h"
#include "filesystem_swcl_manager.h"
#include "parsing/software_cluster_manifest_parser.h"
#include "parsing/software_package.h"
#include "util/rjson.h"
#include "util/string_ext.h"

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @brief PlaceToDestinationPath
/// @return result
/// @throws no
AraResultVoid DeltaUpdateAction::PlaceToDestinationPath()
{
    // Information of the software cluster to be updated to
    // Applicable version of SoftwareCluster on which to apply delta shall be included into related SoftwarePackage’s
    // deltaPackageApplicableVersion attribute.

    // Check the current software set status
    // Information of the currently existing software cluster
    SwClusterStateType swclState;
    AraString curVersionStr;
    AraString newVersionStr;
    bool const ret{FileSystemSWCLManager::GetSwclStatus(swclName_, swclState, curVersionStr, newVersionStr)};
    if (!ret) {
        LOGE << "GetSwclStatus failed";
    }

    LOGD << "got status info, swclName:" << swclName_.c_str()
         << " swclState:" << FileSystemSWCLManager::SwclStateToStr(swclState).c_str()
         << " curVersionStr:" << curVersionStr.c_str() << " newVersionStr:" << newVersionStr.c_str()
         << " deltaPackageApplicableVersion:" << deltaPackageApplicableVersion_.c_str();

    // deltaPackageApplicableVersion
    // This attribute identifies the version of the included SoftwareCluster for which the enclosing SoftwarePackage can
    // be used as a delta update
    if (SoftwarePackage::IsInvalidDeltaPackage(deltaPackageApplicableVersion_, curVersionStr)) {
        // ProcessSwPackage shall raise the error ApplicationError IncompatibleDelta if delta package dependency fails
        // at processing.
        ReturnVoidErrcEnumWithLongLog("IsInvalidDeltaPackage", kIncompatibleDelta);
    }

    // Get the directory where the current version of the software cluster is located
    AraString curSwclVersionDir{targetPath_};
    std::ignore = strutil::ReplaceLast(curSwclVersionDir, swclVersion_, curVersionStr);

    // First copy files from the current version
    LOGD << "copy curSwclVersionDir:" << curSwclVersionDir.c_str() << " to targetPath_:" << targetPath_.c_str();

    AraResultVoid retCopy{tinyfsys::CopyDirectory(curSwclVersionDir, targetPath_)};
    if (!retCopy.HasValue()) {  // TODO: Distinguish between insufficient memory and other errors
        ReturnVoidErrcEnumWithLongLog("copy failed:" << retCopy.Error().Message().data(), kInsufficientMemory);
    }

    // every package replaces the existing software cluster during processing. For delta package type packages the
    // install steps shall be applied on the existing software cluster on the NVM, i.e. they shall be merged.
    // Overwrite the target directory (including var files) with the decompressed directory of the software set delta package
    AraString const& deltaExtractDir{package_->GetExtractionPath()};
    LOGD << "copy deltaExtractDir:" << deltaExtractDir.c_str() << " to targetPath_";

    retCopy = tinyfsys::OverwriteCopyDirectory(deltaExtractDir, targetPath_);
    // Filesystem fs;.
    // retCopy = fs.OverwriteCopyDirectory(deltaExtractDir, targetPath_);.
    if (!retCopy.HasValue()) {  // TODO: Distinguish between insufficient memory and other errors
        ReturnVoidErrcEnumWithLongLog("copy failed:" << retCopy.Error().Message().data(), kInsufficientMemory);
    }

    // if it is delta package, remove these files which are not in swcl manifest file
    if (package_->IsValidDeltaPackage()) {
        LOGD << "_removeOldFiles for delta package";
        _removeOldFiles();
    }

    // Parse the content of the current version software set description file
    AraString const curSwclVersionManifestPath{GetPath().GetSwclManifestPath(swclName_, curVersionStr)};
    SoftwareClusterManifestParser parser;
    std::unique_ptr< SoftwareClusterManifest > const curSwclManifest{parser.ParseFromFile(curSwclVersionManifestPath)};
    AraVectorString const& curExes{curSwclManifest->containedExecutables};
    AraVectorString const& curProcs{curSwclManifest->containedProcesses};

    // Parse the content of the new version software set description file, exe, process
    AraString const newDeltaSwclVersionManifestPath{targetPath_ + "/" + kSwclManifestFileName};
    std::unique_ptr< SoftwareClusterManifest > const newDeltaSwclManifest{
        parser.ParseFromFile(newDeltaSwclVersionManifestPath)};
    AraVectorString& newDeltaExes{newDeltaSwclManifest->containedExecutables};
    AraVectorString& newDeltaProcs{newDeltaSwclManifest->containedProcesses};

    // exe and process contained in the new version
    AraVectorString newExes;
    AraVectorString newProcs;
    std::ignore = std::set_union(curExes.begin(), curExes.end(), newDeltaExes.begin(), newDeltaExes.end(),
                                 std::back_inserter(newExes));
    std::ignore = std::set_union(curProcs.begin(), curProcs.end(), newDeltaProcs.begin(), newDeltaProcs.end(),
                                 std::back_inserter(newProcs));

    LOGD << "curExes:" << strutil::JoinVector(curExes).c_str() << "curProcs:" << strutil::JoinVector(curProcs).c_str()
         << "newDeltaExes:" << strutil::JoinVector(newDeltaExes).c_str()
         << "newDeltaProcs:" << strutil::JoinVector(newDeltaProcs).c_str()
         << "newExes:" << strutil::JoinVector(newExes).c_str() << "newProcs:" << strutil::JoinVector(newProcs).c_str();

    // Save the new exe and process lists
    rjson::Doc doc;
    bool const openRet{rjson::OpenAndCheck(doc, newDeltaSwclVersionManifestPath, std::move(LOGD))};
    if (!openRet) {
        return AraResultVoid(UCMErrorDomainErrc::kIncompatibleDelta);
    }

    if (doc.HasMember(kContainedExecutables)) {
        std::ignore = doc.RemoveMember(kContainedExecutables);
    }
    rjson::Value exesDom{rjson::MakeArray()};
    for (AraString const& it : newExes) {
        rjson::PushS(doc, exesDom, it);
    }
    rjson::AddKO(doc, doc, kContainedExecutables, exesDom);

    if (doc.HasMember(kContainedProcesses)) {
        std::ignore = doc.RemoveMember(kContainedProcesses);
    }
    rjson::Value procsDom{rjson::MakeArray()};
    for (AraString const& it : newProcs) {
        rjson::PushS(doc, procsDom, it);
    }
    rjson::AddKO(doc, doc, kContainedProcesses, procsDom);

    rjson::SavePretty(doc, newDeltaSwclVersionManifestPath);

    return PlaceToOSPatition();
}

/// @brief _removeOldFiles
/// @throws no
void DeltaUpdateAction::_removeOldFiles() const
{
    AraMap< AraString, AraString > const& swclFiles{package_->GetSoftwareCluster().GetSwclManifest().files};

    for (AraString const& fpath : tinyfsys::GetFilesRecursiveRelative(targetPath_)) {
        AraString const fname{tinyfsys::ExtractFileName(fpath)};

        const bool iSSig{kSwpkgManifestSigFileName == fname};
        if ((kSwpkgManifestFileName == fname) || iSSig) {
            continue;
        }

        if (swclFiles.find(fpath) == swclFiles.cend()) {
            LOGE << fpath.c_str() << " is not specified in swclManifest.Files, remove the old file";
            std::ignore = tinyfsys::RemoveFile(fpath);  //if no files in some subdir, remove the subdir//mytodo////
        }
    }
}

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara
