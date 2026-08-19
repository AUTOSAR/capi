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
/// @file       cancellation_state.cpp
/// @brief      This file implements cancellation_state functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/isoft/uds
/// @trace_id_sr=edb12ecd-2fac-4699-bae7-328631dcb14b
/// @unit_name=CancellationState
/// @unit_description=This file provides cancellation_state functionality
/// @interface_level=module
/// @endcode
///
/// ================================================================

#include "isoft/uds/cancellation_state.h"
namespace isoft {
namespace uds {
namespace server {


CancellationState::CancellationState() : notifierTable_{} {}


bool CancellationState::IsCanceled() const { return cancelState_; }


void CancellationState::AddNotifier(std::function< void(void) > const& notifier) { notifierTable_.push_back(notifier); }

void CancellationState::DoCanceled() noexcept
{
    cancelState_ = true;
    for (auto&& notifier : notifierTable_) {
        notifier();
    }
}

}  // namespace server
}  // namespace uds
}  // namespace isoft