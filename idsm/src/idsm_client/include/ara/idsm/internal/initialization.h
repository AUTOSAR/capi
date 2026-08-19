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
/// @brief      =Idsm module deinitialization
/// @details
/// @date       2022-12-27
/// @author     niuliming
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path= /idsm/lib client
/// @interface_level=unit
/// @trace_id_sr=SRS_IDSM_0005
/// @unit_description=Idsm module deinitialization
/// @endcode
///
/// ================================================================

#ifndef _ARA_IDSM_INITIALIZATION_H_
#define _ARA_IDSM_INITIALIZATION_H_
#include <ara/core/result.h>

#include <cstdio>
#include <iostream>
namespace ara {
namespace idsm {
namespace internal {
/// @brief Deinitialization
/// @return Deinitialization result
/// @code{.isoft}
/// @unit_name=Deinitialize
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00051
/// @trace_id_dd=DD_IDSM_00098
/// @needwork = ad
/// @endcode
ara::core::Result< void > Deinitialize() noexcept;
}  // namespace internal
}  // namespace idsm
}  // namespace ara
#endif  // _ARA_IDSM_INITIALIZATION_H_
