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
/// @file       iam_grant_crypto.cpp
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
/// @module_path=/IAM/IAM-CRYPTO
/// @interface_level=software
/// @trace_id_sr=SR_IAM_00201
/// @unit_name=IAM_CRYPTO
/// @unit_description=Verification interface provided by IAM for the CRYPTO
/// module
/// @endcode
///
/// ================================================================
///
/// rights reserved.
///
/// ================================================================

#include "ara/iam/internal/crypto/iam_grant_crypto.h"

#include "ara/iam/internal/grant/serialization_error_domain.h"
#include "common/ara_common.h"
#include "common/ara_log.h"
#include "common/common_api.h"
#include "grant_check_crypto.h"
namespace ara {
namespace iam {
namespace internal {
namespace crypto {

/// @brief Initialize
/// @return true/false
bool PIamGrant_Crypto::Initialize() noexcept
{
    PGrantCheck_Crypto::Uptr pGrantCheck = common::MakeSinglePtr< PGrantCheck_Crypto >();
    if (!pGrantCheck) {
        common::IamLogger().LogInfo() << "PGrantCheck_Crypto Is Nullptr Point!";
        return false;
    }
    if (pGrantCheck->IsReady()) {
        common::IamLogger().LogInfo() << "PGrantCheck_Crypto Is Ready!";
        return true;
    }
    bool bReturn{pGrantCheck->Initialize()};
    if (false == bReturn) {
        common::IamLogger().LogInfo() << "PGrantCheck_Crypto Loaded Field !";
    } else {
        common::IamLogger().LogInfo() << "PGrantCheck_Crypto Loaded Success !";
    }

    return bReturn;
}
/// @brief Clear  All GrantInfo
/// @returns Deinitialization bool result
bool PIamGrant_Crypto::Deinitialize() noexcept
{
    PGrantCheck_Crypto::Uptr pGrantCheck = common::MakeSinglePtr< PGrantCheck_Crypto >();
    if (!pGrantCheck) {
        common::IamLogger().LogInfo() << "PGrantCheck_Crypto Is Nullptr Point!";
        return false;
    }
    return pGrantCheck->Deinitialize();
}

/// @brief Checks if the given grant exists
/// @param stProcess crypto client pid
/// @param slotname slot key name.
/// @returns bool.
/// @code{.isoft}
/// @interface_level=unit
/// @needwork = ad
/// @endcode
bool PIamGrant_Crypto::HasCryptoGrant(ara::core::String const &stProcess,          // NOLINT
                                      ara::core::String const &slotname) noexcept  // NOLINT
{
#ifdef ARA_WITH_EXEC
    if (stProcess.empty()) {
        common::IamLogger().LogError() << "Crypto check failed, pid-->fqn check failed.";
        return false;
    }
    PGrantCheck_Crypto::Uptr pGrantCheck = common::MakeSinglePtr< PGrantCheck_Crypto >();
    if (!pGrantCheck) {
        common::IamLogger().LogInfo() << "PGrantCheck_Crypto Is Nullptr Point!";
        return false;
    }
    if (false == pGrantCheck->IsReady()) {
        common::IamLogger().LogInfo() << "PGrantCheck_Crypto Is Not Ready!";
        return false;
    }
    common::IamLogger().LogDebug() << "Get Crypto ProcessName is " << stProcess.c_str();
    return pGrantCheck->GrantCheck(stProcess, slotname);
#else
    return true;
#endif
}
/// @brief  Checks if the given grant exists
/// @param nPid
/// @param slotname
/// @return true/false
bool PIamGrant_Crypto::HasCryptoGrant(uint32_t const &nPid, ara::core::String const &slotname) noexcept
{
    return HasCryptoGrant(common::Pid2Name(nPid), slotname);
}

}  // namespace crypto
}  // namespace internal
}  // namespace iam
}  // namespace ara
