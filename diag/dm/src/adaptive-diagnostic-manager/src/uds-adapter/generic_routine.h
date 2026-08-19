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
/// @brief
/// @details
/// @date       2021-11-23
/// @author     jiawei
/// @version    1.2.0
///
/// ================================================================

#ifndef ADAPTIVEDIAGNOSTICMANAGER_SRC_UDSADAPTER_GENERICROUTINE_H_
#define ADAPTIVEDIAGNOSTICMANAGER_SRC_UDSADAPTER_GENERICROUTINE_H_
#include <isoft/uds/routine_management/generic_routine.h>

#include "cancellation/cancellation_event_manager.h"
#include "gen_code/generic_routine/serviceAgent/generic_routine_agent.h"
#include "gen_code/raw_routine/serviceAgent/raw_routine_agent.h"
namespace ara {
namespace diag {
namespace dmd {

class GenericRoutine : public isoft::uds::server::GenericRoutineInterface
{
public:
    using MetaInfoMap          = isoft::uds::server::MetaInfoMap;
    using CancellationHandler  = isoft::uds::server::CancellationHandler;
    using OperationOutput      = isoft::uds::server::GenericRoutineInterface::OperationOutput;
    GenericRoutine()           = default;
    ~GenericRoutine() override = default;

    /// @brief copy constructor
    /// @param other
    GenericRoutine(GenericRoutine const &other) noexcept = delete;
    /// @brief copy assignment operator
    /// @param other
    /// @return GenericRoutine&
    GenericRoutine &operator=(GenericRoutine const &other) noexcept = delete;
    /// @brief move constructor
    /// @param other
    GenericRoutine(GenericRoutine &&other) noexcept = delete;
    /// @brief move assignment operator
    /// @param other
    /// @return GenericRoutine&
    GenericRoutine &operator=(GenericRoutine &&other) noexcept = delete;

    void RegisterRoutineInstance(std::uint16_t routineId,
                                 uint16_t const &instanceId,
                                 uint32_t const &serviceInstanceId,
                                 ara::core::String &className) noexcept;

    /// @brief Called for RoutineControl with SubFunction Start request for this
    /// DiagnosticRoutineIdentifier.
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[in] routineId the corresponding RoutineIdentifier
    /// @param[in] requestData Content of request message (without
    /// RoutineIdentifier)
    /// @param[in] metaInfo contains additional meta information
    /// @param[in] cancellationHandler informs if the current conversation is
    /// canceled
    /// @return a Result with either OperationOutput (for a positive response
    /// message) or an UDS NRC value (for an negative response message) error
    /// code:
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
    /// kEngineIsRunning, kEngineIsNotRunning, kEngineRunTimeTooLow,
    /// kTemperatureTooHigh, kTemperatureTooLow
    ///> kVehicleSpeedTooHigh, kVehicleSpeedTooLow, kThrottlePedalTooHigh,
    /// kThrottlePedalTooLow,
    /// kTransmissionRangeNotInNeutral, kTransmissionRangeNotInGear,
    /// kBrakeSwitchNotClosed, kShifterLeverNotInPark
    /// kTorqueConverterClutchLocked, kVoltageTooHigh, kVoltageTooLow
    ///
    ///
    /// @traceid{SWS_DM_00554}@tracestatus{draft}
    isoft::uds::Result< OperationOutput > Start(std::uint16_t routineId,
                                                std::vector< std::uint8_t > requestData,
                                                MetaInfoMap &metaInfo,
                                                CancellationHandler cancellationHandler) override;

