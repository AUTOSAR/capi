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
/// @file       manifest_parse.h
/// @brief      ManifestParse header
/// @details
/// @date       2024-07-20
/// @author     hanzhibo
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// export_level=/
/// @module_path=/UCM Master/Fsm
/// @interface_level=unit
/// @trace_id_sr=SR_UCM_00035
/// @unit_name=ManifestParse
/// @unit_description=ManifestParse header
/// @endcode
///
/// ================================================================

#ifndef VPKGMGR_SRC_PARSING_MANIFEST_PARSE_H_
#define VPKGMGR_SRC_PARSING_MANIFEST_PARSE_H_

#include <ara/core/result.h>
#include <ara/core/string.h>
#include <ara/core/string_view.h>

#include "fsm/fsm_manager.h"

namespace ara {
namespace ucm {
namespace vpkgmgr {

/// @brief ManifestParse
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_Master_00484
/// @trace_id_dd=DD_UCM_Master_00307
/// @needwork = ad
/// @endcode
class ManifestParse
{
public:
    /// @brief Parse
    /// @param manifestFile
    /// @return int
    /// @code{.isoft}
    /// @throw
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00308
    /// @needwork = dda
    /// @endcode
    int32_t Parse(ara::core::StringView const manifestFile);

    /// @brief GetApiWaitTime
    /// @return int
    /// @code{.isoft}
    /// @throw
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00309
    /// @needwork = dda
    /// @endcode
    inline uint32_t GetApiWaitTime() const noexcept { return apiWaitTime_; };

private:
    /// @brief apiWaitTime_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00310
    /// @needwork = dda
    /// @endcode
    uint32_t apiWaitTime_;
};

}  // namespace vpkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // VPKGMGR_SRC_PARSING_MANIFEST_PARSE_H_