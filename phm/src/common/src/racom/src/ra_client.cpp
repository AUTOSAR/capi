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
/// @file       ra_client.cpp
/// @brief
/// @details
/// @date       2024-06-20
/// @author     wangyanlong
/// @version    1.2.0
///
/// ================================================================

#include "ara/phm/internal/com/ra_client.h"

#include <isoft/ipccpp/buffer.h>

#include <cerrno>
#include <cstring>

#include "ara/phm/internal/phm_log.h"

namespace ara {
namespace phm {
namespace internal {
namespace racom {
/// @brief Send data.
/// @param notifyInfo data to send
/// @return 0 success; other fail.
int32_t Client::Notify(std::shared_ptr< NotifyInfo > const &notifyInfo) noexcept
{
    Message const msg{notifyInfo->identifier, notifyInfo->fgName, notifyInfo->executionError,
                      static_cast< uint32_t >(notifyInfo->baseSupervisionType), TimeStamp::GetMs()};
    messageSerializer_.SetMsg(msg);
    Chunk const chunk{messageSerializer_.Serialize()};
    if (chunk.size() == static_cast< size_t >(0)) {
        LOG_ERROR << "serialize error.";
        return -1;
    }

    // Recovery in progress, do not send repeatedly
    if (notifyInfo_.count(notifyInfo->identifier) > static_cast< size_t >(0)) {
        LOG_WARN << notifyInfo->identifier << " is already in recovering, so no need to notify SM repeatedly.";
        return 0;
    }

    isoft::ipc::IPCPacket *const request{_MakeAndFillRequest(chunk)};
    if (request == nullptr) {
        LOG_ERROR << "_MakeAndFillRequest error.";
        return -1;
    }

    notifyInfo_[notifyInfo->identifier] = notifyInfo;
    std::shared_ptr< isoft::ipc::IPCClient > ipcClient{GetIpcClient()};
    int32_t const ret{ipcClient->SendAsync(request, false,
                                           // TODO(wangyanlong): std:bind is not recommended in QAC
                                           std::bind(&Client::_responseCallBack, this, std::placeholders::_1,
                                                     std::placeholders::_2, std::placeholders::_3),
                                           notifyInfo->waitResponseTimeOutMs, notifyInfo.get())};
    if (0 != ret) {
        /// TODO(wangyanlong): clang-tidy suggests that the std::strerror function may throw an exception, QAC does not recommend using errno
        LOG_ERROR << "SendAsync error " << std::strerror(errno);  // NOLINT
        _processRecoveryResult(RecoveryResult::kFail, notifyInfo.get());
        return -1;
    }

    return 0;
}

/// @brief the async callback of ipc SendAsync.
///@details TODO _responseCallBack Does it need a response from the other party? Yes, when receiving the other party's reply, the status is IPC_CLIENT_HANDLER_STATUS_RECIEVED
///@details enum ipc_client_handler_status_s {
///@details     IPC_CLIENT_HANDLER_STATUS_ERR = 1,  // Status reception error, and errno is set
///@details     IPC_CLIENT_HANDLER_STATUS_RECIEVED, // Received a response from the peer
///@details     IPC_CLIENT_HANDLER_STATUS_RECIEVED_CONTINUE, // Multiple responses
///@details     IPC_CLIENT_HANDLER_STATUS_RECIEVED_COMPLETED = IPC_CLIENT_HANDLER_STATUS_RECIEVED, // Multiple responses completed
///@details };
///@details Post interface has no return
/// @throws QAC
/// @param context context set by user.
/// @param status status of ipc SendAsync.
/// @param responsePacket the response packet of ipc SendAsync.
void Client::_responseCallBack(void *const context,
                               isoft::ipc::IPCClientHandlerStatus const status,
                               isoft::ipc::IPCPacket *const responsePacket)
{
    std::ignore = responsePacket;

    NotifyInfo *const notifyInfo{static_cast< NotifyInfo * >(context)};
    if (notifyInfo == nullptr) {
        LOG_ERROR << "context is null.";
        return;
    }

    if (status != IPC_CLIENT_HANDLER_STATUS_ERR) {
        isoft::ipc::IPCPacketBufferReference *const ipcBuffer{responsePacket->GetBuffer()};
        if (nullptr == ipcBuffer) {
            LOG_ERROR << "ipc buffer is null.";
            _processRecoveryResult(RecoveryResult::kSuccess, notifyInfo);
        } else {
            uint8_t *const buf{ipcBuffer->GetPtr()};
            _processRecoveryResult(static_cast< RecoveryResult >(*reinterpret_cast< uint32_t * >(buf)), notifyInfo);
        }

        return;
    }

    LOG_INFO << "recovery of " << notifyInfo->identifier.c_str() << " failed, retried times "
             << notifyInfo->haveRetriedTimes << ", max " << notifyInfo->maxRetryTimes;
    notifyInfo->haveRetriedTimes++;
    if (notifyInfo->haveRetriedTimes > notifyInfo->maxRetryTimes) {
        _processRecoveryResult(RecoveryResult::kFail, notifyInfo);
        return;
    }

    Message const msg{notifyInfo->identifier, notifyInfo->fgName, notifyInfo->executionError,
                      static_cast< uint32_t >(notifyInfo->baseSupervisionType), TimeStamp::GetMs()};
    messageSerializer_.SetMsg(msg);
    Chunk const chunk{messageSerializer_.Serialize()};
    if (chunk.size() == static_cast< size_t >(0)) {
        LOG_ERROR << "serialize error.";
        return;
    }

    isoft::ipc::IPCPacket *const request{_MakeAndFillRequest(chunk)};
    if (request == nullptr) {
        LOG_ERROR << "request is null.";
        return;
    }
    std::shared_ptr< isoft::ipc::IPCClient > ipcClient{GetIpcClient()};
    int32_t const ret{ipcClient->SendAsync(request, false,
                                           // TODO(wangyanlong): std:bind is not recommended in QAC
                                           std::bind(&Client::_responseCallBack, this, std::placeholders::_1,
                                                     std::placeholders::_2, std::placeholders::_3),
                                           notifyInfo->waitResponseTimeOutMs, static_cast< void * >(notifyInfo))};
    if (0 != ret) {
        LOG_ERROR << "send async failed.";
        _processRecoveryResult(RecoveryResult::kFail, notifyInfo);
    }
    return;
}

/// @brief Process the recovery result.
/// @param result recovery result.
/// @param notifyInfo notify info.
void Client::_processRecoveryResult(RecoveryResult const result, NotifyInfo const *const notifyInfo) noexcept
{
    if (recoveryResultHandler_) {
        recoveryResultHandler_(result, notifyInfo->identifier);
    } else {
        LOG_ERROR << "recoveryResultHandler_ is null.";
    }

    if (notifyInfo_.count(notifyInfo->identifier) > static_cast< size_t >(0)) {
        LOG_DEBUG << "destroy notify info of " << notifyInfo->identifier.c_str();
        std::ignore = notifyInfo_.erase(notifyInfo->identifier);
    }
}

}  // namespace racom
}  // namespace internal
}  // namespace phm
}  // namespace ara
