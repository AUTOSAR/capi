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
/// @file       package_storage.cpp
/// @brief      PackageStorage implementation
/// @details
/// @date       2023-10-30
/// @author     hanzhibo
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/UCM Master/Fsm
/// @interface_level=unit
/// @trace_id_sr=SR_UCM_00035
/// @unit_name=PackageStorage
/// @unit_description=PackageStorage implementation
/// @endcode
///
/// ================================================================

#include "fsm/package_transfer/package_storage.h"

#include <ara/ucm/internal/transfer/helper.h>
#include <ara/ucm/pkgmgr/error_domain_ucmerrordomain.h>

#include "utils/alias.h"
#include "utils/ucmm_log.h"

namespace ara {
namespace ucm {
namespace vpkgmgr {

/// @brief GetErrCode
/// @param code
/// @return UCMErrorDomainErrc
/// @throw no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_Master_00480
/// @trace_id_dd=DD_UCM_Master_00938
/// @needwork = dd
/// @endcode
static pkgmgr::UCMErrorDomainErrc GetErrCode(pkgmgr::TransferDataReturnType const code) noexcept
{
    pkgmgr::UCMErrorDomainErrc errCode;
    switch (code) {
        case pkgmgr::TransferDataReturnType::kIncorrectBlock: {
            errCode = pkgmgr::UCMErrorDomainErrc::kIncorrectBlock;
            break;
        }
        case pkgmgr::TransferDataReturnType::kIncorrectSize: {
            errCode = pkgmgr::UCMErrorDomainErrc::kIncorrectSize;
            break;
        }
        case pkgmgr::TransferDataReturnType::kInsufficientMemory: {
            errCode = pkgmgr::UCMErrorDomainErrc::kInsufficientMemory;
            break;
        }
        case pkgmgr::TransferDataReturnType::kInvalidTransferId: {
            errCode = pkgmgr::UCMErrorDomainErrc::kInvalidTransferId;
            break;
        }
        case pkgmgr::TransferDataReturnType::kOperationNotPermitted: {
            errCode = pkgmgr::UCMErrorDomainErrc::kOperationNotPermitted;
            break;
        }
        default: {
            /// TODO(yunfei) API does not have a matching return error value
            ///errCode = pkgmgr::UCMErrorDomainErrc::kGeneralMemoryError;
            errCode = pkgmgr::UCMErrorDomainErrc::kOperationNotPermitted;
            LOG_ERROR << "must not proceed to this step, please check ucmlib";
            break;
        }
    }
    return errCode;
}
/// @brief GetErrCode
/// @param code
/// @return UCMErrorDomainErrc
/// @throw no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_Master_00480
/// @trace_id_dd=DD_UCM_Master_00939
/// @needwork = dd
/// @endcode
static pkgmgr::UCMErrorDomainErrc GetErrCode(pkgmgr::TransferExitReturnType const code) noexcept
{
    pkgmgr::UCMErrorDomainErrc errCode;
    switch (code) {
        case pkgmgr::TransferExitReturnType::kInsufficientData: {
            errCode = pkgmgr::UCMErrorDomainErrc::kInsufficientData;
            break;
        }
        case pkgmgr::TransferExitReturnType::kPackageInconsistent: {
            errCode = pkgmgr::UCMErrorDomainErrc::kPackageInconsistent;
            break;
        }
        case pkgmgr::TransferExitReturnType::kInvalidTransferId: {
            errCode = pkgmgr::UCMErrorDomainErrc::kInvalidTransferId;
            break;
        }
        case pkgmgr::TransferExitReturnType::kOperationNotPermitted: {
            errCode = pkgmgr::UCMErrorDomainErrc::kOperationNotPermitted;
            break;
        }
        default: {
            /// TODO(yunfei) API does not have a matching return error value
            ///errCode = pkgmgr::UCMErrorDomainErrc::kGeneralMemoryError;
            errCode = pkgmgr::UCMErrorDomainErrc::kOperationNotPermitted;
            LOG_ERROR << "must not proceed to this step, please check ucmlib";
            break;
        }
    }
    return errCode;
}

/// static pkgmgr::UCMErrorDomainErrc GetErrCode(pkgmgr::DeleteTransferReturnType const code) {.
///     pkgmgr::UCMErrorDomainErrc errCode;.
///     switch (code) {
///         case pkgmgr::DeleteTransferReturnType::kGeneralReject: {
///             errCode = pkgmgr::UCMErrorDomainErrc::kGeneralReject;
///             break;
///         }
///         case pkgmgr::DeleteTransferReturnType::kGeneralMemoryError: {
///             errCode = pkgmgr::UCMErrorDomainErrc::kGeneralMemoryError;
///             break;
///         }
///         case pkgmgr::DeleteTransferReturnType::kTransferIdInvalid: {
///             errCode = pkgmgr::UCMErrorDomainErrc::kInvalidTransferId;
///             break;
///         }
///         case pkgmgr::DeleteTransferReturnType::kOperationNotPermitted: {
///             errCode = pkgmgr::UCMErrorDomainErrc::kOperationNotPermitted;
///             break;
///         }
///         default: {
///             /// TODO(yunfei) API does not have a matching return error value
///             // errCode = pkgmgr::UCMErrorDomainErrc::kGeneralMemoryError;
///             errCode = pkgmgr::UCMErrorDomainErrc::kOperationNotPermitted;
///             LOG_ERROR << "must not proceed to this step, please check ucmlib";
///             break;
///         }
///     }
///     return errCode;
/// }

/// @brief Create
/// @param vpkgDir
/// @return shared_ptr of PackageStorage
/// @throw
PackageStorage::Ptr PackageStorage::Create(ara::core::String const& vpkgDir)
{
    /// @brief SyncStorageType
    using SyncStorageType = SynchronizedStorage< pkgmgr::TransferIdType, pkgmgr::StreamableSoftwarePackage >;

    pkgmgr::Filesystem fs;

    /// @brief Make_Shared_Enabler
    struct Make_Shared_Enabler : public PackageStorage
    {
    };

    PackageStorage::Ptr self{std::make_shared< Make_Shared_Enabler >()};
    self->softwarePackageExtractor_ = std::make_unique< pkgmgr::LibPocoZipExtractor >(fs);
    self->softwarePackageFactory_   = std::make_unique< pkgmgr::SoftwarePackageDataFactory >(vpkgDir);
    self->packagesData_             = std::make_unique< SyncStorageType >();
    self->RetrievePackagesData(vpkgDir);
    return self;
}

/// @brief RetrievePackagesData
/// @param vpkgDir
/// @throw
void PackageStorage::RetrievePackagesData(ara::core::String const& vpkgDir) const
{
    LOG_DEBUG << "now scanning through directory Dir:" << vpkgDir.c_str();

    // Find all softwarePackage files under the swpksDir
    pkgmgr::AraVectorString const swpPaths{pkgmgr::Filesystem::GetFiles(vpkgDir)};
    if (swpPaths.empty()) {
        LOG_DEBUG << "No softwarePackage found in " << vpkgDir.c_str();
        return;
    }

    // softwarePackage file name
    AraList< pkgmgr::TransferIdType > swpIDs;
    ara::core::String const extension{".zip"};
    for (ara::core::String const& swpPath : swpPaths) {  // Find files with 'zip' in the name
        if (swpPath.find(extension) != ara::core::String::npos) {
            LOG_DEBUG << "got swpPath:" << swpPath.c_str();

            // File name length
            std::size_t const idLen{swpPath.size() - swpPath.find_last_of("/") - 1U - extension.size()};
            // File name
            ara::core::String const swpID{swpPath.substr(swpPath.find_last_of("/") + 1U, idLen)};
            swpIDs.push_back(pkgmgr::helper::HexStrToTransferId((swpID)));
            LOG_DEBUG << "got swpID:" << swpID.c_str();
        }
    }

    // Add corresponding softwarePackage information to packagesData_
    for (pkgmgr::TransferIdType const& swpID : swpIDs) {
        LOG_DEBUG << "try to  CreateAndStoreItemWithKey for swpID:" << pkgmgr::helper::ToHexString(swpID).c_str();

        ara::core::Optional< std::pair< pkgmgr::TransferIdType, std::shared_ptr< pkgmgr::StreamableSoftwarePackage > > >
            item{packagesData_->CreateAndStoreItemWithKey(*softwarePackageFactory_, swpID)};

        if (!item.has_value()) {
            LOG_ERROR << "failed to create package data.";
        }

        std::pair< pkgmgr::TransferIdType, std::shared_ptr< pkgmgr::StreamableSoftwarePackage > > package{*item};

        pkgmgr::SwPackageStateType const state{package.second->GetState()};
        LOG_DEBUG << "package state:" << static_cast< std::int32_t >(state);
    }
}
/// @brief TransferStart
/// @param size
/// @return result of TransferIdType
/// @throw
ara::core::Result< pkgmgr::TransferIdType > PackageStorage::TransferStart(uint64_t const size)
{
    LOG_INFO << "begin";
    ara::core::Optional< std::pair< pkgmgr::TransferIdType, std::shared_ptr< pkgmgr::StreamableSoftwarePackage > > >
        package{packagesData_->CreateAndStoreItem(*softwarePackageFactory_, generator_)};
    if (!package.has_value()) {
        LOG_ERROR << "CreateAndStoreItem error!";
        return ara::core::Result< pkgmgr::TransferIdType >::FromError(pkgmgr::UCMErrorDomainErrc::kInsufficientMemory);
    }

    pkgmgr::TransferIdType const packageId{package->first};

    std::shared_ptr< pkgmgr::StreamableSoftwarePackage > const packageStream{package->second};

    LOG_INFO << "streampackage state=" << static_cast< uint8_t >(packageStream->GetState());
    if (packageStream->GetState() != pkgmgr::SwPackageStateType::kTransferring) {
        LOG_ERROR << "PackageStorage::TransferStart, illegal state.";
        return ara::core::Result< pkgmgr::TransferIdType >::FromError(pkgmgr::UCMErrorDomainErrc::kGeneralReject);
    }

    pkgmgr::TransferStartReturnType const result{packageStream->TransferStart(size)};
    if (pkgmgr::TransferStartSuccessType::kSuccess != result.transferStartSuccess) {
        LOG_DEBUG << "DeleteItem packageId:" << pkgmgr::helper::ToHexString(packageId).c_str();
        packagesData_->DeleteItem(packageId);
        LOG_ERROR << "packageStream transferStart error!";
        return ara::core::Result< pkgmgr::TransferIdType >::FromError(pkgmgr::UCMErrorDomainErrc::kInsufficientMemory);
    }
    packageStream->SetState(pkgmgr::SwPackageStateType::kTransferring);

    LOG_INFO << "end";
    return ara::core::Result< pkgmgr::TransferIdType >::FromValue(packageId);
}

/// @brief TransferData
/// @param id
/// @param data
/// @param blockCounter
/// @return result
/// @throw
ara::core::Result< void > PackageStorage::TransferData(pkgmgr::TransferIdType const& id,
                                                       pkgmgr::ByteVectorType const& data,
                                                       std::uint64_t const& blockCounter) const
{
    std::shared_ptr< pkgmgr::StreamableSoftwarePackage > packageStream{packagesData_->GetItem(id)};
    if (nullptr == packageStream.get()) {
        LOG_ERROR << "transferId is invalid! transferId=" << pkgmgr::helper::ToHexString(id).c_str();
        return ara::core::Result< void >::FromError(pkgmgr::UCMErrorDomainErrc::kInvalidTransferId);
    }
    pkgmgr::TransferDataReturnType const result{packageStream->TransferData(data, blockCounter)};
    if (pkgmgr::TransferDataReturnType::kSuccess != result) {
        packagesData_->DeleteItem(id);
        std::ignore = packageStream->DeleteTransfer();
        LOG_ERROR << " TransferData faild! TransferDataReturnType=" << static_cast< uint8_t >(result);
        return ara::core::Result< void >::FromError(GetErrCode(result));
    }
    return ara::core::Result< void >::FromValue();
}

/// @brief GetPackageStatus
/// @param id
/// @return result of SwPackageStateType
/// @throw
ara::core::Result< pkgmgr::SwPackageStateType > PackageStorage::GetPackageStatus(pkgmgr::TransferIdType const& id) const
{
    std::shared_ptr< pkgmgr::StreamableSoftwarePackage > packageStream{packagesData_->GetItem(id)};
    if (nullptr == packageStream.get()) {
        return ara::core::Result< pkgmgr::SwPackageStateType >::FromError(
            pkgmgr::UCMErrorDomainErrc::kInvalidTransferId);
    }
    return ara::core::Result< pkgmgr::SwPackageStateType >::FromValue(packageStream->GetState());
}
/// @brief TransferExit
/// @param id
/// @return result
/// @throw
ara::core::Result< ara::core::String > PackageStorage::TransferExit(pkgmgr::TransferIdType const& id) const
{
    std::shared_ptr< pkgmgr::StreamableSoftwarePackage > packageStream{packagesData_->GetItem(id)};
    if (nullptr == packageStream.get()) {
        LOG_ERROR << "transferId is invalid!";
        return ara::core::Result< ara::core::String >::FromError(pkgmgr::UCMErrorDomainErrc::kInvalidTransferId);
    }

    pkgmgr::TransferExitReturnType const result{packageStream->TransferExit()};
    if (result != pkgmgr::TransferExitReturnType::kSuccess) {
        packagesData_->DeleteItem(id);
        std::ignore = packageStream->DeleteTransfer();
        LOG_ERROR << "TransferExit failed!";
        return ara::core::Result< ara::core::String >::FromError(GetErrCode(result));
    }

    packageStream->SetState(pkgmgr::SwPackageStateType::kTransferred);
    return ara::core::Result< ara::core::String >::FromValue(packageStream->GetPackageFilename());
}
/// @brief DeleteTransfer
/// @param id
/// @return result
/// @throw
ara::core::Result< void > PackageStorage::DeleteTransfer(pkgmgr::TransferIdType const& id) const
{
    std::shared_ptr< pkgmgr::StreamableSoftwarePackage > packageStream{packagesData_->GetItem(id)};
    if (nullptr == packageStream.get()) {
        LOG_ERROR << "transferId is invalid!";
        return ara::core::Result< void >::FromError(pkgmgr::UCMErrorDomainErrc::kInvalidTransferId);
    }

    pkgmgr::DeleteTransferReturnType const result{packageStream->DeleteTransfer()};
    if (result != pkgmgr::DeleteTransferReturnType::kSuccess) {
        LOG_ERROR << "DeleteTransfer transferId=" << pkgmgr::helper::ToHexString(id).c_str()
                  << " error=" << static_cast< int32_t >(result);
        ///return ara::core::Result<void>::FromError(GetErrCode(result));
    }

    packagesData_->DeleteItem(id);
    return ara::core::Result< void >::FromValue();
}
/// @brief CleanVehiclePackage
/// @param id
/// @return result
/// @throw
ara::core::Result< void > PackageStorage::CleanVehiclePackage(pkgmgr::TransferIdType const& id) const
{  // Currently not called
    softwarePackageExtractor_->Cleanup();
    std::shared_ptr< pkgmgr::StreamableSoftwarePackage > packageStream{packagesData_->GetItem(id)};
    if (nullptr == packageStream.get()) {
        LOG_ERROR << "transferId is invalid!";
        return ara::core::Result< void >::FromError(pkgmgr::UCMErrorDomainErrc::kInvalidTransferId);
    }
    std::ignore = packageStream->DeleteTransfer();
    packagesData_->DeleteItem(id);

    return ara::core::Result< void >::FromValue();
}
/// @brief GetPackagePath
/// @param id
/// @return result with Package Path
/// @throw
ara::core::Result< ara::core::String > PackageStorage::GetPackagePath(pkgmgr::TransferIdType const& id) const
{
    std::shared_ptr< pkgmgr::StreamableSoftwarePackage > packageStream{packagesData_->GetItem(id)};
    if (nullptr == packageStream.get()) {
        LOG_ERROR << "transferId is invalid!";
        return ara::core::Result< ara::core::String >::FromError(pkgmgr::UCMErrorDomainErrc::kInvalidTransferId);
    }
    ara::core::String const packagePath{packageStream->GetPackageFilename()};
    return ara::core::Result< ara::core::String >::FromValue(packagePath);
}

/// @brief Extract
/// @param archivePath
/// @param extractionDirectory
/// @return bool
/// @throw
bool PackageStorage::Extract(ara::core::String const& archivePath, ara::core::String const& extractionDirectory) const
{
    return softwarePackageExtractor_->Extract(archivePath, extractionDirectory);
}

}  // namespace vpkgmgr
}  // namespace ucm
}  // namespace ara