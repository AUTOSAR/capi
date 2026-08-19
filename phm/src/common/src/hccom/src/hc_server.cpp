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
/// @file       hc_server.cpp
/// @brief
/// @details
/// @date       2024-06-20
/// @author     wangyanlong
/// @version    1.2.0
///
/// ================================================================

#include "ara/phm/internal/com/hc_server.h"

#include <isoft/ipccpp/buffer.h>

#include "ara/phm/internal/phm_log.h"

namespace ara {
namespace phm {
namespace internal {
namespace hccom {

/// @brief Server handler shall be implemented by this class.
/// @param packet contains data.
void Server::ServerHandler(isoft::ipc::IPCPacket* packet) noexcept
{
    _SetPacket(packet);

    isoft::ipc::IPCPacketBufferReference* const ipcBuffer{packet->GetBuffer()};
    if (nullptr == ipcBuffer) {
        LOG_ERROR << "ipcBuffer is null.";
        return;
    }
    uint8_t* const bufPtr{ipcBuffer->GetPtr()};
    Chunk data{bufPtr, bufPtr + ipcBuffer->GetLen()};
    if (messageSerializer_.Deserialize(data) != 0) {
        LOG_ERROR << "Deserialize error, addr " << static_cast< char* >(static_cast< void* >(bufPtr)) << ", len "
                  << ipcBuffer->GetLen();
        return;
    }

    Message const msg{messageSerializer_.GetMsg()};
    if (userHandler_) {
        userHandler_(msg.instance, msg.processId, msg.healthStatus);
    } else {
        LOG_ERROR << "userHandler_ is null.";
    }
}

}  // namespace hccom
}  // namespace internal
}  // namespace phm
}  // namespace ara
