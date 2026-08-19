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
/// @file       config_iam_crypto.cpp
/// @brief      AutoSar-IAM-CRYPTO Configuration file
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
/// <tr><td>2025-04-14 <td>0.1 <td>Han Yuxin <td>Refactored IAM-CRYPTO
/// functionality
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/IAM/IAM-CRYPTO
/// @interface_level=Unit
/// @trace_id_sr=SR_IAM_00201
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

#include "config_iam_crypto.h"

#include <isoft/ara_fsh/filesystem_hierarchy.h>
#include <isoft/manifestreader/manifest_node.h>
#include <isoft/manifestreader/manifest_reader.h>

#include <functional>

#include "common/ara_log.h"

namespace {
//********************************/
/// @brief Get configuration file tag string constant: CryptoGrants
/// @return processname
/// @code{.isoft}
/// @interface_level=unit
/// @needwork = dd
/// @endcode
inline constexpr ara::iam::internal::char8_t const *GetJsonKey_CryptoGrants() noexcept { return "CryptoGrants"; }
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
inline constexpr ara::iam::internal::char8_t const *GetJsonKey_SlotName() noexcept { return "slotname"; }
}  // namespace

//********************************/
MR_HEADER(ara::iam::internal::crypto::PIamConfigData_Crypto)
MR_FIELD(GetJsonKey_ProcessName(), stProcessName)
MR_FIELD(GetJsonKey_SlotName(), vecSlogName)
MR_FOOTER
//********************************/
namespace ara {
namespace iam {
namespace internal {
namespace crypto {
//********************************/
/// @brief Load Crypto configuration data
/// @return load success/falied.
/// @code{.isoft}
/// @interface_level=unit
/// @needwork = ad
/// @endcode
bool PConfigIam_Crypto::Initialize() noexcept
{
    mapIamConfigCrypto_.clear();
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
bool PConfigIam_Crypto::Deinitialize() noexcept
{
    mapIamConfigCrypto_.clear();
    _ClearReady();
    SetErrorCode(ara::iam::internal::grant::GrantSerializationErrc::kInitCode);
    return true;
}
/// @brief Initialize configuration file
/// @param stFileName Configuration file name
/// @return true if init sucess false otherwise
bool PConfigIam_Crypto::InitManifest(ara::core::StringView const &stFileName) noexcept
{
    ara::core::Result< std::unique_ptr< isoft::manifestreader::Manifest > > manifestRes{
        isoft::manifestreader::OpenManifest(stFileName)};

    if (!manifestRes.HasValue()) {
        SetErrorCode(ara::iam::internal::grant::GrantSerializationErrc::kFileNotFound);
        return false;
    }
    std::unique_ptr< isoft::manifestreader::Manifest > const pManifestRes{std::move(manifestRes).Value()};
    common::IamLogger().LogInfo() << "PConfigIam_Crypto::LoadData ConfigName = " << stFileName.data();

    std::ignore = common::ReadMapData< PIamConfigData_Crypto >(
        pManifestRes, ara::core::String(GetJsonKey_CryptoGrants()),
        [this](PIamConfigData_Crypto const &data) -> void { mapIamConfigCrypto_[data.stProcessName] = data; });

    _SetReady();
    return true;
}
//********************************/
/// @brief Determine whether a given keyslot is in the allowed list of a given
/// process
/// @param stProcessName Process name FQN
/// @param stSlotName Keyslot name
/// @return true if enable
bool PConfigIam_Crypto::GrantCheck(ara::core::String const &stProcessName,
                                   ara::core::String const &stSlotName) const noexcept
{
    SetErrorCode(ara::iam::internal::grant::GrantSerializationErrc::kInitCode);
    MAP_IamConfig_Crypto::const_iterator const &itFind = mapIamConfigCrypto_.find(stProcessName);
    if (itFind == mapIamConfigCrypto_.end()) {
        SetErrorCode(ara::iam::internal::grant::GrantSerializationErrc::kGrantNotFound);
        return false;
    }
    for (auto const &slotName : itFind->second.vecSlogName) {
        if (slotName == stSlotName) {
            return true;
        }
    }
    SetErrorCode(ara::iam::internal::grant::GrantSerializationErrc::kGrantNotFound);
    return false;
}
//********************************/
}  // namespace crypto
}  // namespace internal
}  // namespace iam
}  // namespace ara