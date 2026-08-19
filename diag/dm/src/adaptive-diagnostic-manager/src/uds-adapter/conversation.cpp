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
/// @brief
/// @details
/// @date       2024-12-06
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================

#include "conversation.h"

#include "log/log.h"
#include "serialization/serialization.h"
namespace ara {
namespace diag {
namespace dmd {

Conversation::Conversation(std::uint32_t index) noexcept
    : index_{index}
    , identifier_{}
    , state_{ConversationState::kInactive}
    , clientAddress_{}
    , requestContextState_{isoft::uds::server::RequestContextState::kEnd}
{
}

/// @brief Initialize the configuration of a software set
/// @param dmc Configuration node
/// @return Whether initialization is successful
bool Conversation::Initialize(std::function< std::uint8_t() > getSessionCallback) noexcept
{
    getSessionCallback_ = std::move(getSessionCallback);
    return true;
}

void Conversation::RegisterConversationStateChangeNotify(
    std::function< void(std::uint32_t, ConversationState) > callback) noexcept
{
    callback_ = std::move(callback);
}

ConversationState Conversation::GetConversationState() noexcept { return state_; }

void Conversation::RequestContextState(isoft::uds::server::Address clientAddress,
                                       isoft::uds::server::RequestContextState state) noexcept
{
    ara::core::String requestStateName;
    requestContextState_ = state;
    common::LogVerbose() << "Conversation::RequestContextState| clientAddress: " << clientAddress << "request state";
    switch (state) {
        case isoft::uds::server::RequestContextState::kRequest:
            requestStateName = "Request";
            SetClientAddress(clientAddress);
            SetConversationSate(ConversationState::kActive);
            break;
        case isoft::uds::server::RequestContextState::kResponse:
            requestStateName = "Response";
            break;
        case isoft::uds::server::RequestContextState::kConfirm:
            requestStateName = "Confirm";
            break;
        case isoft::uds::server::RequestContextState::kCanceled:
            requestStateName = "Canceled";
            SetClientAddress(0);
            SetConversationSate(ConversationState::kInactive);
            break;
        case isoft::uds::server::RequestContextState::kEnd:
            requestStateName = "End";
            if (getSessionCallback_() == 0x01U) {
                SetClientAddress(0);
                SetConversationSate(ConversationState::kInactive);
            }
            break;
        default:
            break;
    }
    common::LogVerbose() << "Conversation::RequestContextState| clientAddress: " << clientAddress << "request state"
                         << requestStateName;
    return;
}

void Conversation::SetConversationSate(ConversationState state) noexcept
{
    if (state != state_) {
        state_ = state;
        _notify();
    }
}

void Conversation::SetClientAddress(isoft::uds::server::Address clientAddress) noexcept
{
    clientAddress_ = clientAddress;
    _updateIdentifier();
}

void Conversation::_notify() noexcept { callback_(index_, state_); }

void Conversation::_updateIdentifier() noexcept
{
    std::vector< std::uint8_t > buffer;
    isoft::serialize::Serialize(buffer, clientAddress_);
    while (buffer.size() != sizeof(std::uint64_t)) {
        buffer.insert(buffer.begin(), 0x00U);
    }
    isoft::serialize::Deserialize(buffer, identifier_);
    common::LogVerbose() << "Conversation::UpdateIdentifier| identifier: " << identifier_;
}
}  // namespace dmd
}  // namespace diag
}  // namespace ara
