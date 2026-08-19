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
/// @file       base_executor.h
/// @brief      Base executor
/// @details
/// @date       2024-08-15
/// @author     cuiyinli
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @export_level=/UCM/FsmManager
/// @module_path=/UCM/FsmManager
/// @interface_level=unit
/// @trace_id_sr=SR_UCM_00001
/// @unit_name=BaseExecutor
/// @unit_description=Base executor
/// @endcode
///
/// ================================================================

#ifndef PACKAGE_MANAGER_BASE_EXECUTOR_H
#define PACKAGE_MANAGER_BASE_EXECUTOR_H

#include "util/noncopy.h"

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @brief class of FsmManager
class FsmManager;

/// @brief BaseExecutor
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10394
/// @trace_id_dd=DD_UCM_10964
/// @needwork = ad
/// @endcode
class BaseExecutor : NonCopyNonMove
{
public:
    /// @brief constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10965
    /// @needwork = dda
    /// @endcode
    BaseExecutor() = default;
    /// @brief destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10966
    /// @needwork = dda
    /// @endcode
    virtual ~BaseExecutor() = default;

    /// @brief delete copy construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10967
    /// @needwork = dda
    /// @endcode
    BaseExecutor(BaseExecutor const& other) = delete;
    /// @brief delete copy asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10968
    /// @needwork = dda
    /// @endcode
    BaseExecutor& operator=(BaseExecutor const& other) = delete;
    /// @brief delete move construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10969
    /// @needwork = dda
    /// @endcode
    BaseExecutor(BaseExecutor&& other) = delete;
    /// @brief delete move asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10970
    /// @needwork = dda
    /// @endcode
    BaseExecutor& operator=(BaseExecutor&& other) = delete;

    /// @brief set owner from FsmManager
    /// @param owner FsmManager
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10971
    /// @needwork = dda
    /// @endcode
    void SetOwner(FsmManager* const owner) noexcept { owner_ = owner; }

protected:
    /// @brief owner from FsmManager
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10972
    /// @needwork = dda
    /// @endcode
    FsmManager* owner_{nullptr};  // NOLINT
};

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // PACKAGE_MANAGER_BASE_EXECUTOR_H
