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
/// @file       isoft_key_private_rsa.cpp
/// @brief      AutoSar-Crypto Encryption/Decryption module
/// @details
/// @date       2021-12-21
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Default encryption decryption/Asymmetric private key
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_06005
/// @unit_name=PKeyPrivate_Rsa
/// @unit_description=RSA private key
/// @endcode
///
/// ================================================================

#include "ara/crypto/cryp/cryobj/isoft_key_private_rsa.h"

#include "ara/crypto/common/isoft_common_api.h"
#include "ara/crypto/common/isoft_data_type.h"
#include "ara/crypto/common/isoft_io_interface.h"
#include "ara/crypto/common/isoft_log_api.h"
#include "ara/crypto/common/serializable.h"
#include "ara/crypto/cryp/cryobj/isoft_crypto_primitive_id.h"
#include "ara/crypto/cryp/cryobj/isoft_key_public_rsa.h"
#include "ara/crypto/cryp/isoft_auto_buff.h"
#include "ara/crypto/openssl/isoft_openssl_encrypt.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
//********************************/
/// @brief General asymmetric private key interface.
//********************************/
// /// @brief Default constructor
// /// @name PKeyPrivate_Rsa
// PKeyPrivate_Rsa::PKeyPrivate_Rsa() noexcept {}
/// @brief Default virtual destructor
/// @name  ~PKeyPrivate_Rsa
PKeyPrivate_Rsa::~PKeyPrivate_Rsa() noexcept { _DeleteRsa(); }
//***************/  //CryptoObject interface
/// @brief Return the CryptoPrimitiveId of this crypto object.
/// @name  GetCryptoPrimitiveId
/// @returns  Instance of CryptoPrimitiveId
CryptoPrimitiveId::Uptr PKeyPrivate_Rsa::GetCryptoPrimitiveId() const noexcept
{
    CryptoPrimitiveId::Uptr pReturn{nullptr};
    switch (GetPayloadSize() * kInt_8U) {
        case kInt_512U: {
            pReturn = std::make_unique< PAlgId_Asymmetric_RsaKey512 >();
        } break;
        case kInt_1024U: {
            pReturn = std::make_unique< PAlgId_Asymmetric_RsaKey1024 >();
        } break;
        case kInt_2048U: {
            pReturn = std::make_unique< PAlgId_Asymmetric_RsaKey2048 >();
        } break;
        case kInt_4096U: {
            pReturn = std::make_unique< PAlgId_Asymmetric_RsaKey4096 >();
        } break;
        case kInt_8192U: {
            pReturn = std::make_unique< PAlgId_Asymmetric_RsaKey8192 >();
        } break;
        default: {
            pReturn = std::make_unique< PAlgId_Asymmetric_RsaKey1024 >();
        } break;
    }

    return pReturn;
}
/// @brief Return the actual size of the object payload, in bytes.
/// @brief Return actual size of the object’s payload.
///         Returned value always must be less than or equal to the maximum payload size expected for this primitive and
///         object type, it is available via call: My Provider().GetPayloadStorageSize(GetObjectType(),
///         GetPrimitiveId()).Value(); Returned value does not take into account the object’s meta-information
///         properties, but their size is fixed and common for all crypto objects independently from their actual type.
///         During an allocation of a @c TrustedContainer, Crypto Providers (and Key Storage Providers) reserve space
///         for an object’s meta-information automatically, according to their implementation details.
/// @returns size in bytes of the object's payload required for its storage
/// @name  GetPayloadSize
std::size_t PKeyPrivate_Rsa::GetPayloadSize() const noexcept
{
    if (pRsaPair_ != nullptr) {
        return static_cast< std::size_t >(Get_RsaKey_Size(pRsaPair_));
    }
    return 0U;
}
/// @brief Save itself to the provided IOInterface. A crypto object with the attribute "session" cannot be saved in a KeySlot.
/// @brief Save itself to provided IOInterface
/// A CryptoObject with property "session" cannot be saved in a KeySlot.
/// @name  Save
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
ara::core::Result< void > PKeyPrivate_Rsa::Save(IOInterface &container) const noexcept
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
/// @name  GetPublicKey
/// @returns Instance of the public key corresponding to the private key
ara::core::Result< PublicKey::Uptrc > PKeyPrivate_Rsa::GetPublicKey() const noexcept
{
    ara::core::String stPublicKey{_MakePublicKey(pRsaPair_)};
    PKeyPublic_Rsa::Uptr pPublicKey{std::make_unique< PKeyPublic_Rsa >(stPublicKey)};
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
/// @brief Find the public key through the private key
/// @name  GetPublicKeyString
/// @param stPrivateKey Private key
/// @returns  Public key data corresponding to the private key
ara::core::String PKeyPrivate_Rsa::GetPublicKeyString(ara::core::StringView const &stPrivateKey) const noexcept
{
    // Private key format //-----BEGIN EVP_PKEY PRIVATE KEY-----~-----END EVP_PKEY PRIVATE KEY-----
    EVP_PKEY *const pRsa{ExplanPrivateKey(stPrivateKey)};
    if (nullptr == pRsa) {
        return ara::core::String();
    }
    // Generate public key  //-----BEGIN EVP_PKEY PUBLIC KEY-----~-----END EVP_PKEY PUBLIC KEY-----
    ara::core::String stPublicKey{_MakePublicKey(pRsa)};
    EVP_PKEY_free(pRsa);
    return stPublicKey;
}
/// @brief Bind an Rsa private key
/// @name  AttachKey
/// @param stPrivateKey Private key
/// @returns  true if attach sucess false otherwise
bool PKeyPrivate_Rsa::AttachKey(ara::core::StringView const &stPrivateKey) noexcept
{
    _DeleteRsa();
    pRsaPair_ = ExplanPrivateKey(stPrivateKey);
    if (pRsaPair_ != nullptr) {
        return true;
    }
    return false;
}
/// @brief Randomly initialize the key
/// @name  RandomInitKey
/// @param nKeyBitLength Key length: in bits
/// @returns  true if init key sucess false otherwise
bool PKeyPrivate_Rsa::RandomInitKey(int32_t const nKeyBitLength) noexcept
{
    _DeleteRsa();
    nPrivateKeyBitLength_ = static_cast< uint32_t >(nKeyBitLength);
    /// EVP_PKEY parameter e uses RSA_F4, i.e., 0x10001L, for higher security
    pRsaPair_ = Generate_rsa_evp_key(nKeyBitLength);
    return pRsaPair_ != nullptr;
}
//***************/
/// @brief Delete all resources possibly allocated by Rsa
/// @name  _DeleteRsa
void PKeyPrivate_Rsa::_DeleteRsa() noexcept
{
    if (pRsaPair_ != nullptr) {
        EVP_PKEY_free(pRsaPair_);
        pRsaPair_ = nullptr;
    }
}
/// @brief Generate the key string
/// @name  MakeKeyString
/// @param pRsaKey RSA key resource
/// @returns  ara::core::String
ara::core::String PKeyPrivate_Rsa::MakeKeyString(BIO *const pRsaKey) noexcept
{
    // Get length
    int32_t const nLen{PH_BIO_pending(pRsaKey)};
    if (nLen <= 0) {
        return ara::core::String();
    }
    internal::PAutoBuff const autoBuff{static_cast< uint32_t >(nLen) + 2U};
    std::ignore = BIO_read(pRsaKey, autoBuff.Data(), nLen);
    ara::core::String stKey(T_String(autoBuff.Data(), static_cast< std::size_t >(nLen)));
    return stKey;
}
/// @brief Generate public key
/// @name  _MakePublicKey
/// @param pRsaPair Pointer to RSA resource
/// @returns  RSA public key data
ara::core::String PKeyPrivate_Rsa::_MakePublicKey(EVP_PKEY const *const pRsaPair) const noexcept
{
    std::ignore = pRsaPair;
    ara::core::String stKey;
    if (nullptr == pRsaPair_) {
        return stKey;
    }
    // Generate public key
    BIO *const pPublic{BIO_new(BIO_s_mem())};
    //-----BEGIN EVP_PKEY PUBLIC KEY-----~-----END EVP_PKEY PUBLIC KEY-----
    std::ignore = PEM_write_bio_PUBKEY(pPublic, pRsaPair_);
    stKey       = MakeKeyString(pPublic);
    std::ignore = BIO_free(pPublic);
    // Sample code for saving to file
    return stKey;
}
/// @brief Generate private key
/// @name  MakePrivateKey
/// @param pRsaPair Pointer to RSA resource
/// @returns  RSA private key data
ara::core::String PKeyPrivate_Rsa::MakePrivateKey(EVP_PKEY *const pRsaPair) noexcept
{
    // Generate private key
    BIO *const pPrivate{BIO_new(BIO_s_mem())};
    //-----BEGIN RSA PRIVATE KEY-----~-----END RSA PRIVATE KEY-----
    std::ignore = PEM_write_bio_PrivateKey(pPrivate, pRsaPair, nullptr, nullptr, 0, nullptr, nullptr);
    ara::core::String stKey{MakeKeyString(pPrivate)};

    std::ignore = BIO_free(pPrivate);
    return stKey;
}
/// @brief Parse the private key
/// @name  ExplanPrivateKey
/// @param stPrivateKey Private key
/// @returns  EVP_PKEY *
EVP_PKEY *PKeyPrivate_Rsa::ExplanPrivateKey(ara::core::StringView const &stPrivateKey) noexcept
{
    // Private key format //-----BEGIN RSA PRIVATE KEY-----~-----END RSA PRIVATE KEY-----
    EVP_PKEY *pRsa{nullptr};
    BIO *const pKeyBio{BIO_new_mem_buf(T_TransBytes(stPrivateKey.data()), static_cast< int32_t >(stPrivateKey.size()))};
    pRsa = PEM_read_bio_PrivateKey(pKeyBio, &pRsa, nullptr, nullptr);
    // Release temporary resources
    std::ignore = BIO_free(pKeyBio);
    return pRsa;
}
/// @brief: Save the key
/// @name  _SaveToKeySlot
/// @param container IO interface
/// @returns  true if save to keyslot false otherwise
bool PKeyPrivate_Rsa::_SaveToKeySlot(IOInterface &container) const noexcept
{
    keys::isoft_def::PIoInterface *const pIoInterface{dynamic_cast< keys::isoft_def::PIoInterface * >(&container)};
    if (nullptr == pIoInterface) {
        return false;
    }
    ara::core::String const stKey{MakePrivateKey(GetRsa())};
    ReadOnlyMemRegion const memKeyData{T_TransBytes(stKey.data()), stKey.size()};
    return pIoInterface->SaveKeyToSlot< PKeyPrivate_Rsa >(this, memKeyData);
}
//***************/
/// @brief Initialize a pair of RSA keys
/// @name  DebugInitKey
/// @param nKeyBitLength Key length: in bits
/// @param bPrint Whether to print logs
void PKeyPrivate_Rsa::DebugInitKey(int32_t const nKeyBitLength, bool const bPrint) noexcept
{
    std::ignore = RandomInitKey(nKeyBitLength);
    if (bPrint) {
        // Generate private key  //-----BEGIN EVP_PKEY PRIVATE KEY-----~-----END EVP_PKEY PRIVATE KEY-----
        DebugPrintRsaKey(MakePrivateKey(pRsaPair_));

        // Generate public key  //-----BEGIN EVP_PKEY PUBLIC KEY-----~-----END EVP_PKEY PUBLIC KEY-----
        DebugPrintRsaKey(_MakePublicKey(pRsaPair_));

        // Generate public key  //-----BEGIN PUBLIC KEY-----~-----END PUBLIC KEY-----
        BIO *const pPublic2{BIO_new(BIO_s_mem())};
        std::ignore = PEM_write_bio_PUBKEY(pPublic2, pRsaPair_);
        DebugPrintRsaKey(MakeKeyString(pPublic2));
        std::ignore = BIO_free(pPublic2);
        // Test extracting the public key from the private key
        ara::core::String const stPrivateKey{MakePrivateKey(pRsaPair_)};
        std::ignore = GetPublicKeyString({stPrivateKey.data(), stPrivateKey.size()});
    }
}
/// @brief Print the key
/// @name  DebugPrintRsaKey
/// @param stKey Key data: in string format
/// @returns  void
void PKeyPrivate_Rsa::DebugPrintRsaKey(ara::core::String const &stKey) noexcept
{
    ara::crypto::isoft_def::LogInfo() << "RSA_KEY = [" << static_cast< int32_t >(stKey.size()) << "].[" << stKey.data()
                                      << "]";
}
//********************************/
/// @brief Serialize the Private itself.
/// @name  ExportPublicly
/// @returns  ara::core::Result<ara::core::Vector<ara::core::Byte> >
/// @throws
/// @param formatId Data format: Raw, DER, PEM, etc.
/// @return Private key data
ara::core::Result< ara::core::Vector< ara::core::Byte > > PKeyPrivate_Rsa::ExportPublicly(
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
    if (nullptr == pRsaPair_) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kUnsupportedFormat);
    }

    // Generate public key
    std::function< int32_t(BIO *, Serializable::FunctionId) > const func{
        [this](BIO *const pbio, Serializable::FunctionId const funcId) -> int32_t {
            if (funcId == Serializable::kFuncDer) {
                int32_t const ret{i2d_PrivateKey_bio(pbio, pRsaPair_)};
                return ret;
            }
            if (funcId == Serializable::kFuncPem) {
                int32_t const ret{PEM_write_bio_PrivateKey(pbio, pRsaPair_, nullptr, nullptr, 0, nullptr, nullptr)};
                return ret;
            }
            return 0;
        }};

    return Serializable::ExportPublic_Fun(formatId, func);
}
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara
