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
/// @file       log.cpp
/// @brief      This file implements log functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=common
/// @trace_id_sr=8521d95b-48dc-4d3f-b024-981bdbf1773b
/// @unit_name=log
/// @unit_description=This file provides log functionality
/// @interface_level=unit
/// @endcode
///
/// ================================================================

#include "common/log.h"

#include <ara/log/logger.h>

namespace isoft {
namespace uds {
namespace server {


static ara::log::Logger& GetDefaultLogger() noexcept
{
    static ara::log::Logger& s_Logger{
        ara::log::CreateLogger(std::move(ara::core::StringView("#UDS")), std::move(ara::core::StringView("udslib")))};
    return s_Logger;
}


ara::log::LogStream LogFatal() noexcept { return GetDefaultLogger().LogFatal(); }


ara::log::LogStream LogError() noexcept { return GetDefaultLogger().LogError(); }


ara::log::LogStream LogWarn() noexcept { return GetDefaultLogger().LogWarn(); }


ara::log::LogStream LogInfo() noexcept { return GetDefaultLogger().LogInfo(); }


ara::log::LogStream LogDebug() noexcept { return GetDefaultLogger().LogDebug(); }


ara::log::LogStream LogVerbose() noexcept { return GetDefaultLogger().LogVerbose(); }

LogHex8 HexFormat(std::uint8_t const& value) noexcept { return ara::log::HexFormat(value); }

LogHex16 HexFormat(std::uint16_t const& value) noexcept { return ara::log::HexFormat(value); }


bool IsEnabled(ara::log::LogLevel const logLevel) noexcept { return GetDefaultLogger().IsEnabled(logLevel); }

}  // namespace server
}  // namespace uds
}  // namespace isoft
