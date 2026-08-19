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
/// @file       conversation_manager.cpp
/// @brief      This file provides the implementation of ConversationManager.
/// @details
/// @date       2022-11-21
/// @author     gaohuiming
/// @version    1.2.0
///
/// ================================================================

#include "conversation_manager.h"

#include <ara/core/core_error_domain.h>

#include "ara/diag/diag_error_domain.h"
#include "ara/diag/internal/conversation_trait.h"
#include "resolve.h"
#include "thread_pool/thread_pool.h"

namespace ara {
namespace diag {
namespace internal {

static const char *g_Specifier{"DefaultConversationInstanceSpecifier"};

ConversationManager::~ConversationManager()
{
    if (agentPtr_ != nullptr) {
        agentPtr_.reset();
    }
}

/// @brief ConversationManager initialization
/// @throws on overflow
void ConversationManager::Initialize()
{
    ara::core::InstanceSpecifier const instanceSpecifier{std::move(ara::core::StringView(g_Specifier))};
    ara::core::Result< InstanceInfo > const result{Resolve(instanceSpecifier)};
    if (!result.HasValue()) {
        LogWarn() << "ConversationManager::Initialize|failed to resolve "
                     "conversation instance specifier";
    } else {
        agentPtr_ = std::make_shared< isoft::dm::dic::ConversationManagerAgent >(result.Value().instanceId,
                                                                                 result.Value().serviceInstanceId);
        agentPtr_->RegisterOnReady([this](bool ready) {
            static bool s_First{true};
            if (ready && s_First) {
                s_First = false;
                isoft::ThreadPool::GetInstance().Submit([this]() {
                    ara::core::Result< uint8_t > requestResult = agentPtr_->GetConversationNumber();
                    if (requestResult.HasValue()) {
                        _setConversation(requestResult.Value());
                    }
                    agentPtr_->RegisterUpdateActiveStatusCallback([this](std::uint8_t index, std::uint8_t status) {
                        conversationTable_[index].UpdateActivity(status);
                        ara::core::Result< uint64_t > requestIdentifierResult = agentPtr_->GetIdentifier(index);
                        if (requestIdentifierResult.HasValue()) {
                            LogInfo() << "ConversationManager::Initialize|index:" << index
                                      << "   identifier:" << requestIdentifierResult.Value();
                            conversationTable_[index].SetIdentifier(requestIdentifierResult.Value());
                        } else {
                            LogWarn() << "ConversationManager::Initialize| get "
                                         "identifier fails! index:"
                                      << index;
                        }
                    });
                    agentPtr_->RegisterUpdateSessionIdCallback([this](std::uint8_t sessionId) {
                        LogInfo() << "conversation manager update session id:" << sessionId << "start";
                        for (auto &&conversation : conversationTable_) {
                            conversation.UpdateSession(sessionId);
                        }
                        LogInfo() << "conversation manager update session id:" << sessionId << "end";
                    });
                    agentPtr_->RegisterUpdateSecurityLevelIdCallback([this](std::uint8_t securityLevelId) {
                        for (auto &&conversation : conversationTable_) {
                            conversation.UpdateSecurityLevel(securityLevelId);
                        }
                    });
                });
            }
        });
    }
}

/// @brief Set the value of stateTable variable
/// @param stateTable
/// @throws on overflow
void ConversationManager::_setConversation(std::uint32_t const &number)
{
    std::unique_lock< std::mutex > const lock{wrapperTableLock_};
    if (conversationTable_.size() >= number) {
        return;
    }
    for (std::uint32_t idx = 1; idx <= number; idx++) {
        conversationTable_.emplace_back(api::ConversationImpl(idx));
    }
    ara::core::Result< uint8_t > sessionResult = agentPtr_->GetSessionId();
    std::uint8_t sessionId{1};
    if (sessionResult.HasValue()) {
        sessionId = sessionResult.Value();
    }
    ara::core::Result< uint8_t > securityLevelResult = agentPtr_->GetSecurityLevelId();
    std::uint8_t securityLevelId{0};
    if (securityLevelResult.HasValue()) {
        securityLevelId = securityLevelResult.Value();
    }
    for (auto &&conversation : conversationTable_) {
        std::uint32_t index                 = conversation.GetIndex();
        ara::core::Result< uint8_t > status = agentPtr_->GetActiveStatus(index);
        if (!status.HasValue()) {
            continue;
        }
        conversation.SetActivityStatus(status.Value());
        conversation.SetSession(sessionId);
        conversation.SetSecurityLevel(securityLevelId);
    }
}

/// @brief Get a conversation according to meta information
/// @param[in] metaInfo Meta information
/// @return Conversation object
/// @throws on overflow
api::ConversationImpl *ConversationManager::GetConversationImpl(MetaInfo &metaInfo)
{
    ara::core::Optional< ara::core::StringView > sa{metaInfo.GetValue(std::move(ara::core::StringView("kSA")))};
    if (!sa.has_value()) {
        return nullptr;
    }
    unsigned testerAddress{};
    std::string strSa{sa->data()};
    testerAddress = std::stoul(strSa, nullptr, UINT16_WIDTH);
    std::unique_lock< std::mutex > const lock{wrapperTableLock_};
    for (auto &&conversation : conversationTable_) {
        if (conversation.GetActivityStatus() != ActivityStatusType::kActive) {
            continue;
        }
        internal::ConversationTrait::IdentifierLayout layout{};
        layout.value = conversation.GetIdentifier();
        std::ignore  = layout.value;
        LogInfo() << "ConversationManager::GetConversationImpl|conversation index:" << conversation.GetIndex()
                  << " identifier:" << layout.value << " testerAddress:" << layout.field.testerAddress;
        if (layout.field.testerAddress == testerAddress) {
            return &conversation;
        }
    }
    return nullptr;
}

/// @brief Get a conversation according to meta information
/// @param[in] metaInfo Meta information
/// @return Conversation object
/// @throws on overflow
ara::core::Result< ara::diag::Conversation & > ConversationManager::GetConversation(MetaInfo &metaInfo)
{
    auto *const conversation = GetConversationImpl(metaInfo);
    if (conversation != nullptr) {
        return ara::core::Result< ara::diag::Conversation & >::FromValue(*conversation);
    }
    return ara::core::Result< Conversation & >::FromError(MakeErrorCode(DiagErrc::kGenericError, __LINE__));
}

/// @brief Get all conversations
/// @return Conversation list
/// @throws on overflow
ara::core::Vector< std::reference_wrapper< ara::diag::Conversation > > ConversationManager::GetAllConversations()
{
    std::unique_lock< std::mutex > const lock{wrapperTableLock_};
    ara::core::Vector< std::reference_wrapper< Conversation > > result;
    for (auto &conversation : conversationTable_) {
        ara::diag::Conversation *conversationPtr{&conversation};
        result.emplace_back(std::ref(*conversationPtr));
    }
    return result;
}

/// @brief Get currently active conversations
/// @return Active conversation list
/// @throws on overflow
ara::core::Vector< std::reference_wrapper< ara::diag::Conversation > >
ConversationManager::GetCurrentActiveConversations()
{
    std::unique_lock< std::mutex > const lock{wrapperTableLock_};
    ara::core::Vector< std::reference_wrapper< Conversation > > result;
    for (auto &conversation : conversationTable_) {
        if (conversation.GetActivityStatus() == ActivityStatusType::kActive) {
            result.emplace_back(conversation);
        }
    }
    return result;
}

/// @brief Get diagnostic session name
/// @param[in] session Session value
/// @return Session name
/// @throws on overflow
ara::core::Result< ara::core::String > ConversationManager::GetDiagnosticSessionShortName(std::uint8_t session)
{
    if (agentPtr_ == nullptr) {
        LogWarn() << "ConversationManager::GetDiagnosticSessionShortName|"
                     "conversation interface not configured";
        return ara::core::Result< ara::core::String >::FromError(MakeErrorCode(DiagErrc::kGenericError, __LINE__));
    }
    return agentPtr_->GetDiagnosticSessionShortName(session);
}

/// @brief Get diagnostic security level name
/// @param[in] securityLevel Security level
/// @return Security level name
/// @throws on overflow
ara::core::Result< ara::core::String > ConversationManager::GetDiagnosticSecurityLevelShortName(
    std::uint8_t securityLevel)
{
    if (agentPtr_ == nullptr) {
        LogWarn() << "ConversationManager::GetDiagnosticSecurityLevelShortName|"
                     "conversation interface not configured";
        return ara::core::Result< ara::core::String >::FromError(MakeErrorCode(DiagErrc::kGenericError, __LINE__));
    }
    return agentPtr_->GetDiagnosticSecurityLevelShortName(securityLevel);
}

/// @brief Switch to default session SWS_DM_00701
/// @param[in] identifier Conversation descriptor
/// @return Operation result
/// @throws on overflow
ara::core::Result< void > ConversationManager::ResetToDefaultSession()
{
    if (agentPtr_ == nullptr) {
        LogWarn() << "ConversationManager::ResetToDefaultSession|conversation "
                     "interface not configured";
        return ara::core::Result< void >::FromError(MakeErrorCode(DiagErrc::kGenericError, __LINE__));
    }
    return agentPtr_->ResetToDefaultSession();
}

}  // namespace internal
}  // namespace diag
}  // namespace ara
