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
/// @file       base_server.cpp
/// @brief
/// @details
/// @date       2024-06-20
/// @author     wangyanlong
/// @version    1.2.0
///
/// ================================================================

#include "ara/phm/internal/com/base_server.h"

#include <isoft/ipccpp/utility.h>

#include "ara/phm/internal/phm_log.h"

namespace ara {
namespace phm {
namespace internal {
namespace com {
namespace common {

/// @brief
/// @param addr
BaseServer::BaseServer(ara::core::String addr) noexcept
    : kAddr{std::move(addr)}
    , ipcServer_{nullptr}
    , mainLoop_{nullptr}
    , packet_{nullptr}
    , opened_{false}
    , pid_{nai_get_pid()}
    , mutex_{}
{
    std::cout << "BaseServer " << kAddr << std::endl;
}

/// @brief
BaseServer::~BaseServer() noexcept { std::ignore = Close(); }

/// @brief
/// @param mainLoop
/// @return
int32_t BaseServer::Open(std::shared_ptr< isoft::naicpp::EvLoop > const& mainLoop) noexcept
{
    std::lock_guard< std::mutex > const lock{mutex_};
    if (opened_) {
        LOG_INFO << kAddr.c_str() << " is already opened";
        return 0;
    }

    if (nullptr == mainLoop) {
        std::ignore
            = isoft::naicpp::GlobalGeneralEvLoop::Initialize(isoft::naicpp::GlobalGeneralEvLoop::Mode::kInsideThread);
        mainLoop_ = isoft::naicpp::GlobalGeneralEvLoop::Get();
    } else {
        mainLoop_ = mainLoop;
    }

    if (nullptr == mainLoop_) {
        LOG_ERROR << "BaseServer::Open, mainloop is null";
        return -1;
    }

    // Allow multiple calls, counter++
    int32_t const ret{isoft::ipc::IPCInitNaiUDS(mainLoop_)};
    if (ret == -1) {
        LOG_ERROR << "BaseServer::Open, IPCInitNaiUDS failed";
        return -1;
    }

    ipcServer_ = isoft::ipc::IPCServer::Create(kAddr.c_str());
    if (nullptr == ipcServer_) {
        LOG_ERROR << "BaseServer::Open, create ipc server failed, server addr " << kAddr.c_str();
        return -1;
    }

    // post corresponds to an event, no response needed
    // once corresponds to a method, response needed
    ipcServer_->SetHandler(IPC_SERVER_HANDLER_CONNECT,
                           // TODO(wangyanlong): std:bind is not recommended in QAC
                           std::bind(&BaseServer::_connectionHandler, this, std::placeholders::_1,
                                     std::placeholders::_2, std::placeholders::_3),
                           this);

    ipcServer_->SetHandler(IPC_SERVER_HANDLER_DISCONNECT,
                           std::bind(&BaseServer::_connectionHandler, this, std::placeholders::_1,
                                     std::placeholders::_2, std::placeholders::_3),
                           this);

    ipcServer_->SetHandler(IPC_SERVER_HANDLER_POST, &BaseServer::IPCServerHandler, this);

    ipcServer_->SetHandler(IPC_SERVER_HANDLER_ONCE, &BaseServer::IPCServerHandler, this);

    ipcServer_->SetHandler(IPC_SERVER_HANDLER_MULTI, &BaseServer::IPCServerHandler, this);
    if (0 != ipcServer_->Start()) {
        LOG_ERROR << "start server " << kAddr.c_str() << " failed.";
        return -1;
    }

    opened_ = true;
    LOG_INFO << "server " << kAddr.c_str() << " opened.";
    return 0;
}

/// @brief
/// @return
int32_t BaseServer::Open() noexcept { return Open(std::shared_ptr< isoft::naicpp::EvLoop >{nullptr}); }

/// @brief
/// @return
int32_t BaseServer::Close() noexcept
{
    std::lock_guard< std::mutex > const lock{mutex_};
    if (!opened_) {
        std::cout << "BaseServer::Close, server " << kAddr.c_str() << " is already closed\n";
        return 0;
    }

    int32_t ret{0};
    if (nullptr == ipcServer_) {
        LOG_WARN << "BaseServer::Close, ipc server is null";
        return -1;
    }

    if (0 != ipcServer_->Stop()) {
        LOG_ERROR << "BaseServer::Close, stop server " << kAddr.c_str() << " failed";
        ret = -1;
    }

    ipcServer_->Release();
    if (-1 == isoft::ipc::IPCDeInitNaiUDS()) {
        LOG_ERROR << "BaseServer::Close, IPCDeInitNaiUDS failed";
        ret = -1;
    }

    opened_ = false;
    mainLoop_.reset();
    LOG_INFO << "server " << kAddr.c_str() << " closed.";
    return ret;
}
/// @brief
/// @return
bool BaseServer::IsOpened() noexcept
{
    std::lock_guard< std::mutex > const lock{mutex_};
    return opened_;
}

/// @brief
/// @param context
/// @param type
/// @param packet
void BaseServer::IPCServerHandler(void* const context,
                                  isoft::ipc::IPCServerHandleType const type,
                                  isoft::ipc::IPCPacket* const packet)
{
    std::ignore = type;
    BaseServer* const baseServer{static_cast< BaseServer* >(context)};
    if (baseServer == nullptr) {
        LOG_FATAL << "context is null.";
        std::terminate();
        return;
    }

    baseServer->ServerHandler(packet);
}

/// @brief
/// @param context
/// @param type
/// @param packet
void BaseServer::_connectionHandler(void const* const context,
                                    ::isoft::ipc::IPCServerHandleType const type,
                                    ::isoft::ipc::IPCPacket* const packet) const
{
    std::ignore = context;
    uint64_t const pid{packet->GetPeerPid()};
    switch (type) {
        case IPC_SERVER_HANDLER_CONNECT: {
            LOG_INFO << "client (pid:" << pid << ") has connected server " << kAddr.c_str()
                     << " successfully";  // TODO(wangyanlong): Why can't I see the print from this function, but the functionality is normal
            break;
        }
        case IPC_SERVER_HANDLER_DISCONNECT: {
            LOG_INFO << "client (pid:" << pid << ") has disconnected server " << kAddr.c_str();
            break;
        }
        default: {
            LOG_INFO << "client (pid:" << pid << ") Error type=" << static_cast< int32_t >(type) << ", server "
                     << kAddr.c_str();
            break;
        }
    }
}

/// @brief
void BaseServer::_SetPacket(isoft::ipc::IPCPacket* const packet) noexcept { packet_ = packet; }

/// @brief
/// @return
isoft::ipc::IPCPacket* const& BaseServer::_GetPacket() const noexcept { return packet_; }

/// @brief
/// @return
std::shared_ptr< isoft::ipc::IPCServer > BaseServer::_GetIpcServer() const noexcept { return ipcServer_; }

}  // namespace common
}  // namespace com
}  // namespace internal
}  // namespace phm
}  // namespace ara