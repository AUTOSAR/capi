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
/// @file       platform.h
/// @brief
/// @details
/// @date       2022-08-29
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================

#ifndef _ISOFT_ARA_FILESYSTEM_HIERARCHY_SOFTWARE_CLUSTER_H_
#define _ISOFT_ARA_FILESYSTEM_HIERARCHY_SOFTWARE_CLUSTER_H_
// clang-format off
#include <cstdint>
#include <limits.h>
#include <stdlib.h>
#include <unistd.h>

#include <iostream>
#include <memory>
#include <string>
#include <vector>
// clang-format on
namespace isoft {
namespace ara_fsh {

const static constexpr char *kEnvFshDebug = "ISOFT_ARA_FSH_DEBUG";

/// @brief Platform-level configuration.
class Platform
{
public:
    /// Folder definition
    const static constexpr char *kAra             = "/ara/";
    const static constexpr char *kTmp             = "/tmp/";
    const static constexpr char *kDefaultAraCore  = "/ara/core/";
    const static constexpr char *kDefaultAraSwcls = "/ara/swcls/";
    const static constexpr char *kDefaultAraVar   = "/ara/var/";
    const static constexpr char *kAraTmpDirPrefix = "/run/ara/";
    const static constexpr char *kAraSysPathFile  = "syspath";
    const static constexpr char *kAraEmdLockFile  = "emd.lock";
    const static constexpr char *kIpcDir          = "/ipc/";
    const static constexpr char *kEtc             = "/etc/";
    const static constexpr char *kLib             = "/lib/";
    const static constexpr char *kSbin            = "/sbin/";
    const static constexpr char *kBin             = "/bin/";
    const static constexpr char *kSecurity        = "/security/";

    /// File name definition
    const static constexpr char *kMachineManifest                = "machine_manifest.json";
    const static constexpr char *kTimeSyncManifest               = "time_sync.json";
    const static constexpr char *kPlatformSwclsManifest          = "run_time_framework_core_list.json";
    const static constexpr char *kApplicationSwclsManifestConfig = "run_time_application_swcl_list.json";
    const static constexpr char *kSwclManifest                   = "swcl_manifest.json";
    const static constexpr char *kSwclStatus                     = "swcl_status.json";
    const static constexpr char *kSwclFuncGrpSet                 = "function_groups.json";

    const static constexpr char *kRsaPubKeyFile           = "pubkey.pem";
    const static constexpr char *kMachineManifestHashFile = "machine.hash";
    const static constexpr char *kMachineManifestSignFile = "machine.hash.sign";

    /// Keyword definition
    const static constexpr char *kCore      = "core";
    const static constexpr char *kFramework = "framework";

    /// Environment variables, execution management will pass the following environment variables to child processes
    const static constexpr char *kEnvKeyAraSysroot        = "ISOFT_ARA_FSH_SYSROOT";
    const static constexpr char *kEnvKeyAraConfigFileName = "ISOFT_ARA_FSH_CONFIG_FILE_NAME";
    const static constexpr char *kEnvKeyConfigDir         = "ISOFT_ARA_FSH_PROC_CONFIG_DIR";
    const static constexpr char *kAraRuntimeDir           = "ISOFT_ARA_RUNTIME_DIR";
    const static constexpr char *kEnvKeyMachineConfigDir  = "ISOFT_ARA_FSH_MACHINE_CONFIG_DIR";

    static std::shared_ptr< Platform > CreatePlatform() noexcept
    {
        std::shared_ptr< Platform > plt = std::make_shared< Platform >();
        if (plt->GetSysroot().empty()) {
            plt.reset();
        }
        return plt;
    }

    static std::shared_ptr< Platform > CreatePlatform(std::string const &araSysroot,
                                                      std::string const &araConfig) noexcept
    {
        std::shared_ptr< Platform > plt = std::make_shared< Platform >(araSysroot, araConfig);
        if (plt->GetSysroot().empty()) {
            plt.reset();
        }
        return plt;
    }

    /// @brief Parameterized constructor, suitable for execution management.
    /// @param araSysroot - Ara platform root path.
    /// @param araConfig - Ara configuration file. This file has multiple versions, and ara-loader selects the usable version.
    Platform(std::string const &araSysroot, std::string const &araConfig) noexcept
    {
        araSysroot_        = araSysroot;
        araConfigFileName_ = araConfig;

        if (0 != LoadConfig()) {
            araConfigFileName_.clear();
        }
    }

    /// @brief Parameterless constructor, suitable for platform-level processes started through execution management.
    Platform(void) noexcept;

    /// @brief Get the platform root path.
    /// @brief Platform root path.
    std::string const &GetSysroot(void) const noexcept { return araSysroot_; }

    /// @brief Get the MD5 string of the platform root path.
    std::string const &GetSysrootMd5Str(void) const noexcept
    {
        if (araSysrootMd5Str_.empty()) {
            araSysrootMd5Str_ = GenMd5Str(araSysroot_);
        }
        return araSysrootMd5Str_;
    }

    /// @brief Get the platform version number.
    /// @return Version number.
    std::string const &GetPlatformVersion(void) const noexcept { return araCoreVersion_; }

    /// @brief Get the platform configuration directory path.
    /// @returns Platform configuration directory path.
    ///         e.g., ${ARA_SYSROOT}/ara/core/1.0.0/etc
    /// @returns Empty string.
    std::string GetPlatformEtcDir(void) const noexcept;

    /// @brief Get the storage directory path for platform security-related configurations.
    /// @returns Storage directory path for platform security-related configurations.
    ///         e.g., ${ARA_SYSROOT}/ara/core/1.0.0/security
    /// @returns Empty string.
    std::string GetPlatformSecurityDir(void) const noexcept
    {
        std::string ret;
        if (!araSysroot_.empty() && !araCore_.empty() && !araCoreVersion_.empty()) {
            ret = araCore_ + "/" + araCoreVersion_ + kSecurity;
        }
        return ret;
    }

    /// @brief Get the platform runtime library directory path.
    /// @returns On success, platform runtime library directory path.
    ///         e.g., ${ARA_SYSROOT}/ara/framework/x.x.x/lib
    /// @returns On failure, empty string.
    std::string GetPlatformLibDir() const noexcept
    {
        std::string ret;
        if (!araSysroot_.empty() && !araFramework_.empty() && !araFrameworkVersion_.empty()) {
            ret = araFramework_ + "/" + araFrameworkVersion_ + kLib;
        }
        return ret;
    }

    /// @brief Get the platform privileged binary directory path.
    /// @returns On success, platform privileged binary directory path.
    ///         e.g., ${ARA_SYSROOT}/ara/framework/1.0.0/sbin
    /// @returns On failure, empty string.
    std::string GetPlatformSbinDir() const noexcept
    {
        std::string ret;
        if (!araSysroot_.empty() && !araFramework_.empty() && !araFrameworkVersion_.empty()) {
            ret = araFramework_ + "/" + araFrameworkVersion_ + kSbin;
        }
        return ret;
    }

    /// @brief Get the platform binary directory path.
    /// @returns On success, platform binary directory path.
    ///         e.g., ${ARA_SYSROOT}/ara/framework/1.0.0/bin
    /// @returns On failure, empty string.
    std::string GetPlatformBinDir() const noexcept
    {
        std::string ret;
        if (!araSysroot_.empty() && !araFramework_.empty() && !araFrameworkVersion_.empty()) {
            ret = araFramework_ + "/" + araFrameworkVersion_ + kBin;
        }
        return ret;
    }

    /// @brief Get the ara directory path.
    std::string GetAraDir(void) const noexcept
    {
        std::string ret;
        if (!araSysroot_.empty()) {
            ret = araSysroot_ + "/" + std::string(kAra);
        }
        return ret;
    }

    /// @brief Get the ara temporary directory path.
    static std::string GetAraTmpDir(const std::string &araSysroot) noexcept;
    std::string GetAraTmpDir() const noexcept { return GetAraTmpDir(araSysroot_); }

    /// @brief Get the tmp directory path under araSysroot.
    /// @returns On failure, empty string.
    static std::string GetSysTmpDir(const std::string &araSysroot) noexcept
    {
        if (araSysroot.empty())
            return std::string();
        return araSysroot + "/" + std::string(kTmp);
    }
    std::string GetSysTmpDir() const noexcept { return GetSysTmpDir(araSysroot_); }

    /// @brief Get the installation path of the framework software collection.
    std::string GetFrameworkDir() const noexcept { return araFramework_ + "/"; }

