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
/// @file       kv_object.cpp
/// @brief      AutoSar-AP data persistence storage module
/// @details    KV system management base class
/// @date       2021-06-30
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification Log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// <tr><td>2021-06-30  <td>1.0.0    <td>hanjingjing      <td>Create initial version
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/per/Common/Reusable Functions
/// @interface_level=module
/// @trace_id_sr=
/// @unit_name=PKvObject
/// @unit_description=Persistence module log output class
/// @endcode
///
/// ================================================================

#include "ara/per/internal/isoftkv/kv_object.h"

#include "ara/per/internal/isoftkv/kv_log_api.h"

namespace {
/// @brief
/// @tparam T_Data
/// @param logStream
/// @param pchKey
/// @param nValue
/// @param bComma
template < typename T_Data >
inline void G_PrintLogKeyValue(ara::log::LogStream &logStream,
                               ara::per::char8_t const *const pchKey,
                               T_Data const nValue,
                               bool const bComma = false) noexcept
{
    if (nullptr != pchKey) {
        if (bComma) {
            logStream << "," << pchKey;
        } else {
            logStream << pchKey;
        }
        if ('\0' != *(pchKey + 1)) {
            logStream << "=";
        }
        logStream << nValue;
    }
}
}  // namespace

namespace ara {
namespace per {
namespace isoftkv {
//********************************/
/// @brief Output log level: ara::log::LogLevel::kVerbose
/// @param pchLogKey
/// @param pchMsg
/// @param pchKey1
/// @param nValue1
/// @param pchKey2
/// @param nValue2
/// @param pchKey3
/// @param nValue3
/// @param pchKey4
/// @param nValue4
/// @param pchKey5
/// @param nValue5
/// @param pchKey6
/// @param nValue6
void PKvObject::_LogDetail(char8_t const *const pchLogKey,
                           char8_t const *const pchMsg,
                           char8_t const *const pchKey1,
                           int32_t const nValue1,
                           char8_t const *const pchKey2,
                           int32_t const nValue2,
                           char8_t const *const pchKey3,
                           int32_t const nValue3,
                           char8_t const *const pchKey4,
                           int32_t const nValue4,
                           char8_t const *const pchKey5,
                           int32_t const nValue5,
                           char8_t const *const pchKey6,
                           int32_t const nValue6) const noexcept
{
    _LogPrint(ara::log::LogLevel::kVerbose, pchLogKey, pchMsg, pchKey1, nValue1, pchKey2, nValue2, pchKey3, nValue3,
              pchKey4, nValue4, pchKey5, nValue5, pchKey6, nValue6);
}
/// @brief Output log level: ara::log::LogLevel::kDebug
/// @param pchLogKey
/// @param pchMsg
/// @param pchKey1
/// @param nValue1
/// @param pchKey2
/// @param nValue2
/// @param pchKey3
/// @param nValue3
/// @param pchKey4
/// @param nValue4
/// @param pchKey5
/// @param nValue5
/// @param pchKey6
/// @param nValue6
void PKvObject::_LogDebug(char8_t const *const pchLogKey,
                          char8_t const *const pchMsg,
                          char8_t const *const pchKey1,
                          int32_t const nValue1,
                          char8_t const *const pchKey2,
                          int32_t const nValue2,
                          char8_t const *const pchKey3,
                          int32_t const nValue3,
                          char8_t const *const pchKey4,
                          int32_t const nValue4,
                          char8_t const *const pchKey5,
                          int32_t const nValue5,
                          char8_t const *const pchKey6,
                          int32_t const nValue6) const noexcept
{
    _LogPrint(ara::log::LogLevel::kDebug, pchLogKey, pchMsg, pchKey1, nValue1, pchKey2, nValue2, pchKey3, nValue3,
              pchKey4, nValue4, pchKey5, nValue5, pchKey6, nValue6);
}
/// @brief Output log level: ara::log::LogLevel::kInfo
/// @param pchLogKey
/// @param pchMsg
/// @param pchKey1
/// @param nValue1
/// @param pchKey2
/// @param nValue2
/// @param pchKey3
/// @param nValue3
/// @param pchKey4
/// @param nValue4
/// @param pchKey5
/// @param nValue5
/// @param pchKey6
/// @param nValue6
void PKvObject::_LogInfo(char8_t const *const pchLogKey,
                         char8_t const *const pchMsg,
                         char8_t const *const pchKey1,
                         int32_t const nValue1,
                         char8_t const *const pchKey2,
                         int32_t const nValue2,
                         char8_t const *const pchKey3,
                         int32_t const nValue3,
                         char8_t const *const pchKey4,
                         int32_t const nValue4,
                         char8_t const *const pchKey5,
                         int32_t const nValue5,
                         char8_t const *const pchKey6,
                         int32_t const nValue6) const noexcept
{
    _LogPrint(ara::log::LogLevel::kInfo, pchLogKey, pchMsg, pchKey1, nValue1, pchKey2, nValue2, pchKey3, nValue3,
              pchKey4, nValue4, pchKey5, nValue5, pchKey6, nValue6);
}
/// @brief Output log level: ara::log::LogLevel::kInfo
/// @tparam T_Data
/// @param pchMsg
/// @param pchKey
/// @param nValue
template < typename T_Data >
inline void PKvObject::_LogInfo(char8_t const *const pchMsg,
                                char8_t const *const pchKey,
                                T_Data const nValue) const noexcept
{
    ara::log::LogStream logStream{GetWorkLogger()->WithLevel(ara::log::LogLevel::kInfo)};
    logStream << "KvName =" << GetLogKvName().data();
    if (nullptr != pchMsg) {
        logStream << pchMsg;
    }
    G_PrintLogKeyValue(logStream, pchKey, nValue, false);
    // logStream.Flush();
}
/// @brief Output log level: ara::log::LogLevel::kError
/// @param pchLogKey
/// @param pchMsg
/// @param pchKey1
/// @param nValue1
void PKvObject::_LogError(char8_t const *const pchLogKey,
                          char8_t const *const pchMsg,
                          char8_t const *const pchKey1,
                          int32_t const nValue1) const noexcept
{
    _LogPrint(ara::log::LogLevel::kError, pchLogKey, pchMsg, pchKey1, nValue1, nullptr, 0, nullptr, 0, nullptr, 0,
              nullptr, 0, nullptr, 0);
}
/// @brief Output log level: ara::log::LogLevel::kFatal
/// @param pchLogKey
/// @param pchMsg
/// @param pchKey1
/// @param nValue1
void PKvObject::_LogFatal(char8_t const *const pchLogKey,
                          char8_t const *const pchMsg,
                          char8_t const *const pchKey1,
                          int32_t const nValue1) const noexcept
{
    _LogPrint(ara::log::LogLevel::kFatal, pchLogKey, pchMsg, pchKey1, nValue1, nullptr, 0, nullptr, 0, nullptr, 0,
              nullptr, 0, nullptr, 0);
}
//***************/
/// @brief
/// @param nLogLevel
/// @param pchLogKey
/// @param pchMsg
/// @param pchKey1
/// @param nValue1
/// @param pchKey2
/// @param nValue2
/// @param pchKey3
/// @param nValue3
/// @param pchKey4
/// @param nValue4
/// @param pchKey5
/// @param nValue5
/// @param pchKey6
/// @param nValue6
void PKvObject::_LogPrint(ara::log::LogLevel const nLogLevel,
                          char8_t const *const pchLogKey,
                          char8_t const *const pchMsg,
                          char8_t const *const pchKey1,
                          int32_t const nValue1,
                          char8_t const *const pchKey2,
                          int32_t const nValue2,
                          char8_t const *const pchKey3,
                          int32_t const nValue3,
                          char8_t const *const pchKey4,
                          int32_t const nValue4,
                          char8_t const *const pchKey5,
                          int32_t const nValue5,
                          char8_t const *const pchKey6,
                          int32_t const nValue6) const noexcept
{
    ara::log::LogStream logStream{GetWorkLogger()->WithLevel(nLogLevel)};
    logStream << "KvName =" << GetLogKvName().data();
    if (nullptr != pchLogKey) {
        logStream << pchLogKey;
    }
    if (nullptr != pchMsg) {
        logStream << pchMsg;
    }
    G_PrintLogKeyValue(logStream, pchKey1, nValue1, false);
    G_PrintLogKeyValue(logStream, pchKey2, nValue2, true);
    G_PrintLogKeyValue(logStream, pchKey3, nValue3, true);
    G_PrintLogKeyValue(logStream, pchKey4, nValue4, true);
    G_PrintLogKeyValue(logStream, pchKey5, nValue5, true);
    G_PrintLogKeyValue(logStream, pchKey6, nValue6, true);

    // logStream.Flush();
}
//********************************/
/// @brief
/// @param pchMsg
/// @param pchKey
/// @param nValue
/// @return
template void PKvObject::_LogInfo< uint32_t >(char8_t const *const pchMsg,
                                              char8_t const *const pchKey,
                                              uint32_t const nValue) const noexcept;
//********************************/
}  // namespace isoftkv
}  // namespace per
}  // namespace ara
