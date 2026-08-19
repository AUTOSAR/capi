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
/// @file       isoft_secret_seed.cpp
/// @brief      AutoSar-Crypto Encryption/Decryption module
/// @details
/// @date       2022-05-24
/// @author     Chang Zheng
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Default encryption decryption/Secret seed
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_06005
/// @unit_name=PSecretSeed
/// @unit_description=Secret seed
/// @endcode
///
/// ================================================================

#include "ara/crypto/cryp/cryobj/isoft_secret_seed.h"

#include "ara/crypto/common/isoft_common_api.h"
#include "ara/crypto/common/isoft_io_interface.h"
#include "ara/crypto/cryp/cryobj/isoft_crypto_primitive_id.h"
#include "ara/crypto/cryp/isoft_crypto_provider.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {

/// @brief Parameterized constructor
/// @param pKeyData Key data
/// @param nKeyLen Key length: in bytes
/// @returns
PSecretSeed::PSecretSeed(void const* const pKeyData, uint32_t const nKeyLen) noexcept
    : PSecretSeed{nullptr, false, false, kAllowPrototypedOnly, pKeyData, nKeyLen, {}}
{
}
/// @brief Parameterized constructor
/// @param dataBuff Data buffer
/// @param bExportable Whether export is allowed
/// @param bSession Whether it is a temporary session
/// @param usage Scope of use
/// @param pKeyData Key data
/// @param nKeyLen Key length: in bytes
/// @param keyId IO interface ID corresponding to the key material
PSecretSeed::PSecretSeed(internal::PAutoBuff const* const dataBuff,
                         bool const bExportable,
                         bool const bSession,
                         Usage const usage,
                         void const* const pKeyData,
                         uint32_t const nKeyLen,
                         COIdentifier const& keyId) noexcept
    : SecretSeed{}  // NOLINT
    , dataBuff_{}   // NOLINT
    , bExportable_{bExportable}
    , bSession_{bSession}
    , usage_{usage}
    , nKeyLen_{nKeyLen}
    , keyId_{keyId}
{
    std::ignore = dataBuff;
    if ((pKeyData != nullptr) && (nKeyLen > 0U)) {
        bool const ret{dataBuff_.SetData(static_cast< uint8_t const* >(static_cast< void const* >(pKeyData)), nKeyLen)};
        std::ignore = ret;
    }
    keyId_.mCOType              = CryptoObjectType::kSecretSeed;
    keyId_.mCouid.mGeneratorUid = GetUuidByTime();
    keyId_.mCouid.mVersionStamp = 0U;
}
/// @brief Default copy assignment function
/// @param other Another object instance of this class
/// @returns  *this
PSecretSeed& PSecretSeed::operator=(PSecretSeed const& other) noexcept
{
    if (this != &other) {
        bExportable_ = other.bExportable_;
        bSession_    = other.bSession_;
        bool const ret{dataBuff_.SetData(other.dataBuff_.Data(), other.dataBuff_.size())};
        std::ignore = ret;
        nKeyLen_    = other.nKeyLen_;
    }
    return *this;
}
/// @brief Default move assignment function
/// @param other Another object instance of this class
/// @returns  *this
PSecretSeed& PSecretSeed::operator=(PSecretSeed&& other) noexcept
{
    bExportable_ = other.bExportable_;
    bSession_    = other.bSession_;
    nKeyLen_     = other.nKeyLen_;
    bool const ret{dataBuff_.SetData(other.dataBuff_.Data(), other.dataBuff_.size())};
    std::ignore = ret;
    other.dataBuff_.ResetData();
    std::ignore = std::move(other);
    return *this;
}

