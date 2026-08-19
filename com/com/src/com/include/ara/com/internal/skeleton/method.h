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
/// @file       method.h
/// @brief      Communication service skeleton method header file
/// @details
/// @date       2023-10-08
/// @author     mazelin
/// @version    1.2.0
///
/// ================================================================

#pragma once

#include "ara/com/types.h"

/// @brief Namespace -- Internal skeleton
namespace ara {
namespace com {
namespace internal {
namespace skeleton {
/// @brief Template function -- Convert to function object -- member function with return
/// @tparam C Class type
/// @tparam R Method return type
/// @tparam Args Method parameter list types
/// @param[in] callback Class member function
/// @param[in] object Class object
/// @return Function object
template < typename C, typename R, typename... Args >
inline std::function< R(Args...) > ToFunction(R (C::*callback)(Args...), C& object) noexcept
{
    return [&, callback](Args&&... args) { return (object.*callback)(std::forward< Args >(args)...); };
}
/// @brief Template function -- Convert to function object -- member function without return
/// @tparam C Class type
/// @tparam Args Method parameter list types
/// @param[in] callback Class member function
/// @param[in] object Class object
/// @return Function object
template < typename C, typename... Args >
inline std::function< void(Args...) > ToFunction(void (C::*callback)(Args...), C& object) noexcept
{
    return [&, callback](Args&&... args) { (object.*callback)(std::forward< Args >(args)...); };
}
}  // namespace skeleton
}  // namespace internal
}  // namespace com
}  // namespace ara