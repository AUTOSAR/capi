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
/// @file       time.h
/// @brief      time utilities
/// @details
/// @date       2023-01-09
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// export_level=/TimeSync/Utils
/// module_path=/TimeSync/Utils
/// @endcode
///
/// ================================================================

#ifndef ARA_ISOFT_UTIL_TIME_H_
#define ARA_ISOFT_UTIL_TIME_H_

#include <ara/core/steady_clock.h>
#include <ara/core/string.h>

#include <cstdint>

#include "isoft/util/type_cast.h"

namespace isoft {
namespace util {

std::uint32_t const kRatioOfSecondAndNano{static_cast< std::uint32_t >(1e9)};
std::uint32_t const kRatioOfSecondAndMicro{static_cast< std::uint32_t >(1e3)};

/// @brief convert system time to steady time
/// @param sysNs - system time
/// @return steady time
inline std::chrono::nanoseconds TimeSystem2Steady(std::chrono::nanoseconds const &sysNs) noexcept
{
    std::chrono::nanoseconds offsetNs;
    offsetNs = std::chrono::system_clock::now().time_since_epoch() - ara::core::SteadyClock::now().time_since_epoch();
    std::chrono::nanoseconds ret;
    ret = sysNs - offsetNs;
    return ret;
}

/// @brief time conversion, seconds to nanoseconds
/// @param s - seconds
/// @return nanoseconds
inline double TimeS2NS(double const &s) noexcept
{
    double const ret{s * static_cast< double >(isoft::util::kRatioOfSecondAndNano)};
    return ret;
}

/// @brief time conversion, nanoseconds to seconds
/// @param ns - nanoseconds
/// @return seconds
inline double TimeNS2S(double const &ns) noexcept
{
    double const ret{ns / static_cast< double >(isoft::util::kRatioOfSecondAndNano)};
    return ret;
}

/// @brief time conversion, seconds to milliseconds
/// @param s - seconds
/// @return microseconds
inline std::uint32_t TimeS2MS(double const &s) noexcept
{
    double const ret{s * static_cast< double >(isoft::util::kRatioOfSecondAndMicro)};
    return static_cast< std::uint32_t >(ret);
}

/// @brief time conversion, milliseconds to seconds
/// @param ms - milliseconds
/// @return seconds
inline double TimeMS2S(double const &ms) noexcept
{
    double const ret{ms / static_cast< double >(isoft::util::kRatioOfSecondAndMicro)};
    return ret;
}

/// @brief time conversion, chrono time to timespec
/// @param ts - timespec time
/// @param ns - chrono time
inline void Chrono2TimeSpec(struct timespec *const ts, std::chrono::nanoseconds const &ns) noexcept
{
    ts->tv_sec  = std::chrono::duration_cast< std::chrono::seconds >(ns).count();
    ts->tv_nsec = ns.count() % static_cast< std::int32_t >(isoft::util::kRatioOfSecondAndNano);
}

/// @brief time conversion, chrono time to timespec
/// @param ts - timespec time
/// @return nanoseconds
inline std::int64_t TimeSpec2Int64(struct timespec const *const ts) noexcept
{
    return ts->tv_sec * static_cast< std::int64_t >(isoft::util::kRatioOfSecondAndNano) + ts->tv_nsec;
}

/// @brief time conversion, keep only the nanoseconds part of the time
/// @param ns - time value
/// @return nanoseconds part value
inline std::int32_t CutoutNanoSecond(std::chrono::nanoseconds const ns) noexcept
{
    std::chrono::seconds const seds{std::move(std::chrono::duration_cast< std::chrono::seconds >(ns))};
    std::chrono::nanoseconds const ns2{std::move(ns - seds)};
    return static_cast< std::int32_t >((ns2).count());
}

/// @brief time conversion, keep only the seconds part of the time
/// @param ns - time value
/// @return seconds part value
inline std::int32_t CutoutSecond(std::chrono::nanoseconds const ns) noexcept
{
    return static_cast< std::int32_t >(std::chrono::duration_cast< std::chrono::seconds >(ns).count());
}

/// @brief get current time value
/// @return current time in nanoseconds
inline std::uint64_t GetNSTimeofDay() noexcept
{
    return static_cast< std::uint64_t >(
        (std::chrono::duration_cast< std::chrono::nanoseconds >(std::chrono::steady_clock::now().time_since_epoch()))
            .count());
}
}  // namespace util
}  // namespace isoft

#endif  /// ARA_ISOFT_UTIL_TIME_H_