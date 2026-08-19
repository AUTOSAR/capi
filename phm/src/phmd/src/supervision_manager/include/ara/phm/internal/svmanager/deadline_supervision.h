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
/// @file       deadline_supervision.h
/// @brief      Mechanism to check that the timing constraints for execution of
/// @details
/// @date       2024-06-05
/// @author     wangyanlong
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/PlatformHealthManagement/SupervisionManager
/// @unit_description=Mechanism to check that the timing constraints for execution of
/// the transition from a Deadline Start Checkpoint to a corresponding Deadline End Checkpoint are within the configured
/// min and max limits.
/// @trace_id_sr=SR_PHM_01004,SR_PHM_01005
/// @unit_name=DeadlineSupervision
/// @interface_level=unit
/// @endcode
///
/// ================================================================
///
/// the transition from a Deadline Start Checkpoint to a corresponding Deadline End Checkpoint are within the configured
/// min and max limits.
///
/// ================================================================

#ifndef ARA_PHM_INTERNAL_SVMANAGER_DEADLINE_SUPERVISION_H_
#define ARA_PHM_INTERNAL_SVMANAGER_DEADLINE_SUPERVISION_H_

#include <memory>

#include "ara/phm/internal/config_struct.h"
#include "ara/phm/internal/svmanager/base_supervision.h"
#include "ara/phm/internal/timer.h"

namespace ara {
namespace phm {
namespace internal {

/// @brief Manages DeadlineSupervision, process its checkpoint and determine its status.
/// @trace_id_sr=SR_PHM_01004,SR_PHM_01005
/// @needwork = ad
class DeadlineSupervision final : public BaseSupervision
{
public:
    /// @brief Creation of a DeadlineSupervision.
    /// @param conf conf of deadline supervision.
    /// @param sourceCheckpoint source checkpoint referenced by deadline supervision.
    /// @param targetCheckpoint target checkpoint referenced by deadline supervision.
    /// @trace_id_sr=SR_PHM_01004,SR_PHM_01005
    /// @needwork = ad
    DeadlineSupervision(DeadlineSupervisionConf const& conf,
                        std::shared_ptr< SupervisionCheckpoint > sourceCheckpoint,
                        std::shared_ptr< SupervisionCheckpoint > targetCheckpoint) noexcept;

    /// @brief Default deconstructor.
    /// @trace_id_sr=SR_PHM_01004,SR_PHM_01005
    /// @needwork = ad
    ~DeadlineSupervision() noexcept final;

    /// @brief The copy constructor shall not be used.
    /// @param obj The object to be copied.
    /// @trace_id_sr=SR_PHM_01004,SR_PHM_01005
    /// @needwork = ad
    DeadlineSupervision(DeadlineSupervision& obj) = delete;

    /// @brief The copy assignment shall not be used.
    /// @param obj The object to be copied.
    /// @return The copied object.
    /// @trace_id_sr=SR_PHM_01004,SR_PHM_01005
    /// @needwork = ad
    DeadlineSupervision& operator=(DeadlineSupervision const& obj) = delete;

    /// @brief The move constructor shall not be used.
    /// @param obj The object to be moved.
    /// @trace_id_sr=SR_PHM_01004,SR_PHM_01005
    /// @needwork = ad
    DeadlineSupervision(DeadlineSupervision&& obj) = delete;

    /// @brief The move assignment shall not be used.
    /// @param obj The object to be moved.
    /// @return The moved object.
    /// @trace_id_sr=SR_PHM_01004,SR_PHM_01005
    /// @needwork = ad
    DeadlineSupervision& operator=(DeadlineSupervision const&& obj) = delete;

    /// @brief Process checkpoint reported from AA.
    /// @param checkpoint object of checkpoint.
    /// @param processId the process id who report this checkpoint.
    /// @param checkpointTimestamp timestamp when process send checkpoint.
    /// @trace_id_sr=SR_PHM_01004,SR_PHM_01005
    /// @needwork = ad
    void ProcessCheckpoint(std::shared_ptr< SupervisionCheckpoint > const& checkpoint,
                           ProcessId const processId,
                           int64_t const checkpointTimestamp) noexcept final;

