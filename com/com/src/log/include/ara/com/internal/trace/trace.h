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
/// @file       trace.h
/// @brief
/// @details
/// @date       2025-01-17
/// @author     jiusen.cui
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_COM_INTERNAL_TRACE_TRACE_H_
#define ARA_COM_INTERNAL_TRACE_TRACE_H_
#include <sstream>

#include "ara/com/internal/log/log.h"
#ifdef MONITOR_LOG_V2
    #include "ara/com/internal/trace/dds/compatible.h"
    #include "ara/com/internal/trace/someip/compatible.h"
    #include "ara/com/internal/trace/userdefine/compatible.h"
#endif
#ifdef MONITOR_LOG_V3
    #include "ara/com/internal/trace/dds/messages.h"
    #include "ara/com/internal/trace/someip/messages.h"
    #include "ara/com/internal/trace/userdefine/messages.h"
#endif

#include "isoft/manifestreader/manifest.h"

namespace ara {
namespace com {
namespace internal {
namespace trace {

/// @brief Service type
enum class ServiceType : uint8_t
{
    kSkeleton = 0,
    kProxy    = 1
};
/// @brief Message type
enum class MsgType : uint8_t
{
    kMethodRequest         = 0x00,
    kMethodRequestNoReturn = 0x01,
    kEventNotify           = 0x02,
    kMethodResponse        = 0x80,
    kMethodError           = 0x81,
    kUnknown               = 0xFF
};
/// @brief Instance status
enum class InstanceState : uint8_t
{
    kStateInitial  = 0x00,
    kStateStarting = 0x01,
    kStateRunning  = 0x02,
    kStateStopping = 0x03,
    kStateStop     = 0x04,
    kStateError    = 0x05,
    kUnknown       = 0xFF
};

/// @brief
/// @tparam Arg
/// @param[in] res
/// @param[in] arg
template < typename Arg >
inline void ExpandSStream(std::stringstream& res, Arg arg) noexcept
{
    res << FmtStr(arg).r;
}
/// @brief
/// @tparam First
/// @tparam Args
/// @param[in] res
/// @param[in] first
/// @param[in] args
template < typename First, typename... Args >
inline void ExpandSStream(std::stringstream& res, First first, Args... args) noexcept
{
    res << FmtStr(first).r << " ";
    ExpandSStream(res, args...);
}

/// @brief Monitoring configuration
struct MonitorConfig
{
    /// @brief Use #CMM log channel
    bool useCMMLogger{true};
    /// @brief Use #COM log channel
    bool useCOMLogger{false};
    /// @brief Output console debug logs
    bool printDebugLog{false};
    /// @brief Payload output level
    uint16_t payloadOutputLevel{};
    /// @brief Payload maximum length (DLT log maximum byte count)
    uint32_t maxPayloadSize{0x00010000 - 0x00001000 - 0x00000007};
    auto ToString() noexcept
    {
        return FmtStr(GenK2V("#CMM", useCMMLogger), GenK2V("#COM", useCOMLogger),
                      GenK2V("printDebugLog", printDebugLog), GenK2V("payloadOutputLevel", payloadOutputLevel),
                      GenK2V("maxPayloadSize", maxPayloadSize));
    };
};

/// @brief
class ComTrace
{
public:
    /// @brief Constructor
    ComTrace() noexcept { ComLogTrace("create com trace"); };
    /// @brief Destructor
    virtual ~ComTrace() noexcept { ComLogTrace("destroy com trace"); };

    /// @brief Copy constructor
    /// @param[in] other the object to be copy.
    ComTrace(ComTrace const& other) noexcept = default;

    /// @brief Copy assignment operator
    /// @param[in] other the object to be copy.
    /// @return ComTrace object
    ComTrace& operator=(ComTrace const& other) noexcept = default;

    /// @brief Move constructor
    /// @param[in] other the object to be move.
    ComTrace(ComTrace&& other) noexcept = default;

    /// @brief move assignment operator of the ComTrace
    /// @param[in] other the object to be move.
    /// @return ComTrace object
    ComTrace& operator=(ComTrace&& other) noexcept = default;
    /// @brief Record communication monitoring log
    /// @tparam MsgId
    /// @tparam Params
    /// @param id
    /// @param args
    template < typename MsgId, typename... Params >
    inline void Log(const MsgId& id, const Params&... args) noexcept;
    /// @brief
    /// @return
    uint16_t GetTraceLevel() const noexcept { return config_.maxPayloadSize; }

public:
    /// @brief
    /// @param path
    /// @return
    ara::core::Result< void > LoadConfig(ara::core::StringView const path) noexcept;

    static ComTrace& Instance() noexcept
    {
        static ComTrace s_Instance;
        return s_Instance;
    }

private:
    /// @brief
    /// @tparam MsgId
    /// @tparam Params
    /// @param id
    /// @param args
    template < typename MsgId, typename... Params >
    void _dumpDebugLog(const MsgId& id, const Params&... args) noexcept
    {
        std::stringstream buffer;
        ExpandSStream(buffer, args...);
        ComLogDebug("Log:", GenK2V("id", id.id), buffer.str());
    }

private:
    /// @brief Use #CMM log channel
    MonitorConfig config_;
};

template < typename MsgId, typename... Params >
inline void ComTrace::Log(const MsgId& id, const Params&... args) noexcept
{
#ifdef HAS_ARA_LOG
    if (config_.printDebugLog) {
        _dumpDebugLog(id, args...);
    }
    if (config_.useCMMLogger) {
        auto& logger = ara::com::internal::log::Logger::GetInstance< ara::com::internal::log::LoggerComMonitor >();
        if (logger.GetLogger()) {
            logger.GetLogger()->Log(id, args...);
        }
    }
    if (config_.useCOMLogger) {
        auto& logger = ara::com::internal::log::Logger::GetInstance< ara::com::internal::log::LoggerCom >();
        if (logger.GetLogger()) {
            logger.GetLogger()->Log(id, args...);
        }
    }
#else
    ComLogError("Monitor Trace need ARA LOG");
#endif
}

/// @brief Format communication monitoring log payload
/// @return Formatted payload string
inline ara::core::String FormatArgs() noexcept { return "{};"; }

/// @brief Format communication monitoring log payload
/// @tparam Args
/// @param args
/// @return Formatted payload string
template < typename... Args >
inline ara::core::String FormatArgs(const Args&... args) noexcept
{
    std::stringstream ss;
    std::stringstream buffer;
    buffer << "{";
    ExpandSStream(ss, args...);
    auto s{ss.str()};
    if (s.length() <= ComTrace::Instance().GetTraceLevel()) {
        buffer << s;
    } else {
        buffer << s.substr(0, ComTrace::Instance().GetTraceLevel()) << "...";
    }
    buffer << "};";
    return ara::core::String(buffer.str());
}

/// @brief Format communication monitoring log payload
/// @tparam T
/// @param value
/// @return Formatted payload string
template < typename T >
inline ara::core::String FormatValue(const T& value) noexcept
{
    std::stringstream buffer;
    buffer << "{";
    auto s{FmtStr(value).r};
    if (s.length() <= ComTrace::Instance().GetTraceLevel()) {
        buffer << s;
    } else {
        buffer << s.substr(0, ComTrace::Instance().GetTraceLevel()) << "...";
    }
    buffer << "};";
    return ara::core::String(buffer.str());
}

}  // namespace trace
}  // namespace internal
}  // namespace com
}  // namespace ara

#endif  // ARA_COM_INTERNAL_TRACE_TRACE_H_
