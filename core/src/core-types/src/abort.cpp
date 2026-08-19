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
/// @file       abort.cpp
/// @brief
/// @details
/// @date       2022-06-24
/// @author     hanzhibo
/// @version    1.2.0
///
/// ================================================================

#include "ara/core/abort.h"

#include <atomic>
#include <cassert>
#include <cstdlib>
#include <mutex>

#include "ara/core/internal/initialization.h"

namespace ara {
namespace core {

AbortHandler SetAbortHandler(AbortHandler handler) noexcept
{
    (void)handler;
    assert(internal::g_CoreContext->abortHandler.is_lock_free());

    AbortHandler oldHandler = internal::g_CoreContext->abortHandler.exchange(handler);
    return oldHandler;
}

void Abort(const char* text) noexcept
{
    // [SWS_CORE_12403]
    // R20-11 requires ara::log, causing circular dependency. Fixed to output to stderr since R21-11.
    // Log before locking to ensure key info output.
    if (text != nullptr) {
        fputs(text, stderr);
    }
    fflush(stdout);
    fflush(stderr);

    /// @uptrace{SWS_CORE_12407, 8d8980d37b3374d9cf6f075acb4d33e27e3f70e0}
    std::lock_guard< std::mutex > lock{internal::g_CoreContext->abortMutexP};

    /// @uptrace{SWS_CORE_12404, c8884f9b87907e7434cae7669305f2835f526bcd}
    AbortHandler const handler = internal::g_CoreContext->abortHandler.load();
    if (handler != nullptr) {
        handler();
        fflush(stdout);
        fflush(stderr);
    }

    /// @uptrace{SWS_CORE_12402, f88afcbb8a55f16a8d71c6a29f62ff04be7c2ccb}
    /// @uptrace{SWS_CORE_12405, b234e43370a81ce97d6f02aa4342bbdb9311f95a}
    /// @uptrace{SWS_CORE_12406, 89d21f1e6fb68f50773051c43e56e92bcd92c12e}
    std::abort();
}

}  // namespace core
}  // namespace ara
