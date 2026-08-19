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
/// @file       client_authentication_controller.h
/// @brief      This file provides client_authentication_controller functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/isoft/uds/authentication_management
/// @interface_level= module
/// @trace_id_sr=f0cf48d7-67b9-4f4f-ac7c-7127d2aa02c8
/// @unit_name=ClientAuthenticationController
/// @unit_description=This file provides client_authentication_controller functionality
/// @endcode
///
/// ================================================================

#ifndef ISOFT_UDS_SERVER_CLIENT_AUTHENTICATION_CONTROLLER_H_
#define ISOFT_UDS_SERVER_CLIENT_AUTHENTICATION_CONTROLLER_H_
#include <isoft/uds/result.h>

#include <chrono>
#include <functional>
#include <memory>
#include <vector>

#include "authentication_state.h"
#include "service_dynamic_access_list.h"
namespace isoft {
namespace uds {
namespace server {
/// @brief Class of client authentication controller
///
/// @code{.isoft}
///
/// @interface_level=unit
/// @needwork = ad
/// @endcode
class ClientAuthenticationController
{
public:
    /// @brief Constructor
    /// @code{.isoft}
    /// @note Constructor default implementation
    /// @interface_level=unit
    /// @needwork = ad
    /// @endcode
    ClientAuthenticationController() = default;
    /// @brief Destructor
    /// @code{.isoft}
    /// @note Destructor default implementation
    /// @interface_level=unit
    /// @needwork = ad
    /// @endcode
    virtual ~ClientAuthenticationController() = default;

    /// @brief Assignment constructor
    /// @param other
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00992
    /// @needwork = dda
    /// @endcode
    ClientAuthenticationController(ClientAuthenticationController const &) = default;
    /// @param
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00993
    /// @needwork = dda
    /// @endcode
    ClientAuthenticationController &operator=(ClientAuthenticationController const &) = default;
    /// @param
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00994
    /// @needwork = dda
    /// @endcode
    ClientAuthenticationController(ClientAuthenticationController &&) = default;
    /// @param
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00995
    /// @needwork = dda
    /// @endcode
    ClientAuthenticationController &operator=(ClientAuthenticationController &&) = default;
    /// @brief This method is provided for the application to temporarily change the default authentication role of the diagnostic server instance. The diagnostic services allowed in the defaultRoles passed through the parameter
    /// can now be accessed by the tester for the time period defined by the parameter timeout
    ///
    /// @param[in] defaultRoles Default roles requested by the application, used to set on the diagnostic client
    /// @param[in] timeout Timeout for the override request to remain active
    /// @return Operation result
    /// @code{.isoft}
    /// @note Application uses this method to temporarily change the default authentication role of the diagnostic server instance
    /// @retval ara::diag::DiagErrc::kServiceNotAvailable Service not available error
    /// @interface_level=unit
    /// @needwork = ad
    /// @artraceid{SWS_DM_01141}@artracestatus{draft}
    /// @aruptrace{RS_Diag_04251}
    /// @endcode
    virtual Result< void > OverrideDefaultRoles(std::vector< AuthRoleName > defaultRoles,
                                                std::chrono::milliseconds timeout) noexcept;

    /// @brief This method is provided for the application to report the authentication status of the diagnostic client
    ///
    /// @param[in] userRoles Authentication roles requested by the application, used to set on the diagnostic client
    /// @return Operation result
    /// @code{.isoft}
    /// @note Application uses this method to report the authentication status of the diagnostic client
    /// @retval ara::diag::DiagErrc::kServiceNotAvailable Service not available error
    /// @interface_level=unit
    /// @needwork = ad
    /// @endcode
    virtual Result< void > Authenticate(std::vector< AuthRoleName > userRoles) noexcept;

    /// @brief This method is provided for the application to query the authentication status of the diagnostic client
    ///
    /// @return Authentication status

