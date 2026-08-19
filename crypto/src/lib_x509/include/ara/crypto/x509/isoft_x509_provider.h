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
/// @file       isoft_x509_provider.h
/// @brief      AutoSar-Crypto Certificate Management Module
/// @details    X.509 Provider interface.
/// @date       2021-12-21
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification Log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// <tr><td>2021-12-21  <td>1.0.0    <td>hanjingjing      <td>Initial version creation
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Certificate Component/Certificate Provider
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_04001
/// @unit_name=PX509Provider
/// @unit_description=X.509 Provider
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_X509_PUHUA_X509_PROVIDER_H_
#define ARA_CRYPTO_X509_PUHUA_X509_PROVIDER_H_

#include <ara/core/vector.h>

#include "ara/crypto/ipc/isoft_ipc_client.h"
#include "ara/crypto/manifest/manifest_map_config.h"
#include "ara/crypto/x509/isoft_certificate.h"
#include "ara/crypto/x509/x509_provider.h"

namespace ara {
namespace crypto {
namespace x509 {
namespace isoft_def {
/// @code{.isoft}
/// @export_level=/Crypto
/// @endcode
//********************************/
/// @brief X.509 Provider interface. X.509 Provider supports two internal storages: volatile (or session-level) and persistent.
///         All X.509 objects created by the provider should have an actual reference to their parent X.509 provider.
///         The X.509 Provider can only be destroyed after all its child objects are destroyed.
///         Every method of this interface that creates X.509 objects is non-const because any such creation increases the reference counter of the X.509 Provider.
/// @brief X.509 Provider interface.
///       The X.509 Provider supports two internal storages: volatile (or session) and persistent.
///       All X.509 objects created by the provider should have an actual reference to their parent X.509 Provider.
///       The X.509 Provider can be destroyed only after destroying of all its daughterly objects.
///       Each method of this interface that creates a X.509 object is non-constant, because any such creation increases
///       a references counter of the X.509 Provider.
/// @code{.isoft}
/// @export_level=/Crypto
/// @trace_id_sws={SWS_CRYPT_40600}
/// @uptrace={RS_CRYPTO_02306}
/// @tracestatus={draft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01397
/// @trace_id_dd=DD_CRYPTO_03678
/// @needwork = ad
/// @endcode
class PX509Provider : public X509Provider
{
public:
    //********************************/
    /// @brief Generate a certificate object using memory data
    /// @param x509Provider Certificate provider
    /// @param stCertName Certificate name
    /// @param vecData Memory data
    /// @return Smart pointer to the certificate object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03679
    /// @needwork = dda
    /// @endcode
    static ara::core::Result< x509::Certificate::Uptr > MakeCertificate(
        x509::X509Provider& x509Provider,
        ara::core::StringView const& stCertName,
        ara::core::Vector< uint8_t > const& vecData) noexcept;
    /// @brief Generate a certificate signing request using memory data
    /// @param x509Provider Certificate provider
    /// @param stCsrName Certificate signing request name
    /// @param vecData Certificate signing request data
    /// @return Certificate signing request instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03680
    /// @needwork = dda
    /// @endcode
    static ara::core::Result< x509::CertSignRequest::Uptr > MakeCertSignRequest(
        x509::X509Provider& x509Provider,
        ara::core::StringView const& stCsrName,
        ara::core::Vector< uint8_t > const& vecData) noexcept;

public:
    /// @brief Create an empty X.500 DN (Distinguished Name) structure. If (0 == capacity), the maximum supported capacity must be reserved (by implementation).
    /// @brief Create an empty X.500 Distinguished Name (DN) structure.
    ///       If <tt>(0 == capacity)</tt> then a maximally supported (by the implementation) capacity must be reserved.
    /// @param capacity  number of bytes that should be reserved for the content of the target @c X509DN object
    /// @returns Unique smart pointer to created empty @c X509DN object
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_40611}
    /// @uptrace={RS_CRYPTO_02306}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_CRYPTO_04001
    /// @trace_id_ad=AD_CRYPTO_01398
    /// @trace_id_dd=DD_CRYPTO_03681
    /// @needwork = ad
    /// @endcode
    ara::core::Result< X509DN::Uptr > CreateEmptyDn(std::size_t capacity = 0U) noexcept override;
    /// @brief Create a complete X.500 Distinguished Name structure based on the provided string representation.
    /// @brief Create completed X.500 Distinguished Name structure from the provided string representation.
    /// @param dn  string representation of the Distinguished Name
    /// @returns unique smart pointer for the created @c X509DN object
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_40612}
    /// @uptrace={RS_CRYPTO_02306}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @error: SecurityErrorDomain::kInvalidArgument  if the @c dn argument has incorrect format
    /// @error: SecurityErrorDomain::kInvalidInputSize  if the @c dn argument has unsupported length (too large)
    /// @interface_level=unit
    /// @trace_id_sr=SR_CRYPTO_04001
    /// @trace_id_ad=AD_CRYPTO_01399
    /// @trace_id_dd=DD_CRYPTO_03682
    /// @needwork = ad
    /// @endcode
    ara::core::Result< X509DN::Uptrc > BuildDn(ara::core::StringView const& dn) noexcept override;
    /// @brief Decode X.500 Distinguished Name structure from the provided serialized format.
    /// @brief Decode X.500 Distinguished Name structure from the provided serialized format.
    /// @param dn  DER/PEM-encoded representation of the Distinguished Name
    /// @param formatId  input format identifier @c kFormatDefault means auto-detect
    /// @returns unique smart pointer for the created @c X509DN object
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_40613}
    /// @uptrace={RS_CRYPTO_02306}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @error: SecurityErrorDomain::kInvalidArgument  if the @c dn argument cannot be parsed
    /// @error: SecurityErrorDomain::kUnknownIdentifier  if the @c formatId argument has unknown value
    /// @interface_level=unit
    /// @trace_id_sr=SR_CRYPTO_04001
    /// @trace_id_ad=AD_CRYPTO_01400
    /// @trace_id_dd=DD_CRYPTO_03683
    /// @needwork = ad
    /// @endcode
    ara::core::Result< X509DN::Uptrc > DecodeDn(ReadOnlyMemRegion const& dn,
                                                Serializable::FormatId formatId
                                                = Serializable::kFormatDefault) noexcept override;

public:
    /// @brief Parse the serialized representation of a certificate and create its instance. Offline verification of the parsed certificate can be done by calling VerifyCertByCrl().
    ///         After verification, the certificate can be imported into session or persistent storage for subsequent searching and usage.
    ///         If the parsed certificate is not imported, it will be lost upon destruction of the returned instance! Only imported certificates can be found via search and undergo automatic verification!
    /// @brief Parse a serialized representation of the certificate and create its instance.
    ///       Off-line validation of the parsed certificate may be done via call @c VerifyCertByCrl().
    ///       After validation the certificate may be imported to the session or persistent storage for following
    ///       search and usage.
    ///       If the parsed certificate is not imported then it will be lost after destroy of the returned instance!
    ///       Only imported certificate may be found by a search and applied for automatic verifications!
    /// @param cert  DER/PEM-encoded certificate
    /// @param formatId  input format identifier @c kFormatDefault means auto-detect
    /// @return unique smart pointer to created certificate
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_40614}
    /// @uptrace={RS_CRYPTO_02306}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @error: SecurityErrorDomain::kInvalidArgument    if the @c cert argument cannot be parsed
    /// @error: SecurityErrorDomain::kUnknownIdentifier  if the @c formatId argument has unknown value
    /// @interface_level=unit
    /// @trace_id_sr=SR_CRYPTO_04002
    /// @trace_id_ad=AD_CRYPTO_01401
    /// @trace_id_dd=DD_CRYPTO_03684
    /// @needwork = ad
    /// @endcode
    ara::core::Result< Certificate::Uptr > ParseCert(ReadOnlyMemRegion const& cert,
                                                     Serializable::FormatId formatId
                                                     = Serializable::kFormatDefault) noexcept override;
    /// @brief Count the number of certificates in a serialized certificate chain represented by a single BLOB.
    /// @brief Count number of certificates in a serialized certificate chain represented by a single BLOB.
    /// @param certChain  DER/PEM-encoded certificate chain in form of a single BLOB
    /// @param formatId  input format identifier @c kFormatDefault means auto-detect
    /// @returns number of certificates in the chain
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_40615}
    /// @uptrace={RS_CRYPTO_02306}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @error: SecurityErrorDomain::kInvalidArgument  if the @c certChain argument cannot be pre-parsed
    /// @error: SecurityErrorDomain::kUnknownIdentifier  if the @c formatId argument has unknown value
    /// @interface_level=unit
    /// @trace_id_sr=SR_CRYPTO_04002
    /// @trace_id_ad=AD_CRYPTO_01402
    /// @trace_id_dd=DD_CRYPTO_03685
    /// @needwork = ad
    /// @endcode
    ara::core::Result< std::size_t > CountCertsInChain(ReadOnlyMemRegion const& certChain,
                                                       Serializable::FormatId formatId
                                                       = Serializable::kFormatDefault) const noexcept override;
    /// @brief 2011 standard interface
    /// @brief Parse the serialized representation of a certificate chain and create their instances.
    ///         Offline verification of the parsed certificate chain can be done by calling VerifyCertChainByCrl().
    ///         After verification, certificates can be saved to session or persistent storage for subsequent searching and usage.
    ///         If certificates are not imported, they will be lost upon destruction of the returned instance! Only imported certificates can be found via search and undergo automatic verification!
    ///         Certificates in the result vector will be placed from the Root CA certificate (index 0) to the final end-entity certificate (last used index of the vector).
    /// @brief Parse a serialized representation of the certificate chain and create their instances.
    ///       Off-line validation of the parsed certification chain may be done via call @c VerifyCertChainByCrl().
    ///       After validation the certificates may be saved to the session or persistent storage for following
    ///       search and usage.
    ///       If the certificates are not imported then they will be lost after destroy of the returned instances!
    ///       Only imported certificates may be found by a search and applied for automatic verifications!
    ///       Certificates in the @c outcome vector will be placed from the root CA certificate (zero index) to
    ///       the final end-entity certificate (last used index of the vector).
    /// @param outcome  an output vector for imported certificates
    /// @param certChain  DER/PEM-encoded certificate chain in form of a single BLOB
    /// @param formatId  input format identifier @c kFormatDefault means auto-detect
    /// @return has value if parase certchain sucess false otherwise
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_40616}
    /// @uptrace={RS_CRYPTO_02306}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @error: SecurityErrorDomain::kInsufficientCapacity   if the capacity of @c outcome vector is less than actual
    /// number of certificates in the chain
    /// @error: SecurityErrorDomain::kInvalidArgument        if the @c certChain argument cannot be parsed
    /// @error: SecurityErrorDomain::kUnknownIdentifier      if the @c formatId argument has unknown value
    /// @interface_level=unit
    /// @trace_id_sr=SR_CRYPTO_04002
    /// @trace_id_ad=AD_CRYPTO_01403
    /// @trace_id_dd=DD_CRYPTO_03686
    /// @needwork = ad
    /// @endcode
    ara::core::Result< void > ParseCertChain(ara::core::Vector< Certificate::Uptr >& outcome,
                                             ReadOnlyMemRegion const& certChain,
                                             Serializable::FormatId formatId
                                             = Serializable::kFormatDefault) noexcept override;
    /// @brief 2011 standard interface
    /// @brief Parse the serialized representation of a certificate chain and create their instances.
    /// @param certChain Output vector for certificate instances
    /// @param blob Input BLOB containing the certificate chain
    /// @param format Encoding format of the BLOB
    /// @details Offline verification of the parsed certificate chain can be done by calling VerifyCertChainByCrl(). After verification, certificates can be imported into session or persistent storage. The capacity of the result vector must equal the size of the certChain vector. If certificates are not imported, they will be lost upon destruction of the returned instance! Only imported certificates can be found via search and undergo automatic verification! Certificates in the result vector will be placed from the Root CA certificate (index 0) to the final end-entity certificate (last used index of the vector).
    /// @brief Parse a serialized representation of the certificate chain and create their instances.
    ///       Off-line validation of the parsed certification chain may be done via call @c VerifyCertChainByCrl().
    ///       After validation the certificates may be imported to the session or persistent storage for following
    ///       search and usage. Capacity of the @c outcome vector must be equal to the size of the @c certChain vector.
    ///       If the certificates are not imported then they will be lost after destroy of the returned instances!
    ///       Only imported certificates may be found by a search and applied for automatic verifications!
    ///       Certificates in the @c outcome vector will be placed from the root CA certificate (zero index) to
    ///       the final end-entity certificate (last used index of the vector).
    /// @param outcome  output vector of imported certificates
    /// @param certChain  DER/PEM-encoded certificates chain each certificate is presented by a separate BLOB in
    /// the input vector
    /// @param formatId  input format identifier @c kFormatDefault means auto-detect
    /// @return has value if parase certchain sucess false otherwise
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_40617}
    /// @uptrace={RS_CRYPTO_02306}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @error: SecurityErrorDomain::kInsufficientCapacity   if capacity of the @c outcome vector is less than number of
    /// elements in the @c certChain
    /// @error: SecurityErrorDomain::kInvalidArgument        if an element of @c certChain argument cannot be parsed
    /// @error: SecurityErrorDomain::kUnknownIdentifier      if the @c formatId argument has unknown value
    /// @interface_level=unit
    /// @trace_id_sr=SR_CRYPTO_04002
    /// @trace_id_ad=AD_CRYPTO_01404
    /// @trace_id_dd=DD_CRYPTO_03687
    /// @needwork = ad
    /// @endcode
    ara::core::Result< void > ParseCertChain(ara::core::Vector< Certificate::Uptr >& outcome,
                                             ara::core::Vector< ReadOnlyMemRegion > const& certChain,
                                             Serializable::FormatId formatId
                                             = Serializable::kFormatDefault) noexcept override;
    /// @brief 2311 standard interface
    /// @brief Parse the serialized representation of a certificate chain and create their instances.
    /// @param certChain Output vector for certificate instances
    /// @param blob Input BLOB containing the certificate chain
    /// @param format Encoding format of the BLOB
    /// @details Offline verification of the parsed certificate chain can be done by calling VerifyCertChainByCrl(). After verification, certificates can be saved to session or persistent storage. If certificates are not imported, they will be lost upon destruction of the returned instance! Only imported certificates can be found via search and undergo automatic verification! Certificates in the result vector will be placed from the Root CA certificate (index 0) to the final end-entity certificate (last used index of the vector).
    /// @brief Parse a serialized representation of the certificate chain and create their instances.
    ///       Off-line validation of the parsed certification chain may be done via call @c VerifyCertChainByCrl().
    ///       After validation the certificates may be saved to the session or persistent storage for following
    ///       search and usage.
    ///       If the certificates are not imported then they will be lost after destroy of the returned instances!
    ///       Only imported certificates may be found by a search and applied for automatic verifications!
    ///       Certificates in the @c outcome vector will be placed from the root CA certificate (zero index) to
    ///       the final end-entity certificate (last used index of the vector).
    /// @param certChain  DER/PEM-encoded certificate chain in form of a single BLOB
    /// @param formatId  input format identifier @c kFormatDefault means auto-detect
    /// @return an output vector for imported certificates
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_40616}
    /// @uptrace={RS_CRYPTO_02306}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @error: SecurityErrorDomain::kInsufficientCapacity   if the capacity of @c outcome vector is less than actual
    /// number of certificates in the chain
    /// @error: SecurityErrorDomain::kInvalidArgument        if the @c certChain argument cannot be parsed
    /// @error: SecurityErrorDomain::kUnknownIdentifier      if the @c formatId argument has unknown value
    /// @interface_level=unit
    /// @trace_id_sr=SR_CRYPTO_
    /// @trace_id_ad=AD_CRYPTO_
    /// @trace_id_dd=DD_CRYPTO_
    /// @needwork = ad
    /// @endcode
    ara::core::Result< ara::core::Vector< Certificate::Uptr > > ParseCertChain(
        ReadOnlyMemRegion const& certChain,
        Serializable::FormatId formatId = Serializable::kFormatDefault) noexcept override;
    /// @brief 2311 standard interface
    /// @brief Parse the serialized representation of a certificate chain and create their instances.
    /// @param certChain Output vector for certificate instances
    /// @param blob Input BLOB containing the certificate chain
    /// @param format Encoding format of the BLOB
    /// @details Offline verification of the parsed certificate chain can be done by calling VerifyCertChainByCrl(). After verification, certificates can be imported into session or persistent storage. The capacity of the result vector must equal the size of the certChain vector. If certificates are not imported, they will be lost upon destruction of the returned instance! Only imported certificates can be found via search and undergo automatic verification! Certificates in the result vector will be placed from the Root CA certificate (index 0) to the final end-entity certificate (last used index of the vector).
    /// @brief Parse a serialized representation of the certificate chain and create their instances.
    ///       Off-line validation of the parsed certification chain may be done via call @c VerifyCertChainByCrl().
    ///       After validation the certificates may be imported to the session or persistent storage for following
    ///       search and usage. Capacity of the @c outcome vector must be equal to the size of the @c certChain vector.
    ///       If the certificates are not imported then they will be lost after destroy of the returned instances!
    ///       Only imported certificates may be found by a search and applied for automatic verifications!
    ///       Certificates in the @c outcome vector will be placed from the root CA certificate (zero index) to
    ///       the final end-entity certificate (last used index of the vector).
    /// @param certChain  DER/PEM-encoded certificates chain each certificate is presented by a separate BLOB in
    /// the input vector
    /// @param formatId  input format identifier @c kFormatDefault means auto-detect
    /// @return output vector of imported certificates
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_40617}
    /// @uptrace={RS_CRYPTO_02306}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @error: SecurityErrorDomain::kInsufficientCapacity   if capacity of the @c outcome vector is less than number of
    /// elements in the @c certChain
    /// @error: SecurityErrorDomain::kInvalidArgument        if an element of @c certChain argument cannot be parsed
    /// @error: SecurityErrorDomain::kUnknownIdentifier      if the @c formatId argument has unknown value
    /// @interface_level=unit
    /// @trace_id_sr=SR_CRYPTO_
    /// @trace_id_ad=AD_CRYPTO_
    /// @trace_id_dd=DD_CRYPTO_
    /// @needwork = ad
    /// @endcode
    ara::core::Result< ara::core::Vector< Certificate::Uptr > > ParseCertChain(
        ara::core::Vector< ReadOnlyMemRegion > const& certChain,
        Serializable::FormatId formatId = Serializable::kFormatDefault) noexcept override;

