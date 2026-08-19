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
/// @file       bussiness_service_proxy.h
/// @brief
/// @details
/// @date       2024-11-28
/// @author     xuhua
/// @version    1.2.0
///
/// ================================================================

#ifndef __BUSSINESS_SERVER_PROXY__
#define __BUSSINESS_SERVER_PROXY__

#include <functional>
#include <mutex>
#include <set>
#include <unordered_map>

#include "rpc_define.h"

namespace isoft {
namespace dm {
namespace dis {

struct CallDataCmp
{
    bool operator()(const CallRequest &lhs, const CallRequest &rhs) const
    {
        return (lhs.callContext->clientId > rhs.callContext->clientId);
    }
};

using CallDataSet = std::set< CallRequest, CallDataCmp >;

class BussinessServiceProxy
{
public:
    explicit BussinessServiceProxy(uint32_t const &serviceInstanceId);

    virtual ~BussinessServiceProxy();
    BussinessServiceProxy(const BussinessServiceProxy &other)  = delete;
    BussinessServiceProxy(const BussinessServiceProxy &&other) = delete;
    BussinessServiceProxy &operator=(const BussinessServiceProxy &other) = delete;
    BussinessServiceProxy &operator=(BussinessServiceProxy const &&other) = delete;

    virtual void CallHandle(CallRequest &&callData) = 0;

    void Subscribe(CallRequest &&callData);
    void UnSubscribe(CallRequest const &callData);
    void ClearSubscribe(void *callContext);  // Clear subscription for the specified call context, clear all if empty
    std::vector< CallRequest > GetSubscribe(uint8_t funcId);
    void Notify(uint8_t funcId, std::vector< uint8_t > &&data);
    void RegisterOnCancel(uint8_t const &funcId,
                          std::uint8_t const &callId,
                          std::function< void() > const &notifier) const;
    void UnregisterOnCancel(uint8_t const &funcId, std::uint8_t const &callId) const;

    void FullyReentrancy(bool enable) noexcept;

private:
    // funcId -> subscribers
    std::unordered_map< std::uint8_t, CallDataSet > funcSubscribers_{};
    std::set< std::uint8_t > processFunc_{};
    std::mutex processFuncLaock_{};
    std::mutex mMtxFuncSubscribers_{};
    bool enableFullyReentrancy_{true};
    uint32_t mServiceInstanceId_{0U};
};

}  // namespace dis
}  // namespace dm
}  // namespace isoft

#endif  // __BUSSINESS_SERVER_PROXY__