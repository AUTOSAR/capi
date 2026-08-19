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
/// @file       isoft_ocsp_request.cpp
/// @brief      AutoSar-Crypto OCSP Request
/// @details
/// @date       2023-11-23
/// @author     chang zheng
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Certificate Component/Certificate Revocation
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_03006
/// @unit_name=POcspRequest
/// @unit_description=OCSP Request
/// @endcode
///
/// ================================================================

#include "ara/crypto/x509/isoft_ocsp_request.h"

#include "ara/crypto/common/isoft_common_api.h"
#include "ara/crypto/openssl/isoft_openssl_encrypt.h"
#include "ara/crypto/x509/isoft_certificate.h"

namespace ara {
namespace crypto {
namespace x509 {
/// @brief Add OCSP certificate
/// @param req   OCSP_REQUEST struct pointer
/// @param cert   Certificate
/// @param certIdMd Hash algorithm
/// @param issuer Issuer certificate
/// @param ids Certificate ID
/// @return 1 sucess 0 failed
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01427
/// @trace_id_dd=DD_CRYPTO_06645
/// @trace_id_sr=SR_CRYPTO_03006
/// @unit_name=Add_ocsp_cert
/// @needwork = dd
/// @endcode
static int32_t Add_ocsp_cert(OCSP_REQUEST **const req,
                             X509 const *const cert,
                             EVP_MD const *const certIdMd,
                             X509 const *const issuer,
                             STACK_OF(OCSP_CERTID) *const ids) noexcept
{
    OCSP_CERTID *id{nullptr};

    if (issuer == nullptr) {
        return 0;
    }
    if (*req == nullptr) {
        *req = OCSP_REQUEST_new();
    }
    if (*req == nullptr) {
        return 0;
    }
    id = OCSP_cert_to_id(certIdMd, cert, issuer);
    if (id == nullptr) {
        return 0;
    }
    if (sk_OCSP_CERTID_push(ids, id) == 0) {
        return 0;
    }
    if (OCSP_request_add0_id(*req, id) == nullptr) {
        return 0;
    }
    return 1;
}
//********************************/
/// @brief OCSP request interface.
//********************************/
/// @brief Constructor
/// @param x509Provider Certificate provider
/// @param pIdsOther     ocsp certificate ID
POcspRequest::POcspRequest(X509Provider &x509Provider, STACK_OF(OCSP_CERTID) *const pIdsOther) noexcept
    : OcspRequest{x509Provider}, pOcspReq_{OCSP_REQUEST_new()}, pIds_{pIdsOther}
{
}
/// @brief Destructor
POcspRequest::~POcspRequest() noexcept
{
    if (pOcspReq_ != nullptr) {
        OCSP_REQUEST_free(pOcspReq_);
        pOcspReq_ = nullptr;
    }
    if (pIds_ != nullptr) {
        sk_OCSP_CERTID_free(pIds_);
    }
}

/// @brief Get the version of the OCSP request format.
/// @brief Get version of the OCSP request format.
/// @returns OCSP request format version
/// @trace_id_sws={SWS_CRYPT_40711}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02306}
/// @threadsafety={Thread-safe}
std::uint32_t POcspRequest::Version() const noexcept
{
    if (pOcspReq_ == nullptr) {
        return 0U;
    }
    OCSP_REQINFO *const inf{&(pOcspReq_->tbsRequest)};
    int64_t const l{ASN1_INTEGER_get(inf->version)};
    return static_cast< std::uint32_t >(l) + 1U;
}
//********************************/ //Serializable   interface
/// @brief Publicly serialize itself.
/// @brief Serialize itself publicly.
/// @param formatId  the Crypto Provider specific identifier of the output format
/// @returns a buffer with the serialized object
/// @error: SecurityErrorDomain::kInsufficientCapacity   if (output.empty() == false), but it's capacity is less than
/// required
/// @error: SecurityErrorDomain::kUnknownIdentifier      if an unknown format ID was specified
/// @error: SecurityErrorDomain::kUnsupportedFormat      if the specified format ID is not supported for this object
/// type
/// @trace_id_sws={SWS_CRYPT_10711}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02112}
/// @threadsafety={Thread-safe}
ara::core::Result< ara::core::Vector< ara::core::Byte > > POcspRequest::ExportPublicly(FormatId formatId) const noexcept
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
            // PRQA S 3048,3049 QAC /// @qac: This expression is a cast to or from a pointer to function type.
            int32_t const ret{ASN1_i2d_bio(reinterpret_cast< i2d_of_void * >(&i2d_OCSP_REQUEST), pbio,
                                           reinterpret_cast< uint8_t * >(pOcspReq_))};
            // PRQA L:QAC
            return ret;
        }
        if (funcId == kFuncPem) {
            // PRQA S 228,3048,3049 QAC /// @qac: This expression is a cast to or from a pointer to function type.
            int32_t const ret{PEM_ASN1_write_bio(I2D_CAST(i2d_OCSP_REQUEST), PEM_STRING_OCSP_REQUEST, pbio,
                                                 T_TransChar(pOcspReq_), nullptr, nullptr, 0, nullptr, nullptr)};
            // PRQA L:QAC
            return ret;
        }
        return 0;
    }};
    return ExportPublic_Fun(formatId, func);
}

/// @brief Add the certificate to be verified to the ocsp request
/// @param subjectCertFile  Certificate file
/// @param issureCertFile   Issuer certificate file
/// @return  true if attach request sucess false otherwise
bool POcspRequest::AttachResqust(ReadOnlyMemRegion const &subjectCertFile,
                                 ReadOnlyMemRegion const &issureCertFile) noexcept
{
    x509::isoft_def::PCertificate::Uptr const pCertficate{
        std::make_unique< x509::isoft_def::PCertificate >(MyProvider())};

    bool ret{pCertficate->LoadCertFile(subjectCertFile, kFormatDefault)};
    if (!ret) {
        return false;
    }

    x509::isoft_def::PCertificate::Uptr const pCertficateIssure{
        std::make_unique< x509::isoft_def::PCertificate >(MyProvider())};
    ret = pCertficateIssure->LoadCertFile(issureCertFile, kFormatDefault);
    if (!ret) {
        return false;
    }

    if (pIds_ == nullptr) {
        pIds_ = sk_OCSP_CERTID_new_null();  // NOLINT
    }

    ret = static_cast< bool >(
        Add_ocsp_cert(&pOcspReq_, pCertficate->GetX509(), EVP_sha1(), pCertficateIssure->GetX509(), pIds_));
    if (!ret) {
        return false;
    }

    return ret;
}

//********************************/
}  // namespace x509
}  // namespace crypto
}  // namespace ara
