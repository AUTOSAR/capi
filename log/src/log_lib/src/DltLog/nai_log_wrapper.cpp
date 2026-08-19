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
/// @file       nai_log_wrapper.cpp
/// @brief      Implementation of NaiLog log adaptation wrapper class
/// @details
/// @date       2026-03-10
/// @author     yangjinbiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/LOG/DltLog
/// @interface_level = unit
/// @unit_name = nai_log_wrapper
/// @unit_description=Implementation of NaiLog log adaptation wrapper class
/// @endcode
///
/// ================================================================

#include "DltLog/nai_log_wrapper.h"

#include <ara/core/string_view.h>
#include <nai/runtime/nai_errno.h>
#include <nai/runtime/nai_log.h>
#include <nai/runtime/nai_time.h>

#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include <cstring>

namespace ara {
namespace log {
namespace internal {

// Static member initialization
Logger *NaiLogWrapper::naiCallbackLogger_ = nullptr;

std::string NaiLogWrapper::NaiErrorToString(std::int32_t const &err) noexcept
{
    if (err < NAI_ECUSTOM_START) {
        return strerror(err);
    }

    switch (err) {
        case NAI_EFAILED: {
            return "Generally failed";
        }

        case NAI_ETYPE: {
            return "Invalid type";
        }
        case NAI_EOPERATION: {
            return "Invalid opeartion";
        }
        case NAI_ESETTING: {
            return "Invalid setting";
        }

        case NAI_ECLOSED: {
            return "Already closed";
        }

        case NAI_EINTERNAL: {
            return "Internal error";
        }

        case NAI_ECORRUPTION: {
            return "Data corruption";
        }

        case NAI_ESINGLETON: {
            return "Singleton";
        }

        case NAI_ENODRI: {
            return "No driver";
        }

        case NAI_EDEVICE_FUNCTION: {
            return "Invalid device function";
        }

        case NAI_EDRIVER_INTERNAL: {
            return "Driver internal error";
        }

        case NAI_EDRIVER_INITIALIZE: {
            return "Driver uninitialize";
        }

        case NAI_EARCHIVE: {
            return "Archive error";
        }
        case NAI_EVERSION: {
            return "Version error";
        }

#if !defined(ESHUTDOWN)
        case NAI_ESHUTDOWN:
            return "Can't send after socket shutdown";
            break;
#endif
        default: {
            return "";
        }
    }
}

bool NaiLogWrapper::SetNaiLogLevel(ara::log::LogLevel const &level) noexcept
{
    switch (level) {
        case LogLevel::kVerbose:
        case LogLevel::kDebug: {
            nai_log_core.level = static_cast< std::uint32_t >(NAI_LOG_DEBUG);
            break;
        }

        case LogLevel::kInfo: {
            nai_log_core.level = static_cast< std::uint32_t >(NAI_LOG_INFO);
            break;
        }

        case LogLevel::kWarn: {
            nai_log_core.level = static_cast< std::uint32_t >(NAI_LOG_WARN);
            break;
        }

        case LogLevel::kError: {
            nai_log_core.level = static_cast< std::uint32_t >(NAI_LOG_CRIT);
            break;
        }

        case LogLevel::kFatal: {
            nai_log_core.level = static_cast< std::uint32_t >(NAI_LOG_ALERT);
            break;
        }

        case LogLevel::kOff: {
            nai_log_core.level = 0U;
            break;
        }

        default: {
            break;
        }
    }
    return true;
}

bool NaiLogWrapper::InitNaiLog(bool const &bOutput, ara::log::LogLevel const &defaultLvl) noexcept
{
    if (!bOutput) {
        nai_log_core.cb
            = [](nai_log_t * /*l*/, const char *func, int line, int lvl, int err, const char *fmt, va_list va) -> int {
            (void)func;
            (void)line;
            (void)lvl;
            (void)err;
            (void)fmt;
            (void)va;
            return 0;
        };
    } else {
        std::ignore = SetNaiLogLevel(defaultLvl);
        nai_log_core.cb
            = [](nai_log_t * /*l*/, const char *func, int line, int lvl, int err, const char *fmt, va_list va) -> int {
            static ara::log::LogLevel const kMap_Levels[] = {
                LogLevel::kError,  // EMERG
                LogLevel::kError,  // ALERT
                LogLevel::kError,  // CRIT
                LogLevel::kWarn,   // ERROR
                LogLevel::kWarn,   // WARN
                LogLevel::kInfo,   // NOTICE
                LogLevel::kInfo,   // INFO
                LogLevel::kDebug   // DEBUG
            };
            ara::log::LogLevel const logLevel{kMap_Levels[lvl - 1]};
            std::size_t len{0};
            std::uint32_t const kMessageOutLen{1024U};
            // extend info
            char kMessage_Out[kMessageOutLen]{};

            if (err == 0) {
                len = nai_snprintf(const_cast< char * >(kMessage_Out), sizeof(kMessage_Out), "[%s:%d] ", func, line);
            } else if (NaiErrorToString(err).empty() == false) {
                const char *const errstr{"error str"};
                len = nai_snprintf(const_cast< char * >(kMessage_Out), sizeof(kMessage_Out), "[%s:%d] errno %d, %s, ",
                                   func, line, err, errstr);
            } else {
                len = nai_snprintf(const_cast< char * >(kMessage_Out), sizeof(kMessage_Out), "[%s:%d] errno %d, ", func,
                                   line, err);
            }
            char userMessage[kMessageOutLen]{};
            std::ignore = nai_vsnprintf(userMessage, sizeof(userMessage), fmt, va);

            if (len < sizeof(kMessage_Out)) {
                std::ignore = nai_snprintf(const_cast< char * >(kMessage_Out) + len, sizeof(kMessage_Out) - len, "%s",
                                           userMessage);
            }

            // output
            ara::core::String const str{kMessage_Out};
            if (naiCallbackLogger_ != nullptr) {
                naiCallbackLogger_->WithLevel(logLevel) << str;
            }
            return 0;
        };
    }
    return true;
}

bool NaiLogWrapper::DeinitNaiLog() noexcept
{
    naiCallbackLogger_ = nullptr;
    nai_log_core.cb    = [](nai_log_t * /*l*/, const char *func, std::int32_t line, std::int32_t lvl, std::int32_t err,
                         Char8_T const *fmt, va_list va) {
        std::ignore = func;
        std::ignore = line;
        std::ignore = lvl;
        std::ignore = err;
        std::ignore = fmt;
        std::ignore = va;
        return 0;
    };
    return true;
}

}  // namespace internal
}  // namespace log
}  // namespace ara
