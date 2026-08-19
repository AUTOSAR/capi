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
/// @file       sysclock.h
/// @brief      system clock base class
/// @details
/// @date       2023-01-09
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================

#ifndef ISOFT_OSI_NETWORK_SYSCLOCK_H_
#define ISOFT_OSI_NETWORK_SYSCLOCK_H_
#include <linux/ptp_clock.h>

#include <chrono>
#include <cstdint>
#include <functional>
#include <memory>
#include <string>

#include "ara/tsync/internal/common.h"
#include "isoft/osi/network/phcdef.h"
#include "isoft/osi/network/sysclockadj.h"
namespace isoft {
namespace osi {
namespace network {

/// @brief system clock adjustment class
class SysClock final
{
public:
    /// @brief constructor
    SysClock() = default;

    /// @brief destructor
    ~SysClock() = default;

    /// @brief copy constructor is prohibited
    /// @param other - other object
    SysClock(SysClock const &other) = delete;

    /// @brief copy is prohibited
    /// @param other - other object
    /// @return reference to class object
    SysClock &operator=(SysClock const &other) = delete;

    /// @brief move constructor
    /// @param other an existing rvalue reference.
    SysClock(SysClock &&) noexcept = default;

    /// @brief '=' operator overload
    /// @param other an existing rvalue reference.
    /// @return reference to class object
    SysClock &operator=(SysClock &&) &noexcept = default;

    /// @brief open system clock
    /// @return <0 indicates failure.
    std::int32_t Open() noexcept;

    /// @brief adjust system clock
    /// @param offset - deviation value
    /// @param ts - clock value
    /// @return <0 indicates failure.
    std::int32_t AdjustClock(int64_t offset, int64_t ts) noexcept;

    /// @brief set clock
    /// @param value  - nano-second
    /// @return <0 indicates failure.
    std::int32_t InitSysClockValue(int64_t value) const noexcept;

private:
    /// @brief query the maximum frequency adjustment of the PTP hardware clock.
    /// @param clockId clock id.
    /// @return maximum frequency adjustment, 0 on failure
    std::int32_t _getSysMaxadj(clockid_t const clockId) const noexcept;

private:
    /// @name clkid_ - system clock handle
    clockid_t clkid_{-1};

    /// @name adjObj_ system clock adjustment object
    std::shared_ptr< SysClockAdj > adjObj_{nullptr};

    /// @name leapBit_
    std::int32_t leapBit_{0};

    /// @name realtimeHz_
    std::int64_t realtimeHz_{0};

    /// @name realtimeNominalTick_
    std::int64_t realtimeNominalTick_{0};
};  /// class SysClock

}  // namespace network
}  // namespace osi
}  // namespace isoft

#endif  /// ISOFT_OSI_NETWORK_SYSCLOCK_H_
