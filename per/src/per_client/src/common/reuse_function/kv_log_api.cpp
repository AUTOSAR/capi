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
/// @file       kv_log_api.cpp
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

#include "ara/per/internal/isoftkv/kv_log_api.h"

#include <iostream>

namespace {
/// @brief
ara::log::Logger *g_LoggerPtr{nullptr};
/// @brief
/// @return
ara::log::Logger &GetLogger() noexcept
{
    if (nullptr == g_LoggerPtr) {
        g_LoggerPtr = ara::per::isoftkv::CreateDefLog();
    }
    return *g_LoggerPtr;
}
}  // namespace

namespace ara {
namespace per {
namespace isoftkv {
//********************************/
/// @brief Return default logger
/// @return
ara::log::Logger *GetWorkLogger() noexcept { return g_LoggerPtr; }
/// @brief Return default logger
/// @param pLogger
void SetWorkLogger(ara::log::Logger *const pLogger) noexcept { g_LoggerPtr = pLogger; }
/// @brief Initialize default Log
/// @return
ara::log::Logger *CreateDefLog() noexcept
{
    return InitLogSystem(GetPerLogCtxID(), GetPerLogCtxDesc(), ara::log::LogLevel::kInfo);
}
/// @brief Initialize Log
/// @param stCtxId
/// @param stCtxDescription
/// @param nLogLevel
/// @return
ara::log::Logger *InitLogSystem(ara::core::StringView const &stCtxId,
                                ara::core::StringView const &stCtxDescription,
                                ara::log::LogLevel const nLogLevel) noexcept
{
    ara::log::Logger &logger{ara::log::CreateLogger(stCtxId, stCtxDescription, nLogLevel)};
    return &(logger);
}
//********************************/
/// @brief
/// @param nLogLevel
/// @return
inline ara::log::LogStream GetLogStream(ara::log::LogLevel const nLogLevel) noexcept
{
    return GetLogger().WithLevel(nLogLevel);
}
/// @brief Output log of ara::log::LogLevel::kFatal level.
/// @return
ara::log::LogStream LogFatal() noexcept { return GetLogStream(ara::log::LogLevel::kFatal); }
/// @brief Output log of ara::log::LogLevel::kError level.
/// @return
ara::log::LogStream LogError() noexcept { return GetLogStream(ara::log::LogLevel::kError); }
/// @brief Output log of ara::log::LogLevel::kWarn level.
/// @return
ara::log::LogStream LogWarn() noexcept { return GetLogStream(ara::log::LogLevel::kWarn); }
/// @brief Output log of ara::log::LogLevel::kInfo level.
/// @return
ara::log::LogStream LogInfo() noexcept { return GetLogStream(ara::log::LogLevel::kInfo); }
/// @brief Output log of ara::log::LogLevel::kDebug level.
/// @return
ara::log::LogStream LogDebug() noexcept { return GetLogStream(ara::log::LogLevel::kDebug); }
/// @brief Output log of ara::log::LogLevel::kVerbose level.
/// @return
ara::log::LogStream LogVerbose() noexcept { return GetLogStream(ara::log::LogLevel::kVerbose); }
//********************************/
/// @brief
/// @param pchEvent
PConsoleLog::PConsoleLog(char8_t const *const pchEvent) noexcept
{
    if (nullptr != pchEvent) {
        std::ignore = printf("%-24s", pchEvent);
    }
}
/// @brief
/// @param pchEvent
/// @param nData
PConsoleLog::PConsoleLog(char8_t const *const pchEvent, int32_t const nData) noexcept
{
    std::ignore = printf("%-24s%d", pchEvent, nData);
}
/// @brief
/// @param pchEvent
/// @param nData
PConsoleLog::PConsoleLog(char8_t const *const pchEvent, uint32_t const nData) noexcept
{
    std::ignore = printf("%-24s%u", pchEvent, nData);
}
/// @brief
/// @param pchEvent
/// @param nData
PConsoleLog::PConsoleLog(char8_t const *const pchEvent, uint64_t const nData) noexcept
{
    std::ignore = printf("%-24s%lu", pchEvent, nData);
}
/// @brief
/// @param pchEvent
/// @param pchMsg
PConsoleLog::PConsoleLog(char8_t const *const pchEvent, char8_t const *const pchMsg) noexcept
{
    if (nullptr != pchEvent) {
        std::ignore = printf("%s", pchEvent);
    }
    if (nullptr != pchMsg) {
        std::ignore = printf("%s", pchMsg);
    }
}
/// @brief
PConsoleLog::~PConsoleLog() noexcept
{
    if (bAutoEndLine_) {
        std::ignore = printf("\n");
    }
}
/// @brief Output integer: int32_t
/// @param pchMsg
/// @param nData
void PConsoleLog::PrintLog(char8_t const *const pchMsg, int32_t const nData) noexcept
{
    std::ignore   = printf("%s%d", pchMsg, nData);
    bAutoEndLine_ = true;
}
/// @brief Output integer: uint8_t
/// @param pchMsg
/// @param nData
void PConsoleLog::PrintLog(char8_t const *const pchMsg, uint8_t const nData) noexcept
{
    return PrintLog(pchMsg, static_cast< uint32_t >(nData));
}
/// @brief Output integer: uint16_t
/// @param pchMsg
/// @param nData
void PConsoleLog::PrintLog(char8_t const *const pchMsg, uint16_t const nData) noexcept
{
    return PrintLog(pchMsg, static_cast< uint32_t >(nData));
}
/// @brief Output integer: uint32_t
/// @param pchMsg
/// @param nData
void PConsoleLog::PrintLog(char8_t const *const pchMsg, uint32_t const nData) noexcept
{
    std::ignore   = printf("%s%u", pchMsg, nData);
    bAutoEndLine_ = true;
}
/// @brief Output integer: uint64_t
/// @param pchMsg
/// @param nData
void PConsoleLog::PrintLog(char8_t const *const pchMsg, uint64_t const nData) noexcept
{
    std::ignore   = printf("%s%lu", pchMsg, nData);
    bAutoEndLine_ = true;
}
/// @brief Output string: Memory
/// @param pchMsg
/// @param pchData
void PConsoleLog::PrintLog(char8_t const *const pchMsg, char8_t const *const pchData) noexcept
{
    std::ignore   = printf("%s%s", pchMsg, pchData);
    bAutoEndLine_ = true;
}
/// @brief Output pointer address
/// @param pchMsg
/// @param nData
void PConsoleLog::PrintLog(char8_t const *const pchMsg, void const *pVoidData) noexcept
{
    std::ignore   = printf("%s0x%p", pchMsg, pVoidData);
    bAutoEndLine_ = true;
}
/// @brief Output string
/// @param pchMsg
void PConsoleLog::PrintLog(char8_t const *const pchMsg) noexcept
{
    if (nullptr != pchMsg) {
        std::ignore = printf("%s", pchMsg);
    }
    bAutoEndLine_ = true;
}
/// @brief Output newline character
void PConsoleLog::PrintLog() noexcept
{
    std::ignore   = printf("\n");
    bAutoEndLine_ = false;
}
/// @brief
/// @tparam T
/// @param pchFormat
/// @param nData
/// @throws
template < typename T >
inline void PConsoleLog::PrintLogEx(char8_t const *const pchFormat, T const nData)
{
    std::ignore   = printf(pchFormat, nData);
    bAutoEndLine_ = true;
}
/// @brief
/// @param pchFormat
/// @param nData
/// @return
/// @throws
template void PConsoleLog::PrintLogEx< int8_t >(char8_t const *const pchFormat, int8_t const nData);
/// @brief
/// @param pchFormat
/// @param nData
/// @return
/// @throws
template void PConsoleLog::PrintLogEx< uint8_t >(char8_t const *const pchFormat, uint8_t const nData);
/// @brief
/// @param pchFormat
/// @param nData
/// @return
/// @throws
template void PConsoleLog::PrintLogEx< int16_t >(char8_t const *const pchFormat, int16_t const nData);
/// @brief
/// @param pchFormat
/// @param nData
/// @return
/// @throws
template void PConsoleLog::PrintLogEx< uint16_t >(char8_t const *const pchFormat, uint16_t const nData);
/// @brief
/// @param pchFormat
/// @param nData
/// @return
/// @throws
template void PConsoleLog::PrintLogEx< int32_t >(char8_t const *const pchFormat, int32_t const nData);
/// @brief
/// @param pchFormat
/// @param nData
/// @return
/// @throws
template void PConsoleLog::PrintLogEx< uint32_t >(char8_t const *const pchFormat, uint32_t const nData);
/// @brief
/// @param pchFormat
/// @param nData
/// @return
/// @throws
template void PConsoleLog::PrintLogEx< int64_t >(char8_t const *const pchFormat, int64_t const nData);
/// @brief
/// @param pchFormat
/// @param nData
/// @return
/// @throws
template void PConsoleLog::PrintLogEx< uint64_t >(char8_t const *const pchFormat, uint64_t const nData);
/// @brief
/// @param pchFormat
/// @param nData
/// @return
/// @throws
template void PConsoleLog::PrintLogEx< float >(char8_t const *const pchFormat, float nData);
/// @brief
/// @param pchFormat
/// @param nData
/// @return
/// @throws
template void PConsoleLog::PrintLogEx< double >(char8_t const *const pchFormat, double nData);
/// @brief
/// @param pchFormat
/// @param nData
/// @return
/// @throws
template void PConsoleLog::PrintLogEx< char8_t const * >(char8_t const *const pchFormat, char8_t const *nData);
/// @brief
/// @param pchFormat
/// @param nData
/// @return
/// @throws
template void PConsoleLog::PrintLogEx< void const * >(char8_t const *const pchFormat, void const *nData);
//********************************/

/// @brief
/// @param nLogLevel
/// @param pchEvent
PAraLog::PAraLog(ara::log::LogLevel const nLogLevel, char8_t const *const pchEvent) noexcept
    : logStream_{GetLogger().WithLevel(nLogLevel)}, bAutoEndLine_{true}
{
    if (nullptr != pchEvent) {
        logStream_ << pchEvent;
    }
}
/// @brief
/// @param nLogLevel
/// @param pchEvent
/// @param nData
PAraLog::PAraLog(ara::log::LogLevel const nLogLevel, char8_t const *const pchEvent, int32_t const nData) noexcept
    : logStream_{GetLogger().WithLevel(nLogLevel)}, bAutoEndLine_{true}
{
    logStream_ << pchEvent << nData;
}
/// @brief
/// @param nLogLevel
/// @param pchEvent
/// @param nData
PAraLog::PAraLog(ara::log::LogLevel const nLogLevel, char8_t const *const pchEvent, uint32_t const nData) noexcept
    : logStream_{GetLogger().WithLevel(nLogLevel)}, bAutoEndLine_{true}
{
    logStream_ << pchEvent << nData;
}
/// @brief
/// @param nLogLevel
/// @param pchEvent
/// @param pchMsg
PAraLog::PAraLog(ara::log::LogLevel const nLogLevel,
                 char8_t const *const pchEvent,
                 char8_t const *const pchMsg) noexcept
    : logStream_{GetLogger().WithLevel(nLogLevel)}, bAutoEndLine_{true}
{
    if (nullptr != pchEvent) {
        logStream_ << pchEvent;
    }
    if (nullptr != pchMsg) {
        logStream_ << pchMsg;
    }
}
/// @brief
PAraLog::~PAraLog() noexcept
{
    if (bAutoEndLine_) {
        logStream_.Flush();
    }
}
/// @brief
/// @param pchMsg
void PAraLog::PrintLog(char8_t const *const pchMsg) noexcept
{
    if (nullptr != pchMsg) {
        logStream_ << pchMsg;
    }
    bAutoEndLine_ = true;
}
/// @brief
/// @tparam T
/// @param pchMsg
/// @param nData
/// @param pchMsg2
/// @throws
template < typename T >
inline void PAraLog::PrintLog(char8_t const *const pchMsg, T const nData, char8_t const *const pchMsg2) noexcept
{
    if (nullptr != pchMsg) {
        logStream_ << pchMsg;
    }
    logStream_ << nData;
    if (nullptr != pchMsg2) {
        logStream_ << pchMsg2;
    }
    bAutoEndLine_ = true;
}
/// @brief
void PAraLog::PrintLog() noexcept
{
    logStream_.Flush();
    bAutoEndLine_ = false;
}
//********************************/
/// @brief
/// @param pchMsg
/// @param nData
/// @param pchMsg2
/// @return
template void PAraLog::PrintLog< int32_t >(char8_t const *const pchMsg,
                                           int32_t const nData,
                                           char8_t const *const pchMsg2) noexcept;
/// @brief
/// @param pchMsg
/// @param nData
/// @param pchMsg2
/// @return
template void PAraLog::PrintLog< uint32_t >(char8_t const *const pchMsg,
                                            uint32_t const nData,
                                            char8_t const *const pchMsg2) noexcept;
/// @brief
/// @param pchMsg
/// @param nData
/// @param pchMsg2
/// @return
template void PAraLog::PrintLog< float >(char8_t const *const pchMsg,
                                         float nData,
                                         char8_t const *const pchMsg2) noexcept;
/// @brief
/// @param pchMsg
/// @param nData
/// @param pchMsg2
/// @return
template void PAraLog::PrintLog< double >(char8_t const *const pchMsg,
                                          double nData,
                                          char8_t const *const pchMsg2) noexcept;
/// @brief
/// @param pchMsg
/// @param nData
/// @param pchMsg2
/// @return
template void PAraLog::PrintLog< char8_t const * >(char8_t const *const pchMsg,
                                                   char8_t const *nData,
                                                   char8_t const *const pchMsg2) noexcept;
/// @brief
/// @param pchMsg
/// @param nData
/// @param pchMsg2
/// @return
template void PAraLog::PrintLog< char8_t * >(char8_t const *const pchMsg,
                                             char8_t *nData,
                                             char8_t const *const pchMsg2) noexcept;
//********************************/
}  // namespace isoftkv
}  // namespace per
}  // namespace ara
