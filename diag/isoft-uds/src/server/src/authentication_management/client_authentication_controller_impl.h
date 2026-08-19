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
/// @file       client_authentication_controller_impl.h
/// @brief      This file provides client_authentication_controller_impl functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/isoft/uds/authentication_management
/// @interface_level=software
/// @trace_id_sr=682f72ca-02bd-4ab8-9a5c-1982f408c96d
/// @unit_name=ClientAuthenticationControllerImpl
/// @unit_description=This file provides client_authentication_controller_impl functionality
/// @endcode
///
/// ================================================================

#ifndef ISOFT_UDS_SERVER_CLIENT_AUTHENTICATION_CONTROLLER_IMPL_H_
#define ISOFT_UDS_SERVER_CLIENT_AUTHENTICATION_CONTROLLER_IMPL_H_
#include <isoft/uds/authentication_management/client_authentication_controller.h>

#include "client_authentication.h"
namespace isoft {
namespace uds {
namespace server {

/// @brief ClientAuthenticationControllerImpl class
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02448
/// @needwork = dd
/// @endcode
class ClientAuthenticationControllerImpl : public ClientAuthenticationController
{
public:
    /// @brief ClientAuthenticationControllerImpl function
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01411
    /// @needwork = dd
    /// @endcode
    ClientAuthenticationControllerImpl(ClientAuthenticationControllerImpl const &) = default;

    /// @brief ClientAuthenticationControllerImpl function
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01412
    /// @needwork = dd
    /// @endcode
    ClientAuthenticationControllerImpl &operator=(ClientAuthenticationControllerImpl const &) = default;

    /// @brief ClientAuthenticationControllerImpl function
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01413
    /// @needwork = dd
    /// @endcode
    ClientAuthenticationControllerImpl(ClientAuthenticationControllerImpl &&) = default;

    /// @brief ClientAuthenticationControllerImpl function
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01414
    /// @needwork = dd
    /// @endcode
    ClientAuthenticationControllerImpl &operator=(ClientAuthenticationControllerImpl &&) = default;

    /// @brief ClientAuthenticationControllerImpl function
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01415
    /// @needwork = dd
    /// @endcode
    explicit ClientAuthenticationControllerImpl(std::shared_ptr< ClientAuthentication > clientAuthPtr);

    /// @brief ClientAuthenticationControllerImpl function
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01416
    /// @needwork = dd
    /// @endcode
    ~ClientAuthenticationControllerImpl() override = default;

    /// @brief Override default role
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01417
    /// @needwork = dd
    /// @endcode
    Result< void > OverrideDefaultRoles(std::vector< AuthRoleName > defaultRoles,
                                        std::chrono::milliseconds timeout) noexcept override;

    /// @brief Set authentication status and assign related roles
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01418
    /// @needwork = dd
    /// @endcode
    Result< void > Authenticate(std::vector< AuthRoleName > userRoles) noexcept override;

    /// @brief Get authentication status
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01419
    /// @needwork = dd
    /// @endcode
    Result< AuthState > GetState() const noexcept override;

    /// @brief Set authentication status change notification function
    /// @param const notifier Authentication status change notification function
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01420
    /// @needwork = dd
    /// @endcode
    Result< void > SetNotifier(std::function< void(AuthState) > notifier) noexcept override;

    /// @brief Append dynamic access list
    /// @param const list Dynamic access list
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01421
    /// @needwork = dd
    /// @endcode
    Result< void > Append(ServiceDynamicAccessList list) noexcept override;

    /// @brief Set dynamic access list
    /// @param const list Dynamic access list
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01422
    /// @needwork = dd
    /// @endcode
    Result< void > Set(ServiceDynamicAccessList list) noexcept override;

    /// @brief Revoke dynamic access list
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01423
    /// @needwork = dd
    /// @endcode
    Result< void > Revoke() noexcept override;

    /// @brief Refresh dynamic access list
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01424
    /// @needwork = dd
    /// @endcode
    Result< void > Refresh() noexcept override;

private:
    /// @brief Client authentication pointer
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01425
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< ClientAuthentication > clientAuthPtr_;
};
}  // namespace server
}  // namespace uds
}  // namespace isoft
#endif  // ISOFT_UDS_SERVER_CLIENT_AUTHENTICATION_CONTROLLER_IMPL_H_