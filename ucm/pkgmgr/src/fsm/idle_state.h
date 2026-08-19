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
/// @file       idle_state.h
/// @brief      Idle state class
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
/// @unit_name=IdleState
/// @unit_description=Idle state class
/// @endcode
///
/// ================================================================

#ifndef ARA_UCM_PKGMGR_FSM_IDLE_STATE_H_
#define ARA_UCM_PKGMGR_FSM_IDLE_STATE_H_

#include "package_manager_state.h"

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @brief Class representing Idle (default) state of Package Manager
///
/// @code{.isoft}
/// @uptrace={SWS_UCM_00007, 1f06ff0dbb62befd08052e7695c09b0f3a05cbbf}
/// @uptrace={SWS_UCM_00081, e728a83c3a7ec27687eccd6704c3f8e7d013f17f}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10149
/// @trace_id_dd=DD_UCM_10222
/// @needwork = ad
/// @endcode
class IdleState final : public PackageManagerState
{
public:
    /// @brief Creates a new instance of Idle state and saves the current state
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10150
    /// @trace_id_dd=DD_UCM_10223
    /// @needwork = ad
    /// @endcode
    IdleState() = default;
    /// @brief destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10151
    /// @trace_id_dd=DD_UCM_10224
    /// @needwork = ad
    /// @endcode
    ~IdleState() final = default;

    /// @brief delete copy construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10152
    /// @trace_id_dd=DD_UCM_10225
    /// @needwork = ad
    /// @endcode
    IdleState(IdleState const& other) = delete;
    /// @brief delete copy asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10153
    /// @trace_id_dd=DD_UCM_10226
    /// @needwork = ad
    /// @endcode
    IdleState& operator=(IdleState const& other) = delete;
    /// @brief delete move construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10154
    /// @trace_id_dd=DD_UCM_10227
    /// @needwork = ad
    /// @endcode
    IdleState(IdleState&& other) = delete;
    /// @brief delete move asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10155
    /// @trace_id_dd=DD_UCM_10228
    /// @needwork = ad
    /// @endcode
    IdleState& operator=(IdleState&& other) = delete;

    /// @brief Retrieve the Status which corresponds to the state.
    ///
    /// @return The corresponding status.
    /// @throws no
    /// @code{.isoft}
    /// <inheritdoc>
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10156
    /// @trace_id_dd=DD_UCM_10229
    /// @needwork = ad
    /// @endcode
    PackageManagerStatusType GetStatus() const noexcept final { return PackageManagerStatusType::kIdle; }

    /// @brief ProcessSwPackage
    /// @param id TransferIdType
    /// @return future with result
    /// @throws no
    /// @code{.isoft}
    /// <inheritdoc>
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10157
    /// @trace_id_dd=DD_UCM_10230
    /// @needwork = ad
    /// @endcode
    AraFutureVoid ProcessSwPackage(TransferIdType const& id) noexcept final;
};

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // ARA_UCM_PKGMGR_FSM_IDLE_STATE_H_
