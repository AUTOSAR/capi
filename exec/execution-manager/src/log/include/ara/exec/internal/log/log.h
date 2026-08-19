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
/// @file       log.h
/// @brief      Log definition
/// @details
/// @date       2023-04-01
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_EXEC_INTERNAL_LOG_H_
#define ARA_EXEC_INTERNAL_LOG_H_

#include <ara/log/logger.h>

/// @brief Output Fatal level log
#define LOGF() LOG().LogFatal()

/// @brief Output Error level log
#define LOGE() LOG().LogError()

/// @brief Output Warn level log
#define LOGW() LOG().LogWarn()

/// @brief Output Info level log
#define LOGI() LOG().LogInfo()

/// @brief Output Debug level log
#define LOGD() LOG().LogDebug()

/// @brief Output Verbose level log
#define LOGV() LOG().LogVerbose()

#ifndef LOG
 /// @brief Default log context information
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    #define LOG() internal::log1::Log< internal::log1::DefaultLogCtx >()
#endif

namespace ara {
namespace exec {
namespace internal {
namespace log1 {

// PRQA S 2024,2025,5215 ++

/// @brief Default log context
class DefaultLogCtx
{
public:
 /// @brief Redefine char
    using Char8_t = char;

 /// @brief Get log context ID
 /// @return Log context ID
    static constexpr Char8_t const* GetLogCtxId() noexcept { return "#DFT"; }

 /// @brief Log context description
 /// @return Log context descriptor
    static constexpr Char8_t const* GetLogCtxDesc() noexcept { return "Default Log Context"; }
};

/// @brief Create or get log handle
/// @tparam T Log context type, default is DefaultLogCtx
/// @return Log handle
/// @exception std::runtime_error If log creation fails
template < typename T = DefaultLogCtx >
ara::log::Logger const& Log()
{
    static ara::log::Logger const& s_Logger{
        ara::log::CreateLogger(T::GetLogCtxId(), T::GetLogCtxDesc(), ara::log::LogLevel::kVerbose)};
    return s_Logger;
}

// PRQA S 2024,2025,5215 --

}  // namespace log1
}  // namespace internal
}  // namespace exec
}  // namespace ara

#endif  ///< ARA_EXEC_INTERNAL_LOG_H_
