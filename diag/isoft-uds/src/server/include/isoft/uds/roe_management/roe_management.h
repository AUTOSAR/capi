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
/// @file       roe_management.h
/// @brief      This file provides roe_management functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/isoft/uds/roe_management
/// @interface_level=software
/// @trace_id_sr=831b81da-d9a2-437a-afe5-9cc6fb7afaf7
/// @unit_name=RoeManagement
/// @unit_description=This file provides roe_management functionality
/// @endcode
///
/// ================================================================

#ifndef ISOFT_UDS_SERVER_CONFIG_DIAGNOSTICRESPONSEONEVENT_H_
#define ISOFT_UDS_SERVER_CONFIG_DIAGNOSTICRESPONSEONEVENT_H_
#include <memory>
#include <vector>

#include "isoft/uds/authentication_management/authentication_state.h"
#include "isoft/uds/common.h"
#include "isoft/uds/security_access_management/security_level.h"
#include "isoft/uds/session_management/session_model.h"
namespace isoft {
namespace uds {
namespace server {

/// @brief Event window time
/// @code{.isoft}
/// @note None
/// @interface_level=unit
/// @needwork=ad
/// @endcode
enum class DiagnosticEventWindowTimeEnum : std::uint8_t
{
    kInfiniteTimeToResponse = 3,
    kPowerWindowTime        = 4
};

/// @brief Event response operation
/// @code{.isoft}
/// @note None
/// @interface_level=unit
/// @needwork=ad
/// @endcode
enum class ResponseOnEventActionEnum : std::uint8_t
{
    kStopResponseOnEvent      = 0x00U,
    kOnDTCStatusChange        = 0x01U,
    kOnChangeOfDataIdentifier = 0x03U,
    kReportActivatedEvents    = 0x04U,
    kStartResponseOnEvent     = 0x05U,
    kClearResponseOnEvent     = 0x06U,
    kOnComparisonOfValues     = 0x07U,
};

/// @brief Event response instance configuration
/// @code{.isoft}
/// @note None
/// @interface_level=unit
/// @needwork=ad
/// @endcode
struct ResponseOnEventInstanceConfig
{
    /// @brief Event window time
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01052
    /// @trace_id_dd=DD_UDS_02166
    /// @needwork = ad
    /// @endcode
    std::vector< DiagnosticEventWindowTimeEnum > eventWindowTime{};
    /// @brief Event response operation
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01053
    /// @trace_id_dd=DD_UDS_02167
    /// @needwork = ad
    /// @endcode
    ResponseOnEventActionEnum responseOnEventAction{ResponseOnEventActionEnum::kStopResponseOnEvent};
    /// @brief Access permission authentication
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01054
    /// @trace_id_dd=DD_UDS_02168
    /// @needwork = ad
    /// @endcode
    EnableAuth accessPermissionAuth{};
    /// @brief Access permission session
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01055
    /// @trace_id_dd=DD_UDS_02169
    /// @needwork = ad
    /// @endcode
    std::set< SessionId > accessPermissionSession{};
    /// @brief Access permission security level
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01056
    /// @trace_id_dd=DD_UDS_02170
    /// @needwork = ad
    /// @endcode
    std::set< SecurityLevelId > accessPermissionSecurityLevel{};
    /// @brief Comparison operator
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01057
    /// @trace_id_dd=DD_UDS_02171
    /// @needwork = ad
    /// @endcode
    bool operator<(const ResponseOnEventInstanceConfig &other) const
    {
        return responseOnEventAction < other.responseOnEventAction;
    }
};

/// @brief Event response management
/// @code{.isoft}
/// @note None
/// @interface_level=unit
/// @needwork=ad
/// @endcode
struct RoeManagement : public ServiceConfigBase
{
    /// @brief Event response instance configuration
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01058
    /// @trace_id_dd=DD_UDS_02172
    /// @needwork = ad
    /// @endcode
    std::set< ResponseOnEventInstanceConfig > instanceTable{};
    /// @brief Event storage
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01059
    /// @trace_id_dd=DD_UDS_02173
    /// @needwork = ad
    /// @endcode
    bool storeEventEnabled{false};
    /// @brief Event response scheduler rate
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01060
    /// @trace_id_dd=DD_UDS_02174
    /// @needwork = ad
    /// @endcode
    std::uint32_t responseOnEventSchedulerRate{0U};
    /// @brief Maximum number of stored TC status change events
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01061
    /// @trace_id_dd=DD_UDS_02175
    /// @needwork = ad
    /// @endcode
    std::uint8_t maxNumberOfStoredDTCStatusChangedEvents{0U};
    /// @brief Maximum number of stored TC status change events
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01062
    /// @trace_id_dd=DD_UDS_02176
    /// @needwork = ad
    /// @endcode
    std::uint8_t maxNumChangeOfDataIdentfierEvents{0U};
    /// @brief Maximum number of stored TC status change events
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01063
    /// @trace_id_dd=DD_UDS_02177
    /// @needwork = ad
    /// @endcode
    std::uint8_t maxNumComparisionOfValueEvents{0U};
    /// @brief Maximum supported DID length
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01064
    /// @trace_id_dd=DD_UDS_02178
    /// @needwork = ad
    /// @endcode
    std::uint8_t maxSupportedDIDLength{0U};
    /// @brief Event response scheduler rate
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01065
    /// @trace_id_dd=DD_UDS_02179
    /// @needwork = ad
    /// @endcode
    std::uint32_t interMessageTime{0U};
};
}  // namespace server
}  // namespace uds
}  // namespace isoft

#endif  ///ISOFT_UDS_SERVER_CONFIG_DIAGNOSTICRESPONSEONEVENT_H_