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
/// @file       serverinner.cpp
/// @brief
/// @details
/// @date       2022-09-20
/// @author     yangtao
/// @version    1.2.0
///
/// ================================================================

#include "serverinner.h"

#include <iostream>

#include "isoft/ipc/server.h"
#include "packetinner.h"
using namespace std;
namespace isoft {
namespace ipc {

void ServerHandlerWrapper(void* context, ipc_server_handle_type_t type, ipc_packet_t* packet)
{
    IPCServerInnerManager::GetInstance()->Callback(context, type, packet);
}

IPCServerInner::IPCServerInner() : _serverC(nullptr), _serverCPP(nullptr) {}

IPCServerInner::~IPCServerInner()
{
    _serverC   = nullptr;
    _serverCPP = nullptr;

    for (auto iter = _handlerList.begin(); iter != _handlerList.end(); iter++) {
        if (*iter == nullptr)
            continue;
        delete *iter;
        *iter = nullptr;
    }
    _handlerList.clear();
}

void IPCServerInner::SetHandler(IPCServerHandleType type, IPCServerHandler handler, void* context)
{
    std::lock_guard< std::mutex > lock_guard(_handlerListLock);

    auto iter     = _handlerList.begin();
    CBContext* cb = nullptr;

    for (; iter != _handlerList.end(); iter++) {
        if (*iter == nullptr)
            continue;
        if ((*iter)->handlerType == (ipc_server_handle_type_t)type) {
            cb = (*iter);
            break;
        }
    }
    if (iter == _handlerList.end()) {
        cb              = new CBContext();
        cb->handlerType = (ipc_server_handle_type_t)type;
        _handlerList.push_back(cb);
    }

    if (cb != nullptr) {
        cb->context = context;
        cb->handler = handler;

        ipc_server_set_handler(this->_serverC, cb->handlerType, ServerHandlerWrapper, cb);
    }
}

bool IPCServerInner::Callback(void* context, ipc_server_handle_type_t type, ipc_packet_t* packet)
{
    for (auto iter = _handlerList.begin(); iter != _handlerList.end(); iter++) {
        if (*iter == nullptr) {
            continue;
        }
        if ((*iter) == context && (*iter)->handlerType == type) {
            IPCPacket* packetCPP = IPCPacket::Create(packet);
            (*iter)->handler((*iter)->context, (IPCServerHandleType)type, packetCPP);
            IPCPacketInnerManager::GetInstance()->ReleasePacket(packetCPP);

            return true;
        }
    }
    return false;
}

////////////////////////////////////////////////////////////////////////
IPCServerInnerManager* IPCServerInnerManager::_instance = nullptr;

IPCServerInnerManager* IPCServerInnerManager::GetInstance()
{
    if (_instance == nullptr)
        _instance = new IPCServerInnerManager();
    return _instance;
}

IPCServerInnerManager::~IPCServerInnerManager() { _serverList.clear(); }

IPCServerInner* IPCServerInnerManager::CreateServerInner()
{
    auto inner = new IPCServerInner();
    _serverList.push_back(inner);
    return inner;
}

void IPCServerInnerManager::DeleteServerInner(IPCServerInner* serverInner)
{
    if (serverInner == nullptr)
        return;

    std::lock_guard< std::mutex > lock_guard(_serverListLock);

    for (auto iter = _serverList.begin(); iter != _serverList.end(); iter++) {
        if (*iter == serverInner) {
            delete *iter;
            *iter = nullptr;
            _serverList.erase(iter);
            return;
        }
    }
}

void IPCServerInnerManager::Callback(void* context, ipc_server_handle_type_t type, ipc_packet_t* packet)
{
    for (auto& s : _serverList) {
        if (s != nullptr && s->Callback(context, type, packet))
            return;
    }
}

}  // namespace ipc
}  // namespace isoft
