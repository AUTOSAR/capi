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
/// @brief      This file provides generic_routine functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/isoft/uds/routine_management
/// @interface_level=software
/// @trace_id_sr=aadc6f5f-7092-492e-8f4b-b1958e845481
/// @unit_name=GenericRoutine
/// @unit_description=This file provides generic_routine functionality
/// @endcode
///
/// ================================================================

#ifndef ISOFT_UDS_SERVER_GENERICROUTINE_H_
#define ISOFT_UDS_SERVER_GENERICROUTINE_H_

#include <cstdint>
#include <memory>
#include <vector>

#include "isoft/uds/cancellation_handler.h"
#include "isoft/uds/common.h"
#include "isoft/uds/result.h"

namespace isoft {
namespace uds {
namespace server {

/// @brief Generic routine interface
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01352
/// @trace_id_dd=DD_UDS_02644
/// @needwork = ad
/// @endcode
class GenericRoutineInterface
{
public:
    /// @param
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_01004
    /// @needwork = dda
    /// @endcode
    GenericRoutineInterface(GenericRoutineInterface const&) = delete;
    /// @param
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_01005
    /// @needwork = dda
    /// @endcode
    GenericRoutineInterface& operator=(GenericRoutineInterface const&) = delete;
    /// @param
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_01006
    /// @needwork = dda
    /// @endcode
    GenericRoutineInterface(GenericRoutineInterface&&) = delete;
    /// @param
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_01007
    /// @needwork = dda
    /// @endcode
    GenericRoutineInterface& operator=(GenericRoutineInterface&&) = delete;

public:
    /// @brief Operation output
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01080
    /// @trace_id_dd=DD_UDS_02194
    /// @needwork = ad
    /// @endcode
    struct OperationOutput
    {
        ///
        /// @trace_id_sws={SWS_DM_00633}@tracestatus{draft}

        /// @brief Response data
        /// @code{.isoft}
        /// @interface_level=unit
        /// @needwork=ad
        /// @endcode
        std::vector< std::uint8_t > responseData;
    };

public:
    /// @brief Constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01081
    /// @trace_id_dd=DD_UDS_02195
    /// @needwork = ad
    /// @endcode
    explicit GenericRoutineInterface() = default;

    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01082
    /// @trace_id_dd=DD_UDS_02196
    /// @needwork = ad
    /// @endcode
    virtual ~GenericRoutineInterface() noexcept = default;

    /// @brief Start routine
    /// @param[in] routineId Corresponding routine identifier
    /// @param[in] requestData Request message content (excluding RoutineIdentifier)
    /// @param[in] metaInfo Contains additional meta information
    /// @param[in] cancellationHandler Notifies whether the current session is cancelled
    /// @return Returns Result, contains OperationOutput on success (for positive response message), contains UDS NRC value on failure (for negative response message)
    /// Possible error codes:
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @retval kGeneralReject: General reject
    /// @retval kServiceNotSupported: Service not supported
    /// @retval kSubfunctionNotSupported: Subfunction not supported
    /// @retval kIncorrectMessageLengthOrInvalidFormat: Incorrect message length or invalid format
    /// @retval kResponseTooLong: Response too long
    /// @retval kBusyRepeatRequest: Busy, repeat request
    /// @retval kConditionsNotCorrect: Conditions not correct
    /// @retval kRequestSequenceError: Request sequence error
    /// @retval kNoResponseFromSubnetComponent: No response from subnet component
    /// @retval kFailurePreventsExecutionOfRequestedAction: Failure prevents execution of requested action
    /// @retval kRequestOutOfRange: Request out of range
    /// @retval kSecurityAccessDenied: Security access denied
    /// @retval kInvalidKey: Invalid key
    /// @retval kExceedNumberOfAttempts: Exceed number of attempts
    /// @retval kRequiredTimeDelayNotExpired: Required time delay not expired
    /// @retval kUploadDownloadNotAccepted: Upload download not accepted
    /// @retval kTransferDataSuspended: Transfer data suspended
    /// @retval kGeneralProgrammingFailure: General programming failure
    /// @retval kWrongBlockSequenceCounter: Wrong block sequence counter
    /// @retval kSubFunctionNotSupportedInActiveSession: Subfunction not supported in active session
    /// @retval kServiceNotSupportedInActiveSession: Service not supported in active session
    /// @retval kRpmTooHigh: RPM too high
    /// @retval kRpmTooLow: RPM too low
    /// @retval kEngineIsRunning: Engine is running
    /// @retval kEngineIsNotRunning: Engine is not running
    /// @retval kEngineRunTimeTooLow: Engine run time too low
    /// @retval kTemperatureTooHigh: Temperature too high
    /// @retval kTemperatureTooLow: Temperature too low
    /// @retval kVehicleSpeedTooHigh: Vehicle speed too high
    /// @retval kVehicleSpeedTooLow: Vehicle speed too low
    /// @retval kThrottlePedalTooHigh: Throttle pedal too high
    /// @retval kThrottlePedalTooLow: Throttle pedal too low
    /// @retval kTransmissionRangeNotInNeutral: Transmission range not in neutral
    /// @retval kTransmissionRangeNotInGear: Transmission range not in gear
    /// @retval kBrakeSwitchNotClosed: Brake switch not closed
    /// @retval kShifterLeverNotInPark: Shifter lever not in park
    /// @retval kTorqueConverterClutchLocked: Torque converter clutch locked
    /// @retval kVoltageTooHigh: Voltage too high
    /// @retval kVoltageTooLow: Voltage too low
    ///
    /// @trace_id_sws={SWS_DM_00554}@tracestatus{draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00886
    /// @trace_id_dd=DD_UDS_01008
    /// @needwork = ad
    /// @endcode
    virtual Result< OperationOutput > Start(std::uint16_t routineId,
                                            std::vector< std::uint8_t > requestData,
                                            MetaInfoMap& metaInfo,
                                            CancellationHandler cancellationHandler)
        = 0;

