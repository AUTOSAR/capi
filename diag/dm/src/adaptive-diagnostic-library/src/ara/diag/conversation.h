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
/// @brief      This file provides the definitions of Conversation and related types.
/// @details
/// @date       2021-11-23
/// @author     jiawei
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_DIAG_CONVERSATION_H_
#define ARA_DIAG_CONVERSATION_H_

#include <ara/core/result.h>
#include <ara/core/string_view.h>
#include <ara/core/vector.h>

#include <cstdint>
#include <functional>
#include <memory>

#include "meta_info.h"

namespace ara {
namespace diag {

/// @brief Type for current activity status
///
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
///
/// @traceid{SWS_DM_00690}@tracestatus{draft}
enum class ActivityStatusType : std::uint8_t
{
    kActive   = 0x00,  ///<  Currently active; i.e. request is currently processed or non-default session is active.
    kInactive = 0x01,  ///<  Currently not active
};

/// @brief Type for the active security level.
///
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
///
/// @traceid{SWS_DM_00705}@tracestatus{draft}
enum class SecurityLevelType : std::uint8_t
{
    kLocked = 0x00,  ///<  security level locked;
    /// k<configuration_dependent>  ///< i.e. the security is enabled, configuration dependent; formula:
    /// (SecurityAccessType
    ///< (requestSeed) + 1) / 2
};

/// @brief Type for the active diagnostic session.
///
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
///
/// @traceid{SWS_DM_00706}@tracestatus{draft}
enum class SessionControlType : std::uint8_t
{
    kDefaultSession                = 0x01,  ///< default session according to IS0 14229-1.
    kProgrammingSession            = 0x02,  ///< programming session according to IS0 14229-1.
    kExtendedDiagnosticSession     = 0x03,  ///< extended diagnostic session according to IS0 14229-1.
    kSafetySystemDiagnosticSession = 0x04   ///< safety system diagnostic session according to IS0 14229-1.
    ///< configuration dependent sessions
};

namespace api {
/// @brief Name ConversationIdentifier
using ConversationIdentifier = std::uint64_t;
}  // namespace api

/// @brief Conversation interface
///
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
///
/// @traceid{SWS_DM_00693}@tracestatus{draft}
class Conversation
{
public:
    /// @brief Properties allowing an identification of the conversation
    ///
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    ///
    /// @traceid{SWS_DM_00691}@tracestatus{draft}
    struct ConversationIdentifierType
    {
        /// @brief vendor-specific
        api::ConversationIdentifier value_;  /// NOLINT
    };

    /// @brief Get one conversation based on given MetaInfo
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[in] metaInfo contains additional meta information
    /// @return ara::core::Result<Conversation&> Conversation object or error
    ///
    ///
    /// @traceid{SWS_DM_00692}@tracestatus{draft}
    static ara::core::Result< Conversation& > GetConversation(MetaInfo& metaInfo);

    /// @brief Get all possible conversations
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @return a vector of all possibe Conversation objects
    ///
    ///
    /// @traceid{SWS_DM_00782}@tracestatus{draft}
    static ara::core::Vector< std::reference_wrapper< Conversation > > GetAllConversations();
    // FIXME: the signature in 2011 is
    /// static ara::core::Vector<Conversation&> GetAllConversations();

    /// @brief Get all currently active conversations
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @return a vector of all currently active (GetActivityStatus() == kActive) Conversation objects
    /// @throws on overflow
    ///
    /// @traceid{SWS_DM_00783}@tracestatus{draft}
    static ara::core::Vector< std::reference_wrapper< Conversation > > GetCurrentActiveConversations();
    // FIXME: the signature in 2011 is
    /// static ara::core::Vector<Conversation&> GetCurrentActiveConversations();

    /// @brief Represents the status of an active conversation.
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @return the activity status of the conversation
    ///
    ///
    /// @traceid{SWS_DM_00694}@tracestatus{draft}
    ara::core::Result< ActivityStatusType > GetActivityStatus();

    /// @brief Register a notifier function which is called if the activity is changed.
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[in] notifier notifier function to be called
    /// @return void when the registering went fine or error
    /// @throws on overflow
    ///
    /// @traceid{SWS_DM_00695}@tracestatus{draft}
    ara::core::Result< void > SetActivityNotifier(std::function< void(ActivityStatusType) > notifier);

    /// @brief Represents the current active diagnostic session of an active conversation.
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @return the current session
    ///
    ///
    /// @traceid{SWS_DM_00696}@tracestatus{draft}
    ara::core::Result< SessionControlType > GetDiagnosticSession();

    /// @brief Register a notifier function which is called if the Session is changed.
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[in] notifier notifier function to be called
    /// @return void when the registering went fine or error
    /// @throws on overflow
    /// @traceid{SWS_DM_00697}@tracestatus{draft}
    ara::core::Result< void > SetDiagnosticSessionNotifier(std::function< void(SessionControlType) > notifier);

    /// @brief Converts the given diagnostic session into the ShortName
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[in] session Diagnostic session the shortname shall be returned for.
    /// @return ara::core::Result<ara::core::StringView> the session as shortName; DiagnosticSession.shortName
    ///
    ///
    /// @traceid{SWS_DM_00707}@tracestatus{draft}
    ara::core::Result< ara::core::StringView > GetDiagnosticSessionShortName(SessionControlType session);

    /// @brief Represents the current active diagnostic SecurityLevel of an active conversation.
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @return the current SecurityLevel
    ///
    ///
    /// @traceid{SWS_DM_00698}@tracestatus{draft}
    ara::core::Result< SecurityLevelType > GetDiagnosticSecurityLevel();

    /// @brief Register a notifier function which is called if the SecurityLevel is changed.
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[in] notifier notifier function to be called
    /// @return void when the registering went fine or error
    /// @throws on overflow
    /// @traceid{SWS_DM_00699}@tracestatus{draft}
    ara::core::Result< void > SetSecurityLevelNotifier(std::function< void(SecurityLevelType) > notifier);

    /// @brief Converts the given diagnostic SecurityLevel into the ShortName
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[in] securityLevel Security level enum the shortname shall be returned for.
    /// @return ara::core::Result<ara::core::StringView> the SecurityLevel as shortName;
    /// DiagnosticSecurityLevel.shortName
    /// @throws on overflow
    ///
    /// @traceid{SWS_DM_00708}@tracestatus{draft}
    ara::core::Result< ara::core::StringView > GetDiagnosticSecurityLevelShortName(SecurityLevelType securityLevel);

    /// @brief Getter for the current identification properties of the active conversation.
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @return the conversation information
    ///
    ///
    /// @traceid{SWS_DM_00700}@tracestatus{draft}
    ara::core::Result< ConversationIdentifierType > GetConversationIdentifier();

    /// @brief Method to reset the current session to default session.
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @return void on success or error
    /// @throws on overflow
    ///
    /// @traceid{SWS_DM_00701}@tracestatus{draft}
    ara::core::Result< void > ResetToDefaultSession();

protected:
    /// @brief Ctor is vendor-specific
    ///
    /// @traceid{SWS_DM_00703}@tracestatus{draft}
    Conversation() noexcept = default;

    /// @brief Dtor is vendor-specific
    ///
    /// @traceid{SWS_DM_00704}@tracestatus{draft}
    virtual ~Conversation() noexcept = default;

public:
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
};
}  // namespace diag
}  // namespace ara

#endif  // ARA_DIAG_CONVERSATION_H_