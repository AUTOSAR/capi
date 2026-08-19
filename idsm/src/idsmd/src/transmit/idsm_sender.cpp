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
/// @file       idsm_sender.cpp
/// @brief      Remote transmission related classes
/// @details
/// @date       2023-01-13
/// @author     niuliming
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/idsm/qualified security event storage
/// @interface_level=module
/// @trace_id_sr=SR_IDSM_0011
/// @unit_description=
/// @endcode
///
/// ================================================================

#include "idsm_sender.h"

#include <iostream>
#include <memory>

namespace ara {
namespace idsm {
/// @brief Get specific data for transmission
/// @return Specific data for transmission
/// @code{.isoft}
/// @threadsafety={unsafe}
/// @endcode
TransDataPtr Message::GetData() const noexcept { return transData; }

}  // namespace idsm
}  // namespace ara
