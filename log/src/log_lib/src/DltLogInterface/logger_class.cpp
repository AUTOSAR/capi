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
/// @file       logger_class.cpp
/// @brief      No description provided.
/// @details
/// @date       2025-09-15
/// @author     yangjinbiao
/// @version    1.2.0
///
/// ================================================================

#include "logger_class.h"

namespace ara {
namespace log {
LogStream Logger::LogFatal() const noexcept { return mImpl_.LogFatal(); }
LogStream Logger::LogError() const noexcept { return mImpl_.LogError(); }
LogStream Logger::LogWarn() const noexcept { return mImpl_.LogWarn(); }
LogStream Logger::LogInfo() const noexcept { return mImpl_.LogInfo(); }
LogStream Logger::LogDebug() const noexcept { return mImpl_.LogDebug(); }
LogStream Logger::LogVerbose() const noexcept { return mImpl_.LogVerbose(); }
LogStream Logger::WithLevel(LogLevel logLevel) const noexcept { return mImpl_.WithLevel(logLevel); }
bool Logger::IsEnabled(LogLevel logLevel) const noexcept { return mImpl_.IsEnabled(logLevel); }
void Logger::SetThreshold(LogLevel const& loglevel) noexcept { mImpl_.SetThreshold(loglevel); }
std::int32_t Logger::ExtSetOuterTimeFunc(internal::ExtSetOuterTimeFunc const& func) noexcept
{
    return mImpl_.ExtSetOuterTimeFunc(func);
}

}  // namespace log
}  // namespace ara