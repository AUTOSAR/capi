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
/// @file       dlt_logger.h
/// @brief      Template class for encapsulating log parameters
/// @details
/// @date       2024-06-28
/// @author     yangjinbiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/LOG
/// @interface_level = module
/// @trace_id_sr=LOG_SR_00001,LOG_SR_00002,LOG_SR_00003,LOG_SR_00004,LOG_SR_00006,LOG_SR_00010,LOG_SR_00011,LOG_SR_00012,LOG_SR_00013,LOG_SR_00014,LOG_SR_000021,LOG_SR_000022
/// @unit_name = logger
/// @unit_description=Lib interface side of Dlt, providing application interfaces externally for users
/// @endcode
///
/// ================================================================

#ifndef ___ARA_LOG_LOGGER_CLASS_H__
#define ___ARA_LOG_LOGGER_CLASS_H__

#include <ara/core/instance_specifier.h>
#include <ara/core/span.h>
#include <ara/core/string.h>
#include <ara/core/string_view.h>

#include <cstdint>
#include <cstring>
#include <memory>
#include <string>
#include <type_traits>

#include "common.h"
#include "log_stream.h"

#ifdef ARA_LOG_HAS_MODEL_MESSAGE
    #include "ara/log/internal/dltmessage/define.h"
#endif  // DEBUG

namespace ara {
namespace log {
class Logger;
namespace internal {
class DltProtocolEncoder;
struct LoggerScopeData;
using LoggerDataPtr       = std::shared_ptr< LoggerScopeData >;
using ExtSetOuterTimeFunc = std::function< std::uint64_t(void) >;

// Custom deleter, used to keep forward declaration in header files
struct DltProtocolEncoderDeleter
{
    void operator()(DltProtocolEncoder *ptr) const noexcept;
};

}  // namespace internal

/// @brief DltLogger of a specific context
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_LOG_00067
/// @trace_id_dd=DD_LOG_00080
/// @needwork = ad
/// @endcode
class DltLogger final
{
    using LoggerDataPtr = std::shared_ptr< internal::LoggerScopeData >;

private:
    /// @brief
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00067
    /// @trace_id_dd=DD_LOG_00082
    /// @needwork = dda
    /// @endcode
    internal::LoggerDataPtr logScopedData_{nullptr};
    friend Logger;

public:
    /// @brief Custom constructor to create an instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00067
    /// @trace_id_dd=DD_LOG_00081
    /// @needwork = dda
    /// @endcode
    explicit DltLogger(internal::LoggerDataPtr logScopedData);

    /// @brief  Constructor
    DltLogger() noexcept = delete;
    /// @brief  Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00068
    /// @trace_id_dd=DD_LOG_00083
    /// @needwork = ad
    /// @endcode
    ~DltLogger() noexcept;

    /// @brief  Disable copy construction
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00339
    /// @trace_id_dd=DD_LOG_01738
    /// @needwork = ad
    /// @endcode

    /// @brief Move constructor
    /// @code{.isoft}
    /// @interface_level=unit
    DltLogger(DltLogger &&) noexcept = default;

    /// @brief Move assignment operator
    /// @code{.isoft}
    /// @interface_level=unit
    DltLogger &operator=(DltLogger &&) noexcept = default;
    DltLogger(DltLogger const &)                = delete;
    /// @brief Disable copy assignment
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00340
    /// @trace_id_dd=DD_LOG_01739
    /// @needwork = ad
    /// @endcode
    DltLogger &operator=(DltLogger const &) = delete;
    /// @brief Prevent logger from being copied
    /// @code{.isoft}
    /// @see WithLevel
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00069
    /// @trace_id_dd=DD_LOG_00084
    /// @needwork = ad
    /// @endcode
    LogStream LogFatal() const noexcept;

    /// @brief Create a stream for error logs
    /// @returns Error log stream in the current context
    /// @code{.isoft}
    /// @see WithLevel
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00070
    /// @trace_id_dd=DD_LOG_00085
    /// @needwork = ad
    /// @endcode
    LogStream LogError() const noexcept;

    /// @brief Create a stream for warning logs
    /// @returns Warning log stream in the current context
    /// @code{.isoft}
    /// @see WithLevel
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00071
    /// @trace_id_dd=DD_LOG_00086
    /// @needwork = ad
    /// @endcode
    LogStream LogWarn() const noexcept;

    /// @brief Create a stream for information logs
    /// @returns Information log stream in the current context
    /// @code{.isoft}
    /// @see WithLevel
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00072
    /// @trace_id_dd=DD_LOG_00087
    /// @needwork = ad
    /// @endcode
    LogStream LogInfo() const noexcept;

    /// @brief Create a stream for debug logs
    /// @returns Debug log stream in the current context
    /// @code{.isoft}
    /// @see WithLevel
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00073
    /// @trace_id_dd=DD_LOG_00088
    /// @needwork = ad
    /// @endcode
    LogStream LogDebug() const noexcept;

    /// @brief Create a stream for verbose logs
    /// @returns Verbose log stream in the current context
    /// @code{.isoft}
    /// @see WithLevel
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00074
    /// @trace_id_dd=DD_LOG_00089
    /// @needwork = ad
    /// @endcode
    LogStream LogVerbose() const noexcept;

    /// @brief Determine whether a certian log level is enabled in the current
    /// context or not
    /// @param[in]  logLevel Input log severity level
    /// @returns True if the level is enabled; otherwise false
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00075
    /// @trace_id_dd=DD_LOG_00090
    /// @needwork = ad
    /// @endcode
    bool IsEnabled(LogLevel logLevel) const noexcept;

    /// @brief Create a stream for certian level logs
    /// @param[in]  logLevel Input log severity level
    /// @returns Log stream with the determined level in the current context
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00076
    /// @trace_id_dd=DD_LOG_00091
    /// @needwork = ad
    /// @endcode
    LogStream WithLevel(LogLevel logLevel) const noexcept;
    /// @brief
    /// @param[in]  id
    /// @param[in]  ...args
    /// @code{.isoft}
    /// @tparam MsgId
    /// @tparam ...Params
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00077
    /// @trace_id_dd=DD_LOG_00092
    /// @needwork = ad
    /// @endcode
    template < typename MsgId, typename... Params >
    void Log(MsgId const &id, Params const &...args) noexcept
    {
        if (_canLog() == false) {
            return;
        }
        std::int32_t numArgs = sizeof...(args);
        if (numArgs != id.argsNum) {
            std::cout << "args number wrong " << std::to_string(id.id) << std::endl;
            return;
        }

        static constexpr LogLevel kDefaultLogLevel = LogLevel::kVerbose;

        if (!IsEnabled(kDefaultLogLevel)) {
            return;
        }

        /// EXP: Modeled message text printing for testing.
        if (_shouldLogAsPlainText()) {
            _logPlainTextMessage(id.typeInfo, id.sourceFile, id.lineNumber, kDefaultLogLevel, args...);
        } else {
            /// TODO : r2311 replace with _traceSwitch(m_impl, id, args..., logScopedData_);
            _logModeledMessage(kDefaultLogLevel, id.id, args...);
        }
    }

    // LogWith function: supports custom location information, message type, and variable arguments
    template < typename... Attrs, typename MsgId, typename... Params >
    void LogWith(const std::tuple< Attrs... > &attrs, const MsgId &msgId, const Params &...args) noexcept
    {
        if (_canLog() == false) {
            return;
        }
        // 1. Parse location information
        const std::string &sourceFile   = std::get< 0 >(attrs);
        const std::uint32_t &lineNumber = std::get< 1 >(attrs);

        std::int32_t numArgs = sizeof...(args);
        if (numArgs != msgId.argsNum) {
            std::cout << "args number wrong " << std::to_string(msgId.id) << std::endl;
            return;
        }
        static constexpr LogLevel kDefaultLogLevel = LogLevel::kInfo;
        if (!IsEnabled(kDefaultLogLevel)) {
            return;
        }
        /// EXP: Convert modeled messages to non-modeled
        if (_shouldLogAsPlainText()) {
            _logPlainTextMessage(msgId.typeInfo, sourceFile, lineNumber, kDefaultLogLevel, args...);
        } else {
            _logModeledMessage(kDefaultLogLevel, msgId.id, args...);
        }
    }
    /// @brief
    /// @param[in]  loglevel
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00078
    /// @trace_id_dd=DD_LOG_00093
    /// @needwork = ad
    /// @endcode
    void SetThreshold(LogLevel const &loglevel) noexcept;

