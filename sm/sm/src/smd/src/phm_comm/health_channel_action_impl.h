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
/// @file       health_channel_action_impl.h
/// @brief      Inherit PHM's HealthChannelAction, add appendEventHandler member to facilitate event publishing
/// @details
/// @date       2024-06-06
/// @author     lianglongxiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/StateManagement/PHMComm
/// @unit_name=HealthChannelActionImpl
/// @interface_level=uint
/// @unit_description=Inherit PHM's HealthChannelAction, add appendEventHandler member to facilitate event publishing
/// @trace_id_sr=SR_SM_08001
/// @endcode
///
/// ================================================================

#ifndef HEALTH_CHANNEL_ACTION_IMPL_H_
#define HEALTH_CHANNEL_ACTION_IMPL_H_

#include <ara/phm/health_channel_action.h>

#include "define.h"
#include "event.h"

namespace ara {
namespace sm {
namespace phm_comm {

/// @brief HealthChannelAction
/// @code{.isoft}
/// @tparam EnumT
/// @interface_level=none
/// @needwork = no
/// @endcode
template < typename EnumT >
using HealthChannelAction = phm::HealthChannelAction< EnumT >;

/// @brief Inherit PHM's HealthChannelAction, add appendEventHandler member to facilitate event publishing
/// @code{.isoft}
/// @tparam EnumT Enum template parameter
/// @interface_level=unit
/// @trace_id_sr=SR_SM_08001
/// @trace_id_ad=AD_SM_08044
/// @trace_id_dd=DD_SM_08123
/// @needwork = ad
/// @endcode
template < typename EnumT >
class HealthChannelActionImpl : public HealthChannelAction< EnumT >
{
public:
    /// @brief Use base class constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = no
    /// @endcode
    using HealthChannelAction< EnumT >::HealthChannelAction;

    /// @brief Move constructor function
    /// @param other The HealthChannelActionImpl instance to be moved
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_08001
    /// @trace_id_ad=AD_SM_00105
    /// @trace_id_dd=DD_SM_00105
    /// @needwork = ad
    /// @endcode
    HealthChannelActionImpl(HealthChannelActionImpl &&other) noexcept = default;

    /// @brief copy constructor function
    /// @param other The HealthChannelActionImpl instance to be copy
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_08001
    /// @trace_id_ad=AD_SM_00106
    /// @trace_id_dd=DD_SM_00106
    /// @needwork = ad
    /// @endcode
    HealthChannelActionImpl(HealthChannelActionImpl const &other) = delete;

    /// @brief Move assignment function
    /// @param other The HealthChannelActionImpl instance to be moved
    /// @return the assigned HealthChannelActionImpl instance itself
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_08001
    /// @trace_id_ad=AD_SM_00107
    /// @trace_id_dd=DD_SM_00107
    /// @needwork = ad
    /// @endcode
    HealthChannelActionImpl &operator=(HealthChannelActionImpl &&other) noexcept = delete;

    /// @brief copy assignment function
    /// @param other The HealthChannelActionImpl instance to be copy
    /// @return the assigned HealthChannelActionImpl instance itself
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_08001
    /// @trace_id_ad=AD_SM_00108
    /// @trace_id_dd=DD_SM_00108
    /// @needwork = ad
    /// @endcode
    HealthChannelActionImpl &operator=(HealthChannelActionImpl const &other) = delete;

    /// @brief Register the callback function for appending events
    /// @param appendEventHandler Callback function for appending events
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_08001
    /// @trace_id_ad=AD_SM_00109
    /// @trace_id_dd=DD_SM_00109
    /// @needwork = ad
    /// @endcode
    void RegisterAppendEventHandler(std::function< void(common::Event &&) > const &appendEventHandler) noexcept;

    /// @brief Forward RecoveryNotification request
    /// @param[in] type, HealthStatus type of PHM
    /// @param[in] healthStatusId, The identifier representing the Health Status.
    /// @param[in] promise  Request promise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_08001
    /// @trace_id_ad=AD_SM_00110
    /// @trace_id_dd=DD_SM_00110
    /// @needwork = ad
    /// @endcode
    void DeliverRecoveryNotificationRequest(common::PHMHealthStatusType const type,
                                            EnumT const healthStatusId,
                                            ara::core::Promise< void > &&promise) const noexcept;
    /// @brief Destructor function
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_08001
    /// @trace_id_ad=AD_SM_08045
    /// @trace_id_dd=DD_SM_08124
    /// @needwork = ad
    /// @endcode
    ~HealthChannelActionImpl() override = default;

private:
    /// @brief Function handle for publishing events to EventManager
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_08001
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08125
    /// @needwork = dda
    /// @endcode
    std::function< void(common::Event &&) > appendEventHandler_{nullptr};

    /// @brief Logger instance
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_08001
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08126
    /// @needwork = dda
    /// @endcode
    log::Logger &log_{log::CreateLogger((core::StringView{"#CMG"}), (core::StringView{"PHMCommManager context"}))};
};
}  // namespace phm_comm
}  // namespace sm
}  // namespace ara

#endif  // HEALTH_CHANNEL_ACTION_IMPL_H_
