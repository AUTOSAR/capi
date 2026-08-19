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
/// @file       isoft_cert_sign_request.h
/// @brief      AutoSar-Crypto Certificate Management Module
/// @details    Certificate Signing Request (CSR) object interface.
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
/// @module_path=/CRYPTO/Certificate Component/Certificate Request
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_03005
/// @unit_name=PCertSignRequest
/// @unit_description=Certificate Signing Request (CSR) Object Interface
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_X509_PUHUA_CERT_SIGN_REQUEST_H_
#define ARA_CRYPTO_X509_PUHUA_CERT_SIGN_REQUEST_H_

#include <openssl/x509.h>

#include "ara/crypto/x509/cert_sign_request.h"

namespace ara {
namespace crypto {
namespace x509 {
namespace isoft_def {
//********************************/ //2022-07-06 hanjingjing understands this as handling certificate request applications
//- @interface PCertSignRequest
/// @brief Certificate Signing Request (CSR) object interface. This interface is specifically used for complete parsing of request content.
/// @brief Certificate Signing Request (CSR) object interface.
///         This interface is dedicated for complete parsing of the request content.
/// @code{.isoft}
/// @trace_id_sws={SWS_CRYPT_40300}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02306}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01434
/// @trace_id_dd=DD_CRYPTO_03761
/// @needwork = ad
/// @endcode
// PRQA S 2659 QAC  /// @qac: Suspected inability to modify [2659]:
// Base class '::ara::crypto::x509::CertSignRequest' has non-deleted public copy/move special member functions.
class PCertSignRequest : public CertSignRequest
// PRQA L:QAC
{
public:
    /// @brief Unique smart pointer for the interface.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01434
    /// @trace_id_dd=DD_CRYPTO_06346
    /// @needwork = dd
    /// @endcode
    using Uptr = std::unique_ptr< PCertSignRequest >;
    /// @brief Default copy constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03762
    /// @needwork = dda
    /// @endcode
    PCertSignRequest(PCertSignRequest const &other) = delete;
    /// @brief Default move constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03763
    /// @needwork = dda
    /// @endcode
    PCertSignRequest(PCertSignRequest &&other) = delete;
    /// @brief Default copy assignment operator
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03764
    /// @needwork = dda
    /// @endcode
    PCertSignRequest &operator=(PCertSignRequest const &other) = delete;
    /// @brief Default move assignment operator
    /// @param other Another object instance of this class
    /// @return  *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03765
    /// @needwork = dda
    /// @endcode
    PCertSignRequest &operator=(PCertSignRequest &&other) = delete;

public:  // CertSignRequest interface
    /// @brief Verify the self-signature of the certificate request.
    /// @brief Verifies self-signed signature of the certificate request.
    /// @returns @c true if the signature is correct
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_40311}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02306}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03766
    /// @needwork = dda
    /// @endcode
    bool Verify() const noexcept override;
    /// @brief Export this certificate signing request in DER-encoded ASN1 format. Note: This is the CSR that can be sent to a CA to obtain a certificate. //2011 document does not have "const" modifier
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
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03767
    /// @needwork = dda
    /// @endcode
    ara::core::Result< ara::core::Vector< ara::core::Byte > > ExportASN1CertSignRequest() const noexcept override;
    /// @brief Return the format version of the certificate request.
    /// @brief Return format version of the certificate request.
    /// @returns format version of the certificate request
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_40314}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02306}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03768
    /// @needwork = dda
    /// @endcode
    uint32_t Version() const noexcept override;
    /// @brief Return the signature object of the request.
    /// @brief Return signature object of the request.
    /// @returns signature object of the request
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_40315}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02306}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03769
    /// @needwork = dda
    /// @endcode
    ara::crypto::cryp::Signature const &GetSignature() const noexcept override;

public:  // Serializable interface
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
    /// @trace_id_dd=DD_CRYPTO_03770
    /// @needwork = dda
    /// @endcode
    ara::core::Result< ara::core::Vector< ara::core::Byte > > ExportPublicly(FormatId formatId
                                                                             = kFormatDefault) const noexcept override;

