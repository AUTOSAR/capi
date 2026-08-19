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
/// @file       logger.h
/// @brief      time synchronization internal log stream definition
/// @details
/// @date       2023-02-09
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// export_level=/TimeSync/LogWrapper
/// module_path=/TimeSync/LogWrapper
/// @endcode
///
/// ================================================================

#ifndef ARA_TSYNC_INTERNAL_LOG_LOGGER_H_
#define ARA_TSYNC_INTERNAL_LOG_LOGGER_H_

#include <ara/core/string.h>
#include <ara/core/string_view.h>

#include <cstdint>
#include <functional>
#include <type_traits>

#include "ara/tsync/internal/log/defines.h"
#include "ara/tsync/internal/log/stream.h"

namespace ara {
namespace tsync {
namespace internal {
namespace tslog {

/// @brief log printer
class Logger final
{
public:
    /// @brief constructor
    Logger() = default;

    /// @brief constructor with parameters
    /// @param logAttr - log attributes
    explicit Logger(LogAttribute logAttr) noexcept : logAttribute_{std::move(logAttr)} {}

    /// @brief default destructor
    ~Logger() = default;

    /// @brief copy constructor is prohibited
    /// @param other - other object
    Logger(Logger const &other) = delete;

    /// @brief copy is prohibited
    /// @param other - other object
    /// @return reference to class object
    Logger &operator=(Logger const &other) = delete;

    /// @brief move constructor
    /// @param other an existing rvalue reference.
    Logger(Logger &&other) noexcept = default;

    /// @brief '=' operator overload
    /// @param other an existing rvalue reference.
    /// @return reference to class object
    Logger &operator=(Logger &&other) &noexcept = default;

    /// @brief print verbose level log
    /// @return log stream
    Stream Verbose() noexcept
    {
        logAttribute_.SetLogLevel(LogLevel::kVerbose);
        return Stream(logAttribute_, s_LogOutputCb_);
    }

    /// @brief print debug level log
    /// @return log stream
    Stream Debug() noexcept
    {
        logAttribute_.SetLogLevel(LogLevel::kDebug);
        return Stream(logAttribute_, s_LogOutputCb_);
    }

    /// @brief print warning level log
    /// @return log stream
    Stream Warn() noexcept
    {
        logAttribute_.SetLogLevel(LogLevel::kWarn);
        return Stream(logAttribute_, s_LogOutputCb_);
    }

    /// @brief print info level log
    /// @return log stream
    Stream Info() noexcept
    {
        logAttribute_.SetLogLevel(LogLevel::kInfo);
        return Stream(logAttribute_, s_LogOutputCb_);
    }

    /// @brief print error level log
    /// @return log stream
    Stream Error() noexcept
    {
        logAttribute_.SetLogLevel(LogLevel::kError);
        return Stream(logAttribute_, s_LogOutputCb_);
    }

    /// @brief print fatal level log
    /// @return log stream
    Stream Fatal() noexcept
    {
        logAttribute_.SetLogLevel(LogLevel::kFatal);
        return Stream(logAttribute_, s_LogOutputCb_);
    }

    /// @brief set log output handling function
    /// @param cb - handling function
    static void SetOutputHandler(OutputHandler const &cb) noexcept { s_LogOutputCb_ = cb; }

private:
    /// @name s_LogOutputCb_ - log output callback function
    static OutputHandler s_LogOutputCb_;

    /// @name logAttribute_ - log attributes
    LogAttribute logAttribute_;
};

/// @brief get log handle
/// @return log handle
Logger &Log() noexcept;

/// @brief get log handle, output only to console
/// @return log handle
Logger &Log2Console() noexcept;

}  // namespace tslog
}  // namespace internal
}  // namespace tsync
}  // namespace ara

#endif  /// ARA_TSYNC_INTERNAL_LOG_LOGGER_H_