    /// @brief Get the installation path of the core software collection.
    std::string GetCoreDir() const noexcept { return araCore_ + "/"; }

    /// @brief Get the installation path of the user software collection.
    std::string GetAraSwclsDir() const noexcept { return araSwcls_ + "/"; }

    /// @brief Get the ara variable data directory path.
    std::string GetAraVarDir() const noexcept { return araVar_ + "/"; }

    /// @brief Get the IPC communication directory path.
    /// @param araSysroot - Ara platform root path.
    /// @returns On success, IPC communication directory path.
    /// @returns On failure, empty string.
    static std::string GetUnixDomainSocketDir(const std::string &araSysroot) noexcept;
    std::string GetUnixDomainSocketDir() const noexcept { return GetUnixDomainSocketDir(araSysroot_); }

    /// @brief Get the execution management singleton lock file path.
    /// @param araSysroot - Ara platform root path.
    /// @returns On success, emd singleton lock file path.
    /// @returns On failure, empty string.
    static std::string GetEmdLockFile(const std::string &araSysroot) noexcept;
    std::string GetEmdLockFile() const noexcept { return GetEmdLockFile(araSysroot_); }

    /// @brief Get the machine configuration manifest file path.
    std::string GetMachineManifest() const noexcept
    {
        std::string ret;
        ret = GetPlatformEtcDir();
        if (ret.empty()) {
            ret.clear();
        } else {
            ret += kMachineManifest;
        }
        return ret;
    }

    /// @brief Get the time synchronization configuration manifest file path.
    std::string GetTimeSyncManifest() const noexcept
    {
        std::string ret;
        ret = GetPlatformEtcDir();
        if (ret.empty()) {
            ret.clear();
        } else {
            ret += kTimeSyncManifest;
        }
        return ret;
    }

    /// @brief Get the platform process list configuration file path.
    std::string GetPlatformSwclsManifest() const noexcept
    {
        std::string ret;
        ret = GetPlatformEtcDir();
        if (ret.empty()) {
            ret.clear();
        } else {
            ret += kPlatformSwclsManifest;
        }
        return ret;
    }

    /// @brief Get the user process list configuration file path.
    /// @returns On failure, empty string.
    std::string GetApplicationSwclsManifest() const noexcept;

    /// @brief Get the application-level software collection configuration file.
    /// @brief File path.
    /// @returns On failure, empty string.
    std::string GetApplicationSwclsManifestConfig() const noexcept
    {
        std::string ret;
        ret = GetPlatformEtcDir();
        if (ret.empty()) {
            ret.clear();
        } else {
            ret += kApplicationSwclsManifestConfig;
        }
        return ret;
    }

    /// @brief Get the software collection root directory path.
    /// @param swclName - Software collection name.
    /// @param swclVersion - Software collection version.
    /// @returns On success, software collection root directory path.
    ///         e.g., ${ARA_SYSROOT}/ara/swcls/Radar/x.x.x/
    ///         Or: ${ARA_SYSROOT}/ara/core/x.x.x/
    ///         Or: ${ARA_SYSROOT}/ara/framework/x.x.x/
    /// @returns On failure, empty string.
    std::string GetSwclRootDir(const std::string &swclName, const std::string &swclVersion) const noexcept;

    /// @brief Get the software collection bin directory path.
    /// @param swclName - Software collection name.
    /// @param swclVersion - Software collection version.
    /// @returns On success, software collection bin directory path.
    ///         e.g., ${ARA_SYSROOT}/ara/swcls/Radar/x.x.x/bin/
    ///         Or: ${ARA_SYSROOT}/ara/core/x.x.x/bin/
    ///         Or: ${ARA_SYSROOT}/ara/framework/x.x.x/bin/
    /// @returns On failure, empty string.
    template < class StringType >
    StringType GetSwclBinDir(const StringType &swclName, const StringType &swclVersion) const noexcept
    {
        StringType root = GetSwclRootDir(swclName.c_str(), swclVersion.c_str()).c_str();
        if (!root.empty()) {
            return root + kBin;
        } else {
            return root;
        }
    }

