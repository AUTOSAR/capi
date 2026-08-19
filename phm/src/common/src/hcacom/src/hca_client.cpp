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
/// @file       hca_client.cpp
/// @brief
/// @details
/// @date       2024-06-20
/// @author     wangyanlong
/// @version    1.2.0
///
/// ================================================================

#include "ara/phm/internal/com/hca_client.h"

#include <isoft/ipccpp/buffer.h>

#include <cerrno>
#include <cstring>

#include "ara/phm/internal/phm_log.h"
#include "ara/phm/internal/types.h"

namespace ara {
namespace phm {
namespace internal {
namespace hcacom {
/// @brief Send data.
/// @param notifyInfo data to send.
/// @return 0 success；other fail.
int32_t Client::Notify(std::shared_ptr< NotifyInfo > const& notifyInfo) noexcept
{
    Message const msg{notifyInfo->identifier, notifyInfo->status, TimeStamp::GetMs()};
    messageSerializer_.SetMsg(msg);
    Chunk const chunk{messageSerializer_.Serialize()};
    if (chunk.size() == static_cast< size_t >(0)) {
        LOG_ERROR << "serialize error.";
        return -1;
    }

    // begin: the order of these code blocks cannot be changed
    if (notifyInfo_.count(notifyInfo->identifier) > static_cast< size_t >(0)) {
        LOG_WARN << notifyInfo->identifier.c_str() << " is already in recovering.";
        return 0;
    }

    isoft::ipc::IPCPacket* const request{_MakeAndFillRequest(chunk)};
    if (request == nullptr) {
        LOG_ERROR << "_MakeAndFillRequest error.";
        return -1;
    }

    notifyInfo_[notifyInfo->identifier] = notifyInfo;
    // end: the order of these code blocks cannot be changed

    std::shared_ptr< isoft::ipc::IPCClient > ipcClient{GetIpcClient()};
    int32_t const ret{
        std::move(ipcClient->SendAsync(request, false,
                                       // TODO(wangyanlong): std:bind is not recommended in QAC
                                       std::bind(&Client::_responseCallBack, this, std::placeholders::_1,
                                                 std::placeholders::_2, std::placeholders::_3),
                                       notifyInfo->waitResponseTimeOutMs, static_cast< void* >(notifyInfo.get())))};
    if (0 != ret) {
        /// TODO(wangyanlong): clang-tidy suggests that the std::strerror function may throw an exception, QAC does not recommend using errno
        LOG_ERROR << "SendAsync error " << std::strerror(errno);  // NOLINT
        _processRecoveryResult(RecoveryResult::kFail, notifyInfo.get());
        return -1;
    }

    return 0;
}

/// @brief the async callback of ipc SendAsync.
/// @throws QAC
/// @param context context set by user.
/// @param status status of ipc SendAsync.
/// @param responsePacket the response packet of ipc SendAsync.
void Client::_responseCallBack(void* const context,
                               isoft::ipc::IPCClientHandlerStatus const status,
                               isoft::ipc::IPCPacket* const responsePacket)
{
    std::ignore = responsePacket;

    NotifyInfo* const notifyInfo{reinterpret_cast< NotifyInfo* >(context)};
    if (notifyInfo == nullptr) {
        LOG_ERROR << "context is null.";
        return;
    }

    if (status != IPC_CLIENT_HANDLER_STATUS_ERR) {
        isoft::ipc::IPCPacketBufferReference* const ipcBuffer{responsePacket->GetBuffer()};
        if (nullptr == ipcBuffer) {
            LOG_ERROR << "ipc buffer is null.";
            _processRecoveryResult(RecoveryResult::kSuccess, notifyInfo);
        } else {
            uint8_t* const buf{ipcBuffer->GetPtr()};
            _processRecoveryResult(static_cast< RecoveryResult >(*reinterpret_cast< uint32_t* >(buf)), notifyInfo);
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

    Message const msg{notifyInfo->identifier, notifyInfo->status, TimeStamp::GetMs()};
    messageSerializer_.SetMsg(msg);
    Chunk const chunk{messageSerializer_.Serialize()};
    if (chunk.size() == static_cast< size_t >(0)) {
        LOG_ERROR << "serialize error.";
        return;
    }

    isoft::ipc::IPCPacket* const request{_MakeAndFillRequest(chunk)};
    if (request == nullptr) {
        LOG_ERROR << "request is null.";
        return;
    }
    std::shared_ptr< isoft::ipc::IPCClient > ipcClient{GetIpcClient()};
    int32_t const ret{ipcClient->SendAsync(request, false,
                                           // TODO(wangyanlong): std:bind is not recommended in QAC
                                           std::bind(&Client::_responseCallBack, this, std::placeholders::_1,
                                                     std::placeholders::_2, std::placeholders::_3),
                                           notifyInfo->waitResponseTimeOutMs, context)};
    if (0 != ret) {
        LOG_ERROR << "send async failed.";
        _processRecoveryResult(RecoveryResult::kFail, notifyInfo);
    }
    return;
}

/// @brief Process the recovery result.
/// @param result result of recovery.
/// @param notifyInfo the notify info from user.
void Client::_processRecoveryResult(RecoveryResult const result, NotifyInfo const* const notifyInfo) noexcept
{
    if (recoveryResultHandler_) {
        recoveryResultHandler_(result, notifyInfo->identifier);
    } else {
        LOG_ERROR << "recovery result handler is null";
    }

    if (notifyInfo_.count(notifyInfo->identifier) > static_cast< size_t >(0)) {
        LOG_DEBUG << "destroy notify info of " << notifyInfo->identifier.c_str();
        std::ignore = notifyInfo_.erase(notifyInfo->identifier);
    }
}

}  // namespace hcacom
}  // namespace internal
}  // namespace phm
}  // namespace ara
