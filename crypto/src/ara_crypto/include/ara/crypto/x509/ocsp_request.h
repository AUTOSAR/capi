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
/// @file       ocsp_request.h
/// @brief      AutoSar-Crypto Certificate Management Module
/// @details    Online Certificate Status Protocol request.
/// @date       2021-12-21
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// <tr> <td>2021-12-21 <td>1.0.0 | <tr>hanjingjing <td>Created initial version
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Certificate Components/Certificate Revocation
/// @interface_level=software
/// @trace_id_sr=SR_CRYPTO_03006
/// @unit_name=OcspRequest
/// @unit_description=OCSP Request Base Class
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_X509_OCSP_REQUEST_H_
#define ARA_CRYPTO_X509_OCSP_REQUEST_H_

#include "ara/crypto/x509/x509_object.h"

namespace ara {
namespace crypto {
namespace x509 {
//********************************/
//- @interface OcspRequest
/// @brief Online Certificate Status Protocol request.
/// @brief On-line Certificate Status Protocol Request.
/// @code{.isoft}
/// @trace_id_sws={SWS_CRYPT_40700}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02306}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02570
/// @trace_id_dd=DD_CRYPTO_05376
/// @needwork = ad
/// @endcode
class OcspRequest : public X509Object
{
public:
    /// @brief Unique smart pointer for the constant interface.
    /// @brief Shared smart pointer of the interface.
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_40702}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02306}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_03227
    /// @trace_id_dd=DD_CRYPTO_06439
    /// @needwork = ad
    /// @endcode
    using Uptrc = std::unique_ptr< OcspRequest const >;
    /// @brief Unique smart pointer for the interface.
    /// @brief Shared smart pointer of the interface.
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_40701}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02306}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_03228
    /// @trace_id_dd=DD_CRYPTO_06440
    /// @needwork = ad
    /// @endcode
    using Uptr = std::unique_ptr< OcspRequest >;
    /// @brief Default destructor
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02571
    /// @trace_id_dd=DD_CRYPTO_05377
    /// @needwork = ad
    /// @endcode
    ~OcspRequest() override = default;
    /// @brief Default copy constructor
    /// @param other Another instance of this class
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02572
    /// @trace_id_dd=DD_CRYPTO_05378
    /// @needwork = ad
    /// @endcode
    OcspRequest(OcspRequest const &other) = delete;
    /// @brief Default move constructor
    /// @param other Another instance of this class
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02573
    /// @trace_id_dd=DD_CRYPTO_05379
    /// @needwork = ad
    /// @endcode
    OcspRequest(OcspRequest &&other) = delete;
    /// @brief Default copy assignment operator
    /// @param other Another instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02574
    /// @trace_id_dd=DD_CRYPTO_05380
    /// @needwork = ad
    /// @endcode
    OcspRequest &operator=(OcspRequest const &other) = delete;
    /// @brief Default move assignment operator
    /// @param other Another instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02575
    /// @trace_id_dd=DD_CRYPTO_05381
    /// @needwork = ad
    /// @endcode
    OcspRequest &operator=(OcspRequest &&other) = delete;

public:
    /// @brief Get the version of the OCSP request format.
    /// @brief Get version of the OCSP request format.
    /// @returns OCSP request format version
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_40711}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02306}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02576
    /// @trace_id_dd=DD_CRYPTO_05382
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
    /// @trace_id_ad=AD_CRYPTO_02577
    /// @trace_id_dd=DD_CRYPTO_05383
    /// @needwork = ad
    /// @endcode
    ara::core::Result< ara::core::Vector< ara::core::Byte > > ExportPublicly(FormatId formatId
                                                                             = kFormatDefault) const noexcept override;
    /// @brief Use base class template functions
    using Serializable::ExportPublicly;

public:
    using X509Object::X509Object;
};
//********************************/
}  // namespace x509
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_X509_OCSP_REQUEST_H_
