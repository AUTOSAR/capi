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
/// @file       server.h
/// @brief      State client communication protocol Server class definition
/// @details
/// @date       2023-04-01
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/ExecutionManagement/SMS
/// @unit_name=Server
/// @unit_description=The Server of State Management Service.
/// @interface_level=module
/// @endcode
///
/// ================================================================

#ifndef _ARA_EXEC_INTERNAL_SMS_SERVER_H_
#define _ARA_EXEC_INTERNAL_SMS_SERVER_H_

#include "ara/core/map.h"
#include "ara/core/string.h"
#include "ara/exec/internal/ipc/server.h"
#include "ara/exec/internal/sms/message.h"

namespace ara {
namespace exec {
namespace internal {
namespace sms {

/// @brief SMS server class
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_EM_00054,SR_EM_00055,SR_EM_00056
/// @trace_id_ad=AD_EM_00074
/// @trace_id_dd=DD_EM_00357
/// @needwork = ad
/// @endcode
class Server
{
public:
 /// @brief char type redefinition
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    using Char8_t = char;

 /// @brief Get log context ID
 /// @return Log context ID
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    static constexpr Char8_t const *GetLogCtxId() noexcept { return "SMSS"; }

 /// @brief Get log context description
 /// @return Log context description
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    static constexpr Char8_t const *GetLogCtxDesc() noexcept { return "The Server Of State Management Service"; }

 /// @brief Default constructor
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00074
    /// @trace_id_dd=DD_EM_00358
    /// @needwork = dda
    /// @endcode
    Server() = default;

 /// @brief Default destructor
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_ide_level=module
    /// @trace_id_ad=AD_EM_00074
    /// @trace_id_dd=DD_EM_00359
    /// @needwork = dda
    /// @endcode
    ~Server() = default;

 /// @brief Disable move construction
    /// @param other the other Server
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    Server(Server &&other) noexcept = delete;

 /// @brief Disable copy construction
    /// @param other the other Server
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    Server(Server const &other) noexcept = delete;

 /// @brief Disable move assignment
    /// @param other the other Server
 /// @return New Server instance
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    Server &operator=(Server &&other) noexcept = delete;

 /// @brief Disable copy assignment
    /// @param other the other Server
 /// @return New server instance
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    Server &operator=(Server const &other) noexcept = delete;

 /// @brief Open SMC server
 /// @param spMainLoop Main event loop handle
 /// @return 0 success; <0 failure
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00074
    /// @trace_id_dd=DD_EM_00360
    /// @needwork = dda
    /// @endcode
    int32_t Open(std::shared_ptr< isoft::naicpp::EvLoop > spMainLoop) noexcept;

 /// @brief Close SMC server
 /// @return 0 success; <0 failure
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00074
    /// @trace_id_dd=DD_EM_00361
    /// @needwork = dda
    /// @endcode
    int32_t Close() const noexcept;

 /// @brief Function group state callback function type definition
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    using FgStateHandler
        = std::function< void(ara::core::StringView const &fgName, ara::core::StringView const &stateName) >;

 /// @brief Register set function group state callback function
 /// @param cb Callback function
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00074
    /// @trace_id_dd=DD_EM_00362
    /// @needwork = dda
    /// @endcode
    void OnGetFunctionGroupState(FgStateHandler const &cb) noexcept { fGetFgStateCb_ = cb; }

 /// @brief Register get function group state callback function
 /// @param cb Callback function
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00074
    /// @trace_id_dd=DD_EM_00363
    /// @needwork = dda
    /// @endcode
    void OnSetFunctionGroupState(FgStateHandler const &cb) noexcept { fSetFgStateCb_ = cb; }

 /// @brief Register cancel function group state transition callback function
 /// @param cb Callback function
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00074
    /// @trace_id_dd=DD_EM_00364
    /// @needwork = dda
    /// @endcode
    void OnCancelFunctionGroupStateTransition(FgStateHandler const &cb) noexcept { fCancelFgStateCb_ = cb; }

 /// @brief After state transition is complete, reply to client with the completed state
 /// @param fgName Function group name
 /// @param te Translation error code
 /// @param ee Execution error code
 /// @return 0 success; <0 failure
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00074
    /// @trace_id_dd=DD_EM_00365
    /// @needwork = dda
    /// @endcode
    int32_t ResponseSetFunctionGroupState(ara::core::StringView const &fgName,
                                          ara::exec::ExecErrc const &te,
                                          uint32_t const ee) noexcept;

 /// @brief Reply to client with the obtained function group state
 /// @param fgName Function group name
 /// @param fgState Function group state
 /// @param te Translation error code
 /// @param ee Execution error code
 /// @return 0 success; <0 failure
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00074
    /// @trace_id_dd=DD_EM_00366
    /// @needwork = dda
    /// @endcode
    int32_t ResponseGetFunctionGroupState(ara::core::StringView const &fgName,
                                          ara::core::StringView const &fgState,
                                          ara::exec::ExecErrc const &te,
                                          uint32_t const ee) noexcept;

 /// @brief When entering an undefined function group state, notify the client
 /// @param fgName Function group name
 /// @param te Translation error code
 /// @param ee Execution error code
 /// @return 0 success; <0 failure
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00074
    /// @trace_id_dd=DD_EM_00367
    /// @needwork = dda
    /// @endcode
    int32_t NotifyUndefineFunctionGroupState(ara::core::StringView const &fgName,
                                             ara::exec::ExecErrc const &te,
                                             uint32_t const ee) noexcept;

private:
 /// @brief Handler function for connection establishment or disconnection
 /// @param type Connection type
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00074
    /// @trace_id_dd=DD_EM_00369
    /// @needwork = dda
    /// @endcode
    void _HandleConnection(isoft::ipc::IPCServerHandleType const type) noexcept;

 /// @brief Handle request message
 /// @param sid Session ID
 /// @param pMsg Message body
 /// @param msgSize Message body size
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00074
    /// @trace_id_dd=DD_EM_00370
    /// @needwork = dda
    /// @endcode
    void _HandleRequest(uint64_t const sid, Message *const pMsg, uint32_t const msgSize) noexcept;

 /// @brief Send response message
 /// @param opt Operation code
 /// @param fgName Function group name
 /// @param fgState State name
 /// @param transError Error code
 /// @param execError Execution error code
 /// @return 0 success; <0 failure
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00074
    /// @trace_id_dd=DD_EM_00371
    /// @needwork = dda
    /// @endcode
    int32_t _SendResponse(Message::Operation const opt,
                          ara::core::StringView const &fgName,
                          ara::core::StringView const &fgState,
                          ara::exec::ExecErrc const &transError,
                          uint32_t const execError) noexcept;

private:
 /// @brief Set state session definition
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_EM_00054,SR_EM_00055,SR_EM_00056
    /// @trace_id_ad=AD_EM_00074
    /// @trace_id_dd=DD_EM_00372
    /// @needwork = dd
    /// @endcode
    class SetStateSession
    {
    public:
 /// @brief Constructor
 /// @param name Function group name
 /// @param sid Session ID
        /// @param cid Call ID
        /// @code{.isoft}
        /// @interface_level=none
        /// @trace_id_ad=AD_EM_00074
        /// @trace_id_dd=DD_EM_00801
        /// @needwork = dd
        /// @endcode
        SetStateSession(ara::core::StringView const &name,
                        isoft::ipc::IPCSessionId const sid,
                        Message::CallId const cid) noexcept
            : stateName_{name}, sid_{sid}, callId_{cid}
        {
        }

 /// @brief Get state name
 /// @return State name
        /// @code{.isoft}
        /// @interface_level=none
        /// @needwork = no
        /// @endcode
        ara::core::String GetStateName() const noexcept { return stateName_; }

