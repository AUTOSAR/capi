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
/// @file       isoft_secret_seed.h
/// @brief      AutoSar-Crypto Encryption Module
/// @details    Secret seed object interface.
/// @date       2022-05-24
/// @author     Chang Zheng
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification Log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// <tr><td>2022-05-24  <td>1.0.0    <td>Chang Zheng  <td>Initial version creation
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Default Encryption/Secret Seed
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_06005
/// @unit_name=PSecretSeed
/// @unit_description=Secret Seed
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_CRYP_PUHUA_SECRET_SEED_H_
#define ARA_CRYPTO_CRYP_PUHUA_SECRET_SEED_H_

#include "ara/crypto/cryp/cryobj/secret_seed.h"
#include "ara/crypto/cryp/isoft_auto_buff.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
//********************************/
/// @brief Secret seed object interface.
///     This object contains a raw bit sequence of specific length (without any filtering of allowed/disallowed values)!
///     Secret seed values can only be loaded into non-key inputs of cryptographic transformation contexts (e.g., IV/salt/nonce)!
///     The bit length of the secret seed is specific to the cryptographic algorithm and corresponds to the maximum of its input/output/salt block lengths.
/// @brief Secret Seed object interface.
///     This object contains a raw bit sequence of specific length (without any filtering of allowed/disallowed values)!
///     The secret seed value can be loaded only to a non-key input of a cryptographic transformation context (like
///     IV/salt/nonce)! Bit length of the secret seed is specific to concret crypto algorithm and corresponds to maximum
///     of its input/output/salt block-length.
/// @interface PSecretSeed
/// @AUTOSAR_SWS {SWS_CRYPT_23000}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02007}
//********************************/
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01254
/// @trace_id_dd=DD_CRYPTO_03265
/// @needwork = ad
/// @endcode
class PSecretSeed : public SecretSeed
{
private:
    /// @brief Data buffer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03266
    /// @needwork = dda
    /// @endcode
    internal::PAutoBuff dataBuff_;
    /// @brief Whether export is allowed
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03267
    /// @needwork = dda
    /// @endcode
    bool bExportable_;
    /// @brief Whether temporary
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03268
    /// @needwork = dda
    /// @endcode
    bool bSession_;
    /// @brief Scope of use
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03269
    /// @needwork = dda
    /// @endcode
    Usage usage_;
    /// @brief Length depends on the block size of the algorithm used
    /// @brief For example, 3DES is 8 bytes, AES128 is 16 bytes, etc.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03270
    /// @needwork = dda
    /// @endcode
    uint32_t nKeyLen_;
    /// @brief Object unique ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03271
    /// @needwork = dda
    /// @endcode
    COIdentifier keyId_;
    /// @brief Parameterized constructor
    /// @param dataBuff Data buffer
    /// @param bExportable Whether export is allowed
    /// @param bSession Whether temporary session
    /// @param usage Scope of use
    /// @param pKeyData Key data
    /// @param nKeyLen Key length: in bytes
    /// @param keyId IO interface ID corresponding to the key material
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03272
    /// @needwork = dda
    /// @endcode
    PSecretSeed(internal::PAutoBuff const *const dataBuff,
                bool const bExportable,
                bool const bSession,
                Usage const usage,
                void const *const pKeyData,
                uint32_t const nKeyLen,
                COIdentifier const &keyId) noexcept;

public:
    /// @brief Constant unique smart pointer of the interface.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01254
    /// @trace_id_dd=DD_CRYPTO_06322
    /// @needwork = dd
    /// @endcode
    using Uptrc = std::unique_ptr< PSecretSeed const >;
    /// @brief Unique smart pointer of the interface.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01254
    /// @trace_id_dd=DD_CRYPTO_06323
    /// @needwork = dd
    /// @endcode
    using Uptr = std::unique_ptr< PSecretSeed >;

public:
    /// @brief Parameterized constructor
    /// @param pKeyData Key data
    /// @param nKeyLen Key length: in bytes
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01255
    /// @trace_id_dd=DD_CRYPTO_03273
    /// @needwork = ad
    /// @endcode
    explicit PSecretSeed(void const *const pKeyData, uint32_t const nKeyLen) noexcept;
    /// @brief Default virtual destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01256
    /// @trace_id_dd=DD_CRYPTO_03274
    /// @needwork = ad
    /// @endcode
    ~PSecretSeed() noexcept override = default;
    /// @brief Default copy assignment function
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01257
    /// @trace_id_dd=DD_CRYPTO_03275
    /// @needwork = ad
    /// @endcode
    PSecretSeed &operator=(PSecretSeed const &other) noexcept;
    /// @brief Default move assignment function
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01258
    /// @trace_id_dd=DD_CRYPTO_03276
    /// @needwork = ad
    /// @endcode
    PSecretSeed &operator=(PSecretSeed &&other) noexcept;
    /// @brief Default copy constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01259
    /// @trace_id_dd=DD_CRYPTO_03277
    /// @needwork = ad
    /// @endcode
    PSecretSeed(PSecretSeed const &other) noexcept = delete;
    /// @brief Default move constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01260
    /// @trace_id_dd=DD_CRYPTO_03278
    /// @needwork = ad
    /// @endcode
    PSecretSeed(PSecretSeed &&other) noexcept = delete;

public:
    /// @brief Clone this SecretSeed object into a new session object.
    ///     The created object instance is session and non-exportable; the "AllowedUsageFlags" attribute of the "cloned" object is the same as that of the source object!
    ///     If the size of the xorDelta parameter is smaller than the value size of this seed, then only the corresponding leading bytes of the original seed should be XOR-ed, and the rest should be copied unchanged.
    ///     If the size of the xorDelta parameter is larger than the value size of this seed, the extra bytes of xorDelta shall be ignored.
    /// @brief Clone this Secret Seed object to new session object.
    ///         Created object instance is session and non-exportable, @c AllowedUsageFlags attribute of the "cloned"
    ///         object is identical to this attribute of the source object! If size of the @c xorDelta argument is less
    ///         than the value size of this seed then only correspondent number of leading bytes of the original seed
    ///         should be XOR-ed, but the rest should be copied without change. If size of the @c xorDelta argument is
    ///         larger than the value size of this seed then extra bytes of the @c xorDelta should be ignored.
    /// @param xorDelta  optional "delta" value that must be XOR-ed with the "cloned" copy of the original seed
    /// @returns unique smart pointer to "cloned" session @c SecretSeed object
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_23011}
    /// @uptrace={RS_CRYPTO_02007}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01261
    /// @trace_id_dd=DD_CRYPTO_03279
    /// @needwork = ad
    /// @endcode
    ara::core::Result< SecretSeed::Uptr > Clone(ReadOnlyMemRegion const &xorDelta
                                                = ReadOnlyMemRegion()) const noexcept override;
    /// @brief Set the value of this seed object to "jump" from the initial state to the specified step count according to the "count" expression defined by the cryptographic algorithm associated with this object.
    ///         Steps may have positive and negative values, corresponding to forward and backward directions of "jumping," respectively, but a value of 0 means copying from the value to this seed object only. The seed size of the from parameter must be greater than or equal to this seed size.
    /// @brief Set value of this seed object as a "jump" from an initial state to specified number of steps,
    ///     according to "counting" expression defined by a cryptographic algorithm associated with this object.
    ///     @c steps may have positive and negative values that correspond to forward and backward direction of the
    ///     "jump" respectively, but 0 value means only copy @c from value to this seed object.
    ///     Seed size of the @c from argument always must be greater or equal of this seed size.
    /// @param from  source object that keeps the initial value for jumping from
    /// @param steps  number of steps for the "jump"
    /// @returns reference to this updated object
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_23012}
    /// @uptrace={RS_CRYPTO_02007}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @error: SecurityErrorDomain::kIncompatibleObject  if this object and the @c from argument are associated
    ///     with incompatible cryptographic algorithms
    /// @error: SecurityErrorDomain::kInvalidInputSize  if value size of the @c from seed is less then
    ///     value size of this one
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01262
    /// @trace_id_dd=DD_CRYPTO_03280
    /// @needwork = ad
    /// @endcode
    ara::core::Result< void > JumpFrom(SecretSeed const &from, std::int64_t steps) noexcept override;
    /// @brief Set the value of this seed object to "jump" from its current state to the specified step count according to the "count" expression defined by the cryptographic algorithm associated with this object.
    ///         Steps may have positive and negative values, corresponding to forward and backward directions of "jumping," respectively, but a value of 0 means the current seed value is unchanged.
    /// @brief Set value of this seed object as a "jump" from it's current state to specified number of steps,
    ///     according to "counting" expression defined by a cryptographic algorithm associated with this object.
    ///     @c steps may have positive and negative values that correspond to forward and backward direction of the
    ///     "jump" respectively, but 0 value means no changes of the current seed value.
    /// @param steps  number of "steps" for jumping (forward or backward) from the current state
    /// @returns reference to this updated object
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_23014}
    /// @uptrace={RS_CRYPTO_02007}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01263
    /// @trace_id_dd=DD_CRYPTO_03281
    /// @needwork = ad
    /// @endcode
    SecretSeed &Jump(std::int64_t steps) noexcept override;
    /// @brief Set the next value of the secret seed according to the "count" expression defined by the cryptographic algorithm associated with this object.
    /// If the relevant cryptographic algorithm does not specify a "count" expression, the generic increment operation must be used as the default implementation (little-endian representation, i.e., the first byte is the least significant).
    /// @brief Set next value of the secret seed according to "counting" expression defined by a cryptographic algorithm
    ///     associated with this object. If the associated cryptographic algorithm doesn't specify a "counting"
    ///     expression then generic increment operation must be implemented as default (little-endian notation, i.e.
    ///     first byte is least significant).
    /// @returns reference to this updated object
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_23013}
    /// @uptrace={RS_CRYPTO_02007}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01264
    /// @trace_id_dd=DD_CRYPTO_03282
    /// @needwork = ad
    /// @endcode
    SecretSeed &Next() noexcept override;
    /// @brief XOR the value of this seed object with another seed object and save the result to this object.
    ///     If the seed sizes in this object and the source parameter are different, only the corresponding leading bytes of this seed object should be updated.
    /// @brief XOR value of this seed object with another one and save result to this object.
    ///       If seed sizes in this object and in the @c source argument are different then only correspondent number
    ///       of leading bytes in this seed object should be updated.
    /// @brief @qac Cannot be modified [2074]: Assignment operator 'XXXX &operator ^=(XXXX) &' does not return an lvalue reference to the class in which it is defined.
    /// @param source right argument for the XOR operation
    /// @return reference to this updated object
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_23015}
    /// @uptrace={RS_CRYPTO_02007}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01265
    /// @trace_id_dd=DD_CRYPTO_03283
    /// @needwork = ad
    /// @endcode
    // PRQA S 2074 QAC /// @qac: AUTOSAR standard interface
    SecretSeed &operator^=(SecretSeed const &source) &noexcept override;
    // PRQA L:QAC
    /// @brief XOR the value of the seed object with a memory area and save the result to this object.
    //          If the seed sizes in this object and the source parameter are different, the leading bytes of this seed object should be updated.
    /// @brief XOR value of this seed object with provided memory region and save result to this object.
    ///       If seed sizes in this object and in the @c source argument are different then only correspondent number
    ///       of leading bytes of this seed object should be updated.
    /// @param source  right argument for the XOR operation
    /// @returns reference to this updated object
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_23016}
    /// @uptrace={RS_CRYPTO_02007}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01266
    /// @trace_id_dd=DD_CRYPTO_03284
    /// @needwork = ad
    /// @qac Cannot be modified [2074]: Assignment operator 'XXXX &operator ^=(XXXX) &' does not return an lvalue reference to the class in which it is defined.
    /// @endcode
    // PRQA S 2074 QAC /// @qac: AUTOSAR standard interface
    SecretSeed &operator^=(ReadOnlyMemRegion const &source) &noexcept override;
    // PRQA L:QAC

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
    /// @trace_id_ad=AD_CRYPTO_01267
    /// @trace_id_dd=DD_CRYPTO_03285
    /// @needwork = ad
    /// @endcode
    inline Usage GetAllowedUsage() const noexcept override { return usage_; }

