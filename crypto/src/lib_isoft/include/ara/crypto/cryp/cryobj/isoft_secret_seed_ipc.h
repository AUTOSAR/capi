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
/// @file       isoft_secret_seed_ipc.h
/// @brief      AutoSar-Crypto Encryption Module
/// @details    Secret seed object IPC interface.
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
/// @unit_name=PSecretSeed_Ipc
/// @unit_description=IPC Secret Seed
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_CRYP_PUHUA_SECRET_SEED_IPC_H_
#define ARA_CRYPTO_CRYP_PUHUA_SECRET_SEED_IPC_H_

#include "ara/crypto/cryp/cryobj/secret_seed.h"
#include "ara/crypto/cryp/isoft_auto_buff.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
//********************************/
/// @brief Secret seed object IPC interface.
///     This object contains a raw bit sequence of specific length (without any filtering of allowed/disallowed values)!
///     Secret seed values can only be loaded into non-key inputs of cryptographic transformation contexts (e.g., IV/salt/nonce)!
///     The bit length of the secret seed is specific to the cryptographic algorithm and corresponds to the maximum of its input/output/salt block lengths.
/// @brief Secret Seed object interface.
///     This object contains a raw bit sequence of specific length (without any filtering of allowed/disallowed values)!
///     The secret seed value can be loaded only to a non-key input of a cryptographic transformation context (like
///     IV/salt/nonce)! Bit length of the secret seed is specific to concret crypto algorithm and corresponds to maximum
///     of its input/output/salt block-length.
/// @code{.isoft}
/// @AUTOSAR_SWS {SWS_CRYPT_23000}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02007}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01303
/// @trace_id_dd=DD_CRYPTO_03347
/// @needwork = ad
/// @endcode
class PSecretSeed_Ipc : public SecretSeed
{
private:
    /// @brief Whether export is allowed
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03348
    /// @needwork = dda
    /// @endcode
    bool bExportable_;
    /// @brief Whether temporary
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03349
    /// @needwork = dda
    /// @endcode
    bool bSession_;
    /// @brief Scope of use
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03350
    /// @needwork = dda
    /// @endcode
    Usage usage_;
    /// @brief Length in bits
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03351
    /// @needwork = dda
    /// @endcode
    uint32_t nBitlen_;
    /// @brief Key slot ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03352
    /// @needwork = dda
    /// @endcode
    uint32_t nSlotId_;
    /// @brief IO interface ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03353
    /// @needwork = dda
    /// @endcode
    uint32_t nIointerfaceId_;
    /// @brief Slot name
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03354
    /// @needwork = dda
    /// @endcode
    ara::core::StringView nSlotName_;
    /// @brief Parameterized constructor
    /// @param bExportable Whether exportable
    /// @param bSession Whether temporary session
    /// @param usage Usage
    /// @param nBitlen Length in bits
    /// @param nSlotId Key slot ID
    /// @param nIointerfaceId IO interface ID
    /// @param nSlotName Key slot name
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03355
    /// @needwork = dda
    /// @endcode
    PSecretSeed_Ipc(bool const bExportable,
                    bool const bSession,
                    Usage const usage,
                    uint32_t const nBitlen,
                    uint32_t const nSlotId,
                    uint32_t const nIointerfaceId,
                    ara::core::StringView const &nSlotName) noexcept;

public:
    /// @brief Unique smart pointer for the constant interface.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01303
    /// @trace_id_dd=DD_CRYPTO_06325
    /// @needwork = dd
    /// @endcode
    using Uptrc = std::unique_ptr< PSecretSeed_Ipc const >;
    /// @brief Unique smart pointer for the interface.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01303
    /// @trace_id_dd=DD_CRYPTO_06326
    /// @needwork = dd
    /// @endcode
    using Uptr = std::unique_ptr< PSecretSeed_Ipc >;

public:
    /// @brief Parameterized constructor
    /// @param nSlotId Key slot ID
    /// @param nIointerfaceId IO interface ID
    /// @param nSlotName Key slot name
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03356
    /// @needwork = dda
    /// @endcode
    explicit PSecretSeed_Ipc(uint32_t const nSlotId,
                             uint32_t const nIointerfaceId,
                             ara::core::StringView const &nSlotName) noexcept;
    /// @brief default virtual destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03357
    /// @needwork = dda
    /// @endcode
    ~PSecretSeed_Ipc() noexcept override = default;
    /// @brief default copy assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03358
    /// @needwork = dda
    /// @endcode
    PSecretSeed_Ipc &operator=(PSecretSeed_Ipc const &other) noexcept = delete;
    /// @brief default move assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03359
    /// @needwork = dda
    /// @endcode
    PSecretSeed_Ipc &operator=(PSecretSeed_Ipc &&other) noexcept = delete;
    /// @brief default copy constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03360
    /// @needwork = dda
    /// @endcode
    PSecretSeed_Ipc(PSecretSeed_Ipc const &other) noexcept = delete;
    /// @brief default move constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03361
    /// @needwork = dda
    /// @endcode
    PSecretSeed_Ipc(PSecretSeed_Ipc &&other) noexcept = delete;

public:
    /// @brief Clone this secret.
    /// Seed object to a new session object. The created object instance is session and non-exportable, and the "AllowedUsageFlags" property of the "cloned" object is the same as that property of the source object!
    ///         If the size of the xorDelta parameter is smaller than the value size of this seed, only the corresponding leading bytes of the original seed shall be XOR-ed, but the rest shall be copied unchanged.
    ///         If the size of the xorDelta parameter is larger than the value size of this seed, the extra bytes of xorDelta shall be ignored.
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
    /// @trace_id_ad=AD_CRYPTO_01304
    /// @trace_id_dd=DD_CRYPTO_03362
    /// @needwork = ad
    /// @endcode
    ara::core::Result< SecretSeed::Uptr > Clone(ReadOnlyMemRegion const &xorDelta
                                                = ReadOnlyMemRegion()) const noexcept override;
    /// @brief Set the value of this seed object to "jump" from the initial state to the specified number of steps, according to the "counting" expression defined by the cryptographic algorithm associated with this object.
    ///         Steps can be positive or negative values, corresponding to forward and backward directions of the "jump", but a value of 0 means simply copy the value to this seed object from the from parameter. The seed size of the from parameter must be greater than or equal to this seed size.
    /// @brief Set value of this seed object as a "jump" from an initial state to specified number of steps,
    ///           according to "counting" expression defined by a cryptographic algorithm associated with this object.
    ///       @c steps may have positive and negative values that correspond to forward and backward direction of the
    ///       "jump" respectively, but 0 value means only copy @c from value to this seed object.
    ///       Seed size of the @c from argument always must be greater or equal of this seed size.
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
    /// @trace_id_ad=AD_CRYPTO_01305
    /// @trace_id_dd=DD_CRYPTO_03363
    /// @needwork = ad
    /// @endcode
    ara::core::Result< void > JumpFrom(SecretSeed const &from, std::int64_t steps) noexcept override;
    /// @brief Set the value of this seed object to "jump" from its current state to the specified number of steps, according to the "counting" expression defined by the cryptographic algorithm associated with this object.
    ///         Steps can be positive or negative values, corresponding to forward and backward directions of the "jump", but a value of 0 means the current seed value does not change.
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
    /// @trace_id_ad=AD_CRYPTO_01306
    /// @trace_id_dd=DD_CRYPTO_03364
    /// @needwork = ad
    /// @endcode
    SecretSeed &Jump(std::int64_t steps) noexcept override;
    /// @brief Set the next value of the secret seed according to the "counting" expression defined by the cryptographic algorithm associated with this object.
    /// If the associated cryptographic algorithm does not specify a "counting" expression, then a generic increment operation must be used as the default implementation (little-endian representation, i.e., the first byte is the least significant).
    /// @brief Set next value of the secret seed according to "counting" expression defined by a cryptographic algorithm
    /// associated with this object.
    ///       If the associated cryptographic algorithm doesn't specify a "counting" expression then generic increment
    ///       operation must be implemented as default (little-endian notation, i.e. first byte is least significant).
    /// @returns reference to this updated object
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_23013}
    /// @uptrace={RS_CRYPTO_02007}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01307
    /// @trace_id_dd=DD_CRYPTO_03365
    /// @needwork = ad
    /// @endcode
    SecretSeed &Next() noexcept override;
    /// @brief XOR this seed object with another seed object and save the result to this object.
    ///         If the seed sizes of this object and the source parameter are different, only the corresponding leading bytes in this seed object should be updated.
    /// @brief XOR value of this seed object with another one and save result to this object.
    ///       If seed sizes in this object and in the @c source argument are different then only correspondent number
    ///       of leading bytes in this seed object should be updated.
    /// @param source Another seed object
    /// @return reference to this updated object
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_23015}
    /// @uptrace={RS_CRYPTO_02007}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01308
    /// @trace_id_dd=DD_CRYPTO_03366
    /// @needwork = ad
    /// @qac Suspected unable to modify [2074]: Assignment operator 'XXXX &operator ^=(XXXX) &' does not return an lvalue reference to the class in which it is defined.
    /// @endcode
    // PRQA S 2074 QAC /// @qac: AUTOSAR standard interface
    SecretSeed &operator^=(SecretSeed const &source) &noexcept override;
    // PRQA L:QAC
    /// @brief XOR the provided memory region with the seed object and save the result to this object.
    //          If the seed sizes of this object and the source parameter are different, the leading bytes of this seed object should be updated.
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
    /// @trace_id_ad=AD_CRYPTO_01309
    /// @trace_id_dd=DD_CRYPTO_03367
    /// @needwork = ad
    /// @qac Suspected unable to modify [2074]: Assignment operator 'XXXX &operator ^=(XXXX) &' does not return an lvalue reference to the class in which it is defined.
    /// @endcode
    // PRQA S 2074 QAC /// @qac: AUTOSAR standard interface
    SecretSeed &operator^=(ReadOnlyMemRegion const &source) &noexcept override;
    // PRQA L:QAC

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
    /// @trace_id_ad=AD_CRYPTO_01310
    /// @trace_id_dd=DD_CRYPTO_03368
    /// @needwork = ad
    /// @endcode
    inline Usage GetAllowedUsage() const noexcept override { return usage_; }

