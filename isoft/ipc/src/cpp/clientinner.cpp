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
/// @file       clientinner.cpp
/// @brief
/// @details
/// @date       2022-09-20
/// @author     yangtao
/// @version    1.2.0
///
/// ================================================================

#include "clientinner.h"

#include <iostream>

#include "packetinner.h"

using namespace std;

namespace isoft {
namespace ipc {

void ClientHandlerWrapper(void* context, ipc_client_handler_status_s status, ipc_packet_t* response_packet)
{
    IPCClientInnerManager::GetInstance()->SendCallback(context, status, response_packet);
}

void ClientConnHandlerWrapper(void* context, ipc_client_connection_status_s status)
{
    IPCClientInnerManager::GetInstance()->ConnCallback(context, status);
}

IPCClientInner::IPCClientInner(const char* peer_name) : _clientC(nullptr), _clientCPP(nullptr), _connHandler(nullptr)
{
    _peerName = strdup(peer_name);
}

IPCClientInner::~IPCClientInner()
{
    _clientC   = nullptr;
    _clientCPP = nullptr;

    if (_connHandler != nullptr)
        delete _connHandler;
    _connHandler = nullptr;

    if (_peerName != NULL)
        free(_peerName);
}

IPCClientInner::ClientCBContext* IPCClientInner::SetSendHandler(IPCClientHandler handler,
                                                                void* context,
                                                                ipc_client_handler_t* handlerC)
{
    auto* cb    = new IPCClientInner::ClientCBContext();
    cb->context = context;
    cb->handler = handler;
    _handlerList.push_back(cb);

    *handlerC = ClientHandlerWrapper;
    return cb;
}

void IPCClientInner::SetConnCallback(IPCClientConnectionHandler handler, void* context)
{
    std::lock_guard< std::mutex > lock_guard(_connHandlerLock);

    if (_connHandler == nullptr)
        _connHandler = new IPCClientInner::ClientConnCBContext();

    _connHandler->context = context;
    _connHandler->handler = handler;

    ipc_client_set_connection_handler(this->_clientC, ClientConnHandlerWrapper, _connHandler);
}

bool IPCClientInner::ConnCallback(void* context, ipc_client_connection_status_t status)
{
    if (_connHandler != nullptr && _connHandler == context) {
        _connHandler->handler(_connHandler->context, (IPCClientConnectionStatus)status);
        return true;
    }
    return false;
}

bool IPCClientInner::SendCallback(void* context, ipc_client_handler_status_s status, ipc_packet_t* reponse_packet)
{
    std::lock_guard< std::mutex > lock_guard(_handlerListLock);

    for (auto iter = _handlerList.begin(); iter != _handlerList.end(); iter++) {
        if (*iter == nullptr)
            continue;

        if (*iter == context) {
            IPCPacket* packetCPP = IPCPacket::Create(reponse_packet);
            (*iter)->handler((*iter)->context, (IPCClientHandlerStatus)status, packetCPP);
            IPCPacketInnerManager::GetInstance()->ReleasePacket(packetCPP);
            if (status == IPC_CLIENT_HANDLER_STATUS_RECIEVED_COMPLETED) {
                delete *iter;
                *iter = nullptr;
                _handlerList.erase(iter);
            }
            return true;
        }
    }
    return false;
}

///////////////////////////////////////////////////////////////////////////////
IPCClientInnerManager* IPCClientInnerManager::_instance = nullptr;
IPCClientInnerManager::~IPCClientInnerManager()
{
    for (auto& it : _clientMap) {
        char* map_key = NULL;
        map_key       = (char*)it.first;
        if (map_key != NULL && strlen(map_key) > 0) {
            free(map_key);
        }
    }
    _clientMap.clear();
}

IPCClientInnerManager* IPCClientInnerManager::GetInstance()
{
    if (_instance == nullptr)
        _instance = new IPCClientInnerManager();
    return _instance;
}

std::weak_ptr< IPCClient > IPCClientInnerManager::GetClient(const char* peer_name)
{
    auto it = _clientMap.find(peer_name);
    if (it == _clientMap.end() || it->second.expired()) {
        std::weak_ptr< IPCClient > empty;
        return empty;
    }
    return it->second;
}

void IPCClientInnerManager::InsertClient(const char* peer_name, std::weak_ptr< IPCClient > client)
{
    std::lock_guard< std::mutex > lock_guard(_clientListLock);

    char* map_key = strdup(peer_name);

    _clientMap[map_key] = client;
}

void IPCClientInnerManager::ReleaseClient(const char* peer_name)
{
    std::lock_guard< std::mutex > lock_guard(_clientListLock);
    char* map_key = NULL;

    auto it = _clientMap.find(peer_name);
    if (it != _clientMap.end()) {
        map_key = (char*)it->first;
    }
    _clientMap.erase(peer_name);
    if (map_key != NULL && strlen(map_key) > 0) {
        free(map_key);
    }
}

void IPCClientInnerManager::SendCallback(void* context,
                                         ipc_client_handler_status_s status,
                                         ipc_packet_t* response_packet)
{
    for (auto& it : _clientMap) {
        if (it.second.expired())
            continue;

        if (it.second.lock()->_clientInner->SendCallback(context, status, response_packet))
            return;
    }
}

void IPCClientInnerManager::ConnCallback(void* context, ipc_client_connection_status_s status)
{
    for (auto& it : _clientMap) {
        if (it.second.expired())
            continue;
        if (it.second.lock()->_clientInner->ConnCallback(context, status))
            return;
    }
}

}  // namespace ipc
}  // namespace isoft
