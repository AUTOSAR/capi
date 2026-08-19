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
/// @file       update_action.cpp
/// @brief      update action implementation
/// @details
/// @date       2022-01-01
/// @author     cuiyinli
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/UCM/SoftwareClusterManager
/// @interface_level=unit
/// @trace_id_sr=SR_UCM_00001
/// @unit_name=UpdateAction
/// @unit_description=Action for update of a software package
/// @endcode
///
/// ================================================================

#include "update_action.h"

#include "ara/ucm/internal/extraction/tinyfs.h"
#include "ara/ucm/internal/extraction/tinyfsys.h"
#include "ara/ucm/pkgmgr/impl_type_resulttype.h"
#include "common/const.h"
#include "common/errc.h"
#include "common/log.h"
#include "filesystem_swcl_manager.h"

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @brief Execute
/// @return result
/// @throws no
AraResult< ResultType > UpdateAction::Execute()
{
    LOGD << "call... " << ShortInfo().c_str();

    // Get the status information of the current software cluster
    {
        // Information of the software cluster to be updated to
        Version const swclVersion{swclVersion_};

        // Information of the currently existing software cluster
        SwClusterStateType swclState;
        AraString curVersionStr;
        AraString newVersionStr;
        bool const ret{FileSystemSWCLManager::GetSwclStatus(swclName_, swclState, curVersionStr, newVersionStr)};
        if (ret) {
            //  If there is information,
            //      When swclState is kPresent, swclVersion > curVersion; continue
            //      When swclState is kUpdated, swclVersion == newVersion; continue
            //  In other cases, exit the update
            if (curVersionStr.empty()) {
                curVersionStr = kEmptyVersion;
            }
            if (newVersionStr.empty()) {
                newVersionStr = kEmptyVersion;
            }

            Version const curVersion{curVersionStr};
            Version const newVersion{newVersionStr};
            if ((swclState == SwClusterStateType::kPresent) && (swclVersion > curVersion)) {
                LOGD << "update curVer:" << curVersionStr.c_str() << " to swclVer:" << swclVersion_.c_str();
            } else if ((swclState == SwClusterStateType::kUpdated) && (swclVersion == newVersion)) {
                LOGD << "swclVer == newVer, we continue the uncompleted update";
            } else {
                ReturnErrcEnumWithLongLog("failed to compare curVer,newVer,swclVer: OldVersion", ResultType,
                                          kOldVersion);
            }
        } else {  // No information
            // Handle the case where UpdateSwclStatus fails and is redone:
            // At this time, there must be a subdirectory of the original version under the software set directory
            LOGD << "we want to update the swcl, but no status file found";

            AraVectorString const swclVersionList{tinyfsys::GetSubdirectories(swclPath_)};
            if (swclVersionList.empty()) {  // No subdirectories
                ReturnErrcEnumWithLongLog("no swcl version sub dir found, do nothing", ResultType,
                                          kSoftwareClusterMissing);
            }

            // The software set has multiple versions, can update
            // TODO: If kProcessing state can be redone, test this branch
            LOGD << "more than one swcl version sub dir found, we continue the uncompleted update";
        }
    }

    // First try to clear the target directory
    ///AraResultVoid ret{std::move(tinyfsys::RemoveDirectory(targetPath_))};// mytodo666///err///////////////////
    AraResultVoid ret;
    ret = tinyfsys::RemoveDirectory(targetPath_);
    if (!ret.HasValue()) {
        LOGW << "remove old version sub dir failed, err:" << ret.Error().Message().data();
    }

    // Place the new version software set into its target installation directory (including the var directory)
    // Move the contents of the SWCL decompressed directory (package_->GetExtractionPath) to its location (targetPath_) (Note: the decompressed directory named by id is empty but not deleted at this time)
    // Move contents of SWCL to its location (to e.g. /var/lib/apd_ucm/SWCL_TEST1_VENDOR_A)
    ret = PlaceToDestinationPath();
    if (!ret.HasValue()) {
        ReturnErrcWithLongLog("PlaceToDestinationPath failed", ResultType, GetAraResultErrc(ret));
    }

    /// set the timestamp of the action
    std::int64_t const timestamp{
        std::chrono::duration_cast< std::chrono::milliseconds >(std::chrono::system_clock::now().time_since_epoch())
            .count()};
    actionTimeStamp_ = static_cast< std::uint64_t >(timestamp);

    // Consistency check after processing
    if (false == _CheckConsistency()) {
        // Clear the target directory
        LOGD << "Failed to CheckConsistency, so try to RemoveDirectory targetPath_:" << targetPath_.c_str();
        std::ignore = tinyfsys::RemoveDirectory(targetPath_);
        ReturnErrcEnumWithLongLog("CheckConsistency failed", ResultType, kProcessedSoftwarePackageInconsistent);
    }

    allowCommitOrRevert_ = true;
    LOGD << "success, timestamp:" << actionTimeStamp_;

    // Modify the status information of this SoftwareCluster to kUpdated
    FileSystemSWCLManager::UpdateSwclStatus(swclName_, SwClusterStateType::kUpdated, _GetCurrentVersion(),
                                            swclVersion_);
    resolution_ = ResultType::kSuccessfull;
    return AraResult< ResultType >({resolution_});
}

