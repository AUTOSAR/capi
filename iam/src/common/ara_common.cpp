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
/// @file       ara_common.cpp
/// @brief      log
/// @details
/// @date       2024-05-13
/// @author     jzy
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification Log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author       <th>Description
/// <tr><td> <td> <td> <td>Created initial version
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/IAM-COMMON
/// @interface_level=software
/// @trace_id_sr=SR_IAM_00501
/// @unit_name=IAM_Common
/// @unit_description=Provided by IAM
/// @endcode
///
/// ================================================================
///
/// rights reserved.
///
/// ================================================================

#include "common/ara_common.h"
#ifdef ARA_WITH_EXEC
    #include <ara/exec/internal/find_process_client.h>
#endif
#include <isoft/ara_fsh/filesystem_hierarchy.h>

#include "common/ara_log.h"
namespace ara {
namespace iam {
namespace internal {
namespace common {
//********************************/
#ifdef ARA_WITH_EXEC
/// @brief pid to fqn
/// @param pid  pid
/// @param proName  fqn value.
/// @return true/false
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_IAM_00000
/// @trace_id_dd=DD_IAM_00424
/// @needwork = dda
/// @endcode
ara::core::String PAraCommon::PidTranslate(
    uint32_t const &pid,
    isoft::manifestreader::tps::TrustedPlatformExecutableLaunchBehaviorEnum const eTrustPlatform) const noexcept
{
    ara::core::String stProcessName;
    if (true == fpc_.FindByPid(pid, stProcessName).HasValue()) {
        return stProcessName;
    }
    common::IamLogger().LogInfo() << "common get eTrustPlatform  " << static_cast< uint32_t >(eTrustPlatform);
    // exec strict verification mode, i.e. actual vehicle mode. All applications
    // must be launched via exec.
    if (eTrustPlatform == isoft::manifestreader::tps::TrustedPlatformExecutableLaunchBehaviorEnum::kStrictMode) {
        common::IamLogger().LogDebug() << "get FQN true, this mode is strict  mode!";
        stProcessName.clear();
        return stProcessName;
    }
    common::IamLogger().LogWarn() << "get FQN false, this mode is  non credit  mode!";
    stProcessName = "noncreditapp";
    return stProcessName;
}
#endif

/// @brief get all   iam configuation  dir.
/// @param fileDir file dirctory
/// @param type  file type
/// @return Configuration file name
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_IAM_00000
/// @trace_id_dd=DD_IAM_00425
/// @needwork = dda
/// @endcode
ara::core::String PAraCommon::GetIamConfigDir(EFileDirectoryType const &type) const noexcept
{
    ara::core::String fileDirectory{cmPlatform_.GetPlatformEtcDir().c_str()};  // NOLINT
    if (fileDirectory.empty()) {
        common::IamLogger().LogWarn() << "ara::iam Get Ap machine directory is empty!" << fileDirectory.c_str();
        return ara::core::String{""};
    }
    switch (type) {
        case EFileDirectoryType::kComFileDirectory: {
            fileDirectory += "/iam_com.json";
        } break;
        case EFileDirectoryType::kCryptoFileDirectory: {
            fileDirectory += "/iam_crypto.json";
        } break;
        case EFileDirectoryType::kPhmFileDirectory: {
            fileDirectory += "/iam_phm.json";
        } break;
        case EFileDirectoryType::kIdsmFileDirectory: {
            fileDirectory += "/iam_idsm.json";
        } break;
        case EFileDirectoryType::kRawFileDirectory: {
            fileDirectory += "/iam_raw.json";
        } break;
        case EFileDirectoryType::kMachineFileDirectory: {
            fileDirectory += "/machine_manifest.json";
        } break;
        default: {
            break;
        }
    }
    return fileDirectory;
}
//********************************/
}  // namespace common
}  // namespace internal
}  // namespace iam
}  // namespace ara
