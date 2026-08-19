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
/// @file       deadline_supervision.cpp
/// @brief
/// @details
/// @date       2024-06-20
/// @author     wangyanlong
/// @version    1.2.0
///
/// ================================================================

#include "ara/phm/internal/svmanager/deadline_supervision.h"

#include "ara/phm/internal/phm_log.h"

namespace ara {
namespace phm {
namespace internal {
/// @brief Creation of a DeadlineSupervision.
/// @param conf conf of deadline supervision.
/// @param sourceCheckpoint source checkpoint referenced by deadline supervision.
/// @param targetCheckpoint target checkpoint referenced by deadline supervision.
DeadlineSupervision::DeadlineSupervision(DeadlineSupervisionConf const& conf,
                                         std::shared_ptr< SupervisionCheckpoint > sourceCheckpoint,
                                         std::shared_ptr< SupervisionCheckpoint > targetCheckpoint) noexcept
    : BaseSupervision{conf.shortName, conf.fqn, BaseSupervisionType::kDeadline}
    , kMaxDeadline{SECOND_TO_MS(conf.maxDeadline)}
    , kMinDeadline{SECOND_TO_MS(conf.minDeadline)}
    , sourceCheckpoint_{std::move(sourceCheckpoint)}
    , targetCheckpoint_{std::move(targetCheckpoint)}
    , sourceCheckpointReached_{false}
    , sourceCheckpointTimeStamp_{0}
    , supervisionTimer_{nullptr}
    , lastCheckpoint_{nullptr}

{
    LOG_INFO << "create DeadlineSupervision " << conf.shortName << ", minDeadline " << kMinDeadline << ", maxDeadline "
             << kMaxDeadline;

    sourceCheckpointReached_   = false;
    sourceCheckpointTimeStamp_ = 0;
    // TODO(wangyanlong): Should the input parameters be validated???
    // lastCheckpoint_ indicates the most recently reported checkpoint, ensuring lastCheckpoint_ is never empty, so assign a value here
    lastCheckpoint_   = sourceCheckpoint_;
    supervisionTimer_ = std::make_unique< ara::phm::internal::Timer >(
        "deadlinesupervision_" + conf.shortName, kMaxDeadline,
        // TODO(wangyanlong): std:bind is not recommended in QAC
        std::bind(&DeadlineSupervision::_supervisionTimerCb, this), false);
}

/// @brief Default deconstructor.
DeadlineSupervision::~DeadlineSupervision() noexcept
{
    LOG_INFO << "destroy deadline supervision " << GetShortName();
    Stop();
    supervisionTimer_.reset();
}

/// @brief Stop this supervision.
void DeadlineSupervision::Stop() noexcept
{
    LOG_INFO << "stop deadline supervision " << GetFqn();
    supervisionTimer_->Stop();
}

/// @brief Process checkpoint reported from AA.
/// @param checkpoint object of checkpoint.
/// @param processId the process id who report this checkpoint.
/// @param checkpointTimestamp timestamp when process send checkpoint.
void DeadlineSupervision::ProcessCheckpoint(std::shared_ptr< SupervisionCheckpoint > const& checkpoint,
                                            ProcessId const processId,
                                            int64_t const checkpointTimestamp) noexcept
{
    std::ignore = processId;
    std::ignore = checkpointTimestamp;

    lastCheckpoint_ = checkpoint;
    if (checkpoint == sourceCheckpoint_) {
        if (IsSourceCheckpointReached()) {
            // [ASWS_HM_00229] dWhen a given Source Checkpoint is reached two or more
            // times on or before the expiration of the maximum limit without reaching the corresponding Target
            // Checkpoint, this shall be considered as an error and the result of the DeadlineSupervision for this
            // SupervisedEntity shall be considered as incorrect
            SetStatus(BaseSupervisionStatus::kInCorrect);
            LOG_DEBUG << "deadline supervision " << GetShortName() << " source checkpoint "
                      << checkpoint->GetCheckpointId() << " from process " << checkpoint->GetProcessName()
                      << " reached. status is incorrect because source checkpoint reached repeatdly.";
        } else {
            SetStatus(BaseSupervisionStatus::kCorrect);
            if (!supervisionTimer_->IsRun()) {
                std::ignore = supervisionTimer_->Start();
            } else {
                // Strictly speaking, this condition is not needed. If it is not satisfied, it means the code implementation is unreasonable
                LOG_ERROR << "deadline supervision " << GetShortName().c_str() << " timer is already run.";
                supervisionTimer_->Stop();
                std::ignore = supervisionTimer_->Start();
            }
            LOG_DEBUG << "deadline supervision " << GetShortName() << " source checkpoint "
                      << checkpoint->GetCheckpointId() << " from process " << checkpoint->GetProcessName()
                      << " reached. status is correct.";
        }

        sourceCheckpointReached_   = true;
        sourceCheckpointTimeStamp_ = checkpointTimestamp;
    } else if (checkpoint == targetCheckpoint_) {
        supervisionTimer_->Stop();
        if (!IsSourceCheckpointReached()) {
            // [ASWS_HM_00354] dWhen a given Target Checkpoint is reached before the occurrence of the
            // corresponding Source Checkpoint, the function ReportCheckpoint
            // [SWS_HM_00447] shall ignore this Checkpoint and not update the result of the
            // Deadline Supervision for the Supervised Entity.c
            return;
        }
        // [ASWS_HM_00294] dIf the time difference between the Target Checkpoint and the
        // Source Checkpoint is not within the minimum and the maximum limits (that is, the
        // time difference is either less than MinDeadline or greater than MaxDeadline), then
        // the result of DeadlineSupervision for this SupervisedEntity shall be defined
        // as incorrect. Otherwise, it shall be defined as correct
        if ((checkpointTimestamp - sourceCheckpointTimeStamp_ < kMinDeadline)
            || (checkpointTimestamp - sourceCheckpointTimeStamp_ > kMaxDeadline)) {
            SetStatus(BaseSupervisionStatus::kInCorrect);
            LOG_DEBUG << "deadline supervision " << GetShortName() << " target checkpoint "
                      << checkpoint->GetCheckpointId() << " from process " << checkpoint->GetProcessName()
                      << " reached. status is incorrect because target - source is out of [mindeadline, maxdeadline]."
                      << ", source timestamp " << sourceCheckpointTimeStamp_ << ", target timestamp "
                      << checkpointTimestamp << ", durtion=" << checkpointTimestamp - sourceCheckpointTimeStamp_;
        } else {
            SetStatus(BaseSupervisionStatus::kCorrect);
            LOG_DEBUG << "deadline supervision " << GetShortName() << " target checkpoint "
                      << checkpoint->GetCheckpointId() << " from process " << checkpoint->GetProcessName()
                      << " reached. status is correct, "
                      << ", durtion=" << checkpointTimestamp - sourceCheckpointTimeStamp_;
        }
        ResetStatistics();
    } else {
        /// @details for QAC need this else
    }

    _NotifyStatusToLocalSupervision();
}

/// @brief Start this supervision.
void DeadlineSupervision::Start() noexcept
{
    // nothing
    LOG_INFO << "start " << GetFqn();
}

/// @brief Reset statistics of this supervision.
void DeadlineSupervision::ResetStatistics() noexcept
{
    sourceCheckpointReached_   = false;
    sourceCheckpointTimeStamp_ = 0;
}

/// @brief Returns the process name this supervision belongs to,when supervison need recover, should get the process
/// name.
/// @return the process name this supervision belongs to.
ara::core::String DeadlineSupervision::GetProcessName() noexcept { return lastCheckpoint_->GetProcessName(); }

/// @brief Returns process cluster affiliation.
/// @return process cluster affiliation.
ara::phm::internal::svcom::ProcessClusterAffiliation DeadlineSupervision::GetProcessClusterAffiliation() noexcept
{
    return std::move(lastCheckpoint_->GetProcessClusterAffiliation());
}

/// @brief Returns the checkpoints referenced by this supervision.
/// @return the checkpoints referenced by this supervision.
ara::core::Vector< std::shared_ptr< SupervisionCheckpoint > > DeadlineSupervision::GetCheckpoint() noexcept
{
    ara::core::Vector< std::shared_ptr< SupervisionCheckpoint > > ret;
    ret.push_back(sourceCheckpoint_);
    ret.push_back(targetCheckpoint_);
    return ret;
}

/// @brief Returns whether the source checkpoint reached.
/// @return true, source checkpoint reached;false, source checkpoint did not reach.
bool DeadlineSupervision::IsSourceCheckpointReached() const noexcept { return sourceCheckpointReached_; }

/// @brief The max deadline timeout callback.
/// @throws QAC
void DeadlineSupervision::_supervisionTimerCb()
{
    LOG_INFO << "deadline supervision " << GetShortName().c_str() << " timeout "
             << ara::phm::internal::TimeStamp::GetMs();
    SetStatus(BaseSupervisionStatus::kInCorrect);
    ResetStatistics();
    _NotifyStatusToLocalSupervision();
}

}  // namespace internal
}  // namespace phm
}  // namespace ara
