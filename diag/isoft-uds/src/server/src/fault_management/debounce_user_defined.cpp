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
/// @file       debounce_user_defined.cpp
/// @brief      This file implements debounce_user_defined functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=fault_management
/// @trace_id_sr=57fe8d5b-de31-489c-b770-51fa6668be20
/// @unit_name=DebounceUserDefined
/// @unit_description=This file provides debounce_user_defined functionality
/// @interface_level=unit
/// @endcode
///
/// ================================================================

#include "fault_management/debounce_user_defined.h"

namespace isoft {
namespace uds {
namespace server {

void DebounceUserDefined::IndicateReportedStatus(MonitorAction action)
{
    LogVerbose() << "DebounceUserDefined::IndicateReportedStatus -> enter. action="
                 << std::move(Helper::ToString(action)) << ", freeze=" << _IsFreeze();
    if (_IsFreeze()) {
        return;
    }
    switch (action) {
        case MonitorAction::kPrefailed:
        case MonitorAction::kPrepassed: {
            _FireEventGetFDCFromMonitor();
        } break;
        case MonitorAction::kFailed: {
            SetFDC(kFdcMax);
        } break;
        case MonitorAction::kPassed: {
            SetFDC(kFdcMin);
        } break;
        default: {
            break;
        }
    }
}


void DebounceUserDefined::Reset()
{
    LogVerbose() << "DebounceUserDefined::Reset -> enter.";
    _SetFDC(0, true);
}


void DebounceUserDefined::Freeze(bool b) noexcept
{
    LogVerbose() << "DebounceUserDefined::Freeze -> enter.";
    _SetFreeze(b);
}


DebounceTypeEnum DebounceUserDefined::GetType() const noexcept { return DebounceTypeEnum::kUserDefined; }

}  // namespace server
}  // namespace uds
}  // namespace isoft
