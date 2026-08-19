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
/// @file       vehicle_state_manager_service.h
/// @brief      All service interfaces provided by VUCM to the vehicle state manager adaptive application
/// @details
/// @date       2024-07-20
/// @author     hanzhibo
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// export_level=/UCM Master/Package Management Service
/// @module_path=/UCM Master/Services
/// @interface_level=module
/// @trace_id_sr=SR_UCM_00035,SR_UCM_00037
/// @unit_name=VehicleStateManagerService
/// @unit_description=All service interfaces provided by VUCM to the vehicle state manager adaptive application
/// @endcode
///
/// ================================================================

#ifndef ARA_UCM_VPKGMGR_VEHICLE_STATE_MANAGER_H_
#define ARA_UCM_VPKGMGR_VEHICLE_STATE_MANAGER_H_

#include <ara/core/future.h>
#include <ara/ucm/pkgmgr/error_domain_ucmerrordomain.h>

#include <algorithm>
#include <functional>
#include <memory>
#include <utility>

#include "ara/ucm/vsm/vehiclestatemanager_skeleton.h"
///#include "fsm/fsm_manager.h"

namespace ara {
namespace ucm {
namespace vpkgmgr {

/// @brief  All service interfaces provided by VUCM to the vehicle state manager adaptive application
///
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_Master_00444
/// @trace_id_dd=DD_UCM_Master_00897
/// @trace_id_sr=SR_UCM_00035,SR_UCM_00037
/// @needwork = ad
/// @endcode
class VehicleStateManagerService : public ara::ucm::vsm::skeleton::VehicleStateManagerSkeleton
{
public:
    /// @brief alias CB
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_
    /// @trace_id_dd=DD_UCM_
    /// @needwork = no
    /// @endcode
    using CB = std::function< void(bool const&, pkgmgr::SafetyPolicyType const&) >;
    /// @brief alias Ptr
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_
    /// @trace_id_dd=DD_UCM_
    /// @needwork = no
    /// @endcode
    using Ptr = std::shared_ptr< VehicleStateManagerService >;

    /// @brief constructor
    /// @param instanceSpec
    /// @param mode
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00445
    /// @trace_id_dd=DD_UCM_Master_00898
    /// @needwork = ad
    /// @endcode
    VehicleStateManagerService(ara::core::InstanceSpecifier instanceSpec,
                               ara::com::MethodCallProcessingMode const mode);
    /// @brief constructor
    /// @param instanceId
    /// @param mode
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00446
    /// @trace_id_dd=DD_UCM_Master_00899
    /// @needwork = ad
    /// @endcode
    VehicleStateManagerService(ara::com::InstanceIdentifier instanceId, ara::com::MethodCallProcessingMode const mode);
    ///virtual ~VehicleStateManagerService() { std::cout << __FUNCTION__ << std::endl; }

    /// @brief FieldInit
    /// @return bool
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00447
    /// @trace_id_dd=DD_UCM_Master_00900
    /// @needwork = ad
    /// @endcode
    bool FieldInit();

    ///inline void AttachOperator(FsmManager::Ptr const& fsmPtr) { fsmPtr_ = fsmPtr; }

    /// @brief SetSafetyStateCB
    /// @param cb
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00448
    /// @trace_id_dd=DD_UCM_Master_00901
    /// @trace_id_sr=SR_UCM_00035,SR_UCM_00037
    /// @needwork = ad
    /// @endcode
    void SetSafetyStateCB(CB&& cb) noexcept;

    /// @brief Method called by the vehicle state manager adaptive application when the safety state changes
    /// @param safetyPolicy Safety policy calculated by the vehicle state manager adaptive application
    /// @param safeToUpdate Whether the vehicle state is safe
    /// @return Returns a Future of the vehicle state safety (please refer to ara::core::Future usage instructions) or a specific error code
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00449
    /// @trace_id_dd=DD_UCM_Master_00902
    /// @trace_id_sr=SR_UCM_00035,SR_UCM_00037
    /// @needwork = ad
    /// @endcode
    ara::core::Future< void > SafetyState(pkgmgr::SafetyPolicyType const& safetyPolicy,
                                          bool const& safeToUpdate) override;

private:
    /// @brief actionCB_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00903
    /// @needwork = dda
    /// @endcode
    CB actionCB_;
    /// @brief fsmPtr_  ///FsmManager::Ptr fsmPtr_;
};

}  // namespace vpkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // ARA_UCM_VPKGMGR_VEHICLE_STATE_MANAGER_H_