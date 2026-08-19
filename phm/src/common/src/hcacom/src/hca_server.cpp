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
/// @file       hca_server.cpp
/// @brief
/// @details
/// @date       2024-06-20
/// @author     wangyanlong
/// @version    1.2.0
///
/// ================================================================

#include "ara/phm/internal/com/hca_server.h"

#include <isoft/ipccpp/buffer.h>

#include <cerrno>
#include <cstring>

#include "ara/phm/internal/phm_log.h"

namespace ara {
namespace phm {
namespace internal {
namespace hcacom {
/// @brief To register handler of reported error msg from HealthChannelManager.
/// @param instance instance specifier of HealthChannelAction
/// @param recoveryHandler recovery handler
void Server::RegisterRecoveryHandler(ara::core::String const& instance, RecoveryHandler const& recoveryHandler) noexcept
{
    LOG_INFO << instance.c_str() << " register recovery handler.";

    std::shared_ptr< HealthChannelActionInfo > info{std::make_shared< HealthChannelActionInfo >()};
    info->isOffered              = false;
    info->recoveryHandler        = recoveryHandler;
    recoveryActionMap_[instance] = info;
}

/// @brief When Offer, permit the recoveryhandler to be called.
/// @param instance instance specifier of HealthChannelAction
void Server::Offer(ara::core::String const& instance) noexcept
{
    LOG_INFO << "offer " << instance.c_str();

    if (recoveryActionMap_.count(instance) > static_cast< size_t >(0)) {
        recoveryActionMap_[instance]->isOffered = true;
    } else {
        LOG_ERROR << instance.c_str() << " not exist in recoveryActionMap_.";
    }
}

/// @brief When StopOffer, do not permit the recoveryhandler to be called.
/// @param instance instance specifier of HealthChannelAction
void Server::StopOffer(ara::core::String const& instance) noexcept
{
    LOG_INFO << "stop offer " << instance.c_str();

    if (recoveryActionMap_.count(instance) > static_cast< size_t >(0)) {
        recoveryActionMap_[instance]->isOffered = false;
    } else {
        LOG_ERROR << instance.c_str() << " not exist in recoveryActionMap_.";
    }
}

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

    size_t const len{ipcBuffer->GetLen()};
    uint8_t* const bufPtr{ipcBuffer->GetPtr()};
    Chunk data{bufPtr, bufPtr + len};
    if (messageSerializer_.Deserialize(data) != 0) {
        LOG_ERROR << "deserialize error, data len " << data.size();
        return;
    }
    Message const msg{messageSerializer_.GetMsg()};

    uint16_t const size{sizeof(uint32_t)};  // Keep consistent with the RecoveryResult data type
    isoft::ipc::IPCSessionId const sessionID{packet->GetSessionId()};
    isoft::ipc::IPCPacket* const response{this->_GetIpcServer()->MakeResponse(sessionID)};
    isoft::ipc::IPCPacketBufferReference* const buffer{response->AppendBuffer(size)};
    uint8_t* const buf{buffer->GetPtr()};
    std::ignore = buffer->SetLen(size);
    uint32_t recoveryResult{static_cast< uint32_t >(RecoveryResult::kSuccess)};
    if (recoveryActionMap_.count(msg.identifier) > 0U) {
        if (recoveryActionMap_[msg.identifier]->isOffered) {
            LOG_DEBUG << "call heath channel action handler of " << msg.identifier.c_str();
            recoveryActionMap_[msg.identifier]->recoveryHandler(msg.healthStatus);
            recoveryResult = static_cast< uint32_t >(RecoveryResult::kSuccess);
        } else {
            LOG_INFO << msg.identifier << " is not offered.";
            recoveryResult = static_cast< uint32_t >(RecoveryResult::kNotOffered);
        }
    } else {
        recoveryResult = static_cast< uint32_t >(RecoveryResult::kNotOffered);
        LOG_WARN << msg.identifier << " may not offered.";
    }

    std::ignore = memcpy(buf, &recoveryResult, size);
    if (_GetIpcServer()->Send(response, true) != 0) {
        /// TODO(wangyanlong): clang-tidy suggests that the std::strerror function may throw an exception, QAC does not recommend using errno
        LOG_ERROR << "send error:" << std::strerror(errno);  // NOLINT
    }
}

}  // namespace hcacom
}  // namespace internal
}  // namespace phm
}  // namespace ara