    /// @code{.isoft}
    /// @note Application uses this method to query the authentication status of the diagnostic client
    /// @retval ara::diag::DiagErrc::kServiceNotAvailable Service not available error
    /// @interface_level=unit
    /// @needwork = ad
    /// @artraceid{SWS_DM_01143}@artracestatus{draft}
    /// @aruptrace{RS_Diag_04251}
    /// @endcode
    virtual Result< AuthState > GetState() const noexcept;

    /// @brief This method is provided for the application to set the authentication status change notification function
    ///
    /// @param[in] notifier Authentication status change notification function
    /// @return Operation result
    /// @code{.isoft}
    /// @note Application uses this method to set the authentication status change notification function
    /// @retval ara::diag::DiagErrc::kServiceNotAvailable Service not available error
    /// @interface_level=unit
    /// @needwork = ad
    /// @artraceid{SWS_DM_01144}@artracestatus{draft}
    /// @aruptrace{RS_Diag_04251}
    /// @endcode
    virtual Result< void > SetNotifier(std::function< void(AuthState) > notifier) noexcept;

    /// @brief This method is provided for the application to append the service dynamic access list to the dynamic access list of the diagnostic client
    ///
    /// @param[in] list Service dynamic access list
    /// @return Operation result
    /// @code{.isoft}
    /// @note Application uses this method to append the service dynamic access list to the dynamic access list of the diagnostic client
    /// @retval ara::diag::DiagErrc::kServiceNotAvailable Service not available error
    /// @interface_level=unit
    /// @needwork = ad
    /// @artraceid{SWS_DM_01145}@artracestatus{draft}
    /// @aruptrace{RS_Diag_04251}
    /// @endcode
    virtual Result< void > Append(ServiceDynamicAccessList list) noexcept;

    /// @brief This method is provided for the application to set the dynamic access list of the diagnostic client
    ///
    /// @param[in] list Service dynamic access list
    /// @return Operation result
    /// @param[in] list Service dynamic access list
    /// @return Operation result
    /// @code{.isoft}
    /// @note Application uses this method to set the dynamic access list of the diagnostic client
    /// @retval ara::diag::DiagErrc::kServiceNotAvailable Service not available error
    /// @retval ara::diag::DiagErrc::kServiceNotAvailable Service not available error
    /// @interface_level=unit
    /// @needwork = ad
    /// @artraceid{SWS_DM_01146}@artracestatus{draft}
    /// @aruptrace{RS_Diag_04251}
    /// @endcode
    virtual Result< void > Set(ServiceDynamicAccessList list) noexcept;

    /// @brief This method is provided for the application to revoke the dynamic access list of the diagnostic client
    ///
    /// @return Operation result
    /// @code{.isoft}
    /// @note Application uses this method to revoke the dynamic access list of the diagnostic client
    /// @retval ara::diag::DiagErrc::kServiceNotAvailable Service not available error
    /// @interface_level=unit
    /// @needwork = ad
    /// @artraceid{SWS_DM_01147}@artracestatus{draft}
    /// @aruptrace{RS_Diag_04251}
    /// @endcode
    virtual Result< void > Revoke() noexcept;

    /// @brief This method is provided for the application to refresh the dynamic access list of the diagnostic client
    ///
    /// @return Operation result
    /// @code{.isoft}
    /// @note Application uses this method to refresh the dynamic access list of the diagnostic client
    /// @retval ara::diag::DiagErrc::kServiceNotAvailable Service not available error
    /// @interface_level=unit
    /// @needwork = ad
    /// @artraceid{SWS_DM_01148}@artracestatus{draft}
    /// @aruptrace{RS_Diag_04251}
    /// @endcode
    virtual Result< void > Refresh() noexcept;
};
}  // namespace server
}  // namespace uds
}  // namespace isoft
#endif  // ISOFT_UDS_SERVER_CLIENT_AUTHENTICATION_CONTROLLER_H_