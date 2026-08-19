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
/// @file       client_authentication_handle.h
/// @brief
/// @details
/// @date       2025-09-12
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_DIAG_CLIENT_AUTHENTICATION_HANDLE_H_
#define ARA_DIAG_CLIENT_AUTHENTICATION_HANDLE_H_

#include "ara/core/result.h"
#include "ara/diag/diagnostic_service_dynamic_access_list.h"

namespace isoft {
namespace dm {
class ClientAuthenticationHandleImpl;
}  // namespace dm
}  // namespace isoft
namespace ara {
namespace diag {
class ClientAuthentication;
/**
 * @brief Definition of the ClientAuthenticationHandle which is returned to the
 * application when an AuthenticationState is set by the application
 *
 * @artraceid{SWS_DM_01145}@artracestatus{draft}
 * @aruptrace{RS_Diag_04251}
 */
class ClientAuthenticationHandle final
{
public:
    /**
   * @brief Constructor of ClientAuthenticationHandle
   *
   * @artraceid{SWS_DM_01146}@artracestatus{draft}
   * @aruptrace{RS_Diag_04251}
   */
    ClientAuthenticationHandle();

    /**
   * @brief Destructor of ClientAuthenticationHandle
   *
   * @artraceid{SWS_DM_01147}@artracestatus{draft}
   * @aruptrace{RS_Diag_04251}
   */
    ~ClientAuthenticationHandle() noexcept = default;

    /**
   * @brief Move constructor of ClientAuthenticationHandle
   *
   * @param[in] other Object to move-construct from
   *
   * @artraceid{SWS_DM_01148}@artracestatus{draft}
   * @aruptrace{RS_Diag_04251}
   */
    ClientAuthenticationHandle(ClientAuthenticationHandle &&other) noexcept = default;

    /**
   * @brief Move assignment operator of ClientAuthenticationHandle
   *
   * @param[in] other Object to move-assign from.
   *
   * @artraceid{SWS_DM_01149}@artracestatus{draft}
   * @aruptrace{RS_Diag_04251}
   */
    auto operator=(ClientAuthenticationHandle &&other) &noexcept -> ClientAuthenticationHandle & = default;

    /**
   * @brief Copy constructor of ClientAuthenticationHandle cannot be used
   *
   * @param[in] other Object to copy-construct from
   *
   * @artraceid{SWS_DM_01150}@artracestatus{draft}
   * @aruptrace{RS_Diag_04251}
   */
    ClientAuthenticationHandle(ClientAuthenticationHandle const &other) = delete;

    /**
   * @brief Copy assignment operator of CancellationHandler cannot be used
   *
   * @param[in] other Object to copy-assign from.
   *
   * @artraceid{SWS_DM_01151}@artracestatus{draft}
   * @aruptrace{RS_Diag_04251}
   */
    auto operator=(ClientAuthenticationHandle const &other) -> ClientAuthenticationHandle & = delete;

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
    ara::core::Result< void > Append(DiagnosticServiceDynamicAccessList dynamicAccessList) noexcept;

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
    ara::core::Result< void > Set(DiagnosticServiceDynamicAccessList dynamicAccessList) noexcept;

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
    friend ClientAuthentication;
    explicit ClientAuthenticationHandle(std::shared_ptr< isoft::dm::ClientAuthenticationHandleImpl > implPtr) noexcept;
    std::shared_ptr< isoft::dm::ClientAuthenticationHandleImpl > implPtr_{};
};

}  // namespace diag
}  // namespace ara

#endif
