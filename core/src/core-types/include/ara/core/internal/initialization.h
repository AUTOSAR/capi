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
/// @file       initialization.h
/// @brief      This file describes the global initialization and shutdown functions that initialize resp. deinitialize data structures and threads of the AUTOSAR Runtime for Adaptive Applications.
/// @details
/// @date       2022-06-24
/// @author     hanzhibo
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_CORE_INTERNAL_INITIALIZATION_H_
#define ARA_CORE_INTERNAL_INITIALIZATION_H_
#include <atomic>
#include <mutex>

#include "ara/core/abort.h"
#include "ara/core/array.h"
#include "ara/core/result.h"

namespace ara {
namespace core {
namespace internal {
constexpr unsigned int kAraModuleCount = 10;
struct CoreContext
{
    std::atomic< AbortHandler > abortHandler{};
    std::mutex abortMutexP{};
    unsigned int deinitializeFunCount{0};
    ara::core::Array< std::function< ara::core::Result< void >() >, kAraModuleCount > deinitializeFunList{};
    //?
};

extern struct CoreContext* g_CoreContext;

ara::core::Result< void > Initialize();
ara::core::Result< void > Deinitialize();

void RegisterDeinitialize(std::function< ara::core::Result< void >() > const& func);
}  // namespace internal
}  // namespace core
}  // namespace ara
#endif  // ARA_CORE_INTERNAL_INITIALIZATION_H_