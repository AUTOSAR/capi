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
/// @file       recovery_action_impl.h
/// @brief      Inherit PHM's RecoveryAction, add appendEventHandler member to facilitate event publishing
/// @details
/// @date       2024-05-13
/// @author     lianglongxiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/StateManagement/PHMComm
/// @unit_name=RecoveryActionImpl
/// @interface_level=uint
/// @unit_description=Inherit PHM's RecoveryAction, add appendEventHandler member to facilitate event publishing
/// @trace_id_sr=SR_SM_08001
/// @endcode
///
/// ================================================================

#ifndef RECOVERY_ACTION_IMPL_H_
#define RECOVERY_ACTION_IMPL_H_

#include <ara/phm/recovery_action.h>

#include "event.h"

namespace ara {
namespace sm {
namespace phm_comm {
/// @brief Class name simplification
/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
using RecoveryAction = phm::RecoveryAction;

/// @brief  Inherit PHM's RecoveryAction, add appendEventHandler member to facilitate event publishing
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_sr=SR_SM_08001
/// @trace_id_ad=AD_SM_08054
/// @trace_id_dd=DD_SM_08139
/// @needwork = ad
/// @endcode
class RecoveryActionImpl : public RecoveryAction
{
public:
    /// @brief Use base class constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = no
    /// @endcode
    using RecoveryAction::RecoveryAction;

    /// @brief deleted copy constructor function
    /// @param other The RecoveryActionImpl instance to be copyed
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_08001
    /// @trace_id_ad=AD_SM_00131
    /// @trace_id_dd=DD_SM_00131
    /// @needwork = ad
    /// @endcode
    RecoveryActionImpl(RecoveryActionImpl const &other) = delete;

    /// @brief deleted copy assignment function
    /// @param other The RecoveryActionImpl instance to be copyed
    /// @return the assigned RecoveryActionImpl instance itself
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_08001
    /// @trace_id_ad=AD_SM_00132
    /// @trace_id_dd=DD_SM_00132
    /// @needwork = ad
    /// @endcode
    RecoveryActionImpl &operator=(RecoveryActionImpl const &other) = delete;

    /// @brief Move constructor function
    /// @param other The RecoveryActionImpl instance to be moved
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_08001
    /// @trace_id_ad=AD_SM_00133
    /// @trace_id_dd=DD_SM_00133
    /// @needwork = ad
    /// @endcode
    RecoveryActionImpl(RecoveryActionImpl &&other) = default;

    /// @brief Move assignment function
    /// @param other The RecoveryActionImpl instance to be moved
    /// @return the assigned RecoveryActionImpl instance itself
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_08001
    /// @trace_id_ad=AD_SM_00134
    /// @trace_id_dd=DD_SM_00134
    /// @needwork = ad
    /// @endcode
    RecoveryActionImpl &operator=(RecoveryActionImpl &&other) = delete;

    /// @brief Register the callback function for appending events
    /// @param appendEventHandler Callback function for appending events
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_08001
    /// @trace_id_ad=AD_SM_00135
    /// @trace_id_dd=DD_SM_00135
    /// @needwork = ad
    /// @endcode
    void RegisterAppendEventHandler(std::function< void(common::Event &&) > const &appendEventHandler) noexcept;

    /// @brief Forward recovery notification request
    /// @param executionError Execution error
    /// @param supervision Supervision
    /// @param promise Result storage
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_08001
    /// @trace_id_ad=AD_SM_00136
    /// @trace_id_dd=DD_SM_00136
    /// @needwork = ad
    /// @endcode
    void DeliverRecoveryNotificationRequest(exec::ExecutionErrorEvent const &executionError,
                                            phm::TypeOfSupervision const &supervision,
                                            core::Promise< void > &&promise) noexcept;

    /// @brief Destructor function
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_08001
    /// @trace_id_ad=AD_SM_08055
    /// @trace_id_dd=DD_SM_08140
    /// @needwork = ad
    /// @endcode
    ~RecoveryActionImpl() noexcept override;

protected:
    /// @brief Convert PHM global supervision state to SM internal global supervision state
    /// @param status PHM global supervision state
    /// @return SM internal global supervision state
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_08001
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08141
    /// @needwork = dda
    /// @endcode
    common::PhmGlobalSupervisionStatusInternal _ToInternalType(
        phm::GlobalSupervisionStatus const &status) const noexcept;

    /// @brief Convert PHM supervision type to SM internal supervision type
    /// @param type PHM supervision type
    /// @return SM internal supervision type
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_08001
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08142
    /// @needwork = dda
    /// @endcode
    common::PhmTypeOfSupervisionInternal _ToInternalType(phm::TypeOfSupervision const &type) const noexcept;

private:
    /// @brief Function handle for publishing events to EventManager
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_08001
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08143
    /// @needwork = dda
    /// @endcode
    std::function< void(common::Event &&) > appendEventHandler_{nullptr};

    /// @brief Logger instance
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_08001
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08144
    /// @needwork = dda
    /// @endcode
    log::Logger &log_{log::CreateLogger((core::StringView{"#CMG"}), (core::StringView{"PHMCommManager context"}))};
};
}  // namespace phm_comm
}  // namespace sm
}  // namespace ara

#endif  // RECOVERY_ACTION_IMPL_H_
