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
/// @file       isoft_log_api.cpp
/// @brief      AutoSar-Crypto log module
/// @details
/// @date       2021-06-09
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/ReuseFunction/ReuseFunctionModule
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_06005
/// @unit_name=LogSystem
/// @endcode
///
/// ================================================================

#include "ara/crypto/common/isoft_log_api.h"

#include <iostream>

namespace {
/// @brief Log system pointer
/// @name  g_LoggerPtr
ara::log::Logger *g_LoggerPtr{nullptr};
}  // namespace
namespace ara {
namespace crypto {
namespace isoft_def {
//********************************/
/// @brief Return the working log system
/// @name  GetWorkLogger
/// @returns logger pointer instance
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_03264
/// @trace_id_dd=DD_CRYPTO_06507
/// @trace_id_sr=SR_CRYPTO_06005
/// @unit_name=LogSystem
/// @needwork = ad
/// @endcode
ara::log::Logger *GetWorkLogger() noexcept { return g_LoggerPtr; }
/// @brief Set the working log system
/// @name  SetWorkLogger
/// @param pLogger log object
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_03265
/// @trace_id_dd=DD_CRYPTO_06508
/// @trace_id_sr=SR_CRYPTO_06005
/// @unit_name=LogSystem
/// @needwork = ad
/// @endcode
void SetWorkLogger(ara::log::Logger *const pLogger) noexcept { g_LoggerPtr = pLogger; }
/// @brief Create default log system
/// @name  CreateDefLog
/// @returns logger pointer instance
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_03266
/// @trace_id_dd=DD_CRYPTO_06509
/// @trace_id_sr=SR_CRYPTO_06005
/// @unit_name=LogSystem
/// @needwork = ad
/// @endcode
ara::log::Logger *CreateDefLog() noexcept
{
    return InitLogSystem(GetkCryptoLogCtxID(), GetkCryptoLogCtxDesc(), ara::log::LogLevel::kInfo);
}

/// @brief Initialize log system
/// @param ctxId log output category identifier
/// @param ctxDescription log category description string
/// @param nLogLevel log output level
/// @return logger pointer instance
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_03267
/// @trace_id_dd=DD_CRYPTO_06510
/// @trace_id_sr=SR_CRYPTO_06005
/// @unit_name=LogSystem
/// @needwork = ad
/// @endcode
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
/// @returns  Logger reference
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_03268
/// @trace_id_dd=DD_CRYPTO_06511
/// @trace_id_sr=SR_CRYPTO_06005
/// @unit_name=LogSystem
/// @needwork = ad
/// @endcode
ara::log::Logger &GetLogger() noexcept
{
    if (nullptr == g_LoggerPtr) {
        g_LoggerPtr = CreateDefLog();
    }
    return *g_LoggerPtr;
}
}  // namespace
/// @brief Get a specific type of log stream object
/// @name  GetLogStreamByLevel
/// @param nLogLevel log output level
/// @returns  log stream object
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_03269
/// @trace_id_dd=DD_CRYPTO_06512
/// @trace_id_sr=SR_CRYPTO_06005
/// @unit_name=LogSystem
/// @needwork = ad
/// @endcode
ara::log::LogStream GetLogStreamByLevel(ara::log::LogLevel const nLogLevel) noexcept
{
    return GetLogger().WithLevel(nLogLevel);
}
/// @brief Get log stream object: fatal
/// @name  LogFatal
/// @returns log stream object
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_03270
/// @trace_id_dd=DD_CRYPTO_06513
/// @trace_id_sr=SR_CRYPTO_06005
/// @unit_name=LogSystem
/// @needwork = ad
/// @endcode
ara::log::LogStream LogFatal() noexcept { return GetLogStreamByLevel(ara::log::LogLevel::kFatal); }
/// @brief Get log stream object: error
/// @name  LogError
/// @returns  log stream object
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_03271
/// @trace_id_dd=DD_CRYPTO_06514
/// @trace_id_sr=SR_CRYPTO_06005
/// @unit_name=LogSystem
/// @needwork = ad
/// @endcode
ara::log::LogStream LogError() noexcept { return GetLogStreamByLevel(ara::log::LogLevel::kError); }
/// @brief Get log stream object: warning
/// @name  LogWarn
/// @returns  log stream object
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_03272
/// @trace_id_dd=DD_CRYPTO_06515
/// @trace_id_sr=SR_CRYPTO_06005
/// @unit_name=LogSystem
/// @needwork = ad
/// @endcode
ara::log::LogStream LogWarn() noexcept { return GetLogStreamByLevel(ara::log::LogLevel::kWarn); }
/// @brief Get log stream object: info
/// @name  LogInfo
/// @returns  log stream object
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_03273
/// @trace_id_dd=DD_CRYPTO_06516
/// @trace_id_sr=SR_CRYPTO_06005
/// @unit_name=LogSystem
/// @needwork = ad
/// @endcode
ara::log::LogStream LogInfo() noexcept { return GetLogStreamByLevel(ara::log::LogLevel::kInfo); }
/// @brief Get log stream object: debug
/// @name  LogDebug
/// @returns  log stream object
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_03274
/// @trace_id_dd=DD_CRYPTO_06517
/// @trace_id_sr=SR_CRYPTO_06005
/// @unit_name=LogSystem
/// @needwork = ad
/// @endcode
ara::log::LogStream LogDebug() noexcept { return GetLogStreamByLevel(ara::log::LogLevel::kDebug); }
/// @brief Get log stream object: verbose
/// @name  LogVerbose
/// @returns  log stream object
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_03275
/// @trace_id_dd=DD_CRYPTO_06518
/// @trace_id_sr=SR_CRYPTO_06005
/// @unit_name=LogSystem
/// @needwork = ad
/// @endcode
ara::log::LogStream LogVerbose() noexcept { return GetLogStreamByLevel(ara::log::LogLevel::kVerbose); }
///****************/    // Console output log wrapper class
/// @brief Parameterized constructor
/// @param pchEvent log message
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_03276
/// @trace_id_dd=DD_CRYPTO_06519
/// @needwork = ad
/// @endcode
PConsoleLog::PConsoleLog(ara::crypto::char8_t const *const pchEvent) noexcept
{
    if (pchEvent != nullptr) {
        std::ignore = printf("%-24s", pchEvent);
    }
}
/// @brief Parameterized constructor
/// @param pchEvent log message
/// @param nData integer log data
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_03277
/// @trace_id_dd=DD_CRYPTO_06520
/// @needwork = ad
/// @endcode
PConsoleLog::PConsoleLog(ara::crypto::char8_t const *const pchEvent, int32_t const nData) noexcept
{
    std::ignore = printf("%-24s%d", pchEvent, nData);
}
/// @brief Parameterized constructor
/// @param pchEvent log message
/// @param nData integer log data
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_03278
/// @trace_id_dd=DD_CRYPTO_06521
/// @needwork = ad
/// @endcode
PConsoleLog::PConsoleLog(ara::crypto::char8_t const *const pchEvent, uint32_t const nData) noexcept
{
    std::ignore = printf("%-24s%u", pchEvent, nData);
}
/// @brief Parameterized constructor
/// @param pchEvent log message
/// @param nData integer log data
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_03279
/// @trace_id_dd=DD_CRYPTO_06522
/// @needwork = ad
/// @endcode
PConsoleLog::PConsoleLog(ara::crypto::char8_t const *const pchEvent, uint64_t const nData) noexcept
{
    std::ignore = printf("%-24s%lu", pchEvent, nData);
}
/// @brief Parameterized constructor
/// @param pchEvent log message
/// @param pchMsg log message string
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_03280
/// @trace_id_dd=DD_CRYPTO_06523
/// @needwork = ad
/// @endcode
PConsoleLog::PConsoleLog(ara::crypto::char8_t const *const pchEvent, ara::crypto::char8_t const *const pchMsg) noexcept
{
    if (pchEvent != nullptr) {
        std::ignore = printf("%s", pchEvent);
    }
    if (pchMsg != nullptr) {
        std::ignore = printf("%s", pchMsg);
    }
}
/// @brief Destructor
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_03281
/// @trace_id_dd=DD_CRYPTO_06524
/// @needwork = ad
/// @endcode
PConsoleLog::~PConsoleLog() noexcept
{
    if (bAutoEndLine_) {
        std::ignore = printf("\n");
    }
}
/// @brief Output log: pchMsg + int32_t
/// @param pchMsg log message string
/// @param nData integer log data
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_03282
/// @trace_id_dd=DD_CRYPTO_06525
/// @needwork = ad
/// @endcode
void PConsoleLog::PrintLog(ara::crypto::char8_t const *const pchMsg, int32_t const nData) noexcept
{
    std::ignore   = printf("%s%d", pchMsg, nData);
    bAutoEndLine_ = true;
}
/// @brief Output log: pchMsg + uint32_t
/// @param pchMsg log message string
/// @param nData integer log data
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_03283
/// @trace_id_dd=DD_CRYPTO_06526
/// @needwork = ad
/// @endcode
void PConsoleLog::PrintLog(ara::crypto::char8_t const *const pchMsg, uint32_t const nData) noexcept
{
    std::ignore   = printf("%s%u", pchMsg, nData);
    bAutoEndLine_ = true;
}
/// @brief Output log: pchMsg + uint64_t
/// @param pchMsg log message string
/// @param nData integer log data
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_03284
/// @trace_id_dd=DD_CRYPTO_06527
/// @needwork = ad
/// @endcode
void PConsoleLog::PrintLog(ara::crypto::char8_t const *const pchMsg, uint64_t const nData) noexcept
{
    std::ignore   = printf("%s%lu", pchMsg, nData);
    bAutoEndLine_ = true;
}
/// @brief Output log: pchMsg + char8_t *
/// @param pchMsg log message string
/// @param pchData string message
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_03285
/// @trace_id_dd=DD_CRYPTO_06528
/// @needwork = ad
/// @endcode
void PConsoleLog::PrintLog(ara::crypto::char8_t const *const pchMsg, ara::crypto::char8_t const *const pchData) noexcept
{
    std::ignore   = printf("%s%s", pchMsg, pchData);
    bAutoEndLine_ = true;
}
/// @brief Output log: pchMsg +
/// @param pchMsg log message string
void PConsoleLog::PrintLog(ara::crypto::char8_t const *const pchMsg) noexcept
{
    if (pchMsg != nullptr) {
        std::ignore = printf("%s", pchMsg);
    }
    bAutoEndLine_ = true;
}
/// @brief Extended log output: pchFormat + template type
/// @param pchFormat log output format string
/// @param nData integer log data
/// @code{.isoft}
/// @tparam T
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_03286
/// @trace_id_dd=DD_CRYPTO_06529
/// @needwork = ad
/// @endcode
template < typename T >
void PConsoleLog::PrintLogEx(ara::crypto::char8_t const *const pchFormat,
                             T const nData) noexcept(noexcept(printf(pchFormat, nData)))
{
    std::ignore   = printf(pchFormat, nData);
    bAutoEndLine_ = true;
}
/// @brief Output log: newline
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_03287
/// @trace_id_dd=DD_CRYPTO_06530
/// @needwork = ad
/// @endcode
void PConsoleLog::PrintLog() noexcept
{
    std::ignore   = printf("\n");
    bAutoEndLine_ = false;
}
/// @brief Extended log output: pchFormat + int8_t
/// @param pchFormat log output format string
/// @param nData integer log data
/// @return
/// @throws
template void PConsoleLog::PrintLogEx< int8_t >(ara::crypto::char8_t const *pchFormat,
                                                int8_t nData) noexcept(noexcept(printf(pchFormat, nData)));
/// @brief Extended log output: pchFormat + uint8_t
/// @param pchFormat log output format string
/// @param nData integer log data
/// @return
/// @throws
template void PConsoleLog::PrintLogEx< uint8_t >(ara::crypto::char8_t const *pchFormat,
                                                 uint8_t nData) noexcept(noexcept(printf(pchFormat, nData)));
/// @brief Extended log output: pchFormat + int16_t
/// @param pchFormat log output format string
/// @param nData integer log data
/// @return
/// @throws
template void PConsoleLog::PrintLogEx< int16_t >(ara::crypto::char8_t const *pchFormat,
                                                 int16_t nData) noexcept(noexcept(printf(pchFormat, nData)));
/// @brief Extended log output: pchFormat + uint16_t
/// @param pchFormat log output format string
/// @param nData integer log data
/// @return
/// @throws
template void PConsoleLog::PrintLogEx< uint16_t >(ara::crypto::char8_t const *pchFormat,
                                                  uint16_t nData) noexcept(noexcept(printf(pchFormat, nData)));
/// @brief Extended log output: pchFormat + int32_t
/// @param pchFormat log output format string
/// @param nData integer log data
/// @return
/// @throws
template void PConsoleLog::PrintLogEx< int32_t >(ara::crypto::char8_t const *pchFormat,
                                                 int32_t nData) noexcept(noexcept(printf(pchFormat, nData)));
/// @brief Extended log output: pchFormat + uint32_t
/// @param pchFormat log output format string
/// @param nData integer log data
/// @return
/// @throws
template void PConsoleLog::PrintLogEx< uint32_t >(ara::crypto::char8_t const *pchFormat,
                                                  uint32_t nData) noexcept(noexcept(printf(pchFormat, nData)));
/// @brief Extended log output: pchFormat + int64_t
/// @param pchFormat log output format string
/// @param nData integer log data
/// @return
/// @throws
template void PConsoleLog::PrintLogEx< int64_t >(ara::crypto::char8_t const *pchFormat,
                                                 int64_t nData) noexcept(noexcept(printf(pchFormat, nData)));
/// @brief Extended log output: pchFormat + uint64_t
/// @param pchFormat log output format string
/// @param nData integer log data
/// @return
/// @throws
template void PConsoleLog::PrintLogEx< uint64_t >(ara::crypto::char8_t const *pchFormat,
                                                  uint64_t nData) noexcept(noexcept(printf(pchFormat, nData)));
/// @brief Extended log output: pchFormat + float
/// @param pchFormat log output format string
/// @param nData integer log data
/// @return
/// @throws
template void PConsoleLog::PrintLogEx< float >(ara::crypto::char8_t const *pchFormat,
                                               float nData) noexcept(noexcept(printf(pchFormat, nData)));
/// @brief Extended log output: pchFormat + double
/// @param pchFormat log output format string
/// @param nData integer log data
/// @return
/// @throws
template void PConsoleLog::PrintLogEx< double >(ara::crypto::char8_t const *pchFormat,
                                                double nData) noexcept(noexcept(printf(pchFormat, nData)));
/// @brief Extended log output: pchFormat + char8_t *
/// @param pchFormat log output format string
/// @param nData string log data
/// @throw ???
template void PConsoleLog::PrintLogEx< ara::crypto::char8_t const * >(
    ara::crypto::char8_t const *pchFormat,
    ara::crypto::char8_t const *nData) noexcept(noexcept(printf(pchFormat, nData)));
//********************************/
/// @brief Parameterized constructor
/// @param nLogLevel log output level
/// @param pchEvent log message
/// @param nData integer log data
/// @param bAutoEndLine
PAraLog::PAraLog(ara::log::LogLevel const nLogLevel,
                 ara::crypto::char8_t const *const pchEvent,
                 uint32_t const nData,
                 bool const bAutoEndLine) noexcept
    : logStream_{GetLogger().WithLevel(nLogLevel)}, bAutoEndLine_{bAutoEndLine}
{
    logStream_ << pchEvent << nData;
}
/// @brief Parameterized constructor
/// @param nLogLevel log output level
/// @param pchEvent log message
PAraLog::PAraLog(ara::log::LogLevel const nLogLevel, ara::crypto::char8_t const *const pchEvent) noexcept
    : logStream_{GetLogger().WithLevel(nLogLevel)}, bAutoEndLine_{true}
{
    if (pchEvent != nullptr) {
        logStream_ << pchEvent;
    }
}
/// @brief Parameterized constructor
/// @param nLogLevel log output level
/// @param pchEvent log message
/// @param nData integer log data
PAraLog::PAraLog(ara::log::LogLevel const nLogLevel,
                 ara::crypto::char8_t const *const pchEvent,
                 uint32_t const nData) noexcept
    : logStream_{GetLogger().WithLevel(nLogLevel)}, bAutoEndLine_{true}
{
    logStream_ << pchEvent << nData;
}

/// @brief Parameterized constructor
/// @param nLogLevel log output level
/// @param pchEvent log message
/// @param pchMsg log message string
PAraLog::PAraLog(ara::log::LogLevel const nLogLevel,
                 ara::crypto::char8_t const *const pchEvent,
                 ara::crypto::char8_t const *const pchMsg) noexcept
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
PAraLog::~PAraLog() noexcept
{
    if (bAutoEndLine_) {
        logStream_.Flush();
    }
}
/// @brief Output log: pchMsg
/// @name  PrintLog
/// @param pchMsg log message string
/// @return  log stream object
void PAraLog::PrintLog(ara::crypto::char8_t const *const pchMsg) noexcept
{
    if (pchMsg != nullptr) {
        logStream_ << pchMsg;
    }
    bAutoEndLine_ = true;
}
/// @brief Log output template class: pchMsg + T + pchMsg2
/// @name  PrintLog
/// @param pchMsg log message string
/// @param nData integer log data
/// @param pchMsg2 second log message string
template < typename T >
void PAraLog::PrintLog(ara::crypto::char8_t const *const pchMsg,
                       T const nData,
                       ara::crypto::char8_t const *const pchMsg2) noexcept
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
/// @brief Output log: force flush
/// @name  PrintLog
void PAraLog::PrintLog() noexcept
{
    logStream_.Flush();
    bAutoEndLine_ = false;
}

/// @brief Log output template class: pchMsg + int32_t + pchMsg2
/// @param pchMsg log message string
/// @param nData integer log data
/// @param pchMsg2 second log message string
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_03288
/// @trace_id_dd=DD_CRYPTO_06531
/// @needwork = ad
/// @endcode
template void PAraLog::PrintLog< int32_t >(ara::crypto::char8_t const *pchMsg,
                                           int32_t nData,
                                           ara::crypto::char8_t const *pchMsg2) noexcept;
/// @brief Log output template class: pchMsg + uint32_t + pchMsg2
/// @param pchMsg log message string
/// @param nData integer log data
/// @param pchMsg2 second log message string
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_03289
/// @trace_id_dd=DD_CRYPTO_06532
/// @needwork = ad
/// @endcode
template void PAraLog::PrintLog< uint32_t >(ara::crypto::char8_t const *pchMsg,
                                            uint32_t nData,
                                            ara::crypto::char8_t const *pchMsg2) noexcept;
/// @brief Log output template class: pchMsg + float + pchMsg2
/// @param pchMsg log message string
/// @param nData integer log data
/// @param pchMsg2 second log message string
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_03290
/// @trace_id_dd=DD_CRYPTO_06533
/// @needwork = ad
/// @endcode
template void PAraLog::PrintLog< float >(ara::crypto::char8_t const *pchMsg,
                                         float nData,
                                         ara::crypto::char8_t const *pchMsg2) noexcept;
/// @brief Log output template class: pchMsg + double + pchMsg2
/// @param pchMsg log message string
/// @param nData integer log data
/// @param pchMsg2 second log message string
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_03291
/// @trace_id_dd=DD_CRYPTO_06534
/// @needwork = ad
/// @endcode
template void PAraLog::PrintLog< double >(ara::crypto::char8_t const *pchMsg,
                                          double nData,
                                          ara::crypto::char8_t const *pchMsg2) noexcept;
/// @brief Log output template class: pchMsg + char8_t const * + pchMsg2
/// @param pchMsg log message string
/// @param nData integer log data
/// @param pchMsg2 second log message string
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_03292
/// @trace_id_dd=DD_CRYPTO_06535
/// @needwork = ad
/// @endcode
template void PAraLog::PrintLog< ara::crypto::char8_t const * >(ara::crypto::char8_t const *pchMsg,
                                                                ara::crypto::char8_t const *nData,
                                                                ara::crypto::char8_t const *pchMsg2) noexcept;
/// @brief Log output template class: pchMsg + char8_t * + pchMsg2
/// @param pchMsg log message string
/// @param nData integer log data
/// @param pchMsg2 second log message string
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_03293
/// @trace_id_dd=DD_CRYPTO_06536
/// @needwork = ad
/// @endcode
template void PAraLog::PrintLog< ara::crypto::char8_t * >(ara::crypto::char8_t const *pchMsg,
                                                          ara::crypto::char8_t *nData,
                                                          ara::crypto::char8_t const *pchMsg2) noexcept;

}  // namespace  isoft_def
}  // namespace crypto
}  // namespace ara