public:  // BasicCertInfo interface
    /// @brief Get the key constraints of the key associated with this PKCS#10 object.
    /// @brief Get the key constraints for the key associated with this PKCS#10 object.
    /// @returns key constraints
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_40115}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02306}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03771
    /// @needwork = dda
    /// @endcode
    KeyConstraints GetConstraints() const noexcept override;
    /// @brief Get the path length constraint defined in the Basic Constraints extension.
    /// @brief Get the constraint on the path length defined in the Basic Constraints extension.
    /// @returns certification path length limit
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_40114}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02306}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03772
    /// @needwork = dda
    /// @endcode
    std::uint32_t GetPathLimit() const noexcept override;
    /// @brief Check if the CA attribute of X509v3 Basic Constraints is true (i.e., pathlen=0).
    /// @brief Check whether the CA attribute of X509v3 Basic Constraints is true (i.e. pathlen=0).
    /// @returns @c true if it is a CA request and @c false otherwise
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_40113}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02306}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03773
    /// @needwork = dda
    /// @endcode
    bool IsCa() const noexcept override;
    /// @brief Get Subject DN.
    /// @brief Get the subject DN.
    /// @returns subject DN
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_40112}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02306}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03774
    /// @needwork = dda
    /// @endcode
    X509DN const &SubjectDn() const noexcept override;
    /// @brief Load the Subject Public Key Info object into the domain of the specified crypto provider.
    ////        If (cryptoProvider == nullptr), the X509PublicKeyInfo object will be loaded
    ///        into the domain of the Stack-default Crypto Provider.
    /// @brief Load the subject public key information object to realm of specified crypto provider.
    ///         If (cryptoProvider == nullptr) then @c X509PublicKeyInfo object will be loaded in realm of the
    ///         Stack-default Crypto Provider.
    /// @param cryptoProvider Crypto provider unique pointer, indicating where the public key will be used
    /// @returns constant reference of the subject public key interface
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_40111}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02306}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03775
    /// @needwork = dda
    /// @endcode
    X509PublicKeyInfo const &SubjectPubKey(cryp::CryptoProvider *cryptoProvider = nullptr) const noexcept override;

public:
    /// @brief Parameterized constructor
    /// @param x509Provider Certificate provider
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03776
    /// @needwork = dda
    /// @endcode
    explicit PCertSignRequest(X509Provider &x509Provider) noexcept;
    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03777
    /// @needwork = dda
    /// @endcode
    ~PCertSignRequest() noexcept override;
    /// @brief Load a certificate from memory
    /// @param memCert Certificate memory data
    /// @param formatID Certificate encoding format
    /// @return true load cert sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03778
    /// @needwork = dda
    /// @endcode
    bool LoadCertFile(ReadOnlyMemRegion const &memCert, FormatId const formatID) noexcept;
    /// @brief Set version number: actual value will be -1
    /// @param nVersion Version number
    /// @return true if set version sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03779
    /// @needwork = dda
    /// @endcode
    bool SetVersion(int32_t const nVersion) noexcept;
    /// @brief Set SubjectDn
    /// @param memDn Certificate DN information
    /// @param formatID DN encoding format
    /// @return  ture if set DN sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03780
    /// @needwork = dda
    /// @endcode
    bool SetSubjectDn(ReadOnlyMemRegion const &memDn, FormatId const formatID) noexcept;
    /// @brief Set extension attributes
    /// @param memExt Extension data information
    /// @param formatID DN encoding format
    /// @return  true if set extension sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03781
    /// @needwork = dda
    /// @endcode
    bool SetExtension(ReadOnlyMemRegion const &memExt, FormatId const formatID) noexcept;
    /// @brief Set public key information
    /// @param pPrivateKey Private key object pointer
    /// @return true if set PublicKey sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03782
    /// @needwork = dda
    /// @endcode
    bool SetPublicKey(cryp::PrivateKey const *const pPrivateKey) noexcept;
    /// @brief Set signature, including hash algorithm, hash length, signature length, etc.
    /// @param nHashAlgId Hash algorithm crypto primitive ID
    /// @param nHashSize Hash length
    /// @param nSignatureSize Signature length
    /// @return  true if set Signatur sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03783
    /// @needwork = dda
    /// @endcode
    bool SetSignatur(CryptoAlgId const nHashAlgId, uint32_t const nHashSize, uint32_t const nSignatureSize) noexcept;
    /// @brief Associate an X509_REQ object
    /// @param pX509Req Certificate signing request object
    /// @return true if Attach X509Req sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03784
    /// @needwork = dda
    /// @endcode
    bool AttachX509Req(X509_REQ *const pX509Req) noexcept;
    /// @brief Return its own clone
    /// @name  CloneSelf
    /// @return self instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03785
    /// @needwork = dda
    /// @endcode
    PCertSignRequest::Uptr CloneSelf() const noexcept;

protected:
    /// @brief Load DER format certificate request
    /// @param memCert Certificate memory data
    /// @return X509_REQ structure pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03786
    /// @needwork = dda
    /// @endcode
    X509_REQ *_LoadCertFile_Der(ReadOnlyMemRegion const &memCert) noexcept;
    /// @brief Load PEM format certificate request
    /// @param memCert Certificate memory data
    /// @return X509_REQ structure pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03787
    /// @needwork = dda
    /// @endcode
    X509_REQ *_LoadCertFile_Pem(ReadOnlyMemRegion const &memCert) noexcept;
    /// @brief Export DER format certificate request
    /// @param vecData Receives exported certificate file
    /// @return  ture if export cert sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03788
    /// @needwork = dda
    /// @endcode
    bool _ExportCert_Der(ara::core::Vector< ara::core::Byte > &vecData) const noexcept;
    /// @brief Export PEM format certificate request
    /// @param vecData Receives exported certificate file
    /// @return  ture if export cert sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03789
    /// @needwork = dda
    /// @endcode
    bool _ExportCert_Pem(ara::core::Vector< ara::core::Byte > &vecData) const noexcept;
    /// @brief Initialize some certificate attribute information
    /// @param pX509Req Certificate signing request
    /// @return ture if init cert sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03790
    /// @needwork = dda
    /// @endcode
    bool _InitCertData(X509_REQ const *const pX509Req) const noexcept;
    /// @brief Initialize some certificate attribute information
    /// @param pX509Req Certificate signing request
    /// @return    ture if init SubjectDn sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03791
    /// @needwork = dda
    /// @endcode
    bool _InitSubjectDn(X509_REQ const *const pX509Req) const noexcept;
    /// @brief Process acquired Extension data via NID
    /// @param nID NID
    /// @param pfunc Callback function
    /// @return ture if DealAttr By ID sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03792
    /// @needwork = dda
    /// @endcode
    bool _DealAttrByID(int32_t const nID,
                       std::function< bool(uint8_t const *pData, int32_t nLen) > const &pfunc) const noexcept;
    /// @brief Get digest information from certificate request
    /// @param vecData Receives exported certificate file
    /// @return ture if get digest sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03793
    /// @needwork = dda
    /// @endcode
    bool _GetDigest(ara::core::Vector< ara::core::Byte > &vecData) const noexcept;
    /// @brief Delete X509_REQ
    /// @name   _DeleteX509Req
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03794
    /// @needwork = dda
    /// @endcode
    void _DeleteX509Req() noexcept;

private:
    /// @brief Signature object unique pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03795
    /// @needwork = dda
    /// @endcode
    mutable ara::crypto::cryp::Signature::Uptr pSignature_;
    /// @brief Signature algorithm ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03796
    /// @needwork = dda
    /// @endcode
    CryptoAlgId nSignHashAlgId_;
    /// @brief Hash size
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03797
    /// @needwork = dda
    /// @endcode
    uint32_t nHashSize_;
    /// @brief Signature size
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03798
    /// @needwork = dda
    /// @endcode
    uint32_t nSignatureSize_;
    /// @brief Certificate signing request
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03799
    /// @needwork = dda
    /// @endcode
    X509_REQ *pReqX509_;
    /// @brief DN
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03800
    /// @needwork = dda
    /// @endcode
    mutable X509DN dnSubject_;
    /// @brief Public key information
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03801
    /// @needwork = dda
    /// @endcode
    mutable X509PublicKeyInfo keyPublic_;
    /// @brief Certificate file path
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03802
    /// @needwork = dda
    /// @endcode
    mutable ara::core::String stCertFileName_;
    /// @brief Full-parameter constructor qac
    /// @param x509Provider Certificate provider
    /// @param pSignature Signature object
    /// @param nSignHashAlgId Hash algorithm ID
    /// @param nHashSize Hash length
    /// @param nSignatureSize Signature length
    /// @param pReqX509 Certificate signing request object
    /// @param dnSubject Subject DN
    /// @param keyPublic Public key
    /// @param stCertFileName Certificate file path
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03803
    /// @needwork = dda
    /// @endcode
    PCertSignRequest(X509Provider &x509Provider,
                     ara::crypto::cryp::Signature::Uptr pSignature,
                     CryptoAlgId const nSignHashAlgId,
                     uint32_t const nHashSize,
                     uint32_t const nSignatureSize,
                     X509_REQ *const pReqX509,
                     X509DN const *const dnSubject,
                     X509PublicKeyInfo const *const keyPublic,
                     ara::core::String stCertFileName) noexcept;

public:
    /// @brief Get permanent storage file name; volatile storage is empty
    /// @name  GetStorageFile
    /// @returns Permanent storage file name
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03804
    /// @needwork = dda
    /// @endcode
    ara::core::StringView GetStorageFile() const noexcept
    {
        return ara::core::StringView{stCertFileName_.data(), stCertFileName_.length()};
    }
    /// @brief Get permanent storage file name; volatile storage is empty
    /// @name  SetStorageFile
    /// @param stCertFileName Certificate file path
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03805
    /// @needwork = dda
    /// @endcode
    void SetStorageFile(ara::core::StringView const &stCertFileName) const noexcept
    {
        stCertFileName_ = stCertFileName;
    }
};
//********************************/
}  // namespace  isoft_def
}  // namespace x509
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_X509_PUHUA_CERT_SIGN_REQUEST_H_
