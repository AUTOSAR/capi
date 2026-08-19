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
/// @file       engine_recovery_action.h
/// @brief      RecoveryAction for engine supervision.
/// @details
/// @date       2024-06-06
/// @author     lianglongxiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/StateManagement/PHMComm
/// @unit_name=EngineRecoveryAction
/// @interface_level=uint
/// @unit_description=RecoveryAction for engine supervision.
/// @trace_id_sr=SR_SM_08001
/// @endcode
///
/// ================================================================

#ifndef ENGINE_RECOVERY_ACTION_H_
#define ENGINE_RECOVERY_ACTION_H_

#include "phm_comm/recovery_action_impl.h"

namespace ara {
namespace sm {
namespace phm_comm {

/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
using TypeOfSupervision = phm::TypeOfSupervision;

/// @brief RecoveryAction for engine supervision.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_sr=SR_SM_08001
/// @trace_id_ad=AD_SM_08039
/// @trace_id_dd=DD_SM_08107
/// @needwork = ad
/// @endcode
class EngineRecoveryAction : public RecoveryActionImpl
{
public:
    /// @brief Use base class constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = no
    /// @endcode
    using RecoveryActionImpl::RecoveryActionImpl;

    /// @brief deleted copy constructor function
    /// @param other The EngineRecoveryAction instance to be copyed
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_08001
    /// @trace_id_ad=AD_SM_00085
    /// @trace_id_dd=DD_SM_00085
    /// @needwork = ad
    /// @endcode
    EngineRecoveryAction(EngineRecoveryAction const& other) = delete;

    /// @brief deleted copy assignment function
    /// @param other The EngineRecoveryAction instance to be copyed
    /// @return the assigned EngineRecoveryAction instance itself
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_08001
    /// @trace_id_ad=AD_SM_00086
    /// @trace_id_dd=DD_SM_00086
    /// @needwork = ad
    /// @endcode
    EngineRecoveryAction& operator=(EngineRecoveryAction const& other) = delete;

    /// @brief Move constructor function
    /// @param other The EngineRecoveryAction instance to be moved
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_08001
    /// @trace_id_ad=AD_SM_00087
    /// @trace_id_dd=DD_SM_00087
    /// @needwork = ad
    /// @endcode
    EngineRecoveryAction(EngineRecoveryAction&& other) = default;

    /// @brief Move assignment function
    /// @param other The EngineRecoveryAction instance to be moved
    /// @return the assigned EngineRecoveryAction instance itself
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_08001
    /// @trace_id_ad=AD_SM_00088
    /// @trace_id_dd=DD_SM_00088
    /// @needwork = ad
    /// @endcode
    EngineRecoveryAction& operator=(EngineRecoveryAction&& other) = delete;

    /// @brief RecoveryHandler to be invoked by PHM.
    /// @param[in] executionError Information on detected error, shall give further information for error recovery.
    /// @param[in] supervision The type of local supervision which failed
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_08001
    /// @trace_id_ad=AD_SM_00089
    /// @trace_id_dd=DD_SM_00089
    /// @needwork = ad
    /// @endcode
    void RecoveryHandler(ara::exec::ExecutionErrorEvent const& executionError,
                         TypeOfSupervision supervision) noexcept override;

    /// @brief Destructor function
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_08001
    /// @trace_id_ad=AD_SM_08040
    /// @trace_id_dd=DD_SM_08108
    /// @needwork = ad
    /// @endcode
    ~EngineRecoveryAction() override = default;

private:
    /// @brief Logger instance
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_08001
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08109
    /// @needwork = dda
    /// @endcode
    log::Logger& log_{log::CreateLogger((core::StringView{"#CMG"}), (core::StringView{"PHMCommManager context"}))};
};

}  // namespace phm_comm
}  // namespace sm
}  // namespace ara

#endif  // ENGINE_RECOVERY_ACTION_H_
