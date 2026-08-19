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
/// @file       dtc_filter.cpp
/// @brief      This file implements dtc_filter functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=fault_management
/// @trace_id_sr=fb2562bf-0b4e-451b-aefb-b329096652a8
/// @unit_name=DtcFilter
/// @unit_description=This file provides dtc_filter functionality
/// @interface_level=unit
/// @endcode
///
/// ================================================================

#include "fault_management/dtc_filter.h"

#include "common/log.h"
#include "fault_management/dtc.h"
#include "fault_management/dtc_group.h"
#include "fault_management/fault_management_runtime_context.h"
#include "fault_management/memory_destination.h"

namespace isoft {
namespace uds {
namespace server {

DtcFilter::DtcFilter(FaultManagementRuntimeContext& context) noexcept : context_{context} {}
DtcFilter::~DtcFilter() noexcept = default;


int32_t DtcFilter::GetNumberOfDTCByStatusMask(std::uint8_t const dtcStatusMask,
                                              std::uint8_t& dtcStatusAvailabilityMask,
                                              std::uint8_t& dtcFormatIdentifier,
                                              std::uint16_t& dtcCount)
{
    std::ignore = dtcStatusAvailabilityMask;
    std::ignore = dtcFormatIdentifier;
    std::ignore = dtcCount;
    std::shared_ptr< MemoryDestination > const dest{context_.GetPrimaryDestination()};
    if (nullptr == dest) {
        dtcStatusAvailabilityMask = 0U;
        dtcFormatIdentifier       = 0U;
        dtcCount                  = 0U;
        LogWarn() << "DtcFilter::GetNumberOfDTCByStatusMask -> primary destination is nullptr";
        return -1;
    }

    bool const isSuccess
        = dest->GetNumberOfDTCByStatusMask(dtcStatusMask, dtcStatusAvailabilityMask, dtcFormatIdentifier, dtcCount);
    return (isSuccess ? 0 : -2);
}


int32_t DtcFilter::GetDTCByStatusMask(std::uint8_t const dtcStatusMask,
                                      std::uint8_t& dtcStatusAvailabilityMask,
                                      std::vector< DTCAndStatusRecord >& dtcAndStatusRecords)
{
    std::ignore = dtcStatusAvailabilityMask;
    std::shared_ptr< MemoryDestination > dest{context_.GetPrimaryDestination()};
    if (nullptr == dest) {
        dtcStatusAvailabilityMask = 0U;
        dtcAndStatusRecords.clear();
        LogWarn() << "DtcFilter::GetDTCByStatusMask -> primary destination is nullptr";
        return -1;
    }

    bool const isSuccess = dest->GetDTCByStatusMask(dtcStatusMask, dtcStatusAvailabilityMask, dtcAndStatusRecords);
    return (isSuccess ? 0 : -2);
}

int32_t DtcFilter::GetDTCSnapshotNumber(std::vector< DTCAndSnapshotNumber >& dtcAndSnapshotNumberTable) noexcept
{
    auto dtcTable{context_.GetAllDtc()};
    for (auto&& dtcPtr : dtcTable) {
        if (dtcPtr.get() == nullptr) {
            continue;
        }

        if (dtcPtr->IsDtcSuppressionOn()) {
            continue;
        }

        DTCAndSnapshotNumber dtcAndSnapshotNumber{};
        dtcAndSnapshotNumber.dtcValue                = dtcPtr->GetValue();
        dtcAndSnapshotNumber.snapshotIdentifierTable = dtcPtr->GetFreezeFrameNumberTable();
        dtcAndSnapshotNumberTable.emplace_back(dtcAndSnapshotNumber);
    }
    return 0;
}

int32_t DtcFilter::GetDTCSnapshotRecordByDTCNumber(std::uint32_t const dtcMaskRecord,
                                                   std::uint8_t const ssrNumber,
                                                   DTCAndStatusRecord& dtcAndStatusRecord,
                                                   std::vector< SnapshotDataRecordType >& records)
{
    std::shared_ptr< MemoryDestination > dest{context_.GetPrimaryDestination()};
    if (dest.get() == nullptr) {
        LogWarn() << "DtcFilter::GetDTCSnapshotRecordByDTCNumber -> primary dest is nullptr"
                  << "dtc_make_record=" << dtcMaskRecord << ", ssrNumber=" << ssrNumber;
        return -1;
    }

    bool const isSuccess = dest->GetDTCSnapshotRecordByDTCNumber(dtcMaskRecord, ssrNumber, dtcAndStatusRecord, records);
    return (isSuccess ? 0 : -2);
}


int32_t DtcFilter::GetDTCExtDataRecordByDTCNumber(std::uint32_t const dtcMaskRecord,
                                                  std::uint8_t const edrNumber,
                                                  DTCAndStatusRecord& dtcAndStatusRecord,
                                                  std::vector< DTCExtDataRecord >& records)
{
    std::shared_ptr< MemoryDestination > dest{context_.GetPrimaryDestination()};
    if (dest.get() == nullptr) {
        LogWarn() << "DtcFilter::GetDTCExtDataRecordByDTCNumber -> primary dest is nullptr"
                  << "dtc_make_record=" << dtcMaskRecord << ", edrNumber=" << edrNumber;
        return -1;
    }

    bool const isSuccess = dest->GetDTCExtDataRecordByDTCNumber(dtcMaskRecord, edrNumber, dtcAndStatusRecord, records);
    return (isSuccess ? 0 : -2);
}


int32_t DtcFilter::GetNumberOfDTCBySeverityMaskRecord(std::uint8_t const dtcSeverityMask,
                                                      std::uint8_t const dtcStatusMask,
                                                      std::uint8_t& dtcStatusAvailabilityMask,
                                                      std::uint8_t& dtcFormatIdentifier,
                                                      std::uint16_t& dtcCount)
{
    std::shared_ptr< MemoryDestination > dest{context_.GetPrimaryDestination()};
    if (nullptr == dest) {
        LogWarn() << "DtcFilter::GetNumberOfDTCBySeverityMaskRecord -> dest is nullptr"
                  << "dtcStatusMask=" << dtcStatusMask << ", dtcSeverityMask=" << dtcSeverityMask;
        return -1;
    }

    bool const isSuccess = dest->GetNumberOfDTCBySeverityMaskRecord(
        dtcSeverityMask, dtcStatusMask, dtcStatusAvailabilityMask, dtcFormatIdentifier, dtcCount);
    return (isSuccess ? 0 : -2);
}


int32_t DtcFilter::GetSupportedDTC(std::uint8_t& dtcStatusAvailabilityMask,
                                   std::vector< DTCAndStatusRecord >& dtcAndStatusRecords)
{
    std::shared_ptr< MemoryDestination > dest{context_.GetPrimaryDestination()};
    if (nullptr == dest) {
        LogWarn() << "DtcFilter::GetSupportedDTC -> dest is nullptr";
        return -1;
    }

    bool const isSuccess = dest->GetSupportedDTC(dtcStatusAvailabilityMask, dtcAndStatusRecords);
    return (isSuccess ? 0 : -2);
}


int32_t DtcFilter::GetDTCFaultDetectionCounter(std::vector< DTCFaultDetectionCounterRecord >& dtcFdcRecords)
{
    dtcFdcRecords.clear();
    std::shared_ptr< DtcGroup > const group{context_.GetDtcGroup(kGroupOfAllDTCs)};
    if (group != nullptr) {
        for (std::map< uint32_t, std::shared_ptr< Dtc > >::const_iterator itr{group->Begin()}; itr != group->End();
             ++itr) {
            std::int8_t const fdc{itr->second->GetCurrentFDC()};
            if ((fdc > 0) && (fdc < kFdcMax)) {
                DTCFaultDetectionCounterRecord record{};
                record.dtcValue = itr->second->GetValue();
                record.fdc      = fdc;
                dtcFdcRecords.emplace_back(record);
                std::ignore = record;
            }
        }
    }
    return 0;
}


int32_t DtcFilter::GetUserDefMemoryDTCByStatusMask(std::uint8_t const dtcStatusMask,
                                                   std::uint8_t const memorySelection,
                                                   std::uint8_t& dtcStatusAvailabilityMask,
                                                   std::vector< DTCAndStatusRecord >& dtcAndStatusRecords)
{
    std::shared_ptr< MemoryDestination > const dest{
        context_.GetDestination(static_cast< std::uint32_t >(memorySelection))};
    if (dest.get() == nullptr) {
        LogWarn() << "DtcFilter::GetUserDefMemoryDTCByStatusMask -> dest is nullptr"
                  << "dtcStatusMask=" << dtcStatusMask << ", memorySelection=" << memorySelection;
        return -1;
    }

    if (dest->IsPrimary()) {
        LogWarn() << "DtcFilter::GetUserDefMemoryDTCByStatusMask -> dest is primary"
                  << "dtcStatusMask=" << dtcStatusMask << ", memorySelection=" << memorySelection;
        return -2;
    }

    bool const isSuccess = dest->GetDTCByStatusMask(dtcStatusMask, dtcStatusAvailabilityMask, dtcAndStatusRecords);
    return (isSuccess ? 0 : -3);
}


int32_t DtcFilter::GetUserDefMemoryDTCSnapshotRecordByDTCNumber(std::uint32_t const dtcMaskRecord,
                                                                std::uint8_t const ssrNumber,
                                                                std::uint8_t const memorySelection,
                                                                DTCAndStatusRecord& dtcAndStatusRecord,
                                                                std::vector< SnapshotDataRecordType >& records)
{
    std::shared_ptr< MemoryDestination > const dest{
        context_.GetDestination(static_cast< std::uint32_t >(memorySelection))};
    if (dest.get() == nullptr) {
        LogWarn() << "DtcFilter::GetUserDefMemoryDTCSnapshotRecordByDTCNumber -> dest is nullptr memorySelection="
                  << memorySelection;
        return -1;
    }

    if (dest->IsPrimary()) {
        LogWarn() << "DtcFilter::GetUserDefMemoryDTCSnapshotRecordByDTCNumber -> dest is primary memorySelection="
                  << memorySelection;
        return -2;
    }

    bool const isSuccess = dest->GetDTCSnapshotRecordByDTCNumber(dtcMaskRecord, ssrNumber, dtcAndStatusRecord, records);
    return (isSuccess ? 0 : -3);
}


int32_t DtcFilter::GetUserDefMemoryDTCExtDataRecordByDTCNumber(std::uint32_t const dtcMaskRecord,
                                                               std::uint8_t const edrNumber,
                                                               std::uint8_t const memorySelection,
                                                               DTCAndStatusRecord& dtcAndStatusRecord,
                                                               std::vector< DTCExtDataRecord >& records)
{
    std::shared_ptr< MemoryDestination > const dest{
        context_.GetDestination(static_cast< std::uint32_t >(memorySelection))};
    if (dest.get() == nullptr) {
        LogWarn() << "DtcFilter::GetUserDefMemoryDTCExtDataRecordByDTCNumber -> dest is nullptr memorySelection="
                  << memorySelection;
        return -1;
    }

    if (dest->IsPrimary()) {
        LogWarn() << "DtcFilter::GetUserDefMemoryDTCExtDataRecordByDTCNumber -> dest is primary memorySelection="
                  << memorySelection;
        return -2;
    }

    bool const isSuccess = dest->GetDTCExtDataRecordByDTCNumber(dtcMaskRecord, edrNumber, dtcAndStatusRecord, records);
    return (isSuccess ? 0 : -3);
}

}  // namespace server
}  // namespace uds
}  // namespace isoft
