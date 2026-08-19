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
/// @file       function_group_set.cpp
/// @brief      ara configuration series function group set manifest class
/// @details
/// @date       2024-03-28
/// @author     xueliang.bao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/ExecutionManagement/Config
/// @unit_name=FunctionGroupManifest
/// @unit_description=Used to read and save the function group configuration.
/// @interface_level=module
/// @endcode
///
/// ================================================================

#include "ara/exec/internal/config/function_group_set.h"

#include <isoft/manifestreader/manifest_reader.h>

#include "ara/exec/internal/config/software_cluster_manifest.h"
#include "ara/exec/internal/log/log.h"
#include "isoft/ara_fsh/platform.h"

namespace ara {
namespace exec {
namespace internal {
namespace config {

/// @brief Load the specified function group information from Node
/// @param node Node
/// @return 0 success; <0 failure
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_EM_00110
/// @trace_id_dd=DD_EM_00231
/// @needwork = dda
/// @endcode
int32_t FunctionGroup::ManifestLoader(isoft::manifestreader::ManifestNode const& node) noexcept
{
    int32_t ret{0};
    if (isoft::kSuccess != node.Load(kName, name_)) {
        LOGE() << "lack of {" << kName << "}";
        ret = -1;
    }

    if (isoft::kSuccess != node.Load(kStates, states_)) {
        LOGE() << "lack of {" << kStates << "}";
        ret = -1;
    }

    if (ret == 0) {
        hasConfigError_ = false;
    }

    return 0;
}

/// @brief Whether the configuration is correct
/// @return true configuration error; false configuration correct
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_EM_00110
/// @trace_id_dd=DD_EM_00232
/// @needwork = dda
/// @endcode
bool FunctionGroup::HasConfigError() const noexcept
{
    if (hasConfigError_) {
        return true;
    }

    if (name_.empty()) {
        return true;
    }

    for (auto const& state : states_) {  // PRQA S 2961
        if (state.empty()) {
            return true;
        }
    }

    return false;
}

/// @brief Print FunctionGroup information
/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
void FunctionGroup::Debug() const noexcept
{
#if ARA_EXEC_DEBUG
    std::cout << "+++++ FunctionGroup::Debug +++++\n";
    std::cout << "Name: " << GetName() << std::endl;
    std::cout << "States: ";
    for (auto const& s : GetStates()) {  // PRQA S 2961
        std::cout << " " << s;
    }
    std::cout << std::endl;
    std::cout << "----- FunctionGroup::Debug -----\n";
#endif  ///< ARA_EXEC_DEBUG
}

/// @brief Load all function group sets and create a function group set instance
/// @return Function group set instance
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_EM_00110
/// @trace_id_dd=DD_EM_00239
/// @needwork = dda
/// @endcode
std::shared_ptr< FunctionGroupSet > FunctionGroupSet::CreateInstance() noexcept
{
    std::shared_ptr< FunctionGroupSet > fgSet{std::make_shared< FunctionGroupSet >()};
    if (fgSet == nullptr) {
        LOGE() << "FunctionGroupSet::CreateInstance(): Make Shared Memory failed !!!";
        return nullptr;
    }

    isoft::ara_fsh::Platform const pf;
    fgSet->_LoadSwclFunctionGroups(ara::core::String{pf.GetPlatformSwclsManifest()});
    fgSet->_LoadSwclFunctionGroups(ara::core::String{pf.GetApplicationSwclsManifest()});

    return fgSet;
}

/// @brief Load function group information of the platform or user software cluster
/// @param swclPath Software cluster list path
/// @code{.isoft}
/// @interface_level=none
/// @trace_id_ad=AD_EM_00110
/// @trace_id_dd=DD_EM_00243
/// @needwork = dda
/// @endcode
void FunctionGroupSet::_LoadSwclFunctionGroups(ara::core::String const& swclPath) noexcept
{
    std::shared_ptr< SoftwareClusterManifest > const spSwclManifest{SoftwareClusterManifest::CreateInstance(swclPath)};
    if (nullptr == spSwclManifest) {
        return;
    }

    isoft::ara_fsh::Platform pf;
    ara::core::Vector< SoftwareCluster > const& swclList{spSwclManifest->GetSoftwareClusters()};
    for (auto const& swcl : swclList) {  // PRQA S 2961
        if (swcl.HasConfigError()) {
            continue;
        }

        std::string const fgsPath{pf.GetSwclFuncGrpSet(swcl.GetName().c_str(), swcl.GetVersion().c_str())};
        if (0 != access(fgsPath.c_str(), F_OK | R_OK)) {
            continue;
        }

        std::shared_ptr< FunctionGroupSet > const fgSet{FunctionGroupSet::CreateInstance(ara::core::String{fgsPath})};
        if (fgSet == nullptr) {
            continue;
        }
        ara::core::Vector< FunctionGroup > const& fgList{fgSet->GetFunctionGroups()};
        for (auto const& fg : fgList) {  // PRQA S 2961
            if (fg.HasConfigError()) {
                continue;
            }

            functionGroups_.emplace_back(fg);
        }
    }

    return;
}

/// @brief Load the function group set according to the function group set path, and create a function group set instance
/// @param fgSetPath Function group set manifest file path
/// @return Function group set instance
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_EM_00110
/// @trace_id_dd=DD_EM_00240
/// @needwork = dda
/// @endcode
std::shared_ptr< FunctionGroupSet > FunctionGroupSet::CreateInstance(ara::core::String const& fgSetPath) noexcept
{
    std::shared_ptr< FunctionGroupSet > fgSet{std::make_shared< FunctionGroupSet >()};
    if (fgSet == nullptr) {
        LOGE() << "FunctionGroupSet::CreateInstance(): Make Shared Memory failed !!!";
        return nullptr;
    }

    if (0 > fgSet->_Load(fgSetPath)) {
        fgSet.reset();
    }

    return fgSet;
}

/// @brief Load the function group set according to the function group set path
/// @param fgsPath Function group set manifest file path
/// @return Loading result, <0 loading failed; =0 loading successful
/// @code{.isoft}
/// @interface_level=none
/// @trace_id_ad=AD_EM_00110
/// @trace_id_dd=DD_EM_00242
/// @needwork = dda
/// @endcode
int32_t FunctionGroupSet::_Load(ara::core::String const& fgsPath) noexcept
{
    ara::core::Result< std::unique_ptr< isoft::manifestreader::Manifest > > manifestRes{
        isoft::manifestreader::OpenManifest(fgsPath)};
    if (!manifestRes.HasValue()) {
        LOGE() << "FunctionGroupSet::Load(): OpenManifest(" << fgsPath << ") failed !!!";
        return -1;
    }

    std::unique_ptr< isoft::manifestreader::Manifest > const manifest{std::move(manifestRes).Value()};
    if (isoft::kSuccess != manifest->Load(kFunctionGroups, functionGroups_)) {
        LOGE() << "lack of {" << kFunctionGroups << "}";
        return -1;
    }

    return 0;
}

}  // namespace config
}  // namespace internal
}  // namespace exec
}  // namespace ara
