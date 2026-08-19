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
/// @file       session_manager.cpp
/// @brief      This file implements session_manager functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=session_management
/// @trace_id_sr=1edf8088-f170-41bb-a325-19b81f84c357
/// @unit_name=SessionManager
/// @unit_description=This file provides session_manager functionality
/// @interface_level=unit
/// @endcode
///
/// ================================================================

#include "session_manager.h"

#include "common/log.h"
namespace isoft {
namespace uds {
namespace server {

Session::Session(SessionId& currentSessionId) noexcept : config_{}, currentSessionId_{currentSessionId} {}

std::int32_t Session::Initialize(SessionModel const& model) noexcept
{
    config_ = model;
    return 0;
}

bool Session::Check() const noexcept { return config_.id == currentSessionId_; }

std::string Session::GetShortName() const noexcept { return config_.shortName; }

std::uint16_t Session::GetP2ServerMax() const noexcept { return config_.p2ServerMax; }

std::uint16_t Session::GetP2StarServerMax() const noexcept { return config_.p2StarServerMax; }

std::uint8_t Session::GetSessionId() const noexcept { return config_.id; }

SessionManager::SessionManager() noexcept : currentSessionId_{1} {}


std::int32_t SessionManager::Initialize(std::set< SessionModel > const& model) noexcept
{
    if (model.empty()) {
        LogWarn() << "Please add session model";
        return -1;
    }
    std::int32_t result{0};
    for (auto&& sessionConfig : model) {
        std::shared_ptr< Session > sessionPtr{nullptr};
        sessionPtr = std::make_shared< Session >(currentSessionId_);
        if (sessionPtr.get() == nullptr) {
            LogWarn() << "Failed to allocate session object memory,Please check memory";
            return -2;
        }
        result = sessionPtr->Initialize(sessionConfig);
        if (result != 0) {
            LogWarn() << "Please check session model,id:" << sessionConfig.id;
            return result;
        }
        auto findSessionResult = table_.find(sessionConfig.id);
        if (findSessionResult != table_.end()) {
            table_[sessionConfig.id] = sessionPtr;
        } else {
            std::ignore = table_.insert({sessionConfig.id, sessionPtr});
        }
    }
    return result;
}

SessionId SessionManager::GetCurrentSessionId() const noexcept { return currentSessionId_; }

NotifyOfSessionChangeCallbackId SessionManager::AddNotifyOfSessionChange(
    NotifyOfSessionChangeCallback& callback) noexcept
{
    NotifyOfSessionChangeCallbackId id{0U};
    NotifyOfSessionChangeCallbackId const maxId{0xFFFFFFFFU};
    while (id < maxId) {
        auto findIdResult = notifyOfSessionChangeCallbackTable_.find(id);
        if (findIdResult != notifyOfSessionChangeCallbackTable_.end()) {
            id++;
            continue;
        }
        std::ignore = notifyOfSessionChangeCallbackTable_.insert({id, callback});
        break;
    }
    return id;
}

void SessionManager::RemoveNotifyOfSessionChange(NotifyOfSessionChangeCallbackId id) noexcept
{
    auto findIdResult = notifyOfSessionChangeCallbackTable_.find(id);
    if (findIdResult != notifyOfSessionChangeCallbackTable_.end()) {
        std::ignore = notifyOfSessionChangeCallbackTable_.erase(findIdResult);
    }
}

void SessionManager::SetSession(SessionId id, Address client) noexcept
{
    SessionId const old{currentSessionId_};
    currentSessionId_ = id;
    if (id == 1U) {
        client_ = 0;
    } else {
        client_ = client;
    }
    LogInfo() << "SessionManager::SetSession|update session:" << id;
    for (auto&& notify : notifyOfSessionChangeCallbackTable_) {
        notify.second(old, currentSessionId_);
    }
}

std::shared_ptr< Session > SessionManager::GetSession(SessionId id) noexcept
{
    auto findSessionResult = table_.find(id);
    if (findSessionResult != table_.end()) {
        return findSessionResult->second;
    }
    return {};
}

}  // namespace server
}  // namespace uds
}  // namespace isoft