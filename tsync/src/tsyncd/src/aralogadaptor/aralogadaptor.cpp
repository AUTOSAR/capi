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
/// @file       aralogadaptor.cpp
/// @brief      time synchronization internal log stream definition
/// @details
/// @date       2023-02-14
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================

#include "ara/tsync/internal/aralogadaptor/aralogadaptor.h"

#include <ara/log/logger.h>

#include <unordered_map>

#include "ara/tsync/internal/log/logger.h"
namespace ara {
namespace tsync {
namespace internal {
namespace tsaralog {

/// @brief print log via AraLog
/// @param logAttr - log attributes
/// @param logMesg - log content
/// @exception
void AraLogAdaptor::Log2AraLog(ara::tsync::internal::tslog::LogAttribute const &logAttr,
                               ara::core::StringView const &logMesg)
{
    static std::unordered_map< ara::core::String, ara::log::Logger * > s_LoggerMap;
    std::unordered_map< ara::core::String, ara::log::Logger * >::const_iterator ctxIter{};
    ctxIter = std::move(s_LoggerMap.find({logAttr.GetLogContextId().c_str()}));
    if (ctxIter == s_LoggerMap.cend()) {
        ara::log::Logger &araLogger{ara::log::CreateLogger(
            std::move(ara::core::StringView(logAttr.GetLogContextId().c_str(), logAttr.GetLogContextId().length())),
            std::move(ara::core::StringView(logAttr.GetLogDescription().c_str(), logAttr.GetLogDescription().length())),
            ara::log::LogLevel::kVerbose)};
        std::ignore = s_LoggerMap.emplace(logAttr.GetLogContextId().c_str(), &araLogger);
    }

    switch (logAttr.GetLogLevel()) {
        case ara::tsync::internal::tslog::LogLevel::kFatal: {
            s_LoggerMap[logAttr.GetLogContextId().c_str()]->LogFatal() << logMesg;
            break;
        }
        case ara::tsync::internal::tslog::LogLevel::kError: {
            s_LoggerMap[logAttr.GetLogContextId().c_str()]->LogError() << logMesg;
            break;
        }
        case ara::tsync::internal::tslog::LogLevel::kWarn: {
            s_LoggerMap[logAttr.GetLogContextId().c_str()]->LogWarn() << logMesg;
            break;
        }
        case ara::tsync::internal::tslog::LogLevel::kInfo: {
            s_LoggerMap[logAttr.GetLogContextId().c_str()]->LogInfo() << logMesg;
            break;
        }
        case ara::tsync::internal::tslog::LogLevel::kDebug: {
            s_LoggerMap[logAttr.GetLogContextId().c_str()]->LogDebug() << logMesg;
            break;
        }
        case ara::tsync::internal::tslog::LogLevel::kVerbose: {
            s_LoggerMap[logAttr.GetLogContextId().c_str()]->LogVerbose() << logMesg;
            break;
        }
        default: {
            break;
        }
    }
}

}  // namespace tsaralog
}  // namespace internal
}  // namespace tsync
}  // namespace ara
