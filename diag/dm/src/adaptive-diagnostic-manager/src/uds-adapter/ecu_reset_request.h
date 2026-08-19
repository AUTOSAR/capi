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
/// @file       ecu_reset_request.h
/// @brief
/// @details
/// @date       2021-11-23
/// @author     jiawei
/// @version    1.2.0
///
/// ================================================================

#ifndef ADAPTIVEDIAGNOSTICMANAGER_SRC_UDSADAPTER_ECURESETREQUEST_H_
#define ADAPTIVEDIAGNOSTICMANAGER_SRC_UDSADAPTER_ECURESETREQUEST_H_
#include <isoft/uds/ecu_reset_management/ecu_reset_request.h>

#include "gen_code/ecu_reset_request/serviceAgent/ecu_reset_request_agent.h"
namespace ara {
namespace diag {
namespace dmd {
/// @brief Service EcuReset Request interface
///
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
///
/// @traceid{SWS_DM_01009}@tracestatus{draft}
class EcuResetRequest : public isoft::uds::server::EcuResetRequestInterface
{
public:
    using MetaInfoMap         = isoft::uds::server::MetaInfoMap;
    using CancellationHandler = isoft::uds::server::CancellationHandler;
    using ResetRequestType    = isoft::uds::server::ResetRequestType;

    /// @brief Constructor of EcuResetRequest
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[in] specifier An InstanceSpecifier linking this instance with the
    /// PortPrototype in the manifest
    /// @throws on overflow
    ///
    /// @traceid{SWS_DM_01010}@tracestatus{draft}
    explicit EcuResetRequest(uint16_t const &instanceId, uint32_t const &serviceInstanceId);

    /// @brief Destructor of EcuResetRequest
    ///
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    ///
    /// @traceid{SWS_DM_01011}@tracestatus{draft}
    ~EcuResetRequest() noexcept override = default;

    /// @brief copy constructor
    /// @param other
    EcuResetRequest(EcuResetRequest const &other) noexcept = delete;
    /// @brief copy assignment operator
    /// @param other
    /// @return EcuResetRequest&
    EcuResetRequest &operator=(EcuResetRequest const &other) noexcept = delete;
    /// @brief move constructor
    /// @param other
    EcuResetRequest(EcuResetRequest &&other) noexcept = default;  // NOLINT
    /// @brief move assignment operator
    /// @param other
    /// @return EcuResetRequest&
    EcuResetRequest &                                       // NOLINT
    operator=(EcuResetRequest &&other) noexcept = default;  // NOLINT

    /// @brief interface for subFunction En-/DisableRapidShutdown
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[in] enable when enable is set to true the rapid shutdown will be
    /// enabled, setting enable to false will disable rapid shutdown
    /// @param[in] metaInfo MetaInfo of the request.
    /// @param[in] cancellationHandler Set if the current conversation is
    /// canceled.
    /// @return ara::core::Future<void> error code: kRejected,
    /// kResetTypeNotSupported
    /// @throws on overflow
    /// @traceid{SWS_DM_01012}@tracestatus{draft}
    isoft::uds::Result< void > EnableRapidShutdown(bool enable,
                                                   MetaInfoMap const &metaInfo,
                                                   CancellationHandler cancellationHandler) override;

    /// @brief Called for any EcuRest subFunction, except
    /// En-/DisableRapidShutdown. StateManagement needs to evalute carefully if
    /// the request to restart parts or the whole machine. Once the request to
    /// reset is accepted, the StateManagement has to rely on this decision for
    /// the ExecuteReset() trigger.
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[in] resetType Type of the requested reset.
    /// @param[in] id id of the custom reset type. Will only be evaluated when
    /// resetType is "custom"
    /// @param[in] metaInfo[in] MetaInfo of the request.
    /// @param[in] cancellationHandler Set if the current conversation is
    /// canceled.
    /// @return ara::core::Future<void> error code : kRejected, kRequestFailed,
    /// kCustomResetTypeNotSupported, kResetTypeNotSupported
    /// @throws on overflow
    ///
    /// @traceid{SWS_DM_01013}@tracestatus{draft}
    isoft::uds::Result< void > RequestReset(ResetRequestType resetType,
                                            ara::core::Optional< std::uint8_t > id,
                                            MetaInfoMap const &metaInfo,
                                            CancellationHandler cancellationHandler) noexcept override;

    /// @brief StateManagement has to execute the requested reset.
    /// Called after DM sent the response message to tester.
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[in] metaInfo
    /// @throws on overflow
    ///
    /// @traceid{SWS_DM_01014}@tracestatus{draft}
    void ExecuteReset(MetaInfoMap metaInfo) noexcept override;

private:
    std::unique_ptr< isoft::dm::dic::EcuResetRequestAgent > agentPtr_;
};
}  // namespace dmd
}  // namespace diag
}  // namespace ara

#endif  /// ADAPTIVEDIAGNOSTICMANAGER_SRC_UDSADAPTER_ECURESETREQUEST_H_