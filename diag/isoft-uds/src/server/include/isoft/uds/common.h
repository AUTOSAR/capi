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
/// @file       common.h
/// @brief      This file provides common functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/uds
/// @interface_level=component
/// @trace_id_sr=d5e95495-5802-442c-a52c-3911a193969f
/// @unit_name=common
/// @unit_description=This file provides common functionality
/// @endcode
///
/// ================================================================

#ifndef ISOFT_UDS_SERVER_COMMON_H_
#define ISOFT_UDS_SERVER_COMMON_H_

#include <cstdint>
#include <functional>
#include <map>

#include "security_access_management/security_level.h"
#include "session_management/session_model.h"
namespace isoft {
namespace uds {
namespace server {

/// @brief Address type
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01295
/// @trace_id_dd=DD_UDS_02587
/// @needwork = ad
/// @endcode
using Address = std::uint16_t;
/// @brief Channel identifier type
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01296
/// @trace_id_dd=DD_UDS_02588
/// @needwork = ad
/// @endcode
using ChannelIdentifier = std::uint64_t;
/// @brief Priority type
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01297
/// @trace_id_dd=DD_UDS_02589
/// @needwork = ad
/// @endcode
using Priority = std::uint8_t;
/// @brief Metadata mapping type
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01298
/// @trace_id_dd=DD_UDS_02590
/// @needwork = ad
/// @endcode
using MetaInfoMap = std::map< std::string, std::string >;
/// @brief Session change callback type
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01299
/// @trace_id_dd=DD_UDS_02591
/// @needwork = ad
/// @endcode
using NotifyOfSessionChangeCallback = std::function< void(SessionId, SessionId) >;
/// @brief Security level change callback type
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01300
/// @trace_id_dd=DD_UDS_02592
/// @needwork = ad
/// @endcode
using NotifyOfSessionChangeCallbackId = std::uint32_t;
/// @brief Security level change callback type
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01301
/// @trace_id_dd=DD_UDS_02593
/// @needwork = ad
/// @endcode
using NotifyOfSecurityLevelChangeCallback = std::function< void(SecurityLevelId) >;
/// @brief Security level change callback type
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01302
/// @trace_id_dd=DD_UDS_02594
/// @needwork = ad
/// @endcode
using NotifyOfSecurityLevelChangeCallbackId = std::uint32_t;
/// @brief Request context status enumeration
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01303
/// @trace_id_dd=DD_UDS_02595
/// @needwork = ad
/// @endcode
enum class RequestContextState
{
    kRequest,
    kResponse,
    kConfirm,
    kCanceled,
    kEnd
};
/// @brief Request context status callback type
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01304
/// @trace_id_dd=DD_UDS_02596
/// @needwork = ad
/// @endcode
using NotifyRequestContextStateCallback = std::function< void(Address, Address, RequestContextState) >;
/// @brief Service configuration base class
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01305
/// @trace_id_dd=DD_UDS_02597
/// @needwork = ad
/// @endcode
struct ServiceConfigBase
{
    /// @brief P4 timer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00912
    /// @trace_id_dd=DD_UDS_02026
    /// @needwork = ad
    /// @endcode
    std::uint32_t p4ServerMax{0U};
};

}  // namespace server
}  // namespace uds
}  // namespace isoft

#endif
