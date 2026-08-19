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
/// @file       initialization.h
/// @brief      Template class for encapsulating log parameters
/// @details
/// @date       2024-06-28
/// @author     yangjinbiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @export_level=/LOG/DltLog
/// @module_path=/LOG/DltLog
/// @unit_name=log_inter_initialization
/// @interface_level = unit
/// @trace_id_sr=SR_LOG_1
/// @endcode
///
/// ================================================================

#ifndef __INTERNAL_LOG_LIB_API__
#define __INTERNAL_LOG_LIB_API__
#include "ara/core/error_code.h"
#include "ara/core/instance_specifier.h"
#include "ara/core/span.h"
namespace ara {
namespace log {
namespace internal {
/// @brief Internal initialization of log
/// @return Initialization result
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_LOG_00336
/// @trace_id_dd=DD_LOG_01735
/// @needwork = ad
/// @endcode
ara::core::Result< void > Initialize() noexcept;
/// @brief Internal deinitialization of log
/// @return Deinitialization result
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_LOG_00337
/// @trace_id_dd=DD_LOG_01736
/// @needwork = ad
/// @endcode
ara::core::Result< void > Deinitialize() noexcept;
/// @brief Whether the log has been initialized
/// @return Whether initialization is ok
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_LOG_00338
/// @trace_id_dd=DD_LOG_01737
/// @needwork = ad
/// @endcode
bool LogInitialized() noexcept;

}  // namespace internal
}  // namespace log
}  // namespace ara

#endif  // ARA_LOG_INITIALIZATION_ARA_H_