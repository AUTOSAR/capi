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
/// @file       log_api.cpp
/// @brief      fw log processing
/// @details    fw log processing
/// @date       2025-01-16
/// @author     jzy
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/fw/common processing
/// @interface_level=module
/// @trace_id_sr=SR_FW_0001
/// @unit_name=Logapi
/// @unit_description=log processing class
/// @endcode
///
/// ================================================================
///
///
/// ================================================================

#include "ara/fw/internal/log_api.h"

#include <iostream>

namespace {
/// @brief Log system pointer
/// @name  gG_Logger
ara::log::Logger *gG_Logger{nullptr};
}  // namespace
namespace ara {
namespace fw {
namespace internal {
//********************************/
/// @brief Return the working log system
/// @name  GetWorkLogger
/// @returns
ara::log::Logger *GetWorkLogger() noexcept { return gG_Logger; }
/// @brief Set the working log system
/// @name  SetWorkLogger
/// @param pLogger Log object
/// @returns  void
void SetWorkLogger(ara::log::Logger *const pLogger) noexcept { gG_Logger = pLogger; }
/// @brief Create default log system
/// @name  CreateDefLog
/// @returns  log::Logger *
ara::log::Logger *CreateDefLog() noexcept
{
    return InitLogSystem(GetkCryptoLogCtxID(), GetkCryptoLogCtxDesc(), ara::log::LogLevel::kInfo);
}

/// @brief Initialize log system
/// @param ctxId Log output category identifier
/// @param ctxDescription Log category description string
/// @param nLogLevel Log output level
/// @return
ara::log::Logger *InitLogSystem(ara::core::StringView const &ctxId,
                                ara::core::StringView const &ctxDescription,
                                ara::log::LogLevel const nLogLevel) noexcept
{
    ara::log::Logger &logger{ara::log::CreateLogger(ctxId, ctxDescription, nLogLevel)};
    return &(logger);
}
//********************************/
namespace {
/// @brief Get log object
/// @name  GetLogger
/// @returns  log::Logger &
/// @throws
ara::log::Logger &GetLogger() noexcept
{
    if (nullptr == gG_Logger) {
        gG_Logger = CreateDefLog();
    }
    return *gG_Logger;
}
}  // namespace
/// @brief Get specific type log stream object
/// @name  GetLogStream
/// @param nLogLevel Log output level
/// @returns  ara::log::LogStream
/// @throws
ara::log::LogStream GetLogStream(ara::log::LogLevel const nLogLevel) noexcept
{
    return GetLogger().WithLevel(nLogLevel);
}
/// @brief Get log stream object: Fatal
/// @name  LogFatal
/// @returns  log::LogStream
/// @throws
ara::log::LogStream LogFatal() noexcept { return GetLogStream(ara::log::LogLevel::kFatal); }
/// @brief Get log stream object: Error
/// @name  LogError
/// @returns  log::LogStream
/// @throws
ara::log::LogStream LogError() noexcept { return GetLogStream(ara::log::LogLevel::kError); }
/// @brief Get log stream object: Warning
/// @name  LogWarn
/// @returns  log::LogStream
/// @throws
ara::log::LogStream LogWarn() noexcept { return GetLogStream(ara::log::LogLevel::kWarn); }
/// @brief Get log stream object: Info
/// @name  LogInfo
/// @returns  log::LogStream
/// @throws
ara::log::LogStream LogInfo() noexcept { return GetLogStream(ara::log::LogLevel::kInfo); }
/// @brief Get log stream object: Debug
/// @name  LogDebug
/// @returns  log::LogStream
/// @throws
ara::log::LogStream LogDebug() noexcept { return GetLogStream(ara::log::LogLevel::kDebug); }
/// @brief Get log stream object: Verbose
/// @name  LogVerbose
/// @returns  log::LogStream
/// @throws
ara::log::LogStream LogVerbose() noexcept { return GetLogStream(ara::log::LogLevel::kVerbose); }
///****************/ //Console output log wrapper class
/// @brief Parameterized constructor
/// @param pchEvent Log message
/// @throws
PConsoleLog::PConsoleLog(char8_t const *const pchEvent)
{
    if (pchEvent != nullptr) {
        std::ignore = printf("%-24s", pchEvent);
    }
}
/// @brief Parameterized constructor
/// @param pchEvent Log message
/// @param nData Integer log data
/// @throws
PConsoleLog::PConsoleLog(char8_t const *const pchEvent, int32_t const nData)
{
    std::ignore = printf("%-24s%d", pchEvent, nData);
}
/// @brief Parameterized constructor
/// @param pchEvent Log message
/// @param nData Integer log data
/// @throws
PConsoleLog::PConsoleLog(char8_t const *const pchEvent, uint32_t const nData)
{
    std::ignore = printf("%-24s%u", pchEvent, nData);
}
/// @brief Parameterized constructor
/// @param pchEvent Log message
/// @param nData Integer log data
/// @throws
PConsoleLog::PConsoleLog(char8_t const *const pchEvent, uint64_t const nData)
{
    std::ignore = printf("%-24s%lu", pchEvent, nData);
}
/// @brief Parameterized constructor
/// @param pchEvent Log message
/// @param pchMsg Log message string
/// @throws
PConsoleLog::PConsoleLog(char8_t const *const pchEvent, char8_t const *const pchMsg)
{
    if (pchEvent != nullptr) {
        std::ignore = printf("%s", pchEvent);
    }
    if (pchMsg != nullptr) {
        std::ignore = printf("%s", pchMsg);
    }
}
/// @brief Destructor
/// @throws
PConsoleLog::~PConsoleLog() noexcept
{
    if (bAutoEndLine_) {
        std::ignore = printf("\n");
    }
}
/// @brief Output log: pchMsg + int32_t
/// @param pchMsg Log message string
/// @param nData Integer log data
/// @throws
void PConsoleLog::PrintLog(char8_t const *const pchMsg, int32_t const nData)
{
    std::ignore   = printf("%s%d", pchMsg, nData);
    bAutoEndLine_ = true;
}
/// @brief Output log: pchMsg + uint32_t
/// @param pchMsg Log message string
/// @param nData Integer log data
/// @throws
void PConsoleLog::PrintLog(char8_t const *const pchMsg, uint32_t const nData)
{
    std::ignore   = printf("%s%u", pchMsg, nData);
    bAutoEndLine_ = true;
}
/// @brief Output log: pchMsg + uint64_t
/// @param pchMsg Log message string
/// @param nData Integer log data
/// @throws
void PConsoleLog::PrintLog(char8_t const *const pchMsg, uint64_t const nData)
{
    std::ignore   = printf("%s%lu", pchMsg, nData);
    bAutoEndLine_ = true;
}
/// @brief Output log: pchMsg + char8_t *
/// @param pchMsg Log message string
/// @param pchData String message
/// @throws
void PConsoleLog::PrintLog(char8_t const *const pchMsg, char8_t const *const pchData)
{
    std::ignore   = printf("%s%s", pchMsg, pchData);
    bAutoEndLine_ = true;
}
/// @brief Output log: pchMsg +
/// @param pchMsg Log message string
/// @throws
void PConsoleLog::PrintLog(char8_t const *const pchMsg)
{
    if (pchMsg != nullptr) {
        std::ignore = printf("%s", pchMsg);
    }
    bAutoEndLine_ = true;
}
/// @brief Output log extension: pchFormat + template type
/// @tparam T
/// @param pchFormat Log output format string
/// @param nData Integer log data
/// @throws
template < typename T >
void PConsoleLog::PrintLogEx(char8_t const *const pchFormat, T const nData)
{
    std::ignore   = printf(pchFormat, nData);
    bAutoEndLine_ = true;
}
/// @brief Output log: newline
/// @throws
void PConsoleLog::PrintLog()
{
    std::ignore   = printf("\n");
    bAutoEndLine_ = false;
}
/// @brief Output log extension: pchFormat + int8_t
/// @param pchFormat Log output format string
/// @param nData Integer log data
/// @return
/// @throws
template void PConsoleLog::PrintLogEx< int8_t >(char8_t const *pchFormat, int8_t nData);
/// @brief Output log extension: pchFormat + uint8_t
/// @param pchFormat Log output format string
/// @param nData Integer log data
/// @return
/// @throws
template void PConsoleLog::PrintLogEx< uint8_t >(char8_t const *pchFormat, uint8_t nData);
/// @brief Output log extension: pchFormat + int16_t
/// @param pchFormat Log output format string
/// @param nData Integer log data
/// @return
/// @throws
template void PConsoleLog::PrintLogEx< int16_t >(char8_t const *pchFormat, int16_t nData);
/// @brief Output log extension: pchFormat + uint16_t
/// @param pchFormat Log output format string
/// @param nData Integer log data
/// @return
/// @throws
template void PConsoleLog::PrintLogEx< uint16_t >(char8_t const *pchFormat, uint16_t nData);
/// @brief Output log extension: pchFormat + int32_t
/// @param pchFormat Log output format string
/// @param nData Integer log data
/// @return
/// @throws
template void PConsoleLog::PrintLogEx< int32_t >(char8_t const *pchFormat, int32_t nData);
/// @brief Output log extension: pchFormat + uint32_t
/// @param pchFormat Log output format string
/// @param nData Integer log data
/// @return
/// @throws
template void PConsoleLog::PrintLogEx< uint32_t >(char8_t const *pchFormat, uint32_t nData);
/// @brief Output log extension: pchFormat + int64_t
/// @param pchFormat Log output format string
/// @param nData Integer log data
/// @return
/// @throws
template void PConsoleLog::PrintLogEx< int64_t >(char8_t const *pchFormat, int64_t nData);
/// @brief Output log extension: pchFormat + uint64_t
/// @param pchFormat Log output format string
/// @param nData Integer log data
/// @return
/// @throws
template void PConsoleLog::PrintLogEx< uint64_t >(char8_t const *pchFormat, uint64_t nData);
/// @brief Output log extension: pchFormat + float
/// @param pchFormat Log output format string
/// @param nData Integer log data
/// @return
/// @throws
template void PConsoleLog::PrintLogEx< float >(char8_t const *pchFormat, float nData);
/// @brief Output log extension: pchFormat + double
/// @param pchFormat Log output format string
/// @param nData Integer log data
/// @return
/// @throws
template void PConsoleLog::PrintLogEx< double >(char8_t const *pchFormat, double nData);
/// @brief Output log extension: pchFormat + char8_t *
/// @param pchFormat Log output format string
/// @param nData Integer log data
/// @return
/// @throws
template void PConsoleLog::PrintLogEx< char8_t const * >(char8_t const *pchFormat, char8_t const *nData);
//********************************/
/// @brief Parameterized constructor
/// @param nLogLevel Log output level
/// @param pchEvent Log message
/// @param nData Integer log data
/// @param bAutoEndLine
PAraLog::PAraLog(ara::log::LogLevel const nLogLevel,
                 char8_t const *const pchEvent,
                 uint32_t const nData,
                 bool const bAutoEndLine) noexcept
    : logStream_{GetLogger().WithLevel(nLogLevel)}, bAutoEndLine_{bAutoEndLine}
{
    logStream_ << pchEvent << nData;
}
/// @brief Parameterized constructor
/// @name  PAraLog
/// @param nLogLevel Log output level
/// @param pchEvent Log message
/// @throws
PAraLog::PAraLog(ara::log::LogLevel const nLogLevel, char8_t const *const pchEvent) noexcept
    : logStream_{GetLogger().WithLevel(nLogLevel)}, bAutoEndLine_{true}
{
    if (pchEvent != nullptr) {
        logStream_ << pchEvent;
    }
}
/// @brief Parameterized constructor
/// @name  PAraLog
/// @param nLogLevel Log output level
/// @param pchEvent Log message
/// @param nData Integer log data
/// @throws
PAraLog::PAraLog(ara::log::LogLevel const nLogLevel, char8_t const *const pchEvent, uint32_t const nData) noexcept
    : logStream_{GetLogger().WithLevel(nLogLevel)}, bAutoEndLine_{true}
{
    logStream_ << pchEvent << nData;
}

/// @brief Parameterized constructor
/// @name  PAraLog
/// @param nLogLevel Log output level
/// @param pchEvent Log message
/// @param pchMsg Log message string
/// @throws
PAraLog::PAraLog(ara::log::LogLevel const nLogLevel,
                 char8_t const *const pchEvent,
                 char8_t const *const pchMsg) noexcept
    : logStream_{GetLogger().WithLevel(nLogLevel)}, bAutoEndLine_{true}
{
    if (pchEvent != nullptr) {
        logStream_ << pchEvent;
    }
    if (pchMsg != nullptr) {
        logStream_ << pchMsg;
    }
}
/// @brief Destructor
/// @name  ~PAraLog
/// @throws
PAraLog::~PAraLog() noexcept
{
    if (bAutoEndLine_) {
        logStream_.Flush();
    }
}
/// @brief Get log stream object
/// @name  GetLogStream
/// @throws
/// @return
inline PAraLog::LogStream &PAraLog::GetLogStream() noexcept { return logStream_; }
/// @brief Output log: pchMsg
/// @name  PrintLog
/// @param pchMsg Log message string
/// @return  void
/// @throws
void PAraLog::PrintLog(char8_t const *const pchMsg) noexcept
{
    if (pchMsg != nullptr) {
        logStream_ << pchMsg;
    }
    bAutoEndLine_ = true;
}
/// @brief Output log template class: pchMsg + T + pchMsg2
/// @name  PrintLog
/// @param pchMsg Log message string
/// @param nData Integer log data
/// @param pchMsg2 Log message string 2
/// @throws
template < typename T >
void PAraLog::PrintLog(char8_t const *const pchMsg, T const nData, char8_t const *const pchMsg2) noexcept
{
    if (pchMsg != nullptr) {
        logStream_ << pchMsg;
    }
    logStream_ << nData;
    if (pchMsg2 != nullptr) {
        logStream_ << pchMsg2;
    }
    bAutoEndLine_ = true;
}
/// @brief Output log: force flush to disk
/// @name  PrintLog
/// @returns  void
/// @throws
void PAraLog::PrintLog() noexcept
{
    logStream_.Flush();
    bAutoEndLine_ = false;
}

/// @brief Output log template class: pchMsg + int32_t + pchMsg2
/// @param pchMsg Log message string
/// @param nData Integer log data
/// @param pchMsg2 Log message string 2
/// @return
/// @throws
template void PAraLog::PrintLog< int32_t >(char8_t const *pchMsg, int32_t nData, char8_t const *pchMsg2);
/// @brief Output log template class: pchMsg + uint32_t + pchMsg2
/// @param pchMsg Log message string
/// @param nData Integer log data
/// @param pchMsg2 Log message string 2
/// @return
/// @throws
template void PAraLog::PrintLog< uint32_t >(char8_t const *pchMsg, uint32_t nData, char8_t const *pchMsg2);
/// @brief Output log template class: pchMsg + float + pchMsg2
/// @param pchMsg Log message string
/// @param nData Integer log data
/// @param pchMsg2 Log message string 2
/// @return
/// @throws
template void PAraLog::PrintLog< float >(char8_t const *pchMsg, float nData, char8_t const *pchMsg2);
/// @brief Output log template class: pchMsg + double + pchMsg2
/// @param pchMsg Log message string
/// @param nData Integer log data
/// @param pchMsg2 Log message string 2
/// @return
/// @throws
template void PAraLog::PrintLog< double >(char8_t const *pchMsg, double nData, char8_t const *pchMsg2);
/// @brief Output log template class: pchMsg + char8_t const * + pchMsg2
/// @param pchMsg Log message string
/// @param nData Integer log data
/// @param pchMsg2 Log message string 2
/// @return
/// @throws
template void PAraLog::PrintLog< char8_t const * >(char8_t const *pchMsg, char8_t const *nData, char8_t const *pchMsg2);
/// @brief Output log template class: pchMsg + char8_t * + pchMsg2
/// @param pchMsg Log message string
/// @param nData Integer log data
/// @param pchMsg2 Log message string 2
/// @return
/// @throws
template void PAraLog::PrintLog< char8_t * >(char8_t const *pchMsg, char8_t *nData, char8_t const *pchMsg2);

}  // namespace internal
}  // namespace fw
}  // namespace ara
