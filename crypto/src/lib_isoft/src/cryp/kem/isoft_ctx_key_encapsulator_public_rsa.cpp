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
/// @file       isoft_ctx_key_encapsulator_public_rsa.cpp
/// @brief      AutoSar-Crypto encryption/decryption module
/// @details
/// @date       2022-04-24
/// @author     Chang Zheng
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/DefaultEncryptionDecryption/KeyEncapsulation
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_01015
/// @unit_name=PCtxKeyEncapsulatorPublicRsa
/// @unit_description=RSA-based public key encapsulation context
/// @endcode
///
/// ================================================================

#include "ara/crypto/cryp/kem/isoft_ctx_key_encapsulator_public_rsa.h"

#include "ara/crypto/common/isoft_data_type.h"
#include "ara/crypto/cryp/cryobj/isoft_crypto_primitive_id.h"
#include "ara/crypto/cryp/isoft_crypto_provider.h"
#include "ara/crypto/cryp/isoft_service_extension.h"
#include "ara/crypto/cryp/symmetric/isoft_ctx_symmetric_block_aes.h"
#include "ara/crypto/cryp/wrap/isoft_ctx_symmetric_key_wrapper_aes_pad.h"
#include "ara/crypto/cryp/wrap/isoft_ctx_symmetric_key_wrapper_des.h"
#include "ara/crypto/openssl/isoft_openssl_encrypt.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {

/// @brief Parameterized constructor
/// @param cryptoProvider crypto provider
/// @qac Suspected unable to modify [4268]: Initialization helper can be replaced with a delegator.
PCtxKeyEncapsulatorPublicRsa::PCtxKeyEncapsulatorPublicRsa(PCryptoProvider &cryptoProvider) noexcept
    : PCtxKeyEncapsulatorPublic{cryptoProvider}
{
    pCtxEncryptorPublicRsa_ = std::make_unique< PCtxEncryptorPublic_Rsa >(cryptoProvider);
}

/// @brief Get ExtensionService instance.
/// @returns ExtensionService instance
ExtensionService::Uptr PCtxKeyEncapsulatorPublicRsa::GetExtensionService() const noexcept
{
    return {std::make_unique< PServiceExtension< PCtxKeyEncapsulatorPublicRsa > >(*this)};
}
/// @brief Get the key entropy (bit length) of the key (KEK) material.
/// For RSA systems, the return value corresponds to the modulus length N(-1); for DH-like systems, the return value corresponds to the modulus length q(-1).
/// @returns entropy of the KEK material in bits
std::size_t PCtxKeyEncapsulatorPublicRsa::GetKekEntropy() const noexcept { return nKekEntropy_; }
/// @brief Add content (payload) to be encapsulated according to RFC 5990 ("GetEncapsulatedSize() bytes of the output buffer should be updated by this
/// method"). Currently only SymmetricKey and SecretSeed objects are supported.
/// @param keyingData payload to encapsulate
/// @returns  has vlaue if AddKeyingData sucess false otherwise
ara::core::Result< void > PCtxKeyEncapsulatorPublicRsa::AddKeyingData(RestrictedUseObject &keyingData) noexcept
{
    nEncapsulatedSize_ = keyingData.GetPayloadSize();
    pkeyingData_       = &keyingData;
    return ara::core::Result< void >::FromValue();
}

/// @brief Encapsulate the key encryption key (KEK).
/// @param kdf KDF algorithm context object
/// @param kekAlgId KEK algorithm crypto primitive ID
/// @param salt Salt data
/// @param ctxLabel Context label
/// @return unique smart pointer to a symmetric key object derived from a randomly generated material
/// encapsulated to the output buffer
ara::core::Result< ara::core::Vector< ara::core::Byte > > PCtxKeyEncapsulatorPublicRsa::Encapsulate(
    KeyDerivationFunctionCtx &kdf,
    AlgId kekAlgId,
    ReadOnlyMemRegion const &salt,
    ReadOnlyMemRegion const &ctxLabel) const noexcept
{
    if (!IsInitialized()) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kUninitializedContext);
    }

    PCryptoProvider *const pCryptoProvider{dynamic_cast< PCryptoProvider * >(&MyProvider())};
    if (pCryptoProvider == nullptr) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kInvalidArgument);
    }
    ara::core::Result< ara::core::String > resAlgName{pCryptoProvider->ConvertToAlgName(kekAlgId)};
    if (!resAlgName.HasValue()) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kInvalidArgument);
    }

    PKeyPublic_Rsa const *const pPublicKeyRsa{
        dynamic_cast< PKeyPublic_Rsa const * >(pCtxEncryptorPublicRsa_->GetPublicKey())};
    if (pPublicKeyRsa == nullptr) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromValue();
    }
    ara::core::Vector< u_char > vecRandomByteC;
    vecRandomByteC.resize(kInt_1024U);
    u_char *const randomByteC{vecRandomByteC.data()};

    ara::core::Vector< u_char > vecRandomByteZ;
    vecRandomByteZ.resize(kInt_1024U);
    u_char *const randomByteZ{vecRandomByteZ.data()};

    size_t retlenC{0U};
    size_t retlenZ{0U};
    std::ignore = Rsasve_generate_evp(pPublicKeyRsa->GetRsa(), static_cast< uint8_t * >(randomByteC), &retlenC,
                                      static_cast< uint8_t * >(randomByteZ), &retlenZ);
    // Use the underlying key derivation function to derive the key encryption key KEK of length kekLen bytes from the byte string Z
    // Reset data
    if (kdf.Reset().HasValue() == false) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kInvalidArgument);
    }
    // Add salt
    std::ignore = kdf.AddSalt(salt);

    // Set iteration count
    if (kdf.ConfigIterations(kInt_2U) == 0U) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kInvalidArgument);
    }

    ara::core::String const strAlgName{std::move(resAlgName).Value()};
    if (strAlgName.empty()) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kInvalidArgument);
    }
    ReadOnlyMemRegion const kekId{T_TransBytes(strAlgName.data()), strAlgName.size()};
    if (kdf.Init(kekId, kAlgIdAny, kAllowDataEncryption | kAllowDataDecryption | kAllowKeyExporting | kAllowKdfMaterial,
                 ctxLabel)
            .HasValue()
        == false) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kInvalidArgument);
    }

    // Initialize KDF configuration
    AlgId kdfKeyMaterialAlgId{static_cast< AlgId >(EPhCtxTypeID::kUnDefine)};
    size_t const nKeyLen{kdf.GetTargetKeyBitLength()};
    bool aesKey{false};
    CryptoPrimitiveId::AlgId const kdfAlgid{kdf.GetCryptoPrimitiveId()->GetPrimitiveId()};

    switch (kdfAlgid) {
        case static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kKdf_Aes):
        case static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kKdf_Aes128): {
            aesKey              = true;
            kdfKeyMaterialAlgId = static_cast< AlgId >(EPhCtxTypeID::kSymmetricAesKey128);
        } break;
        case static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kKdf_Aes192): {
            aesKey              = true;
            kdfKeyMaterialAlgId = static_cast< AlgId >(EPhCtxTypeID::kSymmetricAesKey192);
        } break;
        case static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kKdf_Aes256): {
            aesKey              = true;
            kdfKeyMaterialAlgId = static_cast< AlgId >(EPhCtxTypeID::kSymmetricAesKey256);
        } break;
        case static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kKdf_Des): {
            kdfKeyMaterialAlgId = static_cast< AlgId >(EPhCtxTypeID::kSymmetricDesKey);
        } break;
        case static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kKdf_3Des): {
            kdfKeyMaterialAlgId = static_cast< AlgId >(EPhCtxTypeID::kSymmetric3DesKey);
        } break;

        default: {
            break;
        }
    }
    if (kdfKeyMaterialAlgId == static_cast< AlgId >(EPhCtxTypeID::kUnDefine)) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kInvalidArgument);
    }

    if (retlenZ < nKeyLen / kInt_8U) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kInvalidArgument);
    }

    PCtxSymmetricKeyWrapper_Base::Uptr pSymmetricKeyWrapper{nullptr};
    if (aesKey) {
        pSymmetricKeyWrapper = std::make_unique< PCtxSymmetricKeyWrapperAesPad >(*pCryptoProvider);
    } else {
        pSymmetricKeyWrapper = std::make_unique< PCtxSymmetricKeyWrapperDes >(*pCryptoProvider);
    }

    ReadOnlyMemRegion const memKeyData{static_cast< uint8_t const * >(static_cast< void const * >(randomByteZ)),
                                       nKeyLen / kInt_8U};
    ara::core::Result< SymmetricKey::Uptrc > const resultSymmetric{PCryptoProvider::GenerateSymmetricKeyEx(
        kdfKeyMaterialAlgId, memKeyData,
        kAllowDataEncryption | kAllowDataDecryption | kAllowKeyExporting | kAllowKdfMaterial, false, true)};
    if (!resultSymmetric.HasValue()) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kInvalidArgument);
    }
    SymmetricKey::Uptrc const &pSymmetricKeyMaterial{resultSymmetric.Value()};
    if (kdf.SetSourceKeyMaterial(*(pSymmetricKeyMaterial.get())).HasValue() == false) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kInvalidArgument);
    }

    ara::core::Result< SymmetricKey::Uptrc > const pResultKek{kdf.DeriveKey()};
    if (!pResultKek.HasValue()) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kInvalidArgument);
    }

    // Use the underlying key wrapping scheme to wrap the key data K with the key encryption key KEK to obtain the wrapped key data WK
    SymmetricKey const *const symmetricKey{pResultKek->get()};
    nKekEntropy_ = symmetricKey->GetPayloadSize();
    ara::core::Result< void > const resKey{pSymmetricKeyWrapper->SetKey(*symmetricKey, CryptoTransform::kWrap)};
    if (!resKey.HasValue()) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kInvalidArgument);
    }

    ara::core::Result< ara::core::Vector< ara::core::Byte > > const presultByteWk{
        pSymmetricKeyWrapper->WrapKeyMaterial(*pkeyingData_)};
    if (!presultByteWk.HasValue()) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kInvalidArgument);
    }
    ara::core::Vector< ara::core::Byte > const &vecDecryptoWk{presultByteWk.Value()};

    ara::core::Vector< ara::core::Byte > resultByte;
    try {
        resultByte.reserve(static_cast< std::size_t >(retlenC + vecDecryptoWk.size()));
    } catch (std::bad_alloc &) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kInsufficientCapacity);
    }

    // Concatenate the ciphertext C and the wrapped key data WK to obtain the encrypted key data EK
    for (size_t i{0U}; i < retlenC; ++i) {
        ara::core::Byte const byData{*(randomByteC + i)};
        resultByte.push_back(byData);
    }
    for (auto const &it : vecDecryptoWk) {
        resultByte.push_back(it);
    }
    // 288 = 256(fixed) + 16(variable)
    return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromValue(resultByte);
}
/// @brief Clear the encryption context.
/// @returns  has vlaue if Reset sucess false otherwise
ara::core::Result< void > PCtxKeyEncapsulatorPublicRsa::Reset() noexcept
{
    if (pCtxEncryptorPublicRsa_ == nullptr) {
        return ara::core::Result< void >::FromValue();
    }
    return pCtxEncryptorPublicRsa_->Reset();
}
/// @brief Set (deploy) a key into the key encapsulator public algorithm context.
/// @param key Public key
/// @returns  has vlaue if SetKey sucess false otherwise
ara::core::Result< void > PCtxKeyEncapsulatorPublicRsa::SetKey(PublicKey const &key) noexcept
{
    uint32_t const nKeyAllowedUsage{key.GetAllowedUsage()};
    if (((kAllowKeyExporting & nKeyAllowedUsage) != kAllowKeyExporting)
        || ((kAllowDataEncryption & nKeyAllowedUsage) != kAllowDataEncryption)) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kUsageViolation);
    }
    if (pCtxEncryptorPublicRsa_ == nullptr) {
        return ara::core::Result< void >::FromValue();
    }
    return pCtxEncryptorPublicRsa_->SetKey(key);
}

/// @brief Get the allowed usage of this context (according to the key object properties loaded into this context).
/// If the context has not been initialized with a key object, it must return 0 (all flags reset).
/// @returns  a combination of bit-flags that specifies allowed usages of the context
AllowedUsageFlags PCtxKeyEncapsulatorPublicRsa::GetAllowedUsage() const noexcept
{
    if (false == IsKeyAvailable()) {
        return 0U;
    }
    return pCtxEncryptorPublicRsa_->GetAllowedUsage();
}

/// @brief Get the COUID of the key deployed to the context attached to this extended service. If no key is set for the context, return an empty COUID (Nil).
/// @returns  the COUID of the CryptoObject
CryptoObjectUid PCtxKeyEncapsulatorPublicRsa::GetActualKeyCOUID() const noexcept
{
    if (false == IsKeyAvailable()) {
        return CryptoObjectUid{};
    }
    return pCtxEncryptorPublicRsa_->GetActualKeyCOUID();
}
/// @brief Get the maximum supported key length (in bits).
/// @returns  maximal supported length of the key in bits
std::size_t PCtxKeyEncapsulatorPublicRsa::GetMaxKeyBitLength() const noexcept
{
    if (pCtxEncryptorPublicRsa_ == nullptr) {
        return 0U;
    }
    return pCtxEncryptorPublicRsa_->GetMaxKeyBitLength();
}
/// @brief Get the minimum supported key length (in bits).
/// @returns  minimal supported length of the key in bits
std::size_t PCtxKeyEncapsulatorPublicRsa::GetMinKeyBitLength() const noexcept
{
    if (pCtxEncryptorPublicRsa_ == nullptr) {
        return 0U;
    }
    return pCtxEncryptorPublicRsa_->GetMinKeyBitLength();
}
/// @brief Verify support for a specific key length according to the context.
/// @param keyBitLength Key length: in bits
/// @returns @c true if provided value of the key length is supported by the context
bool PCtxKeyEncapsulatorPublicRsa::IsKeyBitLengthSupported(std::size_t keyBitLength) const noexcept
{
    if (pCtxEncryptorPublicRsa_ == nullptr) {
        return false;
    }
    return pCtxEncryptorPublicRsa_->IsKeyBitLengthSupported(keyBitLength);
}
/// @brief Check whether a key is set for this context.
/// @returns   true if a key has been set to this context false otherwise
bool PCtxKeyEncapsulatorPublicRsa::IsKeyAvailable() const noexcept
{
    if (pCtxEncryptorPublicRsa_ == nullptr) {
        return false;
    }
    return pCtxEncryptorPublicRsa_->IsKeyAvailable();
}

//***************/
/// @brief Get the actual bit length of the key loaded into the context. If no key is set for the context, return 0.
/// @returns  actual length of a key (now set to the algorithm context) in bits
std::size_t PCtxKeyEncapsulatorPublicRsa::GetActualKeyBitLength() const noexcept
{
    if (false == IsKeyAvailable()) {
        return 0U;
    }
    return pCtxEncryptorPublicRsa_->GetActualKeyBitLength();
}

/// @brief Check whether the encryption context has been initialized and is ready for use. It checks all required values, including: key value, IV/seed, etc.
/// @returns true if already init false otherwise
bool PCtxKeyEncapsulatorPublicRsa::IsInitialized() const noexcept { return IsKeyAvailable(); }

/// @brief Return a CryptoPrimitivId instance containing the instance identifier.
/// @returns CryptoPrimitivId instance.
CryptoPrimitiveId::Uptr PCtxKeyEncapsulatorPublicRsa::GetCryptoPrimitiveId() const noexcept
{
    return {std::make_unique< PAlgId_Kem_public_rsa >()};
}

//********************************/
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara
