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
/// @file       nmipc.cpp
/// @brief      IPC communication management
/// @details
/// @date       2022-06-10
/// @author     hejunwei
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/NetworkManager/lib
/// @interface_level=unit
/// @trace_id_sr=SRS_NM_00001
/// @unit_name=NmIpc
/// @unit_description=IPC communication management
/// @endcode
///
/// ================================================================

#include <cstring>
#include <fstream>

#include "isoft/ipccpp/buffer.h"
#include "isoft/ipccpp/client.h"
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/writer.h"

#ifdef ARA_NM_INTERNAL_IPC_DEBUG
    #include "isoft/ipccpp/debug.h"
#endif
#include <isoft/ara_fsh/process.h>
#include <isoft/ipccpp/packet.h>
#include <isoft/ipccpp/utility.h>

#include "include/internal/nmipc.h"
#include "isoft/naicpp/evloop.h"

namespace ara {
namespace nm {

/// @brief Static global variable
std::shared_ptr< NmIpc > NmIpc::s_IpcClientHandler_{nullptr};  // NOLINT

/// @brief Destructor
NmIpc::~NmIpc() noexcept { _destroy(); }

/// @brief Initialize
/// @return Initialization result
std::int32_t NmIpc::_init() noexcept
{
    static_cast< void >(
        isoft::naicpp::GlobalGeneralEvLoop::Initialize(isoft::naicpp::GlobalGeneralEvLoop::Mode::kInsideThread));
    mainLoop_ = isoft::naicpp::GlobalGeneralEvLoop::Get();

#ifdef ARA_NM_INTERNAL_IPC_DEBUG
    // create ipc
    if (0 > isoft::ipc::debug::CreateIPC("nm-demo-id33")) {
        std::ignore = NmLogger().LogVerbose() << ("isoft::ipc::debug::CreateIPC() : ");
    }
    std::ignore = NmLogger().LogVerbose() << ("create ipc ");
#endif
    std::int32_t r{0};
    if (nullptr == mainLoop_) {
        NmLogger().LogError() << __func__ << "  mainloop nullptr";
        return -1;
    }
    r = isoft::ipc::IPCInitNaiUDS(mainLoop_);
    if (r == -1) {
        NmLogger().LogError() << __func__ << "   IPCInitNaiUDS failed ";
        std::ignore = isoft::ipc::IPCDeInitNaiUDS();
        return -1;
    }
    // kServiceName
    ara::core::String const ipcAddr{"nmd/CMDC"};
    ipcClient_ = isoft::ipc::IPCClient::Create(ipcAddr.c_str());
    if (nullptr == ipcClient_) {
        std::ignore = isoft::ipc::IPCDeInitNaiUDS();
        NmLogger().LogError() << __func__ << ":IPCClient::Create failed ";
        return -1;
    }
    if (0 != ipcClient_->Start()) {
        ipcClient_->Release();
        std::ignore = isoft::ipc::IPCDeInitNaiUDS();
        NmLogger().LogError() << __func__ << ":IPCClient::Start failed ";
        return -1;
    }
    NmLogger().LogVerbose() << __func__ << " ipcClient_ start ok";
    return 0;
}

/// @brief Release resources, called in destructor
void NmIpc::_destroy() noexcept
{
    if (nullptr != ipcClient_) {
        if (0 != ipcClient_->Stop()) {
            NmLogger().LogError() << "ipcClient_->Stop() failed in : " << __func__;
        }
        ipcClient_->Release();
    }

    std::ignore = isoft::ipc::IPCDeInitNaiUDS();
#ifdef ARA_NM_INTERNAL_IPC_DEBUG

    if (0 > isoft::ipc::debug::DestroyIPC("aranmlibdebuge")) {
        NmLogger().LogError() << ("isoft::ipc::debug::DestroyIPC()") << __func__;
    }
#endif
}

/// @brief Handle control commands from daemon
/// @param  jsonstr JSON string
void NmIpc::_handledaemoncmd(ara::core::String const &jsonstr) noexcept
{
    NmLogger().LogVerbose() << __func__ << "  " << jsonstr.c_str();

    rapidjson::Document d{};
    if (d.Parse(jsonstr.c_str()).HasParseError()) {
    } else {
        if (d.HasMember("cmd")) {
            ara::core::Map< internal::IpcCommand, internal::IpcCommand > notifyMap;
            notifyMap[internal::IpcCommand::kUpdateNetworkStateChange]
                = internal::IpcCommand::kRegisterNetworkStateChangeNotifier;
            notifyMap[internal::IpcCommand::kUpdateNetworkRequestedStateChange]
                = internal::IpcCommand::kRegisterNetworkRequestedStateChangeNotifier;
            ara::core::Map< internal::IpcCommand, internal::IpcCommand > etherMap;
            etherMap[internal::IpcCommand::kNotifyPresentNodeList]   = internal::IpcCommand::kNotifyPresentNodeList;
            etherMap[internal::IpcCommand::kNotifyEtherStateChanged] = internal::IpcCommand::kRegisterEtherStateChanged;
            etherMap[internal::IpcCommand::kNotifyExternalPnRequest] = internal::IpcCommand::kRegisterExternalPnRequest;
            etherMap[internal::IpcCommand::kNotifyExternalPnRelease] = internal::IpcCommand::kRegisterExternalPnRelease;

            std::int32_t const cmd{d["cmd"].GetInt()};
            internal::IpcCommand const serverCmd{static_cast< internal::IpcCommand >(cmd)};

            switch (serverCmd) {
                case internal::IpcCommand::kUpdateNetworkStateChange:
                case internal::IpcCommand::kUpdateNetworkRequestedStateChange: {
                    ara::core::String const instance{d["instance"].GetString()};
                    std::uint32_t const currentState{(d["currentState"].GetUint())};
                    LnNotifierIerator const it{lnNotifierMap_[notifyMap[serverCmd]].find(instance)};
                    if (lnNotifierMap_[notifyMap[serverCmd]].end() != it) {
                        IpcChangeNotifier const &notifier{it->second.first};
                        IpcChangeNotifierExecutor const &executor{it->second.second};
                        if (nullptr == executor) {
                            notifier(currentState);
                        } else {
                            executor(notifier, currentState);
                        }
                    }
                    break;
                }
                case internal::IpcCommand::kNotifyPresentNodeList:
                case internal::IpcCommand::kNotifyEtherStateChanged:
                case internal::IpcCommand::kNotifyExternalPnRequest:
                case internal::IpcCommand::kNotifyExternalPnRelease: {
                    ara::core::String const instance{d["instance"].GetString()};
                    ara::core::String const content{d["content"].GetString()};
                    EtherNotifierIerator const it{etherNotifierMap_[etherMap[serverCmd]].find(instance)};
                    if (etherNotifierMap_[etherMap[serverCmd]].end() != it) {
                        EtherNotifier const &notifier{it->second.first};
                        EtherNotifierExecutor const &executor{it->second.second};
                        if (nullptr == executor) {
                            notifier(content);
                        } else {
                            executor(notifier, content);
                        }

                        if (internal::IpcCommand::kNotifyPresentNodeList == serverCmd) {
                            std::ignore = etherNotifierMap_[etherMap[serverCmd]].erase(instance);
                        }
                    }
                    break;
                }
                default: {
                    break;
                }
            }
        }
    }
}

/// @brief IPC asynchronous send callback function
/// @param  context - User parameter
/// @param  status - IPC communication status
/// @param  responsePacket - the packet responsed from peer
void NmIpc::IPCClientSendAsyncHandler(void *const context,
                                      isoft::ipc::IPCClientHandlerStatus const status,
                                      isoft::ipc::IPCPacket *const responsePacket) noexcept
{
    NmLogger().LogVerbose() << __func__ << " :enter :";
    isoft::ipc::IPCPacketBufferReference *buffer{nullptr};

    NmIpc *const client{static_cast< NmIpc * >(context)};
    if (nullptr == client) {
        NmLogger().LogError() << __func__ << " nullptr == client";
        return;
    }
    if (status == IPC_CLIENT_HANDLER_STATUS_ERR) {
        NmLogger().LogError() << __func__ << " status == IPC_CLIENT_HANDLER_STATUS_ERR";
        return;
    }
    if (nullptr == responsePacket) {
        NmLogger().LogError() << __func__ << "nullptr == responsePacket";
        return;
    }
    buffer = responsePacket->GetBuffer();
    if (nullptr == buffer) {
        NmLogger().LogError() << __func__ << "nullptr == buffer";
        return;
    }
    std::size_t const len{buffer->GetLen()};
    ara::core::String const jsonPacket(reinterpret_cast< char const * >(buffer->GetPtr()), len);
    client->_handledaemoncmd(jsonPacket);
}

/// @brief IPC synchronous request
/// @param cmd Request JSON string
/// @param serverCode Server return code
/// @return IPC communication result
std::int32_t NmIpc::DoIpcSyncRequest(ara::core::String const &cmd, std::uint32_t &serverCode) noexcept
{
    if (nullptr == ipcClient_) {
        return -1;
    }
    isoft::ipc::IPCPacket *const ipcRequest{ipcClient_->MakeRequest()};
    if (nullptr == ipcRequest) {
        return internal::kNmConstNegactive2;
    }
    isoft::ipc::IPCPacketBufferReference *const reqBuffer{ipcRequest->AppendBuffer(cmd.length())};
    if (nullptr == reqBuffer) {
        return internal::kNmConstNegactive3;
    }
    std::ignore = memcpy(reqBuffer->GetPtr(), cmd.c_str(), cmd.length());
    std::ignore = reqBuffer->SetLen(cmd.length());
    /// true means subscription mode, requires multiple replies, -1 means no timeout set
    std::int32_t r{0};
    isoft::ipc::IPCPacket *response{nullptr};
    NmLogger().LogDebug() << "DoIpcSyncRequest SendSync, cmd=" << cmd.c_str();
    r = ipcClient_->SendSync(ipcRequest, &response, kNmIpcTimeout);
    if ((r < 0) || (nullptr == response)) {
        NmLogger().LogError() << __func__ << "ipc send error :" << r;
        return internal::kNmConstNegactive4;
    }
    isoft::ipc::IPCPacketBufferReference *const ipcBuffer{response->GetBuffer()};
    if (nullptr == ipcBuffer) {
        NmLogger().LogError() << __func__ << "response->GetBuffer nullptr :";
        return internal::kNmConstNegactive5;
    }
    std::ignore = serverCode;
    serverCode  = *reinterpret_cast< std::uint32_t * >(ipcBuffer->GetPtr());
    std::ignore = isoft::ipc::IPCPacket::Release(response);
    return 0;
}

/// @brief IPC synchronous request
/// @param cmd Request JSON string
/// @param serverStr Server return string
/// @return IPC communication result
std::int32_t NmIpc::DoIpcSyncRequest(ara::core::String const &cmd, ara::core::String &serverStr) noexcept
{
    if (nullptr == ipcClient_) {
        return -1;
    }
    isoft::ipc::IPCPacket *const ipcRequest{ipcClient_->MakeRequest()};
    if (nullptr == ipcRequest) {
        return internal::kNmConstNegactive2;
    }
    isoft::ipc::IPCPacketBufferReference *const reqBuffer{ipcRequest->AppendBuffer(cmd.length())};
    if (nullptr == reqBuffer) {
        return internal::kNmConstNegactive3;
    }
    std::ignore = memcpy(reqBuffer->GetPtr(), cmd.c_str(), cmd.length());
    std::ignore = reqBuffer->SetLen(cmd.length());
    /// true means subscription mode, requires multiple replies, -1 means no timeout set
    std::int32_t r{0};
    isoft::ipc::IPCPacket *response{nullptr};
    NmLogger().LogDebug() << "DoIpcSyncRequest SendSync, cmd=" << cmd.c_str();
    r = ipcClient_->SendSync(ipcRequest, &response, kNmIpcTimeout);
    if ((r < 0) || (nullptr == response)) {
        NmLogger().LogVerbose() << __func__ << "ipc send error :" << r;
        return internal::kNmConstNegactive4;
    }
    isoft::ipc::IPCPacketBufferReference *const ipcBuffer{response->GetBuffer()};
    if (nullptr == ipcBuffer) {
        NmLogger().LogVerbose() << __func__ << "response->GetBuffer nullptr :";
        return internal::kNmConstNegactive5;
    }
    std::ignore = serverStr;
    ara::core::String const serverRet{reinterpret_cast< char * >(ipcBuffer->GetPtr())};
    serverStr   = serverRet;
    std::ignore = isoft::ipc::IPCPacket::Release(response);
    return 0;
}

/// @brief Send asynchronous IPC notification to server
/// @param instance Ethernet instance
/// @param cmd Listen type
/// @return Processing result
std::int32_t NmIpc::_sendAsyncIpcNotification(ara::core::String const &instance,
                                              internal::IpcCommand const cmd) noexcept
{
    if (nullptr == ipcClient_) {
        return -1;
    }
    rapidjson::Document document;
    std::ignore = document.SetObject();
    rapidjson::Document::AllocatorType &rapidallocator{document.GetAllocator()};

    std::ignore = document.AddMember(std::move(rapidjson::GenericStringRef< NmChar >("cmd")),
                                     static_cast< std::int32_t >(cmd), document.GetAllocator());
    std::ignore = document.AddMember(std::move(rapidjson::GenericStringRef< NmChar >("instance")),
                                     rapidjson::Value().SetString(instance.c_str(), rapidallocator), rapidallocator);

    // Convert Document to string
    rapidjson::StringBuffer buffer;
    rapidjson::Writer< rapidjson::StringBuffer > writer{buffer};
    std::ignore = document.Accept(writer);

    ara::core::String sendstr{buffer.GetString()};
    std::ignore = sendstr.append("\n");
    isoft::ipc::IPCPacket *const ipcRequest{ipcClient_->MakeRequest()};
    if (nullptr == ipcRequest) {
        return internal::kNmConstNegactive2;
    }
    isoft::ipc::IPCPacketBufferReference *const reqBuffer{ipcRequest->AppendBuffer(sendstr.length())};
    if (nullptr == reqBuffer) {
        return internal::kNmConstNegactive3;
    }

    std::ignore = memcpy(reqBuffer->GetPtr(), sendstr.c_str(), sendstr.length());
    std::ignore = reqBuffer->SetLen(sendstr.length());
    /// true means subscription mode, requires multiple replies, -1 means no timeout set
    std::int32_t r{0};
    NmLogger().LogDebug() << "_sendAsyncIpcNotification SendAsync, cmd=" << sendstr.c_str();
    isoft::ipc::IPCClientHandler const ipcHandler{[this](void *const context,
                                                         isoft::ipc::IPCClientHandlerStatus const status,
                                                         isoft::ipc::IPCPacket *const responsePacket) noexcept -> void {
        this->IPCClientSendAsyncHandler(context, status, responsePacket);
    }};
    r = ipcClient_->SendAsync(ipcRequest, true, ipcHandler, -1, this);
    if (0 != r) {
        NmLogger().LogError() << __func__
                              << "_sendAsyncIpcNotification send error, cmd=" << static_cast< std::int32_t >(cmd)
                              << ", ret=" << r;
    }
    return r;
}

/// @brief Register logical network notifier
/// @param instance Logical network instance
/// @param cmd Listen type
/// @param notifier Notification handler
/// @return Whether registration succeeded
std::int32_t NmIpc::RegisterNotification(ara::core::String const &instance,
                                         internal::IpcCommand const cmd,
                                         IpcLnNotifierPair const &notifier) noexcept
{
    std::int32_t ret{0};
    LnNotifierIerator const lnIerator{lnNotifierMap_[cmd].find(instance)};
    if (lnNotifierMap_[cmd].end() == lnIerator) {
        ret = _sendAsyncIpcNotification(instance, cmd);
    }
    if (0 == ret) {
        lnNotifierMap_[cmd][instance] = notifier;
    }
    return ret;
}

/// @brief Register Ethernet instance notifier
/// @param instance Ethernet instance
/// @param cmd Listen type
/// @param notifier Notification handler
/// @return Whether registration succeeded
std::int32_t NmIpc::RegisterNotification(ara::core::String const &instance,
                                         internal::IpcCommand const cmd,
                                         IpcEtherNotifierPair const &notifier) noexcept
{
    std::int32_t ret{0};
    EtherNotifierIerator const etherIerator{etherNotifierMap_[cmd].find(instance)};
    if (etherNotifierMap_[cmd].end() == etherIerator) {
        ret = _sendAsyncIpcNotification(instance, cmd);
    }
    if (0 == ret) {
        etherNotifierMap_[cmd][instance] = notifier;
    }
    return ret;
}

/// @brief Unregister notifier
/// @param instance Instance
/// @param cmd Listen type
/// @return Whether unregistration succeeded
std::int32_t NmIpc::UnregisterNotification(ara::core::String const &instance, internal::IpcCommand const cmd) noexcept
{
    ara::core::Map< internal::IpcCommand, internal::IpcCommand > clearMap;
    clearMap[internal::IpcCommand::kUnregisterNetworkStateChangeNotifier]
        = internal::IpcCommand::kRegisterNetworkStateChangeNotifier;
    clearMap[internal::IpcCommand::kUnregisterNetworkRequestedStateChangeNotifier]
        = internal::IpcCommand::kRegisterNetworkRequestedStateChangeNotifier;
    std::ignore = lnNotifierMap_[clearMap[cmd]].erase(instance);
    std::int32_t const ret{_sendAsyncIpcNotification(instance, cmd)};
    if (0 != ret) {
        NmLogger().LogError() << __func__ << "UnregisterNotification error";
    }
    return ret;
}

}  // namespace nm
}  // namespace ara
