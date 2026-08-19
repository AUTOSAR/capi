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
/// @file       generic_routine.cpp
/// @brief
/// @details
/// @date       2024-12-23
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================

#include "generic_routine.h"
namespace ara {
namespace diag {
namespace dmd {

void GenericRoutine::RegisterRoutineInstance(std::uint16_t routineId,
                                             uint16_t const& instanceId,
                                             uint32_t const& serviceInstanceId,
                                             ara::core::String& className) noexcept
{
    ComInfo comInfo{instanceId, serviceInstanceId, InterfaceType::kGenericRoutine};
    decltype(auto) findComInstance = comInfoToGenericInstance_.find(comInfo);
    if (findComInstance == comInfoToGenericInstance_.end()) {
        if (className == "DIAGNOSTIC-ROUTINE-GENERIC-INTERFACE") {
            GenericRoutineAgentPtr instancePtr
                = std::make_shared< isoft::dm::dic::GenericRoutineAgent >(instanceId, serviceInstanceId);
            comInfo.type = InterfaceType::kGenericRoutine;
            comInfoToGenericInstance_.insert({comInfo, std::move(instancePtr)});
        } else if (className == "DIAGNOSTIC-ROUTINE-INTERFACE") {
            RawRoutineAgentPtr instancePtr
                = std::make_shared< isoft::dm::dic::RawRoutineAgent >(instanceId, serviceInstanceId);
            comInfo.type = InterfaceType::kRawRoutine;
            comInfoToRawInstance_.insert({comInfo, std::move(instancePtr)});
        }
    }
    routineToComInfo_.insert({routineId, comInfo});
}

/// @brief Called for RoutineControl with SubFunction Start request for this DiagnosticRoutineIdentifier.
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
/// @param[in] routineId the corresponding RoutineIdentifier
/// @param[in] requestData Content of request message (without RoutineIdentifier)
/// @param[in] metaInfo contains additional meta information
/// @param[in] cancellationHandler informs if the current conversation is canceled
/// @return a Result with either OperationOutput (for a positive response message) or an UDS NRC value (for an
/// negative response message)
/// error code:
///> kGeneralReject, kServiceNotSupported, kSubfunctionNotSupported, kIncorrectMessageLengthOrInvalidFormat,
///> kResponseTooLong, kBusyRepeatRequest, kConditionsNotCorrect, kRequestSequenceError,
///> kNoResponseFromSubnetComponent, kFailurePreventsExecutionOfRequestedAction, kRequestOutOfRange,
///> kSecurityAccessDenied, kInvalidKey, kExceedNumberOfAttempts, kRequiredTimeDelayNotExpired,
/// kUploadDownloadNotAccepted, kTransferDataSuspended, kGeneralProgrammingFailure, kWrongBlockSequenceCounter
/// kSubFunctionNotSupportedInActiveSession, kServiceNotSupportedInActiveSession, kRpmTooHigh, kRpmTooLow
/// kEngineIsRunning, kEngineIsNotRunning, kEngineRunTimeTooLow, kTemperatureTooHigh, kTemperatureTooLow
///> kVehicleSpeedTooHigh, kVehicleSpeedTooLow, kThrottlePedalTooHigh, kThrottlePedalTooLow,
/// kTransmissionRangeNotInNeutral, kTransmissionRangeNotInGear, kBrakeSwitchNotClosed, kShifterLeverNotInPark
/// kTorqueConverterClutchLocked, kVoltageTooHigh, kVoltageTooLow
///
///
/// @traceid{SWS_DM_00554}@tracestatus{draft}
isoft::uds::Result< GenericRoutine::OperationOutput > GenericRoutine::Start(std::uint16_t routineId,
                                                                            std::vector< std::uint8_t > requestData,
                                                                            MetaInfoMap& metaInfo,
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
    ComInfo comInfo = _getComInfo(routineId);
    if (comInfo.type == InterfaceType::kGenericRoutine) {
        GenericRoutineAgentPtr instancePtr{_getGenericInstance(comInfo)};
        if (instancePtr.get() != nullptr) {
            ara::core::Future< std::vector< uint8_t > > requestResult
                = instancePtr->Start(routineId, requestData, comMetaInfo, cancellationEventPtr);
            ara::core::Result< std::vector< uint8_t > > result{requestResult.GetResult()};
            if (result.HasValue()) {
                GenericRoutine::OperationOutput output{result.Value()};
                return isoft::uds::Result< GenericRoutine::OperationOutput >::FromValue(output);
            }
            if (result.Error().Value() < 0) {
                return isoft::uds::Result< GenericRoutine::OperationOutput >::FromError(isoft::dm::kGeneralReject);
            }
            return isoft::uds::Result< GenericRoutine::OperationOutput >::FromError(result.Error().Value());
        }
    } else if (comInfo.type == InterfaceType::kRawRoutine) {
        RawRoutineAgentPtr instancePtr{_getRawInstance(comInfo)};
        if (instancePtr.get() != nullptr) {
            ara::core::Future< std::vector< uint8_t > > requestResult
                = instancePtr->Start(requestData, comMetaInfo, cancellationEventPtr);
            ara::core::Result< std::vector< uint8_t > > result{requestResult.GetResult()};
            if (result.HasValue()) {
                GenericRoutine::OperationOutput output{result.Value()};
                return isoft::uds::Result< GenericRoutine::OperationOutput >::FromValue(output);
            }
            if (result.Error().Value() < 0) {
                return isoft::uds::Result< GenericRoutine::OperationOutput >::FromError(isoft::dm::kGeneralReject);
            }
            return isoft::uds::Result< GenericRoutine::OperationOutput >::FromError(result.Error().Value());
        }
    }

    return isoft::uds::Result< GenericRoutine::OperationOutput >::FromError(isoft::dm::kGeneralReject);
}

/// @brief Called for RoutineControl with SubFunction Stop request for this DiagnosticRoutineIdentifier.
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
/// @param[in] routineId the corresponding RoutineIdentifier
/// @param[in] requestData Content of request message (without RoutineIdentifier)
/// @param[in] metaInfo contains additional meta information
/// @param[in] cancellationHandler informs if the current conversation is canceled
/// @return a Result with either OperationOutput (for a positive response message) or an UDS NRC value (for an
/// negative response message)
/// error code:
/// kGeneralReject, kServiceNotSupported, kSubfunctionNotSupported, kIncorrectMessageLengthOrInvalidFormat
///> kResponseTooLong, kBusyRepeatRequest, kConditionsNotCorrect, kRequestSequenceError,
///> kNoResponseFromSubnetComponent, kFailurePreventsExecutionOfRequestedAction, kRequestOutOfRange,
///> kSecurityAccessDenied, kInvalidKey, kExceedNumberOfAttempts, kRequiredTimeDelayNotExpired,
///> kUploadDownloadNotAccepted, kTransferDataSuspended, kGeneralProgrammingFailure, kWrongBlockSequenceCounter,
///> kSubFunctionNotSupportedInActiveSession, kServiceNotSupportedInActiveSession, kRpmTooHigh, kRpmTooLow,
///> kEngineIsRunning, kEngineIsNotRunning, kEngineRunTimeTooLow, kTemperatureTooHigh, kTemperatureTooLow,
///> kVehicleSpeedTooHigh, kVehicleSpeedTooLow, kThrottlePedalTooHigh, kThrottlePedalTooLow,
/// kTransmissionRangeNotInNeutral, kTransmissionRangeNotInGear, kBrakeSwitchNotClosed, kShifterLeverNotInPark
/// kTorqueConverterClutchLocked, kVoltageTooHigh, kVoltageTooLow
/// @throws on overflow
///
/// @traceid{SWS_DM_00555}@tracestatus{draft}
isoft::uds::Result< GenericRoutine::OperationOutput > GenericRoutine::Stop(std::uint16_t routineId,
                                                                           std::vector< std::uint8_t > requestData,
                                                                           MetaInfoMap& metaInfo,
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
    ComInfo comInfo = _getComInfo(routineId);
    if (comInfo.type == InterfaceType::kGenericRoutine) {
        GenericRoutineAgentPtr instancePtr{_getGenericInstance(comInfo)};
        if (instancePtr.get() != nullptr) {
            ara::core::Future< std::vector< uint8_t > > requestResult
                = instancePtr->Stop(routineId, requestData, comMetaInfo, cancellationEventPtr);
            ara::core::Result< std::vector< uint8_t > > result{requestResult.GetResult()};
            if (result.HasValue()) {
                GenericRoutine::OperationOutput output{result.Value()};
                return isoft::uds::Result< GenericRoutine::OperationOutput >::FromValue(output);
            }
            if (result.Error().Value() < 0) {
                return isoft::uds::Result< GenericRoutine::OperationOutput >::FromError(isoft::dm::kGeneralReject);
            }
            return isoft::uds::Result< GenericRoutine::OperationOutput >::FromError(result.Error().Value());
        }
    } else if (comInfo.type == InterfaceType::kRawRoutine) {
        RawRoutineAgentPtr instancePtr{_getRawInstance(comInfo)};
        if (instancePtr.get() != nullptr) {
            ara::core::Future< std::vector< uint8_t > > requestResult
                = instancePtr->Stop(requestData, comMetaInfo, cancellationEventPtr);
            ara::core::Result< std::vector< uint8_t > > result{requestResult.GetResult()};
            if (result.HasValue()) {
                GenericRoutine::OperationOutput output{result.Value()};
                return isoft::uds::Result< GenericRoutine::OperationOutput >::FromValue(output);
            }
            if (result.Error().Value() < 0) {
                return isoft::uds::Result< GenericRoutine::OperationOutput >::FromError(isoft::dm::kGeneralReject);
            }
            return isoft::uds::Result< GenericRoutine::OperationOutput >::FromError(result.Error().Value());
        }
    }
    return isoft::uds::Result< GenericRoutine::OperationOutput >::FromError(isoft::dm::kGeneralReject);
}

/// @brief Called for RoutineControl with SubFunction RequestResults request for this DiagnosticRoutineIdentifier.
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
/// @param[in] routineId the corresponding RoutineIdentifier
/// @param[in] requestData Content of request message (without RoutineIdentifier)
/// @param[in] metaInfo contains additional meta information
/// @param[in] cancellationHandler informs if the current conversation is canceled
/// @return a Result with either OperationOutput (for a positive response message) or an UDS NRC value (for an
/// negative response message)
/// error code:
///> kGeneralReject, kServiceNotSupported, kSubfunctionNotSupported, kIncorrectMessageLengthOrInvalidFormat,
///> kResponseTooLong, kBusyRepeatRequest, kConditionsNotCorrect, kRequestSequenceError,
///> kNoResponseFromSubnetComponent, kFailurePreventsExecutionOfRequestedAction, kRequestOutOfRange,
///> kSecurityAccessDenied, kInvalidKey, kExceedNumberOfAttempts, kRequiredTimeDelayNotExpired,
/// kUploadDownloadNotAccepted, kTransferDataSuspended, kGeneralProgrammingFailure, kWrongBlockSequenceCounter
/// kSubFunctionNotSupportedInActiveSession, kServiceNotSupportedInActiveSession, kRpmTooHigh, kRpmTooLow
/// kEngineIsRunning, kEngineIsNotRunning, kEngineRunTimeTooLow, kTemperatureTooHigh, kTemperatureTooLow
///> kVehicleSpeedTooHigh, kVehicleSpeedTooLow, kThrottlePedalTooHigh, kThrottlePedalTooLow,
/// kTransmissionRangeNotInNeutral, kTransmissionRangeNotInGear, kBrakeSwitchNotClosed, kShifterLeverNotInPark
/// kTorqueConverterClutchLocked, kVoltageTooHigh, kVoltageTooLow
/// @throws on overflow
///
/// @traceid{SWS_DM_00556}@tracestatus{draft}
isoft::uds::Result< GenericRoutine::OperationOutput > GenericRoutine::RequestResults(
    std::uint16_t routineId,
    std::vector< std::uint8_t > requestData,
    MetaInfoMap& metaInfo,
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

    ComInfo comInfo = _getComInfo(routineId);
    if (comInfo.type == InterfaceType::kGenericRoutine) {
        GenericRoutineAgentPtr instancePtr{_getGenericInstance(comInfo)};
        if (instancePtr.get() != nullptr) {
            ara::core::Future< std::vector< uint8_t > > requestResult
                = instancePtr->RequestResults(routineId, requestData, comMetaInfo, cancellationEventPtr);
            ara::core::Result< std::vector< uint8_t > > result{requestResult.GetResult()};
            if (result.HasValue()) {
                GenericRoutine::OperationOutput output{result.Value()};
                return isoft::uds::Result< GenericRoutine::OperationOutput >::FromValue(output);
            }
            if (result.Error().Value() < 0) {
                return isoft::uds::Result< GenericRoutine::OperationOutput >::FromError(isoft::dm::kGeneralReject);
            }
            return isoft::uds::Result< GenericRoutine::OperationOutput >::FromError(result.Error().Value());
        }
    } else if (comInfo.type == InterfaceType::kRawRoutine) {
        RawRoutineAgentPtr instancePtr{_getRawInstance(comInfo)};
        if (instancePtr.get() != nullptr) {
            ara::core::Future< std::vector< uint8_t > > requestResult
                = instancePtr->RequestResults(requestData, comMetaInfo, cancellationEventPtr);
            ara::core::Result< std::vector< uint8_t > > result{requestResult.GetResult()};
            if (result.HasValue()) {
                GenericRoutine::OperationOutput output{result.Value()};
                return isoft::uds::Result< GenericRoutine::OperationOutput >::FromValue(output);
            }
            if (result.Error().Value() < 0) {
                return isoft::uds::Result< GenericRoutine::OperationOutput >::FromError(isoft::dm::kGeneralReject);
            }
            return isoft::uds::Result< GenericRoutine::OperationOutput >::FromError(result.Error().Value());
        }
    }
    return isoft::uds::Result< GenericRoutine::OperationOutput >::FromError(isoft::dm::kGeneralReject);
}

GenericRoutine::ComInfo GenericRoutine::_getComInfo(std::uint16_t did) noexcept
{
    decltype(auto) findComInfo = routineToComInfo_.find(did);
    if (findComInfo != routineToComInfo_.end()) {
        return findComInfo->second;
    }
    return {};
}

GenericRoutine::GenericRoutineAgentPtr GenericRoutine::_getGenericInstance(ComInfo routineInfo) noexcept
{
    decltype(auto) findComInstance = comInfoToGenericInstance_.find(routineInfo);
    if (findComInstance != comInfoToGenericInstance_.end()) {
        return findComInstance->second;
    }

    return nullptr;
}

GenericRoutine::RawRoutineAgentPtr GenericRoutine::_getRawInstance(ComInfo routineInfo) noexcept
{
    decltype(auto) findComInstance = comInfoToRawInstance_.find(routineInfo);
    if (findComInstance != comInfoToRawInstance_.end()) {
        return findComInstance->second;
    }

    return nullptr;
}
}  // namespace dmd
}  // namespace diag
}  // namespace ara
