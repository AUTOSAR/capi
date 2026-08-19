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
/// @file       activated_state.h
/// @brief      Activated state class
/// @details
/// @date       2024-01-01
/// @author     cuiyinli
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @export_level=/UCM/FsmManager
/// @module_path=/UCM/FsmManager
/// @interface_level=unit
/// @trace_id_sr=SR_UCM_00001,SR_UCM_00021
/// @unit_name=ActivatedState
/// @unit_description=Handles the activated state of the package manager.
/// @endcode
///
/// ================================================================

#ifndef ARA_UCM_PKGMGR_FSM_ACTIVATED_STATE_H_
#define ARA_UCM_PKGMGR_FSM_ACTIVATED_STATE_H_

#include "package_manager_state.h"

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @brief Class representing Activated state of Package Manager
///
/// @code{.isoft}
/// @uptrace={SWS_UCM_00126, f071c00af3554920e1527d31308542ae480c746f}
/// @uptrace={SWS_UCM_00146, 3c7eaa9e7432b384fa080815fd0449311fb1c74c}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10167
/// @trace_id_dd=DD_UCM_10240
/// @needwork = ad
/// @endcode
class ActivatedState final : public PackageManagerState
{
public:
    /// @brief Creates a new instance of Activated state and saves the current state
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10168
    /// @trace_id_dd=DD_UCM_10241
    /// @needwork = ad
    /// @endcode
    ActivatedState() = default;
    /// @brief destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10169
    /// @trace_id_dd=DD_UCM_10242
    /// @needwork = ad
    /// @endcode
    ~ActivatedState() final = default;

    /// @brief delete copy construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10170
    /// @trace_id_dd=DD_UCM_10243
    /// @needwork = ad
    /// @endcode
    ActivatedState(ActivatedState const& other) = delete;
    /// @brief delete copy asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10171
    /// @trace_id_dd=DD_UCM_10244
    /// @needwork = ad
    /// @endcode
    ActivatedState& operator=(ActivatedState const& other) = delete;
    /// @brief delete move construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10172
    /// @trace_id_dd=DD_UCM_10245
    /// @needwork = ad
    /// @endcode
    ActivatedState(ActivatedState&& other) = delete;
    /// @brief delete move asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10173
    /// @trace_id_dd=DD_UCM_10246
    /// @needwork = ad
    /// @endcode
    ActivatedState& operator=(ActivatedState&& other) = delete;

    /// @brief Retrieve the Status which corresponds to the state.
    ///
    /// @return The corresponding status.
    /// @throws no
    /// @code{.isoft}
    /// <inheritdoc>
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10174
    /// @trace_id_dd=DD_UCM_10247
    /// @needwork = ad
    /// @endcode
    PackageManagerStatusType GetStatus() const noexcept final { return PackageManagerStatusType::kActivated; }

    /// @brief Finish
    /// @return future with result
    /// @throws no
    /// @code{.isoft}
    /// <inheritdoc>
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10175
    /// @trace_id_dd=DD_UCM_10248
    /// @needwork = ad
    /// @endcode
    AraFutureVoid Finish() final;

    /// @brief Rollback
    /// @return future with result
    /// @throws no
    /// @code{.isoft}
    /// <inheritdoc>
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10176
    /// @trace_id_dd=DD_UCM_10249
    /// @needwork = ad
    /// @endcode
    AraFutureVoid Rollback() final;
};

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // ARA_UCM_PKGMGR_FSM_ACTIVATED_STATE_H_
