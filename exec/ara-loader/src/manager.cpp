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
/// @file       manager.cpp
/// @brief      ara loader management class definition
/// @details
/// @date       2023-01-17
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================

#include "ara/loader/manager.h"

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <algorithm>
#include <chrono>
#include <cstring>
#include <iostream>
#include <thread>
#include <tuple>

namespace {

/// @brief char type redefinition
using Char8_t = char;

/// @brief Get the EMD executable file name
/// @return EMD executable file name
constexpr Char8_t const *GetEmdExecutableName() noexcept { return "emd"; }

#ifdef __cplusplus
extern "C"
{
#endif

    /// @brief User signal handler function, used for EMD to notify ara-loader to exit
    /// @param signo Signal value
    void SignalHandler(int32_t const signo) noexcept
    {
        if (signo == SIGUSR1) {
            std::cout << "ara_loader: received SIGUSR1 from emd, I will exit." << std::endl;
        }
    }

#ifdef __cplusplus
}
#endif

}  // namespace

namespace ara {
namespace loader {

/// @brief Initialize the loader
/// @return 0 success; <0 failure
int32_t Manager::Initialize() noexcept
{
    araConfig_ = Config::CreateConfig(araSysroot_);
    if (araConfig_ == nullptr) {
        return -1;
    }

    araConfig_->Debug();

    struct sigaction sa
    {
    };
    std::ignore   = memset(&sa, 0, sizeof(sa));
    sa.sa_handler = &SignalHandler;  // Directly assign function pointer

    if (sigaction(SIGUSR1, &sa, nullptr) == -1) {
        perror("signal");
        return -1;
    }

    return 0;
}

/// @brief Execute the startup operation
/// @param recoverMode - Whether it is recovery mode
/// @return 0 success; <0 failure
int32_t Manager::_DoBoot(bool const recoverMode) noexcept
{
    int32_t ret{-1};

    /// Replace goto
    while (true) {
        if (recoverMode) {
            if (0 != araConfig_->LoadRollBack()) {
                break;
            }
            araConfig_->Debug();
        }

        std::ignore = _PrepareArguments4Emd();
        std::ignore = _PrepareEnvironments4Emd();

        if (0 != _PrepareExecutablePath4Emd()) {
            break;
        }

        pid_t const pid{fork()};
        if (-1 == pid) {
            perror("fork()");
            break;
        }

        /// Child process executes the execution-manager program
        if (0 == pid) {
            if (0 != _ExecuteEmd()) {
                break;
            }
        } else {
            // Normally, the waitpid function will block until Execution Management notifies itself to exit after being successfully started.
            int32_t status{0};
            if (-1 == waitpid(pid, &status, 0)) {
                perror("waitpid()");
                break;
            }
            // PRQA S ALL ++
            if (WIFEXITED(status)) {
                std::cout << "EMD exited with " << WEXITSTATUS(status) << std::endl;
                ret = WEXITSTATUS(status);
            }

            if (WIFSIGNALED(status)) {
                std::cout << "EMD exited with signal " << WTERMSIG(status) << std::endl;
                ret = WTERMSIG(status);
            }
            // PRQA S ALL --
        }

        /// Replace goto
        break;
    }

    return ret;
}

/// @brief Execute startup
/// @param mode - Debug mode
/// @return 0 success; <0 failure
int32_t Manager::Boot(DebugConfig const mode) noexcept
{
    debugMode_ = mode;

    /// First start in normal mode
    if (0 != _DoBoot(false)) {
        if (this->debugMode_.IsDebugOn()) {
            std::cout << "startup Failure in DEBUG mode, so we dont recover it." << std::endl;
            return -1;
        }

        std::cout << "startup Failure in NORMAL mode, we will restart in RECOVERY mode." << std::endl;
        /// Wait 1 second for EMD to fully exit
        std::this_thread::sleep_for(std::chrono::seconds(1));

        /// If startup fails, start in recovery mode
        if (0 != _DoBoot(true)) {
            std::cout << "startup Failure in RECOVERY mode, exit now." << std::endl;
            return -1;
        }
    }

    return 0;
}

/// @brief Deinitialize the loader
/// @return 0 success; <0 failure
int32_t Manager::Deinitialize() noexcept
{
    araConfig_ = nullptr;
    return 0;
}

/// @brief Prepare environment variables for Execution Management
/// @return 0 success; <0 failure
int32_t Manager::_PrepareEnvironments4Emd() noexcept
{
    /// Platform runtime library path
    std::string libDirs{araConfig_->GetFrameworkLibDir()};
    /// System runtime library path
    libDirs += ":" + araSysroot_ + "/usr/lib:" + araSysroot_ + "/usr/lib64";
    /// Runtime library path specified by environment variable
    Char8_t *const sysLibPath{secure_getenv("LD_LIBRARY_PATH")};
    if (nullptr != sysLibPath) {
        libDirs += std::string{":"} + std::string{sysLibPath};
    }

    std::string env{"LD_LIBRARY_PATH=" + libDirs};
    emdEnvironVars_.emplace_back(env);

    return 0;
}

/// @brief Prepare parameters for Execution Management
/// @return 0 success; <0 failure
int32_t Manager::_PrepareArguments4Emd() noexcept
{
    emdArguments_.emplace_back(GetEmdExecutableName());
    emdArguments_.emplace_back("-s");
    emdArguments_.emplace_back(araSysroot_);
    emdArguments_.emplace_back("-c");
    emdArguments_.emplace_back(araConfig_->GetConfigFileName());
    if (this->debugMode_.IsVerboseOn()) {
        emdArguments_.emplace_back("-v");
    }
    if (this->debugMode_.IsResourceGroupDisabled()) {
        emdArguments_.emplace_back("-r");
    }
    if (this->debugMode_.IsServerOn()) {
        std::vector< Char8_t > vecBuff;
        size_t const k128Bytes{128U};
        vecBuff.resize(k128Bytes);
        std::ignore = sprintf(vecBuff.data(), "%d", debugMode_.GetServerPort());
        emdArguments_.emplace_back("-d");
        emdArguments_.emplace_back(vecBuff.data());
    }

    return 0;
}

/// @brief Prepare the executable file for Execution Management
/// @return 0 success; <0 failure
int32_t Manager::_PrepareExecutablePath4Emd() noexcept
{
    emdExecPath_ = araConfig_->GetFrameworkSbinDir() + "/" + GetEmdExecutableName();

    if (0 != ::access(emdExecPath_.c_str(), F_OK | R_OK | X_OK)) {
        std::string const errMsg{"access(" + emdExecPath_ + ")"};
        perror(errMsg.c_str());
        return -1;
    }

    return 0;
}

/// @brief Execute the EMD program
/// @return 0 success; <0 failure
int32_t Manager::_ExecuteEmd() const noexcept
{
    int32_t ret{0};
    /// Set environment variables
    std::vector< Char8_t * > envs;
    std::cout << "Environments:" << std::endl;
    std::ignore = std::for_each(emdEnvironVars_.cbegin(), emdEnvironVars_.cend(), [&envs](auto &env) {
        envs.push_back(const_cast< Char8_t * >(env.c_str()));
        std::cout << "\t" << env << std::endl;
    });
    envs.push_back(nullptr);

    /// Set Arguments
    std::vector< Char8_t * > args;
    std::cout << "Arguments:" << std::endl;
    std::ignore = std::for_each(emdArguments_.cbegin(), emdArguments_.cend(), [&args](auto &arg) {
        args.push_back(const_cast< Char8_t * >(arg.c_str()));
        std::cout << "\t" << arg << std::endl;
    });
    args.push_back(nullptr);

    std::cout << "EmdPath: " << emdExecPath_ << std::endl;
    if (0 != ::execvpe(emdExecPath_.c_str(), args.data(), envs.data())) {
        std::string const errMsg{"execvpe(" + emdExecPath_ + ")"};
        perror(errMsg.c_str());
        ret = -1;
    }

    return ret;
}

}  // namespace loader
}  // namespace ara