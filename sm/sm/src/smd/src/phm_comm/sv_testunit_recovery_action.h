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
/// @file       sv_testunit_recovery_action.h
/// @brief      RecoveryAction for sv_testunit supervision.
/// @details
/// @date       2024-06-06
/// @author     lianglongxiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/StateManagement/PHMComm
/// @unit_name=SvTestUnitRecoveryAction
/// @interface_level=uint
/// @unit_description=RecoveryAction for sv_testunit supervision.
/// @trace_id_sr=SR_SM_08001
/// @endcode
///
/// ================================================================

#ifndef SV_TESTUNIT_RECOVERY_ACTION_H_
#define SV_TESTUNIT_RECOVERY_ACTION_H_

#include "recovery_action_impl.h"

namespace ara {
namespace sm {
namespace phm_comm {

/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
using TypeOfSupervision = phm::TypeOfSupervision;

/// @brief RecoveryAction for sv_testunit supervision.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_sr=SR_SM_08001
/// @trace_id_ad=AD_SM_08046
/// @trace_id_dd=DD_SM_08127
/// @needwork = ad
/// @endcode
class SvTestUnitRecoveryAction : public RecoveryActionImpl
{
public:
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = no
    /// @endcode
    using RecoveryActionImpl::RecoveryActionImpl;

    /// @brief deleted copy constructor function
    /// @param other The SvTestUnitRecoveryAction instance to be copyed
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_08001
    /// @trace_id_ad=AD_SM_00111
    /// @trace_id_dd=DD_SM_00111
    /// @needwork = ad
    /// @endcode
    SvTestUnitRecoveryAction(SvTestUnitRecoveryAction const& other) = delete;

    /// @brief deleted copy assignment function
    /// @param other The SvTestUnitRecoveryAction instance to be copyed
    /// @return the assigned SvTestUnitRecoveryAction instance itself
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_08001
    /// @trace_id_ad=AD_SM_00112
    /// @trace_id_dd=DD_SM_00112
    /// @needwork = ad
    /// @endcode
    SvTestUnitRecoveryAction& operator=(SvTestUnitRecoveryAction const& other) = delete;

    /// @brief Move constructor function
    /// @param other The SvTestUnitRecoveryAction instance to be moved
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_08001
    /// @trace_id_ad=AD_SM_00113
    /// @trace_id_dd=DD_SM_00113
    /// @needwork = ad
    /// @endcode
    SvTestUnitRecoveryAction(SvTestUnitRecoveryAction&& other) = default;

    /// @brief Move assignment function
    /// @param other The SvTestUnitRecoveryAction instance to be moved
    /// @return the assigned SvTestUnitRecoveryAction instance itself
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_08001
    /// @trace_id_ad=AD_SM_00114
    /// @trace_id_dd=DD_SM_00114
    /// @needwork = ad
    /// @endcode
    SvTestUnitRecoveryAction& operator=(SvTestUnitRecoveryAction&& other) = default;

    /// @brief RecoveryHandler to be invoked by PHM.
    /// @param[in] executionError Information on detected error, shall give further information for error recovery.
    /// @param[in] supervision The type of local supervision which failed
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_08001
    /// @trace_id_ad=AD_SM_00115
    /// @trace_id_dd=DD_SM_00115
    /// @needwork = ad
    /// @endcode
    void RecoveryHandler(ara::exec::ExecutionErrorEvent const& executionError,
                         TypeOfSupervision supervision) noexcept override;

    /// @brief Destructor function
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_08001
    /// @trace_id_ad=AD_SM_08047
    /// @trace_id_dd=DD_SM_08128
    /// @needwork = ad
    /// @endcode
    ~SvTestUnitRecoveryAction() noexcept override;

private:
    /// @brief Logger instance
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_08001
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08129
    /// @needwork = dda
    /// @endcode
    log::Logger& log_{log::CreateLogger((core::StringView{"#CMG"}), (core::StringView{"PHMCommManager context"}))};
};
}  // namespace phm_comm
}  // namespace sm
}  // namespace ara

#endif  // SV_TESTUNIT_RECOVERY_ACTION_H_
