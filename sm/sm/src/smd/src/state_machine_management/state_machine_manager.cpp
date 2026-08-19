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
/// @file       state_machine_manager.cpp
/// @brief      Define the state machine management class
/// @details
/// @date       2024-05-07
/// @author     lianglongxiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/StateManagement/StateMachineManagement
/// @interface_level=module
/// @trace_id_sr=SR_SM_00003, SR_SM_01001, SR_SM_01002, SR_SM_02001, SR_SM_02002, SR_SM_03002, SR_SM_04001, SR_SM_04002, SR_SM_04003, SR_SM_04004, SR_SM_04005,
/// SR_SM_04006, SR_SM_04007, SR_SM_04008,
/// @unit_name=StateMachineManager
/// @unit_description=Define the state machine management class
/// @endcode
///
/// ================================================================

#include "state_machine_manager.h"

#include <ara/core/string.h>
#include <ara/core/vector.h>
#include <ara/exec/exec_error_domain.h>
#include <ara/sm/error_domain_sm.h>

#include <algorithm>
#include <utility>

#include "action_item_stop_sm.h"
#include "action_item_sync.h"
#include "ara/sm/impl_type_functiongroupinfo.h"
#include "helper.h"

namespace ara {
namespace sm {
namespace state_machine_management {
/// @brief Define alias to resolve qac2428 item: Direct use of character type.
/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
using Char8_t = char;

/// @brief Constructor function
StateMachineManager::StateMachineManager() noexcept
    : globalConfigInstance_{}
    , sms_{}
    , controllerSm_{}
    , fg2sm_{}
    , fullComNmHandle2FGStatesMap_{}
    , noComNmHandle2FGStatesMap_{}
    , updatingFGSet_{}
#ifdef ARA_WITH_PER
    , updateSessionStatusStorage_{storage::Storage(core::StringView(common::GetkKVModelUpdateSessionIdentifier()))}
#endif

    , fgInfos_{}
{
    log_.LogInfo() << "StateMachineManager::StateMachineManager()";
}

/// @brief Destructor function
StateMachineManager::~StateMachineManager() noexcept
{
    log_.LogInfo() << "StateMachineManager::~StateMachineManager()";
}

/// @brief Register the callback function for appending events
/// @param appendEventHandler Callback function for appending events
void StateMachineManager::RegisterAppendEventHandler(
    std::function< void(common::Event &&) > const &appendEventHandler) noexcept
{
    log_.LogInfo() << "StateMachineManager::RegisterAppendEventHandler()";
    appendEventHandler_ = appendEventHandler;
}

/// @brief Register the callback function for obtaining execution errors
/// @param getExecutionErrorHandler Callback function for obtaining execution errors
void StateMachineManager::RegisterGetExecutionErrorHandler(
    std::function< core::Result< exec::ExecutionErrorEvent >(core::String const &) > const
        &getExecutionErrorHandler) noexcept
{
    log_.LogInfo() << "StateMachineManager::RegisterGetExecutionErrorHandler()";
    getExecutionErrorHandler_ = getExecutionErrorHandler;
}

/// @brief Initialize
/// @return true - success
/// @return false - failure
bool StateMachineManager::Init() noexcept
{
    log_.LogInfo() << "StateMachineManager::Init()";
    core::Vector< config::SMConfigInfo > smInfos{globalConfigInstance_->GetAllSMConfigInfoList()};
    config::NMAfterRunTimeInfo const nmAfterRunTime{globalConfigInstance_->GetNMAfterRunInfo()};
    fgInfos_ = globalConfigInstance_->GetAllFGInfoList();
    sms_.clear();
    fg2sm_.clear();
    bool findController{false};
    std::shared_ptr< StateMachine > pStateMachine{nullptr};
    // First, create state machines according to the state machine configuration
    for (const auto &smInfo : smInfos) {
        pStateMachine = std::make_shared< StateMachine >();
        pStateMachine->SetNMAfterRunTime(nmAfterRunTime);
        bool const initRes{pStateMachine->Init(smInfo)};
        if (!initRes) {
            log_.LogError() << "StateMachineManager::Init(), err because of user defined StateMachine:"
                            << smInfo.smFQN.c_str();
        } else {
            if (!findController) {
                if (pStateMachine->IsController()) {
                    controllerSm_  = pStateMachine;
                    findController = true;
                }
            }
            sms_[smInfo.smFQN] = pStateMachine;
            core::Vector< core::String > const managedFgs{pStateMachine->GetManagedFGs()};
            for (core::String const &fgFQN : managedFgs) {
                fg2sm_[fgFQN] = smInfo.smFQN;
            }
        }
    }
    // Create internal state machines for function groups not managed by any state machine
    for (const auto &fgInfo : fgInfos_) {
        log_.LogDebug() << "StateMachineManager::Init(), fgInfo:" << fgInfo.fgFQN.c_str();
        core::Map< core::String, core::String >::iterator const iter{fg2sm_.find(fgInfo.fgFQN)};
        if (fg2sm_.end() != iter) {
            continue;
        }
        // Create a state machine on its own
        pStateMachine = std::make_shared< StateMachine >();
        pStateMachine->SetNMAfterRunTime(nmAfterRunTime);
        core::String const smFQN{fgInfo.fgFQN + "SM"};
        const bool initRes{pStateMachine->Init(smFQN, fgInfo)};
        if (!initRes) {
            log_.LogError() << "StateMachineManager::Init(), err because of internal defined StateMachine:"
                            << fgInfo.fgFQN.c_str();
        } else {
            if (!findController) {
                if (pStateMachine->IsController()) {
                    controllerSm_  = pStateMachine;
                    findController = true;
                }
            }
            sms_[smFQN] = pStateMachine;
            core::Vector< core::String > const managedFgs{pStateMachine->GetManagedFGs()};
            for (core::String const &fgFQN : managedFgs) {
                fg2sm_[fgFQN] = smFQN;
            }
        }
    }

    // Add Shutdown and Restart states and their action lists to the master state machine
    std::vector< std::shared_ptr< ActionItemBase > > actionList;
    for (const auto &pair : sms_) {
        if (!pair.second->IsController()) {
            std::shared_ptr< ActionItemBase > acton{std::make_shared< ActionItemStopSM >(pair.first)};
            actionList.push_back(std::move(acton));
        }
    }
    if (controllerSm_) {
        if (actionList.empty() == false) {
            actionList.push_back(std::make_shared< ActionItemSync >());
            std::ignore = controllerSm_->InsertActionListToBegin(common::GetkShutdownState(), actionList);
            std::ignore = controllerSm_->InsertActionListToBegin(common::GetkRestartState(), actionList);
            std::ignore = controllerSm_->InsertActionListToBegin(common::GetkFinalState(), actionList);
        }
        controllerSm_->RegisterCheckShutdownHandler(
            [this](core::StringView const nextState, FGStateChangeSource const source) noexcept {
                return CheckCanShutdown(nextState, source);
            });
    }

#ifndef ARA_ENABLE_SM_2311
    // Modify action lists according to NM configuration
    core::Map< core::String, core::Map< core::String, core::Vector< core::String > > > const &fgState2NmHandles{
        globalConfigInstance_->GetFGState2NMHandlesMap()};
    core::Map< core::String, core::Vector< core::String > > const &offFgState2NoComNmHandles{
        globalConfigInstance_->GetOffFG2NoComNMHandlesMap()};
    for (const auto &pair : fgState2NmHandles) {
        core::String const &fgFQN{pair.first};
        core::Map< core::String, core::String >::iterator const fg2SmIt{fg2sm_.find(fgFQN)};
        if (fg2sm_.end() != fg2SmIt) {
            core::String const smFQN{fg2SmIt->second};
            sms_[smFQN]->ModifyActionListBasedOnFullComNMConfig(fgFQN, pair.second);
            core::Map< core::String, core::Vector< core::String > >::const_iterator const fg2HandelsIt{
                offFgState2NoComNmHandles.find(fgFQN)};
            if (offFgState2NoComNmHandles.end() != fg2HandelsIt) {
                sms_[smFQN]->ModifyActionListBasedOnNoComNMConfig(fgFQN, fg2HandelsIt->second);
            } else {
                log_.LogError() << "StateMachineManager::Init(), no NoComNmHandles for fgFQN:" << fgFQN.c_str();
            }
            sms_[smFQN]->RegisterGetNmHandlerNameFun(
                [this](uint64_t const &nmHandleId) { return getChangedNmHandle_(nmHandleId); });
        } else {
            log_.LogError() << "StateMachineManager::Init(), no StateMachine for fgFQN:" << fgFQN.c_str();
        }
    }
#endif

// Load update session state information from persistence
#ifdef ARA_WITH_PER
    log_.LogDebug() << "StateMachineManager::Init(), before updateSessionStatusStorage_ open";
    bool const ret{updateSessionStatusStorage_.Open()};
    if (!ret) {
        log_.LogError() << "StateMachineManager::Init(), failed to open UpdateSessionStatusStorage.";
    } else {
        // Get updated session state information
        log_.LogDebug() << "StateMachineManager::Init(), try to get update session status with:"
                        << common::GetkUpdateSessionStatusKey();
        core::Optional< core::Array< std::int64_t, TWO_UL > > statusOptional{
            updateSessionStatusStorage_.Get< core::Array< std::int64_t, TWO_UL > >(
                common::GetkUpdateSessionStatusKey())};
        if (statusOptional.has_value()) {
            core::Array< std::int64_t, TWO_UL > status{*statusOptional};
            updateSessionActive_    = static_cast< bool >(status[0U]);
            updateSessionStartTime_ = status[1U];

            log_.LogDebug() << "StateMachineManager::Init(), get updateSessionActive_:" << updateSessionActive_
                            << "updateSessionStartTime_:" << _timeStampSecondToStr(updateSessionStartTime_)
                            << "with:" << common::GetkUpdateSessionStatusKey();

            // Get the set of function groups being updated
            if (updateSessionActive_) {
                core::Optional< std::set< core::String > > updatingFGSetOptional{
                    updateSessionStatusStorage_.Get< std::set< core::String > >(common::GetkUpdatingFgSetKey())};
                if (updatingFGSetOptional.has_value()) {
                    updatingFGSet_ = *updatingFGSetOptional;
                    log_.LogDebug() << "StateMachineManager::Init(), got updatingFGSet_:"
                                    << common::ConcatenateStrings(updatingFGSet_).c_str();
                } else {
                    log_.LogDebug() << "StateMachineManager::Init(), not found:" << common::GetkUpdatingFgSetKey();
                }
            }
        } else {
            updateSessionActive_    = false;
            updateSessionStartTime_ = 0;
            log_.LogDebug() << "StateMachineManager::Init(), not found:" << common::GetkUpdateSessionStatusKey()
                            << "so set updateSessionActive_:" << updateSessionActive_
                            << "updateSessionStartTime_:" << updateSessionStartTime_;
        }
    }
#endif
    for (const auto &pair : sms_) {
        pair.second->RegisterAppendEventHandler(appendEventHandler_);
        pair.second->RegisterGetExecutionErrorHandler(getExecutionErrorHandler_);
        pair.second->RegisterStartStateMachineHandler(
            [this](core::String const &smFQN, core::String const &smState) noexcept {
                return StartStateMachine(smFQN, smState);
            });
        pair.second->RegisterStopStateMachineHandler(
            [this](core::String const &smFQN) noexcept { return StopStateMachine(smFQN); });
        pair.second->RegisterHandles2Actions();
        pair.second->SetCheckFGDeactivatedHandler(
            [this](core::String const &fgFQN) noexcept { return _checkFGIsDeactivated(fgFQN); });
        pair.second->Print();
    }

    fullComNmHandle2FGStatesMap_ = globalConfigInstance_->GetFullCommNMHandle2FGStatesMap();
    noComNmHandle2FGStatesMap_   = globalConfigInstance_->GetNoComNMHandle2FGStatesMap();
    return true;
}

/// @brief Start state management
/// @return true - success
/// @return false - failure
bool StateMachineManager::Start() noexcept
{
    log_.LogInfo() << "StateMachineManager::Start()";
    if (!started_) {
        core::Vector< core::String > returnedSms;
        uint32_t userSmCount{0U};
        for (const auto &pair : sms_) {
            if (pair.second->IsUserDefined()) {
                log_.LogDebug() << "StateMachineManager::Start(), user defined smFQN:" << pair.first.c_str();
                ++userSmCount;
                std::shared_ptr< core::Future< void > > future{std::make_shared< core::Future< void > >(
                    pair.second->Start(FGStateChangeSource::kInternal, common::GetkInitialState()))};
                std::ignore = future->then([this, future, &returnedSms, smFQN = pair.first]() mutable {
                    returnedSms.push_back(smFQN);
                    core::Result< void > const res{future->GetResult()};
                    if (res.HasValue()) {
                        log_.LogDebug() << "StateMachineManager::Start(), Start success for StateMachine:"
                                        << smFQN.c_str();
                    } else {
                        log_.LogDebug() << "StateMachineManager::Start(), Start err for StateMachine:" << smFQN.c_str()
                                        << "err info:" << res.Error().Message();
                    }
                    future = nullptr;
                });
            }
        }
        if (updateSessionActive_) {
            core::Promise< void > promise;
            ChangeFGState(common::GetkOTAUpdateFGName(), common::GetkContinueState(), FGStateChangeSource::kInternal,
                          std::move(promise));
        }
        while (returnedSms.size() < userSmCount) {
            std::this_thread::sleep_for(std::chrono::milliseconds(TEN_L));
        }
        started_ = true;
    }
    return true;
}

/// @brief Respond to network state changes
/// @param nmHandleName Network name
/// @param state Network state
void StateMachineManager::NotifyNetworkStateChanged(core::String const &nmHandleName,
                                                    common::NetworkStateInternalType const &state) noexcept
{
    core::String const targetStateStr{common::NetworkStateTypeToString(state)};
    log_.LogInfo() << "StateMachineManager::NotifyNetworkStateChanged(), nmHandleName:" << nmHandleName.c_str()
                   << "state:" << targetStateStr.c_str();
#ifndef ARA_ENABLE_SM_2311
    log_.LogInfo() << "StateMachineManager::NotifyNetworkStateChanged(), fullComNmHandle2FGStatesMap_ size:"
                   << fullComNmHandle2FGStatesMap_.size()
                   << "noComNmHandle2FGStatesMap_ size:" << noComNmHandle2FGStatesMap_.size();
    core::Map< core::String, core::String > fgStates;
    if (common::NetworkStateInternalType::kFullCom == state) {
        core::Map< core::String, core::Map< core::String, core::String > >::iterator const it{
            fullComNmHandle2FGStatesMap_.find(nmHandleName)};
        if (fullComNmHandle2FGStatesMap_.end() != it) {
            fgStates = it->second;
        }
    } else if (common::NetworkStateInternalType::kNoCom == state) {
        core::Map< core::String, core::Map< core::String, core::String > >::iterator const it{
            noComNmHandle2FGStatesMap_.find(nmHandleName)};
        if (noComNmHandle2FGStatesMap_.end() != it) {
            fgStates = it->second;
        }
    } else {
    }

    const auto now{std::chrono::system_clock::now()};
    const auto duration{now.time_since_epoch()};
    const auto nmHandleId{std::chrono::duration_cast< std::chrono::milliseconds >(duration).count()};
    log_.LogInfo() << "StateMachineManager::NotifyNetworkStateChanged(), nmHandleName:" << nmHandleName.c_str()
                   << "state:" << targetStateStr.c_str() << "nmHandleId:" << nmHandleId;
    {
        std::lock_guard< std::mutex > lock{nmMutex_};
        nmId2nmHandle_[nmHandleId] = nmHandleName;
    }

    for (const auto &pair : fgStates) {
        core::Promise< void > promise;
        auto future{std::make_shared< core::Future< void > >(promise.get_future())};
        ChangeFGState(pair.first, pair.second, FGStateChangeSource::kNM, std::move(promise), nmHandleId);
        future->then([this, future, nmHandleId, nmHandleName, targetStateStr]() mutable {
            auto res{future->GetResult()};
            if (res.HasValue()) {
                log_.LogInfo() << "StateMachineManager::NotifyNetworkStateChanged(), nmHandleName:"
                               << nmHandleName.c_str() << "state:" << targetStateStr.c_str() << "success";
            } else {
                log_.LogWarn() << "StateMachineManager::NotifyNetworkStateChanged(), nmHandleName:"
                               << nmHandleName.c_str() << "state:" << targetStateStr.c_str()
                               << "fails, reason:" << res.Error().Message().data();
            }
            {
                std::lock_guard< std::mutex > lock{nmMutex_};
                nmId2nmHandle_.erase(nmHandleId);
            }
        });
    }
#else
    core::String const nmRequest{nmHandleName + common::GetkActionItemBehaviorSeparator() + targetStateStr};
    for (const auto &pair : sms_) {
        std::shared_ptr< ara::sm::state_machine_management::StateMachine > pStateMachine{pair.second};
        pStateMachine->RequestSMStateFromNM(nmRequest);
    }
#endif  // !ARA_ENABLE_SM_2311
}

/// @brief Switch function group state
/// Switching function group state externally is not supported in the following two cases:
/// 1. The function group is managed by a state machine with a state transition table. In this case, the external should switch the function group by switching the state machine.
/// 2
/// The function group is managed by a state machine without a state transition table, but this state machine also manages at least 2 function groups including this one. In this case, the user is prompted to create a state transition table and switch the function group by switching the state machine externally.
/// @param fgFQN Function group name
/// @param fgState Function group state
/// @param source Source of the function group transition request
/// @param promise Used to asynchronously set the execution result
void StateMachineManager::ChangeFGState(core::String const &fgFQN,
                                        core::String const &fgState,
                                        FGStateChangeSource const source,
                                        core::Promise< void > &&promise,
                                        uint64_t const &nmHandleId) noexcept
{
    log_.LogInfo() << "StateMachineManager::ChangeFGState(), fgFQN:" << fgFQN << "fgState:" << fgState
                   << "source:" << ToString(source) << "nmHandleId:" << nmHandleId;
    if ((FGStateChangeSource::kInternal != source) && !started_) {
        log_.LogError() << "StateMachineManager::ChangeFGState(), StateMachineManager hasn't been started";
        promise.SetError(SMErrc::kRejected);
        return;
    }

    // In an update session, reject function group state transition requests other than from UCM
    if (_checkIsUpdating(fgFQN) && (FGStateChangeSource::kUCM != source)) {
        log_.LogError() << "StateMachineManager::ChangeFGState(), updating fgFQN:" << fgFQN.c_str();
        promise.SetError(SMErrc::kRejected);
        return;
    }
    core::String actualFgFQN{""};
    core::String actualSmFQN{""};
    if (_getFgAndSmFQN(fgFQN, actualFgFQN, actualSmFQN)) {
        core::Map< core::String, std::shared_ptr< StateMachine > >::iterator const smIt{sms_.find(actualSmFQN)};
        if (sms_.end() != smIt) {
            smIt->second->RequestFGState(actualFgFQN, fgState, source, std::move(promise), nmHandleId);
        } else {
            promise.SetError(SMErrc::kSMNotFound);
            log_.LogError() << "StateMachineManager::ChangeFGState(), invalid smFQN:" << actualSmFQN.c_str();
        }
    } else {
        promise.SetError(SMErrc::kFGNameIllegal);
        log_.LogError() << "StateMachineManager::ChangeFGState(), invalid fgFQN:" << fgFQN.c_str();
    }
}

/// @brief Switch function group state
/// Switching function group state externally is not supported in the following two cases:
/// 1. The function group is managed by a state machine with a state transition table. In this case, the external should switch the function group by switching the state machine.
/// 2
/// The function group is managed by a state machine without a state transition table, but this state machine also manages at least 2 function groups including this one. In this case, the user is prompted to create a state transition table and switch the function group by switching the state machine externally.
/// @param fgFQN Function group name
/// @param fgState Function group state
/// @param source Source of the function group transition request
/// @param promise Used to asynchronously set the execution result
void StateMachineManager::ChangeFGState(core::String const &fgFQN,
                                        core::String const &fgState,
                                        FGStateChangeSource const source,
                                        core::Promise< core::String > &&promise) noexcept
{
    log_.LogInfo() << "StateMachineManager::ChangeFGState(), fgFQN:" << fgFQN << "fgState:" << fgState
                   << "source:" << ToString(source);
    std::shared_ptr< core::Promise< core::String > > const newPromise{
        std::make_shared< core::Promise< core::String > >(std::move(promise))};
    core::Promise< void > voidPromise;
    std::shared_ptr< core::Future< void > > voidFuture{
        std::make_shared< core::Future< void > >(voidPromise.get_future())};
    ChangeFGState(fgFQN, fgState, source, std::move(voidPromise));
    std::ignore = voidFuture->then([this, voidFuture, newPromise, fgFQN, fgState, source]() mutable {
        ara::core::Result< void > const res{voidFuture->GetResult()};
        if (!res.HasValue()) {
            log_.LogError() << "StateMachineManager::ChangeFGState() error, fgFQN:" << fgFQN << "fgState:" << fgState
                            << "source:" << ToString(source) << "error:" << res.Error().Message();
            newPromise->SetError(res.Error());
        } else {
            newPromise->set_value("");
        }
        voidFuture = nullptr;
    });
}

/// @brief Execute state machine request transition
/// @param smFQN State machine name
/// @param transitionRequest Request sequence number, corresponding to the first column of the state transition table
/// @param promise Used to asynchronously set the execution result
void StateMachineManager::RequestState(core::String const &smFQN,
                                       TransitionRequestType const transitionRequest,
                                       core::Promise< void > &&promise) noexcept
{
    log_.LogInfo() << "StateMachineManager::RequestState(), smFQN:" << smFQN.c_str()
                   << "transitionRequest:" << transitionRequest;
    if (_checkIsUpdating(smFQN)) {
        log_.LogError() << "StateMachineManager::RequestState(), updating smFQN:" << smFQN.c_str();
        promise.SetError(SMErrc::kRejected);
        return;
    }
    core::String const actualSmFQN{_getSmFQN(smFQN)};
    core::Map< core::String, std::shared_ptr< StateMachine > >::iterator const smIt{sms_.find(actualSmFQN)};
    if (sms_.end() != smIt) {
        smIt->second->RequestSMState(transitionRequest, FGStateChangeSource::kAA, std::move(promise));
    } else {
        promise.SetError(SMErrc::kSMNotFound);
        log_.LogError() << "StateMachineManager::RequestState(), invalid actualSmFQN:" << actualSmFQN.c_str();
    }
}

/// @brief Execute error recovery
/// @param executionErrorEvent Execution error event
/// @param promise Used to asynchronously set the execution result
void StateMachineManager::ErrorRecovery(exec::ExecutionErrorEvent const &executionErrorEvent,
                                        core::Promise< void > &&promise) noexcept
{
    core::String const fgFQN{executionErrorEvent.functionGroup.GetMetaModelIdentifier()};
    if (_checkIsUpdating(fgFQN)) {
        log_.LogError() << "StateMachineManager::ErrorRecovery(), updating fgFQN:" << fgFQN.c_str();
        promise.SetError(SMErrc::kRejected);
        return;
    }
    ara::core::String const smFQN{fg2sm_[fgFQN]};
    log_.LogInfo() << "StateMachineManager::ErrorRecovery(), fgFQN:" << fgFQN.c_str() << "smFQN:" << smFQN.c_str();
    core::Map< core::String, std::shared_ptr< StateMachine > >::iterator const smIt{sms_.find(smFQN)};
    if (sms_.end() != smIt) {
        smIt->second->UpdateFGState(fgFQN, "");
        smIt->second->ErrorRecovery(executionErrorEvent, std::move(promise));
    } else {
        promise.SetError(SMErrc::kSMNotFound);
        log_.LogError() << "StateMachineManager::ErrorRecovery(), invalid smFQN:" << smFQN.c_str();
    }
}

/// @brief Execute UCM reset machine request
/// @param promise Used to asynchronously set the execution result
void StateMachineManager::ResetMachine(core::Promise< void > &&promise, FGStateChangeSource source) noexcept
{
    log_.LogInfo() << "StateMachineManager::ResetMachine()";
    std::shared_ptr< core::Promise< void > > const newPromise{
        std::make_shared< core::Promise< void > >(std::move(promise))};
    controllerSm_->Start(newPromise, source, common::GetkRestartState());
}
#ifdef ARA_WITH_UCM
/// @brief Execute UCM start update request
/// @param promise Used to asynchronously set the execution result
void StateMachineManager::StartUpdateSession(core::Promise< void > &&promise) noexcept
{
    log_.LogInfo() << "StateMachineManager::StartUpdateSession()";

    if (updateSessionActive_) {
        promise.SetError(SMErrc::kRejected);
        log_.LogError() << "StateMachineManager::StartUpdateSession(), all ready in update session";
    } else {
        // UpdateRequest's StartUpdateSession currently does not have session information, so the current value of updateSessionActive_ is not judged here
        updateSessionActive_ = true;
        // Current time
        updateSessionStartTime_ = std::time(nullptr);
    #ifdef ARA_WITH_PER
        // Save update session state
        _saveUpdateSessionStatus(updateSessionActive_, updateSessionStartTime_);
    #endif
        promise.set_value();
    }
}

/// @brief Execute UCM start update request
/// @param fgList List of function groups to be updated
/// @param promise Used to asynchronously set the execution result
void StateMachineManager::PrepareUpdate(common::UcmFunctionGroupListInternal const &fgList,
                                        core::Promise< void > &&promise) noexcept
{
    if (!updateSessionActive_) {
        log_.LogError() << "StateMachineManager::PrepareUpdate(), not in update session";
        promise.SetError(SMErrc::kRejected);
    } else {
        isInPrepareUpdate_ = true;
        // Save the set of function groups being updated
        std::ignore = copy(fgList.begin(), fgList.end(), inserter(updatingFGSet_, updatingFGSet_.end()));
    #ifdef ARA_WITH_PER
        _saveUpdatingFGSet(updatingFGSet_);
    #endif
        _asyncChangeFunctionGroups(fgList, common::GetkOffState(), FGStateChangeSource::kUCM, SMErrc::kPrepareFailed,
                                   std::move(promise));
    }
}

/// @brief Execute UCM start verification request
/// @param fgList List of function groups to start verification
/// @param promise Used to asynchronously set the execution result
void StateMachineManager::VerifyUpdate(common::UcmFunctionGroupListInternal const &fgList,
                                       core::Promise< void > &&promise) noexcept
{
    log_.LogInfo() << "StateMachineManager::VerifyUpdate(), begin with fgList:"
                   << common::ConcatenateStrings(fgList).c_str();
    if (!updateSessionActive_) {
        log_.LogError() << "StateMachineManager::VerifyUpdate(), not in update session";
        promise.SetError(SMErrc::kRejected);
    } else {
        isInVerifyUpdate_ = true;
        _asyncChangeFunctionGroups(fgList, common::GetkFGVerifyState(), FGStateChangeSource::kUCM,
                                   SMErrc::kVerifyFailed, std::move(promise));
    }
}

/// @brief Execute UCM prepare rollback request
/// @param fgList List of function groups to prepare rollback
/// @param promise Used to asynchronously set the execution result
void StateMachineManager::PrepareRollback(common::UcmFunctionGroupListInternal const &fgList,
                                          core::Promise< void > &&promise) noexcept
{
    log_.LogInfo() << "StateMachineManager::PrepareRollback(), begin with fgList:"
                   << common::ConcatenateStrings(fgList).c_str();
    if (!updateSessionActive_) {
        log_.LogError() << "StateMachineManager::PrepareRollback(), not in update session";
        promise.SetError(SMErrc::kRejected);
    } else {
        isInPrepareRollback_ = true;
        _asyncChangeFunctionGroups(fgList, common::GetkOffState(), FGStateChangeSource::kUCM, SMErrc::kRollbackFailed,
                                   std::move(promise));
    }
}

/// @brief Execute UCM end update request
/// @param promise Used to asynchronously set the execution result
void StateMachineManager::StopUpdateSession(core::Promise< void > &&promise) noexcept
{
    log_.LogInfo() << "StateMachineManager::StopUpdateSession()";
    promise.set_value();
    // TODO(longxiao.liang): Regarding StopUpdateSession:
    // Update and Config Management has to inform State Management when no more operations for the update have to be
    // done, thus State Management can clear now the information about an ongoing update and can continue its regular
    // job to set Function Groups into meaningful Function Group State.
    updateSessionActive_ = false;

    // Current time
    updateSessionStartTime_ = std::time(nullptr);

    // Clear the set of function groups being updated
    updatingFGSet_.clear();
    #ifdef ARA_WITH_PER
    updateSessionStatusStorage_.Remove(common::GetkUpdatingFgSetKey());

    // Save update session state
    _saveUpdateSessionStatus(updateSessionActive_, updateSessionStartTime_);
    #endif
}
#endif
/// @brief Get the current states of all function groups
/// @param promise Used to asynchronously set the execution result, the String format is: FG1.state;FG2.state
void StateMachineManager::GetAllFGStates(core::Promise< core::String > &&promise) noexcept
{
    log_.LogInfo() << "StateMachineManager::GetAllFGStates()";
    core::String strFgStates;
    for (const auto &pair : sms_) {
        std::shared_ptr< ara::sm::state_machine_management::StateMachine > pStateMachine{pair.second};
        core::Vector< core::String > const fgFQNs{pStateMachine->GetManagedFGs()};
        for (const auto &fgFQN : fgFQNs) {
            core::String const fgState{pStateMachine->GetCurrentFGState(fgFQN)};
            strFgStates += fgFQN + ara::core::String(common::GetkFgStateSplitStr()) + fgState
                           + ara::core::String(common::GetkFgSplitStr());
        }
    }
    promise.set_value(strFgStates);
}

/// @brief Get the current state of the specified function group
/// @param fgFQN Function group name
/// @param promise Used to asynchronously set the execution result
void StateMachineManager::GetFGState(core::String const &fgFQN, core::Promise< core::String > &&promise) noexcept
{
    log_.LogInfo() << "StateMachineManager::GetFGState() with promise, fgFQN:" << fgFQN.c_str();

    core::String actualFgFQN{""};
    core::String actualSmFQN{""};
    if (_getFgAndSmFQN(fgFQN, actualFgFQN, actualSmFQN)) {
        core::Map< core::String, std::shared_ptr< StateMachine > >::iterator const smIt{sms_.find(actualSmFQN)};
        if (sms_.end() != smIt) {
            promise.set_value(smIt->second->GetCurrentFGState(actualFgFQN));
        } else {
            promise.SetError(SMErrc::kSMNotFound);
            log_.LogError() << "StateMachineManager::GetFGState(), invalid smFQN:" << actualSmFQN.c_str();
        }
    } else {
        promise.SetError(SMErrc::kFGNameIllegal);
        log_.LogError() << "StateMachineManager::GetFGState(), invalid fgFQN:" << fgFQN.c_str();
    }
}

/// @brief Get the current state of the specified state machine
/// @param smFQN State machine name
/// @param promise Used to asynchronously set the execution result
void StateMachineManager::GetSMState(core::String const &smFQN, core::Promise< core::String > &&promise) noexcept
{
    log_.LogInfo() << "StateMachineManager::GetSMState(), smFQN:" << smFQN.c_str();
    core::String const actualSmFQN{_getSmFQN(smFQN)};
    core::Map< core::String, std::shared_ptr< StateMachine > >::iterator const smIt{sms_.find(actualSmFQN)};
    if (sms_.end() != smIt) {
        promise.set_value(smIt->second->GetCurrentNotifierSMState());
    } else {
        promise.SetError(SMErrc::kSMNotFound);
        log_.LogError() << "StateMachineManager::GetSMState(), invalid actualSmFQN:" << actualSmFQN.c_str();
    }
}

/// @brief Check whether shutdown is allowed
/// @return Whether shutdown is allowed
core::Result< void > StateMachineManager::CheckCanShutdown(core::StringView const nextState,
                                                           FGStateChangeSource const source) const noexcept
{
    bool const bShutdownStateIsNextState{common::GetkShutdownState() == nextState};
    bool const bRestartStateIsNextState{common::GetkRestartState() == nextState};
    log_.LogInfo() << "StateMachineManager::CheckCanShutdown(), bShutdownStateIsNextState:" << bShutdownStateIsNextState
                   << "bRestartStateIsNextState:" << bRestartStateIsNextState << "source:" << ToString(source);
    if ((bShutdownStateIsNextState && (FGStateChangeSource::kSHELL != source))
        || (bRestartStateIsNextState && (FGStateChangeSource::kUCM != source))) {
        log_.LogDebug() << "StateMachineManager::CheckCanShutdown(), updateSessionActive_:" << updateSessionActive_
                        << "updateSessionStartTime_:" << _timeStampSecondToStr(updateSessionStartTime_);

        // There is an update session
        if (updateSessionActive_) {
            // Current time
            std::int64_t const timeNow{std::time(nullptr)};
            struct tm result
            {
            };
            std::ignore = localtime_r(&timeNow, &result);

            // Determine whether the difference between the current time and the update start time exceeds the set value
            std::int64_t const delta{timeNow - updateSessionStartTime_};
            log_.LogDebug() << "StateMachineManager::CheckCanShutdown(), timeNow:" << _timeStampSecondToStr(timeNow)
                            << "and delta:" << delta;
            if (delta > shutdownPreventionTimeout_) {  // Timeout, shutdown allowed
                return core::Result< void >::FromValue();
            }
            // Not yet timed out, shutdown not allowed
            return core::Result< void >::FromError(SMErrc::kUpdateSessionIsActive);
        }  // No update session, shutdown allowed
        return core::Result< void >::FromValue();
    }
    return core::Result< void >::FromValue();
}

/// @brief Check whether a function group is in a deactivated state
/// @param fgFQN Function group full name
/// @return true - in a deactivated state
/// @return false - in an activated state
bool StateMachineManager::_checkFGIsDeactivated(core::String const &fgFQN) const noexcept
{
    log_.LogInfo() << "StateMachineManager::_checkFGIsDeactivated(), fgFQN:" << fgFQN.c_str();
    std::vector< ara::sm::config::FGInfo >::const_iterator const fgIt{
        std::find_if(fgInfos_.begin(), fgInfos_.end(),
                     [fgFQN](config::FGInfo const &info) noexcept { return fgFQN == info.fgFQN; })};
    if (fgInfos_.end() == fgIt) {
        log_.LogDebug() << "StateMachineManager::_checkFGIsDeactivated(), invalid fgFQN:" << fgFQN.c_str();
        return true;
    }
    if (fgIt->isDeactivated) {
        log_.LogDebug() << "StateMachineManager::_checkFGIsDeactivated(), deactive fgFQN:" << fgFQN.c_str();
        return true;
    }
    return false;
}

/// @brief Set the global configuration instance
/// @param globalConfigInstance Global configuration instance
void StateMachineManager::SetGlobalConfigInstance(
    std::shared_ptr< config::GlobalConfig > const &globalConfigInstance) noexcept
{
    log_.LogInfo() << "StateMachineManager::SetGlobalConfigInstance()";
    globalConfigInstance_ = globalConfigInstance;
}

/// @brief Start the state machine
/// @param smFQN State machine full name
/// @param smState State machine state
/// @return Future object
core::Future< void > StateMachineManager::StartStateMachine(core::String const &smFQN,
                                                            core::String const &smState) noexcept
{
    log_.LogInfo() << "StateMachineManager::StartStateMachine(), smFQN:" << smFQN.c_str()
                   << "smState:" << smState.c_str();
    std::shared_ptr< StateMachine > pStateMachine{sms_[smFQN]};
    if (pStateMachine) {
        return pStateMachine->Start(FGStateChangeSource::kInternal, smState);
    }
    core::Promise< void > promise;
    promise.SetError(SMErrc::kRejected);
    log_.LogError() << "StateMachineManager::StartStateMachine(), invalid smFQN:" << smFQN.c_str()
                    << "smState:" << smState.c_str();
    return promise.get_future();
}

/// @brief Stop the state machine
/// @param smFQN State machine full name
/// @return Future object
core::Future< void > StateMachineManager::StopStateMachine(core::String const &smFQN) noexcept
{
    log_.LogInfo() << "StateMachineManager::StopStateMachine(), smFQN:" << smFQN.c_str();
    std::shared_ptr< StateMachine > pStateMachine{sms_[smFQN]};
    if (pStateMachine) {
        return pStateMachine->Stop();
    }
    core::Promise< void > promise;
    promise.SetError(SMErrc::kRejected);
    log_.LogError() << "StateMachineManager::StopStateMachine(), invalid smFQN:" << smFQN.c_str();
    return promise.get_future();
}
#ifdef ARA_WITH_DIAG
/// @brief Handle diagnostic request reset
/// @param resetType Reset type
/// @param targetAddressTypeStr Target address type
/// @param targetAddress Target address
/// @param promise Used to asynchronously set the execution result
void StateMachineManager::HandleDiagRequestReset(common::DiagResetRequestInternalType const resetType,
                                                 core::String const &targetAddressTypeStr,
                                                 int32_t const targetAddress,
                                                 core::Promise< void > &&promise) noexcept
{
    log_.LogInfo() << "StateMachineManager::HandleDiagRequestReset(), resetType:"
                   << common::ResetRequestTypeToString(resetType).c_str() << "targetAddress:" << targetAddress
                   << "targetAddressTypeStr:" << targetAddressTypeStr.c_str();
    if (common::DiagResetRequestInternalType::kHardReset == resetType) {
        ResetMachine(std::move(promise), FGStateChangeSource::kDM);
    } else if (common::DiagResetRequestInternalType::kKeyOffOnReset == resetType) {
        core::Vector< core::String > resetFgs;
        if (-1 != targetAddress) {
            if (common::GetkPhysicalAddress() == targetAddressTypeStr) {
                ara::core::Map< int32_t, ara::sm::common::UcmFunctionGroupListInternal > physicalAddr2Fgs{
                    globalConfigInstance_->GetPhysicalAddr2FGFQNs()};
                resetFgs = physicalAddr2Fgs[targetAddress];
            } else if (common::GetkFunctionalAddress() == targetAddressTypeStr) {
                ara::core::Map< int32_t, ara::sm::common::UcmFunctionGroupListInternal > functionalAddr2Fgs{
                    globalConfigInstance_->GetFunctionalAddr2FGFQNs()};
                resetFgs = functionalAddr2Fgs[targetAddress];
            } else {
                log_.LogError() << "StateMachineManager::HandleDiagRequestReset(), invalid resetType:"
                                << common::ResetRequestTypeToString(resetType).c_str()
                                << "targetAddressTypeStr:" << targetAddressTypeStr.c_str();
                promise.SetError(SMErrc::kRejected);
                return;
            }
        } else {
            log_.LogError() << "StateMachineManager::HandleDiagRequestReset(), invalid address, resetType:"
                            << common::ResetRequestTypeToString(resetType).c_str();
            promise.SetError(SMErrc::kRejected);
            return;
        }
        std::shared_ptr< ara::core::Promise< void > > const finalPromise{
            std::make_shared< core::Promise< void > >(std::move(promise))};
        uint32_t const fgTotalCount{static_cast< uint32_t >(resetFgs.size())};
        if (0UL == fgTotalCount) {
            finalPromise->set_value();
            log_.LogWarn() << "StateMachineManager::HandleDiagRequestReset(), fgTotalCount == 0";
            return;
        }
        std::shared_ptr< uint32_t > fgResponseCount{std::make_shared< uint32_t >(0UL)};
        std::shared_ptr< bool > const fail{std::make_shared< bool >(false)};
        for (const auto &fgFQN : resetFgs) {
            ara::core::String const originFgState{_getFGState(fgFQN)};
            log_.LogDebug() << "StateMachineManager::HandleDiagRequestReset(), originFgState:" << originFgState.c_str();
            if (originFgState.empty()) {
                log_.LogError() << "StateMachineManager::HandleDiagRequestReset(), invalid originFgState for fgFQN:"
                                << fgFQN.c_str();
                *fgResponseCount = *fgResponseCount + 1U;
            } else {
                core::Promise< void > offPromise;
                std::shared_ptr< core::Future< void > > offFuture{
                    std::make_shared< core::Future< void > >(offPromise.get_future())};
                ChangeFGState(fgFQN, common::GetkOffState(), FGStateChangeSource::kDM, std::move(offPromise));
                std::ignore
                    = offFuture->then([offFuture, fgFQN, originFgState, fgResponseCount, fail, finalPromise,
                                       fgTotalCount, this]() mutable {
                          ara::core::Result< void > const offRes{offFuture->GetResult()};
                          if (!offRes.HasValue()) {
                              log_.LogError()
                                  << "StateMachineManager::HandleDiagRequestReset(), change to Off state error for"
                                  << fgFQN.c_str() << "reason:" << offRes.Error().Message().data();
                          } else {
                              log_.LogDebug()
                                  << "StateMachineManager::HandleDiagRequestReset(), change to Off state success for"
                                  << fgFQN.c_str();
                          }
                          offFuture = nullptr;
                          core::Promise< void > originPromise;
                          std::shared_ptr< core::Future< void > > originFuture{
                              std::make_shared< core::Future< void > >(originPromise.get_future())};
                          log_.LogDebug() << "StateMachineManager::HandleDiagRequestReset(), change to originFgState"
                                          << originFgState.c_str() << "for" << fgFQN.c_str();
                          ChangeFGState(fgFQN, originFgState, FGStateChangeSource::kDM, std::move(originPromise));
                          std::ignore = originFuture->then([offFuture, fgFQN, originFgState, fgResponseCount,
                                                            originFuture, fail, finalPromise, fgTotalCount,
                                                            this]() mutable {
                              *fgResponseCount = *fgResponseCount + 1U;
                              ara::core::Result< void > const res{originFuture->GetResult()};
                              if (res.HasValue()) {
                                  log_.LogDebug()
                                      << "StateMachineManager::HandleDiagRequestReset(), change to originFgState"
                                      << originFgState.c_str() << "success for" << fgFQN.c_str()
                                      << "fgResponseCount:" << *fgResponseCount;
                              } else {
                                  if (!*fail) {
                                      *fail = true;
                                      finalPromise->SetError(res.Error());
                                  }
                                  log_.LogError()
                                      << "StateMachineManager::HandleDiagRequestReset(), change to originFgState"
                                      << originFgState.c_str() << "error for" << fgFQN.c_str()
                                      << "fgResponseCount:" << *fgResponseCount;
                              }
                              if (fgTotalCount == *fgResponseCount) {
                                  if (!(*fail)) {
                                      finalPromise->set_value();
                                      log_.LogDebug() << "StateMachineManager::HandleDiagRequestReset(), all success";
                                  }
                              }
                              originFuture = nullptr;
                              offFuture    = nullptr;
                          });
                      });
            }
        }
    } else {
    }
}
#endif
void StateMachineManager::GetAllFunctionGroupsInfo(core::Promise< core::String > &&promise)
{
    ara::core::Vector< common::FunctionGroupsInfoSM > infos;
    if (!started_) {
        log_.LogError() << "StateMachineManager::GetAllFunctionGroupsInfo(), not ready";
        promise.SetError(SMErrc::kRejected);
    } else {
        for (std::pair< core::String const, std::shared_ptr< StateMachine > > const &pair : sms_) {
            ara::core::String smFQN{pair.first};
            log_.LogDebug() << "StateMachineManager::GetAllFunctionGroupsInfo(), smFQN:" << smFQN;
            std::shared_ptr< StateMachine > stateMachine{pair.second};
            common::FunctionGroupsInfoSM functionGroupsInfo;
            if (stateMachine->IsUserDefined()) {
                functionGroupsInfo.smFQN           = std::move(smFQN);
                functionGroupsInfo.smStates        = stateMachine->GetSmStates();
                functionGroupsInfo.isManagedBySm   = true;
                functionGroupsInfo.transitionTable = stateMachine->GetTransitionTable();
            } else {
                functionGroupsInfo.isManagedBySm = false;
            }
            ara::core::Vector< core::String > managedFgs{stateMachine->GetManagedFGs()};
            for (core::String const &fgFQN : managedFgs) {
                log_.LogDebug() << "StateMachineManager::GetAllFunctionGroupsInfo(), managedFg:" << fgFQN;
                core::Vector< config::FGInfo >::iterator const it{
                    std::find_if(fgInfos_.begin(), fgInfos_.end(),
                                 [&fgFQN](config::FGInfo const &info) noexcept { return info.fgFQN == fgFQN; })};
                if (fgInfos_.end() != it) {
                    functionGroupsInfo.fgInfos.push_back(common::FunctionGroupInfoSM{fgFQN, it->fgStates});
                }
            }
            infos.push_back(std::move(functionGroupsInfo));
        }
    }
    promise.set_value(common::FunctionGroupsInfoToStr(infos));
}

#ifdef ARA_WITH_PER
/// @brief Save the update session state
/// @param updateSessionActive
/// @param updateSessionStartTime
void StateMachineManager::_saveUpdateSessionStatus(bool const updateSessionActive,
                                                   std::int64_t const updateSessionStartTime) noexcept
{
    log_.LogInfo() << "StateMachineManager::_saveUpdateSessionStatus(), updateSessionActive:" << updateSessionActive
                   << "updateSessionStartTime:" << _timeStampSecondToStr(updateSessionStartTime);

    // Save the update session state and start time
    core::Array< std::int64_t, TWO_UL > status{};
    status[0U] = static_cast< uint8_t >(updateSessionActive);
    status[1U] = updateSessionStartTime;
    const bool ret{updateSessionStatusStorage_.Store< core::Array< std::int64_t, TWO_UL > >(
        common::GetkUpdateSessionStatusKey(), status)};
    if (!ret) {
        log_.LogError() << "StateMachineManager::_saveUpdateSessionStatus(), fail to store UpdateSessionStatus"
                        << common::GetkUpdateSessionStatusKey();
    }
}

/// @brief Save the list of function groups to be updated
/// @param updatingFGSet Information of function groups to be updated
void StateMachineManager::_saveUpdatingFGSet(std::set< core::String > const &updatingFGSet) noexcept
{
    log_.LogInfo() << "StateMachineManager::_saveUpdatingFGSet(), updatingFGSet:"
                   << common::ConcatenateStrings(updatingFGSet).c_str();

    // Save the set of function groups being updated
    bool const ret{
        updateSessionStatusStorage_.Store< std::set< core::String > >(common::GetkUpdatingFgSetKey(), updatingFGSet)};
    if (!ret) {
        log_.LogError() << "StateMachineManager::_saveUpdatingFGSet(), fail to store UpdatingFGSet"
                        << common::GetkUpdatingFgSetKey();
    }
}
#endif
/// @brief Check whether the involved function group or state machine is in an update state
/// @param requestFQN Full name of the function group or state machine
/// @return true - updating
/// @return false - not updating
bool StateMachineManager::_checkIsUpdating(core::String const &requestFQN) noexcept
{
    log_.LogInfo() << "StateMachineManager::_checkIsUpdating(), requestFQN:" << requestFQN.c_str();
    std::set< core::String >::iterator const iter1{updatingFGSet_.find(requestFQN)};
    if (updatingFGSet_.end()
        != iter1) {  // First consider requestFQN as a function group FQN, check if it is a function group being updated
        return true;
    }  // Then consider requestFQN as a state machine FQN, check if the function groups managed by this state machine are updating
    core::Map< core::String, std::shared_ptr< StateMachine > >::iterator const smIt{sms_.find(requestFQN)};
    if (sms_.end() != smIt) {
        core::Vector< core::String > const managedFgs{smIt->second->GetManagedFGs()};
        for (core::String const &fgFQN : managedFgs) {
            std::set< core::String >::iterator const iter2{updatingFGSet_.find(fgFQN)};
            if (updatingFGSet_.end() != iter2) {
                return true;
            }
        }
    }
    return false;
}

/// @brief Get the function group FQN and state machine FQN based on the function group name
/// @param fgName Function group name, it may be the function group FQN itself
/// @param fgFQN Function group FQN
/// @param smFQN State machine FQN
/// @return true - retrieval successful; false - retrieval failed
bool StateMachineManager::_getFgAndSmFQN(core::String const &fgName, core::String &fgFQN, core::String &smFQN) noexcept
{
    std::ignore = fgFQN;
    std::ignore = smFQN;
    log_.LogInfo() << "StateMachineManager::_getFgAndSmFQN(), fgName:" << fgName.c_str();
    core::Map< core::String, core::String >::iterator const smFQNIt{fg2sm_.find(fgName)};
    if (fg2sm_.end() != smFQNIt) {
        fgFQN = fgName;
        smFQN = smFQNIt->second;
        return true;
    }
    for (const auto &pair : fg2sm_) {
        ara::core::String const fgFQNInMap{pair.first};
        std::size_t const pos{fgFQNInMap.rfind(fgName)};
        if ((std::string::npos != pos)
            && ((pos + fgName.size() == fgFQNInMap.size()) || (pos + fgName.size() + 1U == fgFQNInMap.size()))) {
            fgFQN = fgFQNInMap;
            smFQN = pair.second;
            return true;
        }
    }
    return false;
}

/// @brief Get the state machine FQN based on the state machine name
/// @param smName State machine name, it may be the state machine FQN itself
/// @return State machine FQN
core::String StateMachineManager::_getSmFQN(core::String const &smName) noexcept
{
    log_.LogInfo() << "StateMachineManager::_getSmFQN(), smName:" << smName.c_str();
    core::String smFQN{""};
    for (const auto &pair : sms_) {
        ara::core::String const smFQNInMap{pair.first};
        if (smFQNInMap == smName) {
            return smName;
        }
        std::size_t const pos{smFQNInMap.rfind(smName)};
        if ((std::string::npos != pos)
            && ((pos + smName.size() == smFQNInMap.size()) || (pos + smName.size() + 1U == smFQNInMap.size()))) {
            smFQN = smFQNInMap;
            break;
        }
    }
    return smFQN;
}

/// @brief Convert a timestamp in seconds to a string in the format YYYY-MM-DD HH:MM:SS
/// @param timeStamp Function group name
/// @return String in the format YYYY-MM-DD HH:MM:SS
core::String StateMachineManager::_timeStampSecondToStr(std::int64_t const &timeStamp) const noexcept
{
    std::ignore = updateSessionStartTime_;
    // Convert to time_t type
    std::time_t const timeStampT{static_cast< time_t >(timeStamp)};
    // Convert to tm structure
    struct tm result
    {
    };
    std::tm *const timeStampTm{localtime_r(&timeStampT, &result)};
    // Format to string
    std::stringstream ss;
    ss << std::put_time(timeStampTm, "%Y-%m-%d %H:%M:%S");
    return ara::core::String(ss.str());
}

/// @brief Get the current state of the specified function group
/// @param fgFQN Function group name
/// @return Current state fgState
core::String StateMachineManager::_getFGState(core::String const &fgFQN) noexcept
{
    log_.LogInfo() << "StateMachineManager::_getFGState(), fgFQN:" << fgFQN.c_str();
    core::String fgState{""};
    core::Map< core::String, core::String >::iterator const smFQNIt{fg2sm_.find(fgFQN)};
    if (fg2sm_.end() != smFQNIt) {
        core::String const smFQN{smFQNIt->second};
        core::Map< core::String, std::shared_ptr< StateMachine > >::iterator const smIt{sms_.find(smFQN)};
        if (sms_.end() != smIt) {
            fgState = smIt->second->GetCurrentFGState(fgFQN);
        } else {
            log_.LogError() << "StateMachineManager::_getFGState(), invalid smFQN:" << smFQN.c_str();
        }
    } else {
        log_.LogError() << "StateMachineManager::_getFGState(), invalid fgFQN:" << fgFQN.c_str();
    }
    return fgState;
}

core::String StateMachineManager::getChangedNmHandle_(uint64_t const &nmHandleId) noexcept
{
    const auto nmHandleIter{nmId2nmHandle_.find(nmHandleId)};
    if (nmId2nmHandle_.end() != nmHandleIter) {
        return nmHandleIter->second;
    }
    return core::String{""};
}

/// @brief Asynchronously switch a group of function groups to the same state
/// @param fgList List of function groups
/// @param fgState Function group state
/// @param source Source of the transition request
/// @param err Error code
/// @param promise Carries the switching result
void StateMachineManager::_asyncChangeFunctionGroups(common::UcmFunctionGroupListInternal const &fgList,
                                                     core::String const &fgState,
                                                     FGStateChangeSource const source,
                                                     SMErrc const err,
                                                     core::Promise< void > &&promise) noexcept
{
    log_.LogInfo() << "StateMachineManager::_asyncChangeFunctionGroups(), fgList:"
                   << common::ConcatenateStrings(fgList).c_str() << "fgState:" << fgState.c_str()
                   << "source:" << ToString(source).c_str();
    std::shared_ptr< core::Promise< void > > const newPromise{
        std::make_shared< core::Promise< void > >(std::move(promise))};
    std::shared_ptr< uint32_t > const fgResponseCount{std::make_shared< uint32_t >(0UL)};
    uint32_t const fgTotalCount{static_cast< uint32_t >(fgList.size())};
    if (0UL == fgTotalCount) {
        newPromise->set_value();
        log_.LogWarn() << "StateMachineManager::_asyncChangeFunctionGroups(), fgTotalCount == 0";
        return;
    }
    std::shared_ptr< bool > const fail{std::make_shared< bool >(false)};
    for (auto const &fgFQN : fgList) {
        core::Promise< void > callPromise;
        std::shared_ptr< core::Future< void > > future{
            std::make_shared< core::Future< void > >(callPromise.get_future())};
        log_.LogDebug() << "StateMachineManager::_asyncChangeFunctionGroups(), try to change" << fgFQN.c_str()
                        << "state to" << fgState.c_str();
        ChangeFGState(fgFQN, fgState, source, std::move(callPromise));
        std::ignore
            = future->then([this, future, fgTotalCount, fgResponseCount, newPromise, fail, fgFQNInFuture = fgFQN,
                            fgStateInFuture = fgState, source, err]() mutable {
                  *fgResponseCount = *fgResponseCount + 1U;
                  ara::core::Result< void > const res{future->GetResult()};
                  if (!res.HasValue()) {
#ifdef ARA_WITH_UCM
                      // Here, SMErrc::kPrepareFailed is temporarily used to indicate being in a PrepareUpdate request
                      if ((res.CheckError(SMErrc::kFGNameIllegal) || res.CheckError(SMErrc::kAlreadyInState))
                          && ((err == SMErrc::kPrepareFailed) || (err == SMErrc::kRollbackFailed))) {
                          log_.LogDebug() << "StateMachineManager::_asyncChangeFunctionGroups(), we get a Error:"
                                          << res.Error().Message()
                                          << "but we can skip it because in PrepareUpdate or PrepareRollback request.";
                      } else {
#endif
                          if (!*fail) {
                              *fail = true;
                              newPromise->SetError(err);
                          }
                          log_.LogError()
                              << "StateMachineManager::_asyncChangeFunctionGroups(), ChangeFGState error for fgFQN:"
                              << fgFQNInFuture.c_str() << "fgState:" << fgStateInFuture.c_str()
                              << "source:" << ToString(source).c_str();
#ifdef ARA_WITH_UCM
                      }
#endif
                  }
                  if (fgTotalCount == *fgResponseCount) {
                      if (!(*fail)) {
                          newPromise->set_value();
                      }
                  }
                  future = nullptr;
              });
    }
}
}  // namespace state_machine_management
}  // namespace sm
}  // namespace ara