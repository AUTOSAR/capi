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
/// @file       isoft_ipc_key_provider.cpp
/// @brief      AutoSar-Crypto Key Storage Module
/// @details
/// @date       2022-01-13
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Default Key Components/Key Provider
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_05001
/// @unit_name=PIpcKeyProvider
/// @unit_description=Key Provider
/// @endcode
///
/// ================================================================

#include "ara/crypto/keys/isoft_ipc_key_provider.h"

#include <isoft/ipccpp/client.h>
#include <isoft/ipccpp/packet.h>
#include <nai/os/nai_proc.h>

#include "ara/crypto/common/isoft_log_api.h"
#include "ara/crypto/internal/initialize.h"
#include "ara/crypto/ipc/isoft_ipc_auto_message.h"
#include "ara/crypto/ipc/isoft_ipc_protocol.h"
#include "ara/crypto/keys/isoft_ipc_key_slot.h"
#include "ara/crypto/keys/isoft_updates_observer.h"
#include "ara/crypto/manifest/manifest_instance.h"

namespace ara {
namespace crypto {
namespace keys {
namespace isoft_def {
/// @brief Extend the logging system using isoft_def implementation
using ara::crypto::isoft_def::LogInfo;
//********************************/
namespace {
/// @brief Callback function for observer events
/// @param context IPC message context information
/// @param status IPC client handle status
/// @param pIpcPacket IPC packet
/// @throw ???
void CallBack_UpdateObserver(void *const context,
                             isoft::ipc::IPCClientHandlerStatus const status,
                             isoft::ipc::IPCPacket *const pIpcPacket)
{
    if (false == internal::IsCryptoInitialize()) {
        return;
    }
    std::ignore = context;
    std::ignore = status;
    std::ignore = pIpcPacket;
    if (nullptr == pIpcPacket) {
        return;
    }
    PIpcKeyProvider *const pIpcKeyProvider{static_cast< PIpcKeyProvider * >(context)};
    PIpcPac_Head *const pRecvHead{
        static_cast< PIpcPac_Head * >(static_cast< void * >(pIpcPacket->GetBuffer()->GetPtr()))};
    ara::core::StringView const funName{keyprovider::GetName_UpdateObserver().data(),
                                        keyprovider::GetName_UpdateObserver().size()};
    if (pRecvHead->GetFuncName() != funName) {
        return;
    }
    keys::isoft_def::keyprovider::PIpcAsw_UpdateOsbserver const *const pAswPac{
        pRecvHead->GetBody< keys::isoft_def::keyprovider::PIpcAsw_UpdateOsbserver >()};

    std::ignore = pIpcKeyProvider->DoUpdateObserver(pAswPac->GetUpdateSlotID(), pAswPac->GetName());
}
}  // namespace
//********************************/
/// @brief Constructor
PIpcKeyProvider::PIpcKeyProvider() noexcept : KeyStorageProvider{}  // NOLINT
{
    ipcClient_ = isoft_def::PIpcClient::GetInstance_Keys();
}
/// @brief Destructor
PIpcKeyProvider::~PIpcKeyProvider() noexcept { std::ignore = _RegisterObserverIpc(false); }
/// @brief Load a key slot. These functions load the information associated with a KeySlot into a KeySlot object.
/// @param iSpecify Target certificate instance specifier
/// @return an unique smart pointer to allocated key slot
ara::core::Result< KeySlot::Uptr > PIpcKeyProvider::LoadKeySlot(ara::core::InstanceSpecifier &iSpecify) noexcept
{
    PH_CheckInit_RetErr(ara::core::Result< KeySlot::Uptr >);
    ara::core::String const stSlotName{manifest::PManifestInstance::Get()->TransName_PortToSlot(iSpecify.ToString())};
    if (stSlotName.empty()) {
        return ara::core::Result< KeySlot::Uptr >::FromError(SecurityErrorDomain::Errc::kUnreservedResource);
    }

    if (ipcClient_ == nullptr) {
        return ara::core::Result< KeySlot::Uptr >::FromError(SecurityErrorDomain::Errc::kUnreservedResource);
    }
    if (ipcClient_->IsWorkerReady() == false) {
        return ara::core::Result< KeySlot::Uptr >::FromError(SecurityErrorDomain::Errc::kUnreservedResource);
    }
    // The pattern of group send packet => send => unpack response packet can also be used here
    PIpcAutoPacket aswMsg;
    bool const bReturn{ipcClient_->DealIpcRequest(
        FUNC_NAME_KeyProvider(LoadKeySlot), aswMsg, [stSlotName](PIpcAutoPacket const *const pReqMsg) -> uint16_t {
            if (pReqMsg != nullptr) {
                keys::isoft_def::keyprovider::PIpcReq_LoadKeySlot ipcReq;
                ipcReq.SetNameLen(static_cast< uint16_t >(stSlotName.size()));
                ipcReq.SetPid(static_cast< uint64_t >(nai_get_pid()));
                std::ignore = pReqMsg->AddDataToIpc(&ipcReq, sizeof(ipcReq));
                std::ignore = pReqMsg->AddDataToIpc(stSlotName.data(), static_cast< uint16_t >(stSlotName.size()));
            }
            return static_cast< uint16_t >(sizeof(keys::isoft_def::keyprovider::PIpcReq_LoadKeySlot)
                                           + stSlotName.size());
        })};
    if (false == bReturn) {
        return ara::core::Result< KeySlot::Uptr >::FromError(SecurityErrorDomain::Errc::kAccessViolation);
    }
    keys::isoft_def::keyprovider::PIpcAsw_LoadKeySlot *const pIpcAsw{
        static_cast< keys::isoft_def::keyprovider::PIpcAsw_LoadKeySlot * >(static_cast< void * >(aswMsg.GetIpcBody()))};
    if (0 != pIpcAsw->GetErrorID()) {
        return ara::core::Result< KeySlot::Uptr >::FromError(TransformErrorID(pIpcAsw->GetErrorID()));
    }
    PIpcKeySlot::Uptr pKeySlot{std::make_unique< PIpcKeySlot >(*ipcClient_, stSlotName, pIpcAsw->GetIpcSlotID())};
    return ara::core::Result< KeySlot::Uptr >::FromValue(std::move(pKeySlot));
}
//***************/
/// @brief Start a new transaction for updating key slots.
///         For a key slot to become part of a transaction scope, the reserveSpareSlot model parameter of the key slot must be set to true.
///         Transactions are dedicated to simultaneously updating related key slots (in an atomic, all-or-nothing manner). All key slots that should be updated by the transaction must be opened and provided to this function.
///         Any changes to slots within the scope are performed by calling commit().
/// @param targetSlots Vector array of target key slots
/// @return a unique ID assigned to this transaction
ara::core::Result< TransactionId > PIpcKeyProvider::BeginTransaction(TransactionScope const &targetSlots) noexcept
{
    PH_CheckInit_RetErr(ara::core::Result< TransactionId >);
    if (ipcClient_ == nullptr) {
        return ara::core::Result< TransactionId >::FromError(SecurityErrorDomain::Errc::kUnreservedResource);
    }
    if (false == ipcClient_->IsWorkerReady()) {
        return ara::core::Result< TransactionId >::FromError(SecurityErrorDomain::Errc::kUnreservedResource);
    }
    PIpcAutoPacket aswMsg;
    bool const bReturn{ipcClient_->DealIpcRequest(
        FUNC_NAME_KeyProvider(BeginTransaction), aswMsg,
        [&targetSlots](PIpcAutoPacket const *const pReqMsg) -> uint16_t {
            if (pReqMsg != nullptr) {
                keys::isoft_def::keyprovider::PIpcReq_BeginTransaction ipcReq;
                ipcReq.nSlotCount = static_cast< uint32_t >(targetSlots.size());
                std::ignore       = pReqMsg->AddDataToIpc(&ipcReq, sizeof(ipcReq));
                for (auto const &itData : targetSlots) {
                    PIpcKeySlot *const pIpcKeySlot{dynamic_cast< PIpcKeySlot * >(itData)};
                    uint32_t const nSlotID{(pIpcKeySlot != nullptr) ? pIpcKeySlot->GetIpcSlotID() : 0U};
                    std::ignore = pReqMsg->AddDataToIpc(&nSlotID, sizeof(nSlotID));
                }
            }
            return static_cast< uint16_t >(sizeof(keys::isoft_def::keyprovider::PIpcReq_BeginTransaction)
                                           + targetSlots.size() * sizeof(uint32_t));
        })};
    if (false == bReturn) {
        return ara::core::Result< TransactionId >::FromError(SecurityErrorDomain::Errc::kUnreservedResource);
    }
    keys::isoft_def::keyprovider::PIpcAsw_BeginTransaction *const pIpcAsw{
        static_cast< keys::isoft_def::keyprovider::PIpcAsw_BeginTransaction * >(
            static_cast< void * >(aswMsg.GetIpcBody()))};
    if (0 != pIpcAsw->GetErrorID()) {
        return ara::core::Result< TransactionId >::FromError(TransformErrorID(pIpcAsw->GetErrorID()));
    }
    return ara::core::Result< TransactionId >::FromValue(pIpcAsw->GetTransactionId());
}
/// @brief Commit the changes of the transaction to the Key Storage.
///         During transaction execution, any changes to key slots are invisible. The commit command permanently saves all changes made during the transaction in the Key Storage.
/// @param id Transaction ID
/// @return  has value if CommitTransaction suecess false otherwise
ara::core::Result< void > PIpcKeyProvider::CommitTransaction(TransactionId id) noexcept
{
    PH_CheckInit_RetErr(ara::core::Result< void >);
    if (ipcClient_ == nullptr) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kUnreservedResource);
    }
    if (false == ipcClient_->IsWorkerReady()) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kUnreservedResource);
    }
    PIpcAutoPacket aswMsg;
    bool const bReturn{ipcClient_->DealIpcRequest(
        FUNC_NAME_KeyProvider(CommitTransaction), aswMsg, [id](PIpcAutoPacket const *const pReqMsg) -> uint16_t {
            if (pReqMsg != nullptr) {
                keys::isoft_def::keyprovider::PIpcReq_CommitTransaction ipcReq;
                ipcReq.SetTransactionId(static_cast< uint64_t >(id));
                std::ignore = pReqMsg->AddDataToIpc(&ipcReq, sizeof(ipcReq));
            }
            return sizeof(keys::isoft_def::keyprovider::PIpcReq_CommitTransaction);
        })};
    if (false == bReturn) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kUnreservedResource);
    }
    keys::isoft_def::keyprovider::PIpcAsw_CommitTransaction *const pIpcAsw{
        static_cast< keys::isoft_def::keyprovider::PIpcAsw_CommitTransaction * >(
            static_cast< void * >(aswMsg.GetIpcBody()))};
    if (0 != pIpcAsw->GetErrorID()) {
        return ara::core::Result< void >::FromError(TransformErrorID(pIpcAsw->GetErrorID()));
    }
    if (pIpcAsw->GetSuccessCount() == 0U) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kUnreservedResource);
    }
    return ara::core::Result< void >::FromValue();
}
/// @brief Roll back all changes made during the transaction in the key storage.
///         The rollback command permanently cancels all changes made during the transaction in the key storage. Rolling back a transaction is completely invisible to all applications.
/// @param id Transaction ID
/// @return has value if RollbackTransaction suecess false otherwise
ara::core::Result< void > PIpcKeyProvider::RollbackTransaction(TransactionId id) noexcept
{
    PH_CheckInit_RetErr(ara::core::Result< void >);
    if (ipcClient_ == nullptr) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kUnreservedResource);
    }
    if (false == ipcClient_->IsWorkerReady()) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kUnreservedResource);
    }
    PIpcAutoPacket aswMsg;
    bool const bReturn{ipcClient_->DealIpcRequest(
        FUNC_NAME_KeyProvider(RollbackTransaction), aswMsg, [id](PIpcAutoPacket const *const pReqMsg) -> uint16_t {
            if (pReqMsg != nullptr) {
                keys::isoft_def::keyprovider::PIpcReq_RollbackTransaction ipcReq;
                ipcReq.SetTransactionId(static_cast< uint64_t >(id));
                std::ignore = pReqMsg->AddDataToIpc(&ipcReq, sizeof(ipcReq));
            }
            return sizeof(keys::isoft_def::keyprovider::PIpcReq_RollbackTransaction);
        })};
    if (false == bReturn) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kUnreservedResource);
    }
    keys::isoft_def::keyprovider::PIpcAsw_RollbackTransaction *const pIpcAsw{
        static_cast< keys::isoft_def::keyprovider::PIpcAsw_RollbackTransaction * >(
            static_cast< void * >(aswMsg.GetIpcBody()))};
    if (0 != pIpcAsw->GetErrorID()) {
        return ara::core::Result< void >::FromError(TransformErrorID(pIpcAsw->GetErrorID()));
    }
    if (pIpcAsw->GetSuccessCount() == 0U) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kUnreservedResource);
    }
    return ara::core::Result< void >::FromValue();
}
//***************/
/// @brief Get a pointer to the registered update observer. If no observer has been registered yet, this method returns nullptr!
/// @return unique pointer to the registered Updates Observer interface (copy of an internal unique pointer is
/// returned, i.e. the %Key Storage provider continues to keep the ownership)
UpdatesObserver::Uptr PIpcKeyProvider::GetRegisteredObserver() const noexcept
{
    PH_CheckInit_RetValue(nullptr);
    if (pRegObserver_ == nullptr) {
        return {nullptr};
    }
    if (ipcClient_ == nullptr) {
        return {nullptr};
    }
    if (false == ipcClient_->IsWorkerReady()) {
        return {nullptr};
    }
    // Need to verify on the server side
    uint64_t regProcessID{0U};
    PIpcAutoPacket aswMsg;
    bool const bReturn{ipcClient_->DealIpcRequest(
        FUNC_NAME_KeyProvider(GetRegisteredObserver), aswMsg,
        [&regProcessID](PIpcAutoPacket const *const pReqMsg) -> uint16_t {
            if (pReqMsg != nullptr) {
                PIpcPac_Head const *const pPacHead{
                    static_cast< PIpcPac_Head const * >(static_cast< void * >(pReqMsg->data()))};
                keys::isoft_def::keyprovider::PIpcReq_GetRegisteredObserver ipcReq;
                ipcReq.nRegProcessID = pPacHead->nProcessID;
                regProcessID         = ipcReq.nRegProcessID;
                std::ignore          = pReqMsg->AddDataToIpc(&ipcReq, sizeof(ipcReq));
            }
            return sizeof(keys::isoft_def::keyprovider::PIpcReq_GetRegisteredObserver);
        })};
    if (false == bReturn) {
        return {nullptr};
    }
    keys::isoft_def::keyprovider::PIpcAsw_GetRegisteredObserver *const pIpcAsw{
        static_cast< keys::isoft_def::keyprovider::PIpcAsw_GetRegisteredObserver * >(
            static_cast< void * >(aswMsg.GetIpcBody()))};
    if (0 != pIpcAsw->GetErrorID()) {
        return {nullptr};
    }
    if (regProcessID != pIpcAsw->GetRegProcessID()) {
        return {nullptr};
    }
    return {CopyUpdateObserver(pRegObserver_.get())};
}
/// @brief Register a consumer update observer.
///         An application process can only register one UpdatesObserver instance, so this method always unregisters the previous observer and returns its unique pointer.
///         If (nullptr == observer), this method only unregisters the previous observer! If no observer has been registered, this method returns nullptr!
/// @param observer Observer
/// @return unique pointer to previously registered Updates Observer interface (the pointer ownership is "moved
///           out" to the caller code)
UpdatesObserver::Uptr PIpcKeyProvider::RegisterObserver(UpdatesObserver::Uptr observer) noexcept
{
    PH_CheckInit_RetValue(nullptr);
    if (ipcClient_ == nullptr) {
        return {nullptr};
    }
    if (false == ipcClient_->IsWorkerReady()) {
        return {nullptr};
    }
    UpdatesObserver::Uptr pOldObserver{std::move(pRegObserver_)};
    if (pOldObserver.operator bool() != observer.operator bool()) {
        std::ignore = _RegisterObserverIpc(observer.operator bool());
    }
    pRegObserver_ = std::move(observer);
    return pOldObserver;
}
/// @brief Unsubscribe the update observer from monitoring changes of the specified slot.
/// @param slot Key slot object
/// @return has value if UnsubscribeObserver suecess false otherwise
ara::core::Result< void > PIpcKeyProvider::UnsubscribeObserver(KeySlot &slot) noexcept
{
    PH_CheckInit_RetErr(ara::core::Result< void >);
    if (ipcClient_ == nullptr) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kUnreservedResource);
    }
    if (false == ipcClient_->IsWorkerReady()) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kUnreservedResource);
    }
    PIpcKeySlot *const pIpcKeySlot{dynamic_cast< PIpcKeySlot * >(&slot)};
    PIpcAutoPacket aswMsg;
    uint32_t keySlotID{0U};
    ara::core::StringView const stFuncName{FUNC_NAME_KeyProvider(UnsubscribeObserver)};
    bool const bReturn{ipcClient_->DealIpcRequest(
        stFuncName, aswMsg, [pIpcKeySlot, &keySlotID](PIpcAutoPacket const *const pReqMsg) -> uint16_t {
            if (pReqMsg != nullptr) {
                keys::isoft_def::keyprovider::PIpcReq_UnsubscribeObserver ipcReq;
                ipcReq.nKeySlotID = pIpcKeySlot->GetIpcSlotID();
                keySlotID         = ipcReq.nKeySlotID;
                std::ignore       = pReqMsg->AddDataToIpc(&ipcReq, sizeof(ipcReq));
            }
            return sizeof(keys::isoft_def::keyprovider::PIpcReq_UnsubscribeObserver);
        })};
    std::ignore = keySlotID;
    if (false == bReturn) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kUnreservedResource);
    }
    keys::isoft_def::keyprovider::PIpcAsw_UnsubscribeObserver *const pIpcAsw{
        static_cast< keys::isoft_def::keyprovider::PIpcAsw_UnsubscribeObserver * >(
            static_cast< void * >(aswMsg.GetIpcBody()))};
    if (0 != pIpcAsw->GetErrorID()) {
        return ara::core::Result< void >::FromError(TransformErrorID(pIpcAsw->GetErrorID()));
    }
    return ara::core::Result< void >::FromValue();
}
//********************************/
/// @brief Update the locally registered observer callback
/// @param nSlotID Key slot ID
/// @param slotName Key slot name
/// @return true if do update sucess false otherwise
bool PIpcKeyProvider::DoUpdateObserver(uint32_t const nSlotID, ara::core::StringView const &slotName) const noexcept
{
    if (pRegObserver_ == nullptr) {
        return false;
    }
    std::unique_ptr< PIpcKeySlot > const pKeySlot{std::make_unique< PIpcKeySlot >(*(ipcClient_), slotName, nSlotID)};
    TransactionScope vecTrans;
    vecTrans.push_back(pKeySlot.get());
    pRegObserver_->OnUpdate(vecTrans);
    return true;
}
//***************/
/// @brief Register/unregister observer callback
/// @param bReg Whether to register
/// @return Whether registering/unregistering the observer callback succeeded
bool PIpcKeyProvider::_RegisterObserverIpc(bool const bReg) noexcept
{
    PH_CheckInit_RetValue(false);
    ara::core::StringView const stFuncName{FUNC_NAME_KeyProvider(RegisterObserver)};
    // Two execution branches: one for registration, one for deregistration
    if (bReg) {
        isoft::ipc::IPCClientHandler const handler{&CallBack_UpdateObserver};
        PIpcClient::CB_IpcReqMsg const funIpcPacket{[](PIpcAutoPacket const *const pReqMsg) -> uint16_t {
            if (pReqMsg != nullptr) {
                PIpcPac_Head *const pPacHead{static_cast< PIpcPac_Head * >(static_cast< void * >(pReqMsg->data()))};
                keyprovider::PIpcReq_RegisterObserver ipcReq;
                ipcReq.nActionPid = pPacHead->nProcessID;
                std::ignore       = pReqMsg->AddDataToIpc(&ipcReq, sizeof(ipcReq));
            }
            return sizeof(keyprovider::PIpcReq_RegisterObserver);
        }};
        return ipcClient_->AsyncIpcRequest(stFuncName, funIpcPacket, handler, this, -1);
    }
    isoft::ipc::IPCClientHandler const handler{[](void *const context, isoft::ipc::IPCClientHandlerStatus const status,
                                                  isoft::ipc::IPCPacket *const pIpcPacket) -> void {
        std::ignore = context;
        std::ignore = status;
        std::ignore = pIpcPacket;
    }};
    PIpcClient::CB_IpcReqMsg const funIpcPacket{[](PIpcAutoPacket const *const pReqMsg) -> uint16_t {
        if (pReqMsg != nullptr) {
            keyprovider::PIpcReq_RegisterObserver ipcReq;
            ipcReq.nActionPid = 0U;
            std::ignore       = pReqMsg->AddDataToIpc(&ipcReq, sizeof(ipcReq));
        }
        return sizeof(keyprovider::PIpcReq_RegisterObserver);
    }};
    return ipcClient_->AsyncIpcRequest(stFuncName, funIpcPacket, handler, this, -1);
}
/// @brief Copy the observer callback
/// @param pSrcObserver Source registered observer
/// @return unique pointer to UpdatesObserver
UpdatesObserver::Uptr PIpcKeyProvider::CopyUpdateObserver(UpdatesObserver const *const pSrcObserver) noexcept
{
    PH_CheckInit_RetValue(nullptr);
    if (nullptr == pSrcObserver) {
        return {nullptr};
    }
    UpdatesObserver::Uptr pUpdateObserver{std::make_unique< PUpdatesObserver >()};
    return pUpdateObserver;
}
//********************************/
}  // namespace  isoft_def
}  // namespace keys
}  // namespace crypto
}  // namespace ara
