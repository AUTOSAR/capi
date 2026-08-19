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
/// @file       nmipcproc.cpp
/// @brief      IPC communication management
/// @details
/// @date       2022-06-10
/// @author     hejunwei
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @interface_level=Module
/// @trace_id_sr=SRS_NM_00004,SRS_NM_00005,SRS_NM_00014,SRS_NM_00015,SRS_NM_00016,SRS_NM_00019,SRS_NM_00020,SRS_NM_00021,SRS_NM_00022,SRS_NM_00023,SRS_NM_00024
/// @unit_name=NMIpcProc
/// @unit_description=IPC communication management
/// @module_path=/NetworkManager/smi
/// @endcode
///
/// ================================================================

#include <algorithm>
#include <functional>
#include <memory>

#ifdef ARA_NM_INTERNAL_IPC_DEBUG
    #include <isoft/ipccpp/debug.h>
#endif  // DEBUG
#include <isoft/naicpp/evloop.h>
#include <rapidjson/document.h>
#include <rapidjson/rapidjson.h>
#include <rapidjson/writer.h>

#include "common/common.h"
#include "include/nmipcproc.h"
#include "utils/include/utils.h"

namespace ara {
namespace nm {
namespace internal {

/// @brief Global variable declaration
std::shared_ptr< NMIpcProc > NMIpcProc::s_IpcServerHandler_{nullptr};  // NOLINT

/// @brief Destructor
NMIpcProc::~NMIpcProc() noexcept
{
#ifdef ARA_NM_INTERNAL_IPC_DEBUG

    if (0 > isoft::ipc::debug::DestroyIPC(kDaemonName)) {
        std::ignore =
    }
    // Clean up the folder used for IPC communication
    if (0 != isoft::ipc::debug::IPCEnvDestroy()) {
        std::ignore =
    }
#endif

    if (nullptr != ipcServer_) {
        if (0 != ipcServer_->Stop()) {
            NmLogger().LogError() << "isoft::ipc::IPCClient::Stop(), errno=";
        }
        ipcServer_->Release();
    }
    if (-1 == isoft::ipc::IPCDeInitNaiUDS()) {
        NmLogger().LogError() << "IPCDeInitNaiUDS(), errno=";
    }
}

/// @brief Initialize
/// @return 0 ok  <0failed
std::int32_t NMIpcProc::_init() noexcept
{
    NmLogger().LogVerbose() << "NMIpcProc::_init enter";
    mainLoop_ = isoft::naicpp::GlobalGeneralEvLoop::Get();

    if (nullptr == mainLoop_) {
        NmLogger().LogError() << "nullptr == mainLoop";
        return -1;
    }
#ifdef ARA_NM_INTERNAL_IPC_DEBUG
    if (0 != isoft::ipc::debug::IPCEnvDestroy()) {
        std::ignore = return -1;
    }

    // Create folder for IPC communication
    if (0 != isoft::ipc::debug::IPCEnvInit()) {
        std::ignore = return -1;
    }

    // create ipc
    if (0 > isoft::ipc::debug::CreateIPC(kDaemonName.c_str())) {
        std::ignore = return -1;
    }
#endif
    //////////////////////  Open IPC server  ////////////////
    std::int32_t const r{isoft::ipc::IPCInitNaiUDS(mainLoop_)};
    if (0 > r) {
        NmLogger().LogError() << "IPCInitNaiUDS error:  ";
        return -1;
    }
    ara::core::String const kServiceName{"CMDC"};
    ipcServer_ = isoft::ipc::IPCServer::Create(kServiceName.c_str());
    if (nullptr == ipcServer_) {
        NmLogger().LogError() << "isoft::ipc::IPCClient::Create(" << kServiceName.c_str() << ")";
        return -1;
    }
    isoft::ipc::IPCServerHandler const ipcConntHandler{
        [this](void *const context, isoft::ipc::IPCServerHandleType const status,
               isoft::ipc::IPCPacket *const responsePacket) noexcept -> void {
            this->_ipcServerConnectionHandler(context, status, responsePacket);
        }};
    isoft::ipc::IPCServerHandler const ipcReqHandler{
        [this](void *const context, isoft::ipc::IPCServerHandleType const status,
               isoft::ipc::IPCPacket *const responsePacket) noexcept -> void {
            this->_ipcServerRequestHandler(context, status, responsePacket);
        }};
    ipcServer_->SetHandler(IPC_SERVER_HANDLER_CONNECT, ipcConntHandler, this);
    ipcServer_->SetHandler(IPC_SERVER_HANDLER_DISCONNECT, ipcConntHandler, this);

    ipcServer_->SetHandler(IPC_SERVER_HANDLER_MULTI, ipcReqHandler, this);
    ipcServer_->SetHandler(IPC_SERVER_HANDLER_ONCE, ipcReqHandler, this);

    if (0 != ipcServer_->Start()) {
        return -1;
    }
    NmLogger().LogVerbose() << "NMIpcProc::_init finish";

    return 0;
}

/// @brief Send data
/// @param  sessionid  Session ID
/// @param  dataBuffer  Data pointer
/// @param  messageSize Data length
/// @return 0 if send succeeded
std::int32_t NMIpcProc::_send(std::uint64_t const &sessionid,
                              char const *const dataBuffer,
                              std::size_t const &messageSize) const noexcept
{
    NmLogger().LogVerbose() << "NMIpcProc::_send, Send content=" << dataBuffer << ", sessionid=" << sessionid;
    isoft::ipc::IPCPacketBufferReference *resBuffer{nullptr};
    isoft::ipc::IPCPacket *const responsePacket{ipcServer_->MakeResponse(sessionid)};
    if (nullptr == responsePacket) {
        return -1;
    }

    resBuffer = responsePacket->AppendBuffer(messageSize);
    if (nullptr == resBuffer) {
        return -1;
    }
    static_cast< void >(resBuffer->SetLen(messageSize));
    std::ignore = memcpy(resBuffer->GetPtr(), dataBuffer, messageSize);
    /// Parameter false means it is not the last message
    if (0 != this->ipcServer_->Send(responsePacket, false)) {
        return -1;
    }

    return 0;
}

/// @brief  Delete session from set
/// @param  vectorRef Set
/// @param  session ipc session
void NMIpcProc::DeleteSession(ara::core::Vector< std::uint64_t > &vectorRef, std::uint64_t const session) noexcept
{
    ara::core::Vector< std::uint64_t >::const_iterator it{std::find(vectorRef.cbegin(), vectorRef.cend(), session)};
    if (vectorRef.end() != it) {
        std::ignore = vectorRef.erase(std::move(it));
    }
}

/// @brief  Add session to set
/// @param  vectorRef Set
/// @param  session ipc session
void NMIpcProc::AddSession(ara::core::Vector< std::uint64_t > &vectorRef, std::uint64_t const session) noexcept
{
    ara::core::Vector< std::uint64_t >::iterator const it{std::find(vectorRef.begin(), vectorRef.end(), session)};
    if (vectorRef.end() == it) {
        vectorRef.emplace_back(session);
    }
}

/// @brief Handle client request
/// @param jsonstr Request
/// @param sessionId ipc session
void NMIpcProc::_handleLibCmd(ara::core::String const &jsonstr, std::uint64_t const &sessionId) noexcept
{
    rapidjson::Document d;
    NmLogger().LogVerbose() << "_handleLibCmd: " << jsonstr.c_str() << ", sessionId=" << sessionId;
    if (d.Parse(jsonstr.c_str()).HasParseError()) {
        NmLogger().LogError() << "_handleLibCmd Parse error:  ";
    } else {
        bool const cmdOK{d["cmd"].IsInt()};
        if (d.HasMember("cmd") && cmdOK) {
            std::int32_t const cmd{d["cmd"].GetInt()};
            ara::core::String const instance{d["instance"].GetString()};
            switch (cmd) {
                case static_cast< std::int32_t >(IpcCommand::kGetNetworkRequestState): {
                    _getNetworkRequestState(instance, sessionId);
                    break;
                }
                case static_cast< std::int32_t >(IpcCommand::kSetNetworkRequestState): {
                    ara::nm::NetworkStateType const requestState{
                        static_cast< ara::nm::NetworkStateType >(d["requeststate"].GetUint())};
                    _setNetworkRequestState(instance, sessionId, requestState);
                    break;
                }
                case static_cast< std::int32_t >(IpcCommand::kGetNetworkCurrentState): {
                    _getNetworkCurrentState(instance, sessionId);
                    break;
                }
                case static_cast< std::int32_t >(IpcCommand::kRegisterNetworkStateChangeNotifier): {
                    AddSession(lnRegisterMap_[IpcCommand::kRegisterNetworkStateChangeNotifier][instance], sessionId);
                    break;
                }
                case static_cast< std::int32_t >(IpcCommand::kUnregisterNetworkStateChangeNotifier): {
                    DeleteSession(lnRegisterMap_[IpcCommand::kRegisterNetworkStateChangeNotifier][instance], sessionId);
                    break;
                }
                case static_cast< std::int32_t >(IpcCommand::kRegisterNetworkRequestedStateChangeNotifier): {
                    AddSession(lnRegisterMap_[IpcCommand::kRegisterNetworkRequestedStateChangeNotifier][instance],
                               sessionId);
                    break;
                }
                case static_cast< std::int32_t >(IpcCommand::kUnregisterNetworkRequestedStateChangeNotifier): {
                    DeleteSession(lnRegisterMap_[IpcCommand::kRegisterNetworkRequestedStateChangeNotifier][instance],
                                  sessionId);
                    break;
                }
                case static_cast< std::int32_t >(IpcCommand::kSetEtherState): {
                    ara::nm::NetworkStateType const requestState{
                        static_cast< ara::nm::NetworkStateType >(d["requeststate"].GetUint())};
                    _setEtherState(instance, sessionId, requestState);
                    break;
                }
                case static_cast< std::int32_t >(IpcCommand::kGetEtherState): {
                    _getEtherState(instance, sessionId);
                    break;
                }
                case static_cast< std::int32_t >(IpcCommand::kRequestDetectNode): {
                    _requestDetectNode(instance, sessionId);
                    break;
                }
                case static_cast< std::int32_t >(IpcCommand::kNotifyWakeup): {
                    _notifyWakeup(instance, sessionId);
                    break;
                }
                case static_cast< std::int32_t >(IpcCommand::kSetMessageCtrlType): {
                    ara::nm::MessageCtrlType const requestType{
                        static_cast< ara::nm::MessageCtrlType >(d["requesttype"].GetUint())};
                    _setMessageCtrlType(instance, sessionId, requestType);
                    break;
                }
                case static_cast< std::int32_t >(IpcCommand::kGetMessageCtrlType): {
                    _getMessageCtrlType(instance, sessionId);
                    break;
                }
                case static_cast< std::int32_t >(IpcCommand::kGetInitState): {
                    _getInitState(instance, sessionId);
                    break;
                }
                case static_cast< std::int32_t >(IpcCommand::kGetExternalPnRequestList): {
                    _getExternalPnRequestList(instance, sessionId);
                    break;
                }
                case static_cast< std::int32_t >(IpcCommand::kRegisterEtherStateChanged): {
                    AddSession(ethRegisterMap_[IpcCommand::kRegisterEtherStateChanged][instance], sessionId);
                    break;
                }
                case static_cast< std::int32_t >(IpcCommand::kUnregisterEtherStateChanged): {
                    DeleteSession(ethRegisterMap_[IpcCommand::kRegisterEtherStateChanged][instance], sessionId);
                    break;
                }
                case static_cast< std::int32_t >(IpcCommand::kRegisterExternalPnRequest): {
                    AddSession(ethRegisterMap_[IpcCommand::kRegisterExternalPnRequest][instance], sessionId);
                    break;
                }
                case static_cast< std::int32_t >(IpcCommand::kUnregisterExternalPnRequest): {
                    DeleteSession(ethRegisterMap_[IpcCommand::kRegisterExternalPnRequest][instance], sessionId);
                    break;
                }
                case static_cast< std::int32_t >(IpcCommand::kRegisterExternalPnRelease): {
                    AddSession(ethRegisterMap_[IpcCommand::kRegisterExternalPnRelease][instance], sessionId);
                    break;
                }
                case static_cast< std::int32_t >(IpcCommand::kUnregisterExternalPnRelease): {
                    DeleteSession(ethRegisterMap_[IpcCommand::kRegisterExternalPnRelease][instance], sessionId);
                    break;
                }
                case static_cast< std::int32_t >(IpcCommand::kNotifyPresentNodeList): {
                    AddSession(ethRegisterMap_[IpcCommand::kNotifyPresentNodeList][instance], sessionId);
                    break;
                }
                default: {
                    break;
                }
            }
        }
    }
}

/// @brief IPC request callback function, called when a request is received from a client.
/// @param  context - User parameter
/// @param  htype - Handle type
/// @param  reqPacket - Data packet
void NMIpcProc::_ipcServerRequestHandler(void *const context,
                                         isoft::ipc::IPCServerHandleType const &htype,
                                         isoft::ipc::IPCPacket *const reqPacket) noexcept
{
    std::ignore = htype;
    std::ignore = context;

    isoft::ipc::IPCSessionId sessionId{0U};
    isoft::ipc::IPCPacketBufferReference *reqBuffer{nullptr};
    NmLogger().LogVerbose() << "NMIpcProc::_ipcServerRequestHandler enter";
    if (nullptr == reqPacket) {
        NmLogger().LogError() << "NMIpcProc::_ipcServerRequestHandler nullptr == reqPacket";
        return;
    }
    sessionId = reqPacket->GetSessionId();
    reqBuffer = reqPacket->GetBuffer();  // request buffer
    ara::core::String const jsonPacket(reinterpret_cast< const char * >(reqBuffer->GetPtr()), reqBuffer->GetLen());
    _handleLibCmd(jsonPacket, sessionId);
}

/// @brief IPC service connection callback function, called when a client establishes or disconnects.
/// @param  context - User parameter
/// @param  type - Type
/// @param  packet - Data packet
void NMIpcProc::_ipcServerConnectionHandler(void *const context,
                                            isoft::ipc::IPCServerHandleType const &type,
                                            isoft::ipc::IPCPacket *const packet)
{
    std::ignore = packet;
    std::ignore = context;
    NmLogger().LogVerbose() << "_ipcServerConnectionHandler: type=" << static_cast< std::uint32_t >(type);
    if (type == IPC_SERVER_HANDLER_DISCONNECT) {
        _delAAClient(packet->GetSessionId());
    } else {
    }
}

/// @brief Update network state
/// @param lnName LN instance
/// @param currentState Network state
void NMIpcProc::UpdateNetworkCurrentState(ara::core::String const &lnName, std::uint32_t const currentState) noexcept
{
    NmLogger().LogVerbose() << "NMIpcProc::UpdateNetworkCurrentState,lnName" << lnName.c_str()
                            << ",currentState=" << currentState;
    rapidjson::Document document;
    std::ignore = document.SetObject();
    rapidjson::Document::AllocatorType &rapidallocator{document.GetAllocator()};

    std::ignore = document.AddMember(std::move(rapidjson::GenericStringRef< NmChar >("cmd")),
                                     static_cast< std::int32_t >(IpcCommand::kUpdateNetworkStateChange),
                                     document.GetAllocator());
    std::ignore = document.AddMember(std::move(rapidjson::GenericStringRef< NmChar >("instance")),
                                     rapidjson::Value().SetString(lnName.c_str(), rapidallocator), rapidallocator);
    std::ignore = document.AddMember(std::move(rapidjson::GenericStringRef< NmChar >("currentState")), currentState,
                                     document.GetAllocator());

    // Convert Document to string
    rapidjson::StringBuffer buffer;
    rapidjson::Writer< rapidjson::StringBuffer > writer{buffer};
    std::ignore = document.Accept(writer);

    ara::core::String sendstr{buffer.GetString()};
    std::ignore = sendstr.append("\n");

    ara::core::Vector< std::uint64_t > &vectorRef{
        lnRegisterMap_[IpcCommand::kRegisterNetworkStateChangeNotifier][lnName]};
    std::ignore = for_each(vectorRef.begin(), vectorRef.end(),
                           [&sendstr, this](std::uint64_t const sessionID) noexcept -> void {
                               std::int32_t const ret{this->_send(sessionID, (sendstr.c_str()), sendstr.length())};
                               if (0 != ret) {
                                   NmLogger().LogError() << "NMIpcProc::UpdateNetworkCurrentState, Send ret=" << ret;
                               }
                           });
}

/// @brief Update network request state
/// @param lnName LN instance
/// @param currentState Network state
void NMIpcProc::UpdateNetworkRequestState(ara::core::String const &lnName, std::uint32_t const currentState) noexcept
{
    NmLogger().LogVerbose() << "NMIpcProc::UpdateNetworkRequestState,lnName" << lnName.c_str()
                            << ",currentState=" << currentState;
    rapidjson::Document document;
    std::ignore = document.SetObject();
    rapidjson::Document::AllocatorType &rapidallocator{document.GetAllocator()};

    std::ignore = document.AddMember(std::move(rapidjson::GenericStringRef< NmChar >("cmd")),
                                     static_cast< std::int32_t >(IpcCommand::kUpdateNetworkRequestedStateChange),
                                     document.GetAllocator());
    std::ignore = document.AddMember(std::move(rapidjson::GenericStringRef< NmChar >("instance")),
                                     rapidjson::Value().SetString(lnName.c_str(), rapidallocator), rapidallocator);
    std::ignore = document.AddMember(std::move(rapidjson::GenericStringRef< NmChar >("currentState")), currentState,
                                     document.GetAllocator());

    // Convert Document to string
    rapidjson::StringBuffer buffer;
    rapidjson::Writer< rapidjson::StringBuffer > writer{buffer};
    std::ignore = document.Accept(writer);

    ara::core::String sendstr{buffer.GetString()};
    std::ignore = sendstr.append("\n");

    ara::core::Vector< std::uint64_t > &vectorRef{
        lnRegisterMap_[IpcCommand::kRegisterNetworkRequestedStateChangeNotifier][lnName]};
    std::ignore = for_each(vectorRef.begin(), vectorRef.end(),
                           [&sendstr, this](std::uint64_t const sessionID) noexcept -> void {
                               std::int32_t const ret{this->_send(sessionID, (sendstr.c_str()), sendstr.length())};
                               if (0 != ret) {
                                   NmLogger().LogError() << "NMIpcProc::UpdateNetworkRequestState, Send ret=" << ret;
                               }
                           });
}

/// @brief Notify node detection result
/// @param ipAddress Ethernet instance address
/// @param nodeList Node set
void NMIpcProc::NotifyPresentNodeList(ara::core::String const &ipAddress, ara::core::String const &nodeList) noexcept
{
    NmLogger().LogVerbose() << "NMIpcProc::NotifyPresentNodeList,ipAddress=" << ipAddress.c_str()
                            << ",nodeList=" << nodeList.c_str();
    rapidjson::Document document;
    std::ignore = document.SetObject();
    rapidjson::Document::AllocatorType &rapidallocator{document.GetAllocator()};

    std::ignore
        = document.AddMember(std::move(rapidjson::GenericStringRef< NmChar >("cmd")),
                             static_cast< std::int32_t >(IpcCommand::kNotifyPresentNodeList), document.GetAllocator());
    std::ignore = document.AddMember(std::move(rapidjson::GenericStringRef< NmChar >("instance")),
                                     rapidjson::Value().SetString(ipAddress.c_str(), rapidallocator), rapidallocator);
    std::ignore = document.AddMember(std::move(rapidjson::GenericStringRef< NmChar >("content")),
                                     rapidjson::Value().SetString(nodeList.c_str(), rapidallocator), rapidallocator);

    // Convert Document to string
    rapidjson::StringBuffer buffer;
    rapidjson::Writer< rapidjson::StringBuffer > writer{buffer};
    std::ignore = document.Accept(writer);

    ara::core::String sendstr{buffer.GetString()};
    std::ignore = sendstr.append("\n");

    ara::core::Vector< std::uint64_t > &vectorRef{ethRegisterMap_[IpcCommand::kNotifyPresentNodeList][ipAddress]};
    std::ignore = for_each(vectorRef.begin(), vectorRef.end(),
                           [&sendstr, this](std::uint64_t const sessionID) noexcept -> void {
                               std::int32_t const ret{this->_send(sessionID, (sendstr.c_str()), sendstr.length())};
                               if (0 != ret) {
                                   NmLogger().LogError() << "NMIpcProc::NotifyPresentNodeList, Send ret=" << ret;
                               }
                           });
    std::ignore = ethRegisterMap_[IpcCommand::kNotifyPresentNodeList].erase(ipAddress);
}

/// @brief Notify state machine state change
/// @param ipAddress Ethernet instance address
/// @param currentState State
void NMIpcProc::NotifyEtherStateChanged(ara::core::String const &ipAddress, std::uint32_t const currentState) noexcept
{
    NmLogger().LogVerbose() << "NMIpcProc::NotifyEtherStateChanged,lnName" << ipAddress.c_str()
                            << ",currentState=" << currentState;
    rapidjson::Document document;
    std::ignore = document.SetObject();
    rapidjson::Document::AllocatorType &rapidallocator{document.GetAllocator()};

    std::ignore = document.AddMember(rapidjson::GenericStringRef< NmChar >("cmd"),
                                     static_cast< std::int32_t >(IpcCommand::kNotifyEtherStateChanged),
                                     document.GetAllocator());
    std::ignore = document.AddMember(std::move(rapidjson::GenericStringRef< NmChar >("instance")),
                                     rapidjson::Value().SetString(ipAddress.c_str(), rapidallocator), rapidallocator);
    ara::core::String const content{ara::core::to_string(currentState)};
    std::ignore = document.AddMember(std::move(rapidjson::GenericStringRef< NmChar >("content")),
                                     rapidjson::Value().SetString(content.c_str(), rapidallocator), rapidallocator);

    // Convert Document to string
    rapidjson::StringBuffer buffer;
    rapidjson::Writer< rapidjson::StringBuffer > writer{buffer};
    std::ignore = document.Accept(writer);

    ara::core::String sendstr{buffer.GetString()};
    std::ignore = sendstr.append("\n");

    ara::core::Vector< std::uint64_t > &vectorRef{ethRegisterMap_[IpcCommand::kRegisterEtherStateChanged][ipAddress]};
    std::ignore = for_each(vectorRef.begin(), vectorRef.end(),
                           [&sendstr, this](std::uint64_t const sessionID) noexcept -> void {
                               std::int32_t const ret{this->_send(sessionID, (sendstr.c_str()), sendstr.length())};
                               if (0 != ret) {
                                   NmLogger().LogError() << "NMIpcProc::NotifyEtherStateChanged, Send ret=" << ret;
                               }
                           });
}

/// @brief Notify receipt of external PN request
/// @param ipAddress Ethernet instance address
/// @param pnList PN set
void NMIpcProc::NotifyExternalPnRequest(ara::core::String const &ipAddress, ara::core::String const &pnList) noexcept
{
    NmLogger().LogVerbose() << "NMIpcProc::NotifyExternalPnRequest,ipAddress=" << ipAddress.c_str()
                            << ",pnList=" << pnList.c_str();
    rapidjson::Document document;
    std::ignore = document.SetObject();
    rapidjson::Document::AllocatorType &rapidallocator{document.GetAllocator()};

    std::ignore = document.AddMember(std::move(rapidjson::GenericStringRef< NmChar >("cmd")),
                                     static_cast< std::int32_t >(IpcCommand::kNotifyExternalPnRequest),
                                     document.GetAllocator());
    std::ignore = document.AddMember(std::move(rapidjson::GenericStringRef< NmChar >("instance")),
                                     rapidjson::Value().SetString(ipAddress.c_str(), rapidallocator), rapidallocator);
    std::ignore = document.AddMember(std::move(rapidjson::GenericStringRef< NmChar >("content")),
                                     rapidjson::Value().SetString(pnList.c_str(), rapidallocator), rapidallocator);

    // Convert Document to string
    rapidjson::StringBuffer buffer;
    rapidjson::Writer< rapidjson::StringBuffer > writer{buffer};
    std::ignore = document.Accept(writer);

    ara::core::String sendstr{buffer.GetString()};
    std::ignore = sendstr.append("\n");

    ara::core::Vector< std::uint64_t > &vectorRef{ethRegisterMap_[IpcCommand::kRegisterExternalPnRequest][ipAddress]};
    std::ignore = for_each(vectorRef.begin(), vectorRef.end(),
                           [&sendstr, this](std::uint64_t const sessionID) noexcept -> void {
                               std::int32_t const ret{this->_send(sessionID, (sendstr.c_str()), sendstr.length())};
                               if (0 != ret) {
                                   NmLogger().LogError() << "NMIpcProc::NotifyExternalPnRequest, Send ret=" << ret;
                               }
                           });
}

/// @brief Notify receipt of external PN release
/// @param ipAddress Ethernet instance address
/// @param pnList PN set
void NMIpcProc::NotifyExternalPnRelease(ara::core::String const &ipAddress, ara::core::String const &pnList) noexcept
{
    NmLogger().LogVerbose() << "NMIpcProc::NotifyExternalPnRelease,ipAddress=" << ipAddress.c_str()
                            << ",pnList=" << pnList.c_str();
    rapidjson::Document document;
    std::ignore = document.SetObject();
    rapidjson::Document::AllocatorType &rapidallocator{document.GetAllocator()};

    std::ignore = document.AddMember(std::move(rapidjson::GenericStringRef< NmChar >("cmd")),
                                     static_cast< std::int32_t >(IpcCommand::kNotifyExternalPnRelease),
                                     document.GetAllocator());
    std::ignore = document.AddMember(std::move(rapidjson::GenericStringRef< NmChar >("instance")),
                                     rapidjson::Value().SetString(ipAddress.c_str(), rapidallocator), rapidallocator);
    std::ignore = document.AddMember(std::move(rapidjson::GenericStringRef< NmChar >("content")),
                                     rapidjson::Value().SetString(pnList.c_str(), rapidallocator), rapidallocator);

    // Convert Document to string
    rapidjson::StringBuffer buffer;
    rapidjson::Writer< rapidjson::StringBuffer > writer{buffer};
    std::ignore = document.Accept(writer);

    ara::core::String sendstr{buffer.GetString()};
    std::ignore = sendstr.append("\n");

    ara::core::Vector< std::uint64_t > &vectorRef{ethRegisterMap_[IpcCommand::kRegisterExternalPnRelease][ipAddress]};
    std::ignore = for_each(vectorRef.begin(), vectorRef.end(),
                           [&sendstr, this](std::uint64_t const sessionID) noexcept -> void {
                               std::int32_t const ret{this->_send(sessionID, (sendstr.c_str()), sendstr.length())};
                               if (0 != ret) {
                                   NmLogger().LogError() << "NMIpcProc::NotifyExternalPnRelease, Send ret=" << ret;
                               }
                           });
}

/// @brief Client offline, delete client session
/// @param  sessionId ipc session
void NMIpcProc::_delAAClient(std::uint64_t const &sessionId) noexcept { std::ignore = sessionId; }

/// @brief Client gets logical network request state
/// @param  lnName Logical network instance
/// @param  sessionId ipc session
void NMIpcProc::_getNetworkRequestState(ara::core::String const &lnName, std::uint64_t const &sessionId) noexcept
{
    std::uint32_t requestState{0U};
    LnMapIterator const it{lnProcMap_.find(lnName)};
    if (lnProcMap_.end() != it) {
        requestState = static_cast< std::uint32_t >(lnProcMap_[lnName].getLNRequestStateHandler());
    }
    NmLogger().LogVerbose() << "NMIpcProc::_getNetworkRequestState: type=" << requestState;
    isoft::ipc::IPCPacketBufferReference *resBuffer{nullptr};
    isoft::ipc::IPCPacket *const responsePacket{ipcServer_->MakeResponse(sessionId)};
    if (nullptr == responsePacket) {
        return;
    }

    resBuffer = responsePacket->AppendBuffer(sizeof(requestState));
    if (nullptr == resBuffer) {
        return;
    }
    static_cast< void >(resBuffer->SetLen(sizeof(requestState)));
    std::ignore = memcpy(resBuffer->GetPtr(), &requestState, sizeof(requestState));
    /// Parameter false means it is not the last message
    if (0 != this->ipcServer_->Send(responsePacket, true)) {
        return;
    }
}

/// @brief Client sets logical network request state
/// @param  lnName Logical network instance
/// @param  sessionId ipc session
/// @param  requestState Request state
void NMIpcProc::_setNetworkRequestState(ara::core::String const &lnName,
                                        std::uint64_t const &sessionId,
                                        ara::nm::NetworkStateType const requestState) noexcept
{
    std::uint32_t currentState{0U};
    LnMapIterator const it{lnProcMap_.find(lnName)};
    if (lnProcMap_.end() != it) {
        currentState = static_cast< std::uint32_t >(lnProcMap_[lnName].setLNRequestStateHandler(requestState));
    }
    NmLogger().LogVerbose() << "NMIpcProc::_setNetworkRequestState: requestState=" << currentState;
    isoft::ipc::IPCPacketBufferReference *resBuffer{nullptr};
    isoft::ipc::IPCPacket *const responsePacket{ipcServer_->MakeResponse(sessionId)};
    if (nullptr == responsePacket) {
        return;
    }

    resBuffer = responsePacket->AppendBuffer(sizeof(currentState));
    if (nullptr == resBuffer) {
        return;
    }
    static_cast< void >(resBuffer->SetLen(sizeof(currentState)));
    std::ignore = memcpy(resBuffer->GetPtr(), &currentState, sizeof(currentState));
    /// Parameter false means it is not the last message
    if (0 != this->ipcServer_->Send(responsePacket, true)) {
        return;
    }
}

/// @brief Client gets logical network state
/// @param  lnName Logical network instance
/// @param  sessionId ipc session
void NMIpcProc::_getNetworkCurrentState(ara::core::String const &lnName, std::uint64_t const &sessionId) noexcept
{
    std::uint32_t currentState{0U};
    LnMapIterator const it{lnProcMap_.find(lnName)};
    if (lnProcMap_.end() != it) {
        currentState = static_cast< std::uint32_t >(lnProcMap_[lnName].getNetowrkCurrentStateHandler());
    }
    NmLogger().LogVerbose() << "NMIpcProc::_getNetworkCurrentState: currentState=" << currentState;
    isoft::ipc::IPCPacketBufferReference *resBuffer{nullptr};
    isoft::ipc::IPCPacket *const responsePacket{ipcServer_->MakeResponse(sessionId)};
    if (nullptr == responsePacket) {
        return;
    }

    resBuffer = responsePacket->AppendBuffer(sizeof(currentState));
    if (nullptr == resBuffer) {
        return;
    }
    static_cast< void >(resBuffer->SetLen(sizeof(currentState)));
    std::ignore = memcpy(resBuffer->GetPtr(), &currentState, sizeof(currentState));
    /// Parameter false means it is not the last message
    if (0 != this->ipcServer_->Send(responsePacket, true)) {
        return;
    }
}

/// @brief Client gets Ethernet instance state machine state
/// @param  ipAddr Ethernet instance
/// @param  sessionId ipc session
void NMIpcProc::_getEtherState(ara::core::String const &ipAddr, std::uint64_t const &sessionId) noexcept
{
    std::uint32_t currentState{0U};
    EtherMapIterator const it{etherProcMap_.find(ipAddr)};
    if (etherProcMap_.end() != it) {
        currentState = static_cast< std::uint32_t >(etherProcMap_[ipAddr].getEtherStateMachineStateHandler());
    }
    NmLogger().LogVerbose() << "NMIpcProc::_getEtherState: currentState=" << currentState;
    isoft::ipc::IPCPacketBufferReference *resBuffer{nullptr};
    isoft::ipc::IPCPacket *const responsePacket{ipcServer_->MakeResponse(sessionId)};
    if (nullptr == responsePacket) {
        return;
    }

    resBuffer = responsePacket->AppendBuffer(sizeof(currentState));
    if (nullptr == resBuffer) {
        return;
    }
    static_cast< void >(resBuffer->SetLen(sizeof(currentState)));
    std::ignore = memcpy(resBuffer->GetPtr(), &currentState, sizeof(currentState));
    /// Parameter false means it is not the last message
    if (0 != this->ipcServer_->Send(responsePacket, true)) {
        return;
    }
}

/// @brief Client sets Ethernet instance state machine state
/// @param  ipAddr Ethernet instance
/// @param  sessionId ipc session
/// @param  requestState Request state
void NMIpcProc::_setEtherState(ara::core::String const &ipAddr,
                               std::uint64_t const &sessionId,
                               ara::nm::NetworkStateType const requestState) noexcept
{
    EtherMapIterator const it{etherProcMap_.find(ipAddr)};
    if (etherProcMap_.end() != it) {
        etherProcMap_[ipAddr].setEtherStateMachineSateHandle(requestState);
    }
    std::uint32_t const currentState{static_cast< std::uint32_t >(requestState)};
    NmLogger().LogVerbose() << "NMIpcProc::_setEtherState: requestState=" << currentState;
    isoft::ipc::IPCPacketBufferReference *resBuffer{nullptr};
    isoft::ipc::IPCPacket *const responsePacket{ipcServer_->MakeResponse(sessionId)};
    if (nullptr == responsePacket) {
        return;
    }

    resBuffer = responsePacket->AppendBuffer(sizeof(currentState));
    if (nullptr == resBuffer) {
        return;
    }
    static_cast< void >(resBuffer->SetLen(sizeof(currentState)));
    std::ignore = memcpy(resBuffer->GetPtr(), &currentState, sizeof(currentState));
    /// Parameter false means it is not the last message
    if (0 != this->ipcServer_->Send(responsePacket, true)) {
        return;
    }
}

/// @brief Client requests node detection
/// @param  ipAddr Ethernet instance
/// @param  sessionId ipc session
void NMIpcProc::_requestDetectNode(ara::core::String const &ipAddr, std::uint64_t const &sessionId) noexcept
{
    EtherMapIterator const it{etherProcMap_.find(ipAddr)};
    if (etherProcMap_.end() != it) {
        etherProcMap_[ipAddr].requestDetectNodeHandle();
    }
    std::uint32_t const currentState{0U};
    NmLogger().LogVerbose() << "NMIpcProc::_requestDetectNode: currentState=" << currentState
                            << ", sessionId=" << sessionId;
    isoft::ipc::IPCPacketBufferReference *resBuffer{nullptr};
    isoft::ipc::IPCPacket *const responsePacket{ipcServer_->MakeResponse(sessionId)};
    if (nullptr == responsePacket) {
        return;
    }

    resBuffer = responsePacket->AppendBuffer(sizeof(currentState));
    if (nullptr == resBuffer) {
        return;
    }
    static_cast< void >(resBuffer->SetLen(sizeof(currentState)));
    std::ignore = memcpy(resBuffer->GetPtr(), &currentState, sizeof(currentState));
    /// Parameter false means it is not the last message
    if (0 != this->ipcServer_->Send(responsePacket, true)) {
        return;
    }
}

/// @brief Client notifies Ethernet instance wakeup
/// @param  ipAddr Ethernet instance
/// @param  sessionId ipc session
void NMIpcProc::_notifyWakeup(ara::core::String const &ipAddr, std::uint64_t const &sessionId) noexcept
{
    EtherMapIterator const it{etherProcMap_.find(ipAddr)};
    if (etherProcMap_.end() != it) {
        etherProcMap_[ipAddr].notifyWakeupHandle();
    }

    std::uint32_t const currentState{0U};
    NmLogger().LogVerbose() << "NMIpcProc::_notifyWakeup: currentState=" << currentState;
    isoft::ipc::IPCPacketBufferReference *resBuffer{nullptr};
    isoft::ipc::IPCPacket *const responsePacket{ipcServer_->MakeResponse(sessionId)};
    if (nullptr == responsePacket) {
        return;
    }

    resBuffer = responsePacket->AppendBuffer(sizeof(currentState));
    if (nullptr == resBuffer) {
        return;
    }
    static_cast< void >(resBuffer->SetLen(sizeof(currentState)));
    std::ignore = memcpy(resBuffer->GetPtr(), &currentState, sizeof(currentState));
    /// Parameter false means it is not the last message
    if (0 != this->ipcServer_->Send(responsePacket, true)) {
        return;
    }
}

/// @brief Client sets Ethernet instance NM message send/receive control
/// @param  ipAddr Ethernet instance
/// @param  sessionId ipc session
/// @param  requestType Send/receive control
void NMIpcProc::_setMessageCtrlType(ara::core::String const &ipAddr,
                                    std::uint64_t const &sessionId,
                                    ara::nm::MessageCtrlType const requestType) noexcept
{
    EtherMapIterator const it{etherProcMap_.find(ipAddr)};
    if (etherProcMap_.end() != it) {
        etherProcMap_[ipAddr].setEtherNmMsgCtrlTypeHandler(requestType);
    }

    std::uint32_t const currentState{0U};
    NmLogger().LogVerbose() << "NMIpcProc::_setMessageCtrlType: currentState=" << currentState;
    isoft::ipc::IPCPacketBufferReference *resBuffer{nullptr};
    isoft::ipc::IPCPacket *const responsePacket{ipcServer_->MakeResponse(sessionId)};
    if (nullptr == responsePacket) {
        return;
    }

    resBuffer = responsePacket->AppendBuffer(sizeof(currentState));
    if (nullptr == resBuffer) {
        return;
    }
    static_cast< void >(resBuffer->SetLen(sizeof(currentState)));
    std::ignore = memcpy(resBuffer->GetPtr(), &currentState, sizeof(currentState));
    /// Parameter false means it is not the last message
    if (0 != this->ipcServer_->Send(responsePacket, true)) {
        return;
    }
}

/// @brief Client gets Ethernet instance NM message send/receive control
/// @param  ipAddr Ethernet instance
/// @param  sessionId ipc session
void NMIpcProc::_getMessageCtrlType(ara::core::String const &ipAddr, std::uint64_t const &sessionId) noexcept
{
    std::uint32_t currentTye{0U};
    EtherMapIterator const it{etherProcMap_.find(ipAddr)};
    if (etherProcMap_.end() != it) {
        currentTye = static_cast< std::uint32_t >(etherProcMap_[ipAddr].getEtherNmMsgCtrlTypeHandler());
    }

    NmLogger().LogVerbose() << "NMIpcProc::_getMessageCtrlType: currentTye=" << currentTye;
    isoft::ipc::IPCPacketBufferReference *resBuffer{nullptr};
    isoft::ipc::IPCPacket *const responsePacket{ipcServer_->MakeResponse(sessionId)};
    if (nullptr == responsePacket) {
        return;
    }

    resBuffer = responsePacket->AppendBuffer(sizeof(currentTye));
    if (nullptr == resBuffer) {
        return;
    }
    static_cast< void >(resBuffer->SetLen(sizeof(currentTye)));
    std::ignore = memcpy(resBuffer->GetPtr(), &currentTye, sizeof(currentTye));
    /// Parameter false means it is not the last message
    if (0 != this->ipcServer_->Send(responsePacket, true)) {
        return;
    }
}

/// @brief Client gets Ethernet instance initialization status
/// @param  ipAddr Ethernet instance
/// @param  sessionId ipc session
void NMIpcProc::_getInitState(ara::core::String const &ipAddr, std::uint64_t const &sessionId) noexcept
{
    std::uint32_t initCode{0U};

    EtherMapIterator const it{etherProcMap_.find(ipAddr)};
    if (etherProcMap_.end() != it) {
        initCode = static_cast< std::uint32_t >(etherProcMap_[ipAddr].getInitStateHandler());
    }
    NmLogger().LogVerbose() << "NMIpcProc::_getInitState: initCode=" << initCode;
    isoft::ipc::IPCPacketBufferReference *resBuffer{nullptr};
    isoft::ipc::IPCPacket *const responsePacket{ipcServer_->MakeResponse(sessionId)};
    if (nullptr == responsePacket) {
        return;
    }

    resBuffer = responsePacket->AppendBuffer(sizeof(initCode));
    if (nullptr == resBuffer) {
        return;
    }
    static_cast< void >(resBuffer->SetLen(sizeof(initCode)));
    std::ignore = memcpy(resBuffer->GetPtr(), &initCode, sizeof(initCode));
    /// Parameter false means it is not the last message
    if (0 != this->ipcServer_->Send(responsePacket, true)) {
        return;
    }
}

/// @brief Client sets Ethernet instance external PN request list
/// @param  ipAddr Ethernet instance
/// @param  sessionId ipc session
void NMIpcProc::_getExternalPnRequestList(ara::core::String const &ipAddr, std::uint64_t const &sessionId) noexcept
{
    ara::core::String pnList{};
    EtherMapIterator const it{etherProcMap_.find(ipAddr)};
    if (etherProcMap_.end() != it) {
        pnList = (etherProcMap_[ipAddr].getExternalPnListHandler()).c_str();
    }
    NmLogger().LogVerbose() << "NMIpcProc::_getExternalPnRequestList: pnList=" << pnList.c_str();
    isoft::ipc::IPCPacketBufferReference *resBuffer{nullptr};
    isoft::ipc::IPCPacket *const responsePacket{ipcServer_->MakeResponse(sessionId)};
    if (nullptr == responsePacket) {
        return;
    }

    resBuffer = responsePacket->AppendBuffer(sizeof(pnList));
    if (nullptr == resBuffer) {
        return;
    }
    static_cast< void >(resBuffer->SetLen(sizeof(pnList)));
    std::ignore = memcpy(resBuffer->GetPtr(), pnList.c_str(), sizeof(pnList));
    /// Parameter false means it is not the last message
    if (0 != this->ipcServer_->Send(responsePacket, true)) {
        return;
    }
}

}  // namespace internal
}  // namespace nm
}  // namespace ara