public:  // CryptoObject interface
    /// @brief Returns the CryptoPrimitivId of this crypto object.
    /// @return CryptoPrimitivId instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03369
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::Uptr GetCryptoPrimitiveId() const noexcept override;
    /// @brief Returns the COIdentifier of the object, which includes the object type and UID.
    /// @returns the object's COIdentifier including the object's type and COUID (or an empty COUID, if this object is
    ///     not identifiable).
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_20514}
    /// @uptrace={RS_CRYPTO_02005}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    ///         Objects without an assigned COUID cannot be (safely) serialized/exported or saved to non-volatile storage. Several related objects of different types can share a COUID (e.g., private and public keys), but the combination of COUID and object type must always be unique!
    ///         /// @brief Return the object's COIdentifier, which includes the object's type and UID. An object that
    ///         has no assigned @a COUID cannot be (securely) serialized / exported or saved to a non-volatile storage.
    ///         An object should not have a @a COUID if it is session and non-exportable simultaneously A few related
    ///         objects of different types can share a single @a COUID (e.g. private and public keys), but a combination
    ///         of @a COUID and object type must be unique always!
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01311
    /// @trace_id_dd=DD_CRYPTO_03370
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
    /// @trace_id_ad=AD_CRYPTO_01312
    /// @trace_id_dd=DD_CRYPTO_03371
    /// @needwork = ad
    /// @endcode
    std::size_t GetPayloadSize() const noexcept override;
    /// @brief Returns the COIdentifier of the crypto object that this crypto object depends on.
    ///         For a signature object, this method must return a reference to the corresponding signature verification public key!
    ///         Uniquely identifying a crypto object requires two components: cryptoobjectuid and cryptoobjecttype.
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
    /// @trace_id_ad=AD_CRYPTO_01313
    /// @trace_id_dd=DD_CRYPTO_03372
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
    /// @trace_id_ad=AD_CRYPTO_01314
    /// @trace_id_dd=DD_CRYPTO_03373
    /// @needwork = ad
    /// @endcode
    inline bool IsExportable() const noexcept override { return bExportable_; }
    /// @brief Returns the "session" (or "temporary") attribute of the object.
    ///     A temporary object cannot be saved to a persistent storage location pointed to by IOInterface! A temporary object will be safely destroyed together with this interface instance!
    ///     Non-session objects must have an assigned COUID (see GetObjectId()).
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
    /// @trace_id_ad=AD_CRYPTO_01315
    /// @trace_id_dd=DD_CRYPTO_03374
    /// @needwork = ad
    /// @endcode
    inline bool IsSession() const noexcept override { return bSession_; }
    /// @brief Save itself to the provided IOInterface. Crypto objects with the "session" attribute cannot be saved in KeySlot.
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
    /// @trace_id_ad=AD_CRYPTO_01316
    /// @trace_id_dd=DD_CRYPTO_03375
    /// @needwork = ad
    /// @endcode
    ara::core::Result< void > Save(IOInterface &container) const noexcept override;
    /// @brief Set allowed usage.
    /// @param usage Usage
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03376
    /// @needwork = dda
    /// @endcode
    inline void SetAllowedUsage(Usage const usage) noexcept { usage_ = usage; }
    /// @brief Set bExportable.
    /// @param bExportable Whether exportable
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03377
    /// @needwork = dda
    /// @endcode
    inline void SetExportable(bool const bExportable) noexcept { bExportable_ = bExportable; }
    /// @brief Set bSession.
    /// @param bSession Whether temporary session
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03378
    /// @needwork = dda
    /// @endcode
    inline void SetSession(bool const bSession) noexcept { bSession_ = bSession; }
    /// @brief Get the slot ID.
    /// @return Slot ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03379
    /// @needwork = dda
    /// @endcode
    inline uint32_t GetSlotId() const noexcept { return nSlotId_; }
    /// @brief Get the slot name.
    /// @return Slot name
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03380
    /// @needwork = dda
    /// @endcode
    inline ara::core::StringView GetSlotName() const noexcept { return nSlotName_; }
    // @brief Get the interface ID.
    /// @return Ininterface ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03381
    /// @needwork = dda
    /// @endcode
    inline uint32_t GetIoinerfaceId() const noexcept { return nIointerfaceId_; }

protected:
    /// @brief Used for clang-tidy hint: nBitlen_ is not used
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = no
    /// @endcode
    inline void _UseBitlen() const noexcept { std::ignore = nBitlen_; }
};
//********************************/
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_CRYP_PUHUA_SECRET_SEED_IPC_H_
