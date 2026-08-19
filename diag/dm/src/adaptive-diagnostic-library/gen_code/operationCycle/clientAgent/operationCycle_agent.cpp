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
namespace dic {

OperationCycleAgent::OperationCycleAgent(uint16_t const& instanceId, uint32_t const& serviceInstanceId)
    : proxy_{std::make_unique< OperationCycleProxy >(instanceId, serviceInstanceId)}
{
}

/// @brief Register callback function for AA side of operation cycle status change
/// @param notifier Callback function to be registered
/// @return
void OperationCycleAgent::RegisterOperationCycleNotifier(
    std::function< void(ara::diag::OperationCycleType) > const& notifier)
{
    if (proxy_.get() == nullptr) {
        return;
    }

    proxy_->RegisterOperationCycleNotifier(notifier);
}

/// @brief Get the status of operation cycle
/// @param
/// @return Current operation cycle type
ara::core::Result< ara::diag::OperationCycleType > OperationCycleAgent::GetOperationCycle()
{
    return proxy_->GetOperationCycle();
}

/// @brief Set the status of operation cycle
/// @param
/// @return Return void for successful
ara::core::Result< void > OperationCycleAgent::SetOperationCycle(ara::diag::OperationCycleType const op)
{
    return proxy_->SetOperationCycle(op);
}

/// @brief Determine whether the service is ready
/// @return true: Ready
bool OperationCycleAgent::IsServiceReady() { return proxy_->IsServiceReady(); }

}  // namespace dic
}  // namespace dm
}  // namespace isoft