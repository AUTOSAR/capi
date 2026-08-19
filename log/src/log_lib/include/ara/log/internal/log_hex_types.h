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
/// @file       log_hex_types.h
/// @brief      Hexadecimal formatting types for log API
/// @details
/// @date       2025-09-03
/// @author     yangjinbiao
/// @version    1.2.0
///
/// ================================================================

#ifndef __LOG_INTERNAL_LOG_HEX_TYPES__
#define __LOG_INTERNAL_LOG_HEX_TYPES__

#include <cstdint>

namespace ara {
namespace log {

/// @brief Hexadecimal number with width of 8 bits.
///
/// @code{.isoft}
/// @trace_id_sws={SWS_LOG_00108}@tracestatus{draft}
///
/// @interface_level=unit
/// @trace_id_ad=AD_LOG_00001
/// @trace_id_dd=DD_LOG_00001
/// @needwork = ad
/// @endcode
struct LogHex8 final
{
    /// @brief value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00000
    /// @trace_id_dd=DD_LOG_01580
    /// @needwork = dda
    /// @endcode
    std::uint8_t value;
};
/// @brief Hexadecimal number with width of 16 bits.
///
/// @code{.isoft}
///
/// @trace_id_sws={SWS_LOG_00109}@tracestatus{draft}
///
/// @interface_level=unit
/// @trace_id_ad=AD_LOG_00002
/// @trace_id_dd=DD_LOG_00002
/// @needwork = ad
/// @endcode
struct LogHex16 final
{
    /// @brief value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00000
    /// @trace_id_dd=DD_LOG_01581
    /// @needwork = dda
    /// @endcode
    std::uint16_t value;
};

/// @brief Hexadecimal number with width of 32 bits.
///
/// @code{.isoft}
///
/// @trace_id_sws={SWS_LOG_00110}@tracestatus{draft}
///
/// @interface_level=unit
/// @trace_id_ad=AD_LOG_00003
/// @trace_id_dd=DD_LOG_00003
/// @needwork = ad
/// @endcode
struct LogHex32 final
{
    /// @brief value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00000
    /// @trace_id_dd=DD_LOG_01582
    /// @needwork = dda
    /// @endcode
    std::uint32_t value;
};

/// @brief Hexadecimal number with width of 64 bits.
///
/// @code{.isoft}
///
/// @trace_id_sws={SWS_LOG_00111}@tracestatus{draft}
///
/// @interface_level=unit
/// @trace_id_ad=AD_LOG_00004
/// @trace_id_dd=DD_LOG_00004
/// @needwork = ad
/// @endcode
struct LogHex64 final
{
    /// @brief value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00000
    /// @trace_id_dd=DD_LOG_01583
    /// @needwork = dda
    /// @endcode
    std::uint64_t value;
};

/// @brief Convert a decimal number with width of 8 bits to hexadecimal.
///
/// @param[in]  value The decimal number to convert
/// @return The converted hexadecimal number
///
/// @code{.isoft}
///
/// @trace_id_sws={SWS_LOG_00022}@tracestatus{draft}
///
/// @interface_level=unit
/// @trace_id_ad=AD_LOG_00009
/// @trace_id_dd=DD_LOG_00009
/// @needwork = ad
/// @endcode
constexpr LogHex8 HexFormat(uint8_t value) noexcept { return LogHex8{value}; }

/// @brief Convert a decimal number with width of 8 bits to hexadecimal.
///
/// @param[in]  value The decimal number to convert
/// @return The converted hexadecimal number
///
/// @code{.isoft}
///
/// @trace_id_sws={SWS_LOG_00023}@tracestatus{draft}
///
/// @interface_level=unit
/// @trace_id_ad=AD_LOG_00010
/// @trace_id_dd=DD_LOG_00010
/// @needwork = ad
/// @endcode
constexpr LogHex8 HexFormat(int8_t value) noexcept { return LogHex8{static_cast< uint8_t >(value)}; }

/// @brief Convert a decimal number with width of 16 bits to hexadecimal.
///
/// @param[in]  value The decimal number to convert
/// @return The converted hexadecimal number
///
/// @code{.isoft}
///
/// @trace_id_sws={SWS_LOG_00024}@tracestatus{draft}
///
/// @interface_level=unit
/// @trace_id_ad=AD_LOG_00011
/// @trace_id_dd=DD_LOG_00011
/// @needwork = ad
/// @endcode
constexpr LogHex16 HexFormat(uint16_t value) noexcept { return LogHex16{value}; }

/// @brief Convert a decimal number with width of 16 bits to hexadecimal.
///
/// @param[in]  value The decimal number to convert
/// @return The converted hexadecimal number
///
/// @code{.isoft}
///
/// @trace_id_sws={SWS_LOG_00025}@tracestatus{draft}
///
/// @interface_level=unit
/// @trace_id_ad=AD_LOG_00012
/// @trace_id_dd=DD_LOG_00012
/// @needwork = ad
/// @endcode
constexpr LogHex16 HexFormat(int16_t value) noexcept { return LogHex16{static_cast< uint16_t >(value)}; }

/// @brief Convert a decimal number with width of 32 bits to hexadecimal.
///
/// @param[in]  value The decimal number to convert
/// @return The converted hexadecimal number
///
/// @code{.isoft}
///
/// @trace_id_sws={SWS_LOG_00026}@tracestatus{draft}
///
/// @interface_level=unit
/// @trace_id_ad=AD_LOG_00013
/// @trace_id_dd=DD_LOG_00013
/// @needwork = ad
/// @endcode
constexpr LogHex32 HexFormat(uint32_t value) noexcept { return LogHex32{value}; }

/// @brief Convert a decimal number with width of 32 bits to hexadecimal.
///
/// @param[in]  value The decimal number to convert
/// @return The converted hexadecimal number
///
/// @code{.isoft}
///
/// @trace_id_sws={SWS_LOG_00027}@tracestatus{draft}
///
/// @interface_level=unit
/// @trace_id_ad=AD_LOG_00014
/// @trace_id_dd=DD_LOG_00014
/// @needwork = ad
/// @endcode
constexpr LogHex32 HexFormat(int32_t value) noexcept { return LogHex32{static_cast< uint32_t >(value)}; }

/// @brief Convert a decimal number with width of 64 bits to hexadecimal.
///
/// @param[in]  value The decimal number to convert
/// @return The converted hexadecimal number
///
/// @code{.isoft}
///
/// @trace_id_sws={SWS_LOG_00028}@tracestatus{draft}
///
/// @interface_level=unit
/// @trace_id_ad=AD_LOG_00015
/// @trace_id_dd=DD_LOG_00015
/// @needwork = ad
/// @endcode
constexpr LogHex64 HexFormat(uint64_t value) noexcept { return LogHex64{value}; }

/// @brief Convert a decimal number with width of 64 bits to hexadecimal.
///
/// @param[in]  value The decimal number to convert
/// @return The converted hexadecimal number
///
/// @code{.isoft}
///
/// @trace_id_sws={SWS_LOG_00029}@tracestatus{draft}
///
/// @interface_level=unit
/// @trace_id_ad=AD_LOG_00016
/// @trace_id_dd=DD_LOG_00016
/// @needwork = ad
/// @endcode
constexpr LogHex64 HexFormat(int64_t value) noexcept { return LogHex64{static_cast< uint64_t >(value)}; }

}  // namespace log
}  // namespace ara

#endif  // __LOG_INTERNAL_LOG_HEX_TYPES__
