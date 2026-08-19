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
/// @file       basic_cert_info.h
/// @brief      AutoSar-Crypto Certificate Management Module
/// @details    "Basic Certificate Information" interface.
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
/// @module_path=/CRYPTO/Certificate Components/X.509 Certificate
/// @interface_level=software
/// @trace_id_sr=SR_CRYPTO_03001
/// @unit_name=BasicCertInfo
/// @unit_description=Basic Certificate Information
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_X509_BASIC_CERT_INFO_H_
#define ARA_CRYPTO_X509_BASIC_CERT_INFO_H_

#include "ara/crypto/common/serializable.h"
#include "ara/crypto/cryp/crypto_provider.h"
#include "ara/crypto/x509/x509_dn.h"
#include "ara/crypto/x509/x509_object.h"
#include "ara/crypto/x509/x509_public_key_info.h"

namespace ara {
namespace crypto {
namespace x509 {
//********************************/
// -@interface BasicCertInfo
/// @brief "Basic Certificate Information" interface.
/// @brief Basic Certificate Information interface.
/// @trace_id_sws={SWS_CRYPT_40100}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02306}
//********************************/
/// @code{.isoft}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02609
/// @trace_id_dd=DD_CRYPTO_05421
/// @needwork = ad
/// @endcode
class BasicCertInfo : public X509Object
{
public:
    /// @brief X.509 v3 Key Usage constraint type definition.
    /// @brief X.509 v3 Key Constraints type definition.
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_40101}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02306}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_03232
    /// @trace_id_dd=DD_CRYPTO_06444
    /// @needwork = ad
    /// @endcode
    using KeyConstraints = std::uint32_t;

public:
    /// @brief Get the key constraints of the key associated with this PKCS#10 object.
    /// @brief Get the key constraints for the key associated with this PKCS#10 object.
    /// @returns key constraints
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_40115}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02306}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02610
    /// @trace_id_dd=DD_CRYPTO_05422
    /// @needwork = ad
    /// @endcode
    virtual KeyConstraints GetConstraints() const noexcept = 0;
    /// @brief Get the path length constraint defined in the basic constraints extension.
    /// @brief Get the constraint on the path length defined in the Basic Constraints extension.
    /// @returns certification path length limit
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_40114}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02306}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02611
    /// @trace_id_dd=DD_CRYPTO_05423
    /// @needwork = ad
    /// @endcode
    virtual std::uint32_t GetPathLimit() const noexcept = 0;
    /// @brief Check if the CA attribute of the X509v3 Basic Constraints is true (i.e., pathlen=0).
    /// @brief Check whether the CA attribute of X509v3 Basic Constraints is true (i.e. pathlen=0).
    /// @returns @c true if it is a CA request and @c false otherwise
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_40113}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02306}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02612
    /// @trace_id_dd=DD_CRYPTO_05424
    /// @needwork = ad
    /// @endcode
    virtual bool IsCa() const noexcept = 0;
    /// @brief Get the subject DN.
    /// @brief Get the subject DN.
    /// @returns subject DN
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_40112}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02306}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02613
    /// @trace_id_dd=DD_CRYPTO_05425
    /// @needwork = ad
    /// @endcode
    virtual X509DN const& SubjectDn() const noexcept = 0;
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
    /// @trace_id_ad=AD_CRYPTO_02614
    /// @trace_id_dd=DD_CRYPTO_05426
    /// @needwork = ad
    /// @endcode
    virtual X509PublicKeyInfo const& SubjectPubKey(cryp::CryptoProvider* cryptoProvider = nullptr) const noexcept = 0;

private:  // Constants of X.509 v3 Key Constraints
    /// @brief No key constraints.
    /// @brief No key constraints.
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_40150}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02306}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05427
    /// @needwork = dda
    /// @endcode
    KeyConstraints const kConstrNone{0U};
    /// @brief This key can be used for digital signature production.
    /// @brief The key can be used for digital signature production.
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_40151}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02306}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05428
    /// @needwork = dda
    /// @endcode
    KeyConstraints const kConstrDigitalSignature{0x8000U};
    /// @brief This key can be used in situations demanding "non-repudiation" assurance.
    /// @brief The key can be used in cases requiring the "non-repudiation" guarantee
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_40152}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02306}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05429
    /// @needwork = dda
    /// @endcode
    KeyConstraints const kConstrNonRepudiation{0x4000U};
    /// @brief This key can be used for key encryption.
    /// @brief The key can be used for key encipherment.
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_40153}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02306}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05430
    /// @needwork = dda
    /// @endcode
    KeyConstraints const kConstrKeyEncipherment{0x2000U};
    /// @brief This key can be used for data encryption.
    /// @brief The key can be used for data encipherment.
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_40154}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02306}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05431
    /// @needwork = dda
    /// @endcode
    KeyConstraints const kConstrDataEncipherment{0x1000U};
    /// @brief This key can be used for key agreement execution.
    /// @brief The key can be used for a key agreement protocol execution.
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_40155}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02306}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05432
    /// @needwork = dda
    /// @endcode
    KeyConstraints const kConstrKeyAgreement{0x0800U};
    /// @brief This key can be used for certificate signing.
    /// @brief The key can be used for certificates signing.
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_40156}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02306}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05433
    /// @needwork = dda
    /// @endcode
    KeyConstraints const kConstrKeyCertSign{0x0400U};
    /// @brief This key can be used for Certificate Revocation List (CRL) signing.
    /// @brief The key can be used for Certificates Revokation Lists (CRL) signing.
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_40157}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02306}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05434
    /// @needwork = dda
    /// @endcode
    KeyConstraints const kConstrCrlSign{0x0200U};
    /// @brief Encryption key can only be used for encryption.
    /// @brief The enciphermet key can be used for enciphering only.
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_40158}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02306}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05435
    /// @needwork = dda
    /// @endcode
    KeyConstraints const kConstrEncipherOnly{0x0100U};
    /// @brief Encryption key can only be used for decryption.
    /// @brief The enciphermet key can be used for deciphering only.
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_40159}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02306}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05436
    /// @needwork = dda
    /// @endcode
    KeyConstraints const kConstrDecipherOnly{0x0080U};

public:
    /// @brief Get the constant for key usage: Key usage
    /// @return Key usage
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02615
    /// @trace_id_dd=DD_CRYPTO_05437
    /// @needwork = ad
    /// @endcode
    KeyConstraints GetkConstrNone() const noexcept { return kConstrNone; }
    /// @brief Get the constant for key usage: This key can be used for digital signature production.
    /// @return Digital signature constraint
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02616
    /// @trace_id_dd=DD_CRYPTO_05438
    /// @needwork = ad
    /// @endcode
    KeyConstraints GetkConstrDigitalSignature() const noexcept { return kConstrDigitalSignature; }
    /// @brief Get the constant for key usage: This key can be used for key encryption.
    /// @return Key encryption constraint
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02617
    /// @trace_id_dd=DD_CRYPTO_05439
    /// @needwork = ad
    /// @endcode
    KeyConstraints GetkConstrNonRepudiation() const noexcept { return kConstrNonRepudiation; }
    /// @brief Get the constant for key usage:
    /// @return Key encryption constraint
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02618
    /// @trace_id_dd=DD_CRYPTO_05440
    /// @needwork = ad
    /// @endcode
    KeyConstraints GetkConstrKeyEncipherment() const noexcept { return kConstrKeyEncipherment; }
    /// @brief Get the constant for key usage: This key can be used for data encryption.
    /// @return Data encryption constraint
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02619
    /// @trace_id_dd=DD_CRYPTO_05441
    /// @needwork = ad
    /// @endcode
    KeyConstraints GetkConstrDataEncipherment() const noexcept { return kConstrDataEncipherment; }
    /// @brief Get the constant for key usage: This key can be used for key agreement execution.
    /// @return Key exchange constraint
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02620
    /// @trace_id_dd=DD_CRYPTO_05442
    /// @needwork = ad
    /// @endcode
    KeyConstraints GetkConstrKeyAgreement() const noexcept { return kConstrKeyAgreement; }
    /// @brief Get the constant for key usage: This key can be used for key agreement execution.
    /// @return Certificate signing constraint
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02621
    /// @trace_id_dd=DD_CRYPTO_05443
    /// @needwork = ad
    /// @endcode
    KeyConstraints GetkConstrKeyCertSign() const noexcept { return kConstrKeyCertSign; }
    /// @brief Get the constant for key usage: This key can be used for Certificate Revocation List (CRL) signing.
    /// @return CRL signing constraint
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02622
    /// @trace_id_dd=DD_CRYPTO_05444
    /// @needwork = ad
    /// @endcode
    KeyConstraints GetkConstrCrlSign() const noexcept { return kConstrCrlSign; }
    /// @brief Get the constant for key usage: Encryption key can only be used for encryption.
    /// @return Encryption only constraint
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02623
    /// @trace_id_dd=DD_CRYPTO_05445
    /// @needwork = ad
    /// @endcode
    KeyConstraints GetkConstrEncipherOnly() const noexcept { return kConstrEncipherOnly; }
    /// @brief Get the constant for key usage: Encryption key can only be used for decryption.
    /// @return Decryption only constraint
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02624
    /// @trace_id_dd=DD_CRYPTO_05446
    /// @needwork = ad
    /// @endcode
    KeyConstraints GetkConstrDecipherOnly() const noexcept { return kConstrDecipherOnly; }