/// @brief RevertChanges
/// @return result
/// @throws no
AraResult< ResultType > UpdateAction::RevertChanges()
{
    LOGD << "call... " << ShortInfo().c_str();

    // security to avoid a RevertChanges() Call in case the Execute() has failed or the CommitChange() has succeeded
    if (allowCommitOrRevert_) {
        // Try to clean up persistent data
        std::ignore = _CleanUpPersistentData(swclVersion_, curSwclVersion_);

        // Invalidate the new version of the OS
        std::ignore = InvalidOSPatition(swclVersion_);

        // InstallAction::RevertChanges also has AraResultVoid removeDirError =.
        // fs_.RemoveDirectory(targetPath_)
        // Currently, the main logic of UpdateAction::RevertChanges and InstallAction::RevertChanges is the same
        // Delete the installation directory of the new version application
        AraResultVoid const rmRet{tinyfsys::RemoveDirectory(targetPath_)};
        if (!rmRet.HasValue()) {
            LOGE << "remove dir failed with error:" << rmRet.Error().Message().data();

            if (rmRet.CheckError(UcmFilesystemErrc::kNoSuchFileOrDirectory)) {
                LOGD << "kNoSuchFileOrDirectory is allowed, continue";
            } else {
                ReturnErrcEnumWithLongLog("unknown err", ResultType, kNotAbleToRevertPackages);
            }
        }

        allowCommitOrRevert_ = false;
        LOGD << "success";

        // Modify the status information of this SoftwareCluster to kPresent
        FileSystemSWCLManager::UpdateSwclStatus(swclName_, SwClusterStateType::kPresent, _GetCurrentVersion(), "");
        resolution_ = ResultType::kFailed;  // Note: Successfully completed Revert, set resolution_ to kFailed
        return AraResult< ResultType >({resolution_});
    }

    ReturnErrcEnumWithLongLog("Execute() failed previously", ResultType, kNotAbleToRevertPackages);
}

