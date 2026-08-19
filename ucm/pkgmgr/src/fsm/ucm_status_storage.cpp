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
/// @file       ucm_status_storage.cpp
/// @brief      UCM status storage implementation
/// @details
/// @date       2024-08-15
/// @author     cuiyinli
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/UCM/FsmManager
/// @interface_level=unit
/// @trace_id_sr=SR_UCM_00001
/// @unit_name=UcmStatusStorage
/// @unit_description=UCM status storage implementation
/// @endcode
///
/// ================================================================

#include "ucm_status_storage.h"

#include "ara/ucm/internal/crypto/crypto.h"
#include "ara/ucm/internal/extraction/tinyfs.h"
#include "ara/ucm/internal/extraction/tinyfsys.h"
#include "common/assert.h"
#include "common/const.h"
#include "common/log.h"
#include "common/path.h"
#include "common/strtype.h"
#include "types/impl_type_finalactiontype.h"
#include "util/rjson.h"

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @brief check consistency between ucm status file and ucm status md5 file
/// @return result
/// @throws no
UcmStatusStorage::CheckResultType UcmStatusStorage::CheckConsistency() noexcept
{
    AraString const& ucmStatusFilePath{GetPathSettings().uCMStatusPath};
    AraString const& ucmStatusMD5FilePath{GetPathSettings().uCMStatusMD5Path};
    AraString const kMD5Str{Crypto().MD5ForFile(ucmStatusFilePath)};
    AraString const kMD5StrFromMD5File{tinyfs::ReadMD5FromFile(ucmStatusMD5FilePath)};

    LOGD << "ucmStatusFilePath:" << ucmStatusFilePath.c_str()
         << ", ucmStatusMD5FilePath:" << ucmStatusMD5FilePath.c_str() << ", MD5Str:" << kMD5Str.c_str()
         << ", MD5StrFromMD5File:" << kMD5StrFromMD5File.c_str();

    const bool bMD5StrFromMD5FileEmpty{kMD5StrFromMD5File.empty()};
    if (kMD5Str.empty() && bMD5StrFromMD5FileEmpty) {  // ucm_status.json file and ucm_status.md5 file not found
        LOGD << "both ucmStatusFilePath and ucmStatusMD5FilePath don't exist";
        return UcmStatusStorage::CheckResultType::kUCMStatusAllFileEmpty;
    }
    if (kMD5Str.empty()) {  // ucm_status.json file not found
        LOGW << "can't get MD5 for ucmStatusFilePath";
        return UcmStatusStorage::CheckResultType::kUCMStatusFileEmpty;
    }
    if (bMD5StrFromMD5FileEmpty) {  // ucm_status.md5 file not found
        LOGW << "can't get MD5 from ucmStatusMD5FilePath";
        return UcmStatusStorage::CheckResultType::kUCMStatusMD5FileEmpty;
    };

    if (0 != kMD5Str.compare(kMD5StrFromMD5File)) {
        LOGW << "MD5Str is not equal with MD5StrFromMD5File";
        return UcmStatusStorage::CheckResultType::kUCMStatusMD5NotConsistent;
    }

    return UcmStatusStorage::CheckResultType::kUCMStatusMD5Consistent;
}

/// @brief remove ucm status md5 file
/// @throws no
void UcmStatusStorage::BeginSaveTransaction() noexcept
{
    AraString const& ucmStatusMD5FilePath{GetPathSettings().uCMStatusMD5Path};
    LOGD << "remove ucmStatusMD5FilePath:" << ucmStatusMD5FilePath.c_str();

    // Delete the MD5 file of the UCM status
    AraResultVoid const ret{tinyfsys::RemoveFile(ucmStatusMD5FilePath)};
    if (!ret.HasValue()) {
        LOGD << "remove ucmStatusMD5FilePath err:" << ret.Error().Message().data();
    }
}

