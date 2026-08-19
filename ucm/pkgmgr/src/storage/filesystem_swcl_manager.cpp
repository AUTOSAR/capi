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
/// @file       filesystem_swcl_manager.cpp
/// @brief      Software cluster manager working with the filesystem implementation
/// @details
/// @date       2024-08-15
/// @author     cuiyinli
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/UCM/SoftwareClusterManager
/// @interface_level=module
/// @trace_id_sr=SR_UCM_00001
/// @unit_name=FileSystemSWCLManager
/// @unit_description=Software cluster manager working with the filesystem directly
/// @endcode
///
/// ================================================================

#include "filesystem_swcl_manager.h"

#include <algorithm>

#include "action_generator.h"
#include "ara/ucm/internal/extraction/tinyfs.h"
#include "ara/ucm/internal/extraction/tinyfsys.h"
#include "common/assert.h"
#include "common/log.h"
#include "common/path.h"
#include "common/strtype.h"
#include "parsing/software_cluster_manifest_parser.h"
#include "ucm_database.h"
#include "util/future_ext.h"
#include "util/rjson.h"

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @brief Retrieve a list of SoftwareClusters that have pending changes.
/// @return FutureGetSwClusterChangeInfo
/// @throws no
FutureGetSwClusterChangeInfo FileSystemSWCLManager::GetSwClusterChangeInfo() const noexcept
{
    LOGI << "call...";
    return SyncCall< GetSwClusterChangeInfoOutput >(
        [this]() { return AraResult< GetSwClusterChangeInfoOutput >({swclChangeInfo_}); });
}

/// @brief Retrieve a Software Clusters information list.
/// @return FutureGetSwClusterDescription
/// @throws no
FutureGetSwClusterDescription FileSystemSWCLManager::GetSwClusterDescription() noexcept
{
    LOGI << "call...";

    SwDescVectorType vec;

    // Get software clusters in kPresent state
    AraList< SoftwareCluster > const data{GetPresentSwcls()};
    std::ignore = std::transform(data.begin(), data.end(), std::back_inserter(vec), [](SoftwareCluster const& swcl) {
        SwDescType desc;

        desc.Name         = swcl.GetSwclManifest().shortName;
        desc.Version      = swcl.GetSwclManifest().version.ToString();
        desc.TypeApproval = swcl.GetSwclManifest().typeApproval;

        // TODO: Return the content of License and ReleaseNotes
        // Please note that Documentation is an ARElement that cannot be owned by a SoftwareCluster. The latter can only
        // refer to it. This aspect also means that once a given license is formalized by means of a Documentation it is
        // in general possible to refer to this formalization from within different SoftwareClusters.
        // [TPS_MANI_01220]fDRAFTg Release notes of software in included SoftwareCluster dIt is possible to refer to
        // release notes for software included in a SoftwareCluster by means of a reference to meta-class Documentation
        // in the role releaseNotes.c(RS_MANI_00035)
        for (const auto& license : swcl.GetSwclManifest().licenses) {
            desc.License += license;
            desc.License += ";";
        }
        desc.ReleaseNotes = swcl.GetSwclManifest().releaseNotes;

        // Software set directory
        AraString const swclPath{GetPath().GetSwclInstallDir(desc.Name, desc.Version.c_str())};
        // Get directory size
        desc.Size = 0U;
        AraResult< std::uint64_t > const sizeRet{tinyfsys::GetDirectorySize(swclPath)};
        if (sizeRet.HasValue()) {
            desc.Size = sizeRet.Value();
        }

        LOGD << "swclDesc: name:" << desc.Name.c_str() << " size:" << desc.Size << " path:" << swclPath.c_str();
        return desc;
    });

    return SyncCall< GetSwClusterDescriptionOutput >(
        [&vec]() { return AraResult< GetSwClusterDescriptionOutput >({vec}); });
}

/// @brief Retrieve a list of SoftwareClusters.
/// @return FutureGetSwClusterInfo
/// @throws no
FutureGetSwClusterInfo FileSystemSWCLManager::GetSwClusterInfo() noexcept
{
    LOGI << "call...";

    SwClusterInfoVectorType vec;

    // Get software clusters in kPresent state
    AraList< SoftwareCluster > const data{GetPresentSwcls()};
    std::ignore = std::transform(data.begin(), data.end(), std::back_inserter(vec), [](SoftwareCluster const& swcl) {
        SwClusterInfoType info;
        info.Name    = swcl.GetSwclManifest().shortName;
        info.Version = swcl.GetSwclManifest().version.ToString();
        info.State   = swcl.GetSwclState();
        return info;
    });

    return SyncCall< GetSwClusterInfoOutput >([&vec]() { return AraResult< GetSwClusterInfoOutput >({vec}); });
}

/// @brief Retrieve all actions performed by UCM in provided time range.
/// @param timestampGE
/// @param timestampLT
/// @return FutureGetHistory
/// @throws no
FutureGetHistory FileSystemSWCLManager::GetHistory(std::uint64_t const& timestampGE,
                                                   std::uint64_t const& timestampLT) noexcept
{
    LOGI << "call...";

    GetHistoryOutput his;

    /// access the UCM database file and get the entire UCM history out of it
    AraResult< GetHistoryVectorType > const fullHis{UcmDatabase(GetPathSettings().historyPath).GetFullHistory()};

    if (fullHis.HasValue()) {
        /// keep only the actions (and related info)
        /// that have been performed by UCM between timestampGE and timestampLT
        std::ignore = std::copy_if(fullHis.Value().begin(), fullHis.Value().end(), std::back_inserter(his.history),
                                   [&timestampGE, &timestampLT](GetHistoryType const& item) {
                                       return (item.Time >= timestampGE) && (item.Time < timestampLT);
                                   });
    }

    return SyncCall< GetHistoryOutput >([&his]() { return AraResult< GetHistoryOutput >(his); });
}

/// @brief Resets the software clusters change info. vector
/// @throws no
void FileSystemSWCLManager::ResetSWCLChangeInfo()
{
    LOGI << "call...";
    swclChangeInfo_.clear();
}

/// @brief Adds new software cluster change info
/// @param swclName
/// @param swclVersion
/// @param actionType
/// @throws no
void FileSystemSWCLManager::AddSWCLChangeInfo(AraString const& swclName,
                                              AraString const& swclVersion,
                                              ActionType const actionType)
{
    SwClusterInfoType swclInfo;

    swclInfo.Name    = swclName;
    swclInfo.Version = swclVersion;

    // Once SWCL state is completely implemented, GetSwclState() should be used instead.
    switch (actionType) {
        case ActionType::kUpdate: {
            swclInfo.State = SwClusterStateType::kUpdated;
            break;
        }
        case ActionType::kInstall: {
            swclInfo.State = SwClusterStateType::kAdded;
            break;
        }
        case ActionType::kRemove: {
            swclInfo.State = SwClusterStateType::kRemoved;
            break;
        }
        default: {
            ///LOGE << "invalid actionType:" << actionType;
            break;
        }
    }

    swclChangeInfo_.push_back(std::move(swclInfo));
}

/// @brief Return all SoftwareClusters that exist in SWCLs
///
/// @returns vector of SoftwareClusters
/// @throws no
AraVector< SoftwareCluster > FileSystemSWCLManager::GetAllSwcls() { return IndexActionsAndSwcls(); }

/// @brief Return all software packages in state kPresent
///
/// @returns vector of packages
/// @throws no
AraList< SoftwareCluster > FileSystemSWCLManager::GetPresentSwcls()
{
    AraList< SoftwareCluster > out;
    AraVector< SoftwareCluster > const clusters{IndexActionsAndSwcls()};

    std::ignore = std::copy_if(
        clusters.begin(), clusters.end(), std::back_inserter(out),
        [](SoftwareCluster const& swcl) noexcept { return swcl.GetSwclState() == SwClusterStateType::kPresent; });

    return out;
}

/// @brief Return all processes that exist in SWCLs in state kPresent, kAdded and kUpdated.
/// @returns vector of applications (i.e. processes)
/// @throws no
AraList< SoftwareCluster > FileSystemSWCLManager::GetSWCLsForActivation()
{
    AraList< SoftwareCluster > activeClusters;
    AraVector< SoftwareCluster > const currentClusters{IndexActionsAndSwcls()};

    // creates a new processes list including the installed and updated processes whose software cluster
    // are not in state kRemoved as well as the platform-level applications.
    std::ignore = std::copy_if(currentClusters.begin(), currentClusters.end(), std::back_inserter(activeClusters),
                               [](SoftwareCluster const& swcl) noexcept {
                                   if ((swcl.GetSwclState() == SwClusterStateType::kPresent)
                                       || (swcl.GetSwclState() == SwClusterStateType::kAdded)
                                       || (swcl.GetSwclState() == SwClusterStateType::kUpdated)) {
                                       return true;
                                   }
                                   return false;
                               });

    return activeClusters;
}

