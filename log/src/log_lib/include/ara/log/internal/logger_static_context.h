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
/// @file       logger_static_context.h
/// @brief      Logger static context — immutable data fixed at Logger creation
/// @details
/// @date       2025-09-03
/// @author     yangjinbiao
/// @version    1.2.0
///
/// ================================================================

#ifndef __LOG_INTERNAL_LOGGER_STATIC_CONTEXT__
#define __LOG_INTERNAL_LOGGER_STATIC_CONTEXT__

#include <memory>

#include "logger_identity.h"

// EncodeContext is defined in the DltProtocol layer, but that header is placed under include/,
// needs to be included via a forward include path
#include "DltProtocol/dlt_protocol_encoder.h"

namespace ara {
namespace log {
namespace internal {

/// @brief Logger static context (constructed once at creation, then immutable)
/// @note Aggregates all fields that do not change during the Logger's lifetime,
///       and provides BuildEncodeBase() to uniformly produce EncodeContext,
///       avoiding LoggerScopeData directly carrying static fields.
struct LoggerStaticContext
{
    /// @brief Identity information (ECU/APP/CTX ID, session, etc.)
    LoggerIdentity identity;

    /// @brief Non-verbose encoding mode (synchronized with LoggerConfig::nonVerboseMode at construction)
    bool nonVerboseMode{true};

    /// @brief Pre-built encoding context (populated by calling RebuildEncodeContext() after all identity/nonVerboseMode fields are ready)
    /// @note Shared with LogStreamContext::encodeCtx, zero-copy transfer of static fields
    std::shared_ptr< const EncodeContext > encodeCtxPtr{nullptr};

    /// @brief Called after all identity / nonVerboseMode fields are ready, builds and caches the shared encoding context
    void RebuildEncodeContext() noexcept { encodeCtxPtr = std::make_shared< EncodeContext >(BuildEncodeBase()); }

    /// @brief Produce the encoding base context from the static context at once (retained for test code to use directly)
    /// @return EncodeContext to be used by DltProtocolEncoder::SetEncodeContext()
    EncodeContext BuildEncodeBase() const noexcept
    {
        EncodeContext ctx{};
        ctx.ecuId          = identity.ecuId;
        ctx.appId          = identity.appId;
        ctx.contextId      = identity.contextId;
        ctx.sessionId      = identity.sessionId;
        ctx.sessionSupport = identity.sessionSupport;
        ctx.nonVerboseMode = nonVerboseMode;
        return ctx;
    }
};

using LoggerStaticContextPtr = std::shared_ptr< LoggerStaticContext >;

}  // namespace internal
}  // namespace log
}  // namespace ara

#endif  // __LOG_INTERNAL_LOGGER_STATIC_CONTEXT__
