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
/// @file       ucm_strings.h
/// @brief      Definition of string constants involved in parsing
/// @details
/// @date       2024-08-15
/// @author     cuiyinli
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/UCM/SoftwarePackageManager
/// @interface_level=unit
/// @trace_id_sr=SR_UCM_00001
/// @unit_name=UcmStrings
/// @unit_description=Definition of string constants involved in parsing
/// @endcode
///
/// ================================================================

#ifndef ARA_UCM_PKGMGR_PARSING_UCM_STRINGS_H_
#define ARA_UCM_PKGMGR_PARSING_UCM_STRINGS_H_

#include "ara/core/string.h"

namespace ara {
namespace ucm {
namespace pkgmgr {
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
constexpr ara::core::StringView kActivationStringRestartApplication{"RestartApplication"};  // Not used for now
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
constexpr ara::core::StringView kActivationStringSystemRestart{"SystemRestart"};  // Not used for now
// const ara::core::String ActivationStringWaitForReboot = "WaitForReboot";
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
constexpr ara::core::StringView kActionStringUpdate{"Update"};  // Not used for now
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
constexpr ara::core::StringView kActionStringRemove{"Remove"};  // Not used for now
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
constexpr ara::core::StringView kActionStringInstall{"Install"};  // Not used for now
}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // ARA_UCM_PKGMGR_PARSING_UCM_STRINGS_H_
