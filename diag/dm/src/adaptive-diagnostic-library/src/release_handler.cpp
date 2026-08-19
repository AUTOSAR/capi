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
/// @file       release_handler.cpp
/// @brief      This file provides the implementation of release_handler.
/// @details
/// @date       2023-08-23
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================

#include "ara/diag/release_handler.h"

#include "gen_code/file_transfer/apiAgent/release_handler_impl.h"
namespace ara {
namespace diag {

/// @brief Default constructor of ReleaseHandler
/// @traceid{SWS_DM_01530}@tracestatus{draft}
ReleaseHandler::ReleaseHandler(std::shared_ptr< isoft::dm::dis::ReleaseHandlerImpl > impl) noexcept
    : impl_{std::move(impl)}
{
}

/// @brief Reports whether the shared resource is no longer in use
/// @return True in if the shared resource is no longer in use, False otherwise
/// @pre -
/// @reentrant FALSE
/// @traceid{SWS_DM_01536}@tracestatus{draft}
bool ReleaseHandler::MayRelease() const noexcept { return impl_->MayRelease(); }

/// @brief Registering a notifier function which is called if the shared resource can be freed. A consecutive call
/// of this method will overwrite the previous registered notifier.
/// @param[in] notifier  Notification function that is called upon releasing the shared resource
/// @pre The passed in function do not throw any exceptions
/// @pre The passed notifier must contain a callable object
/// @reentrant FALSE
/// @traceid{SWS_DM_01537}@tracestatus{draft}
void ReleaseHandler::SetNotifier(std::function< void(void) > notifier) noexcept
{
    return impl_->SetNotifier(std::move(notifier));
}

}  // namespace diag
}  // namespace ara