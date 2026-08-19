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
/// @file       config.cpp
/// @brief      ara configuration class
/// @details
/// @date       2023-03-06
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================

#include "ara/loader/config.h"

#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>

#include <fstream>
#include <iostream>

#include "ara/loader/file.h"

namespace ara {
namespace loader {

/// @brief Load configuration
/// @param isRollBack - rollback mode, in this mode the oldest version of configuration will be loaded; normal mode loads the latest version
/// @return 0 success; <0 failure
int32_t Config::_Load(bool const isRollBack) noexcept
{
    std::string const searchRegStr{"ara_ver\\d+\\.json"};
    std::string const kAraConfigSectionBase{"base"};
    std::string const kAraConfigSectionUcm{"ucm"};
    std::string const kAraConfigKeyFramework{"platform_framework"};
    std::string const kAraConfigKeyCore{"platform_core"};
    std::string const kAraConfigKeyVar{"platform_var"};
    std::string const kAraConfigKeySwcls{"application_swcls"};
    std::string const kAraConfigKeyVersionCore{"core_version"};
    std::string const kAraConfigKeyVersionFramework{"framework_version"};

    std::string const kAraConfigValueSwclsDefault{"/swcls/"};
    std::string const kAraConfigValueVarDefault{"/var/"};
    std::string const kAraConfigValueCoreDefault{"/core/"};
    std::string const kAraConfigValueFrameworkDefault{"/framework/"};

    /// If it is recovery mode, read the configuration file with the smallest version
    if (isRollBack) {
        araConfigPath_
            = isoft::utils::file::Search(GetAraDir(), searchRegStr, isoft::utils::file::SearchPolicy::kMinValue);
        /// Otherwise, read the configuration file with the largest (latest) version
    } else {
        araConfigPath_
            = isoft::utils::file::Search(GetAraDir(), searchRegStr, isoft::utils::file::SearchPolicy::kMaxValue);
    }

    if (araConfigPath_.empty()) {
        std::cout << "araConfigPath_.empty()" << std::endl;
        return -1;
    }

    std::ifstream fileStream{araConfigPath_.c_str()};
    rapidjson::IStreamWrapper jsonStream{fileStream};
    rapidjson::Document handler;
    if (handler.ParseStream(jsonStream).HasParseError()) {  // PRQA S 4127
        std::cout << "parse " << araConfigPath_ << " error.\n";
        return -1;
    }
    if (handler.HasMember(kAraConfigSectionUcm.c_str())) {
        if (handler[kAraConfigSectionUcm.c_str()].HasMember(kAraConfigKeyVersionFramework.c_str())) {
            frameworkVersion_
                = handler[kAraConfigSectionUcm.c_str()][kAraConfigKeyVersionFramework.c_str()].GetString();
        } else {
            std::cout << "framework version version lacked in " << araConfigPath_ << "\n";
            return -1;
        }

        if (handler[kAraConfigSectionUcm.c_str()].HasMember(kAraConfigKeyVersionCore.c_str())) {
            coreVersion_ = handler[kAraConfigSectionUcm.c_str()][kAraConfigKeyVersionCore.c_str()].GetString();
        } else {
            std::cout << "core version lacked in " << araConfigPath_ << "\n";
            return -1;
        }
    } else {
        std::cout << "ucm lacked in " << araConfigPath_ << "\n";
        return -1;
    }

    if (handler.HasMember(kAraConfigSectionBase.c_str())) {
        if (handler[kAraConfigSectionBase.c_str()].HasMember(kAraConfigKeyFramework.c_str())) {
            frameworkDir_ = handler[kAraConfigSectionBase.c_str()][kAraConfigKeyFramework.c_str()].GetString();
        } else {
            frameworkDir_ = GetAraDir();
        }
        frameworkDir_ += kAraConfigValueFrameworkDefault;

        if (handler[kAraConfigSectionBase.c_str()].HasMember(kAraConfigKeyCore.c_str())) {
            coreDir_ = handler[kAraConfigSectionBase.c_str()][kAraConfigKeyCore.c_str()].GetString();
        } else {
            coreDir_ = GetAraDir();
        }
        coreDir_ += kAraConfigValueCoreDefault;

        if (handler[kAraConfigSectionBase.c_str()].HasMember(kAraConfigKeySwcls.c_str())) {
            swclsDir_ = handler[kAraConfigSectionBase.c_str()][kAraConfigKeySwcls.c_str()].GetString();
        } else {
            swclsDir_ = GetAraDir();
        }
        swclsDir_ += kAraConfigValueSwclsDefault;

        if (handler[kAraConfigSectionBase.c_str()].HasMember(kAraConfigKeyVar.c_str())) {
            varDir_ = handler[kAraConfigSectionBase.c_str()][kAraConfigKeyVar.c_str()].GetString();
        } else {
            varDir_ = GetAraDir();
        }
        varDir_ += kAraConfigValueVarDefault;
    } else {
        frameworkDir_ = GetAraDir() + kAraConfigValueFrameworkDefault;
        coreDir_      = GetAraDir() + kAraConfigValueCoreDefault;
        swclsDir_     = GetAraDir() + kAraConfigValueSwclsDefault;
        varDir_       = GetAraDir() + kAraConfigValueVarDefault;
    }

    return 0;
}

/// @brief Print debug information
void Config::Debug() const noexcept
{
    std::cout << "+++++++++++++++++++++ araLoader::Config::Debug() +++++++++++++++++++++" << std::endl;
    std::cout << "AraSysrootDir: " << araSysroot_ << std::endl;
    std::cout << "AraConfig: " << araConfigPath_ << std::endl;
    std::cout << "FrameworkDir: " << GetFrameworkDir() << std::endl;
    std::cout << "FrameworkVersion: " << GetFrameworkVersion() << std::endl;
    std::cout << "CoreDir: " << GetCoreDir() << std::endl;
    std::cout << "CoreVersion: " << GetCoreVersion() << std::endl;
    std::cout << "VarDir: " << GetVarDir() << std::endl;
    std::cout << "SwclsDir: " << GetSwclsDir() << std::endl;
    std::cout << "--------------------- araLoader::Config::Debug() ---------------------" << std::endl;
}

}  // namespace loader
}  // namespace ara