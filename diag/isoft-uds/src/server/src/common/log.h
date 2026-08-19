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
/// @file       log.h
/// @brief      This file provides log functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/isoft/uds/common
/// @interface_level=software
/// @trace_id_sr=8521d95b-48dc-4d3f-b024-981bdbf1773b
/// @unit_name=log
/// @unit_description=This file provides log functionality
/// @endcode
///
/// ================================================================

#ifndef ARA_DIAG_COMMON_LOG_H_
#define ARA_DIAG_COMMON_LOG_H_

#include <ara/log/log_stream.h>

namespace isoft {
namespace uds {
namespace server {
/// @brief Log stream type
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02568
/// @needwork = dd
/// @endcode
using LogHex8 = ara::log::LogHex8;
/// @brief Log stream type
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02569
/// @needwork = dd
/// @endcode
using LogHex16 = ara::log::LogHex16;

/// @brief Fatal level log
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02570
/// @needwork = dd
/// @endcode
ara::log::LogStream LogFatal() noexcept;

/// @brief Error level log
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02571
/// @needwork = dd
/// @endcode
ara::log::LogStream LogError() noexcept;

/// @brief Warning level log
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02572
/// @needwork = dd
/// @endcode
ara::log::LogStream LogWarn() noexcept;

/// @return log stream object
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_
/// @trace_id_dd=DD_UDS_00823
/// @needwork = dd
/// @endcode
ara::log::LogStream LogInfo() noexcept;

/// @brief Debug level log
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02573
/// @needwork = dd
/// @endcode
ara::log::LogStream LogDebug() noexcept;

/// @brief Verbose level log
/// @return log stream object
/// @code{.isoft}
/// @interface_level=unit
/// @needwork=dd
/// @needwork=dd
/// @endcode
ara::log::LogStream LogVerbose() noexcept;

/// @brief HexFormat function
/// @param const Parameter
/// @return Return value
/// @needwork=dd
/// @endcode
LogHex8 HexFormat(std::uint8_t const& value) noexcept;

/// @brief HexFormat function
/// @param const Parameter
/// @return Return value
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02574
/// @needwork = dd
/// @endcode
LogHex16 HexFormat(std::uint16_t const& value) noexcept;

/// @brief Whether to enable the specified log level
/// @param[in] logLevel Log level
/// @return Whether enabled
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02575
/// @needwork = dd
/// @endcode
bool IsEnabled(ara::log::LogLevel const logLevel) noexcept;
}  // namespace server
}  // namespace uds
}  // namespace isoft

#endif  // ARA_DIAG_COMMON_LOG_H_