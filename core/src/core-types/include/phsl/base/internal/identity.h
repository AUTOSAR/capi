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

#ifndef PBSL_BASE_INTERNAL_IDENTITY_H_
#define PBSL_BASE_INTERNAL_IDENTITY_H_

#include "phsl/base/config.h"

namespace phsl {
PBSL_NAMESPACE_BEGIN
namespace internal {

// This is a back-fill of C++20's `std::type_identity`.
template < typename T >
struct type_identity  // NOLINT
{
    typedef T type;  // NOLINT
};

// This is a back-fill of C++20's `std::type_identity_t`.
template < typename T >
using type_identity_t = typename type_identity< T >::type;  // NOLINT

}  // namespace internal
PBSL_NAMESPACE_END
}  // namespace phsl

#endif  // PBSL_BASE_INTERNAL_IDENTITY_H_