    /// @brief Stop this supervision.
    /// @trace_id_sr=SR_PHM_01004,SR_PHM_01005
    /// @needwork = ad
    void Stop() noexcept final;

    /// @brief Start this supervision.
    /// @trace_id_sr=SR_PHM_01004,SR_PHM_01005
    /// @needwork = ad
    void Start() noexcept final;

    /// @brief Reset statistics of this supervision.
    /// @trace_id_sr=SR_PHM_01004,SR_PHM_01005
    /// @needwork = ad
    void ResetStatistics() noexcept final;

    /// @brief Returns the process name this supervision belongs to,when supervison need recover, should get the process
    /// name.
    /// @return the process name this supervision belongs to.
    /// @trace_id_sr=SR_PHM_01004,SR_PHM_01005
    /// @needwork = ad
    ara::core::String GetProcessName() noexcept final;

    /// @brief Returns process cluster affiliation.
    /// @return process cluster affiliation.
    /// @trace_id_sr=SR_PHM_01004,SR_PHM_01005
    /// @needwork = no
    ara::phm::internal::svcom::ProcessClusterAffiliation GetProcessClusterAffiliation() noexcept final;

    /// @brief Returns the checkpoints referenced by this supervision.
    /// @return the checkpoints referenced by this supervision.
    /// @trace_id_sr=SR_PHM_01004,SR_PHM_01005
    /// @needwork = ad
    ara::core::Vector< std::shared_ptr< SupervisionCheckpoint > > GetCheckpoint() noexcept final;

    /// @brief Returns whether the source checkpoint reached.
    /// @return true, source checkpoint reached;false, source checkpoint did not reach.
    /// @trace_id_sr=SR_PHM_01004,SR_PHM_01005
    /// @needwork = ad
    bool IsSourceCheckpointReached() const noexcept;

private:
    /// @brief The max deadline timeout callback.
    /// @throws QAC
    /// @trace_id_sr=SR_PHM_01004,SR_PHM_01005
    /// @needwork = dda
    void _supervisionTimerCb();

private:
    /// @brief max deadline allowed.
    /// @trace_id_sr=SR_PHM_01004,SR_PHM_01005
    /// @needwork = dda
    int32_t const kMaxDeadline;

    /// @brief min deadline allowed.
    /// @trace_id_sr=SR_PHM_01004,SR_PHM_01005
    /// @needwork = dda
    int32_t const kMinDeadline;

    /// @brief source checkpoint referenced by this supervision.
    /// @trace_id_sr=SR_PHM_01004,SR_PHM_01005
    /// @needwork = dda
    std::shared_ptr< SupervisionCheckpoint > sourceCheckpoint_;

    /// @brief target checkpoint referenced by this supervision.
    /// @trace_id_sr=SR_PHM_01004,SR_PHM_01005
    /// @needwork = dda
    std::shared_ptr< SupervisionCheckpoint > targetCheckpoint_;

    /// @brief whether the source checkpoint reached.
    /// @trace_id_sr=SR_PHM_01004,SR_PHM_01005
    /// @needwork = dda
    bool sourceCheckpointReached_;

    /// @brief timestamp of source checkpoint.
    /// @trace_id_sr=SR_PHM_01004,SR_PHM_01005
    /// @needwork = dda
    int64_t sourceCheckpointTimeStamp_;

    /// @brief supervision timer, driving DeadlineSupervision to determine its status.
    /// @trace_id_sr=SR_PHM_01004,SR_PHM_01005
    /// @needwork = dda
    std::unique_ptr< ara::phm::internal::Timer > supervisionTimer_;

    /// @brief The last reached checkpoint, when need recover, get process name from this checkpoint.
    /// @trace_id_sr=SR_PHM_01004,SR_PHM_01005
    /// @needwork = dda
    std::shared_ptr< SupervisionCheckpoint > lastCheckpoint_;
};

}  // namespace internal
}  // namespace phm
}  // namespace ara

#endif  // ARA_PHM_INTERNAL_SVMANAGER_DEADLINE_SUPERVISION_H_
