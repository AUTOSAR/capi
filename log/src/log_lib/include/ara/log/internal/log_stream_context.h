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
/// @file       log_stream_context.h
/// @brief      Lightweight input context for LogStream, constructed once by Logger when creating LogStream
/// @details
/// @date       2025-09-03
/// @author     yangjinbiao
/// @version    1.2.0
///
/// ================================================================

#ifndef __LOG_INTERNAL_LOG_STREAM_CONTEXT__
#define __LOG_INTERNAL_LOG_STREAM_CONTEXT__

#include <atomic>
#include <memory>
#include <string>

#include "DltProtocol/dlt_protocol_encoder.h"  // EncodeContext
#include "ara/log/common.h"                    // LogLevel, DltTimeStamp

namespace ara {
namespace log {
namespace internal {

class IlogSinker;
class LogTimeStamp;
class DeterminedExecutor;  ///< Deterministic executor, full definition in DltLogDerm/determined_executor.h

/// @brief Lightweight input context for LogStream
/// @note LogStream only depends on this structure, no longer directly traverses sub-structures of LoggerScopeData.
///       Built by Logger (in dlt_logger.cpp) from logScopedData_ once each time LogStream is created.
struct LogStreamContext
{
    // ===== Encoding base (static, produced by LoggerStaticContext::RebuildEncodeContext(), shared ownership) =====
    /// @brief Encoding context, used by DltProtocolEncoder/DltPlainEncoder; shared read-only with the Logger instance
    std::shared_ptr< const EncodeContext > encodeCtx{nullptr};

    // ===== Output routing (level filtering already fused, computed once by Logger at creation) =====
    bool consoleEnabled{false};  ///< toConsole && level <= consoleThreshold
    bool remoteEnabled{false};   ///< toRemote  && level <= remoteThreshold

    bool fileEnabled{false};  ///< toFile    && level <= fileThreshold
    bool plainText{false};    ///< File output using plain text format

    // ===== Deterministic path =====
    /// @brief Whether deterministic execution is enabled
    bool isDetermined{false};
    // ===== Sinkers =====
    /// @brief File sinker (may be null)
    std::shared_ptr< IlogSinker > fileSinker{nullptr};

    // ===== Deterministic execution handle =====
    /// @brief Pre-cached deterministic executor, LogStream uses it directly without constructing a temporary object
    std::shared_ptr< DeterminedExecutor > executor{nullptr};

    // ===== Single message runtime state (written when LogStream is constructed) =====
    DltTimeStamp dltTimeStamp{};         ///< Timestamp of this message
    LogLevel logLevel{LogLevel::kInfo};  ///< Log level of this message
    std::uint32_t msgCount{0U};          ///< Sequence number of this message
};

using LogStreamContextPtr = std::shared_ptr< LogStreamContext >;

}  // namespace internal
}  // namespace log
}  // namespace ara

#endif  // __LOG_INTERNAL_LOG_STREAM_CONTEXT__
