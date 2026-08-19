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
/// @file       generic_data_identifier.h
/// @brief      This file provides the definitions of GenericDataIdentifier and related types.
/// @details
/// @date       2021-11-23
/// @author     jiawei
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_DIAG_GENERIC_DATA_IDENTIFIER_H_
#define ARA_DIAG_GENERIC_DATA_IDENTIFIER_H_

#include <ara/core/future.h>
#include <ara/core/instance_specifier.h>
#include <ara/core/result.h>
#include <ara/core/span.h>
#include <ara/core/vector.h>

#include <cstdint>
#include <memory>

#include "cancellation_handler.h"
#include "meta_info.h"
#include "reentrancy.h"

namespace isoft {
namespace dm {
namespace dis {
/// @brief GenericDataIdentifierAgent
class GenericDataIdentifierAgent;
}  // namespace dis
}  // namespace dm
}  // namespace isoft
namespace ara {
namespace diag {

/// @brief Generic DataIdentifier interface
///
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
///
/// @traceid{SWS_DM_00607}@tracestatus{draft}
class GenericDataIdentifier
{
public:
    /// @brief Response data of positive respone message
    ///
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    ///
    /// @traceid{SWS_DM_00641}@tracestatus{draft}
    struct OperationOutput
    {
        /// @brief Content of positive respone message (without DataIdentifier)
        ///
        /// @traceid{SWS_DM_00631}@tracestatus{draft}
        ara::core::Vector< std::uint8_t > responseData;
    };

public:
    /// @brief Class for an GenericDataIdentifier
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[in] specifier InstanceSpecifier to an PortPrototype of an DiagnosticDataIdentifierGenericInterface
    /// @param[in] reentrancyType specifies if interface is callable fully- or non-reentrant for reads, writes or both
    /// @throws on overflow
    ///
    /// @traceid{SWS_DM_00634}@tracestatus{draft}
    explicit GenericDataIdentifier(ara::core::InstanceSpecifier const& specifier,
                                   DataIdentifierReentrancyType reentrancyType);

    /// @brief Destructor of class GenericDataIdentifier
    ///
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    ///
    /// @traceid{SWS_DM_00635}@tracestatus{draft}
    virtual ~GenericDataIdentifier() noexcept = default;

    /// @brief copy constructor
    /// @param other
    GenericDataIdentifier(GenericDataIdentifier const& other) noexcept = default;
    /// @brief copy assignment operator
    /// @param other
    /// @return GenericDataIdentifier&
    GenericDataIdentifier& operator=(GenericDataIdentifier const& other) noexcept = default;
    /// @brief move constructor
    /// @param other
    GenericDataIdentifier(GenericDataIdentifier&& other) noexcept = default;
    /// @brief move assignment operator
    /// @param other
    /// @return GenericDataIdentifier&
    GenericDataIdentifier& operator=(GenericDataIdentifier&& other) noexcept = default;

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
    virtual ara::core::Future< OperationOutput > Read(std::uint16_t dataIdentifier,
                                                      MetaInfo& metaInfo,
                                                      CancellationHandler cancellationHandler)
        = 0;

    /// @brief Called for WriteDataByIdentifier request for this DiagnosticDataIdentifier.
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[in] dataIdentifier the corresponding DataIdentifier
    /// @param[in] requestData Content of request message (without DataIdentifier)
    /// @param[in] metaInfo contains additional meta information
    /// @param[in] cancellationHandler informs if the current conversation is canceled
    /// @return a Result with either void (for a positive response message) or an UDS NRC value (for an negative
    /// response message)
    /// error code:
    /// kGeneralReject, kServiceNotSupported, kSubfunctionNotSupported, kIncorrectMessageLengthOrInvalidFormat
    /// kResponseTooLong, kBusyRepeatRequest, kConditionsNotCorrect, kRequestSequenceError
    /// kNoResponseFromSubnetComponent, kFailurePreventsExecutionOfRequestedAction, kRequestOutOfRange
    ///> kSecurityAccessDenied, kInvalidKey, kExceedNumberOfAttempts, kRequiredTimeDelayNotExpired,
    /// kUploadDownloadNotAccepted, kTransferDataSuspended, kGeneralProgrammingFailure, kWrongBlockSequenceCounter
    /// kSubFunctionNotSupportedInActiveSession, kServiceNotSupportedInActiveSession, kRpmTooHigh, kRpmTooLow
    /// kEngineIsRunning, kEngineIsNotRunning, kEngineRunTimeTooLow, kTemperatureTooHigh, kTemperatureTooLow
    ///> kVehicleSpeedTooHigh, kVehicleSpeedTooLow, kThrottlePedalTooHigh, kThrottlePedalTooLow,
    /// kTransmissionRangeNotInNeutral, kTransmissionRangeNotInGear, kBrakeSwitchNotClosed, kShifterLeverNotInPark
    /// kTorqueConverterClutchLocked, kVoltageTooHigh, kVoltageTooLow
    /// @throws on overflow
    ///
    /// @traceid{SWS_DM_00637}@tracestatus{draft}
    virtual ara::core::Future< void > Write(std::uint16_t dataIdentifier,
                                            ara::core::Span< std::uint8_t > requestData,
                                            MetaInfo& metaInfo,
                                            CancellationHandler cancellationHandler)
        = 0;

    /// @brief This Offer will enable the DM to forward request messages to this handler
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @return ara::core::Result<void> error code:kNotOffered, kGenericError, kAlreadyOffered
    /// @throws on overflow
    /// @traceid{SWS_DM_00638}@tracestatus{draft}
    ara::core::Result< void > Offer();

    /// @brief This StopOffer will disable the forwarding of request messages from DM
    ///
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @throws on overflow
    /// @traceid{SWS_DM_00639}@tracestatus{draft}
    void StopOffer();

private:
    /// @name specifier_
    ara::core::InstanceSpecifier specifier_;
    /// @name reentrancyType_
    DataIdentifierReentrancyType reentrancyType_;
    std::shared_ptr< isoft::dm::dis::GenericDataIdentifierAgent >
        /// @name agent_
        agent_;
};
}  // namespace diag
}  // namespace ara

#endif  // ARA_DIAG_GENERIC_DATA_IDENTIFIER_H_