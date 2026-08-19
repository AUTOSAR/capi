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
/// @file       external_authentication.cpp
/// @brief
/// @details
/// @date       2025-09-22
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================

#include "external_authentication.h"

#include "serialization/serialization.h"
namespace ara {
namespace diag {
namespace dmd {
isoft::uds::server::ServiceDynamicAccessList Serialize(std::vector< std::uint8_t > &data) noexcept
{
    std::uint8_t const kByte      = 1U;
    std::uint8_t const kByteRange = 2U;
    std::uint8_t const kWildcard  = 3U;
    isoft::uds::server::ServiceDynamicAccessList accessList;

    while (!data.empty()) {
        std::size_t dynamicAccessSize{};
        std::vector< std::uint8_t > dynamicAccess;
        isoft::serialize::Deserialize(data, dynamicAccessSize);
        data.erase(data.begin(), data.begin() + sizeof(std::size_t));
        using Diff_t = std::vector< std::uint8_t >::difference_type;
        dynamicAccess.insert(dynamicAccess.begin(), data.begin(),
                             std::next(data.begin(), static_cast< Diff_t >(dynamicAccessSize)));
        data.erase(data.begin(), std::next(data.begin(), static_cast< Diff_t >(dynamicAccessSize)));
        isoft::uds::server::ServiceDynamicAccess access;
        for (size_t index = 0; index < dynamicAccess.size(); index++) {
            std::uint8_t type{dynamicAccess[index]};
            if (type == kByte) {
                ++index;
                std::uint8_t value = dynamicAccess[index];
                isoft::uds::server::RuleItem ruleItem{value};
                access.ruleList.push_back(ruleItem);
            } else if ((type == kByteRange)) {
                ++index;
                std::uint8_t min = dynamicAccess[index];
                ++index;
                std::uint8_t max = dynamicAccess[index];
                isoft::uds::server::RuleItem ruleItem{min, max};
                access.ruleList.push_back(ruleItem);
            } else if ((type == kWildcard)) {
                isoft::uds::server::RuleItem ruleItem{};
                access.ruleList.push_back(ruleItem);
            }
        }
        accessList.push_back(access);
    }
    return accessList;
}
ExternalAuthentication::ExternalAuthentication() : agentPtr_{nullptr} {}
std::uint32_t ExternalAuthentication::Initialize(uint32_t const &serviceInstanceId) noexcept
{
    agentPtr_ = std::make_shared< isoft::dm::dis::ExternalAuthenticationAgent >(serviceInstanceId);
    agentPtr_->RegisterHasClientAddressCallback([this](std::uint16_t address) { return _hasClientAddress(address); });
    agentPtr_->RegisterSetAuthStateCallback(
        [this](std::uint16_t address, bool isAuth, std::vector< std::string > roleList, std::uint64_t msecond) {
            return _setAuthState(address, isAuth, std::move(roleList), msecond);
        });
    agentPtr_->RegisterAddDynamicAccessListCallback(
        [this](std::uint16_t address, std::vector< std::uint8_t > dynamicAccessList) {
            return _addDynamicAccessList(address, std::move(dynamicAccessList));
        });
    agentPtr_->RegisterSetDynamicAccessListCallback(
        [this](std::uint16_t address, std::vector< std::uint8_t > dynamicAccessList) {
            return _setDynamicAccessList(address, std::move(dynamicAccessList));
        });
    agentPtr_->RegisterRevokeCallback([this](std::uint16_t address) { return _revoke(address); });
    agentPtr_->RegisterRefreshCallback([this](std::uint16_t address) { return _refresh(address); });

    agentPtr_->RegisterGetAllAddressCallback([this]() { return _getAllAddress(); });
    return 0;
}
void ExternalAuthentication::RegisterGetClientAuthenticationController(Callback callback) noexcept
{
    callbck_ = std::move(callback);
}

void ExternalAuthentication::RegisterGetAllClientAddress(GetAllClientAddressCallback callback) noexcept
{
    getAllClientAddress_ = std::move(callback);
}
bool ExternalAuthentication::_hasClientAddress(std::uint16_t address) noexcept
{
    std::shared_ptr< isoft::uds::server::ClientAuthenticationController > clientPtr = callbck_(address);
    if (clientPtr.get() != nullptr) {
        clientPtr->SetNotifier([this, address](isoft::uds::server::AuthState state) {
            bool isAuth{false};
            if (state == isoft::uds::server::AuthState::kAuthenticated) {
                isAuth = true;
            }
            agentPtr_->UpdateAuthState(address, isAuth);
        });
        return true;
    }
    return false;
}

void ExternalAuthentication::_setAuthState(std::uint16_t address,
                                           bool isAuth,
                                           std::vector< std::string > roleList,
                                           std::uint64_t msecond) noexcept
{
    std::shared_ptr< isoft::uds::server::ClientAuthenticationController > clientPtr = callbck_(address);
    if (clientPtr.get() == nullptr) {
        return;
    }
    if (isAuth) {
        clientPtr->Authenticate(std::move(roleList));
    } else {
        std::chrono::milliseconds mseconds{msecond};
        clientPtr->OverrideDefaultRoles(std::move(roleList), mseconds);
    }

    return;
}

void ExternalAuthentication::_addDynamicAccessList(std::uint16_t address,
                                                   std::vector< std::uint8_t > dynamicAccessList) noexcept
{
    std::shared_ptr< isoft::uds::server::ClientAuthenticationController > clientPtr = callbck_(address);
    if (clientPtr.get() == nullptr) {
        return;
    }
    isoft::uds::server::ServiceDynamicAccessList accessList{Serialize(dynamicAccessList)};

    clientPtr->Append(accessList);

    return;
}

void ExternalAuthentication::_setDynamicAccessList(std::uint16_t address,
                                                   std::vector< std::uint8_t > dynamicAccessList) noexcept
{
    std::shared_ptr< isoft::uds::server::ClientAuthenticationController > clientPtr = callbck_(address);
    if (clientPtr.get() == nullptr) {
        return;
    }
    isoft::uds::server::ServiceDynamicAccessList accessList{Serialize(dynamicAccessList)};

    clientPtr->Set(accessList);
}

void ExternalAuthentication::_revoke(std::uint16_t address) noexcept
{
    std::shared_ptr< isoft::uds::server::ClientAuthenticationController > clientPtr = callbck_(address);
    if (clientPtr.get() == nullptr) {
        return;
    }

    clientPtr->Revoke();
}

void ExternalAuthentication::_refresh(std::uint16_t address) noexcept
{
    std::shared_ptr< isoft::uds::server::ClientAuthenticationController > clientPtr = callbck_(address);
    if (clientPtr.get() == nullptr) {
        return;
    }

    clientPtr->Refresh();
}

std::vector< std::uint16_t > ExternalAuthentication::_getAllAddress() noexcept
{
    if (getAllClientAddress_) {
        return getAllClientAddress_();
    }
    return {};
}
}  // namespace dmd
}  // namespace diag
}  // namespace ara