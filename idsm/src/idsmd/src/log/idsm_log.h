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
/// @file       idsm_log.h
/// @brief      Log
/// @details
/// @date       2023-02-15
/// @author     niuliming
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/idsm/Common
/// @interface_level=module
/// @trace_id_sr=SRS_IDSM_0001
/// @unit_description=Log
/// @endcode
///
/// ================================================================

#ifndef ARA_IDSM_LOG_H_
#define ARA_IDSM_LOG_H_
#include <ara/log/log_stream.h>
#include <ara/log/logger.h>

namespace ara {
namespace idsm {
namespace common {
/// @brief Get Logger object of log module
/// @return Logger object of log module
/// @code{.isoft}
/// @unit_name=GetDefaultLogger
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00153
/// @trace_id_dd=DD_IDSM_00378
/// @needwork = ad
/// @endcode
ara::log::Logger& GetDefaultLogger() noexcept;
/// @brief Print Fatal level log
/// @return output log
/// @code{.isoft}
/// @unit_name=LogFatal
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00154
/// @trace_id_dd=DD_IDSM_00379
/// @needwork = ad
/// @endcode
inline ara::log::LogStream LogFatal() noexcept { return GetDefaultLogger().LogFatal(); }
/// @brief Print Error level log
/// @return output log
/// @code{.isoft}
/// @unit_name=LogError
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00155
/// @trace_id_dd=DD_IDSM_00380
/// @needwork = ad
/// @endcode
inline ara::log::LogStream LogError() noexcept { return GetDefaultLogger().LogError(); }
/// @brief Print Warn level log
/// @return output log
/// @code{.isoft}
/// @unit_name=LogWarn
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00156
/// @trace_id_dd=DD_IDSM_00381
/// @needwork = ad
/// @endcode
inline ara::log::LogStream LogWarn() noexcept { return GetDefaultLogger().LogWarn(); }
/// @brief Print Info level log
/// @return output log
/// @code{.isoft}
/// @unit_name=LogInfo
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00157
/// @trace_id_dd=DD_IDSM_00382
/// @needwork = ad
/// @endcode
inline ara::log::LogStream LogInfo() noexcept { return GetDefaultLogger().LogInfo(); }
/// @brief Print Debug level log
/// @return output log
/// @code{.isoft}
/// @unit_name=LogDebug
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00158
/// @trace_id_dd=DD_IDSM_00383
/// @needwork = ad
/// @endcode
inline ara::log::LogStream LogDebug() noexcept { return GetDefaultLogger().LogDebug(); }
/// @brief Print Verbose level log
/// @return output log
/// @code{.isoft}
/// @unit_name=LogVerbose
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00159
/// @trace_id_dd=DD_IDSM_00384
/// @needwork = ad
/// @endcode
inline ara::log::LogStream LogVerbose() noexcept { return GetDefaultLogger().LogVerbose(); }

}  // namespace common
}  // namespace idsm
}  // namespace ara

/// @brief LOG_FATAL macro definition
/// @code{.isoft}
/// @unit_name=LOG_FATAL
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00160
/// @trace_id_dd=DD_IDSM_00385
/// @needwork = ad
/// @endcode
#define LOG_FATAL ara::idsm::common::LogFatal() << __FILE__ << __LINE__
/// @brief LOG_ERROR macro definition
/// @code{.isoft}
/// @unit_name=LOG_ERROR
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00161
/// @trace_id_dd=DD_IDSM_00386
/// @needwork = ad
/// @endcode
#define LOG_ERROR ara::idsm::common::LogError() << __FILE__ << __LINE__
/// @brief LOG_WARN macro definition
/// @code{.isoft}
/// @unit_name=LOG_WARN
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00162
/// @trace_id_dd=DD_IDSM_00387
/// @needwork = ad
/// @endcode
#define LOG_WARN ara::idsm::common::LogWarn() << __FILE__ << __LINE__
/// @brief LOG_INFO macro definition
/// @code{.isoft}
/// @unit_name=LOG_INFO
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00163
/// @trace_id_dd=DD_IDSM_00388
/// @needwork = ad
/// @endcode
#define LOG_INFO ara::idsm::common::LogInfo() << __FILE__ << __LINE__
/// @brief LOG_DEBUG macro definition
/// @code{.isoft}
/// @unit_name=LOG_DEBUG
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00164
/// @trace_id_dd=DD_IDSM_00389
/// @needwork = ad
/// @endcode
#define LOG_DEBUG ara::idsm::common::LogDebug() << __FILE__ << __LINE__
/// @brief LOG_VERBOSE macro definition
/// @code{.isoft}
/// @unit_name=LOG_VERBOSE
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00165
/// @trace_id_dd=DD_IDSM_00390
/// @needwork = ad
/// @endcode
#define LOG_VERBOSE ara::idsm::common::LogVerbose() << __FILE__ << __LINE__

#endif
