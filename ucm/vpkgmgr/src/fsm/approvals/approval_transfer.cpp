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
/// @file       approval_transfer.cpp
/// @brief      ApprovalTransfer class implementation
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
/// @unit_name=ApprovalTransfer
/// @unit_description=ApprovalTransfer class implementation
/// @endcode
///
/// ================================================================

#include "fsm/approvals/approval_transfer.h"

#include <ara/core/future.h>
#include <ara/core/promise.h>

#include "fsm/fsm_manager.h"
#include "utils/ucmm_log.h"

namespace ara {
namespace ucm {
namespace vpkgmgr {

/// @brief _signalFromVsm
/// @param safeToUpdate
/// @param vsmSafetyPolicy
/// @throws no
void ApprovalTransfer::_signalFromVsm(bool const& safeToUpdate, pkgmgr::SafetyPolicyType const& vsmSafetyPolicy)
{
    LOG_INFO << "get safetyPolicy=" << vsmSafetyPolicy.c_str() << "safeToUpdate=" << BoolToStr(safeToUpdate).data();
    vsmSafeToUpdate_ = safeToUpdate;
    vsmSafetyPolicy_
        = vsmSafetyPolicy;  // Obtained from vehicle state management notification, used to notify the driver for awareness
    vsmEvent_.Signal();
}
/// @brief _signalFromVdia
/// @param approval
/// @param safetyCondition
/// @throws no
void ApprovalTransfer::_signalFromVdia(bool const& approval, pkgmgr::SafetyPolicyType const& safetyCondition)
{
    LOG_INFO << "get approval=" << BoolToStr(approval).data() << "safetyCondition=" << safetyCondition.c_str();
    approval_
        = approval;  // safetyCondition is not used; the new version of DriverApproval interface only has the approval parameter.
    driverEvent_.Signal();
}

/// @brief Run
/// @param safetyCondition
/// @return bool
/// @throws no
bool ApprovalTransfer::Run(pkgmgr::SafetyPolicyType const& safetyCondition)
{
    if (false == _driverNotification()) {
        return false;
    }
    return _waitrSafeConditions(safetyCondition);
}

/// @brief _waitrSafeConditions
/// @param safetyCondition
/// @return bool
/// @throws no
bool ApprovalTransfer::_waitrSafeConditions(pkgmgr::SafetyPolicyType const& safetyCondition)
{
    /// Note: process waits here until the state management meets the relevant state conditions
    std::weak_ptr< ApprovalTransfer > const self{shared_from_this()};
    {
        VehicleStateManagerService::CB actionCB;
        actionCB = [self](bool const& safeToUpdate, pkgmgr::SafetyPolicyType const& safetyPolicy) {
            ApprovalTransfer::Ptr strongSelf{self.lock()};
            if (nullptr == strongSelf.get()) {
                LOG_WARN << "ApprovalTransfer ";
                return;
            }
            strongSelf->_signalFromVsm(safeToUpdate, safetyPolicy);
        };
        vsm_->SetSafetyStateCB(std::move(actionCB));
    }

    LOG_INFO << "notification to vsm, safetyCondition=" << safetyCondition.c_str();
    FsmManager* const fsm{FsmManager::GetInstance()};
    std::ignore = vsm_->SafetyPolicy.Update(safetyCondition);
    while (!_vsmCondition(safetyCondition)) {
        if (!(fsm->CanMoveForward())) {
            LOG_INFO << "cancel";
            return false;
        }
        LOG_INFO << "vsm is not meet the conditions, wait~";
    }
    return true;
}

/// @brief _vsmCondition
/// @param safetyCondition
/// @return bool
/// @throws no
bool ApprovalTransfer::_vsmCondition(pkgmgr::SafetyPolicyType const& safetyCondition)
{
    vsmEvent_.Add(1);
    FsmManager* const fsm{FsmManager::GetInstance()};
    while (true) {
        if (Event::ResultType::kSuccess == vsmEvent_.Wait(static_cast< std::int32_t >(kGlobalWaitTime))) {
            break;
        }
        if (!(fsm->CanMoveForward())) {
            LOG_INFO << "cancel";
            return false;
        }
        /// If no response within timeout, continue to notify vehicle state management
        // FIXME(yunfei) There is a risk of information being lost during transmission
        LOG_INFO << "event timeout";
        std::ignore = vsm_->SafetyPolicy.Update(safetyCondition);
    }
    LOG_INFO << "VSM SafetyState safeToUpdate= " << BoolToStr(vsmSafeToUpdate_).data();
    std::ignore = vdia_->SafetyState.Update(vsmSafeToUpdate_);
    std::ignore = vdia_->SafetyPolicy.Update(vsmSafetyPolicy_);
    if (!vsmSafeToUpdate_) {
        return false;
    }
    vsmEvent_.Reset();
    return true;
}

/// @brief _driverNotification
/// @return bool
/// @throws no
bool ApprovalTransfer::_driverNotification()
{
    if (!driverApproval_) {
        LOG_INFO << "vehicle package config notification driver transfer= " << BoolToStr(driverApproval_).data();
        return true;
    }

    std::weak_ptr< ApprovalTransfer > const self{shared_from_this()};
    VehicleDriverInterfaceService::CBType actionCB;
    actionCB = [self](bool const& approval, pkgmgr::SafetyPolicyType const& vdiSafetyCondition) {
        ApprovalTransfer::Ptr strongSelf{self.lock()};
        if (nullptr == strongSelf.get()) {
            LOG_WARN << "";
            return;
        }
        strongSelf->_signalFromVdia(approval, vdiSafetyCondition);
    };
    vdia_->SetApprovalCB(actionCB);

    LOG_INFO << "notification: approval transfer.";
    // It is the current vehicle status returned by vehicle state management, requiring driver awareness of the current vehicle status
    std::ignore = vdia_->ApprovalRequired.Update(true);

    /// WaitApproval();. // Define a class containing a promise to encapsulate WaitApproval
    driverEvent_.Add(1);

    FsmManager* const fsm{FsmManager::GetInstance()};
    while (true) {
        if (!(fsm->CanMoveForward())) {
            LOG_INFO << "cancel";
            return false;
        }

        Event::ResultType const ret{driverEvent_.Wait(static_cast< std::int32_t >(kGlobalWaitTime))};
        if (Event::ResultType::kTimeout == ret) {
            std::ignore = vdia_->ApprovalRequired.Update(true);
            LOG_DEBUG << "driverNotification wait timeout";
            continue;
        }
        /// if (Event::ResultType::kFail == ret) {
        ///     LOG_ERROR << "safeEvent wait failed, must check logical!!!";
        ///     return 1;
        /// }
        break;
    }

    LOG_INFO << "driver notification, approval=" << BoolToStr(approval_).data();
    if (!approval_) {
        LOG_DEBUG << "get approval_:" << approval_ << ", so will TriggerCancel in TaskQueue.";
        fsm->TriggerCancelInTQ(pkgmgr::UCMMasterResolutionType::kVehicleDriverInterfaceCommunicationError);
        return false;
    }
    driverEvent_.Reset();
    /// TODO(yunfei) If the com side does not send notification because the field value is the same
    /// this->vdia_->SafetyPolicy.Update(kNone);
    /// this->vdia_->ApprovalRequired.Update(false);
    /// LOG_INFO << "driverNotification:" << driverNotification << " safetyPolicy=" << safetyPolicy.c_str()
    ///          << " approval=" << BoolToStr(approval_).data();
    return true;
}

}  // namespace vpkgmgr
}  // namespace ucm
}  // namespace ara