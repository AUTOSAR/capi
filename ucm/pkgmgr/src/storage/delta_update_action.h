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
/// @file       delta_update_action.h
/// @brief      delta update action
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
/// @unit_name=DeltaUpdateAction
/// @unit_description=Action for delta update of a software package
/// @endcode
///
/// ================================================================

#ifndef ARA_UCM_PKGMGR_STORAGE_DELTA_UPDATE_ACTION_H_
#define ARA_UCM_PKGMGR_STORAGE_DELTA_UPDATE_ACTION_H_

#include "update_action.h"

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @brief Action for delta update of a software package.
///
/// @code{.isoft}
/// @uptrace={SWS_UCM_00001, a665954b7a62b2349db109a9408efe084b8a7136}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10312
/// @trace_id_dd=DD_UCM_10638
/// @needwork = ad
/// @endcode
class DeltaUpdateAction : public UpdateAction
{
    using UpdateAction::UpdateAction;

public:
    /// @brief destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10639
    /// @needwork = dda
    /// @endcode
    ~DeltaUpdateAction() override = default;

    /// @brief delete copy construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10640
    /// @needwork = dda
    /// @endcode
    DeltaUpdateAction(DeltaUpdateAction const& other) = delete;
    /// @brief delete copy asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10641
    /// @needwork = dda
    /// @endcode
    DeltaUpdateAction& operator=(DeltaUpdateAction const& other) = delete;
    /// @brief delete move construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10642
    /// @needwork = dda
    /// @endcode
    DeltaUpdateAction(DeltaUpdateAction&& other) = delete;
    /// @brief delete move asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10643
    /// @needwork = dda
    /// @endcode
    DeltaUpdateAction& operator=(DeltaUpdateAction&& other) = delete;

    // Place the new version software set into its target installation directory (including the var directory)
    /// @brief PlaceToDestinationPath
    /// @return result
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10644
    /// @needwork = dda
    /// @endcode
    AraResultVoid PlaceToDestinationPath() override;

private:
    // remove files which are copied from old version dir but not in new delta version
    /// @brief _removeOldFiles
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10645
    /// @needwork = dda
    /// @endcode
    void _removeOldFiles() const;
};

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // ARA_UCM_PKGMGR_STORAGE_DELTA_UPDATE_ACTION_H_
