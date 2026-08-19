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
/// @file       ipc_base.cpp
/// @brief
/// @details
/// @date       2021-11-30
/// @author     fengjian
/// @version    1.2.0
///
/// ================================================================

#include "isoft/ipc/ipc_base.h"

#include <fcntl.h>
#include <ftw.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <iostream>
#include <string>

#include "isoft/ara_fsh/filesystem_hierarchy.h"
#include "nai/os/nai_file.h"
#include "nai/os/nai_proc.h"
#include "nai/os/nai_socket.h"
#include "nai/runtime/nai_errno.h"

#define ISOFT_IPCBASE_UNIX_SOCK_MAX_LEN 108
#define ISOFT_IPCBASE_ENV_BUFF_SIZE     1024
#define PATH_BUFF_SIZE                  1024

namespace {

int MakeDir(const std::string &path)
{
    struct stat statBuf;
    std::string errMsg;
    std::string absolutePath;
    static char pathBuff[PATH_BUFF_SIZE];

    if (path.empty()) {
        return -1;
    }

    if (NULL == getcwd(pathBuff, PATH_BUFF_SIZE)) {
        return -1;
    }
    if ('/' == path.c_str()[0]) {
        absolutePath = path;
    } else if ('.' == path.c_str()[0]) {
        if (path.size() < 2) {
            return -1;
        }
        if ('/' == path.c_str()[1]) {
            absolutePath = pathBuff + std::string("/") + path.substr(2, path.size());
        } else {
            absolutePath = pathBuff + std::string("/") + path;
        }
    } else {
        absolutePath = pathBuff + std::string("/") + path;
    }

    if (0 != stat(absolutePath.c_str(), &statBuf)) {
        if (ENOENT == errno) {
            // Error reported because directory does not exist
            char *p = strdup(absolutePath.c_str());
            if (NULL == p) {
                errMsg = "ERROR on strdup(" + path + ")";
                perror(errMsg.c_str());
                return -1;
            }
            std::string baseName = basename(p);
            free(p);
            p                   = strdup(absolutePath.c_str());
            std::string dirName = dirname(p);
            free(p);
            // cout << dirName << "-" << baseName << endl;
            if (0 != MakeDir(dirName)) {
                return -2;
            }
            if (0 != mkdir(absolutePath.c_str(), 0755)) {
                errMsg = "ERROR on mkdir(" + path + ")";
                perror(errMsg.c_str());
                return -3;
            }
        } else {
            // Other permission-related errors
            std::string eMsg = "ERROR on state(";
            eMsg += absolutePath + ")";
            perror(eMsg.c_str());
            return -4;
        }
    }

    return 0;
}

int FTWCb(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf)
{
    (void)(sb);
    (void)(typeflag);
    (void)(ftwbuf);

    if (0 != remove(fpath)) {
        if (ENOENT != errno) {
            std::string errMsg = "ERROR on remove ";
            errMsg += fpath;
            perror(errMsg.c_str());
        }
    }
    return FTW_CONTINUE;
}

int RemoveDir(const std::string &path)
{
    if (0 != nftw(path.c_str(), FTWCb, 1, FTW_DEPTH)) {
        if (ENOENT != errno) {
            std::string errMsg = "ERROR on nftw(" + path + ")";
            perror(errMsg.c_str());
            return -1;
        }
    }

    return 0;
}

}  // namespace

namespace isoft {
namespace ipc {

const static constexpr char *kEnvKeyValueSeparator = "=";

/// @brief init ipc env
/// @return 0, success; !0 failure
////
int IPCBase::Init(void)
{
    isoft::ara_fsh::Platform fsh;
    std::string ipcDir = fsh.GetUnixDomainSocketDir();
    if (true == ipcDir.empty()) {
        perror("fsh.GetUnixDomainSocketDir() is empty.");
        return -1;
    }

    return MakeDir(ipcDir);
}

/// @brief destroy ipc env
/// @return 0, success; !0 failure
////

int IPCBase::Destroy(void)
{
    isoft::ara_fsh::Platform fsh;
    std::string ipcDir = fsh.GetUnixDomainSocketDir();
    if (true == ipcDir.empty()) {
        perror("fsh.GetUnixDomainSocketDir() is empty.");
        return -1;
    }

    return RemoveDir(ipcDir);
}

/// @brief Create a socket for IPC communication.
/// @return NAI_FD_INVALID, failure, and set nai_errno; nai_fd_t, success.
/// @note The socket returned by this function is a datagram-oriented UNIX domain socket and has been bound.
////
nai_fd_t IPCBase::CreateSocket(const char *socketName)
{
    if (NULL == socketName) {
        nai_errno = NAI_E_INVALID_ARG;
        return -1;
    }
    nai_sockaddr_un_t sockAddr;
#if 1
    sockAddr.sun_family = AF_UNIX;
    strncpy(sockAddr.sun_path, socketName, ISOFT_IPCBASE_UNIX_SOCK_MAX_LEN);
#else
    // TODO: There is a bug to be fixed. The generated UnixSocketName does not have a trailing \0.
    int socklen = sizeof(nai_sockaddr_un_t);
    nai_sockaddr_mk_unix(socketName, -1, reinterpret_cast< nai_sockaddr_t * >(&sockAddr), &socklen);
#endif

    // Create socket directory

    nai_fd_t sockFd = nai_sock_open(AF_UNIX, SOCK_DGRAM, 0);
    if (NAI_FD_INVALID == sockFd)
        return NAI_FD_INVALID;

    int flag = 1;
    if (0 > nai_sock_setsockopt(sockFd, SOL_SOCKET, SO_PASSCRED, (char *)&flag, (int)sizeof(flag))) {
        return NAI_FD_INVALID;
    }

    if (0 > nai_sock_bind(sockFd, reinterpret_cast< const nai_sockaddr_t * >(&sockAddr), sizeof(nai_sockaddr_un_t))) {
        return NAI_FD_INVALID;
    }

    if (0 > nai_file_cloexec(sockFd, 0))
        return NAI_FD_INVALID;

    return sockFd;
}

///
/// @brief Get the socket file descriptor used for inter-process communication.
/// @return NAI_FD_INVALID, failure, and set nai_errno; nai_fd_t, success.
/// @note The socket returned by this function is a datagram-oriented UNIX domain socket and has been bound.
////
nai_fd_t IPCBase::GetSocket()
{
    char *invalidPtr = NULL;
    char envBuf[ISOFT_IPCBASE_ENV_BUFF_SIZE];

    nai_fd_t fd;
    if (0 > nai_get_env(kEnvKeySocketFd, envBuf, ISOFT_IPCBASE_ENV_BUFF_SIZE)) {
        return -1;
    }

    fd = strtol(envBuf, &invalidPtr, 10);
    if (*invalidPtr != '\0' || fd < 0)
        return -1;

    return fd;
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
int IPCBase::CreateFdEnv(nai_fd_t fd, char *envBuf, unsigned int bufSize)
{
    unsigned int n;

    if (fd < 0 || NULL == envBuf)
        return -1;

    n = snprintf(envBuf, bufSize, "%s=%d", kEnvKeySocketFd, fd);
    if (n > bufSize)
        return -1;

    return 0;
}

///
/// @brief Resolve a process name to a socket address.
/// @param processName, process name
///        socketName, buffer to store the socket address
///        nameLen, buffer size
/// @return 0, success; non-zero, failure.
/// @note  nameLen must be at least 108 bytes to ensure receiving the complete socketName. If the buffer is too small, the string will be truncated.
/// @errors
///         -1, ipcName or processName address is NULL.
///         -2, unable to obtain the IPC directory path (due to environment variables).
////
int IPCBase::ResolveName(const char *processName, char *ipcName, unsigned int ipcNameLen)
{
    if (NULL == ipcName || NULL == processName || 0 == ipcNameLen)
        return -1;

    isoft::ara_fsh::Platform fsh;
    std::string ipcDir = fsh.GetUnixDomainSocketDir();
    if (true == ipcDir.empty()) {
        perror("fsh.GetUnixDomainSocketDir() is empty, using Default.");
    }

    int rlen = ipcNameLen - 1;  // Reserve space for \0
    strncpy(ipcName, ipcDir.c_str(), rlen);
    rlen -= ipcDir.size();

    strncat(ipcName, "/", rlen);
    rlen -= strlen("/");

    strncat(ipcName, processName, rlen);

    ipcName[ipcNameLen] = '\0';
    return 0;
}

}  // namespace ipc
}  // namespace isoft
