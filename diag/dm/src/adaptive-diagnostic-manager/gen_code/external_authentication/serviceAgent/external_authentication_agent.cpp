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
/// @file       external_authentication_agent.cpp
/// @brief
/// @details
/// @date       2025-09-12
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================

#include "external_authentication_agent.h"

namespace isoft {
namespace dm {
namespace dis {

ExternalAuthenticationAgent::ExternalAuthenticationAgent(uint32_t const &serviceInstanceId)
{
    proxy_ = std::make_unique< ExternalAuthenticationProxy >(serviceInstanceId);
}

void ExternalAuthenticationAgent::RegisterHasClientAddressCallback(HasClientAddressCallback callback) noexcept
{
    return proxy_->RegisterHasClientAddressCallback(std::move(callback));
}
void ExternalAuthenticationAgent::RegisterSetAuthStateCallback(SetAuthStateCallback callback) noexcept
{
    return proxy_->RegisterSetAuthStateCallback(std::move(callback));
}

void ExternalAuthenticationAgent::RegisterAddDynamicAccessListCallback(AddDynamicAccessListCallback callback) noexcept
{
    return proxy_->RegisterAddDynamicAccessListCallback(std::move(callback));
}
void ExternalAuthenticationAgent::RegisterSetDynamicAccessListCallback(SetDynamicAccessListCallback callback) noexcept
{
    return proxy_->RegisterSetDynamicAccessListCallback(std::move(callback));
}
void ExternalAuthenticationAgent::RegisterRevokeCallback(RevokeCallback callback) noexcept
{
    return proxy_->RegisterRevokeCallback(std::move(callback));
}
void ExternalAuthenticationAgent::RegisterRefreshCallback(RefreshCallback callback) noexcept
{
    return proxy_->RegisterRefreshCallback(std::move(callback));
}
void ExternalAuthenticationAgent::RegisterGetAllAddressCallback(GetAllAddressCallback callback) noexcept
{
    return proxy_->RegisterGetAllAddressCallback(std::move(callback));
}
void ExternalAuthenticationAgent::UpdateAuthState(std::uint16_t address, bool isAuth) noexcept
{
    return proxy_->UpdateAuthState(address, isAuth);
}
}  // namespace dis
}  // namespace dm
}  // namespace isoft