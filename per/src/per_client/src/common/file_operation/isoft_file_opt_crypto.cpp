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
/// @file       isoft_file_opt_crypto.cpp
/// @brief      AutoSar-AP Data Persistence Storage Module
/// @details    File operation class supporting encryption
/// @date       2021-05-18
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/per/Common/File Operation
/// @interface_level=module
/// @trace_id_sr=
/// @unit_name=PFileOpt_Crypto
/// @unit_description=File operation class for encrypted storage
/// @endcode
///
/// ================================================================
///
/// @par Modification Log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// | 2021-05-18 | 1.0.0   | hanjingjing  | Initial version created                   |
/// | 2023-01-03 | 1.0.0   | hanjingjing  | Refactored file directory and encryption support |
///
/// ================================================================

#include "ara/per/internal/common/isoft_file_opt_crypto.h"

#include <nai/os/nai_dirent.h>

#include "ara/per/internal/common/isoft_common_api.h"
#include "ara/per/per_error_domain.h"

namespace ara {
namespace per {
namespace isoftkv {
//********************************/

/// @brief
/// @param nFileFlags
PFileOpt_Crypto::PFileOpt_Crypto(uint32_t const nFileFlags) noexcept : PFileOpt{nFileFlags} {}
/// @brief Destructor
PFileOpt_Crypto::~PFileOpt_Crypto() noexcept { std::ignore = pPerCrypto_.release(); }
/// @brief
/// @return
int64_t PFileOpt_Crypto::GetFileSize() const noexcept
{
    int64_t nActualFileSize{PFileOpt::GetFileSize()};
    if (nActualFileSize == 0) {
        return 0;
    }
    if (false == _IsCrypto()) {
        return nActualFileSize;
    }

    uint32_t const nCryptoStepLen{pPerCrypto_->GetKeySlotObjectSize()};
    if (nActualFileSize % nCryptoStepLen == sizeof(uint8_t)) {
        /// 2025-11-05 niuliming: Save the current cursor of the file before reading the tail data of the file
        int64_t const curTempCursor{GetPos()};

        /// 2025-11-05 niuliming: Need to read the last byte of data from the file
        std::ignore = SeekPos(ara::per::isoftkv::EFileSeekType::kSeekEnd, -1);
        uint8_t nInvalidSize{0};
        std::ignore = ReadRawData(&nInvalidSize, sizeof(nInvalidSize));

        /// 2025-11-05 niuliming: After reading the tail data of the file, restore the file cursor
        std::ignore = SeekPos(ara::per::isoftkv::EFileSeekType::kSeekSet, curTempCursor);

        return nActualFileSize - nInvalidSize - static_cast< int64_t >(sizeof(nInvalidSize));
    }

    return nActualFileSize;
}

/// @brief Read the first page of an HPKS file (N bytes from the beginning of the file)
/// @param pBuff
/// @param nBuffLen
/// @return
int32_t PFileOpt_Crypto::ReadData(uint8_t* const pBuff, uint32_t nBuffLen) const noexcept
{
    if ((nullptr == pBuff) || (nBuffLen <= 0U)) {
        return 0;
    }
    if (false == _IsCrypto()) {
        return _ReadData(pBuff, nBuffLen);
    }
    // Handle encrypted file
    uint8_t* const pWorkBuff{pNaiMapBuff_->GetBuff()};
    uint32_t const nWorkBuffLen{pNaiMapBuff_->GetLen()};

    uint32_t const nCryptoStepLen{pPerCrypto_->GetKeySlotObjectSize()};
    PER_Assert(0U == (nWorkBuffLen % nCryptoStepLen));

    /// 2025-11-02 niuliming: The starting position of data the user wants to read from the file
    int64_t const nFileCursorPos{GetPos()};

    /// 2025-11-05 niuliming: The length of valid data in the file (excluding ciphertext padding bytes + control data)
    int64_t nFileSize{GetFileSize()};
    /// 2025-11-02 niuliming: The ending position of data the user wants to read from the file
    int64_t nFileReadEndPos{nFileCursorPos + nBuffLen};
    if (nFileSize < nFileReadEndPos) {
        nFileReadEndPos = nFileSize;
    }

    /// 2025-11-02 niuliming: The starting position of the ciphertext block containing the data to be read
    int64_t nCipherTextReadPos{T_AlignNumberDown(nFileCursorPos, nCryptoStepLen)};
    /// 2025-11-02 niuliming: The offset of the data to be read relative to the first ciphertext block
    int32_t nOffsetInCipherText{static_cast< int32_t >(nFileCursorPos - nCipherTextReadPos)};

    // Restart counting the read amount
    int32_t nReadTotal = 0;
    /// 2025-11-05 niuliming: Set the starting position for reading the first ciphertext block
    std::ignore = SeekPos(ara::per::isoftkv::EFileSeekType::kSeekSet, nCipherTextReadPos);
    uint8_t nInvalidSize{0};
    while (true) {
        if (nCipherTextReadPos >= nFileReadEndPos) {
            break;
        }
        uint32_t nSingleReadSize{nWorkBuffLen};
        /// 2025-11-04 niuliming: Ensure there is at least one ciphertext block of data for the last read
        int64_t nRemainCipherTextSize{nFileSize - nCipherTextReadPos};
        if (nRemainCipherTextSize > nWorkBuffLen && (nRemainCipherTextSize - nWorkBuffLen) < nCryptoStepLen) {
            nSingleReadSize -= nCryptoStepLen;
        }
        int32_t const nReadLen{_ReadData(pWorkBuff, nSingleReadSize)};
        if (nReadLen <= 0) {
            return -1 * static_cast< int32_t >(ara::per::PerErrc::kInvalidPosition);
        }
        /// 2025-11-02 niuliming: Get the length of the ciphertext; there is 1 byte of control data at the end of the file
        int32_t nCipherTextSize{static_cast< int32_t >(T_AlignNumberDown(nReadLen, nCryptoStepLen))};
        // Decryption operation
        int32_t const nPlainTextSize{
            pPerCrypto_->CryptoData(pWorkBuff, static_cast< uint32_t >(nCipherTextSize), false)};
        if (nPlainTextSize <= 0) {
            return -1 * static_cast< int32_t >(ara::per::PerErrc::kInvalidPosition);
        }

        /// 2025-11-02 niuliming: Reached the end of the file, the data read is ciphertext block + control data
        if (nCipherTextSize != nReadLen) {
            std::ignore = T_Memcpy(&nInvalidSize, pWorkBuff + nCipherTextSize, sizeof(nInvalidSize));
        }

        /// 2025-11-02 niuliming: Copy valid data to pBuff
        uint32_t nCopySize{static_cast< uint32_t >(nCipherTextSize - nOffsetInCipherText - nInvalidSize)};
        if (nBuffLen - nReadTotal < nCopySize) {
            nCopySize = nBuffLen - nReadTotal;
        }
        std::ignore = T_Memcpy(pBuff + nReadTotal, pWorkBuff + nOffsetInCipherText, nCopySize);
        /// 2025-11-05 niuliming: Update related control fields
        nReadTotal += static_cast< int32_t >(nCopySize);
        nCipherTextReadPos += nReadLen;
        nOffsetInCipherText = 0;
    }
    /// 2025-11-05 niuliming: Set the file cursor to the end position of valid data
    std::ignore = SeekPos(ara::per::isoftkv::EFileSeekType::kSeekSet, nFileReadEndPos);
    return nReadTotal;
}
/// @brief Write data
/// @param pVoidData
/// @param nDataLen
/// @return
int32_t PFileOpt_Crypto::WriteData(const void* const pVoidData, uint32_t const nDataLen) const noexcept
{
    if ((nullptr == pVoidData) || (nDataLen <= 0U)) {
        return 0;
    }
    /// Write non-encrypted data directly
    if (false == _IsCrypto()) {
        return _WriteData(pVoidData, nDataLen);
    }
    /// Handle encrypted file
    uint8_t const* const pBData{T_TransBytes< void >(pVoidData)};
    uint8_t* const pWorkBuff{pNaiMapBuff_->GetBuff()};
    uint32_t const nWorkBuffLen{pNaiMapBuff_->GetLen()};
    uint32_t const nCryptoStepLen{pPerCrypto_->GetKeySlotObjectSize()};
    PER_Assert(0U == (nWorkBuffLen % nCryptoStepLen));
    int64_t nPosOld{GetPos()};
    /// 2025-11-05 niuliming: Determine whether the current cursor position points to valid data
    if (nPosOld != 0) {
        int64_t nFileSize{GetFileSize()};
        if (nPosOld > nFileSize) {
            /// 2025-11-05 niuliming: Adjust the current cursor to the position of the last valid data
            nPosOld = nFileSize;
        }
    }
    int64_t const nPosEnd{nPosOld + static_cast< int64_t >(nDataLen)};
    int64_t nWritePos{T_AlignNumberDown(nPosOld, nCryptoStepLen)};
    // The data at the beginning and end must be read from the source file
    uint32_t nWriteTotal{0U};
    std::ignore = SeekPos(ara::per::isoftkv::EFileSeekType::kSeekSet, nWritePos);
    // Handle the start segment
    int32_t const nCryptoOffset{static_cast< int32_t >(nPosOld - nWritePos)};
    if (nCryptoOffset > 0) {
        pNaiMapBuff_->ResetBuffData(0);
        uint32_t nCryptoReadLen = T_AlignNumberUp(static_cast< uint32_t >(nCryptoOffset), nCryptoStepLen);
        PER_Assert(nCryptoReadLen <= nWorkBuffLen);
        int32_t const nReadLen{_CryptoRead(nWritePos, pWorkBuff, nCryptoReadLen)};
        if (nReadLen < 0) {
            std::ignore = SeekPos(ara::per::isoftkv::EFileSeekType::kSeekSet, nPosOld);
            return -1 * static_cast< int32_t >(ara::per::PerErrc::kInvalidPosition);
        }
        uint32_t const nCopyLen{std::min< uint32_t >(nWorkBuffLen - static_cast< uint32_t >(nCryptoOffset),
                                                     static_cast< uint32_t >(nDataLen) - nWriteTotal)};
        std::ignore = T_Memcpy(pWorkBuff + nCryptoOffset, pBData, nCopyLen);
        // Here nWorkBuffLen must be an integer multiple of nCryptoStepLen, so rounding will not cause out-of-bounds
        uint32_t const nCryptoLen{T_AlignNumberUp(nCryptoOffset + nCopyLen, nCryptoStepLen)};
        PER_Assert(nCryptoLen <= nWorkBuffLen);

        int32_t const nWriteLen{_CryptoWrite(nWritePos, pWorkBuff, nCryptoLen)};
        if ((nWriteLen <= 0) || (static_cast< uint32_t >(nWriteLen) != nCryptoLen)) {
            std::ignore = SeekPos(ara::per::isoftkv::EFileSeekType::kSeekSet, nPosOld);
            return -1 * static_cast< int32_t >(ara::per::PerErrc::kInvalidPosition);
        }
        nWriteTotal += nCopyLen;
        nWritePos += nWriteLen;
    }
    // Handle the middle segment
    while (true) {
        if ((nWritePos + static_cast< int64_t >(nWorkBuffLen)) >= nPosEnd) {
            break;
        }
        pNaiMapBuff_->ResetBuffData(0);
        uint32_t const nCopyLen{nWorkBuffLen};
        std::ignore = T_Memcpy(pWorkBuff, pBData + nWriteTotal, nCopyLen);
        int32_t const nWriteLen{_CryptoWrite(nWritePos, pWorkBuff, nWorkBuffLen)};
        if ((nWriteLen <= 0) || (static_cast< uint32_t >(nWriteLen) != nWorkBuffLen)) {
            std::ignore = SeekPos(ara::per::isoftkv::EFileSeekType::kSeekSet, nPosOld);
            return -1 * static_cast< int32_t >(ara::per::PerErrc::kInvalidPosition);
        }
        nWriteTotal += nCopyLen;
        nWritePos += nWriteLen;
    }
    // Handle the end segment
    if (nWriteTotal < nDataLen) {
        pNaiMapBuff_->ResetBuffData(0);
        int32_t nReadLen{0};
        if ((PFileOpt::GetFileSize() - nWritePos) > 0) {
            uint32_t nCryptoReadLen = T_AlignNumberUp(static_cast< uint32_t >(nDataLen - nWriteTotal), nCryptoStepLen);
            PER_Assert(nCryptoReadLen <= nWorkBuffLen);
            nReadLen = _CryptoRead(nWritePos, pWorkBuff, nCryptoReadLen);
            if (nReadLen < 0) {
                std::ignore = SeekPos(ara::per::isoftkv::EFileSeekType::kSeekSet, nPosOld);
                return -1 * static_cast< int32_t >(ara::per::PerErrc::kInvalidPosition);
            }
        }
        uint32_t const nCopyLen{std::min< uint32_t >(nWorkBuffLen, nDataLen - nWriteTotal)};
        uint32_t const nCryptoLen{T_AlignNumberUp(nCopyLen, nCryptoStepLen)};
        PER_Assert(nCryptoLen <= nWorkBuffLen);
        std::ignore = T_Memcpy(pWorkBuff + 0, pBData + nWriteTotal, nCopyLen);
        int32_t const nWriteLen{_CryptoWrite(nWritePos, pWorkBuff, nCryptoLen)};
        if ((nWriteLen <= 0) || (static_cast< uint32_t >(nWriteLen) != nCryptoLen)) {
            std::ignore = SeekPos(ara::per::isoftkv::EFileSeekType::kSeekSet, nPosOld);
            return -1 * static_cast< int32_t >(ara::per::PerErrc::kInvalidPosition);
        }
        nWriteTotal += nCopyLen;
        nWritePos += static_cast< int64_t >(nWriteLen);
    }
    //
    if (nWritePos > 0) {
    }
    /// 2025-11-02 niuliming: Append plaintext data to the end of the file to record the number of invalid bytes in the last ciphertext block
    uint8_t nInvalidSize{static_cast< uint8_t >(GetPos() - nPosEnd)};
    std::ignore = WriteRawData(&nInvalidSize, sizeof(nInvalidSize));

    std::ignore = SeekPos(ara::per::isoftkv::EFileSeekType::kSeekSet, nPosEnd);
    return static_cast< int32_t >(nWriteTotal);
}
/// @brief Create an encryption/decryption operation object
/// @param stKeySlotName Key slot name
/// @param stAlgorithmName Encryption algorithm name
/// @return Success or failure
bool PFileOpt_Crypto::BuildCrypto(ara::core::String const& stKeySlotName,
                                  ara::core::String const& stAlgorithmName) noexcept
{
    return PFileOpt_Crypto::_BuildCrypto(stKeySlotName, stAlgorithmName, PFileOpt::GetNaiSectorSize());
}
//********************************/
/// @brief Read raw data
/// @param pBuff
/// @param nBuffLen
/// @return
int32_t PFileOpt_Crypto::ReadRawData(uint8_t* const pBuff, uint32_t const nBuffLen) const noexcept
{
    return PFileOpt::ReadData(pBuff, nBuffLen);
}
/// @brief Write raw data
/// @param pVoidData
/// @param nDataLen
/// @return
int32_t PFileOpt_Crypto::WriteRawData(const void* const pVoidData, uint32_t const nDataLen) const noexcept
{
    return PFileOpt::WriteData(pVoidData, nDataLen);
}
//********************************/
/// @brief Create an encryption/decryption operation object
/// @param stKeySlotName Key slot name
/// @param nBuffLen Buffer length
/// @return
bool PFileOpt_Crypto::_BuildCrypto(ara::core::String const& stKeySlotName,
                                   ara::core::String const& stAlgorithmName,
                                   uint32_t const nBuffLen) noexcept
{
    pPerCrypto_  = std::make_unique< ara::per::isoftkv::PPerCrypto >();
    pNaiMapBuff_ = std::make_unique< ara::per::isoftkv::PNaiMapBuff >(nBuffLen);
    if (nullptr == pPerCrypto_) {
        return false;
    }
    return pPerCrypto_->LoadCrypto(stKeySlotName, stAlgorithmName);
}

/// @brief Whether encryption is required
/// @return
bool PFileOpt_Crypto::_IsCrypto() const noexcept
{
    if (nullptr == pPerCrypto_) {
        return false;
    }
    if (nullptr == pNaiMapBuff_) {
        return false;
    }
    return true;
}
/// @brief Handle encrypted reading
/// @param nReadPos
/// @param pBData
/// @param nDataLen
/// @return
int32_t PFileOpt_Crypto::_CryptoRead(int64_t const nReadPos,
                                     uint8_t* const pBData,
                                     uint32_t const nDataLen) const noexcept
{
    std::ignore = SeekPos(ara::per::isoftkv::EFileSeekType::kSeekSet, nReadPos);
    int32_t const nReadLen{_ReadData(pBData, nDataLen)};
    if (nReadLen <= 0) {
        return nReadLen;
    }
    int32_t const nCryptoLen{pPerCrypto_->CryptoData(pBData, nDataLen, false)};
    if (nCryptoLen <= 0) {
        return -1;
    }
    // Symmetric encryption algorithm used during testing on 2022-01-12: nCryptoLen == nReadLen
    return nReadLen;
}
/// @brief Handle encrypted writing
/// @param nWritePos
/// @param pBData
/// @param nDataLen
/// @return
int32_t PFileOpt_Crypto::_CryptoWrite(int64_t const nWritePos,
                                      uint8_t* const pBData,
                                      uint32_t const nDataLen) const noexcept
{
    std::ignore = SeekPos(ara::per::isoftkv::EFileSeekType::kSeekSet, nWritePos);
    int32_t const nCryptoLen{pPerCrypto_->CryptoData(pBData, nDataLen, true)};
    if (nCryptoLen <= 0) {
        return -1;
    }
    return _WriteData(pBData, nDataLen);
}
//********************************/
}  // namespace isoftkv
}  // namespace per
}  // namespace ara
