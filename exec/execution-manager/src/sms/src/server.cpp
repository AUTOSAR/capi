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
/// @file       server.cpp
/// @brief      State client communication protocol Server class definition
/// @details
/// @date       2023-04-01
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/ExecutionManagement/EMS
/// @unit_name=Server
/// @unit_description=The Server of Update Management Service.
/// @interface_level=module
/// @endcode
///
/// ================================================================

#include "ara/exec/internal/sms/server.h"

#include "ara/exec/internal/log/log.h"
#include "ara/exec/internal/sms/config.h"

#ifdef LOG
    #undef LOG
#endif

/// @brief SMS server log macro definition
/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
#define LOG() log1::Log< sms::Server >()

namespace ara {
namespace exec {
namespace internal {
namespace sms {

/// @brief Open SMC server
/// @param spMainLoop Main event loop handle
/// @return 0 success; <0 failure
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_EM_00074
/// @trace_id_dd=DD_EM_00360
/// @needwork = dda
/// @endcode
int32_t Server::Open(std::shared_ptr< isoft::naicpp::EvLoop > spMainLoop) noexcept
{
    upIpcServer_ = std::make_unique< ipc::Server >(
        [this](isoft::ipc::IPCServerHandleType const type) noexcept { _HandleConnection(type); },
        [this](uint32_t const pid, uint64_t const sid, void *const pMsg, uint32_t const msgSize) noexcept {
            std::ignore = pid;
            _HandleRequest(sid, static_cast< Message * >(pMsg), msgSize);
        });
    if (upIpcServer_ == nullptr) {
        LOGE() << "sms::Server::Open(): nullptr == upIpcServer_";
        return -1;
    }

    if (upIpcServer_->Open(std::move(spMainLoop), GetServiceName()) != 0) {
        upIpcServer_.reset();
        LOGE() << "sms::Server::Open(): Open service {" << GetServiceName() << "} failed !!!";
        return -1;
    }

    LOGD() << "sms::Server::Open(): Open service {" << GetServiceName() << "} success!!!";

    return 0;
}

/// @brief Close SMC server
/// @return 0 success; <0 failure
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_EM_00074
/// @trace_id_dd=DD_EM_00361
/// @needwork = dda
/// @endcode
int32_t Server::Close() const noexcept
{
    if (upIpcServer_ == nullptr) {
        LOGE() << "sms::Server::Close(): nullptr == upIpcServer_";
        return -1;
    }

    return upIpcServer_->Close();
}

/// @brief Handler function for connection establishment or disconnection
/// @param type Connection type
/// @code{.isoft}
/// @interface_level=none
/// @trace_id_ad=AD_EM_00074
/// @trace_id_dd=DD_EM_00369
/// @needwork = dda
/// @endcode
void Server::_HandleConnection(isoft::ipc::IPCServerHandleType const type) noexcept
{
    if (type == IPC_SERVER_HANDLER_CONNECT) {
        LOGD() << "sms Client Connected.";
    } else if (type == IPC_SERVER_HANDLER_DISCONNECT) {
        LOGD() << "sms Client Disconnected.";
        undefinedNotifySid_  = 0U;
        getFgStateSessionId_ = 0U;
        setFgStateSessions_.clear();
    } else {
        LOGW() << "Server::_HandleConnection(): Unknown Request";
    }
}

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
void Server::_HandleRequest(uint64_t const sid, Message *const pMsg, uint32_t const msgSize) noexcept
{
    if ((pMsg == nullptr) || (pMsg->GetSize() != msgSize)) {
        LOGE() << "sms::Server::_HandleRequest(): nullptr == pMsg || msgSize != pMsg->GetSize()";
        return;
    }

    Message::Operation const opt{pMsg->GetOperation()};
    Message::CallId const callId{pMsg->GetCallId()};
    ara::core::String fgName;
    ara::core::String stateName;
    pMsg->GetFunctionGroupState(fgName, stateName);

    LOGD() << "sms::Server::_HandleRequest(): FunctionGroup {" << fgName << "}, State {" << stateName << "}, Opt {"
           << static_cast< uint16_t >(opt) << "}, SessionId {" << sid << "}, CallId {" << callId
           << "}, msgSize =" << msgSize;

    FgStateHandler cb{nullptr};
    switch (opt) {
        case Message::Operation::kGetState: {
            cb                   = fGetFgStateCb_;
            getFgStateSessionId_ = sid;
            getFgStateCallId_    = callId;
            break;
        }

        case Message::Operation::kSetState: {
            ara::core::Map< ara::core::String, SetStateSession >::iterator const it{
                setFgStateSessions_.find(ara::core::String{fgName})};
 /// If the requested function group state is already in transition, cancel the ongoing function group state transition and start a new state transition
            if (it != setFgStateSessions_.end()) {
 /// Duplicate request for the same state, return directly, do not record this session
                if (it->second.GetStateName() == stateName) {
                    LOGW() << "sms::Server::_HandleRequest(): Same state {" << stateName
                           << "} transition request for FunctionGroup {" << fgName << "}";
                    ara::exec::ExecErrc const ec{ara::exec::ExecErrc::kInTransitionToSameState};
                    pMsg->SetExecutionError(0U);
                    pMsg->SetTransitionError(ec);
                    std::ignore = upIpcServer_->SendResponse(sid, pMsg, static_cast< size_t >(msgSize), true);
                    return;
                }

 /// New state transition request, cancel the ongoing state transition first
 /// Cancel also triggers a response, which will delete the original session node, so no processing is needed here
                if (fCancelFgStateCb_) {
                    fCancelFgStateCb_(fgName, it->second.GetStateName());
                }
            }
 /// Add new function group state transition session
            std::ignore = setFgStateSessions_.emplace(std::make_pair(fgName, SetStateSession{stateName, sid, callId}));
            cb          = fSetFgStateCb_;
            break;
        }

        case Message::Operation::kNotifyUndefinedState: {
            undefinedNotifySid_    = sid;
            undefinedNotifyCallId_ = callId;
            break;
        }

        default: {
            LOGE() << "sms::Server::_HandleRequest(): Unsupported Function Group Operation {"
                   << static_cast< uint16_t >(opt) << "} !!!";
            break;
        }
    }

    if (cb) {
        cb(fgName, stateName);
    }
}

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
int32_t Server::_SendResponse(Message::Operation const opt,
                              ara::core::StringView const &fgName,
                              ara::core::StringView const &fgState,
                              ara::exec::ExecErrc const &transError,
                              uint32_t const execError) noexcept
{
    if (upIpcServer_ == nullptr) {
        LOGE() << "sms::Server::SendResponse(): nullptr == upIpcServer_";
        return -1;
    }

    isoft::ipc::IPCSessionId sid{0U};
    Message::CallId callId{0U};
    bool isLastRsp{true};

    switch (opt) {
        case Message::Operation::kGetState: {
            sid    = getFgStateSessionId_;
            callId = getFgStateCallId_;
            break;
        }

        case Message::Operation::kSetState: {
            ara::core::Map< ara::core::String, ara::exec::internal::sms::Server::SetStateSession >::iterator const it{
                setFgStateSessions_.find(ara::core::String{fgName})};
            if (it == setFgStateSessions_.end()) {
                break;
            }
            sid         = it->second.GetSessionId();
            callId      = it->second.GetCallId();
            std::ignore = setFgStateSessions_.erase(it);
            break;
        }
        case Message::Operation::kNotifyUndefinedState: {
            sid       = undefinedNotifySid_;
            callId    = undefinedNotifyCallId_;
            isLastRsp = false;
            break;
        }
    }

    std::size_t const msgSize{Message::CalculateSize(fgName, fgState)};
    std::vector< uint8_t > tmpMsg(msgSize);
    Message *const pRspMsg{new (tmpMsg.data()) Message};
    if (pRspMsg == nullptr) {
        LOGE() << "sms::Server::SendResponse(): nullptr == pRspMsg";
        return -1;
    }

    pRspMsg->SetCallId(callId);
    pRspMsg->SetOperation(opt);
    pRspMsg->SetExecutionError(execError);
    pRspMsg->SetTransitionError(transError);
    pRspMsg->SetFunctionGroupState(fgName, fgState);

    LOGD() << "sms::Server::SendResponse(): FunctionGroup {" << fgName << "}, State {" << fgState << "}, Opt {"
           << static_cast< uint16_t >(opt) << "}, SessionId {" << sid << "}, CallId {" << callId
           << "}, TransitionError {" << static_cast< std::int32_t >(transError) << "}, ExecutionError {" << execError
           << "}";

    int32_t const ret{upIpcServer_->SendResponse(sid, pRspMsg, msgSize, isLastRsp)};
    if (ret != 0) {
        LOGE() << "sms::Server::SendResponse(): upIpcServer_->SendResponse() failed, ret {" << ret << "}";
    }

    pRspMsg->~Message();
    return ret;
}  // PRQA S 2707

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
int32_t Server::ResponseGetFunctionGroupState(ara::core::StringView const &fgName,
                                              ara::core::StringView const &fgState,
                                              ara::exec::ExecErrc const &te,
                                              uint32_t const ee) noexcept
{
    return _SendResponse(Message::Operation::kGetState, fgName, fgState, te, ee);
}

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
int32_t Server::ResponseSetFunctionGroupState(ara::core::StringView const &fgName,
                                              ara::exec::ExecErrc const &te,
                                              uint32_t const ee) noexcept
{
    ara::core::Map< ara::core::String, ara::exec::internal::sms::Server::SetStateSession >::iterator const it{
        setFgStateSessions_.find(ara::core::String{fgName})};
    if (it != setFgStateSessions_.end()) {
        return _SendResponse(Message::Operation::kSetState, fgName, "", te, ee);
    }

    LOGW() << "No session found for SetFunctionGroup {" << fgName << "}";
    return -1;
}

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
int32_t Server::NotifyUndefineFunctionGroupState(ara::core::StringView const &fgName,
                                                 ara::exec::ExecErrc const &te,
                                                 uint32_t const ee) noexcept
{
    return _SendResponse(Message::Operation::kNotifyUndefinedState, fgName, "Undefined", te, ee);
}

}  // namespace sms
}  // namespace internal
}  // namespace exec
}  // namespace ara
