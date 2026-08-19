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
/// @file       ab_partition.cpp
/// @brief      a/b partition implementation
/// @details
/// @date       2024-01-01
/// @author     cuiyinli
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/UCM/FsmManager
/// @interface_level=unit
/// @trace_id_sr=SR_UCM_00001
/// @unit_name=ABPartition
/// @unit_description=Handles the A/B partition logic of the package manager.
/// @endcode
///
/// ================================================================

#include "ab_partition.h"

#include "ara/ucm/internal/crypto/crypto.h"
#include "ara/ucm/internal/extraction/tinyfs.h"
#include "ara/ucm/internal/extraction/tinyfsys.h"
#include "common/assert.h"
#include "common/const.h"
#include "common/log.h"
#include "common/path.h"
#include "common/rjson_manifest.h"
#include "common/strtype.h"
#include "em/em_client.h"
#include "storage/filesystem_swcl_manager.h"
#include "util/rjson.h"
#include "util/string_ext.h"

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @brief recover last used id
/// @throws no
void ABPartition::RecoverLastUsedId()
{
    lastUsedBootOptionFileID_ = RecoverLastUsedBootOptionFileId();
    lastUsedSWCLListID_       = RecoverLastUsedSWCLListId();
    LOGD << "lastUsedBootOptionFileID_:" << lastUsedBootOptionFileID_ << " lastUsedSWCLListID_:" << lastUsedSWCLListID_;
}

/// @brief RecoverLastUsedBootOptionFileId
/// @throws no
/// @return result of recover
std::uint32_t ABPartition::RecoverLastUsedBootOptionFileId()
{
    // Search the araDir_ directory for all boot_option_verX.cfg files, use the smallest X as lastUsedBootOptionFileID_
    return GetMinFileNameId(GetPathSettings().araDir, kBootOptionFileNamePrefix, kBootOptionFileNamePostfix);
}

/// @brief RecoverLastUsedSWCLListId
/// @throws no
/// @return result of recover
std::uint32_t ABPartition::RecoverLastUsedSWCLListId()
{
    // Search the appLayerSWCLDir directory for the IDs of all application_swcl_list_verY.json files, use the smallest Y as lastUsedSWCLListID_
    return GetMinFileNameId(GetPathSettings().swclsDir, kSWCLListFileNamePrefix, kSWCLListFileNamePostfix);
}

/// @brief used in verify executor, to link to b partition
/// @param activationSwcls
/// @throws no
void ABPartition::BootOptionFileLinkToB(AraList< SoftwareCluster > const& activationSwcls) const
{
    // 3 If the update involves platform_core, clean and create a new boot_option_ver(X+1).cfg (content is empty);
    //         Use the newVersion of platform_core as core_version
    //         Clean and generate boot_option_ver(X+1).md5 based on boot_option_ver(X+1).cfg;

    // First, determine the new versions of core and framework required for partition B
    AraString const coreVersionToConfig{GetVersionFromSwclStatus(kCORE_SWCL_NAME)};
    AraString const frameworkVersionToConfig{GetVersionFromSwclStatus(kFRAMEWORK_SWCL_NAME)};
    bool ret{false};

    // If the update involves core (including updating framework), clean and create a new run_time_framework_core_list.json
    // Core software set directory
    AraString const coreListFilePath{GetPath().GetSwclInstallDir(kCORE_SWCL_NAME, coreVersionToConfig) + kPathSeparator
                                     + kETC_DIR_NAME + kPathSeparator + kFrameworkCoreListFileName};

    LOGD << "get coreVersionToConfig:" << coreVersionToConfig.c_str()
         << " frameworkVersionToConfig:" << frameworkVersionToConfig.c_str()
         << ", call UpdateSWCLList with coreListFilePath:" << coreListFilePath.c_str();

    // Update the software set list; note that the software set list file does not require a separate md5 file with the same name (its integrity is guaranteed by ucm_status.json);
    std::set< AraString > const updateSWCLCategorySet{AraString(kIsoftFrameworkSWCLCategory),
                                                      AraString(kPlatformCoreSWCLCategory)};
    ret = FileSystemSWCLManager::UpdateSWCLList(activationSwcls, updateSWCLCategorySet, coreListFilePath);
    LOGD << "UpdateSWCLList ret:" << ret;

    // Generate a new boot option file
    LOGD << "call GenBootConfig with lastUsedBootOptionFileID_+1:" << lastUsedBootOptionFileID_ + 1U;
    ret = _genBootConfig(lastUsedBootOptionFileID_ + 1U, frameworkVersionToConfig, coreVersionToConfig);
    LOGD << "_genBootConfig ret:" << ret;
}

