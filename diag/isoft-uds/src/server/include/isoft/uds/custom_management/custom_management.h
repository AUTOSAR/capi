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
/// @file       custom_management.h
/// @brief      This file provides custom_management functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=custom_management
/// @interface_level=external
/// @trace_id_sr=07631d69-16f2-4eab-9962-671db46becce
/// @unit_name=CustomManagement
/// @unit_description=This file provides custom_management functionality
/// @endcode
///
/// ================================================================

#ifndef ISOFT_UDS_SERVER_CONFIG_DIAGNOSTICCUSTOMMANAGEMENT_H_
#define ISOFT_UDS_SERVER_CONFIG_DIAGNOSTICCUSTOMMANAGEMENT_H_
#include "generic_uds_service.h"
#include "isoft/uds/authentication_management/authentication_state.h"
#include "isoft/uds/common.h"
#include "isoft/uds/security_access_management/security_level.h"
#include "isoft/uds/session_management/session_model.h"
namespace isoft {
namespace uds {
namespace server {
/// @brief Define custom instance configuration structure
/// @param nSid Session ID
/// @param accessPermissionAuth Access permission authentication status
/// @param accessPermissionSession Access permission session ID set
/// @param accessPermissionSecurityLevel Access permission security level set
/// @param interfacePtr Generic UDS service interface pointer
/// @code{.isoft}
/// @interface_level=unit
/// @needwork = ad
/// @endcode
struct CustomInstance : public ServiceConfigBase
{
    /// @brief Session ID
    std::uint8_t nSid{0U};
    /// @brief Access permission authentication status
    EnableAuth accessPermissionAuth{};
    /// @brief Access permission session ID set
    std::set< SessionId > accessPermissionSession{};
    /// @brief Access permission security level set
    std::set< SecurityLevelId > accessPermissionSecurityLevel{};
    /// @brief Generic UDS service interface pointer
    std::shared_ptr< GenericUDSServiceInterface > interfacePtr{nullptr};
    /// @brief Comparison operator
    /// @param other Another custom instance configuration structure
    /// @return true if the current custom instance configuration structure's session ID is less than another custom instance configuration structure's session ID, otherwise false
    bool operator<(const CustomInstance &other) const { return nSid < other.nSid; }
};
/// @brief Custom instance configuration pointer type
using CustomInstancePtr = std::shared_ptr< CustomInstance >;
/// @brief Custom instance management structure
/// @param table Custom instance configuration pointer set
/// @code{.isoft}
/// @interface_level=unit
/// @needwork = ad
/// @endcode
struct CustomManagement
{
    /// @brief Custom instance configuration pointer set
    std::set< CustomInstancePtr > table{};
};
}  // namespace server
}  // namespace uds
}  // namespace isoft

#endif  ///ISOFT_UDS_SERVER_CONFIG_DIAGNOSTICCUSTOMMANAGEMENT_H_