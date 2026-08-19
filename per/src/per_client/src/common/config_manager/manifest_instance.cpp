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
/// @file       manifest_instance.cpp
/// @brief      AutoSar-AP data persistence storage module
/// @details    Manifest operations involved in ara/per
/// @date       2021-09-13
/// @author     jiawei
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification Log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// <tr><td>2021-09-13  <td>1.0.0    <td>      <td>Create initial version
/// </table>
///
/// ================================================================

#include "ara/per/internal/manifest/manifest_instance.h"

#include <isoft/ara_fsh/filesystem_hierarchy.h>

#include "ara/per/internal/common/isoft_assert.h"
#include "ara/per/internal/common/isoft_common_api.h"
#include "ara/per/per_error_domain.h"
//********************************/
namespace {
/// @brief
std::unique_ptr< ara::core::String > g_SetConfigFileNamePtr{nullptr};  // NOLINT
/// @brief
std::unique_ptr< ara::per::manifest::PManifestReader_Per > g_InstancePtr{nullptr};  // NOLINT
}  // namespace
//********************************/
namespace ara {
namespace per {
namespace manifest {
//********************************/
/// @brief
/// @return
PManifestReader_Per* PManifestInstance::get() noexcept
{
    if (nullptr != g_InstancePtr.get()) {
        return g_InstancePtr.get();
    }

    isoft::ara_fsh::Platform const fshPlatform;
    g_InstancePtr = std::make_unique< PManifestReader_Per >(fshPlatform.GetAraVarDir().data());
    // Configuration source priority: Set value > Value in manifest > Default value
    ara::core::String stConfigFile{};
    if (g_SetConfigFileNamePtr.operator bool()) {
        stConfigFile = *g_SetConfigFileNamePtr;
    }
    if (stConfigFile.empty()) {
        // Get Per configuration file name in main configuration file
        isoft::ara_fsh::Process const fsh{};
        stConfigFile = fsh.GetStorage();
    }
    if (stConfigFile.empty()) {
        stConfigFile = isoftkv::T_String(manifest::kManifestFileName);
    }
    /// 2025-11-18: niuliming: Return null pointer when reading configuration file fails
    if (g_InstancePtr->InitManifest(ara::core::StringView{stConfigFile.data(), stConfigFile.size()}) == false) {
        return nullptr;
    }
    return g_InstancePtr.get();
}
//***************/
/// @brief
/// @param stJsonConfig
void PManifestInstance::Initialize(ara::core::StringView const& stJsonConfig) noexcept
{
    if (static_cast< int32_t >(stJsonConfig.size()) > 0) {
        if (false == g_SetConfigFileNamePtr.operator bool()) {
            g_SetConfigFileNamePtr = std::make_unique< ara::core::String >();
        }
        *g_SetConfigFileNamePtr = stJsonConfig;
    }
}
/// @brief
void PManifestInstance::DeInitialize() noexcept
{
    if (g_SetConfigFileNamePtr.operator bool()) {
        g_SetConfigFileNamePtr->clear();
    }
    g_SetConfigFileNamePtr.reset();
    g_InstancePtr.reset();
}
/// @brief Find Kv library: Prioritize search by Port identifier, convert to StorageIns and search again if not found
/// @param portIns Port identifier / KvStorage library identifier
/// @return StorageKv configuration data
MConfig_StorageKv const* PManifestInstance::FindStorage_Kv(ara::core::InstanceSpecifier const& portIns) noexcept
{
    PManifestReader_Per* const pManifestPer{get()};
    if (nullptr == pManifestPer) {
        return nullptr;
    }
    ara::core::String const stPortIns{isoftkv::T_String(portIns.ToString())};
    MConfig_StorageKv const* pFindStorage{pManifestPer->FindStorageByPort_Kv(stPortIns)};
    if (nullptr == pFindStorage) {
        // Search again using portIns as PerIns
        pFindStorage = pManifestPer->FindStorageByPer_Kv(stPortIns);
    }
    return pFindStorage;
}
/// @brief Find File library: Prioritize searching by Port identifier; if not found, convert to StorageIns and search again
/// @param portIns Port identifier / KvStorage library identifier
/// @return StorageFile configuration data
MConfig_StorageFile const* PManifestInstance::FindStorage_File(ara::core::InstanceSpecifier const& portIns) noexcept
{
    PManifestReader_Per* const pManifestPer{get()};
    if (nullptr == pManifestPer) {
        return nullptr;
    }
    ara::core::String const stPortIns{isoftkv::T_String(portIns.ToString())};
    MConfig_StorageFile const* pFindStorage{pManifestPer->FindStorageByPort_File(stPortIns)};
    if (nullptr == pFindStorage) {
        // Search again using portIns as PerIns
        pFindStorage = pManifestPer->FindStorageByPer_File(stPortIns);
    }
    return pFindStorage;
}
/// @brief Find Kv/File library: Prioritize searching by Port identifier; if not found, convert to StorageIns and search again
/// @param portIns Port identifier / KvStorage library identifier
/// @param ePerType
/// @return StorageFile configuration data
MConfig_StorageBase const* PManifestInstance::FindStorage(ara::core::InstanceSpecifier const& portIns,
                                                          EPerType const ePerType) noexcept
{
    MConfig_StorageBase const* pFindStorage{nullptr};
    if ((nullptr == pFindStorage) && ((EPerType::kStorage_Any == ePerType) || (EPerType::kStorage_Kv == ePerType))) {
        pFindStorage = FindStorage_Kv(portIns);
    }
    if ((nullptr == pFindStorage) && ((EPerType::kStorage_Any == ePerType) || (EPerType::kStorage_File == ePerType))) {
        pFindStorage = FindStorage_File(portIns);
    }
    return pFindStorage;
}
/// @brief Convert PortIns to StorageIns string
/// @param portIns Port identifier / KvStorage library identifier
/// @param ePerType
/// @return
ara::core::String PManifestInstance::TransPortToStorageIns(ara::core::InstanceSpecifier const& portIns,
                                                           EPerType const ePerType) noexcept
{
    ara::core::String stReturn;
    MConfig_StorageBase const* const pFindStorage{FindStorage(portIns, ePerType)};
    if (nullptr != pFindStorage) {
        stReturn = pFindStorage->stStorageIns;
    }

    return stReturn;
}
//********************************/
}  // namespace manifest
}  // namespace per
}  // namespace ara
