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
/// @file       process_executor.cpp
/// @brief      ProcessExecutor implementation
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
/// @unit_name=ProcessExecutor
/// @unit_description=ProcessExecutor implementation
/// @endcode
///
/// ================================================================

#include "process_executor.h"

#include <type_traits>

#include "ara/ucm/internal/transfer/streamable_software_package.h"
#include "common/assert.h"
#include "common/const.h"
#include "common/errc.h"
#include "common/log.h"
#include "common/path.h"
#include "common/strtype.h"
#include "fsm/fsm_manager.h"
#include "storage/action_generator.h"
#include "storage/ucm_database.h"
#include "util/future_ext.h"

/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
#define ReturnVoidErrcWithLongLogWith_setState(msg, stateEnum, errc)                                                   \
    pkgItem->SetState(SwPackageStateType::stateEnum);                                                                  \
    ReturnVoidErrcWithLongLog(msg, errc)
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
#define ReturnVoidErrcEnumWithLongLogWith_setState(msg, stateEnum, errcEnum)                                           \
    ReturnVoidErrcWithLongLogWith_setState(msg, stateEnum, UCMErrorDomainErrc::errcEnum)
/*//#define ReturnVoidErrcWithShortLogWith_setState(stateEnum, errc)  \
    ReturnVoidErrcWithLongLogWith_setState("", stateEnum, errc)
//#define ReturnVoidErrcEnumWithShortLogWith_setState(stateEnum, errcEnum)  \
    ReturnVoidErrcEnumWithLongLogWith_setState("", stateEnum, errcEnum)*/

/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
#define ReturnVoidErrcWithLongLogWithCleanup(msg, errc)                                                                \
    owner_->swpkgManager.GetExtractor().Cleanup();                                                                     \
    ReturnVoidErrcWithLongLog(msg, errc)
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
#define ReturnVoidErrcWithLongLogWith_setStateWithCleanup(msg, stateEnum, errc)                                        \
    owner_->swpkgManager.GetExtractor().Cleanup();                                                                     \
    ReturnVoidErrcWithLongLogWith_setState(msg, stateEnum, errc)
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
#define ReturnVoidErrcEnumWithLongLogWith_setStateWithCleanup(msg, stateEnum, errcEnum)                                \
    ReturnVoidErrcWithLongLogWith_setStateWithCleanup(msg, stateEnum, UCMErrorDomainErrc::errcEnum)
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
#define ReturnVoidErrcWithLongLogWith_setStateWithCleanupAll(msg, stateEnum, errc)                                     \
    owner_->swpkgManager.CleanupAfterExtraction(id);                                                                   \
    ReturnVoidErrcWithLongLogWith_setState(msg, stateEnum, errc)
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
#define ReturnVoidErrcEnumWithLongLogWith_setStateWithCleanupAll(msg, stateEnum, errcEnum)                             \
    ReturnVoidErrcWithLongLogWith_setStateWithCleanupAll(msg, stateEnum, UCMErrorDomainErrc::errcEnum)

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @brief Clean up the temporary files
/// @param id Transfer ID of the package to be processed.
/// @throws no
/// @return result
AraFutureVoid ProcessExecutor::RunInThread(TransferIdType const& id) noexcept
{
    LOGD << "call...";
    AraString const func{__func__};

    // init for cancel
    cancelledToken_.MarkAsNoCancellRequested();
    processedId_ = id;

    ASYNC_CALL_VOID_BEGIN_WITH_FUNC_ID(func, id)
    {
        AraResultVoid const ret{ProcessSoftwarePackage(id)};
        if (ret.HasValue()) {
            owner_->OnProcessSwPackageSucceeded();
        } else {
            owner_->OnProcessSwPackageFailed();
        }

        cancelledToken_.MarkAsNoCancellRequested();
        processedId_ = TransferIdType{0U};
        owner_->swpkgManager.SetStreaming(std::unique_ptr< AraPromiseVoid >(nullptr), processedId_);

        return ret;
    }
    ASYNC_CALL_END();
}

/// @brief Processing of a sofware package
///
/// @param id Transfer ID of the package to be processed.
///
/// @return The result of of processing this sw package
AraResultVoid ProcessExecutor::ProcessSoftwarePackage(TransferIdType const& id) noexcept
{
    LOGI << "call... TransferId:" << strtype::TransferIdTypeToHexStr(id).c_str();
    AraResultVoid ret;

    std::uint8_t const progress25U{25U};
    std::uint8_t const progress50U{50U};
    std::uint8_t const progress75U{75U};
    std::uint8_t const progress100U{100U};

    LOGD << "sync wait for cancel";
    std::lock_guard< std::timed_mutex > const syncCancel{cancelledLock_};

    std::shared_ptr< StreamableSoftwarePackage > pkgItem{owner_->swpkgManager.GetPackagesData().GetItem(id)};
    if (pkgItem == nullptr) {
        ReturnVoidErrcEnumWithLongLog("pkgItem not found", kInvalidTransferId);
    }

    AraString const pkgPath{pkgItem->GetPackageFilename()};
    AraString const extractionDir{Path::GetExtractionTargetDir(pkgPath)};
    LOGI << "get PackageFilename:" << pkgPath.c_str() << ", extractionDir:" << extractionDir.c_str();

    // Is it processing from a stream?
    bool processingFromStream{false};
    {
        // Mutex for processing actions from a stream
        LOGD << "lock mutexForProcessingFromStream_ for id:" << strtype::TransferIdTypeToHexStr(id).c_str();
        std::unique_lock< std::mutex > lock{owner_->swpkgManager.GetStreamingLock()};

        // Get the status of the software package
        SwPackageStateType const pkgState{pkgItem->GetState()};
        LOGI << "pkgState:" << strtype::SwPackageStateTypeToStr(pkgState).c_str();

        if (pkgState == SwPackageStateType::kTransferring) {  // Being transferred
            pkgItem->SetState(SwPackageStateType::kProcessingStream);

            std::unique_ptr< AraPromiseVoid > streamingPromise{std::make_unique< AraPromiseVoid >()};
            AraFutureVoid streamingFuture{streamingPromise->get_future()};
            owner_->swpkgManager.SetStreaming(std::move(streamingPromise), id);  //////////mytodo666////

            LOGD << "unlock after SetStreaming for id";
            lock.unlock();  // Unlock

            LOGI << "the package is kTransferring and we are waiting for it to finish...";
            streamingFuture.wait();  // may block forever?? //the streamingPromise will be set by swpkgManager transfer

            LOGD << "lock again for id";
            lock.lock();  // Re-lock
            ret = streamingFuture.GetResult();
            if (!ret.HasValue()) {
                ReturnVoidErrcWithLongLog("streaming failed", GetAraResultErrc(ret));
            }

            // Currently processing from a stream
            processingFromStream = true;
            LOGI << "the streaming successfully finished and we will continue";

        } else if (pkgState == SwPackageStateType::kTransferred) {  // Transfer completed
            LOGD << "the package is kTransferred and we will continue";
            pkgItem->SetState(SwPackageStateType::kProcessing);
        } else {
            ReturnVoidErrcEnumWithLongLog("unknown packageState", kOperationNotPermitted);
        }
    }

    pkgItem->SetProcessProgressValue(0U);

    LOGI << "check IsMarkedToBeCancelled... 1";
    if (cancelledToken_.IsMarkedToBeCancelled()) {
        cancelledToken_.MarkAsCancelled();

        // "Cancel" call from client
        pkgItem->SetProcessProgressValue(0U);
        ReturnVoidErrcEnumWithLongLogWith_setStateWithCleanup("interrupted by cancel, cleanup", kTransferred,
                                                              kProcessSwPackageCancelled);
    }

    pkgItem->SetProcessProgressValue(progress25U);

    // added by hanzhibo
    // ProcessSoftwarePackage (re)decompresses at the beginning; deletes the directory at the end (content is already empty)   --- It is also reasonable as part of the installation
    //  AUTOSAR_SWS_UpdateAndConfigManagement.pdf has:
    //      The processing (i.e. unpacking and actually applying changes to the AUTOSAR Adaptive Platform) of Software
    //      Packages described by the state kProcessing is further detailed in Sect. 7.1.5.
    if (!processingFromStream) {  // Not processing from a stream
        // Extract (decompress) the zip package packageFilename to the extractionDirectory
        // Extract the SWCL if possible
        if (!owner_->swpkgManager.GetExtractor().Extract(pkgPath, extractionDir)) {
            pkgItem->SetProcessProgressValue(0U);
            // TODO: Distinguish between insufficient memory and zip format issues
            ReturnVoidErrcEnumWithLongLogWith_setStateWithCleanup("extract failed", kTransferred, kInsufficientMemory);
        }
    }

    std::unique_ptr< SoftwarePackage > swpkg{owner_->swpkgManager.GetParser().Parse(extractionDir)};
    if (swpkg == nullptr) {
        ReturnVoidErrcEnumWithLongLogWith_setStateWithCleanupAll(extractionDir.c_str() << " parse failed", kProcessed,
                                                                 kInvalidPackageManifest);
    }

    // Verify the software package file in the decompressed directory
    ret = swpkg->AuthenAndCheckConsistency();
    if (!ret.HasValue()) {
        ReturnVoidErrcWithLongLogWith_setStateWithCleanupAll(extractionDir.c_str() << " authen failed", kProcessed,
                                                             GetAraResultErrc(ret));
    }

    SoftwarePackageManifest const& swpkgManifest{swpkg->GetManifest()};
    Version const fixedUCMVersion{kFixedUCMVersion};
    if (swpkgManifest.minUCMSupportedVersion > fixedUCMVersion) {
        ReturnVoidErrcEnumWithLongLogWith_setStateWithCleanupAll(
            "The swpkg version is not compatible with the current UCM version "
                << kFixedUCMVersion
                << ", minUCMSupportedVersion:" << swpkgManifest.minUCMSupportedVersion.ToString().c_str(),
            kProcessed, kIncompatiblePackageVersion);
    }

    pkgItem->SetProcessProgressValue(progress50U);

    LOGI << "check IsMarkedToBeCancelled... 2";
    if (cancelledToken_.IsMarkedToBeCancelled()) {
        cancelledToken_.MarkAsCancelled();

        // "Cancel" call from client
        pkgItem->SetProcessProgressValue(0U);
        ReturnVoidErrcEnumWithLongLogWith_setStateWithCleanup("interrupted by cancel, cleanup", kTransferred,
                                                              kProcessSwPackageCancelled);
    }

    // Create, execute and maybe store the action to make it possible to revert or commit it later
    {
        AraString const swpkgCategory{swpkgManifest.category};
        AraString const swpkgName{swpkgManifest.shortName};
        ActionType const actionType{swpkgManifest.actionType};
        ActivateOptionType const activateOptionType{swpkgManifest.activationAction};
        AraString const deltaPackageApplicableVersion{swpkgManifest.deltaPackageApplicableVersion};

        SoftwareClusterManifest const& swclManifest{swpkg->GetSoftwareCluster().GetSwclManifest()};
        AraString const swclName{swclManifest.shortName};
        AraString const swclVersion{swclManifest.version.ToString()};

        // Current software cluster information
        SwClusterStateType swclState;
        AraString curVersionStr;
        AraString newVersionStr;
        FileSystemSWCLManager::GetSwclStatus(swclName, swclState, curVersionStr, newVersionStr);
        std::ignore = swclState;

        // appLayerSWCLDir_ is like: /home/xxx/ara-sysroot/var/lib/apd_ucm/swcls
        LOGI << "try create action{id:" << strtype::TransferIdTypeToHexStr(id).c_str()
             << ", pkgPath:" << pkgPath.c_str() << ", swpkgCategory:" << swpkgCategory.c_str()
             << ", swpkgName:" << swpkgName.c_str() << ", swclName:" << swclName.c_str()
             << ", curVersionStr:" << curVersionStr.c_str() << ", swclVersion:" << swclVersion.c_str()
             << ", actionType:" << strtype::ActionTypeToStr(actionType).c_str()
             << ", activateOptionType:" << strtype::ActivateOptionTypeToStr(activateOptionType).c_str()
             << ", deltaPackageApplicableVersion:" << deltaPackageApplicableVersion.c_str() << "}";

        // Determine whether the software set and version in the software package already exist in the action list
        if (owner_->swclManager.ExistsInActions(swclName, swclVersion)) {
            pkgItem->SetProcessProgressValue(0U);
            ReturnVoidErrcEnumWithLongLogWith_setStateWithCleanup(
                swclName.c_str() << ":" << swclVersion.c_str() << " has already existed in action list", kTransferred,
                kInvalidPackageManifest);
        }

        std::unique_ptr< ReversibleAction > currentAction{
            ActionGenerator()(static_cast< std::underlying_type< ActionType >::type >(actionType),
                              static_cast< std::underlying_type< ActivateOptionType >::type >(activateOptionType),
                              deltaPackageApplicableVersion, swclName, curVersionStr, swclVersion, std::move(swpkg))};
        if (currentAction == nullptr) {
            LOGE << "failed to create action, because its type is unknown";
        }

        AraResult< ResultType > const executeRet{currentAction->Execute()};
        if (executeRet == ResultType::kSuccessfull) {
            owner_->swclManager.AddSWCLChangeInfo(swclName, swclVersion, actionType);
            owner_->swclManager.AddAction(std::move(currentAction));

        } else if (executeRet == ResultType::kFailed) {
            LOGF << "action execute failed";
            AssertWithLog(false);
        } else {
            LOGW << "created action:" << swpkgCategory.c_str() << " for " << swpkgName.c_str()
                 << " could not be executed";

            GetHistoryType record;
            record.Name       = swclName;
            record.Version    = swclVersion;
            record.Action     = actionType;
            record.Resolution = ResultType::kFailed;

            AraList< GetHistoryType > const actionsInfo{record};
            std::ignore = UcmDatabase(GetPathSettings().historyPath).SaveLastSnapshot(actionsInfo);

            if (executeRet.CheckError(
                    UCMErrorDomainErrc::kIncompatibleDelta)) {  // Is it a delta version incompatibility error?
                ReturnVoidErrcEnumWithLongLogWith_setStateWithCleanupAll("execute failed with errc:kIncompatibleDelta",
                                                                         kProcessed, kIncompatibleDelta);
            }

            // Return errors that occurred in Execute
            ReturnVoidErrcWithLongLogWithCleanup("execute failed with errc:other", GetAraResultErrc(executeRet));
        }
    }

    pkgItem->SetProcessProgressValue(progress75U);
    owner_->swpkgManager.GetExtractor().Cleanup();

    pkgItem->SetProcessProgressValue(progress100U);
    pkgItem->SetState(SwPackageStateType::kProcessed);

    return ret;
}

