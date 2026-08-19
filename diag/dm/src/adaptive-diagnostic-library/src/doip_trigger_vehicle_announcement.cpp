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
/// @file       doip_trigger_vehicle_announcement.cpp
/// @brief      This file provides the implementation of DoIPTriggerVehicleAnnouncement.
/// @details
/// @date       2022-10-14
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================

#include "ara/diag/doip_trigger_vehicle_announcement.h"

#include "ara/diag/diag_error_domain.h"
#include "gen_code/trigger_vehicle_announcement/clientAgent/trigger_vehicle_announcement_agent.h"
#include "resolve.h"
#include "utility.h"
namespace ara {
namespace diag {
/// @brief Get DoIPTriggerVehicleAnnouncement interface from DM.
/// @return DoIPTriggerVehicleAnnouncement object
///
/// @traceid{SWS_DM_00821}@tracestatus{draft}
ara::core::Result< DoIPTriggerVehicleAnnouncement& >
DoIPTriggerVehicleAnnouncement::GetDoIPTriggerVehicleAnnouncement() noexcept
{
    static DoIPTriggerVehicleAnnouncement s_Announcement;
    return ara::core::Result< DoIPTriggerVehicleAnnouncement& >::FromValue(s_Announcement);
}
/// @brief Ctor is vendor-specific
///
/// @traceid{SWS_DM_00823}@tracestatus{draft}
DoIPTriggerVehicleAnnouncement::DoIPTriggerVehicleAnnouncement() noexcept
{
    auto instanceSpecifer
        = ara::core::InstanceSpecifier::Create("DefaultDoIpTriggerVehicleAnnouncementInstanceSpecifier");
    ara::core::Result< internal::InstanceInfo > const retrieveResult{internal::Resolve(instanceSpecifer.Value())};
    if (retrieveResult.HasValue()) {
        proxy_ = std::make_shared< isoft::dm::dic::TriggerVehicleAnnouncementAgent >(
            retrieveResult.Value().instanceId, retrieveResult.Value().serviceInstanceId);
        ara::core::Future< bool > const future{promise_.get_future()};
        proxy_->RegisterOnServiceReady([this](bool ready) {
            internal::LogDebug() << "DoIPTriggerVehicleAnnouncement::DoIPTriggerVehicleAnnouncement|service is "
                                    "ready, callback is called, ready ="
                                 << ready << "isInit =" << isInit_.load();
            if (!ready) {
                return;
            }

            if (isInit_.load()) {
                return;
            }

            promise_.set_value(true);
            isInit_.store(true);

            internal::LogDebug() << "DoIPTriggerVehicleAnnouncement::DoIPTriggerVehicleAnnouncement|service is "
                                    "ready, callback is called, finish init";
        });

        future.wait();
        internal::LogDebug()
            << "DoIPTriggerVehicleAnnouncement::DoIPTriggerVehicleAnnouncement|wait init finish, return result to aa";
    } else {
        internal::LogError() << "DoIPTriggerVehicleAnnouncement::DoIPTriggerVehicleAnnouncement|not found specifier";
    }
}
/// @brief Called by application to trigger DM sending out vehicle announcements on the given network interface Id.
/// @note If the reported DoIpNetworkConfiguration.networkInterfaceId belongs to a DoIpNetworkConfiguration
/// with property isActivationLineDependent = 'TRUE', this is an error as on those interfaces sending of
/// announcements happens automatically after activation line going up/ip address assignment.
/// @param networkInterfaceId
/// @return ara::core::Result<void>
///
/// @traceid{SWS_DM_00822}@tracestatus{draft}
ara::core::Result< void > DoIPTriggerVehicleAnnouncement::TriggerVehicleAnnouncement(
    std::uint8_t networkInterfaceId) noexcept
{
    if (proxy_.get() == nullptr) {
        return ara::core::Result< void >::FromError(DiagErrc::kFailed);
    }

    return proxy_->TriggerVehicleAnnouncement(networkInterfaceId);
}

}  // namespace diag
}  // namespace ara
