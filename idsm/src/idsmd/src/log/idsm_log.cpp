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
/// @file       idsm_log.cpp
/// @brief      Log
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
/// @trace_id_sr=SRS_IDSM_0001
/// @unit_description=
/// @endcode
///
/// ================================================================

#include "idsm_log.h"

namespace ara {
namespace idsm {
namespace common {
/// @brief Get Logger object of log module
/// @return Logger object of log module
ara::log::Logger& GetDefaultLogger() noexcept
{
    ara::core::StringView const ctx{"#ISD"};
    ara::core::StringView const desc{"idsm run context"};
    return ara::log::CreateLogger(ctx, desc);
}

}  // namespace common
}  // namespace idsm
}  // namespace ara
