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
/// @file       common.h
/// @brief      common variable declarations
/// @details
/// @date       2023-02-09
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_TSYNC_INTERNAL_COMMON_H_
#define ARA_TSYNC_INTERNAL_COMMON_H_
#include <ara/core/string.h>

#include <cerrno>
#include <cstdint>
#include <cstring>
namespace ara {
namespace tsync {
namespace internal {
static constexpr std::int32_t kRET_OK{0};
static constexpr std::int32_t kRET_E1{-1};
static constexpr std::int32_t kRET_E2{-2};
static constexpr std::int32_t kRET_E3{-3};
static constexpr std::int32_t kRET_E4{-4};
static constexpr std::int32_t kRET_E5{-5};
static constexpr std::int32_t kRET_E6{-6};
static constexpr std::int32_t kRET_E7{-7};
static constexpr std::int32_t kRET_E8{-8};

static constexpr std::int32_t kTS_NUM_INT_2{2};
static constexpr std::int32_t kTS_NUM_INT_3{3};
static constexpr std::int32_t kTS_NUM_INT_6{6};
static constexpr std::int32_t kTS_NUM_INT_9{9};
static constexpr std::size_t kTS_NUM_1{1U};
static constexpr std::size_t kTS_NUM_2{2U};
static constexpr std::size_t kTS_NUM_3{3U};
static constexpr std::size_t kTS_NUM_4{4U};
static constexpr std::size_t kTS_NUM_5{5U};
static constexpr std::size_t kTS_NUM_6{6U};
static constexpr std::size_t kTS_NUM_7{7U};
static constexpr std::size_t kTS_NUM_8{8U};
static constexpr std::size_t kTS_NUM_12{12U};
static constexpr std::size_t kTS_NUM_16{16U};
static constexpr uint32_t kInt_512U{512U};

static constexpr std::uint16_t kTS_NUM_10{10U};
static constexpr std::uint8_t kTS_NUM_63{0x3f};
static constexpr double kTS_NUM_0_POINT_001{1e-3};
static constexpr double kTS_NUM_DOUBLE_2{2.0};
static constexpr double kTS_NUM_DOUBLE_3{3.0};
static constexpr double kTS_NUM_DOUBLE_5{5.0};
/// @brief type declaration
using char8_t = char;  // NOLINT
static constexpr char8_t kTS_FILL_0{'0'};

/// @brief GptpType
enum class GptpType : std::uint8_t
{
    // Event messages, the sending of messages requires recording the exact time
    kSync       = 0x00,
    kDelayReq   = 0x01,
    kPdelayReq  = 0x02,
    kPdelayResp = 0x03,
    // 0x04-7 reserved
    // Ordinary messages, no need to record exact time when sending
    kFollowup           = 0x08,
    kDelayResp          = 0x09,
    kPdelayRespFollowup = 0xA,
    kAnnounce           = 0x0B,
    kSignaling          = 0x0C,
    kManagement         = 0x0D,
    // 0x0E-F reserved

    // Custom, used for error judgment
    kError = 0xFF,
};

/// @brief return errno, reduce qac
/// @return errno
inline std::int32_t GetErrNo() noexcept { return errno; }
/// @brief return errno, reduce qac
/// @return errno
inline ara::core::String GetErrString() noexcept
{
    char8_t buff[kInt_512U]{0};

    // Call the GNU extended version of strerror_r
    char8_t *ret{strerror_r(errno, buff, sizeof(buff))};

    // If fails (nullptr) or returns an empty string, generate fallback information
    if (ret == nullptr || ret[0] == '\0') {
        snprintf(buff, sizeof(buff), "safe_strerror: Unknown error (errno=%d)", errno);
    }

    // Directly construct String via buff
    return ara::core::String(ret);
}
/// @brief clock adjustment type
enum ClockAdjust : std::uint8_t
{
    kAdjustPhc       = 0x00,
    kAdjustPhcSystem = 0x01,
};
/// @brief storage type
enum StorageType : std::uint8_t
{
    kStorageFile = 0x00,
    kStorageKv   = 0x01,
};
}  // namespace internal
}  // namespace tsync
}  // namespace ara

#endif  /// ARA_TSYNC_INTERNAL_COMMON_H_
