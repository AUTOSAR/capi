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
/// @file       remove_action.cpp
/// @brief      Remove action implementation
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
/// @unit_name=RemoveAction
/// @unit_description=Remove action implementation
/// @endcode
///
/// ================================================================

#include "remove_action.h"

#include "ara/per/update.h"
#include "ara/ucm/internal/extraction/tinyfs.h"
#include "ara/ucm/internal/extraction/tinyfsys.h"
#include "ara/ucm/pkgmgr/impl_type_resulttype.h"
#include "common/errc.h"
#include "common/log.h"
#include "common/path.h"
#include "filesystem_swcl_manager.h"
#include "parsing/software_cluster_manifest_parser.h"
#include "util/string_ext.h"

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @brief Execute
/// @return result
/// @throws no
AraResult< ResultType > RemoveAction::Execute()
{
    LOGD << "call... " << ShortInfo().c_str();

    // swcl with category of framework core os can not be removed
    AraString const category{package_->GetSoftwareCluster().GetSwclManifest().category};

    const bool isFrameworkCategory    = (0 == category.compare(kIsoftFrameworkSWCLCategory));
    const bool isPlatformCoreCategory = (0 == category.compare(kPlatformCoreSWCLCategory));
    const bool isOSCategory           = (0 == category.compare(kIsoftOSSWCLCategory));
    if (isFrameworkCategory || isPlatformCoreCategory || isOSCategory) {
        ReturnErrcEnumWithLongLog("can't remove swcl with category" << category.c_str(), ResultType,
                                  kOperationNotPermitted);
    }
    // implementation is still missing

    // Did nothing
    // Get the status information of the current software cluster
    {
        SwClusterStateType swclState;
        AraString curVersion;
        AraString newVersion;
        bool const ret{FileSystemSWCLManager::GetSwclStatus(swclName_, swclState, curVersion, newVersion)};
        std::ignore = swclState;
        if (!ret) {
            LOGD << "we want to remove the swcl, but no status file found";

            // Handle the case where UpdateSwclStatus fails and is redone:
            // At this time, there must be a subdirectory of the original version under the software set directory
            AraVectorString const swclVersionList{tinyfsys::GetSubdirectories(swclPath_)};
            if (swclVersionList.empty()) {  // No subdirectories
                ReturnErrcEnumWithLongLog("no swcl version sub dir found, do nothing", ResultType,
                                          kSoftwareClusterMissing);
            }

            // The software set has multiple versions, need to delete
            // TODO: If kProcessing state can be redone, test this branch
            LOGD << "more than one swcl version sub dir found, we continue the uncompleted remove";
        }
    }

    /// set the timestamp of the action
    std::int64_t const timestamp{
        std::chrono::duration_cast< std::chrono::milliseconds >(std::chrono::system_clock::now().time_since_epoch())
            .count()};
    actionTimeStamp_ = static_cast< std::uint64_t >(timestamp);

    allowCommitOrRevert_ = true;
    LOGI << "success(really do nothing), timestamp:" << actionTimeStamp_;

    // Modify the status information of this SoftwareCluster to kRemoved
    FileSystemSWCLManager::UpdateSwclStatus(swclName_, SwClusterStateType::kRemoved, _GetCurrentVersion(true), "");
    resolution_ = ResultType::kSuccessfull;
    return AraResult< ResultType >({resolution_});
}

/// @brief RevertChanges
/// @return result
/// @throws no
AraResult< ResultType > RemoveAction::RevertChanges()
{
    LOGD << "call... " << ShortInfo().c_str();

    if (allowCommitOrRevert_) {
        allowCommitOrRevert_ = false;
        LOGI << "success";

        // Modify the status information of this SoftwareCluster to kPresent
        FileSystemSWCLManager::UpdateSwclStatus(swclName_, SwClusterStateType::kPresent, _GetCurrentVersion(true), "");
        resolution_ = ResultType::kFailed;  // Note: Successfully completed Revert, set resolution_ to kFailed
        return AraResult< ResultType >({resolution_});
    }

    ReturnErrcEnumWithLongLog("Execute() failed previously", ResultType, kNotAbleToRevertPackages);
}

/// @brief CommitChanges
/// @return result
/// @throws no
AraResult< ResultType > RemoveAction::CommitChanges()
{
    LOGD << "call... " << ShortInfo().c_str();

    // test to prevent swcl to be removed when RevertChanges() has been called after Execute()
    if (allowCommitOrRevert_) {
        // Try to clean up persistent data
        std::ignore = _cleanUpPersistData();

        // Delete the directory where swcl is located (which also contains version information subdirectories) (now Remove does not distinguish version information)
        // Remove the version information from the path
        // InstallAction::Execute also has: targetPath_.substr(0, targetPath_.find_last_of("/")).
        // remove the swcl directory from its final location
        AraResultVoid const rmRet{tinyfsys::RemoveDirectory(swclPath_)};
        if (!rmRet.HasValue()) {
            LOGE << "remove dir failed with error:" << rmRet.Error().Message().data();

            if (rmRet.CheckError(UcmFilesystemErrc::kNoSuchFileOrDirectory)) {
                LOGD << "kNoSuchFileOrDirectory is allowed, continue";
            } else {
                ReturnErrcEnumWithLongLog("unknown err", ResultType, kGeneralReject);
            }
        }

        allowCommitOrRevert_ = false;
        LOGD << "success";

        resolution_ = ResultType::kSuccessfull;
        return AraResult< ResultType >({resolution_});
    }

    ReturnErrcEnumWithLongLog("Execute() failed previously", ResultType, kGeneralReject);
}

/// @brief Clean up persistent data
/// @brief GetActionType
/// @return result
/// @throws no
AraResult< ResultType > RemoveAction::_cleanUpPersistData() const
{
    AraString const swclManifestPath{GetPath().GetSwclManifestPath(swclName_, curSwclVersion_)};
    SoftwareClusterManifestParser parser;
    std::unique_ptr< SoftwareClusterManifest > const swclManifest{parser.ParseFromFile(swclManifestPath)};
    if (swclManifest == nullptr) {
        LOGE << "failed parse swcl manifest " << swclManifestPath.c_str();
        return AraResult< ResultType >{ResultType::kFailed};
    }

    AraString const culSwclVersionPath{GetPath().GetSwclInstallDir(swclName_, curSwclVersion_)};

    // Check the processes contained in the software set
    for (AraString const& processFQN : swclManifest->containedProcesses) {
        // Only take the last part of the process name
        AraString const processName{strutil::GetLastComponent(processFQN)};
        // Check if the process exists in the etc directory
        AraString const processPath{culSwclVersionPath + kPathSeparator + kETC_DIR_NAME + kPathSeparator
                                    + processName};  ////
        bool const ret{ara::per::ClearUpPersistency(processPath, "")};
        LOGD << "clear processPath:" << processPath.c_str() << ", ret:" << ret;
    }

    return AraResult< ResultType >{ResultType::kSuccessfull};
}

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara
