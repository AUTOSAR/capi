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
/// @file       phm_log.h
/// @brief      phm log.
/// @details
/// @date       2024-06-06
/// @author     wangyanlong
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/PlatformHealthManagement/Util
/// @unit_description=phm log.
/// @trace_id_sr=SR_PHM_01014,SR_PHM_01015,SR_PHM_01016,SR_PHM_01017,
/// SR_PHM_01022,SR_PHM_01023,SR_PHM_01025,SR_PHM_01026,SR_PHM_01029,SR_PHM_01030,SR_PHM_01031
/// @unit_name=PhmLog
/// @interface_level=module
/// @endcode
///
/// ================================================================

#ifndef ARA_PHM_INTERNAL_UTIL_PHM_LOG_H_
#define ARA_PHM_INTERNAL_UTIL_PHM_LOG_H_

#include <ara/log/logger.h>

namespace ara {
namespace phm {
namespace internal {

/// @brief returns logger.
/// @return logger.
/// @trace_id_sr=SR_PHM_01014,SR_PHM_01015,SR_PHM_01016,SR_PHM_01017,
///              SR_PHM_01022,SR_PHM_01023,SR_PHM_01025,SR_PHM_01026,SR_PHM_01029,SR_PHM_01030,SR_PHM_01031
/// @needwork = ad
ara::log::Logger& GetDefaultLogger() noexcept;

}  // namespace internal
}  // namespace phm
}  // namespace ara

///@brief A macro of fatal log.
/// @trace_id_sr=SR_PHM_01014,SR_PHM_01015,SR_PHM_01016,SR_PHM_01017,
///              SR_PHM_01022,SR_PHM_01023,SR_PHM_01025,SR_PHM_01026,SR_PHM_01029,SR_PHM_01030,SR_PHM_01031
/// @needwork = dd
#define LOG_FATAL ara::phm::internal::GetDefaultLogger().LogFatal() << __FILE__ << __LINE__

///@brief A macro of error log.
/// @trace_id_sr=SR_PHM_01014,SR_PHM_01015,SR_PHM_01016,SR_PHM_01017,
/// @needwork = dd
#define LOG_ERROR ara::phm::internal::GetDefaultLogger().LogError() << __FILE__ << __LINE__

///@brief A macro of warn log.
/// @trace_id_sr=SR_PHM_01014,SR_PHM_01015,SR_PHM_01016,SR_PHM_01017,
///              SR_PHM_01022,SR_PHM_01023,SR_PHM_01025,SR_PHM_01026,SR_PHM_01029,SR_PHM_01030,SR_PHM_01031
/// @needwork = dd
#define LOG_WARN ara::phm::internal::GetDefaultLogger().LogWarn() << __FILE__ << __LINE__

///@brief A macro of info log.
/// @trace_id_sr=SR_PHM_01014,SR_PHM_01015,SR_PHM_01016,SR_PHM_01017,
/// @needwork = dd
#define LOG_INFO ara::phm::internal::GetDefaultLogger().LogInfo() << __FILE__ << __LINE__

///@brief A macro of debug log.
/// @trace_id_sr=SR_PHM_01014,SR_PHM_01015,SR_PHM_01016,SR_PHM_01017,
///              SR_PHM_01022,SR_PHM_01023,SR_PHM_01025,SR_PHM_01026,SR_PHM_01029,SR_PHM_01030,SR_PHM_01031
/// @needwork = dd
#define LOG_DEBUG ara::phm::internal::GetDefaultLogger().LogDebug() << __FILE__ << __LINE__

///@brief A macro of verbose log.
/// @trace_id_sr=SR_PHM_01014,SR_PHM_01015,SR_PHM_01016,SR_PHM_01017,
///              SR_PHM_01022,SR_PHM_01023,SR_PHM_01025,SR_PHM_01026,SR_PHM_01029,SR_PHM_01030,SR_PHM_01031
/// @needwork = dd
#define LOG_VERBOSE ara::phm::internal::GetDefaultLogger().LogVerbose() << __FILE__ << __LINE__

#endif  // ARA_PHM_INTERNAL_UTIL_PHM_LOG_H_
