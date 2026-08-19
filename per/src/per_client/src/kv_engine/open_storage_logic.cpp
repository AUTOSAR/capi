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
/// @file       open_storage_logic.cpp
/// @brief      AutoSar-AP Data Persistence Storage Module
/// @details    Logic when opening Puhua Storage
/// @date       2021-04-13
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
/// @unit_description=Logic when opening Puhua Storage
/// @endcode
///
/// ================================================================
///
/// @par Modification Log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// | 2021-04-13 | 1.0.0   | hanjingjing  | Initial version created                   |
///
/// ================================================================

#include "ara/per/internal/open_storage_logic.h"

#include "ara/per/file_storage.h"
#include "ara/per/internal/common/isoft_file_opt.h"
#include "ara/per/internal/crypto/isoft_per_redd_calculate.h"
#include "ara/per/internal/isoftkv/kv_data_type.h"
#include "ara/per/internal/isoftkv/kv_log_api.h"
#include "ara/per/internal/isoftkv/kv_system.h"
#include "ara/per/internal/isoftkv/manager_page.h"
#include "ara/per/internal/isoftkv/page_common_struct.h"
#include "ara/per/internal/isoftkv/shared_ptr_manager.h"
#include "ara/per/internal/manifest/manifest_instance.h"
#include "ara/per/key_value_storage.h"