public:
    /// @brief 2011 standard interface
    /// @brief Verify the status of the provided certificate solely through locally stored CA certificates and CRLs. This method updates the Certificate::Status associated with the certificate.
    /// @brief Verify status of the provided certificate by locally stored CA certificates and CRLs only.
    ///       This method updates the @c Certificate::Status associated with the certificate.
    /// @param cert  target certificate for verification
    /// @param myRoot  root certificate to be used for verification - if this is nullptr, use machine root
    /// certificates
    /// @returns verification status of the provided certificate
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_40618}
    /// @uptrace={RS_CRYPTO_02306}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_CRYPTO_04003
    /// @trace_id_ad=AD_CRYPTO_01405
    /// @trace_id_dd=DD_CRYPTO_03688
    /// @needwork = ad
    /// @endcode
    Certificate::Status VerifyCert(Certificate& cert, Certificate::Uptr myRoot = {nullptr}) noexcept override;
    /// @brief 2311 standard interface
    /// @brief Verify the status of the provided certificate solely through locally stored CA certificates and CRLs. This method updates the Certificate::Status associated with the certificate.
    /// @brief Verify status of the provided certificate by locally stored CA certificates and CRLs only.
    ///       This method updates the @c Certificate::Status associated with the certificate.
    /// @param cert  target certificate for verification
    /// @param myRoot  root certificate to be used for verification - if this is nullptr, use machine root
    /// certificates
    /// @returns verification status of the provided certificate
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_40618}
    /// @uptrace={RS_CRYPTO_02306}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_CRYPTO_04003
    /// @trace_id_ad=AD_CRYPTO_01405
    /// @trace_id_dd=DD_CRYPTO_03688
    /// @needwork = ad
    /// @endcode
    Certificate::Status VerifyCert(Certificate const& cert, Certificate const& myRoot) noexcept override;
    /// @brief 2011 standard interface
    /// @brief Verify the status of the provided certificate chain solely through locally stored CA certificates and CRLs.
    /// @details The verification status of the certificate chain is Certificate::Status::kValid; only valid if all certificates in the chain have this status!
    /// @details Certificates in the chain (provided by the container vector) must be placed from the Root CA certificate (index 0) to the target end-entity certificate (last used index of the vector). Verification is performed in the same order.
    /// @details If chain verification fails, the status of the first failed certificate is returned. This method updates the Certificate::Status associated with certificates in the chain.
    /// @brief Verify status of the provided certification chain by locally stored CA certificates and CRLs only.
    ///       Verification status of the certificate chain is @c Certificate::Status::kValid only if all certificates in
    ///       the chain have such status! Certificates in the chain (presented by container vector) must be placed from
    ///       the root CA certificate (zero index) to the target end-entity certificate (last used index of the vector).
    ///       Verification is executed in same order. If the chain verification is failed then status of the first
    ///       failed certificate is returned. This method updates the @c Certificate::Status associated with the
    ///       certificates in the chain.
    /// @param chain  target certificate chain for verification
    /// @param myRoot  root certificate to be used for verification - if this is nullptr, use machine root
    /// certificates
    /// @returns verification status of the provided certificate chain
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_40619}
    /// @uptrace={RS_CRYPTO_02306}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_CRYPTO_04003
    /// @trace_id_ad=AD_CRYPTO_01406
    /// @trace_id_dd=DD_CRYPTO_03689
    /// @needwork = ad
    /// @endcode
    Certificate::Status VerifyCertChain(ara::core::Span< Certificate::Uptr const > const& chain,
                                        Certificate::Uptr myRoot = {nullptr}) const noexcept override;
    /// @brief 2311 standard interface
    /// @brief Verify the status of the provided certificate chain solely through locally stored CA certificates and CRLs.
    /// @details The verification status of the certificate chain is Certificate::Status::kValid; only valid if all certificates in the chain have this status!
    /// @details Certificates in the chain (provided by the container vector) must be placed from the Root CA certificate (index 0) to the target end-entity certificate (last used index of the vector). Verification is performed in the same order.
    /// @details If chain verification fails, the status of the first failed certificate is returned. This method updates the Certificate::Status associated with certificates in the chain.
    /// @brief Verify status of the provided certification chain by locally stored CA certificates and CRLs only.
    ///       Verification status of the certificate chain is @c Certificate::Status::kValid only if all certificates in
    ///       the chain have such status! Certificates in the chain (presented by container vector) must be placed from
    ///       the root CA certificate (zero index) to the target end-entity certificate (last used index of the vector).
    ///       Verification is executed in same order. If the chain verification is failed then status of the first
    ///       failed certificate is returned. This method updates the @c Certificate::Status associated with the
    ///       certificates in the chain.
    /// @param chain  target certificate chain for verification
    /// @returns verification status of the provided certificate chain
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_40619}
    /// @uptrace={RS_CRYPTO_02306}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_CRYPTO_
    /// @trace_id_ad=AD_CRYPTO_
    /// @trace_id_dd=DD_CRYPTO_
    /// @needwork = ad
    /// @endcode
    Certificate::Status VerifyCertChain(
        ara::core::Span< std::reference_wrapper< Certificate const > > chain) const noexcept override;
    /// @brief Set the specified CA certificate as a "Trusted Root Certificate".
    /// @details Only certificates saved in volatile or persistent storage will be marked as "Trust Root"!
    /// @details Only CA certificates are trust roots! Multiple certificates on an ECU can be marked as "Trust Root".
    /// @details Only applications with "Trust Administrator" permission have the right to call this method!
    /// @brief Set specified CA certificate as a "root of trust".
    ///       Only a certificate saved to the volatile or persistent storage may be marked as the "root of trust"!
    ///       Only CA certificate can be a "root of trust"!
    ///       Multiple certificates on an ECU may be marked as the "root of trust".
    ///       Only an application with permissions "Trust Master" has the right to call this method!
    /// @param caCert  a valid CA certificate that should be trusted
    /// @return has value if SetAsRootOfTrust sucess false otherwise
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_40625}
    /// @uptrace={RS_CRYPTO_02306}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @error: SecurityErrorDomain::kInvalidArgument        if the provided certificate is invalid
    /// @error: SecurityErrorDomain::kIncompatibleObject     if provided certificate doesn't belong to a CA
    /// @error: SecurityErrorDomain::kAccessViolation        if the method called by an application without the "Trust
    /// Master" permission
    /// @interface_level=unit
    /// @trace_id_sr=SR_CRYPTO_04005
    /// @trace_id_ad=AD_CRYPTO_01407
    /// @trace_id_dd=DD_CRYPTO_03690
    /// @needwork = ad
    /// @endcode
    ara::core::Result< void > SetAsRootOfTrust(Certificate const& caCert) noexcept override;
    /// @brief 2011 standard interface
    /// @brief Check certificate status via directly provided OCSP response. This method can be used to implement "OCSP stapling". This method updates the Certificate::Status associated with the certificate.
    /// @brief Check certificate status by directly provided OCSP response.
    ///       This method may be used for implementation of the "OCSP stapling".
    ///       This method updates the @c Certificate::Status associated with the certificate.
    /// @param cert  a certificate that should be verified
    /// @param ocspResponse  an OCSP response
    /// @returns @c true if the certificate is verified successfully and @c false otherwise
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_40629}
    /// @uptrace={RS_CRYPTO_02306}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @error: SecurityErrorDomain::kInvalidArgument  if the @c cert is invalid
    /// @error: SecurityErrorDomain::kRuntimeFault  if the @c ocspResponse is invalid
    /// @interface_level=unit
    /// @trace_id_sr=SR_CRYPTO_04006
    /// @trace_id_ad=AD_CRYPTO_01408
    /// @trace_id_dd=DD_CRYPTO_03691
    /// @needwork = ad
    /// @endcode
    ara::core::Result< bool > CheckCertStatus(Certificate& cert,
                                              OcspResponse const& ocspResponse) const noexcept override;
    /// @brief 2311 standard interface
    /// @brief Check certificate status via directly provided OCSP response. This method can be used to implement "OCSP stapling". This method updates the Certificate::Status associated with the certificate.
    /// @brief Check certificate status by directly provided OCSP response.
    ///       This method may be used for implementation of the "OCSP stapling".
    ///       This method updates the @c Certificate::Status associated with the certificate.
    /// @param cert  a certificate that should be verified
    /// @param ocspResponse  an OCSP response
    /// @param rootCert  root certificate
    /// @returns @c Certificate status: 2311 standard modified return value to Certificate::Status; to return error codes, returns ara::core::Result<Certificate::Status>
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_40629}
    /// @uptrace={RS_CRYPTO_02306}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @error: SecurityErrorDomain::kInvalidArgument  if the @c cert is invalid
    /// @error: SecurityErrorDomain::kRuntimeFault  if the @c ocspResponse is invalid
    /// @interface_level=unit
    /// @trace_id_sr=SR_CRYPTO_
    /// @trace_id_ad=AD_CRYPTO_
    /// @trace_id_dd=DD_CRYPTO_
    /// @needwork = ad
    /// @endcode
    ara::core::Result< Certificate::Status > CheckCertStatus(Certificate const& cert,
                                                             OcspResponse const& ocspResponse,
                                                             Certificate const& rootCert) const noexcept override;
    /// 2011 standard interface, no change in 2311
    /// @brief Check the status of a certificate list via directly provided OCSP response.
    /// @details This method can be used to implement "OCSP stapling". This method updates the Certificate::Status associated with certificates in the list.
    /// @brief Check status of a certificates list by directly provided OCSP response.
    ///       This method may be used for implementation of the "OCSP stapling".
    ///       This method updates the @c Certificate::Status associated with the certificates in the list.
    /// @param certList  a certificates list that should be verified
    /// @param ocspResponse  an OCSP response
    /// @returns @c true if the certificates list is verified successfully and @c false otherwise
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_40630}
    /// @uptrace={RS_CRYPTO_02306}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @error: SecurityErrorDomain::kInvalidArgument  if the provided certificates are invalid
    /// @error: SecurityErrorDomain::kRuntimeFault  if the @c ocspResponse is invalid
    /// @interface_level=unit
    /// @trace_id_sr=SR_CRYPTO_04006
    /// @trace_id_ad=AD_CRYPTO_01409
    /// @trace_id_dd=DD_CRYPTO_03692
    /// @needwork = ad
    /// @endcode
    ara::core::Result< bool > CheckCertStatus(ara::core::Vector< Certificate* > const& certList,
                                              OcspResponse const& ocspResponse) const noexcept override;