/// @brief used in verify executor, to link to b partition
/// @param activationSwcls
/// @param nonPlatformCoreUpdated
/// @throws no
void ABPartition::SwclListFileLinkToB(AraList< SoftwareCluster > const& activationSwcls,
                                      bool const nonPlatformCoreUpdated) const
{
    LOGD << "nonPlatformCoreUpdated:" << nonPlatformCoreUpdated << " lastUsedSWCLListID_:" << lastUsedSWCLListID_;

    /// Symlinks or A/B switch()
    // 1 Determine the application software set list
    std::uint32_t sWCLListFileId{
        lastUsedSWCLListID_};  // Use the original application_swcl_list_verY.json as the application software set list
    bool ret{false};

    if (nonPlatformCoreUpdated) {  // If the update involves non-platform software sets
        // Clean and generate a new application software set list application_swcl_list_ver(Y+1).json, and use it as the application software set list
        sWCLListFileId = lastUsedSWCLListID_ + 1U;
        AraString const newSWCLListFilePath{GetSwclListFilePath(sWCLListFileId)};
        LOGD << "get newSWCLListID:" << sWCLListFileId
             << ", call UpdateSWCLList with newSWCLListFilePath:" << newSWCLListFilePath.c_str();

        // Update the software set list; note that the software set list file does not require a separate md5 file with the same name (its integrity is guaranteed by ucm_status.json);
        std::set< AraString > const updateSwclCategorySet{AraString(kAppLayerSWCLCategory),
                                                          AraString(kIsoftOSSWCLCategory)};
        ret = FileSystemSWCLManager::UpdateSWCLList(activationSwcls, updateSwclCategorySet, newSWCLListFilePath);
        LOGD << "UpdateSWCLList ret:" << ret;
    }

    // 2 Link the application software set list determined in step 1 to the appropriate platform_core software set
    // Determine the target platform_core software set version
    AraString const coreVersionToConfig{GetVersionFromSwclStatus(kCORE_SWCL_NAME)};
    LOGD << "get coreVersionToConfig:" << coreVersionToConfig.c_str()
         << ", call LinkSwclList with coreVersionToConfig and sWCLListFileId:" << sWCLListFileId;

    // Clean the etc directory of the target platform_core software set version and create a new run_time_application_swcl_list.cfg
    // Its process_list is the application software set list determined in step 1, generate the corresponding md5;
    // Generate the md5 of run_time_application_swcl_list.cfg
    ret = LinkSwclList(sWCLListFileId, coreVersionToConfig);  ///////////////////////////mytodo//
    LOGD << "LinkSwclList ret:" << ret;
}

/// @brief used in cleanup executor, to cleanup a partition
/// @throws no
void ABPartition::BootOptionFileCleanupA()
{
    // Search the araDir_ directory for all boot_option_verX.cfg files, get min and max
    std::uint32_t minBootOptionFileID{0U};
    std::uint32_t maxBootOptionFileID{0U};
    GetMinMaxFileNameId(GetPathSettings().araDir, kBootOptionFileNamePrefix, kBootOptionFileNamePostfix,
                        minBootOptionFileID, maxBootOptionFileID);

    LOGD << "lastUsedBootOptionFileID_:" << lastUsedBootOptionFileID_ << " minBootOptionFileID:" << minBootOptionFileID
         << " maxBootOptionFileID:" << maxBootOptionFileID;

    if (maxBootOptionFileID
        > minBootOptionFileID) {  // If there are 2 versions, delete the smaller version boot_option_verX.cfg and its md5 file;
        // Delete the BootOption file
        DeleteBootOptionFile(minBootOptionFileID);
        // Set lastUsedBootOptionFileID_ to the larger version ID
        lastUsedBootOptionFileID_ = maxBootOptionFileID;
        LOGD << "delete minBootOptionFileID, set maxBootOptionFileID as lastUsedBootOptionFileID_";
        return;
    }

    // If there is only one, it means the smaller version has already been deleted
    LOGD << "lastUsedBootOptionFileID_ is remained";
}

/// @brief used in cleanup executor, to cleanup a partition
/// @throws no
void ABPartition::SwclListFileCleanupA()
{
    // Search the appLayerSWCLDir directory for all application_swcl_list_verY.json files
    std::uint32_t minSWCLListFileID{0U};
    std::uint32_t maxSWCLListFileID{0U};
    GetMinMaxFileNameId(GetPathSettings().swclsDir, kSWCLListFileNamePrefix, kSWCLListFileNamePostfix,
                        minSWCLListFileID, maxSWCLListFileID);

    LOGD << "lastUsedSWCLListID_:" << lastUsedSWCLListID_ << " minSWCLListFileID:" << minSWCLListFileID
         << " maxSWCLListFileID:" << maxSWCLListFileID;

    if (maxSWCLListFileID
        > minSWCLListFileID) {  // If there are 2 versions, delete the smaller version application_swcl_list_verY.json
        // Software set list filename and Path
        AraString const minSWCLListFilePath{GetSwclListFilePath(minSWCLListFileID)};
        // Delete application_swcl_list_verY.json
        AraResultVoid const ret{tinyfsys::RemoveFile(minSWCLListFilePath)};
        if (!ret.HasValue()) {
            LOGE << "remove minSWCLListFilePath err:" << ret.Error().Message().data();
        }

        // Set lastUsedSWCLListFileID_ to the larger version ID
        lastUsedSWCLListID_ = maxSWCLListFileID;
        LOGD << "delete minSWCLListFileID, set maxSWCLListFileID as lastUsedSWCLListID_";
    }

    // If there is only one, it means the smaller version has already been deleted
    LOGD << "lastUsedSWCLListID_ is remained";
}

