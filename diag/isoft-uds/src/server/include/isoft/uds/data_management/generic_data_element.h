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
/// @file       generic_data_element.h
/// @brief      This file provides generic_data_element functionality
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
/// @trace_id_sr=b91b6b64-f8d1-415c-887d-6959ea5d5800
/// @unit_name=GenericDataElement
/// @unit_description=This file provides generic_data_element functionality
/// @endcode
///
/// ================================================================

#ifndef ISOFT_UDS_SERVER_GENERICDATAELEMENT_H_
#define ISOFT_UDS_SERVER_GENERICDATAELEMENT_H_

#include <cstdint>
#include <list>
#include <memory>
#include <vector>

#include "../cancellation_handler.h"
#include "../result.h"
#include "isoft/uds/common.h"
namespace isoft {
namespace uds {
namespace server {

/// @brief Structure of diagnostic data element
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01341
/// @trace_id_dd=DD_UDS_02633
/// @needwork = ad
/// @endcode
struct DiagnosticDataElement
{
    /// @brief Diagnostic data element ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01066
    /// @trace_id_dd=DD_UDS_02180
    /// @needwork = ad
    /// @endcode
    std::uint32_t id{0U};
    /// @brief Diagnostic data element record
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01067
    /// @trace_id_dd=DD_UDS_02181
    /// @needwork = ad
    /// @endcode
    std::vector< std::uint8_t > record{};
};

/// @brief Generic data element interface, defines basic operations of generic data element
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01342
/// @trace_id_dd=DD_UDS_02634
/// @needwork = ad
/// @endcode
class GenericDataElementInterface
{
public:
    /// @param
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00996
    /// @needwork = dda
    /// @endcode
    GenericDataElementInterface(GenericDataElementInterface const&) = delete;
    /// @param
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00997
    /// @needwork = dda
    /// @endcode
    GenericDataElementInterface& operator=(GenericDataElementInterface const&) = delete;
    /// @param
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00998
    /// @needwork = dda
    /// @endcode
    GenericDataElementInterface(GenericDataElementInterface&&) = delete;
    /// @param
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00999
    /// @needwork = dda
    /// @endcode
    GenericDataElementInterface& operator=(GenericDataElementInterface&&) = delete;

public:
    /// @brief Constructor
    /// @throws on overflow
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_01039
    /// @needwork = dda
    /// @interface_level=unit
    /// @trace_id_sws={SWS_DM_00634}@tracestatus{draft}
    /// @endcode
    explicit GenericDataElementInterface() = default;

    /// @brief Destructor
    /// @throws on overflow
    ///
    /// @code{.isoft}
    /// @needwork=ad
    /// @interface_level=unit
    /// @endcode
    virtual ~GenericDataElementInterface() noexcept = default;

    /// @brief Read diagnostic data element
    /// @param[in] dataElementTable Data element table, containing list of data element IDs to read
    /// @param[in] metaInfo Contains additional meta information
    /// @param[in] cancellationHandler Used to notify whether the current conversation is cancelled
    /// @return Returns result, contains list of diagnostic data elements on success (positive response), contains UDS NRC error code on failure (negative response)
    /// @code{.isoft}
    /// @retval Result<std::list<DiagnosticDataElement>> Returns list of diagnostic data elements on success
    /// @retval UdsErrorCode Returns one of the following error codes on failure:
    /// - kGeneralReject: General reject
    /// - kServiceNotSupported: Service not supported
    /// - kSubfunctionNotSupported: Subfunction not supported
    /// - kIncorrectMessageLengthOrInvalidFormat: Incorrect message length or invalid format
    /// - kResponseTooLong: Response too long
    /// - kBusyRepeatRequest: Busy, please repeat request
    /// - kConditionsNotCorrect: Conditions not correct
    /// - kRequestSequenceError: Request sequence error
    /// - kNoResponseFromSubnetComponent: No response from subnet component
    /// - kFailurePreventsExecutionOfRequestedAction: Failure prevents execution of requested action
    /// - kRequestOutOfRange: Request out of range
    /// - kSecurityAccessDenied: Security access denied
    /// - kInvalidKey: Invalid key
    /// - kExceedNumberOfAttempts: Exceed number of attempts
    /// - kRequiredTimeDelayNotExpired: Required time delay not expired
    /// - kUploadDownloadNotAccepted: Upload/Download not accepted
    /// - kTransferDataSuspended: Transfer data suspended
    /// - kGeneralProgrammingFailure: General programming failure
    /// - kWrongBlockSequenceCounter: Wrong block sequence counter
    /// - kSubFunctionNotSupportedInActiveSession: Subfunction not supported in active session
    /// - kServiceNotSupportedInActiveSession: Service not supported in active session
    /// - kRpmTooHigh: RPM too high
    /// - kRpmTooLow: RPM too low
    /// - kEngineIsRunning: Engine is running
    /// - kEngineIsNotRunning: Engine is not running
    /// - kEngineRunTimeTooLow: Engine run time too low
    /// - kTemperatureTooHigh: Temperature too high
    /// - kTemperatureTooLow: Temperature too low
    /// - kVehicleSpeedTooHigh: Vehicle speed too high
    /// - kVehicleSpeedTooLow: Vehicle speed too low
    /// - kThrottlePedalTooHigh: Throttle pedal too high
    /// - kThrottlePedalTooLow: Throttle pedal too low
    /// - kTransmissionRangeNotInNeutral: Transmission range not in neutral
    /// - kTransmissionRangeNotInGear: Transmission range not in gear
    /// - kBrakeSwitchNotClosed: Brake switch not closed
    /// - kShifterLeverNotInPark: Shifter lever not in park
    /// - kTorqueConverterClutchLocked: Torque converter clutch locked
    /// - kVoltageTooHigh: Voltage too high
    /// - kVoltageTooLow: Voltage too low
    /// - kResourceTemporarilyNotAvailable: Resource temporarily not available
    /// @interface_level=unit
    /// @needwork=ad
    /// @trace_id_sws={SWS_DM_00636}@tracestatus{draft}
    /// @endcode
    virtual Result< std::list< DiagnosticDataElement > > Read(std::vector< std::uint32_t >& dataElementTable,
                                                              MetaInfoMap const& metaInfo,
                                                              CancellationHandler cancellationHandler)
        = 0;
};
}  // namespace server
}  // namespace uds
}  // namespace isoft

#endif  // ISOFT_UDS_SERVER_GENERICDATAELEMENT_H_