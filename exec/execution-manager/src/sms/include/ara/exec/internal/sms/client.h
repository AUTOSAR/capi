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
/// @file       client.h
/// @brief      State client communication protocol Client class definition
/// @details
/// @date       2023-04-01
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/ExecutionManagement/SMS
/// @unit_name=Client
/// @unit_description=The Client of State Management Service.
/// @interface_level=module
/// @endcode
///
/// ================================================================

#ifndef _ARA_EXEC_INTERNAL_SMS_CLIENT_H_
#define _ARA_EXEC_INTERNAL_SMS_CLIENT_H_

#include <ara/core/map.h>

#include "ara/exec/exec_error_domain.h"
#include "ara/exec/internal/ipc/client.h"
#include "ara/exec/internal/sms/message.h"

namespace ara {
namespace exec {
namespace internal {
namespace sms {

/// @brief SMS client
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_EM_10002
/// @trace_id_ad=AD_EM_00073
/// @trace_id_dd=DD_EM_00409
/// @needwork = ad
/// @endcode
class Client
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
    static constexpr Char8_t const *GetLogCtxId() noexcept { return "SMSC"; }

 /// @brief Get log context description
 /// @return Log context description
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    static constexpr Char8_t const *GetLogCtxDesc() noexcept { return "The Client Of State Management Service"; }

 /// @brief Default constructor
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00073
    /// @trace_id_dd=DD_EM_00410
    /// @needwork = dda
    /// @endcode
    Client() = default;

 /// @brief Default destructor
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00073
    /// @trace_id_dd=DD_EM_00411
    /// @needwork = dda
    /// @endcode
    ~Client() = default;

 /// @brief Disable move construction
    /// @param other the other Config
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    Client(Client &&other) noexcept = delete;

 /// @brief Disable copy construction
    /// @param other the other Config
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    Client(Client const &other) noexcept = delete;

 /// @brief Disable move assignment
    /// @param other the other Config
 /// @return New client
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    Client &operator=(Client &&other) noexcept = delete;

 /// @brief Disable copy assignment
    /// @param other the other Config
 /// @return New client
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    Client &operator=(Client const &other) noexcept = delete;

 /// @brief Open SMC client
 /// @param spMainLoop Main event loop handle
 /// @return 0 success; <0 failure
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00073
    /// @trace_id_dd=DD_EM_00412
    /// @needwork = dda
    /// @endcode
    int32_t Open(std::shared_ptr< isoft::naicpp::EvLoop > spMainLoop) noexcept;

 /// @brief Open SMC client
 /// @return 0 success; <0 failure
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_ide_level=module
    /// @trace_id_ad=AD_EM_00073
    /// @trace_id_dd=DD_EM_00413
    /// @needwork = dda
    /// @endcode
    int32_t Open() noexcept { return Open(nullptr); }

 /// @brief Close SMC client
 /// @return 0 success; <0 failure
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00073
    /// @trace_id_dd=DD_EM_00414
    /// @needwork = dda
    /// @endcode
    int32_t Close() noexcept;

 /// @brief GetFunctionGroupState callback function
 /// @param fgName Function group name
 /// @param stateName Retrieved state name
 /// @param errorCode Error code
 /// @param execErrorCode Execution error code
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    using ResponseHandler = std::function< void(ara::core::StringView const &fgName,
                                                ara::core::StringView const &stateName,
                                                ara::exec::ExecErrc const errorCode,
                                                uint32_t const execErrorCode) >;

 /// @brief Get function group state, called asynchronously
 /// @param fgName Function group name
 /// @param cb Callback function, the result is returned through this callback
 /// @return 0 success; <0 failure
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00073
    /// @trace_id_dd=DD_EM_00415
    /// @needwork = dda
    /// @endcode
    int32_t GetFunctionGroupState(ara::core::StringView const &fgName, ResponseHandler const &cb) noexcept
    {
        return _SendMessage(Message::Operation::kGetState, fgName, "", cb);
    }

 /// @brief Get function group state, called asynchronously
 /// @param fgName Function group name
 /// @param stateName Function group state name
 /// @param cb Callback function, the result is returned through this callback
 /// @return 0 success; <0 failure
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00073
    /// @trace_id_dd=DD_EM_00416
    /// @needwork = dda
    /// @endcode
    int32_t SetFunctionGroupState(ara::core::StringView const &fgName,
                                  ara::core::StringView const &stateName,
                                  ResponseHandler const &cb) noexcept
    {
        return _SendMessage(Message::Operation::kSetState, fgName, stateName, cb);
    }

 /// @brief Register undefined state notification callback function
 /// @param cb Callback function
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00073
    /// @trace_id_dd=DD_EM_00417
    /// @needwork = dda
    /// @endcode
    void OnUndefineStateNotify(ResponseHandler const &cb) noexcept { fNotifyUndefinedStateCb_ = cb; }

private:
 /// @brief Register undefined state notification
 /// @return 0 success; <0 failure
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00073
    /// @trace_id_dd=DD_EM_00418
    /// @needwork = dda
    /// @endcode
    int32_t _RegisterUndefinedStateNotify() noexcept;

 /// @brief Send IPC asynchronous message
 /// @param opt Operation code
 /// @param fgName Function group name
 /// @param stateName Function group state name
 /// @param cb Callback function
 /// @return 0 success; <0 failure
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00073
    /// @trace_id_dd=DD_EM_00419
    /// @needwork = dda
    /// @endcode
    int32_t _SendMessage(Message::Operation const opt,
                         ara::core::StringView const &fgName,
                         ara::core::StringView const &stateName,
                         ResponseHandler const &cb) noexcept;

 /// @brief Response message handler function
 /// @param pRspMsg Message body
 /// @param msgSize Message body size
 /// @param errorCode Error code
 /// @exception std::runtime_error If processing fails
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00073
    /// @trace_id_dd=DD_EM_00420
    /// @needwork = dda
    /// @endcode
    void _HandleResponse(void *const pRspMsg, uint32_t const msgSize, int32_t const errorCode) const;

private:
 /// @brief IPC connection client side
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00073
    /// @trace_id_dd=DD_EM_00421
    /// @needwork = dda
    /// @endcode
    std::shared_ptr< ipc::Client > spIpcClient_{nullptr};  // PRQA S 2026

 /// @brief Callback function to receive undefined state notification
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00073
    /// @trace_id_dd=DD_EM_00422
    /// @needwork = dda
    /// @endcode
    ResponseHandler fNotifyUndefinedStateCb_{nullptr};

 /// @brief Record the CallId of undefined state notification
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00073
    /// @trace_id_dd=DD_EM_00423
    /// @needwork = dda
    /// @endcode
    Message::CallId undefineNotifyCallId_{0U};

 /// @brief Session manager
    class SessionManager;
 /// @brief Session manager, used to associate callId with callback function. Suitable for one-request-one-response session mode, including SetState, GetState
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00073
    /// @trace_id_dd=DD_EM_00424
    /// @needwork = dda
    /// @endcode
    std::unique_ptr< SessionManager > sessionManager_;  // PRQA S 2026

};  ///< class Client

/// @brief Session manager, used to associate callId with callback function. Suitable for one-request-one-response session mode, including SetState, GetState
/// @code{.isoft}
/// @interface_level=none
/// @trace_id_sr=SR_EM_10002
/// @trace_id_ad=AD_EM_00073
/// @trace_id_dd=DD_EM_00425
/// @needwork = dd
/// @endcode
class Client::SessionManager
{
public:
 /// @brief Default constructor
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00073
    /// @trace_id_dd=DD_EM_00426
    /// @needwork = dda
    /// @endcode
    SessionManager() = default;

 /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00073
    /// @trace_id_dd=DD_EM_00427
    /// @needwork = dda
    /// @endcode
    ~SessionManager() = default;

 /// @brief Disable move construction
    /// @param other the other SessionManager
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    SessionManager(SessionManager &&other) noexcept = delete;

 /// @brief Disable copy construction
    /// @param other the other SessionManager
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    SessionManager(SessionManager const &other) noexcept = delete;

 /// @brief Disable move assignment
    /// @param other the other SessionManager
 /// @return New SessionManager
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    SessionManager &operator=(SessionManager &&other) noexcept = delete;

 /// @brief Disable copy assignment
    /// @param other the other SessionManager
 /// @return New SessionManager
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    SessionManager &operator=(SessionManager const &other) noexcept = delete;

 /// @brief User callback function type
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    using WalkCallback = std::function< void(Message::CallId const &, ResponseHandler const &) >;

 /// @brief Insert session
 /// @param cid Call ID
 /// @param cb Callback function
 /// @exception std::bad_alloc Thrown when memory allocation fails
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00073
    /// @trace_id_dd=DD_EM_00428
    /// @needwork = dda
    /// @endcode
    void Push(Message::CallId const &cid, ResponseHandler const &cb)
    {
        std::ignore = sessions_.emplace(std::make_pair(cid, cb));
    }

 /// @brief Pop session
 /// @param cid Call ID
 /// @return User callback function
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00073
    /// @trace_id_dd=DD_EM_00429
    /// @needwork = dda
    /// @endcode
    ResponseHandler Pop(Message::CallId const &cid) noexcept
    {
        ara::core::Map< Message::CallId, ResponseHandler >::iterator const sessionIt{sessions_.find(cid)};
        if (sessions_.end() == sessionIt) {
            return nullptr;
        }

        ResponseHandler cb{sessionIt->second};
        std::ignore = sessions_.erase(cid);
        return cb;
    }

 /// @brief Clear session list
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00073
    /// @trace_id_dd=DD_EM_00430
    /// @needwork = dda
    /// @endcode
    void Clear() noexcept { sessions_.clear(); }

 /// @brief Traverse all sessions
 /// @param cb Callback function
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00073
    /// @trace_id_dd=DD_EM_00431
    /// @needwork = dda
    /// @endcode
    void Walk(WalkCallback const &cb) const noexcept
    {
        if (nullptr == cb) {
            return;
        }
        for (auto const &it : sessions_) {
            cb(it.first, it.second);
        }
    }

private:
 /// @brief Save sessions that need to switch function group
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00073
    /// @trace_id_dd=DD_EM_00432
    /// @needwork = dda
    /// @endcode
    ara::core::Map< Message::CallId, ResponseHandler > sessions_;

};  ///< class SessionManager

}  // namespace sms
}  // namespace internal
}  // namespace exec
}  // namespace ara

#endif  ///< _ARA_EXEC_INTERNAL_SMS_CLIENT_H_
