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
/// @file       operationCycle_agent.cpp
/// @brief
/// @details
/// @date       2024-12-12
/// @author     zhanglipeng
/// @version    1.2.0
///
/// ================================================================

#include "operationCycle_agent.h"

namespace isoft {
namespace dm {
namespace dis {

OperationCycleAgent::OperationCycleAgent(uint32_t const& serviceInstanceId)
{
    proxy_ = std::make_unique< OperationCycleProxy >(serviceInstanceId);
}

/// @brief Notify AA side of operation cycle status change
/// @param operation Operation cycle status
/// @return
void OperationCycleAgent::NotifyOperationCycle(ara::diag::OperationCycleType const& operation)
{
    if (proxy_.get() == nullptr) {
        return;
    }

    proxy_->NotifyOperationCycle(operation);
}

/// @brief Register callback for AA side to get operation cycle status
/// @param callBack Callback function to be registered
/// @return
void OperationCycleAgent::RegisterGetOperationCycle(std::function< ara::diag::OperationCycleType() > const& callBack)
{
    if (proxy_.get() == nullptr) {
        return;
    }

    proxy_->RegisterGetOperationCycle(callBack);
}

/// @brief Register callback for AA side to set operation cycle status
/// @param callBack Callback function to be registered
/// @return
void OperationCycleAgent::RegisterSetOperationCycle(
    std::function< void(ara::diag::OperationCycleType const) > const& callBack)
{
    if (proxy_.get() == nullptr) {
        return;
    }

    proxy_->RegisterSetOperationCycle(callBack);
}

}  // namespace dis
}  // namespace dm
}  // namespace isoft