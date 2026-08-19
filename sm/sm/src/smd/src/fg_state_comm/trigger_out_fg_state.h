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
/// @file       trigger_out_fg_state.h
/// @brief      A implementation of TriggerOut_FunctionGroupSkeleton.
/// @details
/// @date       2024-06-25
/// @author     lianglongxiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/StateManagement/FGStateComm
/// @unit_name=TriggerOutFGState
/// @interface_level=uint
/// @unit_description=A implementation of TriggerOut_FunctionGroupSkeleton.
/// @trace_id_sr=SR_SM_01001
/// @endcode
///
/// ================================================================

#ifndef TRIGGER_OUT_FUNCTION_GROUP_H_
#define TRIGGER_OUT_FUNCTION_GROUP_H_

#include <ara/sm/triggerout_functiongroup_skeleton.h>

namespace ara {
namespace sm {
namespace fg_state_comm {

/// @brief A implementation of TriggerOut_FunctionGroupSkeleton.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_sr=SR_SM_01001
/// @trace_id_ad=AD_SM_08117
/// @trace_id_dd=DD_SM_08440
/// @needwork = ad
/// @endcode
class TriggerOutFGState : public ara::sm::skeleton::TriggerOut_FunctionGroupSkeleton
{
public:
    /// @brief Abstract skeleton class which is implemented here.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = no
    /// @endcode
    using Skeleton = ara::sm::skeleton::TriggerOut_FunctionGroupSkeleton;

    /// @brief Use base class constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = no
    /// @endcode
    using Skeleton::Skeleton;

    /// @brief Destructor function
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_01001
    /// @trace_id_ad=AD_SM_00570
    /// @trace_id_dd=DD_SM_00686
    /// @needwork = ad
    /// @endcode
    ~TriggerOutFGState() final = default;

    /// @brief deleted copy constructor function
    /// @param other The TriggerOutFGState instance to be copyed
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_01001
    /// @trace_id_ad=AD_SM_00571
    /// @trace_id_dd=DD_SM_00687
    /// @needwork = ad
    /// @endcode
    TriggerOutFGState(TriggerOutFGState const& other) = delete;

    /// @brief deleted copy assignment function
    /// @param other The TriggerOutFGState instance to be copyed
    /// @return the assigned TriggerOutFGState instance itself
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_01001
    /// @trace_id_ad=AD_SM_00572
    /// @trace_id_dd=DD_SM_00688
    /// @needwork = ad
    /// @endcode
    TriggerOutFGState& operator=(TriggerOutFGState const& other) = delete;

    /// @brief Move constructor function
    /// @param other The TriggerOutFGState instance to be moved
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_01001
    /// @trace_id_ad=AD_SM_00573
    /// @trace_id_dd=DD_SM_00689
    /// @needwork = ad
    /// @endcode
    TriggerOutFGState(TriggerOutFGState&& other) = default;

    /// @brief Move assignment function
    /// @param other The TriggerOutFGState instance to be moved
    /// @return the assigned TriggerOutFGState instance itself
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_01001
    /// @trace_id_ad=AD_SM_00574
    /// @trace_id_dd=DD_SM_00690
    /// @needwork = ad
    /// @endcode
    TriggerOutFGState& operator=(TriggerOutFGState&& other) = default;

protected:
    /// @brief Placeholder function
    /// @return Future object
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_01001
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08441
    /// @needwork = dda
    /// @endcode
    core::Future< void > PlaceholderFunction() noexcept override;
};
}  // namespace fg_state_comm
}  // namespace sm
}  // namespace ara

#endif  // TRIGGER_OUT_FUNCTION_GROUP_H_
