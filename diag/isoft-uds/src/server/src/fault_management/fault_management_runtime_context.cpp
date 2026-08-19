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
/// @file       fault_management_runtime_context.cpp
/// @brief      This file implements fault_management_runtime_context functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=fault_management
/// @trace_id_sr=3769719b-0140-4437-b341-afd828d203d0
/// @unit_name=FaultManagementRuntimeContext
/// @unit_description=This file provides fault_management_runtime_context functionality
/// @interface_level=unit
/// @endcode
///
/// ================================================================

#include "fault_management/fault_management_runtime_context.h"

#include <nai/os/nai_system.h>

#include "common/log.h"
#include "fault_management/aging.h"
#include "fault_management/condition.h"
#include "fault_management/condition_group.h"
#include "fault_management/control_dtc_setting.h"
#include "fault_management/data_identifier_set.h"
#include "fault_management/dtc.h"
#include "fault_management/dtc_group.h"
#include "fault_management/event.h"
#include "fault_management/extended_data_record.h"
#include "fault_management/freeze_frame.h"
#include "fault_management/indicator.h"
#include "fault_management/memory_destination.h"
#include "fault_management/memory_entry.h"
#include "fault_management/memory_primary.h"
#include "fault_management/memory_user_defined.h"
#include "fault_management/operation_cycle.h"

