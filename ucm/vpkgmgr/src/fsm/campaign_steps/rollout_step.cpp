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
/// @file       rollout_step.cpp
/// @brief      RolloutStep impl
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
/// @unit_name=RolloutStep
/// @unit_description=RolloutStep impl
/// @endcode
///
/// ================================================================

#include "fsm/campaign_steps/rollout_step.h"

#include <isoft/manifestreader/manifestreader_error_domain.h>

#include <thread>

#include "fsm/campaign_steps/ucm_step.h"
#include "utils/ucmm_log.h"

namespace ara {
namespace ucm {
namespace vpkgmgr {

/// @brief Load vehicle package upgrade/update step information and generate corresponding RollStep, UcmStep, SwpStep operation step instances
/// @param rolloutInfo Vehicle package upgrade/update step information
/// @return 0 on success, non-zero on failure
/// @throws no
int32_t RolloutStep::Load(RolloutStepInfoPtr const& rolloutInfo)
{
    shortName_    = rolloutInfo->shortName;
    safetyPolicy_ = rolloutInfo->safetyPolicy;

    for (ara::core::Vector< std::shared_ptr< UcmStepInfo > >::iterator iter{rolloutInfo->ucmInfos.begin()};
         iter != rolloutInfo->ucmInfos.end(); iter++) {
        UcmStep::Ptr ucmStepPtr{std::make_shared< UcmStep >()};
        ucmStepPtr->Load(*iter);
        ucmSteps_.push_back(ucmStepPtr);
    }
    return 0;
}

/// @brief Execute, including transferring software package to UCM and installing the update
/// @return Returns void or a specific error code
/// @throws no
ara::core::Result< void > RolloutStep::Execute()
{
    log_.LogDebug() << "RolloutStep::Execute(), begin for shortName_:" << shortName_.c_str();

    // Return result
    ara::core::Result< void > res{};
    ara::core::Vector< std::thread > threadVector{};
    ara::core::Vector< ara::core::Future< void > > futureVector{};

    log_.LogDebug() << "RolloutStep::Execute(), try to execute UcmStep concurrently with ucmSteps_.size():"
                    << ucmSteps_.size();
    for (ara::core::Vector< std::shared_ptr< UcmStep > >::iterator iter{ucmSteps_.begin()}; iter != ucmSteps_.end();
         iter++) {
        ara::core::Promise< void > promise;
        futureVector.push_back(promise.get_future());
        // initialise our vector of threads and launch them this will launch 1 thread per UCM
        log_.LogDebug() << "RolloutStep::Execute(), try to create UcmThread for UcmStep:"
                        << (*iter)->GetShortName().c_str();
        threadVector.push_back(std::thread(&UcmStep::UcmThread, (*iter), std::move(promise)));
    }

    // wait for all promises to be fulfilled
    log_.LogDebug() << "RolloutStep::Execute(), try to GetResult for futureVector.size():" << futureVector.size();
    for (ara::core::Vector< ara::core::Future< void > >::iterator iter{futureVector.begin()};
         iter != futureVector.end(); iter++) {
        ara::core::Result< void > const asyncRes{iter->GetResult()};
        if (!asyncRes.HasValue()) {  // An ucmStep execution failed
            res = ara::core::Result< void >::FromError(pkgmgr::UCMErrorDomainErrc::kGeneralReject);
            break;
        }
    }

    log_.LogDebug() << "RolloutStep::Execute(), try to join for threadVector.size():" << threadVector.size();
    // sync on all threads and return
    for (ara::core::Vector< std::thread >::iterator tditer{threadVector.begin()}; tditer != threadVector.end();
         tditer++) {
        tditer->join();
    }

    if (res) {
        log_.LogDebug() << "RolloutStep::Execute(), end with success for shortName_:" << shortName_.c_str();
    } else {
        log_.LogError() << "RolloutStep::Execute(), end with error:" << res.Error().Message().data()
                        << "for shortName_:" << shortName_.c_str();
    }
    return res;
}

/// @brief Perform activation step
/// @return Returns void or a specific error code
/// @throws no
ara::core::Result< void > RolloutStep::Activate()
{
    log_.LogDebug() << "RolloutStep::Activate(), begin for shortName_:" << shortName_.c_str();
    ara::core::Result< void > res{};
    ara::core::Vector< std::thread > threadVector{};
    ara::core::Vector< ara::core::Future< void > > futureVector{};

    log_.LogDebug() << "RolloutStep::Activate(), try to Activate UcmStep concurrently with ucmSteps_.size():"
                    << ucmSteps_.size();
    for (ara::core::Vector< std::shared_ptr< UcmStep > >::iterator iter{ucmSteps_.begin()}; iter != ucmSteps_.end();
         iter++) {
        ara::core::Promise< void > promise;
        futureVector.push_back(promise.get_future());
        // initialise our vector of threads and launch them this will launch 1 thread per UCM
        log_.LogDebug() << "RolloutStep::Activate(), try to create UcmThread for UcmStep:"
                        << (*iter)->GetShortName().c_str();
        threadVector.push_back(std::thread(&UcmStep::Activate, (*iter), std::move(promise)));
    }

    // wait for all promises to be fulfilled
    log_.LogDebug() << "RolloutStep::Activate(), try to GetResult for futureVector.size():" << futureVector.size();
    for (ara::core::Vector< ara::core::Future< void > >::iterator iter{futureVector.begin()};
         iter != futureVector.end(); iter++) {
        ara::core::Result< void > const asyncRes{iter->GetResult()};
        if (!asyncRes.HasValue()) {
            res = ara::core::Result< void >::FromError(pkgmgr::UCMErrorDomainErrc::kGeneralReject);
            break;
        }
    }
    /// LOG_INFO << "All UCM steps of" << shortName_.c_str() << "Activate succesfully";

    // sync on all threads and return
    log_.LogDebug() << "RolloutStep::Activate(), try to join for threadVector.size():" << threadVector.size();
    for (ara::core::Vector< std::thread >::iterator tditer{threadVector.begin()}; tditer != threadVector.end();
         tditer++) {
        tditer->join();
    }
    if (res) {
        log_.LogDebug() << "RolloutStep::Activate(), end with success for shortName_:" << shortName_.c_str();
    } else {
        log_.LogDebug() << "RolloutStep::Activate(), end with message:" << res.Error().Message().data()
                        << "for shortName_:" << shortName_.c_str();
    }
    return res;
}

}  // namespace vpkgmgr
}  // namespace ucm
}  // namespace ara
