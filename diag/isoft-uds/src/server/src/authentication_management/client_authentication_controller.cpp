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
/// @file       client_authentication_controller.cpp
/// @brief      This file implements client_authentication_controller functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/isoft/uds/authentication_management
/// @trace_id_sr=f0cf48d7-67b9-4f4f-ac7c-7127d2aa02c8
/// @unit_name=ClientAuthenticationController
/// @unit_description=This file provides client_authentication_controller functionality
/// @interface_level= module
/// @endcode
///
/// ================================================================

#include <isoft/uds/authentication_management/client_authentication_controller.h>
#include <isoft/uds/uds_nrc_error_domain.h>
namespace isoft {
namespace uds {
namespace server {

Result< void > ClientAuthenticationController::OverrideDefaultRoles(std::vector< AuthRoleName >,         /// NOLINT
                                                                    std::chrono::milliseconds) noexcept  /// NOLINT
{
    return Result< void >::FromError(static_cast< std::int32_t >(NrcErrc::kGeneralReject));
}

Result< void > ClientAuthenticationController::Authenticate(std::vector< AuthRoleName >) noexcept  /// NOLINT
{
    return Result< void >::FromError(static_cast< std::int32_t >(NrcErrc::kGeneralReject));
}

Result< AuthState > ClientAuthenticationController::GetState() const noexcept
{
    return Result< AuthState >::FromError(static_cast< std::int32_t >(NrcErrc::kGeneralReject));
}

Result< void > ClientAuthenticationController::SetNotifier(std::function< void(AuthState) >) noexcept  /// NOLINT
{
    return Result< void >::FromError(static_cast< std::int32_t >(NrcErrc::kGeneralReject));
}

Result< void > ClientAuthenticationController::Append(ServiceDynamicAccessList) noexcept  /// NOLINT
{
    return Result< void >::FromError(static_cast< std::int32_t >(NrcErrc::kGeneralReject));
}

Result< void > ClientAuthenticationController::Set(ServiceDynamicAccessList) noexcept  /// NOLINT
{
    return Result< void >::FromError(static_cast< std::int32_t >(NrcErrc::kGeneralReject));
}

Result< void > ClientAuthenticationController::Revoke() noexcept
{
    return Result< void >::FromError(static_cast< std::int32_t >(NrcErrc::kGeneralReject));
}

Result< void > ClientAuthenticationController::Refresh() noexcept
{
    return Result< void >::FromError(static_cast< std::int32_t >(NrcErrc::kGeneralReject));
}

}  // namespace server
}  // namespace uds
}  // namespace isoft
