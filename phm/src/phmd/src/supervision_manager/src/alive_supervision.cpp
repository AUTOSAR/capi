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
/// @file       alive_supervision.cpp
/// @brief
/// @details
/// @date       2024-06-20
/// @author     wangyanlong
/// @version    1.2.0
///
/// ================================================================

#include "ara/phm/internal/svmanager/alive_supervision.h"

#include "ara/phm/internal/phm_log.h"

namespace ara {
namespace phm {
namespace internal {
/// @brief Constructor.
/// @param conf conf of alive supervision.
/// @param checkpoint checkpoint referenced by this alive supervision.
AliveSupervision::AliveSupervision(AliveSupervisionConf const& conf,
                                   std::shared_ptr< SupervisionCheckpoint > checkpoint) noexcept
    : BaseSupervision{conf.shortName, conf.fqn, BaseSupervisionType::kAlive}
    , kAliveReferenceCycle{SECOND_TO_MS(conf.aliveRefrenceCycle)}
    , kExpectedAliveIndications{conf.expectedAliveIndications}
    , kMinMargin{conf.minMargin}
    , kMaxMargin{conf.maxMargin}
    , checkpoint_{std::move(checkpoint)}
    , indications_{0U}
    , checkCount_{0U}
    , supervisionTimer_{nullptr}

{
    LOG_INFO << "create AliveSupervision " << conf.shortName.c_str() << ", aliveRefrenceCycle " << kAliveReferenceCycle
             << ", expectedAliveIndications " << kExpectedAliveIndications << ", minMargin " << kMinMargin
             << ", kMaxMargin " << kMaxMargin;
    indications_ = 0U;
    checkCount_  = 0U;
    supervisionTimer_
        = std::make_unique< ara::phm::internal::Timer >("alivesupervision_" + conf.shortName, kAliveReferenceCycle,
                                                        // TODO(wangyanlong): std:bind is not recommended in QAC
                                                        std::bind(&AliveSupervision::_supervisionTimerCb, this), true);
}

/// @brief Default deconstructor.
AliveSupervision::~AliveSupervision() noexcept
{
    LOG_INFO << "destroy alive supervision " << GetShortName();
    Stop();
    supervisionTimer_.reset();
}

/// @brief Process checkpoint reported from AA.
/// @param checkpoint object of checkpoint.
/// @param processId the process id who report this checkpoint.
/// @param checkpointTimestamp timestamp when process send checkpoint.
void AliveSupervision::ProcessCheckpoint(std::shared_ptr< SupervisionCheckpoint > const& checkpoint,
                                         ProcessId const processId,
                                         int64_t const checkpointTimestamp) noexcept
{
    std::ignore = checkpoint;
    std::ignore = processId;
    std::ignore = checkpointTimestamp;

    if (!GetActive()) {
        LOG_DEBUG << "alive supervision " << GetShortName() << " is not active.";
        return;
    }

    std::ignore = indications_.fetch_add(1U);
    LOG_DEBUG << "alive supervision " << GetShortName() << " checkpoint reached " << indications_.load()
              << " times in this cycle. present status is " << GetBaseSupervisionStatusString(GetStatus())
              << ", checkpoint from process " << checkpoint->GetProcessName();
}

/// @brief Stop this supervision.
void AliveSupervision::Stop() noexcept
{
    LOG_INFO << "stop alive supervision " << GetFqn();
    supervisionTimer_->Stop();
}

/// @brief Start this supervision.
void AliveSupervision::Start() noexcept
{
    LOG_INFO << "start alive supervision " << GetFqn();
    supervisionTimer_->Stop();
    std::ignore = supervisionTimer_->Start();
}

/// @brief Reset statistics of this supervision.
void AliveSupervision::ResetStatistics() noexcept
{
    indications_.store(0U);
    checkCount_.store(0U);
}

/// @brief Returns the indications reached within a period.
/// @return the indications reached within a period.
uint32_t AliveSupervision::GetIndications() const noexcept { return indications_; }

/// @brief Returns checkcount, times the _supervisionTimerCb called.
/// @return checkcount.
uint32_t AliveSupervision::GetCheckCount() const noexcept { return checkCount_; }

/// @brief Returns the process name this supervision belongs to, when supervison need recover, should get the
/// process name.
/// @return the process name this supervision belongs to.
ara::core::String AliveSupervision::GetProcessName() noexcept { return checkpoint_->GetProcessName(); }

/// @brief Returns process cluster affiliation.
/// @return process cluster affiliation.
ara::phm::internal::svcom::ProcessClusterAffiliation AliveSupervision::GetProcessClusterAffiliation() noexcept
{
    return std::move(checkpoint_->GetProcessClusterAffiliation());
}

/// @brief Returns the checkpoints referenced by this supervision.
/// @return the checkpoints referenced by this supervision.
ara::core::Vector< std::shared_ptr< SupervisionCheckpoint > > AliveSupervision::GetCheckpoint() noexcept
{
    ara::core::Vector< std::shared_ptr< SupervisionCheckpoint > > ret;
    ret.push_back(checkpoint_);
    return ret;
}

/// @brief Determine status of alive supervision in this function.
void AliveSupervision::_supervisionTimerCb()
{
    uint32_t const localIndications{indications_.load()};
    indications_.store(0U);
    if ((localIndications >= kExpectedAliveIndications - kMinMargin)
        && (localIndications <= kExpectedAliveIndications + kMaxMargin)) {
        SetStatus(BaseSupervisionStatus::kCorrect);
    } else {
        SetStatus(BaseSupervisionStatus::kInCorrect);
    }

    LOG_DEBUG << "cyclicly check alive supervision " << GetShortName() << ", status is "
              << GetBaseSupervisionStatusString(GetStatus()) << ", indications " << localIndications << ", expected "
              << kExpectedAliveIndications << ", min margin " << kMinMargin << ", max margin " << kMaxMargin;

    _NotifyStatusToLocalSupervision();
    checkCount_++;
}

}  // namespace internal
}  // namespace phm
}  // namespace ara
