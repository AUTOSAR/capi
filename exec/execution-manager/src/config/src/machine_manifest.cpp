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
/// @file       machine_manifest.cpp
/// @brief      ara configuration series machine manifest class
/// @details
/// @date       2023-03-06
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/ExecutionManagement/Config
/// @unit_name=MachineManifest
/// @unit_description=Used to read and save the machine manifest configuration.
/// @interface_level=module
/// @endcode
///
/// ================================================================

#include "ara/exec/internal/config/machine_manifest.h"

#include <isoft/manifestreader/manifest_reader.h>
#include <nai/os/nai_proc.h>

#include "ara/exec/internal/config/log.h"

/// @brief Internal environment variable buffer size
/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
#define ARA_EXEC_INERNAL_ENV_BUFF_SIZE 4096U

/// @brief Environment variable LD_LIBRARY_PATH name
/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
#define ENV_LIBPATH "LD_LIBRARY_PATH"

namespace ara {
namespace exec {
namespace internal {
namespace config {

/// @brief Redefine character type
using Char8_t = char;

/// @brief Load resource group configuration
/// @param node Resource group configuration node
/// @return 0 success; <0 failure
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_EM_00108
/// @trace_id_dd=DD_EM_00041
/// @needwork = dda
/// @endcode
int32_t ResourceGroup::ManifestLoader(isoft::manifestreader::ManifestNode const& node) noexcept
{
    name_ = node.GetValue(kName, ara::core::String());
    if (name_.empty()) {
        LOGE() << "ResourceGroup::ManifestLoader(): empty name";
        return isoft::kFailure;
    }

    cpuUsage_ = node.GetValue< uint8_t >(kCpuUsage, 0U);
    memUsage_ = node.GetValue(kMemUsage, 0U);
    return isoft::kSuccess;
}

/// @brief Print debug information
/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
void ResourceGroup::Debug() const noexcept
{
#if ARA_EXEC_DEBUG
    std::cout << "+++++ ResourceGroup::Debug +++++\n";
    std::cout << "Name: " << GetName() << std::endl;
    std::cout << "CpuUsage: " << static_cast< uint32_t >(GetCpuUsage()) << std::endl;
    std::cout << "MemUsage: " << GetMemUsage() << std::endl;
    std::cout << std::endl;
    std::cout << "----- ResourceGroup::Debug -----\n";
#endif  ///< ARA_EXEC_DEBUG
}

/// @brief Load the manifest file
/// @param manifestPath Manifest json file path
/// @return 0 success; <0 failure
/// @code{.isoft}
/// @interface_level=none
/// @trace_id_ad=AD_EM_00108
/// @trace_id_dd=DD_EM_00055
/// @needwork = dda
/// @endcode
int32_t MachineManifest::_Load(ara::core::String const& manifestPath) noexcept
{
    ara::core::Result< std::unique_ptr< isoft::manifestreader::Manifest > > manifestRes{
        isoft::manifestreader::OpenManifest(manifestPath)};
    if (!manifestRes.HasValue()) {
        LOGE() << "MachineManifest::Load: OpenManifest(" << manifestPath << ") failed !!!";
        return -1;
    }
    std::unique_ptr< isoft::manifestreader::Manifest > const& manifest{std::move(manifestRes).Value()};

    defaultEnterTimeout_ = manifest->GetValue(kDefaultEnterTimeout, 0.0);
    defaultExitTimeout_  = manifest->GetValue(kDefaultExitTimeout, 0.0);

    ara::core::Vector< ara::core::String > envs;
    std::ignore = manifest->Load(kEnvironments, envs);
    for (auto const& env : envs) {  // PRQA S 2961
        std::size_t const pos{env.find("=")};
        if (pos != ara::core::String::npos) {
            _AddEnvironment(env);
        } else {
            /// @code{.isoft}
            /// @trace_id_sws=SWS_EM_02249 If the environment variable has no value, then an empty string should be treated as the value of the KEY
            /// @endcode
            _AddEnvironment(env + "=");
        }
    }

    if (isoft::kSuccess != manifest->Load(kProcessor, processors_)) {
        LOGE() << "lack of {" << kProcessor << "}";
        return -1;
    }

    ara::core::String trustStr;
    if (isoft::kSuccess != manifest->Load(kTrustExecLaunchBehavior, trustStr)) {
        LOGE() << "lack of {" << kTrustExecLaunchBehavior << "}";
        return -1;
    }

    if (true != isoft::manifestreader::tps::FromString(trustStr, trustPlatformLaunchBehavior_)) {
        trustPlatformLaunchBehavior_ = TrustedPlatformLaunchBehaviorEnum::kNoTrustedPlatformSupport;
        LOGW() << "lack of {" << kTrustExecLaunchBehavior << "}, using default kNoTrustedPlatformSupport";
    }

    if (isoft::kSuccess != manifest->Load(kResourceGroups, resourceGroups_)) {
        LOGE() << "lack of {" << kResourceGroups << "}";
        return -1;
    }

    /// Read environment variables of interest and add them to the machine manifest
    /// NOTE: If the user configures LD_LIBRARY_PATH in the machine manifest, it will cause an overwrite
    ///     This is determined by the priority of environment variables: execution manifest > machine manifest > system environment variables
    ara::core::Vector< Char8_t > envBuff;
    envBuff.resize(ARA_EXEC_INERNAL_ENV_BUFF_SIZE);
    int64_t const r{nai_get_env(ENV_LIBPATH, envBuff.data(), ARA_EXEC_INERNAL_ENV_BUFF_SIZE)};
    if ((r >= 0) && (r <= static_cast< int64_t >(ARA_EXEC_INERNAL_ENV_BUFF_SIZE))) {
        _AddEnvironment(ara::core::String{ENV_LIBPATH} + "="
                        + ara::core::String{envBuff.data(), static_cast< std::size_t >(r)});
    } else {
        LOGW() << "the environment {" << ENV_LIBPATH << "} have no effect, because it need size {" << r
               << "}, but the buffer size {" << ARA_EXEC_INERNAL_ENV_BUFF_SIZE << "}";
    }

    return 0;
}

/// @brief Print debug information
/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
void MachineManifest::Debug() const noexcept
{
#if ARA_EXEC_DEBUG
    // PRQA S 2961 ++
    std::cout << "+++++ MachineManifest::Debug +++++\n";
    std::cout << "DefaultEnterTimeout:\t" << GetDefaultEnterTimeout() << std::endl;
    std::cout << "DefaultExitTimeout:\t" << GetDefaultExitTimeout() << std::endl;

    std::cout << "Environments:" << std::endl;
    for (auto const& it : GetEnvironments()) {
        std::cout << "\t" << it << std::endl;
    }

    std::cout << "TrustExecLaunchBehavior:\t" << isoft::manifestreader::tps::ToString(GetTrustPlatformLaunchBehavior())
              << std::endl;

    std::cout << "Processor:\t";
    for (uint8_t const id : GetProcessors()) {
        std::cout << static_cast< int32_t >(id) << " ";
    }
    std::cout << std::endl;

    std::cout << "ResourceGroups:\t" << GetResourceGroups().size() << std::endl;
    for (auto const& rg : GetResourceGroups()) {
        rg.Debug();
    }

    std::cout << "----- MachineManifest::Debug -----\n";
    // PRQA S 2961 --
#endif  ///< ARA_EXEC_DEBUG
}

}  // namespace config
}  // namespace internal
}  // namespace exec
}  // namespace ara