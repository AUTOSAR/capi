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
/// @file       base_supervision.cpp
/// @brief
/// @details
/// @date       2024-06-20
/// @author     wangyanlong
/// @version    1.2.0
///
/// ================================================================

#include "ara/phm/internal/phm_log.h"
#include "ara/phm/internal/svmanager/alive_supervision.h"

namespace ara {
namespace phm {
namespace internal {
/// @brief Uesed by local supervision to register status listener. When base supervision status is updated,
/// this callback is called to notify the local supervision.
/// @param localSupervisionFqn the fqn of local supervision.
/// @param handler callback.
void BaseSupervision::RegisterStatusUpdateHandler(ara::core::String const& localSupervisionFqn,
                                                  BaseStatusUpdateHandler const& handler) noexcept
{
    LOG_INFO << "local supervision " << localSupervisionFqn.c_str() << " register status listener to "
             << kShortName.c_str();

    if (statusUpdateHandler_[localSupervisionFqn] == nullptr) {
        statusUpdateHandler_[localSupervisionFqn] = handler;
    } else {
        LOG_WARN << "local supervision " << localSupervisionFqn.c_str() << " register status handler repeatdly.";
    }
}

/// @brief Uesed by local supervision to un register status listener.
/// @param localSupervisionFqn the fqn of local supervision.
void BaseSupervision::UnRegisterStatusUpdateHandler(ara::core::String const& localSupervisionFqn) noexcept
{
    LOG_INFO << "local supervision " << localSupervisionFqn.c_str() << " remove status handler from "
             << kShortName.c_str();
    std::ignore = statusUpdateHandler_.erase(localSupervisionFqn);
}

/// @brief Returns the type of this supervision.
/// @return the type of this supervision.
BaseSupervisionType BaseSupervision::GetType() const noexcept { return kType; }

/// @brief Set active of this supervision.
/// @param active true, active; false, inactive.
void BaseSupervision::SetActive(const bool active) noexcept
{
    LOG_INFO << "active of " << kShortName.c_str() << "," << active_ << "-->" << active;
    active_ = active;
}

/// @brief Return whether this supervision is active.
/// @return true, active;false, inactive.
bool BaseSupervision::GetActive() const noexcept { return active_; }

/// @brief Set supevision status, kCorrect or kInCorrect.
/// @param status status of supervision.
void BaseSupervision::SetStatus(BaseSupervisionStatus const& status) noexcept
{
    if (status == status_) {
        return;
    }

    LOG_INFO << kShortName << ", enter status " << GetBaseSupervisionStatusString(status);
    status_ = status;
}

/// @brief Returns the status of this supervision.
/// @return the status of this supervision.
BaseSupervisionStatus BaseSupervision::GetStatus() const noexcept { return status_; }

/// @brief Returns the fqn of base supervision.
/// @return the fqn of base supervision.
ara::core::String BaseSupervision::GetFqn() const noexcept { return kFqn; }

/// @brief returns shortName
/// @return shortName
ara::core::String BaseSupervision::GetShortName() const noexcept { return kShortName; }

/// @brief Notify status of base supervision to local supervision.
void BaseSupervision::_NotifyStatusToLocalSupervision() noexcept
{
    for (auto const& handler : statusUpdateHandler_) {
        if (handler.second) {
            handler.second(kType, status_, GetProcessName(), GetProcessClusterAffiliation());
        } else {
            LOG_ERROR << "handler is null " << handler.first.c_str() << ", for " << kShortName.c_str();
        }
    }
}

/// @brief returns the string of base supervision status.
/// @param status enum.
/// @return the string of base supervision status.
ara::core::StringView BaseSupervision::GetBaseSupervisionStatusString(BaseSupervisionStatus const& status) noexcept
{
    if (status == BaseSupervisionStatus::kCorrect) {
        return "Correct";
    }
    return "Incorrect";
}

}  // namespace internal
}  // namespace phm
}  // namespace ara
