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
/// @brief
/// @details
/// @date       2024-12-28
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================

#ifndef ADAPTIVEDIAGNOSTICMANAGER_SRC_UDSADAPTER_GENERICDATAELEMENT_H_
#define ADAPTIVEDIAGNOSTICMANAGER_SRC_UDSADAPTER_GENERICDATAELEMENT_H_
#include <isoft/uds/data_management/generic_data_element.h>

#include "cancellation/cancellation_event_manager.h"
#include "gen_code/raw_data_element/serviceAgent/raw_data_element_agent.h"
namespace ara {
namespace diag {
namespace dmd {

class GenericDataElement : public isoft::uds::server::GenericDataElementInterface
{
public:
    using MetaInfoMap                       = isoft::uds::server::MetaInfoMap;
    using CancellationHandler               = isoft::uds::server::CancellationHandler;
    GenericDataElement()                    = default;
    ~GenericDataElement() noexcept override = default;
    /// @brief copy constructor
    /// @param other
    GenericDataElement(GenericDataElement const &other) noexcept = delete;
    /// @brief copy assignment operator
    /// @param other
    /// @return GenericDataElement&
    GenericDataElement &operator=(GenericDataElement const &other) noexcept = delete;
    /// @brief move constructor
    /// @param other
    GenericDataElement(GenericDataElement &&other) noexcept = delete;
    /// @brief move assignment operator
    /// @param other
    /// @return GenericDataElement&
    GenericDataElement &operator=(GenericDataElement &&other) noexcept = delete;

    void RegisterServiceInstance(std::uint32_t did,
                                 uint16_t const &instanceId,
                                 uint32_t const &serviceInstanceId) noexcept;

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
    isoft::uds::Result< std::list< isoft::uds::server::DiagnosticDataElement > > Read(
        std::vector< std::uint32_t > &dataElementTable,
        MetaInfoMap const &metaInfo,
        CancellationHandler cancellationHandler) override;

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
    using RawDataElementAgentPtr = std::shared_ptr< isoft::dm::dic::RawDataElementAgent >;

    ComInfo _getComInfo(std::uint32_t did) noexcept;
    RawDataElementAgentPtr _getGenericDataElementInstance(ComInfo info) noexcept;

    std::map< std::uint32_t, ComInfo > didToComInfo_{};
    std::map< ComInfo, RawDataElementAgentPtr > comInfoToRawDataIdInstance_{};

    isoft::dm::CancellationEventManager cancellationEventManager_{};
};
}  // namespace dmd
}  // namespace diag
}  // namespace ara

#endif  /// ADAPTIVEDIAGNOSTICMANAGER_SRC_UDSADAPTER_GENERICDATAELEMENT_H_