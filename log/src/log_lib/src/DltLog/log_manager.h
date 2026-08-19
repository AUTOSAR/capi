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
/// @file       log_manager.h
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

#ifndef __INTERNAL_LOG_LIB_WORKER__
#define __INTERNAL_LOG_LIB_WORKER__

#include <cstdint>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include "DltLog/nai_log_wrapper.h"
#include "DltLogDefines/application_config.h"
#include "DltLogDefines/dlt_sinkers_config.h"
#include "DltSinkers/log_sinker_interface.h"
#include "file_stat_checker.h"
#include "isoft/manifestreader/manifest_reader.h"
#include "logger_api.h"
#include "logger_class.h"

#ifdef ARA_LOG_HAS_REMOTE
    #include "DltProtocol/log_command_excutor.h"
#endif

namespace ara {
namespace log {
// /// @brief
// /// @param[in]  cid
// /// @param[in]  des
// /// @param[in]  level
// /// @code{.isoft}
// /// @interface_level=unit
// /// @trace_id_ad=AD_LOG_00134
// /// @trace_id_dd=DD_LOG_00188
// /// @needwork = ad
// /// @endcode
// Logger &CreateLogger(ara::core::StringView cid, ara::core::StringView des, ara::log::LogLevel level) noexcept;

namespace internal {
class LogManagerConfigModule;
class LogManagerLoggerModule;
class LogManagerDetermModule;
/// @brief Internal initialization of log
/// @return Execution result
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_LOG_00135
/// @trace_id_dd=DD_LOG_00189
/// @needwork = ad
/// @endcode
extern ara::core::Result< void > Initialize() noexcept;
/// @brief Internal deinitialization of log
/// @return Execution result
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_LOG_00136
/// @trace_id_dd=DD_LOG_00190
/// @needwork = ad
/// @endcode
extern ara::core::Result< void > Deinitialize() noexcept;

class FileSinkerWithLocker;
class LogCommandExcutor;
using ClientStateCallback = std::function< void(ClientState) >;

/// @brief LogManager connects the external lib interface and internal business implementation; performs certain external interface business implementations. Analyzes configuration files here.
/// Need to call it in the initialization function
/// Some externally called functions are implemented in this CPP file
/// Contains overall configuration information and distributes configuration downwards
/// The lifecycle of the worker instance is consistent with the life of the application
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_LOG_00137
/// @trace_id_dd=DD_LOG_00191
/// @needwork = ad
/// @endcode
class LogManager final
{
public:
    /// @brief TLoggerMap Log map type definition
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00319
    /// @trace_id_dd=DD_LOG_01715
    /// @needwork = ad
    /// @endcode
    using TLoggerMap = std::map< std::string, std::shared_ptr< Logger > >;

    /// @brief Constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00138
    /// @trace_id_dd=DD_LOG_00192
    /// @needwork = ad
    /// @endcode
    LogManager() = default;

    /// @brief Default copy constructor
    /// @param other
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00341
    /// @trace_id_dd=DD_LOG_01740
    /// @needwork = ad
    /// @endcode
    LogManager(LogManager const &other) = delete;
    /// @brief Default assignment
    /// @param other
    /// @return Reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00342
    /// @trace_id_dd=DD_LOG_01741
    /// @needwork = ad
    /// @endcode
    LogManager &operator=(LogManager const &other) = delete;
    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00139
    /// @trace_id_dd=DD_LOG_00193
    /// @needwork = ad
    /// @endcode
    ~LogManager() noexcept;

    /// @brief Move constructor
    /// @param[in] object LogManager&& object
    LogManager(LogManager &&object) noexcept = delete;
    /// @brief Move operation
    /// @param[in] object LogManager&& object
    /// @return LogManager&
    LogManager &operator=(LogManager &&object) noexcept = delete;

    /// @brief Initialize
    /// @return Execution result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00140
    /// @trace_id_dd=DD_LOG_00194
    /// @needwork = ad
    /// @endcode
    std::int32_t Init() noexcept;

    /// @brief Resource cleanup
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00141
    /// @trace_id_dd=DD_LOG_00195
    /// @needwork = ad
    /// @endcode
    bool Destroy() noexcept;
    /// @brief  Remote client status
    /// @return  Status
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00142
    /// @trace_id_dd=DD_LOG_00196
    /// @needwork = ad
    /// @endcode
    ClientState RemoteClientState();
    /// @brief Status change callback function
    /// @param[in]  contextid  Channel ID
    /// @param[in]  logLevel Log level
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00143
    /// @trace_id_dd=DD_LOG_00197
    /// @needwork = ad
    /// @endcode
    void OnLogLevelChanged(std::string const &contextid, std::uint8_t const &logLevel) noexcept;

