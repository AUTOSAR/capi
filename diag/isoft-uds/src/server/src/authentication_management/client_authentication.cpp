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
/// @file       client_authentication.cpp
/// @brief      This file implements client_authentication functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=authentication_management
/// @trace_id_sr=e704298f-44ac-434c-83b3-854cd3505484
/// @unit_name=ClientAuthentication
/// @unit_description=This file provides client_authentication functionality
/// @interface_level=unit
/// @endcode
///
/// ================================================================

#include "client_authentication.h"

#include "common/log.h"
#include "common/serialize.h"
namespace isoft {
namespace uds {
namespace server {
ClientAuthentication::ClientAuthentication(std::uint16_t address) : address_{address} {}

std::int32_t ClientAuthentication::Initialize(std::shared_ptr< SessionManager > &sessionManagerPtr,
                                              AuthRoleTable authRoleTableConfig,
                                              std::chrono::milliseconds authTimeout) noexcept
{
    if (sessionManagerPtr.get() != nullptr) {
        NotifyOfSessionChangeCallback callback{[this](SessionId, SessionId next) {
            if (next == 0x01U) {
                if (currentSessionId_ != next) {
                    currentSessionId_ = next;
                }
            } else {
                currentSessionId_ = next;
            }
        }};
        sessionManagerPtr->AddNotifyOfSessionChange(callback);
    }
    authRoleTableConfig_ = std::move(authRoleTableConfig);
    authTimeout_         = authTimeout;
    aliveTimer_.Initialize([this]() {
        LogInfo() << "ClientAuthentication::Initialize| timeout!";
        _defaultState();
    });
    return 0;
}

void ClientAuthentication::Authenticate(std::vector< AuthRoleName > &userRoles) noexcept
{
    std::string strRoleList{};
    for (auto &&userRole : userRoles) {
        strRoleList.append(userRole);
        strRoleList.append(", ");
    }

    LogInfo() << "ClientAuthentication::Authenticat| role:" << strRoleList;
    state_                = AuthState::kAuthenticated;
    overrideDefaultState_ = false;
    currentAuthRoleTable_.clear();
    serviceDynamicAccessList_.clear();
    for (auto &&userRole : userRoles) {
        auto findResult = authRoleTableConfig_.find(userRole);
        if (findResult != authRoleTableConfig_.end()) {
            currentAuthRoleTable_.insert({userRole, findResult->second});
        } else {
            LogWarn() << "ClientAuthentication::Authenticate|Please add new role in "
                         "init config! userRole:"
                      << userRole;
        }
    }
    if (currentAuthRoleTable_.empty()) {
        LogWarn() << "ClientAuthentication::Authenticate|add role fail, please "
                     "check role name!";
        return;
    }
    _notify();
    //Refresh();
}

void ClientAuthentication::SetServiceDynamicAccessList(ServiceDynamicAccessList serviceDynamicAccesslist) noexcept
{
    LogInfo() << "ClientAuthentication::SetServiceDynamicAccessList| list size:" << serviceDynamicAccesslist.size();
    for (auto &&serviceDynamicAccess : serviceDynamicAccesslist) {
        LogInfo() << "ClientAuthentication::SetServiceDynamicAccessList| rule size:"
                  << serviceDynamicAccess.ruleList.size();
    }

    serviceDynamicAccessList_ = std::move(serviceDynamicAccesslist);
}

void ClientAuthentication::AppendServiceDynamicAccessList(ServiceDynamicAccessList serviceDynamicAccesslist) noexcept
{
    serviceDynamicAccessList_.insert(serviceDynamicAccessList_.end(), serviceDynamicAccesslist.begin(),
                                     serviceDynamicAccesslist.end());
}
void ClientAuthentication::ClearServiceDynamicAccessList() noexcept { serviceDynamicAccessList_.clear(); }

void ClientAuthentication::OverrideDefaultState(std::vector< AuthRoleName > &userRoles,
                                                std::chrono::milliseconds timeout) noexcept
{
    state_                  = AuthState::kDeAuthenticated;
    overrideDefaultState_   = true;
    overrideDefaultTimeout_ = timeout;
    currentAuthRoleTable_.clear();
    serviceDynamicAccessList_.clear();
    for (auto &&userRole : userRoles) {
        auto findResult = authRoleTableConfig_.find(userRole);
        if (findResult != authRoleTableConfig_.end()) {
            currentAuthRoleTable_.insert({userRole, findResult->second});
        } else {
            LogWarn() << "ClientAuthentication::OverrideDefaultState|Please add new "
                         "role in init config! userRole:"
                      << userRole;
        }
    }
    if (currentAuthRoleTable_.empty()) {
        LogWarn() << "ClientAuthentication::OverrideDefaultState|add role fail, "
                     "please check role name!";
        return;
    }
    _notify();
    //Refresh();
}

void ClientAuthentication::Start() noexcept { Refresh(); }

void ClientAuthentication::Stop() noexcept { aliveTimer_.Stop(); }

void ClientAuthentication::Refresh() noexcept
{
    aliveTimer_.Stop();
    LogInfo() << "ClientAuthentication::Refresh| current session:" << currentSessionId_;
    if (state_ == AuthState::kAuthenticated) {
        if (currentSessionId_ == 0x01) {
            std::uint32_t value{static_cast< std::uint32_t >(authTimeout_.count())};
            aliveTimer_.Start(value);
            LogInfo() << "ClientAuthentication::Refresh| start timer : " << value << "ms";
        }

    } else {
        if (overrideDefaultState_) {
            std::uint32_t value{static_cast< std::uint32_t >(overrideDefaultTimeout_.count())};
            aliveTimer_.Start(value);
        } else {
            LogVerbose() << "ClientAuthentication::Refresh|ignore";
        }
    }
    return;
}

void ClientAuthentication::Default() noexcept { _defaultState(); }

void ClientAuthentication::SetNotifyAuthState(NotifyAuthState notify) noexcept { notify_ = std::move(notify); }

AuthState ClientAuthentication::GetState() noexcept { return state_; }

std::uint8_t ClientAuthentication::Validate(MessagePtr &requestPtr, std::set< AuthRoleName > &supportAuthRoles) noexcept
{
    std::uint8_t result{static_cast< std::uint8_t >(NrcErrc::kConditionsNotCorrect)};
    if (requestPtr.get() == nullptr) {
        LogWarn() << "ClientAuthentication::Validate| invalid request!";
        return result;
    }
    for (auto &&authRole : supportAuthRoles) {
        auto findResult = currentAuthRoleTable_.find(authRole);
        if (findResult != currentAuthRoleTable_.end()) {
            result = 0;
            break;
        }
    }
    if (result != isoft::kSuccess) {
        LogInfo() << "ClientAuthentication::Validate|start dynamic data match";
        if (_match(requestPtr->GetBody()) >= 0) {
            result = 0;
        }
    }
    return result;
}

// Match packet (vector type)
int ClientAuthentication::_match(const std::vector< uint8_t > &packet) const
{
    return _match(packet.data(), packet.size());
}

// Match packet (raw pointer type)
int ClientAuthentication::_match(const std::uint8_t *data, std::size_t length) const
{
    if (data == nullptr && length > 0) {
        throw std::invalid_argument("Data pointer cannot be null");
    }

    // Check each rule

    for (size_t ruleIdx = 0; ruleIdx < serviceDynamicAccessList_.size(); ++ruleIdx) {
        const auto &rule = serviceDynamicAccessList_[ruleIdx];

        // Rule length must be consistent with packet length
        if (rule.ruleList.size() != length) {
            LogDebug() << "ClientAuthentication::_match| ruleList:" << rule.ruleList.size() << " length:" << length;
            continue;
        }

        // Check whether each byte matches
        bool match = true;
        for (size_t i = 0; i < length; ++i) {
            uint8_t byteVal  = data[i];
            const auto &item = rule.ruleList[i];

            switch (item.type) {
                case RuleType::kWildcard:
                    // Wildcard always matches
                    break;

                case RuleType::kRange:
                    if (!(byteVal >= item.minVal && byteVal <= item.maxVal)) {
                        match = false;
                        break;
                    }
                    break;

                case RuleType::kValue:
                    if (byteVal != item.minVal) {  // min and max are equal
                        match = false;
                        break;
                    }
                    break;
            }

            if (!match) {
                break;
            }
        }

        if (match) {
            return static_cast< int >(ruleIdx);
        }
    }

    return -1;  // No matching rule
}

void ClientAuthentication::_defaultState() noexcept
{
    state_                = AuthState::kDeAuthenticated;
    overrideDefaultState_ = false;
    currentAuthRoleTable_.clear();
    serviceDynamicAccessList_.clear();
    for (auto &&authRole : authRoleTableConfig_) {
        if (authRole.second.isDefault) {
            currentAuthRoleTable_.insert({authRole.first, authRole.second});
        }
    }
    _notify();
}

void ClientAuthentication::_notify() noexcept
{
    if (notify_) {
        notify_(state_);
        if (state_ == AuthState::kAuthenticated) {
            LogInfo() << "ClientAuthentication::_notify|state:kAuthenticated";
        } else {
            LogInfo() << "ClientAuthentication::_notify|state:kDeauthenticated";
        }
    }
}
}  // namespace server
}  // namespace uds
}  // namespace isoft
