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
/// @file       linux_watchdog.cpp
/// @brief
/// @details
/// @date       2024-06-20
/// @author     wangyanlong
/// @version    1.2.0
///
/// ================================================================

#include "ara/phm/internal/linux_watchdog.h"

#include <fcntl.h>
#include <linux/watchdog.h>
#include <sys/ioctl.h>

#include <cerrno>

#include "ara/phm/internal/phm_log.h"

namespace ara {
namespace phm {
namespace internal {

/// @brief Constructor.
/// @param watchdog the watchdog dev.
LinuxWatchdog::LinuxWatchdog(ara::core::String watchdog) noexcept
    : OsWatchdog{}, kWatchdog{std::move(watchdog)}, watchdogFd_{-1}
{
    LOG_INFO << "linux watchdog:" << kWatchdog.c_str();
}

/// @brief Trigger the watchdog.
void LinuxWatchdog::Trigger() noexcept
{
    LOG_INFO << "trigger linux watchdog.";
    int32_t timeoutS{0};
    if (ioctl(watchdogFd_, static_cast< uint64_t >(WDIOC_GETTIMEOUT), &timeoutS) != 0) {
        /// TODO(wangyanlong): clang-tidy suggests that the std::strerror function may throw an exception, QAC does not recommend using errno
        LOG_ERROR << "get watchdog timeout value failed: " << std::strerror(errno);  // NOLINT
        return;
    }
    LOG_INFO << "default watchdog timeout is " << timeoutS;

    timeoutS = 1;
    // Set the timeout to 1 second, and do not feed the watchdog, then the system will restart after 1 second
    if (ioctl(watchdogFd_, static_cast< uint64_t >(WDIOC_SETTIMEOUT), &timeoutS) != 0) {
        /// TODO(wangyanlong): clang-tidy suggests that the std::strerror function may throw an exception, QAC does not recommend using errno
        LOG_ERROR << "set watchdog timeout value failed: " << std::strerror(errno);  // NOLINT
        return;
    }
}

/// @brief open watchdog.
/// @return 0 success; < 0, failed.
int32_t LinuxWatchdog::Open() noexcept
{
    watchdogFd_ = std::move(open(kWatchdog.c_str(), O_RDWR | O_CLOEXEC));
    if (watchdogFd_ < 0) {
        LOG_ERROR << "open watchdog " << kWatchdog << " failed!";
        return -1;
    }
    return 0;
}

/// @brief set watchdog timeout.
/// @param timeoutInSecond time out in second.
/// @return 0, success; other, failed.
int32_t LinuxWatchdog::SetTimeout(int32_t const timeoutInSecond) noexcept
{
    LOG_INFO << "set hard watchdog timeout to " << timeoutInSecond << " second.";
    if (watchdogFd_ < 0) {
        LOG_ERROR << "watchdog " << kWatchdog << " is not opened.";
        return -1;
    }

    int32_t oldTimeout{0};
    if (ioctl(watchdogFd_, static_cast< uint64_t >(WDIOC_GETTIMEOUT), &oldTimeout) != 0) {
        /// TODO(wangyanlong): clang-tidy suggests that the std::strerror function may throw an exception, QAC does not recommend using errno
        LOG_ERROR << "get watchdog timeout value failed:" << std::strerror(errno);  // NOLINT
    }
    LOG_INFO << "OLD watchdog timeout is " << oldTimeout;

    if (ioctl(watchdogFd_, static_cast< uint64_t >(WDIOC_SETTIMEOUT), &timeoutInSecond) != 0) {
        /// TODO(wangyanlong): clang-tidy suggests that the std::strerror function may throw an exception, QAC does not recommend using errno
        LOG_ERROR << "set watchdog timeout value failed:" << std::strerror(errno);  // NOLINT
        return -1;
    }
    return 0;
}

/// @brief close watchdog.
/// @return 0 success; < 0, failed.
int32_t LinuxWatchdog::Close() noexcept
{
    LOG_INFO << "close hard watchdog.";
    if (watchdogFd_ < 0) {
        LOG_ERROR << "watchdog " << kWatchdog << " is not opened.";
        return -1;
    }

    int32_t const option{WDIOS_DISABLECARD};
    if (ioctl(watchdogFd_, static_cast< uint64_t >(WDIOC_SETOPTIONS), &option) != 0) {
        /// TODO(wangyanlong): clang-tidy suggests that the std::strerror function may throw an exception, QAC does not recommend using errno
        LOG_ERROR << "disable watchdog error:" << std::strerror(errno);  // NOLINT
    }
    std::ignore = close(watchdogFd_);
    watchdogFd_ = -1;
    return 0;
}

/// @brief feed watchdog.
/// @return 0, success; other, failed.
int32_t LinuxWatchdog::Feed() noexcept
{
    if (watchdogFd_ < 0) {
        LOG_ERROR << "watchdog " << kWatchdog << " is not opened.";
        return -1;
    }

    if (ioctl(watchdogFd_, static_cast< uint64_t >(WDIOC_KEEPALIVE), nullptr) != 0) {
        /// TODO(wangyanlong): clang-tidy suggests that the std::strerror function may throw an exception, QAC does not recommend using errno
        LOG_ERROR << "feed watchdog error:" << std::strerror(errno);  // NOLINT
        return -1;
    }
    return 0;
}

}  // namespace internal
}  // namespace phm
}  // namespace ara
