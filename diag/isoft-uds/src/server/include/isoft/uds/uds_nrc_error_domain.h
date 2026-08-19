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
/// @file       uds_nrc_error_domain.h
/// @brief      This file provides uds_nrc_error_domain functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/isoft/uds
/// @interface_level=component
/// @trace_id_sr=
/// @unit_name=UdsNrcErrorDomain
/// @unit_description=This file provides uds_nrc_error_domain functionality
/// @endcode
///
/// ================================================================

#ifndef ISOFT_UDS_SERVER_ERROR_DOMAIN_DIAGUDSNRC_H_
#define ISOFT_UDS_SERVER_ERROR_DOMAIN_DIAGUDSNRC_H_

#include <cstdint>

// #include <ara/core/error_domain.h>
// #include <ara/core/exception.h>

namespace isoft {

namespace uds {
namespace server {
/// @code{.isoft}
///
/// export_level=/Diagnostics
///
/// @trace_id_sws={SWS_DM_00526}@tracestatus{draft}
/// @endcode

/// @brief NRC error code
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01317
/// @trace_id_dd=DD_UDS_02609
/// @needwork = ad
/// @endcode
enum class NrcErrc : std::uint8_t
{
    kSuccess                                                = 0x00,
    kGeneralReject                                          = 0x10,  ///< According to ISO.
    kServiceNotSupported                                    = 0x11,  ///< According to ISO.
    kSubfunctionNotSupported                                = 0x12,  ///< According to ISO.
    kIncorrectMessageLengthOrInvalidFormat                  = 0x13,  ///< According to ISO.
    kResponseTooLong                                        = 0x14,  ///< According to ISO.
    kBusyRepeatRequest                                      = 0x21,  ///< According to ISO.
    kConditionsNotCorrect                                   = 0x22,  ///< According to ISO.
    kRequestSequenceError                                   = 0x24,  ///< According to ISO.
    kNoResponseFromSubnetComponent                          = 0x25,  ///< According to ISO.
    kFailurePreventsExecutionOfRequestedAction              = 0x26,  ///< According to ISO.
    kRequestOutOfRange                                      = 0x31,  ///< According to ISO.
    kSecurityAccessDenied                                   = 0x33,  ///< According to ISO.
    kAuthenticationRequired                                 = 0x34,  ///< According to ISO.
    kInvalidKey                                             = 0x35,  ///< According to ISO.
    kExceedNumberOfAttempts                                 = 0x36,  ///< According to ISO.
    kRequiredTimeDelayNotExpired                            = 0x37,  ///< According to ISO.
    kCertificateVerificationFailedInvalidTimePeriod         = 0x50,  ///< According to ISO.
    kCertificateVerificationFailedInvalidSignature          = 0x51,  ///< According to ISO.
    kCertificateVerificationFailedInvalidChainOfTrust       = 0x52,  ///< According to ISO.
    kCertificateVerificationFailedInvalidType               = 0x53,  ///< According to ISO.
    kCertificateVerificationFailedInvalidFormat             = 0x54,  ///< According to ISO.
    kCertificateVerificationFailedInvalidContent            = 0x55,  ///< According to ISO.
    kCertificateVerificationFailedInvalidScope              = 0x56,  ///< According to ISO.
    kCertificateVerificationFailedInvalidCertificateRevoked = 0x57,  ///< According to ISO.
    kOwnershipVerificationFailed                            = 0x58,  ///< According to ISO.
    kChallengeCalculationFailed                             = 0x59,  ///< According to ISO.
    kSettingAccessRightsFailed                              = 0x5A,  ///< According to ISO.
    kSessionKeyCreationDerivationFailed                     = 0x5B,  ///< According to ISO.
    kConfigurationDataUsageFailed                           = 0x5C,  ///< According to ISO.
    kDeAuthenticationFailed                                 = 0x5D,  ///< According to ISO.
    kUploadDownloadNotAccepted                              = 0x70,  ///< According to ISO.
    kTransferDataSuspended                                  = 0x71,  ///< According to ISO.
    kGeneralProgrammingFailure                              = 0x72,  ///< According to ISO.
    kWrongBlockSequenceCounter                              = 0x73,  ///< According to ISO.
    kRequestCorrectlyReceivedResponsePending                = 0x78,  ///< According to ISO.
    kSubFunctionNotSupportedInActiveSession                 = 0x7E,  ///< According to ISO.
    kServiceNotSupportedInActiveSession                     = 0x7F,  ///< According to ISO.
    kRpmTooHigh                                             = 0x81,  ///< According to ISO.
    kRpmTooLow                                              = 0x82,  ///< According to ISO.
    kEngineIsRunning                                        = 0x83,  ///< According to ISO.
    kEngineIsNotRunning                                     = 0x84,  ///< According to ISO.
    kEngineRunTimeTooLow                                    = 0x85,  ///< According to ISO.
    kTemperatureTooHigh                                     = 0x86,  ///< According to ISO.
    kTemperatureTooLow                                      = 0x87,  ///< According to ISO.
    kVehicleSpeedTooHigh                                    = 0x88,  ///< According to ISO.
    kVehicleSpeedTooLow                                     = 0x89,  ///< According to ISO.
    kThrottlePedalTooHigh                                   = 0x8A,  ///< According to ISO.
    kThrottlePedalTooLow                                    = 0x8B,  ///< According to ISO.
    kTransmissionRangeNotInNeutral                          = 0x8C,  ///< According to ISO.
    kTransmissionRangeNotInGear                             = 0x8D,  ///< According to ISO.
    kBrakeSwitchNotClosed                                   = 0x8F,  ///< According to ISO.
    kShifterLeverNotInPark                                  = 0x90,  ///< According to ISO.
    kTorqueConverterClutchLocked                            = 0x91,  ///< According to ISO.
    kVoltageTooHigh                                         = 0x92,  ///< According to ISO.
    kVoltageTooLow                                          = 0x93,  ///< According to ISO.
    kResourceTemporarilyNotAvailable = 0x94,  ///< According to ISO 14229-1 Table A.1.14229-1 Table A.1.
    kNoProcessingNoResponse = 0xFF,  ///< Deviating from ISO - no further service processing and no response (silently
                                     ///< ignore request message).
};

/// @brief DiagReporting error code
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01318
/// @trace_id_dd=DD_UDS_02610
/// @needwork = ad
/// @endcode
enum class DiagReportingErrc : int32_t
{
    kAlreadyOffered        = 101,  ///< AlreadyOffered: The service is already offered
    kConfigurationMismatch = 102,  ///< ConfigurationMismatch: monitor configuration does not match dext
    kDebouncingConfigurationInconsistent
    = 103,                   ///< DebouncingConfigurationInconsistent: monitor debouncing configuration invalid
    kGenericError    = 107,  ///< GenericError: generic issue, e.g. connection to DM lost
    kInvalidArgument = 105,  ///< InvalidArgument: e.g. PreFailed with internal debouncing
    kNotOffered      = 106,  ///< NotOffered: Offer not called before reporting
    kReportIgnored   = 104,  ///< ReportIgnored: Enable Conditions disabled, OC not started, ...
};

/// @brief Diag error code
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01319
/// @trace_id_dd=DD_UDS_02611
/// @needwork = ad
/// @endcode
enum class DiagErrc : int32_t
{
    kAlreadyOffered        = 101,  ///< The service is already offered
    kConfigurationMismatch = 102,  ///< monitor configuration does not match dext
    kDebouncingConfigurationInconsistent
    = 103,  ///<  monitor debouncing configuration invalid, e.g. passed threshold larger than failed threshold...
    kReportIgnored   = 104,  ///<  Enable Conditions disabled, OC not started, ...
    kInvalidArgument = 105,  ///<  e.g. kPreFailed with internal debouncing
    kNotOffered      = 106,  ///<  Offer not called before reporting
    kGenericError    = 107,  ///<  generic issue, e.g. connection to DM lost
    kNoSuchDTC       = 108,  ///< No DTC available.
    kBusy            = 109,  ///< Interface is busy with processing.
    kFailed          = 110,  ///< Failed to process.
    kMemoryError     = 111,  ///< A memory error occurred during processing.
    kWrongDtc        = 112,  ///< A wrong DTC number was requested.
    kRejected        = 113,  ///< Requested operation was rejected due to StateManagements/machines internal state.
    kResetTypeNotSupported
    = 114,                 ///< The requested Diagnostic reset type is not supported by the Diagnostic Address instance.
    kRequestFailed = 115,  ///< Diagnostic request could not be performed successfully.
    kCustomResetTypeNotSupported  ///< The requested Diagnostic custom reset type is not supported by the Diagnostic
                                  ///< Address instance.
};


}  // namespace server
}  // namespace uds
}  // namespace isoft

#endif  // ISOFT_UDS_SERVER_ERROR_DOMAIN_DIAGUDSNRC_H_