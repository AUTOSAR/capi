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
/// @file       condition_proxy.cpp
/// @brief
/// @details
/// @date       2024-12-17
/// @author     zhanglipeng
/// @version    1.2.0
///
/// ================================================================

#include "condition_proxy.h"

#include "ara/diag/diag_error_domain.h"
#include "serialization/serialization.h"

namespace isoft {
namespace dm {
namespace dic {

constexpr uint8_t kFuncIDGetCondition{1U};
constexpr uint8_t kFuncIDSetCondition{2U};

ConditionProxy::ConditionProxy(uint16_t const& instanceId, uint32_t const& serviceInstanceId)
    : BussinessClientProxy{serviceInstanceId, instanceId}
{
}

/// @brief Register the callback for AA side to get the status of operation cycle
/// @param callBack Callback
/// @return Return 0 for successful registration
ara::core::Result< ara::diag::ConditionType > ConditionProxy::GetCondition()
{
    static uint8_t s_CallId{0U};
    ara::core::Result< uint8_t > const result{SyncCallFunc< uint8_t >(kFuncIDGetCondition, s_CallId)};
    if (result.HasValue()) {
        ara::diag::ConditionType ret = static_cast< ara::diag::ConditionType >(result.Value());
        return ara::core::Result< ara::diag::ConditionType >::FromValue(ret);
    }
    return ara::core::Result< ara::diag::ConditionType >::FromError(result.Error());
}

/// @brief Register the callback for AA side to set the status of operation cycle
/// @param callBack Callback
/// @return Return 0 for successful registration
ara::core::Result< void > ConditionProxy::SetCondition(ara::diag::ConditionType const type)
{
    static uint8_t s_CallId{0U};
    ara::core::Result< void > const result{
        SyncCallFunc< void >(kFuncIDSetCondition, s_CallId, static_cast< uint8_t >(type))};
    if (result.HasValue()) {
        return ara::core::Result< void >::FromValue();
    }
    return ara::core::Result< void >::FromError(result.Error());
}

}  // namespace dic
}  // namespace dm
}  // namespace isoft