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
/// @file       certificate.cpp
/// @brief      AutoSar-Crypto Certificate management module
/// @details
/// @date       2021-12-21
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Certificate component/X.509 certificate
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_03001
/// @unit_name=Certificate
/// @unit_description=Certificate base class
/// @endcode
///
/// ================================================================

#include "ara/crypto/x509/certificate.h"

#include <openssl/asn1.h>
#include <openssl/asn1t.h>
#include <openssl/crypto.h>
#include <openssl/evp.h>
#include <openssl/ossl_typ.h>
#include <openssl/pem.h>
#include <openssl/x509.h>
#include <openssl/x509v3.h>

#include <chrono>

#include "ara/crypto/common/entry_point.h"
#include "ara/crypto/common/isoft_common_api.h"
#include "ara/crypto/cryp/cryobj/isoft_key_public.h"
#include "ara/crypto/openssl/isoft_openssl_encrypt.h"
#include "ara/crypto/x509/isoft_certificate.h"
#include "ara/crypto/x509/x509_dn.h"
#include "ara/crypto/x509/x509_provider.h"
#include "ara/crypto/x509/x509_public_key_info.h"

namespace ara {
namespace crypto {
namespace x509 {
namespace {
/// @brief String conversion
/// @param pOctetString ANSI format string
/// @return String
ara::core::Vector< ara::core::Byte > Asn1ToVecByte(ASN1_OCTET_STRING const *const pOctetString) noexcept
{
    ara::core::Vector< ara::core::Byte > vecData;
    try {
        vecData.reserve(static_cast< std::size_t >(pOctetString->length));
    } catch (std::bad_alloc &) {
        return vecData;
    }
    for (int32_t i{0}; i < pOctetString->length; i++) {
        vecData.push_back(ara::core::Byte(*(pOctetString->data + i)));
    }
    return vecData;
}
}  // namespace
/// @brief X.509 certificate interface.
/// @brief Get the X.509 version of this certificate object.
/// @return  X.509 version
std::uint32_t Certificate::X509Version() const noexcept
{
    int32_t const nVersion{static_cast< int32_t >(X509_get_version(pX509_))};
    return static_cast< uint32_t >(nVersion) + 1U;
}
/// @brief Check whether this certificate belongs to a root CA.
/// @return @c true if the TrustMaster has set this certificate as root
bool Certificate::IsRoot() const noexcept
{
    // Determine IssuerDn == SubjectDn
    X509DN const &dnIssuer{IssuerDn()};
    X509DN const &dnSubject{SubjectDn()};
    return dnSubject == dnIssuer;
}
/// @brief Get the issuer certificate DN.
/// @return Issuer DN of this certificate
X509DN const &Certificate::IssuerDn() const noexcept
{
    X509_NAME *const pCommonName{X509_get_issuer_name(pX509_)};
    if (nullptr == pCommonName) {
        return x509DnIssuer_;
    }
    std::ignore = x509DnIssuer_.SetX509Name(pCommonName);
    return x509DnIssuer_;
}
/// @brief Get the "Not Before" of the certificate.
/// @return "Not Before" of the certificate
time_t Certificate::StartTime() const noexcept
{
    ASN1_TIME *const pAsn1Time{X509_get_notBefore(pX509_)};
    if (nullptr == pAsn1Time) {
        return 0;
    }
    std::tm tmData{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, nullptr};
    std::ignore = memset(&tmData, 0, sizeof(tmData));
    std::ignore = ASN1_TIME_to_tm(pAsn1Time, &tmData);
    time_t const endTime{std::mktime(&tmData)};
    if (tmData.tm_isdst == 0) {
    }
    return endTime;
}
/// @brief Get the "Not After" of the certificate.
/// @return "Not After" of the certificate
time_t Certificate::EndTime() const noexcept
{
    ASN1_TIME *const pAsn1Time{X509_get_notAfter(pX509_)};
    if (nullptr == pAsn1Time) {
        return 0;
    }
    std::tm tmData{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, nullptr};
    std::ignore = memset(&tmData, 0, sizeof(tmData));
    std::ignore = ASN1_TIME_to_tm(pAsn1Time, &tmData);
    time_t const endTime{std::mktime(&tmData)};
    if (tmData.tm_isdst == 0) {
    }
    return endTime;
}
/// @brief Get the serial number of this certificate. If (sn.empty() == true), this method returns only the required output buffer size.
/// @name  SerialNumber Serial number
/// @returns size of the certificate serial number in bytes
ara::core::Result< ara::core::Vector< ara::core::Byte > > Certificate::SerialNumber() const noexcept
{
    if (nullptr == pX509_) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kResourceFault);
    }
    ASN1_INTEGER const *const pAsn1Integer{X509_get0_serialNumber(pX509_)};
    if (nullptr == pAsn1Integer) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kResourceFault);
    }
    BIGNUM *const pBigNum{ASN1_INTEGER_to_BN(pAsn1Integer, nullptr)};
    if (nullptr == pBigNum) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kResourceFault);
    }
    /// @error: SecurityErrorDomain::kInsufficientCapacity   if (sn.empty() == false), but its size is not enough for
    /// storing the output value
    int32_t const nSize{BN_num_bytes(pBigNum)};
    uint8_t *const pData{new uint8_t[static_cast< uint64_t >(nSize)]};
    int32_t const nLen{BN_bn2bin(pBigNum, pData)};
    BN_free(pBigNum);
    if (nLen != nSize) {
        delete[] pData;
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kResourceFault);
    }
    ara::core::Vector< ara::core::Byte > vecData;
    try {
        vecData.reserve(static_cast< std::size_t >(nSize));
    } catch (std::bad_alloc &) {
        delete[] pData;
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kInsufficientCapacity);
    }
    for (int32_t i{0}; i < nSize; i++) {
        vecData.push_back(ara::core::Byte(*(pData + i)));
    }
    delete[] pData;
    return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromValue(vecData);
}
/// @brief Get the DER-encoded AuthorityKeyIdentifier of this certificate. If (id.empty() ==
/// @return size of the DER encoded AuthorityKeyIdentifier in bytes
ara::core::Result< ara::core::Vector< ara::core::Byte > > Certificate::AuthorityKeyId() const noexcept
{
    // X509v3 Authority Key Identifier:
    /// @error: SecurityErrorDomain::kInsufficientCapacity   if (id.empty() == false), but its size is not enough for
    /// storing the output value
    ara::core::Vector< ara::core::Byte > vecData;
    std::ignore
        = _DealExtDataByID(NID_authority_key_identifier, [&vecData](void const *const pVoidData) noexcept -> bool {
              AUTHORITY_KEYID const *const pAuthorityKeyID{static_cast< AUTHORITY_KEYID const * >(pVoidData)};
              vecData = Asn1ToVecByte(pAuthorityKeyID->keyid);

              return true;
          });
    if (vecData.empty()) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kInsufficientCapacity);
    }
    return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromValue(vecData);
}
/// @brief Get the DER-encoded SubjectKeyIdentifier of this certificate. If (id.empty() == true), this method returns only the required output buffer size.
/// @returns size of the DER encoded SubjectKeyIdentifier in bytes
ara::core::Result< ara::core::Vector< ara::core::Byte > > Certificate::SubjectKeyId() const noexcept
{
    // X509v3 Subject Key Identifier:
    /// @error: SecurityErrorDomain::kInsufficientCapacity   if (id.empty() == false), but its size is not enough for
    /// storing the output value
    ara::core::Vector< ara::core::Byte > vecData;
    std::ignore
        = _DealExtDataByID(NID_subject_key_identifier, [&vecData](void const *const pVoidData) noexcept -> bool {
              ASN1_OCTET_STRING const *const pExtData{static_cast< ASN1_OCTET_STRING const * >(pVoidData)};
              vecData = Asn1ToVecByte(pExtData);

              return true;
          });
    if (vecData.empty()) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kInsufficientCapacity);
    }
    return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromValue(vecData);
}
/// @brief Verify the certificate signature. A call with the parameter (caCert == nullptr) applies only to root CA certificates.
/// @brief Verify signature of the certificate.
///       Call with (caCert == nullptr) is applicable only if this is a certificate of a root CA.
/// @param caCert  the optional pointer to a Certification Authority certificate used for signature of the current
/// one
/// Optional pointer to the certificate authority certificate used to sign the current certificate
/// @returns @c true if this certificate was verified successfully and @c false otherwise
/// @trace_id_sws={SWS_CRYPT_40219}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02306}
/// @threadsafety={Thread-safe}
bool Certificate::VerifyMe(ara::core::Optional< Certificate const > const &caCert) const noexcept
{
    // Ensure that the root certificate is secure and trustworthy
    if (false == caCert.has_value()) {
        return IsRoot();
    }
    Certificate const *const pCertOther{caCert.operator->()};
    // Verify non-root certificates
    X509 *const pIssuerX509{pCertOther->GetX509()};
    if (nullptr == pIssuerX509) {
        return false;
    }
    EVP_PKEY *const pKeyIssuer{X509_get0_pubkey(pIssuerX509)};
    if (nullptr == pKeyIssuer) {
        return false;
    }
    int32_t const nResult{
        X509_verify(pX509_, pKeyIssuer)};  // Returns 1 on success, otherwise 0 // Corresponds to X509_sign
    return nResult > 0;
}
/// @brief Calculate the fingerprint from the entire certificate. The generated fingerprint value is saved to the output buffer starting from the leading byte of the hash value.
///         If the capacity of the output buffer is less than the digest size, the digest will be truncated, retaining only the leading bytes.
///         If the capacity of the output buffer is greater than the digest size, only the leading bytes of the buffer will be updated.
/// @brief Calculate a fingerprint from the whole certificate.
///       The produced fingerprint value saved to the output buffer starting from leading bytes of the hash value.
///       If the capacity of the output buffer is less than the digest size then the digest will be truncated and only
///       leading bytes will be saved. If the capacity of the output buffer is higher than the digest size then only
///       leading bytes of the buffer will be updated.
/// @param fingerprint  output buffer for the fingerprint storage
/// @param hashCtx  an initialized hash function context
/// @returns number of bytes actually saved to the output buffer
/// @trace_id_sws={SWS_CRYPT_40220}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02306}
/// @error: SecurityErrorDomain::kIncompleteArgState  if the @c hashCtx context is not initialized
/// @threadsafety={Thread-safe}
ara::core::Result< std::size_t > Certificate::GetFingerprint(ReadWriteMemRegion const &fingerprint,
                                                             cryp::HashFunctionCtx &hashCtx) const noexcept
{
    /// @error: SecurityErrorDomain::kIncompleteArgState  if the @c hashCtx context is not initialized
    if (false == hashCtx.IsInitialized()) {
        return ara::core::Result< std::size_t >::FromError(SecurityErrorDomain::Errc::kIncompleteArgState);
    }
    // The content involved in the hash calculation is the entire X509 content (all content in DER format)
    ara::core::Vector< ara::core::Byte > vecData;
    if (false == _ExportCert_Der(vecData)) {
        return ara::core::Result< std::size_t >::FromError(SecurityErrorDomain::Errc::kLogicFault);
    }
    ara::core::Byte *const pData{vecData.data()};
    ReadOnlyMemRegion const readMem{static_cast< uint8_t const * >(static_cast< void const * >(pData)), vecData.size()};
    std::ignore = hashCtx.Update(readMem);
    ara::core::Result< ara::core::Vector< ara::core::Byte > > const result{hashCtx.Finish()};
    if (false == result.HasValue()) {
        return ara::core::Result< std::size_t >::FromError(SecurityErrorDomain::Errc::kLogicFault);
    }
    ara::core::Vector< ara::core::Byte > vecHashResult{result.Value()};
    int32_t const nCopyLen{
        std::min< int32_t >(static_cast< int32_t >(fingerprint.size()), static_cast< int32_t >(vecHashResult.size()))};
    for (int32_t i{0}; i < nCopyLen; i++) {
        fingerprint[static_cast< size_t >(i)] = static_cast< uint8_t >(vecHashResult[static_cast< size_t >(i)]);
    }
    return ara::core::Result< std::size_t >::FromValue(nCopyLen);
}
/// @brief Return the last verification status of the certificate.
/// @return the certificate verification status
inline Certificate::Status Certificate::GetStatus() const noexcept { return statusLast_; }
//********************************/ //Serializable interface
/// @brief Publicly serialize itself.
/// @brief Serialize itself publicly.
/// @param formatId  the Crypto Provider specific identifier of the output format
/// @returns a buffer with the serialized object
/// @trace_id_sws={SWS_CRYPT_10711}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02112}
/// @error: SecurityErrorDomain::kInsufficientCapacity   if (output.empty() == false), but it's capacity is less than
/// required
/// @error: SecurityErrorDomain::kUnknownIdentifier      if an unknown format ID was specified
/// @error: SecurityErrorDomain::kUnsupportedFormat      if the specified format ID is not supported for this object
/// type
/// @threadsafety={Thread-safe}
ara::core::Result< ara::core::Vector< ara::core::Byte > > Certificate::ExportPublicly(FormatId formatId) const noexcept
{
    /// @error: SecurityErrorDomain::kInsufficientCapacity   if (output.empty() == false), but it's capacity is less
    /// than required
    /// @error: SecurityErrorDomain::kUnknownIdentifier      if an unknown format ID was specified
    if (false == isoft_def::PCertificate::IsValidFormatID(formatId, false)) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kUnknownIdentifier);
    }
    /// @error: SecurityErrorDomain::kUnsupportedFormat      if the specified format ID is not supported for this object
    /// type
    if (false == isoft_def::PCertificate::IsValidFormatID(formatId, true)) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kUnsupportedFormat);
    }
    std::function< int32_t(BIO *, FunctionId) > const func{[this](BIO *const pbio, FunctionId const funcId) -> int32_t {
        if (funcId == kFuncDer) {
            int32_t const ret{i2d_X509_bio(pbio, pX509_)};
            return ret;
        }
        if (funcId == kFuncPem) {
            int32_t const ret{PEM_write_bio_X509(pbio, pX509_)};
            return ret;
        }
        return 0;
    }};
    return ExportPublic_Fun(formatId, func);
}
//********************************/ //BasicCertInfo interface
/// @brief Load the subject public key information object into the realm of the specified crypto provider.
///         If (cryptoProvider == nullptr), the X509PublicKeyInfo object will be loaded into the realm of the stack-default crypto provider.
/// @brief Load the subject public key information object to realm of specified crypto provider.
///         If (cryptoProvider == nullptr) then @c X509PublicKeyInfo object will be loaded in realm of the Stack-default
///         Crypto Provider.
/// @param cryptoProvider Crypto provider, unique pointer of a target crypto provider, where the public key will be used
/// @returns constant reference of the subject public key interface
/// @trace_id_sws={SWS_CRYPT_40111}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02306}
/// @threadsafety={Thread-safe}
X509PublicKeyInfo const &Certificate::SubjectPubKey(cryp::CryptoProvider *cryptoProvider) const noexcept
{
    std::ignore = cryptoProvider;
    // Directly convert to RSA
    X509_PUBKEY *const pX509Public{X509_get_X509_PUBKEY(pX509_)};
    if (nullptr == pX509Public) {
        return x509PublicKey_;
    }

    int32_t const nHashNid{X509_get_signature_nid(pX509_)};
    CryptoAlgId const nHashAlgId{Trans2AlgId(static_cast< uint32_t >(nHashNid))};
    uint32_t const nHashSize{TransHashLen(nHashNid)};

    EVP_PKEY *const pKeyPublic{X509_get0_pubkey(pX509_)};
    uint32_t const nKeyLen{static_cast< uint32_t >(EVP_PKEY_size(pKeyPublic))};
    uint32_t const nSignatureSize{
        nKeyLen};  // 2022-06-01 This value is still incorrect (20230808 Chang Zheng: The signature size should be the same as the private key length; this place should be correct)
    std::ignore = x509PublicKey_.UpdatePubkeyData(pX509Public, nHashAlgId, nHashSize, nSignatureSize);

    ara::core::Result< ara::crypto::cryp::PublicKey::Uptrc > const resPublicKey{x509PublicKey_.GetPublicKey()};
    if (!resPublicKey.HasValue()) {
        return x509PublicKey_;
    }
    ara::crypto::cryp::isoft_def::PKeyPublic_Base const *const pPublicKey{
        dynamic_cast< ara::crypto::cryp::isoft_def::PKeyPublic_Base const * >(resPublicKey.Value().get())};
    if (nullptr == pPublicKey) {
        return x509PublicKey_;
    }
    ara::core::Result< ara::core::Vector< ara::core::Byte > > resVec{
        pPublicKey->ExportPublicly(Serializable::kFormatPemEncoded)};
    if (!resVec.HasValue()) {
        return x509PublicKey_;
    }

    ara::core::Vector< ara::core::Byte > const vecData{std::move(resVec).Value()};
    if (vecData.empty()) {
        return x509PublicKey_;
    }
    ara::core::String const pubData{T_TransChar(&vecData[0U]), vecData.size()};

    ara::core::Result< ara::core::Vector< ara::core::Byte > > resSerialNumber{SerialNumber()};
    if (!resSerialNumber.HasValue()) {
        return x509PublicKey_;
    }

    ara::core::Vector< ara::core::Byte > vecSerialNumber{std::move(resSerialNumber).Value()};
    if (vecSerialNumber.empty()) {
        return x509PublicKey_;
    }
    ara::core::String const serialNumber{T_TransChar(&vecSerialNumber[0U]), vecSerialNumber.size()};

    if (cryptoProvider == nullptr) {
        ara::core::InstanceSpecifier const iSpecify{ara::core::StringView{"isoft"}};
        LoadCryptoProvider(iSpecify)->InsertSubjectPubKey(serialNumber, pubData);
    } else {
        cryptoProvider->InsertSubjectPubKey(serialNumber, pubData);
    }

    return x509PublicKey_;
}
/// @brief Get the subject DN.
/// @return subject DN
X509DN const &Certificate::SubjectDn() const noexcept
{
    X509_NAME *const pCommonName{X509_get_subject_name(pX509_)};
    if (nullptr == pCommonName) {
        return x509DnSubject_;
    }
    std::ignore = x509DnSubject_.SetX509Name(pCommonName);
    return x509DnSubject_;
}
/// @brief Check whether the CA attribute of the X509v3 Basic Constraints extension is true (i.e., pathlen=0).
/// @return
bool Certificate::IsCa() const noexcept  // basicConstraints=critical,CA:true,pathlen:1
{
    if (nullptr == pX509_) {
        return false;
    }
    int32_t const nData{X509_check_ca(pX509_)};
    return nData >= 1;
}
/// @brief Get the path length constraint defined in the basic constraints extension.
/// @return
std::uint32_t Certificate::GetPathLimit() const noexcept  // basicConstraints=critical,CA:true,pathlen:1
{
    int32_t nPathLen{static_cast< int32_t >(X509_get_pathlen(pX509_))};
    if (nPathLen != 0) {
        nPathLen += 0;
    }
    std::ignore = _DealExtDataByID(NID_basic_constraints, [&nPathLen](void const *const pVoidData) -> bool {
        BASIC_CONSTRAINTS const *const pExtData{static_cast< BASIC_CONSTRAINTS const * >(pVoidData)};
        if (pExtData->ca != 0) {
            nPathLen = static_cast< int32_t >(ASN1_INTEGER_get(pExtData->pathlen));
        } else {
            nPathLen = 0;
        }
        return true;
    });
    return static_cast< std::uint32_t >(nPathLen);
}
/// @brief Get the key constraints (X509v3 Key Usage) of the key associated with this PKCS#10 object.
/// @return
BasicCertInfo::KeyConstraints Certificate::GetConstraints() const noexcept
{
    KeyConstraints nReturn{GetkConstrNone()};
    uint32_t const nKeyUsage{GetKeyUsage()};
    if ((nKeyUsage & static_cast< uint32_t >(KU_DIGITAL_SIGNATURE)) == static_cast< uint32_t >(KU_DIGITAL_SIGNATURE)) {
        nReturn |= GetkConstrDigitalSignature();  /// @brief This key can be used for digital signature production.
    }
    if ((nKeyUsage & static_cast< uint32_t >(KU_NON_REPUDIATION)) == static_cast< uint32_t >(KU_NON_REPUDIATION)) {
        nReturn
            |= GetkConstrNonRepudiation();  /// @brief This key can be used in situations where non-repudiation guarantees are required.
    }
    if ((nKeyUsage & static_cast< uint32_t >(KU_KEY_ENCIPHERMENT)) == static_cast< uint32_t >(KU_KEY_ENCIPHERMENT)) {
        nReturn |= GetkConstrKeyEncipherment();  /// @brief This key can be used for key encryption.
    }
    if ((nKeyUsage & static_cast< uint32_t >(KU_DATA_ENCIPHERMENT)) == static_cast< uint32_t >(KU_DATA_ENCIPHERMENT)) {
        nReturn |= GetkConstrDataEncipherment();  /// @brief This key can be used for data encryption.
    }
    if ((nKeyUsage & static_cast< uint32_t >(KU_KEY_AGREEMENT)) == static_cast< uint32_t >(KU_KEY_AGREEMENT)) {
        nReturn |= GetkConstrKeyAgreement();  /// @brief This key can be used for key agreement execution.
    }
    if ((nKeyUsage & static_cast< uint32_t >(KU_KEY_CERT_SIGN)) == static_cast< uint32_t >(KU_KEY_CERT_SIGN)) {
        nReturn |= GetkConstrKeyCertSign();  /// @brief This key can be used for certificate signing.
    }
    if ((nKeyUsage & static_cast< uint32_t >(KU_CRL_SIGN)) == static_cast< uint32_t >(KU_CRL_SIGN)) {
        nReturn |= GetkConstrCrlSign();  /// @brief This key can be used for certificate revocation list (CRL) signing.
    }
    if ((nKeyUsage & static_cast< uint32_t >(KU_ENCIPHER_ONLY)) == static_cast< uint32_t >(KU_ENCIPHER_ONLY)) {
        nReturn |= GetkConstrEncipherOnly();  /// @brief The encryption key can only be used for encryption.
    }
    if ((nKeyUsage & static_cast< uint32_t >(KU_DECIPHER_ONLY)) == static_cast< uint32_t >(KU_DECIPHER_ONLY)) {
        nReturn |= GetkConstrDecipherOnly();  /// @brief The encryption key can only be used for decryption.
    }
    return nReturn;
}
//********************************/
/// @brief Constructor
/// @param x509Provider Certificate provider
Certificate::Certificate(X509Provider &x509Provider) noexcept
    // : Certificate(x509Provider)  // If adopting the Certificate inheritance scheme, this is where it goes
    : BasicCertInfo{x509Provider}
    , pX509_{nullptr}
    , bTrustRoot_{false}
    , bTrustCert_{false}
    , x509DnSubject_{x509Provider}
    , x509DnIssuer_{x509Provider}
    , x509PublicKey_{}  // NOLINT
    , statusLast_{Status::kUnknown}
    , stCertFileName_{}  // NOLINT
    , stCertSlotName_{}  // NOLINT
{
}
/// @brief Constructor
/// @param other Another object instance of this class, the other Certificate
Certificate::Certificate(Certificate const &other) noexcept
    : BasicCertInfo{other.MyProvider()}
    , pX509_{other.pX509_}
    , bTrustRoot_{other.bTrustRoot_}
    , bTrustCert_{other.bTrustCert_}
    , x509DnSubject_{other.MyProvider()}
    , x509DnIssuer_{other.MyProvider()}
    , x509PublicKey_{other.x509PublicKey_}
    , statusLast_{other.statusLast_}
    , stCertFileName_{other.stCertFileName_}
    , stCertSlotName_{other.stCertSlotName_}
{
}

