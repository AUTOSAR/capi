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
/// @file       aging.cpp
/// @brief      This file implements aging functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=fault_management
/// @trace_id_sr=407bb799-a443-4c6f-be6a-89a25a6d2286
/// @unit_name=aging
/// @unit_description=This file provides aging functionality
/// @interface_level=unit
/// @endcode
///
/// ================================================================

#include "fault_management/aging.h"

#include "common/log.h"
#include "fault_management/operation_cycle.h"
#include "fault_management/public.h"

namespace isoft {
namespace uds {
namespace server {


bool Aging::Init(AgingConfig& config, std::map< uint32_t, std::shared_ptr< OperationCycle > >& operationCycles)
{
    if (config.threshold == 0U) {
        LogError() << "Aging::Init|invalid threshold value, it is 0, check config file, aging_id =" << config.agingID;
        return false;
    }

    std::map< uint32_t, std::shared_ptr< OperationCycle > >::const_iterator const itr{
        std::move(operationCycles.find(config.agingCycle))};
    if (itr == operationCycles.cend()) {
        LogError() << "Aging::Init|invalid operation_cycle op_id =" << config.agingCycle
                   << ",aging_id =" << config.agingID;
        return false;
    }

    config_ = config;
    itr->second->RegisterAging(shared_from_this());
    LogDebug() << "Aging::Init|success, aging_id =" << config.agingID
               << "aging_operation_cycle_id =" << config.agingCycle;
    return true;
}


void Aging::SetNotifier(std::function< void() > const& notifier) { notifiers_.push_back(notifier); }


void Aging::ReportOperationCycleStateTransition(OperationCycleType const oldState,
                                                OperationCycleType const newState) const
{
    if (((oldState != newState) && (newState == OperationCycleType::kOperationCycleEnd)) ||
        // restart. need end first
        ((OperationCycleType::kOperationCycleStart == oldState)
         && (OperationCycleType::kOperationCycleStart == newState))) {
        for (std::vector< std::function< void() > >::const_iterator itr{notifiers_.cbegin()}; itr != notifiers_.cend();
             ++itr) {
            (*itr)();
        }
    }
}
}  // namespace server
}  // namespace uds
}  // namespace isoft
