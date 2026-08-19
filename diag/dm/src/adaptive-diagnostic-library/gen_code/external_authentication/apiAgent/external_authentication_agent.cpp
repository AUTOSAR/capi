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
/// @file       external_authentication_agent.cpp
/// @brief
/// @details
/// @date       2025-09-12
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================

#include "external_authentication_agent.h"

namespace isoft {
namespace dm {
namespace dic {

ExternalAuthenticationAgent::ExternalAuthenticationAgent(uint16_t const &instanceId, uint32_t const &serviceInstanceId)
    : proxy_{std::make_unique< ExternalAuthenticationProxy >(instanceId, serviceInstanceId)}
{
}

void ExternalAuthenticationAgent::SetReentrancyType(ara::diag::ReentrancyType const &reentrancyType) noexcept
{
    proxy_->SetReentrancyType(reentrancyType);
}

bool ExternalAuthenticationAgent::HasClientAddress(std::uint16_t address) noexcept
{
    return proxy_->HasClientAddress(address);
}

void ExternalAuthenticationAgent::SetAuthState(std::uint16_t address,
                                               bool isAuth,
                                               std::vector< std::string > roleList,
                                               std::uint64_t msecond) noexcept
{
    return proxy_->SetAuthState(address, isAuth, roleList, msecond);
}

bool ExternalAuthenticationAgent::SetNotify(std::uint16_t address,
                                            std::function< void(bool) > notify) noexcept  /// NOLINT
{
    bool result{false};
    if (!isNotify_) {
        result = proxy_->SetNotify([this](std::uint16_t addressNotify, bool state) {
            std::unique_lock< std::mutex > lock{life_};
            auto find = notifyTable_.find(addressNotify);
            if (find != notifyTable_.end()) {
                if (find->second) {
                    find->second(state);
                }
            }
        });
        if (result) {
            isNotify_ = true;
            notifyTable_.insert({address, notify});
        }
    } else {
        result = true;
        notifyTable_.insert({address, notify});
    }
    return result;
}

void ExternalAuthenticationAgent::RemoveNotify(std::uint16_t address) noexcept
{
    std::unique_lock< std::mutex > lock{life_};
    auto result = notifyTable_.find(address);
    if (result != notifyTable_.end()) {
        notifyTable_.erase(address);
    }
    return;
}

void ExternalAuthenticationAgent::AddDynamicAccessList(std::uint16_t address,
                                                       std::vector< std::uint8_t > dynamicAccessList) noexcept
{
    return proxy_->AddDynamicAccessList(address, std::move(dynamicAccessList));
}

void ExternalAuthenticationAgent::SetDynamicAccessList(std::uint16_t address,
                                                       std::vector< std::uint8_t > dynamicAccessList) noexcept
{
    return proxy_->SetDynamicAccessList(address, std::move(dynamicAccessList));
}

void ExternalAuthenticationAgent::Revoke(std::uint16_t address) noexcept { return proxy_->Revoke(address); }

void ExternalAuthenticationAgent::Refresh(std::uint16_t address) noexcept { return proxy_->Refresh(address); }

std::vector< std::uint16_t > ExternalAuthenticationAgent::GetAllAddress() noexcept { return proxy_->GetAllAddress(); }
}  // namespace dic
}  // namespace dm
}  // namespace isoft