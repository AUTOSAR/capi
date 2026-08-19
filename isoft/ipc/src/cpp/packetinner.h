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
/// @file       packetinner.h
/// @brief
/// @details
/// @date       2022-10-10
/// @author     yangtao
/// @version    1.2.0
///
/// ================================================================

#ifndef ISOFT_IPC_PACKET_INNER_CPP_H
#define ISOFT_IPC_PACKET_INNER_CPP_H

#include <isoft/ipc/packet.h>

#include <atomic>
#include <list>
#include <mutex>

namespace isoft {
namespace ipc {

class IPCPacket;
class IPCPacketBufferReference;
class IPCPacketInner
{
public:
    IPCPacketInner(ipc_packet_t* packetC, IPCPacket* packet);
    ~IPCPacketInner();

public:
    void Ref();
    void Release();

    int GetRefCount();
    IPCPacket* GetRefPacket();
    IPCPacket* GetPacket();

private:
    void SetPacket(ipc_packet_t* _packet);

private:
    friend class IPCPacket;

    ipc_packet_t* _packetC;
    IPCPacket* _packet;
    IPCPacket* _refPacket;

    std::atomic< int > _refCount;
};

class IPCPacketInnerManager
{
public:
    static IPCPacketInnerManager* GetInstance();

private:
    static IPCPacketInnerManager* _instance;

public:
    void AppendPacket(IPCPacket* packet);

    IPCPacket* ReleasePacket(IPCPacket* packet);
    IPCPacket* DupPacket(ipc_packet_t* packetC, IPCPacket* packet);

private:
    IPCPacketInnerManager() = default;
    ~IPCPacketInnerManager();

    void DeletePacket(IPCPacket* packet);

private:
    std::list< IPCPacket* > _packetList;
    std::mutex _packetListLock;
};
}  // namespace ipc
}  // namespace isoft
#endif
