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
/// @file       logical_supervision.h
/// @brief      Kind of online supervision of software that checks if the software (
/// @details
/// @date       2024-06-05
/// @author     wangyanlong
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/PlatformHealthManagement/SupervisionManager
/// @unit_description=Kind of online supervision of software that checks if the software (
/// Supervised Entity or set of Supervised Entities) is executed
/// in the sequence defined by the programmer (by the developedcode).
/// @trace_id_sr=SR_PHM_01006,SR_PHM_01007
/// @unit_name=LogicalSupervision
/// @interface_level=unit
/// @endcode
///
/// ================================================================
///
/// Supervised Entity or set of Supervised Entities) is executed
/// in the sequence defined by the programmer (by the developedcode).
///
/// ================================================================

#ifndef ARA_PHM_INTERNAL_SVMANAGER_LOGICAL_SUPERVISION_H_
#define ARA_PHM_INTERNAL_SVMANAGER_LOGICAL_SUPERVISION_H_

#include <ara/core/map.h>
#include <ara/core/vector.h>

#include <atomic>
#include <memory>
#include <mutex>
#include <utility>

#include "ara/phm/internal/svmanager/base_supervision.h"
#include "ara/phm/internal/svmanager/supervision_checkpoint.h"

namespace ara {
namespace phm {
namespace internal {

/// @brief The location checkpoint in the graph.
/// @trace_id_sr=SR_PHM_01006,SR_PHM_01007
/// @needwork = ad
enum class CheckpointLocation : uint32_t
{
    /// @brief initial checkpoint.
    kInitial = 0,

    /// @brief final checkpoint.
    kFinal = 1,

    /// @brief transition checkpoint.
    kTransition = 2,

    /// @brief invalid checkpoint.
    kInvalid = 3
};

/// @brief Manages LogicalSupervision, process its checkpoint and determine its status.
/// @trace_id_sr=SR_PHM_01006,SR_PHM_01007
/// @needwork = ad
class LogicalSupervision final : public BaseSupervision
{
public:
    /// @brief Creation of a LogicalSupervision
    /// @param conf conf of logical supervision.
    /// @param initialCheckpoint initial checkpoint referenced by logical supervision.
    /// @param finalCheckpoint final checkpoint referenced by logical supervision.
    /// @param transition checkpoint transition referenced by logical supervision.
    /// @trace_id_sr=SR_PHM_01006,SR_PHM_01007
    /// @needwork = ad
    LogicalSupervision(
        LogicalSupervisionConf const& conf,
        ara::core::Vector< std::shared_ptr< SupervisionCheckpoint > > const& initialCheckpoint,
        ara::core::Vector< std::shared_ptr< SupervisionCheckpoint > > const& finalCheckpoint,
        ara::core::Vector< std::pair< std::shared_ptr< SupervisionCheckpoint >,
                                      std::shared_ptr< SupervisionCheckpoint > > > const& transition) noexcept;

    /// @brief Default deconstructor.
    /// @trace_id_sr=SR_PHM_01006,SR_PHM_01007
    /// @needwork = ad
    ~LogicalSupervision() noexcept final = default;

    /// @brief The copy constructor shall not be used.
    /// @param obj The object to be copied.
    /// @trace_id_sr=SR_PHM_01006,SR_PHM_01007
    /// @needwork = ad
    LogicalSupervision(LogicalSupervision& obj) = delete;

    /// @brief The copy assignment shall not be used.
    /// @param obj The object to be copied.
    /// @return The copied object.
    /// @trace_id_sr=SR_PHM_01006,SR_PHM_01007
    /// @needwork = ad
    LogicalSupervision& operator=(LogicalSupervision const& obj) = delete;

    /// @brief The move constructor shall not be used.
    /// @param obj The object to be moved.
    /// @trace_id_sr=SR_PHM_01006,SR_PHM_01007
    /// @needwork = ad
    LogicalSupervision(LogicalSupervision&& obj) = delete;

    /// @brief The move assignment shall not be used.
    /// @param obj The object to be moved.
    /// @return LogicalSupervision& The moved object.
    /// @trace_id_sr=SR_PHM_01006,SR_PHM_01007
    /// @needwork = ad
    LogicalSupervision& operator=(LogicalSupervision const&& obj) = delete;

    /// @brief Process checkpoint reported from AA.
    /// @param checkpoint object of checkpoint.
    /// @param processId the process id who report this checkpoint.
    /// @param checkpointTimestamp timestamp when process send checkpoint.
    /// @trace_id_sr=SR_PHM_01006,SR_PHM_01007
    /// @needwork = ad
    void ProcessCheckpoint(std::shared_ptr< SupervisionCheckpoint > const& checkpoint,
                           ProcessId const processId,
                           int64_t const checkpointTimestamp) noexcept final;

    /// @brief Stop this supervision.
    /// @trace_id_sr=SR_PHM_01006,SR_PHM_01007
    /// @needwork = ad
    void Stop() noexcept final;

