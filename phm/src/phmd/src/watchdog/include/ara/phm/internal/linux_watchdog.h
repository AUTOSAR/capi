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
/// @file       linux_watchdog.h
/// @brief      Linux watchdog.
/// @details
/// @date       2024-07-16
/// @author     wangyanlong
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/PlatformHealthManagement/Watchdog
/// @unit_description=Linux watchdog.
/// @trace_id_sr=SR_PHM_01030,SR_PHM_01031
/// @unit_name=LinuxWatchdog
/// @interface_level=unit
/// @endcode
///
/// ================================================================

#ifndef ARA_PHM_INTERNAL_LINUX_WATCHDOG_H_
#define ARA_PHM_INTERNAL_LINUX_WATCHDOG_H_

#include <ara/core/string.h>

#include "ara/phm/internal/os_watchdog.h"

namespace ara {
namespace phm {
namespace internal {

/// @brief Class to manage Linux watchdog.
/// @trace_id_sr=SR_PHM_01030,SR_PHM_01031
/// @needwork = ad
class LinuxWatchdog : public OsWatchdog
{
public:
    /// @brief Constructor.
    /// @param watchdog the watchdog dev.
    /// @trace_id_sr=SR_PHM_01030,SR_PHM_01031
    /// @needwork = ad
    explicit LinuxWatchdog(ara::core::String watchdog) noexcept;

    /// @brief Destructor.
    /// @trace_id_sr=SR_PHM_01030,SR_PHM_01031
    /// @needwork = ad
    ~LinuxWatchdog() override = default;

    /// @brief open watchdog.
    /// @return 0 success; < 0, failed.
    /// @trace_id_sr=SR_PHM_01030,SR_PHM_01031
    /// @needwork = ad
    int32_t Open() noexcept override;

    /// @brief set watchdog timeout.
    /// @param timeoutInSecond time out in second.
    /// @return 0, success; other, failed.
    /// @trace_id_sr=SR_PHM_01030,SR_PHM_01031
    /// @needwork = ad
    int32_t SetTimeout(int32_t const timeoutInSecond) noexcept override;

    /// @brief close watchdog.
    /// @return 0 success; < 0, failed.
    /// @trace_id_sr=SR_PHM_01030,SR_PHM_01031
    /// @needwork = ad
    int32_t Close() noexcept override;

    /// @brief feed watchdog.
    /// @return 0, success; other, failed.
    /// @trace_id_sr=SR_PHM_01030,SR_PHM_01031
    /// @needwork = ad
    int32_t Feed() noexcept override;

    /// @brief Trigger the watchdog.
    /// @trace_id_sr=SR_PHM_01030,SR_PHM_01031
    /// @needwork = ad
    void Trigger() noexcept override;

    /// @brief The copy constructor shall not be used.
    /// @param obj The object to be copied.
    /// @trace_id_sr=SR_PHM_01030,SR_PHM_01031
    /// @needwork = ad
    LinuxWatchdog(LinuxWatchdog& obj) = delete;

    /// @brief The copy assignment shall not be used.
    /// @param obj The object to be copied.
    /// @return The copied object.
    /// @trace_id_sr=SR_PHM_01030,SR_PHM_01031
    /// @needwork = ad
    LinuxWatchdog& operator=(LinuxWatchdog const& obj) = delete;

    /// @brief The move constructor shall not be used.
    /// @param obj The object to be moved.
    /// @trace_id_sr=SR_PHM_01030,SR_PHM_01031
    /// @needwork = ad
    LinuxWatchdog(LinuxWatchdog&& obj) = delete;

    /// @brief The move assignment shall not be used.
    /// @param obj The object to be moved.
    /// @return LogicalSupervision& The moved object.
    /// @trace_id_sr=SR_PHM_01030,SR_PHM_01031
    /// @needwork = ad
    LinuxWatchdog& operator=(LinuxWatchdog const&& obj) = delete;

private:
    /// @brief the watchdog dev.
    /// @trace_id_sr=SR_PHM_01030,SR_PHM_01031
    /// @needwork = dda
    ara::core::String const kWatchdog;

    /// @brief watchdog FD.
    /// @trace_id_sr=SR_PHM_01030,SR_PHM_01031
    /// @needwork = dda
    int32_t watchdogFd_;
};

}  // namespace internal
}  // namespace phm
}  // namespace ara

#endif  // ARA_PHM_INTERNAL_LINUX_WATCHDOG_H_
