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
/// @brief
/// @details
/// @date       2024-12-12
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================

#ifndef ADAPTIVEDIAGNOSTICMANAGER_SRC_UDSADAPTER_GENERICUDSSERVICE_H_
#define ADAPTIVEDIAGNOSTICMANAGER_SRC_UDSADAPTER_GENERICUDSSERVICE_H_
#include <isoft/uds/custom_management/generic_uds_service.h>

#include "cancellation/cancellation_event_manager.h"
#include "gen_code/generic_uds_service/serviceAgent/generic_uds_service_agent.h"
namespace ara {
namespace diag {
namespace dmd {

class GenericUDSService : public isoft::uds::server::GenericUDSServiceInterface
{
public:
    using MetaInfoMap                      = isoft::uds::server::MetaInfoMap;
    using CancellationHandler              = isoft::uds::server::CancellationHandler;
    using OperationOutput                  = isoft::uds::server::GenericUDSServiceInterface::OperationOutput;
    GenericUDSService()                    = default;
    ~GenericUDSService() noexcept override = default;

    /// @brief copy constructor
    /// @param other
    GenericUDSService(GenericUDSService const &other) noexcept = delete;
    /// @brief copy assignment operator
    /// @param other
    /// @return GenericUDSService&
    GenericUDSService &operator=(GenericUDSService const &other) noexcept = delete;
    /// @brief move constructor
    /// @param other
    GenericUDSService(GenericUDSService &&other) noexcept = delete;
    /// @brief move assignment operator
    /// @param other
    /// @return GenericUDSService&
    GenericUDSService &operator=(GenericUDSService &&other) noexcept = delete;

    void RegisterUdsServiceInstance(std::uint8_t sid,
                                    uint16_t const &instanceId,
                                    uint32_t const &serviceInstanceId) noexcept;

    /// @brief Called for any request messsage.
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[in] sid Diagnostic Request Service Identifier.
    /// @param[in] requestData Diagnostic request data (starting after SID).
    /// @param[in] metaInfo MetaInfo of the request.
    /// @param[in] cancellationHandler Set if the current conversation is
    /// canceled.
    /// @return a Result with either a OperationOutput or an error
    /// error code:
    ///> kGeneralReject, kServiceNotSupported, kSubfunctionNotSupported,
    /// kIncorrectMessageLengthOrInvalidFormat, > kResponseTooLong,
    /// kBusyRepeatRequest, kConditionsNotCorrect, kRequestSequenceError, >
    /// kNoResponseFromSubnetComponent,
    /// kFailurePreventsExecutionOfRequestedAction, kRequestOutOfRange, >
    /// kSecurityAccessDenied, kInvalidKey, kExceedNumberOfAttempts,
    /// kRequiredTimeDelayNotExpired,
    /// kUploadDownloadNotAccepted, kTransferDataSuspended,
    /// kGeneralProgrammingFailure, kWrongBlockSequenceCounter
    /// kSubFunctionNotSupportedInActiveSession,
    /// kServiceNotSupportedInActiveSession, kRpmTooHigh, kRpmTooLow
    ///> kEngineIsRunning, kEngineIsNotRunning, kEngineRunTimeTooLow,
    /// kTemperatureTooHigh, kTemperatureTooLow, > kVehicleSpeedTooHigh,
    /// kVehicleSpeedTooLow, kThrottlePedalTooHigh, kThrottlePedalTooLow,
    /// kTransmissionRangeNotInNeutral, kTransmissionRangeNotInGear,
    /// kBrakeSwitchNotClosed, kShifterLeverNotInPark
    /// kTorqueConverterClutchLocked, kVoltageTooHigh, kVoltageTooLow,
    /// kResourceTemporarilyNotAvailable
    ///
    ///
    /// @traceid{SWS_DM_00618}@tracestatus{draft}
    isoft::uds::Result< OperationOutput > HandleMessage(std::uint8_t sid,
                                                        std::vector< std::uint8_t > requestData,
                                                        MetaInfoMap &metaInfo,
                                                        CancellationHandler cancellationHandler) noexcept override;

private:
private:
    struct ComInfo
    {
        uint16_t instanceId{};
        uint32_t serviceInstanceId{};
        bool operator<(const ComInfo &other) const
        {
            if (instanceId < other.instanceId) {
                return true;
            }
            if (instanceId == other.instanceId && serviceInstanceId < other.serviceInstanceId) {
                return true;
            }
            return false;
        }
    };
    using GenericUdsServiceAgentPtr = std::shared_ptr< isoft::dm::dic::GenericUdsServiceAgent >;

    GenericUdsServiceAgentPtr _getInstance(std::uint8_t sid) noexcept;
    std::map< std::uint8_t, ComInfo > sidToComInfo_{};
    std::map< ComInfo, GenericUdsServiceAgentPtr > comInfoToInstance_{};
    isoft::dm::CancellationEventManager cancellationEventManager_{};
};
}  // namespace dmd
}  // namespace diag
}  // namespace ara

#endif  /// ADAPTIVEDIAGNOSTICMANAGER_SRC_UDSADAPTER_GENERICUDSSERVICE_H_