    /// @brief Registration status change
    /// @param[in]  status Status
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00144
    /// @trace_id_dd=DD_LOG_00198
    /// @needwork = ad
    /// @endcode
    void OnRegistStatusChanged(std::int32_t const &status) noexcept;

    /// @brief Client status change callback function
    /// @param[in]  state State
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00146
    /// @trace_id_dd=DD_LOG_00201
    /// @needwork = ad
    /// @endcode
    void OnClientstateChanged(ara::log::ClientState state) noexcept;

    /// @brief  Get Logger by ID
    /// @param[in]  cid Channel ID
    /// @param[in]  des Channel description
    /// @param[in]  level Log level
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00145
    /// @trace_id_dd=DD_LOG_00199
    /// @needwork = ad
    /// @endcode
    Logger &GetLoggerByCid(std::string const &cid, std::string const &des, ara::log::LogLevel const &level) noexcept;

    /// @brief  Get Logger by ID
    /// @param[in]  cid Channel ID
    /// @param[in]  des Channel description
    /// @param[in]  level Log level
    /// @param[in]  domainId Deterministic execution domain id
    /// @return
    /// @needwork = ad
    Logger &GetLoggerByCid(std::string const &cid,
                           std::string const &des,
                           std::uint8_t const domainId,
                           ara::log::LogLevel const &level) noexcept;
    /// @brief Write cache to file
    /// @return Execution result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00146
    /// @trace_id_dd=DD_LOG_00200
    /// @needwork = ad
    /// @endcode
    OperState FlushBufferToFile() noexcept;
    /// @brief
    /// @return
    /// @needwork = dda
    bool PipeBroken() noexcept;

    bool IsLoggingBehavior() const noexcept { return logConfig_ && logConfig_->loggingBehavior; }
    /// @brief Whether initialized
    /// @return
    bool IsInit() const noexcept { return isInit_; }

    std::uint8_t GetDefaultLogLevel()
    {
        // Correct implementation: return the actual defaultLogLevel value
        if (logConfig_) {
            return logConfig_->defaultLogLevel;
        } else {
            // If logConfig_ is empty, can return a default value, e.g., kOff
            return static_cast< std::uint8_t >(LogLevel::kOff);
        }
    };

    /// @brief Get pointer
    std::shared_ptr< Logger > GetLoggerPtr(std::string const &cid) noexcept
    {
        TLoggerMap::iterator it = loggers_.find(cid);
        if (it != loggers_.end()) {
            return it->second;
        }
        return nullptr;
    }

    /// @brief  Get dummy logger

    bool IsDestroyed() const noexcept { return dEIinted_; }

    bool SetConnectionStateHandler(ClientStateCallback callback)
    {
        clientStateCallback_ = callback;
        return true;
    };

private:
    /// @brief
    /// @param cid
    /// @param des
    /// @param level
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00147
    /// @trace_id_dd=DD_LOG_00229
    /// @needwork = ad
    /// @endcode
    friend Logger &CreateLogger(ara::core::StringView cid,
                                ara::core::StringView des,
                                ara::log::LogLevel level) noexcept;

    /// @brief Create a deterministic execution Logger object.
    /// @param[in]  cid Context identifier, up to 4 characters
    /// @param[in]  des Context description
    /// @param[in]  domainId Deterministic execution domain id
    /// @param[in]  level Log level of the context
    /// @return Logger object
    ///
    /// @needwork = ad
    friend Logger &CreateLogger(ara::core::StringView cid,
                                ara::core::StringView des,
                                std::uint8_t const domainId,
                                ara::log::LogLevel level) noexcept;
    // Solution 2: Stat mode (general/fallback)
    bool _startFileCheckTimer() noexcept
    {
        // Create an independent file checker, supporting external filepath and fd input
        if (!fileStatChecker_) {
            fileStatChecker_ = std::make_shared< FileStatChecker >();
        }
        // No specific callback set by default, injected externally via SetFileCheckHandler; ensure it works properly here
        std::int32_t const msecond{5000};
        bool ok = fileStatChecker_->Start(msecond);
        if (!ok) {
            LOGERROR(__func__) << " _setFileCheckTimer failed";
        }
        return ok;
    }

    /// @brief
    /// @return

    /// @brief Create a remote log sinker
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00137
    /// @trace_id_dd=DD_LOG_00201
    /// @needwork = dda
    /// @endcode
    bool _createRemoteSinker();

    /// @brief Configure NAI adaptation
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00137
    /// @trace_id_dd=DD_LOG_00202
    /// @needwork = dda
    /// @endcode
    bool _configNaiLog();

