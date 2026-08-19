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
/// @file       extended_data_record.cpp
/// @brief      This file implements extended_data_record functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=fault_management
/// @trace_id_sr=dcff2b4e-9576-4b50-a132-2fcfc607cad8
/// @unit_name=ExtendedDataRecord
/// @unit_description=This file provides extended_data_record functionality
/// @interface_level=unit
/// @endcode
///
/// ================================================================

#include "fault_management/extended_data_record.h"

#include <set>

#include "common/log.h"
#include "fault_management/dtc.h"
#include "fault_management/fault_management_runtime_context.h"
#include "fault_management/memory_destination.h"
#include "fault_management/memory_entry.h"
#include "isoft/uds/type_int.h"
#include "nai/os/nai_socket.h"

namespace isoft {
namespace uds {
namespace server {


bool ExtendedDataRecord::CaptureRecord(std::vector< MemoryEntryExtendedRecord >& edrt, std::uint32_t const dtc)
{
    if (config_.dataElementList.empty()) {
        LogWarn() << "ExtendedDataRecord::CaptureRecord|data_element_list is empty dtc =" << dtc
                  << "recordNumber =" << GetRecordNumber();
        return false;
    }

    LogVerbose() << "ExtendedDataRecord::CaptureRecord|enter. dtc=" << dtc << "recordNumber =" << GetRecordNumber();

    edrt.clear();
    for (auto const& entry : config_.dataElementList) {
        MemoryEntryExtendedRecord record;
        std::uint32_t const dataElementId{entry.dataElementId};
        record.id = dataElementId;

        if (entry.category == EdrDataElementCategory::kExternal) {
            if (dataManage_.get() == nullptr) {
                LogWarn() << "ExtendedDataRecord::CaptureRecord|dataManage_ is nullptr dtc =" << dtc
                          << "recordNumber =" << GetRecordNumber();
                return false;
            }

            // 从数据管理器获取数据
            MetaInfoMap metaInfo;
            Helper::MakeMetaInfo(dtc, metaInfo);

            std::shared_ptr< CancellationState > cancellationState;

            std::vector< std::uint32_t > idTable;
            idTable.emplace_back(entry.dataElementId);
            Result< std::list< DiagnosticDataElement > > result
                = dataManage_->ReadEdrElement(idTable, metaInfo, cancellationState);
            if (!result.HasValue()) {
                record.vecData.emplace_back(kPandingData);
                LogWarn() << "ExtendedDataRecord::CaptureData|read data fails error =" << result.Error();
                continue;
            }

            if (result.Value().empty()) {
                record.vecData.emplace_back(kPandingData);
                LogWarn() << "ExtendedDataRecord::CaptureData|read data is empty";
                continue;
            }

            record.vecData = result.Value().begin()->record;
        }
        edrt.push_back(record);
        LogVerbose() << "ExtendedDataRecord::CaptureRecord -> enter. dtc=" << dtc
                     << "recordNumber =" << GetRecordNumber() << "id =" << entry.dataElementId
                     << "category =" << static_cast< uint8_t >(entry.category);
    }

    return true;
}


EdrDataElementCategory ExtendedDataRecord::GetDataElementCategory(std::uint32_t const& dataElementId)
{

    std::uint32_t const strDataElementId(dataElementId);
    for (auto const& entry : config_.dataElementList) {
        if (entry.dataElementId != strDataElementId) {
            continue;
        }

        return entry.category;
    }

    LogWarn() << "ExtendedDataRecord::GetDataElementCategory|no find dataelement dataElementId=" << dataElementId
              << "recordNumber =" << GetRecordNumber();
    return EdrDataElementCategory::kExternal;
}


void ExtendedDataRecord::GetInternalData(EdrDataElementCategory const provider,
                                         std::vector< std::uint8_t >& data,
                                         std::uint32_t const dtc,
                                         std::uint32_t const memId) const
{
    std::ignore = data;
    if (EdrDataElementCategory::kExternal == provider) {
        LogVerbose() << "ExtendedDataRecord::GetInternalData| external can't be get dtc=" << dtc
                     << "recordNumber =" << GetRecordNumber();
        return;
    }

    CaptureInternalData(data, context_.GetDtcByValue(dtc), context_.GetDestination(memId), provider);

    
}

void ExtendedDataRecord::CaptureInternalData(std::vector< std::uint8_t >& datas,
                                             std::shared_ptr< Dtc > const& dtc,
                                             std::shared_ptr< MemoryDestination > const& dest,
                                             EdrDataElementCategory const category)
{
    if (EdrDataElementCategory::kExternal == category) {
        LogWarn() << "ExtendedDataRecord::CaptureInternalData -> invalid param."
                  << "provider=" << static_cast< std::uint32_t >(category);
        return;
    }
    if (nullptr == dtc) {
        LogWarn() << "ExtendedDataRecord::CaptureInternalData -> dtc_ptr is null";
        return;
    }
    if (nullptr == dest) {
        LogWarn() << "ExtendedDataRecord::CaptureInternalData -> dest_ptr is null";
        return;
    }
    LogVerbose() << "ExtendedDataRecord::CaptureInternalData -> enter."
                 << "provider=" << static_cast< std::uint32_t >(category) << ", dtc=" << dtc->GetValue()
                 << ", destination=" << dest->GetMemoryId();
    datas.clear();
    switch (category) {
        case EdrDataElementCategory::kDemAgingctrDowncnt: {  // [SWS_DM_00957]
            if (dtc->HasAging()) {
                std::shared_ptr< MemoryEntry > const entry{dest->GetEntry(dtc->GetValue())};
                if (nullptr == entry) {
                    LogWarn() << "ExtendedDataRecord::CaptureInternalData -> not found entry."
                              << "provider=" << static_cast< std::uint32_t >(category) << ", dtc=" << dtc->GetValue()
                              << ", dest=" << dest->GetMemoryId();
                    break;
                }
                uint32_t const downCounter = dtc->GetAgingThreshold() - entry->GetAgingCount();
                datas.emplace_back(static_cast< std::uint8_t >(downCounter));
            } else {
                datas.emplace_back(kInt32_255U);  // [SWS_DM_00959]
            }
            break;
        }
        case EdrDataElementCategory::kDemAgingctrUpcnt: {  // [SWS_DM_00956]
            if (dtc->HasAging()) {
                std::shared_ptr< MemoryEntry > const entry{dest->GetEntry(dtc->GetValue())};
                if (nullptr == entry) {
                    LogWarn() << "ExtendedDataRecord::CaptureInternalData -> not found entry."
                              << "provider=" << static_cast< std::uint32_t >(category) << ", dtc=" << dtc->GetValue()
                              << ", dest=" << dest->GetMemoryId();
                    break;
                }
                datas.emplace_back(static_cast< std::uint8_t >(entry->GetAgingCount()));
            } else {
                datas.emplace_back(0);  // [SWS_DM_00958]
            }
            break;
        }
        case EdrDataElementCategory::kDemCurrentFdc: {  // [SWS_DM_00951]
            datas.emplace_back(dtc->GetCurrentFDC());
            break;
        }
        case EdrDataElementCategory::kDemCyclesSinceFirstFailed: {  // [SWS_DM_00963]
            datas.emplace_back(dtc->GetCyclesSinceFirstFailed());
            break;
        }
        case EdrDataElementCategory::kDemCyclesSinceLastFailed: {  // [SWS_DM_00964]
            datas.emplace_back(dtc->GetCyclesSinceLastFailed());
            break;
        }
        case EdrDataElementCategory::kDemFailedCycles: {  //[SWS_DM_00962]
            datas.emplace_back(dtc->GetFailedCycles());
            break;
        }
        case EdrDataElementCategory::kDemMaxFdcDuringCurrentCycle: {  // [SWS_DM_00953]
            datas.emplace_back(dtc->GetMaxFDCDuringCurrentCycle());
            break;
        }
        case EdrDataElementCategory::kDemMaxFdcSinceLastClear: {  // [SWS_DM_00952]
            datas.emplace_back(dtc->GetMaxFDCSinceLastClear());
            break;
        }
        case EdrDataElementCategory::kDemOccctr: {  // [SWS_DM_00954]
            std::shared_ptr< MemoryEntry > const entry{dest->GetEntry(dtc->GetValue())};
            if (nullptr == entry) {
                LogWarn() << "ExtendedDataRecord::CaptureInternalData -> not found entry."
                          << "provider=" << static_cast< std::uint32_t >(category) << ", dtc=" << dtc->GetValue()
                          << ", dest=" << dest->GetMemoryId();
                break;
            }
            datas.emplace_back(entry->GetOccurrenceCounter());
            break;
        }
        case EdrDataElementCategory::kDemOvflind: {  // [SWS_DM_00921]
            uint8_t data{0U};
            if (dest->IsOverflow()) {
                data = 1U;
            }
            datas.emplace_back(data);
            std::ignore = data;
            break;
        }
        case EdrDataElementCategory::kDemSignificance: {  // [SWS_DM_00961]
            datas.emplace_back(dtc->GetSignificance());
            break;
        }
        case EdrDataElementCategory::kDemPriority: {  // [SWS_DM_00950]
            datas.emplace_back(dtc->GetPriority());
            break;
        }
        case EdrDataElementCategory::kDemEventAssociatedIdentification: {
            std::uint32_t const id{dtc->GetEventID()};
            std::uint32_t const idNew{nai_htonl(id)};
            datas.emplace_back(static_cast< std::uint8_t >(idNew & kInt8_0xFFU));
            datas.emplace_back(static_cast< std::uint8_t >((idNew >> kInt32_8U) & kInt8_0xFFU));
            datas.emplace_back(static_cast< std::uint8_t >((idNew >> kInt32_16U) & kInt8_0xFFU));
            datas.emplace_back(static_cast< std::uint8_t >((idNew >> kInt32_24U) & kInt8_0xFFU));
            break;
        }
        default: {
            LogWarn() << "ExtendedDataRecord::CaptureInternalData -> not support category"
                      << "provider=" << static_cast< std::uint32_t >(category) << ", dtc=" << dtc->GetValue()
                      << ", dest=" << dest->GetMemoryId();
        } break;
    }
}

}  // namespace server
}  // namespace uds
}  // namespace isoft
