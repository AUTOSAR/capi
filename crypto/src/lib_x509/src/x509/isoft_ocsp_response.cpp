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
/// @file       isoft_ocsp_response.cpp
/// @brief      AutoSar-Crypto OCSP Response
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
/// @unit_name=POcspResponse
/// @unit_description=OCSP Response
/// @endcode
///
/// ================================================================

#include "ara/crypto/x509/isoft_ocsp_response.h"

#include "ara/crypto/common/isoft_common_api.h"
#include "ara/crypto/openssl/isoft_openssl_encrypt.h"
#include "ara/crypto/x509/isoft_certificate.h"

namespace ara {
namespace crypto {
namespace x509 {
//********************************/
/// @brief OCSP response interface.
//********************************/
/// @brief Constructor
/// @param x509Provider Certificate provider
POcspResponse::POcspResponse(X509Provider &x509Provider) noexcept : OcspResponse{x509Provider} {}
/// @brief Destructor
POcspResponse::~POcspResponse() noexcept
{
    if (pOcspResponse_ != nullptr) {
        OCSP_RESPONSE_free(pOcspResponse_);
        pOcspResponse_ = nullptr;
    }
}
/// @brief Get the version of the OCSP response format.
/// @brief Get version of the OCSP response format.
/// @returns OCSP response format version
/// @trace_id_sws={SWS_CRYPT_40811}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02306}
/// @threadsafety={Thread-safe}
std::uint32_t POcspResponse::Version() const noexcept
{
    if (pOcspResponse_ == nullptr) {
        return 0U;
    }
    OCSP_BASICRESP *const bs{OCSP_response_get1_basic(pOcspResponse_)};
    int64_t const l{ASN1_INTEGER_get(bs->tbsResponseData.version)};
    return 1U + static_cast< std::uint32_t >(l);
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
ara::core::Result< ara::core::Vector< ara::core::Byte > > POcspResponse::ExportPublicly(
    FormatId formatId) const noexcept
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
            int32_t const ret{ASN1_i2d_bio(reinterpret_cast< i2d_of_void * >(&i2d_OCSP_RESPONSE), pbio,
                                           reinterpret_cast< uint8_t * >(pOcspResponse_))};
            // PRQA L:QAC
            return ret;
        }
        if (funcId == kFuncPem) {
            // PRQA S 228,3048,3049 QAC /// @qac: This expression is a cast to or from a pointer to function type.
            int32_t const ret{PEM_ASN1_write_bio(I2D_CAST(i2d_OCSP_RESPONSE), PEM_STRING_OCSP_RESPONSE, pbio,
                                                 T_TransChar(pOcspResponse_), nullptr, nullptr, 0, nullptr, nullptr)};
            // PRQA L:QAC
            return ret;
        }
        return 0;
    }};
    return ExportPublic_Fun(formatId, func);
}
/// @brief Associate an ocsp response
/// @param pOcspResponse Pointer object of the ocsp response to be associated
/// @return
bool POcspResponse::AttachResponse(OCSP_RESPONSE *const pOcspResponse) noexcept
{
    pOcspResponse_ = pOcspResponse;
    return true;
}

//********************************/
}  // namespace x509
}  // namespace crypto
}  // namespace ara
