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
/// @file       isoft_log_api.h
/// @brief      AutoSar-Crypto log module
/// @details    Error code
/// @date       2021-06-09
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Reusable Functions/Reusable Functions Module
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_06005
/// @unit_name=Common_api
/// @endcode
///
/// ================================================================

#ifndef ARA_COM_PUHUA_LOG_API_H_
#define ARA_COM_PUHUA_LOG_API_H_
#include <cstdint>

#include "ara/core/string.h"
#include "ara/crypto/common/isoft_data_type.h"
#include "ara/log/logger.h"

namespace ara {
namespace crypto {
namespace isoft_def {
//********************************/
/// @brief Get the Crypto module log context identifier
/// @return Log context identifier
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00007
/// @trace_id_dd=DD_CRYPTO_00198
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = dd
/// @endcode
inline constexpr ara::crypto::char8_t const* GetkCryptoLogCtxID() noexcept { return "#CRY"; }
/// @brief Get the Crypto module log context description
/// @return Log context description
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00007
/// @trace_id_dd=DD_CRYPTO_00199
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = dd
/// @endcode
inline constexpr ara::crypto::char8_t const* GetkCryptoLogCtxDesc() noexcept { return "PuHua Crypto"; }
/// @brief Return the working logging system
/// @return Logger pointer instance
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00007
/// @trace_id_dd=DD_CRYPTO_00200
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = dd
/// @endcode
ara::log::Logger* GetWorkLogger() noexcept;
/// @brief Set the working logging system
/// @param pLogger Pointer to ara logging system object
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00007
/// @trace_id_dd=DD_CRYPTO_00201
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = dd
/// @endcode
void SetWorkLogger(ara::log::Logger* const pLogger) noexcept;
/// @brief Create the default logging system
/// @return Logger pointer instance
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00007
/// @trace_id_dd=DD_CRYPTO_00202
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = dd
/// @endcode
ara::log::Logger* CreateDefLog() noexcept;
/// @brief Initialize the logging system
/// @param ctxId Log output category identifier
/// @param ctxDescription Log category description string
/// @param nLogLevel Log output level
/// @return Logger pointer instance
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00007
/// @trace_id_dd=DD_CRYPTO_00203
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = dd
/// @endcode
ara::log::Logger* InitLogSystem(ara::core::StringView const& ctxId,
                                ara::core::StringView const& ctxDescription,
                                ara::log::LogLevel const nLogLevel = ara::log::LogLevel::kInfo) noexcept;
//********************************/
/// @brief Get a log stream object of a specific type
/// @param nLogLevel Log output level
/// @return Log stream object
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00007
/// @trace_id_dd=DD_CRYPTO_00204
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = dd
/// @endcode
ara::log::LogStream GetLogStreamByLevel(ara::log::LogLevel const nLogLevel) noexcept;
/// @brief Get log stream object: fatal
/// @return Log stream object
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00007
/// @trace_id_dd=DD_CRYPTO_00205
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = dd
/// @endcode
ara::log::LogStream LogFatal() noexcept;
/// @brief Get log stream object: error
/// @return Log stream object
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00007
/// @trace_id_dd=DD_CRYPTO_00206
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = dd
/// @endcode
ara::log::LogStream LogError() noexcept;
/// @brief Get log stream object: warning
/// @return Log stream object
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00007
/// @trace_id_dd=DD_CRYPTO_00207
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = dd
/// @endcode
ara::log::LogStream LogWarn() noexcept;
/// @brief Get log stream object: info
/// @return Log stream object
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00007
/// @trace_id_dd=DD_CRYPTO_00208
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = dd
/// @endcode
ara::log::LogStream LogInfo() noexcept;
/// @brief Get log stream object: debug
/// @return Log stream object
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00007
/// @trace_id_dd=DD_CRYPTO_00209
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = dd
/// @endcode
ara::log::LogStream LogDebug() noexcept;
/// @brief Get log stream object: verbose
/// @return Log stream object
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00007
/// @trace_id_dd=DD_CRYPTO_00210
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = dd
/// @endcode
ara::log::LogStream LogVerbose() noexcept;
//********************************/
/// @brief Console output log wrapper class
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00007
/// @trace_id_dd=DD_CRYPTO_00211
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
class PConsoleLog
{
private:
    /// @brief Whether to automatically wrap lines
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00212
    /// @needwork = dda
    /// @endcode
    bool bAutoEndLine_{true};

public:
    /// @brief Default constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00213
    /// @needwork = dda
    /// @endcode
    PConsoleLog() noexcept = default;
    /// @brief Parameterized constructor
    /// @param pchEvent Log message
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00214
    /// @needwork = dda
    /// @endcode
    explicit PConsoleLog(ara::crypto::char8_t const* const pchEvent) noexcept;
    /// @brief Parameterized constructor
    /// @param pchEvent Log message
    /// @param nData Integer log data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00215
    /// @needwork = dda
    /// @endcode
    PConsoleLog(ara::crypto::char8_t const* const pchEvent, int32_t const nData) noexcept;
    /// @brief Parameterized constructor
    /// @param pchEvent Log message
    /// @param nData Integer log data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00216
    /// @needwork = dda
    /// @endcode
    PConsoleLog(ara::crypto::char8_t const* const pchEvent, uint32_t const nData) noexcept;
    /// @brief Parameterized constructor
    /// @param pchEvent Log message
    /// @param nData Integer log data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00217
    /// @needwork = dda
    /// @endcode
    PConsoleLog(ara::crypto::char8_t const* const pchEvent, uint64_t const nData) noexcept;
    /// @brief Parameterized constructor
    /// @param pchEvent Log message
    /// @param pchMsg Log message string
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00218
    /// @needwork = dda
    /// @endcode
    PConsoleLog(ara::crypto::char8_t const* const pchEvent, ara::crypto::char8_t const* const pchMsg) noexcept;
    /// @brief Constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00219
    /// @needwork = dda
    /// @endcode
    PConsoleLog(PConsoleLog const& other) = delete;
    /// @brief Constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00220
    /// @needwork = dda
    /// @endcode
    PConsoleLog(PConsoleLog&& other) = delete;
    /// @brief Default copy assignment operator
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00221
    /// @needwork = dda
    /// @endcode
    PConsoleLog& operator=(PConsoleLog const& other) = delete;
    /// @brief Default move assignment operator
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00222
    /// @needwork = dda
    /// @endcode
    PConsoleLog& operator=(PConsoleLog&& other) = delete;
    /// @brief Destructor
    /// @name      ~PConsoleLog
    /// @returns
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00223
    /// @needwork = dda
    /// @endcode
    virtual ~PConsoleLog() noexcept;
    /// @brief Output log: pchMsg + int32_t
    /// @name   PrintLog
    /// @param pchMsg Log message string
    /// @param nData Integer log data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00224
    /// @needwork = dda
    /// @endcode
    void PrintLog(ara::crypto::char8_t const* const pchMsg, int32_t const nData) noexcept;
    /// @brief Output log: pchMsg + uint32_t
    /// @name   PrintLog
    /// @param pchMsg Log message string
    /// @param nData Integer log data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00225
    /// @needwork = dda
    /// @endcode
    void PrintLog(ara::crypto::char8_t const* const pchMsg, uint32_t const nData) noexcept;
    /// @brief Output log: pchMsg + uint64_t
    /// @name   PrintLog
    /// @param pchMsg Log message string
    /// @param nData Integer log data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00226
    /// @needwork = dda
    /// @endcode
    void PrintLog(ara::crypto::char8_t const* const pchMsg, uint64_t const nData) noexcept;
    /// @brief Output log: pchMsg + char8_t *
    /// @name   PrintLog
    /// @param pchMsg Log message string
    /// @param pchData String message
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00227
    /// @needwork = dda
    /// @endcode
    void PrintLog(ara::crypto::char8_t const* const pchMsg, ara::crypto::char8_t const* const pchData) noexcept;
    /// @brief Output log: char8_t *
    /// @name   PrintLog
    /// @param pchMsg Log message string
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00228
    /// @needwork = dda
    /// @endcode
    void PrintLog(ara::crypto::char8_t const* const pchMsg) noexcept;
    /// @brief Output log extended: pchFormat + extended types
    /// @param pchFormat Log output format string
    /// @param nData Integer log data
    /// @code{.isoft}
    /// @tparam T
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00229
    /// @needwork = dda
    /// @endcode
    template < typename T >
    inline void PrintLogEx(ara::crypto::char8_t const* const pchFormat,
                           T const nData) noexcept(noexcept(printf(pchFormat, nData)));
    /// @brief Output log: newline
    /// @name      PrintLog
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00230
    /// @needwork = dda
    /// @endcode
    void PrintLog() noexcept;
};
/// @brief Wrapper class using the ara logging system
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00008
/// @trace_id_dd=DD_CRYPTO_00231
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
class PAraLog
{
public:
    /// @brief Log stream type alias
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00008
    /// @trace_id_dd=DD_CRYPTO_06239
    /// @needwork = dd
    /// @endcode
    using LogStream = ara::log::LogStream;

private:
    /// @brief Log stream object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00232
    /// @needwork = dda
    /// @endcode
    LogStream logStream_;
    /// @brief Whether to automatically wrap lines
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00233
    /// @needwork = dda
    /// @endcode
    bool bAutoEndLine_;
    /// @brief Parameterized constructor
    /// @param nLogLevel Log output level
    /// @param pchEvent Log message
    /// @param nData Integer log data
    /// @param bAutoEndLine Whether to automatically add a newline
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00234
    /// @needwork = dda
    /// @endcode
    PAraLog(ara::log::LogLevel const nLogLevel,
            ara::crypto::char8_t const* const pchEvent,
            uint32_t const nData,
            bool const bAutoEndLine) noexcept;

public:
    /// @brief Default constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00235
    /// @needwork = dda
    /// @endcode
    PAraLog() noexcept = delete;
    /// @brief Parameterized constructor
    /// @name PAraLog
    /// @param nLogLevel Log output level
    /// @param pchEvent Log message
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00236
    /// @needwork = dda
    /// @endcode
    PAraLog(ara::log::LogLevel const nLogLevel, ara::crypto::char8_t const* const pchEvent) noexcept;
    /// @brief Parameterized constructor
    /// @name  PAraLog
    /// @param nLogLevel Log output level
    /// @param pchEvent Log message
    /// @param nData Integer log data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00237
    /// @needwork = dda
    /// @endcode
    PAraLog(ara::log::LogLevel const nLogLevel,
            ara::crypto::char8_t const* const pchEvent,
            uint32_t const nData) noexcept;
    /// @brief Parameterized constructor
    /// @name PAraLog
    /// @param nLogLevel Log output level
    /// @param pchEvent Log message
    /// @param pchMsg Log message string
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00238
    /// @needwork = dda
    /// @endcode
    PAraLog(ara::log::LogLevel const nLogLevel,
            ara::crypto::char8_t const* const pchEvent,
            ara::crypto::char8_t const* const pchMsg) noexcept;
    /// @brief Constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00239
    /// @needwork = dda
    /// @endcode
    PAraLog(PAraLog const& other) = delete;
    /// @brief Constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00240
    /// @needwork = dda
    /// @endcode
    PAraLog(PAraLog&& other) = delete;
    /// @brief Default copy assignment operator
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00241
    /// @needwork = dda
    /// @endcode
    PAraLog& operator=(PAraLog const& other) = delete;
    /// @brief Default move assignment operator
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00242
    /// @needwork = dda
    /// @endcode
    PAraLog& operator=(PAraLog&& other) = delete;
    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00243
    /// @needwork = dda
    /// @endcode
    virtual ~PAraLog() noexcept;
    /// @brief Output log: pchMsg
    /// @name   PrintLog
    /// @param pchMsg Log message string
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00245
    /// @needwork = dda
    /// @endcode
    void PrintLog(ara::crypto::char8_t const* const pchMsg) noexcept;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00246
    /// @needwork = dda
    /// @endcode
    template < typename T >
    /// @brief Output log template class: pchMsg + T + pchMsg2
    /// @name   PrintLog
    /// @param pchMsg Log message string
    /// @param nData Integer log data
    /// @param pchMsg2 Log message string 2
    inline void PrintLog(ara::crypto::char8_t const* const pchMsg,
                         T const nData,
                         ara::crypto::char8_t const* const pchMsg2 = nullptr) noexcept;
    /// @brief Output log: force flush
    /// @name      PrintLog
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00247
    /// @needwork = dda
    /// @endcode
    void PrintLog() noexcept;
};
/// @brief Type alias for PAraLog
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00008
/// @trace_id_dd=DD_CRYPTO_06240
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = dd
/// @endcode
using PPrintLog = PAraLog;
//********************************/
}  // namespace  isoft_def
}  // namespace crypto
}  // namespace ara

#endif  // ARA_COM_PUHUA_LOG_API_H_
