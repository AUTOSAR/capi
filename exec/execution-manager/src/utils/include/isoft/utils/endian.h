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
/// @file       endian.h
/// @brief      Endianness tool set
/// @details
/// @date       2023-01-09
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_ISOFT_UTIL_ENDIAN_H_
#define ARA_ISOFT_UTIL_ENDIAN_H_

#include <cstdint>
#include <cstring>
#include <tuple>

#include "isoft/utils/type_cast.h"

namespace isoft {
namespace utils {

/// @brief Determine whether the current machine is little-endian
/// @return Boolean value
inline bool IsLittleEndian() noexcept
{
    uint16_t const value{static_cast< uint16_t >(1)};
    uint8_t const *const p{PointerCast< uint8_t const, uint16_t >(&value)};
    if (static_cast< uint8_t >(1) == *p) {
        return true;
    }

    return false;
}

/// @brief Host byte order to network byte order
/// @param dest Destination address
/// @param src Source address
/// @param size Byte size
inline void Host2Network(void *const dest, void const *const src, uint8_t const size) noexcept
{
    uint8_t *d{PointerCast< uint8_t, void >(dest)};
    uint8_t const *s{PointerCast< uint8_t, void >(src) + size};

    if (IsLittleEndian()) {
        while (s > static_cast< uint8_t const * >(src)) {
            s--;
            *d = *s;
            d++;
        }
    } else {
        std::ignore = std::memmove(dest, src, static_cast< std::size_t >(size));
    }
}

/// @brief Network byte order to host byte order
/// @param dest Destination address
/// @param src Source address
/// @param size Byte size
inline void Network2Host(void *const dest, void const *const src, uint8_t const size) noexcept
{
    Host2Network(dest, src, size);
}

/// @brief 16-bit host byte order to network byte order
/// @param srcData Source data
/// @return Converted data
inline uint16_t HtoN16(uint16_t const srcData) noexcept
{
    uint16_t ret{static_cast< uint16_t >(0)};
    Host2Network(&ret, &srcData, sizeof(ret));
    return ret;
}

/// @brief 32-bit host byte order to network byte order
/// @param srcData Source data
/// @return Converted data
inline uint32_t HtoN32(uint32_t const srcData) noexcept
{
    uint32_t ret{static_cast< uint32_t >(0)};
    Host2Network(&ret, &srcData, sizeof(ret));
    return ret;
}

/// @brief 64-bit host byte order to network byte order
/// @param srcData Source data
/// @return Converted data
inline uint64_t HtoN64(uint64_t const srcData) noexcept
{
    uint64_t ret{static_cast< uint64_t >(0)};
    Host2Network(&ret, &srcData, sizeof(ret));
    return ret;
}

/// @brief 16-bit network byte order to host byte order
/// @param srcData Source data
/// @return Converted data
inline uint16_t NtoH16(uint16_t const srcData) noexcept { return HtoN16(srcData); }

/// @brief 32-bit network byte order to host byte order
/// @param srcData Source data
/// @return Converted data
inline uint32_t NtoH32(uint32_t const srcData) noexcept { return HtoN32(srcData); }

/// @brief 64-bit network byte order to host byte order
/// @param srcData Source data
/// @return Converted data
inline uint64_t NtoH64(uint64_t const srcData) noexcept { return HtoN64(srcData); }

}  // namespace utils
}  // namespace isoft

#endif  ///< ARA_ISOFT_UTIL_ENDIAN_H_