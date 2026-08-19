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
/// @file       watchdog_interface.cpp
/// @brief
/// @details
/// @date       2024-06-20
/// @author     wangyanlong
/// @version    1.2.0
///
/// ================================================================

#include "ara/phm/internal/watchdog_interface.h"

#include "ara/phm/internal/linux_watchdog.h"
#include "ara/phm/internal/phm_log.h"
#include "ara/phm/internal/timer.h"

/// How does WatchdogInterface combine with the operating system's watchdog?
/// How to specify it dynamically?
/// Is WatchdogInterface a pure virtual function? Which specific implementation is it?
/// linux watchdog, android watchdog, should there also be a base class?
/// Is WatchdogInterface the base class, or should we define another base class?
namespace ara {
namespace phm {
namespace internal {
/// @brief Constructor.
/// @param watchdog dev of os watchdog
/// @param fire_watchdogTimeoutMs watchdog timeout
WatchdogInterface::WatchdogInterface(ara::core::String watchdog, int32_t const kFireWatchdogTimeoutMs) noexcept
    : kWatchdog{std::move(watchdog)}
    , kFire_watchdogTimeoutMs{kFireWatchdogTimeoutMs}
    , opened_{false}
    , osWatchdog_{nullptr}
    , timeoutTimer_{nullptr}
    , triggered_{false}
    , lastFeedWatchdogTimeStampMs_{ara::phm::internal::TimeStamp::GetMs()}

{
    LOG_INFO << " watchdog " << kWatchdog << ", timeout " << kFire_watchdogTimeoutMs;
    opened_       = false;
    osWatchdog_   = nullptr;
    timeoutTimer_ = nullptr;
    triggered_    = false;
    osWatchdog_   = std::make_unique< LinuxWatchdog >(kWatchdog);
    if (osWatchdog_ == nullptr) {
        LOG_FATAL << "make linux watchdog";
        std::terminate();
    }

    timeoutTimer_ = std::make_unique< ara::phm::internal::Timer >(
        // TODO(wangyanlong): std:bind is not recommended in QAC
        "watchdog-timer", kFire_watchdogTimeoutMs, std::bind(&WatchdogInterface::_watchdogTimeout, this), true);
    if (timeoutTimer_ == nullptr) {
        LOG_FATAL << "make watchdog timer.";
        std::terminate();
    }
}

/// @brief To feed watchdog.
void WatchdogInterface::AliveNotification() noexcept
{
    lastFeedWatchdogTimeStampMs_ = ara::phm::internal::TimeStamp::GetMs();
    if (!opened_) {
        LOG_WARN << "watchdog is not opened when AliveNotification.";
        return;
    }

    std::ignore = osWatchdog_->Feed();
}

/// @brief Trigger watchdog.
void WatchdogInterface::FireWatchdogReaction() noexcept
{
    if (!opened_) {
        LOG_WARN << "watchdog is not opened when FireWatchdogReaction.";
        return;
    }
    _triggerOsWatchdog();
}

/// @brief After Open called, watchdog start work.
/// @return 0 success; < 0, failed.
int32_t WatchdogInterface::Open() noexcept
{
    LOG_INFO << "open watchdog.";
    std::ignore = timeoutTimer_->Start();
    if (osWatchdog_->Open() < 0) {
        LOG_ERROR << "open hard watchdog failed";
        return -1;
    }

    int32_t const divider{1000};
    if (osWatchdog_->SetTimeout(kFire_watchdogTimeoutMs / divider) < 0) {
        LOG_ERROR << "set hard watchdog timeout failed, timeout " << kFire_watchdogTimeoutMs / divider;
        return -1;
    }
    opened_ = true;
    return 0;
}

/// @brief After Close called, watchdog stop work.
/// @return 0 success; < 0, failed.
int32_t WatchdogInterface::Close() noexcept
{
    LOG_INFO << "close watchdog.";
    if (osWatchdog_->Close() < 0) {
        LOG_ERROR << "close watchdog failed";
    }
    timeoutTimer_->Stop();
    opened_ = false;
    return 0;
}

/// @brief Function called when watchdog timeout.
/// @throws QAC
void WatchdogInterface::_watchdogTimeout()
{
    if (lastFeedWatchdogTimeStampMs_ + kFire_watchdogTimeoutMs >= ara::phm::internal::TimeStamp::GetMs()) {
        return;
    }
    LOG_INFO << "timeout " << kFire_watchdogTimeoutMs << ", last feed " << lastFeedWatchdogTimeStampMs_ << ", now "
             << ara::phm::internal::TimeStamp::GetMs();

    LOG_INFO << "watchdog timeout.";
    // Operate the hardware watchdog
    _triggerOsWatchdog();
}

/// @brief Trigger os watchdog.
void WatchdogInterface::_triggerOsWatchdog() noexcept
{
    LOG_INFO << "trigger os watchdog.";
    osWatchdog_->Trigger();
    triggered_ = true;
}

}  // namespace internal
}  // namespace phm
}  // namespace ara