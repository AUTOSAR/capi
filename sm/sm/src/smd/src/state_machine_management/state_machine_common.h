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
/// @file       state_machine_common.h
/// @brief      Define common types used internally by the state management module
/// @details
/// @date       2024-08-22
/// @author     lianglongxiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/StateManagement/StateMachineManagement
/// @unit_name=StateMachineCommon
/// @interface_level=uint
/// @unit_description=Define common types used internally by the state management module
/// @trace_id_sr=SR_SM_09002, SR_SM_09006, SR_SM_09007
/// @endcode
///
/// ================================================================

#ifndef STATE_MACHINE_COMMON_H_
#define STATE_MACHINE_COMMON_H_

#include <ara/core/string.h>

#include <cstdint>

namespace ara {
namespace sm {
namespace state_machine_management {

/// @brief Working status of StateMachine
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_sr=SR_SM_09002, SR_SM_09006, SR_SM_09007
/// @trace_id_ad=AD_SM_08074
/// @trace_id_dd=DD_SM_08257
/// @needwork = ad
/// @endcode
enum class StateMachineWorkStatusType : uint32_t
{
    /// @brief Off, the state machine has stopped working;
    kOff = 0,
    /// @brief Normal transition, will enter NormalTransition state when Start is called, and will enter NormalTransition state after ErrorRecovery state ends
    kNormalTransition,
    /// @brief Error recovery, when in ErrorRecovery state, RequestState will return kRecoveryTransitionOngoing
    kErrorRecovery
};

/// @brief StateMachine processing status type
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_sr=SR_SM_09002, SR_SM_09006, SR_SM_09007
/// @trace_id_ad=AD_SM_08075
/// @trace_id_dd=DD_SM_08258
/// @needwork = ad
/// @endcode
enum class ProcessStatusType : uint32_t
{
    kOff        = 0,  ///< Stopped working
    kProcessing = 1,  ///< Processing
    kSuccess    = 2,  ///< Processing ended: success
    kFailed     = 3,  ///< Processing ended: processing failed (including being stopped)
};

/// @brief Convert working status to String
/// @param workStatus Working status
/// @return String representing the working status
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_sr=SR_SM_09002, SR_SM_09006, SR_SM_09007
/// @trace_id_ad=AD_SM_00255
/// @trace_id_dd=DD_SM_00263
/// @needwork = ad
/// @endcode
inline core::String ToString(StateMachineWorkStatusType const workStatus) noexcept
{
    core::String strWorkStatus{"Unknown"};
    switch (workStatus) {
        case StateMachineWorkStatusType::kOff: {
            strWorkStatus = "Off";
        } break;

        case StateMachineWorkStatusType::kNormalTransition: {
            strWorkStatus = "NormalTransition";
        } break;

        case StateMachineWorkStatusType::kErrorRecovery: {
            strWorkStatus = "ErrorRecovery";
        } break;

        default: {
            strWorkStatus = "Unknown";
        } break;
    }
    return strWorkStatus;
}

/// @brief Convert processing status to String
/// @param processStatus Processing status
/// @return String representing the processing status
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_sr=SR_SM_09002, SR_SM_09006, SR_SM_09007
/// @trace_id_ad=AD_SM_00256
/// @trace_id_dd=DD_SM_00264
/// @needwork = ad
/// @endcode
inline core::String ToString(ProcessStatusType const processStatus) noexcept
{
    core::String strProcessStatus{"Unknown"};
    switch (processStatus) {
        case ProcessStatusType::kOff: {
            strProcessStatus = "Off";
        } break;

        case ProcessStatusType::kProcessing: {
            strProcessStatus = "Processing";
        } break;

        case ProcessStatusType::kSuccess: {
            strProcessStatus = "Success";
        } break;

        case ProcessStatusType::kFailed: {
            strProcessStatus = "Failed";
        } break;

        default: {
            strProcessStatus = "Unknown";
        } break;
    }
    return strProcessStatus;
}

}  // namespace state_machine_management
}  // namespace sm
}  // namespace ara

#endif  // STATE_MACHINE_COMMON_H_