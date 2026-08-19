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
/// @file       cleanup_executor.h
/// @brief      Cleanup executor
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
/// @unit_name=CleanupExecutor
/// @unit_description=Cleanup executor
/// @endcode
///
/// ================================================================

#ifndef PACKAGE_MANAGER_EXECUTOR_OF_CLEANUP_H
#define PACKAGE_MANAGER_EXECUTOR_OF_CLEANUP_H

#include "base_executor.h"
#include "common/alias.h"

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @brief CleanupExecutor
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10396
/// @trace_id_dd=DD_UCM_10982
/// @needwork = ad
/// @endcode
class CleanupExecutor : public BaseExecutor
{
public:
    /// @brief constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10983
    /// @needwork = dda
    /// @endcode
    CleanupExecutor() = default;
    /// @brief destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10984
    /// @needwork = dda
    /// @endcode
    ~CleanupExecutor() override = default;

    /// @brief delete copy construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10985
    /// @needwork = dda
    /// @endcode
    CleanupExecutor(CleanupExecutor const& other) = delete;
    /// @brief delete copy asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10986
    /// @needwork = dda
    /// @endcode
    CleanupExecutor& operator=(CleanupExecutor const& other) = delete;
    /// @brief delete move construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10987
    /// @needwork = dda
    /// @endcode
    CleanupExecutor(CleanupExecutor&& other) = delete;
    /// @brief delete move asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10988
    /// @needwork = dda
    /// @endcode
    CleanupExecutor& operator=(CleanupExecutor&& other) = delete;

    /// @brief Clean up the temporary files
    ///
    /// TODO: Implementation is missing.
    /// @return future with result
    /// @code{.isoft}
    /// @uptrace={SWS_UCM_00158}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10989
    /// @needwork = dda
    /// @endcode
    AraFutureVoid RunInThread() const noexcept;

    /// @brief Clean up the temporary files
    /// @return result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10990
    /// @needwork = dda
    /// @endcode
    AraResultVoid RunInLocal() const noexcept;

private:
    /// @brief Clean up the temporary files
    /// @return result
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10991
    /// @needwork = dda
    /// @endcode
    AraResultVoid _cleanup() const noexcept;
    /// @brief save actions
    /// @param actionsInfo
    /// @param success
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10992
    /// @needwork = dda
    /// @endcode
    static void Save2History(AraList< GetHistoryType >& actionsInfo, bool const success) noexcept;
};

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // PACKAGE_MANAGER_EXECUTOR_OF_CLEANUP_H
