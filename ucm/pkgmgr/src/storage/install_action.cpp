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
/// @file       install_action.cpp
/// @brief      install action implementation
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
/// @unit_name=InstallAction
/// @unit_description=Action for installation of a software package
/// @endcode
///
/// ================================================================

#include "install_action.h"

#include "ara/ucm/internal/extraction/tinyfs.h"
#include "ara/ucm/internal/extraction/tinyfsys.h"
#include "ara/ucm/pkgmgr/impl_type_resulttype.h"
#include "common/alias.h"
#include "common/errc.h"
#include "common/log.h"
#include "filesystem_swcl_manager.h"

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @brief Execute
/// @return result with ResultType
/// @throws no
AraResult< ResultType > InstallAction::Execute()
{
    LOGD << "call... " << ShortInfo().c_str();

    // Get the status information of the current software cluster
    {
        SwClusterStateType swclState;
        AraString curVer;
        AraString newVer;
        bool ret{false};
        ret = FileSystemSWCLManager::GetSwclStatus(swclName_, swclState, curVer, newVer);
        if (ret) {
            // If there is information:
            // "state" is kAdded and "new_version" is the same as the current version indicates an interrupted installation; continue installation
            // Otherwise, prompt error: this software cluster already exists
            int const compareResult = swclVersion_.compare(newVer);
            if ((swclState == SwClusterStateType::kAdded) && (0 == compareResult)) {
                LOGD << "continue the uncompleted install";
            } else {
                ReturnErrcEnumWithLongLog("there has already been the swcl of curVersion:" << curVer.c_str(),
                                          ResultType, kInvalidPackageManifest);
            }
        } else {
            // No information:
            // Continue installation
            LOGD << "there is no status info, do install it";
        }
    }

    // First try to clear the target directory
    AraResultVoid const rmRet{tinyfsys::RemoveDirectory(swclPath_)};
    if (!rmRet.HasValue()) {
        LOGE << "remove dir failed with error:" << rmRet.Error() << rmRet.Error().Message().data();
    }

    // Create the swclPath_ directory
    // create a directory for SWCL (e.g. /var/lib/apd_ucm/SWCL_TEST1_VENDOR_A)
    // TODO: Distinguish between insufficient memory and other errors
    if (!tinyfs::CreateDirectory(swclPath_, kInt32_0755, false)) {
        ReturnErrcEnumWithLongLog("failed to create " << swclPath_.c_str(), ResultType, kInsufficientMemory);
    }

    // check if a directory for SWCL (e.g. /var/lib/apd_ucm/SWCL_TEST1_VENDOR_A) exists
    if (!tinyfsys::DoesDirectoryExist(swclPath_)) {
        ReturnErrcEnumWithLongLog("after create dir, no " << swclPath_.c_str(), ResultType,
                                  kProcessedSoftwarePackageInconsistent);
    }

    // Move the contents of the SWCL decompressed directory (package_->GetExtractionPath) to its location (targetPath_) (Note: the decompressed directory named by id is empty but not deleted at this time)
    // Move contents of SWCL to its location (to e.g. /var/lib/apd_ucm/SWCL_TEST1_VENDOR_A)
    // TODO: Distinguish between insufficient memory and other errors
    if (!tinyfsys::MoveDirectory(package_->GetExtractionPath(), targetPath_).HasValue()) {
        ReturnErrcEnumWithLongLog("failed to move ExtractionPath to finalDestinationPath", ResultType,
                                  kInsufficientMemory);
    }

    // set the timestamp of the action
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
    LOGD << "success, timestamp:" << actionTimeStamp_ << ", from " << package_->GetExtractionPath().c_str() << " to "
         << targetPath_.c_str();

    // Modify the status information of this SoftwareCluster to kAdded
    FileSystemSWCLManager::UpdateSwclStatus(swclName_, SwClusterStateType::kAdded, "", swclVersion_);
    resolution_ = ResultType::kSuccessfull;
    return AraResult< ResultType >({resolution_});
}

/// @brief RevertChanges
/// @return result with ResultType
/// @throws no
AraResult< ResultType > InstallAction::RevertChanges()
{
    LOGD << "call... " << ShortInfo().c_str();

    // security to avoid a RevertChanges() Call in case the Execute() has failed
    if (allowCommitOrRevert_) {
        // Try to clean up persistent data
        std::ignore = _CleanUpPersistentData(swclVersion_, "");

        // Delete the directory (with version information) of the newly installed SWCL
        // remove Directory of the newly installed SWCL and check for errors
        // Should the parent directory also be deleted here? Because InstallAction::Execute will create the parent directory? en
        // drop the "version" part from the path
        AraResultVoid const ret{tinyfsys::RemoveDirectory(swclPath_)};
        if (!ret.HasValue()) {
            LOGE << "remove dir failed with error:" << ret.Error() << ret.Error().Message().data();

            if (ret.CheckError(UcmFilesystemErrc::kNoSuchFileOrDirectory)) {
                LOGD << "kNoSuchFileOrDirectory is allowed, continue";
            } else {
                ReturnErrcEnumWithLongLog("unknown err", ResultType, kNotAbleToRevertPackages);
            }
        }

        allowCommitOrRevert_ = false;
        LOGD << "success";
    }

    resolution_ = ResultType::kFailed;  // Note: Successfully completed Revert, set resolution_ to kFailed
    return AraResult< ResultType >({resolution_});
}

/// @brief CommitChanges
/// @return result with ResultType
/// @throws no
AraResult< ResultType > InstallAction::CommitChanges()
{
    LOGD << "call... " << ShortInfo().c_str();

    // Did nothing (because there was no version of this swcl originally)
    // return {};

    // Modify the status information of this SoftwareCluster to kPresent
    FileSystemSWCLManager::UpdateSwclStatus(swclName_, SwClusterStateType::kPresent, swclVersion_, "");
    resolution_ = ResultType::kSuccessfull;
    return AraResult< ResultType >({resolution_});
}

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara
