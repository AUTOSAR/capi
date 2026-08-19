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
/// @file       trigger_in_fg_state.h
/// @brief      A implementation of TriggerIn_FunctionGroupSkeleton.
/// @details
/// @date       2024-06-25
/// @author     lianglongxiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/StateManagement/FGStateComm
/// @unit_name=TriggerInFGState
/// @interface_level=uint
/// @unit_description=A implementation of TriggerIn_FunctionGroupSkeleton.
/// @trace_id_sr=SR_SM_01002
/// @endcode
///
/// ================================================================

#ifndef TRIGGER_IN_FG_STATE_H_
#define TRIGGER_IN_FG_STATE_H_

#include <ara/sm/triggerin_functiongroup_skeleton.h>

namespace ara {
namespace sm {
namespace fg_state_comm {

/// @brief A implementation of TriggerIn_FunctionGroupSkeleton.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_sr=SR_SM_01002
/// @trace_id_ad=AD_SM_08116
/// @trace_id_dd=DD_SM_08438
/// @needwork = ad
/// @endcode
class TriggerInFGState : public ara::sm::skeleton::TriggerIn_FunctionGroupSkeleton
{
public:
    /// @brief Abstract skeleton class which is implemented here.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = no
    /// @endcode
    using Skeleton = ara::sm::skeleton::TriggerIn_FunctionGroupSkeleton;

    /// @brief Use base class constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = no
    /// @endcode
    using Skeleton::Skeleton;

    /// @brief Destructor function
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_01002
    /// @trace_id_ad=AD_SM_00565
    /// @trace_id_dd=DD_SM_00681
    /// @needwork = ad
    /// @endcode
    ~TriggerInFGState() final = default;

    /// @brief deleted copy constructor function
    /// @param other The TriggerInFGState instance to be copyed
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_01002
    /// @trace_id_ad=AD_SM_00566
    /// @trace_id_dd=DD_SM_00682
    /// @needwork = ad
    /// @endcode
    TriggerInFGState(TriggerInFGState const& other) = delete;

    /// @brief deleted copy assignment function
    /// @param other The TriggerInFGState instance to be copyed
    /// @return the assigned TriggerInFGState instance itself
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_01002
    /// @trace_id_ad=AD_SM_00567
    /// @trace_id_dd=DD_SM_00683
    /// @needwork = ad
    /// @endcode
    TriggerInFGState& operator=(TriggerInFGState const& other) = delete;

    /// @brief Move constructor function
    /// @param other The TriggerInFGState instance to be moved
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_01002
    /// @trace_id_ad=AD_SM_00568
    /// @trace_id_dd=DD_SM_00684
    /// @needwork = ad
    /// @endcode
    TriggerInFGState(TriggerInFGState&& other) = default;

    /// @brief Move assignment function
    /// @param other The TriggerInFGState instance to be moved
    /// @return the assigned TriggerInFGState instance itself
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_01002
    /// @trace_id_ad=AD_SM_00569
    /// @trace_id_dd=DD_SM_00685
    /// @needwork = ad
    /// @endcode
    TriggerInFGState& operator=(TriggerInFGState&& other) = default;

protected:
    /// @brief Placeholder function
    /// @return Future object
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_01002
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08439
    /// @needwork = dda
    /// @endcode
    core::Future< void > PlaceholderFunction() noexcept override;
};
}  // namespace fg_state_comm
}  // namespace sm
}  // namespace ara

#endif  // TRIGGER_IN_FG_STATE_H_
