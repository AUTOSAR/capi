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
/// @file       isoft_key_private_ipc.cpp
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
/// @unit_name=PKeyPrivate_Ipc
/// @unit_description=IPC private key
/// @endcode
///
/// ================================================================

#include "ara/crypto/cryp/cryobj/isoft_key_private_ipc.h"

#include "ara/crypto/cryp/cryobj/isoft_crypto_primitive_id.h"
#include "ara/crypto/cryp/cryobj/isoft_key_public_ecc.h"
#include "ara/crypto/cryp/cryobj/isoft_key_public_rsa.h"
#include "ara/crypto/ipc/isoft_ipc_client.h"
#include "ara/crypto/ipc/isoft_ipc_protocol.h"
#include "ara/crypto/keys/isoft_ipc_io_interface.h"
#include "ara/crypto/keys/isoft_ipc_key_slot.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
//********************************/
/// @brief General asymmetric private key interface.
/// @name  PKeyPrivate_Ipc
/// @param nSlotId Key slot ID
/// @param nIointerfaceId IO interface ID
/// @param nSlotName Key slot name
PKeyPrivate_Ipc::PKeyPrivate_Ipc(uint32_t const nSlotId,
                                 uint32_t const nIointerfaceId,
                                 ara::core::StringView const &nSlotName) noexcept
    : PKeyPrivate_Base{}  // NOLINT
    , nSlotId_{nSlotId}
    , nIointerfaceId_{nIointerfaceId}
    , nSlotName_{nSlotName}
{
}
//***************/  //CryptoObject interface
/// @brief Save itself to the provided IOInterface. A crypto object with the attribute "session" cannot be saved in a KeySlot.
/// @name  Save
/// @param container IO interface
/// @returns has value if save sucess false otherwise
ara::core::Result< void > PKeyPrivate_Ipc::Save(IOInterface &container) const noexcept
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
    keys::isoft_def::PIoInterface_Ipc *const pIoInterfaceIpc{
        dynamic_cast< keys::isoft_def::PIoInterface_Ipc * >(&container)};
    if (pIoInterfaceIpc == nullptr) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kIncompatibleObject);
    }
    uint32_t nSlotId{pIoInterfaceIpc->GetSlotID()};
    ara::core::String const nSlotName{pIoInterfaceIpc->GetSlotName()};
    if (nSlotId == 0U) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kUnexpectedValue);
    }
    if (nSlotName.empty()) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kUnexpectedValue);
    }
    keys::isoft_def::PIpcKeySlot::Uptr const pKeySlot{
        std::make_unique< keys::isoft_def::PIpcKeySlot >(pIoInterfaceIpc->GetIpcClient(), nSlotName.c_str(), nSlotId)};
    if (pKeySlot.get() == nullptr) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kUnexpectedValue);
    }
    if (nSlotId == 0U) {
    }

    keys::isoft_def::PIpcClient *const ipcClient{keys::isoft_def::PIpcClient::GetInstance_Keys()};
    if (ipcClient == nullptr) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kUnexpectedValue);
    }
    keys::isoft_def::PIoInterface_Ipc::Uptr const pIoInterface{
        std::make_unique< keys::isoft_def::PIoInterface_Ipc >(*ipcClient, nSlotName_)};
    pIoInterface->SetSlotID(nSlotId_, nIointerfaceId_);

    ara::core::Result< void > const resVoid{pKeySlot->SaveCopy(*pIoInterface)};
    if (!resVoid.HasValue()) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kUnexpectedValue);
    }
    return ara::core::Result< void >::FromValue();
}
//***************/  //PrivateKey interface
/// @brief Get the public key corresponding to this private key
/// @name  GetPublicKey
/// @returns  Instance of the public key
ara::core::Result< PublicKey::Uptrc > PKeyPrivate_Ipc::GetPublicKey() const noexcept
{
    return ara::core::Result< PublicKey::Uptrc >::FromValue(nullptr);
}
/// @brief Return the COIdentifier of the object, which includes the object type and UID.
/// @return COIdentifier of the object
CryptoObject::COIdentifier PKeyPrivate_Ipc::GetObjectId() const noexcept
{
    keys::isoft_def::PIpcClient *const ipcClient{keys::isoft_def::PIpcClient::GetInstance_Keys()};
    if (ipcClient == nullptr) {
        return CryptoObject::COIdentifier{};
    }
    keys::isoft_def::PIoInterface_Ipc::Uptr const pIpcInterface{
        std::make_unique< keys::isoft_def::PIoInterface_Ipc >(*ipcClient, nSlotName_)};
    if (pIpcInterface.get() == nullptr) {
        return CryptoObject::COIdentifier{};
    }
    pIpcInterface->SetSlotID(nSlotId_, nIointerfaceId_);

    CryptoObject::COIdentifier coIdentifier;

    coIdentifier.mCouid  = pIpcInterface->GetObjectId();
    coIdentifier.mCOType = GetkObjectType();

    return coIdentifier;
}

