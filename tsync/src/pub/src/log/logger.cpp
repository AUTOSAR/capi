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
/// @file       logger.cpp
/// @brief      time synchronization internal log stream definition
/// @details
/// @date       2023-02-14
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================

#include "ara/tsync/internal/log/logger.h"

#include <unordered_map>
namespace ara {
namespace tsync {
namespace internal {
namespace tslog {

/// @name logOutputCb_ initialize static variable
OutputHandler Logger::s_LogOutputCb_{nullptr};  // NOLINT

/// @brief get log handle
/// @return log handle
Logger &Log() noexcept
{
    LogAttribute const logAttr{"TSYD", "TSYNC DAEMON", LogLevel::kVerbose};
    static Logger s_Logger{Logger(logAttr)};
    return s_Logger;
}

/// @brief get log handle, output only to console
/// @return log handle
Logger &Log2Console() noexcept
{
    LogAttribute const logAttr{"#TSY", "TSYNC LIB", LogLevel::kVerbose, false};
    static Logger s_Logger{Logger(logAttr)};
    return s_Logger;
}

}  // namespace tslog
}  // namespace internal
}  // namespace tsync
}  // namespace ara
