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
/// @file       delete.h
/// @brief      FW engine table and chain destruction
/// @details    FW engine table and chain destruction
/// @date       2025-05-06
/// @author     jzy
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/fw/Firewall Engine Abstraction Layer
/// @interface_level=module
/// export_level=module
/// @trace_id_sr=SR_FW_0010
/// @unit_name=DeleteTable
/// @unit_description=Firewall engine abstraction layer deletes nft table.
/// @endcode
///
/// ================================================================
///
///
/// ================================================================

#ifndef _ARA_FW_FILTER_DELETE_H_
#define _ARA_FW_FILTER_DELETE_H_
#include <cstdint>
#include <cstdlib>
#include <cstring>
namespace ara {
namespace fw {
namespace internal {

/// @brief Clear chain rule
/// @return success/failed
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00001
/// @trace_id_dd=DD_FW_00001
/// @needwork = ad
/// @endcode
bool FilterRulesDelete(int32_t const &inOutFlag) noexcept;

/// @brief Delete chain
/// @return success/failed
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00002
/// @trace_id_dd=DD_FW_00002
/// @needwork = ad
/// @endcode
bool FilterChainDelete(int32_t const &inOutFlag) noexcept;

/// @brief Delete table
/// @return success/failed
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00003
/// @trace_id_dd=DD_FW_00003
/// @needwork = ad
/// @endcode
bool FilterTableDelete() noexcept;
}  // namespace internal
}  // namespace fw
}  // namespace ara
#endif