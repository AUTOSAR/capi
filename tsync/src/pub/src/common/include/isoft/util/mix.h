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
/// @file       mix.h
/// @brief      collection of small utilities
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

#ifndef ARA_ISOFT_UTIL_MIX_H_
#define ARA_ISOFT_UTIL_MIX_H_

#include <chrono>
#include <cmath>
#include <cstdint>
#include <string>

#include "isoft/util/type_cast.h"

namespace isoft {
namespace util {

/// @brief determine if floating point number equals 0
/// @param value - floating point number
/// @return boolean value
inline bool IsEq0(double const &value) noexcept
{
    double const kDoubleMaxResolution{1e-15};
    return (std::fabs(value) <= kDoubleMaxResolution);
}

/// @brief CRC8 checksum update
/// @param crcCode - source crc code
/// @param data - data
/// @param len - length
/// @return generated CRC checksum
inline std::uint8_t Crc8Update(std::uint8_t const crcCode,
                               std::uint8_t const *const data,
                               std::uint16_t const len) noexcept
{
    std::uint8_t ret{crcCode};
    std::uint8_t const *src{data};
    while ((src - data) < static_cast< std::int32_t >(len)) {
        ret ^= *src;
        src++;
        std::uint8_t bits{static_cast< std::uint8_t >(0)};
        std::uint8_t const kBit8{static_cast< std::uint8_t >(8)};
        std::uint8_t const kBitMask80{static_cast< std::uint8_t >(0x80)};
        std::uint8_t const kBitMask07{static_cast< std::uint8_t >(0x07)};
        std::uint8_t const kBitMaskFF{static_cast< std::uint8_t >(0xFF)};
        std::uint8_t const kBit1{static_cast< std::uint8_t >(1)};
        while (bits < kBit8) {
            if (static_cast< bool >(ret & kBitMask80)) {
                std::uint32_t tmp{ret};
                tmp = static_cast< std::uint32_t >(tmp << static_cast< std::uint32_t >(kBit1)) ^ kBitMask07;
                ret = static_cast< std::uint8_t >(tmp & kBitMaskFF);
            } else {
                ret <<= kBit1;
            }
            bits++;
        }
    }
    return ret;
}
/// @brief CRC8H2F checksum update
/// @param data - data
/// @param len - length
/// @return generated CRC checksum
inline std::uint8_t Crc8H2FUpdate(std::uint8_t const *const data, std::uint16_t const len) noexcept
{
    constexpr std::uint8_t kNum{0xFFU};
    std::uint8_t ret{kNum};
    std::uint8_t const u8XOR{0xFFU};
    std::uint8_t const *src{data};
    while ((src - data) < static_cast< std::int32_t >(len)) {
        ret ^= *src;
        src++;
        std::uint8_t bits{static_cast< std::uint8_t >(0)};
        std::uint8_t const kBit8{static_cast< std::uint8_t >(8)};
        std::uint8_t const kBitMask80{static_cast< std::uint8_t >(0x80)};
        std::uint8_t const kBitMask07{static_cast< std::uint8_t >(0x2F)};
        std::uint8_t const kBitMaskFF{static_cast< std::uint8_t >(0xFF)};
        std::uint8_t const kBit1{static_cast< std::uint8_t >(1)};
        while (bits < kBit8) {
            if (static_cast< bool >(ret & kBitMask80)) {
                std::uint32_t tmp{ret};
                tmp = static_cast< std::uint32_t >(tmp << static_cast< std::uint32_t >(kBit1)) ^ kBitMask07;
                ret = static_cast< std::uint8_t >(tmp & kBitMaskFF);
            } else {
                ret <<= kBit1;
            }
            bits++;
        }
    }
    ret ^= u8XOR;
    return ret;
}
/// @brief calculate CRC8 byte code
/// @param data - source data
/// @param len - length
/// @return CRC byte code
inline std::uint8_t Crc8(std::uint8_t const *const data, std::uint16_t const len) noexcept
{
    return Crc8H2FUpdate(data, len);
}

}  // namespace util
}  // namespace isoft

#endif  //ARA_ISOFT_UTIL_H_
