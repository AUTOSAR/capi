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
/// @file       abort.h
/// @brief      This file describes the APIs that constitute the explicit abnormal termination facility.
/// @details
/// @date       2022-06-24
/// @author     hanzhibo
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_CORE_ABORT_H_
#define ARA_CORE_ABORT_H_

namespace ara {
namespace core {

///
/// @brief A function declaration with the correct prototype for SetAbortHandler().
///        This declaration exists only for providing a function type that includes "noexcept" and that acts
///        as base type for a type alias, which is defined in SWS_CORE_00050.
///        This compensates for the fact that the C++ standard (up to and including C++14) prohibits that
///        "noexcept" appears in an alias-declaration. There is no implementation of this function.
///
/// @uptrace{SWS_CORE_00053, a5c792db584db3bf7d7df7701721d915546ae4cf}
///
#ifdef __clang__
void AbortHandlerPrototype();
#else
void AbortHandlerPrototype() noexcept;
#endif

///
/// @brief The type of a handler for SetAbortHandler().
///
/// @uptrace{SWS_CORE_00050, a42d1f1aaecad9aac737c415c3b18f07a5adbb23}
///
using AbortHandler = decltype(&AbortHandlerPrototype);

///
/// @brief Set a custom global Abort handler function and return the previously installed one.
///        By setting nullptr, the implementation may restore the default handler instead.
///        This function can be called from multiple threads simultaneously; these calls are performed in
///        an implementation-defined sequence.
///
/// @param[in] handler AbortHandler a custom Abort handler (or nullptr)
///
/// @return the previously installed Abort handler (or nullptr if none was installed)
///
/// @uptrace{SWS_CORE_00051, 13b34c56cd22d3fd10b1152a31286ff8ca7a0ee1}
///
AbortHandler SetAbortHandler(AbortHandler handler) noexcept;

///
/// @brief Abort the current operation.
///        This function will never return to its caller. The stack is not unwound: destructors of variables
///        with automatic storage duration are not called.
///
/// @param[in] text a custom text to include in the log message being output
///
/// @return None
///
/// @uptrace{SWS_CORE_00052, 28f881349b5ba2eae296c71f59f956b24ef38366}
///
[[noreturn]] void Abort(char const* text) noexcept;

}  // namespace core
}  // namespace ara

#endif  // ARA_CORE_ABORT_H_
