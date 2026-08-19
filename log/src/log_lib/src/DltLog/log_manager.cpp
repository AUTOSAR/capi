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
/// @file       log_manager.cpp
/// @brief      Template class for encapsulating log parameters
/// @details
/// @date       2024-06-28
/// @author     yangjinbiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/LOG/DltLog
/// @interface_level = unit
/// @trace_id_sr=LOG_SR_00001,LOG_SR_00002,LOG_SR_00003,LOG_SR_00004,LOG_SR_00006,LOG_SR_00010,LOG_SR_00011,LOG_SR_00012,LOG_SR_00013,LOG_SR_00014
/// @unit_name = log_manager
/// @unit_description=Lib interface side of Dlt, internal support function
/// @endcode
///
/// ================================================================

#include "DltLog/log_manager.h"

#include <ara/core/string_view.h>
#include <isoft/ara_fsh/process.h>
#include <isoft/manifestreader/manifest_reader.h>
#include <nai/runtime/nai_errno.h>
#include <nai/runtime/nai_log.h>
#include <nai/runtime/nai_time.h>
#include <unistd.h>

#include <bitset>
#include <csignal>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <functional>
#include <memory>
#include <string>
#include <tuple>

#include "DltLog/log_manager_config_module.h"
#include "DltLog/log_manager_determ_module.h"
#include "DltLog/log_manager_logger_module.h"
#include "DltLog/nai_log_wrapper.h"
#include "DltLog/time_stamp.h"
#include "DltLogDefines/application_config.h"
#include "DltLogDefines/dlt_default_config.h"
#include "DltLogDefines/dlt_sinkers_config.h"
#include "DltSinkers/file_sinker_no_locker.h"
#include "DltSinkers/file_sinker_withLocker.h"
#include "Utils/src/private_log.h"
#include "common.h"
#include "dlt_logger.h"
#include "logger_class.h"

#ifdef ARA_LOG_HAS_REMOTE
    #include "DltProtocol/log_command_excutor.h"
    #include "DltSinkers/remote_sinker.h"
#endif

