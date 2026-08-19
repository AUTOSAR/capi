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
/// @file       config_iam_phm.cpp
/// @brief      AutoSar-IAM-PHM Configuration file
/// @details
/// @date       2025-04-14
/// @author     Han Yuxin
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification Log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// <tr><td>2025-04-14 <td>0.1 <td>Han Yuxin <td>Refactored IAM-PHM
/// functionality
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/IAM/IAM-PHM
/// @interface_level=Unit
/// @trace_id_sr=SR_IAM_00301
/// @unit_name=IAM_PHM
/// @unit_description=Configuration information provided by IAM to the PHM
/// module
/// @endcode
///
/// ================================================================
///
/// rights reserved.
///
/// ================================================================

#include "config_iam_phm.h"

#include <isoft/ara_fsh/filesystem_hierarchy.h>
#include <isoft/manifestreader/manifest_node.h>
#include <isoft/manifestreader/manifest_reader.h>

#include <functional>

#include "common/ara_log.h"

namespace {
//********************************/
/// @brief Get configuration file tag string constant: PhmCheckPointGrants
/// @return PhmCheckPointGrants
/// @code{.isoft}
/// @interface_level=unit
/// @needwork = dd
/// @endcode
inline constexpr ara::iam::internal::char8_t const *GetJsonKey_PhmCheckPointGrants() noexcept
{
    return "PhmCheckPointGrants";
}
/// @brief Get configuration file tag string constant: PhmChannelGrants
/// @return PhmChannelGrants
/// @code{.isoft}
/// @interface_level=unit
/// @needwork = dd
/// @endcode
inline constexpr ara::iam::internal::char8_t const *GetJsonKey_PhmChannelGrants() noexcept
{
    return "PhmChannelGrants";
}
/// @brief Get configuration file tag string constant: processname
/// @return processname
/// @code{.isoft}
/// @interface_level=unit
/// @needwork = dd
/// @endcode
inline constexpr ara::iam::internal::char8_t const *GetJsonKey_ProcessName() noexcept { return "processname"; }
/// @brief Get configuration file tag string constant: checkpointid
/// @return checkpointid
/// @code{.isoft}
/// @interface_level=unit
/// @needwork = dd
/// @endcode
inline constexpr ara::iam::internal::char8_t const *GetJsonKey_CheckPointId() noexcept { return "checkpointid"; }
/// @brief Get configuration file tag string constant: channelid
/// @return channelid
/// @code{.isoft}
/// @interface_level=unit
/// @needwork = dd
/// @endcode
inline constexpr ara::iam::internal::char8_t const *GetJsonKey_ChannelId() noexcept { return "channelid"; }
}  // namespace

//********************************/
MR_HEADER(ara::iam::internal::phm::PIamConfigData_PhmCheckPoint)
MR_FIELD(GetJsonKey_ProcessName(), stProcessName)
MR_FIELD(GetJsonKey_CheckPointId(), vecCheckPointId)
MR_FOOTER
MR_HEADER(ara::iam::internal::phm::PIamConfigData_PhmChannel)
MR_FIELD(GetJsonKey_ProcessName(), stProcessName)
MR_FIELD(GetJsonKey_ChannelId(), vecCheckChannel)
MR_FOOTER
//********************************/
namespace ara {
namespace iam {
namespace internal {
namespace phm {
//********************************/
/// @brief Load Phm configuration data
/// @return load success/falied.
/// @code{.isoft}
/// @interface_level=unit
/// @needwork = ad
/// @endcode
bool PConfigIam_Phm::Initialize() noexcept
{
    vecPhmCheckPoint_.clear();
    vecPhmCheckChannel_.clear();
    _ClearReady();
    SetErrorCode(ara::iam::internal::grant::GrantSerializationErrc::kInitCode);
    return true;
}
/// @brief Clear  All GrantInfo
/// @returns true
/// @code{.isoft}
/// @interface_level=unit
/// @needwork = ad
/// @endcode
bool PConfigIam_Phm::Deinitialize() noexcept
{
    vecPhmCheckPoint_.clear();
    vecPhmCheckChannel_.clear();
    _ClearReady();
    SetErrorCode(ara::iam::internal::grant::GrantSerializationErrc::kInitCode);
    return true;
}
/// @brief Initialize configuration file
/// @param stFileName Configuration file name
/// @return true if init sucess false otherwise
bool PConfigIam_Phm::InitManifest(ara::core::StringView const &stFileName) noexcept
{
    ara::core::Result< std::unique_ptr< isoft::manifestreader::Manifest > > manifestRes{
        isoft::manifestreader::OpenManifest(stFileName)};

    if (!manifestRes.HasValue()) {
        SetErrorCode(ara::iam::internal::grant::GrantSerializationErrc::kFileNotFound);
        return false;
    }
    std::unique_ptr< isoft::manifestreader::Manifest > const pManifestRes{std::move(manifestRes).Value()};
    common::IamLogger().LogInfo() << "PConfigIam_Phm::LoadData ConfigName = " << stFileName.data();

    bool bReady = common::ReadMapData< PIamConfigData_PhmCheckPoint >(
        pManifestRes, ara::core::String(GetJsonKey_PhmCheckPointGrants()),
        [this](PIamConfigData_PhmCheckPoint const &data) -> void { vecPhmCheckPoint_[data.stProcessName] = data; });
    if (false == bReady) {
        SetErrorCode(ara::iam::internal::grant::GrantSerializationErrc::kJsonLoadFailed);
        return false;
    }
    std::ignore = common::ReadMapData< PIamConfigData_PhmChannel >(
        pManifestRes, ara::core::String(GetJsonKey_PhmChannelGrants()),
        [this](PIamConfigData_PhmChannel const &data) -> void { vecPhmCheckChannel_[data.stProcessName] = data; });

    _SetReady();
    return true;
}
//********************************/
/// @brief grant checkpoint check.
/// @param stProcess  checkpoint fqn
/// @param nFindID checkpoint id
/// @return checkpoint check success or failed.
/// @code{.isoft}
/// @interface_level=unit
/// @needwork = ad
/// @endcode
bool PConfigIam_Phm::GrantCheck_CheckPoint(ara::core::String const &stProcess, uint32_t const &nFindID) noexcept
{
    SetErrorCode(ara::iam::internal::grant::GrantSerializationErrc::kInitCode);
    MAP_IamConfig_PhmCheckPoint::const_iterator const &itFind = vecPhmCheckPoint_.find(stProcess);
    if (itFind == vecPhmCheckPoint_.end()) {
        SetErrorCode(ara::iam::internal::grant::GrantSerializationErrc::kGrantNotFound);
        return false;
    }
    for (auto const &nID : itFind->second.vecCheckPointId) {
        if (nID == nFindID) {
            return true;
        }
    }
    SetErrorCode(ara::iam::internal::grant::GrantSerializationErrc::kGrantNotFound);
    return false;
}
/// @brief grant ChannelGrantCheck.
/// @param stProcess channel fqn
/// @param nFindID channel id
/// @return Channel check success or failed.
/// @code{.isoft}
/// @interface_level=unit
/// @needwork = ad
/// @endcode
bool PConfigIam_Phm::GrantCheck_Channel(ara::core::String const &stProcess, uint32_t const &nFindID) noexcept
{
    SetErrorCode(ara::iam::internal::grant::GrantSerializationErrc::kInitCode);
    MAP_IamConfig_PhmChannel::const_iterator const &itFind = vecPhmCheckChannel_.find(stProcess);
    if (itFind == vecPhmCheckChannel_.end()) {
        SetErrorCode(ara::iam::internal::grant::GrantSerializationErrc::kGrantNotFound);
        return false;
    }
    for (auto const &nID : itFind->second.vecCheckChannel) {
        if (nID == nFindID) {
            return true;
        }
    }
    SetErrorCode(ara::iam::internal::grant::GrantSerializationErrc::kGrantNotFound);
    return false;
}
//********************************/
}  // namespace phm
}  // namespace internal
}  // namespace iam
}  // namespace ara