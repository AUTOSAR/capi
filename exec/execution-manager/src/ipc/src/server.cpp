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
/// @brief      Definition of IPC connection management Server class
/// @details
/// @date       2024-04-01
/// @author     xueliang.bao
/// @version    1.2.0
///
/// ================================================================

#include "ara/exec/internal/ipc/server.h"

#include <cstddef>

#include "ara/exec/internal/log/log.h"
#include "isoft/ipccpp/buffer.h"
#include "isoft/ipccpp/utility.h"
#include "isoft/naicpp/global_evloop.h"

namespace ara {
namespace exec {
namespace internal {
namespace ipc {

/// @brief Open IPC server
/// @param spMainLoop Main event loop handle
/// @param serviceName Service name provided
/// @return 0 success; <0 failure
int32_t Server::Open(std::shared_ptr< isoft::naicpp::EvLoop > spMainLoop,
                     ara::core::StringView const &serviceName) noexcept
{
    if (upIpcServer_) {
        LOGE() << "ipc::Server::Open(): Already opened !!!";
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
        LOGE() << "ipc::Server::Open(): nullptr == spMainLoop";
        return -1;
    }

    if (isoft::ipc::IPCInitNaiUDS(spMainLoop_) != 0) {
        LOGE() << "ipc::Server::Open(): IPCInitNaiUDS() error !!!";
        return -1;
    }

    upIpcServer_ = isoft::ipc::IPCServer::Create(serviceName.data());
    if (upIpcServer_ == nullptr) {
        LOGE() << "ipc::Server::Open(): Create service {" << serviceName << "} failed !!!";
        return -1;
    }

    upIpcServer_->SetHandler(IPC_SERVER_HANDLER_CONNECT, &HandleConnection, this);
    upIpcServer_->SetHandler(IPC_SERVER_HANDLER_DISCONNECT, &HandleConnection, this);
    upIpcServer_->SetHandler(IPC_SERVER_HANDLER_POST, &HandleRequest, this);
    upIpcServer_->SetHandler(IPC_SERVER_HANDLER_ONCE, &HandleRequest, this);
    upIpcServer_->SetHandler(IPC_SERVER_HANDLER_MULTI, &HandleRequest, this);

    if (0 != upIpcServer_->Start()) {
        LOGE() << "ipc::Server::Open(): Start service {" << serviceName << "} failed !!!";
        return -1;
    }

    return 0;
}

/// @brief Close IPC server
/// @return 0 success; <0 failure
int32_t Server::Close(void) noexcept
{
    if (upIpcServer_ == nullptr) {
        LOGE() << "ipc::Server::Close(): nullptr == upIpcServer_ !!!";
        return -1;
    }

    int32_t ret{0};
    while (true) {
        if (0 != upIpcServer_->Stop()) {
            LOGE() << "ipc::Server::Close(): Stop failed !!!";
            ret = -1;
            break;
        }

        upIpcServer_->Release();

        if (0 != isoft::ipc::IPCDeInitNaiUDS()) {
            LOGE() << "ipc::Server::Close(): IPCDeInitNaiUDS() failed !!!";
            ret = -1;
            break;
        }

        break;
    }

    if (0 == ret) {
        upIpcServer_.reset();
    }

    spMainLoop_.reset();
    return 0;
}

/// @brief Send response to IpcClient
/// @param sid Session ID
/// @param pRspMsg Response message
/// @param msgSize Message size
/// @param isLastRsp Whether it is the last response
/// @return 0 success; <0 failure
int32_t Server::SendResponse(uint64_t const sid,
                             const void *const pRspMsg,
                             size_t const msgSize,
                             bool const isLastRsp) const noexcept
{
    if (upIpcServer_ == nullptr) {
        LOGE() << "ipc::Server::SendResponse(): nullptr == upIpcServer_ !!!";
        return -1;
    }

    if (0U == sid) {
        LOGE() << "ipc::Server::SendResponse(): 0 == sid";
        return -1;
    }

    isoft::ipc::IPCPacket *const pRspPacket{upIpcServer_->MakeResponse(sid)};
    if (pRspPacket == nullptr) {
        LOGE() << "ipc::Server::SendResponse(): MakeResponse failed!!!";
        return -1;
    }

    isoft::ipc::IPCPacketBufferReference *const resBuffer{pRspPacket->AppendBuffer(static_cast< size_t >(msgSize))};
    if (resBuffer == nullptr) {
        LOGE() << "ipc::Server::SendResponse(): AppendBuffer failed!!!";
        return -1;
    }

    std::ignore = resBuffer->SetLen(static_cast< std::size_t >(msgSize));
    std::ignore = memcpy(resBuffer->GetPtr(), pRspMsg, static_cast< std::size_t >(msgSize));

    if (0 != upIpcServer_->Send(pRspPacket, isLastRsp)) {
        LOGE() << "ipc::Server::SendResponse(): Send failed !!!";
        return -1;
    }

    return 0;
}

/// @brief Handle IpcClient connection/disconnection request
/// @param pCtx Context pointer
/// @param type Request type
/// @param pReqPacket Request packet
/// @exception std::runtime_error If processing fails
void Server::HandleConnection(void *const pCtx,
                              isoft::ipc::IPCServerHandleType const type,
                              isoft::ipc::IPCPacket const *const pReqPacket)
{
    if ((pCtx == nullptr) || (pReqPacket == nullptr)) {
        LOGE() << "ipc::Server::HandleConnection(): nullptr == pCtx || nullptr == pReqPacket";
        return;
    }

    Server *const server{static_cast< Server * >(pCtx)};
    if (server->fConnHandler_) {
        server->fConnHandler_(type);
    }
}

/// @brief Handle IpcClient request
/// @param pCtx Context pointer
/// @param type Request type
/// @param pReqPacket Request packet
/// @exception std::runtime_error If processing fails
void Server::HandleRequest(void *const pCtx,
                           isoft::ipc::IPCServerHandleType const type,
                           isoft::ipc::IPCPacket *const pReqPacket)
{
    std::ignore = type;

    Server *const server{static_cast< Server * >(pCtx)};
    if (server == nullptr) {
        LOGE() << "ipc::Server::HandleRequest(): nullptr == client !!!";
        return;
    }

    if (pReqPacket == nullptr) {
        LOGE() << "ipc::Server::HandleRequest(): nullptr == pReqPacket !!!";
        return;
    }

    uint32_t const pid{static_cast< uint32_t >(pReqPacket->GetPeerPid())};
    uint64_t const sid{pReqPacket->GetSessionId()};
    isoft::ipc::IPCPacketBufferReference *const reqBuffer{pReqPacket->GetBuffer()};

    void *pReqMsg{nullptr};
    uint32_t msgSize{0U};
    if (reqBuffer != nullptr) {
        pReqMsg = reqBuffer->GetPtr();
        msgSize = static_cast< uint32_t >(reqBuffer->GetLen());
    }

    if (server->fReqHandler_) {
        server->fReqHandler_(pid, sid, pReqMsg, msgSize);
    }
}

}  // namespace ipc
}  // namespace internal
}  // namespace exec
}  // namespace ara
