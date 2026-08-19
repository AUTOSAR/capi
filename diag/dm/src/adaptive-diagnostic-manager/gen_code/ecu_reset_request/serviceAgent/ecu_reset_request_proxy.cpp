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
/// @file       ecu_reset_request_proxy.cpp
/// @brief
/// @details
/// @date       2024-12-13
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================

#include "ecu_reset_request_proxy.h"

#include <thread>

#include "ara/diag/diag_error_domain.h"

namespace isoft {
namespace dm {
namespace dic {

using isoft::dm::RetData;

constexpr uint8_t kFuncIDGetLastResetCause{1U};
constexpr uint8_t kFuncIDEnableRapidShutdown{2U};
constexpr uint8_t kFuncIDRequestReset{3U};
constexpr uint8_t kFuncIDExecuteReset{4U};

EcuResetRequestProxy::EcuResetRequestProxy(uint16_t const& instanceId, uint32_t const& serviceInstanceId)
    : BussinessClientProxy{serviceInstanceId, instanceId}
{
}

/// @brief interface for subFunction En-/DisableRapidShutdown
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
/// @return The type of the last machine reset. error code: kRequestFailed
///
/// @traceid{SWS_DM_01015}@tracestatus{draft}
ara::core::Result< std::uint32_t > EcuResetRequestProxy::GetLastResetCause() noexcept
{
    constexpr uint8_t kFunc_Id{kFuncIDGetLastResetCause};
    static uint8_t s_CallId{0U};

    ara::core::Result< std::uint32_t > result{SyncCallFunc< std::uint32_t >(kFunc_Id, s_CallId)};

    return result;
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
ara::core::Future< void > EcuResetRequestProxy::EnableRapidShutdown(
    bool enable, MetaInfoMap const& metaInfo, std::shared_ptr< CancellationEvent > const& cancellationEventPtr) noexcept
{
    constexpr uint8_t kFunc_Id{kFuncIDEnableRapidShutdown};
    static uint8_t s_CallId{0U};

    ara::core::Future< void > result{AsyncCallFunc< void >(kFunc_Id, s_CallId, cancellationEventPtr, enable, metaInfo)};

    return result;
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
ara::core::Future< void > EcuResetRequestProxy::RequestReset(
    std::uint32_t resetType,
    ara::core::Optional< std::uint8_t > id,
    MetaInfoMap const& metaInfo,
    std::shared_ptr< CancellationEvent > const& cancellationEventPtr) noexcept
{
    constexpr uint8_t kFunc_Id{kFuncIDRequestReset};
    static uint8_t s_CallId{0U};
    std::uint8_t opId{0U};
    if (id.has_value()) {
        opId = *id;
    }
    ara::core::Future< void > result{
        AsyncCallFunc< void >(kFunc_Id, s_CallId, cancellationEventPtr, resetType, opId, metaInfo)};

    return result;
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
void EcuResetRequestProxy::ExecuteReset(MetaInfoMap metaInfo) noexcept
{
    constexpr uint8_t kFunc_Id{kFuncIDExecuteReset};
    static uint8_t s_CallId{0U};

    ara::core::Result< void > result{SyncCallFunc< void >(kFunc_Id, s_CallId, std::move(metaInfo))};
    std::ignore = result;
    return;
}

}  // namespace dic
}  // namespace dm
}  // namespace isoft