//***************/
/// @brief Private keys on the server side are not allowed to be exported
/// @name  ExportPublicly
/// @returns  ara::core::Result<ara::core::Vector<ara::core::Byte> >
/// @param formatId Data format: Raw, DER, PEM, etc.
/// @return Public key data information
ara::core::Result< ara::core::Vector< ara::core::Byte > > PKeyPrivate_Ipc::ExportPublicly(
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
    keys::isoft_def::PIpcClient *const ipcClient{keys::isoft_def::PIpcClient::GetInstance_Keys()};
    if (ipcClient == nullptr) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromValue();
    }
    uint32_t const nSlotId{nSlotId_};
    keys::isoft_def::PIpcAutoPacket aswMsg;  // Receive return packet
    bool const bDealIpc{ipcClient->DealIpcRequest(
        ExportPubliclyFunName(), aswMsg,
        [nSlotId, formatId](keys::isoft_def::PIpcAutoPacket const *const pReqMsg) -> uint16_t {
            if (pReqMsg != nullptr) {
                keys::isoft_def::cryptoctx::PIpcReq_Common ipcReq;
                ipcReq.SetIpcSlotID(nSlotId);
                /// Here, lazily passing formatid through the data length field
                ipcReq.SetDataLen(static_cast< uint32_t >(formatId));
                std::ignore = pReqMsg->AddDataToIpc(&ipcReq, sizeof(ipcReq));
            }
            return sizeof(keys::isoft_def::cryptoctx::PIpcReq_Common);
        })};
    if (false == bDealIpc) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromValue();
    }

    keys::isoft_def::cryptoctx::PIpcAsw_common *const pIpcAsw{
        aswMsg.GetIpcBody< keys::isoft_def::cryptoctx::PIpcAsw_common >()};
    if (0 != pIpcAsw->GetErrorID()) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromValue();
    }

    uint8_t const *const pData{pIpcAsw->GetData()};
    uint32_t const nLen{pIpcAsw->GetDataLen()};
    ara::core::Vector< ara::core::Byte > vecResult;
    for (uint32_t i{0U}; i < nLen; i++) {
        vecResult.push_back(static_cast< ara::core::Byte >(*(pData + i)));
    }

    return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromValue(vecResult);
}
/// @brief Return the actual size of the object payload, in bytes.
/// @name  GetPayloadSize
/// @returns  Actual size of the payload
std::size_t PKeyPrivate_Ipc::GetPayloadSize() const noexcept
{
    keys::isoft_def::PIpcClient *const ipcClient{keys::isoft_def::PIpcClient::GetInstance_Keys()};
    if (ipcClient == nullptr) {
        return 0U;
    }
    uint32_t const nSlotId{nSlotId_};
    keys::isoft_def::PIpcAutoPacket aswMsg;  // Receive return packet
    bool const bDealIpc{ipcClient->DealIpcRequest(
        GetPayloadSizeFunName(), aswMsg, [nSlotId](keys::isoft_def::PIpcAutoPacket const *const pReqMsg) -> uint16_t {
            if (pReqMsg != nullptr) {
                keys::isoft_def::cryptoctx::PIpcReq_Common ipcReq;
                ipcReq.SetIpcSlotID(nSlotId);
                std::ignore = pReqMsg->AddDataToIpc(&ipcReq, sizeof(ipcReq));
            }
            return sizeof(keys::isoft_def::cryptoctx::PIpcReq_Common);
        })};
    if (false == bDealIpc) {
        return 0U;
    }

    keys::isoft_def::cryptoctx::PIpcAsw_common *const pIpcAsw{
        aswMsg.GetIpcBody< keys::isoft_def::cryptoctx::PIpcAsw_common >()};
    if (0 != pIpcAsw->GetErrorID()) {
        return 0U;
    }
    return static_cast< std::size_t >(pIpcAsw->GetDataLen());
}

