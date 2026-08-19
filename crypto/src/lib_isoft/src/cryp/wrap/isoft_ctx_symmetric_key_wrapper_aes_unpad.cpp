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
/// @file       isoft_ctx_symmetric_key_wrapper_aes_unpad.cpp
/// @brief      AutoSar-Crypto Encryption and Decryption Module
/// @details
/// @date       2022-04-18
/// @author     Zheng Chang
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Default Encryption and Decryption/Key Wrapping
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_01018
/// @unit_name=PCtxSymmetricKeyWrapperAesUnPad
/// @unit_description=AES-UNPAD-based Key Wrapping and Unwrapping Context
/// @endcode
///
/// ================================================================

#include "ara/crypto/cryp/wrap/isoft_ctx_symmetric_key_wrapper_aes_unpad.h"

#include "ara/crypto/cryp/cryobj/isoft_crypto_primitive_id.h"
#include "ara/crypto/cryp/cryobj/isoft_key_symmetric_ipc.h"
#include "ara/crypto/cryp/cryobj/isoft_secret_seed.h"
#include "ara/crypto/cryp/cryobj/isoft_secret_seed_ipc.h"
#include "ara/crypto/cryp/isoft_service_extension.h"
#include "ara/crypto/openssl/isoft_openssl_encrypt.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
/// @brief Get the target key granularity
/// @returns Key granularity size
std::size_t PCtxSymmetricKeyWrapperAesUnPad::GetTargetKeyGranularity() const noexcept
{
    return static_cast< std::size_t >(kInt_8U);
}
/// @brief Perform a "key unwrap" operation on the provided BLOB and generate a SecretSeed object.
///         If the implementation is based on the AES block cipher and applied to AES key material, this method should comply with RFC3394 or RFC5649. The created SecretSeed object has the following properties: session and non-exportable (because it is imported without meta information).
/// @param wrappedSeed Wrapped seed
/// @return Unwrapping result
ara::core::Result< ara::core::Vector< ara::core::Byte > > PCtxSymmetricKeyWrapperAesUnPad::DoUnwrap(
    ReadOnlyMemRegion const &wrappedSeed) const noexcept
{
    uint8_t const *const pData{wrappedSeed.data()};
    size_t const size{wrappedSeed.size()};
    if (_GetSlotId() != 0U) {
        return _DoWrapAndUnWrapIpc(FUNC_NAME_Wrap(DoUnwrapKey_Unpad), pData, static_cast< uint32_t >(size));
    }
    return DoUnWrap(pData, static_cast< uint32_t >(size));
}
/// @brief Perform a "key wrapping" operation on the provided key material.
///         If an implementation is based on the AES block cipher and applied to AES keys, this method should comply with RFC3394 or RFC5649. The method CalculateWrappedKeySize() can be used to calculate the size of the required output buffer.
/// @param key Key material
/// @return Wrapping result
ara::core::Result< ara::core::Vector< ara::core::Byte > > PCtxSymmetricKeyWrapperAesUnPad::DoWrapKeyMaterial(
    RestrictedUseObject const &key) const noexcept
{
    uint8_t const *pData{nullptr};
    uint32_t nSlotId{0U};
    PKeySymmetric_Ipc const *const pKeySymmetricIpc{dynamic_cast< PKeySymmetric_Ipc const * >(&key)};
    if (pKeySymmetricIpc != nullptr) {
        nSlotId = pKeySymmetricIpc->GetSlotId();
    } else {
        PKeySymmetric_Base const *const pKeySymmetricBase{dynamic_cast< PKeySymmetric_Base const * >(&key)};
        if (pKeySymmetricBase != nullptr) {
            pData = pKeySymmetricBase->GetKeySymmetric();
        }
    }

    if ((nSlotId == 0U) && (pData == nullptr)) {
        PSecretSeed_Ipc const *const pSecretSeedIpc{dynamic_cast< PSecretSeed_Ipc const * >(&key)};
        if (pSecretSeedIpc != nullptr) {
            nSlotId = pSecretSeedIpc->GetSlotId();
        } else {
            PSecretSeed const *const pSecretSeed{dynamic_cast< PSecretSeed const * >(&key)};
            if (pSecretSeed != nullptr) {
                pData = pSecretSeed->GetSecretSeedData();
            }
        }
    }
    size_t const nSize{key.GetPayloadSize()};

    if (((pData == nullptr) || (nSize == 0U)) && (nSlotId == 0U)) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kLogicFault);
    }

    if (_GetSlotId() != 0U) {
        if (nSlotId != 0U) {
            return _DoWrapAndUnWrapIpc(FUNC_NAME_Wrap(DoWrapKeyMaterial_Unpad), nullptr, 0U, nSlotId);
        }
        return _DoWrapAndUnWrapIpc(FUNC_NAME_Wrap(DoWrapKeyMaterial_Unpad), pData, static_cast< uint32_t >(nSize));
    }
    if (nSlotId != 0U) {
        SymmetricKey const *const pSymmetricKey{_GetSymmetricKey()};
        PKeySymmetric_Base const *const pAesKey{dynamic_cast< PKeySymmetric_Base const * >(pSymmetricKey)};
        if (pAesKey == nullptr) {
            return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
                SecurityErrorDomain::Errc::kLogicFault);
        }
        uint8_t const *const pKeyData{pAesKey->GetKeySymmetric()};
        size_t const nKeySize{pAesKey->GetPayloadSize()};
        return _DoWrapAndUnWrapIpc(FUNC_NAME_Wrap(DoWrapKeyMaterial_Unpad), pKeyData, static_cast< uint32_t >(nKeySize),
                                   nSlotId);  /// Use non-IPC to encrypt IPC
    }
    return DoWrap(pData, static_cast< uint32_t >(nSize));
}

/// @brief Get an ExtensionService instance.
/// @returns ExtensionService instance
ExtensionService::Uptr PCtxSymmetricKeyWrapperAesUnPad::GetExtensionService() const noexcept
{
    return {std::make_unique< PServiceExtension< PCtxSymmetricKeyWrapperAesUnPad > >(*this)};
}

/// @brief Return a CryptoPrimitivId instance containing the instance identifier.
/// @returns CryptoPrimitiveId instance
/// @return
CryptoPrimitiveId::Uptr PCtxSymmetricKeyWrapperAesUnPad::GetCryptoPrimitiveId() const noexcept
{
    return std::make_unique< PAlgId_WrapAesUnPad >();
}
/// @brief Execute wrapping logic
/// @param pInputData Starting address of input data in algorithm operation
/// @param nDataLen Data length
/// @return Wrapping result
ara::core::Result< ara::core::Vector< ara::core::Byte > > PCtxSymmetricKeyWrapperAesUnPad::DoWrap(
    uint8_t const *pInputData, uint32_t nDataLen) const noexcept
{
    ara::core::Vector< u_char > vecPlaintext;
    vecPlaintext.resize(kInt_1024U);
    u_char *const plaintext{vecPlaintext.data()};
    std::ignore = memset(static_cast< void * >(plaintext), 0, kInt_1024U);

    SymmetricKey const *const pSymmetricKey{_GetSymmetricKey()};
    PKeySymmetric_Base const *const pAesKey{dynamic_cast< PKeySymmetric_Base const * >(pSymmetricKey)};
    if (pAesKey == nullptr) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kLogicFault);
    }
    uint8_t const *const pKeyData{pAesKey->GetKeySymmetric()};
    size_t const nkeyLen{pAesKey->GetPayloadSize()};
    std::size_t const len{AES_wrap_key_unpad(pKeyData, nullptr, static_cast< uint8_t * >(plaintext),
                                             static_cast< u_char const * >(static_cast< void const * >(pInputData)),
                                             static_cast< uint32_t >(nDataLen), nkeyLen)};
    if (len <= 0U) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kLogicFault);
    }
    ara::core::Vector< ara::core::Byte > resultByte;
    try {
        resultByte.reserve(static_cast< std::size_t >(len));
    } catch (std::bad_alloc &) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kInsufficientCapacity);
    }
    for (std::size_t i{0U}; i < len; ++i) {
        ara::core::Byte const byData{*(plaintext + i)};
        resultByte.push_back(byData);
    }
    return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromValue(resultByte);
}
/// @brief Execute local unwrapping logic
/// @param pInputData Starting address of input data in algorithm operation
/// @param nDataLen Data length
/// @return Unwrapping result
ara::core::Result< ara::core::Vector< ara::core::Byte > > PCtxSymmetricKeyWrapperAesUnPad::DoUnWrap(
    uint8_t const *pInputData, uint32_t nDataLen) const noexcept
{
    ara::core::Vector< u_char > vecCiphertext;
    vecCiphertext.resize(kInt_1024U);
    u_char *const ciphertext{vecCiphertext.data()};
    std::ignore = memset(static_cast< void * >(ciphertext), 0, kInt_1024U);

    SymmetricKey const *const pSymmetricKey{_GetSymmetricKey()};
    PKeySymmetric_Base const *const pAesKey{dynamic_cast< PKeySymmetric_Base const * >(pSymmetricKey)};
    if (pAesKey == nullptr) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kLogicFault);
    }
    uint8_t const *const pKeyData{pAesKey->GetKeySymmetric()};
    size_t const nkeyLen{pAesKey->GetPayloadSize()};
    std::size_t const len{AES_unwrap_key_unpad(pKeyData, nullptr,
                                               static_cast< u_char * >(static_cast< void * >(ciphertext)),
                                               static_cast< u_char const * >(static_cast< void const * >(pInputData)),
                                               static_cast< uint32_t >(nDataLen), nkeyLen)};
    if (len <= 0U) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kLogicFault);
    }
    ara::core::Vector< ara::core::Byte > resultByte;
    try {
        resultByte.reserve(static_cast< std::size_t >(len));
    } catch (std::bad_alloc &) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kInsufficientCapacity);
    }
    for (std::size_t i{0U}; i < len; ++i) {
        ara::core::Byte const byData{*(ciphertext + i)};
        resultByte.push_back(byData);
    }
    return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromValue(resultByte);
}

}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara
