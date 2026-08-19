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
/// @file       ucmm_log.cpp
/// @brief      UCMM logging utilities implementation
/// @details
/// @date       2024-07-20
/// @author     hanzhibo
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/UCM Master/Utils
/// @interface_level=module
/// @trace_id_sr=SR_UCM_00035
/// @unit_name=UCMMLog
/// @unit_description=UCMM logging utilities implementation
/// @endcode
///
/// ================================================================

#include "utils/ucmm_log.h"

namespace ara {
namespace ucmm {
namespace common {

/// @brief global default logger
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
static ara::log::Logger* g_LoggerInstance{nullptr};

/// @brief GetDefaultLogger
/// @return Logger
/// @throws no
ara::log::Logger& GetDefaultLogger() noexcept
{
    if (nullptr == g_LoggerInstance) {
        g_LoggerInstance = &(ara::log::CreateLogger(std::move(ara::core::StringView("#UCM")),
                                                    std::move(ara::core::StringView("ucmm run context")),
                                                    ara::log::LogLevel::kVerbose));
    }

    return *g_LoggerInstance;
}

void DestroyDefaultLogger() noexcept { g_LoggerInstance = nullptr; }

}  // namespace common
}  // namespace ucmm
}  // namespace ara
