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
/// @file       swpkg_step.cpp
/// @brief      SwpStep impl
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
/// @unit_name=SwpStep
/// @unit_description=SwpStep impl
/// @endcode
///
/// ================================================================

#include "fsm/campaign_steps/swpkg_step.h"

#include <ara/ucm/pkgmgr/error_domain_ucmerrordomain.h>

#include <chrono>
#include <thread>

#include "fsm/fsm_manager.h"
#include "fsm/storage/history_database.h"

namespace ara {
namespace ucm {
namespace vpkgmgr {

/// @brief Load swpStep configuration information
/// @param swpInfo
/// @throws no
void SwpStep::Load(SwpStepInfoPtr const& swpInfo)
{
    shortName_ = swpInfo->shortName;
    ucmId_     = swpInfo->ucmId;
    ///safetyPolicy_ = swpInfo->safetyPolicy;

    // transfer
    FsmManager* const fsm{FsmManager::GetInstance()};
    // if (nullptr != swpInfo->transferStepPtr.get()) {
    //     ara::core::Vector< TransferStepInfoPtr >& transferInfos{swpInfo->transferStepPtr->transferInfos};
    //     for (ara::core::Vector< TransferStepInfoPtr >::iterator iter{transferInfos.begin()};
    //          iter != transferInfos.end(); iter++) {
    //         TransferStep::Ptr transferStepPtr{std::make_shared< TransferStep >()};
    //         transferStepPtr->Load(*iter, ucmId_, swpInfo->transferStepPtr->storing);
    //         transferStepVector_.push_back(transferStepPtr);
    //         fsm->TransferAdd();
    //     }
    // }
    for (auto const& itTransferInfo : swpInfo->transferStepPtrList) {
        for (auto const& it : itTransferInfo->transferInfos) {
            TransferStep::Ptr transferStepPtr{std::make_shared< TransferStep >()};
            transferStepPtr->Load(it, ucmId_, itTransferInfo->storing);
            transferStepVector_.push_back(transferStepPtr);
            fsm->TransferAdd();
        }
    }

    // process
    if ((nullptr != swpInfo->processStepPtr.get())) {
        if (!swpInfo->processStepPtr->softwarePackageRef.empty()) {
            processStepPtr_ = std::make_shared< ProcessStep >();
            processStepPtr_->Load(swpInfo->processStepPtr);
            fsm->ProcessAdd();
        }
    }

    // // activate
    // if (swpInfo->activateStep) {
    //     activationStepPtr_ = std::make_shared< ActivationStep >();
    // }

    return;
}

/// @brief Install/Upgrade
/// @param swclNameToTid
/// @return Returns void on success, corresponding error code on failure
/// @throws no
ara::core::Result< void > SwpStep::Execute(
    ara::core::Map< ara::core::String, std::pair< bool, pkgmgr::TransferIdType > >& swclNameToTid)
{
    log_.LogInfo() << "SwpStep::Execute(), begin with ucmId_:" << ucmId_.c_str()
                   << "shortName_: " << shortName_.c_str();

    // Return result
    ara::core::Result< void > res{};
    ara::core::Vector< std::thread > threads{};
    ara::core::Vector< ara::core::Future< void > > futures{};

    log_.LogDebug() << "SwpStep::Execute(), try to execute TransferStep concurrently with transferStepVector_.size():"
                    << transferStepVector_.size();
    // Changed to multi-threading (including processStepPtr_ below): each thread has its own error return; this function returns only after all threads finish (to provide the function return value); if a NonRecoverableFailure occurs in any transfer or processing, Cancel is triggered.
    for (ara::core::Vector< TransferStep::Ptr >::iterator iter{transferStepVector_.begin()};
         iter != transferStepVector_.end(); iter++) {
        TransferStep::Ptr tranferStep{(*iter)};
        ara::core::Promise< void > promise;
        futures.push_back(promise.get_future());

        log_.LogDebug() << "SwpStep::Execute(), try to create TransferThread for tranferStep->GetSpkgRef():"
                        << tranferStep->GetSpkgRef().c_str();
        threads.push_back(
            std::thread(&TransferStep::TransferThread, (*iter), ucmId_, &swclNameToTid, std::move(promise)));
    }

    // Process software packages
    if (nullptr != processStepPtr_.get()) {
        ara::core::Promise< void > promise{};
        futures.push_back(promise.get_future());
        log_.LogDebug() << "SwpStep::Execute(), try to create ProcessThread for processStepPtr_->GetSpkgRef():"
                        << processStepPtr_->GetSpkgRef().c_str();
        threads.push_back(
            std::thread(&ProcessStep::ProcessThread, processStepPtr_, ucmId_, &swclNameToTid, std::move(promise)));
    }

    log_.LogDebug() << "SwpStep::Execute(), try to GetResult for futures.size():" << futures.size();
    // wait for all promises to be fulfilled
    for (ara::core::Future< void >& it : futures) {
        ara::core::Result< void > asyncRes{it.GetResult()};
        if (!asyncRes.HasValue()) {  // Execution failed
            res = std::move(asyncRes);
            break;
        }
    }

    log_.LogDebug() << "SwpStep::Execute(), try to join for threads.size():" << threads.size();
    // sync on all threads and return
    for (ara::core::Vector< std::thread >::iterator tditer{threads.begin()}; tditer != threads.end(); tditer++) {
        tditer->join();
    }

    if (res) {
        log_.LogDebug() << "SwpStep::Execute(), end with success for shortName_:" << shortName_.c_str()
                        << "ucmId_:" << ucmId_.c_str();
    } else {
        log_.LogError() << "SwpStep::Execute(), end with error:" << res.Error().Message().data()
                        << "for shortName_:" << shortName_.c_str() << "ucmId_:" << ucmId_.c_str();
    }
    return res;
}

// /// @brief Activate
// /// @param ucmId
// /// @return Returns void on success, corresponding error code on failure
// /// @throws no
// ara::core::Result< void > SwpStep::Activate(ara::core::String const& ucmId)
// {
//     log_.LogDebug() << "SwpStep::Activate(), begin for ucmId_:" << ucmId_.c_str()
//                     << "shortName_: " << shortName_.c_str();

//     /// Once activation is called, you can only wait for the activation result (success or failure)
//     if (nullptr != activationStepPtr_.get()) {
//         log_.LogDebug() << "SwpStep::Activate(), try to activationStepPtr_->Execute for ucmId:" << ucmId_.c_str()
//                         << "shortName_: " << shortName_.c_str();
//         ara::core::Result< void > const result{activationStepPtr_->Execute(ucmId)};
//         if (!result.HasValue()) {
//             log_.LogDebug() << "SwpStep::Activate(), haven't finished to activationStepPtr_->Execute for ucmId:"
//                             << ucmId_.c_str() << "SwpStep: " << shortName_.c_str()
//                             << "with error:" << result.Error().Message().data();
//             return result;
//         }

//         log_.LogDebug() << "UcmStep::Activate(), end with suceess for ucmId:" << ucmId_.c_str()
//                         << "UcmStep: " << shortName_.c_str();
//         return result;
//     }
//     log_.LogDebug() << "UcmStep::Activate(), nothing to Execute for ucmId:" << ucmId_.c_str()
//                     << "UcmStep: " << shortName_.c_str();
//     return ara::core::Result< void >{};
// }

}  // namespace vpkgmgr
}  // namespace ucm
}  // namespace ara
