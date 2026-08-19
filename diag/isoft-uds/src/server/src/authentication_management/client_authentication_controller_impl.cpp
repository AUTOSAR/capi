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
/// @file       client_authentication_controller_impl.cpp
/// @brief      This file implements client_authentication_controller_impl functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=authentication_management
/// @trace_id_sr=682f72ca-02bd-4ab8-9a5c-1982f408c96d
/// @unit_name=ClientAuthenticationControllerImpl
/// @unit_description=This file provides client_authentication_controller_impl functionality
/// @interface_level=unit
/// @endcode
///
/// ================================================================

#include "client_authentication_controller_impl.h"
namespace isoft {
namespace uds {
namespace server {

ClientAuthenticationControllerImpl::ClientAuthenticationControllerImpl(
    std::shared_ptr< ClientAuthentication > clientAuthPtr)
    : clientAuthPtr_{std::move(clientAuthPtr)}
{
}

Result< void > ClientAuthenticationControllerImpl::OverrideDefaultRoles(std::vector< AuthRoleName > defaultRoles,
                                                                        std::chrono::milliseconds timeout) noexcept
{
    if (clientAuthPtr_.get() != nullptr) {
        clientAuthPtr_->OverrideDefaultState(defaultRoles, timeout);
        return {};
    }
    return Result< void >::FromError(static_cast< std::int32_t >(NrcErrc::kGeneralReject));
}

Result< void > ClientAuthenticationControllerImpl::Authenticate(std::vector< AuthRoleName > userRoles) noexcept
{
    if (clientAuthPtr_.get() != nullptr) {
        clientAuthPtr_->Authenticate(userRoles);
        return {};
    }
    return Result< void >::FromError(static_cast< std::int32_t >(NrcErrc::kGeneralReject));
}

Result< AuthState > ClientAuthenticationControllerImpl::GetState() const noexcept
{
    if (clientAuthPtr_.get() != nullptr) {
        AuthState state{clientAuthPtr_->GetState()};

        return Result< AuthState >::FromValue(state);
    }
    return Result< AuthState >::FromError(static_cast< std::int32_t >(NrcErrc::kGeneralReject));
}

Result< void > ClientAuthenticationControllerImpl::SetNotifier(std::function< void(AuthState) > notifier) noexcept
{
    if (clientAuthPtr_.get() != nullptr) {
        clientAuthPtr_->SetNotifyAuthState(notifier);

        return {};
    }
    return Result< void >::FromError(static_cast< std::int32_t >(NrcErrc::kGeneralReject));
}

Result< void > ClientAuthenticationControllerImpl::Append(ServiceDynamicAccessList list) noexcept
{
    if (clientAuthPtr_.get() != nullptr) {
        clientAuthPtr_->AppendServiceDynamicAccessList(list);

        return {};
    }
    return Result< void >::FromError(static_cast< std::int32_t >(NrcErrc::kGeneralReject));
}

Result< void > ClientAuthenticationControllerImpl::Set(ServiceDynamicAccessList list) noexcept
{
    if (clientAuthPtr_.get() != nullptr) {
        clientAuthPtr_->SetServiceDynamicAccessList(list);

        return {};
    }
    return Result< void >::FromError(static_cast< std::int32_t >(NrcErrc::kGeneralReject));
}

Result< void > ClientAuthenticationControllerImpl::Revoke() noexcept
{
    if (clientAuthPtr_.get() != nullptr) {
        clientAuthPtr_->Default();

        return {};
    }
    return Result< void >::FromError(static_cast< std::int32_t >(NrcErrc::kGeneralReject));
}

Result< void > ClientAuthenticationControllerImpl::Refresh() noexcept
{
    if (clientAuthPtr_.get() != nullptr) {
        clientAuthPtr_->Refresh();

        return {};
    }
    return Result< void >::FromError(static_cast< std::int32_t >(NrcErrc::kGeneralReject));
}
}  // namespace server
}  // namespace uds
}  // namespace isoft
