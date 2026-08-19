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
/// @brief      This file provides generic_data_identifier functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/isoft/uds/data_management
/// @interface_level=software
/// @trace_id_sr=a7bc1729-d534-440c-9e26-1138a94f7560
/// @unit_name=GenericDataIdentifier
/// @unit_description=This file provides generic_data_identifier functionality
/// @endcode
///
/// ================================================================

#ifndef ISOFT_UDS_SERVER_GENERICDATAIDENTIFIER_H_
#define ISOFT_UDS_SERVER_GENERICDATAIDENTIFIER_H_

#include <cstdint>
#include <list>
#include <memory>
#include <vector>

#include "../cancellation_handler.h"
#include "isoft/uds/common.h"
#include "isoft/uds/result.h"
namespace isoft {
namespace uds {
namespace server {

/// @brief Structure of diagnostic data
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01343
/// @trace_id_dd=DD_UDS_02635
/// @needwork = ad
/// @endcode
struct DiagnosticData
{
    /// @brief Diagnostic data ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01068
    /// @trace_id_dd=DD_UDS_02182
    /// @needwork = ad
    /// @endcode
    std::uint16_t id{0U};
    /// @brief Diagnostic data record
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01069
    /// @trace_id_dd=DD_UDS_02183
    /// @needwork = ad
    /// @endcode
    std::vector< std::uint8_t > record{};
};

/// @brief Generic data identifier interface
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01344
/// @trace_id_dd=DD_UDS_02636
/// @needwork = ad
/// @endcode
class GenericDataIdentifierInterface
{
public:
    /// @param
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_01000
    /// @needwork = dda
    /// @endcode
    GenericDataIdentifierInterface(GenericDataIdentifierInterface const&) = delete;
    /// @param
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_01001
    /// @needwork = dda
    /// @endcode
    GenericDataIdentifierInterface& operator=(GenericDataIdentifierInterface const&) = delete;
    /// @param
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_01002
    /// @needwork = dda
    /// @endcode
    GenericDataIdentifierInterface(GenericDataIdentifierInterface&&) = delete;
    /// @param
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_01003
    /// @needwork = dda
    /// @endcode
    GenericDataIdentifierInterface& operator=(GenericDataIdentifierInterface&&) = delete;

public:
    /// @brief Constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01070
    /// @trace_id_dd=DD_UDS_02184
    /// @needwork = ad
    /// @endcode
    explicit GenericDataIdentifierInterface() = default;

    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01071
    /// @trace_id_dd=DD_UDS_02185
    /// @needwork = ad
    /// @endcode
    virtual ~GenericDataIdentifierInterface() noexcept = default;

    /// @brief Read diagnostic data
    /// @param[in] dataIdentifierTable List of data identifiers
    /// @param[in] metaInfo Contains additional meta information
    /// @param[in] cancellationHandler Notifies whether the current session is cancelled
    /// @return Returns Result, contains list of diagnostic data on success, contains UDS NRC error code on failure
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=ad
    /// @retval kGeneralReject General reject
    /// @retval kServiceNotSupported Service not supported
    /// @retval kSubfunctionNotSupported Subfunction not supported
    /// @retval kIncorrectMessageLengthOrInvalidFormat Incorrect message length or invalid format
    /// @retval kResponseTooLong Response too long
    /// @retval kBusyRepeatRequest Busy, repeat request
    /// @retval kConditionsNotCorrect Conditions not correct
    /// @retval kRequestSequenceError Request sequence error
    /// @retval kNoResponseFromSubnetComponent No response from subnet component
    /// @retval kFailurePreventsExecutionOfRequestedAction Failure prevents execution of requested action
    /// @retval kRequestOutOfRange Request out of range
    /// @retval kSecurityAccessDenied Security access denied
    /// @retval kInvalidKey Invalid key
    /// @retval kExceedNumberOfAttempts Exceed number of attempts
    /// @retval kRequiredTimeDelayNotExpired Required time delay not expired
    /// @retval kUploadDownloadNotAccepted Upload download not accepted
    /// @retval kTransferDataSuspended Transfer data suspended
    /// @retval kGeneralProgrammingFailure General programming failure
    /// @retval kWrongBlockSequenceCounter Wrong block sequence counter
    /// @retval kSubFunctionNotSupportedInActiveSession Subfunction not supported in active session
    /// @retval kServiceNotSupportedInActiveSession Service not supported in active session
    /// @retval kRpmTooHigh RPM too high
    /// @retval kRpmTooLow RPM too low
    /// @retval kEngineIsRunning Engine is running
    /// @retval kEngineIsNotRunning Engine is not running
    /// @retval kEngineRunTimeTooLow Engine run time too low
    /// @retval kTemperatureTooHigh Temperature too high
    /// @retval kTemperatureTooLow Temperature too low
    /// @retval kVehicleSpeedTooHigh Vehicle speed too high
    /// @retval kVehicleSpeedTooLow Vehicle speed too low
    /// @retval kThrottlePedalTooHigh Throttle pedal too high
    /// @retval kThrottlePedalTooLow Throttle pedal too low
    /// @retval kTransmissionRangeNotInNeutral Transmission range not in neutral
    /// @retval kTransmissionRangeNotInGear Transmission range not in gear
    /// @retval kBrakeSwitchNotClosed Brake switch not closed
    /// @retval kShifterLeverNotInPark Shifter lever not in park
    /// @retval kTorqueConverterClutchLocked Torque converter clutch locked
    /// @retval kVoltageTooHigh Voltage too high
    /// @retval kVoltageTooLow Voltage too low
    /// @retval kResourceTemporarilyNotAvailable Resource temporarily not available
    ///
    /// @endcode
    virtual Result< std::list< DiagnosticData > > Read(std::vector< std::uint16_t >& dataIdentifierTable,
                                                       MetaInfoMap const& metaInfo,
                                                       CancellationHandler cancellationHandler)
        = 0;