/// @brief used in rollback executor, to rollback to A partition and cleanup B partition
/// @throws no
void ABPartition::BootOptionFileRollbackToA() const
{
    // Delete the BootOption file
    LOGD << "lastUsedBootOptionFileID_:" << lastUsedBootOptionFileID_
         << " DeleteBootOptionFile with lastUsedBootOptionFileID_+1";
    DeleteBootOptionFile(lastUsedBootOptionFileID_ + 1U);
}

/// @brief used in rollback executor, to rollback to A partition and cleanup B partition
/// @param nonPlatformCoreUpdated
/// @throws no
void ABPartition::SwclListFileRollbackToA(bool const nonPlatformCoreUpdated) const
{
    // 3. Link the original application software set list to the appropriate platform_core software set (i.e., the core_version in boot_option_verX.cfg)
    AraString const coreVersionStrToConfig{GetVersionFromSwclStatus(kCORE_SWCL_NAME, true)};
    LOGD << "call LinkSwclList with coreVersionStrToConfig:" << coreVersionStrToConfig.c_str()
         << " and lastUsedSWCLListID_:" << lastUsedSWCLListID_;

    // Clean the etc directory of the target platform_core software set version and create a new run_time_application_swcl_list.cfg
    // Its process_list is application_swcl_list_verY.json, generate the corresponding md5;
    // Generate the md5 of run_time_application_swcl_list.cfg
    bool const ret{LinkSwclList(lastUsedSWCLListID_, coreVersionStrToConfig)};
    if (!ret) {
        LOGE << "LinkSwclList failed";
    }

    // 4. If the update involves non-platform software sets: delete the new version software set list application_swcl_list_ver(Y+1).json (code already implemented);
    if (nonPlatformCoreUpdated) {  // If the update involves non-platform software sets
        // Symlinks or A/B switch()
        // Delete the new version software set list application_swcl_list_ver(Y+1).json;
        LOGD << "DeleteSWCLListFile with lastUsedSWCLListID_+1";
        DeleteSWCLListFile(lastUsedSWCLListID_ + 1U);
    }
}

/// @brief used in exception rollback executor, to exception rollback to A partition and cleanup B partition
/// @throws no
void ABPartition::BootOptionFileExceptionRollbackToA() const
{
    // Delete the BootOption file
    LOGD << "lastUsedBootOptionFileID_:" << lastUsedBootOptionFileID_
         << " DeleteBootOptionFile with lastUsedBootOptionFileID_+1";
    DeleteBootOptionFile(lastUsedBootOptionFileID_ + 1U, true);
}

/// @brief used in exception rollback executor, to exception rollback to A partition and cleanup B partition
/// @throws no
void ABPartition::SwclListFileExceptionRollbackToA() const
{
    // 4. Link the original application software set list to the appropriate platform_core software set (i.e., the core_version in boot_option_verX.cfg)
    //     Clean the etc directory of the target platform_core software set version and create a new run_time_application_swcl_list.cfg
    //         Its process_list is application_swcl_list_verY.json, generate the corresponding md5;
    //         Generate the md5 of run_time_application_swcl_list.cfg GenBootConfig
    AraString formerFrameworkVer;
    AraString formerCoreVer;
    bool ret{false};
    ret = GetVersionFromBootConfig(lastUsedBootOptionFileID_, formerFrameworkVer, formerCoreVer);
    LOGD << "get formerCoreVer from lastUsedBootOptionFileID_:" << lastUsedBootOptionFileID_
         << ", call LinkSwclList with formerCoreVer:" << formerCoreVer.c_str()
         << " and lastUsedSWCLListID_:" << lastUsedSWCLListID_;
    AssertWithLog(ret == true);
    AssertWithLog(!formerFrameworkVer.empty());
    AssertWithLog(!formerCoreVer.empty());

    ret = LinkSwclList(lastUsedSWCLListID_, formerCoreVer);
    AssertWithLog(ret == true);

    // 5. Delete the new version software set list application_swcl_list_ver(Y+1).json;
    LOGD << "call DeleteSWCLListFile with lastUsedSWCLListID_+1:" << lastUsedSWCLListID_ + 1U;
    DeleteSWCLListFile(lastUsedSWCLListID_ + 1U, true);
}

/// @brief BootOptionFileExceptionCleanupB
/// @throws no
/// @return bool
bool ABPartition::BootOptionFileExceptionCleanupB() const
{
    LOGD << "lastUsedBootOptionFileID_:" << lastUsedBootOptionFileID_;

    // 9. Check the directory where the platform_core software set is located according to boot_option_verX.cfg, delete other versions, and set the software set status to Present accordingly
    AraString formerFrameworkVer;
    AraString formerCoreVer;
    bool ret{false};
    ret = _getFormerVersionFromBootConfig(formerFrameworkVer, formerCoreVer);
    const bool formerFrameworkVerEmpty{formerFrameworkVer.empty()};
    const bool formerCoreVerEmpty{formerCoreVer.empty()};
    if (!ret || formerFrameworkVerEmpty || formerCoreVerEmpty) {
        LOGE << "get formerFrameworkVer and formerCoreVer failed";
        return false;
    }

    LOGD << "CleanSwcl with retained formerFrameworkVer:" << formerFrameworkVer.c_str()
         << " formerCoreVer:" << formerCoreVer.c_str();
    ret = FileSystemSWCLManager::CleanSwcl(kFRAMEWORK_SWCL_NAME, formerFrameworkVer);
    if (!ret) {
        LOGE << "CleanSwcl with retained formerFrameworkVer failed";
        return false;
    }
    ret = FileSystemSWCLManager::CleanSwcl(kCORE_SWCL_NAME, formerCoreVer);
    if (!ret) {
        LOGE << "CleanSwcl with retained formerCoreVer failed";
        return false;
    }

    return true;
}

/// @brief SwclListFileExceptionCleanupB
/// @throws no
/// @return bool
bool ABPartition::SwclListFileExceptionCleanupB() const
{
    LOGD << "start with lastUsedSWCLListID_" << lastUsedSWCLListID_;

    // Load software set information from application_swcl_list_verY.json
    RManifestOpenAndCheck(doc, GetSwclListFilePath(lastUsedSWCLListID_), false);

    /// @brief SWCLInfo
    class SWCLInfo
    {
    public:
        /// @brief ManifestLoader
        /// @param node
        /// @return load result
        /// @throws no
        std::int32_t ManifestLoader(RManifestNode const& node)
        {
            this->name    = RManifestNodeGetSS(node, kSwclName);
            this->version = RManifestNodeGetSS(node, kSwclVersion);
            auto processVec{node.GetValue(std::move(AraStringView(kProcessList)), AraVector< ara::core::String >())};
            this->processList = AraVectorString(processVec.begin(), processVec.end());
            return isoft::kSuccess;
        }

    public:
        /// @brief SWCLIn
        AraString name;
        /// @brief SWCLIn
        AraString version;
        /// @brief SWCLIn
        AraVectorString processList;
    };
    AraVector< SWCLInfo > vecSwclInfoVec;
    RManifestLoadAndStrictCheck(doc, kSwcls, vecSwclInfoVec, false);

    std::map< AraString, AraString > swclVersionMap;
    std::ignore = std::for_each(
        vecSwclInfoVec.begin(),
        vecSwclInfoVec.begin() + static_cast< AraVector< SWCLInfo >::difference_type >(vecSwclInfoVec.size()),
        [&swclVersionMap](SWCLInfo const& info) { swclVersionMap[info.name] = info.version; });

    //  10. Check all software sets under /ara/swcls/ according to application_swcl_list_verY.json:
    //            Check if the software set exists in application_swcl_list_verY.json
    //                 If exists
    //                    Delete other versions, and set the software set status to Present accordingly
    //                 If does not exist
    //                    Delete the directory where the software set is located
    // Get all version names under the software set installation location
    AraVectorString swclDirList{tinyfsys::GetSubdirectories(GetPathSettings().swclsDir)};
    for (AraString const& stSwclDir : swclDirList) {
        AraString const swclName{tinyfs::Basename(stSwclDir)};

        std::map< AraString, AraString >::iterator const it{swclVersionMap.find(swclName)};
        if (it != swclVersionMap.end()) {  // Exists
            bool const cleanRet{FileSystemSWCLManager::CleanSwcl(swclName, it->second)};
            LOGD << "clean swcl:" << swclName.c_str() << " with retained version:" << it->second.c_str()
                 << ", ret:" << cleanRet;
        } else {  // Does not exist
            // Delete the directory where the software set is located
            AraResultVoid const rmRet{tinyfsys::RemoveDirectory(stSwclDir)};
            LOGD << "clean swcl:" << swclName.c_str() << " directly, ret:" << rmRet.HasValue();
        }
    }

    return true;
}

/// @brief CheckCoreVersionAfterReboot
/// @throws no
/// @return bool
bool ABPartition::CheckCoreVersionAfterReboot()
{
    AraString const coreVersionFromPath{GetCoreVersionFromRuntime()};
    // Get the new version information of platform_core
    AraString const coreVersionToCompare{GetVersionFromSwclStatus(kCORE_SWCL_NAME)};
    LOGD << "got coreVersionFromPath:" << coreVersionFromPath.c_str()
         << " coreVersionToCompare:" << coreVersionToCompare.c_str();
    return (0 == coreVersionToCompare.compare(coreVersionFromPath));
}

/// @brief CheckSwclListVersionAfterReboot
/// @throws no
/// @return bool
bool ABPartition::CheckSwclListVersionAfterReboot() const
{
    AraString const usedAppSWCLListVersion{GetSwclListVersionFromRuntime()};
    AraString const newSWCLListFileName{GetSwclListFileName(lastUsedSWCLListID_ + 1U)};
    LOGD << "got usedAppSWCLListVersion:" << usedAppSWCLListVersion.c_str()
         << " newSWCLListFileName:" << newSWCLListFileName.c_str();
    return (0 == usedAppSWCLListVersion.compare(newSWCLListFileName));
}

/// @brief CheckSwclListVersionWithoutUpdate
/// @throws no
/// @return bool
bool ABPartition::CheckSwclListVersionWithoutUpdate() const
{
    AraString const usedAppSWCLListVersion{GetSwclListVersionFromRuntime()};
    AraString const lastSWCLListFileName{GetSwclListFileName(lastUsedSWCLListID_)};
    LOGD << "got usedAppSWCLListVersion:" << usedAppSWCLListVersion.c_str()
         << " lastSWCLListFileName:" << lastSWCLListFileName.c_str();
    return (0 == usedAppSWCLListVersion.compare(lastSWCLListFileName));
}

/// @brief GetId
/// @param fileName
/// @param prefix
/// @param postfix
/// @throws no
/// @return file name id
std::int32_t ABPartition::GetId(AraString const& fileName, AraString const& prefix, AraString const& postfix)
{
    AraString const format{prefix + "%u%s"};
    std::int32_t id{0};  // id
    ///std::vector<char8_t> postfixTmp(kBuffLen, static_cast<char8_t>(0));.//mytodo666.
    char8_t postfixTmp[kBuffLen]{static_cast< char8_t >(0)};  // Suffix
    postfixTmp[0] = '\0';                                     // meaningless, just for QAC complaints

    std::int32_t const expect{2};
    std::int32_t const number{std::sscanf(fileName.c_str(),  // NOLINT : Recommend using C++ strtoul to replace sscanf
                                          format.c_str(), &id, static_cast< char8_t* >(postfixTmp))};

    const bool equal2Postfix{0 == postfix.compare(static_cast< char8_t* >(postfixTmp))};
    if ((expect == number) && equal2Postfix) {
        return id;
    }

    return -1;
}

/// @brief GetSortedFileNameIds
/// @param dir
/// @param prefix
/// @param postfix
/// @throws no
/// @return the vector of sorted file name ids
AraVector< std::uint32_t > ABPartition::GetSortedFileNameIds(AraString const& dir,
                                                             AraString const& prefix,
                                                             AraString const& postfix)
{
    AraVector< std::uint32_t > ids;
    std::int32_t id{0};

    AraVectorString const filePaths{tinyfsys::GetFiles(dir)};
    for (AraString const& filePath : filePaths) {
        AraString const fileName{tinyfsys::ExtractFileName(filePath)};
        id = GetId(fileName, prefix, postfix);
        if (id >= 0) {
            ids.push_back(static_cast< std::uint32_t >(id));
        }
    }

    sort(ids.begin(), ids.end());
    return ids;
}

/// @brief GetMinMaxFileNameId
/// @param dir
/// @param prefix
/// @param postfix
/// @param minId get min file name id
/// @param maxId get max file name id
/// @throws no
/// @return min max file name id
void ABPartition::GetMinMaxFileNameId(
    AraString const& dir, AraString const& prefix, AraString const& postfix, std::uint32_t& minId, std::uint32_t& maxId)
{
    AraVector< std::uint32_t > ids{GetSortedFileNameIds(dir, prefix, postfix)};
    std::size_t const idsLen{ids.size()};
    AssertWithLog((idsLen > 0U) && (idsLen < 3U));

    std::ignore = minId;
    std::ignore = maxId;
    minId       = ids[0U];           // the min value
    maxId       = ids[idsLen - 1U];  // the max value
}

/// @brief GetMinFileNameId
/// @param dir
/// @param prefix
/// @param postfix
/// @throws no
/// @return min file name id
std::uint32_t ABPartition::GetMinFileNameId(AraString const& dir, AraString const& prefix, AraString const& postfix)
{
    std::uint32_t minId{0U};
    std::uint32_t maxId{0U};
    GetMinMaxFileNameId(dir, prefix, postfix, minId, maxId);
    std::ignore = maxId;
    return minId;
}

