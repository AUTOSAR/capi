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
/// @file       ready_state.h
/// @brief      Ready state class
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
/// @unit_name=ReadyState
/// @unit_description=Ready state class
/// @endcode
///
/// ================================================================

#ifndef ARA_UCM_PKGMGR_FSM_READY_STATE_H_
#define ARA_UCM_PKGMGR_FSM_READY_STATE_H_

#include "package_manager_state.h"

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @brief Class representing Ready (default) state of Package Manager
///
/// @code{.isoft}
/// @uptrace={SWS_UCM_00007, c9e41bf68059e0421604c79e27feced11f961119}
/// @uptrace={SWS_UCM_00084, c7a1b66e5cc2a40f5eee7817ba504f88b0ffddd7}
/// @uptrace={SWS_UCM_00081, 234d2e8b38025c88dc81199da6049d52118d87d0}
/// @uptrace={SWS_UCM_00162, cf6d237816e34395ef3d3801d5734a6ceeafeec3}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10177
/// @trace_id_dd=DD_UCM_10250
/// @needwork = ad
/// @endcode
class ReadyState final : public PackageManagerState
{
public:
    /// @brief Creates a new instance of Ready state and saves the current state
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10178
    /// @trace_id_dd=DD_UCM_10251
    /// @needwork = ad
    /// @endcode
    ReadyState() = default;
    /// @brief destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10179
    /// @trace_id_dd=DD_UCM_10252
    /// @needwork = ad
    /// @endcode
    ~ReadyState() final = default;

    /// @brief delete copy construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10180
    /// @trace_id_dd=DD_UCM_10253
    /// @needwork = ad
    /// @endcode
    ReadyState(ReadyState const& other) = delete;
    /// @brief delete copy asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10181
    /// @trace_id_dd=DD_UCM_10254
    /// @needwork = ad
    /// @endcode
    ReadyState& operator=(ReadyState const& other) = delete;
    /// @brief delete move construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10182
    /// @trace_id_dd=DD_UCM_10255
    /// @needwork = ad
    /// @endcode
    ReadyState(ReadyState&& other) = delete;
    /// @brief delete move asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10183
    /// @trace_id_dd=DD_UCM_10256
    /// @needwork = ad
    /// @endcode
    ReadyState& operator=(ReadyState&& other) = delete;

    /// @brief Retrieve the Status which corresponds to the state.
    ///
    /// @return The corresponding status.
    /// @throws no
    /// @code{.isoft}
    /// <inheritdoc>
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10184
    /// @trace_id_dd=DD_UCM_10257
    /// @needwork = ad
    /// @endcode
    PackageManagerStatusType GetStatus() const noexcept final { return PackageManagerStatusType::kReady; }

    /// @brief ProcessSwPackage
    /// @param id TransferIdType
    /// @return future with result
    /// @throws no
    /// @code{.isoft}
    /// <inheritdoc>
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10185
    /// @trace_id_dd=DD_UCM_10258
    /// @needwork = ad
    /// @endcode
    AraFutureVoid ProcessSwPackage(TransferIdType const& id) final;

    /// @brief Activate
    /// @return future with result
    /// @throws no
    /// @code{.isoft}
    /// <inheritdoc>
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10186
    /// @trace_id_dd=DD_UCM_10259
    /// @needwork = ad
    /// @endcode
    AraFutureVoid Activate() final;

    /// @brief RevertProcessedSwPackages
    /// @return future with result
    /// @throws no
    /// @code{.isoft}
    /// <inheritdoc>
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10187
    /// @trace_id_dd=DD_UCM_10260
    /// @needwork = ad
    /// @endcode
    AraFutureVoid RevertProcessedSwPackages() final;
};

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // ARA_UCM_PKGMGR_FSM_READY_STATE_H_
