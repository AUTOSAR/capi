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
/// @file       read_write_accessor.cpp
/// @brief      AutoSar-AP Data Persistence Storage Module
/// @details    File modification
/// @date       2021-04-27
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/per/FileStorage/File Accessor
/// @interface_level=module
/// @trace_id_sr=
/// @unit_name=
/// @unit_description=
/// @endcode
///
/// ================================================================
///
/// @par Modification Log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// | 2021-04-27 | 1.0.0   | hanjingjing  | Initial version created                   |
///
/// ================================================================

#include "ara/per/read_write_accessor.h"

#include "ara/per/internal/initialize.h"

namespace ara {
namespace per {
//********************************/
/// @brief
/// @param reddData
ReadWriteAccessor::ReadWriteAccessor(isoftkv::PReddDataMofN const& reddData) noexcept : ReadAccessor{reddData} {}
/// @brief
ReadWriteAccessor::~ReadWriteAccessor() { _SaveFileReddData(); }
/// @brief Configure callback
/// @param pCallStorage
void ReadWriteAccessor::AttachCallStorage(std::unique_ptr< isoftkv::ICallStorage >&& pCallStorage) noexcept
{
    pCallStorage_ = std::move(pCallStorage);
}
/// @brief Flush the current file content to the physical device.
/// @return
ara::core::Result< void > ReadWriteAccessor::SyncToFile() noexcept
{
    CB_DealFileOpt_Const pfunWork;
    pfunWork = [](isoftkv::PFileOpt const* const pFileOpt) noexcept -> bool { return pFileOpt->Flush(); };
    ara::core::Result< void > resultSync{_ForEachFileOptWrite(PerErrc::kValidationFailed, pfunWork)};
    if (false == resultSync.HasValue()) {
        return resultSync;
    }
    return _SaveFileReddData();
}
/// @brief Set the file size. If the file size is reduced, excess content will be discarded. The file cursor will also be adjusted to a valid position.
/// @param size
/// @return
ara::core::Result< void > ReadWriteAccessor::SetFileSize(uint64_t const size) noexcept
{
    CB_DealFileOpt_Const pfunWork;
    pfunWork = [size](isoftkv::PFileOpt const* const pFileOpt) noexcept -> bool {
        return pFileOpt->ResizeFile(static_cast< int64_t >(size));
    };
    if (false == _CheckSpace(size)) {
        return ara::core::Result< void >::FromError(PerErrc::kOutOfStorageSpace);
    }
    return _ForEachFileOptWrite(PerErrc::kInvalidSize, pfunWork);
}
/// @brief Write a string starting from the current file position.
/// @param s
/// @return
ara::core::Result< void > ReadWriteAccessor::WriteText(ara::core::StringView const s) noexcept
{
    if (false == _CheckSpace_Write(static_cast< uint32_t >(s.size()))) {
        return ara::core::Result< void >::FromError(PerErrc::kOutOfStorageSpace);
    }
    CB_DealFileOpt_Const pfunWork;
    pfunWork = [s](isoftkv::PFileOpt const* const pFileOpt) noexcept -> bool { return pFileOpt->WriteString(s) > 0; };
    return _ForEachFileOptWrite(PerErrc::kPhysicalStorageFailure, pfunWork);
}
/// @brief Write a Span byte stream starting from the current file position.
/// @param b
/// @return
ara::core::Result< void > ReadWriteAccessor::WriteBinary(ara::core::Span< ara::core::Byte const > const b) noexcept
{
    if (false == _CheckSpace_Write(static_cast< uint32_t >(b.size()))) {
        return ara::core::Result< void >::FromError(PerErrc::kOutOfStorageSpace);
    }
    CB_DealFileOpt_Const pfunWork;
    pfunWork = [b](isoftkv::PFileOpt const* const pFileOpt) noexcept -> bool {
        int32_t const nReturn{pFileOpt->WriteData(b.data(), static_cast< uint32_t >(b.size_bytes()))};
        if (nReturn <= 0) {
            return false;
        }
        return true;
    };
    return _ForEachFileOptWrite(PerErrc::kPhysicalStorageFailure, pfunWork);
}
/// @brief Use the "<<" operator to write a string at the current position.
/// @param s
/// @return
ReadWriteAccessor& ReadWriteAccessor::operator<<(ara::core::StringView const s) noexcept
{
    std::ignore = WriteText(s);
    return *this;
}
//********************************/
/// @brief Iterate over all files
/// @param eCode
/// @param pfun
/// @return
ara::core::Result< void > ReadWriteAccessor::_ForEachFileOptWrite(PerErrc eCode,
                                                                  CB_DealFileOpt_Const const& pfun) const noexcept
{
    ara::core::Vector< isoftkv::PFileOpt* > vecFileOpt{_GetFileOptVec()};
    int32_t nScanCount{0};
    int32_t nOpenCount{0};
    int32_t nEnableWrite{0};
    for (auto const& pFileOpt : vecFileOpt) {
        if (nullptr == pFileOpt) {
            continue;
        }
        nScanCount += 1;
        if (false == pFileOpt->IsOpen()) {
            continue;
        }
        nOpenCount += 1;
        if (false == pFileOpt->IsEnableWrite()) {
            continue;
        }
        if (pfun(pFileOpt)) {
            nEnableWrite += 1;
        }
    }
    int32_t const nLeastCount{_GetLeastSuccessCount()};

    if (nOpenCount < nLeastCount) {
        return ara::core::Result< void >::FromError(PerErrc::kFileNotFound);
    }
    if (nEnableWrite < nLeastCount) {
        return ara::core::Result< void >::FromError(PerErrc::kInvalidOpenMode);
    }
    if (nScanCount < nLeastCount) {
        return ara::core::Result< void >::FromError(std::move(eCode));
    }
    return ara::core::Result< void >::FromValue();
}
/// @brief Save potentially existing redundancy check information
/// @return
ara::core::Result< void > ReadWriteAccessor::_SaveFileReddData() const noexcept
{
    CB_DealFileOpt_Const pfunWork;
    pfunWork = [this](isoftkv::PFileOpt const* const pFileOpt) noexcept -> bool {
        if (pCallStorage_) {
            std::ignore = pCallStorage_->CallStorage_WriteReddData(pFileOpt, GetFileName_InStorage());
        }
        return true;
    };
    return _ForEachFileOptWrite(PerErrc::kValidationFailed, pfunWork);
}
/// @brief Check if space allows writing new data
/// @param nAmendLen
/// @return
bool ReadWriteAccessor::_CheckSpace(uint64_t nNewFileSize) noexcept
{
    if (false == pCallStorage_.operator bool()) {
        return false;
    }
    uint64_t nFileSize{GetSize()};
    if (nNewFileSize <= nFileSize) {
        return true;
    }
    int32_t nAddLen{static_cast< int32_t >(nNewFileSize - nFileSize)};
    return pCallStorage_->CheckSpace(nAddLen);
}
/// @brief Check if space allows writing new data: Calculation method starts from the current position; if it exceeds the original file size, verification is required
/// @param nAmendLen
/// @return
bool ReadWriteAccessor::_CheckSpace_Write(uint32_t nWriteSize) noexcept
{
    if (false == pCallStorage_.operator bool()) {
        return false;
    }
    uint64_t nFileSize{GetSize()};
    uint64_t nPos{GetPosition()};
    if (nPos + static_cast< uint64_t >(nWriteSize) <= nFileSize) {
        return true;
    }
    int32_t nAddLen{static_cast< int32_t >(nPos + static_cast< uint64_t >(nWriteSize) - nFileSize)};
    return pCallStorage_->CheckSpace(nAddLen);
}
//********************************/
}  // namespace per
}  // namespace ara
