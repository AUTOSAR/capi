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
/// @file       swp_step_activation.cpp
/// @brief      ActivationStep impl
/// @details
/// @date       2024-07-20
/// @author     hanzhibo
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/UCM Master/Fsm
/// @interface_level=unit
/// @trace_id_sr=SR_UCM_00035
/// @unit_name=ActivationStep
/// @unit_description=ActivationStep impl
/// @endcode
///
/// ================================================================

#include "fsm/campaign_steps/swp_step_activation.h"

#include "fsm/fsm_manager.h"
#include "package_management_service/find_ucm_subs.h"

namespace ara {
namespace ucm {
namespace vpkgmgr {

/// @brief Execute the Activation step, i.e. trigger UCM Subordinate to activate the SwPackage identified by the
/// current Transfer ID
/// @param ucmId The ID of the UCM subordinate that shall compute the action
/// @return result
/// @throws no
ara::core::Result< void > ActivationStep::Execute(ara::core::String const& ucmId) const
{
    log_.LogDebug() << "ActivationStep::Execute(), begin with ucmId:" << ucmId.c_str();

    FindUcmSubs* const subs{FindUcmSubs::GetInstance()};

    PackageManagementAppPtr pmAppPtr{subs->GetPackageManagementAppById(ucmId)};
    if (nullptr == pmAppPtr.get()) {
        log_.LogError() << "ActivationStep::Execute(), can't get GetPackageManagementAppById for ucmId:"
                        << ucmId.c_str() << ", so will TriggerCancel.";

        // Cancel Campaign
        FsmManager::GetInstance()->TriggerCancelInTQ(
            pkgmgr::UCMMasterResolutionType::kSubordinateNotAvailableOnTheNetwork);
        return ara::core::Result< void >::FromError(pkgmgr::UCMErrorDomainErrc::kGeneralReject);
    }
    /// if (!pmAppPtr->UpdateStatus()) {
    ///     LOG_ERROR << "pmApp update status err";
    ///     return std::move(ara::core::Result<void>::FromError(pkgmgr::UCMErrorDomainErrc::kGeneralReject));
    /// }

    bool const result{pmAppPtr->Activate()};
    if (!result) {
        log_.LogDebug() << "ActivationStep::Execute(), haven't finished to call pmAppPtr->Activate() for ucmId:"
                        << ucmId.c_str();
        return ara::core::Result< void >::FromError(pkgmgr::UCMErrorDomainErrc::kGeneralReject);
    }
    log_.LogDebug() << "ActivationStep::Execute(), end with success for ucmId:" << ucmId.c_str();
    return ara::core::Result< void >::FromValue();
}

}  // namespace vpkgmgr
}  // namespace ucm
}  // namespace ara
