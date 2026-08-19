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
/// @file       nm_config.cpp
/// @brief      Read configuration files related to network services
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
/// @trace_id_sr=SR_SM_05001, SR_SM_00104, SR_SM_00105, SR_SM_05005
/// @unit_name=NMConfig
/// @unit_description=Read configuration files related to network services
/// @endcode
///
/// ================================================================

#include "nm_config.h"

#include <ara/exec/function_group_state.h>
#include <isoft/manifestreader/manifest_reader.h>

#include "define.h"
#include "helper.h"

namespace ara {
namespace sm {
namespace config {
/// @brief Define alias to resolve qac2428 item: Direct use of character type.
/// @param Char8_t alias of char
/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
using Char8_t = char;

/// @brief Function group
/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
using FunctionGroup = ara::exec::FunctionGroup;
/// @brief Function group state
/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
using FunctionGroupState = ara::exec::FunctionGroupState;

/// @brief Constructor function
NMConfig::NMConfig() noexcept
    : log_{log::CreateLogger((core::StringView{"#CFG"}), (core::StringView{"Config context"}))}
    , fgState2FullComNmHandles_{}
    , offFgState2NoComNmHandles_{}
    , fullComNmHandle2FGStatesMap_{}
    , noComNmHandle2FGStatesMap_{}
    , nmAfterRunInfo_{}
    , nmServiceInfoList_{}
{
    log_.LogInfo() << "NMConfig::NMConfig()";
}

/// @brief Destructor function
NMConfig::~NMConfig() noexcept { log_.LogInfo() << "NMConfig::~NMConfig()"; }

/// @brief Get the list of all network service information
/// @return List of network service information
core::Vector< NMServiceInfo > const &NMConfig::GetAllNMServiceInfoList() const noexcept { return nmServiceInfoList_; }

/// @brief Get the mapping of function group states to FullCom network list.
/// When a function group is in these states, the corresponding network should be set to FullCom.
/// @return core::Map<Function group FQN, core::Map<Function group state, core::Vector<Network>>>
core::Map< core::String, core::Map< core::String, core::Vector< core::String > > > const &
NMConfig::GetFGState2NMHandlesMap() const noexcept
{
    return fgState2FullComNmHandles_;
}

/// @brief Get the mapping of Off function group to NoCom network list
/// @return core::Map<Function group FQN, core::Vector<Network>>
core::Map< core::String, core::Vector< core::String > > const &NMConfig::GetOffFG2NoComNMHandlesMap() const noexcept
{
    return offFgState2NoComNmHandles_;
}

/// @brief Get the mapping of FullCom network to function group state list.
/// When the network is in FullCom, if the current state of the function group is not one of the state list elements in the Map, switch the function group state to the first one in the state list in the Map.
/// @return core::Map<Network, core::Map<Function group FQN, core::Vector<Function group state>>>
core::Map< core::String, core::Map< core::String, core::String > > const &NMConfig::GetFullCommNMHandle2FGStatesMap()
    const noexcept
{
    return fullComNmHandle2FGStatesMap_;
}

/// @brief Get the mapping of NoCom network to function group state list
/// @return core::Map<Network, core::Map<core::String, core::Vector<core::String>>>
core::Map< core::String, core::Map< core::String, core::String > > const &NMConfig::GetNoComNMHandle2FGStatesMap()
    const noexcept
{
    return noComNmHandle2FGStatesMap_;
}

/// @brief Get the delay information related to NM
/// @return Delay time information
NMAfterRunTimeInfo const &NMConfig::GetNMAfterRunInfo() const noexcept { return nmAfterRunInfo_; }

/// @brief Load all networks' info list
/// @param manifestPath config file path
/// @return true - success
/// @return false - failure
bool NMConfig::Load(core::String const &manifestPath) noexcept
{
    log_.LogInfo() << "NMConfig::Load() begin, got manifestPath:" << manifestPath.c_str();
#ifdef ARA_WITH_NM
    fgState2FullComNmHandles_.clear();
    offFgState2NoComNmHandles_.clear();
    fullComNmHandle2FGStatesMap_.clear();
    noComNmHandle2FGStatesMap_.clear();
    nmServiceInfoList_.clear();
    core::Result< std::unique_ptr< isoft::manifestreader::Manifest > > nmManifestRes{
        isoft::manifestreader::OpenManifest((core::StringView{manifestPath}))};
    if (nmManifestRes.CheckError(isoft::manifestreader::ManifestReaderErrc::kOpenFile)) {  // If the file cannot be found, ignore it
        log_.LogWarn() << "NMConfig::Load(), can't OpenManifest nmManifestPath:" << manifestPath.c_str()
                       << "so just skip it";
        return true;
    }
    std::unique_ptr< isoft::manifestreader::Manifest > const nmManifest{std::move(nmManifestRes).Value()};

    core::StringView kNMHandleInstances{common::GetkNMHandleInstances()};
    int32_t const res1{nmManifest->IterateArray(
        kNMHandleInstances, [this](std::size_t, isoft::manifestreader::ManifestNode const &mapNode) {
            // Get nmhandle
            core::StringView kNMHandle{common::GetkNMHandle()};
            core::String const nmHandleName{mapNode.GetValue< core::String >(kNMHandle, core::String{""})};
            log_.LogDebug() << "NMConfig::Load(), got nmHandleName:" << nmHandleName.c_str();
            if (nmHandleName.empty()) {  // Validity check
                log_.LogWarn() << "NMConfig::Load(), nmHandleName:" << nmHandleName.c_str() << "is illegal.";
                return;
            }

            core::Vector< NMServiceInfo >::iterator const it{std::find_if(
                nmServiceInfoList_.begin(), nmServiceInfoList_.end(),
                [&nmHandleName](NMServiceInfo const &info) noexcept { return info.nmNetworkHandle == nmHandleName; })};
            if (it != nmServiceInfoList_.end()) {
                log_.LogWarn() << "NMConfig::Load(), duplicated nmHandle:" << nmHandleName.c_str();
                return;
            }

    // Get requiredInstanceId
    #ifdef ARA_ENABLE_NM_USING_COM
            core::StringView kRequiredNMInstanceID{common::GetkRequiredNMInstanceID()};
    #else
            core::StringView kRequiredNMInstanceID{common::GetkRequiredPortForIpc()};
    #endif
            core::String const requiredInstanceId{
                mapNode.GetValue< core::String >(kRequiredNMInstanceID, core::String{""})};
            log_.LogDebug() << "NMConfig::Load(), got requiredInstanceId:" << requiredInstanceId.c_str();
            if (requiredInstanceId.empty()) {  // Validity check
                log_.LogWarn() << "NMConfig::Load(), requiredInstanceId:" << requiredInstanceId.c_str()
                               << "is illegal.";
                return;
            }

            // Update network service information
            log_.LogDebug() << "NMConfig::Load(), upadte nmServiceInfoList_, emplace new NMServiceInfo:"
                            << nmHandleName.c_str() << "," << requiredInstanceId.c_str();
            NMServiceInfo nmServiceInfo{nmHandleName, requiredInstanceId};
            nmServiceInfoList_.push_back(std::move(nmServiceInfo));
        })};
    std::ignore = res1;

    core::StringView kNMHandleToFunctionGroupStateMapping{common::GetkNMHandleToFunctionGroupStateMapping()};
    int32_t const res2{nmManifest->IterateArray(
        kNMHandleToFunctionGroupStateMapping, [this](std::size_t, isoft::manifestreader::ManifestNode const &mapNode) {
            // Get nmhandle
            core::StringView kNMHandle{common::GetkNMHandle()};
            core::String const nmHandleName{mapNode.GetValue< core::String >(kNMHandle, core::String{""})};
            log_.LogDebug() << "NMConfig::Load(), got nmHandleName:" << nmHandleName.c_str();
            core::Vector< NMServiceInfo >::iterator const nmServiceInfoIt{std::find_if(
                nmServiceInfoList_.begin(), nmServiceInfoList_.end(),
                [&nmHandleName](NMServiceInfo const &info) noexcept { return info.nmNetworkHandle == nmHandleName; })};
            if (nmServiceInfoIt == nmServiceInfoList_.end()) {
                log_.LogWarn() << "NMConfig::Load(), invalid nmHandle:" << nmHandleName.c_str();
                return;
            }

            // Validity check
            core::StringView kMappingDirection{common::GetkMappingDirection()};
            core::String const mappingDirection{mapNode.GetValue< core::String >(kMappingDirection, core::String{""})};
            log_.LogDebug() << "NMConfig::Load(), got MappingDirection:" << mappingDirection.c_str();
            bool const equalsFunctionGroupStateToNmHandle{
                0 == mappingDirection.compare(common::GetkFunctionGroupStateToNmHandle())};
            bool const equalsNMHandleActiveToFunctionGroupState{
                0 == mappingDirection.compare(common::GetkNMHandleActiveToFunctionGroupState())};
            bool const equalsNMHandleInactiveToFunctionGroupState{
                0 == mappingDirection.compare(common::GetkNMHandleInactiveToFunctionGroupState())};
            bool const mappingDirectionValid{equalsFunctionGroupStateToNmHandle
                                             || equalsNMHandleActiveToFunctionGroupState
                                             || equalsNMHandleInactiveToFunctionGroupState};
            if (!mappingDirectionValid) {
                log_.LogWarn() << "NMConfig::Load(), invalid MappingDirection:" << mappingDirection.c_str();
                return;
            }

            if (0 == mappingDirection.compare(common::GetkNMHandleActiveToFunctionGroupState())) {
                core::Map< core::String, core::Map< core::String, core::String > >::iterator const
                    fullCommNmHandle2FGStatesIt{fullComNmHandle2FGStatesMap_.find(nmHandleName)};
                if (fullComNmHandle2FGStatesMap_.end() != fullCommNmHandle2FGStatesIt) {
                    log_.LogWarn() << "NMConfig::Load(), duplicated nmHandle:" << nmHandleName.c_str();
                    return;
                }
                core::StringView kFunctionGroupStates{common::GetkFunctionGroupStates()};
                int32_t const res3{mapNode.IterateArray(
                    kFunctionGroupStates,
                    [this, &nmHandleName](std::size_t, isoft::manifestreader::ManifestNode const &fgNode) {
                        // Get function group name
                        core::StringView kFunctionGroupName{common::GetkFunctionGroupName()};
                        core::StringView kState{common::GetkState()};
                        core::String const fgFQN{fgNode.GetValue< core::String >(kFunctionGroupName, core::String{""})};
                        core::String const fgState{fgNode.GetValue< core::String >(kState, core::String{""})};
                        log_.LogDebug() << "NMConfig::Load(), got fgFQN:" << fgFQN.c_str()
                                        << "and fgState:" << fgState.c_str();
                        bool const fgValid{_checkFGStateValid(fgFQN, fgState)};
                        if (!fgValid) {
                            log_.LogWarn() << "NMConfig::Load(), fg is invalid, fgFQN:" << fgFQN.c_str()
                                           << "fgState:" << fgState.c_str();
                            return;
                        }
                        fullComNmHandle2FGStatesMap_[nmHandleName][fgFQN] = fgState;
                    })};
                std::ignore = res3;
            } else if (0 == mappingDirection.compare(common::GetkNMHandleInactiveToFunctionGroupState())) {
                core::Map< core::String, core::Map< core::String, core::String > >::iterator const
                    noCommNmHandle2FGStatesIt{noComNmHandle2FGStatesMap_.find(nmHandleName)};
                if (noComNmHandle2FGStatesMap_.end() != noCommNmHandle2FGStatesIt) {
                    log_.LogWarn() << "NMConfig::Load(), duplicated nmHandle:" << nmHandleName.c_str();
                    return;
                }
                int32_t const res3{mapNode.IterateArray(
                    common::GetkFunctionGroupStates(),
                    [this, &nmHandleName](std::size_t, isoft::manifestreader::ManifestNode const &fgNode) {
                        // Get function group name
                        core::StringView kFunctionGroupName{common::GetkFunctionGroupName()};
                        core::StringView kState{common::GetkState()};
                        core::String const fgFQN{fgNode.GetValue< core::String >(kFunctionGroupName, core::String{""})};
                        core::String const fgState{fgNode.GetValue< core::String >(kState, core::String{""})};
                        log_.LogDebug() << "NMConfig::Load(), got fgFQN:" << fgFQN.c_str()
                                        << "and fgState:" << fgState.c_str();
                        bool const valid{_checkFGStateValid(fgFQN, fgState)};
                        if (!valid) {
                            log_.LogWarn() << "NMConfig::Load(), fg is invalid, fgFQN:" << fgFQN.c_str()
                                           << "fgState:" << fgState.c_str();
                            return;
                        }
                        noComNmHandle2FGStatesMap_[nmHandleName][fgFQN] = fgState;
                    })};
                std::ignore = res3;
            } else if (0 == mappingDirection.compare(common::GetkFunctionGroupStateToNmHandle())) {
                int32_t const res3{mapNode.IterateArray(
                    common::GetkFunctionGroupStates(),
                    [this, &nmHandleName](std::size_t, isoft::manifestreader::ManifestNode const &fgNode) {
                        // Get function group name
                        core::StringView kFunctionGroupName{common::GetkFunctionGroupName()};
                        core::StringView kState{common::GetkState()};
                        core::String const fgFQN{fgNode.GetValue< core::String >(kFunctionGroupName, core::String{""})};
                        core::String const fgState{fgNode.GetValue< core::String >(kState, core::String{""})};
                        log_.LogDebug() << "NMConfig::Load(), got fgFQN:" << fgFQN.c_str()
                                        << "and fgState:" << fgState.c_str();
                        bool const valid{_checkFGStateValid(fgFQN, fgState)};
                        if (!valid) {
                            log_.LogWarn() << "NMConfig::Load(), fg is invalid, fgFQN:" << fgFQN.c_str()
                                           << "fgState:" << fgState.c_str();
                            return;
                        }
                        // Update the mapping of function group states to [network name NmNetworkHandle] array. The format of the fgState2FullComNmHandles_ variable can be seen at its definition.
                        /// Find function group in fgState2NmHandlesMap_
                        core::Map< core::String,
                                   core::Map< core::String, core::Vector< core::String > > >::iterator const fgIt{
                            fgState2FullComNmHandles_.find(fgFQN)};
                        if (fgState2FullComNmHandles_.end() == fgIt) {  // This function group does not exist yet
                            core::Map< core::String, core::Vector< core::String > > state2NmHandles;
                            state2NmHandles[fgState]          = core::Vector< core::String >{nmHandleName};
                            fgState2FullComNmHandles_[fgFQN]  = std::move(state2NmHandles);
                            offFgState2NoComNmHandles_[fgFQN] = core::Vector< core::String >{nmHandleName};
                            log_.LogDebug() << "NMConfig::Load(), update offFgState2NoComNmHandles_, emplace new and "
                                               "push back the firt nmHandle:"
                                            << nmHandleName.c_str() << "for fgFQN:" << fgFQN.c_str();
                        } else {  // This function group already exists
                            core::Map< core::String, core::Vector< core::String > > &state2NmHandles{fgIt->second};
                            core::Map< core::String, core::Vector< core::String > >::iterator const stateIt{
                                state2NmHandles.find(fgState)};
                            if (state2NmHandles.end() == stateIt) {  // This function group state does not exist yet
                                state2NmHandles[fgState] = core::Vector< core::String >{nmHandleName};
                            } else {  // This function group state already exists
                                stateIt->second.push_back(nmHandleName);
                            }
                            log_.LogDebug() << "NMConfig::Load(), upadte fgState2FullComNmHandles_, update the existed "
                                               "and push back the nmHandle:"
                                            << nmHandleName.c_str() << "for fgFQN:" << fgFQN.c_str()
                                            << "and fgState:" << fgState.c_str();
                            core::Vector< core::String > const &nmHandles{offFgState2NoComNmHandles_[fgFQN]};
                            core::Vector< core::String >::const_iterator const nmHandleNameIt{
                                std::find(nmHandles.begin(), nmHandles.end(), nmHandleName)};
                            if (nmHandles.end() == nmHandleNameIt) {
                                offFgState2NoComNmHandles_[fgFQN].push_back(nmHandleName);
                                log_.LogDebug() << "NMConfig::Load(), upadte offFgState2NoComNmHandles_, update the "
                                                   "existed and push back the nmHandle:"
                                                << nmHandleName.c_str() << "for fgFQN:" << fgFQN.c_str();
                            }
                        }
                    })};
                std::ignore = res3;
            } else {
            }
        })};
    std::ignore = res2;
    core::String runInfoFgOffTime{common::GetkNMAfterRunInfo()};
    runInfoFgOffTime += ".";
    runInfoFgOffTime += common::GetkFgOffTime();
    nmAfterRunInfo_.fgOffTime
        = nmManifest->GetValue((core::StringView{runInfoFgOffTime}), common::GetkDefaultNMAfterRunTime());

    core::String runInfoNetworkOffTime{common::GetkNMAfterRunInfo()};
    runInfoNetworkOffTime += ".";
    runInfoNetworkOffTime += common::GetkNetworkOffTime();
    nmAfterRunInfo_.networkOffTime
        = nmManifest->GetValue((core::StringView{runInfoNetworkOffTime}), common::GetkDefaultNMAfterRunTime());
#else
    log_.LogWarn() << "NMConfig::Load(), ARA_WITH_NM is not defined";
#endif
    log_.LogDebug() << "NMConfig::Load() fullComNmHandle2FGStatesMap_:"
                    << common::ConcatenateStrings(fullComNmHandle2FGStatesMap_).c_str();
    log_.LogDebug() << "NMConfig::Load() noComNmHandle2FGStatesMap_:"
                    << common::ConcatenateStrings(noComNmHandle2FGStatesMap_).c_str();
    log_.LogDebug() << "NMConfig::Load() fgState2FullComNmHandles_:"
                    << common::ConcatenateStrings(fgState2FullComNmHandles_).c_str();
    log_.LogDebug() << "NMConfig::Load() offFgState2NoComNmHandles_:"
                    << common::ConcatenateStrings(offFgState2NoComNmHandles_).c_str();
    log_.LogDebug() << "NMConfig::Load() nmAfterRunInfo_, fgOffTime:" << nmAfterRunInfo_.fgOffTime
                    << "networkOffTime:" << nmAfterRunInfo_.networkOffTime;
    log_.LogDebug() << "NMConfig::Load() nmServiceInfoList_:" << _nMServiceInfo2Strings(nmServiceInfoList_).c_str();
    return true;
}

/// @brief Concatenate a String from a core::Vector<NMServiceInfo>
/// @param nmServiceInfoList The Vector<NMServiceInfo>
/// @returns The concatenated core::String
core::String NMConfig::_nMServiceInfo2Strings(core::Vector< NMServiceInfo > const &nmServiceInfoList) const noexcept
{
    // for qac: This non static member function does not access any member data.
    std::ignore = nmAfterRunInfo_;

    core::String strRet;
    size_t const seperatorSize{strlen(common::GetkSeperatorSpace())};
    for (const auto &nmServiceInfo : nmServiceInfoList) {  // PRQA S 2961
        std::ignore = strRet.append(nmServiceInfo.nmNetworkHandle);
        std::ignore = strRet.append(".");
        std::ignore = strRet.append(nmServiceInfo.nmNetworkInstanceID);
        std::ignore = strRet.append(common::GetkSeperatorSpace());
    }
    if (seperatorSize + 1U < strRet.size()) {
        std::ignore = strRet.erase(strRet.size() - seperatorSize);
    }
    return strRet;
}

/// @brief Check whether fgFQN and fgState are valid or not
/// @param fgFQN
/// @param fgState
/// @return true - fgFQN and fgState are valid
/// @return false - fgFQN and fgState are invalid
bool NMConfig::_checkFGStateValid(core::String const &fgFQN, core::String const &fgState) const noexcept
{
    // Create function group token
    core::Result< FunctionGroup::CtorToken > const fgTokenResult{
        FunctionGroup::Preconstruct((core::StringView{fgFQN.c_str()}))};

    // Whether the function group is valid
    if (fgTokenResult.HasValue() == false) {
        log_.LogWarn() << "NMConfig::_checkFGStateValid(), failed to call Preconstruct for fgFQN:" << fgFQN.c_str();
        return false;
    }
    FunctionGroup::CtorToken fgToken{fgTokenResult.Value()};
    FunctionGroup const group{std::move(fgToken)};

    // Function group state
    core::Result< FunctionGroupState::CtorToken > const stateTokenResult{
        FunctionGroupState::Preconstruct(group, (core::StringView{fgState.c_str()}))};

    // Whether the function group state is valid
    if (stateTokenResult.HasValue() == false) {
        log_.LogWarn() << "NMConfig::_checkFGStateValid(), failed to call Preconstruct for FGState:" << fgState.c_str();
        return false;
    }

    std::ignore = fgFQN;
    std::ignore = fgState;
    return true;
}

}  // namespace config
}  // namespace sm
}  // namespace ara