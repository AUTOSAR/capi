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
/// @file       rollingback_state.h
/// @brief      RollingBack state class
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
/// @unit_name=RollingBackState
/// @unit_description=RollingBack state class
/// @endcode
///
/// ================================================================

#ifndef ARA_UCM_PKGMGR_FSM_ROLLINGBACK_STATE_H_
#define ARA_UCM_PKGMGR_FSM_ROLLINGBACK_STATE_H_

#include "package_manager_state.h"

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @brief Class representing Rollling-Back state of Package Manager
/// TODO: Requirement needs check of SM field AR-91935
/// @code{.isoft}
/// @uptrace={SWS_UCM_00111}
/// TODO: Currently the switch is done unconditionally AR-91935
/// @uptrace={SWS_UCM_00096}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10158
/// @trace_id_dd=DD_UCM_10231
/// @needwork = ad
/// @endcode
class RollingBackState final : public PackageManagerState
{
public:
    /// @brief Creates a new instance of RollingBack state and saves the current state
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10159
    /// @trace_id_dd=DD_UCM_10232
    /// @needwork = ad
    /// @endcode
    RollingBackState() = default;
    /// @brief destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10160
    /// @trace_id_dd=DD_UCM_10233
    /// @needwork = ad
    /// @endcode
    ~RollingBackState() final = default;

    /// @brief delete copy construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10161
    /// @trace_id_dd=DD_UCM_10234
    /// @needwork = ad
    /// @endcode
    RollingBackState(RollingBackState const& other) = delete;
    /// @brief delete copy asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10162
    /// @trace_id_dd=DD_UCM_10235
    /// @needwork = ad
    /// @endcode
    RollingBackState& operator=(RollingBackState const& other) = delete;
    /// @brief delete move construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10163
    /// @trace_id_dd=DD_UCM_10236
    /// @needwork = ad
    /// @endcode
    RollingBackState(RollingBackState&& other) = delete;
    /// @brief delete move asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10164
    /// @trace_id_dd=DD_UCM_10237
    /// @needwork = ad
    /// @endcode
    RollingBackState& operator=(RollingBackState&& other) = delete;

    /// @brief Retrieve the Status which corresponds to the state.
    ///
    /// @return The corresponding status.
    /// @throws no
    /// @code{.isoft}
    /// <inheritdoc>
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10165
    /// @trace_id_dd=DD_UCM_10238
    /// @needwork = ad
    /// @endcode
    PackageManagerStatusType GetStatus() const noexcept final { return PackageManagerStatusType::kRollingBack; }

    /// @brief Done
    /// @throws no
    /// @code{.isoft}
    /// <inheritdoc>
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10166
    /// @trace_id_dd=DD_UCM_10239
    /// @needwork = ad
    /// @endcode
    void Done() noexcept final;
};

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // ARA_UCM_PKGMGR_FSM_ROLLINGBACK_STATE_H_
