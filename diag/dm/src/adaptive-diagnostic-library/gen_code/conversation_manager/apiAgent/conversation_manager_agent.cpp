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
namespace dic {

ConversationManagerAgent::ConversationManagerAgent(uint16_t const &instanceId, uint32_t const &serviceInstanceId)
{
    proxy_ = std::make_unique< ConversationManagerProxy >(instanceId, serviceInstanceId);
}

void ConversationManagerAgent::RegisterUpdateActiveStatusCallback(
    std::function< void(std::uint8_t, std::uint8_t) > callback) noexcept
{
    proxy_->RegisterUpdateActiveStatusCallback(std::move(callback));
}

void ConversationManagerAgent::RegisterUpdateSessionIdCallback(std::function< void(std::uint8_t) > callback) noexcept
{
    proxy_->RegisterUpdateSessionIdCallback(std::move(callback));
}

void ConversationManagerAgent::RegisterUpdateSecurityLevelIdCallback(
    std::function< void(std::uint8_t) > callback) noexcept
{
    proxy_->RegisterUpdateSecurityLevelIdCallback(std::move(callback));
}

ara::core::Result< std::uint8_t > ConversationManagerAgent::GetConversationNumber() noexcept
{
    return proxy_->GetConversationNumber();
}

ara::core::Result< std::uint8_t > ConversationManagerAgent::GetActiveStatus(std::uint8_t index) noexcept
{
    return proxy_->GetActiveStatus(index);
}

ara::core::Result< std::uint8_t > ConversationManagerAgent::GetSessionId() noexcept { return proxy_->GetSessionId(); }

ara::core::Result< std::uint8_t > ConversationManagerAgent::GetSecurityLevelId() noexcept
{
    return proxy_->GetSecurityLevelId();
}

ara::core::Result< std::uint64_t > ConversationManagerAgent::GetIdentifier(std::uint8_t const index) noexcept
{
    return proxy_->GetIdentifier(index);
}

/// @brief Get diagnostic Session name
/// @param[in] index Conversation index value
/// @param[in] session Session value
/// @return Session name
/// @throws on overflow
ara::core::Result< ara::core::String > ConversationManagerAgent::GetDiagnosticSessionShortName(
    std::uint8_t const session)
{
    return proxy_->GetDiagnosticSessionShortName(session);
}

/// @brief Get diagnostic SecurityLevel name
/// @param[in] index Conversation index value
/// @param[in] securityLevel Security level
/// @return SecurityLevel name
/// @throws on overflow
ara::core::Result< ara::core::String > ConversationManagerAgent::GetDiagnosticSecurityLevelShortName(
    std::uint8_t const securityLevel)
{
    return proxy_->GetDiagnosticSecurityLevelShortName(securityLevel);
}

/// @brief Set to default Session
/// @param[in] identifier Session identifier
/// @return Setting result
/// @throws on overflow
ara::core::Result< void > ConversationManagerAgent::ResetToDefaultSession() const
{
    return proxy_->ResetToDefaultSession();
}

/// @brief Register the callback function when the network service is Ready, the callback will be called according to the current network status only after calling Start to start
/// @return callback Callback function when the network service is Ready
/// @throw unknown
void ConversationManagerAgent::RegisterOnReady(FuncOnReadyCallBack const &callback) noexcept
{
    return proxy_->RegisterOnReady(std::move(callback));
}
}  // namespace dic
}  // namespace dm
}  // namespace isoft