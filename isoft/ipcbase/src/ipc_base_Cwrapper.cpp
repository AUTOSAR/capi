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
/// @file       ipc_base_Cwrapper.cpp
/// @brief
/// @details
/// @date       2021-11-30
/// @author     fengjian
/// @version    1.2.0
///
/// ================================================================

#include <stdio.h>

#include "isoft/ipc/ipc_base.h"
#include "isoft/ipc/ipc_base_C.h"
#include "nai/os/nai_file.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /// @brief Get the socket file descriptor used for inter-process communication.
    /// @return NAI_FD_INVALID, failure, and set nai_errno; nai_fd_t, success.
    /// @note The socket returned by this function is a datagram-oriented UNIX domain socket and has been bound.
    ////
    nai_fd_t ipc_base_socket_get(void) { return isoft::ipc::IPCBase::GetSocket(); }

    /// @brief Create a socket for IPC communication.
    /// @return NAI_FD_INVALID, failure, and set nai_errno; nai_fd_t, success.
    /// @note The socket returned by this function is a datagram-oriented UNIX domain socket and has been bound.
    ////
    nai_fd_t ipc_base_socket_create(const char *socketName) { return isoft::ipc::IPCBase::CreateSocket(socketName); }

    ///
    /// @brief Resolve a process name to a socket address.
    /// @param processName, process name
    ///        socketName, buffer to store the socket address
    ///        nameLen, buffer size
    /// @return 0, success; -1, failure.
    /// @note  nameLen must be at least 108 bytes to ensure receiving the complete socketName.
    ///        The current implementation of this function is based on a static algorithm, so it cannot verify the correctness of processName.
    ///        Therefore, reasons for call failure are:
    ///            1. Invalid processName address; 2. Invalid buffer address; 3. Buffer too small.
    ////
    int ipc_base_socket_resolve_name(const char *process_name, char *ipc_name, unsigned int name_len)
    {
        if (0 != isoft::ipc::IPCBase::ResolveName(process_name, ipc_name, name_len))
            return -1;

        return 0;
    }

    ///
    /// @brief Generate an environment variable string for IPC communication.
    /// @param fd, file descriptor for IPC communication;
    ///        envBuf, buffer to store the generated environment variable string;
    ///        bufSize, buffer size;
    /// @returns 0, success; -1, failure.
    /// @note  Possible reasons for call failure:
    ///        1. The passed fd value is < 0;
    ///        2. The buffer address is NULL;
    ///        3. The buffer is too small to hold the entire environment variable string.
    /// @note  The buffer size must be at least 32 bytes.
    ////
    int ipc_base_socket_create_env(nai_fd_t fd, char *envBuf, unsigned int bufSize)
    {
        return isoft::ipc::IPCBase::CreateFdEnv(fd, envBuf, bufSize);
    }

#ifdef __cplusplus
}
#endif
