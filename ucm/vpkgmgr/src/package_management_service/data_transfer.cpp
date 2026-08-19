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
/// @file       data_transfer.cpp
/// @brief      transfer software package to UCM using PackageManagement service
/// @details
/// @date       2024-07-20
/// @author     hanzhibo
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/UCM Master/Package Management Service
/// @interface_level=unit
/// @trace_id_sr=SR_UCM_00035
/// @unit_name=DataTransfer
/// @unit_description=transfer software package to UCM using PackageManagement service
/// @endcode
///
/// ================================================================

#include "data_transfer.h"

#include <ara/core/future.h>
#include <ara/core/promise.h>
#include <ara/ucm/internal/transfer/helper.h>

#include "utils/types.h"

using ara::core::Future;
using ara::core::Promise;
using ara::ucm::pkgmgr::PackageManagement;
using ara::ucm::pkgmgr::TransferIdType;

namespace ara {
namespace ucm {
namespace vpkgmgr {

/// @brief GetSwPackagesOutput
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
using GetSwPackagesOutput = ara::ucm::pkgmgr::PackageManagement::GetSwPackagesOutput;

// helper methods
/// @brief CalculateBlockCount
/// @param dataAmountInBytes
/// @param blockSizeInBytes
/// @return BlockCount
/// @throws no
// static uint32_t CalculateBlockCount(uint32_t const dataAmountInBytes, uint32_t const blockSizeInBytes) noexcept;.    // Currently not used

/// @brief InitDataBlock
/// @param startIndex
/// @param blockSize
/// @param data
/// @param dataBlock
/// @throws no
// static void InitDataBlock(uint32_t const startIndex, uint32_t const blockSize, std::vector<uint8_t> const& data,.   // Currently not used
//                    std::vector<uint8_t>& dataBlock);.

/// @brief Initializes this object with a reference to the PackageManagement
/// service
///
/// @param packageManagementService The instance of the PackageManagement
/// service to be used
/// @throws no
template < class PackageManagementService >
inline DataTransfer< PackageManagementService >::DataTransfer(
    std::shared_ptr< PackageManagementService > const packageManagementService)
    : packageManagementService_{packageManagementService}
    , blockSize_{0U}
    , currentSession_{0U}
    , logger_{log::CreateLogger(std::move(ara::core::StringView("DT")),
                                std::move(ara::core::StringView("DataTransfer")),
                                log::LogLevel::kVerbose)}
{
}

/// @brief Initializes a new session
///
/// @param size The size of the data that should be transmitted
///
/// @return Boolean indicating whether the session was successfully initiated
/// @throws no
template < class PackageManagementService >
inline bool DataTransfer< PackageManagementService >::InitSession(uint64_t size)
{
    Future< PackageManagement::TransferStartOutput > transferStartFuture{
        std::move(packageManagementService_->TransferStart(size))};
    ara::core::Result< PackageManagement::TransferStartOutput > const transferStartResult{
        transferStartFuture.GetResult()};

    if (!transferStartResult.HasValue()) {
        logger_.LogError() << "Error while initializing transfer session."
                           << "Error message:" << transferStartResult.Error().Message().data();
        return false;
    }
    currentSession_ = transferStartResult.Value().id;
    logger_.LogInfo() << "InitSession was successful for session"
                      << pkgmgr::helper::ToHexString(currentSession_).c_str();

    blockSize_ = transferStartResult.Value().blockSize;
    logger_.LogInfo() << "InitSession get blockSize_" << blockSize_;
    return true;
}

/// @brief Transfers data to UCM sending the provided package
/// to UCM, possibly by using multiple smaller data blocks accoring to the
/// configured block size.
///
/// @param packagePath The file that should be sent to UCM
///
/// @return Boolean indicating whether the data was successfully sent
/// @throws no
template < class PackageManagementService >
inline bool DataTransfer< PackageManagementService >::Transfer(ara::core::String const& packagePath)
{
    // File size
    uint64_t fileSize{0U};

    // Open file
    std::ifstream file{packagePath.c_str(), std::ifstream::binary};

    // Read a data block of size blockSize_ and send it
    std::vector< uint8_t > dataBlock{};
    dataBlock.reserve(static_cast< size_t >(blockSize_));
    dataBlock.resize(static_cast< size_t >(blockSize_));
    char8_t* const buf{static_cast< char8_t* >(static_cast< void* >(dataBlock.data()))};
    uint64_t blocksSent{0U};

    while (true) {
        if (!file.good()) {
            break;
        }

        // Read data block
        std::ignore = file.read(buf, static_cast< int64_t >(blockSize_));

        // // Test installation from stream
        ///std::this_thread::sleep_for(std::chrono::milliseconds(2000));.

        // Size read
        uint64_t const readSize{static_cast< uint64_t >(file.gcount())};
        dataBlock.resize(static_cast< size_t >(
            readSize));  // If resize actually happens, it must be at the end of the file, at which point reading will stop, so dataBlock will not be used again
        if (readSize > 0U) {
            Future< void > transferDataFuture{
                std::move(packageManagementService_->TransferData(currentSession_, dataBlock, blocksSent + 1U))};

            ara::core::Result< void > const transferDataResult{transferDataFuture.GetResult()};
            if (!transferDataResult.HasValue()) {
                logger_.LogError() << "Error while sending data block number" << blocksSent << "in session"
                                   << pkgmgr::helper::ToHexString(currentSession_).c_str()
                                   << ". Error:" << transferDataResult.Error().Message().data();
                return false;
            }
            ++blocksSent;
            fileSize += readSize;
        }
    }

    logger_.LogInfo() << "Successful transfer of data of fileSize" << fileSize << "with block count" << blocksSent
                      << "in session" << pkgmgr::helper::ToHexString(currentSession_).c_str();
    return true;
}

/// @brief Exits the current session
///
/// @return Boolean indicating whether this session was successfully exited
/// @throws no
template < class PackageManagementService >
inline ara::core::Result< void > DataTransfer< PackageManagementService >::ExitSession()
{
    ara::core::Result< void > const transferExitResult{
        std::move(packageManagementService_->TransferExit(currentSession_).GetResult())};

    if (!transferExitResult.HasValue()) {
        logger_.LogError() << "Error while exiting session" << pkgmgr::helper::ToHexString(currentSession_).c_str()
                           << ". Error:" << transferExitResult.Error().Message().data();
    }
    logger_.LogDebug() << "Session exited successfully for session"
                       << pkgmgr::helper::ToHexString(currentSession_).c_str();
    return transferExitResult;
}

/// @brief Delete SoftwarePackage
///
/// @param transferId Id of the package to be deleted
///
/// @returns Enum indicating whether deleting package was sucessful
/// @throws no
template < class PackageManagementService >
inline bool DataTransfer< PackageManagementService >::DeleteSwPackage(
    ara::ucm::pkgmgr::TransferIdType const& transferId)
{
    logger_.LogDebug() << "DataTransfer<PackageManagementService>::DeleteSwPackage, begin with transferId"
                       << pkgmgr::helper::ToHexString(transferId).c_str();

    Future< void > deleteTransferFuture{std::move(packageManagementService_->DeleteTransfer(transferId))};
    ara::core::Result< void > const deleteTransferResult{deleteTransferFuture.GetResult()};

    if (!deleteTransferResult.HasValue()) {
        logger_.LogError()
            << "DataTransfer<PackageManagementService>::DeleteSwPackage, Error during deletion of transfer"
            << pkgmgr::helper::ToHexString(transferId).c_str()
            << ". Error messagge:" << deleteTransferResult.Error().Message().data();
        return false;
    }
    logger_.LogDebug()
        << "DataTransfer<PackageManagementService>::DeleteSwPackage, Deleted package successfully for TransferID"
        << pkgmgr::helper::ToHexString(transferId).c_str();
    return true;
}

/// @brief Get SoftwarePackages
///
/// @returns List of all Software Packages that have successfully transferd
/// @throws no
template < class PackageManagementService >
inline GetSwPackagesOutput DataTransfer< PackageManagementService >::GetSwPackages()
{
    Future< PackageManagement::GetSwPackagesOutput > getSwPackagesFuture{
        std::move(packageManagementService_->GetSwPackages())};

    PackageManagement::GetSwPackagesOutput getSwPackagesOutput{getSwPackagesFuture.get()};

    logger_.LogInfo() << "Get software packages successfully."
                      << "The number of packages:" << getSwPackagesOutput.Packages.size();
    return getSwPackagesOutput;
}

/// @brief Returns the session id of the current session.
///
/// @return The id of the current session
/// @throws no
template < class PackageManagementService >
inline TransferIdType DataTransfer< PackageManagementService >::GetTransferId() noexcept
{
    return currentSession_;
}

/// template <class PackageManagementService>
/// void DataTransfer<PackageManagementService>::SetBlockSize(uint32_t size) {
///     blockSize_ = size;
/// }

/// @brief CalculateBlockCount
/// @param dataAmountInBytes
/// @param blockSizeInBytes
/// @return BlockCount
/// @throws no
// static uint32_t CalculateBlockCount(uint32_t const dataAmountInBytes, uint32_t const blockSizeInBytes) noexcept {.   // Currently not used
//     uint32_t blockCount{dataAmountInBytes / blockSizeInBytes};.
//     if (dataAmountInBytes % blockSizeInBytes != 0U) {.
//         blockCount++;.
//     }.
//     return blockCount;.
// }.

/// @brief InitDataBlock
/// @param startIndex
/// @param blockSize
/// @param data
/// @param dataBlock
/// @throws no
// static void InitDataBlock(uint32_t const startIndex, uint32_t const blockSize, std::vector<uint8_t> const& data,.   // Currently not used
//     std::vector<uint8_t>& dataBlock) {.
//     for (uint32_t i{startIndex}; i < startIndex + blockSize; i++) {.
//         if (i < data.size()) {.
//             dataBlock.push_back(data[i]);.
//         } else {.
//             break;.
//         }.
//     }.
// }.

template class DataTransfer< ara::ucm::pkgmgr::proxy::PackageManagementProxy >;

}  // namespace vpkgmgr
}  // namespace ucm
}  // namespace ara
