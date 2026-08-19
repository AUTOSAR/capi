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
/// @file       isoft_ipc_key_slot.cpp
/// @brief      AutoSar-Crypto Key Storage Module
/// @details
/// @date       2022-08-15
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Default Key Components/Key Slot
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_05004
/// @unit_name=PIpcKeySlot
/// @unit_description=Key slot interface for IPC client
/// @endcode
///
/// ================================================================

#include "ara/crypto/keys/isoft_ipc_key_slot.h"

#include "ara/crypto/common/entry_point.h"
#include "ara/crypto/common/isoft_io_interface_mem.h"
#include "ara/crypto/common/isoft_io_interface_mem_trust.h"

namespace ara {
namespace crypto {
namespace keys {
namespace isoft_def {
//********************************/
/// @brief Key slot interface for IPC client
//********************************/
/// @brief Constructor with parameters
/// @param ipcClient IPC client
/// @param stSlotName Key slot name
/// @param nIpcSlotID IPC key slot ID
PIpcKeySlot::PIpcKeySlot(PIpcClient const &ipcClient,
                         ara::core::StringView const &stSlotName,
                         uint32_t const nIpcSlotID) noexcept
    : KeySlot{}  // NOLINT
    , ipcClient_{ipcClient}
    , stSlotName_{stSlotName}
    , nIpcSlotID_{nIpcSlotID}
{
}
//***************/
/// @brief Retrieve the instance of the CryptoProvider that owns this KeySlot. Any key slot always has an associated default crypto provider that can service this key slot.
///         In the simplest case, all key slots can be serviced by a single crypto provider installed on the adaptive platform.
///         However, in more complex cases, several different crypto providers may coexist in the system,
///         for example, if the ECU has one or more HSMs and software crypto implementations, each with its own physical key storage. In this case, different dedicated crypto providers may service the aforementioned HSMs and software implementations.
/// @return a unique_pointer to the CryptoProvider to be used with this KeySlot
ara::core::Result< cryp::CryptoProvider::Uptr > PIpcKeySlot::MyProvider() const noexcept
{
    /// Request the daemon for the provider InstanceSpecifier via slotname
    uint8_t *pdata{nullptr};
    uint32_t ndataLen{0U};
    PIpcAutoPacket aswMsg;  // Receive response packet
    bool const bDealIpc{ipcClient_.DealIpcRequest(
        FUNC_NAME_KeySlot(MyProvider), aswMsg, [this](PIpcAutoPacket const *const pReqMsg) -> uint16_t {
            if (pReqMsg != nullptr) {
                keyslot::PIpcReq_MyProvider ipcReq;
                ipcReq.nIpcSlotNameLen = static_cast< uint32_t >(stSlotName_.size());
                std::ignore            = pReqMsg->AddDataToIpc(&ipcReq, sizeof(ipcReq));
                std::ignore            = pReqMsg->AddDataToIpc(T_TransBytes(stSlotName_.data()),
                                                    static_cast< uint16_t >(stSlotName_.size()));
            }
            return sizeof(keyslot::PIpcReq_MyProvider);
        })};
    if (true == bDealIpc) {
        keyslot::PIpcAsw_MyProvider *const pIpcAsw{
            static_cast< keyslot::PIpcAsw_MyProvider * >(static_cast< void * >(aswMsg.GetIpcBody()))};
        if (0 == pIpcAsw->GetErrorID()) {
            pdata    = pIpcAsw->GetData();
            ndataLen = pIpcAsw->GetDataLen();
        }
    }

    /// Not found, return error
    if ((pdata == nullptr) || (ndataLen == 0U)) {
        return ara::core::Result< cryp::CryptoProvider::Uptr >::FromError(SecurityErrorDomain::Errc::kLogicFault);
    }

    ara::core::StringView const instance{T_TransChar(pdata), static_cast< size_t >(ndataLen)};
    ara::core::InstanceSpecifier const iSpecify{ara::core::InstanceSpecifier(instance)};
    return ara::core::Result< cryp::CryptoProvider::Uptr >::FromValue(LoadCryptoProvider(iSpecify));
}
/// @brief Clear the content of this key-slot.
/// @return has vlaue if clear sucess false otherwise
ara::core::Result< void > PIpcKeySlot::Clear() noexcept
{
    PIpcAutoPacket aswMsg;  // Receive response packet
    bool const bDealIpc{ipcClient_.DealIpcRequest(FUNC_NAME_KeySlot(Clear), aswMsg,
                                                  [this](PIpcAutoPacket const *const pReqMsg) -> uint16_t {
                                                      if (pReqMsg != nullptr) {
                                                          keyslot::PIpcReq_Clear ipcReq;
                                                          ipcReq.SetIpcSlotID(nIpcSlotID_);
                                                          std::ignore = pReqMsg->AddDataToIpc(&ipcReq, sizeof(ipcReq));
                                                      }
                                                      return sizeof(keyslot::PIpcReq_Clear);
                                                  })};
    if (false == bDealIpc) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kResourceFault);
    }
    keyslot::PIpcAsw_Clear *const pIpcAsw{
        static_cast< keyslot::PIpcAsw_Clear * >(static_cast< void * >(aswMsg.GetIpcBody()))};
    if (0 != pIpcAsw->GetErrorID()) {
        return ara::core::Result< void >::FromError(TransformErrorID(pIpcAsw->GetErrorID()));
    }
    return ara::core::Result< void >::FromValue();
}
/// @brief Check whether the slot is empty.
/// @returns  true if is empty false otherwise
bool PIpcKeySlot::IsEmpty() const noexcept
{
    PIpcAutoPacket aswMsg;  // Receive response packet
    bool const bDealIpc{ipcClient_.DealIpcRequest(FUNC_NAME_KeySlot(IsEmpty), aswMsg,
                                                  [this](PIpcAutoPacket const *const pReqMsg) -> uint16_t {
                                                      if (pReqMsg != nullptr) {
                                                          keyslot::PIpcReq_IsEmpty ipcReq;
                                                          ipcReq.SetIpcSlotID(nIpcSlotID_);
                                                          std::ignore = pReqMsg->AddDataToIpc(&ipcReq, sizeof(ipcReq));
                                                      }
                                                      return sizeof(keyslot::PIpcReq_IsEmpty);
                                                  })};
    if (false == bDealIpc) {
        return false;
    }
    keyslot::PIpcAsw_IsEmpty *const pIpcAsw{
        static_cast< keyslot::PIpcAsw_IsEmpty * >(static_cast< void * >(aswMsg.GetIpcBody()))};
    return pIpcAsw->GetEmpty() != 0U;
}
/// @brief Get the actual attributes of the content in the key slot. If this method is called by the "User" Actor, always: props.exportability == false.
/// @returns Key slot content attribute information
ara::core::Result< KeySlotContentProps > PIpcKeySlot::GetContentProps() const noexcept
{
    /// @param[out] props  the output buffer for storing an actual properties of a content in the key slot
    PIpcAutoPacket aswMsg;  // Receive response packet
    bool const bDealIpc{ipcClient_.DealIpcRequest(FUNC_NAME_KeySlot(GetContentProps), aswMsg,
                                                  [this](PIpcAutoPacket const *const pReqMsg) -> uint16_t {
                                                      if (pReqMsg != nullptr) {
                                                          keyslot::PIpcReq_GetContentProps ipcReq;
                                                          ipcReq.SetIpcSlotID(nIpcSlotID_);
                                                          std::ignore = pReqMsg->AddDataToIpc(&ipcReq, sizeof(ipcReq));
                                                      }
                                                      return sizeof(keyslot::PIpcReq_GetContentProps);
                                                  })};
    if (false == bDealIpc) {
        return ara::core::Result< KeySlotContentProps >::FromError(SecurityErrorDomain::Errc::kResourceFault);
    }
    keyslot::PIpcAsw_GetContentProps *const pIpcAsw{
        static_cast< keyslot::PIpcAsw_GetContentProps * >(static_cast< void * >(aswMsg.GetIpcBody()))};
    if (0 != pIpcAsw->GetErrorID()) {
        return ara::core::Result< KeySlotContentProps >::FromError(TransformErrorID(pIpcAsw->GetErrorID()));
    }
    return ara::core::Result< KeySlotContentProps >::FromValue(pIpcAsw->GetKeySlotContentProps());
}
/// @brief Get the prototype attributes of the key slot.
/// @returns Key slot attribute information
ara::core::Result< KeySlotPrototypeProps > PIpcKeySlot::GetPrototypedProps() const noexcept
{
    /// @param[out] props  the output buffer for storing the prototype properties of the key slot
    PIpcAutoPacket aswMsg;  // Receive response packet
    bool const bDealIpc{ipcClient_.DealIpcRequest(FUNC_NAME_KeySlot(GetPrototypedProps), aswMsg,
                                                  [this](PIpcAutoPacket const *const pReqMsg) -> uint16_t {
                                                      if (pReqMsg != nullptr) {
                                                          keyslot::PIpcReq_GetPrototypedProps ipcReq;
                                                          ipcReq.SetIpcSlotID(nIpcSlotID_);
                                                          std::ignore = pReqMsg->AddDataToIpc(&ipcReq, sizeof(ipcReq));
                                                      }
                                                      return sizeof(keyslot::PIpcReq_GetPrototypedProps);
                                                  })};
    if (false == bDealIpc) {
        return ara::core::Result< KeySlotPrototypeProps >::FromError(SecurityErrorDomain::Errc::kResourceFault);
    }
    keyslot::PIpcAsw_GetPrototypedProps *const pIpcAsw{
        static_cast< keyslot::PIpcAsw_GetPrototypedProps * >(static_cast< void * >(aswMsg.GetIpcBody()))};
    if (0 != pIpcAsw->GetErrorID()) {
        return ara::core::Result< KeySlotPrototypeProps >::FromError(TransformErrorID(pIpcAsw->GetErrorID()));
    }
    return ara::core::Result< KeySlotPrototypeProps >::FromValue(pIpcAsw->GetKeySlotPrototypeProps());
}
//***************/
/// @brief Open this key slot and return an IOInterface to its content.
///         If the UpdatesObserver interface is registered via RegisterObserver(), then each time this slot is updated (and becomes "user" visible), the Key Storage engine (in a dedicated thread) should call the UpdatesObserver::OnUpdate() method.
///         Even after the returned TrustedContainer is destroyed, monitoring of the opened key slot will continue,
///         because the content of the slot may be loaded into volatile memory (as a crypto object or a CryptoContext of a crypto primitive), but after that the TrustedContainer may be destroyed.
///         Because the content of the slot may be loaded into volatile memory (as a crypto object or a CryptoContext of a crypto primitive), but after that, the TrustedContainer may be destroyed.
///         Therefore, if you need to terminate monitoring of a key slot, you should directly call the UnsubscribeObserver(SlotNumber) method.
/// @param subscribeForUpdates Whether to enable update subscription
/// @param writeable Whether to open in write mode
/// @returns  an unique smart pointer to the IOInterface associated with the slot content
ara::core::Result< IOInterface::Uptr > PIpcKeySlot::Open(bool subscribeForUpdates, bool writeable) const noexcept
{
    /// @brief Receive response packet
    PIpcAutoPacket aswMsg;
    bool const bDealIpc{ipcClient_.DealIpcRequest(
        FUNC_NAME_KeySlot(Open), aswMsg,
        [this, subscribeForUpdates, writeable](PIpcAutoPacket const *const pReqMsg) -> uint16_t {
            if (pReqMsg != nullptr) {
                keyslot::PIpcReq_Open ipcReq;
                ipcReq.SetIpcSlotID(nIpcSlotID_);
                ipcReq.SetSubscribeForUpdates(subscribeForUpdates);
                ipcReq.SetWriteable(writeable);
                std::ignore = pReqMsg->AddDataToIpc(&ipcReq, sizeof(ipcReq));
            }
            return sizeof(keyslot::PIpcReq_Open);
        })};
    if (false == bDealIpc) {
        return ara::core::Result< IOInterface::Uptr >::FromError(SecurityErrorDomain::Errc::kResourceFault);
    }
    keyslot::PIpcAsw_Open *const pIpcAsw{
        static_cast< keyslot::PIpcAsw_Open * >(static_cast< void * >(aswMsg.GetIpcBody()))};
    if (0 != pIpcAsw->GetErrorID()) {
        return ara::core::Result< IOInterface::Uptr >::FromError(TransformErrorID(pIpcAsw->GetErrorID()));
    }
    PIoInterface_Ipc::Uptr pIpcInterface{MakeMainIoInterface(pIpcAsw->GetIoInterfaceID())};
    return ara::core::Result< IOInterface::Uptr >::FromValue(std::move(pIpcInterface));
}
/// @brief Save the content of the provided source IOInterface to this key-slot. The source container can represent a volatile trusted container or other KeySlot.
///         This method can be used to atomically update key slots that are within the scope of a transaction. In this case, the slot will only be updated after the corresponding CommitTransaction() call.
/// @brief Save the content of a provided source IOInterface to this key-slot.
///        The source container may represent a volatile trusted container or another KeySlot
///        This method may be used for atomic update of a key slot scoped to some transaction. In such case the
///        the slot will be updated only after correspondent call of @c CommitTransaction().
/// @param container  the source IOInterface
/// @returns @c true if successfully saved
/// @trace_id_sws={SWS_CRYPT_30406}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02004}
/// @error: SecurityErrorDomain::kIncompatibleObject     if the source object has property "session" or if the
///                source IOInterface references a KeySlot from a different CryptoProvider
/// @error: SecurityErrorDomain::kEmptyContainer         if the source IOInterface is empty
/// @error: SecurityErrorDomain::kContentRestrictions    if the source object doesn't satisfy the slot restrictions
/// (including version control)
/// @error: SecurityErrorDomain::kUnreservedResource     if the target slot is not opened writeable.
/// @threadsafety={Thread-safe}
ara::core::Result< void > PIpcKeySlot::SaveCopy(IOInterface const &container) noexcept
{
    /// @error: SecurityErrorDomain::kIncompatibleObject     if the source object has property "session" or if the
    /// source IOInterface references a KeySlot from a different CryptoProvider
    // If the source object has the attribute "session", or the source interface references a key storage from a different crypto provider
    PIpcClient::CB_IpcReqMsg handler;

    PIoInterface_Mem_Trust const *const pIpcMemInterface{dynamic_cast< PIoInterface_Mem_Trust const * >(&container)};
    bool isMemCopy{false};

    if (pIpcMemInterface != nullptr) {
        if (pIpcMemInterface->IsEmpty()) {
            return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kEmptyContainer);
        }
        isMemCopy = true;
        handler   = [this, pIpcMemInterface](PIpcAutoPacket const *const pReqMsg) -> uint16_t {
            CryptoAlgId nKeyAlgID{kAlgIdAny};
            ara::core::Vector< uint8_t > vecKeyKey;
            bool const ret{pIpcMemInterface->ReadKeyKey(nKeyAlgID, vecKeyKey)};
            std::ignore = ret;
            ara::core::Vector< uint8_t > vecKeyData;
            std::ignore = pIpcMemInterface->ReadKeyData(vecKeyData);
            if (pReqMsg != nullptr) {
                keyslot::PIpcReq_SaveCopy_Mem ipcReq;
                ipcReq.SetIpcSlotID(nIpcSlotID_);
                ipcReq.SetCryptoAlgID(nKeyAlgID);
                ipcReq.SetKeySlotContentProps(pIpcMemInterface->GetKeyContent());
                ipcReq.SetKeySlotPrototypeProps(pIpcMemInterface->GetSlotProps());
                ipcReq.SetKeyKeyLen(static_cast< uint16_t >(vecKeyKey.size()));
                ipcReq.SetKeyDataLen(static_cast< uint16_t >(vecKeyData.size()));
                std::ignore = pReqMsg->AddDataToIpc(&ipcReq, sizeof(ipcReq));
                std::ignore = pReqMsg->AddDataToIpc(vecKeyKey.data(), ipcReq.GetKeyKeyLen());
                std::ignore = pReqMsg->AddDataToIpc(vecKeyData.data(), ipcReq.GetKeyDataLen());
            }
            return static_cast< uint16_t >(sizeof(keyslot::PIpcReq_SaveCopy_Mem) + vecKeyKey.size()
                                           + vecKeyData.size());
        };
    } else {
        PIoInterface_Ipc const *const pSrcInterface{dynamic_cast< PIoInterface_Ipc const * >(&container)};
        if (nullptr == pSrcInterface) {
            return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kIncompatibleObject);
        }
        uint32_t const nSrcInterfaceID{pSrcInterface->GetIoInterfaceID()};
        uint32_t const nSrcSlotID{pSrcInterface->GetSlotID()};
        handler = [this, nSrcInterfaceID, nSrcSlotID](PIpcAutoPacket const *const pReqMsg) -> uint16_t {
            if (pReqMsg != nullptr) {
                keyslot::PIpcReq_SaveCopy_Slot ipcReq;
                ipcReq.SetIpcSlotID(nIpcSlotID_);
                ipcReq.SetSaveCopySlotID(nSrcSlotID);
                ipcReq.SetIoInterfaceID(nSrcInterfaceID);
                std::ignore = pReqMsg->AddDataToIpc(&ipcReq, sizeof(ipcReq));
            }
            return sizeof(keyslot::PIpcReq_SaveCopy_Slot);
        };
    }

    PIpcAutoPacket aswMsg;  // Receive response packet
    bool ipcDealRes{false};

    if (isMemCopy) {
        ipcDealRes = ipcClient_.DealIpcRequest(FUNC_NAME_KeySlot(SaveMemory), aswMsg, handler);
    } else {
        ipcDealRes = ipcClient_.DealIpcRequest(FUNC_NAME_KeySlot(SaveCopy), aswMsg, handler);
    }
    if (!ipcDealRes) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kResourceFault);
    }

    int32_t errnoInSrv{0};
    if (isMemCopy) {
        keyslot::PIpcAsw_SaveCopy_Mem *const pIpcAsw{
            static_cast< keyslot::PIpcAsw_SaveCopy_Mem * >(static_cast< void * >(aswMsg.GetIpcBody()))};
        if (0 != pIpcAsw->GetErrorID()) {
            errnoInSrv = pIpcAsw->GetErrorID();
        }
    } else {
        keyslot::PIpcAsw_SaveCopy_Slot *const pIpcAsw{
            static_cast< keyslot::PIpcAsw_SaveCopy_Slot * >(static_cast< void * >(aswMsg.GetIpcBody()))};
        if (0 != pIpcAsw->GetErrorID()) {
            errnoInSrv = pIpcAsw->GetErrorID();
        }
    }

    if (errnoInSrv != 0) {
        return ara::core::Result< void >::FromError(TransformErrorID(errnoInSrv));
    }
    return ara::core::Result< void >::FromValue();
}
/// @brief Save the key passed via PIoInterface_Mem in IPC
/// @param nSlotID Key slot ID
/// @param container IO interface
/// @returns  has value if save memory sucess false otherwise
ara::core::Result< void > PIpcKeySlot::SaveMemory(uint32_t const nSlotID, IOInterface const &container) noexcept
{
    std::ignore = nSlotID;
    /// @error: SecurityErrorDomain::kIncompatibleObject     if the source object has property "session" or if the
    /// source IOInterface references a KeySlot from a different CryptoProvider
    // If the source object has the attribute "session", or the source interface references a key storage from a different crypto provider
    if (false == container.IsObjectSession()) {  // Must be a temporary object
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kIncompatibleObject);
    }
    PIoInterface_Mem const *const pSrcInterface{dynamic_cast< PIoInterface_Mem const * >(&container)};
    if (nullptr == pSrcInterface) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kIncompatibleObject);
    }
    /// @error: SecurityErrorDomain::kEmptyContainer         if the source IOInterface is empty
    if (pSrcInterface->IsEmpty()) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kEmptyContainer);
    }
    // Construct the data packet
    PIpcAutoPacket aswMsg;  // Receive response packet
    bool const bDealIpc{ipcClient_.DealIpcRequest(
        FUNC_NAME_KeySlot(SaveCopy), aswMsg, [this, pSrcInterface](PIpcAutoPacket const *const pReqMsg) -> uint16_t {
            CryptoAlgId nKeyAlgID{kAlgIdAny};
            ara::core::Vector< uint8_t > vecKeyKey;
            bool const ret{pSrcInterface->ReadKeyKey(nKeyAlgID, vecKeyKey)};
            std::ignore = ret;
            ara::core::Vector< uint8_t > vecKeyData;
            std::ignore = pSrcInterface->ReadKeyData(vecKeyData);
            if (pReqMsg != nullptr) {
                keyslot::PIpcReq_SaveCopy_Mem ipcReq;
                ipcReq.SetIpcSlotID(nIpcSlotID_);
                ipcReq.SetCryptoAlgID(nKeyAlgID);
                ipcReq.SetKeySlotContentProps(pSrcInterface->GetKeyContent());
                ipcReq.SetKeySlotPrototypeProps(pSrcInterface->GetSlotProps());
                ipcReq.SetKeyKeyLen(static_cast< uint16_t >(vecKeyKey.size()));
                ipcReq.SetKeyDataLen(static_cast< uint16_t >(vecKeyData.size()));
                std::ignore = pReqMsg->AddDataToIpc(&ipcReq, sizeof(ipcReq));
                std::ignore = pReqMsg->AddDataToIpc(vecKeyKey.data(), ipcReq.GetKeyKeyLen());
                std::ignore = pReqMsg->AddDataToIpc(vecKeyData.data(), ipcReq.GetKeyDataLen());
            }
            return static_cast< uint16_t >(sizeof(keyslot::PIpcReq_SaveCopy_Mem) + vecKeyKey.size()
                                           + vecKeyData.size());
        })};
    if (false == bDealIpc) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kResourceFault);
    }
    keyslot::PIpcAsw_SaveCopy_Mem const *const pIpcAsw{
        static_cast< keyslot::PIpcAsw_SaveCopy_Mem const * >(static_cast< void const * >(aswMsg.GetIpcBody()))};
    if (0 != pIpcAsw->GetErrorID()) {
        return ara::core::Result< void >::FromError(TransformErrorID(pIpcAsw->GetErrorID()));
    }
    return ara::core::Result< void >::FromValue();
}
/// @brief Non-standard interface: Close the key slot. After completing a key modification operation, you must close it; otherwise, the key slot cannot be modified again until the key manager restarts.
/// @return true if close sucess false otherwise
ara::core::Result< bool > PIpcKeySlot::Close() const noexcept
{
    PIpcAutoPacket aswMsg;  // Receive response packet
    bool const bDealIpc{ipcClient_.DealIpcRequest(FUNC_NAME_KeySlot(Close), aswMsg,
                                                  [this](PIpcAutoPacket const *const pReqMsg) -> uint16_t {
                                                      if (pReqMsg != nullptr) {
                                                          keyslot::PIpcReq_Close ipcReq;
                                                          ipcReq.SetIpcSlotID(nIpcSlotID_);
                                                          ipcReq.SetReadOnly(false);
                                                          std::ignore = pReqMsg->AddDataToIpc(&ipcReq, sizeof(ipcReq));
                                                      }
                                                      return sizeof(keyslot::PIpcReq_Open);
                                                  })};
    if (false == bDealIpc) {
        return ara::core::Result< bool >::FromError(SecurityErrorDomain::Errc::kResourceFault);
    }
    keyslot::PIpcAsw_Close *const pIpcAsw{
        static_cast< keyslot::PIpcAsw_Close * >(static_cast< void * >(aswMsg.GetIpcBody()))};
    if (0 != pIpcAsw->GetErrorID()) {
        return ara::core::Result< bool >::FromError(TransformErrorID(pIpcAsw->GetErrorID()));
    }
    return ara::core::Result< bool >::FromValue(pIpcAsw->GetClose());
}
/// @brief Non-standard interface: Get the prototype attributes of the key slot from the configuration file.
/// @returns Key slot attribute information
ara::core::Result< KeySlotPrototypeProps > PIpcKeySlot::GetKeySlotPropsFromJson() const noexcept
{
    /// @param[out] props  the output buffer for storing the prototype properties of the key slot
    PIpcAutoPacket aswMsg;  // Receive response packet
    bool const bDealIpc{ipcClient_.DealIpcRequest(FUNC_NAME_KeySlot(GetKeySlotPropsFromJson), aswMsg,
                                                  [this](PIpcAutoPacket const *const pReqMsg) -> uint16_t {
                                                      if (pReqMsg != nullptr) {
                                                          keyslot::PIpcReq_GetPrototypedProps ipcReq;
                                                          ipcReq.SetIpcSlotID(nIpcSlotID_);
                                                          std::ignore = pReqMsg->AddDataToIpc(&ipcReq, sizeof(ipcReq));
                                                      }
                                                      return sizeof(keyslot::PIpcReq_GetPrototypedProps);
                                                  })};
    if (false == bDealIpc) {
        return ara::core::Result< KeySlotPrototypeProps >::FromError(SecurityErrorDomain::Errc::kResourceFault);
    }
    keyslot::PIpcAsw_GetPrototypedProps *const pIpcAsw{
        static_cast< keyslot::PIpcAsw_GetPrototypedProps * >(static_cast< void * >(aswMsg.GetIpcBody()))};
    if (0 != pIpcAsw->GetErrorID()) {
        return ara::core::Result< KeySlotPrototypeProps >::FromError(TransformErrorID(pIpcAsw->GetErrorID()));
    }
    return ara::core::Result< KeySlotPrototypeProps >::FromValue(pIpcAsw->GetKeySlotPrototypeProps());
}

