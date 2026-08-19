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
/// @file       os_watchdog.h
/// @brief      Interface of os watchdog.
/// @details
/// @date       2024-07-16
/// @author     wangyanlong
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/PlatformHealthManagement/Watchdog
/// @unit_description=Interface of os watchdog.
/// @trace_id_sr=SR_PHM_01030,SR_PHM_01031
/// @unit_name=OsWatchdog
/// @interface_level=unit
/// @endcode
///
/// ================================================================

#ifndef ARA_PHM_INTERNAL_OS_WATCHDOG_H_
#define ARA_PHM_INTERNAL_OS_WATCHDOG_H_

#include <ara/phm/internal/timer.h>
#include <isoft/naicpp/evloop.h>

#include <cassert>
#include <memory>
#include <mutex>

namespace ara {
namespace phm {
namespace internal {

/// @brief Interface of os watchdog.
/// @trace_id_sr=SR_PHM_01030,SR_PHM_01031
/// @needwork = ad
class OsWatchdog
{
public:
    /// @brief Constructor.
    /// @trace_id_sr=SR_PHM_01030,SR_PHM_01031
    /// @needwork = ad
    OsWatchdog() = default;

    /// @brief Destructor.
    /// @trace_id_sr=SR_PHM_01030,SR_PHM_01031
    /// @needwork = ad
    virtual ~OsWatchdog() = default;

    /// @brief open watchdog.
    /// @return 0 success; < 0, failed.
    /// @trace_id_sr=SR_PHM_01030,SR_PHM_01031
    /// @needwork = ad
    virtual int32_t Open() noexcept = 0;

    /// @brief close watchdog.
    /// @return 0 success; < 0, failed.
    /// @trace_id_sr=SR_PHM_01030,SR_PHM_01031
    /// @needwork = ad
    virtual int32_t Close() noexcept = 0;

    /// @brief set watchdog timeout.
    /// @param timeoutInSecond time out in second.
    /// @return 0, success; other, failed.
    /// @trace_id_sr=SR_PHM_01030,SR_PHM_01031
    /// @needwork = ad
    virtual int32_t SetTimeout(int32_t const timeoutInSecond) noexcept = 0;

    /// @brief feed watchdog.
    /// @return 0, success; other, failed.
    /// @trace_id_sr=SR_PHM_01030,SR_PHM_01031
    /// @needwork = ad
    virtual int32_t Feed() noexcept = 0;

    /// @brief To trigger the os watchdog.
    /// @trace_id_sr=SR_PHM_01030,SR_PHM_01031
    /// @needwork = ad
    virtual void Trigger() noexcept = 0;

    /// @brief The copy constructor shall not be used.
    /// @param obj The object to be copied.
    /// @trace_id_sr=SR_PHM_01030,SR_PHM_01031
    /// @needwork = ad
    OsWatchdog(OsWatchdog& obj) = delete;

    /// @brief The copy assignment shall not be used.
    /// @param obj The object to be copied.
    /// @return The copied object.
    /// @trace_id_sr=SR_PHM_01030,SR_PHM_01031
    /// @needwork = ad
    OsWatchdog& operator=(OsWatchdog const& obj) = delete;

    /// @brief The move constructor shall not be used.
    /// @param obj The object to be moved.
    /// @trace_id_sr=SR_PHM_01030,SR_PHM_01031
    /// @needwork = ad
    OsWatchdog(OsWatchdog&& obj) = delete;

    /// @brief The move assignment shall not be used.
    /// @param obj The object to be moved.
    /// @return LogicalSupervision& The moved object.
    /// @trace_id_sr=SR_PHM_01030,SR_PHM_01031
    /// @needwork = ad
    OsWatchdog& operator=(OsWatchdog const&& obj) = delete;
};

}  // namespace internal
}  // namespace phm
}  // namespace ara

#endif  // ARA_PHM_INTERNAL_OS_WATCHDOG_H_
