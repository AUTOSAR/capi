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
/// @file       debugger.h
/// @brief      Debugger class definition file
/// @details
/// @date       2023-11-17
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================

#ifndef _ARA_EXEC_INTERNAL_DEBUG_DEBUGGER_H_
#define _ARA_EXEC_INTERNAL_DEBUG_DEBUGGER_H_

#include <ara/core/string.h>
#include <ara/core/string_view.h>
#include <ara/core/vector.h>
#include <isoft/naicpp/evloop.h>
#include <nai/io/nai_io.h>

#include <cstdint>

#include "ara/exec/internal/rds/debug_info.h"
#include "isoft/osi/network/socket_stream.h"

namespace ara {
namespace exec {
namespace internal {
namespace rds {

/// @brief Debugger class definition
class Debugger
{
public:
 /// @brief Default constructor
    Debugger() = default;

 /// @brief Default destructor
    ~Debugger() = default;

 /// @brief Disable copy constructor
    /// @param  other
    Debugger(Debugger& other) = delete;

 /// @brief Disable move constructor
    /// @param  other
    Debugger(Debugger&& other) = delete;

 /// @brief Disable copy assignment function
    /// @param other the other instance
 /// @return New Debugger instance
    Debugger& operator=(Debugger const& other) = delete;

 /// @brief Disable move assignment function
    /// @param other the other instance
 /// @return New Debugger instance
    Debugger& operator=(Debugger&& other) = delete;

 /// @brief Open debugger
 /// @param mainLoop Main loop
 /// @param serverPort Service port
 /// @return Open result
    int32_t Open(std::shared_ptr< isoft::naicpp::EvLoop > mainLoop, uint16_t const serverPort) noexcept;

 /// @brief Close debugger
 /// @return 0 success; <0 failure
    int32_t Close() noexcept;

 /// @brief Define callback function to get function group information
    using GetFunctionGroupInfoCallBack
        = std::function< bool(ara::core::StringView const& name, FunctionGroupInfo& info) >;

 /// @brief Define callback function to get process information
    using GetProcessInfoCallBack = std::function< bool(ara::core::StringView const& name, ProcessInfo& info) >;

 /// @brief Define callback function to handle trace request
    using TraceRequestCallback = std::function< void(ara::core::Vector< ara::core::String > const& procList) >;

 /// @brief Define callback function to cancel process trace request
    using TraceCancelCallback = std::function< void(ara::core::Vector< ara::core::String > const& procList) >;

 /// @brief Send function group state change notification
 /// @param fgInfo Function group information
    void NotifyFunctionGroupStateChanged(FunctionGroupInfo const& fgInfo) noexcept;

 /// @brief Send process state change notification
 /// @param procInfo Process information
    void NotifyProcessStateChanged(ProcessInfo const& procInfo) noexcept;

 /// @brief Send process trace ready notification
 /// @param procInfo Process information
    void NotifyProcessTraceReady(ProcessInfo const& procInfo) noexcept;

 /// @brief Set callback function, called when Debugger needs to passively obtain function group information
 /// @param getFgInfoCb Callback function to get function group information
    void OnGetFunctionGroupInfo(GetFunctionGroupInfoCallBack const& getFgInfoCb) noexcept
    {
        getFunctionGroupInfoCb_ = getFgInfoCb;
    }

 /// @brief Set callback function, called when Debugger needs to passively obtain process information
 /// @param getProcInfoCb Callback function to get process information
    void OnGetProcessInfo(GetProcessInfoCallBack const& getProcInfoCb) noexcept { getProcessInfoCb_ = getProcInfoCb; }

 /// @brief Set callback function, called when debugger receives a trace request
 /// @param traceRequestCb Callback function to handle process trace request
    void OnTraceRequest(TraceRequestCallback const& traceRequestCb) noexcept { requestCb_ = traceRequestCb; }

 /// @brief Set callback function, called when debugger receives a trace cancel request
 /// @param traceCancelCb Callback function to handle cancel process trace
    void OnTraceCancel(TraceCancelCallback const& traceCancelCb) noexcept { cancelCb_ = traceCancelCb; }

 /// @brief Determine whether the service is already started
 /// @return true service started; false service not started
    bool IsServiceOn() const noexcept
    {
        if (mainLoop_) {
            return true;
        }

        return false;
    }

private:
 /// @brief Listen event handler function
 /// @param stream Data stream
 /// @param events Events
 /// @return Processing result
    static int32_t NaiListenEventHandler(nai_stream_t* const stream, int32_t const events) noexcept;

 /// @brief Data event handler function
 /// @param stream Data stream
 /// @param events Events
 /// @return Processing result
    static int32_t NaiDataEventHandler(nai_stream_t* const stream, int32_t const events) noexcept;

 /// @brief Open NAI stream handle
 /// @param stream Stream handle reference
 /// @param fd File descriptor
 /// @param cb Callback function
 /// @return 0 success; <0 failure
    int32_t _OpenNaiStream(nai_stream_t& stream, int32_t const fd, nai_iobase_cb_f const cb) noexcept;

 /// @brief Initialize data stream
 /// @return 0 success; <0 failure
    int32_t _InitNaiStream() noexcept;

 /// @brief Parse request
 /// @return 0 success; <0 failure
    int32_t _ParseRequest() noexcept;

private:
 /// @brief Passive function group information callback function
    GetFunctionGroupInfoCallBack getFunctionGroupInfoCb_{nullptr};

 /// @brief Passive process information callback function
    GetProcessInfoCallBack getProcessInfoCb_{nullptr};

 /// @brief Debugger ready callback function
    TraceRequestCallback requestCb_{nullptr};

 /// @brief Cancel debug callback function
    TraceCancelCallback cancelCb_{nullptr};

 /// @brief Main event loop
    std::shared_ptr< isoft::naicpp::EvLoop > mainLoop_{nullptr};

    /// @brief socket server
    isoft::osi::network::socket::StreamServer socketServer_;

 /// @brief dataSockStream_ NAI socket handle for sending and receiving data
    nai_stream_t dataSockStream_{};

 /// @brief listenSockStream_ NAI socket handle for listening
    nai_stream_t listenSockStream_{};
};

}  // namespace rds
}  // namespace internal
}  // namespace exec
}  // namespace ara

#endif  ///< _ARA_EXEC_INTERNAL_DEBUG_DEBUGGER_H_