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
/// @file       hca_server.h
/// @brief      to receive health channel status recover request from PHM.
/// @details
/// @date       2024-06-05
/// @author     wangyanlong
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/PlatformHealthManagement/HealthChannelActionCom
/// @unit_description=to receive health channel status recover request from PHM.
/// @trace_id_sr=SR_PHM_01028
/// @unit_name=Server
/// @interface_level=module
/// @endcode
///
/// ================================================================

#ifndef _COM_HCACOM_ARA_PHM_INTERNAL_HCACOM_SERVER_H_
#define _COM_HCACOM_ARA_PHM_INTERNAL_HCACOM_SERVER_H_

#include <ara/core/instance_specifier.h>
#include <ara/core/string_view.h>
#include <isoft/ipccpp/packet.h>

#include <functional>
#include <memory>

#include "ara/phm/internal/com/base_server.h"
#include "ara/phm/internal/com/hca_message.h"

namespace ara {
namespace phm {
namespace internal {
namespace hcacom {

/// @brief Base server inherited by Server.
/// @trace_id_sr=SR_PHM_01028
/// @needwork = no
using BaseServer = ara::phm::internal::com::common::BaseServer;

/// @brief Server of hcacom.
/// @trace_id_sr=SR_PHM_01028
/// @needwork = ad
class Server : public BaseServer
{
public:
    /// @brief Creation of Server, shared singleton.
    /// @code{.isoft}
    /// @traceid {}
    /// @threadsafety{ yes }
    /// @endcode
    /// @return shared ptr of Server.
    /// @trace_id_sr=SR_PHM_01028
    /// @needwork = ad
    static std::shared_ptr< Server > GetInstanceShared()
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
    /// @trace_id_sr=SR_PHM_01028
    /// @needwork = ad
    ~Server() noexcept override = default;

    /// @brief The copy constructor shall not be used.
    /// @param obj The object to be copied.
    /// @trace_id_sr=SR_PHM_01028
    /// @needwork = ad
    Server(Server& obj) = delete;

    /// @brief The copy assignment shall not be used.
    /// @param obj The object to be copied.
    /// @return The copied object.
    /// @trace_id_sr=SR_PHM_01028
    /// @needwork = ad
    Server& operator=(Server const& obj) = delete;

    /// @brief The move constructor shall not be used.
    /// @param obj The object to be moved.
    /// @trace_id_sr=SR_PHM_01028
    /// @needwork = ad
    Server(Server&& obj) = delete;

    /// @brief The move assignment shall not be used.
    /// @param obj The object to be moved.
    /// @return The moved object.
    /// @trace_id_sr=SR_PHM_01028
    /// @needwork = ad
    Server& operator=(Server const&& obj) = delete;

    /// @brief Server handler shall be implemented by this class.
    /// @param packet contains data.
    /// @trace_id_sr=SR_PHM_01028
    /// @needwork = ad
    void ServerHandler(isoft::ipc::IPCPacket* packet) noexcept override;

    /// @brief Definition of a function, it defines user handler to process error info msg from HealthChannelManager.
    /// @needwork = no
    using RecoveryHandler = std::function< void(HealthStatus healthStatusId) >;

    /// @brief infos of HealthChannelAction
    /// @trace_id_sr=SR_PHM_01028
    /// @needwork = ad
    struct HealthChannelActionInfo
    {
        /// @brief true, call recoveryhandler when receive error info from HealthChannelManager do not call
        bool isOffered{false};
        /// @brief recovery handler
        RecoveryHandler recoveryHandler;
    };

    /// @brief To register handler of reported error msg from HealthChannelManager.
    /// @param instance instance specifier of HealthChannelAction
    /// @param recoveryHandler recovery handler
    /// @trace_id_sr=SR_PHM_01028
    /// @needwork = ad
    void RegisterRecoveryHandler(ara::core::String const& instance, RecoveryHandler const& recoveryHandler) noexcept;

    /// @brief When Offer, permit the recoveryhandler to be called.
    /// @param instance instance specifier of HealthChannelAction
    /// @trace_id_sr=SR_PHM_01028
    /// @needwork = ad
    void Offer(ara::core::String const& instance) noexcept;

    /// @brief When StopOffer, do not permit the recoveryhandler to be called.
    /// @param instance instance specifier of HealthChannelAction
    /// @trace_id_sr=SR_PHM_01028
    /// @needwork = ad
    void StopOffer(ara::core::String const& instance) noexcept;

private:
    /// @brief Constructor.
    /// @trace_id_sr=SR_PHM_01028
    /// @needwork = dda
    Server() noexcept : BaseServer{GetIpcPhmHcaCmServer()}, recoveryActionMap_{}, messageSerializer_{} {};

private:
    /// @brief <instance of HealthChannelAction, health channel action info>
    /// @trace_id_sr=SR_PHM_01028
    /// @needwork = dda
    ara::core::Map< ara::core::String, std::shared_ptr< HealthChannelActionInfo > > recoveryActionMap_;

    /// @brief message serializer.
    /// @trace_id_sr=SR_PHM_01028
    /// @needwork = dda
    MessageSerializer messageSerializer_;
};  // class Server

}  // namespace hcacom
}  // namespace internal
}  // namespace phm
}  // namespace ara

#endif  // _COM_HCACOM_ARA_PHM_INTERNAL_HCACOM_SERVER_H_