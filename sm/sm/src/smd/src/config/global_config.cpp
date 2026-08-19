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
/// @file       global_config.cpp
/// @brief      Unified interface for various configurations
/// @details
/// @date       2024-05-02
/// @author     lianglongxiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/StateManagement/Config
/// @interface_level=module
/// @trace_id_sr=SR_SM_00001, SR_SM_00002, SR_SM_01004, SR_SM_00101, SR_SM_00104, SR_SM_00105, SR_SM_05001, SR_SM_00107, SR_SM_09002
/// @unit_name=GlobalConfig
/// @unit_description=Unified interface for various configurations
/// @endcode
///
/// ================================================================

#include "global_config.h"

#include <ara/core/optional.h>
#include <isoft/ara_fsh/filesystem_hierarchy.h>

#include "define.h"
#include "helper.h"
#ifdef ARA_WITH_PER
    #include "storage/storage.h"
#endif
namespace ara {
namespace sm {
namespace config {

/// @brief Constructor function
GlobalConfig::GlobalConfig() noexcept
    : fgInfoConfigInstance_{}
    , fgServiceInfoConfigInstance_{}
    , nmCofigInstance_{}
    , stateMachineConfigInstance_{}
    , fgInfoList_{}
    , log_{log::CreateLogger((core::StringView{"#CFG"}), (core::StringView{"Config context"}))}
{
    log_.LogInfo() << "GlobalConfig::GlobalConfig()";
}
/// @brief Destructor function
GlobalConfig::~GlobalConfig() noexcept { log_.LogInfo() << "GlobalConfig::~GlobalConfig()"; }

/// @brief Get the mapping of function group states to FullCom network list.
/// When a function group is in these states, the corresponding network should be set to FullCom.
/// When a function group is not in these states, the corresponding network should be set to NoCom.
/// @return core::Map<Function group FQN, core::Map<Function group state, core::Vector<Network>>>
core::Map< core::String, core::Map< core::String, core::Vector< core::String > > > const &
GlobalConfig::GetFGState2NMHandlesMap() const noexcept
{
    return nmCofigInstance_.GetFGState2NMHandlesMap();
}

/// @brief Get the mapping of FullCom network to function group state list.
/// When the network is in FullCom, switch the function group state to the state in the Map.
/// When the network is in NoCom, switch the function group state to OFF.
/// @return core::Map<Network, core::Map<Function group FQN, core::Vector<Function group state>>>
core::Map< core::String, core::Map< core::String, core::String > > const &
GlobalConfig::GetFullCommNMHandle2FGStatesMap() const noexcept
{
    return nmCofigInstance_.GetFullCommNMHandle2FGStatesMap();
}

/// @brief Get the mapping of Off function group to NoCom network list
/// @return core::Map<Function group FQN, core::Vector<Network>>
core::Map< core::String, core::Vector< core::String > > const &GlobalConfig::GetOffFG2NoComNMHandlesMap() const noexcept
{
    return nmCofigInstance_.GetOffFG2NoComNMHandlesMap();
}

/// @brief Get the mapping of NoCom network to function group state list
/// @return core::Map<Network, core::Map<core::String, core::String>>&
core::Map< core::String, core::Map< core::String, core::String > > const &GlobalConfig::GetNoComNMHandle2FGStatesMap()
    const noexcept
{
    return nmCofigInstance_.GetNoComNMHandle2FGStatesMap();
}

/// @brief Get the list of function group service information. The function group must be 'active'.
/// @return List of function group service information
core::Vector< FGServiceInfo > const &GlobalConfig::GetAllFGServiceInfoList() const noexcept
{
    return fgServiceInfoConfigInstance_.GetAllFGServiceInfoList();
}

/// @brief Get the list of all function group information
/// @return List of function group information
core::Vector< FGInfo > const &GlobalConfig::GetAllFGInfoList() const noexcept { return fgInfoList_; }

/// @brief Get the list of all network service information
/// @return List of network service information
core::Vector< NMServiceInfo > const &GlobalConfig::GetAllNMServiceInfoList() const noexcept
{
    return nmCofigInstance_.GetAllNMServiceInfoList();
}

/// @brief Get the list of all state machine service information
/// @return List of state machine service information
core::Vector< SMServiceInfo > const &GlobalConfig::GetAllSMServiceInfoList() const noexcept
{
    return stateMachineConfigInstance_.GetAllSMServiceInfoList();
}

/// @brief Get the list of all state machine configuration information
/// @return List of state machine configuration information
core::Vector< SMConfigInfo > const &GlobalConfig::GetAllSMConfigInfoList() const noexcept
{
    return stateMachineConfigInstance_.GetAllSMConfigInfoList();
}

/// @brief Get the delay time information related to NM
/// @return Delay time information
NMAfterRunTimeInfo const &GlobalConfig::GetNMAfterRunInfo() const noexcept
{
    return nmCofigInstance_.GetNMAfterRunInfo();
}

/// @brief Get the mapping of physical address to function group FQN
/// @return Mapping of physical address to function group FQN
core::Map< int32_t, core::Vector< core::String > > const &GlobalConfig::GetPhysicalAddr2FGFQNs() const noexcept
{
    return fgInfoConfigInstance_.GetPhysicalAddr2FGFQNs();
}

/// @brief Get the mapping of functional address to function group FQN
/// @return Mapping of functional address to function group FQN
core::Map< int32_t, core::Vector< core::String > > const &GlobalConfig::GetFunctionalAddr2FGFQNs() const noexcept
{
    return fgInfoConfigInstance_.GetFunctionalAddr2FGFQNs();
}

/// @brief Get the FQN of MachineFG
/// @return FQN of MachineFG
core::String GlobalConfig::GetMachineFgFQN() const noexcept { return fgInfoConfigInstance_.GetMachineFgFQN(); }

/// @brief Load all configuration files
/// @return true - success
/// @return false - failure
bool GlobalConfig::Load() noexcept
{
    log_.LogInfo() << "GlobalConfig::Load()";
    if (!isLoaded_) {
        bool ret{fgInfoConfigInstance_.Load()};
        if (ret) {
            core::Vector< ara::sm::config::FGInfoSimple > const &fgInfoSimpleList{
                fgInfoConfigInstance_.GetAllFGInfoList()};
            fgInfoList_.clear();
            for (const auto &fgInfoSimple : fgInfoSimpleList) {
                FGInfo info;
                info.fgFQN         = fgInfoSimple.fgFQN;
                info.fgStates      = fgInfoSimple.fgStates;
                info.isDeactivated = false;
                log_.LogInfo() << "GlobalConfig::Load(), fgInfoList_ push_back info:" << info.fgFQN.c_str()
                               << "states:" << common::ConcatenateStrings(info.fgStates).c_str();
                fgInfoList_.push_back(std::move(info));
            }
            _deactivateFGsFromCalibrationData(fgInfoList_);
        } else {
            log_.LogWarn() << "GlobalConfig::Load(), FGInfo load error";
            return false;
        }

        // Load FGServiceInfo
        log_.LogInfo() << "GlobalConfig::Load(), load FGServiceInfo";
        isoft::ara_fsh::Process const fshProcess;
        std::string const fgServiceInfoManifestPath{fshProcess.GetTriggerFGService()};
        ret = fgServiceInfoConfigInstance_.Load(core::String(fgServiceInfoManifestPath));
        if (!ret) {
            log_.LogWarn() << "GlobalConfig::Load(), FGServiceInfo load error";
            return false;
        }

        // Load NM related Info
        isoft::ara_fsh::Platform const fshPlatform;
        std::string const platformEtcPath{fshPlatform.GetPlatformEtcDir()};
        core::String const nmConfigManifestPath{platformEtcPath + common::GetkPathSeparator()
                                                + common::GetkNMManifestFileName()};
        ret = nmCofigInstance_.Load(nmConfigManifestPath);
        if (!ret) {
            log_.LogWarn() << "GlobalConfig::Load(), NM infos load error";
            return false;
        }

        // Load state machine information
        std::string const stateMachineConfigManifestPath{fshProcess.GetStateMachineConf()};
        ret = stateMachineConfigInstance_.Load(core::String(stateMachineConfigManifestPath));
        if (!ret) {
            log_.LogWarn() << "GlobalConfig::Load(), StateMachine infos load error";
            return false;
        }
        isLoaded_ = true;
    } else {
        log_.LogWarn() << "GlobalConfig::Load(), config files have been loaded before, skip this time";
    }
    return true;
}

/// @brief Mark function groups as disabled based on calibration data
/// @param fgInfoList List of function groups
void GlobalConfig::_deactivateFGsFromCalibrationData(core::Vector< FGInfo > &fgInfoList) const noexcept
{
    log_.LogInfo() << "GlobalConfig::_deactivateFGsFromCalibrationData(), begin with:"
                   << common::GetkDeactivatedFunctionGroups();
#ifdef ARA_WITH_PER
    ara::sm::storage::Storage calibrationDataStorage{core::StringView(common::GetkKVModelCalibrationIdentifier())};
    // Persistent calibration data information
    bool const ret{calibrationDataStorage.Open()};
    if (!ret) {
        log_.LogInfo() << "GlobalConfig::_deactivateFGsFromCalibrationData(), failed to open calibrationDataStorage.";
        return;
    }
    core::Optional< core::String > deactivatedFgsOptional{
        calibrationDataStorage.Get< core::String >((core::StringView{common::GetkDeactivatedFunctionGroups()}))};
    if (deactivatedFgsOptional.has_value()) {
        core::String const deactivatedFgsValue{*deactivatedFgsOptional};
        log_.LogInfo() << "GlobalConfig::_deactivateFGsFromCalibrationData(), fgFQNsDeactivated:"
                       << deactivatedFgsValue.c_str();
        // Parse to get deactivated function groups
        core::Vector< core::String > fgFQNsDeactivated{common::SplitStr(deactivatedFgsValue, ";")};

        // Deactivate function groups
        for (core::String &fgFQN : fgFQNsDeactivated) {
            log_.LogInfo() << "StateManager::_deactivateFGsFromCalibrationData(), we get a fgFQN:" << fgFQN.c_str();
            // Find function group information
            core::Vector< ara::sm::config::FGInfo >::iterator const it{
                std::find_if(fgInfoList.begin(), fgInfoList.end(),
                             [&fgFQN](FGInfo const &fgInfo) noexcept { return fgInfo.fgFQN == fgFQN; })};
            if (fgInfoList.end() != it) {
                (*it).isDeactivated = true;
            }
        }
    }
#endif
}
}  // namespace config
}  // namespace sm
}  // namespace ara