    /// @brief Start this supervision.
    /// @trace_id_sr=SR_PHM_01006,SR_PHM_01007
    /// @needwork = ad
    void Start() noexcept final;

    /// @brief Reset statistics of this supervision.
    /// @trace_id_sr=SR_PHM_01006,SR_PHM_01007
    /// @needwork = ad
    void ResetStatistics() noexcept final;

    /// @brief Returns the process name this supervision belongs to,when supervison need recover, should get the process
    /// name.
    /// @return the process name this supervision belongs to.
    /// @trace_id_sr=SR_PHM_01006,SR_PHM_01007
    /// @needwork = ad
    ara::core::String GetProcessName() noexcept final;

    /// @brief Returns process cluster affiliation.
    /// @return process cluster affiliation.
    /// @trace_id_sr=SR_PHM_01006,SR_PHM_01007
    /// @needwork = no
    ara::phm::internal::svcom::ProcessClusterAffiliation GetProcessClusterAffiliation() noexcept final;

    /// @brief Returns the checkpoints referenced by this supervision.
    /// @return the checkpoints referenced by this supervision.
    /// @trace_id_sr=SR_PHM_01006,SR_PHM_01007
    /// @needwork = ad
    ara::core::Vector< std::shared_ptr< SupervisionCheckpoint > > GetCheckpoint() noexcept final;

    /// @brief Returns whether checkpoint graph is active.
    /// @return true, active;false, not active
    /// @trace_id_sr=SR_PHM_01006,SR_PHM_01007
    /// @needwork = ad
    bool GetGraphActive() const noexcept;

private:
    /// @brief Returns the location os checkpoint in the graph.
    /// @param checkpoint checkpoint
    /// @return the location os checkpoint in the graph.
    /// @trace_id_sr=SR_PHM_01006,SR_PHM_01007
    /// @needwork = dda
    CheckpointLocation _getCheckpointLocation(
        std::shared_ptr< SupervisionCheckpoint > const& checkpoint) const noexcept;

    /// @brief Returns whether the path to this checkpoint is continous.
    /// @param checkpoint object of checkpoint.
    /// @return true the path to this checkpoint is continous;false, the path to this checkpoint is not continous
    /// @trace_id_sr=SR_PHM_01006,SR_PHM_01007
    /// @needwork = dda
    bool _graphIsContinuous(std::shared_ptr< SupervisionCheckpoint > const& checkpoint) noexcept;

    /// @brief Set all the checkpoint reached flag to false.
    /// @trace_id_sr=SR_PHM_01006,SR_PHM_01007
    /// @needwork = dda
    void _resetCheckpointReachedFlag() noexcept;

    /// @brief returns the string of checkpoint location.
    /// @param location checkpoint location.
    /// @return the string of checkpoint location.
    /// @trace_id_sr=SR_PHM_01006,SR_PHM_01007
    /// @needwork = dda
    static ara::core::StringView GetCheckpointLocationString(CheckpointLocation const& location) noexcept;

private:
    /// @brief initial checkpoint, <fqn, SuperbisionCheckpoint>>
    /// @trace_id_sr=SR_PHM_01006,SR_PHM_01007
    /// @needwork = dda
    ara::core::Map< ara::core::String, std::shared_ptr< SupervisionCheckpoint > > initialCheckpoint_;

    /// @brief final checkpoint, <fqn, SuperbisionCheckpoint>>
    /// @trace_id_sr=SR_PHM_01006,SR_PHM_01007
    /// @needwork = dda
    ara::core::Map< ara::core::String, std::shared_ptr< SupervisionCheckpoint > > finalCheckpoint_;

    /// @brief checkpoint transition, <fqn, upstream SupervisionCheckpoint>>
    /// @trace_id_sr=SR_PHM_01006,SR_PHM_01007
    /// @needwork = dda
    ara::core::Map< ara::core::String, ara::core::Vector< std::shared_ptr< SupervisionCheckpoint > > > transition_;

    /// @brief the flag of whether graph is active.
    /// @trace_id_sr=SR_PHM_01006,SR_PHM_01007
    /// @needwork = dda
    std::atomic_bool graphActive_;

    /// @brief map to store whether the checkpoint reached.
    /// @trace_id_sr=SR_PHM_01006,SR_PHM_01007
    /// @needwork = dda
    ara::core::Map< ara::core::String, bool > checkpointsReachedFlag_;

    /// @brief The last reached checkpoint, when need recover, get process name from this checkpoint.
    /// @trace_id_sr=SR_PHM_01006,SR_PHM_01007
    /// @needwork = dda
    std::shared_ptr< SupervisionCheckpoint > lastCheckpoint_;

    /// @brief all checkpoint referenced by logical supervision.
    /// @trace_id_sr=SR_PHM_01006,SR_PHM_01007
    /// @needwork = dda
    ara::core::Vector< std::shared_ptr< SupervisionCheckpoint > > allCheckpoint_;
};

}  // namespace internal
}  // namespace phm
}  // namespace ara

#endif  // ARA_PHM_INTERNAL_SVMANAGER_LOGICAL_SUPERVISION_H_
