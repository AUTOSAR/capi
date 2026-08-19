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
/// @file       ara_log.h
/// @brief      log
/// @details
/// @date       2022-10-31
/// @author     jzy
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification Log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author       <th>Description
/// <tr><td> <td> <td> <td>Created initial version
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/IAM-COMMON
/// @interface_level=software
/// @trace_id_sr=SR_IAM_00501
/// @unit_name=IAM_Log
/// @unit_description=Provided by IAM
/// @endcode
///
/// ================================================================
///
/// rights reserved.
///
/// ================================================================

#ifndef ARA_IAM_COMMON_ARA_LOG_H_
#define ARA_IAM_COMMON_ARA_LOG_H_
#include <ara/log/logger.h>
namespace ara {
namespace iam {
namespace internal {
namespace common {
/// @brief iam logger
/// @return logger

/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_IAM_00337
/// @trace_id_dd=DD_IAM_00414
/// @needwork = ad
/// @endcode
static inline ara::log::Logger const &IamLogger() noexcept  // NOLINT
{
    static ara::log::Logger const &sS_IamLog{ara::log::CreateLogger(ara::core::StringView("#IAM"),
                                                                    ara::core::StringView("IdentifyAndAccessManager"),
                                                                    ara::log::LogLevel::kVerbose)};
    return sS_IamLog;
}
}  // namespace common
}  // namespace internal
}  // namespace iam
}  // namespace ara
#endif
