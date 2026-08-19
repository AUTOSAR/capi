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
/// @file       authentication_state.h
/// @brief      This file provides authentication_state functionality
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
/// @trace_id_sr=
/// @unit_name=AuthenticationState
/// @unit_description=This file provides authentication_state functionality
/// @endcode
///
/// ================================================================

#ifndef ISOFT_UDS_SERVER_AUTHENTICATION_STATE_H_
#define ISOFT_UDS_SERVER_AUTHENTICATION_STATE_H_
#include <cstdint>
#include <map>
#include <set>
#include <string>
namespace isoft {
namespace uds {
namespace server {
/// @brief Enumeration of authentication states
///
/// @code{.isoft}
///
/// @interface_level=unit
/// @needwork = ad
///
/// @endcode
enum class AuthState : std::uint8_t
{
    kDeAuthenticated = 0x00,  ///< No Diagnostic Clients are currently authenticated
    kAuthenticated   = 0x01   ///< A Diagnostic Client is currently authen ticated
};
///
/// @brief Type of authentication role name
using AuthRoleName = std::string;
/// @brief Structure of authentication role
///
/// @code{.isoft}
///
/// @interface_level=unit
/// @needwork = ad
///
/// @endcode
struct AuthRole
{
    ///
    /// @brief Short name of authentication role
    AuthRoleName shortName;
    ///
    /// @brief Bit position of authentication role
    std::uint8_t bitPosition;
    ///
    /// @brief Whether the authentication role is default role
    bool isDefault;
};
///
/// @brief Type of authentication role table
using AuthRoleTable = std::map< std::string, AuthRole >;
/// @brief Structure of enabled authentication
///
/// @code{.isoft}
///
/// @interface_level=unit
/// @needwork = ad
///
/// @endcode
struct EnableAuth
{
    ///
    /// @brief Whether authentication is enabled
    bool enable{false};
    ///
    /// @brief List of authentication roles for enabled authentication
    std::set< AuthRoleName > authRoleList;
};

}  // namespace server
}  // namespace uds
}  // namespace isoft

#endif  /// ISOFT_UDS_SERVER_AUTHENTICATION_STATE_H_