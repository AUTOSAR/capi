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
/// @file       hcaext_server.cpp
/// @brief
/// @details
/// @date       2024-06-20
/// @author     wangyanlong
/// @version    1.2.0
///
/// ================================================================

#include "ara/phm/internal/com/hcaext_server.h"

#include <isoft/ipccpp/buffer.h>

#include "ara/phm/internal/phm_log.h"
#include "ara/phm/internal/types.h"

namespace ara {
namespace phm {
namespace internal {
namespace hcaextcom {

/// @brief Server handler shall be implemented by this class.
/// @param packet contains data.
void Server::ServerHandler(isoft::ipc::IPCPacket* packet) noexcept
{
    _SetPacket(packet);

    isoft::ipc::IPCPacketBufferReference* const ipcBuffer{packet->GetBuffer()};
    if (nullptr == ipcBuffer) {
        LOG_ERROR << "ipc buffer is null.";
        return;
    }
    uint8_t* const bufPtr{ipcBuffer->GetPtr()};
    size_t len{ipcBuffer->GetLen()};
    Chunk data{bufPtr, bufPtr + len};
    if (messageSerializer_.Deserialize(data) != 0) {
        len = ipcBuffer->GetLen();
        LOG_ERROR << "Deserialize error, addr " << static_cast< char* >(static_cast< void* >(ipcBuffer->GetPtr()))
                  << ", len " << len;
        return;
    }
    Message const msg{messageSerializer_.GetMsg()};
    if (userHandler_) {
        userHandler_(msg.identifier, msg.eventType);
    } else {
        LOG_ERROR << "userHandler_ is null.";
    }
}

}  // namespace hcaextcom
}  // namespace internal
}  // namespace phm
}  // namespace ara