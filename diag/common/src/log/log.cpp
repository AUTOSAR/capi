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
/// @file       log.cpp
/// @brief
/// @details
/// @date       2022-08-29
/// @author     gaohuiming
/// @version    1.2.0
///
/// ================================================================

#include "log/log.h"

#include <ara/log/logger.h>

namespace ara {
namespace diag {
namespace common {

/// @brief Create logger object
/// @return logger
static ara::log::Logger& GetDefaultLogger() noexcept
{
    static ara::log::Logger& logger{ara::log::CreateLogger(ara::core::StringView("#DIA"),  //NOLINT
                                                           (ara::core::StringView("Diagnostics")))};
    return logger;
}

/// @brief Fatal log
/// @return Log stream
ara::log::LogStream LogFatal() noexcept { return GetDefaultLogger().LogFatal(); }

/// @brief Error log
/// @return Log stream
ara::log::LogStream LogError() noexcept { return GetDefaultLogger().LogError(); }

/// @brief Warning log
/// @return Log stream
ara::log::LogStream LogWarn() noexcept { return GetDefaultLogger().LogWarn(); }

/// @brief Info log
/// @return Log stream
ara::log::LogStream LogInfo() noexcept { return GetDefaultLogger().LogInfo(); }

/// @brief Debug log
/// @return Log stream
ara::log::LogStream LogDebug() noexcept { return GetDefaultLogger().LogDebug(); }

/// @brief Verbose log
/// @return Log stream
ara::log::LogStream LogVerbose() noexcept { return GetDefaultLogger().LogVerbose(); }

/// @brief Check if a certain log level is enabled
/// @param[in] logLevel
/// @return Whether enabled
bool IsEnabled(ara::log::LogLevel const logLevel) noexcept { return GetDefaultLogger().IsEnabled(logLevel); }

}  // namespace common
}  // namespace diag
}  // namespace ara
