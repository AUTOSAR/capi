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
/// @file       initialize.cpp
/// @brief
/// @details
/// @date       2022-06-24
/// @author     hanzhibo
/// @version    1.2.0
///
/// ================================================================

#include <cassert>
#include <memory>
#include <string>

#include "ara/core/internal/initialization.h"

namespace ara {
namespace core {
namespace internal {

struct CoreContext* g_CoreContext = nullptr;
::ara::core::Result< void > Initialize()
{
    assert(g_CoreContext == nullptr);
    g_CoreContext = new struct CoreContext();
    if (g_CoreContext == nullptr) {
        assert(0);
        return Result< void >{};  // TODO:(3) Default error code
    }
    g_CoreContext->deinitializeFunCount = 0;
    // auto emptyFun = []() { return ara::core::Result<void>{}; };
    // for (long unsigned int i = 0; i < kAraModuleCount; ++i) {
    //     g_CoreContext->deinitializeFunList[i] = emptyFun;
    // }

    return Result< void >{};
}

void RegisterDeinitialize(std::function< ara::core::Result< void >() > const& func)
{
    ++g_CoreContext->deinitializeFunCount;
    assert(g_CoreContext->deinitializeFunCount < kAraModuleCount);
    g_CoreContext->deinitializeFunList[g_CoreContext->deinitializeFunCount - 1] = func;
}

::ara::core::Result< void > Deinitialize()
{
    assert(g_CoreContext != nullptr);
    while (0 != g_CoreContext->deinitializeFunCount) {
        if (!g_CoreContext->deinitializeFunList[--g_CoreContext->deinitializeFunCount]()) {
            assert(0);
        }
    }

    delete g_CoreContext;
    g_CoreContext = nullptr;

    return Result< void >{};
}

}  // namespace internal
}  // namespace core
}  // namespace ara
