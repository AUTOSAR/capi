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
/// @file       conversation.cpp
/// @brief      This file provides the implementation of Conversation.
/// @details
/// @date       2022-04-29
/// @author     gaohuiming
/// @version    1.2.0
///
/// ================================================================

#include "ara/diag/conversation.h"

#include "conversation_impl.h"
#include "log/log.h"
#include "utility.h"
namespace ara {
namespace diag {

/// @brief Get one conversation based on given MetaInfo
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
/// @param[in] metaInfo contains additional meta information
/// @return ara::core::Result<Conversation&> Conversation object or error
/// @throws on overflow
///
/// @traceid{SWS_DM_00692}@tracestatus{draft}
ara::core::Result< Conversation& > Conversation::GetConversation(MetaInfo& metaInfo)
{
    return internal::GetConversationManager().GetConversation(metaInfo);
}

/// @brief Get all possible conversations
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
/// @return a vector of all possibe Conversation objects
/// @throws on overflow
///
/// @traceid{SWS_DM_00782}@tracestatus{draft}
ara::core::Vector< std::reference_wrapper< Conversation > > Conversation::GetAllConversations()
{
    return internal::GetConversationManager().GetAllConversations();
}

/// @brief Get all currently active conversations
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
/// @return a vector of all currently active (GetActivityStatus() == kActive) Conversation objects
/// @throws on overflow
///
/// @traceid{SWS_DM_00783}@tracestatus{draft}
ara::core::Vector< std::reference_wrapper< Conversation > > Conversation::GetCurrentActiveConversations()
{
    return internal::GetConversationManager().GetCurrentActiveConversations();
}

/// @brief Represents the status of an active conversation.
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
/// @return the activity status of the conversation
/// @throws on overflow
///
/// @traceid{SWS_DM_00694}@tracestatus{draft}
ara::core::Result< ActivityStatusType > Conversation::GetActivityStatus()
{
    api::ConversationImpl* impl{dynamic_cast< api::ConversationImpl* >(this)};
    std::unique_lock< std::mutex > lock{impl->lock_};
    return {impl->activityStatus_};
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
ara::core::Result< void > Conversation::SetActivityNotifier(std::function< void(ActivityStatusType) > notifier)
{
    api::ConversationImpl* impl{dynamic_cast< api::ConversationImpl* >(this)};
    return impl->SetActivityNotifier(std::move(notifier));
}

/// @brief Represents the current active diagnostic session of an active conversation.
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
/// @return the current session
/// @throws on overflow
///
/// @traceid{SWS_DM_00696}@tracestatus{draft}
ara::core::Result< SessionControlType > Conversation::GetDiagnosticSession()
{
    api::ConversationImpl* impl{dynamic_cast< api::ConversationImpl* >(this)};
    std::unique_lock< std::mutex > lock{impl->lock_};
    return {impl->sessionId_};
}

/// @brief Register a notifier function which is called if the Session is changed.
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
/// @param[in] notifier notifier function to be called
/// @return void when the registering went fine or error
/// @throws on overflow
/// @traceid{SWS_DM_00697}@tracestatus{draft}
ara::core::Result< void > Conversation::SetDiagnosticSessionNotifier(std::function< void(SessionControlType) > notifier)
{
    api::ConversationImpl* impl{dynamic_cast< api::ConversationImpl* >(this)};
    return impl->SetDiagnosticSessionNotifier(std::move(notifier));
}

/// @brief Converts the given diagnostic session into the ShortName
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
/// @param[in] session Diagnostic session the shortname shall be returned for.
/// @return ara::core::Result<ara::core::String> the session as shortName; DiagnosticSession.shortName
/// @throws on overflow
///
/// @traceid{SWS_DM_00707}@tracestatus{draft}
ara::core::Result< ara::core::StringView > Conversation::GetDiagnosticSessionShortName(SessionControlType session)
{
    ara::core::Result< ara::core::String > shortNameResult{
        internal::GetConversationManager().GetDiagnosticSessionShortName(static_cast< std::uint8_t >(session))};
    if (shortNameResult.HasValue()) {
        static ara::core::String s_ShortName{};
        s_ShortName = shortNameResult.Value();
        common::LogInfo() << "Conversation::GetDiagnosticSessionShortName| session name:" << s_ShortName;
        return ara::core::Result< ara::core::StringView >::FromValue(s_ShortName.c_str());
    }
    return ara::core::Result< ara::core::StringView >::FromError(shortNameResult.Error());
}

/// @brief Represents the current active diagnostic SecurityLevel of an active conversation.
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
/// @return the current SecurityLevel
/// @throws on overflow
///
/// @traceid{SWS_DM_00698}@tracestatus{draft}
ara::core::Result< SecurityLevelType > Conversation::GetDiagnosticSecurityLevel()
{
    api::ConversationImpl* impl{dynamic_cast< api::ConversationImpl* >(this)};
    std::unique_lock< std::mutex > lock{impl->lock_};
    return {impl->securityLevelId_};
}

/// @brief Register a notifier function which is called if the SecurityLevel is changed.
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
/// @param[in] notifier notifier function to be called
/// @return void when the registering went fine or error
/// @throws on overflow
/// @traceid{SWS_DM_00699}@tracestatus{draft}
ara::core::Result< void > Conversation::SetSecurityLevelNotifier(std::function< void(SecurityLevelType) > notifier)
{
    api::ConversationImpl* impl{dynamic_cast< api::ConversationImpl* >(this)};
    return impl->SetSecurityLevelNotifier(std::move(notifier));
}

/// @brief Converts the given diagnostic SecurityLevel into the ShortName
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
/// @param[in] securityLevel Security level enum the shortname shall be returned for.
/// @return ara::core::Result<ara::core::String> the SecurityLevel as shortName;
/// DiagnosticSecurityLevel.shortName
/// @throws on overflow
///
/// @traceid{SWS_DM_00708}@tracestatus{draft}
ara::core::Result< ara::core::StringView > Conversation::GetDiagnosticSecurityLevelShortName(
    SecurityLevelType securityLevel)
{
    ara::core::Result< ara::core::String > shortNameResult{
        internal::GetConversationManager().GetDiagnosticSecurityLevelShortName(
            static_cast< std::uint8_t >(securityLevel))};
    if (shortNameResult.HasValue()) {
        static ara::core::String s_ShortName{};
        s_ShortName = shortNameResult.Value();
        common::LogInfo() << "Conversation::GetDiagnosticSecurityLevelShortName| session name:" << s_ShortName;
        return ara::core::Result< ara::core::StringView >::FromValue(s_ShortName.c_str());
    }
    return ara::core::Result< ara::core::StringView >::FromError(shortNameResult.Error());
}

/// @brief Getter for the current identification properties of the active conversation.
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
/// @return the conversation information
/// @throws on overflow
///
/// @traceid{SWS_DM_00700}@tracestatus{draft}
ara::core::Result< Conversation::ConversationIdentifierType > Conversation::GetConversationIdentifier()
{
    api::ConversationImpl* impl{dynamic_cast< api::ConversationImpl* >(this)};
    return ara::core::Result< Conversation::ConversationIdentifierType >::FromValue(
        Conversation::ConversationIdentifierType{impl->identifier_});
}

/// @brief Method to reset the current session to default session.
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
/// @return void on success or error
/// @throws on overflow
///
/// @traceid{SWS_DM_00701}@tracestatus{draft}
ara::core::Result< void > Conversation::ResetToDefaultSession()
{
    return internal::GetConversationManager().ResetToDefaultSession();
}

}  // namespace diag
}  // namespace ara
