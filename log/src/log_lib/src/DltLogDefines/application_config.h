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
/// @file       application_config.h
/// @brief      Template class for encapsulating log parameters
/// @details
/// @date       2024-06-28
/// @author     yangjinbiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/LOG/DltLog
/// @interface_level = unit
/// @trace_id_sr=LOG_SR_00001,LOG_SR_00002,LOG_SR_00003,LOG_SR_00004,LOG_SR_00006,LOG_SR_00010,LOG_SR_00011,LOG_SR_00012,LOG_SR_00013,LOG_SR_00014
/// @unit_name = application_config
/// @unit_description=Lib interface side of Dlt, internal support function
/// @endcode
///
/// ================================================================

#ifndef __LOG_INTERNAL_CONFIGPARSE__
#define __LOG_INTERNAL_CONFIGPARSE__

#include <cstdint>
#include <string>
#include <vector>

#include "common.h"
#include "dlt_context_config.h"

namespace ara {
namespace log {
namespace internal {

/// @brief
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_LOG_00322
/// @trace_id_dd=DD_LOG_01718
/// @needwork = ad
/// @endcode
using BackendListenList = std::vector< std::pair< std::string, uint16_t > >;

/// @brief Used to parse configuration information and cache it. Use shared_ptr to pass parameter information.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_LOG_00162
/// @trace_id_dd=DD_LOG_00247
/// @needwork = ad
/// @endcode
struct ApplicationConfig final
{
    std::string dltVersion{"1.0"};
    /// @brief Whether daemon is started
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00000
    /// @trace_id_dd=DD_LOG_01644
    /// @needwork = dda
    /// @endcode
    bool useDaemon{false};
    /// @brief  ecuId_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00000
    /// @trace_id_dd=DD_LOG_01645
    /// @needwork = dda
    /// @endcode
    std::string ecuId{"ECU1"};
    /// @brief  appid_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00000
    /// @trace_id_dd=DD_LOG_01646
    /// @needwork = dda
    /// @endcode
    std::string appid{"DEFT"};
    /// @brief  appdesc_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00000
    /// @trace_id_dd=DD_LOG_01647
    /// @needwork = dda
    /// @endcode
    std::string appdesc{"APPDESC"};
    /// @brief Unique path tag for the command channel
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00000
    /// @trace_id_dd=DD_LOG_01648
    /// @needwork = dda
    /// @endcode
    std::string commandServerName{"LTCM"};

    /// @brief Configuration information for DLT version 2
    DLTV2::DltContextConfigMap dltV2Config{};
    /// @brief Queue size
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00000
    /// @trace_id_dd=DD_LOG_01650
    /// @needwork = dda
    /// @endcode
    std::size_t queueSize{0U};
    /// @brief Client cache size
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00000
    /// @trace_id_dd=DD_LOG_01651
    /// @needwork = dda
    /// @endcode
    std::size_t clientBufferSize{0U};
    /// @brief defaultLogLeve_ Default log level used by the application
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00000
    /// @trace_id_dd=DD_LOG_01652
    /// @needwork = dda
    /// @endcode
    std::uint8_t defaultLogLevel{3U};
    /// @brief For internal use
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00000
    /// @trace_id_dd=DD_LOG_01653
    /// @needwork = dda
    /// @endcode
    std::uint8_t priLogLeve{0U};
    /// @brief Whether session ID is supported
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00000
    /// @trace_id_dd=DD_LOG_01654
    /// @needwork = dda
    /// @endcode
    bool sessionIdSupport{true};

    std::uint32_t sessionId{0};

    /// @brief Whether the log has been initialized
    bool loggingBehavior{false};
    /// @brief Internal log level
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00000
    /// @trace_id_dd=DD_LOG_01657
    /// @needwork = dda
    /// @endcode
    std::uint8_t internallogLevel{0U};
    /// @brief timebaseName_ Name of the time base, should be a consumer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00000
    /// @trace_id_dd=DD_LOG_01658
    /// @needwork = dda
    /// @endcode
    std::string timebaseName{""};
    /// @brief ClientState Client status
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00000
    /// @trace_id_dd=DD_LOG_01659
    /// @needwork = dda
    /// @endcode
    ClientState clientstate{ClientState::kUnknown};

    /// @brief
    /// @needwork = dda
    bool mDoStatis{false};

    /// @brief When both file & console exist, create a thread to handle network and console
    /// @needwork = dda
    bool startConsoleThreadIfFileExist{false};

    /// @brief
    /// @needwork = dda
    std::size_t determFreeCount{0U};

    bool appHaveDetermLogger{false};
};
}  // namespace internal
}  // namespace log
}  // namespace ara

#endif