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
/// @file       trigger_vehicle_announcement_proxy.cpp
/// @brief
/// @details
/// @date       2024-12-27
/// @author     zhanglipeng
/// @version    1.2.0
///
/// ================================================================

#include "trigger_vehicle_announcement_proxy.h"

#include "netProxy/net_proxy.h"
#include "netProxy/rpc_define.h"
#include "serialization/serialization.h"

namespace isoft {
namespace dm {
namespace dis {

constexpr uint8_t kFuncIDTriggerVehicleAnnouncement{1U};

TriggerVehicleAnnouncementProxy::TriggerVehicleAnnouncementProxy(uint32_t const &serviceInstanceId)
    : BussinessServiceProxy{serviceInstanceId}
{
    NetProxy::GetInstance().GetNetAccess()->RegisterService(
        serviceInstanceId, [this](CallRequest &&callData) { CallHandle(std::move(callData)); });
}

void TriggerVehicleAnnouncementProxy::CallHandle(CallRequest &&callData)
{
    CallRequest cr{std::move(callData)};
    switch (cr.funcData.funcId) {
        case kFuncIDTriggerVehicleAnnouncement: {
            _triggerVehicleAnnouncement(std::move(cr));
            break;
        }
        default: {
        } break;
    }
}

/// @brief Register callback for AA side to set condition status
/// @param callBack Callback function to be registered
/// @return
void TriggerVehicleAnnouncementProxy::RegisterTriggerVehicleAnnouncement(
    std::function< int32_t(uint8_t) > const &callBack)
{
    triggerVehicleAnnouncementCb_ = callBack;
}

void TriggerVehicleAnnouncementProxy::_triggerVehicleAnnouncement(CallRequest &&callData)
{
    if (triggerVehicleAnnouncementCb_ == nullptr) {
        return;
    }
    RetData retData;
    std::vector< uint8_t > buf;
    CallRequest const cr{std::move(callData)};

    uint8_t networkId{};
    if (serialize::Deserialize(cr.funcData.data, networkId) > 0) {
        retData.errorCode = triggerVehicleAnnouncementCb_(networkId);
    } else {
        retData.errorCode = static_cast< int32_t >(InnerErrorCode::kDataDeserialize);
    }

    NetProxy::GetInstance().GetNetAccess()->ReturnResult(cr, std::move(retData));
}

}  // namespace dis
}  // namespace dm
}  // namespace isoft