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
/// @file       dtc.cpp
/// @brief      This file implements dtc functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=fault_management
/// @trace_id_sr=7b479965-8c5d-4c12-b97a-545fab6938e1
/// @unit_name=dtc
/// @unit_description=This file provides dtc functionality
/// @interface_level=unit
/// @endcode
///
/// ================================================================

#include "fault_management/dtc.h"

#include "common/log.h"
#include "fault_management/aging.h"
#include "fault_management/condition_group.h"
#include "fault_management/control_dtc_setting.h"
#include "fault_management/data_identifier_set.h"
#include "fault_management/event.h"
#include "fault_management/extended_data_record.h"
#include "fault_management/fault_management_runtime_context.h"
#include "fault_management/freeze_frame.h"
#include "fault_management/memory_destination.h"
#include "fault_management/memory_entry.h"
#include "fault_management/types.h"
#include "isoft/uds/type_int.h"

namespace isoft {
namespace uds {
namespace server {


bool DtcProps::Init(DtcPropsConfig& config,
                    std::map< uint32_t, std::shared_ptr< FreezeFrame > >& ffMap,
                    std::map< uint32_t, std::shared_ptr< ExtendedDataRecord > >& edrMap,
                    std::map< uint32_t, std::shared_ptr< Aging > >& agings,
                    std::map< uint32_t, std::shared_ptr< DataIdentifierSet > >& didSets,
                    std::map< uint32_t, std::shared_ptr< MemoryDestination > >& memDestinations)
{
    for (auto const& memId : config.memoryDestination) {
        std::map< uint32_t, std::shared_ptr< MemoryDestination > >::const_iterator const itr{
            std::move(memDestinations.find(memId))};
        if (itr == memDestinations.end()) {
            LogWarn() << "DtcProps::Init -> no exist memoryDestination id =" << memId
                      << "props_id =" << config.dtcPropsID;
            continue;
        }

        memoryDestinations.push_back(itr->second);
    }

    // [SWS_DM_CONSTR_00084]
    if (memoryDestinations.empty()) {
        LogWarn() << "DtcProps::Init -> no destination be config props_id =" << config.dtcPropsID;
        return false;
    }

    // [SWS_DM_00237]
    if (config.agingID != UINT32_MAX) {
        std::map< uint32_t, std::shared_ptr< Aging > >::const_iterator const itr{
            std::move(agings.find(config.agingID))};
        if (itr == agings.end()) {
            LogWarn() << "DtcProps::Init -> config aging but no find aging id props_id =" << config.dtcPropsID
                      << "aging_id =" << config.agingID;
            return false;
        }
        aging = itr->second;
    } else {
        LogDebug() << "DtcProps::Init -> no config aging props_id =" << config.dtcPropsID;
    }

    immediateNvDataStorage = config.immediateNvDataStorage;

    maxNumberFreezeFrameRecords = config.maxNumberFreezeFrameRecords;

    priority = config.priority;

    significance = config.significance;

    std::map< uint32_t, std::shared_ptr< DataIdentifierSet > >::const_iterator const itrContent{
        std::move(didSets.find(config.snapshotRecordContent))};
    if (itrContent != didSets.end()) {
        snapshotRecordContent = itrContent->second;
    } else {
        LogDebug() << "DtcProps::Init -> no config snapshotRecordContent props_id =" << config.dtcPropsID
                   << "snapshot_record_content_id =" << config.snapshotRecordContent;
    }

    for (auto const& edr : config.extendedDataRecord) {
        std::map< uint32_t, std::shared_ptr< ExtendedDataRecord > >::const_iterator const itrEdr{
            std::move(edrMap.find(edr))};
        if (itrEdr != edrMap.end()) {
            extendedDataRecords.push_back(itrEdr->second);
        } else {
            LogWarn() << "DtcProps::Init -> config edr but no find id props_id =" << config.dtcPropsID
                      << "edr_id =" << edr;
        }
    }
    if (extendedDataRecords.empty()) {
        LogWarn() << "DtcProps::Init -> no config extendedDataRecords.";
    }

    for (auto const& ff : config.freezeFrame) {
        std::map< uint32_t, std::shared_ptr< FreezeFrame > >::const_iterator const itrFf{std::move(ffMap.find(ff))};
        if (itrFf != ffMap.end()) {
            freezeFrames.push_back(itrFf->second);
        } else {
            LogWarn() << "DtcProps::Init -> config ff but no find id props_id =" << config.dtcPropsID
                      << "ff_id =" << ff;
        }
    }
    if (freezeFrames.empty()) {
        LogWarn() << "DtcProps::Init -> no freezeFrames.";
    }

    return true;
}


Dtc::Dtc(FaultManagementRuntimeContext const& context) : context_{context} {}


bool Dtc::Init(DtcConfig const& config,
               std::map< uint32_t, std::shared_ptr< DtcProps > > const& props,
               std::map< uint32_t, std::shared_ptr< ConditionGroup > > const& clearConditionGroups)
{
    severity_ = config.severity;

    dtcValue_ = config.dtcValue;

    if (config.clearConditionGroupId != UINT32_MAX) {
        _registerClearCondtionGroup(clearConditionGroups.find(config.clearConditionGroupId)->second);
    } else {
        LogInfo() << "Dtc::Init -> not config ClearConditionGroup dtc=" << dtcValue_;
    }

    if (config.dtcPropsId == UINT32_MAX) {
        LogWarn() << "Dtc::Init -> no config dtcProps dtcValue =" << dtcValue_;
        return false;
    }
    std::map< uint32_t, std::shared_ptr< DtcProps > >::const_iterator const itrMap{props.find(config.dtcPropsId)};
    if (itrMap == props.end()) {
        LogWarn() << "Dtc::Init -> no find dtcProps dtcValue =" << dtcValue_ << "dtc_props_id =" << config.dtcPropsId;
        return false;
    }
    props_ = itrMap->second;
    _registerAgingCycle();

    dtcStatusHandler_.SetNotifier([this](DTCStatus const statusOld, DTCStatus const statusNew) {
        for (std::vector< std::shared_ptr< MemoryDestination > >::const_iterator itr{
                 props_->memoryDestinations.cbegin()};
             itr != props_->memoryDestinations.cend(); ++itr) {
            if ((*itr).get() == nullptr) {
                continue;
            }
            /// 2311 [SWS_DM_01297]
            if (!IsDtcSuppressionOn()) {
                (*itr)->NotifyDtcStatus(GetValue(), statusOld.Get(), statusNew.Get());
            }
            std::shared_ptr< MemoryEntry > const entry{(*itr)->GetEntry(dtcValue_)};
            if (nullptr == entry) {
                continue;
            }

            // Event occurrence counter increment logic
            switch (context_.GetCommonProps()->GetOccurrenceCounterProcessing()) {
                // [SWS_DM_00947].
                case DiagnosticOccurrenceCounterProcessingEnum::kConfirmedDtcBit: {
                    bool const isOldConfirm{statusOld.Get(DTCStatusBit::kConfirmedDTC)};
                    bool const isNewConfirm{statusNew.Get(DTCStatusBit::kConfirmedDTC)};
                    if (DTCStatus(entry->GetDtcStatus()).Get(DTCStatusBit::kConfirmedDTC) && !isOldConfirm
                        && isNewConfirm) {
                        entry->OccurrenceCounterIncrement();
                    }
                } break;
                // [SWS_DM_00946].
                case DiagnosticOccurrenceCounterProcessingEnum::kTestFailedBit: {
                    bool const isOldFailed{statusOld.Get(DTCStatusBit::kTestFailed)};
                    bool const isNewFailed{statusNew.Get(DTCStatusBit::kTestFailed)};
                    if ((!isOldFailed) && isNewFailed) {
                        entry->OccurrenceCounterIncrement();
                    }
                } break;
                default: {
                } break;
            }
        }
    });

    return true;
}


void Dtc::SetEvent(std::shared_ptr< Event > const& event)
{
    std::unique_lock< std::mutex > const lock{mutexEvent_};
    event_ = event;
}

void Dtc::ResetStatus() noexcept { dtcStatusHandler_.Reset(); }

DTCStatus const& Dtc::GetStatus() const noexcept { return dtcStatusHandler_.GetStatus(); }


void Dtc::SetStatus(MonitorAction const actionType,
                    std::uint32_t const confirmThreshold,
                    bool const indicatorLock,
                    bool const hasIndicator,
                    bool const recoverableInSameOperationCycle) noexcept
{
    // [SWS_DM_00218].
    // ISO14229-1 Figure D.4 — DTC status bit 3 confirmedDTC logic
    // Pre-judgment is done here to facilitate unified status handling in SetStatus
    bool const isFailedThisOperationCycle{GetStatus().Get(DTCStatusBit::kTestFailedThisOperationCycle)};
    if ((MonitorAction::kFailed == actionType) && !isFailedThisOperationCycle) {
        tripCounter_++;
    }
    bool const confirmThresholdReached{tripCounter_ >= confirmThreshold};
    if (confirmThresholdReached) {
        tripCounter_ = 0U;
    }

    dtcStatusHandler_.SetStatus(actionType, confirmThresholdReached, indicatorLock, hasIndicator,
                                recoverableInSameOperationCycle);
}


void Dtc::SetStatusWIR(bool const b) { dtcStatusHandler_.SetStatusWIR(b); }

void Dtc::OpCycleStartedStatusChange() noexcept { dtcStatusHandler_.OpCycleStartedStatusChange(); }

void Dtc::OpCycleStoppedStatusChange() noexcept
{
    // [SWS_DM_00218].
    // ISO14229-1 Figure D.4 — DTC status bit 3 confirmedDTC logic
    // if (GetStatus().Get(DTCStatusBit::kTestFailedThisOperationCycle)) {.
    //     ++tripCounter_;.
    // } else
    bool const isTestNotCompletedThisOperationCycle{GetStatus().Get(DTCStatusBit::kTestNotCompletedThisOperationCycle)};
    if ((!GetStatus().Get(DTCStatusBit::kTestFailedThisOperationCycle)) && (!isTestNotCompletedThisOperationCycle)) {
        tripCounter_ = 0U;
    }
    dtcStatusHandler_.OpCycleStoppedStatusChange();
}


std::vector< std::uint8_t > Dtc::GetFreezeFrameNumberTable() noexcept
{
    std::vector< std::uint8_t > result{};
    if (props_.get() == nullptr) {
        return result;
    }
    for (auto&& ff : props_->freezeFrames) {
        std::uint8_t number{ff->GetRecordNumber()};
        result.emplace_back(number);
    }
    return result;
}


void Dtc::_processEndOfAgingCycle()
{
    // [SWS_DM_00237].
    if (nullptr == props_->aging) {
        return;
    }

    for (std::vector< std::shared_ptr< MemoryDestination > >::const_iterator itr{props_->memoryDestinations.cbegin()};
         itr != props_->memoryDestinations.cend(); ++itr) {
        if ((*itr).get() == nullptr) {
            LogWarn() << "Dtc::ProcessEndOfAgingCycle -> memory destination is nullptr. dtc=" << dtcValue_;
            continue;
        }
        std::shared_ptr< MemoryEntry > const entry{(*itr)->GetEntry(dtcValue_)};
        // [SWS_DM_00240].
        if (nullptr == entry) {
            LogVerbose() << "Dtc::ProcessEndOfAgingCycle -> no entry. dtc=" << dtcValue_
                         << ", destination=" << (*itr)->GetMemoryId();
            continue;
        }

        // [SWS_DM_00238].
        if (GetStatus().Get(DTCStatusBit::kWarningIndicatorRequested) || GetStatus().Get(DTCStatusBit::kTestFailed)) {
            entry->AgingReset();
            (*itr)->SaveEntry(entry, true);
            LogVerbose() << "Dtc::ProcessEndOfAgingCycle -> aging reset. dtc=" << dtcValue_
                         << ", destination=" << (*itr)->GetMemoryId();
            continue;
        }
        // [SWS_DM_00241].
        DTCStatus const status{GetStatus()};
        bool const isAgingRequiresTestedCycle{context_.GetCommonProps()->IsAgingRequiresTestedCycle()};
        bool const isStatusNotCompletedThisOperationCycle{
            status.Get(DTCStatusBit::kTestNotCompletedThisOperationCycle)};
        bool const isAging{(!isAgingRequiresTestedCycle) || (!isStatusNotCompletedThisOperationCycle)};
        if ((!status.Get(DTCStatusBit::kTestFailedThisOperationCycle)) && isAging) {
            // Aging counter direction is just a matter of direction, does not affect internal counter counting, so can be unified as an increment process (0 -> threshold);
            // EDR data acquisition: AGINGCTR_DOWNCNT = threshold - counter; AGINGCTR_UPCNT = counter;
            if (entry->AgingIncrement() >= props_->aging->GetThreshold()) {
                // [SWS_DM_00242].
                LogVerbose() << "Dtc::ProcessEndOfAgingCycle -> aging enter. dtc=" << dtcValue_
                             << ", destination=" << (*itr)->GetMemoryId();
                entry->AgingReset();
                bool const isResetSinceLastClear{
                    context_.GetCommonProps()->GetStatusBitHandlingTestFailedSinceLastClear()
                    == DiagnosticStatusBitHandlingTestFailedSinceLastClearEnum::kStatusBitAgingAndDisplacement};
                (*itr)->ClearFromAging(entry, isResetSinceLastClear);
            }

            (*itr)->SaveEntry(entry, true);
        }
    }
}


void Dtc::Clear(bool const isFromUds)
{
    LogVerbose() << "Dtc::Clear -> enter. dtc=" << dtcValue_;
    tripCounter_ = 0U;
    bool const isForbidden{IsForbiddenClear()};
    bool isWIRLock{false};
    std::shared_ptr< Event > event{event_.lock()};
    if (event != nullptr) {
        isWIRLock = event->IsWIRLock();
        event->DtcClearedCallback(isForbidden, isFromUds);
    }
    if (isForbidden) {
        dtcStatusHandler_.ClearForbidden(GetEventClearAllowedBehavior());
    } else {
        dtcStatusHandler_.Clear(isWIRLock);
    }
}

void Dtc::CheckAndHandleStorage(MonitorAction const actionType, bool const testFaildFrom0To1)
{
    LogVerbose() << "Dtc::CheckAndHandleStorage -> action=" << std::move(Helper::ToString(actionType))
                 << ", dtc=" << dtcValue_;
    for (std::vector< std::shared_ptr< MemoryDestination > >::const_iterator itr{props_->memoryDestinations.cbegin()};
         itr != props_->memoryDestinations.cend(); ++itr) {
        // [SWS_DM_00909].
        // [SWS_DM_00910].
        bool const isEnableDtcControl{context_.GetDtcSetting()->Get()};
        if ((*itr)->IsPrimary() && !isEnableDtcControl) {
            LogVerbose() << "Dtc::CheckAndHandleStorage -> dest is primary and DtcSetting is false."
                         << ", dtc=" << dtcValue_;
            continue;
        }

        bool alloc{false};
        bool enableAllocReletedData{false};
        switch ((*itr)->GetMemoryEntryStorageTrigger()) {
            case DiagnosticMemoryEntryStorageTriggerEnum::kConfirmed: {
                if (GetStatus().Get(DTCStatusBit::kConfirmedDTC)) {
                    alloc = true;
                }
            } break;
            case DiagnosticMemoryEntryStorageTriggerEnum::kFdcThreshold: {
                if (MonitorAction::kFdcThresholdReached == actionType) {
                    std::shared_ptr< Event > event{event_.lock()};
                    bool const isFilledFdcThresholdStorage{event->IsFilledFdcThresholdStorage()};
                    if (event != nullptr && isFilledFdcThresholdStorage) {
                        alloc                  = true;
                        enableAllocReletedData = true;
                    }
                }
            } break;
            case DiagnosticMemoryEntryStorageTriggerEnum::kPending: {
                if (GetStatus().Get(DTCStatusBit::kPendingDTC)) {
                    alloc = true;
                }
            } break;
            case DiagnosticMemoryEntryStorageTriggerEnum::kTestFailed: {
                if (GetStatus().Get(DTCStatusBit::kTestFailed)) {
                    alloc = true;
                }
            } break;
            default: {
            } break;
        }

        bool isDisplacement{false};
        std::shared_ptr< MemoryEntry > entry{nullptr};
        if (alloc) {
            DTCStatus status{GetStatus()};
            // 7.5.2.3.8 Active / Passive Status of Events
            if (!context_.GetCommonProps()->IsStatusBitStorageTestFailed()) {
                status.Reset(DTCStatusBit::kTestFailed);
            }

            LogVerbose() << "Dtc::CheckAndHandleStorage -> alloc entry destination=" << (*itr)->GetMemoryId()
                         << ", dtc=" << dtcValue_ << ", status=" << status.Get()
                         << ", severity_bit=" << _getSeverityBitValue() << ", priority=" << GetPriority();
            // Prioritize ensuring entry is allocated, then capture data
            entry = (*itr)->TryAllocEntry(dtcValue_, status.Get(), isDisplacement, GetPriority());
        } else {
            LogVerbose() << "Dtc::CheckAndHandleStorage -> get old entry destination=" << (*itr)->GetMemoryId()
                         << ", dtc=" << dtcValue_ << ", status=" << GetStatus().Get();
            // Although the capture timing is not met here, freeze frames and extended data may need to be updated
            entry = (*itr)->GetEntry(dtcValue_);
        }

        if (entry != nullptr) {
            LogVerbose() << "Dtc::CheckAndHandleStorage -> try capture related datas."
                         << ", dtc =" << dtcValue_ << ", destination =" << (*itr)->GetMemoryId();
            std::vector< SnapshotDataRecordType > ssrs;
            bool const b1{
                _checkStorageFreezeFrames(entry, actionType, (*itr), ssrs, enableAllocReletedData, testFaildFrom0To1)};
            bool const b2{_checkStorageExtendDatas(entry, actionType, (*itr), enableAllocReletedData)};
            if (IsImmediateNvDataStorage() && (b1 || b2)) {
                if (!(*itr)->SaveEntry(entry, true)) {
                    LogWarn() << "Dtc::CheckAndHandleStorage -> dest->SaveEntry failed."
                              << ", dtc =" << dtcValue_ << ", destination =" << (*itr)->GetMemoryId();
                }
            }

            if (!isDisplacement) {  // [SWS_DM_00894]
                (*itr)->NotifySnapshotRecordUpdated(ssrs, dtcValue_);
            }
        }
    }
}


bool Dtc::IsImmediateNvDataStorage() const noexcept { return props_->immediateNvDataStorage; }


bool Dtc::IsForbiddenClear() const noexcept
{
    if (nullptr == clearConditionGroup_) {
        LogWarn() << "Dtc::IsForbiddenClear -> not config ClearConditionGroup dtc=" << dtcValue_;
        return false;
    }
    return !clearConditionGroup_->CheckConditionGroup();
}


DiagnosticClearEventAllowedBehaviorEnum Dtc::GetEventClearAllowedBehavior() noexcept
{
    std::unique_lock< std::mutex > const lock{mutexEvent_};
    std::shared_ptr< Event > ep{event_.lock()};
    return ep->GetClearAllowedBehavior();
}


std::int8_t Dtc::GetCurrentFDC() noexcept
{
    std::unique_lock< std::mutex > const lock{mutexEvent_};
    std::shared_ptr< Event > event{event_.lock()};
    if (event != nullptr) {
        return event->GetFaultDetectionCounter();
    }
    return 0;
}


std::int8_t Dtc::GetMaxFDCSinceLastClear() const noexcept
{
    std::shared_ptr< Event > event{event_.lock()};
    if (event != nullptr) {
        return event->GetMaxFDCSinceLastClear();
    }
    return 0;
}


std::int8_t Dtc::GetMaxFDCDuringCurrentCycle() const noexcept
{
    std::shared_ptr< Event > event{event_.lock()};
    if (event != nullptr) {
        return event->GetMaxFDCDuringCurrentCycle();
    }
    return 0;
}


std::uint8_t Dtc::GetFailedCycles() const noexcept
{
    std::shared_ptr< Event > event{event_.lock()};
    if (event != nullptr) {
        return event->GetFailedCycles();
    }
    return 0U;
}


std::uint8_t Dtc::GetCyclesSinceFirstFailed() const noexcept
{
    std::shared_ptr< Event > event{event_.lock()};
    if (event != nullptr) {
        return event->GetCyclesSinceFirstFailed();
    }
    return 0U;
}


std::uint8_t Dtc::GetCyclesSinceLastFailed() const noexcept
{
    std::shared_ptr< Event > event{event_.lock()};
    if (event != nullptr) {
        return event->GetCyclesSinceLastFailed();
    }
    return 0U;
}


std::uint8_t Dtc::GetPriority() const noexcept { return props_->priority; }


std::uint8_t Dtc::GetSeverity() const noexcept { return _getSeverityBitValue(); }


bool Dtc::HasAging() const noexcept { return (props_->aging != nullptr); }


std::uint32_t Dtc::GetAgingThreshold() const noexcept
{
    if (props_->aging != nullptr) {
        return props_->aging->GetThreshold();
    }
    return kInt32_255U;
}


std::uint8_t Dtc::GetSignificance() const noexcept
{
    std::uint8_t val{0U};
    switch (props_->significance) {
        case DiagnosticSignificanceEnum::kOccurence: {
            val = 0U;
            break;
        }
        case DiagnosticSignificanceEnum::kFault: {
            val = 1U;
            break;
        }
        default: {
        } break;
    }
    return val;
}


std::uint32_t Dtc::GetEventID() const noexcept
{
    std::shared_ptr< Event > event{event_.lock()};
    if (event != nullptr) {
        return event->GetID();
    }
    return kInvalidEventID;
}


std::shared_ptr< ExtendedDataRecord > Dtc::GetExtendedDataRecord(uint8_t const edrNumber) noexcept
{
    if (props_.get() == nullptr) {
        LogWarn() << "Dtc::GetExtenedDataRecord -> props_ is null. dtc=" << dtcValue_;
        return nullptr;
    }

    for (auto const& entry : props_->extendedDataRecords) {
        if (entry.get() == nullptr) {
            continue;
        }

        if (entry->GetRecordNumber() != edrNumber) {
            continue;
        }

        return entry;
    }

    return nullptr;
}


void Dtc::ResetStatusTo(DTCStatus const& dtcStatus) noexcept { dtcStatusHandler_.ResetTo(dtcStatus); }

std::int32_t Dtc::SetDtcSuppression(DtcSuppressionType const type) noexcept
{
    DTCStatus dtcStatus{};
    dtcStatus = GetStatus();
    std::int32_t const kSuppressionIgnored{117U};
    std::string strType{};
    if (type == DtcSuppressionType::kDTCSuppressionOn) {
        strType = "kDTCSuppressionOn";
        if (dtcStatus.Get(DTCStatusBit::kConfirmedDTC)) {
            LogWarn() << "Dtc::SetDtcSuppression|dtc confirmed status must is false if dtc set suppression on!";
            return kSuppressionIgnored;
        }
        if (dtcStatus.Get(DTCStatusBit::kPendingDTC)) {
            LogWarn() << "Dtc::SetDtcSuppression|dtc pending status must is false if dtc set suppression on!";
            return kSuppressionIgnored;
        }
    } else {
        strType = "kDTCSuppressionOff";
    }
    suppressionType_ = type;
    LogInfo() << "Dtc::SetDtcSuppression| type:" << strType;
    return 0;
}

DtcSuppressionType Dtc::GetDtcSuppression() noexcept { return suppressionType_; }

bool Dtc::IsDtcSuppressionOn() noexcept { return suppressionType_ == DtcSuppressionType::kDTCSuppressionOn; }


void Dtc::_registerClearCondtionGroup(std::shared_ptr< ConditionGroup > const& conditionGroup) noexcept
{
    if (nullptr == conditionGroup) {
        LogWarn() << "Dtc::RegisterClearCondtionGroup -> ClearConditionGroup is null. dtc=" << dtcValue_;
        return;
    }

    clearConditionGroup_ = conditionGroup;
}


void Dtc::_registerAgingCycle()
{
    // [SWS_DM_00237].
    if (nullptr == props_->aging) {
        LogInfo() << "Dtc::RegisterAgingCycle -> no aging. dtc =" << dtcValue_;
        return;
    }
    props_->aging->SetNotifier([this]() { _processEndOfAgingCycle(); });
}

bool Dtc::_checkStorageFreezeFrames(std::shared_ptr< MemoryEntry > const& entry,
                                    MonitorAction const actionType,
                                    std::shared_ptr< MemoryDestination > const& dest,
                                    std::vector< SnapshotDataRecordType >& ssrs,
                                    bool const enableAlloc,
                                    bool const testFaildFrom0To1)
{
    if (entry.get() == nullptr) {
        LogWarn() << "Dtc::CheckStorageFreezeFrames -> entry is nullptr dtc_value =" << dtcValue_;
        return false;
    }

    if (dest.get() == nullptr) {
        LogWarn() << "Dtc::CheckStorageFreezeFrames -> dest is nullptr dtc_value =" << dtcValue_;
        return false;
    }

    if (props_.get() == nullptr) {
        LogWarn() << "Dtc::CheckStorageFreezeFrames -> props_ is nullptr dtc_value =" << dtcValue_;
        return false;
    }

    LogVerbose() << "Dtc::CheckStorageFreezeFrames -> enter. dtc =" << dtcValue_
                 << "actionType =" << static_cast< uint32_t >(actionType) << "memId=" << dest->GetMemoryId()
                 << "allocFfWay =" << static_cast< uint8_t >(dest->GetTypeOfFreezeFrameRecordNumeration())
                 << "testFaildFrom0To1 =" << testFaildFrom0To1 << "enableAlloc =" << enableAlloc;
    bool ret{false};
    switch (dest->GetTypeOfFreezeFrameRecordNumeration()) {
        case DiagnosticTypeOfFreezeFrameRecordNumerationEnum::kCalculated: {
            if (!testFaildFrom0To1) {
                return false;
            }

            if (props_->snapshotRecordContent.get() == nullptr) {
                LogWarn() << "Dtc::CheckStorageFreezeFrames -> snapshotRecordContent is nullptr dtc=" << dtcValue_
                          << ", memId=" << dest->GetMemoryId();
                return false;
            }

            /// According to 23 version SWS_DM_01276
            if (props_->maxNumberFreezeFrameRecords == 1U
                && entry->GetFFSize() >= props_->maxNumberFreezeFrameRecords) {
                LogVerbose() << "Dtc::CheckStorageFreezeFrames -> the count of freezeFrame has reached max. dtc ="
                             << dtcValue_ << "ff_size=" << entry->GetFFSize()
                             << ", max_ff=" << props_->maxNumberFreezeFrameRecords << ", memId=" << dest->GetMemoryId();
                return false;
            }

            /// According to 23 version SWS_DM_01277
            uint8_t needUpdateNum = static_cast< uint8_t >(entry->GetFFSize() + 1);
            if (props_->maxNumberFreezeFrameRecords > 1U && entry->GetFFSize() >= props_->maxNumberFreezeFrameRecords) {
                /// Only update the most recent record
                needUpdateNum = entry->GetFFSize();
            }

            SnapshotDataRecordType ssr;
            ssr.snapshotRecordNumber = needUpdateNum;
            props_->snapshotRecordContent->CaptureData(ssr.snapshotDataIdentifiers, dtcValue_);
            ssrs.emplace_back(ssr);

            std::vector< MemoryEntryFreezeRecord > ff;
            MakeStroedFFStruct(ssr, ff);
            entry->AllocFreezeFrameFrom(needUpdateNum, ff);
            LogVerbose() << "Dtc::CheckStorageFreezeFrames -> kCalculated capture ff success dtc =" << dtcValue_
                         << "ff_size=" << entry->GetFFSize() << ", max_ff=" << props_->maxNumberFreezeFrameRecords
                         << ", memId=" << dest->GetMemoryId() << "capture_size =" << ssr.snapshotDataIdentifiers.size()
                         << "needUpdateNum =" << needUpdateNum;
            ret = false == ssr.snapshotDataIdentifiers.empty();

        } break;
        case DiagnosticTypeOfFreezeFrameRecordNumerationEnum::kConfigured: {
            for (std::vector< std::shared_ptr< FreezeFrame > >::const_iterator itr{props_->freezeFrames.cbegin()};
                 itr != props_->freezeFrames.cend(); ++itr) {
                if ((*itr).get() == nullptr) {
                    continue;
                }

                bool const canTrigger{_canTrigger((*itr)->GetTrigger(), actionType)};
                if ((!enableAlloc) && (!canTrigger)) {
                    continue;
                }

                bool const isFFUpdate{(*itr)->GetUpdate()};
                std::vector< SnapshotDataRecordType > records;
                entry->FindFreezeFrameDataRecord((*itr)->GetRecordNumber(), records);
                bool isFFRHasOldDatas{!records.empty()};
                bool const needUpdate{isFFRHasOldDatas && isFFUpdate};
                if (!needUpdate && isFFRHasOldDatas) {
                    LogVerbose() << "Dtc::CheckStorageFreezeFrames -> not need capture ff dtc =" << dtcValue_
                                 << ", RecordNumerationEnum = kConfigured"
                                 << ", ff_size =" << entry->GetFFSize() << ", memId =" << dest->GetMemoryId()
                                 << "isFFUpdate =" << isFFUpdate << "isFFRHasOldDatas =" << isFFRHasOldDatas
                                 << "needUpdate =" << needUpdate;
                    continue;
                }

                SnapshotDataRecordType ssr;
                bool const isSuccess = (*itr)->CaptureSnapshotRecord(props_->snapshotRecordContent, ssr, dtcValue_);
                if (isSuccess) {
                    std::vector< MemoryEntryFreezeRecord > ff;
                    MakeStroedFFStruct(ssr, ff);
                    entry->AllocFreezeFrameFrom((*itr)->GetRecordNumber(), ff);
                    ssrs.emplace_back(ssr);
                    ret = true;

                    LogVerbose() << "Dtc::CheckStorageFreezeFrames -> kConfigured capture ff success dtc =" << dtcValue_
                                 << ", RecordNumerationEnum = kConfigured"
                                 << ", capture_size=" << ssr.snapshotDataIdentifiers.size()
                                 << ", memId=" << dest->GetMemoryId() << "ff_size =" << entry->GetFFSize();
                } else {
                    LogWarn() << "Dtc::CheckStorageFreezeFrames -> CaptureSnapshotRecord fail recordNumber ="
                              << (*itr)->GetRecordNumber() << ", dtc =" << dtcValue_
                              << ", memId =" << dest->GetMemoryId();
                }
            }
        } break;
        default: {
            LogWarn() << "Dtc::CheckStorageFreezeFrames -> unknown ff number gen way dtc_value =" << dtcValue_;
            return false;
        }
    }

    return ret;
}


bool Dtc::_checkStorageExtendDatas(std::shared_ptr< MemoryEntry > const& entry,
                                   MonitorAction const actionType,
                                   std::shared_ptr< MemoryDestination > const& dest,
                                   bool const enableAlloc)
{
    if (entry.get() == nullptr) {
        LogWarn() << "Dtc::CheckStorageExtendDatas -> entry is nullptr dtc_value =" << dtcValue_;
        return false;
    }

    if (dest.get() == nullptr) {
        LogWarn() << "Dtc::CheckStorageExtendDatas -> dest is nullptr dtc_value =" << dtcValue_;
        return false;
    }

    LogVerbose() << "Dtc::CheckStorageExtendDatas -> dest->AllocExtendedDataRecordFrom enter dtc =" << dtcValue_
                 << ", memId =" << dest->GetMemoryId();
    bool ret{false};
    for (std::vector< std::shared_ptr< ExtendedDataRecord > >::const_iterator itr{props_->extendedDataRecords.cbegin()};
         itr != props_->extendedDataRecords.cend(); ++itr) {  // [SWS_DM_00895]
        if ((*itr).get() == nullptr) {
            continue;
        }

        bool const canTrigger{_canTrigger((*itr)->GetTrigger(), actionType)};
        if ((!enableAlloc) && (!canTrigger)) {
            continue;
        }

        std::vector< DTCExtDataRecord > records;
        entry->FindExtendedDataRecord(static_cast< uint8_t >((*itr)->GetRecordNumber()), records, dest->GetMemoryId());
        bool isEDRTHasOldDatas{!records.empty()};
        bool const isEDRUpdate{(*itr)->GetUpdate()};
        bool const needUpdate{isEDRTHasOldDatas && isEDRUpdate};
        if ((!isEDRTHasOldDatas)  // No old data
            || needUpdate) {      // Update needed
            std::vector< MemoryEntryExtendedRecord > edr{};
            bool const isSuccess = (*itr)->CaptureRecord(edr, dtcValue_);
            if (isSuccess) {
                entry->AllocExtendedDataRecordFrom((*itr)->GetRecordNumber(), edr);
            }
            LogVerbose() << "Dtc::CheckStorageExtendDatas -> CaptureRecord success recordNum ="
                         << (*itr)->GetRecordNumber() << ", dtc=" << dtcValue_ << ", memId =" << dest->GetMemoryId()
                         << "capture_size =" << edr.size() << "isSuccess =" << isSuccess;
            ret = isSuccess;
        }
    }
    return ret;
}

bool Dtc::_canTrigger(DiagnosticRecordTriggerEnum const trigger, MonitorAction const actionType) const
{
    bool canTrigger{false};
    switch (trigger) {
        case DiagnosticRecordTriggerEnum::kTestFailed: {
            if (GetStatus().Get(DTCStatusBit::kTestFailed)) {
                canTrigger = true;
            }
        } break;
        case DiagnosticRecordTriggerEnum::kConfirmed: {
            if (GetStatus().Get(DTCStatusBit::kConfirmedDTC)) {
                canTrigger = true;
            }
        } break;
        case DiagnosticRecordTriggerEnum::kFdcThreshold: {
            if (MonitorAction::kFdcThresholdReached == actionType) {
                canTrigger = true;
            }
        } break;
        case DiagnosticRecordTriggerEnum::kPending: {
            if (GetStatus().Get(DTCStatusBit::kPendingDTC)) {
                canTrigger = true;
            }
        } break;
        case DiagnosticRecordTriggerEnum::kCustom: {
            // This type is determined during initialization configuration
            if (GetStatus().Get(DTCStatusBit::kTestFailed)) {
                canTrigger = true;
            }
        } break;
        case DiagnosticRecordTriggerEnum::kTestFailedThisOperationCycle: {
            if (GetStatus().Get(DTCStatusBit::kTestFailedThisOperationCycle)) {
                canTrigger = true;
            }
        } break;
        default: {
        } break;
    }
    return canTrigger;
}

/// When allocating data to destination, severity in the configuration needs to be further converted to a specific numerical value. For specific conversion details, refer to iso14229-1 D.3

std::uint8_t Dtc::_getSeverityBitValue() const noexcept
{
    // Refer to iso14229-1 D.3
    std::uint8_t val{0U};
    switch (severity_) {
        case DiagnosticUdsSeverityEnum::kNoSeverity: {
            val = 0U;
            break;
        }
        case DiagnosticUdsSeverityEnum::kCheckAtNextHalt: {
            val = 1U << kInt8_6U;
            break;
        }
        case DiagnosticUdsSeverityEnum::kImmediately: {
            val = 1U << kInt8_7U;
            break;
        }
        case DiagnosticUdsSeverityEnum::kMaintenanceOnly: {
            val = 1U << kInt8_5U;
            break;
        }
        default: {
        } break;
    }
    return val;
}


void Dtc::MakeStroedFFStruct(SnapshotDataRecordType const& in, std::vector< MemoryEntryFreezeRecord >& out)
{
    for (auto const& entry : in.snapshotDataIdentifiers) {
        MemoryEntryFreezeRecord record;
        record.did     = entry.dataIdentifier;
        record.vecData = entry.data;
        out.emplace_back(record);
    }
}

}  // namespace server
}  // namespace uds
}  // namespace isoft
