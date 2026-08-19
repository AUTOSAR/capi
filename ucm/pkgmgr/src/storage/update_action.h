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
/// @file       update_action.h
/// @brief      update action
/// @details
/// @date       2022-01-01
/// @author     cuiyinli
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/UCM/SoftwareClusterManager
/// @interface_level=unit
/// @trace_id_sr=SR_UCM_00001
/// @unit_name=UpdateAction
/// @unit_description=Action for update of a software package
/// @endcode
///
/// ================================================================

#ifndef ARA_UCM_PKGMGR_STORAGE_UPDATE_ACTION_H_
#define ARA_UCM_PKGMGR_STORAGE_UPDATE_ACTION_H_

#include "reversible_action.h"

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @brief Action for update of a software package.
///
/// @code{.isoft}
/// @uptrace={SWS_UCM_00001, a665954b7a62b2349db109a9408efe084b8a7136}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10268
/// @trace_id_dd=DD_UCM_10561
/// @needwork = ad
/// @endcode
class UpdateAction : public ReversibleAction
{
    using ReversibleAction::ReversibleAction;

public:
    /// @brief destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10562
    /// @needwork = dda
    /// @endcode
    ~UpdateAction() override = default;

    /// @brief delete copy construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10563
    /// @needwork = dda
    /// @endcode
    UpdateAction(UpdateAction const& other) = delete;
    /// @brief delete copy asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10564
    /// @needwork = dda
    /// @endcode
    UpdateAction& operator=(UpdateAction const& other) = delete;
    /// @brief delete move construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10565
    /// @needwork = dda
    /// @endcode
    UpdateAction(UpdateAction&& other) = delete;
    /// @brief delete move asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10566
    /// @needwork = dda
    /// @endcode
    UpdateAction& operator=(UpdateAction&& other) = delete;

    /// @brief Executes the update of a software package.
    ///
    /// The content of the new version of software package @p package is temporarily installed in a directory until
    /// activation If execution fails, the directory of this new version is removed and the directory of the previous
    /// version is kept
    ///
    /// @return result
    /// @throws no
    /// @code{.isoft}
    /// @copydetails ReversibleAction::Execute()
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10567
    /// @needwork = dda
    /// @endcode
    AraResult< ResultType > Execute() override;

    /// @brief Revert changes in installation directory created by the action.
    ///
    /// The updated version of the previously installed software cluster is removed.
    ///
    /// @return result
    /// @throws no
    /// @code{.isoft}
    /// @copydetails ReversibleAction::RevertChanges()
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10568
    /// @needwork = dda
    /// @endcode
    AraResult< ResultType > RevertChanges() override;

    /// @brief Commit changes in installation directory created by the action.
    ///
    /// The new version of the previously installed software cluster is persisted and its old version is removed
    ///
    /// @return result
    /// @throws no
    /// @code{.isoft}
    /// @copydetails ReversibleAction::CommitChanges()
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10569
    /// @needwork = dda
    /// @endcode
    AraResult< ResultType > CommitChanges() override;

    /// @brief Merge var directory as to the action.
    ///
    /// The new version of the var directory is merged to its old version.
    ///
    /// @return result
    /// @throws no
    /// @code{.isoft}
    /// @copydetails ReversibleAction::MergeVar()
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10570
    /// @needwork = dda
    /// @endcode
    AraResult< ResultType > MergeVar() override;

    /// @brief GetActionType
    /// @return ActionType
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10571
    /// @needwork = dda
    /// @endcode
    ActionType GetActionType() noexcept override { return ActionType::kUpdate; }

    // Place the new version software set into its target installation directory (including the var directory)
    /// @brief PlaceToDestinationPath
    /// @return result
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10572
    /// @needwork = dda
    /// @endcode
    virtual AraResultVoid PlaceToDestinationPath();

    // Install to OS partition
    /// @brief PlaceToOSPatition
    /// @return result
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10573
    /// @needwork = dda
    /// @endcode
    virtual AraResultVoid PlaceToOSPatition() noexcept;

    // Invalidate the OS partition of the specified version
    /// @brief InvalidOSPatition
    /// @param oSVersion
    /// @return result
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10574
    /// @needwork = dda
    /// @endcode
    virtual AraResultVoid InvalidOSPatition(AraString const& oSVersion) noexcept;
};

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // ARA_UCM_PKGMGR_STORAGE_UPDATE_ACTION_H_
