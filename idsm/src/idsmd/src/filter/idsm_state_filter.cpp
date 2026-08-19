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
/// @file       idsm_state_filter.cpp
/// @brief      State filter implementation
/// @details
/// @date       2023-02-15
/// @author     niuliming
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/idsm/Filter chain
/// @interface_level=unit
/// @trace_id_sr=SR_IDSM_0007
/// @unit_name=StateFilter
/// @unit_description=
/// @endcode
///
/// ================================================================

#include "idsm_state_filter.h"

#include "event/idsm_event_memory.h"
#include "log/idsm_log.h"
#include "server/idsm_fg_state.h"

namespace ara {
namespace idsm {
/// @brief Filter interface
/// @param event event to filter
/// @return filter result true: passed, false: not passed
/// @exception stack overflow exception
/// @code{.isoft}
/// @threadsafety={unsafe}
/// export_level=/
/// @endcode
bool StateFilter::Filter(EventPtr const& event)
{
    static_cast< void >(event);
    for (auto& ele : fgStateMap_) {
        ara::core::String const fgState{FunGrpState::GetInstance()->Get(ele.first)};
        if (!fgState.empty()) {
            if (std::find(ele.second.begin(), ele.second.end(), fgState) != ele.second.end()) {
                LOG_WARN << "filter chain check fail. filter: state filter, event id: " << event->GetEventId()
                         << ", counter:" << event->GetCounter();
                return false;
            }
        }
    }
    return true;
}
/// @brief Set blacklist of functional group states
/// @param fgName functional group name
/// @param fgState functional group state
/// @exception stack overflow exception
/// @code{.isoft}
/// @threadsafety={unsafe}
/// @endcode
void StateFilter::SetFGState(ara::core::String const& fgName, ara::core::String const& fgState)
{
    if (fgStateMap_.count(fgName) == 0U) {
        FGStateVec v{fgState};
        std::ignore = fgStateMap_.insert(std::pair< ara::core::String, FGStateVec >(fgName, v));
        return;
    }
    std::map< ara::core::String, FGStateVec >::iterator const it{fgStateMap_.find(fgName)};
    if (std::find(it->second.begin(), it->second.end(), fgState) != it->second.end()) {
        return;
    }
    it->second.push_back(fgState);
}

}  // namespace idsm
}  // namespace ara