namespace {
//********************************/
/// @brief Read Storage information in JSON format configuration
class PReadStorageConfig final
{
private:
    /// @brief
    ara::core::StringView stPortIns_;
    /// @brief
    ara::per::manifest::MConfig_StorageBase const* pFindStorage_;

public:
    /// @brief
    /// @param portIns
    explicit PReadStorageConfig(ara::core::InstanceSpecifier const& portIns) noexcept
        : stPortIns_{std::move(portIns.ToString())}
        , pFindStorage_{ara::per::manifest::PManifestInstance::FindStorage(
              portIns, ara::per::manifest::PManifestInstance::EPerType::kStorage_Any)}
    {
    }
    /// @brief
    ~PReadStorageConfig() = default;
    /// @brief
    /// @param a
    PReadStorageConfig(PReadStorageConfig const& a) = delete;
    /// @brief
    /// @param a
    /// @return
    PReadStorageConfig& operator=(PReadStorageConfig const& a) = delete;
    /// @brief
    /// @param a
    PReadStorageConfig(PReadStorageConfig&& a) = delete;
    /// @brief
    /// @param a
    /// @return
    PReadStorageConfig& operator=(PReadStorageConfig&& a) = delete;
    /// @brief
    /// @return
    inline bool IsConfigValid() const noexcept
    {
        if (nullptr == pFindStorage_) {
            return false;
        }
        return true;
    }
    /// @brief Get the name of KVS from the JSON configuration
    /// @name GetStorageName
    /// @return
    inline ara::core::String GetStorageName() const noexcept
    {
        if (nullptr == pFindStorage_) {
            return ara::core::String{};
        }
        return pFindStorage_->dataStorage.stName;
    }
    /// @brief Get the save path of Storage from the JSON configuration
    /// @return
    inline ara::core::String GetStoragePath() const noexcept
    {
        if (nullptr == pFindStorage_) {
            return ara::core::String{};
        }
        return pFindStorage_->dataStorage.stPath;
    }
    /// @brief Get the persistent data version number from the configuration
    /// @return
    ara::per::isoftkv::PPerVersion GetVersionPer() const noexcept
    {
        ara::per::isoftkv::PPerVersion version;
        if (nullptr == pFindStorage_) {
            return version;
        }
        return ara::per::isoftkv::Version_StringToPer(pFindStorage_->dataStorage.stVersionPer);
    }
    /// @brief
    /// @return
    inline uint64_t GetSpaceAmountMin() const noexcept
    {
        PER_Assert(nullptr != pFindStorage_);
        return pFindStorage_->dataStorage.nSizeMin;
    }
    /// @brief
    /// @return
    inline uint64_t GetSpaceAmountMax() const noexcept
    {
        PER_Assert(nullptr != pFindStorage_);
        return pFindStorage_->dataStorage.nSizeMax;
    }
    /// @brief Get the redundancy strategy
    /// @return
    inline ara::core::String const& GetReddStrategy() const noexcept
    {
        PER_Assert(nullptr != pFindStorage_);
        return pFindStorage_->dataStorage.stReddStrategy;
    }
    /// @brief Get the redundancy configuration
    /// @return
    inline ara::per::manifest::MConfigData_Redundancy const& GetReddConfig() const noexcept
    {
        PER_Assert(nullptr != pFindStorage_);
        return pFindStorage_->dataStorage.reddConfig;
    }
    /// @brief Get the update strategy
    /// @return
    inline ara::core::String const& GetUpdateStrategy() const noexcept
    {
        PER_Assert(nullptr != pFindStorage_);
        return pFindStorage_->dataStorage.stUpdateStrategy;
    }
};
//********************************/
/// @brief Get StorageManager
/// @tparam T_Storage
/// @return
template < typename T_Storage >
inline ara::per::isoftkv::PSharedPtrManager< T_Storage >& G_GetStorageManager() noexcept
{
    /// @brief
    static ara::per::isoftkv::PSharedPtrManager< T_Storage > s_StorageManager;
    return s_StorageManager;
}
/// @brief Ensure a usable Storage is found
/// @tparam T_Storage
/// @param portIns
/// @return
/// @throws
template < typename T_Storage >
inline ara::core::Result< std::shared_ptr< T_Storage > > G_PrepareStorage(
    ara::core::InstanceSpecifier const& portIns) noexcept
{
    ara::core::String stRecordWord{"TimeRecord.POpenStorageLogic::PrepareStorage.PortIns<"};
    stRecordWord += portIns.ToString();
    stRecordWord += ">";
    ara::per::isoftkv::PAutoTimeRecord const autoTimeRecord{ara::per::isoftkv::T_StringView(stRecordWord)};
    PReadStorageConfig const jsonConfig{portIns};
    // autoTimeRecord.LogTimeElapseInfo(isoftkv::T_StringView("__ReadConfig"));
    if (false == jsonConfig.IsConfigValid()) {
        return ara::core::Result< std::shared_ptr< T_Storage > >::FromError(ara::per::PerErrc::kStorageNotFound);
    }
    if (jsonConfig.GetSpaceAmountMin()
        < static_cast< uint64_t >(ara::per::isoftkv::EDefaultValue::kDefSpaceAmountMin)) {
        return ara::core::Result< std::shared_ptr< T_Storage > >::FromError(ara::per::PerErrc::kOutOfStorageSpace);
    }
    if (jsonConfig.GetSpaceAmountMax()
        < static_cast< uint64_t >(ara::per::isoftkv::EDefaultValue::kDefSpaceAmountMin)) {
        return ara::core::Result< std::shared_ptr< T_Storage > >::FromError(ara::per::PerErrc::kOutOfStorageSpace);
    }
    ara::core::String const stStorageName{jsonConfig.GetStorageName()};
    std::shared_ptr< T_Storage > pFindStorage{std::move(G_GetStorageManager< T_Storage >().Find(stStorageName))};
    autoTimeRecord.LogTimeElapseInfo(ara::per::isoftkv::T_StringView("__pFindStorage"));
    if (false == pFindStorage.operator bool()) {
        pFindStorage = ara::per::isoftkv::POpenStorageLogic< T_Storage >::NewStorage(portIns);
        autoTimeRecord.LogTimeElapseInfo(ara::per::isoftkv::T_StringView("__NewStorage"));
    }
    PER_Assert(true == pFindStorage.operator bool());
    return ara::core::Result< std::shared_ptr< T_Storage > >::FromValue(pFindStorage);
}
//***************/
/// @brief Determine whether the given version number structure is the initial version number (0)
/// @param perVersion
/// @return
bool G_IsZeroVersion(ara::per::isoftkv::PPerVersion const& perVersion) noexcept
{
    if (perVersion.sVersionMajor > 0U) {
        return false;
    }
    if (perVersion.sVersionMinor > 0U) {
        return false;
    }
    if (perVersion.sVersionPatch > 0U) {
        return false;
    }
    if (perVersion.sVersionElse > 0U) {
        return false;
    }
    return true;
}
//***************/
/// @brief Get the Per version number of the persistent data
/// @tparam T_Storage
/// @param pStorage
/// @return
/// @throws
template < typename T_Storage >
inline ara::per::isoftkv::PPerVersion G_GetVersion_LocalPer(T_Storage const* const pStorage) noexcept
{
    PER_Assert(nullptr != pStorage);
    ara::per::isoftkv::PKvSystem* const pKvSystem{pStorage->GetKvSystem()};
    PER_Assert(nullptr != pKvSystem);
    return pKvSystem->GetPerVersion_StoragePer();
}
/// @brief Get the App version number of the persistent data
/// @tparam T_Storage
/// @param pStorage
/// @return
/// @throws
template < typename T_Storage >
inline ara::per::isoftkv::PPerVersion G_GetVersion_LocalApp(T_Storage const* const pStorage) noexcept
{
    PER_Assert(nullptr != pStorage);
    ara::per::isoftkv::PKvSystem* const pKvSystem{pStorage->GetKvSystem()};
    PER_Assert(nullptr != pKvSystem);
    return pKvSystem->GetPerVersion_StorageApp();
}
/// @brief Check the space usage of Storage
/// @param pStorage
/// @param nMinSpaceInConfig
/// @param nMaxSpaceInConfig
/// @return
template < typename T_Storage >
inline bool G_CheckStorageSpace(T_Storage const* const pStorage,
                                uint64_t const nMinSpaceInConfig,
                                uint64_t const nMaxSpaceInConfig) noexcept
{
    ara::core::Result< uint64_t > const resultSpace{std::move(pStorage->GetStorageSpace())};
    if (false == resultSpace.HasValue()) {
        return false;
    }
    uint64_t const nSpaceSize{resultSpace.Value()};
    if ((nSpaceSize < nMinSpaceInConfig) || (nSpaceSize > nMaxSpaceInConfig)) {
        return false;
    }
    return true;
}
/// @brief Open a KV library
/// @tparam T_Storage
/// @param pStorage
/// @return
template < typename T_Storage >
inline bool G_OpenMain(T_Storage const* const pStorage) noexcept
{
    PER_Assert(nullptr != pStorage);
    ara::per::isoftkv::PKvSystem* const pMainKvSystem{pStorage->GetKvSystem()};
    PER_Assert(nullptr != pMainKvSystem);
    return pMainKvSystem->OpenSystem();
}
/// @brief Compare the versions of the backup library
/// @tparam T_Storage
/// @param pStorage
/// @param perVersionInJSon Persistent data version number in the configuration file
/// @return
template < typename T_Storage >
inline int32_t G_CompareBackupVersion(T_Storage const* const pStorage,
                                      ara::per::isoftkv::PPerVersion const& perVersionInJSon) noexcept
{
    PER_Assert(nullptr != pStorage);
    ara::per::isoftkv::PKvSystem* const pMainKvSystem{pStorage->GetKvSystem()};
    PER_Assert(nullptr != pMainKvSystem);
    ara::core::String stBakFileName = pMainKvSystem->GetBakFileName(ara::per::isoftkv::ECacheSource::kMain);
    if (false == ara::per::isoftkv::PFileOpt::IsFileExist(ara::per::isoftkv::T_StringView(stBakFileName))) {
        return -1;
    }
    ara::per::isoftkv::PConfigMuster_Kv const bakConfig{pMainKvSystem->GetBakConfigMuster()};
    // The backup of FileStorage is to transfer all files to the backup directory
    std::unique_ptr< ara::per::isoftkv::PKvSystem > pKvSystemBak{
        ara::per::isoftkv::MakeUniqueKvSystem(bakConfig, false)};
    if (false == pKvSystemBak->CheckSystem()) {
        return -1;
    }
    std::ignore = pKvSystemBak->OpenSystem();
    if (false == pKvSystemBak->IsAccessReady()) {
        return -1;
    }
    ara::per::isoftkv::PPerVersion const perVersionBack{pKvSystemBak->GetPerVersion_StoragePer()};
    return ara::per::isoftkv::CompareVersion(perVersionBack, perVersionInJSon);
}
}  // namespace

namespace ara {
namespace per {
namespace isoftkv {
//********************************/
/// @brief Update callback function: global
CB_ApplicationDataUpdate g_FunApplicationDataUpdate{nullptr};  // NOLINT [fuchsia-statically-constructed-objects]
/// @brief Register an Update callback function
/// @param pfunc
void RegUpdateCallback(CB_ApplicationDataUpdate pfunc) { g_FunApplicationDataUpdate = std::move(pfunc); }

/// @brief Return the callback function registered for Update
/// @return
CB_ApplicationDataUpdate GetUpdateCallback() noexcept { return g_FunApplicationDataUpdate; }
//********************************/
/// @brief Create an interface object for performing Redd verification
/// @param configMuster
/// @return
std::unique_ptr< isoftkv::IReddAlgorithm > MakeReddCheckAlgorithm(isoftkv::PConfigMuster const& configMuster) noexcept
{
    if (false == configMuster.IsReddCheckType()) {
        return nullptr;
    }
    // Create possibly existing redundancy
    isoftkv::PReddDataCrc const& reddDataCrc{configMuster.GetReddData_Crc()};
    if (false == reddDataCrc.stAlgorithmFamily.empty()) {
        return std::make_unique< isoftkv::PReddAlgorithm_Crc >(reddDataCrc);
    }
    isoftkv::PReddDataHash const& reddDataHash{configMuster.GetReddData_Hash()};
    if (false == reddDataHash.stAlgorithmFamily.empty()) {
        return std::make_unique< isoftkv::PReddAlgorithm_Hash >(reddDataHash);
    }
    return nullptr;
}
/// @brief Return a newly created unique pointer
/// @param kvConfig
/// @param bForceCreateNew Whether to create a new library if it does not exist
/// @return
std::unique_ptr< isoftkv::PKvSystem > MakeUniqueKvSystem(isoftkv::PConfigMuster_Kv const& kvConfig,
                                                         bool bForceCreateNew) noexcept
{
    std::unique_ptr< isoftkv::PKvSystem > pKvSystemPtr{std::make_unique< isoftkv::PKvSystem >(kvConfig)};
    isoftkv::PKvSystem* const pKvSystem{pKvSystemPtr.get()};
    PER_Assert(nullptr != pKvSystem);
    if (pKvSystem->CheckSystem()) {
        std::ignore = pKvSystem->OpenSystem();
    }
    if (false == pKvSystem->IsAccessReady()) {
        if (bForceCreateNew) {
            std::ignore = pKvSystem->NewSystem(kvConfig.GetInitPageCount());
        }
    }
    if ((nullptr != pKvSystem) && (pKvSystem->IsHaveError())) {
        isoftkv::LogError() << "[PH_System].MakeUniqueKvSystem<" << pKvSystem->GetLogKvName().data()
                            << ">, MainFile = " << pKvSystem->GetMainFileName(pKvSystem->GetWorkPath()).data();
    }
    // Create possibly existing redundancy
    std::unique_ptr< isoftkv::IReddAlgorithm > pReddAlgorithm{MakeReddCheckAlgorithm(kvConfig)};
    if (pReddAlgorithm) {
        pKvSystem->AttachReddAlgorithm(std::move(pReddAlgorithm));
    }
    return pKvSystemPtr;
}
//********************************/
/// @brief Check if Storage is working
/// @tparam T_Storage
/// @param portIns
/// @return
template < typename T_Storage >
inline bool POpenStorageLogic< T_Storage >::IsStorageBusy(ara::core::InstanceSpecifier const& portIns) noexcept
{
    PReadStorageConfig const jsonConfig{portIns};
    if (false == jsonConfig.IsConfigValid()) {
        return false;
    }
    return G_GetStorageManager< T_Storage >().HasObject(jsonConfig.GetStorageName());
}
/// @brief Unregister after FileStorage releases
/// @tparam T_Storage
/// @param stStorageName
template < typename T_Storage >
inline void POpenStorageLogic< T_Storage >::DegStorage(ara::core::String const& stStorageName) noexcept
{
    G_GetStorageManager< T_Storage >().Deg(stStorageName);
}
/// @brief Delete a File data center: 2023-08-11 internal interface
/// @param[in] fs The shortName path of a PortPrototype typed by a PersistencyKeyValueStorageInterface.
/// @returns
/// A Result of void. In case of an error, it contains any of the errors defined below,
/// or a vendor specific error.
template < typename T_Storage >
inline ara::core::Result< void > POpenStorageLogic< T_Storage >::RemoveStorage(
    ara::core::InstanceSpecifier const& portIns) noexcept
{
    if (false == isoftkv::IsPerInitialize()) {
        return ara::core::Result< void >::FromError(PerErrc::kNotInitialized);
    }
    if (isoftkv::POpenStorageLogic< T_Storage >::IsStorageBusy(portIns)) {
        return ara::core::Result< void >::FromError(PerErrc::kResourceBusy);
    }
    if (false == isoftkv::POpenStorageLogic< T_Storage >::IsStorageExist(portIns)) {
        // return ara::core::Result<void>::FromError(PerErrc::kStorageNotFound);
        return ara::core::Result< void >::FromValue();
    }
    std::shared_ptr< T_Storage > pFindStorage{std::move(isoftkv::POpenStorageLogic< T_Storage >::NewStorage(portIns))};
    if (nullptr == pFindStorage) {
        return ara::core::Result< void >::FromError(PerErrc::kStorageNotFound);
    }
    std::ignore = pFindStorage->RemoveMain();
    std::ignore = pFindStorage->RemoveBackup();
    return ara::core::Result< void >::FromValue();
}
//***************/
/// @brief The function logic consists of the following SWS_PER: SWS_PER_00396, SWS_PER_00387, SWS_PER_00446, SWS_PER_00382, SWS_PER_00383
/// @tparam T_Storage
/// @param portIns
/// @return
template < typename T_Storage >
inline ara::core::Result< std::shared_ptr< T_Storage > > POpenStorageLogic< T_Storage >::DoOpenLogic(
    ara::core::InstanceSpecifier const& portIns) noexcept
{
    ara::core::String stRecordWord{"TimeRecord.POpenStorageLogic::DoOpenLogic.PortIns<"};
    stRecordWord += portIns.ToString();
    stRecordWord += ">";
    // isoftkv::PAutoTimeRecord const autoTimeRecord{isoftkv::T_StringView(stRecordWord)};
    // 1. Find the corresponding KV-Storage
    ara::core::Result< std::shared_ptr< T_Storage > > const resultPrepare{
        std::move(G_PrepareStorage< T_Storage >(portIns))};
    if (false == resultPrepare.HasValue()) {
        return isoftkv::T_ErrorResult< std::shared_ptr< T_Storage >, std::shared_ptr< T_Storage > >(resultPrepare);
    }
    std::shared_ptr< T_Storage > pFindStoragePtr{resultPrepare.Value()};
    PER_Assert(true == pFindStoragePtr.operator bool());
    T_Storage* pFindStorage = pFindStoragePtr.get();
    // autoTimeRecord.LogTimeElapseInfo(isoftkv::T_StringView("PrepareStorage"));

    PReadStorageConfig const jsonConfig{portIns};
    isoftkv::PPerVersion const perVersionInJson{jsonConfig.GetVersionPer()};
    isoftkv::PPerVersion const perVersionLocal{G_GetVersion_LocalPer(pFindStorage)};
    isoftkv::PPerVersion const appVersionLocal{G_GetVersion_LocalApp(pFindStorage)};
    int32_t const nCompareVersion{isoftkv::CompareVersion(perVersionLocal, perVersionInJson)};
    // 2. Compare the version number in the JSON with the version number in the local Storage
    if (0 == nCompareVersion) {  // Same: delete backup, no other actions
        LogDebug() << "Open Storage logic [same version number]: delete backup, no other actions";
        std::ignore = pFindStorage->RemoveBackup();
    } else if (
        nCompareVersion
        < 0) {  // JSON version is larger: delete backup, make current library as new backup, clear current library, reinitialize new library
        LogDebug() << "Open Storage logic [JSON version is larger]: delete backup, make current library as new backup, "
                      "clear current library, reinitialize new library";
        std::ignore = pFindStorage->RemoveBackup();
        bool bNewBuild{true};
        if (false == G_IsZeroVersion(perVersionLocal)) {
            if (false == pFindStorage->GetKvSystem()->IsNullStore()) {
                std::ignore = pFindStorage->BackupMain();
                if (jsonConfig.GetUpdateStrategy() == manifest::kUpdateDataDelete) {
                    std::ignore = pFindStorage->RemoveMain();
                } else {
                    bNewBuild = false;
                }
            }
        } else {
            bNewBuild = false;
        }
        ara::core::Result< void > const init{std::move(pFindStorage->InitStorage(portIns, bNewBuild))};
        if (false == init.HasValue()) {
            return isoftkv::T_ErrorResult< void, std::shared_ptr< T_Storage > >(init);
        }
    } else {  // JSON version is smaller: continue comparing backup
        if (0 == G_CompareBackupVersion(pFindStorage, perVersionInJson)) {
            // Backup library version matches JSON version: delete current library, promote backup library to primary, delete backup library
            LogDebug() << "Open Storage logic [backup library version matches JSON version]: delete current library, "
                          "promote backup library to primary, delete backup library";
            std::ignore = pFindStorage->RemoveMain();
            std::ignore = pFindStorage->RecoverFromBackup();
            std::ignore = pFindStorage->RemoveBackup();
        } else {
            // Backup library version does not match JSON version: delete backup, clear current library, reinitialize new library
            LogDebug() << "Open Storage logic [backup library version does not match JSON version]: delete backup, "
                          "clear current library, reinitialize new library";
            std::ignore = pFindStorage->RemoveBackup();
            std::ignore = pFindStorage->RemoveMain();
            ara::core::Result< void > const init{std::move(pFindStorage->InitStorage(portIns, true))};
            if (false == init.HasValue()) {
                return isoftkv::T_ErrorResult< void, std::shared_ptr< T_Storage > >(init);
            }
        }
    }
    // autoTimeRecord.LogTimeElapseInfo(isoftkv::T_StringView("_DealVersionPer"));
    if (false == G_OpenMain(pFindStorage)) {
        return ara::core::Result< std::shared_ptr< T_Storage > >::FromError(PerErrc::kPhOptKvFile);
    }
    if (false == G_CheckStorageSpace(pFindStorage, jsonConfig.GetSpaceAmountMin(), jsonConfig.GetSpaceAmountMax())) {
        return ara::core::Result< std::shared_ptr< T_Storage > >::FromError(PerErrc::kOutOfStorageSpace);
    }
    // autoTimeRecord.LogTimeElapseInfo(isoftkv::T_StringView("DoOpenLogic"));
    isoftkv::CB_ApplicationDataUpdate pUpdateCallback = isoftkv::GetUpdateCallback();
    if (pUpdateCallback) {
        isoftkv::PPerVersion const perVersionLocal2{std::move(G_GetVersion_LocalPer(pFindStorage))};
        isoftkv::PPerVersion const appVersionLocal2{std::move(G_GetVersion_LocalApp(pFindStorage))};
        if (0 != isoftkv::CompareVersion(perVersionLocal2, appVersionLocal2)) {
        }
        manifest::PManifestReader_Per* const pManifestPer{manifest::PManifestInstance::get()};
        if (nullptr == pManifestPer) {
            return ara::core::Result< std::shared_ptr< T_Storage > >::FromError(ara::per::PerErrc::kStorageNotFound);
        }
        ara::core::String stVersionAppConfig = pManifestPer->GetVersionApp();
        isoftkv::PPerVersion appVersionConfig{isoftkv::Version_StringToPer(stVersionAppConfig)};
        if (isoftkv::CompareVersion(appVersionConfig, appVersionLocal) > 0) {
            LogDebug() << "Update Success: Storage = \"" << portIns.ToString().data() << "\", LocalVersion=\""
                       << isoftkv::Version_PerToString(appVersionLocal) << "\", ConfigVersion=\""
                       << stVersionAppConfig.c_str() << "\"";
            pUpdateCallback(portIns, stVersionAppConfig);
        }
    }
    return ara::core::Result< std::shared_ptr< T_Storage > >::FromValue(pFindStoragePtr);
}
//********************************/
/// @brief Create a new FileStorage
/// @param portIns
/// @return
template <>
std::shared_ptr< FileStorage > POpenStorageLogic< FileStorage >::NewStorage(
    ara::core::InstanceSpecifier const& portIns) noexcept
{
    isoftkv::PConfigMuster_File fileConfig;
    bool const bAssembleConfig{fileConfig.AssembleFileConfig(portIns)};
    if (false == bAssembleConfig) {
        isoftkv::LogInfo() << "[OpenFileStorage].AssembleFileConfig" << portIns.ToString().data();
    }
    PER_Assert(bAssembleConfig);
    std::shared_ptr< FileStorage > pFindStorage{isoftkv::T_CreateShared< FileStorage >(fileConfig)};
    ara::core::Result< void > const resultInit{pFindStorage->PrepareKvSystem()};
    if (false == resultInit.HasValue()) {
        return nullptr;
    }
    PER_Assert(resultInit.HasValue());
    std::ignore = G_GetStorageManager< FileStorage >().Reg(fileConfig.GetStorageName(), pFindStorage);
    return pFindStorage;
}
/// @brief Check if Storage exists
/// @tparam T_Storage
/// @param[in] portIns
/// @return
template <>
bool POpenStorageLogic< FileStorage >::IsStorageExist(ara::core::InstanceSpecifier const& portIns) noexcept
{
    isoftkv::PConfigMuster_File fileConfig;
    bool const bAssembleConfig{fileConfig.AssembleFileConfig(portIns)};
    if (false == bAssembleConfig) {
        ara::per::isoftkv::LogInfo() << "[OpenFileStorage].AssembleFileConfig" << portIns.ToString().data();
    }
    PER_Assert(bAssembleConfig);
    ara::per::isoftkv::PConfigMuster_Kv kvConfig;
    std::ignore = kvConfig.SetConfig(fileConfig);
    return ara::per::isoftkv::PKvSystem::IsKvSystemExist(kvConfig);
}
//***************/
/// @brief Create a new KeyValueStorage
/// @param portIns
/// @return
template <>
std::shared_ptr< KeyValueStorage > POpenStorageLogic< KeyValueStorage >::NewStorage(
    ara::core::InstanceSpecifier const& portIns) noexcept
{
    isoftkv::PConfigMuster_Kv kvConfig;
    bool const bAssembleConfig{kvConfig.AssembleKvConfig(portIns)};
    if (false == bAssembleConfig) {
        isoftkv::LogInfo() << "[NewStorage].AssembleKvConfig" << portIns.ToString().data();
    }
    PER_Assert(bAssembleConfig);
    std::shared_ptr< KeyValueStorage > pFindStorage{
        isoftkv::T_CreateShared< KeyValueStorage, isoftkv::PConfigMuster_Kv const& >(kvConfig)};
    std::ignore = G_GetStorageManager< KeyValueStorage >().Reg(kvConfig.GetStorageName(), pFindStorage);
    return pFindStorage;
}
/// @brief Check if Storage exists
/// @tparam T_Storage
/// @param[in] portIns
/// @return
template <>
bool POpenStorageLogic< KeyValueStorage >::IsStorageExist(ara::core::InstanceSpecifier const& portIns) noexcept
{
    ara::per::isoftkv::PConfigMuster_Kv kvConfig;
    bool const bAssembleConfig{kvConfig.AssembleKvConfig(portIns)};
    if (false == bAssembleConfig) {
        ara::per::isoftkv::LogInfo() << "[OpenFileStorage].AssembleFileConfig" << portIns.ToString().data();
    }
    PER_Assert(bAssembleConfig);
    return ara::per::isoftkv::PKvSystem::IsKvSystemExist(kvConfig);
}
//********************************/ // Specialize two classes
template class POpenStorageLogic< FileStorage >;
template class POpenStorageLogic< KeyValueStorage >;
//********************************/
}  // namespace isoftkv
}  // namespace per
}  // namespace ara
