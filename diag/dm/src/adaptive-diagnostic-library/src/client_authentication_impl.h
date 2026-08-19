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
/// @file       client_authentication_impl.h
/// @brief
/// @details
/// @date       2025-09-12
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================

#ifndef ISOFT_DM_CLIENT_AUTHENTICATION_IMPL_H_
#define ISOFT_DM_CLIENT_AUTHENTICATION_IMPL_H_

#include <chrono>
#include <functional>

#include "ara/core/result.h"
#include "ara/core/string.h"
#include "client_authentication_handle_impl.h"

namespace isoft {
namespace dm {

/**
 * @brief Interface for the application to inform the Diagnostic Server instance
 * about the authentication states and the user roles that are currently
 * authenticated
 *
 * @artraceid{SWS_DM_01132}@artracestatus{draft}
 * @aruptrace{RS_Diag_04251}
 */
class ClientAuthenticationImpl final
{
public:
    /**
   * @brief Possible values of the Authentication State of the client
   *
   * @artraceid{SWS_DM_01133}@artracestatus{draft}
   * @aruptrace{RS_Diag_04251}
   */
    enum class DiagnosticAuthState : std::uint8_t
    {
        kDeAuthenticated = 0x00,  ///< No Diagnostic Clients are currently authenticated
        kAuthenticated   = 0x01   ///< A Diagnostic Client is currently authen ticated
    };

    /**
   * @brief The Supported values for the Diagnostic Authentication roles are
   * specified in the Diagnostic Extract
   *
   * @artraceid{SWS_DM_01134}@artracestatus{draft}
   * @aruptrace{RS_Diag_04251}
   */
    using DiagnosticAuthRole = ara::core::String;

    ClientAuthenticationImpl(std::uint16_t clientAddress,
                             std::shared_ptr< isoft::dm::dic::ExternalAuthenticationAgent > agentPtr) noexcept;
    /**
   * @brief Destructor of ClientAuthenticationImpl
   *
   * @artraceid{SWS_DM_01136}@artracestatus{draft}
   * @aruptrace{RS_Diag_04251}
   */
    ~ClientAuthenticationImpl() noexcept;

    /**
   * @brief Move constructor of ClientAuthenticationImpl
   *
   * @param[in] other Object to move-construct from
   *
   * @artraceid{SWS_DM_01137}@artracestatus{draft}
   * @aruptrace{RS_Diag_04251}
   */
    ClientAuthenticationImpl(ClientAuthenticationImpl &&other) noexcept;

    /**
   * @brief Move assignment operator of ClientAuthenticationImpl
   *
   * @param[in] other Object to move-assign from.
   *
   * @artraceid{SWS_DM_01138}@artracestatus{draft}
   * @aruptrace{RS_Diag_04251}
   */
    auto operator=(ClientAuthenticationImpl &&other) &noexcept -> ClientAuthenticationImpl &;

    /**
   * @brief Copy constructor of ClientAuthenticationImpl cannot be used
   *
   * @param[in] other Object to copy-construct from
   *
   * @artraceid{SWS_DM_01139}@artracestatus{draft}
   * @aruptrace{RS_Diag_04251}
   */
    ClientAuthenticationImpl(ClientAuthenticationImpl const &other) = delete;

    /**
   * @brief Copy assignment operator of ClientAuthenticationImpl cannot be used
   *
   * @param[in] other Object to copy-assign from.
   *
   * @artraceid{SWS_DM_01140}@artracestatus{draft}
   * @aruptrace{RS_Diag_04251}
   */
    auto operator=(ClientAuthenticationImpl const &other) -> ClientAuthenticationImpl & = delete;

    /**
   * @brief This method is used by the application to temporarily change the
   * default AuthenticationRole for a Diagnostic Server Instance. The diagnostic
   * services allowed in the passed defaultRoles are now accessible to the
   * tester for a time period defined in the parameter timeout
   *
   * @returns Operation result
   *
   * @param[in] defaultRoles The default roles requested by the application, to
   * be set on the diagnostic client
   * @param[in] timeout The timeout until which the override request is active
   * @arerror ara::diag::DiagErrc::kServiceNotAvailable
   *
   * @artraceid{SWS_DM_01141}@artracestatus{draft}
   * @aruptrace{RS_Diag_04251}
   */
    ara::core::Result< std::shared_ptr< ClientAuthenticationHandleImpl > > OverrideDefaultRoles(
        ara::core::Vector< DiagnosticAuthRole > &defaultRoles, std::chrono::milliseconds timeout);

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
    ara::core::Result< std::shared_ptr< ClientAuthenticationHandleImpl > > Authenticate(
        ara::core::Vector< DiagnosticAuthRole > &userRoles);

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
    ara::core::Result< DiagnosticAuthState > GetState() const noexcept;

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
    ara::core::Result< void > SetNotifier(std::function< void(DiagnosticAuthState) > notifier);

private:
    std::uint16_t clientAddress_{};
    std::shared_ptr< isoft::dm::dic::ExternalAuthenticationAgent > agentPtr_{};
    DiagnosticAuthState state_{DiagnosticAuthState::kDeAuthenticated};
    std::function< void(DiagnosticAuthState) > notifier_{};
};

}  // namespace dm
}  // namespace isoft

#endif  /// ISOFT_DM_CLIENT_AUTHENTICATION_IMPL_H_
