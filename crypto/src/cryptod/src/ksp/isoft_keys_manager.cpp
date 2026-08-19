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
/// @file       isoft_keys_manager.cpp
/// @brief      AutoSar-Crypto Key Storage Module -- key
/// @details
/// @date       2023-09-01
/// @author     Che Jinzhao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Key Manager/Key Slot Manager
/// @interface_level=module
/// @trace_id_sr=SR_CRYPTO_01008
/// @unit_name=PKeys_Manager
/// @unit_description=Key slot manager
/// @endcode
///
/// ================================================================

#include "ara/crypto/ksp/isoft_keys_manager.h"

#include <isoft/ara_fsh/filesystem_hierarchy.h>

#include "ara/crypto/common/entry_point.h"
#include "ara/crypto/common/isoft_assert.h"
#include "ara/crypto/common/isoft_auto_increace_id.h"
#include "ara/crypto/common/isoft_thread_lock.h"
#include "ara/crypto/common/security_error_domain.h"
#include "ara/crypto/keys/isoft_ipc_key_slot.h"
#include "ara/crypto/ksp/isoft_keys_process_asymmetric.h"
#include "ara/crypto/ksp/isoft_keys_process_io_interface.h"
#include "ara/crypto/ksp/isoft_keys_process_key_slot.h"
#include "ara/crypto/ksp/isoft_keys_process_mac.h"
#include "ara/crypto/ksp/isoft_keys_process_private.h"
#include "ara/crypto/ksp/isoft_keys_process_provider.h"
#include "ara/crypto/ksp/isoft_keys_process_public.h"
#include "ara/crypto/ksp/isoft_keys_process_rng.h"
#include "ara/crypto/ksp/isoft_keys_process_secret_seed.h"
#include "ara/crypto/ksp/isoft_keys_process_symmetric.h"
#include "ara/crypto/ksp/isoft_keys_process_wrap.h"

