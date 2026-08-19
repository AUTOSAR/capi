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
/// @file       logger_api.cpp
/// @brief      Local internal variables
/// @details
/// @date       2025-09-15
/// @author     yangjinbiao
/// @version    1.2.0
///
/// ================================================================

#include "logger_api.h"

#include <cstdint>
#include <string>
#include <tuple>

#include "DltLog/log_manager.h"
#include "common.h"

/// @brief Local internal variables
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_TSYNC_02433
/// @trace_id_dd=DD_TSYNC_02832
/// @needwork = ad
/// @endcode
static std::shared_ptr< ara::log::internal::LogManager > g_Local_Share_Worker{nullptr};

extern "C" void Ara_log_internal_Sig_Pipe_Handler(std::int32_t sigNum) noexcept
{
    std::ignore = sigNum;

    if (g_Local_Share_Worker != nullptr) {
        std::ignore = g_Local_Share_Worker->PipeBroken();
    }
}

namespace ara {
namespace log {
Logger Logger::s_Dummylogger_{nullptr};

/// @brief Local empty logger used to suppress printing
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_TSYNC_02434
/// @trace_id_dd=DD_TSYNC_02833
/// @needwork = ad
/// @endcode

/// @brief Internal flag indicating whether initialized
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_TSYNC_02435
/// @trace_id_dd=DD_TSYNC_02834
/// @needwork = ad
/// @endcode

///############################################################logger_api.h#########################################
/// @brief Helper function: handles the common check logic of two CreateLogger functions
/// @param[in] cid Context ID
/// @param[in] des Description
/// @param[in] funcName Name of the calling function (usually __func__)
/// @param[in] isDeterm Whether it is the overloaded version with domainId (used for log distinction)
/// @param[out] loggerDes Converted description string (output parameter)
/// @return If a dummy logger needs to be returned, return its pointer; otherwise return nullptr
bool ConditionCheck(ara::core::StringView cid) noexcept
{
    if (g_Local_Share_Worker == nullptr || g_Local_Share_Worker->IsDestroyed()
        || (g_Local_Share_Worker->IsLoggingBehavior() == false) || (g_Local_Share_Worker->IsInit() == false)) {
        return false;
    }

    if (cid == nullptr || cid == " " || cid.empty()) {
        std::cout << " ctxID empty" << std::endl;
        return false;
    }
    return true;  // All checks passed, a real logger can be created
}

Logger& CreateLogger(const ara::core::InstanceSpecifier& instsf) noexcept
{
    ara::core::StringView strinst{instsf.ToString()};
    if (ConditionCheck(strinst) == false) {
        LOGVERBOSE("leave:") << __func__;
        return Logger::GetDummyLogger();
    }
    /// TODO: Use file descriptor to find the corresponding information.

    /// This should use the manifest

    std::string contextId{""};
    std::string contextDes{""};
    ara::log::LogLevel loglevel{ara::log::LogLevel::kOff};

    return g_Local_Share_Worker->GetLoggerByCid(contextId.data(), contextDes.data(), loglevel);
}
Logger& CreateLogger(ara::core::StringView cid, ara::core::StringView des) noexcept
{
    if (ConditionCheck(cid) == false) {
        LOGVERBOSE("leave:") << __func__;
        return Logger::GetDummyLogger();
    }

    // Execute the unique logic of this overload

    ara::log::LogLevel defLevel{static_cast< ara::log::LogLevel >(g_Local_Share_Worker->GetDefaultLogLevel())};
    LOGVERBOSE(__func__) << ": create logger " << cid.data()
                         << "defleve:" << std::to_string(static_cast< std::int32_t >(defLevel));

    return g_Local_Share_Worker->GetLoggerByCid(cid.data(), des.data(), defLevel);
}

Logger& CreateLoggerByDomainID(ara::core::StringView cid,
                               ara::core::StringView des,
                               std::uint8_t const domainId) noexcept
{
    if (ConditionCheck(cid) == false) {
        LOGVERBOSE("leave:") << __func__;
        return Logger::GetDummyLogger();
    }

    // Execute the unique logic of this overload
    LOGVERBOSE(__func__) << ": create logger " << cid.data();
    ara::log::LogLevel defLevel{static_cast< ara::log::LogLevel >(g_Local_Share_Worker->GetDefaultLogLevel())};
    return g_Local_Share_Worker->GetLoggerByCid(cid.data(), des.data(), domainId, defLevel);
}
/// @brief Overloaded CreateLogger without domainId
/// @param[in] cid Context ID
/// @param[in] des Description
/// @param[in] level Log level
/// @return Logger reference
Logger& CreateLogger(ara::core::StringView cid, ara::core::StringView des, ara::log::LogLevel level) noexcept
{
    if (ConditionCheck(cid) == false) {
        LOGVERBOSE("ConditionCheck leave:") << __func__;
        return Logger::GetDummyLogger();
    }

    // Execute the unique logic of this overload
    LOGVERBOSE(__func__) << ": create logger " << cid.data();
    return g_Local_Share_Worker->GetLoggerByCid(cid.data(), des.data(), level);
}

/// @brief Overloaded CreateLogger with domainId
/// @param[in] cid Context ID
/// @param[in] des Description
/// @param[in] domainId Domain ID
/// @param[in] level Log level
/// @return Logger reference
Logger& CreateLoggerByDomainID(ara::core::StringView cid,
                               ara::core::StringView des,
                               std::uint8_t const domainId,
                               ara::log::LogLevel level) noexcept
{
    if (ConditionCheck(cid) == false) {
        LOGVERBOSE("leave:") << __func__;
        return Logger::GetDummyLogger();
    }

    // Execute the unique logic of this overload
    LOGVERBOSE(__func__) << ": determ create logger " << cid.data();
    return g_Local_Share_Worker->GetLoggerByCid(cid.data(), des.data(), domainId, level);
}

/// @brief Status of the remote client
/// @return
ClientState RemoteClientState() noexcept
{
    LOGVERBOSE("enter :") << __func__;
    if (g_Local_Share_Worker) {
        return g_Local_Share_Worker->RemoteClientState();
    }
    return ClientState::kNotConnected;
}

/// @brief When file caching is enabled, flush the cache to the file.
/// @return Execution result
OperState FlushBufferToFile() noexcept
{
    LOGVERBOSE("enter :") << __func__;
    if (g_Local_Share_Worker) {
        return g_Local_Share_Worker->FlushBufferToFile();
    }
    return OperState::kOk;
}
void RegisterConnectionStateHandler(std::function< void(ClientState) > callback) noexcept
{
    if (g_Local_Share_Worker != nullptr) {
        g_Local_Share_Worker->SetConnectionStateHandler(callback);
    }
}

namespace internal {

/// @brief Function called by the internal initialization program
/// @return Initialization result
ara::core::Result< void > Initialize() noexcept
{
    if (g_Local_Share_Worker != nullptr && g_Local_Share_Worker->IsInit()) {
        return ara::core::Result< void >();
    }

    if (g_Local_Share_Worker == nullptr) {
        g_Local_Share_Worker = std::make_shared< internal::LogManager >();
    }

    std::ignore = g_Local_Share_Worker->Init();

    std::ignore = signal(SIGPIPE, Ara_log_internal_Sig_Pipe_Handler);

    return ara::core::Result< void >();
}
/// @brief Called by the deinitialization function
/// @return Deinitialization result
ara::core::Result< void > Deinitialize() noexcept
{
    if (nullptr != g_Local_Share_Worker) {
        /// This must be called before destructor.
        g_Local_Share_Worker->Destroy();
    }
    g_Local_Share_Worker = nullptr;
    return {};
}
bool LogInitialized() noexcept { return g_Local_Share_Worker && g_Local_Share_Worker->IsInit(); }
std::shared_ptr< Logger > ExtGetLoggerPtr(std::string const& cid) noexcept
{
    if (cid.empty() || cid == " ") {
        return nullptr;
    }
    if (g_Local_Share_Worker->IsDestroyed()) {
        return nullptr;
    }
    if (g_Local_Share_Worker) {
        return g_Local_Share_Worker->GetLoggerPtr(cid);
    }
    return nullptr;
}
// thread_local LogManager::DETERM_TLoggerMap LogManager::localDetermLoggers_;

}  // namespace internal

}  // namespace log
}  // namespace ara
