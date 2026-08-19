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
/// @file       logger_config.h
/// @brief      Logger configuration (can be updated at runtime)
/// @details
/// @date       2025-09-03
/// @author     yangjinbiao
/// @version    1.2.0
///
/// ================================================================

#ifndef __LOG_INTERNAL_LOGGER_CONFIG__
#define __LOG_INTERNAL_LOGGER_CONFIG__

#include <cstdint>
#include <map>
#include <string>

namespace ara {
namespace log {
namespace internal {

using DltSinkerLevelMap = std::map< std::string, std::uint8_t >;

/// @brief Logger configuration information (hot-updatable)
/// @note Contains log output target configuration and level filtering
struct LoggerConfig
{
    /// @brief Print to console flag
    bool toConsole{false};

    /// @brief Print to file flag
    bool toFile{false};

    /// @brief Print to remote flag
    bool toRemote{false};

    /// @brief Plain text mode
    bool plainText{false};

    /// @brief Whether to print logs
    bool loggingBehavior{false};

    /// @brief Non-verbose mode
    bool nonVerboseMode{true};

    /// @brief Whether to enable statistics
    bool doStatis{false};

    /// @brief TraceVfb
    bool traceVfb{true};

    /// @brief Default trace status
    bool defaultTraceState{true};

    /// @brief Log level for each sinker
    DltSinkerLevelMap sinkerLevelMap;
};

}  // namespace internal
}  // namespace log
}  // namespace ara

#endif  // __LOG_INTERNAL_LOGGER_CONFIG__
