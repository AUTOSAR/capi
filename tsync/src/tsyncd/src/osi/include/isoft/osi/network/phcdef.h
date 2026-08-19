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
/// @file       phcdef.h
/// @brief      OSI network module raw socket class
/// @details
/// @date       2023-01-09
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================

#ifndef ISOFT_OSI_NETWORK_PHCDEF_H_
#define ISOFT_OSI_NETWORK_PHCDEF_H_

#include <cstdint>
#include <ctime>

namespace isoft {
namespace osi {
namespace network {
/// @brief kUTC_TAI_OFFSET_NS  The difference between utc and tai will change, currently 37s, may change
static constexpr std::int64_t kUTC_TAI_OFFSET_NS{37000000000};

/// @brief kTS_NSPERSEC  Multiplier between nanoseconds and seconds, used for hardware clock calculation
static constexpr std::int64_t kTS_NSPERSEC{1000000000};
/// @brief kBITS_PER_LONG
static constexpr std::size_t kBITS_PER_LONG{(sizeof(int64_t) * 8U)};
/// @brief kBITS_OF_CPU32
static constexpr std::size_t kBITS_OF_CPU32{32U};
/// @brief kMAX_PPB_32
static constexpr std::int32_t kMAX_PPB_32{32767999};  ///* 2^31 - 1 / 65.536 */
/// @brief kCHECK_MIN_INTERVAL
static constexpr std::int64_t kCHECK_MIN_INTERVAL{1000000000};
/// @brief kCHECK_MAX_FREQ
static constexpr std::int32_t kCHECK_MAX_FREQ{900000000};
/// @brief kHWTS_KP_SCALE
static constexpr double kHWTS_KP_SCALE{0.700000};
/// @brief kHWTS_KI_SCALE
static constexpr double kHWTS_KI_SCALE{0.300000};
/// @brief kSWTS_KP_SCALE
static constexpr double kSWTS_KP_SCALE{0.1};
/// @brief kSWTS_KI_SCALE
static constexpr double kSWTS_KI_SCALE{0.001};
/// @brief kMAX_KP_NORM_MAX
static constexpr double kMAX_KP_NORM_MAX{1.0};
/// @brief kMAX_KI_NORM_MAX
static constexpr double kMAX_KI_NORM_MAX{2.0};
/// @brief kFREQ_EST_MARGIN
static constexpr double kFREQ_EST_MARGIN{0.001};
/// @brief kHWTS_KP_KI_EXPONENT
static constexpr double kHWTS_KP_KI_EXPONENT{0.400000};
/// @brief kHWTS_KP_KI_NORMAL_MAX
static constexpr double kHWTS_KP_KI_NORMAL_MAX{0.300000};
/// @brief kCLOCK_INVALID
static constexpr clockid_t kCLOCK_INVALID{-1};
/// @brief kCLOCKCHECK_FREQUENCY_LIMIT
static constexpr std::int32_t kCLOCKCHECK_FREQUENCY_LIMIT{200000000};
/// @brief kPHC_SYNC_INTERNVAL
static constexpr double kPHC_SYNC_INTERNVAL{0.3};
/// @brief kPHC_SYNC_OFFSET
static constexpr double kPHC_SYNC_OFFSET{0.2};
/// @brief kPHC_PI_INTERNVAL
static constexpr double kPHC_PI_INTERNVAL{1.0};
/// @brief kPHC_SAMPLE_COUNT
static constexpr std::uint32_t kPHC_SAMPLE_COUNT{10U};

/// @brief kPHC_ADJUST_SKIP Number of samples to skip when calculating average, to eliminate burrs and jitter
static constexpr std::int64_t kPHC_ADJUST_SKIP{5};

/// @brief kPHC_AVERAGE_COUNT Number of samples for average calculation
static constexpr std::int64_t kPHC_AVERAGE_COUNT{15};

/// @brief kPHC_OFFSET_THRESHOLD Tolerance threshold for jitter, to eliminate burrs and jitter
static constexpr std::int64_t kPHC_OFFSET_THRESHOLD{10000};

/// @brief kDefNumOffsetValues Tolerance threshold for jitter, to eliminate burrs and jitter
static constexpr std::int32_t kDefNumOffsetValues{10};

/// @brief kDelayScaled Exclude delay that is too large or too small when calculating offset
static constexpr std::int64_t kDelayScaled{2};

/// @brief kOffsetScaled Exclude offset that is too large when calculating offset
static constexpr std::int64_t kOffsetScaled{3};

/// @brief kFreqScaled Unit scale
static constexpr double kFreqScaled{65.536};

/// @brief kCLOCKFD
static constexpr std::size_t kCLOCKFD{3U};
/// @brief kMOVE_BITS_COUNTS
static constexpr std::size_t kMOVE_BITS_COUNTS{3U};

/// @brief kMaxOffsetSeconds  Maximum deviation between system clock and network card clock. If the user modifies the network card clock midway, reset to recover as soon as possible
static constexpr std::int64_t kMaxOffsetSeconds{10};

/// @brief kFreqScaled Unit scale
static constexpr std::int64_t kTickFreHzParameter{1000000};

/// @brief kFreqScaled Unit scale
static constexpr std::int32_t kDefaultSysMaxadj{500000};

/// @brief kRealtimeHzScaled Unit scale
static constexpr std::int64_t kRealtimeHzScaled{1000};

/// @brief kRealtimeHzParameter Unit scale
static constexpr std::int64_t kRealtimeHzParameter{10};
/// @brief convert fd to clock id
/// @param  fd network device name
/// @return clock id
inline clockid_t FD_TO_CLOCKID(std::int32_t const fd) noexcept
{
    /// TODO(zhoubo): type conversion may lose precision
    return (((static_cast< std::uint32_t >(~fd)) << kMOVE_BITS_COUNTS) | kCLOCKFD);  //NOLINT
}

/// @brief convert clock id to fd
/// @param clk clock id
/// @return fd
inline std::int32_t CLOCKID_TO_FD(clockid_t const clk) noexcept { return ~((clk) >> kMOVE_BITS_COUNTS); }

}  // namespace network
}  // namespace osi
}  // namespace isoft
#endif  /// ISOFT_OSI_NETWORK_PHCDEF_H_