public:
#if AP_VERSION_PUHUA == 20
    /// @brief Import a Certificate Revocation List (CRL) or Delta CRL from a memory BLOB.
    /// @details If some certificates saved in persistent or volatile storage are listed in the imported CRL, their status must be automatically updated to "status::kInvalid".
    /// @details If some of these certificates were already opened during this operation, this status change becomes immediately available (via method call Certificate::GetStatus())!
    /// @details All certificates with status kInvalid should be automatically deleted from the corresponding storage (immediately if not in use, otherwise immediately after close).
    /// @brief Import Certificate Revocation List (CRL) or Delta CRL from a memory BLOB.
    ///       If the imported CRL lists some certificates kept in the persistent or volatile storages then their status
    ///       must be automatically updated to @c Status::kInvalid. If some of these certificates were already openned
    ///       during this operation, then this status change becomes available immediately (via method call
    ///       @c Certificate::GetStatus())!
    ///       All certificates with the status @c kInvalid should be automatically removed from correspondent storages
    ///       (immediately if a certificate not in use now or just after its closing otherwise).
    /// @param crl  serialized CRL or Delta CRL (in form of a BLOB)
    /// @returns has value if import success
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_40620}
    /// @uptrace={RS_CRYPTO_02306}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @error: SecurityErrorDomain::kUnexpectedValue  if the provided BLOB is not a CRL/DeltaCRL
    /// @error: SecurityErrorDomain::kRuntimeFault  if the CRL validation has failed
    /// @interface_level=unit
    /// @trace_id_sr=SR_CRYPTO_04006
    /// @trace_id_ad=AD_CRYPTO_01410
    /// @trace_id_dd=DD_CRYPTO_03693
    /// @needwork = ad
    /// @endcode
    ara::core::Result< bool > ImportCrl(ReadOnlyMemRegion const& crl) noexcept override;