/// @brief Destructor
Certificate::~Certificate() noexcept { _DeleteX509(); }
/// @brief Associate X509
/// @param pX509 Standard format certificate
/// @return ture if attach sucess false otherwise
bool Certificate::AttachX509(X509 *const pX509) noexcept
{
    _DeleteX509();
    pX509_ = pX509;
    return true;
}
/// @brief Set the trust root: returns whether the setting was successful
/// @param bTrustRoot Whether to set as trust root
/// @return true if set sucess false otherwise
bool Certificate::SetAsRootOfTrust(bool const bTrustRoot) const noexcept
{
    if (bTrustRoot) {
        SetTrust(true);  // Being a trust root necessarily means it is a trusted certificate
        if (false == IsCa()) {
            return false;
        }
    }
    bTrustRoot_ = bTrustRoot;
    return true;
}
/// @brief Check whether the time is within the validity period of the certificate
/// @return ture is Volatile false otherwise
bool Certificate::IsVolatileStorage() const noexcept { return stCertFileName_.empty(); }
/// @brief Check whether the time is within the validity period of the certificate
/// @param tmCheck Certificate validity period
/// @return ture if time valid flase otherwise
bool Certificate::CheckTimeValid(time_t const tmCheck) const noexcept
{
    time_t const tmStart{StartTime()};
    time_t const tmEnd{EndTime()};
    return (tmStart <= tmCheck) && (tmCheck <= tmEnd);
}
/// @brief Load a certificate from a file
/// @name   LoadCertFile
/// @param stFileName Certificate file
/// @param nFormatID Certificate encoding format
/// @returns  ture load cert sucess false otherwise
bool Certificate::LoadCertFile(ara::core::StringView const &stFileName, FormatId const nFormatID) noexcept
{
    X509 *pX509{nullptr};
    if ((nullptr == pX509) && ((kFormatDefault == nFormatID) || (kFormatRawValueOnly == nFormatID))) {
        pX509 = LoadCertFile_Raw(stFileName);
    }
    if ((nullptr == pX509) && ((kFormatDefault == nFormatID) || (kFormatDerEncoded == nFormatID))) {
        pX509 = LoadCertFile_Der(stFileName);
    }
    if ((nullptr == pX509) && ((kFormatDefault == nFormatID) || (kFormatPemEncoded == nFormatID))) {
        pX509 = LoadCertFile_Pem(stFileName);
    }
    if (nullptr == pX509) {
        pX509 = LoadCertFile(stFileName);
    }
    if (nullptr == pX509) {
        std::ignore = fprintf(stderr, "unable to parse certificate in: %s\n", stFileName.data());
        return false;
    }
    return AttachX509(pX509);
}
/// @brief Load a certificate from memory
/// @param memCert Certificate memory data
/// @param nFormatID Certificate encoding format
/// @return ture load cert sucess false otherwise
bool Certificate::LoadCertFile(ReadOnlyMemRegion const &memCert, FormatId const nFormatID) noexcept
{
    X509 *pX509{nullptr};
    if ((nullptr == pX509) && ((kFormatDefault == nFormatID) || (kFormatRawValueOnly == nFormatID))) {
        uint8_t const *pData{memCert.data()};
        // 2022-06-13 hanjingjing: Highly suspect this usage is incorrect (2023-08-01 Chang Zheng: d2i_X509 also expects DER format data)
        pX509 = d2i_X509(nullptr, &pData, static_cast< int64_t >(memCert.size()));
    }
    if ((nullptr == pX509) && ((kFormatDefault == nFormatID) || (kFormatDerEncoded == nFormatID))) {
        BIO *const pBio{BIO_new_mem_buf(memCert.data(), static_cast< int32_t >(memCert.size()))};
        pX509       = d2i_X509_bio(pBio, nullptr);
        std::ignore = BIO_free(pBio);
    }
    if ((nullptr == pX509) && ((kFormatDefault == nFormatID) || (kFormatPemEncoded == nFormatID))) {
        /// The pBio here cannot be placed outside. When nFormatID is kFormatDefault and the data is in PEM format, pBio will change after the d2i_X509_bio function executes.
        BIO *const pBio{BIO_new_mem_buf(memCert.data(), static_cast< int32_t >(memCert.size()))};
        pX509       = PEM_read_bio_X509(pBio, nullptr, nullptr, nullptr);
        std::ignore = BIO_free(pBio);
    }
    if (nullptr == pX509) {
        std::ignore = fprintf(stderr, "unable to parse certificate in: ReadOnlyMemRegion\n");
        return false;
    }
    return AttachX509(pX509);
}
/// @brief Get the persistent storage file name; volatile storage returns empty
/// @name  GetStorageFile
/// @returns ture load cert sucess false otherwise
ara::core::StringView Certificate::GetStorageFile() const noexcept { return T_StringView(stCertFileName_); }
/// @brief Set the persistent storage file name
/// @name   SetCertSlotName
/// @param stSlotName Key slot name
void Certificate::SetCertSlotName(ara::core::StringView const &stSlotName) noexcept { stCertSlotName_ = stSlotName; }
/// @brief Get the name of the certificate: the name inside the JSON configuration file
/// @name  GetCertSlotName
/// @returns Certificate slot name
ara::core::StringView Certificate::GetCertSlotName() const noexcept { return T_StringView(stCertSlotName_); }
/// @brief Return a clone of itself
/// @name  CloneSelf
/// @return Certificate instance object
Certificate::Uptr Certificate::CloneSelf() const noexcept
{
    Certificate::Uptr pReturn{std::make_unique< Certificate >(MyProvider())};
    X509 *const pOldX509{GetX509()};
    X509 *pNewX509{nullptr};
    if (pOldX509 != nullptr) {
        pNewX509 = X509_dup(GetX509());
    }
    std::ignore              = pReturn->AttachX509(pNewX509);
    pReturn->bTrustRoot_     = bTrustRoot_;
    pReturn->bTrustCert_     = bTrustCert_;
    pReturn->statusLast_     = statusLast_;
    pReturn->stCertFileName_ = stCertFileName_;
    return pReturn;
}
//********************************/
/// @brief Get the extended attribute "Key Usage"
/// @name  GetKeyUsage
/// @return Key usage
uint32_t Certificate::GetKeyUsage() const noexcept
{
    // The return value is values such as KU_DIGITAL_SIGNATURE, etc., as defined by OpenSSL
    int32_t nKeyUsage{0};
    std::ignore = _DealExtDataByID(NID_key_usage, [&nKeyUsage](void const *const pVoidData) noexcept -> bool {
        ASN1_BIT_STRING const *const pExtData{static_cast< ASN1_BIT_STRING const * >(pVoidData)};
        uint32_t nReturn{*(pExtData->data + 0)};
        if (pExtData->length > 1) {
            nReturn |= static_cast< uint32_t >(static_cast< uint32_t >(*(pExtData->data + kInt_1U)) * kInt_256U);
        }

        nKeyUsage = static_cast< int32_t >(nReturn);
        return true;
    });
    return static_cast< uint32_t >(nKeyUsage);
}
/// @brief Get the extension interface
/// @name  GetExtensions
/// @return
X509Extensions::Uptr Certificate::GetExtensions() const noexcept
{
    X509Extensions::Uptr pX509Extensions{std::make_unique< X509Extensions >(MyProvider())};
    std::ignore = pX509Extensions->UpdateExtensions(pX509_);
    return pX509Extensions;
}
/// @brief The signature algorithm of the certificate. Returns the custom AlgId defined by the Crypto module.
/// @name  GetSignatureAlgId
/// @return Custom algorithm ID
cryp::CryptoContext::AlgId Certificate::GetSignatureAlgId() const noexcept
{
    if (nullptr == pX509_) {
        return 0U;
    }
    // 2022-06-01 Test data: NID_sha1WithRSAEncryption == 65
    int32_t const nid{X509_get_signature_nid(pX509_)};
    return Trans2AlgId(static_cast< uint32_t >(nid));
}
//********************************/
/// @brief Delete X509
/// @name  _DeleteX509
void Certificate::_DeleteX509() noexcept
{
    if (pX509_ != nullptr) {
        pX509_ = nullptr;
    }
}
/// @brief Load a certificate from a file
/// @name   LoadCertFile
/// @param stFileName Certificate file
/// @returns  Certificate instance object
X509 *Certificate::LoadCertFile(ara::core::StringView const &stFileName) noexcept
{
    BIO *const pBio{BIO_new_file(stFileName.data(), "rb")};
    if (nullptr == pBio) {
        std::ignore = fprintf(stderr, "unable to open: %s\n", stFileName.data());
        return nullptr;
    }
    X509 *const pX509{PEM_read_bio_X509(pBio, nullptr, nullptr, nullptr)};
    std::ignore = BIO_free(pBio);
    return pX509;
}
/// @brief Load a certificate from raw format data
/// @name   LoadCertFile_Raw
/// @param stFileName Certificate file
/// @returns Certificate instance object
X509 *Certificate::LoadCertFile_Raw(ara::core::StringView const &stFileName) noexcept
{
    return LoadCertFile_Der(stFileName);
}
/// @brief Load a certificate from a DER file
/// @name   LoadCertFile_Der
/// @param stFileName Certificate file
/// @returns  Certificate instance object
X509 *Certificate::LoadCertFile_Der(ara::core::StringView const &stFileName) noexcept
{
    BIO *const pBio{BIO_new_file(stFileName.data(), "rb")};
    if (nullptr == pBio) {
        std::ignore = fprintf(stderr, "unable to open Der: %s\n", stFileName.data());
        return nullptr;
    }
    X509 *const pX509{d2i_X509_bio(pBio, nullptr)};
    std::ignore = BIO_free(pBio);
    return pX509;
}
/// @brief Load a certificate from a PEM format file
/// @name   LoadCertFile_Pem
/// @param stFileName Certificate file
/// @returns  Certificate instance object
X509 *Certificate::LoadCertFile_Pem(ara::core::StringView const &stFileName) noexcept
{
    BIO *const pBio{BIO_new_file(stFileName.data(), "rb")};
    if (nullptr == pBio) {
        std::ignore = fprintf(stderr, "unable to open Pem: %s\n", stFileName.data());
        return nullptr;
    }
    X509 *const pX509{PEM_read_bio_X509(pBio, nullptr, nullptr, nullptr)};
    std::ignore = BIO_free(pBio);
    return pX509;
}
/// @brief Export a certificate request in DER format
/// @name   _ExportCert_Der
/// @param vecData Buffer to receive certificate information
/// @param nType Certificate encoding format
/// @returns true if export sucess false otherwise
bool Certificate::_ExportCert_Der(ara::core::Vector< ara::core::Byte > &vecData, int32_t const nType) const noexcept
{
    vecData.clear();
    int32_t nNeedLen{0};
    // Two different methods
    switch (nType) {
        case 0: {
            BIO *const pNewBio{BIO_new(BIO_s_mem())};
            int32_t const nReturn{i2d_X509_bio(pNewBio, pX509_)};  // 2022-07-11 Tests show:
            if (nReturn <= 0) {
                return false;
            }
            nNeedLen = PH_BIO_pending(pNewBio);
            if (nNeedLen > 0) {
                vecData.resize(static_cast< std::size_t >(nNeedLen));
                std::ignore = BIO_read(pNewBio, static_cast< void * >(vecData.data()), nNeedLen);
            }
            std::ignore = BIO_free(pNewBio);
        } break;
        case 1: {
            uint8_t *pCertBuff{nullptr};
            nNeedLen = ASN1_item_i2d(static_cast< ASN1_VALUE * >(static_cast< void * >(pX509_)), &pCertBuff,
                                     ASN1_ITEM_rptr(X509));
            if (nNeedLen > 0) {
                vecData.reserve(static_cast< std::size_t >(nNeedLen));
                for (int32_t i{0}; i < nNeedLen; i++) {
                    vecData.push_back(ara::core::Byte(*(pCertBuff + i)));
                }
            }
            OPENSSL_free(pCertBuff);
        } break;
        case kInt_2: {
            nNeedLen = i2d_X509(pX509_, nullptr);
            if (nNeedLen > 0) {
                vecData.resize(static_cast< std::size_t >(nNeedLen));
                uint8_t *pDerWork{static_cast< uint8_t * >(static_cast< void * >(vecData.data()))};
                nNeedLen = i2d_X509(pX509_, &pDerWork);
            }
        } break;
        default: {
        } break;
    }
    return nNeedLen > 0;
}
/// @brief Export a certificate request in PEM format
/// @name   _ExportCert_Pem
/// @param vecData Buffer to receive certificate information
/// @returns  true if export sucess false otherwise
bool Certificate::_ExportCert_Pem(ara::core::Vector< ara::core::Byte > &vecData) const noexcept
{
    BIO *const pNewBio{BIO_new(BIO_s_mem())};
    int32_t const nReturn{PEM_write_bio_X509(pNewBio, pX509_)};
    vecData.clear();
    int32_t const nNeedLen{PH_BIO_pending(pNewBio)};
    if ((nReturn > 0) && (nNeedLen > 0)) {
        vecData.resize(static_cast< std::size_t >(nNeedLen));
        std::ignore = BIO_read(pNewBio, static_cast< void * >(vecData.data()), nNeedLen);
    }
    std::ignore = BIO_free(pNewBio);
    return nNeedLen > 0;
}
/// @brief Get public key data: RSA format
/// @name  GetPublicKeyData_Rsa
/// @returns
/// @throws
/// @param pEvpKey Key resource pointer
/// @return Public key data information
ara::core::Vector< ara::core::Byte > Certificate::GetPublicKeyData_Rsa(EVP_PKEY *const pEvpKey) noexcept
{
    ara::core::Vector< ara::core::Byte > vecData;
    if (nullptr == pEvpKey) {
        return vecData;
    }
    // Generate public key
    BIO *const pBioBuff{BIO_new(BIO_s_mem())};
    //-----BEGIN RSA PUBLIC KEY-----~-----END RSA PUBLIC KEY-----
    std::ignore = PEM_write_bio_PUBKEY(pBioBuff, pEvpKey);
    int32_t const nLen{PH_BIO_pending(pBioBuff)};
    if (nLen > 0) {
        vecData.resize(static_cast< std::size_t >(nLen));
        std::ignore = BIO_read(pBioBuff, static_cast< void * >(vecData.data()), nLen);
    }
    BIO_free_all(pBioBuff);
    return vecData;
}
/// @brief Process the obtained extension data via NID
/// @name  _DealExtDataByID
/// @param nID nID
/// @param pfunc Callback function
/// @return sucess
bool Certificate::_DealExtDataByID(int32_t const nID,
                                   std::function< bool(void const *pVoidData) > const &pfunc) const noexcept
{
    if (nullptr == pX509_) {
        return false;
    }
    int32_t nCrit{0};
    int32_t nIdx{0};
    void *pVoidData{X509_get_ext_d2i(pX509_, nID, &nCrit, &nIdx)};
    if (nullptr == pVoidData) {
        // 2022-06-24 hanjingjing: Found that some certificates require two calls to retrieve the value (reason to be investigated);
        pVoidData = X509_get_ext_d2i(pX509_, nID, &nCrit, &nIdx);
        if (nullptr == pVoidData) {
            return false;
        }
        if (nCrit == 0) {
        }  // for qac
        if (nIdx == 0) {
        }  // for qac
    }
    return pfunc(pVoidData);
}
//********************************/
}  // namespace x509
}  // namespace crypto
}  // namespace ara