    /// @brief Write diagnostic data
    /// @param[in] requestDataRecord Diagnostic data record (contains data ID and data content)
    /// @param[in] metaInfo Contains additional meta information
    /// @param[in] cancellationHandler Notifies whether the current session is cancelled
    /// @return Returns Result, returns void on success, contains UDS NRC error code on failure
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=ad
    /// @retval kGeneralReject General reject
    /// @retval kServiceNotSupported Service not supported
    /// @retval kSubfunctionNotSupported Subfunction not supported
    /// @retval kIncorrectMessageLengthOrInvalidFormat Incorrect message length or invalid format
    /// @retval kResponseTooLong Response too long
    /// @retval kBusyRepeatRequest Busy, repeat request
    /// @retval kConditionsNotCorrect Conditions not correct
    /// @retval kRequestSequenceError Request sequence error
    /// @retval kNoResponseFromSubnetComponent No response from subnet component
    /// @retval kFailurePreventsExecutionOfRequestedAction Failure prevents execution of requested action
    /// @retval kRequestOutOfRange Request out of range
    /// @retval kSecurityAccessDenied Security access denied
    /// @retval kInvalidKey Invalid key
    /// @retval kExceedNumberOfAttempts Exceed number of attempts
    /// @retval kRequiredTimeDelayNotExpired Required time delay not expired
    /// @retval kUploadDownloadNotAccepted Upload download not accepted
    /// @retval kTransferDataSuspended Transfer data suspended
    /// @retval kGeneralProgrammingFailure General programming failure
    /// @retval kWrongBlockSequenceCounter Wrong block sequence counter
    /// @retval kSubFunctionNotSupportedInActiveSession Subfunction not supported in active session
    /// @retval kServiceNotSupportedInActiveSession Service not supported in active session
    /// @retval kRpmTooHigh RPM too high
    /// @retval kRpmTooLow RPM too low
    /// @retval kEngineIsRunning Engine is running
    /// @retval kEngineIsNotRunning Engine is not running
    /// @retval kEngineRunTimeTooLow Engine run time too low
    /// @retval kTemperatureTooHigh Temperature too high
    /// @retval kTemperatureTooLow Temperature too low
    /// @retval kVehicleSpeedTooHigh Vehicle speed too high
    /// @retval kVehicleSpeedTooLow Vehicle speed too low
    /// @retval kThrottlePedalTooHigh Throttle pedal too high
    /// @retval kThrottlePedalTooLow Throttle pedal too low
    /// @retval kTransmissionRangeNotInNeutral Transmission range not in neutral
    /// @retval kTransmissionRangeNotInGear Transmission range not in gear
    /// @retval kBrakeSwitchNotClosed Brake switch not closed
    /// @retval kShifterLeverNotInPark Shifter lever not in park
    /// @retval kTorqueConverterClutchLocked Torque converter clutch locked
    /// @retval kVoltageTooHigh Voltage too high
    /// @retval kVoltageTooLow Voltage too low
    /// @retval kResourceTemporarilyNotAvailable Resource temporarily not available
    ///
    /// @endcode
    virtual Result< void > Write(DiagnosticData reuqestDataRecord,
                                 MetaInfoMap const& metaInfo,
                                 CancellationHandler cancellationHandler)
        = 0;
};
}  // namespace server
}  // namespace uds
}  // namespace isoft

#endif  // ISOFT_UDS_SERVER_GENERICDATAIDENTIFIER_H_