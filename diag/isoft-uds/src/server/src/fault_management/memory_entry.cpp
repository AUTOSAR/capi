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
/// @file       memory_entry.cpp
/// @brief      This file implements memory_entry functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=fault_management
/// @trace_id_sr=41be708c-b25e-40c7-ac9f-e7bbb42e2c54
/// @unit_name=MemoryEntry
/// @unit_description=This file provides memory_entry functionality
/// @interface_level=unit
/// @endcode
///
/// ================================================================

#include "fault_management/memory_entry.h"

#include "common/log.h"
#include "fault_management/dtc.h"
#include "fault_management/extended_data_record.h"
#include "fault_management/fault_management_runtime_context.h"
#include "fault_management/status.h"

namespace isoft {
namespace uds {
namespace server {






void MemoryEntry::Clear()
{
    std::unique_lock< std::recursive_mutex > const lock{mutex_};
    entryData_.Clear();
    AgingReset();
}



void MemoryEntry::FindFreezeFrameDataRecord(uint8_t const record, std::vector< SnapshotDataRecordType >& records)
{
    records.clear();
    std::unique_lock< std::recursive_mutex > const lock{mutex_};
    if (record == static_cast< std::uint8_t >(GlobalConstValueEnum::kSnapshotDataRecordAll)) {
        if (entryData_.mapFreezeRecord.empty()) {
            LogVerbose() << "MemoryEntry::FindFreezeFrameDataRecord -> no alloc ff ff_id=" << record
                         << "dtc =" << GetDtcValue();
            return;
        }

        for (std::map< uint8_t, std::vector< MemoryEntryFreezeRecord > >::iterator it
             = entryData_.mapFreezeRecord.begin();
             it != entryData_.mapFreezeRecord.end(); ++it) {
            SnapshotDataRecordType dataRecord;
            dataRecord.snapshotRecordNumber = it->first;
            for (std::vector< MemoryEntryFreezeRecord >::iterator iter = it->second.begin(); iter != it->second.end();
                 ++iter) {
                SnapshotDataIdentifierType did;
                did.dataIdentifier = (*iter).did;
                did.data           = (*iter).vecData;
                dataRecord.snapshotDataIdentifiers.emplace_back(did);
            }
            records.emplace_back(dataRecord);
        }
    } else {
        std::map< uint8_t, std::vector< MemoryEntryFreezeRecord > >::iterator it
            = entryData_.mapFreezeRecord.find(record);
        if (it == entryData_.mapFreezeRecord.end()) {
            LogVerbose() << "MemoryEntry::FindFreezeFrameDataRecord -> no alloc ff ff_id=" << record
                         << "dtc =" << GetDtcValue();
            return;
        }

        SnapshotDataRecordType dataRecord;
        dataRecord.snapshotRecordNumber = record;
        for (std::vector< MemoryEntryFreezeRecord >::iterator iter = it->second.begin(); iter != it->second.end();
             ++iter) {
            SnapshotDataIdentifierType did;
            did.dataIdentifier = (*iter).did;
            did.data           = (*iter).vecData;
            dataRecord.snapshotDataIdentifiers.emplace_back(did);
        }
        records.emplace_back(dataRecord);
    }
}








void MemoryEntry::AllocFreezeFrameFrom(std::uint8_t const recordNumber,
                                       std::vector< MemoryEntryFreezeRecord > const& ff)
{
    std::unique_lock< std::recursive_mutex > const lock{mutex_};
    if (ff.empty()) {
        return;
    }
    entryData_.mapFreezeRecord[recordNumber] = ff;
}







void MemoryEntry::AllocExtendedDataRecordFrom(std::uint8_t const recordNumber,
                                              std::vector< MemoryEntryExtendedRecord > const& edr)
{
    std::unique_lock< std::recursive_mutex > const lock{mutex_};
    if (edr.empty()) {
        return;
    }

    entryData_.mapExtendedDataRecord[recordNumber] = edr;
}


uint32_t MemoryEntry::GetFFSize() noexcept
{
    std::unique_lock< std::recursive_mutex > const lock{mutex_};
    return static_cast< uint32_t >(entryData_.mapFreezeRecord.size());
}

uint8_t MemoryEntry::GetDtcSeverity() const noexcept
{
    std::shared_ptr< Dtc > dtc{context_.GetDtcByValue(dtcValue_)};
    if (dtc != nullptr) {
        return dtc->GetSeverity();
    }
    return 0U;
}

uint8_t MemoryEntry::GetDtcStatus() const noexcept
{
    std::unique_lock< std::recursive_mutex > const lock{mutex_};
    return entryData_.dtcStatus;
}


uint8_t MemoryEntry::GetOccurrenceCounter() const noexcept
{
    std::unique_lock< std::recursive_mutex > const lock{mutex_};
    return entryData_.occurrenceCounter;
}

void MemoryEntry::OccurrenceCounterIncrement() noexcept
{
    std::unique_lock< std::recursive_mutex > const lock{mutex_};
    if (entryData_.occurrenceCounter == kInt8_0xFFU) {  // [SWS_DM_00948]
        return;
    }
    ++entryData_.occurrenceCounter;
}




uint8_t MemoryEntry::GetPriority() const
{
    std::shared_ptr< Dtc > dtc{context_.GetDtcByValue(dtcValue_)};
    if (dtc != nullptr) {
        return dtc->GetPriority();
    }
    return 1U;
}




bool MemoryEntry::IsPassive() const
{
    if (context_.GetCommonProps()->IsStatusBitStorageTestFailed()) {
        std::unique_lock< std::recursive_mutex > const lock{mutex_};
        DTCStatus const statusOfDTC{entryData_.dtcStatus};
        if (!statusOfDTC.Get(DTCStatusBit::kTestFailed)) {
            return true;
        }
    }
    return false;
}




void MemoryEntry::FindExtendedDataRecord(uint8_t const record,
                                         std::vector< DTCExtDataRecord >& records,
                                         std::uint32_t const memId)
{
    std::unique_lock< std::recursive_mutex > const lock{mutex_};
    std::shared_ptr< Dtc > dtc{context_.GetDtcByValue(dtcValue_)};
    if (dtc.get() == nullptr) {
        LogWarn() << "MemoryEntry::FindExtendedDataRecord -> dtc is nullptr dtc =" << GetDtcValue();
        return;
    }

    LogVerbose() << "MemoryEntry::FindExtendedDataRecord|enter edr_id =" << record << "dtc =" << GetDtcValue();
    if (static_cast< std::uint8_t >(GlobalConstValueEnum::kExtDataRecordAll) == record) {
        if (entryData_.mapExtendedDataRecord.empty()) {
            LogVerbose() << "MemoryEntry::FindExtendedDataRecord -> no alloc edr edr_id =" << record
                         << "dtc =" << GetDtcValue();
            return;
        }

        for (std::map< uint8_t, std::vector< MemoryEntryExtendedRecord > >::iterator it
             = entryData_.mapExtendedDataRecord.begin();
             it != entryData_.mapExtendedDataRecord.end(); ++it) {
            std::shared_ptr< ExtendedDataRecord > edr = dtc->GetExtendedDataRecord(it->first);
            if (edr.get() == nullptr) {
                LogWarn() << "MemoryEntry::FindExtendedDataRecord -> 1 not find dtc edr dtc =" << GetDtcValue()
                          << "edrNumber =" << it->first;
                continue;
            }
            DTCExtDataRecord dataRecord;
            dataRecord.recordNumber = it->first;
            for (std::vector< MemoryEntryExtendedRecord >::iterator iter = it->second.begin(); iter != it->second.end();
                 ++iter) {
                EdrDataElementCategory const category = edr->GetDataElementCategory((*iter).id);
                if (category == EdrDataElementCategory::kExternal) {
                    std::ignore
                        = dataRecord.data.insert(dataRecord.data.end(), (*iter).vecData.begin(), (*iter).vecData.end());
                } else {
                    std::vector< std::uint8_t > data{};
                    edr->GetInternalData(category, data, dtcValue_, memId);
                    std::ignore = dataRecord.data.insert(dataRecord.data.end(), data.begin(), data.end());
                }
            }
            records.emplace_back(dataRecord);
        }
    } else {
        std::map< uint8_t, std::vector< MemoryEntryExtendedRecord > >::iterator it
            = entryData_.mapExtendedDataRecord.find(record);
        if (it == entryData_.mapExtendedDataRecord.end()) {
            LogVerbose() << "MemoryEntry::FindExtendedDataRecord -> no alloc edr edr_id =" << record
                         << "dtc =" << GetDtcValue();
            return;
        }

        std::shared_ptr< ExtendedDataRecord > edr = dtc->GetExtendedDataRecord(record);
        if (edr.get() == nullptr) {
            LogWarn() << "MemoryEntry::FindExtendedDataRecord -> 2 not find dtc edr dtc =" << GetDtcValue()
                      << "edrNumber =" << record;
            return;
        }

        DTCExtDataRecord dataRecord;
        dataRecord.recordNumber = record;
        for (std::vector< MemoryEntryExtendedRecord >::iterator iter = it->second.begin(); iter != it->second.end();
             ++iter) {
            EdrDataElementCategory const category = edr->GetDataElementCategory((*iter).id);
            if (category == EdrDataElementCategory::kExternal) {
                std::ignore
                    = dataRecord.data.insert(dataRecord.data.end(), (*iter).vecData.begin(), (*iter).vecData.end());
            } else {
                std::vector< std::uint8_t > data{};
                edr->GetInternalData(category, data, dtcValue_, memId);
                std::ignore = dataRecord.data.insert(dataRecord.data.end(), data.begin(), data.end());
            }
        }
        records.emplace_back(dataRecord);
    }
}


void MemoryEntry::Serialize(std::vector< uint8_t >& serializedMessage)
{
    std::unique_lock< std::recursive_mutex > const lock{mutex_};
    int32_t const res = isoft::serialize::Serialize(serializedMessage, entryData_);
    LogDebug() << "MemoryEntry::Serialize|serializedMessageSize =" << serializedMessage.size() << "res =" << res;
}



bool MemoryEntry::Deserialize(std::vector< uint8_t > const& serializedMessage)
{
    std::unique_lock< std::recursive_mutex > const lock{mutex_};
    int32_t const res = isoft::serialize::Deserialize(serializedMessage, entryData_);
    LogDebug() << "MemoryEntry::Deserialize|serializedMessageSize =" << serializedMessage.size() << "res =" << res;

    return res > 0;
}

}  // namespace server
}  // namespace uds
}  // namespace isoft