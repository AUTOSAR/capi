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
/// @file       isoft_ctx_symmetric_block_des.cpp
/// @brief      AutoSar-Crypto Encryption and Decryption Module
/// @details
/// @date       2022-01-20
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Default Encryption and Decryption/Symmetric Encryption
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_01003
/// @unit_name=PCtxSymmetricBlock_Des_Base
/// @unit_description=DES-based Symmetric Block Cipher Context
/// @endcode
///
/// ================================================================

#include "ara/crypto/cryp/symmetric/isoft_ctx_symmetric_block_des.h"

#include <ara/core/array.h>

#include "ara/crypto/common/isoft_data_type.h"
#include "ara/crypto/cryp/cryobj/isoft_key_symmetric_des.h"
#include "ara/crypto/cryp/cryobj/isoft_key_symmetric_ipc.h"
#include "ara/crypto/cryp/isoft_service_crypto.h"
#include "ara/crypto/openssl/isoft_openssl_encrypt.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
//********************************/
/// @brief Interface for symmetric block cipher context with padding: DES.
//********************************/

/// @brief Clear the encryption context.
/// @name  Reset
/// @returns  has value if reset sucess false otherwise
ara::core::Result< void > PCtxSymmetricBlock_Des_Base::Reset() noexcept
{
    std::ignore = memset(&desKeySchedule_, 0, kInt_8U);
    return PCtxSymmetricBlock::Reset();
}
/// @brief Set (deploy) a key for the symmetric algorithm context.
/// @name  SetKey
/// @param key Symmetric key
/// @param transform Encryption direction: encrypt or decrypt
/// @returns has value if SetKey sucess false otherwise
ara::core::Result< void > PCtxSymmetricBlock_Des_Base::SetKey(SymmetricKey const &key,
                                                              CryptoTransform transform) noexcept
{
    /// @brief Return value type alias
    using PResult = ara::core::Result< void >;
    PKeySymmetric_Ipc const *const pPKeySymmetricIpc{dynamic_cast< PKeySymmetric_Ipc const * >(&key)};
    if (pPKeySymmetricIpc != nullptr) {  // ipc
        return _SetKeyIpc(GetIpcFuncNameSetKey(), key, transform);
    }

    PResult pResult{PCtxSymmetricBlock::SetKey(key, transform)};
    if (false == pResult.HasValue()) {
        return pResult;
    }
    PKeySymmetric_Des const *const pSymmetricKeyDes{dynamic_cast< PKeySymmetric_Des const * >(GetSymmetricKey())};

#if PUHUA_OPENSSL_IS_11X
    const_DES_cblock *const pDesKey{T_TransPtr< const_DES_cblock >(pSymmetricKeyDes->GetKeySymmetric())};
    DES_set_key_unchecked(pDesKey, &desKeySchedule_);
#else
    memcpy(desKeySchedule_, pSymmetricKeyDes->GetKeySymmetric(), kInt_8U);
#endif

    return PResult::FromValue();
}
//********************************/// PServiceCrypto interface
/// @brief Get the block (or internal buffer) size of the underlying algorithm.
///         For digest, byte-by-byte stream cipher, and RNG contexts, it is an informational method only used to optimize interface usage.
/// @name  GetBlockSize
/// @returns Block (or internal buffer) size of the underlying algorithm
std::size_t PCtxSymmetricBlock_Des_Base::GetBlockSize() const noexcept
{
    return static_cast< std::size_t >(PAlgId_Symmetric_DesKey::EKeyLen::kDesBlockSize);
}
/// @brief Get the maximum supported key length (in bits). For DES, it is 64 bits, same for minimum and maximum
/// @name  GetMaxKeyBitLength
/// @returns  maximal supported length of the key in bits
std::size_t PCtxSymmetricBlock_Des_Base::GetMaxKeyBitLength() const noexcept
{
    return static_cast< std::size_t >(PAlgId_Symmetric_DesKey::EKeyLen::kDesKeyMaxBitLength);
}
/// @brief Get the minimum supported key length (in bits).
/// @name  GetMinKeyBitLength
/// @returns minimal supported length of the key in bits
std::size_t PCtxSymmetricBlock_Des_Base::GetMinKeyBitLength() const noexcept
{
    return static_cast< std::size_t >(PAlgId_Symmetric_DesKey::EKeyLen::kDesKeyMinBitLength);
}
//***************/
/// @brief Check whether the key meets the requirements
/// @name  CheckKey
/// @param key Symmetric key
/// @returns  true if check key sucess false otherwise
bool PCtxSymmetricBlock_Des_Base::CheckKey(SymmetricKey const &key) const noexcept
{
    PAlgId_Symmetric_DesKey cryptoKey;
    // 2022-01-21 Accepted key type: PAlgId_Symmetric_DesKey
    return cryptoKey.IsMinePrimitiveId(key.GetCryptoPrimitiveId()->GetPrimitiveId());
}
//********************************/
/// @brief Return a CryptoPrimitivId instance containing the instance identifier.
/// @return CryptoPrimitivId instance
CryptoPrimitiveId::Uptr PCtxSymmetricBlock_Des_Ecb::GetCryptoPrimitiveId() const noexcept
{
    return {std::make_unique< PAlgId_Symmetric_Des_Ecb >()};
}
/// @brief Get a CryptoService instance.
/// @return CryptoService instance
CryptoService::Uptr PCtxSymmetricBlock_Des_Ecb::GetCryptoService() const noexcept
{
    return {std::make_unique< PServiceCrypto< PCtxSymmetricBlock_Des_Ecb > >(*this)};
}
//***************/
/// @brief Local encryption/decryption function
/// @name   DoSymmetric
/// @param pOutput Output
/// @param pInput Input
/// @param nInLen Data length
void PCtxSymmetricBlock_Des_Ecb::DoSymmetric(uint8_t *pOutput, uint8_t const *pInput, uint32_t nInLen) const noexcept
{
    std::ignore = nInLen;
    int32_t const nDesTransform{(CryptoTransform::kEncrypt == _GetCryptoTransform()) ? DES_ENCRYPT : DES_DECRYPT};

#if PUHUA_OPENSSL_IS_11X
    DES_key_schedule *const pDesKey{&desKeySchedule_};
    // PRQA S 3066 QAC /// @qac: Necessary for calling OpenSSL's C interface
    const_DES_cblock *const pDesInput{T_TransPtr< uint8_t, const_DES_cblock >(const_cast< uint8_t * >(pInput))};
    // PRQA L:QAC
    DES_cblock *const pDesOutput{T_TransPtr< uint8_t, DES_cblock >(pOutput)};
    DES_ecb_encrypt(pDesInput, pDesOutput, pDesKey, nDesTransform);
#else
    DES_ecb_encrypt_evp(pInput, pOutput, desKeySchedule_, nDesTransform);
#endif
}
/// @brief Get the IPC function name: DoCipher
/// @return IPC function name corresponding to encryption/decryption
ara::core::StringView PCtxSymmetricBlock_Des_Ecb::GetIpcFuncNameDoCipher() const noexcept
{
    return FUNC_NAME_Symmetric(DoCipher_Des_Ecb);
}
//********************************/
/// @brief Return a CryptoPrimitivId instance containing the instance identifier.
/// @return CryptoPrimitivId instance
CryptoPrimitiveId::Uptr PCtxSymmetricBlock_Des_Cbc::GetCryptoPrimitiveId() const noexcept
{
    return {std::make_unique< PAlgId_Symmetric_Des_Cbc >()};
}
/// @brief Get a CryptoService instance.
/// @return CryptoService instance
CryptoService::Uptr PCtxSymmetricBlock_Des_Cbc::GetCryptoService() const noexcept
{
    return {std::make_unique< PServiceCrypto< PCtxSymmetricBlock_Des_Cbc > >(*this)};
}
/// @brief Clear the encryption context.
/// @return  has value if reset sucess false otherwise
ara::core::Result< void > PCtxSymmetricBlock_Des_Cbc::Reset() noexcept
{
    _InitVector();
    return PCtxSymmetricBlock_Des_Base::Reset();
}
//***************/
/// @brief Local encryption/decryption function
/// @name   DoSymmetric
/// @param pOutput Output
/// @param pInput Input
/// @param nInLen Data length
void PCtxSymmetricBlock_Des_Cbc::DoSymmetric(uint8_t *pOutput, uint8_t const *pInput, uint32_t nInLen) const noexcept
{
    std::ignore = nInLen;
    std::size_t const nBlockLength{GetBlockSize()};
    /// Ensure that when execution reaches here: eCryptoTransform_ has only two possible values (CryptoTransform::kEncrypt/CryptoTransform::kDecrypt)
    int32_t const nDesTransform{_IsTransformEnc() ? DES_ENCRYPT : DES_DECRYPT};
    /// @qac Cannot modify [2410]: This object is an array type.
    ara::core::Array< uint8_t, sizeof(DES_cblock) > desInitArray{};
    std::ignore = memcpy(desInitArray.data(), &initVector_, sizeof(DES_cblock));

#if PUHUA_OPENSSL_IS_11X
    DES_key_schedule *const pDesKey{&desKeySchedule_};
    DES_ncbc_encrypt(pInput, pOutput, static_cast< int64_t >(nBlockLength), pDesKey,
                     T_TransPtr< uint8_t, DES_cblock >(desInitArray.data()), nDesTransform);
#else
    DES_ncbc_encrypt_evp(pInput, pOutput, static_cast< int64_t >(nBlockLength), desKeySchedule_, desInitArray.data(),
                         nDesTransform);
#endif
}
/// @brief Get the IPC function name corresponding to encryption/decryption
/// @name    GetIpcFuncNameDoCipher
/// @returns IPC function name corresponding to encryption/decryption
ara::core::StringView PCtxSymmetricBlock_Des_Cbc::GetIpcFuncNameDoCipher() const noexcept
{
    return FUNC_NAME_Symmetric(DoCipher_Des_Cbc);
}
//***************/
/// @brief Initialize initVector_
void PCtxSymmetricBlock_Des_Cbc::_InitVector() noexcept
{
    initVector_[0] = 0U;
    InitVector(static_cast< uint8_t * >(initVector_), static_cast< int32_t >(sizeof(initVector_)));
}
//********************************/
/// @brief Clear the encryption context.
/// @return has value if reset sucess false otherwise
ara::core::Result< void > PCtxSymmetricBlock_3Des_Base::Reset() noexcept
{
    std::ignore = memset(&desKeySchedule2_, 0, kInt_8U);
    std::ignore = memset(&desKeySchedule3_, 0, kInt_8U);
    return PCtxSymmetricBlock_Des_Base::Reset();
}
/// @brief Set (deploy) a key for the symmetric algorithm context.
/// @param key Symmetric key
/// @param transform Encryption direction: encrypt or decrypt
/// @return has value if SetKey sucess false otherwise
ara::core::Result< void > PCtxSymmetricBlock_3Des_Base::SetKey(SymmetricKey const &key,
                                                               CryptoTransform transform) noexcept
{
    /// @brief Return value type alias
    using PResult = ara::core::Result< void >;
    PKeySymmetric_Ipc const *const pPKeySymmetricIpc{dynamic_cast< PKeySymmetric_Ipc const * >(&key)};
    if (pPKeySymmetricIpc != nullptr) {  // ipc
        return _SetKeyIpc(GetIpcFuncNameSetKey(), key, transform);
    }
    PResult pResult{PCtxSymmetricBlock::SetKey(key, transform)};  // NOLINT
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
    if (pSymmetricKeyDes->GetPayloadSize() >= (kInt_3U * GetBlockSize())) {
        DES_set_key_unchecked(pDesKey + kInt_2U, &desKeySchedule3_);
    }
    if (pSymmetricKeyDes->GetPayloadSize() >= (kInt_2U * GetBlockSize())) {
        DES_set_key_unchecked(pDesKey + 1, &desKeySchedule2_);
    }
#else
    uint8_t *const pDesKey{(pSymmetricKeyDes->GetKeySymmetric())};
    memcpy(desKeySchedule_, pDesKey, kInt_8U);
    memcpy(desKeySchedule2_, pDesKey, kInt_8U);
    memcpy(desKeySchedule3_, pDesKey, kInt_8U);

    if (pSymmetricKeyDes->GetPayloadSize() >= (kInt_3U * GetBlockSize())) {
        memcpy(desKeySchedule3_, pDesKey + kInt_2U * GetBlockSize(), kInt_8U);
    }
    if (pSymmetricKeyDes->GetPayloadSize() >= (kInt_2U * GetBlockSize())) {
        memcpy(desKeySchedule2_, pDesKey + kInt_1U * GetBlockSize(), kInt_8U);
    }
#endif
    return PResult::FromValue();
}
//***************/  // PServiceCrypto interface
/// @brief Get the maximum supported key length (in bits).
/// @return maximal supported length of the key in bits
std::size_t PCtxSymmetricBlock_3Des_Base::GetMaxKeyBitLength() const noexcept
{
    return static_cast< std::size_t >(PAlgId_Symmetric_DesKey::EKeyLen::kDesKeyMaxBitLength);
}
/// @brief Get the maximum supported key length (in bits).
/// @return minimal supported length of the key in bits
std::size_t PCtxSymmetricBlock_3Des_Base::GetMinKeyBitLength() const noexcept
{
    return static_cast< std::size_t >(PAlgId_Symmetric_DesKey::EKeyLen::kDesKeyMinBitLength);
}
/// @brief Verify support for a specific key length according to the context.
/// @param keyBitLength Key length: in bits
/// @return  @c true if provided value of the key length is supported by the context
bool PCtxSymmetricBlock_3Des_Base::IsKeyBitLengthSupported(std::size_t keyBitLength) const noexcept
{
    if (false == PCtxSymmetricBlock::IsKeyBitLengthSupported(keyBitLength)) {
        return false;
    }
    return 0U == (keyBitLength % static_cast< std::size_t >(PAlgId_Symmetric_DesKey::EKeyLen::kDesKeyMinBitLength));
}
/// @brief Get the block (or internal buffer) size of the underlying algorithm.
///         For digest, byte-by-byte stream cipher, and RNG contexts, it is an informational method only used to optimize interface usage.
/// @return Block (or internal buffer) size of the underlying algorithm
std::size_t PCtxSymmetricBlock_3Des_Base::GetBlockSize() const noexcept
{
    return static_cast< std::size_t >(PAlgId_Symmetric_DesKey::EKeyLen::kDesBlockSize);
}
//***************/
/// @brief Check whether the key meets the requirements
/// @param key Symmetric key
/// @return true if check key sucess false otherwise
bool PCtxSymmetricBlock_3Des_Base::CheckKey(SymmetricKey const &key) const noexcept
{
    // 2022-03-22 Accepted key types: PAlgId_Symmetric_DesKey/PAlgId_Symmetric_3DesKey
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
/// @brief Return a CryptoPrimitivId instance containing the instance identifier.
/// @return CryptoPrimitivId instance
CryptoPrimitiveId::Uptr PCtxSymmetricBlock_3Des_Ecb::GetCryptoPrimitiveId() const noexcept
{
    return {std::make_unique< PAlgId_Symmetric_3Des_Ecb >()};
}
/// @brief Get a CryptoService instance.
/// @return CryptoService instance
CryptoService::Uptr PCtxSymmetricBlock_3Des_Ecb::GetCryptoService() const noexcept
{
    return {std::make_unique< PServiceCrypto< PCtxSymmetricBlock_3Des_Ecb > >(*this)};
}
//***************/
/// @brief Local encryption/decryption function
/// @name   DoSymmetric
/// @param pOutput Output
/// @param pInput Input
/// @param nInLen Data length
void PCtxSymmetricBlock_3Des_Ecb::DoSymmetric(uint8_t *pOutput, uint8_t const *pInput, uint32_t nInLen) const noexcept
{
    std::ignore = nInLen;
    /// Ensure that when execution reaches here: eCryptoTransform_ has only two possible values (CryptoTransform::kEncrypt/CryptoTransform::kDecrypt)
    int32_t const nTransform{_IsTransformEnc() ? DES_ENCRYPT : DES_DECRYPT};

#if PUHUA_OPENSSL_IS_11X
    DES_key_schedule *const pDesKey1{&desKeySchedule_};
    DES_key_schedule *const pDesKey2{&desKeySchedule2_};
    DES_key_schedule *const pDesKey3{&desKeySchedule3_};
    // PRQA S 3049,3066 QAC /// @qac: If C-style casts are not used, this is the only way to write it
    const_DES_cblock *const pDesInput{reinterpret_cast< const_DES_cblock * >(const_cast< uint8_t * >(pInput))};
    DES_cblock *const pDesOutput{reinterpret_cast< DES_cblock * >(pOutput)};
    // PRQA L:QAC
    DES_ecb3_encrypt(pDesInput, pDesOutput, pDesKey1, pDesKey2, pDesKey3, nTransform);
#else
    DES_ecb3_encrypt_evp(pInput, pOutput, desKeySchedule_, desKeySchedule2_, desKeySchedule3_, nTransform);
#endif
}
/// @brief Get the IPC function name corresponding to encryption/decryption
/// @name    GetIpcFuncNameDoCipher
/// @returns IPC function name corresponding to encryption/decryption
ara::core::StringView PCtxSymmetricBlock_3Des_Ecb::GetIpcFuncNameDoCipher() const noexcept
{
    return FUNC_NAME_Symmetric(DoCipher_3Des_Ecb);
}
//********************************/
/// @brief Return a CryptoPrimitivId instance containing the instance identifier.
/// @return CryptoPrimitivId instance
CryptoPrimitiveId::Uptr PCtxSymmetricBlock_3Des_Cbc::GetCryptoPrimitiveId() const noexcept
{
    return {std::make_unique< PAlgId_Symmetric_3Des_Cbc >()};
}
/// @brief Get a CryptoService instance.
/// @return CryptoService instance
CryptoService::Uptr PCtxSymmetricBlock_3Des_Cbc::GetCryptoService() const noexcept
{
    return {std::make_unique< PServiceCrypto< PCtxSymmetricBlock_3Des_Cbc > >(*this)};
}
/// @brief Clear the encryption context.
/// @return has value if reset sucess false otherwise
ara::core::Result< void > PCtxSymmetricBlock_3Des_Cbc::Reset() noexcept
{
    _InitVector();
    return PCtxSymmetricBlock_3Des_Base::Reset();
}
//***************/
/// @brief Local encryption/decryption function
/// @name   DoSymmetric
/// @param pOutput Output
/// @param pInput Input
/// @param nInLen Data length
void PCtxSymmetricBlock_3Des_Cbc::DoSymmetric(uint8_t *pOutput, uint8_t const *pInput, uint32_t nInLen) const noexcept
{
    std::ignore = nInLen;
    std::size_t const nBlockLength{GetBlockSize()};
    /// Ensure that when execution reaches here: eCryptoTransform_ has only two possible values (CryptoTransform::kEncrypt/CryptoTransform::kDecrypt)
    int32_t const nTransform{_IsTransformEnc() ? DES_ENCRYPT : DES_DECRYPT};
    /// @qac Cannot modify [2410]: This object is an array type.
    ara::core::Array< uint8_t, sizeof(DES_cblock) > desInitArray{};
    std::ignore = memcpy(desInitArray.data(), &initVector_, sizeof(DES_cblock));

#if PUHUA_OPENSSL_IS_11X
    DES_key_schedule *const pDesKey1{&desKeySchedule_};
    DES_key_schedule *const pDesKey2{&desKeySchedule2_};
    DES_key_schedule *const pDesKey3{&desKeySchedule3_};

    DES_ede3_cbc_encrypt(pInput, pOutput, static_cast< int64_t >(nBlockLength), pDesKey1, pDesKey2, pDesKey3,
                         T_TransPtr< uint8_t, DES_cblock >(desInitArray.data()), nTransform);
#else
    DES_ede3_cbc_encrypt_evp(pInput, pOutput, static_cast< int64_t >(nBlockLength), desKeySchedule_, desKeySchedule2_,
                             desKeySchedule3_, desInitArray.data(), nTransform);
#endif
}
/// @brief Get the IPC function name corresponding to encryption/decryption
/// @name    GetIpcFuncNameDoCipher
/// @returns IPC function name corresponding to encryption/decryption
ara::core::StringView PCtxSymmetricBlock_3Des_Cbc::GetIpcFuncNameDoCipher() const noexcept
{
    return FUNC_NAME_Symmetric(DoCipher_3Des_Cbc);
}
//***************/
/// @brief Initialize initVector_
void PCtxSymmetricBlock_3Des_Cbc::_InitVector() noexcept
{
    initVector_[0] = 0U;
    InitVector(static_cast< uint8_t * >(initVector_), static_cast< int32_t >(sizeof(initVector_)));
}
//********************************/
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara
