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
/// @file       public_log.cpp
/// @brief      This file provides DOIP common log related interface definitions
/// @details
/// @date       2022-08-29
/// @author     gaohuiming
/// @version    1.2.0
///
/// ================================================================

#include "public_log.h"

#include <ara/log/logger.h>

namespace isoft {
namespace doip {

/// @brief Get default logger
/// @return Logger object
static ara::log::Logger& GetDefaultLogger() noexcept
{
    static ara::log::Logger& s_Logger{
        ara::log::CreateLogger(std::move(ara::core::StringView("#DOIP")), std::move(ara::core::StringView("doiplib")))};
    return s_Logger;
}

/// @brief Get Error level LogStream object
/// @return LogStream object
ara::log::LogStream LogError() noexcept { return GetDefaultLogger().LogError(); }

/// @brief Get Warn level LogStream object
/// @return LogStream object
ara::log::LogStream LogWarn() noexcept { return GetDefaultLogger().LogWarn(); }

/// @brief Get Info level LogStream object
/// @return LogStream object
ara::log::LogStream LogInfo() noexcept { return GetDefaultLogger().LogInfo(); }

/// @brief Get debug level LogStream object
/// @return LogStream object
ara::log::LogStream LogDebug() noexcept { return GetDefaultLogger().LogDebug(); }
}  // namespace doip
}  // namespace isoft