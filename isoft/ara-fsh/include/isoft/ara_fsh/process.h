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
/// @file       process.h
/// @brief
/// @details
/// @date       2022-08-29
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================

#ifndef _ISOFT_ARA_FILESYSTEM_HIERARCHY_PROCESS_H_
#define _ISOFT_ARA_FILESYSTEM_HIERARCHY_PROCESS_H_
// clang-format off
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <cstdint>
#include "isoft/ara_fsh/platform.h"
// clang-formate on
namespace isoft {
namespace ara_fsh {

class Process
{
public:
    /// Folder definition
    const static constexpr char *kBin  = "/bin/";
    const static constexpr char *kSbin = "/sbin/";
    const static constexpr char *kEtc  = "/etc/";
    const static constexpr char *kData = "/data/";

    /// File name definition
    const static constexpr char *kExecutionManfest = "manifest.json";
    const static constexpr char *kNsomeip          = "nsomeip.json";
    const static constexpr char *kService          = "service_instance_manifest.json";
    const static constexpr char *kTriggerFGService = "trigger_fg_service_instance_manifest.json";
    const static constexpr char *kStateMachineConf = "state_machine.json";
    const static constexpr char *kStorage          = "storage_manifest.json";
    const static constexpr char *kCrypto           = "crypto_manifest.json";
    const static constexpr char *kTimeBaseMap      = "time_base_map.json";
    const static constexpr char *kLogConfig        = "log.json";
    const static constexpr char *kLogEmdConfig     = "log-emd.json";

    /// Environment variables, execution management will pass the following environment variables to child processes
    const static constexpr char *kEnvKeySwclName    = "ISOFT_ARA_FSH_SWCL_NAME";
    const static constexpr char *kEnvKeySwclVersion = "ISOFT_ARA_FSH_SWCL_VERSION";
    const static constexpr char *kEnvKeyProcName    = "ISOFT_ARA_FSH_PROC_NAME";

    static std::shared_ptr< Process > CreateProcess(void) noexcept
    {
        std::shared_ptr< Process > proc = std::make_shared< Process >();
        if (nullptr == proc) {
            return nullptr;
        }
        if (proc->procName_.empty()) {
            proc.reset();
        }
        return proc;
    }

    static std::shared_ptr< Process > CreateProcess(const std::string &swclName,
                                                    const std::string &swclVersion,
                                                    const std::string &procName) noexcept
    {
        std::shared_ptr< Process > proc = std::make_shared< Process >(swclName, swclVersion, procName);
        if (nullptr == proc) {
            return nullptr;
        }
        if (proc->procName_.empty()) {
            proc.reset();
        }
        return proc;
    }

    /// @brief Constructor of the file system hierarchy process class.
    ///     Used for all applications started through execution management.
    ///     This interface will recognize environment variables. Execution management will pass relevant environment variables to all child processes to ensure normal use of this interface.
    Process(void) noexcept;
    ~Process() = default;

    /// @brief For use by platform modules such as execution management; ordinary user processes need not pay attention.
    Process(const std::string &swclName, const std::string &swclVersion, const std::string &procName) noexcept
    {
        platform_ = Platform::CreatePlatform();
        if (nullptr == platform_) {
            return;
        }
        swclName_    = swclName;
        swclVersion_ = swclVersion;
        procName_    = procName;
    }

    /// @brief Get the root directory of the software collection to which the process belongs.
    /// @returns true != empty() - Success
    /// @returns true == empty() - Failure
    /// @example
    ///         Platform process: /opt/ara-sysroot/ara/core/1.0.0
    ///         User process: /opt/ara-sysroot/ara/swcls/HelloWorld/1.0.0
    std::string GetSwclRootDir() const noexcept
    {
        std::string ret;
        if (nullptr != platform_) {
            ret = platform_->GetSwclRootDir(swclName_, swclVersion_);
        }
        return ret;
    }

    /// @brief Get the process bin path.
    /// @returns true != empty() - Success
    /// @returns true == empty() - Failure
    /// @example
    ///         Platform process: /opt/ara-sysroot/ara/core/1.0.0/bin/smd
    ///         User process: /opt/ara-sysroot/ara/swcls/HelloWorld/1.0.0/bin/hello
    static std::string GetExePath() noexcept;

