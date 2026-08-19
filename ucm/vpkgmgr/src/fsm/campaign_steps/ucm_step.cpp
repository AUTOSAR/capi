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
/// @file       ucm_step.cpp
/// @brief      UcmStep implementation
/// @details
/// @date       2022-02-18
/// @author     hanzhibo
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/UCM Master/Fsm
/// @interface_level=unit
/// @trace_id_sr=SR_UCM_00035
/// @unit_name=UcmStep
/// @unit_description=UcmStep implementation
/// @endcode
///
/// ================================================================

#include "ucm_step.h"

#include <memory>
#include <sstream>
#include <thread>

#include "ara/ucm/pkgmgr/error_domain_ucmerrordomain.h"
#include "services/concentrator.h"
#include "utils/ucmm_log.h"

namespace ara {
namespace ucm {
namespace vpkgmgr {

/// @brief Load configuration
/// @param ucmInfo Configuration information for the ucm step
/// @throws no
void UcmStep::Load(UcmStepInfoPtr const& ucmInfo)
{
    shortName_ = ucmInfo->shortName;
    ucmId_     = ucmInfo->ucmId;
    /// safetyPolicy_ = ucmInfo->safetyPolicy;  // Seems useless

    for (ara::core::Vector< SwpStepInfoPtr >::iterator iter{ucmInfo->swpInfos.begin()}; iter != ucmInfo->swpInfos.end();
         iter++) {
        SwpStep::Ptr swpStepPtr{std::make_shared< SwpStep >()};
        swpStepPtr->Load(*iter);
        swpSteps_.push_back(swpStepPtr);
    }

    // If ucmInfo->swpInfos contains ProcessStep, add activation step
    for (auto const& it : ucmInfo->swpInfos) {
        if (it->processStepPtr) {
            activationStepPtr_ = std::make_shared< ActivationStep >();
            break;
        }
    }

    return;
}

/// @brief Execute UcmStep in a separate thread
/// @param promise
/// @throws no
void UcmStep::UcmThread(ara::core::Promise< void >&& promise)
{
    log_.LogDebug() << "UcmStep::UcmThread(), begin with shortName_:" << shortName_.c_str()
                    << "ucmId_:" << ucmId_.c_str();

    // All swpSteps share a cache, one per UCM
    ara::core::Map< ara::core::String, std::pair< bool, pkgmgr::TransferIdType > > swclNameToTid{};

    log_.LogDebug() << "UcmStep::UcmThread(), try to execute SwpStep sequentially with swpSteps_.size():"
                    << swpSteps_.size();
    for (ara::core::Vector< SwpStep::Ptr >::iterator iter{swpSteps_.begin()}; iter != swpSteps_.end(); iter++) {
        SwpStep::Ptr& swpStep{(*iter)};
        log_.LogDebug() << "UcmStep::UcmThread(), try to swpStep->Execute for swpStep->GetShortName():"
                        << swpStep->GetShortName().data();
        ara::core::Result< void > const result{swpStep->Execute(swclNameToTid)};
        if (result) {
            log_.LogDebug() << "UcmStep::UcmThread(), succeeded to swpStep->Execute for swpStep->GetShortName():"
                            << swpStep->GetShortName().data();
        } else {
            log_.LogError() << "UcmStep::UcmThread(), failed to swpStep->Execute with error:"
                            << result.Error().Message().data()
                            << "for swpStep->GetShortName():" << swpStep->GetShortName().data() << ",so will return.";
            promise.SetError(result.Error());
            return;
        }
    }

    log_.LogDebug() << "UcmStep::UcmThread(), end with success for shortName_:" << shortName_.c_str()
                    << "ucmId_:" << ucmId_.c_str();
    promise.set_value();
    return;
}

/// @brief Execute activation step
/// @param promise
/// @throws no
void UcmStep::Activate(ara::core::Promise< void >&& promise)
{
    log_.LogDebug() << "UcmStep::Activate(), begin for ucmId:" << ucmId_.c_str() << "UcmStep: " << shortName_.c_str();

    /// Once activation is called, you can only wait for the activation result (success or failure)
    if (nullptr != activationStepPtr_.get()) {
        log_.LogDebug() << "UcmStep::Activate(), try to activationStepPtr_->Execute for ucmId:" << ucmId_.c_str()
                        << "shortName_: " << shortName_.c_str();
        ara::core::Result< void > const result{activationStepPtr_->Execute(ucmId_)};
        if (!result.HasValue()) {
            log_.LogDebug() << "UcmStep::Activate(), failed to activationStepPtr_->Execute for ucmId:" << ucmId_.c_str()
                            << "UcmStep: " << shortName_.c_str() << "with error:" << result.Error().Message().data();
            promise.SetResult(result);
            return;
        }

        log_.LogDebug() << "UcmStep::Activate(), end with suceess for ucmId:" << ucmId_.c_str()
                        << "UcmStep: " << shortName_.c_str();

        promise.SetResult(result);
        return;
    }
    log_.LogDebug() << "UcmStep::Activate(), nothing to Execute for ucmId:" << ucmId_.c_str()
                    << "UcmStep: " << shortName_.c_str();

    promise.SetResult(ara::core::Result< void >{});
}

}  // namespace vpkgmgr
}  // namespace ucm
}  // namespace ara