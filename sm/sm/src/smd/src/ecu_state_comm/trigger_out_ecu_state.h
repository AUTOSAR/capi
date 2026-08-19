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
/// @file       trigger_out_ecu_state.h
/// @brief      A implementation of TriggerOut_StateSkeleton.
/// @details
/// @date       2024-06-25
/// @author     lianglongxiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/StateManagement/EcuStateComm
/// @unit_name=TriggerOutEcuState
/// @interface_level=uint
/// @unit_description=A implementation of TriggerOut_StateSkeleton.
/// @trace_id_sr= SR_SM_07001, SR_SM_07002
/// @endcode
///
/// ================================================================

#ifndef TRIGGER_OUT_ECU_STATE_H_
#define TRIGGER_OUT_ECU_STATE_H_

#include <ara/core/promise.h>
#include <ara/sm/triggerout_state_skeleton.h>

namespace ara {
namespace sm {
namespace ecu_state_comm {

/// @brief A implementation of TriggerOut_StateSkeleton. To be used by Adaptive (Platform) Applications to be informed when State Management has changed its internal state.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_sr=SR_SM_07001, SR_SM_07002
/// @trace_id_ad=AD_SM_08110
/// @trace_id_dd=DD_SM_08408
/// @needwork = ad
/// @endcode
class TriggerOutEcuState : public ara::sm::skeleton::TriggerOut_StateSkeleton
{
public:
    /// @brief Abstract skeleton class which is implemented here.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = no
    /// @endcode
    using Skeleton = ara::sm::skeleton::TriggerOut_StateSkeleton;

    /// @brief Use base class constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = no
    /// @endcode
    using Skeleton::Skeleton;

    /// @brief Destructor function
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_07001, SR_SM_07002
    /// @trace_id_ad=AD_SM_00512
    /// @trace_id_dd=DD_SM_00628
    /// @needwork = ad
    /// @endcode
    ~TriggerOutEcuState() final = default;

    /// @brief deleted copy constructor function
    /// @param other The TriggerOutEcuState instance to be copyed
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_07001, SR_SM_07002
    /// @trace_id_ad=AD_SM_00513
    /// @trace_id_dd=DD_SM_00629
    /// @needwork = ad
    /// @endcode
    TriggerOutEcuState(TriggerOutEcuState const& other) = delete;

    /// @brief deleted copy assignment function
    /// @param other The TriggerOutEcuState instance to be copyed
    /// @return the assigned TriggerOutEcuState instance itself
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_07001, SR_SM_07002
    /// @trace_id_ad=AD_SM_00514
    /// @trace_id_dd=DD_SM_00630
    /// @needwork = ad
    /// @endcode
    TriggerOutEcuState& operator=(TriggerOutEcuState const& other) = delete;

    /// @brief Move constructor function
    /// @param other The TriggerOutEcuState instance to be moved
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_07001, SR_SM_07002
    /// @trace_id_ad=AD_SM_00515
    /// @trace_id_dd=DD_SM_00631
    /// @needwork = ad
    /// @endcode
    TriggerOutEcuState(TriggerOutEcuState&& other) = default;

    /// @brief Move assignment function
    /// @param other The TriggerOutEcuState instance to be moved
    /// @return the assigned TriggerOutEcuState instance itself
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_07001, SR_SM_07002
    /// @trace_id_ad=AD_SM_00516
    /// @trace_id_dd=DD_SM_00632
    /// @needwork = ad
    /// @endcode
    TriggerOutEcuState& operator=(TriggerOutEcuState&& other) = default;

protected:
    /// @brief Placeholder function
    /// @return Future object
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_07001, SR_SM_07002
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08409
    /// @needwork = dda
    /// @endcode
    core::Future< void > PlaceholderFunction() noexcept override;
};

}  // namespace ecu_state_comm
}  // namespace sm
}  // namespace ara

#endif  // TRIGGER_OUT_STATE_H_
