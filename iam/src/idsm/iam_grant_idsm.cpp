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
/// @file       iam_grant_idsm.cpp
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
/// @module_path=/IAM/IAM-IDSM
/// @interface_level=software
/// @trace_id_sr=SR_IAM_00401
/// @unit_name=IAM_IDSM
/// @unit_description=Verification interface provided by IAM for the IDSM module
/// @endcode
///
/// ================================================================
///
/// rights reserved.
///
/// ================================================================

#include "ara/iam/internal/idsm/iam_grant_idsm.h"

#include "ara/iam/internal/grant/serialization_error_domain.h"
#include "common/ara_common.h"
#include "common/ara_log.h"
#include "common/common_api.h"
#include "grant_check_idsm.h"
namespace ara {
namespace iam {
namespace internal {
namespace idsm {

/// @brief Initialize
/// @return true/false
bool PIamGrant_Idsm::Initialize() noexcept
{
    PGrantCheck_Idsm::Uptr pGrantCheck = common::MakeSinglePtr< PGrantCheck_Idsm >();
    if (!pGrantCheck) {
        common::IamLogger().LogInfo() << "PGrantCheck_Idsm Is Nullptr Point!";
        return false;
    }
    if (pGrantCheck->IsReady()) {
        common::IamLogger().LogInfo() << "PGrantCheck_Idsm Is Ready!";
        return true;
    }
    bool bReturn{pGrantCheck->Initialize()};
    if (false == bReturn) {
        common::IamLogger().LogInfo() << "PGrantCheck_Idsm Loaded Field !";
    } else {
        common::IamLogger().LogInfo() << "PGrantCheck_Idsm Loaded Success !";
    }

    return bReturn;
}
/// @brief Clear  All GrantInfo
/// @returns Deinitialization bool result
bool PIamGrant_Idsm::Deinitialize() noexcept
{
    PGrantCheck_Idsm::Uptr pGrantCheck = common::MakeSinglePtr< PGrantCheck_Idsm >();
    if (!pGrantCheck) {
        common::IamLogger().LogInfo() << "PGrantCheck_Idsm Is Nullptr Point!";
        return false;
    }
    return pGrantCheck->Deinitialize();
}

/// @brief Checks if the given grant exists
/// @param stProcess idsm client pid
/// @param nID Security event ID
/// @returns bool.
/// @code{.isoft}
/// @interface_level=unit
/// @needwork = ad
/// @endcode
bool PIamGrant_Idsm::HasIdsmGrant(ara::core::String const &stProcess,
                                  uint32_t const &nID) noexcept  // NOLINT
{
#ifdef ARA_WITH_EXEC
    if (stProcess.empty()) {
        common::IamLogger().LogError() << "Idsm check failed, pid-->fqn check failed.";
        return false;
    }
    PGrantCheck_Idsm::Uptr pGrantCheck = common::MakeSinglePtr< PGrantCheck_Idsm >();
    if (!pGrantCheck) {
        common::IamLogger().LogInfo() << "PGrantCheck_Idsm Is Nullptr Point!";
        return false;
    }
    if (false == pGrantCheck->IsReady()) {
        common::IamLogger().LogInfo() << "PGrantCheck_Idsm Is Not Ready!";
        return false;
    }
    common::IamLogger().LogDebug() << "Get Idsm ProcessName is " << stProcess.c_str();
    return pGrantCheck->GrantCheck(stProcess, nID);
#else
    return true;
#endif
}
/// @brief  Checks if the given grant exists
/// @param nPid Process ID
/// @param nID Security event ID
/// @return true/false
bool PIamGrant_Idsm::HasIdsmGrant(uint32_t const &nPid, uint32_t const &nID) noexcept
{
    return HasIdsmGrant(common::Pid2Name(nPid), nID);
}

}  // namespace idsm
}  // namespace internal
}  // namespace iam
}  // namespace ara
