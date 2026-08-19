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
/// @file       memory_destination.cpp
/// @brief      This file implements memory_destination functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=fault_management
/// @trace_id_sr=5ceb3942-513a-4499-a6cd-7e3392ad0950
/// @unit_name=MemoryDestination
/// @unit_description=This file provides memory_destination functionality
/// @interface_level=unit
/// @endcode
///
/// ================================================================

#include "fault_management/memory_destination.h"

#include <sstream>

#include "common/log.h"
#include "fault_management/control_dtc_setting.h"
#include "fault_management/dtc.h"
#include "fault_management/dtc_group.h"
#include "fault_management/fault_management_runtime_context.h"
#include "fault_management/memory_entry.h"
#include "fault_management/status.h"
#include "isoft/uds/persistence_interface.h"
#include "isoft/uds/uds_nrc_error_domain.h"

namespace isoft {
namespace uds {
namespace server {

bool MemoryDestination::CompareEntryByTimeUpOrder(std::shared_ptr< MemoryEntry > const& item1,
                                                  std::shared_ptr< MemoryEntry > const& item2) noexcept
{
    return (item1->GetTime() < item2->GetTime());
}

bool MemoryDestination::CompareEntryByTimeDownOrder(std::shared_ptr< MemoryEntry > const& item1,
                                                    std::shared_ptr< MemoryEntry > const& item2) noexcept
{
    return (item1->GetTime() > item2->GetTime());
}


std::string MemoryDestination::_MakeDTCStroedKey(std::uint32_t const dtcValue) const noexcept
{
    std::stringstream ss;
    ss << GetClusterName() << "_" << GetMemoryId() << "_" << dtcValue;
    return ss.str();
}


std::string MemoryDestination::_MakeOperationStroedKey(std::uint32_t const operation) const noexcept
{
    std::stringstream ss;
    ss << GetClusterName() << "_" << operation;
    return ss.str();
}


bool MemoryDestination::Init(std::vector< uint32_t > const& dtcList, std::vector< uint32_t > const& operationList)
{
    if (per_.get() == nullptr) {
        LogWarn() << "MemoryDestination::Init -> no config per memId=" << GetMemoryId()
                  << "clusterName =" << GetClusterName().c_str();
        return false;
    }

    if (config_.dtcInformationInterface.get() == nullptr) {
        LogWarn() << "MemoryDestination::Init -> no config dtcInformationInterface memId=" << GetMemoryId()
                  << "clusterName =" << GetClusterName().c_str();
    } else {
        /// 0x85 service control dtc change callback notification
        _initRegisterDtcSetting();

        /// Clear dtc
        int32_t res = _initRegisterClear();
        if (res != 0) {
            LogWarn() << "MemoryDestination::Init -> InitRegisterClear trigger error memId=" << GetMemoryId()
                      << "clusterName =" << GetClusterName().c_str() << "res =" << res;
            return false;
        }

        /// Restore dtc status change
        res = _initRegisterGetNumberOfStoredEntries();
        if (res != 0) {
            LogWarn() << "MemoryDestination::Init -> InitRegisterGetNumberOfStoredEntries trigger error memId="
                      << GetMemoryId() << "clusterName =" << GetClusterName().c_str() << "res =" << res;
            return false;
        }

        /// Restore dtc status change
        res = _initRegisterGetControlDTCStatus();
        if (res != 0) {
            LogWarn() << "MemoryDestination::Init -> InitRegisterGetControlDTCStatus trigger error memId="
                      << GetMemoryId() << "clusterName =" << GetClusterName().c_str() << "res =" << res;
            return false;
        }

        /// Restore dtc status change
        res = _initRegisterEnableControlDtc();
        if (res != 0) {
            LogWarn() << "MemoryDestination::Init -> InitRegisterEnableControlDtc trigger error memId=" << GetMemoryId()
                      << "clusterName =" << GetClusterName().c_str() << "res =" << res;
            return false;
        }

        /// Restore dtc status change
        res = _initRegisterGetCurrentStatus();
        if (res != 0) {
            LogWarn() << "MemoryDestination::Init -> InitRegisterGetCurrentStatus trigger error memId=" << GetMemoryId()
                      << "clusterName =" << GetClusterName().c_str() << "res =" << res;
            return false;
        }

        /// Restore dtc status change
        res = _initRegisterGetEventMemoryOverflow();
        if (res != 0) {
            LogWarn() << "MemoryDestination::Init -> InitRegisterGetEventMemoryOverflow trigger error memId="
                      << GetMemoryId() << "clusterName =" << GetClusterName().c_str() << "res =" << res;
            return false;
        }

        res = _initRegisterGetDtcSuppression();
        if (res != 0) {
            LogWarn() << "MemoryDestination::Init -> _initRegisterGetDtcSuppression trigger error memId="
                      << GetMemoryId() << "clusterName =" << GetClusterName().c_str() << "res =" << res;
            return false;
        }
        res = _initRegisterSetDtcSuppression();
        if (res != 0) {
            LogWarn() << "MemoryDestination::Init -> _initRegisterSetDtcSuppression trigger error memId="
                      << GetMemoryId() << "clusterName =" << GetClusterName().c_str() << "res =" << res;
            return false;
        }
    }

    /// Load dtc entity data
    uint32_t const loadEntrySize = _initLoadAllEntry(dtcList);
    std::ignore                  = loadEntrySize;

    /// Load operation cycle data
    uint32_t const loadOperationSize = _initLoadAllOpenationStatus(operationList);
    std::ignore                      = loadOperationSize;

    LogInfo() << "MemoryDestination::Init -> success memId=" << GetMemoryId()
              << "clusterName =" << GetClusterName().c_str() << "dtcSize =" << dtcList.size()
              << "loadEntrySize =" << loadEntrySize << "operationSize =" << operationList.size()
              << "loadOperationSize =" << loadOperationSize;
    return true;
}


bool MemoryDestination::IsPrimary() const noexcept { return (kPrimaryDestinationId == config_.destinationID); }





void MemoryDestination::NotifyDtcStatus(std::uint32_t const dtc,
                                        std::uint8_t const oldStatus,
                                        std::uint8_t const newStatus)
{
    if (config_.dtcInformationInterface.get() == nullptr) {
        return;
    }

    DTCStatusChangedType type{};
    type.dtc          = dtc;
    type.oldValue     = oldStatus;
    type.newValue     = newStatus;
    int32_t const res = config_.dtcInformationInterface->NotifyDTCStatusChanged(type);
    LogInfo() << "MemoryDestination::NotifyDtcStatus -> trigger NotifyDTCStatusChanged memId=" << GetMemoryId()
              << "clusterName =" << GetClusterName().c_str() << "dtc =" << dtc << "oldStatus =" << oldStatus
              << "newStatus =" << newStatus << "res =" << res;
}




void MemoryDestination::NotifySnapshotRecordUpdated(std::vector< SnapshotDataRecordType > const& ssrs,
                                                    uint32_t const dtc)
{
    if (config_.dtcInformationInterface.get() == nullptr) {
        LogWarn() << "MemoryDestination::NotifySnapshotRecordUpdated|dot't config dtcInformationInterface, so won't "
                     "update, dtc = "
                  << dtc << "ssrs_size = " << ssrs.size();
        return;
    }

    if (ssrs.empty()) {
        LogDebug() << "MemoryDestination::NotifyDtcStatus -> trigger NotifySnapshotRecordUpdated memId="
                   << GetMemoryId() << "clusterName =" << GetClusterName().c_str() << "dtc =" << dtc
                   << "ssrs_size =" << ssrs.size() << "ssrs is empty, don't update to aa";
        return;
    }

    SnapshotRecordUpdatedType type;
    type.nDTC            = dtc;
    type.vecToBeReplaced = ssrs;

    int32_t const res = config_.dtcInformationInterface->NotifySnapshotRecordUpdated(type);
    LogDebug() << "MemoryDestination::NotifyDtcStatus -> trigger NotifySnapshotRecordUpdated memId=" << GetMemoryId()
               << "clusterName =" << GetClusterName().c_str() << "dtc =" << dtc << "ssrs_size =" << ssrs.size()
               << "ToBeReplaced_size =" << type.vecToBeReplaced.size() << "res =" << res;
}


void MemoryDestination::Save()
{
    {
        /// Store DTC entity data
        LogVerbose() << "MemoryDestination::Save -> enter. memId=" << GetMemoryId()
                     << "entries_size =" << entries_.size();
        std::unique_lock< std::recursive_mutex > const lock{entryMutex_};
        for (auto const& itr : entries_) {
            std::ignore = SaveEntry(itr.second);
        }
    }

    /// Store operation cycle status
    _SaveOperation();

    std::ignore = per_->SyncData();
}




std::shared_ptr< MemoryEntry > MemoryDestination::GetEntry(uint32_t const dtc)
{
    std::unique_lock< std::recursive_mutex > const lock{entryMutex_};
    std::map< uint32_t, std::shared_ptr< MemoryEntry > >::iterator iter = entries_.find(dtc);
    if (iter == entries_.end()) {
        return nullptr;
    }

    return iter->second;
}








std::shared_ptr< MemoryEntry > MemoryDestination::TryAllocEntry(uint32_t const dtc,
                                                                uint8_t const status,
                                                                bool& isDisplacement,
                                                                uint8_t const priority)
{
    std::ignore = isDisplacement;
    std::shared_ptr< MemoryEntry > entry{GetEntry(dtc)};
    if (entry != nullptr) {
        // For re-acquiring allocated storage entries, DTC status needs to be updated to ensure consistency with current memory status
        entry->SetDtcStatus(status);
        LogVerbose() << "MemoryDestination::TryAllocEntry -> return entry alloced before.";
        return entry;
    }

    std::uint32_t const count{GetCountForNumberOfEntries()};
    if (IsOverflow()) {
        if (config_.dtcInformationInterface.get() != nullptr) {
            int32_t const res = config_.dtcInformationInterface->NotifyEventMemoryOverflow(true);
            if (res != 0) {
                LogWarn() << "MemoryDestination::TryAllocEntry -> NotifyEventMemoryOverflow trigger error destination="
                          << GetMemoryId() << "res =" << res;
            }
        }
        switch (GetDisplacementStrategy()) {
            case DiagnosticEventDisplacementStrategyEnum::kNone: {
                LogVerbose() << "MemoryDestination::TryAllocEntry -> DisplacementStrategyEnum::kNone destination ="
                             << GetMemoryId();
                break;
            }
            case DiagnosticEventDisplacementStrategyEnum::kFull: {
                LogVerbose() << "MemoryDestination::TryAllocEntry -> DisplacementStrategyEnum::kFull destination ="
                             << GetMemoryId();
                isDisplacement = true;
                entry          = _DisplacementFull(priority, dtc, status);
            } break;
            case DiagnosticEventDisplacementStrategyEnum::kPrioOcc: {
                LogVerbose() << "MemoryDestination::TryAllocEntry -> DisplacementStrategyEnum::kPrioOcc destination ="
                             << GetMemoryId();
                isDisplacement = true;
                entry          = _DisplacementPrioOcc(priority, dtc, status);
            } break;
            default: {
            } break;
        }
    } else {
        LogVerbose() << "MemoryDestination::TryAllocEntry -> destinatin is not full, try alloc entry destination ="
                     << GetMemoryId() << "dtc =" << dtc << "status =" << status;
        entry = _AllocEntry(dtc, status);
    }

    std::uint32_t const countNew{GetCountForNumberOfEntries()};
    if ((count != countNew) && (config_.dtcInformationInterface.get() != nullptr)) {
        std::ignore = config_.dtcInformationInterface->NotifyNumberOfStoredEntries(countNew);
    }
    return entry;
}




bool MemoryDestination::SaveEntry(std::shared_ptr< MemoryEntry > const& entry, bool const isFellToDb)
{
    if (per_.get() == nullptr) {
        LogWarn() << "MemoryDestination::SaveEntry -> no config per destination=" << GetMemoryId();
        return false;
    }

    if (entry.get() == nullptr) {
        LogWarn() << "MemoryDestination::SaveEntry -> entry is nullptr destination=" << GetMemoryId();
        return false;
    }

    std::string const key = _MakeDTCStroedKey(entry->GetDtcValue());

    std::vector< uint8_t > bytes;
    entry->Serialize(bytes);

    bool res = per_->SaveData(key, bytes);
    if (!res) {
        LogWarn() << "MemoryDestination::SaveEntry -> SaveData trigger error destination=" << GetMemoryId()
                  << ", entry.dtc=" << entry->GetDtcValue() << ", entry.status=" << entry->GetDtcStatus()
                  << "key =" << key.c_str();
        return false;
    }

    if (isFellToDb) {
        per_->SyncData();
    }

    LogVerbose() << "MemoryDestination::SaveEntry -> enter. destination=" << GetMemoryId()
                 << ", entry.dtc=" << entry->GetDtcValue() << ", entry.status=" << entry->GetDtcStatus()
                 << "key =" << key.c_str() << "byteSize =" << bytes.size();
    return true;
}

void MemoryDestination::_SaveOperation()
{
    if (!IsPrimary()) {
        return;
    }

    LogVerbose() << "MemoryDestination::_SaveOperation -> enter. memId=" << GetMemoryId()
                 << "operation_size =" << ocAndStates_.size();
    if (per_.get() == nullptr) {
        LogWarn() << "MemoryDestination::_SaveOperation -> no config per destination=" << GetMemoryId();
        return;
    }

    std::unique_lock< std::recursive_mutex > const lock{operationMutex_};
    for (auto const& itr : ocAndStates_) {
        std::string const key = _MakeOperationStroedKey(itr.first);

        std::vector< uint8_t > opBytes;
        int32_t const serializeRes = isoft::serialize::Serialize(opBytes, itr.second);
        if (serializeRes < 0) {
            LogWarn() << "MemoryDestination::_SaveOperation -> Serialize error destination =" << GetMemoryId()
                      << ", operation =" << itr.first << "status =" << itr.second << "serializeRes =" << serializeRes;
            continue;
        }

        bool res = per_->SaveData(key, opBytes);
        if (!res) {
            LogWarn() << "MemoryDestination::_SaveOperation -> SaveData trigger error destination =" << GetMemoryId()
                      << ", operation =" << itr.first << "status =" << itr.second;
        }
    }
}


void MemoryDestination::ClearFromAging(std::shared_ptr< MemoryEntry > const& entry, bool const isResetSinceLastClear)
{
    if (nullptr == entry) {
        LogWarn() << "MemoryDestination::ClearFromAging -> entry is nullptr destination=" << GetMemoryId();
        return;
    }

    std::unique_lock< std::mutex > const lock{clearMutex_};
    std::shared_ptr< Dtc > dtcPtr = context_.GetDtcByValue(entry->GetDtcValue());
    if (dtcPtr.get() == nullptr) {
        LogWarn() << "MemoryDestination::ClearFromAging -> entry is nullptr destination=" << GetMemoryId()
                  << "dtc =" << entry->GetDtcValue();
        return;
    }
    // [SWS_DM_00243].
    // ISO14229-1 Figure D.6 — DTC status bit 5 testFailedSinceLastClear logic - OperationCycleChange [vehicle
    // manufacturer specific aging threshold satisfied]
    DTCStatus status{entry->GetDtcStatus()};
    status.Reset(DTCStatusBit::kConfirmedDTC);

    // The dependency for resetting these two status bits to zero is the description of DTC status in test case STS_DIAG_00014
    status.Reset(DTCStatusBit::kTestFailedThisOperationCycle);
    status.Reset(DTCStatusBit::kPendingDTC);
    if (isResetSinceLastClear) {
        status.Reset(DTCStatusBit::kTestFailedSinceLastClear);
    }

    if (!_ClearEntry(entry->GetDtcValue(), false)) {
        LogWarn() << "MemoryDestination::ClearFromAging -> _ClearEntry fail destination=" << GetMemoryId()
                  << "dtc =" << entry->GetDtcValue();
    }
    entry->SetDtcStatus(status.Get());

    // update to memory
    dtcPtr->ResetStatusTo(status);

    std::ignore = SaveEntry(entry);

    std::ignore = per_->SyncData();

    LogVerbose() << "MemoryDestination::ClearFromAging -> enter. dtc=" << entry->GetDtcValue()
                 << ", isResetSinceLastClear=" << isResetSinceLastClear << ", memId=" << GetMemoryId();
}


int32_t MemoryDestination::Clear(std::shared_ptr< DtcGroup > const& group, bool const isFromUds)
{
    LogVerbose() << "MemoryDestination::Clear -> clear group enter. isFromUds=" << isFromUds
                 << "memId =" << GetMemoryId();
    if (nullptr == group) {
        LogWarn() << "MemoryDestination::Clear -> group is null."
                  << "memId =" << GetMemoryId();
        /// SWS_DM_00092
        return isFromUds ? static_cast< int32_t >(NrcErrc::kRequestOutOfRange)
                         : static_cast< int32_t >(DiagErrc::kWrongDtc);
    }

    std::unique_lock< std::mutex > lock{clearMutex_, std::defer_lock};
    if (!lock.try_lock()) {
        LogWarn() << "MemoryDestination::Clear -> busy now."
                  << "memId =" << GetMemoryId();
        /// SWS_DM_00114
        return isFromUds ? static_cast< int32_t >(NrcErrc::kConditionsNotCorrect)
                         : static_cast< int32_t >(DiagErrc::kBusy);
    }
    group->Clear(isFromUds);
    int32_t err = 0;
    // [SWS_DM_00901].
    if (group->IsForbiddenClear()) {
        LogVerbose() << "MemoryDestination::Clear -> clear forbidde group. group=" << group->GetGroupNumber()
                     << "memId =" << GetMemoryId();
        err = (isFromUds ? static_cast< int32_t >(NrcErrc::kConditionsNotCorrect)
                         : static_cast< int32_t >(DiagErrc::kFailed));
        for (std::map< uint32_t, std::shared_ptr< Dtc > >::const_iterator itr{group->Begin()}; itr != group->End();
             ++itr) {
            _ClearEntryForbiddenStatus(GetEntry(itr->second->GetValue()), itr->second->GetEventClearAllowedBehavior());
        }
   
    } else if (group->IsAllGroup()) {
        LogVerbose() << "MemoryDestination::Clear -> clear all group. group=" << group->GetGroupNumber()
                     << "memId =" << GetMemoryId();
        _DeleteAllEntry();
        if (config_.dtcInformationInterface.get() != nullptr) {
            std::ignore = config_.dtcInformationInterface->NotifyEventMemoryOverflow(false);
        }
    } else {
        LogVerbose() << "MemoryDestination::Clear -> clear normal group. group=" << group->GetGroupNumber()
                     << "memId =" << GetMemoryId();
        for (std::map< uint32_t, std::shared_ptr< Dtc > >::const_iterator itr{group->Begin()}; itr != group->End();
             ++itr) {
            if (itr->second->IsForbiddenClear()) {
                _ClearEntryForbiddenStatus(GetEntry(itr->second->GetValue()),
                                           itr->second->GetEventClearAllowedBehavior());
            } else {
                if (!_ClearEntry(itr->second->GetValue(), true)) {
                    /// bug AP-3684, after dtc displacement and then clearing dtc group, returning 111 error code, should not return error
                    // /// SWS_DM_00115
                    // err = (isFromUds ? static_cast< int32_t >(NrcErrc::kGeneralProgrammingFailure)
                    //                  : static_cast< int32_t >(DiagErrc::kMemoryError));
                    LogVerbose() << "MemoryDestination::Clear -> clear invalid, dtc may no longer exist, group="
                                 << group->GetGroupNumber() << "memId =" << GetMemoryId()
                                 << "dtc =" << itr->second->GetValue();
                }
            }
        }
    }

    if (err == 0) {
        Save();
    }

    LogVerbose() << "MemoryDestination::Clear -> Clear group finished"
                 << "group=" << group->GetGroupNumber() << ", memId=" << GetMemoryId();
    return err;
}


int32_t MemoryDestination::Clear(std::shared_ptr< Dtc > const& dtc, bool const isFromUds)
{
    if (nullptr == dtc) {
        LogWarn() << "MemoryDestination::Clear -> dtc is null."
                  << "memId =" << GetMemoryId();
        /// SWS_DM_00092
        return isFromUds ? static_cast< int32_t >(NrcErrc::kRequestOutOfRange)
                         : static_cast< int32_t >(DiagErrc::kWrongDtc);
    }
    LogVerbose() << "MemoryDestination::Clear -> clear dtc enter. isFromUds=" << isFromUds
                 << ", dtc=" << dtc->GetValue() << "memId =" << GetMemoryId();
    std::unique_lock< std::mutex > lock{clearMutex_, std::defer_lock};
    if (!lock.try_lock()) {
        LogVerbose() << "MemoryDestination::Clear -> busy."
                     << ", dtc=" << dtc->GetValue() << "memId =" << GetMemoryId();
        /// SWS_DM_00114
        return isFromUds ? static_cast< int32_t >(NrcErrc::kConditionsNotCorrect)
                         : static_cast< int32_t >(DiagErrc::kBusy);
    }

    if (dtc->IsDtcSuppressionOn()) {
        LogVerbose() << "MemoryDestination::Clear -> suppression."
                     << ", dtc=" << dtc->GetValue() << "memId =" << GetMemoryId();
        /// SWS_DM_01300 & SWS_DM_01303
        return isFromUds ? static_cast< int32_t >(NrcErrc::kRequestOutOfRange)
                         : static_cast< int32_t >(DiagErrc::kWrongDtc);
    }

    dtc->Clear(isFromUds);

    int32_t err = 0;
    // [SWS_DM_00901].
    if (dtc->IsForbiddenClear()) {
        err = (isFromUds ? static_cast< int32_t >(NrcErrc::kConditionsNotCorrect)
                         : static_cast< int32_t >(DiagErrc::kFailed));
        _ClearEntryForbiddenStatus(GetEntry(dtc->GetValue()), dtc->GetEventClearAllowedBehavior());
    } else {
        if (!_ClearEntry(dtc->GetValue(), true)) {
            /// SWS_DM_00115
            err = (isFromUds ? static_cast< int32_t >(NrcErrc::kGeneralProgrammingFailure)
                             : static_cast< int32_t >(DiagErrc::kMemoryError));
        }
    }

    if (0 == err) {
        std::ignore = SaveEntry(GetEntry(dtc->GetValue()));
        std::ignore = per_->SyncData();
    }
    LogVerbose() << "MemoryDestination::Clear -> Clear dtc finished"
                 << "dtc=" << dtc->GetValue() << ", memId=" << GetMemoryId() << "res =" << static_cast< uint32_t >(err);
    return err;
}




void MemoryDestination::AddOperationCycleState(uint32_t const idOc, uint8_t const state)
{
    std::unique_lock< std::recursive_mutex > const lock{operationMutex_};
    ocAndStates_[idOc] = state;
}






bool MemoryDestination::GetNumberOfDTCByStatusMask(std::uint8_t dtcStatusMask,
                                                   std::uint8_t& dtcStatusAvailabilityMask,
                                                   std::uint8_t& dtcFormatIdentifier,
                                                   std::uint16_t& dtcCount) noexcept
{
    std::ignore = dtcStatusMask;
    std::ignore = dtcStatusAvailabilityMask;
    std::ignore = dtcFormatIdentifier;
    std::ignore = dtcCount;
    return false;
}






bool MemoryDestination::GetDTCByStatusMask(std::uint8_t dtcStatusMask,
                                           std::uint8_t& dtcStatusAvailabilityMask,
                                           std::vector< DTCAndStatusRecord >& dtcAndStatusRecords)
{
    LogVerbose() << "MemoryDestination::GetDTCByStatusMask from uds enter. dtcStatusMask=" << dtcStatusMask
                 << "mem_id =" << GetMemoryId() << "clusterName" << GetClusterName().c_str();
    std::unique_lock< std::recursive_mutex > const lock{entryMutex_};
    dtcAndStatusRecords.clear();

    std::ignore               = dtcStatusAvailabilityMask;
    dtcStatusAvailabilityMask = GetDtcStatusAvailabilityMask();

    std::vector< std::shared_ptr< MemoryEntry > > entries;
    for (auto const& itr : entries_) {
        if (itr.second.get() == nullptr) {
            continue;
        }

        std::shared_ptr< Dtc > dtcPtr{context_.GetDtcByValue(itr.second->GetDtcValue())};
        if (dtcPtr.get() == nullptr) {
            continue;
        }
        if (dtcPtr->IsDtcSuppressionOn()) {
            continue;
        }

        uint8_t const status{itr.second->GetDtcStatus()};
        if (0U != (status & dtcStatusMask & dtcStatusAvailabilityMask)) {
            entries.emplace_back(itr.second);
        }
    }

    if (entries.empty()) {
        LogVerbose() << "MemoryDestination::GetDTCByStatusMask|no match entry dtcStatusMask=" << dtcStatusMask
                     << "mem_id =" << GetMemoryId() << "clusterName" << GetClusterName().c_str();
        return true;
    }

    DTCStatus const statusMaskofDTC{dtcStatusMask};
    if (context_.GetCommonProps()->IsResetConfirmedBitOnOverflow()) {
        bool const isStatusPendingDTC{statusMaskofDTC.Get(DTCStatusBit::kPendingDTC)};
        if (statusMaskofDTC.Get(DTCStatusBit::kConfirmedDTC) || isStatusPendingDTC) {
            if (!statusMaskofDTC.IsSetOther(DTCStatusBit::kConfirmedDTC, DTCStatusBit::kPendingDTC)) {
                std::sort(entries.begin(), entries.end(), CompareEntryByTimeDownOrder);
            }
        }
    }
    for (std::vector< std::shared_ptr< MemoryEntry > >::const_iterator itr{entries.cbegin()}; itr != entries.cend();
         ++itr) {
        DTCAndStatusRecord record{};
        record.status   = (*itr)->GetDtcStatus();
        record.dtcValue = (*itr)->GetDtcValue();
        dtcAndStatusRecords.emplace_back(record);
        std::ignore = record;
    }
    return true;
}







bool MemoryDestination::GetDTCSnapshotRecordByDTCNumber(std::uint32_t dtcMaskRecord,
                                                        std::uint8_t ssrMumber,
                                                        DTCAndStatusRecord& dtcAndStatusRecord,
                                                        std::vector< SnapshotDataRecordType >& records)
{
    LogVerbose() << "MemoryPrimary::GetDTCSnapshotRecordByDTCNumber from uds enter. dtcMaskRecord=" << dtcMaskRecord
                 << ", ssrMumber=" << ssrMumber << "memId =" << GetMemoryId() << "clusterName"
                 << GetClusterName().c_str();

    std::shared_ptr< MemoryEntry > entry{GetEntry(dtcMaskRecord)};
    if (entry.get() == nullptr) {
        LogVerbose() << "MemoryPrimary::GetDTCSnapshotRecordByDTCNumber no entry stroed dtcMaskRecord=" << dtcMaskRecord
                     << ", ssrMumber=" << ssrMumber << "memId =" << GetMemoryId() << "clusterName"
                     << GetClusterName().c_str();
        return false;
    }

    std::shared_ptr< Dtc > dtcPtr{context_.GetDtcByValue(dtcMaskRecord)};
    if (dtcPtr.get() == nullptr) {
        LogWarn() << "MemoryPrimary::GetDTCSnapshotRecordByDTCNumber|GetDtcByValue fail, dtcMaskRecord="
                  << dtcMaskRecord << ", ssrMumber=" << ssrMumber << "memId =" << GetMemoryId() << "clusterName"
                  << GetClusterName().c_str();
        return false;
    }

    if (dtcPtr->IsDtcSuppressionOn()) {
        LogVerbose() << "MemoryPrimary::GetDTCSnapshotRecordByDTCNumber|dtc is suppression, dtcMaskRecord="
                     << dtcMaskRecord << ", ssrMumber=" << ssrMumber << "memId =" << GetMemoryId() << "clusterName"
                     << GetClusterName().c_str();
        return false;
    }

    dtcAndStatusRecord.dtcValue = dtcMaskRecord;
    dtcAndStatusRecord.status   = entry->GetDtcStatus();

    entry->FindFreezeFrameDataRecord(ssrMumber, records);
    return true;
}







bool MemoryDestination::GetDTCExtDataRecordByDTCNumber(std::uint32_t dtcMaskRecord,
                                                       std::uint8_t edrNumber,
                                                       DTCAndStatusRecord& dtcAndStatusRecord,
                                                       std::vector< DTCExtDataRecord >& records)
{
    LogVerbose() << "MemoryPrimary::GetDTCExtDataRecordByDTCNumber from uds enter. dtcMaskRecord=" << dtcMaskRecord
                 << ", edrNumber=" << edrNumber << "memId =" << GetMemoryId() << "clusterName"
                 << GetClusterName().c_str();

    std::shared_ptr< MemoryEntry > entry{GetEntry(dtcMaskRecord)};
    if (entry.get() == nullptr) {
        LogVerbose() << "MemoryPrimary::GetDTCExtDataRecordByDTCNumber no entry stroed dtcMaskRecord=" << dtcMaskRecord
                     << ", edrNumber=" << edrNumber << "memId =" << GetMemoryId() << "clusterName"
                     << GetClusterName().c_str();
        return false;
    }

    std::shared_ptr< Dtc > dtcPtr{context_.GetDtcByValue(dtcMaskRecord)};
    if (dtcPtr.get() == nullptr) {
        LogWarn() << "MemoryPrimary::GetDTCExtDataRecordByDTCNumber|GetDtcByValue fail, dtcMaskRecord=" << dtcMaskRecord
                  << ", edrNumber=" << edrNumber << "memId =" << GetMemoryId() << "clusterName"
                  << GetClusterName().c_str();
        return false;
    }

    if (dtcPtr->IsDtcSuppressionOn()) {
        LogVerbose() << "MemoryPrimary::GetDTCExtDataRecordByDTCNumber|dtc is suppression, dtcMaskRecord="
                     << dtcMaskRecord << ", edrNumber=" << edrNumber << "memId =" << GetMemoryId() << "clusterName"
                     << GetClusterName().c_str();
        return false;
    }

    dtcAndStatusRecord.dtcValue = dtcMaskRecord;
    dtcAndStatusRecord.status   = entry->GetDtcStatus();

    entry->FindExtendedDataRecord(edrNumber, records, GetMemoryId());
    return true;
}







bool MemoryDestination::GetNumberOfDTCBySeverityMaskRecord(std::uint8_t dtcSeverityMask,
                                                           std::uint8_t dtcStatusMask,
                                                           std::uint8_t& dtcStatusAvailabilityMask,
                                                           std::uint8_t& dtcFormatIdentifier,
                                                           std::uint16_t& dtcCount) noexcept
{
    std::ignore = dtcSeverityMask;
    std::ignore = dtcStatusMask;
    std::ignore = dtcStatusAvailabilityMask;
    std::ignore = dtcFormatIdentifier;
    std::ignore = dtcCount;
    return false;
}





bool MemoryDestination::GetSupportedDTC(std::uint8_t& dtcStatusAvailabilityMask,
                                        std::vector< DTCAndStatusRecord >& dtcAndStatusRecords)
{
    std::ignore = dtcStatusAvailabilityMask;
    std::ignore = dtcAndStatusRecords;
    return false;
}

std::shared_ptr< MemoryEntry > MemoryDestination::_AllocEntry(uint32_t const dtc, uint8_t const status)
{
    std::unique_lock< std::recursive_mutex > const lock{entryMutex_};
    std::shared_ptr< MemoryEntry > entry{std::make_shared< MemoryEntry >(context_)};
    entry->SetDtcValue(dtc);
    entry->SetDtcStatus(status);
    entry->SetTime(Helper::GetCurTime());
    entries_[dtc] = entry;
    return entry;
}





bool MemoryDestination::_ClearEntry(uint32_t const dtc, bool const isResetStatus)
{
    LogVerbose() << "MemoryDestination::_ClearEntryForbiddenStatus -> enter. dtc=" << dtc
                 << ", is_reset_status=" << isResetStatus << ", memId =" << GetMemoryId();
    std::unique_lock< std::recursive_mutex > const lock{entryMutex_};
    std::uint32_t const count{GetCountForNumberOfEntries()};
    std::shared_ptr< MemoryEntry > entry{GetEntry(dtc)};
    if (nullptr == entry) {
        LogVerbose() << "MemoryInfo::_ClearEntry -> GetEntry null"
                     << ", memId=" << GetMemoryId() << ", clusterName=" << GetClusterName().c_str() << ", dtc=" << dtc;
        return false;
    }

    if (isResetStatus) {
        DTCStatus status{std::move(entry->GetDtcStatus())};
        status.Clear(false);
        entry->SetDtcStatus(status.Get());
    }

    entry->Clear();

    std::uint32_t const countNew{GetCountForNumberOfEntries()};
    if ((count != countNew) && config_.dtcInformationInterface.get() != nullptr) {
        std::ignore = config_.dtcInformationInterface->NotifyNumberOfStoredEntries(countNew);
    }

    return true;
}

void MemoryDestination::_DeleteAllEntry()
{
    LogVerbose() << "MemoryDestination::_DeleteAllEntry -> enter. memId=" << GetMemoryId()
                 << "entries_size =" << entries_.size();
    std::unique_lock< std::recursive_mutex > const lock{entryMutex_};
    for (std::map< uint32_t, std::shared_ptr< MemoryEntry > >::const_iterator itr{entries_.cbegin()};
         itr != entries_.cend(); ++itr) {
        _DeleteEntry(itr->second);
    }

    entries_.clear();
}


void MemoryDestination::_DeleteEntry(std::shared_ptr< MemoryEntry > const& entry)
{
    if (per_.get() == nullptr) {
        LogWarn() << "MemoryDestination::_DeleteEntry -> per_ is nullptr memId=" << GetMemoryId()
                  << "clusterName=" << GetClusterName().c_str();
        return;
    }

    if (entry.get() == nullptr) {
        LogWarn() << "MemoryDestination::_DeleteEntry -> entry is nullptr memId=" << GetMemoryId()
                  << "clusterName=" << GetClusterName().c_str();
        return;
    }

    entry->Clear();

    std::string const key = _MakeDTCStroedKey(entry->GetDtcValue());
    bool const res        = per_->RemoveData(key);
    LogVerbose() << "MemoryDestination::_DeleteEntry -> remove finished memId=" << GetMemoryId()
                 << "dtc =" << entry->GetDtcValue() << "res =" << res;
}


std::uint32_t MemoryDestination::GetCountForNumberOfEntries() noexcept
{
    // [SWS_DM_00902].
    // pendingDTC=1 or confirmedDTC=1

    return GetEntrySize(static_cast< std::uint8_t >(DTCStatusBit::kPendingDTC)
                            | static_cast< std::uint8_t >(DTCStatusBit::kConfirmedDTC),
                        GetDtcStatusAvailabilityMask());
}


std::uint32_t MemoryDestination::GetCountForNumberOfEntriesFilterSuppression() noexcept
{
    std::unique_lock< std::recursive_mutex > const lock{entryMutex_};
    std::uint8_t const statusMask{static_cast< std::uint8_t >(DTCStatusBit::kPendingDTC)
                                  | static_cast< std::uint8_t >(DTCStatusBit::kConfirmedDTC)};
    std::uint8_t const dtcStatusAvailabilityMask{GetDtcStatusAvailabilityMask()};
    uint32_t count{0U};
    for (auto const& itr : entries_) {
        if (itr.second.get() == nullptr) {
            continue;
        }

        std::shared_ptr< Dtc > dtcPtr{context_.GetDtcByValue(itr.second->GetDtcValue())};
        if (dtcPtr.get() == nullptr) {
            continue;
        }
        if (dtcPtr->IsDtcSuppressionOn()) {
            continue;
        }
        uint8_t const status{itr.second->GetDtcStatus()};
        if (0U != (status & statusMask & dtcStatusAvailabilityMask)) {
            ++count;
        }
    }
    return count;
}


uint32_t MemoryDestination::GetEntrySize() noexcept
{
    std::unique_lock< std::recursive_mutex > const lock{entryMutex_};
    return entries_.size();
}


uint32_t MemoryDestination::GetEntrySize(std::uint8_t const statusMask,
                                         std::uint8_t const dtcStatusAvailabilityMask) noexcept
{
    std::unique_lock< std::recursive_mutex > const lock{entryMutex_};
    uint32_t count{0U};
    for (auto const& itr : entries_) {
        if (itr.second.get() == nullptr) {
            continue;
        }
        std::shared_ptr< Dtc > dtcPtr{context_.GetDtcByValue(itr.second->GetDtcValue())};
        if (dtcPtr.get() == nullptr) {
            continue;
        }
        if (dtcPtr->IsDtcSuppressionOn()) {
            continue;
        }
        uint8_t const status{itr.second->GetDtcStatus()};
        if (0U != (status & statusMask & dtcStatusAvailabilityMask)) {
            ++count;
        }
    }
    return count;
}


uint32_t MemoryDestination::GetEntrySize(std::uint8_t const statusMask,
                                         std::uint8_t const dtcSeverityMask,
                                         std::uint8_t const dtcStatusAvailabilityMask) noexcept
{
    std::unique_lock< std::recursive_mutex > const lock{entryMutex_};
    uint32_t count{0U};
    for (auto const& itr : entries_) {
        uint8_t const status{itr.second->GetDtcStatus()};
        uint8_t const severity{itr.second->GetDtcSeverity()};
        std::shared_ptr< Dtc > dtcPtr{context_.GetDtcByValue(itr.second->GetDtcValue())};
        if (dtcPtr.get() == nullptr) {
            continue;
        }
        if (dtcPtr->IsDtcSuppressionOn()) {
            continue;
        }
        if ((0U != (status & statusMask & dtcStatusAvailabilityMask)) && (0U != (severity & dtcSeverityMask))) {
            ++count;
        }
    }
    return count;
}



void MemoryDestination::GetEntriesByMask(std::vector< DTCAndStatusRecord >& dtcAndStatusRecords)
{
    std::unique_lock< std::recursive_mutex > const lock{entryMutex_};
    dtcAndStatusRecords.clear();

    std::set< uint32_t >& dtcList = context_.GetPrimaryDtcList();
    for (auto const& dtcValue : dtcList) {
        DTCAndStatusRecord record{};
        record.dtcValue     = dtcValue;
        decltype(auto) iter = entries_.find(dtcValue);
        if (iter == entries_.end() || iter->second.get() == nullptr) {
            record.status = kDefaultDtcStatus;
        } else {
            std::shared_ptr< Dtc > dtcPtr{context_.GetDtcByValue(dtcValue)};
            if (dtcPtr.get() == nullptr) {
                continue;
            }
            if (dtcPtr->IsDtcSuppressionOn()) {
                continue;
            }
            record.status = iter->second->GetDtcStatus();
        }

        dtcAndStatusRecords.emplace_back(record);
    }
}




void MemoryDestination::_ClearEntryForbiddenStatus(std::shared_ptr< MemoryEntry > const& entry,
                                                   DiagnosticClearEventAllowedBehaviorEnum const clearBehavior) const
{
    if (nullptr == entry) {
        LogVerbose() << "MemoryInfo::_ClearEntry -> entry is null clusterName=" << GetClusterName().c_str()
                     << "memId =" << GetMemoryId();
        return;
    }
    LogVerbose() << "MemoryDestination::_ClearEntryForbiddenStatus -> enter. dtc=" << entry->GetDtcValue()
                 << ", memId =" << GetMemoryId();
    // [SWS_DM_00123].
    // [SWS_DM_00124].
    // [SWS_DM_00121].
    switch (clearBehavior) {
        case DiagnosticClearEventAllowedBehaviorEnum::kNoStatusByteChange: {
            break;
        }
        case DiagnosticClearEventAllowedBehaviorEnum::kOnlyThisCycleAndReadiness: {
            DTCStatus status{entry->GetDtcStatus()};
            status.ClearOnThisCycleAndReadiness();
            entry->SetDtcStatus(status.Get());
            break;
        }
        default: {
        } break;
    }
}










std::shared_ptr< MemoryEntry > MemoryDestination::_DisplacementPrioOcc(uint8_t const priorityNew,
                                                                       uint32_t const dtc,
                                                                       uint8_t const status)
{
    LogVerbose() << "MemoryDestination::_DisplacementPrioOcc -> enter."
                 << "memId=" << GetMemoryId() << ", clusterName=" << GetClusterName().c_str() << ", dtc=" << dtc
                 << ", status=" << status << ", priorityNew=" << priorityNew;
    std::unique_lock< std::recursive_mutex > const lock{entryMutex_};
    std::vector< std::shared_ptr< MemoryEntry > > entries;
    _getLowestPriorityEntries(priorityNew, entries);
    std::shared_ptr< MemoryEntry > entry{GetOldestEntry(entries)};
    if (nullptr == entry) {
        LogVerbose() << "MemoryDestination::_DisplacementPrioOcc -> no oldest entry.";
        return entry;
    }
    // [SWS_DM_00930]
    std::ignore           = entries_.erase(entry->GetDtcValue());
    std::string const key = _MakeDTCStroedKey(entry->GetDtcValue());
    std::ignore           = per_->RemoveData(key);
    uint8_t const nDTCStatus{StatusDuringDisplacement(status, context_)};
    /// @brief _AllocEntry function
    
    return _AllocEntry(dtc, nDTCStatus);
}










std::shared_ptr< MemoryEntry > MemoryDestination::_DisplacementFull(uint8_t const priorityNew,
                                                                    uint32_t const dtc,
                                                                    uint8_t const status)
{
    if (per_.get() == nullptr) {
        LogVerbose() << "MemoryDestination::_DisplacementFull -> per_ is nullptr";
        return std::shared_ptr< MemoryEntry >{nullptr};
    }

    LogVerbose() << "MemoryDestination::_DisplacementFull -> enter."
                 << "memId=" << GetMemoryId() << ", clusterName=" << GetClusterName().c_str() << ", dtc=" << dtc
                 << ", status=" << status << ", priorityNew=" << priorityNew;
    std::unique_lock< std::recursive_mutex > const lock{entryMutex_};
    std::vector< std::shared_ptr< MemoryEntry > > entries;
    bool bHasLowestEntries{true};
    _getLowestPriorityEntries(priorityNew, entries);
    if (entries.empty()) {
        bHasLowestEntries = false;
        _getEqualPriorityEntries(priorityNew, entries);
        if (entries.empty()) {
            LogVerbose() << "MemoryDestination::_DisplacementFull -> no lowest or equel entry.";
            return std::shared_ptr< MemoryEntry >{nullptr};
        }
    }

    std::vector< std::shared_ptr< MemoryEntry > > entriesFilter;
    FilterPassiveEntries(entries, entriesFilter);
    if ((!bHasLowestEntries) && entriesFilter.empty()) {
        FilterTestNotComplateEntries(entries, entriesFilter);
        if (entriesFilter.empty()) {
            LogVerbose() << "MemoryDestination::_DisplacementFull -> filter no complate entry.";
            return std::shared_ptr< MemoryEntry >{nullptr};
        }
    }

    if (!entriesFilter.empty()) {
        entries.clear();
        entries = entriesFilter;
    }

    std::shared_ptr< MemoryEntry > entry{GetOldestEntry(entries)};
    if (nullptr == entry) {
        LogVerbose() << "MemoryDestination::_DisplacementFull -> no oldest entry.";
        return entry;
    }

    std::ignore           = entries_.erase(entry->GetDtcValue());
    std::string const key = _MakeDTCStroedKey(entry->GetDtcValue());
    std::ignore           = per_->RemoveData(key);
    uint8_t const nDTCStatus{StatusDuringDisplacement(status, context_)};
    /// @brief _AllocEntry function
    
    return _AllocEntry(dtc, nDTCStatus);
}

void MemoryDestination::_getLowestPriorityEntries(uint8_t priorityNew,
                                                  std::vector< std::shared_ptr< MemoryEntry > >& entries)
{
    for (auto const& itr : entries_) {
        if (itr.second.get() == nullptr) {
            continue;
        }
        uint8_t const nPriority{itr.second->GetPriority()};
        if (nPriority > priorityNew) {
            priorityNew = nPriority;
            entries.clear();
            entries.emplace_back(itr.second);
        }
    }
}

void MemoryDestination::_getEqualPriorityEntries(uint8_t const priorityNew,
                                                 std::vector< std::shared_ptr< MemoryEntry > >& entries)
{
    for (auto const& itr : entries_) {
        if (itr.second.get() == nullptr) {
            continue;
        }
        if (itr.second->GetPriority() == priorityNew) {
            entries.emplace_back(itr.second);
        }
    }
}

void MemoryDestination::FilterPassiveEntries(std::vector< std::shared_ptr< MemoryEntry > > const& entries,
                                             std::vector< std::shared_ptr< MemoryEntry > >& entriesResult)
{
    for (std::vector< std::shared_ptr< MemoryEntry > >::const_iterator itr{entries.cbegin()}; itr != entries.cend();
         ++itr) {
        if ((*itr)->IsPassive()) {
            entriesResult.emplace_back(*itr);
        }
    }
}

void MemoryDestination::FilterTestNotComplateEntries(std::vector< std::shared_ptr< MemoryEntry > > const& entries,
                                                     std::vector< std::shared_ptr< MemoryEntry > >& entriesResult)
{
    for (std::vector< std::shared_ptr< MemoryEntry > >::const_iterator itr{entries.cbegin()}; itr != entries.cend();
         ++itr) {
        DTCStatus const status{std::move((*itr)->GetDtcStatus())};
        if (status.Get(DTCStatusBit::kTestNotCompletedThisOperationCycle)) {
            entriesResult.emplace_back(*itr);
        }
    }
}

std::shared_ptr< MemoryEntry > MemoryDestination::GetOldestEntry(std::vector< std::shared_ptr< MemoryEntry > >& entries)
{
    if (entries.empty()) {
        return std::shared_ptr< MemoryEntry >{nullptr};
    }
    if (entries.size() == 1U) {
        return entries[0U];
    }
    std::sort(entries.begin(), entries.end(), CompareEntryByTimeUpOrder);
    return entries[0U];
}

uint8_t MemoryDestination::StatusDuringDisplacement(uint8_t const status, FaultManagementRuntimeContext const& context)
{
    DTCStatus dtcStatus{status};
    // ISO14229-1 Figure D.6 — DTC status bit 5 testFailedSinceLastClear logic - FaultMemoryOverflow [vehicle
    // manufacturer specific].
    if (context.GetCommonProps()->IsResetConfirmedBitOnOverflow()) {
        // [SWS_DM_00932]
        dtcStatus.Reset(DTCStatusBit::kConfirmedDTC);
        if (context.GetCommonProps()->GetStatusBitHandlingTestFailedSinceLastClear()
            == DiagnosticStatusBitHandlingTestFailedSinceLastClearEnum::kStatusBitAgingAndDisplacement) {
            // [SWS_DM_00933].
            dtcStatus.Reset(DTCStatusBit::kTestFailedSinceLastClear);
        }
    }
    return dtcStatus.Get();
}

void MemoryDestination::_initRegisterDtcSetting()
{
    context_.GetDtcSetting()->SetNotifier([this](bool const on) {
        if (config_.dtcInformationInterface.get() == nullptr) {
            LogWarn() << "MemoryDestination::InitRegisterDtcSetting -> SetNotifier trigger error "
                         "config_.dtcInformationInterface is nullptr memId ="
                      << GetMemoryId() << "clusterName =" << GetClusterName().c_str();
            return;
        }

        ControlDtcStatusType const val{on ? ControlDtcStatusType::kDTCSettingOn : ControlDtcStatusType::kDTCSettingOff};
        int32_t res = config_.dtcInformationInterface->NotifyControlDtcStatus(val);
        if (res != 0) {
            LogWarn() << "MemoryDestination::InitRegisterDtcSetting -> SetNotifier trigger error "
                         "NotifyControlDtcStatus fail memId ="
                      << GetMemoryId() << "clusterName =" << GetClusterName().c_str();
        }
    });
}

int32_t MemoryDestination::_initRegisterClear()
{
    if (config_.dtcInformationInterface.get() == nullptr) {
        LogWarn() << "MemoryDestination::InitRegisterClear -> config_.dtcInformationInterface is nullptr memId ="
                  << GetMemoryId() << "clusterName =" << GetClusterName().c_str();
        return -1;
    }
    return config_.dtcInformationInterface->RegisterClear([this](uint32_t const dtcGroup) -> int32_t {
        LogDebug() << "MemoryDestination::InitRegisterClear -> clear memId =" << GetMemoryId()
                   << "clusterName =" << GetClusterName().c_str() << "dtcGroup =" << dtcGroup;

        int32_t err = 0;
        std::shared_ptr< DtcGroup > const group{context_.GetDtcGroup(dtcGroup)};
        std::shared_ptr< Dtc > const dtc{context_.GetDtcByValue(dtcGroup)};
        if (group != nullptr) {
            err = static_cast< int32_t >(Clear(group, false));
        } else if (dtc != nullptr) {
            err = static_cast< int32_t >(Clear(dtc, false));
        } else {
            // [SWS_DM_00898].
            err = kInt32_112;
        }

        return err;
    });
}

int32_t MemoryDestination::_initRegisterGetNumberOfStoredEntries()
{
    if (config_.dtcInformationInterface.get() == nullptr) {
        LogWarn() << "MemoryDestination::InitRegisterGetNumberOfStoredEntries -> "
                     "config_.dtcInformationInterface is nullptr memId ="
                  << GetMemoryId() << "clusterName =" << GetClusterName().c_str();
        return -1;
    }
    return config_.dtcInformationInterface->RegisterGetNumberOfStoredEntries(
        /// @brief Get CountForNumberOfEntriesFilterSuppression
        
        [this]() -> uint32_t { return GetCountForNumberOfEntriesFilterSuppression(); });
}

int32_t MemoryDestination::_initRegisterGetControlDTCStatus()
{
    if (config_.dtcInformationInterface.get() == nullptr) {
        LogWarn()
            << "MemoryDestination::InitRegisterEnableControlDtc -> config_.dtcInformationInterface is nullptr memId ="
            << GetMemoryId() << "clusterName =" << GetClusterName().c_str();
        return -1;
    }
    return config_.dtcInformationInterface->RegisterGetControlDTCStatus([this]() -> ControlDtcStatusType {
        ControlDtcStatusType const val{context_.GetDtcSetting()->Get() ? ControlDtcStatusType::kDTCSettingOn
                                                                       : ControlDtcStatusType::kDTCSettingOff};
        return val;
    });
}

int32_t MemoryDestination::_initRegisterEnableControlDtc()
{
    if (config_.dtcInformationInterface.get() == nullptr) {
        LogWarn()
            << "MemoryDestination::InitRegisterEnableControlDtc -> config_.dtcInformationInterface is nullptr memId ="
            << GetMemoryId() << "clusterName =" << GetClusterName().c_str();
        return -1;
    }
    return config_.dtcInformationInterface->RegisterEnableControlDtc([this]() -> int32_t {
        std::ignore = context_.GetDtcSetting()->Set(true);
        return 0;
    });
}

int32_t MemoryDestination::_initRegisterGetCurrentStatus()
{
    if (config_.dtcInformationInterface.get() == nullptr) {
        LogWarn()
            << "MemoryDestination::InitRegisterEnableControlDtc -> config_.dtcInformationInterface is nullptr memId ="
            << GetMemoryId() << "clusterName =" << GetClusterName().c_str();
    }
    return config_.dtcInformationInterface->RegisterGetCurrentStatus(
        [this](std::uint32_t const dtc, uint8_t& dtcStatus) -> int32_t {
            LogDebug() << "MemoryDestination::InitRegisterGetCurrentStatus enter dtc =" << dtc;
            std::shared_ptr< MemoryEntry > dtcEntry{GetEntry(dtc)};
            if (dtcEntry.get() == nullptr) {
                LogDebug() << "MemoryDestination::InitRegisterGetCurrentStatus|GetEntry fail dtc =" << dtc;
                return static_cast< int32_t >(DiagErrc::kNoSuchDTC);
            }
            std::shared_ptr< Dtc > dtcPtr{context_.GetDtcByValue(dtc)};
            if (dtcPtr.get() == nullptr) {
                LogDebug() << "MemoryDestination::InitRegisterGetCurrentStatus|GetDtcByValue fail dtc =" << dtc;
                return static_cast< int32_t >(DiagErrc::kNoSuchDTC);
            }
            if (dtcPtr->IsDtcSuppressionOn()) {
                LogDebug() << "MemoryDestination::InitRegisterGetCurrentStatus|dtc is suppression, dtc =" << dtc;
                return static_cast< int32_t >(DiagErrc::kNoSuchDTC);
            }
            dtcStatus = dtcEntry->GetDtcStatus();
            return 0;
        });
}

int32_t MemoryDestination::_initRegisterGetEventMemoryOverflow()
{
    if (config_.dtcInformationInterface.get() == nullptr) {
        LogWarn()
            << "MemoryDestination::InitRegisterEnableControlDtc -> config_.dtcInformationInterface is nullptr memId ="
            << GetMemoryId() << "clusterName =" << GetClusterName().c_str();
        return -1;
    }

    /// @brief IsOverflow function
    
    return config_.dtcInformationInterface->RegisterGetEventMemoryOverflow([this]() -> bool { return IsOverflow(); });
}

int32_t MemoryDestination::_initRegisterGetDtcSuppression()
{
    if (config_.dtcInformationInterface.get() == nullptr) {
        LogWarn()
            << "MemoryDestination::_initRegisterGetDtcSuppression -> config_.dtcInformationInterface is nullptr memId ="
            << GetMemoryId() << "clusterName =" << GetClusterName().c_str();
        return -1;
    }

    return config_.dtcInformationInterface->RegisterGetDtcSuppression(
        [this](std::uint32_t const dtcValue, DtcSuppressionType& type) -> int32_t {
            std::shared_ptr< Dtc > dtcPtr{context_.GetDtcByValue(dtcValue)};
            if (dtcPtr.get() == nullptr) {
                return static_cast< int32_t >(DiagErrc::kWrongDtc);
            }
            type = dtcPtr->GetDtcSuppression();
            return 0;
        });
}

int32_t MemoryDestination::_initRegisterSetDtcSuppression()
{
    if (config_.dtcInformationInterface.get() == nullptr) {
        LogWarn()
            << "MemoryDestination::_initRegisterSetDtcSuppression -> config_.dtcInformationInterface is nullptr memId ="
            << GetMemoryId() << "clusterName =" << GetClusterName().c_str();
        return -1;
    }

    return config_.dtcInformationInterface->RegisterSetDtcSuppression(
        [this](std::uint32_t const dtcValue, DtcSuppressionType const type) -> int32_t {
            std::shared_ptr< Dtc > dtcPtr{context_.GetDtcByValue(dtcValue)};
            if (dtcPtr.get() == nullptr) {
                return static_cast< int32_t >(DiagErrc::kWrongDtc);
            }
            return dtcPtr->SetDtcSuppression(type);
        });
}

uint32_t MemoryDestination::_initLoadAllEntry(std::vector< uint32_t > const& dtcList)
{
    if (per_.get() == nullptr) {
        LogWarn() << "MemoryDestination::InitLoadAllEntry -> per_ is nullptr memId =" << GetMemoryId()
                  << "clusterName =" << GetClusterName().c_str();
        return 0U;
    }

    std::unique_lock< std::recursive_mutex > const lock{entryMutex_};
    for (size_t i = 0; i < dtcList.size(); i++) {
        uint32_t const& dtc   = dtcList[i];
        std::string const key = _MakeDTCStroedKey(dtc);
        bool const keyExists  = per_->KeyExists(key);
        if (!keyExists) {
            LogDebug() << "MemoryDestination::InitLoadAllEntry -> no key memId =" << GetMemoryId()
                       << "clusterName =" << GetClusterName().c_str() << "dtc =" << dtc;
            continue;
        }

        std::vector< std::uint8_t > const data = per_->LoadData(key);
        if (data.empty()) {
            bool const res = per_->RemoveData(key);
            LogWarn() << "MemoryDestination::InitLoadAllEntry -> Load Data is empty memId =" << GetMemoryId()
                      << "clusterName =" << GetClusterName().c_str() << "dtc =" << dtc << "res =" << res;
            continue;
        }

        std::shared_ptr< MemoryEntry > entry{std::make_shared< MemoryEntry >(context_)};
        entry->SetDtcValue(dtc);
        std::ignore   = entry->Deserialize(data);
        entries_[dtc] = entry;
    }

    return entries_.size();
}

uint32_t MemoryDestination::_initLoadAllOpenationStatus(std::vector< uint32_t > const& operationList)
{
    if (!IsPrimary()) {
        return 0U;
    }

    if (operationList.empty()) {
        return 0U;
    }

    if (per_.get() == nullptr) {
        LogWarn() << "MemoryDestination::InitLoadAllEntry -> per_ is nullptr memId =" << GetMemoryId()
                  << "clusterName =" << GetClusterName().c_str();
        return 0U;
    }

    std::unique_lock< std::recursive_mutex > const lock{operationMutex_};
    for (size_t i = 0; i < operationList.size(); i++) {
        uint32_t const operationId = operationList[i];
        std::string const key      = _MakeOperationStroedKey(operationId);

        bool const keyExists = per_->KeyExists(key);
        if (!keyExists) {
            LogDebug() << "MemoryDestination::InitLoadAllOpenationStatus -> no key memId =" << GetMemoryId()
                       << "clusterName =" << GetClusterName().c_str() << "operationId =" << operationId;
            continue;
        }

        std::vector< std::uint8_t > const data = per_->LoadData(key);
        if (data.empty()) {
            bool const res = per_->RemoveData(key);
            LogWarn() << "MemoryDestination::InitLoadAllOpenationStatus -> Load Data is empty memId =" << GetMemoryId()
                      << "clusterName =" << GetClusterName().c_str() << "operationId =" << operationId
                      << "res =" << res;
            continue;
        }

        uint8_t operationStatus{0U};
        int32_t const deserializeRes = isoft::serialize::Deserialize(data, operationStatus);
        if (deserializeRes < 0) {
            LogWarn() << "MemoryDestination::InitLoadAllEntry -> deserialize error memId =" << GetMemoryId()
                      << "clusterName =" << GetClusterName().c_str() << "operationId =" << operationId;
            continue;
        }

        ocAndStates_[operationId] = operationStatus;
    }
    return ocAndStates_.size();
}

}  // namespace server
}  // namespace uds
}  // namespace isoft