/// @brief Return all SoftwareClusters that exist in SWCLs in state kRemoved, kAdded and kUpdated.
/// @returns vector of SoftwareClusters
/// @throws no
AraList< SoftwareCluster > FileSystemSWCLManager::GetSWCLsToActivate()
{
    AraList< SoftwareCluster > activeClusters;
    AraVector< SoftwareCluster > const currentClusters{IndexActionsAndSwcls()};

    // creates a new processes list including the installed and updated processes whose software cluster
    // are not in state kRemoved as well as the platform-level applications.
    std::ignore = std::copy_if(currentClusters.begin(), currentClusters.end(), std::back_inserter(activeClusters),
                               [](SoftwareCluster const& swcl) noexcept {
                                   if ((swcl.GetSwclState() == SwClusterStateType::kRemoved)
                                       || (swcl.GetSwclState() == SwClusterStateType::kAdded)
                                       || (swcl.GetSwclState() == SwClusterStateType::kUpdated)) {
                                       return true;
                                   }
                                   return false;
                               });

    return activeClusters;
}

/// @brief Return SoftwareClusters that prepare to update
/// @returns vector of SoftwareClusters
/// @throws no
AraList< SoftwareCluster > FileSystemSWCLManager::GetSWCLsToPrepareUpdate()
{
    AraList< SoftwareCluster > activeClusters;
    AraVector< SoftwareCluster > const currentClusters{IndexActionsAndSwcls(true)};

    // creates a new processes list including the installed and updated processes whose software cluster
    // are not in state kRemoved as well as the platform-level applications.
    std::ignore = std::copy_if(currentClusters.begin(), currentClusters.end(), std::back_inserter(activeClusters),
                               [](SoftwareCluster const& swcl) noexcept {
                                   if ((swcl.GetSwclState() == SwClusterStateType::kRemoved)
                                       || (swcl.GetSwclState() == SwClusterStateType::kAdded)
                                       || (swcl.GetSwclState() == SwClusterStateType::kUpdated)) {
                                       return true;
                                   }
                                   return false;
                               });

    return activeClusters;
}

/// @brief clean software cluster
/// @param swclName
/// @param retainedVersion
/// @returns bool
/// @throws no
bool FileSystemSWCLManager::CleanSwcl(AraString const& swclName, AraString const& retainedVersion)
{
    LOGI << "swclName:" << swclName.c_str() << ", retainedVersion:" << retainedVersion.c_str();

    bool const res{true};
    // Is there a retained version?
    bool foundRetainedVersion{false};

    // Get all version names under the software set installation location
    AraString const swclDir{GetPath().GetSwclInstallDir(swclName)};
    AraVectorString const swclVersionDirList{tinyfsys::GetSubdirectories(swclDir)};

    // Skip the retained version, delete all remaining versions
    for (AraString const& swclVersionDir : swclVersionDirList) {
        AraString const swclVersion{tinyfs::Basename(swclVersionDir)};
        LOGD << "got swclVersion:" << swclVersion.c_str();

        if (0 == retainedVersion.compare(swclVersion)) {
            foundRetainedVersion = true;
            continue;
        }

        // Delete non-specified versions
        AraResultVoid const rmRet{tinyfsys::RemoveDirectory(swclVersionDir)};
        if (!rmRet.HasValue()) {
            LOGE << "fail to remove swclVersionDir:" << swclVersionDir.c_str()
                 << ", errc:" << rmRet.Error().Message().data();
            ///res = false;
        }
    }

    // Determine if the retained version was found
    LOGD << "foundRetainedVersion" << foundRetainedVersion;

    // Update the software set status to the retained version
    UpdateSwclStatus(swclName, SwClusterStateType::kPresent, retainedVersion, "");
    return res;
}

