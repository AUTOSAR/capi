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
/// @file       tyre_pressure_health_chanel_action.h
/// @brief      HealthChannelAction for tyre pressure health channel.
/// @details
/// @date       2024-06-25
/// @author     lianglongxiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/StateManagement/PHMComm
/// @unit_name=TyrePressureHealthChannelAction
/// @interface_level=uint
/// @unit_description=HealthChannelAction for tyre pressure health channel.
/// @trace_id_sr=SR_SM_08001
/// @endcode
///
/// ================================================================

#ifndef TYRE_PRESSURE_HEALTH_CHANEL_ACTION_H_
#define TYRE_PRESSURE_HEALTH_CHANEL_ACTION_H_

#include <ara/phm/health_channels/tyre_pressure.h>

#include "phm_comm/health_channel_action_impl.h"

namespace ara {
namespace sm {
namespace phm_comm {

/// @brief the Health Statuses
/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
using TyreHealthStatuses = phm::health_channels::tyre_pressure::HealthStatuses;

/// @brief HealthChannelAction for tyre pressure health channel.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_sr=SR_SM_08001
/// @trace_id_ad=AD_SM_08041
/// @trace_id_dd=DD_SM_08110
/// @needwork = ad
/// @endcode
class TyrePressureHealthChannelAction
    : public HealthChannelActionImpl< phm::health_channels::tyre_pressure::HealthStatuses >
{
public:
    /// @brief Use base class constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = no
    /// @endcode
    using HealthChannelActionImpl::HealthChannelActionImpl;

    /// @brief deleted copy constructor function
    /// @param other The TyrePressureHealthChannelAction instance to be copyed
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_08001
    /// @trace_id_ad=AD_SM_00090
    /// @trace_id_dd=DD_SM_00090
    /// @needwork = ad
    /// @endcode
    TyrePressureHealthChannelAction(TyrePressureHealthChannelAction const& other) = delete;

    /// @brief deleted copy assignment function
    /// @param other The TyrePressureHealthChannelAction instance to be copyed
    /// @return the assigned TyrePressureHealthChannelAction instance itself
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_08001
    /// @trace_id_ad=AD_SM_00091
    /// @trace_id_dd=DD_SM_00091
    /// @needwork = ad
    /// @endcode
    TyrePressureHealthChannelAction& operator=(TyrePressureHealthChannelAction const& other) = delete;

    /// @brief Move constructor function
    /// @param other The TyrePressureHealthChannelAction instance to be moved
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_08001
    /// @trace_id_ad=AD_SM_00092
    /// @trace_id_dd=DD_SM_00092
    /// @needwork = ad
    /// @endcode
    TyrePressureHealthChannelAction(TyrePressureHealthChannelAction&& other) = default;

    /// @brief Move assignment function
    /// @param other The TyrePressureHealthChannelAction instance to be moved
    /// @return the assigned TyrePressureHealthChannelAction instance itself
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_08001
    /// @trace_id_ad=AD_SM_00093
    /// @trace_id_dd=DD_SM_00093
    /// @needwork = ad
    /// @endcode
    TyrePressureHealthChannelAction& operator=(TyrePressureHealthChannelAction&& other) = delete;

    /// @brief RecoveryHandler to be invoked by PHM.
    /// @param[in] tyreHealthStatusId The identifier representing the Health Status.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_08001
    /// @trace_id_ad=AD_SM_00094
    /// @trace_id_dd=DD_SM_00094
    /// @needwork = ad
    /// @endcode
    void RecoveryHandler(TyreHealthStatuses tyreHealthStatusId) noexcept override;

    /// @brief Destructor function
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_08001
    /// @trace_id_ad=AD_SM_08042
    /// @trace_id_dd=DD_SM_08111
    /// @needwork = ad
    /// @endcode
    ~TyrePressureHealthChannelAction() noexcept override;

private:
    /// @brief _tyrePressureHealthStatusesToString
    /// @param status the Health Statuses
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_08001
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08112
    /// @needwork = dda
    /// @endcode
    core::String const _tyrePressureHealthStatusesToString(TyreHealthStatuses const status) const noexcept;

    /// @brief Logger instance
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_08001
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08113
    /// @needwork = dda
    /// @endcode
    log::Logger& log_{log::CreateLogger((core::StringView{"#CMG"}), (core::StringView{"PHMCommManager context"}))};
};
}  // namespace phm_comm
}  // namespace sm
}  // namespace ara

#endif  // TYRE_PRESSURE_HEALTH_CHANEL_ACTION_H_