/// @brief Clone this Secret
/// Seed object to a new session object. The created object instance is session-only and non-exportable. The AllowedUsageFlags property of the "cloned" object is the same as that of the source object!
///         If the size of the xorDelta parameter is smaller than the value size of this seed, then only the corresponding leading bytes of the original seed shall be XOR-ed, but the rest shall be copied without change.
///         If the size of the xorDelta parameter is larger than the value size of this seed, the extra bytes of xorDelta shall be ignored.
/// @brief Clone this Secret Seed object to new session object.
///         Created object instance is session and non-exportable, @c AllowedUsageFlags attribute of the "cloned" object
///         is identical to this attribute of the source object! If size of the @c xorDelta argument is less than the
///         value size of this seed then only correspondent number of leading bytes of the original seed should be
///         XOR-ed, but the rest should be copied without change. If size of the @c xorDelta argument is larger than the
///         value size of this seed then extra bytes of the @c xorDelta should be ignored.
/// @param xorDelta  optional "delta" value that must be XOR-ed with the "cloned" copy of the original seed
/// @returns unique smart pointer to "cloned" session @c SecretSeed object
/// @returns  ara::core::Result<SecretSeed::Uptr>
ara::core::Result< SecretSeed::Uptr > PSecretSeed::Clone(ReadOnlyMemRegion const& xorDelta) const noexcept
{
    uint32_t const len{static_cast< uint32_t >(dataBuff_.size())};

    uint32_t const cloneLen{static_cast< uint32_t >(xorDelta.size())};

    uint8_t const* const pData{xorDelta.data()};

    std::unique_ptr< uint8_t[] > const memData{std::make_unique< uint8_t[] >(static_cast< size_t >(len))};

    for (uint32_t i{0U}; i < len; i++) {
        if (i < cloneLen) {
            memData[static_cast< size_t >(i)] = dataBuff_.at(i) ^ (*(pData + i));
        } else {
            memData[static_cast< size_t >(i)] = dataBuff_.at(static_cast< uint32_t >(i));
        }
    }
    PSecretSeed::Uptr pSecretSeed{std::make_unique< PSecretSeed >(memData.get(), len)};
    return ara::core::Result< SecretSeed::Uptr >::FromValue(std::move(pSecretSeed));
}

/// @brief Set the value of this seed object to "jump" from the initial state to the specified number of steps, according to the "count" expression defined by the encryption algorithm associated with this object.
///         Steps can be positive or negative, corresponding to forward and backward directions of the "jump", but a value of 0 means only copying the value to this seed object. The seed size of the from parameter must be greater than or equal to the size of this seed.
/// @brief Set value of this seed object as a "jump" from an initial state to specified number of steps,
///           according to "counting" expression defined by a cryptographic algorithm associated with this object.
///       @c steps may have positive and negative values that correspond to forward and backward direction of the
///       "jump" respectively, but 0 value means only copy @c from value to this seed object.
///       Seed size of the @c from argument always must be greater or equal of this seed size.
/// @param from  source object that keeps the initial value for jumping from
/// @param steps  number of steps for the "jump"
/// @returns reference to this updated object
/// @error: SecurityErrorDomain::kIncompatibleObject  if this object and the @c from argument are associated
///     with incompatible cryptographic algorithms
/// @error: SecurityErrorDomain::kInvalidInputSize  if value size of the @c from seed is less then
///     value size of this one
ara::core::Result< void > PSecretSeed::JumpFrom(SecretSeed const& from, std::int64_t steps) noexcept
{
    PSecretSeed const* const fromSecretSeed{dynamic_cast< PSecretSeed const* >(&from)};
    if (fromSecretSeed == nullptr) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kIncompatibleObject);
    }

    uint8_t* const pdata{fromSecretSeed->dataBuff_.Data()};
    uint32_t const len{fromSecretSeed->dataBuff_.size()};
    if (steps > 0) {
        while (steps > 0) {
            --steps;
            Ctr_inc(pdata, len);
        }
    } else {
        int32_t n{static_cast< int32_t >(-steps)};
        while (n > 0) {
            --n;
            Ctr_dec(pdata, len);
        }
    }
    dataBuff_.ResetBuff(len);
    std::ignore = dataBuff_.AddData(pdata, len);

    return ara::core::Result< void >::FromValue();
}

