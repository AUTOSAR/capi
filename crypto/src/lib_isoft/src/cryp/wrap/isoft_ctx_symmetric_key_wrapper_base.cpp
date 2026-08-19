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
/// @file       isoft_ctx_symmetric_key_wrapper_base.cpp
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
/// @unit_name=PCtxSymmetricKeyWrapper_Base
/// @unit_description=Key Wrapping Context Base Class
/// @endcode
///
/// ================================================================

#include "ara/crypto/cryp/wrap/isoft_ctx_symmetric_key_wrapper_base.h"

#include "ara/crypto/cryp/cryobj/isoft_signature.h"
#include "ara/crypto/cryp/isoft_crypto_provider.h"
namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {

/// @brief Constructor with parameters
/// @param cryptoProvider Crypto provider
/// @returns
/// @throws
PCtxSymmetricKeyWrapper_Base::PCtxSymmetricKeyWrapper_Base(PCryptoProvider &cryptoProvider) noexcept
    : SymmetricKeyWrapperCtx{}  // NOLINT
    , cryptoProvider_{cryptoProvider}
{
}

/// @brief Get a reference to the Crypto Provider for this context.
/// @returns Reference to the crypto provider
// PRQA S 4623 QAC /// @qac: AUTOSAR standard interface
inline CryptoProvider &PCtxSymmetricKeyWrapper_Base::MyProvider() const noexcept { return cryptoProvider_; }
// PRQA L:QAC

/// @brief Check whether the encryption context has been initialized and is ready for use. It checks all required values, including: key value, IV/seed, etc.
/// @returns  true if already init false otherwise
bool PCtxSymmetricKeyWrapper_Base::IsInitialized() const noexcept { return IsKeyAvailable(); }

/// @brief Perform a "key unwrap" operation on the provided BLOB and generate a key object.
///         If the implementation is based on the AES block cipher and applied to AES keys, this method should comply with RFC3394 or RFC5649.
///         The created Key object has the following properties: session and non-exportable (because it is imported without meta information)!
/// @param wrappedKey
/// @param algId Crypto primitive ID
/// @param allowedUsage Usage scope
/// @return  unique smart pointer to @c Key object, which keeps unwrapped key material
ara::core::Result< RestrictedUseObject::Uptrc > PCtxSymmetricKeyWrapper_Base::UnwrapKey(
    ReadOnlyMemRegion const &wrappedKey, AlgId algId, AllowedUsageFlags allowedUsage) const noexcept
{
    if (!IsInitialized()) {
        return ara::core::Result< RestrictedUseObject::Uptrc >::FromError(
            SecurityErrorDomain::Errc::kUninitializedContext);
    }

    if (wrappedKey.size() < kInt_16U) {
        return ara::core::Result< RestrictedUseObject::Uptrc >::FromError(SecurityErrorDomain::Errc::kInvalidInputSize);
    }
    if (wrappedKey.size() > kInt_40U) {
        return ara::core::Result< RestrictedUseObject::Uptrc >::FromError(SecurityErrorDomain::Errc::kInvalidInputSize);
    }

    ara::core::Result< ara::core::Vector< ara::core::Byte > > pResultByte{DoUnwrap(wrappedKey)};
    if (!pResultByte.HasValue()) {
        return ara::core::Result< RestrictedUseObject::Uptrc >::FromError(SecurityErrorDomain::Errc::kLogicFault);
    }
    // ara::core::Vector<ara::core::Byte> const & vecData {pResultByte.Value()};
    ara::core::Vector< ara::core::Byte > vecData{std::move(pResultByte).Value()};
    if (vecData.empty()) {
        return ara::core::Result< RestrictedUseObject::Uptrc >::FromError(SecurityErrorDomain::Errc::kLogicFault);
    }
    ReadOnlyMemRegion const memKeyData{T_TransBytes(&vecData[0U]), vecData.size()};
    ara::core::Result< SymmetricKey::Uptrc > resultSymmetric{
        PCryptoProvider::GenerateSymmetricKeyEx(algId, memKeyData, allowedUsage)};
    if (!resultSymmetric.HasValue()) {
        return ara::core::Result< RestrictedUseObject::Uptrc >::FromValue(nullptr);
    }

    SymmetricKey::Uptrc pSymmetricKey{std::move(resultSymmetric).Value()};
    return ara::core::Result< RestrictedUseObject::Uptrc >::FromValue(std::move(pSymmetricKey));
}

/// @brief Perform a "key unwrap" operation on the provided BLOB and generate a SecretSeed object.
///         If the implementation is based on the AES block cipher and applied to AES key material, this method should comply with RFC3394 or RFC5649. The created SecretSeed object has the following properties: session and non-exportable (because it is imported without meta information).
/// @returns  ara::core::Result<SecretSeed::Uptrc>
/// @param wrappedSeed Wrapped seed
/// @param targetAlgId Target algorithm crypto primitive ID
/// @param allowedUsage Usage scope
/// @return  unique smart pointer to @c SecretSeed object, which keeps unwrapped key material
ara::core::Result< SecretSeed::Uptrc > PCtxSymmetricKeyWrapper_Base::UnwrapSeed(
    ReadOnlyMemRegion const &wrappedSeed, AlgId targetAlgId, SecretSeed::Usage allowedUsage) const noexcept
{
    if (!IsInitialized()) {
        return ara::core::Result< SecretSeed::Uptrc >::FromError(SecurityErrorDomain::Errc::kUninitializedContext);
    }

    if (wrappedSeed.size() < kInt_16U) {
        return ara::core::Result< SecretSeed::Uptrc >::FromError(SecurityErrorDomain::Errc::kInvalidInputSize);
    }
    if (wrappedSeed.size() > kInt_40U) {
        return ara::core::Result< SecretSeed::Uptrc >::FromError(SecurityErrorDomain::Errc::kInvalidInputSize);
    }

    ara::core::Result< ara::core::Vector< ara::core::Byte > > pResultByte{DoUnwrap(wrappedSeed)};
    if (!pResultByte.HasValue()) {
        return ara::core::Result< SecretSeed::Uptrc >::FromError(SecurityErrorDomain::Errc::kLogicFault);
    }
    ara::core::Vector< ara::core::Byte > vecData{std::move(pResultByte).Value()};
    if (vecData.empty()) {
        return ara::core::Result< SecretSeed::Uptrc >::FromError(SecurityErrorDomain::Errc::kLogicFault);
    }
    ReadOnlyMemRegion const memKeyData{T_TransBytes(&vecData[0U]), vecData.size()};
    PCryptoProvider *const pCryptoProvider{dynamic_cast< PCryptoProvider *const >(&MyProvider())};
    if (nullptr == pCryptoProvider) {
        return ara::core::Result< SecretSeed::Uptrc >::FromValue(nullptr);
    }
    ara::core::Result< SecretSeed::Uptrc > resultSecretSeed{
        pCryptoProvider->GenerateSeedEx(targetAlgId, memKeyData, allowedUsage)};
    if (!resultSecretSeed.HasValue()) {
        return ara::core::Result< SecretSeed::Uptrc >::FromValue(nullptr);
    }

    SecretSeed::Uptrc pSecretSeed{std::move(resultSecretSeed).Value()};
    return ara::core::Result< SecretSeed::Uptrc >::FromValue(std::move(pSecretSeed));
}

/// @brief Perform a "key wrapping" operation on the provided key material.
///         If an implementation is based on the AES block cipher and applied to AES keys, this method should comply with RFC3394 or RFC5649. The method CalculateWrappedKeySize() can be used to calculate the size of the required output buffer.
/// @param key Key material
/// @return Wrapping result
ara::core::Result< ara::core::Vector< ara::core::Byte > > PCtxSymmetricKeyWrapper_Base::WrapKeyMaterial(
    RestrictedUseObject const &key) const noexcept
{
    /// @error: SecurityErrorDomain::kUninitializedContext   if the context was not initialized by a key value
    if (!IsInitialized()) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kUninitializedContext);
    }
    /// @error SecurityErrorDomain::kInvalidInputSize       if the @c key object has an unsupported length
    if (key.GetPayloadSize() < kInt_8U) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kInvalidInputSize);
    }
    if (key.GetPayloadSize() > kInt_256U) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kInvalidInputSize);
    }
    /// @error: SecurityErrorDomain::kInsufficientCapacity   if the size of the @c wrapped buffer is not enough for
    /// storing the result
    if (key.GetPayloadSize() > kInt_1024U) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kInsufficientCapacity);
    }
    /// @error: SecurityErrorDomain::kInsufficientCapacity   if the size of the @c wrapped buffer is not enough for
    /// storing the result
    return DoWrapKeyMaterial(key);
}

}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara