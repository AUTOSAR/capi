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
/// @file       read_accessor.cpp
/// @brief      AutoSar-AP data persistence storage module
/// @details    File reading
/// @date       2021-04-27
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification Log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// <tr><td>2021-04-27  <td>1.0.0    <td>hanjingjing      <td>Create initial version
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/per/FileStorage/File Accessor
/// @interface_level=module
/// @trace_id_sr=
/// @unit_name=ReadAccessor
/// @unit_description=File reading
/// @endcode
///
/// ================================================================

#include "ara/per/read_accessor.h"

#include <ara/core/map.h>

#include "ara/per/internal/common/isoft_common_api.h"
#include "ara/per/internal/common/isoft_file_refuse_opt.h"
#include "ara/per/internal/common/isoft_per_common_api.h"
#include "ara/per/internal/common/isoft_select_mofn.h"
#include "ara/per/internal/initialize.h"
#include "ara/per/internal/isoftkv/kv_error_code.h"
#include "ara/per/internal/isoftkv/nai_map_buff.h"
#include "ara/per/per_error_domain.h"
#include "ara/per/recovery.h"

namespace {
/// @brief
/// @tparam T
/// @param vecReadData
/// @param nSelectIndex
/// @return
/// @throws
template < typename T >
inline ara::core::Result< T > T_GetVecResult(ara::core::Vector< ara::core::Result< T > > &vecReadData,
                                             int32_t const nSelectIndex)
{
    if (nSelectIndex < 0) {
        for (auto &resultData : vecReadData) {
            if (false == resultData.HasValue()) {
                return resultData;
            }
        }
        return ara::core::Result< T >::FromError(ara::per::PerErrc::kValidationFailed);
    }
    std::size_t const nIndex{static_cast< std::size_t >(nSelectIndex)};
    PER_Assert(nIndex < vecReadData.size());
    return std::move(vecReadData[nIndex]);
}
/// @brief Calculate Hash value, using CRC32 algorithm as of 2021-11-03
/// @param pData
/// @param nLen
/// @param nParam
/// @return
uint32_t G_CalHashID(const void *const pData, uint32_t const nLen, uint32_t const nParam) noexcept
{
    PER_Assert((nullptr != pData) && (nLen > 0U));
    return ara::per::isoftkv::CalculateCrc32(pData, nLen, nParam);
}
/// @brief Check if file operation object for reading is valid
/// @param[in] pFileOpt
/// @return
ara::core::Result< void > G_CheckReadOpt(ara::per::isoftkv::PFileOpt const *const pFileOpt) noexcept
{
    if (false == ara::per::isoftkv::IsPerInitialize()) {
        return ara::core::Result< void >::FromError(ara::per::PerErrc::kNotInitialized);
    }
    if (nullptr == (pFileOpt)) {
        return ara::core::Result< void >::FromError(ara::per::PerErrc::kFileNotFound);
    }
    if (false == (pFileOpt)->IsOpen()) {
        return ara::core::Result< void >::FromError(ara::per::PerErrc::kFileNotFound);
    }
    if ((pFileOpt)->IsEof()) {
        return ara::core::Result< void >::FromError(ara::per::PerErrc::kIsEof);
    }
    if ((pFileOpt)->GetFileSize() <= 0) {
        return ara::core::Result< void >::FromError(ara::per::PerErrc::kIsEof);
    }
    return ara::core::Result< void >::FromValue();
}
}  // namespace

