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
/// @file       os_delta_update_action.h
/// @brief      OS delta update action
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
/// @unit_name=OSDeltaUpdateAction
/// @unit_description=OS delta update action
/// @endcode
///
/// ================================================================

#ifndef ARA_UCM_PKGMGR_STORAGE_OS_DELTA_UPDATE_ACTION_H_
#define ARA_UCM_PKGMGR_STORAGE_OS_DELTA_UPDATE_ACTION_H_

#include "delta_update_action.h"
#include "os_update_operator.h"

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @brief OS delta update
///
/// @code{.isoft}
/// @uptrace={SWS_UCM_00001, a665954b7a62b2349db109a9408efe084b8a7136}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10265
/// @trace_id_dd=DD_UCM_10504
/// @needwork = ad
/// @endcode
class OSDeltaUpdateAction : public DeltaUpdateAction
{
    using DeltaUpdateAction::DeltaUpdateAction;

public:
    /// @brief destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10505
    /// @needwork = dda
    /// @endcode
    ~OSDeltaUpdateAction() override = default;

    /// @brief delete copy construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10506
    /// @needwork = dda
    /// @endcode
    OSDeltaUpdateAction(OSDeltaUpdateAction const& other) = delete;
    /// @brief delete copy asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10507
    /// @needwork = dda
    /// @endcode
    OSDeltaUpdateAction& operator=(OSDeltaUpdateAction const& other) = delete;
    /// @brief delete move construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10508
    /// @needwork = dda
    /// @endcode
    OSDeltaUpdateAction(OSDeltaUpdateAction&& other) = delete;
    /// @brief delete move asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10509
    /// @needwork = dda
    /// @endcode
    OSDeltaUpdateAction& operator=(OSDeltaUpdateAction&& other) = delete;

    // Install to OS partition
    /// @brief PlaceToOSPatition
    /// @return result
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10510
    /// @needwork = dda
    /// @endcode
    AraResultVoid PlaceToOSPatition() noexcept override;

    // Invalidate the OS partition of the specified version
    /// @brief InvalidOSPatition
    /// @param oSVersion
    /// @return result
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10511
    /// @needwork = dda
    /// @endcode
    AraResultVoid InvalidOSPatition(AraString const& oSVersion) noexcept override;

private:
    /// @brief osOperator_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10512
    /// @needwork = dda
    /// @endcode
    OSUpdateOperator osOperator_{targetPath_};  // OS update operator
};

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // ARA_UCM_PKGMGR_STORAGE_OS_DELTA_UPDATE_ACTION_H_
