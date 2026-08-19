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
/// @file       isoft_file_opt_page.cpp
/// @brief      AutoSar-AP data persistence storage module
/// @details    File operation class reading by page
/// @date       2021-05-18
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification Log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// <tr><td>2021-05-18  <td>1.0.0    <td>hanjingjing      <td>Create initial version
/// <tr><td>2023-01-03  <td>1.0.0    <td>hanjingjing      <td>Refactor file directory and crypto support
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/per/Common/File Operations
/// @interface_level=module
/// @trace_id_sr=
/// @unit_name=PFileOpt_Page
/// @unit_description=File operation class reading by page
/// @endcode
///
/// ================================================================

#include "ara/per/internal/common/isoft_file_opt_page.h"

#include <nai/os/nai_dirent.h>
#include <nai/os/nai_mman.h>

#include "ara/per/internal/common/isoft_common_api.h"
#include "ara/per/internal/common/isoft_file_opt_page.h"
#include "ara/per/internal/isoftkv/kv_log_api.h"

namespace ara {
namespace per {
namespace isoftkv {
/// @brief File lock operation class
/// @brief Constructor setting direct read page size
/// @param nPageLen
PFileOpt_Page::PFileOpt_Page(uint16_t const nPageLen) noexcept
    : PFileOpt_Crypto{static_cast< uint32_t >(NAI_O_CREAT | NAI_O_RDWR | NAI_O_DIRECT)}, nPageLen_{nPageLen}
{
    if (0U == nPageLen_) {
        nPageLen_ = static_cast< uint16_t >(PFileOpt::GetNaiSectorSize());
    }
}
/// @brief
PFileOpt_Page::~PFileOpt_Page() noexcept { std::ignore = UnlockFile(); }
/// @brief Create encrypt/decrypt operation object
/// @param stKeySlotName Key slot name
/// @param stAlgorithmName Encryption algorithm name
/// @return Success or failure
bool PFileOpt_Page::BuildCrypto(ara::core::String const& stKeySlotName,
                                ara::core::String const& stAlgorithmName) noexcept
{
    uint32_t nBuffLen{PFileOpt::GetNaiSectorSize()};
    if ((nPageLen_ > 0U) && (static_cast< uint32_t >(nPageLen_) > nBuffLen)) {
        nBuffLen = T_AlignNumberUp(static_cast< uint32_t >(nPageLen_), nBuffLen);
    }
    return PFileOpt_Crypto::_BuildCrypto(stKeySlotName, stAlgorithmName, nBuffLen);
}
/// @brief Prepare everything before reading/writing
/// @param stFileName
/// @param nPageLen
/// @return
bool PFileOpt_Page::DoPrepareWork(ara::core::StringView const& stFileName, uint16_t const nPageLen) noexcept
{
    // PFileOpt::DoPrepareWork implements UnlockFile function in base class
    if (false == PFileOpt_Crypto::DoPrepareWork(stFileName, nPageLen)) {
        return false;
    }
    bLockSuccess_ = LockFile(kFileLockWrite | kFileLockTry);
    if (false == bLockSuccess_) {
        LogError() << "File [" << stFileName.data() << "] is locked when opened, error code = " << GetLastError();
        return false;
    }
    nPageLen_ = nPageLen;
    // May reset Nai buffer length
    ara::per::isoftkv::PNaiMapBuff* const pNaiBuff{_GetNaiBuff()};
    if (nullptr != pNaiBuff) {
        pNaiBuff->ResizeBuff(static_cast< uint32_t >(nPageLen_));
    }
    return true;
}
/// @brief Check if file is open
/// @return
bool PFileOpt_Page::IsOpen() const noexcept
{
    bool bOpen = PFileOpt_Crypto::IsOpen();
    if (false == bOpen) {
        return false;
    }
    return bLockSuccess_;
}
/// @brief Check if file lock was successful
/// @return
bool PFileOpt_Page::IsLockSuccess() const noexcept { return bLockSuccess_; }
/// @brief Read one page of data, nPageID starts from 1
/// @param nPageID
/// @param pBuff
/// @param nBuffLen
/// @return Return actual number of data bytes read
int32_t PFileOpt_Page::ReadPage(uint32_t const nPageID, uint8_t* const pBuff, uint32_t const nBuffLen) const noexcept
{
    if (nPageID <= 0U) {
        return -1;
    }
    if ((nullptr == pBuff) || (nBuffLen <= 0U)) {
        return -1;
    }
    // nPageID counts from 1
    int64_t const nOffset{(static_cast< int64_t >(nPageID) - 1) * static_cast< int64_t >(nPageLen_)};
    // Direct processing for non-encrypted files
    if (false == _IsCrypto()) {
        return _ReadData(pBuff, nBuffLen, nOffset);
    }
    // Processing for encrypted files
    ara::per::isoftkv::PNaiMapBuff* const pNaiBuff{_GetNaiBuff()};
    pNaiBuff->ResetBuffData(0U);
    uint8_t* const pWorkBuff{pNaiBuff->GetBuff()};
    uint32_t const nWorkBuffLen{pNaiBuff->GetLen()};
    PER_Assert(nWorkBuffLen >= static_cast< uint32_t >(nBuffLen));
    // Decryption operation
    int32_t const nReadLen{_CryptoRead(nOffset, pWorkBuff, nWorkBuffLen)};
    if (nReadLen <= 0) {
        return -1;
    }
    std::ignore = T_Memcpy(pBuff, pWorkBuff, static_cast< uint32_t >(nReadLen));
    return nReadLen;
}
/// @brief
/// @param nPageID
/// @param pBData
/// @param nDataLen
/// @return
bool PFileOpt_Page::WritePage(uint32_t const nPageID,
                              uint8_t const* const pBData,
                              int32_t const nDataLen) const noexcept
{
    if (nPageID <= 0U) {
        return false;
    }
    if ((nullptr == pBData) || (nDataLen <= 0)) {
        return false;
    }
    int64_t const nNeedFileLen{static_cast< int64_t >(nPageID) * static_cast< int64_t >(nPageLen_)};
    // Expand file space
    if (nNeedFileLen > GetFileSize()) {
        if (false == ResizePage(nPageID)) {
            return false;
        }
    }
    int64_t const nOffset{(static_cast< int64_t >(nPageID) - 1) * static_cast< int64_t >(nPageLen_)};
    // Direct processing for non-encrypted files
    if (false == _IsCrypto()) {
        intptr_t const nNaiReturn{_WriteData(pBData, static_cast< uint32_t >(nDataLen), nOffset)};
        if (nNaiReturn < 0) {
            return false;
        }
        if (nNaiReturn != nDataLen) {
            return false;
        }
        return true;
    }
    // Processing for encrypted files
    ara::per::isoftkv::PNaiMapBuff* const pNaiBuff{_GetNaiBuff()};
    uint8_t* const pWorkBuff{pNaiBuff->GetBuff()};
    uint32_t const nWorkBuffLen{pNaiBuff->GetLen()};
    PER_Assert(nWorkBuffLen >= static_cast< uint32_t >(nDataLen));
    // Encryption operation
    std::ignore = T_Memcpy(pWorkBuff, pBData, static_cast< uint32_t >(nDataLen));
    int32_t const nWriteLen{_CryptoWrite(nOffset, pWorkBuff, nWorkBuffLen)};
    if (nWriteLen <= 0) {
        ara::per::isoftkv::LogError() << "CryptoError : File = " << GetFileName().c_str() << "";
    }
    return nWriteLen == static_cast< int32_t >(nWorkBuffLen);
}
/// @brief Reset file size
/// @param nPageCount
/// @return
bool PFileOpt_Page::ResizePage(uint32_t const nPageCount) const noexcept
{
    return PFileOpt::ResizeFile(static_cast< int64_t >(nPageLen_) * static_cast< int64_t >(nPageCount));
}
/// @brief
/// @param nPageLen
/// @return
uint32_t PFileOpt_Page::NewPage(int32_t const nPageLen) const noexcept
{
    // TODO(Responsible): Allocate a new page of data at end of file
    std::ignore = SeekPos(EFileSeekType::kSeekEnd, 0);
    uint32_t const nPageID{T_Div< int64_t, uint32_t >(GetFileSize(), static_cast< int64_t >(nPageLen)) + 1U};
    if (false == ResizePage(nPageID)) {
        return 0U;
    }
    return nPageID;
}
/// @brief Get Page count
/// @return
uint32_t PFileOpt_Page::GetPageTotal() const noexcept
{
    return T_DivUp< int64_t, uint32_t >(GetFileSize(), static_cast< int64_t >(nPageLen_));
}
/// @brief Get page-aligned file size
/// @return
int64_t PFileOpt_Page::GetFileSizeByPage() const noexcept
{
    return static_cast< int64_t >(
        (GetFileSize() / static_cast< int64_t >(nPageLen_) * static_cast< int64_t >(nPageLen_)));
}
/// @brief Move file cursor by page unit
/// @param eMoveType
/// @param nPageID
/// @return
bool PFileOpt_Page::SeekPosByPageID(EFileSeekType const eMoveType, uint32_t const nPageID) const noexcept
{
    if (nPageID <= 0U) {
        return false;
    }
    // Page 1 starts from offset 0
    uint32_t const nMovePos{static_cast< uint32_t >(nPageLen_) * (nPageID - 1U)};
    return SeekPos(eMoveType, static_cast< int64_t >(nMovePos));
}
//********************************/
int64_t PFileOpt_Page::GetLastSectorStartPos() const noexcept
{
    return static_cast< int64_t >(((GetFileSize() / static_cast< int64_t >(GetNaiSectorSize()) - 1)
                                   * static_cast< int64_t >(GetNaiSectorSize())));
}

}  // namespace isoftkv
}  // namespace per
}  // namespace ara
