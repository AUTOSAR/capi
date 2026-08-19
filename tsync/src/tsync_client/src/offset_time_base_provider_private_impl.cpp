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
/// @file       offset_time_base_provider_private_impl.cpp
/// @brief      offset time base provider private implementation class
/// @details
/// @date       2023-02-09
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================

#include "ara/tsync/offset_time_base_provider_private_impl.h"

#include "ara/tsync/internal/log/logger.h"

namespace ara {
namespace tsync {

/// @brief get current time
/// @return timestamp
ara::tsync::Timestamp OffsetTimeBaseProvider::OffsetTimeBaseProviderPrivateImpl::GetCurrentTime() const noexcept
{
    if (0 != Validate()) {
        return ara::tsync::Timestamp{std::chrono::nanoseconds{-1}};
    }
    return resourceCtx->GetOffsetCurrentTime();
}

/// @brief used to set a new offset time value for the time domain, which will immediately trigger bus transmission. Does not modify the system clock, only records the difference.
/// @param offsetTime - offset time value
/// @param userData - user data
/// @return 0 - success
/// @return <0 - failure
std::int32_t OffsetTimeBaseProvider::OffsetTimeBaseProviderPrivateImpl::SetTime(
    ara::tsync::Timestamp const &offsetTime, ara::core::Span< ara::core::Byte const > const &userData) noexcept
{
    if (0 != Validate()) {
        return kRET_E1;
    }
    std::int32_t ret = resourceCtx->SetOffsetTime(offsetTime, userData);
    if (0 != ret) {
        return ret;
    }
    if (0 != proxyStub->Notify(internal::timebase::proxy::ProxyEventType::kSetTime)) {
        return kRET_E2;
    }
    return 0;
}

/// @brief check the validity of the current time base
/// @return 0 - no exception
/// @return <0 - problematic
std::int32_t OffsetTimeBaseProvider::OffsetTimeBaseProviderPrivateImpl::Validate() const noexcept
{
    if (nullptr == resourceCtx) {
        return kRET_E1;
    }
    std::int32_t ret{resourceCtx->ValidateOffsetTimeBaseProvider()};
    if (0 != ret) {
        return ret;
    }
    if (nullptr == proxyStub) {
        return kRET_E5;
    }
    return 0;
}

}  // namespace tsync
}  // namespace ara
