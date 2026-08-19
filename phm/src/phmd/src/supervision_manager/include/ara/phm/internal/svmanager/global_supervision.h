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
/// @file       global_supervision.h
/// @brief      A GlobalSupervision is an overall state of a software subsystem.
/// @details
/// @date       2024-06-05
/// @author     wangyanlong
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/PlatformHealthManagement/SupervisionManager
/// @unit_description=A GlobalSupervision is an overall state of a software subsystem.
/// a global supervision can only belongs to one function group.
/// @trace_id_sr=SR_PHM_01012,SR_PHM_01013
/// @unit_name=GlobalSupervision
/// @interface_level=unit
/// @endcode
///
/// ================================================================
///
/// a global supervision can only belongs to one function group.
///
/// ================================================================

#ifndef ARA_PHM_INTERNAL_SVMANAGER_GLOBAL_SUPERVISION_H_
#define ARA_PHM_INTERNAL_SVMANAGER_GLOBAL_SUPERVISION_H_

#include <ara/core/map.h>
#include <ara/core/vector.h>

#include "ara/phm/internal/phm_log.h"
#include "ara/phm/internal/svmanager/local_supervision.h"
#include "ara/phm/internal/timer.h"

namespace ara {
namespace phm {
namespace internal {

/// @brief Manages a GlobalSupervision and determine its status.
/// @trace_id_sr=SR_PHM_01012,SR_PHM_01013
/// @needwork = ad
class GlobalSupervision final
{
public:
    /// @brief Creation of a GlobalSupervision.
    /// @param localSupervision local supervision referenced by global supervision.
    /// @param conf conf of global supervision.
    /// @trace_id_sr=SR_PHM_01012,SR_PHM_01013
    /// @needwork = ad
    GlobalSupervision(ara::core::Vector< std::shared_ptr< LocalSupervision > > localSupervision,
                      GlobalSupervisionConf const& conf) noexcept;

    /// @brief Destructor.
    /// @trace_id_sr=SR_PHM_01012,SR_PHM_01013
    /// @needwork = ad
    ~GlobalSupervision() noexcept;

    /// @brief The copy constructor shall not be used.
    /// @param obj The object to be copied.
    /// @trace_id_sr=SR_PHM_01012,SR_PHM_01013
    /// @needwork = ad
    GlobalSupervision(GlobalSupervision& obj) = delete;

    /// @brief The copy assignment shall not be used.
    /// @param obj The object to be copied.
    /// @return The copied object.
    /// @trace_id_sr=SR_PHM_01012,SR_PHM_01013
    /// @needwork = ad
    GlobalSupervision& operator=(GlobalSupervision const& obj) = delete;

    /// @brief The move constructor shall not be used.
    /// @param obj The object to be moved.
    /// @trace_id_sr=SR_PHM_01012,SR_PHM_01013
    /// @needwork = ad
    GlobalSupervision(GlobalSupervision&& obj) = delete;

    /// @brief The move assignment shall not be used.
    /// @param obj The object to be moved.
    /// @return The moved object.
    /// @trace_id_sr=SR_PHM_01012,SR_PHM_01013
    /// @needwork = ad
    GlobalSupervision& operator=(GlobalSupervision const&& obj) = delete;

    /// @brief Definition of a function, When global supervision status is changed,
    /// this function is called to notify the supervision controller.
    /// @trace_id_sr=SR_PHM_01012,SR_PHM_01013
    /// @needwork = no
    using StatusChangeHandler
        = std::function< void(ara::core::String const& globalSupervisionName,
                              GlobalSupervisionStatus const& globalSupervisionStatus,
                              BaseSupervisionType const& baseSupervisionType,
                              ara::core::String const& processName,
                              ara::phm::internal::svcom::ProcessClusterAffiliation const processClusterAffiliation) >;

    /// @brief Used by SupervisionController to register status listener.
    /// @param statusChangeHandler status listener of SupervisionController
    /// @note In SupervisionManager, find the corresponding supervision mode according to the process name
    /// @trace_id_sr=SR_PHM_01012,SR_PHM_01013
    /// @needwork = ad
    void RegisterStatusChangeHandler(StatusChangeHandler const& statusChangeHandler) noexcept;

    /// @brief Used by SupervisionController to remove status listener.
    /// @trace_id_sr=SR_PHM_01012,SR_PHM_01013
    /// @needwork = ad
    void UnRegisterStatusChangeHandler() noexcept;

    /// @brief Switch supervision mode.
    /// @trace_id_sr=SR_PHM_01012,SR_PHM_01013
    /// @needwork = ad
    void SwitchSupervisionMode() noexcept;

    /// @brief Stop supervision.
    /// @trace_id_sr=SR_PHM_01012,SR_PHM_01013
    /// @needwork = ad
    void Stop() const noexcept;

    /// @brief Returns the name of global supervision.
    /// @return the name of this supervision.
    /// @trace_id_sr=SR_PHM_01012,SR_PHM_01013
    /// @needwork = ad
    ara::core::String GetShortName() const noexcept { return kShortName; }

    /// @brief Returns the fqn of global supervision.
    /// @return the fqn of global supervision.
    /// @trace_id_sr=SR_PHM_01012,SR_PHM_01013
    /// @needwork = ad
    ara::core::String GetFqn() const noexcept { return kShortName; }

    /// @brief Returns the status of this global supervision.
    /// @return the status of this global supervision.
    /// @trace_id_sr=SR_PHM_01012,SR_PHM_01013
    /// @needwork = ad
    GlobalSupervisionStatus GetStatus() const noexcept { return status_; }

    /// @brief Returns the recovery notification timeout.
    /// @return the recovery notification timeout.
    /// @trace_id_sr=SR_PHM_01012,SR_PHM_01013
    /// @needwork = ad
    int32_t GetRecoveryNotificationTimeout() const noexcept;

    /// @brief Returns the recovery notification retry.
    /// @return the recovery notification retry.
    /// @trace_id_sr=SR_PHM_01012,SR_PHM_01013
    /// @needwork = ad
    uint32_t GetRecoveryNotificationRetry() const noexcept;

    /// @brief Returns the flag of whether this supervision is offereed.
    /// @return the flag of whether this supervision is offereed.
    /// @trace_id_sr=SR_PHM_01012,SR_PHM_01013
    /// @needwork = ad
    bool IsOffered() const noexcept { return offered_; };

    /// @brief Set the flag of whether this supervision is offereed.
    /// @param offer true, offered; false not offered.
    /// @trace_id_sr=SR_PHM_01012,SR_PHM_01013
    /// @needwork = ad
    void SetOffer(bool const offer) noexcept
    {
        LOG_INFO << kShortName.c_str() << " offer status " << offered_ << "-->" << offer;
        offered_ = offer;
    };

    /// @brief Returns the local supervision referenced by global supervision.
    /// @return the local supervision referenced by global supervision.
    /// @trace_id_sr=SR_PHM_01012,SR_PHM_01013
    /// @needwork = ad
    ara::core::Vector< std::shared_ptr< LocalSupervision > > GetLocalSupervision() noexcept;

    /// @brief The model id of this global supervision.
    /// @return model id of this global supervision.
    /// @trace_id_sr=SR_PHM_01012,SR_PHM_01013
    /// @needwork = ad
    ara::core::String GetMetaModelIdentifier() const noexcept { return kMetaModelIdentifier; };

    /// @brief returns the string of status.
    /// @param status
    /// @return he string of status.
    /// @trace_id_sr=SR_PHM_01012,SR_PHM_01013
    /// @needwork = ad
    static ara::core::StringView GetGlobalSupervisionStatusString(GlobalSupervisionStatus const& status) noexcept;

private:
    /// @brief This function is registered to local supervision. When local supervision status is updated,
    /// this function is called.
    /// @throws QAC
    /// @param loalSupervisionFqn  fqn of local supervision.
    /// @param localSupervisionStatus status of local supervision.
    /// @param baseSupervisionType type of base supervision.
    /// @param processName process name.
    /// @param processClusterAffiliation process cluster affiliation.
    /// @trace_id_sr=SR_PHM_01012,SR_PHM_01013
    /// @needwork = dda
    void _localSupervisionStatusChanged(
        ara::core::String const& loalSupervisionFqn,
        LocalSupervisionStatus const& localSupervisionStatus,
        BaseSupervisionType const& baseSupervisionType,
        ara::core::String const& processName,
        ara::phm::internal::svcom::ProcessClusterAffiliation const processClusterAffiliation);

    /// @brief Called periodically to determine the status of global supervision.
    /// @throws QAC
    /// @trace_id_sr=SR_PHM_01012,SR_PHM_01013
    /// @needwork = dda
    void _supervisionTimerCb();

    /// @brief determine the status of global supervision
    /// @trace_id_sr=SR_PHM_01012,SR_PHM_01013
    /// @needwork = dda
    void _determineStatus() noexcept;

    /// @brief set status.
    /// @param status status.
    /// @trace_id_sr=SR_PHM_01012,SR_PHM_01013
    /// @needwork = dda
    void _setStatus(GlobalSupervisionStatus const& status) noexcept;

private:
    /// @brief name of global supervision.
    /// @trace_id_sr=SR_PHM_01012,SR_PHM_01013
    /// @needwork = dda
    ara::core::String const kShortName;

    /// @brief model id of global supervision.
    /// @trace_id_sr=SR_PHM_01012,SR_PHM_01013
    /// @needwork = dda
    ara::core::String const kMetaModelIdentifier;

    /// @brief expired supervision cycles allowed.
    /// @trace_id_sr=SR_PHM_01012,SR_PHM_01013
    /// @needwork = dda
    uint32_t const kExpiredSupervisionCycleTolerance;

    /// @brief expired supervision cycles.
    /// @trace_id_sr=SR_PHM_01012,SR_PHM_01013
    /// @needwork = dda
    uint32_t expiredSupervisionCycle_;

    /// @brief timeout retry of recovery.
    /// @trace_id_sr=SR_PHM_01012,SR_PHM_01013
    /// @needwork = dda
    int32_t const kRecoveryNotificationTimeout;

    /// @brief max retry of recovery.
    /// @trace_id_sr=SR_PHM_01012,SR_PHM_01013
    /// @needwork = dda
    uint32_t const kRecoveryNotificationRetry;

    /// @brief supervision period of global supervision.
    /// @trace_id_sr=SR_PHM_01012,SR_PHM_01013
    /// @needwork = dda
    int32_t const kSupervisionCycle;

    /// @brief status of global supervision.
    /// @trace_id_sr=SR_PHM_01012,SR_PHM_01013
    /// @needwork = dda
    GlobalSupervisionStatus status_;

    /// @brief local supervision referenced by this global supervision <fqn, local supervision>.
    /// @trace_id_sr=SR_PHM_01012,SR_PHM_01013
    /// @needwork = dda
    ara::core::Map< ara::core::String, std::shared_ptr< LocalSupervision > > localSupervision_;

    /// @brief Timer driving global supervision to determine its status.
    /// @trace_id_sr=SR_PHM_01012,SR_PHM_01013
    /// @needwork = dda
    std::unique_ptr< ara::phm::internal::Timer > supervisionTimer_;

    /// @brief status listener of SupervisionController.
    /// @trace_id_sr=SR_PHM_01012,SR_PHM_01013
    /// @needwork = dda
    StatusChangeHandler statusChangeHandler_;

    /// @brief the type of base supervision who caused global supervision status to expired.
    /// @trace_id_sr=SR_PHM_01012,SR_PHM_01013
    /// @needwork = dda
    BaseSupervisionType baseSupervisionTypeCausedToExpired_;

    /// @brief the process name who caused global supervision status to expired.

    /// @brief the name of process caused this global supervision to expired.
    /// @trace_id_sr=SR_PHM_01012,SR_PHM_01013
    /// @needwork = dda
    ara::core::String processNameCausedToExpired_;

    /// @brief process cluster affiliation.
    /// @trace_id_sr=SR_PHM_01012,SR_PHM_01013
    /// @needwork = dda
    ara::phm::internal::svcom::ProcessClusterAffiliation processClusterAffiliation_;

    /// @brief flag about whether this supervision is offered, true, offered; false not offered.
    /// @trace_id_sr=SR_PHM_01012,SR_PHM_01013
    /// @needwork = dda
    bool offered_;
};

}  // namespace internal
}  // namespace phm
}  // namespace ara

#endif  // ARA_PHM_INTERNAL_SVMANAGER_GLOBAL_SUPERVISION_H_