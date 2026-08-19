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
/// @file       isoft_key_public.h
/// @brief      AutoSar-Crypto encryption/decryption module
/// @details    General asymmetric public key interface.
/// @date       2022-03-02
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// <tr><td>2022-03-02  <tr>1.0.0    <tr>hanjingjing      <tr>Create initial version</tr>
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Default encryption/Asymmetric public key
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_06005
/// @unit_name=PKeyPublic_Base
/// @unit_description=General asymmetric public key
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_CRYP_PUHUA_KEY_PUBLIC_H_
#define ARA_CRYPTO_CRYP_PUHUA_KEY_PUBLIC_H_

#include "ara/core/string.h"
#include "ara/crypto/cryp/cryobj/public_key.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
//********************************/
/// @brief General asymmetric public key interface.
/// @brief General Asymmetric Public Key interface.
/// @code{.isoft}
/// @interface PKeyPublic_Base
/// @AUTOSAR_SWS {SWS_CRYPT_22700}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02202}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01327
/// @trace_id_dd=DD_CRYPTO_03493
/// @needwork = ad
/// @endcode
class PKeyPublic_Base : public PublicKey
{
public:
    /// @brief default virtual destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01328
    /// @trace_id_dd=DD_CRYPTO_03494
    /// @needwork = ad
    /// @endcode
    ~PKeyPublic_Base() noexcept override = default;
    /// @brief default constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01329
    /// @trace_id_dd=DD_CRYPTO_03495
    /// @needwork = ad
    /// @endcode
    PKeyPublic_Base() = default;
    /// @brief default copy constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01330
    /// @trace_id_dd=DD_CRYPTO_03496
    /// @needwork = ad
    /// @endcode
    PKeyPublic_Base(PKeyPublic_Base const &other) = delete;
    /// @brief default move constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01331
    /// @trace_id_dd=DD_CRYPTO_03497
    /// @needwork = ad
    /// @endcode
    PKeyPublic_Base(PKeyPublic_Base &&other) = delete;
    /// @brief default copy assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01332
    /// @trace_id_dd=DD_CRYPTO_03498
    /// @needwork = ad
    /// @endcode
    PKeyPublic_Base &operator=(PKeyPublic_Base const &other) = delete;
    /// @brief default move assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01333
    /// @trace_id_dd=DD_CRYPTO_03499
    /// @needwork = ad
    /// @endcode
    PKeyPublic_Base &operator=(PKeyPublic_Base &&other) = delete;

public:  // CryptoObject interface
    /// @brief Returns the CryptoPrimitivId of this crypto object.
    /// @brief Return the CryptoPrimitivId of this CryptoObject.
    /// @return CryptoPrimitivId instance
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_20505}
    /// @uptrace={RS_CRYPTO_02005}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01334
    /// @trace_id_dd=DD_CRYPTO_03500
    /// @needwork = ad
    /// @endcode
    CryptoPrimitiveId::Uptr GetCryptoPrimitiveId() const noexcept override = 0;
    /// @brief Returns the COIdentifier of the object, which includes the object type and UID.
    ///         Objects without an assigned COUID cannot be (safely) serialized/exported or saved to non-volatile storage. Several related objects of different types can share a COUID (e.g., private and public keys), but the combination of COUID and object type must always be unique!
    /// @brief Return the object's COIdentifier, which includes the object's type and UID.
    ///         An object that has no assigned @a COUID cannot be (securely) serialized / exported or saved to a
    ///         non-volatile storage. An object should not have a @a COUID if it is session and non-exportable
    ///         simultaneously A few related objects of different types can share a single @a COUID (e.g. private and
    ///         public keys), but a combination of @a COUID and object type must be unique always!
    /// @returns the object's COIdentifier including the object's type and COUID (or an empty COUID, if this object is
    ///     not identifiable).
    /// @return COIdentifier of the object
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_20514}
    /// @uptrace={RS_CRYPTO_02005}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01335
    /// @trace_id_dd=DD_CRYPTO_03501
    /// @needwork = ad
    /// @endcode
    COIdentifier GetObjectId() const noexcept override { return keyId_; }
    /// @brief Returns the actual size of the object payload. In bytes.
    /// @brief Return actual size of the object’s payload.
    ///         Returned value always must be less than or equal to the maximum payload size expected for this primitive
    ///         and object type, it is available via call: My Provider().GetPayloadStorageSize(GetObjectType(),
    ///         GetPrimitiveId()).Value(); Returned value does not take into account the object’s meta-information
    ///         properties, but their size is fixed and common for all crypto objects independently from their actual
    ///         type. During an allocation of a @c TrustedContainer, Crypto Providers (and Key Storage Providers)
    ///         reserve space for an object’s meta-information automatically, according to their implementation details.
    /// @returns size in bytes of the object's payload required for its storage
    /// @return Actual payload size
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_20516}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01336
    /// @trace_id_dd=DD_CRYPTO_03502
    /// @needwork = ad
    /// @endcode
    std::size_t GetPayloadSize() const noexcept override = 0;
    /// @brief Returns the COIdentifier of the crypto object that this crypto object depends on.
    ///         For a signature object, this method must return a reference to the corresponding signature verification public key!
    ///         Uniquely identifying a crypto object requires two components: cryptoobjectuid and cryptoobjecttype.
    /// @brief Return the COIdentifier of the CryptoObject that this CryptoObject depends on.
    ///         For signatures objects this method @b must return a reference to correspondent signature verification
    ///         public key! Unambiguous identification of a CryptoObject requires both components: @c CryptoObjectUid
    ///         and @c CryptoObjectType.
    /// @returns target COIdentifier of the existing dependence or @c CryptoObjectType::kUnknown and empty COUID, if the
    ///     current object does not depend on another CryptoObject
    /// @return COIdentifier of the crypto object that this crypto object depends on
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_20515}
    /// @uptrace={RS_CRYPTO_02005}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01337
    /// @trace_id_dd=DD_CRYPTO_03503
    /// @needwork = ad
    /// @endcode
    COIdentifier HasDependence() const noexcept override
    {
        COIdentifier keyID;
        keyID.mCOType = CryptoObjectType::kUndefined;
        keyID.mCouid  = CryptoObjectUid();
        return keyID;
    }
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
    /// @trace_id_ad=AD_CRYPTO_01338
    /// @trace_id_dd=DD_CRYPTO_03504
    /// @needwork = ad
    /// @endcode
    inline bool IsExportable() const noexcept override { return bExportable_; }
    /// @brief Returns the "session" (or "temporary") attribute of the object.
    ///         A temporary object cannot be saved to a persistent storage location pointed to by IOInterface! A temporary object will be safely destroyed together with this interface instance!
    ///         Non-session objects must have an assigned COUID (see GetObjectId()).
    /// @brief Return the "session" (or "temporary") attribute of the object.
    ///      A temporary object cannot be saved to a persistent storage location pointed to by an IOInterface!
    ///      A temporary object will be securely destroyed together with this interface instance!
    ///      A non-session object must have an assigned @a COUID (see @c GetObjectId()).
    /// @returns @c true if the object is temporay (i.e. its life time is limited by the current session only)
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_20512}
    /// @uptrace={RS_CRYPTO_02003}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01339
    /// @trace_id_dd=DD_CRYPTO_03505
    /// @needwork = ad
    /// @endcode
    inline bool IsSession() const noexcept override { return bSession_; }
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
    /// @trace_id_ad=AD_CRYPTO_01340
    /// @trace_id_dd=DD_CRYPTO_03506
    /// @needwork = ad
    /// @endcode
    ara::core::Result< void > Save(IOInterface &container) const noexcept override = 0;

