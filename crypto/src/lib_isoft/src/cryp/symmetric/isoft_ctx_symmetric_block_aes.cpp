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
/// @file       isoft_ctx_symmetric_block_aes.cpp
/// @brief      AutoSar-Crypto Encryption and Decryption Module
/// @details
/// @date       2022-02-15
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Default Encryption and Decryption/Symmetric Encryption
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_01001
/// @unit_name=PCtxSymmetricBlock_Aes_Base
/// @unit_description=AES-based Symmetric Block Cipher Context Base Class
/// @endcode
///
/// ================================================================

#include "ara/crypto/cryp/symmetric/isoft_ctx_symmetric_block_aes.h"

#include "ara/crypto/cryp/cryobj/isoft_key_symmetric_aes.h"
#include "ara/crypto/cryp/cryobj/isoft_key_symmetric_ipc.h"
#include "ara/crypto/cryp/isoft_service_crypto.h"
#include "ara/crypto/ipc/isoft_ipc_auto_message.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
//********************************/
/// @brief Interface for symmetric block cipher context with padding: AES.
/// @name  PCtxSymmetricBlock_Aes_Base
/// @param cryptoProvider Crypto provider
/// @param nKeyBitLength Key length
PCtxSymmetricBlock_Aes_Base::PCtxSymmetricBlock_Aes_Base(PCryptoProvider &cryptoProvider,  // NOLINT
                                                         uint32_t const nKeyBitLength) noexcept
    // PRQA S 2427 QAC /// @qac: AUTOSAR standard interface
    : PCtxSymmetricBlock{cryptoProvider}  // PRQA L:QAC
    , nKeyBitLength_{nKeyBitLength}
{
}

/// @brief Clear the encryption context.
/// @name  Reset
/// @returns  ara::core::Result<void>
ara::core::Result< void > PCtxSymmetricBlock_Aes_Base::Reset() noexcept
{
#if PUHUA_OPENSSL_IS_11X
    std::ignore = memset(&aesKey_, 0, sizeof(AES_KEY));
#endif
    return PCtxSymmetricBlock::Reset();
}

/// @brief Set (deploy) a key for the symmetric algorithm context.
/// @name  SetKey
/// @param key Symmetric key
/// @param transform Encryption direction: encrypt or decrypt
/// @returns  ara::core::Result<void>
/// @throws
ara::core::Result< void > PCtxSymmetricBlock_Aes_Base::SetKey(SymmetricKey const &key,
                                                              CryptoTransform transform) noexcept
{
    /// @brief Return result type alias
    using PResult = ara::core::Result< void >;
    PKeySymmetric_Ipc const *const pPKeySymmetricIpc{dynamic_cast< PKeySymmetric_Ipc const * >(&key)};
    if (pPKeySymmetricIpc != nullptr) {  // ipc
        return _SetKeyIpc(GetIpcFuncNameSetKey(), key, transform);
    }

    PKeySymmetric_Aes const *const pAesKey{dynamic_cast< PKeySymmetric_Aes const * >(&key)};
    if (pAesKey == nullptr) {
        return PResult::FromError(SecurityErrorDomain::Errc::kIncompatibleObject);
    }
    size_t const supportKeyBitLength{static_cast< std::size_t >(_GetKeyBitLength())};
    if ((supportKeyBitLength != pAesKey->GetPayloadSize() * kInt_8U)
        && (supportKeyBitLength != static_cast< size_t >(PAlgId_Symmetric_AesKey::EKeyLen::kKeyBitLength_Any))) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kIncompatibleObject);
    }

    PResult pResult{PCtxSymmetricBlock::SetKey(key, transform)};
    if (false == pResult.HasValue()) {
        return pResult;
    }