public:  // CryptoObject interface
    /// @brief Returns the CryptoPrimitivId of this cryptobject.
    /// @return CryptoPrimitivId instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01268
    /// @trace_id_dd=DD_CRYPTO_03286
    /// @needwork = ad
    /// @endcode
    CryptoPrimitiveId::Uptr GetCryptoPrimitiveId() const noexcept override;
    /// @brief Returns the object's COIdentifier, including object type and UID.
    ///         Objects without a COUID assigned cannot be serialized/exported or saved to non-volatile storage safely. Several related objects of different types may share a COUID (e.g., private and public key), but the combination of COUID and object type must always be unique!
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
    /// @trace_id_ad=AD_CRYPTO_01269
    /// @trace_id_dd=DD_CRYPTO_03287
    /// @needwork = ad
    /// @endcode
    COIdentifier GetObjectId() const noexcept override;
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
    /// @trace_id_ad=AD_CRYPTO_01270
    /// @trace_id_dd=DD_CRYPTO_03288
    /// @needwork = ad
    /// @endcode
    std::size_t GetPayloadSize() const noexcept override;
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
    /// @trace_id_ad=AD_CRYPTO_01271
    /// @trace_id_dd=DD_CRYPTO_03289
    /// @needwork = ad
    /// @endcode
    COIdentifier HasDependence() const noexcept override;
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
    /// @trace_id_ad=AD_CRYPTO_01272
    /// @trace_id_dd=DD_CRYPTO_03290
    /// @needwork = ad
    /// @endcode
    inline bool IsExportable() const noexcept override { return bExportable_; }
    /// @brief Returns the object's "session" (or "temporary") attribute.
    /// A temporary object cannot be saved to persistent storage pointed to by IOInterface! A temporary object will be safely destroyed along with this interface instance!
    /// Non-session objects must have an assigned COUID (see GetObjectId()).
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
    /// @trace_id_ad=AD_CRYPTO_01273
    /// @trace_id_dd=DD_CRYPTO_03291
    /// @needwork = ad
    /// @endcode
    inline bool IsSession() const noexcept override { return bSession_; }
    /// @brief Save self to the provided IOInterface. Cryptobjects with the "session" attribute cannot be saved in KeySlots.
    /// @brief Save itself to provided IOInterface
    ///     A CryptoObject with property "session" cannot be saved in a KeySlot.
    /// @param container IOInterface representing underlying storage
    /// @return has value if save sucess otherwise false
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
    /// @trace_id_ad=AD_CRYPTO_01274
    /// @trace_id_dd=DD_CRYPTO_03292
    /// @needwork = ad
    /// @endcode
    ara::core::Result< void > Save(IOInterface &container) const noexcept override;
    /// @brief Set AllowedUsage
    /// @param usage Scope of use. Alias to the container type for bit-flags of allowed usages of the object.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01275
    /// @trace_id_dd=DD_CRYPTO_03293
    /// @needwork = ad
    /// @endcode
    inline void SetAllowedUsage(Usage const usage) noexcept { usage_ = usage; }
    /// @brief Set bExportable
    /// @param bExportable Whether export is allowed
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01276
    /// @trace_id_dd=DD_CRYPTO_03294
    /// @needwork = ad
    /// @endcode
    inline void SetExportable(bool const bExportable) noexcept { bExportable_ = bExportable; }
    /// @brief Set bSession
    /// @param bSession Whether temporary session
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01277
    /// @trace_id_dd=DD_CRYPTO_03295
    /// @needwork = ad
    /// @endcode
    inline void SetSession(bool const bSession) noexcept { bSession_ = bSession; }

