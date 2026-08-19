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
/// @file       operation_cycle.cpp
/// @brief      This file provides the implementation of the operation class
/// @details
/// @date       2024-12-12
/// @author     zhanglipeng
/// @version    1.2.0
///
/// ================================================================

#include "operation_cycle.h"

namespace ara {
namespace diag {
namespace dmd {

OperationCycle::OperationCycle(uint32_t const& serviceInstanceId) noexcept
    : operationCycleAgent_{std::make_unique< isoft::dm::dis::OperationCycleAgent >(serviceInstanceId)}
{
}

/// @brief Notify AA side of operation cycle status changes
/// @param operation Operation cycle status
/// @return Returns 0 for successful call
std::int32_t OperationCycle::NotifyOperationCycle(isoft::uds::server::OperationCycleType const& operation)
{
    if (operationCycleAgent_.get() == nullptr) {
        return -1;
    }

    operationCycleAgent_->NotifyOperationCycle(static_cast< ara::diag::OperationCycleType >(operation));
    return 0;
}

/// @brief Register callback for AA side to get the status of the operation cycle
/// @param callBack Callback function
/// @return Returns 0 for successful registration
std::int32_t OperationCycle::RegisterGetOperationCycle(isoft::uds::server::GetOperationCycleCallBack const& callBack)
{
    if (operationCycleAgent_.get() == nullptr) {
        return -1;
    }

    getOpCb_ = callBack;
    operationCycleAgent_->RegisterGetOperationCycle(
        [this]() -> ara::diag::OperationCycleType { return static_cast< ara::diag::OperationCycleType >(getOpCb_()); });
    return 0;
}

/// @brief Register callback for AA side to set the status of the operation cycle
/// @param callBack Callback function
/// @return Returns 0 for successful registration
std::int32_t OperationCycle::RegisterSetOperationCycle(isoft::uds::server::SetOperationCycleCallBack const& callBack)
{
    if (operationCycleAgent_.get() == nullptr) {
        return -1;
    }

    setOpCb_ = callBack;
    operationCycleAgent_->RegisterSetOperationCycle([this](ara::diag::OperationCycleType operation) {
        setOpCb_(static_cast< isoft::uds::server::OperationCycleType >(operation));
    });
    return 0;
}

}  // namespace dmd
}  // namespace diag
}  // namespace ara