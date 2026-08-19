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
/// @file       trigger_io_ecu_state.h
/// @brief      A implementation of TriggerInOut_StateSkeleton.
/// @details
/// @date       2024-06-25
/// @author     lianglongxiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/StateManagement/EcuStateComm
/// @unit_name=TriggerIOEcuState
/// @interface_level=uint
/// @unit_description=A implementation of TriggerInOut_StateSkeleton.
/// @trace_id_sr=SR_SM_07001, SR_SM_07002
/// @endcode
///
/// ================================================================

#ifndef TRIGGER_INOUT_ECU_STATE_H_
#define TRIGGER_INOUT_ECU_STATE_H_

#include <ara/core/promise.h>
#include <ara/sm/triggerinout_state_skeleton.h>
namespace ara {
namespace sm {
namespace ecu_state_comm {
/// @brief A implementation of TriggerInOut_StateSkeleton.To be used by Adaptive (Platform) Applications to tigger State Management to change its internal state and to get information when it is carried out.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_sr=SR_SM_07001, SR_SM_07002
/// @trace_id_ad=AD_SM_08108
/// @trace_id_dd=DD_SM_08397
/// @needwork = ad
/// @endcode
class TriggerIOEcuState : public ara::sm::skeleton::TriggerInOut_StateSkeleton
{
public:
    /// @brief Abstract skeleton class which is implemented here.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = no
    /// @endcode
    using Skeleton = ara::sm::skeleton::TriggerInOut_StateSkeleton;

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
    /// @trace_id_ad=AD_SM_00496
    /// @trace_id_dd=DD_SM_00612
    /// @needwork = ad
    /// @endcode
    ~TriggerIOEcuState() final = default;

    /// @brief deleted copy constructor function
    /// @param other The TriggerIOEcuState instance to be copyed
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_07001, SR_SM_07002
    /// @trace_id_ad=AD_SM_00497
    /// @trace_id_dd=DD_SM_00613
    /// @needwork = ad
    /// @endcode
    TriggerIOEcuState(TriggerIOEcuState const& other) = delete;

    /// @brief deleted copy assignment function
    /// @param other The TriggerIOEcuState instance to be copyed
    /// @return the assigned TriggerIOEcuState instance itself
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_07001, SR_SM_07002
    /// @trace_id_ad=AD_SM_00498
    /// @trace_id_dd=DD_SM_00614
    /// @needwork = ad
    /// @endcode
    TriggerIOEcuState& operator=(TriggerIOEcuState const& other) = delete;

    /// @brief Move constructor function
    /// @param other The TriggerIOEcuState instance to be moved
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_07001, SR_SM_07002
    /// @trace_id_ad=AD_SM_00499
    /// @trace_id_dd=DD_SM_00615
    /// @needwork = ad
    /// @endcode
    TriggerIOEcuState(TriggerIOEcuState&& other) = default;

    /// @brief Move assignment function
    /// @param other The TriggerIOEcuState instance to be moved
    /// @return the assigned TriggerIOEcuState instance itself
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_07001, SR_SM_07002
    /// @trace_id_ad=AD_SM_00500
    /// @trace_id_dd=DD_SM_00616
    /// @needwork = ad
    /// @endcode
    TriggerIOEcuState& operator=(TriggerIOEcuState&& other) = default;

protected:
    /// @brief Placeholder function
    /// @return Future object
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_07001, SR_SM_07002
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08398
    /// @needwork = dda
    /// @endcode
    core::Future< void > PlaceholderFunction() noexcept override;
};

}  // namespace ecu_state_comm
}  // namespace sm
}  // namespace ara

#endif  // TRIGGER_INOUT_STATE_H_
