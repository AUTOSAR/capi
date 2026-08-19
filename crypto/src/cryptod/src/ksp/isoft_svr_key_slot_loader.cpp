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
/// @file       isoft_svr_key_slot_loader.cpp
/// @brief      AutoSar-Crypto Key Storage Module
/// @details
/// @date       2022-08-11
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Key Manager/Key Loader
/// @interface_level=module
/// @trace_id_sr=SR_CRYPTO_01008
/// @unit_name=PSvrKeySlotLoader
/// @unit_description=Key loader
/// @endcode
///
/// ================================================================

#include "ara/crypto/ksp/isoft_svr_key_slot_loader.h"

#include "ara/crypto/common/isoft_auto_increace_id.h"
#include "ara/crypto/common/isoft_common_api.h"
#include "ara/crypto/common/isoft_data_type.h"
#include "ara/crypto/common/isoft_io_interface.h"
#include "ara/crypto/common/isoft_log_api.h"
#include "ara/crypto/common/isoft_thread_lock.h"
#include "ara/crypto/cryp/isoft_crypto_provider.h"
#include "ara/crypto/ksp/isoft_io_interface_kv_share.h"
#include "ara/crypto/ksp/isoft_keys_manager.h"
#include "ara/exec/internal/find_process_client.h"

namespace ara {
namespace crypto {
namespace keys {
namespace isoft_def {
//********************************/
KeySlotIndex PSvrKeySlotLoader::s_KeySlotIndex_{};  // NOLINT
/// @brief Creates a new key slot Loader
/// @param keySlotManager Key slot manager
/// @param stCertName Certificate name
/// @return
PSvrKeySlotLoader::Uptr PSvrKeySlotLoader::NewKeySlotLoader(PKeys_Manager &keySlotManager,
                                                            ara::core::StringView const &stCertName) noexcept
{
    ara::core::StringView const pchSlotStorageName{internal::GetSlotStorageName()};
    return std::make_unique< PSvrKeySlotLoader >(keySlotManager, pchSlotStorageName, stCertName);
}
//********************************/
/// @brief Constructor, initializes some member variables
/// @param keySlotManager Key slot manager
/// @param stStorageName Persistent storage name
/// @param stSlotName Key slot name
PSvrKeySlotLoader::PSvrKeySlotLoader(PKeys_Manager &keySlotManager,
                                     ara::core::StringView const &stStorageName,
                                     ara::core::StringView const &stSlotName) noexcept
    : PSvrKeySlotLoader{keySlotManager, stStorageName, stSlotName, {}, 0U, {}, {}, {}}
{
    nSlotID_          = s_KeySlotIndex_.GetKeySlotIndex();
    pMainIoInterface_ = _MakeMainIoInterface();
}
/// @brief Full-parameter constructor qac
/// @param keySlotManager Key slot manager
/// @param stStorageName Persistent storage name
/// @param stSlotName Key slot name
/// @param pMainIoInterface IO interface pointer
/// @param nSlotID Key slot ID
/// @param mapMemIoInterface Map list of key slot data loaded into memory
/// @param mapRegObserverPid List of client process IDs monitoring this keyslot
/// @param mapPidByWrite Write process map list
PSvrKeySlotLoader::PSvrKeySlotLoader(PKeys_Manager &keySlotManager,
                                     ara::core::StringView const &stStorageName,
                                     ara::core::StringView const &stSlotName,
                                     PIoInterface_KvShare::Uptr pMainIoInterface,
                                     uint32_t const nSlotID,
                                     MAP_AgentIoInterface const &mapMemIoInterface,
                                     MAP_ObserverPid mapRegObserverPid,
                                     MAP_WritePid mapPidByWrite) noexcept
    : ipcProcessManager_{keySlotManager}
    , stStorageName_{stStorageName}
    , stSlotName_{stSlotName}
    , pMainIoInterface_{std::move(pMainIoInterface)}
    , nSlotID_{nSlotID}
    , mapMemIoInterface_{}  // NOLINT
    , mapRegObserverPid_{std::move(mapRegObserverPid)}
    , mapPidByWrite_{std::move(mapPidByWrite)}
{
    std::ignore = mapMemIoInterface;
}
//********************************/
/// @brief Finds corresponding memory IO
/// @name   FindIoInterface
/// @param nSlotID Key slot ID
/// @param nIoInterfaceID IO interface ID
/// @returns  PSvrIoInterfaceAgent
PSvrIoInterfaceAgent *PSvrKeySlotLoader::FindIoInterface(uint32_t const nSlotID,
                                                         uint32_t const nIoInterfaceID) const noexcept
{
    if (nSlotID_ != nSlotID) {
        return nullptr;
    }
    MAP_AgentIoInterface::iterator const itFind{mapMemIoInterface_.find(nIoInterfaceID)};
    if (itFind == mapMemIoInterface_.end()) {
        return nullptr;
    }
    PSvrIoInterfaceAgent *const pAgentIo{itFind->second.get()};
    if (pAgentIo->GetIoInterfaceID() != nIoInterfaceID)  // Multiple protection, seems unnecessary
    {
        return nullptr;
    }
    return pAgentIo;
}
//***************/
/// @brief Clears the content of this key-slot
/// @name  Clear
/// @param nProcessID Process ID
/// @returns  has vlaue if clear sucess
ara::core::Result< void > PSvrKeySlotLoader::Clear(uint64_t const nProcessId) const noexcept
{
    /// error: SecurityErrorDomain::kUnreservedResource  if the target slot is not opened writeable.
    if (false == IsEnableProcessWrite(nProcessId)) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kUnreservedResource);
    }

