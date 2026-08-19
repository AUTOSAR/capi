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
/// @date       2024-12-12
/// @author     zhanglipeng
/// @version    1.2.0
///
/// ================================================================

#include "operationCycle_proxy.h"

#include "netProxy/net_proxy.h"
#include "netProxy/rpc_define.h"
#include "serialization/serialization.h"

namespace isoft {
namespace dm {
namespace dis {

constexpr uint8_t kFuncIDNotifyOperationCycle{1U};
constexpr uint8_t kFuncIDGetOperationCycle{2U};
constexpr uint8_t kFuncIDSetOperationCycle{3U};

OperationCycleProxy::OperationCycleProxy(uint32_t const &serviceInstanceId) : BussinessServiceProxy{serviceInstanceId}
{
    NetProxy::GetInstance().GetNetAccess()->RegisterService(
        serviceInstanceId, [this](CallRequest &&callData) { CallHandle(std::move(callData)); });
}

void OperationCycleProxy::CallHandle(CallRequest &&callData)
{
    CallRequest cr{std::move(callData)};
    switch (cr.funcData.funcId) {
        case kFuncIDNotifyOperationCycle: {
            Subscribe(std::move(cr));
            break;
        }
        case kFuncIDGetOperationCycle: {
            _getOperationCycle(std::move(cr));
            break;
        }
        case kFuncIDSetOperationCycle: {
            _setOperationCycle(std::move(cr));
            break;
        }
        default: {
        } break;
    }
}

/// @brief Notify AA side of operation cycle status change
/// @param operation Operation cycle status
/// @return
void OperationCycleProxy::NotifyOperationCycle(ara::diag::OperationCycleType const &operation)
{
    std::vector< uint8_t > payLoad;
    uint8_t const op = static_cast< uint8_t >(operation);
    if (serialize::Serialize(payLoad, op) < 0) {
        return;
    }
    Notify(kFuncIDNotifyOperationCycle, std::move(payLoad));
}

/// @brief Register callback for AA side to get operation cycle status
/// @param callBack Callback function to be registered
/// @return
void OperationCycleProxy::RegisterGetOperationCycle(std::function< ara::diag::OperationCycleType() > const &callBack)
{
    getOperationCycleCb_ = callBack;
}

/// @brief Register callback for AA side to set operation cycle status
/// @param callBack Callback function to be registered
/// @return
void OperationCycleProxy::RegisterSetOperationCycle(
    std::function< void(ara::diag::OperationCycleType const) > const &callBack)
{
    setOperationCycleCb_ = callBack;
}

void OperationCycleProxy::_getOperationCycle(CallRequest &&callData)
{
    if (getOperationCycleCb_ == nullptr) {
        return;
    }

    RetData retData;
    std::vector< uint8_t > buf;
    CallRequest const cr{std::move(callData)};

    uint8_t const operation = static_cast< uint8_t >(getOperationCycleCb_());
    if (serialize::Serialize(buf, operation) > 0) {
        retData.retData = std::move(buf);
    } else {
        retData.errorCode = static_cast< int32_t >(InnerErrorCode::kDataSerialize);
    }
    NetProxy::GetInstance().GetNetAccess()->ReturnResult(cr, std::move(retData));
}

void OperationCycleProxy::_setOperationCycle(CallRequest &&callData)
{
    uint8_t operation{};
    RetData retData;
    std::vector< uint8_t > buf;
    CallRequest const cr{std::move(callData)};
    if (serialize::Deserialize(cr.funcData.data, operation) > 0) {
        if (setOperationCycleCb_ != nullptr) {
            setOperationCycleCb_(static_cast< ara::diag::OperationCycleType >(operation));
        }
    } else {
        retData.errorCode = static_cast< int32_t >(InnerErrorCode::kDataDeserialize);
    }
    NetProxy::GetInstance().GetNetAccess()->ReturnResult(cr, std::move(retData));
}

}  // namespace dis
}  // namespace dm
}  // namespace isoft