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
/// @file       action_item_sleep.cpp
/// @brief      Sleep action class
/// @details
/// @date       2024-08-20
/// @author     lianglongxiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/StateManagement/StateMachineManagement
/// @interface_level=uint
/// @trace_id_sr=SR_SM_09007
/// @unit_name=ActionItemSleep
/// @unit_description=Sleep action item
/// @endcode
///
/// ================================================================

#include "state_machine_management/action_item_sleep.h"

#include <ara/sm/error_domain_sm.h>

#include <memory>

#include "helper.h"

namespace ara {
namespace sm {
namespace state_machine_management {

using ara::sm::SMErrc;

/// @brief Default constructor
/// @param sleepTimeMs the sleep time(ms)
ActionItemSleep::ActionItemSleep(uint32_t const &sleepTimeMs) noexcept : ActionItemBase{ActionType::kSleep}
{
    SetSleepTime(sleepTimeMs);
    GetLog().LogInfo() << "ActionItemSleep::ActionItemSleep(), sleepTimeMs_:" << sleepTimeMs;
}

/// @brief Start executing the action
/// @param processID The ID when this action starts execution (also known as processing sequence number)
void ActionItemSleep::StartExecute(uint32_t const processID) noexcept { std::ignore = processID; }

/// @brief Handling after action execution succeeds
void ActionItemSleep::OnSuccess() noexcept {}

/// @brief Handling after action execution fails
void ActionItemSleep::OnFailure() noexcept {}

/// @brief Callback when the Future corresponding to Action is ready
/// @param processID Processing sequence number
void ActionItemSleep::OnActionFutureReady(uint32_t const processID) noexcept { std::ignore = processID; }
}  // namespace state_machine_management
}  // namespace sm
}  // namespace ara
