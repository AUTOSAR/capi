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
/// @file       service_validation.h
/// @brief      This file provides service_validation functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=uds/isoft/uds
/// @interface_level=component
/// @trace_id_sr=0609df10-d49b-466d-8d06-0bb8aa07a582
/// @unit_name=ServiceValidation
/// @unit_description=This file provides service_validation functionality
/// @endcode
///
/// ================================================================

#ifndef ISOFT_DM_SERVICEVALIDATION_H_
#define ISOFT_DM_SERVICEVALIDATION_H_

#include <cstdint>
#include <memory>
#include <vector>

#include "common.h"
#include "isoft/uds/result.h"

namespace isoft {
namespace uds {
namespace server {

/// @code{.isoft}
///
/// export_level=/Diagnostics
///
/// @trace_id_sws={SWS_DM_00770}@tracestatus{draft}
/// @endcode

/// @brief Confirmation status type
/// @code{.isoft}
/// @interface_level=unit
/// @needwork=ad
///
/// @endcode
enum class ConfirmationStatusType : std::uint8_t
{
    kResPosOk               = 0x00,  ///< Positive response has been sent out successfully
    kResPosNotOk            = 0x01,  ///< Positive response has not been sent out successfully
    kResNegOk               = 0x02,  ///< Negative response has been sent out successfull
    kResNegNotOk            = 0x03,  ///< Negative response has not been sent out successfully
    kResPosSuppressed       = 0x04,  ///< Positive answer suppressed
    kResNegSuppressed       = 0x05,  ///< Negative answer suppressed
    kCanceled               = 0x06,  ///< Processing is canceled
    kNoProcessingNoResponse = 0x07,  ///< Processing rejected in Validation
};

/// @brief Service validation interface
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01294
/// @trace_id_dd=DD_UDS_02586
/// @needwork = ad
/// @endcode
class ServiceValidationInterface
{
public:
    /// @param
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00875
    /// @needwork = dda
    /// @endcode
    ServiceValidationInterface(ServiceValidationInterface const&) = delete;
    /// @param
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00876
    /// @needwork = dda
    /// @endcode
    ServiceValidationInterface& operator=(ServiceValidationInterface const&) = delete;
    /// @param
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00877
    /// @needwork = dda
    /// @endcode
    ServiceValidationInterface(ServiceValidationInterface&&) = delete;
    /// @param
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00878
    /// @needwork = dda
    /// @endcode
    ServiceValidationInterface& operator=(ServiceValidationInterface&&) = delete;

public:
    /// @brief Constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=ad
    ///
    /// @trace_id_sws={SWS_DM_00772}@tracestatus{draft}
    /// @endcode
    explicit ServiceValidationInterface() = default;

    /// @brief Destructor
    /// @code{.isoft}
    ///
    /// @interface_level=unit
    /// @needwork=ad
    ///
    /// @trace_id_sws={SWS_DM_00773}@tracestatus{draft}
    /// @endcode
    virtual ~ServiceValidationInterface() noexcept = default;

    /// @param[in] requestData Diagnostic request data (including SID).
    /// @param[in] metaInfo MetaInfo of the request.
    /// @returns Returns nothing or an error
    /// error code: kGeneralReject, kServiceNotSupported, kSubfunctionNotSupported,
    ///> kIncorrectMessageLengthOrInvalidFormat, kResponseTooLong, kBusyRepeatRequest, kConditionsNotCorrect,
    ///> kRequestSequenceError, kNoResponseFromSubnetComponent, kFailurePreventsExecutionOfRequestedAction,
    ///> kRequestOutOfRange, kSecurityAccessDenied, kInvalidKey, kExceedNumberOfAttempts, kRequiredTimeDelayNotExpired,
    ///> kUploadDownloadNotAccepted, kTransferDataSuspended, kGeneralProgrammingFailure, kWrongBlockSequenceCounter,
    ///> kSubFunctionNotSupportedInActiveSession, kServiceNotSupportedInActiveSession, kRpmTooHigh, kRpmTooLow,
    ///> kEngineIsRunning, kEngineIsNotRunning, kEngineRunTimeTooLow, kTemperatureTooHigh, kTemperatureTooLow,
    ///> kVehicleSpeedTooHigh, kVehicleSpeedTooLow, kThrottlePedalTooHigh, kThrottlePedalTooLow,
    ///> kTransmissionRangeNotInNeutral, kTransmissionRangeNotInGear, kBrakeSwitchNotClosed, kShifterLeverNotInPark,
    /// kTorqueConverterClutchLocked, kVoltageTooHigh, kVoltageTooLow, kNoProcessingNoResponse
    /// @throws on overflow
    ///
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @trace_id_sws={SWS_DM_00774}@tracestatus{draft}
    /// @endcode

    /// @brief Validation function
    /// @param[in] requestData Diagnostic request data (including SID).
    /// @param[in] metaInfo Request meta information.
    /// @returns Returns void or error
    /// @throws on overflow
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00911
    /// @trace_id_dd=DD_UDS_02025
    /// @needwork = ad
    /// @endcode

    /// @code{.isoft}
    ///
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00879
    /// @needwork = dda
    /// @endcode
    virtual Result< void > Validate(std::vector< std::uint8_t > requestData, MetaInfoMap& metaInfo) noexcept = 0;

    /// @brief Confirmation callback function
    /// @param[in] status Service processing status/result.
    /// @param[in] metaInfo Request meta information.
    /// @returns Returns void or error
    /// @throws Throws exception on overflow
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=ad
    /// @trace_id_sws={SWS_DM_00775}@tracestatus{draft}
    /// @endcode
    virtual Result< void > Confirmation(ConfirmationStatusType status, MetaInfoMap& metaInfo) noexcept = 0;
};

}  // namespace server
}  // namespace uds
}  // namespace isoft

#endif  // ISOFT_DM_SERVICEVALIDATION_H_