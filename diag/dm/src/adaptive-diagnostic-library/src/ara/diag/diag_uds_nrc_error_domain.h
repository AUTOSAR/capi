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
/// @file       diag_uds_nrc_error_domain.h
/// @brief      This file provides the definitions of DiagUdsNrcErrorDomain and related types.
/// @details
/// @date       2021-11-23
/// @author     jiawei
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_DIAG_ERROR_DOMAIN_DIAGUDSNRC_H_
#define ARA_DIAG_ERROR_DOMAIN_DIAGUDSNRC_H_

#include <ara/core/error_domain.h>
#include <ara/core/exception.h>

#include <cstdint>

namespace ara {
namespace diag {

/// @brief Specifies the types of internal errors that can occur upon calling Offer or ReportMonitorAction.
///
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
///
/// @traceid{SWS_DM_00526}@tracestatus{draft}
enum class DiagUdsNrcErrc : int32_t
{
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

/// @brief Exception type thrown by Diag Uds Nrc classes.
///
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
///
/// @traceid{SWS_DM_00527}@tracestatus{draft}
class DiagUdsNrcException : public ara::core::Exception
{
public:
    /// @brief Construct a new DiagUdsNrcException from an ErrorCode.
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[in] err the ErrorCode
    ///
    ///
    /// @traceid{SWS_DM_00528}@tracestatus{draft}
    explicit DiagUdsNrcException(ara::core::ErrorCode err) noexcept : ara::core::Exception(err) {}
};

/// @brief Error domain for errors originating from several diagnostic classes.
///
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
///
/// @traceid{SWS_DM_00529}@tracestatus{draft}
class DiagUdsNrcErrorDomain final : public ara::core::ErrorDomain
{
    /// @brief kId
    static constexpr ara::core::ErrorDomain::IdType kId{0x8000000000000411};

public:
    /// @brief Alias for the error code value enumeration
    ///
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    ///
    /// @traceid{SWS_DM_00530}@tracestatus{draft}
    using Errc = DiagUdsNrcErrc;

    /// @brief Alias for the exception base class
    ///
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    ///
    /// @traceid{SWS_DM_00531}@tracestatus{draft}
    using Exception = DiagUdsNrcException;

    /// @brief Default constructor
    ///
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    ///
    /// @traceid{SWS_DM_00532}@tracestatus{draft}
    constexpr DiagUdsNrcErrorDomain() noexcept : ara::core::ErrorDomain(kId) {}

    /// @brief Return the "shortname" ApApplicationErrorDomain.SN of this error domain.
    ///
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @return const char*
    /// @throws on overflow
    /// @traceid{SWS_DM_00533}@tracestatus{draft}
    const char* Name() const noexcept override { return "DiagUdsNrcErrorDomain"; }