/// @brief GetVersionFromSwclStatus
/// @param swclName
/// @param onlyGetCurVersion
/// @throws no
/// @return swcl version
AraString ABPartition::GetVersionFromSwclStatus(AraString const& swclName, bool const onlyGetCurVersion)
{
    // Get the software set status
    SwClusterStateType swclState;
    AraString curVer;
    AraString newVer;
    bool const ret{FileSystemSWCLManager::GetSwclStatus(swclName, swclState, curVer, newVer)};
    if (!ret) {
        LOGE << "GetSwclStatus failed";
    }

    // Based on the software set status, decide whether to use the current version or the new version
    AraString swclVerToConfig;
    if (swclState == SwClusterStateType::kPresent) {
        swclVerToConfig = curVer;
    } else if (swclState == SwClusterStateType::kUpdated) {
        swclVerToConfig = newVer;
    } else {
        LOGE << "GetSwclStatus invalid";
        // AssertWithLog(false);.  // Will not be kAdded/kRemoved.
    }

    if (onlyGetCurVersion) {
        swclVerToConfig = curVer;
    }

    LOGD << "got status info for swclName:" << swclName.c_str()
         << " swclState:" << FileSystemSWCLManager::SwclStateToStr(swclState).c_str()
         << " onlyGetCurVersion:" << onlyGetCurVersion << " curVersion:" << curVer.c_str()
         << " newVersion:" << newVer.c_str() << " swclVerToConfig:" << swclVerToConfig.c_str();

    return swclVerToConfig;
}

/// @brief GetVersionFromBootConfig
/// @param bootOptionFileId
/// @param frameworkVersion
/// @param coreVersion
/// @throws no
/// @return bool
bool ABPartition::GetVersionFromBootConfig(std::uint32_t const bootOptionFileId,
                                           AraString& frameworkVersion,
                                           AraString& coreVersion)
{
    AraString const bootOptionFilePath{GetBootOptionFilePath(bootOptionFileId)};
    AraString const kUcmSection{kFrameworkCoreSection};

    RManifestOpenAndCheck(doc, bootOptionFilePath, false);
    RManifestLoad(doc, kUcmSection + "." + kFrameworkVersion, frameworkVersion);
    RManifestLoad(doc, kUcmSection + "." + kCoreVersion, coreVersion);

    LOGD << "get bootOptionFilePath:" << bootOptionFilePath.c_str() << " frameworkVer:" << frameworkVersion.c_str()
         << " coreVer:" << coreVersion.c_str();
    const bool coreVersionNotEmpty{false == coreVersion.empty()};
    return ((false == frameworkVersion.empty()) && coreVersionNotEmpty);
}

/// @brief _getFormerVersionFromBootConfig
/// @param frameworkVer
/// @param coreVer
/// @throws no
/// @return bool
bool ABPartition::_getFormerVersionFromBootConfig(AraString& frameworkVer, AraString& coreVer) const
{
    return std::move(GetVersionFromBootConfig(lastUsedBootOptionFileID_, frameworkVer, coreVer));
}

/// @brief GetCoreVersionFromRuntime
/// @throws no
/// @return core version
AraString ABPartition::GetCoreVersionFromRuntime()
{
    // Determine if platform_core is the new version based on the current process path: if it is the new version, continue; otherwise, it indicates a switch failure, go to 4.1.4.5
    // Get the process path like: ara-sysroot/ara/core/1.5.0
    AraString const currentPath{tinyfs::GetCwd()};
    // Get the version number from the path
    AraString const coreVersion{tinyfs::Basename(currentPath)};
    LOGD << "got currentPath:" << currentPath.c_str() << " coreVersion:" << coreVersion.c_str();
    return coreVersion;
}

/// @brief GetSwclListVersionFromRuntime
/// @throws no
/// @return version
AraString ABPartition::GetSwclListVersionFromRuntime() noexcept { return ExecClient::GetCurrentUserSWCLManifest(); }

/// @brief GenFileNameWithId
/// @param prefix
/// @param fileId
/// @param postfix
/// @throws no
/// @return file name
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10056
/// @trace_id_dd=DD_UCM_10121
/// @needwork = dda
/// @endcode
static AraString GenFileNameWithId(char8_t const* const prefix,
                                   std::uint32_t const fileId,
                                   char8_t const* const postfix)
{
    return AraString(prefix) + std::to_string(fileId) + AraString(postfix);
}

/// @brief GetBootOptionFileName
/// @param bootOptionFileId
/// @throws no
/// @return file name
AraString ABPartition::GetBootOptionFileName(std::uint32_t const bootOptionFileId)
{
    return GenFileNameWithId(kBootOptionFileNamePrefix, bootOptionFileId, kBootOptionFileNamePostfix);
}

