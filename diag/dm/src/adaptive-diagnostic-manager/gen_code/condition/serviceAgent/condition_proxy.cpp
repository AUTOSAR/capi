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

#include "netProxy/net_proxy.h"
#include "netProxy/rpc_define.h"
#include "serialization/serialization.h"

namespace isoft {
namespace dm {
namespace dis {

constexpr uint8_t kFuncIDGetCondition{1U};
constexpr uint8_t kFuncIDSetCondition{2U};

ConditionProxy::ConditionProxy(uint32_t const &serviceInstanceId) : BussinessServiceProxy{serviceInstanceId}
{
    NetProxy::GetInstance().GetNetAccess()->RegisterService(
        serviceInstanceId, [this](CallRequest &&callData) { CallHandle(std::move(callData)); });
}

void ConditionProxy::CallHandle(CallRequest &&callData)
{
    CallRequest cr{std::move(callData)};
    switch (cr.funcData.funcId) {
        case kFuncIDGetCondition: {
            _getCondition(std::move(cr));
            break;
        }
        case kFuncIDSetCondition: {
            _setCondition(std::move(cr));
            break;
        }
        default: {
        } break;
    }
}

/// @brief Register callback for AA side to set condition status
/// @param callBack Callback function to be registered
/// @return
void ConditionProxy::RegisterSetCondition(std::function< void(ara::diag::ConditionType) > const &callBack)
{
    setConditionCb_ = callBack;
}

/// @brief Register callback for AA side to get condition status
/// @param callBack Callback function to be registered
/// @return
void ConditionProxy::RegisterGetCondition(std::function< ara::diag::ConditionType() > const &callBack)
{
    getConditionCb_ = callBack;
}

void ConditionProxy::_getCondition(CallRequest &&callData)
{
    if (getConditionCb_ == nullptr) {
        return;
    }
    RetData retData;
    std::vector< uint8_t > buf;
    CallRequest const cr{std::move(callData)};

    uint8_t const condition = static_cast< uint8_t >(getConditionCb_());
    if (serialize::Serialize(buf, condition) > 0) {
        retData.retData = std::move(buf);
    } else {
        retData.errorCode = static_cast< int32_t >(InnerErrorCode::kDataDeserialize);
    }
    NetProxy::GetInstance().GetNetAccess()->ReturnResult(cr, std::move(retData));
}

void ConditionProxy::_setCondition(CallRequest &&callData)
{
    if (setConditionCb_ == nullptr) {
        return;
    }
    uint8_t condition{};
    RetData retData;
    std::vector< uint8_t > buf;
    CallRequest const cr{std::move(callData)};
    if (serialize::Deserialize(cr.funcData.data, condition) > 0) {
        setConditionCb_(static_cast< ara::diag::ConditionType >(condition));
    } else {
        retData.errorCode = static_cast< int32_t >(InnerErrorCode::kDataDeserialize);
    }
    NetProxy::GetInstance().GetNetAccess()->ReturnResult(cr, std::move(retData));
}

}  // namespace dis
}  // namespace dm
}  // namespace isoft