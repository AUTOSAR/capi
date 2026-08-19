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
/// @file       certificate.h
/// @brief      AutoSar-Crypto Certificate Management Module
/// @details    X.509 certificate interface.
/// @date       2021-12-21
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// </td> <td>2021-12-21 <td>1.0.0 <td>hanjingjing <td>Created initial version
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Certificate Components/X.509 Certificate
/// @interface_level=software
/// @trace_id_sr=SR_CRYPTO_03001
/// @unit_name=Certificate
/// @unit_description=Certificate Base Class
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_X509_CERTIFICATE_H_
#define ARA_CRYPTO_X509_CERTIFICATE_H_

#include "ara/crypto/cryp/crypto_context.h"
#include "ara/crypto/x509/basic_cert_info.h"
#include "ara/crypto/x509/x509_extensions.h"

namespace ara {
namespace crypto {
namespace x509 {
// 2022-06-20 hanjingjing: The design of Optional in VerifyMe(ara::core::Optional<Certificate const > caCert) definition leads to not using
// the PCertificate approach
//********************************/
//- @interface Certificate
/// @brief X.509 certificate interface.
/// @trace_id_sws={SWS_CRYPT_40200}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02306}
/// @brief X.509 Certificate interface.
//********************************/
/// @code{.isoft}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02671
/// @trace_id_dd=DD_CRYPTO_05510
/// @needwork = ad
/// @endcode
class Certificate : public BasicCertInfo
{
public:
    /// @brief Unique smart pointer for the constant interface.
    /// @brief Unique smart pointer of the interface.
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_40202}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02306}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_03236
    /// @trace_id_dd=DD_CRYPTO_06448
    /// @needwork = ad
    /// @endcode
    using Uptrc = std::unique_ptr< Certificate const >;
    /// @brief Unique smart pointer for the interface.
    /// @brief Unique smart pointer of the interface.
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_40201}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02306}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_03237
    /// @trace_id_dd=DD_CRYPTO_06449
    /// @needwork = ad
    /// @endcode
    using Uptr = std::unique_ptr< Certificate >;

public:
    /// @brief Certificate verification status. 2311 version
    /// @brief Certificate verification status.
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_40203}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02306}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02672
    /// @trace_id_dd=DD_CRYPTO_05511
    /// @needwork = ad
    /// @endcode
    enum class Status : std::uint32_t
    {
        /// @brief Certificate is valid
        /// @brief The certificate is valid
        /// @brief 2311:The signature of the provided certificate is successfully verified and
        /// @brief the signing certificate is a root of trust or is chained to a root of trust on this adaptiv emachine
        kValid = 0,
        /// @brief Certificate is invalid
        /// @brief  The certificate is invalid e.g. the provided certificate can be invalid
        /// @brief if the signature of the provided certificate can not be verified by the root certificate.
        kInvalid = 1,
        /// @brief Status of the certificate is unknown
        /// @brief Status of the certificate is unknown yet
        kUnknown = 2,
        /// @brief Verification result is not available
        /// @brief A verification result is not available because verification could not be executed
        /// @brief e.g. because the provided root is not the signing certificate or a root certificate could not be found.
        kNotAvailable = 3,
        /// @brief Certificate has the correct signature, but has expired (its validity period has ended)
        /// @brief The certificate has correct signature, but it is already expired (its validity period has ended)
        kExpired = 4,
        /// @brief Certificate has the correct signature, but its validity period has not yet started
        /// @brief The certificate has correct signature, but its validity period is not started yet
        kFuture = 5,
        /// @brief Certificate is revoked
        /// @brief The certificate has been revoked i.e. the provided certificate is on CRLlist
        kRevoked = 6,
        /// @brief Certificate has the correct signature, but the ECU has no trust root for this certificate
        /// @brief 2311 actually removed kNoTrust and replaced it with kNotAvailable, but kNoTrust is still retained
        /// @brief The certificate has correct signature, but the ECU has no a root of trust for this certificate
        kNoTrust = 7,
    };