    /// @brief Stop routine
    /// @param[in] routineId Corresponding routine identifier
    /// @param[in] requestData Request message content (excluding RoutineIdentifier)
    /// @param[in] metaInfo Contains additional meta information
    /// @param[in] cancellationHandler Notifies whether the current session is cancelled
    /// @return Returns Result, contains OperationOutput on success (for positive response message), contains UDS NRC value on failure (for negative response message)
    /// Possible error codes:
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @retval kGeneralReject: General reject
    /// @retval kServiceNotSupported: Service not supported
    /// @retval kSubfunctionNotSupported: Subfunction not supported
    /// @retval kIncorrectMessageLengthOrInvalidFormat: Incorrect message length or invalid format
    /// @retval kResponseTooLong: Response too long
    /// @retval kBusyRepeatRequest: Busy, repeat request
    /// @retval kConditionsNotCorrect: Conditions not correct
    /// @retval kRequestSequenceError: Request sequence error
    /// @retval kNoResponseFromSubnetComponent: No response from subnet component
    /// @retval kFailurePreventsExecutionOfRequestedAction: Failure prevents execution of requested action
    /// @retval kRequestOutOfRange: Request out of range
    /// @retval kSecurityAccessDenied: Security access denied
    /// @retval kInvalidKey: Invalid key
    /// @retval kExceedNumberOfAttempts: Exceed number of attempts
    /// @retval kRequiredTimeDelayNotExpired: Required time delay not expired
    /// @retval kUploadDownloadNotAccepted: Upload download not accepted
    /// @retval kTransferDataSuspended: Transfer data suspended
    /// @retval kGeneralProgrammingFailure: General programming failure
    /// @retval kWrongBlockSequenceCounter: Wrong block sequence counter
    /// @retval kSubFunctionNotSupportedInActiveSession: Subfunction not supported in active session
    /// @retval kServiceNotSupportedInActiveSession: Service not supported in active session
    /// @retval kRpmTooHigh: RPM too high
    /// @retval kRpmTooLow: RPM too low
    /// @retval kEngineIsRunning: Engine is running
    /// @retval kEngineIsNotRunning: Engine is not running
    /// @retval kEngineRunTimeTooLow: Engine run time too low
    /// @retval kTemperatureTooHigh: Temperature too high
    /// @retval kTemperatureTooLow: Temperature too low
    /// @retval kVehicleSpeedTooHigh: Vehicle speed too high
    /// @retval kVehicleSpeedTooLow: Vehicle speed too low
    /// @retval kThrottlePedalTooHigh: Throttle pedal too high
    /// @retval kThrottlePedalTooLow: Throttle pedal too low
    /// @retval kTransmissionRangeNotInNeutral: Transmission range not in neutral
    /// @retval kTransmissionRangeNotInGear: Transmission range not in gear
    /// @retval kBrakeSwitchNotClosed: Brake switch not closed
    /// @retval kShifterLeverNotInPark: Shifter lever not in park
    /// @retval kTorqueConverterClutchLocked: Torque converter clutch locked
    /// @retval kVoltageTooHigh: Voltage too high
    /// @retval kVoltageTooLow: Voltage too low
    ///
    /// @trace_id_sws={SWS_DM_00555}@tracestatus{draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00887
    /// @trace_id_dd=DD_UDS_01009
    /// @needwork = ad
    /// @endcode
    virtual Result< OperationOutput > Stop(std::uint16_t routineId,
                                           std::vector< std::uint8_t > requestData,
                                           MetaInfoMap& metaInfo,
                                           CancellationHandler cancellationHandler)
        = 0;

