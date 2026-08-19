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
/// @brief      Tool set mixed miscellany
/// @details
/// @date       2023-03-08
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================

#ifndef ISOFT_UTILS_MIX_H_
#define ISOFT_UTILS_MIX_H_

#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdint>
#include <regex>
#include <string>
#include <vector>

/// @brief Seconds to microseconds macro definition
#define SECOND_TO_MICROSECOND (1e3)

/// @brief Define infinitesimal
#define INFINITESIMAL (1e-15)

namespace isoft {
namespace utils {

/// @brief Redefine char type
using Char8_t = char;

/// @brief Determine whether a floating-point value is 0
/// @param value Floating-point value
/// @return Yes/No
inline bool IsEq0(double const &value) noexcept { return (fabs(value) <= INFINITESIMAL); }

/// @brief Time value conversion, seconds to microseconds
/// @param s Seconds
/// @return Microseconds
inline int32_t TimeS2MS(double const &s) noexcept
{
    return static_cast< int32_t >(s * SECOND_TO_MICROSECOND);  // PRQA S 3708, 3132
}

/// @brief Convert boolean value to string
/// @param is Boolean value
/// @return Constant string
inline Char8_t const *ToString(bool const is) noexcept
{
    if (is) {
        return "true";
    }

    return "false";
}

}  // namespace utils
}  // namespace isoft

#endif  //ISOFT_UTILS_MIX_H_
