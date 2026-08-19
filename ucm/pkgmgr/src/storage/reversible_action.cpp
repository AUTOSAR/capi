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
/// @file       reversible_action.cpp
/// @brief      Reversible action implementation
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
/// @unit_name=ReversibleAction
/// @unit_description=Reversible action implementation
/// @endcode
///
/// ================================================================

#include "reversible_action.h"

#include "ara/per/update.h"
#include "ara/ucm/internal/extraction/tinyfs.h"
#include "ara/ucm/internal/extraction/tinyfsys.h"
#include "common/log.h"
#include "common/path.h"
#include "parsing/software_cluster_manifest_parser.h"

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @brief CheckConsistency
/// @return bool
/// @throws no
bool ReversibleAction::_CheckConsistency() const
{
    bool isConsistent{true};

    // TODO: Check OS software set
    if (package_->IsOSPackage(swclName_)) {
        LOGI << "todo when os update";
        return isConsistent;
    }

    AraResultVoid const ret{package_->CheckConsistency4Swcl()};
    if (!ret.HasValue()) {
        LOGE << "check consis failed, err:" << ret.Error().Message().data();
        isConsistent = false;
    }

    LOGD << "end with isConsistent:" << isConsistent;
    return isConsistent;
}

AraResult< ResultType > ReversibleAction::_CleanUpPersistentData(AraString const& removingVersion,
                                                                 AraString const& keptVersion)
{
    LOGD << "ReversibleAction::_CleanUpPersistentData, begin with removingVersion:" << removingVersion.c_str()
         << "keptVersion:" << keptVersion.c_str();

    SoftwareClusterManifestParser parser;

    // Get information of the software set to be deleted
    AraString removingSwclVersionPath{};
    AraVectorString removingProcesses{};
    if (!removingVersion.empty()) {
        // Manifest path of the software set to delete
        AraString const removingSwclManifestPath{GetPath().GetSwclManifestPath(swclName_, removingVersion)};
        LOGD << "ReversibleAction::_CleanUpPersistentData, get removingSwclManifestPath:"
             << removingSwclManifestPath.c_str();
        std::unique_ptr< SoftwareClusterManifest > const removingSwclManifest{
            parser.ParseFromFile(removingSwclManifestPath)};
        if (removingSwclManifest == nullptr) {
            LOGE << "failed parse removingSwclManifest " << removingSwclManifestPath.c_str();
            return AraResult< ResultType >{ResultType::kFailed};
        }

        // Version path of the software set to delete
        removingSwclVersionPath = GetPath().GetSwclInstallDir(swclName_, removingVersion);
        LOGD << "ReversibleAction::_CleanUpPersistentData, get removingSwclVersionPath:"
             << removingSwclVersionPath.c_str();

        // Processes in the software set to delete
        removingProcesses = removingSwclManifest->containedProcesses;
        LOGD << "ReversibleAction::_CleanUpPersistentData, get removingProcesses:"
             << strutil::JoinVector(removingProcesses).c_str();
    }

    // Get information of the retained software set
    AraString keptSwclVersionPath{};
    AraVectorString keptProcesses{};
    if (!keptVersion.empty()) {
        // Manifest path of the retained software set
        AraString const keptSwclManifestPath{GetPath().GetSwclManifestPath(swclName_, keptVersion)};
        LOGD << "ReversibleAction::_CleanUpPersistentData, get keptSwclManifestPath:" << keptSwclManifestPath.c_str();
        std::unique_ptr< SoftwareClusterManifest > const keptSwclManifest{parser.ParseFromFile(keptSwclManifestPath)};
        if (keptSwclManifest == nullptr) {
            LOGE << "failed parse keptSwclManifest " << keptSwclManifestPath.c_str();
            return AraResult< ResultType >{ResultType::kFailed};
        }

        // Version path of the retained software set
        keptSwclVersionPath = GetPath().GetSwclInstallDir(swclName_, keptVersion);
        LOGD << "ReversibleAction::_CleanUpPersistentData, get keptSwclVersionPath:" << keptSwclVersionPath.c_str();

        // Processes in the retained software set
        keptProcesses = keptSwclManifest->containedProcesses;
        LOGD << "ReversibleAction::_CleanUpPersistentData, get keptProcesses:"
             << strutil::JoinVector(keptProcesses).c_str();
    }

    // Only processes that exist in the retained software set
    AraVectorString keptDiffRemoving;
    std::ignore
        = std::set_difference(keptProcesses.begin(), keptProcesses.end(), removingProcesses.begin(),
                              removingProcesses.end(), std::inserter(keptDiffRemoving, keptDiffRemoving.begin()));
    LOGD << "ReversibleAction::_CleanUpPersistentData, get keptDiffRemoving:"
         << strutil::JoinVector(keptDiffRemoving).c_str();
    for (AraString const& keptProcessFQN : keptDiffRemoving) {
        LOGD << "ReversibleAction::_CleanUpPersistentData, get keptProcessFQN:" << keptProcessFQN.c_str();
        // Only take the last part of the process name
        AraString const keptProcessFQNName{strutil::GetLastComponent(keptProcessFQN)};
        LOGD << "ReversibleAction::_CleanUpPersistentData, get keptProcessFQNName:" << keptProcessFQNName.c_str();
        // Path of the process in the etc directory
        AraString const keptProcessPath{keptSwclVersionPath + kPathSeparator + kETC_DIR_NAME + kPathSeparator
                                        + keptProcessFQNName};  ////
        LOGD << "ReversibleAction::_CleanUpPersistentData, get keptProcessPath:" << keptProcessPath.c_str();
        bool const ret{ara::per::ClearUpPersistency("", keptProcessPath)};
        LOGD << "clear keptProcessPath:" << keptProcessPath.c_str() << ", ret:" << ret;
    }

    // Only processes that exist in the software set to be deleted
    AraVectorString stRemovingDiffkept;
    std::ignore
        = std::set_difference(removingProcesses.begin(), removingProcesses.end(), keptProcesses.begin(),
                              keptProcesses.end(), std::inserter(stRemovingDiffkept, stRemovingDiffkept.begin()));
    LOGD << "ReversibleAction::_CleanUpPersistentData, get RemovingDiffkept:"
         << strutil::JoinVector(stRemovingDiffkept).c_str();
    for (AraString const& removingProcessFQN : stRemovingDiffkept) {
        LOGD << "ReversibleAction::_CleanUpPersistentData, get removingProcessFQN:" << removingProcessFQN.c_str();
        // Only take the last part of the process name
        AraString const removingProcessFQNName{strutil::GetLastComponent(removingProcessFQN)};
        LOGD << "ReversibleAction::_CleanUpPersistentData, get removingProcessFQNName:"
             << removingProcessFQNName.c_str();
        // Path of the process in the etc directory
        AraString const removingProcessPath{removingSwclVersionPath + kPathSeparator + kETC_DIR_NAME + kPathSeparator
                                            + removingProcessFQNName};  ////
        LOGD << "ReversibleAction::_CleanUpPersistentData, get removingProcessPath:" << removingProcessPath.c_str();
        bool const ret{ara::per::ClearUpPersistency(removingProcessPath, "")};
        LOGD << "clear removingProcessPath:" << removingProcessPath.c_str() << ", ret:" << ret;
    }

    // Processes that exist in both the retained software set and the software set to be deleted
    AraVectorString intersection;
    std::ignore = std::set_intersection(keptProcesses.begin(), keptProcesses.end(), removingProcesses.begin(),
                                        removingProcesses.end(), std::inserter(intersection, intersection.begin()));
    LOGD << "ReversibleAction::_CleanUpPersistentData, get intersection:" << strutil::JoinVector(intersection).c_str();
    for (AraString const& processFQN : intersection) {
        LOGD << "ReversibleAction::_CleanUpPersistentData, get processFQN:" << processFQN.c_str();
        // Only take the last part of the process name
        AraString const processFQNName{strutil::GetLastComponent(processFQN)};
        LOGD << "ReversibleAction::_CleanUpPersistentData, get processFQNName:" << processFQNName.c_str();
        // Path of the process in the etc directory
        AraString const keptProcessPath{keptSwclVersionPath + kPathSeparator + kETC_DIR_NAME + kPathSeparator
                                        + processFQNName};
        AraString const removingProcessPath{removingSwclVersionPath + kPathSeparator + kETC_DIR_NAME + kPathSeparator
                                            + processFQNName};  ////
        LOGD << "ReversibleAction::_CleanUpPersistentData, get keptProcessPath:" << keptProcessPath.c_str()
             << "removingProcessPath:" << removingProcessPath.c_str();
        bool const ret{ara::per::ClearUpPersistency(removingProcessPath, keptProcessPath)};
        LOGD << "clear removingProcessPath:" << removingProcessPath.c_str()
             << "keptProcessPath:" << keptProcessPath.c_str() << ", ret:" << ret;
    }

    LOGD << "ReversibleAction::_CleanUpPersistentData, end.";
    return AraResult< ResultType >{ResultType::kSuccessfull};
}

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara
