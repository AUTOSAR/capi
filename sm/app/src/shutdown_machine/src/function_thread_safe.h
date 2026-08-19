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
/// @file       function_thread_safe.h
/// @brief      AutoSar-AP re-wrap some functions to make them thread-safe
/// @details    Some common methods
/// @date       2025-06-28
/// @author     Che Jinzhao
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// </table>
///
/// ================================================================

#ifndef PH_FUNCTION_THERAD_SAFE_H
#define PH_FUNCTION_THERAD_SAFE_H

#include <getopt.h>

#include <cstring>

#include "ara/core/string.h"
using Char8_t = char;
namespace ara {
namespace core {
namespace internal {
/// @brief After calling some system functions, you can capture the error code and output it through strerror
/// @brief
/// However, the buffer where the error code exists is global. In a multi-threaded state, the error code captured in real time may be overwritten by other threads, resulting in incorrect results.
/// @brief Therefore clang-tidy reported an error: function is not thread safe
/// @brief The solution is to use strerror_r to create a local buffer to store the error code, achieving thread safety
/// @param errnum Captured error code
/// @return String of the error code
inline ara::core::String safe_strerror(int32_t const errnum) noexcept
{
    constexpr uint32_t kInt32_512U{512U};
    Char8_t buff[kInt32_512U];

    // Call the GNU extended version of strerror_r
    Char8_t* ret{strerror_r(errnum, buff, sizeof(buff))};

    // If failure (nullptr) or returns an empty string, generate fallback information
    if (ret == nullptr || ret[0] == '\0') {
        snprintf(buff, sizeof(buff), "safe_strerror: Unknown error (errno=%d)", errnum);
    }

    // Directly construct a String via buff
    return ara::core::String(buff);
}
}  // namespace internal
}  // namespace core
}  // namespace ara

#endif  // PH_FUNCTION_THERAD_SAFE_H