public:
    /// @brief Get secret seed data
    /// @return Pointer to secret seed object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01278
    /// @trace_id_dd=DD_CRYPTO_03296
    /// @needwork = ad
    /// @endcode
    uint8_t const *GetSecretSeedData() const noexcept;
    /// @brief Set Hash data
    /// @param pSecretSeedData Secret seed data
    /// @param nSecretSeedLength Secret seed data length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01279
    /// @trace_id_dd=DD_CRYPTO_03297
    /// @needwork = ad
    /// @endcode
    void SetSecretSeedData(void const *const pSecretSeedData, uint32_t const nSecretSeedLength) noexcept;
    /// @brief Randomly initialize key
    /// @param nKeyBitLength Key length: in bits
    /// @return ture if init key sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01280
    /// @trace_id_dd=DD_CRYPTO_03298
    /// @needwork = ad
    /// @endcode
    bool RandomInitKey(int32_t const nKeyBitLength) noexcept;
    /// @brief Get Data
    /// @return Pointer to secret seed object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01281
    /// @trace_id_dd=DD_CRYPTO_03299
    /// @needwork = ad
    /// @endcode
    uint8_t const *GetData() const noexcept;
    /// @brief Get the Data Length
    /// @return Secret seed length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01282
    /// @trace_id_dd=DD_CRYPTO_03300
    /// @needwork = ad
    /// @endcode
    uint32_t GetDataLen() const noexcept;

protected:
    /// @brief Save seed content to IOInterface
    /// @param container IO interface
    /// @return true if save to key slot sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03301
    /// @needwork = dda
    /// @endcode
    bool _SaveToKeySlot(IOInterface &container) const noexcept;
};
//********************************/
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_CRYP_PUHUA_SECRET_SEED_H_
