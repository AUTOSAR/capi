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
/// @file       key_value_storage.cpp
/// @brief      AutoSar-AP Data Persistence Storage Module
/// @details    Key-Value storage
/// @date       2021-06-09
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/per/KeyValueStorage/KV Storage Interface
/// @interface_level=module
/// @trace_id_sr=
/// @unit_name=
/// @unit_description=Interface class for KV storage library
/// @endcode
///
/// ================================================================
///
/// @par Modification Log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// | 2021-06-09 | 1.0.0   | hanjingjing  | Initial version created                   |
/// | 2021-08-30 | 1.0.0   | hanjingjing  | Integrated PHKV implementation into AutoSar interface |
///
/// ================================================================

#include "ara/per/key_value_storage.h"

#include "ara/per/internal/common/isoft_per_common_api.h"
#include "ara/per/internal/initialize.h"
#include "ara/per/internal/isoftkv/kv_error_code.h"
#include "ara/per/internal/isoftkv/kv_log_api.h"
#include "ara/per/internal/isoftkv/kv_system.h"
#include "ara/per/internal/isoftkv/shared_ptr_manager.h"
#include "ara/per/internal/manifest/manifest_instance.h"
#include "ara/per/internal/open_storage_logic.h"
#include "ara/per/per_error_domain.h"
#include "ara/per/recovery.h"

namespace {
/// @brief
/// @param pKvSystem
void G_CheckKvSystem(ara::per::isoftkv::PKvSystem *const pKvSystem) noexcept
{
    PER_Assert(nullptr != pKvSystem);
    PER_Assert(false == pKvSystem->IsHaveError());
    PER_Assert(pKvSystem->OpenSystem());
    // 2025-10-17 niuliming added logic: when creating a new KV library, the main library file exists but has a size of 0
    int64_t size{pKvSystem->MainFileSize()};
    if (0 == size) {
        ara::per::isoftkv::LogInfo() << "[PH_System].CheckSystem<" << pKvSystem->GetLogKvName().data()
                                     << ">, Is Empty System. this may be a new kv system ";
        return;
    }
    PER_Assert(pKvSystem->CheckSystem());
}
/// @brief Reset a Key-Value data center
/// @param kvs The shortName path of a PortPrototype typed by a PersistencyKeyValueStorageInterface.
/// @return
bool G_ResetKvSystem(ara::per::isoftkv::PKvSystem *pKvSystem) noexcept
{
    PER_Assert(nullptr != pKvSystem);

    if (false == pKvSystem->RemoveSystem()) {
        return false;
    }
    if (false == pKvSystem->NewSystem(1U)) {
        return false;
    }
    if (pKvSystem->IsHaveError()) {
        return false;
    }
    if (false == pKvSystem->LoadUpdateFile(false)) {
        if (ara::per::isoftkv::EErrorPHKV::kSuccess != pKvSystem->GetLastError()) {
            return false;
        }
    }
    return true;
}
}  // namespace

namespace ara {
namespace per {
//********************************/
/// @brief
/// @param kvConfig
KeyValueStorage::KeyValueStorage(isoftkv::PConfigMuster_Kv const &kvConfig) noexcept
    : KeyValueStorage{isoftkv::MakeUniqueKvSystem(kvConfig, true)}
{
    G_CheckKvSystem(pKvSystem_.get());
}
/// @brief
/// @param pKvSystem
KeyValueStorage::KeyValueStorage(std::unique_ptr< isoftkv::PKvSystem > pKvSystem) noexcept
    : pKvSystem_{std::move(pKvSystem)}
{
    PER_Assert(pKvSystem_.operator bool());
}
/// @brief
/// @param kvs
KeyValueStorage::KeyValueStorage(KeyValueStorage &&kvs) noexcept : pKvSystem_{std::move(std::move(kvs).pKvSystem_)} {}
/// @brief
/// @param kvs
/// @return
KeyValueStorage &KeyValueStorage::operator=(KeyValueStorage &&kvs) &noexcept
{
    pKvSystem_ = std::move(std::move(kvs).pKvSystem_);
    return *this;
}
/// @brief
KeyValueStorage::~KeyValueStorage() noexcept
{
    isoftkv::POpenStorageLogic< KeyValueStorage >::DegStorage(pKvSystem_->GetSystemName());
}
/// @brief Return a snapshot of all keys at the time of the function call
/// @return
ara::core::Result< ara::core::Vector< ara::core::String > > KeyValueStorage::GetAllKeys() const noexcept
{
    if (false == isoftkv::IsPerInitialize()) {
        return ara::core::Result< ara::core::Vector< ara::core::String > >::FromError(PerErrc::kNotInitialized);
    }
    PER_Assert(nullptr != pKvSystem_);
    ara::core::Vector< ara::core::String > vecKeyList;
    if (false == pKvSystem_->EnumAllKey(vecKeyList)) {
        if (pKvSystem_->IsHaveError()) {
            return isoftkv::T_ErrorResult< ara::core::Vector< ara::core::String > >(pKvSystem_->GetLastError());
        }
    }
    return ara::core::Result< ara::core::Vector< ara::core::String > >::FromValue(vecKeyList);
}
/// @brief Check if a key exists
/// @param stKey
/// @return
ara::core::Result< bool > KeyValueStorage::KeyExists(ara::core::StringView const stKey) const noexcept
{
    if (false == isoftkv::IsPerInitialize()) {
        return ara::core::Result< bool >::FromError(PerErrc::kNotInitialized);
    }
    PER_Assert(nullptr != pKvSystem_);
    bool const bExist{pKvSystem_->IsKeyExist(stKey)};
    if (false == bExist) {
        if (pKvSystem_->IsHaveError()) {
            return isoftkv::T_ErrorResult< bool >(pKvSystem_->GetLastError());
        }
    }
    return ara::core::Result< bool >::FromValue(bExist);
}
/// @brief Returns the size (in bytes) of the value allocated for this key-value store.
// GetCurrentValueSize may be delayed due to another thread's call to RemoveAll
// Keys or DiscardPendingChanges, or postponed due to SetValue, RemoveKey, RecoverKey, or ResetKey calls on the same key-value pair.
/// @code{.isoft}
/// export_level=/Per
/// @traceid {SWS_PER_00554}
/// @threadsafety {re-entrant}
/// @endcode
ara::core::Result< std::uint64_t >
/// @brief Returns the size (in bytes) of the value assigned to a key of this Key-Value Storage.
// GetCurrentValueSize may be delayed by an ongoing call from another thread to RemoveAll Keys or
// DiscardPendingChanges, or to SetValue, RemoveKey, RecoverKey, or ResetKey for the same key-value pair.
/// @param key The key to look up.
/// @return A Result containing the size of the value in bytes. In case of an error, it contains any of the errors
/// defined below, or a vendor specific error.
/// PerErrc::kKeyNotFound Returned if the provided key does not exist in the Key-Value Storage.
/// PerErrc::kPhysicalStorageFailure Returned if access to the physical storage fails.
/// PerErrc::kIntegrityCorrupted Returned if stored data cannot be read because the structural integrity is
/// corrupted. PerErrc::kValidationFailed Returned if the validity of stored data cannot be ensured.
/// PerErrc::kEncryptionFailed Returned if the decryption of stored data fails.
/// PerErrc::kAuthenticationFailed Returned if checking the MAC of stored data fails.
ara::per::KeyValueStorage::GetCurrentValueSize(ara::core::StringView key) const noexcept
{
    ara::core::Result< int32_t > const result{_ReadValueLength(key)};
    if (false == result.HasValue()) {
        return isoftkv::T_ErrorResult< int32_t, uint64_t >(result);
    }
    return ara::core::Result< std::uint64_t >::FromValue(static_cast< uint64_t >(result.Value()));
}
/// @brief Delete a key and its value
/// @param stKey
/// @return
ara::core::Result< void > KeyValueStorage::RemoveKey(ara::core::StringView const stKey) noexcept
{
    if (false == isoftkv::IsPerInitialize()) {
        return ara::core::Result< void >::FromError(PerErrc::kNotInitialized);
    }
    PER_Assert(nullptr != pKvSystem_);
    bool const bReturn{pKvSystem_->RemoveKey(stKey)};
    if (false == bReturn) {
        if (pKvSystem_->IsHaveError()) {
            return isoftkv::T_ErrorResult< void >(pKvSystem_->GetLastError());
        }
    }
    return ara::core::Result< void >::FromValue();
}
/// @brief Restore the value corresponding to a key
/// @param stKey
/// @return
ara::core::Result< void > KeyValueStorage::RecoverKey(ara::core::StringView const stKey) noexcept
{
    if (false == isoftkv::IsPerInitialize()) {
        return ara::core::Result< void >::FromError(PerErrc::kNotInitialized);
    }
    // Request to restore the value from the redundant library // SWS_PER_00547
    if (false == pKvSystem_->IsKeyExist(stKey)) {
        return ara::core::Result< void >::FromError(PerErrc::kKeyNotFound);
    }
    if (pKvSystem_->IsHaveWritePage()) {
        isoftkv::LogWarn() << "[KeyValueStorage::RecoverKey] DiscardPendingChanges, stKey = " << stKey.data();
    }
    ara::core::Vector< uint8_t > vecRecoverRedd;
    bool bSuccess{true};
    if (false == pKvSystem_->RecoverKey(stKey, vecRecoverRedd)) {
        if (false == pKvSystem_->ResetKey(stKey)) {
            bSuccess = false;
        }
        for (uint32_t i = 0; i < pKvSystem_->GetReddCountN(); i++) {
            vecRecoverRedd.push_back(static_cast< uint8_t >(i));
        }
    }
    if (IsHaveRecoverReport()) {
        ara::core::InstanceSpecifier const storage{pKvSystem_->GetSystemName()};
        ara::per::RecoveryReportKind recoveryReportKind{ara::per::RecoveryReportKind::kKeyRecovered};
        ara::core::Vector< ara::core::String > reportedElements;
        reportedElements.push_back(isoftkv::T_String(stKey));
        // An empty reportedInstances indicates the Recover method is not needed
        if (false == bSuccess) {
            recoveryReportKind = ara::per::RecoveryReportKind::kKeyRecoveryFailed;
        }
        RecoveryReport(storage, recoveryReportKind, reportedElements, vecRecoverRedd);
    }
    if (false == bSuccess) {
        return isoftkv::T_ErrorResult< void >(pKvSystem_->GetLastError());
    }
    return ara::core::Result< void >::FromValue();
}
/// @brief Reset the value corresponding to a key
/// @param stKey
/// @return
ara::core::Result< void > KeyValueStorage::ResetKey(ara::core::StringView const stKey) noexcept
{
    if (false == isoftkv::IsPerInitialize()) {
        return ara::core::Result< void >::FromError(PerErrc::kNotInitialized);
    }
    PER_Assert(nullptr != pKvSystem_);
    // Delete KEY
    bool const bReturn{pKvSystem_->RemoveKey(stKey)};
    if (false == bReturn) {
        return isoftkv::T_ErrorResult< void >(pKvSystem_->GetLastError());
    }
    // Reset Key-Value
    if (false == pKvSystem_->ResetKey(stKey)) {
        return isoftkv::T_ErrorResult< void >(pKvSystem_->GetLastError());
    }
    return ara::core::Result< void >::FromValue();
}
/// @brief Delete all keys in this center
/// @return
ara::core::Result< void > KeyValueStorage::RemoveAllKeys() noexcept
{
    if (false == isoftkv::IsPerInitialize()) {
        return ara::core::Result< void >::FromError(PerErrc::kNotInitialized);
    }
    PER_Assert(nullptr != pKvSystem_);
    bool const bReturn{pKvSystem_->RemoveAllKeys()};
    if (false == bReturn) {
        if (pKvSystem_->IsHaveError()) {
            return isoftkv::T_ErrorResult< void >(pKvSystem_->GetLastError());
        }
    }
    return ara::core::Result< void >::FromValue();
}
/// @brief Flush in-memory data to persistent storage
/// @return
ara::core::Result< void > KeyValueStorage::SyncToStorage() noexcept
{
    if (false == isoftkv::IsPerInitialize()) {
        return ara::core::Result< void >::FromError(PerErrc::kNotInitialized);
    }
    PER_Assert(nullptr != pKvSystem_);
    bool bReturn{false};
    bReturn = pKvSystem_->SyncWalogToMain(true);
    if (false == bReturn) {
        if (pKvSystem_->IsHaveError()) {
            return isoftkv::T_ErrorResult< void >(pKvSystem_->GetLastError());
        }
    }
    return ara::core::Result< void >::FromValue();
}
/// @brief Discard all in-memory modifications
/// @return
/// @traceid {SWS_PER_00365}
/// @threadsafety {re-entrant}
ara::core::Result< void > KeyValueStorage::DiscardPendingChanges() noexcept
{
    if (false == isoftkv::IsPerInitialize()) {
        return ara::core::Result< void >::FromError(PerErrc::kNotInitialized);
    }
    PER_Assert(nullptr != pKvSystem_);
    bool const bReturn{pKvSystem_->DiscardChanges()};
    if (false == bReturn) {
        if (pKvSystem_->IsHaveError()) {
            return isoftkv::T_ErrorResult< void >(pKvSystem_->GetLastError());
        }
    }
    return ara::core::Result< void >::FromValue();
}
//***************/
/// @brief Initialize FileStorage using configuration from Manifest
/// @param portIns The shortName path of a PortPrototype typed by aPersistencyFileStorageInterface.
/// @param appVersion
/// @param perVersion
/// @returns A Result of void.
ara::core::Result< void > KeyValueStorage::InitStorage(ara::core::InstanceSpecifier const &portIns,
                                                       bool bNewBuild) const noexcept
{
    PER_Assert(nullptr != pKvSystem_);
    // The original data source for rebuilding the KV library is from the JSon file configuration, so starting with 1 page KV library is sufficient
    if (bNewBuild) {
        if (false == pKvSystem_->NewSystem(1U)) {
            return isoftkv::T_ErrorResult< void >(pKvSystem_->GetLastError());
        }
    } else {
        if (false == pKvSystem_->IsAccessReady()) {
            if (false == pKvSystem_->OpenSystem()) {
                return isoftkv::T_ErrorResult< void >(pKvSystem_->GetLastError());
            }
        }
    }
    manifest::PManifestReader_Per *const pManifestPer{manifest::PManifestInstance::get()};
    if (nullptr == pManifestPer) {
        return ara::core::Result< void >::FromError(ara::per::PerErrc::kStorageNotFound);
    }
    ara::core::String const stPortIns{isoftkv::T_String(portIns.ToString())};
    ara::core::String const stUpdateFileName{pManifestPer->GetKvStorageUpdateFile(stPortIns)};
    if (false == pKvSystem_->LoadUpdateFile(isoftkv::T_StringView(stUpdateFileName), false)) {
        if (isoftkv::EErrorPHKV::kSuccess != pKvSystem_->GetLastError()) {
            return isoftkv::T_ErrorResult< void >(pKvSystem_->GetLastError());
        }
    }
    std::ignore = _AutoRecover("");
    return ara::core::Result< void >::FromValue();
}
/// @brief Get the KvSystem operation pointer
/// @return
isoftkv::PKvSystem *KeyValueStorage::GetKvSystem() const noexcept
{
    if (nullptr == pKvSystem_) {
        return nullptr;
    }
    return pKvSystem_.get();
}
/// @brief Backup the current library
/// @return
ara::core::Result< bool > KeyValueStorage::BackupMain() const noexcept
{
    bool bReturn{false};
    if (nullptr == pKvSystem_) {
        bReturn = false;
    } else {
        bReturn = pKvSystem_->BackupSystem(false);
    }
    return ara::core::Result< bool >::FromValue(bReturn);
}
/// @brief Delete the main library
/// @return
ara::core::Result< bool > KeyValueStorage::RemoveMain() const noexcept
{
    bool bReturn{false};
    if (nullptr == pKvSystem_) {
        bReturn = false;
    } else {
        bReturn = pKvSystem_->RemoveSystem();
    }
    return ara::core::Result< bool >::FromValue(bReturn);
}
/// @brief Restore the current library using the backup library
/// @return
ara::core::Result< bool > KeyValueStorage::RecoverFromBackup() const noexcept
{
    bool bReturn{false};
    if (nullptr == pKvSystem_) {
        bReturn = false;
    } else {
        bReturn = pKvSystem_->RecoverFromBackup();
    }
    return ara::core::Result< bool >::FromValue(bReturn);
}
/// @brief Delete the backup library
/// @return
ara::core::Result< bool > KeyValueStorage::RemoveBackup() const noexcept
{
    bool bReturn{false};
    if (nullptr == pKvSystem_) {
        bReturn = false;
    } else {
        bReturn = pKvSystem_->RemoveBackupFile();
    }
    return ara::core::Result< bool >::FromValue(bReturn);
}
/// @brief Get the total space occupied by all files in bytes
/// May correspond to the Persistency File.fileName of a configured file.
/// @returns A Result containing the occupied space in bytes.
/// @threadsafety {re-entrant}
ara::core::Result< uint64_t > KeyValueStorage::GetStorageSpace() const noexcept
{
    isoftkv::PKvSystem *const pMainKvSystem{GetKvSystem()};
    PER_Assert(nullptr != pMainKvSystem);
    pMainKvSystem->ClearError();
    uint64_t const nSpaceSize{pMainKvSystem->GetSpaceSize()};
    if (pMainKvSystem->IsHaveError()) {
        return isoftkv::T_ErrorResult< uint64_t >(pMainKvSystem->GetLastError());
    }
    return ara::core::Result< uint64_t >::FromValue(nSpaceSize);
}
//********************************/
/// @brief Automatically repair errors using M/N redundancy
/// @param[in] stKey Key involved in the recovery operation
/// @return
ara::core::Result< void > KeyValueStorage::_AutoRecover(ara::core::StringView const &stKey) const noexcept
{
    if (nullptr == pKvSystem_) {
        return ara::core::Result< void >::FromError(PerErrc::kNotInitialized);
    }
    // If there is a write-to-disk request, automatic recovery is prohibited
    if (pKvSystem_->IsHaveWritePage()) {
        return ara::core::Result< void >::FromError(PerErrc::kResourceBusy);
    }
    isoftkv::PReddCheckData_Kv reddCheckData{pKvSystem_->FindRecoverReddPageID()};
    ara::core::Vector< uint8_t > vecReddIndexTotal;
    int32_t nRecoverCount{0};
    int32_t nSuccessCount{0};
    while (isoftkv::PPageOptBase::IsValidPageID(reddCheckData.nPageID)) {
        // Execute page recovery operation
        nRecoverCount += 1;
        if (pKvSystem_->RecoverPage(reddCheckData.nPageID, reddCheckData.nSelectIndex, reddCheckData.vecNeedRecover)) {
            nSuccessCount += 1;
            for (auto &nReddIndex : reddCheckData.vecNeedRecover) {
                if (vecReddIndexTotal.end()
                    != std::find(vecReddIndexTotal.begin(), vecReddIndexTotal.end(), nReddIndex)) {
                    vecReddIndexTotal.push_back(nReddIndex);
                }
            }
        }
        pKvSystem_->DelReddCheck(reddCheckData.nPageID);
        reddCheckData = pKvSystem_->FindRecoverReddPageID();
    }

    if (IsHaveRecoverReport()) {
        if (nRecoverCount > 0) {
            ara::core::Vector< ara::core::String > vecKey{};
            if (false == stKey.empty()) {
                vecKey.push_back(isoftkv::T_String(stKey));
            }
            ara::core::InstanceSpecifier const storage{pKvSystem_->GetSystemName()};
            ara::per::RecoveryReportKind recoveryReportKind{ara::per::RecoveryReportKind::kKeyValueStorageRecovered};
            if (nSuccessCount < nRecoverCount) {
                recoveryReportKind = ara::per::RecoveryReportKind::kKeyValueStorageRecoveryFailed;
            }
            RecoveryReport(storage, recoveryReportKind, vecKey, vecReddIndexTotal);
        }
    }
    return ara::core::Result< void >::FromValue();
}
/// @brief Encapsulated read: binary
/// @param stKey
/// @param pBuff
/// @param nBufLen
/// @return Returns the actual length of data read
ara::core::Result< int32_t > KeyValueStorage::_ReadValueBinary(ara::core::StringView const &stKey,
                                                               uint8_t *const pBuff,
                                                               uint32_t const nBufLen) const noexcept
{
    PER_Assert(nullptr != pKvSystem_);
    int32_t const nReturn{pKvSystem_->ReadValueBinary(stKey, pBuff, nBufLen)};
    if (pKvSystem_->IsHaveError()) {
        return isoftkv::T_ErrorResult< int32_t >(pKvSystem_->GetLastError());
    }
    std::ignore = _AutoRecover(stKey);
    return ara::core::Result< int32_t >::FromValue(nReturn);
}
/// @brief Encapsulated read: String
/// @param stKey
/// @return
ara::core::Result< ara::core::String > KeyValueStorage::_ReadValueString(
    ara::core::StringView const &stKey) const noexcept
{
    PER_Assert(nullptr != pKvSystem_);
    ara::core::String const stData{pKvSystem_->ReadValueString(stKey)};
    if (pKvSystem_->IsHaveError()) {
        return isoftkv::T_ErrorResult< ara::core::String >(pKvSystem_->GetLastError());
    }
    std::ignore = _AutoRecover(stKey);
    return ara::core::Result< ara::core::String >{stData};
}
/// @brief Encapsulated read: length of Value
/// @param stKey
/// @return
ara::core::Result< int32_t > KeyValueStorage::_ReadValueLength(ara::core::StringView const &stKey) const noexcept
{
    PER_Assert(nullptr != pKvSystem_);
    int32_t const nValueLen{pKvSystem_->GetValueLen(stKey)};
    if (nValueLen < 0) {
        return isoftkv::T_ErrorResult< int32_t >(pKvSystem_->GetLastError());
    }
    std::ignore = _AutoRecover(stKey);
    return ara::core::Result< int32_t >::FromValue(nValueLen);
}
/// @brief Encapsulated write: binary
/// @param stKey
/// @param pBValue
/// @param nValueLen
/// @return
ara::core::Result< void > KeyValueStorage::_WriteValueBinary(ara::core::StringView const &stKey,
                                                             uint8_t const *const pBValue,
                                                             uint32_t const nValueLen) const noexcept
{
    PER_Assert(nullptr != pKvSystem_);
    if (false == pKvSystem_->WriteValue(stKey, pBValue, nValueLen)) {
        if (pKvSystem_->IsHaveError()) {
            return isoftkv::T_ErrorResult< void >(pKvSystem_->GetLastError());
        }
    }
    return ara::core::Result< void >::FromValue();
}
/// @brief Encapsulated read: supports basic types such as int, uint32_t, int64_t, uint64_t, float, double
/// @tparam T
/// @param stKey
/// @return
/// @throws
template < typename T >
inline ara::core::Result< T > KeyValueStorage::_ReadValue(ara::core::StringView const &stKey) const noexcept
{
    PER_Assert(nullptr != pKvSystem_);
    T const nData{pKvSystem_->ReadValue< T >(stKey)};
    if ((pKvSystem_)->IsHaveError()) {
        return isoftkv::T_ErrorResult< T >(pKvSystem_->GetLastError());
    }
    std::ignore = _AutoRecover(stKey);
    return ara::core::Result< T >::FromValue(nData);
}
template < typename T >
/// @brief Encapsulated write: supports basic types such as int, uint32_t, int64_t, uint64_t, float, double
/// @tparam T
/// @param stKey
/// @param value
/// @return
/// @throws
inline ara::core::Result< void > KeyValueStorage::_WriteValue(ara::core::StringView const &stKey,
                                                              T const &value) const noexcept
{
    PER_Assert(nullptr != pKvSystem_);
    bool const bReturn{pKvSystem_->WriteValue(stKey, value)};
    if (false == (bReturn)) {
        if ((pKvSystem_)->IsHaveError()) {
            return isoftkv::T_ErrorResult< void >(pKvSystem_->GetLastError());
        }
    }
    return ara::core::Result< void >::FromValue();
}
//********************************/
/// @brief
template ara::core::Result< bool >
/// @brief
/// @param stKey
/// @return
KeyValueStorage::_ReadValue< bool >(ara::core::StringView const &stKey) const noexcept;
/// @brief
template ara::core::Result< char8_t >
/// @brief
/// @param stKey
/// @return
KeyValueStorage::_ReadValue< char8_t >(ara::core::StringView const &stKey) const noexcept;
/// @brief
template ara::core::Result< int8_t >
/// @brief
/// @param stKey
/// @return
KeyValueStorage::_ReadValue< int8_t >(ara::core::StringView const &stKey) const noexcept;
/// @brief
template ara::core::Result< uint8_t >
/// @brief
/// @param stKey
/// @return
KeyValueStorage::_ReadValue< uint8_t >(ara::core::StringView const &stKey) const noexcept;
/// @brief
template ara::core::Result< int16_t >
/// @brief
/// @param stKey
/// @return
KeyValueStorage::_ReadValue< int16_t >(ara::core::StringView const &stKey) const noexcept;
/// @brief
template ara::core::Result< uint16_t >
/// @brief
/// @param stKey
/// @return
KeyValueStorage::_ReadValue< uint16_t >(ara::core::StringView const &stKey) const noexcept;
/// @brief
template ara::core::Result< int32_t >
/// @brief
/// @param stKey
/// @return
KeyValueStorage::_ReadValue< int32_t >(ara::core::StringView const &stKey) const noexcept;
/// @brief
template ara::core::Result< uint32_t >
/// @brief
/// @param stKey
/// @return
KeyValueStorage::_ReadValue< uint32_t >(ara::core::StringView const &stKey) const noexcept;
/// @brief
template ara::core::Result< int64_t >
/// @brief
/// @param stKey
/// @return
KeyValueStorage::_ReadValue< int64_t >(ara::core::StringView const &stKey) const noexcept;
/// @brief
template ara::core::Result< uint64_t >
/// @brief
/// @param stKey
/// @return
KeyValueStorage::_ReadValue< uint64_t >(ara::core::StringView const &stKey) const noexcept;
/// @brief
template ara::core::Result< float >
/// @brief
/// @param stKey
/// @return
KeyValueStorage::_ReadValue< float >(ara::core::StringView const &stKey) const noexcept;
/// @brief
template ara::core::Result< double >
/// @brief
/// @param stKey
/// @return
KeyValueStorage::_ReadValue< double >(ara::core::StringView const &stKey) const noexcept;
/// @brief
template ara::core::Result< void >
/// @brief
/// @param stKey
/// @param value
/// @return
KeyValueStorage::_WriteValue< bool >(ara::core::StringView const &stKey, bool const &value) const noexcept;
/// @brief
template ara::core::Result< void >
/// @brief
/// @param stKey
/// @param value
/// @return
KeyValueStorage::_WriteValue< char8_t >(ara::core::StringView const &stKey, char8_t const &value) const noexcept;
/// @brief
template ara::core::Result< void >
/// @brief
/// @param stKey
/// @param value
/// @return
KeyValueStorage::_WriteValue< int8_t >(ara::core::StringView const &stKey, int8_t const &value) const noexcept;
/// @brief
template ara::core::Result< void >
/// @brief
/// @param stKey
/// @param value
/// @return
KeyValueStorage::_WriteValue< uint8_t >(ara::core::StringView const &stKey, uint8_t const &value) const noexcept;
/// @brief
template ara::core::Result< void >
/// @brief
/// @param stKey
/// @param value
/// @return
KeyValueStorage::_WriteValue< int16_t >(ara::core::StringView const &stKey, int16_t const &value) const noexcept;
/// @brief
template ara::core::Result< void >
/// @brief
/// @param stKey
/// @param value
/// @return
KeyValueStorage::_WriteValue< uint16_t >(ara::core::StringView const &stKey, uint16_t const &value) const noexcept;
/// @brief
template ara::core::Result< void >
/// @brief
/// @param stKey
/// @param value
/// @return
KeyValueStorage::_WriteValue< int32_t >(ara::core::StringView const &stKey, int32_t const &value) const noexcept;
/// @brief
template ara::core::Result< void >
/// @brief
/// @param stKey
/// @param value
/// @return
KeyValueStorage::_WriteValue< uint32_t >(ara::core::StringView const &stKey, uint32_t const &value) const noexcept;
/// @brief
template ara::core::Result< void >
/// @brief
/// @param stKey
/// @param value
/// @return
KeyValueStorage::_WriteValue< int64_t >(ara::core::StringView const &stKey, int64_t const &value) const noexcept;
/// @brief
template ara::core::Result< void >
/// @brief
/// @param stKey
/// @param value
/// @return
KeyValueStorage::_WriteValue< uint64_t >(ara::core::StringView const &stKey, uint64_t const &value) const noexcept;
/// @brief
template ara::core::Result< void >
/// @brief
/// @param stKey
/// @param value
/// @return
KeyValueStorage::_WriteValue< float >(ara::core::StringView const &stKey, float const &value) const noexcept;
/// @brief
template ara::core::Result< void >
/// @brief
/// @param stKey
/// @param value
/// @return
KeyValueStorage::_WriteValue< double >(ara::core::StringView const &stKey, double const &value) const noexcept;
/// @brief
template ara::core::Result< void >
/// @brief
/// @param stKey
/// @param value
/// @return
KeyValueStorage::_WriteValue< ara::core::String >(ara::core::StringView const &stKey,
                                                  ara::core::String const &value) const noexcept;
//********************************/
/// @brief Open/Create a Key-Value data center
/// @param kvs
/// @return
ara::core::Result< SharedHandle< KeyValueStorage > > OpenKeyValueStorage(
    ara::core::InstanceSpecifier const &kvs) noexcept
{
    // The function logic consists of the following SWS_PER: SWS_PER_00396, SWS_PER_00387, SWS_PER_00446, SWS_PER_00382, SWS_PER_00383
    if (false == isoftkv::IsPerInitialize()) {
        return ara::core::Result< SharedHandle< KeyValueStorage > >::FromError(PerErrc::kNotInitialized);
    }
    ara::core::Result< std::shared_ptr< KeyValueStorage > > const resultOpen{
        isoftkv::POpenStorageLogic< KeyValueStorage >::DoOpenLogic(kvs)};
    if (false == resultOpen.HasValue()) {
        return isoftkv::T_ErrorResult< std::shared_ptr< KeyValueStorage >, SharedHandle< KeyValueStorage > >(
            resultOpen);
    }
    return ara::core::Result< SharedHandle< KeyValueStorage > >::FromValue(resultOpen.Value());
}
/// @brief Restore a Key-Value data center
/// @param kvs
/// @return
ara::core::Result< void > RecoverKeyValueStorage(ara::core::InstanceSpecifier const &kvs) noexcept
{
    if (false == isoftkv::IsPerInitialize()) {
        return ara::core::Result< void >::FromError(PerErrc::kNotInitialized);
    }
    if (isoftkv::POpenStorageLogic< KeyValueStorage >::IsStorageBusy(kvs)) {
        return ara::core::Result< void >::FromError(PerErrc::kResourceBusy);
    }

    isoftkv::PConfigMuster_Kv kvConfig;
    bool const bAssembleConfig{kvConfig.AssembleKvConfig(kvs)};
    if (false == bAssembleConfig) {
        isoftkv::LogInfo() << "[RecoverKeyValueStorage].AssembleKvConfig" << kvs.ToString().data();
        return ara::core::Result< void >::FromError(PerErrc::kStorageNotFound);
    }

    ara::core::Vector< uint8_t > reportedInstances;
    std::unique_ptr< isoftkv::PKvSystem > pKvSystem{isoftkv::MakeUniqueKvSystem(kvConfig, false)};
    if (false == pKvSystem->IsAccessReady()) {
        isoftkv::LogInfo() << "[RecoverKeyValueStorage].MakeUniqueKvSystem" << kvs.ToString().data();
        return ara::core::Result< void >::FromError(PerErrc::kStorageNotFound);
    }
    bool bSuccess{pKvSystem->RecoverSystem_FromRedd(reportedInstances)};
    if (false == bSuccess) {
        bSuccess = G_ResetKvSystem(pKvSystem.get());
        if (bSuccess) {
            for (uint32_t i = 0; i < pKvSystem->GetReddCountN(); i++) {
                reportedInstances.push_back(static_cast< uint8_t >(i));
            }
        }
    }
    if (IsHaveRecoverReport()) {
        ara::per::RecoveryReportKind recoveryReportKind{ara::per::RecoveryReportKind::kKeyValueStorageRecovered};
        if (false == bSuccess) {
            recoveryReportKind = ara::per::RecoveryReportKind::kKeyValueStorageRecoveryFailed;
        }
        RecoveryReport(kvs, recoveryReportKind, ara::core::Vector< ara::core::String >{}, reportedInstances);
    }
    if (false == bSuccess) {
        return ara::core::Result< void >::FromError(PerErrc::kInitValueNotAvailable);
    }
    if (pKvSystem->IsHaveError()) {
        return isoftkv::T_ErrorResult< void >(pKvSystem->GetLastError());
    }

    return ara::core::Result< void >::FromValue();
}
/// @brief Reset a Key-Value data center
/// @param kvs The shortName path of a PortPrototype typed by a PersistencyKeyValueStorageInterface.
/// @return
/// @triceid [SWS_PER_00334]
ara::core::Result< void > ResetKeyValueStorage(ara::core::InstanceSpecifier const &kvs) noexcept
{
    if (false == isoftkv::IsPerInitialize()) {
        return ara::core::Result< void >::FromError(PerErrc::kNotInitialized);
    }
    if (isoftkv::POpenStorageLogic< KeyValueStorage >::IsStorageBusy(kvs)) {
        return ara::core::Result< void >::FromError(PerErrc::kResourceBusy);
    }
    std::shared_ptr< KeyValueStorage > pFindStorage{isoftkv::POpenStorageLogic< KeyValueStorage >::NewStorage(kvs)};
    if (nullptr == pFindStorage) {
        return ara::core::Result< void >::FromError(PerErrc::kStorageNotFound);
    }
    if (false == G_ResetKvSystem(pFindStorage.get()->GetKvSystem())) {
        return ara::core::Result< void >::FromError(PerErrc::kInitValueNotAvailable);
    }
    return ara::core::Result< void >::FromValue();
}
/// @brief Return the current storage space occupied by the Key-Value data center in bytes (including redundant and backup sizes)
/// @param kvs
/// @return
ara::core::Result< uint64_t > GetCurrentKeyValueStorageSize(ara::core::InstanceSpecifier const &kvs) noexcept
{
    if (false == isoftkv::IsPerInitialize()) {
        return ara::core::Result< uint64_t >::FromError(PerErrc::kNotInitialized);
    }
    if (isoftkv::POpenStorageLogic< KeyValueStorage >::IsStorageBusy(kvs)) {
        return ara::core::Result< uint64_t >::FromError(PerErrc::kResourceBusy);
    }
    if (false == isoftkv::POpenStorageLogic< KeyValueStorage >::IsStorageExist(kvs)) {
        return ara::core::Result< uint64_t >::FromError(PerErrc::kStorageNotFound);
    }
    std::shared_ptr< KeyValueStorage > pFindStorage;
    pFindStorage = isoftkv::POpenStorageLogic< KeyValueStorage >::NewStorage(kvs);
    if (nullptr == pFindStorage) {
        return ara::core::Result< uint64_t >::FromError(PerErrc::kStorageNotFound);
    }
    return pFindStorage->GetStorageSpace();
}
/// @brief Delete a Key-Value data center: 2023-08-11 internal interface
/// @param[in] kvs The shortName path of a PortPrototype typed by a PersistencyKeyValueStorageInterface.
/// @returns
/// A Result of void. In case of an error, it contains any of the errors defined below,
/// or a vendor specific error.
ara::core::Result< void > RemoveKeyValueStorage(ara::core::InstanceSpecifier const &kvs) noexcept
{
    return isoftkv::POpenStorageLogic< KeyValueStorage >::RemoveStorage(kvs);
}
//********************************/
}  // namespace per
}  // namespace ara
