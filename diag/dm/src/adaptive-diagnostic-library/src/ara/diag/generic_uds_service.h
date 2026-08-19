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
/// @brief      This file provides the definitions of GenericUDSService and related types.
/// @details
/// @date       2021-11-23
/// @author     jiawei
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_DIAG_GENERIC_UDS_SERVICE_H_
#define ARA_DIAG_GENERIC_UDS_SERVICE_H_

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
/// @brief GenericUdsServiceAgent
class GenericUdsServiceAgent;
}  // namespace dis
}  // namespace dm
}  // namespace isoft

namespace ara {
namespace diag {

/// @brief Generic UDS interface
///
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
///
/// @traceid{SWS_DM_00602}@tracestatus{draft}
class GenericUDSService
{
public:
    /// @brief Response data of positive respone message
    ///
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    ///
    /// @traceid{SWS_DM_00578}@tracestatus{draft}
    struct OperationOutput
    {
        /// @brief Content of positive respone message (without SID)
        ///
        /// @traceid{SWS_DM_00632}@tracestatus{draft}
        ara::core::Vector< std::uint8_t > responseData;
    };

    /// @brief Constructor of GenericUDSService
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[in] specifier An InstanceSpecifier linking this instance with the PortPrototype in the manifest
    /// @param[in] reentrancyType specifies if interface is callable fully- or non-reentrant
    /// @throws on overflow
    ///
    /// @traceid{SWS_DM_00616}@tracestatus{draft}
    explicit GenericUDSService(ara::core::InstanceSpecifier const& specifier, ReentrancyType reentrancyType);

    /// @brief Destructor of GenericUDSService
    ///
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    ///
    /// @traceid{SWS_DM_00584}@tracestatus{draft}
    virtual ~GenericUDSService() noexcept = default;

    /// @brief copy constructor
    /// @param other
    GenericUDSService(GenericUDSService const& other) noexcept = default;
    /// @brief copy assignment operator
    /// @param other
    /// @return GenericUDSService&
    GenericUDSService& operator=(GenericUDSService const& other) noexcept = default;
    /// @brief move constructor
    /// @param other
    GenericUDSService(GenericUDSService&& other) noexcept = default;
    /// @brief move assignment operator
    /// @param other
    /// @return GenericUDSService&
    GenericUDSService& operator=(GenericUDSService&& other) noexcept = default;

    /// @brief Called for any request messsage.
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[in] sid Diagnostic Request Service Identifier.
    /// @param[in] requestData Diagnostic request data (starting after SID).
    /// @param[in] metaInfo MetaInfo of the request.
    /// @param[in] cancellationHandler Set if the current conversation is canceled.
    /// @return a Result with either a OperationOutput or an error
    /// error code:
    ///> kGeneralReject, kServiceNotSupported, kSubfunctionNotSupported, kIncorrectMessageLengthOrInvalidFormat,
    ///> kResponseTooLong, kBusyRepeatRequest, kConditionsNotCorrect, kRequestSequenceError,
    ///> kNoResponseFromSubnetComponent, kFailurePreventsExecutionOfRequestedAction, kRequestOutOfRange,
    ///> kSecurityAccessDenied, kInvalidKey, kExceedNumberOfAttempts, kRequiredTimeDelayNotExpired,
    /// kUploadDownloadNotAccepted, kTransferDataSuspended, kGeneralProgrammingFailure, kWrongBlockSequenceCounter
    /// kSubFunctionNotSupportedInActiveSession, kServiceNotSupportedInActiveSession, kRpmTooHigh, kRpmTooLow
    ///> kEngineIsRunning, kEngineIsNotRunning, kEngineRunTimeTooLow, kTemperatureTooHigh, kTemperatureTooLow,
    ///> kVehicleSpeedTooHigh, kVehicleSpeedTooLow, kThrottlePedalTooHigh, kThrottlePedalTooLow,
    /// kTransmissionRangeNotInNeutral, kTransmissionRangeNotInGear, kBrakeSwitchNotClosed, kShifterLeverNotInPark
    /// kTorqueConverterClutchLocked, kVoltageTooHigh, kVoltageTooLow, kResourceTemporarilyNotAvailable
    ///
    ///
    /// @traceid{SWS_DM_00618}@tracestatus{draft}
    virtual ara::core::Future< OperationOutput > HandleMessage(std::uint8_t sid,
                                                               ara::core::Span< std::uint8_t > requestData,
                                                               MetaInfo& metaInfo,
                                                               CancellationHandler cancellationHandler)
        = 0;

    /// @brief This Offer will enable the DM to forward request messages to this handler
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @return ara::core::Result<void> error code: kNotOffered, kGenericError, kAlreadyOffered
    /// @throws on overflow
    ///
    /// @traceid{SWS_DM_00619}@tracestatus{draft}
    ara::core::Result< void > Offer();

    /// @brief This StopOffer will disable the forwarding of request messages from DM
    ///
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @throws on overflow
    ///@traceid{SWS_DM_00620}@tracestatus{draft}
    void StopOffer();

private:
    /// @name specifier_
    ara::core::InstanceSpecifier specifier_;
    /// @name specifier_
    ReentrancyType reentrancyType_;
    std::shared_ptr< isoft::dm::dis::GenericUdsServiceAgent >
        /// @name skeleton_
        skeleton_{};
};
}  // namespace diag
}  // namespace ara

#endif  // ARA_DIAG_GENERIC_UDS_SERVICE_H_