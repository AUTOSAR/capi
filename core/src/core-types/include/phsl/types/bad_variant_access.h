// Modified by iSOFT Infrastructure Software Co., Ltd. on 2022-06-24
//
// Copyright 2018 The Abseil Authors.
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
// bad_variant_access.h
// -----------------------------------------------------------------------------
//
// This header file defines the `phsl::bad_variant_access` type.

#ifndef PBSL_TYPES_BAD_VARIANT_ACCESS_H_
#define PBSL_TYPES_BAD_VARIANT_ACCESS_H_

#include <stdexcept>

#include "phsl/base/config.h"

#ifdef PBSL_USES_STD_VARIANT

    #include <variant>

namespace phsl {
PBSL_NAMESPACE_BEGIN
using std::bad_variant_access;
PBSL_NAMESPACE_END
}  // namespace phsl

#else  // PBSL_USES_STD_VARIANT

namespace phsl {
PBSL_NAMESPACE_BEGIN

// -----------------------------------------------------------------------------
// bad_variant_access
// -----------------------------------------------------------------------------
//
// An `phsl::bad_variant_access` type is an exception type that is thrown in
// the following cases:
//
//   * Calling `phsl::get(phsl::variant) with an index or type that does not
//     match the currently selected alternative type
//   * Calling `phsl::visit on an `phsl::variant` that is in the
//     `variant::valueless_by_exception` state.
//
// Example:
//
//   phsl::variant<int, std::string> v;
//   v = 1;
//   try {
//     phsl::get<std::string>(v);
//   } catch(const phsl::bad_variant_access& e) {
//     std::cout << "Bad variant access: " << e.what() << '\n';
//   }
class bad_variant_access : public std::exception  // NOLINT : Whether to keep type name
{
public:
    bad_variant_access() noexcept = default;
    ~bad_variant_access() override;
    const char *what() const noexcept override;
};

namespace variant_internal {

[[noreturn]] PBSL_DLL void ThrowBadVariantAccess();
[[noreturn]] PBSL_DLL void Rethrow();

}  // namespace variant_internal
PBSL_NAMESPACE_END
}  // namespace phsl

#endif  // PBSL_USES_STD_VARIANT

#endif  // PBSL_TYPES_BAD_VARIANT_ACCESS_H_