 /// @brief Get session id
        /// @return session id
        /// @code{.isoft}
        /// @interface_level=none
        /// @needwork = no
        /// @endcode
        isoft::ipc::IPCSessionId GetSessionId() const noexcept { return sid_; }

 /// @brief Get call id
        /// @return call id
        /// @code{.isoft}
        /// @interface_level=none
        /// @needwork = no
        /// @endcode
        Message::CallId GetCallId() const noexcept { return callId_; }

    private:
 /// @brief Function group state
        /// @code{.isoft}
        /// @interface_level=none
        /// @trace_id_ad=AD_EM_00074
        /// @trace_id_dd=DD_EM_00802
        /// @needwork = dd
        /// @endcode
        ara::core::String stateName_;

 /// @brief Session ID
        /// @code{.isoft}
        /// @interface_level=none
        /// @trace_ide_level=none
        /// @trace_id_ad=AD_EM_00074
        /// @trace_id_dd=DD_EM_00803
        /// @needwork = dd
        /// @endcode
        isoft::ipc::IPCSessionId sid_;

        /// @brief Call ID
        /// @code{.isoft}
        /// @interface_level=none
        /// @trace_id_ad=AD_EM_00074
        /// @trace_id_dd=DD_EM_00804
        /// @needwork = dd
        /// @endcode
        Message::CallId callId_;
    };

private:
 /// @brief IPC service handle
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00074
    /// @trace_id_dd=DD_EM_00373
    /// @needwork = dda
    /// @endcode
    std::unique_ptr< ipc::Server > upIpcServer_{nullptr};  // PRQA S 2026

 /// @brief Get function group state callback function
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00074
    /// @trace_id_dd=DD_EM_00374
    /// @needwork = dda
    /// @endcode
    FgStateHandler fGetFgStateCb_{nullptr};

 /// @brief Remove function group state callback function
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00074
    /// @trace_id_dd=DD_EM_00375
    /// @needwork = dda
    /// @endcode
    FgStateHandler fCancelFgStateCb_{nullptr};

 /// @brief Callback function for set function group state event
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00074
    /// @trace_id_dd=DD_EM_00376
    /// @needwork = dda
    /// @endcode
    FgStateHandler fSetFgStateCb_{nullptr};

 /// @brief Session ID for undefined state notification
 /// FIXME: There may be multiple client instances, each needs to respond. A session manager similar to setFgStateSessions needs to be implemented in the future
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00074
    /// @trace_id_dd=DD_EM_00377
    /// @needwork = dda
    /// @endcode
    isoft::ipc::IPCSessionId undefinedNotifySid_{0};

    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00074
    /// @trace_id_dd=DD_EM_00378
    /// @needwork = dda
    /// @endcode
    Message::CallId undefinedNotifyCallId_{0};

 /// @brief Session ID for get function group state
 /// FIXME: Clients may send multiple requests concurrently, each needs to respond. A session manager similar to setFgStateSessions needs to be implemented in the future
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00074
    /// @trace_id_dd=DD_EM_00379
    /// @needwork = dda
    /// @endcode
    isoft::ipc::IPCSessionId getFgStateSessionId_{0};

    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00074
    /// @trace_id_dd=DD_EM_00380
    /// @needwork = dda
    /// @endcode
    Message::CallId getFgStateCallId_{0};

 /// @brief Clients may send multiple requests concurrently, each needs to respond
 /// @note When the same function group needs to be terminated during a transition and switched to another state, reply directly at the SMC layer, then replace with a new session, save sessions that need to switch function group
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00074
    /// @trace_id_dd=DD_EM_00381
    /// @needwork = dda
    /// @endcode
    ara::core::Map< ara::core::String, SetStateSession > setFgStateSessions_;

};  ///< class Server

}  // namespace sms
}  // namespace internal
}  // namespace exec
}  // namespace ara

#endif  ///< _ARA_EXEC_INTERNAL_SMS_SERVER_H_
