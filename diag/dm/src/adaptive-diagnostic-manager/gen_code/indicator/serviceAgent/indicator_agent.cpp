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
/// @file       indicator_agent.cpp
/// @brief
/// @details
/// @date       2024-12-17
/// @author     zhanglipeng
/// @version    1.2.0
///
/// ================================================================

#include "indicator_agent.h"

namespace isoft {
namespace dm {
namespace dis {

IndicatorAgent::IndicatorAgent(uint32_t const& serviceInstanceId)
{
    proxy_ = std::make_unique< IndicatorProxy >(serviceInstanceId);
}

/// @brief Notify AA side of indicator change
/// @param indicatorType Indicator status
/// @return
void IndicatorAgent::NotifyIndicator(uint8_t const& indicatorType) { proxy_->NotifyIndicator(indicatorType); }

/// @brief Register callback for AA side to get indicator status
/// @param callBack Callback function to be registered
/// @return
void IndicatorAgent::RegisterGetIndicator(std::function< uint8_t() > const& callBack)
{
    proxy_->RegisterGetIndicator(callBack);
}

}  // namespace dis
}  // namespace dm
}  // namespace isoft