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
/// @file       isoft_ctx_symmetric_stream_aes.cpp
/// @brief      AutoSar-Crypto Encryption/Decryption Module
/// @details
/// @date       2022-02-23
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Default Encryption/Symmetric Encryption
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_01002
/// @unit_name=PCtxSymmetricStream_Aes_Base
/// @unit_description=Stream Symmetric Encryption Based on AES
/// @endcode
///
/// ================================================================

#include "ara/crypto/cryp/symmetric/isoft_ctx_symmetric_stream_aes.h"

#include "ara/crypto/cryp/cryobj/isoft_key_symmetric_aes.h"
#include "ara/crypto/cryp/cryobj/isoft_key_symmetric_ipc.h"
#include "ara/crypto/cryp/cryobj/isoft_secret_seed.h"
#include "ara/crypto/cryp/isoft_service_stream.h"
#include "ara/crypto/openssl/isoft_openssl_encrypt.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
//********************************/
/// @brief Interface for symmetric block cipher context with padding: AES.
//********************************/
/// @brief Parameterized constructor
/// @param cryptoProvider Encryption provider
/// @param nKeyBitLength Key length: in bits
/// @returns
/// @throws
PCtxSymmetricStream_Aes_Base::PCtxSymmetricStream_Aes_Base(PCryptoProvider &cryptoProvider,
                                                           uint32_t const nKeyBitLength) noexcept
    : PCtxSymmetricStream{cryptoProvider}  // PRQA S 2427 QAC /// @qac: AUTOSAR standard interface
    // PRQA L:QAC
    , nKeyBitLength_{nKeyBitLength}
{
}
//***************/
/// @brief Check byte-oriented operational mode.
/// @name  IsBytewiseMode
/// @returns  true if byte mode false if block mode
bool PCtxSymmetricStream_Aes_Base::IsBytewiseMode() const noexcept { return false; }
/// @brief Clear encryption context.
/// @name  Reset
/// @returns  has value if reset sucess false otherwise
ara::core::Result< void > PCtxSymmetricStream_Aes_Base::Reset() noexcept
{
    // std::ignore = memset(&aesKey_, 0, sizeof(AES_KEY));
    return PCtxSymmetricStream::Reset();
}
/// @brief Set (deploy) a key for the symmetric algorithm context.
/// @name  SetKey
/// @param key Symmetric key
/// @param transform Encryption direction: Encrypt or Decrypt
/// @returns has value if SetKey sucess false otherwise
ara::core::Result< void > PCtxSymmetricStream_Aes_Base::SetKey(SymmetricKey const &key,
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

#if PUHUA_OPENSSL_IS_11X
    int32_t ret{0};
    PKeySymmetric_Aes const *const pAesKey{dynamic_cast< PKeySymmetric_Aes const * >(GetSymmetricKey())};
    int32_t const nKeyLengthBits{static_cast< int32_t >(pAesKey->GetPayloadSize()) * kInt_8};  // In bits
    switch (_GetCryptoTransform()) {
        case CryptoTransform::kEncrypt: {
            ret = AES_set_encrypt_key(pAesKey->GetKeySymmetric(), nKeyLengthBits, &aesKey_);
        } break;
        case CryptoTransform::kDecrypt: {
            ret = AES_set_decrypt_key(pAesKey->GetKeySymmetric(), nKeyLengthBits, &aesKey_);
        } break;
        default: {
            ret = -1;
        } break;
    }
    if (ret != 0) {
        return PResult::FromError(SecurityErrorDomain::Errc::kUsageViolation);
    }
#endif
    return PResult::FromValue();
}
//********************************///PServiceStream interface
/// @brief Get the maximum supported key length in bits.
/// @name  GetMaxKeyBitLength
/// @returns  maximal supported length of the key in bits
std::size_t PCtxSymmetricStream_Aes_Base::GetMaxKeyBitLength() const noexcept
{
    return static_cast< std::size_t >(PAlgId_Symmetric_AesKey::EKeyLen::kKeyBitLength_256);
}
/// @brief Get the minimum supported key length in bits.
/// @name  GetMinKeyBitLength
/// @returns  minimal supported length of the key in bits
std::size_t PCtxSymmetricStream_Aes_Base::GetMinKeyBitLength() const noexcept
{
    return static_cast< std::size_t >(PAlgId_Symmetric_AesKey::EKeyLen::kKeyBitLength_128);
}
/// @brief Verify support for a specific key length based on the context.
/// @name  IsKeyBitLengthSupported
/// @param keyBitLength Key length: in bits
/// @returns  @c true if provided value of the key length is supported by the context
bool PCtxSymmetricStream_Aes_Base::IsKeyBitLengthSupported(std::size_t keyBitLength) const noexcept
{
    return static_cast< std::size_t >(nKeyBitLength_) == keyBitLength;
}
/// @brief Check if a key is set for this context.
/// @name  IsKeyAvailable
/// @returns  true if a key has been set to this context false otherwise
bool PCtxSymmetricStream_Aes_Base::IsKeyAvailable() const noexcept { return PCtxSymmetricStream::IsKeyAvailable(); }
/// @brief Get the block (or internal buffer) size of the underlying algorithm.
///         For digest, byte-stream cipher, and RNG contexts, this is an informational method used only for interface optimization.
/// @name  GetBlockSize
/// @returns Block (or internal buffer) size of the underlying algorithm
std::size_t PCtxSymmetricStream_Aes_Base::GetBlockSize() const noexcept
{
    return static_cast< std::size_t >(PAlgId_Symmetric_AesKey::EKeyLen::kAesBlockSize);
}
//***************/
/// @brief Check if Key meets requirements
/// @name  CheckKey
/// @param key Symmetric key
/// @returns  true if check key sucess false otherwise
bool PCtxSymmetricStream_Aes_Base::CheckKey(SymmetricKey const &key) const noexcept
{
    PAlgId_Symmetric_AesKey const cryptoKey;
    // 2022-02-14 Accept key type: PAlgId_Symmetric_AesKey
    if (false == cryptoKey.IsMinePrimitiveId(key.GetCryptoPrimitiveId()->GetPrimitiveId())) {
        return false;
    }
    PKeySymmetric_Aes const *const pAesKey{dynamic_cast< PKeySymmetric_Aes const * >(&key)};
    return IsKeyBitLengthSupported(pAesKey->GetPayloadSize() * kInt_8U);
}
/// @brief Execute initialization operation: using initialization vector
/// @name  DoInit
/// @param iv Initialization vector
void PCtxSymmetricStream_Aes_Base::DoInit(ReadOnlyMemRegion const &iv) noexcept { std::ignore = iv; }
/// @brief Execute initialization operation: via secret seed
/// @name  DoInit
/// @param iv Initialization secret seed
void PCtxSymmetricStream_Aes_Base::DoInit(SecretSeed const &iv) noexcept { std::ignore = iv; }
//********************************/
/// @brief Clear encryption context.
/// @return has value if reset sucess false otherwise
ara::core::Result< void > PCtxSymmetricStream_Aes_IV::Reset() noexcept
{
    _InitVector();
    return PCtxSymmetricStream_Aes_Base::Reset();
}
//***************/  //PServiceStream interface
/// @brief Get the actual bit length of the IV loaded into the context.
/// @param ivUid UID of the Initialization Vector (IV)
/// @return actual length of the IV (now set to the algorithm context) in bits
std::size_t PCtxSymmetricStream_Aes_IV::GetActualIvBitLength(
    ara::core::Optional< CryptoObjectUid > &ivUid) const noexcept
{
    if (ivUid.has_value()) {
    }
    ara::core::Optional< CryptoObjectUid > const rhs{ara::core::Optional< CryptoObjectUid >(secretSeedCouId_)};
    ivUid = rhs;
    if (false == IsStarted()) {
        return 0U;
    }
    return (sizeof(initVector_)) * kInt_8U;
}
/// @brief Get the default expected size of the Initialization Vector (IV) or nonce.
/// @return default expected size of IV in bytesdefault expected size of IV in bytes
std::size_t PCtxSymmetricStream_Aes_IV::GetIvSize() const noexcept { return sizeof(initVector_); }
/// @brief Verify the validity of a specific Initialization Vector (IV) length.
/// @param ivSize Initialization vector length
/// @return  @c true if provided IV length is supported by the algorithm and @c false otherwise
bool PCtxSymmetricStream_Aes_IV::IsValidIvSize(std::size_t ivSize) const noexcept
{
    return ivSize >= sizeof(initVector_);
}
//***************/
/// @brief Execute initialization operation: using initialization vector
/// @param iv Initialization vector
void PCtxSymmetricStream_Aes_IV::DoInit(ReadOnlyMemRegion const &iv) noexcept
{
    secretSeedCouId_ = CryptoObjectUid();
    std::size_t const nCopyLen{std::min< std::size_t >(iv.size(), GetIvSize())};
    for (std::size_t i{0U}; i < nCopyLen; ++i) {
        initVector_[i] = iv[i];  // NOLINT
    }
}
/// @brief Execute initialization operation: via secret seed
/// @param iv Initialization secret seed
void PCtxSymmetricStream_Aes_IV::DoInit(SecretSeed const &iv) noexcept
{
    PSecretSeed const *const pSecretSeed{dynamic_cast< PSecretSeed const * >(&iv)};
    if (pSecretSeed == nullptr) {
        return;
    }
    secretSeedCouId_ = pSecretSeed->GetObjectId().mCouid;
    uint8_t const *const pData{pSecretSeed->GetSecretSeedData()};
    size_t const seedLen{pSecretSeed->GetPayloadSize()};
    std::size_t const nCopyLen{std::min< std::size_t >(seedLen, GetIvSize())};
    for (std::size_t i{0U}; i < nCopyLen; i++) {
        initVector_[i] = *(pData + i);  // NOLINT
    }
}
/// @brief Initialize initVector_
void PCtxSymmetricStream_Aes_IV::_InitVector() noexcept
{
    initVector_[0] = kInt8_0U;
    InitVector(static_cast< uint8_t * >((initVector_)), static_cast< int32_t >(sizeof(initVector_)));
}
//********************************/
/// @brief Parameterized constructor
/// @param cryptoProvider Encryption provider
/// @throws
PCtxSymmetricStream_Aes_CfbOfbBase::PCtxSymmetricStream_Aes_CfbOfbBase(PCryptoProvider &cryptoProvider) noexcept
    : PCtxSymmetricStream_Aes_IV{cryptoProvider,
                                 static_cast< uint32_t >(PAlgId_Symmetric_AesKey::EKeyLen::kKeyBitLength_No)}
{
}
//********************************///PServiceStream interface
/// @brief Check byte-oriented operational mode.
/// @return true if byte mode false if block mode
bool PCtxSymmetricStream_Aes_CfbOfbBase::IsBytewiseMode() const noexcept { return true; }
/// @brief Set (deploy) a key for the symmetric algorithm context.
/// @param key Symmetric key
/// @param transform Encryption direction: Encrypt or Decrypt
/// @return has value if SetKey sucess false otherwise
ara::core::Result< void > PCtxSymmetricStream_Aes_CfbOfbBase::SetKey(SymmetricKey const &key,
                                                                     CryptoTransform transform) noexcept
{
    /// @brief Result type alias
    using PResult = ara::core::Result< void >;
    PKeySymmetric_Ipc const *const pPKeySymmetricIpc{dynamic_cast< PKeySymmetric_Ipc const * >(&key)};
    if (pPKeySymmetricIpc != nullptr) {  // ipc
        return _SetKeyIpc(GetIpcFuncNameSetKey(), key, transform);
    }
    PResult pResult{PCtxSymmetricStream::SetKey(key, transform)};  // NOLINT
    if (false == pResult.HasValue()) {
        return pResult;
    }

#if PUHUA_OPENSSL_IS_11X
    // CFB mode uses encryption key (AES_set_encrypt_key) for both encryption and decryption, which is unusual; remember this.
    PKeySymmetric_Aes const *const pAesKey{dynamic_cast< PKeySymmetric_Aes const * >(GetSymmetricKey())};
    // In bits; nKeyBitLength_ is not used here
    int32_t const nKeyLengthBits{static_cast< int32_t >(pAesKey->GetPayloadSize()) * 8};
    std::ignore = AES_set_encrypt_key(pAesKey->GetKeySymmetric(), nKeyLengthBits, &aesKey_);
#endif
    return PResult::FromValue();
}
/// @brief Verify support for a specific key length based on the context.
/// @param keyBitLength Key length: in bits
/// @return @c true if provided value of the key length is supported by the context
bool PCtxSymmetricStream_Aes_CfbOfbBase::IsKeyBitLengthSupported(std::size_t keyBitLength) const noexcept
{
    return (static_cast< std::size_t >(PAlgId_Symmetric_AesKey::EKeyLen::kKeyBitLength_128) == keyBitLength)
           || (static_cast< std::size_t >(PAlgId_Symmetric_AesKey::EKeyLen::kKeyBitLength_192) == keyBitLength)
           || (static_cast< std::size_t >(PAlgId_Symmetric_AesKey::EKeyLen::kKeyBitLength_256) == keyBitLength);
}
//***************/  //AUTOSAR-AP interface
/// @brief Get the block (or internal buffer) size of the underlying algorithm.
///         For digest, byte-stream cipher, and RNG contexts, this is an informational method used only for interface optimization.
/// @return Block (or internal buffer) size of the algorithm.
std::size_t PCtxSymmetricStream_Aes_Cfb1::GetBlockSize() const noexcept { return 1U; }
/// @brief Return a CryptoPrimitivId instance containing the instance identifier.
/// @return CryptoPrimitivId instance
CryptoPrimitiveId::Uptr PCtxSymmetricStream_Aes_Cfb1::GetCryptoPrimitiveId() const noexcept
{
    return {std::make_unique< PAlgId_Symmetric_Aes_Cfb1 >()};
}
/// @brief Get BlockService instance.
/// @return BlockService instance
BlockService::Uptr PCtxSymmetricStream_Aes_Cfb1::GetBlockService() const noexcept
{
    return {std::make_unique< PServiceStream< PCtxSymmetricStream_Aes_Cfb1 > >(*this)};
}
/// @brief Calculate encryption/decryption, save result in buffOutput_, return ciphertext length
/// @param buffOutput Output buffer
/// @param pInputData Start address of input data in algorithm operation
/// @param nDataLen Data length
/// @param alignedData Whether data is aligned
/// @return Ciphertext length
uint32_t PCtxSymmetricStream_Aes_Cfb1::DoCipherLocal(internal::PAutoBuff &buffOutput,
                                                     uint8_t const *pInputData,
                                                     uint32_t nDataLen,
                                                     bool alignedData) noexcept
{
    std::ignore = alignedData;
    PKeySymmetric_Ipc const *const pPKeySymmetricIpc{dynamic_cast< PKeySymmetric_Ipc const * >(GetSymmetricKey())};
    if (pPKeySymmetricIpc != nullptr) {  // ipc
        return _DoCipherIpc(GetIpcFuncNameDoCipher(), buffOutput, pInputData, nDataLen,
                            static_cast< uint8_t const * >(initVector_), _GetinitVectorLen());
    }

    PKeySymmetric_Aes const *const pAesKey{dynamic_cast< PKeySymmetric_Aes const * >(GetSymmetricKey())};
    if (pAesKey == nullptr) {
        return 0;
    }

    buffOutput.ResetData();
    /// Ensure execution reaches here: eCryptoTransform_ has only two possible values (CryptoTransform::kEncrypt/CryptoTransform::kDecrypt)
    int32_t const nTransform{_IsTransformEnc() ? AES_ENCRYPT : AES_DECRYPT};

    int32_t nNum{0};
    uint8_t *const pInitIV{initVector_};
    uint8_t *const pOutput{buffOutput.Data(0U)};
// AES_cfb1_encrypt function's length parameter is the number of bits in the input data, i.e., input data length * 8, not bytes.
#if PUHUA_OPENSSL_IS_11X
    uint32_t const len{nDataLen * kInt_8U};
    AES_cfb1_encrypt(pInputData, pOutput, static_cast< std::size_t >(len), &aesKey_, pInitIV, &nNum, nTransform);
#else
    AES_cfb1_encrypt_evp(pInputData, pOutput, static_cast< int64_t >(nDataLen), pAesKey->GetKeySymmetric(),
                         static_cast< int32_t >(pAesKey->GetPayloadSize()), pInitIV, &nNum, nTransform);
#endif
    if (nNum == 0) {
    }  // for qac
    buffOutput.SetDataSize(nDataLen);
    return nDataLen;
}
/// @brief Get IPC function name corresponding to encryption/decryption
/// @name    GetIpcFuncNameDoCipher
/// @returns IPC function name corresponding to encryption/decryption
ara::core::StringView PCtxSymmetricStream_Aes_Cfb1::GetIpcFuncNameDoCipher() const noexcept
{
    return FUNC_NAME_Symmetric(DoCipher_Aes_Cfb1_Stream);
}
//***************/  //AUTOSAR-AP interface
/// @brief Return a CryptoPrimitivId instance containing the instance identifier.
/// @name  GetCryptoPrimitiveId
/// @return CryptoPrimitivId instance
CryptoPrimitiveId::Uptr PCtxSymmetricStream_Aes_Cfb8::GetCryptoPrimitiveId() const noexcept
{
    return {std::make_unique< PAlgId_Symmetric_Aes_Cfb8 >()};
}
/// @brief Get BlockService instance.
/// @name  GetBlockService
/// @return BlockService instance
BlockService::Uptr PCtxSymmetricStream_Aes_Cfb8::GetBlockService() const noexcept
{
    return {std::make_unique< PServiceStream< PCtxSymmetricStream_Aes_Cfb8 > >(*this)};
}
/// @brief Calculate encryption/decryption, save result in buffOutput_, return ciphertext length
/// @name  DoCipherLocal
/// @param buffOutput Output buffer
/// @param pInputData Start address of input data in algorithm operation
/// @param nDataLen Data length
/// @param alignedData Whether data is aligned
/// @return Ciphertext length
uint32_t PCtxSymmetricStream_Aes_Cfb8::DoCipherLocal(internal::PAutoBuff &buffOutput,
                                                     uint8_t const *pInputData,
                                                     uint32_t nDataLen,
                                                     bool alignedData) noexcept
{
    std::ignore = alignedData;
    PKeySymmetric_Ipc const *const pPKeySymmetricIpc{dynamic_cast< PKeySymmetric_Ipc const * >(GetSymmetricKey())};
    if (pPKeySymmetricIpc != nullptr) {  // ipc
        return _DoCipherIpc(GetIpcFuncNameDoCipher(), buffOutput, pInputData, nDataLen,
                            static_cast< uint8_t const * >(initVector_), _GetinitVectorLen());
    }
    PKeySymmetric_Aes const *const pAesKey{dynamic_cast< PKeySymmetric_Aes const * >(GetSymmetricKey())};
    if (pAesKey == nullptr) {
        return 0;
    }

    buffOutput.ResetData();
    /// Ensure execution reaches here: eCryptoTransform_ has only two possible values (CryptoTransform::kEncrypt/CryptoTransform::kDecrypt)
    int32_t const nTransform{_IsTransformEnc() ? AES_ENCRYPT : AES_DECRYPT};

    int32_t nNum{0};
    uint8_t *const pInitIV{initVector_};
    uint8_t *const pOutput{buffOutput.Data(0U)};

#if PUHUA_OPENSSL_IS_11X
    AES_cfb8_encrypt(pInputData, pOutput, static_cast< std::size_t >(nDataLen), &aesKey_, pInitIV, &nNum, nTransform);
#else
    AES_cfb8_encrypt_evp(pInputData, pOutput, static_cast< std::size_t >(nDataLen), pAesKey->GetKeySymmetric(),
                         static_cast< int32_t >(pAesKey->GetPayloadSize()), pInitIV, &nNum, nTransform);
#endif
    if (nNum == 0) {
    }  // for qac
    buffOutput.SetDataSize(nDataLen);
    return nDataLen;
}
/// @brief Get IPC function name corresponding to encryption/decryption
/// @name    GetIpcFuncNameDoCipher
/// @returns IPC function name corresponding to encryption/decryption
ara::core::StringView PCtxSymmetricStream_Aes_Cfb8::GetIpcFuncNameDoCipher() const noexcept
{
    return FUNC_NAME_Symmetric(DoCipher_Aes_Cfb8_Stream);
}
//********************************/
//***************/  //AUTOSAR-AP interface
/// @brief Return a CryptoPrimitivId instance containing the instance identifier.
/// @return CryptoPrimitivId instance
CryptoPrimitiveId::Uptr PCtxSymmetricStream_Aes_Cfb128::GetCryptoPrimitiveId() const noexcept
{
    return {std::make_unique< PAlgId_Symmetric_Aes_Cfb128 >()};
}
/// @brief Get BlockService instance.
/// @return BlockService instance
BlockService::Uptr PCtxSymmetricStream_Aes_Cfb128::GetBlockService() const noexcept
{
    return {std::make_unique< PServiceStream< PCtxSymmetricStream_Aes_Cfb128 > >(*this)};
}
/// @brief Calculate encryption/decryption, save result in buffOutput_, return ciphertext length
/// @param buffOutput Output buffer
/// @param pInputData Start address of input data in algorithm operation
/// @param nDataLen Data length
/// @param alignedData Whether data is aligned
/// @return Ciphertext length
uint32_t PCtxSymmetricStream_Aes_Cfb128::DoCipherLocal(internal::PAutoBuff &buffOutput,
                                                       uint8_t const *pInputData,
                                                       uint32_t nDataLen,
                                                       bool alignedData) noexcept
{
    std::ignore = alignedData;
    PKeySymmetric_Ipc const *const pPKeySymmetricIpc{dynamic_cast< PKeySymmetric_Ipc const * >(GetSymmetricKey())};
    if (pPKeySymmetricIpc != nullptr) {  // ipc
        return _DoCipherIpc(GetIpcFuncNameDoCipher(), buffOutput, pInputData, nDataLen,
                            static_cast< uint8_t const * >(initVector_), _GetinitVectorLen());
    }

    PKeySymmetric_Aes const *const pAesKey{dynamic_cast< PKeySymmetric_Aes const * >(GetSymmetricKey())};
    if (pAesKey == nullptr) {
        return 0;
    }

    buffOutput.ResetData();
    /// Ensure execution reaches here: eCryptoTransform_ has only two possible values (CryptoTransform::kEncrypt/CryptoTransform::kDecrypt)
    int32_t const nTransform{_IsTransformEnc() ? AES_ENCRYPT : AES_DECRYPT};

    int32_t nNum{0};
    uint8_t *const pInitIV{initVector_};
    uint8_t *const pOutput{buffOutput.Data(0U)};

#if PUHUA_OPENSSL_IS_11X
    AES_cfb128_encrypt(pInputData, pOutput, static_cast< std::size_t >(nDataLen), &aesKey_, pInitIV, &nNum, nTransform);
#else
    AES_cfb128_encrypt_evp(pInputData, pOutput, static_cast< std::size_t >(nDataLen), pAesKey->GetKeySymmetric(),
                           static_cast< int32_t >(pAesKey->GetPayloadSize()), pInitIV, &nNum, nTransform);
#endif
    if (nNum == 0) {
    }  // for qac
    buffOutput.SetDataSize(nDataLen);
    return nDataLen;
}
/// @brief Get IPC function name corresponding to encryption/decryption
/// @name    GetIpcFuncNameDoCipher
/// @returns IPC function name corresponding to encryption/decryption
ara::core::StringView PCtxSymmetricStream_Aes_Cfb128::GetIpcFuncNameDoCipher() const noexcept
{
    return FUNC_NAME_Symmetric(DoCipher_Aes_Cfb128_Stream);
}
/// @brief Check byte-oriented operational mode.
/// @return true if byte mode false if block mode
bool PCtxSymmetricStream_Aes_Cfb128::IsBytewiseMode() const noexcept { return false; }
//********************************/
//***************/  //AUTOSAR-AP interface

