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
/// @file       software_package_manager.cpp
/// @brief      Software package manager implementation
/// @details
/// @date       2024-08-15
/// @author     cuiyinli
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/UCM/SoftwarePackageManager
/// @interface_level=module
/// @trace_id_sr=SR_UCM_00001
/// @unit_name=SoftwarePackageManager
/// @unit_description=Implementation of the manager of Software Package, to transfer software package data, extract software package, parse software package
/// @endcode
///
/// ================================================================

#include "software_package_manager.h"

#include "ara/ucm/internal/extraction/tinyfsys.h"
#include "ara/ucm/internal/transfer/serial_id_generator.h"
#include "ara/ucm/internal/transfer/software_package_data_factory.h"
#include "ara/ucm/pkgmgr/error_domain_ucmerrordomain.h"
#include "common/const.h"
#include "common/errc.h"
#include "common/log.h"
#include "common/path.h"
#include "common/strtype.h"
#include "storage/filesystem_swcl_manager.h"
#include "storage/ucm_database.h"
#include "util/future_ext.h"

/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
#define ReturnVoidErrcWithLongLogWithStreaming(msg, errc)                                                              \
    const bool equalStreamingId{streamingId_ == id};                                                                   \
    if ((streaming_ != nullptr) && equalStreamingId) {                                                                 \
        streaming_->SetError(errc);                                                                                    \
        streaming_ = nullptr;                                                                                          \
    }                                                                                                                  \
    ReturnVoidErrcWithLongLog(msg, errc)

/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
#define ReturnVoidErrcEnumWithLongLogWithStreaming(msg, errcEnum)                                                      \
    ReturnVoidErrcWithLongLogWithStreaming(msg, UCMErrorDomainErrc::errcEnum)

// input errc var, or input UCMErrorDomainErrc::some_errc_enum_name
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
#define ReturnErrcWithLongLogWithStreamingWithCleanupAll(msg, rettype, errc)                                           \
    CleanupAfterExtraction(id);                                                                                        \
    if (streaming_ != nullptr) {                                                                                       \
        streaming_->SetError(errc);                                                                                    \
    }                                                                                                                  \
    ReturnErrcWithLongLog(msg, rettype, errc)
// input errc enum name without prefix UCMErrorDomainErrc, so autoly add UCMErrorDomainErrc:: into errc
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
#define ReturnErrcEnumWithLongLogWithStreamingWithCleanupAll(msg, rettype, errcEnum)                                   \
    ReturnErrcWithLongLogWithStreamingWithCleanupAll(msg, rettype, UCMErrorDomainErrc::errcEnum)

/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
#define ReturnVoidErrcWithLongLogWithStreamingWithCleanupAll(msg, errc)                                                \
    ReturnErrcWithLongLogWithStreamingWithCleanupAll(msg, void, errc)

/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
#define ReturnVoidErrcEnumWithLongLogWithStreamingWithCleanupAll(msg, errcEnum)                                        \
    ReturnErrcEnumWithLongLogWithStreamingWithCleanupAll(msg, void, errcEnum)

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @brief constructor
/// @param softwarePackageExtractor
/// @param softwarePackageParser
/// @param packagesData
SoftwarePackageManager::SoftwarePackageManager(
    std::unique_ptr< SoftwarePackageExtractor > softwarePackageExtractor,
    std::unique_ptr< SoftwarePackageParser > softwarePackageParser,
    std::unique_ptr< SynchronizedStorage< TransferIdType, StreamableSoftwarePackage > > packagesData) noexcept
    : extractor_{std::move(softwarePackageExtractor)}
    , parser_{std::move(softwarePackageParser)}
    , packagesData_{std::move(packagesData)}
{
}

/// @brief Delete a transferred Software Package.
///
/// @param id Transfer ID of Software Package.
///
/// @return The result of operation, which specifies if the
/// request was successful.
FutureVoid SoftwarePackageManager::DeleteTransfer(TransferIdType const& id) noexcept
{
    LOGD << "call...";
    AraString const func{__func__};

    ASYNC_CALL_VOID_BEGIN_WITH_FUNC_ID(func, id) { return _deleteTransfer(id); }
    ASYNC_CALL_END();
}

/// @brief Block-wise transfer of a Software Package.
///
/// @param id Transfer ID of the currently running request.
/// @param data Data block of the Software Package.
/// @param blockCounter Block counter of the current block.
///
/// @return The result of transferring current data block, which specifies if the
/// sw package has been successfully transferred.
FutureVoid SoftwarePackageManager::TransferData(TransferIdType const& id,
                                                ByteVectorType const& data,
                                                std::uint64_t const& blockCounter) noexcept
{
    LOGD << "call...";
    AraString const func{__func__};

    return ASyncCallVoid([&, func, id, blockCounter]() {  // data, .
        // Block size limit
        if (data.size() > kUCM_DATA_BLOCK_SIZE) {
            ReturnVoidErrcEnumWithLongLog(func.c_str() << " provided block size:" << data.size()
                                                       << " exceeds kUCM_DATA_BLOCK_SIZE:" << kUCM_DATA_BLOCK_SIZE,
                                          kIncorrectBlockSize);
        }

        std::shared_ptr< StreamableSoftwarePackage > pkgItem{packagesData_->GetItem(id)};
        if (nullptr == pkgItem) {
            ReturnVoidErrcEnumWithLongLog(
                func.c_str() << " can't find id " << strtype::TransferIdTypeToHexStr(id).c_str(), kInvalidTransferId);
        }

        UCMErrorDomainErrc errc;
        bool success{false};

        TransferDataReturnType const ret{pkgItem->TransferData(data, blockCounter)};
        ///std::unique_lock<std::mutex> lock(mutexForProcessingFromStream_);.//////mytodo//
        switch (ret) {
            case TransferDataReturnType::kSuccess: {
                success = true;
                break;
            }
            case TransferDataReturnType::kIncorrectBlock: {
                errc = UCMErrorDomainErrc::kIncorrectBlock;
                break;
            }
            case TransferDataReturnType::kIncorrectSize: {
                errc = UCMErrorDomainErrc::kIncorrectSize;
                break;
            }
            case TransferDataReturnType::kInsufficientMemory: {
                errc = UCMErrorDomainErrc::kInsufficientMemory;
                break;
            }
            case TransferDataReturnType::kInvalidTransferId: {
                errc = UCMErrorDomainErrc::kInvalidTransferId;
                break;
            }
            case TransferDataReturnType::kOperationNotPermitted:
            default: {
                errc = UCMErrorDomainErrc::kOperationNotPermitted;
                break;
            }
        }

        if (!success) {
            if (UCMErrorDomainErrc::kInvalidTransferId == errc) {
                ReturnVoidErrcWithLongLog(func.c_str(), errc);
            }
            ReturnVoidErrcWithLongLogWithStreaming(func.c_str(), UCMErrorDomainErrc{errc});
        }

        return AraResultVoid{};

    });
}

/// @brief Finish the transfer of a Software Package.
///
/// @param id Transfer ID of the currently running request.
///
/// @return The result of finishing this sw package transfer, which specifies if the
/// sw package has been successfully transferred.
FutureVoid SoftwarePackageManager::TransferExit(TransferIdType const& id, FileSystemSWCLManager* swclManager) noexcept
{
    LOGD << "call...";
    AraString const func{__func__};

    ASYNC_CALL_VOID_BEGIN_WITH_FUNC_ID(func, id)
    {
        AraResultVoid ret;
        UCMErrorDomainErrc errc;
        bool success{false};

        std::shared_ptr< StreamableSoftwarePackage > pkgItem{packagesData_->GetItem(id)};
        if (nullptr == pkgItem) {
            LOGE << "can't find id:" << strtype::TransferIdTypeToHexStr(id).c_str();
            errc = UCMErrorDomainErrc::kInvalidTransferId;
            return AraResultVoid::FromError(errc);
        }

        // TBD: This complete switch should disappear as soon as a (legacy)
        // TransferExitReturnType is removed.
        switch (pkgItem->TransferExit()) {
            case TransferExitReturnType::kSuccess: {
                pkgItem->SetState(SwPackageStateType::kTransferred);
                ret = _transferExitSuccessTask(id, swclManager);
                if (!ret.HasValue()) {
                    LOGE << func.c_str() << " _transferExitSuccessTask failed";
                    errc = GetAraResultErrc(ret);
                } else {
                    success = true;
                }
                break;
            }
            case TransferExitReturnType::kInsufficientData: {
                errc = UCMErrorDomainErrc::kInsufficientData;
                break;
            }
            case TransferExitReturnType::kPackageInconsistent: {
                errc = UCMErrorDomainErrc::kPackageInconsistent;
                break;
            }
            case TransferExitReturnType::kInvalidTransferId: {
                errc = UCMErrorDomainErrc::kInvalidTransferId;
                break;
            }
            case TransferExitReturnType::kOperationNotPermitted:
            default: {
                errc = UCMErrorDomainErrc::kOperationNotPermitted;
                break;
            }
        }

        if (!success) {
            if (UCMErrorDomainErrc::kInvalidTransferId == errc) {
                ReturnVoidErrcWithLongLog(func.c_str(), errc);
            }
            ReturnVoidErrcWithLongLogWithStreaming(func.c_str(), UCMErrorDomainErrc{errc});
        }

        return ret;
    }
    ASYNC_CALL_END();
}

/// @brief Start the transfer of a Software Package.
/// Transfer Id for subsequent calls to TransferData will be generated
/// and returned as a part of TransferStartOutput.
///
/// @param size Size (in bytes) of the Software Package to be transferred.
///
/// @return The struct which contains the result of the transfer start operation
/// and transfer id generated for this operation.
FutureTransferStart SoftwarePackageManager::TransferStart(std::uint64_t const size) noexcept
{
    LOGD << "call...";
    AraString const func{__func__};

    return ASyncCall< TransferStartOutput >([this, func, size]() noexcept {
        if (nullptr == packagesData_) {
            ReturnErrcEnumWithLongLog(
                func.c_str() << " packagesData_ not init", TransferStartOutput,
                kInsufficientMemory);  ///////////////Return an appropriate error code////////mytodo////
        }

        SoftwarePackageDataFactory factory{GetPathSettings().swpkgsDir};
        SerialIDGenerator< TransferIdType > generator;
        AraOptional< std::pair< TransferIdType, std::shared_ptr< StreamableSoftwarePackage > > > item{
            packagesData_->CreateAndStoreItem(factory, generator)};
        if (!item.has_value()) {
            ReturnErrcEnumWithLongLog(func.c_str() << " failed to create package item", TransferStartOutput,
                                      kInsufficientMemory);
        }

        std::pair< TransferIdType, std::shared_ptr< StreamableSoftwarePackage > > package{*item};
        TransferStartReturnType const result{package.second->TransferStart(size)};
        if (result.transferStartSuccess != TransferStartSuccessType::kSuccess) {
            packagesData_->DeleteItem(package.first);
            ReturnErrcEnumWithLongLog(func.c_str() << " failed to TransferStart", TransferStartOutput,
                                      kInsufficientMemory);
        }

        package.second->SetState(SwPackageStateType::kTransferring);
        return AraResult< TransferStartOutput >({result.transferId, kUCM_DATA_BLOCK_SIZE});
    });
}

/// @brief Get the progress of the currently processed Software Package.
///
/// @param id Transfer ID of Software Package.
///
/// @return The progress of the current package processing (0x00 - 0x64).
///
FutureGetSwProcessProgress SoftwarePackageManager::GetSwProcessProgress(TransferIdType const& id) const noexcept
{
    LOGD << "call...";
    AraString const func{__func__};

    return SyncCall< GetSwProcessProgressOutput >([this, &id, &func]() {
        std::shared_ptr< StreamableSoftwarePackage > package{packagesData_->GetItem(id)};
        if (nullptr == package) {
            LOGE << func.c_str() << " GetItem failed";
            return AraResult< GetSwProcessProgressOutput >::FromError(UCMErrorDomainErrc::kInvalidTransferId);
        }

        return AraResult< GetSwProcessProgressOutput >({package->GetProcessProgressValue()});
    });
}

/// @brief Retrieve a Software Package list.
///
/// @return List of all Software Packages that have been successfully
/// transferred and are ready to be installed.
FutureGetSwPackages SoftwarePackageManager::GetSwPackages() const noexcept
{
    LOGD << "call...";

    GetSwPackagesOutput out;

    packagesData_->IterateItems(
        [&out](std::pair< TransferIdType, std::shared_ptr< StreamableSoftwarePackage > > const& pair) {
            SwPackageInfoType info;

            info.TransferID                = pair.first;
            info.State                     = pair.second->GetState();
            info.ConsecutiveBytesReceived  = pair.second->GetReceivedBytes();
            info.ConsecutiveBlocksReceived = pair.second->GetReceivedBlocks();

            // Get software package / software set name and version
            pair.second->GetSwPackageInfo(info.Name, info.Version);

            out.Packages.emplace_back(info);
        });

    return SyncCall< GetSwPackagesOutput >([&out]() { return AraResult< GetSwPackagesOutput >(out); });
}

/// @brief Recover Packages Data
/// @throws no
void SoftwarePackageManager::RecoverPackagesData() const noexcept
{
    LOGD << "call...";

    AraString const& swpkgsDir{GetPathSettings().swpkgsDir};
    LOGD << "swpkgsDir:" << swpkgsDir.c_str();

    // Find all softwarePackage files under swpksDir
    AraVectorString const swpkgPaths{tinyfsys::GetFiles(swpkgsDir)};
    if (swpkgPaths.empty()) {
        LOGD << "No softwarePackage found";
        return;
    }

    // softwarePackage filename
    AraList< TransferIdType > swpkgIDs;
    AraString const extension{".zip"};
    for (AraString const& swpkgPath : swpkgPaths) {  // Find files with 'zip' in the name
        if (swpkgPath.find(extension) == AraString::npos) {
            //////break;/////////////////////mytodo666//
            continue;
        }

        // Filename length
        std::size_t const idLen{swpkgPath.size() - swpkgPath.find_last_of("/") - 1U - extension.size()};
        // Filename
        AraString const swpkgID{swpkgPath.substr(swpkgPath.find_last_of("/") + 1U, idLen)};
        LOGD << "got swpkgID:" << swpkgID.c_str();

        std::size_t const step{2U};
        if (swpkgID.size() == (sizeof(TransferIdType) * step)) {
            swpkgIDs.push_back(strtype::TransferIdTypeFromHexStr(swpkgID));
        }
    }

    // Add the corresponding softwarePackage information to packagesData_
    SoftwarePackageDataFactory factory{swpkgsDir};
    for (TransferIdType const& swpkgID : swpkgIDs) {
        LOGD << "CreateAndStoreItemWithKey for swpkgID:" << strtype::TransferIdTypeToHexStr(swpkgID).c_str();

        AraOptional< std::pair< TransferIdType, std::shared_ptr< StreamableSoftwarePackage > > > item{
            packagesData_->CreateAndStoreItemWithKey(factory, swpkgID)};
        if (!item.has_value()) {
            LOGE << "failed to create package data";
        }

        std::pair< TransferIdType, std::shared_ptr< StreamableSoftwarePackage > > package{*item};
        LOGD << "swpkg state:" << strtype::SwPackageStateTypeToStr(package.second->GetState()).c_str();
    }
}

