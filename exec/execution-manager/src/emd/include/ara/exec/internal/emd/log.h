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
/// @file       log.h
/// @brief      Log definition
/// @details
/// @date       2023-04-01
/// @author     xueliang.bao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/ExecutionManagement/Emd
/// @interface_level=none
/// @endcode
///
/// ================================================================

#ifndef ARA_EXEC_INTERNAL_EMD_LOG_H_
#define ARA_EXEC_INTERNAL_EMD_LOG_H_

#include "ara/exec/internal/log/log.h"

#ifdef LOG
    #undef LOG
#endif

/// @brief Emd log macro definition
/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
#define LOG() internal::log1::Log< internal::emd::EmdLogCtx >()

namespace ara {
namespace exec {
namespace internal {
namespace emd {

/// @brief Emd log context information
/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
class EmdLogCtx  // PRQA S 5215
{
public:
    /// @brief char type redefinition
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    using Char8_t = char;

    /// @brief Get the log context ID
    /// @return Log context ID
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    static constexpr Char8_t const* GetLogCtxId() noexcept { return "#EMD"; }

    /// @brief Get the log context description
    /// @return Log context description
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    static constexpr Char8_t const* GetLogCtxDesc() noexcept { return "Execution Management Daemon"; }
};

}  // namespace emd
}  // namespace internal
}  // namespace exec
}  // namespace ara

#endif