public:  // Certificate interface
    /// @brief Get the X.509 version of this certificate object.
    /// @brief Get the X.509 version of this certificate object.
    /// @returns X.509 version
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_40211}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02306}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02673
    /// @trace_id_dd=DD_CRYPTO_05512
    /// @needwork = ad
    /// @endcode
    virtual std::uint32_t X509Version() const noexcept;
    /// @brief Check whether this certificate belongs to a root CA.
    /// @brief Check whether this certificate belongs to a root CA.
    /// @returns @c true if the TrustMaster has set this certificate as root
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_40212}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02306}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02674
    /// @trace_id_dd=DD_CRYPTO_05513
    /// @needwork = ad
    /// @endcode
    virtual bool IsRoot() const noexcept;
    /// @brief Get the issuer certificate DN.
    /// @brief Get the issuer certificate DN.
    /// @returns Issuer DN of this certificate
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_40213}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02306}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02675
    /// @trace_id_dd=DD_CRYPTO_05514
    /// @needwork = ad
    /// @endcode
    virtual X509DN const& IssuerDn() const noexcept;
    /// @brief Get the "Not Before" date of the certificate.
    /// @brief Get the "Not Before" of the certificate.
    /// @returns "Not Before" of the certificate
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_40214}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02306}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02676
    /// @trace_id_dd=DD_CRYPTO_05515
    /// @needwork = ad
    /// @endcode
    virtual time_t StartTime() const noexcept;
    /// @brief Get the "Not After" date of the certificate.
    /// @brief Get the "Not After" of the certificate.
    /// @returns "Not After" of the certificate
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_40215}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02306}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02677
    /// @trace_id_dd=DD_CRYPTO_05516
    /// @needwork = ad
    /// @endcode
    virtual time_t EndTime() const noexcept;
    /// @brief Get the serial number of this certificate. If (sn.empty() == true), this method only returns the required output buffer size.
    /// @brief Get the serial number of this certificate.
    ///         If (sn.empty() == true) then this method only returns required size of the output buffer.
    /// @returns size of the certificate serial number in bytes
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kInsufficientCapacity   if (sn.empty() == false), but its size is not enough for
    /// storing the output value
    /// @trace_id_sws={SWS_CRYPT_40216}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02306}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02678
    /// @trace_id_dd=DD_CRYPTO_05517
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< ara::core::Vector< ara::core::Byte > > SerialNumber() const noexcept;
    /// @brief Get the DER encoded AuthorityKeyIdentifier of this certificate. If (id.empty() ==
    /// true), this method only returns the required output buffer size.
    /// @brief Get the DER encoded AuthorityKeyIdentifier of this certificate.
    ///       If (id.empty() == true) then this method only returns required size of the output buffer.
    /// @returns size of the DER encoded AuthorityKeyIdentifier in bytes
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kInsufficientCapacity   if (id.empty() == false), but its size is not enough for
    /// storing the output value
    /// @trace_id_sws={SWS_CRYPT_40217}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02306}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02679
    /// @trace_id_dd=DD_CRYPTO_05518
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< ara::core::Vector< ara::core::Byte > > AuthorityKeyId() const noexcept;
    /// @brief Get the DER encoded SubjectKeyIdentifier of this certificate. If (id.empty() == true), this method only returns the required output buffer size.
    /// @brief Get the DER encoded SubjectKeyIdentifier of this certificate.
    ///       If (id.empty() == true) then this method only returns required size of the output buffer.
    /// @returns size of the DER encoded SubjectKeyIdentifier in bytes
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kInsufficientCapacity   if (id.empty() == false), but its size is not enough for
    /// storing the output value
    /// @trace_id_sws={SWS_CRYPT_40218}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02306}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02680
    /// @trace_id_dd=DD_CRYPTO_05519
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< ara::core::Vector< ara::core::Byte > > SubjectKeyId() const noexcept;
    /// @brief Verify the certificate signature. The call with (caCert == nullptr) parameter is only applicable for root CA certificates.
    /// @brief Verify signature of the certificate.
    ///       Call with (caCert == nullptr) is applicable only if this is a certificate of a root CA.
    /// @param caCert  the optional pointer to a Certification Authority certificate used for signature of the
    /// current one Optional pointer pointing to the certificate authority certificate used to sign the current certificate
    /// @returns @c true if this certificate was verified successfully and @c false otherwise
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_40219}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02306}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02681
    /// @trace_id_dd=DD_CRYPTO_05520
    /// @needwork = ad
    /// @endcode
    virtual bool VerifyMe(ara::core::Optional< Certificate const > const& caCert) const noexcept;
    /// @brief Calculate the fingerprint from the entire certificate. The generated fingerprint value is saved to the output buffer starting from the leading bytes of the hash value.
    ///         If the capacity of the output buffer is less than the digest size, the digest will be truncated, keeping only the leading bytes.
    ///         If the capacity of the output buffer is greater than the digest size, only the leading bytes of the buffer will be updated.
    /// @brief Calculate a fingerprint from the whole certificate.
    ///       The produced fingerprint value saved to the output buffer starting from leading bytes of the hash value.
    ///       If the capacity of the output buffer is less than the digest size then the digest will be truncated and
    ///       only leading bytes will be saved. If the capacity of the output buffer is higher than the digest size then
    ///       only leading bytes of the buffer will be updated.
    /// @param fingerprint  output buffer for the fingerprint storage
    /// @param hashCtx  an initialized hash function context
    /// @returns number of bytes actually saved to the output buffer
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kIncompleteArgState  if the @c hashCtx context is not initialized
    /// @trace_id_sws={SWS_CRYPT_40220}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02306}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02682
    /// @trace_id_dd=DD_CRYPTO_05521
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< std::size_t > GetFingerprint(ReadWriteMemRegion const& fingerprint,
                                                            cryp::HashFunctionCtx& hashCtx) const noexcept;
    /// @brief Return the last verification status of the certificate.
    /// @brief Return last verification status of the certificate.
    /// @returns the certificate verification status
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_40221}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02306}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02683
    /// @trace_id_dd=DD_CRYPTO_05522
    /// @needwork = ad
    /// @endcode
    virtual Status GetStatus() const noexcept;

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
    /// @trace_id_ad=AD_CRYPTO_02684
    /// @trace_id_dd=DD_CRYPTO_05523
    /// @needwork = ad
    /// @endcode
    ara::core::Result< ara::core::Vector< ara::core::Byte > > ExportPublicly(FormatId formatId
                                                                             = kFormatDefault) const noexcept override;
    /// @brief Use base class template functions
    using Serializable::ExportPublicly;

