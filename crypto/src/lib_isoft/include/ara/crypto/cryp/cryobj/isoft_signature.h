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
/// @file       isoft_signature.h
/// @brief      AutoSar-Crypto encryption/decryption module
/// @details    Signature container interface for storing digital signatures, hash digests, (Hash-based) Message Authentication Code (MAC/HMAC).
/// @date       2022-03-25
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// <tr><td>2022-03-25  <tr>1.0.0    <tr>hanjingjing      <td>Create initial version</td>
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Default encryption/Signature storage
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_06005
/// @unit_name=PSignature
/// @unit_description=Signature storage
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_CRYP_PUHUA_SIGNATURE_H_
#define ARA_CRYPTO_CRYP_PUHUA_SIGNATURE_H_

#include "ara/crypto/cryp/cryobj/isoft_crypto_primitive_id.h"
#include "ara/crypto/cryp/cryobj/signature.h"
#include "ara/crypto/cryp/isoft_auto_buff.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
//********************************/
/// @brief Signature container interface for storing digital signatures, hash digests, (Hash-based) Message Authentication Code (MAC/HMAC).
///     In the case of a key signature (digital signature or MAC/HMAC), the COUID of the signature verification key can be obtained by calling CryptoObject::HasDependency()!
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01349
/// @trace_id_dd=DD_CRYPTO_03535
/// @needwork = ad
/// @endcode
class PSignature : public Signature
{
private:
    /// @brief Whether exportable
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03536
    /// @needwork = dda
    /// @endcode
    bool bExportable_;
    /// @brief Whether temporary
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03537
    /// @needwork = dda
    /// @endcode
    bool bSession_;
    /// @brief Data buffer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03538
    /// @needwork = dda
    /// @endcode
    internal::PAutoBuff dataBuff_;
    /// @brief Hash algorithm crypto primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03539
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::AlgId nHashAlgID_;
    /// @brief Encryption algorithm crypto primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03540
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::AlgId nCryptoAlgId_;
    /// @brief COIdentifier of the dependent crypto object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03541
    /// @needwork = dda
    /// @endcode
    COIdentifier dePendencedKeyId_;
    /// @brief Parameterized constructor
    /// @param bExportable Whether exportable
    /// @param bSession Whether temporary session
    /// @param dataBuff Data buffer
    /// @param nHashAlgID Hash algorithm crypto primitive ID
    /// @param nCryptoAlgId Encryption algorithm crypto primitive ID
    /// @param dePendencedKeyId
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03542
    /// @needwork = dda
    /// @endcode
    PSignature(bool const bExportable,
               bool const bSession,
               internal::PAutoBuff const* const dataBuff,
               CryptoPrimitiveId::AlgId const nHashAlgID,
               CryptoPrimitiveId::AlgId const nCryptoAlgId,
               COIdentifier const& dePendencedKeyId) noexcept;

public:
    /// @brief Unique smart pointer for the interface.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01349
    /// @trace_id_dd=DD_CRYPTO_06339
    /// @needwork = dd
    /// @endcode
    using Uptr = std::unique_ptr< PSignature >;

public:
    /// @brief Parameterized constructor
    /// @param nHashAlgID Hash algorithm crypto primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01350
    /// @trace_id_dd=DD_CRYPTO_03543
    /// @needwork = ad
    /// @endcode
    explicit PSignature(CryptoPrimitiveId::AlgId const nHashAlgID) noexcept;
    /// @brief Parameterized constructor
    /// @param nHashAlgID Hash algorithm crypto primitive ID
    /// @param nCryptoAlgId Encryption algorithm crypto primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01351
    /// @trace_id_dd=DD_CRYPTO_03544
    /// @needwork = ad
    /// @endcode
    explicit PSignature(CryptoPrimitiveId::AlgId const nHashAlgID,
                        CryptoPrimitiveId::AlgId const nCryptoAlgId) noexcept;
    /// @brief default virtual destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01352
    /// @trace_id_dd=DD_CRYPTO_03545
    /// @needwork = ad
    /// @endcode
    ~PSignature() noexcept override = default;
    /// @brief default copy assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01353
    /// @trace_id_dd=DD_CRYPTO_03546
    /// @needwork = ad
    /// @endcode
    PSignature& operator=(PSignature const& other) noexcept;
    /// @brief default move assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01354
    /// @trace_id_dd=DD_CRYPTO_03547
    /// @needwork = ad
    /// @endcode
    PSignature& operator=(PSignature&& other) noexcept;
    /// @brief default copy constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01355
    /// @trace_id_dd=DD_CRYPTO_03548
    /// @needwork = ad
    /// @endcode
    PSignature(PSignature const& other) noexcept = delete;
    /// @brief default move constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01356
    /// @trace_id_dd=DD_CRYPTO_03549
    /// @needwork = ad
    /// @endcode
    PSignature(PSignature&& other) noexcept = delete;

public:  // CryptoObject interface
    /// @brief Returns the CryptoPrimitivId of this crypto object.
    /// @return CryptoPrimitivId instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01357
    /// @trace_id_dd=DD_CRYPTO_03550
    /// @needwork = ad
    /// @endcode
    CryptoPrimitiveId::Uptr GetCryptoPrimitiveId() const noexcept override;
    /// @brief Returns the COIdentifier of the object, which includes the object type and UID.
    ///         Objects without an assigned COUID cannot be (safely) serialized/exported or saved to non-volatile storage. Several related objects of different types can share a COUID (e.g., private and public keys), but the combination of COUID and object type must always be unique!
    /// @brief Return the object's COIdentifier, which includes the object's type and UID.
    ///         An object that has no assigned @a COUID cannot be (securely) serialized / exported or saved to a
    ///         non-volatile storage. An object should not have a @a COUID if it is session and non-exportable
    ///         simultaneously A few related objects of different types can share a single @a COUID (e.g. private and
    ///         public keys), but a combination of @a COUID and object type must be unique always!
    /// @returns the object's COIdentifier including the object's type and COUID (or an empty COUID, if this object is
    ///     not identifiable).
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_20514}
    /// @uptrace={RS_CRYPTO_02005}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01358
    /// @trace_id_dd=DD_CRYPTO_03551
    /// @needwork = ad
    /// @endcode
    COIdentifier GetObjectId() const noexcept override;
    /// @brief Returns the actual size of the object payload. In bytes.
    /// @brief Return actual size of the object’s payload.
    ///     Returned value always must be less than or equal to the maximum payload size expected for this primitive
    ///     and object type, it is available via call: My Provider().GetPayloadStorageSize(GetObjectType(),
    ///     GetPrimitiveId()).Value(); Returned value does not take into account the object’s meta-information
    ///     properties, but their size is fixed and common for all crypto objects independently from their actual
    ///     type. During an allocation of a @c TrustedContainer, Crypto Providers (and Key Storage Providers)
    ///     reserve space for an object’s meta-information automatically, according to their implementation details.
    /// @returns size in bytes of the object's payload required for its storage
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_20516}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01359
    /// @trace_id_dd=DD_CRYPTO_03552
    /// @needwork = ad
    /// @endcode
    std::size_t GetPayloadSize() const noexcept override;
    /// @brief Returns the COIdentifier of the crypto object that this crypto object depends on.
    /// For a signature object, this method must return a reference to the corresponding signature verification public key!
    /// Uniquely identifying a crypto object requires two components: cryptoobjectuid and cryptoobjecttype.
    /// @brief Return the COIdentifier of the CryptoObject that this CryptoObject depends on.
    ///     For signatures objects this method @b must return a reference to correspondent signature verification
    ///     public key! Unambiguous identification of a CryptoObject requires both components: @c CryptoObjectUid
    ///     and @c CryptoObjectType.
    /// @returns target COIdentifier of the existing dependence or @c CryptoObjectType::kUnknown and empty COUID, if the
    ///     current object does not depend on another CryptoObject
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_20515}
    /// @uptrace={RS_CRYPTO_02005}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01360
    /// @trace_id_dd=DD_CRYPTO_03553
    /// @needwork = ad
    /// @endcode
    COIdentifier HasDependence() const noexcept override;
    /// @brief Get the exportability attribute of the crypto object. An exportable object must have an assigned COUID (see GetObjectId()).
    /// @brief Get the exportability attribute of the crypto object. An exportable object must have an assigned @a COUID
    ///     (see @c GetObjectId()).
    /// @returns @c true if the object is exportable (i.e. if it can be exported outside the trusted environment of the
    ///     Crypto Provider)
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_20513}
    /// @uptrace={RS_CRYPTO_02005}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01361
    /// @trace_id_dd=DD_CRYPTO_03554
    /// @needwork = ad
    /// @endcode
    bool IsExportable() const noexcept override;
    /// @brief Returns the "session" (or "temporary") attribute of the object.
    /// A temporary object cannot be saved to a persistent storage location pointed to by IOInterface! A temporary object will be safely destroyed together with this interface instance!
    /// Non-session objects must have an assigned COUID (see GetObjectId()).
    /// @brief Return the "session" (or "temporary") attribute of the object.
    ///     A temporary object cannot be saved to a persistent storage location pointed to by an IOInterface!
    ///     A temporary object will be securely destroyed together with this interface instance!
    ///     A non-session object must have an assigned @a COUID (see @c GetObjectId()).
    /// @returns @c true if the object is temporay (i.e. its life time is limited by the current session only)
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_20512}
    /// @uptrace={RS_CRYPTO_02003}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01362
    /// @trace_id_dd=DD_CRYPTO_03555
    /// @needwork = ad
    /// @endcode
    bool IsSession() const noexcept override;
    /// @brief Save itself to the provided IOInterface. Crypto objects with the "session" attribute cannot be saved in KeySlot.
    /// @brief Save itself to provided IOInterface
    ///     A CryptoObject with property "session" cannot be saved in a KeySlot.
    /// @param container IOInterface representing underlying storage
    /// @returns has value if save sucess false otherwise
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_20517}
    /// @uptrace={RS_CRYPTO_02004}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @error: SecurityErrorDomain::kIncompatibleObject     if the object is "session", but the IOInterface represents
    ///     a KeySlot.
    /// @error: SecurityErrorDomain::kContentRestrictions    if the object doesn't satisfy the slot restrictions
    /// @error: SecurityErrorDomain::kInsufficientCapacity   if the capacity of the target container is not enough, i.e.
    ///     if <tt>(container.Capacity() < this->StorageSize())</tt>
    /// @error: SecurityErrorDomain::kModifiedResource       if the underlying resource has been modified after  the
    ///     IOInterface has been opened, i.e., the IOInterface has been invalidated.
    /// @error: SecurityErrorDomain::kUnreservedResource     if the IOInterface is not opened writeable.
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01363
    /// @trace_id_dd=DD_CRYPTO_03556
    /// @needwork = ad
    /// @endcode
    ara::core::Result< void > Save(IOInterface& container) const noexcept override;

