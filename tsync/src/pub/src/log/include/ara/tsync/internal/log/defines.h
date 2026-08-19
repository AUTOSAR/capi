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
/// @file       defines.h
/// @brief      time synchronization internal log type definitions
/// @details
/// @date       2023-02-09
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// export_level=/TimeSync/LogWrapper
/// module_path=/TimeSync/LogWrapper
/// @endcode
///
/// ================================================================

#ifndef ARA_TSYNC_INTERNAL_LOG_DEFINES_H_
#define ARA_TSYNC_INTERNAL_LOG_DEFINES_H_

#include <ara/core/string.h>
#include <ara/core/string_view.h>

#include <cstdint>
#include <functional>

namespace ara {
namespace tsync {
namespace internal {
namespace tslog {

/// @brief log level definition
enum class LogLevel : std::uint8_t
{
    kOff     = 0x00,  ///< off
    kFatal   = 0x01,  ///< fatal error
    kError   = 0x02,  ///< error
    kWarn    = 0x03,  ///< warning
    kInfo    = 0x04,  ///< info
    kDebug   = 0x05,  ///< debug
    kVerbose = 0x06   ///< verbose debug
};
/// @brief log attributes
class LogAttribute final
{
public:
    /// @brief default constructor
    /// @exception exception specification
    LogAttribute() = default;

    /// @brief default destructor
    ~LogAttribute() = default;

    /// @brief copy constructor
    /// @param attr - another object
    /// @exception
    LogAttribute(LogAttribute const &attr) = default;

    /// @brief copy assignment
    /// @param attr - another object
    /// @return reference to self
    LogAttribute &operator=(LogAttribute const &attr) & = default;

    /// @brief move constructor
    /// @param attr - another object
    LogAttribute(LogAttribute &&attr) = default;

    /// @brief move assignment
    /// @param attr - another object
    /// @return reference to self
    LogAttribute &operator=(LogAttribute &&attr) = default;

    /// @brief constructor with parameters
    /// @param ctx - log contextID
    /// @param desc - ContextId description
    /// @param level - log level
    /// @param useAraLog - whether to use ara log
    LogAttribute(ara::core::String ctx,
                 ara::core::String desc,
                 LogLevel const &level,
                 bool const useAraLog = true) noexcept
        : logCtxId_{std::move(ctx)}, logDesc_{std::move(desc)}, logLevel_{level}, useAraLog_{useAraLog}
    {
    }

    /// @brief get ContextId
    /// @return ContextId
    ara::core::String const &GetLogContextId() const noexcept { return logCtxId_; }

    /// @brief set ContextId
    /// @param id - ContextId
    void SetLogContextId(ara::core::String const &id) noexcept { logCtxId_ = id; }

    /// @brief get Context description
    /// @return Context description
    ara::core::String const &GetLogDescription() const noexcept { return logDesc_; }

    /// @brief set Context description
    /// @param desc - Context description
    void SetLogDescription(ara::core::String const &desc) noexcept { logDesc_ = desc; }

    /// @brief get log level
    /// @return log level
    LogLevel GetLogLevel() const noexcept { return logLevel_; }

    /// @brief set log level
    /// @param level - log level
    void SetLogLevel(LogLevel const &level) noexcept { logLevel_ = level; }

    /// @brief get whether to use ara log
    /// @return whether to use ara log
    bool UseAraLog() const noexcept { return useAraLog_; }

private:
    /// @name logCtxId_ - log ContextID
    ara::core::String logCtxId_{""};

    /// @name logDesc_ - log description
    ara::core::String logDesc_{""};

    /// @name logLevel_ - log level
    LogLevel logLevel_{LogLevel::kOff};

    /// @name useAraLog_ - whether to use ara log
    bool useAraLog_{false};
};

/// @brief log output handling function
using OutputHandler = std::function< void(LogAttribute const &logAttr, ara::core::StringView const &logMesg) >;

}  // namespace tslog
}  // namespace internal
}  // namespace tsync
}  // namespace ara

#endif  /// ARA_TSYNC_INTERNAL_LOG_DEFINES_H_
