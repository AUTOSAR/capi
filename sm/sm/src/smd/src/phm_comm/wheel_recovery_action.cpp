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
/// @file       wheel_recovery_action.cpp
/// @brief      RecoveryAction for wheel supervision.
/// @details
/// @date       2024-06-25
/// @author     lianglongxiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/StateManagement/PHMComm
/// @interface_level=uint
/// @trace_id_sr=SR_SM_08001
/// @unit_name=WheelRecoveryAction
/// @unit_description=RecoveryAction for wheel supervision.
/// @endcode
///
/// ================================================================

#include "wheel_recovery_action.h"

#include "helper.h"

namespace ara {
namespace sm {
namespace phm_comm {
/// @brief RecoveryHandler to be invoked by PHM.
/// @param executionError Information on detected error, shall give further information for error recovery.
/// @param supervision The type of local supervision which failed
void WheelRecoveryAction::RecoveryHandler(exec::ExecutionErrorEvent const &executionError,
                                          TypeOfSupervision supervision) noexcept
{
    log_.LogInfo() << "WheelRecoveryAction::RecoveryHandler(), executionError:"
                   << common::ErrorEventToString(executionError).c_str()
                   << "supervision:" << common::TypeOfSupervisionToString(_ToInternalType(supervision)).c_str();

    // Corresponding promise for the request
    core::Promise< void > promise;
    ara::core::Future< void, ara::core::ErrorCode > const future{promise.get_future()};

    DeliverRecoveryNotificationRequest(executionError, supervision, std::move(promise));
    std::ignore = future;
}

/// @brief Destructor function
/// @code{.isoft}
/// @interface_level=unit
/// @needwork = ad
/// @endcode
WheelRecoveryAction::~WheelRecoveryAction() noexcept
{
    log_.LogInfo() << "WheelRecoveryAction::~WheelRecoveryAction()";
}
}  // namespace phm_comm
}  // namespace sm
}  // namespace ara