/// @brief Recover Packages Data
/// @param id Transfer ID of Software Package
/// @throws no
void SoftwarePackageManager::CleanupAfterExtraction(TransferIdType const& id) const noexcept
{
    extractor_->Cleanup();
    std::ignore = _deleteTransfer(id);
}

/// @brief Recover Packages Data
/// @param streaming Software Package streaming
/// @param streamingId Software Package streaming id
/// @throws no
void SoftwarePackageManager::SetStreaming(std::unique_ptr< AraPromiseVoid > streaming,
                                          TransferIdType streamingId) noexcept
{
    streaming_   = std::move(streaming);
    streamingId_ = streamingId;
}

/// @brief delete transfer
/// @param id the id of the transfer
/// @return result
AraResultVoid SoftwarePackageManager::_deleteTransfer(TransferIdType const& id) const noexcept
{
    LOGD << "call...";

    AraString const idStr{strtype::TransferIdTypeToHexStr(id)};

    std::shared_ptr< StreamableSoftwarePackage > pkgItem{packagesData_->GetItem(id)};
    if (nullptr == pkgItem) {
        ReturnVoidErrcEnumWithLongLog("failed to GetItem for id " << idStr.c_str(), kInvalidTransferId);
    }

    // Legality check
    SwPackageStateType const pkgState{pkgItem->GetState()};
    if ((pkgState == SwPackageStateType::kProcessing) || (pkgState == SwPackageStateType::kProcessingStream)) {
        ReturnVoidErrcEnumWithLongLog("in processing, can't delete id " << idStr.c_str(), kOperationNotPermitted);
    }

    if (DeleteTransferReturnType::kSuccess != pkgItem->DeleteTransfer()) {
        ReturnVoidErrcEnumWithLongLog("failed to delete id " << idStr.c_str(), kGeneralReject);
    }

    packagesData_->DeleteItem(id);

    ///if (streaming_ && (streamingId_ == id)) {//mytodo//////
    ///    streaming_->SetError(UCMErrorDomainErrc::kOperationNotPermitted);.
    ///    streaming_ = nullptr;
    ///}

    return {};
}

