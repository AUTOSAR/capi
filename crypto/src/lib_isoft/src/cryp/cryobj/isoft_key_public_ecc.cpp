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
/// @file       isoft_key_public_ecc.cpp
/// @brief      AutoSar-Crypto Encryption/Decryption module
/// @details
/// @date       2022-08-02
/// @author     Chang Zheng
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Default encryption decryption/Asymmetric public key
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_06005
/// @unit_name=PKeyPublic_Ecc
/// @unit_description=ECC public key
/// @endcode
///
/// ================================================================

#include "ara/crypto/cryp/cryobj/isoft_key_public_ecc.h"

#include "ara/crypto/common/isoft_common_api.h"
#include "ara/crypto/common/isoft_data_type.h"
#include "ara/crypto/common/isoft_io_interface.h"
#include "ara/crypto/cryp/cryobj/isoft_crypto_primitive_id.h"
#include "ara/crypto/openssl/isoft_openssl_encrypt.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
//********************************/
/// @brief General asymmetric public key interface.
/// @name  PKeyPublic_Rsa
/// @throws
/// @param stPublicKey Public key
PKeyPublic_Ecc::PKeyPublic_Ecc(ara::core::StringView const &stPublicKey) noexcept : PKeyPublic_Base{}  // NOLINT
{
    // The input data must be in the "//-----BEGIN PUBLIC KEY-----~-----END PUBLIC KEY-----" format
    BIO *const pKeyBio{BIO_new_mem_buf(stPublicKey.data(), static_cast< int32_t >(stPublicKey.size()))};
    pEccPair_ = PEM_read_bio_EC_PUBKEY_Compatible(pKeyBio);
    BIO_free_all(pKeyBio);
}
/// @brief Default virtual destructor
PKeyPublic_Ecc::~PKeyPublic_Ecc() noexcept { _DelEcc(); }
//***************/  //CryptoObject interface
/// @brief Return the CryptoPrimitiveId of this crypto object.
/// @return Instance of CryptoPrimitiveId
CryptoPrimitiveId::Uptr PKeyPublic_Ecc::GetCryptoPrimitiveId() const noexcept
{
    CryptoPrimitiveId::Uptr pReturn{nullptr};
    switch (GetPayloadSize() * kInt_8U) {
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
/// @brief Return the actual size of the object payload, in bytes. The return value must be less than or equal to the maximum payload size expected for this primitive and object type,
///         which can be obtained by calling "MyProvider().GetPayloadStorageSize(GetObjectType(), GetPrimitiveId()).Value();"
///         The return value does not consider the meta-information properties of the object, but their size is fixed and common to all crypto objects independent of their actual type. During the allocation of a TrustedContainer,
///         the crypto provider (and key storage provider) automatically reserves space for the object's meta-information according to its implementation details.
/// @brief Return actual size of the object’s payload.
///         Returned value always must be less than or equal to the maximum payload size expected for this primitive and
///         object type, it is available via call: My Provider().GetPayloadStorageSize(GetObjectType(),
///         GetPrimitiveId()).Value(); Returned value does not take into account the object’s meta-information
///         properties, but their size is fixed and common for all crypto objects independently from their actual type.
///         During an allocation of a @c TrustedContainer, Crypto Providers (and Key Storage Providers) reserve space
///         for an object’s meta-information automatically, according to their implementation details.
/// @returns size in bytes of the object's payload required for its storage
/// @code{.isoft}
/// @export_level=/Crypto
/// @trace_id_sws={SWS_CRYPT_20516}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02309}
/// @threadsafety={Thread-safe}
/// @endcode
std::size_t PKeyPublic_Ecc::GetPayloadSize() const noexcept
{
    return static_cast< std::size_t >(Get_EccKey_Size(pEccPair_));
}
/// @brief Save itself to the provided IOInterface. A crypto object with the attribute "session" cannot be saved in a KeySlot.

/// @brief Save itself to provided IOInterface
/// A CryptoObject with property "session" cannot be saved in a KeySlot.
/// @param container IOInterface representing underlying storage
/// @return
/// @error: SecurityErrorDomain::kIncompatibleObject     if the object is "session", but the IOInterface represents a
/// KeySlot.
/// @error: SecurityErrorDomain::kContentRestrictions    if the object doesn't satisfy the slot restrictions
/// @error: SecurityErrorDomain::kInsufficientCapacity   if the capacity of the target container is not enough, i.e. if
/// <tt>(container.Capacity() < this->StorageSize())</tt>
/// @error: SecurityErrorDomain::kModifiedResource       if the underlying resource has been modified after  the
/// IOInterface has been opened, i.e., the IOInterface has been invalidated.
/// @error: SecurityErrorDomain::kUnreservedResource     if the IOInterface is not opened writeable.
/// @trace_id_sws={SWS_CRYPT_20517}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02004}
/// @threadsafety={Thread-safe}
ara::core::Result< void > PKeyPublic_Ecc::Save(IOInterface &container) const noexcept
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
    if (CryptoObjectType::kPublicKey != container.GetTypeRestriction()) {
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
//***************/ //Serializable interface
/// @brief Publicly serialize itself.
/// @brief Serialize itself publicly.
/// @param formatId  the Crypto Provider specific identifier of the output format
/// @returns a buffer with the serialized object
/// @error: SecurityErrorDomain::kInsufficientCapacity   if (output.empty() == false), but it's capacity is less than
/// required
/// @error: SecurityErrorDomain::kUnknownIdentifier      if an unknown format ID was specified
/// @error: SecurityErrorDomain::kUnsupportedFormat      if the specified format ID is not supported for this object
/// type
/// @trace_id_sws={SWS_CRYPT_10711}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02112}
/// @threadsafety={Thread-safe}
ara::core::Result< ara::core::Vector< ara::core::Byte > > PKeyPublic_Ecc::ExportPublicly(
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
    ara::core::Vector< ara::core::Byte > const vecResult{_MakePublicKey(pEccPair_, formatId)};
    return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromValue(vecResult);
}
//***************/ //PublicKey interface
/// @brief Check the correctness of the key.
/// @param strongCheck Whether to enable strong verification
/// @return true if check sucess false otherwise
bool PKeyPublic_Ecc::CheckKey(bool strongCheck) const noexcept
{
    std::ignore = strongCheck;
    /// @param[in] strongCheck  the severeness flag that indicates type of the required check: strong (if @c true) or
    /// fast (if @c false)
    if (nullptr == pEccPair_) {
        return false;
    }
    // //It does not work on RSA public keys that have only the modulus and public exponent elements populated. It also
    // checks that d*e = 1 mod (p-1*q-1), and that dmp1, dmq1 and iqmp are set correctly or are nullptr. It performs
    // integrity checks on all the RSA key material, so the RSA key structure must contain all the private key data too.
    // Therefore, it cannot be used with any arbitrary RSA key object, even if it is otherwise fit for regular RSA
    // operation. return 1 == RSA_check_key( pRsaPair_ );
    return true;
}
/// @brief Calculate the hash value of the public key value. The raw public key value BLOB can be obtained through the Serializable interface.
/// @param hashFunc Crypto context object for hash calculation
/// @return ECC public key data
ara::core::Result< ara::core::Vector< ara::core::Byte > > PKeyPublic_Ecc::HashPublicKey(
    HashFunctionCtx &hashFunc) const noexcept
{
    /// @error: SecurityErrorDomain::kIncompleteArgState     if the @c hashFunc context is not initialized
    if (false == hashFunc.IsInitialized()) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kIncompleteArgState);
    }
    /// @error: SecurityErrorDomain::kInsufficientCapacity   if size of the hash buffer is not enough for storing of the
    ara::core::Result< ara::core::Vector< ara::core::Byte > > const pResultVec{ExportPublicly()};
    if (!pResultVec.HasValue()) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kInsufficientCapacity);
    }
    ReadOnlyMemRegion const memKeyData{
        static_cast< uint8_t const * >(static_cast< void const * >(pResultVec.Value().data())),
        pResultVec.Value().size()};
    std::ignore = hashFunc.Update(memKeyData);
    ara::core::Result< ara::core::Vector< ara::core::Byte > > hashResult{hashFunc.Finish()};
    if (!hashResult.HasValue()) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kInsufficientCapacity);
    }
    return hashResult;
}
//***************/
/// @brief Delete the ECC structure
void PKeyPublic_Ecc::_DelEcc() noexcept
{
    if (pEccPair_ != nullptr) {
        EVP_PKEY_free(pEccPair_);
        pEccPair_ = nullptr;
    }
}
/// @brief Convert POINT to string
/// @param pPublicKey Public key
/// @param len Key length
void PKeyPublic_Ecc::PointToString(u_char *const pPublicKey, size_t &len) const noexcept
{
    if (pPublicKey == nullptr) {
        return;
    }
    BIO *const pub{BIO_new(BIO_s_mem())};
    std::ignore = PEM_write_bio_PUBKEY(pub, pEccPair_);
    size_t const nLen{static_cast< size_t >(PH_BIO_pending(pub))};
    if (len < nLen) {
        len         = 0U;
        std::ignore = BIO_free(pub);
        return;
    }
    len                 = nLen;
    std::ignore         = BIO_read(pub, pPublicKey, static_cast< int32_t >(len));
    *(pPublicKey + len) = static_cast< uint8_t >('\0');
    std::ignore         = BIO_free(pub);
}
/// @brief Set the length of the public key
/// @name   SetEccKeyBitLength
/// @param nKeyBitLength Key length: in bits
void PKeyPublic_Ecc::SetEccKeyBitLength(uint32_t const nKeyBitLength) noexcept { nGenPublicKeyBitLen_ = nKeyBitLength; }
/// @brief Generate public key
/// @param pEccPair Pointer to Ecc key structure
/// @param formatId Data format: Raw, DER, PEM, etc.
/// @return Public key data
ara::core::Vector< ara::core::Byte > PKeyPublic_Ecc::_MakePublicKey(
    EVP_PKEY const *const pEccPair, Serializable::FormatId const formatId) const noexcept
{
    ara::core::Vector< ara::core::Byte > vecData;
    if (nullptr == pEccPair) {
        return vecData;
    }
    // Generate public key
    std::function< int32_t(BIO *, Serializable::FunctionId) > const func{
        [this](BIO *const pbio, Serializable::FunctionId const funcId) -> int32_t {
            if (funcId == Serializable::kFuncDer) {
                int32_t const ret{i2d_PUBKEY_bio(pbio, pEccPair_)};
                return ret;
            }
            if (funcId == Serializable::kFuncPem) {
                int32_t const ret{PEM_write_bio_PUBKEY(pbio, pEccPair_)};
                return ret;
            }
            return 0;
        }};

    return Serializable::ExportPublic_Fun(formatId, func).Value();
}
/// @brief: Save the content from the IO interface to the key slot
/// @param container IO interface
/// @return true if save sucess false otherwise
bool PKeyPublic_Ecc::_SaveToKeySlot(IOInterface &container) const noexcept
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
    return pIoInterface->SaveKeyToSlot< PKeyPublic_Ecc >(this, memKeyData);
}
//********************************/
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara
