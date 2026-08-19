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
/// @file       supervision_controller.cpp
/// @brief
/// @details
/// @date       2024-06-20
/// @author     wangyanlong
/// @version    1.2.0
///
/// ================================================================

#include "ara/phm/internal/svmanager/supervision_controller.h"

#include "ara/phm/internal/phm_log.h"

namespace ara {
namespace phm {
namespace internal {
/// @brief Destructor.
SupervisionController::~SupervisionController() noexcept
{
    LOG_INFO << "~SupervisionController";
    std::ignore = Stop();
}

/// @brief Init supervision controller
/// @return 0, success;< 0, fail
int32_t SupervisionController::Init() noexcept
{
    LOG_INFO << "SupervisionController init start.";
    ara::core::String const tempStr{""};
    configManager_ = ConfigManager::GetInstanceShared(tempStr);
    if (configManager_ == nullptr) {
        LOG_ERROR << "configManager_ is null.";
        return -1;
    }

    ara::core::Vector< RecoveryNotificationPortMappingConf > recoveryNotificationMapping{
        configManager_->GetRecoveryNotificationMap()};
    // TODO(wangyanlong): This may need to be changed later, generate the instance identifier directly into phm
    for (RecoveryNotificationPortMappingConf const &element : recoveryNotificationMapping) {
        LOG_INFO << "health channel id:" << element.healthChannelId.c_str()
                 << ", instance:" << element.instanceSpecifier.c_str();
        recoveryActionMap_[element.healthChannelId]          = element.instanceSpecifier;
        recoveryActionReverseMap_[element.instanceSpecifier] = element.healthChannelId;
    }

    if (_makeAndOrganizeSupervisionElement() < 0) {
        LOG_ERROR << "_makeAndOrganizeSupervisionElement error.";
        return -1;
    }

#ifdef COMPILE_WITH_IAM
    iamHandle_ = std::make_unique< ara::iam::internal::grant::IAMGrantPhmQuery >();
    if (iamHandle_ == nullptr) {
        LOG_ERROR << "iamHandle_ is null.";
        return -1;
    }
    if (!iamHandle_->Initialize()) {
        LOG_ERROR << "init iamHandle_ failed.";
        return -1;
    }
#endif

    if (_makeCommunicationHandles() < 0) {
        LOG_ERROR << "_makeCommunicationHandles failed.";
        return -1;
    }

    findProcessClient_ = std::make_unique< ara::exec::internal::FindProcessClient >();
    LOG_INFO << "SupervisionController init end";
    return 0;
}

/// @brief Stop supervision such as stop alive supervision, stop deadline
/// supervision.
/// @return 0, success;< 0, fail
int32_t SupervisionController::Stop() noexcept
{
    LOG_INFO << "stop SupervisionController";

    if (fgStateClient_ != nullptr) {
        fgStateClient_->Destroy();
        fgStateClient_ = nullptr;
    }

    if (supervisionServer_) {
        LOG_INFO << "close supervisionServer_.";
        std::ignore = supervisionServer_->Close();
    }

    if (recoveryActionExtServer_) {
        LOG_INFO << "close recoveryActionExtServer_.";
        std::ignore = recoveryActionExtServer_->Close();
    }

    if (recoveryActionClient_) {
        LOG_INFO << "close recoveryActionClient_.";
        std::ignore = recoveryActionClient_->Close();
    }

    for (auto const &element : baseSupervision_) {
        LOG_INFO << "stop base supervision " << element.second->GetFqn();
        element.second->Stop();
    }

    for (auto const &element : globalSupervision_) {
        LOG_INFO << "stop global supervision " << element.second->GetFqn();
        element.second->Stop();
    }

    return 0;
}

/// @brief Used by PhmContext to register a function, SupervisionController can use this function to trigger
/// watchdog.
/// @param trigger a callback used by supervision controller to trigger watchdog
void SupervisionController::SetWatchdogTriggerFunction(std::function< void() > const &trigger) noexcept
{
    LOG_INFO << "set watchdog trigger function.";
    watchdogTriggerFuntion_ = trigger;
}

/// @brief Handle the event: process checkpoint, get local supervision status, get global supervision status.
/// @throws QAC
/// @param eventType type of event.
/// @param specifierId instance specifier of SupervisedEntity.
/// @param processId id of process who send the event.
/// @param checkpointId id of checkpoint.
/// @param processClusterAffiliation process cluster affiliation
/// @param timestamp time stamp.
void SupervisionController::_supervisionComServerHandler(
    ara::phm::internal::svcom::EventType const eventType,
    Specifier const &specifierId,
    ProcessId const processId,
    CheckpointId const checkpointId,
    ara::phm::internal::svcom::ProcessClusterAffiliation const processClusterAffiliation,
    int64_t const timestamp)
{
    LOG_DEBUG << "event from supervised entity " << specifierId << ", pid " << processId << ", event type "
              << GetSupervisionComEventTypeString(eventType) << ", checkpoint id " << checkpointId
              << ", cluster affiliation " << static_cast< uint32_t >(processClusterAffiliation);

    if (ara::phm::internal::svcom::EventType::kReportCheckpoint == eventType) {
        if (!_checkAccessByIam(eventType, specifierId, processId, checkpointId)) {
            LOG_ERROR << "check error by iam, specifier " << specifierId << ", pid " << processId << ", event type "
                      << GetSupervisionComEventTypeString(eventType) << ", checkpoint id " << checkpointId;
            return;
        }
    }

    switch (eventType) {
        case ara::phm::internal::svcom::EventType::kReportCheckpoint: {
            if (checkpointBaseMap_.count(specifierId) == static_cast< size_t >(0)) {
                LOG_WARN << "checkpoint not defined " << specifierId.c_str() << ", id " << checkpointId;
                return;
            }

            if (checkpointBaseMap_[specifierId].count(checkpointId) == static_cast< size_t >(0)) {
                LOG_ERROR << "can not find " << checkpointId << " in checkpointBaseMap_.";
                return;
            }

            {
                std::lock_guard< std::mutex > const lk{supervisionModeLock_};
                for (std::shared_ptr< BaseSupervision > &baseSupervision :
                     checkpointBaseMap_[specifierId][checkpointId]) {
                    ara::core::String const idTmp{specifierId + std::move(std::to_string(checkpointId))};
                    std::shared_ptr< SupervisionCheckpoint > const checkpoint{checkpointIdMap_[idTmp]};
                    if (checkpoint == nullptr) {
                        LOG_ERROR << idTmp << " not found in checkpointIdMap_, size " << checkpointIdMap_.size();
                        return;
                    }

                    checkpoint->SetProcessClusterAffiliation(processClusterAffiliation);
                    baseSupervision->ProcessCheckpoint(checkpoint, processId, timestamp);
                }
            }
            break;
        }
        case ara::phm::internal::svcom::EventType::kGetLocalSupervisionStatus: {
            if (localStatusMap_.count(specifierId) == static_cast< size_t >(0)) {
                LOG_ERROR << specifierId.c_str() << " not in localStatusMap_";
                return;
            }
            SupervisionStatus const status{static_cast< uint32_t >(localStatusMap_[specifierId]->GetStatus())};
            supervisionServer_->Reply(status);
            break;
        }
        case ara::phm::internal::svcom::EventType::kGetGlobalSupervisionStatus: {
            if (globalStatusMap_.count(localStatusMap_[specifierId]) == static_cast< size_t >(0)) {
                LOG_ERROR << specifierId.c_str() << " not in globalStatusMap_";
                return;
            }

            GlobalSupervisionStatus const status{globalStatusMap_[localStatusMap_[specifierId]]->GetStatus()};
            supervisionServer_->Reply(static_cast< uint32_t >(status));
            break;
        }
        case ara::phm::internal::svcom::EventType::kUnkown:
        default: {
            LOG_ERROR << "invalid event type " << GetSupervisionComEventTypeString(eventType);
            break;
        }
    }
}

/// @brief Registered to FGStateNotifyAsyncClient to listen change of all function group state in in initial
/// process.
/// @throws QAC
/// @param res result.
/// @param allFgState all function group state.
void SupervisionController::_initialFgStateReached(ara::core::Result< void > const &res,
                                                   ara::core::Vector< FGStateType > const &allFgState)
{
    LOG_INFO << "initial fg state reached.";
    if (!res.HasValue()) {
        LOG_ERROR << "initial fg state error:" << res.Error().Message().data();
        return;
    }

    for (FGStateType const &fgState : allFgState) {
        LOG_DEBUG << "initial fg state, " << fgState.fgName << "-" << fgState.fgState;
        _switchSupervisionMode(fgState.fgName, fgState.fgState);
    }
}

/// @brief The handler of the result of FGStateNotifyAsyncClient::SubscribeFGState.
/// @param res result.
void SupervisionController::FgStateSubscribeResponseHandler(ara::core::Result< void > const &res) noexcept
{
    if (!res.HasValue()) {
        LOG_ERROR << "fg state subscriber error:" << res.Error().Message().data();
    }
}

/// @brief Registered to FGStateNotifyAsyncClient to listen change of one function group state
/// @param fgState function group name and its state
void SupervisionController::_fgStateChanged(FGStateType const &fgState)
{
    LOG_INFO << "fg state change:" << fgState.fgName << "-" << fgState.fgState;
    _switchSupervisionMode(fgState.fgName, fgState.fgState);
}

/// @brief Registerd to GlobalSupervision to listen status change of GlobalSupervision
/// @throws QAC
/// @param globalSupervisionName name of global supervision.
/// @param globalSupervisionStatus status of global supervision.
/// @param baseSupervisionType type of base supervision.
/// @param processName name of process.
/// @param processClusterAffiliation process cluster affiliation
void SupervisionController::_globalSupervisionStatusChanged(
    ara::core::String const &globalSupervisionName,
    GlobalSupervisionStatus const &globalSupervisionStatus,
    BaseSupervisionType const &baseSupervisionType,
    ara::core::String const &processName,
    ara::phm::internal::svcom::ProcessClusterAffiliation const processClusterAffiliation)
{
    LOG_DEBUG << "global supervision " << globalSupervisionName << " status changed to "
              << GlobalSupervision::GetGlobalSupervisionStatusString(globalSupervisionStatus) << ", process name "
              << processName << ", process cluster adffiliation is "
              << static_cast< uint32_t >(processClusterAffiliation);

    if (globalSupervisionStatus != GlobalSupervisionStatus::kStopped) {
        LOG_DEBUG << "status of global supervision " << globalSupervisionName.c_str()
                  << " is not stopped, so futher process is not needed.";
        return;
    }

    if ((processClusterAffiliation == ara::phm::internal::svcom::ProcessClusterAffiliation::kExecutionManagement)
        || (processClusterAffiliation == ara::phm::internal::svcom::ProcessClusterAffiliation::kStateManagement)) {
        LOG_INFO << "EM or SM need recover, affiliation " << static_cast< uint32_t >(processClusterAffiliation);
        if (watchdogTriggerFuntion_) {
            watchdogTriggerFuntion_();
        } else {
            LOG_ERROR << "watchdogTriggerFuntion_ is null";
        }
        return;
    }

    std::shared_ptr< GlobalSupervision > globalSupervision{globalSupervision_[globalSupervisionName]};
    if (!globalSupervision->IsOffered()) {
        LOG_INFO << "global supervision " << globalSupervisionName.c_str() << " is not offered.";
        return;
    }

    ara::core::String const metaModelIdentifier{
        recoveryActionMap_[ara::core::String(globalSupervision->GetMetaModelIdentifier())]};
    if (metaModelIdentifier.empty()) {
        LOG_WARN << "global supervision " << globalSupervisionName
                 << " not configured in recovery notification mapping, so do not recovery it.";
        return;
    }

    LOG_INFO << "global supervision " << globalSupervisionName << " is stopped, "
             << " try to recovery it by notify SM.";

    std::shared_ptr< ara::phm::internal::racom::NotifyInfo > notifyInfo{
        std::make_shared< ara::phm::internal::racom::NotifyInfo >()};
    notifyInfo->identifier = recoveryActionMap_[ara::core::String(globalSupervision->GetMetaModelIdentifier())];

    ara::core::String const fgName{processSupervisionModeMap_[processName]->GetFgName()};
    ara::core::String const fgState{processSupervisionModeMap_[processName]->GetFgState()};
    notifyInfo->fgName = fgName;

    if (fgSupervisionMode_.count(fgName) == static_cast< size_t >(0)) {
        LOG_ERROR << fgName.c_str() << " not in fgSupervisionMode_.";
        return;
    }
    std::shared_ptr< SupervisionMode > const supervisionMode{fgSupervisionMode_[fgName]->GetSupervisionMode(fgState)};
    if (supervisionMode == nullptr) {
        LOG_ERROR << fgState.c_str() << " no supervision mode.";
        return;
    }

    notifyInfo->executionError        = static_cast< uint32_t >(supervisionMode->GetProcessExecutionError(processName));
    notifyInfo->baseSupervisionType   = baseSupervisionType;
    notifyInfo->waitResponseTimeOutMs = globalSupervision->GetRecoveryNotificationTimeout();
    notifyInfo->maxRetryTimes         = static_cast< int32_t >(globalSupervision->GetRecoveryNotificationRetry());
    notifyInfo->haveRetriedTimes      = 0;
    if (recoveryActionClient_->Notify(notifyInfo) < 0) {
        LOG_ERROR << "recoveryActionClient_->Notify error.";
        return;
    }
}

/// @brief Check whether the checkpoint received by SupervisionController is legal
/// @param eventType type of event from SupervisedEntity
/// @param specifierId instance id of SupervisedEntity
/// @param processId id of process who report checkpoint.
/// @param checkpointId checkpoint id.
/// @return true legal;false illegal.
bool SupervisionController::_checkAccessByIam(ara::phm::internal::svcom::EventType const eventType,
                                              Specifier const &specifierId,
                                              ProcessId const processId,
                                              uint32_t const checkpointId) noexcept
{
#ifdef COMPILE_WITH_IAM
    bool res{false};
    std::map< CheckpointId, ara::core::Vector< std::shared_ptr< BaseSupervision > > > checkpointIdMap;
    switch (eventType) {
        case ara::phm::internal::svcom::EventType::kReportCheckpoint: {
            res = iamHandle_->HasCheckPointGrant(processId, checkpointId);
            break;
        }
        case ara::phm::internal::svcom::EventType::kGetLocalSupervisionStatus:
        case ara::phm::internal::svcom::EventType::kGetGlobalSupervisionStatus: {  /// TODO performance?
            if (checkpointBaseMap_.count(specifierId) == static_cast< size_t >(0)) {
                LOG_ERROR << "can not find " << specifierId.c_str() << " in checkpointBaseMap_.";
                res = false;
                break;
            }

            checkpointIdMap = checkpointBaseMap_[specifierId];
            for (auto const &cpId : checkpointIdMap) {
                if (iamHandle_->HasCheckPointGrant(processId, cpId.first)) {
                    res = true;
                    break;
                }
            }
            res = false;
            break;
        }
        default: {
            LOG_ERROR << "invalid event type " << static_cast< uint32_t >(eventType);
            res = false;
            break;
        }
    }
    return res;
#else
    std::ignore = eventType;
    std::ignore = specifierId;
    std::ignore = processId;
    std::ignore = checkpointId;
    return true;
#endif
}

/// @brief switch supervision mode of base supervision.
/// @param baseSupervisionFqn fqn of base supervision.
/// @param stateChange state change of process.
void SupervisionController::_switchBaseSupervisionMode(ara::core::String const &baseSupervisionFqn,
                                                       ProcessStateChange const &stateChange) noexcept
{
    if (stateChange == ProcessStateChange::kIdleToRunning) {
        if (baseSupervision_.count(baseSupervisionFqn) == 0U) {
            LOG_ERROR << baseSupervisionFqn << " not in baseSupervision_.";
            return;
        }

        // When switching from non-running state to running, no other operations can be performed, such as ResetStatistics
        baseSupervision_[baseSupervisionFqn]->SetActive(true);
        baseSupervision_[baseSupervisionFqn]->Start();
    } else if (stateChange == ProcessStateChange::kRunningToIdle) {
        if (baseSupervision_.count(baseSupervisionFqn) == 0U) {
            LOG_ERROR << baseSupervisionFqn << " not in baseSupervision_.";
            return;
        }

        baseSupervision_[baseSupervisionFqn]->Stop();
        baseSupervision_[baseSupervisionFqn]->SetActive(false);
        baseSupervision_[baseSupervisionFqn]->ResetStatistics();
        baseSupervision_[baseSupervisionFqn]->SetStatus(BaseSupervisionStatus::kCorrect);
    } else if (stateChange == ProcessStateChange::kRestart) {
        //@TODO This has flaws. The strict approach is for EM to report terminating to PHM before sending SIGTERM to the process
        if (baseSupervision_.count(baseSupervisionFqn) == 0U) {
            LOG_ERROR << baseSupervisionFqn << " not in baseSupervision_.";
            return;
        }

        baseSupervision_[baseSupervisionFqn]->Stop();
        baseSupervision_[baseSupervisionFqn]->ResetStatistics();
        baseSupervision_[baseSupervisionFqn]->SetStatus(BaseSupervisionStatus::kCorrect);
        baseSupervision_[baseSupervisionFqn]->SetActive(true);
        baseSupervision_[baseSupervisionFqn]->Start();
    } else {
        LOG_ERROR << "invalid process state change";
    }
    // kKeepRunning、kInvalid nothing
}

/// @brief Switch supervision mode.
/// @param fgName name of function group.
/// @param fgState state of function group.
void SupervisionController::_switchSupervisionMode(ara::core::String const &fgName,
                                                   ara::core::String const &fgState) noexcept
{
    bool const fgNameEmpty{fgName.empty()};
    bool const fgStateEmpty{fgState.empty()};
    if (fgNameEmpty || fgStateEmpty) {
        LOG_WARN << "fg name: " << fgName << ", fg state:" << fgState;
        return;
    }

    std::shared_ptr< FgSupervisionMode > const fgSupervisionMode{fgSupervisionMode_[fgName]};
    if (fgSupervisionMode == nullptr) {
        LOG_WARN << "fgSupervisionMode is null, fg " << fgName;
        return;
    }

    LOG_INFO << "switch supervision mode, fg " << fgName << ", state " << fgState << ", old state "
             << fgSupervisionMode->GetFgState();
    {
        std::lock_guard< std::mutex > const lk{supervisionModeLock_};
        std::set< ara::core::String > baseSupervisionsInThisFg;
        std::set< ara::core::String > const processInThisFg{fgSupervisionMode->GetProcess()};
        for (ara::core::String const &element : processInThisFg) {
            ara::core::String const processFqn{element};

            // Get the process pid, determine process state changes based on the pid before and after the function group state switch
            uint32_t pid{0U};
            ara::core::Result< void > const result{findProcessClient_->FindByName(processFqn, pid)};
            if (!result.HasValue()) {
                LOG_DEBUG << "get pid of " << processFqn << " failed.";
                pid = 0U;
            } else {
                LOG_DEBUG << processFqn << ", new pid:" << pid;
            }
            fgSupervisionMode->UpdateProcessPidAndStateChange(processFqn, pid);

            // Based on different process state changes, perform different processing on the base supervision within the process
            ProcessStateChange const stateChange{fgSupervisionMode->GetProcessStateChange(processFqn)};
            if (stateChange == ProcessStateChange::kInvalid) {
                LOG_WARN << "state change of process " << processFqn << " invalid.";
                continue;
            }
            if (processBaseSupervisionMap_.count(processFqn) == 0U) {
                continue;
            }
            std::set< ara::core::String > const baseSupervision{processBaseSupervisionMap_[processFqn]};
            for (ara::core::String const &baseSupervisionFqn : baseSupervision) {
                _switchBaseSupervisionMode(baseSupervisionFqn, stateChange);
                std::ignore = baseSupervisionsInThisFg.insert(baseSupervisionFqn);
            }
        }

        // local
        for (ara::core::String const &element : baseSupervisionsInThisFg) {
            if (baseLocalMap_.count(element) > 0U) {
                for (std::shared_ptr< LocalSupervision > const &local : baseLocalMap_[element]) {
                    local->SwitchSupervisionMode();
                }
            }
        }

        // global
        for (ara::core::String const &element : baseSupervisionsInThisFg) {
            if (baseLocalMap_.count(element) > 0U) {
                for (std::shared_ptr< LocalSupervision > const &local : baseLocalMap_[element]) {
                    if (globalStatusMap_[local] != nullptr) {
                        globalStatusMap_[local]->SwitchSupervisionMode();
                    }
                }
            }
        }
    }

    fgSupervisionMode->SetState(fgState);
}  // namespace internal

/// @brief A callback registered to recoveryActionExtServer_ to process Offer/StopOffer/GetGlobalSupervisionStatus
/// from SM.
/// @throws QAC
/// @param instance the instance specifier of RecoveryAction.
/// @param eventType offer/stopoffer
void SupervisionController::_recoveryActionExtServerHandler(ara::core::String const &instance,
                                                            ara::phm::internal::raextcom::EventType const eventType)
{
    LOG_DEBUG << "recovery action extend server, instance " << instance << ", event "
              << static_cast< uint32_t >(eventType);
    std::shared_ptr< GlobalSupervision > globalSupervision;
    for (auto const &element : globalSupervision_) {
        if (element.second->GetMetaModelIdentifier() == recoveryActionReverseMap_[ara::core::String(instance)]) {
            globalSupervision = element.second;
            break;
        }
    }
    if (globalSupervision == nullptr) {
        LOG_WARN << "there is no global supervision related with " << instance.c_str() << ", it may be not configured";
        return;
    }

    if (eventType == ara::phm::internal::raextcom::EventType::kOffer) {
        globalSupervision->SetOffer(true);
    } else if (eventType == ara::phm::internal::raextcom::EventType::kStopOffer) {
        globalSupervision->SetOffer(false);
    } else if (eventType == ara::phm::internal::raextcom::EventType::kGetGlobalSupervisionStatus) {
        recoveryActionExtServer_->Reply(static_cast< uint32_t >(globalSupervision->GetStatus()));
    } else {
        /// @details for QAC need this else
    }
}

/// @brief To process the recovery result, this function is registered to the ara::phm::internal::racom::Client
/// @throws QAC
/// @param result result of recover.
/// @param instance instance of RecoveryAction.
void SupervisionController::_recoveryResultHandler(ara::phm::internal::racom::RecoveryResult const result,
                                                   ara::core::String const &instance) const
{
    if (result == ara::phm::internal::racom::RecoveryResult::kSuccess) {
        // TODO(wangyanlong): After recovery succeeds, does the supervision state need to be modified? There must be a function group switch in between, and state modification is done during the switch √
        LOG_INFO << instance << " recovery success.";
    } else if (result == ara::phm::internal::racom::RecoveryResult::kNotOffered) {
        LOG_INFO << instance << " is not offered, so it is not needed to recovery it.";
    } else {
        LOG_INFO << instance.c_str() << " recovery failed.";
        if (watchdogTriggerFuntion_) {
            watchdogTriggerFuntion_();
        } else {
            LOG_ERROR << "watchdogTriggerFuntion_ is null";
        }
    }
}

/// @brief Make and organize objects such as
/// AliveSupervision、DeadlineSupervision.
/// @return 0, success;< 0, fail
int32_t SupervisionController::_makeAndOrganizeSupervisionElement() noexcept
{
    LOG_INFO << "SupervisionController::_makeAndOrganizeSupervisionElement start.";

    // The order of creating various resources cannot be changed
    ara::core::Vector< std::shared_ptr< SupervisionCheckpoint > > checkpoint{
        SupervisionFactory::MakeCheckpoint(configManager_->GetCheckpointConf())};
    for (std::shared_ptr< SupervisionCheckpoint > &element : checkpoint) {
        checkpoint_[element->GetFqn()] = element;
        ara::core::String const idTmp{element->GetIdentifier() + std::to_string(element->GetCheckpointId())};
        checkpointIdMap_[idTmp] = element;
    }

    ara::core::Vector< std::shared_ptr< AliveSupervision > > aliveSupervision{
        SupervisionFactory::MakeAliveSupervision(configManager_->GetAliveSupervisionConf(), checkpoint_)};
    for (std::shared_ptr< AliveSupervision > &element : aliveSupervision) {
        baseSupervision_[element->GetFqn()] = element;
    }

    ara::core::Vector< std::shared_ptr< DeadlineSupervision > > deadlineSupervision{
        SupervisionFactory::MakeDeadlineSupervision(configManager_->GetDeadlineSupervisionConf(), checkpoint_)};
    for (std::shared_ptr< DeadlineSupervision > &element : deadlineSupervision) {
        baseSupervision_[element->GetFqn()] = element;
    }

    ara::core::Vector< std::shared_ptr< LogicalSupervision > > logicalSupervision{
        SupervisionFactory::MakeLogicalSupervision(configManager_->GetLogicalSupervisionConf(), checkpoint_)};
    for (std::shared_ptr< LogicalSupervision > &element : logicalSupervision) {
        baseSupervision_[element->GetFqn()] = element;
    }

    ara::core::Vector< std::shared_ptr< LocalSupervision > > localSupervision{SupervisionFactory::MakeLocalSupervision(
        configManager_->GetLocalSupervisionConf(), baseSupervision_, baseLocalMap_)};
    for (std::shared_ptr< LocalSupervision > &element : localSupervision) {
        localSupervision_[element->GetFqn()] = element;
    }

    ara::core::Vector< std::shared_ptr< GlobalSupervision > > globalSupervision{
        SupervisionFactory::MakeGlobalSupervision(configManager_->GetGlobalSupervisionConf(), localSupervision_)};
    for (std::shared_ptr< GlobalSupervision > &element : globalSupervision) {
        globalSupervision_[element->GetFqn()] = element;

        element->RegisterStatusChangeHandler(
            // TODO(wangyanlong): std:bind is not recommended in QAC
            std::bind(&SupervisionController::_globalSupervisionStatusChanged, this, std::placeholders::_1,
                      std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5));
    }

    ara::core::Vector< std::shared_ptr< FgSupervisionMode > > supervisionMode{
        SupervisionFactory::MakeSupervisionMode(configManager_->GetSupervisionModeConf())};
    for (std::shared_ptr< FgSupervisionMode > &element : supervisionMode) {
        fgSupervisionMode_[element->GetFgName()] = element;

        ara::core::Vector< std::shared_ptr< SupervisionMode > > ret{element->GetAllSupervisionMode()};
        for (std::shared_ptr< SupervisionMode > &svMode : ret) {
            ara::core::Map< ara::core::String, int32_t > processInfo{svMode->GetProcessInfo()};
            for (auto const &process : processInfo) {
                processSupervisionModeMap_[process.first] = element;
            }
        }
    }

    _reOrganizeCheckpoint();
    _reOrganizeLocalSupervision();
    _makeLocalGlobalMap();
    _makeProcessBaseSupervisionMap();

    LOG_INFO << "SupervisionController::_makeAndOrganizeSupervisionElement end.";
    return 0;
}

/// @brief Create handles used to communicate with AA、SM.
/// @return 0, success; < 0, fail.
int32_t SupervisionController::_makeCommunicationHandles() noexcept
{
    supervisionServer_ = ara::phm::internal::svcom::Server::GetInstanceUnique(std::bind(
        // TODO(wangyanlong): std:bind is not recommended in QAC
        &SupervisionController::_supervisionComServerHandler, this, std::placeholders::_1, std::placeholders::_2,
        std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6));
    if (supervisionServer_ == nullptr) {
        LOG_ERROR << "get ara::phm::internal::svcom::Server failed.";
        return -1;
    }
    if (supervisionServer_->Open() < 0) {
        LOG_ERROR << "open supervisionServer_ failed.";
        return -1;
    }

    recoveryActionClient_ = ara::phm::internal::racom::Client::GetInstanceUnique(
        // TODO(wangyanlong): std:bind is not recommended in QAC
        std::bind(&SupervisionController::_recoveryResultHandler, this, std::placeholders::_1, std::placeholders::_2));
    if (recoveryActionClient_ == nullptr) {
        LOG_ERROR << "get ara::phm::internal::racom::Client failed.";
        return -1;
    }
    if (recoveryActionClient_->Open() < 0) {
        LOG_ERROR << "open recoveryActionClient_ failed";
        return -1;
    }
    // TODO(wangyanlong): std:bind is not recommended in QAC
    recoveryActionExtServer_ = ara::phm::internal::raextcom::Server::GetInstanceUnique(std::bind(
        &SupervisionController::_recoveryActionExtServerHandler, this, std::placeholders::_1, std::placeholders::_2));
    if (recoveryActionExtServer_ == nullptr) {
        LOG_ERROR << "get ara::phm::internal::raextcom::Server failed.";
        return -1;
    }
    if (recoveryActionExtServer_->Open() < 0) {
        LOG_ERROR << "open recoveryActionExtServer_ failed";
        return -1;
    }

    fgStateClient_ = FGStateNotifyAsyncClient::GetInstance();
    if (fgStateClient_ == nullptr) {
        // TODO(wangyanlong): Should the resources allocated above be released? How to manage them properly,
        // Typically, if creation or initialization fails, release the already allocated resources before returning
        LOG_ERROR << "FGStateNotifyAsyncClient::GetInstance failed";
        return -1;
    }

    ara::core::Result< void > ret{fgStateClient_->Init(std::move(ara::core::String{"fg_state_notify_phmd"}))};
    if (!ret.HasValue()) {
        LOG_ERROR << "init fgStateClient_ errmsg=" << ret.Error().Message().data();
        return -1;
    }

    int32_t const timeout{4000};
    // TODO(wangyanlong): std:bind is not recommended in QAC
    ret = fgStateClient_->RequestAllFGState(
        std::bind(&SupervisionController::_initialFgStateReached, this, std::placeholders::_1, std::placeholders::_2),
        timeout);
    if (!ret.HasValue()) {
        LOG_ERROR << "fgStateClient_->RequestAllFGState errmsg=" << ret.Error().Message().data();
        return -1;
    }

    ret = fgStateClient_->SubscribeFGState(
        [](ara::core::Result< void > const &res) { SupervisionController::FgStateSubscribeResponseHandler(res); },
        // TODO(wangyanlong): std:bind is not recommended in QAC
        std::bind(&SupervisionController::_fgStateChanged, this, std::placeholders::_1), timeout);
    if (!ret.HasValue()) {
        LOG_ERROR << "fgStateClient_->SubscribeFGState errmsg=" << ret.Error().Message().data();
        return -1;
    }

    return 0;
}

/// @brief To make checkpointBaseMap_.
void SupervisionController::_reOrganizeCheckpoint() noexcept
{
    /// Find the base supervision based on the reported checkpoint information
    for (auto const &checkpoint : checkpoint_) {
        for (auto const &baseSupervision : baseSupervision_) {
            ara::core::Vector< std::shared_ptr< SupervisionCheckpoint > > const cps{
                baseSupervision.second->GetCheckpoint()};
            for (std::shared_ptr< SupervisionCheckpoint > const &cp : cps) {
                if (cp == checkpoint.second) {
                    checkpointBaseMap_[checkpoint.second->GetIdentifier()][checkpoint.second->GetCheckpointId()]
                        .push_back(baseSupervision.second);
                }
            }
        }
    }
}

/// @brief to make localStatusMap_.
void SupervisionController::_reOrganizeLocalSupervision() noexcept
{
    /// <checkpoint identifier,
    /// LocalSupervision>, used for SupervisedEntity to obtain LocalSupervisionStatus
    /// The concept here is unclear, can't local supervision have an fqn? ? ? ? ? ? ? ? ?
    /// Overlap is possible here. local supervision status, theoretically, has such a problem
    for (auto const &localSupervision : localSupervision_) {
        ara::core::Vector< std::shared_ptr< BaseSupervision > > const baseSupervisions{
            localSupervision.second->GetBaseSupervision()};
        for (std::shared_ptr< BaseSupervision > const &baseSupervision : baseSupervisions) {
            ara::core::Vector< std::shared_ptr< SupervisionCheckpoint > > const cps{baseSupervision->GetCheckpoint()};
            for (std::shared_ptr< SupervisionCheckpoint > const &cp : cps) {
                localStatusMap_[cp->GetIdentifier()] = localSupervision.second;
            }
        }
    }
}

/// @brief to make globalStatusMap_.
void SupervisionController::_makeLocalGlobalMap() noexcept
{
    for (auto const &globalSupervision : globalSupervision_) {
        ara::core::Vector< std::shared_ptr< LocalSupervision > > const referencedLocalSupervision{
            globalSupervision.second->GetLocalSupervision()};
        for (std::shared_ptr< LocalSupervision > const &element : referencedLocalSupervision) {
            globalStatusMap_[element] = globalSupervision.second;
        }
    }
}

/// @brief to make processBaseSupervisionMap_.
void SupervisionController::_makeProcessBaseSupervisionMap() noexcept
{
    std::cout << "checkpoint count " << checkpoint_.size() << std::endl;
    for (auto const &checkpoint : checkpoint_) {
        //@TODO This is too slow
        ara::core::String const processFqn{checkpoint.second->GetProcessName()};
        for (auto const &baseSupervision : baseSupervision_) {
            //
            ara::core::Vector< std::shared_ptr< SupervisionCheckpoint > > cps{baseSupervision.second->GetCheckpoint()};
            for (std::shared_ptr< SupervisionCheckpoint > &cp : cps) {
                // cp belongs to this current base supervision
                // processFqn is the process to which the checkpoint belongs
                // If cp equals the checkpoint, then base
                // supervision belongs to the process represented by processFqn
                if (cp == checkpoint.second) {
                    std::ignore = processBaseSupervisionMap_[processFqn].insert(baseSupervision.second->GetFqn());
                    LOG_DEBUG << baseSupervision.second->GetFqn() << " belongs to process " << processFqn;
                }
            }
        }
    }
}

/// @brief returns the string of event type from supervised entity.
/// @param eventType event type.
/// @return the string of event type from supervised entity.
ara::core::StringView SupervisionController::GetSupervisionComEventTypeString(
    ara::phm::internal::svcom::EventType const &eventType) noexcept
{
    if (eventType == ara::phm::internal::svcom::EventType::kReportCheckpoint) {
        return "ReportCheckpoint";
    }
    if (eventType == ara::phm::internal::svcom::EventType::kGetLocalSupervisionStatus) {
        return "GetLocalSupervisionStatus";
    }
    if (eventType == ara::phm::internal::svcom::EventType::kGetGlobalSupervisionStatus) {
        return "GetGlobalSupervisionStatus";
    }
    return "Unknown";
}

}  // namespace internal
}  // namespace phm
}  // namespace ara