#elif AP_VERSION_PUHUA == 30
    /// @brief Import a Certificate Revocation List (CRL) or Delta CRL from a memory BLOB.
    /// @details If some certificates saved in persistent or volatile storage are listed in the imported CRL, their status must be automatically updated to "status::kInvalid".
    /// @details If some of these certificates were already opened during this operation, this status change becomes immediately available (via method call Certificate::GetStatus())!
    /// @details All certificates with status kInvalid should be automatically deleted from the corresponding storage (immediately if not in use, otherwise immediately after close).
    /// @brief Import Certificate Revocation List (CRL) or Delta CRL from a memory BLOB.
    ///       If the imported CRL lists some certificates kept in the persistent or volatile storages then their status
    ///       must be automatically updated to @c Status::kInvalid. If some of these certificates were already openned
    ///       during this operation, then this status change becomes available immediately (via method call
    ///       @c Certificate::GetStatus())!
    ///       All certificates with the status @c kInvalid should be automatically removed from correspondent storages
    ///       (immediately if a certificate not in use now or just after its closing otherwise).
    /// @param crl  serialized CRL or Delta CRL (in form of a BLOB)
    /// @returns has value if import success
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_40620}
    /// @uptrace={RS_CRYPTO_02306}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @error: SecurityErrorDomain::kUnexpectedValue  if the provided BLOB is not a CRL/DeltaCRL
    /// @error: SecurityErrorDomain::kRuntimeFault  if the CRL validation has failed
    /// @interface_level=unit
    /// @trace_id_sr=SR_CRYPTO_04006
    /// @trace_id_ad=AD_CRYPTO_01410
    /// @trace_id_dd=DD_CRYPTO_03693
    /// @needwork = ad
    /// @endcode
    ara::core::Result< void > ImportCrl(ReadOnlyMemRegion const& crl) noexcept override;
