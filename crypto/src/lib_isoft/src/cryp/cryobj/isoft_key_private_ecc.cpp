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
/// @file       isoft_key_private_ecc.cpp
/// @brief      AutoSar-Crypto Encryption/Decryption module
/// @details
/// @date       2021-08-02
/// @author     Chang Zheng
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Default encryption decryption/Asymmetric private key
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_06005
/// @unit_name=PKeyPrivate_Ecc
/// @unit_description=ECC private key
/// @endcode
///
/// ================================================================

#include "ara/crypto/cryp/cryobj/isoft_key_private_ecc.h"

#include "ara/crypto/common/isoft_common_api.h"
#include "ara/crypto/common/isoft_io_interface.h"
#include "ara/crypto/common/serializable.h"
#include "ara/crypto/cryp/cryobj/isoft_crypto_primitive_id.h"
#include "ara/crypto/cryp/cryobj/isoft_key_public_ecc.h"
#include "ara/crypto/cryp/isoft_auto_buff.h"
#include "ara/crypto/openssl/isoft_openssl_encrypt.h"
namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
//********************************/
/// @brief General asymmetric private key interface.
//********************************/
/// @brief Parameterized constructor
/// @param nGenKeyBitLength Key length: in bits
/// @param nEccId ECC NID
PKeyPrivate_Ecc::PKeyPrivate_Ecc(std::size_t const nGenKeyBitLength) noexcept : PKeyPrivate_Base{}  // NOLINT
{
    pEccPair_             = Generate_ecc_key(static_cast< int32_t >(nGenKeyBitLength));
    nGenPrivateKeyBitLen_ = nGenKeyBitLength;
}
/// @brief Default virtual destructor
PKeyPrivate_Ecc::~PKeyPrivate_Ecc() noexcept { _DeleteEcc(); }
//***************/  //CryptoObject interface
/// @brief Return the CryptoPrimitiveId of this crypto object.
/// @returns  Instance of CryptoPrimitiveId
CryptoPrimitiveId::Uptr PKeyPrivate_Ecc::GetCryptoPrimitiveId() const noexcept
{
    CryptoPrimitiveId::Uptr pReturn{nullptr};
    switch (nGenPrivateKeyBitLen_) {
        case kInt_128U: {
            pReturn = std::make_unique< PAlgId_Asymmetric_EccKey128 >();
        } break;
        case kInt_192U: {
            pReturn = std::make_unique< PAlgId_Asymmetric_EccKey192 >();
        } break;
        case kInt_256U: {
            pReturn = std::make_unique< PAlgId_Asymmetric_EccKey256 >();
        } break;
        default: {
            pReturn = std::make_unique< PAlgId_Asymmetric_EccKey >();
        } break;
    }
    return pReturn;
}
/// @brief Return the actual size of the object payload, in bytes.
///         The return value does not consider the meta-information properties of the object, but their size is fixed and common to all crypto objects independent of their actual type. During the allocation of a TrustedContainer,
///         the crypto provider (and key storage provider) automatically reserves space for the object's meta-information according to its implementation details.
/// @brief Return actual size of the object’s payload.
/// Returned value always must be less than or equal to the maximum payload size expected for this primitive and
/// object type, it is available via call: My Provider().GetPayloadStorageSize(GetObjectType(),
/// GetPrimitiveId()).Value(); Returned value does not take into account the object’s meta-information
/// properties, but their size is fixed and common for all crypto objects independently from their actual type.
/// During an allocation of a @c TrustedContainer, Crypto Providers (and Key Storage Providers) reserve space
/// for an object’s meta-information automatically, according to their implementation details.
/// @returns size in bytes of the object's payload required for its storage
std::size_t PKeyPrivate_Ecc::GetPayloadSize() const noexcept
{
    return static_cast< std::size_t >(Get_EccKey_Size(pEccPair_));
}
/// @brief Save itself to the provided IOInterface. A crypto object with the attribute "session" cannot be saved in a KeySlot.
/// @brief Save itself to provided IOInterface
/// A CryptoObject with property "session" cannot be saved in a KeySlot.
/// @param container IOInterface representing underlying storage
/// @return
/// @error: SecurityErrorDomain::kIncompatibleObject     if the object is "session", but the IOInterface represents a
///     KeySlot.
/// @error: SecurityErrorDomain::kContentRestrictions    if the object doesn't satisfy the slot restrictions
/// @error: SecurityErrorDomain::kInsufficientCapacity   if the capacity of the target container is not enough, i.e. if
///     <tt>(container.Capacity() < this->StorageSize())</tt>
/// @error: SecurityErrorDomain::kModifiedResource       if the underlying resource has been modified after  the
///     IOInterface has been opened, i.e., the IOInterface has been invalidated.
/// @error: SecurityErrorDomain::kUnreservedResource     if the IOInterface is not opened writeable.
ara::core::Result< void > PKeyPrivate_Ecc::Save(IOInterface &container) const noexcept
{
    /// @error: SecurityErrorDomain::kIncompatibleObject     if the object is "session", but the IOInterface represents
    /// a KeySlot.
    if (IsSession()) {
        if (false == container.IsVolatile()) {
            return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kIncompatibleObject);
        }
    }
    // /// @error: SecurityErrorDomain::kInsufficientCapacity   if the capacity of the target container is not enough,
    if (GetPayloadSize() > container.GetCapacity()) {
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
    if (CryptoObjectType::kPrivateKey != container.GetTypeRestriction()) {
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
//***************/  //PrivateKey interface
/// @brief Get the public key corresponding to this private key
/// @returns  Instance of the public key
ara::core::Result< PublicKey::Uptrc > PKeyPrivate_Ecc::GetPublicKey() const noexcept
{
    ara::core::String stPublicKey{MakePublicKey(pEccPair_)};
    PKeyPublic_Ecc::Uptr pPublicKey{std::make_unique< PKeyPublic_Ecc >(stPublicKey)};
    pPublicKey->SetAllowedUsage(GetAllowedUsage());
    pPublicKey->SetExportable(IsExportable());
    pPublicKey->SetSession(IsSession());
    COIdentifier keyId;
    keyId.mCOType = CryptoObjectType::kPublicKey;
    keyId.mCouid  = GetObjectId().mCouid;
    pPublicKey->SetCOIdentifier(keyId);
    return ara::core::Result< PublicKey::Uptrc >::FromValue(std::move(pPublicKey));
}
//***************/
/// @brief Delete all resources possibly allocated by Rsa
/// @name  _DeleteEcc
void PKeyPrivate_Ecc::_DeleteEcc() noexcept
{
    if (pEccPair_ != nullptr) {
        EVP_PKEY_free(pEccPair_);
        pEccPair_ = nullptr;
    }
}
/// @brief Generate the key string
/// @name  MakeKeyString
/// @param pEccKey ECC key resource
/// @returns  ECC private key string
ara::core::String PKeyPrivate_Ecc::MakeKeyString(BIO *const pEccKey) noexcept
{
    // Get length
    int32_t const nLen{PH_BIO_pending(pEccKey)};
    if (nLen <= 0) {
        return ara::core::String();
    }
    internal::PAutoBuff const autoBuff{static_cast< uint32_t >(nLen) + 2U};
    std::ignore = BIO_read(pEccKey, autoBuff.Data(), nLen);
    ara::core::String stKey(T_String(autoBuff.Data(), static_cast< std::size_t >(nLen)));
    return stKey;
}
/// @brief Generate public key
/// @param pEccPair Pointer to Ecc key structure
/// @returns ECC public key string
ara::core::String PKeyPrivate_Ecc::MakePublicKey(EVP_PKEY *const pEccPair) noexcept
{
    ara::core::String stKey;
    if (nullptr == pEccPair) {
        return stKey;
    }
    // Generate public key
    BIO *const pPublic{BIO_new(BIO_s_mem())};
    std::ignore = PEM_write_bio_PUBKEY(pPublic, pEccPair);
    stKey       = MakeKeyString(pPublic);
    std::ignore = BIO_free(pPublic);
    return stKey;
}
/// @brief Serialize and export the key
/// @param formatId Data format: Raw, DER, PEM, etc.
/// @return Private key data
ara::core::Result< ara::core::Vector< ara::core::Byte > > PKeyPrivate_Ecc::ExportPublicly(
    Serializable::FormatId formatId) const noexcept
{
    /// If not exportable
    if (!IsExportable()) {
        std::ignore = formatId;
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromValue();
    }
    /// @error: SecurityErrorDomain::kInsufficientCapacity   if (output.empty() == false), but it's capacity is less
    /// than required
    /// @error: SecurityErrorDomain::kUnknownIdentifier      if an unknown format ID was specified
    if (false == Serializable::IsValidFormatID(formatId, false)) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kUnknownIdentifier);
    }
    /// @error: SecurityErrorDomain::kUnsupportedFormat      if the specified format ID is not supported for this object
    /// type
    if (false == Serializable::IsValidFormatID(formatId, true)) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kUnsupportedFormat);
    }
    if (nullptr == pEccPair_) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kUnsupportedFormat);
    }
    // Generate public key
    std::function< int32_t(BIO *, Serializable::FunctionId) > const func{
        [this](BIO *const pbio, Serializable::FunctionId const funcId) -> int32_t {
            if (funcId == Serializable::kFuncDer) {
                int32_t const ret{i2d_PrivateKey_bio(pbio, pEccPair_)};
                return ret;
            }
            if (funcId == Serializable::kFuncPem) {
                int32_t const ret{PEM_write_bio_PrivateKey(pbio, pEccPair_, nullptr, nullptr, 0, nullptr, nullptr)};
                return ret;
            }
            return 0;
        }};

    return Serializable::ExportPublic_Fun(formatId, func);
}
/// @brief: Save the key
/// @name  _SaveToKeySlot
/// @param container IO interface
/// @returns true if save sucess false otherwise
bool PKeyPrivate_Ecc::_SaveToKeySlot(IOInterface &container) const noexcept
{
    keys::isoft_def::PIoInterface *const pIoInterface{dynamic_cast< keys::isoft_def::PIoInterface * >(&container)};
    if (nullptr == pIoInterface) {
        return false;
    }
    ara::core::Result< ara::core::Vector< ara::core::Byte > > const result{ExportPublicly()};
    if (false == result.HasValue()) {
        return false;
    }
    ReadOnlyMemRegion const memKeyData{
        static_cast< uint8_t const * >(static_cast< void const * >(result.Value().data())), result.Value().size()};
    return pIoInterface->SaveKeyToSlot< PKeyPrivate_Ecc >(this, memKeyData);
}
//********************************/
/// @brief Bind an Rsa private key
/// @name  AttachKey
/// @param stPrivateKey Private key
/// @returns  true if attach sucess false otherwise
bool PKeyPrivate_Ecc::AttachKey(ara::core::StringView const &stPrivateKey) noexcept
{
    _DeleteEcc();
    pEccPair_ = ExplanPrivateKey(stPrivateKey);

    if (pEccPair_ != nullptr) {
        return true;
    }
    return false;
}
/// @brief Parse the private key
/// @name  ExplanPrivateKey
/// @param stPrivateKey Private key
/// @returns  Instance of the private key object
EVP_PKEY *PKeyPrivate_Ecc::ExplanPrivateKey(ara::core::StringView const &stPrivateKey) noexcept
{
    // Private key format //-----BEGIN RSA PRIVATE KEY-----~-----END RSA PRIVATE KEY-----
    EVP_PKEY *ecKeyPrivate{nullptr};
    BIO *const pKeyBio{BIO_new_mem_buf(static_cast< uint8_t const * >(static_cast< void const * >(stPrivateKey.data())),
                                       static_cast< int32_t >(stPrivateKey.size()))};
    ecKeyPrivate = PEM_read_bio_PrivateKey(pKeyBio, nullptr, nullptr, nullptr);
    // Release temporary resources
    std::ignore = BIO_free(pKeyBio);
    return ecKeyPrivate;
}
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara
