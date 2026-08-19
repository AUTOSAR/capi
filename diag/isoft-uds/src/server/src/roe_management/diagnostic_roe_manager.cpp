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
/// @file       diagnostic_roe_manager.cpp
/// @brief      This file implements diagnostic_roe_manager functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=roe_management
/// @trace_id_sr=5c23e90c-715a-4d0d-8416-a4ebb6cf8eea
/// @unit_name=DiagnosticRoeManager
/// @unit_description=This file provides diagnostic_roe_manager functionality
/// @interface_level=unit
/// @endcode
///
/// ================================================================

#include "diagnostic_roe_manager.h"

#include "common/log.h"
#include "common/serialize.h"
#include "common/thread_pool.h"
#include "isoft/uds/type_int.h"
namespace isoft {
namespace uds {
namespace server {
std::int32_t LocalizationValue(std::uint16_t localization,
                               bool& sign,
                               std::uint8_t& length,
                               std::uint16_t& offset) noexcept
{
    std::uint16_t const signMask{0X80U};
    std::uint16_t const lengthMask{0x3C00U};
    std::uint16_t const lengthPos{0x09U};
    std::uint16_t const offsetMask{0x01FFU};
    sign   = 0 != (localization & signMask);
    length = static_cast< std::uint8_t >((localization & lengthMask) >> lengthPos);
    offset = localization & offsetMask;
    return 0;
}

void EventTable::Add(EventPtr const& eventPtr) noexcept { table_.push_back(eventPtr); }
std::vector< EventPtr >& EventTable::Get() noexcept { return table_; }

std::uint8_t EventTable::GetEventWindowTime() noexcept
{
    if (table_.empty()) {
        return 0;
    }
    return table_.back()->GetEventSetup().eventWindowTime;
}

std::vector< std::uint8_t > EventTable::GetEventTypeRecord() noexcept
{
    if (table_.empty()) {
        return {};
    }
    return table_.back()->GetEventSetup().eventTypeRecord;
}

std::vector< std::uint8_t > EventTable::GetServiceToRespondToRecord() noexcept
{
    if (table_.empty()) {
        return {};
    }
    return table_.back()->GetEventSetup().serviceToRespondToRecord;
}

void EventTable::Clear() noexcept { table_.clear(); }

EventProcessor::EventProcessor(EventTable& eventTable) noexcept : eventTable_{eventTable} {}

void EventProcessor::Process() noexcept
{
    LogVerbose() << "EventProcessor::Process| start!";
    std::vector< EventPtr > eventTable{eventTable_.Get()};
    for (auto&& event : eventTable) {
        std::shared_ptr< Data > dataPtr{nullptr};
        std::int32_t result{0};
        result = event->SampleData(dataPtr);
        if (result != 0) {
            LogWarn() << "EventProcessor::Process| sample data fails,error:" << result;
            continue;
        }
        result = event->CompareData(dataPtr);
        if (result != 0) {
            LogWarn() << "EventProcessor::Process| compare data fails,error:" << result;
            continue;
        }
        result = event->Occurred();
        if (result != 0) {
            LogWarn() << "EventProcessor::Process| event occurred fails,error:" << result;
            continue;
        }
    }
    LogVerbose() << "EventProcessor::Process| end!";
}

std::uint16_t& CountByDTCStatus::Value() noexcept { return count_; }

DTCStatusChange::DTCStatusChange(std::shared_ptr< DtcFilter > dtcFilterPtr) noexcept
    : dtcFilterPtr_{std::move(dtcFilterPtr)}
{
}

std::int32_t DTCStatusChange::Initialize(EventSetup& setup) noexcept
{
    if (!setup_.eventTypeRecord.empty()) {
        dtcStatusMask_ = setup_.eventTypeRecord.back();
        return -1;
    }
    setup_ = setup;
    return 0;
}

void DTCStatusChange::SetRequestCallback(std::function< void(std::vector< std::uint8_t >) > callback) noexcept
{
    callback_ = std::move(callback);
}

std::int32_t DTCStatusChange::SampleData(std::shared_ptr< Data >& dataPtr) noexcept
{
    std::int32_t result{0};
    if (dtcFilterPtr_.get() != nullptr) {
        uint8_t dtcStatusAvailabilityMask{kInt8_0xFFU};
        uint8_t dtcFormatIdentifier{};
        std::shared_ptr< CountByDTCStatus > countByDTCStatusPtr{std::make_shared< CountByDTCStatus >()};
        result = dtcFilterPtr_->GetNumberOfDTCByStatusMask(dtcStatusMask_, dtcStatusAvailabilityMask,
                                                           dtcFormatIdentifier, countByDTCStatusPtr->Value());
        if (result == 0) {
            dataPtr = countByDTCStatusPtr;
        }
    } else {
        result = -1;
    }
    return result;
}

std::int32_t DTCStatusChange::CompareData(std::shared_ptr< Data >& dataPtr) noexcept
{
    std::shared_ptr< CountByDTCStatus > countByDTCStatusPtr{std::dynamic_pointer_cast< CountByDTCStatus >(dataPtr)};
    if (countByDTCStatusPtr.get() != nullptr) {
        if (first_) {
            first_            = false;
            countByDTCStatus_ = *countByDTCStatusPtr;
        } else {
            if (countByDTCStatus_.Value() != countByDTCStatusPtr->Value()) {
                countByDTCStatus_ = *countByDTCStatusPtr;
                return 0;
            }
        }
    }
    return -1;
}

std::int32_t DTCStatusChange::Occurred() noexcept
{
    callback_(setup_.serviceToRespondToRecord);
    return 0;
}

std::int32_t DTCStatusChange::Active(MetaInfoMap&) noexcept { return 0; }

EventSetup& DTCStatusChange::GetEventSetup() noexcept { return setup_; }

DidData::DidData(DiagnosticData& data) noexcept : data_{data} {}

DiagnosticData& DidData::Get() noexcept { return data_; }
using DidDataPtr = std::shared_ptr< DidData >;

ChangeOfDataIdentifier::ChangeOfDataIdentifier(
    std::shared_ptr< DiagnosticDataManager > diagnosticDataManagerPtr) noexcept
    : diagnosticDataManagerPtr_{std::move(diagnosticDataManagerPtr)}
{
}

std::int32_t ChangeOfDataIdentifier::Initialize(EventSetup& setup) noexcept
{
    std::size_t const eventTypeRecordSize{0x02};
    if (setup.eventTypeRecord.size() != eventTypeRecordSize) {
        return -1;
    }
    Serialize deserialize{setup.eventTypeRecord};
    did_   = deserialize.ReadUint16();
    setup_ = setup;
    return 0;
}

void ChangeOfDataIdentifier::SetRequestCallback(std::function< void(std::vector< std::uint8_t >) > callback) noexcept
{
    callback_ = std::move(callback);
}

std::int32_t ChangeOfDataIdentifier::SampleData(std::shared_ptr< Data >& dataPtr) noexcept
{
    if (diagnosticDataManagerPtr_.get() != nullptr) {
        std::vector< std::uint16_t > didTable{did_};
        std::shared_ptr< CancellationState > cancellationStatePtr{std::make_shared< CancellationState >()};
        Result< std::list< DiagnosticData > > readResult;
        readResult = diagnosticDataManagerPtr_->Read(didTable, metaInfo_, cancellationStatePtr);
        if (readResult.HasValue()) {
            std::list< DiagnosticData >& didDataTable{readResult.Value()};

            if (!didDataTable.empty()) {
                DiagnosticData& didData = didDataTable.back();
                dataPtr                 = std::make_shared< DidData >(didData);
                LogVerbose() << "ChangeOfDataIdentifier::SampleData| read did data success! did:" << did_;
                return 0;
            }
            LogVerbose() << "ChangeOfDataIdentifier::SampleData| read did data success! did:" << did_;
        } else {
            LogWarn() << "ChangeOfDataIdentifier::SampleData| read did data fails! did:" << did_;
        }
    } else {
        LogWarn() << "ChangeOfDataIdentifier::SampleData| diagnosticDataManagerPtr_ is nullptr";
    }
    return -1;
}

std::int32_t ChangeOfDataIdentifier::CompareData(std::shared_ptr< Data >& dataPtr) noexcept
{
    DidDataPtr didDataPtr{std::dynamic_pointer_cast< DidData >(dataPtr)};
    if (dataPtr_.get() == nullptr || didDataPtr->Get().record != dataPtr_->Get().record) {
        dataPtr_ = didDataPtr;
        return 0;
    }
    return -1;
}

std::int32_t ChangeOfDataIdentifier::Occurred() noexcept
{
    callback_(setup_.serviceToRespondToRecord);
    return 0;
}

std::int32_t ChangeOfDataIdentifier::Active(MetaInfoMap& metaInfo) noexcept
{
    metaInfo_ = metaInfo;
    return 0;
}

EventSetup& ChangeOfDataIdentifier::GetEventSetup() noexcept { return setup_; }

ComparisonOfValue::ComparisonOfValue(std::shared_ptr< DiagnosticDataManager > diagnosticDataManagerPtr) noexcept
    : diagnosticDataManagerPtr_{std::move(diagnosticDataManagerPtr)}, callback_{}, record_{}, setup_{}, metaInfo_{}
{
}

std::int32_t ComparisonOfValue::Initialize(EventSetup& setup) noexcept
{
    std::size_t const eventTypeRecordSize{0x0A};
    if (setup.eventTypeRecord.size() != eventTypeRecordSize) {
        return -1;
    }
    Serialize deserialize{setup.eventTypeRecord};
    record_.did             = deserialize.ReadUint16();
    record_.comparisonLogic = deserialize.ReadUint8();
    record_.comparisonValue = deserialize.ReadUint32();
    record_.hysteresisValue = deserialize.ReadUint8();
    std::uint16_t const localization{deserialize.ReadUint16()};
    LocalizationValue(localization, record_.sign, record_.length, record_.offset);
    return 0;
}

void ComparisonOfValue::SetRequestCallback(std::function< void(std::vector< std::uint8_t >) > callback) noexcept
{
    callback_ = std::move(callback);
}

std::int32_t ComparisonOfValue::SampleData(std::shared_ptr< Data >& dataPtr) noexcept
{
    if (diagnosticDataManagerPtr_.get() != nullptr) {
        std::vector< std::uint16_t > didTable{record_.did};
        std::shared_ptr< CancellationState > cancellationStatePtr{std::make_shared< CancellationState >()};
        Result< std::list< DiagnosticData > > readResult;
        readResult = diagnosticDataManagerPtr_->Read(didTable, metaInfo_, cancellationStatePtr);
        if (readResult.HasValue()) {
            std::list< DiagnosticData >& didDataTable{readResult.Value()};
            if (!didDataTable.empty()) {
                DiagnosticData& didData = didDataTable.back();
                dataPtr                 = std::make_shared< DidData >(didData);
                return 0;
            }
        }
    }
    return -1;
}

std::int32_t ComparisonOfValue::CompareData(std::shared_ptr< Data >& dataPtr) noexcept
{
    DidDataPtr didDataPtr{std::dynamic_pointer_cast< DidData >(dataPtr)};
    DiagnosticData& diagnosticData{didDataPtr->Get()};

    std::size_t const startPos{static_cast< std::size_t >(record_.offset / static_cast< std::uint8_t >(INT8_WIDTH))};
    std::size_t const endPos{(record_.offset + record_.length - 1U) / INT8_WIDTH};
    std::size_t const validLength{endPos - startPos + 1};
    std::size_t const offsetBit{static_cast< std::size_t >(record_.offset % INT8_WIDTH)};
    if (diagnosticData.record.size() < endPos + 1) {
        return -1;
    }

    
    Serialize deserialize(diagnosticData.record);
    deserialize.Seek(startPos);
    if (record_.sign == false) {
        std::uint64_t rawValue = deserialize.ReadUint64(validLength);
        rawValue <<= offsetBit;
        rawValue >>= (UINT64_WIDTH - record_.length);
        std::uint32_t const measuredValue{static_cast< std::uint32_t >(rawValue)};
        if (record_.comparisonLogic == 1U) {
            std::uint32_t const minComparisonValue{record_.comparisonValue * (1 - record_.hysteresisValue / 100)};
            if (minComparisonValue < measuredValue) {
                return 0;
            }
            return -1;
        }
        if (record_.comparisonLogic == 2U) {
            std::uint32_t const maxComparisonValue{record_.comparisonValue * (1 + record_.hysteresisValue / 100)};
            if (maxComparisonValue > measuredValue) {
                return 0;
            }
            return -1;
        }
        if (record_.comparisonLogic == 3U) {
            if (record_.comparisonValue == measuredValue) {
                return 0;
            }
            return -1;
        }
        if (record_.comparisonLogic == 4U) {
            if (record_.comparisonValue != measuredValue) {
                return 0;
            }
            return -1;
        }
    } else {
        std::int64_t rawValue = static_cast< std::int64_t >(deserialize.ReadUint64(validLength));
        rawValue <<= static_cast< int64_t >(offsetBit);
        rawValue >>= (INT64_WIDTH - record_.length);
        std::int32_t const measuredValue{static_cast< std::int32_t >(rawValue)};
        std::int32_t comparisonValue{static_cast< std::int32_t >(record_.comparisonValue)};
        if (record_.comparisonLogic == 1U) {
            if (comparisonValue < 0L) {
                std::int32_t const minComparisonValue{comparisonValue * (1 + record_.hysteresisValue / 100)};
                if (minComparisonValue < measuredValue) {
                    return 0;
                }
                return -1;
            }
            std::int32_t const minComparisonValue{comparisonValue * (1 - record_.hysteresisValue / 100)};
            if (minComparisonValue < measuredValue) {
                return 0;
            }
            return -1;
        }
        if (record_.comparisonLogic == 2U) {
            if (comparisonValue < 0) {
                std::int32_t const maxComparisonValue{comparisonValue * (1 - record_.hysteresisValue / 100)};
                if (maxComparisonValue > measuredValue) {
                    return 0;
                }
                return -1;
            }
            std::int32_t const maxComparisonValue{comparisonValue * (1 + record_.hysteresisValue / 100)};
            if (maxComparisonValue > measuredValue) {
                return 0;
            }
            return -1;
        }
        if (record_.comparisonLogic == 3U) {
            if (comparisonValue == measuredValue) {
                return 0;
            }
            return -1;
        }
        if (record_.comparisonLogic == 4U) {
            if (comparisonValue != measuredValue) {
                return 0;
            }
            return -1;
        }
    }
    return -1;
}

std::int32_t ComparisonOfValue::Occurred() noexcept
{
    callback_(setup_.serviceToRespondToRecord);
    return 0;
}

std::int32_t ComparisonOfValue::Active(MetaInfoMap& metaInfo) noexcept
{
    metaInfo_ = metaInfo;
    return 0;
}

EventSetup& ComparisonOfValue::GetEventSetup() noexcept { return setup_; }

ResponseOnEventService::ResponseOnEventService(SessionId sessionid) noexcept : sessionid_{sessionid} {}

void ResponseOnEventService::_sendRequest(std::vector< std::uint8_t > data) noexcept
{
    requestCallback_(std::move(data));
}

std::int32_t ResponseOnEventService::Initialize(
    RoeManagement& config,
    std::shared_ptr< DiagnosticDataManager > const& dataManager,
    std::shared_ptr< DtcFilter > const& dtcFilterPtr,
    std::shared_ptr< PersistenceService > const& persistenceServicePtr) noexcept
{
    if (persistenceServicePtr.get() == nullptr) {
        return -1;
    }
    diagnosticDataManagerPtr_ = std::move(dataManager);
    dtcFilterPtr_             = std::move(dtcFilterPtr);
    persistenceServicePtr_    = persistenceServicePtr;
    config_                   = config;
    std::uint8_t const dtcStatusChange{0x02U};
    std::uint8_t const changeOfDataIdentifier{0x03U};
    std::uint8_t const comparisonOfValues{0x07U};
    std::string const eventArressTable{"ResponseOnEventService|eventAddressTable"};
    std::vector< std::uint64_t > eventAddressTable;
    _loadEventTable(eventAddressTable);
    for (auto&& eventAddress : eventAddressTable) {
        std::vector< std::uint8_t > eventData;
        _loadEvent(eventAddress, eventData);
        if (eventData.empty()) {
            continue;
        }
        Serialize deserialize{eventData};
        EventSetup setup;
        setup.eventType       = deserialize.ReadUint8();
        setup.eventWindowTime = deserialize.ReadUint8();
        if (dtcStatusChange == setup.eventType) {
            std::uint8_t dtcStatusMask = deserialize.ReadUint8();
            setup.eventTypeRecord.push_back(dtcStatusMask);
        } else if (changeOfDataIdentifier == setup.eventType) {
            std::size_t count{2U};
            for (size_t i = 0; i < count; i++) {
                std::uint8_t temp = deserialize.ReadUint8();
                setup.eventTypeRecord.push_back(temp);
            }
        } else if (comparisonOfValues == setup.eventType) {
            std::size_t count{kInt32_10U};
            for (size_t i = 0; i < count; i++) {
                std::uint8_t temp = deserialize.ReadUint8();
                setup.eventTypeRecord.push_back(temp);
            }
        }
        setup.serviceToRespondToRecord = deserialize.ReadAll();
        Setup(setup);
    }
    timer_.Initialize([this]() { ThreadPool::GetInstance().Submit([this]() { _timeout(); }); },
                      config.responseOnEventSchedulerRate);
    return 0;
}

void ResponseOnEventService::_loadEvent(std::uint64_t& address, std::vector< std::uint8_t >& eventData) noexcept
{
    std::string eventKey{"ResponseOnEventService|eventData|"};
    std::ignore = eventKey.append(std::to_string(address));
    eventData   = persistenceServicePtr_->LoadData(eventKey);
}

void ResponseOnEventService::_saveEvent(std::uint64_t key,
                                        std::uint8_t& eventType,
                                        std::uint8_t& eventWindowTime,
                                        std::vector< std::uint8_t >& eventTypeRecord,
                                        std::vector< std::uint8_t >& serviceToRespondToRecord) noexcept
{
    std::string eventKey{"ResponseOnEventService|eventData|"};
    std::ignore = eventKey.append(std::to_string(key));
    std::vector< std::uint8_t > eventData;

    Serialize serialize{eventData};
    serialize.Write(eventType);
    serialize.Write(eventWindowTime);
    serialize.Write(eventTypeRecord);
    serialize.Write(serviceToRespondToRecord);
    std::ignore = persistenceServicePtr_->SaveData(eventKey, eventData);
}

void ResponseOnEventService::_loadEventTable(std::vector< std::uint64_t >& table) noexcept
{
    std::string const eventArressTable{"ResponseOnEventService|eventAddressTable"};
    std::vector< std::uint8_t > data;
    data = persistenceServicePtr_->LoadData(eventArressTable);
    if (data.empty()) {
        return;
    }
    Serialize deserialize{data};
    std::uint64_t address{0U};
    while (deserialize.Read(address)) {
        table.push_back(address);
    }
}

void ResponseOnEventService::_saveEventTable(std::vector< std::uint64_t >& table) noexcept
{
    std::string const eventArressTable{"ResponseOnEventService|eventAddressTable"};
    std::vector< std::uint8_t > eventData;
    Serialize serialize{eventData};
    for (auto&& address : table) {
        serialize.Write(address);
    }
    std::ignore = persistenceServicePtr_->SaveData(eventArressTable, eventData);
}

void ResponseOnEventService::_saveNewEventAddress(std::uint64_t& eventAddress) noexcept
{
    std::vector< std::uint64_t > table;
    _loadEventTable(table);
    table.push_back(eventAddress);
    _saveEventTable(table);
}

void ResponseOnEventService::_clearEventTable() noexcept
{
    std::string const eventArressTable{"ResponseOnEventService|eventAddressTable"};
    std::ignore = persistenceServicePtr_->RemoveData(eventArressTable);
}
void ResponseOnEventService::_clearEvent(std::uint64_t& address) noexcept
{
    std::string eventKey{"ResponseOnEventService|eventData|"};
    std::ignore = eventKey.append(std::to_string(address));
    std::ignore = persistenceServicePtr_->RemoveData(eventKey);
}

void ResponseOnEventService::Setup(EventSetup& setup) noexcept
{
    std::uint8_t const dtcStatusChange{0x02U};
    std::uint8_t const changeOfDataIdentifier{0x03U};
    std::uint8_t const comparisonOfValues{0x07U};
    if (setup.store) {
        if (!config_.storeEventEnabled) {
            LogError() << "ResponseOnEventService::Setup| not allow store event! if must store event, please config "
                          "RoeManagement.storeEventEnabled = true";
            return;
        }
        if (setup.eventWindowTime != 0x02) {
            LogError()
                << "ResponseOnEventService::Setup| not allow store event! if must store event, eventWindowTime = 2";
            return;
        }
    }
    if (setup.eventType == dtcStatusChange) {
        if (eventType_ != EventType::kUnknow && eventType_ != EventType::kDTCStatusChange) {
            Clear();
            eventType_ = EventType::kUnknow;
        }
        if (eventType_ == EventType::kUnknow) {
            processorPtr_ = std::make_shared< EventProcessor >(eventTable_);
            eventType_    = EventType::kDTCStatusChange;
        }
        std::shared_ptr< DTCStatusChange > dtcStatusChangePtr{std::make_shared< DTCStatusChange >(dtcFilterPtr_)};
        dtcStatusChangePtr->Initialize(setup);
        dtcStatusChangePtr->SetRequestCallback(
            [this](std::vector< std::uint8_t > data) { _sendRequest(std::move(data)); });
        eventTable_.Add(dtcStatusChangePtr);

    } else if (setup.eventType == comparisonOfValues) {
        if (eventType_ != EventType::kUnknow && eventType_ != EventType::kCompareDataValues) {
            Clear();
            eventType_ = EventType::kUnknow;
        }
        if (eventType_ == EventType::kUnknow) {
            processorPtr_ = std::make_shared< EventProcessor >(eventTable_);
            eventType_    = EventType::kCompareDataValues;
        }
        std::shared_ptr< ComparisonOfValue > comparisonOfValuePtr{
            std::make_shared< ComparisonOfValue >(diagnosticDataManagerPtr_)};
        comparisonOfValuePtr->Initialize(setup);
        comparisonOfValuePtr->SetRequestCallback(
            [this](std::vector< std::uint8_t > data) { _sendRequest(std::move(data)); });
        eventTable_.Add(comparisonOfValuePtr);

    } else if (setup.eventType == changeOfDataIdentifier) {
        if (eventType_ != EventType::kUnknow && eventType_ != EventType::kChangeDataIdentifier) {
            Clear();
            eventType_ = EventType::kUnknow;
        }
        if (eventType_ == EventType::kUnknow) {
            processorPtr_ = std::make_shared< EventProcessor >(eventTable_);
            eventType_    = EventType::kChangeDataIdentifier;
        }
        std::shared_ptr< ChangeOfDataIdentifier > changeOfDataIdentifierPtr{
            std::make_shared< ChangeOfDataIdentifier >(diagnosticDataManagerPtr_)};
        changeOfDataIdentifierPtr->Initialize(setup);
        changeOfDataIdentifierPtr->SetRequestCallback(
            [this](std::vector< std::uint8_t > data) { _sendRequest(std::move(data)); });
        eventTable_.Add(changeOfDataIdentifierPtr);
    }
}

void ResponseOnEventService::SetRequestCallback(
    std::function< void(std::vector< std::uint8_t > const&) > const& callback) noexcept
{
    requestCallback_ = std::move(callback);
}

NrcErrc ResponseOnEventService::Start(bool store, std::shared_ptr< RequestContext > const& requestContext) noexcept
{
    if (store) {
        for (auto&& eventPtr : eventTable_.Get()) {
            std::uintptr_t const ptr{reinterpret_cast< std::uintptr_t >(eventPtr.get())};
            std::uint64_t address{ptr};
            _saveNewEventAddress(address);
            EventSetup& setup{eventPtr->GetEventSetup()};
            _saveEvent(address, setup.eventType, setup.eventWindowTime, setup.eventTypeRecord,
                       setup.serviceToRespondToRecord);
        }
    }
    metaInfo_ = requestContext->GetMetaInfoMap();
    bool active{false};
    for (auto&& eventPtr : eventTable_.Get()) {
        active      = true;
        std::ignore = eventPtr->Active(metaInfo_);
    }
    if (active) {
        std::ignore = timer_.Start();
        {
            std::unique_lock< std::mutex > lock{lock_};
            stop_ = false;
        }
        return NrcErrc::kSuccess;
    }
    return NrcErrc::kRequestSequenceError;
}

void ResponseOnEventService::Active() noexcept
{
    for (auto&& eventPtr : eventTable_.Get()) {
        std::ignore = eventPtr->Active(metaInfo_);
    }
    std::ignore = timer_.Start();
    {
        std::unique_lock< std::mutex > lock{lock_};
        stop_ = false;
    }
}

void ResponseOnEventService::Stop() noexcept
{
    int32_t result = timer_.Stop();
    LogVerbose() << "ResponseOnEventService::Stop| timer stop result:" << result;
    std::unique_lock< std::mutex > lock{lock_};
    stop_ = true;
}

void ResponseOnEventService::Clear() noexcept
{
    std::vector< std::uint64_t > table;
    _loadEventTable(table);
    for (auto&& address : table) {
        _clearEvent(address);
    }
    _clearEventTable();
    eventTable_.Clear();
}

std::int32_t ResponseOnEventService::Report(std::uint8_t& eventType,
                                            std::uint8_t& eventWindowTime,
                                            std::vector< std::uint8_t >& eventTypeRecord,
                                            std::vector< std::uint8_t >& serviceToRespondToRecord) noexcept
{
    std::uint8_t const dtcStatusChange{0x02U};
    std::uint8_t const changeOfDataIdentifier{0x03U};
    std::uint8_t const comparisonOfValues{0x07U};
    switch (eventType) {
        case static_cast< std::uint8_t >(EventType::kDTCStatusChange): {
            eventType = dtcStatusChange;
        } break;
        case static_cast< std::uint8_t >(EventType::kChangeDataIdentifier): {
            eventType = changeOfDataIdentifier;
        } break;
        case static_cast< std::uint8_t >(EventType::kCompareDataValues): {
            eventType = comparisonOfValues;
        } break;
        default:
            break;
    }
    eventWindowTime          = eventTable_.GetEventWindowTime();
    eventTypeRecord          = eventTable_.GetEventTypeRecord();
    serviceToRespondToRecord = eventTable_.GetServiceToRespondToRecord();
    return 0;
}

void ResponseOnEventService::_timeout() noexcept
{
    {
        std::unique_lock< std::mutex > lock{lock_};
        if (stop_) {
            return;
        }
    }
    processorPtr_->Process();
    timer_.Start();
}

std::int32_t DiagnosticRoeManager::Initialize(
    RoeManagement& config,
    std::shared_ptr< SessionManager > const& sessionManagerPtr,
    std::shared_ptr< DiagnosticDataManager > const& diagnosticDataManagerPtr,
    std::shared_ptr< DtcFilter > const& dtcFilterPtr,
    std::shared_ptr< PersistenceService > const& persistenceServicePtr) noexcept
{
    if (sessionManagerPtr.get() == nullptr) {
        LogError() << "DiagnosticRoeManager::Initialize| sessionManagerPtr is nullptr!";
        return -1;
    }
    sessionManagerPtr_ = sessionManagerPtr;
    NotifyOfSessionChangeCallback sessionChangeCallback{[this](SessionId oldSessionId, SessionId newSessionId) {
        auto findService = responseOnEventServiceTable_.find(oldSessionId);
        if (findService != responseOnEventServiceTable_.end()) {
            findService->second->Stop();
        }
        if (newSessionId == 0x01) {
            findService = responseOnEventServiceTable_.find(newSessionId);
            if (findService != responseOnEventServiceTable_.end()) {
                findService->second->Active();
            }
        }
    }};
    std::ignore = sessionManagerPtr_->AddNotifyOfSessionChange(sessionChangeCallback);
    if (diagnosticDataManagerPtr.get() == nullptr) {
        LogError() << "DiagnosticRoeManager::Initialize| diagnosticDataManagerPtr is nullptr!";
        return -1;
    }
    diagnosticDataManagerPtr_ = diagnosticDataManagerPtr;
    if (dtcFilterPtr.get() == nullptr) {
        LogError() << "DiagnosticRoeManager::Initialize| dtcFilterPtr is nullptr!";
        return -1;
    }
    dtcFilterPtr_ = dtcFilterPtr;
    if (persistenceServicePtr.get() == nullptr) {
        LogError() << "DiagnosticRoeManager::Initialize| persistenceServicePtr is nullptr!";
        return -1;
    }
    persistenceServicePtr_ = persistenceServicePtr;
    std::string key{"DiagnosticRoeManager"};
    std::vector< std::uint8_t > data = persistenceServicePtr_->LoadData(key);
    for (auto&& sessionId : data) {
        std::shared_ptr< ResponseOnEventService > roeServicePtr{std::make_shared< ResponseOnEventService >(sessionId)};
        roeServicePtr->SetRequestCallback(
            [this](std::vector< std::uint8_t > const& requestData) { _sendRequest(requestData); });
        roeServicePtr->Initialize(config, diagnosticDataManagerPtr_, dtcFilterPtr_, persistenceServicePtr_);
        std::ignore = responseOnEventServiceTable_.insert({sessionId, roeServicePtr});
    }
    config_ = config;
    requestTimer_.Initialize(
        [this]() {
            if (requestList_.empty()) {
                LogInfo() << "DiagnosticRoeManager::Initialize| request list is empty!";
                requestTimer_.Stop();
                return;
            }
            std::vector< std::uint8_t > requestData{requestList_.front()};
            requestList_.pop_front();
            _sendRequest(requestData);
        },
        config.interMessageTime);

    return 0;
}

void DiagnosticRoeManager::Setup(EventSetup& setup) noexcept
{
    SessionId currentSessionId = sessionManagerPtr_->GetCurrentSessionId();
    auto findService           = responseOnEventServiceTable_.find(currentSessionId);
    if (findService != responseOnEventServiceTable_.end()) {
        findService->second->Setup(setup);
        return;
    }
    std::shared_ptr< ResponseOnEventService > roeServicePtr{
        std::make_shared< ResponseOnEventService >(currentSessionId)};
    roeServicePtr->SetRequestCallback(
        [this](std::vector< std::uint8_t > requestData) { _sendRequest(std::move(requestData)); });
    roeServicePtr->Initialize(config_, diagnosticDataManagerPtr_, dtcFilterPtr_, persistenceServicePtr_);
    roeServicePtr->Setup(setup);
    std::ignore = responseOnEventServiceTable_.insert({currentSessionId, roeServicePtr});
}

NrcErrc DiagnosticRoeManager::Start(bool store, std::shared_ptr< RequestContext > const& requestContext) noexcept
{
    SessionId currentSessionId = sessionManagerPtr_->GetCurrentSessionId();
    auto findService           = responseOnEventServiceTable_.find(currentSessionId);
    clientAddress_             = requestContext->GetRequest()->GetSA();
    if (findService != responseOnEventServiceTable_.end()) {
        return findService->second->Start(store, requestContext);
    }
    return NrcErrc::kRequestSequenceError;
}

void DiagnosticRoeManager::Stop() noexcept
{
    SessionId currentSessionId = sessionManagerPtr_->GetCurrentSessionId();
    auto findService           = responseOnEventServiceTable_.find(currentSessionId);
    if (findService != responseOnEventServiceTable_.end()) {
        findService->second->Stop();
        return;
    }
    LogError() << "DiagnosticRoeManager::Stop| not find ResponseOnEventService in current session,session id:"
               << currentSessionId;
    return;
}

void DiagnosticRoeManager::Clear() noexcept
{
    SessionId currentSessionId = sessionManagerPtr_->GetCurrentSessionId();
    auto findService           = responseOnEventServiceTable_.find(currentSessionId);
    if (findService != responseOnEventServiceTable_.end()) {
        findService->second->Clear();
        return;
    }
    LogError() << "DiagnosticRoeManager::Stop| not find ResponseOnEventService in current session,session id:"
               << currentSessionId;
    return;
}

std::int32_t DiagnosticRoeManager::Report(std::uint8_t& eventType,
                                          std::uint8_t& eventWindowTime,
                                          std::vector< std::uint8_t >& eventTypeRecord,
                                          std::vector< std::uint8_t >& serviceToRespondToRecord) noexcept
{
    SessionId currentSessionId = sessionManagerPtr_->GetCurrentSessionId();
    auto findService           = responseOnEventServiceTable_.find(currentSessionId);
    if (findService != responseOnEventServiceTable_.end()) {
        return findService->second->Report(eventType, eventWindowTime, eventTypeRecord, serviceToRespondToRecord);
    }
    LogError() << "DiagnosticRoeManager::Report| not find ResponseOnEventService in current session,session id:"
               << currentSessionId;
    return -1;
}

void DiagnosticRoeManager::_sendRequest(std::vector< std::uint8_t > data) noexcept
{
    if (requestTimer_.IsRunning()) {
        requestList_.push_back(data);
        return;
    }
    std::ignore = requestTimer_.Start();
    LogDebug() << "DiagnosticRoeManager::SendRequest|clientAddress:" << clientAddress_;
    std::shared_ptr< Channel > channelPtr{_getChannel(clientAddress_)};
    if (channelPtr.get() != nullptr) {
        std::shared_ptr< Message > requestMessagePtr{std::make_shared< Message >(clientAddress_, serverAddress_)};
        std::vector< std::uint8_t >& body{requestMessagePtr->GetBody()};
        body.insert(body.end(), data.begin(), data.end());
        LogDebug() << "DiagnosticRoeManager::SendRequest|indicate request message";
        Result< void > indicateResult = _indicate(requestMessagePtr, priority_, channelPtr);
        if (indicateResult.HasValue()) {
            LogDebug() << "DiagnosticRoeManager::SendRequest| indicate success!";
        } else {
            LogWarn() << "DiagnosticRoeManager::SendRequest| indicate fails!";
        }
    }
}

}  // namespace server
}  // namespace uds
}  // namespace isoft