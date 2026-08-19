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
/// @file       path.cpp
/// @brief      ara path implementation
/// @details
/// @date       2024-08-15
/// @author     cuiyinli
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/UCM/Utils
/// @interface_level=unit
/// @trace_id_sr=SR_UCM_00001
/// @unit_name=Path
/// @unit_description=Global path settings implementation
/// @endcode
///
/// ================================================================

#include "path.h"

#include <isoft/ara_fsh/filesystem_hierarchy.h>

#include "const.h"

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @brief global path settings
std::unique_ptr< Path > Path::s_Instance_{nullptr};  // NOLINT

/// @brief get instance
/// @throws no
/// @return instance
Path& Path::GetInstance()
{
    if (nullptr == s_Instance_) {
        s_Instance_ = std::make_unique< Path >();
    }
    return *s_Instance_;
}

/// @brief load path settings
/// @throws no
void Path::Load() noexcept
{
    isoft::ara_fsh::Platform const platform;

    settings_.araDir       = platform.GetAraDir();
    settings_.frameworkDir = platform.GetFrameworkDir();
    settings_.coreDir      = platform.GetCoreDir();
    settings_.swclsDir     = platform.GetAraSwclsDir();

    settings_.varDataDir       = platform.GetAraVarDir() + "/ucm/";
    settings_.swpkgsDir        = settings_.varDataDir + "/swps";
    settings_.historyPath      = settings_.varDataDir + "/history.json";
    settings_.uCMStatusPath    = settings_.varDataDir + kPathSeparator + kUCMStatusFileName;
    settings_.uCMStatusMD5Path = settings_.varDataDir + kPathSeparator + kUCMStatusMD5FileName;

    settings_.machineManifestPath = platform.GetMachineManifest();

    isoft::ara_fsh::Process const process;
    settings_.executionManifestPath = process.GetExecutionManifest();
}

/// @brief str of global path settings
/// @throws no
/// @return str
AraString Path::Str() const
{
    return "Path{AraDir=" + settings_.araDir + ", FrameworkDir=" + settings_.frameworkDir + ", coreDir="
           + settings_.coreDir + ", SwclsDir=" + settings_.swclsDir + ", varDataDir=" + settings_.varDataDir
           + ", swpkgsDir=" + settings_.swpkgsDir + ", historyPath=" + settings_.historyPath + ", uCMStatusPath="
           + settings_.uCMStatusPath + ", uCMStatusMD5Path=" + settings_.uCMStatusMD5Path + ", machineManifestPath="
           + settings_.machineManifestPath + ", executionManifestPath=" + settings_.executionManifestPath + "}";
}

/// @brief trim the last separator
/// @param dir
/// @throws no
/// @return str
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10399
/// @trace_id_dd=DD_UCM_11116
/// @needwork = dd
/// @endcode
static AraString TrimLastSeparator(AraString const& dir)
{
    if (dir.empty()) {
        return dir;
    }

    const std::size_t endPos = dir.find_last_not_of('/');
    if (endPos == AraString::npos) {
        return "";
    }

    return dir.substr(0U, endPos + 1U);
}

/// @brief swcl path
/// @param name swcl name
/// @throws no
/// @return str
AraString Path::GetSwclInstallDir(AraString const& name) const
{
    if (0 == name.compare(kFRAMEWORK_SWCL_NAME)) {
        return TrimLastSeparator(settings_.frameworkDir);
    }

    if (0 == name.compare(kCORE_SWCL_NAME)) {
        return TrimLastSeparator(settings_.coreDir);
    }

    return settings_.swclsDir + kPathSeparator + name;
}

/// @brief swcl path
/// @param name swcl name
/// @param version swcl version
/// @throws no
/// @return str
AraString Path::GetSwclInstallDir(AraString const& name, AraString const& version) const
{
    return GetSwclInstallDir(name) + kPathSeparator + version;
}

/// @brief swcl path
/// @param name swcl name
/// @throws no
/// @return str
AraString Path::GetSwclStatusPath(AraString const& name) const
{
    return GetSwclInstallDir(name) + kPathSeparator + kSwclStatusFileName;
}

/// @brief swcl path
/// @param name swcl name
/// @param version swcl version
/// @throws no
/// @return str
AraString Path::GetSwclManifestPath(AraString const& name, AraString const& version) const
{
    return GetSwclInstallDir(name, version) + kPathSeparator + kSwclManifestFileName;
}

/// @brief swcl path
/// @param name swcl name
/// @param version swcl version
/// @throws no
/// @return str
AraString Path::GetSwpkgManifestPath(AraString const& name, AraString const& version) const
{
    return GetSwclInstallDir(name, version) + kPathSeparator + kSwpkgManifestFileName;
}

/// @brief swcl path
/// @param fullPath path
/// @throws no
/// @return str
AraString Path::GetExtractionTargetDir(AraString const& fullPath)
{
    constexpr AraStringView kExtension{".zip"};
    return fullPath.substr(0U, fullPath.size() - kExtension.size());
}

/// @brief get global path settings
/// @throws no
/// @return path
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10406
/// @trace_id_dd=DD_UCM_11126
/// @needwork = dd
/// @endcode
Path& GetPath() { return Path::GetInstance(); }

/// @brief get global path settings
/// @throws no
/// @return path settings
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10407
/// @trace_id_dd=DD_UCM_11127
/// @needwork = dd
/// @endcode
Path::PathSettings const& GetPathSettings() { return Path::GetInstance().GetPathSettings(); }

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara
