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
/// @file       event_manager.cpp
/// @brief      Event management class
/// @details
/// @date       2024-05-03
/// @author     lianglongxiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/StateManagement/EventManagement
/// @interface_level=module
/// @trace_id_sr=SR_SM_01001, SR_SM_01002, SR_SM_01003, SR_SM_02001, SR_SM_02002, SR_SM_03002, SR_SM_04001, SR_SM_04005, SR_SM_04006, SR_SM_04007, SR_SM_04008,
/// SR_SM_04009,
/// @unit_name=EventManager
/// @unit_description=Event management class
/// @endcode
///
/// ================================================================

#include "event_manager.h"

#include <ara/exec/exec_error_domain.h>
#include <isoft/naicpp/evloop.h>
#include <isoft/naicpp/global_evloop.h>

#include <memory>

#include "helper.h"

namespace ara {
namespace sm {
namespace event_management {

/// @brief Constructor function
EventManager::EventManager() noexcept
    : log_{log::CreateLogger((core::StringView{"#EVM"}), (core::StringView{"Event Management context"}))}
    , commManagers_{}
    , fgStateCommManager_{}
    , shellCommManager_{}
    , emCommManager_
{
}
#ifdef ARA_WITH_NM
, nmCommManager_ {}
#endif
#ifdef ARA_WITH_DIAG
, dmCommManager_ {}
#endif
#ifdef ARA_WITH_PHM
, phmCommManager_ {}
#endif
#ifdef ARA_WITH_UCM
, ucmCommManager_ {}
#endif
#ifdef ARA_ENABLE_SM_ECU_STATE
, ecuStateCommManager_ {}
#endif
, fgStateNotifyCommManager_{}, smStateCommManager_{}, stateMachineManager_{}, globalConfigInstance_{},
    terminatingHandler_{[this]() noexcept { Stop(); }}, promise_
{
}
#ifdef ARA_TEST_SHELL
, innerTimer_ { nullptr }
#endif
{
    log_.LogInfo() << "EventManager::EventManager()";
    globalConfigInstance_ = std::make_shared< config::GlobalConfig >();
    fgStateCommManager_   = std::make_shared< fg_state_comm::FGStateCommManager >("FGStateCommManager");
    fgStateCommManager_->SetGlobalConfigInstance(globalConfigInstance_);
    commManagers_.push_back(fgStateCommManager_);

    shellCommManager_ = std::make_shared< shell_comm::ShellCommManager >("ShellCommManager");
    commManagers_.push_back(shellCommManager_);

    emCommManager_ = std::make_shared< em_comm::EMCommManager >("EMCommManager");
    commManagers_.push_back(emCommManager_);

#ifdef ARA_WITH_NM
    nmCommManager_ = std::make_shared< nm_comm::NMCommManager >("NMCommManager");
    nmCommManager_->SetGlobalConfigInstance(globalConfigInstance_);
    commManagers_.push_back(nmCommManager_);
#endif

#ifdef ARA_WITH_DIAG
    dmCommManager_ = std::make_shared< dm_comm::DMCommManager >("DMCommManager");
    commManagers_.push_back(dmCommManager_);
#endif

#ifdef ARA_WITH_PHM
    phmCommManager_ = std::make_shared< phm_comm::PHMCommManager >("PHMCommManager");
    commManagers_.push_back(phmCommManager_);
#endif

#ifdef ARA_WITH_UCM
    ucmCommManager_ = std::make_shared< ucm_comm::UCMCommManager >("UCMCommManager");
    commManagers_.push_back(ucmCommManager_);
#endif
#ifdef ARA_ENABLE_SM_ECU_STATE
    ecuStateCommManager_ = std::make_shared< ecu_state_comm::EcuStateCommManager >("EcuStateCommManager");
    commManagers_.push_back(ecuStateCommManager_);
#endif

    fgStateNotifyCommManager_
        = std::make_shared< fg_state_notify_comm::FGStateNotifyCommManager >("FGStateNotifyCommManager");
    commManagers_.push_back(fgStateNotifyCommManager_);

    smStateCommManager_ = std::make_shared< sm_state_comm::SMStateCommManager >("SMStateCommManager");
    smStateCommManager_->SetGlobalConfigInstance(globalConfigInstance_);
    commManagers_.push_back(smStateCommManager_);

    stateMachineManager_ = std::make_shared< state_machine_management::StateMachineManager >();
    stateMachineManager_->SetGlobalConfigInstance(globalConfigInstance_);
}

/// @brief Destructor function
EventManager::~EventManager() noexcept { log_.LogInfo() << "EventManager::~EventManager()"; }

/// @brief Initialize
/// @return true - success
/// @return false - failure
bool EventManager::Init() noexcept
{
    log_.LogInfo() << "EventManager::Init()";
    std::function< void(common::Event &&) > const appendEventHandler{
        [this](common::Event &&event) noexcept { AppendEvent(std::move(event)); }};
    std::function< core::Result< exec::ExecutionErrorEvent >(core::String const &) > const getExecutionErrorHandler{
        [this](core::String const &fgFQN) noexcept { return emCommManager_->GetExecutionError(fgFQN); }};
    bool const res{globalConfigInstance_->Load()};
    if (!res) {
        log_.LogError() << "EventManager::Init(), globalConfigInstance_ Load Error";
        return false;
    }

    for (std::shared_ptr< common::ICommunicationManager > const &commManager : commManagers_) {
        if (!commManager->Init()) {
            if (commManager->Name() == "ShellCommManager") {
                log_.LogError() << "EventManager::Init(), commManager" << commManager->Name().c_str() << "Init Error";
                return false;
            }
            log_.LogWarn() << "EventManager::Init(), commManager" << commManager->Name().c_str() << "Init Error";
        }
        commManager->RegisterAppendEventHandler(appendEventHandler);
    }
    stateMachineManager_->RegisterGetExecutionErrorHandler(getExecutionErrorHandler);
    stateMachineManager_->RegisterAppendEventHandler(appendEventHandler);
    std::ignore = stateMachineManager_->Init();
    return true;
}

/// @brief Start event processing
/// @return true - success
/// @return false - failure
bool EventManager::Run() noexcept
{
    log_.LogInfo() << "EventManager::Run()";
    if (!started_) {
#ifdef ARA_WITH_NM
        if (!nmCommManager_->Start()) {
            return false;
        }
#endif
        if (0 != terminatingHandler_.Register()) {
            log_.LogWarn() << "EventManager::Run(), fail to Register TerminatingHandler.";
        }
        started_ = true;
    }
    bool const res{_getInitialMachineStateTransitionResult()};
    std::ignore = stateMachineManager_->Start();
    for (std::shared_ptr< common::ICommunicationManager > const &commManager : commManagers_) {
        if (!commManager->Start()) {
            if (commManager->Name() == "ShellCommManager") {
                log_.LogError() << "EventManager::Run(), commManager" << commManager->Name().c_str() << "Start Error";
                return false;
            }
            log_.LogWarn() << "EventManager::Init(), commManager" << commManager->Name().c_str() << "Start Error";
        }
    }
    if (res) {
        log_.LogInfo() << "EventManager::Run(), _getInitialMachineStateTransitionResult ok";
    } else {
        log_.LogError() << "EventManager::Run(), _getInitialMachineStateTransitionResult false";
        core::String const machineFgFQN{globalConfigInstance_->GetMachineFgFQN()};
        ara::core::Result< ara::exec::ExecutionErrorEvent > const execRes{GetExecutionError(machineFgFQN)};
        if (execRes) {
            // Get execution error
            exec::ExecutionErrorEvent const &executionErrorEvent{execRes.Value()};
            log_.LogWarn() << "EventManager::Run(), try to Exec ErrorReact for executionError:"
                           << common::ErrorEventToString(executionErrorEvent).data()
                           << "for machineFg:" << machineFgFQN.c_str();
            core::Promise< void > promise;
            stateMachineManager_->ErrorRecovery(executionErrorEvent, std::move(promise));
        } else {
            log_.LogError() << "EventManager::Run(), GetExecutionError for MachineFG";
        }
    }
    ara::core::Future< void, ara::core::ErrorCode > future{promise_.get_future()};
    std::ignore = future.GetResult();
    return true;
}

/// @brief Stop event processing
void EventManager::Stop() noexcept
{
    log_.LogInfo() << "EventManager::Stop()";
    if (started_) {
        for (std::shared_ptr< common::ICommunicationManager > const &commManager : commManagers_) {
            commManager->Stop();
        }
        started_ = false;
        promise_.set_value();
    }
}

/// @brief Add event request
/// @param event  Event request

void EventManager::AppendEvent(common::Event &&event) noexcept
{
    log_.LogInfo() << "EventManager::AppendEvent(), push event to evloop, event type:"
                   << common::EventTypeToString(event.type).data();
    switch (event.type) {
        case common::EventType::kOutNMSetNetworkState: {
            std::unique_ptr< common::NetworkStateInfo > const info{
                static_cast< common::NetworkStateInfo * >(event.data)};
#ifdef ARA_WITH_NM
            event.requestPromiseWithFuture.set_value(
                nmCommManager_->AsyncSetNetworkState(info->nmHandleName, info->state));
#else
            std::ignore = info;
            core::Promise< void > promise;
            promise.SetError(SMErrc::kRejected);
            event.requestPromiseWithFuture.set_value(promise.get_future());
            log_.LogWarn() << "EventManager::AppendEvent(), ARA_WITH_NM is not defined";
#endif
            break;
        }
        case common::EventType::kOutEMSetFGState: {
            std::unique_ptr< common::FGStateInfo > const info{static_cast< common::FGStateInfo * >(event.data)};
            event.requestPromiseWithFuture.set_value(emCommManager_->AsyncSetFGState(info->fgFQN, info->fgState));
            break;
        }
        case common::EventType::kOutNotifyFGStateChange: {
            std::unique_ptr< common::FGStateInfo > const info{static_cast< common::FGStateInfo * >(event.data)};
            fgStateCommManager_->NotifyFGStateUpdate(info->fgFQN, info->fgState);
            fgStateNotifyCommManager_->NotifyFGStateUpdate(info->fgFQN, info->fgState);
            break;
        }
        case common::EventType::kOutNotifySMStateChange: {
            std::unique_ptr< common::SMStateInfo > const info{static_cast< common::SMStateInfo * >(event.data)};
            smStateCommManager_->NotifySMStateUpdate(info->smFQN, info->smState);
            shellCommManager_->NotifySMStateUpdate(info->smFQN, info->smState);
            break;
        }
        case common::EventType::kOutNotifyEcuStateChange: {
#ifdef ARA_ENABLE_SM_ECU_STATE
            std::unique_ptr< core::String > const ecuState{static_cast< core::String * >(event.data)};
            ecuStateCommManager_->NotifyEcuStateUpdate(*ecuState);
#endif
            break;
        }
        default: {
            std::shared_ptr< common::Event > ev = std::make_shared< common::Event >(std::move(event));
            std::ignore                         = isoft::naicpp::GlobalGeneralEvLoop::Get()->Exec(
                [this, ev]() mutable noexcept { _appendEventForEVLoop(std::move(*ev)); });
            break;
        }
    }
}

/// @brief Add event request
/// @param event  Event request
void EventManager::_appendEventForEVLoop(common::Event &&event) noexcept
{
    log_.LogInfo() << "EventManager::_appendEventForEVLoop(), event type:"
                   << common::EventTypeToString(event.type).data();
    switch (event.type) {
        case common::EventType::kInShellSetFGState: {
            std::unique_ptr< common::FGStateInfo > const info{static_cast< common::FGStateInfo * >(event.data)};
            stateMachineManager_->ChangeFGState(info->fgFQN, info->fgState,
                                                state_machine_management::FGStateChangeSource::kSHELL,
                                                std::move(event.requestPromise));
            break;
        }
        case common::EventType::kInShellGetFGState: {
            std::unique_ptr< core::String > const fgFQN{static_cast< core::String * >(event.data)};
            stateMachineManager_->GetFGState(*fgFQN, std::move(event.requestPromiseWithStr));
            break;
        }
        case common::EventType::kInShellSetSMState: {
            std::unique_ptr< common::SMStateRequestInfo > const info{
                static_cast< common::SMStateRequestInfo * >(event.data)};
            stateMachineManager_->RequestState(info->smFQN, info->transitionRequest, std::move(event.requestPromise));
            break;
        }
        case common::EventType::kInShellGetSMState: {
            std::unique_ptr< core::String > const smFQN{static_cast< core::String * >(event.data)};
            stateMachineManager_->GetSMState(*smFQN, std::move(event.requestPromiseWithStr));
            break;
        }
#ifdef ARA_WITH_UCM
        case common::EventType::kInUCMStartUpdateSession: {
            stateMachineManager_->StartUpdateSession(std::move(event.requestPromise));
            break;
        }
        case common::EventType::kInUCMStopUpdateSession: {
            stateMachineManager_->StopUpdateSession(std::move(event.requestPromise));
            break;
        }
        case common::EventType::kInUCMResetMachine: {
            stateMachineManager_->ResetMachine(std::move(event.requestPromise),
                                               state_machine_management::FGStateChangeSource::kUCM);
            break;
        }
        case common::EventType::kInUCMPrepareUpdate: {
            std::unique_ptr< common::FGListInfo > const fgListInfo{static_cast< common::FGListInfo * >(event.data)};
            stateMachineManager_->PrepareUpdate(fgListInfo->fgList, std::move(event.requestPromise));
            break;
        }
        case common::EventType::kInUCMVerifyUpdate: {
            std::unique_ptr< common::FGListInfo > const fgListInfo{static_cast< common::FGListInfo * >(event.data)};
            stateMachineManager_->VerifyUpdate(fgListInfo->fgList, std::move(event.requestPromise));
            break;
        }
        case common::EventType::kInUCMPrepareRollback: {
            std::unique_ptr< common::FGListInfo > const fgListInfo{static_cast< common::FGListInfo * >(event.data)};
            stateMachineManager_->PrepareRollback(fgListInfo->fgList, std::move(event.requestPromise));
            break;
        }
#endif
        case common::EventType::kInEMUndefinedStateCallback: {
            std::unique_ptr< exec::ExecutionErrorEvent > const info{
                static_cast< exec::ExecutionErrorEvent * >(event.data)};
            stateMachineManager_->ErrorRecovery(*info, std::move(event.requestPromise));
            break;
        }

        case common::EventType::kInPHMSupervisionRecoveryNotification: {
            std::unique_ptr< common::PHMSupervisionRecoveryNotificationInfo > const info{
                static_cast< common::PHMSupervisionRecoveryNotificationInfo * >(event.data)};
            stateMachineManager_->ErrorRecovery(info->executionError, std::move(event.requestPromise));
            break;
        }
        case common::EventType::kInPHMHealthChannelRecoveryNotification: {
            // To be implemented by the user
            std::unique_ptr< common::PHMHealthChannelRecoveryNotificationInfo > const info{
                static_cast< common::PHMHealthChannelRecoveryNotificationInfo * >(event.data)};
            event.requestPromise.set_value();
            break;
        }

        case common::EventType::kInNMNetworkStateChange: {
            std::unique_ptr< common::NetworkStateInfo > const info{
                static_cast< common::NetworkStateInfo * >(event.data)};
            stateMachineManager_->NotifyNetworkStateChanged(info->nmHandleName, info->state);
            break;
        }

        case common::EventType::kInAASetFGState: {
            std::unique_ptr< common::FGStateInfo > const info{static_cast< common::FGStateInfo * >(event.data)};
            stateMachineManager_->ChangeFGState(info->fgFQN, info->fgState,
                                                state_machine_management::FGStateChangeSource::kAA,
                                                std::move(event.requestPromiseWithStr));
            break;
        }

        case common::EventType::kInAAGetFGState: {
            std::unique_ptr< core::String > const fgFQN{static_cast< core::String * >(event.data)};
            stateMachineManager_->GetFGState(*fgFQN, std::move(event.requestPromiseWithStr));
            break;
        }
        case common::EventType::kInGetAllFGState: {
            stateMachineManager_->GetAllFGStates(std::move(event.requestPromiseWithStr));
            break;
        }
        case common::EventType::kInSMCSetSMState: {
            std::unique_ptr< common::SMStateRequestInfo > const info{
                static_cast< common::SMStateRequestInfo * >(event.data)};
            stateMachineManager_->RequestState(info->smFQN, info->transitionRequest, std::move(event.requestPromise));
            break;
        }
        case common::EventType::kInGetSMState: {
            std::unique_ptr< core::String > const smFQN{static_cast< core::String * >(event.data)};
            stateMachineManager_->GetSMState(*smFQN, std::move(event.requestPromiseWithStr));
            break;
        }
        case common::EventType::kInDiagEnableRapidShutdown:
        case common::EventType::kInDiagExecuteReset: {
            // To be implemented by the user
            event.requestPromise.set_value();
            break;
        }
        case common::EventType::kInDiagRequestReset: {
#ifdef ARA_WITH_DIAG
            std::unique_ptr< common::DiagRequestResetInfo > const info{
                static_cast< common::DiagRequestResetInfo * >(event.data)};
            stateMachineManager_->HandleDiagRequestReset(info->resetType, info->addressType, info->address,
                                                         std::move(event.requestPromise));
#endif
            break;
        }
        case common::EventType::kInAASetEcuState: {
            // To be implemented by the user
            event.requestPromiseWithStr.set_value("");
#ifdef ARA_ENABLE_SM_ECU_STATE
            std::unique_ptr< core::String > const ecuState{static_cast< core::String * >(event.data)};
            ecuStateCommManager_->NotifyEcuStateUpdate(*ecuState);
#endif
            break;
        }
        case common::EventType::kInGetEcuState: {
            // To be implemented by the user
            event.requestPromiseWithStr.set_value("");
            break;
        }
        case common::EventType::kInShellGetAllFGInfos: {
#ifdef ARA_TEST_SHELL
            event.requestPromiseWithStr.set_value(",;; A,1,2,3;; .. , SM,4,5,6;; B,7,8,9;; .. 666,m,y;; 777,f,l;; , ");
            std::ignore = isoft::naicpp::GlobalGeneralEvLoop::Get()->MakeTimer(
                innerTimer_, 2 * 1000, [this]() mutable { shellCommManager_->NotifySMStateUpdate("aa", "bb"); });
#else
            stateMachineManager_->GetAllFunctionGroupsInfo(std::move(event.requestPromiseWithStr));
#endif
            break;
        }
        default: {
            break;
        }
    }
}  // namespace event_management

/// @brief Get the execution error related to the specified function group
/// @param fgName Function group name
/// @return Set result
core::Result< exec::ExecutionErrorEvent > EventManager::GetExecutionError(core::String const &fgName) noexcept
{
    log_.LogInfo() << "EventManager::GetExecutionError(), fgFQN:" << fgName.c_str();
    return emCommManager_->GetExecutionError(fgName);
}

/// @brief Get the initial machine state transition result
/// @return true Transition successful
/// @return false Transition failed
bool EventManager::_getInitialMachineStateTransitionResult() noexcept
{
    log_.LogInfo() << "EventManager::_getInitialMachineStateTransitionResult(), begin.";
    std::ignore = emCommManager_->ReportExecutionState(ara::exec::ExecutionState::kRunning);
    log_.LogInfo() << "EventManager::_getInitialMachineStateTransitionResult(), after ReportExecutionState.";

    uint32_t const cSleepTime{50U};
    bool checkRes{false};
    do {
        core::Future< void > fut{emCommManager_->GetInitialMachineStateTransitionResult()};
        core::Result< void > const res{fut.GetResult()};
        if (res) {  // Success
            log_.LogInfo() << "EventManager::_getInitialMachineStateTransitionResult(), succeed.";
            checkRes = true;
            break;
        }
        core::ErrorDomain::CodeType const err{res.Error().Value()};
        if (static_cast< core::ErrorDomain::CodeType >(exec::ExecErrc::kNoError) == err) {  // Whether it is during switching
            log_.LogInfo() << "EventManager::_getInitialMachineStateTransitionResult(), got kNoError, so we try to "
                              "_getInitialMachineStateTransitionResult later";
            std::this_thread::sleep_for(std::chrono::milliseconds(cSleepTime));
        } else {  // Other errors
            log_.LogError() << "EventManager::_getInitialMachineStateTransitionResult(), failed with res:"
                            << res.Error().Message().data();
            checkRes = false;
            break;
        }
    } while (started_);
    log_.LogInfo() << "EventManager::_getInitialMachineStateTransitionResult(), end with checkRes:" << checkRes;
    return checkRes;
}

}  // namespace event_management
}  // namespace sm
}  // namespace ara