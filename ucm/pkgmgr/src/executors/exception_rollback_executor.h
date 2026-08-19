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
/// @file       exception_rollback_executor.h
/// @brief      ExceptionRollbackExecutor header
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
/// @trace_id_sr=SR_UCM_00001,SR_UCM_00027
/// @unit_name=ExceptionRollbackExecutor
/// @unit_description=ExceptionRollbackExecutor header
/// @endcode
///
/// ================================================================

#ifndef PACKAGE_MANAGER_EXECUTOR_OF_EXCEPTION_ROLLBACK_H
#define PACKAGE_MANAGER_EXECUTOR_OF_EXCEPTION_ROLLBACK_H

#include "base_executor.h"
#include "common/alias.h"

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @brief ExceptionRollbackExecutor
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10395
/// @trace_id_dd=DD_UCM_10973
/// @needwork = ad
/// @endcode
class ExceptionRollbackExecutor : public BaseExecutor
{
public:
    /// @brief constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10974
    /// @needwork = dda
    /// @endcode
    ExceptionRollbackExecutor() = default;
    /// @brief destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10975
    /// @needwork = dda
    /// @endcode
    ~ExceptionRollbackExecutor() override = default;

    /// @brief delete copy construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10976
    /// @needwork = dda
    /// @endcode
    ExceptionRollbackExecutor(ExceptionRollbackExecutor const& other) = delete;
    /// @brief delete copy asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10977
    /// @needwork = dda
    /// @endcode
    ExceptionRollbackExecutor& operator=(ExceptionRollbackExecutor const& other) = delete;
    /// @brief delete move construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10978
    /// @needwork = dda
    /// @endcode
    ExceptionRollbackExecutor(ExceptionRollbackExecutor&& other) = delete;
    /// @brief delete move asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10979
    /// @needwork = dda
    /// @endcode
    ExceptionRollbackExecutor& operator=(ExceptionRollbackExecutor&& other) = delete;

    /// @brief Clean up the temporary files
    ///
    /// TODO: Implementation is missing.
    /// @return future with result
    /// @code{.isoft}
    /// @uptrace={SWS_UCM_00158}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10980
    /// @needwork = dda
    /// @endcode
    AraFutureVoid RunInThread() const noexcept;

private:
    /// @brief Clean up the temporary files
    /// @throws no
    /// @return result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10981
    /// @needwork = dda
    /// @endcode
    AraResultVoid _doExceptionRollingBack() const noexcept;
};

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // PACKAGE_MANAGER_EXECUTOR_OF_EXCEPTION_ROLLBACK_H
