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
/// @file       sv_server.cpp
/// @brief
/// @details
/// @date       2024-06-20
/// @author     wangyanlong
/// @version    1.2.0
///
/// ================================================================

#include "ara/phm/internal/com/sv_server.h"

#include <isoft/ipccpp/buffer.h>

#include <cstring>

#include "ara/phm/internal/phm_log.h"
#include "ara/phm/internal/types.h"

namespace ara {
namespace phm {
namespace internal {
namespace svcom {

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
    Chunk data{bufPtr, bufPtr + ipcBuffer->GetLen()};
    if (messageSerializer_.Deserialize(data) != 0) {
        LOG_ERROR << "Deserialize error, addr " << static_cast< char* >(static_cast< void* >(bufPtr)) << ", len "
                  << ipcBuffer->GetLen();
        return;
    }
    Message const msg{messageSerializer_.GetMsg()};

    if (kUserHandler) {
        kUserHandler(msg.eventType, msg.instance, msg.pid, msg.checkpointId, msg.processClusterAffiliation,
                     msg.timestamp);
    } else {
        LOG_ERROR << "kUserHandler is null.";
    }
}

///@brief Reponse GlobalSupervisionStatus or LocalSupervisionStatus aginst the request of GetGlobalSupervisionStatus
/// or GetLocalSupervisionStatus
/// @param status GlobalSupervisionStatus or LocalSupervisionStatus
void Server::Reply(SupervisionStatus const status) const noexcept
{
    if (_GetPacket() == nullptr) {
        LOG_ERROR << "svcom ipc server is null";
        return;
    }
    isoft::ipc::IPCPacket* const& packet{_GetPacket()};
    isoft::ipc::IPCSessionId const sessionID{packet->GetSessionId()};
    isoft::ipc::IPCPacket* const response{_GetIpcServer()->MakeResponse(sessionID)};
    if (response == nullptr) {
        LOG_ERROR << "response is null, session id " << sessionID;
        return;
    }

    isoft::ipc::IPCPacketBufferReference* const buffer{
        response->AppendBuffer(static_cast< size_t >(sizeof(SupervisionStatus)))};
    if (buffer == nullptr) {
        LOG_ERROR << "buffer is null.";
        return;
    }

    uint8_t* const buf{buffer->GetPtr()};
    if (buf == nullptr) {
        LOG_ERROR << "buf is null.";
        return;
    }

    std::ignore = memcpy(buf, &status, sizeof(SupervisionStatus));
    std::ignore = buf;
    if (buffer->SetLen(sizeof(SupervisionStatus)) == -1) {
        /// TODO(wangyanlong): clang-tidy suggests that the std::strerror function may throw an exception, QAC does not recommend using errno
        LOG_ERROR << "set buffer len error: " << std::strerror(errno);  // NOLINT
        return;
    }

    if (_GetIpcServer()->Send(response, true) == -1) {
        /// TODO(wangyanlong): clang-tidy suggests that the std::strerror function may throw an exception, QAC does not recommend using errno
        LOG_ERROR << "ipc send error: " << std::strerror(errno);  // NOLINT
        return;
    }
}

}  // namespace svcom
}  // namespace internal
}  // namespace phm
}  // namespace ara