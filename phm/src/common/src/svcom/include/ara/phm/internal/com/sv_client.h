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
/// @file       sv_client.h
/// @brief      used by SupervisedEntity to send request.
/// @details
/// @date       2024-06-05
/// @author     wangyanlong
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/PlatformHealthManagement/SupervisionCom
/// @unit_description=used by SupervisedEntity to send request.
/// @trace_id_sr=SR_PHM_01001
/// @unit_name=Client
/// @interface_level=module
/// @endcode
///
/// ================================================================

#ifndef ARA_PHM_INTERNAL_SVCOM_CLIENT_H_
#define ARA_PHM_INTERNAL_SVCOM_CLIENT_H_

#include <isoft/ara_fsh/filesystem_hierarchy.h>
#include <isoft/ipccpp/client.h>
#include <isoft/naicpp/evloop.h>
#include <isoft/naicpp/global_evloop.h>

#include <memory>

#include "ara/phm/internal/com/base_client.h"
#include "ara/phm/internal/com/sv_message.h"
#include "ara/phm/internal/types.h"

namespace ara {
namespace phm {
namespace internal {
namespace svcom {

/// @brief Base client inherited by Client.
/// @trace_id_sr=SR_PHM_01001
/// @needwork = no
using BaseClient = ara::phm::internal::com::common::BaseClient;

/// @brief Client of svcom.
/// @trace_id_sr=SR_PHM_01001
/// @needwork = ad
class Client : public BaseClient
{
public:
    /// @brief Creation of Client, shared singleton.
    /// @return shared ptr of Client
    /// @trace_id_sr=SR_PHM_01001
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
    /// @trace_id_sr=SR_PHM_01001
    /// @needwork = ad
    ~Client() noexcept override = default;

    /// @brief The copy constructor shall not be used.
    /// @param obj The object to be copied.
    /// @trace_id_sr=SR_PHM_01001
    /// @needwork = ad
    Client(Client& obj) = delete;

    /// @brief The copy assignment shall not be used.
    /// @param obj The object to be copied.
    /// @return The copied object.
    /// @trace_id_sr=SR_PHM_01001
    /// @needwork = ad
    Client& operator=(Client const& obj) = delete;

    /// @brief The move constructor shall not be used.
    /// @param obj The object to be moved.
    /// @trace_id_sr=SR_PHM_01001
    /// @needwork = ad
    Client(Client&& obj) = delete;

    /// @brief The move assignment shall not be used.
    /// @param obj The object to be moved.
    /// @return The moved object.
    /// @trace_id_sr=SR_PHM_01001
    /// @needwork = ad
    Client& operator=(Client const&& obj) = delete;

    /// @brief Report checkpoint to SupervisionManager.
    /// @param specifierId instance specifier of supervised entity.
    /// @param checkpointId checkpoint id.
    /// @return empty sucess, otherwise error.
    /// @trace_id_sr=SR_PHM_01001
    /// @needwork = ad
    int32_t ReportCheckpoint(Specifier const& specifierId, uint32_t const checkpointId) noexcept;

    /// @brief Returns status of LocalSupervision.
    /// @param specifierId instance specifier of supervised entity.
    /// @return The local supervision status.
    /// @trace_id_sr=SR_PHM_01001
    /// @needwork = ad
    SupervisionStatus GetLocalSupervisionStatus(Specifier const& specifierId) noexcept;

    /// @brief Returns status of GlobalSupervision.
    /// @param specifierId instance specifier of supervised entity.
    /// @return The global supervision status.
    /// @trace_id_sr=SR_PHM_01001
    /// @needwork = ad
    SupervisionStatus GetGlobalSupervisionStatus(Specifier const& specifierId) noexcept;

private:
    /// @brief Constructor.
    /// @trace_id_sr=SR_PHM_01001
    /// @needwork = dda
    Client() noexcept
        : BaseClient{GetIpcPhmSvCmClient()}
        , messageSerializer_{}
        , processClusterAffiliation_{GetProcessClusterAffiliation()} {};

    /// @brief get process cluster affiliation.
    /// @return process cluster affiliation.
    /// @trace_id_sr=SR_PHM_01001
    /// @needwork = dda
    static ProcessClusterAffiliation GetProcessClusterAffiliation() noexcept;

    /// @brief if process is SM.
    /// @param manifest execution manifest.
    /// @return true, process is SM; false not SM.
    /// @trace_id_sr=SR_PHM_01001
    /// @needwork = dda
    static bool IsStateManagement(ara::core::String const& manifest) noexcept;

    /// @brief if process is EM.
    /// @return true, process is EM; false not EM.
    /// @trace_id_sr=SR_PHM_01001
    /// @needwork = dda
    static bool IsExecutionManagement() noexcept;

private:
    /// @brief message serializer.
    /// @trace_id_sr=SR_PHM_01001
    /// @needwork = dda
    MessageSerializer messageSerializer_;

    /// @brief process cluster affiliation.
    /// @trace_id_sr=SR_PHM_01001
    /// @needwork = dda
    ProcessClusterAffiliation processClusterAffiliation_;
};  // class Client

}  // namespace svcom
}  // namespace internal
}  // namespace phm
}  // namespace ara

#endif  // ARA_PHM_INTERNAL_SVCOM_CLIENT_H_
