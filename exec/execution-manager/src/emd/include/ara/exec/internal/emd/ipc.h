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
/// @file       ipc.h
/// @brief      IPC operation method definition
/// @details
/// @date       2023-04-01
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/ExecutionManagement/Emd
/// @interface_level=none
/// @endcode
///
/// ================================================================

#ifndef _ARA_EXEC_INTERNAL_IPC_H_
#define _ARA_EXEC_INTERNAL_IPC_H_
#include <ara/core/string.h>
#include <ara/core/string_view.h>

#include <cstdint>

namespace ara {
namespace exec {
namespace internal {
namespace emd {

/// @brief char redefinition
using Char8_t = char;

/// @brief Initialize IPC
/// @return 0 success; <0 failure
/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
int32_t InitIPC() noexcept;

/// @brief Create an interprocess communication environment
/// @param procName Process name
/// @param ipcEnvStr Generated IPC environment variable string, used to inherit to child processes
/// @param isSetSysEnv Whether to set the current system environment variables
/// @return >=0 opened file descriptor; <0 failure
/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
int32_t CreateIPC(ara::core::String const &procName, ara::core::String &ipcEnvStr, bool const isSetSysEnv) noexcept;

/// @brief Create an interprocess communication environment
/// @param procName Process name
/// @param ipcEnvStr Generated IPC environment variable string, used to inherit to child processes
/// @return >=0 opened file descriptor; <0 failure
/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
inline int32_t CreateIPC(ara::core::StringView const &procName, ara::core::String &ipcEnvStr) noexcept
{
    ara::core::String const name{procName};
    return CreateIPC(name, ipcEnvStr, false);
}

/// @brief Create an interprocess communication environment
/// @param procName Process name
/// @param ipcEnvStr Generated IPC environment variable string, used to inherit to child processes
/// @return >=0 opened file descriptor; <0 failure
/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
inline int32_t CreateIPC(Char8_t const *const procName, ara::core::String &ipcEnvStr) noexcept
{
    return CreateIPC(ara::core::StringView{procName}, ipcEnvStr);
}

/// @brief Create an interprocess communication environment
/// @param procName Process name
/// @param ipcEnvStr Generated IPC environment variable string, used to inherit to child processes
/// @return >=0 opened file descriptor; <0 failure
/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
inline int32_t CreateIPC(ara::core::String const &procName, ara::core::String &ipcEnvStr) noexcept
{
    return CreateIPC(procName, ipcEnvStr, false);
}

/// @brief Create an interprocess communication environment
/// @param procName Process name
/// @return >=0 opened file descriptor; <0 failure
/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
inline int32_t CreateIPC(ara::core::String const &procName) noexcept
{
    ara::core::String ipcEnvStr;
    return CreateIPC(procName, ipcEnvStr, true);
}

/// @brief Create an interprocess communication environment
/// @param procName Process name
/// @return >=0 opened file descriptor; <0 failure
/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
inline int32_t CreateIPC(ara::core::StringView const &procName) noexcept
{
    ara::core::String ipcEnvStr;
    ara::core::String const name{procName};
    return CreateIPC(name, ipcEnvStr, true);
}

/// @brief Create an interprocess communication environment
/// @param procName Process name
/// @return >=0 opened file descriptor; <0 failure
/// @exception std::runtime_error If creation fails
/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
inline int32_t CreateIPC(Char8_t const *const procName) { return CreateIPC(ara::core::StringView(procName)); }

/// @brief Clean up the interprocess communication environment
/// @param procName Process name
/// @param isSetSysEnv Whether to set the current system environment variables
/// @return 0 success; <0 failure
/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
int32_t DestroyIPC(ara::core::String const &procName, bool const isSetSysEnv) noexcept;

/// @brief Clean up the interprocess communication environment
/// @param procName Process name
/// @return 0 success; <0 failure
/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
inline int32_t DestroyIPC(ara::core::String const &procName) noexcept { return DestroyIPC(procName, false); }

/// @brief Clean up the interprocess communication environment
/// @param procName Process name
/// @return 0 success; <0 failure
/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
inline int32_t DestroyIPC(ara::core::StringView const &procName) noexcept
{
    ara::core::String const name{procName};
    return DestroyIPC(name, false);
}

/// @brief Clean up the interprocess communication environment
/// @param procName Process name
/// @return 0 success; <0 failure
/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
inline int32_t DestroyIPC(Char8_t const *const procName) noexcept
{
    return DestroyIPC(ara::core::StringView(procName));
}

}  // namespace emd
}  // namespace internal
}  // namespace exec
}  // namespace ara

#endif  ///< _ARA_EXEC_INTERNAL_IPC_INIT_H_