/// @brief Return a CryptoPrimitivId instance containing the instance identifier.
/// @return CryptoPrimitivId instance
CryptoPrimitiveId::Uptr PCtxSymmetricStream_Aes_Ofb128::GetCryptoPrimitiveId() const noexcept
{
    return {std::make_unique< PAlgId_Symmetric_Aes_Ofb128 >()};
}
/// @brief Get BlockService instance.
/// @return BlockService instance
BlockService::Uptr PCtxSymmetricStream_Aes_Ofb128::GetBlockService() const noexcept
{
    return {std::make_unique< PServiceStream< PCtxSymmetricStream_Aes_Ofb128 > >(*this)};
}
/// @brief Calculate encryption/decryption, save result in buffOutput_, return ciphertext length.
/// @param buffOutput Output buffer
/// @param pInputData Start address of input data in algorithm operation
/// @param nDataLen Data length
/// @param alignedData Whether data is aligned
/// @return Ciphertext length
uint32_t PCtxSymmetricStream_Aes_Ofb128::DoCipherLocal(internal::PAutoBuff &buffOutput,
                                                       uint8_t const *pInputData,
                                                       uint32_t nDataLen,
                                                       bool alignedData) noexcept
{
    std::ignore = alignedData;
    PKeySymmetric_Ipc const *const pPKeySymmetricIpc{dynamic_cast< PKeySymmetric_Ipc const * >(GetSymmetricKey())};
    if (pPKeySymmetricIpc != nullptr) {  // ipc
        return _DoCipherIpc(GetIpcFuncNameDoCipher(), buffOutput, pInputData, nDataLen,
                            static_cast< uint8_t const * >(initVector_), _GetinitVectorLen());
    }
    PKeySymmetric_Aes const *const pAesKey{dynamic_cast< PKeySymmetric_Aes const * >(GetSymmetricKey())};
    if (pAesKey == nullptr) {
        return 0;
    }

    buffOutput.ResetData();
    /// Ensure execution reaches here: eCryptoTransform_ has only two possible values (CryptoTransform::kEncrypt/CryptoTransform::kDecrypt)
    int32_t nNum{0};
    uint8_t *const pInitIV{initVector_};
    uint8_t *const pOutput{buffOutput.Data(0U)};
    // AES_ofb128_encrypt function serves as both encryption and decryption. When 'in' is plaintext, it performs encryption; when 'in' is ciphertext, it performs decryption, making it symmetric.

#if PUHUA_OPENSSL_IS_11X
    AES_ofb128_encrypt(pInputData, pOutput, static_cast< std::size_t >(nDataLen), &aesKey_, pInitIV, &nNum);
#else
    AES_ofb128_encrypt_evp(pInputData, pOutput, static_cast< std::size_t >(nDataLen), pAesKey->GetKeySymmetric(),
                           static_cast< int32_t >(pAesKey->GetPayloadSize()), pInitIV, &nNum);
#endif
    if (nNum == 0) {
    }  // for qac
    buffOutput.SetDataSize(nDataLen);
    return nDataLen;
}
/// @brief Get IPC function name corresponding to encryption/decryption
/// @name    GetIpcFuncNameDoCipher
/// @returns IPC function name corresponding to encryption/decryption
ara::core::StringView PCtxSymmetricStream_Aes_Ofb128::GetIpcFuncNameDoCipher() const noexcept
{
    return FUNC_NAME_Symmetric(DoCipher_Aes_Ofb128_Stream);
}
/// @brief Check byte-oriented operational mode.
/// @return
bool PCtxSymmetricStream_Aes_Ofb128::IsBytewiseMode() const noexcept { return false; }
//********************************/
/// @brief Parameterized constructor
/// @param cryptoProvider Encryption provider
PCtxSymmetricStream_Aes_Ctr::PCtxSymmetricStream_Aes_Ctr(PCryptoProvider &cryptoProvider) noexcept
    : PCtxSymmetricStream_Aes_IV{cryptoProvider,
                                 static_cast< uint32_t >(PAlgId_Symmetric_AesKey::EKeyLen::kKeyBitLength_128)}
{
}
/// @brief Check byte-oriented operational mode.
/// @return true if byte mode false if block mode
bool PCtxSymmetricStream_Aes_Ctr::IsBytewiseMode() const noexcept { return false; }
/// @brief Check byte-oriented operational mode.
/// @name  IsSeekableMode
/// @returns  true if support seekable false otherwise
bool PCtxSymmetricStream_Aes_Ctr::IsSeekableMode() const noexcept { return true; }

