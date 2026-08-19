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
/// @brief      This file provides the definitions of ServiceValidation and related types.
/// @details
/// @date       2021-11-23
/// @author     jiawei
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_DIAG_SERVICE_VALIDATION_H_
#define ARA_DIAG_SERVICE_VALIDATION_H_

#include <ara/core/future.h>
#include <ara/core/instance_specifier.h>
#include <ara/core/result.h>
#include <ara/core/span.h>

#include <cstdint>
#include <memory>

#include "meta_info.h"

namespace isoft {
namespace dm {
namespace dis {
/// @brief ServiceValidationAgent
class ServiceValidationAgent;
}  // namespace dis
}  // namespace dm
}  // namespace isoft

namespace ara {
namespace diag {

/// @brief Represents the status of the service processing
///
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
///
/// @traceid{SWS_DM_00770}@tracestatus{draft}
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

/// @brief DiagnosticServiceValidationInterface
///
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
///
/// @traceid{SWS_DM_00771}@tracestatus{draft}
class ServiceValidation
{
public:
    /// @brief Constructor of ServiceValidation
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[in] specifier InstanceSpecifier to an PortPrototype of an DiagnosticServiceValidationInterface
    /// @throws on overflow
    ///
    /// @traceid{SWS_DM_00772}@tracestatus{draft}
    explicit ServiceValidation(ara::core::InstanceSpecifier const& specifier);

    /// @brief Destructor of ServiceValidation
    ///
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    ///
    /// @traceid{SWS_DM_00773}@tracestatus{draft}
    virtual ~ServiceValidation() noexcept = default;

    /// @brief copy constructor
    /// @param other
    ServiceValidation(ServiceValidation const& other) noexcept = default;
    /// @brief copy assignment operator
    /// @param other
    /// @return ServiceValidation&
    ServiceValidation& operator=(ServiceValidation const& other) noexcept = default;
    /// @brief move constructor
    /// @param other
    ServiceValidation(ServiceValidation&& other) noexcept = default;
    /// @brief move assignment operator
    /// @param other
    /// @return ServiceValidation&
    ServiceValidation& operator=(ServiceValidation&& other) noexcept = default;

    /// @brief Called for any request messsage.
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
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
    /// @traceid{SWS_DM_00774}@tracestatus{draft}
    virtual ara::core::Future< void > Validate(ara::core::Span< std::uint8_t > requestData, MetaInfo& metaInfo) = 0;

    /// @brief This method is called, when a diagnostic request has been finished, to notify about the outcome.
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[in] status status/outcome of the service processing.
    /// @param[in] metaInfo MetaInfo of the request.
    /// @returns Returns nothing or an error
    /// @throws on overflow
    ///
    /// @traceid{SWS_DM_00775}@tracestatus{draft}
    virtual ara::core::Future< void > Confirmation(ConfirmationStatusType status, MetaInfo& metaInfo) = 0;

    /// @brief This Offer will enable the DM to forward request messages to this handler
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @returns Returns nothing or an error: kNotOffered, kGenericError, kAlreadyOffered
    /// @throws on overflow
    ///
    /// @traceid{SWS_DM_00776}@tracestatus{draft}
    ara::core::Result< void > Offer();

    /// @brief This StopOffer will disable the forwarding of request messages from DM
    ///
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @throws on overflow
    /// @traceid{SWS_DM_00777}@tracestatus{draft}
    void StopOffer();

private:
    /// @name specifier_
    ara::core::InstanceSpecifier specifier_;
    std::shared_ptr< isoft::dm::dis::ServiceValidationAgent >
        /// @name skeleton_
        skeleton_{};
};
}  // namespace diag
}  // namespace ara

#endif  // ARA_DIAG_SERVICE_VALIDATION_H_