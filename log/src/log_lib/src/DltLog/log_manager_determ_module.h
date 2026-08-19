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
/// @file       log_manager_determ_module.h
/// @brief      LogManager deterministic execution related encapsulation module
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
/// @unit_description=Encapsulation of deterministic execution related logic in LogManager
/// @endcode
///
/// ================================================================

#ifndef __LOG_MANAGER_DETERM_MODULE_H__
#define __LOG_MANAGER_DETERM_MODULE_H__

#include <cstdint>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "DltLogDefines/application_config.h"
#include "DltLogDefines/dlt_sinkers_config.h"
#include "DltLogDefines/logger_scope_data.h"

namespace ara {
namespace log {

class Logger;

namespace internal {

class ThreadPool;
class GlobalMemoryPool;

class LogManagerDetermModule final
{
public:
    /// @brief DetermLoggerVector Container for deterministic execution loggers
    using DetermLoggerVector = std::vector< std::shared_ptr< ara::log::Logger > >;
    /// @brief DetermLoggerMap Mapping for deterministic execution loggers
    using DetermLoggerMap = std::map< std::string, std::shared_ptr< DetermLoggerVector > >;

    /// @brief Create file sinker callback
    using CreateFileSinkerCb
        = std::function< void(std::shared_ptr< DLTV2::DltSinkersConfig > const &, LoggerDataPtr &) >;
    /// @brief Configure sinker callback
    using ConfigureSinkerCb = std::function< void(std::string const &, LoggerDataPtr &) >;
    /// @brief Notify log channel update callback
    using NotifyLogChannelUpdateCb = std::function< void(std::string const &, ara::log::LogLevel const &) >;

    /// @brief Create deterministic execution memory pool
    /// @param[in]  logConfig Log configuration
    /// @param[in]  defaultDetermFreeCount Default number of free blocks for deterministic execution
    /// @return Execution result
    bool CreateDetermMemoryPool(std::shared_ptr< ApplicationConfig > const &logConfig,
                                std::uint32_t defaultDetermFreeCount = 1024U) noexcept;

    /// @brief Create deterministic execution thread pool
    /// @param[in]  logConfig Log configuration
    /// @return Execution result
    bool CreateDetermThreads(std::shared_ptr< ApplicationConfig > const &logConfig) noexcept;

    /// @brief Bind the deterministic logger created by the configuration file (domain 0)
    /// @param[in]  logConfig Log configuration
    /// @param[in]  cid Context ID
    /// @param[in]  logScopedData Log scope data
    /// @param[in]  logger Logger instance
    /// @return Execution result
    bool BindConfiguredDetermLogger(std::shared_ptr< ApplicationConfig > const &logConfig,
                                    std::string const &cid,
                                    LoggerDataPtr &logScopedData,
                                    std::shared_ptr< ara::log::Logger > const &logger) noexcept;

    /// @brief Get or create a deterministic logger
    /// @param[in]  logConfig Log configuration
    /// @param[in]  cid Context ID
    /// @param[in]  des Context description
    /// @param[in]  domainId Deterministic execution domain ID
    /// @param[in]  level Log level
    /// @param[in]  initLoggerScopeData Callback to initialize LoggerScopeData
    /// @param[in]  createFileSinker Callback to create file sinker

    /// @param[in]  notifyLogChannelUpdate Callback to notify channel update
    /// @return Logger reference
    ara::log::Logger &GetLoggerByCidDeterm(std::shared_ptr< ApplicationConfig > const &logConfig,
                                           std::string const &cid,
                                           std::string const &des,
                                           std::uint8_t domainId,
                                           ara::log::LogLevel const &level,
                                           CreateFileSinkerCb const &createFileSinker,
                                           NotifyLogChannelUpdateCb const &notifyLogChannelUpdate) noexcept;

    /// @brief Reset the internal state of the deterministic execution module
    void Reset() noexcept;

private:
    std::size_t GetDetermMetaMemoryLength() const noexcept;

private:
    DetermLoggerMap determLoggers_{};
    std::shared_ptr< ThreadPool > determThreadPool_{nullptr};
    std::shared_ptr< GlobalMemoryPool > globalPool_{nullptr};
};

}  // namespace internal
}  // namespace log
}  // namespace ara

#endif