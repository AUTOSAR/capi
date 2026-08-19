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
/// @file       ecu_reset_request.cpp
/// @brief
/// @details
/// @date       2024-12-13
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================

#include "ecu_reset_request.h"

#include "serialization/serialization/common_data_type.h"
namespace ara {
namespace diag {
namespace dmd {

/// @brief Constructor of EcuResetRequest
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
/// @param[in] specifier An InstanceSpecifier linking this instance with the PortPrototype in the manifest
/// @throws on overflow
///
/// @traceid{SWS_DM_01010}@tracestatus{draft}
EcuResetRequest::EcuResetRequest(uint16_t const& instanceId, uint32_t const& serviceInstanceId)
    : agentPtr_{std::make_unique< isoft::dm::dic::EcuResetRequestAgent >(instanceId, serviceInstanceId)}
{
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
isoft::uds::Result< void > EcuResetRequest::EnableRapidShutdown(bool enable,
                                                                MetaInfoMap const& metaInfo,
                                                                CancellationHandler cancellationHandler)
{
    isoft::dm::MetaInfoMap comMetaInfo{};
    for (auto&& pair : metaInfo) {
        ara::core::String key{pair.first};
        ara::core::String value{pair.second};
        comMetaInfo.insert({key, value});
    }
    std::shared_ptr< isoft::dm::CancellationEvent > cancellationEventPtr{
        std::make_shared< isoft::dm::CancellationEvent >()};
    cancellationHandler.SetNotifier([&cancellationEventPtr]() { cancellationEventPtr->Notifier(); });

    ara::core::Future< void > requestResult = agentPtr_->EnableRapidShutdown(enable, comMetaInfo, cancellationEventPtr);
    ara::core::Result< void > result{requestResult.GetResult()};
    if (result.HasValue()) {
        return {};
    }
    if (result.Error().Value() < 0) {
        return isoft::uds::Result< void >::FromError(isoft::serialize::kInt32_0x10);
    }
    return isoft::uds::Result< void >::FromError(result.Error().Value());
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
isoft::uds::Result< void > EcuResetRequest::RequestReset(ResetRequestType resetType,
                                                         ara::core::Optional< std::uint8_t > id,
                                                         MetaInfoMap const& metaInfo,
                                                         CancellationHandler cancellationHandler) noexcept
{
    isoft::dm::MetaInfoMap comMetaInfo{};
    for (auto&& pair : metaInfo) {
        ara::core::String key{pair.first};
        ara::core::String value{pair.second};
        comMetaInfo.insert({key, value});
    }
    std::shared_ptr< isoft::dm::CancellationEvent > cancellationEventPtr{
        std::make_shared< isoft::dm::CancellationEvent >()};
    cancellationHandler.SetNotifier([&cancellationEventPtr]() { cancellationEventPtr->Notifier(); });

    ara::core::Future< void > requestResult
        = agentPtr_->RequestReset(static_cast< std::uint32_t >(resetType), id, comMetaInfo, cancellationEventPtr);
    ara::core::Result< void > result{requestResult.GetResult()};
    if (result.HasValue()) {
        return {};
    }
    if (result.Error().Value() < 0) {
        return isoft::uds::Result< void >::FromError(isoft::serialize::kInt32_0x10);
    }
    return isoft::uds::Result< void >::FromError(result.Error().Value());
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
void EcuResetRequest::ExecuteReset(MetaInfoMap metaInfo) noexcept
{
    isoft::dm::MetaInfoMap comMetaInfo{};
    for (auto&& pair : metaInfo) {
        ara::core::String key{pair.first};
        ara::core::String value{pair.second};
        comMetaInfo.insert({key, value});
    }

    agentPtr_->ExecuteReset(comMetaInfo);
}

}  // namespace dmd
}  // namespace diag
}  // namespace ara
