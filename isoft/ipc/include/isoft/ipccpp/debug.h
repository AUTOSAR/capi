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
/// @file       debug.h
/// @brief
/// @details
/// @date       2022-05-18
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================

#ifndef _ISOFT_IPC_DEBUG_H_
#define _ISOFT_IPC_DEBUG_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <string>

#include "isoft/ipc/ipc_base.h"

#define ISOFT_IPC_DEBUG_BUFF_SIZE 256

#include <iostream>
using namespace std;

#ifdef __cplusplus
extern "C"
{
#endif  //__cplusplus

#include <errno.h>
#include <stdio.h>
#include <string.h>

#define ERROR_DEBUG(msg)                                                                                               \
    do {                                                                                                               \
        (void)printf("[ipc.debug] %s:%d [%s] -> %s | [%d].%s\n", __FILE__, __LINE__, __FUNCTION__, msg, errno,         \
                     strerror(errno));                                                                                 \
    } while (0)

// (void)printf("%s@%s->%s+%d:[%d]%s\n", msg, __FILE__, __FUNCTION__, __LINE__, errno, \
        //              strerror(errno));                                                                   \

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus

namespace isoft {
namespace ipc {
namespace debug {

/// @brief Initialize the IPC environment
/// @retval 0 - Success
/// @retval <0 - Failure
inline int IPCEnvInit(void)
{
    //Initialize IPC
    // //Clean up the folder used for IPC communication
    // if (0 != isoft::ipc::IPCBase::Destroy()) {
    //     ERROR_DEBUG("main->isoft::ipc::IPCBase::Destroy()");
    //     return -1;
    // }
    //Create the folder used for IPC communication
    if (0 != isoft::ipc::IPCBase::Init()) {
        ERROR_DEBUG("main->isoft::ipc::IPCBase::Init()");
        return -2;
    }

    return 0;
}

/// @brief Clean up the IPC environment
/// @retval 0 - Success
/// @retval <0 - Failure
inline int IPCEnvDestroy(void)
{
    //Initialize IPC
    //Clean up the folder used for IPC communication
    if (0 != isoft::ipc::IPCBase::Destroy()) {
        ERROR_DEBUG("main->isoft::ipc::IPCBase::Destroy()");
        return -1;
    }

    return 0;
}

/// @brief Create an inter-process communication environment
/// @param procName - Process name
/// @param ipcEnvStr - Generated IPC environment variable string, used for inheritance by child processes
/// @param isSetSysEnv - true, will set the current system environment variable; false, will not update the system environment variable
/// @retval >= 0 - Success, opened file descriptor
/// @retval <0 - Failure

inline int CreateIPC(const std::string &procName, std::string &ipcEnvStr, bool isSetSysEnv)
{
    int retCode;
    char ipcName[ISOFT_IPC_DEBUG_BUFF_SIZE];

    //Generate IPC address name based on process name
    retCode = isoft::ipc::IPCBase::ResolveName(procName.c_str(), ipcName, ISOFT_IPC_DEBUG_BUFF_SIZE);
    if (0 != retCode) {
        ERROR_DEBUG("IPCBase::ResolveName()");
        return -3;
    }

    //Create IPC socket
    int fd = isoft::ipc::IPCBase::CreateSocket(ipcName);
    // printf( "ipcName = %s, fd = %d \n", ipcName, fd );
    if (0 > fd) {
        ERROR_DEBUG("IPCBase::CreateSocket()");
        return -4;
    }

    char envBuf[ISOFT_IPC_DEBUG_BUFF_SIZE];
    //Create IPC environment variable to pass to child process
    if (0 != isoft::ipc::IPCBase::CreateFdEnv(fd, envBuf, ISOFT_IPC_DEBUG_BUFF_SIZE)) {
        ERROR_DEBUG("IPCBase::CreateFdEnv()");
        return -5;
    }

    ipcEnvStr = envBuf;

    snprintf(envBuf, ISOFT_IPC_DEBUG_BUFF_SIZE, "%d", fd);
    if (isSetSysEnv) {
        if (0 != setenv(isoft::ipc::IPCBase::kEnvKeySocketFd, envBuf, 1)) {
            ERROR_DEBUG("setenv()");
            return -6;
        }
    }

    return fd;
}

inline int CreateIPC(const std::string &procName, std::string &ipcEnvStr)
{
    return CreateIPC(procName, ipcEnvStr, false);
}

inline int CreateIPC(const std::string &&procName, std::string &ipcEnvStr)
{
    return CreateIPC(procName, ipcEnvStr, false);
}

inline int CreateIPC(const std::string &procName)
{
    std::string ipcEnvStr;
    return CreateIPC(procName, ipcEnvStr, true);
}

inline int CreateIPC(const std::string &&procName)
{
    std::string ipcEnvStr;
    return CreateIPC(procName, ipcEnvStr, true);
}

//@brief Clean up the inter-process communication environment
//@param
//      procName, process name
//      isSetSysEnv, true, will clean up the current system environment variable; false, will not update the system environment variable
//@returns Success, 0; Failure, < 0;

inline int DestroyIPC(const std::string &procName, bool isSetSysEnv)
{
    int retCode = 0;
    char ipcName[ISOFT_IPC_DEBUG_BUFF_SIZE];

    //Generate IPC address name based on process name
    retCode = isoft::ipc::IPCBase::ResolveName(procName.c_str(), ipcName, ISOFT_IPC_DEBUG_BUFF_SIZE);
    if (0 != retCode) {
        ERROR_DEBUG("IPCBase::ResolveName()");
        retCode = -1;
        goto L_END;
    }

    if (0 != unlink(ipcName)) {
        ERROR_DEBUG(ipcName);
        retCode = -2;
        goto L_END;
    }

    if (isSetSysEnv) {
        if (0 != unsetenv(isoft::ipc::IPCBase::kEnvKeySocketFd)) {
            ERROR_DEBUG("unsetenv()");
            retCode = -3;
            goto L_END;
        }
    }

L_END:

    return retCode;
}

inline int DestroyIPC(const std::string &procName) { return DestroyIPC(procName, false); }

inline int DestroyIPC(const std::string &&procName) { return DestroyIPC(procName, false); }

//********************************/
class MIpcDebugInit
{
public:
    explicit MIpcDebugInit(const std::string &stSocketPath)
    {
        if (false == IsHaveInit()) {
            stSocketPath_ = stSocketPath.data();
            //Initialize IPC
            int nReturn = isoft::ipc::debug::IPCEnvInit();
            if (0 != nReturn) {
                perror("IPCEnvInit()");
                nErrorCode_ = nReturn;
                return;
            }
            // 2022-10-17 Modified to clear possible old files before creating socket
            isoft::ipc::debug::DestroyIPC(stSocketPath_);
            // std::string stIpcEnvStr;
            // nReturn = isoft::ipc::debug::CreateIPC(stSocketPath_, stIpcEnvStr, false);
            nReturn = isoft::ipc::debug::CreateIPC(stSocketPath_);
            if (nReturn < 0) {
                perror("CreateIPC()");
                nErrorCode_ = nReturn;
                return;
            }
        }
    }
    ~MIpcDebugInit()
    {
        if (stSocketPath_.size() > 0) {
            isoft::ipc::debug::DestroyIPC(stSocketPath_);
        }
        // isoft::ipc::debug::IPCEnvDestroy();  //2022-10-17 Modified by Han Yuxin, no longer automatically "delete IPC folder"
    }
    bool IsHaveError() const { return nErrorCode_; }
    bool IsHaveInit() const
    {
        char ipcName[ISOFT_IPC_DEBUG_BUFF_SIZE] = {0};
        char *pchIpcName                        = getenv(isoft::ipc::IPCBase::kEnvKeySocketFd);
        if (nullptr == pchIpcName) {
            return false;
        }
        if (strlen(pchIpcName) <= 0) {
            return false;
        }
        return true;
    }

protected:
    std::string stSocketPath_;
    int nErrorCode_ = 0;
};
//********************************/
}  // namespace debug
}  // namespace ipc
}  // namespace isoft

#endif  // _ISOFT_IPC_DEBUG_H_