namespace ara {
namespace log {

namespace internal {

std::string Get_Exec_Name()
{
    static std::string s_Exec_Name = "";
    if (s_Exec_Name.empty()) {
        char path[PATH_MAX]{0};
        if (readlink("/proc/self/exe", path, sizeof(path)) != -1) {
            s_Exec_Name = std::string(path);
        }
    }
    return s_Exec_Name;
}

/// @brief Destructor
LogManager::~LogManager() noexcept
{
    LOGVERBOSE("enter :") << __func__;
    loggers_.clear();
#ifdef ARA_LOG_HAS_REMOTE
    mlogCommandExcutor_ = nullptr;
#endif
}
/// @brief
/// @return
std::int32_t LogManager::Init() noexcept
{
    LOGVERBOSE("enter LogManager::Init :") << Get_Exec_Name() << __func__;

    PrivateLogger::SetPrivateLogLevel(PriLogLevelType::kDlt_Log_Off);
    this->logConfig_ = std::make_shared< ApplicationConfig >();

    if (configModule_ == nullptr) {
        configModule_ = std::make_shared< LogManagerConfigModule >();
    }

    bool const foundConfigPath{configModule_->SearchConfigPath(appConfig_, globalConfig_)};
    LOGVERBOSE(__func__) << this->appConfig_ << this->globalConfig_;
    if (foundConfigPath == false) {
        return -1;
    }

    bool initGlobalConfig{configModule_->ReadGlobalConfig(logConfig_, this->globalConfig_)};
    if (initGlobalConfig == false) {
        LOGVERBOSE(__func__) << " configuring global configuration file ";
    }
    bool initConfig{configModule_->ReadAppConfig(logConfig_, this->appConfig_)};
    if (initConfig == false) {
        logConfig_->loggingBehavior = false;
        return (EXIT_FAILURE);
    }
    LOGVERBOSE(__func__) << " ok appid : " << logConfig_->appid;

    if (logConfig_->loggingBehavior) {
#ifdef ARA_LOG_HAS_REMOTE
        _createRemoteSinker();
#endif

        /// Reduce thread creation; only enable when deterministic execution is present.
        LOGVERBOSE(__func__) << " logConfig_->appHaveDetermLogger : " << logConfig_->appHaveDetermLogger;
        if (logConfig_->appHaveDetermLogger) {
            LOGERROR(__func__) << " ###start determ ";
            if (determModule_ == nullptr) {
                determModule_ = std::make_shared< LogManagerDetermModule >();
            }
            bool ret = determModule_->CreateDetermMemoryPool(logConfig_);
            if (ret == false) {
                return -1;
            }
            ret = determModule_->CreateDetermThreads(logConfig_);
            if (ret == false) {
                return -1;
            }
        }

        _createLoggerFromConfigFile();
        _configNaiLog();
    }

    LOGVERBOSE(__func__) << logConfig_->appid;

    isInit_ = true;
    return 0;
}

bool LogManager::_createRemoteSinker()
{
    if (logConfig_->useDaemon) {
        LOGVERBOSE(__func__) << ":  use daemon" << logConfig_->appid;
        RemoteSinker::InitInstance(logConfig_->queueSize, logConfig_->appid);
        if (logConfig_->appid != "#EMD") {
            std::ignore = RemoteSinker::Instance()->Init();
        }

        mlogCommandExcutor_ = std::make_shared< LogCommandExcutor >(logConfig_);
        if (mlogCommandExcutor_ == nullptr) {
            LOGERROR(__func__) << " create LogCommandExcutor failed";
            return false;
        }
        mlogCommandExcutor_->SetOnClientstateChangedCallback(
            std::bind(&LogManager::OnClientstateChanged, this, std::placeholders::_1));

        mlogCommandExcutor_->SetOnLogLevelChangedCallback(
            std::bind(&LogManager::OnLogLevelChanged, this, std::placeholders::_1, std::placeholders::_2));

        mlogCommandExcutor_->SetOnRegistStatusChangedCallback(
            std::bind(&LogManager::OnRegistStatusChanged, this, std::placeholders::_1));

        mlogCommandExcutor_->Init();
    }
    return true;
}

bool LogManager::_configNaiLog()
{
    if (naiLogWrapper_ == nullptr) {
        naiLogWrapper_ = std::make_shared< NaiLogWrapper >();
    }

    // No nai log channel configured, default to kError
    std::string const logNaiContextId{"#NAI"};
    std::string naiCtxDesc{"#NAI log"};
    if (logConfig_->dltV2Config.end() == logConfig_->dltV2Config.find(logNaiContextId)) {
        return false;
    }

    // Calculate the nai log level (take the maximum value to be compatible with nai)
    std::uint8_t naiLogLvl{0};
    DLTV2::DltContextConfigPtr naiCtxConfig{logConfig_->dltV2Config[logNaiContextId]};
    using SinkerIter = DLTV2::DltSinkersMap::iterator;
    for (SinkerIter it = naiCtxConfig->sinkersMap.begin(); it != naiCtxConfig->sinkersMap.end(); ++it) {
        const std::shared_ptr< DLTV2::DltSinkersConfig > config = it->second;
        if (config->defaultLogThreshold >= naiLogLvl) {
            naiLogLvl = config->defaultLogThreshold;
        }
    }

    // Create naiLogger
    Logger *naiLogger = &(GetLoggerByCid(logNaiContextId, naiCtxDesc, static_cast< LogLevel >(naiLogLvl)));

    naiLogWrapper_->SetNaiCallbackLogger(naiLogger);

    return naiLogWrapper_->InitNaiLog(logConfig_->loggingBehavior, static_cast< LogLevel >(naiLogLvl));
}

/// @brief
bool LogManager::Destroy() noexcept
{
    dEIinted_ = true;

    LOGVERBOSE("enter :") << __func__;
    if (isInit_ == true) {
        isInit_ = false;
#ifdef ARA_LOG_HAS_REMOTE
        if (RemoteSinker::Instance() != nullptr) {
            RemoteSinker::Instance()->StopSending();
        }
#endif
        if (naiLogWrapper_ != nullptr) {
            std::ignore = naiLogWrapper_->DeinitNaiLog();
        }
#ifdef ARA_LOG_HAS_REMOTE
        if (this->mlogCommandExcutor_) {
            this->mlogCommandExcutor_->LogCommandDestroy();
            this->mlogCommandExcutor_ = nullptr;
        }
        if (logConfig_->useDaemon) {
            RemoteSinker::DestroyInstance();
        }
#endif
        std::ignore = FlushBufferToFile();

        fileSinkerMap_.clear();

        loggers_.clear();
        if (determModule_ != nullptr) {
            determModule_->Reset();
        }
    }
    if (fileStatChecker_ != nullptr) {
        fileStatChecker_ = nullptr;
    }

    return true;
}
ClientState LogManager::RemoteClientState()
{
    if (logConfig_ != nullptr) {
        return logConfig_->clientstate;
    }
    return ClientState::kUnknown;
}
/// @brief
/// @return
bool LogManager::_createLoggerFromConfigFile() noexcept
{
    LOGVERBOSE("enter :") << __func__;

    for (std::pair< std::string, DLTV2::DltContextConfigPtr > contextPair : logConfig_->dltV2Config) {
        DLTV2::DltContextConfigPtr contextptr = contextPair.second;
        LOGERROR("create :") << contextptr->contextId;
        LoggerDataPtr logScopedDataTmp{std::make_shared< LoggerScopeData >()};
        logScopedDataTmp->staticCtx->identity.appId              = contextptr->applicationId;
        logScopedDataTmp->config.sinkerLevelMap                  = contextptr->sinkerLevelMap;
        logScopedDataTmp->staticCtx->identity.contextId          = contextptr->contextId;
        logScopedDataTmp->staticCtx->identity.contextDescription = contextptr->contextDescription;
        logScopedDataTmp->staticCtx->identity.sessionId          = contextptr->sessionId;
        if (logConfig_->sessionIdSupport) {
            if (logScopedDataTmp->staticCtx->identity.sessionId == 0) {
                logScopedDataTmp->staticCtx->identity.sessionId = logConfig_->sessionId;
            }
        }

        logScopedDataTmp->config.loggingBehavior             = logConfig_->loggingBehavior;
        logScopedDataTmp->staticCtx->identity.ecuId          = logConfig_->ecuId;
        logScopedDataTmp->staticCtx->identity.sessionSupport = logConfig_->sessionIdSupport;
        logScopedDataTmp->staticCtx->identity.appDescription = logConfig_->appdesc;
        logScopedDataTmp->runtime.timeBase                   = logConfig_->timebaseName;

        logScopedDataTmp->runtime.timeSync = std::make_shared< LogTimeStamp >(logScopedDataTmp->runtime.timeBase);
        bool consoledefaultTrace{true};
        bool fileDefaultTrace{true};
        bool networkDefaultTrace{true};
        for (std::pair< std::string, std::shared_ptr< DLTV2::DltSinkersConfig > > sinkersPair :
             contextptr->sinkersMap) {
            std::string sinkertype                                   = sinkersPair.first;
            std::shared_ptr< DLTV2::DltSinkersConfig > sinkersConfig = sinkersPair.second;
            LOGVERBOSE(__func__) << "sinkertype:" << sinkertype
                                 << " sinkersdefaultLogThreshold:" << sinkersConfig->defaultLogThreshold;

            if (sinkersConfig->sinkerType == kDLT_LOGSINK_FILE) {
                logScopedDataTmp->config.plainText = sinkersConfig->plainText;
                logScopedDataTmp->config.toFile    = true;

                /// One log file corresponds to one fileSinker; multiple channels writing to the same file use the same fileSinker object pointer, mainly because file cache needs to be consistent
                if (fileSinkerMap_.end() != fileSinkerMap_.find(sinkersConfig->logFileName)) {
                    logScopedDataTmp->sinkers.fileSinker = fileSinkerMap_[sinkersConfig->logFileName];
                } else {
#ifndef __android__
                    if (sinkersConfig->fileShared) {
                        std::shared_ptr< FileSinkerWithLocker > filesinker{std::make_shared< FileSinkerWithLocker >()};
                        std::ignore = filesinker->Init(logConfig_->ecuId, sinkersConfig->logFileName,
                                                       sinkersConfig->singleFileSize, sinkersConfig->fileCount,
                                                       sinkersConfig->fileBufferSize);
                        fileSinkerMap_[sinkersConfig->logFileName] = filesinker;
                        filesinker->SetPlainText(sinkersConfig->plainText);
                        filesinker->SetCompress(sinkersConfig->enableCompress);

                        logScopedDataTmp->sinkers.fileSinker = filesinker;
                    } else
#endif
                    {
                        std::shared_ptr< FileSinkerNoLocker > filesinker{std::make_shared< FileSinkerNoLocker >()};
                        std::ignore = filesinker->Init(logConfig_->ecuId, sinkersConfig->logFileName,
                                                       sinkersConfig->singleFileSize, sinkersConfig->fileCount,
                                                       sinkersConfig->fileBufferSize);
                        fileSinkerMap_[sinkersConfig->logFileName] = filesinker;
                        filesinker->SetPlainText(sinkersConfig->plainText);
                        filesinker->SetCompress(sinkersConfig->enableCompress);

                        logScopedDataTmp->sinkers.fileSinker = filesinker;
                    }
                }
                fileDefaultTrace                        = sinkersConfig->defaultTraceState;
                logScopedDataTmp->config.nonVerboseMode = sinkersConfig->nonVerboseMode;
            }
            if (sinkersConfig->sinkerType == kDLT_LOGSINK_REMOTE) {
                logScopedDataTmp->config.toRemote = true;

                networkDefaultTrace = sinkersConfig->defaultTraceState;
            }
            if (sinkersConfig->sinkerType == kDLT_LOGSINK_CONSOLE) {
                logScopedDataTmp->config.toConsole      = true;
                logScopedDataTmp->config.nonVerboseMode = sinkersConfig->nonVerboseMode;
                consoledefaultTrace                     = sinkersConfig->defaultTraceState;
            }
        }
        /// NOTE: The default value is true; it is true even if not configured; it only takes effect when it is explicitly set to false.
        if (consoledefaultTrace == false && fileDefaultTrace == false && networkDefaultTrace == false) {
            logScopedDataTmp->config.defaultTraceState = false;
        }

        // Sync nonVerboseMode to static context and build shared encoding context
        logScopedDataTmp->staticCtx->nonVerboseMode = logScopedDataTmp->config.nonVerboseMode;
        logScopedDataTmp->staticCtx->RebuildEncodeContext();

        std::shared_ptr< ara::log::Logger > tmpLoggerInstance{new Logger{logScopedDataTmp}};
        if (true == contextptr->determConfiged) {
            if (determModule_ == nullptr) {
                determModule_ = std::make_shared< LogManagerDetermModule >();
            }
            std::ignore = determModule_->BindConfiguredDetermLogger(logConfig_, contextptr->contextId, logScopedDataTmp,
                                                                    tmpLoggerInstance);

        } else {
            std::ignore = loggers_.insert({contextptr->contextId, tmpLoggerInstance});
        }
    }
    return true;
}  // namespace internal
/// @brief Determine whether it ends with dlt
/// @param str
/// @return
bool LogManager::_hasValidSuffix(std::string const &str) noexcept
{
    std::size_t const dotPos{str.find_last_of('.')};
    // Check if a dot was found, and that the dot is not the last character and not the first character
    bool const hasSuffix{(dotPos != std::string::npos) && (dotPos < str.size() - 1U) && (dotPos > 0U)};
    return hasSuffix;
}

std::string LogManager::_getDir(std::string const &str) noexcept
{
    std::size_t const found{str.find_last_of('/')};
    std::string tmpfilename{};
    if (std::string::npos != found) {
        tmpfilename = str.substr(0U, found);
    }
    return tmpfilename;
}

/// @brief Lock is required
/// @param[in]  contextid
/// @param[in]  logLevel
void LogManager::OnLogLevelChanged(std::string const &contextid, std::uint8_t const &logLevel) noexcept
{
    LOGVERBOSE("enter :") << __func__;
    bool const haveCID{loggers_.count(contextid) > 0U};
    if ((contextid.empty() == false) && haveCID) {
        LOGVERBOSE("logger: ") << contextid;
        loggers_[contextid]->SetThreshold(static_cast< LogLevel >(logLevel));
    } else {
        for (std::pair< std::string, std::shared_ptr< ara::log::Logger > > tmpC : loggers_) {
            tmpC.second->SetThreshold(static_cast< LogLevel >(logLevel));
        }
    }
}

/// @brief
/// @param[in]  status
void LogManager::OnRegistStatusChanged(std::int32_t const &status) noexcept
{
#ifdef ARA_LOG_HAS_REMOTE
    LOGVERBOSE(__func__) << logConfig_->appid << "status";
    if (RemoteSinker::Instance() != nullptr) {
        RemoteSinker::Instance()->SetRegStatus(status);
    }
#else
    std::ignore = status;
#endif
}

void LogManager::OnClientstateChanged(ara::log::ClientState state) noexcept
{
    if (clientStateCallback_ != nullptr) {
        clientStateCallback_(state);
    }
}
/// @brief
/// @param[in]  cid
/// @param[in]  des
/// @param[in]  level
/// @return
Logger &LogManager::GetLoggerByCid(std::string const &cid,
                                   std::string const &des,
                                   ara::log::LogLevel const &level) noexcept
{
    LOGERROR(__func__) << " , appid : " << logConfig_->appid << "  , ctxid  " << cid;

    if (logConfig_->dltV2Config.end() != logConfig_->dltV2Config.find(cid)) {
        if (true == logConfig_->dltV2Config[cid]->determConfiged) {
            return GetLoggerByCid(cid.data(), des, 0, level);
        }
    }

    std::unique_lock< std::mutex > const loggerguard{loggerLock_};
    TLoggerMap::iterator const iter{loggers_.find(cid)};
    if (iter != loggers_.end()) {
        return *(iter->second);
    }
    /// NOTE: If not configured, can only print to console
    LOGERROR(__func__) << "from api create ,only to console : " << cid
                       << " level:" << std::to_string(static_cast< std::int32_t >(level));

    std::shared_ptr< LoggerScopeData > logScopedDataTmp{std::make_shared< LoggerScopeData >()};
    _initializeLoggerScopeData(logScopedDataTmp, cid, des, level);

    logScopedDataTmp->config.toConsole = true;

    std::shared_ptr< ara::log::Logger > tmpLogger{new Logger{logScopedDataTmp}};
    std::ignore = loggers_.insert({cid, tmpLogger});

    _notifyLogChannelUpdate(cid, level);
    LOGVERBOSE(__func__) << cid.data() << " create end";
    return *tmpLogger;
}

/// @brief Dynamically creating a logger does not guarantee determinism; the purpose of determinism is
/// @param[in]  cid
/// @param[in]  des
/// @param[in]  domainId The domain ID for determined logs, default 0. If there is a corresponding domainId in the configuration file, use that; otherwise use the default value 0
/// @param[in]  level
/// @return
Logger &LogManager::GetLoggerByCid(std::string const &cid,
                                   std::string const &des,
                                   std::uint8_t const domainId,
                                   ara::log::LogLevel const &level) noexcept
{
    std::unique_lock< std::mutex > const loggerguard{loggerLock_};
    if (determModule_ == nullptr) {
        determModule_ = std::make_shared< LogManagerDetermModule >();
    }

    return determModule_->GetLoggerByCidDeterm(
        logConfig_, cid, des, domainId, level,
        [this](std::shared_ptr< DLTV2::DltSinkersConfig > const &fileConfig, LoggerDataPtr &logScopedData) {
            _createFileSinker(fileConfig, logScopedData);
        },
        [this](std::string const &localCid, ara::log::LogLevel const &localLevel) {
            _notifyLogChannelUpdate(localCid, localLevel);
        });
}
/// @brief When file caching is enabled, flush the cache to the file.
/// @return
OperState LogManager::FlushBufferToFile() noexcept
{
    for (std::pair< std::string const, std::shared_ptr< ara::log::internal::IlogSinker > > &mtmp : fileSinkerMap_) {
        std::ignore = mtmp.second->FlushBuffer();
    }
    return OperState::kOk;
}

bool LogManager::PipeBroken() noexcept
{
    if (logConfig_ != nullptr) {
        LOGVERBOSE(__func__) << logConfig_->appid << " logmananger";
    }
#ifdef ARA_LOG_HAS_REMOTE
    if (RemoteSinker::Instance() != nullptr) {
        RemoteSinker::Instance()->StopSending();
    }
#endif
    return false;
}

/// @brief Memory usage size for deterministic logs
/// @return Memory usage size for deterministic logs
/// @brief Create a file sinker
/// @param fileConfig File configuration
/// @param logScopedData Log scope data
void LogManager::_createFileSinker(std::shared_ptr< DLTV2::DltSinkersConfig > const &fileConfig,
                                   LoggerDataPtr &logScopedData) noexcept
{
    if (loggerModule_ == nullptr) {
        loggerModule_ = std::make_shared< LogManagerLoggerModule >();
    }
    loggerModule_->CreateFileSinker(logConfig_, fileSinkerMap_, fileConfig, logScopedData);
}

/// @brief Initialize common attributes of LoggerScopeData
/// @param logScopedData Log scope data
/// @param cid Context ID
/// @param des Description
/// @param level Log level
void LogManager::_initializeLoggerScopeData(LoggerDataPtr &logScopedData,
                                            std::string const &cid,
                                            std::string const &des,
                                            ara::log::LogLevel const &level) noexcept
{
    if (loggerModule_ == nullptr) {
        loggerModule_ = std::make_shared< LogManagerLoggerModule >();
    }
    loggerModule_->InitializeLoggerScopeData(logConfig_, logScopedData, cid, des, level);
}

/// @brief Notify log channel update
/// @param cid Context ID
/// @param level Log level
void LogManager::_notifyLogChannelUpdate(std::string const &cid, ara::log::LogLevel const &level) noexcept
{
#ifdef ARA_LOG_HAS_REMOTE
    bool const checkCmdPTR{mlogCommandExcutor_ != nullptr};
    if (logConfig_->useDaemon && checkCmdPTR) {
        mlogCommandExcutor_->UpdateLogChannel(logConfig_->appid, cid, static_cast< std::uint8_t >(level));
    }
#endif
}

}  // namespace internal
}  // namespace log
}  // namespace ara
