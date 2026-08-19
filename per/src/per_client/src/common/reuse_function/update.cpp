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
/// @file       update.cpp
/// @brief      AutoSar-AP data persistence storage module
/// @details    AP-Per update module
/// @date       2021-04-28
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification Log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// <tr><td>2021-04-28  <td>1.0.0    <td>hanjingjing      <td>Create initial version
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/per/Common/Reusable Functions
/// @interface_level=module
/// @trace_id_sr=
/// @unit_name=
/// @unit_description=Update
/// @endcode
///
/// ================================================================

#include "ara/per/update.h"

#include <isoft/ara_fsh/filesystem_hierarchy.h>

#include "ara/per/file_storage.h"
#include "ara/per/internal/common/isoft_file_opt.h"
#include "ara/per/internal/isoftkv/kv_data_type.h"
#include "ara/per/internal/isoftkv/kv_log_api.h"
#include "ara/per/internal/isoftkv/kv_system.h"
#include "ara/per/internal/manifest/manifest_instance.h"
#include "ara/per/internal/open_storage_logic.h"
#include "ara/per/key_value_storage.h"
#include "ara/per/per_error_domain.h"
#include "ara/per/read_accessor.h"

namespace {
//********************************/
/// @brief Update corresponding Storage system according to manifest configuration
/// @param kvConfig
/// @return
bool G_UpdateKvStorage(ara::per::manifest::MConfig_StorageKv const& kvConfig) noexcept
{
    if (kvConfig.dataStorage.stPath.empty()) {
        return false;
    }
    ara::core::InstanceSpecifier const perIns{kvConfig.stPortIns};
    ara::core::Result< ara::per::SharedHandle< ara::per::KeyValueStorage > > const resultOpen{
        ara::per::OpenKeyValueStorage(perIns)};
    if (false == resultOpen.HasValue()) {
        return false;
    }
    return true;
}
/// @brief
/// @param fileConfig
/// @return
bool G_UpdateFileStorage(ara::per::manifest::MConfig_StorageFile const& fileConfig) noexcept
{
    if (fileConfig.dataStorage.stPath.empty()) {
        return false;
    }
    ara::core::InstanceSpecifier const perPort{fileConfig.stPortIns};
    ara::core::Result< ara::per::SharedHandle< ara::per::FileStorage > > const resultOpen{
        ara::per::OpenFileStorage(perPort)};
    if (false == resultOpen.HasValue()) {
        return false;
    }
    return true;
}
//********************************/
/// @brief Clear corresponding Storage system according to manifest configuration
/// @param perPort
/// @return
bool G_ResetKvStorage(ara::per::manifest::MConfig_StorageKv const& kvConfig) noexcept
{
    ara::core::InstanceSpecifier const perIns{kvConfig.stPortIns};
    ara::core::Result< void > const resultOpen{ara::per::ResetKeyValueStorage(perIns)};
    if (false == resultOpen.HasValue()) {
        return false;
    }
    return true;
}
/// @brief
/// @param perPort
/// @return
bool G_ResetFileStorage(ara::per::manifest::MConfig_StorageFile const& fileConfig) noexcept
{
    ara::core::InstanceSpecifier const perPort{fileConfig.stPortIns};
    ara::core::Result< void > const resultOpen{ara::per::ResetAllFiles(perPort)};
    if (false == resultOpen.HasValue()) {
        return false;
    }
    return true;
}
}  // namespace

namespace ara {
namespace per {
//********************************/
/// @brief Register callback function when updating, opening, or creating data center, handling version consistency issues.
/// @param appDataUpdateCallback
void RegisterApplicationDataUpdateCallback(
    std::function< void(const ara::core::InstanceSpecifier& storage, ara::core::String version) > const&
        appDataUpdateCallback) noexcept
{
    isoftkv::RegUpdateCallback(std::move(appDataUpdateCallback));
}
/// @brief After manifest installation completes, call this function to update all files and key-value data centers.
/// @return
ara::core::Result< void > UpdatePersistency() noexcept
{
    // Preliminary judgment function as follows: Get all created File/KV data centers from Manifest, then perform update operations
    manifest::PManifestReader_Per* const pManifestPer{manifest::PManifestInstance::get()};
    if (nullptr == pManifestPer) {
        return ara::core::Result< void >::FromError(ara::per::PerErrc::kStorageNotFound);
    }
    uint32_t nTotalCount{0U};
    uint32_t* const pnTotalCount{&nTotalCount};
    uint32_t nSuccessCount{0U};
    nSuccessCount
        = pManifestPer->ForEachStorage_Kv([pnTotalCount](manifest::MConfig_StorageKv const& kvConfig) noexcept -> bool {
              *pnTotalCount += 1U;
              bool bUpdate{G_UpdateKvStorage(kvConfig)};
              if (false == bUpdate) {
                  isoftkv::LogError() << "When UpdatePersistency " << kvConfig.stStorageIns.c_str();
              }
              return bUpdate;
          });
    nSuccessCount += pManifestPer->ForEachStorage_File(
        [pnTotalCount](manifest::MConfig_StorageFile const& fileConfig) noexcept -> bool {
            *pnTotalCount += 1U;
            bool bUpdate{G_UpdateFileStorage(fileConfig)};
            if (false == bUpdate) {
                isoftkv::LogError() << "When UpdatePersistency " << fileConfig.stStorageIns.c_str();
            }
            return bUpdate;
        });
    if ((nSuccessCount < nTotalCount) && (nTotalCount > 0U)) {
        return ara::core::Result< void >::FromError(PerErrc::kValidationFailed);
    }
    return ara::core::Result< void >::FromValue();
}
/// @brief Completely reset all files and key-value data centers. To uninstall persistence data related to itself before App uninstallation, call ara::per::ResetPersistency.
/// @return
ara::core::Result< void > ResetPersistency() noexcept
{
    // If tracking all custom Storage, need to persistently save current Storage information every time OpenXX is called
    manifest::PManifestReader_Per* const pManifestPer{manifest::PManifestInstance::get()};
    if (nullptr == pManifestPer) {
        return ara::core::Result< void >::FromError(ara::per::PerErrc::kStorageNotFound);
    }
    uint32_t nTotalCount{0U};
    uint32_t* const pnTotalCount{&nTotalCount};
    uint32_t nSuccessCount{0U};
    nSuccessCount
        = pManifestPer->ForEachStorage_Kv([pnTotalCount](manifest::MConfig_StorageKv const& kvConfig) noexcept -> bool {
              *pnTotalCount += 1U;
              bool bReset{G_ResetKvStorage(kvConfig)};
              if (false == bReset) {
                  isoftkv::LogError() << "When ResetPersistency " << kvConfig.stStorageIns.c_str();
              }
              return bReset;
          });
    nSuccessCount += pManifestPer->ForEachStorage_File(
        [pnTotalCount](manifest::MConfig_StorageFile const& fileConfig) noexcept -> bool {
            *pnTotalCount += 1U;
            bool bReset{G_ResetFileStorage(fileConfig)};
            if (false == bReset) {
                isoftkv::LogError() << "When ResetPersistency " << fileConfig.stStorageIns.c_str();
            }
            return bReset;
        });
    if ((nSuccessCount < nTotalCount) && (nTotalCount > 0U)) {
        return ara::core::Result< void >::FromError(PerErrc::kValidationFailed);
    }
    return ara::core::Result< void >::FromValue();
}
//********************************/
/// [SWS_PER_00389] [SWS_PER_00393]
/// @brief Clean up unused Storage libraries left after update
/// @param stEtcPathOld Old version ETC directory
/// @param stEtcPathNew New version ETC directory
/// @return Success or failure
bool ClearUpPersistency(ara::core::StringView const& stEtcPathOld, ara::core::StringView const& stEtcPathNew) noexcept
{
    isoft::ara_fsh::Platform const fshPlatform;
    std::string const stVarPath{fshPlatform.GetAraVarDir()};
    ara::core::String stConfigNameOld{stEtcPathOld};
    stConfigNameOld += "/";
    stConfigNameOld += isoft::ara_fsh::Process::kStorage;
    manifest::PManifestReader_Per configOld{isoftkv::T_StringView(stVarPath.data(), stVarPath.size())};
    if (false == configOld.InitManifest(isoftkv::T_StringView(stConfigNameOld))) {
        return false;
    }
    // First calculate all lists to be deleted: Old list - New list
    if (false == stEtcPathNew.empty()) {
        ara::core::String stConfigNameNew{stEtcPathNew};
        stConfigNameNew += "/";
        stConfigNameNew += isoft::ara_fsh::Process::kStorage;
        manifest::PManifestReader_Per configNew{isoftkv::T_StringView(stVarPath.data(), stVarPath.size())};
        if (configNew.InitManifest(isoftkv::T_StringView(stConfigNameNew))) {
            std::ignore = configNew.ForEachStorage_Kv([&configOld](manifest::MConfig_StorageKv const& config) -> bool {
                return configOld.DelStorage_Kv(config.stStorageIns);
            });
            std::ignore
                = configNew.ForEachStorage_File([&configOld](manifest::MConfig_StorageFile const& config) -> bool {
                      return configOld.DelStorage_File(config.stStorageIns);
                  });
        }
    }
    uint32_t nSuccessCount{0U};
    // Delete possible leftover KV libraries
    nSuccessCount += configOld.ForEachStorage_Kv([&configOld](manifest::MConfig_StorageKv const& config) -> bool {
        isoftkv::PConfigMuster_Kv configMusterKv;
        bool const bAssembleConfig{configMusterKv.AssembleKvConfig(configOld.GetVersionApp(), config)};
        PER_Assert(bAssembleConfig);
        std::shared_ptr< KeyValueStorage > pFindStorage{
            isoftkv::T_CreateShared< KeyValueStorage, isoftkv::PConfigMuster_Kv const& >(configMusterKv)};
        if (pFindStorage->RemoveMain()) {
            std::ignore = pFindStorage->RemoveBackup();
            bool bDel{isoftkv::PFileOpt::RemoveDir(isoftkv::T_StringView(config.dataStorage.stPath))};
            isoftkv::LogInfo() << "Del.Result<" << bDel << ">, KvStorage.Pah = " << config.dataStorage.stPath.c_str();
            return true;
        }
        return false;
    });
    // Delete possible leftover FILE libraries
    nSuccessCount += configOld.ForEachStorage_File([&configOld](manifest::MConfig_StorageFile const& config) -> bool {
        isoftkv::PConfigMuster_File configMusterFile;
        bool const bAssembleConfig{configMusterFile.AssembleFileConfig(configOld, config)};
        PER_Assert(bAssembleConfig);
        std::shared_ptr< FileStorage > pFindStorage{
            isoftkv::T_CreateShared< FileStorage, isoftkv::PConfigMuster_File const& >(configMusterFile)};
        ara::core::Result< void > const resultInit{pFindStorage->PrepareKvSystem()};
        if (false == resultInit.HasValue()) {
            return false;
        }
        if (pFindStorage->RemoveMain()) {
            std::ignore = pFindStorage->RemoveBackup();
            bool bDel{isoftkv::PFileOpt::RemoveDir(isoftkv::T_StringView(config.dataStorage.stPath))};
            isoftkv::LogInfo() << "Del.Result<" << bDel
                               << ">, FileStorage.Path = " << config.dataStorage.stPath.c_str();
            return true;
        }
        return false;
    });
    return true;
}
//********************************/
}  // namespace per
}  // namespace ara
