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
/// @file       indicator.cpp
/// @brief      This file provides the implementation of Indicator and related types.
/// @details
/// @date       2024-12-12
/// @author     zhanglipeng
/// @version    1.2.0
///
/// ================================================================

#include "indicator.h"

namespace ara {
namespace diag {
namespace dmd {

Indicator::Indicator(uint32_t const& serviceInstanceId) noexcept
    : indicatorAgent_{std::make_unique< isoft::dm::dis::IndicatorAgent >(serviceInstanceId)}
{
}

/// @brief Notify AA side of indicator changes
/// @param indicatorType Indicator status
/// @return Returns 0 for successful sending
std::int32_t Indicator::NotifyIndicator(isoft::uds::server::IndicatorType const& indicatorType)
{
    if (indicatorAgent_.get() == nullptr) {
        return -1;
    }

    indicatorAgent_->NotifyIndicator(static_cast< uint8_t >(indicatorType));
    return 0;
}

/// @brief Register callback for AA side to get indicator status
/// @param callBack Callback function
/// @return Returns 0 for successful registration
std::int32_t Indicator::RegisterGetIndicator(isoft::uds::server::GetIndicatorCallBack const& callBack)
{
    if (indicatorAgent_.get() == nullptr) {
        return -1;
    }

    getIndicatorCallBack_ = callBack;
    indicatorAgent_->RegisterGetIndicator(
        [this]() -> uint8_t { return static_cast< uint8_t >(getIndicatorCallBack_()); });
    return 0;
}

}  // namespace dmd
}  // namespace diag
}  // namespace ara