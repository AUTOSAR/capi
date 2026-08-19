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
/// @file       kv_system.h
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

#ifndef ARA_PER_PHKV_KV_SYSTEM_BASE_H_
#define ARA_PER_PHKV_KV_SYSTEM_BASE_H_

#include <ara/core/map.h>
#include <ara/core/vector.h>

#include <memory>

#include "ara/per/internal/common/isoft_thread_lock.h"
#include "ara/per/internal/crypto/i_per_crypto.h"
#include "ara/per/internal/crypto/i_redd_calculate.h"
#include "ara/per/internal/isoftkv/config_muster.h"
#include "ara/per/internal/isoftkv/kv_object.h"
#include "ara/per/internal/isoftkv/kv_update_file.h"
#include "ara/per/internal/isoftkv/manager_cache.h"
#include "ara/per/internal/isoftkv/manager_kv_store.h"
#include "ara/per/internal/isoftkv/manager_page.h"
#include "ara/per/internal/isoftkv/page_common_struct.h"
#include "ara/per/internal/isoftkv/page_opt_base.h"

namespace ara {
namespace per {
namespace isoftkv {
//********************************/
/// @brief
/// @code{.isoft}
/// @unit_name=EKvBakType
/// @endcode
enum class EKvBakType : uint8_t
{
    kKvBakToBakFile = 0,  // Under the main library directory, rename with *.bak suffix
    kKvBakToDirent  = 1,  // Under the main library directory's *.bak subdirectory, no renaming
};
/// @brief Core of KV storage engine
/// @code{.isoft}
/// @unit_name=PKvSystem
/// @endcode
class PKvSystem : public PKvObject
{
public:
    /// @brief Check if KvSystem exists
    /// @param kvConfig
    /// @return
    static bool IsKvSystemExist(PConfigMuster_Kv const &kvConfig) noexcept;

private:
    /// @brief Configuration file wrapper class
    PConfigMuster_Kv kvConfigMuster_;
    /// @brief Cache management
    PManagerCache managerCache_;
    /// @brief Page management
    PManagerPage managerPage_;
    /// @brief KV storage management
    PManagerKvStore managerKvStore_;

private:
    /// @brief Mutex
    mutable PThreadLockRW threadLockAction_;
    /// @brief Backup strategy
    EKvBakType eKvBakeupType_;

public:
    /// @brief
    PKvSystem() = delete;
    /// @brief
    /// @param a
    PKvSystem(PKvSystem const &a) = delete;
    /// @brief
    /// @param a
    /// @return
    PKvSystem &operator=(PKvSystem const &a) = delete;
    /// @brief
    /// @param a
    PKvSystem(PKvSystem &&a) = delete;
    /// @brief
    /// @param a
    /// @return
    PKvSystem &operator=(PKvSystem &&a) = delete;
    /// @brief Construct KvSystem using a configuration object
    /// @param kvConfigMuster
    explicit PKvSystem(PConfigMuster_Kv const &kvConfigMuster) noexcept;
    /// @brief
    ~PKvSystem() noexcept override;
    /// @brief Set the operation interface for calculating redundant CRC/Hash
    /// @param pfun
    bool AttachReddAlgorithm(std::unique_ptr< IReddAlgorithm > pfun) noexcept
    {
        return managerKvStore_.AttachReddAlgorithm(std::move(pfun));
    }
    /// @brief Get redundancy check
    /// @return
    IReddAlgorithm *GetReddAlgorithm() const noexcept { return managerKvStore_.GetReddAlgorithm(); }

public:
    /// @brief Determine if the Kv library is ready for read/write
    /// @return
    bool IsAccessReady() const noexcept override;
    /// @brief Get the library name for distinguishing Kv libraries when writing logs
    /// @return
    ara::core::StringView GetLogKvName() const noexcept override;
    /// @brief Get the Kv library name
    /// @return
    ara::core::String const &GetSystemName() const noexcept;
    /// @brief Get the number of redundancies from the configuration
    uint32_t GetReddCountN() const noexcept;
    /// @brief Get the working directory of the Kv library
    /// @return
    ara::core::StringView GetWorkPath() const noexcept;
    /// @brief Get the file name of the main file
    /// @param stWorkPath
    /// @return
    ara::core::String GetMainFileName(ara::core::StringView const &stWorkPath) const noexcept;
    /// @brief Get the file name of a redundant file
    /// @param nReddIndex Redundancy index
    /// @return
    ara::core::String GetReddFileName(int32_t const nReddIndex) const noexcept;

