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
/// @file       type_cast.h
/// @brief      type conversion
/// @details
/// @date       2023-02-17
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================

#ifndef ISOFT_UTIL_TYPE_CAST_H_
#define ISOFT_UTIL_TYPE_CAST_H_

namespace isoft {
namespace util {

/// @brief pointer type conversion
/// @tparam T_dest - destination type
/// @tparam T_src - source type
/// @param p - pointer
/// @return pointer
template < typename T_dest, typename T_src >
inline T_dest* PointerCast(T_src* const p) noexcept
{
    return static_cast< T_dest* >(static_cast< void* >(p));
}

/// @brief protected pointer type conversion
/// @tparam T_dest - destination type
/// @tparam T_src - source type
/// @param p - pointer
/// @return pointer
template < typename T_dest, typename T_src >
inline T_dest const* PointerCast(T_src const* const p) noexcept
{
    return static_cast< T_dest const* >(static_cast< void const* >(p));
}

}  // namespace util
}  // namespace isoft

#endif  /// ISOFT_UTIL_TYPE_CAST_H_