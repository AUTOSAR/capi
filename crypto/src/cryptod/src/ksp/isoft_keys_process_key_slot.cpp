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
/// @file       isoft_keys_process_key_slot.cpp
/// @brief      AutoSar-Crypto Key Storage Module
/// @details
/// @date       2022-08-17
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Key Manager/Key Slot IPC Service
/// @interface_level=module
/// @trace_id_sr=SR_CRYPTO_05004
/// @unit_name=PKeysProcess_KeySlot
/// @unit_description=Key slot logic processing
/// @endcode
///
/// ================================================================

#include "ara/crypto/ksp/isoft_keys_process_key_slot.h"

#include "ara/crypto/common/isoft_log_api.h"
#include "ara/crypto/ksp/isoft_keys_manager.h"

namespace ara {
namespace crypto {
namespace keys {
namespace isoft_def {
/// @brief Extended use of isoft's logging system
using ara::crypto::isoft_def::LogInfo;
//********************************/
/// @brief KeyProvider IPC server-side: key slot logic processing
/// @param lpcProcessManager LCP command processing manager
PKeysProcess_KeySlot::PKeysProcess_KeySlot(PKeys_Manager &lpcProcessManager) noexcept
    : PKeysProcess_T_Base< PKeysProcess_KeySlot >{this, lpcProcessManager}
{
    _InsertMapCmd(FUNC_NAME_KeySlot(Clear), &PKeysProcess_KeySlot::Clear);
    _InsertMapCmd(FUNC_NAME_KeySlot(IsEmpty), &PKeysProcess_KeySlot::IsEmpty);
    _InsertMapCmd(FUNC_NAME_KeySlot(GetContentProps), &PKeysProcess_KeySlot::GetKeyContent);
    _InsertMapCmd(FUNC_NAME_KeySlot(GetPrototypedProps), &PKeysProcess_KeySlot::GetSlotProps);
    _InsertMapCmd(FUNC_NAME_KeySlot(Open), &PKeysProcess_KeySlot::Open);
    _InsertMapCmd(FUNC_NAME_KeySlot(Close), &PKeysProcess_KeySlot::Close);
    _InsertMapCmd(FUNC_NAME_KeySlot(SaveCopy), &PKeysProcess_KeySlot::SaveCopy);
    _InsertMapCmd(FUNC_NAME_KeySlot(SaveMemory), &PKeysProcess_KeySlot::SaveMemory);
    _InsertMapCmd(FUNC_NAME_KeySlot(MyProvider), &PKeysProcess_KeySlot::MyProvider);
    _InsertMapCmd(FUNC_NAME_KeySlot(GetKeySlotPropsFromJson), &PKeysProcess_KeySlot::GetKeySlotPropsFromJson);
    _InsertMapCmd(FUNC_NAME_KeySlot(ResetKeySlot), &PKeysProcess_KeySlot::ResetKeySlot);
}
//********************************/
/// @brief Clears the content of this key-slot
/// @name  Clear
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
/// @returns has vlaue if Close sucess false otherwise
PKeysProcess_Base::PResultLen PKeysProcess_KeySlot::Clear(PIpcPac_Head const *const pReqHead,
                                                          PIpcAutoPacket &aswMsg) const noexcept
{
    PSvrKeySlotLoader *const pFindSlotLoader{_FindSlotLoaderByReqHead(pReqHead)};
    if (nullptr == pFindSlotLoader) {
        return PResultLen::FromError(SecurityErrorDomain::Errc::kUnreservedResource);
    }

    // Build response packet
    uint16_t const nAswLen{static_cast< uint16_t >(pReqHead->GetHeadLen() + sizeof(keyslot::PIpcAsw_Clear))};
    aswMsg.CreatePacket(nAswLen);
    std::ignore = aswMsg.InitIpcHead(pReqHead, nAswLen);
    keyslot::PIpcAsw_Clear *const pPacAsw{aswMsg.GetIpcBody< keyslot::PIpcAsw_Clear >()};
    pPacAsw->SetErrorID(0);
    // Logic processing
    ara::core::Result< void > const result{pFindSlotLoader->Clear(pReqHead->nProcessID)};
    if (false == result.HasValue()) {
        pPacAsw->SetErrorID(result.Error().Value());
    }

    return PResultLen::FromValue(pReqHead->nPacSize);
}
/// @brief Checks if the slot is empty
/// @name  IsEmpty
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
/// @returns  has vlaue if Close sucess false otherwise
PKeysProcess_Base::PResultLen PKeysProcess_KeySlot::IsEmpty(PIpcPac_Head const *const pReqHead,
                                                            PIpcAutoPacket &aswMsg) const noexcept
{
    PSvrKeySlotLoader *const pFindSlotLoader{_FindSlotLoaderByReqHead(pReqHead)};
    if (nullptr == pFindSlotLoader) {
        return PResultLen::FromError(SecurityErrorDomain::Errc::kUnreservedResource);
    }
    // Build response packet
    uint16_t const nAswLen{static_cast< uint16_t >(pReqHead->GetHeadLen() + sizeof(keyslot::PIpcAsw_IsEmpty))};
    aswMsg.CreatePacket(nAswLen);
    std::ignore = aswMsg.InitIpcHead(pReqHead, nAswLen);
    keyslot::PIpcAsw_IsEmpty *const pPacAsw{aswMsg.GetIpcBody< keyslot::PIpcAsw_IsEmpty >()};
    pPacAsw->SetErrorID(0);
    // Logic processing
    ara::core::Result< bool > const result{pFindSlotLoader->IsEmpty()};
    if (false == result.HasValue()) {
        pPacAsw->SetErrorID(result.Error().Value());
    } else {
        if (result.Value() == true) {
            pPacAsw->SetEmpty(1U);
        } else {
            pPacAsw->SetEmpty(0U);
        }
    }
    return PResultLen::FromValue(pReqHead->nPacSize);
}
/// @brief Gets the actual properties of the content in the key slot. If this method is called by a "User" Actor, then always: props.exportability == false.
/// @name   GetKeyContent
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
/// @returns  has vlaue if Close sucess false otherwise
PKeysProcess_Base::PResultLen PKeysProcess_KeySlot::GetKeyContent(PIpcPac_Head const *const pReqHead,
                                                                  PIpcAutoPacket &aswMsg) const noexcept
{
    PSvrKeySlotLoader *const pFindSlotLoader{_FindSlotLoaderByReqHead(pReqHead)};
    if (nullptr == pFindSlotLoader) {
        return PResultLen::FromError(SecurityErrorDomain::Errc::kUnreservedResource);
    }
    // Build response packet
    uint16_t const nAswLen{static_cast< uint16_t >(pReqHead->GetHeadLen() + sizeof(keyslot::PIpcAsw_GetContentProps))};
    aswMsg.CreatePacket(nAswLen);
    std::ignore = aswMsg.InitIpcHead(pReqHead, nAswLen);
    keyslot::PIpcAsw_GetContentProps *const pPacAsw{aswMsg.GetIpcBody< keyslot::PIpcAsw_GetContentProps >()};
    pPacAsw->SetErrorID(0);
    // Logic processing
    ara::core::Result< KeySlotContentProps > const result{pFindSlotLoader->GetKeyContent()};
    if (false == result.HasValue()) {
        pPacAsw->SetErrorID(result.Error().Value());
    } else {
        pPacAsw->SetKeySlotContentProps(result.Value());
    }
    return PResultLen::FromValue(pReqHead->nPacSize);
}
/// @brief Gets the prototype properties of the key slot.
/// @name   GetSlotProps
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
/// @returns  has vlaue if Close sucess false otherwise
PKeysProcess_Base::PResultLen PKeysProcess_KeySlot::GetSlotProps(PIpcPac_Head const *const pReqHead,
                                                                 PIpcAutoPacket &aswMsg) const noexcept
{
    PSvrKeySlotLoader *const pFindSlotLoader{_FindSlotLoaderByReqHead(pReqHead)};
    if (nullptr == pFindSlotLoader) {
        return PResultLen::FromError(SecurityErrorDomain::Errc::kUnreservedResource);
    }
    // Build response packet
    uint16_t const nAswLen{
        static_cast< uint16_t >(pReqHead->GetHeadLen() + sizeof(keyslot::PIpcAsw_GetPrototypedProps))};
    aswMsg.CreatePacket(nAswLen);
    std::ignore = aswMsg.InitIpcHead(pReqHead, nAswLen);
    keyslot::PIpcAsw_GetPrototypedProps *const pPacAsw{aswMsg.GetIpcBody< keyslot::PIpcAsw_GetPrototypedProps >()};
    pPacAsw->SetErrorID(0);
    // Logic processing
    ara::core::Result< KeySlotPrototypeProps > const result{pFindSlotLoader->GetSlotProps()};
    if (false == result.HasValue()) {
        pPacAsw->SetErrorID(result.Error().Value());
    } else {
        pPacAsw->SetKeySlotPrototypeProps(result.Value());
    }
    return PResultLen::FromValue(pReqHead->nPacSize);
}
/// @brief Opens this key slot and returns an IOInterface to its content.
/// @name   Open
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
/// @returns  has vlaue if Close sucess false otherwise
PKeysProcess_Base::PResultLen PKeysProcess_KeySlot::Open(PIpcPac_Head const *const pReqHead,
                                                         PIpcAutoPacket &aswMsg) const noexcept
{
    PSvrKeySlotLoader *const pFindSlotLoader{_FindSlotLoaderByReqHead(pReqHead)};
    if (nullptr == pFindSlotLoader) {
        return PResultLen::FromError(SecurityErrorDomain::Errc::kUnreservedResource);
    }
    keyslot::PIpcReq_Open const *const pPacReq{pReqHead->GetBody< keyslot::PIpcReq_Open >()};
    // Build response packet
    uint16_t const nAswLen{static_cast< uint16_t >(pReqHead->GetHeadLen() + sizeof(keyslot::PIpcAsw_Open))};
    aswMsg.CreatePacket(nAswLen);
    std::ignore = aswMsg.InitIpcHead(pReqHead, nAswLen);
    keyslot::PIpcAsw_Open *const pPacAsw{aswMsg.GetIpcBody< keyslot::PIpcAsw_Open >()};
    pPacAsw->SetErrorID(0);
    // Logic processing
    ara::core::Result< uint32_t > const result{
        pFindSlotLoader->Open(pReqHead->nProcessID, pPacReq->GetSubscribeForUpdates(), pPacReq->GetWriteable())};
    if (false == result.HasValue()) {
        pPacAsw->SetErrorID(result.Error().Value());
    } else {
        pPacAsw->SetIoInterfaceID(result.Value());
    }
    return PResultLen::FromValue(pReqHead->nPacSize);
}
/// @brief Specific process closes read/write for the key slot
/// @name   Close
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
/// @returns has vlaue if Close sucess false otherwise
PKeysProcess_Base::PResultLen PKeysProcess_KeySlot::Close(PIpcPac_Head const *const pReqHead,
                                                          PIpcAutoPacket &aswMsg) const noexcept
{
    PSvrKeySlotLoader *const pFindSlotLoader{_FindSlotLoaderByReqHead(pReqHead)};
    if (nullptr == pFindSlotLoader) {
        return PResultLen::FromError(SecurityErrorDomain::Errc::kUnreservedResource);
    }
    keyslot::PIpcReq_Close const *const pPacReq{pReqHead->GetBody< keyslot::PIpcReq_Close >()};
    // Build response packet
    uint16_t const nAswLen{static_cast< uint16_t >(pReqHead->GetHeadLen() + sizeof(keyslot::PIpcAsw_Close))};
    aswMsg.CreatePacket(nAswLen);
    std::ignore = aswMsg.InitIpcHead(pReqHead, nAswLen);
    keyslot::PIpcAsw_Close *const pPacAsw{aswMsg.GetIpcBody< keyslot::PIpcAsw_Close >()};
    pPacAsw->SetErrorID(0);
    // Logic processing
    ara::core::Result< bool > const result{pFindSlotLoader->Close(pReqHead->nProcessID, pPacReq->GetReadOnly())};
    if (false == result.HasValue()) {
        pPacAsw->SetErrorID(result.Error().Value());
    } else {
        pPacAsw->SetClose(result.Value());
    }
    return PResultLen::FromValue(pReqHead->nPacSize);
}
/// @brief Saves the content of the provided source IOInterface to this key-slot
/// @name   SaveCopy
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
/// @returns  has vlaue if Close sucess false otherwise
PKeysProcess_Base::PResultLen PKeysProcess_KeySlot::SaveCopy(PIpcPac_Head const *const pReqHead,
                                                             PIpcAutoPacket &aswMsg) const noexcept
{
    PSvrKeySlotLoader *const pDestSlotLoader{_FindSlotLoaderByReqHead(pReqHead)};
    if (nullptr == pDestSlotLoader) {
        return PResultLen::FromError(SecurityErrorDomain::Errc::kUnreservedResource);
    }

    keyslot::PIpcReq_SaveCopy_Slot const *const pPacReq{pReqHead->GetBody< keyslot::PIpcReq_SaveCopy_Slot >()};
    /// FindSlotLoader corresponding to the slot to copy data from
    PSvrKeySlotLoader *const pSrcSlotLoader{_FindSlotLoader(pPacReq->GetSaveCopySlotID())};
    if (nullptr == pSrcSlotLoader) {
        return PResultLen::FromError(SecurityErrorDomain::Errc::kUnreservedResource);
    }
    PSvrIoInterfaceAgent const *const pAgentInterface{
        pSrcSlotLoader->FindIoInterface(pPacReq->GetSaveCopySlotID(), pPacReq->GetIoInterfaceID())};
    if (nullptr == pAgentInterface) {
        return PResultLen::FromError(SecurityErrorDomain::Errc::kIncompatibleObject);
    }
    PIoInterface *const pSrcInterface{pAgentInterface->GetIoInterface()};

    /// @brief Logic processing
    ara::core::Result< bool > const res{pDestSlotLoader->SaveCopy(pSrcInterface)};

    // Build response packet
    uint16_t const nAswLen{static_cast< uint16_t >(pReqHead->GetHeadLen() + sizeof(keyslot::PIpcAsw_SaveCopy_Slot))};
    aswMsg.CreatePacket(nAswLen);
    std::ignore = aswMsg.InitIpcHead(pReqHead, nAswLen);
    keyslot::PIpcAsw_SaveCopy_Slot *const pPacAsw{aswMsg.GetIpcBody< keyslot::PIpcAsw_SaveCopy_Slot >()};
    pPacAsw->SetErrorID(0);
    if (false == res.HasValue()) {
        pPacAsw->SetErrorID(res.Error().Value());
    } else {
        pPacAsw->SetIpcSlotID(pDestSlotLoader->GetSlotID());
        if (res.Value() == true) {
            pPacAsw->SetIoInterfaceID(1U);
        } else {
            pPacAsw->SetIoInterfaceID(0U);
        }
    }
    return PResultLen::FromValue(pReqHead->nPacSize);
}
/// @brief Saves the key passed in via PIoInterface_Mem via IPC
/// @name   SaveMemory
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
/// @returns  has vlaue if Close sucess false otherwise
PKeysProcess_Base::PResultLen PKeysProcess_KeySlot::SaveMemory(PIpcPac_Head const *const pReqHead,
                                                               PIpcAutoPacket &aswMsg) const noexcept
{
    PSvrKeySlotLoader *const pFindSlotLoader{_FindSlotLoaderByReqHead(pReqHead)};
    if (nullptr == pFindSlotLoader) {
        return PResultLen::FromError(SecurityErrorDomain::Errc::kUnreservedResource);
    }

    ara::core::Result< bool > res{false};
    keyslot::PIpcReq_SaveCopy_Mem const *const pPacReq{pReqHead->GetBody< keyslot::PIpcReq_SaveCopy_Mem >()};
    if (pPacReq != nullptr) {
        res = pFindSlotLoader->SaveCopy(pPacReq->GetIpcSlotID(), pPacReq);
    }

    // Build response packet
    uint16_t const nAswLen{static_cast< uint16_t >(pReqHead->GetHeadLen() + sizeof(keyslot::PIpcAsw_SaveCopy_Mem))};
    aswMsg.CreatePacket(nAswLen);
    std::ignore = aswMsg.InitIpcHead(pReqHead, nAswLen);
    keyslot::PIpcAsw_SaveCopy_Mem *const pPacAsw{aswMsg.GetIpcBody< keyslot::PIpcAsw_SaveCopy_Mem >()};
    pPacAsw->SetErrorID(0);
    if (false == res.HasValue()) {
        pPacAsw->SetErrorID(res.Error().Value());
    } else {
        if (res.Value() == false) {
            pPacAsw->SetErrorID(-1);
        } else {
            pPacAsw->SetIpcSlotID(pFindSlotLoader->GetSlotID());
        }
    }
    return PResultLen::FromValue(pReqHead->nPacSize);
}

//********************************/
/// @brief Gets the provider corresponding to the key slot
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
/// @return has vlaue if Close sucess false otherwise
PKeysProcess_Base::PResultLen PKeysProcess_KeySlot::MyProvider(PIpcPac_Head const *const pReqHead,
                                                               PIpcAutoPacket &aswMsg) const noexcept
{
    keyslot::PIpcReq_MyProvider const *const pReqMsg{pReqHead->GetBody< keyslot::PIpcReq_MyProvider >()};
    if (pReqMsg == nullptr) {
        return PResultLen::FromError(SecurityErrorDomain::Errc::kUnreservedResource);
    }

    ara::core::StringView const nSlotName{
        static_cast< ara::crypto::char8_t const * >(static_cast< void const * >(pReqMsg->GetData())),
        static_cast< size_t >(pReqMsg->nIpcSlotNameLen)};
    if (nSlotName.empty()) {
        return PResultLen::FromError(SecurityErrorDomain::Errc::kUnreservedResource);
    }

    ara::core::String const providerIns{_FindCryptoProviderBySlotName(nSlotName)};
    size_t const providerInsSize{providerIns.size()};
    // Build response packet
    uint16_t const nAswLen{static_cast< uint16_t >(static_cast< size_t >(pReqHead->GetHeadLen())
                                                   + sizeof(keyslot::PIpcAsw_MyProvider)
                                                   + static_cast< size_t >(providerInsSize))};
    aswMsg.CreatePacket(nAswLen);
    std::ignore = aswMsg.InitIpcHead(pReqHead, nAswLen);
    keyslot::PIpcAsw_MyProvider *const pPacAsw{aswMsg.GetIpcBody< keyslot::PIpcAsw_MyProvider >()};
    if (providerInsSize == 0U) {
        pPacAsw->SetErrorID(-1);
    } else {
        pPacAsw->SetErrorID(0);
        pPacAsw->SetDataLen(static_cast< uint32_t >(providerInsSize));
        std::ignore
            = memcpy(static_cast< uint8_t * >(static_cast< void * >(pPacAsw + 1)), providerIns.data(), providerInsSize);
    }

    return PResultLen::FromValue(pReqHead->nPacSize);
}
/// @brief Gets the prototype properties of the key slot.
/// @name   GetKeySlotPropsFromJson
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
/// @returns  PKeysProcess_Base::PResultLen
/// @throws
PKeysProcess_Base::PResultLen PKeysProcess_KeySlot::GetKeySlotPropsFromJson(PIpcPac_Head const *const pReqHead,
                                                                            PIpcAutoPacket &aswMsg) const noexcept
{
    PSvrKeySlotLoader *const pFindSlotLoader{_FindSlotLoaderByReqHead(pReqHead)};
    if (nullptr == pFindSlotLoader) {
        return PResultLen::FromError(SecurityErrorDomain::Errc::kUnreservedResource);
    }
    ara::core::StringView const stNewSlotName{pFindSlotLoader->GetKeySlotName()};

    keys::KeySlotPrototypeProps::Uptr const pFindSlotConfig{_FindConfigKeySlotProps(stNewSlotName)};
    if (nullptr == pFindSlotConfig) {
        return PResultLen::FromError(SecurityErrorDomain::Errc::kUnreservedResource);
    }
    // Build response packet
    uint16_t const nAswLen{
        static_cast< uint16_t >(pReqHead->GetHeadLen() + sizeof(keyslot::PIpcAsw_GetPrototypedProps))};
    aswMsg.CreatePacket(nAswLen);
    std::ignore = aswMsg.InitIpcHead(pReqHead, nAswLen);
    keyslot::PIpcAsw_GetPrototypedProps *const pPacAsw{aswMsg.GetIpcBody< keyslot::PIpcAsw_GetPrototypedProps >()};
    pPacAsw->SetErrorID(0);
    // Logic processing
    pPacAsw->SetKeySlotPrototypeProps(*pFindSlotConfig);

    return PResultLen::FromValue(pReqHead->nPacSize);
}

/// @brief Resets key slot information with key slot property information from the configuration file (clears key slot content)
/// @name   ResetKeySlot
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
/// @returns  has vlaue if Close sucess false otherwise
PKeysProcess_Base::PResultLen PKeysProcess_KeySlot::ResetKeySlot(PIpcPac_Head const *const pReqHead,
                                                                 PIpcAutoPacket &aswMsg) const noexcept
{
    PSvrKeySlotLoader *const pDestSlotLoader{_FindSlotLoaderByReqHead(pReqHead)};
    if (nullptr == pDestSlotLoader) {
        return PResultLen::FromError(SecurityErrorDomain::Errc::kUnreservedResource);
    }

    keyslot::PIpcReq_SaveCopy_Mem const *const pPacReq{pReqHead->GetBody< keyslot::PIpcReq_SaveCopy_Mem >()};
    KeySlotPrototypeProps const keySlotPrototypeProps{pPacReq->GetKeySlotPrototypeProps()};

    /// @brief Logic processing
    ara::core::Result< bool > const res{pDestSlotLoader->ResetKeySlot(keySlotPrototypeProps)};

    // Build response packet
    uint16_t const nAswLen{static_cast< uint16_t >(pReqHead->GetHeadLen() + sizeof(keyslot::PIpcAsw_SaveCopy_Slot))};
    aswMsg.CreatePacket(nAswLen);
    std::ignore = aswMsg.InitIpcHead(pReqHead, nAswLen);
    keyslot::PIpcAsw_SaveCopy_Slot *const pPacAsw{aswMsg.GetIpcBody< keyslot::PIpcAsw_SaveCopy_Slot >()};
    pPacAsw->SetErrorID(0);
    if (false == res.HasValue()) {
        pPacAsw->SetErrorID(res.Error().Value());
    } else {
        pPacAsw->SetIpcSlotID(pDestSlotLoader->GetSlotID());
        if (res.Value() == true) {
            pPacAsw->SetIoInterfaceID(1U);
        } else {
            pPacAsw->SetIoInterfaceID(0U);
        }
    }
    return PResultLen::FromValue(pReqHead->nPacSize);
}
//********************************/
/// @brief Finds slotLoader
/// @name  _FindSlotLoaderByReqHead
/// @param pReqHead IPC request packet header
/// @returns  PSvrKeySlotLoader pointer
PSvrKeySlotLoader *PKeysProcess_KeySlot::_FindSlotLoaderByReqHead(PIpcPac_Head const *const pReqHead) const noexcept
{
    if (nullptr == pReqHead) {
        return nullptr;
    }
    keyslot::PIpcReq_SlotBase const *const pReqMsg{pReqHead->GetBody< keyslot::PIpcReq_SlotBase >()};
    return _FindSlotLoader(pReqMsg->GetIpcSlotID());
}
//********************************/
}  // namespace  isoft_def
}  // namespace keys
}  // namespace crypto
}  // namespace ara
