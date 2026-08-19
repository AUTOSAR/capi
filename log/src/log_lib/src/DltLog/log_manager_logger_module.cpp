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
/// @file       log_manager_logger_module.cpp
/// @brief
/// @details
/// @date       2025-09-03
/// @author     yangjinbiao
/// @version    1.2.0
///
/// ================================================================

#include "DltLog/log_manager_logger_module.h"

#include "DltLog/time_stamp.h"
#include "DltSinkers/file_sinker_no_locker.h"
#include "DltSinkers/file_sinker_withLocker.h"
#ifdef ARA_LOG_HAS_REMOTE
    #include "DltSinkers/remote_sinker.h"
#endif

namespace ara {
namespace log {
namespace internal {

void LogManagerLoggerModule::CreateFileSinker(std::shared_ptr< ApplicationConfig > const &logConfig,
                                              std::map< std::string, std::shared_ptr< IlogSinker > > &fileSinkerMap,
                                              std::shared_ptr< DLTV2::DltSinkersConfig > const &fileConfig,
                                              LoggerDataPtr &logScopedData) const noexcept
{
    if (fileSinkerMap.end() != fileSinkerMap.find(fileConfig->logFileName)) {
        logScopedData->sinkers.fileSinker = fileSinkerMap[fileConfig->logFileName];
    } else {
#ifndef __android__
        if (fileConfig->fileShared) {
            std::shared_ptr< FileSinkerWithLocker > filesinker{std::make_shared< FileSinkerWithLocker >()};
            std::ignore = filesinker->Init(logConfig->ecuId, fileConfig->logFileName, fileConfig->singleFileSize,
                                           fileConfig->fileCount, fileConfig->fileBufferSize);
            fileSinkerMap[fileConfig->logFileName] = filesinker;
            filesinker->SetCompress(fileConfig->enableCompress);

            filesinker->SetPlainText(fileConfig->plainText);
            logScopedData->sinkers.fileSinker = filesinker;
        } else
#endif

        {
            std::shared_ptr< FileSinkerNoLocker > filesinker{std::make_shared< FileSinkerNoLocker >()};
            std::ignore = filesinker->Init(logConfig->ecuId, fileConfig->logFileName, fileConfig->singleFileSize,
                                           fileConfig->fileCount, fileConfig->fileBufferSize);
            filesinker->SetPlainText(fileConfig->plainText);
            filesinker->SetCompress(fileConfig->enableCompress);
            fileSinkerMap[fileConfig->logFileName] = filesinker;
            logScopedData->sinkers.fileSinker      = filesinker;
        }
    }
}

void LogManagerLoggerModule::InitializeLoggerScopeData(std::shared_ptr< ApplicationConfig > const &logConfig,
                                                       LoggerDataPtr &logScopedData,
                                                       std::string const &cid,
                                                       std::string const &des,
                                                       ara::log::LogLevel const &level) const noexcept
{
    logScopedData->staticCtx->identity.appId              = logConfig->appid;
    logScopedData->staticCtx->identity.contextId          = cid;
    logScopedData->staticCtx->identity.contextDescription = des;
    logScopedData->config.loggingBehavior                 = logConfig->loggingBehavior;
    logScopedData->staticCtx->identity.ecuId              = logConfig->ecuId;
    logScopedData->staticCtx->identity.appDescription     = logConfig->appdesc;
    logScopedData->staticCtx->identity.sessionSupport     = logConfig->sessionIdSupport;
    logScopedData->staticCtx->identity.sessionId          = 0;
    if (logConfig->sessionIdSupport) {
        logScopedData->staticCtx->identity.sessionId = logConfig->sessionId;
    }
    logScopedData->runtime.timeBase = logConfig->timebaseName;
    logScopedData->runtime.timeSync = std::make_shared< LogTimeStamp >(logScopedData->runtime.timeBase);
    logScopedData->config.sinkerLevelMap[kDLT_LOGSINK_CONSOLE] = static_cast< std::uint8_t >(level);

    // Sync nonVerboseMode to static context and build shared encoding context
    logScopedData->staticCtx->nonVerboseMode = logScopedData->config.nonVerboseMode;
    logScopedData->staticCtx->RebuildEncodeContext();
}

}  // namespace internal
}  // namespace log
}  // namespace ara