    /// @brief Get the software collection lib directory path.
    /// @param swclName - Software collection name.
    /// @param swclVersion - Software collection version.
    /// @returns On success, software collection lib directory path.
    ///         e.g., ${ARA_SYSROOT}/ara/swcls/Radar/1.0.0/lib/
    ///         Or: ${ARA_SYSROOT}/ara/core/1.0.0/lib/
    ///         Or: ${ARA_SYSROOT}/ara/framework/1.0.0/lib/
    /// @returns On failure, empty string.
    template < class StringType >
    StringType GetSwclLibDir(const StringType &swclName, const StringType &swclVersion) const noexcept
    {
        StringType root = GetSwclRootDir(swclName.c_str(), swclVersion.c_str()).c_str();
        if (!root.empty()) {
            return root + kLib;
        } else {
            return root;
        }
    }

    /// @brief Get the software collection security directory path.
    /// @param swclName - Software collection name.
    /// @param swclVersion - Software collection version.
    /// @returns On success, software collection security directory path.
    ///         e.g., ${ARA_SYSROOT}/ara/swcls/Radar/1.0.0/security/
    ///         Or: ${ARA_SYSROOT}/ara/core/1.0.0/security/
    ///         Or: ${ARA_SYSROOT}/ara/framework/1.0.0/security/
    /// @returns On failure, empty string.
    std::string GetSwclSecurityDir(const std::string &swclName, const std::string &swclVersion) const noexcept
    {
        std::string ret = GetSwclRootDir(swclName.c_str(), swclVersion.c_str()).c_str();
        if (!ret.empty()) {
            return ret + kSecurity;
        } else {
            return ret;
        }
    }

    /// @brief Get the software collection configuration file path.
    /// @param swclName - Software collection name.
    /// @param swclVersion - Software collection version number.
    /// @returns On success, software collection configuration file path.
    ///         e.g., ${ARA_SYSROOT}/ara/swcls/Radar/1.0.0/swcl_manifest.json
    ///         Or: ${ARA_SYSROOT}/ara/core/1.0.0/swcl_manifest.json
    ///         Or: ${ARA_SYSROOT}/ara/framework/1.0.0/swcl_manifest.json
    /// @returns On failure, empty string.
    std::string GetSwclManifest(const std::string &swclName, const std::string &swclVersion) noexcept
    {
        std::string ret;
        ret = GetSwclRootDir(swclName, swclVersion);
        if (ret.empty()) {
            ret.clear();
        } else {
            ret += kSwclManifest;
        }
        return ret;
    }

    /// @brief Get the software collection function group configuration file path.
    /// @param swclName - Software collection name.
    /// @param swclVersion - Software collection version number.
    /// @returns On success, software collection function group configuration file path.
    ///         e.g., ${ARA_SYSROOT}/ara/core/1.1.0/etc/function_groups.json
    ///         Or: ${ARA_SYSROOT}/ara/swcls/Radar/1.1.0/etc/function_groups.json
    /// @returns On failure, empty string.
    std::string GetSwclFuncGrpSet(const std::string &swclName, const std::string &swclVersion) noexcept
    {
        std::string ret = GetSwclRootDir(swclName, swclVersion);
        if (ret.empty()) {
            ret.clear();
        } else {
            ret.append(kEtc).append("/").append(kSwclFuncGrpSet);
        }

        return ret;
    }

    /// @brief Get the system public key file.
    /// @param None
    /// @returns On success, public key storage file path.
    ///         Or: ${ARA_SYSROOT}/ara/core/1.0.0/etc/pubkey.pem
    /// @returns On failure, empty string.
    std::string GetPubKeyFile() noexcept
    {
        std::string ret;
        ret = GetPlatformEtcDir();
        if (ret.empty()) {
            ret.clear();
        } else {
            ret += kRsaPubKeyFile;
        }
        return ret;
    }

    /// @brief Export environment variables.
    std::int32_t ExportEnvironment(std::vector< std::string > &envList) const noexcept;

private:
    static std::string GenMd5Str(const std::string data) noexcept;
    std::int32_t LoadConfig(void) noexcept;

private:
    static std::string araSysroot_;
    static std::string araSysrootMd5Str_;
    static std::string araConfigFileName_;
    static std::string araCore_;
    static std::string araFramework_;
    static std::string araVar_;
    static std::string araSwcls_;
    static std::string araCoreVersion_;
    static std::string araFrameworkVersion_;
};

}  // namespace ara_fsh
}  // namespace isoft

#endif  // _ISOFT_ARA_FILESYSTEM_HIERARCHY_SOFTWARE_CLUSTER_H_
