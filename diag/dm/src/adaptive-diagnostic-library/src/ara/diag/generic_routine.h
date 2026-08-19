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
/// @file       generic_routine.h
/// @brief      This file provides the definitions of GenericRoutine and related types.
/// @details
/// @date       2021-11-23
/// @author     jiawei
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_DIAG_GENERIC_ROUTINE_H_
#define ARA_DIAG_GENERIC_ROUTINE_H_

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
/// @brief GenericRoutineAgent
class GenericRoutineAgent;
}  // namespace dis
}  // namespace dm
}  // namespace isoft

namespace ara {
namespace diag {

/// @brief Generic Routine interface
///
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
///
/// @traceid{SWS_DM_00605}@tracestatus{draft}
class GenericRoutine
{
public:
    /// @brief Response data of positive respone message
    ///
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    ///
    /// @traceid{SWS_DM_00551}@tracestatus{draft}
    struct OperationOutput
    {
        /// @brief Content of positive respone message (without RoutineIdentifier)
        ///
        /// @traceid{SWS_DM_00633}@tracestatus{draft}
        ara::core::Vector< std::uint8_t > responseData;
    };

public:
    /// @brief Class for an GenericRoutine
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[in] specifier InstanceSpecifier to an PortPrototype of an DiagnosticRoutineGenericInterface
    /// @param[in] reentrancyType specifies if interface is callable fully- or non-reentrant
    /// @throws on overflow
    ///
    /// @traceid{SWS_DM_00552}@tracestatus{draft}
    explicit GenericRoutine(ara::core::InstanceSpecifier const& specifier, ReentrancyType reentrancyType);

    /// @brief Destructor of class GenericRoutine
    ///
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    ///
    /// @traceid{SWS_DM_00553}@tracestatus{draft}
    virtual ~GenericRoutine() noexcept = default;

    /// @brief copy constructor
    /// @param other
    GenericRoutine(GenericRoutine const& other) noexcept = default;
    /// @brief copy assignment operator
    /// @param other
    /// @return GenericRoutine&
    GenericRoutine& operator=(GenericRoutine const& other) noexcept = default;
    /// @brief move constructor
    /// @param other
    GenericRoutine(GenericRoutine&& other) noexcept = default;
    /// @brief move assignment operator
    /// @param other
    /// @return GenericRoutine&
    GenericRoutine& operator=(GenericRoutine&& other) noexcept = default;

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
    virtual ara::core::Future< OperationOutput > Start(std::uint16_t routineId,
                                                       ara::core::Span< std::uint8_t > requestData,
                                                       MetaInfo& metaInfo,
                                                       CancellationHandler cancellationHandler)
        = 0;

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
    virtual ara::core::Future< OperationOutput > Stop(std::uint16_t routineId,
                                                      ara::core::Span< std::uint8_t > requestData,
                                                      MetaInfo& metaInfo,
                                                      CancellationHandler cancellationHandler)
        = 0;

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
    virtual ara::core::Future< OperationOutput > RequestResults(std::uint16_t routineId,
                                                                ara::core::Span< std::uint8_t > requestData,
                                                                MetaInfo& metaInfo,
                                                                CancellationHandler cancellationHandler)
        = 0;

    /// @brief This Offer will enable the DM to forward request messages to this handler
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @return ara::core::Result<void>  error code: kNotOffered, kGenericError, kAlreadyOffered
    /// @throws on overflow
    ///
    /// @traceid{SWS_DM_00557}@tracestatus{draft}
    ara::core::Result< void > Offer();

    /// @brief This StopOffer will disable the forwarding of request messages from DM
    ///
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @throws on overflow
    /// @traceid{SWS_DM_00558}@tracestatus{draft}
    void StopOffer();

private:
    /// @name specifier_
    ara::core::InstanceSpecifier specifier_;
    /// @name reentrancyType_
    ReentrancyType reentrancyType_;
    std::shared_ptr< isoft::dm::dis::GenericRoutineAgent >
        /// @name skeleton_
        skeleton_{};
};

}  // namespace diag
}  // namespace ara

#endif  // ARA_DIAG_GENERIC_ROUTINE_H_