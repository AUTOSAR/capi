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
/// @file       log_command_excutor.h
/// @brief      Template class for encapsulating log parameters
/// @details
/// @date       2024-06-28
/// @author     yangjinbiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/LOG/DltProtocol
/// @interface_level = unit
/// @trace_id_sr=LOG_SR_00001,LOG_SR_00003
/// @unit_name = log_command_excutor
/// @unit_description=Implementation of command log forwarding on the Dlt lib side
/// @endcode
///
/// ================================================================

#ifndef INTERNAL_LOG_LIBCOMMANDRECEIVER
#define INTERNAL_LOG_LIBCOMMANDRECEIVER
#include <rapidjson/document.h>

#include <cstdint>
#include <functional>
#include <memory>
#include <string>

#include "DltLogDefines/application_config.h"
#include "ara/core/map.h"
#include "ara/core/string_view.h"
#include "common.h"

namespace ara {
namespace log {
namespace internal {

/// @brief LogCommandExcutor operation return value enumeration
/// @details Used to represent the result status of operations such as initialization, update, destruction, etc.
enum class LogCommandExcutorStatus
{
    /// Operation succeeded
    kSuccess = 0,
    /// Initialization failed (IPC initialization failed)
    kInitFailed = -1,
    /// Failed to create IPC client
    kIpcClientCreateFailed = -2,
    /// Failed to start IPC client
    kIpcClientStartFailed = -3,
    /// Failed to send message
    kSendFailed = -4,
    /// Object not initialized
    kNotInitialized = -5,
    /// Invalid parameter
    kInvalidParameter = -6,
    /// IPC communication exception
    kIpcError = -7,
    /// Destruction failed
    kDestroyFailed = -8,
};

/// @brief Command execution class
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_LOG_00209
/// @trace_id_dd=DD_LOG_00411
/// @needwork = ad
/// @endcode
class LogCommandExcutor final
{
public:
    /// @briefMove constructor
    /// @param[in] object LogCommandExcutor&& object
    LogCommandExcutor(LogCommandExcutor &&object) noexcept = delete;
    /// @brief Move operation
    /// @param[in] object LogCommandExcutor&& object
    /// @return LogCommandExcutor&
    LogCommandExcutor &operator=(LogCommandExcutor &&object) noexcept = delete;

public:
    using OnRegistStatusChangedCallback = std::function< void(std::int32_t const &status) >;
    void SetOnRegistStatusChangedCallback(OnRegistStatusChangedCallback cb) noexcept
    {
        onRegistStatusChangedCallback_ = std::move(cb);
    }
    using OnLogLevelChangedCallback = std::function< void(std::string const &contextid, std::uint8_t const &logLevel) >;
    void SetOnLogLevelChangedCallback(OnLogLevelChangedCallback cb) noexcept
    {
        onLogLevelChangedCallback_ = std::move(cb);
    }
    using OnClientstateChangedCallback = std::function< void(ara::log::ClientState state) >;
    void SetOnClientstateChangedCallback(OnClientstateChangedCallback cb) noexcept
    {
        onClientstateChangedCallback_ = std::move(cb);
    }

    using SendRequestCallback = std::function< std::int32_t(std::string const &) >;
    void SetSendRequestCallback(SendRequestCallback cb) noexcept { sendRequestCallback_ = std::move(cb); }

    /// @brief Constructor
    /// @param[in]  config Configuration information
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00214
    /// @trace_id_dd=DD_LOG_00416
    /// @needwork = ad
    /// @endcode
    explicit LogCommandExcutor(std::shared_ptr< ApplicationConfig > const &config) noexcept;

    /// @brief Default constructor
    /// @param other
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00357
    /// @trace_id_dd=DD_LOG_01756
    /// @needwork = ad
    /// @endcode
    LogCommandExcutor(LogCommandExcutor const &other) = default;
    /// @brief Default assignment
    /// @param other
    /// @return Reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00358
    /// @trace_id_dd=DD_LOG_01757
    /// @needwork = ad
    /// @endcode
    LogCommandExcutor &operator=(LogCommandExcutor const &other) = default;
    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00215
    /// @trace_id_dd=DD_LOG_00417
    /// @needwork = ad
    /// @endcode
    ~LogCommandExcutor() noexcept;

    /// @brief Initialize
    /// @return LogCommandExcutorStatus initialization status
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00216
    /// @trace_id_dd=DD_LOG_00418
    /// @needwork = ad
    /// @endcode
    LogCommandExcutorStatus Init() noexcept;

    bool StartBeatHeartCheck() noexcept;
    /// @brief Handle control commands from the daemon
    /// @param[in]  jsonstr
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00209
    /// @trace_id_dd=DD_LOG_00422
    /// @needwork = dda
    /// @endcode
    void HandleDaemonCmd(std::string const &jsonstr) noexcept;
    /// @brief Update channel information
    /// @param[in]  appId  Application ID
    /// @param[in]  ctxId Channel ID
    /// @param[in]  logLevel  Log level
    /// @return LogCommandExcutorStatus operation status
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00217
    /// @trace_id_dd=DD_LOG_00419
    /// @needwork = ad
    /// @endcode
    LogCommandExcutorStatus UpdateLogChannel(std::string const &appId,
                                             std::string const &ctxId,
                                             std::uint8_t const &logLevel) noexcept;
    /// @brief Release resources, called in the destructor
    /// @return LogCommandExcutorStatus destruction status
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00209
    /// @trace_id_dd=DD_LOG_00420
    /// @needwork = dda
    /// @endcode
    LogCommandExcutorStatus LogCommandDestroy() noexcept;

    /// @brief Register the application
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00209
    /// @trace_id_dd=DD_LOG_00421
    /// @needwork = dda
    /// @endcode
    void RegistApp() noexcept;

private:
    void HandleDltServiceCmd(std::int32_t cmd, rapidjson::Document const &d) noexcept;
    void HandleUserCmd(std::int32_t cmd) noexcept;

    OnClientstateChangedCallback onClientstateChangedCallback_{};
    OnLogLevelChangedCallback onLogLevelChangedCallback_{};
    OnRegistStatusChangedCallback onRegistStatusChangedCallback_{};
    /// @brief config_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00209
    /// @trace_id_dd=DD_LOG_00425
    /// @needwork = dda
    /// @endcode
    std::shared_ptr< ApplicationConfig > config_{nullptr};

    /// @brief Send request callback, injected externally
    SendRequestCallback sendRequestCallback_{};

    /// @brief Initialization status flag
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00209
    /// @trace_id_dd=DD_LOG_00429
    /// @needwork = dda
    /// @endcode
    bool status_{false};
};

}  // namespace internal
}  // namespace log
}  // namespace ara
#endif