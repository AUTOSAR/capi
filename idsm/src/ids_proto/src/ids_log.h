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
/// @file       ids_log.h
/// @brief      Logging
/// @details
/// @date       2022-12-27
/// @author     niuliming
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/idsm/Common
/// @export_level=/idsm/Common
/// @interface_level=unit
/// @trace_id_sr=SRS_IDSM_0011
/// @unit_description=Logging
/// @endcode
///
/// ================================================================

#ifndef ARA_IDS_LOG_H_
#define ARA_IDS_LOG_H_

#include <ara/log/log_stream.h>
#include <ara/log/logger.h>

namespace ara {
namespace ids {
namespace common {
/// @brief Get logger object
/// @return Logger object
/// @code{.isoft}
/// @unit_name=GetDefaultLogger
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00094
/// @trace_id_dd=DD_IDSM_00168
/// @needwork = ad
/// @endcode
ara::log::Logger& GetDefaultLogger() noexcept;
/// @brief Print Fatal level log
/// @return Print log
/// @code{.isoft}
/// @unit_name=LogFatal
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00095
/// @trace_id_dd=DD_IDSM_00169
/// @needwork = ad
/// @endcode
inline ara::log::LogStream LogFatal() noexcept { return GetDefaultLogger().LogFatal(); }
/// @brief Print Error level log
/// @return Print log
/// @code{.isoft}
/// @unit_name=LogError
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00096
/// @trace_id_dd=DD_IDSM_00170
/// @needwork = ad
/// @endcode
inline ara::log::LogStream LogError() noexcept { return GetDefaultLogger().LogError(); }
/// @brief Print Warn level log
/// @return Print log
/// @code{.isoft}
/// @unit_name=LogWarn
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00097
/// @trace_id_dd=DD_IDSM_00171
/// @needwork = ad
/// @endcode
inline ara::log::LogStream LogWarn() noexcept { return GetDefaultLogger().LogWarn(); }
/// @brief Print Info level log
/// @return Print log
/// @code{.isoft}
/// @unit_name=LogInfo
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00098
/// @trace_id_dd=DD_IDSM_00172
/// @needwork = ad
/// @endcode
inline ara::log::LogStream LogInfo() noexcept { return GetDefaultLogger().LogInfo(); }
/// @brief Print Debug level log
/// @return Print log
/// @code{.isoft}
/// @unit_name=LogDebug
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00099
/// @trace_id_dd=DD_IDSM_00173
/// @needwork = ad
/// @endcode
inline ara::log::LogStream LogDebug() noexcept { return GetDefaultLogger().LogDebug(); }
/// @brief Print Verbose level log
/// @return Print log
/// @code{.isoft}
/// @unit_name=LogVerbose
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00100
/// @trace_id_dd=DD_IDSM_00174
/// @needwork = ad
/// @endcode
inline ara::log::LogStream LogVerbose() noexcept { return GetDefaultLogger().LogVerbose(); }

}  // namespace common
}  // namespace ids
}  // namespace ara

/// @brief LOG_FATAL macro definition
/// @code{.isoft}
/// @unit_name=IDS_LOG_FATAL
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00101
/// @trace_id_dd=DD_IDSM_00175
/// @needwork = ad
/// @endcode
#define IDS_LOG_FATAL ara::ids::common::LogFatal() << __FILE__ << __LINE__
/// @brief LOG_ERROR macro definition
/// @code{.isoft}
/// @unit_name=IDS_LOG_ERROR
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00102
/// @trace_id_dd=DD_IDSM_00176
/// @needwork = ad
/// @endcode
#define IDS_LOG_ERROR ara::ids::common::LogError() << __FILE__ << __LINE__
/// @brief LOG_WARN macro definition
/// @code{.isoft}
/// @unit_name=IDS_LOG_WARN
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00103
/// @trace_id_dd=DD_IDSM_00177
/// @needwork = ad
/// @endcode
#define IDS_LOG_WARN ara::ids::common::LogWarn() << __FILE__ << __LINE__
/// @brief LOG_INFO macro definition
/// @code{.isoft}
/// @unit_name=IDS_LOG_INFO
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00104
/// @trace_id_dd=DD_IDSM_00178
/// @needwork = ad
/// @endcode
#define IDS_LOG_INFO ara::ids::common::LogInfo() << __FILE__ << __LINE__
/// @brief LOG_DEBUG macro definition
/// @code{.isoft}
/// @unit_name=IDS_LOG_DEBUG
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00105
/// @trace_id_dd=DD_IDSM_00179
/// @needwork = ad
/// @endcode
#define IDS_LOG_DEBUG ara::ids::common::LogDebug() << __FILE__ << __LINE__
/// @brief LOG_VERBOSE macro definition
/// @code{.isoft}
/// @unit_name=IDS_LOG_VERBOSE
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00106
/// @trace_id_dd=DD_IDSM_00180
/// @needwork = ad
/// @endcode
#define IDS_LOG_VERBOSE ara::ids::common::LogVerbose() << __FILE__ << __LINE__

#endif
