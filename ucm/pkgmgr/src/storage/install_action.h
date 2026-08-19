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
/// @file       install_action.h
/// @brief      install action
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
/// @trace_id_sr=SR_UCM_00001
/// @unit_name=InstallAction
/// @unit_description=Action for installation of a software package
/// @endcode
///
/// ================================================================

#ifndef ARA_UCM_PKGMGR_STORAGE_INSTALL_ACTION_H_
#define ARA_UCM_PKGMGR_STORAGE_INSTALL_ACTION_H_

#include "reversible_action.h"

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @brief Action for installation of a software package.
///
/// @code{.isoft}
/// @uptrace={SWS_UCM_00001, a665954b7a62b2349db109a9408efe084b8a7136}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10267
/// @trace_id_dd=DD_UCM_10551
/// @needwork = ad
/// @endcode
class InstallAction : public ReversibleAction
{
    using ReversibleAction::ReversibleAction;

public:
    /// @brief destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10552
    /// @needwork = dda
    /// @endcode
    ~InstallAction() override = default;

    /// @brief delete copy construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10553
    /// @needwork = dda
    /// @endcode
    InstallAction(InstallAction const& other) = delete;
    /// @brief delete copy asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10554
    /// @needwork = dda
    /// @endcode
    InstallAction& operator=(InstallAction const& other) = delete;
    /// @brief delete move construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10555
    /// @needwork = dda
    /// @endcode
    InstallAction(InstallAction&& other) = delete;
    /// @brief delete move asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10556
    /// @needwork = dda
    /// @endcode
    InstallAction& operator=(InstallAction&& other) = delete;

    /// @brief Executes the installation of a software package.
    ///
    /// The content of the software package @p package is temporarily installed in a directory
    /// until activation. If execution fails, no files are installed.
    ///
    /// @return result with ResultType
    /// @throws no
    /// @code{.isoft}
    /// @copydetails ReversibleAction::Execute()
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10557
    /// @needwork = dda
    /// @endcode
    AraResult< ResultType > Execute() override;

    /// @brief Revert changes in installation directory created by the action.
    ///
    /// The installed software cluster is removed again.
    ///
    /// @return result with ResultType
    /// @throws no
    /// @code{.isoft}
    /// @copydetails ReversibleAction::RevertChanges()
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10558
    /// @needwork = dda
    /// @endcode
    AraResult< ResultType > RevertChanges() override;

    /// @brief Commit changes in installation directory created by the action.
    ///
    /// The installed software cluster is persisted.
    ///
    /// @return result with ResultType
    /// @throws no
    /// @code{.isoft}
    /// @copydetails ReversibleAction::CommitChanges()
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10559
    /// @needwork = dda
    /// @endcode
    AraResult< ResultType > CommitChanges() override;

    /// @brief GetActionType
    /// @return ActionType
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10560
    /// @needwork = dda
    /// @endcode
    ActionType GetActionType() noexcept override { return ActionType::kInstall; }
};

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // ARA_UCM_PKGMGR_STORAGE_INSTALL_ACTION_H_
