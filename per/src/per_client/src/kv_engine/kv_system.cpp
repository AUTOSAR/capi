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
/// @file       kv_system.cpp
/// @brief      AutoSar-AP Data Persistence Storage Module
/// @details    Puhua KV System
/// @date       2021-05-20
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/per/KV Engine/KV Engine
/// @interface_level=module
/// @trace_id_sr=
/// @unit_name=
/// @unit_description=KV Engine
/// @endcode
///
/// ================================================================
///
/// @par Modification Log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// | 2021-05-20 | 1.0.0   | hanjingjing  | Initial version created                   |
///
/// ================================================================

#include "ara/per/internal/isoftkv/kv_system.h"

#include <algorithm>

#include "ara/per/internal/common/isoft_auto_buff.h"
#include "ara/per/internal/common/isoft_common_api.h"
#include "ara/per/internal/common/isoft_per_common_api.h"
#include "ara/per/internal/common/isoft_select_mofn.h"
#include "ara/per/internal/isoftkv/kv_log_api.h"
#include "ara/per/internal/isoftkv/page_opt_key_hash.h"
#include "ara/per/internal/isoftkv/page_opt_kv_store.h"
#include "ara/per/internal/isoftkv/page_opt_page_boss.h"
#include "ara/per/internal/isoftkv/page_opt_page_group.h"
#include "ara/per/internal/isoftkv/page_opt_value.h"

namespace {
/// @brief Persistent version Key name
/// @return
constexpr inline ara::core::StringView G_GetPerVersionName() noexcept
{
    return std::move(ara::core::StringView{"PerVersion"});
}
/// @brief App version Key name
/// @return
constexpr inline ara::core::StringView G_GetAppVersionName() noexcept
{
    return std::move(ara::core::StringView{"AppVersion"});
}
std::unique_ptr< ara::per::isoftkv::IPerCrypto > MakePerCrypto(
    ara::per::isoftkv::PConfigData_Crypto const *const pFindCrypto)
{
    if (nullptr == pFindCrypto) {
        return nullptr;
    }
    std::unique_ptr< ara::per::isoftkv::IPerCrypto > pPerCrypto{ara::per::isoftkv::NewPerCrypto()};
    if (false == pPerCrypto->LoadCrypto(pFindCrypto->stKeySlotName, pFindCrypto->stCryptoAlgorithm)) {
        ara::per::isoftkv::LogError() << "[PH_System].MakePerCrypto Error :" << pFindCrypto->stKeySlotName.c_str();
        return nullptr;
    }
    return pPerCrypto;
}
/// @brief Calculate the length after padding
/// @name  G_CalPaddingLen
/// @param  nDataLen Data size
/// @param  nBlockSize Block size
/// @returns  uint32_t
uint32_t G_CalPaddingLen(uint32_t nDataLen, uint32_t nBlockSize) noexcept
{
    if (0U == (nDataLen % nBlockSize)) {
        return static_cast< uint32_t >(nDataLen + nBlockSize);
    }
    return static_cast< uint32_t >((nDataLen + (nBlockSize - 1U)) & ~(nBlockSize - 1U));
}
/// @brief Pad the input data using the PKCS7Padding scheme
/// @name  G_PaddingInput
/// @param  buffInput
/// @param  in
/// @returns  std::size_t
std::size_t G_PaddingInput(ara::per::isoftkv::PAutoBuff &buffInput,
                           uint8_t const *const pBData,
                           uint32_t nDataLen,
                           uint32_t nBlockSize) noexcept
{
    uint32_t const nNeedLen{G_CalPaddingLen(nDataLen, nBlockSize)};
    if (buffInput.GetBuffLen() < nNeedLen) {
        return 0U;
    }

    std::ignore = buffInput.SetData(pBData, nDataLen, false);
    // PKCS7Padding
    uint8_t const nPaddingData{static_cast< uint8_t >(nNeedLen - nDataLen)};
    std::ignore = buffInput.AddData(nPaddingData, static_cast< uint32_t >(nPaddingData));
    return static_cast< std::size_t >(nNeedLen);
}
}  // namespace

