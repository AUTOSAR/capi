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
/// @brief      Communication server side
/// @details
/// @date       2024-06-25
/// @author     lianglongxiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/StateManagement/Utils
/// @interface_level=module
/// @trace_id_sr=SR_SM_02001, SR_SM_02002
/// @unit_name=Server
/// @unit_description=Communication server side
/// @endcode
///
/// ================================================================

#include "server.h"

#include <ara/log/logger.h>
#include <isoft/naicpp/evloop.h>
#include <isoft/naicpp/global_evloop.h>

#include "function_thread_safe.h"
using isoft::naicpp::GlobalGeneralEvLoop;

namespace ara {
namespace sm {
namespace fg_state_ipc {

/// @brief Process message
/// @param context
/// @param type
/// @param packet
void Server::SMsgHandler(void *const context,
                         isoft::ipc::IPCServerHandleType const type,
                         isoft::ipc::IPCPacket *const packet)
{
    Server *const pFGStateNotify{static_cast< Server * >(context)};
    pFGStateNotify->_MsgHandler(type, packet);
}

/// @brief Process message
/// @param type
/// @param reqPacket
void Server::_MsgHandler(isoft::ipc::IPCServerHandleType const type, isoft::ipc::IPCPacket *const reqPacket) noexcept
{
    log_.LogInfo() << "Server::_MsgHandler(), type:" << static_cast< uint32_t >(type);

    if (type == IPC_SERVER_HANDLER_CONNECT) {
        log_.LogDebug() << "Server::_MsgHandler(), Client Connected.";
    } else if (type == IPC_SERVER_HANDLER_DISCONNECT) {
        log_.LogDebug() << "Server::_MsgHandler(), Client Disconnected.";
    } else {
        isoft::ipc::IPCSessionId sessionId{reqPacket->GetSessionId()};
        isoft::ipc::IPCPacketBufferReference *const reqBuffer{reqPacket->GetBuffer()};  // request buffer
        uint8_t *const request{reqBuffer->GetPtr()};
        size_t const requestSize{reqBuffer->GetSize()};
        if (request[0] == static_cast< uint8_t >(MsgType::kRequestAllFGState)) {
            log_.LogDebug() << "Server::_MsgHandler(), got a RequestAllFGState.";
            _SendAllFGState(sessionId);
        } else if (request[0] == static_cast< uint8_t >(MsgType::kSubscribeFGState)) {
            log_.LogDebug() << "Server::_MsgHandler(), got a SubscribeFGState.";

            SubscribeFGStateMsg receiveMsg;
            receiveMsg.Deserialize(request, requestSize);

            // TODO(longxiao.liang): Check the validity of the message, if invalid, send a response message to the client (using request)
            // Send response message
            SubscribeFGStateRespMsg sendMsg{0U, ara::sm::fg_state_ipc::Get_ERROR_INFO_SUCCESS()};
            _SendSubscribeFGStateResp(sessionId, sendMsg);

            // TODO(longxiao.liang): If SendSubscribeFGStateResp succeeds, add the request to subscriberMap_, otherwise delete the request
            //  It is necessary to modify SendSubscribeFGStateResp to have a return value and add a condition variable wait
            core::String subscriberId{receiveMsg.GetSubscriberId()};
            std::ignore = subscriberMap_.insert(std::make_pair(subscriberId, sessionId));
            log_.LogDebug() << "Server::_MsgHandler(), we dealed with a subscriber"
                            << receiveMsg.GetSubscriberId().c_str() << "sessionId:" << sessionId;
        } else if (request[0] == static_cast< uint8_t >(MsgType::kUnsubscribeFGState)) {
            log_.LogDebug() << "Server::_MsgHandler(), got a UnsubscribeFGState.";

            SubscribeFGStateMsg receiveMsg;
            receiveMsg.Deserialize(request, requestSize);

            // TODO(longxiao.liang): Check the validity of the message
            core::String const subscriberId{receiveMsg.GetSubscriberId()};
            std::map< core::String, isoft::ipc::IPCSessionId >::iterator const it{subscriberMap_.find(subscriberId)};

            // Send response message
            SubscribeFGStateRespMsg sendMsg{0U, ara::sm::fg_state_ipc::Get_ERROR_INFO_SUCCESS(),
                                            MsgType::kUnsubscribeFGStateResp};

            // Do not use SendSubscribeFGStateResp here because when request[0] equals MsgType::kSubscribeFGState, it is more reasonable not to delete the request; here it is reasonable to delete the request
            _SendUnsubscribeFGStateResp(sessionId, sendMsg);

            // TODO(longxiao.liang): Delete only when SendUnsubscribeFGStateResp succeeds
            //  It is necessary to modify SendUnsubscribeFGStateResp to have a return value and add a condition variable wait
            std::ignore = subscriberMap_.erase(it);

            log_.LogDebug() << "Server::_MsgHandler, we dealed with unSubscriber"
                            << receiveMsg.GetSubscriberId().c_str();
        } else {
            assert(false);
        }
    }
}

/// @brief Send all states
/// @param sessionId Session ID
void Server::_SendAllFGState(isoft::ipc::IPCSessionId const &sessionId) noexcept
{
    log_.LogInfo() << "Server::_SendAllFGState()";
    _GetAndSendAllFgStates(sessionId);
}

/// @brief Send subscription state reply
/// @param sessionId
/// @param sendMsg
void Server::_SendSubscribeFGStateResp(isoft::ipc::IPCSessionId const &sessionId,
                                       SubscribeFGStateRespMsg &sendMsg) const noexcept
{
    log_.LogInfo() << "Server::_SendSubscribeFGStateResp()";

    // Create message
    isoft::ipc::IPCPacket *const responsePacket{ipcServerPtr_->MakeResponse(sessionId)};
    if (nullptr == responsePacket) {
        log_.LogError() << "Server::_SendSubscribeFGStateResp(), failed to MakeResponse.";
        return;
    }
    std::size_t const buffSize{sendMsg.GetSize()};
    isoft::ipc::IPCPacketBufferReference *const resBuffer{responsePacket->AppendBuffer(buffSize)};
    sendMsg.Serialize(resBuffer->GetPtr());
    std::ignore = resBuffer->SetLen(buffSize);

    // Send message
    if (0 != ipcServerPtr_->Send(responsePacket, false)) {
        log_.LogError() << "Server::_SendSubscribeFGStateResp(), failed to Send with err"
                        << ara::core::internal::safe_strerror(errno);
        return;
    }
}
/// @brief Send unsubscription state reply
/// @param sessionId
/// @param sendMsg
void Server::_SendUnsubscribeFGStateResp(isoft::ipc::IPCSessionId const &sessionId,
                                         SubscribeFGStateRespMsg &sendMsg) const noexcept
{
    log_.LogInfo() << "Server::_SendUnsubscribeFGStateResp()";

    // Create message
    isoft::ipc::IPCPacket *const responsePacket{ipcServerPtr_->MakeResponse(sessionId)};
    if (nullptr == responsePacket) {
        log_.LogError() << "Server::_SendUnsubscribeFGStateResp(), failed to MakeResponse.";
        return;
    }
    std::size_t const buffSize{sendMsg.GetSize()};
    isoft::ipc::IPCPacketBufferReference *const resBuffer{responsePacket->AppendBuffer(buffSize)};
    sendMsg.Serialize(resBuffer->GetPtr());
    std::ignore = resBuffer->SetLen(buffSize);

    // Send message
    if (0 != ipcServerPtr_->Send(responsePacket, true)) {
        log_.LogError() << "Server::_SendUnsubscribeFGStateResp(), failed to Send with err"
                        << ara::core::internal::safe_strerror(errno);
        return;
    }
}
/// @brief Publish state
/// @param state
void Server::PublishFGState(FGStateInternalType const &state) noexcept
{
    FGStateNotifyMsg sendMsg{state};
    for (auto &subscriber : subscriberMap_) {
        // Create message
        isoft::ipc::IPCPacket *const responsePacket{ipcServerPtr_->MakeResponse(subscriber.second)};
        if (nullptr == responsePacket) {
            log_.LogWarn() << "Server::PublishFGState(), failed to MakeResponse.";
            continue;
        }
        std::size_t const buffSize{sendMsg.GetSize()};
        isoft::ipc::IPCPacketBufferReference *const resBuffer{responsePacket->AppendBuffer(buffSize)};
        sendMsg.Serialize(resBuffer->GetPtr());
        std::ignore = resBuffer->SetLen(buffSize);

        // Send message
        // TODO(longxiao.liang): Set timeout for sending messages
        if (0 != ipcServerPtr_->Send(responsePacket, false)) {
            log_.LogError() << "Server::PublishFGState(), failed to Send with err"
                            << ara::core::internal::safe_strerror(errno) << "for" << subscriber.first.c_str();
        } else {
            log_.LogInfo() << "Server::PublishFGState(), Succeed to Send to" << subscriber.first.c_str();
        }
    }
}
/// @brief State notification
/// @param instance
Server::Server(core::InstanceSpecifier const &instance) noexcept
    : log_{log::CreateLogger(
        (core::StringView{"#CMG"}), (core::StringView{"FGStateNotifyComm"}), log::LogLevel::kVerbose)}
    , ipcServerPtr_{nullptr}
    , subscriberMap_{}
    , evLoopPtr_{nullptr}
    , appendEventHandler_{nullptr}
{
    std::ignore = instance;
}

/// @brief Initialize
/// @return true - success
/// @return false - failure
bool Server::Init() noexcept
{
    log_.LogInfo() << "Server::Init()";
    evLoopPtr_ = GlobalGeneralEvLoop::Get();

    // Initialize nai unix socket
    int32_t const r{isoft::ipc::IPCInitNaiUDS(evLoopPtr_)};
    if (r == -1) {
        log_.LogError() << "Server::Init(), failed to IPCInitNaiUDS with errno:"
                        << ara::core::internal::safe_strerror(errno);
        return false;
    }

    // Create server
    ipcServerPtr_ = isoft::ipc::IPCServer::Create(::ara::sm::fg_state_ipc::Get_SM_IPC_SERVER_NAME());
    if (nullptr == ipcServerPtr_) {
        log_.LogError() << "Server::Init(), failed to Create with SM_IPC_SERVER_NAME:"
                        << ::ara::sm::fg_state_ipc::Get_SM_IPC_SERVER_NAME();
        return false;
    }

    ipcServerPtr_->SetHandler(IPC_SERVER_HANDLER_CONNECT, &SMsgHandler, this);
    ipcServerPtr_->SetHandler(IPC_SERVER_HANDLER_DISCONNECT, &SMsgHandler, this);
    ipcServerPtr_->SetHandler(IPC_SERVER_HANDLER_ONCE, &SMsgHandler, this);
    ipcServerPtr_->SetHandler(IPC_SERVER_HANDLER_MULTI, &SMsgHandler, this);

    if (0 != ipcServerPtr_->Start()) {
        log_.LogError() << "Server::Init(), failed to Start with errno:" << ara::core::internal::safe_strerror(errno);
        return false;
    }
    return true;
}

/// @brief Start accepting requests
/// @return true - success
/// @return false - failure
bool Server::Start() const noexcept
{
    log_.LogInfo() << "Server::Start(), begin.";
    int32_t const ret{ipcServerPtr_->Start()};
    if (ret < 0) {
        log_.LogError() << "Server::Start(), ipc server start error" << ara::core::internal::safe_strerror(errno);
        return false;
    }
    return true;
}

/// @brief Stop accepting requests
void Server::Stop() const noexcept
{
    int32_t const ret{ipcServerPtr_->Stop()};
    if (ret < 0) {
        log_.LogError() << "Server::Stop(), ipc server Stop error" << ara::core::internal::safe_strerror(errno);
    }
}

/// @brief Destroy
/// @return 0 - success
/// @return non-zero - failure
int32_t Server::Destroy() const noexcept
{
    log_.LogInfo() << "Server::Destroy()";
    if (-1 == isoft::ipc::IPCDeInitNaiUDS()) {
        int32_t const err{errno};
        log_.LogWarn() << "Server::Destroy(), failed to IPCDeInitNaiUDS with error:"
                       << ara::core::internal::safe_strerror(err);
        return err;
    }
    return 0;
}

/// @brief Register the callback function for appending events
/// @param appendEventHandler Callback function for appending events
void Server::RegisterAppendEventHandler(std::function< void(common::Event &&) > const &appendEventHandler) noexcept
{
    log_.LogInfo() << "Server::RegisterAppendEventHandler()";
    appendEventHandler_ = appendEventHandler;
}

/// @brief Asynchronously get the current states of all function groups
/// @param sessionId IPC sessionId
void Server::_GetAndSendAllFgStates(isoft::ipc::IPCSessionId const &sessionId) noexcept
{
    log_.LogInfo() << "Server::_GetAndSendAllFgStates(), sessionId:" << sessionId;
    if (appendEventHandler_) {
        core::Promise< core::String > promiseForEvent;
        std::shared_ptr< core::Future< core::String > > futureForEvent{
            std::make_shared< core::Future< core::String > >(promiseForEvent.get_future())};
        std::ignore = futureForEvent->then([this, futureForEvent, sessionId]() mutable {
            core::Result< core::String > const fgStatesRes{futureForEvent->GetResult()};
            if (fgStatesRes.HasValue()) {
                core::String const &fgStatesStr{fgStatesRes.Value()};
                log_.LogDebug() << "Server::_GetAndSendAllFgStates(), fgStatesStr:" << fgStatesStr.c_str()
                                << "sessionId:" << sessionId;
                core::Vector< FGStateInternalType > const fgStates{_Str2FGStateInternalVec(fgStatesStr)};

                // Create message
                _responseToGetAllFgStates(sessionId, fgStates);
            } else {
                ara::core::ErrorCode const errorCode{fgStatesRes.Error()};
                log_.LogError() << "Server::_GetAndSendAllFgStates(), error:" << errorCode.Message().data();
            }
            futureForEvent = nullptr;
        });
        common::Event request;
        request.type                  = common::EventType::kInGetAllFGState;
        request.requestPromiseWithStr = std::move(promiseForEvent);
        appendEventHandler_(std::move(request));
    } else {
        log_.LogError() << "Server::_GetAndSendAllFgStates(), appendEventHandler_ is nullptr";
        _responseToGetAllFgStates(sessionId, core::Vector< FGStateInternalType >{});
    }
}

/// @brief Convert a string in the format FG1.state;FG2.state to core::Vector<FGStateInternalType>
/// @param  str String format of function group states
/// @return List of function group information
core::Vector< FGStateInternalType > Server::_Str2FGStateInternalVec(core::String const &str) const noexcept
{
    core::Vector< FGStateInternalType > fgStateVec;
    core::String formatStr{str};
    core::String const fgSplit{common::GetkFgSplitStr()};
    core::String const stateSplit{common::GetkFgStateSplitStr()};
    size_t pos{ara::core::String::npos};
    if (!formatStr.empty()) {
        formatStr = formatStr + fgSplit;
    }
    do {
        pos = formatStr.find(fgSplit);
        core::String const fgStateStr{formatStr.substr(0U, pos)};
        if (!fgStateStr.empty()) {
            size_t const statePos{fgStateStr.find(stateSplit)};
            if (statePos != ara::core::String::npos) {
                core::String fgFQN{fgStateStr.substr(0U, statePos)};
                if (fgFQN.empty()) {
                    log_.LogWarn() << "Server::_Str2FGStateInternalVec(), fgFQN is empty";
                } else {
                    core::String fgState{fgStateStr.substr(statePos + stateSplit.size(), fgStateStr.size())};
                    if (fgState.empty()) {
                        log_.LogWarn() << "Server::_Str2FGStateInternalVec(), fgState is empty";
                    }
                    log_.LogDebug() << "Server::_Str2FGStateInternalVec(), get fgFQN:" << fgFQN.c_str()
                                    << "and fgState:" << fgState.c_str();
                    FGStateInternalType fg;
                    fg.fgName  = std::move(fgFQN);
                    fg.fgState = std::move(fgState);
                    fgStateVec.push_back(std::move(fg));
                }
            } else {
                log_.LogError() << "Server::_Str2FGStateInternalVec(), invalid fgStateStr:" << fgStateStr.c_str();
            }
        }
        // Remove the split string and split the remaining string
        formatStr = formatStr.substr(pos + fgSplit.size(), formatStr.size());
        pos       = formatStr.find(fgSplit);
    } while (pos != ara::core::String::npos);
    return fgStateVec;
}

/// @brief Response to get all function group states
/// @param sessionId Session ID
/// @param fgStates List of function group states
void Server::_responseToGetAllFgStates(isoft::ipc::IPCSessionId const &sessionId,
                                       core::Vector< FGStateInternalType > const &fgStates) const noexcept
{
    log_.LogInfo() << "Server::_responseToGetAllFgStates(), sessionId:" << sessionId;
    // Create message
    RequestAllFGStateRespMsg sendMsg{fgStates};
    isoft::ipc::IPCPacket *const responsePacket{ipcServerPtr_->MakeResponse(sessionId)};
    if (nullptr == responsePacket) {
        log_.LogError() << "Server::_responseToGetAllFgStates(), failed to MakeResponse";
        return;
    }
    std::size_t const buffSize{sendMsg.GetSize()};
    log_.LogDebug() << "Server::_responseToGetAllFgStates(), try to AppendBuffer with buffSize:" << buffSize;
    isoft::ipc::IPCPacketBufferReference *const resBuffer{responsePacket->AppendBuffer(buffSize)};
    sendMsg.Serialize(resBuffer->GetPtr());
    std::ignore = resBuffer->SetLen(buffSize);

    // Send message
    if (0 != ipcServerPtr_->Send(responsePacket, true)) {
        log_.LogError() << "Server::_responseToGetAllFgStates(), failed to Send with err:"
                        << ara::core::internal::safe_strerror(errno);
    }
}
}  // namespace fg_state_ipc
}  // namespace sm
}  // namespace ara