/// @brief Checks if the version of a software cluster is older than the existing one.
/// @param swcl The software cluster to be checked.
/// @returns True if the version of the software cluster is downgraded, false otherwise.
/// @uptrace={SWS_UCM_00103, 72d6a4ff65df294f6ab37f5eee80221335635046}
/// @throws no
bool FileSystemSWCLManager::IsSwclOldVersion(SoftwareCluster const& swcl)
{
    AraString const& name{swcl.GetSwclManifest().shortName};
    Version const& newVer{swcl.GetSwclManifest().version};

    // Check all previously installed versions
    ;
    AraResult< GetHistoryVectorType > const fullHis{UcmDatabase(GetPathSettings().historyPath).GetFullHistory()};
    if (fullHis.HasValue()) {
        GetHistoryVectorType::const_reverse_iterator const ptrToNewerVersionOfThisSwcl{
            std::find_if(std::move(fullHis.Value().rbegin()), std::move(fullHis.Value().rend()),
                         [&name, &newVer](GetHistoryType const& item) {
                             const bool sameName{item.Name == name};
                             const bool resolutionSuccess{item.Resolution == ResultType::kSuccessfull};
                             if (sameName && resolutionSuccess) {
                                 return newVer <= Version(item.Version);
                             }
                             return false;
                         })};

        if (ptrToNewerVersionOfThisSwcl != fullHis.Value().rend()) {
            return true;
        }
    }

    // Check currently installed version
    for (SoftwareCluster const& it : GetPresentSwcls()) {
        if ((name == it.GetSwclManifest().shortName) && (newVer <= it.GetSwclManifest().version)) {
            return true;
        }
    }

    return false;
}

/// @brief platform_core is updated or not
/// @returns bool
/// @throws no
bool FileSystemSWCLManager::IsPlatformCoreUpdated()
{
    for (SoftwareCluster const& swcl : GetSWCLsToActivate()) {
        const bool equalCoreCategory{kPlatformCoreSWCLCategory == swcl.GetSwclManifest().category};
        if ((kIsoftFrameworkSWCLCategory == swcl.GetSwclManifest().category) || equalCoreCategory) {
            return true;
        }
    }

    return false;
}

/// @brief non-platform_core is updated or not
/// @returns bool
/// @throws no
bool FileSystemSWCLManager::IsNonPlatformCoreUpdated()
{
    for (SoftwareCluster const& swcl : GetSWCLsToActivate()) {
        if (kAppLayerSWCLCategory == swcl.GetSwclManifest().category) {
            return true;
        }
    }

    return IsOSUpdated();
}

/// @brief os is updated or not
/// @returns bool
/// @throws no
bool FileSystemSWCLManager::IsOSUpdated()
{
    for (SoftwareCluster const& swcl : GetSWCLsToActivate()) {
        if (kIsoftOSSWCLCategory == swcl.GetSwclManifest().category) {
            return true;
        }
    }

    return false;
}

/// @brief Add one action to the queue of actions.
///
/// Adds one action to the queue of the actions to be executed. Empty actions (nullptr) are ignored.
/// @param action
/// @throws no
void FileSystemSWCLManager::AddAction(std::unique_ptr< ReversibleAction > action)
{
    if (action != nullptr) {
        actions_.emplace_back(std::move(action));
    }
}

/// @brief name and version of software cluster is in actions or not.
/// @param swclName the name of the software cluster
/// @param swclVer the version of the software cluster
/// @return true if exist.
/// @throws no
bool FileSystemSWCLManager::ExistsInActions(AraString const& swclName, AraString const& swclVer)
{
    for (std::unique_ptr< ReversibleAction > const& it : actions_) {
        const bool equalVersion{it->GetSwclVersion() == swclVer};
        if ((it->GetSwclName() == swclName) && equalVersion) {
            return true;
        }
    }
    return false;
}

/// @brief Persist all actions to the filesystem
/// @throws no
void FileSystemSWCLManager::PersistActionsToFilesystem() noexcept {}

/// @brief Recover all actions from the filesystem
/// @throws no
void FileSystemSWCLManager::RecoverActionsFromFilesystem() noexcept {}

