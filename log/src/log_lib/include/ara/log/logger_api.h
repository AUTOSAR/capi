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
/// @file       logger_api.h
/// @brief      Template class for encapsulating log parameters
/// @details
/// @date       2024-06-28
/// @author     yangjinbiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/LOG
/// @interface_level = module
/// @trace_id_sr=LOG_SR_00001,LOG_SR_00002,LOG_SR_00003,LOG_SR_00004,LOG_SR_00006,LOG_SR_00010,LOG_SR_00011,LOG_SR_00012,LOG_SR_00013,LOG_SR_00014,LOG_SR_000021,LOG_SR_000022
/// @unit_name = logger
/// @unit_description=Lib interface side of Dlt, providing application interfaces externally for users
/// @endcode
///
/// ================================================================

#ifndef ___ARA_LOG_LOGGER_API_H__
#define ___ARA_LOG_LOGGER_API_H__

#include <cstdint>
#include <functional>
#include <memory>

#include "argument.h"
#include "common.h"

#ifdef ARA_LOG_HAS_MODEL_MESSAGE
    #include "ara/log/internal/dltmessage/define.h"
#endif

#include "dlt_logger.h"
#include "logger_class.h"
namespace ara {
namespace log {

/// @brief Use a function to re-encapsulate as an Argument type
/// @param[in]  arg
/// @param[in]  name
/// @param[in]  unit
/// @return
/// @code{.isoft}
/// @tparam T
/// @interface_level=unit
/// @trace_id_ad=AD_LOG_00079
/// @trace_id_dd=DD_LOG_00098
/// @needwork = ad
/// @endcode
template < typename T >
Argument< T > Arg(T &&arg, const char *name = nullptr, const char *unit = nullptr) noexcept
{
    return Argument< T >(std::forward< T >(arg), name, unit);
}

Logger &CreateLogger(const ara::core::InstanceSpecifier &is) noexcept;

Logger &CreateLogger(ara::core::StringView cid, ara::core::StringView des) noexcept;

Logger &CreateLogger(ara::core::StringView cid, ara::core::StringView des, ara::log::LogLevel level) noexcept;

Logger &CreateLoggerByDomainID(ara::core::StringView cid,
                               ara::core::StringView des,
                               std::uint8_t const domainId,
                               ara::log::LogLevel level) noexcept;
Logger &CreateLoggerByDomainID(ara::core::StringView cid,
                               ara::core::StringView des,
                               std::uint8_t const domainId) noexcept;
/// @brief Get the status of the remote client.
///
/// @return Status of the remote client
///
/// @code{.isoft}
///
/// @trace_id_sws={SWS_LOG_00101}@tracestatus{draft}
/// @interface_level=unit
/// @trace_id_ad=AD_LOG_00081
/// @trace_id_dd=DD_LOG_00100
/// @needwork = ad
/// @endcode
ClientState RemoteClientState() noexcept;

/// @brief When file caching is enabled, flush the cache to the file.
///
/// @return Whether the flush is ok
///
/// @code{.isoft}
///
/// @ extension
///
/// @interface_level=unit
/// @trace_id_ad=AD_LOG_00082
/// @trace_id_dd=DD_LOG_00101
/// @needwork = ad
/// @endcode
OperState FlushBufferToFile() noexcept;

//// ###############R2311#######################
void RegisterConnectionStateHandler(std::function< void(ClientState) > callback) noexcept;

//// ####################ext###################
/// @brief Get a smart pointer to a Logger object.
/// @param[in]  cid Context identifier, up to 4 characters
/// @return Smart pointer to the Logger object
/// !!!!!!!!!!!!!!!Important!!!!!!!!!!!!!!!!!
/// This function can only be called after initialization and only when a Logger object has already been created using createLogger can the smart pointer be obtained.
/// !!!

std::shared_ptr< Logger > ExtGetLoggerPtr(std::string const &cid) noexcept;

/// @brief Replace parameter names to avoid conflicts in log macro definitions (for Logger reference, distinguishing Debug/Release modes)
#ifdef ARA_LOG_RELEASE
    // Release mode: Debug logs are not executed
    #define LOG_D(araInternalLoggerRef, ...)                                                                           \
        do {                                                                                                           \
        } while (0)
    #define LOG_V(araInternalLoggerRef, ...)                                                                           \
        do {                                                                                                           \
        } while (0)
    #define LOG_I(araInternalLoggerRef, ...)                                                                           \
        do {                                                                                                           \
            (araInternalLoggerRef).LogInfo() << __VA_ARGS__;                                                           \
        } while (0)
    #define LOG_W(araInternalLoggerRef, ...)                                                                           \
        do {                                                                                                           \
            (araInternalLoggerRef).LogWarn() << __VA_ARGS__;                                                           \
        } while (0)
    #define LOG_E(araInternalLoggerRef, ...)                                                                           \
        do {                                                                                                           \
            (araInternalLoggerRef).LogError() << __VA_ARGS__;                                                          \
        } while (0)
    #define LOG_F(araInternalLoggerRef, ...)                                                                           \
        do {                                                                                                           \
            (araInternalLoggerRef).LogFatal() << __VA_ARGS__;                                                          \
        } while (0)
#else
    // Debug mode: all logs are executed
    #define LOG_D(araInternalLoggerRef, ...)                                                                           \
        do {                                                                                                           \
            (araInternalLoggerRef).LogDebug() << __VA_ARGS__;                                                          \
        } while (0)
    #define LOG_V(araInternalLoggerRef, ...)                                                                           \
        do {                                                                                                           \
            (araInternalLoggerRef).LogVerbose() << __VA_ARGS__;                                                        \
        } while (0)
    #define LOG_I(araInternalLoggerRef, ...)                                                                           \
        do {                                                                                                           \
            (araInternalLoggerRef).LogInfo() << __VA_ARGS__;                                                           \
        } while (0)
    #define LOG_W(araInternalLoggerRef, ...)                                                                           \
        do {                                                                                                           \
            (araInternalLoggerRef).LogWarn() << __VA_ARGS__;                                                           \
        } while (0)
    #define LOG_E(araInternalLoggerRef, ...)                                                                           \
        do {                                                                                                           \
            (araInternalLoggerRef).LogError() << __VA_ARGS__;                                                          \
        } while (0)
    #define LOG_F(araInternalLoggerRef, ...)                                                                           \
        do {                                                                                                           \
            (araInternalLoggerRef).LogFatal() << __VA_ARGS__;                                                          \
        } while (0)
#endif

/// Usage example:

// Logger &logger = CreateLogger("CID", "DES");

// LOG_D(plogger, "Debug message: ", 42);       // Output only in Debug mode
// LOG_V(plogger, "Verbose details");           // Output only in Debug mode
// LOG_I(plogger, "Info: system started");      // Output in all modes
// LOG_W(plogger, "Warning: low memory");       // Output in all modes
// LOG_E(plogger, "Error: file not found");     // Output in all modes
// LOG_F(plogger, "Fatal: critical failure");   // Output in all modes

}  // namespace log
}  // namespace ara

#endif