    /// @brief Create corresponding Logger instances according to the configured log channels, pre-creating the instances
    /// @return  Number of Loggers created
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00137
    /// @trace_id_dd=DD_LOG_00210
    /// @needwork = dda
    /// @endcode
    bool _createLoggerFromConfigFile() noexcept;
    /// @brief Check the suffix
    /// @param str
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00137
    /// @trace_id_dd=DD_LOG_00211
    /// @needwork = dda
    /// @endcode
    bool _hasValidSuffix(std::string const &str) noexcept;

    /// @brief Get the directory from a path
    /// @param str
    /// @return Directory name
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00137
    /// @trace_id_dd=DD_LOG_00213
    /// @needwork = dda
    /// @endcode
    std::string _getDir(std::string const &str) noexcept;

    /// @brief Create a file sinker
    /// @param fileConfig File configuration
    /// @param logScopedData Log scope data
    void _createFileSinker(std::shared_ptr< DLTV2::DltSinkersConfig > const &fileConfig,
                           LoggerDataPtr &logScopedData) noexcept;

    /// @brief Initialize common attributes of LoggerScopeData
    /// @param logScopedData Log scope data
    /// @param cid Context ID
    /// @param des Description
    /// @param level Log level
    void _initializeLoggerScopeData(LoggerDataPtr &logScopedData,
                                    std::string const &cid,
                                    std::string const &des,
                                    ara::log::LogLevel const &level) noexcept;

    /// @brief Notify log channel update
    /// @param cid Context ID
    /// @param level Log level
    void _notifyLogChannelUpdate(std::string const &cid, ara::log::LogLevel const &level) noexcept;

    /// @brief Configuration file
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00137
    /// @trace_id_dd=DD_LOG_00215
    /// @needwork = dda
    /// @endcode
    std::shared_ptr< ApplicationConfig > logConfig_{nullptr};

    /// @brief   loggers_ key(contextid) : value(logger) saves the logger for the corresponding channel
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00137
    /// @trace_id_dd=DD_LOG_00216
    /// @needwork = dda
    /// @endcode
    TLoggerMap loggers_;

    /// @brief verbose_ Whether it is verbose log mode
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00137
    /// @trace_id_dd=DD_LOG_00217
    /// @needwork = dda
    /// @endcode
    bool verbose_{false};
    /// @brief appId_ Application ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00137
    /// @trace_id_dd=DD_LOG_00218
    /// @needwork = dda
    /// @endcode
    std::string appId_{};
    /// @brief EcuId_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00137
    /// @trace_id_dd=DD_LOG_00219
    /// @needwork = dda
    /// @endcode
    std::string ecuId_{};
    /// @brief applogLevel Application-level log level
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00137
    /// @trace_id_dd=DD_LOG_00220
    /// @needwork = dda
    /// @endcode
    std::int32_t applogLevel_{0};

    /// @brief mlogCommandExcutor_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00137
    /// @trace_id_dd=DD_LOG_00222
    /// @needwork = dda
    /// @endcode
#ifdef ARA_LOG_HAS_REMOTE
    std::shared_ptr< LogCommandExcutor > mlogCommandExcutor_{nullptr};
#endif

    /// @brief fileSinkerMap_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00137
    /// @trace_id_dd=DD_LOG_00225
    /// @needwork = dda
    /// @endcode
    std::map< std::string, std::shared_ptr< IlogSinker > > fileSinkerMap_;
    /// @brief loggerLock_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00137
    /// @trace_id_dd=DD_LOG_00228
    /// @needwork = dda
    /// @endcode
    std::mutex loggerLock_;
    /// @brief AA log configuration file
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00137
    /// @trace_id_dd=DD_LOG_00226
    /// @needwork = dda
    /// @endcode
    std::string appConfig_;
    /// @brief Global configuration file
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00137
    /// @trace_id_dd=DD_LOG_00227
    /// @needwork = dda
    /// @endcode
    std::string globalConfig_;

    /// @brief isInit_
    /// @needwork = dda
    bool isInit_{false};

    bool dEIinted_{false};

    ClientStateCallback clientStateCallback_{nullptr};

    std::shared_ptr< isoft::naicpp::EvLoop > pEventLoop_{nullptr};
    std::shared_ptr< LogManagerConfigModule > configModule_{nullptr};
    std::shared_ptr< LogManagerLoggerModule > loggerModule_{nullptr};
    std::shared_ptr< LogManagerDetermModule > determModule_{nullptr};
    // Instance of the file checker wrapper class
    std::shared_ptr< FileStatChecker > fileStatChecker_{nullptr};
    // Instance of the NaiLog wrapper class
    std::shared_ptr< NaiLogWrapper > naiLogWrapper_{nullptr};
};
}  // namespace internal
}  // namespace log
}  // namespace ara
#endif