/// @brief RecoverProcessedSwpkgAction
/// @param actionExecutionInfoVec
/// @throws no
void FileSystemSWCLManager::RecoverProcessedSwpkgAction(
    AraList< ActionExecutionInfoType > const& actionExecutionInfoVec)
{
    for (ActionExecutionInfoType const& it : actionExecutionInfoVec) {
        LOGD << "reload " << it.Str().c_str();

        // Create, execute and maybe store the action to make it possible to revert or commit it later
        std::unique_ptr< SoftwarePackage > swpkg{nullptr};
        std::unique_ptr< ReversibleAction > currentAction{
            ActionGenerator()(static_cast< std::underlying_type< ActionType >::type >(it.actionType),
                              static_cast< std::underlying_type< ActivateOptionType >::type >(it.activateOptionType),
                              it.deltaPackageApplicableVersion, it.swclName, it.curSWCLVersion, it.swclVersion,
                              std::move(swpkg))};  //nullptr is ok, package_ will not be used during recover
        AssertWithLog(currentAction != nullptr);

        // Recover execution information
        currentAction->RecoverExecutionInfo(it.resolution, it.time);
        AddSWCLChangeInfo(it.swclName, it.swclVersion, it.actionType);
        AddAction(std::move(currentAction));
    }

    // LibPocoZipExtractor::Cleanup() will execute fs_.RemoveDirectory(lastExtractionDirectory_); i.e., delete the decompressed directory
    ///softwarePackageExtractor_->Cleanup();.//mytodo666////.
}

/// @brief getter returning action related information of this update sequence
///
/// @return action related information, not the actual actions' ownership
/// @throws no
AraList< GetHistoryType > FileSystemSWCLManager::GetReversibleActionsInfo() const
{
    AraList< GetHistoryType > actionInfo;

    /// collect all action related information of the current update sequence
    for (std::unique_ptr< ReversibleAction > const& action : actions_) {
        GetHistoryType actionSnapshot;
        actionSnapshot.Time       = action->GetTimeStamp();
        actionSnapshot.Name       = action->GetSwclName();
        actionSnapshot.Version    = action->GetSwclVersion();
        actionSnapshot.Action     = action->GetActionType();
        actionSnapshot.Resolution = action->GetResolution();

        actionInfo.push_back(actionSnapshot);
    }

    return actionInfo;
}

/// @brief GetActionExecutionInfo
/// @returns ActionExecutionInfoType vector
/// @throws no
AraList< ActionExecutionInfoType > FileSystemSWCLManager::GetActionExecutionInfo() const
{
    AraList< ActionExecutionInfoType > actionExecutionInfoVec;

    /// collect all action related information of the current update sequence
    for (std::unique_ptr< ReversibleAction > const& action : actions_) {
        ActionExecutionInfoType actionExecution;
        actionExecution.actionType                    = action->GetActionType();
        actionExecution.activateOptionType            = action->GetActivationOption();
        actionExecution.deltaPackageApplicableVersion = action->GetdeltaPackageApplicableVersion();
        actionExecution.swclName                      = action->GetSwclName();
        actionExecution.curSWCLVersion                = action->GetCurSwclVersion();
        actionExecution.swclVersion                   = action->GetSwclVersion();
        actionExecution.time                          = action->GetTimeStamp();
        actionExecution.resolution                    = action->GetResolution();

        actionExecutionInfoVec.push_back(actionExecution);
    }

    return actionExecutionInfoVec;
}

/// @brief The strongest activation option
///
/// @returns the strongest activation option: system reset or function group restart or nothing at all
/// @throws no
ActivateOptionType FileSystemSWCLManager::GetStrongestActivationOption() const noexcept
{
    // do nothing by default
    ActivateOptionType result{ActivateOptionType::kWaitForReboot};

    // calculate the strongest activation option
    for (std::unique_ptr< ReversibleAction > const& it : actions_) {
        ActivateOptionType const option{it->GetActivationOption()};
        if (option == ActivateOptionType::kReboot) {
            return ActivateOptionType::kReboot;
        }
        if (option == ActivateOptionType::kRestartApplication) {
            result = ActivateOptionType::kRestartApplication;
        }
    }

    return result;
}

/// @brief Get the activation option for the SoftwareCluster
/// @param swcl
/// @returns the activation option: system reset or function group restart or nothing at all
/// @throws no
ActivateOptionType FileSystemSWCLManager::GetActivationOption4Swcl(SoftwareCluster const& swcl) const
{
    // get option from local actions_
    for (std::unique_ptr< ReversibleAction > const& it : actions_) {
        if (swcl.GetSwclManifest().shortName == it->GetSwclName()) {
            return it->GetActivationOption();
        }
    }

    // do nothing by default
    return ActivateOptionType::kWaitForReboot;
}

