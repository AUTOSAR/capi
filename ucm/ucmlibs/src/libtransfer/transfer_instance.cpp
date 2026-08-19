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
/// @file       transfer_instance.cpp
/// @brief      The TransferInstance class definition which composes a file from vectors of bytes according to initially passed size.
/// @details
/// @date       2022-06-13
/// @author     cuiyinli
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/UCM/UCMLib
/// @interface_level=module
/// @trace_id_sr=SR_UCM_00025
/// @unit_name=TransferInstance
/// @unit_description=The TransferInstance class definition which composes a file from vectors of bytes according to initially passed size.
/// @endcode
///
/// ================================================================

#include "ara/ucm/internal/transfer/transfer_instance.h"

#include <fcntl.h>

#include "ara/ucm/internal/extraction/alias.h"
#include "ara/ucm/internal/extraction/log.h"
#include "ara/ucm/internal/extraction/tinyfs.h"
#include "ara/ucm/internal/extraction/tinyfsys.h"
#include "ara/ucm/internal/transfer/helper.h"
#include "ara/ucm/internal/transfer/receive_types.h"

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
constexpr std::size_t kStatus0U{0U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
constexpr std::size_t kStatus1U{1U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
constexpr std::size_t kStatus2U{2U};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
constexpr std::size_t kStatus3U{3U};

/// @brief constructor
/// @param id
/// @param path
/// @param storage
/// @throws no
TransferInstance::TransferInstance(TransferIdType const id,
                                   AraString const& path,
                                   std::unique_ptr< TransferStatusStorage > storage)
    : Streamable{}
    , transferId_{id}
    , receivedBytes_{0U}
    , transferDirectory_{path}
    , expectedBytes_{0U}
    , state_{SwPackageStateType::kTransferring}
    , expectedBlock_{1U}
    , fd_{}
    , key_{"transfer_status_" + helper::ToHexString(id)}
    , swPackageInfoKey_{"sw_package_info_" + helper::ToHexString(id)}
    , swName_{}
    , version_{}
    , statusStorage_{std::move(storage)}
{
    LOGD << "call... transferId_:" << helper::ToHexString(id).c_str() << " transferDirectory_:" << path.c_str();

    // Read persistenceFile and resume transferring status

    AraOptional< TransferStatusStorage::value_type > const statusOptional{statusStorage_->GetStatus(key_)};
    if (statusOptional.has_value()) {
        TransferStatusStorage::value_type const& status{*statusOptional};
        receivedBytes_ = status[kStatus0U];
        expectedBytes_ = status[kStatus1U];
        state_         = static_cast< SwPackageStateType >(status[kStatus2U]);
        expectedBlock_ = status[kStatus3U];
    }

    // Get software package information
    AraOptional< TransferStatusStorage::SwPackageInfoValueType > const swpkgOptional{
        statusStorage_->GetSwPackageInfo(swPackageInfoKey_)};
    if (swpkgOptional) {
        TransferStatusStorage::SwPackageInfoValueType const& swpkg{*swpkgOptional};
        swName_  = swpkg.swName;
        version_ = swpkg.version;
    }
}

/// @brief TransferStart
/// @param size
/// @return TransferStartReturnType
/// @throws no
TransferStartReturnType TransferInstance::TransferStart(std::uint64_t size)
{
    AraString const filename{GetPackageFilename()};
    LOGD << "beign with filename:" << filename.c_str() << " size:" << size;

    if (!tinyfsys::DoesDirectoryExist(transferDirectory_)) {
        std::int32_t const mode{0755};
        std::ignore = tinyfs::CreateDirectory(transferDirectory_, mode, false);
        LOGD << "Transfer directory " << transferDirectory_.c_str() << " is created";
    }

    // Check if there is enough space
    AraResult< uint64_t > const retSpace{tinyfsys::GetFreeDiskSpace(transferDirectory_)};
    if (!retSpace.HasValue()) {
        LOGE << "GetFreeDiskSpace errc:" << retSpace.Error().Message().data();
        return {transferId_, TransferStartSuccessType::kInsufficientMemory};
    }
    uint64_t const available{retSpace.Value()};
    if (size > available) {
        LOGE << "file size > available disk space " << available;
        return {transferId_, TransferStartSuccessType::kInsufficientMemory};
    }

    // Truncate if the file already exists
    fd_ = open(filename.c_str(), static_cast< int >(O_RDWR | O_CREAT | O_TRUNC | O_CLOEXEC),
               static_cast< mode_t >(S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH));

    std::int32_t const ret{posix_fallocate(fd_, 0, static_cast< off_t >(size))};
    if (0 != ret) {
        LOGE << "posix_fallocate returned with error:"
             << std::strerror(ret);  // NOLINT: [strerror]function is not thread safe
    }
    if ((EBADF == ret) || (EFBIG == ret) || (EINTR == ret) || (EINVAL == ret) || (ENODEV == ret) || (ESPIPE == ret)
        || (ENOSPC == ret)) {
        // no return value for these errors
        // error: no enough space
        std::ignore = unlink(filename.c_str());  // delete file: though ENOSPC is returned, the file is created
        std::ignore = close(fd_);
        LOGE << "posix_fallocate returned with fatal:"
             << std::strerror(ret);  // NOLINT: [strerror]function is not thread safe
        return {transferId_, TransferStartSuccessType::kInsufficientMemory};
    }

    state_         = SwPackageStateType::kTransferring;
    expectedBytes_ = size;
    _setState();

    return {transferId_, TransferStartSuccessType::kSuccess};
}

/// @brief TransferData
/// @param data
/// @param blockCounter
/// @return TransferDataReturnType
/// @throws no
TransferDataReturnType TransferInstance::TransferData(ByteVectorType const& data, std::uint64_t const& blockCounter)
{
    LOGD << "beign with blockCounter:" << blockCounter;
    if ((SwPackageStateType::kTransferring != state_) && (SwPackageStateType::kProcessingStream != state_)) {
        statusStorage_->RemoveStatus(key_);
        return TransferDataReturnType::kOperationNotPermitted;
    }

    if (expectedBlock_ != blockCounter) {  // when block is different
        statusStorage_->RemoveStatus(key_);
        return TransferDataReturnType::kIncorrectBlock;
    }

    if (expectedBytes_ < receivedBytes_ + data.size()) {  // when the stored data overall
        statusStorage_->RemoveStatus(key_);
        return TransferDataReturnType::kIncorrectSize;
    }

    ssize_t const nwrite{write(fd_, data.data(), data.size())};
    if (nwrite <= 0) {
        LOGE << "write data failed, nwrite:" << nwrite;
    }

    expectedBlock_++;
    receivedBytes_ += data.size();
    _setState();

    return TransferDataReturnType::kSuccess;
}

/// @brief TransferExit
/// @return TransferExitReturnType
/// @throws no
TransferExitReturnType TransferInstance::TransferExit()
{
    LOGD << "beign";
    if ((SwPackageStateType::kTransferring != state_) && (SwPackageStateType::kProcessingStream != state_)) {
        return TransferExitReturnType::kOperationNotPermitted;
    }
    if (0U == receivedBytes_) {                                 // Whether TransferData has been called yet
        return TransferExitReturnType::kOperationNotPermitted;  // SWS_UCM_00148
    }
    if (expectedBytes_ == receivedBytes_) {  // Bytes are sufficient
        std::ignore = close(fd_);
        return TransferExitReturnType::kSuccess;
    }
    return TransferExitReturnType::kInsufficientData;
}

/// @brief GetPackageFilename
/// @return Package Filename
/// @throws no
AraString TransferInstance::GetPackageFilename() const
{
    return transferDirectory_ + "/" + helper::ToHexString(transferId_) + ".zip";
}

/// @brief DeleteTransfer
/// @return DeleteTransferReturnType
/// @throws no
DeleteTransferReturnType TransferInstance::DeleteTransfer()
{
    AraString const path{GetPackageFilename()};
    LOGD << "beign with PackageFilename:" << path.c_str() << " key_:" << key_.c_str()
         << " swPackageInfoKey_:" << swPackageInfoKey_.c_str();

    AraResultVoid const ret{tinyfsys::RemoveFile(path)};
    if (!ret.HasValue()) {
        LOGE << "remove PackageFilename err:" << ret.Error().Message().data();
        return DeleteTransferReturnType::kGeneralMemoryError;
    }

    // Delete key from persistence library
    statusStorage_->RemoveSwPackageInfo(swPackageInfoKey_);
    statusStorage_->RemoveStatus(key_);

    std::ignore = close(fd_);
    return DeleteTransferReturnType::kSuccess;
}

/// @brief IsTransferring
/// @return bool
/// @throws no
bool TransferInstance::IsTransferring() const noexcept { return (state_ == SwPackageStateType::kTransferring); }

/// @brief SetState
/// @param state
/// @throws no
void TransferInstance::SetState(SwPackageStateType state)
{
    LOGD << "beign with state:" << static_cast< std::int32_t >(state);
    // Set status
    state_ = state;
    // Persist
    _setState();
}

/// @brief GetState
/// @return SwPackageStateType
/// @throws no
SwPackageStateType const& TransferInstance::GetState() const noexcept { return state_; }

/// @brief SetSwPackageInfo
/// @param swName
/// @param version
/// @throws no
void TransferInstance::SetSwPackageInfo(AraString const& swName, AraString const& version)
{
    LOGD << "beign with swName:" << swName.c_str() << " version:" << version.c_str();

    // Set name and version
    swName_  = swName;
    version_ = version;

    // Persist
    TransferStatusStorage::SwPackageInfoValueType swPackageInfo;
    swPackageInfo.swName  = swName_;
    swPackageInfo.version = version_;

    statusStorage_->StoreSwPackageInfo(swPackageInfoKey_, swPackageInfo);
}

/// @brief GetSwPackageInfo
/// @param swName
/// @param version
/// @return SwPackageInfo
/// @throws no
void TransferInstance::GetSwPackageInfo(AraString& swName, AraString& version) const
{
    std::ignore = swName;
    std::ignore = version;
    swName      = swName_;
    version     = version_;
}

/// @brief GetReceivedBytes
/// @return Received Bytes
/// @throws no
std::uint64_t TransferInstance::GetReceivedBytes() const noexcept { return receivedBytes_; }

/// @brief GetReceivedBlocks
/// @return Received Blocks
/// @throws no
std::uint64_t TransferInstance::GetReceivedBlocks() const noexcept
{
    return expectedBlock_ - static_cast< std::uint64_t >(1);
}

/// @brief _setState
/// @throws no
void TransferInstance::_setState() const
{
    TransferStatusStorage::value_type status;
    status[kStatus0U] = receivedBytes_;
    status[kStatus1U] = expectedBytes_;
    status[kStatus2U] = static_cast< uint64_t >(state_);
    status[kStatus3U] = expectedBlock_;
    statusStorage_->StoreStatus(key_, status);
}

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara
