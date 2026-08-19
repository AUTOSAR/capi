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
/// @file       ocsp_response.h
/// @brief      AutoSar-Crypto Certificate Management Module
/// @details    Online Certificate Status Protocol response.
/// @date       2021-12-21
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// <tr><td>2021-12-21  <td>1.0.0    <td>hanjingjing <td>Created initial version
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Certificate Components/Certificate Revocation
/// @interface_level=software
/// @trace_id_sr=SR_CRYPTO_03006
/// @unit_name=OcspResponse
/// @unit_description=OCSP Response Base Class
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_X509_OCSP_RESPONSE_H_
#define ARA_CRYPTO_X509_OCSP_RESPONSE_H_

#include "ara/crypto/x509/x509_object.h"

namespace ara {
namespace crypto {
namespace x509 {
//********************************/
/// @brief Online Certificate Status Protocol certificate status.
/// @brief On-line Certificate Status Protocol (OCSP) Certificate Status.
/// @code{.isoft}
/// @trace_id_sws={SWS_CRYPT_40002}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02306}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02559
/// @trace_id_dd=DD_CRYPTO_05365
/// @needwork = ad
/// @endcode
enum class OcspCertStatus : std::uint32_t
{
    /// The certificate is not revoked
    kGood = 0,
    /// The certificate has been revoked (either permanantly or temporarily (on hold))
    kRevoked = 1,
    /// The responder doesn't know about the certificate being requested
    kUnknown = 2,
};
/// @brief OCSP (online Certificate Status Protocol) response status.
/// @brief On-line Certificate Status Protocol (OCSP) Response Status.
/// @code{.isoft}
/// @trace_id_sws={SWS_CRYPT_40001}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02306}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02560
/// @trace_id_dd=DD_CRYPTO_05366
/// @needwork = ad
/// @endcode
enum class OcspResponseStatus : std::uint32_t
{
    /// Response has valid confirmations
    kSuccessful = 0,
    /// Illegal confirmation request
    kMalformedRequest = 1,
    /// Internal error in issuer
    kInternalError = 2,
    /// Try again later
    kTryLater = 3,
    // (4)  is not used

    /// Must sign the request
    kSigRequired = 5,
    /// Request unauthorized
    kUnauthorized = 6,
};
//********************************/
//- @interface OcspResponse
/// @brief Online Certificate Status Protocol response.
/// @brief On-line Certificate Status Protocol Response.
/// @code{.isoft}
/// @trace_id_sws={SWS_CRYPT_40800}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02306}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02561
/// @trace_id_dd=DD_CRYPTO_05367
/// @needwork = ad
/// @endcode
class OcspResponse : public X509Object
{
public:
    /// @brief Default constructor
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02562
    /// @trace_id_dd=DD_CRYPTO_05368
    /// @needwork = ad
    /// @endcode
    OcspResponse() = delete;
    /// @brief Default virtual destructor
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02563
    /// @trace_id_dd=DD_CRYPTO_05369
    /// @needwork = ad
    /// @endcode
    ~OcspResponse() noexcept override = default;
    /// @brief Default copy assignment operator
    /// @brief Copy-assign another OcspResponse to this instance.
    /// @param other Another instance of this class
    /// @returns *this, containing the contents of @a other
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dda
    /// @endcode
    OcspResponse& operator=(OcspResponse const& other) = delete;
    /// @brief Default move constructor
    /// @brief Move-assign another OcspResponse to this instance.
    /// @param other Another instance of this class
    /// @return *this, containing the contents of @a other
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dda
    /// @endcode
    OcspResponse& operator=(OcspResponse&& other) = delete;
    /// @brief Default move constructor
    /// @param other Another instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dda
    /// @endcode
    OcspResponse(OcspResponse&& other) = delete;
    /// @brief Default copy constructor
    /// @param other Another instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dda
    /// @endcode
    OcspResponse(OcspResponse const& other) = delete;

public:
    /// @brief Unique smart pointer for the constant interface.
    /// @brief Shared smart pointer of the interface.
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_40802}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02306}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_03225
    /// @trace_id_dd=DD_CRYPTO_06437
    /// @needwork = ad
    /// @endcode
    using Uptrc = std::unique_ptr< OcspResponse const >;

public:
    /// @brief Unique smart pointer for the interface.
    /// @brief Shared smart pointer of the interface.
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_40801}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02306}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_03226
    /// @trace_id_dd=DD_CRYPTO_06438
    /// @needwork = ad
    /// @endcode
    using Uptr = std::unique_ptr< OcspResponse >;

public:
    /// @brief Get the version of the OCSP response format.
    /// @brief Get version of the OCSP response format.
    /// @returns OCSP response format version
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_40811}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02306}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02568
    /// @trace_id_dd=DD_CRYPTO_05374
    /// @needwork = ad
    /// @endcode
    virtual std::uint32_t Version() const noexcept = 0;

public:  // Serializable interface
    /// @brief Expose serialization itself.
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
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02569
    /// @trace_id_dd=DD_CRYPTO_05375
    /// @needwork = ad
    /// @endcode
    ara::core::Result< ara::core::Vector< ara::core::Byte > > ExportPublicly(FormatId formatId
                                                                             = kFormatDefault) const noexcept override;

public:
    using X509Object::X509Object;
};
//********************************/
}  // namespace x509
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_X509_OCSP_RESPONSE_H_
