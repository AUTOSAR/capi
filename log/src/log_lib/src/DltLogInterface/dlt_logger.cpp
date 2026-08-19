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
/// @file       dlt_logger.cpp
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

#include "dlt_logger.h"

#include <sys/time.h>

#include <cstring>
#include <tuple>
#include <utility>

#include "DltLog/time_stamp.h"
#include "DltLogDefines/logger_scope_data.h"
#include "DltLogDerm/determined_executor.h"
#include "DltLogDerm/thread_pool.h"
#include "DltProtocol/dlt_plain_encoder.h"
#include "DltProtocol/dlt_protocol_encoder.h"
#include "DltProtocol/dlt_protocol_model_encoder.h"
#include "DltProtocol/dlt_protocol_nomodel_encoder.h"
#include "Utils/src/private_log.h"
#include "ara/log/internal/log_stream_context.h"
#include "common.h"

namespace ara {
namespace log {

namespace {

static internal::LogStreamContextPtr buildStreamContext(internal::LoggerDataPtr const &data, LogLevel logLevel)
{
    auto const &cfg            = data->config;
    auto const &sinkerLevelMap = cfg.sinkerLevelMap;
    auto const getSinkerLevel  = [&sinkerLevelMap](std::string const &key) {
        auto const it = sinkerLevelMap.find(key);
        return (it != sinkerLevelMap.end()) ? it->second : static_cast< std::uint8_t >(0U);
    };
    auto const intLevel = static_cast< int >(logLevel);

    auto ctx       = std::make_shared< internal::LogStreamContext >();
    ctx->encodeCtx = data->staticCtx->encodeCtxPtr;

    ctx->consoleEnabled = cfg.toConsole && (intLevel <= getSinkerLevel(internal::kDLT_LOGSINK_CONSOLE));
    ctx->fileEnabled    = cfg.toFile && (intLevel <= getSinkerLevel(internal::kDLT_LOGSINK_FILE));
    ctx->remoteEnabled  = cfg.toRemote && (intLevel <= getSinkerLevel(internal::kDLT_LOGSINK_REMOTE));
    ctx->plainText      = cfg.plainText;
    ctx->fileSinker     = data->sinkers.fileSinker;
    ctx->isDetermined   = data->determined.isDetermined;
    ctx->executor       = data->determined.executor;

    ctx->logLevel = logLevel;
    ctx->msgCount = data->msgCount.fetch_add(1);

    constexpr std::uint64_t kMICROSECONDS_PER_SECOND = 1000000ULL;
    if (data->runtime.timeSync != nullptr && data->runtime.timeSync->TsyncEnabled()) {
        std::uint64_t const microSec = data->runtime.timeSync->TSYNCGetTimeStamp();
        ctx->dltTimeStamp.seconds    = microSec / kMICROSECONDS_PER_SECOND;
        ctx->dltTimeStamp.uSeconds   = microSec % kMICROSECONDS_PER_SECOND;
    } else {
        struct timeval tv
        {
        };
        std::ignore                = gettimeofday(&tv, nullptr);
        ctx->dltTimeStamp.seconds  = tv.tv_sec;
        ctx->dltTimeStamp.uSeconds = tv.tv_usec;
    }

    return ctx;
}

}  // namespace

DltLogger::DltLogger(internal::LoggerDataPtr logScopedData) : logScopedData_{std::move(logScopedData)}
{
    LOGERROR(__func__) << ": enter ";
}

/// @brief Destructor
DltLogger::~DltLogger() noexcept { logScopedData_ = nullptr; }

/// @brief Output fatal level logs

LogStream DltLogger::LogFatal() const noexcept { return WithLevel(LogLevel::kFatal); }

LogStream DltLogger::LogError() const noexcept { return WithLevel(LogLevel::kError); }
LogStream DltLogger::LogWarn() const noexcept { return WithLevel(LogLevel::kWarn); }

LogStream DltLogger::LogInfo() const noexcept { return WithLevel(LogLevel::kInfo); }

LogStream DltLogger::LogDebug() const noexcept { return WithLevel(LogLevel::kDebug); }

LogStream DltLogger::LogVerbose() const noexcept { return WithLevel(LogLevel::kVerbose); }

bool DltLogger::IsEnabled(LogLevel logLevel) const noexcept
{
    if (logLevel == LogLevel::kOff || logScopedData_ == nullptr || !logScopedData_->config.loggingBehavior) {
        return false;
    }

    auto checkLevel = [logLevel](const auto &pair) { return static_cast< std::uint8_t >(logLevel) <= pair.second; };

    return std::any_of(logScopedData_->config.sinkerLevelMap.begin(), logScopedData_->config.sinkerLevelMap.end(),
                       checkLevel);
}

LogStream DltLogger::WithLevel(LogLevel logLevel) const noexcept
{
    if (logScopedData_ == nullptr || logLevel == LogLevel::kOff || !logScopedData_->config.loggingBehavior) {
        return LogStream(LogLevel::kOff, nullptr);
    }

    auto const &cfg            = logScopedData_->config;
    auto const &sinkerLevelMap = cfg.sinkerLevelMap;
    auto const getSinkerLevel  = [&sinkerLevelMap](std::string const &key) {
        auto const it = sinkerLevelMap.find(key);
        return (it != sinkerLevelMap.end()) ? it->second : static_cast< std::uint8_t >(0U);
    };

    bool toConsole{false};
    if (cfg.toConsole && (static_cast< int >(logLevel) <= getSinkerLevel(internal::kDLT_LOGSINK_CONSOLE))) {
        toConsole = true;
    }
    bool toFile{false};
    if (cfg.toFile && (static_cast< int >(logLevel) <= getSinkerLevel(internal::kDLT_LOGSINK_FILE))) {
        toFile = true;
    }
    bool toRemote{false};
    if (cfg.toRemote && (static_cast< int >(logLevel) <= getSinkerLevel(internal::kDLT_LOGSINK_REMOTE))) {
        toRemote = true;
    }

    if (!(toConsole || toFile || toRemote)) {
        return LogStream(LogLevel::kOff, nullptr);
    }

    return LogStream(logLevel, buildStreamContext(logScopedData_, logLevel));
}

LogStream DltLogger::_createModeledLogStream(LogLevel const &logLevel, std::uint32_t const &id) noexcept
{
    std::ignore = id;
    //// TODO Determine a method to disable modeled logging
    if (logScopedData_ == nullptr || false == logScopedData_->config.loggingBehavior
        || false == logScopedData_->config.traceVfb) {
        return LogStream(LogLevel::kOff, nullptr);
    }

    auto const &cfg            = logScopedData_->config;
    auto const &sinkerLevelMap = cfg.sinkerLevelMap;
    auto const getSinkerLevel  = [&sinkerLevelMap](std::string const &key) {
        auto const it = sinkerLevelMap.find(key);
        return (it != sinkerLevelMap.end()) ? it->second : static_cast< std::uint8_t >(0U);
    };

    bool toFile{false};
    if (cfg.toFile && (static_cast< int >(logLevel) <= getSinkerLevel(internal::kDLT_LOGSINK_FILE))) {
        toFile = true;
    }
    bool toRemote{false};
    if (cfg.toRemote && (static_cast< int >(logLevel) <= getSinkerLevel(internal::kDLT_LOGSINK_REMOTE))) {
        toRemote = true;
    }
    if (toRemote == false && toFile == false) {
        return LogStream(LogLevel::kOff, nullptr);
    }

    return LogStream(logLevel, buildStreamContext(logScopedData_, logLevel));
}
bool DltLogger::_canLogToRemote(LogLevel const &logLevel) const noexcept
{
    if (logScopedData_ == nullptr) {
        return false;
    }

    auto const &cfg    = logScopedData_->config;
    auto const levelIt = cfg.sinkerLevelMap.find(internal::kDLT_LOGSINK_REMOTE);
    std::uint8_t const remote
        = (levelIt != cfg.sinkerLevelMap.end()) ? levelIt->second : static_cast< std::uint8_t >(0U);
    return cfg.toRemote && (static_cast< int >(logLevel) <= remote);
}
bool DltLogger::_canLogToFile(LogLevel const &logLevel) const noexcept
{
    if (logScopedData_ == nullptr) {
        return false;
    }

    auto const &cfg         = logScopedData_->config;
    auto const levelIt      = cfg.sinkerLevelMap.find(internal::kDLT_LOGSINK_FILE);
    std::uint8_t const file = (levelIt != cfg.sinkerLevelMap.end()) ? levelIt->second : static_cast< std::uint8_t >(0U);
    return cfg.toFile && (static_cast< int >(logLevel) <= file);
}

internal::DltTimeStamp DltLogger::_createModeledTimeStamp() const noexcept
{
    constexpr std::uint64_t kMICROSECONDS_PER_SECOND = 1000000ULL;
    internal::DltTimeStamp dltTimeStamp{};

    if (logScopedData_ != nullptr && logScopedData_->runtime.timeSync != nullptr
        && logScopedData_->runtime.timeSync->TsyncEnabled()) {
        std::uint64_t const microSec = logScopedData_->runtime.timeSync->TSYNCGetTimeStamp();
        dltTimeStamp.seconds         = microSec / kMICROSECONDS_PER_SECOND;
        dltTimeStamp.uSeconds        = microSec % kMICROSECONDS_PER_SECOND;
    } else {
        struct timeval tv
        {
        };
        std::ignore           = gettimeofday(&tv, nullptr);
        dltTimeStamp.seconds  = tv.tv_sec;
        dltTimeStamp.uSeconds = tv.tv_usec;
    }

    return dltTimeStamp;
}

internal::DltProtocolEncoder *DltLogger::_createNonDetermModeledEncoder(LogLevel const &linelogLevel,
                                                                        std::uint32_t msgId) noexcept
{
    if (logScopedData_ == nullptr) {
        return nullptr;
    }

    std::unique_ptr< internal::DltProtocolEncoder > encoder{new internal::DltProtocolModelEncoder()};

    internal::DltTimeStamp const dltTimeStamp{_createModeledTimeStamp()};

    // Set external buffer (deterministic execution scenario)
    if (logScopedData_->determined.isDetermined && (logScopedData_->determined.executor != nullptr)) {
        std::size_t const defaultExternalSize{internal::kLogBufferSize};
        std::size_t externalCapacity{0U};
        std::uint8_t *externalBuffer{nullptr};
        if (logScopedData_->determined.executor->TryAllocateExternalBuffer(defaultExternalSize, externalBuffer,
                                                                           externalCapacity)) {
            std::memset(externalBuffer, 0, externalCapacity);
            encoder->SetExternalBuffer(externalBuffer, externalCapacity);
        } else {
            return nullptr;
        }
    }

    std::uint8_t const msgCount{logScopedData_->msgCount.fetch_add(1)};
    encoder->InitModeledMsgId(msgId);
    encoder->SetLineLogLevel(linelogLevel);
    encoder->SetDltTime(dltTimeStamp);
    encoder->SetMsgCount(msgCount);
    encoder->SetEncodeContext(logScopedData_->staticCtx->encodeCtxPtr);

    return encoder.release();
}

bool DltLogger::_shouldLogAsPlainText() const noexcept { return false == logScopedData_->config.nonVerboseMode; }
bool DltLogger::_canLog() const noexcept
{
    return logScopedData_ != nullptr && logScopedData_->config.loggingBehavior
           && logScopedData_->config.defaultTraceState;
}

bool DltLogger::_emitEncodedMessage(internal::DltProtocolEncoder *encoder, LogLevel const &linelogLevel) noexcept
{
    if (encoder == nullptr || logScopedData_ == nullptr) {
        return false;
    }
    encoder->Encode();
    std::uint32_t dltIntTime = encoder->GetDltIntTime();
    std::uint8_t *headerPtr  = encoder->HeaderBuffer();
    std::uint32_t headerSize = encoder->HeaderSize();
    std::uint8_t *bodyPtr    = encoder->BodyBuffer();
    std::uint32_t bodySize   = encoder->BodySize();

    if (logScopedData_->determined.isDetermined && logScopedData_->determined.executor != nullptr) {
        return logScopedData_->determined.executor->SubmitModeled(dltIntTime, headerPtr, headerSize, bodyPtr, bodySize,
                                                                  this->_canLogToFile(linelogLevel),
                                                                  this->_canLogToRemote(linelogLevel));
    }

    if (this->_canLogToFile(linelogLevel) && logScopedData_->sinkers.fileSinker != nullptr) {
        std::ignore = logScopedData_->sinkers.fileSinker->SaveLogWithTimeStamp(headerPtr, headerSize, bodyPtr, bodySize,
                                                                               encoder->GetDltTimeStamp());
    }

    if (this->_canLogToRemote(linelogLevel) && logScopedData_->config.toRemote
        && internal::RemoteSinker::Instance() != nullptr) {
        std::ignore = internal::RemoteSinker::Instance()->SaveLog(headerPtr, headerSize, bodyPtr, bodySize);
    }

    return true;
}

void DltLogger::_emitPlainTextLog(std::string const &text) noexcept
{
    if (logScopedData_ == nullptr) {
        return;
    }

    if (logScopedData_->config.toConsole) {
        std::cout << text << std::endl;
    }

    if (logScopedData_->config.toFile && logScopedData_->config.plainText
        && logScopedData_->sinkers.fileSinker != nullptr) {
        std::uint8_t *raw = reinterpret_cast< std::uint8_t * >(const_cast< char * >(text.data()));
        std::ignore       = logScopedData_->sinkers.fileSinker->SavePlainTextLog(raw, text.size());
    }
}

void DltLogger::SetThreshold(LogLevel const &loglevel) noexcept
{
    if (logScopedData_ != nullptr) {
        for (internal::DltSinkerLevelMapIterator it = logScopedData_->config.sinkerLevelMap.begin();
             it != logScopedData_->config.sinkerLevelMap.end(); ++it) {
            it->second = static_cast< std::uint8_t >(loglevel);
        }
    }
}

///########################################
std::int32_t DltLogger::ExtSetOuterTimeFunc(internal::ExtSetOuterTimeFunc func) noexcept
{
    std::ignore = func;
    return 0;
}

// Implementation of custom deleter
namespace internal {

void DltProtocolEncoderDeleter::operator()(DltProtocolEncoder *ptr) const noexcept { delete ptr; }

}  // namespace internal

void DltLogger::_appendModeledArg(internal::DltProtocolEncoder *encoder, bool arg) noexcept
{
    encoder->AppendBool(arg);
}

void DltLogger::_appendModeledArg(internal::DltProtocolEncoder *encoder, std::uint8_t arg) noexcept
{
    encoder->AppendUInt8(arg);
}

void DltLogger::_appendModeledArg(internal::DltProtocolEncoder *encoder, std::uint16_t arg) noexcept
{
    encoder->AppendUInt16(arg);
}

void DltLogger::_appendModeledArg(internal::DltProtocolEncoder *encoder, std::uint32_t arg) noexcept
{
    LOGVERBOSE("enter: ") << __func__;
    encoder->AppendUInt32(arg);
}

void DltLogger::_appendModeledArg(internal::DltProtocolEncoder *encoder, std::uint64_t arg) noexcept
{
    encoder->AppendUInt64(arg);
}

void DltLogger::_appendModeledArg(internal::DltProtocolEncoder *encoder, std::int8_t arg) noexcept
{
    LOGVERBOSE("enter: ") << __func__;
    encoder->AppendInt8(arg);
}

void DltLogger::_appendModeledArg(internal::DltProtocolEncoder *encoder, std::int16_t arg) noexcept
{
    encoder->AppendInt16(arg);
}

void DltLogger::_appendModeledArg(internal::DltProtocolEncoder *encoder, std::int32_t arg) noexcept
{
    LOGVERBOSE("enter: ") << __func__;
    encoder->AppendInt32(arg);
}

void DltLogger::_appendModeledArg(internal::DltProtocolEncoder *encoder, std::int64_t arg) noexcept
{
    encoder->AppendInt64(arg);
}

void DltLogger::_appendModeledArg(internal::DltProtocolEncoder *encoder, float arg) noexcept
{
    encoder->AppendFloat(arg);
}

void DltLogger::_appendModeledArg(internal::DltProtocolEncoder *encoder, double arg) noexcept
{
    encoder->AppendDouble(arg);
}

void DltLogger::_appendModeledArg(internal::DltProtocolEncoder *encoder, char const *arg) noexcept
{
    encoder->AppendCString(arg);
}

void DltLogger::_appendModeledArg(internal::DltProtocolEncoder *encoder, std::string const &arg) noexcept
{
    LOGVERBOSE("enter: ") << __func__;
    encoder->AppendString(arg);
}

void DltLogger::_appendModeledArg(internal::DltProtocolEncoder *encoder,
                                  std::vector< std::uint8_t > const &arg) noexcept
{
    LOGVERBOSE("enter: ") << __func__;
    encoder->AppendBytes(arg);
}

void DltLogger::_appendModeledArg(internal::DltProtocolEncoder *encoder, LogHex8 const &arg) noexcept
{
    encoder->AppendHex8(arg.value);
}

void DltLogger::_appendModeledArg(internal::DltProtocolEncoder *encoder, LogHex16 const &arg) noexcept
{
    encoder->AppendHex16(arg.value);
}

void DltLogger::_appendModeledArg(internal::DltProtocolEncoder *encoder, LogHex32 const &arg) noexcept
{
    encoder->AppendHex32(arg.value);
}

void DltLogger::_appendModeledArg(internal::DltProtocolEncoder *encoder, LogHex64 const &arg) noexcept
{
    encoder->AppendHex64(arg.value);
}

void DltLogger::_appendModeledArg(internal::DltProtocolEncoder *encoder, LogBin8 const &arg) noexcept
{
    encoder->AppendBin8(arg.value);
}

void DltLogger::_appendModeledArg(internal::DltProtocolEncoder *encoder, LogBin16 const &arg) noexcept
{
    encoder->AppendBin16(arg.value);
}

void DltLogger::_appendModeledArg(internal::DltProtocolEncoder *encoder, LogBin32 const &arg) noexcept
{
    encoder->AppendBin32(arg.value);
}

void DltLogger::_appendModeledArg(internal::DltProtocolEncoder *encoder, LogBin64 const &arg) noexcept
{
    encoder->AppendBin64(arg.value);
}

void DltLogger::_appendModeledArg(internal::DltProtocolEncoder *encoder, ara::core::StringView const &arg) noexcept
{
    encoder->AppendString(std::string(arg.data(), arg.size()));
}

void DltLogger::_appendModeledArg(internal::DltProtocolEncoder *encoder, ara::core::String const &arg) noexcept
{
    encoder->AppendString(std::string(arg.data(), arg.size()));
}
void DltLogger::_appendModeledArg(internal::DltProtocolEncoder *encoder,
                                  ara::core::Span< core::Byte const > const &value) noexcept
{
    encoder->AppendBytes(std::vector< std::uint8_t >(value.data(), value.data() + value.size()));
}
void DltLogger::_appendModeledArg(internal::DltProtocolEncoder *encoder,
                                  ara::core::InstanceSpecifier const &value) noexcept
{
    encoder->AppendString(std::string(value.ToString().data()));
}

}  // namespace log

}  // namespace ara