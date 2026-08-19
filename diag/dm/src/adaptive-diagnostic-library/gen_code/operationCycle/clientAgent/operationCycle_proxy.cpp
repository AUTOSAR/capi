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
/// @file       operationCycle_proxy.cpp
/// @brief
/// @details
/// @date       2024-12-06
/// @author     xuhua
/// @version    1.2.0
///
/// ================================================================

#include "operationCycle_proxy.h"

#include "ara/diag/diag_error_domain.h"
#include "log/log.h"
#include "serialization/serialization.h"

namespace isoft {
namespace dm {
namespace dic {

constexpr uint8_t kFuncIDNotifyOperationCycle{1U};
constexpr uint8_t kFuncIDGetOperationCycle{2U};
constexpr uint8_t kFuncIDSetOperationCycle{3U};

using ara::diag::common::LogError;

OperationCycleProxy::OperationCycleProxy(uint16_t const& instanceId, uint32_t const& serviceInstanceId)
    : BussinessClientProxy{serviceInstanceId, instanceId}
{
    RegisterNotificationCallBack([this](FuncData const& funcData) { _notify(funcData); });
}

/// @brief Register callback function for AA side of operation cycle status change
/// @param notifier Callback function to be registered
/// @return
void OperationCycleProxy::RegisterOperationCycleNotifier(
    std::function< void(ara::diag::OperationCycleType) > const& notifier)
{
    static uint8_t s_CallId{0U};

    ara::core::Result< void > result{RegisterCallBack(kFuncIDNotifyOperationCycle, s_CallId)};
    if (!result.HasValue()) {
        LogError() << "OperationCycleProxy::RegisterOperationCycleNotifier|register error";
        return;
    }

    opNotiferCb_ = notifier;
}

/// @brief Get the status of operation cycle
/// @param
/// @return Current operation cycle type
ara::core::Result< ara::diag::OperationCycleType > OperationCycleProxy::GetOperationCycle()
{
    static uint8_t s_CallId{0U};
    ara::core::Result< uint8_t > const result{SyncCallFunc< uint8_t >(kFuncIDGetOperationCycle, s_CallId)};
    if (result.HasValue()) {
        ara::diag::OperationCycleType ret = static_cast< ara::diag::OperationCycleType >(result.Value());
        return ara::core::Result< ara::diag::OperationCycleType >::FromValue(ret);
    }
    return ara::core::Result< ara::diag::OperationCycleType >::FromError(result.Error());
}

/// @brief Set the status of operation cycle
/// @param
/// @return Return void for successful
ara::core::Result< void > OperationCycleProxy::SetOperationCycle(ara::diag::OperationCycleType const op)
{
    static uint8_t s_CallId{0U};
    ara::core::Result< void > const result{
        SyncCallFunc< void >(kFuncIDSetOperationCycle, s_CallId, static_cast< uint8_t >(op))};
    if (result.HasValue()) {
        return ara::core::Result< void >::FromValue();
    }
    return ara::core::Result< void >::FromError(result.Error());
}

void OperationCycleProxy::_notify(FuncData const& funcData) const
{
    switch (static_cast< int >(funcData.funcId)) {
        case kFuncIDNotifyOperationCycle: {
            if (nullptr != opNotiferCb_) {
                std::uint8_t op{};
                if (serialize::Deserialize(funcData.data, op) > 0) {
                    opNotiferCb_(static_cast< ara::diag::OperationCycleType >(op));
                }
            } else {
                LogError() << "OperationCycleProxy::_notify|opNotiferCb_ is nullptr";
            }
        } break;
        default: {
        } break;
    }
}

}  // namespace dic
}  // namespace dm
}  // namespace isoft