namespace ara {
namespace per {
namespace isoftkv {
//********************************/
/// @brief Check if KvSystem exists
/// @param kvConfig
/// @return
bool PKvSystem::IsKvSystemExist(PConfigMuster_Kv const &kvConfig) noexcept
{
    PKvSystem kvSystem{kvConfig};
    return kvSystem.IsExist();
}
//********************************/
/// @brief Construct KvSystem using a configuration object
/// @param kvConfigMuster
PKvSystem::PKvSystem(PConfigMuster_Kv const &kvConfigMuster) noexcept
    : kvConfigMuster_{std::move(kvConfigMuster)}
    , managerCache_{kvConfigMuster_}
    , managerPage_{kvConfigMuster_, managerCache_}
    , managerKvStore_{kvConfigMuster_, managerCache_, managerPage_}
    , eKvBakeupType_{EKvBakType::kKvBakToBakFile}
{
}
/// @brief
PKvSystem::~PKvSystem() noexcept
{
    if (1U == kvConfigMuster_.GetAutoSaveWalog()) {
        // 2022-04-22 hanjingjing added: PKvSystem saves the Walog library on exit
        std::ignore = _SaveCacheToWalog(1U, false);
    }
}
/// @brief Check if the Kv library is ready for read/write
/// @return
bool PKvSystem::IsAccessReady() const noexcept
{
    if (false == managerCache_.IsAccessReady()) {
        return false;
    }
    if (false == managerPage_.IsAccessReady()) {
        return false;
    }
    if (false == managerKvStore_.IsAccessReady()) {
        return false;
    }
    return true;
}
/// @brief Get the library name for distinguishing Kv libraries when writing logs
/// @return
ara::core::StringView PKvSystem::GetLogKvName() const noexcept { return std::move(managerCache_.GetLogKvName()); }
/// @brief Get the Kv library name
/// @return
ara::core::String const &PKvSystem::GetSystemName() const noexcept { return kvConfigMuster_.GetStorageName(); }
/// @brief Get the number of redundancies from the configuration
uint32_t PKvSystem::GetReddCountN() const noexcept { return kvConfigMuster_.GetReddCountN(); }
/// @brief Get the working directory of the Kv library
/// @return
ara::core::StringView PKvSystem::GetWorkPath() const noexcept
{
    return std::move(T_StringView(kvConfigMuster_.GetWorkPath()));
}
/// @brief Get the file name of the main file
/// @param stWorkPath
/// @return
ara::core::String PKvSystem::GetMainFileName(ara::core::StringView const &stWorkPath) const noexcept
{
    return managerCache_.GetFileName(stWorkPath, ECacheSource::kMain, 0);
}
/// @brief Get the file name of a redundant file
/// @param nReddIndex Redundancy index
/// @return
ara::core::String PKvSystem::GetReddFileName(int32_t const nReddIndex) const noexcept
{
    return managerCache_.GetFileName(ECacheSource::kMain, nReddIndex);
}
//***************/
/// @brief Check if the Kv library file exists
/// @return
bool PKvSystem::IsExist() noexcept
{
    PAutoLockRead const autoLock{
        threadLockAction_};  // Ensure read and write operations are atomic in multithreaded scenarios
    uint32_t const nExistCount{managerCache_.GetExistMainCount()};
    if (nExistCount <= 0U) {
        return false;
    }
    return nExistCount >= kvConfigMuster_.GetReddCountM();
}
/// @brief Check if the Kv library is empty
/// @return
bool PKvSystem::IsNullStore() noexcept
{
    PAutoLockRead const autoLock{
        threadLockAction_};  // Ensure read and write operations are atomic in multithreaded scenarios
    if (managerKvStore_.IsHaveKvElement()) {
        return false;
    }
    return true;
}
/// @brief Check if the system format is valid
/// @return
bool PKvSystem::CheckSystem() noexcept
{
    PAutoLockRead const autoLock{
        threadLockAction_};  // Ensure read and write operations are atomic in multithreaded scenarios

    if (false == IsExist()) {
        _LogInfo("[PH_System].CheckSystem<", GetLogKvName().data(), ">, Not Find Exist MainFile.");
        return false;
    }
    uint16_t const nPageLen{static_cast< uint16_t >(_ReadPageLen())};
    if (nPageLen > 0U) {
        return true;
    }
    if (EErrorPHKV::kNotReadyPage == GetLastError()) {
        _LogInfo("[PH_System].CheckSystem<", GetLogKvName().data(), ">, Find Exist MainFile, And Find Error.");
        return false;
    }
    return false;
}
/// @brief Create a new database. nPageLevel indicates the page magnitude; different magnitudes have different initial page layouts.
/// @param nPageLevel
/// @return
bool PKvSystem::NewSystem(uint32_t const nPageLevel) noexcept
{
    PAutoLockWrite const autoLock{
        threadLockAction_};  // Ensure read and write operations are atomic in multithreaded scenarios
    PER_Assert(nPageLevel > 0U);
    uint32_t const nMaxCacheCount{kvConfigMuster_.GetMaxCacheCount()};
    uint16_t const nPageLen{static_cast< uint16_t >(kvConfigMuster_.GetPageInitLen())};
    if (false == managerCache_.InitCachePool(nMaxCacheCount, nPageLen)) {
        PER_OnOptFailed(managerCache_.GetLastError());
        LogError() << "[PH_System].NewSystem " << GetLogKvName().data();
        return false;
    }
    // 4 initialization schemes: fully mixed, KV independent, KV+Boss independent, fully independent
    PCachePagePtr pPageGroup{nullptr};
    if ((nPageLevel > 0U) && (nPageLevel < kInt_4U)) {
        pPageGroup = managerPage_.InitEmptySystem(false, false, false);
    } else if ((nPageLevel >= kInt_4U) && (nPageLevel < kInt_16U)) {
        pPageGroup = managerPage_.InitEmptySystem(false, false, true);
    } else if ((nPageLevel >= kInt_16U) && (nPageLevel < kInt_1024U)) {
        pPageGroup = managerPage_.InitEmptySystem(false, true, true);
    } else {
        pPageGroup = managerPage_.InitEmptySystem(true, true, true);
    }
    if (nullptr == pPageGroup) {
        PER_OnOptFailed(managerPage_.GetLastError());
        return false;
    }
    // Verify the Block layout of the mixed page
    PPageOptBase const optPage{pPageGroup};
    if (false == optPage.CheckBlock()) {
        PER_OnOptFailed(EErrorPHKV::kPageInit);
        return false;
    }
    // First write to disk to ensure an empty library exists
    // bool const bReturn{_SyncWalogToMain(true)};
    // _LogInfo("[PH_System].NewSystem<", GetLogKvName().data(), ">Result", static_cast< int32_t >(bReturn), "PageLen",
    //          static_cast< int32_t >(nPageLen));
    // if (false == bReturn) {
    //     return false;
    // }
    return _OpenSystem();
}
/// @brief Open an existing library
/// @return
bool PKvSystem::OpenSystem() noexcept
{
    PAutoLockWrite const autoLock{
        threadLockAction_};  // Ensure read and write operations are atomic in multithreaded scenarios
    if (IsAccessReady()) {
        return true;
    }
    // Handle possible encryption hash verification configuration
    if (false == _DealStorage_CryptoVirifyHash()) {
        _OnOptFailed(EErrorPHKV::kFileCryptoConfig);
        LogError() << "[PH_System].OpenSystem._DealStorage_CryptoVirifyHash " << GetLogKvName().data();
        return false;
    }
    uint16_t const nPageLen{_ReadPageLen()};
    if (nPageLen <= 0U) {
        _OnOptFailed(EErrorPHKV::kNotReadySystem);
        LogError() << "[PH_System].OpenSystem " << GetLogKvName().data();
        return false;
    }
    uint32_t const nMaxCacheCount{kvConfigMuster_.GetMaxCacheCount()};
    if (false == managerCache_.InitCachePool(nMaxCacheCount, nPageLen)) {
        PER_OnOptFailed(managerCache_.GetLastError());
        LogError() << "[PH_System].OpenSystem " << GetLogKvName().data();
        return false;
    }
    // Discard incomplete Walog library
    PPageTailData pageTailData;
    if (false == managerCache_.WalogReadTail(pageTailData)) {
        std::ignore = _ResetWalog();  // Discard incomplete Walog library
    } else {
        if (0U == pageTailData.nSyncModel) {
            if (false == _SyncWalogToMain(false)) {
                return false;
            }
        } else {
            ara::core::Vector< uint32_t > vecMainPageID;
            std::ignore = managerCache_.WalogReadAllMainPageID(vecMainPageID);
            std::ignore = managerPage_.ResetWalogIdByDbFile(vecMainPageID);
        }
    }
    return _OpenSystem();
}
/// @brief Close the system
/// @return
bool PKvSystem::CloseSystem() noexcept
{
    PAutoLockRead const autoLock{
        threadLockAction_};  // Ensure read and write operations are atomic in multithreaded scenarios
    std::ignore = managerCache_.DbFileClose();
    std::ignore = managerCache_.ClearCachePool();
    _LogInfo("[PH_System].CloseSystem", GetLogKvName().data(), "Result", 1);
    return true;
}
/// @brief Write the content of the Walog log library back to the Main library
/// @param bOpenSystem
/// @return
bool PKvSystem::SyncWalogToMain(bool const bOpenSystem) noexcept
{
    PAutoLockWrite const autoLock{
        threadLockAction_};  // Ensure read and write operations are atomic in multithreaded scenarios
    std::ignore = managerPage_.IncVersionSystemElse();
    if (false == _SyncWalogToMain(true)) {
        return false;
    }
    if (bOpenSystem) {
        return _OpenSystem();
    }
    _OnSuccess();
    return true;
}
/// @brief Discard modifications
/// @return
bool PKvSystem::DiscardChanges() noexcept
{
    PAutoLockWrite const autoLock{
        threadLockAction_};  // Ensure read and write operations are atomic in multithreaded scenarios
    if (false == _ResetWalog()) {
        return false;
    }
    return _OpenSystem();
}
/// @brief Clear the system
/// @return
bool PKvSystem::RemoveSystem() noexcept
{
    PAutoLockWrite const autoLock{
        threadLockAction_};  // Ensure read and write operations are atomic in multithreaded scenarios
    if (false == managerPage_.ReleasePageOpt()) {
        return false;
    }
    if (false == managerKvStore_.ReleasePageOpt()) {
        return false;
    }
    // Then clear the cache
    if (false == managerCache_.ClearCachePool()) {
        return false;
    }
    if (false == managerCache_.DbFileDelete()) {
        return false;
    }
    _LogInfo("[PH_System].RemoveSystem", GetLogKvName().data(), "Result", 1);
    _OnSuccess();
    return true;
}
//***************/
/// @brief Check if modifications are required
/// @return
bool PKvSystem::IsHaveWritePage() const noexcept { return managerCache_.GetNeedWriteCachePageCount() > 0; }
/// @brief Get the total disk space occupied by the entire KV library: includes Main, Walog, Redd, Bak, etc.
/// @return
uint64_t PKvSystem::GetSpaceSize() noexcept
{
    PAutoLockRead const autoLock{
        threadLockAction_};  // Ensure read and write operations are atomic in multithreaded scenarios
    uint64_t nSpaceSize{managerCache_.GetSpaceSize()};
    if (static_cast< uint64_t >(-1) == nSpaceSize) {
        PER_OnOptFailed(managerCache_.GetLastError());
        return 0U;
    }
    // Add the size of the backup library
    ara::core::StringView stBakKvFile;
    stBakKvFile = T_StringView(GetBakFileName(ECacheSource::kMain));
    if (false == isoftkv::PFileOpt::IsFileExist(stBakKvFile)) {
        return nSpaceSize;
    }
    isoftkv::PFileOpt optBak{PFileOpt::ReadOnly()};
    if (optBak.DoPrepareWork(stBakKvFile)) {
        int64_t const nFileSize{optBak.GetFileSize()};
        if (nFileSize > 0) {
            nSpaceSize += static_cast< uint64_t >(nFileSize);
        }
    }
    return nSpaceSize;
}
/// @brief Get the total number of pages in the DB database file
/// @return
uint32_t PKvSystem::GetPageTotal() const noexcept { return managerCache_.GetMainPageTotal(); }
/// @brief Load a page of data in read-only mode
/// @param nMainPageID
/// @return Obtained memory page cache object
PCachePagePtr PKvSystem::LoadPageData(uint32_t const nMainPageID) const noexcept
{
    return managerPage_.LoadCachePage(nMainPageID, false);
}
/// @brief Get the PER format version number: App within the DB library
/// @return
PPerVersion PKvSystem::GetPerVersion_StorageApp() const noexcept
{
    ara::core::String stVersion{GetVersion_StorageApp()};
    return Version_StringToPer(T_StringView(stVersion));
}
/// @brief Get the PER format version number: Per within the DB library
/// @return
PPerVersion const &PKvSystem::GetPerVersion_StoragePer() const noexcept { return managerPage_.GetPerVersion_Per(); }
/// @brief Get the version number: App
/// @return
ara::core::String PKvSystem::GetVersion_StorageApp() const noexcept { return managerPage_.GetStringVersion_App(); }
/// @brief Get the version number: Per within the DB library
/// @return
ara::core::String PKvSystem::GetVersion_StoragePer() const noexcept { return managerPage_.GetStringVersion_Per(); }
/// @brief Get the version number: System within the DB library
/// @return
ara::core::String PKvSystem::GetVersion_StorageSys() const noexcept { return managerPage_.GetStringVersion_Sys(); }
/// @brief Update the APP/PER version numbers within the system
/// @return Success or failure
bool PKvSystem::UpdateVersion() noexcept
{
    PAutoLockWrite const autoLock{
        threadLockAction_};  // Ensure read and write operations are atomic in multithreaded scenarios
    return managerPage_.UpdateVersion(kvConfigMuster_.GetVersionApp(), kvConfigMuster_.GetVersionPer());
}
/// @brief Update the PER version number within the system: only modifies the numeric part, the string part remains unchanged
/// @return Success or failure
bool PKvSystem::UpdateVersionPer(PPerVersion const &perVersion) noexcept
{
    PAutoLockWrite const autoLock{
        threadLockAction_};  // Ensure read and write operations are atomic in multithreaded scenarios
    return managerPage_.UpdateVersionPer(perVersion);
}
/// @brief Get the version number: Per from the configuration
/// @return
ara::core::String PKvSystem::GetVersion_ConfigPer() const noexcept { return kvConfigMuster_.GetVersionPer(); }
/// @brief Get the version number: App
/// @return
ara::core::String PKvSystem::GetVersion_ConfigApp() const noexcept { return kvConfigMuster_.GetVersionApp(); }
//***************/
/// @brief Get the backup name
/// @return
ara::core::String PKvSystem::GetBakKvName() const noexcept
{
    ara::core::String stReturn{};
    stReturn = kvConfigMuster_.GetStorageName().data();
    stReturn += "_bak";
    return stReturn;
}
/// @brief Get the backup directory
/// @return
ara::core::String PKvSystem::GetBakPath() const noexcept
{
    ara::core::String stBakPath;
    switch (eKvBakeupType_) {
        case EKvBakType::kKvBakToBakFile:  // 0: Same directory as the main library, rename with *.bak suffix
        {
            stBakPath = std::move(GetWorkPath());
        } break;
        case EKvBakType::kKvBakToDirent:  // 1: No renaming, but move to the *.bak subdirectory
        {
            stBakPath = PFileOpt::MakeFileName(GetWorkPath(), T_StringView(GetBakKvName()));
        } break;
    }
    return stBakPath;
}
/// @brief Get the backup file name
/// @return
ara::core::String PKvSystem::GetBakFileName(ECacheSource const eCacheSource) const noexcept
{
    ara::core::String stBakFileName{""};
    switch (eKvBakeupType_) {
        case EKvBakType::kKvBakToBakFile:  // 0: Same directory as the main library, rename with *.bak suffix
        {
            stBakFileName = PFileOpt::MakeFileName(GetWorkPath(), T_StringView(GetBakKvName()));
            switch (eCacheSource) {
                case ECacheSource::kMain: {
                    stBakFileName += kFileExt_Main;
                } break;
                case ECacheSource::kWalog: {
                    stBakFileName += kFileExt_Walog;
                } break;
                default:
                    break;
            }
        } break;
        case EKvBakType::kKvBakToDirent:  // 1: No renaming, but move to the *.bak subdirectory
        {
            ara::core::String const stBakPath{PFileOpt::MakeFileName(GetWorkPath(), T_StringView(GetBakKvName()))};
            stBakFileName = managerCache_.GetFileName(T_StringView(stBakPath), eCacheSource, 0);
        } break;
    }
    return stBakFileName;
}
/// @brief Set the backup type, two types: backup by file (backup file will be renamed) or backup by directory (copy all files as-is to a backup directory)
/// @param eBakType
void PKvSystem::SetBakeupType(EKvBakType const eBakType) noexcept
{
    eKvBakeupType_ = eBakType;
    if (EKvBakType::kKvBakToDirent == eBakType) {
        ara::core::String stBakPath;
        stBakPath   = PFileOpt::MakeFileName(GetWorkPath(), T_StringView(GetBakKvName()));
        std::ignore = PFileOpt::MakeDir(stBakPath, true);
    }
}
/// @brief Get the backup configuration table; eBakType indicates the backup strategy
/// @return
PConfigMuster_Kv PKvSystem::GetBakConfigMuster() const noexcept
{
    PConfigMuster_Kv bkConfigMuster{kvConfigMuster_};
    switch (eKvBakeupType_) {
        case EKvBakType::kKvBakToBakFile:  // 0: Same directory as the main library, rename with *.bak suffix
        {
            bkConfigMuster.SetStorageName(GetBakKvName());
        } break;
        case EKvBakType::kKvBakToDirent:  // 1: No renaming, but move to the *.bak subdirectory
        {
            ara::core::String const stBakPath{PFileOpt::MakeFileName(GetWorkPath(), T_StringView(GetBakKvName()))};
            bkConfigMuster.SetWorkPath(stBakPath);
        } break;
    }
    // The backup library does not have M/N redundancy settings
    PReddConfigData reddConfigData;
    reddConfigData.Reset();
    bkConfigMuster.SetReddConfig(EReddStrategy::kNone, reddConfigData);
    return bkConfigMuster;
}
/// @brief Check if the backup format is valid
/// @return
bool PKvSystem::CheckBakSystem() noexcept
{
    PAutoLockRead const autoLock{
        threadLockAction_};  // Ensure read and write operations are atomic in multithreaded scenarios
    ara::core::String const stBakFile{GetBakFileName(ECacheSource::kMain)};
    PMemoryPage const memoryPage{0U, PFileOpt::GetNaiSectorSize()};
    uint32_t const nPageLen{memoryPage.GetCacheLen()};
    PFileOpt_Page fileBak{static_cast< uint16_t >(nPageLen)};
    if (false == fileBak.DoPrepareWork(T_StringView(stBakFile), static_cast< uint16_t >(nPageLen))) {
        PER_OnOptFailed(EErrorPHKV::kFileKvOpt);
        return false;
    }
    if (fileBak.GetFileSize() <= 0) {
        _OnOptFailed(EErrorPHKV::kFileSize);
        return false;
    }
    if (kvConfigMuster_.IsHaveCrypto_Storage(ECryptoKeySlotUsage::kHaveCrypto)) {
        PConfigData_Crypto const &configCrypto{kvConfigMuster_.GetCryptoConfig()};
        std::ignore = fileBak.BuildCrypto(configCrypto.stKeySlotName, configCrypto.stCryptoAlgorithm);
    }
    if (fileBak.ReadPage(1U, memoryPage.GetCache(), nPageLen) < 0) {
        PER_OnOptFailed(EErrorPHKV::kFileKvRead);
        return false;
    }
    PPageHeadData *const pHeadData{T_TransPtr< PPageHeadData >(memoryPage.GetCache())};

    return static_cast< int32_t >(pHeadData->sPageLen) > 0;
}
/// @brief Delete the backup library
/// @return
bool PKvSystem::RemoveBackupFile() const noexcept
{
    bool bRemove{false};
    if (EKvBakType::kKvBakToDirent == eKvBakeupType_) {
        bRemove = PFileOpt::RemoveDir(T_StringView(GetBakPath()));
    } else {
        bRemove = PFileOpt::DelFile(T_StringView(GetBakFileName(ECacheSource::kMain)));
        bRemove = PFileOpt::DelFile(T_StringView(GetBakFileName(ECacheSource::kWalog)));
    }
    LogInfo() << "[PKvSystem]::RemoveBackupFile: Result = " << bRemove;
    return bRemove;
}
/// @brief Backup
/// @param bDiscardChanges
/// @return
bool PKvSystem::BackupSystem(bool const bDiscardChanges) noexcept
{
    // Save all local modifications to disk before backing up
    if (bDiscardChanges) {
        if (false == DiscardChanges()) {
            return false;
        }
    } else {
        if (false == SyncWalogToMain(false)) {
            return false;
        }
    }
    PAutoLockWrite const autoLock{
        threadLockAction_};  // Ensure read and write operations are atomic in multithreaded scenarios
    if (false == managerCache_.DbFileClose()) {
        PER_OnOptFailed(EErrorPHKV::kFileRecoverRedd);
        return false;
    }
    // The original library for the backup here still needs to perform possible M/N redundancy selection
    isoftkv::PSelectMofN const selectMofN{kvConfigMuster_.GetReddCountM(), kvConfigMuster_.GetReddCountN()};
    int32_t nMainIndex{0};
    nMainIndex = selectMofN.SelectGoal(0U, [this](int32_t const nIndex) noexcept -> uint64_t {
        ara::core::String const stFileName{managerCache_.GetFileName(ECacheSource::kMain, nIndex)};
        return static_cast< uint64_t >(isoftkv::PSelectMofN::CalFileHashID(T_StringView(stFileName)));
    });
    if (nMainIndex < 0) {
        PER_OnOptFailed(EErrorPHKV::kFileRecoverRedd);
        return false;
    }
    ara::core::String const stFileMain{managerCache_.GetFileName(ECacheSource::kMain, nMainIndex)};
    ara::core::String const stFileBack{GetBakFileName(ECacheSource::kMain)};
    return PFileOpt::CopyFile(stFileMain, stFileBack);
}
/// @brief Restore the system: restore this library from the backup
/// @return
bool PKvSystem::RecoverFromBackup() noexcept
{
    // Delete the current library
    if (false == RemoveSystem()) {
        return false;
    }
    // Verify that the backup library is a valid KV library
    if (false == CheckBakSystem()) {
        return false;
    }
    // Copy
    PAutoLockWrite const autoLock{
        threadLockAction_};  // Ensure read and write operations are atomic in multithreaded scenarios
    int32_t const nMainCount{static_cast< int32_t >(kvConfigMuster_.GetReddCountN())};
    uint32_t nSuccess{0U};
    ara::core::String const stFileBack{GetBakFileName(ECacheSource::kMain)};
    for (int32_t i{0}; i < nMainCount; i++) {
        ara::core::String const stFileMain{managerCache_.GetFileName(ECacheSource::kMain, i)};
        if (PFileOpt::CopyFile(stFileBack, stFileMain)) {
            nSuccess += 1U;
        }
    }
    return nSuccess >= kvConfigMuster_.GetReddCountM();
}
/// @brief Restore the system
/// @return
bool PKvSystem::RecoverSystem_FromRedd(ara::core::Vector< uint8_t > &vecRecoverRedd) noexcept
{
    PAutoLockWrite const autoLock{
        threadLockAction_};  // Ensure read and write operations are atomic in multithreaded scenarios
    uint16_t const nPageLen{static_cast< uint16_t >(kvConfigMuster_.GetPageInitLen())};
    if (managerCache_.GetPageLen() <= 0U) {
        managerCache_.SetPageLen(nPageLen);
    }
    if (false == managerCache_.RecoverReddFile(vecRecoverRedd)) {
        _OnOptFailed(managerCache_.GetLastError());
        return false;
    }
    return true;
}
/// @brief Load update file
/// @param stFileName
/// @param bOpenSystem
/// @return
bool PKvSystem::LoadUpdateFile(ara::core::StringView const &stFileName, bool const bOpenSystem) noexcept
{
    PAutoLockWrite const autoLock{
        threadLockAction_};  // Ensure read and write operations are atomic in multithreaded scenarios
    if (false == IsAccessReady()) {
        PER_OnOptFailed(EErrorPHKV::kNotReadySystem);
        return false;
    }
    // Load the default GroupPage
    PCachePagePtr pPageGroup{nullptr};
    pPageGroup = managerPage_.LoadCachePage(static_cast< uint32_t >(EDefaultValue::kDefStartPageID), false);
    PPageOptPageGroup optPageGroup{pPageGroup};
    if (false == optPageGroup.IsAccessReady()) {
        PER_OnOptFailed(optPageGroup.GetLastError());
        return false;
    }
    // The version number updated inside _LoadUpdateFile is the one within the *.per file
    if (false
        == _LoadUpdateFile(stFileName, managerPage_.GetPerVersion_Sys(), managerPage_.GetPerVersion_Per(),
                           static_cast< uint32_t >(managerCache_.GetPageLen()))) {
        return false;
    }
    // Save changes
    if (false == _SyncWalogToMain(true)) {
        return false;
    }
    // Decide whether to reopen KvSystem based on parameters
    if (bOpenSystem) {
        if (false == _OpenSystem()) {
            return false;
        }
    }
    _OnSuccess();
    return true;
}
/// @brief Load update file: the file source is the file saved in PConfigMuster_Kv
/// @param bOpenSystem
/// @return
bool PKvSystem::LoadUpdateFile(bool const bOpenSystem) noexcept
{
    return LoadUpdateFile(kvConfigMuster_.GetInitUpdateFile(), bOpenSystem);
}
/// @brief Save update file
/// @param stFileName
/// @return
bool PKvSystem::SaveUpdateFile(ara::core::StringView const &stFileName) noexcept
{
    if (false == IsAccessReady()) {
        if (false == OpenSystem()) {
            PER_OnOptFailed(EErrorPHKV::kNotReadySystem);
            return false;
        }
    }
    PAutoLockRead const autoLock{
        threadLockAction_};  // Ensure read and write operations are atomic in multithreaded scenarios
    // Load the default GroupPage
    PCachePagePtr pPageGroup{nullptr};
    pPageGroup = managerPage_.LoadCachePage(static_cast< uint32_t >(EDefaultValue::kDefStartPageID), false);
    PPageOptPageGroup optPageGroup{pPageGroup};
    if (false == optPageGroup.IsAccessReady()) {
        PER_OnOptFailed(optPageGroup.GetLastError());
        return false;
    }
    return _SaveUpdateFile(stFileName, managerPage_.GetStringVersion_Per(), managerPage_.GetStringVersion_App());
}
/// @brief Delete a redundancy recovery request
/// @param nPageID
void PKvSystem::DelReddCheck(uint32_t const nPageID) noexcept { return managerCache_.DelReddCheck(nPageID); }
/// @brief Check if there is a redundancy recovery request
/// @return Redundancy data that needs recovery
PReddCheckData_Kv PKvSystem::FindRecoverReddPageID() const noexcept { return managerCache_.FindRecoverData(); }
/// @brief Recover specific page data
/// @param nPageID Page number
/// @return Whether successful
ara::core::Vector< uint8_t > PKvSystem::RecoverPage(uint32_t const nPageID) noexcept
{
    PAutoLockWrite const autoLock{
        threadLockAction_};  // Ensure read and write operations are atomic in multithreaded scenarios
    if (kvConfigMuster_.GetReddCountN() <= 1U) {
        return ara::core::Vector< uint8_t >{};
    }
    ara::core::Vector< uint8_t > const vecRecoverIndex{
        managerCache_.RecoverReddPage(nPageID, kvConfigMuster_.GetReddCountN(), kvConfigMuster_.GetReddCountM())};
    _OnSuccess();
    return vecRecoverIndex;
}
/// @param nPageID Page number to recover
/// @param nSelectIndex Redundancy number of the correct data
/// @param vecRecoverIndex Redundant page numbers of the erroneous data
/// @return Success or failure
bool PKvSystem::RecoverPage(uint32_t const nPageID,
                            uint8_t const nSelectIndex,
                            ara::core::Vector< uint8_t > const &vecRecoverIndex) noexcept
{
    PAutoLockWrite const autoLock{
        threadLockAction_};  // Ensure read and write operations are atomic in multithreaded scenarios
    if (kvConfigMuster_.GetReddCountN() <= 1U) {
        return false;
    }
    bool const bResult{managerCache_.RecoverReddPage(nPageID, nSelectIndex, vecRecoverIndex)};
    _OnSuccess();
    return bResult;
}
//********************************/
/// @brief Search the entire file for the value corresponding to the key: supports basic types such as int, uint32_t, int64_t, uint64_t, float, double
/// @tparam T
/// @param stKey
/// @return
/// @throws
template < typename T >
inline T PKvSystem::ReadValue(ara::core::StringView const &stKey) noexcept
{
    PAutoLockRead const autoLock{
        threadLockAction_};  // Ensure read and write operations are atomic in multithreaded scenarios
    uint8_t bBuff[kInt_64]{0U};
    int32_t nReadLen{static_cast< int32_t >(sizeof(T))};
    EDataType const eDataType{T_GetDataType< T >()};
    PER_Assert((EDataType::kInt8 <= eDataType) && (eDataType <= EDataType::kDouble));
    nReadLen = _ReadValueByKey(stKey, eDataType, static_cast< uint8_t * >(bBuff), static_cast< uint32_t >(nReadLen));
    if ((0U != bBuff[0])) {
        // This output parameter value is not subsequently checked.
    }
    if ((nReadLen < 0)) {
        PH_Error(GetLastError());
        return static_cast< T >(0);
    }
    if (static_cast< uint32_t >(nReadLen) != sizeof(T)) {
        PER_OnOptFailed(EErrorPHKV::kKvDataType);
        return static_cast< T >(0);
    }
    _OnSuccess();
    T nReadData{static_cast< T >(0)};
    std::ignore = T_Memcpy(T_TransBytes(&nReadData), static_cast< void * >(bBuff), sizeof(nReadData));
    return nReadData;
}
/// @brief
/// @param stKey
/// @return
/// @throws
template <>
ara::core::String PKvSystem::ReadValue< ara::core::String >(ara::core::StringView const &stKey) noexcept
{
    return ReadValueString(stKey);
}
/// @brief
/// @param stKey
/// @return
ara::core::String PKvSystem::ReadValueString(ara::core::StringView const &stKey) noexcept
{
    PAutoLockRead const autoLock{
        threadLockAction_};  // Ensure read and write operations are atomic in multithreaded scenarios
    int32_t nReadTotal{0};
    nReadTotal = managerKvStore_.GetKvValueLen(stKey, nullptr);
    if (nReadTotal <= 0) {
        if (EErrorPHKV::kKvElementNotFind != managerKvStore_.GetLastError()) {
            PER_OnOptFailed(EErrorPHKV::kKvElementRead);
        } else {
            _OnOptFailed(managerKvStore_.GetLastError());
        }
        return ara::core::String{};
    }

    PAutoBuff const perBuff{static_cast< uint32_t >(
        nReadTotal)};  // Buffer rounded up to a multiple of 64 (to meet the rounding requirements of most encryption algorithms)
    uint8_t *const pBuffData{perBuff.data()};
    int32_t const nReadLen{_ReadValueByKey(stKey, EDataType::kString, pBuffData, nReadTotal)};
    if (nReadLen < 0) {
        PH_Error(GetLastError());
        return ara::core::String{};
    }

    _OnSuccess();
    return T_String(pBuffData, static_cast< uint32_t >(nReadLen));
}
/// @brief Search the entire file for the value corresponding to the key: returns the actual size read
/// @param stKey
/// @param pBuff
/// @param nBufLen
/// @return
int32_t PKvSystem::ReadValueBinary(ara::core::StringView const &stKey,
                                   uint8_t *const pBuff,
                                   uint32_t const nBufLen) noexcept
{
    PAutoLockRead const autoLock{
        threadLockAction_};  // Ensure read and write operations are atomic in multithreaded scenarios
    PER_Assert((nullptr != pBuff) && (nBufLen > 0U));
    int32_t nReadLen{managerKvStore_.GetKvValueLen(stKey, nullptr)};
    if (nReadLen <= 0) {
        if (managerKvStore_.IsHaveError()) {
            if (EErrorPHKV::kKvElementNotFind != managerKvStore_.GetLastError()) {  // NOLINT [bugprone-branch-clone]
                PER_OnOptFailed(managerKvStore_.GetLastError());
            } else {
                _OnOptFailed(managerKvStore_.GetLastError());
            }
            return 0;
        }
    }
    if (nReadLen > static_cast< int32_t >(nBufLen)) {
        PER_OnOptFailed(EErrorPHKV::kKvBuffCapacity);
        return 0;
    }
    nReadLen = _ReadValueByKey(stKey, EDataType::kBinary, T_TransBytes(pBuff), nBufLen);
    return nReadLen;
}
/// @brief Read a new KV-Value
/// @param stKey
/// @return
/// @throws bool
template bool PKvSystem::ReadValue< bool >(ara::core::StringView const &stKey) noexcept;
/// @brief Read a new KV-Value
/// @param stKey
/// @return
/// @throws char8_t
template char8_t PKvSystem::ReadValue< char8_t >(ara::core::StringView const &stKey) noexcept;
/// @brief Read a new KV-Value
/// @param stKey
/// @return
/// @throws int8_t
template int8_t PKvSystem::ReadValue< int8_t >(ara::core::StringView const &stKey) noexcept;
/// @brief Read a new KV-Value
/// @param stKey
/// @return
/// @throws uint8_t
template uint8_t PKvSystem::ReadValue< uint8_t >(ara::core::StringView const &stKey) noexcept;
/// @brief Read a new KV-Value
/// @param stKey
/// @return
/// @throws int16_t
template int16_t PKvSystem::ReadValue< int16_t >(ara::core::StringView const &stKey) noexcept;
/// @brief Read a new KV-Value
/// @param stKey
/// @return
/// @throws uint16_t
template uint16_t PKvSystem::ReadValue< uint16_t >(ara::core::StringView const &stKey) noexcept;
/// @brief Read a new KV-Value
/// @param stKey
/// @return
/// @throws int32_t
template int32_t PKvSystem::ReadValue< int32_t >(ara::core::StringView const &stKey) noexcept;
/// @brief Read a new KV-Value
/// @param stKey
/// @return
/// @throws uint32_t
template uint32_t PKvSystem::ReadValue< uint32_t >(ara::core::StringView const &stKey) noexcept;
/// @brief Read a new KV-Value
/// @param stKey
/// @return
/// @throws int64_t
template int64_t PKvSystem::ReadValue< int64_t >(ara::core::StringView const &stKey) noexcept;
/// @brief Read a new KV-Value
/// @param stKey
/// @return
/// @throws uint64_t
template uint64_t PKvSystem::ReadValue< uint64_t >(ara::core::StringView const &stKey) noexcept;
/// @brief Read a new KV-Value
/// @param stKey
/// @return
/// @throws float
template float PKvSystem::ReadValue< float >(ara::core::StringView const &stKey) noexcept;
/// @brief Read a new KV-Value
/// @param stKey
/// @return
/// @throws double
template double PKvSystem::ReadValue< double >(ara::core::StringView const &stKey) noexcept;
//***************/
/// @brief Search the entire file for the value corresponding to the key
/// @tparam T
/// @param stKey
/// @param value
/// @return
template < typename T >  // Supports basic types such as int, uint32_t, int64_t, uint64_t, float, double
inline bool PKvSystem::WriteValue(ara::core::StringView const &stKey, T const &value) noexcept
{
    PAutoLockWrite const autoLock{
        threadLockAction_};  // Ensure read and write operations are atomic in multithreaded scenarios
    EDataType const eValueType{T_GetDataType< T >()};
    return _WriteValue(stKey, eValueType, T_TransBytes(&value), sizeof(value));
}
/// @brief Write a new KV-Element: string
/// @param stKey
/// @param stValue
/// @return
template <>
bool PKvSystem::WriteValue< ara::core::String >(ara::core::StringView const &stKey,
                                                ara::core::String const &stValue) noexcept
{
    PAutoLockWrite const autoLock{
        threadLockAction_};  // Ensure read and write operations are atomic in multithreaded scenarios
    return _WriteValue(stKey, EDataType::kString, T_TransBytes(stValue.data()),
                       static_cast< uint32_t >(stValue.size()));
}
/// @brief Write a new KV-Element: string
/// @param stKey
/// @param stValue
/// @return
template <>
bool PKvSystem::WriteValue< ara::core::StringView >(ara::core::StringView const &stKey,
                                                    ara::core::StringView const &stValue) noexcept
{
    PAutoLockWrite const autoLock{
        threadLockAction_};  // Ensure read and write operations are atomic in multithreaded scenarios
    return _WriteValue(stKey, EDataType::kString, T_TransBytes(stValue.data()),
                       static_cast< uint32_t >(stValue.size()));
}
/// @brief Write a new KV-Element: bool
/// @param stKey
/// @param value
/// @return
template bool PKvSystem::WriteValue< bool >(ara::core::StringView const &stKey, const bool &value) noexcept;
/// @brief Write a new KV-Element: char8_t
/// @param stKey
/// @param value
/// @return
template bool PKvSystem::WriteValue< char8_t >(ara::core::StringView const &stKey, char8_t const &value) noexcept;
/// @brief Write a new KV-Element: inti_t
/// @param stKey
/// @param value
/// @return
template bool PKvSystem::WriteValue< int8_t >(ara::core::StringView const &stKey, int8_t const &value) noexcept;
/// @brief Write a new KV-Element: uint_t
/// @param stKey
/// @param value
/// @return
template bool PKvSystem::WriteValue< uint8_t >(ara::core::StringView const &stKey, uint8_t const &value) noexcept;
/// @brief Write a new KV-Element: int16_t
/// @param stKey
/// @param value
/// @return
template bool PKvSystem::WriteValue< int16_t >(ara::core::StringView const &stKey, int16_t const &value) noexcept;
/// @brief Write a new KV-Element: uint16_t
/// @param stKey
/// @param value
/// @return
template bool PKvSystem::WriteValue< uint16_t >(ara::core::StringView const &stKey, uint16_t const &value) noexcept;
/// @brief Write a new KV-Element: int32_t
/// @param stKey
/// @param value
/// @return
template bool PKvSystem::WriteValue< int32_t >(ara::core::StringView const &stKey, int32_t const &value) noexcept;
/// @brief Write a new KV-Element: uint32_t
/// @param stKey
/// @param value
/// @return
template bool PKvSystem::WriteValue< uint32_t >(ara::core::StringView const &stKey, uint32_t const &value) noexcept;
/// @brief Write a new KV-Element: int64_t
/// @param stKey
/// @param value
/// @return
template bool PKvSystem::WriteValue< int64_t >(ara::core::StringView const &stKey, int64_t const &value) noexcept;
/// @brief Write a new KV-Element: uint64_t
/// @param stKey
/// @param value
/// @return
template bool PKvSystem::WriteValue< uint64_t >(ara::core::StringView const &stKey, uint64_t const &value) noexcept;
/// @brief Write a new KV-Element: float
/// @param stKey
/// @param value
/// @return
template bool PKvSystem::WriteValue< float >(ara::core::StringView const &stKey, float const &value) noexcept;
/// @brief Write a new KV-Element: double
/// @param stKey
/// @param value
/// @return
template bool PKvSystem::WriteValue< double >(ara::core::StringView const &stKey, double const &value) noexcept;
/// @brief Write a new KV-Element: string
/// @param stKey
/// @param stValue
/// @return
template bool PKvSystem::WriteValue<>(ara::core::StringView const &stKey,          // NOLINT
                                      ara::core::String const &stValue) noexcept;  // NOLINT
/// @brief Write a new KV-Element: string
/// @param stKey
/// @param stValue
/// @return
template bool PKvSystem::WriteValue<>(ara::core::StringView const &stKey,              // NOLINT
                                      ara::core::StringView const &stValue) noexcept;  // NOLINT
/// @brief Write a new KV-Element: specific type data
/// @param stKey
/// @param eValueType
/// @param pBValue
/// @param nValueLen
/// @return
bool PKvSystem::WriteValue(ara::core::StringView const &stKey,
                           EDataType const eValueType,
                           uint8_t const *const pBValue,
                           uint32_t const nValueLen) noexcept
{
    PAutoLockWrite const autoLock{
        threadLockAction_};  // Ensure read and write operations are atomic in multithreaded scenarios
    return _WriteValue(stKey, eValueType, pBValue, nValueLen);
}
/// @brief Write a new KV-Element: binary data
/// @param stKey
/// @param pBValue
/// @param nValueLen
/// @return
bool PKvSystem::WriteValue(ara::core::StringView const &stKey,
                           uint8_t const *const pBValue,
                           uint32_t const nValueLen) noexcept
{
    PAutoLockWrite const autoLock{
        threadLockAction_};  // Ensure read and write operations are atomic in multithreaded scenarios
    return _WriteValue(stKey, EDataType::kBinary, pBValue, nValueLen);
}
/// @brief Write a new KV-Element: placeholder for empty data with total length nValueLen
/// @param stKey
/// @param eValueType
/// @param nValueLen
/// @return
bool PKvSystem::WriteValue(ara::core::StringView const &stKey,
                           EDataType const eValueType,
                           uint32_t const nValueLen) noexcept
{
    PAutoLockWrite const autoLock{
        threadLockAction_};  // Ensure read and write operations are atomic in multithreaded scenarios
    return _WriteValue(stKey, eValueType, nullptr, nValueLen);
}
//********************************/
/// @brief Search the entire file for the redundancy check data corresponding to the key
/// @param stKey
/// @return Read redundancy check data
ara::core::Vector< uint8_t > PKvSystem::ReadReddData(ara::core::StringView const &stKey) noexcept
{
    ara::core::Vector< uint8_t > vecRead;  // 2023-01-18 hanjingjing: Possible encryption/decryption is not handled here
    if (false == managerKvStore_.ReadReddDataByKey(stKey, vecRead)) {
        if (EErrorPHKV::kKvElementNotFind == managerKvStore_.GetLastError()) {  // NOLINT [bugprone-branch-clone]
            _OnOptFailed(managerKvStore_.GetLastError());
        } else {
            PER_OnOptFailed(managerKvStore_.GetLastError());
        }
        return vecRead;
    }
    _OnSuccess();
    return vecRead;
}
/// @brief Get the length of the value corresponding to the key
/// @param stKey
/// @return
int32_t PKvSystem::GetValueLen(ara::core::StringView const &stKey) noexcept
{
    PAutoLockRead const autoLock{
        threadLockAction_};  // Ensure read and write operations are atomic in multithreaded scenarios
    return managerKvStore_.GetKvValueLen(stKey, nullptr);
}
/// @brief Check if a key exists
/// @param stKey
/// @return
bool PKvSystem::IsKeyExist(ara::core::StringView const &stKey) noexcept
{
    PAutoLockRead const autoLock{
        threadLockAction_};  // Ensure read and write operations are atomic in multithreaded scenarios
    bool const bFind{managerKvStore_.IsKeyExist(stKey)};
    if ((false == bFind) && (managerKvStore_.IsHaveError())) {
        PER_OnOptFailed(managerKvStore_.GetLastError());
        return false;
    }
    _OnSuccess();
    return bFind;
}
/// @brief Extended interface for reading/writing cross-page Values, finds the storage locations of all Blocks of the Value via stKey
/// @param stKey
/// @param vecKeyList
/// @return
bool PKvSystem::FindBlockByKey(ara::core::StringView const &stKey,
                               ara::core::Vector< PBlockValue > &vecBlockList) noexcept
{
    PAutoLockRead const autoLock{
        threadLockAction_};  // Ensure read and write operations are atomic in multithreaded scenarios
    int32_t nReadTotal{0};
    bool bReturn{false};
    bReturn = managerKvStore_.ReadValueByKey(
        stKey,
        [&nReadTotal, &vecBlockList](int32_t, uint8_t const *const, int32_t const nReadLen, uint32_t const nPageID,
                                     uint16_t const nBlockID) -> int32_t {
            if ((PPageOptBase::IsValidPageID(nPageID)) && (nBlockID != kInt_0xFFFFU)) {
                vecBlockList.push_back(PBlockValue{nPageID, nBlockID, static_cast< uint16_t >(nReadLen)});
            }
            nReadTotal += nReadLen;
            return nReadLen;
        });
    if ((false == bReturn) || (nReadTotal <= 0)) {
        PER_OnOptFailed(managerKvStore_.GetLastError());
        return false;
    }
    _OnSuccess();
    return true;
}
/// @brief Directly operate on a certain page of Value, replacing the data within it
/// @param blockValue
/// @param nOffset
/// @param pBValue
/// @param nValueLen
/// @return
bool PKvSystem::AmendValueData(PBlockValue const &blockValue,
                               int32_t const nOffset,
                               uint8_t const *const pBValue,
                               int32_t const nValueLen) noexcept
{
    PAutoLockWrite const autoLock{
        threadLockAction_};  // Ensure read and write operations are atomic in multithreaded scenarios
    if ((false == PPageOptBase::IsValidPageID(blockValue.nPageID)) || (blockValue.sBlockID == kInt_0xFFFFU)
        || (blockValue.sDataLen <= 0U)) {
        PER_OnOptFailed(EErrorPHKV::kNullParam);
        return false;
    }
    PCachePagePtr pPageValue;
    pPageValue = managerPage_.LoadCachePage(blockValue.nPageID, true);
    PPageOptValue optValue{pPageValue};
    if (false == optValue.IsAccessReady()) {
        PER_OnOptFailed(EErrorPHKV::kNotReadyPageValue);
        return false;
    }
    if (false
        == optValue.AmendBlock(blockValue.sBlockID, static_cast< uint16_t >(nOffset), pBValue,
                               static_cast< uint16_t >(nValueLen))) {
        PER_OnOptFailed(optValue.GetLastError());
        return false;
    }
    _OnSuccess();
    return true;
}
//***************/
/// @brief Scan all Keys and execute a callback function for each
/// @param pfun
/// @return
bool PKvSystem::ScanAllKey(CB_DealStringView const &pfun) noexcept
{
    PAutoLockWrite const autoLock{
        threadLockAction_};  // Ensure read and write operations are atomic in multithreaded scenarios
    int32_t nScanCount{0};
    nScanCount
        = managerKvStore_.ScanAllKvPair([this, pfun](PCachePagePtr const &pKvPage, int32_t const nKvIndex) -> int32_t {
              bool const bResult{pfun(isoftkv::T_StringView(managerKvStore_.GetKvKeyString(pKvPage, nKvIndex)))};
              if (bResult) {
                  return 1;
              }
              return 0;
          });
    if (nScanCount < 0) {
        PER_OnOptFailed(managerKvStore_.GetLastError());
        return false;
    }
    _OnSuccess();
    return true;
}
/// @brief Enumerate all key values
/// @param vecKeyList
/// @return
bool PKvSystem::EnumAllKey(ara::core::Vector< ara::core::String > &vecKeyList) noexcept
{
    vecKeyList.clear();
    PAutoLockRead const autoLock{
        threadLockAction_};  // Ensure read and write operations are atomic in multithreaded scenarios
    int32_t nScanCount{0};
    nScanCount = managerKvStore_.ScanAllKvPair(
        [this, &vecKeyList](PCachePagePtr const &pKvPage, int32_t const nKvIndex) -> int32_t {
            ara::core::String stKey;
            stKey = managerKvStore_.GetKvKeyString(pKvPage, nKvIndex);
            vecKeyList.push_back(std::move(stKey));
            return 1;
        });
    if (nScanCount < 0) {
        PER_OnOptFailed(managerKvStore_.GetLastError());
        return false;
    }
    _OnSuccess();
    return true;
}
/// @brief Delete a KV-Element
/// @param stKey
/// @return
bool PKvSystem::RemoveKey(ara::core::StringView const &stKey) noexcept
{
    PAutoLockWrite const autoLock{
        threadLockAction_};  // Ensure read and write operations are atomic in multithreaded scenarios
    if (false == managerKvStore_.DeleteKvElement(stKey)) {
        if (EErrorPHKV::kKvElementNotFind != managerKvStore_.GetLastError()) {  // NOLINT [bugprone-branch-clone]
            // Note: This error code is not passed to KvSystem
            PER_OnOptFailed(managerKvStore_.GetLastError());
        } else {
            _OnOptFailed(managerKvStore_.GetLastError());
        }
        return false;
    }
    _OnSuccess();
    return true;
}
/// @brief Delete all Keys in this center (operation takes effect immediately)
/// @return
bool PKvSystem::RemoveAllKeys() noexcept
{
    uint32_t nPageTotal{0U};
    {
        PAutoLockRead const autoLock{
            threadLockAction_};  // Ensure read and write operations are atomic in multithreaded scenarios
        nPageTotal = managerCache_.GetMainPageTotal();
    }
    // First clear the file
    if (false == RemoveSystem()) {
        return false;
    }
    // Close the file
    if (false == NewSystem(nPageTotal)) {
        return false;
    }
    _LogInfo("[PH_System].RemoveAllKeys", GetLogKvName().data(), "Result", 1);
    _OnSuccess();
    return true;
}
/// @brief Restore a KV-Element
/// @param stKey
/// @return
bool PKvSystem::RecoverKey(ara::core::StringView const &stKey, ara::core::Vector< uint8_t > &vecRecoverRedd) noexcept
{
    vecRecoverRedd.clear();
    if (false == IsKeyExist(stKey)) {
        return false;
    }
    // If M/N redundancy is not configured, reset the KV pair directly
    if (kvConfigMuster_.GetReddCountN() <= 1) {
        return false;
    }
    // Ensure read and write operations are atomic in multithreaded scenarios
    PAutoLockRead const autoLock{threadLockAction_};
    // M/N election is done on a per-page basis internally, so redundancy recovery requires checking and recovering all PageIDs encountered while reading stKey
    ara::core::Vector< uint32_t > const vecPageID{managerKvStore_.EnumReadValuePageID(stKey)};
    if (vecPageID.empty()) {
        return false;
    }
    ara::core::Map< uint32_t, int32_t > mapPageID;
    for (uint32_t nPageID : vecPageID) {
        ara::core::Map< uint32_t, int32_t >::const_iterator const itFind{mapPageID.find(nPageID)};
        if (mapPageID.end() == itFind) {
            ara::core::Vector< uint8_t > const vecReddIndex{managerCache_.RecoverReddPage(
                nPageID, kvConfigMuster_.GetReddCountN(), kvConfigMuster_.GetReddCountM())};
            for (auto const &reddIndex : vecReddIndex) {
                if (vecRecoverRedd.end() == std::find(vecRecoverRedd.begin(), vecRecoverRedd.end(), reddIndex)) {
                    vecRecoverRedd.push_back(reddIndex);
                }
            }
            mapPageID[nPageID] = 0;
        }
        mapPageID[nPageID] += 1;
    }
    return true;
}
//***************/
/// @brief Reset a KV-Element: creates if it does not exist; if it exists, sets the Value to its initial value
/// @param stKey
/// @return
bool PKvSystem::ResetKey(ara::core::StringView const &stKey) noexcept
{
    // if (false == IsKeyExist(stKey)) {
    //     return false;
    // }
    // Ensure read and write operations are atomic in multithreaded scenarios
    PAutoLockWrite const autoLock{threadLockAction_};
    // The first line is version information, followed by Key="Type/Length/Data", Value = "Type/Length/Data"
    PKvUpdateFile updateFile{T_StringView(GetSystemName()), kvConfigMuster_.GetInitUpdateFile(), true};
    if (false == updateFile.IsAccessReady()) {
        _OnOptFailed(EErrorPHKV::kFileUpdateOpen);
        return false;
    }
    // Find the initial value corresponding to stKey from the update file
    EDataType eDataType{EDataType::kDefault};
    ara::core::Vector< uint8_t > vecFindData{updateFile.FindLineData(stKey, managerCache_.GetPageLen(), eDataType)};
    if (eDataType != EDataType::kDefault) {
        WriteValue(stKey, eDataType, T_TransBytes(vecFindData.data()), static_cast< uint32_t >(vecFindData.size()));
    }
    return true;
}
/// @brief Enumerate all PageIDs encountered during reading the Value of a KV-Element
/// @param stKey Key name
/// @return List of PageIDs involved in order
ara::core::Vector< uint32_t > PKvSystem::EnumReadValuePageID(ara::core::StringView const &stKey) noexcept
{
    PAutoLockRead const autoLock{
        threadLockAction_};  // Ensure read and write operations are atomic in multithreaded scenarios
    return managerKvStore_.EnumReadValuePageID(stKey);
}

/// @brief Get the size of the main library, excluding redundant backups. Returns the size of the xxxxx.main file.
/// @return Returns the size of the main file
/// @code{.isoft}
/// @needwork = dda
/// @endcode
int64_t PKvSystem::MainFileSize() noexcept { return managerCache_.GetMainFileSize(); }
//***************/
/// @brief Print the Key values within a KV page
/// @return
int32_t PKvSystem::Debug_PrintKvStore() noexcept { return managerKvStore_.Debug_PrintKvStore(); }
/// @brief Iterate over all caches
void PKvSystem::Debug_ScanCache() noexcept { managerCache_.Debug_ScanCache(); }
//********************************/
/// @brief Open an existing library
/// @return
bool PKvSystem::_OpenSystem() noexcept
{
    // Resident memory PageGroup
    if (false == managerPage_.InitPageOptGroup()) {
        PER_OnOptFailed(managerPage_.GetLastError());
        return false;
    }
    // Resident memory PageHash
    if (false == managerKvStore_.InitOptPageHash()) {
        PER_OnOptFailed(managerKvStore_.GetLastError());
        return false;
    }
    // Set the statistically obtained backup file size
    ara::core::String const stBakFileName{GetBakFileName(ECacheSource::kMain)};
    isoftkv::PFileOpt fileOptBak{isoftkv::PFileOpt::ReadOnly()};
    int64_t nBakFileLen{0};
    if (fileOptBak.DoPrepareWork(isoftkv::T_StringView(stBakFileName))) {
        nBakFileLen = fileOptBak.GetFileSize();
    }
    managerPage_.SetBakFileLen(nBakFileLen);
    _OnSuccess();
    return true;
}
/// @brief Reset the Walog library: discard in-memory modifications and reinitialize the Walog page
/// @return
bool PKvSystem::_ResetWalog() noexcept
{
    std::ignore = managerPage_.ReleasePageOpt();
    std::ignore = managerKvStore_.ReleasePageOpt();
    std::ignore = managerCache_.WalogResetCache();  // Destroy all Walog caches
    if (false == managerPage_.InitWalogDB()) {
        PER_OnOptFailed(managerPage_.GetLastError());
        return false;
    }
    return true;
}
/// @brief Save all modified Cache pages to the Walog library
/// @param nSyncModel
/// @param bForce
/// @return
bool PKvSystem::_SaveCacheToWalog(uint32_t const nSyncModel, bool const bForce) noexcept
{
    int32_t const nSaveCount{managerCache_.WalogSaveCachePage(bForce)};
    if (nSaveCount == 0) {
        return true;
    }
    if (nSaveCount < 0) {
        PER_OnOptFailed(managerCache_.GetLastError());
        return false;
    }
    PPageTailData pageTailData;
    pageTailData.nPageSaveCount = static_cast< int32_t >(managerCache_.GetWalogPageTotal());
    pageTailData.tmOptTime      = isoftkv::TM_NowSecond();
    pageTailData.nSyncModel     = nSyncModel;
    pageTailData.nTailCrc       = CalculateCrc32(&pageTailData, sizeof(PPageTailData), 0U);
    if (false == managerCache_.WalogWriteTail(pageTailData)) {
        PER_OnOptFailed(managerCache_.GetLastError());
        return false;
    }
    return true;
}
/// @brief Write the content of the Walog log library back to the Main library
/// @param pPageWork
/// @return
bool PKvSystem::_SaveWalogPage(PCachePagePtr const &pPageWork) noexcept
{
    PER_Assert(nullptr != pPageWork);
    PMemoryPage *const pMemoryPage{pPageWork->GetMemoryPage()};
    PER_Assert(nullptr != pMemoryPage);
    if (false == PPageOptBase::IsValidPageID(pMemoryPage->GetMainPageID())) {
        return false;
    }
    // Update the page's CRC32 before writing to disk
    PPageOptBase const pageOptBase{pPageWork};
    if (false == pageOptBase.CheckPageCRC32()) {
        pageOptBase.UpdatePageCRC32();
    }
    // Save the main library
    std::ignore = managerCache_.SaveMainPage(pMemoryPage, pMemoryPage->GetMainPageID());
    // If redundant libraries exist, handle them here
    // Maintain Cache status
    pMemoryPage->SetPageSource(ECacheSource::kMain);
    pMemoryPage->SetWalogPageID(kInvalidPageID);
    pMemoryPage->SetWriteRef(0);
    return true;
}
/// @brief Attempt to write the content of the Walog log library back to the Main library
/// @param bSaveCache
/// @return
bool PKvSystem::_SyncWalogToMain(bool const bSaveCache) noexcept
{
    // First write Cache to the Walog library, then transfer the Walog library content to the Main library
    if (bSaveCache) {
        if (false == _SaveCacheToWalog(0U, false)) {
            return false;
        }
    }
    // First check: whether synchronizing the Walog library is allowed
    PPageTailData pageTailData;
    if (false == managerCache_.WalogReadTail(pageTailData)) {
        return false;
    }
    if ((pageTailData.nPageSaveCount <= 0) || (pageTailData.tmOptTime <= 0U)) {
        _LogInfo("[PH_System].SyncWalogToMain", ": Not Necessary", "ageTailData", 0);
        return true;
    }
    // Save page by page
    _LogInfo("[PH_System].SyncWalogToMain", ": Begin", " nPageSaveCount", pageTailData.nPageSaveCount);
    int32_t nLogStep{kInt_32};
    if (pageTailData.nPageSaveCount > kInt_32 * kInt_3) {
        nLogStep = pageTailData.nPageSaveCount / kInt_3;
    }
    for (int32_t i{0}; i < pageTailData.nPageSaveCount; i++) {
        PCachePagePtr const pPageWork{managerPage_.LoadWalogFilePage(TransPage_IndexToID(i))};
        std::ignore = _SaveWalogPage(pPageWork);
        if (((i % nLogStep) == 0) || (i == (pageTailData.nPageSaveCount - 1))) {
            _LogDebug("[PH_System].SyncWalogToMain", ": ", "TotalCount", pageTailData.nPageSaveCount, "SyncIndex", i);
        }
    }
    _LogInfo("[PH_System].SyncWalogToMain", ": End  ", "TotalCount", pageTailData.nPageSaveCount);
    // Change cache properties to Main library
    std::ignore = managerCache_.WalogCacheToMain();
    // Reset the Walog library
    std::ignore = _ResetWalog();
    return true;
}
/// @brief Read the PageLen of a known library; the return value can determine whether a database is a valid legal library
/// @return
uint16_t PKvSystem::_ReadPageLen() noexcept
{
    PPageHeadData const pageHead{managerCache_.ReadFileHead()};
    // Make an extra copy to prevent out-of-bounds from dirty data
    uint32_t const nMagicLen{
        static_cast< uint32_t >(strnlen(static_cast< char8_t const * >(pageHead.cMagic), sizeof(pageHead.cMagic)))};
    ara::core::StringView const stMagic{
        std::move(isoftkv::T_StringView(static_cast< void const * >(pageHead.cMagic), nMagicLen))};
    if ((false == IsKvPage(stMagic)) || ('\0' == pageHead.cMagic[0])) {
        // kFileSize error may be caused by an empty file during initialization
        if (managerCache_.GetLastError() != EErrorPHKV::kFileSize) {
            PER_OnOptFailed(managerCache_.GetLastError());
        }
        return 0U;
    }
    _OnSuccess();
    return pageHead.sPageLen;
}
//********************************/
/// @brief Class for processing/operating line data in an UpdateFile: specialized for update operation business logic
class POptUpdateKvElement final
{
private:
    /// @brief Update strategy
    EUpdateAction eUpdateAction_;
    /// @brief KeyData
    ara::core::String stKey_;
    /// @brief Total length written to the database
    int32_t nLineValueTotal_;
    /// @brief Cursor for data already written
    int32_t nWriteValueOffset_;
    /// @brief Working cursor for storage location within the database
    int32_t nWorkBlockValueIndex_;
    /// @brief List of page locations where data is stored within the database
    ara::core::Vector< PBlockValue > vecBlockValue_;
    /// @brief CRC checksum read from the file
    uint32_t nReadCrc32_;

private:
    /// @brief
    PKvSystem *pKvSystem_;
    /// @brief Maximum size of a single page
    uint32_t nPageMaxValue_;
    /// @brief Cumulative number of KV pairs
    int32_t nKeyCount_;

