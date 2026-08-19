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
/// @file       isoft_key_private.h
/// @brief      AutoSar-Crypto Encryption Module
/// @details    General asymmetric private key interface.
/// @date       2021-12-21
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification Log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// <tr><td>2021-12-21  <td>1.0.0    <td>hanjingjing  <td>Initial version creation
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Default Encryption/Asymmetric Private Key
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_06005
/// @unit_name=PKeyPrivate_Base
/// @unit_description=General asymmetric private key
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_CRYP_PUHUA_KEY_PRIVATE_H_
#define ARA_CRYPTO_CRYP_PUHUA_KEY_PRIVATE_H_

#include "ara/core/string.h"
#include "ara/crypto/cryp/cryobj/private_key.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
//********************************/
/// @brief General asymmetric private key interface.
/// @brief Generalized Asymmetric Private Key interface.
/// @code{.isoft}
/// @interface PKeyPrivate_Base
/// @AUTOSAR_SWS {SWS_CRYPT_22500}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02002}
/// @uptrace={RS_CRYPTO_02403}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01284
/// @trace_id_dd=DD_CRYPTO_03324
/// @needwork = ad
/// @endcode
class PKeyPrivate_Base : public PrivateKey
{
public:
    /// @brief Default virtual destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01285
    /// @trace_id_dd=DD_CRYPTO_03325
    /// @needwork = ad
    /// @endcode
    ~PKeyPrivate_Base() noexcept override = default;
    /// @brief Default constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01286
    /// @trace_id_dd=DD_CRYPTO_03326
    /// @needwork = ad
    /// @endcode
    PKeyPrivate_Base() noexcept;
    /// @brief Default move constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01287
    /// @trace_id_dd=DD_CRYPTO_03327
    /// @needwork = ad
    /// @endcode
    PKeyPrivate_Base(PKeyPrivate_Base&& other) = delete;
    /// @brief Default move assignment function
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01288
    /// @trace_id_dd=DD_CRYPTO_03328
    /// @needwork = ad
    /// @endcode
    PKeyPrivate_Base& operator=(PKeyPrivate_Base&& other) = delete;
    /// @brief Default copy assignment function
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01289
    /// @trace_id_dd=DD_CRYPTO_03329
    /// @needwork = ad
    /// @endcode
    PKeyPrivate_Base& operator=(PKeyPrivate_Base const& other) = delete;
    /// @brief Default copy constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01290
    /// @trace_id_dd=DD_CRYPTO_03330
    /// @needwork = ad
    /// @endcode
    PKeyPrivate_Base(PKeyPrivate_Base const& other) = delete;

public:  // CryptoObject interface
    /// @brief Returns the CryptoPrimitivId of this cryptobject.
    /// @brief Return the CryptoPrimitivId of this CryptoObject.
    /// @return CryptoPrimitivId instance
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_20505}
    /// @uptrace={RS_CRYPTO_02005}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01291
    /// @trace_id_dd=DD_CRYPTO_03331
    /// @needwork = ad
    /// @endcode
    CryptoPrimitiveId::Uptr GetCryptoPrimitiveId() const noexcept override = 0;
    /// @brief Returns the object's COIdentifier, including object type and UID.
    ///     Objects without a COUID assigned cannot be serialized/exported or saved to non-volatile storage safely.
    ///     Several related objects of different types may share a COUID (e.g., private and public key), but the combination of COUID and object type must always be unique!
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
    /// @trace_id_ad=AD_CRYPTO_01292
    /// @trace_id_dd=DD_CRYPTO_03332
    /// @needwork = ad
    /// @endcode
    COIdentifier GetObjectId() const noexcept override
    {
        if (bSession_ && !bExportable_) {
            COIdentifier keyId;
            keyId.mCOType = CryptoObjectType::kUndefined;
            keyId.mCouid  = CryptoObjectUid();
            return keyId;
        }
        return keyId_;
    }
    /// @brief Returns the actual size of the object payload. In bytes.
    /// @brief Return actual size of the object’s payload.
    ///         Returned value always must be less than or equal to the maximum payload size expected for this primitive
    ///         and object type, it is available via call: My Provider().GetPayloadStorageSize(GetObjectType(),
    ///         GetPrimitiveId()).Value(); Returned value does not take into account the object’s meta-information
    ///         properties, but their size is fixed and common for all crypto objects independently from their actual
    ///         type. During an allocation of a @c TrustedContainer, Crypto Providers (and Key Storage Providers)
    ///         reserve space for an object’s meta-information automatically, according to their implementation details.
    /// @returns size in bytes of the object's payload required for its storage
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_20516}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01293
    /// @trace_id_dd=DD_CRYPTO_03333
    /// @needwork = ad
    /// @endcode
    std::size_t GetPayloadSize() const noexcept override = 0;
    /// @brief Returns the COIdentifier of the cryptobject that this cryptobject depends on.
    ///         For signature objects, this method must return the reference to the corresponding signature verification public key!
    ///         Explicitly identifying a cryptobject requires two components: cryptobjectuid and cryptobjecttype.
    /// @brief Return the COIdentifier of the CryptoObject that this CryptoObject depends on.
    ///         For signatures objects this method @b must return a reference to correspondent signature verification
    ///         public key! Unambiguous identification of a CryptoObject requires both components: @c CryptoObjectUid
    ///         and @c CryptoObjectType.
    /// @returns target COIdentifier of the existing dependence or @c CryptoObjectType::kUnknown and empty COUID, if the
    ///     current object does not depend on another CryptoObject
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_20515}
    /// @uptrace={RS_CRYPTO_02005}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01294
    /// @trace_id_dd=DD_CRYPTO_03334
    /// @needwork = ad
    /// @endcode
    COIdentifier HasDependence() const noexcept override
    {
        COIdentifier keyID;
        keyID.mCOType = CryptoObjectType::kUndefined;
        keyID.mCouid  = CryptoObjectUid();
        return keyID;
    };
    /// @brief Gets the exportability property of the crypto object. An exportable object must have an assigned COUID (see GetObjectId()).
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
    /// @trace_id_ad=AD_CRYPTO_01295
    /// @trace_id_dd=DD_CRYPTO_03335
    /// @needwork = ad
    /// @endcode
    inline bool IsExportable() const noexcept override { return bExportable_; }
    /// @brief Returns the object's "session" (or "temporary") attribute.
    ///         A temporary object cannot be saved to persistent storage pointed to by IOInterface! A temporary object will be safely destroyed along with this interface instance!
    ///         Non-session objects must have an assigned COUID (see GetObjectId()).
    /// @brief Return the "session" (or "temporary") attribute of the object.
    ///      A temporary object cannot be saved to a persistent storage location pointed to by an IOInterface!
    ///      A temporary object will be securely destroyed together with this interface instance!
    ///      A non-session object must have an assigned @a COUID (see @c GetObjectId()).
    /// @returns @c true if the object is temporay (i.e. its life time is limited by the current session only)
    /// @return true if is session object false otherwise
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_20512}
    /// @uptrace={RS_CRYPTO_02003}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01296
    /// @trace_id_dd=DD_CRYPTO_03336
    /// @needwork = ad
    /// @endcode
    inline bool IsSession() const noexcept override { return bSession_; }
    /// @brief Save self to the provided IOInterface. Cryptobjects with the "session" attribute cannot be saved in KeySlots.
    /// @brief Save itself to provided IOInterface
    ///     A CryptoObject with property "session" cannot be saved in a KeySlot.
    /// @param container IOInterface representing underlying storage
    /// @return has value if save sucess false otherwise
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
    /// @trace_id_ad=AD_CRYPTO_01297
    /// @trace_id_dd=DD_CRYPTO_03337
    /// @needwork = ad
    /// @endcode
    ara::core::Result< void > Save(IOInterface& container) const noexcept override = 0;

