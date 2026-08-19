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
/// @file       activation_line_proxy.cpp
/// @brief
/// @details
/// @date       2024-12-27
/// @author     zhanglipeng
/// @version    1.2.0
///
/// ================================================================

#include "activation_line_proxy.h"

#include "log/log.h"

namespace isoft {
namespace dm {
namespace dic {

constexpr uint8_t kFuncIDGetNetworkInterfaceId{1U};
constexpr uint8_t kFuncIDGetActivationLineState{2U};
constexpr uint8_t kFuncIDUpdateActivationLineState{3U};

ActivationLineProxy::ActivationLineProxy(uint16_t const& instanceId, uint32_t const& serviceInstanceId)
    : BussinessClientProxy{serviceInstanceId, instanceId}
{
    RegisterNotificationCallBack([this](FuncData const& funcData) { _notify(funcData); });
}

ara::core::Result< std::uint8_t > ActivationLineProxy::GetNetworkInterfaceId()
{
    static uint8_t s_CallId{0U};
    ara::core::Result< std::uint8_t > result{SyncCallFunc< std::uint8_t >(kFuncIDGetNetworkInterfaceId, s_CallId)};
    return result;
}

ara::core::Result< bool > ActivationLineProxy::GetActivationLineState()
{
    static uint8_t s_CallId{0U};
    ara::core::Result< bool > result{SyncCallFunc< bool >(kFuncIDGetActivationLineState, s_CallId)};
    return result;
}

void ActivationLineProxy::RegisterActivationLineStateNotifier(std::function< void(bool) > const& notifier)
{
    static uint8_t s_CallId{0U};
    ara::core::Result< void > result{RegisterCallBack(kFuncIDUpdateActivationLineState, s_CallId)};
    if (!result.HasValue()) {
        ara::diag::common::LogError() << "ActivationLineProxy::RegisterActivationLineStateNotifier|register error";
        return;
    }

    activationLineStateCb_ = notifier;
}

void ActivationLineProxy::_notify(FuncData const& funcData) const
{
    switch (static_cast< int >(funcData.funcId)) {
        case kFuncIDUpdateActivationLineState: {
            if (nullptr != activationLineStateCb_) {
                bool state{};
                if (serialize::Deserialize(funcData.data, state) > 0) {
                    activationLineStateCb_(state);
                }
            } else {
                ara::diag::common::LogError() << "ActivationLineProxy::_notify|activationLineStateCb_ is nullptr";
            }
        } break;
        default: {
        } break;
    }
}

void ActivationLineProxy::RegisterOnReady(std::function< void(bool) > const& callback) noexcept
{
    RegisterOnServiceReady(callback);
}

}  // namespace dic
}  // namespace dm
}  // namespace isoft
