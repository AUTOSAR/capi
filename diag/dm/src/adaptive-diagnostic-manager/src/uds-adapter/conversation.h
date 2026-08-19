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
/// @file       conversation.h
/// @brief
/// @details
/// @date       2024-12-06
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================

#ifndef ADAPTIVEDIAGNOSTICMANAGER_SRC_UDSADAPTER_CONVERSATION_H_
#define ADAPTIVEDIAGNOSTICMANAGER_SRC_UDSADAPTER_CONVERSATION_H_
#include <isoft/uds/common.h>
namespace ara {
namespace diag {
namespace dmd {
enum class ConversationState
{
    kActive,
    kInactive
};

class Conversation
{
public:
    /// @brief Default constructor
    /// @param index
    explicit Conversation(std::uint32_t index) noexcept;
    /// @brief Default destructor
    ~Conversation() = default;
    /// @brief copy constructor
    /// @param other
    Conversation(Conversation const& other) noexcept = default;
    /// @brief copy assignment operator
    /// @param other
    /// @return Conversation&
    Conversation& operator=(Conversation const& other) noexcept = default;
    /// @brief move constructor
    /// @param other
    Conversation(Conversation&& other) noexcept = default;
    /// @brief move assignment operator
    /// @param other
    /// @return Conversation&
    Conversation& operator=(Conversation&& other) noexcept = default;

    /// @brief Initialize the configuration of a software set
    /// @param dmc Configuration node
    /// @return Whether initialization is successful
    bool Initialize(std::function< std::uint8_t() > getSessionCallback) noexcept;

    void RegisterConversationStateChangeNotify(
        std::function< void(std::uint32_t, ConversationState) > callback) noexcept;

    void RequestContextState(isoft::uds::server::Address clientAddress,
                             isoft::uds::server::RequestContextState state) noexcept;

    void SetConversationSate(ConversationState state) noexcept;

    /// @brief Get the current status of the request
    /// @return
    isoft::uds::server::RequestContextState GetRequestContextState() noexcept { return requestContextState_; }

    std::uint32_t Index() const noexcept { return index_; }

    std::uint64_t GetIdentifier() const noexcept { return identifier_; }

    void SetClientAddress(isoft::uds::server::Address clientAddress) noexcept;

    isoft::uds::server::Address GetClientAddress() const noexcept { return clientAddress_; }

    ConversationState GetConversationState() noexcept;

private:
    void _notify() noexcept;

    void _updateIdentifier() noexcept;

private:
    std::uint32_t index_;
    std::uint64_t identifier_;
    ConversationState state_;
    isoft::uds::server::Address clientAddress_;
    std::function< void(std::uint32_t, ConversationState) > callback_;
    std::function< std::uint8_t() > getSessionCallback_;
    isoft::uds::server::RequestContextState requestContextState_;
};
}  // namespace dmd
}  // namespace diag
}  // namespace ara

#endif  /// ADAPTIVEDIAGNOSTICMANAGER_SRC_UDSADAPTER_CONVERSATION_H_