//********************************/
/// @brief Return the CryptoPrimitiveId of this crypto object.
/// @return Instance of CryptoPrimitiveId
CryptoPrimitiveId::Uptr PKeyPrivate_Ipc_Ecc::GetCryptoPrimitiveId() const noexcept
{
    return {std::make_unique< PAlgId_Asymmetric_EccKey >()};
}

/// @brief Get the public key corresponding to this private key.
/// @return  Instance of the public key
ara::core::Result< PublicKey::Uptrc > PKeyPrivate_Ipc_Ecc::GetPublicKey() const noexcept
{
    keys::isoft_def::PIpcAutoPacket aswMsg;  // Receive return packet
    keys::isoft_def::PIpcClient *const ipcClient{keys::isoft_def::PIpcClient::GetInstance_Keys()};
    if (ipcClient == nullptr) {
        return ara::core::Result< PublicKey::Uptrc >::FromValue(nullptr);
    }
    uint32_t const nSlotId{GetSlotId()};
    bool const bDealIpc{
        ipcClient->DealIpcRequest(FUNC_NAME_PrivateKey(GetPublicKey_Ecc), aswMsg,
                                  [nSlotId](keys::isoft_def::PIpcAutoPacket const *const pReqMsg) -> uint16_t {
                                      if (pReqMsg != nullptr) {
                                          keys::isoft_def::cryptoctx::PIpcReq_Common ipcReq;
                                          ipcReq.SetIpcSlotID(nSlotId);
                                          std::ignore = pReqMsg->AddDataToIpc(&ipcReq, sizeof(ipcReq));
                                      }
                                      return sizeof(keys::isoft_def::cryptoctx::PIpcReq_Common);
                                  })};
    if (false == bDealIpc) {
        return ara::core::Result< PublicKey::Uptrc >::FromValue(nullptr);
    }

    keys::isoft_def::cryptoctx::PIpcAsw_GetPublic *const pIpcAsw{
        aswMsg.GetIpcBody< keys::isoft_def::cryptoctx::PIpcAsw_GetPublic >()};
    if (0 != pIpcAsw->GetErrorID()) {
        return ara::core::Result< PublicKey::Uptrc >::FromValue(nullptr);
    }
    uint8_t const *const pdata{pIpcAsw->GetData()};
    int32_t const len{static_cast< int32_t >(pIpcAsw->GetDataLen())};
    ara::core::String stKey(static_cast< ara::crypto::char8_t const * >(static_cast< void const * >(pdata)),
                            static_cast< std::size_t >(len));

    PKeyPublic_Ecc::Uptr pPublicKey{std::make_unique< PKeyPublic_Ecc >(stKey)};
    pPublicKey->SetAllowedUsage(GetAllowedUsage());
    pPublicKey->SetExportable(IsExportable());
    pPublicKey->SetSession(IsSession());
    COIdentifier keyId;
    keyId.mCOType = CryptoObjectType::kPublicKey;
    keyId.mCouid  = GetObjectId().mCouid;
    pPublicKey->SetCOIdentifier(keyId);
    return ara::core::Result< PublicKey::Uptrc >::FromValue(std::move(pPublicKey));
}
/// @brief Return the function name: GetPayloadSize
/// @return Function name
ara::core::StringView PKeyPrivate_Ipc_Ecc::GetPayloadSizeFunName() const noexcept
{
    return FUNC_NAME_PrivateKey(GetPayloadSize_Ecc);
}
/// @brief Return the function name: ExportPublicly
/// @return  Function name
ara::core::StringView PKeyPrivate_Ipc_Ecc::ExportPubliclyFunName() const noexcept
{
    return FUNC_NAME_PrivateKey(ExportPrivateKeyPublicly_Ecc);
}
//********************************/

