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
/// @file       grant_check_raw.cpp
/// @brief      IAM-com module logic processing function
/// @details
/// @date       2025-04-14
/// @author     Han Yuxin
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification Log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author     <th>Description
/// <tr><td>2025-04-14 <td>1.0.0 <td>Han Yuxin <td>Refactored based on
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/IAM/IAM-COM
/// @interface_level=module
/// @trace_id_sr=SR_IAM_00201
/// @unit_name=IAM_COM
/// @unit_description=Check verification provided by IAM for the Com module: Raw
/// @endcode
///
/// ================================================================
///
/// rights reserved.
///
/// ================================================================

#include "grant_check_raw.h"

#include <map>
#include <set>

#include "ara/iam/internal/grant/serialization_error_domain.h"
#include "common/ara_common.h"
#include "common/ara_log.h"

namespace ara {
namespace iam {
namespace internal {
namespace com {
//********************************/
/// @brief Load Raw configuration data
/// @brief com init  load  data .
/// @return load success/falied.
bool PGrantCheck_Raw::Initialize() noexcept
{
    if (iamConfigRaw_.IsReady()) {
        return true;
    }
    if (false == iamConfigRaw_.Initialize()) {
        return false;
    }
    // load machine manifest.
#if defined(ARA_IAM_DEBUG_WITHOUT_EM)
    ara::core::StringView const &stManifestPath{GetConfigFileName_IamRaw()};
#else
    common::PAraCommon const cmDir;
    common::EFileDirectoryType const fileType{common::EFileDirectoryType::kRawFileDirectory};
    ara::core::String configFileName{cmDir.GetIamConfigDir(fileType)};
    ara::core::StringView const &stManifestPath{configFileName};
    common::IamLogger().LogInfo() << "com load file:" << stManifestPath;
#endif
    return iamConfigRaw_.InitManifest(stManifestPath);
}
/// @brief Clear  All GrantInfo
/// @returns true
bool PGrantCheck_Raw::Deinitialize() noexcept { return iamConfigRaw_.Deinitialize(); }
/// @brief Whether initialization is successful
/// @return true if has init manifest sucess false otherwise
bool PGrantCheck_Raw::IsReady() const noexcept { return iamConfigRaw_.IsReady(); }
/// @brief Return error code
/// @return Error code
/// @interface_level=unit
/// @needwork = dda
/// @endcode
ara::iam::internal::grant::GrantSerializationErrc PGrantCheck_Raw::GetErrorCode() const noexcept
{
    return iamConfigRaw_.GetErrorCode();
}
//********************************/
/// @brief grant Event check.
/// @param grantObject event struct
/// @return  event check success or failed.
bool PGrantCheck_Raw::GrantCheck_RawDataStream(PIamConfigData_RawDataStreamGrant const &grantObject) const noexcept
{
    iamConfigRaw_.SetErrorCode(ara::iam::internal::grant::GrantSerializationErrc::kInitCode);
    return iamConfigRaw_.GrantCheck_RawDataStream(grantObject);
}
//********************************/
}  // namespace com
}  // namespace internal
}  // namespace iam
}  // namespace ara
