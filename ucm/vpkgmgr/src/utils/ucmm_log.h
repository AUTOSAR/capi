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
/// @file       ucmm_log.h
/// @brief      UCMM logging utilities
/// @details
/// @date       2024-07-20
/// @author     hanzhibo
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// export_level=/UCM Master/Utils
/// @module_path=/UCM Master/Utils
/// @interface_level=module
/// @trace_id_sr=SR_UCM_00035
/// @unit_name=UCMMLog
/// @unit_description=UCMM logging utilities
/// @endcode
///
/// ================================================================

#ifndef ARA_UCMM_COMMON_LOG_H_
#define ARA_UCMM_COMMON_LOG_H_

#include <ara/log/log_stream.h>
#include <ara/log/logger.h>

#include "consts.h"

namespace ara {
namespace ucm {
namespace vpkgmgr {

/// @brief Convert bool to string
/// @param v
/// @return str
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_Master_00420
/// @trace_id_dd=DD_UCM_Master_00845
/// @needwork = ad
/// @endcode
inline ara::core::StringView BoolToStr(bool const& v)
{
    if (v) {
        return std::move(ara::core::StringView("true"));
    }
    return std::move(ara::core::StringView("false"));
}

}  // namespace vpkgmgr
}  // namespace ucm
}  // namespace ara

namespace ara {
namespace ucmm {
namespace common {

/// @brief GetDefaultLogger
/// @return Logger
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_Master_00421
/// @trace_id_dd=DD_UCM_Master_00846
/// @needwork = ad
/// @endcode
ara::log::Logger& GetDefaultLogger() noexcept;

/// @brief DestroyDefaultLogger
/// @return void
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_Master_00421
/// @trace_id_dd=DD_UCM_Master_00846
/// @needwork = dd
/// @endcode
void DestroyDefaultLogger() noexcept;

/// @brief LogFatal
/// @return LogStream
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_Master_00422
/// @trace_id_dd=DD_UCM_Master_00847
/// @needwork = ad
/// @endcode
inline ara::log::LogStream LogFatal() noexcept { return GetDefaultLogger().LogFatal(); }
/// @brief LogError
/// @return LogStrea
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_Master_00423
/// @trace_id_dd=DD_UCM_Master_00848
/// @needwork = ad
/// @endcode
inline ara::log::LogStream LogError() noexcept { return GetDefaultLogger().LogError(); }
/// @brief LogWarn
/// @return LogStream
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_Master_00424
/// @trace_id_dd=DD_UCM_Master_00849
/// @needwork = ad
/// @endcode
inline ara::log::LogStream LogWarn() noexcept { return GetDefaultLogger().LogWarn(); }
/// @brief LogInfo
/// @return LogStream
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_Master_00425
/// @trace_id_dd=DD_UCM_Master_00850
/// @needwork = ad
/// @endcode
inline ara::log::LogStream LogInfo() noexcept { return GetDefaultLogger().LogInfo(); }
/// @brief LogDebug
/// @return LogStream
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_Master_00426
/// @trace_id_dd=DD_UCM_Master_00851
/// @needwork = ad
/// @endcode
inline ara::log::LogStream LogDebug() noexcept { return GetDefaultLogger().LogDebug(); }
/// @brief LogVerbose
/// @return LogStream
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_Master_00427
/// @trace_id_dd=DD_UCM_Master_00852
/// @needwork = ad
/// @endcode
inline ara::log::LogStream LogVerbose() noexcept { return GetDefaultLogger().LogVerbose(); }

}  // namespace common
}  // namespace ucmm
}  // namespace ara

/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
#define LOG_FATAL ara::ucmm::common::LogFatal() << __PRETTY_FUNCTION__ << ","
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
#define LOG_ERROR ara::ucmm::common::LogError() << __PRETTY_FUNCTION__ << ","
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
#define LOG_WARN ara::ucmm::common::LogWarn() << __PRETTY_FUNCTION__ << ","
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
#define LOG_INFO ara::ucmm::common::LogInfo() << __PRETTY_FUNCTION__ << ","
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
#define LOG_DEBUG ara::ucmm::common::LogDebug() << __PRETTY_FUNCTION__ << ","
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
#define LOG_VERBOSE ara::ucmm::common::LogVerbose() << __PRETTY_FUNCTION__ << ","

#endif  // ARA_UCMM_COMMON_LOG_H_
