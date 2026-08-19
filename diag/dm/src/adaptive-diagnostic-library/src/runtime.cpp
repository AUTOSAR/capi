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
/// @file       runtime.cpp
/// @brief      This file provides the implementation of g_Runtime.
/// @details
/// @date       2022-11-21
/// @author     gaohuiming
/// @version    1.2.0
///
/// ================================================================

#include "ara/diag/internal/runtime.h"

#include <memory>

#include "ara/diag/diag_error_domain.h"
#include "ara/diag/diag_uds_nrc_error_domain.h"
#include "isoft/ara_fsh/process.h"
#include "resolve.h"
#include "runtime_enviroment.h"
#include "utility.h"
namespace ara {
namespace diag {
namespace internal {

static std::unique_ptr< RuntimeEnviroment > g_Runtime;

/// @brief Runtime initialization
/// @code{.isoft}
/// export_level=/Diagnostics/Diagnostic Library
/// @endcode
/// @return Result
/// @throws on overflow
ara::core::Result< void > Initialize()
{
    // register error domain
    // ara::com::internal::Runtime& comRuntime{ara::com::internal::GetInstance()};
    // comRuntime.RegisterErrorDomain(GetDiagDomain());
    // comRuntime.RegisterErrorDomain(GetDiagOfferDomain());
    // comRuntime.RegisterErrorDomain(GetDiagReportingDomain());
    // comRuntime.RegisterErrorDomain(GetDiagUdsNrcDomain());

    g_Runtime = std::make_unique< RuntimeEnviroment >();
    auto ret  = g_Runtime->Initialize();

    // register skeleton and proxy
    // ara::core::Result<void> result{std::move(ara::com::internal::GetInstance())};
    // if (!result.HasValue()) {
    //     return result;
    // }

    return ret;
}

/// @brief Runtime deinitialization
/// @code{.isoft}
/// export_level=/Diagnostics/Diagnostic Library
/// @endcode
/// @return Result
ara::core::Result< void > Deinitialize() noexcept
{
    g_Runtime.reset();
    return {};
}

/// @brief Get fatal error log stream object
/// @return Log stream object
ara::log::LogStream LogFatal() noexcept { return g_Runtime->GetLogger().LogFatal(); }

/// @brief Get error log stream object
/// @return Log stream object
ara::log::LogStream LogError() noexcept { return g_Runtime->GetLogger().LogError(); }

/// @brief Get warning log stream object
/// @return Log stream object
ara::log::LogStream LogWarn() noexcept { return g_Runtime->GetLogger().LogWarn(); }

/// @brief Get info log stream object
/// @return Log stream object
ara::log::LogStream LogInfo() noexcept { return g_Runtime->GetLogger().LogInfo(); }

/// @brief Get debug log stream object
/// @return Log stream object
ara::log::LogStream LogDebug() noexcept { return g_Runtime->GetLogger().LogDebug(); }

/// @brief Get verbose log stream object
/// @return Log stream object
ara::log::LogStream LogVerbose() noexcept { return g_Runtime->GetLogger().LogVerbose(); }

/// @brief Get instance descriptor according to instance specifier
/// @return Instance descriptor
/// @throws on overflow
ara::core::Result< InstanceInfo > Resolve(ara::core::InstanceSpecifier const& specifier)
{
    return g_Runtime->Retrieve(specifier);
}

/// @brief Get conversation manager
/// @return Conversation manager
ConversationManager& GetConversationManager() noexcept { return g_Runtime->GetConversationManager(); }

}  // namespace internal
}  // namespace diag
}  // namespace ara