public:  // Serializable interface
    /// @brief Serialize itself publicly.
    /// @brief Serialize itself publicly.
    /// @param formatId  the Crypto Provider specific identifier of the output format
    /// @returns a buffer with the serialized object
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_10711}
    /// @uptrace={RS_CRYPTO_02112}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @error: SecurityErrorDomain::kInsufficientCapacity   if (output.empty() == false), but it's capacity is less
    ///     than required
    /// @error: SecurityErrorDomain::kUnknownIdentifier      if an unknown format ID was specified
    /// @error: SecurityErrorDomain::kUnsupportedFormat      if the specified format ID is not supported for this object
    ///     type
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01364
    /// @trace_id_dd=DD_CRYPTO_03557
    /// @needwork = ad
    /// @endcode
    ara::core::Result< ara::core::Vector< ara::core::Byte > > ExportPublicly(
        Serializable::FormatId formatId = Serializable::kFormatDefault) const noexcept override;

public:  // Signature interface
    /// @brief Get the ID of the hash algorithm used for generating this signature object.
    /// @brief Get an ID of hash algorithm used for this signature object production.
    /// @returns ID of used hash algorithm only (without signature algorithm specification)
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_23311}
    /// @uptrace={RS_CRYPTO_02204}
    /// @uptrace={RS_CRYPTO_02203}
    /// @uptrace={RS_CRYPTO_02205}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01365
    /// @trace_id_dd=DD_CRYPTO_03558
    /// @needwork = ad
    /// @endcode
    inline CryptoPrimitiveId::AlgId GetHashAlgId() const noexcept override { return nHashAlgID_; }
    /// @brief Get the hash size required by the current signature algorithm.
    /// @brief Get the hash size required by current signature algorithm.
    /// @returns required hash size in bytes
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_23312}
    /// @uptrace={RS_CRYPTO_02309}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01366
    /// @trace_id_dd=DD_CRYPTO_03559
    /// @needwork = ad
    /// @endcode
    std::size_t GetRequiredHashSize() const noexcept override;

public:
    /// @brief Set hash data.
    /// @param nHashAlgID Hash algorithm crypto primitive ID
    /// @param pSignatureData Signature data
    /// @param nSignatureLength Signature data length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01367
    /// @trace_id_dd=DD_CRYPTO_03560
    /// @needwork = ad
    /// @endcode
    void SetSignatureData(CryptoPrimitiveId::AlgId const nHashAlgID,
                          void const* const pSignatureData,
                          uint32_t const nSignatureLength) noexcept;
    /// @brief Set the public key COUID that the signature depends on.
    /// @param keyId IO interface ID corresponding to the key material
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01368
    /// @trace_id_dd=DD_CRYPTO_03561
    /// @needwork = ad
    /// @endcode
    void SetDependence(COIdentifier const& keyId) noexcept override;
    /// @brief When verifying a signature, retrieve the COUID of the public key that reported the error, for comparison with the public key COUID used by the verification context.
    /// This step is located in the VerifyPrehashed() function in ph_ctx_dsv_verifier_public.cpp
    /// @return Dependent COUID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01369
    /// @trace_id_dd=DD_CRYPTO_03562
    /// @needwork = ad
    /// @endcode
    CryptoObjectUid GetDependence() const noexcept override;
    /// @brief This implementation is only used for the SaveKeyToSlot template function to compile, has no actual effect.
    /// @return Usage restriction
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01370
    /// @trace_id_dd=DD_CRYPTO_03563
    /// @needwork = ad
    /// @endcode
    inline static AllowedUsageFlags GetAllowedUsage() noexcept { return kAllowPrototypedOnly; }
    /// @brief Get the crypto primitive ID of the encryption algorithm.
    /// @return Crypto primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01371
    /// @trace_id_dd=DD_CRYPTO_03564
    /// @needwork = ad
    /// @endcode
    inline CryptoPrimitiveId::AlgId GetCryptoAlgId() const noexcept { return nCryptoAlgId_; }

protected:
    /// @brief Determine whether the FormatID is supported. The signature only supports metadata format.
    /// @param nFormatID Certificate encoding format
    /// @param bCheckForWrite the value to Check for Write
    /// @return true if valid formatID false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03565
    /// @needwork = dda
    /// @endcode
    static bool IsValidFormatID(Serializable::FormatId const nFormatID, bool const bCheckForWrite) noexcept
    {
        if (bCheckForWrite) {
            return (nFormatID == Serializable::kFormatRawValueOnly);
        }
        /// Check nFormatID must be in the enumeration definition {Serializable::kFormatDefault, Serializable::kFormatPemEncoded}

        return (nFormatID == Serializable::kFormatDefault) || (nFormatID == Serializable::kFormatRawValueOnly)
               || (nFormatID == Serializable::kFormatDerEncoded) || (nFormatID == Serializable::kFormatPemEncoded);
    }
    /// @brief: Save signature information to IOInterface.
    /// @name  _SaveToKeySlot
    /// @param container IO interface
    /// @returns  true if save to keyslot sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03566
    /// @needwork = dda
    /// @endcode
    bool _SaveToKeySlot(IOInterface& container) const noexcept;
};
//********************************/
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_CRYP_PUHUA_SIGNATURE_H_
