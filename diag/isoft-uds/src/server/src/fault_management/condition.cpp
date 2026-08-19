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
/// @file       condition.cpp
/// @brief      This file implements condition functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=fault_management
/// @trace_id_sr=5c3ea362-aee6-4de8-871f-fc1332e90b6c
/// @unit_name=condition
/// @unit_description=This file provides condition functionality
/// @interface_level=unit
/// @endcode
///
/// ================================================================

#include "fault_management/condition.h"

#include <string>

#include "common/log.h"
#include "fault_management/public.h"
namespace isoft {
namespace uds {
namespace server {



bool Condition::Init()
{
    if (config_.conditionInterface.get() == nullptr) {
        LogWarn() << "Condition::Init|no config conditionInterface condition_id =" << config_.enableConditionId;
    } else {
        int res = config_.conditionInterface->RegisterSetCondition([this](bool state) {
            if (state_ == state) {
                return;
            }

            LogInfo() << "Condition::Init|aa set condition state =" << state
                      << "condition_id =" << config_.enableConditionId;
            state_ = state;
            for (std::vector< StatusChangedCallback >::const_iterator itr{callbacks_.cbegin()};
                 itr != callbacks_.cend(); ++itr) {
                (*itr)();
            }
        });
        if (res != 0) {
            LogWarn() << "Condition::Init|RegisterSetCondition fail condition_id =" << config_.enableConditionId;
            return false;
        }

        res = config_.conditionInterface->RegisterGetCondition([this]() -> bool { return state_; });
        if (res != 0) {
            LogWarn() << "Condition::Init RegisterGetCondition fail condition_id =" << config_.enableConditionId;
            return false;
        }
    }

    state_ = config_.state;

    return true;
}


bool Condition::CheckCondition() const noexcept { return state_.load(); }


void Condition::OnConditionStatusChanged(StatusChangedCallback const& callback) { callbacks_.push_back(callback); }

}  // namespace server
}  // namespace uds
}  // namespace isoft
