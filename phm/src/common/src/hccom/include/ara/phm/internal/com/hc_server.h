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
/// @file       hc_server.h
/// @brief      used by HealthChannelManager to receive request.
/// @details
/// @date       2024-06-05
/// @author     wangyanlong
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/PlatformHealthManagement/HealthChannelCom
/// @unit_description=used by HealthChannelManager to receive request.
/// @trace_id_sr=SR_PHM_01024,SR_PHM_01026
/// @unit_name=Server
/// @interface_level=module
/// @endcode
///
/// ================================================================

#ifndef ARA_PHM_INTERNAL_HCCOM_SERVER_H_
#define ARA_PHM_INTERNAL_HCCOM_SERVER_H_

#include <functional>
#include <memory>

#include "ara/phm/internal/com/base_server.h"
#include "ara/phm/internal/com/hc_message.h"

namespace ara {
namespace phm {
namespace internal {
namespace hccom {

/// @brief Base server inherited by Server.
/// @trace_id_sr=SR_PHM_01028
/// @needwork = no
using BaseServer = ara::phm::internal::com::common::BaseServer;

/// @brief Server of hccom.
/// @trace_id_sr=SR_PHM_01024,SR_PHM_01026
/// @needwork = ad
class Server : public BaseServer
{
public:
    /// @brief Definition of a function, it defines user handler to process health status from AA.
    /// @trace_id_sr=SR_PHM_01024,SR_PHM_01026
    /// @needwork = no
    using UserHandler = std::function< void(
        Specifier const& specifierId, ProcessId const processId, HealthStatus const healthStatusId) >;

    /// @brief Creation of Server, unique singleton.
    /// @param userHandler user handler to process server event.
    /// @return std::unique_ptr<Server> unique ptr of Server.
    /// @trace_id_sr=SR_PHM_01024,SR_PHM_01026
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
    /// @trace_id_sr=SR_PHM_01024,SR_PHM_01026
    /// @needwork = ad
    ~Server() noexcept override = default;

    /// @brief The copy constructor shall not be used.
    /// @param obj The object to be copied.
    /// @trace_id_sr=SR_PHM_01024,SR_PHM_01026
    /// @needwork = ad
    Server(Server& obj) = delete;

    /// @brief The copy assignment shall not be used.
    /// @param obj The object to be copied.
    /// @return The copied object.
    /// @trace_id_sr=SR_PHM_01024,SR_PHM_01026
    /// @needwork = ad
    Server& operator=(Server const& obj) = delete;

    /// @brief The move constructor shall not be used.
    /// @param obj The object to be moved.
    /// @trace_id_sr=SR_PHM_01024,SR_PHM_01026
    /// @needwork = ad
    Server(Server&& obj) = delete;

    /// @brief The move assignment rver shall not be used.
    /// @param obj The object to be moved.
    /// @return The moved object.
    /// @trace_id_sr=SR_PHM_01024,SR_PHM_01026
    /// @needwork = ad
    Server& operator=(Server const&& obj) = delete;

    /// @brief Server handler shall be implemented by this class.
    /// @param packet contains data.
    /// @trace_id_sr=SR_PHM_01024,SR_PHM_01026
    /// @needwork = ad
    void ServerHandler(isoft::ipc::IPCPacket* packet) noexcept override;

private:
    /// @brief Constructor of Server.
    /// @param userHandler user handler to process event of server.
    /// @trace_id_sr=SR_PHM_01024,SR_PHM_01026
    /// @needwork = dda
    explicit Server(UserHandler userHandler) noexcept
        : BaseServer{GetIpcPhmHcCmServer()}, userHandler_{std::move(userHandler)}, messageSerializer_{} {};

private:
    /// @brief user handler.
    /// @trace_id_sr=SR_PHM_01024,SR_PHM_01026
    /// @needwork = dda
    UserHandler userHandler_;

    /// @brief message serializer.
    /// @trace_id_sr=SR_PHM_01024,SR_PHM_01026
    /// @needwork = dda
    MessageSerializer messageSerializer_;
};  // class Server

}  // namespace hccom
}  // namespace internal
}  // namespace phm
}  // namespace ara

#endif  // ARA_PHM_INTERNAL_HCCOM_SERVER_H_