    /// @brief Request routine result
    /// @param[in] routineId Corresponding routine identifier
    /// @param[in] requestData Request message content (excluding RoutineIdentifier)
    /// @param[in] metaInfo Contains additional meta information
    /// @param[in] cancellationHandler Notifies whether the current session is cancelled
    /// @return Returns Result, contains OperationOutput on success (for positive response message), contains UDS NRC value on failure (for negative response message)
    /// Possible error codes:
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @retval kGeneralReject: General reject
    /// @retval kServiceNotSupported: Service not supported
    /// @retval kSubfunctionNotSupported: Subfunction not supported
    /// @retval kIncorrectMessageLengthOrInvalidFormat: Incorrect message length or invalid format
    /// @retval kResponseTooLong: Response too long
    /// @retval kBusyRepeatRequest: Busy, repeat request
    /// @retval kConditionsNotCorrect: Conditions not correct
    /// @retval kRequestSequenceError: Request sequence error
    /// @retval kNoResponseFromSubnetComponent: No response from subnet component
    /// @retval kFailurePreventsExecutionOfRequestedAction: Failure prevents execution of requested action
    /// @retval kRequestOutOfRange: Request out of range
    /// @retval kSecurityAccessDenied: Security access denied
    /// @retval kInvalidKey: Invalid key
    /// @retval kExceedNumberOfAttempts: Exceed number of attempts
    /// @retval kRequiredTimeDelayNotExpired: Required time delay not expired
    /// @retval kUploadDownloadNotAccepted: Upload download not accepted
    /// @retval kTransferDataSuspended: Transfer data suspended
    /// @retval kGeneralProgrammingFailure: General programming failure
    /// @retval kWrongBlockSequenceCounter: Wrong block sequence counter
    /// @retval kSubFunctionNotSupportedInActiveSession: Subfunction not supported in active session
    /// @retval kServiceNotSupportedInActiveSession: Service not supported in active session
    /// @retval kRpmTooHigh: RPM too high
    /// @retval kRpmTooLow: RPM too low
    /// @retval kEngineIsRunning: Engine is running
    /// @retval kEngineIsNotRunning: Engine is not running
    /// @retval kEngineRunTimeTooLow: Engine run time too low
    /// @retval kTemperatureTooHigh: Temperature too high
    /// @retval kTemperatureTooLow: Temperature too low
    /// @retval kVehicleSpeedTooHigh: Vehicle speed too high
    /// @retval kVehicleSpeedTooLow: Vehicle speed too low
    /// @retval kThrottlePedalTooHigh: Throttle pedal too high
    /// @retval kThrottlePedalTooLow: Throttle pedal too low
    /// @retval kTransmissionRangeNotInNeutral: Transmission range not in neutral
    /// @retval kTransmissionRangeNotInGear: Transmission range not in gear
    /// @retval kBrakeSwitchNotClosed: Brake switch not closed
    /// @retval kShifterLeverNotInPark: Shifter lever not in park
    /// @retval kTorqueConverterClutchLocked: Torque converter clutch locked
    /// @retval kVoltageTooHigh: Voltage too high
    /// @retval kVoltageTooLow: Voltage too low
    ///
    /// @trace_id_sws={SWS_DM_00556}@tracestatus{draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01010
    /// @needwork = ad
    /// @endcode
    virtual Result< OperationOutput > RequestResults(std::uint16_t routineId,
                                                     std::vector< std::uint8_t > requestData,
                                                     MetaInfoMap& metaInfo,
                                                     CancellationHandler cancellationHandler)
        = 0;
};

}  // namespace server
}  // namespace uds
}  // namespace isoft

#endif  // ARA_DIAG_GENERIC_ROUTINE_H_