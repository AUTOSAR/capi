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
/// @file       packetinner.cpp
/// @brief
/// @details
/// @date       2022-10-10
/// @author     yangtao
/// @version    1.2.0
///
/// ================================================================

#include "packetinner.h"

#include "isoft/ipccpp/buffer.h"
#include "isoft/ipccpp/packet.h"

namespace isoft {
namespace ipc {
IPCPacketInner::IPCPacketInner(ipc_packet_t* packetC, IPCPacket* packet)
{
    this->_packetC   = packetC;
    this->_packet    = packet;
    this->_refPacket = nullptr;
    this->_refCount  = 1;
}

IPCPacketInner::~IPCPacketInner() { _packetC = nullptr; }

void IPCPacketInner::SetPacket(ipc_packet_t* packet) { _packetC = packet; }

void IPCPacketInner::Ref() { _refCount++; }

void IPCPacketInner::Release() { _refCount--; }

int IPCPacketInner::GetRefCount() { return _refCount; }

IPCPacket* IPCPacketInner::GetPacket() { return _packet; }

IPCPacket* IPCPacketInner::GetRefPacket() { return _refPacket; }

///////////////////////////////////////////////////////////////

IPCPacketInnerManager* IPCPacketInnerManager::_instance = nullptr;
IPCPacketInnerManager::~IPCPacketInnerManager()
{
    for (auto iter = _packetList.begin(); iter != _packetList.end(); iter++) {
        if (*iter != nullptr) {
            delete *iter;
            *iter = nullptr;
        }
    }
    _packetList.clear();
}

IPCPacketInnerManager* IPCPacketInnerManager::GetInstance()
{
    if (_instance == nullptr)
        _instance = new IPCPacketInnerManager();
    return _instance;
}

void IPCPacketInnerManager::AppendPacket(IPCPacket* packet) { _packetList.push_back(packet); }

IPCPacket* IPCPacketInnerManager::ReleasePacket(IPCPacket* packet)
{
    if (packet == nullptr)
        return nullptr;

    packet->_packetInner->Release();

    if (packet->_packetInner->GetRefPacket() != nullptr)
        IPCPacketInnerManager::GetInstance()->ReleasePacket(packet->_packetInner->GetRefPacket());

    if (packet->_packetInner->GetRefCount() == 0) {
        DeletePacket(packet);
        return nullptr;
    }
    return packet;
}

void IPCPacketInnerManager::DeletePacket(IPCPacket* packet)
{
    if (packet == nullptr)
        return;

    std::lock_guard< std::mutex > lock_guard(_packetListLock);
    for (auto iter = _packetList.begin(); iter != _packetList.end(); iter++) {
        if (*iter == packet) {
            delete *iter;
            *iter = nullptr;
            _packetList.erase(iter);
            return;
        }
    }
}

IPCPacket* IPCPacketInnerManager::DupPacket(ipc_packet_t* packetC, IPCPacket* packet)
{
    if (packet == nullptr || packetC == nullptr)
        return nullptr;

    IPCPacket* newPacket;
    newPacket = IPCPacket::Create(packetC);
    newPacket->SetRefPacket(packet);
    packet->_packetInner->Ref();

    return newPacket;
}

}  // namespace ipc
}  // namespace isoft
