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
/// @file       swp_step_transfer.cpp
/// @brief      TransferStep impl
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
/// @unit_name=TransferStep
/// @unit_description=TransferStep impl
/// @endcode
///
/// ================================================================

#include "fsm/campaign_steps/swp_step_transfer.h"

#include <ara/core/result.h>
#include <ara/core/string.h>
#include <ara/log/logger.h>

#include <chrono>
#include <condition_variable>
#include <memory>

#include "ara/ucm/pkgmgr/error_domain_ucmerrordomain.h"
#include "fsm/fsm_manager.h"
#include "fsm/package_transfer/software_package_manager.h"
#include "fsm/storage/history_database.h"
#include "package_management_service/find_ucm_subs.h"
#include "utils/ucmm_log.h"

namespace ara {
namespace ucm {
namespace vpkgmgr {

/// @brief Load configuration
/// @param transferInfo Configuration related to transferring the software package
/// @param ucmId
/// @param storing
/// @throws no
void TransferStep::Load(TransferStepInfoPtr const& transferInfo,
                        ara::core::String const& ucmId,
                        SoftwarePackageStoringEnum const storing)
{
    this->storing_ = storing;
    spkgRef_       = transferInfo->softwarePackageRef;  // fqn
    std::ignore    = ucmId;

    ///FindUcmSubs* const subs{FindUcmSubs::GetInstance()};
    SwPackageInfoPtr spkgPtr{FsmManager::GetInstance()->GetSpkgByName(spkgRef_)};
    if (nullptr == spkgPtr.get()) {
        LOG_ERROR << "";
        return;
    }
    ///if (spkgPtr->isReboot) {
    ///    subs->SetUcmIsReboot(ucmId, true); // ucmId is used here   --- indeed used
    ///}
    return;
}

/// @brief Execute TransferThread in a separate thread
/// @param ucmId
/// @param swclNameToTid
/// @param promise
/// @throws no
void TransferStep::TransferThread(
    ara::core::String const& ucmId,
    ara::core::Map< ara::core::String, std::pair< bool, pkgmgr::TransferIdType > >* swclNameToTid,
    ara::core::Promise< void >&& promise)
{
    log_.LogDebug() << "TransferStep::TransferThread(), begin with spkgRef_:" << spkgRef_.c_str()
                    << "ucmId:" << ucmId.c_str();

    // TODO(retry) Retry software package transfer
    // Set the value of swclNameToTid here; processStepPtr_->Execute will use this value
    ara::core::Result< void > const result{_execute(ucmId, *swclNameToTid)};
    if (!result.HasValue()) {  // Execution failed
        log_.LogError() << "TransferStep::TransferThread(), failed to _execute for spkgRef_:" << spkgRef_.c_str()
                        << "ucmId:" << ucmId.c_str();
    } else {  // Execution succeeded
        log_.LogDebug() << "TransferStep::TransferThread(), succeed to _execute for spkgRef_:" << spkgRef_.c_str()
                        << "ucmId:" << ucmId.c_str();
        FsmManager::GetInstance()->TransferredAdd();
    }

    promise.SetResult(result);
    log_.LogDebug() << "TransferStep::TransferThread(), end for spkgRef_:" << spkgRef_.c_str()
                    << "ucmId:" << ucmId.c_str();
}

/// @brief _execute the Transfer step, i.e. trigger UCM Subordinate to transfer the SwPackage identified by the
/// current Transfer ID
///
/// @param ucmId The ID of the UCM subordinate that shall compute the action
/// @param swclNameToTid Map that link a software cluster name to a Transfer ID
/// @return result
/// @throws no
ara::core::Result< void > TransferStep::_execute(
    ara::core::String const& ucmId,
    ara::core::Map< ara::core::String, std::pair< bool, pkgmgr::TransferIdType > >& swclNameToTid)
{
    log_.LogDebug() << "TransferStep::_execute(), begin with spkgRef_:" << spkgRef_.c_str()
                    << "ucmId:" << ucmId.c_str();

    FindUcmSubs* const subs{FindUcmSubs::GetInstance()};

    PackageManagementAppPtr pmAppPtr{subs->GetPackageManagementAppById(ucmId)};
    if (nullptr == pmAppPtr.get()) {
        log_.LogError() << "TransferStep::_execute(), can't get GetPackageManagementAppById for spkgRef_:"
                        << spkgRef_.c_str() << "ucmId:" << ucmId.c_str() << ", so will TriggerCancel.";

        // Cancel Campaign
        FsmManager::GetInstance()->TriggerCancelInTQ(
            pkgmgr::UCMMasterResolutionType::kSubordinateNotAvailableOnTheNetwork);
        return ara::core::Result< void >::FromError(pkgmgr::UCMErrorDomainErrc::kGeneralReject);
    }

    // the only condition to transfer a SwPackage is that the UCM subordinate is running ([SWS_UCM_00007])
    ///ara::core::String packagePath = "/usr/share/apdtest/ucm/vpm/data/" + spkgRef_ + ".zip";
    SwPackageInfoPtr swPackageInfoPtr{nullptr};
    while (true) {
        if (!FsmManager::GetInstance()->CanMoveForward()) {
            log_.LogError() << "TransferStep::_execute(), failed to CanMoveForward for spkgRef_:" << spkgRef_.c_str()
                            << "ucmId:" << ucmId.c_str() << ", so will return.";
            return ara::core::Result< void >::FromError(pkgmgr::UCMErrorDomainErrc::kGeneralReject);
        }
        swPackageInfoPtr = FsmManager::GetInstance()->GetTransferdSpkgByName(spkgRef_);
        if (nullptr != swPackageInfoPtr.get()) {
            break;
        }
        log_.LogVerbose() << "TransferStep::_execute(), spkgRef_" << spkgRef_.c_str() << "has not cached yet.";

        // If the connection to the UCM subordinate is lost, the com component manages retries (findservice needs to run continuously)
        if (!pmAppPtr->UpdateStatus()) {
            log_.LogError() << "TransferStep::_execute(), failed to UpdateStatus for spkgRef_:" << spkgRef_.c_str()
                            << "ucmId:" << ucmId.c_str() << ", so will TriggerCancel.";

            // Cancel Campaign
            FsmManager::GetInstance()->TriggerCancelInTQ(
                pkgmgr::UCMMasterResolutionType::kSubordinateNotAvailableOnTheNetwork);
            return ara::core::Result< void >::FromError(pkgmgr::UCMErrorDomainErrc::kGeneralReject);
        }
        std::this_thread::sleep_for(
            std::chrono::milliseconds(kGlobalWaitTime));  // Wait for software package transfer to complete.
    }
    /// FIXME(yunfei) storing is later saved in ucm, vucm
    swPackageInfoPtr->storing = storing_;
    log_.LogDebug()
        << "TransferStep::_execute(), try to TransferSoftwarePackage with swPackageInfoPtr->temporaryStorePath:"
        << swPackageInfoPtr->temporaryStorePath << "for spkgRef_:" << spkgRef_.c_str();
    ara::core::Result< pkgmgr::TransferIdType > const result{
        pmAppPtr->TransferSoftwarePackage(swPackageInfoPtr->temporaryStorePath)};
    if (!result.HasValue()) {
        std::ignore = swclNameToTid.emplace(spkgRef_, std::make_pair(false, pkgmgr::TransferIdType{}));
        log_.LogError()
            << "TransferStep::_execute(), failed to TransferSoftwarePackage for swPackageInfoPtr->temporaryStorePath:"
            << swPackageInfoPtr->temporaryStorePath.c_str() << "for spkgRef_:" << spkgRef_.c_str()
            << ", so will TriggerCancel.";

        pkgmgr::UCMErrorDomainErrc errCode{static_cast< pkgmgr::UCMErrorDomainErrc >(result.Error().Value())};
        pkgmgr::UCMStepErrorType const ucmStepError{ucmId, pkgmgr::SoftwarePackageStepType::kTransfer,
                                                    static_cast< uint8_t >(errCode)};
        std::ignore = FsmManager::GetInstance()->SaveUcmStepError(ucmStepError);

        // Cancel Campaign
        FsmManager::GetInstance()->TriggerCancelInTQ(pkgmgr::UCMMasterResolutionType::kUCMError);
        return ara::core::Result< void >::FromError(errCode);
    }

    log_.LogDebug()
        << "TransferStep::_execute(), succeeded to TransferSoftwarePackage for swPackageInfoPtr->temporaryStorePath:"
        << swPackageInfoPtr->temporaryStorePath.c_str() << "for spkgRef_:" << spkgRef_.c_str();

    // update below UcmStep member so that process action can use it later on
    pkgmgr::TransferIdType transferId{result.Value()};

    // Save ucm transfer information
    HistoryDatabase::GetInstance()->AddUCMTransferInfo(ucmId, transferId);

    /// @brief iterType
    // Define alias
    using IterType = ara::core::Map< ara::core::String, std::pair< bool, pkgmgr::TransferIdType > >::iterator;
    std::pair< IterType, bool > const pair{swclNameToTid.emplace(spkgRef_, std::make_pair(true, transferId))};
    assert(pair.second);

    log_.LogDebug() << "TransferStep::_execute(), end with success for spkgRef_:" << spkgRef_.c_str()
                    << "ucmId:" << ucmId.c_str();
    return ara::core::Result< void >::FromValue();
}

}  // namespace vpkgmgr
}  // namespace ucm
}  // namespace ara
