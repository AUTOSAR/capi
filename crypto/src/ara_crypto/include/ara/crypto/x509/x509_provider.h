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
/// @file       x509_provider.h
/// @brief      AutoSar-Crypto Certificate Management Module
/// @details    X.509 Provider interface.
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
/// @module_path=/CRYPTO/Certificate Components/Certificate Provider
/// @interface_level=software
/// @trace_id_sr=SR_CRYPTO_04001
/// @unit_name=X509Provider
/// @unit_description=Certificate Provider
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_X509_X509_PROVIDER_H_
#define ARA_CRYPTO_X509_X509_PROVIDER_H_

#include "ara/core/instance_specifier.h"
#include "ara/core/optional.h"
#include "ara/core/result.h"
#include "ara/core/vector.h"
#include "ara/crypto/cryp/signer_private_ctx.h"
#include "ara/crypto/x509/cert_sign_request.h"
#include "ara/crypto/x509/certificate.h"
#include "ara/crypto/x509/ocsp_request.h"
#include "ara/crypto/x509/ocsp_response.h"
#include "ara/crypto/x509/x509_dn.h"
#include "ara/crypto/x509/x509_extensions.h"

#define AP_VERSION_PUHUA 20
namespace ara {
namespace crypto {
namespace x509 {
//********************************/
//- @interface X509Provider
/// @brief X.509 Provider interface. The X.509 Provider supports two types of internal storage: volatile (or session-level) and persistent.
///         All X.509 objects created by the provider should have an actual reference to their parent X.509 Provider. The X.509
///         Provider can only be destroyed after all its child objects have been destroyed.
///         Each method of this interface that creates an X.509 object is non-const, because any such creation will increment the reference counter of the X.509 Provider.
/// @brief X.509 Provider interface.
///       The X.509 Provider supports two internal storages: volatile (or session) and persistent.
///       All X.509 objects created by the provider should have an actual reference to their parent X.509 Provider.
///       The X.509 Provider can be destroyed only after destroying of all its daughterly objects.
///       Each method of this interface that creates a X.509 object is non-constant, because any such creation increases
///       a references counter of the X.509 Provider.
/// @code{.isoft}
/// @trace_id_sws={SWS_CRYPT_40600}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02306}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02714
/// @trace_id_dd=DD_CRYPTO_05571
/// @needwork = ad
/// @endcode
class X509Provider
{
public:
    /// @brief Unique smart pointer for the interface.    //Comment and declaration do not match
    /// @brief Shared smart pointer of the interface.
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_40601}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02306}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_03238
    /// @trace_id_dd=DD_CRYPTO_06450
    /// @needwork = ad
    /// @endcode
    using Uptr = std::unique_ptr< X509Provider >;
    /// @brief Type for internal indices in the certificate store.
    /// @brief Type of an internal index inside the certificate storage.
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_40602}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02306}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_03239
    /// @trace_id_dd=DD_CRYPTO_06451
    /// @needwork = ad
    /// @endcode
    using StorageIndex = std::size_t;

private:
    /// @brief Reserved "invalid index" value for navigation within the certificate store.
    /// @brief Reserved "invalid index" value for navigation inside the certificate storage.
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_40603}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02306}
    /// static const StorageIndex kInvalidIndex_ = static_cast<std::size_t>(-1LL);
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05572
    /// @needwork = dda
    /// @endcode
    StorageIndex const kInvalidIndex{static_cast< std::size_t >(SIZE_MAX)};

public:
    /// @brief Get the "invalid index" value reserved for navigation within the certificate store.
    /// @return
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02715
    /// @trace_id_dd=DD_CRYPTO_05573
    /// @needwork = ad
    /// @endcode
    StorageIndex GetkInvalidIndex() const noexcept { return kInvalidIndex; }

