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
/// @file       global_supervision.cpp
/// @brief
/// @details
/// @date       2024-06-20
/// @author     wangyanlong
/// @version    1.2.0
///
/// ================================================================

#include "ara/phm/internal/svmanager/global_supervision.h"

#include "ara/phm/internal/phm_log.h"

namespace ara {
namespace phm {
namespace internal {

/// @brief Creation of a GlobalSupervision.
/// @param localSupervision local supervision referenced by global supervision.
/// @param conf conf of global supervision.
GlobalSupervision::GlobalSupervision(
    ara::core::Vector< std::shared_ptr< LocalSupervision > > localSupervision,  //NOLINT
    GlobalSupervisionConf const& conf) noexcept
    : kShortName{conf.shortName}
    , kMetaModelIdentifier{conf.metaModelIdentifier}
    , kExpiredSupervisionCycleTolerance{conf.expiredSupervisionCycleTolerance}
    , expiredSupervisionCycle_{0U}
    , kRecoveryNotificationTimeout{SECOND_TO_MS(conf.recoveryNotificationTimeout)}
    , kRecoveryNotificationRetry{conf.recoveryNotificationRetry}
    , kSupervisionCycle{SECOND_TO_MS(conf.supervisionCycle)}
    , status_{GlobalSupervisionStatus::kDeactivated}
    , localSupervision_{}
    , supervisionTimer_{nullptr}
    , statusChangeHandler_{}
    , baseSupervisionTypeCausedToExpired_{BaseSupervisionType::kAlive}
    , processNameCausedToExpired_{}
    , processClusterAffiliation_{ara::phm::internal::svcom::ProcessClusterAffiliation::kOther}
    , offered_{true}

{
    LOG_INFO << "create a global supervision, name " << kShortName.c_str() << ", local supervision count "
             << localSupervision.size() << ", metaModelIdentifier " << kMetaModelIdentifier.c_str()
             << ", expiredSupervisionCycleTolerance " << kExpiredSupervisionCycleTolerance
             << ", expiredSupervisionCycle " << expiredSupervisionCycle_ << ", recoveryNotificationTimeout "
             << kRecoveryNotificationTimeout << ", recoveryNotificationRetry " << kRecoveryNotificationRetry;

    for (std::shared_ptr< LocalSupervision >& localSv : localSupervision) {
        localSupervision_[localSv->GetFqn()] = localSv;
        localSv->RegisterStatusUpdateHandler(
            // TODO(wangyanlong): std:bind is not recommended in QAC
            std::bind(&GlobalSupervision::_localSupervisionStatusChanged, this, std::placeholders::_1,
                      std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5));
    }

    supervisionTimer_
        = std::make_unique< ara::phm::internal::Timer >("globalsupervision_" + conf.shortName, kSupervisionCycle,
                                                        // TODO(wangyanlong): std:bind is not recommended in QAC
                                                        std::bind(&GlobalSupervision::_supervisionTimerCb, this), true);
    std::ignore = supervisionTimer_->Start();
}

/// @brief Destructor.
GlobalSupervision::~GlobalSupervision() noexcept
{
    LOG_INFO << "destroy global supervision " << kShortName;
    Stop();
    supervisionTimer_.reset();
    for (auto const& localSv : localSupervision_) {
        localSv.second->UnRegisterStatusUpdateHandler();
    }
}

/// @brief Used by SupervisionController to register status listener.
/// @param statusChangeHandler status listener of SupervisionController
/// @note In SupervisionManager, find the corresponding supervision mode according to the process name
void GlobalSupervision::RegisterStatusChangeHandler(StatusChangeHandler const& statusChangeHandler) noexcept
{
    LOG_INFO << "register status handler.";
    statusChangeHandler_ = statusChangeHandler;
}

/// @brief Used by SupervisionController to remove status listener.
void GlobalSupervision::UnRegisterStatusChangeHandler() noexcept
{
    LOG_INFO << "remove status handler.";
    statusChangeHandler_ = nullptr;
}

/// @brief Returns the local supervision referenced by global supervision.
/// @return the local supervision referenced by global supervision.
ara::core::Vector< std::shared_ptr< LocalSupervision > > GlobalSupervision::GetLocalSupervision() noexcept
{
    ara::core::Vector< std::shared_ptr< LocalSupervision > > ret;
    for (auto const& element : localSupervision_) {
        ret.push_back(element.second);
    }
    return ret;
}

/// @brief Returns the recovery notification timeout.
/// @return the recovery notification timeout.
int32_t GlobalSupervision::GetRecoveryNotificationTimeout() const noexcept { return kRecoveryNotificationTimeout; }

/// @brief Returns the recovery notification retry.
/// @return the recovery notification retry.
uint32_t GlobalSupervision::GetRecoveryNotificationRetry() const noexcept { return kRecoveryNotificationRetry; }

/// @brief returns the string of status.
/// @param status
/// @return he string of status.
ara::core::StringView GlobalSupervision::GetGlobalSupervisionStatusString(
    GlobalSupervisionStatus const& status) noexcept
{
    if (status == GlobalSupervisionStatus::kDeactivated) {
        return "Deactivated";
    }
    if (status == GlobalSupervisionStatus::kOK) {
        return "OK";
    }
    if (status == GlobalSupervisionStatus::kFailed) {
        return "Failed";
    }
    if (status == GlobalSupervisionStatus::kExpired) {
        return "Expired";
    }
    return "Stopped";
}

/// @brief set status.
/// @param status status.
void GlobalSupervision::_setStatus(GlobalSupervisionStatus const& status) noexcept
{
    if (status == status_) {
        return;
    }

    LOG_DEBUG << kShortName << " enter " << GetGlobalSupervisionStatusString(status);
    status_ = status;
}

/// @brief Stop supervision.
void GlobalSupervision::Stop() const noexcept
{
    LOG_INFO << "stop global supervision " << kShortName;
    if (supervisionTimer_) {
        supervisionTimer_->Stop();
    }
}

/// @brief This function is registered to local supervision. When local supervision status is updated,
/// this function is called.
/// @throws QAC
/// @param loalSupervisionFqn  fqn of local supervision.
/// @param localSupervisionStatus status of local supervision.
/// @param baseSupervisionType type of base supervision.
/// @param processName process name.
/// @param process cluster affiliation.
/// @param processClusterAffiliation process cluster affiliation.
void GlobalSupervision::_localSupervisionStatusChanged(
    ara::core::String const& loalSupervisionFqn,
    LocalSupervisionStatus const& localSupervisionStatus,
    BaseSupervisionType const& baseSupervisionType,
    ara::core::String const& processName,
    ara::phm::internal::svcom::ProcessClusterAffiliation const processClusterAffiliation)
{
    std::ignore = loalSupervisionFqn;
    if (localSupervisionStatus == LocalSupervisionStatus::kExpired) {
        baseSupervisionTypeCausedToExpired_ = baseSupervisionType;
        processNameCausedToExpired_         = processName;
        processClusterAffiliation_          = processClusterAffiliation;
    }
}

/// @brief Called periodically to determine the status of global supervision.
/// @throws QAC
void GlobalSupervision::_supervisionTimerCb()
{
    _determineStatus();
    LOG_DEBUG << "cyclicly update status of global supervision " << kShortName << ":"
              << GetGlobalSupervisionStatusString(status_) << ",expiredCounter=" << expiredSupervisionCycle_
              << ",tolerance=" << kExpiredSupervisionCycleTolerance;
    // The second parameter is meaningful. In the 2311 standard, not only this state needs to be passed
    if (status_ == GlobalSupervisionStatus::kStopped) {
        if (statusChangeHandler_) {
            statusChangeHandler_(kShortName, status_, baseSupervisionTypeCausedToExpired_, processNameCausedToExpired_,
                                 processClusterAffiliation_);
        } else {
            LOG_ERROR << "statusChangeHandler_ of global supervision " << kShortName.c_str() << " is null";
        }
    }
}

/// @brief determine the status of global supervision
void GlobalSupervision::_determineStatus() noexcept
{
    bool atLeastOneLocalSupervisionIsFailed{false};
    bool atLeastOneLocalSupervisionIsExpired{false};
    bool allLocalSupervisionDeactivated{true};
    for (auto const& localSv : localSupervision_) {
        LocalSupervisionStatus const localStatus{localSv.second->GetStatus()};

        if (localStatus != LocalSupervisionStatus::kDeactivated) {
            allLocalSupervisionDeactivated = false;
        }

        if (localStatus == LocalSupervisionStatus::kExpired) {
            atLeastOneLocalSupervisionIsExpired = true;
        }

        if (localStatus == LocalSupervisionStatus::kFailed) {
            atLeastOneLocalSupervisionIsFailed = true;
        }
    }

    if (allLocalSupervisionDeactivated == true) {
        status_                  = GlobalSupervisionStatus::kDeactivated;
        expiredSupervisionCycle_ = 0U;
        return;
    }

    if (status_ == GlobalSupervisionStatus::kOK) {
        if (!atLeastOneLocalSupervisionIsExpired && !atLeastOneLocalSupervisionIsFailed) {
            _setStatus(GlobalSupervisionStatus::kOK);
        } else if (atLeastOneLocalSupervisionIsFailed && !atLeastOneLocalSupervisionIsExpired) {
            _setStatus(GlobalSupervisionStatus::kFailed);
        } else if (atLeastOneLocalSupervisionIsExpired && (kExpiredSupervisionCycleTolerance > 0U)) {
            _setStatus(GlobalSupervisionStatus::kExpired);
            expiredSupervisionCycle_++;
        } else if (atLeastOneLocalSupervisionIsExpired && (kExpiredSupervisionCycleTolerance == 0U)) {
            _setStatus(GlobalSupervisionStatus::kStopped);
        } else {
            // Keep the state unchanged
        }
    } else if (status_ == GlobalSupervisionStatus::kFailed) {
        if (atLeastOneLocalSupervisionIsFailed && !atLeastOneLocalSupervisionIsExpired) {
            _setStatus(GlobalSupervisionStatus::kFailed);
        } else if (!atLeastOneLocalSupervisionIsExpired && !atLeastOneLocalSupervisionIsFailed) {
            _setStatus(GlobalSupervisionStatus::kOK);
        } else if (atLeastOneLocalSupervisionIsExpired && (kExpiredSupervisionCycleTolerance > 0U)) {
            _setStatus(GlobalSupervisionStatus::kExpired);
            expiredSupervisionCycle_++;
        } else if (atLeastOneLocalSupervisionIsExpired && (kExpiredSupervisionCycleTolerance == 0U)) {
            _setStatus(GlobalSupervisionStatus::kStopped);
        } else {
            // Keep the state unchanged
        }
    } else if (status_ == GlobalSupervisionStatus::kExpired) {
        if (atLeastOneLocalSupervisionIsExpired && (expiredSupervisionCycle_ < kExpiredSupervisionCycleTolerance)) {
            _setStatus(GlobalSupervisionStatus::kExpired);
            expiredSupervisionCycle_++;
        } else if (atLeastOneLocalSupervisionIsExpired
                   && (expiredSupervisionCycle_ == kExpiredSupervisionCycleTolerance)) {
            _setStatus(GlobalSupervisionStatus::kStopped);
        } else {
            // Keep the state unchanged
        }
    } else if (status_ == GlobalSupervisionStatus::kStopped) {
        _setStatus(GlobalSupervisionStatus::kStopped);
    } else {
        //QAC
    }
}

/// @brief Switch supervision mode.
void GlobalSupervision::SwitchSupervisionMode() noexcept
{
    LOG_DEBUG << "switch supervision mode of global supervision " << kShortName;

    bool atLeastOneLocalSupervisionIsActive{false};
    bool atLeastOneLocalSupervisionIsFailed{false};
    bool alLeastOneLocalSupervisionIsExpired{false};
    for (auto const& localSv : localSupervision_) {
        if (localSv.second->GetStatus() != LocalSupervisionStatus::kDeactivated) {
            atLeastOneLocalSupervisionIsActive = true;
        }

        if (localSv.second->GetStatus() != LocalSupervisionStatus::kFailed) {
            atLeastOneLocalSupervisionIsFailed = true;
        }

        if (localSv.second->GetStatus() != LocalSupervisionStatus::kExpired) {
            alLeastOneLocalSupervisionIsExpired = true;
        }
    }

    if (atLeastOneLocalSupervisionIsActive) {
        if (status_ == GlobalSupervisionStatus::kDeactivated) {
            // Do not clear the timeout count
            _setStatus(GlobalSupervisionStatus::kOK);
            std::ignore = supervisionTimer_->Start();
        } else if (status_ == GlobalSupervisionStatus::kFailed) {
            if (!atLeastOneLocalSupervisionIsFailed) {
                _setStatus(GlobalSupervisionStatus::kOK);
                expiredSupervisionCycle_ = 0U;
            }
        } else if (status_ == GlobalSupervisionStatus::kExpired) {
            if (!alLeastOneLocalSupervisionIsExpired) {
                _setStatus(GlobalSupervisionStatus::kOK);
                expiredSupervisionCycle_ = 0U;
            }
        } else {
            // stoped state remains unchanged
        }
    } else {
        Stop();
        _setStatus(GlobalSupervisionStatus::kDeactivated);
        expiredSupervisionCycle_ = 0U;
    }
}

}  // namespace internal
}  // namespace phm
}  // namespace ara