public:  // RestrictedUseObject interface
    /// @brief Gets the allowed usage of this object.
    /// @brief Get allowed usages of this object.
    /// @returns a combination of bit-flags that specifies allowed applications of the object
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_24811}
    /// @uptrace={RS_CRYPTO_02008}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01298
    /// @trace_id_dd=DD_CRYPTO_03338
    /// @needwork = ad
    /// @endcode
    inline Usage GetAllowedUsage() const noexcept override { return usage_; }

public:  // PrivateKey interface
    /// @brief Gets the public key corresponding to this private key.
    /// @brief Get the public key correspondent to this private key.
    /// @returns unique smart pointer to the public key correspondent to this private key
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_22511}
    /// @uptrace={RS_CRYPTO_02108}
    /// @uptrace={RS_CRYPTO_02115}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01299
    /// @trace_id_dd=DD_CRYPTO_03339
    /// @needwork = ad
    /// @endcode
    ara::core::Result< PublicKey::Uptrc > GetPublicKey() const noexcept override = 0;

public:  // Custom virtual function: Derived from Serializable interface due to disallowing multiple inheritance.
    /// @brief Serialize Public itself.
    virtual ara::core::Result< ara::core::Vector< ara::core::Byte > >
    /// @name   ExportPublicly
    /// @param formatId Data format: Raw, DER, PEM, etc.
    /// @return Public key data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_08780
    /// @needwork = dda
    /// @endcode
    ExportPublicly(Serializable::FormatId formatId = Serializable::kFormatDefault) const noexcept = 0;

public:
    /// @brief Set Allowed Usage
    /// @param usage Scope of use. Alias to the container type for bit-flags of allowed usages of the object.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01300
    /// @trace_id_dd=DD_CRYPTO_03340
    /// @needwork = ad
    /// @endcode
    inline void SetAllowedUsage(Usage const usage) noexcept { usage_ = usage; }
    /// @brief Set Exportable
    /// @param bExportable Whether export is allowed
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01301
    /// @trace_id_dd=DD_CRYPTO_03341
    /// @needwork = ad
    /// @endcode
    inline void SetExportable(bool const bExportable) noexcept { bExportable_ = bExportable; }
    /// @brief Set Session
    /// @param bSession Whether temporary session
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01302
    /// @trace_id_dd=DD_CRYPTO_03342
    /// @needwork = ad
    /// @endcode
    inline void SetSession(bool const bSession) noexcept { bSession_ = bSession; }

private:
    /// @brief Whether export is allowed
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03343
    /// @needwork = dda
    /// @endcode
    bool bExportable_{false};
    /// @brief Whether temporary
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03344
    /// @needwork = dda
    /// @endcode
    bool bSession_{false};
    /// @brief Scope of use
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03345
    /// @needwork = dda
    /// @endcode
    Usage usage_{kAllowPrototypedOnly};
    /// @brief Object unique ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03346
    /// @needwork = dda
    /// @endcode
    COIdentifier keyId_{};
};
//********************************/
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_CRYP_PUHUA_KEY_PRIVATE_H_
