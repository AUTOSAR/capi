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
/// @file       watchdog_interface.h
/// @brief      PHM can reboot os by watchdog interface when.
/// @details
/// @date       2024-06-05
/// @author     wangyanlong
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/PlatformHealthManagement/Watchdog
/// @unit_description=PHM can reboot os by watchdog interface when.
/// @trace_id_sr=SR_PHM_01030,SR_PHM_01031
/// @unit_name=WatchdogInterface
/// @interface_level=module
/// @endcode
///
/// ================================================================

#ifndef ARA_PHM_INTERNAL_WATCHDOG_INTERFACE_H_
#define ARA_PHM_INTERNAL_WATCHDOG_INTERFACE_H_

#include <ara/core/string_view.h>

#include <memory>
#include <mutex>

#include "ara/phm/internal/os_watchdog.h"
#include "ara/phm/internal/timer.h"
#include "ara/phm/internal/types.h"

namespace ara {
namespace phm {
namespace internal {

/// @brief PHM can reboot os by watchdog when some condition can not recover via SM.
/// @trace_id_sr=SR_PHM_01030,SR_PHM_01031
/// @needwork = ad
class WatchdogInterface
{
public:
    /// @brief Creation of WatchdogInterface, unique singleton.
    /// @param watchdog os watchdog.
    /// @param fire_watchdogTimeoutMs watchdog timeout.
    /// @return std::unique_ptr<WatchdogInterface> unique ptr of WatchdogInterface.
    /// @trace_id_sr=SR_PHM_01030,SR_PHM_01031
    /// @needwork = ad
    static std::unique_ptr< WatchdogInterface > GetInstanceUnique(ara::core::String const &watchdog,
                                                                  int32_t const kFireWatchdogTimeoutMs)
    {
        static std::unique_ptr< WatchdogInterface > s_Instance{nullptr};
        static std::once_flag s_Flag{};
        std::call_once(s_Flag, [&watchdog, &kFireWatchdogTimeoutMs]() {
            s_Instance.reset(new WatchdogInterface(watchdog, kFireWatchdogTimeoutMs));
        });
        std::ignore = s_Flag;

        /// Exclusive singleton, only allowed to be obtained once
        /// std::call_once() will be called only once,
        /// So when GetInstanceUnique() is called the second time, std::call_once() will not be called,
        /// So the instance is null, assert
        assert(s_Instance);
        return std::move(s_Instance);
    }

    /// @brief The copy constructor shall not be used.
    /// @param obj The object to be copied.
    /// @trace_id_sr=SR_PHM_01030,SR_PHM_01031
    /// @needwork = ad
    WatchdogInterface(WatchdogInterface &obj) = delete;

    /// @brief The copy assignment shall not be used.
    /// @param obj The object to be copied.
    /// @return The copied object.
    /// @trace_id_sr=SR_PHM_01030,SR_PHM_01031
    /// @needwork = ad
    WatchdogInterface &operator=(WatchdogInterface &obj) = delete;

    /// @brief The move constructor shall not be used.
    /// @param obj The object to be moved.
    /// @trace_id_sr=SR_PHM_01030,SR_PHM_01031
    /// @needwork = ad
    WatchdogInterface(WatchdogInterface &&obj) = delete;

    /// @brief The move assignment shall not be used.
    /// @param obj The object to be moved.
    /// @return The moved object.
    /// @trace_id_sr=SR_PHM_01030,SR_PHM_01031
    /// @needwork = ad
    WatchdogInterface &operator=(WatchdogInterface &&obj) = delete;

    /// @brief Destructor.
    /// @throws QAC
    /// @trace_id_sr=SR_PHM_01030,SR_PHM_01031
    /// @needwork = ad
    ~WatchdogInterface() = default;

    /// @brief To feed watchdog.
    /// @trace_id_sr=SR_PHM_01030,SR_PHM_01031
    /// @needwork = ad
    void AliveNotification() noexcept;

    /// @brief Trigger watchdog.
    /// @trace_id_sr=SR_PHM_01030,SR_PHM_01031
    /// @needwork = ad
    void FireWatchdogReaction() noexcept;

    /// @brief After Open called, watchdog start work.
    /// @return 0 success; < 0, failed.
    /// @trace_id_sr=SR_PHM_01030,SR_PHM_01031
    /// @needwork = ad
    int32_t Open() noexcept;

    /// @brief After Close called, watchdog stop work.
    /// @return 0 success; < 0, failed.
    /// @trace_id_sr=SR_PHM_01030,SR_PHM_01031
    /// @needwork = ad
    int32_t Close() noexcept;

private:
    /// @brief Constructor.
    /// @param watchdog dev of os watchdog
    /// @param fire_watchdogTimeoutMs watchdog timeout
    /// @trace_id_sr=SR_PHM_01030,SR_PHM_01031
    /// @needwork = dda
    WatchdogInterface(ara::core::String watchdog, int32_t const kFireWatchdogTimeoutMs) noexcept;

    /// @brief Function called when watchdog timeout.
    /// @throws QAC
    /// @trace_id_sr=SR_PHM_01030,SR_PHM_01031
    /// @needwork = dda
    void _watchdogTimeout();

    /// @brief Trigger os watchdog.
    /// @trace_id_sr=SR_PHM_01030,SR_PHM_01031
    /// @needwork = dda
    void _triggerOsWatchdog() noexcept;

private:
    /// @brief watchddog.
    /// @trace_id_sr=SR_PHM_01030,SR_PHM_01031
    /// @needwork = dda
    ara::core::String const kWatchdog;

    /// @brief Timeout of watchdog.
    /// @trace_id_sr=SR_PHM_01030,SR_PHM_01031
    /// @needwork = dda
    int32_t const kFire_watchdogTimeoutMs;

    /// @brief Whether watchdog is opened.
    /// @trace_id_sr=SR_PHM_01030,SR_PHM_01031
    /// @needwork = dda
    bool opened_;

    /// @brief Os watchdog.
    /// @trace_id_sr=SR_PHM_01030,SR_PHM_01031
    /// @needwork = dda
    std::unique_ptr< OsWatchdog > osWatchdog_;

    /// @brief Timer to trigger hardware watchdog.
    /// @trace_id_sr=SR_PHM_01030,SR_PHM_01031
    /// @needwork = dda
    std::unique_ptr< ara::phm::internal::Timer > timeoutTimer_;

    /// @brief Whether watchdog is triggered.
    /// @trace_id_sr=SR_PHM_01030,SR_PHM_01031
    /// @needwork = dda
    bool triggered_;

    /// @brief time stamp last feed watchdog.
    /// @trace_id_sr=SR_PHM_01030,SR_PHM_01031
    /// @needwork = dda
    int64_t lastFeedWatchdogTimeStampMs_;
};

}  // namespace internal
}  // namespace phm
}  // namespace ara

#endif  // ARA_PHM_INTERNAL_WATCHDOG_INTERFACE_H_
