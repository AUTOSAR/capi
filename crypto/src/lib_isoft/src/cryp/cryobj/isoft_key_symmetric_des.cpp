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
/// @file       isoft_key_symmetric_des.cpp
/// @brief      AutoSar-Crypto Encryption/Decryption module
/// @details
/// @date       2021-12-21
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Default encryption decryption/Symmetric key
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_06005
/// @unit_name=PKeySymmetric_Des
/// @unit_description=DES symmetric key
/// @endcode
///
/// ================================================================

#include "ara/crypto/cryp/cryobj/isoft_key_symmetric_des.h"

#include "ara/crypto/common/isoft_assert.h"
#include "ara/crypto/common/isoft_data_type.h"
#include "ara/crypto/common/security_error_domain.h"
#include "ara/crypto/cryp/cryobj/isoft_crypto_primitive_id.h"
#include "ara/crypto/cryp/isoft_crypto_provider.h"
#include "ara/crypto/cryp/rng/isoft_ctx_rng_default.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
//********************************/
/// @brief Constructor
PKeySymmetric_Des::PKeySymmetric_Des() noexcept
    : PKeySymmetric_Des{nullptr, static_cast< uint32_t >(PAlgId_Symmetric_DesKey::EKeyLen::kDesKeyMinBitLength)}
{
}
/// @brief Parameterized constructor
/// @param nKeyBitLen Key length: in bits
PKeySymmetric_Des::PKeySymmetric_Des(uint32_t const nKeyBitLen) noexcept : PKeySymmetric_Des{nullptr, nKeyBitLen} {}
/// @brief Parameterized constructor
/// @param pKeyData Key data
/// @param nKeyByteLen Key length: in bytes
PKeySymmetric_Des::PKeySymmetric_Des(const void *const pKeyData, uint32_t const nKeyByteLen) noexcept
    : PKeySymmetric_Base{}  // NOLINT
    , keyData_{}            // NOLINT
    , nKeyBitLen_{nKeyByteLen * kInt_8U}
{
    PH_ASSERT(0U == (nKeyBitLen_ % static_cast< uint32_t >(PAlgId_Symmetric_DesKey::EKeyLen::kDesKeyMinBitLength)));
    if ((pKeyData != nullptr)
        && (nKeyBitLen_ >= static_cast< uint32_t >(PAlgId_Symmetric_DesKey::EKeyLen::kDesKeyMinBitLength))
        && (nKeyBitLen_ <= static_cast< uint32_t >(PAlgId_Symmetric_DesKey::EKeyLen::kDesKeyMaxBitLength))) {
        std::ignore = memcpy(static_cast< void * >(keyData_), pKeyData, static_cast< std::size_t >(nKeyByteLen));
    }
}
/// @brief Return the CryptoPrimitiveId of this crypto object.
/// @name  GetCryptoPrimitiveId
/// @returns  Instance of CryptoPrimitiveId
CryptoPrimitiveId::Uptr PKeySymmetric_Des::GetCryptoPrimitiveId() const noexcept
{
    uint32_t const nKeyLevel{static_cast< uint32_t >(PAlgId_Symmetric_DesKey::EKeyLen::kDesKeyMinBitLength)};
    if (nKeyBitLen_ > nKeyLevel) {
        return {std::make_unique< PAlgId_Symmetric_3DesKey >()};
    }
    return {std::make_unique< PAlgId_Symmetric_DesKey >()};
}
/// @brief Return the actual size of the object payload.
///         The return value does not consider the meta-information properties of the object, but their size is fixed and common to all crypto objects independent of their actual type. During the allocation of a TrustedContainer,
///         the crypto provider (and key storage provider) automatically reserves space for the object's meta-information according to its implementation details.
/// @name  GetPayloadSize
/// @returns Actual size of the payload
std::size_t PKeySymmetric_Des::GetPayloadSize() const noexcept { return static_cast< size_t >(nKeyBitLen_) / kInt_8U; }
/// @brief Save itself to the provided IOInterface. A crypto object with the attribute "session" cannot be saved in a KeySlot.
/// @name  Save
/// @param container IO interface
/// @returns  has value if save sucess false otherwise
ara::core::Result< void > PKeySymmetric_Des::Save(IOInterface &container) const noexcept
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
    if ((sizeof(keyData_)) > container.GetCapacity()) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kInsufficientCapacity);
    }
    // /// @error: SecurityErrorDomain::kModifiedResource       if the underlying resource has been modified after  the
    // IOInterface has been opened, i.e., the IOInterface has been invalidated.
    if (false == container.IsValid()) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kModifiedResource);
    }
    /// @error: SecurityErrorDomain::kUnreservedResource     if the IOInterface is not opened writeable.
    if (false == container.IsWritable()) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kUnreservedResource);
    }
    /// @error SecurityErrorDomain::kContentRestrictions    if the object doesn't satisfy the slot restrictions (@see
    if (CryptoObjectType::kSymmetricKey != container.GetTypeRestriction()) {
        if (container.GetTypeRestriction() != CryptoObjectType::kUndefined) {
            return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kContentRestrictions);
        }
    }
    /// @error: SecurityErrorDomain::kContentRestrictions    if the object doesn't satisfy the slot restrictions (@see
    if (false == _SaveToKeySlot(container)) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kContentRestrictions);
    }
    return ara::core::Result< void >::FromValue();
}
//***************/
/// @brief Randomly initialize the key
/// @name  RandomInitKey
/// @param nKeyBitLength Key length: in bits
/// @returns true if init key sucess false otherwise
bool PKeySymmetric_Des::RandomInitKey(int32_t const nKeyBitLength) noexcept
{
    PCryptoProvider provider;
    ara::core::Result< RandomGeneratorCtx::Uptr > resultRng{provider.CreateRandomGeneratorCtx(0U, true)};
    if (false == resultRng.HasValue()) {
        return false;
    }
    RandomGeneratorCtx::Uptr const pCtxRng{std::move(std::move(resultRng).Value())};
    // int32_t const len{(nKeyBitLength / 8 + 7) & ~7};
    uint32_t const len{(static_cast< uint32_t >(nKeyBitLength) / kInt_8U + 7U) & static_cast< uint32_t >(~7U)};

    ara::core::Result< ara::core::Vector< ara::core::Byte > > const result{pCtxRng->Generate(len)};
    if (false == result.HasValue()) {
        return false;
    }

    ara::core::Vector< ara::core::Byte > const &vecRandom{result.Value()};
    uint32_t nCopyLen{
        std::min< uint32_t >(static_cast< uint32_t >(PAlgId_Symmetric_DesKey::EKeyLen::kDesKeyMaxBitLength) / kInt_8U,
                             static_cast< uint32_t >(vecRandom.size()))};
    nCopyLen = std::max< uint32_t >(
        static_cast< uint32_t >(PAlgId_Symmetric_DesKey::EKeyLen::kDesKeyMinBitLength) / kInt_8U, nCopyLen);
    std::ignore = memset(static_cast< void * >(keyData_), 0, sizeof(keyData_));
    for (uint32_t i{0U}; i < nCopyLen; ++i) {
        keyData_[i] = static_cast< uint8_t >(vecRandom.at(static_cast< size_t >(i)));  // NOLINT
    }
    uint32_t const nStepLen{static_cast< uint32_t >(PAlgId_Symmetric_DesKey::EKeyLen::kDesBlockSize) - 1U};
    nKeyBitLen_ = ((nCopyLen + nStepLen) & (~nStepLen)) * kInt_8U;
    return true;
}
/// @brief Initialize a DES key
/// @name  DebugInitKey
/// @param stKey Key data: in string format
/// @returns  void
void PKeySymmetric_Des::DebugInitKey(ara::core::StringView const &stKey) noexcept
{
    uint32_t nCopyLen{
        std::min< uint32_t >(static_cast< uint32_t >(PAlgId_Symmetric_DesKey::EKeyLen::kDesKeyMaxBitLength) / kInt_8U,
                             static_cast< uint32_t >(stKey.size()))};
    nCopyLen = std::max< uint32_t >(
        static_cast< uint32_t >(PAlgId_Symmetric_DesKey::EKeyLen::kDesKeyMinBitLength) / kInt_8U, nCopyLen);
    std::ignore = memset(static_cast< void * >(keyData_), 0, sizeof(keyData_));
    for (uint32_t i{0U}; i < nCopyLen; ++i) {
        keyData_[i] = static_cast< uint8_t >(stKey[static_cast< size_t >(i)]);  // NOLINT
    }
    uint32_t const nStepLen{static_cast< uint32_t >(PAlgId_Symmetric_DesKey::EKeyLen::kDesBlockSize) - 1U};
    nKeyBitLen_ = ((nCopyLen + nStepLen) & (~nStepLen)) * kInt_8U;
    SetAllowedUsage(kAllowDataDecryption | kAllowVerification);
}
//********************************/
/// @brief Constructor
PKeySymmetric_3Des::PKeySymmetric_3Des() noexcept
    : PKeySymmetric_Des{static_cast< uint32_t >(PAlgId_Symmetric_DesKey::EKeyLen::kDesKeyMaxBitLength)}
{
}
/// @brief Parameterized constructor
/// @param pKeyData Key data
/// @param nDataLen Data length
PKeySymmetric_3Des::PKeySymmetric_3Des(void const *const pKeyData, uint32_t const nDataLen) noexcept
    : PKeySymmetric_Des{pKeyData,
                        static_cast< uint32_t >(PAlgId_Symmetric_DesKey::EKeyLen::kDesKeyMaxBitLength) / kInt_8U}
{
    std::ignore = nDataLen;
}
/// @brief Return the CryptoPrimitiveId of this crypto object.
/// @return Instance of CryptoPrimitiveId
CryptoPrimitiveId::Uptr PKeySymmetric_3Des::GetCryptoPrimitiveId() const noexcept
{
    return {std::make_unique< PAlgId_Symmetric_3DesKey >()};
}
//********************************/
/// @brief Constructor
PSymmetricKey_2Des::PSymmetricKey_2Des() noexcept
    : PKeySymmetric_Des{static_cast< uint32_t >(PAlgId_Symmetric_DesKey::EKeyLen::kDesKeyMinBitLength) * kInt_2U}
{
}
/// @brief Parameterized constructor
/// @param pKeyData Key data
/// @param nDataLen Data length
PSymmetricKey_2Des::PSymmetricKey_2Des(void const *const pKeyData, uint32_t const nDataLen) noexcept
    : PKeySymmetric_Des{
        pKeyData, static_cast< uint32_t >(PAlgId_Symmetric_DesKey::EKeyLen::kDesKeyMinBitLength) * kInt_2U / kInt_8U}
{
    std::ignore = nDataLen;
}
/// @brief Return the CryptoPrimitiveId of this crypto object.
/// @return Instance of CryptoPrimitiveId
CryptoPrimitiveId::Uptr PSymmetricKey_2Des::GetCryptoPrimitiveId() const noexcept
{
    return {std::make_unique< PAlgId_Symmetric_2DesKey >()};
}
//********************************/
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara
