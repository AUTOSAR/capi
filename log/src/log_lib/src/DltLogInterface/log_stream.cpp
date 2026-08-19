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
/// @file       log_stream.cpp
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
/// @trace_id_sr=LOG_SR_00001,LOG_SR_00002,LOG_SR_00003,LOG_SR_00004,LOG_SR_00006,LOG_SR_00010,LOG_SR_00011,LOG_SR_00012,LOG_SR_00013,LOG_SR_00014
/// @unit_name = log_stream
/// @unit_description=Lib interface side of Dlt, providing application interfaces externally for users
/// @endcode
///
/// ================================================================

#include "log_stream.h"

#include <sys/time.h>

#include <atomic>
#include <cstdint>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <tuple>

#include "DltLog/time_stamp.h"
#include "DltLogDefines/dlt_sinkers_config.h"
#include "DltLogDerm/determined_executor.h"
#include "DltLogDerm/thread_pool.h"
#include "DltProtocol/dlt_plain_encoder.h"
#include "DltProtocol/dlt_protocol_nomodel_encoder.h"
#include "Utils/src/private_log.h"
#include "ara/log/internal/log_stream_context.h"

// Required: includes struct timeval and gettimeofday declaration

#ifdef __android__
    #include <android/log.h>
#endif
/// @brief For internal use

namespace ara {
namespace log {
namespace internal {

#ifdef __android__

// Conversion function: LogLevel -> android_LogPriority
android_LogPriority ToAndroidLogPriority(LogLevel level) noexcept
{
    switch (level) {
        case LogLevel::kVerbose:
            return ANDROID_LOG_VERBOSE;
        case LogLevel::kDebug:
            return ANDROID_LOG_DEBUG;
        case LogLevel::kInfo:
            return ANDROID_LOG_INFO;
        case LogLevel::kWarn:
            return ANDROID_LOG_WARN;
        case LogLevel::kError:
            return ANDROID_LOG_ERROR;
        case LogLevel::kFatal:
            return ANDROID_LOG_FATAL;
        case LogLevel::kOff:
            return ANDROID_LOG_SILENT;  // Turn off log corresponds to SILENT
        default:
            return ANDROID_LOG_DEFAULT;  // Unknown level
    }
}
#endif
}  // namespace internal
//// This class is created only when LogLevel is OK
/// @brief
/// @param[in]  logLevel
/// @param[in]  priData
/// @param[in]  ToConsole
LogStream::LogStream(LogLevel logLevel, internal::LogStreamContextPtr logCtx) noexcept : logCtx_{logCtx}
{
    /// TODO: Console output format; fields can be configured using a model or configuration file.
    LOGVERBOSE(__func__) << "enter";

    if (this->logCtx_) {
        LOGVERBOSE(__func__) << "logCtx_->consoleEnabled:" << logCtx_->consoleEnabled;

        if (logCtx_->consoleEnabled) {
            this->mPlainTextCache_ = true;
        }

        if (logCtx_->fileEnabled) {
            if (logCtx_->plainText) {
                this->mPlainTextCache_ = true;
            } else {
                if (logCtx_->fileSinker != nullptr) {
                    this->mDltEnCache_ = true;
                }
            }
        }
#ifdef ARA_LOG_HAS_REMOTE
        if (logCtx_->remoteEnabled) {
            this->mDltEnCache_ = true;
        }
#endif

        _CreateDltEncoder();

        if (this->mPlainTextCache_) {
            mPlainEncoder_ = std::make_shared< internal::DltPlainEncoder >();
            mPlainEncoder_->SetEncodeContext(logCtx_->encodeCtx);
            mPlainEncoder_->SetLineLogLevel(logLevel);
            mPlainEncoder_->SetDltTime(logCtx_->dltTimeStamp);
            mPlainEncoder_->AppendString(std::to_string(logCtx_->msgCount));
            mPlainEncoder_->AppendString(logCtx_->encodeCtx->appId);
            mPlainEncoder_->AppendString(logCtx_->encodeCtx->contextId);

            switch (logLevel) {
                case LogLevel::kFatal: {
                    mPlainEncoder_->AppendString("Fatal");
                    break;
                }
                case LogLevel::kError: {
                    mPlainEncoder_->AppendString("Error");
                    break;
                }
                case LogLevel::kWarn: {
                    mPlainEncoder_->AppendString("Warn");
                    break;
                }
                case LogLevel::kInfo: {
                    mPlainEncoder_->AppendString("Info");
                    break;
                }
                case LogLevel::kDebug: {
                    mPlainEncoder_->AppendString("Debug");
                    break;
                }
                case LogLevel::kVerbose: {
                    mPlainEncoder_->AppendString("Verbose");
                    break;
                }
                default: {
                    break;
                }
            }

            mPlainEncoder_->AppendString("[");
        }
    }
}

void LogStream::_CreateDltEncoder() noexcept
{
    if ((logCtx_ == nullptr) || (mDltEnCache_ == false) || (mDltEncoder_ != nullptr)) {
        return;
    }

    mDltEncoder_ = std::make_shared< internal::DltProtocolNonModelEncoder >();

    if (mDltEncoder_ != nullptr) {
        if (logCtx_->isDetermined && (logCtx_->executor != nullptr)) {
            std::size_t const defaultExternalSize{internal::kLogBufferSize};
            std::size_t externalCapacity{0U};
            std::uint8_t *externalBuffer{nullptr};
            if (logCtx_->executor->TryAllocateExternalBuffer(defaultExternalSize, externalBuffer, externalCapacity)) {
                std::memset(externalBuffer, 0, externalCapacity);
                mDltEncoder_->SetExternalBuffer(externalBuffer, externalCapacity);
            } else {
                // Failed to allocate memory, should not output; clear all flags; the subsequent invalid actions should be as few as possible
                mPlainTextCache_        = false;
                mDltEnCache_            = false;
                logCtx_->consoleEnabled = false;
                logCtx_->fileEnabled    = false;
                logCtx_->remoteEnabled  = false;
                mDltEncoder_            = nullptr;
                return;
            }
        }
        mDltEncoder_->SetEncodeContext(logCtx_->encodeCtx);
        mDltEncoder_->SetDltTime(logCtx_->dltTimeStamp);
        mDltEncoder_->SetMsgCount(logCtx_->msgCount);
        mDltEncoder_->SetLineLogLevel(logCtx_->logLevel);
    }
}

/// @brief Move constructor
/// @param[in]  src
LogStream::LogStream(LogStream &&src) noexcept
{
    this->mDltEncoder_ = src.mDltEncoder_;

    this->mDltEnCache_     = src.mDltEnCache_;
    this->mPlainTextCache_ = src.mPlainTextCache_;
    this->mLocationLoged_  = src.mLocationLoged_;
    this->logCtx_          = std::move(src.logCtx_);

    if (this->mPlainTextCache_) {
        this->mPlainEncoder_->SetLineLogLevel(this->logCtx_->logLevel);
        this->mPlainEncoder_->SetDltTime(this->logCtx_->dltTimeStamp);
        this->mPlainEncoder_->SetEncodeContext(this->logCtx_->encodeCtx);
    }

    src.mDltEncoder_ = nullptr;

    src.mDltEnCache_     = false;
    src.mPlainTextCache_ = false;
    src.mLocationLoged_  = false;
}

/// @brief Destructor
LogStream::~LogStream() noexcept
{
    if (logCtx_ == nullptr) {
        return;
    }

    Flush();
}

void LogStream::ExtSetTimeStamp(std::uint64_t seconds, std::uint64_t useconds) noexcept
{
    if (logCtx_ == nullptr) {
        return;
    }
    logCtx_->dltTimeStamp.seconds  = seconds;
    logCtx_->dltTimeStamp.uSeconds = useconds;
    if (this->mPlainTextCache_) {
        this->mPlainEncoder_->SetDltTime(logCtx_->dltTimeStamp);
    }
    if (this->mDltEncoder_ != nullptr) {
        this->mDltEncoder_->SetDltTime(logCtx_->dltTimeStamp);
    }
}

/// @brief sws Requires that this does not clear the buffer, it just outputs the content
void LogStream::Flush() noexcept
{
    if (logCtx_ == nullptr) {
        return;
    }
    if (this->mPlainTextCache_) {
        mPlainEncoder_->AppendString("]");
        mPlainEncoder_->Encode();
    }
    if (logCtx_->isDetermined) {
        _FlushDetermined();
    } else {
        _FlushNonDetermined();
    }
}

void LogStream::_FlushDetermined() noexcept
{
    if (logCtx_->executor == nullptr) {
        return;
    }
    auto &executor = *logCtx_->executor;

    if (logCtx_->consoleEnabled && this->mPlainTextCache_) {
        executor.SubmitPlain(logCtx_->logLevel, logCtx_->msgCount, mPlainEncoder_->BodyBuffer(),
                             mPlainEncoder_->BodySize());
    }

    if (this->mDltEnCache_ && this->mDltEncoder_ != nullptr) {
        std::int32_t const encodeRet{mDltEncoder_->Encode()};
        if (encodeRet != static_cast< std::int32_t >(internal::DltProtocolEncoder::kEncodeOk)) {
            return;
        }
        executor.SubmitEncoded(logCtx_->logLevel, logCtx_->msgCount, mDltEncoder_->GetArgsNum(),
                               mDltEncoder_->GetDltIntTime(), mDltEncoder_->HeaderBuffer(), mDltEncoder_->HeaderSize(),
                               mDltEncoder_->BodyBuffer(), mDltEncoder_->BodySize());
    }
}

void LogStream::_FlushNonDetermined() noexcept
{
    if (logCtx_->consoleEnabled) {
#ifdef __android__
        std::string const tempStr(reinterpret_cast< const char * >(mPlainEncoder_->BodyBuffer()),
                                  mPlainEncoder_->BodySize());
        __android_log_print(ToAndroidLogPriority(mLogLevel_), "LOGTAG", tempStr.c_str());
#else
        std::cout << reinterpret_cast< const char * >(mPlainEncoder_->BodyBuffer()) << std::endl;
#endif
    }
    /// mPlainTextCache_ There are two cases: 1. Console output; 2. File output and file output is plain text
    if (logCtx_->fileEnabled) {
        if (logCtx_->plainText && this->mPlainTextCache_) {
            logCtx_->fileSinker->SavePlainTextLog(mPlainEncoder_->BodyBuffer(), mPlainEncoder_->BodySize());
        } else {
            if (logCtx_->fileSinker != nullptr) {
                mDltEncoder_->Encode();
                std::ignore = logCtx_->fileSinker->SaveLogWithTimeStamp(
                    mDltEncoder_->HeaderBuffer(), mDltEncoder_->HeaderSize(), mDltEncoder_->BodyBuffer(),
                    mDltEncoder_->BodySize(), logCtx_->dltTimeStamp);
            }
        }
    }
#ifdef ARA_LOG_HAS_REMOTE
    if (logCtx_->remoteEnabled) {
        mDltEncoder_->Encode();
        std::ignore
            = internal::RemoteSinker::Instance()->SaveLog(mDltEncoder_->HeaderBuffer(), mDltEncoder_->HeaderSize(),
                                                          mDltEncoder_->BodyBuffer(), mDltEncoder_->BodySize());
    }
#endif
}

std::uint64_t LogStream::ExtGetTimeStamp() noexcept
{
    constexpr std::int32_t kUSeconds{1000000};
    std::uint64_t mTimeStamp
        = static_cast< std::int64_t >(logCtx_->dltTimeStamp.seconds) * kUSeconds + (logCtx_->dltTimeStamp.uSeconds);
    return mTimeStamp;
}
/// @brief
/// @param[in]  value
/// @return
LogStream &LogStream::operator<<(bool value) noexcept
{
    if (this->mPlainTextCache_) {
        mPlainEncoder_->AppendString(value ? "true" : "false");
    }
    bool const ptrOK{this->mDltEncoder_ != nullptr};
    if (this->mDltEnCache_ && ptrOK) {
        mDltEncoder_->AppendBool(value);
    }
    return *this;
}
/// @brief Support for uint8_t
/// @param[in]  value
/// @return
LogStream &LogStream::operator<<(std::uint8_t value) noexcept
{
    if (this->mPlainTextCache_) {
        mPlainEncoder_->AppendString(std::to_string(static_cast< std::int32_t >(value)));
    }
    bool const ptrOK{this->mDltEncoder_ != nullptr};
    if (this->mDltEnCache_ && ptrOK) {
        mDltEncoder_->AppendUInt8(value);
    }
    return *this;
}
/// @brief Support for uint16_t
/// @param[in]  value
/// @return
LogStream &LogStream::operator<<(std::uint16_t value) noexcept
{
    if (this->mPlainTextCache_) {
        mPlainEncoder_->AppendString(std::to_string(static_cast< unsigned int >(value)));
    }
    bool const ptrOK{this->mDltEncoder_ != nullptr};
    if (this->mDltEnCache_ && ptrOK) {
        mDltEncoder_->AppendUInt16(value);
    }

    return *this;
}

/// @brief
/// @param[in]  value
/// @return
LogStream &LogStream::operator<<(std::uint32_t value) noexcept
{
    if (this->mPlainTextCache_) {
        mPlainEncoder_->AppendString(std::to_string(value));
    }
    bool const ptrOK{this->mDltEncoder_ != nullptr};
    if (this->mDltEnCache_ && ptrOK) {
        mDltEncoder_->AppendUInt32(value);
    }

    return *this;
}
/// @brief
/// @param[in]  value
/// @return
LogStream &LogStream::operator<<(std::uint64_t value) noexcept
{
    if (this->mPlainTextCache_) {
        mPlainEncoder_->AppendString(std::to_string(value));
    }
    bool const ptrOK{this->mDltEncoder_ != nullptr};
    if (this->mDltEnCache_ && ptrOK) {
        mDltEncoder_->AppendUInt64(value);
    }

    return *this;
}
/// @brief
/// @param[in]  value
/// @return
LogStream &LogStream::operator<<(std::int8_t value) noexcept
{
    if (this->mPlainTextCache_) {
        mPlainEncoder_->AppendString(std::to_string(static_cast< std::int32_t >(value)));
    }
    bool const ptrOK{this->mDltEncoder_ != nullptr};
    if (this->mDltEnCache_ && ptrOK) {
        mDltEncoder_->AppendInt8(value);
    }

    return *this;
}
/// @brief
/// @param[in]  value
/// @return
LogStream &LogStream::operator<<(std::int16_t value) noexcept
{
    if (this->mPlainTextCache_) {
        mPlainEncoder_->AppendString(std::to_string(value));
    }

    bool const ptrOK{this->mDltEncoder_ != nullptr};
    if (this->mDltEnCache_ && ptrOK) {
        mDltEncoder_->AppendInt16(value);
    }

    return *this;
}
/// @brief
/// @param[in]  value
/// @return
LogStream &LogStream::operator<<(std::int32_t value) noexcept
{
    if (this->mPlainTextCache_) {
        mPlainEncoder_->AppendString(std::to_string(value));
    }
    bool const noNUll{this->mDltEncoder_ != nullptr};
    if (this->mDltEnCache_ && noNUll) {
        mDltEncoder_->AppendInt32(value);
    }

    return *this;
}
/// @brief
/// @param[in]  value
/// @return
LogStream &LogStream::operator<<(std::int64_t value) noexcept
{
    if (this->mPlainTextCache_) {
        mPlainEncoder_->AppendString(std::to_string(value));
    }

    bool const ptrOK{this->mDltEncoder_ != nullptr};
    if (this->mDltEnCache_ && ptrOK) {
        mDltEncoder_->AppendInt64(value);
    }

    return *this;
}
/// @brief
/// @param[in]  value
/// @return
LogStream &LogStream::operator<<(float value) noexcept
{
    std::int8_t const kDecFour{4};
    if (sizeof(float) != kDecFour) {
        return *this;
    }
    if (this->mPlainTextCache_) {
        mPlainEncoder_->AppendString(std::to_string(value));
    }

    bool const ptrOK{this->mDltEncoder_ != nullptr};
    if (this->mDltEnCache_ && ptrOK) {
        mDltEncoder_->AppendFloat(value);
    }

    return *this;
}
/// @brief
/// @param[in]  value
/// @return
LogStream &LogStream::operator<<(double value) noexcept
{
    std::int8_t const kDecEight{8};
    if (sizeof(double) != kDecEight) {
        return *this;
    }
    if (this->mPlainTextCache_) {
        mPlainEncoder_->AppendString(std::to_string(value));
    }

    bool const ptrOK{this->mDltEncoder_ != nullptr};
    if (this->mDltEnCache_ && ptrOK) {
        mDltEncoder_->AppendDouble(value);
    }

    return *this;
}
/// @brief
/// @param[in]  value
/// @return
LogStream &LogStream::operator<<(LogHex8 const &value) noexcept
{
    if (this->mPlainTextCache_) {
        std::int8_t const kDecTwo{2};
        std::stringstream stream;
        // note : qac can not be
        char const zeroChar{'0'};
        stream << "0x" << std::setfill(zeroChar) << std::setw(kDecTwo) << std::hex
               << static_cast< std::int32_t >(value.value);

        mPlainEncoder_->AppendString(stream.str());
    }

    bool const ptrOK{this->mDltEncoder_ != nullptr};
    if (this->mDltEnCache_ && ptrOK) {
        mDltEncoder_->AppendHex8(value.value);
    }

    return *this;
}
/// @brief
/// @param[in]  value
/// @return
LogStream &LogStream::operator<<(LogHex16 const &value) noexcept
{
    if (this->mPlainTextCache_) {
        mPlainEncoder_->AppendString(_decimalToHexString(value.value));
    }

    bool const ptrOK{this->mDltEncoder_ != nullptr};
    if (this->mDltEnCache_ && ptrOK) {
        mDltEncoder_->AppendHex16(value.value);
    }

    return *this;
}
/// @brief
/// @param[in]  value
/// @return
LogStream &LogStream::operator<<(LogHex32 const &value) noexcept
{
    if (this->mPlainTextCache_) {
        mPlainEncoder_->AppendString(_decimalToHexString(value.value));
    }

    bool const ptrOK{this->mDltEncoder_ != nullptr};
    if (this->mDltEnCache_ && ptrOK) {
        mDltEncoder_->AppendHex32(value.value);
    }

    return *this;
}
/// @brief
/// @param[in]  value
/// @return
LogStream &LogStream::operator<<(LogHex64 const &value) noexcept
{
    if (this->mPlainTextCache_) {
        mPlainEncoder_->AppendString(_decimalToHexString(value.value));
    }

    bool const ptrOK{this->mDltEncoder_ != nullptr};
    if (this->mDltEnCache_ && ptrOK) {
        mDltEncoder_->AppendHex64(value.value);
    }

    return *this;
}
/// @brief
/// @param[in]  value
/// @return
LogStream &LogStream::operator<<(LogBin8 const &value) noexcept
{
    if (this->mPlainTextCache_) {
        mPlainEncoder_->AppendString(_decimalToBinaryString(value.value));
    }

    bool const ptrOK{this->mDltEncoder_ != nullptr};
    if (this->mDltEnCache_ && ptrOK) {
        mDltEncoder_->AppendBin8(value.value);
    }

    return *this;
}
/// @brief
/// @param[in]  value
/// @return
LogStream &LogStream::operator<<(LogBin16 const &value) noexcept
{
    if (this->mPlainTextCache_) {
        mPlainEncoder_->AppendString(_decimalToBinaryString(value.value));
    }

    bool const ptrOK{this->mDltEncoder_ != nullptr};
    if (this->mDltEnCache_ && ptrOK) {
        mDltEncoder_->AppendBin16(value.value);
    }

    return *this;
}
/// @brief
/// @param[in]  value
/// @return
LogStream &LogStream::operator<<(LogBin32 const &value) noexcept
{
    if (this->mPlainTextCache_) {
        mPlainEncoder_->AppendString(_decimalToBinaryString(value.value));
    }

    bool const ptrOK{this->mDltEncoder_ != nullptr};
    if (this->mDltEnCache_ && ptrOK) {
        mDltEncoder_->AppendBin32(value.value);
    }

    return *this;
}
/// @brief
/// @param[in]  value
/// @return
LogStream &LogStream::operator<<(LogBin64 const &value) noexcept
{
    if (this->mPlainTextCache_) {
        mPlainEncoder_->AppendString(_decimalToBinaryString(value.value));
    }
    bool const ptrOK{this->mDltEncoder_ != nullptr};
    if (this->mDltEnCache_ && ptrOK) {
        mDltEncoder_->AppendBin64(value.value);
    }

    return *this;
}
/// @brief
/// @param[in]  value
/// @return
LogStream &LogStream::operator<<(ara::core::StringView const &value) noexcept
{
    bool const checkEmpty{value == ""};
    if ((value.length() == 0U) || checkEmpty) {
        return *this;
    }
    if (this->mPlainTextCache_) {
        mPlainEncoder_->AppendString(value.data());
    }
    bool const ptrOK{this->mDltEncoder_ != nullptr};
    if (this->mDltEnCache_ && ptrOK) {
        mDltEncoder_->AppendString(value.data());
    }

    return *this;
}
/// @brief
/// @param[in]  value
/// @return
LogStream &LogStream::operator<<(const char *const value) noexcept
{
    if (value == nullptr) {
        LOGVERBOSE(__func__) << " value is null";
        return *this;
    }
    if (this->mPlainTextCache_) {
        mPlainEncoder_->AppendString(value);
    }
    bool const ptrOK{this->mDltEncoder_ != nullptr};
    if (this->mDltEnCache_ && ptrOK) {
        mDltEncoder_->AppendString(value);
    }
    return *this;
}
/// @brief Append a string, the string can be empty
/// @param[in]  value
/// @return
LogStream &LogStream::operator<<(std::string const &value) noexcept
{
    if (this->mPlainTextCache_) {
        mPlainEncoder_->AppendString(value);
    }
    bool const ptrOK{this->mDltEncoder_ != nullptr};
    if (this->mDltEnCache_ && ptrOK) {
        mDltEncoder_->AppendString(value);
    }

    return *this;
}

/// @brief /// This needs to be reimplemented: whether to print in hexadecimal or directly in decimal
/// @param[in]  value
/// @return
LogStream &LogStream::operator<<(std::vector< std::uint8_t > const &value) noexcept
{
    std::stringstream localStringstream;
    char const zeroChar{'0'};
    localStringstream << std::hex << std::setfill(zeroChar);
    std::int8_t const kDecTwo{2};
    for (std::uint8_t const &byte : value) {
        std::uint32_t const byteInteger{static_cast< uint32_t >(byte)};
        localStringstream << std::setw(kDecTwo) << byteInteger;
    }
    if (this->mPlainTextCache_) {
        mPlainEncoder_->AppendString(localStringstream.str());
    }
    bool const ptrOK{this->mDltEncoder_ != nullptr};
    if (this->mDltEnCache_ && ptrOK) {
        mDltEncoder_->AppendBytes(value);
    }

    return *this;
}
/// @brief
/// @param[in]  value
/// @return
LogStream &LogStream::operator<<(core::Span< core::Byte const > const &value) noexcept
{
    std::int8_t const kDecTwo{2};
    std::stringstream localStringstream;
    char const zeroChar{'0'};
    localStringstream << std::hex << std::setfill(zeroChar);
    for (core::Byte const &byte : value) {
        std::uint32_t const byteInteger{static_cast< unsigned char >(byte)};
        localStringstream << std::setw(kDecTwo) << byteInteger;
    }
    if (this->mPlainTextCache_) {
        mPlainEncoder_->AppendString(localStringstream.str());
    }
    bool const enOK{this->mDltEncoder_ != nullptr};
    if (this->mDltEnCache_ && enOK) {
        mDltEncoder_->AppendString(localStringstream.str());
    }
    return *this;
}
/// @brief Merge using positions
/// @param[in]  file
/// @param[in]  line
/// @return
LogStream &LogStream::WithLocation(ara::core::StringView file, std::int32_t line) noexcept
{
    if (mLocationLoged_ == false) {
        std::string data{file.data()};
        std::ignore = data.append(":");
        std::ignore = data.append(std::to_string(line));
        if (this->mPlainTextCache_) {
            mPlainEncoder_->AppendString(data);
        }
        bool const enOK{this->mDltEncoder_ != nullptr};
        if (this->mDltEnCache_ && enOK) {
            mDltEncoder_->AppendString(data);
        }
        mLocationLoged_ = true;
    }

    return *this;
}
/// @brief Increase log level
/// @param[in]  out
/// @param[in]  value
/// @return
LogStream &operator<<(LogStream &out, LogLevel value) noexcept
{
    switch (value) {
        case LogLevel::kOff: {
            out << "off";
            break;
        }
        case LogLevel::kFatal: {
            out << "kFatal";
            break;
        }
        case LogLevel::kError: {
            out << "kError";
            break;
        }
        case LogLevel::kWarn: {
            out << "kWarn";
            break;
        }
        case LogLevel::kInfo: {
            out << "kInfo";
            break;
        }
        case LogLevel::kDebug: {
            out << "kDebug";
            break;
        }
        case LogLevel::kVerbose: {
            out << "kVerbose";
            break;
        }
        default: {
            break;
        }
    }

    return out;
}

/// @brief
/// @param[in]  out
/// @param[in]  value
/// @return
LogStream &operator<<(LogStream &out, core::ErrorCode const &value) noexcept
{
    std::string str{};
    std::ignore = str.append(value.Domain().Name())
                      .append(":")
                      .append(std::to_string(value.Value()))
                      .append(":")
                      .append(value.Message().data());
    out << str;
    return out;
}
/// @brief InstanceSpecifier
/// @param[in]  out
/// @param[in]  value
/// @return
LogStream &operator<<(LogStream &out, ara::core::InstanceSpecifier const &value) noexcept
{
    out << value.ToString();
    return out;
}

}  // namespace log
}  // namespace ara
