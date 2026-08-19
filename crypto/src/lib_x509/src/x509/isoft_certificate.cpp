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
/// @file       isoft_certificate.cpp
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
/// @unit_name=PCertificate
/// @unit_description=X.509 certificate interface
/// @endcode
///
/// ================================================================

#include "ara/crypto/x509/isoft_certificate.h"

#include <openssl/asn1.h>
#include <openssl/pem.h>
#include <openssl/x509v3.h>

#include <chrono>

#include "ara/crypto/common/entry_point.h"
#include "ara/crypto/common/isoft_log_api.h"
#include "ara/crypto/cryp/cryobj/isoft_crypto_primitive_id.h"
#include "ara/crypto/x509/x509_dn.h"
#include "ara/crypto/x509/x509_provider.h"
#include "ara/crypto/x509/x509_public_key_info.h"

namespace ara {
namespace crypto {
namespace x509 {
namespace isoft_def {
namespace {
/// @brief String conversion
/// @param pOctetString ANSI format string
/// @return String
/// @throw ???
ara::core::Vector< ara::core::Byte > Asn1ToVecByte(ASN1_OCTET_STRING const *const pOctetString)
{
    ara::core::Vector< ara::core::Byte > vecData;
    vecData.reserve(static_cast< std::size_t >(pOctetString->length));
    for (int32_t i{0}; i < pOctetString->length; ++i) {
        vecData.push_back(ara::core::Byte(*((pOctetString->data) + i)));
    }
    return vecData;
}
/// @brief Convert certificate to string
/// @param n Integer data
/// @return Character
char8_t Int2string(int32_t n) noexcept
{
    char8_t const nCharZero{'0'};
    n %= kInt_16;
    if (n >= kInt_10) {
        char8_t const nCharA{'a'};
        return static_cast< char8_t >(static_cast< int32_t >(nCharA) + (n - kInt_10));
    }
    return static_cast< char8_t >(static_cast< int32_t >(nCharZero) + n);
}
/// @brief Print string
/// @param stName Print string
/// @param pData Data: starting address of memory
/// @param nLen Data length: in bytes
void PrintfByteString(ara::core::StringView const &stName, uint8_t const *const pData, int32_t const nLen) noexcept
{
    ara::crypto::isoft_def::LogInfo() << stName.data() << " = [" << nLen << "]:0x";
    for (int32_t i{0}; i < nLen; ++i) {
        int32_t const nA{static_cast< int32_t >(*(pData + i)) / 0x10};
        int32_t const nB{static_cast< int32_t >(*(pData + i)) % 0x10};
        ara::crypto::isoft_def::LogInfo() << Int2string(nA) << Int2string(nB);
    }
}
}  // namespace
//********************************/
/// @brief X.509 certificate interface.
/// @param x509Provider  Certificate provider
PCertificate::PCertificate(X509Provider &x509Provider) noexcept
    : Certificate{x509Provider}  // If adopting the PCertificate inheritance scheme, this is where it goes
{
}
/// @brief Constructor
/// @param other Another object instance of this class, the other PCertificate
/// @throws
PCertificate::PCertificate(PCertificate const &other) noexcept : Certificate{other.MyProvider()} {}
/// @param x509Provider Certificate provider
/// @param stFileName Certificate file
/// @param nFormatID Certificate encoding format
/// @return Certificate data
ara::core::Vector< PCertificate::Uptr > PCertificate::LoadCertFile_P7b(X509Provider &x509Provider,
                                                                       ara::core::StringView const &stFileName,
                                                                       FormatId const nFormatID) noexcept
{
    BIO *const pBio{BIO_new_file(stFileName.data(), "rb")};
    ara::core::Vector< PCertificate::Uptr > vecCert;
    ara::core::Result< ara::core::Vector< PCertificate::Uptr > > resCert{LoadCertP7b(x509Provider, pBio, nFormatID)};
    if (!resCert.HasValue()) {
        return vecCert;
    }
    vecCert = std::move(resCert).Value();
    if (vecCert.empty()) {
        std::ignore = fprintf(stderr, "unable to open/parse: %s\n", stFileName.data());
    }
    std::ignore = BIO_free(pBio);
    return vecCert;
}
/// @brief Load a certificate chain
/// @param x509Provider Certificate provider
/// @param pBio Certificate chain memory information
/// @param nFormatID Certificate encoding format
/// @return Certificate data
ara::core::Result< ara::core::Vector< PCertificate::Uptr > > PCertificate::LoadCertP7b(
    X509Provider &x509Provider, BIO *const pBio, FormatId const nFormatID) noexcept
{
    if (nullptr == pBio) {
        return ara::core::Result< ara::core::Vector< PCertificate::Uptr > >::FromError(
            SecurityErrorDomain::Errc::kLogicFault);
    }
    PKCS7 *pPkcs7{nullptr};
    // The execution strategy still tries both formats
    if ((nullptr == pPkcs7) && ((kFormatDefault == nFormatID) || (kFormatDerEncoded == nFormatID))) {
        pPkcs7 = d2i_PKCS7_bio(pBio, nullptr);
    }
    if ((nullptr == pPkcs7) && ((kFormatDefault == nFormatID) || (kFormatPemEncoded == nFormatID))) {
        pPkcs7 = PEM_read_bio_PKCS7(pBio, nullptr, nullptr, nullptr);
    }
    if (nullptr == pPkcs7) {
        return ara::core::Result< ara::core::Vector< PCertificate::Uptr > >::FromError(
            SecurityErrorDomain::Errc::kLogicFault);
    }

    int32_t const nObjId{OBJ_obj2nid(pPkcs7->type)};
    STACK_OF(X509) * pCertStack{nullptr};
    if (NID_pkcs7_signed == nObjId) {
        pCertStack = pPkcs7->d.sign->cert;
    } else if (NID_pkcs7_signedAndEnveloped == nObjId) {
        pCertStack = pPkcs7->d.signed_and_enveloped->cert;
    } else {
    }
    if (nullptr == pCertStack) {
        return ara::core::Result< ara::core::Vector< PCertificate::Uptr > >::FromError(
            SecurityErrorDomain::Errc::kLogicFault);
    }
    int32_t const nCount{sk_X509_num(pCertStack)};
    ara::core::Vector< PCertificate::Uptr > vecCert;
    try {
        vecCert.reserve(static_cast< std::size_t >(nCount));
    } catch (std::bad_alloc &) {
        return ara::core::Result< ara::core::Vector< PCertificate::Uptr > >::FromError(
            SecurityErrorDomain::Errc::kInsufficientCapacity);
    }
    for (int32_t i{0}; i < nCount; ++i) {
        X509 *const pX509{sk_X509_value(pCertStack, i)};  //NOLINT
        PCertificate::Uptr pNewCert{std::make_unique< PCertificate >(x509Provider)};
        std::ignore = pNewCert->AttachX509(pX509);
        vecCert.push_back(std::move(pNewCert));
    }
    return ara::core::Result< ara::core::Vector< PCertificate::Uptr > >::FromValue(std::move(vecCert));
}
/// @brief Calculate the number of certificates in the chain
/// @param pBio  Certificate chain memory data
/// @param nFormatID Encoding format
/// @return
int32_t PCertificate::CalculateCertCount(
    BIO *const pBio, FormatId const nFormatID) noexcept  // Calculate the number of certificates in the chain
{
    if (nullptr == pBio) {
        return -1;
    }
    PKCS7 *pPkcs7{nullptr};
    // The execution strategy still tries both formats
    if ((nullptr == pPkcs7) && ((kFormatDefault == nFormatID) || (kFormatDerEncoded == nFormatID))) {
        pPkcs7 = d2i_PKCS7_bio(pBio, nullptr);
    }
    if ((nullptr == pPkcs7) && ((kFormatDefault == nFormatID) || (kFormatPemEncoded == nFormatID))) {
        pPkcs7 = PEM_read_bio_PKCS7(pBio, nullptr, nullptr, nullptr);
    }
    if (nullptr == pPkcs7) {
        return -1;
    }

    int32_t const nObjId{OBJ_obj2nid(pPkcs7->type)};
    STACK_OF(X509) * pCertStack{nullptr};
    if (NID_pkcs7_signed == nObjId) {
        pCertStack = pPkcs7->d.sign->cert;  // NOLINT
    } else if (NID_pkcs7_signedAndEnveloped == nObjId) {
        pCertStack = pPkcs7->d.signed_and_enveloped->cert;  // NOLINT
    } else {
    }
    if (nullptr == pCertStack) {
        return -1;
    }
    int32_t const nCount{sk_X509_num(pCertStack)};
    return nCount;
}
/// @brief Check whether the FormatID is supported
/// @param nFormatID  Encoding format
/// @param bCheckForWrite Check for writability
/// @return
bool PCertificate::IsValidFormatID(FormatId const nFormatID, bool const bCheckForWrite) noexcept
{
    return Serializable::IsValidFormatID(nFormatID, bCheckForWrite);
}
/// @brief Verify the certificate and modify statusLast_
/// @param cert  Certificate
/// @param myRoot Root certificate
/// @return Certificate status
Certificate::Status PCertificate::VerifyCert(Certificate const &cert, Certificate const *const myRoot) noexcept
{
    PCertificate::Status nStatus;
    if (nullptr == myRoot) {
        if (cert.IsRoot())  // Root nodes are automatically set as trusted
        {
            cert.SetTrust(true);
        }
        if (false == cert.IsTrust()) {
            nStatus = PCertificate::Status::kNotAvailable;
            cert.SetCertStatus(nStatus);
            return nStatus;
        }
    } else {
        bool const bVerify{cert.VerifyMe(*myRoot)};
        if (false == bVerify) {
            nStatus = PCertificate::Status::kInvalid;
            cert.SetCertStatus(nStatus);
            return nStatus;
        }
        if (false == cert.IsTrust()) {
            if (myRoot->IsTrust()) {
                cert.SetTrust(true);
            }
            if (false == cert.IsTrust()) {
                cert.SetTrust(false);
                nStatus = PCertificate::Status::kNotAvailable;
                cert.SetCertStatus(nStatus);
                return nStatus;
            }
        }
    }
    time_t const tmNow{std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())};
    if (tmNow > cert.EndTime()) {
        nStatus = PCertificate::Status::kExpired;
        cert.SetCertStatus(nStatus);
        return nStatus;
    }
    if (tmNow < cert.StartTime()) {
        nStatus = PCertificate::Status::kFuture;
        cert.SetCertStatus(nStatus);
        return nStatus;
    }
    nStatus = PCertificate::Status::kValid;
    cert.SetCertStatus(nStatus);
    return nStatus;
}
//***************/
/// @brief Get the "public key algorithm type"
/// @return Public key algorithm type
uint32_t PCertificate::GetPublicKeyAlgType() const noexcept
{
    if (nullptr == GetX509()) {
        return 0U;
    }
    // X509_certificate_type
    EVP_PKEY *const pEvpKey{X509_get0_pubkey(GetX509())};
    if (nullptr == pEvpKey) {
        return 0U;
    }
    return static_cast< uint32_t >(
        EVP_PKEY_base_id(pEvpKey));  // The return value of EVP_PKEY_base_id may have up to 24 types
}
/// @brief Get the "public key algorithm ID"
/// @return Public key algorithm ID
uint32_t PCertificate::GetPublicKeyAlgId() const noexcept
{
    if (nullptr == GetX509()) {
        return 0U;
    }
    EVP_PKEY *const pEvpKey{X509_get0_pubkey(GetX509())};
    if (nullptr == pEvpKey) {
        return 0U;
    }
    uint32_t const nReturn{static_cast< uint32_t >(EVP_PKEY_id(pEvpKey))};
    return nReturn;
}
/// @brief Get the "public key length"
/// @return Public key length
uint32_t PCertificate::GetPublicKeyLength() const noexcept
{
    if (nullptr == GetX509()) {
        return 0U;
    }
    EVP_PKEY *const pEvpKey{X509_get0_pubkey(GetX509())};
    if (nullptr == pEvpKey) {
        return 0U;
    }
    uint32_t const nReturn{static_cast< uint32_t >(EVP_PKEY_size(pEvpKey))};
    return nReturn;
}
/// @brief Get public key data
/// @return Public key data
ara::core::Vector< ara::core::Byte > PCertificate::GetPublicKeyData() const noexcept
{
    ara::core::Vector< ara::core::Byte > vecData;
    if (nullptr == GetX509()) {
        return vecData;
    }
    ASN1_BIT_STRING *const pPubkeyString{X509_get0_pubkey_bitstr(GetX509())};
    if (nullptr == pPubkeyString) {
        return vecData;
    }
    vecData.reserve(static_cast< std::size_t >(pPubkeyString->length));
    for (int32_t i{0}; i < pPubkeyString->length; ++i) {
        vecData.push_back(ara::core::Byte(*((pPubkeyString->data) + i)));
    }
    return vecData;
}
/// @brief Certificate signature algorithm type: RSA, DSA, etc.
/// @return
uint32_t PCertificate::GetSignatureAlgType() const noexcept  // Certificate signature algorithm type: RSA, DSA, etc.
{
    if (nullptr == GetX509()) {
        return 0U;
    }
    int32_t const nReturn{X509_get_signature_type(
        GetX509())};  // Returns the algorithm ID for the certificate fingerprint verification part
    return static_cast< uint32_t >(nReturn);
}
/// @brief The signature algorithm of the certificate. Refers to the algorithm (including hash algorithm) used to sign the certificate.
/// @return The signature algorithm of the certificate
uint32_t PCertificate::GetSignatureAlgOid() const noexcept
{
    if (nullptr == GetX509()) {
        return 0U;
    }
    // 2022-06-01 Test data: NID_sha1WithRSAEncryption == 65
    int32_t const nReturn{X509_get_signature_nid(GetX509())};
    // Whether to convert OpenSSL's SignatureAlgOid to CryptoAlgId
    return static_cast< uint32_t >(nReturn);
}
/// @brief Get the signature string
/// @return Signature string
ara::core::Result< ara::core::Vector< ara::core::Byte > > PCertificate::GetSignatureString() const noexcept
{
    if (nullptr == GetX509()) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kLogicFault);
    }
    ASN1_BIT_STRING const *pBitString{nullptr};
    X509_get0_signature(&pBitString, nullptr, GetX509());
    ara::core::Vector< ara::core::Byte > const vecData{Asn1ToVecByte(pBitString)};
    return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromValue(vecData);
}
/// @brief Get the fingerprint stored inside the X509
/// @param nHashType
/// @return Fingerprint information stored inside the X509
ara::core::Result< ara::core::Vector< ara::core::Byte > > PCertificate::GetFingerprintInX509(
    int32_t const nHashType) const noexcept
{
    ara::core::Vector< ara::core::Byte > vecData;
    EVP_MD const *const pDigest{EVP_get_digestbynid(nHashType)};
    uint32_t const nHashLen{TransHashLen(nHashType)};
    if (pDigest != nullptr) {
        ara::core::Vector< uint8_t > vecBuff;
        vecBuff.resize(kInt_1024U);
        uint8_t *const buff{vecBuff.data()};
        uint32_t nLen{0U};
        int32_t const nRc{X509_digest(GetX509(), pDigest, static_cast< uint8_t * >(buff), &nLen)};
        if ((0 == nRc) || (nLen > nHashLen)) {
            return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
                SecurityErrorDomain::Errc::kLogicFault);
        }
        for (uint32_t i{0U}; i < nLen; ++i) {
            vecData.push_back(ara::core::Byte(*(buff + i)));
        }
    }
    return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromValue(vecData);
}
/// @brief Get the extended attribute "Extended Key Usage"
/// @return Extended Key Usage
ara::core::Vector< uint32_t > PCertificate::GetExtKeyUsage() const noexcept
{
    ara::core::Vector< uint32_t > vecData;
    std::ignore = _DealExtDataByID(NID_ext_key_usage, [&vecData](void const *const pVoidData) -> bool {
        EXTENDED_KEY_USAGE const *const pExtData{static_cast< EXTENDED_KEY_USAGE const * >(pVoidData)};
        for (int32_t i{0}; i < sk_ASN1_OBJECT_num(pExtData); ++i) {
            ASN1_OBJECT *const pObj{sk_ASN1_OBJECT_value(pExtData, i)};  //NOLINT
            int32_t const nId{OBJ_obj2nid(pObj)};
            vecData.push_back(static_cast< uint32_t >(nId));
        }
        return true;
    });
    return vecData;
}
//********************************/
/// @brief Get fingerprint information
/// @param nSelectMode Selection mode
/// @param nIndex Index
/// @param hashCtx     Hash context
/// @param nOffset Offset
/// @param nPlanLen    Planned length
/// @param nFingerType Fingerprint type
void PCertificate::GetFingerprintTest(uint32_t const nSelectMode,
                                      int32_t &nIndex,
                                      cryp::HashFunctionCtx &hashCtx,
                                      int32_t const nOffset,
                                      int32_t const nPlanLen,
                                      int32_t const nFingerType) const noexcept
{
    ara::crypto::isoft_def::LogInfo() << "--------nFingerType = " << nFingerType << " nIndex = " << nIndex
                                      << " nOffset = " << nOffset << " nPlanLen = " << nPlanLen << "--------\n";

    if ((nSelectMode & 1U) == 1U) {
        ara::core::Vector< ara::core::Byte > vecData;
        std::ignore = _ExportCert_Der(vecData, 0);
        if (nIndex <= 0) {
            ara::core::Byte *const pData{vecData.data()};
            PrintfByteString(ara::core::StringView("EVP_Digest.SRC"),
                             static_cast< uint8_t const * >(static_cast< void const * >(pData)),
                             static_cast< int32_t >(vecData.size()));
        }
        ara::core::Vector< uint8_t > vecOutBuff;
        vecOutBuff.resize(kInt_1024U);
        uint8_t *const pOutBuff{vecOutBuff.data()};

        uint32_t nOutLen{0U};
        void const *const data{vecData.data()};
        size_t const count{vecData.size()};
        std::ignore = EVP_Digest(data, count, static_cast< uint8_t * >(pOutBuff), &nOutLen,
                                 EVP_get_digestbynid(nFingerType), nullptr);

        PrintfByteString(ara::core::StringView("EVP_Digest.DST"), static_cast< uint8_t const * >(pOutBuff),
                         static_cast< int32_t >(nOutLen));
        ara::core::Byte *const pCertData{vecData.data()};
        ReadOnlyMemRegion const readMem{static_cast< uint8_t const * >(static_cast< void const * >(pCertData)),
                                        vecData.size()};
        std::ignore = hashCtx.Start();
        std::ignore = hashCtx.Update(readMem);
        // Output result
        ara::core::Result< ara::core::Vector< ara::core::Byte > > const result{hashCtx.Finish()};
        PrintfByteString(ara::core::StringView("HashCtx.ASN1.DST"),
                         static_cast< uint8_t const * >(static_cast< void const * >(result.Value().data())),
                         static_cast< int32_t >(result.Value().size()));
    }
    if ((nSelectMode & kInt_2U) == kInt_2U) {
        // The content involved in the hash calculation is the entire X509 content
        ara::core::Vector< ara::core::Byte > vecData;

        std::ignore = _ExportCert_Der(vecData, kInt_2);
        ReadOnlyMemRegion const readMem{
            static_cast< uint8_t const * >(static_cast< void const * >(vecData.data())) + nOffset,
            static_cast< std::size_t >(nPlanLen)};
        std::ignore = hashCtx.Start();
        std::ignore = hashCtx.Update(readMem);
        // Output result
        ara::core::Result< ara::core::Vector< ara::core::Byte > > const result{hashCtx.Finish()};
        PrintfByteString(ara::core::StringView("HashCtx.X509.DST"),
                         static_cast< uint8_t const * >(static_cast< void const * >(result.Value().data())),
                         static_cast< int32_t >(result.Value().size()));
        if (nIndex <= 0) {
            PrintfByteString(ara::core::StringView("i2d_X509.SRC"), readMem.data(),
                             static_cast< int32_t >(readMem.size()));
        }
    }
    nIndex += 1;
}
//********************************/
}  // namespace  isoft_def
}  // namespace x509
}  // namespace crypto
}  // namespace ara
