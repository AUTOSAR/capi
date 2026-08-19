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
/// @file       x509_public_key_info.h
/// @brief      AutoSar-Crypto Certificate Management Module
/// @details
/// @date       2021-12-21
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Certificate Components/X.509 Public Key
/// @interface_level=software
/// @trace_id_sr=SR_CRYPTO_03004
/// @unit_name=X509PublicKeyInfo
/// @unit_description=X.509 Public Key Information
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_X509_X509_PUBLIC_KEY_INFO_H_
#define ARA_CRYPTO_X509_X509_PUBLIC_KEY_INFO_H_

#include <openssl/rsa.h>

#include "ara/core/result.h"
#include "ara/crypto/common/base_id_types.h"
#include "ara/crypto/common/serializable.h"
#include "ara/crypto/cryp/cryobj/crypto_primitive_id.h"
#include "ara/crypto/cryp/cryobj/public_key.h"

namespace ara {
namespace crypto {
namespace x509 {
//********************************/
//- @interface X509PublicKeyInfo
/// @brief X.509 public key information interface.
/// @brief X.509 Public Key Information interface.
/// @code{.isoft}
/// @trace_id_sws={SWS_CRYPT_24400}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02307}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02661
/// @trace_id_dd=DD_CRYPTO_05496
/// @needwork = ad
/// @endcode
class X509PublicKeyInfo : public Serializable
{
public:
    /// @brief Unique smart pointer for the constant interface.
    /// @brief Unique smart pointer of the interface.
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_24401}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02307}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_03235
    /// @trace_id_dd=DD_CRYPTO_06447
    /// @needwork = ad
    /// @endcode
    using Uptrc = std::unique_ptr< X509PublicKeyInfo const >;

public:
    /// @brief Get the subject's public key object.
    ///         The created PublicKey object is session-level and non-exportable, because a generic X.509 certificate or Certificate Signing Request (CSR) does not have a COUID for the public key, so it can only be saved or transmitted as part of the corresponding certificate or CSR.
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_24414}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02108}
    /// @uptrace={RS_CRYPTO_02306}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02662
    /// @trace_id_dd=DD_CRYPTO_05497
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< ara::crypto::cryp::PublicKey::Uptrc >
    /// @brief Get public key object of the subject.
    /// Created @c PublicKey object is @b session and @c non-exportable, because generic X.509 certificate or
    ///       certificate signing request (CSR) doesn't have COUID of the public key, therefore it should be saved
    ///       or transmitted only as a part of correspondent certificate or CSR.
    /// @returns unique smart pointer to the created public key of the subject
    GetPublicKey() const noexcept;
    /// @brief Get the ID of the hash algorithm required by the current signature algorithm.
    /// @brief Get an ID of hash algorithm required by current signature algorithm.
    /// @returns required hash algorithm ID or @c kAlgIdAny if the signature algorithm specification does not include a
    /// concrete hash function
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_24412}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02663
    /// @trace_id_dd=DD_CRYPTO_05498
    /// @needwork = ad
    /// @endcode
    inline virtual CryptoAlgId GetRequiredHashAlgId() const noexcept { return nHashAlgId_; }
    /// @brief Get the hash size required by the current signature algorithm.
    /// @brief Get the hash size required by current signature algorithm.
    /// @returns required hash size in bytes
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_24411}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02309}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02664
    /// @trace_id_dd=DD_CRYPTO_05499
    /// @needwork = ad
    /// @endcode
    virtual std::size_t GetRequiredHashSize() const noexcept { return static_cast< std::size_t >(nHashSize_); }
    /// @brief Get the size of the signature value produced and required by the current algorithm.
    /// @brief Get size of the signature value produced and required by the current algorithm.
    /// @returns size of the signature value in bytes
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_24413}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02665
    /// @trace_id_dd=DD_CRYPTO_05500
    /// @needwork = ad
    /// @endcode
    inline virtual std::size_t GetSignatureSize() const noexcept { return static_cast< std::size_t >(nSignatureSize_); }
    /// @brief Get the CryptoPrimitiveId instance of this class.
    /// @brief Get the CryptoPrimitiveId instance of this class.
    /// @return Algorithm instance
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_24410}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02307}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02666
    /// @trace_id_dd=DD_CRYPTO_05501
    /// @needwork = ad
    /// @endcode
    virtual ara::crypto::cryp::CryptoPrimitiveId::Uptrc GetAlgorithmId() noexcept;
    /// @brief Verify the consistency between the provided and stored public keys. This method only compares the public key values.
    /// @brief Verify the sameness of the provided and kept public keys.
    /// This method compare the public key values only.
    /// @param publicKey  the public key object for comparison
    /// @returns @c true if values of the stored public key and object provided by the argument are identical and
    ///          @c false otherwise
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_24415}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02306}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02667
    /// @trace_id_dd=DD_CRYPTO_05502
    /// @needwork = ad
    /// @endcode
    virtual bool IsSameKey(ara::crypto::cryp::PublicKey const &publicKey) const noexcept;

public:  // Serializable interface
    /// @brief Expose serialization itself.
    /// @brief Serialize itself publicly.
    /// @param formatId  the Crypto Provider specific identifier of the output format
    /// @returns a buffer with the serialized object
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_10711}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02112}
    /// @error: SecurityErrorDomain::kInsufficientCapacity   if (output.empty() == false), but it's capacity is less
    /// than required
    /// @error: SecurityErrorDomain::kUnknownIdentifier      if an unknown format ID was specified
    /// @error: SecurityErrorDomain::kUnsupportedFormat      if the specified format ID is not supported for this object
    /// type
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02668
    /// @trace_id_dd=DD_CRYPTO_05503
    /// @needwork = ad
    /// @endcode
    ara::core::Result< ara::core::Vector< ara::core::Byte > > ExportPublicly(FormatId formatId
                                                                             = kFormatDefault) const noexcept override;

private:
    /// @brief X509_PUBKEY pointer
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05504
    /// @needwork = dda
    /// @endcode
    X509_PUBKEY *pSubjectPublicKeyInfo_{nullptr};
    /// @brief Hash algorithm crypto primitive ID
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05505
    /// @needwork = dda
    /// @endcode
    CryptoAlgId nHashAlgId_{kAlgIdUndefined};
    /// @brief Hash length
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05506
    /// @needwork = dda
    /// @endcode
    uint32_t nHashSize_{0U};
    /// @brief Signature length
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05507
    /// @needwork = dda
    /// @endcode
    uint32_t nSignatureSize_{0U};

public:
    /// @brief Default constructor
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02669
    /// @trace_id_dd=DD_CRYPTO_05508
    /// @needwork = ad
    /// @endcode
    X509PublicKeyInfo() noexcept = default;
    /// @brief Set public key data
    /// @param pSubjectPublicKeyInfo X509_PUBKEY public key pointer
    /// @param nHashAlgId Hash algorithm crypto primitive ID
    /// @param nHashSize Hash length
    /// @param nSignatureSize Signature length
    /// @return true if update sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02670
    /// @trace_id_dd=DD_CRYPTO_05509
    /// @needwork = ad
    /// @endcode
    bool UpdatePubkeyData(X509_PUBKEY *const pSubjectPublicKeyInfo,
                          CryptoAlgId const nHashAlgId,
                          uint32_t const nHashSize,
                          uint32_t const nSignatureSize) noexcept;
};
//********************************/
}  // namespace x509
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_X509_X509_PUBLIC_KEY_INFO_H_