public:  // BasicCertInfo interface
    /// @brief Load the subject public key information object into the realm of the specified crypto provider.
    ////        If (cryptoProvider == nullptr) then the X509PublicKeyInfo object will be loaded into the realm of the Stack-default Crypto Provider.
    /// In the field of Provider.
    /// @brief Load the subject public key information object to realm of specified crypto provider.
    ///         If (cryptoProvider == nullptr) then @c X509PublicKeyInfo object will be loaded in realm of the
    ///         Stack-default Crypto Provider.
    /// @param cryptoProvider unique pointer of a target Crypto Provider, where the public key will be used
    /// @returns constant reference of the subject public key interface
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_40111}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02306}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02685
    /// @trace_id_dd=DD_CRYPTO_05524
    /// @needwork = ad
    /// @endcode
    X509PublicKeyInfo const& SubjectPubKey(cryp::CryptoProvider* cryptoProvider = nullptr) const noexcept override;
    /// @brief Get the subject DN.
    /// @brief Get the subject DN.
    /// @returns subject DN
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_40112}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02306}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02686
    /// @trace_id_dd=DD_CRYPTO_05525
    /// @needwork = ad
    /// @endcode
    X509DN const& SubjectDn() const noexcept override;
    /// @brief Check if the CA attribute of the X509v3 Basic Constraints is true (i.e., pathlen=0).
    /// @brief Check whether the CA attribute of X509v3 Basic Constraints is true (i.e. pathlen=0).
    /// @returns @c true if it is a CA request and @c false otherwise
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_40113}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02306}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02687
    /// @trace_id_dd=DD_CRYPTO_05526
    /// @needwork = ad
    /// @endcode
    bool IsCa() const noexcept override;
    /// @brief Get the path length constraint defined in the basic constraints extension.
    /// @brief Get the constraint on the path length defined in the Basic Constraints extension.
    /// @returns certification path length limit
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_40114}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02306}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02688
    /// @trace_id_dd=DD_CRYPTO_05527
    /// @needwork = ad
    /// @endcode
    std::uint32_t GetPathLimit() const noexcept override;
    /// @brief Get the key constraints (X509v3 Key Usage) of the key associated with this PKCS#10 object.
    /// @brief Get the key constraints for the key associated with this PKCS#10 object.
    /// @returns key constraints
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_40115}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02306}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02689
    /// @trace_id_dd=DD_CRYPTO_05528
    /// @needwork = ad
    /// @endcode
    KeyConstraints GetConstraints() const noexcept override;

private:
    /// @brief X509 Certificate pointer // 2022-05-19 Could not find the definition header file for X509
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05529
    /// @needwork = dda
    /// @endcode
    X509* pX509_;
    /// @brief Whether it is a root of trust certificate
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05530
    /// @needwork = dda
    /// @endcode
    mutable bool bTrustRoot_;
    /// @brief Whether it is a trusted certificate
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05531
    /// @needwork = dda
    /// @endcode
    mutable bool bTrustCert_;
    /// @brief SN information
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05532
    /// @needwork = dda
    /// @endcode
    mutable X509DN x509DnSubject_;
    /// @brief DN information
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05533
    /// @needwork = dda
    /// @endcode
    mutable X509DN x509DnIssuer_;
    /// @brief Public key information
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05534
    /// @needwork = dda
    /// @endcode
    mutable X509PublicKeyInfo x509PublicKey_;
    /// @brief Status of the most recent certificate operation
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05535
    /// @needwork = dda
    /// @endcode
    mutable Status statusLast_;
    /// @brief Certificate file name
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05536
    /// @needwork = dda
    /// @endcode
    ara::core::String stCertFileName_;
    /// @brief Certificate slot name
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05537
    /// @needwork = dda
    /// @endcode
    ara::core::String stCertSlotName_;

public:
    /// @brief Constructor with parameters
    /// @param x509Provider Certificate provider
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02690
    /// @trace_id_dd=DD_CRYPTO_05538
    /// @needwork = ad
    /// @endcode
    explicit Certificate(X509Provider& x509Provider) noexcept;
    /// @brief Copy constructor
    /// @param other Another instance of this class
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_08815
    /// @needwork = dda
    /// @endcode
    Certificate(Certificate const& other) noexcept;
    /// @brief Default move constructor
    /// @param other Another instance of this class
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_08816
    /// @needwork = dda
    /// @endcode
    Certificate(Certificate&& other) noexcept = delete;
    /// @brief Default move assignment operator
    /// @param other Another instance of this class
    /// @returns *this
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_08817
    /// @needwork = dda
    /// @endcode
    Certificate& operator=(Certificate&& other) noexcept = delete;
    /// @brief Default copy assignment operator
    /// @param other Another instance of this class
    /// @returns *this
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_08818
    /// @needwork = dda
    /// @endcode
    Certificate& operator=(Certificate const& other) noexcept = delete;
    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_08819
    /// @needwork = dda
    /// @endcode
    ~Certificate() noexcept override;
    /// @brief Associate an x509 structure
    /// @name   AttachX509
    /// @param pX509 Standard format certificate
    /// @returns ture if attach sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02696
    /// @trace_id_dd=DD_CRYPTO_05544
    /// @needwork = ad
    /// @endcode
    bool AttachX509(X509* const pX509) noexcept;
    /// @brief Return the x509 structure
    /// @return x509 pointer object
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02697
    /// @trace_id_dd=DD_CRYPTO_05545
    /// @needwork = ad
    /// @endcode
    inline X509* GetX509() const noexcept { return pX509_; }
    /// @brief Set the certificate status
    /// @param nStatus Certificate status
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02698
    /// @trace_id_dd=DD_CRYPTO_05546
    /// @needwork = ad
    /// @endcode
    inline void SetCertStatus(Status const nStatus) const noexcept { statusLast_ = nStatus; }
    /// @brief Check if there is root of trust authority
    /// @return ture if turst false otherwise
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02699
    /// @trace_id_dd=DD_CRYPTO_05547
    /// @needwork = ad
    /// @endcode
    inline bool IsRootOfTrust() const noexcept { return bTrustRoot_; }
    /// @brief Set as root of trust: return value indicates success
    /// @name   SetAsRootOfTrust
    /// @param bTrustRoot Whether it has root of trust authority
    /// @returns true if set sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02700
    /// @trace_id_dd=DD_CRYPTO_05548
    /// @needwork = ad
    /// @endcode
    bool SetAsRootOfTrust(bool const bTrustRoot) const noexcept;
    /// @brief Check if it is a root of trust
    /// @return ture if turst false otherwise
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02701
    /// @trace_id_dd=DD_CRYPTO_05549
    /// @needwork = ad
    /// @endcode
    inline bool IsTrust() const noexcept { return bTrustCert_; }
    /// @brief Set as root of trust: return value indicates success
    /// @param bTrust Whether it is a root of trust
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02702
    /// @trace_id_dd=DD_CRYPTO_05550
    /// @needwork = ad
    /// @endcode
    inline void SetTrust(bool const bTrust) const noexcept { bTrustCert_ = bTrust; }
    /// @brief Whether it is volatile storage
    /// @name   IsVolatileStorage
    /// @returns ture is Volatile false otherwise
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02703
    /// @trace_id_dd=DD_CRYPTO_05551
    /// @needwork = ad
    /// @endcode
    bool IsVolatileStorage() const noexcept;
    /// @brief Check if the time is within the certificate's validity period
    /// @name   CheckTimeValid
    /// @param tmCheck Time to check within certificate validity
    /// @returns ture if time valid flase otherwise
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02704
    /// @trace_id_dd=DD_CRYPTO_05552
    /// @needwork = ad
    /// @endcode
    bool CheckTimeValid(time_t const tmCheck) const noexcept;
    /// @brief Load a certificate from a file
    /// @name   LoadCertFile
    /// @param stFileName Certificate file
    /// @param nFormatID Certificate encoding format
    /// @returns ture load cert sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02705
    /// @trace_id_dd=DD_CRYPTO_05553
    /// @needwork = ad
    /// @endcode
    bool LoadCertFile(ara::core::StringView const& stFileName, FormatId const nFormatID) noexcept;
    /// @brief Load a certificate from memory
    /// @name   LoadCertFile
    /// @param memCert Certificate memory data
    /// @param nFormatID Certificate encoding format
    /// @returns ture load cert sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02706
    /// @trace_id_dd=DD_CRYPTO_05554
    /// @needwork = ad
    /// @endcode
    bool LoadCertFile(ReadOnlyMemRegion const& memCert, FormatId const nFormatID) noexcept;
    /// @brief Get the file name for persistent storage, empty for volatile storage
    /// @name   GetStorageFile
    /// @returns Persistent file name
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02707
    /// @trace_id_dd=DD_CRYPTO_05555
    /// @needwork = ad
    /// @endcode
    ara::core::StringView GetStorageFile() const noexcept;
    /// @brief Set the file name for persistent storage
    /// @name   SetCertSlotName
    /// @param stSlotName Key slot name
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02708
    /// @trace_id_dd=DD_CRYPTO_05556
    /// @needwork = ad
    /// @endcode
    void SetCertSlotName(ara::core::StringView const& stSlotName) noexcept;
    /// @brief Get the name of the certificate: the name inside the Json configuration file
    /// @name   GetCertSlotName
    /// @returns Certificate slot name
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02709
    /// @trace_id_dd=DD_CRYPTO_05557
    /// @needwork = ad
    /// @endcode
    ara::core::StringView GetCertSlotName() const noexcept;
    /// @brief Return a clone of itself
    /// @name   CloneSelf
    /// @returns Certificate instance object
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02710
    /// @trace_id_dd=DD_CRYPTO_05558
    /// @needwork = ad
    /// @endcode
    Certificate::Uptr CloneSelf() const noexcept;

