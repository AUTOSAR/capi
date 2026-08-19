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
/// @file       hc_client.h
/// @brief      used by SupervisedEntity to send request.
/// @details
/// @date       2024-06-05
/// @author     wangyanlong
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/PlatformHealthManagement/HealthChannelCom
/// @unit_description=used by SupervisedEntity to send request.
/// @trace_id_sr=SR_PHM_01024
/// @unit_name=Client
/// @interface_level=module
/// @endcode
///
/// ================================================================

#ifndef ARA_PHM_INTERNAL_HCCOM_CLIENT_H_
#define ARA_PHM_INTERNAL_HCCOM_CLIENT_H_

#include <memory>

#include "ara/phm/internal/com/base_client.h"
#include "ara/phm/internal/com/hc_message.h"
#include "ara/phm/internal/types.h"

namespace ara {
namespace phm {
namespace internal {
namespace hccom {

/// @brief Base client inherited by Client.
/// @trace_id_sr=SR_PHM_01024
/// @needwork = no
using BaseClient = ara::phm::internal::com::common::BaseClient;

/// @brief Client of hccom.
/// @trace_id_sr=SR_PHM_01024
/// @needwork = ad
class Client : public BaseClient
{
public:
    /// @brief Creation of Client, shared singleton.
    /// @return shared ptr of Client.
    /// @trace_id_sr=SR_PHM_01024
    /// @needwork = ad
    static std::shared_ptr< Client > GetInstanceShared()
    {
        static std::shared_ptr< Client > s_Instance{nullptr};
        if (s_Instance.get() == nullptr) {
            static std::once_flag s_Flag{};
            std::call_once(s_Flag, []() { s_Instance.reset(new Client()); });
            std::ignore = s_Flag;
        }
        return s_Instance;
    }

    /// @brief Default deconstructor.
    /// @trace_id_sr=SR_PHM_01024
    /// @needwork = ad
    ~Client() noexcept override = default;

    /// @brief The copy constructor shall not be used.
    /// @param obj The object to be copied.
    /// @trace_id_sr=SR_PHM_01024
    /// @needwork = ad
    Client(Client& obj) = delete;

    /// @brief The copy assignment shall not be used.
    /// @param obj The object to be copied.
    /// @return The copied object.
    /// @trace_id_sr=SR_PHM_01024
    /// @needwork = ad
    Client& operator=(Client const& obj) = delete;

    /// @brief The move constructor shall not be used.
    /// @param obj The object to be moved.
    /// @trace_id_sr=SR_PHM_01024
    /// @needwork = ad
    Client(Client&& obj) = delete;

    /// @brief The move assignment shall not be used.
    /// @param obj The object to be moved.
    /// @return The moved object.
    /// @trace_id_sr=SR_PHM_01024
    /// @needwork = ad
    Client& operator=(Client const&& obj) = delete;

    /// @brief Report health status to HealthChannelManager.
    /// @param specifierId instance specifier.
    /// @param healthStatusId health status id
    /// @return 0 sucess; other fail.
    /// @trace_id_sr=SR_PHM_01024
    /// @needwork = ad
    int32_t ReportHealthStatus(Specifier const& specifierId, HealthStatus const healthStatusId) noexcept;

private:
    /// @brief Construct a new Client object.
    /// @trace_id_sr=SR_PHM_01024
    /// @needwork = dda
    Client() noexcept : BaseClient{GetIpcPhmHcCmClient()}, messageSerializer_{} {};

private:
    /// @brief message serializer.
    /// @trace_id_sr=SR_PHM_01024
    /// @needwork = dda
    MessageSerializer messageSerializer_;
};  // class Client

}  // namespace hccom
}  // namespace internal
}  // namespace phm
}  // namespace ara

#endif  // ARA_PHM_INTERNAL_HCCOM_CLIENT_H_