#if PUHUA_OPENSSL_IS_11X
    int32_t const nKeyLengthBits{static_cast< int32_t >(pAesKey->GetPayloadSize()) * kInt_8};  // in bits
    int32_t ret{0};
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
//********************************/// PServiceCrypto interface
/// @brief Get the maximum supported key length (in bits).
/// @name  GetMaxKeyBitLength
/// @returns  std::size_t
std::size_t PCtxSymmetricBlock_Aes_Base::GetMaxKeyBitLength() const noexcept
{
    return static_cast< std::size_t >(PAlgId_Symmetric_AesKey::EKeyLen::kKeyBitLength_256);
}
/// @brief Get the minimum supported key length (in bits).
/// @name  GetMinKeyBitLength
/// @returns  std::size_t
std::size_t PCtxSymmetricBlock_Aes_Base::GetMinKeyBitLength() const noexcept
{
    return static_cast< std::size_t >(PAlgId_Symmetric_AesKey::EKeyLen::kKeyBitLength_128);
}
/// @brief Verify support for a specific key length according to the context.
/// @name  IsKeyBitLengthSupported
/// @param keyBitLength Key length: in bits
/// @returns  bool
bool PCtxSymmetricBlock_Aes_Base::IsKeyBitLengthSupported(std::size_t keyBitLength) const noexcept
{
    return (static_cast< std::size_t >(PAlgId_Symmetric_AesKey::EKeyLen::kKeyBitLength_128) == keyBitLength)
           || (static_cast< std::size_t >(PAlgId_Symmetric_AesKey::EKeyLen::kKeyBitLength_192) == keyBitLength)
           || (static_cast< std::size_t >(PAlgId_Symmetric_AesKey::EKeyLen::kKeyBitLength_256) == keyBitLength);
}
/// @brief Check whether a key is set for this context.
/// @name  IsKeyAvailable
/// @returns  bool
bool PCtxSymmetricBlock_Aes_Base::IsKeyAvailable() const noexcept { return PCtxSymmetricBlock::IsKeyAvailable(); }
/// @brief Get the block (or internal buffer) size of the underlying algorithm.
/// For digest, byte-by-byte stream cipher, and RNG contexts, it is an informational method only used to optimize interface usage.
/// @name  GetBlockSize
/// @returns  std::size_t
std::size_t PCtxSymmetricBlock_Aes_Base::GetBlockSize() const noexcept
{
    return static_cast< std::size_t >(PAlgId_Symmetric_AesKey::EKeyLen::kAesBlockSize);
}
//***************/
/// @brief Check whether the key meets the requirements
/// @name  CheckKey
/// @param key Symmetric key
/// @returns  bool
bool PCtxSymmetricBlock_Aes_Base::CheckKey(SymmetricKey const &key) const noexcept
{
    PAlgId_Symmetric_AesKey const cryptoKey;
    // 2022-02-14 Accepted key type: PAlgId_Symmetric_AesKey
    return cryptoKey.IsMinePrimitiveId(key.GetCryptoPrimitiveId()->GetPrimitiveId());
}
//********************************/
/// @brief Return a CryptoPrimitivId instance containing the instance identifier.
/// @return
CryptoPrimitiveId::Uptr PCtxSymmetricBlock_Aes_Ecb::GetCryptoPrimitiveId() const noexcept
{
    CryptoPrimitiveId::Uptr pReturn{nullptr};
    switch (_GetKeyBitLength()) {
        case static_cast< uint32_t >(PAlgId_Symmetric_AesKey::EKeyLen::kKeyBitLength_128): {
            pReturn = std::make_unique< PAlgId_Symmetric_Aes_Ecb_128 >();
            break;
        }
        case static_cast< uint32_t >(PAlgId_Symmetric_AesKey::EKeyLen::kKeyBitLength_192): {
            pReturn = std::make_unique< PAlgId_Symmetric_Aes_Ecb_192 >();
            break;
        }
        case static_cast< uint32_t >(PAlgId_Symmetric_AesKey::EKeyLen::kKeyBitLength_256): {
            pReturn = std::make_unique< PAlgId_Symmetric_Aes_Ecb_256 >();
            break;
        }
        case static_cast< uint32_t >(PAlgId_Symmetric_AesKey::EKeyLen::kKeyBitLength_Any): {
            pReturn = std::make_unique< PAlgId_Symmetric_Aes_Ecb >();
            break;
        }
        default: {
            break;
        }
    }
    return pReturn;
}
/// @brief Get a CryptoService instance.
/// @return
CryptoService::Uptr PCtxSymmetricBlock_Aes_Ecb::GetCryptoService() const noexcept
{
    return {std::make_unique< PServiceCrypto< PCtxSymmetricBlock_Aes_Ecb > >(*this)};
}
/// @brief Local encryption/decryption function
/// @name   DoSymmetric
/// @param pOutput Output
/// @param pInput Input
/// @param nInLen Data length
void PCtxSymmetricBlock_Aes_Ecb::DoSymmetric(uint8_t *pOutput, uint8_t const *pInput, uint32_t nInLen) const noexcept
{
    std::ignore = nInLen;
    PKeySymmetric_Aes const *const pAesKey{dynamic_cast< PKeySymmetric_Aes const * >(GetSymmetricKey())};
    if (pAesKey == nullptr) {
        return;
    }
    /// Ensure that when execution reaches here: eCryptoTransform_ has only two possible values (CryptoTransform::kEncrypt/CryptoTransform::kDecrypt)
    int32_t const nTransform{_IsTransformEnc() ? AES_ENCRYPT : AES_DECRYPT};
#if PUHUA_OPENSSL_IS_11X
    AES_ecb_encrypt(pInput, pOutput, &aesKey_, nTransform);
#else
    AES_ecb_encrypt_evp(pInput, pOutput, pAesKey->GetKeySymmetric(), static_cast< int32_t >(pAesKey->GetPayloadSize()),
                        nTransform);
#endif
}
/// @brief Get the function name in the IPC packet: DoCipher
/// @return
ara::core::StringView PCtxSymmetricBlock_Aes_Ecb::GetIpcFuncNameDoCipher() const noexcept
{
    return FUNC_NAME_Symmetric(DoCipher_Aes_Ecb);
}

//********************************/
/// @brief Return a CryptoPrimitivId instance containing the instance identifier.
/// @return
CryptoPrimitiveId::Uptr PCtxSymmetricBlock_Aes_Cbc::GetCryptoPrimitiveId() const noexcept
{
    CryptoPrimitiveId::Uptr pReturn{nullptr};
    switch (_GetKeyBitLength()) {
        case static_cast< uint32_t >(PAlgId_Symmetric_AesKey::EKeyLen::kKeyBitLength_128): {
            pReturn = std::make_unique< PAlgId_Symmetric_Aes_Cbc_128 >();
            break;
        }
        case static_cast< uint32_t >(PAlgId_Symmetric_AesKey::EKeyLen::kKeyBitLength_192): {
            pReturn = std::make_unique< PAlgId_Symmetric_Aes_Cbc_192 >();
            break;
        }
        case static_cast< uint32_t >(PAlgId_Symmetric_AesKey::EKeyLen::kKeyBitLength_256): {
            pReturn = std::make_unique< PAlgId_Symmetric_Aes_Cbc_256 >();
            break;
        }
        case static_cast< uint32_t >(PAlgId_Symmetric_AesKey::EKeyLen::kKeyBitLength_Any): {
            pReturn = std::make_unique< PAlgId_Symmetric_Aes_Cbc >();
            break;
        }
        default: {
            break;
        }
    }
    return pReturn;
}
/// @brief Get a CryptoService instance.
/// @return
CryptoService::Uptr PCtxSymmetricBlock_Aes_Cbc::GetCryptoService() const noexcept
{
    return {std::make_unique< PServiceCrypto< PCtxSymmetricBlock_Aes_Cbc > >(*this)};
}
/// @brief Clear the encryption context.
/// @return
ara::core::Result< void > PCtxSymmetricBlock_Aes_Cbc::Reset() noexcept
{
    _InitVector();
    return PCtxSymmetricBlock_Aes_Base::Reset();
}
/// @brief Local encryption/decryption function
/// @name   DoSymmetric
/// @param pOutput Output
/// @param pInput Input
/// @param nInLen Data length
void PCtxSymmetricBlock_Aes_Cbc::DoSymmetric(uint8_t *pOutput, uint8_t const *pInput, uint32_t nInLen) const noexcept
{
    std::ignore = nInLen;

    std::size_t const nBlockLength{GetBlockSize()};
    /// Ensure that when execution reaches here: eCryptoTransform_ has only two possible values (CryptoTransform::kEncrypt/CryptoTransform::kDecrypt)
    int32_t const nTransform{(CryptoTransform::kEncrypt == _GetCryptoTransform()) ? AES_ENCRYPT : AES_DECRYPT};
    uint8_t initVector[static_cast< uint32_t >(PAlgId_Symmetric_AesKey::EKeyLen::kAesBlockSize)]{0U};
    std::ignore = memcpy(&initVector, &initVector_, sizeof(initVector_));

#if PUHUA_OPENSSL_IS_11X
    AES_cbc_encrypt(pInput, pOutput, nBlockLength, &aesKey_, static_cast< uint8_t * >(initVector), nTransform);
#else
    PKeySymmetric_Aes const *const pAesKey{dynamic_cast< PKeySymmetric_Aes const * >(GetSymmetricKey())};
    if (pAesKey == nullptr) {
        return;
    }
    AES_cbc_encrypt_evp(pInput, pOutput, nBlockLength, pAesKey->GetKeySymmetric(),
                        static_cast< int32_t >(pAesKey->GetPayloadSize()), static_cast< uint8_t * >(initVector),
                        nTransform);
#endif

    if (initVector[0] == 0U) {
    }  // for qac
}
/// @brief Get the IPC function name corresponding to encryption/decryption
/// @name    GetIpcFuncNameDoCipher
/// @returns IPC function name corresponding to encryption/decryption
ara::core::StringView PCtxSymmetricBlock_Aes_Cbc::GetIpcFuncNameDoCipher() const noexcept
{
    return FUNC_NAME_Symmetric(DoCipher_Aes_Cbc);
}
/// @brief Initialize initVector_
void PCtxSymmetricBlock_Aes_Cbc::_InitVector() noexcept
{
    initVector_[0] = 0U;
    InitVector(static_cast< uint8_t * >(initVector_), static_cast< int32_t >(sizeof(initVector_)));
}
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara
