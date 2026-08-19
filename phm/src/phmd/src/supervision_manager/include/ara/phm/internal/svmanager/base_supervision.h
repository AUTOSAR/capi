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
/// @file       base_supervision.h
/// @brief      The base class of AliveSupervision、DeadlineSupervision、LogicalSupervision.
/// @details
/// @date       2024-06-06
/// @author     wangyanlong
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/PlatformHealthManagement/SupervisionManager
/// @unit_description=The base class of AliveSupervision、DeadlineSupervision、LogicalSupervision.
/// @trace_id_sr=SR_PHM_01002,SR_PHM_01003,SR_PHM_01004,SR_PHM_01005,SR_PHM_01006,SR_PHM_01007
/// @unit_name=BaseSupervision
/// @interface_level=unit
/// @endcode
///
/// ================================================================

#ifndef ARA_PHM_INTERNAL_SVMANAGER_BASE_SUPERVISION_H_
#define ARA_PHM_INTERNAL_SVMANAGER_BASE_SUPERVISION_H_

#include <ara/core/string.h>
#include <ara/core/vector.h>

#include <memory>

#include "ara/phm/internal/svmanager/supervision_checkpoint.h"
#include "ara/phm/internal/types.h"

namespace ara {
namespace phm {
namespace internal {

/// @brief The status of base supervision.
/// @trace_id_sr=SR_PHM_01002,SR_PHM_01003,SR_PHM_01004,SR_PHM_01005,SR_PHM_01006,SR_PHM_01007
/// @needwork = ad
enum class BaseSupervisionStatus : SupervisionStatus
{
    /// @brief correct
    kCorrect = 0,

    /// @brief incorrect
    kInCorrect = 1
};

/// @brief The base class of AliveSupervision、DeadlineSupervision、LogicalSupervision.
/// @trace_id_sr=SR_PHM_01002,SR_PHM_01003,SR_PHM_01004,SR_PHM_01005,SR_PHM_01006,SR_PHM_01007
/// @needwork = ad
class BaseSupervision
{
public:
    /// @brief Default deconstructor.
    /// @throws QAC
    /// @trace_id_sr=SR_PHM_01002,SR_PHM_01003,SR_PHM_01004,SR_PHM_01005,SR_PHM_01006,SR_PHM_01007
    /// @needwork = dda
    virtual ~BaseSupervision() = default;

    /// @brief Process checkpoint reported from AA.
    /// @param checkpoint object of checkpoint.
    /// @param processId the process id who report this checkpoint.
    /// @param checkpointTimestamp timestamp when process send checkpoint.
    /// @trace_id_sr=SR_PHM_01002,SR_PHM_01003,SR_PHM_01004,SR_PHM_01005,SR_PHM_01006,SR_PHM_01007
    /// @needwork = dda
    virtual void ProcessCheckpoint(std::shared_ptr< SupervisionCheckpoint > const& checkpoint,
                                   ProcessId const processId,
                                   int64_t const checkpointTimestamp) noexcept = 0;

    /// @brief Definition of a function, When base supervision status is updated,
    /// this function is called to notify the local supervision referencing this base supervision.
    /// @trace_id_sr=SR_PHM_01002,SR_PHM_01003,SR_PHM_01004,SR_PHM_01005,SR_PHM_01006,SR_PHM_01007
    /// @needwork = no
    using BaseStatusUpdateHandler
        = std::function< void(BaseSupervisionType const type,
                              BaseSupervisionStatus const status,
                              ara::core::String const& processName,
                              ara::phm::internal::svcom::ProcessClusterAffiliation const processClusterAffiliation) >;

    /// @brief Uesed by local supervision to register status listener. When base supervision status is updated,
    /// this callback is called to notify the local supervision.
    /// @param localSupervisionFqn the fqn of local supervision.
    /// @param handler callback.
    /// @trace_id_sr=SR_PHM_01002,SR_PHM_01003,SR_PHM_01004,SR_PHM_01005,SR_PHM_01006,SR_PHM_01007
    /// @needwork = dda
    void RegisterStatusUpdateHandler(ara::core::String const& localSupervisionFqn,
                                     BaseStatusUpdateHandler const& handler) noexcept;

    /// @brief Uesed by local supervision to un register status listener.
    /// @param localSupervisionFqn the fqn of local supervision.
    /// @trace_id_sr=SR_PHM_01002,SR_PHM_01003,SR_PHM_01004,SR_PHM_01005,SR_PHM_01006,SR_PHM_01007
    /// @needwork = dda
    void UnRegisterStatusUpdateHandler(ara::core::String const& localSupervisionFqn) noexcept;

    /// @brief Stop this supervision.
    /// @trace_id_sr=SR_PHM_01002,SR_PHM_01003,SR_PHM_01004,SR_PHM_01005,SR_PHM_01006,SR_PHM_01007
    /// @needwork = dda
    virtual void Stop() noexcept = 0;

    /// @brief Start this supervision.
    /// @trace_id_sr=SR_PHM_01002,SR_PHM_01003,SR_PHM_01004,SR_PHM_01005,SR_PHM_01006,SR_PHM_01007
    /// @needwork = dda
    virtual void Start() noexcept = 0;

    /// @brief Reset statistics of this supervision.
    /// @trace_id_sr=SR_PHM_01002,SR_PHM_01003,SR_PHM_01004,SR_PHM_01005,SR_PHM_01006,SR_PHM_01007
    /// @needwork = dda
    virtual void ResetStatistics() noexcept = 0;

    /// @brief Returns the process name this supervision belongs to, when supervison need recover, should get the
    /// process name.
    /// @return the process name this supervision belongs to.
    /// @trace_id_sr=SR_PHM_01002,SR_PHM_01003,SR_PHM_01004,SR_PHM_01005,SR_PHM_01006,SR_PHM_01007
    /// @needwork = dda
    virtual ara::core::String GetProcessName() noexcept = 0;

    /// @brief Returns process cluster affiliation.
    /// @return process cluster affiliation.
    /// @trace_id_sr=SR_PHM_01002,SR_PHM_01003,SR_PHM_01004,SR_PHM_01005,SR_PHM_01006,SR_PHM_01007
    /// @needwork = no
    virtual ara::phm::internal::svcom::ProcessClusterAffiliation GetProcessClusterAffiliation() noexcept = 0;

    /// @brief Returns the checkpoints referenced by this supervision.
    /// @return the checkpoints referenced by this supervision.
    /// @trace_id_sr=SR_PHM_01002,SR_PHM_01003,SR_PHM_01004,SR_PHM_01005,SR_PHM_01006,SR_PHM_01007
    /// @needwork = dda
    virtual ara::core::Vector< std::shared_ptr< SupervisionCheckpoint > > GetCheckpoint() noexcept = 0;

    /// @brief Returns the type of this supervision.
    /// @return the type of this supervision.
    /// @trace_id_sr=SR_PHM_01002,SR_PHM_01003,SR_PHM_01004,SR_PHM_01005,SR_PHM_01006,SR_PHM_01007
    /// @needwork = dda
    BaseSupervisionType GetType() const noexcept;

    /// @brief Set supevision status, kCorrect or kInCorrect.
    /// @param status status of supervision.
    /// @trace_id_sr=SR_PHM_01002,SR_PHM_01003,SR_PHM_01004,SR_PHM_01005,SR_PHM_01006,SR_PHM_01007
    /// @needwork = dda
    void SetStatus(BaseSupervisionStatus const& status) noexcept;

    /// @brief Returns the status of this supervision.
    /// @return the status of this supervision.
    /// @trace_id_sr=SR_PHM_01002,SR_PHM_01003,SR_PHM_01004,SR_PHM_01005,SR_PHM_01006,SR_PHM_01007
    /// @needwork = dda
    BaseSupervisionStatus GetStatus() const noexcept;

    /// @brief Set active of this supervision.
    /// @param active true, active; false, inactive.
    /// @trace_id_sr=SR_PHM_01002,SR_PHM_01003,SR_PHM_01004,SR_PHM_01005,SR_PHM_01006,SR_PHM_01007
    /// @needwork = dda
    void SetActive(const bool active) noexcept;

    /// @brief Return whether this supervision is active.
    /// @return true, active;false, inactive.
    /// @trace_id_sr=SR_PHM_01002,SR_PHM_01003,SR_PHM_01004,SR_PHM_01005,SR_PHM_01006,SR_PHM_01007
    /// @needwork = dda
    bool GetActive() const noexcept;

    /// @brief Returns the fqn of base supervision.
    /// @return the fqn of base supervision.
    /// @trace_id_sr=SR_PHM_01002,SR_PHM_01003,SR_PHM_01004,SR_PHM_01005,SR_PHM_01006,SR_PHM_01007
    /// @needwork = dda
    ara::core::String GetFqn() const noexcept;

    /// @brief returns shortName
    /// @return shortName
    /// @trace_id_sr=SR_PHM_01002,SR_PHM_01003,SR_PHM_01004,SR_PHM_01005,SR_PHM_01006,SR_PHM_01007
    /// @needwork = dda
    ara::core::String GetShortName() const noexcept;

    /// @brief returns the string of base supervision status.
    /// @param status enum.
    /// @return the string of base supervision status.
    /// @trace_id_sr=SR_PHM_01002,SR_PHM_01003,SR_PHM_01004,SR_PHM_01005,SR_PHM_01006,SR_PHM_01007
    /// @needwork = dda
    static ara::core::StringView GetBaseSupervisionStatusString(BaseSupervisionStatus const& status) noexcept;

    /// @brief The copy constructor shall not be used.
    /// @param obj The object to be copied.
    /// @trace_id_sr=SR_PHM_01002,SR_PHM_01003,SR_PHM_01004,SR_PHM_01005,SR_PHM_01006,SR_PHM_01007
    /// @needwork = dda
    BaseSupervision(BaseSupervision& obj) = delete;

    /// @brief The copy assignment shall not be used.
    /// @param obj The object to be copied.
    /// @return The copied object.
    /// @trace_id_sr=SR_PHM_01002,SR_PHM_01003,SR_PHM_01004,SR_PHM_01005,SR_PHM_01006,SR_PHM_01007
    /// @needwork = dda
    BaseSupervision& operator=(BaseSupervision const& obj) = delete;

    /// @brief The move constructor shall not be used.
    /// @param obj The object to be moved.
    /// @trace_id_sr=SR_PHM_01002,SR_PHM_01003,SR_PHM_01004,SR_PHM_01005,SR_PHM_01006,SR_PHM_01007
    /// @needwork = dda
    BaseSupervision(BaseSupervision&& obj) = delete;

    /// @brief The move assignment shall not be used.
    /// @param obj The object to be moved.
    /// @return LogicalSupervision& The moved object.
    /// @trace_id_sr=SR_PHM_01002,SR_PHM_01003,SR_PHM_01004,SR_PHM_01005,SR_PHM_01006,SR_PHM_01007
    /// @needwork = dda
    BaseSupervision& operator=(BaseSupervision const&& obj) = delete;

protected:
    /// @brief Creation of BaseSupervision.
    /// @param shortName name of base supervision.
    /// @param fqn fqn of base supervision.
    /// @param supervisionType type of base supervision.
    /// @trace_id_sr=SR_PHM_01002,SR_PHM_01003,SR_PHM_01004,SR_PHM_01005,SR_PHM_01006,SR_PHM_01007
    /// @needwork = dda
    BaseSupervision(ara::core::String shortName,
                    ara::core::String fqn,
                    BaseSupervisionType const supervisionType) noexcept
        : kShortName{std::move(shortName)}
        , kFqn{std::move(fqn)}
        , kType{supervisionType}
        , active_{false}
        , status_{BaseSupervisionStatus::kCorrect}
        , statusUpdateHandler_{}

    {
    }

    /// @brief Notify status of base supervision to local supervision.
    /// @trace_id_sr=SR_PHM_01002,SR_PHM_01003,SR_PHM_01004,SR_PHM_01005,SR_PHM_01006,SR_PHM_01007
    /// @needwork = dda
    void _NotifyStatusToLocalSupervision() noexcept;

private:
    /// @brief name of base supervision.
    /// @trace_id_sr=SR_PHM_01002,SR_PHM_01003,SR_PHM_01004,SR_PHM_01005,SR_PHM_01006,SR_PHM_01007
    /// @needwork = dda
    ara::core::String const kShortName;

    /// @brief fqn of base supervision.
    /// @trace_id_sr=SR_PHM_01002,SR_PHM_01003,SR_PHM_01004,SR_PHM_01005,SR_PHM_01006,SR_PHM_01007
    /// @needwork = dda
    ara::core::String const kFqn;

    /// @brief type of base supervision.
    /// @trace_id_sr=SR_PHM_01002,SR_PHM_01003,SR_PHM_01004,SR_PHM_01005,SR_PHM_01006,SR_PHM_01007
    /// @needwork = dda
    BaseSupervisionType const kType;

    /// @brief whether this base supervision is active.
    /// @trace_id_sr=SR_PHM_01002,SR_PHM_01003,SR_PHM_01004,SR_PHM_01005,SR_PHM_01006,SR_PHM_01007
    /// @needwork = dda
    bool active_;

    /// @brief status of base supervision.
    /// @trace_id_sr=SR_PHM_01002,SR_PHM_01003,SR_PHM_01004,SR_PHM_01005,SR_PHM_01006,SR_PHM_01007
    /// @needwork = dda
    BaseSupervisionStatus status_;

    /// @brief When base supervision status is updated,
    /// statusUpdateHandler_ is called to notify the local supervision referencing this base supervision.
    /// @trace_id_sr=SR_PHM_01002,SR_PHM_01003,SR_PHM_01004,SR_PHM_01005,SR_PHM_01006,SR_PHM_01007
    /// @needwork = dda
    ara::core::Map< ara::core::String, BaseStatusUpdateHandler > statusUpdateHandler_;
};

}  // namespace internal
}  // namespace phm
}  // namespace ara

#endif  // ARA_PHM_INTERNAL_SVMANAGER_BASE_SUPERVISION_H_
