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
/// @file       therad_safe.h
/// @brief      Autosar-AP re-encapsulates some functions to achieve thread safety
/// @details    Some common methods
/// @date       2025-07-11
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

#ifndef THERAD_SAFE_H
#define THERAD_SAFE_H

#include <cstring>

#include "../serialization/serialization/common_data_type.h"
#include "ara/core/string.h"

namespace isoft {
namespace threadsafe {
using Char8_t = char;
/// @brief When calling some system functions, the error code can be captured and output through strerror
/// @brief
/// But the buffer where the error code exists is global. In a multi-threaded state, the error code captured in real time may be overwritten by other threads, resulting in inability to obtain the correct result
/// @brief Therefore clang-tidy reported an error: function is not thread safe
/// @brief The solution is to use strerror_r, create a local buffer to store the error code, achieving thread safety
/// @param errnum Captured error code
/// @return String of the error code
inline ara::core::String Safe_Strerror(int32_t const errnum) noexcept
{
    Char8_t buff[isoft::serialize::kInt32_512];

    // Call the GNU extended version of strerror_r
    Char8_t* ret{strerror_r(errnum, buff, sizeof(buff))};

    // If it fails (nullptr) or returns an empty string, generate fallback information
    if (ret == nullptr || ret[0] == '\0') {
        snprintf(buff, sizeof(buff), "safe_strerror: Unknown error (errno=%d)", errnum);
        return ara::core::String(buff);
    }

    // Construct String directly through buff
    return ara::core::String(ret);
}
}  // namespace threadsafe
}  // namespace isoft

#endif  // THERAD_SAFE_H