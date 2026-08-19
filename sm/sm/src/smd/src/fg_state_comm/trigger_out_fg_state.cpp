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
/// @file       trigger_out_fg_state.cpp
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
/// @interface_level=uint
/// @trace_id_sr=SR_SM_01001
/// @unit_name=TriggerOutFGState
/// @unit_description=A implementation of TriggerOut_FunctionGroupSkeleton.
/// @endcode
///
/// ================================================================

#include "trigger_out_fg_state.h"

namespace ara {
namespace sm {
namespace fg_state_comm {

/// @brief Placeholder function
/// @return Future object
core::Future< void > TriggerOutFGState::PlaceholderFunction() noexcept
{
    core::Promise< void > promise;
    core::Future< void > future{promise.get_future()};
    return future;
}
}  // namespace fg_state_comm
}  // namespace sm
}  // namespace ara
