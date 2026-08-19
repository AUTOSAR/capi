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
/// @brief      Implementation of function group state information asynchronous notification client
/// @details
/// @date       2022-06-13
/// @author     hanzhibo
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/StateManagement/Utils
/// @interface_level=module
/// @trace_id_sr=SR_SM_02001, SR_SM_02002
/// @unit_name=Client
/// @unit_description=Implementation of function group state information asynchronous notification client
/// @endcode
///
/// ================================================================

#include <ara/log/logger.h>
#include <isoft/ipc/client.h>
#include <isoft/naicpp/evloop.h>
#include <isoft/naicpp/global_evloop.h>

#include "client.h"
#include "common.h"
#include "function_thread_safe.h"
#include "msg_type.h"
/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
using ara::core::InstanceSpecifier;
/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
using isoft::naicpp::GlobalGeneralEvLoop;

/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
#define FreeIpcMsg(msg)                                                                                                \
    if (nullptr != (msg)) {                                                                                            \
        ::isoft::ipc::DeleteMessage((msg));                                                                            \
        (msg) = nullptr;                                                                                               \
    }

namespace ara {
namespace sm {
namespace fg_state_ipc {

/// @brief Initialize
/// @param clientID
/// @return
FGStateNotifyResult Client::Init(core::String const &clientID) noexcept
{
    logger_.LogInfo() << "Client::Init(), clientID:" << clientID;
    if (bInit_) {
        return FGStateNotifyResult::kAlreadyInited;
    }

    evLoopPtr_ = GlobalGeneralEvLoop::Get();
    if (nullptr == evLoopPtr_) {
        logger_.LogError() << "Client::Init(), Get EvLoop err:" << ara::core::internal::safe_strerror(errno);
        return FGStateNotifyResult::kCanNotGetEVLoop;
    }

    // Initialize nai socket
    int32_t const r{isoft::ipc::IPCInitNaiUDS(evLoopPtr_)};
    if (r == -1) {
        logger_.LogError() << "Client::Init(), fail to IPCInitNaiUDS.";
        return FGStateNotifyResult::kCanNotInitNai;
    }

    core::String serviceAddress{::ara::sm::fg_state_ipc::Get_SM_APP_NAME()};
    serviceAddress += "/";
    serviceAddress += ara::sm::fg_state_ipc::Get_SM_IPC_SERVER_NAME();
    clientPtr_ = ::isoft::ipc::IPCClient::Create(serviceAddress.c_str());
    if (nullptr == clientPtr_) {
        logger_.LogError() << "Client::Init(), ipc client init err:" << ara::core::internal::safe_strerror(errno);
        return FGStateNotifyResult::kCanNotCreateIPCClient;
    }

    /// Start() will send a connection to the Server side, then trigger the Server's connectHandler
    if (0 != clientPtr_->Start()) {
        logger_.LogError() << "Client::Init(), failed to Start().";
        return FGStateNotifyResult::kCanNotStartIPCClient;
    }

    bInit_    = true;
    clientId_ = clientID;
    return FGStateNotifyResult::kSuccess;
}

/// @brief Deinitialize
void Client::Destroy() const noexcept
{
    logger_.LogInfo() << "Client::Destroy(), bInit_:" << bInit_;
    if (bInit_) {
        std::ignore = clientPtr_->Stop();
        clientPtr_->Release();
        std::ignore = isoft::ipc::IPCDeInitNaiUDS();
    }
}

/// @brief Asynchronously get all function group state information
/// @param reqRespMsgHandler
/// @param timeout
/// @return
FGStateNotifyResult Client::RequestAllFGState(
    std::function< RequestRespMsgHandlerInternalType > const &reqRespMsgHandler, int32_t const &timeout) noexcept
{
    logger_.LogInfo() << "Client::RequestAllFGState(), timeout:" << timeout;
    if (!bInit_) {
        return FGStateNotifyResult::kNotInited;
    }

    // Create message
    isoft::ipc::IPCPacket *const request{clientPtr_->MakeRequest()};
    if (nullptr == request) {
        logger_.LogError() << "Client::RequestAllFGState(), failed to MakeRequest.";
        return FGStateNotifyResult::kCanNotCreateMSG;
    }

    // Message
    FGStateMsg sendReq{MsgType::kRequestAllFGState};

    // Calculate the message size, then append to buffer
    std::size_t const buffSize{sendReq.GetSize()};
    isoft::ipc::IPCPacketBufferReference *const reqBuffer{request->AppendBuffer(buffSize)};
    if (nullptr == reqBuffer) {
        logger_.LogError() << "Client::RequestAllFGState(), failed to AppendBuffer with buffSize:" << buffSize;
        return FGStateNotifyResult::kCanNotAppendBuffer;
    }

    // Serialize the message
    sendReq.Serialize(reqBuffer->GetPtr());

    // Set the size of the buffer
    std::ignore = reqBuffer->SetLen(buffSize);

    // Save the callback function
    reqRespMsgHandler_ = reqRespMsgHandler;

    // Asynchronous request
    /// false means only need to reply once, 3000 means a timeout of 3 seconds
    logger_.LogInfo() << "Client::RequestAllFGState(), try to call SendAsync with timeout" << timeout;
    std::function< void(void *const, isoft::ipc::IPCClientHandlerStatus const &, isoft::ipc::IPCPacket *const) > const
        handlerAfterRequestingAllFGStateWrap{[this](void *const context,
                                                    isoft::ipc::IPCClientHandlerStatus const &status,
                                                    isoft::ipc::IPCPacket *const responsePacket) noexcept {
            _handlerAfterRequestingAllFGState(context, status, responsePacket);
        }};
    int32_t const ret{clientPtr_->SendAsync(request, false, handlerAfterRequestingAllFGStateWrap, timeout, this)};
    if (ret < 0) {
        reqRespMsgHandler_ = nullptr;
        logger_.LogError() << "Client::RequestAllFGState(), SendAsync err" << ara::core::internal::safe_strerror(errno);

        if (ECONNREFUSED == errno) {
            return FGStateNotifyResult::kConnectionRefused;
        }
        if (ETIMEDOUT == errno) {
            return FGStateNotifyResult::kTimeout;
        }
        return FGStateNotifyResult::kOthers;
    }

    return FGStateNotifyResult::kSuccess;
}

/// @brief Process all states after the request
/// @param context
/// @param status
/// @param responsePacket
/// @exception
void Client::_handlerAfterRequestingAllFGState([[maybe_unused]] void *const context,
                                               isoft::ipc::IPCClientHandlerStatus const &status,
                                               isoft::ipc::IPCPacket *const responsePacket) const noexcept
{
    logger_.LogInfo() << "Client::_handlerAfterRequestingAllFGState()";
    // Return result
    FGStateNotifyResult res{FGStateNotifyResult::kSuccess};
    if (status == IPC_CLIENT_HANDLER_STATUS_ERR) {
        if (ENOENT == errno) {
            res = FGStateNotifyResult::kCanNotFindServer;
        } else if (ECONNREFUSED == errno) {
            res = FGStateNotifyResult::kConnectionRefused;
        } else if (ETIMEDOUT == errno) {
            res = FGStateNotifyResult::kTimeout;
        } else {
            res = FGStateNotifyResult::kOthers;
        }

        core::Vector< FGStateInternalType > const fGStateVec;
        reqRespMsgHandler_(res, fGStateVec);
    } else {
        isoft::ipc::IPCPacketBufferReference *buffer{responsePacket->GetBuffer()};

        // Message address and size
        uint8_t *pMsg{buffer->GetPtr()};

        size_t msgSize{buffer->GetLen()};

        // Multiple buffers
        isoft::ipc::IPCPacketBufferReference *nextBuffer{responsePacket->GetNextBuffer(buffer)};
        if (nextBuffer != nullptr) {  // There are multiple buffers
            // Accumulate to get the message size
            while (nullptr != nextBuffer) {
                msgSize += nextBuffer->GetLen();
                nextBuffer = responsePacket->GetNextBuffer(nextBuffer);
            }

            // Allocate memory of the message size
            pMsg = static_cast< uint8_t * >(malloc(msgSize));
            if (pMsg != nullptr) {  // Allocation successful
                // Move the message to pMsg
                buffer = responsePacket->GetBuffer();
                size_t buffindex{0U};
                while (nullptr != buffer) {
                    size_t const bufferLen{buffer->GetLen()};
                    std::ignore = memcpy(pMsg + buffindex, buffer->GetPtr(), bufferLen);
                    buffindex += bufferLen;
                    buffer = responsePacket->GetNextBuffer(buffer);
                }

                // Deserialize
                RequestAllFGStateRespMsg receiveMsg;
                receiveMsg.Deserialize(pMsg, msgSize);
                reqRespMsgHandler_(res, receiveMsg.GetFGStateVec());

                // Free memory
                free(pMsg);
            } else {  // Allocation failed
                res = FGStateNotifyResult::kCanNotMalloc;
                core::Vector< FGStateInternalType > const fGStateVec;
                reqRespMsgHandler_(res, fGStateVec);
            }
        } else {  // A single buffer
            // Deserialize
            RequestAllFGStateRespMsg receiveMsg;
            receiveMsg.Deserialize(pMsg, msgSize);
            reqRespMsgHandler_(res, receiveMsg.GetFGStateVec());
        }
    }
}

/// @brief Process the state after subscription
/// @param context
/// @param status
/// @param responsePacket
void Client::_handlerAfterSubscribingFGState([[maybe_unused]] void *const context,
                                             isoft::ipc::IPCClientHandlerStatus const &status,
                                             isoft::ipc::IPCPacket *const responsePacket) noexcept
{
    logger_.LogInfo() << "Client::_handlerAfterSubscribingFGState()";
    // Return result
    FGStateNotifyResult res{FGStateNotifyResult::kSuccess};
    if (status == IPC_CLIENT_HANDLER_STATUS_ERR) {
        if (ENOENT == errno) {
            res = FGStateNotifyResult::kCanNotFindServer;
        } else if (ECONNREFUSED == errno) {
            res = FGStateNotifyResult::kConnectionRefused;
        } else if (ETIMEDOUT == errno) {
            res = FGStateNotifyResult::kTimeout;
        } else {
            res = FGStateNotifyResult::kOthers;
        }

        subscribeRespMsgHandler_(res);

        subScribed_              = false;
        subscribeRespMsgHandler_ = nullptr;
        fGStateHandler_          = nullptr;
    } else {
        isoft::ipc::IPCPacketBufferReference *const buffer{responsePacket->GetBuffer()};
        uint8_t *const response{buffer->GetPtr()};
        size_t const responseSize{buffer->GetLen()};

        if (response[0] == static_cast< uint8_t >(MsgType::kSubscribeFGStateResp)) {
            SubscribeFGStateRespMsg receiveMsg;
            receiveMsg.Deserialize(response, responseSize);
            assert(receiveMsg.GetErrorCode() == 0U);

            subscribeRespMsgHandler_(res);

        } else if (response[0] == static_cast< uint8_t >(MsgType::kFGStateNotify)) {
            FGStateNotifyMsg receiveMsg;
            const bool fgStateHandlerValid{fGStateHandler_ != nullptr};
            receiveMsg.Deserialize(response, responseSize);
            if (subScribed_ && fgStateHandlerValid) {
                fGStateHandler_(receiveMsg.GetFgState());
            }
        } else {
            assert(false);
        }
    }
}

/// @brief Subscribe to state
/// @param subscribeRespMsgHandler
/// @param fGStateChangeHandler
/// @param timeout
FGStateNotifyResult Client::SubscribeFGState(
    std::function< SubscribeRespMsgHandlerInternalType > const &subscribeRespMsgHandler,
    std::function< void(FGStateInternalType const &fgState) > const &fGStateChangeHandler,
    int32_t const &timeout) noexcept
{
    logger_.LogInfo() << "Client::SubscribeFGState(), timeout:" << timeout;
    if (!bInit_) {
        return FGStateNotifyResult::kNotInited;
    }

    if (subScribed_) {
        return FGStateNotifyResult::kAlreadySubscribed;
    }

    // Create message
    isoft::ipc::IPCPacket *const request{clientPtr_->MakeRequest()};
    if (nullptr == request) {
        logger_.LogError() << "Client::SubscribeFGState, failed to MakeRequest.";
        return FGStateNotifyResult::kCanNotCreateMSG;
    }

    // Subscription message
    SubscribeFGStateMsg sendReq{clientId_};

    // Calculate the message size, then append to buffer
    std::size_t const buffSize{sendReq.GetSize()};
    isoft::ipc::IPCPacketBufferReference *const reqBuffer{request->AppendBuffer(buffSize)};
    if (nullptr == reqBuffer) {
        logger_.LogError() << "Client::SubscribeFGState(), failed to AppendBuffer with buffSize:" << buffSize;
        return FGStateNotifyResult::kCanNotAppendBuffer;
    }

    // Serialize the message
    sendReq.Serialize(reqBuffer->GetPtr());

    // Set the size of the buffer
    std::ignore = reqBuffer->SetLen(buffSize);

    // Save the callback function
    subscribeRespMsgHandler_ = subscribeRespMsgHandler;
    fGStateHandler_          = fGStateChangeHandler;

    // Asynchronous request
    logger_.LogInfo() << "Client::SubscribeFGState(), try to call SendAsync with timeout" << timeout;
    std::function< void(void *const, isoft::ipc::IPCClientHandlerStatus const &, isoft::ipc::IPCPacket *const) > const
        handlerAfterSubscribingFGStateWrap{[this](void *const context, isoft::ipc::IPCClientHandlerStatus const &status,
                                                  isoft::ipc::IPCPacket *const responsePacket) noexcept {
            _handlerAfterSubscribingFGState(context, status, responsePacket);
        }};
    int32_t const ret{clientPtr_->SendAsync(request, true, handlerAfterSubscribingFGStateWrap, timeout, this)};
    if (ret < 0) {
        subscribeRespMsgHandler_ = nullptr;
        fGStateHandler_          = nullptr;
        logger_.LogError() << "Client::SubscribeFGState(), SendAsync err" << ara::core::internal::safe_strerror(errno);

        if (ECONNREFUSED == errno) {
            return FGStateNotifyResult::kConnectionRefused;
        }
        if (ETIMEDOUT == errno) {
            return FGStateNotifyResult::kTimeout;
        }
        return FGStateNotifyResult::kOthers;
    }

    // Subscribed
    subScribed_ = true;
    return FGStateNotifyResult::kSuccess;
}

/// @brief Unsubscribe
/// @param unsubscribeRespMsgHandler
/// @param timeout
/// @return Result
FGStateNotifyResult Client::UnsubscribeFGState(
    std::function< UnsubscribeRespMsgHandlerInternalType > const &unsubscribeRespMsgHandler,
    int32_t const &timeout) noexcept
{
    logger_.LogInfo() << "Client::UnsubscribeFGState()";
    if (!bInit_) {
        return FGStateNotifyResult::kNotInited;
    }

    if (!subScribed_) {
        return FGStateNotifyResult::kNotAlreadySubscribed;
    }

    // Create message
    isoft::ipc::IPCPacket *const request{clientPtr_->MakeRequest()};
    if (nullptr == request) {
        logger_.LogError() << "Client::UnsubscribeFGState(), failed to MakeRequest.";
        return FGStateNotifyResult::kCanNotCreateMSG;
    }

    // Unsubscription message
    SubscribeFGStateMsg sendReq{clientId_, MsgType::kUnsubscribeFGState};

    // Calculate the message size, then append to buffer
    std::size_t const buffSize{sendReq.GetSize()};
    isoft::ipc::IPCPacketBufferReference *reqBuffer{request->AppendBuffer(buffSize)};
    if (nullptr == reqBuffer) {
        logger_.LogError() << "Client::UnsubscribeFGState(), failed to AppendBuffer with buffSize:" << buffSize;
        return FGStateNotifyResult::kCanNotAppendBuffer;
    }

    // Serialize the message
    sendReq.Serialize(reqBuffer->GetPtr());

    // Set the size of the buffer
    std::ignore = reqBuffer->SetLen(buffSize);

    // Save the callback function
    unsubscribeRespMsgHandler_ = unsubscribeRespMsgHandler;

    // Asynchronous request
    /// false means only need to reply once, 3000 means a timeout of 3 seconds
    logger_.LogInfo() << "Client::UnsubscribeFGState(), try to call SendAsync with timeout" << timeout;
    std::function< void(void *const, isoft::ipc::IPCClientHandlerStatus const &, isoft::ipc::IPCPacket *const) > const
        handlerAfterUnsubscribingFGStateWrap{[this](void *const context,
                                                    isoft::ipc::IPCClientHandlerStatus const &status,
                                                    isoft::ipc::IPCPacket *const responsePacket) noexcept {
            _handlerAfterUnsubscribingFGState(context, status, responsePacket);
        }};
    int32_t const ret{clientPtr_->SendAsync(request, false, handlerAfterUnsubscribingFGStateWrap, timeout, this)};
    if (ret < 0) {
        unsubscribeRespMsgHandler_ = nullptr;
        logger_.LogError() << "Client::UnsubscribeFGState(), SendAsync err"
                           << ara::core::internal::safe_strerror(errno);

        if (ECONNREFUSED == errno) {
            return FGStateNotifyResult::kConnectionRefused;
        }
        if (ETIMEDOUT == errno) {
            return FGStateNotifyResult::kTimeout;
        }
        return FGStateNotifyResult::kOthers;
    }

    return FGStateNotifyResult::kSuccess;
}

/// @brief Process the state after unsubscription
/// @param context
/// @param status
/// @param responsePacket
void Client::_handlerAfterUnsubscribingFGState([[maybe_unused]] void *const context,
                                               isoft::ipc::IPCClientHandlerStatus const &status,
                                               isoft::ipc::IPCPacket *const responsePacket) noexcept
{
    logger_.LogInfo() << "Client::_handlerAfterUnsubscribingFGState()";
    // Return result
    FGStateNotifyResult res{FGStateNotifyResult::kSuccess};
    if (status == IPC_CLIENT_HANDLER_STATUS_ERR) {
        if (ENOENT == errno) {
            res = FGStateNotifyResult::kCanNotFindServer;
        } else if (ECONNREFUSED == errno) {
            res = FGStateNotifyResult::kConnectionRefused;
        } else if (ETIMEDOUT == errno) {
            res = FGStateNotifyResult::kTimeout;
        } else {
            res = FGStateNotifyResult::kOthers;
        }

        unsubscribeRespMsgHandler_(res);
    } else {
        isoft::ipc::IPCPacketBufferReference *const buffer{responsePacket->GetBuffer()};
        uint8_t *const response{buffer->GetPtr()};
        size_t const responseSize{buffer->GetLen()};

        assert(response[0] == static_cast< uint8_t >(MsgType::kUnsubscribeFGStateResp));
        SubscribeFGStateRespMsg receiveMsg;
        receiveMsg.Deserialize(response, responseSize);
        assert(0U == receiveMsg.GetErrorCode());
        unsubscribeRespMsgHandler_(res);

        unsubscribeRespMsgHandler_ = nullptr;

        // Unsubscribe
        subScribed_     = false;
        fGStateHandler_ = nullptr;
    }
}

}  // namespace fg_state_ipc
}  // namespace sm
}  // namespace ara
