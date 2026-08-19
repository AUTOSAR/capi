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
/// @file       conversation_manager_agent.cpp
/// @brief
/// @details
/// @date       2024-12-18
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================

#include "conversation_manager_agent.h"

namespace isoft {
namespace dm {
namespace dis {

ConversationManagerAgent::ConversationManagerAgent(uint32_t const &serviceInstanceId)
{
    proxy_ = std::make_unique< ConversationManagerProxy >(serviceInstanceId);
}

void ConversationManagerAgent::RegisterGetConversationNumberCallback(std::function< std::uint8_t() > callback) noexcept
{
    proxy_->RegisterGetConversationNumberCallback(std::move(callback));
}

void ConversationManagerAgent::RegisterGetActiveStatusCallback(
    std::function< std::uint8_t(std::uint8_t) > callback) noexcept
{
    proxy_->RegisterGetActiveStatusCallback(std::move(callback));
}

void ConversationManagerAgent::RegisterGetSessionIdCallback(std::function< std::uint8_t() > callback) noexcept
{
    proxy_->RegisterGetSessionIdCallback(std::move(callback));
}

void ConversationManagerAgent::RegisterGetSecurityLevelIdCallback(std::function< std::uint8_t() > callback) noexcept
{
    proxy_->RegisterGetSecurityLevelIdCallback(std::move(callback));
}

void ConversationManagerAgent::RegisterGetIdentifierCallback(
    std::function< std::uint64_t(std::uint8_t) > callback) noexcept
{
    proxy_->RegisterGetIdentifierCallback(std::move(callback));
}

void ConversationManagerAgent::RegisterGetDiagnosticSessionShortNameCallback(
    std::function< ara::core::StringView(std::uint8_t) > callback) noexcept
{
    proxy_->RegisterGetDiagnosticSessionShortNameCallback(std::move(callback));
}

void ConversationManagerAgent::RegisterGetDiagnosticSecurityLevelShortNameCallback(
    std::function< ara::core::StringView(std::uint8_t) > callback) noexcept
{
    proxy_->RegisterGetDiagnosticSecurityLevelShortNameCallback(std::move(callback));
}

void ConversationManagerAgent::RegisterResetToDefaultSessionCallback(std::function< void() > callback) noexcept
{
    proxy_->RegisterResetToDefaultSessionCallback(std::move(callback));
}

void ConversationManagerAgent::UpdateActiveState(std::uint8_t index, std::uint8_t activeStatus) noexcept
{
    proxy_->UpdateActiveState(index, activeStatus);
}

void ConversationManagerAgent::UpdateSessionId(std::uint8_t sessionId) noexcept { proxy_->UpdateSessionId(sessionId); }

void ConversationManagerAgent::UpdateSecurityLevelId(std::uint8_t securityLevelId) noexcept
{
    proxy_->UpdateSecurityLevelId(securityLevelId);
}
}  // namespace dis
}  // namespace dm
}  // namespace isoft