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
/// @file       phcclock.h
/// @brief      PHC clock base class
/// @details
/// @date       2023-01-09
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================

#ifndef ISOFT_OSI_NETWORK_PHCCLOCK_H_
#define ISOFT_OSI_NETWORK_PHCCLOCK_H_
#include <linux/ptp_clock.h>

#include <chrono>
#include <cstdint>
#include <functional>
#include <memory>
#include <string>

#include "ara/tsync/internal/common.h"
#include "isoft/osi/network/phcclockadj.h"
#include "isoft/osi/network/phcdef.h"
namespace isoft {
namespace osi {
namespace network {

/// @brief PHC clock base class
class PhcClock final
{
public:
    /// @brief constructor
    PhcClock() = default;

    /// @brief destructor
    ~PhcClock() noexcept;

    /// @brief copy constructor is prohibited
    /// @param other - other object
    PhcClock(PhcClock const &other) = delete;

    /// @brief copy is prohibited
    /// @param other - other object
    /// @return reference to class object
    PhcClock &operator=(PhcClock const &other) = delete;

    /// @brief move constructor
    /// @param other an existing rvalue reference.
    PhcClock(PhcClock &&) noexcept = default;

    /// @brief '=' operator overload
    /// @param other an existing rvalue reference.
    /// @return reference to class object
    PhcClock &operator=(PhcClock &&) &noexcept = default;

    /// @brief open phc clock
    /// @param deviceName - network device name
    /// @return <0 indicates failure.
    std::int32_t Open(std::string const &deviceName) noexcept;

    /// @brief adjust phc clock
    /// @param offset - deviation value
    /// @param ts - clock value
    /// @return <0 indicates failure.
    std::int32_t AdjustClock(int64_t offset, int64_t ts) noexcept;

    /// @brief measure the deviation between phc clock and system clock
    /// @param offset - deviation value
    /// @param delay - delay
    /// @param ts - phc clock
    /// @return <0 indicates failure.
    std::int32_t GetClockSysOffset(int64_t &offset, int64_t &delay, int64_t &ts) noexcept;

    /// @brief set clock
    /// @param value  - nano-second
    /// @return <0 indicates failure.
    std::int32_t InitPhcClockValue(int64_t value) noexcept;

private:
    /// @brief get ptp clock index of the specified network card
    /// @param name network device name
    /// @return ptp clock index <0 - failure
    static std::int32_t GetPhcIndex(ara::tsync::internal::char8_t const *const name) noexcept;

    /// @brief open PTP hardware clock
    /// @param phcName PTP device name
    /// @return legal clock ID on success, CLOCK_INVALID on failure.
    clockid_t _openPhcClock(ara::tsync::internal::char8_t const *const phcName) const noexcept;

    /// @brief close PTP hardware clock
    /// @param clockId clock id.
    static void ClosePhcClock(clockid_t const clockId) noexcept;

    /// @brief get PTP hardware clock capabilities
    /// @param clockId clock id.
    /// @param caps PTP hardware clock capabilities.
    /// @return 0 - success other - failure
    std::int32_t _getPhcCaps(clockid_t const clockId, struct ptp_clock_caps *const caps) const noexcept;

    /// @brief query the maximum frequency adjustment of the PTP hardware clock.
    /// @param clockId clock id.
    /// @return maximum frequency adjustment, 0 on failure
    std::int32_t _getPhcMaxadj(clockid_t const clockId) const noexcept;

    /// @brief close phc clock
    /// @return 0 - success <0 - failure
    void _close() noexcept;

private:
    /// @name ifName_ - network device name
    std::string ifName_;

    /// @name ptpName_ - ptp clock name
    std::string ptpName_;

    /// @name clkid_ - ptp clock handle
    clockid_t clkid_{-1};

    /// @name adjObj_ phc clock adjustment object
    std::shared_ptr< PhcClockAdj > adjObj_{nullptr};
};  /// class PhcClock

}  // namespace network
}  // namespace osi
}  // namespace isoft

#endif  /// ISOFT_OSI_NETWORK_PHCCLOCK_H_
