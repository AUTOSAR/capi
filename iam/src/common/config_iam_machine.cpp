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
/// @file       config_iam_machine.cpp
/// @brief      AutoSar-IAM-COM Configuration file
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
/// <tr><td>2025-04-14 <td>0.1 <td>Han Yuxin <td>Refactored IAM-COM
/// functionality
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/IAM/IAM-COM
/// @interface_level=Unit
/// @trace_id_sr=SR_IAM_
/// @unit_name=IAM_COM
/// @unit_description=Configuration information provided by IAM to the COM
/// module
/// @endcode
///
/// ================================================================
///
/// rights reserved.
///
/// ================================================================

#include "common/config_iam_machine.h"

#include <isoft/ara_fsh/filesystem_hierarchy.h>
#include <isoft/manifestreader/manifest_node.h>
#include <isoft/manifestreader/manifest_reader.h>
#include <isoft/manifestreader/tps_enumeration.h>

#include <functional>

#include "common/ara_common.h"
#include "common/ara_log.h"
#include "common/common_api.h"

namespace {
//********************************/
/// @brief Get configuration file tag string constant:
/// trustedPlatformExecutableLaunchBehavior
/// @return processname
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_sr=SR_IAM_
/// @needwork = dd
/// @endcode
inline constexpr ara::iam::internal::char8_t const *GetJsonKey_TrustedPlatformExecutableLaunchBehavior() noexcept
{
    return "trustedPlatformExecutableLaunchBehavior";
}
}  // namespace
//********************************/
namespace ara {
namespace iam {
namespace internal {
namespace common {
//********************************/
/// @brief Load Com configuration data
/// @return load success/falied.
/// @code{.isoft}
/// @interface_level=unit
/// @needwork = ad
/// @endcode
bool PConfigIam_Machine::Initialize() noexcept
{
    _ResetData();
    _ClearReady();
    SetErrorCode(ara::iam::internal::grant::GrantSerializationErrc::kInitCode);
    return true;
}
/// @brief Clear  All GrantInfo
/// @returns true
bool PConfigIam_Machine::Deinitialize() noexcept
{
    _ResetData();
    _ClearReady();
    SetErrorCode(ara::iam::internal::grant::GrantSerializationErrc::kInitCode);
    return true;
}
/// @brief Initialize configuration file
/// @param stFileName Configuration file
/// @return true if has init manifest sucess false otherwise
/// @code{.isoft}
/// @interface_level=unit
/// @needwork = dda
/// @endcode
bool PConfigIam_Machine::InitManifest(ara::core::StringView const &stFileName) noexcept
{
    ara::core::Result< std::unique_ptr< isoft::manifestreader::Manifest > > manifestRes{
        isoft::manifestreader::OpenManifest(stFileName)};
    if (!manifestRes.HasValue()) {
        SetErrorCode(ara::iam::internal::grant::GrantSerializationErrc::kFileNotFound);
        return false;
    }
    std::unique_ptr< isoft::manifestreader::Manifest > const pManifestRes{std::move(manifestRes).Value()};
    common::IamLogger().LogInfo() << "PConfigIam_Machine::LoadData ConfigName = " << stFileName.data();

    ara::core::String stTrustPlatform;
    if (isoft::kSuccess != pManifestRes->Load(GetJsonKey_TrustedPlatformExecutableLaunchBehavior(), stTrustPlatform)) {
        common::IamLogger().LogWarn() << "strTrustPlatform is null!";
        SetErrorCode(ara::iam::internal::grant::GrantSerializationErrc::kJsonLoadFailed);
        return false;
    }

    isoft::manifestreader::tps::TrustedPlatformExecutableLaunchBehaviorEnum eTrustPlatform{
        isoft::manifestreader::tps::TrustedPlatformExecutableLaunchBehaviorEnum::kNoTrustedPlatformSupport};

    if (true != isoft::manifestreader::tps::FromString(stTrustPlatform, eTrustPlatform)) {
        common::IamLogger().LogError() << "lack of {" << GetJsonKey_TrustedPlatformExecutableLaunchBehavior()
                                       << "}, using default kNoTrustedPlatformSupport";
    }
    common::IamLogger().LogInfo() << "get eTrustPlatform is " << stTrustPlatform;
    eTrustedPlatformExecutableLaunchBehavior_ = eTrustPlatform;
    return true;
}
//********************************/
/// @brief Reset data
void PConfigIam_Machine::_ResetData()
{
    eTrustedPlatformExecutableLaunchBehavior_
        = isoft::manifestreader::tps::TrustedPlatformExecutableLaunchBehaviorEnum::kNoTrustedPlatformSupport;
}
//********************************/
bool InitMachineManifest()
{
    // load machine manifest.
#if defined(ARA_IAM_DEBUG_WITHOUT_EM)
    ara::core::StringView const &stManifestPathMachine{GetConfigFileName_Machine()};
#else
    common::PAraCommon const cmDir;

    EFileDirectoryType const fileType{EFileDirectoryType::kMachineFileDirectory};
    ara::core::String configFileName{cmDir.GetIamConfigDir(fileType)};
    ara::core::StringView const &stManifestPathMachine{configFileName};
    common::IamLogger().LogInfo() << "com load file:" << stManifestPathMachine;
#endif
    PConfigIam_Machine::Uptr pConfigMachine{MakeSinglePtr< PConfigIam_Machine >()};
    if (false == pConfigMachine->IsReady()) {
        if (false == pConfigMachine->InitManifest(stManifestPathMachine)) {
            return false;
        }
    }
    return true;
}
}  // namespace common
}  // namespace internal
}  // namespace iam
}  // namespace ara