    ////#######################Extended interfaces###################
    std::int32_t ExtSetOuterTimeFunc(internal::ExtSetOuterTimeFunc func) noexcept;

private:
    bool _shouldLogAsPlainText() const noexcept;

    LogStream _createModeledLogStream(LogLevel const &logLevel, std::uint32_t const &id) noexcept;
    internal::DltProtocolEncoder *_createNonDetermModeledEncoder(LogLevel const &linelogLevel,
                                                                 std::uint32_t msgId) noexcept;
    bool _emitEncodedMessage(internal::DltProtocolEncoder *encoder, LogLevel const &linelogLevel) noexcept;
    bool _canLogToRemote(LogLevel const &linelogLevel) const noexcept;
    bool _canLogToFile(LogLevel const &linelogLevel) const noexcept;
    internal::DltTimeStamp _createModeledTimeStamp() const noexcept;

    void _emitPlainTextLog(std::string const &text) noexcept;

    void _appendModeledArg(internal::DltProtocolEncoder *encoder, bool arg) noexcept;
    void _appendModeledArg(internal::DltProtocolEncoder *encoder, std::uint8_t arg) noexcept;
    void _appendModeledArg(internal::DltProtocolEncoder *encoder, std::uint16_t arg) noexcept;
    void _appendModeledArg(internal::DltProtocolEncoder *encoder, std::uint32_t arg) noexcept;
    void _appendModeledArg(internal::DltProtocolEncoder *encoder, std::uint64_t arg) noexcept;
    void _appendModeledArg(internal::DltProtocolEncoder *encoder, std::int8_t arg) noexcept;
    void _appendModeledArg(internal::DltProtocolEncoder *encoder, std::int16_t arg) noexcept;
    void _appendModeledArg(internal::DltProtocolEncoder *encoder, std::int32_t arg) noexcept;
    void _appendModeledArg(internal::DltProtocolEncoder *encoder, std::int64_t arg) noexcept;
    void _appendModeledArg(internal::DltProtocolEncoder *encoder, float arg) noexcept;
    void _appendModeledArg(internal::DltProtocolEncoder *encoder, double arg) noexcept;
    void _appendModeledArg(internal::DltProtocolEncoder *encoder, char const *arg) noexcept;
    void _appendModeledArg(internal::DltProtocolEncoder *encoder, std::string const &arg) noexcept;
    void _appendModeledArg(internal::DltProtocolEncoder *encoder, std::vector< std::uint8_t > const &arg) noexcept;
    void _appendModeledArg(internal::DltProtocolEncoder *encoder, LogHex8 const &arg) noexcept;
    void _appendModeledArg(internal::DltProtocolEncoder *encoder, LogHex16 const &arg) noexcept;
    void _appendModeledArg(internal::DltProtocolEncoder *encoder, LogHex32 const &arg) noexcept;
    void _appendModeledArg(internal::DltProtocolEncoder *encoder, LogHex64 const &arg) noexcept;
    void _appendModeledArg(internal::DltProtocolEncoder *encoder, LogBin8 const &arg) noexcept;
    void _appendModeledArg(internal::DltProtocolEncoder *encoder, LogBin16 const &arg) noexcept;
    void _appendModeledArg(internal::DltProtocolEncoder *encoder, LogBin32 const &arg) noexcept;
    void _appendModeledArg(internal::DltProtocolEncoder *encoder, LogBin64 const &arg) noexcept;

    void _appendModeledArg(internal::DltProtocolEncoder *encoder, ara::core::StringView const &arg) noexcept;
    void _appendModeledArg(internal::DltProtocolEncoder *encoder, ara::core::String const &arg) noexcept;
    void _appendModeledArg(internal::DltProtocolEncoder *encoder, core::Span< core::Byte const > const &value) noexcept;
    void _appendModeledArg(internal::DltProtocolEncoder *encoder, ara::core::InstanceSpecifier const &value) noexcept;

