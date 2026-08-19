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
/// @file       generic_data_element.cpp
/// @brief
/// @details
/// @date       2024-12-28
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================

#include "generic_data_element.h"

#include "gen_code/defines.h"
#include "serialization/serialization.h"
namespace ara {
namespace diag {
namespace dmd {

void GenericDataElement::RegisterServiceInstance(std::uint32_t did,
                                                 uint16_t const& instanceId,
                                                 uint32_t const& serviceInstanceId) noexcept
{
    ComInfo comInfo{instanceId, serviceInstanceId};
    decltype(auto) findComInstance = comInfoToRawDataIdInstance_.find(comInfo);
    if (findComInstance == comInfoToRawDataIdInstance_.end()) {
        RawDataElementAgentPtr instancePtr
            = std::make_shared< isoft::dm::dic::RawDataElementAgent >(instanceId, serviceInstanceId);
        comInfoToRawDataIdInstance_.insert({comInfo, std::move(instancePtr)});
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
isoft::uds::Result< std::list< isoft::uds::server::DiagnosticDataElement > > GenericDataElement::Read(
    std::vector< std::uint32_t >& dataElementTable,
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
    std::map< std::uint32_t, ReadResultTypePtr > resultTable;
    for (auto&& dataElement : dataElementTable) {
        ComInfo comInfo = _getComInfo(dataElement);
        RawDataElementAgentPtr instancePtr{_getGenericDataElementInstance(comInfo)};
        if (instancePtr.get() != nullptr) {
            ara::core::Future< ara::core::Vector< uint8_t > > requestResult
                = instancePtr->Read(comMetaInfo, cancellationEventPtr);
            ReadResultTypePtr resultPtr{std::make_shared< ReadResultType >(std::move(requestResult))};
            resultTable.insert({dataElement, resultPtr});
        }
    }
    std::list< isoft::uds::server::DiagnosticDataElement > diagnosticDataList{};
    for (auto&& readResult : resultTable) {
        isoft::uds::server::DiagnosticDataElement data{};
        ara::core::Result< ara::core::Vector< uint8_t > > result = readResult.second->GetResult();
        if (result.HasValue()) {
            data.id = readResult.first;
            data.record.assign(result.Value().begin(), result.Value().end());
            diagnosticDataList.push_back(data);
        }
    }

    return isoft::uds::Result< std::list< isoft::uds::server::DiagnosticDataElement > >::FromValue(diagnosticDataList);
}

GenericDataElement::ComInfo GenericDataElement::_getComInfo(std::uint32_t did) noexcept
{
    decltype(auto) findComInfo = didToComInfo_.find(did);
    if (findComInfo != didToComInfo_.end()) {
        return findComInfo->second;
    }
    return {};
}

GenericDataElement::RawDataElementAgentPtr GenericDataElement::_getGenericDataElementInstance(ComInfo info) noexcept
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
