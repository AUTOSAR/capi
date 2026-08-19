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
/// @file       isoft_ocsp_response.h
/// @brief      AutoSar-Crypto OCSP Response
/// @details    Online Certificate Status Protocol Response.
/// @date       2023-11-23
/// @author     Chang Zheng
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification Log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// <tr><td>2023-11-23  <td>1.0.0    <td>Chang Zheng      <td>Initial version creation
/// </table>
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

#ifndef ARA_CRYPTO_X509_PUHUA_OCSP_RESPONSE_H_
#define ARA_CRYPTO_X509_PUHUA_OCSP_RESPONSE_H_

#include <openssl/ocsp.h>
#include <openssl/pem.h>

#include "ara/crypto/x509/ocsp_response.h"

/// @brief OCSP response data type: ID
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01428
/// @trace_id_dd=DD_CRYPTO_03743
/// @needwork = ad
/// @endcode
struct ocsp_responder_id_st
{
    /// @brief ID type
    int32_t type;
    /// @brief Response Value data
    // PRQA S 2176 QAC // @qac Suspected inability to modify [2176]: This is an untagged union.
    union
    // PRQA L:QAC
    {
        /// @brief X509 certificate name pointer
        X509_NAME *byName;
        /// @brief ASN1_OCTET_STRING string type pointer
        ASN1_OCTET_STRING *byKey;
    } value;
};
/// @brief OCSP response data type: DATA
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01429
/// @trace_id_dd=DD_CRYPTO_03744
/// @needwork = ad
/// @endcode
struct ocsp_response_data_st
{
    /// @brief Version number
    ASN1_INTEGER *version;
    /// @brief Response ID
    OCSP_RESPID responderId;
    /// @brief Validity time
    ASN1_GENERALIZEDTIME *producedAt;
    /// @brief OCSP response
    STACK_OF(OCSP_SINGLERESP) * responses;
    /// @brief X509 extension data
    STACK_OF(X509_EXTENSION) * responseExtensions;
};
/// @brief OCSP response data type
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01430
/// @trace_id_dd=DD_CRYPTO_03745
/// @needwork = ad
/// @endcode
struct ocsp_basic_response_st
{
    /// @brief TBS response data
    OCSP_RESPDATA tbsResponseData;
    /// @brief Signature algorithm
    X509_ALGOR signatureAlgorithm;
    /// @brief Signature data
    ASN1_BIT_STRING *signature;
    /// @brief Certificate data
    STACK_OF(X509) * certs;
};

/*-  ResponseBytes ::=       SEQUENCE {
 *       responseType   OBJECT IDENTIFIER,
 *       response       OCTET STRING }
 */
/// @brief OCSP response structure: byte data
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01431
/// @trace_id_dd=DD_CRYPTO_03746
/// @needwork = ad
/// @endcode
struct ocsp_resp_bytes_st
{
    /// @brief Response type
    ASN1_OBJECT *responseType;
    /// @brief Response data: ASN1_OCTET_STRING
    ASN1_OCTET_STRING *response;
};

/*-  OCSPResponse ::= SEQUENCE {
 *      responseStatus         OCSPResponseStatus,
 *      responseBytes          [0] EXPLICIT ResponseBytes OPTIONAL }
 */
/// @brief OCSP response structure
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01432
/// @trace_id_dd=DD_CRYPTO_03747
/// @needwork = ad
/// @endcode
struct ocsp_response_st
{
    /// @brief Response status
    ASN1_ENUMERATED *responseStatus;
    /// @brief Response data
    OCSP_RESPBYTES *responseBytes;
};

namespace ara {
namespace crypto {
namespace x509 {
//********************************/
//- @interface OcspRequest
/// @brief Online Certificate Status Protocol Response.
/// @brief On-line Certificate Status Protocol Request.
/// @code{.isoft}
/// @trace_id_sws={SWS_CRYPT_40700}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02306}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01433
/// @trace_id_dd=DD_CRYPTO_03748
/// @needwork = ad
/// @endcode
class POcspResponse : public OcspResponse
{
public:
    /// @brief Constant interface unique smart pointer.
    /// @brief Shared smart pointer of the interface.
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_40702}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02306}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01433
    /// @trace_id_dd=DD_CRYPTO_06540
    /// @needwork = dd
    /// @endcode
    using Uptrc = std::unique_ptr< POcspResponse const >;
    /// @brief Interface unique smart pointer.
    /// @brief Shared smart pointer of the interface.
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_40701}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02306}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01433
    /// @trace_id_dd=DD_CRYPTO_06541
    /// @needwork = dd
    /// @endcode
    using Uptr = std::unique_ptr< POcspResponse >;

public:
    /// @brief Parameterized constructor
    /// @param x509Provider Certificate provider
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03749
    /// @needwork = dda
    /// @endcode
    explicit POcspResponse(X509Provider &x509Provider) noexcept;
    /// @brief Default constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03750
    /// @needwork = dda
    /// @endcode
    POcspResponse() = delete;
    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03751
    /// @needwork = dda
    /// @endcode
    ~POcspResponse() noexcept override;
    /// @brief Copy constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03752
    /// @needwork = dda
    /// @endcode
    POcspResponse(POcspResponse const &other) = delete;
    /// @brief Move constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03753
    /// @needwork = dda
    /// @endcode
    POcspResponse(POcspResponse &&other) = delete;
    /// @brief Copy assignment
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03754
    /// @needwork = dda
    /// @endcode
    POcspResponse &operator=(POcspResponse &&other) = delete;
    /// @brief Move assignment
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03755
    /// @needwork = dda
    /// @endcode
    POcspResponse &operator=(POcspResponse &other) = delete;

public:
    /// @brief Get OCSP response format version.
    /// @brief Get version of the OCSP response format.
    /// @returns OCSP response format version
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_40811}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02306}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03756
    /// @needwork = dda
    /// @endcode
    std::uint32_t Version() const noexcept override;

public
    :  // Serializable interface
       /// @brief Publicly serialize itself.
    /// @brief Serialize itself publicly.
    /// @param formatId  the Crypto Provider specific identifier of the output format
    /// @returns a buffer with the serialized object
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kInsufficientCapacity   if (output.empty() == false), but it's capacity is less
    /// than required
    /// @error: SecurityErrorDomain::kUnknownIdentifier      if an unknown format ID was specified
    /// @error: SecurityErrorDomain::kUnsupportedFormat      if the specified format ID is not supported for this object
    /// type
    /// @trace_id_sws={SWS_CRYPT_10711}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02112}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03757
    /// @needwork = dda
    /// @endcode
    ara::core::Result< ara::core::Vector< ara::core::Byte > > ExportPublicly(FormatId formatId
                                                                             = kFormatDefault) const noexcept override;

private:
    /// @brief ocsp response pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03758
    /// @needwork = dda
    /// @endcode
    OCSP_RESPONSE *pOcspResponse_{};

public:
    /// @brief Associate an ocsp response structure
    /// @param pOcspResponse ocsp response pointer
    /// @return true if attach response false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03759
    /// @needwork = dda
    /// @endcode
    bool AttachResponse(OCSP_RESPONSE *const pOcspResponse) noexcept;
    /// @brief Get ocsp response structure
    /// @return OCSP_RESPONSE structure pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03760
    /// @needwork = dda
    /// @endcode
    OCSP_RESPONSE *GetOCSP_RESPONSE() const noexcept { return pOcspResponse_; }
};
//********************************/
}  // namespace x509
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_X509_PUHUA_OCSP_RESPONSE_H_
