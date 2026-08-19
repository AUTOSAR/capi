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
/// @brief
/// @details
/// @date       2021-11-23
/// @author     jiawei
/// @version    1.2.0
///
/// ================================================================

#ifndef ADAPTIVEDIAGNOSTICMANAGER_SRC_UDSADAPTER_GENERICDATAIDENTIFIER_H_
#define ADAPTIVEDIAGNOSTICMANAGER_SRC_UDSADAPTER_GENERICDATAIDENTIFIER_H_
#include <isoft/uds/data_management/generic_data_identifier.h>

#include "cancellation/cancellation_event_manager.h"
#include "gen_code/generic_data_identifier/serviceAgent/genericDataIdentifier_agent.h"
#include "gen_code/generic_uds_service/serviceAgent/generic_uds_service_agent.h"
#include "gen_code/raw_data_identifier/serviceAgent/raw_data_identifier_agent.h"
namespace ara {
namespace diag {
namespace dmd {

class GenericDataIdentifier : public isoft::uds::server::GenericDataIdentifierInterface
{
public:
    using MetaInfoMap                          = isoft::uds::server::MetaInfoMap;
    using CancellationHandler                  = isoft::uds::server::CancellationHandler;
    GenericDataIdentifier()                    = default;
    ~GenericDataIdentifier() noexcept override = default;

    /// @brief copy constructor
    /// @param other
    GenericDataIdentifier(GenericDataIdentifier const &other) noexcept = delete;
    /// @brief copy assignment operator
    /// @param other
    /// @return GenericDataIdentifier&
    GenericDataIdentifier &operator=(GenericDataIdentifier const &other) noexcept = delete;
    /// @brief move constructor
    /// @param other
    GenericDataIdentifier(GenericDataIdentifier &&other) noexcept = delete;
    /// @brief move assignment operator
    /// @param other
    /// @return GenericDataIdentifier&
    GenericDataIdentifier &operator=(GenericDataIdentifier &&other) noexcept = delete;

    void RegisterServiceInstance(std::uint16_t did,
                                 uint16_t const &instanceId,
                                 uint32_t const &serviceInstanceId,
                                 ara::core::String &serviceClass) noexcept;

    /// @brief Called for ReadDataByIdentifier request for this
    /// DiagnosticDataIdentifier.
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[in] dataIdentifier the corresponding DataIdentifier
    /// @param[in] metaInfo contains additional meta information
    /// @param[in] cancellationHandler informs if the current conversation is
    /// canceled
    /// @return a Result with either OperationOutput (for a positive response
    /// message) or an UDS NRC value (for an negative response message) error
    /// code:
    ///> kGeneralReject, kServiceNotSupported, kSubfunctionNotSupported,
    /// kIncorrectMessageLengthOrInvalidFormat,
    /// kResponseTooLong kBusyRepeatRequest, kConditionsNotCorrect,
    /// kRequestSequenceError, kNoResponseFromSubnetComponent
    /// kFailurePreventsExecutionOfRequestedAction, kRequestOutOfRange,
    /// kSecurityAccessDenied kInvalidKey, kExceedNumberOfAttempts,
    /// kRequiredTimeDelayNotExpired,
    ///> kUploadDownloadNotAccepted, kTransferDataSuspended
    /// kGeneralProgrammingFailure, kWrongBlockSequenceCounter, >
    /// kSubFunctionNotSupportedInActiveSession,
    /// kServiceNotSupportedInActiveSession, kRpmTooHigh, kRpmTooLow, >
    /// kEngineIsRunning, kEngineIsNotRunning, kEngineRunTimeTooLow
    /// kTemperatureTooHigh, kTemperatureTooLow, > kVehicleSpeedTooHigh,
    /// kVehicleSpeedTooLow, kThrottlePedalTooHigh, kThrottlePedalTooLow, >
    /// kTransmissionRangeNotInNeutral, kTransmissionRangeNotInGear,
    /// kBrakeSwitchNotClosed, kShifterLeverNotInPark,
    /// kTorqueConverterClutchLocked, kVoltageTooHigh, kVoltageTooLow,
    /// kResourceTemporarilyNotAvailable
    ///
    ///
    /// @traceid{SWS_DM_00636}@tracestatus{draft}
    isoft::uds::Result< std::list< isoft::uds::server::DiagnosticData > > Read(
        std::vector< std::uint16_t > &dataIdentifierTable,
        MetaInfoMap const &metaInfo,
        CancellationHandler cancellationHandler) override;

    /// @brief Called for WriteDataByIdentifier request for this
    /// DiagnosticDataIdentifier.
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[in] dataIdentifier the corresponding DataIdentifier
    /// @param[in] requestData Content of request message (without DataIdentifier)
    /// @param[in] metaInfo contains additional meta information
    /// @param[in] cancellationHandler informs if the current conversation is
    /// canceled
    /// @return a Result with either void (for a positive response message) or an
    /// UDS NRC value (for an negative response message) error code:
    /// kGeneralReject, kServiceNotSupported, kSubfunctionNotSupported,
    /// kIncorrectMessageLengthOrInvalidFormat kResponseTooLong,
    /// kBusyRepeatRequest, kConditionsNotCorrect, kRequestSequenceError
    /// kNoResponseFromSubnetComponent,
    /// kFailurePreventsExecutionOfRequestedAction, kRequestOutOfRange
    ///> kSecurityAccessDenied, kInvalidKey, kExceedNumberOfAttempts,
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
    /// @traceid{SWS_DM_00637}@tracestatus{draft}
    isoft::uds::Result< void > Write(isoft::uds::server::DiagnosticData reuqestDataRecord,
                                     MetaInfoMap const &metaInfo,
                                     CancellationHandler cancellationHandler) override;

private:
    enum InterfaceType
    {
        kGenericDataIdentifier,
        kGenericUDSService,
        kRawDataIdentifier
    };
    struct ComInfo
    {
        uint16_t instanceId{};
        uint32_t serviceInstanceId{};
        InterfaceType type{};
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
    using GenericDataIdentifierAgentPtr = std::shared_ptr< isoft::dm::dic::GenericDataIdentifierAgent >;
    using GenericUdsServiceAgentPtr     = std::shared_ptr< isoft::dm::dic::GenericUdsServiceAgent >;
    using RawDataIdentifierAgentPtr     = std::shared_ptr< isoft::dm::dic::RawDataIdentifierAgent >;
    ComInfo _getComInfo(std::uint16_t did) noexcept;
    GenericDataIdentifierAgentPtr _getGenericDataIdentifierInstance(ComInfo info) noexcept;
    GenericUdsServiceAgentPtr _getGenericUdsServiceInstance(ComInfo info) noexcept;
    RawDataIdentifierAgentPtr _getRawDataIdentifierInstance(ComInfo info) noexcept;
    std::map< std::uint16_t, ComInfo > didToComInfo_{};
    std::map< ComInfo, GenericDataIdentifierAgentPtr > comInfoToGenericDataIdInstance_{};
    std::map< ComInfo, RawDataIdentifierAgentPtr > comInfoToRawDataIdInstance_{};
    std::map< ComInfo, GenericUdsServiceAgentPtr > comInfoToGenericUdsInstance_{};
    isoft::dm::CancellationEventManager cancellationEventManager_{};
};
}  // namespace dmd
}  // namespace diag
}  // namespace ara

#endif  /// ADAPTIVEDIAGNOSTICMANAGER_SRC_UDSADAPTER_GENERICDATAIDENTIFIER_H_