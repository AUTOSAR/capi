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
/// @file       e2e.cpp
/// @brief
/// @details
/// @date       2021-12-31
/// @author     mazelin
/// @version    1.2.0
///
/// ================================================================

#include "ara/com/e2e/e2e.h"

namespace ara {
namespace com {
namespace e2exf {
/// @brief
std::shared_ptr< Transformer > Transformer::s_Instance_{};  // NOLINT

/// @brief Constructor
/// @param[in] state State machine check status
/// @param[in] status Profile check status
Result::Result(e2e::SMState state, e2e::ProfileCheckStatus status, CounterType counter) noexcept
    : state_{state}, status_{status}, counter_{counter}
{
}

/// @brief Constructor
Result::Result() noexcept : Result{e2e::SMState::kNoData, e2e::ProfileCheckStatus::kNotAvailable, 0} {}

/// @brief Get the state machine check result from Result
/// @return State machine check result
e2e::SMState Result::GetSMState() const noexcept { return state_; }

/// @brief Get the Profile check result from Result
/// @return Profile check result
e2e::ProfileCheckStatus Result::GetProfileCheckStatus() const noexcept { return status_; }

/// @brief Check if the result is valid
/// @return bool true valid false invalid
bool Result::IsOk() const noexcept
{
    return (state_ == e2e::SMState::kValid) && (status_ == e2e::ProfileCheckStatus::kOk);
}

/// @brief Load configuration file
/// @param[in] bindingFile e2e mapping configuration file path
/// @param[in] bindingFormat bindingFile configuration file format
/// @param[in] e2exfFile e2e profile configuration file path
/// @param[in] e2exfFormat e2exfFile configuration file format
/// @return true success false failure
bool StatusHandler::Configure(ara::core::String const& bindingFile,
                              Format const bindingFormat,
                              ara::core::String const& e2exfFile,
                              Format const e2exfFormat) noexcept
{
    return Impl::Instance().Configure(bindingFile, bindingFormat, e2exfFile, e2exfFormat);
}

/// @brief Get dataId
/// @param[in] serviceId Service ID
/// @param[in] instanceId Instance ID
/// @param[in] eventId Event/Method ID
/// @return data ID
IdlistType StatusHandler::GetDataId(uint16_t const serviceId,
                                    uint16_t const instanceId,
                                    uint16_t const eventId) noexcept
{
    return Impl::Instance().GetDataId(serviceId, instanceId, eventId);
}

uint32_t StatusHandler::GetSourceId(uint16_t const serviceId,
                                    uint16_t const instanceId,
                                    uint16_t const methodId) noexcept
{
    return Impl::Instance().GetSourceId(serviceId, instanceId, methodId);
}

End2EndEventProtectionProps* StatusHandler::GetEPropByDataId(IdlistType const& ids) noexcept
{
    return Impl::Instance().GetEPropByDataId(ids);
}

End2EndMethodProtectionProps* StatusHandler::GetMPropByDataId(IdlistType const& ids) noexcept
{
    return Impl::Instance().GetMPropByDataId(ids);
}

uint32_t StatusHandler::GetHeaderLength(IdlistType const& ids) noexcept
{
    return Impl::Instance().GetHeaderLength(ids);
}

/// @brief Check if the service is configured with e2e protection
/// @param[in] serviceid Service ID
/// @return bool true configured, false not configured
bool StatusHandler::HasE2E(uint16_t const serviceid) noexcept { return Impl::Instance().HasE2E(serviceid); }

/// @brief Load e2e mapping configuration file
/// @param[in] bindingFile e2e mapping configuration file path
/// @param[in] bindingFormat e2e mapping configuration file path format kJson, kXml
/// @return Configuration loading result
bool StatusHandler::Impl::GetDataIdMapping(ara::core::String const& bindingFile, Format const bindingFormat) noexcept
{
    // decltype(ids_) retList;
    if (bindingFormat == Format::kJson) {
        ara::core::Result< std::unique_ptr< isoft::manifestreader::Manifest > > manifestRes{
            isoft::manifestreader::OpenManifest(bindingFile)};
        if (!manifestRes) {
            ComLogError("Open failed", GenArg(bindingFile));
            // return decltype(ids_){};
            return false;
        }
        ara::core::Vector< End2EndEventDataIdMapping > eventMapping;
        auto ret{manifestRes.Value()->Load("End2EndEventDataIdMapping", eventMapping)};
        if (isoft::kSuccess != ret) {
            ComLogDebug("Load End2EndEventDataIdMapping error:", ret);
            // return decltype(ids_){};
            return false;
        }
        for (auto const& m : eventMapping) {
            ComLogDebug("", GenArg(m.dataId), GenK2V("serviceId", m.serviceId), GenK2V("instanceId", m.instanceId),
                        GenK2V("eventId", m.eventId));
            std::ignore = ids_.insert(std::make_pair(
                std::tuple< uint16_t, uint16_t, uint16_t >(m.serviceId, m.instanceId, m.eventId), m.dataId));
        }
        ara::core::Vector< End2EndMethodDataIdMapping > methdoMapping;
        ret = manifestRes.Value()->Load("End2EndMethodDataIdMapping", methdoMapping);
        if (isoft::kSuccess != ret) {
            ComLogDebug("Load End2EndMethodDataIdMapping error:", ret);
            // return decltype(ids_){};
            return false;
        }
        for (auto const& m : methdoMapping) {
            ComLogDebug("", GenArg(m.dataId), GenK2V("serviceId", m.serviceId), GenK2V("instanceId", m.instanceId),
                        GenK2V("methodId", m.methodId));
            std::ignore = ids_.insert(std::make_pair(
                std::tuple< uint16_t, uint16_t, uint16_t >(m.serviceId, m.instanceId, m.methodId), m.dataId));

            /// Special scenario 1: For serviceId, instanceId, methodId, configure different protection mechanisms for different proxy sides:
            /// Skeleton side -> serviceId, instanceId, methodId, sourceId correspond to one dataId
            /// Proxy side -> serviceId, instanceId, methodId, sourceId correspond to one dataId

            /// Special scenario 2: For serviceId, instanceId, methodId, configure the same protection mechanism for different proxy sides:
            /// Skeleton side -> serviceId, instanceId, methodId, sourceId correspond to one dataId
            /// Proxy side -> serviceId, instanceId, methodId, sourceId correspond to one dataId

            /// TODO() This logic needs optimization, otherwise the logic is quite confusing and difficult to understand (skeleton side and proxy side share a set of configuration logic)
            /// This is only used for the proxy side. The skeleton side can obtain sourceId through the e2e header
            std::ignore = sids_.insert(std::make_pair(
                std::tuple< uint16_t, uint16_t, uint16_t >(m.serviceId, m.instanceId, m.methodId), m.sourceId));
        }

    } else if (bindingFormat == Format::kXml) {
        ComLogError("xml format is not supported", GenArg(bindingFile));
        return false;
    } else {
        ComLogError("unknown format is not supported", GenArg(bindingFile));
        return false;
    }
    return true;
}

bool StatusHandler::Impl::GetE2EXfConfiguration(ara::core::String const& file, Format const format) noexcept
{
    if (format == Format::kJson) {
        auto manifestRes{isoft::manifestreader::OpenManifest(file)};
        if (!manifestRes) {
            ComLogError("Open failed", GenArg(file));
            return false;
        }
        ara::core::Vector< End2EndEventProtectionProps > eventProps;

        auto ret{manifestRes.Value()->Load("End2EndEventProtectionProps", eventProps)};
        if (isoft::kSuccess != ret) {
            ComLogDebug("Load End2EndEventProtectionProps error:", ret);
            return false;
        }
        ComLogDebug("", GenArg(ret));
        for (auto p : eventProps) {
            ComLogDebug("", GenArg(p.dataId), GenArg(p.dataIdList), p.dataIdMode, p.profileName);
            IdlistType ids;
            if (p.dataId > 0) {
                ids.push_back(p.dataId);
            } else {
                for (auto n : p.dataIdList) {
                    ids.push_back(n);
                }
            }
            std::ignore = eventProps_.insert(std::make_pair(ids, std::make_unique< End2EndEventProtectionProps >(p)));
            std::ignore = dataIdHeaderLengths_.insert(std::make_pair(ids, GetProfileHeaderLength(p.profileName)));
        }

        ara::core::Vector< End2EndMethodProtectionProps > methodProps;
        ret = manifestRes.Value()->Load("End2EndMethodProtectionProps", methodProps);
        if (isoft::kSuccess != ret) {
            ComLogDebug("Load End2EndMethodProtectionProps error:", ret);
            return false;
        }
        ComLogDebug("", GenArg(ret));

        for (auto p : methodProps) {
            ComLogDebug("", GenArg(p.dataId), GenArg(p.dataIdList), p.dataIdMode, p.profileName);
            IdlistType ids;
            if (p.dataId > 0) {
                ids.push_back(p.dataId);
            } else {
                for (auto _ : p.dataIdList) {
                    ids.push_back(_);
                }
            }
            std::ignore = methodProps_.insert(std::make_pair(ids, std::make_unique< End2EndMethodProtectionProps >(p)));
            std::ignore = dataIdHeaderLengths_.insert(std::make_pair(ids, GetProfileHeaderLength(p.profileName)));
        }
    }
    return true;
}

uint32_t StatusHandler::Impl::GetProfileHeaderLength(profile::ProfileName const profileName) noexcept
{
    uint32_t length{};
    switch (profileName) {
        case profile::ProfileName::kProfile_01: {
            length = ara::com::profile_01::Profile01::kHeaderLength;
        } break;
        case profile::ProfileName::kProfile_02: {
            length = ara::com::profile_02::Profile02::kHeaderLength;
        } break;
        case profile::ProfileName::kProfile_04: {
            length = ara::com::profile_04::Profile04::kHeaderLength;
        } break;
        case profile::ProfileName::kProfile_04m: {
            length = ara::com::profile_04m::Profile04m::kHeaderLength;
        } break;
        case profile::ProfileName::kProfile_05: {
            length = ara::com::profile_05::Profile05::kHeaderLength;
        } break;
        case profile::ProfileName::kProfile_06: {
            length = ara::com::profile_06::Profile06::kHeaderLength;
        } break;
        case profile::ProfileName::kProfile_07: {
            length = ara::com::profile_07::Profile07::kHeaderLength;
        } break;
        case profile::ProfileName::kProfile_07m: {
            length = ara::com::profile_07m::Profile07m::kHeaderLength;
        } break;
        case profile::ProfileName::kProfile_08: {
            length = ara::com::profile_08::Profile08::kHeaderLength;
        } break;
        case profile::ProfileName::kProfile_11: {
            length = ara::com::profile_11::Profile11::kHeaderLength;
        } break;
        case profile::ProfileName::kProfile_22: {
            length = ara::com::profile_22::Profile22::kHeaderLength;
        } break;
        case profile::ProfileName::kProfile_44: {
            length = ara::com::profile_44::Profile44::kHeaderLength;
        } break;
        default: {
            ComLogError("please check logical, cant' execute this step!", GenArg0(profileName));
        } break;
    }
    return length;
}

/// @brief Get Impl instance
/// @return Impl instance
StatusHandler::Impl& StatusHandler::Impl::Instance() noexcept
{
    static Impl s_Instance;
    return s_Instance;
}

/// @brief Load configuration file
/// @param[in] bindingFile e2e mapping configuration file path
/// @param[in] bindingFormat bindingFile configuration file format
/// @param[in] e2exfFile e2e profile configuration file path
/// @param[in] e2exfFormat e2exfFile configuration file format
/// @return true success false failure
bool StatusHandler::Impl::Configure(ara::core::String const& bindingFile,
                                    Format const bindingFormat,
                                    ara::core::String const& e2exfFile,
                                    Format const e2exfFormat) noexcept
{
    std::ignore = bindingFormat;  // for fix warning
    std::ignore = e2exfFile;      // for fix warning
    std::ignore = e2exfFormat;    // for fix warning
    if (configured_) {
        ComLogWarning("Status handler was already configured, skipping");
        return true;
    }
    try {
        ComLogDebug("Loading e2e mapping from: ", bindingFile);
        if (!GetDataIdMapping(bindingFile, bindingFormat)) {
            return false;
        }
        for (auto const& it : ids_) {
            uint16_t serviceid{};
            uint16_t instanceId{};
            uint16_t mid{};

            std::tie(serviceid, instanceId, mid) = it.first;
            services_.insert(serviceid);
        }

        if (!GetE2EXfConfiguration(e2exfFile, e2exfFormat)) {
            return false;
        }

    } catch (std::exception& e) {
        ComLogError("Failed to configure status handler due to: ", e.what());
        return false;
    }
    configured_ = true;
    return true;
}

/// @brief Get dataId
/// @param[in] serviceId Service ID
/// @param[in] instanceId Instance ID
/// @param[in] eventId Event/Method ID
/// @return data ID
IdlistType StatusHandler::Impl::GetDataId(uint16_t const serviceId,
                                          uint16_t const instanceId,
                                          uint16_t const eventId) noexcept
{
    auto const& itId{ids_.find({serviceId, instanceId, eventId})};
    if (itId == ids_.end()) {
        return IdlistType{};
    }
    return itId->second;
}

uint32_t StatusHandler::Impl::GetSourceId(uint16_t const serviceId,
                                          uint16_t const instanceId,
                                          uint16_t const eventId) noexcept
{
    auto const& itId{sids_.find({serviceId, instanceId, eventId})};
    if (itId == sids_.end()) {
        return 0;
    }
    return itId->second;
}

End2EndEventProtectionProps* StatusHandler::Impl::GetEPropByDataId(IdlistType const& ids) noexcept
{
    auto const& iter{eventProps_.find(ids)};
    if (iter == eventProps_.end()) {
        return nullptr;
    }
    return iter->second.get();
}

End2EndMethodProtectionProps* StatusHandler::Impl::GetMPropByDataId(IdlistType const& ids) noexcept
{
    auto const& iter{methodProps_.find(ids)};
    if (iter == methodProps_.end()) {
        return nullptr;
    }
    return iter->second.get();
}

uint32_t StatusHandler::Impl::GetHeaderLength(IdlistType const& ids) noexcept
{
    auto const& iter{dataIdHeaderLengths_.find(ids)};
    if (iter == dataIdHeaderLengths_.end()) {
        ComLogError("The headerlenght of dataId does not exist");
        return 0;
    }
    return iter->second;
}

/// @brief Configuration loading
/// @param[in] file Configuration file path
/// @param[in] format Configuration file format json xml
/// @return true loading success false loading failure
bool Transformer::Configure(ara::core::String const& file, Format const format) noexcept
{
    s_Instance_ = std::make_shared< Transformer >(file, format);
    if (nullptr == s_Instance_.get()) {
        return false;
    }
    return (s_Instance_->config_.ret == isoft::kSuccess);
}

/// @brief e2e check wrapper for events
/// @param[in] dataId dataID
/// @param[in] buffer Data to be checked
/// @return Check result
E2EResult Transformer::E2E_check(IdlistType const& dataId, BufferType& buffer) noexcept
{
    return s_Instance_->Check(dataId, buffer);
}

/// @brief e2e check wrapper for events
/// @param[in] dataId dataID
/// @param[in] buffer Data to be checked
/// @return Check result
E2EResult Transformer::E2E_check(IdlistType const& dataId, BufferType* buffer) noexcept
{
    if (buffer == nullptr) {
        return s_Instance_->Check(dataId);
    }
    return s_Instance_->Check(dataId, *buffer);
}

/// @brief e2e protection wrapper for events
/// @param[in] dataId dataID
/// @param[in] buffer Data to be protected
/// @return Result object -- empty/value or error
ara::core::Result< void > Transformer::E2E_protect(IdlistType const& dataId, BufferType& buffer) noexcept
{
    return s_Instance_->Protect(dataId, buffer);
}

/// @brief e2e check wrapper for methods (proxy side)
/// @param[in] dataId dataID
/// @param[in] messageType Message type kMessageTypeResponse
/// @param[in] messageResult Result type in response message. kMessageResultOk or kMessageResultError.
/// @param[in] sourceID Source ID
/// @param[in] buffer Data to be checked
/// @return Result object -- check result or error
E2EResult Transformer::E2E_check(IdlistType const& dataId,
                                 MessageType const& messageType,
                                 ResultType const& messageResult,
                                 SourceidType const& sourceID,
                                 BufferType& buffer) noexcept
{
    return s_Instance_->Check(dataId, buffer, messageType, messageResult, sourceID);
}

/// @brief e2e check wrapper for methods (skeleton side)
/// @param[in] dataId dataID
/// @param[in] messageType Message type kMessageTypeRequest
/// @param[in] messageResult Result type in response message
/// @param[in] sourceID Source ID
/// @param[in] buffer Data to be checked
/// @return Result object -- check result or error
E2EResult Transformer::E2E_check(IdlistType const& dataId,
                                 uint16_t const client,
                                 uint16_t const methodId,
                                 MessageType const& messageType,
                                 ResultType const& messageResult,
                                 SourceidType* sourceID,
                                 BufferType& buffer) noexcept
{
    return s_Instance_->Check(dataId, client, methodId, buffer, messageType, messageResult, sourceID);
}

/// @brief e2e protection wrapper for methods
/// @param[in] dataId dataID
/// @param[in] messageType Message type kMessageTypeRequest or kMessageTypeResponse
/// @param[in] messageResult Result type in response message. kMessageResultOk or kMessageResultError.
/// @param[in] sourceID Source ID
/// @param[in] buffer Data to be protected
/// @return Result object -- empty/value or error
ara::core::Result< void > Transformer::E2E_protect(IdlistType const& dataId,
                                                   MessageType const& messageType,
                                                   ResultType const& messageResult,
                                                   SourceidType const& sourceID,
                                                   BufferType& buffer) noexcept
{
    return s_Instance_->Protect(dataId, buffer, messageType, messageResult, sourceID);
}

/// @brief Get e2e protection header length
/// @param[in] dataId DataID
/// @return e2e protection header length
uint32_t Transformer::E2E_GetHeaderLength(IdlistType const& dataId) noexcept
{
    return s_Instance_->GetHeaderLength(dataId);
}

/// @brief Check if dataid is configured with e2e protection
/// @param[in] dataId dataid
/// @return bool true configured with protection false not configured
bool Transformer::E2E_IsProtected(IdlistType const& dataId) noexcept { return s_Instance_->IsProtected(dataId); }

/// @brief Insert clientId to sourceId mapping into cache
/// @param[in] clientId Client ID
/// @param[in] sourceId Source ID
void Transformer::E2E_InsertClientSourceIdMap(uint16_t const clientId,
                                              uint16_t const methodId,
                                              SourceidType const& sourceId) noexcept
{
    s_Instance_->InsertClientSourceIdMap(clientId, methodId, sourceId);
}

/// @brief Get sourceid corresponding to clientId
/// @param[in] clientId Client ID
/// @return Result object -- SourceidType/value or error
ara::core::Result< SourceidType > Transformer::E2E_ClientIdToSourceId(uint16_t const clientId,
                                                                      uint16_t const methodId) noexcept
{
    return s_Instance_->ClientIdToSourceId(clientId, methodId);
}

/// @brief
/// @param dataId
/// @return
ara::core::Result< uint32_t > Transformer::E2E_GetProtecterCounter(IdlistType const& dataId,
                                                                   SourceidType const& sourceID) noexcept
{
    return s_Instance_->GetProtecterCounter(dataId, sourceID);
}

/// @brief Configuration loading
/// @param[in] file Configuration file path
/// @param[in] format Configuration file format json xml
/// @return Configuration loading result
Config Transformer::GetE2EXfConfiguration(ara::core::String const& file, Format const format) noexcept
{
    std::ignore = file;
    std::ignore = format;
    Config retConfig;
    retConfig.ret = isoft::kSuccess;
    return retConfig;
}

/// @brief Constructor
/// @param[in] file Configuration file path
/// @param[in] format Configuration file format
Transformer::Transformer(ara::core::String const& file, Format const format) noexcept
    : config_{[&]() {
        std::ignore = file;    // for fix warning
        std::ignore = format;  // for fix warning
        return LoadConfiguration< decltype(config_) >(file, format, GetE2EXfConfiguration);
    }()}
{
}

/// @brief Constructor
/// @param[in] config Configuration structure
Transformer::Transformer(Config&& config) noexcept : config_{std::move(config)} {}

/// @brief Constructor
Transformer::Transformer() noexcept : config_{} { ComLogDebug("E2E Transformer not used"); }

/// @brief Check if dataid is configured with e2e protection
/// @param[in] id dataid
/// @return bool true configured with protection false not configured
bool Transformer::IsProtected(IdType const id) const noexcept
{
    ara::core::Vector< e2exf::IdType > ids{id};

    return (nullptr != StatusHandler::GetEPropByDataId(ids)) || (nullptr != StatusHandler::GetMPropByDataId(ids));
    // return config_.profileCheckers.find(ids) != config_.profileCheckers.end() ||
    //        config_.stateMachines.find(ids) != config_.stateMachines.end();
}
/// @brief Check if dataid is configured with e2e protection
/// @param[in] id dataid
/// @return bool true configured with protection false not configured
bool Transformer::IsProtected(IdlistType const& ids) const noexcept
{
    return (nullptr != StatusHandler::GetEPropByDataId(ids)) || (nullptr != StatusHandler::GetMPropByDataId(ids));
    // return config_.profileCheckers.find(id) != config_.profileCheckers.end() ||
    //        config_.stateMachines.find(id) != config_.stateMachines.end();
}
/// @brief e2e protection wrapper for events
/// @param[in] dataID dataID
/// @param[in] buffer Data to be protected
/// @return Result object -- empty/value or error
ara::core::Result< void > Transformer::Protect(IdlistType const& dataID, BufferType& buffer) noexcept
{
    std::shared_ptr< profile::ProtectorInterface > protector{};
    auto const& itProtector{config_.eventProtectors.find(dataID)};
    if (itProtector == config_.eventProtectors.end()) {
        End2EndEventProtectionProps* const p{StatusHandler::GetEPropByDataId(dataID)};
        if (nullptr == p) {
            return ara::core::Result< void >::FromError(e2e::E2EErrc::kError);
        }
        protector   = CreateProtector(*p, kApSomeipHeaderoffset);
        std::ignore = config_.eventProtectors.insert(std::make_pair(dataID, protector));
    } else {
        protector = itProtector->second;
    }
    if (buffer.size() < (protector->GetHeaderLength() + protector->GetHeaderOffset())) {
        ComLogError(GenK2V("Length", buffer.size()), GenK2V("HeaderLength", protector->GetHeaderLength()),
                    GenK2V("HeaderOffset", protector->GetHeaderOffset()));
        return ara::core::Result< void >::FromError(e2e::E2EErrc::kError);
    }
    return protector->Protect(buffer);
}
/// @brief e2e protection wrapper for methods
/// @param[in] id dataID
/// @param[in] buffer Data to be protected
/// @param[in] messageType Message type kMessageTypeRequest or kMessageTypeResponse
/// @param[in] messageResult Result type in response message. kMessageResultOk or kMessageResultError.
/// @param[in] sourceId Source ID
/// @return Result object -- empty/value or error
ara::core::Result< void > Transformer::Protect(IdlistType const& id,
                                               BufferType& buffer,
                                               MessageType const& messageType,
                                               ResultType const& messageResult,
                                               SourceidType const& sourceId) noexcept
{
    std::shared_ptr< profile::ProtectorInterface > protector{};
    auto const& protectorIt{config_.methodProtectors.find({id, sourceId})};
    if (protectorIt == config_.methodProtectors.end()) {
        End2EndMethodProtectionProps* const p{StatusHandler::GetMPropByDataId(id)};
        if (nullptr == p) {
            return ara::core::Result< void >::FromError(e2e::E2EErrc::kError);
        }
        protector   = CreateProtector(*p, kApSomeipHeaderoffset);
        std::ignore = config_.methodProtectors.insert(
            std::make_pair(std::tuple< IdlistType, SourceidType >(id, sourceId), protector));
    } else {
        protector = protectorIt->second;
    }
    if (buffer.size() < (protector->GetHeaderLength() + protector->GetHeaderOffset())) {
        ComLogError(GenK2V("Length", buffer.size()), GenK2V("HeaderLength", protector->GetHeaderLength()),
                    GenK2V("HeaderOffset", protector->GetHeaderOffset()));
        return ara::core::Result< void >::FromError(e2e::E2EErrc::kError);
    }
    return protector->Protect(buffer, messageType, messageResult, sourceId);
}
/// @brief e2e protection wrapper for events
/// @param[in] id dataID list
/// @param[in] buffer Data to be protected
/// @return Result object -- empty/value or error
E2EResult Transformer::Check(IdlistType const& id, BufferType const& buffer) noexcept
{
    std::shared_ptr< profile::CheckerInterface > checker{};
    std::shared_ptr< e2e::StateMachine > stateMachine{};
    auto const& itChecker{config_.eventCheckers.find(id)};
    if (itChecker == config_.eventCheckers.end()) {
        End2EndEventProtectionProps const* p{StatusHandler::GetEPropByDataId(id)};
        if (nullptr == p) {
            return E2EResult::FromError(e2e::E2EErrc::kError);
        }
        checker     = CreateChecker(*p, kApSomeipHeaderoffset);
        std::ignore = config_.eventCheckers.insert(std::make_pair(id, checker));

        e2e::Config config{p->windowSizeValid,
                           p->minOkStateInit,
                           p->maxErrorStateInit,
                           p->minOkStateValid,
                           p->maxErrorStateValid,
                           p->minOkStateInvalid,
                           p->maxErrorStateInvalid,
                           p->windowSizeInit,
                           p->windowSizeInvalid,
                           static_cast< boolean >(p->clearFromValidToInvalid),
                           static_cast< boolean >(p->transitToInvalidExtended)};
        stateMachine = std::make_shared< e2e::StateMachine >(config);
        std::ignore  = config_.eventSM.insert(std::make_pair(id, stateMachine));
    } else {
        checker = itChecker->second;
        auto const& itStateMachine{config_.eventSM.find(id)};
        stateMachine = itStateMachine->second;
    }
    if (buffer.size() < (checker->GetHeaderLength() + checker->GetHeaderOffset())) {
        return E2EResult::FromError(e2e::E2EErrc::kError);
    }
    e2e::ProfileCheckStatus status;
    e2e::SMState state;
    checker->Check(buffer, status);
    stateMachine->Check(status, state);
    return E2EResult{{state, status, checker->GetCounter()}};
}

/// @brief e2e protection wrapper for events
/// @param[in] id dataID list
/// @return Result object -- empty/value or error
E2EResult Transformer::Check(IdlistType const& id) noexcept
{
    std::shared_ptr< e2e::StateMachine > stateMachine{};
    auto const& iter{config_.eventSM.find(id)};
    if (iter == config_.eventSM.end()) {
        End2EndEventProtectionProps const* p{StatusHandler::GetEPropByDataId(id)};
        if (nullptr == p) {
            return E2EResult::FromError(e2e::E2EErrc::kError);
        }
        e2e::Config config{p->windowSizeValid,
                           p->minOkStateInit,
                           p->maxErrorStateInit,
                           p->minOkStateValid,
                           p->maxErrorStateValid,
                           p->minOkStateInvalid,
                           p->maxErrorStateInvalid,
                           p->windowSizeInit,
                           p->windowSizeInvalid,
                           static_cast< boolean >(p->clearFromValidToInvalid),
                           static_cast< boolean >(p->transitToInvalidExtended)};
        stateMachine = std::make_shared< e2e::StateMachine >(config);
        std::ignore  = config_.eventSM.insert(std::make_pair(id, stateMachine));
    } else {
        stateMachine = iter->second;
    }

    e2e::ProfileCheckStatus status = e2e::ProfileCheckStatus::kNoNewData;
    e2e::SMState state;
    stateMachine->Check(status, state);
    return E2EResult{{state, status, 0}};  // TODO(yf) can't find checker
}

/// @brief e2e check wrapper for methods (proxy side) Response to method call
/// @param[in] id dataID list
/// @param[in] buffer Data to be checked
/// @param[in] messageType Message type kMessageTypeResponse
/// @param[in] messageResult Result type in response message. kMessageResultOk or kMessageResultError.
/// @param[in] sourceId Source ID
/// @return Result object -- check result or error
E2EResult Transformer::Check(IdlistType const& id,
                             BufferType const& buffer,
                             MessageType const& messageType,
                             ResultType const& messageResult,
                             SourceidType const& sourceId) noexcept
{
    std::shared_ptr< profile::CheckerInterface > checker{};
    std::shared_ptr< e2e::StateMachine > stateMachine{};
    auto const& itChecker{config_.methodCheckers.find(std::tuple< IdlistType, SourceidType >(id, sourceId))};
    if (itChecker == config_.methodCheckers.end()) {
        End2EndMethodProtectionProps const* p{StatusHandler::GetMPropByDataId(id)};
        if (nullptr == p) {
            return E2EResult::FromError(e2e::E2EErrc::kError);
        }
        checker     = CreateChecker(*p, kApSomeipHeaderoffset);
        std::ignore = config_.methodCheckers.insert(
            std::make_pair(std::tuple< IdlistType, SourceidType >(id, sourceId), checker));

        e2e::Config config{p->windowSizeValid,
                           p->minOkStateInit,
                           p->maxErrorStateInit,
                           p->minOkStateValid,
                           p->maxErrorStateValid,
                           p->minOkStateInvalid,
                           p->maxErrorStateInvalid,
                           p->windowSizeInit,
                           p->windowSizeInvalid,
                           static_cast< boolean >(p->clearFromValidToInvalid),
                           static_cast< boolean >(p->transitToInvalidExtended)};
        stateMachine = std::make_shared< e2e::StateMachine >(config);
        std::ignore  = config_.methodSM.insert(
            std::make_pair(std::tuple< IdlistType, SourceidType >(id, sourceId), stateMachine));
    } else {
        checker = itChecker->second;
        auto const& itStateMachine{config_.methodSM.find(std::tuple< IdlistType, SourceidType >(id, sourceId))};
        stateMachine = itStateMachine->second;
    }
    // if (buffer.size() < (checker->GetHeaderLength() + checker->GetHeaderOffset())) {
    //     return E2EResult::FromError(e2e::E2EErrc::kError);
    // }

    ComLogDebug("client", GenArg(id), GenArg(sourceId), GenArg(stateMachine.get()));
    if (buffer.size() < (checker->GetHeaderLength() + checker->GetHeaderOffset())) {
        return E2EResult::FromError(e2e::E2EErrc::kError);
    }
    e2e::ProfileCheckStatus status;
    e2e::SMState state;
    ara::core::Result< void > result{checker->Check(buffer, status, messageType, messageResult, sourceId)};
    stateMachine->Check(status, state);
    return E2EResult{{state, status, checker->GetCounter()}};
}

/// @brief e2e check wrapper for methods (skeleton side) Skeleton side E2E_Check for responding to proxy requests
/// @param[in] id dataID list
/// @param[in] buffer Data to be checked
/// @param[in] messageType Message type kMessageTypeRequest
/// @param[in] messageResult Result type in response message
/// @param[in] sourceId Source ID
/// @return Result object -- check result or error
E2EResult Transformer::Check(IdlistType const& id,
                             uint16_t const client,
                             uint16_t const methodId,
                             BufferType const& buffer,
                             MessageType const& messageType,
                             ResultType const& messageResult,
                             SourceidType* sourceId) noexcept
{
    /// If not found in clientId cache, it indicates a request from a new client
    bool isNewClient{false};
    std::shared_ptr< profile::CheckerInterface > checker{};
    std::shared_ptr< e2e::StateMachine > stateMachine{};

    auto it{clientSourceIdMap_.find(std::tuple< uint16_t, uint16_t >(client, methodId))};
    if (it == clientSourceIdMap_.end()) {
        isNewClient = true;
        End2EndMethodProtectionProps const* p{StatusHandler::GetMPropByDataId(id)};
        if (nullptr == p) {
            return E2EResult::FromError(e2e::E2EErrc::kError);
        }
        checker = CreateChecker(*p, kApSomeipHeaderoffset);

        e2e::Config config{p->windowSizeValid,
                           p->minOkStateInit,
                           p->maxErrorStateInit,
                           p->minOkStateValid,
                           p->maxErrorStateValid,
                           p->minOkStateInvalid,
                           p->maxErrorStateInvalid,
                           p->windowSizeInit,
                           p->windowSizeInvalid,
                           static_cast< boolean >(p->clearFromValidToInvalid),
                           static_cast< boolean >(p->transitToInvalidExtended)};
        stateMachine = std::make_shared< e2e::StateMachine >(config);
    } else {
        SourceidType sourceIdTmp = it->second;
        auto const& checkerIt{config_.methodCheckers.find(std::tuple< IdlistType, SourceidType >(id, sourceIdTmp))};
        auto const& itStateMachine{config_.methodSM.find(std::tuple< IdlistType, SourceidType >(id, sourceIdTmp))};
        if (checkerIt == config_.methodCheckers.end() || itStateMachine == config_.methodSM.end()) {
            ComLogError("method checker or method stateMachine can't find", GenArg(id), GenArg(sourceIdTmp));
            return E2EResult::FromError(e2e::E2EErrc::kError);
        }
        checker      = checkerIt->second;
        stateMachine = itStateMachine->second;
    }

    if (buffer.size() < (checker->GetHeaderLength() + checker->GetHeaderOffset())) {
        ComLogError("buffer size is error", GenArg(id), GenArg0(buffer.size()));
        return E2EResult::FromError(e2e::E2EErrc::kError);
    }
    e2e::ProfileCheckStatus status;
    e2e::SMState state;
    ara::core::Result< void > result{checker->Check(buffer, status, messageType, messageResult, sourceId)};

    if (isNewClient) {
        std::ignore = config_.methodCheckers.insert(
            std::make_pair(std::tuple< IdlistType, SourceidType >(id, *sourceId), checker));
        std::ignore = config_.methodSM.insert(
            std::make_pair(std::tuple< IdlistType, SourceidType >(id, *sourceId), stateMachine));
    }

    ComLogDebug("server", GenArg(id), GenArg(*sourceId), GenArg(stateMachine.get()));
    stateMachine->Check(status, state);
    if (!result) {
        return E2EResult::FromError(result.Error());
    }
    return E2EResult{{state, status, checker->GetCounter()}};
}

/// @brief Get e2e protection header length
/// @param[in] dataId DataID
/// @return e2e protection header length
uint32_t Transformer::GetHeaderLength(IdlistType const& dataId) noexcept
{
    return StatusHandler::GetHeaderLength(dataId);
}

/// @brief Insert clientId to sourceId mapping into cache
/// @param[in] clientId Client ID
/// @param[in] sourceId Source ID
void Transformer::InsertClientSourceIdMap(uint16_t const clientId,
                                          uint16_t const methodId,
                                          SourceidType const& sourceId) noexcept
{
    auto it{clientSourceIdMap_.find(std::tuple< uint16_t, uint16_t >(clientId, methodId))};
    if (it == clientSourceIdMap_.end()) {
        std::ignore
            = clientSourceIdMap_.insert(std::make_pair(std::tuple< uint16_t, uint16_t >(clientId, methodId), sourceId));
    }
}

/// @brief Get sourceid corresponding to clientId
/// @param[in] clientId Client ID
/// @return Result object -- SourceidType/value or error
ara::core::Result< SourceidType > Transformer::ClientIdToSourceId(uint16_t const clientId,
                                                                  uint16_t const methodId) noexcept
{
    auto it{clientSourceIdMap_.find(std::tuple< uint16_t, uint16_t >(clientId, methodId))};
    if (it == clientSourceIdMap_.end()) {
        return ara::core::Result< SourceidType >::FromError(e2e::E2EErrc::kError);
    }
    return ara::core::Result< SourceidType >::FromValue(it->second);
}

ara::core::Result< uint32_t > Transformer::GetProtecterCounter(IdlistType const& id,
                                                               SourceidType const& sourceId) noexcept
{
    std::shared_ptr< profile::ProtectorInterface > protector{};
    auto const& protectorIt{config_.methodProtectors.find({id, sourceId})};
    if (protectorIt == config_.methodProtectors.end()) {
        End2EndMethodProtectionProps* const p{StatusHandler::GetMPropByDataId(id)};
        if (nullptr == p) {
            return ara::core::Result< uint32_t >::FromError(e2e::E2EErrc::kError);
        }
        protector   = CreateProtector(*p, kApSomeipHeaderoffset);
        std::ignore = config_.methodProtectors.insert(
            std::make_pair(std::tuple< IdlistType, SourceidType >(id, sourceId), protector));
    } else {
        protector = protectorIt->second;
    }
    return protector->GetCounter();
}

}  // namespace e2exf
}  // namespace com
}  // namespace ara