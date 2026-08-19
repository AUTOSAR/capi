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
/// @file       generic_uds_service.h
/// @brief      This file provides generic_uds_service functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/isoft/uds/custom_management
/// @interface_level=Module
/// @trace_id_sr=e8fc29b8-4512-407a-8875-67af1ddccacd
/// @unit_name=GenericUdsService
/// @unit_description=This file provides generic_uds_service functionality
/// @endcode
///
/// ================================================================

#ifndef DM_UDS_GENERIC_UDS_SERVICE_H_
#define DM_UDS_GENERIC_UDS_SERVICE_H_

#include <cstdint>
#include <memory>
#include <vector>

#include "isoft/uds/cancellation_handler.h"
#include "isoft/uds/common.h"
#include "isoft/uds/result.h"

namespace isoft {
namespace uds {
namespace server {

/// @brief GenericUDSServiceInterface class
/// @code{.isoft}
/// @interface_level=unit
/// @needwork = ad
///
/// @trace_id_sws={SWS_DM_00602}@tracestatus{draft}
/// @endcode
class GenericUDSServiceInterface
{
public:
    /// @brief Constructor
    /// @param
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00950
    /// @needwork = dda
    /// @endcode
    GenericUDSServiceInterface(GenericUDSServiceInterface const&) = delete;
    /// @param
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00951
    /// @needwork = dda
    /// @endcode
    GenericUDSServiceInterface& operator=(GenericUDSServiceInterface const&) = delete;
    /// @param
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00952
    /// @needwork = dda
    /// @endcode
    GenericUDSServiceInterface(GenericUDSServiceInterface&&) = delete;
    /// @param
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00953
    /// @needwork = dda
    /// @endcode
    GenericUDSServiceInterface& operator=(GenericUDSServiceInterface&&) = delete;

public:
    /// @brief OperationOutput structure
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = ad
    ///
    /// @trace_id_sws={SWS_DM_00578}@tracestatus{draft}
    /// @endcode
    struct OperationOutput
    {
        /// @brief Operation output data vector
        /// @trace_id_sws={SWS_DM_00632}@tracestatus{draft}
        std::vector< std::uint8_t > responseData;
    };

    /// @brief Constructor
    /// @throws Throws on overflow
    ///
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = ad
    /// @trace_id_sws={SWS_DM_00616}@tracestatus{draft}
    /// @endcode
    explicit GenericUDSServiceInterface() = default;

    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = ad
    ///
    /// @trace_id_sws={SWS_DM_00584}@tracestatus{draft}
    /// @endcode
    virtual ~GenericUDSServiceInterface() noexcept = default;

    /// @brief Process message function
    /// @param[in] sid Diagnostic request service identifier.
    /// @param[in] requestData Diagnostic request data (starting after SID).
    /// @param[in] metaInfo Request meta information.
    /// @param[in] cancellationHandler Set if current session is cancelled.
    /// @return Returns a Result containing OperationOutput or error
    /// Error codes:
    /// kGeneralReject, kServiceNotSupported, kSubfunctionNotSupported, kIncorrectMessageLengthOrInvalidFormat,
    /// kResponseTooLong, kBusyRepeatRequest, kConditionsNotCorrect, kRequestSequenceError,
    /// kNoResponseFromSubnetComponent, kFailurePreventsExecutionOfRequestedAction, kRequestOutOfRange,
    /// kSecurityAccessDenied, kInvalidKey, kExceedNumberOfAttempts, kRequiredTimeDelayNotExpired,
    /// kUploadDownloadNotAccepted, kTransferDataSuspended, kGeneralProgrammingFailure, kWrongBlockSequenceCounter,
    /// kSubFunctionNotSupportedInActiveSession, kServiceNotSupportedInActiveSession, kRpmTooHigh, kRpmTooLow,
    /// kEngineIsRunning, kEngineIsNotRunning, kEngineRunTimeTooLow, kTemperatureTooHigh, kTemperatureTooLow,
    /// kVehicleSpeedTooHigh, kVehicleSpeedTooLow, kThrottlePedalTooHigh, kThrottlePedalTooLow,
    /// kTransmissionRangeNotInNeutral, kTransmissionRangeNotInGear, kBrakeSwitchNotClosed, kShifterLeverNotInPark,
    /// kTorqueConverterClutchLocked, kVoltageTooHigh, kVoltageTooLow, kResourceTemporarilyNotAvailable
    ///
    ///
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = ad
    /// @trace_id_sws={SWS_DM_00618}@tracestatus{draft}
    /// @endcode
    virtual Result< OperationOutput > HandleMessage(std::uint8_t sid,
                                                    std::vector< std::uint8_t > requestData,
                                                    MetaInfoMap& metaInfo,
                                                    CancellationHandler cancellationHandler) noexcept = 0;
};

}  // namespace server
}  // namespace uds
}  // namespace isoft

#endif  // DM_UDS_GENERIC_UDS_SERVICE_H_