    /// @brief
    void _reset() noexcept
    {
        eUpdateAction_ = EUpdateAction::kIgnore;
        stKey_.clear();
        nLineValueTotal_      = 0;
        nWriteValueOffset_    = 0;
        nWorkBlockValueIndex_ = 0;
        vecBlockValue_.clear();
        nReadCrc32_ = 0U;
    }

public:
    /// @brief Delete default constructor
    POptUpdateKvElement() = delete;
    /// @brief Parameterized constructor
    /// @param kvSystem
    /// @param nPageMaxValue
    POptUpdateKvElement(PKvSystem *const kvSystem, uint32_t const nPageMaxValue) noexcept
        : eUpdateAction_{EUpdateAction::kIgnore}
        , nLineValueTotal_{0}
        , nWriteValueOffset_{0}
        , nWorkBlockValueIndex_{0}
        , nReadCrc32_{0U}
        , pKvSystem_{kvSystem}
        , nPageMaxValue_{nPageMaxValue}
        , nKeyCount_{0}
    {
    }
    /// @brief Get the number of keys processed
    /// @return
    inline int32_t GetKeyCount() const noexcept { return nKeyCount_; }
    /// @brief Get the currently operating key
    /// @return
    inline ara::core::String const &GetKey() const noexcept { return stKey_; }
    /// @brief Process the line-read data (will not be interrupted)
    /// @param nLineID
    /// @param pOptKvUpdateRead
    /// @return
    int32_t DealReadData(int32_t const nLineID, PKvUpdateRead_Base const *const pOptKvUpdateRead) noexcept
    {
        PER_Assert(nullptr != pOptKvUpdateRead);
        // eUpdateWord is used to distinguish the type of pOptKvUpdateRead object
        EUpdateKeyWord const eUpdateWord{pOptKvUpdateRead->GetUpdateWord()};
        if ((EUpdateAction::kIgnore == eUpdateAction_) && (EUpdateKeyWord::kAction != eUpdateWord)) {
            return 0;
        }
        int32_t const nDataType{pOptKvUpdateRead->GetReadDataType()};
        int32_t const nTotalLen{static_cast< int32_t >(pOptKvUpdateRead->GetReadTotalLen())};
        uint8_t const *const pBRead{pOptKvUpdateRead->GetReadData()};
        int32_t const nReadLen{static_cast< int32_t >(pOptKvUpdateRead->GetReadLen())};
        switch (eUpdateWord) {
            case EUpdateKeyWord::kAction: {
                std::ignore = _DealReadAction(nDataType, nTotalLen, pBRead, nReadLen);
            } break;
            case EUpdateKeyWord::kKey: {
                std::ignore = _DealReadKey(nDataType, nTotalLen, pBRead, nReadLen);
                LogDebug() << "KvName =" << pKvSystem_->GetLogKvName().data() << "[PH_System].LoadUpdate: Key"
                           << stKey_.data() << ", nLineID" << nLineID;
            } break;
            case EUpdateKeyWord::kValue: {
                // After receiving the CRC and successfully verifying it, proceed with the second traversal of the Value data
                std::ignore = _DealReadValue(nDataType, nTotalLen, pBRead, nReadLen);
            } break;
            case EUpdateKeyWord::kCrc: {
                std::ignore = _DealReadCrc(nDataType, nTotalLen, pBRead, nReadLen);
            } break;
        }
        return nReadLen;
    }

protected:
    /// @brief Process update action data
    /// @param nDataType
    /// @param nTotalLen
    /// @param pBRead
    /// @param nReadLen
    /// @return
    int32_t _DealReadAction(int32_t const nDataType,
                            int32_t const nTotalLen,
                            uint8_t const *const pBRead,
                            int32_t const nReadLen) noexcept
    {
        std::ignore = nDataType;
        std::ignore = nTotalLen;
        if ((nullptr == pBRead) || (nReadLen <= 0)) {
            return 0;
        }
        _reset();
        ara::core::StringView const stAction{std::move(T_StringView(pBRead, static_cast< uint32_t >(nReadLen)))};
        eUpdateAction_ = GetUpdateAction(stAction.at(0U));
        nKeyCount_ += 1;
        return nReadLen;
    }
    /// @brief Process Key
    /// @param nDataType
    /// @param nTotalLen
    /// @param pBRead
    /// @param nReadLen
    /// @return
    int32_t _DealReadKey(int32_t const nDataType,
                         int32_t const nTotalLen,
                         uint8_t const *const pBRead,
                         int32_t const nReadLen) noexcept
    {
        std::ignore = nDataType;
        std::ignore = nTotalLen;
        if ((nullptr == pBRead) || (nReadLen <= 0)) {
            return 0;
        }
        if ((EUpdateAction::kKeepExisting == eUpdateAction_) || (EUpdateAction::kOverWrite == eUpdateAction_)
            || (EUpdateAction::kDelete == eUpdateAction_)) {
            stKey_ = T_String(pBRead, static_cast< uint32_t >(nReadLen));
        }
        return nReadLen;
    }
    /// @brief Process Value
    /// @param nDataType
    /// @param nTotalLen
    /// @param pBRead
    /// @param nReadLen
    /// @return
    int32_t _DealReadValue(int32_t const nDataType,
                           int32_t const nTotalLen,
                           uint8_t const *const pBRead,
                           int32_t const nReadLen) noexcept
    {
        bool bNeedAdd{false};
        switch (eUpdateAction_) {
            case EUpdateAction::kIgnore: {
            } break;
            case EUpdateAction::kKeepExisting: {
                if (pKvSystem_->IsKeyExist(T_StringView(stKey_))) {
                    return nReadLen;
                }
                bNeedAdd = true;
            } break;
            case EUpdateAction::kOverWrite: {
                bNeedAdd = true;
            } break;
            case EUpdateAction::kDelete: {
                if (pKvSystem_->RemoveKey(T_StringView(stKey_))) {
                    LogInfo() << "Kv library [" << pKvSystem_->GetLogKvName().data()
                              << "], failed to delete during initialization of Key[" << stKey_.c_str() << "].";
                }
            } break;
        }
        if (bNeedAdd) {
            if ((nullptr == pBRead) || (nReadLen <= 0)) {
                return 0;
            }
            PPageOptValue const optValue{nullptr};
            EDataType const eDataType{TransToDataType(nDataType % kInt_0x100)};
            PER_Assert_LOG(eDataType != EDataType::kDefault);
            // If the Value length exceeds the limit, use another method to insert the KV pair
            if (nTotalLen > static_cast< int32_t >(nPageMaxValue_)) {
                // Prepare the environment before the first write
                if (nLineValueTotal_ <= 0) {
                    PER_Assert_LOG(
                        pKvSystem_->WriteValue(T_StringView(stKey_), eDataType, static_cast< uint32_t >(nTotalLen)));
                    PER_Assert_LOG(pKvSystem_->FindBlockByKey(T_StringView(stKey_), vecBlockValue_));
                    nLineValueTotal_   = 0;
                    nWriteValueOffset_ = 0;
                }
                int32_t nWriteLen{0};
                while (true) {
                    if (nWriteLen >= nReadLen) {
                        break;
                    }
                    if (nWorkBlockValueIndex_ >= static_cast< int32_t >(vecBlockValue_.size())) {
                        break;
                    }
                    PBlockValue &blockValue{vecBlockValue_[static_cast< std::size_t >(nWorkBlockValueIndex_)]};
                    int32_t const nAmendLen{std::min< int32_t >(
                        nReadLen - nWriteLen, static_cast< int32_t >(blockValue.sDataLen) - nWriteValueOffset_)};
                    bool const bAmendResult{
                        pKvSystem_->AmendValueData(blockValue, nWriteValueOffset_, pBRead + nWriteLen, nAmendLen)};
                    PER_Assert_LOG(bAmendResult);
                    if ((nWriteValueOffset_ + nAmendLen) >= static_cast< int32_t >(blockValue.sDataLen)) {
                        nWriteValueOffset_ = 0;
                        nWorkBlockValueIndex_ += 1;
                    } else {
                        nWriteValueOffset_ += nAmendLen;
                    }
                    nWriteLen += nAmendLen;
                }
                nLineValueTotal_ += nWriteLen;
            } else {
                PER_Assert_LOG(
                    pKvSystem_->WriteValue(T_StringView(stKey_), eDataType, pBRead, static_cast< uint32_t >(nReadLen)));
            }
        }
        return nReadLen;
    }
    /// @brief Process Crc
    /// @param nDataType
    /// @param nTotalLen
    /// @param pBRead
    /// @param nReadLen
    /// @return
    int32_t _DealReadCrc(int32_t const nDataType,
                         int32_t const nTotalLen,
                         uint8_t const *const pBRead,
                         int32_t const nReadLen) noexcept
    {
        std::ignore = nDataType;
        std::ignore = nTotalLen;
        if ((nullptr == pBRead) || (nReadLen <= 0)) {
            return 0;
        }
        ara::core::StringView const stCrc{std::move(T_StringView(pBRead, static_cast< uint32_t >(nReadLen)))};
        nReadCrc32_ = isoftkv::T_stoi< uint32_t >(stCrc.data(), static_cast< uint32_t >(stCrc.size()), kInt_10);
        // Verify whether the CRC is valid; if verification fails, this Key will be forcibly removed from the library
        return nReadLen;
    }
};
//***************/
/// @brief Search the entire file for the value corresponding to the key
/// @param stKey
/// @param pfun
/// @return
bool PKvSystem::_ReadValueByKey(ara::core::StringView const &stKey, PPageOptBase::CB_ReadData const &pfun) noexcept
{
    // 2023-01-18 hanjingjing: Possible encryption/decryption is not handled here
    if (false == managerKvStore_.ReadValueByKey(stKey, pfun)) {
        if (EErrorPHKV::kKvElementNotFind == managerKvStore_.GetLastError()) {  // NOLINT [bugprone-branch-clone]
            _OnOptFailed(managerKvStore_.GetLastError());
        } else {
            PER_OnOptFailed(managerKvStore_.GetLastError());
        }
        return false;
    }
    _OnSuccess();
    return true;
}
/// @brief Search the entire file for the value corresponding to the key
/// @param stKey
/// @param eDataType
/// @param pReadBuff
/// @param nReadBuffLen
/// @return Returns the length of the read Value itself, -1 indicates failure
int32_t PKvSystem::_ReadValueByKey(ara::core::StringView const &stKey,
                                   EDataType const eDataType,
                                   uint8_t *const pReadBuff,
                                   uint32_t const nReadBuffLen) noexcept
{
    PConfigData_Crypto const *const pFindCrypto{kvConfigMuster_.IsHaveCrypto_Element(T_String(stKey))};
    if (nullptr == pFindCrypto) {
        return _DoReadValueByKey(stKey, eDataType, pReadBuff, nReadBuffLen);
    }
    // Encryption required
    std::unique_ptr< IPerCrypto > pPerCrypto{MakePerCrypto(pFindCrypto)};
    if (false == pPerCrypto.operator bool()) {
        PER_OnOptFailed(EErrorPHKV::kKvElementCrypot);
        return -1;
    }
    int32_t nReadDatLen{0};
    if (ECryptoKeySlotUsage::kEncryption == pFindCrypto->eKeySlotUsage) {
        nReadDatLen = _DealCryptoElement_Decode(pPerCrypto.get(), stKey, eDataType, pReadBuff, nReadBuffLen);
    } else if (ECryptoKeySlotUsage::kVerification == pFindCrypto->eKeySlotUsage) {
        nReadDatLen
            = _DealCryptoElement_Verify(pPerCrypto.get(), pFindCrypto, stKey, eDataType, pReadBuff, nReadBuffLen);
    }
    if (nReadDatLen < 0) {
        PER_OnOptFailed(EErrorPHKV::kKvElementCrypot);
        return -1;
    }
    _OnSuccess();
    return static_cast< int32_t >(nReadDatLen);
}
/// @brief Search the entire file for the value corresponding to the key
/// @param stKey
/// @param eDataType
/// @param pReadBuff
/// @param nReadBuffLen
/// @return Returns the length of the read Value itself, -1 indicates failure
int32_t PKvSystem::_DoReadValueByKey(ara::core::StringView const &stKey,
                                     EDataType const eDataType,
                                     uint8_t *const pReadBuff,
                                     uint32_t const nReadBuffLen) noexcept
{
    int32_t nReadTotal{0};
    int32_t nReadDataType{0};
    bool bReturn{false};
    bReturn = managerKvStore_.ReadValueByKey(
        stKey,
        [&nReadTotal, &nReadDataType, &pReadBuff, nReadBuffLen](int32_t const nDataType, uint8_t const *const pReadData,
                                                                int32_t const nReadLen, uint32_t,
                                                                uint16_t) noexcept -> int32_t {
            nReadDataType = nDataType;
            int32_t const nCopyLen{ara::per::isoftkv::PManagerKvStore::CopyValueData(
                nReadTotal, pReadBuff, static_cast< int32_t >(nReadBuffLen), pReadData, nReadLen)};
            nReadTotal += nCopyLen;
            return nCopyLen;
        });
    if (false == bReturn) {
        if (EErrorPHKV::kKvElementNotFind != managerKvStore_.GetLastError()) {  // NOLINT [bugprone-branch-clone]
            PER_OnOptFailed(managerKvStore_.GetLastError());
        } else {
            _OnOptFailed(managerKvStore_.GetLastError());
        }
        return -1;
    }
    if (nReadDataType != static_cast< int32_t >(eDataType)) {
        PER_OnOptFailed(EErrorPHKV::kKvDataType);
        return -1;
    }
    return nReadTotal;
}
/// @brief Write a new KV-Element
/// @param stKey
/// @param eValueType
/// @param pBValue
/// @param nValueLen
/// @return
bool PKvSystem::_WriteValue(ara::core::StringView const &stKey,
                            EDataType const eValueType,
                            uint8_t const *const pBValue,
                            uint32_t const nValueLen) noexcept
{
    PConfigData_Crypto const *pFindCrypto{nullptr};
    pFindCrypto = kvConfigMuster_.IsHaveCrypto_ElementEncryption(T_String(stKey));
    if (nullptr == pFindCrypto) {
        if (false == managerKvStore_.WriteValue(stKey, eValueType, pBValue, nValueLen)) {
            PER_OnOptFailed(managerKvStore_.GetLastError());
            return false;
        }
    } else {
        int32_t nCryptoLen{_DealCryptoElement_Encode(stKey, eValueType, pBValue, nValueLen)};
        if (nCryptoLen < 0) {
            PER_OnOptFailed(EErrorPHKV::kKvElementCrypot);
            return false;
        }
    }
    _OnSuccess();
    return true;
}
/// @brief Load update file
/// @param stFileName
/// @param perVersion
/// @param appVersion
/// @param nPageLen
/// @return
bool PKvSystem::_LoadUpdateFile(ara::core::StringView const &stFileName,
                                PPerVersion const &systemVersion,
                                PPerVersion const &perVersion,
                                uint32_t const nPageLen) noexcept
{
    if (false == managerPage_.UpdateVersion(kvConfigMuster_.GetVersionApp(), kvConfigMuster_.GetVersionPer())) {
        return false;
    }
    if (false == PFileOpt::IsFileExist(stFileName)) {
        LogWarn() << "KvName =" << GetLogKvName().data() << "[PH_System].LoadUpdate: Not Find File ["
                  << stFileName.data() << "].";
        return true;  // 2023-08-10: The version number after this update is the one within the manifest file
    }
    // The first line is version information, followed by Key="Type/Length/Data", Value = "Type/Length/Data"
    PKvUpdateFile updateFile{T_StringView(GetSystemName()), stFileName, true};
    if (false == updateFile.IsAccessReady()) {
        if (false == PFileOpt::IsFileExist(stFileName)) {
            LogWarn() << "KvName =" << GetLogKvName().data() << "[PH_System].LoadUpdate: Not Find UpdateFile ["
                      << stFileName.data() << "].";
            return true;
        }
        PER_OnOptFailed(EErrorPHKV::kFileUpdateOpen);
        return false;
    }
    // First verify the Version
    ara::core::String const stVersionSystemInFile{GetKvSystemVersion()};
    PPerVersion versionSystemInFile{Version_StringToPer(stVersionSystemInFile)};
    if (IsVersionZero(versionSystemInFile)) {
        PER_OnOptFailed(EErrorPHKV::kPageGroupVersion);
        return false;
    }
    // If the local Per version is higher than the Per version in the KVS update file, the update is not allowed
    if (CompareVersion(systemVersion, versionSystemInFile) > 0) {
        PER_OnOptFailed(EErrorPHKV::kFileVersion);
        LogError() << "KvName =" << GetLogKvName().data()
                   << "[PH_System].LoadUpdate: Update.PerVersion Is High, Forbid Update. "
                   << "VersionSystem In File =" << stVersionSystemInFile.c_str();
        return false;
    }
    ara::core::String const stVersionPerInFile{kvConfigMuster_.GetVersionPer()};
    PPerVersion versionPerInFile{Version_StringToPer(stVersionPerInFile)};
    if (IsVersionZero(versionPerInFile)) {
        PER_OnOptFailed(EErrorPHKV::kPageGroupVersion);
        return false;
    }
    // If the local App version is higher than the App version in the update file, no update is needed
    if (CompareVersion(perVersion, versionPerInFile) > 0) {
        LogInfo() << "KvName =" << GetLogKvName().data()
                  << "[PH_System].LoadUpdate: Update.AppVersion Is Low, Not Need Updata. "
                  << "VersionPer In File =" << stVersionPerInFile.c_str();
        return true;
    }
    LogInfo() << "KvName =" << GetLogKvName().data() << "[PH_System].LoadUpdate: Start Update. "
              << "VersionSystem In File =" << stVersionSystemInFile.c_str()
              << "VersionPer In File =" << stVersionPerInFile.c_str();
    // Delete unused KV pairs SWS_PER_00391
    ara::core::Map< ara::core::String, int32_t > mapKvInUse{};  // Record the activity of each Key
    isoftkv::EUpdateStrategy_Storage eUpdateStorage{isoftkv::TransUpdate_Storage(kvConfigMuster_.GetUpdateStrategy())};
    if (isoftkv::EUpdateStrategy_Storage::kDelete == eUpdateStorage) {
        ara::core::Vector< ara::core::String > vecKeyList;
        if (EnumAllKey(vecKeyList)) {
            for (auto &stKey : vecKeyList) {
                mapKvInUse[stKey] = 0;
            }
        }
    }
    // The version in the file is higher
    PPageOptValue const optValuePrev{nullptr};
    uint32_t const nPageMaxValue{static_cast< uint32_t >(managerCache_.GetPageLen())
                                 - static_cast< uint32_t >(optValuePrev.GetMinValidLen())};
    POptUpdateKvElement optUpdateKv{this, nPageMaxValue};
    int32_t nLoadKvTotal{0};
    nLoadKvTotal = updateFile.ScanAllLine(
        true, nPageLen,
        [&optUpdateKv, &mapKvInUse, eUpdateStorage](
            int32_t const nLineID, PKvUpdateRead_Base const *const pOptKvUpdateRead) noexcept -> int32_t {
            int32_t nReturn = optUpdateKv.DealReadData(nLineID, pOptKvUpdateRead);
            if (EUpdateKeyWord::kCrc == pOptKvUpdateRead->GetUpdateWord()) {
                // When the Storage upgrade strategy is DELETE, all KV pairs not present in the *_init.per file will be deleted
                if (isoftkv::EUpdateStrategy_Storage::kDelete == eUpdateStorage) {
                    mapKvInUse[optUpdateKv.GetKey()] += 1;
                }
            }
            return nReturn;
        });
    // Save version number
    _LogInfo("[PH_System].LoadUpdate", ":", "LoadTotal", nLoadKvTotal, "KeyCount", optUpdateKv.GetKeyCount());

    // Delete unused KV pairs SWS_PER_00391: KV pairs marked for deletion have already been handled in POptUpdateKvElement
    if (isoftkv::EUpdateStrategy_Storage::kDelete == eUpdateStorage) {
        for (auto &it : mapKvInUse) {
            if (it.second <= 0) {
                RemoveKey(T_StringView(it.first));
            }
        }
    }
    // Need to compare the version numbers in kvConfigMuster_ and the UpdateFile
    ara::core::String const stVersionInPer{updateFile.FindVersion(G_GetPerVersionName(), managerCache_.GetPageLen())};
    ara::core::String const stVersionInApp{updateFile.FindVersion(G_GetAppVersionName(), managerCache_.GetPageLen())};
    if (false == managerPage_.UpdateVersion(stVersionInApp, stVersionInPer)) {
        PER_OnOptFailed(EErrorPHKV::kPageGroupVersion);
        return false;
    }
    _OnSuccess();
    return true;
}
/// @brief Save update file
/// @param stFileName
/// @param perVersion
/// @param appVersion
/// @return
bool PKvSystem::_SaveUpdateFile(ara::core::StringView const &stFileName,
                                ara::core::String const &stVersionPer,
                                ara::core::String const &stVersionApp) noexcept
{
    // The first line is version information, followed by Key="Type/Length/Data", Value = "Type/Length/Data"
    PKvUpdateFile updateFile{T_StringView(GetSystemName()), stFileName, false};
    if (false == updateFile.IsAccessReady()) {
        PER_OnOptFailed(EErrorPHKV::kFileUpdateOpen);
        return false;
    }
    std::ignore = updateFile.ClearFile();
    std::ignore = updateFile.SaveElement(
        0, EUpdateAction::kIgnore, T_String(G_GetPerVersionName()), static_cast< int32_t >(EDataType::kString),
        static_cast< const void * >(stVersionPer.data()), static_cast< uint32_t >(stVersionPer.size()));
    std::ignore = updateFile.SaveElement(
        1, EUpdateAction::kIgnore, T_String(G_GetAppVersionName()), static_cast< int32_t >(EDataType::kString),
        static_cast< const void * >(stVersionApp.data()), static_cast< uint32_t >(stVersionApp.size()));
    // Prepare file reading and writing
    int32_t nReadTotal{0};
    nReadTotal = managerKvStore_.ScanAllKvPair(
        [this, &updateFile](PCachePagePtr const &pKvPage, int32_t const nKvIndex) noexcept -> int32_t {
            if (false == _SaveUpdateKv(updateFile, pKvPage, nKvIndex)) {
                return 0;
            }
            return 1;
        });
    _LogInfo("[PH_System].SaveUpdate", ": ", "SaveTotal", nReadTotal + 1);
    _LogInfo("[PH_System].SaveUpdate.AppVersion", stVersionApp.c_str());
    _LogInfo("[PH_System].SaveUpdate.PerVersion", stVersionPer.c_str());
    _OnSuccess();
    return true;
}
/// @brief Save update KV
/// @param updateFile
/// @param pKvPage
/// @param nKvIndex
/// @return
bool PKvSystem::_SaveUpdateKv(PKvUpdateFile &updateFile, PCachePagePtr const &pKvPage, int32_t const nKvIndex) noexcept
{
    uint32_t nCrc{0U};
    int32_t const nLineIndex{nKvIndex + 2};  // +2 because of AP-Per version number and APP version number
    // Save Action and Key
    bool bReturn{false};
    bReturn = managerKvStore_.GetKvKeyData(
        pKvPage, nKvIndex,
        [nLineIndex, &updateFile, &nCrc](int32_t const nDataType, uint8_t const *const pReadData,
                                         int32_t const nReadLen, uint32_t, uint16_t) -> int32_t {
            std::ignore = nDataType;
            if ((nullptr == pReadData) || (nReadLen <= 0)) {
                return 0;
            }
            ara::core::String const stKeyData{T_String(pReadData, static_cast< uint32_t >(nReadLen))};
            std::ignore = updateFile.SaveKey(nLineIndex, EUpdateAction::kOverWrite, stKeyData);
            nCrc        = CalculateCrc32(pReadData, static_cast< uint32_t >(nReadLen), nCrc);
            return nReadLen;
        });
    PER_Assert(bReturn);
    int32_t nValueDataType{0};
    int32_t const nValueLen{managerKvStore_.GetKvValueLen(pKvPage, nKvIndex, &nValueDataType)};
    if (nValueLen <= 0) {
        return false;
    }
    // Save Value
    if (nValueLen > 0) {
        PWordEncode wordEncode{nCrc};
        PKvUpdateSave_Value optSaveUpdate{updateFile.GetFileOpt(), nLineIndex, &wordEncode};
        std::ignore
            = optSaveUpdate.SaveBegin(EUpdateKeyWord::kValue, nValueDataType, static_cast< uint32_t >(nValueLen));
        bReturn = managerKvStore_.GetKvValueData(
            pKvPage, nKvIndex,
            [&optSaveUpdate, &nCrc](int32_t const nDataType, uint8_t const *const pReadData, int32_t const nReadLen,
                                    uint32_t, uint16_t) -> int32_t {
                std::ignore = nDataType;
                nCrc        = CalculateCrc32(pReadData, static_cast< uint32_t >(nReadLen), nCrc);
                std::ignore = optSaveUpdate.SaveData(pReadData, static_cast< uint32_t >(nReadLen));
                return nReadLen;
            });
        std::ignore = optSaveUpdate.SaveEnd(false);
    }
    // Save CRC
    if (nCrc != 0U) {
        ara::core::String const stCrc{std::to_string(nCrc)};
        PKvUpdateSave_String optSaveCrc{updateFile.GetFileOpt(), 0};
        optSaveCrc.SaveString(EUpdateKeyWord::kCrc, stCrc, true);
    }
    PER_Assert(bReturn);
    return bReturn;
}
/// @brief Handle Crypto requirements for storage files
/// @return
bool PKvSystem::_DealStorage_CryptoVirifyHash() noexcept
{
    if (false == kvConfigMuster_.IsHaveCrypto_Storage(ECryptoKeySlotUsage::kVerification)) {
        return true;
    }
    // Encryption handling: calculate the file hash of the storage area
    PConfigData_Crypto const configCrypto{kvConfigMuster_.GetCryptoConfig()};
    std::unique_ptr< IPerCrypto > pPerCryptoSymmetric{NewPerCrypto()};
    PER_Assert(pPerCryptoSymmetric->LoadCrypto(configCrypto.stKeySlotName, configCrypto.stCryptoAlgorithm));
    // Election algorithm callback function
    std::function< ara::core::Vector< uint8_t >(int32_t const nIndex, uint32_t &nHashData) > pfunWork;
    pfunWork = [this, &pPerCryptoSymmetric, configCrypto](int32_t const nIndex,
                                                          uint32_t &nHashData) -> ara::core::Vector< uint8_t > {
        ara::core::String const stFileName{managerCache_.GetFileName(ECacheSource::kMain, nIndex)};
        ara::core::Vector< uint8_t > vecOut;
        vecOut    = pPerCryptoSymmetric->CryptoFileHash(T_StringView(stFileName), configCrypto.stCryptoAlgorithm);
        nHashData = 0;
        if (false == vecOut.empty()) {
            nHashData = CalculateCrc32(static_cast< void * >(vecOut.data()), static_cast< uint32_t >(vecOut.size()), 0);
        }
        return vecOut;
    };

    ara::core::Vector< uint8_t > vecFileHash;
    if (false
        == SelectMofN< ara::core::Vector< uint8_t > >(vecFileHash, kvConfigMuster_.GetReddCountM(),
                                                      kvConfigMuster_.GetReddCountN(), pfunWork)) {
        return false;
    }
    // Compare file hash
    return CompareHashData(vecFileHash, configCrypto.stVerfHash);
}
/// @brief Handle Crypto requirements for KV data pairs
/// @param stKey
/// @param pBData
/// @param nDataLen
/// @param bEncodeOrDecode
/// @return Returns the processed data byte length, -1 indicates error
int32_t PKvSystem::_DealCryptoElement_Verify(ara::per::isoftkv::IPerCrypto *pPerCrypto,
                                             PConfigData_Crypto const *const pFindCrypto,
                                             ara::core::StringView const &stKey,
                                             EDataType const eDataType,
                                             uint8_t *const pBData,
                                             uint32_t const nDataLen) noexcept
{
    if (nullptr == pFindCrypto) {
        return static_cast< int32_t >(nDataLen);
    }
    if (nullptr == pPerCrypto) {
        return static_cast< int32_t >(nDataLen);
    }
    if (ECryptoKeySlotUsage::kVerification != pFindCrypto->eKeySlotUsage) {
        return -1;
    }
    if (_DoReadValueByKey(stKey, eDataType, pBData, nDataLen) < 0) {
        return -1;
    }
    // Verify only for read-only
    if (false == pFindCrypto->stVerfHash.empty()) {
        ara::core::Vector< uint8_t > vecHash;
        pPerCrypto->CryptoDataHash(pBData, nDataLen, pFindCrypto->stCryptoAlgorithm);
        if (isoftkv::CompareHashData(vecHash, pFindCrypto->stVerfHash)) {
            return static_cast< int32_t >(nDataLen);
        }
    }
    return -1;
}
/// @brief Handle Crypto requirements for KV data pairs
/// @param pPerCrypto
/// @param stKey
/// @param eDataType
/// @param pBData
/// @param nDataLen
/// @return Returns the processed data byte length, -1 indicates error
int32_t PKvSystem::_DealCryptoElement_Decode(ara::per::isoftkv::IPerCrypto *pPerCrypto,
                                             ara::core::StringView const &stKey,
                                             EDataType const eDataType,
                                             uint8_t *const pBData,
                                             uint32_t const nDataLen) noexcept
{
    if (nullptr == pPerCrypto) {
        return -1;
    }
    uint32_t const nCryptoStepLen{pPerCrypto->GetKeySlotObjectSize()};
    uint32_t nCryptoLen = G_CalPaddingLen(nDataLen, nCryptoStepLen);
    PAutoBuff const perBuff{nCryptoLen};
    uint8_t *const pBuffData{perBuff.data()};
    int32_t nReadLen = _DoReadValueByKey(stKey, eDataType, pBuffData, nCryptoLen);
    if (nReadLen < 0) {
        PER_OnOptFailed(EErrorPHKV::kKvElementCrypot);
        return -1;
    }
    int32_t nDecodeLen = pPerCrypto->CryptoData(pBuffData, nReadLen, false);
    if (nDecodeLen < 0) {
        PER_OnOptFailed(EErrorPHKV::kKvElementCrypot);
        return -1;
    }
    // Remove padding
    nDecodeLen -= pBuffData[nDecodeLen - 1];
    uint32_t nCopyLen{std::min< uint32_t >(nDataLen, static_cast< uint32_t >(nDecodeLen))};
    T_Memcpy(pBData, pBuffData, nCopyLen);
    _OnSuccess();
    return static_cast< int32_t >(nCopyLen);
}
/// @brief Handle Crypto requirements for KV data pairs
/// @param stKey
/// @param pBData
/// @param nDataLen
/// @param nBuffLen
/// @param bEncodeOrDecode
/// @return Returns the processed data byte length, -1 indicates error
int32_t PKvSystem::_DealCryptoElement_Encode(ara::core::StringView const &stKey,
                                             EDataType const eDataType,
                                             uint8_t const *const pBData,
                                             uint32_t const nDataLen) noexcept
{
    PConfigData_Crypto const *const pFindCrypto{kvConfigMuster_.IsHaveCrypto_Element(T_String(stKey))};
    if (nullptr == pFindCrypto) {
        return static_cast< int32_t >(nDataLen);
    }
    // The verification option does not require encryption; its content is...
    if (ECryptoKeySlotUsage::kEncryption != pFindCrypto->eKeySlotUsage) {
        return static_cast< int32_t >(nDataLen);
    }
    std::unique_ptr< IPerCrypto > pPerCrypto{MakePerCrypto(pFindCrypto)};
    if (false == pPerCrypto.operator bool()) {
        return -1;
    }
    uint32_t const nCryptoStepLen{pPerCrypto->GetKeySlotObjectSize()};
    uint32_t nCryptoLen = G_CalPaddingLen(nDataLen, nCryptoStepLen);
    PAutoBuff perBuff{nCryptoLen, nDataLen};
    std::ignore = G_PaddingInput(perBuff, pBData, nDataLen, nCryptoStepLen);
    uint8_t *const pBuffData{perBuff.data()};
    uint32_t const nBuffLen{perBuff.GetBuffLen()};
    PER_Assert(nCryptoLen <= nBuffLen);
    if (pPerCrypto->CryptoData(pBuffData, nCryptoLen, true) < 0) {
        return -1;
    }
    if (false == managerKvStore_.WriteValue(stKey, eDataType, pBuffData, nCryptoLen)) {
        PER_OnOptFailed(managerKvStore_.GetLastError());
        return -1;
    }
    return static_cast< int32_t >(nDataLen);
}
//********************************/
}  // namespace isoftkv
}  // namespace per
}  // namespace ara