/// @brief update ucm status file
/// @param st
/// @param actionExecutionInfos
/// @throws no
void UcmStatusStorage::SaveCurrentStatus(PackageManagerState const& st,
                                         AraList< ActionExecutionInfoType > const& actionExecutionInfos) noexcept
{
    AraString const& ucmStatusFile{GetPathSettings().uCMStatusPath};
    PackageManagerStatusType const status{st.GetStatus()};
    FinalActionType const finalActionType{st.GetFinalActionType()};

    LOGD << "start with ucmStatusFile:" << ucmStatusFile.c_str() << ", kFixedUCMVersion:" << kFixedUCMVersion
         << ", status:" << strtype::PackageManagerStatusTypeToStr(status).c_str()
         << ", finalActionType:" << strtype::FinalActionTypeToStr(finalActionType).c_str();

    // (With the current UCM version) First modify/overwrite ucm_status.json and delete the corresponding md5 file;    --- Must not delete and rewrite
    // Execute the corresponding operation; (there may be no operation).
    // Generate ucm_status.json and its corresponding ucm_status.md5;
    // --- Can save in IDLE, READY, ACTIVATED, ROLLED-BACK (these 4 states have no work to do and wait for user calls). (Although READY saves, an unexpected power off during PROCESSING state will lose previously processed software packages).
    // --- When recovering to these states, ucm_status.json should not be rewritten
    // --- Must save before restarting in VERIFYING/ROLLING-BACK. RollingBack involves conversion from new UCM version to old UCM version, also save at this time
    // --- Recovering to VERIFYING will invalidate ucm_status.json; recovering to ROLLING-BACK will not invalidate ucm_status.json
    // --- Save also when entering CLEANING_UP (to avoid going all the way to MyROLLING-BACK), and save FinalAction at the same time, so it can be redone after restart? en
    // --- When recovering to this state, ucm_status.json should not be rewritten
    // --- Rollback cleanup state: save before restarting in ROLLING-BACK-AND-CLEANING_UP
    // --- When recovering to ROLLING-BACK-AND-CLEANING_UP, ucm_status.json will not be invalidated
    // Normal state transition process: Persist new state related information; Transition to new state; Perform operations related to the new state;
    // State recovery process after restart: Read persisted state related information; Recover to that specified state; Perform operations related to the specified state;
    // Divided into interfaces: Begin, Set, Commit

    // [{
    // 	"Version": "1.0.0",.
    // 	"CurrentStatus": "Idle",.
    // 	"FinalAction": "Revert/Commit",.
    // Save only when entering the CLEANING_UP state from PROCESSING/READY/ACTIVATED/ROLLED-BACK, and read when recovering to the CLEANING_UP state
    // (For consistency, other states can save/read empty) 	"ProcessedSoftwarePackages":
    //      [
    //            {
    //                 "TransferId":"2",.
    //                 "actionResolution":"kSuccessfull",                --- Is it really unnecessary to recover these to memory? Or is recovery needed?
    //                 "actionTimeStamp":"2021/09/17 18:32:55"
    //             },
    //             {
    //                 "TransferId":"3",.
    //                 "actionResolution":"kSuccessfull",.
    //                 "actionTimeStamp":"2021/09/17 18:32:56"
    //             }
    //      ]
    // }]

    // No more multiple versions
    rjson::Doc statusDom{rjson::MakeDomObject()};
    rjson::AddKS(statusDom, statusDom, kUCMVersion, kFixedUCMVersion);
    rjson::AddKN(statusDom, statusDom, kUCMCurrentStatus, static_cast< uint64_t >(status));
    rjson::AddKN(statusDom, statusDom, kUCMFinalAction, static_cast< uint64_t >(finalActionType));

    // ProcessedSoftwarePackages
    rjson::Value processedSwpkgDom{rjson::MakeArray()};
    for (ActionExecutionInfoType const& it : actionExecutionInfos) {
        LOGD << "save " << it.Str().c_str();

        rjson::Value actionDom{rjson::MakeObject()};
        rjson::AddKN(statusDom, actionDom, kUCMProcessedSoftwarePackageActionType,
                     static_cast< uint64_t >(it.actionType));
        rjson::AddKN(statusDom, actionDom, kUCMProcessedSoftwarePackageActivateOptionType,
                     static_cast< uint64_t >(it.activateOptionType));
        rjson::AddKS(statusDom, actionDom, kUCMProcessedSoftwarePackagedeltaPackageApplicableVersion,
                     it.deltaPackageApplicableVersion);
        rjson::AddKS(statusDom, actionDom, kUCMProcessedSoftwarePackageSwclName, it.swclName);
        rjson::AddKS(statusDom, actionDom, kUCMProcessedSoftwarePackageCurSwclVersion, it.curSWCLVersion);
        rjson::AddKS(statusDom, actionDom, kUCMProcessedSoftwarePackageSwclVersion, it.swclVersion);
        rjson::AddKN(statusDom, actionDom, kUCMProcessedSoftwarePackageActionResolution,
                     static_cast< uint64_t >(it.resolution));
        rjson::AddKN(statusDom, actionDom, kUCMProcessedSoftwarePackageActionTimeStamp,
                     static_cast< uint64_t >(it.time));
        rjson::PushO(statusDom, processedSwpkgDom, actionDom);
    }
    rjson::AddKO(statusDom, statusDom, kUCMProcessedSoftwarePackages, processedSwpkgDom);

    rjson::Save(statusDom, ucmStatusFile);
}

/// @brief generate ucm status md5 file
/// @throws no
void UcmStatusStorage::CommitSaveTransaction() noexcept
{
    // Generate the MD5 value based on the UCM status file
    AraString const& ucmStatusFile{GetPathSettings().uCMStatusPath};
    AraString const& ucmStatusMD5File{GetPathSettings().uCMStatusMD5Path};
    AraString const kMD5Str{Crypto().MD5ForFile(ucmStatusFile)};
    LOGD << "ucmStatusFile:" << ucmStatusFile.c_str() << " ucmStatusMD5File:" << ucmStatusMD5File.c_str()
         << " MD5Str:" << kMD5Str.c_str() << " now write MD5Str into ucmStatusMD5File";

    // Write the md5 value to the MD5 file
    AssertWithLog(!kMD5Str.empty());
    tinyfs::OverWriteToFile(ucmStatusMD5File, kMD5Str);
}

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara
