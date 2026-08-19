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
/// @file       isoft_keys_process_base.cpp
/// @brief      AutoSar-Crypto Key Storage Module
/// @details
/// @date       2022-08-11
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Key Manager/Key Slot Manager
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_01008
/// @unit_name=PKeysProcess_Base
/// @unit_description=Base class for server-side key provider logic processing
/// @endcode
///
/// ================================================================

#include "ara/crypto/ksp/isoft_keys_process_base.h"

#include "ara/crypto/ksp/isoft_keys_manager.h"

namespace ara {
namespace crypto {
namespace keys {
namespace isoft_def {
//********************************/
/// @brief KeyProvider IPC server-side key slot manager.
//********************************/
/// @brief Constructor
/// @param lpcProcessManager LCP command processing manager
PKeysProcess_Base::PKeysProcess_Base(PKeys_Manager &lpcProcessManager) noexcept : lpcProcessManager_{lpcProcessManager}
{
}
//***************/
/// @brief Gets the crypto provider
/// @return Crypto provider reference
cryp::CryptoProvider &PKeysProcess_Base::GetCryptoProvider() const noexcept
{
    return lpcProcessManager_.GetCryptoProvider();
}
/// @brief Finds key slot loader
/// @param ipcSlotID Key slot ID
/// @return  PSvrKeySlotLoader pointer
keys::isoft_def::PSvrKeySlotLoader *PKeysProcess_Base::_FindSlotLoader(uint32_t const ipcSlotID) const noexcept
{
    return lpcProcessManager_.FindSlotByID(ipcSlotID);
}
/// @brief Prepares a key slot loader
/// @param nProcessID Process ID
/// @param stSlotName Key slot name
/// @return  PSvrKeySlotLoader pointer
ara::core::Result< PSvrKeySlotLoader * > PKeysProcess_Base::_PrepareKeySlotLoader(
    uint64_t const &nProcessID, ara::core::StringView const &stSlotName) const noexcept
{
    return lpcProcessManager_.PrepareKeySlotLoader(nProcessID, stSlotName);
}
/// @brief Adds a transaction
/// @param nProcessID Process ID
/// @param pSlotID Key slot ID array
/// @param nSlotCount Number of key slot IDs in the array
/// @return  Transaction ID
ara::core::Result< uint64_t > PKeysProcess_Base::_AddNewTransaction(uint32_t const nProcessID,
                                                                    uint32_t const *const pSlotID,
                                                                    uint32_t const nSlotCount) const noexcept
{
    return lpcProcessManager_.AddNewTransaction(nProcessID, pSlotID, nSlotCount);
}

/// @brief Commits transaction
/// @param nTransactionID Transaction ID
/// @return  0 fail 1 sucess
ara::core::Result< uint32_t > PKeysProcess_Base::_CommitTransaction(uint64_t const nTransactionID) const noexcept
{
    return lpcProcessManager_.CommitTransaction(nTransactionID);
}
/// @brief Finds the corresponding crypto provider by name
/// @param stSlotName Key slot name
/// @return Crypto provider obtained from key slot name
ara::core::String PKeysProcess_Base::_FindCryptoProviderBySlotName(
    ara::core::StringView const &stSlotName) const noexcept
{
    return lpcProcessManager_.FindCryptoProviderBySlotName(stSlotName);
}
/// @brief Rolls back transaction
/// @param nTransactionID Transaction ID
/// @return 0 fail 1 sucess
ara::core::Result< uint32_t > PKeysProcess_Base::_RollbackTransaction(uint64_t const nTransactionID) const noexcept
{
    return lpcProcessManager_.RollbackTransaction(nTransactionID);
}
/// @brief Finds observer by pid
/// @param nPid Process ID
/// @return 0 not find
uint64_t PKeysProcess_Base::_FindUpdateObserver(uint64_t const nPid) const noexcept
{
    return lpcProcessManager_.FindUpdateObserver(nPid);
}
/// @brief Registers observer
/// @param pReqHead IPC request packet header
/// @param nActionPid Active process ID
/// @param nSessionID Temporary session ID
/// @return Active process ID
ara::core::Result< uint32_t > PKeysProcess_Base::_RegisterObserver(PIpcPac_Head const *const pReqHead,
                                                                   uint64_t const nActionPid,
                                                                   uint64_t const nSessionID) const noexcept
{
    return lpcProcessManager_.RegisterObserver(pReqHead, nActionPid, nSessionID);
}
/// @brief Unregisters observer
/// @param nProcessID Process ID
/// @param nSlotID Key slot ID
/// @return  Process ID
ara::core::Result< uint32_t > PKeysProcess_Base::_UnsubscribeObserver(uint64_t const nProcessID,
                                                                      uint32_t const nSlotID) const noexcept
{
    return lpcProcessManager_.UnsubscribeObserver(nProcessID, nSlotID);
}
/// @brief Finds slot property configuration information by key slot name
/// @param stSlotName Key slot name
/// @return  Key slot property information found in configuration file
keys::KeySlotPrototypeProps::Uptr PKeysProcess_Base::_FindConfigKeySlotProps(
    ara::core::StringView const &stSlotName) const noexcept
{
    return lpcProcessManager_.FindConfigKeySlotProps(stSlotName);
}
/// @brief Sets key
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
/// @return  has value if setkey sucess false otherwise
keys::isoft_def::PKeysProcess_Base::PResultLen PKeysProcess_Base::_SetKey(
    keys::isoft_def::PIpcPac_Head const *const pReqHead, keys::isoft_def::PIpcAutoPacket &aswMsg) const noexcept
{
    if (nullptr == pReqHead) {
        return PResultLen::FromError(SecurityErrorDomain::Errc::kIpcFault);  // IPC parameter error;
    }
    keys::isoft_def::cryptoctx::PIpcReq_Setkey const *const pReqMsg{
        pReqHead->GetBody< keys::isoft_def::cryptoctx::PIpcReq_Setkey >()};
    keys::isoft_def::PSvrKeySlotLoader *const pFindSlotLoader{_FindSlotLoader(pReqMsg->nIpcSlotID)};
    if (nullptr == pFindSlotLoader) {
        return PResultLen::FromError(SecurityErrorDomain::Errc::kIpcFault);  // IPC parameter error;
    }

    // Build response packet
    uint16_t const nAswLen{
        static_cast< uint16_t >(pReqHead->GetHeadLen() + sizeof(keys::isoft_def::cryptoctx::PIpcAsw_Setkey))};
    aswMsg.CreatePacket(nAswLen);
    std::ignore = aswMsg.InitIpcHead(pReqHead, nAswLen);
    keys::isoft_def::cryptoctx::PIpcAsw_Setkey *const pPacAsw{
        static_cast< keys::isoft_def::cryptoctx::PIpcAsw_Setkey * >(static_cast< void * >(aswMsg.GetIpcBody()))};
    pPacAsw->SetErrorID(0);

    return PResultLen::FromValue(pReqHead->nPacSize);
}
//********************************/
/// @brief Handles error messages
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
/// @param nErrorCode Error code
/// @return 0 sucess fail otherwise
PKeysProcess_Base::PResultLen PKeysProcess_Base::ProcessCmd_Error(PIpcPac_Head const *const pReqHead,
                                                                  PIpcAutoPacket &aswMsg,
                                                                  SecurityErrorDomain::Errc const nErrorCode) noexcept
{
    PKeys_Manager::ProcessCmd_Error(pReqHead, aswMsg, nErrorCode);
    return PResultLen::FromValue(pReqHead->nPacSize);
}
/// @brief Handles error messages
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
/// @param nErrorCode Error code
/// @return 0 sucess fail otherwise
PKeysProcess_Base::PResultLen PKeysProcess_Base::ProcessCmd_Error(PIpcPac_Head const *const pReqHead,
                                                                  PIpcAutoPacket &aswMsg,
                                                                  int32_t const nErrorCode) noexcept
{
    PKeys_Manager::ProcessCmd_Error(pReqHead, aswMsg, static_cast< SecurityErrorDomain::Errc >(nErrorCode));
    return PResultLen::FromValue(pReqHead->nPacSize);
}
//********************************/
}  // namespace  isoft_def
}  // namespace keys
}  // namespace crypto
}  // namespace ara
