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
/// @file       log_manager_determ_module.cpp
/// @brief      Implementation of the deterministic execution related encapsulation module of LogManager
/// @details
/// @date       2026-03-10
/// @author     yangjinbiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/LOG/DltLog
/// @interface_level = unit
/// @unit_name = log_manager_determ_module
/// @unit_description=Encapsulation implementation of deterministic execution related logic in LogManager
/// @endcode
///
/// ================================================================

#include "DltLog/log_manager_determ_module.h"

#include <memory>

#include "DltLog/time_stamp.h"
#include "DltLogDerm/determ_log_meta_data.h"
#include "DltLogDerm/determined_executor.h"
#include "DltLogDerm/memory_pool.h"
#include "DltLogDerm/thread_pool.h"
#include "DltProtocol/dlt_protocol_model_encoder.h"
#include "DltProtocol/dlt_protocol_nomodel_encoder.h"
#include "Utils/src/private_log.h"
#include "common.h"
#include "logger_class.h"

namespace ara {
namespace log {
namespace internal {

bool LogManagerDetermModule::CreateDetermMemoryPool(std::shared_ptr< ApplicationConfig > const &logConfig,
                                                    std::uint32_t defaultDetermFreeCount) noexcept
{
    LOGVERBOSE(__func__) << "enter";
    if (logConfig == nullptr) {
        return false;
    }
    if (logConfig->determFreeCount == 0U) {
        logConfig->determFreeCount = defaultDetermFreeCount;
    }
    if (0U < logConfig->determFreeCount) {
        globalPool_ = std::make_shared< GlobalMemoryPool >(GetDetermMetaMemoryLength(), logConfig->determFreeCount);
        LOGVERBOSE(__func__) << ": startConsoleThreadIfFileExist " << logConfig->startConsoleThreadIfFileExist
                             << ", determFreeCount=" << logConfig->determFreeCount;
    }
    LOGVERBOSE(__func__) << "leave";
    return true;
}

bool LogManagerDetermModule::CreateDetermThreads(std::shared_ptr< ApplicationConfig > const &logConfig) noexcept
{
    LOGVERBOSE(__func__) << "enter";
    if ((logConfig != nullptr) && (0U < logConfig->determFreeCount) && (globalPool_ != nullptr)) {
        determThreadPool_ = std::make_shared< ThreadPool >();
        determThreadPool_->CreateBasicThread(logConfig->determFreeCount * 10U, globalPool_);
        if (true == logConfig->startConsoleThreadIfFileExist) {
            determThreadPool_->CreateConsoleThread(logConfig->determFreeCount * 10U);
        }
    }
    LOGVERBOSE(__func__) << "leave";
    return true;
}

bool LogManagerDetermModule::BindConfiguredDetermLogger(std::shared_ptr< ApplicationConfig > const &logConfig,
                                                        std::string const &cid,
                                                        LoggerDataPtr &logScopedData,
                                                        std::shared_ptr< ara::log::Logger > const &logger) noexcept
{
    if ((logConfig == nullptr) || (logScopedData == nullptr) || (logger == nullptr)) {
        return false;
    }

    if (globalPool_ == nullptr) {
        std::ignore = CreateDetermMemoryPool(logConfig);
    }
    if (determThreadPool_ == nullptr) {
        std::ignore = CreateDetermThreads(logConfig);
    }
    if ((globalPool_ == nullptr) || (determThreadPool_ == nullptr)) {
        return false;
    }

    std::shared_ptr< DetermLoggerVector > determVec = std::make_shared< DetermLoggerVector >();
    logScopedData->determined.domainIndex           = 0U;
    logScopedData->determined.isDetermined          = true;
    auto localPool                                  = std::make_shared< LocalPool >(*globalPool_);
    logScopedData->determined.executor
        = std::make_shared< DeterminedExecutor >(localPool, determThreadPool_.get(), logScopedData->config.toFile,
                                                 logScopedData->config.toRemote, &logScopedData->sinkers.fileSinker);
    determVec->emplace_back(logger);
    determLoggers_[cid] = determVec;
    return true;
}

ara::log::Logger &LogManagerDetermModule::GetLoggerByCidDeterm(
    std::shared_ptr< ApplicationConfig > const &logConfig,
    std::string const &cid,
    std::string const &des,
    std::uint8_t domainId,
    ara::log::LogLevel const &level,
    CreateFileSinkerCb const &createFileSinker,
    NotifyLogChannelUpdateCb const &notifyLogChannelUpdate) noexcept
{
    DetermLoggerMap::iterator const localIter{determLoggers_.find(cid)};
    if (localIter != determLoggers_.end()) {
        if (domainId < localIter->second->size()) {
            return *((*(localIter->second))[domainId]);
        }
    }

    LOGERROR(__func__) << " only to console create determ new : " << cid << ", domainId=" << domainId;

    LoggerDataPtr logScopedDataTmp{std::make_shared< LoggerScopeData >()};

    // Directly use logConfig to fill the static context, consistent with LogManagerLoggerModule::InitializeLoggerScopeData
    if (logConfig != nullptr) {
        logScopedDataTmp->staticCtx->identity.appId              = logConfig->appid;
        logScopedDataTmp->staticCtx->identity.contextId          = cid;
        logScopedDataTmp->staticCtx->identity.contextDescription = des;
        logScopedDataTmp->config.loggingBehavior                 = logConfig->loggingBehavior;
        logScopedDataTmp->staticCtx->identity.ecuId              = logConfig->ecuId;
        logScopedDataTmp->staticCtx->identity.appDescription     = logConfig->appdesc;
        logScopedDataTmp->staticCtx->identity.sessionSupport     = logConfig->sessionIdSupport;
        logScopedDataTmp->staticCtx->identity.sessionId          = 0U;
        if (logConfig->sessionIdSupport) {
            logScopedDataTmp->staticCtx->identity.sessionId = logConfig->sessionId;
        }
        logScopedDataTmp->runtime.timeBase = logConfig->timebaseName;
        logScopedDataTmp->runtime.timeSync = std::make_shared< LogTimeStamp >(logScopedDataTmp->runtime.timeBase);
    }
    logScopedDataTmp->config.sinkerLevelMap[kDLT_LOGSINK_CONSOLE] = static_cast< std::uint8_t >(level);
    logScopedDataTmp->staticCtx->nonVerboseMode                   = logScopedDataTmp->config.nonVerboseMode;
    logScopedDataTmp->staticCtx->RebuildEncodeContext();
    if ((logConfig != nullptr) && (logConfig->dltV2Config.count(cid) > 0U)
        && (true == logConfig->dltV2Config[cid]->determConfiged)) {
        LOGVERBOSE(__func__) << "determ LoggerScopeData : " << cid;

        logScopedDataTmp->config.sinkerLevelMap = logConfig->dltV2Config[cid]->sinkerLevelMap;

        if (logConfig->dltV2Config[cid]->sinkersMap.count(kDLT_LOGSINK_FILE) > 0U) {
            logScopedDataTmp->config.toFile = true;
            std::shared_ptr< DLTV2::DltSinkersConfig > tmpFileConfig
                = logConfig->dltV2Config[cid]->sinkersMap[kDLT_LOGSINK_FILE];
            createFileSinker(tmpFileConfig, logScopedDataTmp);
        }

        if (logConfig->dltV2Config[cid]->sinkersMap.count(kDLT_LOGSINK_CONSOLE) > 0U) {
            logScopedDataTmp->config.toConsole = true;
        }

        if (logConfig->dltV2Config[cid]->sinkersMap.count(kDLT_LOGSINK_REMOTE) > 0U) {
            logScopedDataTmp->config.toRemote = true;
            if (logScopedDataTmp->config.toConsole && logScopedDataTmp->config.toFile) {
                logConfig->startConsoleThreadIfFileExist = true;
            }
        }
    }

    std::shared_ptr< ara::log::Logger > tmpLogger{new Logger{logScopedDataTmp}};
    std::shared_ptr< DetermLoggerVector > determVec = nullptr;

    if (0 < determLoggers_.count(cid)) {
        determVec = determLoggers_[cid];
    } else {
        determVec = std::make_shared< DetermLoggerVector >();
        if (nullptr == globalPool_) {
            CreateDetermMemoryPool(logConfig);
        }
        if (nullptr == determThreadPool_) {
            CreateDetermThreads(logConfig);
        }
    }

    logScopedDataTmp->determined.domainIndex  = domainId;
    logScopedDataTmp->determined.isDetermined = true;
    auto localPool                            = std::make_shared< LocalPool >(*globalPool_);
    logScopedDataTmp->determined.executor     = std::make_shared< DeterminedExecutor >(
        localPool, determThreadPool_.get(), logScopedDataTmp->config.toFile, logScopedDataTmp->config.toRemote,
        &logScopedDataTmp->sinkers.fileSinker);
    determVec->emplace_back(tmpLogger);
    determLoggers_[cid] = determVec;

    notifyLogChannelUpdate(cid, level);
    LOGVERBOSE(__func__) << cid.data() << " create determ end";
    return *tmpLogger;
}

void LogManagerDetermModule::Reset() noexcept
{
    if (determThreadPool_ != nullptr) {
        determThreadPool_->DestroyThreads();
        determThreadPool_ = nullptr;
    }
    determLoggers_.clear();
    globalPool_ = nullptr;
}

std::size_t LogManagerDetermModule::GetDetermMetaMemoryLength() const noexcept
{
    std::size_t modelEncoderlen{sizeof(DltProtocolModelEncoder)};
    std::size_t nonModelEncoderlen{sizeof(DltProtocolNonModelEncoder)};
    if (modelEncoderlen > nonModelEncoderlen) {
        return modelEncoderlen + sizeof(LogMetaData);
    }
    return nonModelEncoderlen + sizeof(LogMetaData);
}

}  // namespace internal
}  // namespace log
}  // namespace ara