/// @brief Set the value of this seed object to "jump" from its current state to the specified number of steps, according to the "count" expression defined by the encryption algorithm associated with this object.
///         Steps can be positive or negative, corresponding to forward and backward directions of the "jump", but a value of 0 means the current seed value is not changed.
/// @brief Set value of this seed object as a "jump" from it's current state to specified number of steps,
///     according to "counting" expression defined by a cryptographic algorithm associated with this object.
///     @c steps may have positive and negative values that correspond to forward and backward direction of the
///     "jump" respectively, but 0 value means no changes of the current seed value.
/// @param steps  number of "steps" for jumping (forward or backward) from the current state
/// @returns reference to this updated object
SecretSeed& PSecretSeed::Jump(std::int64_t steps) noexcept
{
    u_char* const pdata{dataBuff_.Data()};
    uint32_t const len{dataBuff_.size()};
    if (steps > 0) {
        while (steps > 0) {
            --steps;
            Ctr_inc(pdata, len);
        }
    } else {
        int32_t n{static_cast< int32_t >(-steps)};
        while (n > 0) {
            --n;
            Ctr_dec(pdata, len);
        }
    }
    return *this;
}

/// @brief Set the next value of the secret seed according to the "count" expression defined by the encryption algorithm associated with this object.
///         If the associated encryption algorithm does not specify a "count" expression, then a generic increment operation must be used as the default implementation (little-endian representation, i.e., the first byte is the least significant).
/// @brief Set next value of the secret seed according to "counting" expression defined by a cryptographic algorithm
///     associated with this object. If the associated cryptographic algorithm doesn't specify a "counting" expression
///     then generic increment operation must be implemented as default (little-endian notation, i.e. first byte is
///     least significant).
/// @returns  reference to this updated object
SecretSeed& PSecretSeed::Next() noexcept
{
    u_char* const pdata{dataBuff_.Data()};
    uint32_t const len{dataBuff_.size()};
    Ctr_inc(pdata, len);
    return *this;
}

/// @brief XOR this seed object with another seed object and save the result to this object.
///         If the seed sizes of this object and the source parameter are different, then only the corresponding leading bytes in this seed object shall be updated.
/// @brief XOR value of this seed object with another one and save result to this object.
///       If seed sizes in this object and in the @c source argument are different then only correspondent number
///       of leading bytes in this seed object should be updated.
/// @param source  right argument for the XOR operation
/// @returns reference to this updated object
SecretSeed& PSecretSeed::operator^=(SecretSeed const& source) & noexcept
{
    int32_t const len{static_cast< int32_t >(dataBuff_.size())};

    PSecretSeed const* const sourceSecretSeed{dynamic_cast< PSecretSeed const* >(&source)};

    if (sourceSecretSeed == nullptr) {
        return *this;
    }

    int32_t const sourceLen{static_cast< int32_t >(sourceSecretSeed->dataBuff_.size())};

    uint8_t* const pDataDst{dataBuff_.Data()};
    uint8_t const* const pDataSrc{sourceSecretSeed->dataBuff_.Data()};

    for (int32_t i{0}; i < len; ++i) {
        if (i < sourceLen) {
            *(pDataDst + i) ^= *(pDataSrc + i);
        }
    }
    return *this;
}

