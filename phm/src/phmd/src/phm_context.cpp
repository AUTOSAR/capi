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
/// @file       phm_context.cpp
/// @brief
/// @details
/// @date       2024-06-20
/// @author     wangyanlong
/// @version    1.2.0
///
/// ================================================================

#include "phm_context.h"

#include <isoft/ara_fsh/filesystem_hierarchy.h>
#include <unistd.h>

#include "ara/phm/internal/phm_log.h"

namespace ara {
namespace phm {
namespace internal {
/// @brief Init PhmContext, create and init other module such as
/// SupervisionManager、HealthChannelManager.
/// @return 0, success；< 0, fail.
int32_t PhmContext::Init() noexcept
{
    LOG_INFO << "PhmContext init start.";

    evLoopPtr_ = isoft::naicpp::GlobalGeneralEvLoop::Get();
    if (nullptr == evLoopPtr_.get()) {
        LOG_ERROR << "event loop null.";
        return -1;
    }

    isoft::ara_fsh::Platform const fsh{};
    ara::core::String const phmContribute{fsh.GetPlatformEtcDir() + "/phm_contribute.json"};
    LOG_INFO << "phm contribute: " << phmContribute.c_str();
    if (access(const_cast< char* >(phmContribute.c_str()), F_OK) != 0) {
        LOG_WARN << phmContribute << " not exist, there may no PHM application";
        return 0;
    }

    configManager_ = ConfigManager::GetInstanceShared(phmContribute);
    if (configManager_ == nullptr) {
        LOG_ERROR << "configManager_ null.";
        return -1;
    }
    if (configManager_->Parse() < 0) {
        LOG_ERROR << "configManager_->Parse failed.";
        return -1;
    }

    watchdogConf_ = configManager_->GetWatchdogConf();
    LOG_INFO << "watchdog enable " << watchdogConf_.enable;
    if (watchdogConf_.enable) {
        if (_startWatchdog(watchdogConf_) < 0) {
            LOG_ERROR << "start watchdog failed.";
            return -1;
        }
    }

    supervisionController_ = SupervisionController::GetInstanceUnique();
    if (supervisionController_ == nullptr) {
        LOG_ERROR << "supervisionController_ null.";
        return -1;
    }
    if (supervisionController_->Init() < 0) {
        LOG_ERROR << "init supervisionController_ failed.";
        return -1;
    }
    // TODO(wangyanlong): std:bind is not recommended in QAC
    supervisionController_->SetWatchdogTriggerFunction(std::bind(&PhmContext::_triggerWatchdog, this));

    healthChannelController_ = HealthChannelController::GetInstanceUnique();
    if (healthChannelController_ == nullptr) {
        LOG_ERROR << "healthChannelController_ null.";
        return -1;
    }
    if (healthChannelController_->Init() < 0) {
        LOG_ERROR << "init healthChannelController_ failed.";
        return -1;
    }
    // TODO(wangyanlong): std:bind is not recommended in QAC
    healthChannelController_->SetWatchdogTriggerFunction(std::bind(&PhmContext::_triggerWatchdog, this));

    LOG_INFO << "PhmContext init end.";
    return 0;
}

/// @brief start watchdog.
/// @param watchdogConf
/// @return 0 success; < 0 failed.
int32_t PhmContext::_startWatchdog(WatchdogConf const& watchdogConf) noexcept
{
    LOG_INFO << "starting watchdog.";

    watchdogInterface_ = WatchdogInterface::GetInstanceUnique(watchdogConf.osWatchdog, watchdogConf.watchdogTimeoutMs);
    if (watchdogInterface_ == nullptr) {
        LOG_ERROR << "watchdogInterface_ null.";
        return -1;
    }

    LOG_INFO << "watchdogConf.watchdogTimeoutMs " << watchdogConf.watchdogTimeoutMs;
    int32_t const watchdogTimeoutDivider{20};
    feedWatchdogTimer_ = std::make_unique< ara::phm::internal::Timer >(
        "feed_watchdog", watchdogConf.watchdogTimeoutMs / watchdogTimeoutDivider,
        [this]() noexcept { this->watchdogInterface_->AliveNotification(); }, true);
    if (feedWatchdogTimer_->Start() < 0) {
        LOG_ERROR << "start feedWatchdogTimer_ failed.";
        return -1;
    }

    // Placed after starting the watchdog timer
    if (watchdogInterface_->Open() < 0) {
        LOG_ERROR << "open watchdog failed.";
        return -1;
    }
    return 0;
}

/// @brief Start PhmContext, the event loop run.
/// @return 0, success; < 0, fail
int32_t PhmContext::Run() noexcept
{
    LOG_INFO << "phm context run.";
    if (isoft::kSuccess != evLoopPtr_->Run(true)) {
        LOG_ERROR << "phm context run error.";
        return -1;
    }
    LOG_INFO << "phm context run return.";
    return 0;
}

/// @brief Stop PhmContext, stop the event loop.
/// @return 0, success; < 0, fail
int32_t PhmContext::Stop() const noexcept
{
    LOG_INFO << "stop phm context.";

    // Placed before stopping the watchdog timer
    if (watchdogConf_.enable) {
        if (watchdogInterface_) {
            LOG_INFO << "close watchdogInterface_.";
            std::ignore = watchdogInterface_->Close();
        }

        if (feedWatchdogTimer_) {
            LOG_INFO << "stop watchdog feed timer.";
            feedWatchdogTimer_->Stop();
        }
    }

    if (supervisionController_) {
        LOG_INFO << "stop supervisionController_.";
        std::ignore = supervisionController_->Stop();
    }

    if (healthChannelController_) {
        LOG_INFO << "stop healthChannelController_.";
        std::ignore = healthChannelController_->Stop();
    }

    LOG_INFO << "stopped phm context.";
    return 0;
}

/// @brief Destroy the resource of PhmContext.
/// @return 0, success;< 0, fail
int32_t PhmContext::Destroy() noexcept
{
    LOG_INFO << "destroy phm context.";
    healthChannelController_.reset();

    LOG_INFO << "destroy supervisionController_.";
    supervisionController_.reset();

    if (watchdogConf_.enable) {
        LOG_INFO << "destroy feedWatchdogTimer_.";
        feedWatchdogTimer_.reset();

        LOG_INFO << "destroy watchdogInterface_.";
        watchdogInterface_.reset();
    }

    LOG_INFO << "destroy configManager_.";
    configManager_.reset();

    if (evLoopPtr_) {
        LOG_INFO << "stop event loop.";
        evLoopPtr_->Stop();
        LOG_INFO << "event loop stopped.";
    }
    LOG_INFO << "destroied phm context.";
    return 0;
}

/// @brief Phmcontext set this function to SupervisionManager and
/// HealthChannelManager, SupervisionManager and HealthChannelManager call it
/// to trigger the watchdog.
void PhmContext::_triggerWatchdog() const
{
    LOG_INFO << "trigger watchdog.";

    if (!watchdogConf_.enable) {
        LOG_INFO << "watchdog is not enabled, so do not trigger it.";
        return;
    }

    if (feedWatchdogTimer_) {
        LOG_INFO << "stop watchdog timer.";
        feedWatchdogTimer_->Stop();
    }
    if (watchdogInterface_) {
        LOG_INFO << "fire watchdog.";
        watchdogInterface_->FireWatchdogReaction();
    }
}

}  // namespace internal
}  // namespace phm
}  // namespace ara
