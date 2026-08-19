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
/// @file       client_authentication.h
/// @brief      This file provides client_authentication functionality
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
/// @trace_id_sr=e704298f-44ac-434c-83b3-854cd3505484
/// @unit_name=ClientAuthentication
/// @unit_description=This file provides client_authentication functionality
/// @endcode
///
/// ================================================================

#ifndef ISOFT_UDS_SERVER_CLIENT_AUTHENTICATION_H_
#define ISOFT_UDS_SERVER_CLIENT_AUTHENTICATION_H_
#include <isoft/uds/authentication_management/authentication_state.h>
#include <isoft/uds/authentication_management/service_dynamic_access_list.h>
#include <isoft/uds/message.h>

#include <chrono>
#include <functional>
#include <map>
#include <string>
#include <vector>

#include "common/timer.h"
#include "session_management/session_manager.h"
namespace isoft {
namespace uds {
namespace server {

/// @brief Authentication status notification function
/// @param state Parameter
/// @return Return value
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02445
/// @needwork = dd
/// @endcode
using NotifyAuthState = std::function< void(AuthState) >;

/// @brief Client authentication class
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02446
/// @needwork = dd
/// @endcode
class ClientAuthentication
{
public:
    /// @brief ClientAuthentication function
    /// @param address Parameter
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00703
    /// @needwork = dda
    /// @endcode

    /// @brief ClientAuthentication function
    /// @param address Parameter
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01382
    /// @needwork = dd
    /// @endcode
    explicit ClientAuthentication(std::uint16_t address);

    /// @brief Destructor
    /// @param
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01383
    /// @needwork = dd
    /// @endcode
    ~ClientAuthentication() = default;

    /// @param
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00704
    /// @needwork = dda
    /// @endcode
    ClientAuthentication(ClientAuthentication const &) = delete;
    /// @param
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00705
    /// @needwork = dda
    /// @endcode
    ClientAuthentication &operator=(ClientAuthentication const &) = delete;
    /// @param
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00706
    /// @needwork = dda
    /// @endcode
    ClientAuthentication(ClientAuthentication &&) = delete;
    /// @param
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00707
    /// @needwork = dda
    /// @endcode
    ClientAuthentication &operator=(ClientAuthentication &&) = delete;

    /// @brief Initialization function
    /// @param sessionManagerPtr Parameter
    /// @param authRoleTableConfig Parameter
    /// @param authTimeout Parameter
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01384
    /// @needwork = dd
    /// @endcode
    std::int32_t Initialize(std::shared_ptr< SessionManager > &sessionManagerPtr,
                            AuthRoleTable authRoleTableConfig,
                            std::chrono::milliseconds authTimeout) noexcept;

    /// @brief Set authentication status and assign related roles
    /// @param userRoles Parameter
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01385
    /// @needwork = dd
    /// @endcode
    void Authenticate(std::vector< AuthRoleName > &userRoles) noexcept;

    /// @brief Set ServiceDynamicAccessList
    /// @param serviceDynamicAccesslist Parameter
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00708
    /// @needwork = dda
    /// @endcode

    /// @brief Set service dynamic access list
    /// @param serviceDynamicAccesslist Parameter
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01386
    /// @needwork = dd
    /// @endcode
    void SetServiceDynamicAccessList(ServiceDynamicAccessList serviceDynamicAccesslist) noexcept;

    /// @brief Append service dynamic access list
    /// @param serviceDynamicAccesslist Parameter
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01387
    /// @needwork = dd
    /// @endcode
    void AppendServiceDynamicAccessList(ServiceDynamicAccessList serviceDynamicAccesslist) noexcept;

    /// @brief Clear service dynamic access list
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01388
    /// @needwork = dd
    /// @endcode
    void ClearServiceDynamicAccessList() noexcept;

    /// @brief Override default role
    /// @param userRoles Parameter
    /// @param timeout Parameter
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01389
    /// @needwork = dd
    /// @endcode
    void OverrideDefaultState(std::vector< AuthRoleName > &userRoles, std::chrono::milliseconds timeout) noexcept;

