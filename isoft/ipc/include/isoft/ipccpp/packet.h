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
/// @file       packet.h
/// @brief
/// @details
/// @date       2022-09-20
/// @author     yangtao
/// @version    1.2.0
///
/// ================================================================

#ifndef ISOFT_IPC_PACKET_CPP_H
#define ISOFT_IPC_PACKET_CPP_H

#include "isoft/ipc/packet.h"

namespace isoft {
namespace ipc {

typedef void (*IPCFreeFunc)(void* context, uint8_t* addr, size_t size);

typedef uint64_t IPCSessionId;

class IPCPacketInner;
class IPCPacketBufferReference;
class IPCPacket
{
public:
    static IPCPacket* Release(IPCPacket* packet);
    /**
     * @brief Get the buffer address from the packet
     * @return buffer address
     */
    IPCPacketBufferReference* GetBuffer();

    /**
     * @brief Get the next buffer address of the buffer
     * @return buffer address
     */
    IPCPacketBufferReference* GetNextBuffer(IPCPacketBufferReference* buffer);

    /**
     * @brief Expand the system buffer
     * @param len Size of the buffer to expand
     * @return New buffer address
     */
    IPCPacketBufferReference* AppendBuffer(const size_t& len);

    /**
     * @brief Expand the user buffer
     * @param buf User-defined buffer
     * @param len Buffer size
     * @param free Method to destroy the buffer
     * @return New buffer address
     */
    IPCPacketBufferReference* AppendUserBuffer(void* buf, const size_t& len, IPCFreeFunc free);

    /**
     */
    void Ref();

    /**
     * @brief Get the sending process pid from the packet
     * @return Process pid
     */
    uint64_t GetPeerPid();

    /**
     * @brief Get the session id from the packet
     * @return session pid
     */
    IPCSessionId GetSessionId();

public:
    IPCPacket(const IPCPacket&) = delete;
    IPCPacket& operator=(const IPCPacket&) = delete;
    IPCPacket(IPCPacket&&)                 = delete;
    IPCPacket& operator=(IPCPacket&&) = delete;
    virtual ~IPCPacket();

protected:
    friend class IPCClient;
    friend class IPCServer;
    friend class IPCClientInner;
    friend class IPCServerInner;
    friend class IPCPacketInnerManager;

    IPCPacket(ipc_packet_t* data);
    static IPCPacket* Create(ipc_packet_t* packet);

    ipc_packet_t* GetData();
    void SetRefPacket(IPCPacket* refPacket);

private:
    IPCPacketInner* _packetInner;
};
}  // namespace ipc
}  // namespace isoft

#endif