/// @brief Return the CryptoPrimitiveId of this crypto object.
/// @return Instance of CryptoPrimitiveId
CryptoPrimitiveId::Uptr PKeyPrivate_Ipc_Rsa::GetCryptoPrimitiveId() const noexcept
{
    return {std::make_unique< PAlgId_Asymmetric_RsaKey >()};
}

/// @brief Get the public key corresponding to this private key.
/// @return Instance of the public key
ara::core::Result< PublicKey::Uptrc > PKeyPrivate_Ipc_Rsa::GetPublicKey() const noexcept
{
    keys::isoft_def::PIpcAutoPacket aswMsg;  // Receive return packet
    keys::isoft_def::PIpcClient *const ipcClient{keys::isoft_def::PIpcClient::GetInstance_Keys()};
    if (ipcClient == nullptr) {
        return ara::core::Result< PublicKey::Uptrc >::FromValue(nullptr);
    }
    uint32_t const nSlotId{GetSlotId()};
    bool const bDealIpc{
        ipcClient->DealIpcRequest(FUNC_NAME_PrivateKey(GetPublicKey_Rsa), aswMsg,
                                  [nSlotId](keys::isoft_def::PIpcAutoPacket const *const pReqMsg) -> uint16_t {
                                      if (pReqMsg != nullptr) {
                                          keys::isoft_def::cryptoctx::PIpcReq_Common ipcReq;
                                          ipcReq.SetIpcSlotID(nSlotId);
                                          std::ignore = pReqMsg->AddDataToIpc(&ipcReq, sizeof(ipcReq));
                                      }
                                      return sizeof(keys::isoft_def::cryptoctx::PIpcReq_Common);
                                  })};
    if (false == bDealIpc) {
        return ara::core::Result< PublicKey::Uptrc >::FromValue(nullptr);
    }

    keys::isoft_def::cryptoctx::PIpcAsw_GetPublic *const pIpcAsw{
        aswMsg.GetIpcBody< keys::isoft_def::cryptoctx::PIpcAsw_GetPublic >()};
    if (0 != pIpcAsw->GetErrorID()) {
        return ara::core::Result< PublicKey::Uptrc >::FromValue(nullptr);
    }
    uint8_t const *const pdata{pIpcAsw->GetData()};
    int32_t const len{static_cast< int32_t >(pIpcAsw->GetDataLen())};
    ara::core::String stKey(static_cast< ara::crypto::char8_t const * >(static_cast< void const * >(pdata)),
                            static_cast< std::size_t >(len));

    PKeyPublic_Rsa::Uptr pPublicKey{std::make_unique< PKeyPublic_Rsa >(stKey)};
    pPublicKey->SetKeySlotId(nSlotId);
    pPublicKey->SetAllowedUsage(GetAllowedUsage());
    pPublicKey->SetExportable(IsExportable());
    pPublicKey->SetSession(IsSession());
    COIdentifier keyId;
    keyId.mCOType = CryptoObjectType::kPublicKey;
    keyId.mCouid  = GetObjectId().mCouid;
    pPublicKey->SetCOIdentifier(keyId);
    return ara::core::Result< PublicKey::Uptrc >::FromValue(std::move(pPublicKey));
}
/// @brief Return the function name: GetPayloadSize
/// @return Function name
ara::core::StringView PKeyPrivate_Ipc_Rsa::GetPayloadSizeFunName() const noexcept
{
    return FUNC_NAME_PrivateKey(GetPayloadSize_Rsa);
}
/// @brief Return the function name: ExportPublicly
/// @return Function name
ara::core::StringView PKeyPrivate_Ipc_Rsa::ExportPubliclyFunName() const noexcept
{
    return FUNC_NAME_PrivateKey(ExportPrivateKeyPublicly_Rsa);
}

}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara
