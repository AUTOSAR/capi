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
/// @file       common.h
/// @brief      =System call error errno
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
/// @trace_id_sr=SRS_IDSM_0011
/// @unit_description=System call error errno
/// @endcode
///
/// ================================================================

#ifndef ARA_IDSM_INTERNAL_COMMON_H_
#define ARA_IDSM_INTERNAL_COMMON_H_
#include <cerrno>
#include <cstdint>
namespace ara {
namespace idsm {
/// @brief Return operating system error number errno
/// @return Operating system error number errno
/// @code{.isoft}
/// @unit_name=SysErr
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00062
/// @trace_id_dd=DD_IDSM_00114
/// @needwork = ad
/// @endcode
inline int32_t SysErr() noexcept { return errno; }
}  // namespace idsm
}  // namespace ara
#endif