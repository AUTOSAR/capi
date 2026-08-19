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
/// @file       ra_server.h
/// @brief      used by SupervisionManager to receive request.
/// @details
/// @date       2024-06-05
/// @author     wangyanlong
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/PlatformHealthManagement/RecoveryActionCom
/// @unit_description=used by SupervisionManager to receive request.
/// @trace_id_sr=SR_PHM_01027
/// @unit_name=Server
/// @interface_level=module
/// @endcode
///
/// ================================================================

#ifndef ARA_PHM_INTERNAL_RACOM_SERVER_H_
#define ARA_PHM_INTERNAL_RACOM_SERVER_H_

#include <ara/core/instance_specifier.h>
#include <ara/core/result.h>
#include <ara/exec/execution_error_event.h>
#include <isoft/ipccpp/buffer.h>

#include <cstring>
#include <functional>
#include <memory>

#include "ara/phm/internal/com/base_server.h"
#include "ara/phm/internal/com/ra_message.h"
#include "ara/phm/internal/phm_log.h"

namespace ara {
namespace phm {
namespace internal {
namespace racom {

/// @brief Base server inherited by Server.
/// @trace_id_sr=SR_PHM_01027
/// @needwork = no
using BaseServer = ara::phm::internal::com::common::BaseServer;

/// @brief Server of racom.
/// @trace_id_sr=SR_PHM_01027
/// @needwork = ad
template < typename SupervisionT >
class Server : public BaseServer
{
public:
    /// @brief Creation of Server, shared singleton.
    /// @return shared ptr of Server
    /// @trace_id_sr=SR_PHM_01027
    /// @needwork = ad
    static std::shared_ptr< Server > GetInstanceShared() noexcept
    {
        static std::shared_ptr< Server > s_Instance{nullptr};
        if (s_Instance.get() == nullptr) {
            static std::once_flag s_Flag{};
            std::call_once(s_Flag, []() { s_Instance.reset(new Server()); });
            std::ignore = s_Flag;
        }
        return s_Instance;
    }

    /// @brief Default deconstructor.
    /// @trace_id_sr=SR_PHM_01027
    /// @needwork = ad
    ~Server() noexcept override = default;

    /// @brief The copy constructor shall not be used.
    /// @param obj The object to be copied.
    /// @trace_id_sr=SR_PHM_01027
    /// @needwork = ad
    Server(Server& obj) = delete;

    /// @brief The copy assignment shall not be used.
    /// @param obj The object to be copied.
    /// @return The copied object.
    /// @trace_id_sr=SR_PHM_01027
    /// @needwork = ad
    Server& operator=(Server const& obj) = delete;

    /// @brief The move constructor for Server shall not be used.
    /// @param obj The object to be moved.
    /// @trace_id_sr=SR_PHM_01027
    /// @needwork = ad
    Server(Server&& obj) = delete;

    /// @brief The move assignment shall not be used.
    /// @param obj The object to be moved.
    /// @return The moved object.
    /// @trace_id_sr=SR_PHM_01027
    /// @needwork = ad
    Server& operator=(Server const&& obj) = delete;

    /// @brief Server handler shall be implemented by this class.
    /// @param packet contains data.
    /// @trace_id_sr=SR_PHM_01027
    /// @needwork = ad
    void ServerHandler(isoft::ipc::IPCPacket* packet) noexcept override
    {
        if (packet == nullptr) {
            LOG_ERROR << "racom packet is null.";
            return;
        }
        _SetPacket(packet);

        isoft::ipc::IPCPacketBufferReference* const ipcBuffer{packet->GetBuffer()};
        if (nullptr == ipcBuffer) {
            LOG_ERROR << "ipc buffer is null.";
            return;
        }
        uint8_t* const bufPtr{ipcBuffer->GetPtr()};
        Chunk data{bufPtr, bufPtr + ipcBuffer->GetLen()};
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
                LOG_INFO << "call recovery action handler of " << msg.identifier;

                ara::core::Result< ara::exec::FunctionGroup::CtorToken > const groupTokenResult{
                    ara::exec::FunctionGroup::Preconstruct(
                        std::move(ara::core::String(msg.functionGroup.data(), msg.functionGroup.size())))};
                if (!groupTokenResult.HasValue()) {
                    LOG_ERROR << "pre construct function group " << msg.functionGroup
                              << " error, this function group may be invalid.";
                    return;
                }
                ara::exec::FunctionGroup::CtorToken groupToken{groupTokenResult.ValueOrThrow()};
                ara::exec::ExecutionErrorEvent const eventMsg{msg.executionError, std::move(groupToken)};
                recoveryActionMap_[msg.identifier]->recoveryHandler(eventMsg,
                                                                    static_cast< SupervisionT >(msg.supervisionType));

                recoveryResult = static_cast< uint32_t >(RecoveryResult::kSuccess);
            } else {
                LOG_INFO << msg.identifier << " is not offered.";
                recoveryResult = static_cast< uint32_t >(RecoveryResult::kNotOffered);
            }
        } else {
            // Not found, likely because it was not offered, not created, also means not offered
            recoveryResult = static_cast< uint32_t >(RecoveryResult::kNotOffered);
            LOG_WARN << msg.identifier << " may not offered.";
        }

