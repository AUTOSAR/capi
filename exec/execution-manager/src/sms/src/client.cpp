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
/// @file       client.cpp
/// @brief      State client communication protocol Client class definition
/// @details
/// @date       2023-04-01
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/ExecutionManagement/EMS
/// @unit_name=Client
/// @unit_description=The Client of Update Management Service.
/// @interface_level=module
/// @endcode
///
/// ================================================================

#include "ara/exec/internal/sms/client.h"

#include "ara/exec/internal/config/sysconfig.h"
#include "ara/exec/internal/log/log.h"
#include "ara/exec/internal/sms/config.h"

#ifdef LOG
    #undef LOG
#endif

/// @brief SMS client log macro definition
/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
#define LOG() log1::Log< sms::Client >()

namespace ara {
namespace exec {
namespace internal {
namespace sms {

/// @brief Open SMC client
/// @param spMainLoop Main event loop handle
/// @return 0 success; <0 failure
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_EM_00073
/// @trace_id_dd=DD_EM_00412
/// @needwork = dda
/// @endcode
int32_t Client::Open(std::shared_ptr< isoft::naicpp::EvLoop > spMainLoop) noexcept
{
    if (spIpcClient_) {
        LOGE() << "sms::Client::Open(): Already Opened !!!";
        return -1;
    }

    spIpcClient_
        = std::make_shared< ipc::Client >([this](void *const pRspMsg, uint32_t const msgSize, int32_t const errorCode) {
              _HandleResponse(pRspMsg, msgSize, errorCode);
          });

    if (spIpcClient_ == nullptr) {
        LOGE() << "sms::Client::Open(): Failed to create spIpcClient_";
        return -1;
    }

    if (0 != spIpcClient_->Open(std::move(spMainLoop), config::GetExecutionManagerIpcName(), GetServiceName())) {
        spIpcClient_.reset();
        return -1;
    }

    sessionManager_ = std::make_unique< SessionManager >();
    if (sessionManager_ == nullptr) {
        LOGE() << "sms::Client::Open(): nullptr == sessionManager_";
        return -1;
    }

 /// Register Undefined notification
    if (_RegisterUndefinedStateNotify() != 0) {
        LOGE() << "_RegisterUndefinedStateNotify() failed !!!";
        return -1;
    }

    return 0;
}

/// @brief Close SMC client
/// @return 0 success; <0 failure
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_EM_00073
/// @trace_id_dd=DD_EM_00414
/// @needwork = dda
/// @endcode
int32_t Client::Close() noexcept
{
    if (spIpcClient_ == nullptr) {
        LOGE() << "sms::Client::Close(): nullptr == spIpcClient_";
        return -1;
    }

    if (0 != spIpcClient_->Close()) {
        return -1;
    }

    sessionManager_.reset();
    return 0;
}

/// @brief Register undefined state notification
/// @return 0 success; <0 failure
/// @code{.isoft}
/// @interface_level=none
/// @trace_id_ad=AD_EM_00073
/// @trace_id_dd=DD_EM_00418
/// @needwork = dda
/// @endcode
int32_t Client::_RegisterUndefinedStateNotify() noexcept
{
    ResponseHandler const undefinedStateCb{
        [this](ara::core::StringView const &fgName, ara::core::StringView const &stateName,
               ara::exec::ExecErrc const errorCode, uint32_t const execErrorCode) -> void {
            if (fNotifyUndefinedStateCb_) {
                fNotifyUndefinedStateCb_(fgName, stateName, errorCode, execErrorCode);
            }
        }};
    return _SendMessage(Message::Operation::kNotifyUndefinedState, "", "", undefinedStateCb);
}

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
void Client::_HandleResponse(void *const pRspMsg, uint32_t const msgSize, int32_t const errorCode) const
{
 /// Communication failure, return error to all sessions and clear sessions
    if (errorCode != 0) {
        SessionManager::WalkCallback const walkSessionCb{
            [](Message::CallId const &cid, ResponseHandler const &cb) -> void {
                if (cb) {
                    std::ignore = cid;
                    cb("", "", ara::exec::ExecErrc::kCommunicationError, 0U);
                }
            }};

        sessionManager_->Walk(walkSessionCb);
        sessionManager_->Clear();
        return;
    }

    if ((pRspMsg == nullptr) || (msgSize == 0U)) {
        LOGE() << "sms::Client::HandleResponse(): Invalid response message, msgSize =" << msgSize
               << ", errorCode =" << errorCode;
        return;
    }

    while (true) {
        Message *const pMsg{static_cast< Message * >(pRspMsg)};
        if (pMsg->GetSize() != msgSize) {
            LOGE() << "Invalid Message Size";
            break;
        }

        ara::core::String fgName;
        ara::core::String stateName;
        pMsg->GetFunctionGroupState(fgName, stateName);

        LOGD() << "sms::Client::HandleResponse(): FunctionGroup {" << fgName << "}, State {" << stateName << "}, Opt {"
               << static_cast< uint16_t >(pMsg->GetOperation()) << "}, CallId {" << pMsg->GetCallId()
               << "}, TransitionError {" << static_cast< int32_t >(pMsg->GetTransitionError()) << "}, ExecutionError {"
               << pMsg->GetExecutionError() << "}";

        Message::CallId const callId{pMsg->GetCallId()};
        ResponseHandler callBack{nullptr};  // PRQA S 4117

 /// Undefined notification is not in the session manager, callback function is taken out separately
        if (callId == undefineNotifyCallId_) {
            if (pMsg->GetOperation() != Message::Operation::kNotifyUndefinedState) {
                LOGE() << "sms::Client::HandleResponse(): Invalid Undefined State Notification Message !!!";
                break;
            }
            callBack = fNotifyUndefinedStateCb_;
        } else {
            callBack = sessionManager_->Pop(callId);
        }

        if (callBack == nullptr) {
            LOGE() << "sms::Client::HandleResponse(): Invalid callId {" << callId << "}";
            return;
        }

        callBack(fgName, stateName, pMsg->GetTransitionError(), pMsg->GetExecutionError());
        return;
    }
}

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
int32_t Client::_SendMessage(Message::Operation const opt,
                             ara::core::StringView const &fgName,
                             ara::core::StringView const &stateName,
                             ResponseHandler const &cb) noexcept
{
    if (spIpcClient_ == nullptr) {
        LOGE() << "sms::Client::SendMessage(): nullptr == spIpcClient_ !!!";
        return -1;
    }

    if (cb == nullptr) {
        LOGE() << "sms::Client::SendMessage(): Invalid Cb !!!";
        return -1;
    }

    Message::CallId const cid{Message::GenCallId()};
    uint16_t const msgSize{Message::CalculateSize(fgName, stateName)};
    std::vector< uint8_t > tmpMsg(msgSize);
    Message *const pMsg{new (tmpMsg.data()) Message};
    if (pMsg == nullptr) {
        LOGE() << "sms::Client::SendMessage(): Failed to allocate memory for Message";
        return -1;
    }

    pMsg->SetCallId(cid);
    pMsg->SetOperation(opt);
    pMsg->SetFunctionGroupState(fgName, stateName);

    LOGD() << "sms::Client::SendMessage(): FunctionGroup {" << fgName << "}, State {" << stateName << "}, Opt {"
           << static_cast< uint16_t >(opt) << "}, CallId {" << cid << "}, MsgSize {" << msgSize << "}";

    int32_t ret{-1};
    switch (opt) {
        case Message::Operation::kGetState: {
 /// Insert this call into the session manager
            sessionManager_->Push(cid, cb);
            int32_t const kTimeout{3000};
            ret = spIpcClient_->SendAsync(pMsg, static_cast< uint64_t >(msgSize), kTimeout);
            break;
        }

        case Message::Operation::kSetState: {
 /// Insert this call into the session manager
            sessionManager_->Push(cid, cb);
            ret = spIpcClient_->SendAsync(pMsg, static_cast< uint64_t >(msgSize), -1);
            break;
        }

        case Message::Operation::kNotifyUndefinedState: {
 /// Undefined notification, persistent, does not need to be added to the session manager.
            undefineNotifyCallId_ = cid;
            ret                   = spIpcClient_->Subscribe(pMsg, static_cast< uint64_t >(msgSize));
            break;
        }

        default: {
            ret = -1;
            break;
        }
    }

    pMsg->~Message();

    if (ret < 0) {
        LOGE() << "sms::Client::SendMessage() failed !!!";
    }

    return ret;
}  // PRQA S 2707

}  // namespace sms
}  // namespace internal
}  // namespace exec
}  // namespace ara
