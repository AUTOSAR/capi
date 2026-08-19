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
/// @file       isoft_key_symmetric_ipc.cpp
/// @brief      AutoSar-Crypto Encryption/Decryption module
/// @details
/// @date       2021-08-22
/// @author     Chang Zheng
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Default encryption decryption/Symmetric key
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_06005
/// @unit_name=PKeySymmetric_Ipc
/// @unit_description=IPC symmetric key
/// @endcode
///
/// ================================================================

#include "ara/crypto/cryp/cryobj/isoft_key_symmetric_ipc.h"

#include "ara/crypto/ipc/isoft_ipc_protocol.h"
#include "ara/crypto/keys/isoft_ipc_io_interface.h"
#include "ara/crypto/keys/isoft_ipc_key_slot.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
//********************************/
/// @brief Parameterized constructor
/// @param nSlotId Key slot ID
/// @param nIointerfaceId IO interface ID
/// @param nSlotName Key slot name
/// @param nAlgId Cryptographic primitive ID
PKeySymmetric_Ipc::PKeySymmetric_Ipc(uint32_t const nSlotId,
                                     uint32_t const nIointerfaceId,
                                     ara::core::StringView const &nSlotName,
                                     CryptoAlgId const nAlgId) noexcept
    : PKeySymmetric_Base{}  // NOLINT
    , nSlotId_{nSlotId}
    , nIointerfaceId_{nIointerfaceId}
    , nSlotName_{nSlotName}
    , nAlgId_{nAlgId}
{
}
/// @brief Return the CryptoPrimitiveId of this crypto object.
/// @return Instance of CryptoPrimitiveId
CryptoPrimitiveId::Uptr PKeySymmetric_Ipc::GetCryptoPrimitiveId() const noexcept
{
    CryptoPrimitiveId::Uptr pReturn{nullptr};
    switch (nAlgId_) {
        case static_cast< CryptoAlgId >(EPhCtxTypeID::kSymmetricAesKey):
        case static_cast< CryptoAlgId >(EPhCtxTypeID::kSymmetricAesEcb):
        case static_cast< CryptoAlgId >(EPhCtxTypeID::kSymmetricAesCbc):
        case static_cast< CryptoAlgId >(EPhCtxTypeID::kSymmetricAesCfb): {
            pReturn = std::make_unique< PAlgId_Symmetric_AesKey >();
            break;
        }
        case static_cast< CryptoAlgId >(EPhCtxTypeID::kSymmetricAesKey128):
        case static_cast< CryptoAlgId >(EPhCtxTypeID::kSymmetricAesCfb1):
        case static_cast< CryptoAlgId >(EPhCtxTypeID::kSymmetricAesCfb8):
        case static_cast< CryptoAlgId >(EPhCtxTypeID::kSymmetricAesCfb128):
        case static_cast< CryptoAlgId >(EPhCtxTypeID::kSymmetricAesOfb):
        case static_cast< CryptoAlgId >(EPhCtxTypeID::kSymmetricAesOfb128): {
            pReturn = std::make_unique< PAlgId_Symmetric_AesKey128 >();
            break;
        }
        case static_cast< CryptoAlgId >(EPhCtxTypeID::kSymmetricAesKey192): {
            pReturn = std::make_unique< PAlgId_Symmetric_AesKey192 >();
            break;
        }
        case static_cast< CryptoAlgId >(EPhCtxTypeID::kSymmetricAesKey256): {
            pReturn = std::make_unique< PAlgId_Symmetric_AesKey256 >();
            break;
        }
        case static_cast< CryptoAlgId >(EPhCtxTypeID::kSymmetricDesKey):
        case static_cast< CryptoAlgId >(EPhCtxTypeID::kSymmetricDesEcb):
        case static_cast< CryptoAlgId >(EPhCtxTypeID::kSymmetricDesCbc):
        case static_cast< CryptoAlgId >(EPhCtxTypeID::kSymmetricDesCfb):
        case static_cast< CryptoAlgId >(EPhCtxTypeID::kSymmetricDesCfb64):
        case static_cast< CryptoAlgId >(EPhCtxTypeID::kSymmetricDesOfb):
        case static_cast< CryptoAlgId >(EPhCtxTypeID::kSymmetricDesOfb64): {
            pReturn = std::make_unique< PAlgId_Symmetric_DesKey >();
            break;
        }
        case static_cast< CryptoAlgId >(EPhCtxTypeID::kSymmetric2DesKey): {
            pReturn = std::make_unique< PAlgId_Symmetric_2DesKey >();
            break;
        }
        case static_cast< CryptoAlgId >(EPhCtxTypeID::kSymmetric3DesKey):
        case static_cast< CryptoAlgId >(EPhCtxTypeID::kSymmetric3DesEcb):
        case static_cast< CryptoAlgId >(EPhCtxTypeID::kSymmetric3DesCbc):
        case static_cast< CryptoAlgId >(EPhCtxTypeID::kSymmetric3DesCfb1):
        case static_cast< CryptoAlgId >(EPhCtxTypeID::kSymmetric3DesCfb64):
        case static_cast< CryptoAlgId >(EPhCtxTypeID::kSymmetric3DesOfb64): {
            pReturn = std::make_unique< PAlgId_Symmetric_3DesKey >();
            break;
        }
        default: {
            break;
        }
    }
    return pReturn;
}
/// @brief Return the COIdentifier of the object, which includes the object type and UID.
///         Objects without an assigned COUID cannot be (safely) serialized/exported or saved to non-volatile storage.
///         Several related objects of different types can share a COUID (e.g., private key and public key), but the combination of COUID and object type must always be unique!
/// @name  GetObjectId
/// @returns  the object's COIdentifier including the object's type and COUID (or an empty COUID, if this object is
///     not identifiable).
CryptoObject::COIdentifier PKeySymmetric_Ipc::GetObjectId() const noexcept
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
/// @brief Return the actual size of the object payload.
/// @return Actual size of the payload
std::size_t PKeySymmetric_Ipc::GetPayloadSize() const noexcept
{
    keys::isoft_def::PIpcAutoPacket aswMsg;  // Receive return packet
    keys::isoft_def::PIpcClient *const ipcClient{keys::isoft_def::PIpcClient::GetInstance_Keys()};
    if (ipcClient == nullptr) {
        return 0U;
    }
    uint32_t const nSlotId{nSlotId_};
    bool const bDealIpc{
        ipcClient->DealIpcRequest(FUNC_NAME_Symmetric(GetPayloadSize), aswMsg,
                                  [nSlotId](keys::isoft_def::PIpcAutoPacket const *const pReqMsg) -> uint16_t {
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
/// @brief Save itself to the provided IOInterface. A crypto object with the attribute "session" cannot be saved in a KeySlot.
/// @param container IO interface
/// @return has value if save sucess false otherwise
ara::core::Result< void > PKeySymmetric_Ipc::Save(IOInterface &container) const noexcept
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
    /// @error: SecurityErrorDomain::kContentRestrictions    if the object doesn't satisfy the slot restrictions
    if (CryptoObjectType::kSymmetricKey != container.GetTypeRestriction()) {
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
//********************************/
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara
