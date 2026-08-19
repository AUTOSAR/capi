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
/// @file       client_authentication_handle_impl.cpp
/// @brief
/// @details
/// @date       2025-09-12
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================

#include "client_authentication_handle_impl.h"
namespace isoft {
namespace dm {
ClientAuthenticationHandleImpl::ClientAuthenticationHandleImpl(
    std::uint16_t clientAddress, std::shared_ptr< isoft::dm::dic::ExternalAuthenticationAgent > agentPtr) noexcept
    : clientAddress_{clientAddress}, agentPtr_{std::move(agentPtr)}
{
}

/**
 * @brief This function is used by the application to append a
 * DynamicAccessList to the already existing DynamicAccessList of a Diagnostic
 * Conversation
 *
 * @returns void
 *
 * @param[in] dynamicAccessList The DynamicAccessList to be appended in the
 * client
 * @arerror ara::diag::DiagErrc::kServiceNotAvailable
 *
 * @artraceid{SWS_DM_01152}@artracestatus{draft}
 * @aruptrace{RS_Diag_04251}
 */
ara::core::Result< void > ClientAuthenticationHandleImpl::Append(
    ara::diag::DiagnosticServiceDynamicAccessList dynamicAccessList) noexcept
{
    list_.push_back(dynamicAccessList);
    std::vector< std::uint8_t > raw;
    for (auto &&dynamicAccess : dynamicAccessList.list_) {
        std::vector< std::uint8_t > temp;
        isoft::serialize::Serialize(temp, dynamicAccess.size());
        raw.insert(raw.end(), temp.begin(), temp.end());
        raw.insert(raw.end(), dynamicAccess.begin(), dynamicAccess.end());
    }
    agentPtr_->AddDynamicAccessList(clientAddress_, raw);
    return {};
}

/**
 * @brief This function is used by the application to set/replace a
 * DynamicAccessList  of  a diagnostic conversation
 *
 * @returns void
 *
 * @param[in] dynamicAccessList The new DynamicAccessList to be set in the
 * client
 * @arerror ara::diag::DiagErrc::kServiceNotAvailable
 *
 * @artraceid{SWS_DM_01153}@artracestatus{draft}
 * @aruptrace{RS_Diag_04251}
 */
ara::core::Result< void > ClientAuthenticationHandleImpl::Set(
    ara::diag::DiagnosticServiceDynamicAccessList dynamicAccessList) noexcept
{
    list_.clear();
    list_.push_back(dynamicAccessList);
    std::vector< std::uint8_t > raw;
    for (auto &&dynamicAccess : dynamicAccessList.list_) {
        std::vector< std::uint8_t > temp;
        isoft::serialize::Serialize(temp, dynamicAccess.size());
        raw.insert(raw.end(), temp.begin(), temp.end());
        raw.insert(raw.end(), dynamicAccess.begin(), dynamicAccess.end());
    }
    agentPtr_->SetDynamicAccessList(clientAddress_, raw);

    return {};
}

/**
 * @brief This function is used by the application to de-authenticate a
 * client, and also to clear the DynamicAccessList and any overridden defaults
 *
 * @returns void
 * @arerror ara::diag::DiagErrc::kServiceNotAvailable
 *
 * @artraceid{SWS_DM_01154}@artracestatus{draft}
 * @aruptrace{RS_Diag_04251}
 */
ara::core::Result< void > ClientAuthenticationHandleImpl::Revoke() noexcept
{
    agentPtr_->Revoke(clientAddress_);
    return {};
}

/**
 * @brief This function is used by the application to refresh the timer that
 * was started by Authenticate or OverrideDefaultRoles. If both Methods were
 * previously called, both timers are refreshed
 *
 * @returns void
 * @arerror ara::diag::DiagErrc::kServiceNotAvailable
 *
 * @artraceid{SWS_DM_01155}@artracestatus{draft}
 * @aruptrace{RS_Diag_04251}
 */
ara::core::Result< void > ClientAuthenticationHandleImpl::Refresh() noexcept
{
    agentPtr_->Refresh(clientAddress_);
    return {};
}
}  // namespace dm
}  // namespace isoft
