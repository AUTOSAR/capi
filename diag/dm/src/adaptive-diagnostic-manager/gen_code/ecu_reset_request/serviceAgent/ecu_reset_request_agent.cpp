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
/// @file       ecu_reset_request_agent.cpp
/// @brief
/// @details
/// @date       2024-12-13
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================

#include "ecu_reset_request_agent.h"

namespace isoft {
namespace dm {
namespace dic {

EcuResetRequestAgent::EcuResetRequestAgent(uint16_t const& instanceId, uint32_t const& serviceInstanceId)
{
    proxy_ = std::make_unique< EcuResetRequestProxy >(instanceId, serviceInstanceId);
}

/// @brief interface for subFunction En-/DisableRapidShutdown
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
/// @return The type of the last machine reset. error code: kRequestFailed
///
/// @traceid{SWS_DM_01015}@tracestatus{draft}
ara::core::Result< std::uint32_t > EcuResetRequestAgent::GetLastResetCause() noexcept
{
    return proxy_->GetLastResetCause();
}

/// @brief interface for subFunction En-/DisableRapidShutdown
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
/// @param[in] enable when enable is set to true the rapid shutdown will be enabled, setting enable to false will
/// disable rapid shutdown
/// @param[in] metaInfo MetaInfo of the request.
/// @param[in] cancellationHandler Set if the current conversation is canceled.
/// @return ara::core::Future<void> error code: kRejected, kResetTypeNotSupported
/// @throws on overflow
/// @traceid{SWS_DM_01012}@tracestatus{draft}
ara::core::Future< void > EcuResetRequestAgent::EnableRapidShutdown(
    bool enable, MetaInfoMap const& metaInfo, std::shared_ptr< CancellationEvent > const& cancellationEventPtr) noexcept
{
    return proxy_->EnableRapidShutdown(enable, metaInfo, cancellationEventPtr);
}

/// @brief Called for any EcuRest subFunction, except En-/DisableRapidShutdown.
/// StateManagement needs to evalute carefully if the request to restart parts or the whole machine.
/// Once the request to reset is accepted, the StateManagement has to rely on this decision for the
/// ExecuteReset() trigger.
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
/// @param[in] resetType Type of the requested reset.
/// @param[in] id id of the custom reset type. Will only be evaluated when resetType is "custom"
/// @param[in] metaInfo[in] MetaInfo of the request.
/// @param[in] cancellationHandler Set if the current conversation is canceled.
/// @return ara::core::Future<void> error code : kRejected, kRequestFailed, kCustomResetTypeNotSupported,
/// kResetTypeNotSupported
/// @throws on overflow
///
/// @traceid{SWS_DM_01013}@tracestatus{draft}
ara::core::Future< void > EcuResetRequestAgent::RequestReset(
    std::uint32_t resetType,
    ara::core::Optional< std::uint8_t > id,
    MetaInfoMap const& metaInfo,
    std::shared_ptr< CancellationEvent > const& cancellationEventPtr) noexcept
{
    return proxy_->RequestReset(resetType, id, metaInfo, cancellationEventPtr);
}

/// @brief StateManagement has to execute the requested reset.
/// Called after DM sent the response message to tester.
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
/// @param[in] metaInfo
/// @throws on overflow
///
/// @traceid{SWS_DM_01014}@tracestatus{draft}
void EcuResetRequestAgent::ExecuteReset(MetaInfoMap metaInfo) noexcept
{
    return proxy_->ExecuteReset(std::move(metaInfo));
}

}  // namespace dic
}  // namespace dm
}  // namespace isoft