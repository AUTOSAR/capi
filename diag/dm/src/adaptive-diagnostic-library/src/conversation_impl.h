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
/// @file       conversation_impl.h
/// @brief      This file provides the definitions of ConversationImpl and related types.
/// @details
/// @date       2024-12-18
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_DIAG_API_CONVERSATION_IMPL_H_
#define ARA_DIAG_API_CONVERSATION_IMPL_H_

#include <ara/core/vector.h>

#include <cstdint>
#include <memory>
#include <mutex>

#include "ara/diag/conversation.h"

namespace ara {
namespace diag {
namespace api {
/// @brief ConversationImpl
class ConversationImpl final : public Conversation
{
    friend class Conversation;

    /// @brief identifier_
    ConversationIdentifier identifier_{};

    std::uint32_t index_{};
    ActivityStatusType activityStatus_{};
    SessionControlType sessionId_{};
    SecurityLevelType securityLevelId_{};
    std::function< void(ActivityStatusType) > activityStatusNotifier_;
    std::function< void(SessionControlType) > diagnosticSessionNotifier_;
    std::function< void(SecurityLevelType) > securityLevelNotifier_;
    std::mutex lock_;
    /// @brief Move the content from another ConversationImpl
    /// @param that R-value reference of ConversationImpl to be moved
    inline void _move(ConversationImpl&& that) noexcept
    {
        index_           = that.index_;
        identifier_      = that.identifier_;
        activityStatus_  = that.activityStatus_;
        sessionId_       = that.sessionId_;
        securityLevelId_ = that.securityLevelId_;
    }

public:
    /// @brief Default destructor
    ~ConversationImpl() override = default;
    /// @brief Copy constructor is prohibited
    /// @param
    ConversationImpl(ConversationImpl const&) = delete;
    /// @brief Copy assignment operator is prohibited
    /// @param
    /// @return
    ConversationImpl& operator=(ConversationImpl const&) = delete;

    /// @brief ConversationImpl constructor
    /// @param state Conversation state
    /// @throws on overflow
    explicit ConversationImpl(std::uint32_t index)
    {
        index_           = index;
        identifier_      = 0U;
        activityStatus_  = ActivityStatusType::kInactive;
        sessionId_       = SessionControlType::kDefaultSession;
        securityLevelId_ = SecurityLevelType::kLocked;
    }

    /// @brief Move constructor
    /// @param that Reference to ConversationImpl
    ConversationImpl(ConversationImpl&& that) noexcept { _move(std::move(that)); }

    /// @brief Move assignment operator
    /// @param that
    /// @return
    inline ConversationImpl& operator=(ConversationImpl&& that) noexcept
    {
        if (this != &that) {
            _move(std::move(that));
        }
        return *this;
    }

    /// @brief Set conversation activation state
    /// @param[in] value State value
    /// @throws on overflow
    inline void SetActivityStatus(std::uint8_t const value)
    {
        std::unique_lock< std::mutex > lock{lock_};
        activityStatus_ = static_cast< ActivityStatusType >(value);
    }

    /// @brief Set conversation session
    /// @param[in] value Session ID
    /// @throws on overflow
    inline void SetSession(std::uint8_t const value)
    {
        std::unique_lock< std::mutex > lock{lock_};
        sessionId_ = static_cast< SessionControlType >(value);
    }

    /// @brief Set security level
    /// @param[in] value Security level
    /// @throws on overflow
    inline void SetSecurityLevel(std::uint8_t const value)
    {
        std::unique_lock< std::mutex > lock{lock_};
        securityLevelId_ = static_cast< SecurityLevelType >(value);
    }

    /// @brief Get conversation descriptor
    /// @return Conversation identifier
    inline ConversationIdentifier GetIdentifier() noexcept
    {
        std::unique_lock< std::mutex > lock{lock_};
        return identifier_;
    }

    /// @brief Set conversation identifier
    /// @param[in] value Conversation identifier
    inline void SetIdentifier(ConversationIdentifier value) noexcept
    {
        std::unique_lock< std::mutex > lock{lock_};
        identifier_ = value;
    }

    inline std::uint32_t GetIndex() noexcept
    {
        std::unique_lock< std::mutex > lock{lock_};
        return index_;
    }

    /// @brief Register a notifier function which is called if the activity is changed.
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[in] notifier notifier function to be called
    /// @return void when the registering went fine or error
    /// @throws on overflow
    ///
    /// @traceid{SWS_DM_00695}@tracestatus{draft}
    inline ara::core::Result< void > SetActivityNotifier(std::function< void(ActivityStatusType) > notifier) noexcept
    {
        std::unique_lock< std::mutex > lock{lock_};
        activityStatusNotifier_ = std::move(notifier);
        return {};
    }

    /// @brief Register a notifier function which is called if the Session is changed.
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[in] notifier notifier function to be called
    /// @return void when the registering went fine or error
    /// @throws on overflow
    /// @traceid{SWS_DM_00697}@tracestatus{draft}
    inline ara::core::Result< void > SetDiagnosticSessionNotifier(
        std::function< void(SessionControlType) > notifier) noexcept
    {
        std::unique_lock< std::mutex > lock{lock_};
        diagnosticSessionNotifier_ = std::move(notifier);
        return {};
    }

    /// @brief Register a notifier function which is called if the SecurityLevel is changed.
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[in] notifier notifier function to be called
    /// @return void when the registering went fine or error
    /// @throws on overflow
    /// @traceid{SWS_DM_00699}@tracestatus{draft}
    inline ara::core::Result< void > SetSecurityLevelNotifier(
        std::function< void(SecurityLevelType) > notifier) noexcept
    {
        std::unique_lock< std::mutex > lock{lock_};
        securityLevelNotifier_ = std::move(notifier);
        return {};
    }

    inline void UpdateActivity(std::uint8_t activeStatus) noexcept
    {
        SetActivityStatus(activeStatus);
        if (activityStatusNotifier_) {
            activityStatusNotifier_(activityStatus_);
        }
    }

    inline void UpdateSession(std::uint8_t sessionId) noexcept
    {
        SetSession(sessionId);
        if (diagnosticSessionNotifier_) {
            diagnosticSessionNotifier_(sessionId_);
        }
    }

    inline void UpdateSecurityLevel(std::uint8_t securityLevelId) noexcept
    {
        SetSecurityLevel(securityLevelId);
        if (securityLevelNotifier_) {
            securityLevelNotifier_(securityLevelId_);
        }
    }
};

}  // namespace api
}  // namespace diag
}  // namespace ara

#endif  // ARA_DIAG_API_CONVERSATION_IMPL_H_