    /// @brief Get the process bin directory.
    /// @returns true != empty() - Success
    /// @returns true == empty() - Failure
    /// @example
    ///         Platform process: /opt/ara-sysroot/ara/core/1.0.0/bin
    ///         User process: /opt/ara-sysroot/ara/swcls/HelloWorld/1.0.0/bin
    std::string GetBinDir() const noexcept;

    /// @brief Get the process executable file name.
    /// @returns true != empty() - Success
    /// @returns true == empty() - Failure
    /// @example emd、smd、phmd
    std::string GetBinName() const noexcept;

    /// @brief Get the EM process executable file name.
    /// @returns true != empty() - Success
    /// @returns true == empty() - Failure
    /// @example emd
    std::string GetEmBinName() const noexcept;

    /// @brief Get the process etc directory.
    /// @returns true != empty() - Success
    /// @returns true == empty() - Failure
    /// @example
    ///         Platform process: /opt/ara-sysroot/ara/core/1.0.0/etc/demo_proc
    ///         User process: /opt/ara-sysroot/ara/swcls/HelloWorld/1.0.0/etc/demo_proc
    std::string GetEtcDir() const noexcept
    {
        std::string ret;
        ret = GetSwclRootDir();
        if (!ret.empty() && !procName_.empty()) {
            ret += "/" + std::string{kEtc} + "/" + procName_ + std::string{"/"};
        }

        if (ret.empty()) {
            char *configDir = getenv(isoft::ara_fsh::Platform::kEnvKeyConfigDir);
            if (configDir) {
                ret = std::string(configDir);
            }
        }

        if (ret.empty()) {
            ret = GetBinDir();
        }

        if (getenv(kEnvFshDebug)) {
            std::cout << "process etc dir:" << ret << std::endl;
        }
        return ret;
    }

    /// @brief Get process-related configuration.
    /// @param file - Configuration file name.
    /// @return File path.
    std::string GetConfig(std::string const &file) const noexcept
    {
        std::string ret;
        ret = GetEtcDir();
        if (ret.empty()) {
            /// For debugging convenience, read the manifest in the current directory when no directory is specified.
            ret = "./" + file;
        } else {
            ret += "/" + file;
        }
        return ret;
    }

    /// @brief Get the process execution manifest path.
    /// @returns true != empty() - Success
    /// @returns true == empty() - Failure
    /// @example
    ///         Platform process: /opt/ara-sysroot/ara/core/1.0.0/etc/demo_proc/manifest.json
    ///         User process: /opt/ara-sysroot/ara/swcls/HelloWorld/1.0.0/etc/demo_proc/manifest.json
    std::string GetExecutionManifest() const noexcept { return GetConfig(kExecutionManfest); }

    /// @brief Get the process nsomeip manifest path.
    /// @returns true != empty() - Success
    /// @returns true == empty() - Failure
    /// @example
    ///         Platform process: /opt/ara-sysroot/ara/core/1.0.0/etc/demo_proc/nsomeip.json
    ///         User process: /opt/ara-sysroot/ara/swcls/HelloWorld/1.0.0/etc/demo_proc/nsomeip.json
    std::string GetNsomeip() const noexcept { return GetConfig(kNsomeip); }

    /// @brief Get the process service instance manifest path.
    /// @returns true != empty() - Success
    /// @returns true == empty() - Failure
    /// @example
    ///         Platform process: /opt/ara-sysroot/ara/core/1.0.0/etc/demo_proc/service_instance_manifest.json
    ///         User process: /opt/ara-sysroot/ara/swcls/HelloWorld/1.0.0/etc/demo_proc/service_instance_manifest.json
    std::string GetService() const noexcept { return GetConfig(kService); }

    /// @brief Get the process function group trigger service instance manifest path (for state management use only).
    /// @returns true != empty() - Success
    /// @returns true == empty() - Failure
    /// @example
    ///         SM process: /opt/ara-sysroot/ara/core/1.0.0/etc/state_manager/trigger_fg_service_instance_manifest.json
    std::string GetTriggerFGService() const noexcept { return GetConfig(kTriggerFGService); }

    /// @brief Get the process state machine configuration manifest path (for state management use only).
    /// @returns true != empty() - Success
    /// @returns true == empty() - Failure
    /// @example
    ///         SM process: /opt/ara-sysroot/ara/core/1.0.0/etc/state_manager/state_machine.json
    std::string GetStateMachineConf() const noexcept { return GetConfig(kStateMachineConf); }

    /// @brief Get the process crypto manifest path.
    /// @returns true != empty() - Success
    /// @returns true == empty() - Failure
    /// @example
    ///         Platform process: /opt/ara-sysroot/ara/core/1.0.0/etc/demo_proc/crypto_manifest.json
    ///         User process: /opt/ara-sysroot/ara/swcls/HelloWorld/1.0.0/etc/demo_proc/crypto_manifest.json
    std::string GetCrypto() const noexcept { return GetConfig(kCrypto); }

    /// @brief Get the process storage manifest path.
    /// @returns true != empty() - Success
    /// @returns true == empty() - Failure
    /// @example
    ///         Platform process: /opt/ara-sysroot/ara/core/1.0.0/etc/demo_proc/storage_manifest.json
    ///         User process: /opt/ara-sysroot/ara/swcls/HelloWorld/1.0.0/etc/demo_proc/storage_manifest.json
    std::string GetStorage() const noexcept { return GetConfig(kStorage); }

    /// @brief Get the process time synchronization mapping manifest path.
    /// @returns true != empty() - Success
    /// @returns true == empty() - Failure
    /// @example
    ///         Platform process: /opt/ara-sysroot/ara/core/1.0.0/etc/demo_proc/time_base_map.json
    ///         User process: /opt/ara-sysroot/ara/swcls/HelloWorld/1.0.0/etc/demo_proc/time_base_map.json
    std::string GetTimeBaseMap() const noexcept { return GetConfig(kTimeBaseMap); }

    /// @brief Get the process executable program file path.
    /// @returns true != empty() - Success
    /// @returns true == empty() - Failure
    /// @example
    ///         Platform process: /opt/ara-sysroot/ara/core/1.0.0/bin/demo_exec
    ///         User process: /opt/ara-sysroot/ara/swcls/HelloWorld/1.0.0/bin/demo_exec
    std::string GetExecutable(const std::string &execName) const noexcept { return GetBinDir() + execName; }

    /// @brief Get the process IPC communication directory.
    /// @returns true != empty() - Success
    /// @returns true == empty() - Failure
    /// @example /tmp/ara-ipc-de74725d365691a10b6081dd0acd6a1f
    /// @note Why is the IPC directory placed in /tmp instead of ara-sysroot/tmp? Because IPC communication uses Unix sockets, and the socket 
    /// path supports a maximum of 108 bytes, while the actual ara-sysroot path far exceeds 108.
    ///         The current IPC path is based on the MD5 hash of ara-sysroot, which can still avoid conflicts when debugging multiple machines, because the ara-sysroot of multiple machines is not allowed to be the same.
    std::string GetUnixDomainSocketDir() const noexcept { return platform_->GetUnixDomainSocketDir(); }

    /// @brief Get the process log configuration file path.
    /// @returns true != empty() - Success
    /// @returns true == empty() - Failure
    /// @example
    ///         Ordinary process: /opt/ara-sysroot/ara/core/1.0.0/etc/demo1/log.json
    ///         EMD：/opt/ara-sysroot/ara/core/1.0.0/etc/log-emd.json
    std::string GetLogConfig() const noexcept
    {
        std::string ret;
        if (GetBinName() == GetEmBinName()) {
            ret = Platform().GetPlatformEtcDir() + kLogEmdConfig;
        } else {
            ret = GetConfig(kLogConfig);
        }

        if (getenv(kEnvFshDebug)) {
            std::cout << "log config:" << ret << std::endl;
        }
        return ret;
    }

    /// @brief Export the environment variables related to FSH for the current process.
    /// @param[out] envList, the output environment variable list.
    std::int32_t ExportEnvironment(std::vector< std::string > &envList) const noexcept;

    /// @brief returns whether process is started independently.
    /// @return whether process is started independently.
    static bool ProcessStartedIndependently() noexcept;

private:
    std::shared_ptr< Platform > platform_;
    static std::string swclName_;
    static std::string swclVersion_;
    static std::string procName_;
};  // namespace ara_fsh

}  // namespace ara_fsh
}  // namespace isoft

#endif  // _ISOFT_ARA_FILESYSTEM_HIERARCHY_PROCESS_H_