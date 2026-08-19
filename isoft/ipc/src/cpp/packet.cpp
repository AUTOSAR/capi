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
/// @file       packet.cpp
/// @brief
/// @details
/// @date       2022-09-20
/// @author     yangtao
/// @version    1.2.0
///
/// ================================================================

#include "isoft/ipccpp/packet.h"

#include "isoft/ipccpp/buffer.h"
#include "packetinner.h"

namespace isoft {
namespace ipc {

IPCPacket::IPCPacket(ipc_packet_t* packet) { _packetInner = new IPCPacketInner(packet, this); }

IPCPacket::~IPCPacket()
{
    if (this->_packetInner != nullptr)
        delete this->_packetInner;
    this->_packetInner = nullptr;
}

IPCPacket* IPCPacket::Create(ipc_packet_t* raw)
{
    if (raw != nullptr) {
        IPCPacket* packet = new IPCPacket(raw);
        IPCPacketInnerManager::GetInstance()->AppendPacket(packet);
        return packet;
    }
    return nullptr;
}

IPCPacket* IPCPacket::Release(IPCPacket* packet)
{
    if (packet == nullptr) {
        return nullptr;
    } else {
        if (packet->GetData() != nullptr)
            ipc_packet_release(packet->GetData());

        return IPCPacketInnerManager::GetInstance()->ReleasePacket(packet);
    }
}

IPCPacketBufferReference* IPCPacket::GetBuffer()
{
    if (this->GetData() != nullptr) {
        ipc_buffer_t* bufC = ipc_packet_get_buffer(this->GetData());
        if (bufC != nullptr) {
            return static_cast< IPCPacketBufferReference* >((nai_buf_s*)bufC);
        }
    }
    return nullptr;
}

IPCPacketBufferReference* IPCPacket::AppendBuffer(const size_t& len)
{
    if (this->GetData() != nullptr) {
        ipc_buffer_t* bufC = ipc_packet_append_buffer(this->GetData(), len);
        if (bufC != nullptr) {
            return static_cast< IPCPacketBufferReference* >((nai_buf_s*)bufC);
        }
    }
    return nullptr;
}

IPCPacketBufferReference* IPCPacket::AppendUserBuffer(void* buf, const size_t& len, ipc_free_func_t free)
{
    if (this->GetData() != nullptr) {
        ipc_buffer_t* bufC = ipc_packet_append_user_buffer(this->GetData(), buf, len, free);
        if (bufC != nullptr) {
            return static_cast< IPCPacketBufferReference* >((nai_buf_s*)bufC);
        }
    }
    return nullptr;
}

void IPCPacket::Ref()
{
    if (this->GetData() != nullptr)
        this->_packetInner->SetPacket(ipc_packet_ref(this->GetData()));

    _packetInner->Ref();
}

uint64_t IPCPacket::GetPeerPid()
{
    if (this->GetData() != nullptr)
        return ipc_packet_get_peer_pid(this->GetData());
    return 0;
}

IPCSessionId IPCPacket::GetSessionId()
{
    if (this->GetData() != nullptr)
        return ipc_packet_get_session_id(this->GetData());
    return 0;
}

ipc_packet_t* IPCPacket::GetData()
{
    if (_packetInner != nullptr)
        return _packetInner->_packetC;
    return nullptr;
}

IPCPacketBufferReference* IPCPacket::GetNextBuffer(IPCPacketBufferReference* buffer)
{
    if (this->GetData() != nullptr) {
        ipc_buffer_t* nextC = ipc_buffer_get_next(this->GetData(), (ipc_buffer_t*)static_cast< nai_buf_s* >(buffer));
        if (nextC != nullptr)
            return static_cast< IPCPacketBufferReference* >((nai_buf_s*)nextC);
    }
    return nullptr;
}

void IPCPacket::SetRefPacket(IPCPacket* refPacket)
{
    if (_packetInner != nullptr)
        _packetInner->_refPacket = refPacket;
}

}  // namespace ipc
}  // namespace isoft
