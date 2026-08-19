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
/// @brief      This file provides the implementation of CancellationHandler.
/// @details
/// @date       2022-08-02
/// @author     gaohuiming
/// @version    1.2.0
///
/// ================================================================

#include "ara/diag/cancellation_handler.h"

#include <cstdint>
#include <utility>

#include "cancellation/cancellation_event.h"
#include "netProxy/net_proxy.h"

namespace ara {
namespace diag {

/// @brief Constructor
/// @param[in] event Conversation event
CancellationHandler::CancellationHandler(std::shared_ptr< isoft::dm::CancellationEvent > event) noexcept
    : event_{std::move(event)}
{
}

/// @brief Destructor of CancellationHandler cannot be used
///
CancellationHandler::~CancellationHandler()
{
    if (event_.get() != nullptr) {
        event_.reset();
    }
}

/// @brief Returns true in if the diagnostic service execution is cancelled in DM.
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
/// @return true
/// @return false
/// @throws on overflow
///
/// @traceid{SWS_DM_00614}@tracestatus{draft}
bool CancellationHandler::IsCanceled() const { return event_->IsCancel(); }

/// @brief Regisering a notifier function which is called if the diagnostic service execution is canceled in DM.
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
/// @param[in] notifier Callback function
/// @throws on overflow
///
/// @traceid{SWS_DM_00615}@tracestatus{draft}
// void CancellationHandler::SetNotifier(std::function<void(void)> notifier) { event_->SetNotifier(std::move(notifier)); }

void CancellationHandler::SetNotifier(std::function< void(void) > notifier)
{
    // isoft::dm::dis::NetProxy::GetInstance().GetNetAccess()->RegisterOnCancel()
    event_->SetNotifier(std::move(notifier));
}

}  // namespace diag
}  // namespace ara
