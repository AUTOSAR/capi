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
/// @file       grant_check_phm.cpp
/// @brief      IAM-PHM module logic processing function
/// @details
/// @date       2025-04-14
/// @author     Han Yuxin
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification Log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author       <th>Description
/// <tr><td>2025-04-14 <td>1.0.0 <td>Han Yuxin <td>Refactored based on
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/IAM/IAM-PHM
/// @interface_level=module
/// @trace_id_sr=SR_IAM_00301
/// @unit_name=IAM_PHM
/// @unit_description=Check verification provided by IAM for the PHM module
/// @endcode
///
/// ================================================================
///
/// rights reserved.
///
/// ================================================================

#include "grant_check_phm.h"

#include <map>
#include <set>

#include "ara/iam/internal/grant/serialization_error_domain.h"
#include "common/ara_common.h"
#include "common/ara_log.h"

namespace ara {
namespace iam {
namespace internal {
namespace phm {
//********************************/
/// @brief Load Crypto configuration data
/// @brief crypto init  load  data .

/// @return load success/falied.
/// @code{.isoft}
/// @interface_level=unit
/// @needwork = ad
/// @endcode
bool PGrantCheck_Phm::Initialize() noexcept
{
    if (iamConfigPhm_.IsReady()) {
        return true;
    }
    if (false == iamConfigPhm_.Initialize()) {
        return false;
    }
    // load machine manifest.
#if defined(ARA_IAM_DEBUG_WITHOUT_EM)
    ara::core::StringView const &stManifestPath{GetConfigFileName_IamPhm()};
#else
    common::PAraCommon const cmDir;

    common::EFileDirectoryType const fileType{common::EFileDirectoryType::kPhmFileDirectory};
    ara::core::String configFileName{cmDir.GetIamConfigDir(fileType)};
    ara::core::StringView const &stManifestPath{configFileName};
    common::IamLogger().LogInfo() << "phm load file:" << stManifestPath;
#endif
    return iamConfigPhm_.InitManifest(stManifestPath);
}
/// @brief Clear  All GrantInfo
/// @returns true
/// @code{.isoft}
/// @interface_level=unit
/// @needwork = ad
/// @endcode
bool PGrantCheck_Phm::Deinitialize() noexcept { return iamConfigPhm_.Deinitialize(); }
/// @brief Whether initialization is successful
/// @return true if has init manifest sucess false otherwise
bool PGrantCheck_Phm::IsReady() const noexcept { return iamConfigPhm_.IsReady(); }
/// @brief grant checkpoint check.
/// @param stProcess  checkpoint fqn
/// @param nFindID checkpoint id
/// @return checkpoint check success or failed.
bool PGrantCheck_Phm::GrantCheck_CheckPoint(ara::core::String const &stProcess, uint32_t const &nFindID) noexcept
{
    return iamConfigPhm_.GrantCheck_CheckPoint(stProcess, nFindID);
}
/// @brief grant ChannelGrantCheck.
/// @param stProcess channel fqn
/// @param nFindID channel id
/// @return Channel check success or failed.
bool PGrantCheck_Phm::GrantCheck_Channel(ara::core::String const &stProcess, uint32_t const &nFindID) noexcept
{
    return iamConfigPhm_.GrantCheck_Channel(stProcess, nFindID);
}
//********************************/
}  // namespace phm
}  // namespace internal
}  // namespace iam
}  // namespace ara
