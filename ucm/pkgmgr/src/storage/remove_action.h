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
/// @file       remove_action.h
/// @brief      Remove action
/// @details
/// @date       2024-08-15
/// @author     cuiyinli
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @export_level=/UCM/SoftwareClusterManager
/// @module_path=/UCM/SoftwareClusterManager
/// @interface_level=unit
/// @trace_id_sr=SR_UCM_00001,SR_UCM_00004
/// @unit_name=RemoveAction
/// @unit_description=Remove action
/// @endcode
///
/// ================================================================

#ifndef ARA_UCM_PKGMGR_STORAGE_REMOVE_ACTION_H_
#define ARA_UCM_PKGMGR_STORAGE_REMOVE_ACTION_H_

#include "reversible_action.h"

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @brief uninstall the installation of the software package.
/// Since also the persistency kvs is removed together with the binary
/// this implementation implicitely fulfills SWS_UCM_00184
/// @code{.isoft}
/// @uptrace={SWS_UCM_00184, c9bea65f2d2d072d5ce159060c934e322b07b75e}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10263
/// @trace_id_dd=DD_UCM_10481
/// @needwork = ad
/// @endcode
class RemoveAction : public ReversibleAction
{
    using ReversibleAction::ReversibleAction;

public:
    /// @brief destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10482
    /// @needwork = dda
    /// @endcode
    ~RemoveAction() override = default;

    /// @brief delete copy construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10483
    /// @needwork = dda
    /// @endcode
    RemoveAction(RemoveAction const& other) = delete;
    /// @brief delete copy asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10484
    /// @needwork = dda
    /// @endcode
    RemoveAction& operator=(RemoveAction const& other) = delete;
    /// @brief delete move construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10485
    /// @needwork = dda
    /// @endcode
    RemoveAction(RemoveAction&& other) = delete;
    /// @brief delete move asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10486
    /// @needwork = dda
    /// @endcode
    RemoveAction& operator=(RemoveAction&& other) = delete;

    /// @brief inherit SWCLReversibleAction virtual Execute function for removing action.
    /// @return result
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10487
    /// @needwork = dda
    /// @endcode
    AraResult< ResultType > Execute() override;

    /// @brief Revert removal changes.
    ///
    /// The software cluster is kept.
    ///
    /// @return result
    /// @throws no
    /// @code{.isoft}
    /// @copydetails ReversibleAction::RevertChanges()
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10488
    /// @needwork = dda
    /// @endcode
    AraResult< ResultType > RevertChanges() override;

    /// @brief Commit removal changes.
    ///
    /// The installed software cluster is finally removed.
    ///
    /// @return result
    /// @throws no
    /// @code{.isoft}
    /// @copydetails ReversibleAction::CommitChanges()
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10489
    /// @needwork = dda
    /// @endcode
    AraResult< ResultType > CommitChanges() override;

    /// @brief GetActionType
    /// @return ActionType
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10490
    /// @needwork = dda
    /// @endcode
    ActionType GetActionType() noexcept override { return ActionType::kRemove; }

private:
    /// @brief Clean up persistent data
    /// @brief GetActionType
    /// @return result
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10491
    /// @needwork = dda
    /// @endcode
    AraResult< ResultType > _cleanUpPersistData() const;
};

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // ARA_UCM_PKGMGR_STORAGE_REMOVE_ACTION_H_
