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
/// @brief      log source file
/// @details
/// @date       2022-01-06
/// @author     mazelin
/// @version    1.2.0
///
/// ================================================================

#include "ara/com/internal/log/log.h"

#undef ComLogTrace
#undef ComLogDebug
#undef ComLogInfo
#undef ComLogWarning
#undef ComLogError
#undef ComLogFatal

#include <unistd.h>

#include <chrono>
#include <cstdio>
#include <thread>

#ifdef HAS_ARA_LOG
    #include "ara/log/internal/initialization.h"
#else
#endif
#include "isoft/ara_fsh/process.h"
#include "isoft/manifestreader/manifest_reader.h"
#include "nai/runtime/nai_errno.h"
#include "nai/runtime/nai_time.h"

namespace ara {
namespace com {
namespace internal {
namespace log {
/// @brief Level2Str
/// @param[in] level
/// @return Level2Str
ara::core::StringView Level2Str(LogLevel level) noexcept
{
    switch (level) {
        case LogLevel::kTrace:
            return "trace";
        case LogLevel::kDebug:
            return "debug";
        case LogLevel::kInfo:
            return "info";
        case LogLevel::kWarning:
            return "warn";
        case LogLevel::kError:
            return "error";
        case LogLevel::kFatal:
            return "fatal";
        default:
            return "";
    }
    return "";
}
/// @brief Str2Level
/// @param[in] level
/// @return LogLevel
LogLevel Str2Level(ara::core::StringView level) noexcept
{
    if (level == "trace" || level == "TRACE" || level == "verbose" || level == "VERBOSE") {
        return LogLevel::kTrace;
    }
    if (level == "debug" || level == "DEBUG") {
        return LogLevel::kDebug;
    }
    if (level == "info" || level == "INFO") {
        return LogLevel::kInfo;
    }
    if (level == "warn" || level == "WARN" || level == "warning" || level == "WARNING") {
        return LogLevel::kWarning;
    }
    if (level == "error" || level == "ERROR") {
        return LogLevel::kError;
    }
    if (level == "fatal" || level == "FATAL") {
        return LogLevel::kFatal;
    }
    return LogLevel::kNone;
}
/// @brief Int2Level
/// @param[in] level
/// @return LogLevel
LogLevel Int2Level(int32_t levelIn) noexcept
{
    auto level{static_cast< LogLevel >(levelIn)};
    if (level >= LogLevel::kNone && level <= LogLevel::kTrace) {
        return level;
    }
    return LogLevel::kNone;
}

void LoadConfig(Logger& logger, ara::core::StringView ctxId) noexcept
{
    isoft::ara_fsh::Process proc;
    auto manifestFile{proc.GetConfig("log.json")};
    auto manifestRes{isoft::manifestreader::OpenManifest(manifestFile.c_str())};
    if (!manifestRes) {
        return;
    }
    auto manifest{std::move(manifestRes).Value()};
    int32_t defaultLevel{};
    if (manifest->Load("defaultLevel", defaultLevel) == isoft::kSuccess) {
        std::ignore = logger.SetLevel(Int2Level(defaultLevel));
    }
#ifdef HAS_ARA_LOG
    bool loggingBehavior{true};
    if (manifest->Load("loggingBehavior", loggingBehavior) == isoft::kSuccess) {
        std::ignore = logger.EnableLogger(loggingBehavior);
    }
    if (ara::log::internal::LogInitialized()) {
        std::ignore
            = logger.SetLogger(&ara::log::CreateLogger(ctxId, "", static_cast< ara::log::LogLevel >(defaultLevel)));
    }
#else
#endif
    struct Channel
    {
        int32_t modes{};
        int32_t level{};
        ara::core::String fileName;
        ara::core::String ctxId;
        int32_t ManifestLoader(isoft::manifestreader::ManifestNode const& node) noexcept
        {
            if (isoft::kSuccess != node.Load("modes", modes)) {
                return isoft::kFailure;
            }
            if (isoft::kSuccess != node.Load("level", level)) {
                return isoft::kFailure;
            }
            if (isoft::kSuccess != node.Load("fileName", fileName)) {
                return isoft::kFailure;
            }
            if (isoft::kSuccess != node.Load("ctxId", ctxId)) {
                return isoft::kFailure;
            }
            return isoft::kSuccess;
        }
    };
    ara::core::Vector< Channel > channels;
    std::ignore = manifest->Load("channels", channels);
    for (auto& channel : channels) {
        if (channel.ctxId != ctxId) {
            continue;
        }
        char fileName[PATH_MAX]{};
        auto len{snprintf(fileName, sizeof(fileName) - 1, "%s", channel.fileName.data())};
        if (channel.fileName.empty() || channel.fileName.back() == '/') {
            ara::core::String appId{};
            if (manifest->Load("appId", appId) == isoft::kSuccess && !appId.empty()) {
            } else {
                char procname[PATH_MAX];
                auto ret{readlink("/proc/self/exe", procname, sizeof(procname) - 1)};
                if (ret == -1) {
                    fprintf(stderr, "get process name failed: %s\n",
                            strerror(errno));  // NOLINT -- compatibility>[concurrency-mt-unsafe]
                    continue;
                }
                auto* basename{strrchr(procname, '/')};
                appId = basename != nullptr ? basename + 1 : procname;
            }
            len += snprintf(fileName + len, sizeof(fileName) - len - 1, "%s", appId.data());
        }
        len += snprintf(fileName + len, sizeof(fileName) - len - 1, ".%d.log", getpid());
        std::ignore = logger.SetLevel(Int2Level(channel.level));
        std::ignore = logger.SetFilename(fileName);
        std::ignore = logger.EnableFile((channel.modes & (1 << 1)) != 0);
        std::ignore = logger.EnableConsole((channel.modes & (1 << 2)) != 0);
        std::ignore = logger.EnableNetwork((channel.modes & (1 << 0)) != 0);
        break;
    }
}

class LoggerImpl : virtual public Logger  // NOLINT -- design>[fuchsia-virtual-inheritance]
{
private:
    // filter level and key
    /// @brief
    LogLevel level_{LogLevel::kDebug};
    /// @brief
    ara::core::Map< ara::core::String, LogLevel > customKeys_{
        {"assert", LogLevel::kFatal},
        {"exception", LogLevel::kError},
        {"profile", LogLevel::kInfo},
    };

public:
    /// @brief GetLevel
    /// @return LogLevel
    LogLevel GetLevel() const noexcept override { return level_; }
    /// @brief SetLevel
    /// @param[in] level
    /// @return bool
    bool SetLevel(LogLevel level) noexcept override
    {
        bool r{true};
        level_ = level;
        return r;
    }
    /// @brief EnableLevel
    /// @param[in] level
    /// @return bool
    bool EnableLevel(LogLevel level) const noexcept override { return level <= GetLevel() && level > LogLevel::kNone; }
    /// @brief GetKey
    /// @param[in] key
    /// @return LogLevel
    LogLevel GetKey(ara::core::StringView key) const noexcept override
    {
        if (key.empty()) {
            return LogLevel::kNone;
        }
        auto it{customKeys_.find(key.data())};
        return it != customKeys_.end() ? it->second : LogLevel::kNone;
    }
    /// @brief SetKey
    /// @param[in] key
    /// @param[in] level
    /// @return bool
    bool SetKey(ara::core::StringView key, LogLevel level) noexcept override
    {
        if (key.empty()) {
            return false;
        }
        customKeys_[key.data()] = level;
        return true;
    }
#ifdef HAS_ARA_LOG
private:
    // output way
    /// @brief
    bool enableLogger_{false};
    /// @brief
    ara::log::Logger* logger_{};

public:
    /// @brief GetLogger
    /// @return ara::log::Logger*
    ara::log::Logger* GetLogger() const noexcept override { return logger_; }
    /// @brief SetLogger
    /// @param[in] logger
    /// @return bool
    bool SetLogger(ara::log::Logger* logger) noexcept override
    {
        logger_ = logger;
        if (logger == nullptr) {
        } else if (logger->IsEnabled(ara::log::LogLevel::kVerbose)) {
            std::ignore = SetLevel(LogLevel::kTrace);
        } else if (logger->IsEnabled(ara::log::LogLevel::kDebug)) {
            std::ignore = SetLevel(LogLevel::kDebug);
        } else if (logger->IsEnabled(ara::log::LogLevel::kInfo)) {
            std::ignore = SetLevel(LogLevel::kInfo);
        } else if (logger->IsEnabled(ara::log::LogLevel::kWarn)) {
            std::ignore = SetLevel(LogLevel::kWarning);
        } else if (logger->IsEnabled(ara::log::LogLevel::kError)) {
            std::ignore = SetLevel(LogLevel::kError);
        } else if (logger->IsEnabled(ara::log::LogLevel::kFatal)) {
            std::ignore = SetLevel(LogLevel::kFatal);
        } else if (logger->IsEnabled(ara::log::LogLevel::kOff)) {
            std::ignore = SetLevel(LogLevel::kNone);
        }
        return true;
    }
    /// @brief EnableLogger
    /// @return bool
    bool EnableLogger() const noexcept override
    {
        return enableLogger_ && logger_ != nullptr && ara::log::internal::LogInitialized();
    }
    /// @brief EnableLogger
    /// @param[in] enable
    /// @return bool
    bool EnableLogger(bool enable) noexcept override
    {
        enableLogger_ = enable;
        return EnableLogger();
    }
#else
#endif
private:
    /// @brief
    std::shared_ptr< FILE > file_{};
    /// @brief
    ara::core::String filename_{};
    /// @brief
    bool enableFile_{false};
    /// @brief
    bool enableConsole_{true};
    /// @brief
    bool enableNetwork_{false};

private:
    /// @brief Initialize file
    /// @return bool
    bool _initFile() noexcept
    {
        if (file_ != nullptr) {
            return true;
        }
        if (filename_.empty()) {
            return false;
        }
        auto* file{fopen(filename_.data(), "a+e")};  // use 'fopen' mode 'e' to set O_CLOEXEC [android-cloexec-fopen]
        if (file == nullptr) {
            fprintf(stderr, "open file failed: %s, %s\n", strerror(errno),  // NOLINT -- compatibility>[concurrency-mt-unsafe]
                    filename_.data());
            return false;
        }
        file_ = std::shared_ptr< FILE >{file, [](auto _) { fclose(_); }};
        return true;
    }

public:
    /// @brief GetFilename
    /// @return ara::core::StringView
    ara::core::StringView GetFilename() const noexcept override { return filename_; }
    /// @brief SetFilename
    /// @param[in] filename
    /// @return bool
    bool SetFilename(ara::core::StringView filename) noexcept override
    {
        if (file_ != nullptr && filename.empty()) {
            return false;
        }
        filename_ = filename;
        return true;
    }
    /// @brief EnableFile
    /// @return bool
    bool EnableFile() const noexcept override { return enableFile_; }
    /// @brief EnableFile
    /// @param[in] enable
    /// @return bool
    bool EnableFile(bool enable) noexcept override
    {
        enableFile_ = enable;
        return true;
    }
    /// @brief EnableConsole
    /// @return bool
    bool EnableConsole() const noexcept override { return enableConsole_; }
    /// @brief EnableConsole
    /// @param[in] enable
    /// @return bool
    bool EnableConsole(bool enable) noexcept override
    {
        enableConsole_ = enable;
        return true;
    }
    /// @brief EnableNetwork
    /// @return bool
    bool EnableNetwork() const noexcept override { return enableNetwork_; }
    /// @brief EnableNetwork
    /// @param[in] enable
    /// @return bool
    bool EnableNetwork(bool enable) noexcept override
    {
        enableNetwork_ = enable;
        return true;
    }

public:
    /// @brief Write
    /// @param[in] str
    /// @return int32_t
    int32_t Write(ara::core::StringView str) noexcept override
    {
        int32_t result{};
        int32_t ret{};
        if (EnableFile() && (file_ != nullptr || _initFile())) {
            ret = fprintf(file_.get(), "%s\n", str.data());
            if (ret < 0) {
                result = ret;
                // return result;
            } else if (result >= 0) {
                fflush(file_.get());
                result += ret;
            }
        }
        if (EnableConsole()) {
            ret = fprintf(stdout, "%s\n", str.data());
            if (ret < 0) {
                result = ret;
                // return result;
            } else if (result >= 0) {
                result += ret;
            }
        }
        if (EnableNetwork()) {
        }
        return result;
    }
    /// @brief Write
    /// @param[in] level
    /// @param[in] str
    /// @return int32_t
    int32_t Write(ara::core::StringView str,
                  LogLevel level,
                  char const* key,
                  char const* func,
                  char const* file,
                  int32_t line) noexcept override
    {
        if (!EnableLevel(level)) {
            return -__LINE__;
        }
        int32_t result{};
        int32_t ret{};
        if (key == nullptr) {
            key = "";
        }
        if (func == nullptr) {
            func = "";
        }
        if (file == nullptr) {
            file = "";
        }
#ifdef HAS_ARA_LOG
        if (EnableLogger()) {
            result = ret = static_cast< int32_t >(str.size());
            switch (level) {
                case LogLevel::kTrace: {
                    auto stream{logger_->LogVerbose()};
                    FmtNM::FormatToString(stream, GenMode(FmtNM::FormatMode::kBlack, "[", key, "]\t"), str);
                    if (*func != '\0') {
                        FmtNM::FormatToString(stream, GenMode(FmtNM::FormatMode::kBlue, "\n\t", func));
                    }
                    if (*file != '\0') {
                        FmtNM::FormatToString(stream, GenMode(FmtNM::FormatMode::kBlue, "\n\t", file, ":", line));
                    }
                } break;
                case LogLevel::kDebug: {
                    auto stream{logger_->LogDebug()};
                    FmtNM::FormatToString(stream, GenMode(FmtNM::FormatMode::kWhite, "[", key, "]\t"), str);
                } break;
                case LogLevel::kInfo: {
                    auto stream{logger_->LogInfo()};
                    FmtNM::FormatToString(stream, GenMode(FmtNM::FormatMode::kGreen, "[", key, "]\t"), str);
                } break;
                case LogLevel::kWarning: {
                    auto stream{logger_->LogWarn()};
                    FmtNM::FormatToString(stream, GenMode(FmtNM::FormatMode::kYellow, "[", key, "]\t"), str);
                    if (*func != '\0') {
                        FmtNM::FormatToString(stream, GenMode(FmtNM::FormatMode::kBlue, "\n\t", func));
                    }
                    if (*file != '\0') {
                        FmtNM::FormatToString(stream, GenMode(FmtNM::FormatMode::kBlue, "\n\t", file, ":", line));
                    }
                } break;
                case LogLevel::kError: {
                    auto stream{logger_->LogError()};
                    FmtNM::FormatToString(stream, GenMode(FmtNM::FormatMode::kRed, "[", key, "]\t"), str);
                    if (*func != '\0') {
                        FmtNM::FormatToString(stream, GenMode(FmtNM::FormatMode::kBlue, "\n\t", func));
                    }
                    if (*file != '\0') {
                        FmtNM::FormatToString(stream, GenMode(FmtNM::FormatMode::kBlue, "\n\t", file, ":", line));
                    }
                } break;
                case LogLevel::kFatal: {
                    auto stream{logger_->LogFatal()};
                    FmtNM::FormatToString(stream, GenMode(FmtNM::FormatMode::kMagenta, "[", key, "]\t"), str);
                    if (*func != '\0') {
                        FmtNM::FormatToString(stream, GenMode(FmtNM::FormatMode::kBlue, "\n\t", func));
                    }
                    if (*file != '\0') {
                        FmtNM::FormatToString(stream, GenMode(FmtNM::FormatMode::kBlue, "\n\t", file, ":", line));
                    }
                } break;
                default: {
                    return -__LINE__;
                } break;
            }
            return result;
        }
#else
#endif
        auto now{std::chrono::system_clock::now()};
        auto tp{std::chrono::time_point_cast< std::chrono::nanoseconds >(now)};
        auto stm{std::chrono::system_clock::to_time_t(now)};
        std::tm ltm{};
        localtime_r(&stm, &ltm);
        constexpr decltype(ltm.tm_year) kTmYearBase{1900};
        constexpr decltype(ltm.tm_mon) kTmMonthOffset{1};
        constexpr int32_t kMicrosecondsPerSecond{1000000};
        auto year{ltm.tm_year + kTmYearBase};
        auto mon{ltm.tm_mon + kTmMonthOffset};
        auto day{ltm.tm_mday};
        auto hour{ltm.tm_hour};
        auto min{ltm.tm_min};
        auto sec{ltm.tm_sec};
        auto ts{tp.time_since_epoch()};
        auto usec{static_cast< int32_t >(ts.count() % kMicrosecondsPerSecond)};
        auto tid{std::hash< std::thread::id >{}(std::this_thread::get_id())};
        if (EnableFile() && (file_ != nullptr || _initFile())) {
            switch (level) {
                case LogLevel::kTrace: {
                    auto* stream{file_.get()};
                    ret = fprintf(stream, "%04d-%02d-%02d %02d:%02d:%02d.%06d %016lx %sT[%s]%s\t%s\n", year, mon, day,
                                  hour, min, sec, usec, tid, GenMode(FmtNM::FormatMode::kBlack, key), str.data());
                    result += ret < 0 ? 0 : ret;
                    if (*func != '\0') {
                        ret = fprintf(stream, "\t%s%s%s\n", GenMode(FmtNM::FormatMode::kBlue, func));
                        result += ret < 0 ? 0 : ret;
                    }
                    if (*file != '\0') {
                        ret = fprintf(stream, "\t%s%s:%d%s\n", GenMode(FmtNM::FormatMode::kBlue, file, line));
                        result += ret < 0 ? 0 : ret;
                    }
                    fflush(stream);
                } break;
                case LogLevel::kDebug: {
                    auto* stream{file_.get()};
                    fprintf(stream, "%04d-%02d-%02d %02d:%02d:%02d.%06d %016lx %sD[%s]%s\t%s\n", year, mon, day, hour,
                            min, sec, usec, tid, GenMode(FmtNM::FormatMode::kWhite, key), str.data());
                    result += ret < 0 ? 0 : ret;
                    fflush(stream);
                } break;
                case LogLevel::kInfo: {
                    auto* stream{file_.get()};
                    ret = fprintf(stream, "%04d-%02d-%02d %02d:%02d:%02d.%06d %016lx %sI[%s]%s\t%s\n", year, mon, day,
                                  hour, min, sec, usec, tid, GenMode(FmtNM::FormatMode::kGreen, key), str.data());
                    result += ret < 0 ? 0 : ret;
                    fflush(stream);
                } break;
                case LogLevel::kWarning: {
                    auto* stream{file_.get()};
                    ret = fprintf(stream, "%04d-%02d-%02d %02d:%02d:%02d.%06d %016lx %sW[%s]%s\t%s\n", year, mon, day,
                                  hour, min, sec, usec, tid, GenMode(FmtNM::FormatMode::kYellow, key), str.data());
                    result += ret < 0 ? 0 : ret;
                    if (*func != '\0') {
                        ret = fprintf(stream, "\t%s%s%s\n", GenMode(FmtNM::FormatMode::kBlue, func));
                        result += ret < 0 ? 0 : ret;
                    }
                    if (*file != '\0') {
                        ret = fprintf(stream, "\t%s%s:%d%s\n", GenMode(FmtNM::FormatMode::kBlue, file, line));
                        result += ret < 0 ? 0 : ret;
                    }
                    fflush(stream);
                } break;
                case LogLevel::kError: {
                    auto* stream{file_.get()};
                    ret = fprintf(stream, "%04d-%02d-%02d %02d:%02d:%02d.%06d %016lx %sE[%s]%s\t%s\n", year, mon, day,
                                  hour, min, sec, usec, tid, GenMode(FmtNM::FormatMode::kRed, key), str.data());
                    result += ret < 0 ? 0 : ret;
                    if (*func != '\0') {
                        ret = fprintf(stream, "\t%s%s%s\n", GenMode(FmtNM::FormatMode::kBlue, func));
                        result += ret < 0 ? 0 : ret;
                    }
                    if (*file != '\0') {
                        ret = fprintf(stream, "\t%s%s:%d%s\n", GenMode(FmtNM::FormatMode::kBlue, file, line));
                        result += ret < 0 ? 0 : ret;
                    }
                    fflush(stream);
                } break;
                case LogLevel::kFatal: {
                    auto* stream{file_.get()};
                    ret = fprintf(stream, "%04d-%02d-%02d %02d:%02d:%02d.%06d %016lx %sF[%s]%s\t%s\n", year, mon, day,
                                  hour, min, sec, usec, tid, GenMode(FmtNM::FormatMode::kMagenta, key), str.data());
                    result += ret < 0 ? 0 : ret;
                    if (*func != '\0') {
                        ret = fprintf(stream, "\t%s%s%s\n", GenMode(FmtNM::FormatMode::kBlue, func));
                        result += ret < 0 ? 0 : ret;
                    }
                    if (*file != '\0') {
                        ret = fprintf(stream, "\t%s%s:%d%s\n", GenMode(FmtNM::FormatMode::kBlue, file, line));
                        result += ret < 0 ? 0 : ret;
                    }
                    fflush(stream);
                } break;
                default: {
                    return -__LINE__;
                }
            }
        }
        if (EnableConsole()) {
            switch (level) {
                case LogLevel::kTrace: {
                    auto* stream{stdout};
                    ret = fprintf(stream, "%04d-%02d-%02d %02d:%02d:%02d.%06d %016lx %sT[%s]%s\t%s\n", year, mon, day,
                                  hour, min, sec, usec, tid, GenMode(FmtNM::FormatMode::kBlack, key), str.data());
                    result += ret < 0 ? 0 : ret;
                    if (*func != '\0') {
                        ret = fprintf(stream, "\t%s%s%s\n", GenMode(FmtNM::FormatMode::kBlue, func));
                        result += ret < 0 ? 0 : ret;
                    }
                    if (*file != '\0') {
                        ret = fprintf(stream, "\t%s%s:%d%s\n", GenMode(FmtNM::FormatMode::kBlue, file, line));
                        result += ret < 0 ? 0 : ret;
                    }
                    fflush(stream);
                } break;
                case LogLevel::kDebug: {
                    auto* stream{stdout};
                    fprintf(stream, "%04d-%02d-%02d %02d:%02d:%02d.%06d %016lx %sD[%s]%s\t%s\n", year, mon, day, hour,
                            min, sec, usec, tid, GenMode(FmtNM::FormatMode::kWhite, key), str.data());
                    result += ret < 0 ? 0 : ret;
                    fflush(stream);
                } break;
                case LogLevel::kInfo: {
                    auto* stream{stdout};
                    ret = fprintf(stream, "%04d-%02d-%02d %02d:%02d:%02d.%06d %016lx %sI[%s]%s\t%s\n", year, mon, day,
                                  hour, min, sec, usec, tid, GenMode(FmtNM::FormatMode::kGreen, key), str.data());
                    result += ret < 0 ? 0 : ret;
                    fflush(stream);
                } break;
                case LogLevel::kWarning: {
                    auto* stream{stdout};
                    ret = fprintf(stream, "%04d-%02d-%02d %02d:%02d:%02d.%06d %016lx %sW[%s]%s\t%s\n", year, mon, day,
                                  hour, min, sec, usec, tid, GenMode(FmtNM::FormatMode::kYellow, key), str.data());
                    result += ret < 0 ? 0 : ret;
                    if (*func != '\0') {
                        ret = fprintf(stream, "\t%s%s%s\n", GenMode(FmtNM::FormatMode::kBlue, func));
                        result += ret < 0 ? 0 : ret;
                    }
                    if (*file != '\0') {
                        ret = fprintf(stream, "\t%s%s:%d%s\n", GenMode(FmtNM::FormatMode::kBlue, file, line));
                        result += ret < 0 ? 0 : ret;
                    }
                    fflush(stream);
                } break;
                case LogLevel::kError: {
                    auto* stream{stderr};
                    ret = fprintf(stream, "%04d-%02d-%02d %02d:%02d:%02d.%06d %016lx %sE[%s]%s\t%s\n", year, mon, day,
                                  hour, min, sec, usec, tid, GenMode(FmtNM::FormatMode::kRed, key), str.data());
                    result += ret < 0 ? 0 : ret;
                    if (*func != '\0') {
                        ret = fprintf(stream, "\t%s%s%s\n", GenMode(FmtNM::FormatMode::kBlue, func));
                        result += ret < 0 ? 0 : ret;
                    }
                    if (*file != '\0') {
                        ret = fprintf(stream, "\t%s%s:%d%s\n", GenMode(FmtNM::FormatMode::kBlue, file, line));
                        result += ret < 0 ? 0 : ret;
                    }
                    fflush(stream);
                } break;
                case LogLevel::kFatal: {
                    auto* stream{stderr};
                    ret = fprintf(stream, "%04d-%02d-%02d %02d:%02d:%02d.%06d %016lx %sF[%s]%s\t%s\n", year, mon, day,
                                  hour, min, sec, usec, tid, GenMode(FmtNM::FormatMode::kMagenta, key), str.data());
                    result += ret < 0 ? 0 : ret;
                    if (*func != '\0') {
                        ret = fprintf(stream, "\t%s%s%s\n", GenMode(FmtNM::FormatMode::kBlue, func));
                        result += ret < 0 ? 0 : ret;
                    }
                    if (*file != '\0') {
                        ret = fprintf(stream, "\t%s%s:%d%s\n", GenMode(FmtNM::FormatMode::kBlue, file, line));
                        result += ret < 0 ? 0 : ret;
                    }
                    fflush(stream);
                } break;
                default: {
                    return -__LINE__;
                }
            }
        }
        if (EnableNetwork()) {
        }
        return result;
    }
};
std::unique_ptr< Logger > Logger::Create(ara::core::StringView ctxId) noexcept
{
    auto logger{std::make_unique< LoggerImpl >()};
    LoadConfig(*logger, ctxId);
    return logger;
}

std::unique_ptr< Logger > LoggerCom::Create() noexcept { return Logger::Create("#COM"); }

class LoggerNaiImpl
    : public LoggerNai
    , public LoggerImpl
{
private:
    /// @brief
    bool enableNaiLog_{true};
    /// @brief
    nai_log_t* naiLog_{};
    /// @brief
    decltype(naiLog_->cb) naiLogCB_{};

public:
    LoggerNaiImpl() = default;
    ~LoggerNaiImpl() noexcept override { SetNaiLog(nullptr); }
    /// @brief Copy constructor
    /// @param other
    LoggerNaiImpl(LoggerNaiImpl const& other) noexcept = default;
    /// @brief Move constructor
    /// @param other
    LoggerNaiImpl(LoggerNaiImpl&& other) noexcept = default;
    /// @brief Copy assignment function
    /// @param other
    /// @return LoggerNaiImpl
    LoggerNaiImpl& operator=(LoggerNaiImpl const& other) noexcept = default;
    /// @brief Move assignment function
    /// @param other
    /// @return LoggerNaiImpl
    LoggerNaiImpl& operator=(LoggerNaiImpl&& other) noexcept = default;
    /// @brief SetLevel
    /// @param[in] level
    /// @return bool
    bool SetLevel(LogLevel level) noexcept override
    {
        bool r{true};
        if (naiLog_ != nullptr) {
            switch (level) {
                case LogLevel::kTrace: {  // NOLINT -- compatibility>[bugprone-branch-clone]
                    naiLog_->level = static_cast< uint32_t >(NAI_LOG_DEBUG);
                } break;
                case LogLevel::kDebug: {
                    naiLog_->level = static_cast< uint32_t >(NAI_LOG_DEBUG);
                } break;
                case LogLevel::kInfo: {
                    naiLog_->level = static_cast< uint32_t >(NAI_LOG_INFO);
                } break;
                case LogLevel::kWarning: {
                    naiLog_->level = static_cast< uint32_t >(NAI_LOG_WARN);
                } break;
                case LogLevel::kError: {  // NOLINT -- compatibility>[bugprone-branch-clone]
                    naiLog_->level = static_cast< uint32_t >(NAI_LOG_ERROR);
                } break;
                case LogLevel::kFatal: {
                    naiLog_->level = static_cast< uint32_t >(NAI_LOG_ERROR);
                } break;
                case LogLevel::kNone: {
                    naiLog_->level = 0;
                } break;
                default: {
                    r = false;
                }
            }
        }
        if (r) {
            r = LoggerImpl::SetLevel(level);
        }
        return r;
    }

    /// @brief EnableNaiLog
    /// @param[in] enable
    /// @return bool
    bool EnableNaiLog(bool enable) noexcept override
    {
        enableNaiLog_ = enable;
        return EnableNaiLog();
    }
    /// @brief EnableNaiLog
    /// @return bool
    bool EnableNaiLog() const noexcept override { return enableNaiLog_ && naiLog_ != nullptr; }
    /// @brief SetNaiLog
    /// @param[in] naiLog
    /// @return bool
    bool SetNaiLog(nai_log_t* naiLog) noexcept override
    {
        if (naiLog_ == naiLog) {
            return false;
        }
        if (naiLog_ != nullptr) {
            naiLog_->cb = naiLogCB_;
        }
        naiLog_ = naiLog;
        if (naiLog_ != nullptr) {
            naiLogCB_   = naiLog_->cb;
            naiLog_->cb = &LoggerNaiImpl::OnLog;
        }
        return true;
    }
    /// @brief GetNaiLog
    /// @return nai_log_t*
    nai_log_t* GetNaiLog() const noexcept override { return naiLog_; }

private:
    /// @brief Errno2Str
    /// @param[in] err
    /// @return char const*
    static char const* Errno2Str(int32_t err) noexcept
    {
        if (err < NAI_ECUSTOM_START) {
            return strerror(err);  // NOLINT -- compatibility>[concurrency-mt-unsafe]
        }
        char const* r{};
        switch (err) {
            case NAI_EFAILED: {
                r = "Generally failed";
            } break;
            case NAI_ETYPE: {
                r = "Invalid type";
            } break;
            case NAI_EOPERATION: {
                r = "Invalid opeartion";
            } break;
            case NAI_ESETTING: {
                r = "Invalid setting";
            } break;
            case NAI_ECLOSED: {
                r = "Already closed";
            } break;
            case NAI_EINTERNAL: {
                r = "Internal error";
            } break;
            case NAI_ECORRUPTION: {
                r = "Data corruption";
            } break;
            case NAI_ESINGLETON: {
                r = "Singleton";
            } break;
            case NAI_ENODRI: {
                r = "No driver";
            } break;
            case NAI_EDEVICE_FUNCTION: {
                r = "Invalid device function";
            } break;
            case NAI_EDRIVER_INTERNAL: {
                r = "Driver internal error";
            } break;
            case NAI_EDRIVER_INITIALIZE: {
                r = "Driver uninitialize";
            } break;
            case NAI_EARCHIVE: {
                r = "Archive error";
            } break;
            case NAI_EVERSION: {
                r = "Version error";
            } break;
#if !defined(ESHUTDOWN)
            case NAI_ESHUTDOWN: {
                r = "Can't send after socket shutdown";
            } break;
#endif
            default: {
                r = "";
            } break;
        };
        return r;
    }

    /// @brief OnLog
    /// @param[in] l
    /// @param[in] func
    /// @param[in] line
    /// @param[in] lvl
    /// @param[in] err
    /// @param[in] fmt
    /// @param[in] va
    /// @return int32_t
    static int32_t OnLog(
        nai_log_t* l, char const* func, int32_t line, int32_t lvl, int32_t err, char const* fmt, va_list va) noexcept
    {
        std::ignore = l;
        constexpr LogLevel kLevels[]{
            LogLevel::kError,    // EMERG
            LogLevel::kError,    // ALERT
            LogLevel::kError,    // CRIT
            LogLevel::kError,    // ERROR
            LogLevel::kWarning,  // WARN
            LogLevel::kInfo,     // NOTICE
            LogLevel::kInfo,     // INFO
            LogLevel::kDebug     // DEBUG
        };
        auto level{kLevels[lvl - 1]};  // NOLINT -> [cppcoreguidelines-pro-bounds-constant-array-index]
        auto& logger{Logger::GetInstance< LoggerNai >()};
        if (!logger.EnableLevel(level)) {
            return 0;
        }
        if (!logger.EnableNaiLog()) {
            return 0;
        }
        // extend info
        ara::core::String str;
        char const* errstr{};
        if (err == 0) {
        } else if ((errstr = Errno2Str(err)) != nullptr) {
            FmtNM::FormatToString(str, "errno[", err, "]: ", errstr, ", ");
        } else {
            FmtNM::FormatToString(str, "errno[", err, "], ");
        }
        // compute length
        va_list va2;
        va_copy(va2, va);
        auto len{nai_vsnprintf(nullptr, 0, fmt, va2)};
        va_end(va2);
        if (len <= 0) {
            return len;
        }
        // output
        str.resize(str.size() + static_cast< size_t >(len));
        std::ignore = nai_vsnprintf(&str.back() - len + 1, len, fmt, va);
        constexpr size_t kSize{128};
        char key[kSize];
        snprintf(key, sizeof(key) - 1, "%s:%d", func, line);
        return logger.Write(str, level, key);
    }
};
std::unique_ptr< LoggerNai > LoggerNai::Create() noexcept
{
    auto logger{std::make_unique< LoggerNaiImpl >()};
    LoadConfig(*logger, "#NAI");
    return logger;
}

#ifdef HAS_MONITOR_LOG
std::unique_ptr< Logger > LoggerComMonitor::Create() noexcept { return Logger::Create("#CMM"); }
#endif
}  // namespace log
}  // namespace internal
}  // namespace com
}  // namespace ara