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
/// @file       error.h
/// @brief      Error handling tool set
/// @details
/// @date       2025-08-20
/// @author     xueliang.bao
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_ISOFT_UTIL_ERROR_H_
#define ARA_ISOFT_UTIL_ERROR_H_

#include <cstdint>
#include <cstring>
#include <string>

namespace isoft {
namespace utils {

/// @brief Get value of errno
/// @return errno
inline int32_t GetErrNo() noexcept { return errno; }  // PRQA S 2024,3224

/// @brief Get error cause
/// @return Error cause
inline std::string StrError() noexcept { return strerror(errno); }  // NOLINT

}  // namespace utils
}  // namespace isoft

#endif  ///< ARA_ISOFT_UTIL_ERROR_H_