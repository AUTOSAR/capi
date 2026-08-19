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
/// @file       fg_service_info_config.cpp
/// @brief      Read configuration files related to function group services
/// @details
/// @date       2024-04-30
/// @author     lianglongxiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/StateManagement/Config
/// @interface_level=uint
/// @trace_id_sr=SR_SM_01004
/// @unit_name=FGServiceInfoConfig
/// @unit_description=Read configuration files related to function group services
/// @endcode
///
/// ================================================================

#include "fg_service_info_config.h"

#include <isoft/manifestreader/manifest_reader.h>

#include <algorithm>

#include "define.h"

namespace ara {
namespace sm {
namespace config {

/// @brief Constructor function
FGServiceInfoConfig::FGServiceInfoConfig() noexcept
    : log_{log::CreateLogger((core::StringView{"#CFG"}), (core::StringView{"Config context"}))}, fgServiceInfoList_{}
{
    log_.LogInfo() << "FGServiceInfoConfig::FGServiceInfoConfig()";
}

/// @brief Destructor function
FGServiceInfoConfig::~FGServiceInfoConfig() noexcept
{
    log_.LogInfo() << "FGServiceInfoConfig::~FGServiceInfoConfig()";
}

/// @brief Get the list of all function group service information
/// @return List of function group service information
core::Vector< FGServiceInfo > const &FGServiceInfoConfig::GetAllFGServiceInfoList() const noexcept
{
    return fgServiceInfoList_;
}

/// @brief Load all funtion groups' service info list
/// @param manifestPath config file path
/// @return true - success
/// @return false - failure
bool FGServiceInfoConfig::Load(core::String const &manifestPath) noexcept
{
    log_.LogInfo() << "FGServiceInfoConfig::Load() begin, got manifestPath:" << manifestPath.c_str();

    fgServiceInfoList_.clear();
    /// @brief Type simplification definition
    using ManifestRes = core::Result< std::unique_ptr< isoft::manifestreader::Manifest > >;
    ManifestRes machineManifestRes{isoft::manifestreader::OpenManifest((core::StringView{manifestPath.c_str()}))};
    if (machineManifestRes) {
        std::unique_ptr< isoft::manifestreader::Manifest > const machineManifest{std::move(machineManifestRes).Value()};

        // Load TriggerIn_State interface service instance information
        {
            std::ignore = machineManifest->IterateArray(
                (core::StringView{common::GetkTriggerInFGServiceInstances()}),
                [this](std::size_t, isoft::manifestreader::ManifestNode const &fGServiceIntanceInfoNode) {
                    // Function group fqn
                    core::String const fgFQN{fGServiceIntanceInfoNode.GetValue< core::String >(
                        (core::StringView{common::GetkFunctionGroupFQN()}), core::String{""})};

                    // Instance id
                    core::String const intanceID{fGServiceIntanceInfoNode.GetValue< core::String >(
                        (core::StringView{common::GetkInstanceID()}), core::String{""})};

                    _appendFGServiceInfo(fgFQN, common::GetkTriggerInFGServiceInstances(), intanceID);
                });
        }

        // Load TriggerOut_State interface service instance information
        {
            std::ignore = machineManifest->IterateArray(
                (core::StringView{common::GetkTriggerOutFGServiceInstances()}),
                [this](std::size_t, isoft::manifestreader::ManifestNode const &fGServiceIntanceInfoNode) {
                    // Function group fqn
                    core::String const fgFQN{fGServiceIntanceInfoNode.GetValue< core::String >(
                        (core::StringView{common::GetkFunctionGroupFQN()}), core::String{""})};

                    // Instance id
                    core::String const intanceID{fGServiceIntanceInfoNode.GetValue< core::String >(
                        (core::StringView{common::GetkInstanceID()}), core::String{""})};

                    _appendFGServiceInfo(fgFQN, common::GetkTriggerOutFGServiceInstances(), intanceID);
                });
        }

        // Load TriggerInOut_State interface service instance information
        {
            std::ignore = machineManifest->IterateArray(
                (core::StringView{common::GetkTriggerIOFGServiceInstances()}),
                [this](std::size_t, isoft::manifestreader::ManifestNode const &fGServiceIntanceInfoNode) {
                    // Function group fqn
                    core::String const fgFQN{fGServiceIntanceInfoNode.GetValue< core::String >(
                        (core::StringView{common::GetkFunctionGroupFQN()}), core::String{""})};

                    // Instance id
                    core::String const intanceID{fGServiceIntanceInfoNode.GetValue< core::String >(
                        (core::StringView{common::GetkInstanceID()}), core::String{""})};

                    _appendFGServiceInfo(fgFQN, common::GetkTriggerIOFGServiceInstances(), intanceID);
                });
        }
    }
    return true;
}

/// @brief Add function group service information to fgServiceInfoList_
/// @param fgFQN Function group FQN
/// @param type Service type
/// @param instanceId Instance ID
void FGServiceInfoConfig::_appendFGServiceInfo(core::String const &fgFQN,
                                               core::String const &type,
                                               core::String const &instanceId) noexcept
{
    log_.LogInfo() << "FGServiceInfoConfig::_appendFGServiceInfo() begin, fgFQN:" << fgFQN.c_str()
                   << "type:" << type.c_str() << "instanceId:" << instanceId.c_str();
    core::Vector< ara::sm::config::FGServiceInfo >::iterator const it{
        std::find_if(fgServiceInfoList_.begin(), fgServiceInfoList_.end(),
                     [&fgFQN](FGServiceInfo const &info) noexcept { return info.fgFQN == fgFQN; })};
    if (fgServiceInfoList_.end() == it) {
        log_.LogInfo() << "FGServiceInfoConfig::_appendFGServiceInfo(), can't find FGServiceInfo with fgFQN:"
                       << fgFQN.c_str() << "insert a new one";
        FGServiceInfo newFgServiceInfo;
        newFgServiceInfo.fgFQN = fgFQN;
        std::ignore            = newFgServiceInfo.fgInstanceID.insert(std::make_pair(type, instanceId));
        fgServiceInfoList_.push_back(std::move(newFgServiceInfo));
    } else {
        log_.LogInfo() << "FGServiceInfoConfig::_appendFGServiceInfo(), find FGServiceInfo with fgFQN:"
                       << fgFQN.c_str();
        ara::sm::config::FGServiceInfo &fgServiceInfo{*it};
        core::Map< core::String, core::String >::iterator const typeIt{fgServiceInfo.fgInstanceID.find(type)};
        if (fgServiceInfo.fgInstanceID.end() == typeIt) {
            log_.LogInfo() << "FGServiceInfoConfig::_appendFGServiceInfo(), insert type:" << type.c_str()
                           << "and instanceId:" << instanceId.c_str() << "for fgFQN:" << fgFQN.c_str();
            std::ignore = fgServiceInfo.fgInstanceID.insert(std::make_pair(type, instanceId));
        } else {
            log_.LogWarn() << "FGServiceInfoConfig::_appendFGServiceInfo(), find existed type:" << type.c_str()
                           << "for fgFQN:" << fgFQN.c_str() << "skip insert this type";
        }
    }
}

}  // namespace config
}  // namespace sm
}  // namespace ara