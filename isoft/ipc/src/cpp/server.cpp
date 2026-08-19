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
/// @brief
/// @details
/// @date       2022-09-20
/// @author     yangtao
/// @version    1.2.0
///
/// ================================================================

#include "isoft/ipccpp/server.h"

#include "packetinner.h"
#include "serverinner.h"

namespace isoft {
namespace ipc {

IPCServer::IPCServer(ipc_server_t* server)
{
    _serverInner = IPCServerInnerManager::GetInstance()->CreateServerInner();

    if (_serverInner != nullptr) {
        _serverInner->_serverC   = server;
        _serverInner->_serverCPP = this;
    }
}

IPCServer::~IPCServer()
{
    IPCServerInnerManager::GetInstance()->DeleteServerInner(_serverInner);

    // this->Release();
    _serverInner = nullptr;
}

int IPCServer::Start()
{
    if (_serverInner->_serverC != nullptr)
        return ipc_server_start(_serverInner->_serverC);
    return -1;
}

int IPCServer::Stop()
{
    if (_serverInner->_serverC != nullptr)
        return ipc_server_stop(_serverInner->_serverC);
    return -1;
}

std::unique_ptr< IPCServer > IPCServer::Create(const char* name)
{
    std::unique_ptr< IPCServer > server;
    ipc_server_t* raw = ipc_server_create(name);
    if (raw != nullptr)
        server.reset(new IPCServer(raw));
    return server;
}

void IPCServer::Release()
{
    if (_serverInner->_serverC != nullptr)
        ipc_server_release(_serverInner->_serverC);
    _serverInner->_serverC = nullptr;
}

IPCPacket* IPCServer::MakeResponse(IPCSessionId session_id)
{
    if (_serverInner->_serverC != nullptr) {
        ipc_packet_t* packetC = ipc_server_make_response(_serverInner->_serverC, session_id);
        if (packetC != nullptr) {
            return IPCPacket::Create(packetC);
        }
    }
    return nullptr;
}

/**
 * @brief Copy a response packet using the payload of the reference packet as the source, for multi call
 */
IPCPacket* IPCServer::DupResponse(IPCSessionId session_id, IPCPacket* packet)
{
    if (_serverInner->_serverC != nullptr) {
        ipc_packet_t* packetC = ipc_server_dup_response(_serverInner->_serverC, session_id, packet->GetData());
        return IPCPacketInnerManager::GetInstance()->DupPacket(packetC, packet);
    }
    return nullptr;
}

/**
 * @brief Send response to the corresponding client
 * @param complete Indicates whether this response is the last response
 * @return 0 success, -1 failure, set errno. Error could be client does not exist
 */
int IPCServer::Send(IPCPacket* response_packet, bool complete)
{
    int status = -1;
    if (_serverInner->_serverC != nullptr)
        status = ipc_server_send(_serverInner->_serverC, response_packet->GetData(), complete);

    IPCPacketInnerManager::GetInstance()->ReleasePacket(response_packet);
    return status;
}

void IPCServer::SetHandler(IPCServerHandleType type, IPCServerHandler handler, void* context)
{
    if (_serverInner != nullptr)
        _serverInner->SetHandler(type, handler, context);
}

int IPCServer::CheckSessionID(IPCSessionId session_id)
{
    if (_serverInner->_serverC != nullptr)
        return ipc_server_check_session_id(_serverInner->_serverC, session_id);
    return -1;
}

}  // namespace ipc
}  // namespace isoft
