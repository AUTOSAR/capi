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
/// @file       state_machine_config.cpp
/// @brief      Read configuration files related to state machines
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
/// @trace_id_sr=SR_SM_00107
/// @unit_name=StateMachineConfig
/// @unit_description=Read configuration files related to state machines
/// @endcode
///
/// ================================================================

#include "state_machine_config.h"

#include <ara/core/string.h>
#include <ara/exec/function_group_state.h>

#include "define.h"
#include "helper.h"

namespace ara {
namespace sm {
namespace config {
/// @brief Function group
using FunctionGroup = ara::exec::FunctionGroup;
/// @brief Function group state
using FunctionGroupState = ara::exec::FunctionGroupState;

/// @brief Constructor function
StateMachineConfig::StateMachineConfig() noexcept
    : log_{log::CreateLogger((core::StringView{"#CFG"}), (core::StringView{"Config context"}))}
    , smConfigInfoList_{}
    , smServiceInfoList_{}
    , fg2StateMachineMap_{}
{
    log_.LogInfo() << "StateMachineConfig::StateMachineConfig()";
}

/// @brief Destructor function
StateMachineConfig::~StateMachineConfig() noexcept { log_.LogInfo() << "StateMachineConfig::~StateMachineConfig()"; }

/// @brief Get the list of all state machine service information
/// @return List of state machine service information
core::Vector< SMServiceInfo > const &StateMachineConfig::GetAllSMServiceInfoList() const noexcept
{
    return smServiceInfoList_;
}

/// @brief Get the list of all state machine configuration information
/// @return List of state machine configuration information
core::Vector< SMConfigInfo > const &StateMachineConfig::GetAllSMConfigInfoList() const noexcept
{
    return smConfigInfoList_;
}

/// @brief Load all state machine' info list
/// @param manifestPath config file path
/// @return true - success
/// @return false - failure
bool StateMachineConfig::Load(core::String const &manifestPath) noexcept
{
    log_.LogInfo() << "StateMachineConfig::Load(), got stateMachineManifestPath:" << manifestPath.c_str();
    smConfigInfoList_.clear();
    smServiceInfoList_.clear();
    fg2StateMachineMap_.clear();
    bool ret{true};
    core::Result< std::unique_ptr< isoft::manifestreader::Manifest > > smManifestRes{
        isoft::manifestreader::OpenManifest((core::StringView{manifestPath.c_str()}))};
    if (smManifestRes) {
        std::unique_ptr< isoft::manifestreader::Manifest > const smManifest{std::move(smManifestRes).Value()};
        // Load notifications configuration information
        if (ret) {
            ret = _iterateNotifications(smManifest);
        }

        // Load actionItemLists configuration information
        if (ret) {
            ret = _iterateActionItemLists(smManifest);
        }

        // Load requests configuration information
        if (ret) {
            ret = _iterateRequests(smManifest);
        }
    } else {
        log_.LogWarn() << "StateMachineConfig::Load(), OpenManifest error, stateMachineManifestPath:"
                       << manifestPath.c_str();
    }
    return ret;
}

/// @brief Parse state machine services and state machine states
/// @param stateMachineManifest State machine configuration node
/// @return true Parsing successful
/// @return false Parsing failed
bool StateMachineConfig::_iterateNotifications(
    std::unique_ptr< isoft::manifestreader::Manifest > const &stateMachineManifest) noexcept
{
    log_.LogInfo() << "StateMachineConfig::_iterateNotifications()";
    bool const ret{true};

    std::ignore = stateMachineManifest->IterateArray(
        (core::StringView{common::GetkNotifications()}),
        [this](std::size_t, isoft::manifestreader::ManifestNode const &notificationInfoNode) {
            // notificationPort
            core::String const notificationPort{notificationInfoNode.GetValue< core::String >(
                (core::StringView{common::GetkNotificationPort()}), core::String{""})};
            log_.LogDebug() << "StateMachineConfig::_iterateNotifications(), got a notificationPort:"
                            << notificationPort.c_str();

            // stateMachineService_instance_id information
            core::String const smServiceInstanceID{notificationInfoNode.GetValue< core::String >(
                (core::StringView{common::GetkStateMachineServiceInstanceID()}), core::String{""})};
            log_.LogDebug() << "StateMachineConfig::_iterateNotifications(), got a smServiceInstanceID:"
                            << smServiceInstanceID.c_str();

            // State machine FQN
            core::String smFQN;
            std::ignore = notificationInfoNode.Load(
                (core::StringView{core::String(common::GetkStateMachine()) + "." + core::String(common::GetkFQN())}),
                smFQN);
            if (smFQN.empty()) {
                log_.LogWarn()
                    << "StateMachineConfig::_iterateNotifications(), fails to get smFQN, skip this notification";
                return;
            }
            log_.LogDebug() << "StateMachineConfig::_iterateNotifications(), got a smFQN:" << smFQN.c_str();

            // Whether state machine service configuration already exists
            std::vector< ara::sm::config::SMServiceInfo >::iterator const serviceInfoIt{
                std::find_if(smServiceInfoList_.begin(), smServiceInfoList_.end(),
                             [&smFQN](SMServiceInfo const &info) noexcept { return smFQN == info.smFQN; })};
            if (smServiceInfoList_.end() != serviceInfoIt) {
                log_.LogWarn()
                    << "StateMachineConfig::_iterateNotifications(), there is already a SMServiceInfo for smFQN:"
                    << smFQN.c_str();
                return;
            }

            // Set of state machine states
            std::vector< core::String > smStateVec;
            std::ignore = notificationInfoNode.Load(
                (core::StringView{core::String(common::GetkStateMachine()) + "." + core::String(common::GetkStates())}),
                smStateVec);
            log_.LogDebug() << "StateMachineConfig::_iterateNotifications(), got a smStateVec:"
                            << common::ConcatenateStrings(smStateVec).c_str();

            // Check the validity of state machine state configuration
            if (smStateVec.end() == std::find(smStateVec.begin(), smStateVec.end(), common::GetkInitialState())) {
                log_.LogWarn() << "StateMachineConfig::_iterateNotifications(), there is no"
                               << common::GetkInitialState() << "for smFQN:" << smFQN.c_str();
                return;
            }
            if (smStateVec.end() == std::find(smStateVec.begin(), smStateVec.end(), common::GetkFinalState())) {
                log_.LogWarn() << "StateMachineConfig::_iterateNotifications(), there is no" << common::GetkFinalState()
                               << "for smFQN:" << smFQN.c_str();
                return;
            }

            SMServiceInfo serviceInfo;
            serviceInfo.smFQN            = smFQN;
            serviceInfo.notificationPort = notificationPort;
            serviceInfo.instanceId       = smServiceInstanceID;
            log_.LogDebug() << "StateMachineConfig::_iterateNotifications(), update smServiceInfoList_, push back new "
                               "SMServiceInfo, smFQN:"
                            << smFQN.c_str() << "notificationPort:" << notificationPort.c_str()
                            << "smServiceInstanceID:" << smServiceInstanceID.c_str();
            smServiceInfoList_.push_back(std::move(serviceInfo));

            // _iterateNotifications must be called before _iterateActionItemLists and _iterateRequests, so smFQN in smConfigInfoList_
            // will definitely not be duplicated. If duplicated, the configuration must be wrong. Whether state machine state configuration already exists.
            std::vector< ara::sm::config::SMConfigInfo >::iterator const configInfoIt{
                std::find_if(smConfigInfoList_.begin(), smConfigInfoList_.end(),
                             [&smFQN](SMConfigInfo const &info) noexcept { return smFQN == info.smFQN; })};
            if (smConfigInfoList_.end() != configInfoIt) {
                log_.LogWarn()
                    << "StateMachineConfig::_iterateNotifications(), there is already a state2ActionList for smFQN:"
                    << smFQN.c_str();
                return;
            }
            SMConfigInfo configInfo;
            configInfo.smFQN = smFQN;
            for (core::String const &str : smStateVec) {
                configInfo.state2ActionList[str] = core::Vector< SMAcionItemInfo >();
            }
            std::ignore = notificationInfoNode.Load((core::StringView{core::String(common::GetkStateMachine()) + "."
                                                                      + core::String(common::GetkInitialSMState())}),
                                                    configInfo.initialStartState);
            log_.LogDebug() << "StateMachineConfig::_iterateNotifications(), update smConfigInfoList_, push back "
                               "new SMConfigInfo with initialStartState:"
                            << configInfo.initialStartState.c_str() << "for smFQN:" << smFQN.c_str();
            smConfigInfoList_.push_back(std::move(configInfo));
        });
    return ret;
}

/// @brief Parse the action list of state machine states
/// @param stateMachineManifest State machine configuration node
/// @return true Parsing successful
/// @return false Parsing failed
bool StateMachineConfig::_iterateActionItemLists(
    std::unique_ptr< isoft::manifestreader::Manifest > const &stateMachineManifest) noexcept
{
    log_.LogInfo() << "StateMachineConfig::_iterateActionItemLists()";
    bool const ret{true};

    std::ignore = stateMachineManifest->IterateArray(
        (core::StringView{common::GetkActionItemLists()}),
        [this](std::size_t, isoft::manifestreader::ManifestNode const &actionItemListNode) {
            // fqn of actionItemList
            core::String const actionItemListFQN{
                actionItemListNode.GetValue< core::String >((core::StringView{common::GetkFQN()}), core::String{""})};
            log_.LogDebug() << "StateMachineConfig::_iterateActionItemLists(), got a actionItemListFQN:"
                            << actionItemListFQN.c_str();

            // Corresponding state machine state
            core::String affectedSmFQN{actionItemListNode.GetValue< core::String >(
                (core::StringView{core::String(common::GetkAffectedState()) + "." + common::GetkStateMachineFQN()}),
                core::String{""})};
            log_.LogDebug() << "StateManager::_iterateActionItemLists(), got a affectedSmFQN:" << affectedSmFQN.c_str();

            core::String const affectedState{actionItemListNode.GetValue< core::String >(
                (core::StringView{core::String(common::GetkAffectedState()) + "." + common::GetkState()}),
                core::String{""})};
            log_.LogDebug() << "StateMachineConfig::_iterateActionItemLists(), got a affectedState:"
                            << affectedState.c_str();

            std::vector< ara::sm::config::SMConfigInfo >::iterator const configInfoIt{std::find_if(
                smConfigInfoList_.begin(), smConfigInfoList_.end(),
                [&affectedSmFQN](SMConfigInfo const &info) noexcept { return affectedSmFQN == info.smFQN; })};
            if (smConfigInfoList_.end() == configInfoIt) {
                log_.LogWarn() << "StateMachineConfig::_iterateActionItemLists(), invalid affectedSmFQN:"
                               << affectedSmFQN.c_str();
                return;
            }
            core::Map< core::String, core::Vector< ara::sm::config::SMAcionItemInfo > > state2ActionList{
                configInfoIt->state2ActionList};
            core::Map< core::String, core::Vector< ara::sm::config::SMAcionItemInfo > >::iterator const stateIt{
                state2ActionList.find(affectedState)};
            if (state2ActionList.end() == stateIt) {
                log_.LogWarn() << "StateMachineConfig::_iterateActionItemLists(), for affectedSmFQN:"
                               << affectedSmFQN.c_str() << "invalid affectedState:" << affectedState.c_str();
                return;
            }

            // Get the action list actionItems
            core::Vector< SMAcionItemInfo > actionItemInfos;
            std::ignore
                = actionItemListNode.IterateArray(
                    (core::StringView{common::GetkActionItems()}),
                    [&affectedSmFQN, &actionItemInfos, this](
                        std::size_t, isoft::manifestreader::ManifestNode const &actionItemNode) {
                        core::String const type{actionItemNode.GetValue< core::String >(
                            (core::StringView{common::GetkType()}), core::String{""})};
                        log_.LogDebug() << "StateMachineConfig::_iterateActionItemLists(), got a type:" << type.c_str();
                        if (0 == type.compare(common::GetkTypeSetFunctionGroupState())) {  // Set function group state
                            // Get function group FQN
                            core::String const fgFQN{actionItemNode.GetValue< core::String >(
                                (core::StringView{core::String(common::GetkSetFunctionGroupState()) + "."
                                                  + common::GetFunctionGroupFQN()}),
                                core::String{""})};
                            log_.LogDebug()
                                << "StateMachineConfig::_iterateActionItemLists(), got a fgFQN:" << fgFQN.c_str();

                            // Check if function group is valid
                            core::Result< FunctionGroup::CtorToken > const fgTokenResult{
                                FunctionGroup::Preconstruct(core::StringView{fgFQN})};
                            if (fgTokenResult.HasValue() == false) {
                                log_.LogWarn()
                                    << "StateMachineConfig::_iterateActionItemLists(), fails to Preconstruct fgFQN:"
                                    << fgFQN.c_str();
                                return;
                            }

                            // Check that a function group is configured in only one state machine
                            core::Map< core::String, core::String >::iterator const fgIt{
                                fg2StateMachineMap_.find(fgFQN)};
                            if (fg2StateMachineMap_.end() != fgIt) {
                                core::String const smFQN{fgIt->second};
                                if (0 != smFQN.compare(affectedSmFQN)) {
                                    log_.LogWarn()
                                        << "StateMachineConfig::_iterateActionItemLists(), fgFQN:" << fgFQN.c_str()
                                        << "can't exist both in" << affectedSmFQN.c_str() << "and" << smFQN.c_str();
                                    return;
                                }
                            }

                            // Function group state
                            core::String const fgState{actionItemNode.GetValue< core::String >(
                                (core::StringView{core::String(common::GetkSetFunctionGroupState()) + "."
                                                  + common::GetkState()}),
                                core::String{""})};
                            log_.LogDebug()
                                << "StateMachineConfig::_iterateActionItemLists(), got a fgState:" << fgState.c_str();

                            // Check if function group state is valid
                            FunctionGroup::CtorToken fgToken{fgTokenResult.Value()};
                            FunctionGroup const fg{std::move(fgToken)};
                            core::Result< ara::exec::FunctionGroupState::CtorToken > const stateTokenResult{
                                FunctionGroupState::Preconstruct(fg, (core::StringView{fgState}))};
                            if (stateTokenResult.HasValue() == false) {
                                log_.LogWarn()
                                    << "StateMachineConfig::_iterateActionItemLists(), fails to Preconstruct fgState:"
                                    << fgState.c_str() << "for fgFQN:" << fgFQN.c_str();
                                return;
                            }
                            SMAcionItemInfo actionItemInfo;
                            actionItemInfo.type     = type;
                            actionItemInfo.behavior = fgFQN + common::GetkActionItemBehaviorSeparator() + fgState;
                            log_.LogDebug()
                                << "StateMachineConfig::_iterateActionItemLists(), update actionItemInfos, push "
                                   "back actionItemInfo, type:"
                                << actionItemInfo.type.c_str() << "behavior:" << actionItemInfo.behavior.c_str()
                                << "for smFQN:" << affectedSmFQN.c_str();
                            actionItemInfos.push_back(std::move(actionItemInfo));
                            fg2StateMachineMap_[fgFQN] = affectedSmFQN;
                        } else if (0 == type.compare(common::GetkTypeSync())) {  // Sync
                            SMAcionItemInfo actionItemInfo;
                            actionItemInfo.type = type;
                            log_.LogDebug()
                                << "StateMachineConfig::_iterateActionItemLists(), update actionItemInfos, push "
                                   "back actionItemInfo, type:"
                                << actionItemInfo.type.c_str() << "behavior:" << actionItemInfo.behavior.c_str()
                                << "for smFQN:" << affectedSmFQN.c_str();
                            actionItemInfos.push_back(std::move(actionItemInfo));
                        } else if (0 == type.compare(common::GetkTypeCtrlStateMachine())) {
                            core::String startSmFQN{actionItemNode.GetValue< core::String >(
                                (core::StringView{core::String(common::GetkStartStateMachineFQN())}),
                                core::String{""})};
                            core::String initialSmState{actionItemNode.GetValue< core::String >(
                                (core::StringView{core::String(common::GetkInitialSMState())}), core::String{""})};
                            core::String stopSmFQN{actionItemNode.GetValue< core::String >(
                                (core::StringView{core::String(common::GetkStopStateMachineFQN())}), core::String{""})};
                            if (startSmFQN.empty() ^ stopSmFQN.empty()) {
                                SMAcionItemInfo actionItemInfo;
                                if (!startSmFQN.empty()) {
                                    actionItemInfo.type = common::GetkTypeStartStateMachine();
                                    if (initialSmState.empty()) {
                                        actionItemInfo.behavior = std::move(startSmFQN);
                                    } else {
                                        actionItemInfo.behavior
                                            = startSmFQN + common::GetkActionItemBehaviorSeparator() + initialSmState;
                                    }
                                } else {
                                    actionItemInfo.type     = common::GetkTypeStopStateMachine();
                                    actionItemInfo.behavior = std::move(stopSmFQN);
                                }
                                log_.LogDebug()
                                    << "StateMachineConfig::_iterateActionItemLists(), update actionItemInfos, push "
                                       "back actionItemInfo, type:"
                                    << actionItemInfo.type.c_str() << "behavior:" << actionItemInfo.behavior.c_str()
                                    << "for smFQN:" << affectedSmFQN.c_str();
                                actionItemInfos.push_back(std::move(actionItemInfo));
                            }
                        } else if (0 == type.compare(common::GetkTypeSetNetworkState())) {
                            std::vector< core::String > netWorkHandles;
                            std::ignore = actionItemNode.Load(
                                core::StringView{core::String(common::GetkNetworkHandles())}, netWorkHandles);
                            log_.LogDebug() << "StateMachineConfig::_iterateActionItemLists(), got netWorkHandles:"
                                            << common::ConcatenateStrings(netWorkHandles).c_str();
                            core::String const netState{actionItemNode.GetValue< core::String >(
                                (core::StringView{core::String(common::GetkState())}), core::String{""})};
                            for (core::String const &handle : netWorkHandles) {
                                SMAcionItemInfo actionItemInfo;
                                actionItemInfo.type     = type;
                                actionItemInfo.behavior = handle + common::GetkActionItemBehaviorSeparator() + netState;
                                log_.LogDebug()
                                    << "StateMachineConfig::_iterateActionItemLists(), update actionItemInfos, push "
                                       "back actionItemInfo, type:"
                                    << actionItemInfo.type.c_str() << "behavior:" << actionItemInfo.behavior.c_str()
                                    << "for smFQN:" << affectedSmFQN.c_str();
                                actionItemInfos.push_back(std::move(actionItemInfo));
                            }
                        } else if (0 == type.compare(common::GetkTypeSleep())) {
                            SMAcionItemInfo actionItemInfo;
                            actionItemInfo.type = type;
                            _Float32 sleepTime{actionItemNode.GetValue((core::StringView{common::GetkSleepTime()}),
                                                                       common::GetkDefaultSleepTime())};
                            actionItemInfo.behavior = core::to_string(sleepTime);
                            log_.LogDebug()
                                << "StateMachineConfig::_iterateActionItemLists(), update actionItemInfos, push "
                                   "back actionItemInfo, type:"
                                << actionItemInfo.type.c_str() << "behavior:" << actionItemInfo.behavior.c_str()
                                << "for smFQN:" << affectedSmFQN.c_str();
                            actionItemInfos.push_back(std::move(actionItemInfo));
                        } else if (0 == type.compare(common::GetkTypeSleep())) {
                            SMAcionItemInfo actionItemInfo;
                            actionItemInfo.type = type;
                            _Float32 sleepTime{actionItemNode.GetValue((core::StringView{common::GetkSleepTime()}),
                                                                       common::GetkDefaultSleepTime())};
                            actionItemInfo.behavior = core::to_string(sleepTime);
                            log_.LogDebug()
                                << "StateMachineConfig::_iterateActionItemLists(), update actionItemInfos, push "
                                   "back actionItemInfo, type:"
                                << actionItemInfo.type.c_str() << "behavior:" << actionItemInfo.behavior.c_str()
                                << "for smFQN:" << affectedSmFQN.c_str();
                            actionItemInfos.push_back(std::move(actionItemInfo));
                        }
                    });
            // If the last element in ActionList is not SYNC, add a SYNC to its end;
            if (actionItemInfos.empty() == false) {
                if (actionItemInfos.back().type != common::GetkTypeSync()) {
                    SMAcionItemInfo actionItemInfo;
                    actionItemInfo.type = common::GetkTypeSync();
                    actionItemInfos.push_back(std::move(actionItemInfo));
                }
            }
            configInfoIt->state2ActionList[affectedState] = std::move(actionItemInfos);
            log_.LogDebug() << "StateMachineConfig::_iterateActionItemLists(), actionItemInfos.size():"
                            << configInfoIt->state2ActionList[affectedState].size()
                            << "affectedState:" << affectedState.c_str();
        });
    return ret;
}

/// @brief Parse the state transition table and error recovery table of state machine states
/// @param stateMachineManifest State machine configuration node
/// @return true Parsing successful
/// @return false Parsing failed
bool StateMachineConfig::_iterateRequests(
    std::unique_ptr< isoft::manifestreader::Manifest > const &stateMachineManifest) noexcept
{
    log_.LogInfo() << "StateMachineConfig::_iterateRequests()";
    bool const ret{true};

    std::ignore = stateMachineManifest->IterateArray(
        (core::StringView{common::GetkRequests()}),
        [this](std::size_t, isoft::manifestreader::ManifestNode const &requestNode) {
            // fqn of request
            core::String const requestFQN{
                requestNode.GetValue< core::String >((core::StringView{common::GetkFQN()}), core::String{""})};
            log_.LogDebug() << "StateMachineConfig::_iterateRequests(), got a requestFQN:" << requestFQN.c_str();

            // type of request
            core::String requestType{
                requestNode.GetValue< core::String >((core::StringView{common::GetkType()}), core::String{""})};
            log_.LogDebug() << "StateMachineConfig::_iterateRequests(), got a requestType:" << requestType.c_str();

            // Parse rule
            std::ignore = requestNode.IterateArray(
                (core::StringView{common::GetkRules()}),
                [&requestType, this](std::size_t, isoft::manifestreader::ManifestNode const &ruleNode) {
                    // contextStateMachine
                    core::String contextStateMachine{ruleNode.GetValue< core::String >(
                        (core::StringView{common::GetkContextStateMachine()}), core::String{""})};
                    log_.LogDebug() << "StateMachineConfig::_iterateRequests(), got a contextStateMachine:"
                                    << contextStateMachine.c_str();

                    // Check if contextStateMachine exists
                    std::vector< ara::sm::config::SMConfigInfo >::iterator const configInfoIt{
                        std::find_if(smConfigInfoList_.begin(), smConfigInfoList_.end(),
                                     [&contextStateMachine](SMConfigInfo const &info) noexcept {
                                         return contextStateMachine == info.smFQN;
                                     })};
                    if (smConfigInfoList_.end() == configInfoIt) {
                        log_.LogWarn() << "StateMachineConfig::_iterateRequests(), invalid contextStateMachine:"
                                       << contextStateMachine.c_str();
                        return;
                    }

                    // nextState and check validity
                    core::String const nextState{ruleNode.GetValue< core::String >(
                        (core::StringView{common::GetkNextState()}), core::String{""})};
                    log_.LogDebug() << "StateMachineConfig::_iterateRequests(), got a nextState:" << nextState.c_str();
                    core::Map< core::String, core::Vector< ara::sm::config::SMAcionItemInfo > > state2ActionList{
                        configInfoIt->state2ActionList};
                    core::Map< core::String, core::Vector< ara::sm::config::SMAcionItemInfo > >::iterator const nextIt{
                        state2ActionList.find(nextState)};
                    if (state2ActionList.end() == nextIt) {
                        log_.LogWarn() << "StateMachineConfig::_iterateActionItemLists(), for contextStateMachine:"
                                       << contextStateMachine.c_str() << "invalid nextState:" << nextState.c_str();
                        return;
                    }

                    // compareValue
                    core::String compareValue{ruleNode.GetValue< core::String >(
                        (core::StringView{common::GetkCompareValue()}), core::String{""})};
                    compareValue = common::ToUpper(compareValue);
                    log_.LogDebug() << "StateMachineConfig::_iterateRequests(), got a compareValue:"
                                    << compareValue.c_str();

                    // Update state transition table or error recovery table
                    if (0 == requestType.compare(common::GetkTypeRequestTrigger())) {
                        // assumedCurrentState and check validity
                        core::String const assumedCurrentState{ruleNode.GetValue< core::String >(
                            (core::StringView{common::GetkAssumedCurrentState()}), core::String{""})};
                        log_.LogDebug() << "StateMachineConfig::_iterateRequests(), got a assumedCurrentState:"
                                        << assumedCurrentState.c_str();
                        core::Map< core::String, core::Vector< ara::sm::config::SMAcionItemInfo > >::iterator const
                            currentIt{state2ActionList.find(assumedCurrentState)};
                        if (state2ActionList.end() == currentIt) {
                            log_.LogWarn() << "StateMachineConfig::_iterateActionItemLists(), for contextStateMachine:"
                                           << contextStateMachine.c_str()
                                           << "invalid assumedCurrentState:" << assumedCurrentState.c_str();
                            return;
                        }
                        // State transition table, transition to transitionRequest
                        TransitionRequestType transitionRequest{};
                        if (common::String2Number(compareValue, transitionRequest)) {
                            core::Map< ara::sm::TransitionRequestType, core::Map< core::String, core::String > >
                                &transitionRequestTable{configInfoIt->transitionRequestTable};
                            core::Map< ara::sm::TransitionRequestType,
                                       core::Map< core::String, core::String > >::iterator const
                                transitionRequestTableIt{transitionRequestTable.find(transitionRequest)};
                            if (transitionRequestTable.end() == transitionRequestTableIt) {
                                core::Map< core::String, core::String > current2NextState;
                                current2NextState[assumedCurrentState] = nextState;
                                log_.LogDebug()
                                    << "StateMachineConfig::_iterateRequests(), update transitionRequestTable, insert "
                                       "new transitionRequest:"
                                    << transitionRequest << "for contextStateMachine:" << contextStateMachine.c_str()
                                    << "current2NextState:" << common::ConcatenateStrings(current2NextState).c_str();
                                transitionRequestTable[transitionRequest] = std::move(current2NextState);
                            } else {
                                core::Map< core::String, core::String > &current2NextState{
                                    transitionRequestTableIt->second};
                                core::Map< core::String, core::String >::iterator const currentStateIt{
                                    current2NextState.find(assumedCurrentState)};
                                if (current2NextState.end() == currentStateIt) {
                                    current2NextState[assumedCurrentState] = nextState;
                                    log_.LogDebug() << "StateMachineConfig::_iterateRequests(), update "
                                                       "transitionRequestTable, update existed transitionRequest:"
                                                    << transitionRequest
                                                    << "for contextStateMachine:" << contextStateMachine.c_str()
                                                    << "assumedCurrentState:" << assumedCurrentState.c_str()
                                                    << "nextState:" << nextState.c_str();
                                } else {
                                    log_.LogWarn()
                                        << "StateMachineConfig::_iterateRequests(), duplicated assumedCurrentState:"
                                        << assumedCurrentState.c_str() << "for transitionRequest:" << transitionRequest
                                        << "in contextStateMachine:" << contextStateMachine.c_str();
                                }
                            }
                        } else {
                            log_.LogDebug()
                                << "StateMachineConfig::_iterateRequests(), compareValue is not number, so "
                                   "try to create extendTransitionRequestTable for interaction with nm, compareValue:"
                                << compareValue.c_str();
                            if ((0 == strcmp(compareValue.c_str(), common::GetkNoComStr()))
                                || (0 == strcmp(compareValue.c_str(), common::GetkFullComStr()))) {
                            } else {
                                log_.LogWarn() << "StateMachineConfig::_iterateRequests(), invalid compareValue:"
                                               << compareValue.c_str();
                                return;
                            }

                            std::vector< core::String > netWorkHandles;
                            std::ignore = ruleNode.Load(core::StringView{core::String(common::GetkNetworkHandles())},
                                                        netWorkHandles);
                            log_.LogDebug() << "StateMachineConfig::_iterateRequests(), got netWorkHandles:"
                                            << common::ConcatenateStrings(netWorkHandles).c_str();
                            for (core::String const &handle : netWorkHandles) {
                                core::String const nmRequest{handle + common::GetkActionItemBehaviorSeparator()
                                                             + compareValue};
                                core::Map< core::String, core::Map< core::String, core::String > >
                                    &extendTransitionRequestTable{configInfoIt->extendTransitionRequestTable};
                                core::Map< core::String, core::Map< core::String, core::String > >::iterator const
                                    extendTransitionRequestTableIt{extendTransitionRequestTable.find(nmRequest)};
                                if (extendTransitionRequestTable.end() == extendTransitionRequestTableIt) {
                                    core::Map< core::String, core::String > current2NextState;
                                    current2NextState[assumedCurrentState] = nextState;
                                    log_.LogDebug()
                                        << "StateMachineConfig::_iterateRequests(), update "
                                           "extendTransitionRequestTable, insert "
                                           "new nmRequest:"
                                        << nmRequest << "for contextStateMachine:" << contextStateMachine.c_str()
                                        << "current2NextState:"
                                        << common::ConcatenateStrings(current2NextState).c_str();
                                    extendTransitionRequestTable[nmRequest] = std::move(current2NextState);
                                } else {
                                    core::Map< core::String, core::String > &current2NextState{
                                        extendTransitionRequestTableIt->second};
                                    core::Map< core::String, core::String >::iterator const currentStateIt{
                                        current2NextState.find(assumedCurrentState)};
                                    if (current2NextState.end() == currentStateIt) {
                                        current2NextState[assumedCurrentState] = nextState;
                                        log_.LogDebug()
                                            << "StateMachineConfig::_iterateRequests(), update "
                                               "extendTransitionRequestTable, update existed nmRequest:"
                                            << nmRequest << "for contextStateMachine:" << contextStateMachine.c_str()
                                            << "assumedCurrentState:" << assumedCurrentState.c_str()
                                            << "nextState:" << nextState.c_str();
                                    } else {
                                        log_.LogWarn()
                                            << "StateMachineConfig::_iterateRequests(), duplicated assumedCurrentState:"
                                            << assumedCurrentState.c_str() << "for nmRequest:" << nmRequest
                                            << "in contextStateMachine:" << contextStateMachine.c_str();
                                    }
                                }
                            }
                        }
                    } else if (0 == requestType.compare(common::GetkTypeRequestError())) {  // Rule of type RequestError
                        if (0 == compareValue.compare(common::GetkANY())) {                 // Whether it is ANY
                            core::String &anyErrRecoveryState{configInfoIt->anyErrRecoveryState};
                            if (!anyErrRecoveryState.empty()) {
                                log_.LogWarn() << "StateMachineConfig::_iterateRequests(), anyErrRecoveryState now is"
                                               << anyErrRecoveryState.c_str()
                                               << "so skip to set anyErrRecoveryState to be" << nextState.c_str();
                                return;
                            }
                            log_.LogDebug()
                                << "StateMachineConfig::_iterateRequests(), update anyErrRecoveryState:"
                                << nextState.c_str() << "for contextStateMachine:" << contextStateMachine.c_str();
                            anyErrRecoveryState = nextState;
                        } else {
                            // Error recovery table, transition to executionErr
                            ara::exec::ExecutionError executionErr{};
                            if (common::String2Number(compareValue, executionErr)) {
                                core::Map< ara::exec::ExecutionError, core::String > &errorRecoveryTable{
                                    configInfoIt->errorRecoveryTable};
                                core::Map< ara::exec::ExecutionError, core::String >::iterator const
                                    errorRecoveryTableIt{errorRecoveryTable.find(executionErr)};
                                if (errorRecoveryTable.end() == errorRecoveryTableIt) {
                                    errorRecoveryTable[executionErr] = nextState;
                                    log_.LogDebug() << "StateMachineConfig::_iterateRequests(), update "
                                                       "errorRecoveryTable, insert new executionErr:"
                                                    << executionErr << "nextState:" << nextState.c_str()
                                                    << "for contextStateMachine:" << contextStateMachine.c_str();
                                } else {
                                    log_.LogWarn()
                                        << "StateMachineConfig::_iterateRequests(), duplicated executionErr:"
                                        << executionErr << "in contextStateMachine:" << contextStateMachine.c_str();
                                    return;
                                }
                            }
                        }
                    } else {
                    }
                });
        });
    return ret;
}

}  // namespace config
}  // namespace sm
}  // namespace ara