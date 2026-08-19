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
/// @file       platform.cpp
/// @brief
/// @details
/// @date       2022-08-30
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================

#include "isoft/ara_fsh/platform.h"

#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>

#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>

#include "debug.h"
#include "isoft/ara_fsh/process.h"
#include "openssl/evp.h"

namespace isoft {
namespace ara_fsh {

std::string Platform::araSysroot_;
std::string Platform::araSysrootMd5Str_;
std::string Platform::araConfigFileName_;
std::string Platform::araCore_;
std::string Platform::araFramework_;
std::string Platform::araVar_;
std::string Platform::araSwcls_;
std::string Platform::araCoreVersion_;
std::string Platform::araFrameworkVersion_;

std::int32_t Platform::LoadConfig(void) noexcept
{
    static const char *kAraConfigSectionBase         = "base";
    static const char *kAraConfigSectionUcm          = "ucm";
    static const char *kAraConfigKeyFramework        = "platform_framework";
    static const char *kAraConfigKeyCore             = "platform_core";
    static const char *kAraConfigKeyVar              = "platform_var";
    static const char *kAraConfigKeySwcls            = "application_swcls";
    static const char *kAraConfigKeyVersionCore      = "core_version";
    static const char *kAraConfigKeyVersionFramework = "framework_version";

    static const char *kAraConfigValueSwclsDefault     = "/swcls/";
    static const char *kAraConfigValueVarDefault       = "/var/";
    static const char *kAraConfigValueCoreDefault      = "/core/";
    static const char *kAraConfigValueFrameworkDefault = "/framework/";

    std::string araConfigPath = GetAraDir();
    if (araConfigPath.empty()) {
        return -1;
    }
    araConfigPath += araConfigFileName_;

    std::ifstream fileStream{araConfigPath.c_str()};
    rapidjson::IStreamWrapper jsonStream{fileStream};
    rapidjson::Document handler;
    handler.ParseStream(jsonStream);
    if (handler.HasParseError()) {
        std::cout << "parse " << araConfigPath << " error.\n";
        return -1;
    }

    if (handler.HasMember(kAraConfigSectionUcm)) {
        if (handler[kAraConfigSectionUcm].HasMember(kAraConfigKeyVersionFramework)) {
            araFrameworkVersion_ = handler[kAraConfigSectionUcm][kAraConfigKeyVersionFramework].GetString();
        } else {
            std::cout << "framework version version lacked in " << araConfigPath << "\n";
            return -1;
        }

        if (handler[kAraConfigSectionUcm].HasMember(kAraConfigKeyVersionCore)) {
            araCoreVersion_ = handler[kAraConfigSectionUcm][kAraConfigKeyVersionCore].GetString();
        } else {
            std::cout << "core version lacked in " << araConfigPath << "\n";
            return -1;
        }
    } else {
        std::cout << "ucm lacked in " << araConfigPath << "\n";
        return -1;
    }

    if (handler.HasMember(kAraConfigSectionBase)) {
        if (handler[kAraConfigSectionBase].HasMember(kAraConfigKeyFramework)) {
            araFramework_ = handler[kAraConfigSectionBase][kAraConfigKeyFramework].GetString();
        } else {
            araFramework_ = GetAraDir();
        }
        araFramework_ += kAraConfigValueFrameworkDefault;

        if (handler[kAraConfigSectionBase].HasMember(kAraConfigKeyCore)) {
            araCore_ = handler[kAraConfigSectionBase][kAraConfigKeyCore].GetString();
        } else {
            araCore_ = GetAraDir();
        }
        araCore_ += kAraConfigValueCoreDefault;

        if (handler[kAraConfigSectionBase].HasMember(kAraConfigKeySwcls)) {
            araSwcls_ = handler[kAraConfigSectionBase][kAraConfigKeySwcls].GetString();
        } else {
            araSwcls_ = GetAraDir();
        }
        araSwcls_ += kAraConfigValueSwclsDefault;

        if (handler[kAraConfigSectionBase].HasMember(kAraConfigKeyVar)) {
            araVar_ = handler[kAraConfigSectionBase][kAraConfigKeyVar].GetString();
        } else {
            araVar_ = GetAraDir();
        }
        araVar_ += kAraConfigValueVarDefault;
    } else {
        araFramework_ = GetAraDir() + kAraConfigValueFrameworkDefault;
        araCore_      = GetAraDir() + kAraConfigValueCoreDefault;
        araSwcls_     = GetAraDir() + kAraConfigValueSwclsDefault;
        araVar_       = GetAraDir() + kAraConfigValueVarDefault;
    }

    return 0;
}

Platform::Platform() noexcept
{
    char *envValue;
    if (!GetSysroot().empty()) {
        return;
    }

    /// Get the platform root path via environment variables.
    envValue = getenv(kEnvKeyAraSysroot);
    if (NULL != envValue) {
        araSysroot_ = envValue;
    } else {
        // 1. Called in em, araSysroot_ is not empty, so this step is not reached.
        // 2. Processes started by em have environment variables set, so this step is not reached.
        // 3. When a process is started independently, environment variables are not set, so this step is reached.
        std::cout << "\nError:environment variable " << kEnvKeyAraSysroot
                  << " not set, you should set it manully when you start process "
                     "individually."
                  << std::endl;
        std::exit(-1);
        return;
    }

    if (Process::ProcessStartedIndependently()) {
        return;
    }

    /// Get the configuration file via environment variables.
    envValue = getenv(kEnvKeyAraConfigFileName);
    if (NULL != envValue) {
        araConfigFileName_ = envValue;
    } else {
        LOG_ERROR("env " << kEnvKeyAraConfigFileName << " is not set, process may started independentlly."
                         << std::endl);
        return;
    }

    if (0 != LoadConfig()) {
        LOG_ERROR("LoadConfig failed." << std::endl);
        araConfigFileName_.clear();
        return;
    }
}

/// @brief Get the software collection root directory path.
/// @param swclName - Software collection name.
/// @param swclVersion - Software collection version.
/// @returns On success, software collection root directory path.
///         e.g., ${ARA_SYSROOT}/ara/swcls/Radar/x.x.x/
///         Or: ${ARA_SYSROOT}/ara/core/x.x.x/
///         Or: ${ARA_SYSROOT}/ara/framework/x.x.x/
/// @returns On failure, empty string.
std::string Platform::GetSwclRootDir(const std::string &swclName, const std::string &swclVersion) const noexcept
{
    std::string ret;
    while (true) {
        if (swclName.empty() || swclVersion.empty()) {
            ret.clear();
            break;
        }
        ret = GetAraDir();
        if (ret.empty()) {
            ret.clear();
            break;
        }

        if (kCore == swclName) {
            if (araCore_.empty()) {
                ret.clear();
            } else {
                ret = araCore_ + "/" + swclVersion + "/";
            }
            break;
        } else if (kFramework == swclName) {
            if (araFramework_.empty()) {
                ret.clear();
            } else {
                ret = araFramework_ + "/" + swclVersion + "/";
            }
            break;
        } else {
            if (araSwcls_.empty()) {
                ret.clear();
            } else {
                ret = araSwcls_ + "/" + swclName + "/" + swclVersion + "/";
            }
            break;
        }
        break;
    }
    return ret;
}

std::string Platform::GetApplicationSwclsManifest() const noexcept
{
    // return araSysroot + kDefaultAraSwcls + procListName;

    static const char *kSectionName  = "run_time_application_swcl_list";
    static const char *kManifestName = "file_name";

    std::string configPath = GetApplicationSwclsManifestConfig();
    std::string ret;
    std::ifstream fileStream{configPath.c_str()};
    rapidjson::IStreamWrapper jsonStream{fileStream};
    rapidjson::Document handler;
    handler.ParseStream(jsonStream);
    if (handler.HasParseError()) {
        std::cout << "parse " << configPath << " error.\n";
        return ret;
    }

    std::string manifestName;
    if (handler.HasMember(kSectionName)) {
        if (handler[kSectionName].HasMember(kManifestName)) {
            manifestName = handler[kSectionName][kManifestName].GetString();
        } else {
            std::cout << kManifestName << " lacked in " << kSectionName << "\n";
            return ret;
        }
    } else {
        std::cout << kSectionName << " lacked in " << configPath << "\n";
        return ret;
    }

    ret = GetAraSwclsDir();
    if (ret.empty()) {
        return ret;
    }
    ret += manifestName;
    return ret;
}

/// @brief Get the ara temporary directory path.
std::string Platform::GetAraTmpDir(const std::string &araSysroot) noexcept
{
    if (araSysroot.empty()) {
        return std::string();
    }

    // 1.
    // realpath is not used because it supports configuring araSysroot as a non-directory string when starting independently.
    // When started by EM, the araSysroot path does not have issues.

    // 2.
    // When the run directory does not exist in the system, use the directory configured by kAraRuntimeDir.

    // 3.
    // When starting independently, if araSysroot is not configured, a prompt message will be printed and the process will exit.
    // When started by EM, araSysroot will definitely be configured.
    char *araRuntimeDir = getenv(kAraRuntimeDir);
    if (araRuntimeDir) {
        return std::string(araRuntimeDir) + "/" + GenMd5Str(araSysroot);
    } else {
        return kAraTmpDirPrefix + GenMd5Str(araSysroot);
    }
}

std::string Platform::GetUnixDomainSocketDir(const std::string &araSysroot) noexcept
{
    std::string tmpAraDir = GetAraTmpDir(araSysroot);
    if (tmpAraDir.empty())
        return std::string();

    return tmpAraDir + kIpcDir;
}

std::string Platform::GetEmdLockFile(const std::string &araSysroot) noexcept
{
    std::string tmpAraDir = GetAraTmpDir(araSysroot);
    if (tmpAraDir.empty())
        return std::string();

    return tmpAraDir + "/" + kAraEmdLockFile;
}

std::string Platform::GenMd5Str(const std::string data) noexcept
{
    EVP_MD_CTX *const pCtx{EVP_MD_CTX_new()};
    if (pCtx == nullptr) {
        return std::string{};
    }

    if (EVP_DigestInit_ex(pCtx, EVP_md5(), nullptr) != 1) {
        EVP_MD_CTX_free(pCtx);
        return std::string{};
    }

    if (EVP_DigestUpdate(pCtx, data.data(), data.size()) != 1) {
        EVP_MD_CTX_free(pCtx);
        return std::string{};
    }

    std::vector< uint8_t > digest(static_cast< size_t >(EVP_MD_get_size(EVP_md5())));
    unsigned int outLen{0};
    if (EVP_DigestFinal_ex(pCtx, digest.data(), &outLen) != 1) {
        EVP_MD_CTX_free(pCtx);
        return std::string{};
    }

    EVP_MD_CTX_free(pCtx);

    uint32_t const kTwoBytes{2U};
    std::ostringstream oss;
    oss << std::hex << std::setfill('0');
    for (size_t i{0U}; i < outLen; ++i) {
        oss << std::setw(kTwoBytes) << static_cast< int >(digest.data()[i]);
    }
    return oss.str();
}

std::int32_t Platform::ExportEnvironment(std::vector< std::string > &envList) const noexcept
{
    if (araSysroot_.empty()) {
        return -1;
    }
    if (araConfigFileName_.empty()) {
        return -2;
    }
    envList.clear();
    envList.emplace_back(kEnvKeyAraSysroot + std::string("=") + araSysroot_);
    envList.emplace_back(kEnvKeyAraConfigFileName + std::string("=") + araConfigFileName_);
    return 0;
}

std::string Platform::GetPlatformEtcDir(void) const noexcept
{
    std::string ret;
    if (!araSysroot_.empty() && !araCore_.empty() && !araCoreVersion_.empty()) {
        ret = araCore_ + "/" + araCoreVersion_ + kEtc;
    }

    if (ret.empty()) {
        char *configDir = getenv(kEnvKeyMachineConfigDir);
        if (configDir) {
            ret = std::string(configDir);
        }
    }

    if (ret.empty()) {
        // example: /home/wangyanlong/ara-sysroot/ara/framework/1.1.0/bin/phmd
        std::string exePath = Process::GetExePath();

        uint32_t lastSeparator = exePath.find_last_of("/");
        if (lastSeparator == std::string::npos) {
            return ret;
        }

        ret = exePath.substr(0, lastSeparator + 1);
    }

    if (getenv(kEnvFshDebug)) {
        std::cout << "platform etc dir:" << ret << std::endl;
    }
    return ret;
}

}  // namespace ara_fsh
}  // namespace isoft