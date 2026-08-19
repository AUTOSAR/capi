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
/// @file       indicator_proxy.cpp
/// @brief
/// @details
/// @date       2024-12-17
/// @author     zhanglipeng
/// @version    1.2.0
///
/// ================================================================

#include "indicator_proxy.h"

#include "netProxy/net_proxy.h"
#include "netProxy/rpc_define.h"
#include "serialization/serialization.h"

namespace isoft {
namespace dm {
namespace dis {

constexpr uint8_t kFuncIDNotifyIndicator{1U};
constexpr uint8_t kFuncIDGetIndicator{2U};

IndicatorProxy::IndicatorProxy(uint32_t const &serviceInstanceId) : BussinessServiceProxy{serviceInstanceId}
{
    NetProxy::GetInstance().GetNetAccess()->RegisterService(
        serviceInstanceId, [this](CallRequest &&callData) { CallHandle(std::move(callData)); });
}

void IndicatorProxy::CallHandle(CallRequest &&callData)
{
    CallRequest cr{std::move(callData)};

    switch (cr.funcData.funcId) {
        case kFuncIDNotifyIndicator: {
            Subscribe(std::move(cr));
            break;
        }
        case kFuncIDGetIndicator: {
            _getIndicator(std::move(cr));
            break;
        }
        default: {
        } break;
    }
}

/// @brief Notify AA side of indicator change
/// @param indicatorType Indicator status
/// @return
void IndicatorProxy::NotifyIndicator(uint8_t const &indicatorType)
{
    std::vector< uint8_t > payLoad;
    if (serialize::Serialize(payLoad, indicatorType) < 0) {
        return;
    }
    Notify(kFuncIDNotifyIndicator, std::move(payLoad));
}

/// @brief Register callback for AA side to get indicator status
/// @param callBack Callback function to be registered
/// @return
void IndicatorProxy::RegisterGetIndicator(std::function< uint8_t() > const &callBack)
{
    getIndicatorTypeCb_ = callBack;
}

void IndicatorProxy::_getIndicator(CallRequest &&callData)
{
    if (getIndicatorTypeCb_ == nullptr) {
        return;
    }
    RetData retData;
    std::vector< uint8_t > buf;
    CallRequest const cr{std::move(callData)};

    uint8_t const type = getIndicatorTypeCb_();
    if (serialize::Serialize(buf, type) > 0) {
        retData.retData = std::move(buf);
    } else {
        retData.errorCode = static_cast< int32_t >(InnerErrorCode::kDataSerialize);
    }
    NetProxy::GetInstance().GetNetAccess()->ReturnResult(cr, std::move(retData));
}

}  // namespace dis
}  // namespace dm
}  // namespace isoft