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
/// @file       dlt_context_config.h
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
/// @unit_name = channel_config
/// @unit_description=Lib interface side of Dlt, internal support function.
/// @endcode
///
/// ================================================================

#ifndef __LOG_INTERNAL_CHANNEL_CONFIG__
#define __LOG_INTERNAL_CHANNEL_CONFIG__
// clang-format off

#include <cstdint>
#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include "dlt_sinkers_config.h"

// clang-format on
namespace ara {
namespace log {
namespace internal {

namespace DLTV2 {
// DLT_LOGSINK_REMOTE DLT_LOGSINK_DLT kDLT_LOGSINK_CONSOLE kDLT_LOGSINK_FILE kDLT_LOGSINK_ARTI

struct DltContextConfig final
{
    /// @brief applicationId_ Application ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dda
    /// @endcode
    std::string applicationId{};

    /// @brief applicationDescription_ Application description
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00000
    /// @trace_id_dd=DD_LOG_01631
    /// @needwork = dda
    /// @endcode
    std::string applicationDescription{};

    /// @brief contextDescription Channel description
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dda
    /// @endcode
    std::string contextDescription{};

    /// @brief contextId Channel ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dda
    /// @endcode
    std::string contextId{};

    /// @brief Session ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dda
    /// @endcode
    std::uint32_t sessionId{0};

    /// @brief serviceInstance_ Service instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dda
    /// @endcode
    std::string serviceInstance{};

    /// @brief Whether it is a deterministic channel flag
    /// @needwork = dda
    bool determConfiged{false};

    bool enableCompress{false};

    DltSinkersMap sinkersMap{};

    /// @brief Log level mapping, used to quickly determine the level of each sinker for the current context.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dda
    /// @endcode

    DltSinkerLevelMap sinkerLevelMap{};
};

using DltContextConfigPtr = std::shared_ptr< DltContextConfig >;
using DltContextConfigMap = std::map< std::string, DltContextConfigPtr >;
}  // namespace DLTV2

}  // namespace internal
}  // namespace log
}  // namespace ara
#endif
