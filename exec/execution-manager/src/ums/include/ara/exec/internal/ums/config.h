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
/// @file       config.h
/// @brief      Update client communication protocol service configuration
/// @details
/// @date       2023-04-01
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/ExecutionManagement/UMS
/// @interface_level=none
/// @endcode
///
/// ================================================================

#ifndef _ARA_EXEC_INTERNAL_UMS_SERVICE_CONFIG_H_
#define _ARA_EXEC_INTERNAL_UMS_SERVICE_CONFIG_H_

#include <unistd.h>

#include <chrono>
#include <cstdint>

namespace ara {
namespace exec {
namespace internal {
namespace ums {

/// @brief char type redefinition
using Char8_t = char;

/// @brief Get SMS service name
/// @return SMS service name
/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
inline constexpr Char8_t const* GetServiceName() noexcept { return "UMS_"; }

}  // namespace ums
}  // namespace internal
}  // namespace exec
}  // namespace ara
#endif  ///< _ARA_EXEC_INTERNAL_UMS_SERVICE_CONFIG_H_
