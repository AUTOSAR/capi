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
/// @file       types.h
/// @brief      Common type definitions
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
/// @unit_name=Types
/// @unit_description=Common type definitions
/// @endcode
///
/// ================================================================

#ifndef COMMON_TYPES_H_
#define COMMON_TYPES_H_

#include <cinttypes>

namespace ara {
namespace ucm {
namespace vpkgmgr {

/// @brief char
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
using char8_t = char;  // NOLINT

/// @brief unsigned char
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
using uchar8_t = unsigned char;  // NOLINT

constexpr uint32_t kInt32_40U{40U};
constexpr uint32_t kInt32_50U{50U};
constexpr uint32_t kInt32_1024U{1024U};
constexpr uint32_t kInt32_3000U{3000U};
}  // namespace vpkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // COMMON_TYPES_H_