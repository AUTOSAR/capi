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
/// @file       service_validation.cpp
/// @brief
/// @details
/// @date       2024-12-23
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================

#include "service_validation.h"
namespace ara {
namespace diag {
namespace dmd {
ServiceValidation::ServiceValidation(uint16_t const& instanceId, uint32_t const& serviceInstanceId)
    : agentPtr_{std::make_unique< isoft::dm::dic::ServiceValidationAgent >(instanceId, serviceInstanceId)}
{
}

isoft::uds::Result< void > ServiceValidation::Validate(std::vector< std::uint8_t > requestData,
                                                       MetaInfoMap& metaInfo) noexcept
{
    isoft::dm::MetaInfoMap comMetaInfo{};
    for (auto&& pair : metaInfo) {
        ara::core::String key{pair.first};
        ara::core::String value{pair.second};
        comMetaInfo.insert({key, value});
    }

    ara::core::Result< void > result = agentPtr_->Validate(requestData, comMetaInfo);
    if (result.HasValue()) {
        return {};
    }
    if (result.Error().Value() < 0) {
        return isoft::uds::Result< void >::FromError(isoft::serialize::kInt32_0x10);
    }
    return isoft::uds::Result< void >::FromError(result.Error().Value());
}

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
isoft::uds::Result< void > ServiceValidation::Confirmation(isoft::uds::server::ConfirmationStatusType status,
                                                           MetaInfoMap& metaInfo) noexcept
{
    isoft::dm::dic::ConfirmationStatusType comType{static_cast< std::uint32_t >(status)};
    isoft::dm::MetaInfoMap comMetaInfo{};
    for (auto&& pair : metaInfo) {
        ara::core::String key{pair.first};
        ara::core::String value{pair.second};
        comMetaInfo.insert({key, value});
    }

    ara::core::Result< void > result = agentPtr_->Confirmation(comType, comMetaInfo);
    if (result.HasValue()) {
        return {};
    }
    if (result.Error().Value() < 0) {
        return isoft::uds::Result< void >::FromError(isoft::serialize::kInt32_0x10);
    }
    return isoft::uds::Result< void >::FromError(result.Error().Value());
}

}  // namespace dmd
}  // namespace diag
}  // namespace ara
