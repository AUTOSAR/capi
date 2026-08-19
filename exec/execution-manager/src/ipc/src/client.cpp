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
/// @brief      Definition of IPC connection management Client class
/// @details
/// @date       2024-04-01
/// @author     xueliang.bao
/// @version    1.2.0
///
/// ================================================================

#include "ara/exec/internal/ipc/client.h"

#include <bits/stdint-intn.h>

#include <cstddef>
#include <cstring>

#include "ara/exec/internal/log/log.h"
#include "isoft/ipccpp/buffer.h"
#include "isoft/ipccpp/utility.h"
#include "isoft/naicpp/global_evloop.h"
#include "isoft/utils/error.h"

namespace ara {
namespace exec {
namespace internal {
namespace ipc {

/// @brief Open IPC client
/// @param spMainLoop Main event loop handle
/// @param serviceProvider Service provider name
/// @param serviceName Service name to connect to
/// @return 0 success; <0 failure
int32_t Client::Open(std::shared_ptr< isoft::naicpp::EvLoop > spMainLoop,
                     ara::core::StringView const &serviceProvider,
                     ara::core::StringView const &serviceName) noexcept
{
    if (spIpcClient_) {
        LOGE() << "ipc::Client::Open(): Already opened !!!";
        return -1;
    }

    if (spMainLoop == nullptr) {
        std::ignore
            = isoft::naicpp::GlobalGeneralEvLoop::Initialize(isoft::naicpp::GlobalGeneralEvLoop::Mode::kInsideThread);
        spMainLoop_ = isoft::naicpp::GlobalGeneralEvLoop::Get();
    } else {
        spMainLoop_ = std::move(spMainLoop);
    }

    if (spMainLoop_ == nullptr) {
        LOGE() << "ipc::Client::Open(): nullptr == spMainLoop_";
        return -1;
    }

    if (isoft::ipc::IPCInitNaiUDS(spMainLoop_) < 0) {
        LOGE() << "ipc::Client::Open(): IPCInitNaiUDS() failed !!!";
        return -1;
    }

    ara::core::String ipcAddr{serviceProvider};
    ipcAddr      = ipcAddr + "/" + ara::core::String(serviceName);
    spIpcClient_ = isoft::ipc::IPCClient::Create(ipcAddr.c_str());
    if (spIpcClient_ == nullptr) {
        LOGE() << "ipc::Client::Open(): Create ipc client {" << ipcAddr << "} failed !!!";
        return -1;
    }

    if (0 != spIpcClient_->Start()) {
        LOGE() << "ipc::Client::Open(): Start ipc client {" << ipcAddr << "} failed !!!";
        return -1;
    }

    return 0;
}

/// @brief Close IPC client
/// @return 0 success; <0 failure
int32_t Client::Close() noexcept
{
    if (spIpcClient_ == nullptr) {
        LOGE() << "ipc::Client::Close(): nullptr == spIpcClient_";
        return -1;
    }

    int32_t ret{0};
    while (true) {
        if (0 != spIpcClient_->Stop()) {
            LOGE() << "ipc::Client::Close(): Stop failed !!!";
            ret = -1;
            break;
        }

        spIpcClient_->Release();

        if (-1 == isoft::ipc::IPCDeInitNaiUDS()) {
            LOGE() << "ipc::Client::Close(): IPCDeInitNaiUDS() failed !!!";
            ret = -1;
            break;
        }

        break;
    }

    if (0 == ret) {
        spIpcClient_.reset();
    }
    spMainLoop_.reset();
    return ret;
}

/// @brief Push message, no response required
/// @param pMsg Message to send
/// @param msgSize Message length
/// @return 0 success; <0 failure
int32_t Client::Post(void const *const pMsg, uint32_t const msgSize) noexcept
{
    if (spIpcClient_ == nullptr) {
        LOGE() << "ipc::Client::Post(): nullptr == spIpcClient_ !!!";
        return -1;
    }

 /// No need to release reqBuffer
    isoft::ipc::IPCPacket *const reqPacket{spIpcClient_->MakeRequest()};
    if (reqPacket == nullptr) {
        LOGE() << "ipc::Client::Post(): nullptr == reqPacket !!!";
        return -1;
    }

    isoft::ipc::IPCPacketBufferReference *const reqBuffer{reqPacket->AppendBuffer(static_cast< size_t >(msgSize))};
    if (reqBuffer == nullptr) {
        LOGE() << "ipc::Client::Post(): nullptr == reqBuffer !!!";
        return -1;
    }

    if (reqBuffer->GetPtr() == nullptr) {
        LOGE() << "ipc::Client::Post(): nullptr == reqBuffer->GetPtr() !!!";
        return -1;
    }

    std::ignore = reqBuffer->SetLen(static_cast< size_t >(msgSize));
    std::ignore = memcpy(reqBuffer->GetPtr(), pMsg, static_cast< size_t >(msgSize));

    if (spIpcClient_->Post(reqPacket) != 0) {
        LOGE() << "ipc::Client::Post() failed !!!";
        return -1;
    }

    return 0;
}

/// @brief Send asynchronous message
/// @param pMsg Message to send
/// @param msgSize Message size
/// @param multiReply Whether multiple replies are required
/// @param timeout Timeout duration
/// @return 0 success; <0 failure
int32_t Client::_SendMessage(void const *const pMsg,
                             uint64_t const msgSize,
                             bool const multiReply,
                             int32_t const &timeout) noexcept
{
    if (spIpcClient_ == nullptr) {
        LOGE() << "ipc::Client::SendMessage(): nullptr == spIpcClient_ !!!";
        return -1;
    }

 /// No need to release reqBuffer
    isoft::ipc::IPCPacket *const reqPacket{spIpcClient_->MakeRequest()};
    if (reqPacket == nullptr) {
        LOGE() << "ipc::Client::SendMessage(): nullptr == reqPacket !!!";
        return -1;
    }

    isoft::ipc::IPCPacketBufferReference *const reqBuffer{reqPacket->AppendBuffer(static_cast< size_t >(msgSize))};
    if (reqBuffer == nullptr) {
        LOGE() << "ipc::Client::SendMessage(): nullptr == reqBuffer !!!";
        return -1;
    }

    if (nullptr == reqBuffer->GetPtr()) {
        LOGE() << "ipc::Client::SendMessage(): nullptr == reqBuffer->GetPtr() !!!";
        return -1;
    }

    std::ignore = reqBuffer->SetLen(msgSize);
    std::ignore = memcpy(reqBuffer->GetPtr(), pMsg, static_cast< size_t >(msgSize));

    int32_t const r{spIpcClient_->SendAsync(reqPacket, multiReply, &HandleResponse, timeout, this)};

    if (r != 0) {
        LOGE() << "ipc::Client::SendMessage() error:" << isoft::utils::StrError();
        return -1;
    }

    return 0;
}

/// @brief Process response message
/// @param pCtx Context information
/// @param status Connection status
/// @param pRspPacket Response message
/// @exception std::runtime_error If processing fails
void Client::HandleResponse(void *const pCtx,
                            isoft::ipc::IPCClientHandlerStatus const status,
                            isoft::ipc::IPCPacket *const pRspPacket)
{
    if (pCtx == nullptr) {
        LOGE() << "ipc::Client::HandleResponse(): nullptr == pCtx";
        return;
    }

    void *pMsg{nullptr};
    uint32_t msgSize{0U};
 int32_t ret{0}; ///< Flag indicating whether IPC connection has an error

    while (true) {
        if (status == IPC_CLIENT_HANDLER_STATUS_ERR) {
            LOGE() << "ipc::Client::HandleResponse(): IPC_CLIENT_HANDLER_STATUS_ERR == status";
            ret = -1;
            break;
        }

        if (pRspPacket == nullptr) {
            LOGE() << "ipc::Client::HandleResponse(): nullptr == pRspPacket";
            break;
        }

        isoft::ipc::IPCPacketBufferReference *const buffer{pRspPacket->GetBuffer()};
        if (buffer == nullptr) {
            LOGE() << "ipc::Client::HandleResponse(): nullptr == buffer";
            break;
        }

        if (buffer->GetPtr() == nullptr) {
            LOGE() << "ipc::Client::HandleResponse(): nullptr == buffer->GetPtr()";
            break;
        }

        pMsg    = buffer->GetPtr();
        msgSize = static_cast< uint32_t >(buffer->GetLen());

        break;
    }

    Client *const pClient{static_cast< Client * >(pCtx)};
    if (pClient->fResponseHandler_) {
        pClient->fResponseHandler_(pMsg, msgSize, ret);
    }

    return;
}

}  // namespace ipc
}  // namespace internal
}  // namespace exec
}  // namespace ara
