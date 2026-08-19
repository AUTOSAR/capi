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
/// @file       bussiness_service_proxy.cpp
/// @brief
/// @details
/// @date       2024-11-28
/// @author     xuhua
/// @version    1.2.0
///
/// ================================================================

#include "netProxy/bussiness_service_proxy.h"

#include "netProxy/net_proxy.h"
#include "serialization/serialization/common_data_type.h"

namespace isoft {
namespace dm {
namespace dis {

BussinessServiceProxy::BussinessServiceProxy(uint32_t const &serviceInstanceId) : mServiceInstanceId_{serviceInstanceId}
{
    NetProxy::GetInstance().GetNetAccess()->RegisterService(serviceInstanceId, [this](CallRequest callData) {
        std::uint8_t funcId{callData.funcData.funcId};
        if (!enableFullyReentrancy_) {
            std::unique_lock< std::mutex > lock{processFuncLaock_};
            decltype(auto) findResult = processFunc_.find(funcId);
            if (findResult != processFunc_.end()) {
                RetData retData;
                retData.errorCode = isoft::serialize::kInt32_0x10;
                isoft::dm::NetProxy::GetInstance().GetNetAccess()->ReturnResult(callData, std::move(retData));
                return;
            }
            processFunc_.insert(funcId);
        }
        this->CallHandle(std::move(callData));
        if (!enableFullyReentrancy_) {
            std::unique_lock< std::mutex > lock{processFuncLaock_};
            decltype(auto) findResult = processFunc_.find(funcId);
            if (findResult != processFunc_.end()) {
                processFunc_.erase(funcId);
            }
        }
    });
}

BussinessServiceProxy::~BussinessServiceProxy()
{
    NetProxy::GetInstance().GetNetAccess()->UnregisterService(mServiceInstanceId_);
}

void BussinessServiceProxy::Subscribe(CallRequest &&callData)
{
    std::lock_guard< std::mutex > lock(mMtxFuncSubscribers_);
    funcSubscribers_[callData.funcData.funcId].emplace(callData);
    RetData retData;
    NetProxy::GetInstance().GetNetAccess()->ReturnResult(callData, std::move(retData));
}

void BussinessServiceProxy::UnSubscribe(CallRequest const &callData)
{
    std::lock_guard< std::mutex > lock(mMtxFuncSubscribers_);
    std::ignore = callData;
}

std::vector< CallRequest > BussinessServiceProxy::GetSubscribe(uint8_t funcId)
{
    std::lock_guard< std::mutex > lock(mMtxFuncSubscribers_);
    std::vector< CallRequest > ret;
    auto iter = funcSubscribers_.find(funcId);
    if (iter != funcSubscribers_.end()) {
        for (auto const &item : funcSubscribers_[funcId]) {
            ret.push_back(item);
        }
    }
    return ret;
}

void BussinessServiceProxy::ClearSubscribe(void *callContext)
{
    std::lock_guard< std::mutex > lock(mMtxFuncSubscribers_);
    std::ignore = callContext;
}

void BussinessServiceProxy::Notify(uint8_t funcId, std::vector< uint8_t > &&data)
{
    FuncData funcData;
    funcData.funcId   = funcId;
    funcData.funcType = FuncType::kFuncType_notify;
    funcData.data     = data;
    auto callDataList = GetSubscribe(funcId);
    for (CallRequest &callData : callDataList) {
        funcData.callId   = callData.funcData.callId + 1;
        callData.funcData = funcData;
        // Notify
        NetProxy::GetInstance().GetNetAccess()->Notify(callData);
    }
}

void BussinessServiceProxy::RegisterOnCancel(uint8_t const &funcId,
                                             std::uint8_t const &callId,
                                             std::function< void() > const &notifier) const
{
    NetProxy::GetInstance().GetNetAccess()->RegisterOnCancel(mServiceInstanceId_, funcId, callId, notifier);
}
void BussinessServiceProxy::UnregisterOnCancel(uint8_t const &funcId, std::uint8_t const &callId) const
{
    NetProxy::GetInstance().GetNetAccess()->UnregisterOnCancel(mServiceInstanceId_, funcId, callId);
}

void BussinessServiceProxy::FullyReentrancy(bool enable) noexcept { enableFullyReentrancy_ = enable; }

}  // namespace dis
}  // namespace dm
}  // namespace isoft