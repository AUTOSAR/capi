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
/// @file       log_command_ipc_client.h
/// @brief      IPC client encapsulation, handling IPC communication with the log daemon
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
/// @unit_name = log_command_ipc_client
/// @unit_description=IPC client communication module on the Dlt lib side
/// @endcode
///
/// ================================================================

#ifndef INTERNAL_LOG_COMMAND_IPC_CLIENT_H
#define INTERNAL_LOG_COMMAND_IPC_CLIENT_H

#include <cstdint>
#include <functional>
#include <memory>
#include <string>

#include "isoft/ipccpp/client.h"
#include "isoft/ipccpp/packet.h"
#include "isoft/naicpp/global_evloop.h"

namespace ara {
namespace log {
namespace internal {

/// @brief IPC client operation return value enumeration
enum class LogCommandIpcClientStatus
{
    /// Operation succeeded
    kSuccess = 0,
    /// Initialization failed
    kInitFailed = -1,
    /// Failed to create IPC client
    kClientCreateFailed = -2,
    /// Failed to start IPC client
    kClientStartFailed = -3,
    /// Failed to send message
    kSendFailed = -4,
    /// Object not initialized
    kNotInitialized = -5,
    /// IPC communication exception
    kIpcError = -7,
    /// Destruction failed
    kDestroyFailed = -8,
};

/// @brief IPC client class, handling communication with the log daemon
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_LOG_00209
/// @trace_id_dd=DD_LOG_00411
/// @endcode
class LogCommandIpcClient final
{
public:
    /// @brief Delete move constructor
    LogCommandIpcClient(LogCommandIpcClient &&object) noexcept = delete;
    /// @brief Delete move assignment
    LogCommandIpcClient &operator=(LogCommandIpcClient &&object) noexcept = delete;

public:
    /// @brief Delete copy constructor
    LogCommandIpcClient(LogCommandIpcClient const &other) = delete;
    /// @brief Delete copy assignment
    LogCommandIpcClient &operator=(LogCommandIpcClient const &other) = delete;

public:
    /// @brief Asynchronous send callback function type definition
    using IpcResponseCallback = std::function< void(std::string const &responseData) >;

    /// @brief Set the asynchronous response callback
    /// @param[in] callback Response data callback function
    void SetResponseCallback(IpcResponseCallback callback) noexcept { responseCallback_ = std::move(callback); }

public:
    /// @brief Constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00214
    /// @trace_id_dd=DD_LOG_00416
    /// @endcode
    LogCommandIpcClient() noexcept;

    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00215
    /// @trace_id_dd=DD_LOG_00417
    /// @endcode
    ~LogCommandIpcClient() noexcept;

    /// @brief Initialize the IPC client
    /// @return LogCommandIpcClientStatus initialization status
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00216
    /// @trace_id_dd=DD_LOG_00418
    /// @endcode
    LogCommandIpcClientStatus Init() noexcept;

    /// @brief Destroy the IPC client
    /// @return LogCommandIpcClientStatus destruction status
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00209
    /// @trace_id_dd=DD_LOG_00420
    /// @endcode
    LogCommandIpcClientStatus Destroy() noexcept;

    /// @brief Get the initialization status
    /// @return bool true means initialized, false means not initialized
    bool IsInitialized() const noexcept { return status_; }

    /// @brief Send an IPC request
    /// @param[in] cmd IPC command string (JSON format)
    /// @return std::int32_t sending result, 0 indicates success, < 0 indicates failure
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00209
    /// @trace_id_dd=DD_LOG_00423
    /// @endcode
    std::int32_t SendRequest(std::string const &cmd) noexcept;

private:
    /// @brief IPC asynchronous send callback function
    /// @param[in] context User parameter (points to the current object)
    /// @param[in] status IPC communication status
    /// @param[in] responsePacket Response data packet
    /// @code{.isoft}
    /// @threadsafety={no}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00209
    /// @trace_id_dd=DD_LOG_00424
    /// @endcode
    static void IPCClientSendAsyncHandler(void *const context,
                                          isoft::ipc::IPCClientHandlerStatus const status,
                                          isoft::ipc::IPCPacket *const responsePacket);

private:
    /// @brief IPC client handle
    std::shared_ptr< isoft::ipc::IPCClient > ipcClient_{nullptr};

    /// @brief Event loop
    std::shared_ptr< isoft::naicpp::EvLoop > mainLoop_{nullptr};

    /// @brief Initialization status flag
    bool status_{false};

    /// @brief Asynchronous response callback function
    IpcResponseCallback responseCallback_{};
};

}  // namespace internal
}  // namespace log
}  // namespace ara

#endif
