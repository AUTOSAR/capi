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
/// @brief
/// @details
/// @date       2024-12-06
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================

#ifndef ADAPTIVEDIAGNOSTICMANAGER_SRC_UDSADAPTER_CONVERSATIONMANAGER_H_
#define ADAPTIVEDIAGNOSTICMANAGER_SRC_UDSADAPTER_CONVERSATIONMANAGER_H_
#include <ara/core/result.h>
#include <isoft/uds/server.h>

#include "conversation.h"
#include "gen_code/conversation_manager/serviceAgent/conversation_manager_agent.h"
namespace ara {
namespace diag {
namespace dmd {
class ConversationManager
{
public:
    /// @brief Default constructor
    ConversationManager(/* args */) = default;
    /// @brief Default destructor
    ~ConversationManager() = default;
    /// @brief copy constructor
    /// @param other
    ConversationManager(ConversationManager const& other) noexcept = delete;
    /// @brief copy assignment operator
    /// @param other
    /// @return ConversationManager&
    ConversationManager& operator=(ConversationManager const& other) noexcept = delete;
    /// @brief move constructor
    /// @param other
    ConversationManager(ConversationManager&& other) noexcept = default;
    /// @brief move assignment operator
    /// @param other
    /// @return ConversationManager&
    ConversationManager& operator=(ConversationManager&& other) noexcept = default;

    /// @brief Initialize the configuration of a software set
    /// @param dmc Configuration node
    /// @return Whether initialization is successful
    bool Initialize(std::shared_ptr< isoft::uds::server::Server > const& serverPtr,
                    std::uint32_t maxNumber,
                    std::uint32_t serviceId,
                    std::set< isoft::uds::server::SessionModel >& sessionConfigTable,
                    std::set< isoft::uds::server::SecurityLevelConfig >& securityLevelTable) noexcept;

private:
    void _requestContextState(isoft::uds::server::Address serverAddress,
                              isoft::uds::server::Address clientAddress,
                              isoft::uds::server::RequestContextState state) noexcept;
    void _initializeAgent(std::uint32_t serviceId) noexcept;

    /// @brief Get Conversation object
    /// @param clientAddress
    /// @return
    std::shared_ptr< Conversation > _getConversation(isoft::uds::server::Address clientAddress) noexcept;

private:
    std::map< std::uint32_t, std::shared_ptr< Conversation > > conversationTable_;

    isoft::uds::server::SessionId currentSession_{1};
    isoft::uds::server::SecurityLevelId currentSecurityLevel_{0};
    std::shared_ptr< isoft::uds::server::Server > serverPtr_;
    std::set< isoft::uds::server::SessionModel > sessionConfigTable_;
    std::set< isoft::uds::server::SecurityLevelConfig > securityLevelTable_;
    std::unique_ptr< isoft::dm::dis::ConversationManagerAgent > agentPtr_;
};
}  // namespace dmd
}  // namespace diag
}  // namespace ara

#endif  /// ADAPTIVEDIAGNOSTICMANAGER_SRC_UDSADAPTER_CONVERSATIONMANAGER_H_