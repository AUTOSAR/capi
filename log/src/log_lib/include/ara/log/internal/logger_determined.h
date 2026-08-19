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
/// @file       logger_determined.h
/// @brief      Logger determined execution configuration
/// @details
/// @date       2025-09-03
/// @author     yangjinbiao
/// @version    1.2.0
///
/// ================================================================

#ifndef __LOG_INTERNAL_LOGGER_DETERMINED__
#define __LOG_INTERNAL_LOGGER_DETERMINED__

#include <cstdint>
#include <memory>

namespace ara {
namespace log {
namespace internal {

// Forward declaration
class DeterminedExecutor;  ///< Deterministic executor, full definition in DltLogDerm/determined_executor.h

/// @brief Logger deterministic execution configuration
/// @note Configuration and resource management for deterministic execution
struct LoggerDetermined
{
    /// @brief Whether deterministic execution is enabled
    bool isDetermined{false};

    /// @brief Deterministic execution domain index (0-255 valid)
    std::int16_t domainIndex{-1};

    /// @brief Pre-cached deterministic executor (injected by BindConfiguredDetermLogger/GetLoggerByCidDeterm)
    /// @note executor only holds necessary runtime dependencies (thread pool, memory pool, switch and file sinker reference)
    std::shared_ptr< DeterminedExecutor > executor{nullptr};
};

}  // namespace internal
}  // namespace log
}  // namespace ara

#endif  // __LOG_INTERNAL_LOGGER_DETERMINED__