/// @brief Commits all actions
/// @returns result
/// @throws no
AraResultVoid FileSystemSWCLManager::CommitChanges()
{
    LOGI << "call...";

    for (std::unique_ptr< ReversibleAction > const& it : actions_) {
        AraResult< ResultType > const ret{it->CommitChanges()};
        if (!ret.HasValue()) {
            LOGE << "CommitChanges errc:" << ret.Error().Message().data();
            return AraResultVoid::FromError(ret.Error());
        }
    }

    actions_.clear();  // finish the campaign
    return {};
}

/// @brief Reverts all actions
/// @returns result
/// @throws no
AraResultVoid FileSystemSWCLManager::RevertChanges()
{
    LOGI << "call...";

    for (std::unique_ptr< ReversibleAction > const& it : actions_) {
        AraResult< ResultType > const ret{it->RevertChanges()};
        if (!ret.HasValue()) {
            LOGE << "RevertChanges errc:" << ret.Error().Message().data();
            return AraResultVoid::FromError(ret.Error());
        }
    }

    actions_.clear();  // finish the campaign
    return {};
}

/// @brief MergeVar for all actions
/// @returns result
/// @throws no
AraResultVoid FileSystemSWCLManager::MergeVars()
{
    LOGI << "call...";

    for (std::unique_ptr< ReversibleAction > const& it : actions_) {
        AraResult< ResultType > const ret{it->MergeVar()};
        if (!ret.HasValue()) {
            LOGE << "MergeVars errc:" << ret.Error().Message().data();
            return AraResultVoid::FromError(ret.Error());
        }
    }

    return {};
}

/// @brief update software cluster status
/// @param swclName
/// @param swclState
/// @param version
/// @param newVersion
/// @throws no
void FileSystemSWCLManager::UpdateSwclStatus(AraString const& swclName,
                                             SwClusterStateType const swclState,
                                             AraString const& version,
                                             AraString const& newVersion)
{
    AraString const state{SwclStateToStr(swclState)};
    LOGD << "name:" << swclName.c_str() << " state:" << state.c_str() << " ver:" << version.c_str()
         << " newVer:" << newVersion.c_str();

    /// {
    ///     "state":"kAdded/kPresent/kUpdated/kRemoved",
    ///     "version":"1.0.0",
    ///     "new_version":"1.0.1"
    /// }
    rjson::Doc statusDom{rjson::MakeDomObject()};
    rjson::AddKS(statusDom, statusDom, "state", state);
    rjson::AddKS(statusDom, statusDom, "version", version);
    rjson::AddKS(statusDom, statusDom, "new_version", newVersion);
    rjson::Save(statusDom, GetPath().GetSwclStatusPath(swclName));
}

/// @brief get software cluster status
/// @param swclName
/// @param swclState
/// @param version
/// @param newVersion
/// @returns bool
/// @throws no
bool FileSystemSWCLManager::GetSwclStatus(AraString const& swclName,
                                          SwClusterStateType& swclState,
                                          AraString& version,
                                          AraString& newVersion)
{
    RManifestOpenAndCheck(doc, GetPath().GetSwclStatusPath(swclName), false);

    AraString const state{RManifestDocGetSS(doc, "state")};
    std::ignore = swclState;
    swclState   = SwclStateFromStr(state);

    RManifestLoad(doc, "version", version);
    RManifestLoad(doc, "new_version", newVersion);

    LOGD << "name:" << swclName.c_str() << " state:" << state.c_str() << " ver:" << version.c_str()
         << " newVer:" << newVersion.c_str();
    return true;
}

/// @brief convert SwClusterStateType to string
/// @param swclState
/// @returns string
/// @throws no
AraString FileSystemSWCLManager::SwclStateToStr(SwClusterStateType const swclState)
{
    AraString swclStateStr{"Unknown"};
    switch (swclState) {
        case SwClusterStateType::kPresent: {
            swclStateStr = "kPresent";
            break;
        }
        case SwClusterStateType::kAdded: {
            swclStateStr = "kAdded";
            break;
        }
        case SwClusterStateType::kUpdated: {
            swclStateStr = "kUpdated";
            break;
        }
        case SwClusterStateType::kRemoved: {
            swclStateStr = "kRemoved";
            break;
        }
        default: {
            break;
        }
    }
    return swclStateStr;
}