public:
    /// @brief Create a complete X.500 Distinguished Name structure from the provided string representation.
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kInvalidArgument  if the @c dn argument has incorrect format
    /// @error: SecurityErrorDomain::kInvalidInputSize  if the @c dn argument has unsupported length (too large)
    /// @trace_id_sws={SWS_CRYPT_40612}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02306}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02716
    /// @trace_id_dd=DD_CRYPTO_05574
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< X509DN::Uptrc >
    /// @brief Create completed X.500 Distinguished Name structure from the provided string representation.
    /// @name BuildDn
    /// @param dn string representation of the Distinguished Name
    /// @returns unique smart pointer for the created @c X509DN object
    BuildDn(ara::core::StringView const& dn) noexcept = 0;
    /// @brief 2011 standard interface
    /// @brief Check the certificate status via a directly provided OCSP response. This method can be used to implement "OCSP
    /// stapling". This method updates the Certificate::Status associated with the certificate.
    /// @brief Check certificate status by directly provided OCSP response.
    ///       This method may be used for implementation of the "OCSP stapling".
    ///       This method updates the @c Certificate::Status associated with the certificate.
    /// @param cert  a certificate that should be verified
    /// @param ocspResponse  an OCSP response
    /// @returns @c true if the certificate is verified successfully and @c false otherwise
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kInvalidArgument  if the @c cert is invalid
    /// @error: SecurityErrorDomain::kRuntimeFault  if the @c ocspResponse is invalid
    /// @trace_id_sws={SWS_CRYPT_40629}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02306}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_02717
    /// @trace_id_dd=DD_CRYPTO_05575
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< bool > CheckCertStatus(Certificate& cert,
                                                      OcspResponse const& ocspResponse) const noexcept = 0;
    /// @brief 2311 standard interface
    /// @brief Check the certificate status via a directly provided OCSP response. This method can be used to implement "OCSP
    /// stapling". This method updates the Certificate::Status associated with the certificate.
    /// @brief Check certificate status by directly provided OCSP response.
    ///       This method may be used for implementation of the "OCSP stapling".
    ///       This method updates the @c Certificate::Status associated with the certificate.
    /// @param cert  a certificate that should be verified
    /// @param ocspResponse  an OCSP response
    /// @param rootCert  root certificate
    /// @returns @c Status of the certificate: The 2311 standard changed the return value to Certificate::Status, to allow returning error codes, returns ara::core::Result< Certificate::Status >
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kInvalidArgument  if the @c cert is invalid
    /// @error: SecurityErrorDomain::kRuntimeFault  if the @c ocspResponse is invalid
    /// @trace_id_sws={SWS_CRYPT_40629}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02306}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_
    /// @trace_id_dd=DD_CRYPTO_
    /// @needwork = ad
    /// @endcode
    // PRQA S 2024 QAC /// @qac: False positive
    virtual ara::core::Result< Certificate::Status > CheckCertStatus(Certificate const& cert,
                                                                     OcspResponse const& ocspResponse,
                                                                     Certificate const& rootCert) const noexcept = 0;
    // PRQA L:QAC
    /// @brief Check the status of a list of certificates via a directly provided OCSP response. This method can be used to implement "OCSP
    /// stapling". This method updates the Certificate::Status associated with the certificates in the list.
    /// @brief Check status of a certificates list by directly provided OCSP response.
    ///       This method may be used for implementation of the "OCSP stapling".
    ///       This method updates the @c Certificate::Status associated with the certificates in the list.
    /// @param certList  a certificates list that should be verified
    /// @param ocspResponse  an OCSP response
    /// @returns @c true if the certificates list is verified successfully and @c false otherwise
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kInvalidArgument  if the provided certificates are invalid
    /// @error: SecurityErrorDomain::kRuntimeFault  if the @c ocspResponse is invalid
    /// @trace_id_sws={SWS_CRYPT_40630}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02306}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02718
    /// @trace_id_dd=DD_CRYPTO_05576
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< bool > CheckCertStatus(ara::core::Vector< Certificate* > const& certList,
                                                      OcspResponse const& ocspResponse) const noexcept = 0;
    /// @brief Clear the volatile certificate store.
    /// After executing this command, certificates previously imported into volatile storage will not be found via search, but it will not affect already loaded certificate instances!
    /// @brief Cleanup the volatile certificates storage.
    ///       After execution of this command the certificates previously imported to the volatile storage cannot be
    ///       found by a search, but it doesn't influence to already loaded @c Certificate instances!
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_40635}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02306}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02719
    /// @trace_id_dd=DD_CRYPTO_05577
    /// @needwork = ad
    /// @endcode
    virtual void CleanupVolatileStorage() noexcept = 0;
    /// @brief Create a certificate request for the private key loaded into the context.
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kUnexpectedValue    if any of arguments has incorrect/unsupported value
    /// @trace_id_sws={SWS_CRYPT_40640}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02306}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02720
    /// @trace_id_dd=DD_CRYPTO_05578
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< CertSignRequest::Uptrc >
    /// @brief Create certification request for a private key loaded to the context.
    /// @param signerCtx  the fully-configured SignerPrivateCtx to be used for signing this certificate request
    /// @param derSubjectDN  the DER-encoded subject distinguished name (DN) of the private key owner
    /// @param x509Extensions  the DER-encoded X.509 Extensions that should be included to the certification request
    /// @param version  the format version of the target certification request
    /// @returns unique smart pointer to created certification request
    CreateCertSignRequest(cryp::SignerPrivateCtx::Uptr signerCtx,
                          ReadOnlyMemRegion const& derSubjectDN,
                          ReadOnlyMemRegion const& x509Extensions = ReadOnlyMemRegion(),
                          uint32_t version                        = 1U) const noexcept = 0;
    /// @brief Count the number of certificates in a serialized certificate chain represented by a single BLOB.
    /// @brief Count number of certificates in a serialized certificate chain represented by a single BLOB.
    /// @param certChain  DER/PEM-encoded certificate chain in form of a single BLOB
    /// @param formatId  input format identifier @c kFormatDefault means auto-detect
    /// @returns number of certificates in the chain
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_40615}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02306}
    /// @error: SecurityErrorDomain::kInvalidArgument  if the @c certChain argument cannot be pre-parsed
    /// @error: SecurityErrorDomain::kUnknownIdentifier  if the @c formatId argument has unknown value
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02721
    /// @trace_id_dd=DD_CRYPTO_05579
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< std::size_t > CountCertsInChain(ReadOnlyMemRegion const& certChain,
                                                               Serializable::FormatId formatId
                                                               = Serializable::kFormatDefault) const noexcept = 0;
    /// @brief Create an empty X.500 DN (Distinguished Name) structure. If (0 == capacity), the maximum supported capacity (by implementation) must be reserved.
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_40611}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02306}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02722
    /// @trace_id_dd=DD_CRYPTO_05580
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< X509DN::Uptr >
    /// @brief Create an empty X.500 Distinguished Name (DN) structure.
    ///       If <tt>(0 == capacity)</tt> then a maximally supported (by the implementation) capacity must be reserved.
    /// @param capacity  number of bytes that should be reserved for the content of the target @c X509DN object
    /// @returns Unique smart pointer to created empty @c X509DN object
    CreateEmptyDn(std::size_t capacity = 0U) noexcept = 0;
    /// @brief Create an empty X.509 Extensions structure. If (0 == capacity), the maximum supported capacity (by implementation) must be reserved.
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_40636}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02306}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02723
    /// @trace_id_dd=DD_CRYPTO_05581
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< X509Extensions::Uptr >
    /// @brief Create an empty X.509 Extensions structure.
    ///       If <tt>(0 == capacity)</tt> then a maximally supported (by the implementation) capacity must be reserved.
    /// @param capacity  number of bytes that should be reserved for the content of the target @c X509Extensions
    /// object
    /// @returns Shared smart pointer to created empty @c X509X509Extensions object
    CreateEmptyExtensions(std::size_t capacity = 0U) noexcept = 0;
    /// @brief Create an OCSP request for the specified certificate. This method can be used to implement "OCSP stapling".
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kInvalidArgument  if the provided certificate is invalid
    /// @error: SecurityErrorDomain::kIncompleteArgState  if the @c signer context is not initialized by a key
    /// @trace_id_sws={SWS_CRYPT_40626}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02306}
    /// @threadsafety={Thread-safe}
    /// @TODO: 2022-01-13 The type of function parameter @c signer is highly suspected to be ara::core::Optional<const
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02724
    /// @trace_id_dd=DD_CRYPTO_05582
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< OcspRequest::Uptrc >
    /// @brief Create OCSP request for specified certificate.
    ///       This method may be used for implementation of the "OCSP stapling".
    /// @param cert  a certificate that should be verified
    /// @param signer  an optional pointer to initialized signer context (if the request should be signed)
    /// @returns unique smart pointer to the created OCSP request
    CreateOcspRequest(Certificate const& cert,
                      ara::core::Optional< cryp::SignerPrivateCtx::Uptr const > const& signer) noexcept = 0;
    /// @brief Create an OCSP request for the specified list of certificates. This method can be used to implement "OCSP stapling".
    /// @brief Create OCSP request for specified list of certificates.
    ///       This method may be used for implementation of the "OCSP stapling".
    /// @param certList  a certificates' list that should be verified
    /// @param signer  an optional pointer to initialized signer context (if the request should be signed)
    /// @returns unique smart pointer to the created OCSP request
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kInvalidArgument  if the provided certificates are invalid
    /// @error: SecurityErrorDomain::kIncompleteArgState  if the @c signer context is not initialized by a key
    /// @trace_id_sws={SWS_CRYPT_40627}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02306}
    /// @threadsafety={Thread-safe}
    /// @TODO: 2022-01-13 The type of function parameter @c signer is highly suspected to be ara::core::Optional<const
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02725
    /// @trace_id_dd=DD_CRYPTO_05583
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< OcspRequest::Uptrc > CreateOcspRequest(
        ara::core::Vector< Certificate const* > const& certList,
        ara::core::Optional< cryp::SignerPrivateCtx::Uptr const > const& signer) noexcept = 0;
    /// @brief Send an OCSP request (non-standard interface)
    /// @param certNames Issuer certificate (possibly multiple)
    /// @param pOcspRequest OCSP request
    /// @return
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02726
    /// @trace_id_dd=DD_CRYPTO_05584
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< ara::core::Vector< ara::core::Byte > > SendOcspRequest(
        ara::core::Vector< ara::core::StringView > const& certNames, OcspRequest* pOcspRequest) noexcept = 0;
    /// @brief Decode an X.500 Distinguished Name structure from the provided serialized format.
    /// @brief Decode X.500 Distinguished Name structure from the provided serialized format.
    /// @param dn  DER/PEM-encoded representation of the Distinguished Name
    /// @param formatId  input format identifier @c kFormatDefault means auto-detect
    /// @returns unique smart pointer for the created @c X509DN object
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kInvalidArgument  if the @c dn argument cannot be parsed
    /// @error: SecurityErrorDomain::kUnknownIdentifier  if the @c formatId argument has unknown value
    /// @trace_id_sws={SWS_CRYPT_40613}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02306}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02727
    /// @trace_id_dd=DD_CRYPTO_05585
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< X509DN::Uptrc > DecodeDn(ReadOnlyMemRegion const& dn,
                                                        Serializable::FormatId formatId
                                                        = Serializable::kFormatDefault) noexcept = 0;
    /// @brief Find a certificate by the Distinguished Names (DN) of the subject and issuer.
    ///         The parameter certIndex indicates the internal index of the current certificate in the store. If starting certificate search from the beginning, you need to set certIndex =
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
    /// @trace_id_sws={SWS_CRYPT_40631}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02306}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02728
    /// @trace_id_dd=DD_CRYPTO_05586
    /// @needwork = ad
    /// @endcode
    virtual Certificate::Uptrc FindCertByDn(X509DN const& subjectDn,
                                            X509DN const& issuerDn,
                                            time_t validityTimePoint,
                                            StorageIndex& certIndex) noexcept = 0;
    /// @brief Find a certificate by its SKID and AKID.
    /// @brief Find a certificate by its SKID & AKID.
    /// @param subjectKeyId  subject key identifier (SKID)
    /// @param authorityKeyId  optional authority key identifier (AKID)
    /// @returns unique smart pointer to found certificate or @c nullptr if nothing is found
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_40632}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02306}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02729
    /// @trace_id_dd=DD_CRYPTO_05587
    /// @needwork = ad
    /// @endcode
    virtual Certificate::Uptrc FindCertByKeyIds(ReadOnlyMemRegion const& subjectKeyId,
                                                ReadOnlyMemRegion const& authorityKeyId
                                                = ReadOnlyMemRegion()) noexcept = 0;
    /// @brief Find a certificate by its serial number.
    /// @brief Find a certificate by its serial number.
    /// @param sn  serial number of the target certificate
    /// @param issuerDn  authority's Distinguished Names (DN)
    /// @returns unique smart pointer to a found certificate or @c nullptr if nothing is found
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_40633}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02306}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02730
    /// @trace_id_dd=DD_CRYPTO_05588
    /// @needwork = ad
    /// @endcode
    virtual Certificate::Uptrc FindCertBySn(ReadOnlyMemRegion const& sn, X509DN const& issuerDn) noexcept = 0;
    /// @brief Parse a user-provided Certificate Signing Request (CSR).
    /// @brief Parse a certificate signing request (CSR) provided by the user.
    /// @param csr  the buffer containing a certificate signing request
    /// @param withMetaData  specifies the format of the buffer content: @c TRUE means the object has been
    ///                          previously serialized by using the Serializable interface; @c FALSE means
    ///                          the CSR was exported using the CertSignRequest::ExportASN1CertSignRequest() interface
    /// @returns unique smart pointer to the certificate signing request
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kUnsupportedFormat  is returned in case the provided buffer does not contain the
    /// expected format
    /// @trace_id_sws={SWS_CRYPT_40634}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02306}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02731
    /// @trace_id_dd=DD_CRYPTO_05589
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< CertSignRequest::Uptrc > ParseCertSignRequest(ReadOnlyMemRegion const& csr,
                                                                             bool withMetaData = true) noexcept = 0;
#if AP_VERSION_PUHUA == 20
    /// @brief Import a Certificate Revocation List (CRL) or Delta CRL from a memory BLOB.
    ///         If the imported CRL lists some certificates stored in persistent or volatile storage, the status of those certificates must be automatically updated to "status::kInvalid".
    ///         If some of these certificates are already opened during this operation, this status change will be immediately available (via the Certificate::GetStatus() method call)!
    ///         All certificates with status kInvalid should be automatically deleted from the corresponding storage (immediately if the certificate is not currently in use, otherwise immediately after closing).
    /// @brief Import Certificate Revocation List (CRL) or Delta CRL from a memory BLOB.
    ///       If the imported CRL lists some certificates kept in the persistent or volatile storages then their status
    ///       must be automatically updated to @c Status::kInvalid. If some of these certificates were already openned
    ///       during this operation, then this status change becomes available immediately (via method call
    ///       @c Certificate::GetStatus())!
    ///       All certificates with the status @c kInvalid should be automatically removed from correspondent storages
    ///       (immediately if a certificate not in use now or just after its closing otherwise).
    /// @param crl  serialized CRL or Delta CRL (in form of a BLOB)
    /// @returns  has value if import success
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_40620}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02306}
    /// @error: SecurityErrorDomain::kUnexpectedValue  if the provided BLOB is not a CRL/DeltaCRL
    /// @error: SecurityErrorDomain::kRuntimeFault  if the CRL validation has failed
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02732
    /// @trace_id_dd=DD_CRYPTO_05590
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< bool > ImportCrl(ReadOnlyMemRegion const& crl) noexcept = 0;
#elif AP_VERSION_PUHUA == 30
    /// @brief Import a Certificate Revocation List (CRL) or Delta CRL from a memory BLOB.
    ///         If the imported CRL lists some certificates stored in persistent or volatile storage, the status of those certificates must be automatically updated to "status::kInvalid".
    ///         If some of these certificates are already opened during this operation, this status change will be immediately available (via the Certificate::GetStatus() method call)!
    ///         All certificates with status kInvalid should be automatically deleted from the corresponding storage (immediately if the certificate is not currently in use, otherwise immediately after closing).
    /// @brief Import Certificate Revocation List (CRL) or Delta CRL from a memory BLOB.
    ///       If the imported CRL lists some certificates kept in the persistent or volatile storages then their status
    ///       must be automatically updated to @c Status::kInvalid. If some of these certificates were already openned
    ///       during this operation, then this status change becomes available immediately (via method call
    ///       @c Certificate::GetStatus())!
    ///       All certificates with the status @c kInvalid should be automatically removed from correspondent storages
    ///       (immediately if a certificate not in use now or just after its closing otherwise).
    /// @param crl  serialized CRL or Delta CRL (in form of a BLOB)
    /// @returns  has value if import success
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_40620}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02306}
    /// @error: SecurityErrorDomain::kUnexpectedValue  if the provided BLOB is not a CRL/DeltaCRL
    /// @error: SecurityErrorDomain::kRuntimeFault  if the CRL validation has failed
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02732
    /// @trace_id_dd=DD_CRYPTO_05590
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< void > ImportCrl(ReadOnlyMemRegion const& crl) noexcept = 0;
#endif
    /// @brief Import a certificate into volatile or persistent storage.
    ///         Only imported certificates can be found by search and subject to automatic verification! Certificates can only be imported into one type of storage: volatile storage and persistent storage.
    ///         Therefore, if you import a certificate already saved in persistent storage into volatile storage, nothing changes.
    ///         However, if you import a certificate already saved in volatile storage into persistent storage, it is "moved" to the persistent domain.
    ///         If the application successfully imports a certificate corresponding to a CSR present in the store, that CSR should be deleted.
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
    /// @return
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kInvalidArgument        if the provided certificate is invalid
    /// @error: SecurityErrorDomain::kIncompatibleObject     if provided certificate has partial collision with a
    /// matched CSR in the storage
    /// @error: SecurityErrorDomain::kContentDuplication     if the provided certificate already exists in the storage
    /// @error: SecurityErrorDomain::kAccessViolation        if the InstanceSpecifier points to a CertificateSlot, which
    /// the application may only read
    /// @trace_id_sws={SWS_CRYPT_40621}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02306}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02733
    /// @trace_id_dd=DD_CRYPTO_05591
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< void > Import(
        Certificate const& cert, ara::core::Optional< ara::core::InstanceSpecifier > const& iSpecify) noexcept = 0;
    /// @brief Import a certificate signing request into persistent storage (non-standard interface)
    /// @param csr Certificate signing request
    /// @param iSpecify Target certificate instance specifier
    /// @return
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02734
    /// @trace_id_dd=DD_CRYPTO_05592
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< void > ImportCsr(
        CertSignRequest const& csr, ara::core::Optional< ara::core::InstanceSpecifier > const& iSpecify) noexcept = 0;
    /// @brief Load a certificate from persistent certificate storage.
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kUnreservedResource  if the InstanceSpecifier is incorrect (the certificate cannot
    /// be found)
    /// @trace_id_sws={SWS_CRYPT_40641}
    /// @tracestatus={draft}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02735
    /// @trace_id_dd=DD_CRYPTO_05593
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< Certificate::Uptr >
    /// @brief Load a certificate from the persistent certificate storage.
    /// @param iSpecify  the target certificate instance specifier
    /// @returns an unique smart pointer to the instantiated certificate
    LoadCertificate(ara::core::InstanceSpecifier& iSpecify) noexcept = 0;
    /// @brief Load a certificate signing request from persistent certificate storage (non-standard interface)
    /// @param iSpecify Target certificate instance specifier
    /// @return Certificate signing request instance object loaded from persistent file
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02736
    /// @trace_id_dd=DD_CRYPTO_05594
    /// @needwork = ad
    /// @endcode
    // PRQA S 2024 QAC /// @qac: False positive
    virtual ara::core::Result< CertSignRequest::Uptr > LoadCertSignRequest(
        ara::core::InstanceSpecifier& iSpecify) noexcept = 0;
    // PRQA L:QAC
    /// @brief Return the list of key slots bound to the certificate
    /// @param iSpecify Target certificate instance specifier
    /// @return List of key slot names corresponding to the certificate
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02737
    /// @trace_id_dd=DD_CRYPTO_05595
    /// @needwork = ad
    /// @endcode
    // PRQA S 2024 QAC /// @qac: False positive
    virtual ara::core::Result< ara::core::Vector< ara::core::String > > GetCertificateSlots(
        ara::core::InstanceSpecifier& iSpecify) noexcept = 0;
    // PRQA L:QAC
    /// @brief 2011 standard interface
    /// @brief Parse the serialized representation of a certificate chain and create instances of them.
    ///         Offline verification of the parsed certificate chain can be done by calling VerifyCertChainByCrl().
    ///         After verification, certificates can be saved to session or persistent storage for subsequent search and use.
    ///         If certificates are not imported, they will be lost after the returned instance is destroyed! Only imported certificates can be found by search and subject to automatic verification!
    ///         The certificates in the result vector are placed from the root CA certificate (zero index) to the final end-entity certificate (last used index of the vector).
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
    /// @param formatId  input format identifier
    /// @return has vlaue if parese sucess false otherwise
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_40616}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02306}
    /// @error: SecurityErrorDomain::kInsufficientCapacity   if the capacity of @c outcome vector is less than actual
    /// number of certificates in the chain
    /// @error: SecurityErrorDomain::kInvalidArgument        if the @c certChain argument cannot be parsed
    /// @error: SecurityErrorDomain::kUnknownIdentifier      if the @c formatId argument has unknown value
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_02738
    /// @trace_id_dd=DD_CRYPTO_05596
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< void > ParseCertChain(ara::core::Vector< Certificate::Uptr >& outcome,
                                                     ReadOnlyMemRegion const& certChain,
                                                     Serializable::FormatId formatId
                                                     = Serializable::kFormatDefault) noexcept = 0;
    /// @brief 2011 standard interface
    /// @brief Parse the serialized representation of a certificate chain and create instances of them.
    ///         Offline verification of the parsed certificate chain can be done by calling VerifyCertChainByCrl().
    ///         After verification, certificates can be imported into session or persistent storage for subsequent search and use. The capacity of the result vector must equal the size of the certChain vector.
    ///         If certificates are not imported, they will be lost after the returned instance is destroyed! Only imported certificates can be found by search and subject to automatic verification!
    ///         The certificates in the result vector are placed from the root CA certificate (zero index) to the final end-entity certificate (last used index of the vector).
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
    /// @param formatId  input format identifier
    /// @return has vlaue if parese sucess false otherwise
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_40617}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02306}
    /// @error: SecurityErrorDomain::kInsufficientCapacity   if capacity of the @c outcome vector is less than number of
    /// elements in the @c certChain
    /// @error: SecurityErrorDomain::kInvalidArgument        if an element of @c certChain argument cannot be parsed
    /// @error: SecurityErrorDomain::kUnknownIdentifier      if the @c formatId argument has unknown value
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_02739
    /// @trace_id_dd=DD_CRYPTO_05597
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< void > ParseCertChain(ara::core::Vector< Certificate::Uptr >& outcome,
                                                     ara::core::Vector< ReadOnlyMemRegion > const& certChain,
                                                     Serializable::FormatId formatId
                                                     = Serializable::kFormatDefault) noexcept = 0;
    /// @brief 2311 standard interface
    /// @brief Parse the serialized representation of a certificate chain and create instances of them.
    ///         Offline verification of the parsed certificate chain can be done by calling VerifyCertChainByCrl().
    ///         After verification, certificates can be saved to session or persistent storage for subsequent search and use.
    ///         If certificates are not imported, they will be lost after the returned instance is destroyed! Only imported certificates can be found by search and subject to automatic verification!
    ///         The certificates in the result vector are placed from the root CA certificate (zero index) to the final end-entity certificate (last used index of the vector).
    /// @brief Parse a serialized representation of the certificate chain and create their instances.
    ///       Off-line validation of the parsed certification chain may be done via call @c VerifyCertChainByCrl().
    ///       After validation the certificates may be saved to the session or persistent storage for following
    ///       search and usage.
    ///       If the certificates are not imported then they will be lost after destroy of the returned instances!
    ///       Only imported certificates may be found by a search and applied for automatic verifications!
    ///       Certificates in the @c outcome vector will be placed from the root CA certificate (zero index) to
    ///       the final end-entity certificate (last used index of the vector).
    /// @param certChain  DER/PEM-encoded certificate chain in form of a single BLOB
    /// @param formatId  input format identifier
    /// @return an output vector for imported certificates
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_40616}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02306}
    /// @error: SecurityErrorDomain::kInsufficientCapacity   if the capacity of @c outcome vector is less than actual number of certificates in the chain
    /// @error: SecurityErrorDomain::kInvalidArgument        if the @c certChain argument cannot be parsed
    /// @error: SecurityErrorDomain::kUnknownIdentifier      if the @c formatId argument has unknown value
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_
    /// @trace_id_dd=DD_CRYPTO_
    /// @needwork = ad
    /// @endcode
    // PRQA S 2024 QAC /// @qac: False positive
    virtual ara::core::Result< ara::core::Vector< Certificate::Uptr > > ParseCertChain(
        ReadOnlyMemRegion const& certChain,
        Serializable::FormatId formatId = Serializable::kFormatDefault) noexcept = 0;
    // PRQA L:QAC
    /// @brief 2311 standard interface
    /// @brief Parse the serialized representation of a certificate chain and create instances of them.
    ///         Offline verification of the parsed certificate chain can be done by calling VerifyCertChainByCrl().
    ///         After verification, certificates can be imported into session or persistent storage for subsequent search and use. The capacity of the result vector must equal the size of the certChain vector.
    ///         If certificates are not imported, they will be lost after the returned instance is destroyed! Only imported certificates can be found by search and subject to automatic verification!
    ///         The certificates in the result vector are placed from the root CA certificate (zero index) to the final end-entity certificate (last used index of the vector).
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
    /// @param formatId  input format identifier
    /// @return output vector of imported certificates
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_40617}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02306}
    /// @error: SecurityErrorDomain::kInsufficientCapacity   if capacity of the @c outcome vector is less than number of
    /// elements in the @c certChain
    /// @error: SecurityErrorDomain::kInvalidArgument        if an element of @c certChain argument cannot be parsed
    /// @error: SecurityErrorDomain::kUnknownIdentifier      if the @c formatId argument has unknown value
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_
    /// @trace_id_dd=DD_CRYPTO_
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< ara::core::Vector< Certificate::Uptr > > ParseCertChain(
        ara::core::Vector< ReadOnlyMemRegion > const& certChain,
        Serializable::FormatId formatId = Serializable::kFormatDefault) noexcept = 0;
    /// @brief Parse the serialized representation of a certificate and create an instance of it. Offline verification of the parsed certificate can be done by calling VerifyCertByCrl().
    ///         After verification, the certificate can be imported into session or persistent storage for subsequent search and use.
    ///         If the parsed certificate is not imported, it will be lost after the returned instance is destroyed! Only imported certificates can be found by search and subject to automatic verification!
    /// @brief Parse a serialized representation of the certificate and create its instance.
    ///       Off-line validation of the parsed certificate may be done via call @c VerifyCertByCrl().
    ///       After validation the certificate may be imported to the session or persistent storage for following
    ///       search and usage.
    ///       If the parsed certificate is not imported then it will be lost after destroy of the returned instance!
    ///       Only imported certificate may be found by a search and applied for automatic verifications!
    /// @param cert  DER/PEM-encoded certificate
    /// @param formatId  input format identifier
    /// @returns unique smart pointer to created certificate
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_40614}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02306}
    /// @error: SecurityErrorDomain::kInvalidArgument    if the @c cert argument cannot be parsed
    /// @error: SecurityErrorDomain::kUnknownIdentifier  if the @c formatId argument has unknown value
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02740
    /// @trace_id_dd=DD_CRYPTO_05598
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< Certificate::Uptr > ParseCert(ReadOnlyMemRegion const& cert,
                                                             Serializable::FormatId formatId
                                                             = Serializable::kFormatDefault) noexcept = 0;
#if AP_VERSION_PUHUA == 20
    /// @brief Parse the serialized OCSP response and create the corresponding interface instance. This method can be used to implement "OCSP stapling".
    /// @brief Parse serialized OCSP response and create correspondent interface instance.
    ///       This method may be used for implementation of the "OCSP stapling".
    /// @param response  a serialized OCSP response
    /// @returns unique smart pointer to the created OCSP response instance
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_40628}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02306}
    /// @error: SecurityErrorDomain::kUnexpectedValue    if the provided BLOB @c response doesn't keep an OCSP response
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02741
    /// @trace_id_dd=DD_CRYPTO_05599
    /// @needwork = ad
    /// @endcode
    virtual OcspResponse::Uptrc ParseOcspResponse(ReadOnlyMemRegion const& response) const noexcept = 0;
    /// @brief 2011 standard interface
    /// @brief Delete the specified certificate (volatile or persistent) from the store and destroy it.
    /// @brief Remove specified certificate from the storage (volatile or persistent) and destroy it.
    /// @param cert  a unique smart pointer to a certificate that should be removed
    /// @returns @c true if the certificate was found and removed from the storage, @c false if it was not found
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_40622}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02306}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_02742
    /// @trace_id_dd=DD_CRYPTO_05600
    /// @needwork = ad
    /// @endcode
    virtual bool Remove(Certificate::Uptrc&& cert) noexcept = 0;