    if (false == pMainIoInterface_.operator bool()) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kIpcSlotNotExist);
    }
    std::ignore = pMainIoInterface_->Clear();
    return ara::core::Result< void >::FromValue();
}
/// @brief Checks if the slot is empty.
/// @name  IsEmpty
/// @returns   @c true if the slot is empty or @c false otherwise
ara::core::Result< bool > PSvrKeySlotLoader::IsEmpty() const noexcept
{
    if (false == pMainIoInterface_.operator bool()) {
        return ara::core::Result< bool >::FromError(SecurityErrorDomain::Errc::kIpcSlotNotExist);
    }
    return ara::core::Result< bool >::FromValue(pMainIoInterface_->IsEmpty());
}
/// @brief Gets the actual properties of the content in the key slot. If this method is called by a "User" Actor, then always: props.exportability == false.
/// @name  GetKeyContent
/// @returns Actual properties of the content in the key slot
ara::core::Result< KeySlotContentProps > PSvrKeySlotLoader::GetKeyContent() const noexcept
{
    ara::core::Result< bool > const nErrorCode{IsReady(false)};
    if (!nErrorCode.HasValue()) {
        return ara::core::Result< KeySlotContentProps >::FromError(
            static_cast< SecurityErrorDomain::Errc >(nErrorCode.Error().Value()));
    }
    if (false == pMainIoInterface_.operator bool()) {
        return ara::core::Result< KeySlotContentProps >::FromError(SecurityErrorDomain::Errc::kIpcSlotNotExist);
    }
    KeySlotContentProps const keyContent{pMainIoInterface_->GetKeyContent()};
    return ara::core::Result< KeySlotContentProps >::FromValue(keyContent);
}
/// @brief Gets the prototype properties of the key slot.
/// @name  GetSlotProps
/// @returns Prototype properties of the key slot
ara::core::Result< KeySlotPrototypeProps > PSvrKeySlotLoader::GetSlotProps() const noexcept
{
    ara::core::Result< bool > const nErrorCode{IsReady(false)};
    if (!nErrorCode.HasValue()) {
        return ara::core::Result< KeySlotPrototypeProps >::FromError(
            static_cast< SecurityErrorDomain::Errc >(nErrorCode.Error().Value()));
    }
    if (false == pMainIoInterface_.operator bool()) {
        return ara::core::Result< KeySlotPrototypeProps >::FromError(SecurityErrorDomain::Errc::kIpcSlotNotExist);
    }
    KeySlotPrototypeProps const keySlotPropsPrototype{pMainIoInterface_->GetSlotProps()};
    return ara::core::Result< KeySlotPrototypeProps >::FromValue(keySlotPropsPrototype);
}
/// @brief Sets the prototype properties of the key slot.
/// @name   SaveSlotProps
/// @param slotProps Key slot properties
/// @returns  true if save sucess
ara::core::Result< bool > PSvrKeySlotLoader::SaveSlotProps(KeySlotPrototypeProps const &slotProps) const noexcept
{
    if (false == pMainIoInterface_.operator bool()) {
        return ara::core::Result< bool >::FromError(SecurityErrorDomain::Errc::kIpcSlotNotExist);
    }
    bool const ret{pMainIoInterface_->SaveSlotProps(slotProps)};
    return ara::core::Result< bool >::FromValue(ret);
}
/// @brief Opens this key slot and returns an IOInterface to its content.
///         If an UpdatesObserver interface is registered via RegisterObserver(), then each time this slot is updated (and becomes "user" visible), the Key
///         Storage engine (in a dedicated thread) should call the UpdatesObserver::OnUpdate() method.
///         Monitoring of the opened key slot continues even after the returned TrustedContainer is destroyed,
///         because the slot's content may be loaded into volatile memory (as a cryptobject or a CryptoContext of a crypto primitive), but after that, the TrustedContainer may be destroyed.
///         Therefore, if you need to terminate monitoring of a key slot, you should directly call the UnsubscribeObserver(SlotNumber) method.
/// @name  Open
/// @param nProcessID Process ID
/// @param subscribeForUpdates Whether to enable update subscription
/// @param writeable Whether to open in write mode
/// @return Underlying IO interface ID
ara::core::Result< uint32_t > PSvrKeySlotLoader::Open(uint64_t const nProcessID,
                                                      bool const subscribeForUpdates,
                                                      bool writeable) const noexcept
{
    if (false == pMainIoInterface_.operator bool()) {
        return ara::core::Result< uint32_t >::FromError(SecurityErrorDomain::Errc::kIpcSlotNotExist);
    }
    /// error: SecurityErrorDomain::kModifiedResource   if the specified slot has been modified after the KeySlot has
    /// been opened
    if ((false == pMainIoInterface_->IsEmpty())) {
        if (pMainIoInterface_->IsHaveWriteAction()) {
            return ara::core::Result< uint32_t >::FromError(SecurityErrorDomain::Errc::kModifiedResource);
        }
    }
    /// error: SecurityErrorDomain::kInvalidUsageOrder  if (true == subscribeForUpdates),
    ///                but there is no registered instance of the @c UpdatesObserver in the %Key Storage Provider
    ///                context
    EnumSlotOpenModal const accessLevel{writeable ? EnumSlotOpenModal::kReadWrite : EnumSlotOpenModal::kReadOnly};
    if (subscribeForUpdates) {
        uint64_t const nFindObserverPid{ipcProcessManager_.FindUpdateObserver(nProcessID)};
        if (0U == nFindObserverPid) {
            return ara::core::Result< uint32_t >::FromError(SecurityErrorDomain::Errc::kInvalidUsageOrder);
        }
        /// @param[in] subscribeForUpdates  If this flag is @c true, the @c UpdatesObserver instance (previously registered via @c
        /// RegisterObserver()) will subscribe to updates of the opened key slot
        ///
        /// If I myself am an observer, opening a key slot A with open(true, true), subscribeForUpdates is true allows observing changes to key slot A,
        /// false does not allow observing changes to key slot A. If there are multiple key slots ABCD, this method controls whether to observe other key slots.
        std::ignore = RegObserverPid(nProcessID, accessLevel);
    }
    /// error: SecurityErrorDomain::kBusyResource       if the specified slot is busy because writeable == true but
    ///                 (a) the keyslot is already opened writable, and/or
    ///                 (b) the keyslot is in scope of another ongoing transaction
    if (writeable) {
        if (false == IsEnableProcessWrite(nProcessID)) {
            return ara::core::Result< uint32_t >::FromError(SecurityErrorDomain::Errc::kBusyResource);
        }
        _AddPidByWrite(nProcessID);
    }
    // 2022-04-12 Using KV store as underlying persistence implementation
    if (nullptr == pMainIoInterface_) {
        return ara::core::Result< uint32_t >::FromError(SecurityErrorDomain::Errc::kModifiedResource);
    }
    PSvrIoInterfaceAgent::Uptr pNewAgentIo{
        std::make_unique< PSvrIoInterfaceAgent >(writeable, nSlotID_, pMainIoInterface_.get())};
    if (writeable == false) {
    }
    // After everything is normal, add to management
    uint32_t const nIoInterfaceID{pNewAgentIo->GetIoInterfaceID()};
    std::ignore = mapMemIoInterface_.insert(std::make_pair(pNewAgentIo->GetIoInterfaceID(), std::move(pNewAgentIo)));
    return ara::core::Result< uint32_t >::FromValue(nIoInterfaceID);
}
/// @brief Closes read/write records for a specific process on the key slot
/// @name   Close
/// @param nPid Process ID
/// @param bReadOnly Whether read-only
/// @returns true if close sucess
ara::core::Result< bool > PSvrKeySlotLoader::Close(uint64_t const nPid, bool const bReadOnly) const noexcept
{
    std::ignore = bReadOnly;
    MAP_WritePid::iterator const itFind{mapPidByWrite_.find(nPid)};
    if (itFind != mapPidByWrite_.end()) {
        itFind->second -= 1U;
        if (itFind->second <= 0U) {
            std::ignore = mapPidByWrite_.erase(itFind);
        }
    }
    return ara::core::Result< bool >::FromValue(true);
}
/// @brief Data copy check
/// @param pSrcInterface Source IO interface pointer
/// @return true if SaveCopyCheck sucess
ara::core::Result< bool > PSvrKeySlotLoader::SaveCopyCheck(PIoInterface const *const pSrcInterface) const noexcept
{
    if (false == pMainIoInterface_.operator bool()) {
        return ara::core::Result< bool >::FromError(SecurityErrorDomain::Errc::kIpcSlotNotExist);
    }

    PIoInterface *const pDestInterface{pMainIoInterface_.get()};
    /// error: SecurityErrorDomain::kUnreservedResource     if the target slot is not opened writeable.
    if (nullptr == pDestInterface) {
        return ara::core::Result< bool >::FromError(SecurityErrorDomain::Errc::kUnreservedResource);
    }
    if (false == pDestInterface->IsWritable()) {
        return ara::core::Result< bool >::FromError(SecurityErrorDomain::Errc::kUnreservedResource);
    }

    /// error: SecurityErrorDomain::kContentRestrictions    if the source object doesn't satisfy the slot
    /// restrictions (including version control)
    CryptoObjectType const nMainRestriction{pDestInterface->GetTypeRestriction()};
    if (pSrcInterface == nullptr) {
        if (CryptoObjectType::kUndefined != nMainRestriction) {
            return ara::core::Result< bool >::FromError(SecurityErrorDomain::Errc::kContentRestrictions);
        }
    } else {
        CryptoObjectType const nSrcObjType{pSrcInterface->GetCryptoObjectType()};
        if ((CryptoObjectType::kUndefined != nMainRestriction) && (nMainRestriction != nSrcObjType)) {
            return ara::core::Result< bool >::FromError(SecurityErrorDomain::Errc::kContentRestrictions);
        }
    }

    return ara::core::Result< bool >::FromValue(true);
}
/// @brief Data copy
/// @param pSrcInterface Source IO interface pointer
/// @return true if Save sucess
ara::core::Result< bool > PSvrKeySlotLoader::SaveCopy(PIoInterface const *const pSrcInterface) const noexcept
{
    /// error: SecurityErrorDomain::kIncompatibleObject     if the source object has property "session" or if the
    ///         source IOInterface references a KeySlot from a different CryptoProvider
    // If the source object has the "session" attribute, or the source interface references a keystore from a different crypto provider
    if (pSrcInterface->IsObjectSession()) {
        return ara::core::Result< bool >::FromError(SecurityErrorDomain::Errc::kIncompatibleObject);
    }
    /// error: SecurityErrorDomain::kEmptyContainer         if the source IOInterface is empty
    if (pSrcInterface->IsEmpty()) {
        return ara::core::Result< bool >::FromError(SecurityErrorDomain::Errc::kEmptyContainer);
    }

    /// @brief Get target key slot
    PIoInterface const *pDestInterface{nullptr};
    if (pSrcInterface->GetKeySlotName().data() == stSlotName_) {
        /// @brief Key slot update
        pDestInterface = pSrcInterface;
    } else {
        ara::core::Result< bool > res{SaveCopyCheck(pSrcInterface)};
        if (res.HasValue() == false) {
            return res;
        }
        pDestInterface = pMainIoInterface_.get();
    }

    // Transfer and save data: KeySlotContentProps
    std::ignore = pDestInterface->SaveKeyContent(pSrcInterface->GetKeyContent());
    // Transfer and save data: KeySlotPrototypeProps
    KeySlotPrototypeProps const keySlotProps{pSrcInterface->GetSlotProps()};
    if (KeySlotType::kUnDefined != keySlotProps.mSlotType) {
        std::ignore = pDestInterface->SaveSlotProps(keySlotProps);
    }
    // @brief Transfer and save data: Ciphertext KeyData
    CryptoAlgId nKeyAlgID{kAlgIdAny};  // Algorithm ID of the encryption key
    ara::core::Vector< uint8_t > vecKeyKey;
    ara::core::Vector< uint8_t > vecKeyData;
    /// @brief Get key encryption key used when encrypting key plaintext
    std::ignore = pSrcInterface->ReadKeyKey(nKeyAlgID, vecKeyKey);
    /// @brief Get key plaintext needing persistence
    std::ignore = pSrcInterface->ReadKeyData(vecKeyData);
    /// @brief Save data: keyData (key plaintext needing persistence)
    /// @details: keykey is saved in ciphertext form, keykey is the key used when encrypting key plaintext
    uint8_t *const pKeyData{vecKeyData.data()};
    uint8_t *const pKeyKeyData{vecKeyKey.data()};
    bool const bSave{pDestInterface->SaveKeyData(ReadOnlyMemRegion(pKeyData, vecKeyData.size()), nKeyAlgID,
                                                 ReadOnlyMemRegion(pKeyKeyData, vecKeyKey.size()))};
    if (!bSave) {
        return ara::core::Result< bool >::FromValue(false);
    }

    // Save current write count
    pDestInterface->SetDeleteFlag(0U);
    std::ignore = pDestInterface->SetWriteIndex();

    return ara::core::Result< bool >::FromValue(true);
}
/// @brief Saves key passed in via PIoInterface_Mem via IPC
/// @name  SaveCopy
/// @throws
/// @param nSlotID Key slot ID
/// @param keysSrc Source key data
/// @return true if SaveCopy sucess
ara::core::Result< bool > PSvrKeySlotLoader::SaveCopy(uint32_t const nSlotID,
                                                      keyslot::PIpcReq_SaveCopy_Mem const *const keysSrc) const noexcept
{
    std::ignore = nSlotID;
    /// error: SecurityErrorDomain::kEmptyContainer         if the source IOInterface is empty
    if (nullptr == keysSrc) {
        return ara::core::Result< bool >::FromError(SecurityErrorDomain::Errc::kEmptyContainer);
    }

    /// @brief Get target key slot
    ara::core::Result< bool > res{SaveCopyCheck(nullptr)};
    if (res.HasValue() == false) {
        return res;
    }
    PIoInterface *const pDestInterface{pMainIoInterface_.get()};
    if (pDestInterface == nullptr) {
        return ara::core::Result< bool >::FromError(SecurityErrorDomain::Errc::kIpcSlotNotExist);
    }

    /// @brief Save data: KeySlotContentProps
    std::ignore = pDestInterface->SaveKeyContent(keysSrc->GetKeySlotContentProps());
    // @brief Save data: KeySlotPrototypeProps
    if (KeySlotType::kUnDefined != keysSrc->GetKeySlotPrototypeProps().mSlotType) {
        std::ignore = pDestInterface->SaveSlotProps(keysSrc->GetKeySlotPrototypeProps());
    }

    /// @brief Save data: Ciphertext KeyData
    CryptoAlgId const nKeyAlgID{keysSrc->GetCryptoAlgID()};
    /// @brief Get key encryption key used when encrypting key plaintext
    uint8_t const *pRecvData{static_cast< uint8_t const * >(static_cast< void const * >(keysSrc + 1))};
    ReadOnlyMemRegion const readMemKeyKey{pRecvData, static_cast< std::size_t >(keysSrc->GetKeyKeyLen())};
    /// @brief Get key plaintext needing persistence
    pRecvData += static_cast< int32_t >(keysSrc->GetKeyKeyLen());
    ReadOnlyMemRegion const readMemKeyData{pRecvData, static_cast< std::size_t >(keysSrc->GetKeyDataLen())};
    /// @brief Save data: keyData (key plaintext needing persistence)
    /// @details: keykey is saved in ciphertext form, keykey is the key used when encrypting key plaintext
    bool const bSave{pDestInterface->SaveKeyData(readMemKeyData, nKeyAlgID, readMemKeyKey)};
    if (!bSave) {
        ara::crypto::isoft_def::LogWarn() << "SaveCopy: memory keys persist fail.";
        return ara::core::Result< bool >::FromValue(false);
    }

    // Save current write count
    pDestInterface->SetDeleteFlag(0U);
    std::ignore = pDestInterface->SetWriteIndex();

    ara::core::StringView const slotSaveFileName{ipcProcessManager_.GetSlotSavedFileName(T_StringView(stSlotName_))};
    if (false == slotSaveFileName.empty()) {
        if (!SaveDataToFile(slotSaveFileName, pRecvData, static_cast< uint32_t >(keysSrc->GetKeyKeyLen()))) {
            ara::crypto::isoft_def::LogWarn() << "SaveFile failed";
        }
    }
    return ara::core::Result< bool >::FromValue(true);
}
/// @brief Saves KeyData via IPC
/// @name  SaveKeyData
/// @param nSlotID Key slot ID
/// @param pData Data: memory start address
/// @param nLen Data length: byte unit
/// @returns    true if SaveKeyData sucess
ara::core::Result< bool > PSvrKeySlotLoader::SaveKeyData(uint32_t const nSlotID,
                                                         uint8_t const *const pData,
                                                         uint16_t const nLen) const noexcept
{
    std::ignore = nSlotID;
    /// error: SecurityErrorDomain::kEmptyContainer         if the source IOInterface is empty
    if ((nullptr == pData) || (nLen <= 0U)) {
        return ara::core::Result< bool >::FromError(SecurityErrorDomain::Errc::kEmptyContainer);
    }
    /// error: SecurityErrorDomain::kUnreservedResource     if the target slot is not opened writeable.
    PIoInterface const *const pSelfInterface{pMainIoInterface_.get()};
    if (nullptr == pSelfInterface) {
        return ara::core::Result< bool >::FromError(SecurityErrorDomain::Errc::kUnreservedResource);
    }
    if (false == pSelfInterface->IsWritable()) {
        return ara::core::Result< bool >::FromError(SecurityErrorDomain::Errc::kUnreservedResource);
    }
    // Transfer and save data: KeyData
    CryptoAlgId nKeyAlgID{kAlgIdAny};
    ara::core::Vector< uint8_t > vecData;
    std::ignore = pMainIoInterface_->ReadKeyKey(nKeyAlgID, vecData);
    uint8_t *const pKeyKeyData{vecData.data()};
    ReadOnlyMemRegion const readMemKeyKey{pKeyKeyData, vecData.size()};
    ReadOnlyMemRegion const readMemKeyData{pData, static_cast< std::size_t >(nLen)};
    bool const bSave{pSelfInterface->SaveKeyData(readMemKeyData, nKeyAlgID, readMemKeyKey)};
    if (!bSave) {
        return ara::core::Result< bool >::FromValue(false);
    }
    // Save current write count data
    pSelfInterface->SetDeleteFlag(0U);
    std::ignore = pSelfInterface->SetWriteIndex();
    std::ignore = pSelfInterface->CommitSaveAction();

    return ara::core::Result< bool >::FromValue(true);
}
/// @brief Gets deletion flag
/// @name  GetDeleteFlag
/// @returns   has value if get DeleteFlag sucess
ara::core::Result< uint32_t > PSvrKeySlotLoader::GetDeleteFlag() const noexcept
{
    PIoInterface const *const pSelfInterface{pMainIoInterface_.get()};
    if (pSelfInterface == nullptr) {
        return ara::core::Result< uint32_t >::FromError(SecurityErrorDomain::Errc::kIpcSlotNotExist);
    }
    return ara::core::Result< uint32_t >::FromValue(pSelfInterface->GetDeleteFlag());
}
/// @brief Sets deletion flag
/// @name  SetDeleteFlag
/// @param nProcessID Process ID
/// @returns  true if set DeleteFlag sucess
ara::core::Result< bool > PSvrKeySlotLoader::SetDeleteFlag(uint64_t const nProcessID) const noexcept
{
    PIoInterface const *const pSelfInterface{pMainIoInterface_.get()};
    if (pSelfInterface == nullptr) {
        return ara::core::Result< bool >::FromError(SecurityErrorDomain::Errc::kIpcSlotNotExist);
    }
    pSelfInterface->SetDeleteFlag(static_cast< uint32_t >(nProcessID));
    // Save current write count data

    std::ignore = pSelfInterface->CommitSaveAction();
    return ara::core::Result< bool >::FromValue(true);
}

