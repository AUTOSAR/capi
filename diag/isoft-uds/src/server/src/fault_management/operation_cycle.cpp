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
/// @file       operation_cycle.cpp
/// @brief      This file implements operation_cycle functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=fault_management
/// @trace_id_sr=887d3c38-14f0-442b-869b-8ea1df7256c2
/// @unit_name=OperationCycle
/// @unit_description=This file provides operation_cycle functionality
/// @interface_level=unit
/// @endcode
///
/// ================================================================

#include "fault_management/operation_cycle.h"

#include <string>

#include "common/log.h"
#include "fault_management/aging.h"
#include "fault_management/event.h"
#include "fault_management/indicator.h"

namespace isoft {
namespace uds {
namespace server {



bool OperationCycle::Init()
{
    if (config_.operationInterface.get() == nullptr) {
        LogWarn() << "OperationCycle::Init|no config operationInterface operation_id =" << config_.operationCycleId;
    } else {
        int res
            = config_.operationInterface->RegisterGetOperationCycle([this]() -> OperationCycleType { return state_; });
        if (res != 0) {
            LogWarn() << "OperationCycle::Init|RegisterGetOperationCycle fail "
                         "operation_id ="
                      << config_.operationCycleId << "res =" << res;
            return false;
        }

        res = config_.operationInterface->RegisterSetOperationCycle([this](OperationCycleType const oc) {
            LogInfo() << "OperationCycle::Init|aa set operation state =" << static_cast< uint16_t >(oc)
                      << "operation_id =" << config_.operationCycleId;
            _setOperationCycleState(oc);
        });
        if (res != 0) {
            LogWarn() << "OperationCycle::Init|RegisterSetOperationCycle fail "
                         "operation_id ="
                      << config_.operationCycleId << "res =" << res;
            return false;
        }
    }

    return true;
}



void OperationCycle::Start()
{
    // [SWS_DM_00889].
    if (config_.autoStart) {
        _setOperationCycleState(OperationCycleType::kOperationCycleStart);
    }
}


void OperationCycle::Stop()
{
    // [SWS_DM_00890].
    // [SWS_DM_00892].
    if (config_.autoEnd && (state_ != OperationCycleType::kOperationCycleEnd)) {
        _setOperationCycleState(OperationCycleType::kOperationCycleEnd);
    }
}



void OperationCycle::_setOperationCycleState(OperationCycleType const newState)
{
    LogVerbose() << "OperationCycle::SetOperationCycleState|enter. newState =" << static_cast< std::uint32_t >(newState)
                 << "operation_id =" << config_.operationCycleId << "indicators_size =" << indicators_.size()
                 << "agings_size =" << agings_.size();

    OperationCycleType const oldState{state_};
    state_ = newState;
    for (std::vector< std::shared_ptr< Aging > >::const_iterator itr{agings_.cbegin()}; itr != agings_.cend(); ++itr) {
        if ((*itr) != nullptr) {
            (*itr)->ReportOperationCycleStateTransition(oldState, newState);
        }
    }
    for (std::vector< std::shared_ptr< Indicator > >::const_iterator itr{indicators_.cbegin()};
         itr != indicators_.cend(); ++itr) {
        if ((*itr) != nullptr) {
            (*itr)->ReportOperationCycleStateTransition(oldState, newState);
        }
    }
    for (std::vector< std::shared_ptr< Event > >::const_iterator itr{events_.cbegin()}; itr != events_.cend(); ++itr) {
        if ((*itr) != nullptr) {
            (*itr)->ReportOperationCycleStateTransition(oldState, newState);
        }
    }
    if (config_.operationInterface.get() != nullptr) {
        int32_t const res = config_.operationInterface->NotifyOperationCycle(state_);
        LogInfo() << "OperationCycle::SetOperationCycleState|NotifyOperationCycle "
                     "currentState Update operation_id ="
                  << config_.operationCycleId << ", res =" << res << "state =" << static_cast< uint16_t >(state_);
    }
}

}  // namespace server
}  // namespace uds
}  // namespace isoft