    // Parameter append function - recursively expand parameter pack
    template < typename Arg >
    void _appendArgsToEncoder(internal::DltProtocolEncoder *encoder, Arg const &arg)
    {
        _appendModeledArg(encoder, arg);
    }

    template < typename First, typename... Rest >
    void _appendArgsToEncoder(internal::DltProtocolEncoder *encoder, First const &first, Rest const &...rest)
    {
        _appendModeledArg(encoder, first);
        _appendArgsToEncoder(encoder, rest...);
    }

    bool _canLog() const noexcept;

    template < typename T >
    void _appendHexToSStream(std::stringstream &res, T const &decimal)
    {
        std::stringstream stream;
        std::int32_t const width{static_cast< std::int32_t >(sizeof(T) * 2)};
        stream << "0x" << std::setfill('0') << std::setw(width) << std::hex
               << static_cast< typename std::make_unsigned< T >::type >(decimal);
        res << stream.str();
    }

    template < typename T >
    void _appendBinaryToSStream(std::stringstream &res, T const &decimal)
    {
        std::stringstream stream;
        stream << "0b";

        using UnsignedT                 = typename std::make_unsigned< T >::type;
        UnsignedT const unsignedDecimal = static_cast< UnsignedT >(decimal);

        constexpr std::int32_t kRangeEight = 8;
        size_t const totalBits             = sizeof(T) * kRangeEight;

        for (size_t i = totalBits - 1; i < totalBits; --i) {
            stream << ((unsignedDecimal >> i) & 1U);
        }

        res << stream.str();
    }

    void _appendArgToSStream(std::stringstream &res, bool arg) { res << (arg ? "true" : "false"); }

    void _appendArgToSStream(std::stringstream &res, std::uint8_t arg) { res << static_cast< std::uint32_t >(arg); }

    void _appendArgToSStream(std::stringstream &res, std::int8_t arg) { res << static_cast< std::int32_t >(arg); }

    void _appendArgToSStream(std::stringstream &res, const char *arg)
    {
        if (arg == nullptr) {
            res << "(null)";
            return;
        }
        res << arg;
    }

    void _appendArgToSStream(std::stringstream &res, LogHex8 const &arg) { _appendHexToSStream(res, arg.value); }
    void _appendArgToSStream(std::stringstream &res, LogHex16 const &arg) { _appendHexToSStream(res, arg.value); }
    void _appendArgToSStream(std::stringstream &res, LogHex32 const &arg) { _appendHexToSStream(res, arg.value); }
    void _appendArgToSStream(std::stringstream &res, LogHex64 const &arg) { _appendHexToSStream(res, arg.value); }

    void _appendArgToSStream(std::stringstream &res, LogBin8 const &arg) { _appendBinaryToSStream(res, arg.value); }
    void _appendArgToSStream(std::stringstream &res, LogBin16 const &arg) { _appendBinaryToSStream(res, arg.value); }
    void _appendArgToSStream(std::stringstream &res, LogBin32 const &arg) { _appendBinaryToSStream(res, arg.value); }
    void _appendArgToSStream(std::stringstream &res, LogBin64 const &arg) { _appendBinaryToSStream(res, arg.value); }

    template < typename T >
    void _appendArgToSStream(std::stringstream &res, Argument< T > const &arg)
    {
        _appendArgToSStream(res, arg.name);
        res << " ";
        _appendArgToSStream(res, arg.arg);
        res << " ";
        _appendArgToSStream(res, arg.unit);
    }

    template < typename Arg >
    void _appendArgToSStream(std::stringstream &res, Arg const &arg)
    {
        res << arg;
    }

    void _appendArgsToSStream(std::stringstream &) {}

    template < typename First, typename... Rest >
    void _appendArgsToSStream(std::stringstream &res, First const &first, Rest const &...rest)
    {
        _appendArgToSStream(res, first);
        _appendArgsToSStream(res, rest...);
    }

    template < typename... Params >
    void _logPlainTextMessage(char const *typeInfo,
                              std::string const &sourceFile,
                              std::uint32_t lineNumber,
                              LogLevel const &linelogLevel,
                              Params const &...args) noexcept
    {
        LogStream resultStream = WithLevel(linelogLevel);
        std::string logstr;
        std::stringstream buffer;
        _expandSStream(buffer, args...);
        logstr.append(typeInfo)
            .append(" file : ")
            .append(sourceFile)
            .append(" line  : ")
            .append(std::to_string(lineNumber))
            .append(" data : ")
            .append(buffer.str());
        resultStream << logstr;
    }

    template < typename... Params >
    void _logModeledMessage(LogLevel const &linelogLevel, std::uint32_t msgId, Params const &...args)
    {
        if (!_canLogToFile(linelogLevel) && !_canLogToRemote(linelogLevel)) {
            return;
        }

        std::unique_ptr< internal::DltProtocolEncoder, internal::DltProtocolEncoderDeleter > encoder{
            _createNonDetermModeledEncoder(linelogLevel, msgId)};
        if (!encoder) {
            return;
        }

        _appendArgsToEncoder(encoder.get(), args...);

        std::ignore = _emitEncodedMessage(encoder.get(), linelogLevel);
    }
    // Or handle separately.

    /// @brief
    /// @param[in]  res
    /// @param[in]  arg
    /// @code{.isoft}
    /// @tparam Arg
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00067
    /// @trace_id_dd=DD_LOG_00094
    /// @needwork = dda
    /// @endcode
    template < typename Arg >
    void _expand(LogStream &res, Arg arg)
    {
        res << arg;
    }
    /// @brief
    /// @param[in]  res
    /// @param[in]  first
    /// @param[in]  ...args
    /// @code{.isoft}
    /// @tparam First
    /// @tparam ...Args
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00067
    /// @trace_id_dd=DD_LOG_00095
    /// @needwork = dda
    /// @endcode
    template < typename First, typename... Args >
    void _expand(LogStream &res, First first, Args... args)
    {
        res << first;
        _expand(res, args...);
    }
    /// @brief
    /// @param[in]  res
    /// @param[in]  arg
    /// @code{.isoft}
    /// @tparam Arg
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00067
    /// @trace_id_dd=DD_LOG_00096
    /// @needwork = dda
    /// @endcode
    template < typename Arg >
    void _expandSStream(std::stringstream &res, Arg arg)
    {
        res << arg;
    }
    /// @brief
    /// @param[in]  res
    /// @param[in]  first
    /// @param[in]  ...args
    /// @code{.isoft}
    /// @tparam First
    /// @tparam ...Args
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00067
    /// @trace_id_dd=DD_LOG_00097
    /// @needwork = dda
    /// @endcode
    template < typename First, typename... Args >
    void _expandSStream(std::stringstream &res, First first, Args... args)
    {
        res << first;
        _expandSStream(res, args...);
    }

    template < typename MsgId, typename... Params >
    void _traceSwitch(DltLogger &logger,
                      const MsgId &messageId,
                      const Params &...params,
                      internal::LoggerDataPtr logScopedData) noexcept
    {
        logger.Log(messageId, params..., logScopedData);
    }

    // /**
    //  * @brief _traceSwitch specialized version - for ExecutionManagerProcessStateChangeMsgType
    //  * Routes to TraceArti
    //  */
    // template <typename... Params>
    // void _traceSwitch(DltLoggerImpl& logger,
    //                  const ExecutionManagerProcessStateChangeMsgType& msg_id,
    //                  const Params&... params,
    //                  std::shared_ptr<internal::DltLoggerScopeData> logScopedData) noexcept {
    //     // Route to ARTI tracking implementation
    //     ara::log::ext::TraceArti(msg_id, params...);

    //     // If you also need to log, uncomment the following
    //     // logger.Log(msg_id, params..., logScopedData);
    // }

    ////  ########################################
};
using DltLoggerPtr = std::shared_ptr< DltLogger >;
}  // namespace log
}  // namespace ara

#endif