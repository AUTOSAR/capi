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
/// @file       idsm_server.cpp
/// @brief      Security event receiver
/// @details
/// @date       2023-01-13
/// @author     niuliming
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/idsm/event receiver handler
/// @interface_level=module
/// @trace_id_sr=SR_IDSM_0001
/// @unit_name=IdsmServer
/// @unit_description=
/// @endcode
///
/// ================================================================

#include "idsm_server.h"

#include <ara/core/instance_specifier.h>
#include <isoft/ara_fsh/filesystem_hierarchy.h>
#include <isoft/ipccpp/buffer.h>
#include <isoft/ipccpp/packet.h>

#include "ara/idsm/internal/common.h"
#include "idsm_fg_state.h"
#include "idsm_sev_proc.h"
#include "log/idsm_log.h"
#ifdef ARA_WITH_SM
    #include <ara/sm/internal/fg_state_notify_async_client.h>
    #include <ara/sm/internal/fg_state_notify_error_domain.h>
#endif

namespace ara {
namespace idsm {
#ifdef ARA_WITH_SM
/// @brief Type redefinition
using FGStateNotifyAsyncClient = ara::sm::fg_state_notify_client::FGStateNotifyAsyncClient;
/// @brief Type redefinition
using FGStateType = ara::sm::fg_state_notify_client::FGStateType;
#endif
/// @brief Namespace
using isoft::naicpp::EvLoop;
/// @brief Namespace
using isoft::naicpp::GlobalGeneralEvLoop;
/// @brief Initialize
/// @return 0 on success, non-zero on failure
/// @code{.isoft}
/// @threadsafety={unsafe}
/// @endcode
int32_t IdsmServer::Init() noexcept
{
    uint16_t constexpr kNum1000{1000U};
    feedInterval_ = kNum1000;
    /// IPC initialization
    int32_t ret{_initIpc()};
    if (0 != ret) {
        return ret;
    }
    evProcesser_ = std::make_shared< EventProcer >();
    std::ignore  = evProcesser_->Init();
    // Get function group state registration
    ret = _initFGNotify();
    if (0 != ret) {
        return ret;
    }
    /// @Register timer
    {
        std::function< void() > timer{[this]() {
            this->evProcesser_->Timerhandler();
            if (this->feedTimer_->UpdateTime(this->feedInterval_) < 0) {
                LOG_ERROR << "filters registe timer fail";
            }
        }};
        std::ignore = evLoopPtr_->MakeTimer(feedTimer_, feedInterval_, std::move(timer));
    }
    return 0;
}
/// @brief Start event loop
/// @return 0 on success, non-zero on failure
/// @code{.isoft}
/// @threadsafety={unsafe}
/// @endcode
int32_t IdsmServer::Start() noexcept
{
    int32_t const ret{evLoopPtr_->Run(true)};
    if (ret < 0) {
        LOG_ERROR << "Idsm event main loop run errno" << SysErr();
        int32_t const tempErr{2};
        return tempErr;
    }
    return 0;
}
/// @brief Stop all services && clean up service resources
/// @code{.isoft}
/// @threadsafety={unsafe}
/// @endcode
void IdsmServer::Stop() const noexcept
{
    LOG_DEBUG << "Startup handle term signal.";
    int32_t const ret{ipcServer_->Stop()};
    if (0 != ret) {
        LOG_ERROR << "ipc server stop erorr";
        return;
    }
    ipcServer_->Release();
    std::ignore = isoft::ipc::IPCDeInitNaiUDS();
#ifdef ARA_WITH_SM
    FGStateNotifyAsyncClient::GetInstance()->Destroy();
#endif
}
/// @brief Process data sent by user
/// @param packet User data
/// @exception Stack overflow exception
/// @code{.isoft}
/// @threadsafety={unsafe}
/// @endcode
void IdsmServer::_handleReceiveData(::isoft::ipc::IPCPacket* const packet)
{
    ::isoft::ipc::IPCPacketBufferReference* const ipcBuffer{packet->GetBuffer()};
    if (nullptr == ipcBuffer) {
        LOG_ERROR << "Idsm receive event from AA. err: ipc read data fail";
        return;
    }

    ClientEventMsg eventMsg;
    size_t const msgSize{ipcBuffer->GetLen()};
    ClientEventParse::Deserialize(ipcBuffer->GetPtr(), msgSize, &eventMsg);
    LOG_DEBUG << "Idsm receive event from AA. event: " << eventMsg.GetInstanceId().c_str()
              << " counter:" << eventMsg.GetCounter();
    std::ignore = evProcesser_->Process(eventMsg);
}
/// @brief Initialize IPC
/// @return 0 on success, non-zero on failure
/// @code{.isoft}
/// @threadsafety={unsafe}
/// @endcode
int32_t IdsmServer::_initIpc() noexcept
{
    //  Initialize IPC
    LOG_DEBUG << "idsm server: begin init IPC";
    evLoopPtr_ = GlobalGeneralEvLoop::Get();
    if (evLoopPtr_ == nullptr) {
        return -1;
    }
    int32_t const ret{::isoft::ipc::IPCInitNaiUDS(evLoopPtr_)};
    if (ret == -1) {
        LOG_ERROR << "idsm server: call IPCInitNaiUDS func fail";
        return -1;
    }
    ara::core::StringView const ipcIdsmServer{"qsev"};
    ipcServer_ = ::isoft::ipc::IPCServer::Create(ipcIdsmServer.data());
    if (nullptr == ipcServer_) {
        LOG_ERROR << "idsm server: create ipc server fail. errno: " << SysErr();
        return 1;
    }
    /// @brief Register connection and disconnection handling functions
    {
        ::isoft::ipc::IPCServerHandler const handler{[](void const* const context,
                                                        ::isoft::ipc::IPCServerHandleType const type,
                                                        ::isoft::ipc::IPCPacket* const packet) {
            static_cast< void >(context);
            uint64_t const pid{packet->GetPeerPid()};
            switch (type) {
                case IPC_SERVER_HANDLER_CONNECT: {
                    LOG_INFO << "idsm server: client (pid:" << pid << ") has connected successfully";
                    break;
                }
                case IPC_SERVER_HANDLER_DISCONNECT: {
                    LOG_INFO << "idsm server: client (pid:" << pid << ") has disconnected";
                    break;
                }
                default: {
                    LOG_ERROR << "idsm server: client (pid:" << pid << ") Error type=" << static_cast< int32_t >(type);
                    break;
                }
            }
        }};
        ipcServer_->SetHandler(IPC_SERVER_HANDLER_CONNECT, handler, ipcServer_.get());
        ipcServer_->SetHandler(IPC_SERVER_HANDLER_DISCONNECT, handler, ipcServer_.get());
    }
    /// @brief Register handler for POST type messages
    {
        ::isoft::ipc::IPCServerHandler const handler{[this](void const* const context,
                                                            ::isoft::ipc::IPCServerHandleType const type,
                                                            ::isoft::ipc::IPCPacket* const packet) {
            static_cast< void >(context);
            if (type != IPC_SERVER_HANDLER_POST) {
                LOG_ERROR << "idsm server: receive type must IPC_SERVER_HANDLER_POST! type="
                          << static_cast< int32_t >(type);
                return;
            }
            if (nullptr == packet) {
                LOG_ERROR << "idsm server: PostHandler packet is nullptr";
                return;
            }

            this->_handleReceiveData(packet);
        }};
        ipcServer_->SetHandler(IPC_SERVER_HANDLER_POST, handler, ipcServer_.get());
    }
    /// @brief Register handler for IPC_SERVER_HANDLER_ONCE type messages
    {
        ::isoft::ipc::IPCServerHandler const handler{[this](void const* const context,
                                                            ::isoft::ipc::IPCServerHandleType const type,
                                                            ::isoft::ipc::IPCPacket* const packet) {
            static_cast< void >(context);
            static_cast< void >(packet);
            if (type != IPC_SERVER_HANDLER_ONCE) {
                LOG_ERROR << "idsm server: receive type must IPC_SERVER_HANDLER_ONCE! type="
                          << static_cast< int32_t >(type);
                return;
            }

            this->_handleReceiveData(packet);

            ::isoft::ipc::IPCSessionId const sessionId{packet->GetSessionId()};
            ::isoft::ipc::IPCPacket* const responsePacket{ipcServer_->MakeResponse(sessionId)};
            uint16_t const responseSize{5U};
            ::isoft::ipc::IPCPacketBufferReference* const buffer{responsePacket->AppendBuffer(responseSize)};
            uint8_t* const buf{buffer->GetPtr()};
            uint32_t const status{0U};
            std::ignore = memcpy(buf, &status, sizeof(status));
            std::ignore = buffer->SetLen(sizeof(status));
            std::ignore = ipcServer_->Send(responsePacket, true);
        }};
        ipcServer_->SetHandler(IPC_SERVER_HANDLER_ONCE, handler, ipcServer_.get());
    }

    LOG_INFO << "idsm server: init ipc server success. ipc server: " << ipcIdsmServer;
    return 0;
}
/// @brief Initialize function group states
/// @return 0 on success, non-zero on failure
/// @exception Stack overflow exception
/// @code{.isoft}
/// @threadsafety={unsafe}
/// @endcode
int32_t IdsmServer::_initFGNotify()
{
    // LOG_INFO << "init functional group notify start";
#ifdef ARA_WITH_SM
    // Initialize state management interface for getting function group states
    ara::core::Result< void > ret{
        FGStateNotifyAsyncClient::GetInstance()->Init(ara::core::String{"fg_state_notify_test_async"})};
    if (!ret.HasValue()) {
        LOG_ERROR << "InitNotify FGStateNotifyAsyncClient Init errmsg=" << ret.Error().Message();
        return -1;
    }

    // Get all function group states
    {
        int32_t const timeout{6000};
        std::function< ara::sm::fg_state_notify_client::RequestFGStateRespMsgHandlerType > const handler{
            [this](ara::core::Result< void > const& res, ara::core::Vector< FGStateType > const& allFGState) {
                if (!res.HasValue()) {
                    LOG_ERROR << "SwitchModel AllFgStateHandler errmsg=" << res.Error().Message();
                    return;
                }

                if (allFGState.empty()) {
                    LOG_ERROR << "state manager notify function group state is empty";
                }

                size_t const allFGStateSize{allFGState.size()};
                for (size_t i{0U}; i < allFGStateSize; ++i) {
                    LOG_INFO << "function group name:" << allFGState.at(i).fgName.c_str()
                             << "function group state:" << allFGState.at(i).fgState.c_str();
                    FunGrpState::GetInstance()->Set(allFGState.at(i).fgName, allFGState.at(i).fgState);
                }

                std::ignore = this->evLoopPtr_->Exec([this]() {
                    if (this->ipcServer_->Start() < 0) {
                        LOG_ERROR << "ipc server start errno" << SysErr();
                        this->Stop();
                        return;
                    }
                    LOG_INFO << "ipc server start successful";
                });
            }};
        ret = FGStateNotifyAsyncClient::GetInstance()->RequestAllFGState(handler, timeout);
        if (!ret.HasValue()) {
            LOG_ERROR << "InitNotify FGStateNotifyClient RequestAllFGState errmsg=" << ret.Error().Message();
            return 1;
        }
    }

    // Register function group state change notification
    {
        int32_t const timeout{2000};
        std::function< ara::sm::fg_state_notify_client::SubscribeRespMsgHandlerType > const subscribeHandler{
            [](ara::core::Result< void > const& res) {
                if (!res.HasValue()) {
                    LOG_ERROR << "SwitchModel SubscribeFgStateMsgHandler errmsg=" << res.Error().Message();
                    return;
                }
            }};
        std::function< void(FGStateType & fGState) > const handler{[](FGStateType const& state) {
            LOG_DEBUG << "SwitchModel FGStateHanler start.";
            FunGrpState::GetInstance()->Set(state.fgName, state.fgState);
            LOG_INFO << "SwitchModel FGStateHanler successful.";
        }};
        ret = FGStateNotifyAsyncClient::GetInstance()->SubscribeFGState(subscribeHandler, handler, timeout);
        if (!ret.HasValue()) {
            LOG_ERROR << "Failed to SubscribeFGState.";
            int32_t const tempErr{3};
            return tempErr;
        }
    }
#endif
    return 0;
}

}  // namespace idsm
}  // namespace ara