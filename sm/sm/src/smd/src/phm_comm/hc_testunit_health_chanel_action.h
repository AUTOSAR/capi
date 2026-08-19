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
/// @file       hc_testunit_health_chanel_action.h
/// @brief      HealthChannelAction for tyre pressure health channel.
/// @details
/// @date       2024-06-06
/// @author     lianglongxiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/StateManagement/PHMComm
/// @unit_name=HcTestUnitHealthChannelAction
/// @interface_level=uint
/// @unit_description=HealthChannelAction for tyre pressure health channel.
/// @trace_id_sr=SR_SM_08001
/// @endcode
///
/// ================================================================

#ifndef HC_TESTUNIT_HEALTH_CHANEL_ACTION_H_
#define HC_TESTUNIT_HEALTH_CHANEL_ACTION_H_
#include <ara/phm/health_channels/hc_testunit.h>

#include "health_channel_action_impl.h"

namespace ara {
namespace sm {
namespace phm_comm {

/// @brief HealthChannelAction for tyre pressure health channel.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_sr=SR_SM_08001
/// @trace_id_ad=AD_SM_08052
/// @trace_id_dd=DD_SM_08136
/// @needwork = ad
/// @endcode
class HcTestUnitHealthChannelAction
    : public HealthChannelActionImpl< phm::health_channels::hc_testunit::HealthStatuses >
{
public:
    /// @brief Use base class constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = no
    /// @endcode
    using HealthChannelActionImpl::HealthChannelActionImpl;

    /// @brief deleted copy constructor function
    /// @param other The HcTestUnitHealthChannelAction instance to be copyed
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_08001
    /// @trace_id_ad=AD_SM_00126
    /// @trace_id_dd=DD_SM_00126
    /// @needwork = ad
    /// @endcode
    HcTestUnitHealthChannelAction(HcTestUnitHealthChannelAction const& other) = delete;

    /// @brief deleted copy assignment function
    /// @param other The HcTestUnitHealthChannelAction instance to be copyed
    /// @return the assigned HcTestUnitHealthChannelAction instance itself
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_08001
    /// @trace_id_ad=AD_SM_00127
    /// @trace_id_dd=DD_SM_00127
    /// @needwork = ad
    /// @endcode
    HcTestUnitHealthChannelAction& operator=(HcTestUnitHealthChannelAction const& other) = delete;

    /// @brief Move constructor function
    /// @param other The HcTestUnitHealthChannelAction instance to be moved
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_08001
    /// @trace_id_ad=AD_SM_00128
    /// @trace_id_dd=DD_SM_00128
    /// @needwork = ad
    /// @endcode
    HcTestUnitHealthChannelAction(HcTestUnitHealthChannelAction&& other) = default;

    /// @brief Move assignment function
    /// @param other The HcTestUnitHealthChannelAction instance to be moved
    /// @return the assigned HcTestUnitHealthChannelAction instance itself
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_08001
    /// @trace_id_ad=AD_SM_00129
    /// @trace_id_dd=DD_SM_00129
    /// @needwork = ad
    /// @endcode
    HcTestUnitHealthChannelAction& operator=(HcTestUnitHealthChannelAction&& other) = default;

    /// @brief RecoveryHandler called by PHM, implemented on the SM side.
    /// @param healthStatusId Abnormal health channel state
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_08001
    /// @trace_id_ad=AD_SM_00130
    /// @trace_id_dd=DD_SM_00130
    /// @needwork = ad
    /// @endcode
    void RecoveryHandler(phm::health_channels::hc_testunit::HealthStatuses healthStatusId) noexcept override;

    /// @brief Destructor function
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_08001
    /// @trace_id_ad=AD_SM_08053
    /// @trace_id_dd=DD_SM_08137
    /// @needwork = ad
    /// @endcode
    ~HcTestUnitHealthChannelAction() override = default;

private:
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_08001
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08138
    /// @needwork = dda
    /// @endcode
    log::Logger const& log_{
        log::CreateLogger((core::StringView{""}), (core::StringView{"HcTestUnitHealthChannelAction"}))};
};
}  // namespace phm_comm
}  // namespace sm
}  // namespace ara

#endif  // HC_TESTUNIT_HEALTH_CHANEL_ACTION_H_