        std::ignore = memcpy(buf, &recoveryResult, size);
        if (_GetIpcServer()->Send(response, true) != 0) {
            /// TODO(wangyanlong): clang-tidy suggests that the std::strerror function may throw an exception, QAC does not recommend using errno
            LOG_ERROR << "send error:" << std::strerror(errno);  // NOLINT
        }
    }

    /// @brief Definition of a function, it defines user handler to process error info msg from SupervisionManager.
    /// @trace_id_sr=SR_PHM_01027
    /// @needwork = no
    using RecoveryHandler
        = std::function< void(ara::exec::ExecutionErrorEvent const& executionError, SupervisionT supervision) >;

    /// @brief infos of RecoveryAction
    /// @trace_id_sr=SR_PHM_01027
    /// @needwork = ad
    struct RecoveryActionInfo
    {
        /// @brief true, call recoveryhandler when receive error info from SupervisionManager，false do not  call.
        bool isOffered{false};
        /// @brief recovery handler.
        RecoveryHandler recoveryHandler;
    };

    /// @brief To register handler of reported error msg from SupervisionManager.
    /// @param instance instance specifier of recovery action.
    /// @param recoveryHandler recovery handler.
    /// @trace_id_sr=SR_PHM_01027
    /// @needwork = ad
    void RegisterRecoveryHandler(ara::core::String const& instance, RecoveryHandler const recoveryHandler) noexcept
    {
        LOG_INFO << instance.c_str() << "register recovery handler.";
        std::shared_ptr< RecoveryActionInfo > info{std::move(std::make_shared< RecoveryActionInfo >())};
        info->isOffered              = false;
        info->recoveryHandler        = recoveryHandler;
        recoveryActionMap_[instance] = info;
    }

    /// @brief When Offer, permit the recoveryhandler to be called.
    /// @param instance  instance specifier of recovery action.
    /// @trace_id_sr=SR_PHM_01027
    /// @needwork = ad
    void Offer(ara::core::String const& instance) noexcept
    {
        LOG_INFO << "offer " << instance.c_str();

        if (recoveryActionMap_.count(instance) > static_cast< size_t >(0)) {
            recoveryActionMap_[instance]->isOffered = true;
        } else {
            LOG_ERROR << instance.c_str() << " not exist in recoveryActionMap_.";
        }
    }

    /// @brief When StopOffer, do not permit the recoveryhandler to be called.
    /// @param instance instance specifier of recovery action.
    /// @trace_id_sr=SR_PHM_01027
    /// @needwork = ad
    void StopOffer(ara::core::String const& instance) noexcept
    {
        LOG_INFO << "stop offer " << instance.c_str();

        if (recoveryActionMap_.count(instance) > static_cast< size_t >(0)) {
            recoveryActionMap_[instance]->isOffered = false;
        } else {
            LOG_ERROR << instance.c_str() << " not exist in recoveryActionMap_.";
        }
    }

private:
    /// @brief Constructor.
    /// @trace_id_sr=SR_PHM_01027
    /// @needwork = dda
    Server() noexcept : BaseServer{GetIpcPhmRaCmServer()} {};

private:
    /// @brief <instance, RecoveryActionInfo>.
    /// @trace_id_sr=SR_PHM_01027
    /// @needwork = dda
    ara::core::Map< ara::core::String, std::shared_ptr< RecoveryActionInfo > > recoveryActionMap_{};

    /// @brief message serializer.
    /// @trace_id_sr=SR_PHM_01027
    /// @needwork = dda
    MessageSerializer messageSerializer_{};
};  // class Server

}  // namespace racom
}  // namespace internal
}  // namespace phm
}  // namespace ara

#endif  // ARA_PHM_INTERNAL_RACOM_SERVER_H_
