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
/// @file       log_manager_config_module.cpp
/// @brief
/// @details
/// @date       2025-09-03
/// @author     yangjinbiao
/// @version    1.2.0
///
/// ================================================================

#include "DltLog/log_manager_config_module.h"

#include <isoft/ara_fsh/process.h>
#include <rapidjson/document.h>
#include <unistd.h>

#include <bitset>
#include <cstdio>
#include <fstream>
#include <sstream>

#include "DltLogDefines/dlt_default_config.h"
#include "Utils/src/private_log.h"

namespace ara {
namespace log {
namespace internal {

std::string LogManagerConfigModule::GetCurrentExecutableDir() noexcept
{
    constexpr std::size_t kInt32_1024U{1024U};
#if defined(__linux__)
    char result[kInt32_1024U]{0};
    ssize_t count = readlink("/proc/self/exe", result, sizeof(result));
    if (count > 0) {
        std::string filePath(result, count);

        size_t lastSlashPos = filePath.find_last_of("/\\");
        if (lastSlashPos != std::string::npos) {
            return filePath.substr(0, lastSlashPos + 1);
        }
    }
    return "./";
#else
    return "./";
#endif
}

bool LogManagerConfigModule::FileExists(std::string const &filename) noexcept
{
    std::ifstream file(filename);
    return file.good();
}

bool LogManagerConfigModule::SearchConfigPath(std::string &appConfig, std::string &globalConfig) const
{
    isoft::ara_fsh::Process const proc{};
    std::string aaConfigName{proc.GetLogConfig()};
    std::string globalConfigName{isoft::ara_fsh::Platform().GetPlatformEtcDir() + "/log_global_config.json"};

    std::string fallbackEtcPath{GetCurrentExecutableDir() + "etc/"};

    if (FileExists(aaConfigName) == false) {
        aaConfigName = fallbackEtcPath + "log.json";
        if (FileExists(aaConfigName) == false) {
            return false;
        }
    }

    if (FileExists(globalConfigName) == false) {
        globalConfigName = fallbackEtcPath + "log_global_config.json";
    }

    appConfig    = aaConfigName;
    globalConfig = globalConfigName;
    return true;
}

bool LogManagerConfigModule::ReadGlobalConfig(std::shared_ptr< ApplicationConfig > const &logConfig,
                                              std::string const &configPath) const noexcept
{
    if (logConfig == nullptr) {
        return false;
    }

    std::ifstream const ifs{configPath};
    std::stringstream buffer;
    buffer << ifs.rdbuf();

    if (buffer.str().empty()) {
        return false;
    }
    rapidjson::Document d;
    std::string const datastr(buffer.str());

    if (d.Parse(datastr.c_str()).HasParseError() == false) {
        if (d.HasMember("ecuId") && d["ecuId"].IsString()) {
            logConfig->ecuId = d["ecuId"].GetString();
        }

        if (d.HasMember("queueSize") && d["queueSize"].IsUint()) {
            logConfig->queueSize = static_cast< std::size_t >(d["queueSize"].GetUint());
        }
        if (d.HasMember("sessionIdSupport") && d["sessionIdSupport"].IsBool()) {
            logConfig->sessionIdSupport = d["sessionIdSupport"].GetBool();
            if (logConfig->sessionIdSupport) {
                logConfig->sessionId = static_cast< std::uint32_t >(getpid());
            }
        }
        if (d.HasMember("timebaseName") && d["timebaseName"].IsString()) {
            logConfig->timebaseName = d["timebaseName"].GetString();
        }

        if (d.HasMember("logconsolenobuffer") && d["logconsolenobuffer"].IsBool()) {
            bool const bNoBuffer{d["logconsolenobuffer"].GetBool()};
            if (bNoBuffer) {
                std::ignore = setvbuf(stdout, nullptr, _IONBF, 0U);
            }
        }
    }
    return true;
}

bool LogManagerConfigModule::ReadAppConfig(std::shared_ptr< ApplicationConfig > const &logConfig,
                                           std::string const &configPath) const noexcept
{
    if (logConfig == nullptr) {
        return false;
    }

    std::ifstream const ifs{configPath};
    std::stringstream buffer;
    buffer << ifs.rdbuf();

    if (buffer.str().empty()) {
        LOGERROR(__func__) << "json error !";
        return false;
    }

    rapidjson::Document d;
    std::string const datastr{buffer.str()};
    if (d.Parse(datastr.c_str()).HasParseError() == false) {
        if (d.HasMember("dltVersion") && d["dltVersion"].IsString()) {
            logConfig->dltVersion = d["dltVersion"].GetString();
        }
    }

    if (logConfig->dltVersion == "2.0") {
        return ReadAppConfigV2(logConfig, configPath);
    }

    return ReadAppConfigV1(logConfig, configPath);
}

bool LogManagerConfigModule::ParseV1Channel(std::shared_ptr< ApplicationConfig > const &logConfig,
                                            rapidjson::Value &channelObj,
                                            bool appEnableCompress,
                                            bool appPlainText,
                                            std::string const &fileEndSuf) const noexcept
{
    DLTV2::DltContextConfigPtr contextPtr{std::make_shared< DLTV2::DltContextConfig >()};
    if (channelObj.HasMember("ctxId") && channelObj["ctxId"].IsString()) {
        contextPtr->contextId = (channelObj["ctxId"].GetString());
    }
    if (contextPtr->contextId.empty()) {
        return false;
    }

    contextPtr->applicationId          = logConfig->appid;
    contextPtr->applicationDescription = logConfig->appdesc;
    contextPtr->enableCompress         = appEnableCompress;

    std::uint8_t contextLevel{3U};
    if (channelObj.HasMember("level") && channelObj["level"].IsUint()) {
        contextLevel = static_cast< std::uint8_t >(channelObj["level"].GetUint());
    }

    if (channelObj.HasMember("ctxDesc") && channelObj["ctxDesc"].IsString()) {
        contextPtr->contextDescription = (channelObj["ctxDesc"].GetString());
    }
    if (channelObj.HasMember("sessionId") && channelObj["sessionId"].IsUint()) {
        contextPtr->sessionId = (channelObj["sessionId"].GetUint());
    }

    if (channelObj.HasMember("determChannel") && channelObj["determChannel"].IsBool()) {
        contextPtr->determConfiged = (channelObj["determChannel"].GetBool());
    }

    if (contextPtr->determConfiged) {
        logConfig->appHaveDetermLogger = true;
        if (0U == logConfig->determFreeCount) {
            logConfig->determFreeCount = DLT_DEFAULT_CONFIG.defDetermFreeCount;
        }
    }
    bool nonVerboseMode{true};
    if (channelObj.HasMember("nonVerboseMode") && channelObj["nonVerboseMode"].IsBool()) {
        nonVerboseMode = (channelObj["nonVerboseMode"].GetBool());
    }

    bool contextEnableCompress{appEnableCompress};
    if (channelObj.HasMember("enableCompress") && channelObj["enableCompress"].IsBool()) {
        contextEnableCompress = (channelObj["enableCompress"].GetBool());
    }

    bool contextPlainText{appPlainText};
    if (channelObj.HasMember("plainText") && channelObj["plainText"].IsBool()) {
        contextPlainText = (channelObj["plainText"].GetBool());
    }

    if (channelObj.HasMember("modes") && channelObj["modes"].IsUint()) {
        std::uint8_t const logmode{static_cast< std::uint8_t >(channelObj["modes"].GetUint())};
        std::uint8_t const bitWid{8U};
        std::bitset< bitWid > const bitmode{logmode};
        if (bitmode.test(0U)) {
            logConfig->useDaemon                                 = true;
            std::shared_ptr< DLTV2::DltSinkersConfig > sinkerPtr = std::make_shared< DLTV2::DltSinkersConfig >();
            sinkerPtr->sinkerType                                = kDLT_LOGSINK_REMOTE;
            sinkerPtr->defaultLogThreshold                       = contextLevel;

            contextPtr->sinkerLevelMap[sinkerPtr->sinkerType] = sinkerPtr->defaultLogThreshold;
            contextPtr->sinkersMap[sinkerPtr->sinkerType]     = sinkerPtr;
        }
        if (bitmode.test(1U)) {
            std::shared_ptr< DLTV2::DltSinkersConfig > sinkerPtr = std::make_shared< DLTV2::DltSinkersConfig >();
            sinkerPtr->sinkerType                                = kDLT_LOGSINK_FILE;
            sinkerPtr->nonVerboseMode                            = nonVerboseMode;
            sinkerPtr->defaultLogThreshold                       = contextLevel;
            sinkerPtr->enableCompress                            = contextEnableCompress;
            sinkerPtr->plainText                                 = contextPlainText;

            if (channelObj.HasMember("fileName") && channelObj["fileName"].IsString()) {
                std::string const logfilename{channelObj["fileName"].GetString()};
                if (logfilename.empty()) {
                    std::string fileName{logConfig->appid};
                    sinkerPtr->logFileName = fileName.append(fileEndSuf);
                } else {
                    bool const checkEnd{HasValidSuffix(logfilename)};
                    if (checkEnd) {
                        sinkerPtr->logFileName = logfilename;
                    } else {
                        std::string fileName{logfilename};
                        std::ignore            = fileName.append(logConfig->appid);
                        sinkerPtr->logFileName = fileName.append(fileEndSuf);
                    }
                }
            }
            if (channelObj.HasMember("fileCount") && channelObj["fileCount"].IsInt()) {
                sinkerPtr->fileCount = channelObj["fileCount"].GetInt();
            }
            if (0 >= sinkerPtr->fileCount || sinkerPtr->fileCount >= DLT_DEFAULT_CONFIG.maxFileCount) {
                sinkerPtr->fileCount = DLT_DEFAULT_CONFIG.fileCount;
            }
            if (channelObj.HasMember("bufferSize") && channelObj["bufferSize"].IsInt()) {
                sinkerPtr->fileBufferSize = channelObj["bufferSize"].GetInt();
            }
            if (0 >= sinkerPtr->fileBufferSize || sinkerPtr->fileBufferSize >= DLT_DEFAULT_CONFIG.maxFileBufferSize) {
                sinkerPtr->fileBufferSize = DLT_DEFAULT_CONFIG.fileBufferSize;
            }

            if (channelObj.HasMember("fileSize") && channelObj["fileSize"].IsUint()) {
                sinkerPtr->singleFileSize = (channelObj["fileSize"].GetUint());
            }
            if (0 >= sinkerPtr->singleFileSize || sinkerPtr->singleFileSize >= DLT_DEFAULT_CONFIG.maxFingleFileSize) {
                sinkerPtr->singleFileSize = DLT_DEFAULT_CONFIG.singleFileSize;
            }

            if (channelObj.HasMember("fileShared") && channelObj["fileShared"].IsBool()) {
                sinkerPtr->fileShared = (channelObj["fileShared"].GetBool());
            }

            contextPtr->sinkerLevelMap[sinkerPtr->sinkerType] = sinkerPtr->defaultLogThreshold;
            contextPtr->sinkersMap[sinkerPtr->sinkerType]     = sinkerPtr;
        }

        if (bitmode.test(2U)) {
            std::shared_ptr< DLTV2::DltSinkersConfig > sinkerPtr = std::make_shared< DLTV2::DltSinkersConfig >();
            sinkerPtr->sinkerType                                = kDLT_LOGSINK_CONSOLE;
            sinkerPtr->nonVerboseMode                            = nonVerboseMode;
            sinkerPtr->defaultLogThreshold                       = contextLevel;
            sinkerPtr->bufferOutput                              = false;
            contextPtr->sinkerLevelMap[sinkerPtr->sinkerType]    = sinkerPtr->defaultLogThreshold;
            contextPtr->sinkersMap[sinkerPtr->sinkerType]        = sinkerPtr;
        }
    }

    logConfig->dltV2Config[contextPtr->contextId] = contextPtr;

    bool bConsole{false};
    if (contextPtr->sinkersMap.end() != contextPtr->sinkersMap.find(kDLT_LOGSINK_CONSOLE)) {
        bConsole = true;
    }
    bool bFile{false};
    if (contextPtr->sinkersMap.end() != contextPtr->sinkersMap.find(kDLT_LOGSINK_FILE)) {
        bFile = true;
    }
    if (bConsole && bFile && contextPtr->determConfiged) {
        logConfig->startConsoleThreadIfFileExist = true;
    }
    return true;
}

bool LogManagerConfigModule::ReadAppConfigV1(std::shared_ptr< ApplicationConfig > const &logConfig,
                                             std::string const &configPath) const noexcept
{
    std::ifstream const ifs{configPath};
    std::stringstream buffer;
    buffer << ifs.rdbuf();

    if (buffer.str().empty()) {
        LOGERROR(__func__) << "json error";
        return false;
    }

    rapidjson::Document d;
    std::string const datastr{buffer.str()};

    if (d.Parse(datastr.c_str()).HasParseError() == false) {
        if (d.HasMember("appId") && d["appId"].IsString()) {
            logConfig->appid = NormalizeIdentifier(d["appId"].GetString());
        } else {
            LOGERROR(__func__) << "no appid ";
            return false;
        }
        if (d.HasMember("loggingBehavior") && d["loggingBehavior"].IsBool()) {
            logConfig->loggingBehavior = d["loggingBehavior"].GetBool();
        }

        if (d.HasMember("defaultLevel") && d["defaultLevel"].IsInt()) {
            logConfig->defaultLogLevel = static_cast< std::uint8_t >(d["defaultLevel"].GetInt());
        }
        if (d.HasMember("appDesc") && d["appDesc"].IsString()) {
            logConfig->appdesc = d["appDesc"].GetString();
        }
        if (d.HasMember("determFreeCount") && d["determFreeCount"].IsUint64()) {
            logConfig->determFreeCount = d["determFreeCount"].GetUint64();
        }
        if (d.HasMember("doStatis") && d["doStatis"].IsBool()) {
            logConfig->mDoStatis = d["doStatis"].GetBool();
        }
        if (d.HasMember("internalLogLevel") && d["internalLogLevel"].IsUint()) {
            logConfig->internallogLevel = static_cast< std::uint8_t >(d["internalLogLevel"].GetUint());
            PrivateLogger::SetPrivateLogLevel(static_cast< PriLogLevelType >(logConfig->internallogLevel));
            PrivateLogger::Set_Appid(logConfig->appid);
        }

        bool appEnableCompress{false};
        if (d.HasMember("enableCompress") && d["enableCompress"].IsBool()) {
            appEnableCompress = (d["enableCompress"].GetBool());
        }

        bool appPlainText{false};
        if (d.HasMember("plainText") && d["plainText"].IsBool()) {
            appPlainText = (d["plainText"].GetBool());
        }

        bool const checkCArr{d.HasMember("channels") && d["channels"].IsArray()};

        std::string const fileEndSuf{".dlt"};
        if (d.HasMember("channels") && checkCArr) {
            for (rapidjson::Value &v : d["channels"].GetArray()) {
                if (v.IsObject()) {
                    std::ignore = ParseV1Channel(logConfig, v, appEnableCompress, appPlainText, fileEndSuf);
                }
            }
        } else {
            LOGERROR(__func__) << "log.json error";
        }
    }
    return true;
}

bool LogManagerConfigModule::ParseV2Context(std::shared_ptr< ApplicationConfig > const &logConfig,
                                            rapidjson::Value &contextObj,
                                            bool appEnableCompress,
                                            bool appPlainText,
                                            std::uint32_t defSingleFileSize,
                                            std::uint32_t defFileCount,
                                            std::uint32_t defileBuffer,
                                            std::uint32_t defDetermFreeCount) const noexcept
{
    DLTV2::DltContextConfigPtr contextPtr{std::make_shared< DLTV2::DltContextConfig >()};
    contextPtr->applicationId = logConfig->appid;

    if (contextObj.HasMember("ctxId") && contextObj["ctxId"].IsString()) {
        contextPtr->contextId = (contextObj["ctxId"].GetString());
    }
    if (contextPtr->contextId.empty()) {
        return false;
    }

    if (contextObj.HasMember("ctxDesc") && contextObj["ctxDesc"].IsString()) {
        contextPtr->contextDescription = (contextObj["ctxDesc"].GetString());
    }

    if (contextObj.HasMember("determChannel") && contextObj["determChannel"].IsBool()) {
        contextPtr->determConfiged = (contextObj["determChannel"].GetBool());
        if (contextPtr->determConfiged) {
            logConfig->appHaveDetermLogger = true;
            if (0U == logConfig->determFreeCount) {
                logConfig->determFreeCount = defDetermFreeCount;
            }
        }
    }

    bool contextEnableCompress{appEnableCompress};
    if (contextObj.HasMember("enableCompress") && contextObj["enableCompress"].IsBool()) {
        contextEnableCompress = (contextObj["enableCompress"].GetBool());
    }

    bool contextPlainText{appPlainText};
    if (contextObj.HasMember("plainText") && contextObj["plainText"].IsBool()) {
        contextPlainText = (contextObj["plainText"].GetBool());
    }

    if (contextObj.HasMember("dltSessionId") && contextObj["dltSessionId"].IsUint()) {
        contextPtr->sessionId = (contextObj["dltSessionId"].GetUint());
    }

    if (contextObj.HasMember("sinkers") && contextObj["sinkers"].IsArray()) {
        for (rapidjson::Value &sinkerObj : contextObj["sinkers"].GetArray()) {
            if (sinkerObj.IsObject()) {
                std::shared_ptr< DLTV2::DltSinkersConfig > sinkerPtr = std::make_shared< DLTV2::DltSinkersConfig >();
                if (sinkerObj.HasMember("type") && sinkerObj["type"].IsString()) {
                    sinkerPtr->sinkerType = sinkerObj["type"].GetString();
                }
                if (sinkerObj.HasMember("defaultTraceState") && sinkerObj["defaultTraceState"].IsBool()) {
                    sinkerPtr->defaultTraceState = (sinkerObj["defaultTraceState"].GetBool());
                }

                if (sinkerObj.HasMember("nonVerboseMode") && sinkerObj["nonVerboseMode"].IsBool()) {
                    sinkerPtr->nonVerboseMode = (sinkerObj["nonVerboseMode"].GetBool());
                }

                if (sinkerObj.HasMember("defaultLogThreshold") && sinkerObj["defaultLogThreshold"].IsUint()) {
                    sinkerPtr->defaultLogThreshold = (sinkerObj["defaultLogThreshold"].GetUint());
                }

                contextPtr->sinkerLevelMap[sinkerPtr->sinkerType] = sinkerPtr->defaultLogThreshold;
                if (sinkerPtr->sinkerType == kDLT_LOGSINK_FILE) {
                    if (sinkerObj.HasMember("fileSize") && sinkerObj["fileSize"].IsUint()) {
                        sinkerPtr->singleFileSize = (sinkerObj["fileSize"].GetUint());
                    } else {
                        sinkerPtr->singleFileSize = defSingleFileSize;
                    }

                    sinkerPtr->enableCompress = contextEnableCompress;
                    sinkerPtr->plainText      = contextPlainText;

                    if (sinkerObj.HasMember("fileName") && sinkerObj["fileName"].IsString()) {
                        std::string const logfilename{sinkerObj["fileName"].GetString()};
                        if (logfilename.empty()) {
                            std::string fileName{logConfig->appid};
                            sinkerPtr->logFileName = fileName.append(".dlt");
                        } else {
                            bool const checkEnd{HasValidSuffix(logfilename)};
                            if (checkEnd) {
                                sinkerPtr->logFileName = logfilename;
                            } else {
                                std::string fileName{logfilename};
                                std::ignore            = fileName.append(logConfig->appid);
                                sinkerPtr->logFileName = fileName.append(".dlt");
                            }
                        }
                    }
                    sinkerPtr->fileCount            = defFileCount;
                    std::uint8_t const maxFileCount = 15;
                    if (sinkerObj.HasMember("fileCount") && sinkerObj["fileCount"].IsInt()) {
                        std::int32_t const origCount{sinkerObj["fileCount"].GetInt()};
                        if (origCount > 0 && origCount < maxFileCount) {
                            sinkerPtr->fileCount = origCount;
                        }
                    }

                    sinkerPtr->fileBufferSize = defileBuffer;

                    if (sinkerObj.HasMember("bufferSize") && sinkerObj["bufferSize"].IsInt()) {
                        sinkerPtr->fileBufferSize = sinkerObj["bufferSize"].GetInt();
                    }

                    if (0 >= sinkerPtr->fileBufferSize
                        || sinkerPtr->fileBufferSize >= DLT_DEFAULT_CONFIG.maxFileBufferSize) {
                        sinkerPtr->fileBufferSize = DLT_DEFAULT_CONFIG.fileBufferSize;
                    }

                    if (sinkerObj.HasMember("fileShared") && sinkerObj["fileShared"].IsBool()) {
                        sinkerPtr->fileShared = (sinkerObj["fileShared"].GetBool());
                    } else {
                        sinkerPtr->fileShared = true;
                    }
                }

                if (sinkerPtr->sinkerType == kDLT_LOGSINK_REMOTE) {
                    logConfig->useDaemon = true;
                }
                if (sinkerPtr->sinkerType == kDLT_LOGSINK_CONSOLE) {
                    if (sinkerObj.HasMember("bufferOutput") && sinkerObj["bufferOutput"].IsBool()) {
                        sinkerPtr->bufferOutput = (static_cast< bool >(sinkerObj["bufferOutput"].GetBool()));
                    }
                    if (sinkerObj.HasMember("defaultLogThreshold") && sinkerObj["defaultLogThreshold"].IsUint()) {
                        sinkerPtr->defaultLogThreshold = sinkerObj["defaultLogThreshold"].GetUint();
                    }
                    if (sinkerObj.HasMember("nonVerboseMode") && sinkerObj["nonVerboseMode"].IsBool()) {
                        sinkerPtr->nonVerboseMode = (sinkerObj["nonVerboseMode"].GetBool());
                    }
                }

                contextPtr->sinkersMap.insert({sinkerPtr->sinkerType, sinkerPtr});
            }
        }
    }

    logConfig->dltV2Config[contextPtr->contextId] = contextPtr;

    bool bConsole{false};
    if (contextPtr->sinkersMap.end() != contextPtr->sinkersMap.find(kDLT_LOGSINK_CONSOLE)) {
        bConsole = true;
    }
    bool bFile{false};
    if (contextPtr->sinkersMap.end() != contextPtr->sinkersMap.find(kDLT_LOGSINK_FILE)) {
        bFile = true;
    }
    if (bConsole && bFile && contextPtr->determConfiged) {
        logConfig->startConsoleThreadIfFileExist = true;
    }

    return true;
}

bool LogManagerConfigModule::ReadAppConfigV2(std::shared_ptr< ApplicationConfig > const &logConfig,
                                             std::string const &configPath) const noexcept
{
    std::ifstream const ifs{configPath};
    std::stringstream buffer;
    buffer << ifs.rdbuf();

    if (buffer.str().empty()) {
        return false;
    }

    std::uint32_t const defileBuffer{0U};
    std::uint32_t const defSingleFileSize{1024U * 1024 * 10U};
    std::uint32_t const defFileCount{5U};
    std::uint32_t const defDetermFreeCount{1024U};

    rapidjson::Document d;
    std::string const datastr{buffer.str()};

    if (d.Parse(datastr.c_str()).HasParseError() == false) {
        if (d.HasMember("appId") && d["appId"].IsString()) {
            logConfig->appid = NormalizeIdentifier(d["appId"].GetString());
        } else {
            return false;
        }

        if (d.HasMember("loggingBehavior") && d["loggingBehavior"].IsBool()) {
            logConfig->loggingBehavior = d["loggingBehavior"].GetBool();
        }

        if (d.HasMember("appDesc") && d["appDesc"].IsString()) {
            logConfig->appdesc = d["appDesc"].GetString();
        }
        if (d.HasMember("determFreeCount") && d["determFreeCount"].IsUint64()) {
            logConfig->determFreeCount = d["determFreeCount"].GetUint64();
        }
        if (d.HasMember("doStatis") && d["doStatis"].IsBool()) {
            logConfig->mDoStatis = d["doStatis"].GetBool();
        }
        if (d.HasMember("internalLogLevel") && d["internalLogLevel"].IsUint()) {
            logConfig->internallogLevel = static_cast< std::uint8_t >(d["internalLogLevel"].GetUint());
            PrivateLogger::SetPrivateLogLevel(static_cast< PriLogLevelType >(logConfig->internallogLevel));
            PrivateLogger::Set_Appid(logConfig->appid);
        }
        if (d.HasMember("defaultLevel") && d["defaultLevel"].IsInt()) {
            logConfig->defaultLogLevel = static_cast< std::uint8_t >(d["defaultLevel"].GetInt());
        }
        bool appEnableCompress{false};
        if (d.HasMember("enableCompress") && d["enableCompress"].IsBool()) {
            appEnableCompress = (d["enableCompress"].GetBool());
        }

        bool appPlainText{false};
        if (d.HasMember("plainText") && d["plainText"].IsBool()) {
            appPlainText = (d["plainText"].GetBool());
        }
        if (d.HasMember("Contexts") && d["Contexts"].IsArray()) {
            for (rapidjson::Value &contextOBj : d["Contexts"].GetArray()) {
                if (contextOBj.IsObject()) {
                    std::ignore = ParseV2Context(logConfig, contextOBj, appEnableCompress, appPlainText,
                                                 defSingleFileSize, defFileCount, defileBuffer, defDetermFreeCount);
                }
            }
        }
    }
    return true;
}

bool LogManagerConfigModule::HasValidSuffix(std::string const &str) noexcept
{
    std::size_t const dotPos{str.find_last_of('.')};
    bool const hasSuffix{(dotPos != std::string::npos) && (dotPos < str.size() - 1U) && (dotPos > 0U)};
    return hasSuffix;
}

std::string LogManagerConfigModule::NormalizeIdentifier(std::string const &identifier) noexcept
{
    if (identifier.empty()) {
        return "DEFT";
    }

    std::string result;
    std::size_t const kMaxLen{4};

    for (char c : identifier) {
        if (result.length() >= kMaxLen) {
            break;
        }

        if (c == '#') {
            result.push_back('_');
        } else if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_') {
            result.push_back(c);
        }
    }

    if (result.empty()) {
        return "DEFT";
    }

    return result;
}

}  // namespace internal
}  // namespace log
}  // namespace ara