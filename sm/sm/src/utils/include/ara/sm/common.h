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
/// @brief      Define macros and string constants used by both Client and Server
/// @details
/// @date       2024-06-25
/// @author     lianglongxiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/StateManagement/Utils
/// @unit_name=Common
/// @interface_level=uint
/// @unit_description=Define macros and string constants used by both Client and Server
/// @trace_id_sr=SR_SM_02001, SR_SM_02002
/// @endcode
///
/// ================================================================

#ifndef ARA_SM_INTERNAL_COMMON_H_
#define ARA_SM_INTERNAL_COMMON_H_

/// @brief gcc built-in function, helps compiler with branch optimization
/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
#define likely(x) __builtin_expect(!!(x), 1)

/// @brief gcc built-in function, helps compiler with branch optimization
/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
#define unlikely(x) __builtin_expect(!!(x), 0)

/// @brief Macro for outputting exception information
/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
#define ipc_expect(condition, format, ...)                                                                             \
    ((condition)                                                                                                       \
         ? 0 == 0                                                                                                      \
         : (fprintf(stderr, "%s|%d|%s|" format "\r\n", __FILE__, __LINE__, strerror(errno), ##__VA_ARGS__), 0 == 1))

namespace ara {
namespace sm {
namespace fg_state_ipc {

/// @brief Define alias to resolve qac2428 item: Direct use of character type.
/// @param Char8_t alias of char
/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
using Char8_t = char;

/// @brief IPC name for communication between event notification client and SM
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_sr=SR_SM_02001, SR_SM_02002
/// @trace_id_ad=AD_SM_00443
/// @trace_id_dd=DD_SM_00459
/// @needwork = ad
/// @endcode
inline constexpr Char8_t const *Get_SM_APP_NAME() noexcept { return "smd"; }

/// @brief Event notification server name
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_sr=SR_SM_02001, SR_SM_02002
/// @trace_id_ad=AD_SM_00444
/// @trace_id_dd=DD_SM_00460
/// @needwork = ad
/// @endcode
inline constexpr Char8_t const *Get_SM_IPC_SERVER_NAME() noexcept { return "smnm"; }

/// @brief Error message - success
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_sr=SR_SM_02001, SR_SM_02002
/// @trace_id_ad=AD_SM_00445
/// @trace_id_dd=DD_SM_00461
/// @needwork = ad
/// @endcode
inline constexpr Char8_t const *Get_ERROR_INFO_SUCCESS() noexcept { return "success"; }

}  // namespace fg_state_ipc
}  // namespace sm
}  // namespace ara

#endif