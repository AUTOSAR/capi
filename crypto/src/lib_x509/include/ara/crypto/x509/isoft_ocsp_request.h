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
/// @file       isoft_ocsp_request.h
/// @brief      AutoSar-Crypto OCSP Request
/// @details    Online Certificate Status Protocol Request.
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
/// @unit_name=POcspRequest
/// @unit_description=OCSP Request
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_X509_PUHUA_OCSP_REQUEST_H_
#define ARA_CRYPTO_X509_PUHUA_OCSP_REQUEST_H_

#include <openssl/ocsp.h>
#include <openssl/pem.h>

#include "ara/crypto/x509/ocsp_request.h"

/// @brief OCSP request packet
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01425
/// @trace_id_dd=DD_CRYPTO_03728
/// @needwork = ad
/// @endcode
struct ocsp_req_info_st
{
    /// @brief EXPLICIT Version DEFAULT v1
    ASN1_INTEGER* version;
    /// @brief EXPLICIT GeneralName OPTIONA
    GENERAL_NAME* requestorName;
    /// @brief SEQUENCE OF Request
    STACK_OF(OCSP_ONEREQ) * requestList;
    /// @brief EXPLICIT Extensions OPTIONA
    STACK_OF(X509_EXTENSION) * requestExtensions;
};

/// @brief OCSP response packet
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01426
/// @trace_id_dd=DD_CRYPTO_03729
/// @needwork = ad
/// @endcode
struct ocsp_request_st
{
    /// @brief TBSRequest
    OCSP_REQINFO tbsRequest;
    /// @brief EXPLICIT Signature OPTIONAL
    OCSP_SIGNATURE* optionalSignature; /* OPTIONAL */
};

namespace ara {
namespace crypto {
namespace x509 {
//********************************/
//- @interface OcspRequest
/// @brief Online Certificate Status Protocol Request.
/// @brief On-line Certificate Status Protocol Request.
/// @code{.isoft}
/// @trace_id_sws={SWS_CRYPT_40700}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02306}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01427
/// @trace_id_dd=DD_CRYPTO_03730
/// @needwork = ad
/// @endcode
class POcspRequest : public OcspRequest
{
public:
    /// @brief Constant interface unique smart pointer.
    /// @brief Shared smart pointer of the interface.
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_40702}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02306}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01427
    /// @trace_id_dd=DD_CRYPTO_06344
    /// @needwork = dd
    /// @endcode
    using Uptrc = std::unique_ptr< POcspRequest const >;
    /// @brief Interface unique smart pointer.
    /// @brief Shared smart pointer of the interface.
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_40701}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02306}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01427
    /// @trace_id_dd=DD_CRYPTO_06345
    /// @needwork = dd
    /// @endcode
    using Uptr = std::unique_ptr< POcspRequest >;

public:
    /// @brief Default constructor deleted
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03731
    /// @needwork = dda
    /// @endcode
    POcspRequest() = delete;
    /// @brief Parameterized constructor
    /// @param x509Provider Certificate provider
    /// @param pIdsOther OCSP certificate ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03732
    /// @needwork = dda
    /// @endcode
    explicit POcspRequest(X509Provider& x509Provider, STACK_OF(OCSP_CERTID) * const pIdsOther = nullptr) noexcept;
    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03733
    /// @needwork = dda
    /// @endcode
    ~POcspRequest() noexcept override;
    /// @brief Copy constructor
    /// @param other Another object instance of this class (the other POcspRequest)
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03734
    /// @needwork = dda
    /// @endcode
    POcspRequest(POcspRequest const& other) = delete;
    /// @brief Move constructor
    /// @param other Another object instance of this class (the other POcspRequest)
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03735
    /// @needwork = dda
    /// @endcode
    POcspRequest(POcspRequest&& other) = delete;
    /// @brief Copy assignment
    /// @param other Another object instance of this class (the other POcspRequest)
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03736
    /// @needwork = dda
    /// @endcode
    POcspRequest& operator=(POcspRequest&& other) = delete;
    /// @brief Move assignment
    /// @param other Another object instance of this class (the other POcspRequest)
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03737
    /// @needwork = dda
    /// @endcode
    POcspRequest& operator=(POcspRequest& other) = delete;

public:
    /// @brief Get OCSP request format version.
    /// @brief Get version of the OCSP request format.
    /// @returns OCSP request format version
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_40711}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02306}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03738
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
    /// @trace_id_dd=DD_CRYPTO_03739
    /// @needwork = dda
    /// @endcode
    ara::core::Result< ara::core::Vector< ara::core::Byte > > ExportPublicly(FormatId formatId
                                                                             = kFormatDefault) const noexcept override;

private:
    /// @brief OCSP request packet pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03740
    /// @needwork = dda
    /// @endcode
    OCSP_REQUEST* pOcspReq_;
    /// @brief stack_st_OCSP_CERTID structure
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03741
    /// @needwork = dda
    /// @endcode
    STACK_OF(OCSP_CERTID) * pIds_;

public:
    /// @brief Associate an ocsp request structure
    /// @param subjectCertFile Subject certificate file
    /// @param issureCertFile Issuer certificate file
    /// @return true if attach request sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03742
    /// @needwork = dda
    /// @endcode
    bool AttachResqust(ReadOnlyMemRegion const& subjectCertFile, ReadOnlyMemRegion const& issureCertFile) noexcept;
};
//********************************/
}  // namespace x509
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_X509_PUHUA_OCSP_REQUEST_H_
