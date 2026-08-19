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
/// @file       ipc_server.cpp
/// @brief      fw IPC processing logic
/// @details    fw IPC processing logic
/// @date       2024-12-17
/// @author     jzy
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/fw/IPC Communication Management
/// @interface_level=module
/// @trace_id_sr=SR_FW_0001,SR_FW_0011
/// @unit_name=Ipc_Server
/// @unit_description=Firewall IPC processing class
/// @endcode
///
/// ================================================================
///
///
/// ================================================================

#include "ipc_server.h"

#include <isoft/ipccpp/buffer.h>
#include <isoft/ipccpp/utility.h>

#include "ara/fw/internal/ipc_name.h"
#include "ara/fw/internal/log_api.h"

namespace ara {
namespace fw {
namespace internal {

//********************************/
/// @brief Extend the event loop using the nai library
using ::isoft::naicpp::EvLoop;
/// @brief Extend the event loop using the nai library
using ::isoft::naicpp::GlobalGeneralEvLoop;
//********************************/

/// @brief
/// @param fwManager
PIpcServer::PIpcServer(FWManager &fwManager) noexcept : pEvLoop_{}, pIpcServer_{}, ipcProcessManager_{fwManager} {}

/// @brief Start IPC service
/// @return
bool PIpcServer::Init() noexcept
{
    // Initialize event loop
    //  2022-10-27
    // According to GlobalGeneralEvLoop::Initialize internal logic, this function
    // can be called multiple times (only the first time performs initialization).
    // So, to ensure robustness, call again.
    // //2022-10-22 Initialization in ara::core::Initialize depends on whether
    // precompiled macros are defined
    pEvLoop_ = GlobalGeneralEvLoop::Get();
    if (nullptr == pEvLoop_) {
        LogError() << "PIpcServer::Begin(): nullptr == GlobalGeneralEvLoop";
        return false;
    }
    int32_t const r{isoft::ipc::IPCInitNaiUDS(pEvLoop_)};
    if (r == -1) {
        LogError() << "PIpcServer::Begin()->IPCInitNaiUDS(): " << r;
        return false;
    }
    // Initialize IPCServer
    ara::core::String const stIpcName{GetFWIpcFile_Server_Keys()};
    pIpcServer_ = isoft::ipc::IPCServer::Create(stIpcName.data());
    if (nullptr == pIpcServer_) {
        LogError() << "PIpcServer::Begin()->isoft::ipc::IPCServer::Create(" << stIpcName.data() << ")";
        return false;
    }
    pIpcServer_->SetHandler(IPC_SERVER_HANDLER_CONNECT, &CB_IpcProcess_Connect, this);
    pIpcServer_->SetHandler(IPC_SERVER_HANDLER_DISCONNECT, &CB_IpcProcess_DisConnect, this);
    pIpcServer_->SetHandler(IPC_SERVER_HANDLER_POST, &CB_IpcProcess_MsgPost, this);
    pIpcServer_->SetHandler(IPC_SERVER_HANDLER_ONCE, &CB_IpcProcess_MsgReq, this);
    pIpcServer_->SetHandler(IPC_SERVER_HANDLER_MULTI, &CB_IpcProcess_MsgReq, this);
    LogInfo() << "PIpcServer::Begin";

    return true;
}

/// @brief IPC working
/// @return
bool PIpcServer::Start() noexcept
{
    if (pEvLoop_ == nullptr) {
        return false;
    }
    int32_t ret{pIpcServer_->Start()};
    if (ret < 0) {
        LogError() << "PIpcServer::start() error ";
        return false;
    }

    LogInfo() << "PIpcServer::Start";
    ret = pEvLoop_->Run(true);
    if (ret < 0) {
        LogError() << "PIpcServer::Start evloop run error ";
        return false;
    }
    LogInfo() << "PIpcServer::Start Exit.";
    return true;
}
/// @brief IPC service end
/// @return
void PIpcServer::Stop() noexcept
{
    if (pIpcServer_ != nullptr) {
        std::ignore = pIpcServer_->Stop();
        pIpcServer_->Release();
        pIpcServer_.reset();
    }
    /// Do not stop Evloop, other modules may still be using it, and even if not,
    /// there is no need to shut it down.
    if (pEvLoop_ != nullptr) {
        pEvLoop_.reset();
    }

    if (-1 == isoft::ipc::IPCDeInitNaiUDS()) {
        LogError() << "eccp::Server::Close()->IPCDeInitNaiUDS()";
        return;
    }

    LogInfo() << "PIpcServer::End";
    return;
}

/// @brief Handle connect/disconnect messages
/// @param context IPC message context information
/// @param type IPC service handle type (IPC event type)
/// @param pReqPacket IPC packet
/// @throws
void PIpcServer::CB_IpcProcess_Connect(void *const context,
                                       isoft::ipc::IPCServerHandleType const type,
                                       isoft::ipc::IPCPacket *const pReqPacket) noexcept
{
    std::ignore = (context);
    std::ignore = type;
    std::ignore = pReqPacket;
    if ((type != IPC_SERVER_HANDLER_CONNECT)) {
        LogError() << "CB_IpcProcess_Connect: get wrong ipc type.type=" << static_cast< int32_t >(type);
        return;
    }
    LogInfo() << "ipc connected success!";
    return;
}

/// @brief Disconnect message
/// @param context IPC message context information
/// @param type IPC service handle type (IPC event type)
/// @param pReqPacket IPC packet
/// @throws
void PIpcServer::CB_IpcProcess_DisConnect(void *const context,
                                          isoft::ipc::IPCServerHandleType const type,
                                          isoft::ipc::IPCPacket *const pReqPacket) noexcept
{
    std::ignore = (context);
    std::ignore = type;
    std::ignore = pReqPacket;
    if ((type != IPC_SERVER_HANDLER_DISCONNECT)) {
        LogError() << "CB_IpcProcess_DisConnect: get wrong ipc type.type=" << static_cast< int32_t >(type);
        return;
    }
    LogInfo() << "disconnected success!";
    return;
}

/// @brief Handle notification messages
/// @param context IPC message context information
/// @param type IPC service handle type (IPC event type)
/// @param pReqPacket IPC packet
/// @throws
void PIpcServer::CB_IpcProcess_MsgPost(void *const context,
                                       isoft::ipc::IPCServerHandleType const type,
                                       isoft::ipc::IPCPacket *const pReqPacket) noexcept
{
    // static_cast<void>(context);
    // static_cast<void>(packet);
    if (type != IPC_SERVER_HANDLER_POST) {
        LogError() << "CB_IpcProcess_MsgPost: receive type must "
                      "IPC_SERVER_HANDLER_POST! type="
                   << static_cast< int32_t >(type);
        return;
    }
    PIpcServer *const pIpcServer{static_cast< PIpcServer * >(context)};
    if (nullptr == pIpcServer) {
        LogError() << "CB_IpcProcess_MsgPost:get context failed!";
        return;
    }

    // Actual business processing
    FwErrc errCode{pIpcServer->ipcProcessManager_.ReceiveDataProcess(pReqPacket)};
    // Processing completed, return result to client.
    pIpcServer->ipcProcessManager_.ResponseProcessResult(errCode, pReqPacket);
    return;
}

/// @brief Handle request messages
/// @param context IPC message context information
/// @param type IPC service handle type (IPC event type)
/// @param pReqPacket IPC packet
/// @throws
void PIpcServer::CB_IpcProcess_MsgReq(void *const context,
                                      isoft::ipc::IPCServerHandleType const type,
                                      isoft::ipc::IPCPacket *const pReqPacket) noexcept
{
    // static_cast<void>(context);
    // static_cast<void>(pReqPacket);
    if ((type != IPC_SERVER_HANDLER_ONCE) && (type != IPC_SERVER_HANDLER_MULTI)) {
        LogError() << "CB_IpcProcess_MsgReq: receive type \
        must IPC_SERVER_HANDLER_ONCE or IPC_SERVER_HANDLER_MULTI! type="
                   << static_cast< int32_t >(type);
        return;
    }

    PIpcServer *const pIpcServer{static_cast< PIpcServer * >(context)};

    if (nullptr == pIpcServer) {
        LogError() << "CB_IpcProcess_MsgReq:get context failed!";
        return;
    }

    // Actual business processing
    FwErrc errCode{pIpcServer->ipcProcessManager_.ReceiveDataProcess(pReqPacket)};

    // Processing completed, return result to client.
    pIpcServer->ipcProcessManager_.ResponseProcessResult(errCode, pReqPacket);
    return;
}

/// @brief Send IPC packet
/// @param pReqPacket IPC packet
/// @param bComplete Whether completed
/// @return
/// @throws
int32_t PIpcServer::SendIpcPacket(isoft::ipc::IPCPacket *const pReqPacket, bool const bComplete) const
{
    if ((nullptr == pIpcServer_) || (nullptr == pReqPacket)) {
        return 0;
    }
    int32_t const len{static_cast< int32_t >(pReqPacket->GetBuffer()->GetLen())};
    int32_t const ret{pIpcServer_->Send(pReqPacket, bComplete)};
    if (ret != 0) {
        std::ignore = printf("errno:%d\n", ret);
    }
    return len;
}

/// @brief Create a new IPC packet
/// @param nSessionID Temporary session ID
/// @return
/// @throws
isoft::ipc::IPCPacket *PIpcServer::NewIpcPacket(uint64_t const nSessionID) const noexcept
{
    if (nullptr == pIpcServer_) {
        return nullptr;
    }
    return pIpcServer_->MakeResponse(nSessionID);
}

//********************************/
}  // namespace internal
}  // namespace fw
}  // namespace ara
