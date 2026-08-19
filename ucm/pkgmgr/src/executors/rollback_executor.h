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
/// @file       rollback_executor.h
/// @brief      RollbackExecutor
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
/// @trace_id_sr=SR_UCM_00001,SR_UCM_00008
/// @unit_name=RollbackExecutor
/// @unit_description=RollbackExecutor
/// @endcode
///
/// ================================================================

#ifndef PACKAGE_MANAGER_EXECUTOR_OF_ROLLBACK_H
#define PACKAGE_MANAGER_EXECUTOR_OF_ROLLBACK_H

#include "base_executor.h"
#include "common/alias.h"

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @brief RollbackExecutor
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10391
/// @trace_id_dd=DD_UCM_10930
/// @needwork = ad
/// @endcode
class RollbackExecutor : public BaseExecutor
{
public:
    /// @brief constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10931
    /// @needwork = dda
    /// @endcode
    RollbackExecutor() = default;
    /// @brief destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10932
    /// @needwork = dda
    /// @endcode
    ~RollbackExecutor() override = default;

    /// @brief delete copy construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10933
    /// @needwork = dda
    /// @endcode
    RollbackExecutor(RollbackExecutor const& other) = delete;
    /// @brief delete copy asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10934
    /// @needwork = dda
    /// @endcode
    RollbackExecutor& operator=(RollbackExecutor const& other) = delete;
    /// @brief delete move construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10935
    /// @needwork = dda
    /// @endcode
    RollbackExecutor(RollbackExecutor&& other) = delete;
    /// @brief delete move asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10936
    /// @needwork = dda
    /// @endcode
    RollbackExecutor& operator=(RollbackExecutor&& other) = delete;

    /// @brief Perform the rollback
    ///
    /// @return future with result
    /// @code{.isoft}
    /// @uptrace={SWS_UCM_00005, b65b510e652e67989fcbe115b5e8b5f1895ec741}
    /// @uptrace={SWS_UCM_00110, 0b6f5384fadbe10dab26fb6075881d6fd012a470}
    /// TODO: todo
    /// @uptrace={SWS_UCM_00299}
    /// @uptrace={SWS_UCM_00300, 0786f697dcd71020e24a6b5ddcc03b52f009927b}
    /// TODO: todo
    /// @uptrace={SWS_UCM_00302}
    /// @uptrace={SWS_UCM_00286, 46712a7aa117960870d28e78cb635ba4df4a79b2}
    /// @uptrace={SWS_UCM_00287, db3dec35dc8887fb77a6742e73d2a8a4031b9c86}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10937
    /// @needwork = dda
    /// @endcode
    AraFutureVoid RunInThread() noexcept;
    /// @brief rollback in local
    /// @throws no
    /// @return result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10938
    /// @needwork = dda
    /// @endcode
    AraResultVoid RunInLocal() const noexcept;

private:
    /// @brief _rollback
    /// @throws no
    /// @return result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10939
    /// @needwork = dda
    /// @endcode
    AraResultVoid _rollback() const noexcept;
};

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // PACKAGE_MANAGER_EXECUTOR_OF_ROLLBACK_H