/// @brief convert SwClusterStateType from string
/// @param swclStateStr
/// @returns SwClusterStateType
/// @throws no
SwClusterStateType FileSystemSWCLManager::SwclStateFromStr(AraString const& swclStateStr)
{
    if (0 == swclStateStr.compare("kPresent")) {
        return SwClusterStateType::kPresent;
    }
    if (0 == swclStateStr.compare("kAdded")) {
        return SwClusterStateType::kAdded;
    }
    if (0 == swclStateStr.compare("kUpdated")) {
        return SwClusterStateType::kUpdated;
    }
    if (0 == swclStateStr.compare("kRemoved")) {
        return SwClusterStateType::kRemoved;
    }
    LOGE << "illegal swclStateStr:" << swclStateStr.c_str();
    return SwClusterStateType::kPresent;
}

/// @brief UpdateSWCLList
/// @param activeSWCLs
/// @param updateSwclCategorySet
/// @param swclListFilePath
/// @returns bool
/// @throws no
bool FileSystemSWCLManager::UpdateSWCLList(AraList< SoftwareCluster > const& activeSWCLs,
                                           std::set< AraString > const& updateSwclCategorySet,
                                           AraString const& swclListFilePath)
{
    LOGD << "call... swclListFilePath:" << swclListFilePath.c_str();
    // {
    //     "swcl_name": "CMDemo_client",.
    //     "swcl_fqn": "/ISOFT/Development/SoftwareClusters1/VER_100/CMDemo_client",.
    //     "category": "APPLICATION_LAYER",.
    //     "swcl_version": "1.0.0",.
    //     "dependsOn": [],
    //     "executable_list": [
    //         "/ClientApp/exe/clientd"
    //     ],
    //     "process_list": [
    //         "client_process"
    //     ]
    // },

    // Mapping from software set FQN to its dependencies
    AraMap< AraString, SoftwareClusterDependencyFormula > mapSwcl2DependencyFormula;
    for (SoftwareCluster const& cluster : activeSWCLs) {
        SoftwareClusterManifest const& mnf{cluster.GetSwclManifest()};
        mapSwcl2DependencyFormula[mnf.fQN] = mnf.dependsOnFormula;
    }

    rjson::Doc listDom{rjson::MakeDomObject()};
    rjson::Value swclsDom{rjson::MakeArray()};
    for (SoftwareCluster const& cluster : activeSWCLs) {
        SoftwareClusterManifest const& mnf{cluster.GetSwclManifest()};
        LOGD << "got " << SwclManifestUtil(mnf).ShortInfo().c_str();

        // Determine if it is a software set category to be updated
        if (updateSwclCategorySet.find(mnf.category) == updateSwclCategorySet.end()) {
            continue;
        }

        rjson::Value swclDom{rjson::MakeObject()};
        rjson::AddKS(listDom, swclDom, kSwclName, mnf.shortName.c_str());
        rjson::AddKS(listDom, swclDom, kSwclFQN, mnf.fQN.c_str());
        rjson::AddKS(listDom, swclDom, kSwclCategory, mnf.category.c_str());
        rjson::AddKS(listDom, swclDom, kSwclVersion, mnf.version.ToString().c_str());

        // Get all dependencies
        std::set< AraString > const dependsOnSet{
            cluster.GetSwclManifest().dependsOnFormula.GetDependsOnSWCL(mapSwcl2DependencyFormula)};
        rjson::Value dependsOnDom{rjson::MakeArray()};
        for (AraString const& it : dependsOnSet) {
            rjson::PushS(listDom, dependsOnDom, it);
        }
        rjson::AddKO(listDom, swclDom, kDependsOn, dependsOnDom);

        // Get all exe
        rjson::Value executableListDom{rjson::MakeArray()};
        for (AraString const& it : cluster.GetSwclManifest().containedExecutables) {
            rjson::PushS(listDom, executableListDom, it);
        }
        rjson::AddKO(listDom, swclDom, kExecutableList, executableListDom);

        // Get all processes
        rjson::Value processListDom{rjson::MakeArray()};
        for (AraString const& it : cluster.GetSwclManifest().containedProcesses) {
            rjson::PushS(listDom, processListDom, strutil::GetLastComponent(it));
        }
        rjson::AddKO(listDom, swclDom, kProcessList, processListDom);

        rjson::PushO(listDom, swclsDom, swclDom);
    }
    rjson::AddKO(listDom, listDom, kSwcls, swclsDom);

    rjson::SavePretty(listDom, swclListFilePath);

    LOGI << "end. Processes list is successfully updated";
    return true;
}