#elif AP_VERSION_PUHUA == 30
    /// @brief Parse the serialized OCSP response and create the corresponding interface instance. This method can be used to implement "OCSP stapling".
    /// @brief Parse serialized OCSP response and create correspondent interface instance.
    ///       This method may be used for implementation of the "OCSP stapling".
    /// @param response  a serialized OCSP response
    /// @returns unique smart pointer to the created OCSP response instance
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_40628}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02306}
    /// @error: SecurityErrorDomain::kUnexpectedValue    if the provided BLOB @c response doesn't keep an OCSP response
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02741
    /// @trace_id_dd=DD_CRYPTO_05599
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< OcspResponse::Uptrc > ParseOcspResponse(
        ReadOnlyMemRegion const& response) const noexcept = 0;
    /// @brief Delete the specified certificate (volatile or persistent) from the store and destroy it.
    /// @brief Remove specified certificate from the storage (volatile or persistent) and destroy it.
    /// @param cert  a unique smart pointer to a certificate that should be removed
    /// @returns @c true if the certificate was found and removed from the storage, @c false if it was not found
    virtual bool Remove(Certificate::Uptrc cert) noexcept = 0;
#endif

    /// @brief Set the specified CA certificate as a "trusted root certificate".
    ///         Only certificates saved in volatile or persistent storage can be marked as "trust root"!
    ///         Only CA certificates are trust root certificates! Multiple certificates on one ECU can be marked as "trust root".
    ///         Only applications with "trust administrator" permission have the right to call this method!
    /// @brief Set specified CA certificate as a "root of trust".
    ///       Only a certificate saved to the volatile or persistent storage may be marked as the "root of trust"!
    ///       Only CA certificate can be a "root of trust"!
    ///       Multiple certificates on an ECU may be marked as the "root of trust".
    ///       Only an application with permissions "Trust Master" has the right to call this method!
    /// @param caCert  a valid CA certificate that should be trusted
    /// @return has vlaue if SetAsRootOfTrust sucess false otherwise
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_40625}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02306}
    /// @error: SecurityErrorDomain::kInvalidArgument        if the provided certificate is invalid
    /// @error: SecurityErrorDomain::kIncompatibleObject     if provided certificate doesn't belong to a CA
    /// @error: SecurityErrorDomain::kAccessViolation        if the method called by an application without the "Trust
    /// Master" permission
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02743
    /// @trace_id_dd=DD_CRYPTO_05601
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< void > SetAsRootOfTrust(Certificate const& caCert) noexcept = 0;
    /// @brief Set the "pending" status associated with the CSR, indicating the CSR has been sent to the CA.
    ///         If the CSR is already marked as "pending", this method does nothing. Only applications with "CA connector" permission have the right to call this method!
    /// @brief Set the "pending" status associated to the CSR that means that the CSR already sent to CA.
    ///       This method do nothing if the CSR already marked as "pending".
    ///       Only an application with permissions "CA Connector" has the right to call this method!
    /// @param request  certificate signing request that should be marked as "pending"
    /// @return has vlaue if SetPendingStatus sucess false otherwise
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_40624}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02306}
    /// @error: SecurityErrorDomain::kAccessViolation    if the method called by an application without the "CA
    /// Connector" permission
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02744
    /// @trace_id_dd=DD_CRYPTO_05602
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< void > SetPendingStatus(CertSignRequest const& request) noexcept = 0;
    /// @brief Check if the certificate signing request has a pending status set (non-standard interface)
    /// @param request Certificate request
    /// @return true if has already pendding false otherwise
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02745
    /// @trace_id_dd=DD_CRYPTO_05603
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< bool > IsCsrPenddingStatus(CertSignRequest const& request) noexcept = 0;
    /// @brief 2011 standard interface
    /// @brief Verify the status of the provided certificate using only locally stored CA certificates and CRLs. This method updates the Certificate::Status associated with the certificate.
    /// @brief Verify status of the provided certificate by locally stored CA certificates and CRLs only.
    ///       This method updates the @c Certificate::Status associated with the certificate.
    /// @param cert  target certificate for verification
    /// @param myRoot  root certificate to be used for verification - if this is nullptr, use machine root
    /// certificates
    /// @returns verification status of the provided certificate
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_40618}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02306}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_02746
    /// @trace_id_dd=DD_CRYPTO_05604
    /// @needwork = ad
    /// @endcode
    virtual Certificate::Status VerifyCert(Certificate& cert, Certificate::Uptr myRoot = {nullptr}) noexcept = 0;
    /// @brief 2311 standard interface
    /// @brief Verify the status of the provided certificate using only locally stored CA certificates and CRLs. This method updates the Certificate::Status associated with the certificate.
    /// @brief Verify status of the provided certificate by locally stored CA certificates and CRLs only.
    ///       This method updates the @c Certificate::Status associated with the certificate.
    /// @param cert  target certificate for verification
    /// @param myRoot  root certificate to be used for verification - if this is nullptr, use machine root
    /// certificates
    /// @returns verification status of the provided certificate
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_40618}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02306}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_
    /// @trace_id_dd=DD_CRYPTO_
    /// @needwork = ad
    /// @endcode
    virtual Certificate::Status VerifyCert(Certificate const& cert, Certificate const& myRoot) noexcept = 0;
    /// @brief 2011 standard interface
    /// @brief Verify the status of the provided certificate chain using only locally stored CA certificates and CRLs.
    ///         The verification status of the certificate chain is certificate:: status::kValid, only valid if all certificates in the chain are in this state!
    ///         The certificates in the chain (provided by the container vector) must be placed from the root CA certificate (zero index) to the target end-entity certificate (last used vector index). Verification is performed in the same order.
    ///         If chain verification fails, the status of the first failed certificate is returned. This method updates the Certificate::Status associated with the certificates in the certificate chain.
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
    /// @trace_id_sws={SWS_CRYPT_40619}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02306}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_02747
    /// @trace_id_dd=DD_CRYPTO_05605
    /// @needwork = ad
    /// @endcode
    virtual Certificate::Status VerifyCertChain(ara::core::Span< Certificate::Uptr const > const& chain,
                                                Certificate::Uptr myRoot = {nullptr}) const noexcept = 0;
    /// @brief 2311 standard interface
    /// @brief Verify the status of the provided certificate chain using only locally stored CA certificates and CRLs.
    ///         The verification status of the certificate chain is certificate:: status::kValid, only valid if all certificates in the chain are in this state!
    ///         The certificates in the chain (provided by the container vector) must be placed from the root CA certificate (zero index) to the target end-entity certificate (last used vector index). Verification is performed in the same order.
    ///         If chain verification fails, the status of the first failed certificate is returned. This method updates the Certificate::Status associated with the certificates in the certificate chain.
    /// @brief Verify status of the provided certification chain by locally stored CA certificates and CRLs only.
    ///       Verification status of the certificate chain is @c Certificate::Status::kValid only if all certificates in
    ///       the chain have such status! Certificates in the chain (presented by container vector) must be placed from
    ///       the root CA certificate (zero index) to the target end-entity certificate (last used index of the vector).
    ///       Verification is executed in same order. If the chain verification is failed then status of the first
    ///       failed certificate is returned. This method updates the @c Certificate::Status associated with the
    ///       certificates in the chain.
    /// @param chain  target certificate chain for verification
    /// certificates
    /// @returns verification status of the provided certificate chain
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_40619}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02306}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_
    /// @trace_id_dd=DD_CRYPTO_
    /// @needwork = ad
    /// @endcode
    virtual Certificate::Status VerifyCertChain(
        ara::core::Span< std::reference_wrapper< Certificate const > > chain) const noexcept = 0;
    /// @brief Default constructor
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02748
    /// @trace_id_dd=DD_CRYPTO_05606
    /// @needwork = ad
    /// @endcode
    X509Provider() noexcept = default;
    /// @brief Default virtual destructor
    /// @brief Destructor.
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_40604}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02306}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02749
    /// @trace_id_dd=DD_CRYPTO_05607
    /// @needwork = ad
    /// @endcode
    virtual ~X509Provider() noexcept = default;
    /// @brief Default copy assignment operator
    /// @param other Another instance of this class
    /// @returns *this
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02750
    /// @trace_id_dd=DD_CRYPTO_05608
    /// @needwork = ad
    /// @endcode
    X509Provider& operator=(X509Provider const& other) noexcept = delete;
    /// @brief Default move assignment operator
    /// @param other Another instance of this class
    /// @returns *this
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02751
    /// @trace_id_dd=DD_CRYPTO_05609
    /// @needwork = ad
    /// @endcode
    X509Provider& operator=(X509Provider&& other) noexcept = delete;
    /// @brief Default copy constructor
    /// @param other Another instance of this class
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02752
    /// @trace_id_dd=DD_CRYPTO_05610
    /// @needwork = ad
    /// @endcode
    X509Provider(X509Provider const& other) = delete;
    /// @brief Default move constructor
    /// @param other Another instance of this class
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02753
    /// @trace_id_dd=DD_CRYPTO_05611
    /// @needwork = ad
    /// @endcode
    X509Provider(X509Provider&& other) = delete;

public:
    /// @brief External interface for revoking a certificate
    /// @param revokedISpecify
    /// @return true if Revoke Cert sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02754
    /// @trace_id_dd=DD_CRYPTO_05612
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< bool > RevokeCertificate(ara::core::InstanceSpecifier& revokedISpecify) noexcept = 0;
    /// @brief External interface for generating a CRL
    /// @param iSpecify Target certificate instance specifier
    /// @return CRL data
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02755
    /// @trace_id_dd=DD_CRYPTO_05613
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< ara::core::Vector< ara::core::Byte > > GenerateCertRevokeList(
        ara::core::InstanceSpecifier& iSpecify) const noexcept = 0;
};

}  // namespace x509
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_X509_X509_PROVIDER_H_