/// @brief Execute Seek operation
/// @name   DoSeek
/// @param offset Offset
/// @param fromBegin Whether starting from the Begin position
/// @returns has value if DoSeek sucess false otherwise
ara::core::Result< void > PCtxSymmetricStream_Aes_Ctr::DoSeek(std::int64_t offset, bool fromBegin) noexcept
{
    if ((!fromBegin) && ((static_cast< int64_t >(currentPos_) + offset) < static_cast< int64_t >(kInt_0))) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kBelowBoundary);
    }

    offset_    = offset;
    fromBegin_ = fromBegin;
    return ara::core::Result< void >::FromValue();
}
//***************/  //AUTOSAR-AP interface
/// @brief Get the block (or internal buffer) size of the underlying algorithm.
///         For digest, byte-stream cipher, and RNG contexts, this is an informational method used only for interface optimization.
/// @return Block (or internal buffer) size of the underlying algorithm
std::size_t PCtxSymmetricStream_Aes_Ctr::GetBlockSize() const noexcept { return kInt_16U; }
/// @brief Return a CryptoPrimitivId instance containing the instance identifier.
/// @return CryptoPrimitivId instance
CryptoPrimitiveId::Uptr PCtxSymmetricStream_Aes_Ctr::GetCryptoPrimitiveId() const noexcept
{
    return {std::make_unique< PAlgId_Symmetric_Aes_Ctr >()};
}
/// @brief Get BlockService instance.
/// @return BlockService instance
BlockService::Uptr PCtxSymmetricStream_Aes_Ctr::GetBlockService() const noexcept
{
    return {std::make_unique< PServiceStream< PCtxSymmetricStream_Aes_Ctr > >(*this)};
}
/// @brief Set (deploy) a key for the symmetric algorithm context.
/// @param key Symmetric key
/// @param transform Encryption direction: Encrypt or Decrypt
/// @return has value if SetKey sucess false otherwise
ara::core::Result< void > PCtxSymmetricStream_Aes_Ctr::SetKey(SymmetricKey const &key,
                                                              CryptoTransform transform) noexcept
{
    /// @brief Result type alias
    using PResult = ara::core::Result< void >;
    PKeySymmetric_Ipc const *const pPKeySymmetricIpc{dynamic_cast< PKeySymmetric_Ipc const * >(&key)};
    if (pPKeySymmetricIpc != nullptr) {  // ipc
        return _SetKeyIpc(GetIpcFuncNameSetKey(), key, transform);
    }
    PResult pResult{PCtxSymmetricStream::SetKey(key, transform)};  // NOLINT
    if (false == pResult.HasValue()) {
        return pResult;
    }
#if PUHUA_OPENSSL_IS_11X
    // CFB mode uses encryption key (AES_set_encrypt_key) for both encryption and decryption, which is unusual; remember this.
    PKeySymmetric_Aes const *const pAesKey{dynamic_cast< PKeySymmetric_Aes const * >(GetSymmetricKey())};
    int32_t const nKeyLengthBits{static_cast< int32_t >(pAesKey->GetPayloadSize())
                                 * 8};  // In bits; nKeyBitLength_ is not used here
    std::ignore = AES_set_encrypt_key(pAesKey->GetKeySymmetric(), nKeyLengthBits, &aesKey_);
#endif
    return PResult::FromValue();
}
/// @brief Calculate encryption/decryption, save result in buffOutput_, return ciphertext length.
/// @param buffOutput Output buffer
/// @param pInputData Start address of input data in algorithm operation
/// @param nDataLen Data length
/// @param alignedData Whether data is aligned
/// @return Ciphertext length
uint32_t PCtxSymmetricStream_Aes_Ctr::DoCipherLocal(internal::PAutoBuff &buffOutput,
                                                    uint8_t const *pInputData,
                                                    uint32_t nDataLen,
                                                    bool alignedData) noexcept
{
    PKeySymmetric_Ipc const *const pPKeySymmetricIpc{dynamic_cast< PKeySymmetric_Ipc const * >(GetSymmetricKey())};
    if (pPKeySymmetricIpc != nullptr) {  // ipc
        return _DoCipherIpc(GetIpcFuncNameDoCipher(), buffOutput, pInputData, nDataLen,
                            static_cast< uint8_t const * >(initVector_), _GetinitVectorLen(), alignedData);
    }
    /* Counter */
    ara::crypto::internal::PAutoBuff ctr{static_cast< uint32_t >(AES_BLOCK_SIZE)};
    ctr.ResetData();
    uint8_t *const pData{ctr.Data()};
    std::ignore = memcpy(pData, static_cast< uint8_t const * >(initVector_), static_cast< size_t >(AES_BLOCK_SIZE));
    int64_t needOffsetLen{0};
    /// Default counter increment
    buffOutput.ResetData();
    PKeySymmetric_Aes const *const pAesKey{dynamic_cast< PKeySymmetric_Aes const * >(GetSymmetricKey())};
    int32_t const nKeyLengthBits{static_cast< int32_t >(pAesKey->GetPayloadSize())
                                 * 8};  // In bits; nKeyBitLength_ is not used here

    if (alignedData) {
        if (fromBegin_) {
            needOffsetLen = offset_;
        } else {
            needOffsetLen = (offset_ + static_cast< int64_t >(currentPos_));
        }
        if (needOffsetLen < 0) {
            return 0U;
        }
        currentPos_       = static_cast< uint32_t >(needOffsetLen);
        finishByteOffset_ = nDataLen;
    } else {
        for (uint32_t i{0U}; i < finishByteOffset_ / static_cast< uint32_t >(AES_BLOCK_SIZE); i++) {
            Increment_counter(pData, AES_BLOCK_SIZE);
        }
        finishByteOffset_ = 0U;
    }

    if (nDataLen <= static_cast< uint32_t >(needOffsetLen)) {
        return 0U;
    }
    offset_    = 0;
    fromBegin_ = true;
    Aes_ctr_encrypt(pInputData + needOffsetLen, static_cast< int32_t >(needOffsetLen),
                    static_cast< int32_t >(nDataLen) - static_cast< int32_t >(needOffsetLen), pData, buffOutput.Data(),
                    pAesKey->GetKeySymmetric(), nKeyLengthBits);
    return nDataLen - static_cast< uint32_t >(needOffsetLen);
}
/// @brief Get IPC function name corresponding to encryption/decryption
/// @name    GetIpcFuncNameDoCipher
/// @returns IPC function name corresponding to encryption/decryption
ara::core::StringView PCtxSymmetricStream_Aes_Ctr::GetIpcFuncNameDoCipher() const noexcept
{
    return FUNC_NAME_Symmetric(DoCipher_Aes_Ctr_Stream);
}
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara
