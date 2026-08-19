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
/// @file       x509_public_key_info.cpp
/// @brief      AutoSar-Crypto Certificate Management Module
/// @details
/// @date       2021-12-21
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Certificate Component/X.509 Public Key
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_03004
/// @unit_name=X509PublicKeyInfo
/// @unit_description=X.509 Public Key Information
/// @endcode
///
/// ================================================================

#include "ara/crypto/x509/x509_public_key_info.h"

#include <openssl/pem.h>

#include "ara/crypto/common/isoft_common_api.h"
#include "ara/crypto/cryp/cryobj/isoft_key_public_ecc.h"
#include "ara/crypto/cryp/cryobj/isoft_key_public_rsa.h"
#include "ara/crypto/openssl/isoft_openssl_encrypt.h"
#include "ara/crypto/x509/isoft_certificate.h"

namespace ara {
namespace crypto {
namespace x509 {
//********************************/
/// @brief ara::crypto::cryp::isoft_def::PKeyPublic_Rsa declaration
/// @code{.isoft}
/// @interface_level=unit
/// @needwork = no
/// @endcode
using PKeyPublic_Rsa = ara::crypto::cryp::isoft_def::PKeyPublic_Rsa;
/// @brief ara::crypto::cryp::isoft_def::PKeyPublic_Ecc declaration
/// @code{.isoft}
/// @interface_level=unit
/// @needwork = no
/// @endcode
using PKeyPublic_Ecc = ara::crypto::cryp::isoft_def::PKeyPublic_Ecc;
namespace {
/// @brief Initialize pPublicKey_ pointer
/// @param str ECC public key string
/// @return
/// @throws exception-object exception description
PKeyPublic_Ecc::Uptr InitPublicKeyEcc(ara::core::StringView const& str)
{
    PKeyPublic_Ecc::Uptr pPublicKeyEcc{std::make_unique< PKeyPublic_Ecc >(str)};
    pPublicKeyEcc->SetExportable(true);
    pPublicKeyEcc->SetSession(false);
    // Configure public key usage scope
    pPublicKeyEcc->SetAllowedUsage(kAllowDataEncryption | kAllowDataDecryption | kAllowSignature | kAllowVerification
                                   | kAllowKeyAgreement | kAllowKeyDiversify | kAllowRngInit);
    return pPublicKeyEcc;
}
/// @brief Initialize pPublicKey_ pointer
/// @return PKeyPublic_Rsa smart pointer
PKeyPublic_Rsa::Uptr InitPublicKeyRsa() noexcept
{
    PKeyPublic_Rsa::Uptr pPublicKeyRsa{std::make_unique< PKeyPublic_Rsa >("")};
    pPublicKeyRsa->SetExportable(true);
    pPublicKeyRsa->SetSession(false);
    // Configure public key usage scope
    pPublicKeyRsa->SetAllowedUsage(kAllowDataEncryption | kAllowDataDecryption | kAllowSignature | kAllowVerification
                                   | kAllowKeyAgreement | kAllowKeyDiversify | kAllowRngInit);
    return pPublicKeyRsa;
}
}  // namespace
//********************************/
/// @brief X.509 Public Key Information interface.
//********************************/ //Serializable   interface
/// @brief Publicly serialize itself.
/// @param formatId Data format: Raw, DER, PEM, etc.
/// @return X.509 Public Key Information
ara::core::Result< ara::core::Vector< ara::core::Byte > > X509PublicKeyInfo::ExportPublicly(
    FormatId formatId) const noexcept
{
    /// @brief Result type alias
    using PResult = ara::core::Result< ara::core::Vector< ara::core::Byte > >;
    /// @error: SecurityErrorDomain::kInsufficientCapacity   if (output.empty() == false), but it's capacity is less
    /// than required
    /// @error: SecurityErrorDomain::kUnknownIdentifier      if an unknown format ID was specified
    if (false == isoft_def::PCertificate::IsValidFormatID(formatId, false)) {
        return PResult::FromError(SecurityErrorDomain::Errc::kUnknownIdentifier);
    }
    /// @error: SecurityErrorDomain::kUnsupportedFormat      if the specified format ID is not supported for this object
    /// type
    if (false == isoft_def::PCertificate::IsValidFormatID(formatId, true)) {
        return PResult::FromError(SecurityErrorDomain::Errc::kUnsupportedFormat);
    }
    ara::core::Result< ara::crypto::cryp::PublicKey::Uptrc > const resPubilcKey{GetPublicKey()};
    if (!resPubilcKey.HasValue()) {
        return PResult::FromError(SecurityErrorDomain::Errc::kUnsupportedFormat);
    }
    cryp::isoft_def::PKeyPublic_Base const* const pKeyPublic{
        dynamic_cast< cryp::isoft_def::PKeyPublic_Base const* >(resPubilcKey.Value().get())};
    if (nullptr == pKeyPublic) {
        return PResult::FromError(SecurityErrorDomain::Errc::kUnexpectedValue);
    }
    return pKeyPublic->ExportPublicly(formatId);
}

/// @brief Get the subject's public key object.
///         The created PublicKey object is session-level and non-exportable, because generic X.509 certificates or CSRs do not have a COUID for the public key, so it can only be saved or transmitted as part of the corresponding certificate or CSR.
/// @return  unique smart pointer to the created public key of the subject
ara::core::Result< ara::crypto::cryp::PublicKey::Uptrc > X509PublicKeyInfo::GetPublicKey() const noexcept
{
    /// Created @c PublicKey object is @b session and @c non-exportable, because generic X.509 certificate or
    ///       certificate signing request (CSR) doesn't have COUID of the public key, therefore it should be saved
    ///       or transmitted only as a part of correspondent certificate or CSR.
    if (pSubjectPublicKeyInfo_ == nullptr) {
        return ara::core::Result< ara::crypto::cryp::PublicKey::Uptrc >::FromValue(nullptr);
    }
    EVP_PKEY* const pKeyPublic{X509_PUBKEY_get0(pSubjectPublicKeyInfo_)};
    if (nullptr == pKeyPublic) {
        return ara::core::Result< ara::crypto::cryp::PublicKey::Uptrc >::FromValue(nullptr);
    }

    int32_t const type{EVP_PKEY_id(pKeyPublic)};
    if (EVP_PKEY_RSA == type) {
        PKeyPublic_Rsa::Uptr pNewPublicKey{InitPublicKeyRsa()};
        BIO* const pPublic{BIO_new(BIO_s_mem())};
        int32_t const ret{PEM_write_bio_PUBKEY(pPublic, pKeyPublic)};
        if (ret < 1) {
            std::ignore = BIO_free(pPublic);
            return ara::core::Result< ara::crypto::cryp::PublicKey::Uptrc >::FromValue(nullptr);
        }

        ara::core::Vector< ara::core::Byte > vecData;
        int32_t const nNeedLen{static_cast< int32_t >(PH_BIO_pending(pPublic))};
        if (nNeedLen > 0) {
            vecData.resize(static_cast< std::size_t >(nNeedLen));
            std::ignore = BIO_read(pPublic, vecData.data(), nNeedLen);
        }
        pNewPublicKey->SetRsaKey(reinterpret_cast< const uint8_t* >(vecData.data()),
                                 static_cast< std::size_t >(nNeedLen));
        pNewPublicKey->SetSession(true);
        pNewPublicKey->SetExportable(false);
        return ara::core::Result< ara::crypto::cryp::PublicKey::Uptrc >::FromValue(std::move(pNewPublicKey));
    }
    if (EVP_PKEY_EC == type) {
        BIO* const pPublic{BIO_new(BIO_s_mem())};
        int32_t const ret{PEM_write_bio_PUBKEY(pPublic, pKeyPublic)};
        if (ret < 1) {
            std::ignore = BIO_free(pPublic);
            return ara::core::Result< ara::crypto::cryp::PublicKey::Uptrc >::FromValue(nullptr);
        }

        ara::core::Vector< ara::core::Byte > vecData;
        int32_t const nNeedLen{static_cast< int32_t >(PH_BIO_pending(pPublic))};
        if (nNeedLen > 0) {
            vecData.resize(static_cast< std::size_t >(nNeedLen));
            std::ignore = BIO_read(pPublic, vecData.data(), nNeedLen);
        }
        PKeyPublic_Ecc::Uptr pNewPublicKey{
            InitPublicKeyEcc(T_StringView(vecData.data(), static_cast< std::size_t >(nNeedLen)))};
        std::ignore = BIO_free(pPublic);
        pNewPublicKey->SetSession(true);
        pNewPublicKey->SetExportable(false);
        return ara::core::Result< ara::crypto::cryp::PublicKey::Uptrc >::FromValue(std::move(pNewPublicKey));
    }
    return ara::core::Result< ara::crypto::cryp::PublicKey::Uptrc >::FromValue(nullptr);
}
/// @brief Get the CryptoPrimitiveId instance of this class.
/// @return CryptoPrimitiveId instance
ara::crypto::cryp::CryptoPrimitiveId::Uptrc X509PublicKeyInfo::GetAlgorithmId() noexcept
{
    ara::core::Result< ara::crypto::cryp::PublicKey::Uptrc > const resPubilcKey{GetPublicKey()};
    if (!resPubilcKey.HasValue()) {
        return {nullptr};
    }
    ara::crypto::cryp::PublicKey::Uptrc const& pPublicKey{resPubilcKey.Value()};
    return {pPublicKey->GetCryptoPrimitiveId()};
}
/// @brief Verify consistency between provided and saved public keys. This method only compares public key values.
/// @param publicKey the public key object for comparison
/// @return @c true if values of the stored public key and object provided by the argument are identical and
///          @c false otherwise
bool X509PublicKeyInfo::IsSameKey(ara::crypto::cryp::PublicKey const& publicKey) const noexcept
{
    ara::core::Result< ara::crypto::cryp::PublicKey::Uptrc > const resPubilcKey{GetPublicKey()};
    if (!resPubilcKey.HasValue()) {
        return false;
    }
    ara::crypto::cryp::isoft_def::PKeyPublic_Base const* const pPublicKeyA{
        dynamic_cast< ara::crypto::cryp::isoft_def::PKeyPublic_Base const* >(resPubilcKey.Value().get())};
    if (nullptr == pPublicKeyA) {
        return false;
    }
    ara::crypto::cryp::isoft_def::PKeyPublic_Base const* const pPublicKeyB{
        dynamic_cast< ara::crypto::cryp::isoft_def::PKeyPublic_Base const* >(&publicKey)};
    if (pPublicKeyA->GetPayloadSize() != publicKey.GetPayloadSize()) {
        return false;
    }
    ara::core::Result< ara::core::Vector< ara::core::Byte > > const resultThis{pPublicKeyA->ExportPublicly()};
    if (false == resultThis.HasValue()) {
        return false;
    }
    ara::core::Result< ara::core::Vector< ara::core::Byte > > const resultOther{pPublicKeyB->ExportPublicly()};
    if (false == resultOther.HasValue()) {
        return false;
    }
    return resultThis.Value() == resultOther.Value();
}
//********************************/
/// @brief //Set public key data
/// @param pSubjectPublicKeyInfo Public key information
/// @param nHashAlgId Hash algorithm primitive ID
/// @param nHashSize Hash length
/// @param nSignatureSize Signature length
/// @return  true if update sucess false otherwise
bool X509PublicKeyInfo::UpdatePubkeyData(X509_PUBKEY* const pSubjectPublicKeyInfo,
                                         CryptoAlgId const nHashAlgId,
                                         uint32_t const nHashSize,
                                         uint32_t const nSignatureSize) noexcept  // Set public key data
{
    pSubjectPublicKeyInfo_ = pSubjectPublicKeyInfo;
    nHashAlgId_            = nHashAlgId;
    nHashSize_             = nHashSize;
    nSignatureSize_        = nSignatureSize;
    return true;
}
//********************************/
}  // namespace x509
}  // namespace crypto
}  // namespace ara
