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
/// @file       client_authentication_handle.cpp
/// @brief
/// @details
/// @date       2025-09-12
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================

#include <ara/diag/client_authentication_handle.h>

#include "client_authentication_handle_impl.h"
namespace ara {
namespace diag {
ClientAuthenticationHandle::ClientAuthenticationHandle(
    std::shared_ptr< isoft::dm::ClientAuthenticationHandleImpl > implPtr) noexcept
    : implPtr_{std::move(implPtr)}
{
}
/**
 * @brief Constructor of ClientAuthenticationHandle
 *
 * @artraceid{SWS_DM_01146}@artracestatus{draft}
 * @aruptrace{RS_Diag_04251}
 */
ClientAuthenticationHandle::ClientAuthenticationHandle() {}  /// NOLINT

/**
 * @brief This function is used by the application to append a
 * DynamicAccessList to the already existing DynamicAccessList of a Diagnostic
 * Conversation
 *
 * @returns void
 *
 * @param[in] dynamicAccessList The DynamicAccessList to be appended in the
 * client
 * @arerror ara::diag::DiagErrc::kServiceNotAvailable
 *
 * @artraceid{SWS_DM_01152}@artracestatus{draft}
 * @aruptrace{RS_Diag_04251}
 */
ara::core::Result< void > ClientAuthenticationHandle::Append(
    DiagnosticServiceDynamicAccessList dynamicAccessList) noexcept
{
    return implPtr_->Append(std::move(dynamicAccessList));
}

/**
 * @brief This function is used by the application to set/replace a
 * DynamicAccessList  of  a diagnostic conversation
 *
 * @returns void
 *
 * @param[in] dynamicAccessList The new DynamicAccessList to be set in the
 * client
 * @arerror ara::diag::DiagErrc::kServiceNotAvailable
 *
 * @artraceid{SWS_DM_01153}@artracestatus{draft}
 * @aruptrace{RS_Diag_04251}
 */
ara::core::Result< void > ClientAuthenticationHandle::Set(DiagnosticServiceDynamicAccessList dynamicAccessList) noexcept
{
    return implPtr_->Set(std::move(dynamicAccessList));
}

/**
 * @brief This function is used by the application to de-authenticate a
 * client, and also to clear the DynamicAccessList and any overridden defaults
 *
 * @returns void
 * @arerror ara::diag::DiagErrc::kServiceNotAvailable
 *
 * @artraceid{SWS_DM_01154}@artracestatus{draft}
 * @aruptrace{RS_Diag_04251}
 */
ara::core::Result< void > ClientAuthenticationHandle::Revoke() noexcept { return implPtr_->Revoke(); }

/**
 * @brief This function is used by the application to refresh the timer that
 * was started by Authenticate or OverrideDefaultRoles. If both Methods were
 * previously called, both timers are refreshed
 *
 * @returns void
 * @arerror ara::diag::DiagErrc::kServiceNotAvailable
 *
 * @artraceid{SWS_DM_01155}@artracestatus{draft}
 * @aruptrace{RS_Diag_04251}
 */
ara::core::Result< void > ClientAuthenticationHandle::Refresh() noexcept { return implPtr_->Refresh(); }
}  // namespace diag
}  // namespace ara
