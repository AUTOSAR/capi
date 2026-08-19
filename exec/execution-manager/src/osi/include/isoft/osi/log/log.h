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
/// @brief      Log definition
/// @details
/// @date       2023-04-01
/// @author     xueliang.bao
/// @version    1.2.0
///
/// ================================================================

#ifndef ISOFT_OSI_LOG_H_
#define ISOFT_OSI_LOG_H_

#include <chrono>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <tuple>

/// @brief Output Fatal level log
#define LOGF() LOG().LogFatal()
/// @brief Output Error level log
#define LOGE() LOG().LogError()
/// @brief Output Warn level log
#define LOGW() LOG().LogWarn()
/// @brief Output Info level log
#define LOGI() LOG().LogInfo()
/// @brief Output Debug level log
#define LOGD() LOG().LogDebug()
/// @brief Output Verbose level log
#define LOGV() LOG().LogVerbose()

/// @brief Output end
#define LOGEND() " ]" << std::endl

#ifndef LOG
 /// @brief OSI log output macro
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    #define LOG() isoft::osi::log::Log< isoft::osi::log::OsiLogCtx >()
#endif

namespace isoft {
namespace osi {
namespace log {

/// @brief char type redefinition
using Char8_t = char;

/// @brief Get padding space string
/// @return Padding space string
inline constexpr Char8_t const *GetPadSpace() noexcept { return "                     "; }

/// @brief Safely get local time
/// @param tt Timestamp
/// @param out Output local time
/// @exception std::bad_alloc Thrown when memory allocation fails
inline void SafeLocalTime(std::time_t const *const tt, std::tm &out)
{
 std::ignore = localtime_r(tt, &out); // Linux/macOS/POSIX safe version1,3
}

/// @brief Get current time
/// @return Current time string
inline std::string CurrentTime() noexcept
{
    std::chrono::_V2::system_clock::time_point const now{std::chrono::system_clock::now()};
    std::time_t const tt{std::chrono::system_clock::to_time_t(now)};

    std::tm localTm{};
 SafeLocalTime(&tt, localTm); // Thread-safe time conversion

    std::ostringstream oss;
    std::chrono::microseconds const value{
        std::chrono::duration_cast< std::chrono::microseconds >(now.time_since_epoch()) % 1000000};
    oss << std::put_time(&localTm, "%Y/%m/%d %H:%M:%S") << "." << value.count() << GetPadSpace();

    return oss.str();
}

/// @brief OSI log information
class Logger
{
public:
 /// @brief Custom constructor
 /// @param ctxId Log context ID
    explicit Logger(std::string ctxId) noexcept : ctxId_{std::move(ctxId)} {}

 /// @brief Destructor
    ~Logger() noexcept { std::cout << std::endl; }

 /// @brief Move constructor
 /// @param other Other Logger
    Logger(Logger &&other) = default;

 /// @brief Copy constructor
 /// @param other Other Logger
    Logger(Logger const &other) = default;

 /// @brief Disable move assignment operation
 /// @param other Other Logger
 /// @return New Logger
    Logger &operator=(Logger &&other) = delete;

 /// @brief Disable copy assignment operation
 /// @param other Other Logger
 /// @return New Logger
    Logger &operator=(Logger const &other) = delete;

 /// @brief Output fatal error log
 /// @return Output stream
    std::ostream &LogFatal() const noexcept { return std::cout << CurrentTime() << "OSI " << ctxId_ << " Fatal [ "; }

 /// @brief Output general error log
 /// @return Output stream
    std::ostream &LogError() const noexcept { return std::cout << CurrentTime() << "OSI " << ctxId_ << " Error [ "; }

 /// @brief Output warning log
 /// @return Output stream
    std::ostream &LogWarn() const noexcept { return std::cout << CurrentTime() << "OSI " << ctxId_ << " Warn [ "; }

 /// @brief Output general information log
 /// @return Output stream
    std::ostream &LogInfo() const noexcept { return std::cout << CurrentTime() << "OSI " << ctxId_ << " Info [ "; }

 /// @brief Output debug information log
 /// @return Output stream
    std::ostream &LogDebug() const noexcept { return std::cout << CurrentTime() << "OSI " << ctxId_ << " Debug [ "; }

 /// @brief Output detailed information log
 /// @return Output stream
    std::ostream &LogVerbose() const noexcept
    {
        return std::cout << CurrentTime() << "OSI " << ctxId_ << " Verbose [ ";
    }

private:
 /// @brief Log prefix information
    std::string ctxId_;
};

/// @brief OSI log context information
class OsiLogCtx
{
public:
 /// @brief Get log context ID
 /// @return Log context ID
    static std::string GetLogCtxId() noexcept { return std::string("#DFT"); }
};

/// @brief OSI log output function
/// @tparam T Log context type, default is OsiLogCtx
/// @return Log instance
template < typename T = OsiLogCtx >
Logger &Log() noexcept
{
    static Logger s_Logger{T::GetLogCtxId()};
    return s_Logger;
}

}  // namespace log
}  // namespace osi
}  // namespace isoft

#endif  ///< ISOFT_OSI_LOG_H_
