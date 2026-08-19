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
/// @file       cert_sign_request.h
/// @brief      AutoSar-Crypto Certificate Management Module
/// @details    Certificate Signing Request (CSR) object interface.
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
/// @module_path=/CRYPTO/Certificate Components/Certificate Request
/// @interface_level=software
/// @trace_id_sr=SR_CRYPTO_03005
/// @unit_name=CertSignRequest
/// @unit_description=Certificate Signing Request Base Class
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_X509_CERT_SIGN_REQUEST_H_
#define ARA_CRYPTO_X509_CERT_SIGN_REQUEST_H_

#include "ara/core/result.h"
#include "ara/crypto/common/mem_region.h"
#include "ara/crypto/x509/basic_cert_info.h"

namespace ara {
namespace crypto {
namespace x509 {
/// @brief X509 Certificate Provider
class X509Provider;
//********************************/
//- @interface CertSignRequest
/// @brief Certificate Signing Request (CSR) object interface. This interface is specifically for complete parsing of request content.
/// @brief Certificate Signing Request (CSR) object interface.
///         This interface is dedicated for complete parsing of the request content.
/// @trace_id_sws={SWS_CRYPT_40300}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02306}
//********************************/
/// @code{.isoft}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02603
/// @trace_id_dd=DD_CRYPTO_05415
/// @needwork = ad
/// @endcode
class CertSignRequest : public BasicCertInfo
{
public:
    /// @brief Unique smart pointer for the constant interface.
    /// @brief Unique smart pointer of the constant interface.
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_40301}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02306}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_03230
    /// @trace_id_dd=DD_CRYPTO_06442
    /// @needwork = ad
    /// @endcode
    using Uptrc = std::unique_ptr< CertSignRequest const >;
    /// @brief Unique smart pointer for the interface.
    /// @brief Unique smart pointer of the interface.
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_40302}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02306}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_03231
    /// @trace_id_dd=DD_CRYPTO_06443
    /// @needwork = ad
    /// @endcode
    using Uptr = std::unique_ptr< CertSignRequest >;

public:  // CertSignRequest interface
    /// @brief Verify the self-signed signature of the certificate request.
    /// @brief Verifies self-signed signature of the certificate request.
    /// @returns @c true if the signature is correct
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_40311}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02306}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02604
    /// @trace_id_dd=DD_CRYPTO_05416
    /// @needwork = ad
    /// @endcode
    virtual bool Verify() const noexcept = 0;
    /// @brief Export this certificate signing request in DER encoded ASN1 format. Note: This is the CSR that can be sent to a CA to obtain a certificate. // "const" modifier not present in 2011 documentation
    /// @brief Export this certificate signing request in DER encoded ASN1 format.
    ///         Note: this is the CSR that can be sent to the CA for obtaining the certificate.
    /// @returns a buffer with the formatted CSR
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kInvalidUsageOrder  this error will be returned in case not all required
    /// information has been provided
    /// @trace_id_sws={SWS_CRYPT_40313}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02306}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02605
    /// @trace_id_dd=DD_CRYPTO_05417
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< ara::core::Vector< ara::core::Byte > > ExportASN1CertSignRequest() const noexcept = 0;
    /// @brief Return the signature object of the request.
    /// @brief Return signature object of the request.
    /// @returns signature object of the request
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_40315}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02306}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02606
    /// @trace_id_dd=DD_CRYPTO_05418
    /// @needwork = ad
    /// @endcode
    virtual ara::crypto::cryp::Signature const& GetSignature() const noexcept = 0;
    /// @brief Return the format version of the certificate request.
    /// @brief Return format version of the certificate request.
    /// @returns format version of the certificate request
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_40314}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02306}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02607
    /// @trace_id_dd=DD_CRYPTO_05419
    /// @needwork = ad
    /// @endcode
    virtual uint32_t Version() const noexcept = 0;

public:
    /// @brief Constructor with parameters
    /// @param x509Provider Certificate provider
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02608
    /// @trace_id_dd=DD_CRYPTO_05420
    /// @needwork = ad
    /// @endcode
    explicit CertSignRequest(X509Provider& x509Provider) noexcept;
};
//********************************/
}  // namespace x509
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_X509_CERT_SIGN_REQUEST_H_
