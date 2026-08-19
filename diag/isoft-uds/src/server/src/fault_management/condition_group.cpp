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
/// @file       condition_group.cpp
/// @brief      This file implements condition_group functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=fault_management
/// @trace_id_sr=218ebb01-f354-4639-9bc1-5f91a494023b
/// @unit_name=ConditionGroup
/// @unit_description=This file provides condition_group functionality
/// @interface_level=unit
/// @endcode
///
/// ================================================================

#include "fault_management/condition_group.h"

#include <set>

#include "common/log.h"
#include "fault_management/condition.h"
#include "fault_management/public.h"

namespace isoft {
namespace uds {
namespace server {

bool ConditionGroup::Init(std::set< uint32_t >& conditionList,
                          std::map< uint32_t, std::shared_ptr< Condition > >& conditions,
                          std::string const& groupName)
{
    if (conditionList.empty()) {
        LogWarn() << "ConditionGroup::Init|conditionList is empty groupName =" << groupName.c_str();
        return false;
    }

    for (uint32_t const id : conditionList) {
        std::map< uint32_t, std::shared_ptr< Condition > >::const_iterator const itr{std::move(conditions.find(id))};
        if (itr != conditions.end()) {
            conditions_.push_back(itr->second);
        }
    }

    _initCallbacks();
    return true;
}


bool ConditionGroup::CheckConditionGroup() const noexcept
{
    bool ret{true};
    for (std::vector< std::shared_ptr< Condition > >::const_iterator itr{conditions_.cbegin()};
         itr != conditions_.cend(); ++itr) {
        if (!(*itr)->CheckCondition()) {
            ret = false;
            break;
        }
    }
    return ret;
}


void ConditionGroup::OnConditionGroupStatusChanged(StatusChangedCallback const& callback)
{
    callbacks_.push_back(callback);
}


void ConditionGroup::_initCallbacks()
{
    for (std::vector< std::shared_ptr< Condition > >::const_iterator itr{conditions_.cbegin()};
         itr != conditions_.cend(); ++itr) {
        (*itr)->OnConditionStatusChanged([this]() {
            if (CheckConditionGroup() != this->currentState_.load()) {
                currentState_ = !currentState_.load();
                for (std::vector< StatusChangedCallback >::const_iterator itrCB{callbacks_.cbegin()};
                     itrCB != callbacks_.cend(); ++itrCB) {
                    (*itrCB)(currentState_.load());
                }
            }
        });
    }
    currentState_ = CheckConditionGroup();
}
}  // namespace server
}  // namespace uds
}  // namespace isoft
