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
/// @file       state_machine.cpp
/// @brief      Define the state machine class
/// @details
/// @date       2024-06-26
/// @author     lianglongxiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/StateManagement/StateMachineManagement
/// @interface_level=uint
/// @trace_id_sr=SR_SM_00003, SR_SM_01001, SR_SM_01002, SR_SM_02001, SR_SM_02002, SR_SM_03002, SR_SM_04001, SR_SM_04002, SR_SM_04003, SR_SM_04004, SR_SM_04005,
/// SR_SM_04006, SR_SM_04007, SR_SM_04008,
/// @unit_name=StateMachine
/// @unit_description=State machine class
/// @endcode
///
/// ================================================================

#include "state_machine_management/state_machine.h"

#include <ara/core/string.h>
#include <ara/exec/exec_error_domain.h>
#include <ara/sm/error_domain_sm.h>
#include <bits/stdint-uintn.h>
#include <isoft/manifestreader/manifest_node.h>
#include <isoft/manifestreader/manifest_reader.h>
#include <isoft/naicpp/evloop.h>
#include <isoft/naicpp/global_evloop.h>

#include <memory>

#include "define.h"
#include "helper.h"
#include "state_machine_management/action_item_base.h"
#include "state_machine_management/action_item_set_fg_state.h"
#include "state_machine_management/action_item_set_network_state.h"
#include "state_machine_management/action_item_sleep.h"
#include "state_machine_management/action_item_start_sm.h"
#include "state_machine_management/action_item_stop_sm.h"
#include "state_machine_management/action_item_sync.h"

namespace ara {
namespace sm {
namespace state_machine_management {

using ara::sm::SMErrc;

/// @brief Default constructor
StateMachine::StateMachine() noexcept { log_.LogInfo() << "StateMachine::StateMachine()"; }

/// @brief Destructor
StateMachine::~StateMachine() noexcept { log_.LogInfo() << "StateMachine::~StateMachine(), smFQN_:" << smFQN_.c_str(); }

/// @brief Initialize the state machine according to the state machine configuration information
/// @param info State machine configuration information
/// @return true - success
/// @return false - failure
bool StateMachine::Init(sm::config::SMConfigInfo const &info) noexcept
{
    log_.LogInfo() << "StateMachine::Init(), smFQN_:" << info.smFQN.c_str()
                   << "initialStartState_:" << info.initialStartState.c_str();
    state2ActionListMap_.clear();
    states_.clear();
    smFQN_ = info.smFQN;
    ara::core::Map< ara::core::String, ara::core::Vector< ara::sm::config::SMAcionItemInfo > > const &state2ActionList{
        info.state2ActionList};
    std::ignore                   = _initActionItemList(state2ActionList);
    transitionRequestTable_       = info.transitionRequestTable;
    extendTransitionRequestTable_ = info.extendTransitionRequestTable;
    errorRecoveryTable_           = info.errorRecoveryTable;
    anyErrRecoveryState_          = info.anyErrRecoveryState;
    processContext_               = std::make_shared< ProcessActionListContext >();
    isUserDefined_                = true;
    initialStartState_            = info.initialStartState;
    return true;
}

/// @brief Find the action item that sets the function group state in the action list. If its function group state is related to the network state, add the corresponding FullCom action item before it.
/// @param fgFQN Function group name
/// @param fgState2FullComNmHandlesMap Mapping of function group states to FullCom network list
void StateMachine::ModifyActionListBasedOnFullComNMConfig(
    core::String const &fgFQN,
    core::Map< core::String, core::Vector< core::String > > const &fgState2FullComNmHandlesMap) noexcept
{
    log_.LogInfo() << "StateMachine::ModifyActionListBasedOnFullComNMConfig()";
    if (isUserDefined_) {
        fgState2FullComNmHandles_[fgFQN] = fgState2FullComNmHandlesMap;
    }
    // Traverse the action lists of all states
    for (std::pair< ara::core::String const, ara::sm::state_machine_management::ActionItemList > const &pair1 :
         state2ActionListMap_) {
        core::String const smState{pair1.first};
        ara::sm::state_machine_management::ActionItemList actionList{pair1.second};
        // For each action item, verify whether the action item content matches fgName and fgState
        for (size_t i{0U}; i < actionList.size(); i++) {
            core::String const actionStr{actionList[i]->ActionItemToString()};
            if (ara::core::String::npos != actionStr.find(fgFQN)) {
                for (std::pair< ara::core::String const, ara::sm::common::UcmFunctionGroupListInternal > const &pair2 :
                     fgState2FullComNmHandlesMap) {
                    core::String const &fgState{pair2.first};
                    if (ara::core::String::npos
                        != actionStr.find(fgFQN + common::GetkActionItemBehaviorSeparator() + fgState)) {
                        // After finding a match, insert the action item for setting the network state
                        for (ara::core::String const &handle : pair2.second) {
                            std::shared_ptr< ActionItemSetNetworkState > const actionSetNetworkState{
                                std::make_shared< ActionItemSetNetworkState >(
                                    handle, common::NetworkStateInternalType::kFullCom)};
                            _insertActionItem(smState, i, actionSetNetworkState);
                        }
                    }
                }
            }
        }
    }
}

/// @brief Find the action item that sets the function group state in the action list. If its function group state is related to the network state, add the corresponding NoCom action item after it.
/// @param fgFQN Function group name
/// @param noComNMHandles Networks that need to be switched to NoCom when the function group is in Off state
void StateMachine::ModifyActionListBasedOnNoComNMConfig(core::String const &fgFQN,
                                                        core::Vector< core::String > const &noComNMHandles) noexcept
{
    log_.LogInfo() << "StateMachine::ModifyActionListBasedOnNoComNMConfig()";
    if (isUserDefined_) {
        offFgState2NoComNmHandles_[fgFQN] = noComNMHandles;
    }
    for (std::pair< ara::core::String const, ara::sm::state_machine_management::ActionItemList > const &pair1 :
         state2ActionListMap_) {
        core::String const smState{pair1.first};
        ara::sm::state_machine_management::ActionItemList actionList{pair1.second};
        for (size_t i{0U}; i < actionList.size(); i++) {
            core::String const actionStr{actionList[i]->ActionItemToString()};
            if (ara::core::String::npos
                != actionStr.find(fgFQN + common::GetkActionItemBehaviorSeparator() + common::GetkOffState())) {
                // Since the network is delayed after turning off the function group, insert a delay action item
                std::shared_ptr< ActionItemSleep > const actionSleep{
                    std::make_shared< ActionItemSleep >(nmAfterRunTime_.networkOffTime * 1000.0F)};
                _insertActionItem(smState, i + 1U, actionSleep);
                // Insert the action item for turning off the network
                for (ara::core::String const &handle : noComNMHandles) {
                    std::shared_ptr< ActionItemSetNetworkState > const actionSetNetworkState{
                        std::make_shared< ActionItemSetNetworkState >(handle,
                                                                      common::NetworkStateInternalType::kNoCom)};
                    _insertActionItem(smState, i + TWO_UL, actionSetNetworkState);
                }
            }
        }
    }
}

/// @brief Insert an action list at the beginning of the state machine action list
/// @param state State machine state
/// @param actionItemList The action list to be inserted
/// @return true - success
/// @return false - failure
bool StateMachine::InsertActionListToBegin(
    core::String const &state, std::vector< std::shared_ptr< ActionItemBase > > const &actionItemList) noexcept
{
    log_.LogInfo() << "StateMachine::InsertActionListToBegin(), smFQN_:" << smFQN_.c_str()
                   << "smState:" << state.c_str();
    ara::core::String const keyState{state};
    ara::sm::state_machine_management::ActionItemList &originActionList{state2ActionListMap_[keyState]};
    log_.LogDebug() << "StateMachine::InsertActionListToBegin(), smFQN_:" << smFQN_.c_str()
                    << "smState:" << state.c_str() << "originActionList:" << ToString(originActionList).c_str();
    std::ignore = originActionList.insert(originActionList.cbegin(), actionItemList.begin(), actionItemList.end());
    return true;
}

/// @brief Insert an action list at the end of the state machine action list
/// @param state State machine state
/// @param actionItemList The action list to be inserted
/// @return true - success
/// @return false - failure
bool StateMachine::InsertActionListToEnd(
    core::String const &state, std::vector< std::shared_ptr< ActionItemBase > > const &actionItemList) noexcept
{
    log_.LogInfo() << "StateMachine::InsertActionListToEnd(), smFQN_:" << smFQN_.c_str() << "smState:" << state.c_str();
    ara::sm::state_machine_management::ActionItemList &originActionList{state2ActionListMap_[state]};
    std::ignore = originActionList.insert(originActionList.cend(), actionItemList.begin(), actionItemList.end());
    return true;
}

/// @brief Initialize the action list
/// @param state2ActionList Mapping of state machine states to action lists
/// @return true - success
/// @return false - failure
bool StateMachine::_initActionItemList(
    core::Map< core::String, core::Vector< config::SMAcionItemInfo > > const &state2ActionList) noexcept
{
    log_.LogInfo() << "StateMachine::_initActionItemList(), smFQN_:" << smFQN_.c_str();
    for (std::pair< ara::core::String const, ara::core::Vector< ara::sm::config::SMAcionItemInfo > > const &pair :
         state2ActionList) {
        states_.push_back(pair.first);
        for (ara::sm::config::SMAcionItemInfo const &smActionInfo : pair.second) {
            log_.LogDebug() << "StateMachine::_initActionItemList(), smActionInfo, type:" << smActionInfo.type.c_str()
                            << "behavior:" << smActionInfo.behavior.c_str();
            std::shared_ptr< ActionItemBase > action;
            if (common::GetkTypeSetFunctionGroupState() == smActionInfo.type) {
                std::size_t const separatorPos{smActionInfo.behavior.find(common::GetkActionItemBehaviorSeparator())};
                if (ara::core::String::npos == separatorPos) {
                    log_.LogWarn() << "StateMachine::_initActionItemList(), no separator, invalid behavior for type"
                                   << smActionInfo.type.c_str() << "behavior:" << smActionInfo.behavior.c_str();
                } else {
                    // Parse function group state information from behavior, determine whether it is a Controller state machine, and create a set function group state action item
                    core::String fgFQN{smActionInfo.behavior.substr(0UL, separatorPos)};
                    if (fg2State_[fgFQN].empty()) {
                        fg2State_[fgFQN] = common::GetkOffState();
                    }
                    if (!isController_) {
                        std::size_t const slashPos1{fgFQN.rfind("[")};
                        core::String const slashPart{fgFQN.substr(slashPos1 + 1U)};
                        std::size_t const slashPos2{slashPart.rfind("]")};
                        core::String fgName{slashPart.substr(0U, slashPos2)};
                        if (fgName.empty() || (std::string::npos == slashPos1) || (std::string::npos == slashPos2)) {
                            std::size_t const slashPos{fgFQN.rfind("/")};
                            fgName = fgFQN.substr(slashPos + 1U);
                        }
                        log_.LogDebug() << "StateMachine::InitActionItemList(), fgName:" << fgName.c_str();
                        if (fgName == common::GetkMachineFunctionGroupName()) {
                            isController_    = true;
                            fg2State_[fgFQN] = common::GetkStartupState();
                            log_.LogDebug()
                                << "StateMachine::_initActionItemList(), user defined, find Controller, smFQN_:"
                                << smFQN_.c_str() << "fgFQN:" << fgFQN.c_str()
                                << "curFgState:" << common::GetkStartupState();
                        }
                    }
                    core::String fgState{smActionInfo.behavior.substr(separatorPos + 1U)};
                    std::shared_ptr< ActionItemSetFGState > const fgAction{
                        std::make_shared< ActionItemSetFGState >(fgFQN, fgState)};
                    action = fgAction;
                }
            } else if (common::GetkTypeStartStateMachine() == smActionInfo.type) {
                std::size_t const separatorPos{smActionInfo.behavior.find(common::GetkActionItemBehaviorSeparator())};
                core::String smFQN{smActionInfo.behavior.substr(0UL, separatorPos)};
                core::String smState{};
                if (ara::core::String::npos != separatorPos) {
                    smState = smActionInfo.behavior.substr(separatorPos + 1U);
                }
                std::shared_ptr< ActionItemStartSM > const smAction{
                    std::make_shared< ActionItemStartSM >(smFQN, smState)};
                action = smAction;
            } else if (common::GetkTypeStopStateMachine() == smActionInfo.type) {
                std::shared_ptr< ActionItemStopSM > const smAction{
                    std::make_shared< ActionItemStopSM >(smActionInfo.behavior)};
                action = smAction;
            } else if (common::GetkTypeSync() == smActionInfo.type) {
                std::shared_ptr< ActionItemBase > const smAction{std::make_shared< ActionItemSync >()};
                action = smAction;
            } else if (common::GetkTypeSleep() == smActionInfo.type) {
                auto sleepTime = stof(smActionInfo.behavior);
                std::shared_ptr< ActionItemBase > const smAction{
                    std::make_shared< ActionItemSleep >(sleepTime * 1000.0F)};
                action = smAction;
            } else if (common::GetkTypeSetNetworkState() == smActionInfo.type) {
                std::size_t const separatorPos{smActionInfo.behavior.find(common::GetkActionItemBehaviorSeparator())};
                if (ara::core::String::npos == separatorPos) {
                    log_.LogWarn() << "StateMachine::_initActionItemList(), no separator, invalid behavior for type"
                                   << smActionInfo.type.c_str() << "behavior:" << smActionInfo.behavior.c_str();
                } else {
                    core::String const netHandle{smActionInfo.behavior.substr(0UL, separatorPos)};
                    core::String const netState{smActionInfo.behavior.substr(separatorPos + 1U)};
                    if (common::GetkFullComStr() == netState) {
                        action = std::make_shared< ActionItemSetNetworkState >(
                            netHandle, common::NetworkStateInternalType::kFullCom);
                    } else if (common::GetkNoComStr() == netState) {
                        action = std::make_shared< ActionItemSetNetworkState >(
                            netHandle, common::NetworkStateInternalType::kNoCom);
                    } else {
                        log_.LogWarn() << "StateMachine::_initActionItemList(), invalid netState for type"
                                       << smActionInfo.type.c_str() << "netState:" << netState.c_str();
                    }
                }
            }
            if (action) {
                log_.LogDebug() << "StateMachine::_initActionItemList(), insert action"
                                << action->ActionItemToString().c_str() << "for smState" << pair.first.c_str();
                state2ActionListMap_[pair.first].push_back(action);
            }
        }
    }
    return true;
}

/// @brief Set the Check Shutdown Handler
/// @param handler The handler which can be used to check whether the machine can be shutdown
void StateMachine::RegisterCheckShutdownHandler(
    std::function< core::Result< void >(core::StringView const, FGStateChangeSource const) > const &handler) noexcept
{
    log_.LogInfo() << "StateMachine::RegisterCheckShutdownHandler(), smFQN_:" << smFQN_.c_str();
    checkShutdownHandler_ = handler;
}

/// @brief Set the delay information related to network management
/// @param info Delay information related to network management
void StateMachine::SetNMAfterRunTime(config::NMAfterRunTimeInfo const &info) noexcept
{
    log_.LogInfo() << "StateMachine::SetNMAfterRunTime(), nmAfterRunTime_:" << info.fgOffTime << info.networkOffTime
                   << "smFQN_:" << smFQN_.c_str();
    nmAfterRunTime_ = info;
}

/// @brief Set the function handle for checking whether a function group is deactivated
/// @param handler
void StateMachine::SetCheckFGDeactivatedHandler(std::function< bool(core::String const &) > const &handler) noexcept
{
    log_.LogInfo() << "StateMachine::SetCheckFGDeactivatedHandler(), smFQN_:" << smFQN_.c_str();
    checkFGDeactivatedHandler_ = handler;
}

/// @brief Called by the action item that switches the function group to notify that the function group state has changed
/// @param fgFQN Function group full name
/// @param fgState Function group state
void StateMachine::NotifyFGStateChange(core::String const &fgFQN, core::String const &fgState) const noexcept
{
    log_.LogInfo() << "StateMachine::NotifyFGStateChange(), fgFQN:" << fgFQN.c_str() << "fgState:" << fgState.c_str()
                   << "smFQN_:" << smFQN_.c_str();
    if (appendEventHandler_) {
        core::Promise< void > notifierPromise;
        common::Event request;
        request.type           = common::EventType::kOutNotifyFGStateChange;
        request.data           = new common::FGStateInfo{fgFQN, fgState};
        request.requestPromise = std::move(notifierPromise);
        appendEventHandler_(std::move(request));
    } else {
        log_.LogError()
            << "StateMachine::NotifyFGStateChange(), fail to Notify, because appendEventHandler_ is nullptr";
    }
}

/// @brief Update the current state of the function group
/// @param fgFQN Function group full name
/// @param fgState Function group state
void StateMachine::UpdateFGState(core::String const &fgFQN, core::String const &fgState) noexcept
{
    log_.LogInfo() << "StateMachine::UpdateFGState(), fgFQN:" << fgFQN.c_str() << "fgState:" << fgState.c_str()
                   << "smFQN_:" << smFQN_.c_str();
    if (fgState != fg2State_[fgFQN]) {
        fg2State_[fgFQN] = fgState;
        NotifyFGStateChange(fgFQN, fgState);
    }
}

/// @brief Set the function group state and its associated network state
/// @param fgFQN Function group full name
/// @param fgState Function group state
/// @param processID Processing ID, used to filter whether to perform network switching
/// @param promise Used to asynchronously set the execution result
void StateMachine::ChangeFGAndRelatedNetworkStates(core::String const &fgFQN,
                                                   core::String const &fgState,
                                                   uint32_t const &processID,
                                                   std::shared_ptr< core::Promise< void > > const &promise,
                                                   uint64_t const &nmHandleId) noexcept
{
    log_.LogInfo() << "StateMachine::ChangeFGAndRelatedNetworkStates(), fgFQN:" << fgFQN.c_str()
                   << "fgState:" << fgState.c_str() << "processID:" << processID;
    if (common::GetkOffState() == fgState) {
        core::Promise< void > outerPromise;
        std::shared_ptr< core::Future< void > > outerFuture{
            std::make_shared< core::Future< void > >(outerPromise.get_future())};
        ChangeFGStateOnly(fgFQN, fgState, std::move(outerPromise));
        std::ignore = outerFuture->then([this, promise, outerFuture, fgFQN, fgState, processID, nmHandleId]() mutable {
            ara::core::Result< void > const outerRes{outerFuture->GetResult()};
            if (outerRes.HasValue()) {
                // After successfully switching the function group, start switching related networks to NoCom state
                UpdateFGState(fgFQN, fgState);
                core::Vector< core::String > const &nmList{offFgState2NoComNmHandles_[fgFQN]};
                core::Promise< void > innerPromise;
                std::shared_ptr< core::Future< void > > innerFuture{
                    std::make_shared< core::Future< void > >(innerPromise.get_future())};
                int32_t const delayTime{static_cast< int32_t >(nmAfterRunTime_.networkOffTime * 1000.0F)};
                log_.LogDebug() << "StateMachine::ChangeFGAndRelatedNetworkStates(), EM change fg state success, fgFQN:"
                                << fgFQN.c_str() << "fgState:" << fgState.c_str() << "delayTime:" << delayTime;
                if (0 < delayTime) {
                    std::shared_ptr< EvNodeTimer > innerTimer{nullptr};  /// Timer pointer
                    std::ignore = isoft::naicpp::GlobalGeneralEvLoop::Get()->MakeTimer(
                        innerTimer, delayTime,
                        [this, newNmList = nmList, nmState = common::NetworkStateInternalType::kNoCom,
                         promiseInTimer = std::make_shared< core::Promise< void > >(std::move(innerPromise)), fgFQN,
                         processID, nmHandleId]() mutable {
                            if (processID == fg2ProcessID_[fgFQN]) {
                                AsyncChangeNetworkStates(newNmList, nmState, std::move(*(promiseInTimer.get())),
                                                         nmHandleId);
                            } else {
                                log_.LogError() << "StateMachine::ChangeFGAndRelatedNetworkStates(), NM change network "
                                                   "states canceled";
                                promiseInTimer->SetError(SMErrc::kRejected);
                            }
                            std::ignore = fgProcessID2Timer_.erase(fgFQN + core::to_string(processID));
                        });
                    std::ignore = fgProcessID2Timer_.emplace(fgFQN + core::to_string(processID), std::move(innerTimer));
                } else {
                    AsyncChangeNetworkStates(nmList, common::NetworkStateInternalType::kNoCom, std::move(innerPromise),
                                             nmHandleId);
                }

                std::ignore = innerFuture->then(
                    [this, innerFuture, promise]() mutable {
                        ara::core::Result< void > const innerRes{innerFuture->GetResult()};
                        if (innerRes.HasValue()) {
                            log_.LogDebug()
                                << "StateMachine::ChangeFGAndRelatedNetworkStates(), NM change network states success";
                            promise->set_value();
                        } else {
                            log_.LogError()
                                << "StateMachine::ChangeFGAndRelatedNetworkStates(), NM change network states failed";
                            promise->SetError(innerRes.Error());
                        }
                        innerFuture = nullptr;
                    });
            } else {
                if (outerRes.CheckError(exec::ExecErrc::kAlreadyInState)) {
                    log_.LogWarn()
                        << "StateMachine::ChangeFGAndRelatedNetworkStates(), EM change fg state failed, fgFQN:"
                        << fgFQN.c_str() << "fgState:" << fgState.c_str();
                    promise->SetError(SMErrc::kAlreadyInState);
                } else {
                    log_.LogError()
                        << "StateMachine::ChangeFGAndRelatedNetworkStates(), EM change fg state failed, fgFQN:"
                        << fgFQN.c_str() << "fgState:" << fgState.c_str();
                    promise->SetError(SMErrc::kFailedToSetStateFromEM);
                }
            }
            outerFuture = nullptr;
        });
    } else {
        core::Promise< void > outerPromise;
        std::shared_ptr< core::Future< void > > outerFuture{
            std::make_shared< core::Future< void > >(outerPromise.get_future())};
        core::Vector< core::String > const &nmList{fgState2FullComNmHandles_[fgFQN][fgState]};
        ara::sm::common::NetworkStateInternalType const nmState{common::NetworkStateInternalType::kFullCom};
        AsyncChangeNetworkStates(nmList, nmState, std::move(outerPromise), nmHandleId);
        std::ignore = outerFuture->then([this, outerFuture, promise, fgFQN, fgState]() mutable {
            ara::core::Result< void > const outerRes{outerFuture->GetResult()};
            if (outerRes.HasValue()) {
                log_.LogDebug() << "StateMachine::ChangeFGAndRelatedNetworkStates(), NM change network states success";
                core::Promise< void > innerPromise;
                std::shared_ptr< core::Future< void > > innerFuture{
                    std::make_shared< core::Future< void > >(innerPromise.get_future())};
                ChangeFGStateOnly(fgFQN, fgState, std::move(innerPromise));
                std::ignore = innerFuture->then([this, promise, innerFuture, fgFQN, fgState]() mutable {
                    ara::core::Result< void > const innerRes{innerFuture->GetResult()};
                    if (innerRes.HasValue()) {
                        UpdateFGState(fgFQN, fgState);
                        log_.LogDebug()
                            << "StateMachine::ChangeFGAndRelatedNetworkStates(), EM change fg state success, fgFQN:"
                            << fgFQN.c_str() << "fgState:" << fgState.c_str();
                        promise->set_value();
                    } else {
                        if (innerRes.CheckError(exec::ExecErrc::kAlreadyInState)) {
                            log_.LogWarn()
                                << "StateMachine::ChangeFGAndRelatedNetworkStates(), EM change fg state failed, fgFQN:"
                                << fgFQN.c_str() << "fgState:" << fgState.c_str();
                            promise->SetError(SMErrc::kAlreadyInState);
                        } else {
                            log_.LogError()
                                << "StateMachine::ChangeFGAndRelatedNetworkStates(), EM change fg state failed, fgFQN:"
                                << fgFQN.c_str() << "fgState:" << fgState.c_str();
                            promise->SetError(SMErrc::kFailedToSetStateFromEM);
                        }
                    }
                    innerFuture = nullptr;
                });
            } else {
                log_.LogError() << "StateMachine::ChangeFGAndRelatedNetworkStates(), NM change network states failed";
                promise->SetError(outerRes.Error());
            }
            outerFuture = nullptr;
        });
    }
}

/// @brief Publish a function group switching request to EventManager
/// @param fgFQN Function group full name
/// @param fgState Function group state
/// @param promise Used to asynchronously set the execution result
void StateMachine::ChangeFGStateOnly(core::String const &fgFQN,
                                     core::String const &fgState,
                                     core::Promise< void > &&promise) noexcept
{
    core::Promise< core::Future< void > > futruePromise;
    std::shared_ptr< core::Future< core::Future< void > > > future{
        std::make_shared< core::Future< core::Future< void > > >(futruePromise.get_future())};
    log_.LogInfo() << "StateMachine::ChangeFGStateOnly(), fgFQN:" << fgFQN.c_str() << "fgState:" << fgState.c_str()
                   << "smFQN_:" << smFQN_.c_str();
    if (appendEventHandler_) {
        common::Event request;
        request.type                     = common::EventType::kOutEMSetFGState;
        request.data                     = new common::FGStateInfo{fgFQN, fgState};
        request.requestPromiseWithFuture = std::move(futruePromise);
        appendEventHandler_(std::move(request));
        std::ignore = future->then([this, future,
                                    outerPromise = std::make_shared< ara::core::Promise< void > >(std::move(promise)),
                                    fgFQN, fgState]() mutable {
            ara::core::Result< ara::core::Future< void >, ara::core::ErrorCode > res{future->GetResult()};
            if (res.HasValue()) {
                log_.LogDebug() << "StateMachine::ChangeFGStateOnly(), send cmd to EM success, fgFQN:" << fgFQN.c_str()
                                << "fgState:" << fgState.c_str();
                std::shared_ptr< ara::core::Future< void > > innerFuture{
                    std::make_shared< core::Future< void > >(std::move(res).Value())};
                std::ignore = innerFuture->then(
                    [this, innerFuture,
                     innerPromise = std::make_shared< ara::core::Promise< void > >(std::move(*outerPromise)), fgFQN,
                     fgState]() mutable {
                        ara::core::Result< void > const innerRes{innerFuture->GetResult()};
                        if (innerRes.HasValue()) {
                            log_.LogDebug() << "StateMachine::ChangeFGStateOnly(), EM change fg state success, fgFQN:"
                                            << fgFQN.c_str() << "fgState:" << fgState.c_str();
                            innerPromise->set_value();
                        } else {
                            if (innerRes.CheckError(exec::ExecErrc::kAlreadyInState)) {
                                log_.LogWarn() << "StateMachine::ChangeFGStateOnly(), EM change fg state failed, fgFQN:"
                                               << fgFQN.c_str() << "fgState:" << fgState.c_str();
                            } else {
                                log_.LogError()
                                    << "StateMachine::ChangeFGStateOnly(), EM change fg state failed, fgFQN:"
                                    << fgFQN.c_str() << "fgState:" << fgState.c_str();
                            }
                            innerPromise->SetError(innerRes.Error());
                        }
                        innerFuture = nullptr;
                    });
            } else {
                log_.LogError() << "StateMachine::ChangeFGStateOnly(), send cmd to EM failed, fgFQN:" << fgFQN.c_str()
                                << "fgState:" << fgState.c_str();
                outerPromise->SetError(res.Error());
            }
            future = nullptr;
        });
    } else {
        log_.LogError() << "StateMachine::ChangeFGStateOnly(), appendEventHandler_ is nullptr";
        promise.SetError(SMErrc::kRejected);
    }
}

/// @brief Publish a network state switching request to EventManager
/// @param nmHandle Network name
/// @param nmState Network state
/// @param promise Used to asynchronously set the execution result
void StateMachine::ChangeNetworkStateOnly(core::String const &nmHandle,
                                          common::NetworkStateInternalType const &nmState,
                                          core::Promise< void > &&promise,
                                          uint64_t const &nmHandleId) noexcept
{
    auto nmHandleSource{getNmHandlerNameFun_(nmHandleId)};
    log_.LogInfo() << "StateMachine::ChangeNetworkStateOnly(), nmHandle:" << nmHandle.c_str()
                   << "nmHandle:" << common::NetworkStateTypeToString(nmState).c_str() << "smFQN_:" << smFQN_.c_str()
                   << "nmHandleId:" << nmHandleId << "nmHandleSource:" << nmHandleSource;
    // Explanation: The cause of this network state switching is the active change of the network state of the same network, so there is no need to switch this network state again, directly set the promise
    if (nmHandle == nmHandleSource) {
        promise.set_value();
        log_.LogInfo() << "StateMachine::ChangeNetworkStateOnly(), skip set network, nmHandle:" << nmHandle
                       << "nmState:" << common::NetworkStateTypeToString(nmState);
    } else {
        core::Promise< core::Future< void > > futruePromise;
        std::shared_ptr< core::Future< core::Future< void > > > future{
            std::make_shared< core::Future< core::Future< void > > >(futruePromise.get_future())};
        if (appendEventHandler_) {
            common::Event request;
            request.type                     = common::EventType::kOutNMSetNetworkState;
            request.data                     = new common::NetworkStateInfo{nmHandle, nmState};
            request.requestPromiseWithFuture = std::move(futruePromise);
            appendEventHandler_(std::move(request));
            std::ignore = future->then([this, future,
                                        outerPromise
                                        = std::make_shared< ara::core::Promise< void > >(std::move(promise)),
                                        nmHandle, nmState]() mutable {
                ara::core::Result< ara::core::Future< void >, ara::core::ErrorCode > res{future->GetResult()};
                if (res.HasValue()) {
                    log_.LogDebug() << "StateMachine::ChangeNetworkStateOnly(), send cmd to NM success, nmHandle:"
                                    << nmHandle.c_str()
                                    << "nmState:" << common::NetworkStateTypeToString(nmState).c_str();
                    std::shared_ptr< ara::core::Future< void > > innerFuture{
                        std::make_shared< core::Future< void > >(std::move(res).Value())};
                    std::ignore = innerFuture->then([this, innerFuture,
                                                     innerPromise = std::make_shared< ara::core::Promise< void > >(
                                                         std::move(*outerPromise)),
                                                     nmHandle, nmState]() mutable {
                        ara::core::Result< void > const innerRes{innerFuture->GetResult()};
                        if (innerRes.HasValue()) {
                            log_.LogDebug()
                                << "StateMachine::ChangeNetworkStateOnly(), NM ChangeNetworkState success, nmHandle:"
                                << nmHandle.c_str() << "nmState:" << common::NetworkStateTypeToString(nmState).c_str();
                            innerPromise->set_value();
                        } else {
                            log_.LogError()
                                << "StateMachine::ChangeNetworkStateOnly(), NM ChangeNetworkState failed, nmHandle:"
                                << nmHandle.c_str() << "nmState:" << common::NetworkStateTypeToString(nmState).c_str()
                                << innerRes.Error().Message();
                            innerPromise->SetError(innerRes.Error());
                        }
                        innerFuture = nullptr;
                    });
                } else {
                    log_.LogError() << "StateMachine::ChangeNetworkStateOnly(), send cmd to NM failed, nmHandle:"
                                    << nmHandle.c_str()
                                    << "nmState:" << common::NetworkStateTypeToString(nmState).c_str();
                    outerPromise->SetError(res.Error());
                }
                future = nullptr;
            });
        } else {
            log_.LogError() << "StateMachine::ChangeNetworkStateOnly(), appendEventHandler_ is nullptr";
            promise.SetError(SMErrc::kRejected);
        }
    }
}

/// @brief Start the state machine
/// @param source Source of the state machine start request
/// @param smState State machine state
/// @return Future object
core::Future< void > StateMachine::Start(FGStateChangeSource const source,
                                         core::String smState,
                                         uint64_t const &nmHandleId) noexcept
{
    // Initialize working status to NormalTransition
    workStatus_ = StateMachineWorkStatusType::kNormalTransition;  // ErrorRecovery/RequestState are allowed during start
    log_.LogInfo() << "StateMachine::Start(), smFQN_:" << smFQN_.c_str() << "smState:" << smState.c_str()
                   << "source:" << ToString(source) << "processStatus_:" << ToString(processStatus_).c_str()
                   << "workStatus_:" << ToString(workStatus_).c_str() << "nmHandleId:" << nmHandleId;
    std::shared_ptr< ara::core::Promise< void > > promise{std::make_shared< core::Promise< void > >()};
    core::Future< void > future{promise->get_future()};
    if (smState.empty() && !started_) {
        if (!initialStartState_.empty()) {
            smState = initialStartState_;
        } else {
            smState = common::GetkInitialState();
        }
    } else if (smState.empty() && started_) {  // No state machine state specified, and the state machine is already started, return directly
        promise->set_value();
        return future;
    }

    started_ = true;
    if ((smState == currentState_) && (ProcessStatusType::kProcessing == processStatus_)) {
        log_.LogDebug() << "StateMachine::Start(), smFQN_:" << smFQN_.c_str()
                        << "target state is same with currentState_:" << currentState_.c_str()
                        << "and processStatus_ is" << ToString(processStatus_).c_str() << "so reject this transition";
        promise->SetError(SMErrc::kRejected);
        return future;
    }
    // Stop current processing
    if (ProcessStatusType::kProcessing == processStatus_) {
        _stopProcessing(StateMachineWorkStatusType::kNormalTransition);
    }
    processStatus_ = ProcessStatusType::kProcessing;  // Start processing

    // Use the Exec interface of GlobalGeneralEvLoop to put it into the evloop for execution
    std::ignore = isoft::naicpp::GlobalGeneralEvLoop::Get()->Exec(
        [this, smState, source, promiseInner = std::move(promise), nmHandleId]() mutable noexcept {
            _onStartEvent(smState, source, std::move(promiseInner), nmHandleId);
        });
    return future;
}

/// @brief State machine start event
/// @param smState State machine state
/// @param source Source of the state machine start request
/// @param promise Used to asynchronously set the execution result
void StateMachine::_onStartEvent(core::String const &smState,
                                 FGStateChangeSource const source,
                                 std::shared_ptr< core::Promise< void > > const &promise,
                                 uint64_t const &nmHandleId) noexcept
{
    log_.LogInfo() << "StateMachine::_onStartEvent(), processStatus_:" << ToString(processStatus_).c_str()
                   << "workStatus_:" << ToString(workStatus_).c_str() << "smFQN_:" << smFQN_.c_str()
                   << "smState:" << smState.c_str() << "source:" << ToString(source);
    if (StateMachineWorkStatusType::kErrorRecovery == workStatus_) {
        log_.LogWarn() << "StateMachine::_onStartEvent(), workStatus_:" << ToString(workStatus_).c_str()
                       << "smFQN_:" << smFQN_.c_str() << "smState:" << smState.c_str() << "source:" << ToString(source)
                       << ", so return";
        promise->SetError(SMErrc::kRejected);
        return;
    }
    if (FGStateChangeSource::kNM == source) {
        for (std::pair< core::String const, ActionItemList > const &pair : state2ActionListMap_) {
            ActionItemList const &actionItems{pair.second};
            for (const auto &actionItemBasePtr : actionItems) {
                if (ActionType::kSetNetworkState == actionItemBasePtr->GetType()) {
                    auto pActionItemSetNetworkState{
                        std::dynamic_pointer_cast< ActionItemSetNetworkState >(actionItemBasePtr)};
                    pActionItemSetNetworkState->SetNmHHandleId(nmHandleId);
                }
            }
        }
    }
    // switch to the InitialState immediately and start processing ActionListItems from the ActionList referencing this
    // StateMachine State
    SwitchToState(StateMachineWorkStatusType::kNormalTransition, (core::StringView{smState.c_str()}), source,
                  std::move(*(promise.get())));
}

/// @brief  Called at the very beginning of Terminate, because it needs the msControlServerVector_ variable to notify the outside world of the state machine Final state;
/// But actually the function group switching required when the state machine stops does not need StateManager::Run() to run; and the following SwitchToState will inevitably succeed
/// @return Future object
core::Future< void > StateMachine::Stop() noexcept
{
    log_.LogInfo() << "StateMachine::Stop(), smFQN_:" << smFQN_.c_str();
    std::shared_ptr< ara::core::Promise< void > > promise{std::make_shared< core::Promise< void > >()};
    core::Future< void > future{promise->get_future()};
    if (started_) {  // Already started
        // Use the Exec interface of GlobalGeneralEvLoop to put it into the evloop for execution
        std::ignore = isoft::naicpp::GlobalGeneralEvLoop::Get()->Exec(
            [this, promiseInner = std::move(promise)]() mutable noexcept { _onStopEvent(std::move(promiseInner)); });
        started_ = false;
    } else {
        promise->set_value();
    }
    return future;
}

/// @brief Register the callback function for appending events
/// @param appendEventHandler Callback function for appending events
void StateMachine::RegisterAppendEventHandler(
    std::function< void(common::Event &&) > const &appendEventHandler) noexcept
{
    log_.LogInfo() << "StateMachine::RegisterAppendEventHandler(), smFQN_:" << smFQN_.c_str();
    appendEventHandler_ = appendEventHandler;

    // Traverse the action lists of all states
    if (true == state2ActionListMap_.empty()) {
        log_.LogError() << "StateMachine::RegisterAppendEventHandler(), no actions!";
    } else {
        for (std::pair< ara::core::String const, ara::sm::state_machine_management::ActionItemList > const &pair1 :
             state2ActionListMap_) {
            core::String const smState{pair1.first};
            ara::sm::state_machine_management::ActionItemList actionList{pair1.second};
            for (std::shared_ptr< ActionItemBase > const &actionListPtr : actionList) {
                actionListPtr->RegisterAppendEventHandler(appendEventHandler);
            }
        }
    }
}

/// @brief Register the callback function for obtaining execution errors
/// @param getExecutionErrorHandler Callback function for obtaining execution errors
void StateMachine::RegisterGetExecutionErrorHandler(
    std::function< core::Result< exec::ExecutionErrorEvent >(core::String const &) > const
        &getExecutionErrorHandler) noexcept
{
    log_.LogInfo() << "StateMachine::RegisterGetExecutionErrorHandler(), smFQN_:" << smFQN_.c_str();
    // Traverse the action lists of all states
    if (true == state2ActionListMap_.empty()) {
        log_.LogError() << "StateMachine::RegisterGetExecutionErrorHandler(), no actions!";
    } else {
        for (std::pair< ara::core::String const, ara::sm::state_machine_management::ActionItemList > const &pair1 :
             state2ActionListMap_) {
            core::String const smState{pair1.first};
            ara::sm::state_machine_management::ActionItemList actionList{pair1.second};
            for (std::shared_ptr< ActionItemBase > const &actionItemBasePtr : actionList) {
                if (ActionType::kSetFunctionGroupState == actionItemBasePtr->GetType()) {
                    /// Conversion from base class to derived class
                    std::shared_ptr< ActionItemSetFGState > pActionItemSetFGState{
                        std::dynamic_pointer_cast< ActionItemSetFGState >(actionItemBasePtr)};
                    pActionItemSetFGState->RegisterGetExecutionErrorHandler(getExecutionErrorHandler);
                }
            }
        }
    }
}

/// @brief State machine stop event
/// @param promise Used to asynchronously set the execution result
void StateMachine::_onStopEvent(std::shared_ptr< core::Promise< void > > const &promise) noexcept
{
    workStatus_ = StateMachineWorkStatusType::kOff;  // During stop, ErrorRecovery/RequestState is no longer needed (allowed)
    log_.LogInfo() << "StateMachine::_onStopEvent(), processStatus_:" << ToString(processStatus_)
                   << "workStatus_:" << ToString(workStatus_);

    // stop processing ActionListItems from the ActionList referencing the current StateMachine State i.e., call:
    if (ProcessStatusType::kProcessing == processStatus_) {
        _stopProcessing(StateMachineWorkStatusType::kOff);
    }
    processStatus_ = ProcessStatusType::kProcessing;
    // switch to the FinalState immediately and start processing ActionListItems from the ActionList referencing this
    // StateMachine State
    SwitchToState(StateMachineWorkStatusType::kOff, common::GetkFinalState(), FGStateChangeSource::kInternal,
                  std::move(*(promise.get())));  // Will inevitably succeed
}

/// @brief Get the current externally broadcast state of the state machine
/// @return The current externally broadcast state of the state machine
core::String StateMachine::GetCurrentNotifierSMState() const noexcept
{
    log_.LogInfo() << "StateMachine::GetCurrentNotifierSMState(), currentNotifierState_:" << currentNotifierState_;
    return currentNotifierState_;
}

/// @brief Get the current state of the function group
/// @param fgFQN Function group name
/// @return Current state of the function group
core::String const &StateMachine::GetCurrentFGState(core::String const &fgFQN) noexcept
{
    log_.LogInfo() << "StateMachine::GetCurrentFGState(), fgFQN:" << fgFQN << "smFQN_:" << smFQN_;
    return fg2State_[fgFQN];
}

/// @brief Get the function groups managed by this state machine
/// @return The function groups managed by this state machine
core::Vector< core::String > StateMachine::GetManagedFGs() noexcept
{
    log_.LogInfo() << "StateMachine::GetManagedFGs(), smFQN_:" << smFQN_;
    core::Vector< core::String > fgFQNs;
    for (std::pair< ara::core::String const, ara::core::String > const &pair : fg2State_) {
        fgFQNs.emplace_back(pair.first);
    }
    return fgFQNs;
}

/// @brief  Output the current state
void StateMachine::Print() const noexcept
{
    log_.LogDebug() << "StateMachine::Print() start, smFQN_:" << smFQN_;

    // Set of states
    log_.LogDebug() << "StateMachine::Print(), states_:" << common::ConcatenateStrings(states_);

    // Mapping of state machine states to action lists
    log_.LogDebug() << "StateMachine::Print(), state2ActionListMap_ is as follow:";
    for (std::pair< ara::core::String const, ara::sm::state_machine_management::ActionItemList > const &it :
         state2ActionListMap_) {
        log_.LogDebug() << "StateMachine::Print(), state" << it.first;
        log_.LogDebug() << "StateMachine::Print(), ActionList is:" << ToString(it.second);
    }

    // Transition request table
    log_.LogDebug() << "StateMachine::Print(), transitionRequestTable_ is as follow:";
    for (std::pair< TransitionRequestType const, std::map< core::String, core::String > > const &it :
         transitionRequestTable_) {
        log_.LogDebug() << "StateMachine::Print(), TransitionRequest" << it.first;
        for (std::pair< ara::core::String const, ara::core::String > const &it2 : it.second) {
            log_.LogDebug() << "StateMachine::Print(), CurrentState:" << it2.first << "NextState:" << it2.second;
        }
    }

    // Extended transition request table
    log_.LogDebug() << "StateMachine::Print(), extendTransitionRequestTable_ is as follow:";
    for (std::pair< core::String const, std::map< core::String, core::String > > const &it :
         extendTransitionRequestTable_) {
        log_.LogDebug() << "StateMachine::Print(), TransitionRequest" << it.first;
        for (std::pair< ara::core::String const, ara::core::String > const &it2 : it.second) {
            log_.LogDebug() << "StateMachine::Print(), CurrentState:" << it2.first << "NextState:" << it2.second;
        }
    }

    // Error recovery table
    log_.LogDebug() << "StateMachine::Print(), errorRecoveryTable_ is as follow:";
    for (std::pair< ara::exec::ExecutionError const, ara::core::String > const &it : errorRecoveryTable_) {
        log_.LogDebug() << "StateMachine::Print(), ExecutionError" << it.first << "NextState:" << it.second;
    }

    log_.LogDebug() << "StateMachine::Print(), isController_:" << isController_;
    log_.LogDebug() << "StateMachine::Print(), only for internal State Machine fg2Sm_:"
                    << common::ConcatenateStrings(fg2Sm_);

    // Default error recovery state
    log_.LogDebug() << "StateMachine::Print(), anyErrRecoveryState_:" << anyErrRecoveryState_;

    // Current state machine state
    log_.LogDebug() << "StateMachine::Print(), currentState_:" << currentState_;
    log_.LogDebug() << "StateMachine::Print(), currentNotifierState_:" << currentNotifierState_;
    log_.LogDebug() << "StateMachine::Print() end, smFQN_:" << smFQN_;
}

/// @brief Check whether it is possible to transition to the next state machine state
/// @param nextState Next state
/// @return Whether the transition is allowed
core::Result< void > StateMachine::_canTransitionTo(core::String const &nextState) const noexcept
{
    log_.LogInfo() << "StateMachine::_canTransitionTo(), begin with nextState:" << nextState
                   << "currentState_:" << currentState_ << "processStatus_:" << ToString(processStatus_);

    if (nextState
        == currentState_) {  // Regardless of whether in NormalTransition or ErrorRecovery (including when there is ErrorRecovery in NormalTransition), if the next state is the same as the current state, further determine:
        if (ProcessStatusType::kProcessing == processStatus_) {  // Processing
            log_.LogError() << "StateMachine::_canTransitionTo(), a transition to the requested state is already "
                               "ongoing for smFQN_:"
                            << smFQN_;
            return core::Result< void >::FromError(
                SMErrc::kInTransitionToSameState);  // kInTransitionToSameState: transitioning to the same state;
        }
        if (ProcessStatusType::kSuccess == processStatus_) {  // Processing succeeded;
            log_.LogWarn()
                << "StateMachine::_canTransitionTo(), The StateMachine is already in requested state for smFQN_:"
                << smFQN_;
            return core::Result< void >::FromError(SMErrc::kAlreadyInState);  // kAlreadyInState: already in that state;
        }
        if (ProcessStatusType::kFailed == processStatus_) {  // Processing did not succeed;
            log_.LogDebug()
                << "StateMachine::_canTransitionTo(), return true because of last failed process for smFQN_:" << smFQN_;
            return core::Result< void >::FromValue();  // If the processing of the current state did not complete successfully, it is still possible to transition to that state; return success
        }
        // Processing status is Off
        log_.LogDebug() << "StateMachine::_canTransitionTo(), return true for smFQN_:" << smFQN_;
        return core::Result< void >::FromValue();
    }
    log_.LogDebug() << "StateMachine::_canTransitionTo(), return true for smFQN_:" << smFQN_;
    return core::Result< void >::FromValue();  // Return success
}

/// @brief Handling of RequestSMState event
/// @param transitionRequest Transition request
/// @param source Source of the state machine transition request
/// @param promise Used to asynchronously set the execution result
void StateMachine::RequestSMState(TransitionRequestType const transitionRequest,
                                  FGStateChangeSource const source,
                                  core::Promise< void > &&promise) noexcept
{
    log_.LogInfo() << "StateMachine::RequestSMState(), begin with transitionRequest:" << transitionRequest
                   << "currentState_:" << currentState_ << "workStatus_:" << ToString(workStatus_)
                   << "processStatus_:" << ToString(processStatus_) << "smFQN_:" << smFQN_;

    if (StateMachineWorkStatusType::kOff == workStatus_) {
        // Reject the request
        promise.SetError(SMErrc::kNotWorking);
        log_.LogError() << "StateMachine::RequestSMState(), The StateMachine is not working for smFQN_:" << smFQN_;
        return;
    }
    if (StateMachineWorkStatusType::kErrorRecovery == workStatus_) {  // Error recovery in progress
        promise.SetError(SMErrc::kRecoveryTransitionOngoing);
        log_.LogError() << "StateMachine::RequestSMState(), Request will not be carried out, because currently "
                           "recovery is ongoing for smFQN_:"
                        << smFQN_;
        return;
    }
    // StateMachine can call this interface in NormalTransition state:
    // Search transitionRequest to get the next state
    core::String nextState{};
    std::map< TransitionRequestType, std::map< core::String, core::String > >::iterator const it1{
        transitionRequestTable_.find(transitionRequest)};
    if (it1 == transitionRequestTable_.end()) {  // transitionRequest does not exist
        promise.SetError(SMErrc::kInvalidValue);
        log_.LogError() << "StateMachine::RequestSMState(), The provided value:" << transitionRequest
                        << "is not mapped to any transition for smFQN_:" << smFQN_;
        return;
    }
    // Search currentState
    std::map< ara::core::String, ara::core::String >::iterator const it2{it1->second.find(currentState_)};
    if (it2 == it1->second.end()) {  // currentState_ does not exist
        promise.SetError(SMErrc::kTransitionNotAllowed);
        log_.LogError() << "StateMachine::RequestSMState(), Requested transition is not possible from "
                           "current StateMachine state:"
                        << currentState_ << "for transitionRequest:" << transitionRequest << "smFQN_:" << smFQN_;
        return;
    }
    // Save the next state machine state
    nextState = it2->second;
    log_.LogDebug() << "StateMachine::RequestSMState(), get nextState:" << nextState
                    << "for transitionRequest:" << transitionRequest << "smFQN_:" << smFQN_;

    // Check whether it is possible to transition to the next state:
    ara::core::Result< void > const res{_canTransitionTo(nextState)};
    if (res.HasValue() == false) {
        promise.SetResult(res);
        return;
    }

    // Stop current processing
    if (ProcessStatusType::kProcessing == processStatus_) {
        _stopProcessing(StateMachineWorkStatusType::kNormalTransition);
    }
    processStatus_ = ProcessStatusType::kProcessing;  // Start processing

    // Clear error
    lasExecutionError_ = core::nullopt;

    // Immediately transition to the next state machine state, then start executing the corresponding action list
    SwitchToState(StateMachineWorkStatusType::kNormalTransition, nextState, source, std::move(promise));
}

void StateMachine::RequestSMStateFromNM(core::String const nmTransitionRequest) noexcept  // NOLINT
{
    std::map< core::String, std::map< core::String, core::String > >::iterator const it1{
        extendTransitionRequestTable_.find(nmTransitionRequest)};
    if (it1 == extendTransitionRequestTable_.end()) {  // transitionRequest does not exist
        return;
    }
    log_.LogInfo() << "StateMachine::RequestSMStateFromNM(), begin with nmTransitionRequest:"
                   << nmTransitionRequest.c_str() << "workStatus_:" << ToString(workStatus_)
                   << "processStatus_:" << ToString(processStatus_) << "smFQN_:" << smFQN_;

    if (StateMachineWorkStatusType::kOff == workStatus_) {
        // Reject the request
        log_.LogWarn() << "StateMachine::RequestSMStateFromNM(), The StateMachine is not working for smFQN_:" << smFQN_;
        return;
    }
    if (StateMachineWorkStatusType::kErrorRecovery == workStatus_) {  // Error recovery in progress
        log_.LogWarn() << "StateMachine::RequestSMStateFromNM(), Request will not be carried out, because currently "
                          "recovery is ongoing for smFQN_:"
                       << smFQN_;
        return;
    }
    // StateMachine can call this interface in NormalTransition state:
    // Search transitionRequest to get the next state

    // Search currentState
    std::map< ara::core::String, ara::core::String >::iterator const it2{it1->second.find(currentState_)};
    if (it2 == it1->second.end()) {  // currentState_ does not exist
        log_.LogWarn() << "StateMachine::RequestSMStateFromNM(), Requested transition is not possible from "
                          "current StateMachine state:"
                       << currentState_ << "for nmTransitionRequest:" << nmTransitionRequest << "smFQN_:" << smFQN_;
        return;
    }
    // Save the next state machine state
    core::String const nextState{it2->second};
    log_.LogDebug() << "StateMachine::RequestSMStateFromNM(), get nextState:" << nextState
                    << "for nmTransitionRequest:" << nmTransitionRequest << "smFQN_:" << smFQN_;

    // Check whether it is possible to transition to the next state:
    ara::core::Result< void > const res{_canTransitionTo(nextState)};
    if (res.HasValue() == false) {
        log_.LogWarn() << "StateMachine::RequestSMStateFromNM(), can't transition to next StateMachine state:"
                       << nextState << "for nmTransitionRequest:" << nmTransitionRequest << "smFQN_:" << smFQN_;
        return;
    }

    // Stop current processing
    if (ProcessStatusType::kProcessing == processStatus_) {
        _stopProcessing(StateMachineWorkStatusType::kNormalTransition);
    }
    processStatus_ = ProcessStatusType::kProcessing;  // Start processing

    // Clear error
    lasExecutionError_ = core::nullopt;

    core::Promise< void > promise;
    std::shared_ptr< core::Future< void > > future{std::make_shared< core::Future< void > >(promise.get_future())};
    // Immediately transition to the next state machine state, then start executing the corresponding action list
    SwitchToState(StateMachineWorkStatusType::kNormalTransition, nextState, FGStateChangeSource::kNM,
                  std::move(promise));
    std::ignore = future->then([future, nextState, nmTransitionRequest, this]() mutable {
        ara::core::Result< void > const requestRes{future->GetResult()};
        if (requestRes.HasValue()) {
            log_.LogInfo() << "StateMachine::RequestSMStateFromNM(), succeed to transition from" << currentState_
                           << "to next StateMachine state:" << nextState
                           << "for nmTransitionRequest:" << nmTransitionRequest << "smFQN_:" << smFQN_;
        } else {
            log_.LogWarn() << "StateMachine::RequestSMStateFromNM(), failed to transition from" << currentState_
                           << "to next StateMachine state:" << nextState
                           << "for nmTransitionRequest:" << nmTransitionRequest
                           << "err:" << requestRes.Error().Message() << "smFQN_:" << smFQN_;
        }
        future = nullptr;
    });
}

/// @brief Handling of error recovery event, result is not cared about
/// @param executionErrorEvent Execution error
/// @param promise Used to asynchronously set the execution result
void StateMachine::ErrorRecovery(ara::exec::ExecutionErrorEvent const &executionErrorEvent,
                                 core::Promise< void > &&promise) noexcept
{
    log_.LogInfo() << "StateMachine::ErrorRecovery(), begin with executionErrorEvent:"
                   << common::ErrorEventToString(executionErrorEvent) << "currentState_:" << currentState_
                   << "workStatus_:" << ToString(workStatus_) << "processStatus_:" << ToString(processStatus_)
                   << "smFQN_:" << smFQN_;
    if (StateMachineWorkStatusType::kOff == workStatus_) {
        // Reject the request //When the StateMachine stops, it will set the state of the related function group to Off (and setting the function group state to Off will keep retrying, so error recovery no longer needs to be considered).
        promise.SetError(SMErrc::kNotWorking);
        log_.LogError() << "StateMachine::ErrorRecovery(), The StateMachine is not working for smFQN_:" << smFQN_;
        return;
    }
    // Error
    ara::exec::ExecutionError executionError{executionErrorEvent.executionError};
    log_.LogDebug() << "StateMachine::ErrorRecovery(), get executionError:" << executionError;

    // Search executionError to get the next state
    core::String nextState{};
    std::map< ara::exec::ExecutionError, ara::core::String >::iterator const it{
        errorRecoveryTable_.find(executionError)};
    if (it == errorRecoveryTable_.end()) {  // executionError does not exist
        log_.LogDebug() << "StateMachine::ErrorRecovery(), executionError:" << executionError
                        << "is not mapped to any transition for smFQN_:" << smFQN_;

        // Whether the default value is valid
        if (states_.end() == std::find(states_.begin(), states_.end(), anyErrRecoveryState_)) {
            promise.SetError(SMErrc::kInvalidValue);
            log_.LogWarn() << "StateMachine::ErrorRecovery(), anyErrRecoveryState_:" << anyErrRecoveryState_
                           << "is invalid for smFQN_:" << smFQN_;
            return;
        }

        // Use the default value as the next state machine state
        nextState = anyErrRecoveryState_;
        log_.LogDebug() << "StateMachine::ErrorRecovery(), we will use anyErrRecoveryState_:" << anyErrRecoveryState_
                        << "as nextState.";

    } else {
        // Save the next state machine state
        nextState = it->second;
        log_.LogDebug() << "StateMachine::ErrorRecovery(), find nextState:" << nextState
                        << "for executionError:" << executionError << "in errorRecoveryTable_ within smFQN_:" << smFQN_;
    }

    // Check whether it is possible to transition to the next state
    ara::core::Result< void > const res{_canTransitionTo(nextState)};
    if (res.HasValue() == false) {
        promise.SetResult(res);
        return;
    }

    // If the current ExecutionError is the same as the last failed ExecutionError, do not continue
    if (ProcessStatusType::kFailed == processStatus_) {
        if (lasExecutionError_) {
            if (executionError == *lasExecutionError_) {
                log_.LogWarn()
                    << "StateMachine::ErrorRecovery(), return because the coming executionError is the same as "
                       "lasExecutionError_, smFQN_:"
                    << smFQN_;
                promise.SetError(SMErrc::kTransitionFailed);
                return;
            }
        }
    }

    // Stop current processing
    if (ProcessStatusType::kProcessing == processStatus_) {
        _stopProcessing(StateMachineWorkStatusType::kErrorRecovery);
    }
    processStatus_ = ProcessStatusType::kProcessing;  // Start processing
    // Save the error
    lasExecutionError_ = executionError;

    // Set the working status of the state machine to ErrorRecovery; moving this step down is to wait for the previous ErrorRecovery to end (which may reset
    // ErrorRecoveryOngoing). Moving it here has no semantic problem.
    //  set internal flag ErrorRecoveryOngoing
    workStatus_ = StateMachineWorkStatusType::kErrorRecovery;
    log_.LogDebug() << "StateMachine::ErrorRecovery(), workStatus_ is set to:" << ToString(workStatus_)
                    << "smFQN_:" << smFQN_;

    // Immediately transition to the next state machine state, then start executing the corresponding action list
    SwitchToState(workStatus_, nextState, FGStateChangeSource::kInternal, std::move(promise));
}

/// @brief Switch state machine state
/// @param startFrom State machine working status when starting the transition
/// @param nextState Target state
/// @param source Source of the state machine start request
/// @param promise Used to asynchronously set the execution result
void StateMachine::SwitchToState(StateMachineWorkStatusType const startFrom,
                                 core::StringView const nextState,
                                 FGStateChangeSource const source,
                                 core::Promise< void > &&promise) noexcept
{
    log_.LogInfo() << "StateMachine::SwitchToState(), startFrom:" << ToString(startFrom) << "nextState:" << nextState
                   << "source:" << ToString(source) << "smFQN_:" << smFQN_;
    if (isController_) {
        if (common::GetkOffState() == nextState) {
            log_.LogError() << "StateMachine::SwitchToState(), smFQN_:" << smFQN_ << "can't be change to Off state";
            promise.SetError(SMErrc::kRejected);
            return;
        }
        assert(checkShutdownHandler_);
        if (!(checkShutdownHandler_(nextState, source).HasValue())) {
            log_.LogError() << "StateMachine::SwitchToState(), smFQN_:" << smFQN_ << "can't shutdown or restart";
            promise.SetError(SMErrc::kRejected);
            processStatus_ = ProcessStatusType::kFailed;  // Start processing
            return;
        }
    }
    // Set the current state to the next state
    currentState_ = nextState;
    log_.LogDebug() << "StateMachine::SwitchToState(), currentState_ is set to:" << currentState_
                    << "smFQN_:" << smFQN_;

    // Current notification state
    currentNotifierState_ = common::GetkInTransition();
    _notifySMStateChange(smFQN_, currentNotifierState_);

    // Query to get the corresponding ActionList according to nextState;
    ActionItemList const *actionItemList{nullptr};  // Action list
    std::map< ara::core::String, ActionItemList >::iterator const it{state2ActionListMap_.find(currentState_)};
    if (it == state2ActionListMap_.end()) {  // No action list
        log_.LogWarn() << "StateMachine::SwitchToState(), there is no ActionList for currentState_:" << currentState_
                       << "smFQN_:" << smFQN_;
    } else {
        actionItemList = &(it->second);
        log_.LogDebug() << "StateMachine::SwitchToState(), got actionItemList:" << ToString(*actionItemList)
                        << "for currentState_:" << currentState_ << "smFQN_:" << smFQN_;
    }
    _startProcessActionList(startFrom, actionItemList, std::move(promise));
}

/// @brief Start executing the action list
/// @param startFrom State machine working status when starting to process the action list
/// @param actionItemList Mapping of state machine states to action lists
/// @param promise Used to asynchronously set the execution result
void StateMachine::_startProcessActionList(StateMachineWorkStatusType const startFrom,
                                           ActionItemList const *const actionItemList,
                                           core::Promise< void > &&promise) noexcept
{
    if (nullptr != actionItemList) {
        log_.LogInfo() << "StateMachine::_startProcessActionList(), startFrom:" << ToString(startFrom)
                       << "actionItemList:" << ToString(*actionItemList)
                       << "processStatus_:" << ToString(processStatus_) << "smFQN_:" << smFQN_;
    } else {
        log_.LogInfo() << "StateMachine::_startProcessActionList(), startFrom:" << ToString(startFrom)
                       << "actionItemList is nullptr, processStatus_:" << ToString(processStatus_)
                       << "smFQN_:" << smFQN_;
    }

    // Assign context
    processContext_->startFrom      = startFrom;
    processContext_->actionItemList = actionItemList;
    processContext_->nextGroupIndex = 0UL;  // Start position of the next processing group
    processContext_->groupNum       = 0UL;  // Number of non-SYNC actions in the current processing group
    processContext_->hasErrorResult = false;
    processContext_->requestPromise = std::move(promise);

    ContinueProcessingActionList(0U);

    // Start processing from the beginning
    log_.LogDebug() << "StateMachine::_startProcessActionList(), now processContext_:" << ToString(processContext_)
                    << "currentState_:" << currentState_ << "smFQN_:" << smFQN_;
}

/// @brief Determine whether this state machine is the master state machine (i.e., the state machine managing MachineFG)
/// @return true - is the master state machine
/// @return false - is not the master state machine
bool StateMachine::IsController() const noexcept { return isController_; }

/// @brief Handling of RequestFGState event
/// @param fgFQN Function group name
/// @param fgState Function group state
/// @param source Source of the function group transition request
/// @param promise Used to asynchronously set the execution result
void StateMachine::RequestFGState(core::String const &fgFQN,
                                  core::String const &fgState,
                                  FGStateChangeSource const source,
                                  core::Promise< void > &&promise,
                                  uint64_t const &nmHandleId) noexcept
{
    log_.LogInfo() << "StateMachine::RequestFGState(), fgFQN:" << fgFQN << "fgState:" << fgState
                   << "source:" << ToString(source) << "smFQN_:" << smFQN_ << "isUserDefined_:" << isUserDefined_
                   << "processStatus_:" << ToString(processStatus_) << "nmHandleId:" << nmHandleId;
    assert(checkFGDeactivatedHandler_);
    std::shared_ptr< core::Promise< void > > newPromise{std::make_shared< core::Promise< void > >(std::move(promise))};
    if (checkFGDeactivatedHandler_(fgFQN)) {
        log_.LogError() << "StateMachine::RequestFGState(), deactivated fgFQN:" << fgFQN;
        newPromise->SetError(SMErrc::kFGDeactived);
        return;
    }

    if (FGStateChangeSource::kAA == source) {
        if (isUserDefined_) {  // If a user-defined state machine manages this function group, AA cannot switch the function group state through the Trigger service
            log_.LogError()
                << "StateMachine::RequestFGState(), fgFQN:" << fgFQN
                << "is managed by user defined StateMachine, please use SMControlApplication to change fg state";
            newPromise->SetError(SMErrc::kRejected);
            return;
        }
    }

    core::String const offState{common::GetkOffState()};
    core::String const restartState{common::GetkRestartState()};
    core::String const shutDownState{common::GetkShutdownState()};
    bool const isOffState{offState == fgState};
    bool const isRestartState{restartState == fgState};
    bool const isShutDownState{shutDownState == fgState};
    if (isController_ && (isOffState || isRestartState || isShutDownState)) {
        if (common::GetkOffState() == fgState) {
            log_.LogError() << "StateMachine::RequestFGState(), fgFQN:" << fgFQN << "can't be change to Off state";
            newPromise->SetError(SMErrc::kRejected);
            return;
        }
        // The Shutdown or Restart states of MachineFG are uniformly implemented through the state machine
        assert(checkShutdownHandler_);
        if (!(checkShutdownHandler_(fgState, source).HasValue())) {
            log_.LogError() << "StateMachine::RequestFGState(), fgFQN:" << fgFQN << "can't shutdown or restart";
            newPromise->SetError(SMErrc::kRejected);
            return;
        }
        Start(newPromise, source, fgState, nmHandleId);
        return;
    }

    fg2ProcessID_[fgFQN]++;
    uint32_t const fgProcessID{fg2ProcessID_[fgFQN]};
    uint32_t const delayTime{static_cast< uint32_t >(nmAfterRunTime_.fgOffTime * 1000.0F)};
    log_.LogDebug() << "StateMachine::RequestFGState(), delayTime:" << delayTime;
    // If a user-defined state machine manages this function group, then switching the function group state cannot be achieved by switching the state machine state. In this case, the linkage between function group state and network state is involved.
    if (isUserDefined_ || (FGStateChangeSource::kUCM == source)) {
        // This is an Off request from network management, triggered by the network state changing to NoCom. Therefore, consider delaying fgOffTime to switch the function group to the Off state.
        if ((FGStateChangeSource::kNM == source) && isOffState && (0U < delayTime)) {
            std::shared_ptr< EvNodeTimer > timer{nullptr};  /// Timer pointer
            std::ignore = isoft::naicpp::GlobalGeneralEvLoop::Get()->MakeTimer(
                timer, static_cast< int32_t >(delayTime),
                [this, fgFQN, fgState, fgProcessID, newPromise, nmHandleId]() {
                    log_.LogDebug() << "StateMachine::RequestFGState(), fgFQN:" << fgFQN << "fgState:" << fgState;
                    if (fgProcessID == fg2ProcessID_[fgFQN]) {
                        ChangeFGAndRelatedNetworkStates(fgFQN, fgState, fgProcessID, newPromise, nmHandleId);
                    } else {
                        log_.LogError() << "StateMachine::RequestFGState(), ChangeFGAndRelatedNetworkStates canceled";
                        newPromise->SetError(SMErrc::kRejected);
                    }
                    std::ignore = fgProcessID2Timer_.erase(fgFQN + core::to_string(fgProcessID));
                });
            std::ignore = fgProcessID2Timer_.emplace(fgFQN + core::to_string(fgProcessID), std::move(timer));
        } else {
            ChangeFGAndRelatedNetworkStates(fgFQN, fgState, fgProcessID, newPromise, nmHandleId);
        }
    } else {
        ara::core::String smState{fg2Sm_[fgState]};
        if (smState.empty()) {
            log_.LogError() << "StateMachine::RequestFGState(), invalid fgState:" << fgState;
            newPromise->SetError(SMErrc::kFGStateIllegal);
        } else {
            if ((FGStateChangeSource::kNM == source) && (ProcessStatusType::kProcessing == processStatus_)) {
                log_.LogDebug() << "StateMachine::RequestFGState(), pend smState request to" << smState;
                PendingSMStateRequest request;
                request.smState = std::move(smState);
                request.source  = source;
                request.promise = newPromise;
                pendingSmStateRequests_.push(std::move(request));
            } else if ((FGStateChangeSource::kNM == source) && isOffState && (0U < delayTime)) {
                std::shared_ptr< EvNodeTimer > timer{nullptr};  /// Timer pointer
                std::ignore = isoft::naicpp::GlobalGeneralEvLoop::Get()->MakeTimer(
                    timer, static_cast< int32_t >(delayTime),
                    [this, fgFQN, smState, newPromise, source, fgProcessID, nmHandleId]() {
                        log_.LogDebug() << "StateMachine::RequestFGState(), Start, smState:" << smState;
                        if (fgProcessID == fg2ProcessID_[fgFQN]) {
                            Start(newPromise, source, smState, nmHandleId);
                        } else {
                            log_.LogError() << "StateMachine::RequestFGState(), Start canceled";
                            newPromise->SetError(SMErrc::kRejected);
                        }
                        std::ignore = fgProcessID2Timer_.erase(fgFQN + core::to_string(fgProcessID));
                    });
                std::ignore = fgProcessID2Timer_.emplace(fgFQN + core::to_string(fgProcessID), std::move(timer));
            } else {
                Start(newPromise, source, smState, nmHandleId);
            }
        }
    }
}  // namespace state_machine_management

/// @brief Continue processing the action list until the end or encountering SYNC
/// @param startIndex The index of the action list to start execution next time
void StateMachine::ContinueProcessingActionList(ActionItemList::size_type const startIndex) noexcept
{
    log_.LogInfo() << "StateMachine::ContinueProcessingActionList(), startIndex:" << startIndex << "smFQN_:" << smFQN_;

    // Actual start position
    processContext_->currentGroupIndex = startIndex;

    // Calculate the number of non-Sync actions in this processing group
    processContext_->groupNum     = 0UL;
    processContext_->sleepItemNum = 0UL;
    ActionItemList::size_type luIndex{processContext_->currentGroupIndex};
    while (true) {
        if (0UL != processContext_->groupNum) {
            break;
        }
        if (processContext_->actionItemList == nullptr) {
            break;
        }
        if (luIndex == processContext_->actionItemList->size()) {
            break;
        }
        for (; luIndex != processContext_->actionItemList->size(); luIndex++) {
            ActionType const type{(*(processContext_->actionItemList))[luIndex]->GetType()};
            if (type != ActionType::kSync) {  // If actionItem is not Sync
                if (type != ActionType::kSleep) {
                    processContext_->groupNum++;  // Sleep action items also do not increase groupNum
                } else {
                    processContext_->sleepItemNum++;
                }
            } else {  // If actionItem is Sync or Sleep
                luIndex++;
                break;
            }
        }
        if (0UL == processContext_->groupNum) {  // If there are no actions to process, continue searching
            processContext_->currentGroupIndex = luIndex;
        }
    }

    // Save the start position of the next processing group
    processContext_->nextGroupIndex = luIndex;
    log_.LogDebug() << "StateMachine::ContinueProcessingActionList(), got groupNum:" << processContext_->groupNum
                    << "currentGroupIndex:" << processContext_->currentGroupIndex
                    << "nextGroupIndex:" << processContext_->nextGroupIndex << "smFQN_:" << smFQN_;
    if (0UL == processContext_->groupNum) {  // If there are no actions to process
        _onProcessSuccess();
    } else {
        // Process processContext_.groupNum actions from currentGroupIndex in processContext_.actionList
        ActionItemList::size_type const endIndex{processContext_->currentGroupIndex + processContext_->groupNum
                                                 + processContext_->sleepItemNum};
        uint32_t sleepTime{0U};
        // Set the delay time for non-Sleep action items and execute them

        std::size_t currentGroupIndex{processContext_->currentGroupIndex};
        while (currentGroupIndex != endIndex) {
            if (processContext_->hasErrorResult) {
                break;
            }
            std::shared_ptr< ActionItemBase > action{(*(processContext_->actionItemList))[currentGroupIndex]};
            if (ActionType::kSleep == action->GetType()) {
                sleepTime = sleepTime + action->GetSleepTime();
            } else {
                action->SetSleepTime(sleepTime);
                action->StartExecute(processContext_->processID);
            }
            log_.LogInfo() << "StateMachine::ContinueProcessingActionList(), action:" << action->ActionItemToString()
                           << "sleepTime:" << sleepTime << "index:" << currentGroupIndex;
            currentGroupIndex++;
        }
    }
}

/// @brief The action list processing ended successfully
void StateMachine::_onProcessSuccess() noexcept
{
    log_.LogInfo() << "StateMachine::_onProcessSuccess(), begin with processContext_:" << ToString(processContext_)
                   << "smFQN_:" << smFQN_;

    // Current notification state
    // When a TriggerOut interface is configured for the StateMachinethe value of the "Notifier" field shall be set to
    // the current StateMachine State as soon as all ActionListItems (in the ActionList referencing the current
    // StateMachine State) have been executed and all results have been collected.
    currentNotifierState_ = currentState_;
    log_.LogDebug() << "StateMachine::_onProcessSuccess(), try to NotifySMStateUpdate:" << currentNotifierState_
                    << "smFQN_:" << smFQN_;
    _notifySMStateChange(smFQN_, currentNotifierState_);

    // The internal ErrorRecoveryOngoing flag shall be reset, when all ActionListItems of an ActionList referencing a
    // StateMachine State, which is requested due to error reaction, are successfully processed.
    if (StateMachineWorkStatusType::kErrorRecovery == workStatus_) {  // If the current working status is ErrorRecovery
        workStatus_ = StateMachineWorkStatusType::kNormalTransition;
        log_.LogDebug() << "StateMachine::_onProcessSuccess(), workStatus_ is set to:" << ToString(workStatus_)
                        << "smFQN_:" << smFQN_;
    }

    // If the current working status is Off
    if (StateMachineWorkStatusType::kOff == workStatus_) {
        processStatus_ = ProcessStatusType::kOff;
    } else {
        processStatus_ = ProcessStatusType::kSuccess;
    }
    log_.LogDebug() << "StateMachine::_onProcessSuccess(), processStatus_ is set to:" << ToString(processStatus_)
                    << "smFQN_:" << smFQN_;
    // Set the value of requestPromise
    processContext_->requestPromise.set_value();
    _handlePendingRequest();
}

/// @brief Get the position of the action item of type type and matching the description actionItemDescriptionStr in the action list of SMState
/// @param smState State machine state
/// @param type Action type
/// @param actionItemDescriptionStr Action description string
/// @return Position of the action item
uint32_t StateMachine::_getActionItemPos(core::String const &smState,
                                         ActionType const type,
                                         core::String const &actionItemDescriptionStr) noexcept
{
    log_.LogInfo() << "StateMachine::_getActionItemPos(), smState:" << smState << "type:" << ToString(type)
                   << "actionItemDescriptionStr:" << actionItemDescriptionStr;
    ActionItemList const &actionList{state2ActionListMap_[smState]};
    for (size_t i{0U}; i < actionList.size(); i++) {
        if (type == actionList[i]->GetType()) {
            ara::core::String const descriptionStr{actionList[i]->ActionItemToString()};
            if (ara::core::String::npos != descriptionStr.find(actionItemDescriptionStr)) {
                return static_cast< uint32_t >(i);
            }
        }
    }
    return UINT32_MAX;
}

/// @brief Insert an action item before the index position in the action list of SMState
/// @param smState State machine state
/// @param luIndex Index of the action item in the action list
/// @param actionItem The action item to be inserted
void StateMachine::_insertActionItem(core::String const &smState,
                                     ActionItemList::size_type const luIndex,
                                     std::shared_ptr< ActionItemBase > const &actionItem) noexcept
{
    log_.LogInfo() << "StateMachine::_insertActionItem(), smState:" << smState << "index:" << luIndex;
    if (actionItem) {
        ActionItemList &actionList{state2ActionListMap_[smState]};
        if (luIndex < actionList.size()) {
            log_.LogDebug() << "StateMachine::_insertActionItem(), smState:" << smState << "index:" << luIndex
                            << "actionItem:" << actionItem->ActionItemToString();
            std::ignore = actionList.insert(actionList.cbegin() + static_cast< int32_t >(luIndex), actionItem);
        } else if (luIndex == actionList.size()) {
            actionList.push_back(actionItem);
        } else {
            log_.LogError() << "StateMachine::_insertActionItem(), index is bigger than actionList.size()";
        }
    } else {
        log_.LogError() << "StateMachine::_insertActionItem(), actionItem is nullptr";
    }
}

/// @brief Handling for when the action list processing fails
void StateMachine::OnProcessFailure() noexcept
{
    // Processing ID + 1
    processContext_->processID++;
    log_.LogInfo() << "StateMachine::OnProcessFailure(), processContext_:" << ToString(processContext_)
                   << "processID:" << processContext_->processID << "smFQN_:" << smFQN_;

    // Cancel the timer
    // Process processContext_.groupNum actions from processContext_->currentGroupIndex in processContext_.actionList
    ActionItemList::size_type luIndex{processContext_->currentGroupIndex};
    ActionItemList::size_type const endIndex{processContext_->nextGroupIndex};
    while (luIndex != endIndex) {
        if (processContext_->hasErrorResult) {
            break;
        }
        (*(processContext_->actionItemList))[luIndex]->StopAppendingEvent();
        luIndex++;
    }

    // StateMachineService's Method: RequestState can return kTransitionFailed
    // Returning this during error recovery does not conflict with the below
    //    The internal ErrorRecoveryOngoing flag shall be reset, when all ActionListItems of an ActionList referencing a
    //    StateMachine State, which is requested due to error reaction, are successfully processed. The RequestState
    //    method shall return kRecoveryTransitionOngoing if internal flag ErrorRecoveryOngoing is set
    processContext_->requestPromise.SetError(SMErrc::kTransitionFailed);

    // After error recovery fails, the state machine should still be able to respond to RequestState
    if (StateMachineWorkStatusType::kErrorRecovery == workStatus_) {  // If the current working status is ErrorRecovery
        workStatus_ = StateMachineWorkStatusType::kNormalTransition;
        log_.LogDebug() << "StateMachine::OnProcessFailure(), workStatus_ is set to:" << ToString(workStatus_)
                        << "smFQN_:" << smFQN_;
    }
    processStatus_ = ProcessStatusType::kFailed;
    _handlePendingRequest();
    log_.LogInfo() << "StateMachine::OnProcessFailure(), workStatus_:" << ToString(workStatus_)
                   << "processStatus_:" << ToString(processStatus_) << "processContext_:" << ToString(processContext_)
                   << "smFQN_:" << smFQN_;
}

/// @brief Stop the ongoing processing
/// @param stopFrom State machine working status when finishing processing the action list
void StateMachine::_stopProcessing(StateMachineWorkStatusType const stopFrom) noexcept
{
    log_.LogInfo() << "StateMachine::_stopProcessing(), stopFrom:" << ToString(stopFrom).c_str()
                   << "processStatus_:" << ToString(processStatus_).c_str()
                   << "workStatus_:" << ToString(workStatus_).c_str()
                   << "processContext_:" << ToString(processContext_).c_str() << "smFQN_:" << smFQN_;

    // Processing ID + 1
    processContext_->processID++;
    log_.LogDebug() << "StateMachine::_stopProcessing(), processID is set to:" << processContext_->processID
                    << "smFQN_:" << smFQN_;

    // Cancel the timer
    // Process processContext_.groupNum actions from processContext_->currentGroupIndex in processContext_.actionList
    ActionItemList::size_type luIndex{processContext_->currentGroupIndex};
    ActionItemList::size_type const endIndex{processContext_->nextGroupIndex};
    while (luIndex != endIndex) {
        if (processContext_->hasErrorResult) {
            break;
        }
        (*(processContext_->actionItemList))[luIndex]->StopAppendingEvent();
        luIndex++;
    }

    if (StateMachineWorkStatusType::kOff == stopFrom) {  // Stopped for Off
        if (StateMachineWorkStatusType::kOff == workStatus_) {
            assert(false);
        } else if ((StateMachineWorkStatusType::kNormalTransition == workStatus_)
                   || (StateMachineWorkStatusType::kErrorRecovery == workStatus_)) {
            log_.LogDebug() << "StateMachine::_stopProcessing(), try to SetError(kCanceled) because of stopFrom:"
                            << ToString(stopFrom) << "and workStatus_:" << ToString(workStatus_) << "smFQN_:" << smFQN_;
            processContext_->requestPromise.SetError(SMErrc::kCanceled);
        } else {
        }
        processStatus_ = ProcessStatusType::kFailed;
    } else if (StateMachineWorkStatusType::kNormalTransition == stopFrom) {  // Stopped for NormalTransition request
        if (StateMachineWorkStatusType::kOff == workStatus_) {
            assert(false);
        } else if (StateMachineWorkStatusType::kNormalTransition == workStatus_) {
            log_.LogDebug() << "StateMachine::_stopProcessing(), try to SetError(kCanceled) because of stopFrom:"
                            << ToString(stopFrom) << "workStatus_:" << ToString(workStatus_) << "smFQN_:" << smFQN_;
            processContext_->requestPromise.SetError(SMErrc::kCanceled);
        } else if (StateMachineWorkStatusType::kErrorRecovery == workStatus_) {
            assert(false);
        } else {
        }
        processStatus_ = ProcessStatusType::kFailed;
    } else if (StateMachineWorkStatusType::kErrorRecovery == stopFrom) {  // Stopped for ErrorRecovery
        if (StateMachineWorkStatusType::kOff == workStatus_) {
            assert(false);
        } else if ((StateMachineWorkStatusType::kNormalTransition == workStatus_)
                   || (StateMachineWorkStatusType::kErrorRecovery == workStatus_)) {
            log_.LogDebug()
                << "StateMachine::_stopProcessing(), try to SetError(kTransitionFailed) because of stopFrom:"
                << ToString(stopFrom) << "and workStatus_:" << ToString(workStatus_) << "smFQN_:" << smFQN_;
            processContext_->requestPromise.SetError(SMErrc::kTransitionFailed);
        } else {
        }
        processStatus_ = ProcessStatusType::kFailed;
    } else {
        assert(false);
    }

    log_.LogInfo() << "StateMachine::_stopProcessing(), processStatus_ is set to:" << ToString(processStatus_)
                   << "processContext_:" << ToString(processContext_) << "smFQN_:" << smFQN_;
}

/// @brief Initialize the action list
/// @param info Function group information
/// @return true - success
/// @return false - failure
bool StateMachine::InitActionItemList(config::FGInfo const &info) noexcept
{
    log_.LogInfo() << "StateMachine::InitActionItemList()";
    core::String const fgFQN{info.fgFQN};
    if (fg2State_[fgFQN].empty()) {
        fg2State_[fgFQN] = common::GetkOffState();
    }
    if (!isController_) {
        std::size_t const slashPos1{fgFQN.rfind("[")};
        core::String const slashPart{fgFQN.substr(slashPos1 + 1U)};
        std::size_t const slashPos2{slashPart.rfind("]")};
        core::String fgName{slashPart.substr(0U, slashPos2)};
        if (fgName.empty() || (std::string::npos == slashPos1) || (std::string::npos == slashPos2)) {
            std::size_t const slashPos{fgFQN.rfind("/")};
            fgName = fgFQN.substr(slashPos + 1U);
        }
        log_.LogDebug() << "StateMachine::InitActionItemList(), fgName:" << fgName.c_str();
        if (fgName == common::GetkMachineFunctionGroupName()) {
            isController_    = true;
            fg2State_[fgFQN] = common::GetkStartupState();
            log_.LogDebug() << "StateMachine::InitActionItemList(), self defined, find Controller, smFQN_:" << smFQN_
                            << "fgFQN:" << fgFQN << "curFgState:" << common::GetkStartupState();
        }
    }

    for (ara::core::String const &fgState : info.fgStates) {
        if (common::GetkOffState() == fgState) {
            states_.push_back(common::GetkFinalState());
            fg2Sm_[fgState] = common::GetkFinalState();
            log_.LogDebug() << "StateMachine::InitActionItemList() test fg2Sm_[fgState]" << common::GetkFinalState();
        } else {
            states_.push_back(fgState);
            fg2Sm_[fgState] = fgState;
            log_.LogDebug() << "StateMachine::InitActionItemList() test fg2Sm_[fgState]" << fgState;
        }
        std::shared_ptr< ActionItemSetFGState > const action{std::make_shared< ActionItemSetFGState >(fgFQN, fgState)};
        state2ActionListMap_[fg2Sm_[fgState]].push_back(action);
        state2ActionListMap_[fg2Sm_[fgState]].push_back(std::make_shared< ActionItemSync >());
    }
    states_.push_back(common::GetkInitialState());
    if (!isController_) {
        std::shared_ptr< ActionItemSetFGState > const action{
            std::make_shared< ActionItemSetFGState >(fgFQN, common::GetkOffState())};
        state2ActionListMap_[common::GetkInitialState()].push_back(action);
    } else {
        std::shared_ptr< ActionItemSetFGState > const action{
            std::make_shared< ActionItemSetFGState >(fgFQN, common::GetkStartupState())};
        state2ActionListMap_[common::GetkInitialState()].push_back(action);
    }
    state2ActionListMap_[common::GetkInitialState()].push_back(std::make_shared< ActionItemSync >());
    return true;
}

/// @brief Initialize the state machine according to function group information
/// @param smFQN State machine full name
/// @param info Function group information
/// @return true - success
/// @return false - failure
bool StateMachine::Init(core::String const &smFQN, sm::config::FGInfo const &info) noexcept
{
    log_.LogInfo() << "StateMachine::Init(), smFQN:" << smFQN.c_str() << "fgFQN:" << info.fgFQN.c_str()
                   << "fgStates:" << common::ConcatenateStrings(info.fgStates);
    smFQN_          = smFQN;
    std::ignore     = InitActionItemList(info);
    processContext_ = std::make_shared< ProcessActionListContext >();
    return true;
}

/// @brief Set the required function handles for all action items in the action list
void StateMachine::RegisterHandles2Actions() noexcept
{
    log_.LogInfo() << "StateMachine::RegisterHandles2Actions()";
    // Traverse the action lists of all states
    if (true == state2ActionListMap_.empty()) {
        log_.LogError() << "StateMachine::RegisterHandles2Actions(), no actions!";
    } else {
        for (std::pair< ara::core::String const, ara::sm::state_machine_management::ActionItemList > const &pair1 :
             state2ActionListMap_) {
            core::String const smState{pair1.first};
            ara::sm::state_machine_management::ActionItemList actionList{pair1.second};
            for (std::shared_ptr< ActionItemBase > const &actionItemBasePtr : actionList) {
                if (ActionType::kSetFunctionGroupState == actionItemBasePtr->GetType()) {
                    /// Conversion from base class to derived class
                    std::shared_ptr< ActionItemSetFGState > pActionItemSetFGState{
                        std::dynamic_pointer_cast< ActionItemSetFGState >(actionItemBasePtr)};
                    pActionItemSetFGState->RegisterChangeFgStateHandler([this](core::String const &fgFQNInLambda,
                                                                               core::String const &fgStateInLambda,
                                                                               core::Promise< void > &&promise) {
                        ChangeFGStateOnly(fgFQNInLambda, fgStateInLambda, std::move(promise));
                    });
                    pActionItemSetFGState->RegisterPostExecuteHandlerWithStrParas(
                        [this](core::String const &fgFQN, core::String const &fgState) {
                            UpdateFGState(fgFQN, fgState);
                        });
                    pActionItemSetFGState->RegisterProcessFailureHandler([this]() noexcept { OnProcessFailure(); });
                    pActionItemSetFGState->RegisterContinueProcessingActionListHandler(
                        [this](size_t const luIndex) noexcept { ContinueProcessingActionList(luIndex); });
                    pActionItemSetFGState->RegisterErrorRecoveryHandler(
                        [this](ara::exec::ExecutionErrorEvent const &error, core::Promise< void > &&promise) noexcept {
                            ErrorRecovery(error, std::move(promise));
                        });
                    pActionItemSetFGState->SetProcessActionListContext(processContext_);
                }
                if (ActionType::kSetNetworkState == actionItemBasePtr->GetType()) {
                    /// Conversion from base class to derived class
                    std::shared_ptr< ActionItemSetNetworkState > pActionItemSetNetworkState{
                        std::dynamic_pointer_cast< ActionItemSetNetworkState >(actionItemBasePtr)};
                    pActionItemSetNetworkState->RegisterChangeNetworkStateHandler(
                        [this](core::String const &nmHandle, common::NetworkStateInternalType const &nmState,
                               core::Promise< void > &&promise, uint64_t const &nmHandleId) {
                            ChangeNetworkStateOnly(nmHandle, nmState, std::move(promise), nmHandleId);
                        });
                    pActionItemSetNetworkState->RegisterProcessFailureHandler(
                        [this]() noexcept { OnProcessFailure(); });
                    pActionItemSetNetworkState->RegisterContinueProcessingActionListHandler(
                        [this](size_t const luIndex) noexcept { ContinueProcessingActionList(luIndex); });
                    pActionItemSetNetworkState->RegisterErrorRecoveryHandler(
                        [this](ara::exec::ExecutionErrorEvent const &error, core::Promise< void > &&promise) noexcept {
                            ErrorRecovery(error, std::move(promise));
                        });
                    pActionItemSetNetworkState->SetProcessActionListContext(processContext_);
                }
                if (ActionType::kStartStateMachine == actionItemBasePtr->GetType()) {
                    /// Conversion from base class to derived class
                    std::shared_ptr< ActionItemStartSM > pActionItemStartSM{
                        std::dynamic_pointer_cast< ActionItemStartSM >(actionItemBasePtr)};
                    pActionItemStartSM->RegisterStartStateMachineHandler(startStateMachineHandler_);
                    pActionItemStartSM->RegisterProcessFailureHandler([this]() noexcept { OnProcessFailure(); });
                    pActionItemStartSM->RegisterContinueProcessingActionListHandler(
                        [this](size_t const luIndex) noexcept { ContinueProcessingActionList(luIndex); });
                    pActionItemStartSM->RegisterErrorRecoveryHandler(
                        [this](ara::exec::ExecutionErrorEvent const &error, core::Promise< void > &&promise) noexcept {
                            ErrorRecovery(error, std::move(promise));
                        });
                    pActionItemStartSM->SetProcessActionListContext(processContext_);
                }
                if (ActionType::kStopStateMachine == actionItemBasePtr->GetType()) {
                    /// Conversion from base class to derived class
                    std::shared_ptr< ActionItemStopSM > pActionItemStopSM{
                        std::dynamic_pointer_cast< ActionItemStopSM >(actionItemBasePtr)};
                    pActionItemStopSM->RegisterStopStateMachineHandler(stopStateMachineHandler_);
                    pActionItemStopSM->RegisterProcessFailureHandler([this]() noexcept { OnProcessFailure(); });
                    pActionItemStopSM->RegisterContinueProcessingActionListHandler(
                        [this](size_t const luIndex) noexcept { ContinueProcessingActionList(luIndex); });
                    pActionItemStopSM->RegisterErrorRecoveryHandler(
                        [this](ara::exec::ExecutionErrorEvent const &error, core::Promise< void > &&promise) noexcept {
                            ErrorRecovery(error, std::move(promise));
                        });
                    pActionItemStopSM->SetProcessActionListContext(processContext_);
                }
            }
        }
    }
}

bool StateMachine::IsUserDefined() const noexcept { return isUserDefined_; }

core::Vector< core::String > const &StateMachine::GetSmStates() const noexcept { return states_; }

core::Vector< common::TransitionItemSM > StateMachine::GetTransitionTable() const noexcept
{
    core::Vector< common::TransitionItemSM > table;
    for (std::pair< TransitionRequestType const, std::map< core::String, core::String > > const &pair :
         transitionRequestTable_) {
        uint32_t const requestID{pair.first};
        std::map< core::String, core::String > const &currrent2NextStateMap{pair.second};
        for (std::pair< core::String const, core::String > const &pairState : currrent2NextStateMap) {
            common::TransitionItemSM item{requestID, pairState.first, pairState.second};
            table.push_back(std::move(item));
        }
    }
    return table;
}

void StateMachine::RegisterGetNmHandlerNameFun(std::function< core::String(uint64_t const &) > getNmHandlerNameFun)
{
    getNmHandlerNameFun_ = getNmHandlerNameFun;
}

/// @brief Publish a state machine state change event to EventManager
/// @param smFQN State machine full name
/// @param smNotifierState Externally published state machine state
void StateMachine::_notifySMStateChange(core::String const &smFQN, core::String const &smNotifierState) const noexcept
{
    log_.LogInfo() << "StateMachine::_notifySMStateChange(), smFQN:" << smFQN.c_str()
                   << "smNotifierState:" << smNotifierState.c_str();
    if (isUserDefined_) {
        if (appendEventHandler_) {
            core::Promise< void > notifierPromise;
            common::Event request;
            request.type           = common::EventType::kOutNotifySMStateChange;
            request.data           = new common::SMStateInfo{smFQN, smNotifierState};
            request.requestPromise = std::move(notifierPromise);
            appendEventHandler_(std::move(request));
        } else {
            log_.LogError()
                << "StateMachine::_notifySMStateChange(), fail to Notify, because appendEventHandler_ is nullptr";
        }
    }
}

/// @brief Handle postponed state transition requests
void StateMachine::_handlePendingRequest() noexcept
{
    log_.LogInfo() << "StateMachine::_handlePendingRequest(), smFQN_:" << smFQN_.c_str();
    if (pendingSmStateRequests_.empty() == false) {
        PendingSMStateRequest const request{std::move(pendingSmStateRequests_.front())};
        Start(request.promise, request.source, request.smState);
        pendingSmStateRequests_.pop();
    }
}

/// @brief Overloaded function to start the state machine
/// @param promise Used to asynchronously set the execution result
/// @param smState State machine state
void StateMachine::Start(std::shared_ptr< core::Promise< void > > const &promise,
                         FGStateChangeSource const source,
                         core::String const &smState,
                         uint64_t const &nmHandleId) noexcept
{
    log_.LogInfo() << "StateMachine::Start() overload version, smState:" << smState.c_str()
                   << "smFQN:" << smFQN_.c_str() << "processStatus_:" << ToString(processStatus_).c_str()
                   << "nmHandleId:" << nmHandleId;
    std::shared_ptr< core::Future< void > > future{
        std::make_shared< core::Future< void > >(Start(source, smState, nmHandleId))};
    std::ignore = future->then([future, promise]() mutable {
        ara::core::Result< void > const res{future->GetResult()};
        if (res.HasValue()) {
            promise->set_value();
        } else {
            promise->SetError(res.Error());
        }
        future = nullptr;
    });
}

/// @brief Asynchronously switch a group of networks to the same state
/// @param nmList List of networks
/// @param nmState Network state
/// @param promise Carries the switching result
void StateMachine::AsyncChangeNetworkStates(core::Vector< core::String > const &nmList,
                                            common::NetworkStateInternalType const &nmState,
                                            core::Promise< void > &&promise,
                                            uint64_t const &nmHandleId) noexcept
{
    log_.LogInfo() << "StateMachineManager::AsyncChangeNetworkStates(), nmList:"
                   << common::ConcatenateStrings(nmList).c_str()
                   << "nmState:" << common::NetworkStateTypeToString(nmState).c_str();
    if (nmList.empty() == false) {
        std::shared_ptr< core::Promise< void > > const newPromise{
            std::make_shared< core::Promise< void > >(std::move(promise))};
        std::shared_ptr< uint32_t > const nmResponseCount{std::make_shared< uint32_t >(0UL)};
        uint32_t const nmTotalCount{static_cast< uint32_t >(nmList.size())};
        std::shared_ptr< bool > const fail{std::make_shared< bool >(false)};
        for (ara::core::String const &nmHandle : nmList) {
            core::Promise< void > callPromise;
            std::shared_ptr< core::Future< void > > outerFuture{
                std::make_shared< core::Future< void > >(callPromise.get_future())};
            log_.LogDebug() << "StateMachineManager::AsyncChangeNetworkStates(), try to change" << nmHandle.c_str()
                            << "state to" << common::NetworkStateTypeToString(nmState).c_str();
            ChangeNetworkStateOnly(nmHandle, nmState, std::move(callPromise), nmHandleId);
            std::ignore = outerFuture->then(
                [this, outerFuture, nmTotalCount, nmResponseCount, newPromise, fail, nmHandle, nmState]() mutable {
                    *nmResponseCount = *nmResponseCount + 1U;
                    ara::core::Result< void > const outerRes{outerFuture->GetResult()};
                    if (outerRes.HasValue()) {
                        log_.LogDebug() << "StateMachine::AsyncChangeNetworkStates(), ChangeNetworkStateOnly, NM "
                                           "change network state success, nmHandle:"
                                        << nmHandle.c_str()
                                        << "nmState:" << common::NetworkStateTypeToString(nmState).c_str();
                    } else {
                        if (!*fail) {
                            *fail = true;
                            newPromise->SetError(outerRes.Error());
                        }
                        log_.LogError() << "StateMachineManager::AsyncChangeNetworkStates(), ChangeNetworkStateOnly, "
                                           "NM change network state failed, nmHandle:"
                                        << nmHandle.c_str()
                                        << "nmState:" << common::NetworkStateTypeToString(nmState).c_str();
                    }
                    if (nmTotalCount == *nmResponseCount) {
                        if (!(*fail)) {
                            log_.LogDebug() << "StateMachineManager::AsyncChangeNetworkStates(), all success";
                            newPromise->set_value();
                        }
                    }
                    outerFuture = nullptr;
                });
        }
    } else {
        log_.LogWarn() << "StateMachineManager::AsyncChangeNetworkStates(), nmList is empty";
        promise.set_value();
    }
}

/// @brief Register the callback function for starting the state machine to the start state machine action item
/// @param startStateMachineHandler Callback function for starting the state machine
void StateMachine::RegisterStartStateMachineHandler(
    std::function< core::Future< void >(core::String const &, core::String const &) > const
        &startStateMachineHandler) noexcept
{
    log_.LogInfo() << "StateMachine::RegisterStartStateMachineHandler()";
    startStateMachineHandler_ = startStateMachineHandler;
}

/// @brief Register the callback function for stopping the state machine to the stop state machine action item
/// @param stopStateMachineHandler Callback function for stopping the state machine
void StateMachine::RegisterStopStateMachineHandler(
    std::function< core::Future< void >(core::String const &) > const &stopStateMachineHandler) noexcept
{
    log_.LogInfo() << "StateMachine::RegisterStopStateMachineHandler()";
    stopStateMachineHandler_ = stopStateMachineHandler;
}

}  // namespace state_machine_management
}  // namespace sm
}  // namespace ara
