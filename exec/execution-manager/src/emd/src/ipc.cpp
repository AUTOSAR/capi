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
/// @file       ipc.cpp
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
/// @unit_name=ExecutionManager
/// @endcode
///
/// ================================================================

#include "ara/exec/internal/emd/ipc.h"

#include <isoft/ipc/ipc_base.h>

#include <array>

#include "ara/exec/internal/emd/log.h"
#include "isoft/utils/error.h"

/// @brief IPC name length
/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
#define IPC_NAME_LENGTH 256U

namespace ara {
namespace exec {
namespace internal {
namespace emd {

/// @brief Initialize IPC
/// @return 0 success; <0 failure
/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
int32_t InitIPC() noexcept
{
    /// It needs to be ensured that platform initialization has been completed. For emd, platform initialization has been completed in previous steps
    /// Clean up the ipc environment to prevent the previous uncleaned environment from affecting the current one
    if (0 != isoft::ipc::IPCBase::Destroy()) {
        perror("IPCBase::Destroy(): ");
        return -1;
    }

    /// Then create the ipc environment
    if (0 != isoft::ipc::IPCBase::Init()) {
        std::cerr << "CreateIPC::IPCBase::Init()";
        return -1;
    }

    return 0;
}

/// @brief Create an interprocess communication environment
/// @param procName Process name
/// @param ipcEnvStr Generated IPC environment variable string, used to inherit to child processes
/// @param isSetSysEnv Whether to set the current system environment variables
/// @return >=0 opened file descriptor; <0 failure
/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
int32_t CreateIPC(ara::core::String const &procName, ara::core::String &ipcEnvStr, bool const isSetSysEnv) noexcept
{
    // Generate an ipc address name based on the process name
    std::array< Char8_t, IPC_NAME_LENGTH > ipcName{};
    int32_t const retCode{isoft::ipc::IPCBase::ResolveName(procName.c_str(), ipcName.data(), IPC_NAME_LENGTH)};
    if (0 != retCode) {
        std::ostringstream oss;
        oss << "IPCBase::ResolveName(" << procName << "): ";
        perror(oss.str().c_str());
        return -1;
    }

    // Create the ipc socket
    int32_t const fd{isoft::ipc::IPCBase::CreateSocket(ipcName.data())};
    if (0 > fd) {
        std::ostringstream oss;
        oss << "IPCBase::CreateSocket(" << ipcName.data() << "): ";
        perror(oss.str().c_str());
        return -1;
    }

    std::array< Char8_t, IPC_NAME_LENGTH > envBuf{};
    // Create an IPC environment variable to pass to the child process
    if (0 != isoft::ipc::IPCBase::CreateFdEnv(fd, envBuf.data(), IPC_NAME_LENGTH)) {
        std::ostringstream oss;
        oss << "IPCBase::CreateFdEnv(" << static_cast< int32_t >(fd) << "," << envBuf.data() << "): ";
        perror(oss.str().c_str());
        return -1;
    }
    std::ignore = ipcEnvStr;
    ipcEnvStr   = envBuf.data();

    if (isSetSysEnv) {
        std::ignore = snprintf(envBuf.data(), IPC_NAME_LENGTH, "%d", fd);
        if (0 != setenv(isoft::ipc::IPCBase::kEnvKeySocketFd, envBuf.data(), 1)) {  // NOLINT
            std::ostringstream oss;
            oss << "setenv(" << isoft::ipc::IPCBase::kEnvKeySocketFd << "," << envBuf.data() << "): ";
            perror(oss.str().c_str());
            return -1;
        }
    }

    return fd;
}

/// @brief Clean up the interprocess communication environment
/// @param procName Process name
/// @param isSetSysEnv Whether to set the current system environment variables
/// @return 0 success; <0 failure
/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
int32_t DestroyIPC(ara::core::String const &procName, bool const isSetSysEnv) noexcept
{
    // Generate an ipc address name based on the process name
    std::array< Char8_t, IPC_NAME_LENGTH > ipcName{};
    int32_t retCode{isoft::ipc::IPCBase::ResolveName(procName.c_str(), ipcName.data(), IPC_NAME_LENGTH)};
    if (0 == retCode) {
        if (0 != unlink(ipcName.data())) {
            if (ENOENT != isoft::utils::GetErrNo()) {
                LOGE() << "unlink(" << ipcName.data() << ") failed !!!";
                retCode = -1;
            } else {
                retCode = 0;
            }
        }
    } else {
        LOGE() << "IPCBase::ResolveName(" << procName << ") failed !!!";
        retCode = -1;
    }

    if (isSetSysEnv) {
        if (0 != unsetenv(isoft::ipc::IPCBase::kEnvKeySocketFd)) {  // NOLINT
            LOGE() << "unsetenv(" << isoft::ipc::IPCBase::kEnvKeySocketFd << ") failed !!!";
            retCode = -1;
        }
    }

    return retCode;
}

}  // namespace emd
}  // namespace internal
}  // namespace exec
}  // namespace ara
