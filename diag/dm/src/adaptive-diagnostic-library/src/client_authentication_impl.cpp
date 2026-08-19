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
/// @file       client_authentication_impl.cpp
/// @brief
/// @details
/// @date       2025-09-12
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================

#include "client_authentication_impl.h"

namespace isoft {
namespace dm {
ClientAuthenticationImpl::ClientAuthenticationImpl(
    std::uint16_t clientAddress, std::shared_ptr< isoft::dm::dic::ExternalAuthenticationAgent > agentPtr) noexcept
    : clientAddress_{clientAddress}, agentPtr_{std::move(agentPtr)}
{
    agentPtr_->SetNotify(clientAddress_, [this](bool state) {
        if (state) {
            state_ = DiagnosticAuthState::kAuthenticated;
        } else {
            state_ = DiagnosticAuthState::kDeAuthenticated;
        }
        if (notifier_) {
            notifier_(state_);
        }
    });
}
ClientAuthenticationImpl::~ClientAuthenticationImpl() noexcept
{
    if (agentPtr_.get() != nullptr) {
        agentPtr_->RemoveNotify(clientAddress_);
    }
}

/**
 * @brief This method is used by the application to temporarily change the
 * default AuthenticationRole for a Diagnostic Server Instance. The diagnostic
 * services allowed in the passed defaultRoles are now accessible to the
 * tester for a time period defined in the parameter timeout
 *
 * @returns Operation result
 *
 * @param[in] defaultRoles The default roles requested by the application, to be
 * set on the diagnostic client
 * @param[in] timeout The timeout until which the override request is active
 * @arerror ara::diag::DiagErrc::kServiceNotAvailable
 *
 * @artraceid{SWS_DM_01141}@artracestatus{draft}
 * @aruptrace{RS_Diag_04251}
 */
ara::core::Result< std::shared_ptr< ClientAuthenticationHandleImpl > > ClientAuthenticationImpl::OverrideDefaultRoles(
    ara::core::Vector< DiagnosticAuthRole > &defaultRoles, std::chrono::milliseconds timeout)
{
    std::vector< std::string > roles{};
    for (auto &&defaultRole : defaultRoles) {
        roles.emplace_back(defaultRole.c_str());
    }

    agentPtr_->SetAuthState(clientAddress_, false, roles, timeout.count());
    return ara::core::Result< std::shared_ptr< ClientAuthenticationHandleImpl > >::FromValue(
        std::make_shared< ClientAuthenticationHandleImpl >(clientAddress_, agentPtr_));
}

/**
 * @brief This function is used by the application to report the authenticated
 * state to the Diagnostic Server instance. The authentication could be either
 * done using the Authentication interfaces of the Diagnostic Server instance,
 * or through other means in the application
 *
 * @returns A handler of the Authentication State, which can be used by the
 * application to set or extend the DynamicAccessList
 *
 * @param[in] userRoles The user roles to set on the diagnostic client
 * @arerror ara::diag::DiagErrc::kServiceNotAvailable
 *
 * @artraceid{SWS_DM_01142}@artracestatus{draft}
 * @aruptrace{RS_Diag_04251}
 */
ara::core::Result< std::shared_ptr< ClientAuthenticationHandleImpl > > ClientAuthenticationImpl::Authenticate(
    ara::core::Vector< DiagnosticAuthRole > &userRoles)
{
    std::vector< std::string > roles{};
    for (auto &&userRole : userRoles) {
        roles.emplace_back(userRole.c_str());
    }
    agentPtr_->SetAuthState(clientAddress_, true, roles, 0);
    return ara::core::Result< std::shared_ptr< ClientAuthenticationHandleImpl > >::FromValue(
        std::make_shared< ClientAuthenticationHandleImpl >(clientAddress_, agentPtr_));
}

/**
 * @brief This function is used by the application to query the current
 * authentication state of the diagnostic client
 *
 * @returns The Authentication State of the Diagnostic Client or error
 * @arerror ara::diag::DiagErrc::kServiceNotAvailable
 *
 * @artraceid{SWS_DM_01143}@artracestatus{draft}
 * @aruptrace{RS_Diag_04251}
 */
ara::core::Result< ClientAuthenticationImpl::DiagnosticAuthState > ClientAuthenticationImpl::GetState() const noexcept
{
    return ara::core::Result< ClientAuthenticationImpl::DiagnosticAuthState >::FromValue(state_);
}

/**
 * @brief This function is used by the application to set a Notifier Function
 * that shall be called by the Diagnostic Server instance when an
 * Authentication Status Change Occurs. This may be used, for. E.g, to notify
 * the application when a transition to kDeAuthenticated  State occurred due
 * to an S3 timeout. A consecutive call of this method will overwrite the
 * previous registered notifier.
 *
 * @returns void
 *
 * @param[in] notifier The notifier to call on state transition
 *
 * @artraceid{SWS_DM_01144}@artracestatus{draft}
 * @aruptrace{RS_Diag_04251}
 */
ara::core::Result< void > ClientAuthenticationImpl::SetNotifier(std::function< void(DiagnosticAuthState) > notifier)
{
    notifier_ = std::move(notifier);
    return {};
}
}  // namespace dm
}  // namespace isoft
