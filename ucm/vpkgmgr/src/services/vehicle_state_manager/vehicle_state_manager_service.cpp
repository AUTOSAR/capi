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
/// @file       vehicle_state_manager_service.cpp
/// @brief      All service interfaces provided by VUCM to the vehicle state manager adaptive application
/// @details
/// @date       2024-07-20
/// @author     hanzhibo
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/UCM Master/Services
/// @interface_level=module
/// @trace_id_sr=SR_UCM_00035
/// @unit_name=VehicleStateManagerService
/// @unit_description=All service interfaces provided by VUCM to the vehicle state manager adaptive application
/// @endcode
///
/// ================================================================

#include "services/vehicle_state_manager/vehicle_state_manager_service.h"

#include "consts.h"
#include "fsm/fsm_manager.h"
#include "utils/ucmm_log.h"

namespace ara {
namespace ucm {
namespace vpkgmgr {

/// @brief alias SkeletonApp
using SkeletonApp = ara::ucm::vsm::skeleton::VehicleStateManagerSkeleton;

/// @brief constructor
/// @param instanceSpec
/// @param mode
/// @throws no
VehicleStateManagerService::VehicleStateManagerService(ara::core::InstanceSpecifier instanceSpec,
                                                       ara::com::MethodCallProcessingMode const mode)
    : SkeletonApp{std::move(instanceSpec), mode}
    , actionCB_{[](bool const&, pkgmgr::SafetyPolicyType const&) noexcept { LOG_WARN << "call default callback;"; }}
{
}

/// @brief constructor
/// @param instanceId
/// @param mode
/// @throws no
VehicleStateManagerService::VehicleStateManagerService(ara::com::InstanceIdentifier instanceId,
                                                       ara::com::MethodCallProcessingMode const mode)
    : SkeletonApp{std::move(instanceId), mode}
    , actionCB_{[](bool const&, pkgmgr::SafetyPolicyType const&) noexcept { LOG_WARN << "call default callback;"; }}
{
}

/// @brief FieldInit
/// @return bool
/// @throws no
bool VehicleStateManagerService::FieldInit()
{
    ara::core::Result< void > const voidRes{SafetyPolicy.Update(kNone)};
    if (!voidRes.HasValue()) {
        LOG_ERROR << "VehicleStateManagerService SafetyPolicy Init value failed errmsg="
                  << voidRes.Error().Message().data();
        return false;
    }
    return true;
}

/// @brief SetSafetyStateCB
/// @param cb
/// @throws no
void VehicleStateManagerService::SetSafetyStateCB(CB&& cb) noexcept { actionCB_ = std::move(cb); }

///
/// @brief The skeleton receives the safetyPolicy from the proxy (vehicle state app) and the
/// calculated current status indicating whether the operation is allowed. Called by the vehicle state app when vehicle state changes.
///
/// @param safetyPolicy  Safety policy, vehicle state (e.g., driving, parked, reversing, engine off, etc.)
/// @param safeToUpdate  Whether the vehicle state is safe, and if VehicleStateManager can recognize/support it.
/// @return ara::core::Future<void>
/// @throws no
ara::core::Future< void > VehicleStateManagerService::SafetyState(pkgmgr::SafetyPolicyType const& safetyPolicy,
                                                                  const bool& safeToUpdate)
{
    LOG_INFO << "VehicleStateManagerService::SafetyState safeToUpdate=" << BoolToStr(safeToUpdate).data()
             << "safetyPolicy" << safetyPolicy.c_str();

    ara::core::Promise< void > promise;
    ara::core::Future< void > future{promise.get_future()};

    actionCB_(safeToUpdate, safetyPolicy);

    promise.set_value();
    return future;
}  // namespace vpkgmgr

}  // namespace vpkgmgr
}  // namespace ucm
}  // namespace ara
