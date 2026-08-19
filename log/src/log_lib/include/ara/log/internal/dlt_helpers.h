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
/// @file       dlt_helpers.h
/// @brief      DLT protocol helper functions for internal encoding module
/// @details
/// @date       2025-09-03
/// @author     yangjinbiao
/// @version    1.2.0
///
/// ================================================================

#ifndef __LOG_INTERNAL_DLT_HELPERS__
#define __LOG_INTERNAL_DLT_HELPERS__

#include <cstdint>

#include "dlt_constants.h"

namespace ara {
namespace log {
namespace internal {

// ==================== HTYP (Header Type) Helper Functions ====================

/// @brief Check if extended header is enabled
inline bool DLT_IS_HTYP_UEH(std::uint8_t const& htyp) noexcept { return ((htyp)&kDlt_Htyp_Ueh) != 0; }

/// @brief Check if ECU ID is present
inline bool DLT_IS_HTYP_WEID(std::uint8_t const& htyp) noexcept { return ((htyp)&kDlt_Htyp_Weid) != 0; }

/// @brief Check if Session ID is present
inline bool DLT_IS_HTYP_WSID(std::uint8_t const& htyp) noexcept { return ((htyp)&kDlt_Htyp_Wsid) != 0; }

/// @brief Check if timestamp is present
inline bool DLT_IS_HTYP_WTMS(std::uint8_t const& htyp) noexcept { return ((htyp)&kDlt_Htyp_Wtms) != 0; }

/// @brief Calculate extra header size based on htyp
inline std::uint32_t DLT_STANDARD_HEADER_EXTRA_SIZE(std::uint8_t const& htyp)
{
    std::uint32_t ret{0U};
    if (DLT_IS_HTYP_WEID(htyp)) {
        ret += kDlt_Size_Weid;
    }
    if (DLT_IS_HTYP_WSID(htyp)) {
        ret += kDlt_Size_Weid;
    }
    if (DLT_IS_HTYP_WTMS(htyp)) {
        ret += kDlt_Size_Wtms;
    }
    return ret;
}

}  // namespace internal
}  // namespace log
}  // namespace ara

#endif  // __LOG_INTERNAL_DLT_HELPERS__
