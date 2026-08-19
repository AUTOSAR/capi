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
/// @brief
/// @details
/// @date       2022-06-24
/// @author     hanzhibo
/// @version    1.2.0
///
/// ================================================================

#ifndef ISOFT_CORE_IPC_IPC_H_
#define ISOFT_CORE_IPC_IPC_H_

#include <ara/core/result.h>
#include <ara/core/string.h>
#include <isoft/ara_fsh/process.h>
#include <isoft/ipc/ipc_base.h>
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <sys/stat.h>
#include <unistd.h>

#include <cerrno>
#include <cstdlib>
#include <fstream>
#include <iostream>

#include "ipc_error_domain.h"

namespace isoft {
namespace core {

/// @brief the fd of unix socket of this process
int32_t &GetIpcFileFd() noexcept
{
    static int32_t s_FD{0};
    return s_FD;
}
ara::core::String &GetIpcTotalPath() noexcept
{
    /// @brief example: /run/ara/xxx/ipc/phmd
    static ara::core::String s_IpcTotalPath{""};
    return s_IpcTotalPath;
}

class Ipc
{
public:
    /// @brief ipc initialization
    /// @return empty, success; non-empty, failure
    static ara::core::Result< void > Initialize()
    {
        if (IpcInited()) {
            return ara::core::Result< void >::FromValue();
        }
        return Open();
    }

    /// @brief ipc deinitialization
    /// @return empty, success; non-empty, failure
    static ara::core::Result< void > Deinitialize()
    {
        // Close();
        return ara::core::Result< void >::FromValue();
    }

private:
    /// @brief Create directory: /run/ara/xxx/ipc
    /// @param[in] path Directory to create
    /// @return 0, created successfully; -1, creation failed
    static int32_t CreateDir(const ara::core::String &path)
    {
        constexpr mode_t kMakeDirMode{0755};
        size_t pos = 0;
        ara::core::String subPath;

        while ((pos = path.find('/', pos)) != ara::core::String::npos) {
            subPath = path.substr(0, pos++);
            if (subPath.empty()) {
                continue;
            }

            if (mkdir(const_cast< char * >(subPath.c_str()), kMakeDirMode) != 0 && errno != EEXIST) {
                std::cerr << "#IPC# creating directory:" << subPath
                          << ",error:" << strerror(errno)  // NOLINT :[strerror]function is not thread safe
                          << std::endl;
                return -1;
            }
        }

        return 0;
    }

    /// @brief Create directory: /run/ara/xxx/ipc
    /// @param[in] path Directory to create
    /// @return true, created successfully; false, creation failed
    static ara::core::String GetIpcNameFromManifest(ara::core::String const &manifestPath)
    {
        std::ifstream fileStream{const_cast< char * >(manifestPath.c_str())};
        rapidjson::IStreamWrapper jsonStream{fileStream};
        rapidjson::Document handler;
        handler.ParseStream(jsonStream);
        if (handler.HasParseError()) {
            std::cerr << "#IPC# parse " << manifestPath << " error." << std::endl;
            return ara::core::String();
        }

        if (handler.HasMember("ipcName")) {
            return ara::core::String{handler["ipcName"].GetString()};
        }
        return ara::core::String{handler["executable"]["name"].GetString()};
    }

    /// @brief Create unix socket fd and set environment variables
    /// @return 0 success; -1, failure
    static ara::core::Result< void > Open()
    {
        // 1.Get execution manifest
        isoft::ara_fsh::Process process;

        ara::core::String ipcName;
        ara::core::String manifestPath{process.GetExecutionManifest()};
        if (manifestPath.empty() || access(const_cast< char * >(manifestPath.c_str()), F_OK) != 0) {
            std::cerr << "\nWarn:can not find execution manifest\n     if there is a manifest for this process, you "
                         "shall set "
                         "the config dir environment "
                      << isoft::ara_fsh::Platform::kEnvKeyConfigDir
                      << " or cp the config files to the dir of your executable." << std::endl;
            ipcName = process.GetBinName();
        } else {
        // 2.Get ipcname
            ipcName = GetIpcNameFromManifest(manifestPath);
            if (ipcName.empty()) {
                std::cerr << "#IPC# can not find ipc name in execution manifest " << manifestPath << std::endl;
                return ara::core::Result< void >::FromError(
                    isoft::core::IpcErrc::kCanNotFindIpcNameInExecutionManifest);
            }
        }

        // 3.Get full ipc path, use fsh in this step
        ara::core::String ipcTotalPathStr(PATH_MAX + 1, '\0');
        if (0
            != isoft::ipc::IPCBase::ResolveName(const_cast< char * >(ipcName.c_str()),
                                                const_cast< char * >(ipcTotalPathStr.c_str()), PATH_MAX)) {
            std::cerr << "#IPC# resolve ipc name " << ipcName << " failed." << std::endl;
            return ara::core::Result< void >::FromError(isoft::core::IpcErrc::kInvalidIpcDir);
        }

        // 4.Create folder, create folder if ipc folder does not exist
        uint32_t lastSeparator = ipcTotalPathStr.find_last_of("/");
        if (lastSeparator == ara::core::String::npos) {
            std::cerr << "#IPC# " << ipcTotalPathStr << " is invalid." << std::endl;
            return ara::core::Result< void >::FromError(isoft::core::IpcErrc::kInvalidIpcDir);
        }
        ara::core::String ipcDir = ipcTotalPathStr.substr(0, lastSeparator + 1);
        if (access(const_cast< char * >(ipcDir.c_str()), F_OK) != 0) {
            if (CreateDir(ipcDir) != 0) {
                return ara::core::Result< void >::FromError(isoft::core::IpcErrc::kCreateIpcDirFailed);
            }
        }

        // 4.If ipc already exists, delete and recreate it
        if (access(const_cast< char * >(ipcTotalPathStr.c_str()), F_OK) == 0) {
            if (unlink(const_cast< char * >(ipcTotalPathStr.c_str())) != 0) {
                std::cerr << "#IPC# unlink " << ipcTotalPathStr << " failed.\n";
                perror("unlink error:");
                return ara::core::Result< void >::FromError(isoft::core::IpcErrc::kIpcResourceIsbusy);
            }
            std::cout << "#IPC# deleted old ipc socket " << ipcTotalPathStr << "\n";
        }
        int32_t &nFD{GetIpcFileFd()};
        // 5.Create socket
        nFD = isoft::ipc::IPCBase::CreateSocket(const_cast< char * >(ipcTotalPathStr.c_str()));
        if (nFD < 0) {
            std::cerr << "#IPC# create socket " << ipcTotalPathStr << " failed.\n";
            return ara::core::Result< void >::FromError(isoft::core::IpcErrc::kCreateIpcSocketFailed);
        }

        // 6.Set environment variables
        uint32_t const fdMaxLen = 256;
        char envBuf[fdMaxLen];
        if (0 != isoft::ipc::IPCBase::CreateFdEnv(nFD, envBuf, fdMaxLen)) {
            std::cerr << "#IPC# CreateFdEnv error.\n";
            return ara::core::Result< void >::FromError(isoft::core::IpcErrc::kSetSocketFdEnvFailed);
        }
        snprintf(envBuf, fdMaxLen, "%d", nFD);
        if (0
            != setenv(isoft::ipc::IPCBase::kEnvKeySocketFd, envBuf, 1))  // NOLINT :[setenv]function is not thread safe
        {
            perror("#IPC# set socket fd env:");
            return ara::core::Result< void >::FromError(isoft::core::IpcErrc::kSetSocketFdEnvFailed);
        }
        ara::core::String &stIpcTotalPath{GetIpcTotalPath()};
        stIpcTotalPath = ipcTotalPathStr;
        return ara::core::Result< void >::FromValue();
    }

    /// @brief Close unix socket
    static void Close()
    {
        constexpr int32_t kInt32_10{10U};
        std::cout << "close ipc." << std::endl;
        char *socketFdEnv
            = getenv(isoft::ipc::IPCBase::kEnvKeySocketFd);  // NOLINT : [getenv]function is not thread safe
        if (socketFdEnv != nullptr) {
            int32_t intFd{static_cast< int32_t >(strtol(socketFdEnv, nullptr, kInt32_10))};
            std::cout << "socket fd env:" << socketFdEnv << ", fd_int:" << intFd << std::endl;
            if (intFd > 0) {
                close(intFd);
            }
        }
        ara::core::String &stIpcTotalPath{GetIpcTotalPath()};
        if (!stIpcTotalPath.empty() && access(const_cast< char * >(stIpcTotalPath.c_str()), F_OK) == 0) {
            if (unlink(const_cast< char * >(stIpcTotalPath.c_str())) != 0) {
                std::cout << "delete ipc socket " << stIpcTotalPath << " error." << std::endl;
                perror("unlink ipc socket not success:");
            }
            stIpcTotalPath.clear();
        }
    }

    static bool IpcInited()
    {
        char *socketFdEnv
            = getenv(isoft::ipc::IPCBase::kEnvKeySocketFd);  // NOLINT : [getenv]function is not thread safe
        if (socketFdEnv != nullptr) {
            return true;
        }
        return false;
    }
};  // namespace core

}  // namespace core
}  // namespace isoft

#endif  // ISOFT_CORE_IPC_IPC_H_
