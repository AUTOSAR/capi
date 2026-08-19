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
/// @file       log_command_trans.cpp
/// @brief      Template class for encapsulating log parameters
/// @details
/// @date       2024-06-28
/// @author     yangjinbiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/LOG/DltLogd
/// @interface_level = unit
/// @trace_id_sr=LOG_SR_00001,LOG_SR_00003,LOG_SR_00009
/// @unit_name = log_command_trans
/// @unit_description=Backend of Dlt module, used to support log and command forwarding.
/// @endcode
///
/// ================================================================

#include "log_command_trans.h"

#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include <functional>
#include <iostream>
#include <memory>

#ifdef ARA_LOG_INTERNAL_IPC_DEBUG
    #include <isoft/ipccpp/debug.h>
#endif  // DEBUG
#include <isoft/naicpp/evloop.h>
#include <rapidjson/document.h>
#include <rapidjson/rapidjson.h>
#include <rapidjson/writer.h>

#include "Utils/src/private_log.h"
#include "Utils/src/usercommand_define.h"

namespace ara {
namespace log {
namespace internal {
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_LOG_00329
/// @trace_id_dd=DD_LOG_01725
/// @needwork = ad
/// @endcode

using rapidjson::Document;
using rapidjson::StringBuffer;
using rapidjson::Value;
using rapidjson::Writer;

LogCommandTrans::~LogCommandTrans() noexcept
{
    LOGVERBOSE(__func__) << " enter ";

    // this->deIniting();
    LOGVERBOSE(__func__) << " leave ";
}

std::int32_t LogCommandTrans::Init() noexcept
{
    LOGVERBOSE(__func__) << " enter ";
    appInfos_ = std::make_shared< AAClientInfoMap >();

    mainLoop_ = isoft::naicpp::GlobalGeneralEvLoop::Get();

    if (nullptr == mainLoop_) {
        LOGERROR(__func__) << "nullptr == mainLoop";
        return -1;
    }
#ifdef ARA_LOG_INTERNAL_IPC_DEBUG
    if (0 != isoft::ipc::debug::IPCEnvDestroy()) {
        std::ignore = LOGERROR(__func__);
        std::ignore = LOGVERBOSE("isoft::ipc::debug::IPCEnvDestroy()");
        return -1;
    }

    // Create folder for IPC communication
    if (0 != isoft::ipc::debug::IPCEnvInit()) {
        std::ignore = LOGERROR(__func__);
        std::ignore = LOGVERBOSE("isoft::ipc::debug::IPCEnvInit()");
        return -1;
    }

    //create ipc
    if (0 > isoft::ipc::debug::CreateIPC(kDaemonName.c_str())) {
        std::ignore = LOGERROR(__func__);
        std::ignore = LOGVERBOSE("isoft::ipc::debug::CreateIPC()");
        return -1;
    }
#endif
    //////////////////// Open IPC server  ////////////////
    std::int32_t const r{isoft::ipc::IPCInitNaiUDS(mainLoop_)};
    if (0 > r) {
        LOGERROR(__func__) << "IPCInitNaiUDS error:  ";
        return -1;
    }
    std::string const kServiceName{"CMDC"};
    ipcServer_ = isoft::ipc::IPCServer::Create(kServiceName.c_str());
    if (nullptr == ipcServer_) {
        LOGERROR(__func__) << "isoft::ipc::IPCClient::Create(" << kServiceName << ")";
        return -1;
    }

    ipcServer_->SetHandler(IPC_SERVER_HANDLER_CONNECT,
                           std::bind(&LogCommandTrans::_IpcServerConnectionHandler, this, std::placeholders::_1,
                                     std::placeholders::_2, std::placeholders::_3),
                           this);
    ipcServer_->SetHandler(IPC_SERVER_HANDLER_DISCONNECT,
                           std::bind(&LogCommandTrans::_IpcServerConnectionHandler, this, std::placeholders::_1,
                                     std::placeholders::_2, std::placeholders::_3),
                           this);

    ipcServer_->SetHandler(IPC_SERVER_HANDLER_MULTI,
                           std::bind(&LogCommandTrans::_IpcServerRequestHandler, this, std::placeholders::_1,
                                     std::placeholders::_2, std::placeholders::_3),
                           this);

    if (0 != ipcServer_->Start()) {
        return -1;
    }

    LOGVERBOSE(__func__) << " leave   ";
    return 0;
}

std::int32_t LogCommandTrans::SendMessage(std::uint8_t* const mesg, std::size_t const& messageSize) noexcept
{
    LOGVERBOSE(__func__) << " enter  ";
    if (this->mStoping_) {
        LOGVERBOSE(__func__) << " stopping  ";
        return -1;
    }

    for (AppIDSocketMap::iterator appIter{mapAppidSocket_.begin()}; appIter != mapAppidSocket_.end(); appIter++) {
        std::ignore = this->Send(appIter->second, mesg, messageSize);
    }
    LOGVERBOSE(__func__) << " leave  ";
    return 0;
}

std::int32_t LogCommandTrans::Send(std::uint64_t const& sessionid,
                                   std::uint8_t* const dataBuffer,
                                   std::size_t const& messageSize) noexcept
{
    LOGVERBOSE(__func__) << " enter ";
    if (this->mStoping_) {
        LOGVERBOSE(__func__) << " stopping  " << sessionid;
        return -1;
    }

    isoft::ipc::IPCPacketBufferReference* resBuffer = nullptr;
    isoft::ipc::IPCPacket* const responsePacket{ipcServer_->MakeResponse(sessionid)};
    if (nullptr == responsePacket) {
        return -1;
    }

    resBuffer = responsePacket->AppendBuffer(messageSize);
    if (nullptr == resBuffer) {
        return -1;
    }
    static_cast< void >(resBuffer->SetLen(messageSize));
    std::ignore = memcpy(resBuffer->GetPtr(), dataBuffer, messageSize);
    /// The parameter false means it is not the last message
    if (0 != this->ipcServer_->Send(responsePacket, false)) {
        return -1;
    }
    LOGVERBOSE(__func__) << " leave   ";
    return 0;
}

std::int32_t LogCommandTrans::SendByAppid(std::string const& appid,
                                          std::uint8_t* const dataBuffer,
                                          std::size_t const& messageSize) noexcept
{
    LOGVERBOSE(__func__) << " enter ";
    if (this->mStoping_) {
        LOGVERBOSE(__func__) << " stopping  " << appid;
        return -1;
    }

    return this->Send(_getSocketFdByAppID(appid), dataBuffer, messageSize);
}

bool LogCommandTrans::SetStoping() noexcept
{
    this->mStoping_ = true;
    return true;
}

bool LogCommandTrans::DeIniting() noexcept
{
    LOGVERBOSE(__func__) << " enter  release ipc";

#ifdef ARA_LOG_INTERNAL_IPC_DEBUG

    if (0 > isoft::ipc::debug::DestroyIPC(kDaemonName)) {
        std::ignore = LOGVERBOSE("isoft::ipc::debug::DestroyIPC()");
    }
    // Clean up the folder used for IPC communication
    if (0 != isoft::ipc::debug::IPCEnvDestroy()) {
        std::ignore = LOGVERBOSE("isoft::ipc::debug::IPCEnvDestroy()");
    }
#endif
    LOGVERBOSE(__func__) << "deconstuctor ";
    if (nullptr != ipcServer_) {
        if (0 != ipcServer_->Stop()) {
            LOGERROR(__func__) << "isoft::ipc::IPCClient::Stop(), errno=";
        }
        ipcServer_->Release();
    }
    if (-1 == isoft::ipc::IPCDeInitNaiUDS()) {
        LOGERROR(__func__) << "IPCDeInitNaiUDS(), errno=";
    }
    LOGVERBOSE(__func__) << " leave ";

    return true;
}

void LogCommandTrans::_DoIpcResponse(std::uint64_t const& sessionid, std::string const& msg) noexcept
{
    LOGVERBOSE(__func__) << " enter   ";
    if (this->mStoping_) {
        LOGERROR(__func__) << " stopping  " << sessionid;
        return;
    }

    isoft::ipc::IPCPacketBufferReference* resBuffer = nullptr;
    isoft::ipc::IPCPacket* const responsePacket{ipcServer_->MakeResponse(sessionid)};
    if (nullptr == responsePacket) {
        return;
    }

    resBuffer = responsePacket->AppendBuffer(msg.length());
    if (nullptr == resBuffer) {
        return;
    }
    static_cast< void >(resBuffer->SetLen(msg.length()));
    std::ignore = memcpy(resBuffer->GetPtr(), msg.c_str(), msg.length());
    /// The parameter false means it is not the last message
    if (0 != this->ipcServer_->Send(responsePacket, false)) {
        return;
    }
    LOGVERBOSE(__func__) << " leave   ";
}

void LogCommandTrans::_HandleLibCmd(std::string const& jsonstr, std::uint64_t const& socketfd) noexcept
{
    LOGVERBOSE(__func__) << " enter   ";
    Document d;
    if (d.Parse(jsonstr.c_str()).HasParseError()) {
        std::ignore = LOGERROR(" LogCommandTrans::handleLibCmd  json error ");
    } else {
        std::ignore = LOGERROR(" LogCommandTrans::handleLibCmd json OK");
        bool const cmdOK{d["cmd"].IsInt()};
        if (d.HasMember("cmd") && cmdOK) {
            std::int32_t const cmd{d["cmd"].GetInt()};
            switch (cmd) {
                case static_cast< std::int32_t >(UserCommand::kRegApp): {
                    std::string const appid{d["appid"].GetString()};
                    std::ignore = LOGVERBOSE(appid + ": reg");
                    std::string const appdescp{d["appidDesc"].GetString()};
                    _addAAClient(appid, socketfd);
                    (*appInfos_)[appid]             = std::make_shared< AAClientInfo >();
                    appInfos_->at(appid)->appId     = appid;
                    appInfos_->at(appid)->sessionId = socketfd;
                    appInfos_->at(appid)->appDesc   = appdescp;
                    bool const infoListOK{d["contextIdInfoList"].IsArray()};
                    if (d.HasMember("contextIdInfoList") && infoListOK) {
                        AAClientChannelMap& cmap{appInfos_->at(appid)->channels};
                        for (rapidjson::Value& v : d["contextIdInfoList"].GetArray()) {
                            if (v.IsObject()) {
                                std::string const contextid{v["contextId"].GetString()};
                                std::int8_t const loglevel{static_cast< std::int8_t >(v["logLevel"].GetInt())};

                                std::string const contextDesc{v["contextDesc"].GetString()};
                                cmap[contextid]                     = std::make_shared< AAClientChannel >();
                                cmap[contextid]->contextId          = (contextid);
                                cmap[contextid]->logLevel           = (loglevel);
                                cmap[contextid]->contextDescription = (contextDesc);
                            }
                        }
                    }
                    _RegStatusResponse(socketfd);
                    break;
                }
                case static_cast< std::int32_t >(UserCommand::kUpdateLogChannel): {
                    std::string const appid{d["appid"].GetString()};
                    if (appInfos_->count(appid) > 0U) {
                        bool const infoListOK{d["contextIdInfoList"].IsArray()};
                        if (d.HasMember("contextIdInfoList") && infoListOK) {
                            AAClientChannelMap& cmap{appInfos_->at(appid)->channels};
                            for (rapidjson::Value& v : d["contextIdInfoList"].GetArray()) {
                                if (v.IsObject()) {
                                    std::string const contextid{v["contextId"].GetString()};
                                    std::ignore = LOGVERBOSE(appid + ": channel: " + contextid);
                                    std::int8_t const loglevel{static_cast< std::int8_t >(v["logLevel"].GetInt())};

                                    std::string const contextDesc{"default desc"};
                                    cmap[contextid]                     = std::make_shared< AAClientChannel >();
                                    cmap[contextid]->contextId          = (contextid);
                                    cmap[contextid]->logLevel           = (loglevel);
                                    cmap[contextid]->contextDescription = (contextDesc);
                                }
                            }
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
    LOGVERBOSE(__func__) << " leave   ";
}

void LogCommandTrans::_IpcServerRequestHandler(void* const context,
                                               isoft::ipc::IPCServerHandleType const& htype,
                                               isoft::ipc::IPCPacket* const reqPacket) noexcept
{
    std::ignore = htype;
    std::ignore = context;

    isoft::ipc::IPCSessionId sessionId              = 0;
    isoft::ipc::IPCPacketBufferReference* reqBuffer = nullptr;

    if (nullptr == reqPacket) {
        LOGVERBOSE(__func__) << ": request is null";
        return;
    }
    sessionId = reqPacket->GetSessionId();
    reqBuffer = reqPacket->GetBuffer();  // request buffer
    std::string const jsonPacket(reinterpret_cast< const char* >(reqBuffer->GetPtr()), reqBuffer->GetLen());
    this->_HandleLibCmd(jsonPacket, sessionId);
    LOGVERBOSE(__func__) << " leave   ";
}

void LogCommandTrans::_IpcServerConnectionHandler(void* const context,
                                                  isoft::ipc::IPCServerHandleType const& type,
                                                  isoft::ipc::IPCPacket* const packet)
{
    LOGVERBOSE(__func__) << "type: " << static_cast< std::uint64_t >(type);
    std::ignore = packet;
    std::ignore = context;
    if (type == IPC_SERVER_HANDLER_CONNECT) {
        LOGVERBOSE(__func__) << "Client Connected.";
    } else if (type == IPC_SERVER_HANDLER_DISCONNECT) {
        LOGVERBOSE(__func__) << "Client Disconnected. ";
        _DelAAClient(packet->GetSessionId());
    } else {
        LOGVERBOSE(__func__) << "for qac";
    }
    LOGVERBOSE(__func__) << " leave  ";
}

void LogCommandTrans::_addAAClient(std::string const& appid, std::uint64_t const& socketfd) noexcept
{
    LOGVERBOSE(__func__) << appid << " : " << socketfd;
    mapAppidSocket_[appid] = socketfd;
}

void LogCommandTrans::_DelAAClient(std::uint64_t const& socketfd) noexcept
{
    LOGVERBOSE(__func__) << " enter ";
    for (AppIDSocketMap::iterator it{mapAppidSocket_.begin()}; it != mapAppidSocket_.end(); ++it) {
        if (it->second == socketfd) {
            std::ignore = appInfos_->erase(it->first);
            std::ignore = mapAppidSocket_.erase(it);
            break;
        }
    }
    LOGVERBOSE(__func__) << " leave  ";
}

std::uint64_t LogCommandTrans::_getSocketFdByAppID(std::string const& appid) noexcept
{
    LOGVERBOSE(__func__) << " enter ";
    if (mapAppidSocket_.count(appid) > 0U) {
        LOGVERBOSE(__func__) << " have leave ";
        return mapAppidSocket_[appid];
    }
    LOGVERBOSE(__func__) << " leave  ";
    return -1;
}

void LogCommandTrans::_RegStatusResponse(std::uint64_t const& sessionId) noexcept
{
    std::string retstr{};
    rapidjson::StringBuffer buf;
    rapidjson::Writer< rapidjson::StringBuffer > writer{buf};
    std::ignore = writer.StartObject();
    std::ignore = writer.Key("cmd");
    std::ignore = writer.Int(UserCommand::kRegAppStatusOK);
    std::ignore = writer.EndObject();
    std::ignore = retstr.append(buf.GetString());
    _DoIpcResponse(sessionId, retstr);
    LOGVERBOSE(__func__) << " leave  ";
}

}  // namespace internal
}  // namespace log
}  // namespace ara
