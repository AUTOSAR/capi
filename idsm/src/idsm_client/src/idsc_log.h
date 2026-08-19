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
/// @file       idsc_log.h
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
/// @interface_level=unit
/// @trace_id_sr=SRS_IDSM_0005
/// @unit_description=Logging
/// @endcode
///
/// ================================================================

#ifndef ARA_IDSC_LOG_H_
#define ARA_IDSC_LOG_H_

#include <ara/log/log_stream.h>
#include <ara/log/logger.h>

namespace ara {
namespace idsm {
namespace common {
/// @brief Get logger object
/// @return Logger object
/// @code{.isoft}
/// @unit_name=GetDefaultLogger
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00002
/// @trace_id_dd=DD_IDSM_00028
/// @needwork = ad
/// @endcode
ara::log::Logger& GetDefaultLogger() noexcept;
/// @brief Print Fatal level log
/// @return Print log
/// @code{.isoft}
/// @unit_name=LogFatal
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00003
/// @trace_id_dd=DD_IDSM_00029
/// @needwork = ad
/// @endcode
inline ara::log::LogStream LogFatal() noexcept { return GetDefaultLogger().LogFatal(); }
/// @brief Print Error level log
/// @return Print log
/// @code{.isoft}
/// @unit_name=LogError
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00004
/// @trace_id_dd=DD_IDSM_00030
/// @needwork = ad
/// @endcode
inline ara::log::LogStream LogError() noexcept { return GetDefaultLogger().LogError(); }
/// @brief Print Warn level log
/// @return Print log
/// @code{.isoft}
/// @unit_name=LogWarn
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00005
/// @trace_id_dd=DD_IDSM_00031
/// @needwork = ad
/// @endcode
inline ara::log::LogStream LogWarn() noexcept { return GetDefaultLogger().LogWarn(); }
/// @brief Print Info level log
/// @return Print log
/// @code{.isoft}
/// @unit_name=LogInfo
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00006
/// @trace_id_dd=DD_IDSM_00032
/// @needwork = ad
/// @endcode
inline ara::log::LogStream LogInfo() noexcept { return GetDefaultLogger().LogInfo(); }
/// @brief Print Debug level log
/// @return Print log
/// @code{.isoft}
/// @unit_name=LogDebug
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00007
/// @trace_id_dd=DD_IDSM_00033
/// @needwork = ad
/// @endcode
inline ara::log::LogStream LogDebug() noexcept { return GetDefaultLogger().LogDebug(); }
/// @brief Print Verbose level log
/// @return Print log
/// @code{.isoft}
/// @unit_name=LogVerbose
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00008
/// @trace_id_dd=DD_IDSM_00034
/// @needwork = ad
/// @endcode
inline ara::log::LogStream LogVerbose() noexcept { return GetDefaultLogger().LogVerbose(); }

}  // namespace common
}  // namespace idsm
}  // namespace ara

/// @brief LOG_FATAL macro definition
/// @code{.isoft}
/// @unit_name=IDSC_LOG_FATAL
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00009
/// @trace_id_dd=DD_IDSM_00035
/// @needwork = ad
/// @endcode
#define IDSC_LOG_FATAL ara::idsm::common::LogFatal() << __FILE__ << __LINE__
/// @brief LOG_ERROR macro definition
/// @code{.isoft}
/// @unit_name=IDSC_LOG_ERROR
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00010
/// @trace_id_dd=DD_IDSM_00036
/// @needwork = ad
/// @endcode
#define IDSC_LOG_ERROR ara::idsm::common::LogError() << __FILE__ << __LINE__
/// @brief LOG_WARN macro definition
/// @code{.isoft}
/// @unit_name=IDSC_LOG_WARN
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00011
/// @trace_id_dd=DD_IDSM_00037
/// @needwork = ad
/// @endcode
#define IDSC_LOG_WARN ara::idsm::common::LogWarn() << __FILE__ << __LINE__
/// @brief LOG_INFO macro definition
/// @code{.isoft}
/// @unit_name=IDSC_LOG_INFO
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00012
/// @trace_id_dd=DD_IDSM_00038
/// @needwork = ad
/// @endcode
#define IDSC_LOG_INFO ara::idsm::common::LogInfo() << __FILE__ << __LINE__
/// @brief LOG_DEBUG macro definition
/// @code{.isoft}
/// @unit_name=IDSC_LOG_DEBUG
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00013
/// @trace_id_dd=DD_IDSM_00039
/// @needwork = ad
/// @endcode
#define IDSC_LOG_DEBUG ara::idsm::common::LogDebug() << __FILE__ << __LINE__
/// @brief LOG_VERBOSE macro definition
/// @code{.isoft}
/// @unit_name=IDSC_LOG_VERBOSE
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00014
/// @trace_id_dd=DD_IDSM_00040
/// @needwork = ad
/// @endcode
#define IDSC_LOG_VERBOSE ara::idsm::common::LogVerbose() << __FILE__ << __LINE__

#endif