    /// @brief Check if the Kv library file exists
    /// @return
    bool IsExist() noexcept;
    /// @brief Check if the Kv library is empty
    /// @return
    bool IsNullStore() noexcept;
    /// @brief Check if the system format is valid
    /// @return
    bool CheckSystem() noexcept;
    /// @brief Create a new database. nPageLevel indicates the page magnitude; different magnitudes have different initial page layouts.
    /// @param nPageLevel
    /// @return
    bool NewSystem(uint32_t const nPageLevel = 1U) noexcept;
    /// @brief Open an existing library
    /// @return
    bool OpenSystem() noexcept;
    /// @brief Close the system
    /// @return
    bool CloseSystem() noexcept;
    /// @brief Write the content of the Walog log library back to the Main library
    /// @param bOpenSystem Whether to keep the system open
    /// @return
    bool SyncWalogToMain(bool const bOpenSystem = true) noexcept;
    /// @brief Discard modifications
    /// @return
    bool DiscardChanges() noexcept;
    /// @brief Clear the system
    /// @return
    bool RemoveSystem() noexcept;
    /// @brief Check if modifications are required
    /// @return
    bool IsHaveWritePage() const noexcept;

public:
    /// @brief Get the total disk space occupied by the entire KV library: includes Main, Walog, Redd, etc.
    /// @return
    uint64_t GetSpaceSize() noexcept;
    /// @brief Get the total number of pages in the DB database file
    /// @return
    uint32_t GetPageTotal() const noexcept;
    /// @brief Load a page of data in read-only mode
    /// @param nMainPageID
    /// @return Obtained memory page cache object
    PCachePagePtr LoadPageData(uint32_t const nMainPageID) const noexcept;
    /// @brief Get the version number of the PER format: App within the DB library
    /// @return
    PPerVersion GetPerVersion_StorageApp() const noexcept;
    /// @brief Get the version number of the PER format: Per within the DB library
    /// @return
    PPerVersion const &GetPerVersion_StoragePer() const noexcept;
    /// @brief Get the version number: App within the DB library
    /// @return
    ara::core::String GetVersion_StorageApp() const noexcept;
    /// @brief Get the version number: Per within the DB library
    /// @return
    ara::core::String GetVersion_StoragePer() const noexcept;
    /// @brief Get the version number: System within the DB library
    /// @return
    ara::core::String GetVersion_StorageSys() const noexcept;
    /// @brief Update the APP/PER version numbers within the system
    /// @return Success or failure
    bool UpdateVersion() noexcept;
    /// @brief Update the PER version number within the system: only modifies the numeric part, the string part remains unchanged
    /// @return Success or failure
    bool UpdateVersionPer(PPerVersion const &perVersion) noexcept;
    /// @brief Get the version number: Per from the configuration
    /// @return
    ara::core::String GetVersion_ConfigPer() const noexcept;
    /// @brief Get the version number: App
    /// @return
    ara::core::String GetVersion_ConfigApp() const noexcept;
    // Backup related
public:
    /// @brief Get the backup name
    /// @return
    ara::core::String GetBakKvName() const noexcept;
    /// @brief Get the backup directory
    /// @return
    ara::core::String GetBakPath() const noexcept;
    /// @brief Get the backup file name
    /// @return
    ara::core::String GetBakFileName(ECacheSource const eCacheSource) const noexcept;
    /// @brief Set the backup strategy
    /// @param eBakType
    void SetBakeupType(EKvBakType const eBakType) noexcept;
    /// @brief Get the backup configuration table; eBakType indicates the backup strategy
    /// @return
    PConfigMuster_Kv GetBakConfigMuster() const noexcept;
    /// @brief Check if the backup format is valid
    /// @return
    bool CheckBakSystem() noexcept;
    /// @brief Delete the backup library
    /// @return
    bool RemoveBackupFile() const noexcept;
    /// @brief Backup
    /// @param bDiscardChanges
    /// @return
    bool BackupSystem(bool const bDiscardChanges) noexcept;
    /// @brief Restore the system: restore this library from the backup
    /// @return
    bool RecoverFromBackup() noexcept;
    /// @brief Restore the system: use M/N recovery scheme
    /// @return
    bool RecoverSystem_FromRedd(ara::core::Vector< uint8_t > &vecRecoverRedd) noexcept;
    /// @brief Load update file
    /// @param stFileName
    /// @param bOpenSystem
    /// @return
    bool LoadUpdateFile(ara::core::StringView const &stFileName, bool const bOpenSystem) noexcept;
    /// @brief Load update file: the file source is the file saved in PConfigMuster_Kv
    /// @param bOpenSystem
    /// @return
    bool LoadUpdateFile(bool const bOpenSystem) noexcept;
    /// @brief Save update file
    /// @param stFileName
    /// @return
    bool SaveUpdateFile(ara::core::StringView const &stFileName) noexcept;
    /// @brief Delete a redundancy recovery request
    /// @param nPageID
    void DelReddCheck(uint32_t const nPageID) noexcept;
    /// @brief Check if there is a redundancy recovery request
    /// @return Redundancy data that needs recovery
    PReddCheckData_Kv FindRecoverReddPageID() const noexcept;
    /// @brief Recover specific page data
    /// @param nPageID Page number
    /// @return Whether successful
    ara::core::Vector< uint8_t > RecoverPage(uint32_t const nPageID) noexcept;
    /// @param nPageID Page number to recover
    /// @param nSelectIndex Redundancy index of the correct data
    /// @param vecRecoverIndex Redundant page numbers of the erroneous data
    /// @return Success or failure
    bool RecoverPage(uint32_t const nPageID,
                     uint8_t const nSelectIndex,
                     ara::core::Vector< uint8_t > const &vecRecoverIndex) noexcept;

public:
    /// @brief Search the entire file for the value corresponding to the key: supports basic types such as int, uint32_t, int64_t, uint64_t, float, double
    /// @tparam T
    /// @param stKey
    /// @return
    /// @throws
    template < typename T >
    T ReadValue(ara::core::StringView const &stKey) noexcept;
    /// @brief Search the entire file for the value corresponding to the key
    /// @param stKey
    /// @return
    ara::core::String ReadValueString(ara::core::StringView const &stKey) noexcept;
    /// @brief Search the entire file for the value corresponding to the key: returns the actual size read
    /// @param stKey
    /// @param pBuff
    /// @param nBufLen
    /// @return
    int32_t ReadValueBinary(ara::core::StringView const &stKey, uint8_t *const pBuff, uint32_t const nBufLen) noexcept;
    /// @brief Search the entire file for the value corresponding to the key: returns the actual size read
    /// @tparam nLen
    /// @param stKey
    /// @param arrayData
    /// @return
    template < uint32_t nLen >
    int32_t ReadValueBinary(ara::core::StringView const &stKey, uint8_t arrayData[nLen]) noexcept
    {
        return ReadValueBinary(stKey, arrayData, nLen);
    }
    /// @brief Write a value of type T: supports basic types such as int, uint32_t, int64_t, uint64_t, float, double
    /// @tparam T
    /// @param stKey
    /// @param value
    /// @return
    template < typename T >
    bool WriteValue(ara::core::StringView const &stKey, T const &value) noexcept;
    /// @brief Write a new KV-Element
    /// @param stKey
    /// @param eValueType
    /// @param pBValue
    /// @param nValueLen
    /// @return
    bool WriteValue(ara::core::StringView const &stKey,
                    EDataType const eValueType,
                    uint8_t const *const pBValue,
                    uint32_t const nValueLen) noexcept;
    /// @brief Write a new KV-Element
    /// @param stKey
    /// @param pBValue
    /// @param nValueLen
    /// @return
    bool WriteValue(ara::core::StringView const &stKey,
                    uint8_t const *const pBValue,
                    uint32_t const nValueLen) noexcept;
    /// @brief Write a new KV-Element: placeholder for empty data with total length nValueLen
    /// @param stKey
    /// @param eValueType
    /// @param nValueLen
    /// @return
    bool WriteValue(ara::core::StringView const &stKey, EDataType const eValueType, uint32_t const nValueLen) noexcept;

public:
    /// @brief Search the entire file for the redundancy check data corresponding to the key
    /// @param stKey
    /// @return Read redundancy check data
    ara::core::Vector< uint8_t > ReadReddData(ara::core::StringView const &stKey) noexcept;
    /// @brief Get the length of the value corresponding to the key
    /// @param stKey
    /// @return
    int32_t GetValueLen(ara::core::StringView const &stKey) noexcept;
    /// @brief Check if a key exists
    /// @param stKey
    /// @return
    bool IsKeyExist(ara::core::StringView const &stKey) noexcept;
    /// @brief Directly operate on a certain page of Value, replacing the data within it
    /// @param blockValue
    /// @param nOffset
    /// @param pBValue
    /// @param nValueLen
    /// @return
    bool AmendValueData(PBlockValue const &blockValue,
                        int32_t const nOffset,
                        uint8_t const *const pBValue,
                        int32_t const nValueLen) noexcept;
    /// @brief Extended interface for reading/writing cross-page Values, finds the storage locations of all Blocks of the Value via stKey
    /// @param stKey
    /// @param vecKeyList
    /// @return
    bool FindBlockByKey(ara::core::StringView const &stKey, ara::core::Vector< PBlockValue > &vecKeyList) noexcept;
    /// @brief Iterate over all Key values
    /// @param pfun
    /// @return
    bool ScanAllKey(CB_DealStringView const &pfun) noexcept;
    /// @brief Enumerate all Key values
    /// @param vecKeyList
    /// @return
    bool EnumAllKey(ara::core::Vector< ara::core::String > &vecKeyList) noexcept;
    /// @brief Delete a KV-Element
    /// @param stKey
    /// @return
    bool RemoveKey(ara::core::StringView const &stKey) noexcept;
    /// @brief Delete all Keys in this center (operation takes effect immediately)
    /// @return
    bool RemoveAllKeys() noexcept;
    /// @brief Restore a KV-Element
    /// @param stKey
    /// @return
    bool RecoverKey(ara::core::StringView const &stKey, ara::core::Vector< uint8_t > &vecRecoverRedd) noexcept;
    /// @brief Reset a KV-Element: creates if it does not exist; if it exists, sets the Value to its initial value
    /// @param stKey Key
    /// @return
    bool ResetKey(ara::core::StringView const &stKey) noexcept;
    /// @brief Enumerate all PageIDs encountered during reading the Value of a KV-Element
    /// @param stKey Key name
    /// @return List of PageIDs involved in order
    ara::core::Vector< uint32_t > EnumReadValuePageID(ara::core::StringView const &stKey) noexcept;
    /// @brief Get the size of the main library, excluding redundant backups. Returns the size of the xxxxx.main file.
    /// @return Returns the size of the main file
    /// @code{.isoft}
    /// @needwork = dda
    /// @endcode
    int64_t MainFileSize() noexcept;

public:
    /// @brief Print the Key values within a KV page
    /// @return
    int32_t Debug_PrintKvStore() noexcept;
    /// @brief Iterate over all caches
    void Debug_ScanCache() noexcept;

    // Read the first page of the file. Because the page length is unknown initially, the actual Page management page requires a "double read": first read with the default page length, obtain the page length, then compare before deciding whether to read the whole page a second time.
protected:
    /// @brief Open an existing library
    /// @return
    bool _OpenSystem() noexcept;
    /// @brief Reset the Walog library: discard all Walog cache pages and reinitialize the Walog page
    /// @return
    bool _ResetWalog() noexcept;
    /// @brief Save all modified Cache pages to the Walog library
    /// @param nSyncModel
    /// @param bForce
    /// @return
    bool _SaveCacheToWalog(uint32_t const nSyncModel, bool const bForce) noexcept;
    /// @brief Write the content of the Walog log library back to the Main library
    /// @param pPageWork
    /// @return
    bool _SaveWalogPage(PCachePagePtr const &pPageWork) noexcept;
    /// @brief Attempt to write the content of the Walog log library back to the Main library
    /// @param bSaveCache
    /// @return
    bool _SyncWalogToMain(bool const bSaveCache) noexcept;
    /// @brief Read the PageLen of a known library; the return value can determine whether a database is a valid legal library
    /// @return
    uint16_t _ReadPageLen() noexcept;

protected:
    /// @brief Search the entire file for the value corresponding to the key
    /// @param stKey
    /// @param pfun
    /// @return
    bool _ReadValueByKey(ara::core::StringView const &stKey, PPageOptBase::CB_ReadData const &pfun) noexcept;
    /// @brief Search the entire file for the value corresponding to the key
    /// @param stKey
    /// @param eDataType
    /// @param pReadBuff
    /// @param nBuffLen
    /// @return Returns the length of the read Value itself, -1 indicates failure
    int32_t _ReadValueByKey(ara::core::StringView const &stKey,
                            EDataType const eDataType,
                            uint8_t *const pReadBuff,
                            uint32_t const nBuffLen) noexcept;
    /// @brief Search the entire file for the value corresponding to the key
    /// @param stKey
    /// @param eDataType
    /// @param pReadBuff
    /// @param nBuffLen
    /// @return Returns the length of the read Value itself, -1 indicates failure
    int32_t _DoReadValueByKey(ara::core::StringView const &stKey,
                              EDataType const eDataType,
                              uint8_t *const pReadBuff,
                              uint32_t const nBuffLen) noexcept;
    /// @brief Write a new KV-Element
    /// @param stKey
    /// @param eValueType
    /// @param pBValue
    /// @param nValueLen
    /// @return
    bool _WriteValue(ara::core::StringView const &stKey,
                     EDataType const eValueType,
                     uint8_t const *const pBValue,
                     uint32_t const nValueLen) noexcept;

protected:
    /// @brief Load update file
    /// @param stFileName
    /// @param systemVersion Version number of the KvSystem logic
    /// @param perVersion Version number of the persistent data
    /// @param nPageLen
    /// @return
    bool _LoadUpdateFile(ara::core::StringView const &stFileName,
                         PPerVersion const &systemVersion,
                         PPerVersion const &perVersion,
                         uint32_t const nPageLen) noexcept;
    /// @brief Save update file
    /// @param stFileName
    /// @param perVersion
    /// @param appVersion
    /// @return
    bool _SaveUpdateFile(ara::core::StringView const &stFileName,
                         ara::core::String const &stVersionPer,
                         ara::core::String const &stVersionApp) noexcept;
    /// @brief Save update KV
    /// @param updateFile
    /// @param pKvPage
    /// @param nKvIndex
    /// @return
    bool _SaveUpdateKv(PKvUpdateFile &updateFile, PCachePagePtr const &pKvPage, int32_t const nKvIndex) noexcept;

protected:
    /// @brief Handle Crypto requirements for storage files
    /// @return
    bool _DealStorage_CryptoVirifyHash() noexcept;
    /// @brief Handle Crypto requirements for KV data pairs
    /// @param stKey
    /// @param pBData
    /// @param nDataLen
    /// @param bEncodeOrDecode
    /// @return Returns the processed data byte length, -1 indicates error
    int32_t _DealCryptoElement_Verify(ara::per::isoftkv::IPerCrypto *pPerCrypto,
                                      PConfigData_Crypto const *const pFindCrypto,
                                      ara::core::StringView const &stKey,
                                      EDataType const eDataType,
                                      uint8_t *const pBData,
                                      uint32_t const nDataLen) noexcept;
    /// @brief Handle Crypto requirements for KV data pairs
    /// @param stKey
    /// @param pBData
    /// @param nDataLen
    /// @param bEncodeOrDecode
    /// @return Returns the processed data byte length, -1 indicates error
    int32_t _DealCryptoElement_Decode(ara::per::isoftkv::IPerCrypto *pPerCrypto,
                                      ara::core::StringView const &stKey,
                                      EDataType const eDataType,
                                      uint8_t *const pBData,
                                      uint32_t const nDataLen) noexcept;
    /// @brief Handle Crypto requirements for KV data pairs
    /// @param stKey
    /// @param pBData
    /// @param nDataLen
    /// @param bEncodeOrDecode
    /// @return Returns the processed data byte length, -1 indicates error
    int32_t _DealCryptoElement_Encode(ara::core::StringView const &stKey,
                                      EDataType const eDataType,
                                      uint8_t const *const pBData,
                                      uint32_t const nDataLen) noexcept;
};
//********************************/
/// @brief Bypass the protection/private constructor of class T to construct a shared smart pointer of the class
/// @code{.isoft}
/// @unit_name=T_CreateShared
/// @interface_level=unit
/// @endcode
/// @tparam T_Class
/// @tparam ...Args
/// @param args
/// @return
/// @throws
template < typename T_Class, typename... Args >
std::shared_ptr< T_Class > T_CreateShared(Args &&...args)
{
    /// @brief Bypass the protected constructor to create a class object
    struct T_make_shared_enabler : public T_Class
    {
    public:
        /// @brief Constructor
        /// @param args
        explicit T_make_shared_enabler(Args &&...args) noexcept : T_Class{std::forward< Args >(args)...} {}
    };
    return std::move(std::make_shared< T_make_shared_enabler >(std::forward< Args >(args)...));
}
//********************************/
}  // namespace isoftkv
}  // namespace per
}  // namespace ara

#endif
