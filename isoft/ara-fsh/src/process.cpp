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
/// @file       process.cpp
/// @brief
/// @details
/// @date       2022-08-29
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================

#include "isoft/ara_fsh/process.h"

#include <unistd.h>

#include "debug.h"

namespace isoft {
namespace ara_fsh {

std::string Process::swclName_;
std::string Process::swclVersion_;
std::string Process::procName_;

Process::Process(void) noexcept
{
    platform_ = Platform::CreatePlatform();
    if (nullptr == platform_) {
        return;
    }

    /// If procName_ is not empty, it means it has already been created, and no re-initialization is needed.
    if (!procName_.empty()) {
        return;
    }

    if (ProcessStartedIndependently()) {
        return;
    }

    char *envValue = NULL;
    // Get software collection name via environment variable.
    envValue = getenv(kEnvKeySwclName);
    if (NULL != envValue) {
        swclName_ = envValue;
    } else {
        LOG_ERROR("env " << kEnvKeySwclName << " is not set" << std::endl);
        return;
    }

    // Get software collection version via environment variable.
    envValue = getenv(kEnvKeySwclVersion);
    if (NULL != envValue) {
        swclVersion_ = envValue;
    } else {
        LOG_ERROR("env " << kEnvKeySwclVersion << " is not set" << std::endl);
        return;
    }

    // Get process name via environment variable.
    envValue = getenv(kEnvKeyProcName);
    if (NULL != envValue) {
        procName_ = envValue;
    } else {
        LOG_ERROR("env " << kEnvKeyProcName << " is not set" << std::endl);
        return;
    }

    return;
}

std::string Process::GetExePath() noexcept
{
    char exe[PATH_MAX] = {'\0'};
    if (readlink("/proc/self/exe", exe, PATH_MAX) == -1) {
        LOG_ERROR("read /proc/self/exe failed:" << strerror(errno) << std::endl);
        return std::string();
    }
    return std::string(exe);
}

std::string Process::GetBinDir() const noexcept
{
    // example: /home/wangyanlong/ara-sysroot/ara/framework/1.1.0/bin/phmd
    std::string exePath = GetExePath();

    uint32_t lastSeparator = exePath.find_last_of("/");
    if (lastSeparator == std::string::npos) {
        LOG_ERROR("exe path is invalid:" << exePath << std::endl);
        return std::string();
    }

    return exePath.substr(0, lastSeparator + 1);
}

std::string Process::GetBinName() const noexcept
{
    // example: /home/wangyanlong/ara-sysroot/ara/framework/1.1.0/bin/phmd
    std::string exePath = GetExePath();

    uint32_t lastSeparator = exePath.find_last_of("/");
    if (lastSeparator == std::string::npos) {
        LOG_ERROR("exe path is invalid:" << exePath << std::endl);
        return std::string();
    }

    return exePath.substr(lastSeparator + 1);
}

std::string Process::GetEmBinName() const noexcept { return "emd"; }

std::int32_t Process::ExportEnvironment(std::vector< std::string > &envList) const noexcept
{
    if (nullptr == platform_) {
        return -1;
    }
    envList.clear();
    platform_->ExportEnvironment(envList);
    if (true != swclName_.empty())
        envList.emplace_back(kEnvKeySwclName + std::string("=") + swclName_);
    if (true != swclVersion_.empty())
        envList.emplace_back(kEnvKeySwclVersion + std::string("=") + swclVersion_);
    if (true != procName_.empty())
        envList.emplace_back(kEnvKeyProcName + std::string("=") + procName_);
    return 0;
}

bool Process::ProcessStartedIndependently() noexcept
{
    return (NULL == getenv(kEnvKeySwclName) && NULL == getenv(kEnvKeySwclVersion) && NULL == getenv(kEnvKeyProcName));
}

}  // namespace ara_fsh
}  // namespace isoft