namespace {
/// @brief Returns unique transaction ID
/// @code{.isoft}
/// @interface_level=unit
/// @needwork = dda
/// @endcode
uint64_t G_GetTransactionIndex_Ph() noexcept
{
    static ara::crypto::PAutoIncreaceID s_AutoIncreaceID_HsmBst{};
    uint64_t nReturnID{s_AutoIncreaceID_HsmBst.GetAutoIncreaceID()};
    nReturnID |= ara::crypto::keys::isoft_def::PIpcKeyProvider::GetTransactionIndexPrefix();
    return nReturnID;
}
}  // namespace
namespace ara {
namespace crypto {
namespace keys {
namespace isoft_def {
//********************************/
/// @brief Extended use of isoft's logging system
using ara::crypto::isoft_def::LogInfo;
//********************************/
/// @brief Constructor
PKeys_Manager::PKeys_Manager() noexcept
{
    using PKeysProcess_Symmetric  = cryp::isoft_def::PKeysProcess_Symmetric;
    using PKeysProcess_Asymmetric = cryp::isoft_def::PKeysProcess_Asymmetric;
    using PKeysProcess_Private    = cryp::isoft_def::PKeysProcess_Private;
    pCryptoProvider_              = DefCryptoProvider();
    PH_ASSERT(manifestKspConfig_.InitManifest(GetConfigFileName()));
    mapIpcProcess_[keyprovider::GetName_PIpcKeyProvider()]   = std::make_unique< PSvrProcess_KeyProvider >(*this);
    mapIpcProcess_[keyslot::GetName_PIpcKeySlot()]           = std::make_unique< PKeysProcess_KeySlot >(*this);
    mapIpcProcess_[iointerface::GetkName_PIoInterface_Ipc()] = std::make_unique< PKeysProcess_IoInterface >(*this);
    mapIpcProcess_[cryptoctx::GetName_PSymmetricBlockStream_Ipc()] = std::make_unique< PKeysProcess_Symmetric >(*this);
    mapIpcProcess_[cryptoctx::GetName_PMac_Ipc()]  = std::make_unique< cryp::isoft_def::PKeysProcess_Mac >(*this);
    mapIpcProcess_[cryptoctx::GetName_PWrap_Ipc()] = std::make_unique< cryp::isoft_def::PKeysProcess_Wrap >(*this);
    mapIpcProcess_[cryptoctx::GetName_PAsymmetric_Ipc()] = std::make_unique< PKeysProcess_Asymmetric >(*this);
    mapIpcProcess_[cryptoctx::GetName_PRng_Ipc()]        = std::make_unique< cryp::isoft_def::PKeysProcess_Rng >(*this);
    mapIpcProcess_[cryptoctx::GetName_PSeed_Ipc()]
        = std::make_unique< cryp::isoft_def::PKeysProcess_SecretSeed >(*this);
    mapIpcProcess_[cryptoctx::GetName_PPrivateKey_Ipc()] = std::make_unique< PKeysProcess_Private >(*this);
    mapIpcProcess_[cryptoctx::GetName_PPublicKey_Ipc()]
        = std::make_unique< cryp::isoft_def::PKeysProcess_Public >(*this);
    // Initialize IAM
#ifdef ARA_WITH_IAM
    std::ignore = ara::iam::internal::grant::IAMGrantCryptoQuery::Initialize();
#endif
}
/// @brief Destructor
PKeys_Manager::~PKeys_Manager() noexcept  // NOLINT
{
#ifdef ARA_WITH_IAM
    ara::iam::internal::grant::IAMGrantCryptoQuery::Deinitialize();
#endif
}
/// @brief Runs the service
void PKeys_Manager::Running() const noexcept
{
    std::ignore = ipcServer_.Begin();
    std::ignore = ipcServer_.Working();
    std::ignore = ipcServer_.End();
}
/// @brief Processing when IPC connection is established
/// @name   OnIpcConnect
/// @param nPid Process ID
/// @param nSessionID Temporary session ID
/// @returns 0 sucess fail otherwise
int32_t PKeys_Manager::OnIpcConnect(uint64_t const nPid, uint64_t const nSessionId) noexcept
{
    for (auto &it : mapIpcProcess_) {
        std::ignore = it.second->OnIpcConnect(nPid, nSessionId);
    }
    return 0;
}
/// @brief Processing when IPC connection is disconnected
/// @name   OnIpcClose
/// @param nPid Process ID
/// @param nSessionID Temporary session ID
/// @returns noexcept
int32_t PKeys_Manager::OnIpcClose(uint64_t const nPid, uint64_t const nSessionId) noexcept
{
    for (auto &it : mapIpcProcess_) {
        std::ignore = it.second->OnIpcClose(nPid, nSessionId);
    }
    for (auto &it : mapKeySlot_) {
        std::ignore = it.second->Close(nPid, false);
    }
    return 0;
}
/// @brief Processes IPC messages
/// @name  ProcessIpcMsg
/// @param pReq IPC request data
/// @param nReqLen IPC request data length
/// @param aswMsg IPC managed response message
/// @returns  0 sucess fail otherwise
int32_t PKeys_Manager::ProcessIpcMsg(uint8_t *const pReq, uint16_t const nReqLen, PIpcAutoPacket &aswMsg) noexcept
{
    try {
        PIpcPac_Head *const pPacHead{static_cast< PIpcPac_Head * >(static_cast< void * >(pReq))};
        ara::core::StringView const stFuncName{pPacHead->GetFuncName()};
        ara::core::StringView const stClassName{stFuncName.substr(0U, stFuncName.find("::"))};
        MAP_IpcProcess::iterator const itFind{mapIpcProcess_.find(stClassName)};
        ara::crypto::isoft_def::LogDebug() << "IPC.Process <<<<< FuncName = " << stFuncName;
        if (itFind == mapIpcProcess_.end()) {
            // Command processing class object not found
            return ProcessCmd_Error(pPacHead, aswMsg, SecurityErrorDomain::Errc::kIpcFault);
        }
        if (itFind->second == nullptr) {
            // Command processing class object is empty (almost impossible)
            return ProcessCmd_Error(pPacHead, aswMsg, SecurityErrorDomain::Errc::kIpcFault);
        }
        PKeysProcess_Base::PResultLen const result{itFind->second->ProcessIpcMsg(pReq, nReqLen, aswMsg)};
        if (false == result.HasValue()) {
            // Command processing function not found
            return ProcessCmd_Error(pPacHead, aswMsg, static_cast< SecurityErrorDomain::Errc >(result.Error().Value()));
        }
        ara::crypto::isoft_def::LogDebug() << "IPC.Process > >> >> SUCCESS.";
        return static_cast< int32_t >(result.Value());
    } catch (const std::exception &e) {
        return 0;
    }
}
/// @brief Handles IPC errors
/// @name  ProcessCmd_Error
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
/// @param nErrorCode Error code
/// @returns  0 sucess fail otherwise
int32_t PKeys_Manager::ProcessCmd_Error(PIpcPac_Head const *const pReqHead,
                                        PIpcAutoPacket &aswMsg,
                                        SecurityErrorDomain::Errc const nErrorCode) noexcept
{
    int32_t const nMsgLen{static_cast< int32_t >(pReqHead->GetHeadLen())
                          + static_cast< int32_t >(sizeof(PIpcAsw_LogicHead))};
    aswMsg.CreatePacket(static_cast< uint16_t >(nMsgLen));
    std::ignore = aswMsg.InitIpcHead(pReqHead, static_cast< uint16_t >(nMsgLen));
    PIpcAsw_LogicHead *const pPacAsw{aswMsg.GetIpcBody< PIpcAsw_LogicHead >()};
    pPacAsw->SetErrorID(static_cast< int32_t >(nErrorCode));
    ara::crypto::isoft_def::LogError() << "IPC.Process > >> >> ErrorID = " << pPacAsw->GetErrorID() << " .";
    return nMsgLen;
}
/// @brief Gets the crypto provider object
/// @name  GetCryptoProvider
/// @returns  Crypto provider reference
cryp::CryptoProvider &PKeys_Manager::GetCryptoProvider() const noexcept { return *(pCryptoProvider_.get()); }
//***************/
/// @brief Prepares the KeySlot, loading it if it does not exist
/// @name  PrepareKeySlotLoader
/// @param nProcessID Process ID
/// @param stSlotName Key slot name
/// @return PSvrKeySlotLoader pointer
ara::core::Result< PSvrKeySlotLoader * > PKeys_Manager::PrepareKeySlotLoader(
    uint64_t const &nProcessID, ara::core::StringView const &stSlotName) noexcept
{
    // Search in Json configuration; report error if not found
    ara::core::String const stNewSlotName{_FindRealSlotName(stSlotName)};
    if (stNewSlotName.empty()) {
        ara::crypto::isoft_def::LogError() << "crypto load_keySlot fail. not find slot:" << stSlotName << ". in json";
        return ara::core::Result< PSvrKeySlotLoader * >::FromError(SecurityErrorDomain::Errc::kUnreservedResource);
    }
    // First verify access permission PDP for nProcessID in IAM
    if (false == _CheckIamLevel(nProcessID, stNewSlotName)) {
        // IAM permission error
        return ara::core::Result< PSvrKeySlotLoader * >::FromError(SecurityErrorDomain::Errc::kAccessViolation);
    }
    // First look in cache
    PSvrKeySlotLoader *const pFindSlot{FindSlotByName(stNewSlotName)};
    if (pFindSlot != nullptr) {
        return ara::core::Result< PSvrKeySlotLoader * >::FromValue(pFindSlot);
    }

    // Create a new SlotLoader
    PSvrKeySlotLoader *const pNewKeySlot{_InitNewSlotLoader(stNewSlotName)};
    if (nullptr == pNewKeySlot) {
        return ara::core::Result< PSvrKeySlotLoader * >::FromError(SecurityErrorDomain::Errc::kInsufficientResource);
    }
    // If slot exists but is not ready, initialize it with configuration data
    ara::core::Result< bool > const ready{pNewKeySlot->IsReady(true)};
    if (!ready.HasValue()) {
        keys::KeySlotPrototypeProps::Uptr const pFindSlotConfig{FindConfigKeySlotProps(stNewSlotName)};
        if (nullptr == pFindSlotConfig.get()) {  // If InstanceSpecifier is incorrect (slot not allocated)
            return ara::core::Result< PSvrKeySlotLoader * >::FromError(SecurityErrorDomain::Errc::kUnreservedResource);
        }
        ara::core::Result< bool > const resBool{pNewKeySlot->SaveSlotProps(*pFindSlotConfig)};
        if (!resBool.HasValue()) {
            return ara::core::Result< PSvrKeySlotLoader * >::FromError(
                static_cast< SecurityErrorDomain::Errc >(resBool.Error().Value()));
        }
        bool const ret{pNewKeySlot->CommitSaveAction()};
        if (!ret) {
            ara::crypto::isoft_def::LogInfo() << "NewSlot CommitSaveAction failed";
            return ara::core::Result< PSvrKeySlotLoader * >::FromError(SecurityErrorDomain::Errc::kUnreservedResource);
        }
    }
    return ara::core::Result< PSvrKeySlotLoader * >::FromValue(pNewKeySlot);
}
/// @brief Finds key slot loader by name
/// @name  FindSlotByName
/// @param stSlotName Key slot name
/// @return PSvrKeySlotLoader pointer
PSvrKeySlotLoader *PKeys_Manager::FindSlotByName(ara::core::StringView const &stSlotName) const noexcept
{
    for (const auto &it : mapKeySlot_) {
        if (it.second->GetKeySlotName() == stSlotName) {
            return it.second.get();
        }
    }
    return nullptr;
}
/// @brief Finds key slot loader by ID
/// @name  FindSlotByID
/// @param nSlotID Key slot ID
/// @returns  PSvrKeySlotLoader pointer
PSvrKeySlotLoader *PKeys_Manager::FindSlotByID(uint32_t const nSlotID) const noexcept
{
    MAP_KeySlot::const_iterator const itFind{mapKeySlot_.find(nSlotID)};
    if (itFind == mapKeySlot_.end()) {
        return nullptr;
    }
    return itFind->second.get();
}
/// @brief Finds observer
/// @name  FindUpdateObserver
/// @param nPid Process ID
/// @returns  0 not find
uint64_t PKeys_Manager::FindUpdateObserver(uint64_t const nPid) const noexcept
{
    MAP_UpdatesObserver::const_iterator const itFind{mapUpdateObserver_.find(nPid)};
    if (itFind == mapUpdateObserver_.end()) {
        return 0U;
    }
    return itFind->second;
}
/// @brief Triggers observer action
/// @name  CallObserver
/// @param nProcessID Process ID
/// @param nSlotID Key slot ID
/// @param slotName Key slot name
/// @returns true if call observer sucess false otherwise
bool PKeys_Manager::CallObserver(uint64_t const nProcessID,
                                 uint32_t const nSlotID,
                                 ara::core::String const &slotName) const noexcept
{
    uint64_t const nFindSessionId{FindUpdateObserver(nProcessID)};
    if (0U == nFindSessionId) {
        return false;
    }
    // Build observer callback packet and send to client
    ara::core::StringView const stFuncName{keyprovider::GetName_UpdateObserver().data(),
                                           keyprovider::GetName_UpdateObserver().size()};
    uint16_t const nNeedLen{static_cast< uint16_t >(sizeof(PIpcPac_Head) + stFuncName.size()
                                                    + sizeof(keyprovider::PIpcAsw_UpdateOsbserver) + slotName.size())};
    PIpcAutoPacket aswMsg;
    aswMsg.AttachPacket(ipcServer_.NewIpcPacket(nFindSessionId), false);
    aswMsg.CreatePacket(nNeedLen);
    std::ignore = aswMsg.InitIpcHead(stFuncName, nNeedLen);
    keyprovider::PIpcAsw_UpdateOsbserver *const pAswPac{
        static_cast< keyprovider::PIpcAsw_UpdateOsbserver * >(static_cast< void * >(aswMsg.GetIpcBody()))};
    pAswPac->SetUpdateSlotID(nSlotID);
    pAswPac->SetNameLen(static_cast< uint16_t >(slotName.size()));
    if (false == slotName.empty()) {
        std::ignore = memcpy(static_cast< void * >(pAswPac + 1), slotName.data(), slotName.size());
    }
    std::ignore = ipcServer_.SendIpcPacket(aswMsg.GetIpcPacket(), false);
    return true;
}
//********************************/
/// @brief Adds a transaction and returns the new transaction ID
/// @name  AddNewTransaction
/// @param nProcessID Process ID
/// @param pSlotID Key slot ID array
/// @param nSlotCount Number of key slot IDs in the array
/// @returns  New transaction ID
ara::core::Result< uint64_t > PKeys_Manager::AddNewTransaction(uint32_t const nProcessID,
                                                               uint32_t const *const pSlotID,
                                                               uint32_t const nSlotCount) noexcept
{
    /// error: SecurityErrorDomain::kUnreservedResource  If slots in @c targetSlots list are not configured with reserveSpareSlot parameter in manifest
    /// error: SecurityErrorDomain::kBusyResource       If key slots in @c targetSlots list are already involved in another pending transaction or opened in write mode
    ara::core::Vector< uint32_t > vecSlotID;
    for (uint32_t i{0U}; i < nSlotCount; ++i) {
        vecSlotID.push_back(*(pSlotID + i));
    }
    // Verify validity of key slot IDs
    for (uint32_t &itData : vecSlotID) {
        PSvrKeySlotLoader *const pSlotLoader{FindSlotByID(itData)};
        if (nullptr == pSlotLoader) {
            return ara::core::Result< uint64_t >::FromError(SecurityErrorDomain::Errc::kUnreservedResource);
        }
        if (_FindSlotInTransaction(itData) != 0U) {
            return ara::core::Result< uint64_t >::FromError(SecurityErrorDomain::Errc::kBusyResource);
        }
        if (false == pSlotLoader->IsEnableProcessWrite(static_cast< uint64_t >(nProcessID))) {
            return ara::core::Result< uint64_t >::FromError(SecurityErrorDomain::Errc::kBusyResource);
        }
    }
    /// Index is 64-bit: upper 16 bits are identifier bits, e.g., 00FF indicates HSM, FF00 indicates PH; remaining 48 bits are specific data
    uint64_t const nNewTransaction{G_GetTransactionIndex_Ph()};
    std::ignore = mapTransation_.insert(std::make_pair(nNewTransaction, vecSlotID));
    return ara::core::Result< uint64_t >::FromValue(nNewTransaction);
}
/// @brief Updates transaction
/// @name  CommitTransaction
/// @param nTransactionID Transaction ID
/// @returns  0 fail 1 sucess
ara::core::Result< uint32_t > PKeys_Manager::CommitTransaction(uint64_t const nTransactionID) noexcept
{
    MAP_Transaction::iterator const itFind{mapTransation_.find(nTransactionID)};
    /// error: SecurityErrorDomain::kInvalidArgument    if provided @c id is invalid, i.e. this ID is unknown or
    ///         correspondent transaction already was finished (commited or rolled back)
    if (itFind == mapTransation_.end()) {
        return ara::core::Result< uint32_t >::FromError(SecurityErrorDomain::Errc::kInvalidArgument);
    }
    uint32_t nCount{0U};
    for (uint32_t &itData : itFind->second) {
        PSvrKeySlotLoader *const pFindSlot{FindSlotByID(itData)};
        if (nullptr == pFindSlot) {
            continue;
        }
        if (pFindSlot->CommitSaveAction()) {
            std::ignore = pFindSlot->CallObserver();
            nCount += 1U;
        }
    }
    std::ignore = mapTransation_.erase(nTransactionID);
    return ara::core::Result< uint32_t >::FromValue(nCount);
}
/// @brief Rolls back transaction
/// @name  RollbackTransaction
/// @param nTransactionID Transaction ID
/// @returns  0 fail 1 sucess
ara::core::Result< uint32_t > PKeys_Manager::RollbackTransaction(uint64_t const nTransactionID) noexcept
{
    MAP_Transaction::iterator const itFind{mapTransation_.find(nTransactionID)};
    /// error: SecurityErrorDomain::kInvalidArgument    if provided @c id is invalid, i.e. this ID is unknown or
    ///         correspondent transaction already was finished (commited or rolled back)
    if (itFind == mapTransation_.end()) {
        return ara::core::Result< uint32_t >::FromError(SecurityErrorDomain::Errc::kInvalidArgument);
    }
    uint32_t nCount{0U};
    for (uint32_t &itData : itFind->second) {
        PSvrKeySlotLoader *const pFindSlot{FindSlotByID(itData)};
        if (nullptr == pFindSlot) {
            continue;
        }
        if (pFindSlot->RollbackSaveAction()) {
            nCount += 1U;
        }
    }
    std::ignore = mapTransation_.erase(nTransactionID);
    return ara::core::Result< uint32_t >::FromValue(nCount);
}
/// @brief Registers subscription observer
/// @name  RegisterObserver
/// @param pReqHead IPC request packet header
/// @param nActionPid Active process ID
/// @param nSessionID Temporary session ID
/// @return Active process ID
ara::core::Result< uint32_t > PKeys_Manager::RegisterObserver(PIpcPac_Head const *const pReqHead,
                                                              uint64_t const nActionPid,
                                                              uint64_t const nSessionID) noexcept
{
    if (0U == nActionPid) {
        MAP_UpdatesObserver::iterator const itFind{mapUpdateObserver_.find(pReqHead->nProcessID)};
        if (itFind != mapUpdateObserver_.end()) {
            std::ignore = mapUpdateObserver_.erase(itFind);
        }
    } else {
        mapUpdateObserver_[pReqHead->nProcessID] = nSessionID;
    }
    return ara::core::Result< uint32_t >::FromValue(nActionPid);
}
/// @brief Unregisters subscription observer for the key slot
/// @name  UnsubscribeObserver
/// @param nProcessID Process ID
/// @param nSlotID Key slot ID
/// @return Process ID
ara::core::Result< uint32_t > PKeys_Manager::UnsubscribeObserver(uint64_t const nProcessID,
                                                                 uint32_t const nSlotID) const noexcept
{
    /// error: SecurityErrorDomain::kInvalidArgument    if the specified slot is not monitored now (i.e. if it was not
    ///         successfully opened via @c OpenAsUser() or it was already unsubscribed by this method)
    PSvrKeySlotLoader *const pFindSlot{FindSlotByID(nSlotID)};
    if (nullptr == pFindSlot) {
        return ara::core::Result< uint32_t >::FromError(SecurityErrorDomain::Errc::kInvalidArgument);
    }
    bool const ret{pFindSlot->DelObserverPid(nProcessID)};
    if (ret == false) {
        return ara::core::Result< uint32_t >::FromError(SecurityErrorDomain::Errc::kInvalidArgument);
    }

    return ara::core::Result< uint32_t >::FromValue(nSlotID);
}
//***************/
/// @brief Finds the corresponding crypto provider by slot name
/// @param stSlotName Key slot name
/// @return Crypto provider name
ara::core::String PKeys_Manager::FindCryptoProviderBySlotName(ara::core::StringView const &stSlotName) const noexcept
{
    return manifestKspConfig_.FindCryptoProviderBySlotName(stSlotName);
}
/// @brief Gets the file path where the key slot is saved in the configuration file
/// @param stSlotName Key slot name
/// @return File path where the key slot is saved in the configuration file
ara::core::StringView PKeys_Manager::GetSlotSavedFileName(ara::core::StringView const &stSlotName) const noexcept
{
    manifest::PConfig_CryptoKeySlot const *const pFindSlot{manifestKspConfig_.FindKeySlotByName(stSlotName)};
    if (nullptr == pFindSlot) {
        return ara::core::StringView{};
    }
    return T_StringView(pFindSlot->keySlotData.stFileName);
}
//***************/
/// @brief Gets the filename of the configuration file
/// @return Filename of the configuration file
ara::core::String PKeys_Manager::GetConfigFileName() noexcept
{
    ara::core::String stConfigFileName;

    isoft::ara_fsh::Platform const platForm;
    stConfigFileName = platForm.GetPlatformEtcDir();
    stConfigFileName += isoft::ara_fsh::Process::kCrypto;
    if (false == stConfigFileName.empty()) {
        return stConfigFileName;
    }

    isoft::ara_fsh::Process const fsh;
    stConfigFileName = fsh.GetCrypto();
    if (false == stConfigFileName.empty()) {
        return stConfigFileName;
    }

    stConfigFileName = manifest::GetkCryptoManifestFileName();
    return stConfigFileName;
}
/// @brief Loads the default provider
/// @name  DefCryptoProvider
/// @returns  Crypto provider object instance
cryp::CryptoProvider::Uptr PKeys_Manager::DefCryptoProvider() noexcept
{
    ara::core::InstanceSpecifier const iSpecify{ara::core::StringView("isoft")};
    cryp::CryptoProvider::Uptr pCrypto{LoadCryptoProvider(iSpecify)};
    return pCrypto;
}
/// @brief Checks if permission exists in IAM configuration
/// @param nProcessID Process ID
/// @param stSlotName Key slot name
/// @return true can access false otherwise
bool PKeys_Manager::_CheckIamLevel(uint64_t const nProcessID, ara::core::StringView const &stSlotName) const noexcept
{
#ifdef ARA_WITH_IAM
    manifest::PConfig_CryptoKeySlot const *pFindKeySlot{nullptr};
    pFindKeySlot = manifestKspConfig_.FindKeySlotByName(stSlotName);
    ara::core::String stNewSlotName(stSlotName.data(), stSlotName.size());
    // Replace with new complete SlotInstance identifier name
    if (pFindKeySlot != nullptr) {
        stNewSlotName = pFindKeySlot->stKeySlotInstance;
    }
    if (false == ara::iam::internal::grant::IAMGrantCryptoQuery::HasCryptoGrant(nProcessID, stNewSlotName)) {
        ara::crypto::isoft_def::LogInfo()
            << "CheckIam Not Success ! ProcessID = " << nProcessID << ", SlotName = " << stNewSlotName.c_str();
        return false;
    }
    return true;
#else
    std::ignore = stSlotName;
    std::ignore = nProcessID;
    return true;
#endif
}
/// @brief Finds new transaction
/// @param nFindSlotID Key slot ID
/// @return 0 no find  slotID otherwise
uint32_t PKeys_Manager::_FindSlotInTransaction(uint32_t const nFindSlotID) const noexcept
{
    for (auto const &it : mapTransation_) {
        for (uint32_t const itData : it.second) {
            if (itData == nFindSlotID) {
                return itData;
            }
        }
    }
    return 0U;
}
/// @brief Initializes a new SlotLoader and adds it to mapKeySlot_ management
/// @name  _InitNewSlotLoader
/// @param stSlotName Key slot name
/// @returns  PSvrKeySlotLoader *
PSvrKeySlotLoader *PKeys_Manager::_InitNewSlotLoader(ara::core::StringView const &stSlotName) noexcept
{
    PSvrKeySlotLoader::Uptr pKeySlot{PSvrKeySlotLoader::NewKeySlotLoader(*this, stSlotName)};
    uint32_t nKeySlotID{pKeySlot->GetSlotID()};
    if (pKeySlot.get() != nullptr) {
        std::ignore = mapKeySlot_.insert(std::make_pair(nKeySlotID, std::move(pKeySlot)));
    }
    MAP_KeySlot::iterator const itFind{mapKeySlot_.find(nKeySlotID)};
    if (itFind == mapKeySlot_.end()) {
        return nullptr;
    }
    return itFind->second.get();
}
/// @brief Gets KeySlotPrototypeProps and fills mAlgId
/// @param stSlotName Key slot name
/// @return
keys::KeySlotPrototypeProps::Uptr PKeys_Manager::FindConfigKeySlotProps(
    ara::core::StringView const &stSlotName) const noexcept
{
    ara::core::String stCryptoAlgID;
    keys::KeySlotPrototypeProps::Uptr pFindSlotConfig{
        manifestKspConfig_.FindSlotPropsByName(stSlotName, stCryptoAlgID)};
    if (nullptr == pFindSlotConfig.get()) {
        return {nullptr};
    }
    if (!stCryptoAlgID.empty()) {
        pFindSlotConfig->mAlgId = GetCryptoProvider().ConvertToAlgId({stCryptoAlgID.data(), stCryptoAlgID.size()});
    }

    return pFindSlotConfig;
}
/// @brief Gets the real key slot name
/// @param stSlotName Key slot name
/// @return Key slot name
ara::core::String PKeys_Manager::_FindRealSlotName(ara::core::StringView const &stSlotName) const noexcept
{
    manifest::PConfig_CryptoKeySlot const *const pFindSlot{manifestKspConfig_.FindKeySlotByName(stSlotName)};
    if (nullptr == pFindSlot) {
        return ara::core::String{};
    }
    return pFindSlot->stKeySlotInstance;
}
//********************************/
}  // namespace  isoft_def
}  // namespace keys
}  // namespace crypto
}  // namespace ara