/// @brief XOR the seed object with the provided memory region and save the result to this object.
//          If the seed sizes of this object and the source parameter are different, then the leading bytes of this seed object shall be updated.
/// @brief XOR value of this seed object with provided memory region and save result to this object.
///       If seed sizes in this object and in the @c source argument are different then only correspondent number
///       of leading bytes of this seed object should be updated.
/// @param source  right argument for the XOR operation
/// @returns reference to this updated object
SecretSeed& PSecretSeed::operator^=(ReadOnlyMemRegion const& source) & noexcept
{
    int32_t const len{static_cast< int32_t >(dataBuff_.size())};

    int32_t const sourceLen{static_cast< int32_t >(source.size())};
    uint8_t* const pDataDst{dataBuff_.Data()};
    uint8_t const* const pDataSrc{source.data()};

    for (int32_t i{0}; i < len; ++i) {
        if (i < sourceLen) {
            *(pDataDst + i) ^= *(pDataSrc + i);
        }
    }
    return *this;
}
//***************/  //CryptoObject interface
/// @brief Returns the CryptoPrimitiveId of this cryptobject.
/// @returns CryptoPrimitiveId instance
CryptoPrimitiveId::Uptr PSecretSeed::GetCryptoPrimitiveId() const noexcept
{
    return {std::make_unique< PAlgId_SecretseedObj >()};
}
/// @brief Returns the COIdentifier of the object, which includes the object type and UID.
/// Objects without an assigned COUID cannot be (safely) serialized/exported or saved to non-volatile storage.
/// Several related objects of different types can share a COUID (e.g., private key and public key), but the combination of COUID and object type must always be unique!
/// @returns   the object's COIdentifier including the object's type and COUID (or an empty COUID, if this object is
///     not identifiable).
CryptoObject::COIdentifier PSecretSeed::GetObjectId() const noexcept
{
    if (bSession_ && !bExportable_) {
        COIdentifier keyId;
        keyId.mCOType = CryptoObjectType::kUndefined;
        keyId.mCouid  = CryptoObjectUid();
        return keyId;
    }
    return keyId_;
}
/// @brief Returns the actual size of the object payload. Unit: bytes.
/// @brief Return actual size of the object’s payload.
///         Returned value always must be less than or equal to the maximum payload size expected for this primitive and
///         object type, it is available via call: My Provider().GetPayloadStorageSize(GetObjectType(),
///         GetPrimitiveId()).Value(); Returned value does not take into account the object’s meta-information
///         properties, but their size is fixed and common for all crypto objects independently from their actual type.
///         During an allocation of a @c TrustedContainer, Crypto Providers (and Key Storage Providers) reserve space
///         for an object’s meta-information automatically, according to their implementation details.
/// @returns size in bytes of the object's payload required for its storage
std::size_t PSecretSeed::GetPayloadSize() const noexcept { return static_cast< std::size_t >(dataBuff_.size()); }
/// @brief Returns the COIdentifier of the cryptobject that this cryptobject depends on.
/// For a signature object, this method must return a reference to the corresponding signature verification public key!
/// Uniquely identifying a cryptobject requires two components: cryptobject UID and cryptobject type.
/// @brief Return the COIdentifier of the CryptoObject that this CryptoObject depends on.
///         For signatures objects this method @b must return a reference to correspondent signature verification public
///         key! Unambiguous identification of a CryptoObject requires both components: @c CryptoObjectUid and @c
///         CryptoObjectType.
/// @returns target COIdentifier of the existing dependence or @c CryptoObjectType::kUnknown and empty COUID, if the
/// current object does not depend on another CryptoObject
CryptoObject::COIdentifier PSecretSeed::HasDependence() const noexcept
{
    COIdentifier keyID;
    keyID.mCOType = CryptoObjectType::kUndefined;
    keyID.mCouid  = CryptoObjectUid();
    return keyID;
}
/// @brief Saves itself to the provided IOInterface. Cryptobjects with the "session" attribute cannot be saved in a KeySlot.
/// @brief Save itself to provided IOInterface
///     A CryptoObject with property "session" cannot be saved in a KeySlot.
/// @param container IOInterface representing underlying storage
/// @returns  ara::core::Result<void>
/// @error: SecurityErrorDomain::kIncompatibleObject     if the object is "session", but the IOInterface represents a
///     KeySlot.
/// @error: SecurityErrorDomain::kContentRestrictions    if the object doesn't satisfy the slot restrictions
/// @error: SecurityErrorDomain::kInsufficientCapacity   if the capacity of the target container is not enough, i.e. if
///     <tt>(container.Capacity() < this->StorageSize())</tt>
/// @error: SecurityErrorDomain::kModifiedResource       if the underlying resource has been modified after  the
///     IOInterface has been opened, i.e., the IOInterface has been invalidated.
/// @error: SecurityErrorDomain::kUnreservedResource     if the IOInterface is not opened writeable.
ara::core::Result< void > PSecretSeed::Save(IOInterface& container) const noexcept
{
    /// @error: SecurityErrorDomain::kIncompatibleObject     if the object is "session", but the IOInterface represents
    /// a KeySlot.
    if (IsSession()) {
        if (false == container.IsVolatile()) {
            return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kIncompatibleObject);
        }
    }
    /// @error: SecurityErrorDomain::kInsufficientCapacity   if the capacity of the target container is not enough, i.e.
    /// if <tt>(container.Capacity() < this->StorageSize())</tt>
    if (dataBuff_.GetBuffLen() > container.GetCapacity()) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kInsufficientCapacity);
    }
    /// @error: SecurityErrorDomain::kModifiedResource       if the underlying resource has been modified after  the
    /// IOInterface has been opened, i.e., the IOInterface has been invalidated.
    if (false == container.IsValid()) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kModifiedResource);
    }
    /// @error: SecurityErrorDomain::kUnreservedResource     if the IOInterface is not opened writeable.
    if (false == container.IsWritable()) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kUnreservedResource);
    }
    /// @error SecurityErrorDomain::kContentRestrictions    if the object doesn't satisfy the slot restrictions (@see
    if (CryptoObjectType::kSecretSeed != container.GetTypeRestriction()) {
        if (container.GetTypeRestriction() != CryptoObjectType::kUndefined) {
            return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kContentRestrictions);
        }
    }
    /// @error: SecurityErrorDomain::kContentRestrictions    if the object doesn't satisfy the slot restrictions
    if (false == _SaveToKeySlot(container)) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kContentRestrictions);
    }
    return ara::core::Result< void >::FromValue();
}
/// @brief Gets the secret seed data.
/// @returns Pointer to the secret seed object.
uint8_t const* PSecretSeed::GetSecretSeedData() const noexcept { return dataBuff_.Data(); }
/// @brief Sets the secret seed data.
/// @param pSecretSeedData Secret seed data.
/// @param nSecretSeedLength Length of the secret seed data.
void PSecretSeed::SetSecretSeedData(void const* const pSecretSeedData, uint32_t const nSecretSeedLength) noexcept
{
    dataBuff_.ResetBuff(nSecretSeedLength);
    bool const ret{dataBuff_.SetData(static_cast< uint8_t const* >(static_cast< void const* >(pSecretSeedData)),
                                     nSecretSeedLength)};
    std::ignore = ret;
}

