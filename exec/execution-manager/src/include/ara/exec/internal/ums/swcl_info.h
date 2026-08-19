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
/// @file       swcl_info.h
/// @brief      Software cluster information
/// @details
/// @date       2023-04-01
/// @author     xueliang.bao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/ExecutionManagement/UpdateClient
/// @unit_name=SwclInfo
/// @unit_description=Used to represent software collection information.
/// @interface_level=software
/// @endcode
///
/// ================================================================

#ifndef ARA_EXEC_INTERNAL_UMS_SWCL_INFO_H_
#define ARA_EXEC_INTERNAL_UMS_SWCL_INFO_H_

#include <ara/core/string.h>

namespace ara {
namespace exec {
namespace internal {
namespace ums {

/// @brief Software cluster information class
/// @code{.isoft}
/// @interface_level=software
/// @trace_id_sr=SR_EM_10007
/// @trace_id_ad=AD_EM_00046
/// @trace_id_dd=DD_EM_00719
/// @needwork = dd
/// @endcode
class SwclInfo  // PRQA S 5215
{
public:
    /// @brief Tag type definition
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    using IsEnumerableTag = void;

    /// @brief Enumerate function
    /// @tparam F Enumerate function type
    /// @param fun Enumerate function
    /// @exception std::bad_alloc Thrown when memory allocation fails
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    template < typename F >
    void enumerate(F& fun)  // NOLINT
    {
        fun(swclName);
        fun(swclVer);
    }

public:
    /// @brief Software cluster name
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_EM_00046
    /// @trace_id_dd=DD_EM_00720
    /// @needwork = dda
    /// @endcode
    ara::core::String swclName;
    /// @brief Software cluster version
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_EM_00046
    /// @trace_id_dd=DD_EM_00721
    /// @needwork = dda
    /// @endcode
    ara::core::String swclVer;
};

}  // namespace ums
}  // namespace internal
}  // namespace exec
}  // namespace ara

#endif  ///< ARA_EXEC_INTERNAL_UMS_SWCL_INFO_H_