/// @brief Parse system software packages' data and cache it
/// @param forPreparingUpdate
/// @returns SoftwareCluster vector
/// @throws no
AraVector< SoftwareCluster > FileSystemSWCLManager::IndexActionsAndSwcls(bool const forPreparingUpdate)
{
    // indexing only the final installation directory, all temporaries are ignored
    AraVector< SoftwareCluster > swcls;
    SoftwareClusterManifestParser parser;

    // Get all subdirectories under appLayerSWCLDir (different software clusters)
    AraVectorString swclDirs{tinyfsys::GetSubdirectories(GetPathSettings().swclsDir)};

    // Add framework software set
    AraString const frameworkDir{GetPath().GetSwclInstallDir(kFRAMEWORK_SWCL_NAME)};
    if (tinyfsys::DoesDirectoryExist(frameworkDir)) {
        swclDirs.push_back(frameworkDir);
    }

    // Add platform-level core software set
    AraString const coreDir{GetPath().GetSwclInstallDir(kCORE_SWCL_NAME)};
    if (tinyfsys::DoesDirectoryExist(coreDir)) {
        swclDirs.push_back(coreDir);
    }

    // parse all swcls
    for (AraString const& swclDir : swclDirs) {
        AraString const swclName{tinyfs::Basename(swclDir)};
        LOGD << "swclDir:" << swclDir.c_str() << ", swclName:" << swclName.c_str();

        // get software cluster state
        SwClusterStateType swclState;
        AraString curVersion;
        AraString newVersion;
        bool const ret{GetSwclStatus(swclName, swclState, curVersion, newVersion)};
        if (!ret) {  /// TODO: After completing the generation of the system image runtime environment, delete this code;
            /// Since the VisitAllSWCLInfo interface provided by UCM scans the software set installation directory (which may have half-installed directories), this code needs to be retained
            continue;
        }

        // get manifest path
        AraString swclVersion{curVersion};
        bool isInvalidStateType{false};
        switch (swclState) {
            case SwClusterStateType::kPresent:
            case SwClusterStateType::kRemoved: {
                break;
            }
            case SwClusterStateType::kAdded: {
                swclVersion = newVersion;
                break;
            }
            case SwClusterStateType::kUpdated: {
                if (!forPreparingUpdate) {
                    swclVersion = newVersion;
                }
                break;
            }
            default: {
                isInvalidStateType = true;
                break;
            }
        }
        if (isInvalidStateType) {
            LOGE << "got a illegal swclState";
            continue;
        }

        AraString const mnfPath{GetPath().GetSwclManifestPath(swclName, swclVersion)};
        LOGD << "manifestPath:" << mnfPath.c_str();

        // parse SoftwareCluster
        // The GetSwclState of the SoftwareCluster obtained here will all return kPresent, because:
        /// SoftwareCluster::SoftwareCluster(...) : swclState_(SwClusterStateType::kPresent)
        std::unique_ptr< SoftwareClusterManifest > const swclMnf{parser.ParseFromFile(mnfPath)};
        if (swclMnf == nullptr) {
            LOGE << mnfPath.c_str() << " parse failed";
            continue;
        }

        swcls.emplace_back(*swclMnf, mnfPath);
        (swcls.end() - 1)->SetSwclState(swclState);
    }

    return swcls;
}

/// @brief Parse system software packages' data and cache it
/// @param forPreparingUpdate
/// @returns SoftwareCluster vector
/// @throws no
AraList< AraString > FileSystemSWCLManager::GetAllSwclDirs()
{
    // Get all subdirectories under appLayerSWCLDir (different software clusters)
    AraVectorString swclDirs{tinyfsys::GetSubdirectories(GetPathSettings().swclsDir)};

    // Add framework software set
    AraString const frameworkDir{GetPath().GetSwclInstallDir(kFRAMEWORK_SWCL_NAME)};
    if (tinyfsys::DoesDirectoryExist(frameworkDir)) {
        swclDirs.push_back(frameworkDir);
    }

    // Add platform-level core software set
    AraString const coreDir{GetPath().GetSwclInstallDir(kCORE_SWCL_NAME)};
    if (tinyfsys::DoesDirectoryExist(coreDir)) {
        swclDirs.push_back(coreDir);
    }

    LOGD << "swclDirs:" << strutil::JoinVector(swclDirs);
    return swclDirs;
}

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara
