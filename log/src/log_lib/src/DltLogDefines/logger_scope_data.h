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
/// @file       logger_scope_data.h
/// @brief      Template class for encapsulating log parameters
/// @details
/// @date       2024-06-28
/// @author     yangjinbiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/LOG/DltProtocol
/// @interface_level = module
/// @trace_id_sr=LOG_SR_00005,LOG_SR_00008,LOG_SR_00010,LOG_SR_00015,LOG_SR_00016,LOG_SR_00017,LOG_SR_00018,LOG_SR_00019,LOG_SR_00020
/// @unit_name = logger_scope_data
/// @unit_description==Protocol structure
/// @endcode
///
/// ================================================================

#ifndef __INTERNAL_LOG_logger_share_data__
#define __INTERNAL_LOG_logger_share_data__

#include <atomic>
#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <vector>

// Introduce the split structures (recommended)
#include "internal/logger_config.h"
#include "internal/logger_determined.h"
#include "internal/logger_sinkers.h"
#include "internal/logger_static_context.h"
#include "internal/logger_timeinfo.h"

namespace ara {
namespace log {
namespace internal {

class LogTimeStamp;
class IlogSinker;

class ThreadPool;
class LocalPool;

enum class DLTVERSION : std::uint8_t
{
    kDltVersion1 = 0x01,
    kDltVersion2 = 0x02,
};

using SinkerListType            = std::vector< std::shared_ptr< IlogSinker > >;
using DltSinkerLevelMap         = std::map< std::string, std::uint8_t >;
using DltSinkerLevelMapIterator = DltSinkerLevelMap::iterator;

/// @brief Logger private data (composite pattern)
/// @note Static immutable fields are collectively consolidated into LoggerStaticContext, while runtime variable fields remain here.
///       - LoggerStaticContext: Identity information + encoding base (constructed once, immutable)
///       - LoggerConfig: Configuration information (hot-updatable)
///       - LoggerSinkers: Output sinker management
///       - LoggerTimeInfo: Runtime time
///       - LoggerDetermined: Deterministic execution configuration
struct LoggerScopeData
{
    // ========== Static context (constructed once at creation) ==========
    /// @brief Static identity and encoding base (immutable, injected by LogManager when creating Logger)
    LoggerStaticContextPtr staticCtx{std::make_shared< LoggerStaticContext >()};

    // ========== Dynamic fields (runtime variable) ==========
    /// @brief Configuration information (hot-updatable)
    LoggerConfig config;

    /// @brief Output sinker management
    LoggerSinkers sinkers;

    /// @brief Runtime state (atomic operations)
    LoggerTimeInfo runtime;

    /// @brief Deterministic execution configuration
    LoggerDetermined determined;

    std::atomic< std::uint8_t > msgCount{0U};
};
using LoggerDataPtr = std::shared_ptr< internal::LoggerScopeData >;
}  // namespace internal
}  // namespace log
}  // namespace ara

#endif