public:  // RestrictedUseObject interface
    /// @brief Get the allowed usage of this object.
    /// @brief Get allowed usages of this object.
    /// @returns a combination of bit-flags that specifies allowed applications of the object
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_24811}
    /// @uptrace={RS_CRYPTO_02008}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01341
    /// @trace_id_dd=DD_CRYPTO_03507
    /// @needwork = ad
    /// @endcode
    inline Usage GetAllowedUsage() const noexcept override { return usage_; }

public:  // PublicKey interface
    /// @brief Check the correctness of the key.
    /// @brief Check the key for its correctness.
    /// @param strongCheck  the severeness flag that indicates type of the required check: strong (if @c true) or
    ///     fast (if @c false)
    /// @returns @c true if the key is correct
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_22711}
    /// @uptrace={RS_CRYPTO_02202}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01342
    /// @trace_id_dd=DD_CRYPTO_03508
    /// @needwork = ad
    /// @endcode
    bool CheckKey(bool strongCheck = true) const noexcept override = 0;
    /// @brief Calculate the hash value of the public key value. The raw public key value BLOB can be obtained through the Serializable interface.
    /// @brief Calculate hash of the Public Key value.
    ///        The original public key value BLOB is available via the @c Serializable interface.
    /// @param hashFunc  a hash-function instance that should be used the hashing
    /// @returns a buffer preallocated for the resulting hash value
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_22712}
    /// @uptrace={RS_CRYPTO_02202}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @error: SecurityErrorDomain::kInsufficientCapacity   if size of the hash buffer is not enough for storing of the
    ///     result
    /// @erro: SecurityErrorDomain::kIncompleteArgState     if the @c hashFunc context is not initialized
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01343
    /// @trace_id_dd=DD_CRYPTO_03509
    /// @needwork = ad
    /// @endcode
    ara::core::Result< ara::core::Vector< ara::core::Byte > > HashPublicKey(
        HashFunctionCtx &hashFunc) const noexcept override;

public
    :  // Custom virtual functions: derived from the Serializable interface, created as new virtual functions because multiple inheritance is not allowed.
    /// @brief Serialize the Public itself.
    virtual ara::core::Result< ara::core::Vector< ara::core::Byte > >
    /// @name   ExportPublicly
    /// @param formatId Data format: Raw, DER, PEM, etc.
    /// @return Public key data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_08781
    /// @needwork = dda
    /// @endcode
    ExportPublicly(Serializable::FormatId formatId = Serializable::kFormatDefault) const noexcept = 0;

public:
    /// @brief Set AllowedUsage
    /// @param usage Usage Alias to the container type for bit-flags of allowed usages of the object.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01344
    /// @trace_id_dd=DD_CRYPTO_03510
    /// @needwork = ad
    /// @endcode
    inline void SetAllowedUsage(Usage const usage) noexcept { usage_ = usage; }
    /// @brief Set Exportable
    /// @param bExportable Whether exportable
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01345
    /// @trace_id_dd=DD_CRYPTO_03511
    /// @needwork = ad
    /// @endcode
    inline void SetExportable(bool const bExportable) noexcept { bExportable_ = bExportable; }
    /// @brief Set Session
    /// @param bSession Whether temporary session
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01346
    /// @trace_id_dd=DD_CRYPTO_03512
    /// @needwork = ad
    /// @endcode
    inline void SetSession(bool const bSession) noexcept { bSession_ = bSession; }
    /// @brief Set COIdentifier
    /// @param keyId IO interface ID corresponding to the key material
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01347
    /// @trace_id_dd=DD_CRYPTO_03513
    /// @needwork = ad
    /// @endcode
    inline void SetCOIdentifier(COIdentifier const &keyId) noexcept { keyId_ = keyId; }

private:
    /// @brief Whether exportable
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03514
    /// @needwork = dda
    /// @endcode
    bool bExportable_{false};
    /// @brief Whether temporary
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03515
    /// @needwork = dda
    /// @endcode
    bool bSession_{false};
    /// @brief Usage range
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03516
    /// @needwork = dda
    /// @endcode
    Usage usage_{kAllowPrototypedOnly};
    /// @brief Object unique ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03517
    /// @needwork = dda
    /// @endcode
    COIdentifier keyId_{};
};
//********************************/
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_CRYP_PUHUA_KEY_PUBLIC_H_
