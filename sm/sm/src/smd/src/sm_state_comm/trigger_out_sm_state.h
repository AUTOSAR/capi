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
/// @file       trigger_out_sm_state.h
/// @brief      A implementation of ara::sm::skeleton::TriggerOut_StateMachineSkeleton.
/// @details
/// @date       2024-05-07
/// @author     lianglongxiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/StateManagement/SMStateComm
/// @unit_name=TriggerOutSMState
/// @interface_level=uint
/// @unit_description=A implementation of ara::sm::skeleton::TriggerOut_StateMachineSkeleton.
/// @trace_id_sr=SR_SM_09008
/// @endcode
///
/// ================================================================

#ifndef TRIGGER_OUT_SM_STATE_H_
#define TRIGGER_OUT_SM_STATE_H_

#include <ara/sm/triggerout_statemachine_skeleton.h>

namespace ara {
namespace sm {
namespace sm_state_comm {

/// @brief A implementation of ara::sm::skeleton::TriggerOut_StateMachineSkeleton.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_sr=SR_SM_09008
/// @trace_id_ad=AD_SM_08090
/// @trace_id_dd=DD_SM_08320
/// @needwork = ad
/// @endcode
class TriggerOutSMState : public ara::sm::skeleton::TriggerOut_StateMachineSkeleton
{
public:
    /// @brief Abstract skeleton class which is implemented here.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = no
    /// @endcode
    using Skeleton = ara::sm::skeleton::TriggerOut_StateMachineSkeleton;

    /// @brief Use base class constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = no
    /// @endcode
    using Skeleton::Skeleton;

    /// @brief Destructor function
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_09008
    /// @trace_id_ad=AD_SM_00320
    /// @trace_id_dd=DD_SM_00336
    /// @needwork = ad
    /// @endcode
    ~TriggerOutSMState() final = default;

    /// @brief deleted copy constructor function
    /// @param other The TriggerOutSMState instance to be copyed
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_09008
    /// @trace_id_ad=AD_SM_00321
    /// @trace_id_dd=DD_SM_00337
    /// @needwork = ad
    /// @endcode
    TriggerOutSMState(TriggerOutSMState const& other) = delete;

    /// @brief deleted copy assignment function
    /// @param other The TriggerOutSMState instance to be copyed
    /// @return the assigned TriggerOutSMState instance itself
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_09008
    /// @trace_id_ad=AD_SM_00322
    /// @trace_id_dd=DD_SM_00338
    /// @needwork = ad
    /// @endcode
    TriggerOutSMState& operator=(TriggerOutSMState const& other) = delete;

    /// @brief Move constructor function
    /// @param other The TriggerOutSMState instance to be moved
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_09008
    /// @trace_id_ad=AD_SM_00323
    /// @trace_id_dd=DD_SM_00339
    /// @needwork = ad
    /// @endcode
    TriggerOutSMState(TriggerOutSMState&& other) = default;

    /// @brief Move assignment function
    /// @param other The TriggerOutSMState instance to be moved
    /// @return the assigned TriggerOutSMState instance itself
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_09008
    /// @trace_id_ad=AD_SM_00324
    /// @trace_id_dd=DD_SM_00340
    /// @needwork = ad
    /// @endcode
    TriggerOutSMState& operator=(TriggerOutSMState&& other) = default;

protected:
    /// @brief Placeholder function
    /// @return Future object
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_09008
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08321
    /// @needwork = dda
    /// @endcode
    core::Future< void > PlaceholderFunction() noexcept override;
};

}  // namespace sm_state_comm
}  // namespace sm
}  // namespace ara

#endif  // TRIGGER_OUT_SM_STATE_H_
