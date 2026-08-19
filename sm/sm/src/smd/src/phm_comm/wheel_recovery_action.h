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
/// @file       wheel_recovery_action.h
/// @brief      RecoveryAction for wheel supervision.
/// @details
/// @date       2024-06-25
/// @author     lianglongxiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/StateManagement/PHMComm
/// @unit_name=WheelRecoveryAction
/// @interface_level=uint
/// @unit_description=RecoveryAction for wheel supervision.
/// @trace_id_sr=SR_SM_08001
/// @endcode
///
/// ================================================================

#ifndef WHEEL_RECOVERY_ACTION_H_
#define WHEEL_RECOVERY_ACTION_H_

#include "recovery_action_impl.h"

namespace ara {
namespace sm {
namespace phm_comm {
/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
using TypeOfSupervision = ara::phm::TypeOfSupervision;

/// @brief RecoveryAction for wheel supervision.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_sr=SR_SM_08001
/// @trace_id_ad=AD_SM_08050
/// @trace_id_dd=DD_SM_08133
/// @needwork = ad
/// @endcode
class WheelRecoveryAction : public RecoveryActionImpl
{
public:
    /// @brief Use base class constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = no
    /// @endcode
    using RecoveryActionImpl::RecoveryActionImpl;

    /// @brief deleted copy constructor function
    /// @param other The WheelRecoveryAction instance to be copyed
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_08001
    /// @trace_id_ad=AD_SM_00121
    /// @trace_id_dd=DD_SM_00121
    /// @needwork = ad
    /// @endcode
    WheelRecoveryAction(WheelRecoveryAction const& other) = delete;

    /// @brief deleted copy assignment function
    /// @param other The WheelRecoveryAction instance to be copyed
    /// @return the assigned WheelRecoveryAction instance itself
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_08001
    /// @trace_id_ad=AD_SM_00122
    /// @trace_id_dd=DD_SM_00122
    /// @needwork = ad
    /// @endcode
    WheelRecoveryAction& operator=(WheelRecoveryAction const& other) = delete;

    /// @brief Move constructor function
    /// @param other The WheelRecoveryAction instance to be moved
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_08001
    /// @trace_id_ad=AD_SM_00123
    /// @trace_id_dd=DD_SM_00123
    /// @needwork = ad
    /// @endcode
    WheelRecoveryAction(WheelRecoveryAction&& other) = default;

    /// @brief Move assignment function
    /// @param other The WheelRecoveryAction instance to be moved
    /// @return the assigned WheelRecoveryAction instance itself
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_08001
    /// @trace_id_ad=AD_SM_00124
    /// @trace_id_dd=DD_SM_00124
    /// @needwork = ad
    /// @endcode
    WheelRecoveryAction& operator=(WheelRecoveryAction&& other) = delete;

    /// @brief RecoveryHandler to be invoked by PHM.
    /// @param executionError Information on detected error, shall give further information for error recovery.
    /// @param supervision The type of local supervision which failed
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_08001
    /// @trace_id_ad=AD_SM_00125
    /// @trace_id_dd=DD_SM_00125
    /// @needwork = ad
    /// @endcode
    void RecoveryHandler(ara::exec::ExecutionErrorEvent const& executionError,
                         TypeOfSupervision supervision) noexcept override;

    /// @brief Destructor function
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_08001
    /// @trace_id_ad=AD_SM_08051
    /// @trace_id_dd=DD_SM_08134
    /// @needwork = ad
    /// @endcode
    ~WheelRecoveryAction() noexcept override;

private:
    /// @brief Logger instance
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_08001
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08135
    /// @needwork = dda
    /// @endcode
    log::Logger& log_{log::CreateLogger((core::StringView{"#CMG"}), (core::StringView{"PHMCommManager context"}))};
};
}  // namespace phm_comm
}  // namespace sm
}  // namespace ara

#endif  // WHEEL_RECOVERY_ACTION_H_
