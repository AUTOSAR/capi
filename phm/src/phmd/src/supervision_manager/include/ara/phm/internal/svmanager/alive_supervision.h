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
/// @file       alive_supervision.h
/// @brief      AliveSupervision, Mechanism to check the timing constraints of
/// @details
/// @date       2024-06-05
/// @author     wangyanlong
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/PlatformHealthManagement/SupervisionManager
/// @unit_description=AliveSupervision, Mechanism to check the timing constraints of
/// cyclic SupervisedEntitys to be within the configured min and max limits.
/// @trace_id_sr=SR_PHM_01002,SR_PHM_01003
/// @unit_name=AliveSupervision
/// @interface_level=unit
/// @endcode
///
/// ================================================================
///
/// cyclic SupervisedEntitys to be within the configured min and max limits.
///
/// ================================================================

#ifndef ARA_PHM_INTERNAL_SVMANAGER_ALIVE_SUPERVISION_H_
#define ARA_PHM_INTERNAL_SVMANAGER_ALIVE_SUPERVISION_H_

#include <atomic>
#include <memory>

#include "ara/phm/internal/config_struct.h"
#include "ara/phm/internal/svmanager/base_supervision.h"
#include "ara/phm/internal/timer.h"

namespace ara {
namespace phm {
namespace internal {

/// @brief Manages AliveSupervision, process its checkpoint and determine its status.
/// @trace_id_sr=SR_PHM_01002,SR_PHM_01003
/// @needwork = ad
class AliveSupervision final : public BaseSupervision
{
public:
    /// @brief Constructor.
    /// @param conf conf of alive supervision.
    /// @param checkpoint checkpoint referenced by this alive supervision.
    /// @trace_id_sr=SR_PHM_01002,SR_PHM_01003
    /// @needwork = ad
    AliveSupervision(AliveSupervisionConf const& conf, std::shared_ptr< SupervisionCheckpoint > checkpoint) noexcept;

    /// @brief Default deconstructor.
    /// @trace_id_sr=SR_PHM_01002,SR_PHM_01003
    /// @needwork = ad
    ~AliveSupervision() noexcept final;

    /// @brief The copy constructor shall not be used.
    /// @param obj The object to be copied.
    /// @trace_id_sr=SR_PHM_01002,SR_PHM_01003
    /// @needwork = ad
    AliveSupervision(AliveSupervision& obj) = delete;

    /// @brief The copy assignment shall not be used.
    /// @param obj The object to be copied.
    /// @return The copied object.
    /// @trace_id_sr=SR_PHM_01002,SR_PHM_01003
    /// @needwork = ad
    AliveSupervision& operator=(AliveSupervision const& obj) = delete;

    /// @brief The move constructor shall not be used.
    /// @param obj The object to be moved.
    /// @trace_id_sr=SR_PHM_01002,SR_PHM_01003
    /// @needwork = ad
    AliveSupervision(AliveSupervision&& obj) = delete;

    /// @brief The move assignment shall not be used.
    /// @param obj The object to be moved.
    /// @return the moved object.
    /// @trace_id_sr=SR_PHM_01002,SR_PHM_01003
    /// @needwork = ad
    AliveSupervision& operator=(AliveSupervision const&& obj) = delete;

    /// @brief Process checkpoint reported from AA.
    /// @param checkpoint object of checkpoint.
    /// @param processId the process id who report this checkpoint.
    /// @param checkpointTimestamp timestamp when process send checkpoint.
    /// @trace_id_sr=SR_PHM_01002,SR_PHM_01003
    /// @needwork = ad
    void ProcessCheckpoint(std::shared_ptr< SupervisionCheckpoint > const& checkpoint,
                           ProcessId const processId,
                           int64_t const checkpointTimestamp) noexcept final;

    /// @brief Stop this supervision.
    /// @trace_id_sr=SR_PHM_01002,SR_PHM_01003
    /// @needwork = ad
    void Stop() noexcept final;

    /// @brief Start this supervision.
    /// @trace_id_sr=SR_PHM_01002,SR_PHM_01003
    /// @needwork = ad
    void Start() noexcept final;

