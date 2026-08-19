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
/// @file       condition.cpp
/// @brief
/// @details
/// @date       2023-04-17
/// @author     zhanglipeng
/// @version    1.2.0
///
/// ================================================================

#include "condition.h"

namespace ara {
namespace diag {
namespace dmd {

Condition::Condition(uint32_t const& serviceInstanceId) noexcept
    : conditionAgent_{std::make_unique< isoft::dm::dis::ConditionAgent >(serviceInstanceId)}
{
}

/// @brief Register callback for AA side to set condition status
/// @param callBack Callback function
/// @return Returns 0 for successful registration
std::int32_t Condition::RegisterSetCondition(isoft::uds::server::SetConditionCallBack const& callBack)
{
    if (conditionAgent_.get() == nullptr) {
        return -1;
    }

    setCb_ = callBack;
    conditionAgent_->RegisterSetCondition([this](ara::diag::ConditionType type) { setCb_(static_cast< bool >(type)); });
    return 0;
}

/// @brief Register callback for AA side to get condition status
/// @param callBack Callback function
/// @return Returns 0 for successful registration
std::int32_t Condition::RegisterGetCondition(isoft::uds::server::GetConditionCallBack const& callBack)
{
    if (conditionAgent_.get() == nullptr) {
        return -1;
    }

    getCb_ = callBack;
    conditionAgent_->RegisterGetCondition(
        [this]() -> ara::diag::ConditionType { return static_cast< ara::diag::ConditionType >(getCb_()); });
    return 0;
}

}  // namespace dmd
}  // namespace diag
}  // namespace ara