public:
    /// @brief Get the extension attribute "Key Usage"
    /// @name   GetKeyUsage
    /// @returns Key usage
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02711
    /// @trace_id_dd=DD_CRYPTO_05559
    /// @needwork = ad
    /// @endcode
    uint32_t GetKeyUsage() const noexcept;
    /// @brief Get the extension interface
    /// @name   GetExtensions
    /// @returns Certificate extension instance
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02712
    /// @trace_id_dd=DD_CRYPTO_05560
    /// @needwork = ad
    /// @endcode
    X509Extensions::Uptr GetExtensions() const noexcept;
    /// @brief The signature algorithm of the certificate. Returns the custom AlgId defined by the Crypto module.
    /// @name   GetSignatureAlgId
    /// @returns Custom algorithm ID
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02713
    /// @trace_id_dd=DD_CRYPTO_05561
    /// @needwork = ad
    /// @endcode
    cryp::CryptoContext::AlgId GetSignatureAlgId() const noexcept;

protected:
    /// @brief Delete the x509 structure
    /// @name   _DeleteX509
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05562
    /// @needwork = dda
    /// @endcode
    void _DeleteX509() noexcept;
    /// @brief Load the certificate
    /// @name   LoadCertFile
    /// @param stFileName Certificate file
    /// @returns Certificate instance object
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05563
    /// @needwork = dda
    /// @endcode
    static X509* LoadCertFile(ara::core::StringView const& stFileName) noexcept;
    /// @brief Load the certificate (source data format)
    /// @name   LoadCertFile_Raw
    /// @param stFileName Certificate file
    /// @returns Certificate instance object
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05564
    /// @needwork = dda
    /// @endcode
    static X509* LoadCertFile_Raw(ara::core::StringView const& stFileName) noexcept;
    /// @brief Load the certificate (DER format)
    /// @name   LoadCertFile_Der
    /// @param stFileName Certificate file
    /// @returns Certificate instance object
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05565
    /// @needwork = dda
    /// @endcode
    static X509* LoadCertFile_Der(ara::core::StringView const& stFileName) noexcept;
    /// @brief Load the certificate (PEM format)
    /// @name   LoadCertFile_Pem
    /// @param stFileName Certificate file
    /// @returns Certificate instance object
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05566
    /// @needwork = dda
    /// @endcode
    static X509* LoadCertFile_Pem(ara::core::StringView const& stFileName) noexcept;
    /// @brief Export the certificate request in Der format
    /// @param vecData Return result
    /// @param nType Certificate encoding format
    /// @return true if export sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05567
    /// @needwork = dda
    /// @endcode
    bool _ExportCert_Der(ara::core::Vector< ara::core::Byte >& vecData, int32_t const nType = 0) const noexcept;
    /// @brief Export the certificate request in Pem format
    /// @name   _ExportCert_Pem
    /// @param vecData Return result
    /// @returns true if export sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05568
    /// @needwork = dda
    /// @endcode
    bool _ExportCert_Pem(ara::core::Vector< ara::core::Byte >& vecData) const noexcept;
    /// @brief Get the public key data: RSA format
    /// @name   GetPublicKeyData_Rsa
    /// @param pEvpKey Key resource pointer
    /// @returns Public key data information
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05569
    /// @needwork = dda
    /// @endcode
    static ara::core::Vector< ara::core::Byte > GetPublicKeyData_Rsa(EVP_PKEY* const pEvpKey) noexcept;
    /// @brief Process the obtained Extension data by NID
    /// @name   _DealExtDataByID
    /// @param nID Extension item ID
    /// @param pfunc Callback function
    /// @return Extension data information
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05570
    /// @needwork = dda
    /// @endcode
    bool _DealExtDataByID(int32_t const nID, std::function< bool(void const* pVoidData) > const& pfunc) const noexcept;

public:
};
//********************************/
}  // namespace x509
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_X509_CERTIFICATE_H_