namespace isoft {
namespace uds {
namespace server {

std::int32_t FaultManagementRuntimeContext::Initialize(FaultManagement& config,
                                                       std::shared_ptr< PersistenceInterface >& per,
                                                       std::shared_ptr< DiagnosticDataManager >& dataManager)
{
    clusterName_ = config.clusterName;
    dtcSetting_  = std::make_shared< ControlDtcSetting >();
    std::int32_t const ret{nai_init()};
    if (ret < 0) {
        LogWarn() << "FaultManagementRuntimeContext::Initialize -> nai_init() err:" << ret
                  << "clusterName =" << clusterName_.c_str();
        return ret;
    }

    if (!_createCommonProperties(config)) {
        LogWarn() << "FaultManagementRuntimeContext::Initialize -> CreateCommonProperties fail, cluster="
                  << clusterName_.c_str();
        return -1;
    }
    if (!_createConditionGroups(config)) {
        LogWarn() << "FaultManagementRuntimeContext::Initialize -> CreateConditionGroups fail, cluster="
                  << clusterName_.c_str();
        return -1;
    }
    if (!_createOperationCycles(config)) {
        LogWarn() << "FaultManagementRuntimeContext::Initialize -> CreateOperationCycles fail, cluster="
                  << clusterName_.c_str();
        return -1;
    }
    if (!_createConfiguredDids(config, dataManager)) {
        LogWarn() << "FaultManagementRuntimeContext::Initialize -> CreateConfiguredDids fail, cluster="
                  << clusterName_.c_str();
        return -1;
    }
    if (!_createDTCGroups(config, per)) {
        LogWarn() << "FaultManagementRuntimeContext::Initialize -> CreateDTCGroups fail, cluster="
                  << clusterName_.c_str();
        return -1;
    }
    if (!_createAndMapEvents(config)) {
        LogWarn() << "FaultManagementRuntimeContext::Initialize -> CreateAndMapEvents fail, cluster="
                  << clusterName_.c_str();
        return -1;
    }
    LogVerbose() << "FaultManagementRuntimeContext::Initialize success. cluster=" << clusterName_.c_str();
    return 0;
}


int32_t FaultManagementRuntimeContext::Start()
{
    // OperationCycles
    for (auto& entry : operationCycles_) {
        if (entry.second.get() == nullptr) {
            LogWarn() << "FaultManagementRuntimeContext::Start -> start operation cycles fail. id =" << entry.first
                      << "cluster =" << clusterName_.c_str();
            continue;
        }
        entry.second->Start();
    }

    LogVerbose() << "FaultManagementRuntimeContext::Start success. cluster=" << clusterName_.c_str();
    return 0;
}

void FaultManagementRuntimeContext::Stop()
{
    LogVerbose() << "FaultManagementRuntimeContext::stop success";
    // DemEvents
    for (auto& itr : events_) {
        for (auto& dest : memDestinations_) {  // [SWS_DM_00028]
            std::shared_ptr< MemoryEntry > entry{dest.second->GetEntry(itr.second->GetDTCValue())};
            bool const isStorageCounter{itr.second->IsStorageCounter()};
            bool const isCounterBase{itr.second->GetDebounceType() == DebounceTypeEnum::kCounterBase};
            if (entry != nullptr && isStorageCounter && isCounterBase) {
                entry->SetFDC(itr.second->GetFaultDetectionCounter());
                break;
            }
        }
    }

    // OperationCycles
    std::shared_ptr< MemoryDestination > dest{GetPrimaryDestination()};
    for (auto& entry : operationCycles_) {
        entry.second->Stop();
        bool const isCycleStatusStorage{entry.second->IsCycleStatusStorage()};
        if (dest != nullptr && isCycleStatusStorage) {
            dest->AddOperationCycleState(entry.first, static_cast< uint8_t >(entry.second->GetOperationCycleState()));
        }
    }

    /// Data landing
    for (auto& itr : memDestinations_) {
        if (itr.second.get() == nullptr) {
            continue;
        }
        itr.second->Save();
    }
}


std::shared_ptr< Dtc > const FaultManagementRuntimeContext::GetDtc(uint32_t const id) const
{
    std::shared_ptr< Dtc > result{nullptr};
    std::map< uint32_t, std::shared_ptr< Dtc > >::const_iterator const dtcIt{dtcs_.find(id)};
    if (dtcIt != dtcs_.end()) {
        result = dtcIt->second;
    }
    return result;
}


std::shared_ptr< Dtc > FaultManagementRuntimeContext::GetDtcByValue(uint32_t const dtcValue) const
{
    std::map< uint32_t, std::shared_ptr< Dtc > >::const_iterator const dtcIt{dtcValueMapping_.find(dtcValue)};
    if (dtcIt != dtcValueMapping_.end()) {
        return dtcIt->second;
    }

    return std::shared_ptr< Dtc >{nullptr};
}


std::shared_ptr< DtcGroup > FaultManagementRuntimeContext::GetDtcGroup(uint32_t const groupNumber) const
{
    std::shared_ptr< DtcGroup > result{nullptr};
    std::map< uint32_t, std::shared_ptr< DtcGroup > >::const_iterator const dtcIt{dtcGroups_.find(groupNumber)};
    if (dtcIt != dtcGroups_.end()) {
        result = dtcIt->second;
    }
    return result;
}


std::vector< std::shared_ptr< Dtc > > FaultManagementRuntimeContext::GetAllDtc() noexcept
{
    std::vector< std::shared_ptr< Dtc > > dtcTable{};
    for (auto&& dtc : dtcs_) {
        dtcTable.emplace_back(dtc.second);
    }

    return dtcTable;
}

std::shared_ptr< MemoryDestination > FaultManagementRuntimeContext::GetDestination(uint32_t const id) const
{
    std::map< uint32_t, std::shared_ptr< MemoryDestination > >::const_iterator const it{memDestinations_.find(id)};
    if (it != memDestinations_.end()) {
        return it->second;
    }

    return std::shared_ptr< MemoryDestination >{nullptr};
}


std::shared_ptr< MemoryDestination > FaultManagementRuntimeContext::GetPrimaryDestination() const
{

    return GetDestination(kPrimaryDestinationId);
}


std::shared_ptr< FreezeFrame > FaultManagementRuntimeContext::GetFreezeFrame(std::uint32_t const number) const
{
    std::map< uint32_t, std::shared_ptr< FreezeFrame > >::const_iterator const it{ffs_.find(number)};
    if (it != ffs_.end()) {
        return it->second;
    }

    return std::shared_ptr< FreezeFrame >{nullptr};
}


std::shared_ptr< ExtendedDataRecord > FaultManagementRuntimeContext::GetExtendDataRecord(std::uint32_t const number)
{
    std::map< uint32_t, std::shared_ptr< ExtendedDataRecord > >::const_iterator const it{std::move(edrs_.find(number))};
    if (it != edrs_.end()) {
        return it->second;
    }

    return std::shared_ptr< ExtendedDataRecord >{nullptr};
}


bool FaultManagementRuntimeContext::_createCommonProperties(FaultManagement& config)
{
    commonProps_ = std::make_shared< DemDiagnosticCommonProps >(config.commonProps);
    if (commonProps_.get() == nullptr) {
        LogWarn() << "FaultManagementRuntimeContext::CreateCommonProperties|create commonProps_ fail";
        return false;
    }
    return true;
}


bool FaultManagementRuntimeContext::_createConfiguredDids(FaultManagement& config,
                                                          std::shared_ptr< DiagnosticDataManager >& dataManager)
{
    for (auto& entry : config.dataIdentifierSet) {
        std::shared_ptr< DataIdentifierSet > set{
            std::make_shared< DataIdentifierSet >(entry.first, entry.second, dataManager)};
        didSets_[entry.first] = set;
    }

    for (auto& entry : config.freezeFrameConfig) {
        std::shared_ptr< FreezeFrame > ff{std::make_shared< FreezeFrame >(entry.second)};
        ffs_[entry.first] = ff;
    }

    for (auto& entry : config.extendedDataRecordConfig) {
        std::shared_ptr< ExtendedDataRecord > edr{
            std::make_shared< ExtendedDataRecord >(*this, dataManager, entry.second)};
        edrs_[entry.first] = edr;
    }

    return true;
}


bool FaultManagementRuntimeContext::_createDTCGroups(FaultManagement& config,
                                                     std::shared_ptr< PersistenceInterface >& per)
{
    for (auto& entry : config.agingConfig) {
        std::shared_ptr< Aging > aging{std::make_shared< Aging >()};
        if (!aging->Init(entry.second, operationCycles_)) {
            LogWarn() << "FaultManagementRuntimeContext::CreateDTCGroups -> aging->Init fail. id=" << entry.first;
            return false;
        }
        agings_[entry.first] = aging;
    }

    std::vector< uint32_t > dtcList;
    std::vector< uint32_t > operationList;
    std::ignore = std::transform(config.dtcConfig.begin(), config.dtcConfig.end(), std::back_inserter(dtcList),
                                 [](std::pair< uint32_t, DtcConfig > const& p) { return p.second.dtcValue; });
    std::ignore
        = std::transform(operationCycles_.begin(), operationCycles_.end(), std::back_inserter(operationList),
                         [](std::pair< uint32_t, std::shared_ptr< OperationCycle > > const& p) { return p.first; });
    for (auto& entry : config.memoryDestinationConfig) {
        uint32_t memId = entry.first;
        std::shared_ptr< MemoryDestination > dest{nullptr};
        if (memId == kPrimaryDestinationId) {
            dest = std::make_shared< MemoryPrimary >(*this, entry.second, clusterName_, per);
        } else {
            dest = std::make_shared< MemoryUserDefined >(*this, entry.second, clusterName_, per);
        }

        if (!dest->Init(dtcList, operationList)) {
            LogWarn() << "FaultManagementRuntimeContext::CreateDTCGroups -> memory destination init failed id ="
                      << entry.first;
            return false;
        }

        memDestinations_[memId] = dest;
    }

    std::set< uint32_t > inPrimaryDtcPropsList;
    for (auto& entry : config.dtcPropsConfig) {
        std::shared_ptr< DtcProps > props{std::make_shared< DtcProps >()};
        if (!props->Init(entry.second, ffs_, edrs_, agings_, didSets_, memDestinations_)) {
            LogWarn() << "FaultManagementRuntimeContext::CreateDTCGroups -> invalid dtc props id=" << entry.first;
            return false;
        }
        dtcProps_[entry.first] = props;

        if (entry.second.memoryDestination.find(kPrimaryDestinationId) != entry.second.memoryDestination.end()) {
            std::ignore = inPrimaryDtcPropsList.insert(entry.second.dtcPropsID);
        }
    }

    for (auto& entry : config.dtcConfig) {
        std::shared_ptr< Dtc > dtc{std::make_shared< Dtc >(*this)};
        if (!dtc->Init(entry.second, dtcProps_, clearConditionGroups_)) {
            LogWarn() << "FaultManagementRuntimeContext::CreateDTCGroups -> idtc->Init fail. id=" << entry.first;
            return false;
        }
        dtcs_[entry.first]                = dtc;
        dtcValueMapping_[dtc->GetValue()] = dtc;

        if (inPrimaryDtcPropsList.find(entry.second.dtcPropsId) != inPrimaryDtcPropsList.end()) {
            std::ignore = inPrimaryDtcList_.insert(dtc->GetValue());
        }
    }

    for (auto& entry : config.dtcGroup) {
        if (entry.first == kGroupOfAllDTCs) {
            LogWarn() << "FaultManagementRuntimeContext::CreateDTCGroups -> invalid dtc group id=" << entry.first;
            continue;
        }

        if (entry.second.empty()) {
            LogWarn() << "FaultManagementRuntimeContext::CreateDTCGroups -> dtc group is empty id=" << entry.first;
            return false;
        }

        std::shared_ptr< DtcGroup > group{std::make_shared< DtcGroup >(entry.first, entry.second, dtcs_)};
        dtcGroups_[entry.first] = group;
    }

    _defineDefaultGroups();
    return true;
}

bool FaultManagementRuntimeContext::_createConditionGroups(FaultManagement& config)
{
    for (auto& entry : config.enableConditionConfig) {
        std::shared_ptr< Condition > condition{std::make_shared< Condition >(entry.second)};
        if (!condition->Init()) {
            return false;
        }
        enableConditions_[entry.first] = condition;
    }

    for (auto& entry : config.enableConditionGroup) {
        std::shared_ptr< ConditionGroup > group{std::make_shared< ConditionGroup >()};
        if (!group->Init(entry.second, enableConditions_, std::string("enableCondition"))) {
            return false;
        }
        enableConditionGroups_[entry.first] = group;
    }

    for (auto& entry : config.clearConditionConfig) {
        std::shared_ptr< Condition > condition{std::make_shared< Condition >(entry.second)};
        if (!condition->Init()) {
            return false;
        }
        clearConditions_[entry.first] = condition;
    }

    for (auto& entry : config.clearConditionGroup) {
        std::shared_ptr< ConditionGroup > group{std::make_shared< ConditionGroup >()};
        if (!group->Init(entry.second, clearConditions_, std::string("clearCondition"))) {
            return false;
        }
        clearConditionGroups_[entry.first] = group;
    }

    return true;
}


bool FaultManagementRuntimeContext::_createOperationCycles(FaultManagement& config)
{
    for (auto& entry : config.operationCycleConfig) {
        std::shared_ptr< OperationCycle > oc{std::make_shared< OperationCycle >(entry.second)};
        if (!oc->Init()) {
            return false;
        }
        operationCycles_[entry.first] = oc;
    }

    return true;
}


bool FaultManagementRuntimeContext::_createAndMapEvents(FaultManagement& config)
{
    for (auto& entry : config.indicatorConfig) {
        std::shared_ptr< Indicator > indicator{std::make_shared< Indicator >(entry.second)};
        if (!indicator->Init()) {
            LogWarn() << "FaultManagementRuntimeContext::CreateAndMapEvents -> indicator->Init fail id=" << entry.first;
            return false;
        }
        indicators_[entry.first] = indicator;
    }

    for (auto& entry : config.eventConfig) {
        uint32_t const eventId = entry.first;
        std::shared_ptr< Event > event{std::make_shared< Event >(*this)};
        if (!event->Init(entry.second, config.debounceAlgorithmConfig, indicators_, enableConditionGroups_,
                         operationCycles_, dtcs_, memDestinations_)) {
            LogWarn() << "FaultManagementRuntimeContext::CreateAndMapEvents -> dem_event->Init fail id=" << eventId;
            return false;
        }
        std::shared_ptr< Monitor > const mon{event->GetMonitor()};
        if (mon == nullptr) {
            LogWarn() << "FaultManagementRuntimeContext::CreateAndMapEvents -> dem_event get monitor fail id="
                      << eventId;
            return false;
        }
        monitors_.push_back(mon);
        events_[eventId] = event;
    }

    return true;
}


void FaultManagementRuntimeContext::_defineDefaultGroups()
{
    // [SWS_DM_00065].
    std::shared_ptr< DtcGroup > allDtcs{std::make_shared< DtcGroup >(kGroupOfAllDTCs, dtcs_)};
    std::ignore = dtcGroups_.insert(std::make_pair(kGroupOfAllDTCs, allDtcs));
}
}  // namespace server
}  // namespace uds
}  // namespace isoft
