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
/// @file       kv_log_api.h
/// @brief      AutoSar-AP data persistence storage module
/// @details    Error codes
/// @date       2021-06-09
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification Log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// <tr><td>2021-06-09  <td>1.0.0    <td>hanjingjing      <td>Create initial version
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/per/Common/Reusable Functions
/// @interface_level=module
/// @trace_id_sr=
/// @unit_name=
/// @unit_description=
/// @endcode
///
/// ================================================================

#ifndef ARA_PER_PHKV_KV_LOG_API_H_
#define ARA_PER_PHKV_KV_LOG_API_H_

#include "ara/core/string.h"
#include "ara/log/logger.h"
#include "ara/per/internal/common/isoft_assert.h"
#include "ara/per/internal/common/isoft_data_type.h"

namespace ara {
namespace per {
namespace isoftkv {
//********************************/
/// @brief
/// @code{.isoft}
/// @unit_name=GetPerLogCtxID
/// @interface_level=unit
/// @endcode
/// @return
constexpr inline ara::core::StringView GetPerLogCtxID() noexcept { return std::move(ara::core::StringView{"#PER"}); }
/// @brief
/// @code{.isoft}
/// @unit_name=GetPerLogCtxDesc
/// @interface_level=unit
/// @endcode
/// @return
constexpr inline ara::core::StringView GetPerLogCtxDesc() noexcept { return std::move(ara::core::StringView{"PH_KV"}); }
/// @brief Return default logger
/// @code{.isoft}
/// @unit_name=GetWorkLogger
/// @endcode
/// @return
ara::log::Logger* GetWorkLogger() noexcept;
/// @brief Return default logger
/// @code{.isoft}
/// @unit_name=SetWorkLogger
/// @endcode
/// @param pLogger
void SetWorkLogger(ara::log::Logger* const pLogger) noexcept;
/// @brief Initialize default Log
/// @code{.isoft}
/// @unit_name=CreateDefLog
/// @endcode
/// @return
ara::log::Logger* CreateDefLog() noexcept;
/// @brief Initialize Log
/// @code{.isoft}
/// @unit_name=InitLogSystem
/// @interface_level=unit
/// @endcode
/// @param stCtxId
/// @param stCtxDescription
/// @param nLogLevel
/// @return
ara::log::Logger* InitLogSystem(ara::core::StringView const& stCtxId          = GetPerLogCtxID(),
                                ara::core::StringView const& stCtxDescription = GetPerLogCtxDesc(),
                                ara::log::LogLevel const nLogLevel            = ara::log::LogLevel::kInfo) noexcept;
//********************************/
/// @brief
/// @code{.isoft}
/// @unit_name=GetLogStream
/// @interface_level=unit
/// @endcode
/// @param nLogLevel
/// @return
inline ara::log::LogStream GetLogStream(ara::log::LogLevel const nLogLevel) noexcept;
/// @brief Output log of ara::log::LogLevel::kFatal level.
/// @code{.isoft}
/// @unit_name=LogFatal
/// @endcode
/// @return
ara::log::LogStream LogFatal() noexcept;
/// @brief Output log of ara::log::LogLevel::kError level.
/// @code{.isoft}
/// @unit_name=LogError
/// @endcode
/// @return
ara::log::LogStream LogError() noexcept;
/// @brief Output log of ara::log::LogLevel::kWarn level.
/// @code{.isoft}
/// @unit_name=LogWarn
/// @endcode
/// @return
ara::log::LogStream LogWarn() noexcept;
/// @brief Output log of ara::log::LogLevel::kInfo level.
/// @code{.isoft}
/// @unit_name=LogInfo
/// @endcode
/// @return
ara::log::LogStream LogInfo() noexcept;
/// @brief Output log of ara::log::LogLevel::kDebug level.
/// @code{.isoft}
/// @unit_name=LogDebug
/// @endcode
/// @return
ara::log::LogStream LogDebug() noexcept;
/// @brief Output log of ara::log::LogLevel::kVerbose level.
/// @code{.isoft}
/// @unit_name=LogVerbose
/// @endcode
/// @return
ara::log::LogStream LogVerbose() noexcept;
//********************************/
/// @brief
/// @code{.isoft}
/// @unit_name=PConsoleLog
/// @endcode
class PConsoleLog final
{
private:
    /// @brief
    bool bAutoEndLine_{true};

public:
    /// @brief
    /// @param pchEvent
    explicit PConsoleLog(char8_t const* const pchEvent) noexcept;
    /// @brief
    /// @param pchEvent
    /// @param nData
    PConsoleLog(char8_t const* const pchEvent, int32_t const nData) noexcept;
    /// @brief
    /// @param pchEvent
    /// @param nData
    PConsoleLog(char8_t const* const pchEvent, uint32_t const nData) noexcept;
    /// @brief
    /// @param pchEvent
    /// @param nData
    PConsoleLog(char8_t const* const pchEvent, uint64_t const nData) noexcept;
    /// @brief
    /// @param pchEvent
    /// @param pchMsg
    PConsoleLog(char8_t const* const pchEvent, char8_t const* const pchMsg) noexcept;
    /// @brief
    PConsoleLog() = delete;
    /// @brief
    ~PConsoleLog() noexcept;
    /// @brief
    /// @param a
    PConsoleLog(PConsoleLog const& a) = delete;
    /// @brief
    /// @param a
    /// @return
    PConsoleLog& operator=(PConsoleLog const& a) = delete;
    /// @brief
    /// @param a
    PConsoleLog(PConsoleLog&& a) = delete;
    /// @brief
    /// @param a
    /// @return
    PConsoleLog& operator=(PConsoleLog&& a) = delete;

public:
    /// @brief Output integer: int32_t
    /// @param pchMsg
    /// @param nData
    void PrintLog(char8_t const* const pchMsg, int32_t const nData) noexcept;
    /// @brief Output integer: uint8_t
    /// @param pchMsg
    /// @param nData
    void PrintLog(char8_t const* const pchMsg, uint8_t const nData) noexcept;
    /// @brief Output integer: uint16_t
    /// @param pchMsg
    /// @param nData
    void PrintLog(char8_t const* const pchMsg, uint16_t const nData) noexcept;
    /// @brief Output integer: uint32_t
    /// @param pchMsg
    /// @param nData
    void PrintLog(char8_t const* const pchMsg, uint32_t const nData) noexcept;
    /// @brief Output integer: uint64_t
    /// @param pchMsg
    /// @param nData
    void PrintLog(char8_t const* const pchMsg, uint64_t const nData) noexcept;
    /// @brief Output string: Memory
    /// @param pchMsg
    /// @param pchData
    void PrintLog(char8_t const* const pchMsg, char8_t const* const pchData) noexcept;
    /// @brief Output pointer address
    /// @param pchMsg
    /// @param nData
    void PrintLog(char8_t const* const pchMsg, void const* pVoidData) noexcept;
    /// @brief Output string
    /// @param pchMsg
    void PrintLog(char8_t const* const pchMsg) noexcept;
    /// @brief Output newline character
    void PrintLog() noexcept;
    /// @brief
    /// @tparam T
    /// @param pchFormat
    /// @param nData
    /// @throws
    template < typename T >
    void PrintLogEx(char8_t const* const pchFormat, T const nData);
};
/// @brief
/// @code{.isoft}
/// @unit_name=PAraLog
/// @endcode
class PAraLog final
{
public:
    /// @brief
    using LogStream = ara::log::LogStream;

private:
    /// @brief
    LogStream logStream_;
    /// @brief
    bool bAutoEndLine_;

public:
    /// @brief
    /// @param nLogLevel
    /// @param pchEvent
    PAraLog(ara::log::LogLevel const nLogLevel, char8_t const* const pchEvent) noexcept;
    /// @brief
    /// @param nLogLevel
    /// @param pchEvent
    /// @param nData
    PAraLog(ara::log::LogLevel const nLogLevel, char8_t const* const pchEvent, int32_t const nData) noexcept;
    /// @brief
    /// @param nLogLevel
    /// @param pchEvent
    /// @param nData
    PAraLog(ara::log::LogLevel const nLogLevel, char8_t const* const pchEvent, uint32_t const nData) noexcept;
    /// @brief
    /// @param nLogLevel
    /// @param pchEvent
    /// @param pchMsg
    PAraLog(ara::log::LogLevel const nLogLevel, char8_t const* const pchEvent, char8_t const* const pchMsg) noexcept;
    /// @brief
    PAraLog() = delete;
    /// @brief
    ~PAraLog() noexcept;
    /// @brief
    /// @param a
    PAraLog(PAraLog const& a) = delete;
    /// @brief
    /// @param a
    /// @return
    PAraLog& operator=(PAraLog const& a) = delete;
    /// @brief
    /// @param a
    PAraLog(PAraLog&& a) = delete;
    /// @brief
    /// @param a
    /// @return
    PAraLog& operator=(PAraLog&& a) = delete;

public:
    /// @brief
    /// @return
    inline LogStream& GetLocalLogStream() noexcept { return logStream_; }
    /// @brief
    /// @param pchMsg
    void PrintLog(char8_t const* const pchMsg) noexcept;
    /// @brief
    /// @tparam T
    /// @param pchMsg
    /// @param nData
    /// @param pchMsg2
    /// @throws
    template < typename T >
    void PrintLog(char8_t const* const pchMsg, T const nData, char8_t const* const pchMsg2 = nullptr) noexcept;
    /// @brief
    void PrintLog() noexcept;
};
/// @brief
/// @code{.isoft}
/// @unit_name=PPrintLog
/// @endcode
using PPrintLog = PAraLog;
//********************************/
/// @brief Log output class automatically recording runtime
/// @code{.isoft}
/// @unit_name=PAutoTimeRecord
/// @endcode
class PAutoTimeRecord final
{
private:
    /// @brief Identifier
    ara::core::String stFlagWord_;
    /// @brief Statistics start time point
    std::chrono::system_clock::time_point tmStart_;

public:
    /// @brief
    PAutoTimeRecord() = delete;
    /// @brief
    /// @param a
    PAutoTimeRecord(PAutoTimeRecord const& a) = delete;
    /// @brief
    /// @param a
    /// @return
    PAutoTimeRecord& operator=(PAutoTimeRecord const& a) = delete;
    /// @brief
    /// @param a
    PAutoTimeRecord(PAutoTimeRecord&& a) = delete;
    /// @brief
    /// @param a
    /// @return
    PAutoTimeRecord& operator=(PAutoTimeRecord&& a) = delete;
    /// @brief
    /// @param stFlagWord
    /// @throw
    explicit PAutoTimeRecord(ara::core::StringView const& stFlagWord)
        : stFlagWord_{stFlagWord}, tmStart_{std::chrono::system_clock::now()}
    {
    }
    /// @brief
    ~PAutoTimeRecord() noexcept
    {
        std::chrono::system_clock::time_point const tmEnd{std::chrono::system_clock::now()};
        std::chrono::nanoseconds const tmLong{tmEnd - tmStart_};
        LogDebug() << stFlagWord_.c_str() << tmLong.count();
    }
    /// @brief Get elapsed time
    /// @return Returns nanoseconds, 1 second = 1000000000 nanoseconds
    uint64_t GetTimeElapse() const noexcept
    {
        std::chrono::system_clock::time_point const tmEnd{std::chrono::system_clock::now()};
        std::chrono::nanoseconds const tmLong{tmEnd - tmStart_};
        return static_cast< uint64_t >(tmLong.count());
    }
    /// @brief Get elapsed time
    /// @return Returns microseconds, 1 second = 1000000 microseconds
    uint64_t GetTimeElapse_Micro() const noexcept { return static_cast< uint64_t >(GetTimeElapse() / kInt_1000U); }
    /// @brief Get elapsed time: Milliseconds
    /// @return Returns microseconds, 1 second = 1000 milliseconds
    uint64_t GetTimeElapse_Millie() const noexcept
    {
        return static_cast< uint64_t >(GetTimeElapse_Micro() / kInt_1000U);
    }
    /// @brief Update statistics start time point
    void UpdateTimeStart() noexcept { tmStart_ = std::chrono::system_clock::now(); }
    /// @brief Print log information of elapsed time
    /// @param stInfo
    void LogTimeElapseInfo(ara::core::StringView const& stInfo) const noexcept
    {
        uint64_t const nTimeElapse{GetTimeElapse()};
        LogInfo() << stFlagWord_.c_str() << ":" << stInfo.data() << "=" << nTimeElapse;
    }
};
//********************************/
}  // namespace isoftkv
}  // namespace per
}  // namespace ara

#endif