/// @brief Clean up the temporary files
/// @param id Transfer ID of the package to be processed.
/// @param isRevert commit or revert
/// @throws no
/// @return result
AraResultVoid ProcessExecutor::Cancel(TransferIdType const& id, bool const isRevert) noexcept
{
    LOGD << "call with id:" << strtype::TransferIdTypeToHexStr(id).c_str()
         << " processedId_:" << strtype::TransferIdTypeToHexStr(processedId_).c_str() << " isRevert:" << isRevert;

    if (!processedId_.empty()) {
        if (processedId_ != id) {
            ReturnVoidErrcEnumWithLongLog("processedId_ != id", kInvalidTransferId);
        }
    }

    // Mark as canceled
    LOGD << "token MarkToBeCancelled";
    cancelledToken_.MarkToBeCancelled();

    // Wait synchronously
    LOGD << "synchronous wait for processing swpkg thread exit";
    std::unique_lock< std::timed_mutex > syncCancel{cancelledLock_, std::defer_lock};
    std::int32_t const waittime{1000};
    if (!syncCancel.try_lock_for(std::chrono::milliseconds(waittime))) {  // it is useful to check result of lock
        LOGE << "failed to sync wait for processing swpkg thread exit";
    }

    // Determine if cancellation was successful
    bool const ret{cancelledToken_.IsMarkedAsCancelled()};

    // Set the cancel request flag
    cancelledToken_.MarkAsNoCancellRequested();

    if (!ret) {
        const UCMErrorDomainErrc errc{isRevert ? UCMErrorDomainErrc::kNotAbleToRevertPackages
                                               : UCMErrorDomainErrc::kCancelFailed};
        ReturnVoidErrcWithLongLog("not IsMarkedAsCancelled", UCMErrorDomainErrc(errc));
    }

    return {};
}

/// @brief Clean up the temporary files
/// @throws no
/// @return result
AraResultVoid ProcessExecutor::CancelForRevert() noexcept { return Cancel(processedId_, true); }

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara
