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
/// @file       isoft_key_public_rsa.cpp
/// @brief      AutoSar-Crypto Encryption/Decryption module
/// @details
/// @date       2022-03-02
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Default encryption decryption/Asymmetric public key
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_06005
/// @unit_name=PKeyPublic_Rsa
/// @unit_description=RSA public key
/// @endcode
///
/// ================================================================

#include "ara/crypto/cryp/cryobj/isoft_key_public_rsa.h"

#include <openssl/evp.h>

#include "ara/crypto/common/isoft_data_type.h"
#include "ara/crypto/common/isoft_io_interface.h"
#include "ara/crypto/cryp/cryobj/isoft_crypto_primitive_id.h"
#include "ara/crypto/ipc/isoft_ipc_client.h"
#include "ara/crypto/openssl/isoft_openssl_encrypt.h"
namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
//********************************/
/// @brief General asymmetric public key interface.
/// @param stPublicKey Public key
PKeyPublic_Rsa::PKeyPublic_Rsa(ara::core::StringView const &stPublicKey) noexcept
    : PKeyPublic_Rsa{0U, stPublicKey, nullptr, 0U, 0U}
{
}
/// @brief Parameterized constructor
/// @param nPublicKeyBitLength Public key length: in bits
/// @param stPublicKey Public key
/// @param pRsaPair Pointer to RSA resource
/// @param slotId Key slot ID
/// @param onlyPublicKeyId Slot ID of the public key
PKeyPublic_Rsa::PKeyPublic_Rsa(uint32_t const nPublicKeyBitLength,
                               ara::core::StringView const &stPublicKey,
                               EVP_PKEY const *const pRsaPair,
                               uint32_t const slotId,
                               uint32_t const onlyPublicKeyId) noexcept
    : PKeyPublic_Base{}  // NOLINT
    , nPublicKeyBitLength_{nPublicKeyBitLength}
    , pRsaPair_{nullptr}
    , slotId_{slotId}
    , onlyPublicKeyId_{onlyPublicKeyId}
{
    std::ignore = pRsaPair;
    // The input data must be in the "//-----BEGIN RSA PUBLIC KEY-----~-----END RSA PUBLIC KEY-----" format
    BIO *const pKeyBio{BIO_new_mem_buf(stPublicKey.data(), static_cast< int32_t >(stPublicKey.size()))};
    pRsaPair_ = PEM_read_bio_PUBKEY(pKeyBio, nullptr, nullptr, nullptr);

    BIO_free_all(pKeyBio);

    if (pRsaPair_ != nullptr) {
    }
}
/// @brief Default virtual destructor
PKeyPublic_Rsa::~PKeyPublic_Rsa() noexcept { _DelRsa(); }
//***************/  //CryptoObject interface
/// @brief Return the CryptoPrimitiveId of this crypto object.
/// @return Instance of CryptoPrimitiveId
CryptoPrimitiveId::Uptr PKeyPublic_Rsa::GetCryptoPrimitiveId() const noexcept
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
/// @brief Return the actual size of the object payload.
///         The return value in bytes must be less than or equal to the maximum payload size expected for this primitive and object type, which can be obtained by calling: GetPayloadStorageSize
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
/// @trace_id_sws={SWS_CRYPT_20516}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02309}
/// @threadsafety={Thread-safe}
std::size_t PKeyPublic_Rsa::GetPayloadSize() const noexcept
{
    if (pRsaPair_ != nullptr) {
        return static_cast< std::size_t >(Get_RsaKey_Size(pRsaPair_));
    }
    return 0U;
}
/// @brief Save itself to the provided IOInterface. A crypto object with the attribute "session" cannot be saved in a KeySlot.
/// @brief Save itself to provided IOInterface
/// A CryptoObject with property "session" cannot be saved in a KeySlot.
/// @param container IOInterface representing underlying storage
/// @return has value if save sucess false otherwise
/// @trace_id_sws={SWS_CRYPT_20517}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02004}
/// @error: SecurityErrorDomain::kIncompatibleObject     if the object is "session", but the IOInterface represents a
/// KeySlot.
/// @error: SecurityErrorDomain::kContentRestrictions    if the object doesn't satisfy the slot restrictions
/// @error: SecurityErrorDomain::kInsufficientCapacity   if the capacity of the target container is not enough, i.e. if
/// <tt>(container.Capacity() < this->StorageSize())</tt>
/// @error: SecurityErrorDomain::kModifiedResource       if the underlying resource has been modified after  the
/// IOInterface has been opened, i.e., the IOInterface has been invalidated.
/// @error: SecurityErrorDomain::kUnreservedResource     if the IOInterface is not opened writeable.
/// @threadsafety={Thread-safe}
ara::core::Result< void > PKeyPublic_Rsa::Save(IOInterface &container) const noexcept
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
    /// @error: SecurityErrorDomain::kContentRestrictions    if the object doesn't satisfy the slot restrictions
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
/// @trace_id_sws={SWS_CRYPT_10711}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02112}
/// @error: SecurityErrorDomain::kInsufficientCapacity   if (output.empty() == false), but it's capacity is less than
/// required
/// @error: SecurityErrorDomain::kUnknownIdentifier      if an unknown format ID was specified
/// @error: SecurityErrorDomain::kUnsupportedFormat      if the specified format ID is not supported for this object
/// type
/// @threadsafety={Thread-safe}
ara::core::Result< ara::core::Vector< ara::core::Byte > > PKeyPublic_Rsa::ExportPublicly(
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
    ara::core::Vector< ara::core::Byte > const vecResult{_MakePublicKey(pRsaPair_, formatId)};
    return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromValue(vecResult);
}
//***************/ //PublicKey interface
/// @brief Check the correctness of the key.
/// @param strongCheck Whether to enable strong verification
/// @return true if check sucess false otherwise
bool PKeyPublic_Rsa::CheckKey(bool strongCheck) const noexcept
{
    /// slotId indicates that both the public and private keys are generated locally, so return true directly
    if (slotId_ == 0U) {
        return true;
    }
    // //It does not work on RSA public keys that have only the modulus and public exponent elements populated. It also
    // checks that d*e = 1 mod (p-1*q-1), and that dmp1, dmq1 and iqmp are set correctly or are nullptr. It performs
    // integrity checks on all the RSA key material, so the RSA key structure must contain all the private key data too.
    // Therefore, it cannot be used with any arbitrary RSA key object, even if it is otherwise fit for regular RSA
    // operation. return 1 == RSA_check_key( pRsaPair_ );
    uint32_t const nSlotId{slotId_};
    keys::isoft_def::PIpcClient::CB_IpcReqMsg const func{
        [nSlotId, strongCheck](keys::isoft_def::PIpcAutoPacket const *const pReqMsg) -> uint16_t {
            if (pReqMsg != nullptr) {
                keys::isoft_def::cryptoctx::PIpcReq_Common ipcReq;
                ipcReq.SetIpcSlotID(nSlotId);
                if (strongCheck) {
                    ipcReq.SetDataLen(1U);
                } else {
                    ipcReq.SetDataLen(0U);
                }
                std::ignore = pReqMsg->AddDataToIpc(&ipcReq, sizeof(ipcReq));
            }
            return sizeof(keys::isoft_def::cryptoctx::PIpcReq_Common);
        }};
    keys::isoft_def::PIpcClient *const ipcClient{keys::isoft_def::PIpcClient::GetInstance_Keys()};
    if (ipcClient == nullptr) {
        return false;
    }
    keys::isoft_def::PIpcAutoPacket aswMsg;
    bool const bDealIpc{ipcClient->DealIpcRequest(FUNC_NAME_PrivateKey(CheckKey), aswMsg, func)};
    if (!bDealIpc) {
        return false;
    }
    keys::isoft_def::cryptoctx::PIpcAsw_common *const pIpcAsw{
        aswMsg.GetIpcBody< keys::isoft_def::cryptoctx::PIpcAsw_common >()};
    if (pIpcAsw->GetErrorID() != 0) {
        return false;
    }
    if (pIpcAsw->GetDataLen() != 0U) {
        return false;
    }
    return true;
}
//***************/
/// @brief Set the RSA key
/// @param pData Data: starting address of memory
/// @param nLen Data length: in bytes
/// @return true if set sucess false otherwise
bool PKeyPublic_Rsa::SetRsaKey(uint8_t const *pData, uint32_t const nLen) noexcept
{
    _DelRsa();
    // ASN1 format string
    pRsaPair_ = d2i_PublicKey(EVP_PKEY_RSA, nullptr, &pData, static_cast< int64_t >(nLen));
    if (nullptr == pRsaPair_) {
        return false;
    }
    return true;
}
/// @brief Set the RSA key
/// @param pRsa RSA key resource
/// @return true if set sucess false otherwise
bool PKeyPublic_Rsa::SetRsaKey(EVP_PKEY *const pRsa) noexcept
{
    _DelRsa();
    pRsaPair_ = Evp_pkey_dup(pRsa);
    return true;
}
//***************/
/// @brief Release RSA resources
void PKeyPublic_Rsa::_DelRsa() noexcept
{
    if (pRsaPair_ != nullptr) {
        EVP_PKEY_free(pRsaPair_);
        pRsaPair_ = nullptr;
    }
}
/// @brief Generate public key
/// @param pRsaPair Pointer to RSA resource
/// @param formatId Data format: Raw, DER, PEM, etc.
/// @return Public key data
ara::core::Vector< ara::core::Byte > PKeyPublic_Rsa::_MakePublicKey(
    EVP_PKEY *const pRsaPair, Serializable::FormatId const formatId) const noexcept
{
    ara::core::Vector< ara::core::Byte > vecData;
    if (nullptr == pRsaPair_) {
        return vecData;
    }
    // Generate public key
    std::function< int32_t(BIO *, Serializable::FunctionId) > const func{
        [pRsaPair](BIO *const pbio, Serializable::FunctionId const funcId) -> int32_t {
            if (funcId == Serializable::kFuncDer) {
                int32_t const ret{i2d_PUBKEY_bio(pbio, pRsaPair)};
                return ret;
            }
            if (funcId == Serializable::kFuncPem) {
                int32_t const ret{PEM_write_bio_PUBKEY(pbio, pRsaPair)};
                return ret;
            }
            return 0;
        }};

    return Serializable::ExportPublic_Fun(formatId, func).Value();
}
/// @brief: Save the key
/// @param container IO interface
/// @return true if save to keyslot sucess false otherwise
bool PKeyPublic_Rsa::_SaveToKeySlot(IOInterface &container) const noexcept
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
    return pIoInterface->SaveKeyToSlot< PKeyPublic_Rsa >(this, memKeyData);
}
//********************************/
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara
