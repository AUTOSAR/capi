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
/// @file       rolledback_state.h
/// @brief      RolledBack state class
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
/// @unit_name=RolledBackState
/// @unit_description=RolledBack state class
/// @endcode
///
/// ================================================================

#ifndef ARA_UCM_PKGMGR_FSM_ROLLEDBACK_STATE_H_
#define ARA_UCM_PKGMGR_FSM_ROLLEDBACK_STATE_H_

#include "package_manager_state.h"

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @brief Class representing RolledBack state of Package Manager
///
/// @code{.isoft}
/// @uptrace={SWS_UCM_00146, 3c7eaa9e7432b384fa080815fd0449311fb1c74c}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10140
/// @trace_id_dd=DD_UCM_10213
/// @needwork = ad
/// @endcode
class RolledBackState final : public PackageManagerState
{
public:
    /// @brief Creates a new instance of RolledBack state and saves the current state
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10141
    /// @trace_id_dd=DD_UCM_10214
    /// @needwork = ad
    /// @endcode
    RolledBackState() = default;
    /// @brief destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10142
    /// @trace_id_dd=DD_UCM_10215
    /// @needwork = ad
    /// @endcode
    ~RolledBackState() final = default;

    /// @brief delete copy construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10143
    /// @trace_id_dd=DD_UCM_10216
    /// @needwork = ad
    /// @endcode
    RolledBackState(RolledBackState const& other) = delete;
    /// @brief delete copy asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10144
    /// @trace_id_dd=DD_UCM_10217
    /// @needwork = ad
    /// @endcode
    RolledBackState& operator=(RolledBackState const& other) = delete;
    /// @brief delete move construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10145
    /// @trace_id_dd=DD_UCM_10218
    /// @needwork = ad
    /// @endcode
    RolledBackState(RolledBackState&& other) = delete;
    /// @brief delete move asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10146
    /// @trace_id_dd=DD_UCM_10219
    /// @needwork = ad
    /// @endcode
    RolledBackState& operator=(RolledBackState&& other) = delete;

    /// @brief Retrieve the Status which corresponds to the state.
    ///
    /// @return The corresponding status.
    /// @throws no
    /// @code{.isoft}
    /// <inheritdoc>
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10147
    /// @trace_id_dd=DD_UCM_10220
    /// @needwork = ad
    /// @endcode
    PackageManagerStatusType GetStatus() const noexcept final { return PackageManagerStatusType::kRolledBack; }

    /// @brief Finish
    /// @return future with result
    /// @throws no
    /// @code{.isoft}
    /// <inheritdoc>
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10148
    /// @trace_id_dd=DD_UCM_10221
    /// @needwork = ad
    /// @endcode
    AraFutureVoid Finish() noexcept final;
};

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // ARA_UCM_PKGMGR_FSM_ROLLEDBACK_STATE_H_