/// @brief Reset the key slot
/// @param keySlotPrototypeProps Key slot attributes
/// @return  has vlaue if reset keyslot sucess false otherwise
ara::core::Result< void > PIpcKeySlot::ResetKeySlot(KeySlotPrototypeProps const &keySlotPrototypeProps) noexcept
{
    // Construct the data packet
    PIpcAutoPacket aswMsg;  // Receive response packet
    bool const bDealIpc{
        ipcClient_.DealIpcRequest(FUNC_NAME_KeySlot(ResetKeySlot), aswMsg,
                                  [this, keySlotPrototypeProps](PIpcAutoPacket const *const pReqMsg) -> uint16_t {
                                      if (pReqMsg != nullptr) {
                                          CryptoAlgId const nKeyAlgID{kAlgIdAny};
                                          keyslot::PIpcReq_SaveCopy_Mem ipcReq;
                                          ipcReq.SetIpcSlotID(nIpcSlotID_);
                                          ipcReq.SetCryptoAlgID(nKeyAlgID);
                                          ipcReq.SetKeySlotPrototypeProps(keySlotPrototypeProps);
                                          std::ignore = pReqMsg->AddDataToIpc(&ipcReq, sizeof(ipcReq));
                                      }
                                      return static_cast< uint16_t >(sizeof(keyslot::PIpcReq_SaveCopy_Mem));
                                  })};
    if (false == bDealIpc) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kResourceFault);
    }
    keyslot::PIpcAsw_SaveCopy_Mem const *const pIpcAsw{
        static_cast< keyslot::PIpcAsw_SaveCopy_Mem const * >(static_cast< void const * >(aswMsg.GetIpcBody()))};
    if (0 != pIpcAsw->GetErrorID()) {
        return ara::core::Result< void >::FromError(TransformErrorID(pIpcAsw->GetErrorID()));
    }
    return ara::core::Result< void >::FromValue();
}
//********************************/
/// @brief Return the name of the key slot
/// @returns Name of the key slot
ara::core::StringView PIpcKeySlot::GetKeySlotName() const noexcept
{
    return {stSlotName_.data(), stSlotName_.length()};
}
/// @brief Return the PIoInterface_Ipc used by itself
/// @name  MakeMainIoInterface
/// @param nInterfaceID IO interface ID
/// @returns  a unique_pointer to the IOInterface
PIoInterface_Ipc::Uptr PIpcKeySlot::MakeMainIoInterface(uint32_t const nInterfaceID) const noexcept
{
    PIoInterface_Ipc::Uptr pIpcInterface{std::make_unique< PIoInterface_Ipc >(ipcClient_, stSlotName_)};
    pIpcInterface->SetSlotID(nIpcSlotID_, nInterfaceID);
    return pIpcInterface;
}
//********************************/
}  // namespace  isoft_def
}  // namespace keys
}  // namespace crypto
}  // namespace ara