    /// @brief Translate an error code value into a text message.
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[in] errorCode  the error code value
    /// @returns the text message, never nullptr
    ///
    ///
    /// @traceid{SWS_DM_00534}@tracestatus{draft}
    const char* Message(ara::core::ErrorDomain::CodeType errorCode) const noexcept override
    {
        ara::core::StringView msg{};
        switch (static_cast< Errc >(errorCode)) {
            case Errc::kGeneralReject: {
                msg = ara::core::StringView("kGeneralRejec");
            } break;
            case Errc::kServiceNotSupported: {
                msg = ara::core::StringView("kServiceNotSupporte");
            } break;
            case Errc::kSubfunctionNotSupported: {
                msg = ara::core::StringView("kSubfunctionNotSupporte");
            } break;
            case Errc::kIncorrectMessageLengthOrInvalidFormat: {
                msg = ara::core::StringView("kIncorrectMessageLengthOrInvalidForma");
            } break;
            case Errc::kResponseTooLong: {
                msg = ara::core::StringView("kResponseTooLon");
            } break;
            case Errc::kBusyRepeatRequest: {
                msg = ara::core::StringView("kBusyRepeatReques");
            } break;
            case Errc::kConditionsNotCorrect: {
                msg = ara::core::StringView("kConditionsNotCorrec");
            } break;
            case Errc::kRequestSequenceError: {
                msg = ara::core::StringView("kRequestSequenceErro");
            } break;
            case Errc::kNoResponseFromSubnetComponent: {
                msg = ara::core::StringView("kNoResponseFromSubnetComponen");
            } break;
            case Errc::kFailurePreventsExecutionOfRequestedAction: {
                msg = ara::core::StringView("kFailurePreventsExecutionOfRequestedActio");
            } break;
            case Errc::kRequestOutOfRange: {
                msg = ara::core::StringView("kRequestOutOfRang");
            } break;
            case Errc::kSecurityAccessDenied: {
                msg = ara::core::StringView("kSecurityAccessDenie");
            } break;
            case Errc::kAuthenticationRequired: {
                msg = ara::core::StringView("kAuthenticationRequired");
            } break;
            case Errc::kInvalidKey: {
                msg = ara::core::StringView("kInvalidKe");
            } break;
            case Errc::kExceedNumberOfAttempts: {
                msg = ara::core::StringView("kExceedNumberOfAttempt");
            } break;
            case Errc::kRequiredTimeDelayNotExpired: {
                msg = ara::core::StringView("kRequiredTimeDelayNotExpire");
            } break;
            case Errc::kCertificateVerificationFailedInvalidTimePeriod: {
                msg = ara::core::StringView("kCertificateVerificationFailedInvalidTimePeriod");
            } break;
            case Errc::kCertificateVerificationFailedInvalidSignature: {
                msg = ara::core::StringView("kCertificateVerificationFailedInvalidSignature");
            } break;
            case Errc::kCertificateVerificationFailedInvalidChainOfTrust: {
                msg = ara::core::StringView("kCertificateVerificationFailedInvalidChainOfTrust");
            } break;
            case Errc::kCertificateVerificationFailedInvalidType: {
                msg = ara::core::StringView("kCertificateVerificationFailedInvalidType");
            } break;
            case Errc::kCertificateVerificationFailedInvalidFormat: {
                msg = ara::core::StringView("kCertificateVerificationFailedInvalidFormat");
            } break;
            case Errc::kCertificateVerificationFailedInvalidContent: {
                msg = ara::core::StringView("kCertificateVerificationFailedInvalidContent");
            } break;
            case Errc::kCertificateVerificationFailedInvalidScope: {
                msg = ara::core::StringView("kCertificateVerificationFailedInvalidScope");
            } break;
            case Errc::kCertificateVerificationFailedInvalidCertificateRevoked: {
                msg = ara::core::StringView("kCertificateVerificationFailedInvalidCertificateRevoked");
            } break;
            case Errc::kOwnershipVerificationFailed: {
                msg = ara::core::StringView("kOwnershipVerificationFailed");
            } break;
            case Errc::kChallengeCalculationFailed: {
                msg = ara::core::StringView("kChallengeCalculationFailed");
            } break;
            case Errc::kSettingAccessRightsFailed: {
                msg = ara::core::StringView("kSettingAccessRightsFailed");
            } break;
            case Errc::kSessionKeyCreationDerivationFailed: {
                msg = ara::core::StringView("kSessionKeyCreationDerivationFailed");
            } break;
            case Errc::kConfigurationDataUsageFailed: {
                msg = ara::core::StringView("kConfigurationDataUsageFailed");
            } break;
            case Errc::kDeAuthenticationFailed: {
                msg = ara::core::StringView("kDeAuthenticationFailed");
            } break;
            case Errc::kUploadDownloadNotAccepted: {
                msg = ara::core::StringView("kUploadDownloadNotAccepte");
            } break;
            case Errc::kTransferDataSuspended: {
                msg = ara::core::StringView("kTransferDataSuspende");
            } break;
            case Errc::kGeneralProgrammingFailure: {
                msg = ara::core::StringView("kGeneralProgrammingFailur");
            } break;
            case Errc::kWrongBlockSequenceCounter: {
                msg = ara::core::StringView("kWrongBlockSequenceCounte");
            } break;
            case Errc::kSubFunctionNotSupportedInActiveSession: {
                msg = ara::core::StringView("kSubFunctionNotSupportedInActiveSessio");
            } break;
            case Errc::kServiceNotSupportedInActiveSession: {
                msg = ara::core::StringView("kServiceNotSupportedInActiveSessio");
            } break;
            case Errc::kRpmTooHigh: {
                msg = ara::core::StringView("kRpmTooHig");
            } break;
            case Errc::kRpmTooLow: {
                msg = ara::core::StringView("kRpmTooLo");
            } break;
            case Errc::kEngineIsRunning: {
                msg = ara::core::StringView("kEngineIsRunnin");
            } break;
            case Errc::kEngineIsNotRunning: {
                msg = ara::core::StringView("kEngineIsNotRunnin");
            } break;
            case Errc::kEngineRunTimeTooLow: {
                msg = ara::core::StringView("kEngineRunTimeTooLo");
            } break;
            case Errc::kTemperatureTooHigh: {
                msg = ara::core::StringView("kTemperatureTooHig");
            } break;
            case Errc::kTemperatureTooLow: {
                msg = ara::core::StringView("kTemperatureTooLo");
            } break;
            case Errc::kVehicleSpeedTooHigh: {
                msg = ara::core::StringView("kVehicleSpeedTooHig");
            } break;
            case Errc::kVehicleSpeedTooLow: {
                msg = ara::core::StringView("kVehicleSpeedTooLo");
            } break;
            case Errc::kThrottlePedalTooHigh: {
                msg = ara::core::StringView("kThrottlePedalTooHig");
            } break;
            case Errc::kThrottlePedalTooLow: {
                msg = ara::core::StringView("kThrottlePedalTooLo");
            } break;
            case Errc::kTransmissionRangeNotInNeutral: {
                msg = ara::core::StringView("kTransmissionRangeNotInNeutra");
            } break;
            case Errc::kTransmissionRangeNotInGear: {
                msg = ara::core::StringView("kTransmissionRangeNotInGea");
            } break;
            case Errc::kBrakeSwitchNotClosed: {
                msg = ara::core::StringView("kBrakeSwitchNotClose");
            } break;
            case Errc::kShifterLeverNotInPark: {
                msg = ara::core::StringView("kShifterLeverNotInPar");
            } break;
            case Errc::kTorqueConverterClutchLocked: {
                msg = ara::core::StringView("kTorqueConverterClutchLocke");
            } break;
            case Errc::kVoltageTooHigh: {
                msg = ara::core::StringView("kVoltageTooHig");
            } break;
            case Errc::kVoltageTooLow: {
                msg = ara::core::StringView("kVoltageTooLo");
            } break;
            case Errc::kResourceTemporarilyNotAvailable: {
                msg = ara::core::StringView("kResourceTemporarilyNotAvailabl");
            } break;
            case Errc::kNoProcessingNoResponse: {
                msg = ara::core::StringView("kNoProcessingNoResponse");
            } break;
            default: {
                msg = ara::core::StringView("Unknow error");
            } break;
        }
        return msg.data();
    }

    /// @brief Throw the exception type corresponding to the given ErrorCode.
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[in] errorCode  the ErrorCode instance
    /// @throws on overflow
    ///
    /// @traceid{SWS_DM_00535}@tracestatus{draft}
    void ThrowAsException(ara::core::ErrorCode const& errorCode) const noexcept(false) override
    {
        ara::core::internal::ThrowOrTerminate< Exception >(errorCode);
    }
};

namespace api {
constexpr DiagUdsNrcErrorDomain kG_DiagUdsNrcErrorDomain;
}  // namespace api

/// @brief Obtain the reference to the single global DiagUdsNrcErrorDomain instance.
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
/// @return reference to the DiagUdsNrcErrorDomain instance
///
///
/// @traceid{SWS_DM_00536}@tracestatus{draft}
constexpr ara::core::ErrorDomain const& GetDiagUdsNrcDomain() noexcept { return api::kG_DiagUdsNrcErrorDomain; }

/// @brief Create a new ErrorCode for DiagUdsNrcErrorDomain with the given support data type and message.
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
/// @param[in] code  an enumeration value from diag_uds_nrc__errc
/// @param[in] data a vendor-defined supplementary value
/// @return the new ErrorCode instance
///
///
/// @traceid{SWS_DM_00537}@tracestatus{draft}
constexpr ara::core::ErrorCode MakeErrorCode(DiagUdsNrcErrc code, ara::core::ErrorDomain::SupportDataType data) noexcept
{
    return ara::core::ErrorCode(static_cast< ara::core::ErrorDomain::CodeType >(code), GetDiagUdsNrcDomain(), data);
}

}  // namespace diag
}  // namespace ara

#endif  // ARA_DIAG_DIAG_UDS_NRC_ERROR_DOMAIN_H_