#endif
    /// @brief Import a certificate into volatile or persistent storage.
    /// @details Only imported certificates can be found via search and undergo automatic verification! Certificates can only be imported into one of the storages: volatile or persistent.
    /// @details Therefore, if you import a certificate already saved in persistent storage into volatile storage, nothing changes.
    /// @details However, if you import a certificate already saved in volatile storage into persistent storage, it is "moved" to the persistent domain.
    /// @details If the application successfully imports a certificate corresponding to a CSR existing in storage, that CSR should be deleted.
    /// @brief Import the certificate to volatile or persistent storage.
    ///       Only imported certificate may be found by a search and applied for automatic verifications!
    ///       A certificate can be imported to only one of storage: volatile or persistent. Therefore if you
    ///       import a certificate already kept in the persistent storage to the volatile one then nothing changes.
    ///       But if you import a certificate already kept in the volatile storage to the persistent one then it is
    ///       "moved" to the persistent realm.
    ///       If an application successfully imports a certificate that correspond to a CSR existing in the storage
    ///       then this CSR should be removed.
    /// @param cert  a valid certificate that should be imported
    /// @param iSpecify  optionally a valid InstanceSpecifier can be provided that points to a CertificateSlot
    ///                      for persistent storage of the certificate, otherwise the certificate shall be stored
    ///                      in volatile (session) storage
    /// @return has value if Import sucess false otherwise
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_40621}
    /// @uptrace={RS_CRYPTO_02306}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @error: SecurityErrorDomain::kInvalidArgument        if the provided certificate is invalid
    /// @error: SecurityErrorDomain::kIncompatibleObject     if provided certificate has partial collision with a
    /// matched CSR in the storage
    /// @error: SecurityErrorDomain::kContentDuplication     if the provided certificate already exists in the storage
    /// @error: SecurityErrorDomain::kAccessViolation        if the InstanceSpecifier points to a CertificateSlot, which
    /// the application may only read
    /// @interface_level=unit
    /// @trace_id_sr=SR_CRYPTO_04004
    /// @trace_id_ad=AD_CRYPTO_01411
    /// @trace_id_dd=DD_CRYPTO_03694
    /// @needwork = ad
    /// @endcode
    ara::core::Result< void > Import(
        Certificate const& cert, ara::core::Optional< ara::core::InstanceSpecifier > const& iSpecify) noexcept override;
    /// @brief Import a certificate signing request into persistent storage (Non-standard interface)
    /// @param csr Certificate signing request object
    /// @param iSpecify Target certificate instance specifier
    /// @return has value if ImportCsr sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_CRYPTO_04004
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03695
    /// @needwork = dda
    /// @endcode
    ara::core::Result< void > ImportCsr(
        CertSignRequest const& csr,
        ara::core::Optional< ara::core::InstanceSpecifier > const& iSpecify) noexcept override;
#if AP_VERSION_PUHUA == 20
    /// @brief 2011 standard interface
    /// @brief Remove the specified certificate from storage (volatile or persistent) and destroy it.
    /// @brief Remove specified certificate from the storage (volatile or persistent) and destroy it.
    /// @param cert  a unique smart pointer to a certificate that should be removed
    /// @returns @c true if the certificate was found and removed from the storage, @c false if it was not found
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_40622}
    /// @uptrace={RS_CRYPTO_02306}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_CRYPTO_04004
    /// @trace_id_ad=AD_CRYPTO_01412
    /// @trace_id_dd=DD_CRYPTO_03696
    /// @needwork = ad
    /// @endcode
    bool Remove(Certificate::Uptrc&& cert) noexcept override;
#elif AP_VERSION_PUHUA == 30
    /// @brief 2311 standard interface
    /// @brief Remove the specified certificate from storage (volatile or persistent) and destroy it.
    /// @brief Remove specified certificate from the storage (volatile or persistent) and destroy it.
    /// @param cert  a unique smart pointer to a certificate that should be removed
    /// @returns @c true if the certificate was found and removed from the storage, @c false if it was not found
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_40622}
    /// @uptrace={RS_CRYPTO_02306}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_CRYPTO_04004
    /// @trace_id_ad=AD_CRYPTO_01412
    /// @trace_id_dd=DD_CRYPTO_03696
    /// @needwork = ad
    /// @endcode
    bool Remove(Certificate::Uptrc cert) noexcept override;
#endif

public:
    /// @brief Set the "pending" state associated with the CSR, indicating that the CSR has been sent to the CA.
    /// @details If the CSR is already marked "pending", this method does nothing. Only applications with "CA Connector" permission have the right to call this method!
    /// @brief Set the "pending" status associated to the CSR that means that the CSR already sent to CA.
    ///       This method do nothing if the CSR already marked as "pending".
    ///       Only an application with permissions "CA Connector" has the right to call this method!
    /// @param request  certificate signing request that should be marked as "pending"
    /// @return has value if SetPendingStatus sucess false otherwise
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_40624}
    /// @uptrace={RS_CRYPTO_02306}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @error: SecurityErrorDomain::kAccessViolation    if the method called by an application without the "CA
    /// Connector" permission
    /// @interface_level=unit
    /// @trace_id_sr=SR_CRYPTO_04009
    /// @trace_id_ad=AD_CRYPTO_01413
    /// @trace_id_dd=DD_CRYPTO_03697
    /// @needwork = ad
    /// @endcode
    ara::core::Result< void > SetPendingStatus(CertSignRequest const& request) noexcept override;
    /// @brief Parse a user-provided Certificate Signing Request (CSR).
    /// @brief Parse a certificate signing request (CSR) provided by the user.
    /// @param csr  the buffer containing a certificate signing request
    /// @param withMetaData  specifies the format of the buffer content: @c TRUE means the object has been
    ///                          previously serialized by using the Serializable interface; @c FALSE means
    ///                          the CSR was exported using the CertSignRequest::ExportASN1CertSignRequest() interface
    /// @returns unique smart pointer to the certificate signing request
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_40634}
    /// @uptrace={RS_CRYPTO_02306}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @error: SecurityErrorDomain::kUnsupportedFormat  is returned in case the provided buffer does not contain the
    /// expected format
    /// @interface_level=unit
    /// @trace_id_sr=SR_CRYPTO_04002
    /// @trace_id_ad=AD_CRYPTO_01414
    /// @trace_id_dd=DD_CRYPTO_03698
    /// @needwork = ad
    /// @endcode
    ara::core::Result< CertSignRequest::Uptrc > ParseCertSignRequest(ReadOnlyMemRegion const& csr,
                                                                     bool withMetaData = true) noexcept override;
    /// @brief Create a certificate request for the private key loaded into the context.
    /// @brief Create certification request for a private key loaded to the context.
    /// @param signerCtx  the fully-configured SignerPrivateCtx to be used for signing this certificate request
    /// @param derSubjectDN  the DER-encoded subject distinguished name (DN) of the private key owner
    /// @param x509Extensions  the DER-encoded X.509 Extensions that should be included to the certification request
    /// @param version  the format version of the target certification request
    /// @returns unique smart pointer to created certification request
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_40640}
    /// @uptrace={RS_CRYPTO_02306}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @error: SecurityErrorDomain::kUnexpectedValue    if any of arguments has incorrect/unsupported value
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01415
    /// @trace_id_dd=DD_CRYPTO_03699
    /// @needwork = ad
    /// @endcode
    ara::core::Result< CertSignRequest::Uptrc > CreateCertSignRequest(cryp::SignerPrivateCtx::Uptr signerCtx,
                                                                      ReadOnlyMemRegion const& derSubjectDN,
                                                                      ReadOnlyMemRegion const& x509Extensions
                                                                      = ReadOnlyMemRegion(),
                                                                      uint32_t version = 1U) const noexcept override;

public:
    /// @brief Create an OCSP request for the specified certificate. This method can be used to implement "OCSP stapling".
    /// @brief Create OCSP request for specified certificate.
    ///       This method may be used for implementation of the "OCSP stapling".
    /// @param cert  a certificate that should be verified
    /// @param signer  an optional pointer to initialized signer context (if the request should be signed)
    /// @returns unique smart pointer to the created OCSP request
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_40626}
    /// @uptrace={RS_CRYPTO_02306}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @error: SecurityErrorDomain::kInvalidArgument  if the provided certificate is invalid
    /// @error: SecurityErrorDomain::kIncompleteArgState  if the @c signer context is not initialized by a key
    /// @interface_level=unit
    /// @trace_id_sr=SR_CRYPTO_04006
    /// @trace_id_ad=AD_CRYPTO_01416
    /// @trace_id_dd=DD_CRYPTO_03700
    /// @needwork = ad
    /// @endcode
    ara::core::Result< OcspRequest::Uptrc > CreateOcspRequest(
        Certificate const& cert,
        ara::core::Optional< cryp::SignerPrivateCtx::Uptr const > const& signer) noexcept override;
    /// @brief Create an OCSP request for the specified list of certificates. This method can be used to implement "OCSP stapling".
    /// @brief Create OCSP request for specified list of certificates.
    ///       This method may be used for implementation of the "OCSP stapling".
    /// @param certList  a certificates' list that should be verified
    /// @param signer  an optional pointer to initialized signer context (if the request should be signed)
    /// @returns unique smart pointer to the created OCSP request
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_40627}
    /// @uptrace={RS_CRYPTO_02306}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @error: SecurityErrorDomain::kInvalidArgument  if the provided certificates are invalid
    /// @error: SecurityErrorDomain::kIncompleteArgState  if the @c signer context is not initialized by a key
    /// @interface_level=unit
    /// @trace_id_sr=SR_CRYPTO_04006
    /// @trace_id_ad=AD_CRYPTO_01417
    /// @trace_id_dd=DD_CRYPTO_03701
    /// @needwork = ad
    /// @endcode
    ara::core::Result< OcspRequest::Uptrc > CreateOcspRequest(
        ara::core::Vector< Certificate const* > const& certList,
        ara::core::Optional< cryp::SignerPrivateCtx::Uptr const > const& signer) noexcept override;
    /// @brief Send OCSP request (Non-standard interface)
    /// @param certNames Issuer certificates (possibly multiple)
    /// @param pOcspRequest OCSP request
    /// @return OCSP response data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_CRYPTO_04006
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03702
    /// @needwork = dda
    /// @endcode
    ara::core::Result< ara::core::Vector< ara::core::Byte > > SendOcspRequest(
        ara::core::Vector< ara::core::StringView > const& certNames, OcspRequest* pOcspRequest) noexcept override;