/// @brief Task which will be executed on another thread if the transfer exit
/// is successful
/// @param id the id of the transfer
/// the TransferExit function
/// @return result
AraResultVoid SoftwarePackageManager::_transferExitSuccessTask(TransferIdType const& id,
                                                               FileSystemSWCLManager* const swclManager) const noexcept
{
    std::ignore = swclManager;
    LOGD << "call...";

    // According to the id, get the software package path and the directory to be decompressed
    std::shared_ptr< StreamableSoftwarePackage > pkgItem{packagesData_->GetItem(id)};

    // The temporary to which the reference is bound or the temporary that is the complete object of a subobject to which the reference is bound persists for the lifetime of the reference
    AraString const& packageFileName{pkgItem->GetPackageFilename()};
    AraString const& extractionDir{Path::GetExtractionTargetDir(packageFileName)};

    // Decompress first
    AraResultVoid const extractRet{_extractSwPackage(packageFileName, extractionDir, id)};
    if (!extractRet.HasValue()) {
        return extractRet;
    }

    // Then parse
    AraResult< std::unique_ptr< SoftwarePackage > > parseRet{_parseSwPackage(extractionDir, id)};
    if (!parseRet.HasValue()) {
        return AraResultVoid::FromError(parseRet.Error());
    }
    std::unique_ptr< SoftwarePackage > const swpkg{std::move(std::move(parseRet).Value())};

    SoftwareCluster const& swcl{swpkg->GetSoftwareCluster()};
    SoftwareClusterManifest const& swclMnf{swcl.GetSwclManifest()};
    AraString const swclVer{swclMnf.version.ToString()};

    // Check compatibility
    SoftwarePackageManifest const& swpkgMnf{swpkg->GetManifest()};
    Version const fixedUCMVer{kFixedUCMVersion};
    if (swpkgMnf.minUCMSupportedVersion > fixedUCMVer) {
        ReturnVoidErrcEnumWithLongLogWithStreamingWithCleanupAll(
            "The swpkg version is not compatible with the current UCM version "
                << kFixedUCMVersion
                << ", minUCMSupportedVersion:" << swpkgMnf.minUCMSupportedVersion.ToString().c_str(),
            kIncompatiblePackageVersion);
    }

    // Check if the version is older
    const bool isOldVersion{FileSystemSWCLManager::IsSwclOldVersion(swcl)};
    if ((swpkgMnf.actionType != ActionType::kRemove) && isOldVersion) {
        // Save this attempt record
        GetHistoryType oldRecord;
        oldRecord.Name       = swclMnf.shortName;
        oldRecord.Version    = swclVer;
        oldRecord.Action     = swpkgMnf.actionType;
        oldRecord.Resolution = ResultType::kFailed;

        AraList< GetHistoryType > const actionsInfo{oldRecord};
        std::ignore = UcmDatabase(GetPathSettings().historyPath).SaveLastSnapshot(actionsInfo);

        ReturnVoidErrcEnumWithLongLogWithStreamingWithCleanupAll("too old swcl", kOldVersion);
    }

    // No other errors
    if (streaming_) {
        streaming_->set_value();
        LOGD << "success";
    }

    // Save software package / software set name and version
    pkgItem->SetSwPackageInfo(swclMnf.shortName, swclVer);
    // Set software package status to kTransferred
    pkgItem->SetState(SwPackageStateType::kTransferred);

    return {};
}

/// @brief Function which decompresses a software cluster package archive
/// @param packageFilename
/// @param extractionDir
/// @param id the id of the transfer
/// @return result
AraResultVoid SoftwarePackageManager::_extractSwPackage(AraString const& packageFilename,
                                                        AraString const& extractionDir,
                                                        TransferIdType const& id) const noexcept
{
    LOGD << "call...";

    std::ignore = tinyfsys::RemoveDirectory(extractionDir);

    if (!extractor_->Extract(packageFilename, extractionDir)) {
        // failed decompression cleanup is handled in the library, but we need to
        // cleanup the internal data
        ReturnVoidErrcEnumWithLongLogWithStreamingWithCleanupAll("extract failed", kPackageInconsistent);
    }

    return {};
}

/// @brief Function which parses a software cluster package located at
/// extractionDirectory
/// @param extractionDir Where the decompressed software package is
/// located
/// @param id the id of the transfer
/// @return result
AraResult< std::unique_ptr< SoftwarePackage > > SoftwarePackageManager::_parseSwPackage(
    AraString const& extractionDir, TransferIdType const& id) const noexcept
{
    LOGD << "call...";

    std::unique_ptr< SoftwarePackage > swpkg{nullptr};
    AraResultVoid retAuthen;

    swpkg = parser_->Parse(extractionDir.c_str());
    if (swpkg == nullptr) {
        ReturnErrcEnumWithLongLogWithStreamingWithCleanupAll(
            extractionDir.c_str() << " parsed failed", std::unique_ptr< SoftwarePackage >, kInvalidPackageManifest);
    }

    retAuthen = swpkg->AuthenAndCheckConsistency();
    if (!retAuthen.HasValue()) {
        ReturnErrcWithLongLogWithStreamingWithCleanupAll(
            extractionDir.c_str() << " authen failed", std::unique_ptr< SoftwarePackage >, GetAraResultErrc(retAuthen));
    }

    return AraResult< std::unique_ptr< SoftwarePackage > >(std::move(swpkg));
}

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara
