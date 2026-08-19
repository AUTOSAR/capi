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
/// @brief      log header file
/// @details
/// @date       2022-01-06
/// @author     mazelin
/// @version    1.2.0
///
/// ================================================================

#pragma once

#include "ara/com/internal/format/format.h"
#include "nai/runtime/nai_log.h"
#ifdef HAS_ARA_LOG
    #include "ara/log/logger.h"
#else
#endif

#if defined(__GNUC__)
    #define __FUNC__ static_cast< char const* >(__PRETTY_FUNCTION__)
#elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 19901L
    #define __FUNC__ static_cast< char const* >(__func__)
#else
    #define __FUNC__ static_cast< char const* >(__FUNCTION__)
#endif

#define LogNM ara::com::internal::log
#define __LogPlain(log, ...)                                                                                           \
    do {                                                                                                               \
        auto& __log{log};                                                                                              \
        std::ignore = __log.Write(FmtStr(__VA_ARGS__).r);                                                              \
    } while (false)
#define __LogTrace(log, ...)                                                                                           \
    do {                                                                                                               \
        auto& __log{log};                                                                                              \
        if (__log.EnableLevel(LogNM::LogLevel::kTrace)) {                                                              \
            std::ignore = __log.Write(FmtStr(__VA_ARGS__).r, LogNM::LogLevel::kTrace, __FUNCTION__, __FUNC__,          \
                                      __FILE__, __LINE__);                                                             \
        }                                                                                                              \
    } while (false)
#define __LogDebug(log, ...)                                                                                           \
    do {                                                                                                               \
        auto& __log{log};                                                                                              \
        if (__log.EnableLevel(LogNM::LogLevel::kDebug)) {                                                              \
            std::ignore = __log.Write(FmtStr(__VA_ARGS__).r, LogNM::LogLevel::kDebug, __FUNCTION__, __FUNC__,          \
                                      __FILE__, __LINE__);                                                             \
        }                                                                                                              \
    } while (false)
#define __LogInfo(log, ...)                                                                                            \
    do {                                                                                                               \
        auto& __log{log};                                                                                              \
        if (__log.EnableLevel(LogNM::LogLevel::kInfo)) {                                                               \
            std::ignore = __log.Write(FmtStr(__VA_ARGS__).r, LogNM::LogLevel::kInfo, __FUNCTION__, __FUNC__, __FILE__, \
                                      __LINE__);                                                                       \
        }                                                                                                              \
    } while (false)
#define __LogWarning(log, ...)                                                                                         \
    do {                                                                                                               \
        auto& __log{log};                                                                                              \
        if (__log.EnableLevel(LogNM::LogLevel::kWarning)) {                                                            \
            std::ignore = __log.Write(FmtStr(__VA_ARGS__).r, LogNM::LogLevel::kWarning, __FUNCTION__, __FUNC__,        \
                                      __FILE__, __LINE__);                                                             \
        }                                                                                                              \
    } while (false)
#define __LogError(log, ...)                                                                                           \
    do {                                                                                                               \
        auto& __log{log};                                                                                              \
        if (__log.EnableLevel(LogNM::LogLevel::kError)) {                                                              \
            std::ignore = __log.Write(FmtStr(__VA_ARGS__).r, LogNM::LogLevel::kError, __FUNCTION__, __FUNC__,          \
                                      __FILE__, __LINE__);                                                             \
        }                                                                                                              \
    } while (false)
#define __LogFatal(log, ...)                                                                                           \
    do {                                                                                                               \
        auto& __log{log};                                                                                              \
        if (__log.EnableLevel(LogNM::LogLevel::kFatal)) {                                                              \
            std::ignore = __log.Write(FmtStr(__VA_ARGS__).r, LogNM::LogLevel::kFatal, __FUNCTION__, __FUNC__,          \
                                      __FILE__, __LINE__);                                                             \
        }                                                                                                              \
    } while (false)
#define __LogKey(log, key, ...)                                                                                        \
    do {                                                                                                               \
        auto& __log{log};                                                                                              \
        auto __level{__log.GetKey(key)};                                                                               \
        if (__log.EnableLevel(__level)) {                                                                              \
            std::ignore = __log.Write(FmtStr(__VA_ARGS__).r, __level, key, __FUNC__, __FILE__, __LINE__);              \
        }                                                                                                              \
    } while (false)
#define __LogAssert(log, cond, ...)                                                                                    \
    do {                                                                                                               \
        auto& __log{log};                                                                                              \
        auto __level{__log.GetKey("assert")};                                                                          \
        if (__log.EnableLevel(__level)) {                                                                              \
            std::ignore                                                                                                \
                = __log.Write(FmtStr(__VA_ARGS__).r, __level, "[assert !(" #cond ")] ", __FUNC__, __FILE__, __LINE__); \
        }                                                                                                              \
    } while (false)

#define ComLog                  LogNM::Logger::GetInstance< LogNM::LoggerCom >
#define ComLogPlain(...)        __LogPlain(ComLog(), __VA_ARGS__)
#define ComLogTrace(...)        __LogTrace(ComLog(), __VA_ARGS__)
#define ComLogDebug(...)        __LogDebug(ComLog(), __VA_ARGS__)
#define ComLogInfo(...)         __LogInfo(ComLog(), __VA_ARGS__)
#define ComLogWarning(...)      __LogWarning(ComLog(), __VA_ARGS__)
#define ComLogError(...)        __LogError(ComLog(), __VA_ARGS__)
#define ComLogFatal(...)        __LogFatal(ComLog(), __VA_ARGS__)
#define ComLogKey(key, ...)     __LogKey(ComLog(), key, __VA_ARGS__)
#define ComLogAssert(cond, ...) __LogAssert(ComLog(), cond, __VA_ARGS__)

#ifdef HAS_MONITOR_LOG
    #define __MonitorTrace(trace, mode, ...)                                                                           \
        do {                                                                                                           \
            trace.Trace(mode, __VA_ARGS__);                                                                            \
        } while (false)

    #define MonitorTrace(trace, mode, ...) __MonitorTrace(trace, mode, __VA_ARGS__)
#else
    #define MonitorTrace(trace, mode, ...)
#endif

namespace ara {
namespace com {
namespace internal {
namespace log {
/// @brief LogLevel
enum class LogLevel : uint8_t
{
    kNone,
    kFatal,
    kError,
    kWarning,
    kInfo,
    kDebug,
    kTrace,
};
/// @brief Logger
class Logger
{
public:
    /// @brief GetInstance
    /// @tparam Scene
    /// @param[in] level
    /// @return Logger&
    template < typename Scene = Logger, bool c = std::is_base_of< Logger, Scene >::value >
    static std::enable_if_t< c, Scene& > GetInstance() noexcept
    {
        static auto s_PInstance{Scene::Create()};
        static auto& s_Instance{static_cast< Scene& >(*s_PInstance)};
        return s_Instance;
    }
    static std::unique_ptr< Logger > Create(ara::core::StringView ctxId = "") noexcept;
    virtual ~Logger() noexcept           = default;
    Logger() noexcept                    = default;
    Logger(Logger const& other) noexcept = default;
    Logger(Logger&& other) noexcept      = default;
    Logger& operator=(Logger const& other) noexcept = default;
    Logger& operator=(Logger&& other) noexcept = default;

public:
    /// @brief GetLevel
    /// @return LogLevel
    virtual LogLevel GetLevel() const noexcept = 0;
    /// @brief SetLevel
    /// @param[in] level
    /// @return bool
    virtual bool SetLevel(LogLevel level) noexcept = 0;
    /// @brief EnableLevel
    /// @param[in] level
    /// @return bool
    virtual bool EnableLevel(LogLevel level) const noexcept = 0;
    /// @brief GetKey
    /// @param[in] key
    /// @return LogLevel
    virtual LogLevel GetKey(ara::core::StringView key) const noexcept = 0;
    /// @brief SetKey
    /// @param[in] key
    /// @param[in] level
    /// @return bool
    virtual bool SetKey(ara::core::StringView key, LogLevel level) noexcept = 0;

public:
    // file
    /// @brief GetFilename
    /// @return ara::core::StringView
    virtual ara::core::StringView GetFilename() const noexcept = 0;
    /// @brief SetFilename
    /// @param[in] path
    /// @return bool
    virtual bool SetFilename(ara::core::StringView path) noexcept = 0;
    /// @brief EnableFile
    /// @return bool
    virtual bool EnableFile() const noexcept = 0;
    /// @brief EnableFile
    /// @param[in] enable
    /// @return bool
    virtual bool EnableFile(bool enable) noexcept = 0;
    // console
    /// @brief EnableConsole
    /// @return bool
    virtual bool EnableConsole() const noexcept = 0;
    /// @brief EnableConsole
    /// @param[in] enable
    /// @return bool
    virtual bool EnableConsole(bool enable) noexcept = 0;
    // network
    /// @brief EnableNetwork
    /// @return bool
    virtual bool EnableNetwork() const noexcept = 0;
    /// @brief EnableNetwork
    /// @param[in] enable
    /// @return bool
    virtual bool EnableNetwork(bool enable) noexcept = 0;

public:
#ifdef HAS_ARA_LOG
    // logger
    /// @brief GetLogger
    /// @return ara::log::Logger*
    virtual ara::log::Logger* GetLogger() const noexcept = 0;
    /// @brief SetLogger
    /// @param[in] logger
    /// @return bool
    virtual bool SetLogger(ara::log::Logger* logger) noexcept = 0;
    /// @brief EnableLogger
    /// @return bool
    virtual bool EnableLogger() const noexcept = 0;
    /// @brief EnableLogger
    /// @param[in] enable
    /// @return bool
    virtual bool EnableLogger(bool enable) noexcept = 0;
#else
#endif

public:
    /// @brief Write
    /// @param[in] str
    /// @return int32_t
    virtual int32_t Write(ara::core::StringView str) noexcept = 0;
    /// @brief Write
    /// @param[in] str
    /// @param[in] level
    /// @param[in] key
    /// @param[in] file
    /// @param[in] line
    /// @return int32_t
    virtual int32_t Write(ara::core::StringView str,
                          LogLevel level,
                          char const* key  = nullptr,
                          char const* func = nullptr,
                          char const* file = nullptr,
                          int32_t line     = 0) noexcept = 0;
};
class LoggerCom : public Logger
{
public:
    static std::unique_ptr< Logger > Create() noexcept;
};
class LoggerNai : virtual public Logger  // NOLINT -- design>[fuchsia-virtual-inheritance]
{
public:
    static std::unique_ptr< LoggerNai > Create() noexcept;

public:
    // nai log
    /// @brief EnableNaiLog
    /// @param[in] enable
    /// @return bool
    virtual bool EnableNaiLog(bool enable) noexcept = 0;
    /// @brief EnableNaiLog
    /// @return bool
    virtual bool EnableNaiLog() const noexcept = 0;
    /// @brief SetNaiLog
    /// @param[in] naiLog
    /// @return bool
    virtual bool SetNaiLog(nai_log_t* naiLog) noexcept = 0;
    /// @brief GetNaiLog
    /// @return nai_log_t*
    virtual nai_log_t* GetNaiLog() const noexcept = 0;
};
#ifdef HAS_MONITOR_LOG
class LoggerComMonitor : public Logger
{
public:
    static std::unique_ptr< Logger > Create() noexcept;
};
#endif
}  // namespace log
}  // namespace internal
}  // namespace com
}  // namespace ara
