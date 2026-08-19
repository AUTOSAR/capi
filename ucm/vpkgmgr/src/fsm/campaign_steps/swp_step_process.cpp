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
/// @file       swp_step_process.cpp
/// @brief      ProcessStep impl
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
/// @unit_name=ProcessStep
/// @unit_description=ProcessStep impl
/// @endcode
///
/// ================================================================

#include "fsm/campaign_steps/swp_step_process.h"

#include <ara/com/internal/runtime.h>
#include <ara/core/string.h>
#include <ara/ucm/internal/transfer/helper.h>

#include <utility>

#include "fsm/fsm_manager.h"
#include "package_management_service/find_ucm_subs.h"
#include "utils/helper.h"
#include "utils/ucmm_log.h"

namespace ara {
namespace ucm {
namespace vpkgmgr {

/// @brief GetSpkgRef
/// @param processInfo
/// @throws no
void ProcessStep::Load(ProcessStepInfoPtr const& processInfo)
{
    spkgRef_ = processInfo->softwarePackageRef;
    return;
}

/// @brief Execute ProcessThread in a separate thread
/// @param ucmId
/// @param swclNameToTid
/// @param promise
/// @throws no
void ProcessStep::ProcessThread(
    ara::core::String const& ucmId,
    ara::core::Map< ara::core::String, std::pair< bool, pkgmgr::TransferIdType > >* const swclNameToTid,
    ara::core::Promise< void >&& promise) const
{
    log_.LogDebug() << "ProcessStep::ProcessThread(), begin with spkgRef_:" << spkgRef_.c_str()
                    << "ucmId:" << ucmId.c_str();
    // Set the value of swclNameToTid here; processStepPtr_->Execute will use this value
    ara::core::Result< void > const result{_execute(ucmId, *swclNameToTid)};
    if (!result.HasValue()) {  // Execution failed
        log_.LogError() << "ProcessStep::ProcessThread(), failed to _execute for spkgRef_:" << spkgRef_.c_str()
                        << "ucmId:" << ucmId.c_str();
    } else {  // Execution succeeded
        log_.LogDebug() << "ProcessStep::ProcessThread(), succeed to _execute for spkgRef_:" << spkgRef_.c_str()
                        << "ucmId:" << ucmId.c_str();
        FsmManager::GetInstance()->ProcessedAdd();
    }

    promise.SetResult(result);
    log_.LogDebug() << "ProcessStep::ProcessThread(), end for spkgRef_:" << spkgRef_.c_str()
                    << "ucmId:" << ucmId.c_str();
}

/// @brief _execute the Process step, i.e. trigger UCM Subordinate to process the SwPackage identified by the current
/// Transfer ID
///
/// @param ucmId The ID of the UCM subordinate that shall compute the action
/// @param swclNameToTid Map that link a software cluster name to a Transfer ID
/// Map<ara::core::String, TransferIdType>& swclNameToTid
/// @return result
/// @throws no
ara::core::Result< void > ProcessStep::_execute(
    ara::core::String const& ucmId,
    ara::core::Map< ara::core::String, std::pair< bool, pkgmgr::TransferIdType > >& swclNameToTid) const
{
    log_.LogDebug() << "ProcessStep::_execute(), begin with spkgRef_:" << spkgRef_.c_str() << "ucmId:" << ucmId.c_str();

    FindUcmSubs* const subs{FindUcmSubs::GetInstance()};
    FsmManager* const fsm{FsmManager::GetInstance()};

    PackageManagementAppPtr pmAppPtr{subs->GetPackageManagementAppById(ucmId)};
    if (nullptr == pmAppPtr.get()) {
        log_.LogError() << "ProcessStep::_execute(), can't get GetPackageManagementAppById for ucmId:" << ucmId.c_str()
                        << ", so will TriggerCancel.";

        // Cancel Campaign
        FsmManager::GetInstance()->TriggerCancelInTQ(
            pkgmgr::UCMMasterResolutionType::kSubordinateNotAvailableOnTheNetwork);
        return ara::core::Result< void >::FromError(pkgmgr::UCMErrorDomainErrc::kGeneralReject);
    }

    // Wait to find TransferIdType from swclNameToTid, and execute installation
    while (true) {
        if (!(fsm->CanMoveForward())) {
            log_.LogError() << "ProcessStep::_execute(), failed to CanMoveForward for spkgRef_:" << spkgRef_.c_str()
                            << "ucmId:" << ucmId.c_str() << ", so will return.";
            return ara::core::Result< void >::FromError(pkgmgr::UCMErrorDomainErrc::kProcessSwPackageCancelled);
        }

        // Found
        if (swclNameToTid.find(spkgRef_) != swclNameToTid.end()) {
            break;
        }
        log_.LogVerbose() << "ProcessStep::_execute(), the softwarepackage" << spkgRef_.c_str()
                          << "has not transferred yet";

        if (!pmAppPtr->UpdateStatus()) {
            log_.LogError() << "TransferStep::_execute(), failed to UpdateStatus for ucmId:" << ucmId.c_str()
                            << ", so will TriggerCancelInTQ.";

            // Cancel Campaign
            FsmManager::GetInstance()->TriggerCancelInTQ(
                pkgmgr::UCMMasterResolutionType::kSubordinateNotAvailableOnTheNetwork);
            return ara::core::Result< void >::FromError(pkgmgr::UCMErrorDomainErrc::kGeneralReject);
        }
        std::this_thread::sleep_for(
            std::chrono::milliseconds(kGlobalWaitTime));  // Wait for software package transfer to complete.
    }
    ara::core::Map< ara::core::String, std::pair< bool, pkgmgr::TransferIdType > >::iterator const iter{
        swclNameToTid.find(spkgRef_)};
    if (iter == swclNameToTid.end()) {
        log_.LogError() << "ProcessStep::_execute(), can't find transferId for the spkgRef_" << spkgRef_.c_str()
                        << ", so will TriggerCancel.";

        // Cancel Campaign
        FsmManager::GetInstance()->TriggerCancelInTQ(pkgmgr::UCMMasterResolutionType::kUCMError);
        return ara::core::Result< void >::FromError(pkgmgr::UCMErrorDomainErrc::kGeneralReject);
    }
    if (!iter->second.first) {
        log_.LogError() << "ProcessStep::_execute(), the spkgRef_" << spkgRef_.c_str()
                        << "has not transferred successfully, so will TriggerCancel.";

        // Cancel Campaign
        FsmManager::GetInstance()->TriggerCancelInTQ(pkgmgr::UCMMasterResolutionType::kUCMError);
        return ara::core::Result< void >::FromError(pkgmgr::UCMErrorDomainErrc::kGeneralReject);
    }
    pkgmgr::TransferIdType const id{(*iter).second.second};
    log_.LogDebug() << "ProcessStep::_execute(), get id:" << pkgmgr::helper::ToHexString(id).c_str()
                    << "for spkgRef_:" << spkgRef_.c_str();

    if (!FsmManager::GetInstance()->CanProcess()) {
        log_.LogError() << "ProcessStep::_execute(), failed to CanProcess for spkgRef_:" << spkgRef_.c_str()
                        << "ucmId:" << ucmId.c_str() << ", so will return.";
        return ara::core::Result< void >::FromError(pkgmgr::UCMErrorDomainErrc::kGeneralReject);
    }

    if (!pmAppPtr->UpdateStatus()) {
        log_.LogError() << "TransferStep::_execute(), failed to UpdateStatus for ucmId:" << ucmId.c_str()
                        << ", so will TriggerCancel.";

        // Cancel Campaign
        FsmManager::GetInstance()->TriggerCancelInTQ(
            pkgmgr::UCMMasterResolutionType::kSubordinateNotAvailableOnTheNetwork);
        return ara::core::Result< void >::FromError(pkgmgr::UCMErrorDomainErrc::kGeneralReject);
    }

    bool const result{pmAppPtr->Install(id)};
    if (!result) {
        log_.LogError() << "TransferStep::_execute(), failed to Install for spkgRef_:" << spkgRef_.c_str();
        return ara::core::Result< void >::FromError(pkgmgr::UCMErrorDomainErrc::kGeneralReject);
    }
    // Installation successful

    // // Clean up software package according to SoftwarePackageStoring.storing
    // cleanSWP(pmAppPtr, id);.

    if (0U >= swclNameToTid.erase(spkgRef_)) {
        ///LOG_WARN << "swclNameToTid erase err key=" << spkgRef_.c_str();
    }
    log_.LogDebug() << "ProcessStep::_execute(), end with success for spkgRef_" << spkgRef_.c_str();
    return ara::core::Result< void >::FromValue();
}

// /// @brief  Clean up software package
// /// @param pmAppPtr
// /// @param transferId
// /// @throws no
// void ProcessStep::cleanSWP(PackageManagementAppPtr pmAppPtr, pkgmgr::TransferIdType const& transferId) const {.
//     log_.LogDebug() << "ProcessStep::cleanSWP(), begin with transferId:" << pkgmgr::helper::ToHexString(transferId).c_str() << "spkgRef_:" << spkgRef_.c_str();.

//     SwPackageInfoPtr spkgPtr{FsmManager::GetInstance()->GetSpkgByName(spkgRef_)};.
//     if (nullptr == spkgPtr.get()) {.
//         log_.LogDebug() << "ProcessStep::cleanSWP(), can't GetSpkgByName for spkgRef_" << spkgRef_.c_str();.
//         return;.
//     }.

//     SoftwarePackageStoringEnum const storing{spkgPtr->storing};.
//     pkgmgr::TransferIdType const transferIdInUCMM{spkgPtr->transferId};.
//     log_.LogDebug() << "ProcessStep::cleanSWP(), get storing:" << helper::SoftwarePackageStoringToString(storing).data().
//                     << "transferIdInUCMM:" << pkgmgr::helper::ToHexString(transferIdInUCMM).c_str().
//                     << "for spkgRef_:" << spkgRef_.c_str();.
//     switch (storing) {.
//         case SoftwarePackageStoringEnum::kNone: {.
//             /// none: In SOFTWAREPACKAGE_TRANSFERRING state, the software package is not stored inside ucmm, it is immediately passed to ucm; and deleted from ucm after installation ends.
//             /// Delete from target ucm.
//             std::ignore = pmAppPtr->DeleteSoftwarePackage(transferId);.

//             /// Delete from current ucmm.
//             std::ignore = FsmManager::GetInstance()->CleanSWPackage(transferIdInUCMM);.
//             break;.
//         }.
//         case SoftwarePackageStoringEnum::kUcmMaster: {.
//             /// ucmMaster: In SOFTWAREPACKAGE_TRANSFERRING state, the software package is first stored in ucmm, then passed to ucm (does not need to wait for complete package), and attempt deletion from ucm after installation ends (even if the installation package may no longer exist on ucm).
//             /// Delete from target ucm
//             std::ignore = pmAppPtr->DeleteSoftwarePackage(transferId);.
//             break;.
//         }
//         case SoftwarePackageStoringEnum::kUcm: {.
//             /// ucm: In SOFTWAREPACKAGE_TRANSFERRING state, the software package is not stored in ucmm, it is immediately passed to ucm; and it is not deleted from ucm after installation ends (ucm may delete it itself after installation).
//             /// Delete from current ucmm.
//             std::ignore = FsmManager::GetInstance()->CleanSWPackage(transferIdInUCMM);.
//             break;.
//         }.
//         default: {.
//             break;.
//         }.
//     }.

//     log_.LogDebug() << "ProcessStep::cleanSWP(), end for spkgRef_:" << spkgRef_.c_str();.
// }.

}  // namespace vpkgmgr
}  // namespace ucm
}  // namespace ara
