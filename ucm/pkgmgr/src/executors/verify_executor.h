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
/// @file       verify_executor.h
/// @brief      VerifyExecutor
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
/// @trace_id_sr=SR_UCM_00001,SR_UCM_00021,SR_UCM_00030
/// @unit_name=VerifyExecutor
/// @unit_description=VerifyExecutor
/// @endcode
///
/// ================================================================

#ifndef PACKAGE_MANAGER_EXECUTOR_OF_VERIFY_H
#define PACKAGE_MANAGER_EXECUTOR_OF_VERIFY_H

#include "base_executor.h"
#include "common/alias.h"

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @brief VerifyExecutor
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10390
/// @trace_id_dd=DD_UCM_10917
/// @needwork = ad
/// @endcode
class VerifyExecutor : public BaseExecutor
{
public:
    /// @brief constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10918
    /// @needwork = dda
    /// @endcode
    VerifyExecutor() = default;
    /// @brief destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10919
    /// @needwork = dda
    /// @endcode
    ~VerifyExecutor() override = default;

    /// @brief delete copy construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10920
    /// @needwork = dda
    /// @endcode
    VerifyExecutor(VerifyExecutor const& other) = delete;
    /// @brief delete copy asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10921
    /// @needwork = dda
    /// @endcode
    VerifyExecutor& operator=(VerifyExecutor const& other) = delete;
    /// @brief delete move construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10922
    /// @needwork = dda
    /// @endcode
    VerifyExecutor(VerifyExecutor&& other) = delete;
    /// @brief delete move asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10923
    /// @needwork = dda
    /// @endcode
    VerifyExecutor& operator=(VerifyExecutor&& other) = delete;

    /// @brief Perform the verification
    ///
    /// @return result
    /// @code{.isoft}
    /// @uptrace={SWS_UCM_00154, 8bf9580f59d19108febf06a764bb10a48224873d}
    /// @uptrace={SWS_UCM_00155, c1d53900d0df783edff9a9acf0669e46fca4e67e}
    /// @uptrace={SWS_UCM_00085, 8e3bf330c39d9e3efbebecfa60b2c1282e0528d0}
    /// @uptrace={SWS_UCM_00107, fe5a90cd6b652044e515c9bf808ac2bb27f67001}
    /// @uptrace={SWS_UCM_00293, 22b4a514945ea63c7c2bfb9dbf9d5c8469336825}
    /// @uptrace={SWS_UCM_00264}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10924
    /// @needwork = dda
    /// @endcode
    AraResultVoid RunInLocal() const noexcept;

    /// @brief do something after restart
    /// @throws no
    /// @return result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10925
    /// @needwork = dda
    /// @endcode
    void RunInThreadAfterReboot() noexcept;

private:
    /// @brief _verify
    /// @throws no
    /// @return result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10926
    /// @needwork = dda
    /// @endcode
    AraResultVoid _verify() const noexcept;
    /// @brief Verify Before Restart
    /// @throws no
    /// @return result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10927
    /// @needwork = dda
    /// @endcode
    AraResultVoid _verifyBeforeRestart() const noexcept;
    /// @brief Verify On Restart
    /// @throws no
    /// @return result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10928
    /// @needwork = dda
    /// @endcode
    AraResultVoid _verifyOnRestart() const noexcept;
    /// @brief Verify After Restart
    /// @throws no
    /// @return result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10929
    /// @needwork = dda
    /// @endcode
    AraResultVoid _verifyAfterRestart() const noexcept;
};

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // PACKAGE_MANAGER_EXECUTOR_OF_VERIFY_H
