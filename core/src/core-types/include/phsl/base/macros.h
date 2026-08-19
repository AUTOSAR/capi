// Modified by iSOFT Infrastructure Software Co., Ltd. on 2022-06-24
//
// Copyright 2017 The Abseil Authors.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// -----------------------------------------------------------------------------
// File: macros.h
// -----------------------------------------------------------------------------
//
// This header file defines the set of language macros used within Abseil code.
// For the set of macros used to determine supported compilers and platforms,
// see phsl/base/config.h instead.
//
// This code is compiled directly on many platforms, including client
// platforms like Windows, Mac, and embedded systems.  Before making
// any changes here, make sure that you're not breaking any platforms.

#ifndef PBSL_BASE_MACROS_H_
#define PBSL_BASE_MACROS_H_

#include <cassert>
#include <cstddef>

#include "phsl/base/attributes.h"
#include "phsl/base/config.h"
#include "phsl/base/optimization.h"
#include "phsl/base/options.h"
#include "phsl/base/port.h"

// PBSL_ARRAYSIZE()
//
// Returns the number of elements in an array as a compile-time constant, which
// can be used in defining new arrays. If you use this macro on a pointer by
// mistake, you will get a compile-time error.
#define PBSL_ARRAYSIZE(array) (sizeof(::phsl::macros_internal::ArraySizeHelper(array)))

namespace phsl {
PBSL_NAMESPACE_BEGIN
namespace macros_internal {
// Note: this internal template function declaration is used by PBSL_ARRAYSIZE.
// The function doesn't need a definition, as we only use its type.
template < typename T, size_t N >  // NOLINT : keep template parameter naming style
auto ArraySizeHelper(const T (&array)[N]) -> char (&)[N];
}  // namespace macros_internal
PBSL_NAMESPACE_END
}  // namespace phsl

// PBSL_BAD_CALL_IF()
//
// Used on a function overload to trap bad calls: any call that matches the
// overload will cause a compile-time error. This macro uses a clang-specific
// "enable_if" attribute, as described at
// https://clang.llvm.org/docs/AttributeReference.html#enable-if
//
// Overloads which use this macro should be bracketed by
// `#ifdef PBSL_BAD_CALL_IF`.
//
// Example:
//
//   int isdigit(int c);
//   #ifdef PBSL_BAD_CALL_IF
//   int isdigit(int c)
//     PBSL_BAD_CALL_IF(c <= -1 || c > 255,
//                       "'c' must have the value of an unsigned char or EOF");
//   #endif // PBSL_BAD_CALL_IF
#if PBSL_HAVE_ATTRIBUTE(enable_if)
    #define PBSL_BAD_CALL_IF(expr, msg) __attribute__((enable_if(expr, "Bad call trap"), unavailable(msg)))
#endif

// PBSL_ASSERT()
//
// In C++11, `assert` can't be used portably within constexpr functions.
// `assert` also generates spurious unused-symbol warnings.
// PBSL_ASSERT functions as a runtime assert but works in C++11 constexpr
// functions, and maintains references to symbols.  Example:
//
// constexpr double Divide(double a, double b) {
//   return PBSL_ASSERT(b != 0), a / b;
// }
//
// This macro is inspired by
// https://akrzemi1.wordpress.com/2017/05/18/asserts-in-constexpr-functions/
#if defined(NDEBUG)
    #if PBSL_INTERNAL_CPLUSPLUS_LANG >= 202002L

        // We use `decltype` here to avoid generating unnecessary code that the
        // optimizer then has to optimize away.
        // This not only improves compilation performance by reducing codegen bloat
        // and optimization work, but also guarantees fast run-time performance without
        // having to rely on the optimizer.
        #define PBSL_ASSERT(expr) (decltype((expr) ? void() : void())())
    #else

        // Pre-C++20, lambdas can't be inside unevaluated operands, so we're forced to
        // rely on the optimizer.
        #define PBSL_ASSERT(expr) (false ? ((expr) ? void() : void()) : void())
    #endif
#else
    #define PBSL_ASSERT(expr)                                                                                          \
        (PBSL_PREDICT_TRUE((expr)) ? static_cast< void >(0) : [] { assert(false && #expr); }())  // NOLINT
#endif

// `PBSL_INTERNAL_HARDENING_ABORT()` controls how `PBSL_HARDENING_ASSERT()`
// aborts the program in release mode (when NDEBUG is defined). The
// implementation should abort the program as quickly as possible and ideally it
// should not be possible to ignore the abort request.
#define PBSL_INTERNAL_HARDENING_ABORT()                                                                                \
    do {                                                                                                               \
        PBSL_INTERNAL_IMMEDIATE_ABORT_IMPL();                                                                          \
        PBSL_INTERNAL_UNREACHABLE_IMPL();                                                                              \
    } while (false)

// PBSL_HARDENING_ASSERT()
//
// `PBSL_HARDENING_ASSERT()` is like `PBSL_ASSERT()`, but used to implement
// runtime assertions that should be enabled in hardened builds even when
// `NDEBUG` is defined.
//
// When `NDEBUG` is not defined, `PBSL_HARDENING_ASSERT()` is identical to
// `PBSL_ASSERT()`.
//
// See `PBSL_OPTION_HARDENED` in `phsl/base/options.h` for more information on
// hardened mode.
#if (PBSL_OPTION_HARDENED == 1 || PBSL_OPTION_HARDENED == 2) && defined(NDEBUG)
    #define PBSL_HARDENING_ASSERT(expr)                                                                                \
        (PBSL_PREDICT_TRUE((expr)) ? static_cast< void >(0) : [] { PBSL_INTERNAL_HARDENING_ABORT(); }())
#else
    #define PBSL_HARDENING_ASSERT(expr) PBSL_ASSERT(expr)
#endif

// PBSL_HARDENING_ASSERT_SLOW()
//
// `PBSL_HARDENING_ASSERT()` is like `PBSL_HARDENING_ASSERT()`,
//  but specifically for assertions whose predicates are too slow
//  to be enabled in many applications.
//
// When `NDEBUG` is not defined, `PBSL_HARDENING_ASSERT_SLOW()` is identical to
// `PBSL_ASSERT()`.
//
// See `PBSL_OPTION_HARDENED` in `phsl/base/options.h` for more information on
// hardened mode.
#if PBSL_OPTION_HARDENED == 1 && defined(NDEBUG)
    #define PBSL_HARDENING_ASSERT_SLOW(expr)                                                                           \
        (PBSL_PREDICT_TRUE((expr)) ? static_cast< void >(0) : [] { PBSL_INTERNAL_HARDENING_ABORT(); }())
#else
    #define PBSL_HARDENING_ASSERT_SLOW(expr) PBSL_ASSERT(expr)
#endif

#ifdef PBSL_HAVE_EXCEPTIONS
    #define PBSL_INTERNAL_TRY       try
    #define PBSL_INTERNAL_CATCH_ANY catch (...)
    #define PBSL_INTERNAL_RETHROW                                                                                      \
        do {                                                                                                           \
            throw;                                                                                                     \
        } while (false)
#else  // PBSL_HAVE_EXCEPTIONS
    #define PBSL_INTERNAL_TRY       if (true)
    #define PBSL_INTERNAL_CATCH_ANY else if (false)
    #define PBSL_INTERNAL_RETHROW                                                                                      \
        do {                                                                                                           \
        } while (false)
#endif  // PBSL_HAVE_EXCEPTIONS

// PBSL_DEPRECATE_AND_INLINE()
//
// Marks a function or type alias as deprecated and tags it to be picked up for
// automated refactoring by go/cpp-inliner. It can added to inline function
// definitions or type aliases. It should only be used within a header file. It
// differs from `PBSL_DEPRECATED` in the following ways:
//
// 1. New uses of the function or type will be discouraged via Tricorder
//    warnings.
// 2. If enabled via `METADATA`, automated changes will be sent out inlining the
//    functions's body or replacing the type where it is used.
//
// For example:
//
// PBSL_DEPRECATE_AND_INLINE() inline int OldFunc(int x) {
//   return NewFunc(x, 0);
// }
//
// will mark `OldFunc` as deprecated, and the go/cpp-inliner service will
// replace calls to `OldFunc(x)` with calls to `NewFunc(x, 0)`. Once all calls
// to `OldFunc` have been replaced, `OldFunc` can be deleted.
//
// See go/cpp-inliner for more information.
//
// Note: go/cpp-inliner is Google-internal service for automated refactoring.
// While open-source users do not have access to this service, the macro is
// provided for compatibility, and so that users receive deprecation warnings.
#if PBSL_HAVE_CPP_ATTRIBUTE(deprecated) && PBSL_HAVE_CPP_ATTRIBUTE(clang::annotate)
    #define PBSL_DEPRECATE_AND_INLINE() [[deprecated, clang::annotate("inline-me")]]
#elif PBSL_HAVE_CPP_ATTRIBUTE(deprecated)
    #define PBSL_DEPRECATE_AND_INLINE() [[deprecated]]
#else
    #define PBSL_DEPRECATE_AND_INLINE()
#endif

// Requires the compiler to prove that the size of the given object is at least
// the expected amount.
#if PBSL_HAVE_ATTRIBUTE(diagnose_if) && PBSL_HAVE_BUILTIN(__builtin_object_size)
    #define PBSL_INTERNAL_NEED_MIN_SIZE(Obj, N)                                                                        \
        __attribute__((diagnose_if(__builtin_object_size(Obj, 0) < N,                                                  \
                                   "object size provably too small "                                                   \
                                   "(this would corrupt memory)",                                                      \
                                   "error")))
#else
    #define PBSL_INTERNAL_NEED_MIN_SIZE(Obj, N)
#endif

#endif  // PBSL_BASE_MACROS_H_
