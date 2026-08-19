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
/// @brief
/// @details
/// @date       2024-12-06
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================

#include "conversation_manager.h"

#include "log/log.h"
#include "thread_pool/thread_pool.h"
namespace ara {
namespace diag {
namespace dmd {
/// @brief Initialize the configuration of a software set
/// @param dmc Configuration node
/// @return Whether initialization is successful
bool ConversationManager::Initialize(std::shared_ptr< isoft::uds::server::Server > const& serverPtr,
                                     std::uint32_t maxNumber,
                                     std::uint32_t serviceId,
                                     std::set< isoft::uds::server::SessionModel >& sessionConfigTable,
                                     std::set< isoft::uds::server::SecurityLevelConfig >& securityLevelTable) noexcept
{
    isoft::uds::server::NotifyRequestContextStateCallback stateCallback{
        [&](isoft::uds::server::Address serverAddress, isoft::uds::server::Address clientAddress,
            isoft::uds::server::RequestContextState state) {
            common::LogInfo() << "ConversationManager| update request context state:"
                              << static_cast< std::uint8_t >(state);
            isoft::ThreadPool::GetInstance().Submit([this, serverAddress, clientAddress, state]() {
                _requestContextState(serverAddress, clientAddress, state);
            });
        }};
    serverPtr_ = serverPtr;
    serverPtr->AddNotifyRequestContextState(stateCallback);
    isoft::uds::server::NotifyOfSessionChangeCallback sessionCallback{[this](isoft::uds::server::SessionId,
                                                                             isoft::uds::server::SessionId newId) {
        currentSession_ = newId;
        if (agentPtr_.get() != nullptr) {
            agentPtr_->UpdateSessionId(currentSession_);
        }
        if (currentSession_ == 1) {
            for (auto&& conversationPtr : conversationTable_) {
                if (conversationPtr.second->GetRequestContextState() == isoft::uds::server::RequestContextState::kEnd) {
                    conversationPtr.second->SetClientAddress(0);
                    conversationPtr.second->SetConversationSate(ConversationState::kInactive);
                }
            }
        }
        common::LogInfo() << "ConversationManager| update sesssion id:" << newId;
    }};
    serverPtr->AddNotifyOfSessionChange(sessionCallback);
    isoft::uds::server::NotifyOfSecurityLevelChangeCallback securityLevelCallback{
        [this](isoft::uds::server::SecurityLevelId id) {
            currentSecurityLevel_ = id;
            if (agentPtr_.get() != nullptr) {
                agentPtr_->UpdateSecurityLevelId(currentSecurityLevel_);
            }
            common::LogInfo() << "ConversationManager| update security level id:" << id;
        }};
    serverPtr->AddNotifyOfSecurityLevelChange(securityLevelCallback);
    conversationTable_.clear();
    for (size_t i = 1; i <= maxNumber; i++) {
        std::shared_ptr< Conversation > conversationPtr{std::make_shared< Conversation >(i)};
        conversationPtr->Initialize([this]() { return currentSession_; });
        conversationPtr->RegisterConversationStateChangeNotify([this](std::uint32_t index, ConversationState state) {
            switch (state) {
                case ConversationState::kInactive: {
                    if (agentPtr_.get() != nullptr) {
                        agentPtr_->UpdateActiveState(index, 1);
                    }
                    common::LogInfo() << "ConversationManager|update active state index:" << index
                                      << " active status: inActive";
                }

                break;
                case ConversationState::kActive: {
                    if (agentPtr_.get() != nullptr) {
                        agentPtr_->UpdateActiveState(index, 0);
                    }
                    common::LogInfo() << "ConversationManager|update active state index:" << index
                                      << " active status: Active";
                } break;
                default:
                    break;
            }
        });
        conversationTable_.insert({i, conversationPtr});
    }
    sessionConfigTable_ = sessionConfigTable;
    securityLevelTable_ = securityLevelTable;

    _initializeAgent(serviceId);
    return true;
}

void ConversationManager::_requestContextState(isoft::uds::server::Address clientAddress,
                                               isoft::uds::server::Address,
                                               isoft::uds::server::RequestContextState state) noexcept
{
    std::shared_ptr< Conversation > conversationPtr{_getConversation(clientAddress)};
    if (conversationPtr.get() != nullptr) {
        conversationPtr->RequestContextState(clientAddress, state);
    }
}

/// @brief Get Conversation object
/// @param serverAddress
/// @return
std::shared_ptr< Conversation > ConversationManager::_getConversation(
    isoft::uds::server::Address clientAddress) noexcept
{
    std::shared_ptr< Conversation > conversationPtr{nullptr};
    /// Match Conversation according to clientAddress, apply for a new Conversation object if the match fails
    auto findResult = std::find_if(
        conversationTable_.begin(), conversationTable_.end(),
        [&clientAddress](
            std::pair< const unsigned int, std::shared_ptr< ara::diag::dmd::Conversation > > const& valuePtr) {
            return clientAddress == valuePtr.second->GetClientAddress();
        });
    if (findResult != conversationTable_.end()) {
        conversationPtr = findResult->second;
    } else {
        findResult = std::find_if(
            conversationTable_.begin(), conversationTable_.end(),
            [&clientAddress](
                std::pair< const unsigned int, std::shared_ptr< ara::diag::dmd::Conversation > > const& valuePtr) {
                return valuePtr.second->GetConversationState() == ConversationState::kInactive;
            });
        if (findResult != conversationTable_.end()) {
            conversationPtr = findResult->second;
        }
    }
    return conversationPtr;
}

void ConversationManager::_initializeAgent(std::uint32_t serviceId) noexcept
{
    if (serviceId == 0) {
        common::LogError() << "ConversationManager::_initializeAgent| conversation sevice not config!";
        agentPtr_.reset();
        return;
    }
    agentPtr_ = std::make_unique< isoft::dm::dis::ConversationManagerAgent >(serviceId);

    agentPtr_->RegisterGetActiveStatusCallback([this](std::uint8_t index) {
        std::uint8_t activeStatus{1U};
        decltype(auto) findResult = conversationTable_.find(index);
        if (findResult != conversationTable_.end()) {
            if (findResult->second->GetConversationState() == ConversationState::kActive) {
                activeStatus = 0;
            }
        }
        return activeStatus;
    });

    agentPtr_->RegisterGetConversationNumberCallback([this]() { return conversationTable_.size(); });

    agentPtr_->RegisterGetSessionIdCallback([this]() { return currentSession_; });

    agentPtr_->RegisterGetSecurityLevelIdCallback([this]() { return currentSecurityLevel_; });

    agentPtr_->RegisterGetIdentifierCallback([this](std::uint8_t index) {
        std::uint64_t result{0U};
        decltype(auto) findResult = conversationTable_.find(index);
        if (findResult != conversationTable_.end()) {
            result = findResult->second->GetIdentifier();
            common::LogInfo() << "conversation index:" << index << " identifier:" << result;
        } else {
            common::LogWarn() << "not found conversation in index:" << index;
        }
        return result;
    });

    agentPtr_->RegisterGetDiagnosticSessionShortNameCallback([this](std::uint8_t sessionId) {
        isoft::uds::server::SessionModel model{};
        model.id                  = sessionId;
        decltype(auto) findResult = sessionConfigTable_.find(model);
        if (findResult != sessionConfigTable_.end()) {
            return ara::core::StringView(findResult->shortName.c_str());
        }
        return ara::core::StringView();
    });

    agentPtr_->RegisterGetDiagnosticSecurityLevelShortNameCallback([this](std::uint8_t securityLevelId) {
        isoft::uds::server::SecurityLevelConfig model{};
        model.id                  = securityLevelId;
        decltype(auto) findResult = securityLevelTable_.find(model);
        if (findResult != securityLevelTable_.end()) {
            return ara::core::StringView(findResult->shortName.c_str());
        }
        return ara::core::StringView();
    });

    agentPtr_->RegisterResetToDefaultSessionCallback([this]() {
        common::LogInfo() << "conversation reset default session";
        serverPtr_->ResetDefaultSession();
    });
}

}  // namespace dmd
}  // namespace diag
}  // namespace ara
