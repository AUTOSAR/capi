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
/// @file       logger_class.h
/// @brief
/// @details
/// @date       2025-09-03
/// @author     yangjinbiao
/// @version    1.2.0
///
/// ================================================================

#ifndef __ara_log_class__
#define __ara_log_class__

#include <cstdint>
#include <tuple>
#include <utility>

#include "common.h"
#include "dlt_logger.h"
#include "log_stream.h"

namespace ara {
namespace log {

/**
 * @ingroup logger
 * @class Logger
 *
 * @brief this is an decorator for the ara::log::dltlogger
 */
class Logger
{
public:
    LogStream LogFatal() const noexcept;
    LogStream LogError() const noexcept;
    LogStream LogWarn() const noexcept;
    LogStream LogInfo() const noexcept;
    LogStream LogDebug() const noexcept;
    LogStream LogVerbose() const noexcept;
    LogStream WithLevel(LogLevel logLevel) const noexcept;
    void SetThreshold(LogLevel const& loglevel) noexcept;
    bool IsEnabled(LogLevel logLevel) const noexcept;
    ////#######################Extended interfaces###################
    std::int32_t ExtSetOuterTimeFunc(internal::ExtSetOuterTimeFunc const& func) noexcept;
    static Logger& GetDummyLogger() { return s_Dummylogger_; }
    template < typename... Attrs, typename MsgId, typename... Params >
    void LogWith(const std::tuple< Attrs... >& attrs, const MsgId& msgId, const Params&... args) noexcept
    {
        mImpl_.LogWith(attrs, msgId, args...);
    }

    /**
     * @brief definition out of R20-11
     */
    template < typename MsgId, typename... Params >
    void Log(const MsgId& messageId, const Params&... args) noexcept
    {
        TraceSwitch(mImpl_, messageId, args...);
    }
    explicit Logger(internal::LoggerDataPtr const& logScopedData) : mImpl_(logScopedData) {}

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    Logger(Logger&&)                 = delete;
    Logger& operator=(Logger&&) = delete;

    Logger() noexcept : mImpl_(nullptr) {}

    ~Logger() = default;

private:
    ara::log::DltLogger mImpl_;

    static Logger s_Dummylogger_;  // Static dummy instance inside the class
};

} /* namespace log */
} /* namespace ara */

#endif