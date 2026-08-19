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
/// @file       event.cpp
/// @brief      This file implements event functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=fault_management
/// @trace_id_sr=07e8882d-2602-407f-87d2-9b72ead21809
/// @unit_name=event
/// @unit_description=This file provides event functionality
/// @interface_level=
/// @endcode
///
/// ================================================================

#include "fault_management/event.h"

#include <functional>

#include "common/log.h"
#include "fault_management/condition_group.h"
#include "fault_management/control_dtc_setting.h"
#include "fault_management/debounce_counter_base.h"
#include "fault_management/debounce_interface.h"
#include "fault_management/debounce_time_base.h"
#include "fault_management/debounce_user_defined.h"
#include "fault_management/dtc.h"
#include "fault_management/fault_management_runtime_context.h"
#include "fault_management/indicator.h"
#include "fault_management/memory_destination.h"
#include "fault_management/memory_entry.h"
#include "fault_management/monitor.h"
#include "fault_management/operation_cycle.h"
#include "isoft/uds/uds_nrc_error_domain.h"
namespace isoft {
namespace uds {
namespace server {


Event::Event(FaultManagementRuntimeContext& context) noexcept : context_{context} {}


bool Event::Init(EventConfig& config,
                 std::map< uint32_t, DebounceAlgorithmConfig >& debounceAlgorithms,
                 std::map< uint32_t, std::shared_ptr< Indicator > >& indicators,
                 std::map< uint32_t, std::shared_ptr< ConditionGroup > >& conditionGroups,
                 std::map< uint32_t, std::shared_ptr< OperationCycle > >& operationCycles,
                 std::map< uint32_t, std::shared_ptr< Dtc > >& dtcs,
                 std::map< uint32_t, std::shared_ptr< MemoryDestination > >& memDestinations)
{
    config_ = config;
    if (config_.eventInterface.get() == nullptr) {
        LogDebug() << "Event::Init -> not config eventInterface event_id=" << config_.associatedEventIdentification;
    } else {
        int32_t res = config_.eventInterface->RegisterGetEventStatus([this]() -> std::uint8_t {
            LogDebug() << "Event::Init -> aa get event status event_id=" << config_.associatedEventIdentification;
            return eventStatusHandler_.GetStatus().Get();
        });
        if (res != 0) {
            LogWarn() << "Event::Init -> RegisterGetEventStatus error event_id ="
                      << config_.associatedEventIdentification << "res =" << res;
            return false;
        }

        res = config_.eventInterface->RegisterGetLatchedWIRStatus([this]() -> bool {
            LogDebug() << "Event::Init -> aa get wir status event_id=" << config_.associatedEventIdentification;
            if (dtc_.get() == nullptr) {
                LogWarn() << "Event::Init -> aa get wir status dtc_ is nullptr event_id ="
                          << config_.associatedEventIdentification;
                return false;
            }

            return dtc_->GetStatus().Get(DTCStatusBit::kWarningIndicatorRequested);
        });
        if (res != 0) {
            LogWarn() << "Event::Init -> RegisterGetLatchedWIRStatus error event_id ="
                      << config_.associatedEventIdentification << "res =" << res;
            return false;
        }

        res = config_.eventInterface->RegisterSetLatchedWIRStatus([this](bool const state) -> int32_t {
            LogDebug() << "Event::Init -> aa set wir status event_id=" << config_.associatedEventIdentification
                       << "state =" << state;
            // [SWS_DM_01035].
            if (!context_.GetDtcSetting()->Get()) {
                LogDebug() << "Event::Init ->aa set wir status but dtc is be setting event_id ="
                           << config_.associatedEventIdentification;
                return static_cast< int32_t >(DiagErrc::kReportIgnored);
            }

            if (dtc_.get() == nullptr) {
                LogWarn() << "Event::Init -> aa set wir status dtc is nullptr event_id ="
                          << config_.associatedEventIdentification;
                return -2;
            }

            if (!state) {
                // [SWS_DM_01034].
                // If the function ara::diag::Event::SetLatchedWIRStatus is called with parameter
                // status = FALSE and the DM internal WIR-bit handling is also not requesting it,
                // the DM shall reset the WIR-bit of the corresponding event to "0"
                DTCStatus const& dtcStatus = dtc_->GetStatus();
                if (dtcStatus.Get(DTCStatusBit::kConfirmedDTC)
                    && dtcStatus.Get(DTCStatusBit::kWarningIndicatorRequested)
                    && !indicatorLock_) {  // If it is WIR-bit set by AA, reset to 0; otherwise do not affect current WIR-bit state.
                    LogDebug() << "Event::Init -> aa set wir status dtc but DM internal wir handling is"
                                  "requesting it event_id ="
                               << config_.associatedEventIdentification;
                    return 0;
                }
            }

            indicatorLock_ = state;
            // [SWS_DM_01033].
            dtc_->SetStatusWIR(state);
            for (std::vector< std::shared_ptr< Indicator > >::const_iterator itr{connectedIndicators_.cbegin()};
                 itr != connectedIndicators_.cend(); ++itr) {
                if ((*itr).get() == nullptr) {
                    continue;
                }
                (*itr)->SetStatusWIR(state);
            }

            return 0;
        });
        if (res != 0) {
            LogWarn() << "Event::Init -> RegisterSetLatchedWIRStatus error event_id ="
                      << config_.associatedEventIdentification << "res =" << res;
            return false;
        }

        res = config_.eventInterface->RegisterGetDTCNumber([this](std::uint8_t const& dtcFormat,
                                                                  std::uint32_t& dtcNumber) -> std::int32_t {
            LogDebug() << "Event::Init -> aa get dtc number event_id=" << config_.associatedEventIdentification
                       << "dtcFormat =" << dtcFormat;
            if (static_cast< std::uint8_t >(DTCFormatType::kDTCFormatUDS) != dtcFormat) {
                LogWarn() << "Event::Init ->aa get dtc number not support dtcFormat event_id="
                          << config_.associatedEventIdentification << "dtcFormat =" << dtcFormat;
                return -1;
            }
            if (IsDtcSuppressionOn()) {
                LogWarn() << "A suppressed DTC shall be treated like not configured for the interface ara::diag::Event "
                             "with function ara::diag::Event::GetDTCNumber and shall return kNoSuchDTC ";
                return static_cast< int32_t >(DiagErrc::kNoSuchDTC);
            }
            dtcNumber = GetDTCValue();
            return 0;
        });
        if (res != 0) {
            LogWarn() << "Event::Init -> RegisterGetDTCNumber error event_id =" << config_.associatedEventIdentification
                      << "res =" << res;
            return false;
        }

        res = config_.eventInterface->RegisterGetDebouncingStatus([this]() -> DebouncingState {
            LogDebug() << "Event::Init -> aa get debouncing status event_id=" << config_.associatedEventIdentification;
             return GetDebouncingStatus();
        });
        if (res != 0) {
            LogWarn() << "Event::Init -> RegisterGetDebouncingStatus error event_id ="
                      << config_.associatedEventIdentification << "res =" << res;
            return false;
        }

        res = config_.eventInterface->RegisterGetFaultDetectionCounter([this]() -> std::int8_t {
            LogDebug() << "Event::Init -> aa get fdc event_id=" << config_.associatedEventIdentification;

            return GetFaultDetectionCounter();
        });
        if (res != 0) {
            LogWarn() << "Event::Init -> RegisterGetFaultDetectionCounter error event_id ="
                      << config_.associatedEventIdentification << "res =" << res;
            return false;
        }

        res = config_.eventInterface->RegisterGetTestComplete([this]() -> bool {
            LogDebug() << "Event::Init -> aa get test is_complete event_id=" << config_.associatedEventIdentification;
            return GetTestComplete();
        });
        if (res != 0) {
            LogWarn() << "Event::Init -> RegisterGetTestComplete error event_id ="
                      << config_.associatedEventIdentification << "res =" << res;
            return false;
        }
    }

    if (config_.enableConditionGroup == UINT32_MAX) {
        LogDebug() << "Event::Init -> not config enableConditionGroup event_id="
                   << config_.associatedEventIdentification;
    } else {
        if (!_registerEnableConditionGroup(conditionGroups.find(config_.enableConditionGroup)->second)) {
            LogWarn() << "Event::Init -> RegisterEnableConditionGroup fail event_id ="
                      << config_.associatedEventIdentification
                      << "enableConditionGroup =" << config_.enableConditionGroup;
            return false;
        }
    }

    if (!_registerOperationCycle(operationCycles.find(config_.operationCycle)->second)) {
        LogWarn() << "Event::Init -> RegisterOperationCycle fail event_id =" << config_.associatedEventIdentification
                  << "operationCycle =" << config_.operationCycle;
        return false;
    }

    if (!_registerDtc(dtcs.find(config_.troubleCodeUds)->second)) {
        LogWarn() << "Event::Init -> RegisterDtc fail event_id =" << config_.associatedEventIdentification
                  << "troubleCodeUds =" << config_.troubleCodeUds;
        return false;
    }

    if (config_.monitorInterface.get() == nullptr) {
        LogWarn() << "Event::Init -> no config monitor event_id =" << config_.associatedEventIdentification;
        return false;
    }
    std::shared_ptr< Monitor > monitor{std::make_shared< Monitor >()};
    if (!monitor->Init(config_.monitorInterface, [this](std::int8_t const fdc) {
            if (debounce_ != nullptr) {
                debounce_->SetFDC(fdc);
            }
        })) {
        LogWarn() << "Event::Init -> create monitor fail event_id =" << config_.associatedEventIdentification;
        return false;
    }
    if (!_registerDiagnosticMonitor(monitor)) {
        LogWarn() << "Event::Init -> RegisterDiagnosticMonitor fail event_id ="
                  << config_.associatedEventIdentification;
        return false;
    }

    // [SWS_DM_01037].
    if (config_.confirmationThreshold == 0U) {
        config_.confirmationThreshold = 1U;
    }

    if (config.debounceAlgorithm == UINT32_MAX) {
        LogDebug() << "Event::Init -> not config debouce event_id=" << config_.associatedEventIdentification;
    } else {
        std::map< uint32_t, DebounceAlgorithmConfig >::iterator itr{debounceAlgorithms.find(config.debounceAlgorithm)};
        if (itr == debounceAlgorithms.end()) {
            LogWarn() << "Event::Init -> no find debounceAlgorithm event_id =" << config_.associatedEventIdentification
                      << "debounceAlgorithm_id =" << config.debounceAlgorithm;
            return false;
        }

        DebounceAlgorithmConfig& debounceNode = itr->second;
        switch (debounceNode.type) {
            case DebounceTypeEnum::kCounterBase: {
                debounce_ = std::make_shared< DebounceCounterBase >(shared_from_this(), debounceNode);
            } break;
            case DebounceTypeEnum::kTimeBase: {
                debounce_ = std::make_shared< DebounceTimeBase >(shared_from_this(), debounceNode);
            } break;
            case DebounceTypeEnum::kUserDefined: {
                debounce_ = std::make_shared< DebounceUserDefined >(shared_from_this(), debounceNode);
            } break;
            default: {
                LogWarn() << "Event::Init ->unknown debouce type=" << static_cast< std::uint32_t >(debounceNode.type)
                          << ", event_id=" << config_.associatedEventIdentification
                          << ", type=" << static_cast< std::uint32_t >(debounceNode.type);
            } break;
        }

        if (debounce_ != nullptr) {
            debounce_->SetNotifierFDCChanged([this](std::int8_t const fdc) noexcept {
                if (maxFdcDuringCurCycle_.load() < fdc) {
                    maxFdcDuringCurCycle_ = fdc;
                }
                if (maxFdcSinceLastClear_.load() < fdc) {
                    maxFdcSinceLastClear_ = fdc;
                }
            });
            // [SWS_DM_00018]
            for (std::map< uint32_t, std::shared_ptr< MemoryDestination > >::const_iterator itrMem{
                     memDestinations.cbegin()};
                 itrMem != memDestinations.cend(); ++itrMem) {
                std::shared_ptr< MemoryEntry > entry{itrMem->second->GetEntry(dtc_->GetValue())};
                if (entry.get() == nullptr) {
                    continue;
                }
                bool const isStorageCounter{debounce_->IsStorageCounter()};
                bool const isCounterBase{debounce_->GetType() == DebounceTypeEnum::kCounterBase};
                if (isStorageCounter && isCounterBase) {
                    debounce_->SetFDC(entry->GetFDC(), false);
                    break;
                }
            }
        }
    }

    if (config.connectedIndicator.empty()) {
        LogDebug() << "Event::Init -> no config indicator event_id =" << config_.associatedEventIdentification;
    }
    for (auto const& entry : config.connectedIndicator) {
        std::map< uint32_t, std::shared_ptr< OperationCycle > >::const_iterator const itrOperationCycle{
            std::move(operationCycles.find(entry.healingCycle))};
        if (itrOperationCycle == operationCycles.end()) {
            LogWarn() << "Event::Init -> no find healing operationCycles event_id ="
                      << config_.associatedEventIdentification << "indicator_id =" << entry.indicatorId
                      << "healingCycle_id =" << entry.healingCycle;
            return false;
        }
        std::map< uint32_t, std::shared_ptr< Indicator > >::const_iterator const itr{
            std::move(indicators.find(entry.indicatorId))};
        if (itr == indicators.end()) {
            LogWarn() << "Event::Init -> no find indicator event_id =" << config_.associatedEventIdentification
                      << "indicator_id =" << entry.indicatorId;
            return false;
        }

        if (!itr->second->InitValues(entry.behavior, itrOperationCycle->second, entry.healingCycleCounterThreshold,
                                     shared_from_this())) {
            LogWarn() << "Event::Init -> indicator InitValues error event_id =" << config_.associatedEventIdentification
                      << "indicator_id =" << entry.indicatorId;
            return false;
        }
        connectedIndicators_.push_back(itr->second);
    }

    eventStatusHandler_.SetNotifier([this](EventStatus const statusOld, EventStatus const statusNew) {
        if (config_.eventInterface.get() != nullptr) {
            int32_t const res = config_.eventInterface->NotifyEventStatus(statusNew.Get());
            LogDebug() << "Event::Init ->NotifyEventStatus event_id=" << config_.associatedEventIdentification
                       << "res=" << res;
        }

        if (!isFailedFirst_) {
            bool const isTestFailedThisOperationCycle{statusNew.Get(EventStatusBit::kTestFailedThisOperationCycle)};
            if ((!statusOld.Get(EventStatusBit::kTestFailedThisOperationCycle)) && isTestFailedThisOperationCycle) {
                isFailedFirst_               = true;
                isFailedLast_                = true;
                cycleCounterSinceLastFailed_ = 0U;
            }
        }
    });

    context_.GetDtcSetting()->SetNotifier([this](bool const on) {
        // [SWS_DM_00088].
        // [SWS_DM_00378].
        bFreezeUpdateDtc_ = !on;
        if (debounce_ != nullptr) {
            bool const isDebounceBehaviorReset{debounce_->GetDebounceBehavior()
                                               == DiagnosticDebounceBehaviorEnum::kReset};
            if ((!on) && isDebounceBehaviorReset) {
                debounce_->Reset();
            }
            if (debounce_->GetType() == DebounceTypeEnum::kTimeBase) {
                debounce_->Freeze(!on);
            }
        }

        // [SWS_DM_00565](deleted in R21-11)
        if (on && nullptr != monitor_) {
            monitor_->InitMonitor(InitMonitorReason::kReenabled);
        }
    });

    return true;
}


bool Event::_registerDiagnosticMonitor(std::shared_ptr< Monitor > const& monitor)
{
    if (nullptr == monitor) {
        LogWarn() << "Event::RegisterDiagnosticMonitor -> monitor is null.";
        return false;
    }

    monitor_ = monitor;


    monitor_->OnRecieveAction([this](MonitorAction const& action) { return _processMonitorActionEvent(action); });

    monitor_->OnRecieveCounter([this](CounterBased const& counter) {
        if (debounce_.get() == nullptr) {
            LogDebug() << "Event::RegisterDiagnosticMonitor -> debounce_ is null.";
            return;
        }

        if (debounce_->GetType() != DebounceTypeEnum::kCounterBase) {
            LogWarn() << "Event::RegisterDiagnosticMonitor -> debounce type error "
                      << static_cast< uint8_t >(debounce_->GetType());
            return;
        }

        debounce_->SetDebounceCounterParam(counter);
    });

    monitor_->OnRecieveTimer([this](TimeBased const& timer) {
        if (debounce_.get() == nullptr) {
            LogDebug() << "Event::RegisterDiagnosticMonitor -> debounce_ is null.";
            return;
        }

        if (debounce_->GetType() != DebounceTypeEnum::kTimeBase) {
            LogWarn() << "Event::RegisterDiagnosticMonitor -> debounce type error "
                      << static_cast< uint8_t >(debounce_->GetType());
            return;
        }

        debounce_->SetDebounceTimerParam(timer);
    });
    return true;
}


bool Event::_registerDtc(std::shared_ptr< Dtc > const& dtc)
{
    if (nullptr == dtc) {
        // [SWS_DM_00165] Only consider events for which a mapping to a DTC exists
        LogWarn() << "Event::RegisterDtc -> dtc is null";
        return false;
    }

    dtc_ = dtc;
    dtc_->SetEvent(shared_from_this());
    return true;
}


bool Event::_registerEnableConditionGroup(std::shared_ptr< ConditionGroup > const& group)
{
    if (nullptr == group) {
        LogWarn() << "Event::RegisterEnableConditionGroup -> group is null.";
        return false;
    }

    enableConditionGroup_ = group;
    enableConditionGroup_->OnConditionGroupStatusChanged([this](bool const fulfilled) {
        // [SWS_DM_00564].
        if (monitor_ != nullptr) {
            InitMonitorReason reason{InitMonitorReason::kDisabled};
            if (fulfilled) {
                reason = InitMonitorReason::kReenabled;
            }
            monitor_->InitMonitor(reason);
        }

        // [SWS_DM_00881].
        // [SWS_DM_00882].
        if (!fulfilled) {
            if (debounce_ != nullptr) {
                if (debounce_->GetDebounceBehavior() == DiagnosticDebounceBehaviorEnum::kReset) {
                    debounce_->Reset();
                }

                debounce_->Freeze(true);
            }
        } else {
            if (debounce_ != nullptr) {
                debounce_->Freeze(false);
            }
        }
    });

    return true;
}


bool Event::_registerOperationCycle(std::shared_ptr< OperationCycle > const& opCycle)
{
    if (nullptr == opCycle) {
        LogWarn() << "Event::RegisterOperationCycle -> opCycle is null.";
        return false;
    }

    if (opCycle->GetOperationCycleState() == OperationCycleType::kOperationCycleStart) {
        isOperationCycleStarted_ = true;
        if (monitor_ != nullptr) {
            monitor_->InitMonitor(InitMonitorReason::kRestart);
        }
    }

    opCycle->RegisterEvent(shared_from_this());

    return true;
}


void Event::DtcClearedCallback(bool const isForbidden, bool const isFromUds)
{
    // [SWS_DM_00562].
    bool const isNotNullMonitor{monitor_ != nullptr};
    std::ignore = isFromUds;
    if (isNotNullMonitor) {
        monitor_->InitMonitor(InitMonitorReason::kClear);
    }

    maxFdcSinceLastClear_ = 0;
    if (!isForbidden) {
        eventStatusHandler_.Clear();
    } else {
        eventStatusHandler_.ClearForbidden(GetClearAllowedBehavior());
    }
    if (!indicatorLock_) {
        for (std::vector< std::shared_ptr< Indicator > >::const_iterator itr{connectedIndicators_.cbegin()};
             itr != connectedIndicators_.cend(); ++itr) {
            (*itr)->SetStatusWIR(false);
        }
    }
    // [SWS_DM_00086]
    if (debounce_ != nullptr) {
        debounce_->Reset();
    }
}


void Event::GetFaultDetectionCounterFromMonitor()
{
    if (nullptr == monitor_) {
        return;
    }

    monitor_->GetFaultDetectionCounter();
}


void Event::ReportOperationCycleStateTransition(OperationCycleType const oldState, OperationCycleType const newState)
{
    std::ignore = oldState;
    bool const isNewStateStart{newState == OperationCycleType::kOperationCycleStart};
    if (monitor_ != nullptr && isNewStateStart) {  // [SWS_DM_00563]
        monitor_->InitMonitor(InitMonitorReason::kRestart);
    }

    if (!isOperationCycleStarted_) {
        if (newState == OperationCycleType::kOperationCycleStart) {
            // start operation cycle here
            LogVerbose() << "Event::ReportOperationCycleStateTransition -> oc start. newState="
                         << static_cast< std::uint32_t >(newState) << ", event_id =" << GetID();
            isOperationCycleStarted_ = true;
            maxFdcDuringCurCycle_    = 0;

            eventStatusHandler_.OpCycleStartedStatusChange();
            dtc_->OpCycleStartedStatusChange();
            // [SWS_DM_00039]
            if (debounce_ != nullptr) {
                debounce_->Reset();
            }
        }
    } else {
        if (newState == OperationCycleType::kOperationCycleStart) {
            // RESTART
            LogVerbose() << "Event::ReportOperationCycleStateTransition -> oc restart. newState="
                         << static_cast< std::uint32_t >(newState) << ", event_id =" << GetID();
            if (eventStatusHandler_.GetStatus().Get(EventStatusBit::kTestFailedThisOperationCycle)) {
                ++failedCycleCounter_;
            }
            if (isFailedFirst_) {
                ++cycleCounterSinceFirstFailed_;
            }
            if (isFailedLast_) {
                ++cycleCounterSinceLastFailed_;
            }
            eventStatusHandler_.OpCycleStoppedStatusChange();
            dtc_->OpCycleStoppedStatusChange();
            eventStatusHandler_.OpCycleStartedStatusChange();
            dtc_->OpCycleStartedStatusChange();

            isOperationCycleStarted_ = true;
            maxFdcDuringCurCycle_    = 0;
            // [SWS_DM_00039]
            if (debounce_ != nullptr) {
                debounce_->Reset();
            }
        } else {
            // Stopped
            LogVerbose() << "Event::ReportOperationCycleStateTransition -> oc stop. newState="
                         << static_cast< std::uint32_t >(newState) << ", event_id =" << GetID();
            isOperationCycleStarted_ = false;
            if (eventStatusHandler_.GetStatus().Get(EventStatusBit::kTestFailedThisOperationCycle)) {
                ++failedCycleCounter_;
            }
            if (isFailedFirst_) {
                ++cycleCounterSinceFirstFailed_;
            }
            if (isFailedLast_) {
                ++cycleCounterSinceLastFailed_;
            }

            eventStatusHandler_.OpCycleStoppedStatusChange();
            dtc_->OpCycleStoppedStatusChange();
        }
    }
}


void Event::SetDebouncedStatus(MonitorAction const actionType)
{
    if (dtc_.get() == nullptr) {
        LogWarn() << "Event::SetDebouncedStatus -> dtc is nullptr"
                  << "event_id =" << GetID();
        return;
    }
    LogVerbose() << __func__ << " -> enter. action=" << Helper::ToString(actionType).data();
    bool const beforeTestFailed = dtc_->GetStatus().Get(DTCStatusBit::kTestFailed);
    eventStatusHandler_.SetStatus(actionType, config_.recoverableInSameOperationCycle);
    dtc_->SetStatus(actionType, config_.confirmationThreshold, indicatorLock_, !connectedIndicators_.empty(),
                    config_.recoverableInSameOperationCycle);
    if (dtc_->GetStatus().Get(DTCStatusBit::kWarningIndicatorRequested)) {
        if (actionType == MonitorAction::kFailed) {
            for (std::vector< std::shared_ptr< Indicator > >::const_iterator itr{connectedIndicators_.cbegin()};
                 itr != connectedIndicators_.cend(); ++itr) {
                (*itr)->SetStatusWIR(true);
            }
        }
    } else {
        if (!indicatorLock_) {
            for (std::vector< std::shared_ptr< Indicator > >::const_iterator itr{connectedIndicators_.cbegin()};
                 itr != connectedIndicators_.cend(); ++itr) {
                (*itr)->SetStatusWIR(false);
            }
        }
    }
    bool const afterTestFailed      = dtc_->GetStatus().Get(DTCStatusBit::kTestFailed);
    bool const isTestFailedFrom0To1 = (!beforeTestFailed && afterTestFailed);
    dtc_->CheckAndHandleStorage(actionType, isTestFailedFrom0To1);
    LogVerbose() << "Event::SetDebouncedStatus -> over. event_status=" << eventStatusHandler_.GetStatus().Get()
                 << ", dtc_status=" << dtc_->GetStatus().Get();
}


int32_t Event::_processMonitorActionEvent(MonitorAction const action)
{
    LogVerbose() << "Event::ProcessMonitorActionEvent -> enter. action=" << std::move(Helper::ToString(action))
                 << "event_id =" << GetID();
    // [SWS_DM_00567].
    if (!isOperationCycleStarted_) {
        LogWarn() << "Event::ProcessMonitorActionEvent -> oc not start"
                  << "event_id =" << GetID();
        return static_cast< int32_t >(DiagReportingErrc::kReportIgnored);
    }

    // [SWS_DM_00568]. Because enableConditionGroup information cannot be obtained in AA, the 2011 version indicates a problem, here the implementation satisfies the 2211 version
    if ((enableConditionGroup_ != nullptr) && (!enableConditionGroup_->CheckConditionGroup())) {
        LogWarn() << "Event::ProcessMonitorActionEvent -> condition group is not fulled"
                  << "event_id =" << GetID();
        return static_cast< int32_t >(DiagReportingErrc::kReportIgnored);
    }

    if (bFreezeUpdateDtc_) {
        LogInfo() << "Event::ProcessMonitorActionEvent -> 0x85 service, switch off updating of dtc."
                  << "event_id =" << GetID();
        return static_cast< int32_t >(DiagReportingErrc::kGenericError);
    }

    switch (action) {
        case MonitorAction::kPassed:
        case MonitorAction::kFailed: {
            if (debounce_ != nullptr) {
                debounce_->IndicateReportedStatus(action);
            } else {
                SetDebouncedStatus(action);
            }
        } break;
        case MonitorAction::kPrepassed:
        case MonitorAction::kPrefailed: {
            if (nullptr == debounce_) {
                // [SWS_DM_00874].
                LogInfo() << "Event::ProcessMonitorActionEvent -> event debounce not config, action="
                          << static_cast< uint32_t >(action) << "event_id=" << GetID();
                return static_cast< int32_t >(DiagReportingErrc::kDebouncingConfigurationInconsistent);
            }

            debounce_->IndicateReportedStatus(action);
        } break;
        case MonitorAction::kResetDebouncing: {
            if (nullptr == debounce_) {
                LogInfo() << "Event::ProcessMonitorActionEvent -> event debounce not config, action="
                          << static_cast< uint32_t >(action) << "event_id=" << GetID();
                return static_cast< int32_t >(DiagReportingErrc::kDebouncingConfigurationInconsistent);
            }

            debounce_->Reset();
        } break;
        case MonitorAction::kResetTestFailed: {  // [SWS_DM_01026].
            // Rationale: This is an AUTOSAR-specific additional reset condition for the ’kTest-Failed’
            // bit of the Event status bits.
            // According to the original description of the requirement, the reset here is only for event. Many places in the document [SWS_DM_01024] describe that event state and dtc
            // state should be handled the same way, so dtc state is also processed the same way here.
            eventStatusHandler_.Reset();
            dtc_->ResetStatus();  // TODO(Ju Kai): To be confirmed whether to perform reset processing on DTC
            break;
        }
        case MonitorAction::kFdcThresholdReached: {
            dtc_->CheckAndHandleStorage(action, false);
            break;
        }
        case MonitorAction::kFreezeDebouncing: {
            if (nullptr == debounce_) {
                LogInfo() << "Event::ProcessMonitorActionEvent -> event debounce not config, action="
                          << static_cast< uint32_t >(action) << "event_id=" << GetID();
                return static_cast< int32_t >(DiagReportingErrc::kDebouncingConfigurationInconsistent);
            }
            // [SWS_DM_00880].
            if (debounce_->GetType() == DebounceTypeEnum::kTimeBase) {
                debounce_->IndicateReportedStatus(action);
            }
            break;
        }
        default: {
            break;
        }
    }

    return 0;
}


void Event::ProcessEndOfHealingCycle(std::shared_ptr< Indicator > const& indicator)
{
    if (dtc_.get() == nullptr) {
        LogWarn() << "Event::ProcessEndOfHealingCycle|dtc_ is nullptr";
        return;
    }
    // [SWS_DM_00224].
    LogVerbose() << "Event::ProcessEndOfHealingCycle|kTestFailedThisOperationCycle ="
                 << dtc_->GetStatus().Get(DTCStatusBit::kTestFailedThisOperationCycle)
                 << "kWarningIndicatorRequested =" << dtc_->GetStatus().Get(DTCStatusBit::kWarningIndicatorRequested)
                 << "indicator_lock =" << indicatorLock_;
    if (!dtc_->GetStatus().Get(DTCStatusBit::kTestFailedThisOperationCycle)  // not failed
        && dtc_->GetStatus().Get(DTCStatusBit::kWarningIndicatorRequested)
        // no user controlled WIR-bit request
        && (!indicatorLock_)) {
        if (indicator->CheckThreshold(
                eventStatusHandler_.GetStatus().Get(EventStatusBit::kTestFailedThisOperationCycle))) {
            dtc_->SetStatusWIR(false);
            indicator->SetStatusWIR(false);
        }
    }
}


std::uint8_t Event::GetFailedCycles() noexcept { return failedCycleCounter_.load(); }


std::uint8_t Event::GetCyclesSinceFirstFailed() noexcept { return cycleCounterSinceFirstFailed_.load(); }


std::uint8_t Event::GetCyclesSinceLastFailed() noexcept { return cycleCounterSinceLastFailed_.load(); }


std::int8_t Event::GetMaxFDCDuringCurrentCycle() noexcept { return maxFdcDuringCurCycle_.load(); }


std::int8_t Event::GetMaxFDCSinceLastClear() noexcept { return maxFdcSinceLastClear_.load(); }


std::uint32_t Event::GetDTCValue() noexcept { return dtc_->GetValue(); }


DebounceTypeEnum Event::GetDebounceType() noexcept
{
    if (debounce_ != nullptr) {
        return debounce_->GetType();
    }
    return DebounceTypeEnum::kUnknown;
}


bool Event::IsStorageCounter() noexcept
{
    if (debounce_ != nullptr) {
        return debounce_->IsStorageCounter();
    }
    return false;
}


DebouncingState Event::GetDebouncingStatus() noexcept
{
    if (nullptr == debounce_) {
        return DebouncingState::kNeutral;
    }
    return debounce_->GetDouncingState();
}


int8_t Event::GetFaultDetectionCounter() noexcept
{
    if (nullptr == debounce_) {
        return 0;
    }
    return debounce_->GetFaultDetectionCounter();
}


bool Event::GetTestComplete() noexcept
{
    int8_t const count{GetFaultDetectionCounter()};
    return ((kFdcMin == count) || (kFdcMax == count));
}


bool Event::IsFilledFdcThresholdStorage() noexcept
{
    if (nullptr == debounce_) {
        return false;
    }
    return debounce_->IsFilledFdcThresholdStorage();
}

bool Event::IsDtcSuppressionOn() noexcept { return dtc_->IsDtcSuppressionOn(); }
}  // namespace server
}  // namespace uds
}  // namespace isoft