/// @brief GetSwclListFileName
/// @param swclListFileId
/// @throws no
/// @return file name
AraString ABPartition::GetSwclListFileName(std::uint32_t const swclListFileId)
{
    return GenFileNameWithId(kSWCLListFileNamePrefix, swclListFileId, kSWCLListFileNamePostfix);
}

/// @brief GetBootOptionFilePath
/// @param bootOptionFileId
/// @throws no
/// @return file path
AraString ABPartition::GetBootOptionFilePath(std::uint32_t const bootOptionFileId)
{
    return GetPathSettings().araDir + kPathSeparator
           + GenFileNameWithId(kBootOptionFileNamePrefix, bootOptionFileId, kBootOptionFileNamePostfix);
}

/// @brief GetBootOptionFilePath
/// @param bootOptionFileName
/// @throws no
/// @return file path
AraString ABPartition::GetBootOptionFilePath(AraString const& bootOptionFileName)
{
    return GetPathSettings().araDir + kPathSeparator + bootOptionFileName;
}

/// @brief GetBootOptionMD5FilePath
/// @param bootOptionFilePath
/// @throws no
/// @return file path
AraString ABPartition::GetBootOptionMD5FilePath(AraString const& bootOptionFilePath)
{
    AraString stMD5Path{bootOptionFilePath};
    std::ignore = strutil::ReplaceLast(stMD5Path, kBootOptionFileNamePostfix, kBootOptionMD5FileNamePostfix);
    return stMD5Path;
}

/// @brief GetSwclListFilePath
/// @param swclListFileId
/// @throws no
/// @return file path
AraString ABPartition::GetSwclListFilePath(std::uint32_t const swclListFileId)
{
    return GetPathSettings().swclsDir + kPathSeparator
           + GenFileNameWithId(kSWCLListFileNamePrefix, swclListFileId, kSWCLListFileNamePostfix);
}

/// @brief GetSwclListFilePath
/// @param swclListFileName
/// @throws no
/// @return file path
AraString ABPartition::GetSwclListFilePath(AraString const& swclListFileName)
{
    return GetPathSettings().swclsDir + kPathSeparator + swclListFileName;
}

/// @brief LinkSwclList
/// @param swclListFileId
/// @param coreVersion
/// @throws no
/// @return bool
bool ABPartition::LinkSwclList(std::uint32_t const swclListFileId, AraString const& coreVersion)
{
    // Get the path of the software set list file
    AraString const swclListFileName{GetSwclListFileName(swclListFileId)};
    AraString const swclListFilePath{GetSwclListFilePath(swclListFileName)};
    // Get the MD5 value of the software set list file
    AraString const kMD5Str{Crypto().MD5ForFile(swclListFilePath)};
    AssertWithLog(!kMD5Str.empty());

    // core etc path
    AraString const coreEtcPath{GetPath().GetSwclInstallDir(kCORE_SWCL_NAME) + kPathSeparator + coreVersion
                                + kPathSeparator + kPLATFORM_CORE_SWCL_ETC_DIR + kPathSeparator};
    // Update the application-level software set list configuration file
    AraString const appSwclListCfgFilePath{coreEtcPath + kAPPLICATION_SWCL_LIST_CFG_FILE_NAME};
    rjson::Doc appSwclListDom{rjson::MakeDomObject()};
    rjson::Value appSwclListSectionDom{rjson::MakeObject()};
    rjson::AddKS(appSwclListDom, appSwclListSectionDom, kFileNameKey, swclListFileName);
    rjson::AddKS(appSwclListDom, appSwclListSectionDom, kMD5Key, kMD5Str);
    rjson::AddKO(appSwclListDom, appSwclListDom, kApplicationSWCLListSection, appSwclListSectionDom);
    rjson::SavePretty(appSwclListDom, appSwclListCfgFilePath);

    // Update the application-level software set list configuration md5 file
    AraString const appSwclListCfgMD5FilePath{coreEtcPath + kAPPLICATION_SWCL_LIST_MD5_FILE_NAME};
    AraString const kMD5Str4Cfg{Crypto().MD5ForFile(appSwclListCfgFilePath)};
    AssertWithLog(!kMD5Str4Cfg.empty());
    tinyfs::OverWriteToFile(appSwclListCfgMD5FilePath, kMD5Str4Cfg);

    LOGD << "platformCoreVersion:" << coreVersion.c_str() << " swclListFileId:" << swclListFileId
         << ", write swclListFilePath:" << swclListFilePath.c_str() << " and MD5Str:" << kMD5Str.c_str()
         << " into appSwclListCfgFilePath:" << appSwclListCfgFilePath.c_str()
         << ", then write MD5Str4Cfg:" << kMD5Str4Cfg.c_str()
         << " into appSwclListCfgMD5FilePath:" << appSwclListCfgMD5FilePath.c_str();
    return true;
}

