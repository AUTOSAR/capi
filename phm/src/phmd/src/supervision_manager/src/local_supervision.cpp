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
/// @file       local_supervision.cpp
/// @brief      A supervised level, a LocalSupervision should reference at least one base supervision.
/// @details
/// @date       2024-06-05
/// @author     wangyanlong
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/PlatformHealthManagement/SupervisionManager
/// @interface_level=unit
/// @trace_id_sr=SR_PHM_01009,SR_PHM_01010
/// @unit_name=LocalSupervision
/// @unit_description=A supervised level, a LocalSupervision should reference at least one base supervision.
/// @endcode
///
/// ================================================================

#include "ara/phm/internal/svmanager/local_supervision.h"

namespace ara {
namespace phm {
namespace internal {
/// @brief Creation of a LocalSupervision.
/// @param baseSupervision referenced base supervision.
/// @param conf conf of local supervision.
LocalSupervision::LocalSupervision(ara::core::Vector< std::shared_ptr< BaseSupervision > > baseSupervision,  //NOLINT
                                   LocalSupervisionConf const& conf) noexcept
    : kShortName{conf.shortName}
    , kFqn{conf.fqn}
    , kFailedSupervisionCyclesTolerance{conf.failedSupervisionCyclesTolerance}
    , failedSupervisionReferenceCycles_{0U}
    , status_{LocalSupervisionStatus::kDeactivated}
    , baseSupervision_{}
    , statusUpdateHandler_{}
{
    LOG_INFO << "create LocalSupervision, name " << kShortName.c_str() << ", fqn " << kFqn.c_str()
             << ", kFailedSupervisionCyclesTolerance " << kFailedSupervisionCyclesTolerance
             << ", base supervision count " << baseSupervision.size();

    ;

    for (std::shared_ptr< BaseSupervision >& element : baseSupervision) {
        baseSupervision_[element->GetFqn()] = element;
        element->RegisterStatusUpdateHandler(
            // TODO(wangyanlong): std:bind is not recommended in QAC
            kFqn, std::bind(&LocalSupervision::_baseSupervisionStatusUpdated, this, std::placeholders::_1,
                            std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
    }
}

/// @brief Destructor.
LocalSupervision::~LocalSupervision() noexcept
{
    LOG_INFO << "deconstruct local supervision " << kShortName;
    for (auto const& element : baseSupervision_) {
        element.second->UnRegisterStatusUpdateHandler(kFqn);
    }
}

/// @brief Used by GlobalSupervision to register status listener. When local supervision status updated,
/// this callback is called to notify the global supervision this local supervision belongs to.
/// @param statusUpdateHandler callback local supervision used to notify the global supervision status updated.
void LocalSupervision::RegisterStatusUpdateHandler(StatusUpdateHandler const& statusUpdateHandler) noexcept
{
    LOG_INFO << kShortName << " register status update handler.";
    statusUpdateHandler_ = statusUpdateHandler;
}

/// @brief Used by GlobalSupervision to remove status listener.
void LocalSupervision::UnRegisterStatusUpdateHandler() noexcept
{
    LOG_INFO << kShortName << " unregister status update handler.";
    statusUpdateHandler_ = nullptr;
}

/// @brief returns the string of local supervision status.
/// @param status enum.
/// @return the string of local supervision status.
ara::core::StringView LocalSupervision::GetLocalSupervisionStatusString(LocalSupervisionStatus const& status) noexcept
{
    if (status == LocalSupervisionStatus::kOK) {
        return "OK";
    }
    if (status == LocalSupervisionStatus::kDeactivated) {
        return "Deactivated";
    }
    if (status == LocalSupervisionStatus::kFailed) {
        return "Failed";
    }
    return "Expired";
}

/// @brief set status
/// @param status new status
void LocalSupervision::_setStatus(LocalSupervisionStatus const& status) noexcept
{
    if (status == status_) {
        return;
    }

    LOG_DEBUG << kShortName << " enter " << GetLocalSupervisionStatusString(status);
    status_ = status;
}

/// @brief Returns base supervisions referenced by this local supervision.
/// @return base supervisions referenced by this local supervision.
ara::core::Vector< std::shared_ptr< BaseSupervision > > LocalSupervision::GetBaseSupervision() noexcept
{
    ara::core::Vector< std::shared_ptr< BaseSupervision > > ret;
    for (auto const& element : baseSupervision_) {
        ret.push_back(element.second);
    }
    return ret;
}

/// @brief Switch supervision mode.
void LocalSupervision::SwitchSupervisionMode() noexcept
{
    LOG_DEBUG << "switch supervision mode of local supervision " << kShortName;

    bool atLeastOneBaseSupervisionIsActive{false};
    bool atLeastOneAliveSupervisionIsIncorrect{false};
    bool atLeastOneDeadlineOrLogicalSupervisionIsIncorrect{false};
    for (auto const& element : baseSupervision_) {
        if (element.second->GetActive() == true) {
            atLeastOneBaseSupervisionIsActive = true;
            if (element.second->GetStatus() == BaseSupervisionStatus::kInCorrect) {
                if (element.second->GetType() == BaseSupervisionType::kAlive) {
                    atLeastOneAliveSupervisionIsIncorrect = true;
                } else {
                    atLeastOneDeadlineOrLogicalSupervisionIsIncorrect = true;
                }
            }
        }
    }

    if (atLeastOneBaseSupervisionIsActive) {
        if (status_ == LocalSupervisionStatus::kDeactivated) {
            // Do not clear the failure count to 0
            _setStatus(LocalSupervisionStatus::kOK);
        } else if (status_ == LocalSupervisionStatus::kFailed) {
            if (atLeastOneAliveSupervisionIsIncorrect == false) {
                _setStatus(LocalSupervisionStatus::kOK);
                failedSupervisionReferenceCycles_ = 0U;
            }
        } else if (status_ == LocalSupervisionStatus::kExpired) {
            if (!(atLeastOneDeadlineOrLogicalSupervisionIsIncorrect
                  || (atLeastOneAliveSupervisionIsIncorrect
                      && (failedSupervisionReferenceCycles_ >= kFailedSupervisionCyclesTolerance)))) {
                _setStatus(LocalSupervisionStatus::kOK);
                failedSupervisionReferenceCycles_ = 0U;
            }
        } else {
            // keep ok
        }
    } else {
        _setStatus(LocalSupervisionStatus::kDeactivated);
        failedSupervisionReferenceCycles_ = 0U;
    }
}

/// @brief This function is registered to base supervision. When base supervision status is updated,
/// this callback is called, so local supervision can hear this.
/// @throws QAC
/// @param type type of base supervision.
/// @param status status of base supervision.
/// @param processName process name.
/// @param processClusterAffiliation process cluster affiliation
void LocalSupervision::_baseSupervisionStatusUpdated(
    BaseSupervisionType const type,
    BaseSupervisionStatus const baseStatus,
    ara::core::String const& processName,
    ara::phm::internal::svcom::ProcessClusterAffiliation const processClusterAffiliation)
{
    if (status_ == LocalSupervisionStatus::kExpired) {
        LOG_DEBUG << "local supervision " << kShortName << " now is expired, do not to do further process.";
        return;
    }

    if ((type == BaseSupervisionType::kDeadline) || (type == BaseSupervisionType::kLogical)) {
        if (baseStatus == BaseSupervisionStatus::kInCorrect) {
            _setStatus(LocalSupervisionStatus::kExpired);
        }
    } else {  // alive
        // TODO(wangyanglong): clang-tidy suggests that the processing logic of the two branches is the same
        if (status_ == LocalSupervisionStatus::kOK) {  // NOLINT
            if (baseStatus == BaseSupervisionStatus::kCorrect) {
                _setStatus(LocalSupervisionStatus::kOK);
            } else {
                if (kFailedSupervisionCyclesTolerance == 0U) {
                    _setStatus(LocalSupervisionStatus::kExpired);
                } else {
                    _setStatus(LocalSupervisionStatus::kFailed);
                    failedSupervisionReferenceCycles_++;
                }
            }
        } else if (status_ == LocalSupervisionStatus::kFailed) {
            if (baseStatus == BaseSupervisionStatus::kCorrect) {
                if (failedSupervisionReferenceCycles_ > 1U) {
                    _setStatus(LocalSupervisionStatus::kFailed);
                    failedSupervisionReferenceCycles_--;
                } else {
                    _setStatus(LocalSupervisionStatus::kOK);
                    failedSupervisionReferenceCycles_--;
                }
            } else {
                if (failedSupervisionReferenceCycles_ < kFailedSupervisionCyclesTolerance) {
                    _setStatus(LocalSupervisionStatus::kFailed);
                    failedSupervisionReferenceCycles_++;
                } else {
                    _setStatus(LocalSupervisionStatus::kExpired);
                }
            }
        } else if (status_ == LocalSupervisionStatus::kDeactivated) {
            // 2311 SWS_PHM_01354
            if (baseStatus == BaseSupervisionStatus::kCorrect) {
                _setStatus(LocalSupervisionStatus::kOK);
            } else {
                if (kFailedSupervisionCyclesTolerance == 0U) {
                    _setStatus(LocalSupervisionStatus::kExpired);
                } else {
                    _setStatus(LocalSupervisionStatus::kFailed);
                    failedSupervisionReferenceCycles_++;
                }
            }
        } else {
            // QAC
        }
    }

    LOG_DEBUG << "local supervision " << kShortName << ",failedCounter=" << failedSupervisionReferenceCycles_
              << ",tolerance=" << failedSupervisionReferenceCycles_ << ", status is "
              << GetLocalSupervisionStatusString(status_);
    if (statusUpdateHandler_) {
        statusUpdateHandler_(kFqn, status_, type, processName, processClusterAffiliation);
    } else {
        LOG_ERROR << kShortName.c_str() << " statusUpdateHandler_ is null";
    }
}

}  // namespace internal
}  // namespace phm
}  // namespace ara