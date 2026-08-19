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
/// @file       stub.cpp
/// @brief      time base proxy Stub class
/// @details
/// @date       2023-02-01
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================

#include "ara/tsync/internal/timebase/proxy/stub.h"

#include "ara/tsync/internal/common.h"
#include "ara/tsync/internal/log/logger.h"

namespace ara {
namespace tsync {
namespace internal {
namespace timebase {
namespace proxy {

/// @brief log output
/// @return Logger object reference
static inline ara::tsync::internal::tslog::Logger &LOG() noexcept { return ara::tsync::internal::tslog::Log2Console(); }

/// @brief create proxy stub (client)
/// @param tbName - time base name
/// @return proxy stub pointer
std::unique_ptr< Stub > Stub::CreateStub(ara::core::String const &tbName) noexcept
{
    std::unique_ptr< Stub > stub{nullptr};

    ara::core::Result< void > ret;
    ret  = isoft::naicpp::GlobalGeneralEvLoop::Initialize(isoft::naicpp::GlobalGeneralEvLoop::Mode::kInsideThread);
    stub = std::make_unique< Stub >();
    if (nullptr != stub) {
        if (0 != stub->Open(tbName)) {
            static_cast< void >(stub->Close());
            stub = nullptr;
        }
    }
    return stub;
}

/// @brief destructor
Stub::~Stub() noexcept { static_cast< void >(Close()); }

/// @brief IPC asynchronous send callback function
/// @param context - user parameter
/// @param status - IPC communication status
/// @param responsePacket - the packet responsed from peer
/// @exception exception specification
void Stub::IPCClientSendAsyncHandler(void *const context,
                                     isoft::ipc::IPCClientHandlerStatus const status,
                                     isoft::ipc::IPCPacket *const responsePacket)
{
    Stub *client{nullptr};
    isoft::ipc::IPCPacketBufferReference *buffer{nullptr};
    Message *msg{nullptr};
    Message::Type type;
    std::uint8_t option{0U};

    client = static_cast< Stub * >(context);
    if (nullptr == client) {
        LOG().Error() << "nullptr == client";
        return;
    }

    if (status == IPC_CLIENT_HANDLER_STATUS_ERR) {
        LOG().Error() << "IPC_CLIENT_HANDLER_STATUS_ERR == status";
        return;
    }

    if (nullptr == responsePacket) {
        LOG().Error() << "SendAsync()->Handler(): nullptr == responsePacket";
        return;
    }

    buffer = responsePacket->GetBuffer();
    if (nullptr == buffer) {
        LOG().Error() << "nullptr == buffer";
        return;
    }

    msg = reinterpret_cast< Message * >(buffer->GetPtr());
    if (sizeof(Message) != buffer->GetLen()) {
        LOG().Error() << "Invalid Message Size";
        return;
    }

    type   = msg->GetType();
    option = msg->GetOption();
    NotificationHandler const cb{client->notificationHandlers_[type]};
    if (nullptr != cb) {
        cb(type, option);
    }

    return;
}

/// @brief IPC general request operation
/// @param type - event type
/// @param option - event operation
/// @return 0 - success
/// @return <0 - failure
std::int32_t Stub::_doIpcRequest(ProxyEventType const &type, std::uint8_t const &option) noexcept
{
    if (nullptr == ipcClient_) {
        LOG().Error() << "nullptr == ipcClient_";
        return kRET_E1;
    }

    isoft::ipc::IPCPacket *const ipcRequest{ipcClient_->MakeRequest()};
    if (nullptr == ipcRequest) {
        LOG().Error() << "nullptr == ipcClient_->MakeRequest()";
        return kRET_E2;
    }

    isoft::ipc::IPCPacketBufferReference *const reqBuffer{ipcRequest->AppendBuffer(sizeof(Message))};
    if (nullptr == reqBuffer) {
        LOG().Error() << "ipcRequest->AppendBuffer(" << sizeof(Message) << ")";
        return kRET_E3;
    }
    static_cast< void >(reqBuffer->SetLen(sizeof(Message)));

    Message *const msg{new (reinterpret_cast< Message * >(reqBuffer->GetPtr())) Message()};
    msg->SetTimeBaseId(timeBaseId_);
    msg->SetType(type);
    switch (type) {
        case Message::Type::kSetTime:
        case Message::Type::kUpdateTime:
        case Message::Type::kSetUserData:
        case Message::Type::kSetRateDeviation: {
            msg->SetOption(Message::Option::kNotify);
            break;
        }
        case Message::Type::kStatusChanged:
        case Message::Type::kSynchronizationStateChanged:
        case Message::Type::kLeapJump:
        case Message::Type::kPrecisionMeasurement: {
            Message::Option const opt{static_cast< Message::Option >(option)};
            if ((Message::Option::kRegister == opt) || (Message::Option::kUnregister == opt)) {
                msg->SetOption(opt);
            }
            break;
        }

        case Message::Type::kValidationMeasurement: {
            Message::ValidationMeasurementOption const vmOpt{
                static_cast< Message::ValidationMeasurementOption >(option)};
            if ((Message::ValidationMeasurementOption::kRegister == vmOpt)
                || (Message::ValidationMeasurementOption::kUnregister == vmOpt)) {
                msg->SetOption(vmOpt);
            }
            break;
        }

        default: {
            break;
        }
    }

    /// true represents subscription mode, requires multiple replies; -1 means no timeout is set
    std::int32_t r{0};
    isoft::ipc::IPCClientHandler ipcHandler{[this](void *const context, isoft::ipc::IPCClientHandlerStatus const status,
                                                   isoft::ipc::IPCPacket *const responsePacket) noexcept -> void {
        this->IPCClientSendAsyncHandler(context, status, responsePacket);
    }};
    r = ipcClient_->SendAsync(ipcRequest, true, std::move(ipcHandler), -1, this);
    if (r < 0) {
        LOG().Error() << "SendAsync(), errno=" << GetErrNo();
        return kRET_E6;
    }

    return 0;
}

/// @brief open time base proxy client
/// @param tbName - time base name
/// @return 0 - success
/// @return <0 - failure
std::int32_t Stub::Open(ara::core::String const &tbName) noexcept
{
    timeBaseName_ = tbName;
    resourceMan_  = timebase::resource::TBResManager::OpenManager();
    if (nullptr == resourceMan_) {
        // LOG().Error() << "Stub::Open TBResManager::OpenManager fail, " << timeBaseName_
        return internal::kRET_E5;
    }

    timeBaseId_ = resourceMan_->GetIdbyName(timeBaseName_);
    if (true != timeBaseId_.IsValid()) {
        LOG().Error() << "Stub::Open timeBaseId_.IsInvalid, " << timeBaseName_;
        return internal::kRET_E6;
    }

    {
        mainLoop_ = isoft::naicpp::GlobalGeneralEvLoop::Get();

        if (nullptr == mainLoop_) {
            LOG().Error() << "Stub::Open nullptr == mainLoop_";
            return internal::kRET_E1;
        }

        std::int32_t const r{isoft::ipc::IPCInitNaiUDS(mainLoop_)};
        if (r == -1) {
            LOG().Error() << "Stub::Open IPCInitNaiUDS(): " << r;
            mainLoop_ = nullptr;
            return internal::kRET_E2;
        }

        ara::core::String ipcAddr{kDaemonName};
        ipcAddr    = ipcAddr + "/" + kServiceName;
        ipcClient_ = isoft::ipc::IPCClient::Create(ipcAddr.c_str());
        if (nullptr == ipcClient_) {
            std::ignore = isoft::ipc::IPCDeInitNaiUDS();
            mainLoop_   = nullptr;
            LOG().Error() << "Stub::Open isoft::ipc::IPCClient::Create(" << ipcAddr << ")";
            return internal::kRET_E3;
        }

        /// Start() will send a connection to the server side, then trigger the server's connectHandler
        if (0 != ipcClient_->Start()) {
            ipcClient_->Release();
            std::ignore = isoft::ipc::IPCDeInitNaiUDS();
            mainLoop_   = nullptr;
            ipcClient_  = nullptr;
            LOG().Error() << "Stub::Open isoft::ipc::IPCClient::Start(), errno=" << GetErrNo();
            return internal::kRET_E4;
        }
    }

    return internal::kRET_OK;
}

/// @brief close time base proxy client
/// @return 0 - success
/// @return <0 - failure
std::int32_t Stub::Close() noexcept
{
    std::int32_t const r{0};
    if (nullptr != ipcClient_) {
        std::ignore = ipcClient_->Stop();
        ipcClient_->Release();
        std::ignore = isoft::ipc::IPCDeInitNaiUDS();
        ipcClient_  = nullptr;
    }
    mainLoop_    = nullptr;
    resourceMan_ = nullptr;

    return r;
}

/// @brief trigger event
/// @param event - event type
/// @return 0 - success
/// @return <0 - failure
std::int32_t Stub::Notify(ProxyEventType const &event) noexcept
{
    std::int32_t ret{0};

    if ((event == ProxyEventType::kSetTime) || (event == ProxyEventType::kUpdateTime)
        || (event == ProxyEventType::kSetUserData) || (event == ProxyEventType::kSetRateDeviation)) {
        ret = _doIpcRequest(event, static_cast< std::uint8_t >(Message::Option::kNotify));
    } else {
        ret = kRET_E1;
    }

    return ret;
}

/// @brief register event notification
/// @param event - event
/// @param cb - callback function
/// @return 0 - success
/// @return <0 - failure
std::int32_t Stub::RegisterNotification(ProxyEventType const &event, NotificationHandler const &cb) noexcept
{
    notificationHandlers_[event] = cb;
    std::uint8_t option{0U};
    std::int32_t ret{0};

    switch (event) {
        case ProxyEventType::kSetTime:
        case ProxyEventType::kUpdateTime:
        case ProxyEventType::kSetUserData:
        case ProxyEventType::kSetRateDeviation: {
            ret = kRET_E1;
            break;
        }
        case ProxyEventType::kStatusChanged:
        case ProxyEventType::kSynchronizationStateChanged:
        case ProxyEventType::kLeapJump:
        case ProxyEventType::kPrecisionMeasurement: {
            option = static_cast< std::uint8_t >(Message::Option::kRegister);
            break;
        }
        case ProxyEventType::kValidationMeasurement: {
            option = static_cast< std::uint8_t >(Message::ValidationMeasurementOption::kRegister);
            break;
        }

        default: {
            ret = kRET_E2;
            break;
        }
    }

    if (0 == ret) {
        ret = _doIpcRequest(event, option);
    }
    return ret;
}

/// @brief unregister event notification
/// @param event - event
/// @return 0 - success
/// @return <0 - failure
std::int32_t Stub::UnregisterNotification(ProxyEventType const &event) noexcept
{
    notificationHandlers_[event] = nullptr;
    std::uint8_t option{0U};
    std::int32_t ret{0};

    switch (event) {
        case ProxyEventType::kSetTime:
        case ProxyEventType::kUpdateTime:
        case ProxyEventType::kSetUserData:
        case ProxyEventType::kSetRateDeviation: {
            ret = kRET_E1;
            break;
        }
        case ProxyEventType::kStatusChanged:
        case ProxyEventType::kSynchronizationStateChanged:
        case ProxyEventType::kLeapJump:
        case ProxyEventType::kPrecisionMeasurement: {
            option = static_cast< std::uint8_t >(Message::Option::kUnregister);
            break;
        }
        case ProxyEventType::kValidationMeasurement: {
            option = static_cast< std::uint8_t >(Message::ValidationMeasurementOption::kUnregister);
            break;
        }
        default: {
            ret = kRET_E1;
            break;
        }
    }

    if (0 == ret) {
        ret = _doIpcRequest(event, option);
    }
    return ret;
}

}  // namespace proxy
}  // namespace timebase
}  // namespace internal
}  // namespace tsync
}  // namespace ara