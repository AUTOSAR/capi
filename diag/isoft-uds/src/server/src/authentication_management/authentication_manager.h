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
/// @file       authentication_manager.h
/// @brief      This file provides authentication_manager functionality
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
/// @trace_id_sr=8d0228f3-6205-47db-a0a5-bd8ad38a80e8
/// @unit_name=AuthenticationManager
/// @unit_description=This file provides authentication_manager functionality
/// @endcode
///
/// ================================================================

#ifndef ISOFT_UDS_SERVER_AUTHENTICATION_MANAGER_H_
#define ISOFT_UDS_SERVER_AUTHENTICATION_MANAGER_H_
#include <isoft/uds/authentication_management/authentication_management.h>

#include "client_authentication.h"
#include "session_management/session_manager.h"
namespace isoft {
namespace uds {
namespace server {

/// @brief Authentication manager
/// @interface_level=unit
/// @needwork=dd
/// @endcode
class AuthenticationManager
{
public:
    /// @param
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00719
    /// @needwork = dda
    /// @endcode
    AuthenticationManager(AuthenticationManager const &) = delete;
    /// @param
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00720
    /// @needwork = dda
    /// @endcode
    AuthenticationManager &operator=(AuthenticationManager const &) = delete;
    /// @param
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00721
    /// @needwork = dda
    /// @endcode
    AuthenticationManager(AuthenticationManager &&) = delete;
    /// @param
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00722
    /// @needwork = dda
    /// @endcode
    AuthenticationManager &operator=(AuthenticationManager &&) = delete;

public:
    /// @brief Constructor
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01438
    /// @needwork = dd
    /// @endcode
    AuthenticationManager() = default;

    /// @brief Destructor
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01439
    /// @needwork = dd
    /// @endcode
    ~AuthenticationManager() = default;

    /// @brief Initialize authentication manager
    /// @param sessionManagerPtr Parameter
    /// @param authRoleList Parameter
    /// @param authenticationTimeout Parameter
    /// @param externalAuthenticationList Parameter
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01440
    /// @needwork = dd
    /// @endcode
    std::int32_t Initialize(std::shared_ptr< SessionManager > sessionManagerPtr,
                            std::vector< AuthRole > const &authRoleList,
                            std::uint32_t authenticationTimeout,
                            std::vector< ExternalAuthenticationIdentification > externalAuthenticationList) noexcept;

    /// @brief Get client authentication
    /// @param clientAddress Parameter
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01441
    /// @needwork = dd
    /// @endcode
    Result< ClientAuthenticationPtr > GetClientAuthentication(Address const &clientAddress) noexcept;

    /// @brief Get all client authentication addresses
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01442
    /// @needwork = dd
    /// @endcode
    Result< std::vector< Address > > GetAllClientAuthenticationAddress() noexcept;

    /// @brief Set VerifyCertificateState
    /// @param clientAddress Parameter
    /// @param state Parameter
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01443
    /// @needwork = dd
    /// @endcode
    void SetVerifyCertificateState(Address clientAddress, bool state) noexcept;

    /// @brief Get VerifyCertificateState
    /// @param clientAddress Parameter
    /// @return Operation result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01444
    /// @needwork = dd
    /// @endcode
    bool GetVerifyCertificateState(Address clientAddress) noexcept;

private:
    /// @brief Client authentication table
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01445
    /// @needwork = dd
    /// @endcode
    std::map< Address, ClientAuthenticationPtr > clientAuthenticationTable_{};
    /// @brief Verify client certificate status table
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01446
    /// @needwork = dd
    /// @endcode
    std::map< Address, bool > verifyCertificateStateTable_{};
    /// @brief Authentication role list
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01447
    /// @needwork = dd
    /// @endcode
    std::vector< AuthRole > authRoleList_{};
    /// @brief Authentication timeout
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01448
    /// @needwork = dd
    /// @endcode
    std::uint32_t authenticationTimeout_{};
    /// @brief Session manager pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01449
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< SessionManager > sessionManagerPtr_{};
    /// @brief External authentication list
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01450
    /// @needwork = dd
    /// @endcode
    std::vector< ExternalAuthenticationIdentification > externalAuthenticationList_{};
};

}  // namespace server
}  // namespace uds
}  // namespace isoft
#endif  // ISOFT_UDS_SERVER_AUTHENTICATION_MANAGEMENT_H_