#if AP_VERSION_PUHUA == 20
    /// @brief Parse serialized OCSP response and create the corresponding interface instance. This method can be used to implement "OCSP stapling".
    /// @brief Parse serialized OCSP response and create correspondent interface instance.
    ///       This method may be used for implementation of the "OCSP stapling".
    /// @param response  a serialized OCSP response
    /// @returns unique smart pointer or has value to the created OCSP response instance
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_40628}
    /// @uptrace={RS_CRYPTO_02306}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @error: SecurityErrorDomain::kUnexpectedValue    if the provided BLOB @c response doesn't keep an OCSP response
    /// @interface_level=unit
    /// @trace_id_sr=SR_CRYPTO_04006
    /// @trace_id_ad=AD_CRYPTO_01418
    /// @trace_id_dd=DD_CRYPTO_03703
    /// @needwork = ad
    /// @endcode
    OcspResponse::Uptrc ParseOcspResponse(ReadOnlyMemRegion const& response) const noexcept override;
#elif AP_VERSION_PUHUA == 30
    /// @brief Parse serialized OCSP response and create the corresponding interface instance. This method can be used to implement "OCSP stapling".
    /// @brief Parse serialized OCSP response and create correspondent interface instance.
    ///       This method may be used for implementation of the "OCSP stapling".
    /// @param response  a serialized OCSP response
    /// @returns unique smart pointer or has value to the created OCSP response instance
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_40628}
    /// @uptrace={RS_CRYPTO_02306}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @error: SecurityErrorDomain::kUnexpectedValue    if the provided BLOB @c response doesn't keep an OCSP response
    /// @interface_level=unit
    /// @trace_id_sr=SR_CRYPTO_04006
    /// @trace_id_ad=AD_CRYPTO_01418
    /// @trace_id_dd=DD_CRYPTO_03703
    /// @needwork = ad
    /// @endcode
    ara::core::Result< OcspResponse::Uptrc > ParseOcspResponse(
        ReadOnlyMemRegion const& response) const noexcept override;
#endif

public:
    /// @brief Find a certificate based on Subject and Issuer Distinguished Names (DN).
    ///         The parameter certIndex represents the internal index of the current certificate in the store. If searching for a certificate from the starting position, certIndex needs to be set to
    ///         kInvalidIndex。
    /// @brief Find a certificate by the subject and issuer Distinguished Names (DN).
    ///       Argument @c certIndex represents an internal index of current certificate in the storage.
    ///       In order to start certificate search from begin, set: <tt>certIndex = kInvalidIndex_</tt>
    /// @param subjectDn  subject DN of the target certificate
    /// @param issuerDn  issuer DN of the target certificate
    /// @param validityTimePoint  a time point when the target certificate should be valid
    /// @param certIndex  an index for iteration through all suitable certificates in the storage
    ///                (input: index of previous found cerificate, output: index of current found cerificate)
    /// @returns unique smart pointer to found certificate or @c nullptr if nothing is found
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_40631}
    /// @uptrace={RS_CRYPTO_02306}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_CRYPTO_04007
    /// @trace_id_ad=AD_CRYPTO_01419
    /// @trace_id_dd=DD_CRYPTO_03704
    /// @needwork = ad
    /// @endcode
    Certificate::Uptrc FindCertByDn(X509DN const& subjectDn,
                                    X509DN const& issuerDn,
                                    time_t validityTimePoint,
                                    StorageIndex& certIndex) noexcept override;
    /// @brief Find a certificate by its SKID and AKID.
    /// @brief Find a certificate by its SKID & AKID.
    /// @param subjectKeyId  subject key identifier (SKID)
    /// @param authorityKeyId  optional authority key identifier (AKID)
    /// @returns unique smart pointer to found certificate or @c nullptr if nothing is found
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_40632}
    /// @uptrace={RS_CRYPTO_02306}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_CRYPTO_04007
    /// @trace_id_ad=AD_CRYPTO_01420
    /// @trace_id_dd=DD_CRYPTO_03705
    /// @needwork = ad
    /// @endcode
    Certificate::Uptrc FindCertByKeyIds(ReadOnlyMemRegion const& subjectKeyId,
                                        ReadOnlyMemRegion const& authorityKeyId
                                        = ReadOnlyMemRegion()) noexcept override;
    /// @brief Find a certificate based on its serial number.
    /// @brief Find a certificate by its serial number.
    /// @param sn  serial number of the target certificate
    /// @param issuerDn  authority's Distinguished Names (DN)
    /// @returns unique smart pointer to a found certificate or @c nullptr if nothing is found
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_40633}
    /// @uptrace={RS_CRYPTO_02306}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_CRYPTO_04007
    /// @trace_id_ad=AD_CRYPTO_01421
    /// @trace_id_dd=DD_CRYPTO_03706
    /// @needwork = ad
    /// @endcode
    Certificate::Uptrc FindCertBySn(ReadOnlyMemRegion const& sn, X509DN const& issuerDn) noexcept override;

public:
    /// @brief Clear volatile certificate storage.
    /// After executing this command, certificates previously imported into volatile storage cannot be found via search, but it does not affect already loaded certificate instances!
    /// @brief Cleanup the volatile certificates storage.
    ///       After execution of this command the certificates previously imported to the volatile storage cannot be
    ///       found by a search, but it doesn't influence to already loaded @c Certificate instances!
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_40635}
    /// @uptrace={RS_CRYPTO_02306}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01422
    /// @trace_id_dd=DD_CRYPTO_03707
    /// @needwork = ad
    /// @endcode
    void CleanupVolatileStorage() noexcept override;
    /// @brief Create an empty X.509 Extensions structure. If (0 == capacity), the maximum supported capacity must be reserved (by the implementation).
    /// @brief Create an empty X.509 Extensions structure.
    ///       If <tt>(0 == capacity)</tt> then a maximally supported (by the implementation) capacity must be reserved.
    /// @param capacity  number of bytes that should be reserved for the content of the target @c X509Extensions
    /// object
    /// @returns Shared smart pointer to created empty @c X509X509Extensions object
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_40636}
    /// @uptrace={RS_CRYPTO_02306}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_CRYPTO_04010
    /// @trace_id_ad=AD_CRYPTO_01423
    /// @trace_id_dd=DD_CRYPTO_03708
    /// @needwork = ad
    /// @endcode
    ara::core::Result< X509Extensions::Uptr > CreateEmptyExtensions(std::size_t capacity = 0U) noexcept override;

