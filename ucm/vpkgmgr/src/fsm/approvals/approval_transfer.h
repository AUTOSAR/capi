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
/// @file       approval_transfer.h
/// @brief      ApprovalTransfer class definition
/// @details
/// @date       2024-07-20
/// @author     hanzhibo
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// export_level=/UCM Master/Fsm
/// @module_path=/UCM Master/Fsm
/// @interface_level=unit
/// @trace_id_sr=SR_UCM_00035
/// @unit_name=ApprovalTransfer
/// @unit_description=ApprovalTransfer class definition
/// @endcode
///
/// ================================================================

#ifndef VPKGMGR_SRC_FSM_APPROVAL_TRANSFER_H_
#define VPKGMGR_SRC_FSM_APPROVAL_TRANSFER_H_

#include <memory>

#include "fsm/approvals/event.h"
#include "services/vehicle_driver_application/vehicle_driver_interface_service.h"
#include "services/vehicle_state_manager/vehicle_state_manager_service.h"

namespace ara {
namespace ucm {
namespace vpkgmgr {

/// @brief ApprovalTransfer
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_Master_00094
/// @trace_id_dd=DD_UCM_Master_00234
/// @needwork = ad
/// @endcode
class ApprovalTransfer : public std::enable_shared_from_this< ApprovalTransfer >
{
public:
    /// @brief shared_ptr of ApprovalTransfer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_
    /// @trace_id_dd=DD_UCM_
    /// @needwork = no
    /// @endcode
    using Ptr = std::shared_ptr< ApprovalTransfer >;

    /// @brief constructor
    /// @param vdiSPtr
    /// @param vsmSPtr
    /// @param approval
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00095
    /// @trace_id_dd=DD_UCM_Master_00235
    /// @needwork = ad
    /// @endcode
    ApprovalTransfer(VehicleDriverInterfaceService::Ptr vdiSPtr,
                     VehicleStateManagerService::Ptr vsmSPtr,
                     bool const& approval)
        : std::enable_shared_from_this< ApprovalTransfer >{}
        , driverApproval_{approval}
        , approval_{false}
        , vsmSafeToUpdate_{false}
        , vsmSafetyPolicy_{""}
        , vsmEvent_{}
        , driverEvent_{}
        , vdia_{std::move(vdiSPtr)}
        , vsm_{std::move(vsmSPtr)}
    {
    }
    /// @brief destructor
    ///virtual ~ApprovalTransfer() = default;

    /// @brief Run
    /// @param safetyCondition
    /// @return bool
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00096
    /// @trace_id_dd=DD_UCM_Master_00236
    /// @needwork = ad
    /// @endcode
    bool Run(pkgmgr::SafetyPolicyType const& safetyCondition);

private:
    /// @brief _signalFromVsm
    /// @param safeToUpdate
    /// @param vsmSafetyPolicy
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00237
    /// @needwork = dda
    /// @endcode
    void _signalFromVsm(bool const& safeToUpdate, pkgmgr::SafetyPolicyType const& vsmSafetyPolicy);
    /// @brief _signalFromVdia
    /// @param approval
    /// @param safetyCondition
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00238
    /// @needwork = dda
    /// @endcode
    void _signalFromVdia(bool const& approval, pkgmgr::SafetyPolicyType const& safetyCondition);
    /// @brief _vsmCondition
    /// @param safetyCondition
    /// @return bool
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00239
    /// @needwork = dda
    /// @endcode
    bool _vsmCondition(pkgmgr::SafetyPolicyType const& safetyCondition);
    /// @brief _waitrSafeConditions
    /// @param safetyCondition
    /// @return bool
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00240
    /// @needwork = dda
    /// @endcode
    bool _waitrSafeConditions(pkgmgr::SafetyPolicyType const& safetyCondition);
    /// @brief _driverNotification
    /// @return bool
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00241
    /// @needwork = dda
    /// @endcode
    bool _driverNotification();

private:
    /// @brief Whether to request approval from the driver
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00242
    /// @needwork = dda
    /// @endcode
    bool driverApproval_;
    /// @brief approval_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00243
    /// @needwork = dda
    /// @endcode
    bool approval_;
    /// @brief Notification from vehicle state management on whether to continue the upgrade process
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00244
    /// @needwork = dda
    /// @endcode
    bool vsmSafeToUpdate_;
    /// @brief Notification from vehicle state management on support or not support, with a text hint for the driver
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00245
    /// @needwork = dda
    /// @endcode
    pkgmgr::SafetyPolicyType vsmSafetyPolicy_;

    /// @brief vsmEvent_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00246
    /// @needwork = dda
    /// @endcode
    Event vsmEvent_;
    /// @brief driverEvent_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00247
    /// @needwork = dda
    /// @endcode
    Event driverEvent_;
    /// @brief vdia_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00248
    /// @needwork = dda
    /// @endcode
    VehicleDriverInterfaceService::Ptr vdia_;
    /// @brief vsm_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00249
    /// @needwork = dda
    /// @endcode
    VehicleStateManagerService::Ptr vsm_;
};

}  // namespace vpkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // VPKGMGR_SRC_FSM_APPROVAL_TRANSFER_H_