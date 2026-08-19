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
/// @file       doip_trigger_vehicle_announcement.h
/// @brief      This file provides the definitions of DoIPTriggerVehicleAnnouncement and related types.
/// @details
/// @date       2021-11-23
/// @author     jiawei
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_DIAG_DOIP_TRIGGER_VEHICLE_ANNOUNCEMENT_H_
#define ARA_DIAG_DOIP_TRIGGER_VEHICLE_ANNOUNCEMENT_H_

#include <ara/core/result.h>

#include <atomic>
#include <cstdint>
#include <memory>

#include "ara/core/promise.h"

namespace isoft {
namespace dm {
namespace dic {
class TriggerVehicleAnnouncementAgent;
}  // namespace dic
}  // namespace dm
}  // namespace isoft

namespace ara {
namespace diag {

/// @brief DiagnosticDoIPTriggerVehicleAnnouncement
///
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
///
/// @traceid{SWS_DM_00820}@tracestatus{draft}
class DoIPTriggerVehicleAnnouncement
{
public:
    /// @brief Get DoIPTriggerVehicleAnnouncement interface from DM.
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @return DoIPTriggerVehicleAnnouncement object
    ///
    ///
    /// @traceid{SWS_DM_00821}@tracestatus{draft}
    static ara::core::Result< DoIPTriggerVehicleAnnouncement & > GetDoIPTriggerVehicleAnnouncement() noexcept;

    /// @brief Called by application to trigger DM sending out vehicle announcements on the given network interface Id.
    /// @note If the reported DoIpNetworkConfiguration.networkInterfaceId belongs to a DoIpNetworkConfiguration
    /// with property isActivationLineDependent = 'TRUE', this is an error as on those interfaces sending of
    /// announcements happens automatically after activation line going up/ip address assignment.
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[in] networkInterfaceId
    /// @return ara::core::Result<void>
    ///
    ///
    /// @traceid{SWS_DM_00822}@tracestatus{draft}
    virtual ara::core::Result< void > TriggerVehicleAnnouncement(std::uint8_t networkInterfaceId) noexcept;

    /// @brief Constructor of DoIPTriggerVehicleAnnouncement
    ///
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    DoIPTriggerVehicleAnnouncement(DoIPTriggerVehicleAnnouncement const &) = delete;

    /// @brief Constructor of DoIPTriggerVehicleAnnouncement
    ///
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    DoIPTriggerVehicleAnnouncement(DoIPTriggerVehicleAnnouncement &&) = delete;

    /// @brief Constructor of DoIPTriggerVehicleAnnouncement
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @return DoIPTriggerVehicleAnnouncement&
    ///
    DoIPTriggerVehicleAnnouncement &operator=(DoIPTriggerVehicleAnnouncement const &) = delete;

    /// @brief move Constructor of DoIPTriggerVehicleAnnouncement
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @return DoIPTriggerVehicleAnnouncement&
    ///
    DoIPTriggerVehicleAnnouncement &operator=(DoIPTriggerVehicleAnnouncement &&) = delete;

private:
    /// @brief Ctor is vendor-specific
    ///
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    ///
    /// @traceid{SWS_DM_00823}@tracestatus{draft}
    DoIPTriggerVehicleAnnouncement() noexcept;

    /// @brief Dtor is vendor-specific
    ///
    /// @traceid{SWS_DM_00824}@tracestatus{draft}
    ~DoIPTriggerVehicleAnnouncement() noexcept = default;

    std::shared_ptr< isoft::dm::dic::TriggerVehicleAnnouncementAgent >
        /// @name proxy_
        proxy_;

    /// @brief Synchronize
    ara::core::Promise< bool > promise_;

    std::atomic_bool isInit_{false};
};

}  // namespace diag
}  // namespace ara

#endif  // ARA_DIAG_DOIP_TRIGGER_VEHICLE_ANNOUNCEMENT_H_