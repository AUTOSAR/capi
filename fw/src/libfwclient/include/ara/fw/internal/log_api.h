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
/// @file       log_api.h
/// @brief      AutoSar-fw log module
/// @details    Log
/// @date       2025-01-22
/// @author     jzy
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/fw/common processing
/// export_level=module
/// @interface_level=module
/// @trace_id_sr=SR_FW_0001
/// @unit_name=Logapi
/// @endcode
///
/// ================================================================
///
///
/// ================================================================

#ifndef ARA_FW_LOG_API_H_
#define ARA_FW_LOG_API_H_
#include <cstdint>

#include "ara/core/string.h"
#include "ara/fw/common/common.h"
#include "ara/log/logger.h"

namespace ara {
namespace fw {
namespace internal {
//********************************/
/// @brief Get Crypto module log context identifier
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00120
/// @trace_id_dd=DD_FW_00197
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkCryptoLogCtxID() noexcept { return "#FWD"; }
/// @brief Get Crypto module log context description
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00121
/// @trace_id_dd=DD_FW_00198
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkCryptoLogCtxDesc() noexcept { return "firewall daemon"; }
/// @brief Return the working log system
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00122
/// @trace_id_dd=DD_FW_00199
/// @needwork = ad
/// @endcode
ara::log::Logger *GetWorkLogger() noexcept;
/// @brief Set the working log system
/// @param pLogger ara log system object pointer
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00123
/// @trace_id_dd=DD_FW_00200
/// @needwork = ad
/// @endcode
void SetWorkLogger(ara::log::Logger *const pLogger) noexcept;
/// @brief Create default log system
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00124
/// @trace_id_dd=DD_FW_00201
/// @needwork = ad
/// @endcode
ara::log::Logger *CreateDefLog() noexcept;
/// @brief Initialize log system
/// @param ctxId Log output category identifier
/// @param ctxDescription Log category description string
/// @param nLogLevel Log output level
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00125
/// @trace_id_dd=DD_FW_00202
/// @needwork = ad
/// @endcode
ara::log::Logger *InitLogSystem(ara::core::StringView const &ctxId,
                                ara::core::StringView const &ctxDescription,
                                ara::log::LogLevel const nLogLevel = ara::log::LogLevel::kInfo) noexcept;
//********************************/
/// @brief Get specific type log stream object
/// @param nLogLevel Log output level
/// @return
/// @throws
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00126
/// @trace_id_dd=DD_FW_00203
/// @needwork = ad
/// @endcode
ara::log::LogStream GetLogStream(ara::log::LogLevel const nLogLevel) noexcept;
/// @brief Get log stream object: Fatal
/// @return
/// @throws
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00127
/// @trace_id_dd=DD_FW_00204
/// @needwork = ad
/// @endcode
ara::log::LogStream LogFatal() noexcept;
/// @brief Get log stream object: Error
/// @return
/// @throws
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00128
/// @trace_id_dd=DD_FW_00205
/// @needwork = ad
/// @endcode
ara::log::LogStream LogError() noexcept;
/// @brief Get log stream object: Warning
/// @return
/// @throws
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00129
/// @trace_id_dd=DD_FW_00206
/// @needwork = ad
/// @endcode
ara::log::LogStream LogWarn() noexcept;
/// @brief Get log stream object: Info
/// @return
/// @throws
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00130
/// @trace_id_dd=DD_FW_00207
/// @needwork = ad
/// @endcode
ara::log::LogStream LogInfo() noexcept;
/// @brief Get log stream object: Debug
/// @return
/// @throws
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00131
/// @trace_id_dd=DD_FW_00208
/// @needwork = ad
/// @endcode
ara::log::LogStream LogDebug() noexcept;
/// @brief Get log stream object: Verbose
/// @return
/// @throws
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00132
/// @trace_id_dd=DD_FW_00209
/// @needwork = ad
/// @endcode
ara::log::LogStream LogVerbose() noexcept;
//********************************/
/// @brief Console output log wrapper class
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00133
/// @trace_id_dd=DD_FW_00210
/// @needwork = ad
/// @endcode
class PConsoleLog
{
private:
    /// @brief Whether to auto newline
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00211
    /// @needwork = dda
    /// @endcode
    bool bAutoEndLine_{true};

public:
    /// @brief Default constructor
    /// @name
    /// @returns
    /// @throws
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00134
    /// @trace_id_dd=DD_FW_00212
    /// @needwork = ad
    /// @endcode
    PConsoleLog() = default;
    /// @brief Parameterized constructor
    /// @name
    /// @param pchEvent Log message
    /// @returns
    /// @throws
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00135
    /// @trace_id_dd=DD_FW_00213
    /// @needwork = ad
    /// @endcode
    explicit PConsoleLog(char8_t const *const pchEvent);
    /// @brief Parameterized constructor
    /// @name
    /// @param pchEvent Log message
    /// @param nData Integer log data
    /// @returns
    /// @throws
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00136
    /// @trace_id_dd=DD_FW_00214
    /// @needwork = ad
    /// @endcode
    PConsoleLog(char8_t const *const pchEvent, int32_t const nData);
    /// @brief Parameterized constructor
    /// @name
    /// @param pchEvent Log message
    /// @param nData Integer log data
    /// @returns
    /// @throws
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00137
    /// @trace_id_dd=DD_FW_00215
    /// @needwork = ad
    /// @endcode
    PConsoleLog(char8_t const *const pchEvent, uint32_t const nData);
    /// @brief Parameterized constructor
    /// @name
    /// @param pchEvent Log message
    /// @param nData Integer log data
    /// @returns
    /// @throws
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00138
    /// @trace_id_dd=DD_FW_00216
    /// @needwork = ad
    /// @endcode
    PConsoleLog(char8_t const *const pchEvent, uint64_t const nData);
    /// @brief Parameterized constructor
    /// @name
    /// @param pchEvent Log message
    /// @param pchMsg Log message string
    /// @returns
    /// @throws
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00139
    /// @trace_id_dd=DD_FW_00217
    /// @needwork = ad
    /// @endcode
    PConsoleLog(char8_t const *const pchEvent, char8_t const *const pchMsg);
    /// @brief Constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00140
    /// @trace_id_dd=DD_FW_00218
    /// @needwork = ad
    /// @endcode
    PConsoleLog(PConsoleLog const &other) = delete;
    /// @brief Constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00141
    /// @trace_id_dd=DD_FW_00219
    /// @needwork = ad
    /// @endcode
    PConsoleLog(PConsoleLog &&other) = delete;
    /// @brief Default copy assignment function
    /// @param other Another object instance of this class
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00142
    /// @trace_id_dd=DD_FW_00220
    /// @needwork = ad
    /// @endcode
    PConsoleLog &operator=(PConsoleLog const &other) = delete;
    /// @brief Default move assignment function
    /// @param other Another object instance of this class
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00143
    /// @trace_id_dd=DD_FW_00221
    /// @needwork = ad
    /// @endcode
    PConsoleLog &operator=(PConsoleLog &&other) = delete;
    /// @brief Destructor
    /// @name      ~PConsoleLog
    /// @returns
    /// @throws
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00144
    /// @trace_id_dd=DD_FW_00222
    /// @needwork = ad
    /// @endcode
    virtual ~PConsoleLog() noexcept;
    /// @brief Output log: pchMsg + int32_t
    /// @name   PrintLog
    /// @param pchMsg Log message string
    /// @param nData Integer log data
    /// @returns
    /// @throws
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00145
    /// @trace_id_dd=DD_FW_00223
    /// @needwork = ad
    /// @endcode
    void PrintLog(char8_t const *const pchMsg, int32_t const nData);
    /// @brief Output log: pchMsg + uint32_t
    /// @name   PrintLog
    /// @param pchMsg Log message string
    /// @param nData Integer log data
    /// @returns
    /// @throws
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00146
    /// @trace_id_dd=DD_FW_00224
    /// @needwork = ad
    /// @endcode
    void PrintLog(char8_t const *const pchMsg, uint32_t const nData);
    /// @brief Output log: pchMsg + uint64_t
    /// @name   PrintLog
    /// @param pchMsg Log message string
    /// @param nData Integer log data
    /// @returns
    /// @throws
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00147
    /// @trace_id_dd=DD_FW_00225
    /// @needwork = ad
    /// @endcode
    void PrintLog(char8_t const *const pchMsg, uint64_t const nData);
    /// @brief Output log: pchMsg + char8_t *
    /// @name   PrintLog
    /// @param pchMsg Log message string
    /// @param pchData String message
    /// @returns
    /// @throws
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00148
    /// @trace_id_dd=DD_FW_00226
    /// @needwork = ad
    /// @endcode
    void PrintLog(char8_t const *const pchMsg, char8_t const *const pchData);
    /// @brief Output log: char8_t *
    /// @name   PrintLog
    /// @param pchMsg Log message string
    /// @returns
    /// @throws
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00149
    /// @trace_id_dd=DD_FW_00227
    /// @needwork = ad
    /// @endcode
    void PrintLog(char8_t const *const pchMsg);
    /// @brief Output log extension: pchFormat + extended type
    /// @param pchFormat Log output format string
    /// @param nData Integer log data
    /// @throws
    /// @code{.isoft}
    /// @tparam T
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00150
    /// @trace_id_dd=DD_FW_00228
    /// @needwork = ad
    /// @endcode
    template < typename T >
    inline void PrintLogEx(char8_t const *const pchFormat, T const nData);
    /// @brief Output log: newline
    /// @name      PrintLog
    /// @returns
    /// @throws
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00151
    /// @trace_id_dd=DD_FW_00229
    /// @needwork = ad
    /// @endcode
    void PrintLog();
};
/// @brief Wrapper class using ara log system
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00152
/// @trace_id_dd=DD_FW_00230
/// @needwork = ad
/// @endcode
class PAraLog
{
public:
    /// @brief Log stream type alias
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00152
    /// @trace_id_dd=DD_FW_00230
    /// @needwork = ad
    /// @endcode
    using LogStream = ara::log::LogStream;

private:
    /// @brief Log stream object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00231
    /// @needwork = dda
    /// @endcode
    LogStream logStream_;
    /// @brief Whether to auto newline
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00232
    /// @needwork = dda
    /// @endcode
    bool bAutoEndLine_{};
    /// @brief Parameterized constructor
    /// @param nLogLevel Log output level
    /// @param pchEvent Log message
    /// @param nData Integer log data
    /// @param bAutoEndLine Whether to auto add newline
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00233
    /// @needwork = dda
    /// @endcode
    PAraLog(ara::log::LogLevel const nLogLevel,
            char8_t const *const pchEvent,
            uint32_t const nData,
            bool const bAutoEndLine) noexcept;

public:
    /// @brief Parameterized constructor
    /// @name
    /// @param nLogLevel Log output level
    /// @param pchEvent Log message
    /// @returns
    /// @throws
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00153
    /// @trace_id_dd=DD_FW_00234
    /// @needwork = ad
    /// @endcode
    PAraLog(ara::log::LogLevel const nLogLevel, char8_t const *const pchEvent) noexcept;
    /// @brief Parameterized constructor
    /// @name
    /// @param nLogLevel Log output level
    /// @param pchEvent Log message
    /// @param nData Integer log data
    /// @returns
    /// @throws
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00154
    /// @trace_id_dd=DD_FW_00235
    /// @needwork = ad
    /// @endcode
    PAraLog(ara::log::LogLevel const nLogLevel, char8_t const *const pchEvent, uint32_t const nData) noexcept;
    /// @brief Parameterized constructor
    /// @name
    /// @param nLogLevel Log output level
    /// @param pchEvent Log message
    /// @param pchMsg Log message string
    /// @returns
    /// @throws
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00155
    /// @trace_id_dd=DD_FW_00236
    /// @needwork = ad
    /// @endcode
    PAraLog(ara::log::LogLevel const nLogLevel, char8_t const *const pchEvent, char8_t const *const pchMsg) noexcept;
    /// @brief Constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00156
    /// @trace_id_dd=DD_FW_00237
    /// @needwork = ad
    /// @endcode
    PAraLog(PAraLog const &other) = delete;
    /// @brief Constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00157
    /// @trace_id_dd=DD_FW_00238
    /// @needwork = ad
    /// @endcode
    PAraLog(PAraLog &&other) = delete;
    /// @brief Default copy assignment function
    /// @param other Another object instance of this class
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00158
    /// @trace_id_dd=DD_FW_00239
    /// @needwork = ad
    /// @endcode
    PAraLog &operator=(PAraLog const &other) = delete;
    /// @brief Default move assignment function
    /// @param other Another object instance of this class
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00159
    /// @trace_id_dd=DD_FW_00240
    /// @needwork = ad
    /// @endcode
    PAraLog &operator=(PAraLog &&other) = delete;
    /// @brief Destructor
    /// @name      ~PAraLo
    /// @returns
    /// @throws
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00160
    /// @trace_id_dd=DD_FW_00241
    /// @needwork = ad
    /// @endcode
    virtual ~PAraLog() noexcept;
    /// @brief Get log stream object
    /// @name      GetLogStream
    /// @returns
    /// @throws
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00161
    /// @trace_id_dd=DD_FW_00242
    /// @needwork = ad
    /// @endcode
    LogStream &GetLogStream() noexcept;
    /// @brief Output log: pchMsg
    /// @name   PrintLog
    /// @param pchMsg Log message string
    /// @returns
    /// @throws
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00162
    /// @trace_id_dd=DD_FW_00243
    /// @needwork = ad
    /// @endcode
    void PrintLog(char8_t const *const pchMsg) noexcept;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00163
    /// @trace_id_dd=DD_FW_00244
    /// @needwork = ad
    /// @endcode
    template < typename T >
    /// @brief Output log template class: pchMsg + T + pchMsg2
    /// @name   PrintLog
    /// @param pchMsg Log message string
    /// @param nData Integer log data
    /// @param pchMsg2 Log message string 2
    /// @returns
    /// @throws
    inline void PrintLog(char8_t const *const pchMsg, T const nData, char8_t const *const pchMsg2 = nullptr) noexcept;
    /// @brief Output log: force flush to disk
    /// @name      PrintLog
    /// @returns
    /// @throws
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00164
    /// @trace_id_dd=DD_FW_00245
    /// @needwork = ad
    /// @endcode
    void PrintLog() noexcept;
};
/// @brief Type alias for PAraLog
using PPrintLog = PAraLog;
//********************************/
}  // namespace internal
}  // namespace fw
}  // namespace ara

#endif  // ARA_FW_LOG_API_H_