/// @brief _genBootConfig
/// @param bootOptionFileId
/// @param frameworkVersion
/// @param coreVersion
/// @throws no
/// @return bool
bool ABPartition::_genBootConfig(std::uint32_t const bootOptionFileId,
                                 AraString const& frameworkVersion,
                                 AraString const& coreVersion) const
{
    LOGD << "start with bootOptionFileId:" << bootOptionFileId << " frameworkVersion:" << frameworkVersion.c_str()
         << " platformCoreVersion:" << coreVersion.c_str();

    AraString bootOptionFilePath;
    AraString formerBootOptionFilePath;
    AraString bootOptionMD5FilePath;
    AraString stMD5Str4BootOption;

    // Generate the boot option file
    bootOptionFilePath       = GetBootOptionFilePath(bootOptionFileId);
    formerBootOptionFilePath = GetBootOptionFilePath(lastUsedBootOptionFileID_);
    LOGD << "get bootOptionFilePath:" << bootOptionFilePath.c_str()
         << " formerBootOptionFilePath:" << formerBootOptionFilePath.c_str();

    rjson::Doc araVerCfgDom;
    bool const ret{rjson::OpenAndCheck(araVerCfgDom, formerBootOptionFilePath, std::move(LOGD))};
    if (!ret) {
        return false;
    }
    rjson::Value ucmDom{rjson::MakeObject()};
    rjson::AddKS(araVerCfgDom, ucmDom, kFrameworkVersion, frameworkVersion.c_str());
    rjson::AddKS(araVerCfgDom, ucmDom, kCoreVersion, coreVersion.c_str());
    if (araVerCfgDom.HasMember(kFrameworkCoreSection)) {
        std::ignore = araVerCfgDom.RemoveMember(kFrameworkCoreSection);
    }
    rjson::AddKO(araVerCfgDom, araVerCfgDom, kFrameworkCoreSection, ucmDom);
    rjson::SavePretty(araVerCfgDom, bootOptionFilePath.c_str());

    // Generate the md5 file corresponding to the boot option file
    bootOptionMD5FilePath = GetBootOptionMD5FilePath(bootOptionFilePath);
    stMD5Str4BootOption   = Crypto().MD5ForFile(bootOptionFilePath);
    LOGD << "get bootOptionMD5FilePath" << bootOptionMD5FilePath.c_str()
         << " to write MD5Str4BootOption:" << stMD5Str4BootOption.c_str();

    AssertWithLog(!stMD5Str4BootOption.empty());
    tinyfs::OverWriteToFile(bootOptionMD5FilePath, stMD5Str4BootOption);

    return true;
}

/// @brief DeleteBootOptionFile
/// @param bootOptionFileId
/// @param mayNotExist
/// @throws no
void ABPartition::DeleteBootOptionFile(std::uint32_t const bootOptionFileId, bool const mayNotExist)
{
    AraString bootOptionFilePath;
    AraString bootOptionMD5FilePath;
    AraResultVoid ret;

    bootOptionFilePath    = GetBootOptionFilePath(bootOptionFileId);
    bootOptionMD5FilePath = GetBootOptionMD5FilePath(bootOptionFilePath);
    LOGD << "got bootOptionFilePath:" << bootOptionFilePath.c_str()
         << " bootOptionMD5FilePath:" << bootOptionMD5FilePath.c_str() << " mayNotExist:" << mayNotExist
         << " now remove them";

    ret = tinyfsys::RemoveFile(bootOptionMD5FilePath);
    if (!ret.HasValue()) {
        LOGE << "rm bootOptionMD5FilePath err:" << ret.Error().Message().data();
    }

    ret = tinyfsys::RemoveFile(bootOptionFilePath);
    if (!ret.HasValue()) {
        LOGE << "rm bootOptionFilePath err:" << ret.Error().Message().data();
    }

    if (!mayNotExist) {
        AssertWithLog(ret.HasValue());
    }
}

/// @brief DeleteSWCLListFile
/// @param swclListId
/// @param mayNotExist
/// @throws no
void ABPartition::DeleteSWCLListFile(std::uint32_t const swclListId, bool const mayNotExist)
{
    AraString swclListFilePath;
    AraResultVoid ret;

    swclListFilePath = GetSwclListFilePath(swclListId);
    LOGD << "get swclListFilePath:" << swclListFilePath.c_str() << " mayNotExist:" << mayNotExist << " now remove it";

    ret = tinyfsys::RemoveFile(swclListFilePath);
    if (!ret.HasValue()) {
        LOGE << "rm swclListFilePath err:" << ret.Error() << " " << ret.Error().Message().data();
    }

    if (!mayNotExist) {
        AssertWithLog(ret.HasValue());
    }
}

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara
