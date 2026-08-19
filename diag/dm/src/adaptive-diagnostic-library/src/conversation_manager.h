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
/// @file       conversation_manager.h
/// @brief      This file provides the definitions of ConversationManager and related types.
/// @details
/// @date       2022-11-21
/// @author     gaohuiming
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_DIAG_INTERNAL_CONVERSATION_MANAGER_H_
#define ARA_DIAG_INTERNAL_CONVERSATION_MANAGER_H_

#include <ara/core/map.h>
#include <ara/core/result.h>
#include <ara/core/string.h>
#include <ara/core/vector.h>

#include <cstdint>
#include <functional>
#include <memory>
#include <mutex>

#include "ara/diag/cancellation_handler.h"
#include "ara/diag/conversation.h"
#include "ara/diag/meta_info.h"
#include "conversation_impl.h"
#include "gen_code/conversation_manager/apiAgent/conversation_manager_agent.h"

namespace ara {
namespace diag {
namespace internal {
/// @brief ConversationManager
class ConversationManager final
{
public:
    /// @brief Copy constructor is prohibited
    /// @param
    ConversationManager(ConversationManager const&) = delete;
    /// @brief Copy assignment operator is prohibited
    /// @param
    /// @return
    ConversationManager& operator=(ConversationManager const&) = delete;
    /// @brief Move constructor is prohibited
    /// @param
    ConversationManager(ConversationManager&&) = delete;
    /// @brief  Move assignment operator is prohibited
    /// @param
    /// @return
    ConversationManager& operator=(ConversationManager&&) = delete;

    /// @brief Default constructor
    ConversationManager() = default;

    /// @brief Default destructor
    ~ConversationManager();

    /// @brief ConversationManager initialization
    /// @throws on overflow
    void Initialize();

    /// @brief Get a conversation according to meta information
    /// @param[in] metaInfo Meta information
    /// @return Conversation object
    /// @throws on overflow
    api::ConversationImpl* GetConversationImpl(MetaInfo& metaInfo);

    /// @brief Get a conversation according to meta information
    /// @param[in] metaInfo Meta information
    /// @return Conversation object
    /// @throws on overflow
    ara::core::Result< Conversation& > GetConversation(MetaInfo& metaInfo);

    /// @brief Get all conversations
    /// @return Conversation list
    /// @throws on overflow
    ara::core::Vector< std::reference_wrapper< Conversation > > GetAllConversations();

    /// @brief Get currently active conversations
    /// @return Active conversation list
    /// @throws on overflow
    ara::core::Vector< std::reference_wrapper< Conversation > > GetCurrentActiveConversations();

    /// @brief Get diagnostic session name
    /// @param[in] session Session ID
    /// @return Session name
    /// @throws on overflow
    ara::core::Result< ara::core::String > GetDiagnosticSessionShortName(std::uint8_t session);

    /// @brief Get diagnostic security level name
    /// @param[in] securityLevel Security level
    /// @return Security level name
    /// @throws on overflow
    ara::core::Result< ara::core::String > GetDiagnosticSecurityLevelShortName(std::uint8_t securityLevel);

    /// @brief Switch to default session
    /// @param[in] identifier Conversation descriptor
    /// @return Operation result
    /// @throws on overflow
    ara::core::Result< void > ResetToDefaultSession();

private:
    /// @name proxy_
    std::shared_ptr< isoft::dm::dic::ConversationManagerAgent > agentPtr_;
    /// @name conversationTable_
    ara::core::Vector< api::ConversationImpl > conversationTable_;
    /// @name wrapperTableLock_
    std::mutex wrapperTableLock_;

    /// @brief Set the value of stateTable variable
    /// @param stateTable
    /// @throws on overflow
    void _setConversation(std::uint32_t const& number);
};

}  // namespace internal
}  // namespace diag
}  // namespace ara

#endif  // ARA_DIAG_INTERNAL_CONVERSATION_MANAGER_H_