namespace ara {
namespace per {
//********************************/
/// @brief
class ReadAccessor::PAccessBuff final
{
private:
    /// @brief Local default buffer
    isoftkv::PNaiMapBuff naiMapBuff_;
    /// @brief User-attached buffer
    ara::core::Span< ara::core::Byte > *pSpanBuff_;

public:
    /// @brief
    PAccessBuff() noexcept = default;
    /// @brief
    ~PAccessBuff() noexcept = default;
    /// @brief
    /// @param nDefBuffLen
    explicit PAccessBuff(uint32_t const nDefBuffLen) noexcept;
    /// @brief
    /// @param a
    PAccessBuff(PAccessBuff const &a) = delete;
    /// @brief
    /// @param a
    /// @return
    PAccessBuff &operator=(PAccessBuff const &a) = delete;
    /// @brief
    /// @param a
    PAccessBuff(PAccessBuff &&a) noexcept;
    /// @brief
    /// @param a
    /// @return
    PAccessBuff &operator=(PAccessBuff &&a) noexcept;
    /// @brief Bind a SpanBuffer
    /// @param pSpanBuff
    /// @return
    bool AttachSpanBuff_Inter(ara::core::Span< ara::core::Byte > *const pSpanBuff) noexcept;
    /// @brief Whether there is a SpanBuffer
    /// @return
    bool IsHaveSpanBuff() const noexcept;
    /// @brief Get working Buffer
    /// @return
    uint8_t *GetWorkBuff() const noexcept;
    /// @brief Return working Buffer length
    /// @return
    uint32_t GetWorkBuffLen() const noexcept;
    /// @brief Return NaiBuffer length
    /// @return
    uint32_t GetNaiBuffLen() const noexcept;
};
/// @brief Construct
/// @param nDefBuffLen
ReadAccessor::PAccessBuff::PAccessBuff(uint32_t const nDefBuffLen) noexcept
    : naiMapBuff_{nDefBuffLen}, pSpanBuff_{nullptr}
{
    uint32_t nNaiLen{0U};
    nNaiLen = isoftkv::PFileOpt::GetNaiSectorSize();
    PER_Assert((nDefBuffLen > 0U) && (0U == (nDefBuffLen % nNaiLen)));
}
/// @brief
/// @param a
ReadAccessor::PAccessBuff::PAccessBuff(ReadAccessor::PAccessBuff &&a) noexcept
    : naiMapBuff_{std::move(std::move(a).naiMapBuff_)}, pSpanBuff_{std::move(std::move(a).pSpanBuff_)}
{
    a.pSpanBuff_ = nullptr;
}
/// @brief
/// @param a
/// @return
ReadAccessor::PAccessBuff &ReadAccessor::PAccessBuff::operator=(ReadAccessor::PAccessBuff &&a) noexcept
{
    naiMapBuff_  = std::move(std::move(a).naiMapBuff_);
    pSpanBuff_   = std::move(a).pSpanBuff_;
    a.pSpanBuff_ = nullptr;
    return *this;
}
/// @brief Bind a SpanBuffer
/// @param pSpanBuff
/// @return
bool ReadAccessor::PAccessBuff::AttachSpanBuff_Inter(ara::core::Span< ara::core::Byte > *const pSpanBuff) noexcept
{
    pSpanBuff_ = pSpanBuff;
    if (nullptr != pSpanBuff) {
        uint32_t const nNaiLen{isoftkv::PFileOpt::GetNaiSectorSize()};
        return 0U == (pSpanBuff->size() % nNaiLen);
    }
    return true;
}
/// @brief Whether there is a SpanBuffer
/// @return
bool ReadAccessor::PAccessBuff::IsHaveSpanBuff() const noexcept
{
    if (nullptr == pSpanBuff_) {
        return false;
    }
    if (nullptr == pSpanBuff_->data()) {
        return false;
    }
    if (pSpanBuff_->size_bytes() <= 0U) {
        return false;
    }
    return true;
}
/// @brief Get working Buffer
/// @return
uint8_t *ReadAccessor::PAccessBuff::GetWorkBuff() const noexcept
{
    if (IsHaveSpanBuff()) {
        return isoftkv::T_TransBytes(pSpanBuff_->data());
    }
    return naiMapBuff_.GetBuff();
}
/// @brief Return working Buffer length
/// @return
uint32_t ReadAccessor::PAccessBuff::GetWorkBuffLen() const noexcept
{
    if (IsHaveSpanBuff()) {
        return static_cast< uint32_t >(pSpanBuff_->size_bytes());
    }
    return naiMapBuff_.GetLen();
}
/// @brief Return NaiBuffer length
/// @return
uint32_t ReadAccessor::PAccessBuff::GetNaiBuffLen() const noexcept { return naiMapBuff_.GetLen(); }
//********************************/
/// @brief Constructor
/// @param reddData
ReadAccessor::ReadAccessor(isoftkv::PReddDataMofN const &reddData) noexcept
    : reddDataMofN_{reddData}
    , pAccessBuff_{std::make_unique< PAccessBuff >(isoftkv::PFileOpt::GetNaiSectorSize())}
    , pConfigCrypto_{}
{
}
/// @brief
ReadAccessor::~ReadAccessor() noexcept
{
    for (std::unique_ptr< isoftkv::PFileOpt > &fileOpt : vecReddFileOpt_) {
        std::ignore = fileOpt.release();
    }
    vecReddFileOpt_.clear();
    std::ignore = pConfigCrypto_.release();
    ara::per::isoftkv::PFileRefuseOpt::GetInstance()->OperationComplete(stFileName_);
}
/// @brief Bind a SpanBuffer
/// @param pSpanBuff
void ReadAccessor::AttachSpanBuff(ara::core::Span< ara::core::Byte > *const pSpanBuff) const noexcept
{
    std::ignore = pAccessBuff_->AttachSpanBuff_Inter(pSpanBuff);
}
/// @brief Set encryption configuration
/// @param configCrypto
void ReadAccessor::AttachCryptoConfig(isoftkv::PConfigData_Crypto const &configCrypto) noexcept
{
    pConfigCrypto_  = std::make_unique< isoftkv::PConfigData_Crypto >();
    *pConfigCrypto_ = configCrypto;
}
/// @brief Initialize file operation class object
/// @param stFileName The filename to be access.
/// @param nFileFlags File operation mode parameters used by the nai library
/// @return Whether successful
ara::core::Result< bool > ReadAccessor::InitFileOpt(ara::core::StringView const &stFileName,
                                                    uint32_t const nFileFlags) noexcept
{
    int32_t nReddCount{1};
    if (reddDataMofN_.IsValid()) {
        nReddCount = static_cast< int32_t >(reddDataMofN_.n);
    }
    PerErrc perError{PerErrc::kPhErrorCode};
    int nScanCount{0};
    nScanCount = isoftkv::ScanAllReddFile(
        stFileName, nReddCount,
        [this, nFileFlags, &perError](ara::core::StringView const &stNewFileName) noexcept -> bool {
            std::unique_ptr< isoftkv::PFileOpt_Crypto > pFileOpt;
            pFileOpt = std::make_unique< isoftkv::PFileOpt_Crypto >(nFileFlags);
            if (false == pFileOpt->DoPrepareWork(stNewFileName)) {
                perError = PerErrc::kFileNotFound;
                return false;
            }
            if (this->_IsHaveCryptoEvent()) {
                if (false == pFileOpt->BuildCrypto(pConfigCrypto_->stKeySlotName, pConfigCrypto_->stCryptoAlgorithm)) {
                    perError = PerErrc::kEncryptionFailed;
                    return false;
                }
            }
            vecReddFileOpt_.push_back(std::move(pFileOpt));
            return true;
        });
    if (PerErrc::kPhErrorCode == perError) {
        return ara::core::Result< bool >::FromValue(nReddCount == nScanCount);
    }
    return ara::core::Result< bool >::FromError(perError);
}
/// @brief Initialize the Storage information associated with the file
/// @param stStorageIns
/// @param stFileName
void ReadAccessor::InitOwnStorage(ara::core::StringView const &stStorageIns, ara::core::StringView const &stFileName)
{
    stStorageIns_ = stStorageIns;
    stFileName_   = isoftkv::T_String(stFileName);
}
/// @brief Verify the potentially existing PConfigData_Crypto.stVerfHash
/// @return
bool ReadAccessor::VerifCryptoHash() const noexcept
{
    if (nullptr == pConfigCrypto_.get()) {
        return true;
    }
    // Only Virefi requires verification
    if (isoftkv::ECryptoKeySlotUsage::kVerification != pConfigCrypto_->eKeySlotUsage) {
        return true;
    }
    if (pConfigCrypto_->stVerfHash.empty()) {
        return false;
    }

    std::unique_ptr< isoftkv::IPerCrypto > pPerCryptoSymmetric{isoftkv::NewPerCrypto()};
    PER_Assert(pPerCryptoSymmetric->LoadCrypto(pConfigCrypto_->stKeySlotName, pConfigCrypto_->stCryptoAlgorithm));
    ara::core::String const &stCryptoAlgorithm{pConfigCrypto_->stCryptoAlgorithm};
    // Election algorithm callback function
    std::function< ara::core::Vector< uint8_t >(int32_t const nIndex, uint32_t &nHashData) > pfunWork;
    pfunWork = [this, &pPerCryptoSymmetric, stCryptoAlgorithm](int32_t const nIndex,
                                                               uint32_t &nHashData) -> ara::core::Vector< uint8_t > {
        nHashData = 0;
        ara::core::Vector< uint8_t > vecOut;
        isoftkv::PFileOpt *const pFileOpt{_GetFileOpt(nIndex)};
        if (nullptr != pFileOpt) {
            vecOut = pPerCryptoSymmetric->CryptoFileHash(*pFileOpt, stCryptoAlgorithm);
            if (false == vecOut.empty()) {
                nHashData = isoftkv::CalculateCrc32(static_cast< void * >(vecOut.data()),
                                                    static_cast< uint32_t >(vecOut.size()), 0);
            }
        }
        return vecOut;
    };
    // Calculate the file's Hash code
    ara::core::Vector< uint8_t > vecFileHash;
    if (false == reddDataMofN_.IsValid()) {
        uint32_t nHashData{0};
        vecFileHash = pfunWork(0, nHashData);
        if (nHashData == 0) {
            return false;
        }
    } else {
        if (false
            == isoftkv::SelectMofN< ara::core::Vector< uint8_t > >(vecFileHash, reddDataMofN_.m, reddDataMofN_.n,
                                                                   pfunWork)) {
            return false;
        }
    }
    // Compare file Hash
    return isoftkv::CompareHashData(vecFileHash, pConfigCrypto_->stVerfHash);
}
//********************************/
/// @brief Returns the character at the current file position, without moving the current position.
/// @return
ara::core::Result< char8_t > ReadAccessor::PeekChar() const noexcept
{
    // 2022-12-28 hanjingjing: If the file is encrypted, the return value of this function is no longer correct
    return _ReadDataByMofN< char8_t >(true);
}
/// @brief Returns the byte at the current file position, without moving the current position.
/// @return
ara::core::Result< ara::core::Byte > ReadAccessor::PeekByte() const noexcept
{
    return _ReadDataByMofN< ara::core::Byte >(true);
}
/// @brief Returns the character at the current file position, moving the current position forward.
/// @return
ara::core::Result< char8_t > ReadAccessor::GetChar() const noexcept { return _ReadDataByMofN< char8_t >(false); }
/// @brief Returns the byte at the current file position, moving the current position forward.
/// @return
ara::core::Result< ara::core::Byte > ReadAccessor::GetByte() const noexcept
{
    return _ReadDataByMofN< ara::core::Byte >(false);
}
/// @brief Reads all characters from the current position to the end of the file, moves the cursor to the end of the file, and returns a string.
/// @return
ara::core::Result< ara::core::String > ReadAccessor::ReadText() const noexcept
{
    return ReadText(static_cast< uint64_t >(-1));
}
/// @brief Reads up to n characters from the current position, moves the cursor forward, and returns a string.
/// @param nLen
/// @return
ara::core::Result< ara::core::String > ReadAccessor::ReadText(uint64_t const nLen) const noexcept
{
    // 2021-11-03, this function does not optimize storage; the current version uses extra memory (storing multiple redundant copies of read results). Further optimization could involve calculating the HashID of each redundant version first, then performing an extra read after selection.
    ara::core::Result< void > const check{_CheckReadReady()};
    if (false == check.HasValue()) {
        return isoftkv::T_ErrorResult< void, ara::core::String >(check);
    }
    if (false == reddDataMofN_.IsValid()) {
        return _ReadString(_GetMainFileOpt(), nLen);
    }
    // Enter redundancy handling process
    std::function< ara::core::Result< ara::core::String >(isoftkv::PFileOpt const *const) > pfunWork;
    pfunWork = [this, nLen](isoftkv::PFileOpt const *const pFileOpt) -> ara::core::Result< ara::core::String > {
        if (nullptr == pFileOpt) {
            return ara::core::Result< ara::core::String >::FromError(PerErrc::kFileNotFound);
        }
        return _ReadString(pFileOpt, nLen);
    };
    return _ReadFileByMofN< ara::core::String >(0U, pfunWork);
}
/// @brief Reads all bytes from the current position to the end of the file, moves the cursor to the end of the file, and returns a vector byte stream.
/// @return
ara::core::Result< ara::core::Vector< ara::core::Byte > > ReadAccessor::ReadBinary() const noexcept
{
    return ReadBinary(static_cast< uint64_t >(-1));
}
/// @brief Reads up to n bytes from the current position, moves the cursor forward, and returns a vector byte stream.
/// @param nLen
/// @return
ara::core::Result< ara::core::Vector< ara::core::Byte > > ReadAccessor::ReadBinary(uint64_t const nLen) const noexcept
{
    // 2021-11-03, this function does not optimize storage; the current version uses extra memory (storing multiple redundant copies of read results). Further optimization could involve calculating the HashID of each redundant version first, then performing an extra read after selection.
    ara::core::Result< void > const check{_CheckReadReady()};
    if (false == check.HasValue()) {
        return isoftkv::T_ErrorResult< void, ara::core::Vector< ara::core::Byte > >(check);
    }
    if (false == reddDataMofN_.IsValid()) {
        return _ReadBinary(_GetMainFileOpt(), nLen);
    }
    // Enter redundancy handling process
    std::function< ara::core::Result< ara::core::Vector< ara::core::Byte > >(isoftkv::PFileOpt const *const) > pfunWork;
    pfunWork
        = [this,
           nLen](isoftkv::PFileOpt const *const pFileOpt) -> ara::core::Result< ara::core::Vector< ara::core::Byte > > {
        if (nullptr == pFileOpt) {
            return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(PerErrc::kFileNotFound);
        }
        return _ReadBinary(pFileOpt, nLen);
    };
    return _ReadFileByMofN< ara::core::Vector< ara::core::Byte > >(0U, pfunWork);
}
/// @brief Reads a line of string, default line terminator is '\n'
/// @param delimiter
/// @return
ara::core::Result< ara::core::String > ReadAccessor::ReadLine(char8_t const delimiter) const noexcept
{
    ara::core::Result< void > const check{_CheckReadReady()};
    if (false == check.HasValue()) {
        return isoftkv::T_ErrorResult< void, ara::core::String >(check);
    }
    if (false == reddDataMofN_.IsValid()) {
        return _ReadLine(_GetMainFileOpt(), delimiter);
    }
    // Enter redundancy handling process
    std::function< ara::core::Result< ara::core::String >(isoftkv::PFileOpt const *const pFileOpt) > pfunWork;
    pfunWork = [this, delimiter](isoftkv::PFileOpt const *const pFileOpt) -> ara::core::Result< ara::core::String > {
        if (nullptr == pFileOpt) {
            return ara::core::Result< ara::core::String >::FromError(PerErrc::kFileNotFound);
        }
        return _ReadLine(pFileOpt, delimiter);
    };
    return _ReadFileByMofN< ara::core::String >(0U, pfunWork);
}
/// @brief Returns the current file size in bytes.
/// @return
uint64_t ReadAccessor::GetSize() const noexcept
{
    if (false == reddDataMofN_.IsValid()) {
        isoftkv::PFileOpt *const pFileOpt{_GetMainFileOpt()};
        if (nullptr == pFileOpt) {
            return 0U;
        }
        if (false == pFileOpt->IsOpen()) {
            return 0U;
        }
        return static_cast< uint64_t >(pFileOpt->GetFileSize());
    }
    // Enter redundancy handling flow
    std::function< ara::core::Result< uint64_t >(isoftkv::PFileOpt const *const pFileOpt) > pfunWork;
    pfunWork = [](isoftkv::PFileOpt const *const pFileOpt) -> ara::core::Result< uint64_t > {
        if (nullptr == pFileOpt) {
            return ara::core::Result< uint64_t >::FromError(PerErrc::kFileNotFound);
        }
        if (false == pFileOpt->IsOpen()) {
            return ara::core::Result< uint64_t >::FromError(PerErrc::kFileNotFound);
        }
        return ara::core::Result< uint64_t >::FromValue(pFileOpt->GetFileSize());
    };
    ara::core::Result< uint64_t > result{static_cast< uint64_t >(0)};
    result = _GetFileInfoByMofN< uint64_t >(0U, pfunWork);
    if (false == result.HasValue()) {
        return 0U;
    }
    return result.Value();
}
/// @brief Returns the current file cursor: position offset relative to the beginning of the file.
/// @return
uint64_t ReadAccessor::GetPosition() const noexcept
{
    ara::core::Result< uint64_t > const result{_GetPosition()};
    if (false == result.HasValue()) {
        return 0U;
    }
    return result.Value();
}
/// @brief Sets the current file cursor: position offset relative to the beginning of the file.
/// @param nPosition
/// @return
ara::core::Result< void > ReadAccessor::SetPosition(uint64_t const nPosition) noexcept
{
    ara::core::Result< void > const check{_CheckReadReady()};
    if (false == check.HasValue()) {
        return isoftkv::T_ErrorResult< void, void >(check);
    }
    CB_DealFileOpt_Const pfunWork;
    pfunWork = [nPosition](isoftkv::PFileOpt const *const pFileOpt) noexcept -> bool {
        return pFileOpt->SeekPos(isoftkv::EFileSeekType::kSeekSet, static_cast< int64_t >(nPosition));
    };
    int32_t const nOpenCount{_ForEachFileOpt(std::move(pfunWork))};
    if (nOpenCount < _GetLeastSuccessCount()) {
        return ara::core::Result< void >::FromError(PerErrc::kInvalidPosition);
    }
    return ara::core::Result< void >::FromValue();
}
/// @brief Moves the current file cursor according to the parameter, which can be positive, negative, or 0, with the base position specified by the Origin enumeration.
/// @param origin
/// @param offset
/// @return
ara::core::Result< uint64_t > ReadAccessor::MovePosition(Origin const origin, int64_t const offset) noexcept
{
    ara::core::Result< void > const check{_CheckReadReady()};
    if (false == check.HasValue()) {
        return isoftkv::T_ErrorResult< void, uint64_t >(check);
    }
    // Origin corresponds to EFileSeekType
    CB_DealFileOpt_Const pfunWork;
    pfunWork = [origin, offset](isoftkv::PFileOpt const *const pFileOpt) noexcept -> bool {
        return pFileOpt->SeekPos(static_cast< isoftkv::EFileSeekType >(origin), offset);
    };
    int32_t const nOpenCount{_ForEachFileOpt(std::move(pfunWork))};
    if (nOpenCount < _GetLeastSuccessCount()) {
        return ara::core::Result< uint64_t >::FromError(PerErrc::kInvalidPosition);
    }
    return _GetPosition();
}
/// @brief Returns whether the current file cursor has reached the end of the file.
/// @return
bool ReadAccessor::IsEof() const noexcept
{
    if (false == reddDataMofN_.IsValid()) {
        isoftkv::PFileOpt *const pFileOpt{_GetMainFileOpt()};
        if (nullptr == pFileOpt) {
            return false;
        }
        if (false == pFileOpt->IsOpen()) {
            return false;
        }
        return pFileOpt->IsEof();
    }
    // Enter redundancy handling process
    std::function< ara::core::Result< bool >(isoftkv::PFileOpt const *const pFileOpt) > pfunWork;
    pfunWork = [](isoftkv::PFileOpt const *const pFileOpt) -> ara::core::Result< bool > {
        if (nullptr == pFileOpt) {
            return ara::core::Result< bool >::FromError(PerErrc::kFileNotFound);
        }
        if (false == pFileOpt->IsOpen()) {
            return ara::core::Result< bool >::FromError(PerErrc::kFileNotFound);
        }
        return ara::core::Result< bool >::FromValue(pFileOpt->IsEof());
    };
    ara::core::Result< bool > resultGet{false};
    resultGet = _GetFileInfoByMofN< bool >(static_cast< uint64_t >(-1), std::move(pfunWork));
    if (false == resultGet.HasValue()) {
        return false;
    }
    return resultGet.Value();
}
/// @brief Returns whether the current file exists.
/// @return
bool ReadAccessor::IsExist() const noexcept
{
    if (false == reddDataMofN_.IsValid()) {
        isoftkv::PFileOpt *const pFileOpt{_GetMainFileOpt()};
        if (nullptr == pFileOpt) {
            return false;
        }
        return pFileOpt->IsOpen();
    }
    // Enter redundancy handling process
    std::function< ara::core::Result< bool >(isoftkv::PFileOpt const *const pFileOpt) > pfunWork;
    pfunWork = [](isoftkv::PFileOpt const *const pFileOpt) -> ara::core::Result< bool > {
        if (nullptr == pFileOpt) {
            return ara::core::Result< bool >::FromError(PerErrc::kFileNotFound);
        }
        return ara::core::Result< bool >::FromValue(pFileOpt->IsOpen());
    };
    ara::core::Result< bool > const result{_GetFileInfoByMofN< bool >(static_cast< uint64_t >(-1), pfunWork)};
    if (false == result.HasValue()) {
        return false;
    }
    return result.Value();
}
/// @brief Determines whether the file is open (non-AutoSar standard function)
/// @return
bool ReadAccessor::IsOpen() const noexcept
{
    if (false == reddDataMofN_.IsValid()) {
        isoftkv::PFileOpt *const pFileOpt{_GetMainFileOpt()};
        if (nullptr == pFileOpt) {
            return false;
        }
        return pFileOpt->IsOpen();
    }
    // Enter redundancy handling process
    std::function< ara::core::Result< bool >(isoftkv::PFileOpt const *const pFileOpt) > pfunWork;
    pfunWork = [](isoftkv::PFileOpt const *const pFileOpt) -> ara::core::Result< bool > {
        if (nullptr == pFileOpt) {
            return ara::core::Result< bool >::FromError(PerErrc::kFileNotFound);
        }
        return ara::core::Result< bool >::FromValue(pFileOpt->IsOpen());
    };
    ara::core::Result< bool > const resultGet{_GetFileInfoByMofN< bool >(static_cast< uint64_t >(-1), pfunWork)};
    if (false == resultGet.HasValue()) {
        return false;
    }
    return resultGet.Value();
}
/// @brief Returns the name of the file currently being operated on
/// @return
ara::core::StringView ReadAccessor::GetFileName() const noexcept
{
    isoftkv::PFileOpt *const pFileOpt{_GetMainFileOpt()};
    if (nullptr == pFileOpt) {
        return ara::core::StringView{};
    }
    return isoftkv::T_StringView(pFileOpt->GetFileName());
}
//********************************/
/// @brief M/N election algorithm for reading simple data types
/// @tparam T
/// @param bPeek
/// @return
/// @throws
template < typename T >
ara::core::Result< T > ReadAccessor::_ReadDataByMofN(bool const bPeek) const
{
    ara::core::Result< void > const check{_CheckReadReady()};
    if (false == check.HasValue()) {
        return isoftkv::T_ErrorResult< void, T >(check);
    }
    uint32_t const nDirtyData{0U};
    ara::core::Vector< ara::core::Result< T > > vecReadData;
    ara::core::Vector< uint32_t > vecHashID;
    isoftkv::PSelectMofN const selectMofN{static_cast< uint32_t >(reddDataMofN_.m),
                                          static_cast< uint32_t >(reddDataMofN_.n)};
    isoftkv::PReddCheckData_File reddCheckData;
    int32_t nSelectIndex{0};
    nSelectIndex = selectMofN.SelectGoal(
        static_cast< uint64_t >(nDirtyData),
        std::move([this, bPeek, &vecReadData, &vecHashID](int32_t const nReddIndex) -> uint64_t {
            uint32_t nHashID{nDirtyData};
            while (true) {
                isoftkv::PFileOpt *const pFileOpt{_GetFileOpt(nReddIndex)};
                if (nullptr == pFileOpt) {
                    vecReadData.push_back(ara::core::Result< T >::FromError(PerErrc::kFileNotFound));
                    break;
                }
                if (false == pFileOpt->IsOpen()) {
                    vecReadData.push_back(ara::core::Result< T >::FromError(PerErrc::kFileNotFound));
                    break;
                }
                int64_t const nPos{pFileOpt->GetPos()};
                if (nPos >= pFileOpt->GetFileSize()) {
                    vecReadData.push_back(ara::core::Result< T >::FromError(PerErrc::kIsEof));
                    break;
                }
                T data{std::move(static_cast< T >(0U))};
                int32_t const nReadLen{
                    pFileOpt->ReadData(isoftkv::T_TransBytes(&data), static_cast< uint32_t >(sizeof(data)))};
                if (nReadLen <= 0) {
                    vecReadData.push_back(ara::core::Result< T >::FromError(PerErrc::kInvalidPosition));
                    break;
                }
                // Calculate Hash value
                if (bPeek && (nReadLen > 0)) {
                    std::ignore
                        = pFileOpt->SeekPos(isoftkv::EFileSeekType::kSeekCur, -1 * static_cast< int64_t >(nReadLen));
                }
                vecReadData.push_back(ara::core::Result< T >::FromValue(data));
                nHashID = G_CalHashID(&data, sizeof(data), 0U);
                break;
            }
            vecHashID.push_back(nHashID);
            return static_cast< uint64_t >(nHashID);
        }));
    // Check for redundancy errors
    if (nSelectIndex < 0) {
        return ara::core::Result< T >::FromError(PerErrc::kValidationFailed);
    }
    reddCheckData.nSelectIndex = static_cast< uint8_t >(nSelectIndex);
    if (reddDataMofN_.n > 1) {
        isoftkv::PFileOpt *const pFileOpt{_GetFileOpt(reddCheckData.nSelectIndex)};
        reddCheckData.nPosStart = pFileOpt->GetPos();
        reddCheckData.nDataLen  = sizeof(T);
        for (std::size_t i{0}; i < vecHashID.size(); i++) {
            if ((i != reddCheckData.nSelectIndex) && (vecHashID[i] != vecHashID[reddCheckData.nSelectIndex])) {
                reddCheckData.vecNeedRecover.push_back(static_cast< uint8_t >(i));
            }
        }
        if (false == reddCheckData.vecNeedRecover.empty()) {
            checkReddFile_.AddReddCheck(reddCheckData);
        }
    }
    std::ignore = _AutoRecover();
    return T_GetVecResult(vecReadData, reddCheckData.nSelectIndex);
}
/// @brief Get file content via M/N redundancy strategy
/// @tparam T
/// @param nDirtyData
/// @param pfun
/// @return
/// @throws
template < typename T >
ara::core::Result< T > ReadAccessor::_ReadFileByMofN(
    uint64_t const nDirtyData,
    std::function< ara::core::Result< T >(isoftkv::PFileOpt const *const pFileOpt) > const pfun) const
{
    ara::core::Vector< ara::core::Result< T > > vecReadData;
    isoftkv::PSelectMofN const selectMofN{static_cast< uint32_t >(reddDataMofN_.m),
                                          static_cast< uint32_t >(reddDataMofN_.n)};
    isoftkv::PReddCheckData_File reddCheckData;
    ara::core::Vector< uint32_t > vecHashID;
    int32_t nSelectIndex{0};
    nSelectIndex = selectMofN.SelectGoal(
        nDirtyData, std::move([this, nDirtyData, pfun, &vecReadData, &vecHashID,
                               &reddCheckData](int32_t const nReddIndex) -> uint64_t {
            isoftkv::PFileOpt *const pFileOpt{_GetFileOpt(nReddIndex)};
            if (pFileOpt) {
                reddCheckData.nPosStart = pFileOpt->GetPos();
            }
            ara::core::Result< T > const result{std::move(pfun(pFileOpt))};
            vecReadData.push_back(result);
            if (false == result.HasValue()) {
                return nDirtyData;
            }
            T const &data{result.Value()};
            reddCheckData.nDataLen = static_cast< uint32_t >(data.size());
            const uint32_t nHashID{G_CalHashID(data.data(), static_cast< uint32_t >(data.size()), 0U)};
            vecHashID.push_back(nHashID);
            return static_cast< uint64_t >(nHashID);
        }));
    // Check for redundancy errors
    if (nSelectIndex < 0) {
        return ara::core::Result< T >::FromError(PerErrc::kValidationFailed);
    }
    reddCheckData.nSelectIndex = static_cast< uint8_t >(nSelectIndex);
    if (reddDataMofN_.n > 1) {
        for (std::size_t i{0}; i < vecHashID.size(); i++) {
            if ((i != reddCheckData.nSelectIndex) && (vecHashID[i] != vecHashID[reddCheckData.nSelectIndex])) {
                reddCheckData.vecNeedRecover.push_back(static_cast< uint8_t >(i));
            }
        }
        if (false == reddCheckData.vecNeedRecover.empty()) {
            checkReddFile_.AddReddCheck(reddCheckData);
        }
    }
    std::ignore = _AutoRecover();
    return T_GetVecResult(vecReadData, reddCheckData.nSelectIndex);
}
/// @brief Automatically repair errors using M/N redundancy
/// @return
ara::core::Result< void > ReadAccessor::_AutoRecover() const noexcept
{
    isoftkv::PReddCheckData_File reddCheckData{checkReddFile_.FindRecoverData()};
    ara::core::Vector< uint8_t > vecReddIndexTotal;
    int32_t nRecoverCount{0};
    int32_t nSuccessCount{0};
    while ((reddCheckData.nPosStart >= 0) && (reddCheckData.nDataLen > 0)) {
        // Execute page recovery operation
        nRecoverCount += 1;
        isoftkv::PAutoBuff autoBuff{reddCheckData.nDataLen};
        if (true == _ReddRecoverData_Read(reddCheckData.nSelectIndex, reddCheckData, autoBuff)) {
            uint32_t nWriteSuccess{0};
            for (auto &nReddIndex : reddCheckData.vecNeedRecover) {
                if (true == _ReddRecoverData_Write(nReddIndex, reddCheckData, autoBuff)) {
                    nWriteSuccess += 1;
                }
            }
            if (nWriteSuccess >= reddCheckData.vecNeedRecover.size()) {
                nSuccessCount += 1;
            }
        }
        checkReddFile_.DelReddCheck(reddCheckData.nPosStart, reddCheckData.nDataLen);
        reddCheckData = checkReddFile_.FindRecoverData();
    }

    if (IsHaveRecoverReport()) {
        if (nRecoverCount > 0) {
            ara::core::Vector< ara::core::String > vecFileName{};
            vecFileName.push_back(stFileName_);
            ara::core::InstanceSpecifier const storageIns{stStorageIns_};
            ara::per::RecoveryReportKind recoveryReportKind{ara::per::RecoveryReportKind::kKeyValueStorageRecovered};
            if (nSuccessCount < nRecoverCount) {
                recoveryReportKind = ara::per::RecoveryReportKind::kKeyValueStorageRecoveryFailed;
            }
            RecoveryReport(storageIns, recoveryReportKind, vecFileName, vecReddIndexTotal);
        }
    }
    return ara::core::Result< void >::FromValue();
}
/// @brief Get file information via M/N redundancy strategy: must be basic types that are all integers
/// @tparam T
/// @param nDirtyData
/// @param pfun
/// @return
/// @throws
template < typename T >
ara::core::Result< T > ReadAccessor::_GetFileInfoByMofN(
    uint64_t const nDirtyData,
    std::function< ara::core::Result< T >(isoftkv::PFileOpt const *const pFileOpt) > const pfun) const
{
    ara::core::Vector< ara::core::Result< T > > vecReadData;
    isoftkv::PSelectMofN const selectMofN{static_cast< uint32_t >(reddDataMofN_.m),
                                          static_cast< uint32_t >(reddDataMofN_.n)};
    int32_t nFindReddIndex{0};
    nFindReddIndex = selectMofN.SelectGoal(
        nDirtyData, std::move([this, &vecReadData, nDirtyData, pfun](int32_t const nReddIndex) -> uint64_t {
            isoftkv::PFileOpt *const pFileOpt{_GetFileOpt(nReddIndex)};
            ara::core::Result< T > const result{std::move(pfun(pFileOpt))};
            vecReadData.push_back(result);
            if (false == result.HasValue()) {
                return nDirtyData;
            }
            return static_cast< uint64_t >(result.Value());
        }));
    return T_GetVecResult(vecReadData, nFindReddIndex);
}
//********************************/
/// @brief Get the file operation cursor position
/// @return
ara::core::Result< uint64_t > ReadAccessor::_GetPosition() const noexcept
{
    ara::core::Result< uint64_t > resultReturn{0U};
    if (false == reddDataMofN_.IsValid()) {
        isoftkv::PFileOpt *const pFileOpt{_GetMainFileOpt()};
        if (nullptr == pFileOpt) {
            return resultReturn;
        }
        if (false == pFileOpt->IsOpen()) {
            return resultReturn;
        }
        resultReturn = ara::core::Result< uint64_t >::FromValue(static_cast< uint64_t >(pFileOpt->GetPos()));
    } else {
        // Enter redundancy handling process
        std::function< ara::core::Result< uint64_t >(isoftkv::PFileOpt const *const pFileOpt) > pfunWork;
        pfunWork = [](isoftkv::PFileOpt const *const pFileOpt) noexcept -> ara::core::Result< uint64_t > {
            if (nullptr == pFileOpt) {
                return ara::core::Result< uint64_t >::FromError(PerErrc::kFileNotFound);
            }
            if (false == pFileOpt->IsOpen()) {
                return ara::core::Result< uint64_t >::FromError(PerErrc::kFileNotFound);
            }
            return ara::core::Result< uint64_t >::FromValue(pFileOpt->GetPos());
        };
        resultReturn = _GetFileInfoByMofN< uint64_t >(static_cast< uint64_t >(-1), std::move(pfunWork));
    }
    return resultReturn;
}
/// @brief Read a string of a specified length
/// @param pFileOpt
/// @param nLen
/// @return
ara::core::Result< ara::core::String > ReadAccessor::_ReadString(isoftkv::PFileOpt const *const pFileOpt,
                                                                 uint64_t const nLen) const noexcept
{
    ara::core::Result< void > const check{G_CheckReadOpt(pFileOpt)};
    if (false == check.HasValue()) {
        return isoftkv::T_ErrorResult< void, ara::core::String >(check);
    }
    ara::core::String stData;
    int64_t nReadTotal{static_cast< int64_t >(nLen)};
    int64_t const nLeftLen{pFileOpt->GetFileSize() - pFileOpt->GetPos()};
    if ((nReadTotal < 0) || (nLen > static_cast< uint64_t >(nLeftLen))) {
        nReadTotal = nLeftLen;
    }
    uint8_t *const pWorkBuff{pAccessBuff_->GetWorkBuff()};
    int32_t const nWorkBuffLen{static_cast< int32_t >(pAccessBuff_->GetWorkBuffLen())};
    int64_t const nReadPos{pFileOpt->GetPos()};
    while (true) {
        if (nReadTotal <= 0) {
            break;
        }
        if (pFileOpt->IsEof()) {
            break;
        }
        std::ignore = memset(pWorkBuff, 0, static_cast< std::size_t >(nWorkBuffLen));
        int32_t nReadLen{nWorkBuffLen};
        if (static_cast< int64_t >(nReadLen) > nReadTotal) {
            nReadLen = static_cast< int32_t >(nReadTotal);
        }
        nReadLen = pFileOpt->ReadData(pWorkBuff, static_cast< uint32_t >(nReadLen));
        if (nReadLen <= 0) {
            std::ignore = pFileOpt->SeekPos(isoftkv::EFileSeekType::kSeekSet, nReadPos);
            return ara::core::Result< ara::core::String >::FromError(PerErrc::kInvalidPosition);
        }
        nReadTotal -= static_cast< int64_t >(nReadLen);
        std::ignore = stData.append(isoftkv::T_TransPtr< ara::core::String::value_type const >(pWorkBuff),
                                    static_cast< std::size_t >(nReadLen));
    }
    return ara::core::Result< ara::core::String >::FromValue(stData);
}
/// @brief Read a line of data
/// @param pFileOpt
/// @param delimiter
/// @return
ara::core::Result< ara::core::String > ReadAccessor::_ReadLine(isoftkv::PFileOpt const *const pFileOpt,
                                                               char8_t const delimiter) const noexcept
{
    ara::core::Result< void > const check{G_CheckReadOpt(pFileOpt)};
    if (false == check.HasValue()) {
        return isoftkv::T_ErrorResult< void, ara::core::String >(check);
    }
    ara::core::String stData;
    uint8_t *const pWorkBuff{pAccessBuff_->GetWorkBuff()};
    uint32_t nWorkBuffLen{0};
    nWorkBuffLen = pAccessBuff_->GetWorkBuffLen();
    while (false == pFileOpt->IsEof()) {
        std::ignore = memset(pWorkBuff, 0, static_cast< std::size_t >(nWorkBuffLen));
        int32_t nReadLen{0};
        nReadLen = pFileOpt->ReadData(isoftkv::T_TransBytes(pWorkBuff), nWorkBuffLen);
        if (nReadLen <= 0) {
            break;
        }
        int32_t nFindPos{-1};
        for (int32_t i{0}; i < nReadLen; i++) {
            if (*(pWorkBuff + i) == static_cast< uint8_t >(delimiter)) {
                nFindPos = i;
                break;
            }
        }
        // 2025-10-28 niuliming modified logic: The data returned by the ReadLine interface does not include the delimiter
        int32_t nAppendLen{nFindPos};
        if (nFindPos < 0) {
            nAppendLen = nReadLen;
        }
        std::ignore = stData.append(isoftkv::T_TransPtr< ara::core::String::value_type >(pWorkBuff),
                                    static_cast< std::size_t >(nAppendLen));
        if (nFindPos >= 0) {
            int32_t const nSeekLen{nReadLen - nFindPos - 1};
            if (nSeekLen > 0)  // Move cursor backward
            {
                std::ignore
                    = pFileOpt->SeekPos(isoftkv::EFileSeekType::kSeekCur, -1 * static_cast< int64_t >(nSeekLen));
            }
            break;
        }
    }
    return ara::core::Result< ara::core::String >::FromValue(stData);
}
/// @brief Read a segment of binary data
/// @param pFileOpt
/// @param nLen
/// @return
ara::core::Result< ara::core::Vector< ara::core::Byte > > ReadAccessor::_ReadBinary(
    isoftkv::PFileOpt const *const pFileOpt, uint64_t const nLen) const noexcept
{
    ara::core::Result< void > const check{G_CheckReadOpt(pFileOpt)};
    if (false == check.HasValue()) {
        return isoftkv::T_ErrorResult< void, ara::core::Vector< ara::core::Byte > >(check);
    }
    /// 2025-10-28 niuliming modified logic: Handle the case where nLen is -1 to read all remaining data
    int32_t nReadTotal{static_cast< int32_t >(pFileOpt->GetFileSize() - pFileOpt->GetPos())};
    if (static_cast< uint64_t >(-1) != nLen && static_cast< int32_t >(nLen) < nReadTotal) {
        nReadTotal = static_cast< int32_t >(nLen);
    }
    uint8_t *const pWorkBuff{pAccessBuff_->GetWorkBuff()};
    uint32_t const nWorkBuffLen{pAccessBuff_->GetWorkBuffLen()};
    ara::core::Vector< ara::core::Byte > vecData;
    vecData.reserve(static_cast< std::size_t >(nReadTotal));
    int64_t const nReadPos{pFileOpt->GetPos()};
    while (true) {
        if (nReadTotal <= 0) {
            break;
        }
        int32_t nReadLen{std::min< int32_t >(nReadTotal, static_cast< int32_t >(nWorkBuffLen))};
        nReadLen = pFileOpt->ReadData(pWorkBuff, static_cast< uint32_t >(nReadLen));
        if (nReadLen <= 0) {
            std::ignore = pFileOpt->SeekPos(isoftkv::EFileSeekType::kSeekSet, nReadPos);
            return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(PerErrc::kInvalidPosition);
        }
        nReadTotal -= nReadLen;
        for (int32_t i{0}; i < nReadLen; i++) {
            ara::core::Byte const byData{static_cast< ara::core::Byte >(*(pWorkBuff + i))};
            vecData.push_back(byData);
        }
    }
    return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromValue(std::move(vecData));
}
/// @brief Read data from redundant data for recovery
/// @param nPosStart Starting position of the file to read
/// @param nDataLen Length of data to read
/// @param autoBuff Automatic buffer to save the read content
/// @return Whether successful
bool ReadAccessor::_ReddRecoverData_Read(uint8_t nIndex,
                                         isoftkv::PReddCheckData_File const &reddCheckData,
                                         isoftkv::PAutoBuff &autoBuff) const noexcept
{
    ara::per::isoftkv::PFileOpt *pOptWork{_GetFileOpt(nIndex)};
    if (nullptr == pOptWork) {
        return false;
    }
    if (false == pOptWork->IsEnableRead()) {
        return false;
    }
    int64_t const nPosOld{pOptWork->GetPos()};
    if (false == pOptWork->SeekPos(isoftkv::EFileSeekType::kSeekSet, reddCheckData.nPosStart)) {
        return false;
    }
    int32_t const nReadLen{pOptWork->ReadData(autoBuff.data(), autoBuff.GetBuffLen())};
    pOptWork->SeekPos(isoftkv::EFileSeekType::kSeekSet, nPosOld);
    return nReadLen == static_cast< int32_t >(reddCheckData.nDataLen);
}
/// @brief Write data into redundant data for recovery
/// @param nIndex Redundant file index
/// @param reddCheckData Redundancy error recovery data
/// @param autoBuff Automatic buffer to save the read content
/// @return Whether successful
bool ReadAccessor::_ReddRecoverData_Write(uint8_t const nIndex,
                                          isoftkv::PReddCheckData_File const &reddCheckData,
                                          isoftkv::PAutoBuff const &autoBuff) const noexcept
{
    ara::per::isoftkv::PFileOpt *pOptWork{_GetFileOpt(nIndex)};
    if (nullptr == pOptWork) {
        return false;
    }
    int64_t const nPosOld{pOptWork->GetPos()};
    int32_t nWriteLen{0};
    if (pOptWork->IsEnableWrite()) {
        if (false == pOptWork->SeekPos(isoftkv::EFileSeekType::kSeekSet, reddCheckData.nPosStart)) {
            return false;
        }
        nWriteLen = pOptWork->WriteData(autoBuff.data(), autoBuff.GetBuffLen());
        if (false == pOptWork->SeekPos(isoftkv::EFileSeekType::kSeekSet, nPosOld)) {
            return false;
        }
    } else {
        ara::core::String stFileName{pOptWork->GetFileName()};
        uint32_t const nReadFlags{pOptWork->GetFileFlags()};
        if (false == pOptWork->CloseFile()) {
            return false;
        }
        std::unique_ptr< isoftkv::PFileOpt_Crypto > pOptWrite;
        pOptWrite = std::make_unique< isoftkv::PFileOpt_Crypto >(isoftkv::PFileOpt::ReadWrite());
        if (pOptWrite->DoPrepareWork(isoftkv::T_StringView(stFileName))) {
            if (this->_IsHaveCryptoEvent()) {
                if (false == pOptWrite->BuildCrypto(pConfigCrypto_->stKeySlotName, pConfigCrypto_->stCryptoAlgorithm)) {
                    return false;
                }
            }
            // Requires a writable PFileOpt to temporarily operate the file
            if (pOptWrite->IsEnableWrite()) {
                if (pOptWrite->SeekPos(isoftkv::EFileSeekType::kSeekSet, reddCheckData.nPosStart)) {
                    nWriteLen = pOptWrite->WriteData(autoBuff.data(), autoBuff.GetBuffLen());
                }
            }
            std::ignore = pOptWrite->CloseFile();
        }
        // Rebuild the operation pointers in the Opt list
        std::unique_ptr< isoftkv::PFileOpt_Crypto > pOptNewRead;
        pOptNewRead = std::make_unique< isoftkv::PFileOpt_Crypto >(nReadFlags);
        if (false == pOptNewRead->DoPrepareWork(isoftkv::T_StringView(stFileName))) {
            return false;
        }
        if (this->_IsHaveCryptoEvent()) {
            if (false == pOptWrite->BuildCrypto(pConfigCrypto_->stKeySlotName, pConfigCrypto_->stCryptoAlgorithm)) {
                return false;
            }
        }
        if (false == pOptNewRead->SeekPos(isoftkv::EFileSeekType::kSeekSet, nPosOld)) {
            return false;
        }
        vecReddFileOpt_[nIndex].release();
        vecReddFileOpt_[nIndex] = std::move(pOptNewRead);
    }
    return nWriteLen == static_cast< int32_t >(reddCheckData.nDataLen);
}
//********************************/
/// @brief Whether there is encryption event
/// @return
bool ReadAccessor::_IsHaveCryptoEvent() const noexcept
{
    if (nullptr == pConfigCrypto_) {
        return false;
    }
    return false == pConfigCrypto_->stKeySlotName.empty();
}
/// @brief
/// @return
bool ReadAccessor::_IsFileOptOpen() const noexcept
{
    if (false == reddDataMofN_.IsValid()) {
        return _GetMainFileOpt()->IsOpen();
    }
    CB_DealFileOpt_Const pfunWork;
    pfunWork = [](isoftkv::PFileOpt const *const) noexcept -> bool { return true; };
    int32_t const nOpenCount{_ForEachFileOpt(std::move(pfunWork))};
    return static_cast< uint16_t >(nOpenCount) >= reddDataMofN_.m;
}
/// @brief Iterate over all files
/// @param pfun
/// @return
int32_t ReadAccessor::_ForEachFileOpt(CB_DealFileOpt_Const const &pfun) const noexcept
{
    ara::core::Vector< isoftkv::PFileOpt * > vecFileOpt{_GetFileOptVec()};
    int32_t nOpenCount{0};
    int32_t *const pnOpenCount{&nOpenCount};
    CB_DealFileOpt_Const pfunWork;
    pfunWork = [pnOpenCount, pfun](isoftkv::PFileOpt const *const pFileOpt) noexcept -> bool {
        if (nullptr == pFileOpt) {
            return false;
        }
        if (false == pFileOpt->IsOpen()) {
            return false;
        }
        if (false == pfun(pFileOpt)) {
            return false;
        }
        *pnOpenCount += 1;
        return true;
    };
    std::ignore = std::for_each(vecFileOpt.begin(), vecFileOpt.end(), pfunWork);
    return nOpenCount;
}
/// @brief Get all FileOpt pointers
/// @return
ara::core::Vector< isoftkv::PFileOpt * > ReadAccessor::_GetFileOptVec() const noexcept
{
    ara::core::Vector< isoftkv::PFileOpt * > vecReturn;
    for (auto &it : vecReddFileOpt_) {
        vecReturn.push_back(it.get());
    }
    return vecReturn;
}
/// @brief Get the main file
/// @return
isoftkv::PFileOpt *ReadAccessor::_GetMainFileOpt() const noexcept { return _GetFileOpt(0); }
/// @brief Get the file operation object for a specific index
/// @param i
/// @return
isoftkv::PFileOpt *ReadAccessor::_GetFileOpt(int32_t const i) const noexcept
{
    std::size_t const nIndex{static_cast< std::size_t >(i)};
    if (nIndex >= vecReddFileOpt_.size()) {
        return nullptr;
    }
    return vecReddFileOpt_.at(nIndex).get();
}
/// @brief Get the minimum number of successful operations required
/// @return
int32_t ReadAccessor::_GetLeastSuccessCount() const noexcept
{
    if (reddDataMofN_.IsValid()) {
        return static_cast< int32_t >(reddDataMofN_.m);
    }
    return 1;
}
/// @brief Check if ready to read the file
/// @return
ara::core::Result< void > ReadAccessor::_CheckReadReady() const noexcept
{
    if (false == isoftkv::IsPerInitialize()) {
        return ara::core::Result< void >::FromError(PerErrc::kNotInitialized);
    }
    if (false == _IsFileOptOpen()) {
        return ara::core::Result< void >::FromError(PerErrc::kPhysicalStorageFailure);
    }
    return ara::core::Result< void >::FromValue();
}
//********************************/
}  // namespace per
}  // namespace ara
