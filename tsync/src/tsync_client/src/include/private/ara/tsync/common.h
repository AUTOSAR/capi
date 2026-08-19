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
/// @file       common.h
/// @brief      common variable declarations
/// @details
/// @date       2023-02-09
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_TSYNC_COMMON_H_
#define ARA_TSYNC_COMMON_H_

#include <ara/core/string_view.h>

#include <cstdint>

#include "ara/tsync/internal/log/logger.h"

namespace ara {
namespace tsync {

static constexpr std::int32_t kRET_E1{-1};
static constexpr std::int32_t kRET_E2{-2};
static constexpr std::int32_t kRET_E3{-3};
static constexpr std::int32_t kRET_E4{-4};
static constexpr std::int32_t kRET_E5{-5};

/// @brief log output
/// @return Logger object reference
inline ara::tsync::internal::tslog::Logger &LOG() noexcept { return ara::tsync::internal::tslog::Log2Console(); }

}  // namespace tsync
}  // namespace ara

#endif  /// ARA_TSYNC_COMMON_H_