    /// @brief Reset statistics of this supervision.
    /// @trace_id_sr=SR_PHM_01002,SR_PHM_01003
    /// @needwork = ad
    void ResetStatistics() noexcept final;

    /// @brief Returns the process name this supervision belongs to, when supervison need recover, should get the
    /// process name.
    /// @return the process name this supervision belongs to.
    /// @trace_id_sr=SR_PHM_01002,SR_PHM_01003
    /// @needwork = ad
    ara::core::String GetProcessName() noexcept final;

    /// @brief Returns process cluster affiliation.
    /// @return process cluster affiliation.
    /// @trace_id_sr=SR_PHM_01002,SR_PHM_01003
    /// @needwork = no
    ara::phm::internal::svcom::ProcessClusterAffiliation GetProcessClusterAffiliation() noexcept final;

    /// @brief Returns the checkpoints referenced by this supervision.
    /// @return the checkpoints referenced by this supervision.
    /// @trace_id_sr=SR_PHM_01002,SR_PHM_01003
    /// @needwork = ad
    ara::core::Vector< std::shared_ptr< SupervisionCheckpoint > > GetCheckpoint() noexcept final;

    /// @brief Returns the indications reached within a period.
    /// @return the indications reached within a period.
    /// @trace_id_sr=SR_PHM_01002,SR_PHM_01003
    /// @needwork = ad
    uint32_t GetIndications() const noexcept;

    /// @brief Returns checkcount, times the _supervisionTimerCb called.
    /// @return checkcount.
    /// @trace_id_sr=SR_PHM_01002,SR_PHM_01003
    /// @needwork = ad
    uint32_t GetCheckCount() const noexcept;

private:
    /// @brief Determine status of alive supervision in this function.
    /// @trace_id_sr=SR_PHM_01002,SR_PHM_01003
    /// @needwork = dda
    void _supervisionTimerCb();

private:
    /// @brief supervision period of alive supervision.
    /// @trace_id_sr=SR_PHM_01002,SR_PHM_01003
    /// @needwork = dda
    int32_t const kAliveReferenceCycle;

    /// @brief expected indications within a kAliveReferenceCycle.
    /// @trace_id_sr=SR_PHM_01002,SR_PHM_01003
    /// @needwork = dda
    uint32_t const kExpectedAliveIndications;

    /// @brief the amount of Alive Indications of the Supervision Checkpoint that are acceptable to be
    /// additional to the expectedAliveIndications within the aliveReferenceCycle.
    /// @trace_id_sr=SR_PHM_01002,SR_PHM_01003
    /// @needwork = dda
    uint32_t const kMinMargin;

    /// @brief max deviation allowed of kExpectedAliveIndications.
    /// @trace_id_sr=SR_PHM_01002,SR_PHM_01003
    /// @needwork = dda
    uint32_t const kMaxMargin;

    /// @brief the amount of Alive Indications of the Supervision Checkpoint that are acceptable to be missing
    /// to the expectedAliveIndications within the aliveReferenceCycle.
    /// @trace_id_sr=SR_PHM_01002,SR_PHM_01003
    /// @needwork = dda
    std::shared_ptr< SupervisionCheckpoint > checkpoint_;

    /// @brief the indications reached within a kAliveReferenceCycle.
    /// @trace_id_sr=SR_PHM_01002,SR_PHM_01003
    /// @needwork = dda
    std::atomic_uint32_t indications_;

    /// @brief check count, the times supervisionTimer_ run.
    /// @trace_id_sr=SR_PHM_01002,SR_PHM_01003
    /// @needwork = dda
    std::atomic_uint32_t checkCount_;

    /// @brief supervision timer, driving AliveSupervision to determine its status.
    /// @trace_id_sr=SR_PHM_01002,SR_PHM_01003
    /// @needwork = dda
    std::unique_ptr< ara::phm::internal::Timer > supervisionTimer_;
};

}  // namespace internal
}  // namespace phm
}  // namespace ara

#endif  // ARA_PHM_INTERNAL_SVMANAGER_ALIVE_SUPERVISION_H_