public:
    /// @brief Default constructor
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02625
    /// @trace_id_dd=DD_CRYPTO_05447
    /// @needwork = ad
    /// @endcode
    BasicCertInfo() = delete;
    /// @brief Default virtual destructor P
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02626
    /// @trace_id_dd=DD_CRYPTO_05448
    /// @needwork = ad
    /// @endcode
    ~BasicCertInfo() noexcept override = default;

public:
    /// @brief Default copy assignment operator
    /// @brief Copy-assign another BasicCertInfo to this instance.
    /// @param other Another instance of this class
    /// @returns *this, containing the contents of @a other
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dda
    /// @endcode
    BasicCertInfo& operator=(BasicCertInfo const& other) noexcept = delete;
    /// @brief Default move constructor
    /// @brief Move-assign another BasicCertInfo to this instance.
    /// @param other Another instance of this class
    /// @return *this, containing the contents of @a other
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dda
    /// @endcode
    BasicCertInfo& operator=(BasicCertInfo&& other) noexcept = delete;

protected:
    /// @brief Default move constructor
    /// @param other Another instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dda
    /// @endcode
    BasicCertInfo(BasicCertInfo&& other) noexcept = default;
    /// @brief Default copy constructor
    /// @param other Another instance of this class
    /// @code{.isoft}
    /// @interface_level=software
    /// @needwork = dda
    /// @endcode
    BasicCertInfo(BasicCertInfo const& other) noexcept = default;

    using X509Object::X509Object;
    /// @brief Get the length of the hash algorithm identified by NID_
    /// @param nIdHashType Hash type
    /// @return Length of the hash algorithm
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02631
    /// @trace_id_dd=DD_CRYPTO_05453
    /// @needwork = ad
    /// @endcode
    static uint32_t TransHashLen(int32_t const nIdHashType) noexcept;
    /// @brief Openssl.nid => CryptoAlgId
    /// @param nAlgOid Openssl.nid in Openssl
    /// @return Algorithm ID
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02632
    /// @trace_id_dd=DD_CRYPTO_05454
    /// @needwork = ad
    /// @endcode
    static CryptoAlgId Trans2AlgId(uint32_t const nAlgOid) noexcept;
    /// @brief CryptoAlgId => Openssl.nid
    /// @param algId Crypto primitive ID
    /// @return Crypto algorithm combination ID
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02633
    /// @trace_id_dd=DD_CRYPTO_05455
    /// @needwork = ad
    /// @endcode
    static uint32_t Trans2Nid(CryptoAlgId const algId) noexcept;
};
//********************************/
}  // namespace x509
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_X509_BASIC_CERT_INFO_H_