/// @brief CommitChanges
/// @return result
/// @throws no
AraResult< ResultType > UpdateAction::CommitChanges()
{
    LOGD << "call... " << ShortInfo().c_str();

    // security to avoid a CommitChanges() Call in case the Execute() has failed or the RevertChange() has succeeded
    if (allowCommitOrRevert_) {
        // only remove unnecessary SWCLs in case the new directory exists
        if (!tinyfsys::DoesDirectoryExist(targetPath_)) {
            // please note, we still allowCommitOrRevert_ here
            ReturnErrcEnumWithLongLog("the new version not exist", ResultType, kProcessedSoftwarePackageInconsistent);
        }

        // Delete the remaining versions
        // remove all but the version which is installed by this action
        AraVectorString const swclVersionList{tinyfsys::GetSubdirectories(swclPath_)};
        for (AraString const& swclVersion : swclVersionList) {
            if (swclVersion == targetPath_) {
                continue;
            }

            // Try to clean up persistent data
            AraString const swclVersionStr{tinyfs::Basename(swclVersion)};
            LOGD << "get swclVersionStr:" << swclVersionStr.c_str();
            std::ignore = _CleanUpPersistentData(swclVersionStr, swclVersion_);

            AraResultVoid const rmRet{tinyfsys::RemoveDirectory(swclVersion)};
            bool const rmErrorIsNoSuchFile{rmRet.CheckError(UcmFilesystemErrc::kNoSuchFileOrDirectory)};
            bool const rmErrorIsRemoveDirectory{rmRet.CheckError(UcmFilesystemErrc::kRemoveDirectory)};
            if (rmErrorIsNoSuchFile || rmErrorIsRemoveDirectory) {
                ReturnErrcEnumWithLongLog("failed to remove " << swclVersion.c_str(), ResultType,
                                          kProcessedSoftwarePackageInconsistent);
            }

            LOGD << "success to remove " << swclVersion.c_str();
        }

        // Delete the var directory under the installation directory of the new version application
        AraVectorString const swclApps{tinyfsys::GetSubdirectories(targetPath_)};
        for (AraString const& swclAppDir : swclApps) {
            // var directory of the application in the installation package
            AraString const swclAppVarDir{swclAppDir + kPathSeparator + kAppVarDirName};

            // Delete the var directory in the installation directory
            AraResultVoid const rmRet{tinyfsys::RemoveDirectory(swclAppVarDir)};
            if (!rmRet.HasValue()) {
                LOGD << "remove swclAppVarDir:" << swclAppVarDir.c_str()
                     << " failed with error:" << rmRet.Error().Message().data();

                if (rmRet.CheckError(UcmFilesystemErrc::kNoSuchFileOrDirectory)) {
                    LOGD << "kNoSuchFileOrDirectory is allowed, continue";
                } else {
                    ReturnErrcEnumWithLongLog("unknown err", ResultType, kProcessedSoftwarePackageInconsistent);
                }
            }

            LOGD << "success to remove swclAppVarDir:" << swclAppVarDir.c_str();
        }

        allowCommitOrRevert_ = false;
        LOGD << "success";

        // Invalidate the old version of the OS
        if (!InvalidOSPatition(curSwclVersion_).HasValue()) {
            ReturnErrcEnumWithLongLog("failed to InvalidOSPatition for curSwclVersion_:" << curSwclVersion_.c_str(),
                                      ResultType, kProcessedSoftwarePackageInconsistent);
        }

        // Modify the status information of this SoftwareCluster to kPresent
        FileSystemSWCLManager::UpdateSwclStatus(swclName_, SwClusterStateType::kPresent, swclVersion_, "");
        resolution_ = ResultType::kSuccessfull;
        return AraResult< ResultType >({resolution_});
    }

    ReturnErrcEnumWithLongLog("Execute() failed previously", ResultType, kProcessedSoftwarePackageInconsistent);
}

/// @brief MergeVar
/// @return result
/// @throws no
AraResult< ResultType > UpdateAction::MergeVar()
{
    LOGD << "call... " << ShortInfo().c_str();
    resolution_ = ResultType::kSuccessfull;
    return AraResult< ResultType >({resolution_});
}

/// @brief PlaceToDestinationPath
/// @return result
/// @throws no
AraResultVoid UpdateAction::PlaceToDestinationPath()
{
    // TODO: Distinguish between insufficient memory and other errors
    AraString const extractPath{package_->GetExtractionPath()};
    if (!tinyfsys::MoveDirectory(extractPath, targetPath_).HasValue()) {
        ReturnVoidErrcEnumWithLongLog("failed to move " << extractPath.c_str() << " to " << targetPath_.c_str(),
                                      kInsufficientMemory);
    }

    return PlaceToOSPatition();
}

/// @brief PlaceToOSPatition
/// @return result
/// @throws no
AraResultVoid UpdateAction::PlaceToOSPatition() noexcept { return {}; }

/// @brief InvalidOSPatition
/// @param OSVersion
/// @return result
/// @throws no
AraResultVoid UpdateAction::InvalidOSPatition(AraString const& stOSVersion) noexcept
{
    std::ignore = stOSVersion;
    return {};
}

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara
