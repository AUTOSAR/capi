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
/// @file       main.cpp
/// @brief      Emd main function
/// @details
/// @date       2023-03-06
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/ExecutionManagement/Emd
/// @unit_name=ExecutionManager
/// @unit_description=The main entry point for the Execution Management System.
/// @interface_level=none
/// @endcode
///
/// ================================================================

#include <sys/fcntl.h>
#include <sys/file.h>
#include <sys/types.h>

#include <array>
#include <fstream>
#include <iostream>
#include <string>

#include "ara/exec/internal/emd/execution_manager.h"
#include "isoft/utils/error.h"
#include "isoft/utils/file.h"

namespace {
/// @brief Redefine char
using Char8_t = char;

/// @brief Print help information
/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
void Help() noexcept
{
    std::cout << "Usage:" << std::endl;
    std::cout << "emd [OPTION] [OPTARG]" << std::endl;
    std::cout << std::endl;
    std::cout << "Option:" << std::endl;
    std::cout << "OPT     ARG              COMMENT" << std::endl;
    std::cout << " -s     ARA_SYSROOT      the top dir wich include ara. default is \"/\"" << std::endl;
    std::cout << " -c     ARA_CONFIG       the configuration file of ara" << std::endl;
    std::cout << " -d     DEBUG_PORT       enable debug server and set the port" << std::endl;
    std::cout << " -v     None             DebugMode, show more Verbose log info" << std::endl;
    std::cout << " -t     None             DebugMode, disable process start and exit timeout" << std::endl;
    std::cout << " -r     None             DebugMode, disable resource group" << std::endl;
}

/// @brief Singleton pattern, ensuring only one execution management process is running at the same time
/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
class SingleInstance
{
public:
    /// @brief Default constructor
    SingleInstance() noexcept = delete;

    /// @brief Constructor
    /// @param araSysroot sdk root directory
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    explicit SingleInstance(ara::core::String const& araSysroot) noexcept
        : araSysRoot_{araSysroot}, lockFile_{}, lockFd_{-1}
    {
        std::string const araTmpDir{isoft::ara_fsh::Platform::GetAraTmpDir(araSysroot.c_str())};
        if (araTmpDir.empty()) {
            std::cerr << "SingleInstance(): Illegal AraSysRoot - " << araSysroot << std::endl;
            return;
        }

        if (0 != isoft::utils::file::MakeCascadeDir(araTmpDir)) {
            std::ostringstream oss;
            oss << "SingleInstance(): Create Directory(" << araTmpDir << ") failed - ";
            perror(oss.str().c_str());
            return;
        }

        std::string const sysPathFileName{araTmpDir + "/" + isoft::ara_fsh::Platform::kAraSysPathFile};
        std::ofstream sysPathFile{sysPathFileName};
        if (sysPathFile.is_open()) {
            sysPathFile << araSysroot << std::endl;  // Write data to the file
            sysPathFile.close();
        } else {
            std::ostringstream oss;
            oss << "SingleInstance(): Create SysPath file(" << sysPathFileName << ") failed - ";
            perror(oss.str().c_str());
            return;
        }

        lockFile_ = isoft::ara_fsh::Platform::GetEmdLockFile(araSysroot.c_str()).c_str();
    }

    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    ~SingleInstance() noexcept
    {
        std::string const araTmpDir{isoft::ara_fsh::Platform::GetAraTmpDir(araSysRoot_.c_str())};
        if (!araTmpDir.empty()) {
            if (0 != isoft::utils::file::RemoveDir(araTmpDir)) {
                ara::core::String const errMsg{"~SingleInstance(): Remove Directory(" + araTmpDir + ") failed - "};
                perror(errMsg.c_str());
            }
        }
    }

    /// @brief Disable move constructor
    /// @param other the other SingleInstance
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    SingleInstance(SingleInstance&& other) noexcept = delete;
    /// @brief Disable copy constructor
    /// @param other the other SingleInstance
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    SingleInstance(SingleInstance const& other) noexcept = delete;
    /// @brief Disable move assignment
    /// @param other the other SingleInstance
    /// @return new SingleInstance
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    SingleInstance& operator=(SingleInstance&& other) noexcept = delete;
    /// @brief Disable copy assignment
    /// @param other the other SingleInstance
    /// @return new SingleInstance
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    SingleInstance& operator=(SingleInstance const& other) noexcept = delete;

