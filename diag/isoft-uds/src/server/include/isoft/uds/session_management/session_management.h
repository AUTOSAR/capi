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
/// @file       session_management.h
/// @brief      This file provides session_management functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/isoft/uds/session_management
/// @interface_level=software
/// @trace_id_sr=68e9a8f4-901d-4cd0-a4a8-87e7b0a135dd
/// @unit_name=SessionManagement
/// @unit_description=This file provides session_management functionality
/// @endcode
///
/// ================================================================

#ifndef ISOFT_UDS_SERVER_CONFIG_DIAGNOSTICSESSIONCONTROL_H_
#define ISOFT_UDS_SERVER_CONFIG_DIAGNOSTICSESSIONCONTROL_H_
#include <set>
#include <string>

#include "isoft/uds/authentication_management/authentication_state.h"
#include "isoft/uds/common.h"
#include "isoft/uds/security_access_management/security_level.h"
#include "session_model.h"
namespace isoft {
namespace uds {
namespace server {
/// @brief Session control instance configuration
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01320
/// @trace_id_dd=DD_UDS_02612
/// @needwork = ad
/// @endcode
struct SessionControlInstanceConfig
{
    /// @brief Session ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00999
    /// @trace_id_dd=DD_UDS_02113
    /// @needwork = ad
    /// @endcode
    SessionId sesssionIdConfig{0U};
    /// @brief Access authentication permission
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01000
    /// @trace_id_dd=DD_UDS_02114
    /// @needwork = ad
    /// @endcode
    EnableAuth accessPermissionAuth{};
    /// @brief Session permission
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01001
    /// @trace_id_dd=DD_UDS_02115
    /// @needwork = ad
    /// @endcode
    std::set< SessionId > sessionPermission{};
    /// @brief Security level permission
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01002
    /// @trace_id_dd=DD_UDS_02116
    /// @needwork = ad
    /// @endcode
    std::set< SecurityLevelId > securityLevelPermission{};
    /// @brief Comparison operator
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01003
    /// @trace_id_dd=DD_UDS_02117
    /// @needwork = ad
    /// @endcode
    bool operator<(const SessionControlInstanceConfig &other) const
    {
        return sesssionIdConfig < other.sesssionIdConfig;
    }
};
/// @brief Session management
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01321
/// @trace_id_dd=DD_UDS_02613
/// @needwork = ad
/// @endcode
struct SessionMangement : public ServiceConfigBase
{
    /// @brief Session configuration table
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01004
    /// @trace_id_dd=DD_UDS_02118
    /// @needwork = ad
    /// @endcode
    std::set< SessionModel > sessionConfigTable{};
    /// @brief Session control instance table
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01005
    /// @trace_id_dd=DD_UDS_02119
    /// @needwork = ad
    /// @endcode
    std::set< SessionControlInstanceConfig > sessionControlInstanceTable{};
};
}  // namespace server
}  // namespace uds
}  // namespace isoft

#endif  ///ISOFT_UDS_SERVER_CONFIG_DIAGNOSTICSESSIONCONTROL_H_