/// @brief Resets key slot with configuration file
/// @param keySlotPrototypeProps Key slot properties
/// @return true if reset keyslot config sucess
ara::core::Result< bool > PSvrKeySlotLoader::ResetKeySlot(
    KeySlotPrototypeProps const &keySlotPrototypeProps) const noexcept
{
    PIoInterface *const pDestInterface{pMainIoInterface_.get()};
    if (pDestInterface == nullptr) {
        return ara::core::Result< bool >::FromError(SecurityErrorDomain::Errc::kIpcSlotNotExist);
    }
    bool const ret{pDestInterface->ResetKeySlot(keySlotPrototypeProps)};
    return ara::core::Result< bool >::FromValue(ret);
}
//***************/
/// @brief Checks if subscription is already registered
/// @name  IsRegObserver
/// @param nPid Process ID
/// @returns  true has already registed observer
bool PSvrKeySlotLoader::IsRegObserver(uint64_t const nPid) const noexcept
{
    MAP_ObserverPid::iterator const itFind{mapRegObserverPid_.find(nPid)};
    return itFind == mapRegObserverPid_.end();
}
/// @brief Registers/subscribes observer
/// @name  RegObserverPid
/// @param nPid Process ID
/// @param accessLevel Access level
/// @returns true if regist observer sucess
bool PSvrKeySlotLoader::RegObserverPid(uint64_t nPid, EnumSlotOpenModal accessLevel) const noexcept
{
    if (nPid <= 0U) {
        return false;
    }
    MAP_ObserverPid::iterator const itFind{mapRegObserverPid_.find(nPid)};
    if (itFind == mapRegObserverPid_.end()) {
        std::ignore = mapRegObserverPid_.insert(std::make_pair(nPid, accessLevel));
    } else {
        itFind->second = accessLevel;
    }
    return true;
}
/// @brief Unsubscribes
/// @name  DelObserverPid
/// @param nPid Process ID
/// @returns true if unregist observer suecess
bool PSvrKeySlotLoader::DelObserverPid(uint64_t const nPid) const noexcept
{
    MAP_ObserverPid::iterator const itFind{mapRegObserverPid_.find(nPid)};
    if (itFind == mapRegObserverPid_.end()) {
        return false;
    }
    std::ignore = mapRegObserverPid_.erase(itFind);
    return true;
}
/// @brief Triggers subscription
/// @name  CallObserver
/// @returns  0 fail 1 sucess
int32_t PSvrKeySlotLoader::CallObserver() const noexcept
{
    int32_t nCount{0};
    for (auto &it : mapRegObserverPid_) {
        uint64_t const nPid{it.first};
        if (ipcProcessManager_.CallObserver(nPid, nSlotID_, stSlotName_)) {
            nCount += 1;
        }
    }
    return nCount;
}
//***************/
/// @brief Checks if preparation work is done
/// @name  IsReady
/// @param bCheckEmpty Whether to check empty key slot
/// @returns  SecurityErrorDomain::Errc
/// @error: SecurityErrorDomain::kEmptyContainer     if the slot is empty
/// @error: SecurityErrorDomain::kAccessViolation    if this method is called by an Actor, which has no any ("Owner"
/// or "User") access rights to the key slot
ara::core::Result< bool > PSvrKeySlotLoader::IsReady(bool const bCheckEmpty) const noexcept
{
    if (false == pMainIoInterface_.operator bool()) {
        return ara::core::Result< bool >::FromError(SecurityErrorDomain::Errc::kIpcSlotNotExist);
    }

    ara::core::Result< bool > const resultIsEmpty{IsEmpty()};
    if (bCheckEmpty) {
        if (resultIsEmpty.HasValue()) {
            if (resultIsEmpty.Value()) {
                return ara::core::Result< bool >::FromError(SecurityErrorDomain::Errc::kEmptyContainer);
            }
        }
    }

    return ara::core::Result< bool >::FromValue(true);
}
/// @brief Gets key information stored in the key slot
/// @name  LoadKeyData
/// @returns Key information stored in the key slot
ara::core::Result< ara::core::Vector< uint8_t > > PSvrKeySlotLoader::LoadKeyData() const noexcept
{
    ara::core::Result< bool > const nErrorCode{IsReady(true)};
    if (!nErrorCode.HasValue()) {
        return ara::core::Result< ara::core::Vector< uint8_t > >::FromError(
            static_cast< SecurityErrorDomain::Errc >(nErrorCode.Error().Value()));
    }

    if (false == pMainIoInterface_.operator bool()) {
        return ara::core::Result< ara::core::Vector< uint8_t > >::FromError(
            SecurityErrorDomain::Errc::kIpcSlotNotExist);
    }
    ara::core::Vector< uint8_t > vecKeyData;
    if (false == pMainIoInterface_->ReadKeyData(vecKeyData)) {
        return ara::core::Result< ara::core::Vector< uint8_t > >::FromError(SecurityErrorDomain::Errc::kLogicFault);
    }
    return ara::core::Result< ara::core::Vector< uint8_t > >::FromValue(vecKeyData);
}
/// @brief Gets algorithm ID stored in the key slot
/// @name  GetPrimitiveId
/// @returns Algorithm ID stored in the key slot
ara::core::Result< ara::crypto::CryptoAlgId > PSvrKeySlotLoader::GetPrimitiveId() const noexcept
{
    ara::core::Result< bool > const nErrorCode{IsReady(true)};
    if (!nErrorCode.HasValue()) {
        return ara::core::Result< ara::crypto::CryptoAlgId >::FromError(
            static_cast< SecurityErrorDomain::Errc >(nErrorCode.Error().Value()));
    }

    if (false == pMainIoInterface_.operator bool()) {
        return ara::core::Result< ara::crypto::CryptoAlgId >::FromError(SecurityErrorDomain::Errc::kIpcSlotNotExist);
    }
    return ara::core::Result< ara::crypto::CryptoAlgId >::FromValue(pMainIoInterface_->GetPrimitiveId());
}
/// @brief Gets algorithm ID stored in the key slot
/// @name  GetAllowedUsage
/// @returns Algorithm usage restriction stored in the key slot
ara::core::Result< AllowedUsageFlags > PSvrKeySlotLoader::GetAllowedUsage() const noexcept
{
    ara::core::Result< bool > const nErrorCode{IsReady(true)};
    if (!nErrorCode.HasValue()) {
        return ara::core::Result< AllowedUsageFlags >::FromError(
            static_cast< SecurityErrorDomain::Errc >(nErrorCode.Error().Value()));
    }

    if (false == pMainIoInterface_.operator bool()) {
        return ara::core::Result< AllowedUsageFlags >::FromError(SecurityErrorDomain::Errc::kIpcSlotNotExist);
    }
    return ara::core::Result< AllowedUsageFlags >::FromValue(pMainIoInterface_->GetAllowedUsage());
}
/// @brief Verifies if the process has "write" permission
/// @name  IsEnableProcessWrite
/// @param nProcessID Process ID
/// @returns  true if can write
bool PSvrKeySlotLoader::IsEnableProcessWrite(uint64_t const nProcessID) const noexcept
{
    // A key slot can have at most one process with write permission
    int32_t nFindCount{0};
    MAP_WritePid::iterator itFind{mapPidByWrite_.begin()};
    while (true) {
        if (itFind == mapPidByWrite_.end()) {
            break;
        }
        if (itFind->first == nProcessID) {
            return true;
        }
        ara::exec::internal::FindProcessClient const findProc;
        ara::core::String stProcName;
        ara::core::Result< void > const result{findProc.FindByPid(static_cast< uint32_t >(nProcessID), stProcName)};
        uint32_t const nameLen{static_cast< uint32_t >(stProcName.size())};
        if (result.HasValue() && (nameLen > 0U)) {
            nFindCount += 1;
            itFind++;
        } else {
            itFind = mapPidByWrite_.erase(itFind);
        }
    }
    return nFindCount <= 0;
}
/// @brief Sets the opened iointerface to invalid (content modified after opening, thus invalid)
void PSvrKeySlotLoader::_SetOpenIointerfaceInvalid() const noexcept
{
    for (auto &it : mapMemIoInterface_) {
        PIoInterface *const pIoInterface{it.second->GetIoInterface()};
        PIoInterface_Mem_Server *const pMemIointerface{dynamic_cast< PIoInterface_Mem_Server * >(pIoInterface)};
        if (pMemIointerface == nullptr) {
            continue;
        }
        pMemIointerface->SetIsValid(false);
    }
}
/// @brief Saves changes
/// @name  CommitSaveAction
/// @returns  true if CommitSaveAction sucess
bool PSvrKeySlotLoader::CommitSaveAction() const noexcept
{
    if (!IsReady(false).HasValue()) {
        return false;
    }
    if (false == pMainIoInterface_.operator bool()) {
        return false;
    }
    bool const ret{pMainIoInterface_->CommitSaveAction()};

    if (ret) {
        _SetOpenIointerfaceInvalid();
    }
    return ret;
}
/// @brief Rollback operation
/// @name  RollbackSaveAction
/// @returns   true if RollbackSaveAction sucess
bool PSvrKeySlotLoader::RollbackSaveAction() const noexcept
{
    if (!IsReady(false).HasValue()) {
        return false;
    }
    if (false == pMainIoInterface_.operator bool()) {
        return false;
    }
    return pMainIoInterface_->RollbackSaveAction();
}
//********************************/
/// @brief Checks if there is permission to operate this object
/// @name  IsHaveAccessRight
/// @returns  true if has access right
bool PSvrKeySlotLoader::IsHaveAccessRight() noexcept { return true; }
/// @brief Returns the PIOInterface used by itself
/// @name  _MakeMainIoInterface
/// @returns PIoInterface_KvShare smart pointer
PIoInterface_KvShare::Uptr PSvrKeySlotLoader::_MakeMainIoInterface() const noexcept
{
    PIoInterface_KvShare::Uptr pIoInterface{
        std::make_unique< PIoInterface_KvShare >(ipcProcessManager_.GetCryptoProvider(), stStorageName_)};
    if (false == pIoInterface->InitIoInterface({stSlotName_.data(), stSlotName_.size()})) {
        ara::crypto::isoft_def::LogWarn() << "InitIoInterface failed";
        return {nullptr};
    }
    return pIoInterface;
}
/// @brief Records processes that opened in read/write mode
/// @param nPid Process ID
void PSvrKeySlotLoader::_AddPidByWrite(uint64_t const nPid) const noexcept
{
    MAP_WritePid::iterator const itFind{mapPidByWrite_.find(nPid)};
    if (itFind != mapPidByWrite_.end()) {
        itFind->second += 1U;
    } else {
        mapPidByWrite_[nPid] = 1U;
    }
}
//********************************/
}  // namespace  isoft_def
}  // namespace keys
}  // namespace crypto
}  // namespace ara
