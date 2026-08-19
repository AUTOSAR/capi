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
/// @brief      fw
/// @details    fw deinitialization processing
/// @date       2025-05-06
/// @author     jzy
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/fw/firewall initialization
/// @interface_level=module
/// export_level=module
/// @trace_id_sr=SR_FW_0001
/// @unit_name=Initialize
/// @endcode
///
/// ================================================================
///
///
/// ================================================================

#ifndef _ARA_FW_INITIALIZATION_H_
#define _ARA_FW_INITIALIZATION_H_
#include <ara/core/result.h>

#include <cstdio>
#include <iostream>
namespace ara {
namespace fw {
namespace internal {
/// @brief Deinitialization function
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00195
/// @trace_id_dd=DD_FW_00313
/// @needwork = ad
/// @endcode
ara::core::Result< void > Deinitialize() noexcept;
}  // namespace internal
}  // namespace fw
}  // namespace ara
#endif  // _ARA_IDSM_INITIALIZATION_H_
