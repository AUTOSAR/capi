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
/// @file       trigger_io_fg_state.h
/// @brief      A implementation of TriggerInOut_FunctionGroupSkeleton.
/// @details
/// @date       2024-06-25
/// @author     lianglongxiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/StateManagement/FGStateComm
/// @unit_name=TriggerIOFGState
/// @interface_level=uint
/// @unit_description=A implementation of TriggerInOut_FunctionGroupSkeleton.
/// @trace_id_sr=SR_SM_01003
/// @endcode
///
/// ================================================================

#ifndef TRIGGER_IO_FG_STATE_H_
#define TRIGGER_IO_FG_STATE_H_

#include <ara/sm/triggerinout_functiongroup_skeleton.h>

namespace ara {
namespace sm {
namespace fg_state_comm {

/// @brief A implementation of TriggerIn_FunctionGroupSkeleton.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_sr=SR_SM_01003
/// @trace_id_ad=AD_SM_08115
/// @trace_id_dd=DD_SM_08436
/// @needwork = ad
/// @endcode
class TriggerIOFGState : public ara::sm::skeleton::TriggerInOut_FunctionGroupSkeleton
{
public:
    /// @brief Abstract skeleton class which is implemented here.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = no
    /// @endcode
    using Skeleton = ara::sm::skeleton::TriggerInOut_FunctionGroupSkeleton;

    /// @brief Use base class constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = no
    /// @endcode
    using Skeleton::Skeleton;

    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_01003
    /// @trace_id_ad=AD_SM_00560
    /// @trace_id_dd=DD_SM_00676
    /// @needwork = ad
    /// @endcode
    ~TriggerIOFGState() final = default;

    /// @brief deleted copy constructor function
    /// @param other The TriggerIOFGState instance to be copyed
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_01003
    /// @trace_id_ad=AD_SM_00561
    /// @trace_id_dd=DD_SM_00677
    /// @needwork = ad
    /// @endcode
    TriggerIOFGState(TriggerIOFGState const& other) = delete;

    /// @brief deleted copy assignment function
    /// @param other The TriggerIOFGState instance to be copyed
    /// @return the assigned TriggerIOFGState instance itself
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_01003
    /// @trace_id_ad=AD_SM_00562
    /// @trace_id_dd=DD_SM_00678
    /// @needwork = ad
    /// @endcode
    TriggerIOFGState& operator=(TriggerIOFGState const& other) = delete;

    /// @brief Move constructor function
    /// @param other The TriggerIOFGState instance to be moved
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_01003
    /// @trace_id_ad=AD_SM_00563
    /// @trace_id_dd=DD_SM_00679
    /// @needwork = ad
    /// @endcode
    TriggerIOFGState(TriggerIOFGState&& other) = default;

    /// @brief Move assignment function
    /// @param other The TriggerIOFGState instance to be moved
    /// @return the assigned TriggerIOFGState instance itself
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_01003
    /// @trace_id_ad=AD_SM_00564
    /// @trace_id_dd=DD_SM_00680
    /// @needwork = ad
    /// @endcode
    TriggerIOFGState& operator=(TriggerIOFGState&& other) = default;

protected:
    /// @brief Placeholder function
    /// @return Future object
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_01003
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08437
    /// @needwork = dda
    /// @endcode
    core::Future< void > PlaceholderFunction() noexcept override;
};

}  // namespace fg_state_comm
}  // namespace sm
}  // namespace ara

#endif  // TRIGGER_IO_FG_STATE_H_
