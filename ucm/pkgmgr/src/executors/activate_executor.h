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
/// @file       activate_executor.h
/// @brief      Activate executor
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
/// @trace_id_sr=SR_UCM_00001,SR_UCM_00021,SR_UCM_00007
/// @unit_name=ActivateExecutor
/// @unit_description=Activate executor
/// @endcode
///
/// ================================================================

#ifndef ARA_UCM_PKGMGR_EXECUTORS_ACTIVATE_EXECUTOR_H_
#define ARA_UCM_PKGMGR_EXECUTORS_ACTIVATE_EXECUTOR_H_

#include "base_executor.h"
#include "common/alias.h"

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @brief Activate Executor
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10392
/// @trace_id_dd=DD_UCM_10940
/// @needwork = ad
/// @endcode
class ActivateExecutor : public BaseExecutor
{
public:
    /// @brief constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10941
    /// @needwork = dda
    /// @endcode
    ActivateExecutor() = default;
    /// @brief destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10942
    /// @needwork = dda
    /// @endcode
    ~ActivateExecutor() override = default;

    /// @brief delete copy construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10943
    /// @needwork = dda
    /// @endcode
    ActivateExecutor(ActivateExecutor const& other) = delete;
    /// @brief delete copy asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10944
    /// @needwork = dda
    /// @endcode
    ActivateExecutor& operator=(ActivateExecutor const& other) = delete;
    /// @brief delete move construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10945
    /// @needwork = dda
    /// @endcode
    ActivateExecutor(ActivateExecutor&& other) = delete;
    /// @brief delete move asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10946
    /// @needwork = dda
    /// @endcode
    ActivateExecutor& operator=(ActivateExecutor&& other) = delete;

    /// @brief Perform the activation
    ///
    /// TODO: Improve the dependency checking?
    /// @return the future with result
    /// @code{.isoft}
    /// @uptrace={SWS_UCM_00026, 243b4ac88c72a2c4fc6f59c3f46bf819f1031e19}
    /// @uptrace={SWS_UCM_00022, beddf391910d370d80df8b7a0fdbdddd0603ea4c}
    /// @uptrace={SWS_UCM_00257, faa76897e930a2d9036f15d0e55e16680981e251}
    /// @uptrace={SWS_UCM_00258, 0b014f2e30b4e6d1676636cea5d1a50bdd4bcc68}
    /// @uptrace={SWS_UCM_00263, 6840cb06728ed67a711786023546bc8b4865f1a2}
    /// @uptrace={SWS_UCM_00242, 73214f4c5fcf508c981481a552fccc1019e95708}
    /// @uptrace={SWS_UCM_00280, 4c40e6478ca1a8cab042b2e6d59b0393cf393134}
    /// TODO: Implement retry strategies AR-115917
    /// @uptrace={SWS_VUCM_00298}
    /// @uptrace={SWS_UCM_00260}
    /// @uptrace={SWS_UCM_00262}
    /// @uptrace={SWS_UCM_00281, 3db46661bca746546729e8a74d18394158ec6cad}
    /// @uptrace={SWS_UCM_00286, 46712a7aa117960870d28e78cb635ba4df4a79b2}
    /// @uptrace={SWS_UCM_00287, db3dec35dc8887fb77a6742e73d2a8a4031b9c86}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10947
    /// @needwork = dda
    /// @endcode
    AraFutureVoid RunInThread() noexcept;

    /// @brief Perform the activation
    /// @return result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10948
    /// @needwork = dda
    /// @endcode
    AraResultVoid Activate() const noexcept;

private:
    /// @brief check dependency
    /// @throws no
    /// @return bool
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10949
    /// @trace_id_sr=SR_UCM_00007
    /// @needwork = dda
    /// @endcode
    bool _dependencyCheck() const;
};

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // ARA_UCM_PKGMGR_EXECUTORS_ACTIVATE_EXECUTOR_H_
