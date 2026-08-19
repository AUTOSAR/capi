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
/// @file       sv_server.h
/// @brief      used by SupervisionManager to receive and process request.
/// @details
/// @date       2024-06-05
/// @author     wangyanlong
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/PlatformHealthManagement/SupervisionCom
/// @unit_description=used by SupervisionManager to receive and process request.
/// @trace_id_sr=SR_PHM_01003,SR_PHM_01005,SR_PHM_01007
/// @unit_name=Server
/// @interface_level=module
/// @endcode
///
/// ================================================================

#ifndef ARA_PHM_INTERNAL_SVCOM_SERVER_H_
#define ARA_PHM_INTERNAL_SVCOM_SERVER_H_

#include <cassert>
#include <functional>
#include <memory>

#include "ara/phm/internal/com/base_server.h"
#include "ara/phm/internal/com/sv_message.h"
#include "ara/phm/internal/types.h"

namespace ara {
namespace phm {
namespace internal {
namespace svcom {

/// @brief Base server inherited by Server.
/// @trace_id_sr=SR_PHM_01003,SR_PHM_01005,SR_PHM_01007
/// @needwork = no
using BaseServer = ara::phm::internal::com::common::BaseServer;

/// @brief Server of svcom.
/// @trace_id_sr=SR_PHM_01003,SR_PHM_01005,SR_PHM_01007
/// @needwork = ad
class Server : public BaseServer
{
public:
    /// @brief Definition of a function, it defines user handler to process resuest.
    /// @trace_id_sr=SR_PHM_01003,SR_PHM_01005,SR_PHM_01007
    /// @needwork = no
    using UserHandler = std::function< void(EventType const eventType,
                                            Specifier const specifierId,
                                            ProcessId const processId,
                                            uint32_t const checkpointId,
                                            ProcessClusterAffiliation const processClusterAffiliation,
                                            int64_t timestamp) >;

    /// @brief Creation of Server, unique singleton.
    /// @param userHandler user handler to process server event.
    /// @return std::unique_ptr<Server> unique ptr of Server.
    /// @trace_id_sr=SR_PHM_01003,SR_PHM_01005,SR_PHM_01007
    /// @needwork = ad
    static std::unique_ptr< Server > GetInstanceUnique(UserHandler const& userHandler)
    {
        static std::unique_ptr< Server > s_Instance{nullptr};
        static std::once_flag s_Flag{};
        std::call_once(s_Flag, [&userHandler]() { s_Instance.reset(new Server(userHandler)); });
        std::ignore = s_Flag;

        /// Exclusive singleton, only allowed to be obtained once
        assert(s_Instance);
        return std::move(s_Instance);
    }

    /// @brief Default deconstructor.
    /// @trace_id_sr=SR_PHM_01003,SR_PHM_01005,SR_PHM_01007
    /// @needwork = ad
    ~Server() noexcept override = default;

    /// @brief The copy constructor shall not be used.
    /// @param obj The object to be copied.
    /// @trace_id_sr=SR_PHM_01003,SR_PHM_01005,SR_PHM_01007
    /// @needwork = ad
    Server(Server& obj) = delete;

    /// @brief The copy assignment shall not be used.
    /// @param obj The object to be copied.
    /// @return The copied object.
    /// @trace_id_sr=SR_PHM_01003,SR_PHM_01005,SR_PHM_01007
    /// @needwork = ad
    Server& operator=(Server const& obj) = delete;

    /// @brief The move constructor shall not be used.
    /// @param obj The object to be moved.
    /// @trace_id_sr=SR_PHM_01003,SR_PHM_01005,SR_PHM_01007
    /// @needwork = ad
    Server(Server&& obj) = delete;

    /// @brief The move assignment shall not be used.
    /// @param obj The object to be moved.
    /// @return The moved object.
    /// @trace_id_sr=SR_PHM_01003,SR_PHM_01005,SR_PHM_01007
    /// @needwork = ad
    Server& operator=(Server const&& obj) = delete;

    /// @brief Server handler shall be implemented by this class.
    /// @param packet contains data.
    /// @trace_id_sr=SR_PHM_01003,SR_PHM_01005,SR_PHM_01007
    /// @needwork = ad
    void ServerHandler(isoft::ipc::IPCPacket* packet) noexcept override;

    ///@brief Reponse GlobalSupervisionStatus or LocalSupervisionStatus aginst the request of GetGlobalSupervisionStatus
    /// or GetLocalSupervisionStatus
    /// @param status GlobalSupervisionStatus or LocalSupervisionStatus
    /// @trace_id_sr=SR_PHM_01003,SR_PHM_01005,SR_PHM_01007
    /// @needwork = ad
    void Reply(SupervisionStatus const status) const noexcept;

private:
    /// @brief default constructor.
    /// @param userHandler user handler
    /// @trace_id_sr=SR_PHM_01003,SR_PHM_01005,SR_PHM_01007
    /// @needwork = dda
    explicit Server(UserHandler userHandler) noexcept
        : BaseServer{GetIpcPhmSvCmServer()}, kUserHandler{std::move(userHandler)}, messageSerializer_{} {};

private:
    /// @brief user handler to process event of server.
    /// @trace_id_sr=SR_PHM_01003,SR_PHM_01005,SR_PHM_01007
    /// @needwork = dda
    UserHandler const kUserHandler;

    /// @brief message serializer.
    /// @trace_id_sr=SR_PHM_01003,SR_PHM_01005,SR_PHM_01007
    /// @needwork = dda
    MessageSerializer messageSerializer_;
};  // class Server

}  // namespace svcom
}  // namespace internal
}  // namespace phm
}  // namespace ara

#endif  // ARA_PHM_INTERNAL_SVCOM_SERVER_H_