    /// @brief Called for RoutineControl with SubFunction Stop request for this
    /// DiagnosticRoutineIdentifier.
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[in] routineId the corresponding RoutineIdentifier
    /// @param[in] requestData Content of request message (without
    /// RoutineIdentifier)
    /// @param[in] metaInfo contains additional meta information
    /// @param[in] cancellationHandler informs if the current conversation is
    /// canceled
    /// @return a Result with either OperationOutput (for a positive response
    /// message) or an UDS NRC value (for an negative response message) error
    /// code: kGeneralReject, kServiceNotSupported, kSubfunctionNotSupported,
    /// kIncorrectMessageLengthOrInvalidFormat
    ///> kResponseTooLong, kBusyRepeatRequest, kConditionsNotCorrect,
    /// kRequestSequenceError, > kNoResponseFromSubnetComponent,
    /// kFailurePreventsExecutionOfRequestedAction, kRequestOutOfRange, >
    /// kSecurityAccessDenied, kInvalidKey, kExceedNumberOfAttempts,
    /// kRequiredTimeDelayNotExpired, > kUploadDownloadNotAccepted,
    /// kTransferDataSuspended, kGeneralProgrammingFailure,
    /// kWrongBlockSequenceCounter, > kSubFunctionNotSupportedInActiveSession,
    /// kServiceNotSupportedInActiveSession, kRpmTooHigh, kRpmTooLow, >
    /// kEngineIsRunning, kEngineIsNotRunning, kEngineRunTimeTooLow,
    /// kTemperatureTooHigh, kTemperatureTooLow, > kVehicleSpeedTooHigh,
    /// kVehicleSpeedTooLow, kThrottlePedalTooHigh, kThrottlePedalTooLow,
    /// kTransmissionRangeNotInNeutral, kTransmissionRangeNotInGear,
    /// kBrakeSwitchNotClosed, kShifterLeverNotInPark
    /// kTorqueConverterClutchLocked, kVoltageTooHigh, kVoltageTooLow
    /// @throws on overflow
    ///
    /// @traceid{SWS_DM_00555}@tracestatus{draft}
    isoft::uds::Result< OperationOutput > Stop(std::uint16_t routineId,
                                               std::vector< std::uint8_t > requestData,
                                               MetaInfoMap &metaInfo,
                                               CancellationHandler cancellationHandler) override;

    /// @brief Called for RoutineControl with SubFunction RequestResults request
    /// for this DiagnosticRoutineIdentifier.
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[in] routineId the corresponding RoutineIdentifier
    /// @param[in] requestData Content of request message (without
    /// RoutineIdentifier)
    /// @param[in] metaInfo contains additional meta information
    /// @param[in] cancellationHandler informs if the current conversation is
    /// canceled
    /// @return a Result with either OperationOutput (for a positive response
    /// message) or an UDS NRC value (for an negative response message) error
    /// code:
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
    /// kEngineIsRunning, kEngineIsNotRunning, kEngineRunTimeTooLow,
    /// kTemperatureTooHigh, kTemperatureTooLow
    ///> kVehicleSpeedTooHigh, kVehicleSpeedTooLow, kThrottlePedalTooHigh,
    /// kThrottlePedalTooLow,
    /// kTransmissionRangeNotInNeutral, kTransmissionRangeNotInGear,
    /// kBrakeSwitchNotClosed, kShifterLeverNotInPark
    /// kTorqueConverterClutchLocked, kVoltageTooHigh, kVoltageTooLow
    /// @throws on overflow
    ///
    /// @traceid{SWS_DM_00556}@tracestatus{draft}
    isoft::uds::Result< OperationOutput > RequestResults(std::uint16_t routineId,
                                                         std::vector< std::uint8_t > requestData,
                                                         MetaInfoMap &metaInfo,
                                                         CancellationHandler cancellationHandler) override;

private:
    enum InterfaceType
    {
        kGenericRoutine,
        kRawRoutine
    };
    struct ComInfo
    {
        uint16_t instanceId{};
        uint32_t serviceInstanceId{};
        InterfaceType type;
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
    using GenericRoutineAgentPtr = std::shared_ptr< isoft::dm::dic::GenericRoutineAgent >;
    using RawRoutineAgentPtr     = std::shared_ptr< isoft::dm::dic::RawRoutineAgent >;

    ComInfo _getComInfo(std::uint16_t did) noexcept;
    GenericRoutineAgentPtr _getGenericInstance(ComInfo routineInfo) noexcept;
    RawRoutineAgentPtr _getRawInstance(ComInfo routineInfo) noexcept;
    std::map< std::uint16_t, ComInfo > routineToComInfo_{};
    std::map< ComInfo, GenericRoutineAgentPtr > comInfoToGenericInstance_{};
    std::map< ComInfo, RawRoutineAgentPtr > comInfoToRawInstance_{};
    isoft::dm::CancellationEventManager cancellationEventManager_{};
};
}  // namespace dmd
}  // namespace diag

}  // namespace ara

#endif  /// ADAPTIVEDIAGNOSTICMANAGER_SRC_UDSADAPTER_GENERICROUTINE_H_