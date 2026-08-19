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
/// @file       iam_grant_phm.cpp
/// @brief      com  interface.
/// @details
/// @date       2022-08-19
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
/// @module_path=/IAM/IAM-PHM
/// @interface_level=software
/// @trace_id_sr=SR_IAM_00301
/// @unit_name=IAM_PHM
/// @unit_description=Verification interface provided by IAM for the PHM module
/// @endcode
///
/// ================================================================
///
/// rights reserved.
///
/// ================================================================

#include "ara/iam/internal/phm/iam_grant_phm.h"

#include "ara/iam/internal/grant/serialization_error_domain.h"
#include "common/ara_common.h"
#include "common/ara_log.h"
#include "common/common_api.h"
#include "grant_check_phm.h"
namespace ara {
namespace iam {
namespace internal {
namespace phm {
//********************************/
/// @brief Initialize
/// @return true/false
bool PIamGrant_Phm::Initialize() noexcept
{
    PGrantCheck_Phm::Uptr pGrantCheck = common::MakeSinglePtr< PGrantCheck_Phm >();
    if (!pGrantCheck) {
        common::IamLogger().LogInfo() << "PGrantCheck_Phm Is Nullptr Point!";
        return false;
    }
    if (pGrantCheck->IsReady()) {
        common::IamLogger().LogInfo() << "PGrantCheck_Phm Is Ready!";
        return true;
    }
    bool bReturn{pGrantCheck->Initialize()};
    if (false == bReturn) {
        common::IamLogger().LogInfo() << "PGrantCheck_Phm Loaded Field !";
    } else {
        common::IamLogger().LogInfo() << "PGrantCheck_Phm Loaded Success !";
    }

    return bReturn;
}

/// @brief Clear  All GrantInfo
/// @returns Deinitialization bool result
bool PIamGrant_Phm::Deinitialize() noexcept
{
    PGrantCheck_Phm::Uptr pGrantCheck = common::MakeSinglePtr< PGrantCheck_Phm >();
    if (!pGrantCheck) {
        common::IamLogger().LogInfo() << "PGrantCheck_Phm Is Nullptr Point!";
        return false;
    }
    return pGrantCheck->Deinitialize();
}
/// @brief phm checkpoint grant check.
/// @param stProcess phm client processname
/// @param checkPointId The ID of the checkpoint.
/// @return true/false
/// @code{.isoft}
/// @interface_level=unit
/// @needwork = ad
/// @endcode
bool PIamGrant_Phm::HasCheckPointGrant(ara::core::String const &stProcess,     // NOLINT
                                       uint32_t const &checkPointId) noexcept  // NOLINT
{
#ifdef ARA_WITH_EXEC
    if (stProcess.empty()) {
        common::IamLogger().LogError() << "Phm check failed, pid-->fqn check failed.";
        return false;
    }
    PGrantCheck_Phm::Uptr pGrantCheck = common::MakeSinglePtr< PGrantCheck_Phm >();
    if (!pGrantCheck) {
        common::IamLogger().LogInfo() << "PGrantCheck_Phm Is Nullptr Point!";
        return false;
    }
    if (false == pGrantCheck->IsReady()) {
        common::IamLogger().LogInfo() << "PGrantCheck_Phm Is Not Ready!";
        return false;
    }
    common::IamLogger().LogDebug() << "Get Phm ProcessName is " << stProcess.c_str();
    return pGrantCheck->GrantCheck_CheckPoint(stProcess, checkPointId);
#else
    return true;
#endif
}
/// @brief phm checkpoint grant check.
/// @param nPid phm client pid
/// @param checkPointId The ID of the checkpoint.
/// @return true/false
bool PIamGrant_Phm::HasCheckPointGrant(uint32_t const &nPid, uint32_t const &checkPointId) noexcept
{
    return HasCheckPointGrant(common::Pid2Name(nPid), checkPointId);
}

/// @brief phm channel grant check.
/// @param stProcess phm client processname
/// @param channelId channel id
/// @return true/false
/// @code{.isoft}
/// @interface_level=unit
/// @needwork = ad
/// @endcode
bool PIamGrant_Phm::HasChannelGrant(ara::core::String const &stProcess,
                                    uint32_t const &channelId) noexcept  // NOLINT
{
#ifdef ARA_WITH_EXEC
    if (stProcess.empty()) {
        common::IamLogger().LogError() << "Phm check failed, pid-->fqn check failed.";
        return false;
    }
    PGrantCheck_Phm::Uptr pGrantCheck = common::MakeSinglePtr< PGrantCheck_Phm >();
    if (!pGrantCheck) {
        common::IamLogger().LogInfo() << "PGrantCheck_Phm Is Nullptr Point!";
        return false;
    }
    if (false == pGrantCheck->IsReady()) {
        common::IamLogger().LogInfo() << "PGrantCheck_Phm Is Not Ready!";
        return false;
    }
    common::IamLogger().LogDebug() << "Get Phm ProcessName is " << stProcess.c_str();
    return pGrantCheck->GrantCheck_Channel(stProcess, channelId);
#else
    return true;
#endif
}
/// @brief phm channel grant check.
/// @param nPid phm client pid
/// @param channelId channel id
/// @return true/false
bool PIamGrant_Phm::HasChannelGrant(uint32_t const &nPid, uint32_t const &channelId) noexcept
{
    return HasChannelGrant(common::Pid2Name(nPid), channelId);
}
//********************************/
}  // namespace phm
}  // namespace internal
}  // namespace iam
}  // namespace ara
