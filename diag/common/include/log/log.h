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
/// @file       log.h
/// @brief
/// @details
/// @date       2022-08-29
/// @author     gaohuiming
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_DIAG_COMMON_LOG_H_
#define ARA_DIAG_COMMON_LOG_H_

#include <ara/log/log_stream.h>

namespace ara {
namespace diag {
namespace common {

/// @brief Fatal log interface
/// @return Log stream object
ara::log::LogStream LogFatal() noexcept;

/// @brief Error log interface
/// @return Log stream object
ara::log::LogStream LogError() noexcept;

/// @brief Warning log interface
/// @return Log stream object
ara::log::LogStream LogWarn() noexcept;

/// @brief Info log interface
/// @return Log stream object
ara::log::LogStream LogInfo() noexcept;

/// @brief Debug log interface
/// @return Log stream object
ara::log::LogStream LogDebug() noexcept;

/// @brief Verbose log interface
/// @return Log stream object
ara::log::LogStream LogVerbose() noexcept;

/// @brief Check if a certain log level is enabled
/// @param[in] logLevel
/// @return Whether enabled
bool IsEnabled(ara::log::LogLevel const logLevel) noexcept;
}  // namespace common
}  // namespace diag
}  // namespace ara

#endif  // ARA_DIAG_COMMON_LOG_H_