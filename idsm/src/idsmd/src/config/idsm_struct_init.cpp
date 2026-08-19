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
/// @file       idsm_struct_init.cpp
/// @brief      Configuration item pool. Parse configuration and store for subsequent use
/// @details
/// @date       2024-05-12
/// @author     niuliming
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/idsm/Configuration Center
/// @interface_level=module
/// @trace_id_sr=SR_IDSM_0006
/// @unit_name=IdsmStruct
/// @unit_description=
/// @endcode
///
/// ================================================================

#include "idsm_struct_init.h"

#include <ara/core/string.h>
#include <ara/core/string_view.h>
#include <isoft/ara_fsh/filesystem_hierarchy.h>

#include "event/idsm_event_proper.h"
#include "filter/idsm_filter_chain.h"
#include "filter/idsm_sample_filter.h"
#include "filter/idsm_state_filter.h"
#include "filter/idsm_thres_filter.h"
#include "log/idsm_log.h"
#ifdef ARA_WITH_DIAG
    #include "diag/idsm_diag_did_server.h"
    #include "diag/idsm_diag_dtc_server.h"
#endif
namespace ara {
namespace idsm {
/// @brief Unique instance of IdsmStruct
std::shared_ptr< IdsmStruct > IdsmStruct::s_SingleInstance_{nullptr};  // NOLINT
/// @brief Mutex used for thread-safe singleton pattern
std::mutex IdsmStruct::s_SingleMutex_{};
/// @brief Get the unique instance of IdsmStruct
/// @return IdsmStruct instance
std::shared_ptr< IdsmStruct > IdsmStruct::GetInstance() noexcept
{
    if (s_SingleInstance_ == nullptr) {
        std::lock_guard< std::mutex > const lck{s_SingleMutex_};
        if (s_SingleInstance_ == nullptr) {
            s_SingleInstance_.reset(new IdsmStruct);
        }
    }
    return s_SingleInstance_;
}
/// @brief Configuration parsing initialization
/// @return 0: success, non-zero: failure
/// @exception stack overflow exception
IdsmErrorCode IdsmStruct::StructInit() noexcept
{
    uint16_t constexpr kNum1024{1024U};
    memLimitUnit_ = kNum1024 * kNum1024;

    isoft::ara_fsh::Platform const fsh;
    ara::core::String const jsonFile{fsh.GetPlatformEtcDir() + "idsm_contribute.json"};
    return StructParseByPath(jsonFile);
}
/// @brief TODO: public for testing, private for non-testing
/// @param filtPath configuration file path
/// @return 0: success, non-zero: failure
/// @exception stack overflow exception
IdsmErrorCode IdsmStruct::StructParseByPath(ara::core::String const& filtPath)
{
    int32_t const ret{IdsmManifest::Parse(filtPath)};
    if (ret != 0) {
        return IdsmErrorCode::kIdsmConfigParseErr;
    }

    _configPreProcess();
    IdsmErrorCode const portErr{_initPort()};
    if (portErr != IdsmErrorCode::kIdsmSuccess) {
        return portErr;
    }

    IdsmErrorCode const internalEventErr{_internalEventInit()};
    if (internalEventErr != IdsmErrorCode::kIdsmSuccess) {
        return internalEventErr;
    }

    // did
    IdsmErrorCode const didServerErr{_initDidServer()};
    if (didServerErr != IdsmErrorCode::kIdsmSuccess) {
        return didServerErr;
    }
    // dtc
    IdsmErrorCode const dtcServerErr{_initDtcServer()};
    if (dtcServerErr != IdsmErrorCode::kIdsmSuccess) {
        return dtcServerErr;
    }
    /// @brief eventId map to event property
    ara::core::Vector< SecurityEventContextProp > const securityEventCtxProp{GetSecurityEventContextProps()};
    for (auto const& it : securityEventCtxProp) {
        ara::core::Result< uint16_t, IdsmErrorCode > eventIdRes{_getEventIdByFqn(it.securityEvent)};
        if (eventIdRes.HasValue() == false) {
            continue;
        }
        uint16_t eventId{eventIdRes.Value()};
        EventProperPtr const proper{
            std::make_shared< EventProper >(eventId, it.persistentStorage, it.sensorInstanceId)};
        EventProperPool::GetInstance()->SetEventProper(eventId, proper);
        std::ignore = EventProperPool::GetInstance()->SetEventMode(eventId, it.defaultReportingMode);
    }
    IdsmErrorCode const filterChainErr{_eventMapFilterChain()};
    if (filterChainErr != IdsmErrorCode::kIdsmSuccess) {
        return filterChainErr;
    }
    return IdsmErrorCode::kIdsmSuccess;
}
/// @brief Configuration parsing deinitialization
/// @return 0: success, non-zero: failure
/// @exception stack overflow exception
IdsmErrorCode IdsmStruct::StructDeInit() noexcept
{
    for (auto& ele : filterMap_) {
        ele.second.reset();
    }
    filterMap_.clear();
#ifdef ARA_WITH_DIAG
    if (diagDidServer_ != nullptr) {
        diagDidServer_->StopOffer();
        diagDidServer_.reset();
    }

    for (auto& ele : diagDtcServerMap_) {
        ele.second->StopOffer();
        ele.second.reset();
    }
    diagDtcServerMap_.clear();
#endif

    internalSev_.trafficEvent.reset();
    internalSev_.communicationEvent.reset();
    internalSev_.eventbufEvent.reset();
    internalSev_.contextBufEvent.reset();

    return IdsmErrorCode::kIdsmSuccess;
}
/// @brief Get mapping between event id and filter chain
/// @param filtersMap mapping between event id and filter chain
/// @exception stack overflow exception
ara::core::Map< uint16_t, FilterChainPtr > IdsmStruct::GetFilterMap() const { return filterMap_; }
/// @brief Get mapping between Port instance descriptor and security event id
/// @param portMap mapping between Port instance descriptor and security event id
/// @exception stack overflow exception
ara::core::Map< ara::core::String, uint16_t > IdsmStruct::GetPortMap() { return portMap_; }
/// @brief Get mapping between process and security event set
/// @param processMap mapping between process and security event set
/// @exception stack overflow exception
ara::core::Map< ara::core::String, EventVec > IdsmStruct::GetProcessMap() { return processMap_; }
/// @brief Get internal security events of idsm
/// @param internalEvent internal security events of idsm
/// @exception stack overflow exception
IdsmInternalEvent IdsmStruct::GetInternalEvent() const { return internalSev_; }
#ifdef ARA_WITH_DIAG
/// @brief Get mapping between security event id and diagnosis event related data structure
/// @param dtcServerMap mapping between security event id and diagnosis event related data structure
/// @exception stack overflow exception
ara::core::Map< uint16_t, DiagDtcServerPtr > IdsmStruct::GetDiagDtcServer() const { return diagDtcServerMap_; }
#endif
/// @brief Get rate limiting related data
/// @param intervals rate limiting period
/// @param maxEvents maximum number of security events transmitted within the rate limiting period
/// @exception stack overflow exception
void IdsmStruct::GetRateLimit(time_t& intervals, uint32_t& maxEvents)
{
    std::ignore = intervals;
    std::ignore = maxEvents;
    intervals   = static_cast< time_t >(GetIdsmInstance().rateLimitationFilter.timeInterval);
    maxEvents   = GetIdsmInstance().rateLimitationFilter.maxEventsInInterval;
}
/// @brief Get flow control related data
/// @param intervals flow control period
/// @param maxBytes maximum number of bytes transmitted within the flow control period
/// @exception stack overflow exception
void IdsmStruct::GetTrafficLimit(time_t& intervals, uint64_t& maxBytes)
{
    std::ignore = intervals;
    std::ignore = maxBytes;
    intervals   = static_cast< time_t >(GetIdsmInstance().trafficLimitationFilter.timeInterval);
    maxBytes    = GetIdsmInstance().trafficLimitationFilter.maxBytesInInterva;
}
/// @brief Get IP address of remote idsr
/// @param ipAddr IP address of idsr
/// @exception stack overflow exception
ara::core::String IdsmStruct::GetIdsrIpAddr() { return GetIdsmInstance().moduleInstall.net.ipv4IpAddress; }
/// @brief Data format conversion. Convert string to enumeration value in the specification
/// @exception stack overflow exception
void IdsmStruct::_configPreProcess()
{
    // FilterChain
    std::ignore = configMap_.insert(std::pair< ara::core::String, ContextDataSource >(
        "USE-FIRST-CONTEXT-DATA", ContextDataSource::kUseFirstContextData));
    std::ignore = configMap_.insert(std::pair< ara::core::String, ContextDataSource >(
        "USE-LAST-CONTEXT-DATA", ContextDataSource::kUseLastContextData));
}
/// @brief Get Idsm internal events
/// @return initialization of Idsm internal events
/// @exception stack overflow exception
IdsmErrorCode IdsmStruct::_internalEventInit()
{
    /// @brief speed overrun security event

    if (internalSev_.trafficEvent.get() == nullptr) {
        internalSev_.trafficEvent = std::make_shared< Event >();
    }
    ara::core::String const trafficEventFqn{"IDSM_INTERNAL_EVENT_TRAFFIC_LIMITATION_EXCEEDED"};
    ara::core::Result< uint16_t, IdsmErrorCode > trafficEventIdRes{_getEventIdByFqn(trafficEventFqn, true)};
    if (trafficEventIdRes.HasValue() == false) {
        IdsmErrorCode const trafficErr{trafficEventIdRes.Error()};
        LOG_ERROR << "Idsm internal traffic event fail. err: " << static_cast< uint16_t >(trafficErr);
        return IdsmErrorCode::kIdsmTrafficLimitErr;
    }
    uint16_t trafficEventId{trafficEventIdRes.Value()};
    if (trafficEventId != 0U) {
        internalSev_.trafficEvent->SetEventId(trafficEventId);
    }

    /// @brief traffic overrun security event
    if (internalSev_.communicationEvent.get() == nullptr) {
        internalSev_.communicationEvent = std::make_shared< Event >();
    }
    ara::core::String const communicationEventFqn{"IDSM_INTERNAL_EVENT_COMMUNICATION_ERROR"};
    ara::core::Result< uint16_t, IdsmErrorCode > comEventIdRes{_getEventIdByFqn(communicationEventFqn, true)};
    if (comEventIdRes.HasValue() == false) {
        IdsmErrorCode const communicationErr{comEventIdRes.Error()};
        LOG_ERROR << "Idsm internal communication event fail. err: " << static_cast< uint16_t >(communicationErr);
        return IdsmErrorCode::kIdsmCommunicationErr;
    }
    uint16_t communicationEventId{comEventIdRes.Value()};
    if (communicationEventId != 0U) {
        internalSev_.communicationEvent->SetEventId(communicationEventId);
    }

    /// @brief event buf not avaliable
    if (internalSev_.eventbufEvent.get() == nullptr) {
        internalSev_.eventbufEvent = std::make_shared< Event >();
    }
    ara::core::String const eventBufEventFqn{"IDSM_INTERNAL_EVENT_NO_EVENT_BUFFER_AVAILABLE"};
    ara::core::Result< uint16_t, IdsmErrorCode > frameEventIdRes{_getEventIdByFqn(eventBufEventFqn, true)};
    if (frameEventIdRes.HasValue() == false) {
        IdsmErrorCode const frameEventErr{frameEventIdRes.Error()};
        LOG_ERROR << "Idsm internal event buf not avaliable event fail. err: "
                  << static_cast< uint16_t >(frameEventErr);
        return frameEventErr;
    }
    uint16_t eventId{frameEventIdRes.Value()};
    if (eventId != 0U) {
        internalSev_.eventbufEvent->SetEventId(eventId);
    }

    /// @brief event context buf not avaliable
    if (internalSev_.contextBufEvent.get() == nullptr) {
        internalSev_.contextBufEvent = std::make_shared< Event >();
    }
    ara::core::String const contextBufEventFqn{"IDSM_INTERNAL_EVENT_NO_CONTEXT_DATA_BUFFER_AVAILABLE"};
    ara::core::Result< uint16_t, IdsmErrorCode > contextEventIdRes{_getEventIdByFqn(contextBufEventFqn, true)};
    if (contextEventIdRes.HasValue() == false) {
        IdsmErrorCode const contextBufErr{contextEventIdRes.Error()};
        LOG_ERROR << "Idsm internal event context buf not avaliable event fail. err: "
                  << static_cast< uint16_t >(contextBufErr);
        return contextBufErr;
    }
    uint16_t contextEventId{contextEventIdRes.Value()};
    if (contextEventId != 0U) {
        internalSev_.contextBufEvent->SetEventId(contextEventId);
    }
    return IdsmErrorCode::kIdsmSuccess;
}
/// @brief Get security event Id
/// @param fqn FQN of the security event
/// @param eventId Id of the security event
/// @param isInternalEvent whether fqn is an Idsm internal event
/// @return 0 success, non-zero failure
/// @code{.isoft}
/// @threadsafety={unsafe}
/// @endcode
ara::core::Result< uint16_t, IdsmErrorCode > IdsmStruct::_getEventIdByFqn(ara::core::String const& fqn,
                                                                          bool const isInternalEvent) const noexcept
{
    ara::core::Vector< SecurityEventDefinition > const securityEventDefinition{GetSecurityEventDefinitions()};
    ara::core::Vector< SecurityEventDefinition >::const_iterator it{securityEventDefinition.cbegin()};
    for (; it != securityEventDefinition.cend(); ++it) {
        if (isInternalEvent && (fqn == it->shortName)) {
            break;
        }
        if (fqn == it->fqn) {
            break;
        }
    }
    if (it == securityEventDefinition.cend()) {
        return ara::core::Result< uint16_t, IdsmErrorCode >::FromError(IdsmErrorCode::kManifestKeyNotFound);
    }
    uint16_t eventId{0U};
    uint16_t const tempEventId{it->id};
    uint16_t const invalidEventId{0xFFFFU};
    uint16_t const customEventIdMin{0x8000U};
    if (tempEventId == invalidEventId) {
        LOG_ERROR << "user config invalid event id. event FQN: " << it->fqn.c_str() << ". event id: " << it->id;
        return ara::core::Result< uint16_t, IdsmErrorCode >::FromValue(eventId);
    }
    if (isInternalEvent && (tempEventId >= customEventIdMin)) {
        LOG_ERROR << "user config invalid internal event id. event FQN: " << it->fqn.c_str()
                  << ". event id: " << it->id;
        return ara::core::Result< uint16_t, IdsmErrorCode >::FromValue(eventId);
    }
    if (!isInternalEvent && ((tempEventId < customEventIdMin) || (tempEventId >= invalidEventId))) {
        LOG_ERROR << "user config invalid custom event id. event FQN: " << it->fqn.c_str() << ". event id: " << it->id;
        return ara::core::Result< uint16_t, IdsmErrorCode >::FromValue(eventId);
    }

    eventId = tempEventId;
    return ara::core::Result< uint16_t, IdsmErrorCode >::FromValue(eventId);
}
/// @brief Get security event Id
/// @param fqn FQN of the security event property
/// @param eventId Id of the security event
/// @return 0 success, non-zero failure
/// @code{.isoft}
/// @threadsafety={unsafe}
/// @endcode
ara::core::Result< uint16_t, IdsmErrorCode > IdsmStruct::_getEventIdByContextFqn(
    ara::core::String const& fqn) const noexcept
{
    ara::core::Vector< SecurityEventContextProp > const securityEventCtxProp{GetSecurityEventContextProps()};
    ara::core::Vector< SecurityEventContextProp >::const_iterator it{securityEventCtxProp.cbegin()};
    for (; it != securityEventCtxProp.cend(); ++it) {
        if (it->fqn == fqn) {
            return _getEventIdByFqn(it->securityEvent);
        }
    }
    return ara::core::Result< uint16_t, IdsmErrorCode >::FromError(IdsmErrorCode::kManifestKeyNotFound);
}
/// @brief Get the filter chain for the specified fqn
/// @param fqn fqn of the chain
/// @param filters obtained filter chain
/// @return 0 success, non-zero failure
/// @exception stack overflow exception
ara::core::Result< FilterChainPtr, IdsmErrorCode > IdsmStruct::_getFilterChainByFqn(ara::core::String const& fqn)
{
    FilterChainPtr filters = std::make_shared< FilterChain >();
    ara::core::Vector< SecurityEventFilterChain > const filterChain{GetSecurityEventFilterChains()};
    ara::core::Vector< SecurityEventFilterChain >::const_iterator it{filterChain.cbegin()};
    for (; it != filterChain.cend(); ++it) {
        if (it->fqn == fqn) {
            break;
        }
    }
    if (it == filterChain.cend()) {
        return ara::core::Result< FilterChainPtr, IdsmErrorCode >::FromError(IdsmErrorCode::kManifestKeyNotFound);
    }

    if (it->hasState) {
        std::shared_ptr< StateFilter > state{std::make_shared< StateFilter >()};
        /// @details iterator traversing vector
        ara::core::Vector< FunctionGroupState >::const_iterator ele{it->state.blockIfStateActiveAp.cbegin()};
        for (; ele != it->state.blockIfStateActiveAp.cend(); ++ele) {
            state->SetFGState(ele->modeDeclarationGroup, ele->modeDeclaration);
        }
        filters->AddFilter(state);
    }
    if (it->hasOneEveryN) {
        std::shared_ptr< SampleFilter > const sample{std::make_shared< SampleFilter >(it->oneEveryN)};
        filters->AddFilter(sample);
    }
    if (it->hasAgg) {
        std::shared_ptr< AggregationFilter > const agg{std::make_shared< AggregationFilter >(
            it->agg.minimumIntervalLength, configMap_[it->agg.contextDataSource])};
        filters->SetAggreFilter(agg);
    }
    if (it->hasThres) {
        std::shared_ptr< ThresholdFilter > const threshold{
            std::make_shared< ThresholdFilter >(it->thres.intervalLength, it->thres.thresholdNumber)};
        filters->SetThresholdFilter(threshold);
    }

    return ara::core::Result< FilterChainPtr, IdsmErrorCode >::FromValue(filters);
}
/// @brief Establish mapping from prototype to event id
/// @return 0 success, non-zero failure
/// @exception stack overflow exception
/// @code{.isoft}
/// @threadsafety={unsafe}
/// @endcode
IdsmErrorCode IdsmStruct::_initPort()
{
    ara::core::Vector< SecurityEventMappingPort > const eventMappingPort{GetSecurityEventPorts()};
    for (auto const& it : eventMappingPort) {
        ara::core::String const port{it.instanceId};
        ara::core::Result< uint16_t, IdsmErrorCode > eventIdRes{_getEventIdByFqn(it.mappedEvent)};
        if (eventIdRes.HasValue() == false) {
            IdsmErrorCode const eventIdErr{eventIdRes.Error()};
            LOG_ERROR << "port map event id: not found event id by event fqn. event fqn " << it.mappedEvent.c_str()
                      << " err " << static_cast< uint16_t >(eventIdErr);
            return eventIdErr;
        }
        uint16_t eventId{eventIdRes.Value()};
        std::ignore = portMap_.insert(std::pair< ara::core::String, uint32_t >(port, eventId));
        if (eventId == 0U) {
            /// TODO
        }
    }

    ara::core::Vector< ProcessMappingPort > const processMappingPort{GetSecurityEventProcess()};
    for (auto const& it : processMappingPort) {
        ara::core::String process{it.mappedProecess};
        if (processMap_.count(process) == 0U) {
            EventVec vec;
            std::ignore = processMap_.insert(std::pair< ara::core::String, EventVec >(process, vec));
        }
        ara::core::String const port{it.instanceId};
        ara::core::Map< ara::core::String, EventVec >::iterator const mapIt{processMap_.find(process)};
        if (portMap_.count(port) != 1U) {
            LOG_ERROR << "process map event id: not found event id by event port. event port " << it.instanceId.c_str();
            continue;
        }
        mapIt->second.push_back(portMap_[port]);
    }
    return IdsmErrorCode::kIdsmSuccess;
}
/// @brief Initialize Did Server, provide external services
/// @return 0 success, non-zero failure
/// @exception stack overflow exception
IdsmErrorCode IdsmStruct::_initDidServer()
{
#ifdef ARA_WITH_DIAG
    ara::core::Vector< DidToSecurityEventContextProp > const didToEventCtxProp{GetDidToSecurityEventContextProps()};
    if (!didToEventCtxProp.empty()) {
        ara::core::InstanceSpecifier didInstance{
            ara::core::StringView{"idsmd/rootSwComponentPrototype_idsmd/pPort_did_common"}};
        ara::diag::DataIdentifierReentrancyType didReentrancy{};
        didReentrancy.read      = ara::diag::ReentrancyType::kFully;
        didReentrancy.readWrite = ara::diag::ReentrancyType::kFully;
        didReentrancy.write     = ara::diag::ReentrancyType::kFully;
        diagDidServer_          = std::make_shared< DiagDidServer >(didInstance, didReentrancy);
        if (didReentrancy.read == ara::diag::ReentrancyType::kFully) {
        }
        ara::core::Map< uint16_t, uint16_t > m;

        ara::core::Vector< DidToSecurityEventContextProp >::const_iterator it{didToEventCtxProp.cbegin()};
        for (; it != didToEventCtxProp.cend(); ++it) {
            ara::core::String fqn{it->securityEventContextProps};
            ara::core::Result< uint16_t, IdsmErrorCode > eventIdRes{_getEventIdByContextFqn(fqn)};
            if (eventIdRes.HasValue() == false) {
                IdsmErrorCode const eventIdErr{eventIdRes.Error()};
                LOG_ERROR << "idsm diag did server: not found event id by event context props fqn. context props fqn "
                          << it->securityEventContextProps.c_str() << " err " << static_cast< uint16_t >(eventIdErr);
                return eventIdErr;
            }
            uint16_t eventId{eventIdRes.Value()};
            uint16_t didId{it->did};
            std::ignore = m.insert(std::pair< uint16_t, uint16_t >(didId, eventId));
        }
        std::ignore = diagDidServer_->Init(m);
        ara::core::Result< void > const result{diagDidServer_->Offer()};
        if (!result.HasValue()) {
            LOG_ERROR << "idsm diag did server: Offer fail. did instance " << didInstance.ToString().data() << " err "
                      << result.Error().Value();
            return IdsmErrorCode::kIdsmDidServerOfferErr;
        }
        LOG_INFO << "idsm diag did server: Offer sucess. did intance:" << didInstance.ToString().data();
    }
#endif
    return IdsmErrorCode::kIdsmSuccess;
}
/// @brief Initialize Dtc Server, provide external services
/// @return 0 success, non-zero failure
/// @exception stack overflow exception
IdsmErrorCode IdsmStruct::_initDtcServer()
{
#ifdef ARA_WITH_DIAG
    ara::diag::DataIdentifierReentrancyType dtcReentrancy{};
    dtcReentrancy.read      = ara::diag::ReentrancyType::kFully;
    dtcReentrancy.readWrite = ara::diag::ReentrancyType::kNot;
    dtcReentrancy.write     = ara::diag::ReentrancyType::kNot;

    ara::core::Vector< DtcToSecurityEventContextProp > const dtcToEventCtxProp{GetDtcToSecurityEventContextProps()};
    ara::core::Vector< DtcToSecurityEventContextProp >::const_iterator it{dtcToEventCtxProp.cbegin()};
    for (; it != dtcToEventCtxProp.cend(); ++it) {
        ara::core::String fqn{it->securityEventContextProps};
        ara::core::Result< uint16_t, IdsmErrorCode > eventIdRes{_getEventIdByContextFqn(fqn)};
        if (eventIdRes.HasValue() == false) {
            IdsmErrorCode const eventIdErr{eventIdRes.Error()};
            LOG_ERROR << "idsm diag dtc server: not found event id by event context props fqn. context props fqn "
                      << it->securityEventContextProps.c_str() << " err " << static_cast< uint16_t >(eventIdErr);
            return eventIdErr;
        }
        uint16_t eventId{eventIdRes.Value()};
        ara::core::InstanceSpecifier monitor{ara::core::StringView{it->monitorInstance.c_str()}};
        ara::core::InstanceSpecifier const dtcInstance{ara::core::StringView{it->didInstance.c_str()}};
        std::shared_ptr< DiagDtcServer > server{std::make_shared< DiagDtcServer >(dtcInstance, dtcReentrancy, monitor)};
        if (dtcReentrancy.read == ara::diag::ReentrancyType::kFully) {
        }
        server->Init(eventId);
        std::ignore = diagDtcServerMap_.emplace(eventId, server);
        if (eventId == 0U) {
        }
        ara::core::Result< void > const r{server->Offer()};
        if (!r.HasValue()) {
            LOG_ERROR << "idsm diag dtc server: Offer fail. did instance" << dtcInstance.ToString().data()
                      << " err: " << r.Error();
            return IdsmErrorCode::kIdsmDtcServerOfferErr;
        }
        LOG_INFO << "idsm diag dtc server: Offer sucess. did instance:" << dtcInstance.ToString().data();
    }
#endif
    return IdsmErrorCode::kIdsmSuccess;
}
/// @brief Establish mapping from security event to filter chain
/// @return 0 success, non-zero failure
/// @exception stack overflow exception
IdsmErrorCode IdsmStruct::_eventMapFilterChain()
{
    /// @brief eventId map to event filter chain
    ara::core::Vector< SecurityEventContextMappingApplication > const eventCtxMappingApp{
        GetSecurityEventContextMappingApplications()};
    ara::core::Vector< SecurityEventContextMappingApplication >::const_iterator it{eventCtxMappingApp.cbegin()};
    for (; it != eventCtxMappingApp.cend(); ++it) {
        // Iterate over events in app mapping to get the filter chain corresponding to the event Id
        ara::core::Result< FilterChainPtr, IdsmErrorCode > filterChainRes{_getFilterChainByFqn(it->filterChain)};
        if (filterChainRes.HasValue() == false) {
            /// @brief Failed to get filter chain based on fqn
            IdsmErrorCode const filterChainErr{filterChainRes.Error()};
            LOG_ERROR << "idsm filter: found filter chain by fqn fail. err: "
                      << static_cast< uint16_t >(filterChainErr);
            return filterChainErr;
        }
        FilterChainPtr filterChainEntity{filterChainRes.Value()};
        ara::core::Vector< ara::core::String >::const_iterator event{it->mappedSecurityEvent.cbegin()};
        for (; event != it->mappedSecurityEvent.cend(); ++event) {
            ara::core::Result< uint16_t, IdsmErrorCode > eventIdRes{_getEventIdByContextFqn(*event)};
            if (eventIdRes.HasValue() == false) {
                IdsmErrorCode const eventIdErr{eventIdRes.Error()};
                LOG_ERROR << "idsm filter: found security event id by event context fqn fail. err: "
                          << static_cast< uint16_t >(eventIdErr);
                return eventIdErr;
            }
            uint16_t eventId{eventIdRes.Value()};
            if (filterMap_.count(eventId) != 0U) {
                // Multiple filter chains correspond to the same event type
                if (filterMap_[eventId].get() != nullptr) {
                    filterMap_[eventId].reset();
                }
                LOG_ERROR << "idsm filter: event id:" << eventId << " has multi filters chain.";
            } else {
                std::ignore = filterMap_.insert(std::pair< uint32_t, FilterChainPtr >(eventId, filterChainEntity));
            }
        }
    }
    return IdsmErrorCode::kIdsmSuccess;
}

}  // namespace idsm
}  // namespace ara
