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
/// @file       os_update_action.h
/// @brief      OS update action
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
/// @unit_name=OSUpdateAction
/// @unit_description=OS update action
/// @endcode
///
/// ================================================================

#ifndef ARA_UCM_PKGMGR_STORAGE_OS_UPDATE_ACTION_H_
#define ARA_UCM_PKGMGR_STORAGE_OS_UPDATE_ACTION_H_

#include "os_update_operator.h"
#include "update_action.h"

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @brief Action for os update of a software package.
///
/// @code{.isoft}
/// @uptrace={SWS_UCM_00001, a665954b7a62b2349db109a9408efe084b8a7136}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10269
/// @trace_id_dd=DD_UCM_10575
/// @needwork = ad
/// @endcode
class OSUpdateAction : public UpdateAction
{
    using UpdateAction::UpdateAction;

public:
    /// @brief destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10576
    /// @needwork = dda
    /// @endcode
    ~OSUpdateAction() override = default;

    /// @brief delete copy construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10577
    /// @needwork = dda
    /// @endcode
    OSUpdateAction(OSUpdateAction const& other) = delete;
    /// @brief delete copy asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10578
    /// @needwork = dda
    /// @endcode
    OSUpdateAction& operator=(OSUpdateAction const& other) = delete;
    /// @brief delete move construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10579
    /// @needwork = dda
    /// @endcode
    OSUpdateAction(OSUpdateAction&& other) = delete;
    /// @brief delete move asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10580
    /// @needwork = dda
    /// @endcode
    OSUpdateAction& operator=(OSUpdateAction&& other) = delete;

    // Install to OS partition
    /// @brief PlaceToOSPatition
    /// @return result
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10581
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
    /// @trace_id_dd=DD_UCM_10582
    /// @needwork = dda
    /// @endcode
    AraResultVoid InvalidOSPatition(AraString const& oSVersion) noexcept override;

private:
    /// @brief osOperator_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10583
    /// @needwork = dda
    /// @endcode
    OSUpdateOperator osOperator_{targetPath_};  // OS update operator
};

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // ARA_UCM_PKGMGR_STORAGE_OS_UPDATE_ACTION_H_
