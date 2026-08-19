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
/// @file       local_supervision.h
/// @brief      local supervision
/// @details
/// @date       2024-06-06
/// @author     wangyanlong
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/PlatformHealthManagement/SupervisionManager
/// @unit_description=local supervision
/// @trace_id_sr=SR_PHM_01009,SR_PHM_01010
/// @unit_name=LocalSupervision
/// @interface_level=unit
/// @endcode
///
/// ================================================================

#ifndef ARA_PHM_INTERNAL_SVMANAGER_LOCAL_SUPERVISION_H_
#define ARA_PHM_INTERNAL_SVMANAGER_LOCAL_SUPERVISION_H_

#include <ara/core/map.h>

#include <functional>

#include "ara/phm/internal/phm_log.h"
#include "ara/phm/internal/svmanager/base_supervision.h"
#include "ara/phm/supervised_entity.h"

namespace ara {
namespace phm {
namespace internal {

/// @brief Manages a LocalSupervision and determine its status.
/// @trace_id_sr=SR_PHM_01009,SR_PHM_01010
/// @needwork = ad
class LocalSupervision final
{
public:
    /// @brief Creation of a LocalSupervision.
    /// @param baseSupervision referenced base supervision.
    /// @param conf conf of local supervision.
    /// @trace_id_sr=SR_PHM_01009,SR_PHM_01010
    /// @needwork = ad
    LocalSupervision(ara::core::Vector< std::shared_ptr< BaseSupervision > > baseSupervision,
                     LocalSupervisionConf const& conf) noexcept;

    /// @brief Destructor.
    /// @trace_id_sr=SR_PHM_01009,SR_PHM_01010
    /// @needwork = ad
    ~LocalSupervision() noexcept;

    /// @brief The copy assignment shall not be used.
    /// @param obj The object to be copied.
    /// @return The copied object.
    /// @trace_id_sr=SR_PHM_01009,SR_PHM_01010
    /// @needwork = ad
    LocalSupervision& operator=(LocalSupervision const& obj) = delete;

    /// @brief The copy constructor shall not be used.
    /// @param obj The object to be copied.
    /// @trace_id_sr=SR_PHM_01009,SR_PHM_01010
    /// @needwork = ad
    LocalSupervision(LocalSupervision& obj) = delete;

    /// @brief The move constructor shall not be used.
    /// @param obj The object to be moved.
    /// @trace_id_sr=SR_PHM_01009,SR_PHM_01010
    /// @needwork = ad
    LocalSupervision(LocalSupervision&& obj) = delete;

    /// @brief The move assignment shall not be used.
    /// @param obj The object to be moved.
    /// @return the moved object.
    /// @trace_id_sr=SR_PHM_01009,SR_PHM_01010
    /// @needwork = ad
    LocalSupervision& operator=(LocalSupervision const&& obj) = delete;

    /// @brief Definition of a function,
    /// When local supervision status is updated,
    /// this function is called to notify the global supervision this local supervision belongs to.
    /// @trace_id_sr=SR_PHM_01009,SR_PHM_01010
    /// @needwork = no
    using StatusUpdateHandler
        = std::function< void(ara::core::String const& localSupervisionFqn,
                              LocalSupervisionStatus const& localSupervisionStatus,
                              BaseSupervisionType const& baseSupervisionType,
                              ara::core::String const& processName,
                              ara::phm::internal::svcom::ProcessClusterAffiliation const processClusterAffiliation) >;

    /// @brief Used by GlobalSupervision to register status listener. When local supervision status updated,
    /// this callback is called to notify the global supervision this local supervision belongs to.
    /// @param statusUpdateHandler callback local supervision used to notify the global supervision status updated.
    /// @trace_id_sr=SR_PHM_01009,SR_PHM_01010
    /// @needwork = ad
    void RegisterStatusUpdateHandler(StatusUpdateHandler const& statusUpdateHandler) noexcept;

    /// @brief Used by GlobalSupervision to remove status listener.
    /// @trace_id_sr=SR_PHM_01009,SR_PHM_01010
    /// @needwork = ad
    void UnRegisterStatusUpdateHandler() noexcept;

    /// @brief Switch supervision mode.
    /// @trace_id_sr=SR_PHM_01009,SR_PHM_01010
    /// @needwork = ad
    void SwitchSupervisionMode() noexcept;

    /// @brief Returns the name of this local supervision.
    /// @return the name of this local supervision.
    /// @trace_id_sr=SR_PHM_01009,SR_PHM_01010
    /// @needwork = ad
    ara::core::String const GetShortName() const noexcept { return kShortName; }

    /// @brief Returns the status of this local supervision.
    /// @return the status of this local supervision.
    /// @trace_id_sr=SR_PHM_01009,SR_PHM_01010
    /// @needwork = ad
    LocalSupervisionStatus GetStatus() const noexcept { return status_; };

    /// @brief Returns base supervisions referenced by this local supervision.
    /// @return base supervisions referenced by this local supervision.
    /// @trace_id_sr=SR_PHM_01009,SR_PHM_01010
    /// @needwork = ad
    ara::core::Vector< std::shared_ptr< BaseSupervision > > GetBaseSupervision() noexcept;

    /// @brief Returns the fqn of local supervision.
    /// @return the fqn of local supervision.
    /// @trace_id_sr=SR_PHM_01009,SR_PHM_01010
    /// @needwork = ad
    ara::core::String GetFqn() const noexcept { return kFqn; };

    /// @brief returns the string of local supervision status.
    /// @param status enum.
    /// @return the string of local supervision status.
    /// @trace_id_sr=SR_PHM_01009,SR_PHM_01010
    /// @needwork = ad
    static ara::core::StringView GetLocalSupervisionStatusString(LocalSupervisionStatus const& status) noexcept;

private:
    /// @brief This function is registered to base supervision. When base supervision status is updated,
    /// this callback is called, so local supervision can hear this.
    /// @throws QAC
    /// @param type type of base supervision.
    /// @param status status of base supervision.
    /// @param processName process name.
    /// @param processClusterAffiliation process cluster affiliation
    /// @trace_id_sr=SR_PHM_01009,SR_PHM_01010
    /// @needwork = dda
    void _baseSupervisionStatusUpdated(
        BaseSupervisionType const type,
        BaseSupervisionStatus const baseStatus,
        ara::core::String const& processName,
        ara::phm::internal::svcom::ProcessClusterAffiliation const processClusterAffiliation);

    /// @brief set status;
    /// @trace_id_sr=SR_PHM_01009,SR_PHM_01010
    /// @needwork = dda
    void _setStatus(LocalSupervisionStatus const& status) noexcept;

private:
    /// @brief name of local supervision.
    /// @trace_id_sr=SR_PHM_01009,SR_PHM_01010
    /// @needwork = dda
    ara::core::String const kShortName;

    /// @brief fqn of local supervision.
    /// @trace_id_sr=SR_PHM_01009,SR_PHM_01010
    /// @needwork = dda
    ara::core::String const kFqn;

    /// @brief failed supervision cycles allowed.
    /// @trace_id_sr=SR_PHM_01009,SR_PHM_01010
    /// @needwork = dda
    uint32_t const kFailedSupervisionCyclesTolerance;

    /// @brief failed cycles.
    /// @trace_id_sr=SR_PHM_01009,SR_PHM_01010
    /// @needwork = dda
    uint32_t failedSupervisionReferenceCycles_;

    /// @brief status of local supervision.
    /// @trace_id_sr=SR_PHM_01009,SR_PHM_01010
    /// @needwork = dda
    LocalSupervisionStatus status_;

    /// @brief base supervisions referenced by this local supervision, <fqn，BaseSupervision>.
    /// @trace_id_sr=SR_PHM_01009,SR_PHM_01010
    /// @needwork = dda
    ara::core::Map< ara::core::String, std::shared_ptr< BaseSupervision > > baseSupervision_;

    /// @brief When local supervision status is updated,
    /// this callback is called to notify the global supervision this local supervision belongs to.
    /// @trace_id_sr=SR_PHM_01009,SR_PHM_01010
    /// @needwork = dda
    StatusUpdateHandler statusUpdateHandler_;
};

}  // namespace internal
}  // namespace phm
}  // namespace ara

#endif  // ARA_PHM_INTERNAL_SVMANAGER_LOCAL_SUPERVISION_H_