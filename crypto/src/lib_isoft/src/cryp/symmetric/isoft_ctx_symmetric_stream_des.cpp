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
/// @file       isoft_ctx_symmetric_stream_des.cpp
/// @brief      AutoSar-Crypto Encryption/Decryption Module
/// @details
/// @date       2022-02-16
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Default Encryption/Symmetric Encryption
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_01004
/// @unit_name=PCtxSymmetricStream_Des_Base
/// @unit_description=Stream Symmetric Encryption Base Class Based on DES
/// @endcode
///
/// ================================================================

#include "ara/crypto/cryp/symmetric/isoft_ctx_symmetric_stream_des.h"

#include "ara/crypto/common/isoft_data_type.h"
#include "ara/crypto/cryp/cryobj/isoft_key_symmetric_des.h"
#include "ara/crypto/cryp/cryobj/isoft_key_symmetric_ipc.h"
#include "ara/crypto/cryp/cryobj/isoft_secret_seed.h"
#include "ara/crypto/cryp/isoft_service_stream.h"
#include "ara/crypto/openssl/isoft_openssl_encrypt.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
//********************************/
/// @brief Parameterized constructor
/// @param cryptoProvider Encryption provider
/// @qac Suppress warning [4268]: Initialization helper can be replaced with a delegator.
PCtxSymmetricStream_Des_Base::PCtxSymmetricStream_Des_Base(PCryptoProvider &cryptoProvider) noexcept  // NOLINT
    : PCtxSymmetricStream{cryptoProvider}
{
    _InitVector();
}
//***************/  //AUTOSAR-AP interface
/// @brief Check byte-oriented operational mode.
/// @name  IsBytewiseMode
/// @returns  true if byte mode false if block mode
bool PCtxSymmetricStream_Des_Base::IsBytewiseMode() const noexcept { return false; }
/// @brief Clear encryption context.
/// @name  Reset
/// @returns has value if reset sucess false otherwise
ara::core::Result< void > PCtxSymmetricStream_Des_Base::Reset() noexcept
{
    _InitVector();
#if PUHUA_OPENSSL_IS_11X
    std::ignore = memset(&desKeySchedule_, 0, sizeof(DES_key_schedule));
#else
    std::ignore = memset(desKeySchedule_, 0, kInt_8U);
#endif
    return PCtxSymmetricStream::Reset();
}
/// @brief Set (deploy) a key for the symmetric algorithm context.
/// @name  SetKey
/// @param key Symmetric key
/// @param transform Encryption direction: Encrypt or Decrypt
/// @returns  has value if SetKey sucess false otherwise
ara::core::Result< void > PCtxSymmetricStream_Des_Base::SetKey(SymmetricKey const &key,
                                                               CryptoTransform transform) noexcept
{
    /// @brief Result type alias
    using PResult = ara::core::Result< void >;
    PKeySymmetric_Ipc const *const pPKeySymmetricIpc{dynamic_cast< PKeySymmetric_Ipc const * >(&key)};
    if (pPKeySymmetricIpc != nullptr) {  // ipc
        return _SetKeyIpc(GetIpcFuncNameSetKey(), key, transform);
    }
    PResult pResult{PCtxSymmetricStream::SetKey(key, transform)};
    if (false == pResult.HasValue()) {
        return pResult;
    }
    PKeySymmetric_Des const *const pSymmetricKeyDes{dynamic_cast< PKeySymmetric_Des const * >(GetSymmetricKey())};
#if PUHUA_OPENSSL_IS_11X
    const_DES_cblock *const pDesKey{
        static_cast< const_DES_cblock * >(static_cast< void * >(pSymmetricKeyDes->GetKeySymmetric()))};
    DES_set_key_unchecked(pDesKey, &desKeySchedule_);
#else
    memcpy(desKeySchedule_, pSymmetricKeyDes->GetKeySymmetric(), kInt_8U);
#endif
    return PResult::FromValue();
}
//***************/  //PServiceStream interface
/// @brief Get the maximum supported key length in bits.
/// @name  GetMaxKeyBitLength
/// @returns  maximal supported length of the key in bits
std::size_t PCtxSymmetricStream_Des_Base::GetMaxKeyBitLength() const noexcept
{
    return static_cast< std::size_t >(PAlgId_Symmetric_DesKey::EKeyLen::kDesKeyMinBitLength);
}
/// @brief Get the minimum supported key length in bits.
/// @name  GetMinKeyBitLength
/// @returns  minimal supported length of the key in bits
std::size_t PCtxSymmetricStream_Des_Base::GetMinKeyBitLength() const noexcept
{
    return static_cast< std::size_t >(PAlgId_Symmetric_DesKey::EKeyLen::kDesKeyMinBitLength);
}
/// @brief Get the block (or internal buffer) size of the underlying algorithm.
/// @name  GetBlockSize
/// @returns  Block (or internal buffer) size of the underlying algorithm
std::size_t PCtxSymmetricStream_Des_Base::GetBlockSize() const noexcept
{
    return static_cast< std::size_t >(PAlgId_Symmetric_DesKey::EKeyLen::kDesBlockSize);
}
//***************/  //PCtxSymmetricStream interface
/// @brief Check if Key meets requirements
/// @name  CheckKey
/// @param key Symmetric key
/// @returns  true if check key sucess false otherwise
bool PCtxSymmetricStream_Des_Base::CheckKey(SymmetricKey const &key) const noexcept
{
    PAlgId_Symmetric_DesKey cryptoKey;
    // 2022-01-21 Accept key type: PAlgId_Symmetric_DesKey
    return cryptoKey.IsMinePrimitiveId(key.GetCryptoPrimitiveId()->GetPrimitiveId());
}
/// @brief Execute initialization operation: via secret seed
/// @name  DoInit
/// @param iv Initialization secret seed
void PCtxSymmetricStream_Des_Base::DoInit(SecretSeed const &iv) noexcept
{
    PSecretSeed const *const pSecretSeed{dynamic_cast< PSecretSeed const * >(&iv)};
    if (pSecretSeed == nullptr) {
        return;
    }
    _SetCryptoObjectUid(pSecretSeed->GetObjectId().mCouid);
    uint8_t const *const pData{pSecretSeed->GetSecretSeedData()};
    size_t const seedLen{pSecretSeed->GetPayloadSize()};
    std::size_t const nCopyLen{std::min< std::size_t >(seedLen, sizeof(DES_cblock))};
    for (std::size_t i{0U}; i < nCopyLen; i++) {
        initVector_[i] = *(static_cast< unsigned char const * >(pData) + i);
    }
}
/// @brief Execute initialization operation: via initialization memory
/// @name  DoInit
/// @param iv Initialization vector
void PCtxSymmetricStream_Des_Base::DoInit(ReadOnlyMemRegion const &iv) noexcept
{
    _SetCryptoObjectUid(CryptoObjectUid());
    std::size_t const nCopyLen{std::min< std::size_t >(iv.size(), sizeof(DES_cblock))};
    for (std::size_t i{0U}; i < nCopyLen; i++) {
        initVector_[i] = iv[i];
    }
}
/// @brief Initialize initVector_
/// @name  _InitVector
void PCtxSymmetricStream_Des_Base::_InitVector() noexcept
{
    initVector_[0] = kInt8_0U;
    InitVector(static_cast< uint8_t * >(initVector_), static_cast< int32_t >(sizeof(initVector_)));
}
//***************/ //PServiceStream interface
/// @brief Get the actual bit length of the IV loaded into the context.
/// @name  GetActualIvBitLength
/// @returns  std::size_t
/// @throws
/// @param ivUid UID of the Initialization Vector (IV)
/// @return actual length of the IV (now set to the algorithm context) in bits
std::size_t PCtxSymmetricStream_Des_Base::GetActualIvBitLength(
    ara::core::Optional< CryptoObjectUid > &ivUid) const noexcept
{
    if (ivUid.has_value()) {
    }
    ara::core::Optional< CryptoObjectUid > const rhs{_GetCryptoObjectUid()};
    ivUid = rhs;
    if (false == IsStarted()) {
        return 0U;
    }
    return (sizeof(initVector_)) * kInt_8U;
}
/// @brief Get the default expected size of the Initialization Vector (IV) or nonce.
/// @name  GetIvSize
/// @returns default expected size of IV in bytes
std::size_t PCtxSymmetricStream_Des_Base::GetIvSize() const noexcept { return sizeof(DES_cblock); }
/// @brief Verify the validity of a specific Initialization Vector (IV) length.
/// @name  IsValidIvSize
/// @param ivSize Initialization vector length
/// @returns   @c true if provided IV length is supported by the algorithm and @c false otherwise
bool PCtxSymmetricStream_Des_Base::IsValidIvSize(std::size_t ivSize) const noexcept
{
    return ivSize >= sizeof(DES_cblock);
}
//********************************/
/// @brief Return a CryptoPrimitivId instance containing the instance identifier.
/// @name  GetCryptoPrimitiveId
/// @returns  CryptoPrimitivId instance
CryptoPrimitiveId::Uptr PCtxSymmetricStream_Des_Cfb::GetCryptoPrimitiveId() const noexcept
{
    return {std::make_unique< PAlgId_Symmetric_Des_Cfb >()};
}
/// @brief Get BlockService instance.
/// @name  GetBlockService
/// @returns BlockService instance
BlockService::Uptr PCtxSymmetricStream_Des_Cfb::GetBlockService() const noexcept
{
    return {std::make_unique< PServiceStream< PCtxSymmetricStream_Des_Cfb > >(*this)};
}
/// @brief Calculate encryption/decryption, save result in buffOutput, return ciphertext length
/// @name  DoCipherLocal
/// @param buffOutput Output buffer
/// @param pInputData Start address of input data in algorithm operation
/// @param nDataLen Data length
/// @param alignedData Whether data is aligned
/// @returns  Ciphertext length
uint32_t PCtxSymmetricStream_Des_Cfb::DoCipherLocal(internal::PAutoBuff &buffOutput,
                                                    uint8_t const *pInputData,
                                                    uint32_t nDataLen,
                                                    bool alignedData) noexcept
{
    std::ignore = alignedData;
    PKeySymmetric_Ipc const *const pPKeySymmetricIpc{dynamic_cast< PKeySymmetric_Ipc const * >(GetSymmetricKey())};
    if (pPKeySymmetricIpc != nullptr) {  // ipc
        return _DoCipherIpc(GetIpcFuncNameDoCipher(), buffOutput, pInputData, nDataLen);
    }
    buffOutput.ResetData();

    uint8_t const *const pInput{pInputData};
    uint8_t *const pOutput{buffOutput.Data(0U)};
    int32_t const nDesTransform{_IsTransformEnc() ? DES_ENCRYPT : DES_DECRYPT};

#if PUHUA_OPENSSL_IS_11X
    DES_key_schedule *const pDesKey{&desKeySchedule_};
    DES_cfb_encrypt(pInput, pOutput, kInt_8, static_cast< int64_t >(nDataLen), pDesKey, &initVector_, nDesTransform);
#else
    DES_cfb_encrypt_evp(pInput, pOutput, kInt_8, static_cast< int64_t >(nDataLen), desKeySchedule_, initVector_,
                        nDesTransform);
#endif
    buffOutput.SetDataSize(nDataLen);
    return nDataLen;
}
/// @brief Get IPC function name corresponding to encryption/decryption
/// @name    GetIpcFuncNameDoCipher
/// @returns IPC function name corresponding to encryption/decryption
ara::core::StringView PCtxSymmetricStream_Des_Cfb::GetIpcFuncNameDoCipher() const noexcept
{
    return FUNC_NAME_Symmetric(DoCipher_Des_Cfb_Stream);
}
/// @brief Check byte-oriented operational mode.
/// @name  IsBytewiseMode
/// @returns  true if byte mode false if block mode
bool PCtxSymmetricStream_Des_Cfb::IsBytewiseMode() const noexcept { return true; }
//********************************/
/// @brief Return a CryptoPrimitivId instance containing the instance identifier.
/// @name  GetCryptoPrimitiveId
/// @returns  CryptoPrimitivId instance
CryptoPrimitiveId::Uptr PCtxSymmetricStream_Des_Cfb64::GetCryptoPrimitiveId() const noexcept
{
    return {std::make_unique< PAlgId_Symmetric_Des_Cfb64 >()};
}
/// @brief Get BlockService instance.
/// @name  GetBlockService
/// @returns  BlockService instance
BlockService::Uptr PCtxSymmetricStream_Des_Cfb64::GetBlockService() const noexcept
{
    return {std::make_unique< PServiceStream< PCtxSymmetricStream_Des_Cfb64 > >(*this)};
}
/// @brief Calculate encryption/decryption, save result in buffOutput, return ciphertext length
/// @name  DoCipherLocal
/// @param buffOutput Output buffer
/// @param pInputData Start address of input data in algorithm operation
/// @param nDataLen Data length
/// @param alignedData Whether data is aligned
/// @returns  Ciphertext length
uint32_t PCtxSymmetricStream_Des_Cfb64::DoCipherLocal(internal::PAutoBuff &buffOutput,
                                                      uint8_t const *pInputData,
                                                      uint32_t nDataLen,
                                                      bool alignedData) noexcept
{
    std::ignore = alignedData;
    PKeySymmetric_Ipc const *const pPKeySymmetricIpc{dynamic_cast< PKeySymmetric_Ipc const * >(GetSymmetricKey())};
    if (pPKeySymmetricIpc != nullptr) {  // ipc
        return _DoCipherIpc(GetIpcFuncNameDoCipher(), buffOutput, pInputData, nDataLen);
    }
    buffOutput.ResetData();
    uint8_t const *const pInput{T_TransBytes(pInputData)};
    uint8_t *const pOutput{buffOutput.Data(0U)};
    int32_t num{0};

    int32_t const nDesTransform{_IsTransformEnc() ? DES_ENCRYPT : DES_DECRYPT};
#if PUHUA_OPENSSL_IS_11X
    DES_key_schedule *const pDesKey{&desKeySchedule_};
    DES_cfb64_encrypt(pInput, pOutput, static_cast< int64_t >(nDataLen), pDesKey, &initVector_, &num, nDesTransform);
#else
    DES_cfb64_encrypt_evp(pInput, pOutput, static_cast< int64_t >(nDataLen), desKeySchedule_, initVector_, &num,
                          nDesTransform);
#endif
    if (num == 0) {
    }
    buffOutput.SetDataSize(nDataLen);
    return nDataLen;
}
/// @brief Get IPC function name corresponding to encryption/decryption
/// @name    GetIpcFuncNameDoCipher
/// @returns IPC function name corresponding to encryption/decryption
ara::core::StringView PCtxSymmetricStream_Des_Cfb64::GetIpcFuncNameDoCipher() const noexcept
{
    return FUNC_NAME_Symmetric(DoCipher_Des_Cfb64_Stream);
}
//********************************/
/// @brief Return a CryptoPrimitivId instance containing the instance identifier.
/// @name  GetCryptoPrimitiveId
/// @returns  CryptoPrimitivId instance
CryptoPrimitiveId::Uptr PCtxSymmetricStream_Des_Ofb::GetCryptoPrimitiveId() const noexcept
{
    return {std::make_unique< PAlgId_Symmetric_Des_Ofb >()};
}
/// @brief Get BlockService instance.
/// @name  GetBlockService
/// @returns  BlockService instance
BlockService::Uptr PCtxSymmetricStream_Des_Ofb::GetBlockService() const noexcept
{
    return {std::make_unique< PServiceStream< PCtxSymmetricStream_Des_Ofb > >(*this)};
}
/// @brief Calculate encryption/decryption, save result in buffOutput, return ciphertext length
/// @name  DoCipherLocal
/// @param buffOutput Output buffer
/// @param pInputData Start address of input data in algorithm operation
/// @param nDataLen Data length
/// @param alignedData Whether data is aligned
/// @returns  Ciphertext length
uint32_t PCtxSymmetricStream_Des_Ofb::DoCipherLocal(internal::PAutoBuff &buffOutput,
                                                    uint8_t const *pInputData,
                                                    uint32_t nDataLen,
                                                    bool alignedData) noexcept
{
    std::ignore = alignedData;
    PKeySymmetric_Ipc const *const pPKeySymmetricIpc{dynamic_cast< PKeySymmetric_Ipc const * >(GetSymmetricKey())};
    if (pPKeySymmetricIpc != nullptr) {  // ipc
        return _DoCipherIpc(GetIpcFuncNameDoCipher(), buffOutput, pInputData, nDataLen);
    }
    buffOutput.ResetData();

    uint8_t const *const pInput{T_TransBytes(pInputData)};
    uint8_t *const pOutput{buffOutput.Data(0U)};

#if PUHUA_OPENSSL_IS_11X
    DES_key_schedule *const pDesKey{&desKeySchedule_};
    DES_ofb_encrypt(pInput, pOutput, kInt_8, static_cast< int64_t >(nDataLen), pDesKey, &initVector_);
#else
    DES_ofb_encrypt_evp(pInput, pOutput, kInt_8, static_cast< int64_t >(nDataLen), desKeySchedule_, initVector_);
#endif
    buffOutput.SetDataSize(nDataLen);
    return nDataLen;
}
/// @brief Get IPC function name corresponding to encryption/decryption
/// @name    GetIpcFuncNameDoCipher
/// @returns IPC function name corresponding to encryption/decryption
ara::core::StringView PCtxSymmetricStream_Des_Ofb::GetIpcFuncNameDoCipher() const noexcept
{
    return FUNC_NAME_Symmetric(DoCipher_Des_Ofb_Stream);
}
/// @brief Check byte-oriented operational mode.
/// @name  IsBytewiseMode
/// @returns  true if byte mode false if block mode
bool PCtxSymmetricStream_Des_Ofb::IsBytewiseMode() const noexcept { return true; }
//********************************/
/// @brief Return a CryptoPrimitivId instance containing the instance identifier.
/// @name  GetCryptoPrimitiveId
/// @returns  CryptoPrimitivId instance
CryptoPrimitiveId::Uptr PCtxSymmetricStream_Des_Ofb64::GetCryptoPrimitiveId() const noexcept
{
    return {std::make_unique< PAlgId_Symmetric_Des_Ofb64 >()};
}
/// @brief Get BlockService instance.
/// @name  GetBlockService
/// @returns  BlockService instance
BlockService::Uptr PCtxSymmetricStream_Des_Ofb64::GetBlockService() const noexcept
{
    return {std::make_unique< PServiceStream< PCtxSymmetricStream_Des_Ofb64 > >(*this)};
}
/// @brief Calculate encryption/decryption, save result in buffOutput, return ciphertext length
/// @name  DoCipherLocal
/// @param buffOutput Output buffer
/// @param pInputData Start address of input data in algorithm operation
/// @param nDataLen Data length
/// @param alignedData Whether data is aligned
/// @returns  Ciphertext length
uint32_t PCtxSymmetricStream_Des_Ofb64::DoCipherLocal(internal::PAutoBuff &buffOutput,
                                                      uint8_t const *pInputData,
                                                      uint32_t nDataLen,
                                                      bool alignedData) noexcept
{
    std::ignore = alignedData;
    PKeySymmetric_Ipc const *const pPKeySymmetricIpc{dynamic_cast< PKeySymmetric_Ipc const * >(GetSymmetricKey())};
    if (pPKeySymmetricIpc != nullptr) {  // ipc
        return _DoCipherIpc(GetIpcFuncNameDoCipher(), buffOutput, pInputData, nDataLen);
    }
    buffOutput.ResetData();

    uint8_t const *const pInput{T_TransBytes(pInputData)};
    uint8_t *const pOutput{buffOutput.Data(0U)};
    int32_t num{0};

#if PUHUA_OPENSSL_IS_11X
    DES_key_schedule *const pDesKey{&desKeySchedule_};
    DES_ofb64_encrypt(pInput, pOutput, static_cast< int64_t >(nDataLen), pDesKey, &initVector_, &num);
#else
    DES_ofb64_encrypt_evp(pInput, pOutput, static_cast< int64_t >(nDataLen), desKeySchedule_, initVector_, &num);
#endif
    if (num == 0) {
    }
    buffOutput.SetDataSize(nDataLen);
    return nDataLen;
}
/// @brief Get IPC function name corresponding to encryption/decryption
/// @name    GetIpcFuncNameDoCipher
/// @returns IPC function name corresponding to encryption/decryption
ara::core::StringView PCtxSymmetricStream_Des_Ofb64::GetIpcFuncNameDoCipher() const noexcept
{
    return FUNC_NAME_Symmetric(DoCipher_Des_Ofb64_Stream);
}
//********************************/
//***************/  //AUTOSAR-AP interface
/// @brief Clear encryption context.
/// @name  Reset
/// @returns  ara::core::Result<void>
ara::core::Result< void > PCtxSymmetricStream_3Des_Base::Reset() noexcept
{
#if PUHUA_OPENSSL_IS_11X
    std::ignore = memset(&desKeySchedule2_, 0, sizeof(DES_key_schedule));
    std::ignore = memset(&desKeySchedule3_, 0, sizeof(DES_key_schedule));
#else
    std::ignore = memset(desKeySchedule2_, 0, kInt_8U);
    std::ignore = memset(desKeySchedule3_, 0, kInt_8U);
#endif
    return PCtxSymmetricStream_Des_Base::Reset();
}
/// @brief Set (deploy) a key for the symmetric algorithm context.
/// @name  SetKey
/// @param key Symmetric key
/// @param transform Encryption direction: Encrypt or Decrypt
/// @returns  has value if SetKey sucess false otherwise
ara::core::Result< void > PCtxSymmetricStream_3Des_Base::SetKey(SymmetricKey const &key,
                                                                CryptoTransform transform) noexcept
{
    /// @brief Result type alias
    using PResult = ara::core::Result< void >;
    PKeySymmetric_Ipc const *const pPKeySymmetricIpc{dynamic_cast< PKeySymmetric_Ipc const * >(&key)};
    if (pPKeySymmetricIpc != nullptr) {  // ipc
        return _SetKeyIpc(GetIpcFuncNameSetKey(), key, transform);
    }
    PResult pResult{PCtxSymmetricStream_Des_Base::SetKey(key, transform)};
    if (false == pResult.HasValue()) {
        return pResult;
    }
    PKeySymmetric_Des const *const pSymmetricKeyDes{dynamic_cast< PKeySymmetric_Des const * >(GetSymmetricKey())};
#if PUHUA_OPENSSL_IS_11X
    const_DES_cblock *const pDesKey{T_TransPtr< const_DES_cblock >(pSymmetricKeyDes->GetKeySymmetric())};

    DES_key_schedule *const pDesKeySchedule{&desKeySchedule_};
    DES_set_key_unchecked(pDesKey, pDesKeySchedule);
    std::ignore = memcpy(&desKeySchedule2_, pDesKeySchedule, sizeof(DES_key_schedule));
    std::ignore = memcpy(&desKeySchedule3_, pDesKeySchedule, sizeof(DES_key_schedule));
    if (pSymmetricKeyDes->GetPayloadSize() >= (kInt_3U * sizeof(const_DES_cblock))) {
        DES_set_key_unchecked(pDesKey + kInt_2U, &desKeySchedule3_);
    }
    if (pSymmetricKeyDes->GetPayloadSize() >= (kInt_2U * sizeof(const_DES_cblock))) {
        DES_set_key_unchecked(pDesKey + 1, &desKeySchedule2_);
    }
#else
    uint8_t *const pDesKey{(pSymmetricKeyDes->GetKeySymmetric())};
    memcpy(desKeySchedule_, pDesKey, kInt_8U);
    memcpy(desKeySchedule2_, pDesKey, kInt_8U);
    memcpy(desKeySchedule3_, pDesKey, kInt_8U);

    if (pSymmetricKeyDes->GetPayloadSize() >= (kInt_3U * GetBlockSize())) {
        memcpy(desKeySchedule3_, desKeySchedule_ + kInt_2U * GetBlockSize(), kInt_8U);
    }
    if (pSymmetricKeyDes->GetPayloadSize() >= (kInt_2U * GetBlockSize())) {
        memcpy(desKeySchedule2_, desKeySchedule_ + 1 * GetBlockSize(), kInt_8U);
    }
#endif
    return PResult::FromValue();
}
/// @brief Get the maximum supported key length in bits.
/// @name  GetMaxKeyBitLength
/// @returns  maximal supported length of the key in bits
std::size_t PCtxSymmetricStream_3Des_Base::GetMaxKeyBitLength() const noexcept
{
    return static_cast< std::size_t >(PAlgId_Symmetric_DesKey::EKeyLen::kDesKeyMaxBitLength);
}
/// @brief Get the minimum supported key length in bits.
/// @name  GetMinKeyBitLength
/// @returns  minimal supported length of the key in bits
std::size_t PCtxSymmetricStream_3Des_Base::GetMinKeyBitLength() const noexcept
{
    return static_cast< std::size_t >(PAlgId_Symmetric_DesKey::EKeyLen::kDesKeyMinBitLength);
}
/// @brief Verify support for a specific key length based on the context.
/// @name  IsKeyBitLengthSupported
/// @param keyBitLength Key length: in bits
/// @returns  @c true if provided value of the key length is supported by the context
bool PCtxSymmetricStream_3Des_Base::IsKeyBitLengthSupported(std::size_t keyBitLength) const noexcept
{
    if ((keyBitLength < GetMinKeyBitLength()) || (keyBitLength > GetMaxKeyBitLength())) {
        return false;
    }
    return 0U == (keyBitLength % static_cast< std::size_t >(PAlgId_Symmetric_DesKey::EKeyLen::kDesKeyMinBitLength));
}
//***************/
/// @brief Check if Key meets requirements
/// @name  CheckKey
/// @param key Symmetric key
/// @returns true if check key sucess false otherwise
bool PCtxSymmetricStream_3Des_Base::CheckKey(SymmetricKey const &key) const noexcept
{
    // 2022-03-22 Accept key type: PAlgId_Symmetric_DesKey/PAlgId_Symmetric_3DesKey
    AlgId const nAlgID{key.GetCryptoPrimitiveId()->GetPrimitiveId()};
    PAlgId_Symmetric_DesKey cryptoKey;
    PAlgId_Symmetric_3DesKey key3Des;
    if (cryptoKey.IsMinePrimitiveId(nAlgID)) {
        return true;
    }
    if (key3Des.IsMinePrimitiveId(nAlgID)) {
        return true;
    }
    return false;
}
//********************************/
//***************/ //AUTOSAR-AP interface
/// @brief Return a CryptoPrimitivId instance containing the instance identifier.
/// @name  GetCryptoPrimitiveId
/// @returns  CryptoPrimitivId instance
CryptoPrimitiveId::Uptr PCtxSymmetricStream_3Des_Cfb1::GetCryptoPrimitiveId() const noexcept
{
    return {std::make_unique< PAlgId_Symmetric_3Des_Cfb1 >()};
}
/// @brief Get BlockService instance.
/// @name  GetBlockService
/// @returns  BlockService instance
BlockService::Uptr PCtxSymmetricStream_3Des_Cfb1::GetBlockService() const noexcept
{
    return {std::make_unique< PServiceStream< PCtxSymmetricStream_3Des_Cfb1 > >(*this)};
}
//***************/  //PCtxSymmetricStream interface
/// @brief Calculate encryption/decryption, save result in buffOutput_, return ciphertext length
/// @name  DoCipherLocal
/// @param buffOutput Output buffer
/// @param pInputData Start address of input data in algorithm operation
/// @param nDataLen Data length
/// @param alignedData Whether data is aligned
/// @return Ciphertext length
uint32_t PCtxSymmetricStream_3Des_Cfb1::DoCipherLocal(internal::PAutoBuff &buffOutput,
                                                      uint8_t const *pInputData,
                                                      uint32_t nDataLen,
                                                      bool alignedData) noexcept
{
    std::ignore = alignedData;
    buffOutput.ResetData();
    PKeySymmetric_Ipc const *const pPKeySymmetricIpc{dynamic_cast< PKeySymmetric_Ipc const * >(GetSymmetricKey())};
    if (pPKeySymmetricIpc != nullptr) {  // ipc
        return _DoCipherIpc(GetIpcFuncNameDoCipher(), buffOutput, pInputData, nDataLen);
    }

    /// Ensure execution reaches here: eCryptoTransform_ has only two possible values (CryptoTransform::kEncrypt/CryptoTransform::kDecrypt)
    int32_t const nDesTransform{_IsTransformEnc() ? DES_ENCRYPT : DES_DECRYPT};

    uint8_t *const pOutput{buffOutput.Data(0U)};

#if PUHUA_OPENSSL_IS_11X
    DES_key_schedule *const pDesKey1{&desKeySchedule_};
    DES_key_schedule *const pDesKey2{&desKeySchedule2_};
    DES_key_schedule *const pDesKey3{&desKeySchedule3_};
    DES_ede3_cfb_encrypt(pInputData, pOutput, kInt_8, static_cast< int64_t >(nDataLen), pDesKey1, pDesKey2, pDesKey3,
                         &initVector_, nDesTransform);
#else
    DES_ede3_cfb_encrypt_evp(pInputData, pOutput, kInt_8, static_cast< int64_t >(nDataLen), desKeySchedule_,
                             desKeySchedule2_, desKeySchedule3_, initVector_, nDesTransform);
#endif
    buffOutput.SetDataSize(nDataLen);
    return nDataLen;
}
/// @brief Get IPC function name corresponding to encryption/decryption
/// @name    GetIpcFuncNameDoCipher
/// @returns IPC function name corresponding to encryption/decryption
ara::core::StringView PCtxSymmetricStream_3Des_Cfb1::GetIpcFuncNameDoCipher() const noexcept
{
    return FUNC_NAME_Symmetric(DoCipher_3Des_Cfb1_Stream);
}
/// @brief Check byte-oriented operational mode.
/// @name  IsBytewiseMode
/// @returns  true if byte mode false if block mode
bool PCtxSymmetricStream_3Des_Cfb1::IsBytewiseMode() const noexcept { return true; }
//********************************/
//***************/ //AUTOSAR-AP interface
/// @brief Return a CryptoPrimitivId instance containing the instance identifier.
/// @name  GetCryptoPrimitiveId
/// @returns CryptoPrimitivId instance
CryptoPrimitiveId::Uptr PCtxSymmetricStream_3Des_Cfb64::GetCryptoPrimitiveId() const noexcept
{
    return {std::make_unique< PAlgId_Symmetric_3Des_Cfb64 >()};
}
/// @brief Get BlockService instance.
/// @name  GetBlockService
/// @returns BlockService instance
BlockService::Uptr PCtxSymmetricStream_3Des_Cfb64::GetBlockService() const noexcept
{
    return {std::make_unique< PServiceStream< PCtxSymmetricStream_3Des_Cfb64 > >(*this)};
}
//***************/  //AUTOSAR-AP interface
//***************/  //PCtxSymmetricStream interface
/// @brief Calculate encryption/decryption, save result in buffOutput_, return ciphertext length
/// @name  DoCipherLocal
/// @param buffOutput Output buffer
/// @param pInputData Start address of input data in algorithm operation
/// @param nDataLen Data length
/// @param alignedData Whether data is aligned
/// @return Ciphertext length
uint32_t PCtxSymmetricStream_3Des_Cfb64::DoCipherLocal(internal::PAutoBuff &buffOutput,
                                                       uint8_t const *pInputData,
                                                       uint32_t nDataLen,
                                                       bool alignedData) noexcept
{
    std::ignore = alignedData;
    buffOutput.ResetData();
    PKeySymmetric_Ipc const *const pPKeySymmetricIpc{dynamic_cast< PKeySymmetric_Ipc const * >(GetSymmetricKey())};
    if (pPKeySymmetricIpc != nullptr) {  // ipc
        return _DoCipherIpc(GetIpcFuncNameDoCipher(), buffOutput, pInputData, nDataLen);
    }
    /// Ensure execution reaches here: eCryptoTransform_ has only two possible values (CryptoTransform::kEncrypt/CryptoTransform::kDecrypt)
    int32_t const nDesTransform{_IsTransformEnc() ? DES_ENCRYPT : DES_DECRYPT};

    uint8_t *const pOutput{buffOutput.Data(0U)};
    int32_t nNum{0};

#if PUHUA_OPENSSL_IS_11X
    DES_key_schedule *const pDesKey1{&desKeySchedule_};
    DES_key_schedule *const pDesKey2{&desKeySchedule2_};
    DES_key_schedule *const pDesKey3{&desKeySchedule3_};
    DES_ede3_cfb64_encrypt(pInputData, pOutput, static_cast< int64_t >(nDataLen), pDesKey1, pDesKey2, pDesKey3,
                           &initVector_, &nNum, nDesTransform);
#else
    DES_ede3_cfb64_encrypt_evp(pInputData, pOutput, static_cast< int64_t >(nDataLen), desKeySchedule_, desKeySchedule2_,
                               desKeySchedule3_, initVector_, &nNum, nDesTransform);
#endif
    if (nNum == 0) {
    }  // for qac
    buffOutput.SetDataSize(nDataLen);
    return nDataLen;
}
/// @brief Get IPC function name corresponding to encryption/decryption
/// @name    GetIpcFuncNameDoCipher
/// @returns IPC function name corresponding to encryption/decryption
ara::core::StringView PCtxSymmetricStream_3Des_Cfb64::GetIpcFuncNameDoCipher() const noexcept
{
    return FUNC_NAME_Symmetric(DoCipher_3Des_Cfb64_Stream);
}
//********************************/
//***************/ //AUTOSAR-AP interface
/// @brief Return a CryptoPrimitivId instance containing the instance identifier.
/// @name  GetCryptoPrimitiveId
/// @returns  CryptoPrimitivId instance
CryptoPrimitiveId::Uptr PCtxSymmetricStream_3Des_Ofb64::GetCryptoPrimitiveId() const noexcept
{
    return {std::make_unique< PAlgId_Symmetric_3Des_Ofb64 >()};
}
/// @brief Get BlockService instance.
/// @name  GetBlockService
/// @returns  BlockService instance
BlockService::Uptr PCtxSymmetricStream_3Des_Ofb64::GetBlockService() const noexcept
{
    return {std::make_unique< PServiceStream< PCtxSymmetricStream_3Des_Ofb64 > >(*this)};
}
//***************/  //AUTOSAR-AP interface
//***************/  //PCtxSymmetricStream interface
/// @brief Calculate encryption/decryption, save result in buffOutput_, return ciphertext length
/// @name  DoCipherLocal
/// @param buffOutput Output buffer
/// @param pInputData Start address of input data in algorithm operation
/// @param nDataLen Data length
/// @param alignedData Whether data is aligned
/// @return  Ciphertext length
uint32_t PCtxSymmetricStream_3Des_Ofb64::DoCipherLocal(internal::PAutoBuff &buffOutput,
                                                       uint8_t const *pInputData,
                                                       uint32_t nDataLen,
                                                       bool alignedData) noexcept
{
    std::ignore = alignedData;
    buffOutput.ResetData();
    PKeySymmetric_Ipc const *const pPKeySymmetricIpc{dynamic_cast< PKeySymmetric_Ipc const * >(GetSymmetricKey())};
    if (pPKeySymmetricIpc != nullptr) {  // ipc
        return _DoCipherIpc(GetIpcFuncNameDoCipher(), buffOutput, pInputData, nDataLen);
    }

    /// Ensure execution reaches here: eCryptoTransform_ has only two possible values (CryptoTransform::kEncrypt/CryptoTransform::kDecrypt)
    uint8_t *const pOutput{buffOutput.Data(0U)};
    int32_t nNum{0};

// // DES_ede3_ofb64_encrypt function serves as both encryption and decryption. When 'in' is plaintext, it performs encryption; when 'in' is ciphertext, it performs decryption, making it symmetric.
#if PUHUA_OPENSSL_IS_11X
    DES_key_schedule *const pDesKey1{&desKeySchedule_};
    DES_key_schedule *const pDesKey2{&desKeySchedule2_};
    DES_key_schedule *const pDesKey3{&desKeySchedule3_};
    DES_ede3_ofb64_encrypt(pInputData, pOutput, static_cast< int64_t >(nDataLen), pDesKey1, pDesKey2, pDesKey3,
                           &initVector_, &nNum);
#else
    DES_ede3_ofb64_encrypt_evp(pInputData, pOutput, static_cast< int64_t >(nDataLen), desKeySchedule_, desKeySchedule2_,
                               desKeySchedule3_, initVector_, &nNum);
#endif
    if (nNum == 0) {
    }  // for qac
    buffOutput.SetDataSize(nDataLen);
    return nDataLen;
}
/// @brief Get IPC function name corresponding to encryption/decryption
/// @name    GetIpcFuncNameDoCipher
/// @returns IPC function name corresponding to encryption/decryption
ara::core::StringView PCtxSymmetricStream_3Des_Ofb64::GetIpcFuncNameDoCipher() const noexcept
{
    return FUNC_NAME_Symmetric(DoCipher_3Des_Ofb64_Stream);
}
//********************************/
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara
