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
/// @file       config_iam_idsm.cpp
/// @brief      AutoSar-IAM-CRYPTO Configuration file
/// @details
/// @date       2025-04-16
/// @author     Han Yuxin
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification Log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// <tr><td>2025-04-16 <td>0.1 <td>Han Yuxin <td>Refactored IAM-CRYPTO
/// functionality
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/IAM/IAM-CRYPTO
/// @interface_level=Unit
/// @trace_id_sr=SR_IAM_00401
/// @unit_name=IAM_CRYPTO
/// @unit_description=Configuration information provided by IAM to the CRYPTO
/// module
/// @endcode
///
/// ================================================================
///
/// rights reserved.
///
/// ================================================================

#include "config_iam_idsm.h"

#include <isoft/manifestreader/manifest_node.h>
#include <isoft/manifestreader/manifest_reader.h>

#include "common/ara_log.h"

namespace {
//********************************/
/// @brief Get configuration file tag string constant: processname
/// @return processname
/// @code{.isoft}
/// @interface_level=unit
/// @needwork = dd
/// @endcode
inline constexpr ara::iam::internal::char8_t const *GetJsonKey_IdsmGrants() noexcept { return "IdsmGrants"; }
/// @brief Get configuration file tag string constant: processname
/// @return processname
/// @code{.isoft}
/// @interface_level=unit
/// @needwork = dd
/// @endcode
inline constexpr ara::iam::internal::char8_t const *GetJsonKey_ProcessName() noexcept { return "processname"; }
/// @brief Get configuration file tag string constant: slotname
/// @return slotname
/// @code{.isoft}
/// @interface_level=unit
/// @needwork = dd
/// @endcode
inline constexpr ara::iam::internal::char8_t const *GetJsonKey_SecureEventID() noexcept { return "secureeventid"; }
}  // namespace

//********************************/
MR_HEADER(ara::iam::internal::idsm::PIamConfigData_Idsm)
MR_FIELD(GetJsonKey_ProcessName(), stProcessName)
MR_FIELD(GetJsonKey_SecureEventID(), vecSecureEventId)
MR_FOOTER
//********************************/
namespace ara {
namespace iam {
namespace internal {
namespace idsm {
//********************************/
/// @brief Load IDSM configuration data
/// @return load success/falied.
/// @code{.isoft}
/// @interface_level=unit
/// @needwork = ad
/// @endcode
bool PConfigIam_Idsm::Initialize() noexcept
{
    mapIamConfigIdsm_.clear();
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
bool PConfigIam_Idsm::Deinitialize() noexcept
{
    mapIamConfigIdsm_.clear();
    _ClearReady();
    SetErrorCode(ara::iam::internal::grant::GrantSerializationErrc::kInitCode);
    return true;
}
/// @brief Initialize configuration file
/// @param stFileName Configuration file name
/// @return true if init sucess false otherwise
bool PConfigIam_Idsm::InitManifest(ara::core::StringView const &stFileName) noexcept
{
    ara::core::Result< std::unique_ptr< isoft::manifestreader::Manifest > > manifestRes{
        isoft::manifestreader::OpenManifest(stFileName)};

    if (!manifestRes.HasValue()) {
        SetErrorCode(ara::iam::internal::grant::GrantSerializationErrc::kFileNotFound);
        return false;
    }
    std::unique_ptr< isoft::manifestreader::Manifest > const pManifestRes{std::move(manifestRes).Value()};
    common::IamLogger().LogInfo() << "PConfigIam_Idsm::LoadData ConfigName = " << stFileName.data();

    std::ignore = common::ReadMapData< PIamConfigData_Idsm >(
        pManifestRes, ara::core::String(GetJsonKey_IdsmGrants()),
        [this](PIamConfigData_Idsm const &data) -> void { mapIamConfigIdsm_[data.stProcessName] = data; });

    _SetReady();
    return true;
}
//********************************/
/// @brief Determine whether a given keyslot is in the allowed list of a given
/// process
/// @param stProcessName Process name FQN
/// @param nID Security event ID
/// @return true if enable
bool PConfigIam_Idsm::GrantCheck(ara::core::String const &stProcessName, uint32_t const &nID) const noexcept
{
    SetErrorCode(ara::iam::internal::grant::GrantSerializationErrc::kInitCode);
    MAP_IamConfig_Idsm::const_iterator const &itFind = mapIamConfigIdsm_.find(stProcessName);
    if (itFind == mapIamConfigIdsm_.end()) {
        SetErrorCode(ara::iam::internal::grant::GrantSerializationErrc::kGrantNotFound);
        return false;
    }
    for (auto const &id : itFind->second.vecSecureEventId) {
        if (id == nID) {
            return true;
        }
    }
    SetErrorCode(ara::iam::internal::grant::GrantSerializationErrc::kGrantNotFound);
    return false;
}
//********************************/
}  // namespace idsm
}  // namespace internal
}  // namespace iam
}  // namespace ara