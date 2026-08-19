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
/// @file       sysconfig.h
/// @brief      System global configuration file
/// @details
/// @date       2023-03-06
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/ExecutionManagement/Config
/// @interface_level=none
/// @endcode
///
/// ================================================================

#ifndef _ARA_EXEC_INTERNAL_CONFIG_SYSCONF_H_
#define _ARA_EXEC_INTERNAL_CONFIG_SYSCONF_H_

#include <ara/core/string.h>

namespace ara {
namespace exec {
namespace internal {
namespace config {

/// @brief Redefine char type
using Char8_t = char;

/// @brief Execution manager IPC name
/// @return emd ipc
/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
inline constexpr Char8_t const* GetExecutionManagerIpcName() noexcept { return "emd"; }

/// @brief MachineFG name
/// @return machine fg name
/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
inline constexpr Char8_t const* GetMachineFunctionGroupName() noexcept { return "MachineFG"; }

/// @brief Startup state of MachineFG
/// @return startup state
/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
inline constexpr Char8_t const* GetFunctionGroupStateStartup() noexcept { return "Startup"; }

/// @brief Off state of all function groups
/// @return Off state
/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
inline constexpr Char8_t const* GetFunctionGroupStateOff() noexcept { return "Off"; }

/// @brief Undefined state of all function groups
/// @return Off state
/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
static constexpr Char8_t const* GetFunctionGroupStateUndefined() noexcept { return "Undefined"; }

}  // namespace config
}  // namespace internal
}  // namespace exec
}  // namespace ara
#endif  ///< _ARA_EXEC_INTERNAL_CONFIG_SYSCONF_H_
