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
/// @file       skeleton.cpp
/// @brief      time base proxy Skeleton class
/// @details
/// @date       2023-02-01
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================

#include "ara/tsync/internal/timebase/proxy/skeleton.h"

#include "ara/tsync/internal/common.h"
#include "ara/tsync/internal/log/logger.h"
#include "isoft/util/type_cast.h"

namespace ara {
namespace tsync {
namespace internal {
namespace timebase {
namespace proxy {

/// @brief log output
/// @return Logger object reference
static inline ara::tsync::internal::tslog::Logger &LOG() noexcept { return ara::tsync::internal::tslog::Log(); }

/// @brief create proxy skeleton
/// @param rs - time base resource manager handle
/// @param pconfig - configuration manager handle
/// @param timeDomainManager - time domain resource manager handle
/// @return proxy skeleton pointer
std::unique_ptr< Skeleton > Skeleton::CreateSkeleton(
    std::shared_ptr< timebase::resource::TBResManager > const &rs,
    std::shared_ptr< config::ConfigManager > const &pconfig,
    std::shared_ptr< timedomain::TDManager > const &timeDomainManager) noexcept
{
    std::unique_ptr< Skeleton > sk{nullptr};

    if (nullptr == rs) {
        return sk;
    }
    sk = std::make_unique< Skeleton >();
    if (nullptr != sk) {
        if (0 != sk->_open(rs, pconfig, timeDomainManager)) {
            sk = nullptr;
        }
    }
    return sk;
}

/// @brief destructor
Skeleton::~Skeleton() noexcept
{
    std::int32_t const ret{_close()};
    if (0 != ret) {
        LOG().Error() << "~Skeleton, Close error:" << ret;
    }
    resourceMan_ = nullptr;
}

/// @brief IPC service connection callback function, called when a client establishes or disconnects a connection.
/// @param context - user parameter
/// @param type - type
/// @param packet - data packet
/// @exception exception specification
void Skeleton::IPCServerConnectionHandler(void *const context,
                                          isoft::ipc::IPCServerHandleType const type,
                                          isoft::ipc::IPCPacket *const packet)
{
    std::ignore = packet;
    Skeleton *server{nullptr};

    server = isoft::util::PointerCast< Skeleton, void >(context);
    if (nullptr == server) {
        LOG().Error() << "nullptr == skeleton";
        return;
    }

    if (nullptr == server->ipcServer_) {
        LOG().Error() << "nullptr == server->ipcServer_";
        return;
    }

    if (type == IPC_SERVER_HANDLER_CONNECT) {
        LOG().Verbose() << "Client Connected.";
    } else if (type == IPC_SERVER_HANDLER_DISCONNECT) {
        /// Clear invalid session ID
        if (nullptr == server->sessionMan_) {
            return;
        }
        ara::core::Set< isoft::ipc::IPCSessionId > sidList;
        server->sessionMan_->GetAllSessionId(sidList);
        for (isoft::ipc::IPCSessionId const &id : sidList) {
            if (0 != server->ipcServer_->CheckSessionID(id)) {
                LOG().Verbose() << "clear unused session ID{" << id << "}";
                server->sessionMan_->ClearSession(id);
            }
        }
    } else {
        ;
    }
    return;
}

/// @brief IPC request callback function, called when a request from the client is received.
/// @param context - user parameter
/// @param htype - handle type
/// @param reqPacket - data packet
void Skeleton::IPCServerRequestHandler(void *const context,
                                       isoft::ipc::IPCServerHandleType const htype,
                                       isoft::ipc::IPCPacket *const reqPacket) noexcept
{
    std::ignore = htype;
    Skeleton *server{nullptr};
    isoft::ipc::IPCSessionId sessionId{0U};
    isoft::ipc::IPCPacketBufferReference *reqBuffer{nullptr};
    Message *msg{nullptr};
    Message::Type type;
    timebase::resource::TimeBaseId tbId;

    server = static_cast< Skeleton * >(static_cast< void * >(context));
    if (nullptr == server) {
        LOG().Error() << "nullptr == server";
        return;
    }

    if (nullptr == reqPacket) {
        LOG().Error() << "nullptr == reqPacket";
        return;
    }

    sessionId = reqPacket->GetSessionId();
    reqBuffer = reqPacket->GetBuffer();  // request buffer
    msg       = static_cast< Message * >(static_cast< void * >(reqBuffer->GetPtr()));

    type = msg->GetType();
    tbId = msg->GetTimeBaseId();

    switch (type) {
        case Message::Type::kSetTime:
        case Message::Type::kUpdateTime:
        case Message::Type::kSetUserData:
        case Message::Type::kSetRateDeviation: {
            if (nullptr != server->eventCb_) {
                server->eventCb_(tbId, type);
            }
            break;
        }
        case Message::Type::kStatusChanged:
        case Message::Type::kSynchronizationStateChanged:
        case Message::Type::kLeapJump:
        case Message::Type::kPrecisionMeasurement: {
            if (nullptr == server->sessionMan_) {
                break;
            }
            // SWS_TS_00803
            if ((Message::Type::kPrecisionMeasurement == type)
                && (true != server->_checkAllowMeasurementRegistration(tbId))) {
                break;
            }
            switch (msg->GetOption< Message::Option >()) {
                case Message::Option::kRegister: {
                    server->sessionMan_->AddSession(tbId.ToUint16(), type, sessionId);
                    break;
                }
                case Message::Option::kUnregister: {
                    server->sessionMan_->DelSession(tbId.ToUint16(), type, sessionId);
                    break;
                }
                default: {
                    break;
                }
            }
            break;
        }

        case Message::Type::kValidationMeasurement: {
            if (nullptr == server->sessionMan_) {
                break;
            }
            switch (msg->GetOption< Message::ValidationMeasurementOption >()) {
                case Message::ValidationMeasurementOption::kRegister: {
                    server->sessionMan_->AddSession(tbId.ToUint16(), type, sessionId);
                    break;
                }
                case Message::ValidationMeasurementOption::kUnregister: {
                    server->sessionMan_->DelSession(tbId.ToUint16(), type, sessionId);
                    break;
                }
                default: {
                    break;
                }
            }
            break;
        }
        default: {
            break;
        }
    }
    return;
}

/// @brief open service
/// @param rs - time base resource manager handle
/// @param pconfig - configuration manager handle
/// @param timeDomainManager - time domain resource manager handle
/// @return 0 - success
/// @return <0 - failure
std::int32_t Skeleton::_open(std::shared_ptr< timebase::resource::TBResManager > const &rs,
                             std::shared_ptr< config::ConfigManager > const &pconfig,
                             std::shared_ptr< timedomain::TDManager > const &timeDomainManager) noexcept
{
    resourceMan_       = rs;
    configManager_     = pconfig;
    timeDomainManager_ = timeDomainManager;
    //////////////////////  Open session manager  ////////////////
    sessionMan_ = std::make_unique< SessionManager >();
    if (nullptr == sessionMan_) {
        return kRET_E2;
    }
    /// TODO(person in charge): If service online notification needs to be registered, add code here. Currently, this feature is not needed
    //if (nullptr != sessionMan_) sessionMan_->OnDomainOnline(cb).

    //////////////////////  Open IPC server  ////////////////
    std::int32_t const r{isoft::ipc::IPCInitNaiUDS(isoft::naicpp::GlobalGeneralEvLoop::Get())};
    if (0 > r) {
        LOG().Error() << "IPCInitNaiUDS(): " << GetErrString();
        return kRET_E3;
    }

    ipcServer_ = isoft::ipc::IPCServer::Create(kServiceName);
    if (nullptr == ipcServer_) {
        LOG().Error() << "isoft::ipc::IPCClient::Create(" << kServiceName << ")";
        return kRET_E4;
    }
    isoft::ipc::IPCServerHandler ipcConntHandler{[this](void *const context,
                                                        isoft::ipc::IPCServerHandleType const status,
                                                        isoft::ipc::IPCPacket *const responsePacket) noexcept -> void {
        this->IPCServerConnectionHandler(context, status, responsePacket);
    }};
    isoft::ipc::IPCServerHandler ipcReqHandler{[this](void *const context, isoft::ipc::IPCServerHandleType const status,
                                                      isoft::ipc::IPCPacket *const responsePacket) noexcept -> void {
        this->IPCServerRequestHandler(context, status, responsePacket);
    }};
    ipcServer_->SetHandler(IPC_SERVER_HANDLER_CONNECT, ipcConntHandler, this);
    ipcServer_->SetHandler(IPC_SERVER_HANDLER_DISCONNECT, std::move(ipcConntHandler), this);
    // ipcServer_->SetHandler(IPC_SERVER_HANDLER_ONCE, IPCServerRequestHandler, this).
    ipcServer_->SetHandler(IPC_SERVER_HANDLER_MULTI, std::move(ipcReqHandler), this);

    if (0 != ipcServer_->Start()) {
        LOG().Error() << "isoft::ipc::IPCServer::Start(), errno=" << GetErrNo();
        return kRET_E5;
    }

    return 0;
}

/// @brief close service
/// @return 0 - success
/// @return <0 - failure
std::int32_t Skeleton::_close() noexcept
{
    std::int32_t ret{0};

    if (nullptr != ipcServer_) {
        if (0 != ipcServer_->Stop()) {
            LOG().Error() << "isoft::ipc::IPCClient::Stop(), errno=" << GetErrNo();
            ret = kRET_E1;
        }
        ipcServer_->Release();
    }

    if (-1 == isoft::ipc::IPCDeInitNaiUDS()) {
        LOG().Error() << "IPCDeInitNaiUDS(), errno=" << GetErrNo();
        ret = kRET_E2;
    }

    /// Do not stop the event loop; other modules may still be using it. Even if not in use, there is no need to close it.
    /// if (nullptr != mainLoop_) mainLoop_->Stop();

    ipcServer_  = nullptr;
    sessionMan_ = nullptr;

    return ret;
}

/// @brief general response function
/// @param serviceId - service ID (time base ID)
/// @param kSid - session ID
/// @param type - message type
/// @param option - message operation
/// @return 0 - success
/// @return <0 - failure
std::int32_t Skeleton::_doIpcResponse(SessionManager::ServiceId const &serviceId,
                                      SessionManager::SessionId const &kSid,
                                      SessionManager::SessionType const &type,
                                      std::uint8_t const &option) const noexcept
{
    isoft::ipc::IPCPacketBufferReference *resBuffer{nullptr};
    isoft::ipc::IPCPacket *responsePacket{nullptr};
    Message *msg{nullptr};

    responsePacket = ipcServer_->MakeResponse(kSid);
    if (nullptr == responsePacket) {
        LOG().Error() << "nullptr == ipcServer_->MakeResponse(" << kSid << ")";
        return internal::kRET_E1;
    }

    resBuffer = responsePacket->AppendBuffer(sizeof(Message));
    if (nullptr == resBuffer) {
        LOG().Error() << "nullptr == responsePacket->AppendBuffer(" << sizeof(Message) << ")";
        return internal::kRET_E2;
    }
    static_cast< void >(resBuffer->SetLen(sizeof(Message)));

    msg = new (reinterpret_cast< Message * >(resBuffer->GetPtr())) Message();
    msg->SetTimeBaseId(timebase::resource::TimeBaseId{serviceId});
    msg->SetType(type);
    switch (type) {
        case Message::Type::kStatusChanged:
        case Message::Type::kSynchronizationStateChanged:
        case Message::Type::kLeapJump:
        case Message::Type::kPrecisionMeasurement: {
            msg->SetOption(static_cast< Message::Option >(option));
            break;
        }
        case Message::Type::kValidationMeasurement: {
            msg->SetOption(static_cast< Message::ValidationMeasurementOption >(option));
            break;
        }
        default: {
            break;
        }
    }

    /// Parameter false means it is not the last message
    if (0 != this->ipcServer_->Send(responsePacket, false)) {
        LOG().Error() << "Send(), errno=" << GetErrNo();
        return internal::kRET_E2;
    }

    return internal::kRET_OK;
}

/// @brief general event notification. Notify the specified event on the specified time base.
/// @param timeBaseId - time base ID
/// @param eventType - event type
/// @param option - operation
/// @return notification result
std::int32_t Skeleton::_doNotify(timebase::resource::TimeBaseId const &timeBaseId,
                                 ProxyEventType const &eventType,
                                 std::uint8_t const &option) noexcept
{
    if (nullptr == ipcServer_) {
        LOG().Error() << "nullptr == ipcServer_";
        return kRET_E1;
    }

    if (nullptr == sessionMan_) {
        LOG().Error() << "nullptr == sessionMan_";
        return kRET_E2;
    }

    SessionManager::SessionTraverseHandler const walkSessionCb{
        [this, option](SessionManager::ServiceId const serId, SessionManager::SessionType const t,
                       SessionManager::SessionId const kSid) noexcept -> std::int32_t {
            return _doIpcResponse(serId, kSid, t, option);
        }};

    return sessionMan_->TraverseSession(timeBaseId.ToUint16(), eventType, walkSessionCb);
}

/// @traceid  {SWS_TS_00803}
/// @brief check whether time precision measurement registration is allowed, only non-global domain masters are allowed to register
/// @param timeBaseId - time base ID
/// @return true allowed, false not allowed
bool Skeleton::_checkAllowMeasurementRegistration(timebase::resource::TimeBaseId const &timeBaseId) noexcept
{
    if (nullptr == configManager_) {
        LOG().Error() << "nullptr == configManager_";
        return false;
    }
    if (nullptr == resourceMan_) {
        LOG().Error() << "nullptr == configManager_";
        return false;
    }

    timebase::resource::TBContext *const timeBaseCtx{resourceMan_->GetContext(timeBaseId)};
    if (nullptr == timeBaseCtx) {
        LOG().Error() << "nullptr == timeDomainManager_";
        return false;
    }
    internal::TimeDomainId const domainId{timeBaseCtx->GetTimeData().DomainId()};

    if (nullptr == configManager_->GetTimeDomainSet()) {
        LOG().Error() << "nullptr == configManager_->GetTimeDomainSet";
        return false;
    }
    config::TimeDomainSet::Master const *const timeDomainMasterCfg{
        configManager_->GetTimeDomainSet()->GetMaster(domainId)};
    config::TimeDomainSet::Domain const *const pdomaincfg{timeDomainManager_->GetConfig(domainId)};
    if ((nullptr != timeDomainMasterCfg) && (true == timeDomainMasterCfg->IsSystemWideGlobalTimeMaster())
        && (nullptr != pdomaincfg) && (pdomaincfg->GetIsMasterOnThisMachine())) {
        LOG().Error() << "isSystemWideGlobalTimeMaster is true cant register";
        return false;
    }
    return true;
}

}  // namespace proxy
}  // namespace timebase
}  // namespace internal
}  // namespace tsync
}  // namespace ara