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
/// @file       control_dtc_setting.cpp
/// @brief      This file implements control_dtc_setting functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=fault_management
/// @trace_id_sr=e67ae07e-cf42-48e2-9cfe-1b92dfea4173
/// @unit_name=ControlDtcSetting
/// @unit_description=This file provides control_dtc_setting functionality
/// @interface_level=unit
/// @endcode
///
/// ================================================================

#include "fault_management/control_dtc_setting.h"

#include "common/log.h"
namespace isoft {
namespace uds {
namespace server {


ControlDtcSetting::~ControlDtcSetting() noexcept { UnInit(); }

int32_t ControlDtcSetting::Set(bool const on)
{
    bool const first{first_.load()};
    if ((on == state_.load()) && !first) {
        return -1;
    }
    first_ = false;
    LogVerbose() << "ControlDtcSetting::Set -> on=" << on;
    state_ = on;
    {
        std::unique_lock< std::mutex > const lock{lock_};
        for (std::vector< StatusChangedCallback >::const_iterator itr{callbacks_.cbegin()}; itr != callbacks_.cend();
             ++itr) {
            (*itr)(on);
        }
    }
    return 0;
}


bool ControlDtcSetting::Get() const noexcept { return state_.load(); }


void ControlDtcSetting::UnInit()
{
    std::unique_lock< std::mutex > const lock{lock_};
    callbacks_.clear();
}


void ControlDtcSetting::SetNotifier(StatusChangedCallback const& callback)
{
    std::unique_lock< std::mutex > const lock{lock_};
    callbacks_.push_back(callback);
}
}  // namespace server
}  // namespace uds
}  // namespace isoft
