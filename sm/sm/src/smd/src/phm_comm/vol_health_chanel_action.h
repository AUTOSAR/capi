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
/// @file       vol_health_chanel_action.h
/// @brief      HealthChannelAction for voltage health channel.
/// @details
/// @date       2024-06-25
/// @author     lianglongxiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/StateManagement/PHMComm
/// @unit_name=VolHealthChannelAction
/// @interface_level=uint
/// @unit_description=HealthChannelAction for voltage health channel.
/// @trace_id_sr=SR_SM_08001
/// @endcode
///
/// ================================================================

#ifndef VOL_HEALTH_CHANEL_ACTION_H_
#define VOL_HEALTH_CHANEL_ACTION_H_

#include <ara/phm/health_channels/vol.h>

#include "phm_comm/health_channel_action_impl.h"

namespace ara {
namespace sm {
namespace phm_comm {

/// @brief the Health Statuses
/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
using VolHealthStatuses = phm::health_channels::vol::HealthStatuses;

/// @brief HealthChannelAction for voltage health channel.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_sr=SR_SM_08001
/// @trace_id_ad=AD_SM_08048
/// @trace_id_dd=DD_SM_08130
/// @needwork = ad
/// @endcode
class VolHealthChannelAction : public HealthChannelActionImpl< phm::health_channels::vol::HealthStatuses >
{
public:
    /// @brief Use base class constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = no
    /// @endcode
    using HealthChannelActionImpl::HealthChannelActionImpl;

    /// @brief deleted copy constructor function
    /// @param other The VolHealthChannelAction instance to be copyed
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_08001
    /// @trace_id_ad=AD_SM_00116
    /// @trace_id_dd=DD_SM_00116
    /// @needwork = ad
    /// @endcode
    VolHealthChannelAction(VolHealthChannelAction const& other) = delete;

    /// @brief deleted copy assignment function
    /// @param other The VolHealthChannelAction instance to be copyed
    /// @return the assigned VolHealthChannelAction instance itself
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_08001
    /// @trace_id_ad=AD_SM_00117
    /// @trace_id_dd=DD_SM_00117
    /// @needwork = ad
    /// @endcode
    VolHealthChannelAction& operator=(VolHealthChannelAction const& other) = delete;

    /// @brief Move constructor function
    /// @param other The VolHealthChannelAction instance to be moved
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_08001
    /// @trace_id_ad=AD_SM_00118
    /// @trace_id_dd=DD_SM_00118
    /// @needwork = ad
    /// @endcode
    VolHealthChannelAction(VolHealthChannelAction&& other) = default;

    /// @brief Move assignment function
    /// @param other The VolHealthChannelAction instance to be moved
    /// @return the assigned VolHealthChannelAction instance itself
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_08001
    /// @trace_id_ad=AD_SM_00119
    /// @trace_id_dd=DD_SM_00119
    /// @needwork = ad
    /// @endcode
    VolHealthChannelAction& operator=(VolHealthChannelAction&& other) = delete;

    /// @brief RecoveryHandler to be invoked by PHM.
    /// @param volHealthStatusId The identifier representing the Health Status.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_08001
    /// @trace_id_ad=AD_SM_00120
    /// @trace_id_dd=DD_SM_00120
    /// @needwork = ad
    /// @endcode
    void RecoveryHandler(VolHealthStatuses volHealthStatusId) noexcept override;

    /// @brief Destructor function
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_08001
    /// @trace_id_ad=AD_SM_08049
    /// @trace_id_dd=DD_SM_08131
    /// @needwork = ad
    /// @endcode
    ~VolHealthChannelAction() noexcept override;

private:
    /// @brief _volHealthStatusesToString
    /// @param status the Health Statuses
    /// @return string
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_08001
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08132
    /// @needwork = dda
    /// @endcode
    core::String const _volHealthStatusesToString(VolHealthStatuses const status) const noexcept;

    /// @brief Logger instance
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_08001
    /// @interface_level=none
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08497
    /// @needwork = dda
    /// @endcode
    log::Logger& log_{log::CreateLogger((core::StringView{"#CMG"}), (core::StringView{"PHMCommManager context"}))};
};
}  // namespace phm_comm
}  // namespace sm
}  // namespace ara

#endif  // VOL_HEALTH_CHANEL_ACTION_H_
