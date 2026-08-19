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
/// @file       log_bin_types.h
/// @brief      Binary formatting types for log API
/// @details
/// @date       2025-09-03
/// @author     yangjinbiao
/// @version    1.2.0
///
/// ================================================================

#ifndef __LOG_INTERNAL_LOG_BIN_TYPES__
#define __LOG_INTERNAL_LOG_BIN_TYPES__

#include <cstdint>

namespace ara {
namespace log {

/// @brief Binary number with width of 8 bits.
///
/// @code{.isoft}
///
/// @trace_id_sws={SWS_LOG_00112}@tracestatus{draft}
///
/// @interface_level=unit
/// @trace_id_ad=AD_LOG_00005
/// @trace_id_dd=DD_LOG_00005
/// @needwork = ad
/// @endcode
struct LogBin8 final
{
    /// @brief value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00000
    /// @trace_id_dd=DD_LOG_01584
    /// @needwork = dda
    /// @endcode
    std::uint8_t value;
};

/// @brief Binary number with width of 16 bits.
///
/// @code{.isoft}
///
/// @trace_id_sws={SWS_LOG_00113}@tracestatus{draft}
///
/// @interface_level=unit
/// @trace_id_ad=AD_LOG_00006
/// @trace_id_dd=DD_LOG_00006
/// @needwork = ad
/// @endcode
struct LogBin16 final
{
    /// @brief value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00000
    /// @trace_id_dd=DD_LOG_01585
    /// @needwork = dda
    /// @endcode
    std::uint16_t value;
};

/// @brief Binary number with width of 32 bits.
///
/// @code{.isoft}
///
/// @trace_id_sws={SWS_LOG_00114}@tracestatus{draft}
///
/// @interface_level=unit
/// @trace_id_ad=AD_LOG_00007
/// @trace_id_dd=DD_LOG_00007
/// @needwork = ad
/// @endcode
struct LogBin32 final
{
    /// @brief value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00000
    /// @trace_id_dd=DD_LOG_01586
    /// @needwork = dda
    /// @endcode
    std::uint32_t value;
};

/// @brief Binary number with width of 64 bits.
///
/// @code{.isoft}
///
/// @trace_id_sws={SWS_LOG_00115}@tracestatus{draft}
///
/// @interface_level=unit
/// @trace_id_ad=AD_LOG_00008
/// @trace_id_dd=DD_LOG_00008
/// @needwork = ad
/// @endcode
struct LogBin64 final
{
    /// @brief value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00000
    /// @trace_id_dd=DD_LOG_01587
    /// @needwork = dda
    /// @endcode
    std::uint64_t value;
};

/// @brief Convert a decimal number with width of 8 bits to binary.
///
/// @param[in]  value The decimal number to convert
/// @return The converted binary number
///
/// @code{.isoft}
///
/// @trace_id_sws={SWS_LOG_00030}@tracestatus{draft}
///
/// @interface_level=unit
/// @trace_id_ad=AD_LOG_00017
/// @trace_id_dd=DD_LOG_00017
/// @needwork = ad
/// @endcode
constexpr LogBin8 BinFormat(uint8_t value) noexcept { return LogBin8{value}; }

/// @brief Convert a decimal number with width of 8 bits to binary.
///
/// @param[in]  value The decimal number to convert
/// @return The converted binary number
///
/// @code{.isoft}
///
/// @trace_id_sws={SWS_LOG_00031}@tracestatus{draft}
///
/// @interface_level=unit
/// @trace_id_ad=AD_LOG_00018
/// @trace_id_dd=DD_LOG_00018
/// @needwork = ad
/// @endcode
constexpr LogBin8 BinFormat(int8_t value) noexcept { return LogBin8{static_cast< uint8_t >(value)}; }

/// @brief Convert a decimal number with width of 16 bits to binary.
///
/// @param[in]  value The decimal number to convert
/// @return The converted binary number
///
/// @code{.isoft}
///
/// @trace_id_sws={SWS_LOG_00032}@tracestatus{draft}
///
/// @interface_level=unit
/// @trace_id_ad=AD_LOG_00019
/// @trace_id_dd=DD_LOG_00019
/// @needwork = ad
/// @endcode
constexpr LogBin16 BinFormat(uint16_t value) noexcept { return LogBin16{value}; }

/// @brief Convert a decimal number with width of 16 bits to binary.
///
/// @param[in]  value The decimal number to convert
/// @return The converted binary number
///
/// @code{.isoft}
///
/// @trace_id_sws={SWS_LOG_00033}@tracestatus{draft}
///
/// @interface_level=unit
/// @trace_id_ad=AD_LOG_00020
/// @trace_id_dd=DD_LOG_00020
/// @needwork = ad
/// @endcode
constexpr LogBin16 BinFormat(int16_t value) noexcept { return LogBin16{static_cast< uint16_t >(value)}; }

/// @brief Convert a decimal number with width of 32 bits to binary.
///
/// @param[in]  value The decimal number to convert
/// @return The converted binary number
///
/// @code{.isoft}
///
/// @trace_id_sws={SWS_LOG_00034}@tracestatus{draft}
///
/// @interface_level=unit
/// @trace_id_ad=AD_LOG_00021
/// @trace_id_dd=DD_LOG_00021
/// @needwork = ad
/// @endcode
constexpr LogBin32 BinFormat(uint32_t value) noexcept { return LogBin32{value}; }

/// @brief Convert a decimal number with width of 32 bits to binary.
///
/// @param[in]  value The decimal number to convert
/// @return The converted binary number
///
/// @code{.isoft}
///
/// @trace_id_sws={SWS_LOG_00035}@tracestatus{draft}
///
/// @interface_level=unit
/// @trace_id_ad=AD_LOG_00022
/// @trace_id_dd=DD_LOG_00022
/// @needwork = ad
/// @endcode
constexpr LogBin32 BinFormat(int32_t value) noexcept { return LogBin32{static_cast< uint32_t >(value)}; }

/// @brief Convert a decimal number with width of 64 bits to binary.
///
/// @param[in]  value The decimal number to convert
/// @return The converted binary number
///
/// @code{.isoft}
///
/// @trace_id_sws={SWS_LOG_00036}@tracestatus{draft}
///
/// @interface_level=unit
/// @trace_id_ad=AD_LOG_00023
/// @trace_id_dd=DD_LOG_00023
/// @needwork = ad
/// @endcode
constexpr LogBin64 BinFormat(uint64_t value) noexcept { return LogBin64{value}; }

/// @brief Convert a decimal number with width of 64 bits to binary.
///
/// @param[in]  value The decimal number to convert
/// @return The converted binary number
///
/// @code{.isoft}
///
/// @trace_id_sws={SWS_LOG_00037}@tracestatus{draft}
///
/// @interface_level=unit
/// @trace_id_ad=AD_LOG_00024
/// @trace_id_dd=DD_LOG_00024
/// @needwork = ad
/// @endcode
constexpr LogBin64 BinFormat(int64_t value) noexcept { return LogBin64{static_cast< uint64_t >(value)}; }

}  // namespace log
}  // namespace ara

#endif  // __LOG_INTERNAL_LOG_BIN_TYPES__
