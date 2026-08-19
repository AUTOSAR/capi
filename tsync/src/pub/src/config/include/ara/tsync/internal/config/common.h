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
/// @brief      configuration module common file
/// @details
/// @date       2023-01-09
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// export_level=/TimeSync/Config
/// module_path=/TimeSync/Config
/// @endcode
///
/// ================================================================

#ifndef ARA_TSYNC_INTERNAL_CONFIG_COMMON_H_
#define ARA_TSYNC_INTERNAL_CONFIG_COMMON_H_
#include <cstring>

#include "ara/core/string.h"
#include "ara/core/string_view.h"
#include "ara/tsync/internal/common.h"
#include "ara/tsync/internal/log/logger.h"

namespace ara {
namespace tsync {
namespace internal {
namespace config {

/// @brief time domain type definition
enum class TSTimeStampTypeEnum : std::uint8_t
{
    /// @brief kernel timestamp
    kSoftware = 1,
    /// @brief network card hardware timestamp
    kHardware = 2,
};

/// @brief char8_t
using char8_t = char;  // NOLINT

/// @brief get Short Name from FQN delimited by "[]"
/// @param fqn full name
/// @return short name
inline ara::core::StringView GetNewBaseName(ara::core::StringView const &fqn)
{
    /// If the string does not contain "[]", treat fqn as short name
    std::size_t const fqnLen{fqn.length()};
    if (fqn[fqnLen - 1U] != ']') {
        /// If the last character of the string is not ']', but contains any character in "[.]", the format is incorrect
        if (fqn.find_last_of("[.]") != ara::core::StringView::npos) {
            return "";
        }

        return fqn;
    }

    /// If the last character is ']', the minimum length, and a string that meets the requirements should be like "[a]"
    /// So if the string length is less than 3, even if the format is correct, it is an invalid fqn
    constexpr uint32_t kMinFqnLength{3U};
    if (fqnLen < kMinFqnLength) {
        return "";
    }

    std::size_t const beginPos{fqn.find_last_of("[")};
    std::size_t const endPos{fqn.find_last_of("]", fqnLen - 2U)};
    /// If the string ends with something like "[aaaa]bbbb]", it is also considered an error
    if ((beginPos == ara::core::StringView::npos) && (endPos != ara::core::StringView::npos)) {
        if (endPos > beginPos) {
            return "";
        }
    }

    return fqn.substr(beginPos + 1, fqnLen - beginPos - 2);
}

/// @brief get name via FQN
/// @param fqn - fully qualified name
/// @return short Name
inline ara::core::StringView BaseName(ara::core::StringView const &fqn) noexcept
{
    ara::core::StringView::size_type pos{0U};
    ara::core::StringView sv;
    char8_t const letter{'/'};

    if (fqn.empty()) {
        return sv;
    }
    pos = fqn.find_last_of(letter);
    if (ara::core::StringView::npos != pos) {
        sv = fqn.substr(pos + 1U);

    } else {
        sv = GetNewBaseName(fqn);
    }
    return sv;
}

/// @brief get name via FQN
/// @param fqn - fully qualified name
/// @return short Name
inline ara::core::StringView BaseName(ara::core::String const &fqn) noexcept
{
    ara::core::StringView const sv{fqn.data(), fqn.size()};
    ara::core::StringView ret;
    ret = BaseName(sv);
    return ret;
}
/// @brief time domain CRC verification enumeration definition
enum class GlobalTimeCrcValidation : std::uint8_t
{
    kCrcValidated = 1,
    kCrcNotValidated,
    kCrcIgnord,
    kCrcOptional
};

/// @brief maximum rate value set by user SWS_TS_01405, rateDeviation range: 0..+-32000
/// PPM, refer to CP
static constexpr double kUSER_RATE_MAX{1.032};
/// @brief minimum rate value set by user
static constexpr double kUSER_RATE_MIN{0.968};
/// @brief jitter range when calculating rate
static constexpr double kUSER_CALC_VIBRATION{0.01};

/// @brief log output
/// @return Logger object reference
inline ara::tsync::internal::tslog::Logger &LOG() noexcept { return ara::tsync::internal::tslog::Log(); }

}  // namespace config
}  // namespace internal
}  // namespace tsync
}  // namespace ara

#endif  /// ARA_TSYNC_INTERNAL_CONFIG_COMMON_H_
