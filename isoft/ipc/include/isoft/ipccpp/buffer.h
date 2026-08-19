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
/// @file       buffer.h
/// @brief
/// @details
/// @date       2022-09-20
/// @author     yangtao
/// @version    1.2.0
///
/// ================================================================

#ifndef ISOFT_IPC_PACKET_BUFFER_CPP_H
#define ISOFT_IPC_PACKET_BUFFER_CPP_H

#include <nai/io/nai_buf.h>

#include "isoft/ipc/packet.h"
namespace isoft {
namespace ipc {

class IPCPacketBufferReference final : public nai_buf_s
{
private:
    ipc_buffer_t* GetParent() { return reinterpret_cast< ipc_buffer_t* >(this); }

public:
    /**
     * @brief Get the buffer address
     * @return buffer address
     */
    uint8_t* GetPtr();

    /**
     * @brief Get the used length of the buffer
     * @return Used length of the buffer
     */
    size_t GetLen();

    /**
     * @brief Set the used length of the buffer
     * @param len Used length of the buffer
     * @return Returns 0 on success, returns -1 and sets errno on failure
     */
    int SetLen(const size_t& len);

    /**
     * @brief Get the allocated length of the buffer
     * @return Allocated length
     */
    size_t GetSize();

protected:
    friend class IPCPacket;
    ~IPCPacketBufferReference() = default;
    IPCPacketBufferReference()  = default;
};
}  // namespace ipc
}  // namespace isoft

#endif