    /// @brief Refresh authentication status
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01390
    /// @needwork = dd
    /// @endcode
    void Refresh() noexcept;

    /// @brief Start authentication status
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01391
    /// @needwork = dd
    /// @endcode
    void Start() noexcept;

    /// @brief Stop authentication status
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01392
    /// @needwork = dd
    /// @endcode
    void Stop() noexcept;

    /// @brief Set default authentication status
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01393
    /// @needwork = dd
    /// @endcode
    void Default() noexcept;

    /// @brief Set notification authentication status
    /// @param notify Parameter
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01394
    /// @needwork = dd
    /// @endcode
    void SetNotifyAuthState(NotifyAuthState notify) noexcept;

    /// @brief Get authentication status
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01395
    /// @needwork = dd
    /// @endcode
    AuthState GetState() noexcept;

    /// @brief Verify authentication status
    /// @param requestPtr Parameter
    /// @param supportAuthRoles Parameter
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01396
    /// @needwork = dd
    /// @endcode
    std::uint8_t Validate(MessagePtr &requestPtr, std::set< AuthRoleName > &supportAuthRoles) noexcept;

private:
    // Match packet (vector type)
    /// @brief Verify whether the packet is valid
    /// @param std Parameter
    /// @return Return code
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01397
    /// @needwork = dd
    /// @endcode
    int _match(const std::vector< uint8_t > &packet) const;

    // Match packet (raw pointer type)
    /// @brief Verify whether the packet is valid
    /// @param std Parameter
    /// @param length Parameter
    /// @return Return code
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01398
    /// @needwork = dd
    /// @endcode
    int _match(const std::uint8_t *data, std::size_t length) const;

    /// @brief Set default status
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01399
    /// @needwork = dd
    /// @endcode
    void _defaultState() noexcept;
    /// @brief Notify authentication status
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01400
    /// @needwork = dd
    /// @endcode
    void _notify() noexcept;

    /// @brief Client address
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01401
    /// @needwork = dd
    /// @endcode
    std::uint16_t address_;

    /// @brief Authentication status
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01402
    /// @needwork = dd
    /// @endcode
    AuthState state_{AuthState::kDeAuthenticated};

    /// @brief Current session ID
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01403
    /// @needwork = dd
    /// @endcode
    SessionId currentSessionId_{0x01};

    /// @brief Current authentication role table
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01404
    /// @needwork = dd
    /// @endcode
    std::map< AuthRoleName, AuthRole > currentAuthRoleTable_{};

    /// @brief Authentication role table configuration
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01405
    /// @needwork = dd
    /// @endcode
    AuthRoleTable authRoleTableConfig_;

    /// @brief Service dynamic access list
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01406
    /// @needwork = dd
    /// @endcode
    ServiceDynamicAccessList serviceDynamicAccessList_;

    /// @brief Authentication timeout
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01407
    /// @needwork = dd
    /// @endcode
    std::chrono::milliseconds authTimeout_{};

    /// @brief Notify authentication status
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01408
    /// @needwork = dd
    /// @endcode
    NotifyAuthState notify_;

    /// @brief Session timeout timer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01409
    /// @needwork = dd
    /// @endcode
    Timer aliveTimer_;

    /// @brief Whether to override default status
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01410
    /// @needwork = dd
    /// @endcode
    bool overrideDefaultState_{false};

    /// @brief Session timeout overrides default timeout
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    std::chrono::milliseconds overrideDefaultTimeout_{};
};
/// @brief Client authentication pointer
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02447
/// @needwork = dd
/// @endcode
using ClientAuthenticationPtr = std::shared_ptr< ClientAuthentication >;
}  // namespace server
}  // namespace uds
}  // namespace isoft

#endif  /// ISOFT_UDS_SERVER_CLIENT_AUTHENTICATION_H_
