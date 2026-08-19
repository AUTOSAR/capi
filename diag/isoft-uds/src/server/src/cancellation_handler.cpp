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
/// @file       cancellation_handler.cpp
/// @brief      This file implements cancellation_handler functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/isoft/uds
/// @trace_id_sr=2b766c9e-e488-41f6-b62e-c9d31918fcb9
/// @unit_name=CancellationHandler
/// @unit_description=This file provides cancellation_handler functionality
/// @interface_level=module
/// @endcode
///
/// ================================================================

#include "isoft/uds/cancellation_handler.h"

namespace isoft {
namespace uds {
namespace server {


CancellationHandler::CancellationHandler(std::shared_ptr< CancellationState > state) noexcept
    : lifePtr_{std::make_shared< std::uint8_t >()}, statePtr_{std::move(state)}
{
}


bool CancellationHandler::IsCanceled() const
{
    if (statePtr_.get() != nullptr) {
        return statePtr_->IsCanceled();
    }
    return false;
}


void CancellationHandler::SetNotifier(std::function< void(void) > const& notifier)
{
    if (statePtr_.get() != nullptr) {
        std::weak_ptr< std::uint8_t > weakLife{lifePtr_};
        return statePtr_->AddNotifier([notifier, weakLife]() {
            if (!weakLife.expired()) {
                notifier();
            }
        });
    }
    return;
}

}  // namespace server
}  // namespace uds
}  // namespace isoft