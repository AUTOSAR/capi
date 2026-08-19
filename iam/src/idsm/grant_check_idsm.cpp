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
/// @file       grant_check_idsm.cpp
/// @brief      IAM-crypto module logic processing function
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
/// @module_path=/IAM/IAM-CRYPTO
/// @interface_level=module
/// @trace_id_sr=SR_IAM_00401
/// @unit_name=IAM_CRYPTO
/// @unit_description=Check verification provided by IAM for the IDSM module
/// @endcode
///
/// ================================================================
///
/// rights reserved.
///
/// ================================================================

#include "grant_check_idsm.h"

#include <map>
#include <set>

#include "ara/iam/internal/grant/serialization_error_domain.h"
#include "common/ara_common.h"
#include "common/ara_log.h"

namespace ara {
namespace iam {
namespace internal {
namespace idsm {

/// @brief Load IDSM configuration data
/// @brief crypto init  load  data .

/// @return load success/falied.
/// @code{.isoft}
/// @interface_level=unit
/// @needwork = ad
/// @endcode
bool PGrantCheck_Idsm::Initialize() noexcept
{
    if (iamConfigIdsm_.IsReady()) {
        return true;
    }
    if (false == iamConfigIdsm_.Initialize()) {
        return false;
    }
    // load machine manifest.
#if defined(ARA_IAM_DEBUG_WITHOUT_EM)
    ara::core::StringView const &stManifestPath{GetConfigFileName_IamIdsm()};
#else
    common::PAraCommon const cmDir;

    common::EFileDirectoryType const fileType{common::EFileDirectoryType::kIdsmFileDirectory};
    ara::core::String configFileName{cmDir.GetIamConfigDir(fileType)};
    ara::core::StringView const &stManifestPath{configFileName};
    common::IamLogger().LogInfo() << "crypto load file:" << stManifestPath;
#endif
    return iamConfigIdsm_.InitManifest(stManifestPath);
}
/// @brief Clear  All GrantInfo
/// @returns true
/// @code{.isoft}
/// @interface_level=unit
/// @needwork = ad
/// @endcode
bool PGrantCheck_Idsm::Deinitialize() noexcept { return iamConfigIdsm_.Deinitialize(); }
/// @brief Whether initialization is successful
/// @return true if has init manifest sucess false otherwise
bool PGrantCheck_Idsm::IsReady() const noexcept { return iamConfigIdsm_.IsReady(); }

/// @brief grant crypto check.
/// @param stProcess process fqn
/// @param nID Security event ID
/// @return crypto check success or failed.
bool PGrantCheck_Idsm::GrantCheck(ara::core::String const &stProcess, uint32_t const &nID) noexcept
{
    return iamConfigIdsm_.GrantCheck(stProcess, nID);
}

}  // namespace idsm
}  // namespace internal
}  // namespace iam
}  // namespace ara
