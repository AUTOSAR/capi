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
/// @file       generic_data_identifier.cpp
/// @brief
/// @details
/// @date       2024-12-28
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================

#include "generic_data_identifier.h"

#include "gen_code/defines.h"
#include "log/log.h"
#include "serialization/serialization.h"
#include "serialization/serialization/common_data_type.h"
namespace ara {
namespace diag {
namespace dmd {

void GenericDataIdentifier::RegisterServiceInstance(std::uint16_t did,
                                                    uint16_t const& instanceId,
                                                    uint32_t const& serviceInstanceId,
                                                    ara::core::String& serviceClass) noexcept
{
    InterfaceType type{};
    ComInfo comInfo{instanceId, serviceInstanceId, type};
    if (serviceClass == "DIAGNOSTIC-DATA-IDENTIFIER-GENERIC-INTERFACE") {
        type                           = InterfaceType::kGenericDataIdentifier;
        comInfo.type                   = type;
        decltype(auto) findComInstance = comInfoToGenericDataIdInstance_.find(comInfo);
        if (findComInstance == comInfoToGenericDataIdInstance_.end()) {
            GenericDataIdentifierAgentPtr instancePtr
                = std::make_shared< isoft::dm::dic::GenericDataIdentifierAgent >(instanceId, serviceInstanceId);
            comInfoToGenericDataIdInstance_.insert({comInfo, std::move(instancePtr)});
        }
    } else if (serviceClass == "DIAGNOSTIC-GENERIC-UDS-INTERFACE") {
        type                           = InterfaceType::kGenericUDSService;
        comInfo.type                   = type;
        decltype(auto) findComInstance = comInfoToGenericUdsInstance_.find(comInfo);
        if (findComInstance == comInfoToGenericUdsInstance_.end()) {
            GenericUdsServiceAgentPtr instancePtr
                = std::make_shared< isoft::dm::dic::GenericUdsServiceAgent >(instanceId, serviceInstanceId);
            comInfoToGenericUdsInstance_.insert({comInfo, std::move(instancePtr)});
        }
    } else if (serviceClass == "DIAGNOSTIC-DATA-IDENTIFIER-INTERFACE") {
        type                           = InterfaceType::kRawDataIdentifier;
        comInfo.type                   = type;
        decltype(auto) findComInstance = comInfoToRawDataIdInstance_.find(comInfo);
        if (findComInstance == comInfoToRawDataIdInstance_.end()) {
            RawDataIdentifierAgentPtr instancePtr
                = std::make_shared< isoft::dm::dic::RawDataIdentifierAgent >(instanceId, serviceInstanceId);
            comInfoToRawDataIdInstance_.insert({comInfo, std::move(instancePtr)});
        }
    }
    didToComInfo_.insert({did, comInfo});
}

/// @brief Called for ReadDataByIdentifier request for this DiagnosticDataIdentifier.
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
/// @param[in] dataIdentifier the corresponding DataIdentifier
/// @param[in] metaInfo contains additional meta information
/// @param[in] cancellationHandler informs if the current conversation is canceled
/// @return a Result with either OperationOutput (for a positive response message) or an UDS NRC value (for an
/// negative response message)
/// error code:
///> kGeneralReject, kServiceNotSupported, kSubfunctionNotSupported, kIncorrectMessageLengthOrInvalidFormat,
/// kResponseTooLong kBusyRepeatRequest, kConditionsNotCorrect, kRequestSequenceError,
/// kNoResponseFromSubnetComponent kFailurePreventsExecutionOfRequestedAction, kRequestOutOfRange,
/// kSecurityAccessDenied kInvalidKey, kExceedNumberOfAttempts, kRequiredTimeDelayNotExpired,
///> kUploadDownloadNotAccepted, kTransferDataSuspended kGeneralProgrammingFailure, kWrongBlockSequenceCounter,
///> kSubFunctionNotSupportedInActiveSession, kServiceNotSupportedInActiveSession, kRpmTooHigh, kRpmTooLow,
///> kEngineIsRunning, kEngineIsNotRunning, kEngineRunTimeTooLow kTemperatureTooHigh, kTemperatureTooLow,
///> kVehicleSpeedTooHigh, kVehicleSpeedTooLow, kThrottlePedalTooHigh, kThrottlePedalTooLow,
///> kTransmissionRangeNotInNeutral, kTransmissionRangeNotInGear, kBrakeSwitchNotClosed, kShifterLeverNotInPark,
/// kTorqueConverterClutchLocked, kVoltageTooHigh, kVoltageTooLow, kResourceTemporarilyNotAvailable
///
///
/// @traceid{SWS_DM_00636}@tracestatus{draft}
isoft::uds::Result< std::list< isoft::uds::server::DiagnosticData > > GenericDataIdentifier::Read(
    std::vector< std::uint16_t >& dataIdentifierTable,
    MetaInfoMap const& metaInfo,
    CancellationHandler cancellationHandler)
{
    isoft::dm::MetaInfoMap comMetaInfo{};
    for (auto&& pair : metaInfo) {
        ara::core::String key{pair.first};
        ara::core::String value{pair.second};
        comMetaInfo.insert({key, value});
    }
    std::shared_ptr< isoft::dm::CancellationEvent > cancellationEventPtr{
        std::make_shared< isoft::dm::CancellationEvent >()};
    cancellationHandler.SetNotifier([&cancellationEventPtr]() { cancellationEventPtr->Notifier(); });
    using ReadResultType    = ara::core::Future< ara::core::Vector< uint8_t > >;
    using ReadResultTypePtr = std::shared_ptr< ReadResultType >;
    std::map< std::uint16_t, ReadResultTypePtr > resultTable;
    for (auto&& dataIdentifier : dataIdentifierTable) {
        ComInfo comInfo = _getComInfo(dataIdentifier);
        if (comInfo.type == InterfaceType::kGenericDataIdentifier) {
            GenericDataIdentifierAgentPtr instancePtr{_getGenericDataIdentifierInstance(comInfo)};
            if (instancePtr.get() != nullptr) {
                ara::core::Future< ara::core::Vector< uint8_t > > requestResult
                    = instancePtr->Read(dataIdentifier, comMetaInfo, cancellationEventPtr);
                ReadResultTypePtr resultPtr{std::make_shared< ReadResultType >(std::move(requestResult))};
                resultTable.insert({dataIdentifier, resultPtr});
            }
        } else if (comInfo.type == InterfaceType::kGenericUDSService) {
            GenericUdsServiceAgentPtr instancePtr{_getGenericUdsServiceInstance(comInfo)};
            if (instancePtr.get() != nullptr) {
                ara::core::Vector< uint8_t > request;
                isoft::serialize::Serialize(request, dataIdentifier);
                ara::core::Future< ara::core::Vector< uint8_t > > requestResult = instancePtr->HandleMessage(
                    isoft::serialize::kInt8_0x22U, request, comMetaInfo, cancellationEventPtr);
                ReadResultTypePtr resultPtr{std::make_shared< ReadResultType >(std::move(requestResult))};
                resultTable.insert({dataIdentifier, resultPtr});
            }
        } else if (comInfo.type == InterfaceType::kRawDataIdentifier) {
            RawDataIdentifierAgentPtr instancePtr{_getRawDataIdentifierInstance(comInfo)};
            if (instancePtr.get() != nullptr) {
                ara::core::Future< ara::core::Vector< uint8_t > > requestResult
                    = instancePtr->Read(comMetaInfo, cancellationEventPtr);
                ReadResultTypePtr resultPtr{std::make_shared< ReadResultType >(std::move(requestResult))};
                resultTable.insert({dataIdentifier, resultPtr});
            }
        }
    }
    std::list< isoft::uds::server::DiagnosticData > diagnosticDataList{};
    for (auto&& readResult : resultTable) {
        isoft::uds::server::DiagnosticData data{};
        ara::core::Result< ara::core::Vector< uint8_t > > result = readResult.second->GetResult();
        if (result.HasValue()) {
            data.id = readResult.first;
            data.record.assign(result.Value().begin(), result.Value().end());
        } else {
            if (resultTable.size() == 1) {
                common::LogDebug() << "GenericDataIdentifier::Read| DiagErrorDomain:" << result.Error().Domain().Name();
                if (result.Error().Value() < 0) {
                    return isoft::uds::Result< std::list< isoft::uds::server::DiagnosticData > >::FromError(
                        isoft::dm::kGeneralReject);
                }
                return isoft::uds::Result< std::list< isoft::uds::server::DiagnosticData > >::FromError(
                    result.Error().Value());
            }
        }
        diagnosticDataList.push_back(data);
    }

    return isoft::uds::Result< std::list< isoft::uds::server::DiagnosticData > >::FromValue(diagnosticDataList);
}

isoft::uds::Result< void > GenericDataIdentifier::Write(isoft::uds::server::DiagnosticData reuqestDataRecord,
                                                        MetaInfoMap const& metaInfo,
                                                        CancellationHandler cancellationHandler)
{
    isoft::dm::MetaInfoMap comMetaInfo{};
    for (auto&& pair : metaInfo) {
        ara::core::String key{pair.first};
        ara::core::String value{pair.second};
        comMetaInfo.insert({key, value});
    }
    std::shared_ptr< isoft::dm::CancellationEvent > cancellationEventPtr{
        std::make_shared< isoft::dm::CancellationEvent >()};
    cancellationHandler.SetNotifier([&cancellationEventPtr]() { cancellationEventPtr->Notifier(); });

    ComInfo comInfo = _getComInfo(reuqestDataRecord.id);
    if (comInfo.type == InterfaceType::kGenericDataIdentifier) {
        GenericDataIdentifierAgentPtr instancePtr{_getGenericDataIdentifierInstance(comInfo)};
        if (instancePtr.get() != nullptr) {
            ara::core::Future< void > result
                = instancePtr->Write(reuqestDataRecord.id, reuqestDataRecord.record, comMetaInfo, cancellationEventPtr);
            ara::core::Result< void > res = result.GetResult();
            if (res.HasValue()) {
                return {};
            }

            if (res.Error().Value() < 0) {
                return isoft::uds::Result< void >::FromError(isoft::dm::kGeneralReject);
            }

            return isoft::uds::Result< void >::FromError(res.Error().Value());
        }
    } else if (comInfo.type == InterfaceType::kGenericUDSService) {
        GenericUdsServiceAgentPtr instancePtr{_getGenericUdsServiceInstance(comInfo)};
        if (instancePtr.get() != nullptr) {
            ara::core::Vector< uint8_t > request;
            isoft::serialize::Serialize(request, reuqestDataRecord.id);
            request.insert(request.end(), reuqestDataRecord.record.begin(), reuqestDataRecord.record.end());
            ara::core::Future< ara::core::Vector< uint8_t > > result
                = instancePtr->HandleMessage(isoft::serialize::kInt8_0x2EU, request, comMetaInfo, cancellationEventPtr);
            ara::core::Result< ara::core::Vector< uint8_t > > res = result.GetResult();
            if (res.HasValue()) {
                return {};
            }
            if (res.Error().Value() < 0) {
                return isoft::uds::Result< void >::FromError(isoft::dm::kGeneralReject);
            }
            return isoft::uds::Result< void >::FromError(res.Error().Value());
        }
    } else if (comInfo.type == InterfaceType::kRawDataIdentifier) {
        RawDataIdentifierAgentPtr instancePtr{_getRawDataIdentifierInstance(comInfo)};
        if (instancePtr.get() != nullptr) {
            ara::core::Future< void > requestResult
                = instancePtr->Write(reuqestDataRecord.record, comMetaInfo, cancellationEventPtr);
            ara::core::Result< void > res = requestResult.GetResult();
            if (res.HasValue()) {
                return {};
            }
            if (res.Error().Value() < 0) {
                return isoft::uds::Result< void >::FromError(isoft::dm::kGeneralReject);
            }
            return isoft::uds::Result< void >::FromError(res.Error().Value());
        }
    }

    return isoft::uds::Result< void >::FromError(isoft::serialize::kInt32_0x10);
}

GenericDataIdentifier::ComInfo GenericDataIdentifier::_getComInfo(std::uint16_t did) noexcept
{
    decltype(auto) findComInfo = didToComInfo_.find(did);
    if (findComInfo != didToComInfo_.end()) {
        return findComInfo->second;
    }
    return {};
}

GenericDataIdentifier::GenericDataIdentifierAgentPtr GenericDataIdentifier::_getGenericDataIdentifierInstance(
    ComInfo info) noexcept
{
    decltype(auto) findComInstance = comInfoToGenericDataIdInstance_.find(info);
    if (findComInstance != comInfoToGenericDataIdInstance_.end()) {
        return findComInstance->second;
    }
    return {nullptr};
}

GenericDataIdentifier::GenericUdsServiceAgentPtr GenericDataIdentifier::_getGenericUdsServiceInstance(
    ComInfo info) noexcept
{
    decltype(auto) findComInstance = comInfoToGenericUdsInstance_.find(info);
    if (findComInstance != comInfoToGenericUdsInstance_.end()) {
        return findComInstance->second;
    }
    return {nullptr};
}

GenericDataIdentifier::RawDataIdentifierAgentPtr GenericDataIdentifier::_getRawDataIdentifierInstance(
    ComInfo info) noexcept
{
    decltype(auto) findComInstance = comInfoToRawDataIdInstance_.find(info);
    if (findComInstance != comInfoToRawDataIdInstance_.end()) {
        return findComInstance->second;
    }
    return {nullptr};
}
}  // namespace dmd
}  // namespace diag
}  // namespace ara
