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
/// @file       vector.h
/// @brief      The definition and implementation of ara::core::Vector that represents a container which can change in size.
/// @details
/// @date       2022-06-24
/// @author     hanzhibo
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_CORE_VECTOR_H_
#define ARA_CORE_VECTOR_H_

#include <memory>
#include <type_traits>
#include <vector>

namespace ara {
namespace core {

/**
 * @brief A sequence container that encapsulates dynamically sized arrays
 *
 * @tparam T  the type of contained values
 * @tparam Allocator  the type of Allocator to use for this container
 *
 * @uptrace{SWS_CORE_01301, d95748740ae19ec41b11c5744d88cde51c5b9f43}
 */
template < typename T, typename Allocator = std::allocator< T > >
using Vector = std::vector< T, Allocator >;

/**
 * @brief Global operator== for Vector instances
 *
 * @uptrace{SWS_CORE_01390, d2268134fd9c147ab972bb7ac776177834e9e0d9}
 */
template < typename T,
           typename Allocator,
           typename = typename std::enable_if<
               !std::is_same< Vector< T, Allocator >, std::vector< T, Allocator > >::value >::type >
inline bool operator==(Vector< T, Allocator > const& lhs, Vector< T, Allocator > const& rhs)
{
    return std::operator==(lhs, rhs);
}

/**
 * @brief Global operator!= for Vector instances
 *
 * @uptrace{SWS_CORE_01391, 967df123c61833645199f417a01bbd1f993ecef9}
 */
template < typename T,
           typename Allocator,
           typename = typename std::enable_if<
               !std::is_same< Vector< T, Allocator >, std::vector< T, Allocator > >::value >::type >
inline bool operator!=(Vector< T, Allocator > const& lhs, Vector< T, Allocator > const& rhs)
{
    return std::operator!=(lhs, rhs);
}

/**
 * @brief Global operator< for Vector instances
 *
 * @uptrace{SWS_CORE_01392, 0b22dcece70b9e067c3b4d6f9f75ab7099967f33}
 */
template < typename T,
           typename Allocator,
           typename = typename std::enable_if<
               !std::is_same< Vector< T, Allocator >, std::vector< T, Allocator > >::value >::type >
inline bool operator<(Vector< T, Allocator > const& lhs, Vector< T, Allocator > const& rhs)
{
    return std::operator<(lhs, rhs);
}

/**
 * @brief Global operator<= for Vector instances
 *
 * @uptrace{SWS_CORE_01393, b3019d07193d118801fe8adf9caea682ee0df2ef}
 */
template < typename T,
           typename Allocator,
           typename = typename std::enable_if<
               !std::is_same< Vector< T, Allocator >, std::vector< T, Allocator > >::value >::type >
inline bool operator<=(Vector< T, Allocator > const& lhs, Vector< T, Allocator > const& rhs)
{
    return std::operator<=(lhs, rhs);
}

/**
 * @brief Global operator> for Vector instances
 *
 * @uptrace{SWS_CORE_01394, 86bfa14b8d63fef04ace5c4aad0538f1b1e035a9}
 */
template < typename T,
           typename Allocator,
           typename = typename std::enable_if<
               !std::is_same< Vector< T, Allocator >, std::vector< T, Allocator > >::value >::type >
inline bool operator>(Vector< T, Allocator > const& lhs, Vector< T, Allocator > const& rhs)
{
    return std::operator>(lhs, rhs);
}

/**
 * @brief Global operator>= for Vector instances
 *
 * @uptrace{SWS_CORE_01395, 69161d21ddc8268d8a166bd95e521779dd414270}
 */
template < typename T,
           typename Allocator,
           typename = typename std::enable_if<
               !std::is_same< Vector< T, Allocator >, std::vector< T, Allocator > >::value >::type >
inline bool operator>=(Vector< T, Allocator > const& lhs, Vector< T, Allocator > const& rhs)
{
    return std::operator>=(lhs, rhs);
}

/**
 * @brief Add overload of swap for ara::core::Vector
 *
 * We actually don't need this overload at all, because our implementation is just
 * a type alias and thus can simply use the overload for the std:: type.
 * However, we need this symbol in order to provide uptracing.
 *
 * @tparam T  the type of values in the Vectors
 * @tparam Allocator  the Allocator used by the Vectors
 * @param lhs  the first argument of the swap invocation
 * @param rhs  the second argument of the swap invocation
 *
 * @uptrace{SWS_CORE_01396, 11dfcff84ee7e4514c511dd3d8c55e306feacd0a}
 */
template < typename T,
           typename Allocator,
           typename = typename std::enable_if<
               !std::is_same< Vector< T, Allocator >, std::vector< T, Allocator > >::value >::type >
void swap(Vector< T, Allocator >& lhs, Vector< T, Allocator >& rhs)  // NOLINT
{
    lhs.swap(rhs);
}

}  // namespace core
}  // namespace ara

#endif  // ARA_CORE_VECTOR_H_
