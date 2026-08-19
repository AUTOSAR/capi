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
/// @file       isoft_keys_process_provider.cpp
/// @brief      AutoSar-Crypto Key Storage Module
/// @details
/// @date       2022-08-17
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Key Manager/Key Provider IPC Service
/// @interface_level=module
/// @trace_id_sr=SR_CRYPTO_05001
/// @unit_name=PSvrProcess_KeyProvider
/// @unit_description=Key provider IPC service
/// @endcode
///
/// ================================================================

#include "ara/crypto/ksp/isoft_keys_process_provider.h"

#include "ara/crypto/common/isoft_log_api.h"
#include "ara/crypto/ksp/isoft_keys_manager.h"

namespace ara {
namespace crypto {
namespace keys {
namespace isoft_def {
//********************************/
/// @brief Extended use of isoft's logging system
using ara::crypto::isoft_def::LogInfo;
//********************************/
/// @brief Constructor
/// @param lpcProcessManager LCP command processing manager
PSvrProcess_KeyProvider::PSvrProcess_KeyProvider(PKeys_Manager &lpcProcessManager) noexcept
    : PKeysProcess_T_Base< PSvrProcess_KeyProvider >{this, lpcProcessManager}
{
    _InsertMapCmd(FUNC_NAME_KeyProvider(LoadKeySlot), &PSvrProcess_KeyProvider::LoadKeySlot);
    _InsertMapCmd(FUNC_NAME_KeyProvider(BeginTransaction), &PSvrProcess_KeyProvider::BeginTransaction);
    _InsertMapCmd(FUNC_NAME_KeyProvider(CommitTransaction), &PSvrProcess_KeyProvider::CommitTransaction);
    _InsertMapCmd(FUNC_NAME_KeyProvider(RollbackTransaction), &PSvrProcess_KeyProvider::RollbackTransaction);
    _InsertMapCmd(FUNC_NAME_KeyProvider(GetRegisteredObserver), &PSvrProcess_KeyProvider::GetRegisteredObserver);
    _InsertMapCmd(FUNC_NAME_KeyProvider(RegisterObserver), &PSvrProcess_KeyProvider::RegisterObserver);
    _InsertMapCmd(FUNC_NAME_KeyProvider(UnsubscribeObserver), &PSvrProcess_KeyProvider::UnsubscribeObserver);
    _InsertMapCmd(FUNC_NAME_KeyProvider(FindKeySlot), &PSvrProcess_KeyProvider::FindKeySlot);
}
//********************************/
/// @brief Loads key slot
PKeysProcess_Base::PResultLen
/// @name  LoadKeySlot
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
/// @returns  slotID if sucess
PSvrProcess_KeyProvider::LoadKeySlot(PIpcPac_Head const *const pReqHead, PIpcAutoPacket &aswMsg) const noexcept
{
    if (nullptr == pReqHead) {
        return PResultLen::FromError(SecurityErrorDomain::Errc::kIpcFault);  // IPC parameter error
    }
    keys::isoft_def::keyprovider::PIpcReq_LoadKeySlot const *const pPacReq{
        pReqHead->GetBody< keys::isoft_def::keyprovider::PIpcReq_LoadKeySlot >()};
    ara::core::StringView const stSlotName{pPacReq->GetName()};

    // 2022-08-11 Temporary simplified processing
    // Get process ID from packet body
    uint64_t nRealProcessId{pPacReq->GetPid()};
    if (0 == nRealProcessId) {
        // Process ID is 0, get thread ID from packet header
        nRealProcessId = pReqHead->nProcessID;
    }
    ara::core::Result< PSvrKeySlotLoader * > const resultKeySlot{
        _PrepareKeySlotLoader(static_cast< uint32_t >(nRealProcessId), stSlotName)};
    if (false == resultKeySlot.HasValue()) {
        return ProcessCmd_Error(pReqHead, aswMsg, resultKeySlot.Error().Value());
    }
    PSvrKeySlotLoader *const pKeySlotLoader{resultKeySlot.Value()};
    // If slot exists but is not ready, initialize it with configuration data
    ara::core::Result< bool > const nErrorCode{pKeySlotLoader->IsReady(false)};
    if (!nErrorCode.HasValue()) {
        return ProcessCmd_Error(pReqHead, aswMsg, nErrorCode.Error().Value());
    }
    // Convert local KeySlot to network data and send out
    uint16_t const nAswLen{
        static_cast< uint16_t >(pReqHead->GetHeadLen() + sizeof(keys::isoft_def::keyprovider::PIpcAsw_LoadKeySlot))};
    aswMsg.CreatePacket(nAswLen);
    std::ignore = aswMsg.InitIpcHead(pReqHead, nAswLen);
    keys::isoft_def::keyprovider::PIpcAsw_LoadKeySlot *const pPacAsw{
        static_cast< keys::isoft_def::keyprovider::PIpcAsw_LoadKeySlot * >(static_cast< void * >(aswMsg.GetIpcBody()))};
    pPacAsw->SetErrorID(0);
    pPacAsw->SetIpcSlotID(pKeySlotLoader->GetSlotID());
    return PResultLen::FromValue(pReqHead->nPacSize);
}
//********************************/
/// @brief Starts transaction
PKeysProcess_Base::PResultLen
/// @name  BeginTransaction
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
/// @returns  Transaction ID if sucess
PSvrProcess_KeyProvider::BeginTransaction(PIpcPac_Head const *const pReqHead, PIpcAutoPacket &aswMsg) const noexcept
{
    if (nullptr == pReqHead) {
        return PResultLen::FromError(SecurityErrorDomain::Errc::kIpcFault);
    }
    keys::isoft_def::keyprovider::PIpcReq_BeginTransaction const *const pPacReq{
        pReqHead->GetBody< keys::isoft_def::keyprovider::PIpcReq_BeginTransaction >()};
    uint32_t const *const pSlotID{static_cast< uint32_t const * >(static_cast< void const * >(pPacReq + 1))};
    // Processing logic: transaction start
    ara::core::Result< uint64_t > const result{
        _AddNewTransaction(static_cast< uint32_t >(pReqHead->nProcessID), pSlotID, pPacReq->nSlotCount)};
    if (false == result.HasValue()) {
        return PResultLen::FromError(static_cast< SecurityErrorDomain::Errc >(result.Error().Value()));
    }
    uint16_t const nAswLen{static_cast< uint16_t >(pReqHead->GetHeadLen()
                                                   + sizeof(keys::isoft_def::keyprovider::PIpcAsw_BeginTransaction))};
    aswMsg.CreatePacket(nAswLen);
    std::ignore = aswMsg.InitIpcHead(pReqHead, nAswLen);
    keys::isoft_def::keyprovider::PIpcAsw_BeginTransaction *const pPacAsw{
        static_cast< keys::isoft_def::keyprovider::PIpcAsw_BeginTransaction * >(
            static_cast< void * >(aswMsg.GetIpcBody()))};
    pPacAsw->SetErrorID(0);
    pPacAsw->SetTransactionId(result.Value());
    return PResultLen::FromValue(pReqHead->nPacSize);
}
/// @brief Commits transaction
PKeysProcess_Base::PResultLen
/// @name  CommitTransaction
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
/// @returns  has vlaue if sucess
PSvrProcess_KeyProvider::CommitTransaction(PIpcPac_Head const *const pReqHead, PIpcAutoPacket &aswMsg) const noexcept
{
    if (nullptr == pReqHead) {
        return PResultLen::FromError(SecurityErrorDomain::Errc::kIpcFault);
    }
    keys::isoft_def::keyprovider::PIpcReq_CommitTransaction const *const pPacReq{
        pReqHead->GetBody< keys::isoft_def::keyprovider::PIpcReq_CommitTransaction >()};
    // Processing logic:
    ara::core::Result< uint32_t > const result{_CommitTransaction(pPacReq->GetTransactionId())};
    if (false == result.HasValue()) {
        return PResultLen::FromError(static_cast< SecurityErrorDomain::Errc >(result.Error().Value()));
    }

    uint16_t const nAswLen{static_cast< uint16_t >(pReqHead->GetHeadLen()
                                                   + sizeof(keys::isoft_def::keyprovider::PIpcAsw_CommitTransaction))};
    aswMsg.CreatePacket(nAswLen);
    std::ignore = aswMsg.InitIpcHead(pReqHead, nAswLen);
    keys::isoft_def::keyprovider::PIpcAsw_CommitTransaction *const pPacAsw{
        static_cast< keys::isoft_def::keyprovider::PIpcAsw_CommitTransaction * >(
            static_cast< void * >(aswMsg.GetIpcBody()))};
    pPacAsw->SetErrorID(0);
    pPacAsw->SetSuccessCount(result.Value());
    return PResultLen::FromValue(pReqHead->nPacSize);
}
/// @brief Rolls back transaction
PKeysProcess_Base::PResultLen
/// @name  RollbackTransaction
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
/// @returns  has value if suecess
PSvrProcess_KeyProvider::RollbackTransaction(PIpcPac_Head const *const pReqHead, PIpcAutoPacket &aswMsg) const noexcept
{
    if (nullptr == pReqHead) {
        return PResultLen::FromError(SecurityErrorDomain::Errc::kIpcFault);
    }
    keys::isoft_def::keyprovider::PIpcReq_RollbackTransaction const *const pPacReq{
        pReqHead->GetBody< keys::isoft_def::keyprovider::PIpcReq_RollbackTransaction >()};
    // Processing logic:
    ara::core::Result< uint32_t > const result{
        _RollbackTransaction(static_cast< uint64_t >(pPacReq->GetTransactionId()))};
    if (false == result.HasValue()) {
        return PResultLen::FromError(static_cast< SecurityErrorDomain::Errc >(result.Error().Value()));
    }

    uint16_t const nAswLen{static_cast< uint16_t >(
        pReqHead->GetHeadLen() + sizeof(keys::isoft_def::keyprovider::PIpcAsw_RollbackTransaction))};
    aswMsg.CreatePacket(nAswLen);
    std::ignore = aswMsg.InitIpcHead(pReqHead, nAswLen);
    keys::isoft_def::keyprovider::PIpcAsw_RollbackTransaction *const pPacAsw{
        static_cast< keys::isoft_def::keyprovider::PIpcAsw_RollbackTransaction * >(
            static_cast< void * >(aswMsg.GetIpcBody()))};
    pPacAsw->SetErrorID(0);
    pPacAsw->SetSuccessCount(result.Value());
    return PResultLen::FromValue(pReqHead->nPacSize);
}
//********************************/
/// @brief Gets the pointer to register update observer.
PKeysProcess_Base::PResultLen
/// @name  GetRegisteredObserver
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
/// @returns ObserverID if get sucess
PSvrProcess_KeyProvider::GetRegisteredObserver(PIpcPac_Head const *const pReqHead,
                                               PIpcAutoPacket &aswMsg) const noexcept
{
    if (nullptr == pReqHead) {
        return PResultLen::FromError(SecurityErrorDomain::Errc::kIpcFault);
    }
    keys::isoft_def::keyprovider::PIpcReq_GetRegisteredObserver const *const pPacReq{
        pReqHead->GetBody< keys::isoft_def::keyprovider::PIpcReq_GetRegisteredObserver >()};
    // Processing logic:
    uint64_t const nFindObserver{_FindUpdateObserver(pPacReq->nRegProcessID)};
    uint16_t const nAswLen{static_cast< uint16_t >(
        pReqHead->GetHeadLen() + sizeof(keys::isoft_def::keyprovider::PIpcAsw_GetRegisteredObserver))};
    aswMsg.CreatePacket(nAswLen);
    std::ignore = aswMsg.InitIpcHead(pReqHead, nAswLen);
    keys::isoft_def::keyprovider::PIpcAsw_GetRegisteredObserver *const pPacAsw{
        static_cast< keys::isoft_def::keyprovider::PIpcAsw_GetRegisteredObserver * >(
            static_cast< void * >(aswMsg.GetIpcBody()))};
    pPacAsw->SetErrorID(0);
    if (nFindObserver <= 0U) {
        pPacAsw->SetRegProcessID(0U);
    } else {
        pPacAsw->SetRegProcessID(pPacReq->nRegProcessID);
    }
    pPacAsw->SetRegProcessID(pPacReq->nRegProcessID);
    return PResultLen::FromValue(pReqHead->nPacSize);
}
/// @brief Consumer registers update observer.
PKeysProcess_Base::PResultLen
/// @name  RegisterObserver
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
/// @returns  pid if sucess
PSvrProcess_KeyProvider::RegisterObserver(PIpcPac_Head const *const pReqHead, PIpcAutoPacket &aswMsg) const noexcept
{
    if (nullptr == pReqHead) {
        return PResultLen::FromError(SecurityErrorDomain::Errc::kIpcFault);
    }
    keys::isoft_def::keyprovider::PIpcReq_RegisterObserver const *const pPacReq{
        pReqHead->GetBody< keys::isoft_def::keyprovider::PIpcReq_RegisterObserver >()};
    // Processing logic:
    ara::core::Result< uint32_t > const result{_RegisterObserver(pReqHead, pPacReq->nActionPid, aswMsg.GetSessionId())};
    if (false == result.HasValue()) {
        return PResultLen::FromError(static_cast< SecurityErrorDomain::Errc >(result.Error().Value()));
    }
    uint16_t const nAswLen{static_cast< uint16_t >(pReqHead->GetHeadLen()
                                                   + sizeof(keys::isoft_def::keyprovider::PIpcAsw_RegisterObserver))};
    aswMsg.CreatePacket(nAswLen);
    std::ignore = aswMsg.InitIpcHead(pReqHead, nAswLen);
    keys::isoft_def::keyprovider::PIpcAsw_RegisterObserver *const pPacAsw{
        static_cast< keys::isoft_def::keyprovider::PIpcAsw_RegisterObserver * >(
            static_cast< void * >(aswMsg.GetIpcBody()))};
    pPacAsw->SetErrorID(0);
    pPacAsw->SetActionPid(static_cast< uint64_t >(result.Value()));
    return PResultLen::FromValue(0);  // Memory pointed to by pReqHead needs to be saved, framework does not delete it
}
/// @brief Unsubscribes update observer from monitoring changes of the specified slot.
PKeysProcess_Base::PResultLen
/// @name  UnsubscribeObserver
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
/// @returns  slotId if sucess
PSvrProcess_KeyProvider::UnsubscribeObserver(PIpcPac_Head const *const pReqHead, PIpcAutoPacket &aswMsg) const noexcept
{
    if (nullptr == pReqHead) {
        return PResultLen::FromError(SecurityErrorDomain::Errc::kIpcFault);
    }
    keys::isoft_def::keyprovider::PIpcReq_UnsubscribeObserver const *const pPacReq{
        pReqHead->GetBody< keys::isoft_def::keyprovider::PIpcReq_UnsubscribeObserver >()};
    // Processing logic:
    ara::core::Result< uint32_t > const result{_UnsubscribeObserver(pReqHead->nProcessID, pPacReq->nKeySlotID)};
    if (false == result.HasValue()) {
        return PResultLen::FromError(static_cast< SecurityErrorDomain::Errc >(result.Error().Value()));
    }
    uint16_t const nAswLen{static_cast< uint16_t >(
        pReqHead->GetHeadLen() + sizeof(keys::isoft_def::keyprovider::PIpcAsw_UnsubscribeObserver))};
    aswMsg.CreatePacket(nAswLen);
    std::ignore = aswMsg.InitIpcHead(pReqHead, nAswLen);
    keys::isoft_def::keyprovider::PIpcAsw_UnsubscribeObserver *const pPacAsw{
        static_cast< keys::isoft_def::keyprovider::PIpcAsw_UnsubscribeObserver * >(
            static_cast< void * >(aswMsg.GetIpcBody()))};
    pPacAsw->SetErrorID(0);
    pPacAsw->SetKeySlotID(result.Value());
    return PResultLen::FromValue(pReqHead->nPacSize);
}
/// @brief Finds key slot
PKeysProcess_Base::PResultLen
/// @name  FindKeySlot
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
/// @returns  has value if find keyslot sucess
PSvrProcess_KeyProvider::FindKeySlot(PIpcPac_Head const *const pReqHead, PIpcAutoPacket &aswMsg) const noexcept
{
    if (nullptr == pReqHead) {
        return PResultLen::FromError(SecurityErrorDomain::Errc::kIpcFault);  // IPC parameter error
    }
    keys::isoft_def::keyprovider::PIpcReq_FindKeySlot const *const pPacReq{
        pReqHead->GetBody< keys::isoft_def::keyprovider::PIpcReq_FindKeySlot >()};

    PSvrKeySlotLoader *const pFindSlotLoader{_FindSlotLoader(pPacReq->nIpcSlotID)};

    ara::core::StringView stSlotName;
    if (nullptr == pFindSlotLoader) {
        return PResultLen::FromError(SecurityErrorDomain::Errc::kIpcSlotNotExist);
    }
    uint32_t const nSlotID{pFindSlotLoader->GetSlotID()};
    stSlotName = pFindSlotLoader->GetKeySlotName();

    uint16_t const nAswLen{static_cast< uint16_t >(
        pReqHead->GetHeadLen() + static_cast< uint16_t >(sizeof(keys::isoft_def::keyprovider::PIpcAsw_FindKeySlot))
        + static_cast< uint16_t >(stSlotName.size()))};
    aswMsg.CreatePacket(nAswLen);
    std::ignore = aswMsg.InitIpcHead(pReqHead, nAswLen);
    keys::isoft_def::keyprovider::PIpcAsw_FindKeySlot *const pPacAsw{
        static_cast< keys::isoft_def::keyprovider::PIpcAsw_FindKeySlot * >(static_cast< void * >(aswMsg.GetIpcBody()))};
    pPacAsw->SetErrorID(static_cast< int32_t >(0U));
    pPacAsw->SetIpcSlotID(static_cast< uint32_t >(nSlotID));
    pPacAsw->SetNameLen(static_cast< uint16_t >(stSlotName.size()));
    if (false == stSlotName.empty()) {
        std::ignore = memcpy(static_cast< void * >(pPacAsw + 1), stSlotName.data(), stSlotName.size());
    }
    // Convert local KeySlot to network data and send out
    return PResultLen::FromValue(pReqHead->nPacSize);
}
//********************************/
}  // namespace  isoft_def
}  // namespace keys
}  // namespace crypto
}  // namespace ara
