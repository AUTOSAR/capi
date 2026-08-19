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
/// @file       client_authentication_handle_impl.h
/// @brief
/// @details
/// @date       2025-09-12
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================

#ifndef ISOFT_DM_CLIENT_AUTHENTICATION_HANDLE_IMPL_H_
#define ISOFT_DM_CLIENT_AUTHENTICATION_HANDLE_IMPL_H_

#include "ara/core/result.h"
#include "ara/diag/diagnostic_service_dynamic_access_list.h"
#include "gen_code/external_authentication/apiAgent/external_authentication_agent.h"
namespace isoft {
namespace dm {

/**
 * @brief Definition of the ClientAuthenticationHandleImpl which is returned to the
 * application when an AuthenticationState is set by the application
 *
 * @artraceid{SWS_DM_01145}@artracestatus{draft}
 * @aruptrace{RS_Diag_04251}
 */
class ClientAuthenticationHandleImpl final
{
public:
    ClientAuthenticationHandleImpl(std::uint16_t clientAddress,
                                   std::shared_ptr< isoft::dm::dic::ExternalAuthenticationAgent > agentPtr) noexcept;
    /**
   * @brief Destructor of ClientAuthenticationHandleImpl
   *
   * @artraceid{SWS_DM_01147}@artracestatus{draft}
   * @aruptrace{RS_Diag_04251}
   */
    ~ClientAuthenticationHandleImpl() noexcept = default;

    /**
   * @brief Move constructor of ClientAuthenticationHandleImpl
   *
   * @param[in] other Object to move-construct from
   *
   * @artraceid{SWS_DM_01148}@artracestatus{draft}
   * @aruptrace{RS_Diag_04251}
   */
    ClientAuthenticationHandleImpl(ClientAuthenticationHandleImpl&& other) noexcept = default;

    /**
   * @brief Move assignment operator of ClientAuthenticationHandleImpl
   *
   * @param[in] other Object to move-assign from.
   *
   * @artraceid{SWS_DM_01149}@artracestatus{draft}
   * @aruptrace{RS_Diag_04251}
   */
    auto operator=(ClientAuthenticationHandleImpl&& other) & noexcept -> ClientAuthenticationHandleImpl& = default;

    /**
   * @brief Copy constructor of ClientAuthenticationHandleImpl cannot be used
   *
   * @param[in] other Object to copy-construct from
   *
   * @artraceid{SWS_DM_01150}@artracestatus{draft}
   * @aruptrace{RS_Diag_04251}
   */
    ClientAuthenticationHandleImpl(ClientAuthenticationHandleImpl const& other) = delete;

    /**
   * @brief Copy assignment operator of CancellationHandler cannot be used
   *
   * @param[in] other Object to copy-assign from.
   *
   * @artraceid{SWS_DM_01151}@artracestatus{draft}
   * @aruptrace{RS_Diag_04251}
   */
    auto operator=(ClientAuthenticationHandleImpl const& other) -> ClientAuthenticationHandleImpl& = delete;

    /**
   * @brief This function is used by the application to append a
   * DynamicAccessList to the already existing DynamicAccessList of a Diagnostic
   * Conversation
   *
   * @returns void
   *
   * @param[in] dynamicAccessList The DynamicAccessList to be appended in the client
   * @arerror ara::diag::DiagErrc::kServiceNotAvailable
   *
   * @artraceid{SWS_DM_01152}@artracestatus{draft}
   * @aruptrace{RS_Diag_04251}
   */
    ara::core::Result< void > Append(ara::diag::DiagnosticServiceDynamicAccessList dynamicAccessList) noexcept;

    /**
   * @brief This function is used by the application to set/replace a
   * DynamicAccessList  of  a diagnostic conversation
   *
   * @returns void
   *
   * @param[in] dynamicAccessList The new DynamicAccessList to be set in the client
   * @arerror ara::diag::DiagErrc::kServiceNotAvailable
   *
   * @artraceid{SWS_DM_01153}@artracestatus{draft}
   * @aruptrace{RS_Diag_04251}
   */
    ara::core::Result< void > Set(ara::diag::DiagnosticServiceDynamicAccessList dynamicAccessList) noexcept;

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
    ara::core::Result< void > Revoke() noexcept;

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
    ara::core::Result< void > Refresh() noexcept;

private:
    std::uint16_t clientAddress_{};
    std::shared_ptr< isoft::dm::dic::ExternalAuthenticationAgent > agentPtr_{};
    ara::core::Vector< ara::diag::DiagnosticServiceDynamicAccessList > list_;
};

}  // namespace dm
}  // namespace isoft
#endif  ///ISOFT_DM_CLIENT_AUTHENTICATION_HANDLE_IMPL_H_
