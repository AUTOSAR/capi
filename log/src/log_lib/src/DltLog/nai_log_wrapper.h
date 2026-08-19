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
/// @file       nai_log_wrapper.h
/// @brief      NaiLog log adaptation wrapper class
/// @details
/// @date       2026-03-10
/// @author     yangjinbiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/LOG/DltLog
/// @interface_level = unit
/// @unit_name = nai_log_wrapper
/// @unit_description=NaiLog log adaptation wrapper class
/// @endcode
///
/// ================================================================

#ifndef __INTERNAL_NAI_LOG_WRAPPER__
#define __INTERNAL_NAI_LOG_WRAPPER__

#include <cstdint>
#include <memory>
#include <string>

#include "DltLogDefines/application_config.h"
#include "logger_api.h"
#include "logger_class.h"
#include "nai/runtime/nai_errno.h"
#include "nai/runtime/nai_log.h"

namespace ara {
namespace log {
namespace internal {

// Forward declaration of Logger class is resolved by including logger_class.h

/// @brief NaiLogWrapper class encapsulates related functions of NaiLog
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_TSYNC_02437
/// @trace_id_dd=DD_TSYNC_02836
/// @needwork = ad
/// @endcode
class NaiLogWrapper final
{
public:
    /// @brief Constructor
    NaiLogWrapper() = default;

    /// @brief Copy constructor
    NaiLogWrapper(NaiLogWrapper const &other) = delete;
    /// @brief Copy assignment
    NaiLogWrapper &operator=(NaiLogWrapper const &other) = delete;
    /// @brief Move constructor
    NaiLogWrapper(NaiLogWrapper &&object) noexcept = delete;
    /// @brief Move assignment
    NaiLogWrapper &operator=(NaiLogWrapper &&object) noexcept = delete;

    /// @brief Destructor
    ~NaiLogWrapper() = default;

    /// @brief Convert nai log level
    /// @param err Level
    /// @return Log string
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_TSYNC_02432
    /// @trace_id_dd=DD_TSYNC_02831
    /// @needwork = ad
    /// @endcode
    static std::string NaiErrorToString(std::int32_t const &err) noexcept;

    /// @brief Initialize NaiLog
    /// @param bOutput Whether to output
    /// @param defaultLvl Default log level
    /// @return Execution result
    bool InitNaiLog(bool const &bOutput, ara::log::LogLevel const &defaultLvl) noexcept;

    /// @brief Deinitialize NaiLog
    /// @return Execution result
    bool DeinitNaiLog() noexcept;

    /// @brief Get the NaiLog callback Logger
    /// @return NaiLog callback Logger pointer
    Logger *GetNaiCallbackLogger() const { return naiCallbackLogger_; }

    /// @brief Set the NaiLog callback Logger
    /// @param logger Logger pointer
    static void SetNaiCallbackLogger(Logger *logger) { naiCallbackLogger_ = logger; }

private:
    /// @brief Set the NaiLog log level
    /// @param level Log level
    /// @return Execution result
    bool SetNaiLogLevel(ara::log::LogLevel const &level) noexcept;

private:
    /// @brief NaiLog callback Logger pointer (static member for lambda access)
    static Logger *naiCallbackLogger_;
};

}  // namespace internal
}  // namespace log
}  // namespace ara

#endif  // __INTERNAL_NAI_LOG_WRAPPER__
