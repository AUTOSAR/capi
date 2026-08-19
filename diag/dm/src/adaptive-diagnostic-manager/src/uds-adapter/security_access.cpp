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
/// @file       security_access.cpp
/// @brief
/// @details
/// @date       2024-12-23
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================

#include "security_access.h"
namespace ara {
namespace diag {
namespace dmd {
SecurityAccess::SecurityAccess(uint16_t const& instanceId, uint32_t const& serviceInstanceId)
    : agentPtr_{std::make_unique< isoft::dm::dic::SecurityAccessAgent >(instanceId, serviceInstanceId)}
{
}

/// @brief Called for any request messsage.
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
/// @param[in] securityAccessDataRecord Security Access payload
/// @param[in] metaInfo MetaInfoMap of the request.
/// @param[in] cancellationHandler Set if the current conversation is canceled.
/// @return provided seed
/// error code: kSubfunctionNotSupported, kIncorrectMessageLengthOrInvalidFormat, kConditionsNotCorrect,
///> kRequestSequenceError, kRequestOutOfRange, kInvalidKey, kExceedNumberOfAttempts, kRequiredTimeDelayNotExpired,
///
///
/// @traceid{SWS_DM_00764}@tracestatus{draft}
isoft::uds::Result< std::vector< std::uint8_t > > SecurityAccess::GetSeed(
    std::vector< std::uint8_t > const& securityAccessDataRecord,
    MetaInfoMap& metaInfo,
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

    ara::core::Future< std::vector< uint8_t > > requestResult
        = agentPtr_->GetSeed(securityAccessDataRecord, comMetaInfo, cancellationEventPtr);
    ara::core::Result< std::vector< uint8_t > > result{requestResult.GetResult()};
    if (result.HasValue()) {
        return isoft::uds::Result< std::vector< std::uint8_t > >::FromValue(result.Value());
    }
    if (result.Error().Value() < 0) {
        return isoft::uds::Result< std::vector< std::uint8_t > >::FromError(isoft::dm::kGeneralReject);
    }
    return isoft::uds::Result< std::vector< std::uint8_t > >::FromError(result.Error().Value());
}

/// @brief This method is called, when a diagnostic request has been finished, to notify about the outcome.
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
/// @param[in] key The key to be validated
/// @param[in] metaInfo MetaInfoMap of the request.
/// @param[in] cancellationHandler Set if the current conversation is canceled.
/// @return Result of the key validation.
/// error code: kSubfunctionNotSupported, kIncorrectMessageLengthOrInvalidFormat, kConditionsNotCorrect,
/// kRequestSequenceError, kRequestOutOfRange, kInvalidKey, kExceedNumberOfAttempts, kRequiredTimeDelayNotExpired
///
///
/// @traceid{SWS_DM_00765}@tracestatus{draft}
isoft::uds::Result< isoft::uds::server::KeyCompareResultType > SecurityAccess::CompareKey(
    std::vector< std::uint8_t > const& key, MetaInfoMap& metaInfo, CancellationHandler cancellationHandler) noexcept
{
    isoft::dm::MetaInfoMap comMetaInfo{};
    for (auto&& pair : metaInfo) {
        ara::core::String mapKey{pair.first};
        ara::core::String mapValue{pair.second};
        comMetaInfo.insert({mapKey, mapValue});
    }
    std::shared_ptr< isoft::dm::CancellationEvent > cancellationEventPtr{
        std::make_shared< isoft::dm::CancellationEvent >()};
    cancellationHandler.SetNotifier([&cancellationEventPtr]() { cancellationEventPtr->Notifier(); });

    ara::core::Future< uint32_t > requestResult = agentPtr_->CompareKey(key, comMetaInfo, cancellationEventPtr);
    ara::core::Result< uint32_t > result{requestResult.GetResult()};
    if (result.HasValue()) {
        isoft::uds::server::KeyCompareResultType keyCompareResultType{
            static_cast< isoft::uds::server::KeyCompareResultType >(result.Value())};
        return isoft::uds::Result< isoft::uds::server::KeyCompareResultType >::FromValue(keyCompareResultType);
    }
    if (result.Error().Value() < 0) {
        return isoft::uds::Result< isoft::uds::server::KeyCompareResultType >::FromError(isoft::dm::kGeneralReject);
    }
    return isoft::uds::Result< isoft::uds::server::KeyCompareResultType >::FromError(result.Error().Value());
}

}  // namespace dmd
}  // namespace diag
}  // namespace ara