public:
    /// @brief Load a certificate from persistent certificate storage.
    /// @brief Load a certificate from the persistent certificate storage.
    /// @param iSpecify  the target certificate instance specifier
    /// @returns an unique smart pointer to the instantiated certificate
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_40641}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @error: SecurityErrorDomain::kUnreservedResource  if the InstanceSpecifier is incorrect (the certificate cannot
    /// be found)
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01424
    /// @trace_id_dd=DD_CRYPTO_03709
    /// @needwork = ad
    /// @endcode
    ara::core::Result< Certificate::Uptr > LoadCertificate(ara::core::InstanceSpecifier& iSpecify) noexcept override;
    /// @brief Load a Certificate Signing Request (CSR) from persistent certificate storage (non-standard interface)
    /// @param iSpecify Target certificate instance specifier
    /// @return Certificate Signing Request instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03710
    /// @needwork = dda
    /// @endcode
    ara::core::Result< CertSignRequest::Uptr > LoadCertSignRequest(
        ara::core::InstanceSpecifier& iSpecify) noexcept override;
    /// @brief Get the slot name corresponding to the certificate (non-standard interface)
    /// @param iSpecify Target certificate instance specifier
    /// @return Port name of the key slot corresponding to the certificate
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03711
    /// @needwork = dda
    /// @endcode
    ara::core::Result< ara::core::Vector< ara::core::String > > GetCertificateSlots(
        ara::core::InstanceSpecifier& iSpecify) noexcept override;
    /// @brief Check if the Certificate Signing Request has the pending state set (non-standard interface)
    /// @param request Certificate request
    /// @return has value if has already set pedding false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03712
    /// @needwork = dda
    /// @endcode
    ara::core::Result< bool > IsCsrPenddingStatus(CertSignRequest const& request) noexcept override;
    /// @brief Default virtual destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03713
    /// @needwork = dda
    /// @endcode
    ~PX509Provider() noexcept override = default;
    /// @brief Default copy assignment operator
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03714
    /// @needwork = dda
    /// @endcode
    PX509Provider& operator=(PX509Provider const& other) = delete;
    /// @brief Default move assignment operator
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03715
    /// @needwork = dda
    /// @endcode
    PX509Provider& operator=(PX509Provider&& other) = delete;
    /// @brief Default copy constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03716
    /// @needwork = dda
    /// @endcode
    PX509Provider(PX509Provider const& other) = delete;
    /// @brief Default move constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03717
    /// @needwork = dda
    /// @endcode
    PX509Provider(PX509Provider&& other) = delete;

private:
    /// @brief Save the loaded certificate:
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03718
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< PCertificate::Uptr > vecCertVolatile_;
    /// @brief Save the loaded Certificate Signing Request:
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03719
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< CertSignRequest::Uptr > vecCsrVolatile_;
    /// @brief Save the loaded certificate:
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03720
    /// @needwork = dda
    /// @endcode
    mutable ara::core::Vector< Certificate::Uptr > vecCertRootOfTrust_;

protected:
    /// @brief Verify a certificate chain
    /// @param chain Certificate chain
    /// @param myRoot Root certificate
    /// @return Certificate status
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03721
    /// @needwork = dda
    /// @endcode
    static Certificate::Status VerifyCertChain2(ara::core::Span< std::reference_wrapper< Certificate const > > chain,
                                                Certificate const* const myRoot) noexcept;
    /// @brief Extract common logic for standard interfaces 2011 and 2311 into _VerifyCertChain
    /// @param chain  target certificate chain for verification
    /// @param myRoot  root certificate to be used for verification - if this is nullptr, use machine root
    /// certificates
    /// @returns verification status of the provided certificate chain
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @uptrace={RS_CRYPTO_}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_04352
    /// @trace_id_dd=DD_CRYPTO_08782
    /// @needwork = ad
    /// @endcode
    Certificate::Status _VerifyCertChain(ara::core::Span< std::reference_wrapper< Certificate const > > const& chain,
                                         Certificate::Uptr const myRoot = {nullptr}) const noexcept;
    /// @brief Get the issuing certificate corresponding to the given certificate
    /// @param cert Certificate object
    /// @return Pointer to certificate structure
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03722
    /// @needwork = dda
    /// @endcode
    Certificate* _GetIssureCertificate(Certificate const& cert) const noexcept;
    /// @brief Used to implement the core logic of the standard interface ParseOcspResponse
    /// @param response  a serialized OCSP response
    /// @returns unique smart pointer to the created OCSP response instance
    // PRQA S 2024 QAC /// @qac: False positive
    ara::core::Result< OcspResponse::Uptrc > _ParseOcspResponse(ReadOnlyMemRegion const& response) const noexcept;
    // PRQA L:QAC

public:
    /// @brief Default constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03723
    /// @needwork = dda
    /// @endcode
    PX509Provider() = default;

private:
    /// @brief Locally stored list of serial numbers for certificates that have been revoked
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03724
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< ara::core::Vector< uint8_t > > localCRLists_;

protected:
    /// @brief Check certificate validity based on the local CRL list
    /// @name  _CheckCertRevoked
    /// @param cert Certificate object
    /// @return true or false
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03725
    /// @needwork = dda
    /// @endcode
    ara::core::Result< bool > _CheckCertRevoked(Certificate const& cert) const noexcept;
    /// @brief Check certificate status via directly provided OCSP response. This method can be used to implement "OCSP
    /// stapling". This method updates the Certificate::Status associated with the certificate.
    /// @param cert  certificate
    /// @param ocspResponse  an OCSP response
    /// @return true or false
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kInvalidArgument  if the @c cert is invalid
    /// @error: SecurityErrorDomain::kRuntimeFault  if the @c ocspResponse is invalid
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_04359
    /// @trace_id_dd=DD_CRYPTO_08814
    /// @needwork = ad
    /// @endcode
    ara::core::Result< Certificate::Status > _CheckCertStatus(Certificate const& cert,
                                                              OcspResponse const& ocspResponse) const noexcept;
    /// @brief Core logic for the Remove function, suitable for calls from both 2011 and 2311 standard interfaces
    /// @param cert  a unique smart pointer to a certificate that should be removed
    /// @returns @c true if the certificate was found and removed from the storage, @c false if it was not found
    bool _Remove(Certificate const* const cert) noexcept;
    /// @brief Encapsulate logic for saving loaded CSR into this function to reduce complexity of the Import function
    void _PushToCsrVolatile() noexcept;
    /// @brief Check for partial conflicts between the provided certificate and matching CSRs in the store
    /// @param cert Provided certificate
    /// @return true indicates check passed, false indicates conflict exists
    bool _CheckCollision(Certificate const& cert) noexcept;
    /// @brief Implement core logic for ImportCrl
    /// @param crl  serialized CRL or Delta CRL (in form of a BLOB)
    /// @returns no return value
    ara::core::Result< bool > _ImportCrl(ReadOnlyMemRegion const& crl) noexcept;

public:
    /// @brief External interface for revoking certificates
    /// @param revokedISpecify
    /// @return has vlaue if revoke sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03726
    /// @needwork = dda
    /// @endcode
    ara::core::Result< bool > RevokeCertificate(ara::core::InstanceSpecifier& revokedISpecify) noexcept override;
    /// @brief External interface for generating CRL
    /// @param iSpecify Target certificate instance specifier
    /// @return Certificate Revocation List information
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03727
    /// @needwork = dda
    /// @endcode
    ara::core::Result< ara::core::Vector< ara::core::Byte > > GenerateCertRevokeList(
        ara::core::InstanceSpecifier& iSpecify) const noexcept override;

private:
    /// @brief Return a non-const reference to self via a const function
    /// @return Non-const self
    PX509Provider& _getSelf() const noexcept;
};
//********************************/
}  // namespace  isoft_def
}  // namespace x509
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_X509_PUHUA_X509_PROVIDER_H_