    /// @brief Lock the execution management process, ensuring only one execution management process is running at the same time
    /// @return 0 lock successful; <0 lock failed
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    int32_t Lock() noexcept
    {
        if (lockFile_.empty()) {
            std::cerr << "SingleInstance(): Necessary File(AraTmpDir or SysPathFile) is not created !!!" << std::endl;
            return -1;
        }

        /// File lock, preventing multiple execution management processes from running under the same ARA_SYSROOT
        constexpr mode_t kMaskMode{384U};  // 0600
        lockFd_ = open(lockFile_.c_str(), O_CREAT | O_RDWR | O_CLOEXEC, kMaskMode);
        if (lockFd_ < 0) {
            ara::core::String const errMsg{"SingleInstance()->open(" + lockFile_ + ")"};
            perror(errMsg.c_str());
            return -1;
        }

        if (0 != flock(lockFd_, LOCK_EX | LOCK_NB)) {
            if (EWOULDBLOCK != isoft::utils::GetErrNo()) {
                perror("SingleInstance::Lock()->flock(LOCK_EX | LOCK_NB)");
                return -1;
            }

            int32_t const emPid{_GetLockPid()};
            /// If EMD is still running, report an error and exit
            if (0 == kill(emPid, 0)) {
                std::cout << "another execution_manager process is running[" << emPid
                          << "], please terminate it first !" << std::endl;
                return -1;
            }
        }

        /// Successfully opened the lock, write its own PID to identify
        std::ignore = dprintf(lockFd_, "%d", getpid());

        return 0;
    }

    /// @brief Unlock the execution management process
    /// @return 0 unlock successful; <0 unlock failed
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    int32_t Unlock() const noexcept
    {
        if (0 > lockFd_) {
            perror("SingleInstance::Unlock(): invalid lockFd_");
            return -1;
        }

        if (0 != flock(lockFd_, LOCK_UN)) {
            perror("SingleInstance::Unlock()->flock(LOCK_UN)");
            return -1;
        }

        if (0 != close(lockFd_)) {
            perror("~SingleInstance()->close()");
            return -1;
        }

        if (0 != unlink(lockFile_.c_str())) {
            ara::core::String const errMsg{"~SingleInstance()->unlink(" + lockFile_ + ")"};
            perror(errMsg.c_str());
            return -1;
        }
        return 0;
    }

private:
    /// @brief Get the EMD process ID saved in the file lock
    /// @return pid
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    int32_t _GetLockPid() const noexcept
    {
        constexpr uint32_t kPidStringLen{32U};
        std::array< Char8_t, kPidStringLen > pidStr{};
        if (0 >= read(lockFd_, pidStr.data(), kPidStringLen)) {
            perror("SingleInstance::Lock()->read()");
            return -1;
        }
        return std::stoi(pidStr.data());
    }

private:
    /// @brief ara system root directory
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    ara::core::String araSysRoot_;

    /// @brief Lock file path
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    ara::core::String lockFile_;

    /// @brief Lock file descriptor
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    int32_t lockFd_;
};

}  // namespace
/// @brief Main function, entry point of the execution management process
/// @param argc Number of arguments
/// @param argv Argument information
/// @return Execution result
/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
int32_t main(int32_t const argc, Char8_t* argv[]) noexcept
{
    int32_t ret{0};
    try {
        ara::core::String araSysroot{"/"};
        ara::core::String araConfig;
        ara::exec::internal::emd::Argument debugArg;

        int32_t opt{0};
        while (true) {
            opt = getopt(argc, argv, "s:c:d:vr");  // NOLINT
            if (-1 == opt) {
                break;
            }
            switch (opt) {
                case 's': {
                    araSysroot = optarg;
                    break;
                }
                case 'c': {
                    araConfig = optarg;
                    break;
                }
                case 'd': {
                    debugArg.SetServerPort(static_cast< uint16_t >(std::stoi(optarg)));
                    break;
                }
                case 'v': {
                    debugArg.SetVerbose(true);
                    break;
                }
                case 'r': {
                    debugArg.DisableResourceGroup(true);
                    break;
                }
                default: {
                    Help();
                    ret = -1;
                    break;
                }
            }
        }

        if (ret < 0) {
            return -1;
        }

        SingleInstance si{araSysroot};
        if (0 != si.Lock()) {
            std::cerr << "Lock() failure, exit()" << std::endl;
            return -1;
        }

        /// Initialize EvLoop. Initialize evloop and fsh here early to solve the problem of log module usage. Consider calling ara::core::init later?
        std::ignore
            = isoft::naicpp::GlobalGeneralEvLoop::Initialize(isoft::naicpp::GlobalGeneralEvLoop::Mode::kUserThread);

        // Run
        std::shared_ptr< ara::exec::internal::emd::ExecutionManager > em{
            ara::exec::internal::emd::ExecutionManager::CreateInstance(araSysroot, araConfig, debugArg)};
        if (nullptr == em) {
            std::cerr << "ExecutionManager::CreateInstance() error !!!" << std::endl;
            return -1;
        }

        if (true != debugArg.IsDebugOn()) {
            // Notify ara-loader to exit
            std::int32_t const ppid{getppid()};
            if (1 != ppid) {
                std::cout << "send SIGUSR1 to ara_loader !!!" << std::endl;
                std::ignore = kill(ppid, SIGUSR1);
            }
        }

        ret = em->Run();

        std::ignore = si.Unlock();
    } catch (...) {
        ret = -1;
    }
    return ret;
}
