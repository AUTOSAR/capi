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
/// @file       hcaext_client.h
/// @brief      used by HealthChannelAction to send request(Offer/StopOffer) to PHM.
/// @details
/// @date       2024-06-05
/// @author     wangyanlong
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/PlatformHealthManagement/HealthChannelActionExtendedCom
/// @unit_description=used by HealthChannelAction to send request(Offer/StopOffer) to PHM.
/// @trace_id_sr=SR_PHM_01028
/// @unit_name=Client
/// @interface_level=module
/// @endcode
///
/// ================================================================

#ifndef ARA_PHM_INTERNAL_HCAEXTCOM_CLIENT_H_
#define ARA_PHM_INTERNAL_HCAEXTCOM_CLIENT_H_

#include <ara/core/string_view.h>

#include <memory>

#include "ara/phm/internal/com/base_client.h"
#include "ara/phm/internal/com/hcaext_message.h"
#include "ara/phm/internal/types.h"

namespace ara {
namespace phm {
namespace internal {
namespace hcaextcom {

/// @brief Base client inherited by Client.
/// @trace_id_sr=SR_PHM_01028
/// @needwork = no
using BaseClient = ara::phm::internal::com::common::BaseClient;

/// @brief Client of hcaextcom.
/// @trace_id_sr=SR_PHM_01028
/// @needwork = ad
class Client : public BaseClient
{
public:
    /// @brief Creation of Client, shared singleton.
    /// @return shared ptr of Client.
    /// @trace_id_sr=SR_PHM_01028
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
    /// @trace_id_sr=SR_PHM_01028
    /// @needwork = ad
    ~Client() noexcept override = default;

    /// @brief The copy constructor shall not be used.
    /// @param obj The object to be copied.
    /// @trace_id_sr=SR_PHM_01028
    /// @needwork = ad
    Client(Client& obj) = delete;

    /// @brief The copy assignment shall not be used.
    /// @param obj The object to be copied.
    /// @return The copied object.
    /// @trace_id_sr=SR_PHM_01028
    /// @needwork = ad
    Client& operator=(Client const& obj) = delete;

    /// @brief The move constructor shall not be used.
    /// @param obj The object to be moved.
    /// @trace_id_sr=SR_PHM_01028
    /// @needwork = ad
    Client(Client&& obj) = delete;

    /// @brief The move assignment shall not be used.
    /// @param obj The object to be moved.
    /// @return The moved object.
    /// @trace_id_sr=SR_PHM_01028
    /// @needwork = ad
    Client& operator=(Client const&& obj) = delete;

    /// @brief When Offer, permit the recoveryhandler to be called.
    /// @param instance instance specifier of health channel action.
    /// @return int32_t 0 success; other failed.
    /// @trace_id_sr=SR_PHM_01028
    /// @needwork = ad
    int32_t Offer(ara::core::String const& instance) noexcept;

    /// @brief When StopOffer, do not permit the recoveryhandler to be called.
    /// @param instance instance specifier of health channel action.
    /// @return int32_t 0 success; other failed.
    /// @trace_id_sr=SR_PHM_01028
    /// @needwork = ad
    int32_t StopOffer(ara::core::String const& instance) noexcept;

private:
    /// @brief Constructor of Client.
    /// @trace_id_sr=SR_PHM_01028
    /// @needwork = dda
    Client() noexcept : BaseClient{GetIpcPhmHcaExtCmClient()}, messageSerializer_{} {};

private:
    /// @brief message serializer.
    /// @trace_id_sr=SR_PHM_01028
    /// @needwork = dda
    MessageSerializer messageSerializer_;
};  // class Client

}  // namespace hcaextcom
}  // namespace internal
}  // namespace phm
}  // namespace ara

#endif  // ARA_PHM_INTERNAL_HCAEXTCOM_CLIENT_H_
