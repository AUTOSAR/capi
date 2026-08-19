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
/// @file       security_access_management.h
/// @brief      This file provides security_access_management functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/isoft/uds/security_access_management
/// @interface_level=software
/// @trace_id_sr=d4b2e51b-71f8-45b3-b651-87439e784ebd
/// @unit_name=SecurityAccessManagement
/// @unit_description=This file provides security_access_management functionality
/// @endcode
///
/// ================================================================

#ifndef ISOFT_UDS_SERVER_CONFIG_DIAGNOSTICSECURITYLEVEL_H_
#define ISOFT_UDS_SERVER_CONFIG_DIAGNOSTICSECURITYLEVEL_H_
#include <map>
#include <memory>
#include <string>

#include "isoft/uds/authentication_management/authentication_state.h"
#include "isoft/uds/common.h"
#include "security_access.h"
#include "security_level.h"
namespace isoft {
namespace uds {
namespace server {
/// @brief Security access instance configuration
/// @code{.isoft}
/// @note None
/// @interface_level=unit
/// @needwork=ad
/// @endcode
struct SecurityAccessInstanceConfig
{
    /// @brief Request seed ID
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01271
    /// @trace_id_dd=DD_UDS_02385
    /// @needwork = ad
    /// @endcode
    std::uint8_t requestSeedId{0U};
    /// @brief Security level configuration
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01272
    /// @trace_id_dd=DD_UDS_02386
    /// @needwork = ad
    /// @endcode
    SecurityLevelConfig securityLevel{};
    /// @brief Access permission authentication
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01273
    /// @trace_id_dd=DD_UDS_02387
    /// @needwork = ad
    /// @endcode
    EnableAuth accessPermissionAuth{};
    /// @brief Access permission session ID
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01274
    /// @trace_id_dd=DD_UDS_02388
    /// @needwork = ad
    /// @endcode
    std::set< SessionId > accessPermissionSession{};
    /// @brief Access permission security level ID
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01275
    /// @trace_id_dd=DD_UDS_02389
    /// @needwork = ad
    /// @endcode
    std::set< SecurityLevelId > accessPermissionSecurityLevel{};
    /// @brief Comparison operator
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01276
    /// @trace_id_dd=DD_UDS_02390
    /// @needwork = ad
    /// @endcode
    bool operator<(const SecurityAccessInstanceConfig &other) const { return requestSeedId < other.requestSeedId; }
};

/// @brief Security access management
/// @code{.isoft}
/// @note None
/// @interface_level=unit
/// @needwork=ad
/// @endcode
struct SecurityAccessManagement : public ServiceConfigBase
{
    /// @brief Security level configuration
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01277
    /// @trace_id_dd=DD_UDS_02391
    /// @needwork = ad
    /// @endcode
    std::set< SecurityLevelConfig > securityLevelTable{};
    /// @brief Security access instance configuration
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01278
    /// @trace_id_dd=DD_UDS_02392
    /// @needwork = ad
    /// @endcode
    std::set< SecurityAccessInstanceConfig > securityAccessInstanceTable{};
    /// @brief Waiting duration after restart
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01279
    /// @trace_id_dd=DD_UDS_02393
    /// @needwork = ad
    /// @endcode
    std::uint32_t securityDelayTimeOnBoot{0U};
    /// @brief Shared timer
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01280
    /// @trace_id_dd=DD_UDS_02394
    /// @needwork = ad
    /// @endcode
    bool sharedTimer{false};
};

}  // namespace server
}  // namespace uds
}  // namespace isoft

#endif  /// ISOFT_UDS_SERVER_CONFIG_DIAGNOSTICSECURITYLEVEL_H_