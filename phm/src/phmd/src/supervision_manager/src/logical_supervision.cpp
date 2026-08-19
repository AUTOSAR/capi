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
/// @file       logical_supervision.cpp
/// @brief
/// @details
/// @date       2024-06-20
/// @author     wangyanlong
/// @version    1.2.0
///
/// ================================================================

#include "ara/phm/internal/svmanager/logical_supervision.h"

#include <set>

#include "ara/phm/internal/phm_log.h"

namespace ara {
namespace phm {
namespace internal {

/// @brief Creation of a LogicalSupervision
/// @param conf conf of logical supervision.
/// @param initialCheckpoint initial checkpoint referenced by logical supervision.
/// @param finalCheckpoint final checkpoint referenced by logical supervision.
/// @param transition checkpoint transition referenced by logical supervision.
LogicalSupervision::LogicalSupervision(
    LogicalSupervisionConf const &conf,
    ara::core::Vector< std::shared_ptr< SupervisionCheckpoint > > const &initialCheckpoint,
    ara::core::Vector< std::shared_ptr< SupervisionCheckpoint > > const &finalCheckpoint,
    ara::core::Vector< std::pair< std::shared_ptr< SupervisionCheckpoint >,
                                  std::shared_ptr< SupervisionCheckpoint > > > const &transition) noexcept
    : BaseSupervision{conf.shortName, conf.fqn, BaseSupervisionType::kLogical}
    , initialCheckpoint_{}
    , finalCheckpoint_{}
    , transition_{}
    , graphActive_{false}
    , checkpointsReachedFlag_{}
    , lastCheckpoint_{}
    , allCheckpoint_{}
{
    LOG_INFO << "create LogicalSupervision " << GetShortName().c_str() << ", initial checkpoint count "
             << initialCheckpoint.size() << ", final checkpoint count " << finalCheckpoint.size();

    graphActive_    = false;
    lastCheckpoint_ = nullptr;
    std::set< ara::core::String > checkpointSet;
    for (std::shared_ptr< SupervisionCheckpoint > const &checkpoint : initialCheckpoint) {
        // Ensure lastCheckpoint_ always points to a checkpoint
        if (lastCheckpoint_ == nullptr) {
            lastCheckpoint_ = checkpoint;
        }

        initialCheckpoint_[checkpoint->GetFqn()]      = checkpoint;
        checkpointsReachedFlag_[checkpoint->GetFqn()] = false;

        /// @details modify for QAC
        /// if (checkpointSet.find(checkpoint->GetFqn()) == checkpointSet.end()) {
        if (checkpointSet.count(checkpoint->GetFqn()) == 0U) {
            allCheckpoint_.push_back(checkpoint);
            std::ignore = checkpointSet.insert(checkpoint->GetFqn());
        }
    }

    for (std::shared_ptr< SupervisionCheckpoint > const &checkpoint : finalCheckpoint) {
        finalCheckpoint_[checkpoint->GetFqn()]        = checkpoint;
        checkpointsReachedFlag_[checkpoint->GetFqn()] = false;
        /// @details modify for QAC
        /// if (checkpointSet.find(checkpoint->GetFqn()) == checkpointSet.end()) {
        if (checkpointSet.count(checkpoint->GetFqn()) == 0U) {
            allCheckpoint_.push_back(checkpoint);
            std::ignore = checkpointSet.insert(checkpoint->GetFqn());
        }
    }

    for (std::pair< std::shared_ptr< SupervisionCheckpoint >, std::shared_ptr< SupervisionCheckpoint > > const &pair :
         transition) {
        transition_[pair.first->GetFqn()].push_back(pair.second);
        checkpointsReachedFlag_[pair.first->GetFqn()]  = false;
        checkpointsReachedFlag_[pair.second->GetFqn()] = false;

        if (checkpointSet.count(pair.first->GetFqn()) == 0U) {
            allCheckpoint_.push_back(pair.first);
            std::ignore = checkpointSet.insert(pair.first->GetFqn());
        }

        if (checkpointSet.count(pair.second->GetFqn()) == 0U) {
            allCheckpoint_.push_back(pair.second);
            std::ignore = checkpointSet.insert(pair.second->GetFqn());
        }
    }
}

/// @brief Process checkpoint reported from AA.
/// @param checkpoint object of checkpoint.
/// @param processId the process id who report this checkpoint.
/// @param checkpointTimestamp timestamp when process send checkpoint.
void LogicalSupervision::ProcessCheckpoint(std::shared_ptr< SupervisionCheckpoint > const &checkpoint,
                                           ProcessId const processId,
                                           int64_t const checkpointTimestamp) noexcept
{
    std::ignore = processId;
    std::ignore = checkpointTimestamp;

    lastCheckpoint_ = checkpoint;
    CheckpointLocation const checkpointLocation{_getCheckpointLocation(checkpoint)};

    if (graphActive_.load() == false) {
        ///If the Graph of the reported Checkpoint is inactive, then:
        ///a. If the Checkpoint is an Initial Checkpoint (see LogicalSupervision),
        ///then the result of this Logical Supervision within the SupervisedEntity
        ///of the reported Checkpoint is correct, otherwise incorrect
        if (checkpointLocation == CheckpointLocation::kInitial) {
            _resetCheckpointReachedFlag();
            ///dIf the function ReportCheckpoint determines that the result of
            ///the Logical Supervision for the given Checkpoint is true, and the Checkpoint
            ///is the initial one (see LogicalSupervision), then the Graph corresponding to the
            ///Checkpoint shall be considered as active
            graphActive_.store(true);
            SetStatus(BaseSupervisionStatus::kCorrect);
            LOG_DEBUG << "logical supervision " << GetShortName() << " Initial checkpoint "
                      << checkpoint->GetCheckpointId() << " from process " << checkpoint->GetProcessName()
                      << " reached. status is correct.";
        } else {
            SetStatus(BaseSupervisionStatus::kInCorrect);
            LOG_DEBUG << "logical supervision " << GetShortName() << " Initial checkpoint "
                      << checkpoint->GetCheckpointId() << " from process " << checkpoint->GetProcessName()
                      << " reached. status is incorrect because graph is not active and checpoint is not initial.";
        }
    } else {
        if (GetStatus() == BaseSupervisionStatus::kInCorrect) {
            ///the Graph is active, but at least one Checkpoint in this Graph was
            ///previously called in a wrong sequence):
            ///a. The result of this Logical Supervision of the Supervised Entity keeps incorrect.
            LOG_DEBUG << "logical supervision " << GetShortName() << " "
                      << GetCheckpointLocationString(checkpointLocation) << " checkpoint "
                      << checkpoint->GetCheckpointId() << " from process " << checkpoint->GetProcessName()
                      << " reached. status is already incorrect, so keep incorrect.";
        } else {
            ///Else if the Graph is active and all previously called Checkpoints of this Graph.
            ///were called in the right sequence, then:
            ///a. If the reported Checkpoint is a successor of the stored Checkpoint within
            ///the Graph of the reported Checkpoint (this means there is a Transition with
            ///Source and Target), then the result of this Logical Supervision for SupervisedEntity of the reported Checkpoint is correct, otherwise incorrect.
            if (_graphIsContinuous(checkpoint)) {
                SetStatus(BaseSupervisionStatus::kCorrect);
                LOG_DEBUG << "logical supervision " << GetShortName() << " "
                          << GetCheckpointLocationString(checkpointLocation) << " checkpoint "
                          << checkpoint->GetCheckpointId() << " from process " << checkpoint->GetProcessName()
                          << " reached. status is correct because path is continous.";
            } else {
                SetStatus(BaseSupervisionStatus::kInCorrect);
                LOG_DEBUG << "logical supervision " << GetShortName() << " "
                          << GetCheckpointLocationString(checkpointLocation) << " checkpoint "
                          << checkpoint->GetCheckpointId() << " from process " << checkpoint->GetProcessName()
                          << " reached. status is incorrect because path is not continous.";
            }
        }
    }

    // [ASWS_HM_00331] If the result of the Logical Supervision triggered by
    // ReportCheckpoint is correct and the Checkpoint is defined as a final one,
    // then the function ReportCheckpoint shall set Graph as inactive. After a
    // final checkpoint, only initial checkpoints are possible
    if ((GetStatus() == BaseSupervisionStatus::kCorrect) && (checkpointLocation == CheckpointLocation::kFinal)) {
        graphActive_.store(false);
    }

    checkpointsReachedFlag_[checkpoint->GetFqn()] = true;
    _NotifyStatusToLocalSupervision();
}

/// @brief Returns whether the path to this checkpoint is continous.
/// @param checkpoint object of checkpoint.
/// @return true the path to this checkpoint is continous;false, the path to this checkpoint is not continous
bool LogicalSupervision::_graphIsContinuous(std::shared_ptr< SupervisionCheckpoint > const &checkpoint) noexcept
{
    if (transition_.count(checkpoint->GetFqn()) > 0U) {
        for (std::shared_ptr< SupervisionCheckpoint > const &cp : transition_[checkpoint->GetFqn()]) {
            if (checkpointsReachedFlag_[cp->GetFqn()] == false) {
                LOG_INFO << "transition from " << cp->GetShortName().c_str() << " to "
                         << checkpoint->GetShortName().c_str() << ", but upstream is not reached.";
                return false;
            }
        }
    } else {
        /// assert？
        LOG_ERROR << "checkpoint not found in transition, checkpoint fqn " << checkpoint->GetFqn().c_str();
        return false;
    }
    return true;
}

/// @brief Returns the location os checkpoint in the graph.
/// @param checkpoint checkpoint
/// @return the location os checkpoint in the graph.
CheckpointLocation LogicalSupervision::_getCheckpointLocation(
    std::shared_ptr< SupervisionCheckpoint > const &checkpoint) const noexcept
{
    // The order of branches cannot be changed
    if (initialCheckpoint_.count(checkpoint->GetFqn()) != 0U) {
        return CheckpointLocation::kInitial;
    }
    if (finalCheckpoint_.count(checkpoint->GetFqn()) != 0U) {
        return CheckpointLocation::kFinal;
    }
    if (transition_.count(checkpoint->GetFqn()) != 0U) {
        return CheckpointLocation::kTransition;
    }
    LOG_WARN << "checkpoint " << checkpoint->GetFqn().c_str() << " not belongs to logical supervision "
             << GetShortName().c_str();
    return CheckpointLocation::kInvalid;
}

/// @brief returns the string of checkpoint location.
/// @param location checkpoint location.
/// @return the string of checkpoint location.
ara::core::StringView LogicalSupervision::GetCheckpointLocationString(CheckpointLocation const &location) noexcept
{
    if (location == CheckpointLocation::kInitial) {
        return "Initial";
    }
    if (location == CheckpointLocation::kFinal) {
        return "Final";
    }
    if (location == CheckpointLocation::kTransition) {
        return "Transition";
    }
    return "Valid";
}

/// @brief Stop this supervision.
void LogicalSupervision::Stop() noexcept { LOG_INFO << "stop " << GetFqn(); }

/// @brief Start this supervision.
void LogicalSupervision::Start() noexcept
{
    // nothing
    LOG_INFO << "start " << GetFqn();
}

/// @brief Reset statistics of this supervision.
void LogicalSupervision::ResetStatistics() noexcept
{
    _resetCheckpointReachedFlag();
    graphActive_.store(false);
}

/// @brief Set all the checkpoint reached flag to false.
void LogicalSupervision::_resetCheckpointReachedFlag() noexcept
{
    for (auto const &element : checkpointsReachedFlag_) {
        checkpointsReachedFlag_[element.first] = false;
    }
}

/// @brief Returns the process name this supervision belongs to,when supervison need recover, should get the process
/// name.
/// @return the process name this supervision belongs to.
ara::core::String LogicalSupervision::GetProcessName() noexcept { return lastCheckpoint_->GetProcessName(); }

/// @brief Returns process cluster affiliation.
/// @return process cluster affiliation.
ara::phm::internal::svcom::ProcessClusterAffiliation LogicalSupervision::GetProcessClusterAffiliation() noexcept
{
    return std::move(lastCheckpoint_->GetProcessClusterAffiliation());
}

/// @brief Returns the checkpoints referenced by this supervision.
/// @return the checkpoints referenced by this supervision.
ara::core::Vector< std::shared_ptr< SupervisionCheckpoint > > LogicalSupervision::GetCheckpoint() noexcept
{
    return allCheckpoint_;
}

/// @brief Returns whether checkpoint graph is active.
/// @return true, active;false, not active
bool LogicalSupervision::GetGraphActive() const noexcept { return graphActive_; }

}  // namespace internal
}  // namespace phm
}  // namespace ara