/// @brief Randomly initializes the key.
/// @param nKeyBitLength Key length: in bits.
/// @returns ture if init key sucess false otherwise
bool PSecretSeed::RandomInitKey(int32_t const nKeyBitLength) noexcept
{
    PCryptoProvider provider;
    ara::core::Result< RandomGeneratorCtx::Uptr > resultRng{provider.CreateRandomGeneratorCtx(0U, true)};
    if (false == resultRng.HasValue()) {
        return false;
    }
    RandomGeneratorCtx::Uptr const pCtxRng{std::move(std::move(resultRng).Value())};
    uint32_t const calBit{~7U};
    uint32_t const len{(static_cast< uint32_t >(nKeyBitLength) / kInt_8U + 7U) & calBit};
    ara::core::Result< ara::core::Vector< ara::core::Byte > > const result{
        pCtxRng->Generate(static_cast< uint32_t >(len))};
    if (false == result.HasValue()) {
        return false;
    }
    ara::core::Vector< ara::core::Byte > const& vecRandom{result.Value()};
    std::size_t const nResLen{vecRandom.size()};
    dataBuff_.ResetBuff(static_cast< uint32_t >(nResLen));
    bool const ret{dataBuff_.SetData(static_cast< uint8_t const* >(static_cast< void const* >(vecRandom.data())),
                                     static_cast< uint32_t >(nResLen))};
    std::ignore = ret;
    return true;
}
/// @brief Gets the data.
/// @return Pointer to the secret seed object.
uint8_t const* PSecretSeed::GetData() const noexcept { return dataBuff_.Data(); }
/// @brief Gets the data length.
/// @return Secret seed length.
uint32_t PSecretSeed::GetDataLen() const noexcept { return dataBuff_.size(); }
/// @brief: Saves the key.
/// @param container IO interface.
/// @returns  true if save to key slot sucess false otherwise
bool PSecretSeed::_SaveToKeySlot(IOInterface& container) const noexcept
{
    keys::isoft_def::PIoInterface* const pIoInterface{dynamic_cast< keys::isoft_def::PIoInterface* >(&container)};
    if (nullptr == pIoInterface) {
        return false;
    }
    ReadOnlyMemRegion const memKeyData{T_TransBytes(GetData()), static_cast< size_t >(GetDataLen())};
    return pIoInterface->SaveKeyToSlot< PSecretSeed >(this, memKeyData);
}

//********************************/
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara
