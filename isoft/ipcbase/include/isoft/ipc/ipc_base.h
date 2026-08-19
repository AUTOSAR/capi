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
/// @file       ipc_base.h
/// @brief
/// @details
/// @date       2021-11-30
/// @author     fengjian
/// @version    1.2.0
///
/// ================================================================

#ifndef _IPC_BASE_H_
#define _IPC_BASE_H_

#include "nai/os/nai_file.h"

namespace isoft {
namespace ipc {

class IPCBase
{
public:
    //@brief Environment variable used to save the created IPC communication handle (file descriptor)
    const static constexpr char *kEnvKeySocketFd = "ENV_IPC_SOCKET_FD";

    IPCBase()  = default;
    ~IPCBase() = default;

    /// @brief init ipc env
    /// @return 0, success; !0 failure
    ////

    static int Init(void);

    /// @brief destroy ipc env
    /// @return 0, success; !0 failure
    ////

    static int Destroy(void);

    /// @brief Create a socket for IPC communication.
    /// @param  name, Unix local communication domain address name
    /// @return NAI_FD_INVALID, failure, and set nai_errno; nai_fd_t, success.
    /// @note The socket returned by this function is a datagram-oriented UNIX domain socket and has been bound.
    ////
    static nai_fd_t CreateSocket(const char *name);

    /// @brief Get the socket file descriptor used for inter-process communication.
    /// @return NAI_FD_INVALID, failure, and set nai_errno; nai_fd_t, success.
    /// @note The socket returned by this function is a datagram-oriented UNIX domain socket and has been bound.
    ////
    static nai_fd_t GetSocket(void);

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
    static int CreateFdEnv(int fd, char *envBuf, unsigned int buffSize);

    /// @brief Resolve a process name to a socket address.
    /// @param processName, process name
    ///        socketName, buffer to store the socket address
    ///        nameLen, buffer size
    /// @return 0, success; non-zero, failure.
    /// @note  nameLen must be at least 108 bytes to ensure receiving the complete socketName. If the buffer is too small, the string will be truncated.
    ///        The current implementation of this function is based on a static algorithm, so it cannot verify the correctness of processName. Therefore, the only reason for call failure is that the buffer is too small.
    ////
    static int ResolveName(const char *processName, char *ipcName, unsigned int nameLen);

};  // class IPCBase

}  // namespace ipc
}  // namespace isoft
#endif  //_IPC_BASE_H_
